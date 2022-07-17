/**=============================================================================
@file
   wav_file_handle.h

@brief
   C implementation of wav file handling routines.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include "wav_file_handle.h"

void wav_header_create(wav_header* header, uint16_t num_channels, uint32_t sample_rate, uint16_t bits_per_sample, uint32_t num_samples, uint16_t audio_format)
{
    // "RIFF" part
    header->chunk_ID = 0x46464952;      // "RIFF" in ASCII form
    header->format = 0x45564157;        // "WAVE"

    // "fmt" part
    header->subchunk1_ID = 0x20746d66;  // "fmt "
    header->subchunk1_size = 16;        // 16 for PCM.  This is the size of the rest of the Subchunk which follows this number.
    header->audio_format = audio_format;
    header->num_channels = num_channels;
    header->sample_rate = sample_rate;
    header->byte_rate = sample_rate * (uint32_t) (num_channels) * ((uint32_t) bits_per_sample / 8);
    header->block_align = num_channels * bits_per_sample / 8;
    header->bits_per_sample = bits_per_sample;

    // "data" part
    header->subchunk2_ID = 0x61746164;  // "data"
    header->subchunk2_size = num_samples * (uint32_t) (num_channels) * ((uint32_t) bits_per_sample / 8);

    header->chunk_size = 4 + (8 + header->subchunk1_size) + (8 + header->subchunk2_size); // 36 + SubChunk2Size = 4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
}

uint32_t wav_header_get_num_samples(wav_header* header)
{
    return header->subchunk2_size / header->num_channels / (header->bits_per_sample / 8);
}
