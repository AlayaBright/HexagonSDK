#include <stdio.h>
#include <string.h>

#if __hexagon__
#include <qhmath.h>
#include <qhcomplex.h>
#include <hexagon_cache.h>
#endif
#include "qhl_common.h"

#include "../common/wav_file_handle.h"
#include "qhdsp_fir.h"
#include "qhdsp.h"
#include "test_data/coeffs/fir_coeffs_f.h"
#include "test_data/coeffs/fir_coeffs_h.h"

char input_file_path[512];
char dut_file_path_C[512];
char dut_file_path_ASM[512];
char ref_file_path[512];
char filter_type[20];

#define BLOCK_SIZE      64
#define NUM_FILT_TAPS   64

float input_buff_f_C[BLOCK_SIZE];
float delay_line_C[NUM_FILT_TAPS];
float output_buff_f_C[BLOCK_SIZE];

float input_buff_f_ASM[BLOCK_SIZE];
float delay_line_ASM[NUM_FILT_TAPS];
float_a8_t output_buff_f_ASM[BLOCK_SIZE];

float ref_buff_f[BLOCK_SIZE];
float *filter_coeffs_ptr_f;

int16_t input_buff_h_C[BLOCK_SIZE + NUM_FILT_TAPS - 1];
int16_a8_t input_buff_h_ASM[BLOCK_SIZE + NUM_FILT_TAPS - 1];
int16_t output_buff_h_C[BLOCK_SIZE];
int16_a8_t output_buff_h_ASM[BLOCK_SIZE];
int16_t ref_buff_h[BLOCK_SIZE];
int16_t *filter_coeffs_ptr_h;
int16_a8_t filter_coeffs_h[NUM_FILT_TAPS];    // needs to be aligned because of ASM implementation

