/*
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/* DSP-side packet queue example. Implements a simple asynchronous processing
   service that calculates a byte-wise square of data buffers. */

#define FARF_ERROR 1
#define FARF_HIGH 1
#define FARF_MEDIUM 0
#define FARF_LOW 0
#include <HAP_farf.h>
#include <string.h>
#include <HAP_power.h>
#include <HAP_mem.h>
#include <HAP_perf.h>
#include <HAP_ps.h>
#include <qurt_thread.h>
#include <AEEStdErr.h>
#include "dspqueue_sample.h"
#include "dspqueue_sample_shared.h"
#include "dspqueue.h"


// Context structure for the test
struct test_context {
    dspqueue_t queue;
    uint64_t process_time;
};



AEEResult dspqueue_sample_open(const char *uri, remote_handle64 *handle) {

    int err = 0;
    struct test_context *c;

    c = calloc(1, sizeof(*c));
    if ( c == NULL ) {
        return AEE_ENOMEMORY;
    }

    // Use the test context structure as a handle
    *handle = (remote_handle64) c;

    // Enable FARF logs
    HAP_setFARFRuntimeLoggingParams(0x1f, NULL, 0);

    // Set client class
    HAP_power_request_t request;
    memset(&request, 0, sizeof(HAP_power_request_t));
    request.type = HAP_power_set_apptype;
    request.apptype = HAP_POWER_COMPUTE_CLIENT_CLASS;

    if ( (err = HAP_power_set((void*) c, &request)) != 0 ) {
        return err;
    }

    // Set to turbo and disable DCVS
    memset(&request, 0, sizeof(HAP_power_request_t));
    request.type = HAP_power_set_DCVS_v2;
    request.dcvs_v2.dcvs_enable = FALSE;
    request.dcvs_v2.set_dcvs_params = TRUE;
    request.dcvs_v2.dcvs_params.min_corner = HAP_DCVS_VCORNER_DISABLE;
    request.dcvs_v2.dcvs_params.max_corner = HAP_DCVS_VCORNER_DISABLE;
    request.dcvs_v2.dcvs_params.target_corner = HAP_DCVS_VCORNER_TURBO;
    request.dcvs_v2.set_latency = TRUE;
    request.dcvs_v2.latency = 40;
    return HAP_power_set((void*) c, &request);
}


AEEResult dspqueue_sample_close(remote_handle64 handle) {

    struct test_context *c = (struct test_context*) handle;

    if ( c == NULL ) {
        return AEE_EBADPARM;
    }
    if ( c->queue != NULL ) {
        FARF(ERROR, "Closing handle with queue still open");
        return AEE_EITEMBUSY;
    }
    free(c);
    return AEE_SUCCESS;
}


static void sample_packet_callback(dspqueue_t queue, int error, void *context);
static void sample_error_callback(dspqueue_t queue, int error, void *context);


AEEResult dspqueue_sample_start(remote_handle64 handle, uint64 dsp_queue_id) {

    int err = 0;
    struct test_context *c = (struct test_context*) handle;

    if ( c == NULL ) {
        return AEE_EBADPARM;
    }
    if ( c->queue != NULL ) {
        FARF(ERROR, "Queue already open");
        return AEE_EITEMBUSY;
    }
    c->process_time = 0;

    // Import queue created on the CPU
    err = dspqueue_import(dsp_queue_id, // Queue ID from dspqueue_export
                          sample_packet_callback, // Packet callback
                          sample_error_callback, // Error callback; no errors expected on the DSP
                          (void*)c, // Callback context
                          &c->queue);
    if ( err != 0 ) {
        FARF(ERROR, "Queue import failed with 0x%08x", (unsigned)err);
        return err;
    }

    // The framework will start calling sample_packet_callback when requests
    // become available from the CPU

    return AEE_SUCCESS;
}


AEEResult dspqueue_sample_stop(remote_handle64 handle, uint64 *process_time) {

    int err = 0;
    struct test_context *c = (struct test_context*) handle;

    if ( c == NULL ) {
        return AEE_EBADPARM;
    }
    if ( c->queue == NULL ) {
        FARF(ERROR, "Queue not open");
        return AEE_EBADSTATE;
    }

    // Close queue. dspqueue_close() will also wait for callbacks to finish.
    err = dspqueue_close(c->queue);
    c->queue = NULL;
    if ( err != 0 ) {
        FARF(ERROR, "Queue close failed with 0x%08x", (unsigned)err);
        return err;
    }

    *process_time = c->process_time;

    return AEE_SUCCESS;
}


// Calculate a bytewise square of one buffer to another.
static void byte_square(const uint8_t *in_buf, uint8_t *out_buf, size_t size) {
    while ( size-- ) {
        uint8_t v = *in_buf++;
        *out_buf++ = v * v;
    }
}


static void sample_error_callback(dspqueue_t queue, int error, void *context) {
    // No errors expected on the DSP.
    FARF(ERROR, "Error callback: 0x%08x", (unsigned)error);
}


