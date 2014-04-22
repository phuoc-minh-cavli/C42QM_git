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
typedef int ADSPResult;
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
ADSPResult elite_thrd_prio_init(void);
uint32_t elite_get_thrd_prio(elite_thrd_prio_id_t thrd_prio_id);
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
