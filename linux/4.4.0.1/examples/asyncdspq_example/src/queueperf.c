/**=============================================================================
Copyright (c) 2017 QUALCOMM Technologies Incorporated.
All Rights Reserved Qualcomm Proprietary
=============================================================================**/

/* Performance Tests for asyncdspq */

#include "queuetest.h"
#include "AEEStdErr.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "rpcmem.h" // helper API's for shared buffer allocation
#include "asyncdspq.h"
#include "dsp_capabilities_utils.h"     // available under <HEXAGON_SDK_ROOT>/utils/examples
#include "os_defines.h"

#define DEBUGPRINT(x, ...)
//#define DEBUGPRINT(x, ...) printf(x, ##__VA_ARGS__)

#define QUEUE_SIZE 256
#define QUEUE_ALIGN 256


unsigned long long GetTime(void)
{
    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);

    return tv.tv_sec * 1000000ULL + tv.tv_usec;
}

void error(char *msg)
{
    fprintf(stderr, "ERROR: %s\n", msg);
    exit(EXIT_FAILURE);
}

void error_code(char *msg, int code)
{
    fprintf(stderr, "ERROR: %s (0x%x)\n", msg, code);
    exit(EXIT_FAILURE);
}

void error_code_fail(const char *file, int line, const char *expr, int code)
{
    fprintf(stderr, "%s(%d): %s failed with 0x%x\n", file, line, expr, code);
    exit(EXIT_FAILURE);
}

#define TEST_ERROR_CODE(x) { int e; if ( (e = x) != 0 ) { error_code_fail(__FILE__, __LINE__, #x, e); } }


#define NUMADDS 100000

static void error_callback(asyncdspq_t queue, void *context, AEEResult error)
{
    error_code("error_callback", error);
}

typedef struct {
    asyncdspq_t *queue;
    int reqBufSize;
} asyncdspq_thread_args_t;

static void* asyncdspq_add_write_thread(void *data)
{
    int err;
    asyncdspq_thread_args_t *a = (asyncdspq_thread_args_t *)data;
    int i;
    for (i = 0; i < NUMADDS; i++) {
        // Add
        DEBUGPRINT("Write: add %d\n", i);
        uint32_t *req = malloc(a->reqBufSize);
        req[0] = 1;
        req[1] = i;
        req[2] = 65536 + i;
        if ((err = asyncdspq_write(a->queue, (uint8_t *)req, a->reqBufSize)) != 0) {
            free(req);
            error_code("asyncdspq_write failed", err);
        }
        free(req);
    }

    // Exit
    {
        uint32_t req[1];
        req[0] = 2;
        if ((err = asyncdspq_write(a->queue, (uint8_t *)req, sizeof(req))) != 0) {
            error_code("asyncdspq_write failed", err);
        }
    }

    return NULL;
}


static void* asyncdspq_add_read_thread(void *data)
{
    int err;
    asyncdspq_thread_args_t *a = (asyncdspq_thread_args_t *)data;
    int i = 0;
    uint32_t lastsum = 0;

    while (1) {
        // Add
        uint32_t resp[16];
        uint32_t resplen;
        DEBUGPRINT("Read: Getting\n");
        if ((err = asyncdspq_read(a->queue, (uint8_t *)resp, sizeof(resp), &resplen)) != 0) {
            error_code("asyncdspq_read_resp failed", err);
        }
        if (resplen < 4) {
            error("Bad response length");
        }
        if (resp[0] == 1) {
            // Sum
            if (resplen != 8) {
                error("Bad response length for sum");
            }
            lastsum = resp[1];
            if (lastsum != (2 * i + 65536)) {
                fprintf(stderr, "%d: %u %u\n", i, resp[0], resp[1]);
                error("Sum mismatch");
            }
            DEBUGPRINT("Read: Got sum %d\n", i);
            i++;
        } else if (resp[0] == 2) {
            DEBUGPRINT("Read: Got exit\n");
            // Exit
            if (resplen != 8) {
                error("Bad response length for exit");
            }
            if (resp[1] != 0) {
                error_code("Exit response with an error", resp[1]);
            }
            return (void *)((uintptr_t)lastsum);
        } else {
            fprintf(stderr, "Read: Got unknown response %u\n", resp[1]);
        }
    }

    return NULL;
}


