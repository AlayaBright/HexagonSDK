/**=============================================================================
Copyright (c) 2020 QUALCOMM Technologies Incorporated.
All Rights Reserved Qualcomm Proprietary
=============================================================================**/

#include "hap_example.h"

#include "remote.h"

#include "HAP_farf.h"

#include <stdlib.h>

#include "AEEStdErr.h"

/*
 * Structures and Global Variables
 */


/* Placeholder structure to create a unique handle for a FastRPC session */
typedef struct {
    int element;
} hap_example_context_t;



/*
 * Function Declarations
 */

AEEResult hap_example_open(const char *uri, remote_handle64 *h)
{
    /*
     * Whenever the hap_example_open is called by the APPS process, FastRPC generates a unique apps-side handle
     * and remembers the mapping from the apps-side handle to the DSP handle *h that we assign here.
     */

    hap_example_context_t *hap_example_ctx = malloc(sizeof(hap_example_context_t));

    if(hap_example_ctx==NULL)
    {
        FARF(ERROR, "Failed to allocate memory for the hap_example context");
        return AEE_ENOMEMORY;
    }

    *h = (remote_handle64) hap_example_ctx;

    return 0;
}



AEEResult hap_example_close(remote_handle64 h)
{
    hap_example_context_t *hap_example_ctx = (hap_example_context_t*) h;

    if(hap_example_ctx==NULL)
    {
        FARF(ERROR, "NULL handle received in hap_example_close()");
        return AEE_EBADHANDLE;
    }

    free(hap_example_ctx);

    return 0;
}