static void sample_packet_callback(dspqueue_t queue, int error, void *context) {

    int err = 0;
    struct test_context *c = (struct test_context*) context;

    // Repeatedly read packets from the queue until it's empty. We don't
    // necessarily get a separate callback for each packet, and new packets
    // may arrive while we're processing the previous one. This ensures we
    // keep the DSP busy as much as possible and avoid waiting for the CPU.

    while ( 1 ) {
        uint32_t msg[SAMPLE_MAX_MESSAGE_SIZE/4];
        uint32_t resp_msg[SAMPLE_MAX_MESSAGE_SIZE/4];
        uint32_t flags;
        uint32_t msg_length;
        struct dspqueue_buffer bufs[SAMPLE_MAX_PACKET_BUFFERS];
        struct dspqueue_buffer resp_bufs[SAMPLE_MAX_PACKET_BUFFERS];
        uint32_t num_bufs;
        uint32_t len, early_limit;
        uint64_t t1, t2;

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
            FARF(ERROR, "dspqueue_read_noblock failed: 0x%08x", (unsigned)err);
            return;
        }

        // Basic sanity checks
        if ( msg_length < 4 ) {
            FARF(ERROR, "Bad message");
            continue;
        }

        // Process packet based on its message type
        switch ( msg[0] ) {
            case SAMPLE_MSG_ECHO:
                // Echo - send back the same message.
                if ( msg_length != 8 ) {
                    FARF(ERROR, "Bad echo packet");
                    continue;
                }
                FARF(HIGH, "Echo %u", (unsigned)msg[1]);

                // We'll use a blocking write call here. Typically the CPU
                // can process responses fast enough to avoid blocking; in the
                // off chance that isn't the case we'd rather block than handle
                // timeouts etc.
                resp_msg[0] = SAMPLE_MSG_ECHO_RESP;
                resp_msg[1] = msg[1];
                err = dspqueue_write(queue,
                                     0, // Flags
                                     0, NULL, // No buffers
                                     8, (const uint8_t*)resp_msg, // Message
                                     DSPQUEUE_TIMEOUT_NONE);
                if ( err != 0 ) {
                    FARF(ERROR, "dspqueue_write failed: 0x%08x", (unsigned)err);
                    return;
                }
                break;


            case SAMPLE_MSG_BYTE_SQUARE:
                // Byte-wise square
                if ( (msg_length != 8) || (num_bufs != 2) ||
                     (bufs[0].size != bufs[1].size) ) {
                    FARF(ERROR, "Bad square packet");
                    continue;
                }
                len = bufs[0].size;
                early_limit = msg[1];
                if ( early_limit > len ) {
                    FARF(ERROR, "Bad square packet");
                    continue;
                }

                // Do the work, measuring the time used for processing
                FARF(HIGH, "Square from %d (%p) to %d (%p), size %u", bufs[0].fd, bufs[0].ptr,
                     bufs[1].fd, bufs[1].ptr, bufs[0].size);
                t1 = HAP_perf_get_time_us();
                byte_square(bufs[0].ptr, bufs[1].ptr, len - early_limit);
                t2 = HAP_perf_get_time_us();
                c->process_time += t2-t1;

                if ( early_limit > 0 ) {
                    // Send early wakeup packet and finish the rest of the buffer
                    dspqueue_write_early_wakeup_noblock(queue, 0, 0);
                    // We'll ignore errors writing early wakeup packets - they're
                    // effectively a hint to the CPU.
                    t1 = HAP_perf_get_time_us();
                    byte_square(((const uint8_t*)bufs[0].ptr) + (len - early_limit),
                                ((uint8_t*)bufs[1].ptr) + (len - early_limit),
                                early_limit);
                    t2 = HAP_perf_get_time_us();
                    c->process_time += t2-t1;
                }

                // Construct response. We'll send back a SAMPLE_MSG_BYTE_SQUARE
                // with two buffer references: The original input and output
                // buffers. We'll release references on both buffers and
                // perform cache maintenance on the output buffer.
                resp_msg[0] = SAMPLE_MSG_BYTE_SQUARE_RESP;
                memset(resp_bufs, 0, sizeof(resp_bufs));
                resp_bufs[0].fd = bufs[0].fd;
                resp_bufs[0].flags = DSPQUEUE_BUFFER_FLAG_DEREF; // Release reference
                // (If we had written to the input buffer, we'd also need to flush it)
                resp_bufs[1].fd = bufs[1].fd;
                resp_bufs[1].flags = (DSPQUEUE_BUFFER_FLAG_DEREF | // Release reference
                                      DSPQUEUE_BUFFER_FLAG_FLUSH_SENDER | // Flush DSP
                                      DSPQUEUE_BUFFER_FLAG_INVALIDATE_RECIPIENT); // Invalidate CPU

                // Write response packet. As above we'll use a blocking write
                err = dspqueue_write(queue,
                                     0, // Flags
                                     2, resp_bufs, // Buffer references
                                     4, (const uint8_t*)resp_msg, // Message
                                     DSPQUEUE_TIMEOUT_NONE);
                if ( err != 0 ) {
                    FARF(ERROR, "dspqueue_write failed: 0x%08x", (unsigned)err);
                    return;
                }
                break;


            default:
                FARF(ERROR, "Unknown message type %u", (unsigned)msg[0]);
        }
    }
}


AEEResult dspqueue_sample_do_process(remote_handle64 handle,
                                     const uint8 *input, int input_len,
                                     uint8 *output, int output_len,
                                     uint64 *process_time) {

    uint64_t t1, t2;

    t1 = HAP_perf_get_time_us();
    byte_square(input, output, input_len);
    t2 = HAP_perf_get_time_us();
    *process_time = t2 - t1;

    return AEE_SUCCESS;
}
