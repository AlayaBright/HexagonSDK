/* ==================================================================================== */
/*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
/*                           All Rights Reserved.                                       */
/*                  QUALCOMM Confidential and Proprietary                               */
/* ==================================================================================== */

#include "ion_allocation.h"

bool use_libdmabuf = false;
bool use_newer_ioctl = false;
bool use_libion = false;

namespace {

// DMA-BUF support
void *dmabufAllocator = nullptr;

extern "C" {
     const char *dmabuf_heap = "qcom,system";

     __attribute__((weak)) extern void *CreateDmabufHeapBufferAllocator();
     typedef void * (*rem_dmabuf_create_fn)();
     rem_dmabuf_create_fn dmabuf_create_fn = nullptr;

     __attribute__((weak)) extern int DmabufHeapAlloc(void *buffer_allocator, const char *heap_name, size_t len, unsigned int heap_flags, size_t legacy_align);
     typedef int (*rem_dmabuf_alloc_fn)(void *, const char *, size_t, unsigned int, size_t);
     rem_dmabuf_alloc_fn dmabuf_alloc_fn = nullptr;

     __attribute__((weak)) extern void FreeDmabufHeapBufferAllocator(void *buffer_allocator);
     typedef void (*rem_dmabuf_deinit_fn)(void *);
     rem_dmabuf_deinit_fn dmabuf_deinit_fn = nullptr;
}

// ION support

// Allocations that are intended to be shared with Hexagon can be
// shared without copying if they are contiguous in physical
// memory. Android's ION allocator gives us a mechanism with which we
// can allocate contiguous physical memory.

enum ion_heap_id {
    system_heap_id = 25,
};

enum ion_flags {
    ion_flag_cached = 1,
};

typedef int ion_user_handle_t;

struct ion_allocation_data {
    size_t len;
    size_t align;
    unsigned int heap_id_mask;
    unsigned int flags;
    ion_user_handle_t handle;
};

struct ion_allocation_data_newer {
    uint64_t len;
    uint32_t heap_id_mask;
    uint32_t flags;
    uint32_t fd;
    uint32_t unused;
};

struct ion_fd_data {
    ion_user_handle_t handle;
    int fd;
};

struct ion_handle_data {
    ion_user_handle_t handle;
};

#define ION_IOC_ALLOC _IOWR('I', 0, ion_allocation_data)
#define ION_IOC_ALLOC_NEWER _IOWR('I', 0, ion_allocation_data_newer)
#define ION_IOC_FREE _IOWR('I', 1, ion_handle_data)
#define ION_IOC_MAP _IOWR('I', 2, ion_fd_data)

extern "C" {
     __attribute__((weak)) extern int ion_open();
     typedef int (*rem_ion_open_fn)();
     rem_ion_open_fn ion_open_fn = nullptr;

     __attribute__((weak)) extern int ion_alloc_fd(int ion_fd, size_t len, size_t align, unsigned int heap_id_mask, unsigned int flags, int *map_fd);
     typedef int (*rem_ion_alloc_fd_fn)(int ion_fd, size_t len, size_t align, unsigned int heap_id_mask, unsigned int flags, int *map_fd);
     rem_ion_alloc_fd_fn ion_alloc_fd_fn = nullptr;
}
// ION IOCTL approach
// This function will first try older IOCTL approach provided we have not determined
// that we should use the newer IOCTL. Once we have determined we should use the
// newer IOCTL method , we cease calling the older IOCTL.

ion_user_handle_t ion_alloc(int ion_fd, size_t len, size_t align, unsigned int heap_id_mask, unsigned int flags) {

    if (use_libion) {
        int map_fd = 0;
        __android_log_print(ANDROID_LOG_INFO, "halide_test", "host_malloc: Using_libion");

        if (ion_alloc_fd_fn(ion_fd, len, 0, heap_id_mask, flags, &map_fd)) {
            __android_log_print(ANDROID_LOG_ERROR, "halide_test", "ion_alloc: ion_alloc failed");
            return -1;
        }
        __android_log_print(ANDROID_LOG_INFO, "halide_test", "ion_alloc: ion_alloc succeeded");

        return map_fd;
    }

    if (!use_newer_ioctl) {
        ion_allocation_data alloc = {
            len,
            align,
            heap_id_mask,
            flags,
            0};
        if (ioctl(ion_fd, ION_IOC_ALLOC, &alloc) >= 0) {
            return alloc.handle;
        }
    }

    // Lets try newer ioctl API
    ion_allocation_data_newer alloc_newer = {
        len,
        heap_id_mask,
        flags,
        0,
        0};
    if (ioctl(ion_fd, ION_IOC_ALLOC_NEWER, &alloc_newer) >= 0) {
        use_newer_ioctl = true;
        return alloc_newer.fd;
    }
    // Abject failure
    __android_log_print(ANDROID_LOG_INFO, "halide_test", "ion_alloc: Abject failure\n");
    return -1;
}

int ion_map(int ion_fd, ion_user_handle_t handle) {
    if (use_libion) {
        return 0;
    }

    // old ioctl approach
    ion_fd_data data = {
        handle,
        0};
    if (ioctl(ion_fd, ION_IOC_MAP, &data) < 0) {
        return -1;
    }
    return data.fd;
}

int ion_free(int ion_fd, ion_user_handle_t ion_handle) {
    if (!use_libion) {
        // We need to use this approach only if we are not using
        // libion. If we are using libion (use_libion == true),
        // then simply closing the fd (buf_fd) is enough. See
        // alloc_ion_free below
        if (ioctl(ion_fd, ION_IOC_FREE, &ion_handle) < 0) {
            return -1;
        }
    }
    return 0;
}

// We need to be able to keep track of the size and some other
// information about ION allocations, so define a simple linked list
// of allocations we can traverse later.
struct allocation_record {
    allocation_record *next;
    ion_user_handle_t handle;
    int buf_fd;
    void *buf;
    size_t size;
};

// Make a dummy allocation so we don't need a special case for the
// head list node.
allocation_record allocations = {
    nullptr,
};
pthread_mutex_t allocations_mutex = PTHREAD_MUTEX_INITIALIZER;

int ion_fd = -1;

}  // namespace

