#ifndef HALIDE_HALIDERUNTIMEQURT_H
#define HALIDE_HALIDERUNTIMEQURT_H

// Don't include HalideRuntime.h if the contents of it were already pasted into a generated header above this one
#ifndef HALIDE_HALIDERUNTIME_H

#include "HalideRuntime.h"

#endif

#ifdef __cplusplus
extern "C" {
#endif

/** \file
 *  Routines specific to the Halide QuRT runtime.
 */

/** Lock and unlock an HVX context.
 *  A successful call to hvx_lock must be followed by a call to
 *  hvx_unlock. */
// @{
extern int halide_qurt_hvx_lock(void *user_context);
extern int halide_qurt_hvx_unlock(void *user_context);
extern void halide_qurt_hvx_unlock_as_destructor(void *user_context, void * /*obj*/);
extern void* halide_request_vtcm(void *user_context, int size, int page);
extern void halide_release_vtcm(void *user_context, void *addr);
extern void* halide_vtcm_manager_init(void *user_context);
extern void halide_vtcm_manager_destroy(void *user_context, void* vtcm_manager);
extern int halide_get_vtcm_slot(void *user_context, void* vtcm_manager);
extern int halide_free_vtcm_slot(void *user_context, void* vtcm_manager, int slot);
// @}

#ifdef __cplusplus
}  // End extern "C"
#endif

#endif  // HALIDE_HALIDERUNTIMEQURT_H
