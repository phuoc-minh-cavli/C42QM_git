#include "../config/devcfg_elite.h"
typedef long _Int32t;
typedef unsigned long _Uint32t;
typedef int _Ptrdifft;
typedef unsigned int _Sizet;
typedef __builtin_va_list va_list;
typedef long long _Longlong;
typedef unsigned long long _ULonglong;
typedef int _Wchart;
typedef int _Wintt;
typedef va_list _Va_list;
void _Atexit(void (*)(void));
typedef char _Sysch_t;
void _Locksyslock(int);
void _Unlocksyslock(int);
typedef unsigned short __attribute__((__may_alias__)) alias_short;
static alias_short *strict_aliasing_workaround(unsigned short *ptr) __attribute__((always_inline,unused));
static alias_short *strict_aliasing_workaround(unsigned short *ptr)
{
  alias_short *aliasptr = (alias_short *)ptr;
  return aliasptr;
}
typedef signed char int8_t;
typedef short int16_t;
typedef _Int32t int32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef _Uint32t uint32_t;
typedef signed char int_least8_t;
typedef short int_least16_t;
typedef _Int32t int_least32_t;
typedef unsigned char uint_least8_t;
typedef unsigned short uint_least16_t;
typedef _Uint32t uint_least32_t;
typedef signed char int_fast8_t;
typedef short int_fast16_t;
typedef _Int32t int_fast32_t;
typedef unsigned char uint_fast8_t;
typedef unsigned short uint_fast16_t;
typedef _Uint32t uint_fast32_t;
typedef unsigned int uintptr_t;
typedef int intptr_t;
typedef _Longlong int64_t;
typedef _ULonglong uint64_t;
typedef _Longlong int_least64_t;
typedef _ULonglong uint_least64_t;
typedef _Longlong int_fast64_t;
typedef _ULonglong uint_fast64_t;
typedef _Longlong intmax_t;
typedef _ULonglong uintmax_t;
typedef char char_t;
typedef unsigned char bool_t;
typedef struct {
    uint32_t size_lpa_heap_from_ddr;
} lpa_prop_struct_t;
const lpa_prop_struct_t lpa_prop = {0};
