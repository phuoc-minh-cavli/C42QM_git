# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/elite/utils/devcfg/config/elite_devcfg_8909.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 328 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/elite/utils/devcfg/config/elite_devcfg_8909.c" 2
# 20 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/elite/utils/devcfg/config/elite_devcfg_8909.c"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/elite/utils/inc/elite_thrd_prio.h" 1
# 26 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/elite/utils/inc/elite_thrd_prio.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/mmutils/mmdefs.h" 1
# 48 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/mmutils/mmdefs.h"
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
# 49 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/mmutils/mmdefs.h" 2
# 84 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/mmutils/mmdefs.h"
typedef char char_t;
typedef unsigned char bool_t;
# 27 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/elite/utils/inc/elite_thrd_prio.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_error_codes.h" 1
# 90 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_error_codes.h"
typedef int ADSPResult;
# 28 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/elite/utils/inc/elite_thrd_prio.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/elite/utils/devcfg/inc/elite_thrd_prio_dev_cfg.h" 1
# 36 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/elite/utils/devcfg/inc/elite_thrd_prio_dev_cfg.h"
typedef enum elite_thrd_prio_id_t
{
  ELITETHREAD_STAT_AFE_PORT_MANAGER_PRIO_ID = 0,
  ELITETHREAD_STAT_AFE_FBSP_EXCUR_PRIO_ID,
  ELITETHREAD_AVTIMER_SERVER_PRIO_ID,
  ELITETHREAD_DEFAULT_IST_PRIO_ID,
  ELITETHREAD_DYNA_AFE_INTERNAL_BT_PRIO_ID,
  ELITETHREAD_STAT_AFE_VFR_PRIO_ID,
  ELITETHREAD_STAT_VOICE_TIMER_SVC_PRIO_ID,
  ELITETHREAD_STAT_VOICE_DELIVERY_SVC_PRIO_ID,
  ELITETHREAD_MVM_HIGH_PRIO_ID,
  ELITETHREAD_DYNA_AFE_INTERNAL_FM_PRIO_ID,
  ELITETHREAD_STAT_VOICE_MIXER_SVC_PRIO_ID,
  ELITETHREAD_STAT_AFE_DEV_SVC_PRIO_ID,
  ELITETHREAD_STAT_VOICE_STREAM_SVC_PRIO_ID,
  ELITETHREAD_STAT_AUDIO_STREAM_SVC_PRIO_ID,
  ELITETHREAD_STAT_AUDIO_DEV_SVC_PRIO_ID,
  ELITETHREAD_STAT_ADSP_CORE_SVC_PRIO_ID,
  ELITETHREAD_STAT_AFE_CDC_INT_SVC_PRIO_ID,
  ELITETHREAD_DYNA_MIXER_SVC_PRIO_ID,
  ELITETHREAD_DYNA_MTMX_PSPD_SVC_PRIO_ID,
  ELITETHREAD_DYNA_STREAM_ROUTER_SVC_PRIO_ID,
  ELITETHREAD_MVM_MED_PRIO_ID,
  ELITETHREAD_CVD_VFR_PRIO_ID,
  ELITETHREAD_CVP_LOW_PRIO_ID,
  ELITETHREAD_CVS_MAILBOX_TIMER_PRIO_ID,
  ELITETHREAD_DYNA_LL_PP_SVC_PRIO_ID,
  ELITETHREAD_DYNA_LL_DEC_SVC_PRIO_ID,
  ELITETHREAD_DYNA_LL_ENC_SVC_PRIO_ID,
  ELITETHREAD_DYNA_VOICE_DEC_PRIO_ID,
  ELITETHREAD_DYNA_VOICE_PROC_RX_PRIO_ID,
  ELITETHREAD_DYNA_VOICE_PROC_TX_PRIO_ID,
  ELITETHREAD_MVM_LOW_PRIO_ID,
  ELITETHREAD_DYNA_VOICE_ENC_PRIO_ID,
  ELITETHREAD_DYNA_PP_SVC_PRIO_ID,
  ELITETHREAD_DYNA_DEC_SVC_PRIO_ID,
  ELITETHREAD_DYNA_SWITCH_SVC_PRIO_ID,
  ELITETHREAD_DYNA_ENC_SVC_PRIO_ID,
  ELITETHREAD_DYNA_LSM_SVC_PRIO_ID,
  ELITETHREAD_AUDENCDEC_TEST_PRIO_ID,
  ELITETHREAD_COMMON_TEST_PRIO_ID,
  ELITETHREAD_AMDB_THREADPOOL_PRIO_ID,
  ELITETHREAD_STAT_LSM_SVC_PRIO_ID,
  ELITETHREAD_STAT_AFE_SW_MAD_PRIO_ID,
  ELITETHREAD_STAT_AFE_AANC_PRIO_ID,
  ELITETHREAD_STAT_AFE_SW_DUTY_CYCLE_PRIO_ID,
  ELITETHREAD_CVD_CAL_LOGGER_PRIO_ID,
  ELITETHREAD_THRD_PRIO_ID_MAX
}elite_thrd_prio_id_t;


typedef struct elite_thrd_prio_struct_t
{
  elite_thrd_prio_id_t id;


  uint32_t prio;


}elite_thrd_prio_struct_t;



