/*==============================================================================
  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/
#ifndef RPCPERF_H
#define RPCPERF_H

int rpcperf_noop(remote_handle64 handle, int cnt);
int rpcperf_inbuf(remote_handle64 handle, int uncached, int size, int ion, int mem_test, int cnt);
int rpcperf_routbuf(remote_handle64 handle, int uncached, int size, int ion, int mem_test, int cnt);
int rpcperf_default(remote_handle64 handle, int ion);

#endif    // RPCPERF_H