extern "C" {

// If this symbol is defined in the stub library we are going to link
// to, we need to call this in order to actually get zero copy
// behavior from our buffers.
__attribute__((weak)) void remote_register_buf(void *buf, int size, int fd);

}  // extern "C"
void alloc_init() {
    if (ion_fd != -1) {
        return;
    }
    if (dmabufAllocator != nullptr) {
        return;
    }

    __android_log_print(ANDROID_LOG_INFO, "halide_test", "alloc_init: In alloc_init");
    pthread_mutex_init(&allocations_mutex, nullptr);
    use_libdmabuf = false;
    use_newer_ioctl = false;
    use_libion = false;
    void* lib = nullptr;

    // Try to access libdmabufheap.so, if it succeeds use DMA-BUF
    lib = dlopen("libdmabufheap.so", RTLD_LAZY);
    if (lib) {
        use_libdmabuf = true;
        dmabuf_create_fn = (rem_dmabuf_create_fn) dlsym(lib, "CreateDmabufHeapBufferAllocator");
        dmabuf_deinit_fn = (rem_dmabuf_deinit_fn) dlsym(lib, "FreeDmabufHeapBufferAllocator");
        dmabuf_alloc_fn  = (rem_dmabuf_alloc_fn)  dlsym(lib, "DmabufHeapAlloc");
        if (!dmabuf_create_fn || !dmabuf_deinit_fn || !dmabuf_alloc_fn) {
            __android_log_print(ANDROID_LOG_ERROR, "halide", "huge problem in libdmabufheap.so");
            return;
        }

        dmabufAllocator = dmabuf_create_fn();
        if (!dmabufAllocator) {
            __android_log_print(ANDROID_LOG_ERROR, "halide", "dmabuf init failed");
            return;
        }
        __android_log_print(ANDROID_LOG_INFO, "halide_test",
                            "alloc_init: using dmabuf. Finished.");
        return;
    }

    // Try to access libion.so, if it succeeds use new approach
    lib = dlopen("libion.so", RTLD_LAZY);
    if (lib) {
        use_libion = true;
        ion_open_fn = (rem_ion_open_fn) dlsym(lib, "ion_open");
        ion_alloc_fd_fn = (rem_ion_alloc_fd_fn) dlsym(lib, "ion_alloc_fd");
        __android_log_print(ANDROID_LOG_INFO, "halide_test", "alloc_init: libion exists. use_libion=true");

        if (!ion_open_fn || !ion_alloc_fd_fn) {
            __android_log_print(ANDROID_LOG_ERROR, "halide_test", "alloc_init: huge problem in libion.so");
            return;
        }
        ion_fd = ion_open_fn();
        if (ion_fd < 0) {
            __android_log_print(ANDROID_LOG_ERROR, "halide_test", "alloc_init: ion_open failed");
        }
        __android_log_print(ANDROID_LOG_INFO, "halide_test",
                            "alloc_init: using libion. Finished.");
    } else {
        ion_fd = open("/dev/ion", O_RDONLY, 0);
        if (ion_fd < 0) {
            __android_log_print(ANDROID_LOG_ERROR, "halide_test", "alloc_init: open('/dev/ion') failed");
        } else {
            __android_log_print(ANDROID_LOG_INFO, "halide_test",
                                "alloc_init: Not using libion. Finished.");
        }
    }
}

