/**=============================================================================
@file
    time_utils.c

@brief
    Definitions of wrapper functions used to get time on different platforms.

Copyright (c) 2021 Qualcomm Technologies Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
=============================================================================**/

#include "time_utils.h"

#ifdef _WINDOWS

    // This function gets the time on windows target in microseconds
    unsigned long long get_time()
    {
        LARGE_INTEGER read_time, time_in_usec;
        LARGE_INTEGER freq;

        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&read_time);
        time_in_usec.QuadPart = (read_time.QuadPart * 1000000)/freq.QuadPart;
        return time_in_usec.QuadPart;
    }

    void sleep_in_microseconds(unsigned long long interval)
    {
        unsigned long long time1 = 0, time2 = 0, freq = 0;

        QueryPerformanceCounter((LARGE_INTEGER *) &time1);
        QueryPerformanceFrequency((LARGE_INTEGER *)&freq);

        do {
            QueryPerformanceCounter((LARGE_INTEGER *) &time2);
        } while((time2-time1) < interval);
    }

#else
#ifndef __hexagon__

    // This function gets the time on non-windows target in microseconds
    unsigned long long get_time()
    {
        struct timeval tv;

        gettimeofday(&tv, 0);

        return (tv.tv_sec * 1000000ULL + tv.tv_usec);
    }

#endif
#endif