extern int asyncdspq_read_waits;
extern int asyncdspq_write_waits;

int test_asyncdspq(remote_handle64 handle)
{
    asyncdspq_t queue;
    asyncdspq_attach_handle_t attach_handle;
    asyncdspq_t resp_queue;
    asyncdspq_attach_handle_t resp_attach_handle;
    int err = AEE_SUCCESS;
    unsigned t;
    uint64_t t1, t2;
    int sizes[][2] = {
        { 12, 256 }, //{request buffer size, queue size}
        { 12, 512 },
        { 12, 1024 },
        { 12, 2048 },
        { 12, 4096 },
        { 244, 256 },
        { 244, 512 },
        { 244, 1024 },
        { 244, 2048 },
        { 244, 4096 }
    };

    printf("\nqueueperf: Creating queues and performing %d operations on DSP\n", NUMADDS);
    printf("This can take a few minutes. Please wait...\n\n");
    printf("%25s %25s %25s %25s %25s\n",
           "Request buf size (Bytes)",
           "Queue size (Bytes)",
           "Average Time (usec)",
           "Read queue waits",
           "Write queue waits"
           );
    printf("---------------------------");
    printf("---------------------------");
    printf("---------------------------");
    printf("---------------------------");
    printf("---------------------------\n");
    for (int i = 0; i < sizeof(sizes) / sizeof(sizes[0]); ++i) {
        asyncdspq_read_waits = 0;
        asyncdspq_write_waits = 0;
        queue = NULL;
        attach_handle = 0;
        resp_queue = NULL;
        resp_attach_handle = 0;
        err = asyncdspq_create(&queue, &attach_handle,
                              ASYNCDSPQ_ENDPOINT_APP_CPU, ASYNCDSPQ_ENDPOINT_CDSP, // apps to DSP
                              sizes[i][1], error_callback, NULL, NULL, NULL, 0);
        if (err != 0) {
            fprintf(stderr, "ERROR 0x%x: asyncdspq_create failed for Apps->DSP\n", err);
            goto error;
        }
        err = asyncdspq_create(&resp_queue, &resp_attach_handle,
                              ASYNCDSPQ_ENDPOINT_CDSP, ASYNCDSPQ_ENDPOINT_APP_CPU,
                              sizes[i][1], error_callback, NULL, NULL, NULL, 0);
        if (err != 0) {
            fprintf(stderr, "ERROR 0x%x: asyncdspq_create failed for DSP->Apps\n", err);
            goto error_destroy_req;
        }
        DEBUGPRINT("Queues created. Handles 0x%08x, 0x%08x. Starting adder on DSP\n",
                   attach_handle, resp_attach_handle);

        t1 = GetTime();

        pthread_attr_t attr;
        pthread_t read_thread, write_thread;
        asyncdspq_thread_args_t read_args, write_args;
        write_args.queue = queue;
        write_args.reqBufSize = sizes[i][0];
        read_args.queue = resp_queue;
        if ((err = pthread_attr_init(&attr)) != 0) {
            error_code("pthread_attr_init failed", err);
            goto error_destroy_resp;
        }
        if ((err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE)) != 0) {
            fprintf(stderr, "ERROR 0x%x: pthread_attr_setdetachstate failed\n", err);
            goto error_destroy_resp;
        }
        if ((err = pthread_create(&write_thread, &attr, asyncdspq_add_write_thread, &write_args)) != 0) {
            fprintf(stderr, "ERROR 0x%x: Writer pthread_create failed\n", err);
            goto error_destroy_resp;
        }
        if ((err = pthread_create(&read_thread, &attr, asyncdspq_add_read_thread, &read_args)) != 0) {
            fprintf(stderr, "ERROR 0x%x: Reader pthread_create failed\n", err);
            goto error_destroy_resp;
        }

        if ((err = queuetest_asyncdspq_adder_spin(handle, attach_handle, resp_attach_handle)) != 0) {
            fprintf(stderr, "ERROR 0x%x: queuetest_asyncdspq_adder_spin failed\n", err);
            goto error_destroy_resp;
        }

        void *ret;
        if ((err = pthread_join(write_thread, &ret)) != 0) {
            fprintf(stderr, "ERROR 0x%x: Writer pthread_join failed\n", err);
            goto error_destroy_resp;
        }
        if (ret != NULL) {
            fprintf(stderr, "ERROR 0x%x: Writer pthread join failed\n", err);
            goto error_destroy_resp;
        }
        if ((err = pthread_join(read_thread, &ret)) != 0) {
            fprintf(stderr, "ERROR 0x%x: Reader pthread_join failed\n", err);
            goto error_destroy_resp;
        }
        uintptr_t lastsum = (uintptr_t)ret;
        if (lastsum != (2 * (NUMADDS - 1) + 65536)) {
            fprintf(stderr, "Unexpected last sum\n");
            goto error_destroy_resp;
        }

        t2 = GetTime();

        t = (unsigned)(t2 - t1);
        printf("%25d", sizes[i][0]);
        printf("%25d", sizes[i][1]);
        printf("%25d", t / NUMADDS);
        printf("%25d", asyncdspq_read_waits);
        printf("%25d\n", asyncdspq_write_waits);

    error_destroy_resp:
        if ((err = asyncdspq_destroy(resp_queue)) != 0) {
            fprintf(stderr, "ERROR 0x%x: asyncdspq_destroy failed for response queue\n", err);
            break;
        }
    error_destroy_req:
        if ((err = asyncdspq_destroy(queue)) != 0) {
            fprintf(stderr, "ERROR 0x%x: asyncdspq_destroy failed for request queue\n", err);
            break;
        }
    }

    printf("\n");
