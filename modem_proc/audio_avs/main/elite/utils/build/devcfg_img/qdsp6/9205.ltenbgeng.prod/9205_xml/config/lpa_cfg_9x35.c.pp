# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00388-9205_LTENBGE_PACK-1_20230727_073504/b/modem_proc/audio_avs/main/elite/utils/devcfg/config/lpa_cfg_9x35.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 328 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00388-9205_LTENBGE_PACK-1_20230727_073504/b/modem_proc/audio_avs/main/elite/utils/devcfg/config/lpa_cfg_9x35.c" 2
# 26 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00388-9205_LTENBGE_PACK-1_20230727_073504/b/modem_proc/audio_avs/main/elite/utils/devcfg/config/lpa_cfg_9x35.c"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00388-9205_LTENBGE_PACK-1_20230727_073504/b/modem_proc/audio_avs/main/elite/utils/devcfg/inc/lpa_prop.h" 1
# 26 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00388-9205_LTENBGE_PACK-1_20230727_073504/b/modem_proc/audio_avs/main/elite/utils/devcfg/inc/lpa_prop.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00388-9205_LTENBGE_PACK-1_20230727_073504/b/modem_proc/audio_avs/main/api/mmutils/mmdefs.h" 1
# 48 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00388-9205_LTENBGE_PACK-1_20230727_073504/b/modem_proc/audio_avs/main/api/mmutils/mmdefs.h"
# 1 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/stdint.h" 1 3 4




# 1 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/yvals.h" 1 3 4
# 297 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/yvals.h" 3 4
typedef long _Int32t;
typedef unsigned long _Uint32t;
# 308 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/yvals.h" 3 4
typedef int _Ptrdifft;
# 318 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/yvals.h" 3 4
typedef unsigned int _Sizet;
# 680 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/yvals.h" 3 4
# 1 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/stdarg.h" 1 3 4
# 12 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/stdarg.h" 3 4
typedef __builtin_va_list va_list;
# 681 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/yvals.h" 2 3 4
# 790 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/yvals.h" 3 4
typedef long long _Longlong;
typedef unsigned long long _ULonglong;
# 854 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/yvals.h" 3 4
typedef int _Wchart;
typedef int _Wintt;
# 884 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/yvals.h" 3 4
typedef va_list _Va_list;
# 906 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/yvals.h" 3 4
void _Atexit(void (*)(void));
# 919 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/yvals.h" 3 4
typedef char _Sysch_t;
# 939 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/yvals.h" 3 4
void _Locksyslock(int);
void _Unlocksyslock(int);
# 1082 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/yvals.h" 3 4
typedef unsigned short __attribute__((__may_alias__)) alias_short;
static alias_short *strict_aliasing_workaround(unsigned short *ptr) __attribute__((always_inline,unused));

static alias_short *strict_aliasing_workaround(unsigned short *ptr)
{
  alias_short *aliasptr = (alias_short *)ptr;
  return aliasptr;
}
# 6 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/stdint.h" 2 3 4
# 28 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/stdint.h" 3 4
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
# 59 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/stdint.h" 3 4
typedef signed char int_fast8_t;
typedef short int_fast16_t;
typedef _Int32t int_fast32_t;

typedef unsigned char uint_fast8_t;
typedef unsigned short uint_fast16_t;
typedef _Uint32t uint_fast32_t;
# 81 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/stdint.h" 3 4
typedef unsigned int uintptr_t;
# 100 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/stdint.h" 3 4
typedef int intptr_t;
# 115 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/stdint.h" 3 4
typedef _Longlong int64_t;



typedef _ULonglong uint64_t;

typedef _Longlong int_least64_t;
typedef _ULonglong uint_least64_t;
# 133 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/stdint.h" 3 4
typedef _Longlong int_fast64_t;
typedef _ULonglong uint_fast64_t;




typedef _Longlong intmax_t;
typedef _ULonglong uintmax_t;
# 49 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00388-9205_LTENBGE_PACK-1_20230727_073504/b/modem_proc/audio_avs/main/api/mmutils/mmdefs.h" 2
# 84 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00388-9205_LTENBGE_PACK-1_20230727_073504/b/modem_proc/audio_avs/main/api/mmutils/mmdefs.h"
typedef char char_t;
typedef unsigned char bool_t;
# 27 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00388-9205_LTENBGE_PACK-1_20230727_073504/b/modem_proc/audio_avs/main/elite/utils/devcfg/inc/lpa_prop.h" 2







typedef struct {

    uint32_t size_lpa_heap_from_ddr;

} lpa_prop_struct_t;
# 27 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00388-9205_LTENBGE_PACK-1_20230727_073504/b/modem_proc/audio_avs/main/elite/utils/devcfg/config/lpa_cfg_9x35.c" 2


const lpa_prop_struct_t lpa_prop = {0};
