/**=============================================================================
@file
    qhl_common.h

@brief
    Common datatypes and macros used in QHL libraries.

Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#ifndef _QHL_COMMON_H
#define _QHL_COMMON_H

#include <stdint.h>

#ifdef RUN_ON_ADSP
#   define NR_OF_ITERATIONS 10000
#elif defined(RUN_ON_ARM)
#   define NR_OF_ITERATIONS 10000
#else
#   define NR_OF_ITERATIONS 100
#endif

#define ARR_LEN 1024
#define TOL 0.0001f
#define MATRIX_N 3
#define NUMBER_OF_MATRIX_ITERATIONS 1000
#define VEC_LEN 10
#define VEC_MAX_ARR_LEN 100000

typedef struct
{
  __fp16 lo;  // Lowest value
  __fp16 hi;  // Highest value
} range_float16_t;


typedef struct
{
  float lo;  // Lowest value
  float hi;  // Highest value
} range_float_t;

typedef struct
{
  int8_t lo;  // Lowest value
  int8_t hi;  // Highest value
} range_int8_t;

typedef struct
{
  int16_t lo;  // Lowest value
  int16_t hi;  // Highest value
} range_int16_t;

typedef struct
{
    uint16_t lo;  // Lowest value
    uint16_t hi;  // Highest value
} range_uint16_t;

typedef struct
{
  int32_t lo;  // Lowest value
  int32_t hi;  // Highest value
} range_int32_t;

typedef struct
{
  uint32_t lo;  // Lowest value
  uint32_t hi;  // Highest value
} range_uint32_t;

#define TCALL(FNAME) test_##FNAME(NR_OF_ITERATIONS)

#if __hexagon__
#define _F_COMPLEX_(re, im) ((float complex){re, im})
#else
#define _F_COMPLEX_(re, im) (re+  im*_Complex_I)
#endif

#if __hexagon__
#define ABSF(x) 	qhmath_abs_f( (x) )
#else
#define ABSF(x) 	fabsf( (x) )
#endif

//------------------------------------------------------------------
//------------------------------------------------------------------
//------------------------------------------------------------------

#define CYCLES_PER_CALL(NR_TRIES, NR_CYCLES) ((double) (NR_CYCLES)) / ((double) (NR_TRIES))

typedef struct measurement_t
{
    uint64_t start;
    uint64_t stop;
    uint64_t elapsed;
} measurement_t;

static measurement_t upcycle;
static measurement_t utimer;

#ifdef RUN_ON_ADSP

inline static uint64_t read_pcycles(void)
{
    uint64_t pcyc;
    __asm__ __volatile__ ( " %0 = UPCYCLE \n" : "=r"(pcyc));
    return pcyc;
}

inline static uint64_t read_timer(void)
{
    uint64_t ret;
    __asm__ __volatile__ ( " %0 = UTIMER \n" : "=r"(ret));
    return ret;
}

static const double tick_timer_freq_MHz = (double) 19.2;

#elif defined(RUN_ON_ARM)

#include <sys/resource.h>

inline static uint64_t read_pcycles(void)
{
    uint64_t pcyc = 0L;
    return pcyc;
}

inline static uint64_t read_timer(void)
{
    uint64_t ret;
    struct rusage _rusage;

    getrusage(RUSAGE_SELF, &_rusage);

    ret = 1e6 * (_rusage.ru_utime.tv_sec + _rusage.ru_stime.tv_sec)
        + _rusage.ru_utime.tv_usec + _rusage.ru_stime.tv_usec;

    return ret;
}

static const double tick_timer_freq_MHz = (double) 1.0;

#else // RUN_ON_SIM
#   include <hexagon_sim_timer.h>
#   define read_pcycles() hexagon_sim_read_pcycles()

inline static uint64_t read_timer(void)
{
    return 0;
}

static const double tick_timer_freq_MHz = (double) 0.5; // 500 MHz

#endif // RUN_ON_ADSP


//------------------------------------------------------------------
//------------------------------------------------------------------
//------------------------------------------------------------------

#ifdef RUN_ON_ADSP

#include "AEEStdDef.h"
#include "HAP_power.h"
#include <string.h>

static HAP_power_request_t request;

static int __attribute__((unused)) performance_high(void)
{
    int retVal;

    memset(&request, 0, sizeof(HAP_power_request_t)); //Important to clear the structure if only selected fields are updated.

    request.type = HAP_power_set_DCVS_v2;

    request.dcvs_v2.dcvs_enable = TRUE;

    request.dcvs_v2.dcvs_option = HAP_DCVS_V2_PERFORMANCE_MODE;

    request.dcvs_v2.set_latency = TRUE;

    request.dcvs_v2.latency = 100;

    request.dcvs_v2.set_dcvs_params = TRUE;

    request.dcvs_v2.dcvs_params.min_corner = HAP_DCVS_VCORNER_MAX;

    request.dcvs_v2.dcvs_params.max_corner = HAP_DCVS_VCORNER_MAX;

    request.dcvs_v2.dcvs_params.target_corner = HAP_DCVS_VCORNER_MAX;

    /* Call HAP_power_set API with the updated request structure */

    retVal = HAP_power_set(NULL, &request);

    return retVal;
}

