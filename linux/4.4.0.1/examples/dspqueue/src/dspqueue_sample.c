/*
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/* Packet queue example CPU-side main program */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <rpcmem.h>
#include <semaphore.h>
#include <inttypes.h>
#include <time.h>
#include <assert.h>
#include <AEEStdErr.h>
#include <unistd.h>
#include "dspqueue.h"
#include "dspqueue_sample.h"
#include "dspqueue_sample_shared.h"
#include "dsp_capabilities_utils.h"     // available under <HEXAGON_SDK_ROOT>/utils/examples
#include "os_defines.h"


#define NUM_ECHOES 400
#define NUM_PROCS 100
#define MAX_BUFFERS 64


static remote_handle64 sample_handle = -1;


// Error callback - simply terminates with an error. Used where we don't
// expect errors.
static void error_callback_fatal(dspqueue_t queue, AEEResult error, void *context) {
    fprintf(stderr, "ERROR 0x%x: for queue %p\n", error, queue);
    exit(EXIT_FAILURE);
}


static uint64_t time_usec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec*1000000ULL + ts.tv_nsec/1000;
}


struct test_context {
    uint32_t next;
    uint32_t done_value;
    sem_t done_sem;
    unsigned num_squares;
    unsigned num_ops;
    uint64_t end_time;
    int test;
};


// Confirm the bytewise square of a buffer is correct
static void test_byte_square(const uint8_t *in_buf, const uint8_t *out_buf, size_t size) {
    unsigned idx = 0;
    while ( size-- ) {
        uint8_t i = *in_buf++;
        uint8_t o = *out_buf++;
        if ( o != (uint8_t)(i * i) ) {
            fprintf(stderr, "Byte square mismatch at %u: Expected %u, got %u\n",
                    idx, (unsigned)((uint8_t)(i*i)), (unsigned)o);
            exit(EXIT_FAILURE);
        }
        idx++;
    }
}


// Packet callback
static void packet_callback(dspqueue_t queue, AEEResult error, void *context) {

    int err = 0;
    struct test_context *c = (struct test_context*) context;

    // Repeatedly read packets from the queue until it's empty. We don't
    // necessarily get a separate callback for each packet, and new packets
    // may arrive while we're processing the previous one.

    while ( 1 ) {
        uint32_t msg[SAMPLE_MAX_MESSAGE_SIZE/4];
        uint32_t flags;
        uint32_t msg_length;
        struct dspqueue_buffer bufs[SAMPLE_MAX_PACKET_BUFFERS];
        uint32_t num_bufs;

        // Read packet from queue
        err = dspqueue_read_noblock(queue,
                                    &flags,
                                    SAMPLE_MAX_PACKET_BUFFERS, // Maximum number of buffer references
                                    &num_bufs, // Number of buffer references
                                    bufs, // Buffer references
                                    sizeof(msg), // Max message length
                                    &msg_length, // Message length
                                    (uint8_t*)msg); // Message
        if ( err == AEE_EWOULDBLOCK ) {
            // Consumed all packets available for now
            return;
        } else if ( err != 0 ) {
            fprintf(stderr, "dspqueue_read_noblock failed: 0x%08x\n", (unsigned)err);
            exit(EXIT_FAILURE);
        }

        // Basic sanity checks
        if ( msg_length < 4 ) {
            fprintf(stderr, "Bad message\n");
            exit(EXIT_FAILURE);
        }

        // Process packet based on its message type
        switch ( msg[0] ) {
            case SAMPLE_MSG_ECHO_RESP:
                // Echo response
                if ( msg_length != 8 ) {
                    fprintf(stderr, "Bad echo packet\n");
                    continue;
                }
                // If the value in the packet matches the end marker we'll
                // signal the semaphore to indicate to the main thread that
                // we've reached the end of the use case. Otherwise we'll
                // just check we're getting reponses back in the expected
                // order
                if ( msg[1] == c->done_value ) {
                    sem_post(&c->done_sem);
                } else {
                    if ( msg[1] != c->next ) {
                        fprintf(stderr, "Unexpected echo response "
                                "(expected %u, got %u)\n", (unsigned) c->next,
                                (unsigned)msg[1]);
                        exit(EXIT_FAILURE);
                    }
                    c->next++;
                }
                break;

            case SAMPLE_MSG_BYTE_SQUARE_RESP:
                // Byte square response
                if ( (msg_length != 4) || (num_bufs != 2) ||
                     (bufs[0].size != bufs[1].size) ) {
                    fprintf(stderr, "Bad square response packet\n");
                    exit(EXIT_FAILURE);
                }

                if ( c->test ) {
                    // Check the data is correct (if we're not benchmarking)
                    test_byte_square(bufs[0].ptr, bufs[1].ptr, bufs[0].size);
                }

                c->num_squares++;

                if ( c->num_squares == c->num_ops ) {
                    // Got all responses - capture the timestamp for the
                    // last response and wake up the main thread
                    c->end_time = time_usec();
                    sem_post(&c->done_sem);
                }
                break;


            default:
                fprintf(stderr, "Unknown message type %u\n", (unsigned)msg[0]);
                break;
        }
    }
}


