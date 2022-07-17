#ifndef _RUNGEN_INTERFACE_H
#define _RUNGEN_INTERFACE_H
#include <AEEStdDef.h>
#include <remote.h>
#ifndef __QAIC_HEADER
#define __QAIC_HEADER(ff) ff
#endif //__QAIC_HEADER

#ifndef __QAIC_HEADER_EXPORT
#define __QAIC_HEADER_EXPORT
#endif // __QAIC_HEADER_EXPORT

#ifndef __QAIC_HEADER_ATTRIBUTE
#define __QAIC_HEADER_ATTRIBUTE
#endif // __QAIC_HEADER_ATTRIBUTE

#ifndef __QAIC_IMPL
#define __QAIC_IMPL(ff) ff
#endif //__QAIC_IMPL

#ifndef __QAIC_IMPL_EXPORT
#define __QAIC_IMPL_EXPORT
#endif // __QAIC_IMPL_EXPORT

#ifndef __QAIC_IMPL_ATTRIBUTE
#define __QAIC_IMPL_ATTRIBUTE
#endif // __QAIC_IMPL_ATTRIBUTE
#ifdef __cplusplus
extern "C" {
#endif
#if !defined(__QAIC_STRING1_OBJECT_DEFINED__) && !defined(__STRING1_OBJECT__)
#define __QAIC_STRING1_OBJECT_DEFINED__
#define __STRING1_OBJECT__
typedef struct _cstring1_s {
   char* data;
   int dataLen;
} _cstring1_t;

#endif /* __QAIC_STRING1_OBJECT_DEFINED__ */
#define LOG_OUT_FILE "rungen.output"
__QAIC_HEADER_EXPORT int __QAIC_HEADER(rungen_power_on_hvx)(void) __QAIC_HEADER_ATTRIBUTE;
__QAIC_HEADER_EXPORT int __QAIC_HEADER(rungen_power_off_hvx)(void) __QAIC_HEADER_ATTRIBUTE;
__QAIC_HEADER_EXPORT int __QAIC_HEADER(rungen_set_hvx_perf_mode_turbo)(void) __QAIC_HEADER_ATTRIBUTE;
__QAIC_HEADER_EXPORT int __QAIC_HEADER(rungen_set_hvx_perf_mode_nominal)(void) __QAIC_HEADER_ATTRIBUTE;
__QAIC_HEADER_EXPORT int __QAIC_HEADER(rungen_set_hvx_perf_mode_low)(void) __QAIC_HEADER_ATTRIBUTE;
typedef struct _rungen_str__seq_char _rungen_str__seq_char;
typedef _rungen_str__seq_char rungen_str;
struct _rungen_str__seq_char {
   char* data;
   int dataLen;
};
typedef struct _rungen_str_seq__seq_rungen_str _rungen_str_seq__seq_rungen_str;
typedef _rungen_str_seq__seq_rungen_str rungen_str_seq;
struct _rungen_str_seq__seq_rungen_str {
   rungen_str* data;
   int dataLen;
};
__QAIC_HEADER_EXPORT int __QAIC_HEADER(rungen_main)(const rungen_str* str, int strLen) __QAIC_HEADER_ATTRIBUTE;
#ifdef __cplusplus
}
#endif
#endif //_RUNGEN_INTERFACE_H