void alloc_finalize() {
    if (use_libdmabuf) {
        if (dmabufAllocator == nullptr) {
            return;
        }
        __android_log_print(ANDROID_LOG_INFO, "halide", "entering halide_hexagon_host_malloc_deinit dmabuf");
        dmabuf_deinit_fn(dmabufAllocator);
        dmabufAllocator = nullptr;
    } else {
        if (ion_fd == -1) {
            return;
        }
        __android_log_print(ANDROID_LOG_INFO, "halide", "entering halide_hexagon_host_malloc_deinit ion");
        close(ion_fd);
        ion_fd = -1;
    }
    pthread_mutex_destroy(&allocations_mutex);
}

void *alloc_ion(size_t size) {
    const int heap_id = system_heap_id;
    const int ion_flags = ion_flag_cached;

    // Hexagon can only access a small number of mappings of these
    // sizes. We reduce the number of mappings required by aligning
    // large allocations to these sizes.
    static const size_t alignments[] = {0x1000, 0x4000, 0x10000, 0x40000, 0x100000};
    size_t alignment = alignments[0];

    // Align the size up to the minimum alignment.
    size = (size + alignment - 1) & ~(alignment - 1);

    if (heap_id != system_heap_id) {
        for (size_t i = 0; i < sizeof(alignments) / sizeof(alignments[0]); i++) {
            if (size >= alignments[i]) {
                alignment = alignments[i];
            }
        }
    }

    int buf_fd = 0;
    ion_user_handle_t handle = 0;

    if (use_libdmabuf) {
        buf_fd = dmabuf_alloc_fn(dmabufAllocator, dmabuf_heap, size, 0, 0);
        if (buf_fd < 0) {
            __android_log_print(ANDROID_LOG_ERROR, "halide_test", "alloc_ion: DmabufHeapAlloc(%p, \"%s\", %zd, %d, %d) failed",
                                dmabufAllocator, dmabuf_heap, size, 0, 0);
            return nullptr;
        }
    } else if (use_libion) {
        buf_fd = ion_alloc(ion_fd, size, alignment, 1 << heap_id, ion_flags);
        if (buf_fd < 0) {
            __android_log_print(ANDROID_LOG_ERROR, "halide_test", "alloc_ion: ion_alloc(%d, %zd, %zd, %d, %d) failed",
                                ion_fd, size, alignment, 1 << heap_id, ion_flags);
            return nullptr;
        }
    } else { // !use_libion
        handle = ion_alloc(ion_fd, size, alignment, 1 << heap_id, ion_flags);
        if (handle < 0) {
            __android_log_print(ANDROID_LOG_ERROR, "halide_test", "alloc_ion: ion_alloc(%d, %zd, %zd, %d, %d) failed. use_libion=false",
                                ion_fd, size, alignment, 1 << heap_id, ion_flags);
            return nullptr;
        }
        // Map the ion handle to a file buffer.
        if (use_newer_ioctl) {
            buf_fd = handle;
        } else {
            buf_fd = ion_map(ion_fd, handle);
            if (buf_fd < 0) {
                __android_log_print(ANDROID_LOG_ERROR, "halide_test", "alloc_ion: ion_map(%d, %d) failed", ion_fd, handle);
                ion_free(ion_fd, handle);
                return nullptr;
            }
        }
    }

    // Map the file buffer to a pointer.
    void *buf = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, buf_fd, 0);
    if (buf == MAP_FAILED) {
        __android_log_print(ANDROID_LOG_ERROR, "halide_test", "alloc_ion: mmap(nullptr, %lu, PROT_READ | PROT_WRITE, MAP_SHARED, %d, 0) failed",
                            (long long)size, buf_fd);
        close(buf_fd);
        if (!use_newer_ioctl) {
            ion_free(ion_fd, handle);
        }
        return nullptr;
    }

    // Register the buffer, so we get zero copy.
    if (remote_register_buf) {
        remote_register_buf(buf, size, buf_fd);
    }

    // Build a record for this allocation.
    allocation_record *rec = (allocation_record *)malloc(sizeof(allocation_record));
    if (!rec) {
        __android_log_print(ANDROID_LOG_ERROR, "halide_test", "alloc_ion: malloc for allocation record failed");
        munmap(buf, size);
        close(buf_fd);
        if (!use_newer_ioctl) {
            ion_free(ion_fd, handle);
        }
        return nullptr;
    }

    rec->next = nullptr;
    rec->handle = handle;
    rec->buf_fd = buf_fd;
    rec->buf = buf;
    rec->size = size;

    // Insert this record into the list of allocations. Insert it at
    // the front, since it's simpler, and most likely to be freed
    // next.
    pthread_mutex_lock(&allocations_mutex);
    rec->next = allocations.next;
    allocations.next = rec;
    pthread_mutex_unlock(&allocations_mutex);
    __android_log_print(ANDROID_LOG_INFO, "halide_test", "alloc_ion: alloc succeeded");
    return buf;
}