static int open_session( bool isUnsignedPD_Enabled) {

    const char *uri = dspqueue_sample_URI CDSP_DOMAIN;
    int err;
    struct remote_rpc_control_latency ldata;
    if(isUnsignedPD_Enabled) {
        if(remote_session_control) {
            struct remote_rpc_control_unsigned_module data;
            data.domain = CDSP_DOMAIN_ID;
            data.enable = 1;
            if (AEE_SUCCESS != (err = remote_session_control(DSPRPC_CONTROL_UNSIGNED_MODULE, (void*)&data, sizeof(data)))) {
                printf("ERROR 0x%x: remote_session_control failed\n", err);
                goto bail;
            }
        }
        else {
            err = AEE_EUNSUPPORTED;
            printf("ERROR 0x%x: remote_session_control interface is not supported on this device\n", err);
            goto bail;
        }
    }

    err = dspqueue_sample_open(uri, &sample_handle);
    if ( err != 0 ) {
        fprintf(stderr, "DSP session open failed: 0x%08x\n", (unsigned)err);
        goto bail;
    }

    // Enable FastRPC QoS mode
    ldata.enable = 1;
    err = remote_handle64_control(sample_handle, DSPRPC_CONTROL_LATENCY, (void*)&ldata, sizeof(ldata));
    if ( err != 0 ) {
        fprintf(stderr, "Enabling FastRPC QoS mode failed: 0x%08x\n", (unsigned)err);
        goto bail;
    }

bail:
    return err;
}


static void close_session(void) {
    dspqueue_sample_close(sample_handle);
}


static struct test_context *alloc_context(void) {

    int err = 0;
    struct test_context *c;

    c = calloc(1, sizeof(*c));
    if ( c == NULL ) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }
    err = sem_init(&c->done_sem, 0, 0);
    if ( err != 0 ) {
        fprintf(stderr, "sem_init failed: 0x%x\n", err);
        exit(EXIT_FAILURE);
    }

    return c;
}


static void free_context(struct test_context *c) {
    sem_destroy(&c->done_sem);
    free(c);
}