void print_usage()
{
    printf("-----------------------------------------------------------------------------\n");
    printf("FIR test bench\n");
    printf("-----------------------------------------------------------------------------\n");
    printf("Author: QHML team.\n");
    printf("August 5th 2019.\n");
    printf("-----------------------------------------------------------------------------\n");
    printf("Usage: -i<INPUT_FILE_PATH -dc<DUT_FILE_PATH_C> -da<DUT_FILE_PATH_ASM> \n");
    printf("       -r<REF_FILE_PATH> <optional flags>\n");
    printf("Optional flags:");
    printf("\n\t\t: -f<FILTER_TYPE>; valid options: HP/LP. If not specified, default is LP.");
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
        if( strncmp( argv[i], "-dc", 3 ) == 0 )
        {
            j = 3;

            while(argv[i][j]!='\0')
            {
                dut_file_path_C[j-3] = argv[i][j];
                j++;
            }
        }
        if( strncmp( argv[i], "-da", 3 ) == 0 )
        {
            j = 3;

            while(argv[i][j]!='\0')
            {
                dut_file_path_ASM[j-3] = argv[i][j];
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
        if( strncmp( argv[i], "-f", 2 ) == 0 )
        {
            j = 2;

            while(argv[i][j]!='\0')
            {
                filter_type[j-2] = argv[i][j];
                j++;
            }

            if( strcmp( filter_type, "HP") == 0 )
            {
                filter_coeffs_ptr_f = fir_coeffs_10kHz_HP_64taps_f;
                filter_coeffs_ptr_h = fir_coeffs_10kHz_HP_64taps_h;
            }
            else if( strcmp( filter_type, "LP") == 0 )
            {
                filter_coeffs_ptr_f = fir_coeffs_2kHz_LP_64taps_f;
                filter_coeffs_ptr_h = fir_coeffs_2kHz_LP_64taps_h;
            }
            else    // default
            {
                filter_coeffs_ptr_f = fir_coeffs_2kHz_LP_64taps_f;
                filter_coeffs_ptr_h = fir_coeffs_2kHz_LP_64taps_h;
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
    FILE *fp_in, *fp_dut_C, *fp_dut_ASM, *fp_ref;
    wav_header input_file_wav_header, dut_file_wav_header, ref_file_wav_header;
    uint32_t num_input_samples;
    uint16_t dut_file_audio_format;
    double snr;

    double cpc_C = 0;
    double cpc_ASM = 0;
    double min_snr_C = 1000.0;   // intentionally large value
    double min_snr_ASM = 1000.0;   // intentionally large value

    if(argc<4)
    {
        print_usage();
        return -1;
    }

    performance_high();

    filter_coeffs_ptr_f = fir_coeffs_2kHz_LP_64taps_f;       // set defaults
    filter_coeffs_ptr_h = fir_coeffs_2kHz_LP_64taps_h;

    parse_command_line(argc, argv);

    // copy filter coeffs to aligned filter coeffs buffer
    memcpy((void *)filter_coeffs_h, (const void *)filter_coeffs_ptr_h, NUM_FILT_TAPS*sizeof(int16_t));

    fp_in = fopen(input_file_path, "rb");
    if(fp_in==NULL)
    {
        printf("\nInput file doesn't exist!\n");
        return -1;
    }

    fp_dut_C = fopen(dut_file_path_C, "wb");
    if(fp_dut_C==NULL)
    {
        printf("\nCannot open output DUT C file!\n");
        return -1;
    }

    fp_dut_ASM = fopen(dut_file_path_ASM, "wb");
    if(fp_dut_ASM==NULL)
    {
        printf("\nCannot open output DUT ASM file!\n");
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
    wav_header_create(&dut_file_wav_header, input_file_wav_header.num_channels, input_file_wav_header.sample_rate, input_file_wav_header.bits_per_sample, num_input_samples, dut_file_audio_format);

    // write output file header
    fwrite(&dut_file_wav_header, sizeof(dut_file_wav_header), 1, fp_dut_C);
    fwrite(&dut_file_wav_header, sizeof(dut_file_wav_header), 1, fp_dut_ASM);   // wav header is the same as for C DUT file

    // check type of input wave
    if(input_file_wav_header.audio_format == AUDIO_FMT_IEEE_Float_pt)
    {
        // float .wav file

        // initialize delayline buffers - C & ASM
        memset(delay_line_C, 0, NUM_FILT_TAPS*sizeof(float));
        memset(delay_line_ASM, 0, NUM_FILT_TAPS*sizeof(float));

        // loop:
        for(uint32_t i = 0; i < num_input_samples/BLOCK_SIZE; i++)
        {
            // read input samples into FIR buffer
            fread(&input_buff_f_C, input_file_wav_header.bits_per_sample / 8, BLOCK_SIZE, fp_in);
            memcpy((void *)&input_buff_f_ASM, (const void *)&input_buff_f_C, BLOCK_SIZE*sizeof(float));

            // read ref samples
            fread(&ref_buff_f, ref_file_wav_header.bits_per_sample / 8, BLOCK_SIZE, fp_ref);

            // -----------------------------------------------
            // FIR processing - C

            hexagon_buffer_cleaninv((uint8_t *)input_buff_f_C, BLOCK_SIZE*sizeof(float));
            hexagon_buffer_cleaninv((uint8_t *)filter_coeffs_ptr_f, NUM_FILT_TAPS*sizeof(float));
            measurement_start();
            fir_f(input_buff_f_C, delay_line_C, filter_coeffs_ptr_f, NUM_FILT_TAPS, BLOCK_SIZE, output_buff_f_C);
            measurement_finish();

            cpc_C += (double)(measurement_get_cycles());

            // calc SNR
            snr = calculate_snr_f(ref_buff_f, output_buff_f_C, BLOCK_SIZE);
            if(snr < min_snr_C)
                min_snr_C = snr;
            // -----------------------------------------------

            // -----------------------------------------------
            // FIR processing - ASM

            hexagon_buffer_cleaninv((uint8_t *)input_buff_f_ASM, BLOCK_SIZE*sizeof(float));
            hexagon_buffer_cleaninv((uint8_t *)filter_coeffs_ptr_f, NUM_FILT_TAPS*sizeof(float));
            measurement_start();
            qhdsp_fir_af(input_buff_f_ASM, delay_line_ASM, filter_coeffs_ptr_f, NUM_FILT_TAPS, BLOCK_SIZE, output_buff_f_ASM);
            measurement_finish();

            cpc_ASM += (double)(measurement_get_cycles());

            // calc SNR
            snr = calculate_snr_f(ref_buff_f, output_buff_f_ASM, BLOCK_SIZE);
            if(snr < min_snr_ASM)
                min_snr_ASM = snr;
            // -----------------------------------------------

            fwrite(&output_buff_f_C, dut_file_wav_header.bits_per_sample / 8, BLOCK_SIZE, fp_dut_C);
            fwrite(&output_buff_f_ASM, dut_file_wav_header.bits_per_sample / 8, BLOCK_SIZE, fp_dut_ASM);
        }

        cpc_C /= (double)(num_input_samples/BLOCK_SIZE);
        cpc_ASM /= (double)(num_input_samples/BLOCK_SIZE);
        printf("FIR float test/bench:\n", cpc_C);
        printf("C implementation:\n");
        printf("    [num cycles]:\t%.2lf CPC\n", cpc_C);
        printf("    [SNR]:       \t%.2lf dB\n", min_snr_C);

        printf("ASM implementation:\n");
        printf("    [num cycles]:\t%.2lf CPC\n", cpc_ASM);
        printf("    [SNR]:       \t%.2lf dB\n", min_snr_ASM);
    }
    else if(input_file_wav_header.audio_format == AUDIO_FMT_Uncompressed_PCM)
    {
        // PCM, check bit depth
        if(input_file_wav_header.bits_per_sample == 16)
        {
            // 16b PCM

            // initialize input buffers - C & ASM
            memset(input_buff_h_C, 0, (BLOCK_SIZE + NUM_FILT_TAPS)*sizeof(int16_t));
            memset(input_buff_h_ASM, 0, (BLOCK_SIZE + NUM_FILT_TAPS)*sizeof(int16_t));

            // loop:
            for (uint32_t i = 0; i < num_input_samples/BLOCK_SIZE; i++)
            {
                // read input samples into FIR buffer
                fread(&input_buff_h_C[NUM_FILT_TAPS-1], input_file_wav_header.bits_per_sample / 8, BLOCK_SIZE, fp_in);
                memcpy((void *)&input_buff_h_ASM[NUM_FILT_TAPS-1], (const void *)&input_buff_h_C[NUM_FILT_TAPS-1], BLOCK_SIZE*sizeof(int16_t));

                // read ref samples
                fread(&ref_buff_h, ref_file_wav_header.bits_per_sample / 8, BLOCK_SIZE, fp_ref);

                // -----------------------------------------------
                // FIR processing - C

                hexagon_buffer_cleaninv((uint8_t *)input_buff_h_C, BLOCK_SIZE*sizeof(int16_t));
                hexagon_buffer_cleaninv((uint8_t *)filter_coeffs_h, NUM_FILT_TAPS*sizeof(int16_t));
                measurement_start();
                fir_h(input_buff_h_C, filter_coeffs_h, NUM_FILT_TAPS, BLOCK_SIZE, output_buff_h_C);
                measurement_finish();

                cpc_C += (double)(measurement_get_cycles());

                // calc SNR
                snr = calculate_snr_h(ref_buff_h, output_buff_h_C, BLOCK_SIZE);
                if(snr < min_snr_C)
                    min_snr_C = snr;
                // -----------------------------------------------

                // -----------------------------------------------
                // FIR processing - ASM

                hexagon_buffer_cleaninv((uint8_t *)input_buff_h_ASM, BLOCK_SIZE*sizeof(int16_t));
                hexagon_buffer_cleaninv((uint8_t *)filter_coeffs_h, NUM_FILT_TAPS*sizeof(int16_t));
                measurement_start();
                qhdsp_fir_ah(input_buff_h_ASM, filter_coeffs_h, NUM_FILT_TAPS, BLOCK_SIZE, output_buff_h_ASM);
                measurement_finish();

                cpc_ASM += (double)(measurement_get_cycles());

                // calc SNR
                snr = calculate_snr_h(ref_buff_h, output_buff_h_ASM, BLOCK_SIZE);
                if(snr < min_snr_ASM)
                    min_snr_ASM = snr;
                // -----------------------------------------------

                fwrite(&output_buff_h_C, dut_file_wav_header.bits_per_sample / 8, BLOCK_SIZE, fp_dut_C);
                fwrite(&output_buff_h_ASM, dut_file_wav_header.bits_per_sample / 8, BLOCK_SIZE, fp_dut_ASM);
            }

            cpc_C /= (double)(num_input_samples/BLOCK_SIZE);
            cpc_ASM /= (double)(num_input_samples/BLOCK_SIZE);
            printf("FIR fixed point test/bench:\n", cpc_C);
            printf("C implementation:\n");
            printf("    [num cycles]:\t%.2lf CPC\n", cpc_C);
            printf("    [SNR]:       \t%.2lf dB\n", min_snr_C);

            printf("ASM implementation:\n");
            printf("    [num cycles]:\t%.2lf CPC\n", cpc_ASM);
            printf("    [SNR]:       \t%.2lf dB\n", min_snr_ASM);
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

                fwrite(&sample, dut_file_wav_header.bits_per_sample / 8, 1, fp_dut_C);
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
    fclose(fp_dut_C);
    fclose(fp_dut_ASM);
    fclose(fp_ref);

    performance_normal();

    return 0;
}
