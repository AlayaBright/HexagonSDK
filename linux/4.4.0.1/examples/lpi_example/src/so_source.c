/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 */

#include "HAP_farf.h"
#include <dlfcn.h>
#include "qurt_memory.h"

int main(int argc, char *argv[])
{
    int err = 0;
    int (*main_ptr)(void) = NULL;
    Dl_info so_info;
    FARF(ALWAYS, "Successfully loaded shared object");

    /* dlsym() with the special handle NULL is interpreted as a reference
       to the shared object from which the call is being made*/
    main_ptr = (int (*)(void))dlsym(NULL, "main");

    /* dladdr() returns information about a shared object containing a given address.
       Using dladdr() below to get the load address of the shared object by providing main symbol address */
    err = dladdr(main_ptr, (Dl_info *)&so_info);
    if(err) {
        qurt_paddr_64_t paddr = NULL;
        // convert virtual address to physical address
        paddr = qurt_lookup_physaddr_64((qurt_addr_t) so_info.dli_fbase);
	if(paddr != 0){
            FARF(ALWAYS, "load address of shared object is 0x%x\n",paddr);
	}
    }
   return 0;
}
