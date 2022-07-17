/*==============================================================================
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "qurt_thread.h"
#include "qurt.h"
#include "HAP_mem.h"

uint64 HAP_mem_available_stack(void){
    int id = 0, status = -1;
    unsigned int size, sp;
    qurt_thread_attr_t thread_attr;

    asm("%0 = r29" : "=r" (sp));
    id = qurt_thread_get_id();
    if (id == QURT_EOK) {
        return AEE_EINVALIDTHREAD;
    }
    status = qurt_thread_attr_get(id, &thread_attr);
    if (status != QURT_EOK) {
        return AEE_ERESOURCENOTFOUND;
    }

    size = thread_attr.stack_size + (unsigned int)(thread_attr.stack_addr) - sp;
    return (uint64)size;
}