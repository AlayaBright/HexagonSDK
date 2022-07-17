/*
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/* Packet queue example: CPU-DSP shared definitions */

#ifndef DSPQUEUE_SAMPLE_SHARED_H
#define DSPQUEUE_SAMPLE_SHARED_H


enum dspqueue_sample_message_type {
    SAMPLE_MSG_ECHO = 1,
    SAMPLE_MSG_BYTE_SQUARE = 2,
    SAMPLE_MSG_END = 3
};

enum dspqueue_sample_request_type {
    SAMPLE_MSG_ECHO_RESP = 1,
    SAMPLE_MSG_BYTE_SQUARE_RESP = 2,
    SAMPLE_MSG_END_RESP = 3
};

#define SAMPLE_MAX_MESSAGE_SIZE 8
#define SAMPLE_MAX_PACKET_BUFFERS 2

/* Example message types:

   Each packet contains a message with at least a single 32-bit word
   (message type). Other packet contents depend on the message type.


   REQUESTS (CPU->DSP)
   -------------------

   SAMPLE_MSG_ECHO (1):
   - Message length 8 bytes:
          0: SAMPLE_MSG_ECHO
          4: Value to echo
   - The DSP sends a SAMPLE_MSG_ECHO_RESP message back to the CPU

   SAMPLE_MSG_BYTE_SQUARE (2):
   - Message length 8 bytes:
          0: SAMPLE_MSG_BYTE_SQUARE
          4: Early wakeup limit: The DSP will send an early wakeup packet
             when it is X bytes from the end of the buffer. 0 to disable.
   - Two buffer references: Input and output
   - The DSP sends a SAMPLE_MSG_BYTE_SQUARE_RESP message back as an
     acknowledgement with the same buffer references.


   RESPONSES (DSP->CPU)
   --------------------

   SAMPLE_MSG_ECHO_RESP (1):
   - Message length 8 bytes:
          0: SAMPLE_MSG_ECHO_RESP
          4: Value to echo

   SAMPLE_MSG_BYTE_SQUARE_RESP (2):
   - Message length 4 bytes:
          0: SAMPLE_MSG_BYTE_SQUARE_RESP
*/


#endif /* DSPQUEUE_SAMPLE_SHARED_H */
