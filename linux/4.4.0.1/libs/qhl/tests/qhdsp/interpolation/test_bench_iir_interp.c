#include <stdio.h>
#include <string.h>

#include "qhmath.h"
#include "qhl_common.h"

#include "../common/wav_file_handle.h"
#include "qhdsp.h"
#include "test_data/coeffs/iir_coeffs_interp_f.h"
#include "test_data/coeffs/iir_coeffs_interp_h.h"

char input_file_path[512];
char dut_file_path[512];
char ref_file_path[512];

#define BLOCK_SIZE              64
#define NUM_BIQUADS             5
#define INTERPOLATION_FACTOR    2

float input_buff_f[BLOCK_SIZE];
float output_buff_f[BLOCK_SIZE * INTERPOLATION_FACTOR];
float ref_buff_f[BLOCK_SIZE * INTERPOLATION_FACTOR];
float *filter_coeffs_ptr_f;

int16_t input_buff_h[BLOCK_SIZE];
int16_t output_buff_h[BLOCK_SIZE * INTERPOLATION_FACTOR];
int16_t ref_buff_h[BLOCK_SIZE * INTERPOLATION_FACTOR];
int16_t *filter_coeffs_ptr_h;

float_a8_t biquad_filter_states_f[NUM_BIQUADS][2];
int64_t biquad_filter_states_h[NUM_BIQUADS][2];

void print_usage()
{
    printf("-----------------------------------------------------------------------------\n");
    printf("IIR INTERPOLATION test bench\n");
    printf("-----------------------------------------------------------------------------\n");
    printf("Author: QHML team.\n");
    printf("September 5th 2019.\n");
    printf("-----------------------------------------------------------------------------\n");
    printf("Usage: -i<INPUT_FILE_PATH -d<DUT_FILE_PATH> -r<REF_FILE_PATH> <optional flags>\n");
    printf("Optional flags:");
    printf("\n-----------------------------------------------------------------------------\n");
}

void parse_command_line(int argc, char **argv)
{
    uint32_t i;
    uint32_t j;

    for(i = 0; i < argc; i++)
    {
        if( strncmp( argv[i], "-i", 2 ) == 0 )
        {
            j = 2;

            while(argv[i][j]!='\0')
            {
                input_file_path[j-2] = argv[i][j];
                j++;
            }
        }
        if( strncmp( argv[i], "-d", 2 ) == 0 )
        {
            j = 2;

            while(argv[i][j]!='\0')
            {
                dut_file_path[j-2] = argv[i][j];
                j++;
            }
        }
        if( strncmp( argv[i], "-r", 2 ) == 0 )
        {
            j = 2;

            while(argv[i][j]!='\0')
            {
                ref_file_path[j-2] = argv[i][j];
                j++;
            }
        }
    }
}

double calculate_snr_f(float * REF, float * DUT, uint32_t nPoints)
{
    double err, sig;

    err = 0.0;
    sig = 0.0;

    for(uint32_t i = 0; i < nPoints; i++)
    {
        sig += REF[i] * REF[i];

        err += (REF[i] - DUT[i]) * (REF[i] - DUT[i]);
    }

    return 10 * qhmath_log10_f(sig / err);
}

double calculate_snr_h(int16_t * REF, int16_t * DUT, uint32_t nPoints)
{
    int16_t max_err = 0;

    for(uint32_t i = 0; i < nPoints; i++)
    {
        if(qhmath_abs_h(REF[i] - DUT[i]) > max_err)
        {
            max_err = qhmath_abs_h(REF[i] - DUT[i]);
        }
    }

    if(max_err!=0)    // avoid log2(zero)
    {
        return (double)((float)96.0 - (float)6.0*((qhmath_log2_f((float)max_err))+(float)1.0));
    }
    else
    {
        return (double)96.0;       // max for 16bit fixed point (6dB rule)
    }
}