// Simple echo test
void echo_test(void) {

    int err = 0;
    dspqueue_t queue;
    uint64_t dsp_queue_id;
    struct test_context *c;
    int i;
    uint32_t msg[2];
    uint64 t;

    // Create a simple context structure for our test
    c = alloc_context();
    c->next = 0;
    c->done_value = 0xffffffff;

    // Create queue
    err = dspqueue_create(CDSP_DOMAIN_ID,
                          0, // Flags
                          256, // Request queue size
                          256, // Response queue size
                          packet_callback,
                          error_callback_fatal,
                          (void*)c,  // Callback context
                          &queue);
    if ( err != 0 ) {
        fprintf(stderr, "dspqueue_create failed: 0x%08x\n", (unsigned)err);
        exit(EXIT_FAILURE);
    }

    // Export queue for use on the DSP
    err = dspqueue_export(queue, &dsp_queue_id);
    if ( err != 0 ) {
        fprintf(stderr, "dspqueue_export failed: 0x%08x\n", (unsigned)err);
        exit(EXIT_FAILURE);
    }

    // Start the DSP-side echo service. We need to pass the queue ID to the
    // DSP in a FastRPC call; the DSP side will import the queue and start
    // listening for packets in a callback.
    err = dspqueue_sample_start(sample_handle, dsp_queue_id);
    if ( err != 0 ) {
        fprintf(stderr, "dspqueue_sample_start failed: 0x%08x\n", (unsigned)err);
        exit(EXIT_FAILURE);
    }

    // Send a number of echo requests to the DSP, followed by a final
    // one with value 0xffffffff which we'll use to indicate the end
    // of the use case.
    for ( i = 0; i <= NUM_ECHOES; i++ ) {
        msg[0] = SAMPLE_MSG_ECHO; // Echo request
        if ( i < NUM_ECHOES ) {
            msg[1] = i;
        } else {
            msg[1] = 0xffffffff; // End of use case
        }
        // We'll use a blocking write call to write the packet. This will block
        // once the queue is empty, effectively backpressuring the CPU. This is
        // what we want: We want to keep the queue full enough that there's
        // always work for the DSP to do without waiting for the CPU, but we
        // don't want to grow it indefinitely or have to poll for free space.
        // We'll also set a timeout to catch if the DSP hangs unexpectedly.
        err = dspqueue_write(queue,
                             0, // Flags
                             0, NULL, // No buffer references in this packet
                             2 * sizeof(uint32_t), (const uint8_t*)msg, // Message
                             1000000); // Timeout
        if ( err != 0 ) {
            fprintf(stderr, "dspqueue_write failed: 0x%08x\n", (unsigned)err);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for the final response to come back. The callback function will
    // signal the semaphore once all requests have been processed.
    sem_wait(&c->done_sem);

    // Stop the DSP-side service and close the queue
    err = dspqueue_sample_stop(sample_handle, &t);
    if ( err != 0 ) {
        fprintf(stderr, "dspqueue_sample_stop failed: 0x%08x\n", (unsigned)err);
        exit(EXIT_FAILURE);
    }

    // Close the queue and clean up the context
    err = dspqueue_close(queue);
    if ( err != 0 ) {
        fprintf(stderr, "dspqueue_close failed: 0x%08x\n", (unsigned)err);
        exit(EXIT_FAILURE);
    }
    free_context(c);
}



// Data processing test
void process_test(size_t buffer_size, size_t early_wakeup_limit, int num_ops, unsigned num_buffers, int test) {

    int err = 0;
    dspqueue_t queue;
    uint64_t dsp_queue_id;
    struct test_context *c;
    int i;
    uint32_t msg[2];
    void *buffers[MAX_BUFFERS];
    int fds[MAX_BUFFERS];
    uint64_t start_time, end_time;
    uint64 process_time;

    assert(num_buffers <= MAX_BUFFERS);
    if ( !test ) {
        printf("- %d operations, buffer size %u bytes\n", num_ops, (unsigned) buffer_size);
    }

    // Create a simple context structure for our test
    c = alloc_context();
    c->test = test;
    c->num_ops = num_ops;

    // Allocate shared buffers to use as test input/output and map to the DSP.
    // We'll use the first num_buffers/2 buffers as input and the second half
    // as output.
    for ( i = 0; i < num_buffers; i++ ) {

        // Use RPCMEM to allocate a shared ION buffer from the default heap and
        // find its FD
        buffers[i] = rpcmem_alloc(RPCMEM_HEAP_ID_SYSTEM, RPCMEM_DEFAULT_FLAGS | RPCMEM_HEAP_NOREG, buffer_size);
        if ( buffers[i] == NULL ) {
            fprintf(stderr, "Couldn't allocate shared buffer\n");
            exit(EXIT_FAILURE);
        }
        fds[i] = rpcmem_to_fd(buffers[i]);
        if ( fds[i] <= 0 ) {
            fprintf(stderr, "Couldn't get FD for buffer %p\n", buffers[i]);
            exit(EXIT_FAILURE);
        }

        // Map buffer to the DSP.
        err = fastrpc_mmap(CDSP_DOMAIN_ID, fds[i], buffers[i], 0, buffer_size, FASTRPC_MAP_FD);
        if ( err != 0 ) {
            fprintf(stderr, "Buffer mapping failed for buffed %d: 0x%08x\n", fds[i], (unsigned)err);
            exit(EXIT_FAILURE);
        }
    }

    // Fill input buffers with easily recognizable data.
    // The buffers contain a simple sweep of byte values, starting with
    // the file descriptor (FD) to ensure each buffer is unique.
    for ( i = 0; i < (num_buffers/2); i++ ) {
        uint8_t *p = buffers[i];
        size_t count = buffer_size;
        uint8_t v = (uint8_t) fds[i];
        while ( count-- ) {
            *p++ = v++;
        }
    }

    // Create queue
    err = dspqueue_create(CDSP_DOMAIN_ID,
                          0, // Flags
                          4096, // Request queue size
                          4096, // Response queue size
                          packet_callback,
                          error_callback_fatal,
                          (void*)c,  // Callback context
                          &queue);
    if ( err != 0 ) {
        fprintf(stderr, "dspqueue_create failed: 0x%08x\n", (unsigned)err);
        exit(EXIT_FAILURE);
    }

    // Export queue for use on the DSP
    err = dspqueue_export(queue, &dsp_queue_id);
    if ( err != 0 ) {
        fprintf(stderr, "dspqueue_export failed: 0x%08x\n", (unsigned)err);
        exit(EXIT_FAILURE);
    }

    // Start the DSP-side service. We need to pass the queue ID to the
    // DSP in a FastRPC call; the DSP side will import the queue and start
    // listening for packets in a callback.
    err = dspqueue_sample_start(sample_handle, dsp_queue_id);
    if ( err != 0 ) {
        fprintf(stderr, "dspqueue_sample_start failed: 0x%08x\n", (unsigned)err);
        exit(EXIT_FAILURE);
    }

    // Send a number of processing requests to the DSP.
    // Note that this example will reuse the same buffers multiple times,
    // overwriting them as we go. A real application should maintain a
    // buffer pool and only reuse buffers when they have been released by
    // the DSP.
    c->num_squares = 0;
    start_time = time_usec();
    for ( i = 0; i < num_ops; i++ ) {
        struct dspqueue_buffer bufs[2];
        int input_buf = i % (num_buffers/2);
        int output_buf = input_buf + (num_buffers/2);

        // Construct packet contents
        msg[0] = SAMPLE_MSG_BYTE_SQUARE; // Bytewise square request
        msg[1] = early_wakeup_limit;

        memset(bufs, 0, sizeof(bufs));
        // First buffer reference - the input. This is a buffer that the CPU
        // writes and the DSP reads, so we'll need to flush CPU caches and
        // invalidate DSP ones. On platforms with I/O coherency support the
        // framework will automatically skip cache operations where possible.
        bufs[0].fd = fds[input_buf];
        bufs[0].flags = (DSPQUEUE_BUFFER_FLAG_REF | // Take a reference
                         DSPQUEUE_BUFFER_FLAG_FLUSH_SENDER | // Flush CPU
                         DSPQUEUE_BUFFER_FLAG_INVALIDATE_RECIPIENT); // Invalidate DSP

        // Second buffer reference - the output. We'll handle DSP
        // cache maintenance in the response message but need to flush
        // CPU caches to ensure any previously written dirty lines are
        // written out before writes from the DSP start.
        bufs[1].fd = fds[output_buf];
        bufs[1].flags = (DSPQUEUE_BUFFER_FLAG_REF |
                         DSPQUEUE_BUFFER_FLAG_FLUSH_SENDER);

        // Send the packet. As in the echo example we'll use a blocking write.
        // DSP-side processing is the bottleneck, and we'll want the CPU to
        // wait once the queue gets full.
        err = dspqueue_write(queue,
                             0, // Flags - the framework will update this
                             2, bufs, // Buffer references
                             sizeof(msg), (const uint8_t*)msg, // Message
                             1000000); // Timeout
        if ( err != 0 ) {
            fprintf(stderr, "dspqueue_write failed: 0x%08x\n", (unsigned)err);
            exit(EXIT_FAILURE);
        }
    }

    // Wait until we've got all responses back in the callback
    sem_wait(&c->done_sem);
    end_time = c->end_time;

    // Stop the DSP-side service and close the queue
    err = dspqueue_sample_stop(sample_handle, &process_time);
    if ( err != 0 ) {
        fprintf(stderr, "dspqueue_sample_stop failed: 0x%08x\n", (unsigned)err);
        exit(EXIT_FAILURE);
    }

    // Unmap and free shared buffers
    for ( i = 0; i < num_buffers; i++ ) {
        err = fastrpc_munmap(CDSP_DOMAIN_ID, fds[i], NULL, 0);
        if ( err != 0 ) {
            fprintf(stderr, "fastrpc_munmap failed: 0x%08x\n", (unsigned)err);
            exit(EXIT_FAILURE);
        }
        rpcmem_free(buffers[i]);
    }

    if ( !test ) {
        // Display timing statistics
        printf("  - %"PRIu64" us total, %"PRIu64" us DSP processing time for %d operations\n",
               end_time - start_time, (uint64_t)process_time, num_ops);
        printf("  - %"PRIu64" us DSP processing time, %"PRIu64" us overhead per operation\n",
               (uint64_t)process_time / num_ops,
               (end_time - start_time - (uint64_t)process_time) / num_ops);

        // Display early wakeup statistics
        if ( early_wakeup_limit > 0 ) {
            uint64_t wait_time, misses;
            err = dspqueue_get_stat(queue, DSPQUEUE_STAT_EARLY_WAKEUP_WAIT_TIME, &wait_time);
            if ( err == 0 ) {
                err = dspqueue_get_stat(queue, DSPQUEUE_STAT_EARLY_WAKEUP_MISSES, &misses);
            }
            if ( err != 0 ) {
                fprintf(stderr, "dspqueue_get_stat failed: 0x%08x\n", (unsigned)err);
                exit(EXIT_FAILURE);
            }
            printf("  - Early wakeup wait time %"PRIu64" us; misses %"PRIu64"\n", wait_time, misses);
        }
    }

    // Close the queue and clean up the context
    err = dspqueue_close(queue);
    if ( err != 0 ) {
        fprintf(stderr, "dspqueue_close failed: 0x%08x\n", (unsigned)err);
        exit(EXIT_FAILURE);
    }
    free_context(c);
}


// FastRPC data processing test.
// This test runs equivalent data processing operations on the DSP
// using regular FastRPC calls to demonstrate the performance
// difference between using the packet queue and normal FastRPC calls.

void fastrpc_process_test(size_t buffer_size, unsigned num_ops, unsigned num_buffers, int static_maps, int test) {

    int err = 0;
    int i;
    void *buffers[MAX_BUFFERS];
    int fds[MAX_BUFFERS];
    uint64_t start_time, end_time, process_time;

    assert(num_buffers <= MAX_BUFFERS);

    if ( !test ) {
        printf("- %d operations, buffer size %u bytes\n", num_ops, (unsigned) buffer_size);
    }

    // Allocate and optionally map buffers. FastRPC can map buffers automatically
    // during calls, but pre-mapping them reduces overhead.
    // We'll use the first num_buffers/2 buffers as input and the second half
    // as output.
    for ( i = 0; i < num_buffers; i++ ) {
        buffers[i] = rpcmem_alloc(RPCMEM_HEAP_ID_SYSTEM, RPCMEM_DEFAULT_FLAGS, buffer_size);
        if ( buffers[i] == NULL ) {
            fprintf(stderr, "Couldn't allocate shared buffer\n");
            exit(EXIT_FAILURE);
        }
        if ( static_maps ) {
            fds[i] = rpcmem_to_fd(buffers[i]);
            if ( fds[i] <= 0 ) {
                fprintf(stderr, "Couldn't get FD for buffer %p\n", buffers[i]);
                exit(EXIT_FAILURE);
            }
            err = fastrpc_mmap(CDSP_DOMAIN_ID, fds[i], buffers[i], 0, buffer_size, FASTRPC_MAP_STATIC);
            if ( err != 0 ) {
                fprintf(stderr, "Buffer mapping failed for buffed %d: 0x%08x\n", fds[i], (unsigned)err);
                exit(EXIT_FAILURE);
            }
        }
    }

    // Fill input buffers with easily recognizable data.
    for ( i = 0; i < (num_buffers/2); i++ ) {
        uint8_t *p = buffers[i];
        size_t count = buffer_size;
        uint8_t v = (uint8_t) fds[i];
        while ( count-- ) {
            *p++ = v++;
        }
    }

    // Process buffers on the DSP. Note that we don't validate the output when
    // running a benchmark to avoid adding to the overhead.
    process_time = 0;
    start_time = time_usec();
    for ( i = 0; i < num_ops; i++ ) {
        int input_buf = i % (num_buffers/2);
        int output_buf = input_buf + (num_buffers/2);
        uint64 t;

        // dspqueue_sample_do_process() runs the same processing algorithm
        // on its buffers as SAMPLE_MSG_BYTE_SQUARE messages above
        err = dspqueue_sample_do_process(sample_handle,
                                         buffers[input_buf], buffer_size,
                                         buffers[output_buf], buffer_size,
                                         &t);
        if ( err != 0 ) {
            fprintf(stderr, "dspqueue_sample_do_process failed: 0x%08x\n", (unsigned)err);
            exit(EXIT_FAILURE);
        }
        process_time += t;
        if ( test ) {
            test_byte_square(buffers[input_buf], buffers[output_buf], buffer_size);
        }
    }
    end_time = time_usec();

    // Unmap and free shared buffers
    for ( i = 0; i < num_buffers; i++ ) {
        if ( static_maps ) {
            err = fastrpc_munmap(CDSP_DOMAIN_ID, fds[i], NULL, 0);
            if ( err != 0 ) {
                fprintf(stderr, "fastrpc_munmap failed: 0x%08x\n", (unsigned)err);
                exit(EXIT_FAILURE);
            }
        }
        rpcmem_free(buffers[i]);
    }

    // Display timing statistics
    if ( !test ) {
        printf("  - %"PRIu64" us total, %"PRIu64" us DSP processing time for %d operations\n",
               end_time - start_time, process_time, num_ops);
        printf("  - %"PRIu64" us DSP processing time, %"PRIu64" us overhead per operation\n",
               process_time / num_ops,
               (end_time - start_time - process_time) / num_ops);
    }
}


// Buffer size to test with. Buffer size directly affects DSP-side
// processing time as well as cache usage.
#define BUFFER_SIZE (1024*1024)

// Early wakeup limit. The number represents the number of bytes left
// to be processed before sending an early wakeup signal to the
// CPU. This should be tuned for each target and algorithm to signal
// early wakeup just early enough - we'll want to get the signal to
// the CPU as early as possible, but without spending extra time
// waiting for it.
#define EARLY_WAKEUP_LIMIT ((8 * BUFFER_SIZE)/256)

// Number of buffers used in tests. The number of buffers mapped can
// have a minor impact on performance.
#define NUM_BUFFERS 16


static void print_usage()
{

    printf( "Usage:\n"
            "    dspqueue_sample [-d domain] [-U unsigned_PD]\n\n"
            "Options:\n"
            "-d domain: Run on a specific domain. This example is supported only on CDSP.\n"
            "    3: Run the example on CDSP\n"
            "        Default Value: 3(CDSP) for targets having CDSP.\n"
            "-U unsigned_PD: Run on signed or unsigned PD.\n"
            "    0: Run on signed PD.\n"
            "    1: Run on unsigned PD.\n"
            "        Default Value: 1\n"
            );
}


int main(int argc, char* argv[]) {

    int nErr = AEE_SUCCESS;
    int option = 0;
    int domain = -1;
    int unsignedPDFlag = 1;
    bool isUnsignedPD_Enabled = false;

    setbuf(stdout,NULL);
    setbuf(stderr,NULL);

    while ( (option = getopt(argc, argv,"d:U:")) != -1 ) {
        switch (option) {
            case 'd' : domain = atoi(optarg);
                break;
            case 'U' : unsignedPDFlag = atoi(optarg);
                break;
            default:
                print_usage();
                return -1;
        }
    }

    if ( domain == -1 ) {
        printf("\nDSP domain is not provided. Retrieving DSP information using Remote APIs.\n");
        nErr = get_dsp_support(&domain);
        if(nErr != AEE_SUCCESS) {
            printf("ERROR in get_dsp_support: 0x%x, defaulting to CDSP domain\n", nErr);
        }
    }

    if ( domain != CDSP_DOMAIN_ID ) {
        nErr = AEE_EBADPARM;
        printf("\nERROR 0x%x: Invalid domain %d\n", nErr, domain);
        print_usage();
        goto bail;
    }

    if (unsignedPDFlag < 0 || unsignedPDFlag > 1) {
        nErr = AEE_EBADPARM;
        printf("\nERROR 0x%x: Invalid unsigned PD flag %d\n", nErr, unsignedPDFlag);
        print_usage();
        goto bail;
    }
    if(unsignedPDFlag == 1) {
        isUnsignedPD_Enabled = get_unsignedpd_support();
    }

    printf("Attempting to run on %s PD on domain %d\n", isUnsignedPD_Enabled==true?"unsigned":"signed", domain);
    nErr = open_session(isUnsignedPD_Enabled);
    if( nErr != AEE_SUCCESS ) {
        printf("ERROR 0x%x: Test FAILED\n", nErr);
        goto bail;
    }

    printf("Echo test\n");
    echo_test();

    printf("\nPacket queue data processing test\n");
    process_test(BUFFER_SIZE, 0, 10, NUM_BUFFERS, 1);
    process_test(BUFFER_SIZE, 0, 1, NUM_BUFFERS, 0);
    process_test(BUFFER_SIZE, 0, 10, NUM_BUFFERS, 0);
    process_test(BUFFER_SIZE, 0, 100, NUM_BUFFERS, 0);
    process_test(BUFFER_SIZE, 0, 1000, NUM_BUFFERS, 0);

    printf("\nPacket queue data processing - with early wakeup\n");
    process_test(BUFFER_SIZE, EARLY_WAKEUP_LIMIT, 10, NUM_BUFFERS, 1);
    process_test(BUFFER_SIZE, EARLY_WAKEUP_LIMIT, 1, NUM_BUFFERS, 0);
    process_test(BUFFER_SIZE, EARLY_WAKEUP_LIMIT, 10, NUM_BUFFERS, 0);
    process_test(BUFFER_SIZE, EARLY_WAKEUP_LIMIT, 100, NUM_BUFFERS, 0);
    process_test(BUFFER_SIZE, EARLY_WAKEUP_LIMIT, 1000, NUM_BUFFERS, 0);

    printf("\nFastRPC data processing test\n");
    fastrpc_process_test(BUFFER_SIZE, 10, NUM_BUFFERS, 0, 1);
    fastrpc_process_test(BUFFER_SIZE, 1, NUM_BUFFERS, 0, 0);
    fastrpc_process_test(BUFFER_SIZE, 10, NUM_BUFFERS, 0, 0);
    fastrpc_process_test(BUFFER_SIZE, 100, NUM_BUFFERS, 0, 0);
    fastrpc_process_test(BUFFER_SIZE, 1000, NUM_BUFFERS, 0, 0);

    printf("\nFastRPC data processing test - with static mappings\n");
    fastrpc_process_test(BUFFER_SIZE, 10, NUM_BUFFERS, 1, 1);
    fastrpc_process_test(BUFFER_SIZE, 1, NUM_BUFFERS, 1, 0);
    fastrpc_process_test(BUFFER_SIZE, 10, NUM_BUFFERS, 1, 0);
    fastrpc_process_test(BUFFER_SIZE, 100, NUM_BUFFERS, 1, 0);
    fastrpc_process_test(BUFFER_SIZE, 1000, NUM_BUFFERS, 1, 0);

    close_session();

bail:
    return nErr;
}
