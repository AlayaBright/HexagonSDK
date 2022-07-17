/**=============================================================================
@file
   wav_file_handle.h

@brief
   Header file for handling wav files.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#ifndef _WAV_FILE_HANDLE_H
#define _WAV_FILE_HANDLE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint32_t chunk_ID;
    uint32_t chunk_size;
    uint32_t format;
    uint32_t subchunk1_ID;
    uint32_t subchunk1_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    uint32_t subchunk2_ID;
    uint32_t subchunk2_size;
} wav_header;

#define AUDIO_FMT_Uncompressed_PCM 1
#define AUDIO_FMT_IEEE_Float_pt    3

void wav_header_create(wav_header* header, uint16_t num_channels, uint32_t sample_rate, uint16_t bits_per_sample, uint32_t num_samples, uint16_t audio_format);

uint32_t wav_header_get_num_samples(wav_header* header);


#ifdef __cplusplus
}
#endif

#endif /* _WAV_FILE_HANDLE_H */