typedef struct elite_thrd_prio_prop_struct_t
{
  uint32_t num_threads;

  elite_thrd_prio_struct_t *thrd_prio_ptr;

} elite_thrd_prio_prop_struct_t;
# 29 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/elite/utils/inc/elite_thrd_prio.h" 2
# 48 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/elite/utils/inc/elite_thrd_prio.h"
ADSPResult elite_thrd_prio_init(void);
# 61 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/elite/utils/inc/elite_thrd_prio.h"
uint32_t elite_get_thrd_prio(elite_thrd_prio_id_t thrd_prio_id);
# 21 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/elite/utils/devcfg/config/elite_devcfg_8909.c" 2
# 86 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/elite/utils/devcfg/config/elite_devcfg_8909.c"
static elite_thrd_prio_struct_t thrd_priorities[] =
{
    {ELITETHREAD_STAT_AFE_PORT_MANAGER_PRIO_ID, (255 - 78)},
    {ELITETHREAD_STAT_AFE_FBSP_EXCUR_PRIO_ID, (255 - 164)},
    {ELITETHREAD_AVTIMER_SERVER_PRIO_ID, (255 - 64)},
    {ELITETHREAD_DEFAULT_IST_PRIO_ID, (255 - 65)},
    {ELITETHREAD_DYNA_AFE_INTERNAL_BT_PRIO_ID, (255 - 40)},
    {ELITETHREAD_STAT_AFE_VFR_PRIO_ID, (255 - 71)},
    {ELITETHREAD_STAT_VOICE_TIMER_SVC_PRIO_ID, (255 - 74)},
    {ELITETHREAD_STAT_VOICE_DELIVERY_SVC_PRIO_ID, (255 - 74)},
    {ELITETHREAD_MVM_HIGH_PRIO_ID, (255 - 120)},
    {ELITETHREAD_DYNA_AFE_INTERNAL_FM_PRIO_ID, (255 - 78)},
    {ELITETHREAD_STAT_VOICE_MIXER_SVC_PRIO_ID, (255 - 79)},
    {ELITETHREAD_STAT_AFE_DEV_SVC_PRIO_ID, (255 - 118)},
    {ELITETHREAD_STAT_VOICE_STREAM_SVC_PRIO_ID, (255 - 80)},
    {ELITETHREAD_STAT_AUDIO_STREAM_SVC_PRIO_ID, (255 - 118)},
    {ELITETHREAD_STAT_AUDIO_DEV_SVC_PRIO_ID, (255 - 118)},
    {ELITETHREAD_STAT_ADSP_CORE_SVC_PRIO_ID, (255 - 118)},
    {ELITETHREAD_STAT_AFE_CDC_INT_SVC_PRIO_ID, (255 - 83)},
    {ELITETHREAD_DYNA_MIXER_SVC_PRIO_ID, (255 - 83)},
    {ELITETHREAD_DYNA_MTMX_PSPD_SVC_PRIO_ID, (255 - 38)},
    {ELITETHREAD_DYNA_STREAM_ROUTER_SVC_PRIO_ID, (255 - 83)},
    {ELITETHREAD_MVM_MED_PRIO_ID, (255 - 120)},
    {ELITETHREAD_CVD_VFR_PRIO_ID, (255 - 51)},
    {ELITETHREAD_CVP_LOW_PRIO_ID, (255 - 119)},
    {ELITETHREAD_CVS_MAILBOX_TIMER_PRIO_ID, (255 - 82)},
    {ELITETHREAD_DYNA_VOICE_DEC_PRIO_ID, (255 - 120)},
    {ELITETHREAD_DYNA_VOICE_PROC_RX_PRIO_ID, (255 - 121)},
    {ELITETHREAD_DYNA_VOICE_PROC_TX_PRIO_ID, (255 - 123)},
    {ELITETHREAD_MVM_LOW_PRIO_ID, (255 - 120)},
    {ELITETHREAD_DYNA_VOICE_ENC_PRIO_ID, (255 - 122)},
    {ELITETHREAD_DYNA_LL_PP_SVC_PRIO_ID, (255 - 87)},
    {ELITETHREAD_DYNA_LL_DEC_SVC_PRIO_ID, (255 - 87)},
    {ELITETHREAD_DYNA_LL_ENC_SVC_PRIO_ID, (255 - 87)},
    {ELITETHREAD_DYNA_PP_SVC_PRIO_ID, (255 - 160)},
    {ELITETHREAD_DYNA_DEC_SVC_PRIO_ID, (255 - 161)},
    {ELITETHREAD_DYNA_SWITCH_SVC_PRIO_ID, (255 - 161)},
    {ELITETHREAD_DYNA_ENC_SVC_PRIO_ID, (255 - 161)},
    {ELITETHREAD_DYNA_LSM_SVC_PRIO_ID, (255 - 161)},
    {ELITETHREAD_AUDENCDEC_TEST_PRIO_ID, (255 - 69)},
    {ELITETHREAD_COMMON_TEST_PRIO_ID, (255 - 69)},
    {ELITETHREAD_AMDB_THREADPOOL_PRIO_ID, (255 - 69)},
    {ELITETHREAD_STAT_LSM_SVC_PRIO_ID, (255 - 163)},
    {ELITETHREAD_STAT_AFE_SW_MAD_PRIO_ID, (255 - 164)},
    {ELITETHREAD_STAT_AFE_AANC_PRIO_ID, (255 - 164)},
    {ELITETHREAD_STAT_AFE_SW_DUTY_CYCLE_PRIO_ID, (255 - 164)},
    {ELITETHREAD_CVD_CAL_LOGGER_PRIO_ID, (255 - 109)}
};




elite_thrd_prio_prop_struct_t elite_thrd_prio_prop =
{
    (sizeof(thrd_priorities)/sizeof(thrd_priorities[0])), thrd_priorities
};