int main(int argc, char *argv[])
{
    FILE *fp_in, *fp_dut, *fp_ref;
    wav_header input_file_wav_header, dut_file_wav_header, ref_file_wav_header;
    uint32_t num_input_samples;
    uint16_t dut_file_audio_format;
    double snr;

    double cpc = 0;
    double min_snr = 1000.0;   // intentionally large value

    if(argc<4)
    {
        print_usage();
        return -1;
    }

    performance_high();

    filter_coeffs_ptr_f = *iir_coeffs_24kHz_LP_f;       // set defaults
    filter_coeffs_ptr_h = *iir_coeffs_24kHz_LP_h;

    parse_command_line(argc, argv);

    fp_in = fopen(input_file_path, "rb");
    if(fp_in==NULL)
    {
        printf("\nInput file doesn't exist!\n");
        return -1;
    }

    fp_dut = fopen(dut_file_path, "wb");
    if(fp_dut==NULL)
    {
        printf("\nCannot open output DUT file!\n");
        return -1;
    }

    fp_ref = fopen(ref_file_path, "rb");
    if(fp_ref==NULL)
    {
        printf("\nCannot open output REF file!\n");
        return -1;
    }

    // read input file header
    fread(&input_file_wav_header, sizeof(input_file_wav_header), 1, fp_in);

    // read input file header
    fread(&ref_file_wav_header, sizeof(ref_file_wav_header), 1, fp_ref);

    // get number of samples in input wav file
    num_input_samples = wav_header_get_num_samples(&input_file_wav_header);

    // out file audio format should be the same as input file audio format
    dut_file_audio_format = input_file_wav_header.audio_format;

    // create out file wav header
    wav_header_create(&dut_file_wav_header, input_file_wav_header.num_channels, input_file_wav_header.sample_rate*INTERPOLATION_FACTOR, input_file_wav_header.bits_per_sample, num_input_samples*INTERPOLATION_FACTOR, dut_file_audio_format);

    // write output file header
    fwrite(&dut_file_wav_header, sizeof(dut_file_wav_header), 1, fp_dut);

    // check type of input wave
    if(input_file_wav_header.audio_format == AUDIO_FMT_IEEE_Float_pt)
    {
        // float .wav file

        // initialize filter states
        memset(biquad_filter_states_f, 0, (NUM_BIQUADS*2)*sizeof(float));

        // loop:
        for(uint32_t i = 0; i < num_input_samples/BLOCK_SIZE; i++)
        {
            // read input samples into input buffer
            fread(&input_buff_f, input_file_wav_header.bits_per_sample / 8, BLOCK_SIZE, fp_in);

            // read ref samples
            fread(&ref_buff_f, ref_file_wav_header.bits_per_sample / 8, BLOCK_SIZE*INTERPOLATION_FACTOR, fp_ref);

            // -----------------------------------------------
            // INTERPOLATION processing - C

            measurement_start();
            qhdsp_iirinterp_af(input_buff_f, filter_coeffs_ptr_f, *biquad_filter_states_f, \
                             NUM_BIQUADS, BLOCK_SIZE, INTERPOLATION_FACTOR, output_buff_f);
            measurement_finish();

            cpc += (double)(measurement_get_cycles());

            // calc SNR
            snr = calculate_snr_f(ref_buff_f, output_buff_f, BLOCK_SIZE*INTERPOLATION_FACTOR);
            if(snr < min_snr)
                min_snr = snr;
            // -----------------------------------------------

            fwrite(&output_buff_f, dut_file_wav_header.bits_per_sample / 8, BLOCK_SIZE*INTERPOLATION_FACTOR, fp_dut);
        }

        cpc /= (double)(num_input_samples/BLOCK_SIZE);
        printf("IIR INTERPOLATION float test/bench:\n");
        printf("    [num cycles]:\t%.2lf CPC\n", cpc);
        printf("    [SNR]:       \t%.2lf dB\n", min_snr);
    }
    else if(input_file_wav_header.audio_format == AUDIO_FMT_Uncompressed_PCM)
    {
        // PCM, check bit depth
        if(input_file_wav_header.bits_per_sample == 16)
        {
            // 16b PCM

            // initialize filter states
            memset(biquad_filter_states_h, 0, (NUM_BIQUADS*2)*sizeof(int64_t));

            // loop:
            for (uint32_t i = 0; i < num_input_samples/BLOCK_SIZE; i++)
            {
                // read input samples into input buffer
                fread(&input_buff_h, input_file_wav_header.bits_per_sample / 8, BLOCK_SIZE, fp_in);

                // read ref samples
                fread(&ref_buff_h, ref_file_wav_header.bits_per_sample / 8, BLOCK_SIZE*INTERPOLATION_FACTOR, fp_ref);


                // -----------------------------------------------
                // INTERPOLATION processing - C

                measurement_start();
                qhdsp_iirinterp_ah(input_buff_h, filter_coeffs_ptr_h, *biquad_filter_states_h, \
                                 NUM_BIQUADS, BLOCK_SIZE, INTERPOLATION_FACTOR, output_buff_h);
                measurement_finish();

                cpc += (double)(measurement_get_cycles());

                // calc SNR
                snr = calculate_snr_h(ref_buff_h, output_buff_h, BLOCK_SIZE*INTERPOLATION_FACTOR);
                if(snr < min_snr)
                    min_snr = snr;
                // -----------------------------------------------

                fwrite(&output_buff_h, dut_file_wav_header.bits_per_sample / 8, BLOCK_SIZE*INTERPOLATION_FACTOR, fp_dut);
            }


            cpc /= (double)(num_input_samples/BLOCK_SIZE);
            printf("IIR INTERPOLATION fixed point test/bench:\n");
            printf("C implementation:\n");
            printf("    [num cycles]:\t%.2lf CPC\n", cpc);
            printf("    [SNR]:       \t%.2lf dB\n", min_snr);
        }
        else if(input_file_wav_header.bits_per_sample == 24)
        {
            // 24b PCM
            int32_t sample;

            // loop:
            for (uint32_t i = 0; i < num_input_samples; i++)
            {
                fread(&sample, input_file_wav_header.bits_per_sample / 8, 1, fp_in);

                // some processing

                fwrite(&sample, dut_file_wav_header.bits_per_sample / 8, 1, fp_dut);
            }
        }
        else
        {
            // usage();
            return -1;
        }
    }
    else
    {
        // usage();
        return -1;
    }

    fclose(fp_in);
    fclose(fp_dut);
    fclose(fp_ref);

    performance_normal();

    return 0;
}
