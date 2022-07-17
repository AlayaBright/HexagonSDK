/*==============================================================================
  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#ifndef UIMG_DL_VER_MAJOR
#define UIMG_DL_VER_MAJOR 2
#endif
#ifndef UIMG_DL_VER_MINOR
#define UIMG_DL_VER_MINOR 0
#endif
#ifndef UIMG_DL_VER_MAINT
#define UIMG_DL_VER_MAINT 0
#endif

#define __TOSTR(_x) #_x
#define _TOSTR(_x) __TOSTR(_x)

typedef struct note_type{
  int sizename;
  int sizedesc;
  int type;
  char name[24];
  int desc[3];
} note_type;

#ifdef __llvm__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#endif

/*put constant string "uimg.dl.ver.2.0.0" in the new section created using uimage_v2.lcs*/
const note_type uimg_dl_ver __attribute__ ((section (".note.qti.uimg.dl.ver")))
                            __attribute__ ((visibility ("default"))) = {
  24,
  12,
  0,
  "uimg.dl.ver." _TOSTR(UIMG_DL_VER_MAJOR) "." _TOSTR(UIMG_DL_VER_MINOR) "." _TOSTR(UIMG_DL_VER_MAINT),
  {UIMG_DL_VER_MAJOR, UIMG_DL_VER_MINOR, UIMG_DL_VER_MAINT}
};

#ifdef __llvm__
#pragma clang diagnostic pop
#endif