static int __attribute__((unused)) performance_normal(void)
{
    int retVal;

    //To remove the vote keeping DCVS enabled.

    memset(&request, 0, sizeof(HAP_power_request_t)); //Remove all votes.

    request.type = HAP_power_set_DCVS_v2;

    request.dcvs_v2.dcvs_enable = TRUE;

    request.dcvs_v2.dcvs_option = HAP_DCVS_V2_PERFORMANCE_MODE; //Enable DCVS

    retVal = HAP_power_set(NULL, &request);

    return retVal;
}

#else // RUN_ON_ADSP

static inline int __attribute__((unused)) performance_high(void) { return 0; }
static inline int __attribute__((unused)) performance_normal(void) { return 0; }

#endif // RUN_ON_ADSP

//------------------------------------------------------------------
//------------------------------------------------------------------
//------------------------------------------------------------------

inline static void measurement_start(void)
{
    upcycle.start = read_pcycles();
    utimer.start = read_timer();
}

inline static void measurement_finish(void)
{
    upcycle.stop = read_pcycles();
    utimer.stop = read_timer();

    upcycle.elapsed = upcycle.stop - upcycle.start;
    utimer.elapsed = utimer.stop - utimer.start;
}

inline static uint64_t measurement_get_cycles(void)
{
    return upcycle.elapsed;
}

inline static double measurement_get_us(void)
{
    double us = utimer.elapsed;
    us /= tick_timer_freq_MHz;

    return us;
}

inline static double measurement_get_cpc(uint64_t funct_calls)
{
    return CYCLES_PER_CALL(funct_calls, measurement_get_cycles());
}

inline static double measurement_get_mps(uint64_t funct_calls)
{
    double mps = (double) funct_calls;
    mps /= measurement_get_us();

    return mps;
}

static double cpc = 0.0;
static double mps = 0.0;

inline static void measurement_acc_clr(void)
{
    cpc = 0.0;
    mps = 0.0;
}

inline static void measurement_acc(uint64_t nr_of_iterations)
{
    cpc += measurement_get_cpc(nr_of_iterations);
    mps += measurement_get_mps(nr_of_iterations);
}

inline static void measurement_acc_prn(const char *fname, uint64_t range_cnt, uint64_t arr_len)
{
    cpc /= (double) range_cnt; // average
    mps /= (double) range_cnt; // average

    double cpcpm = cpc / (double) arr_len;
    double mpspm = mps * (double) arr_len;

    printf("  %-8s:  %7.4f CPC | %7.4f MPS (per member)"
        " || whole array (size= %lld)= %7.4f CPC | %7.4f MPS\n",
        fname, cpcpm, mpspm, arr_len, cpc, mps);
}

inline static void measurement_print(void)
{
    printf("    [num cycles]:\t%llu CPC | %7.4f MPS << divide by nr of exec. \n", measurement_get_cycles(), measurement_get_mps(1));
}

/** Calculate cycles per call and print (CPC) */
inline static void measurement_print_f(const char *fname, uint64_t n, uint64_t arr_len)
{
    double nr_tries = n * arr_len;

    printf("  %-8s:  %7.4f CPC | %7.4f MPS\n",
            fname,
            measurement_get_cpc(nr_tries),
            measurement_get_mps(nr_tries));
}

/** Calculate both CPC and million execution per second (MPS) and print */
inline static void measurement_print_af(const char *fname, uint64_t n, uint64_t arr_len)
{
    double cpc = measurement_get_cpc(n);
    double cpm = cpc / (double) arr_len;
    printf("  %-8s:  %7.4f CPC | %7.4f CPM | %7.4f MPS\n",
            fname,
            cpc,
            cpm,
            measurement_get_mps(n));
}

#endif //_QHL_COMMON_H