void alloc_ion_free(void *ptr) {
    if (!ptr) {
        return;
    }

    // Find the record for this allocation and remove it from the list.
    pthread_mutex_lock(&allocations_mutex);
    allocation_record *rec = &allocations;
    while (rec) {
        allocation_record *cur = rec;
        rec = rec->next;
        if (rec && rec->buf == ptr) {
            cur->next = rec->next;
            break;
        }
    }
    pthread_mutex_unlock(&allocations_mutex);
    if (!rec) {
        __android_log_print(ANDROID_LOG_WARN, "halide_test", "alloc_ion_free: Allocation not found in allocation records");
        return;
    }

    // Unregister the buffer.
    if (remote_register_buf) {
        remote_register_buf(rec->buf, rec->size, -1);
    }

    // Unmap the memory
    munmap(rec->buf, rec->size);

    // free the ION or DMA-BUF allocation
    close(rec->buf_fd);
    if (!use_libdmabuf && !use_libion && !use_newer_ioctl) {
        // We free rec->handle only if we are not using libion and are
        // also using the older ioctl. See alloc_ion above for more
        // information.
        if (ion_free(ion_fd, rec->handle) < 0) {
            __android_log_print(ANDROID_LOG_WARN, "halide_test", "alloc_ion_free: ion_free(%d, %d) failed", ion_fd, rec->handle);
        }
    }
    free(rec);
}