error:
    return err;
}

static int open_session(bool isUnsignedPD_Enabled)
{
    int nErr = 0;
    remote_handle64 handle = -1;
    char *queuetest_URI_domain = NULL;

    if(isUnsignedPD_Enabled) {
        if(remote_session_control) {
            struct remote_rpc_control_unsigned_module data;
            data.domain = CDSP_DOMAIN_ID;
            data.enable = 1;
            if (AEE_SUCCESS != (nErr = remote_session_control(DSPRPC_CONTROL_UNSIGNED_MODULE, (void*)&data, sizeof(data)))) {
                printf("ERROR 0x%x: remote_session_control failed\n", nErr);
                goto bail;
            }
        }
        else {
            nErr = AEE_EUNSUPPORTED;
            printf("ERROR 0x%x: remote_session_control interface is not supported on this device\n", nErr);
            goto bail;
        }
    }

    queuetest_URI_domain = queuetest_URI CDSP_DOMAIN;

    nErr = queuetest_open(queuetest_URI_domain, &handle);
    if (nErr) {
        printf("ERROR 0x%x: unable to create fastrpc session on CDSP\n", nErr);
        goto bail;
    }

    setbuf(stdout,NULL);
    setbuf(stderr,NULL);

    rpcmem_init();
    TEST_ERROR_CODE(queuetest_set_clocks(handle));
    queuetest_enable_logging(handle);

    test_asyncdspq(handle);

bail:

    if(handle != -1)
        queuetest_close(handle);
    rpcmem_deinit();
    return nErr;
}

static void print_usage()
{
    printf( "Usage:\n"
            "    queueperf [-d domain] [-U unsigned_PD]\n\n"
            "Options:\n"
            "-d domain: DSP domain to run. This example is supported only on CDSP.\n"
            "    3: Run the example on CDSP\n"
            "        Default Value: 3(CDSP) for targets having CDSP.\n"
            "-U unsigned_PD: Run on signed or unsigned PD.\n"
            "    0: Run on signed PD.\n"
            "    1: Run on unsigned PD.\n"
            "        Default Value: 1\n"
            );
}


int main(int argc, char *argv[])
{
    int nErr = 0;
    int option = 0;
    int domain = -1;
    int unsignedPDFlag = 1;
    bool isUnsignedPD_Enabled = false;

    while ( (option = getopt(argc, argv,"d:U:")) != -1 )
    {
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

    if ( domain == -1 )
    {
        printf("\nDSP domain is not provided. Retrieving DSP information using Remote APIs.\n");
        nErr = get_dsp_support(&domain);
        if (nErr != AEE_SUCCESS) {
            printf("ERROR in get_dsp_support: 0x%x, defaulting to CDSP domain\n", nErr);
        }
    }

    if ( domain != CDSP_DOMAIN_ID )
    {
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

    printf("Done\n");

bail:
    return nErr;
}
