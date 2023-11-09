# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/common/cvd/cvd_devcfg/mdm9x07/cvd_devcfg.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 328 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/common/cvd/cvd_devcfg/mdm9x07/cvd_devcfg.c" 2
# 10 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/common/cvd/cvd_devcfg/mdm9x07/cvd_devcfg.c"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 1
# 38 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
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
# 39 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 1
# 82 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
typedef struct voice_param_data_t voice_param_data_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 85 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2







struct voice_param_data_t
{
   uint32_t module_id;




   uint32_t param_id;




   uint16_t param_size;





   uint16_t reserved;







}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 120 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2
;




typedef struct voice_param_data_v2_t voice_param_data_v2_t;







struct voice_param_data_v2_t
{
   uint32_t module_id;




   uint16_t instance_id;




   uint16_t reserved;


   uint32_t param_id;




   uint32_t param_size;
# 165 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 167 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2
;





typedef struct voice_set_param_v2_t voice_set_param_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 176 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2
# 185 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
struct voice_set_param_v2_t
{
   uint32_t payload_address_lsw;



   uint32_t payload_address_msw;
# 205 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
   uint32_t payload_size;






   uint32_t mem_map_handle;
# 227 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 229 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2
;
# 256 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
typedef struct voice_get_param_v2_t voice_get_param_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 259 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2
# 268 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
struct voice_get_param_v2_t
{
   uint32_t payload_address_lsw;



   uint32_t payload_address_msw;
# 289 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
   uint32_t module_id;




   uint32_t param_id;




   uint16_t param_max_size;







   uint16_t reserved;


   uint32_t mem_map_handle;
# 322 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 324 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2
;
# 355 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
typedef struct voice_get_param_ack_t voice_get_param_ack_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 358 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2
# 368 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
struct voice_get_param_ack_t
{
   uint32_t status;
# 380 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 382 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2
;
# 417 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
typedef enum
{
   VFR_NONE = 0,
   VFR_HARD,
   VFR_HARD_EXT,
   VFR_MODE_END
} voice_vfr_mode_t;





typedef struct voice_set_timing_params_t voice_set_timing_params_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 432 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2






struct voice_set_timing_params_t
{
   uint16_t mode;
# 450 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
   uint16_t enc_offset;
# 461 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
   uint16_t dec_req_offset;
# 470 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
   uint16_t dec_offset;
# 480 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 482 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2
;
# 544 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
typedef struct voice_tap_point_v2_t voice_tap_point_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 547 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2






struct voice_tap_point_v2_t
{
  uint32_t tap_point;
# 564 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
  uint16_t direction;
# 579 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
  uint16_t sampling_rate;





  uint16_t duration_ms;




  uint16_t reserved;


   uint32_t mem_map_handle;






}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 602 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2
;



typedef struct voice_start_host_pcm_v2_t voice_start_host_pcm_v2_t;



# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 610 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2






struct voice_start_host_pcm_v2_t
{

  uint32_t num_tap_points;





  voice_tap_point_v2_t tap_points[ 1 ];




}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 632 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2
;
# 779 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
typedef struct voice_evt_push_host_pcm_buf_v2_t voice_evt_push_host_pcm_buf_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 782 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2






struct voice_evt_push_host_pcm_buf_v2_t
{

   uint32_t tap_point;
# 800 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
   uint32_t mask;
# 811 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
   uint32_t rd_buff_addr_lsw;





   uint32_t rd_buff_addr_msw;
# 826 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
   uint32_t wr_buff_addr_lsw;





   uint32_t wr_buff_addr_msw;
# 841 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
   uint16_t rd_buff_size;





   uint16_t wr_buff_size;



   uint16_t sampling_rate;
# 860 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
   uint16_t wr_num_chan;





}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 868 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2
;



typedef struct voice_evt_notify_host_pcm_buf_v2_t voice_evt_notify_host_pcm_buf_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 875 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2







struct voice_evt_notify_host_pcm_buf_v2_t
{

   uint32_t tap_point;
# 894 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
   uint32_t mask;
# 906 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
   uint32_t rd_buff_addr_lsw;





   uint32_t rd_buff_addr_msw;
# 924 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
   uint32_t wr_buff_addr_lsw;




   uint32_t wr_buff_addr_msw;
# 940 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
   uint16_t rd_buff_size;
# 949 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
   uint16_t wr_buff_size;
# 958 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
   uint16_t sampling_rate;
# 967 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
   uint16_t rd_num_chan;






   uint16_t wr_num_chan;




   uint16_t reserved;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 983 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2
;
# 1043 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
typedef enum
{
   VOICE_SET_MUTE_TX_ONLY = 0,
   VOICE_SET_MUTE_RX_ONLY,
   VOICE_SET_MUTE_TX_AND_RX
} voice_mute_path_t;



typedef enum
{
   VOICE_UNMUTE = 0,
   VOICE_MUTE
} voice_mute_status_t;





typedef struct voice_set_soft_mute_v2_t voice_set_soft_mute_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 1065 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2






struct voice_set_soft_mute_v2_t
{
   uint16_t direction;







   uint16_t mute;
# 1091 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
   uint16_t ramp_duration;






   uint16_t reserved;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 1102 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2
;
# 1136 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
typedef enum
{
   VOICE_PHYSICAL_SHARED_MEM_ADDR =0,



   VOICE_VIRTUAL_SHARED_MEM_ADDR,



} voice_mem_map_property_flag_t;



typedef struct voice_cmd_shared_mem_map_regions_t voice_cmd_shared_mem_map_regions_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 1153 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2








struct voice_cmd_shared_mem_map_regions_t
{
    uint16_t mem_pool_id;
# 1175 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
    uint16_t num_regions;


    uint32_t property_flag;
# 1198 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 1200 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2
;



# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 1204 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2


typedef struct voice_shared_map_region_payload_t voice_shared_map_region_payload_t;






struct voice_shared_map_region_payload_t
{
    uint32_t shm_addr_lsw;





    uint32_t shm_addr_msw;
# 1234 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
    uint32_t mem_size_bytes;





}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 1242 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2
;
# 1265 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
typedef struct voice_rsp_shared_mem_map_regions_t voice_rsp_shared_mem_map_regions_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 1268 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2








struct voice_rsp_shared_mem_map_regions_t
{
    uint32_t mem_map_handle;







}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 1288 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2
;
# 1312 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h"
typedef struct voice_cmd_shared_mem_unmap_regions_t voice_cmd_shared_mem_unmap_regions_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 1315 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2








struct voice_cmd_shared_mem_unmap_regions_t
{
    uint32_t mem_map_handle;


}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 1330 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2
;






typedef struct voice_cmd_set_param_v3_t voice_cmd_set_param_v3_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 1340 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2
struct voice_cmd_set_param_v3_t
{
    uint32_t cal_handle;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 1346 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2
;


typedef struct voice_voc_param_data_t voice_voc_param_data_t ;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 1352 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2







struct voice_voc_param_data_t
{
   uint32_t media_id;

   uint32_t param_id;

   uint32_t direction_id;

   uint32_t param_size;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 1371 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vcmn_api.h" 2
;
# 40 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2
# 136 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
typedef enum
{
   VPM_TX_DTMF_DETECTION_DISABLE = 0,
   VPM_TX_DTMF_DETECTION_ENABLE
}vpm_set_tx_dtmf_detection_mode_t;





typedef struct vpm_set_tx_dtmf_detection_t vpm_set_tx_dtmf_detection_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 149 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2






struct vpm_set_tx_dtmf_detection_t
{
   uint32_t enable;





}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 165 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2
;
# 200 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
typedef struct vpm_tx_dtmf_detected_t vpm_tx_dtmf_detected_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 203 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2






struct vpm_tx_dtmf_detected_t
{
   uint16_t low_freq;




   uint16_t high_freq;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 222 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2
;
# 362 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
typedef struct vpm_create_session_v2_t vpm_create_session_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 365 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2








struct vpm_create_session_v2_t
{
   uint16_t tx_port;






   uint16_t tx_sampling_rate;




   uint32_t tx_topology_id;
# 402 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
   uint16_t rx_port;





   uint16_t rx_sampling_rate;




   uint32_t rx_topology_id;






   uint32_t ec_mode;
# 439 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
   uint16_t audio_ref_port;
# 448 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
   uint16_t reserved;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 452 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2
;






typedef struct vpm_create_session_v3_t vpm_create_session_v3_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 462 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2








struct vpm_create_session_v3_t
{
   uint16_t tx_port;






   uint16_t tx_sampling_rate;




   uint32_t tx_topology_id;






   uint16_t rx_port;





   uint16_t rx_sampling_rate;




   uint32_t rx_topology_id;





   uint32_t ec_mode;
# 527 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
   uint16_t audio_ref_port;
# 536 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
   uint16_t reserved;


   uint32_t param_payload_virt_addr;


   uint32_t param_payload_size;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 546 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2
;





typedef struct vpm_create_param_t vpm_create_param_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 555 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2








struct vpm_create_param_t
{
   uint32_t param_id;

   uint32_t param_size;

   uint32_t param_virt_addr;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 573 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2
;
# 644 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
typedef struct vpm_set_timing_params_t vpm_set_timing_params_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 647 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2






struct vpm_set_timing_params_t
{
   uint16_t mode;
# 665 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
   uint16_t vptx_start_offset;
# 674 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
   uint16_t vptx_delivery_offset;
# 684 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
   uint16_t vprx_delivery_offset;
# 695 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
   uint16_t enc_offset;
# 706 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 708 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2
;
# 753 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
typedef struct vpm_get_kpps_ack_t vpm_get_kpps_ack_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 756 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2






struct vpm_get_kpps_ack_t
{
   uint32_t vptx_kpps;




   uint32_t vprx_kpps;




   uint32_t vptx_frame_size_us;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 780 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2
;
# 814 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
typedef struct vpm_set_timing_params_v2_t vpm_set_timing_params_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 817 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2




struct vpm_set_timing_params_v2_t
{
   uint16_t mode;






   uint32_t vsid;






   uint16_t vfr_cycle;





   uint16_t vptx_start_offset;
# 853 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
   uint16_t vptx_proc_start_offset;




   uint16_t vptx_delivery_offset;
# 869 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
   uint16_t vprx_delivery_offset;
# 880 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
   uint16_t enc_offset;
# 892 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 894 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2
;
# 942 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
typedef struct vpm_get_delay_ack_t vpm_get_delay_ack_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 945 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2






struct vpm_get_delay_ack_t
{
   uint32_t vptx_delay;







   uint32_t vprx_delay;






}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 970 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2
;
# 1014 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
typedef struct vpm_cmd_register_event_t vpm_cmd_register_event_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 1017 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2



struct vpm_cmd_register_event_t
{
   uint32_t event_id;
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 1025 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2
;
# 1073 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
typedef struct vpm_voice_activity_status_update_t vpm_voice_activity_status_update_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_begin_pack.h" 1
# 1076 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2



struct vpm_voice_activity_status_update_t
{
   uint32_t activity_type;
# 1091 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/avcs/inc/adsp_end_pack.h" 1
# 1093 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/voice/inc/adsp_vpm_api.h" 2
;
# 11 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/common/cvd/cvd_devcfg/mdm9x07/cvd_devcfg.c" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h" 1
# 96 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
typedef unsigned char boolean;




typedef unsigned long int uint32;




typedef signed long int int32;




typedef unsigned long long uint64;




typedef signed long long int64;




typedef unsigned char byte;




typedef signed char int8;




typedef unsigned long int bool32;




typedef unsigned char uint8;
# 146 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
typedef enum {
    MMPM_STATUS_SUCCESS,
    MMPM_STATUS_FAILED,
    MMPM_STATUS_NOMEMORY,
    MMPM_STATUS_VERSIONNOTSUPPORT,
    MMPM_STATUS_BADCLASS,
    MMPM_STATUS_BADSTATE,
    MMPM_STATUS_BADPARM,
    MMPM_STATUS_INVALIDFORMAT,
    MMPM_STATUS_UNSUPPORTED,
    MMPM_STATUS_RESOURCENOTFOUND,
    MMPM_STATUS_BADMEMPTR,
    MMPM_STATUS_BADHANDLE,
    MMPM_STATUS_RESOURCEINUSE,
    MMPM_STATUS_NOBANDWIDTH,
    MMPM_STATUS_NULLPOINTER,
    MMPM_STATUS_NOTINITIALIZED,
    MMPM_STATUS_RESOURCENOTREQUESTED,
    MMPM_STATUS_CORERESOURCENOTAVAILABLE,

    MMPM_STATUS_MAX,
    MMPM_STATUS_FORCE32BITS = 0x7FFFFFFF

} MmpmStatusType;


typedef MmpmStatusType MMPM_STATUS;
# 195 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
typedef enum {
    MMPM_CORE_ID_NONE = 0,

    MMPM_CORE_ID_2D_GRP = 1,
    MMPM_CORE_ID_3D_GRP = 2,
    MMPM_CORE_ID_MDP = 3,
    MMPM_CORE_ID_VCODEC = 4,
    MMPM_CORE_ID_VPE = 5,
    MMPM_CORE_ID_VFE = 6,
    MMPM_CORE_ID_MIPICSI = 7,

    MMPM_CORE_ID_SENSOR = 8,
    MMPM_CORE_ID_JPEGD = 9,
    MMPM_CORE_ID_JPEGE = 10,
    MMPM_CORE_ID_FABRIC = 11,
    MMPM_CORE_ID_IMEM = 12,
    MMPM_CORE_ID_SMMU = 13,
    MMPM_CORE_ID_ROTATOR = 14,
    MMPM_CORE_ID_TV = 15,
    MMPM_CORE_ID_DSI = 16,
    MMPM_CORE_ID_AUDIOIF = 17,
    MMPM_CORE_ID_GMEM = 18,


    MMPM_CORE_ID_LPASS_START = 100,
    MMPM_CORE_ID_LPASS_ADSP = 101,
    MMPM_CORE_ID_LPASS_CORE = 102,
    MMPM_CORE_ID_LPASS_LPM = 103,
    MMPM_CORE_ID_LPASS_DML = 104,

    MMPM_CORE_ID_LPASS_AIF = 105,

    MMPM_CORE_ID_LPASS_SLIMBUS = 106,

    MMPM_CORE_ID_LPASS_MIDI = 107,
    MMPM_CORE_ID_LPASS_AVSYNC = 108,

    MMPM_CORE_ID_LPASS_HWRSMP = 109,

    MMPM_CORE_ID_LPASS_SRAM = 110,
    MMPM_CORE_ID_LPASS_DCODEC = 111,
    MMPM_CORE_ID_LPASS_SPDIF = 112,
    MMPM_CORE_ID_LPASS_HDMIRX = 113,
    MMPM_CORE_ID_LPASS_HDMITX = 114,
    MMPM_CORE_ID_LPASS_SIF = 115,
    MMPM_CORE_ID_LPASS_BSTC = 116,

    MMPM_CORE_ID_LPASS_ADSP_HVX = 117,

    MMPM_CORE_ID_LPASS_END,

    MMPM_CORE_ID_MAX,
    MMPM_CORE_ID_FORCE32BITS = 0x7FFFFFFF

} MmpmCoreIdType;






typedef enum {
    MMPM_CALLBACK_EVENT_ID_NONE,

    MMPM_CALLBACK_EVENT_ID_IDLE,


    MMPM_CALLBACK_EVENT_ID_BUSY,

    MMPM_CALLBACK_EVENT_ID_THERMAL,

    MMPM_CALLBACK_EVENT_ID_COMPLETE,

    MMPM_CALLBACK_EVENT_ID_MAX,

    MMPM_CALLBACK_EVENT_ID_FORCE32BITS = 0x7FFFFFFF

} MmpmCallbackEventIdType;





typedef struct {
    MmpmCallbackEventIdType eventId;




    uint32 clientId;


    uint32 callbackDataSize;


    void* callbackData;


} MmpmCallbackParamType;





typedef struct {
    uint32 reqTag;
    MMPM_STATUS result;
}MmpmCompletionCallbackDataType;
# 353 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
typedef enum{
    MMPM_CORE_INSTANCE_NONE,
    MMPM_CORE_INSTANCE_0,
    MMPM_CORE_INSTANCE_1,
    MMPM_CORE_INSTANCE_2,
    MMPM_CORE_INSTANCE_MAX,
    MMPM_CORE_INSTANCE_FORCE32BITS = 0x7FFFFFFF


} MmpmCoreInstanceIdType;
# 375 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
typedef struct {
    uint32 rev;




    MmpmCoreIdType coreId;




    MmpmCoreInstanceIdType instanceId;



    char *pClientName;





    uint32 pwrCtrlFlag;





    uint32 callBackFlag;




    uint32 (*MMPM_Callback)(MmpmCallbackParamType *pCbParam);


    uint32 cbFcnStackSize;


} MmpmRegParamType;
# 424 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
typedef enum {
    MMPM_RSC_ID_NONE = 0,


    MMPM_RSC_ID_POWER = 1,



    MMPM_RSC_ID_VREG = 2,



    MMPM_RSC_ID_REG_PROG = 3,


    MMPM_RSC_ID_CORE_CLK = 4,







    MMPM_RSC_ID_CORE_CLK_DOMAIN = 5,



    MMPM_RSC_ID_MEM_BW = 6,


    MMPM_RSC_ID_AXI_EN = 7,

    MMPM_RSC_ID_MIPS = 8,



    MMPM_RSC_ID_SLEEP_LATENCY = 9,



    MMPM_RSC_ID_ACTIVE_STATE = 10,

    MMPM_RSC_ID_PMIC_GPIO = 11,

    MMPM_RSC_ID_RESET = 12,



    MMPM_RSC_ID_MIPS_EXT = 13,



    MMPM_RSC_ID_GENERIC_BW = 14,


    MMPM_RSC_ID_THERMAL = 15,


    MMPM_RSC_ID_MEM_POWER = 16,


    MMPM_RSC_ID_GENERIC_BW_EXT = 17,






    MMPM_RSC_ID_MPPS = 18,





    MMPM_RSC_ID_MAX ,


    MMPM_RSC_ID_FORCE32BITS = 0x7FFFFFFF


} MmpmRscIdType;





typedef enum {
    MMPM_REG_PROG_NONE,
    MMPM_REG_PROG_NORM,
    MMPM_REG_PROG_FAST,
    MMPM_REG_PROG_MAX,
    MMPM_REG_PROG_FORCE32BITS = 0x7FFFFFFF

} MmpmRegProgMatchType;






typedef enum {
    MMPM_CLK_ID_2D_GRP_NONE,

    MMPM_CLK_ID_2D_GRP,


    MMPM_CLK_ID_2D_GRP_MAX,

    MMPM_CLK_ID_2D_GRP_FORCE32BITS = 0x7FFFFFFF

} MmpmClkId2dGrpType;





typedef enum {
    MMPM_CLK_ID_3D_GRP_NONE,

    MMPM_CLK_ID_3D_GRP,


    MMPM_CLK_ID_3D_GRP_MAX,

    MMPM_CLK_ID_3D_GRP_FORCE32BITS = 0x7FFFFFFF

} MmpmClkId3dGrpType;





typedef enum {
    MMPM_CLK_ID_MDP_NONE,

    MMPM_CLK_ID_MDP,

    MMPM_CLK_ID_MDP_VSYNC,


    MMPM_CLK_ID_MDP_LUT,

    MMPM_CLK_ID_MDP_MAX,

    MMPM_CLK_ID_MDP_FORCE32BITS = 0x7FFFFFFF

} MmpmClkIdMdpType;





typedef enum {
    MMPM_CLK_ID_VCODEC_NONE,

    MMPM_CLK_ID_VCODEC,


    MMPM_CLK_ID_VCODEC_MAX,

    MMPM_CLK_ID_VCODEC_FORCE32BITS = 0x7FFFFFFF

} MmpmClkIdVcodecType;





typedef enum {
    MMPM_CLK_ID_VPE_NONE,

    MMPM_CLK_ID_VPE,

    MMPM_CLK_ID_VPE_MAX,

    MMPM_CLK_ID_VPE_FORCE32BITS = 0x7FFFFFFF

} MmpmClkIdVpeType;





typedef enum {
    MMPM_CLK_ID_VFE_NONE,

    MMPM_CLK_ID_VFE,

    MMPM_CLK_ID_VFE_MAX,

    MMPM_CLK_ID_VFE_FORCE32BITS = 0x7FFFFFFF

} MmpmClkIdVfeType;





typedef enum {
    MMPM_CLK_ID_CSI_NONE,

    MMPM_CLK_ID_CSI,


    MMPM_CLK_ID_CSI_VFE,


    MMPM_CLK_ID_CSI_PHY,


    MMPM_CLK_ID_CSI_PHY_TIMER,


    MMPM_CLK_ID_CSI_PIX0,



    MMPM_CLK_ID_CSI_PIX1,


    MMPM_CLK_ID_CSI_RDI0,


    MMPM_CLK_ID_CSI_RDI1,


    MMPM_CLK_ID_CSI_RDI2,


    MMPM_CLK_ID_CSI_MAX,

    MMPM_CLK_ID_CSI_FORCE32BITS = 0x7FFFFFFF

} MmpmClkIdCsiType;





typedef enum {
    MMPM_CLK_ID_SENSOR_NONE,

    MMPM_CLK_ID_SENSOR,


    MMPM_CLK_ID_SENSOR_MAX,

    MMPM_CLK_ID_SENSOR_FORCE32BITS = 0x7FFFFFFF

} MmpmClkIdSensorType;





typedef enum {
    MMPM_CLK_ID_JPEGD_NONE,

    MMPM_CLK_ID_JPEGD,


    MMPM_CLK_ID_JPEGD_MAX,

    MMPM_CLK_ID_JPEGD_FORCE32BITS = 0x7FFFFFFF

} MmpmClkIdJpegdType;





typedef enum {
    MMPM_CLK_ID_JPEGE_NONE,

    MMPM_CLK_ID_JPEGE,


    MMPM_CLK_ID_JPEGE_MAX,

    MMPM_CLK_ID_JPEGE_FORCE32BITS = 0x7FFFFFFF

} MmpmClkIdJpegeType;





typedef enum {
    MMPM_CLK_ID_ROTATOR_NONE,

    MMPM_CLK_ID_ROTATOR,


    MMPM_CLK_ID_ROTATOR_MAX,

    MMPM_CLK_ID_ROTATOR_FORCE32BITS = 0x7FFFFFFF

} MmpmClkIdRotatorType;





typedef enum {
    MMPM_CLK_ID_TV_NONE,

    MMPM_CLK_ID_TV_ENC,


    MMPM_CLK_ID_TV_DAC,


    MMPM_CLK_ID_TV_MDP,


    MMPM_CLK_ID_TV_HDMI_APP,


    MMPM_CLK_ID_TV_HDMI,


    MMPM_CLK_ID_TV_RGB,

    MMPM_CLK_ID_TV_NPL,

    MMPM_CLK_ID_TV_MAX,

    MMPM_CLK_ID_TV_FORCE32BITS = 0x7FFFFFFF

} MmpmClkIdTvType;





typedef enum {
    MMPM_CLK_ID_DSI_NONE,

    MMPM_CLK_ID_DSI,

    MMPM_CLK_ID_DSI_ESC,


    MMPM_CLK_ID_DSI_PIX,


    MMPM_CLK_ID_DSI_BYTE,


    MMPM_CLK_ID_DSI_LVDS,

    MMPM_CLK_ID_DSI_MDP_P2,

    MMPM_CLK_ID_DSI_MAX,

    MMPM_CLK_ID_DSI_FORCE32BITS = 0x7FFFFFFF

} MmpmClkIdDsiType;





typedef enum {
    MMPM_CLK_ID_AUDIOIF_NONE,

    MMPM_CLK_ID_AUDIOIF_PCM,


    MMPM_CLK_ID_AUDIOIF_I2S_CODEC_SPKR_MCLK,


    MMPM_CLK_ID_AUDIOIF_I2S_CODEC_SPKR_SCLK,


    MMPM_CLK_ID_AUDIOIF_I2S_SPARE_SPKR_MCLK,


    MMPM_CLK_ID_AUDIOIF_I2S_SPARE_SPKR_SCLK,


    MMPM_CLK_ID_AUDIOIF_I2S_CODEC_MIC_MCLK,


    MMPM_CLK_ID_AUDIOIF_I2S_CODEC_MIC_SCLK,


    MMPM_CLK_ID_AUDIOIF_I2S_SPARE_MIC_MCLK,


    MMPM_CLK_ID_AUDIOIF_I2S_SPARE_MIC_SCLK,


    MMPM_CLK_ID_AUDIOIF_MI2S_CODEC_MCLK,


    MMPM_CLK_ID_AUDIOIF_MI2S_CODEC_SCLK,


    MMPM_CLK_ID_AUDIOIF_MI2S_CODEC_TX_MCLK,


    MMPM_CLK_ID_AUDIOIF_MI2S_CODEC_TX_SCLK,


    MMPM_CLK_ID_AUDIOIF_LPASS_SB_REF_CLK,


    MMPM_CLK_ID_AUDIOIF_SPS_SB_REF_CLK,


    MMPM_CLK_ID_AUDIOIF_MAX,

    MMPM_CLK_ID_AUDIOIF_FORCE32BITS = 0x7FFFFFFF

} MmpmClkIdAudioIfType;
# 849 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
typedef enum {
    MMPM_CLK_ID_LPASS_NONE = 0,

    MMPM_CLK_ID_LPASS_HWRSP_CORE,

    MMPM_CLK_ID_LPASS_MIDI_CORE,

    MMPM_CLK_ID_LPASS_AVSYNC_XO,

    MMPM_CLK_ID_LPASS_AVSYNC_BT,

    MMPM_CLK_ID_LPASS_AVSYNC_FM,

    MMPM_CLK_ID_LPASS_SLIMBUS_CORE,

    MMPM_CLK_ID_LPASS_SLIMBUS2_CORE,

    MMPM_CLK_ID_LPASS_AVTIMER_CORE,

    MMPM_CLK_ID_LPASS_ATIME_CORE,

    MMPM_CLK_ID_LPASS_ATIME2_CORE,

    MMPM_CLK_ID_LPASS_BSTC_CORE,


    MMPM_CLK_ID_LPASS_ADSP_CORE,

    MMPM_CLK_ID_LPASS_AHB_ROOT,

    MMPM_CLK_ID_LPASS_ENUM_MAX,

    MMPM_CLK_ID_LPASS_FORCE32BITS = 0x7fffffff

} MmpmClkIdLpassType;




typedef enum {
    MMPM_CLK_ID_VCAP_NONE,

    MMPM_CLK_ID_VCAP,

    MMPM_CLK_ID_VCAP_NPL,

    MMPM_CLK_ID_VCAP_MAX,

    MMPM_CLK_ID_VCAP_FORCE32BITS = 0x7FFFFFFF

} MmpmClkIdVcapType;




typedef union {

    MmpmClkId2dGrpType clkId2dGrp;
    MmpmClkId3dGrpType clkId3dGrp;
    MmpmClkIdMdpType clkIdMdp;

    MmpmClkIdVcodecType clkIdVcodec;
    MmpmClkIdVpeType clkIdVpe;

    MmpmClkIdVfeType clkIdVfe;
    MmpmClkIdCsiType clkIdCsi;

    MmpmClkIdSensorType clkIdSensor;
    MmpmClkIdJpegdType clkIdJpegd;
    MmpmClkIdJpegeType clkIdJpege;
    MmpmClkIdRotatorType clkIdRotator;
    MmpmClkIdTvType clkIdTv;
    MmpmClkIdDsiType clkIdDsi;

    MmpmClkIdAudioIfType clkIdAudioIf;
    MmpmClkIdVcapType clkIdVcap;


    MmpmClkIdLpassType clkIdLpass;




} MmpmCoreClkIdType;





typedef enum {
    MMPM_FREQ_AT_LEAST,
    MMPM_FREQ_AT_MOST,
    MMPM_FREQ_CLOSEST,
    MMPM_FREQ_EXACT,
    MMPM_FREQ_MAX,
    MMPM_FREQ_FORCE32BITS = 0x7FFFFFFF

} MmpmFreqMatchType;





typedef struct {
    MmpmCoreClkIdType clkId;



    uint32 clkFreqHz;

    MmpmFreqMatchType freqMatch;



} MmpmClkValType;






typedef struct {
    uint32 numOfClk;

    MmpmClkValType *pClkArray;


} MmpmClkReqType;







typedef enum {
    MMPM_CLK_DOMAIN_SRC_ID_HDMI_NONE,

    MMPM_CLK_DOMAIN_SRC_ID_HDMI0,

    MMPM_CLK_DOMAIN_SRC_ID_HDMI_MAX,

    MMPM_CLK_DOMAIN_SRC_ID_HDMI_FORCE32BITS = 0x7FFFFFFF

} MmpmClkDomainSrcIdHdmiType;





typedef enum {
    MMPM_CLK_DOMAIN_SRC_ID_DSI_NONE,

    MMPM_CLK_DOMAIN_SRC_ID_DSI0,

    MMPM_CLK_DOMAIN_SRC_ID_DSI1,

    MMPM_CLK_DOMAIN_SRC_ID_LVDS,

    MMPM_CLK_DOMAIN_SRC_ID_DSI_MAX,

    MMPM_CLK_DOMAIN_SRC_ID_DSI_FORCE32BITS = 0x7FFFFFFF

} MmpmClkDomainSrcIdDsiType;





typedef enum {
    MMPM_CLK_DOMAIN_SRC_ID_LPASS_NONE,

    MMPM_CLK_DOMAIN_SRC_ID_Q6PLL,

    MMPM_CLK_DOMAIN_SRC_ID_AUDIOPLL,

    MMPM_CLK_DOMAIN_SRC_ID_PRIUSPLL,

    MMPM_CLK_DOMAIN_SRC_ID_LPASS_MAX,

    MMPM_CLK_DOMAIN_SRC_ID_LPASS_FORCE32BITS = 0x7FFFFFFF

} MmpmClkDomainSrcIdLpassType;





typedef union {

    MmpmClkDomainSrcIdHdmiType clkDomainSrcIdHdmi;

    MmpmClkDomainSrcIdDsiType clkDomainSrcIdDsi;



    MmpmClkDomainSrcIdLpassType clkDomainSrcIdLpass;



} MmpmClkDomainSrcIdType;




typedef struct {
    MmpmCoreClkIdType clkId;


    uint32 M_val;

    uint32 N_val;

    uint32 n2D_val;

    uint32 div_val;

    uint32 clkFreqHz;


    MmpmClkDomainSrcIdType clkDomainSrc;

} MmpmClkDomainType;





typedef struct {
    uint32 numOfClk;

    MmpmClkDomainType *pClkDomainArray;

} MmpmClkDomainReqType;






typedef enum {
    MMPM_THERMAL_NONE,
    MMPM_THERMAL_LOW,
    MMPM_THERMAL_NORM,

    MMPM_THERMAL_HIGH_L1,
    MMPM_THERMAL_HIGH_L2,
    MMPM_THERMAL_HIGH_L3,
    MMPM_THERMAL_HIGH_L4,
    MMPM_THERMAL_HIGH_L5,
    MMPM_THERMAL_HIGH_L6,
    MMPM_THERMAL_HIGH_L7,
    MMPM_THERMAL_HIGH_L8,
    MMPM_THERMAL_HIGH_L9,
    MMPM_THERMAL_HIGH_L10,
    MMPM_THERMAL_HIGH_L11,
    MMPM_THERMAL_HIGH_L12,
    MMPM_THERMAL_HIGH_L13,
    MMPM_THERMAL_HIGH_L14,
    MMPM_THERMAL_HIGH_L15,
    MMPM_THERMAL_HIGH_L16,
    MMPM_THERMAL_MAX,
    MMPM_THERMAL_FORCE32BITS = 0x7FFFFFFF

} MmpmThermalType;







typedef enum {
    MMPM_BW_USAGE_2D_GRP_NONE,
    MMPM_BW_USAGE_2D_GRP,
    MMPM_BW_USAGE_2D_GRP_MAX,
    MMPM_BW_USAGE_2D_GRP_FORCE32BITS = 0x7FFFFFFF

} MmpmBwUsage2dGrpType;





typedef enum {
    MMPM_BW_USAGE_3D_GRP_NONE,
    MMPM_BW_USAGE_3D_GRP,
    MMPM_BW_USAGE_3D_GRP_MAX,
    MMPM_BW_USAGE_3D_GRP_FORCE32BITS = 0x7FFFFFFF

} MmpmBwUsage3dGrpType;





typedef enum {
    MMPM_BW_USAGE_MDP_NONE,
    MMPM_BW_USAGE_MDP,
    MMPM_BW_USAGE_MDP_HRES,
    MMPM_BW_USAGE_MDP_MAX,
    MMPM_BW_USAGE_MDP_FORCE32BITS = 0x7FFFFFFF

} MmpmBwUsageMdpType;





typedef enum {
    MMPM_BW_USAGE_VCODEC_NONE,

    MMPM_BW_USAGE_VCODEC_ENC,

    MMPM_BW_USAGE_VCODEC_DEC,

    MMPM_BW_USAGE_VCODEC_ENC_DEC,

    MMPM_BW_USAGE_VCODEC_MAX,

    MMPM_BW_USAGE_VCODEC_FORCE32BITS = 0x7FFFFFFF

} MmpmBwUsageVcodecType;





typedef enum {
    MMPM_BW_USAGE_VPE_NONE,
    MMPM_BW_USAGE_VPE,
    MMPM_BW_USAGE_VPE_MAX,
    MMPM_BW_USAGE_VPE_FORCE32BITS = 0x7FFFFFFF

} MmpmBwUsageVpeType;





typedef enum {
    MMPM_BW_USAGE_VFE_NONE,
    MMPM_BW_USAGE_VFE,
    MMPM_BW_USAGE_VFE_MAX,
    MMPM_BW_USAGE_VFE_FORCE32BITS = 0x7FFFFFFF

} MmpmBwUsageVfeType;





typedef enum {
    MMPM_BW_USAGE_JPEGD_NONE,
    MMPM_BW_USAGE_JPEGD,
    MMPM_BW_USAGE_JPEGD_MAX,
    MMPM_BW_USAGE_JPEGD_FORCE32BITS = 0x7FFFFFFF


} MmpmBwUsageJpegdType;





typedef enum {
    MMPM_BW_USAGE_JPEGE_NONE,
    MMPM_BW_USAGE_JPEGE,
    MMPM_BW_USAGE_JPEGE_MAX,
    MMPM_BW_USAGE_JPEGE_FORCE32BITS = 0x7FFFFFFF

} MmpmBwUsageJpegeType;





typedef enum {
    MMPM_BW_USAGE_ROTATOR_NONE,
    MMPM_BW_USAGE_ROTATOR,
    MMPM_BW_USAGE_ROTATOR_MAX,
    MMPM_BW_USAGE_ROTATOR_FORCE32BITS = 0x7FFFFFFF

} MmpmBwUsageRotatorType;





typedef enum {
    MMPM_BW_USAGE_LPASS_NONE,
    MMPM_BW_USAGE_LPASS_DSP,
    MMPM_BW_USAGE_LPASS_DMA,
    MMPM_BW_USAGE_LPASS_EXT_CPU,

    MMPM_BW_USAGE_LPASS_ENUM_MAX,
    MMPM_BW_USAGE_LPASS_FORCE32BITS = 0x7FFFFFFF

} MmpmBwUsageLpassType;





typedef union {

    MmpmBwUsage2dGrpType bwUsage2dGrp;
    MmpmBwUsage3dGrpType bwUsage3dGrp;
    MmpmBwUsageMdpType bwUsageMdp;
    MmpmBwUsageVcodecType bwUsageVcodec;
    MmpmBwUsageVpeType bwUsageVpe;
    MmpmBwUsageVfeType bwUsageVfe;
    MmpmBwUsageJpegdType bwUsageJpegd;
    MmpmBwUsageJpegeType bwUsageJpege;
    MmpmBwUsageRotatorType bwUsageRotator;


    MmpmBwUsageLpassType bwUsageLpass;



} MmpmCoreBwUsageType;




typedef enum {
    MMPM_BW_PORT_ID_NONE = 0,
    MMPM_BW_PORT_ID_ADSP_MASTER,
    MMPM_BW_PORT_ID_DML_MASTER,
    MMPM_BW_PORT_ID_AIF_MASTER,
    MMPM_BW_PORT_ID_SLIMBUS_MASTER,
    MMPM_BW_PORT_ID_SLIMBUS2_MASTER,
    MMPM_BW_PORT_ID_MIDI_MASTER,
    MMPM_BW_PORT_ID_HWRSMP_MASTER,
    MMPM_BW_PORT_ID_EXT_AHB_MASTER,

    MMPM_BW_PORT_ID_SPDIF_MASTER,
    MMPM_BW_PORT_ID_HDMIRX_MASTER,
    MMPM_BW_PORT_ID_HDMITX_MASTER,
    MMPM_BW_PORT_ID_SIF_MASTER,
    MMPM_BW_PORT_ID_DML_SLAVE,
    MMPM_BW_PORT_ID_AIF_SLAVE,
    MMPM_BW_PORT_ID_SLIMBUS_SLAVE,
    MMPM_BW_PORT_ID_SLIMBUS2_SLAVE,
    MMPM_BW_PORT_ID_MIDI_SLAVE,
    MMPM_BW_PORT_ID_HWRSMP_SLAVE,
    MMPM_BW_PORT_ID_AVSYNC_SLAVE,
    MMPM_BW_PORT_ID_LPM_SLAVE,
    MMPM_BW_PORT_ID_SRAM_SLAVE,
    MMPM_BW_PORT_ID_EXT_AHB_SLAVE,
    MMPM_BW_PORT_ID_DDR_SLAVE,
    MMPM_BW_PORT_ID_OCMEM_SLAVE,
    MMPM_BW_PORT_ID_PERIFNOC_SLAVE,
    MMPM_BW_PORT_ID_SPDIF_SLAVE,
    MMPM_BW_PORT_ID_HDMIRX_SLAVE,
    MMPM_BW_PORT_ID_HDMITX_SLAVE,
    MMPM_BW_PORT_ID_SIF_SLAVE,
    MMPM_BW_PORT_ID_BSTC_SLAVE,
    MMPM_BW_PORT_ID_DCODEC_SLAVE,
    MMPM_BW_PORT_ID_CORE,



    MMPM_BW_PORT_ID_MAX,
    MMPM_BW_PORT_ID_FORCE32BITS = 0x7FFFFFFF

} MmpmBwPortIdType;






typedef struct {

    uint32 memPhyAddr;

    uint32 axiPort;



    uint32 bwBytePerSec;



    uint32 usagePercentage;

    MmpmCoreBwUsageType bwUsageType;


} MmpmBwValType;





typedef struct {
    uint32 numOfBw;

    MmpmBwValType *pBandWidthArray;

} MmpmBwReqType;







typedef struct{
    MmpmBwPortIdType masterPort;




    MmpmBwPortIdType slavePort;



} MmpmBusRouteType;




typedef struct{
    uint64 Ab;
    uint64 Ib;
} MmpmBusBWDataIbAbType;




typedef struct{
    uint64 bwBytePerSec;

    uint32 usagePercentage;

    MmpmBwUsageLpassType usageType;



} MmpmBusBWDataUsageType;





typedef union{
    MmpmBusBWDataIbAbType busBwAbIb;

    MmpmBusBWDataUsageType busBwValue;

} MmpmBusBWDataType;





typedef struct{
    MmpmBusRouteType busRoute;

    MmpmBusBWDataType bwValue;

} MmpmGenBwValType;





typedef struct {
    uint32 numOfBw;


    MmpmGenBwValType *pBandWidthArray;

} MmpmGenBwReqType;





typedef enum {
    MMPM_MEM_POWER_NONE,
    MMPM_MEM_POWER_OFF,
    MMPM_MEM_POWER_RETENTION,
    MMPM_MEM_POWER_ACTIVE,
    MMPM_MEM_POWER_MAX,
    MMPM_MEM_POWER_FORCE32BITS = 0x7FFFFFFF

} MmpmMemPowerStateType;




typedef enum{
    MMPM_MEM_NONE,
    MMPM_MEM_OCMEM,
    MMPM_MEM_LPASS_LPM,
    MMPM_MEM_SRAM,
    MMPM_MEM_MAX,
    MMPM_MEM_FORCE32BITS = 0x7FFFFFFF

} MmpmMemIdType;




typedef struct{
    MmpmMemIdType memory;




    MmpmMemPowerStateType powerState;



}MmpmMemPowerReqParamType;





typedef struct {
    uint32 gpioId;

    boolean configGpio;

    uint32 gpioVoltageSource;

    boolean gpioModeOnOff;

    uint32 gpioModeSelect;

    uint32 gpioOutBufferConfig;

    boolean invertExtPin;

    uint32 gpioCurrentSourcePulls;

    uint32 gpioOutBufferDriveStrength;

    uint32 gpioDtestBufferOnOff;

    uint32 gpioExtPinConfig;

    uint32 gpioSourceConfig;

    boolean interrupPolarity;

    uint32 uartPath;

} MmpmPmicGpioParamType;





typedef enum {
     MMPM_PMIC_CONFIG,

     MMPM_PMIC_CONFIG_BIAS_VOLTAGE,

     MMPM_PMIC_CONFIG_DIGITAL_INPUT,

     MMPM_PMIC_CONFIG_DIGITAL_OUTPUT,

     MMPM_PMIC_CONFIG_SET_VOLTAGE_SOURCE,

     MMPM_PMIC_CONFIG_MODE_SELECTION,

     MMPM_PMIC_CONFIG_SET_OUTPUT_BUFFER,

     MMPM_PMIC_CONFIG_SET_INVERSION,

     MMPM_PMIC_CONFIG_SET_CURRENT_SOURCE_PULLS,

     MMPM_PMIC_CONFIG_SET_EXT_PIN,

     MMPM_PMIC_CONFIG_SET_OUTPUT_BUF_DRIVE_STRG,

     MMPM_PMIC_CONFIG_SET_SOURCE,

     MMPM_PMIC_CONFIG_SET_INT_POLARITY,

     MMPM_PMIC_CONFIG_SET_MUX_CTRL,

} MmpmPmicGpioConfigType;
# 1548 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
typedef struct {
    MmpmPmicGpioConfigType configId;

    MmpmPmicGpioParamType gpioParam;

} MmpmPmicGpioReqType;




typedef enum {
     MMPM_RESET_NONE,
     MMPM_RESET_DEASSERT,
     MMPM_RESET_ASSERT,
     MMPM_RESET_PULSE,
     MMPM_RESET_ENUM_MAX,
     MMPM_RESET_ENUM_FORCE32BITS = 0x7FFFFFFF

} MmpmResetType;






typedef struct {
    MmpmCoreClkIdType clkId;

    MmpmResetType resetType;

} MmpmResetParamType;




typedef struct {
    uint32 numOfReset;

    MmpmResetParamType *pResetParamArray;

} MmpmResetReqType;





typedef enum {
    MMPM_MIPS_REQUEST_NONE = 0,

    MMPM_MIPS_REQUEST_CPU_CLOCK_ONLY,


    MMPM_MIPS_REQUEST_CPU_CLOCK_AND_BW,




    MMPM_MIPS_REQUEST_ENUM_MAX,

    MMPM_MIPS_REQUEST_FORCE32BITS = 0x7FFFFFFF

} MmpmMipsRequestFnType;




typedef struct {
    uint32 mipsTotal;

    uint32 mipsPerThread;

    MmpmBwPortIdType codeLocation;



    MmpmMipsRequestFnType reqOperation;



} MmpmMipsReqType;




typedef struct {
    uint32 mppsTotal;

    uint32 adspFloorClock;

} MmpmMppsReqType;
# 1648 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
typedef struct {
    uint32 numOfClk;

    MmpmCoreClkIdType *pClkIdArray;

} MmpmClkIdArrayParamType;
# 1738 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
typedef union {

    uint32 vregMilliVolt;







    MmpmRegProgMatchType regProgMatch;







    MmpmPmicGpioReqType *pPmicGpio;







    MmpmClkReqType *pCoreClk;







    MmpmClkDomainReqType *pCoreClkDomain;







    MmpmBwReqType *pBwReq;







    MmpmGenBwReqType *pGenBwReq;







    uint32 sleepMicroSec;







    uint32 mips;
# 1815 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
    MmpmResetReqType *pResetReq;







    MmpmMipsReqType *pMipsExt;







    MmpmMppsReqType *pMppsReq;







    MmpmThermalType thermalMitigation;







    MmpmMemPowerReqParamType *pMemPowerState;
# 1856 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
    MmpmClkIdArrayParamType *pRelClkIdArray;
# 1866 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
    MmpmCoreClkIdType gateClkId;






}MmpmRscParamStructType;
# 1882 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
typedef struct {
    MmpmRscIdType rscId;




    MmpmRscParamStructType rscParam;

} MmpmRscParamType;
# 1903 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
typedef enum{
     MMPM_API_TYPE_NONE,
     MMPM_API_TYPE_SYNC,
     MMPM_API_TYPE_ASYNC,
     MMPM_API_TYPE_ENUM_MAX,
     MMPM_API_TYPE_FORCE32BITS = 0x7FFFFFFF


} MmpmApiType;




typedef struct {
    MmpmApiType apiType;




    uint32 numOfReq;


    MmpmRscParamType *pReqArray;


    MMPM_STATUS *pStsArray;


    uint32 reqTag;


    void *pExt;


} MmpmRscExtParamType;
# 1948 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
typedef enum {
    MMPM_INFO_ID_NONE,



    MMPM_INFO_ID_CORE_CLK,



    MMPM_INFO_ID_CORE_CLK_MAX,



    MMPM_INFO_ID_CORE_CLK_MAX_SVS,


    MMPM_INFO_ID_CORE_CLK_MAX_NOMINAL,


    MMPM_INFO_ID_CORE_CLK_MAX_TURBO,




    MMPM_INFO_ID_MIPS_MAX,

    MMPM_INFO_ID_BW_MAX,



    MMPM_INFO_ID_CRASH_DUMP,

    MMPM_INFO_ID_POWER_SUPPORT,



    MMPM_INFO_ID_CLK_FREQ,



    MMPM_INFO_ID_PERFMON,

    MMPM_INFO_ID_PMIC_GPIO,

    MMPM_INFO_ID_SET_DEBUG_LEVEL,

    MMPM_INFO_ID_MMSS_BUS,

    MMPM_INFO_ID_LPASS_BUS,



    MMPM_INFO_ID_AGGREGATE_CLIENT_CLASS,

    MMPM_INFO_ID_MPPS,

    MMPM_INFO_ID_BW_EXT,

    MMPM_INFO_ID_DCVS_STATE,


    MMPM_INFO_ID_EXT_REQ,


    MMPM_INFO_ID_MAX,

    MMPM_INFO_ID_FORCE32BITS = 0x7FFFFFFF

} MmpmInfoIdType;






typedef struct {
    MmpmCoreClkIdType clkId;

    uint32 clkFreqHz;

} MmpmInfoClkType;
# 2040 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
typedef struct {
    uint32 clkId;

    uint32 clkFreqHz;

    uint32 forceMeasure;




} MmpmInfoClkFreqType;

typedef struct
{
    uint32 clientClasses;



    uint32 aggregateMpps;


} MmpmInfoMppsType;

typedef struct
{
    MmpmBusRouteType busRoute;

    uint64 totalBw;


} MmpmInfoBwExtType;
# 2106 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
typedef struct {
    uint32 masterMeasureArray[7];


    uint32 measurementConfig[7];


    uint32 latencyMasterPort;

    uint32 holdoffTime;

    uint32 triggerMode;


    void *pDataMsg;

    char pFilename[64];

    uint32 axiClockFreq;


    uint32 clockInfo[10];


    uint32 isClockFreqCalc[10];



} MmpmInfoPerfmonType;




typedef struct {
    uint32 gpioModeSelect;

    uint32 gpioVoltageSource;

    uint32 gpioModeOnOff;

    uint32 gpioOutBufferConfig;

    uint32 gpioOutBufferDriveStrength;

    uint32 gpioCurrentSourcePulls;

    uint32 gpioSourceConfig;

    uint32 gpioDtestBufferOnOff;

    uint32 gpioExtPinConfig;

} MmpmPmicGpioStatusType;





typedef struct {
    uint32 gpioId;
    MmpmPmicGpioStatusType gpioSts;

} MmpmPmicGpioInfoType;
# 2193 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
typedef struct {
    uint32 clientId;

    char clientName[32];

    char fabClientName[32];

    uint32 uIbBytePerSec;


    uint32 uAbBytePerSec;


} MmpmBwClientType;




typedef struct {
    uint32 coreId;

    uint32 instanceId;

    uint32 axiPort;

    uint32 slaveId;

    uint32 uIbBytePerSec;


    uint32 uAbBytePerSec;


    uint32 numOfClients;

    MmpmBwClientType client[16];

} MmpmMasterSlaveType;




typedef struct {
    uint32 mmFabClkFreq;

    uint32 appsFabClkFreq;

    uint32 sysFabClkFreq;

    uint32 lpassFabClkFreq;

    uint32 ebiClkFreq;

    uint32 mmImemClkFreq;

    uint32 numOfMasterSlave;

    MmpmMasterSlaveType masterSlave[16];

} MmpmFabStatusInfoType;






typedef union {

    MmpmInfoClkType *pInfoClk;




    boolean bInfoPower;



    MmpmInfoPerfmonType *pInfoPerfmon;




    MmpmInfoClkFreqType *pInfoClkFreqType;





    MmpmPmicGpioInfoType *pPmicGpioStatus;

    uint32 infoSetLotLevel;

    MmpmFabStatusInfoType *pFabStatus;



    uint32 mipsValue;





    uint64 bwValue;





    uint32 aggregateClientClass;







    MmpmInfoMppsType *pMppsInfo;



    MmpmInfoBwExtType *pBwExtInfo;



    boolean dcvsState;







    void *pExtInfo;





} MmpmInfoDataStructType;





typedef struct {
    MmpmInfoIdType infoId;
# 2353 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
    MmpmCoreIdType coreId;




    MmpmCoreInstanceIdType instanceId;





    MmpmInfoDataStructType info;


} MmpmInfoDataType;
# 2376 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
typedef enum{
    MMPM_PARAM_ID_NONE = 0,



    MMPM_PARAM_ID_RESOURCE_LIMIT,



    MMPM_PARAM_ID_CLIENT_OCMEM_MAP,

    MMPM_PARAM_ID_MEMORY_MAP,

    MMPM_PARAM_ID_CLIENT_CLASS,

    MMPM_PARAM_ID_L2_CACHE_LINE_LOCK,

    MMPM_PARAM_ID_DCVS_PARTICIPATION,

    MMPM_PARAM_ID_ENUM_MAX,

    MMPM_PARAM_ID_Force32bit = 0x7fffffff

} MmpmParameterIdType;





typedef struct
{
    MmpmMemIdType memory;
    uint64 startAddress;

    uint32 size;
} MmpmMemoryMapType;







typedef struct
{
    void* startAddress;

    uint32 size;


    uint32 throttleBlockSize;



    uint32 throttlePauseUs;


} MmpmL2CacheLineLockParameterType;


typedef enum
{
    MMPM_UNKNOWN_CLIENT_CLASS = 0x00,
    MMPM_AUDIO_CLIENT_CLASS = 0x01,
    MMPM_VOICE_CLIENT_CLASS = 0x02,
    MMPM_COMPUTE_CLIENT_CLASS = 0x04,
    MMPM_STREAMING_1HVX_CLIENT_CLASS = 0x08,

    MMPM_STREAMING_2HVX_CLIENT_CLASS = 0x10,

    MMPM_MAX_CLIENT_CLASS,
} MmpmClientClassType;



typedef enum
{
    MMPM_DCVS_ADJUST_UP_DOWN,
    MMPM_DCVS_ADJUST_ONLY_UP
} MmpmDcvsEnableOptionsType;


typedef struct
{
    boolean enable;

    MmpmDcvsEnableOptionsType enableOpt;


} MmpmDcvsParticipationType;




typedef struct{
    MmpmParameterIdType paramId;




    void* pParamConfig;
# 2489 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
} MmpmParameterConfigType;




typedef struct{
    uint32 regionId;

    uint32 numKeys;

    uint32 *pKey;

} MmpmOcmemMapRegionDescType;




typedef struct{
    uint32 numRegions;

    MmpmOcmemMapRegionDescType *pRegions;


} MmpmOcmemMapType;
# 2544 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
uint32 MMPM_Init(char * cmd_line);
# 2578 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
uint32 MMPM_Register_Ext(MmpmRegParamType *pRegParam);
# 2603 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
MMPM_STATUS MMPM_Deregister_Ext(uint32 clientId);
# 2653 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
MMPM_STATUS MMPM_Request(uint32 clientId,
                             MmpmRscParamType *pMmpmRscParam);
# 2686 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
MMPM_STATUS MMPM_Release(uint32 clientId,
                             MmpmRscParamType *pMmpmRscParam);
# 2724 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
MMPM_STATUS MMPM_Pause(uint32 clientId,
                           MmpmRscParamType *pMmpmRscParam);
# 2753 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
MMPM_STATUS MMPM_Resume(uint32 clientId,
                            MmpmRscParamType *pMmpmRscParam);
# 2812 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
MMPM_STATUS MMPM_Request_Ext(uint32 clientId,
                                 MmpmRscExtParamType *pRscExtParam);
# 2845 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
MMPM_STATUS MMPM_Release_Ext(uint32 clientId,
                                 MmpmRscExtParamType *pRscExtParam);
# 2881 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
MMPM_STATUS MMPM_Pause_Ext(uint32 clientId,
                               MmpmRscExtParamType *pRscExtParam);
# 2917 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
MMPM_STATUS MMPM_Resume_Ext(uint32 clientId,
                                MmpmRscExtParamType *pRscExtParam);
# 2948 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
MMPM_STATUS MMPM_GetInfo(MmpmInfoDataType *pInfoData);
# 2976 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/power/mmpm.h"
MMPM_STATUS MMPM_SetParameter(uint32 clientId, MmpmParameterConfigType *pParamConfig);
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/common/cvd/cvd_devcfg/mdm9x07/cvd_devcfg.c" 2

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 1
# 17 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h" 1
# 17 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_comdef.h" 1
# 18 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imemory_public_if.h" 1
# 120 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imemory_public_if.h"
typedef struct vss_imemory_block_t vss_imemory_block_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 123 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imemory_public_if.h" 2





struct vss_imemory_block_t
{
  uint64_t mem_address;







  uint32_t mem_size;
# 148 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imemory_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 150 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imemory_public_if.h" 2
;
# 180 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imemory_public_if.h"
typedef struct vss_imemory_cmd_map_virtual_t vss_imemory_cmd_map_virtual_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 183 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imemory_public_if.h" 2





struct vss_imemory_cmd_map_virtual_t
{
  vss_imemory_block_t block;


  bool_t is_cached;






  uint16_t cache_line_size;




  uint32_t access_mask;






  uint32_t page_align;




  uint8_t min_data_width;




  uint8_t max_data_width;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 228 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imemory_public_if.h" 2
;
# 277 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imemory_public_if.h"
typedef struct vss_imemory_table_descriptor_t vss_imemory_table_descriptor_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 280 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imemory_public_if.h" 2






struct vss_imemory_table_descriptor_t
{
  uint64_t mem_address;






  uint32_t mem_size;






}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 304 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imemory_public_if.h" 2
;



typedef struct vss_imemory_table_t vss_imemory_table_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 311 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imemory_public_if.h" 2





struct vss_imemory_table_t
{
  vss_imemory_table_descriptor_t next_table_descriptor;







  vss_imemory_block_t blocks[];


}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 331 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imemory_public_if.h" 2
;



typedef struct vss_imemory_cmd_map_physical_t vss_imemory_cmd_map_physical_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 338 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imemory_public_if.h" 2





struct vss_imemory_cmd_map_physical_t
{
  vss_imemory_table_descriptor_t table_descriptor;



  bool_t is_cached;






  uint16_t cache_line_size;




  uint32_t access_mask;






  uint32_t page_align;




  uint8_t min_data_width;




  uint8_t max_data_width;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 384 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imemory_public_if.h" 2
;
# 404 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imemory_public_if.h"
typedef struct vss_imemory_rsp_map_t vss_imemory_rsp_map_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 407 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imemory_public_if.h" 2





struct vss_imemory_rsp_map_t
{
  uint32_t mem_handle;






}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 423 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imemory_public_if.h" 2
;
# 446 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imemory_public_if.h"
typedef struct vss_imemory_cmd_unmap_t vss_imemory_cmd_unmap_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 449 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imemory_public_if.h" 2





struct vss_imemory_cmd_unmap_t
{
  uint32_t mem_handle;


}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 461 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imemory_public_if.h" 2
;
# 19 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h" 2
# 30 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
typedef struct cvd_addr64_t
{
   uint32_t word0;

   uint32_t word1;

}cvd_addr64_t;

typedef union cvd_virt_addr_t
{
   cvd_addr64_t addr64;
   void* ptr;
}cvd_virt_addr_t;
# 54 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_init ( void );






extern int32_t cvd_mem_mapper_deinit ( void );
# 71 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_cache_invalidate (
  uint64_t virt_addr,
  uint32_t size
);
# 84 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_cache_invalidate_v2 (
  cvd_virt_addr_t* virt_addr,
  uint32_t size
);
# 97 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_cache_flush (
  uint64_t virt_addr,
  uint32_t size
);
# 110 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_cache_flush_v2 (
  cvd_virt_addr_t* virt_addr,
  uint32_t size
);
# 125 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_map_virtual (
  vss_imemory_cmd_map_virtual_t* map_virtual_arg,
  uint32_t* ret_mem_handle
);
# 140 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_map_physical (
  vss_imemory_cmd_map_physical_t* map_physical_arg,
  uint32_t* ret_mem_handle
);
# 152 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_unmap (
  uint32_t mem_handle
);
# 166 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_get_virtual_addr (
  uint32_t mem_handle,
  uint64_t mem_addr,
  uint64_t* ret_virt_addr
);
# 182 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_get_virtual_addr_v2 (
  uint32_t mem_handle,
  uint64_t mem_addr,
  cvd_virt_addr_t* ret_virt_addr
);
# 199 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_get_mem_addr (
  uint32_t mem_handle,
  uint64_t virt_addr,
  uint64_t* ret_mem_addr
);
# 215 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_get_region_base_virt_addr (
  uint32_t mem_handle,
  uint64_t* ret_base_virt_addr
);
# 229 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_get_region_base_virt_addr_v2 (
  uint32_t mem_handle,
  cvd_virt_addr_t* ret_base_virt_addr
);
# 242 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_get_region_size (
  uint32_t mem_handle,
  uint32_t* ret_size
);
# 255 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_validate_handle (
  uint32_t mem_handle
);
# 269 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_validate_attributes_align (
  uint32_t mem_handle,
  uint64_t mem_addr
);
# 285 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_validate_access_align (
  void* mem_addr,
  uint32_t mem_size,
  uint32_t align
);
# 302 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_validate_mem_is_in_region (
  uint32_t mem_handle,
  uint64_t mem_addr,
  uint32_t mem_size
);
# 319 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_validate_virt_mem_is_in_region (
  uint32_t mem_handle,
  void* mem_addr,
  uint32_t mem_size
);
# 333 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_set_vsm_mem_handle (
  uint32_t cvd_mem_handle,
  uint32_t vsm_mem_handle
);
# 346 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_set_vpm_mem_handle (
  uint32_t cvd_mem_handle,
  uint32_t vpm_mem_handle
);
# 359 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_get_vsm_mem_handle (
  uint32_t cvd_mem_handle,
  uint32_t* ret_vsm_mem_handle
);
# 372 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern int32_t cvd_mem_mapper_get_vpm_mem_handle (
  uint32_t cvd_mem_handle,
  uint32_t* ret_vpm_mem_handle
);
# 386 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
static inline void cvd_mem_mapper_set_uint64_to_virt_addr( void **ptr_dest, uint64_t virt_addr_src )
{
  *ptr_dest = ( void* )( ( uint32_t )virt_addr_src );
  return;
}






static inline void cvd_mem_mapper_set_virt_addr( cvd_virt_addr_t *virt_addr_dest, uint64_t virt_addr_src )
{
  cvd_mem_mapper_set_uint64_to_virt_addr( &virt_addr_dest->ptr, virt_addr_src );
  return;
}







static inline void cvd_mem_mapper_set_virt_addr_to_uint32( uint32_t *msw_dest, uint32_t *lsw_dest, void *ptr_src )
{
  *lsw_dest = ( uint32_t )ptr_src;
  *msw_dest = 0;
  return;
}






static inline void cvd_mem_mapper_set_virt_addr_to_uint64( uint64_t *virt_addr_dest, void *ptr_src )
{
  *virt_addr_dest = ( uint64_t )( ( uint32_t )ptr_src );
  return;
}
# 434 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_mem_mapper_i.h"
extern uint32_t cvd_mem_mapper_obj_stat (
  uint32_t *objects_used
);
# 18 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_istream_private_if.h" 1
# 15 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_istream_private_if.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 1
# 92 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_cmd_create_full_control_session_t vss_istream_cmd_create_full_control_session_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 95 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2






struct vss_istream_cmd_create_full_control_session_t
{
   uint16_t direction;
# 112 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
   uint32_t enc_media_type;


   uint32_t dec_media_type;


   uint32_t network_id;





   char_t name[];
# 136 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 138 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 161 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_cmd_set_media_type_t vss_istream_cmd_set_media_type_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 164 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2





struct vss_istream_cmd_set_media_type_t
{
   uint32_t rx_media_id;




   uint32_t tx_media_id;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 182 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 201 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_cmd_attach_vocproc_t vss_istream_cmd_attach_vocproc_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 204 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2





struct vss_istream_cmd_attach_vocproc_t
{
  uint16_t handle;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 215 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 234 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_cmd_detach_vocproc_t vss_istream_cmd_detach_vocproc_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 237 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2





struct vss_istream_cmd_detach_vocproc_t
{
  uint16_t handle;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 248 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;







typedef union vss_istream_cal_na_value_t vss_istream_cal_na_value_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 259 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2





union vss_istream_cal_na_value_t
{
  uint8_t uint8_val;

  uint16_t uint16_val;

  uint32_t uint32_val;

  uint64_t uint64_val;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 276 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;



typedef struct vss_istream_cal_column_t vss_istream_cal_column_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 283 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2





struct vss_istream_cal_column_t
{
  uint32_t id;




  uint32_t type;





  vss_istream_cal_na_value_t na_value;
# 310 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 312 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 381 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_cmd_register_static_calibration_data_t vss_istream_cmd_register_static_calibration_data_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 384 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2





struct vss_istream_cmd_register_static_calibration_data_t
{
  uint32_t cal_mem_handle;



  uint64_t cal_mem_address;


  uint32_t cal_mem_size;







  uint32_t num_columns;




  vss_istream_cal_column_t columns[];





}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 419 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 456 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_cmd_set_enc_dtx_mode_t vss_istream_cmd_set_enc_dtx_mode_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 459 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2





struct vss_istream_cmd_set_enc_dtx_mode_t
{
   uint32_t enable;





}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 474 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 497 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_cmd_cdma_set_enc_minmax_rate_t vss_istream_cmd_cdma_set_enc_minmax_rate_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 500 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2






struct vss_istream_cmd_cdma_set_enc_minmax_rate_t
{
   uint16_t min_rate;
# 518 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
   uint16_t max_rate;
# 527 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 529 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 603 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_cmd_cdma_set_enc_rate_modulation_t vss_istream_cmd_cdma_set_enc_rate_modulation_t;






# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 610 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2






struct vss_istream_cmd_cdma_set_enc_rate_modulation_t
{
   uint32_t mode;
# 633 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 635 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 659 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_cmd_voc_qcelp13k_set_rate_t vss_istream_cmd_voc_qcelp13k_set_rate_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 662 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2






struct vss_istream_cmd_voc_qcelp13k_set_rate_t
{
   uint32_t rate;
# 679 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 681 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 704 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_cmd_voc_4gvnb_set_rate_t vss_istream_cmd_voc_4gvnb_set_rate_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 707 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2






struct vss_istream_cmd_voc_4gvnb_set_rate_t
{
   uint32_t rate;
# 727 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 729 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 752 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_cmd_voc_4gvwb_set_rate_t vss_istream_cmd_voc_4gvwb_set_rate_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 755 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2






struct vss_istream_cmd_voc_4gvwb_set_rate_t
{
   uint32_t rate;






}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 772 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 795 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_cmd_voc_4gvnw_set_rate_t vss_istream_cmd_voc_4gvnw_set_rate_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 798 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2






struct vss_istream_cmd_voc_4gvnw_set_rate_t
{
   uint32_t rate;
# 818 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 820 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 843 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_cmd_voc_4gvnw2k_set_rate_t vss_istream_cmd_voc_4gvnw2k_set_rate_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 846 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2





struct vss_istream_cmd_voc_4gvnw2k_set_rate_t
{
   uint32_t rate;
# 865 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 867 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 890 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_cmd_voc_amr_set_enc_rate_t vss_istream_cmd_voc_amr_set_enc_rate_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 893 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2






struct vss_istream_cmd_voc_amr_set_enc_rate_t
{
   uint32_t mode;
# 913 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 915 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 938 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_cmd_voc_amrwb_set_enc_rate_t vss_istream_cmd_voc_amrwb_set_enc_rate_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 941 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2






struct vss_istream_cmd_voc_amrwb_set_enc_rate_t
{
   uint32_t mode;
# 962 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 964 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 1017 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_cmd_set_evs_voc_enc_operating_mode_t vss_istream_cmd_set_evs_voc_enc_operating_mode_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1020 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2






struct vss_istream_cmd_set_evs_voc_enc_operating_mode_t
{
   uint8_t mode;
# 1063 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
  uint8_t bandwidth;
# 1072 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1074 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 1107 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_cmd_set_evs_enc_channel_aware_mode_enable_t vss_istream_cmd_set_evs_enc_channel_aware_mode_enable_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1110 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2





struct vss_istream_cmd_set_evs_enc_channel_aware_mode_enable_t
{
   uint8_t fec_offset;




   uint8_t fer_rate;





}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1130 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 1198 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_cmd_set_dtmf_generation_t vss_istream_cmd_set_dtmf_generation_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1201 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2






struct vss_istream_cmd_set_dtmf_generation_t
{
   uint16_t direction;





   uint16_t mix_flag;






   uint16_t tone_1;




   uint16_t tone_2;




   uint16_t gain;





   int16_t duration;







}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1248 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 1269 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_evt_dtmf_generation_ended_t vss_istream_evt_dtmf_generation_ended_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1272 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2






struct vss_istream_evt_dtmf_generation_ended_t
{
   uint16_t direction;




}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1287 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 1327 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_cmd_set_rx_dtmf_detection_t vss_istream_cmd_set_rx_dtmf_detection_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1330 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2






struct vss_istream_cmd_set_rx_dtmf_detection_t
{
   uint32_t enable;





}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1346 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 1372 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_evt_rx_dtmf_detected_t vss_istream_evt_rx_dtmf_detected_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1375 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2





struct vss_istream_evt_rx_dtmf_detected_t
{
   uint16_t low_freq;




   uint16_t high_freq;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1393 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 1441 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_evt_eamr_mode_changed_t vss_istream_evt_eamr_mode_changed_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1444 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2





struct vss_istream_evt_eamr_mode_changed_t
{
   uint32_t mode;





}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1459 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 1490 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_evt_evs_rx_bandwidth_changed_t vss_istream_evt_evs_rx_bandwidth_changed_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1493 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2






struct vss_istream_evt_evs_rx_bandwidth_changed_t
{
   uint8_t bandwidth;







}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1511 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 1577 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef struct vss_istream_cmd_set_voc_param_t vss_istream_cmd_set_voc_param_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1580 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2



struct vss_istream_cmd_set_voc_param_t
{
  uint32_t mem_handle;
# 1602 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
  uint64_t mem_address;




  uint32_t mem_size;
# 1625 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1627 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;


typedef struct vss_istream_voc_param_data_t vss_istream_voc_param_data_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1633 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2








struct vss_istream_voc_param_data_t
{
  uint32_t media_id;







  uint32_t param_id;




  uint32_t direction_id;







  uint32_t param_size;
# 1677 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1679 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h" 2
;
# 1690 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_public_if.h"
typedef enum vss_istream_param_direction_t
{
   VSS_ISTREAM_DIRECTION_TX,
   VSS_ISTREAM_DIRECTION_RX,
   VSS_ISTREAM_DIRECTION_TX_RX
} vss_istream_param_direction_t ;
# 16 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_istream_private_if.h" 2
# 140 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_istream_private_if.h"
typedef struct vss_istream_cmd_set_voice_timing_t vss_istream_cmd_set_voice_timing_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 143 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_istream_private_if.h" 2



struct vss_istream_cmd_set_voice_timing_t
{
  uint32_t vsid;

  uint16_t vfr_mode;




   uint32_t vfr_cycle_duration_us;



  uint16_t enc_offset;



  uint16_t dec_req_offset;



  uint16_t dec_offset;




  uint16_t vprx_delivery_offset;



  uint16_t vptx_buf_start_offset;



  uint16_t vptx_proc_start_offset;



  uint16_t vptx_delivery_offset;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 190 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_istream_private_if.h" 2
;
# 215 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_istream_private_if.h"
typedef struct vss_istream_cmd_set_vocproc_avsync_delays_t vss_istream_cmd_set_vocproc_avsync_delays_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 218 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_istream_private_if.h" 2



struct vss_istream_cmd_set_vocproc_avsync_delays_t
{
  uint32_t vp_rx_normalized_total_delay;

  uint32_t vp_tx_normalized_total_delay;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 229 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_istream_private_if.h" 2
;
# 242 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_istream_private_if.h"
typedef struct vss_istream_cmd_set_dejitter_mode_t vss_istream_cmd_set_dejitter_mode_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 245 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_istream_private_if.h" 2
struct vss_istream_cmd_set_dejitter_mode_t
{
   uint16_t mode;






   uint16_t num_frames;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 260 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_istream_private_if.h" 2
;
# 274 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_istream_private_if.h"
typedef struct vss_istream_cmd_cdma_voip_set_enc_dtx_mode_t vss_istream_cmd_cdma_voip_set_enc_dtx_mode_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 277 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_istream_private_if.h" 2
struct vss_istream_cmd_cdma_voip_set_enc_dtx_mode_t
{
   uint16_t enable;






   uint16_t min_interval;






   uint16_t max_interval;






   uint16_t hangover;






}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 309 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_istream_private_if.h" 2
;
# 346 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_istream_private_if.h"
typedef struct vss_istream_evt_voc_operating_mode_update_t vss_istream_evt_voc_operating_mode_update_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 349 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_istream_private_if.h" 2




struct vss_istream_evt_voc_operating_mode_update_t
{
  uint32_t rx_mode;



  uint32_t tx_mode;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 365 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_istream_private_if.h" 2
;
# 19 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivocproc_private_if.h" 1
# 15 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivocproc_private_if.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h" 1
# 164 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h"
typedef struct vss_ivocproc_cmd_create_full_control_session_v3_t vss_ivocproc_cmd_create_full_control_session_v3_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 167 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h" 2





struct vss_ivocproc_cmd_create_full_control_session_v3_t
{
  uint16_t direction;







  uint16_t tx_port_id;





  uint32_t tx_topology_id;
# 197 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h"
  uint16_t rx_port_id;





  uint32_t rx_topology_id;
# 212 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h"
  uint32_t profile_id;


  uint32_t vocproc_mode;







  uint16_t ec_ref_port_id;
# 234 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h"
  char_t session_name[];






}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 243 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h" 2
;
# 294 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h"
typedef struct vss_ivocproc_cmd_set_device_v3_t vss_ivocproc_cmd_set_device_v3_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 297 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h" 2





struct vss_ivocproc_cmd_set_device_v3_t
{
  uint16_t tx_port_id;





  uint32_t tx_topology_id;
# 319 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h"
  uint16_t rx_port_id;





  uint32_t rx_topology_id;
# 334 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h"
  uint32_t vocproc_mode;







  uint16_t ec_ref_port_id;
# 351 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 353 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h" 2
;
# 467 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h"
typedef struct vss_ivocproc_cmd_register_device_config_t vss_ivocproc_cmd_register_device_config_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 470 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h" 2






struct vss_ivocproc_cmd_register_device_config_t
{

  uint32_t mem_handle;



  uint64_t mem_address;


  uint32_t mem_size;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 492 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h" 2
;
# 516 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h"
typedef union vss_ivocproc_cal_na_value_t vss_ivocproc_cal_na_value_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 519 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h" 2





union vss_ivocproc_cal_na_value_t
{
  uint8_t uint8_val;

  uint16_t uint16_val;

  uint32_t uint32_val;

  uint64_t uint64_val;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 536 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h" 2
;



typedef struct vss_ivocproc_cal_column_t vss_ivocproc_cal_column_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 543 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h" 2





struct vss_ivocproc_cal_column_t
{
  uint32_t id;




  uint32_t type;





  vss_ivocproc_cal_na_value_t na_value;
# 570 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 572 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h" 2
;
# 642 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h"
typedef struct vss_ivocproc_cmd_register_static_calibration_data_t vss_ivocproc_cmd_register_static_calibration_data_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 645 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h" 2





struct vss_ivocproc_cmd_register_static_calibration_data_t
{
  uint32_t cal_mem_handle;


  uint64_t cal_mem_address;


  uint32_t cal_mem_size;





  uint32_t num_columns;




  vss_ivocproc_cal_column_t columns[];




}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 676 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h" 2
;
# 765 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h"
typedef struct vss_ivocproc_cmd_register_dynamic_calibration_data_t vss_ivocproc_cmd_register_dynamic_calibration_data_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 768 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h" 2





struct vss_ivocproc_cmd_register_dynamic_calibration_data_t
{
  uint32_t cal_mem_handle;


  uint64_t cal_mem_address;


  uint32_t cal_mem_size;





  uint32_t num_columns;




  vss_ivocproc_cal_column_t columns[];




}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 799 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h" 2
;
# 852 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h"
typedef struct vss_ivocproc_cmd_set_tx_dtmf_detection_t vss_ivocproc_cmd_set_tx_dtmf_detection_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 855 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h" 2






struct vss_ivocproc_cmd_set_tx_dtmf_detection_t
{
   uint32_t enable;





}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 871 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h" 2
;
# 897 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h"
typedef struct vss_ivocproc_evt_tx_dtmf_detected_t vss_ivocproc_evt_tx_dtmf_detected_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 900 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h" 2





struct vss_ivocproc_evt_tx_dtmf_detected_t
{
   uint16_t low_freq;




   uint16_t high_freq;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 918 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_public_if.h" 2
;
# 16 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivocproc_private_if.h" 2
# 74 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivocproc_private_if.h"
typedef struct vss_ivocproc_cmd_attach_stream_t vss_ivocproc_cmd_attach_stream_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 77 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivocproc_private_if.h" 2
struct vss_ivocproc_cmd_attach_stream_t
{
   uint16_t direction;







}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 89 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivocproc_private_if.h" 2
;




typedef struct vss_ivocproc_rsp_attach_detach_stream_t vss_ivocproc_rsp_attach_detach_stream_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 97 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivocproc_private_if.h" 2
struct vss_ivocproc_rsp_attach_detach_stream_t
{
   uint32_t direction;







   uint32_t mixer_inport_mask;


   uint32_t mixer_outport_mask;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 114 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivocproc_private_if.h" 2
;
# 123 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivocproc_private_if.h"
typedef struct vss_ivocproc_rsp_detach_stream_t vss_ivocproc_rsp_detach_stream_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 126 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivocproc_private_if.h" 2
struct vss_ivocproc_rsp_detach_stream_t
{
   uint16_t vdsp_session_handle;

   uint16_t direction;







}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 140 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivocproc_private_if.h" 2
;
# 211 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivocproc_private_if.h"
typedef struct vss_ivocproc_cmd_set_voice_timing_t vss_ivocproc_cmd_set_voice_timing_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 214 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivocproc_private_if.h" 2



struct vss_ivocproc_cmd_set_voice_timing_t
{
  uint32_t vsid;

  uint16_t vfr_mode;




  uint32_t vfr_cycle_duration_us;



  uint16_t enc_offset;



  uint16_t dec_req_offset;



  uint16_t dec_offset;




  uint16_t vprx_delivery_offset;



  uint16_t vptx_buf_start_offset;



  uint16_t vptx_proc_start_offset;



  uint16_t vptx_delivery_offset;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 261 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivocproc_private_if.h" 2
;
# 308 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivocproc_private_if.h"
typedef struct vss_ivocproc_rsp_get_avsync_delays_t vss_ivocproc_rsp_get_avsync_delays_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 311 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivocproc_private_if.h" 2



struct vss_ivocproc_rsp_get_avsync_delays_t
{
  uint32_t vp_rx_algorithmic_delay;


  uint32_t vp_tx_algorithmic_delay;


}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 324 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivocproc_private_if.h" 2
;
# 367 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivocproc_private_if.h"
typedef struct vss_ivocproc_evt_afe_port_info_t vss_ivocproc_evt_afe_port_info_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 370 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivocproc_private_if.h" 2
struct vss_ivocproc_evt_afe_port_info_t
{
   uint32_t rx_port_id;

   uint32_t tx_port_id;

   uint32_t ec_ref_port_id;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 380 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivocproc_private_if.h" 2
;
# 20 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_imvm_private_if.h" 1
# 21 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_common_private_if.h" 1
# 14 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_common_private_if.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 1
# 691 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
typedef struct vss_icommon_param_data_t vss_icommon_param_data_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 694 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2








struct vss_icommon_param_data_t
{
  uint32_t module_id;




  uint32_t param_id;




  uint16_t param_size;





  uint16_t reserved;
# 739 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 741 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2
;
# 774 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
typedef struct vss_icommon_cmd_set_param_v2_t vss_icommon_cmd_set_param_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 777 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2







struct vss_icommon_cmd_set_param_v2_t
{
  uint32_t mem_handle;
# 803 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
  uint64_t mem_address;




  uint32_t mem_size;
# 826 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 828 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2
;
# 861 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
typedef struct vss_icommon_cmd_get_param_v2_t vss_icommon_cmd_get_param_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 864 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2








struct vss_icommon_cmd_get_param_v2_t
{
  uint32_t mem_handle;
# 891 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
  uint64_t mem_address;







  uint16_t mem_size;
# 909 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
  uint32_t module_id;




  uint32_t param_id;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 920 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2
;
# 943 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
typedef struct vss_icommon_rsp_get_param_t vss_icommon_rsp_get_param_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 946 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2
# 956 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
struct vss_icommon_rsp_get_param_t
{
  uint32_t status;
# 976 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 978 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2
;



typedef struct vss_icommon_param_data_v3_t vss_icommon_param_data_v3_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 985 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2








struct vss_icommon_param_data_v3_t
{
  uint32_t module_id;




  uint16_t instance_id;


  uint16_t reserved;


  uint32_t param_id;




  uint32_t param_size;
# 1032 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1034 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2
;
# 1070 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
typedef struct vss_icommon_cmd_set_param_v3_t vss_icommon_cmd_set_param_v3_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1073 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2







struct vss_icommon_cmd_set_param_v3_t
{
  uint32_t mem_handle;
# 1092 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
  uint64_t mem_address;





  uint32_t mem_size;
# 1114 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1116 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2
;
# 1153 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
typedef struct vss_icommon_cmd_get_param_v3_t vss_icommon_cmd_get_param_v3_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1156 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2








struct vss_icommon_cmd_get_param_v3_t
{
  uint32_t mem_handle;
# 1176 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
  uint64_t mem_address;






  uint16_t mem_size;
# 1193 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
  uint32_t module_id;




  uint16_t instance_id;


  uint16_t reserved;


  uint32_t param_id;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1210 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2
;
# 1235 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
typedef struct vss_icommon_rsp_get_param_v3_t vss_icommon_rsp_get_param_v3_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1238 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2
# 1248 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
struct vss_icommon_rsp_get_param_v3_t
{
  uint32_t status;
# 1263 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1265 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2
;
# 1293 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
typedef struct vss_icommon_cmd_set_ui_property_t vss_icommon_cmd_set_ui_property_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1296 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2







struct vss_icommon_cmd_set_ui_property_t
{
  uint32_t module_id;




  uint32_t param_id;




  uint16_t param_size;





  uint16_t reserved;
# 1332 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1334 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2
;
# 1356 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
typedef struct vss_icommon_cmd_get_ui_property_t vss_icommon_cmd_get_ui_property_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1359 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2







struct vss_icommon_cmd_get_ui_property_t
{
  uint32_t module_id;




  uint32_t param_id;




  uint32_t param_size;




}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1385 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2
;
# 1405 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
typedef struct vss_icommon_rsp_get_ui_property_t vss_icommon_rsp_get_ui_property_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1408 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2








struct vss_icommon_rsp_get_ui_property_t
{
  uint32_t status;







  uint32_t module_id;




  uint32_t param_id;




  uint16_t param_size;





  uint16_t reserved;
# 1453 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1455 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2
;
# 1480 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
typedef struct vss_icommon_cmd_set_ui_property_v2_t vss_icommon_cmd_set_ui_property_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1483 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2








struct vss_icommon_cmd_set_ui_property_v2_t
{
  uint32_t module_id;




  uint16_t instance_id;


  uint16_t reserved;


  uint32_t param_id;




  uint32_t param_size;
# 1522 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1524 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2
;
# 1548 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
typedef struct vss_icommon_cmd_get_ui_property_v2_t vss_icommon_cmd_get_ui_property_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1551 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2





struct vss_icommon_cmd_get_ui_property_v2_t
{
  uint32_t module_id;




  uint16_t instance_id;


  uint16_t reserved;


  uint32_t param_id;




  uint32_t param_size;




}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1581 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2
;
# 1601 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
typedef struct vss_icommon_rsp_get_ui_property_v2_t vss_icommon_rsp_get_ui_property_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1604 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2








struct vss_icommon_rsp_get_ui_property_v2_t
{
  uint32_t status;







  uint32_t module_id;




  uint16_t instance_id;


  uint16_t reserved;


  uint32_t param_id;




  uint32_t param_size;
# 1651 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1653 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2
;
# 1696 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
typedef struct vss_icommon_cmd_set_voice_timing_v2_t vss_icommon_cmd_set_voice_timing_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1699 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2





struct vss_icommon_cmd_set_voice_timing_v2_t
{
  uint16_t mode;







  uint32_t vsid;


  uint16_t enc_offset;




  uint16_t dec_req_offset;




  uint16_t dec_offset;







}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1737 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2
;
# 1756 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
typedef struct vss_icommon_cmd_set_vfr_config_t vss_icommon_cmd_set_vfr_config_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1759 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2




struct vss_icommon_cmd_set_vfr_config_t
{
  uint16_t vfr_mode;







  uint16_t vfr_cycle_duration_ms;





  uint32_t vsid;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1783 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2
;
# 1801 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
typedef struct vss_icommon_cmd_set_voice_timing_reference_t vss_icommon_cmd_set_voice_timing_reference_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1804 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2




struct vss_icommon_cmd_set_voice_timing_reference_t
{
  uint64_t tx_ref_timstamp_us;
# 1823 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
  uint64_t rx_ref_timstamp_us;
# 1835 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1837 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2
;
# 1909 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
typedef struct vss_icommon_evt_voice_activity_update_t vss_icommon_evt_voice_activity_update_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 1912 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2




struct vss_icommon_evt_voice_activity_update_t
{
   uint32_t activity;
# 1930 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1932 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_public_if.h" 2
;
# 15 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_common_private_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/mvm_api_i.h" 1
# 13 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/mvm_api_i.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/mvm_api.h" 1
# 14 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/mvm_api.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/drv_api.h" 1
# 15 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/mvm_api.h" 2
# 34 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/mvm_api.h"
extern int32_t mvm_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
);
# 14 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/mvm_api_i.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/cvd_cal_protected_i.h" 1
# 27 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/cvd_cal_protected_i.h"
typedef struct cvd_cal_param_t cvd_cal_param_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 30 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/cvd_cal_protected_i.h" 2

struct cvd_cal_param_t
{
  uint32_t minor_version;





  uint32_t module_id;
  uint32_t param_id;
  uint32_t param_data_size;
  void* param_data;
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 45 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/cvd_cal_protected_i.h" 2
;

typedef struct cvd_cal_param_v1_t cvd_cal_param_v1_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 50 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/cvd_cal_protected_i.h" 2

struct cvd_cal_param_v1_t
{
  uint32_t minor_version;





  uint32_t module_id;
  uint16_t instance_id;
  uint16_t reserved;
  uint32_t param_id;
  uint32_t param_data_size;
  void* param_data;
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 67 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/cvd_cal_protected_i.h" 2
;

typedef struct cvd_rx_gain_cal_param_t cvd_rx_gain_cal_param_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 72 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/cvd_cal_protected_i.h" 2

struct cvd_rx_gain_cal_param_t
{
    uint32_t minor_version;


    uint32_t module_id;
    uint32_t param_id;
    uint32_t param_data_size;
    uint32_t param_data;
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 84 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/cvd_cal_protected_i.h" 2
;

typedef struct cvd_rx_gain_cal_param_v1_t cvd_rx_gain_cal_param_v1_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 89 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/cvd_cal_protected_i.h" 2

struct cvd_rx_gain_cal_param_v1_t
{
    uint32_t minor_version;


    uint32_t module_id;
    uint16_t instance_id;
    uint16_t reserved;
    uint32_t param_id;
    uint32_t param_data_size;
    uint32_t param_data;
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 103 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/cvd_cal_protected_i.h" 2
;
# 114 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/cvd_cal_protected_i.h"
  typedef void ( *cvd_cal_query_cb_fn_t ) ( void* cal_params,
                                            void* client_data );
# 15 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/mvm_api_i.h" 2
# 48 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/mvm_api_i.h"
typedef struct vss_imvm_cmd_cal_query_t vss_imvm_cmd_cal_query_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 51 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/mvm_api_i.h" 2



struct vss_imvm_cmd_cal_query_t
{
  uint32_t query_handle;
  cvd_cal_query_cb_fn_t cb_fn;
  const void* client_data;
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 61 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/mvm_api_i.h" 2
;






typedef enum mvm_csm_client_enum_t{
   MVM_CSM_CLIENT_ENUM_VPTX,
   MVM_CSM_CLIENT_ENUM_VPRX,
   MVM_CSM_CLIENT_ENUM_VENC,
   MVM_CSM_CLIENT_ENUM_VDEC,
   MVM_CSM_CLIENT_ENUM_VSM,
   MVM_CSM_CLIENT_ENUM_VPM,
   MVM_CSM_CLIENT_ENUM_MVM,
   MVM_CSM_CLIENT_ENUM_MAX
 } mvm_csm_client_enum_t;
# 102 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/mvm_api_i.h"
typedef struct vss_imvm_csm_cmd_request_vote_t vss_imvm_csm_cmd_request_vote_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 105 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/mvm_api_i.h" 2



struct vss_imvm_csm_cmd_request_vote_t
{
  mvm_csm_client_enum_t client;


  bool_t vote;




}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 120 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/protected/mvm_api_i.h" 2
;
# 16 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_common_private_if.h" 2
# 39 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_common_private_if.h"
typedef struct vss_icommon_cmd_set_system_config_t vss_icommon_cmd_set_system_config_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 42 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_common_private_if.h" 2



struct vss_icommon_cmd_set_system_config_t
{
  uint32_t network_id;



  uint32_t media_id;


  uint32_t rx_voc_op_mode;



  uint32_t tx_voc_op_mode;



  uint32_t dec_sr;

  uint32_t enc_sr;

  uint32_t rx_pp_sr;

  uint32_t tx_pp_sr;

  uint32_t feature;



  uint32_t vsid;

  uint16_t vfr_mode;




  uint16_t vfr_cycle_duration_us;




  uint32_t call_num;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 90 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_common_private_if.h" 2
;
# 110 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_common_private_if.h"
typedef struct vss_icommon_rsp_set_system_config_t vss_icommon_rsp_set_system_config_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 113 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_common_private_if.h" 2



struct vss_icommon_rsp_set_system_config_t
{
  uint32_t dec_sr;

  uint32_t enc_sr;

  uint32_t rx_pp_sr;

  uint32_t tx_pp_sr;

  uint32_t vocproc_tx_topology_id;

  uint32_t vocproc_rx_topology_id;

  uint32_t stream_media_id;

  uint32_t enc_kpps;

  uint32_t dec_kpps;

  uint32_t dec_pp_kpps;

  uint32_t vp_rx_kpps;

  uint32_t vp_tx_kpps;

  uint32_t tx_num_channels;

  uint32_t tx_mpps_scale_factor;

  uint32_t tx_bw_scale_factor;

  uint32_t rx_mpps_scale_factor;

  uint32_t rx_bw_scale_factor;

  uint32_t tx_afe_port_sr;

  uint32_t rx_afe_port_sr;

  uint32_t ec_ref_afe_port_sr;

  uint32_t vptx_frame_size_us;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 162 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_common_private_if.h" 2
;
# 182 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_common_private_if.h"
typedef struct vss_icommon_cmd_set_dynamic_system_config_t vss_icommon_cmd_set_dynamic_system_config_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 185 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_common_private_if.h" 2



struct vss_icommon_cmd_set_dynamic_system_config_t
{
  uint32_t rx_voc_op_mode;



  uint32_t tx_voc_op_mode;



  uint32_t feature_id;



  uint32_t rx_pp_sr;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 206 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_common_private_if.h" 2
;
# 225 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_common_private_if.h"
typedef struct vss_icommon_cmd_set_var_voc_sampling_rate_t vss_icommon_cmd_set_var_voc_sampling_rate_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 228 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_common_private_if.h" 2



struct vss_icommon_cmd_set_var_voc_sampling_rate_t
{
  uint32_t rx_pp_sr;


  uint32_t tx_pp_sr;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 240 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_common_private_if.h" 2
;
# 270 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_common_private_if.h"
typedef struct vss_icommon_evt_toggle_csm_t vss_icommon_evt_toggle_csm_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 273 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_common_private_if.h" 2




struct vss_icommon_evt_toggle_csm_t
{
bool_t status;




}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 286 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_common_private_if.h" 2
;
# 22 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_iccm_private_if.h" 1
# 54 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_iccm_private_if.h"
typedef struct vss_iccm_evt_active_sessions_t vss_iccm_evt_active_sessions_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 57 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_iccm_private_if.h" 2



struct vss_iccm_evt_active_sessions_t
{
  uint16_t num_voice_sessions;

  uint16_t num_nb_streams;

  uint16_t num_wb_streams;

  uint16_t num_swb_streams;

  uint16_t num_fb_plus_streams;

  uint16_t num_nb_vocprocs;

  uint16_t num_wb_vocprocs;

  uint16_t num_swb_vocprocs;

  uint16_t num_fb_plus_vocprocs;

  uint32_t total_core_kpps;

  uint32_t vptx_preemption_safety_margin;

  uint32_t vprx_preemption_safety_margin;

  uint32_t venc_preemption_safety_margin;

  uint32_t vdec_preemption_safety_margin;

  uint16_t vptx_proc;
  uint16_t vprx_proc;
  uint16_t venc_proc;
  uint16_t vdec_proc;
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 96 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_iccm_private_if.h" 2
;
# 23 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_issr_private_if.h" 1
# 40 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_issr_private_if.h"
typedef struct vss_issr_cmd_cleanup_t vss_issr_cmd_cleanup_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 43 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_issr_private_if.h" 2



struct vss_issr_cmd_cleanup_t
{
  uint8_t domain_id;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 52 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_issr_private_if.h" 2
;
# 24 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvs_api.h" 1
# 15 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvs_api.h"
typedef enum cvs_callindex_enum_t
{
                CVS_CALLINDEX_ENUM_UNINITIALIZED = 0,
                CVS_CALLINDEX_ENUM_INIT = 1,
                CVS_CALLINDEX_ENUM_POSTINIT = 2,
                CVS_CALLINDEX_ENUM_PREDEINIT = 3,
                CVS_CALLINDEX_ENUM_DEINIT = 4,
                CVS_CALLINDEX_ENUM_RUN = 5,
                CVS_CALLINDEX_ENUM_INVALID
}
  cvs_callindex_enum_t;

extern int32_t cvs_call (
  cvs_callindex_enum_t index,
  void* params,
  uint32_t size
);
# 25 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvp_api.h" 1
# 15 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvp_api.h"
typedef enum cvp_callindex_enum_t
{
                CVP_CALLINDEX_ENUM_UNINITIALIZED = 0,
                CVP_CALLINDEX_ENUM_INIT = 1,
                CVP_CALLINDEX_ENUM_POSTINIT = 2,
                CVP_CALLINDEX_ENUM_PREDEINIT = 3,
                CVP_CALLINDEX_ENUM_DEINIT = 4,
                CVP_CALLINDEX_ENUM_RUN = 5,
                CVP_CALLINDEX_ENUM_INVALID
}
  cvp_callindex_enum_t;

extern int32_t cvp_call (
  cvp_callindex_enum_t index,
  void* params,
  uint32_t size
);
# 26 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 2

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_vfr_api.h" 1
# 16 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_vfr_api.h"
extern uint32_t cvd_vfr_subscribe_to_vtm( uint32_t vfr_cycle_duration_us );

extern uint32_t cvd_vfr_unsubscribe_to_vtm( void );

extern uint32_t cvd_vfr_get_voice_frame_reference(
  uint64_t *vfrTimestamp
);

extern int32_t cvd_vfr_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
);
# 28 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/mvs_module_api.h" 1
# 15 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/mvs_module_api.h"
typedef enum mvs_callindex_enum_t
{
                MVS_CALLINDEX_ENUM_UNINITIALIZED = 0,
                MVS_CALLINDEX_ENUM_INIT = 1,
                MVS_CALLINDEX_ENUM_POSTINIT = 2,
                MVS_CALLINDEX_ENUM_PREDEINIT = 3,
                MVS_CALLINDEX_ENUM_DEINIT = 4,
                MVS_CALLINDEX_ENUM_RUN = 5,
                MVS_CALLINDEX_ENUM_INVALID
}
  mvs_callindex_enum_t;

extern int32_t mvs_call (
  mvs_callindex_enum_t index,
  void* params,
  uint32_t size
);
# 29 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h" 1
# 18 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_memmgr.h" 1
# 95 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_memmgr.h"
# 1 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/stddef.h" 1 3 4
# 42 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/stddef.h" 3 4
typedef _Ptrdifft ptrdiff_t;
# 52 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/stddef.h" 3 4
typedef _Sizet size_t;





typedef _Wchart wchar_t;


typedef long double max_align_t;
# 96 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_memmgr.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_memmgr_config.h" 1
# 97 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_memmgr.h" 2

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h" 1
# 118 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/comdef.h" 1
# 168 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/comdef.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/build/cust/coreapi_variation.h" 1
# 13 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/build/cust/coreapi_variation.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/build/cust/customer.h" 1



# 1 "./cust9205.ltenbgefi.prodq.h" 1
# 11 "./cust9205.ltenbgefi.prodq.h"
# 1 "./targ9205.ltenbgefi.prodq.h" 1
# 12 "./cust9205.ltenbgefi.prodq.h" 2
# 629 "./cust9205.ltenbgefi.prodq.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/build/cust/custglobal.h" 1
# 29 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/build/cust/custglobal.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/build/cust/cust_local_defines.h" 1
# 14 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/build/cust/cust_local_defines.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/build/cust/custcdma2000.h" 1
# 15 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/build/cust/cust_local_defines.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/build/cust/custmodem.h" 1
# 16 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/build/cust/cust_local_defines.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/build/cust/custrela.h" 1
# 17 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/build/cust/cust_local_defines.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/build/cust/custsrch.h" 1
# 18 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/build/cust/cust_local_defines.h" 2
# 30 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/build/cust/custglobal.h" 2


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/build/cust/rfa_local_defines.h" 1
# 33 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/build/cust/custglobal.h" 2
# 630 "./cust9205.ltenbgefi.prodq.h" 2
# 5 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/build/cust/customer.h" 2
# 14 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/build/cust/coreapi_variation.h" 2
# 169 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/comdef.h" 2



# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/com_dtypes.h" 1
# 151 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/com_dtypes.h"
typedef unsigned short uint16;
# 172 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/com_dtypes.h"
typedef signed short int16;
# 197 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/com_dtypes.h"
typedef unsigned short word;


typedef unsigned long dword;



typedef unsigned char uint1;


typedef unsigned short uint2;


typedef unsigned long uint4;



typedef signed char int1;


typedef signed short int2;


typedef long int int4;



typedef signed long sint31;


typedef signed short sint15;


typedef signed char sint7;

typedef uint16 UWord16 ;
typedef uint32 UWord32 ;
typedef int32 Word32 ;
typedef int16 Word16 ;
typedef uint8 UWord8 ;
typedef int8 Word8 ;
typedef int32 Vect32 ;
# 277 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/com_dtypes.h"
typedef uint64 UWord64 ;



typedef unsigned long uintnt;
# 173 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/comdef.h" 2
# 194 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/comdef.h"
typedef void * addr_t;
# 289 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/comdef.h"
        typedef struct __attribute__((packed))
        { uint16 x; }
        unaligned_uint16;
        typedef struct __attribute__((packed))
        { uint32 x; }
        unaligned_uint32;
        typedef struct __attribute__((packed))
        { uint64 x; }
        unaligned_uint64;
        typedef struct __attribute__((packed))
        { int16 x; }
        unaligned_int16;
        typedef struct __attribute__((packed))
        { int32 x; }
        unaligned_int32;
        typedef struct __attribute__((packed))
        { int64 x; }
        unaligned_int64;
# 864 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/comdef.h"
  extern dword rex_int_lock(void);
  extern dword rex_int_free(void);
# 1015 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/comdef.h"
   extern void rex_task_lock( void);
   extern void rex_task_free( void);
# 119 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h" 2



# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/amssassert.h" 1
# 142 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/amssassert.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h" 1
# 86 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h"
# 1 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/stdio.h" 1 3 4
# 60 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/stdio.h" 3 4
typedef struct _Mbstatet
 {
 unsigned long _Wchar;
 unsigned short _Byte, _State;
 } _Mbstatet;
# 77 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/stdio.h" 3 4
typedef struct fpos_t
 {
 _Longlong _Off;
 _Mbstatet _Wstate;
 } fpos_t;



struct _Dnk_filet
 {
 unsigned short _Mode;
 unsigned char _Idx;
 int _Handle;

 unsigned char *_Buf, *_Bend, *_Next;
 unsigned char *_Rend, *_Wend, *_Rback;

 _Wchart *_WRback, _WBack[2];
 unsigned char *_Rsave, *_WRend, *_WWend;

 _Mbstatet _Wstate;
 char *_Tmpnam;
 unsigned char _Back[8], _Cbuf;
 };



typedef struct _Dnk_filet _Filet;


typedef _Filet FILE;



int vasprintf(char ** strp, const char * fmt, va_list ap);

extern FILE _Stdin, _Stdout, _Stderr;

void clearerr(FILE *) __attribute__((__nothrow__));
int fclose(FILE *) __attribute__((__nothrow__));
int feof(FILE *) __attribute__((__nothrow__));
int ferror(FILE *) __attribute__((__nothrow__));
int fflush(FILE *) __attribute__((__nothrow__));
int fgetc(FILE *) __attribute__((__nothrow__));
int fgetpos(FILE *, fpos_t *) __attribute__((__nothrow__));
char *fgets(char *, int, FILE *) __attribute__((__nothrow__));
FILE *fopen(const char *, const char *) __attribute__((__nothrow__));





int fprintf(FILE *, const char *, ...) __attribute__((__nothrow__));
int fputc(int, FILE *) __attribute__((__nothrow__));
int fputs(const char *, FILE *) __attribute__((__nothrow__));
size_t fread(void *, size_t, size_t, FILE *) __attribute__((__nothrow__));
FILE *freopen(const char *, const char *,
 FILE *) __attribute__((__nothrow__));





int fscanf(FILE * , const char *, ...) __attribute__((__nothrow__));
int fseek(FILE *, long, int) __attribute__((__nothrow__));
int fsetpos(FILE *, const fpos_t *) __attribute__((__nothrow__));
long ftell(FILE *) __attribute__((__nothrow__));
size_t fwrite(const void *, size_t, size_t,
 FILE *) __attribute__((__nothrow__));
char *gets(char *) __attribute__((__nothrow__));
void perror(const char *) __attribute__((__nothrow__));

int fseeko (FILE *, long, int) __attribute__((__nothrow__));
long ftello (FILE *) __attribute__((__nothrow__));
int getchar_unlocked (void) __attribute__((__nothrow__));
int getc_unlocked (FILE *) __attribute__((__nothrow__));
int putchar_unlocked (int) __attribute__((__nothrow__));
int putc_unlocked (int, FILE *) __attribute__((__nothrow__));





int printf(const char *, ...) __attribute__((__nothrow__));
int puts(const char *) __attribute__((__nothrow__));
int remove(const char *) __attribute__((__nothrow__));
int rename(const char *, const char *) __attribute__((__nothrow__));
void rewind(FILE *) __attribute__((__nothrow__));





int scanf(const char *, ...) __attribute__((__nothrow__));
void setbuf(FILE * , char *) __attribute__((__nothrow__));
int setvbuf(FILE * , char *, int, size_t) __attribute__((__nothrow__));





int sprintf(char *, const char *, ...) __attribute__((__nothrow__));





int sscanf(const char *, const char *, ...) __attribute__((__nothrow__));
FILE *tmpfile(void) __attribute__((__nothrow__));
char *tmpnam(char *) __attribute__((__nothrow__));
int ungetc(int, FILE *) __attribute__((__nothrow__));
int vfprintf(FILE *, const char *, _Va_list) __attribute__((__nothrow__));
int vprintf(const char *, _Va_list) __attribute__((__nothrow__));
int vsprintf(char *, const char *, _Va_list) __attribute__((__nothrow__));


FILE *fdopen(int, const char *) __attribute__((__nothrow__));
int fileno(FILE *) __attribute__((__nothrow__));
int getw(FILE *) __attribute__((__nothrow__));
int putw(int, FILE *) __attribute__((__nothrow__));


long _Fgpos(FILE *, fpos_t *) __attribute__((__nothrow__));
int _Flocale(FILE *, const char *, int) __attribute__((__nothrow__));
void _Fsetlocale(FILE *, int) __attribute__((__nothrow__));
int _Fspos(FILE *, const fpos_t *, long, int) __attribute__((__nothrow__));


void _Lockfilelock(_Filet *) __attribute__((__nothrow__));
void _Unlockfilelock(_Filet *) __attribute__((__nothrow__));


extern FILE *_Files[20];







int snprintf(char *, size_t,
 const char *, ...) __attribute__((__nothrow__));
int vsnprintf(char *, size_t,
 const char *, _Va_list) __attribute__((__nothrow__));
int vfscanf(FILE *,
 const char *, _Va_list) __attribute__((__nothrow__));
int vscanf(const char *, _Va_list) __attribute__((__nothrow__));
int vsscanf(const char *,
 const char *, _Va_list) __attribute__((__nothrow__));
# 247 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/stdio.h" 3 4
int getc(FILE *) __attribute__((__nothrow__));
int getchar(void) __attribute__((__nothrow__));
int putc(int, FILE *) __attribute__((__nothrow__));
int putchar(int) __attribute__((__nothrow__));
# 87 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h" 2
# 117 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h"
typedef struct
{
 const char *fmt;
 const char *fname;
 uint16 line;
} err_FatalParam_type;



# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg.h" 1
# 77 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h" 1
# 90 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/qw.h" 1
# 82 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/qw.h"
# 1 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/string.h" 1 3 4
# 34 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/string.h" 3 4
int memcmp(const void *, const void *, size_t) __attribute__((__nothrow__));
void *memcpy(void *, const void *, size_t) __attribute__((__nothrow__));
void *memcpy_v(volatile void *, const volatile void *, size_t) __attribute__((__nothrow__));
void *memset(void *, int, size_t) __attribute__((__nothrow__));
char *strcat(char *, const char *) __attribute__((__nothrow__));
int strcmp(const char *, const char *) __attribute__((__nothrow__));
char *strcpy(char *, const char *) __attribute__((__nothrow__));
size_t strlen(const char *) __attribute__((__nothrow__));

void *memmove(void *, const void *, size_t) __attribute__((__nothrow__));
void *memmove_v(volatile void *, const volatile void *, size_t) __attribute__((__nothrow__));
int strcoll(const char *, const char *) __attribute__((__nothrow__));
size_t strcspn(const char *, const char *) __attribute__((__nothrow__));
char *strerror(int) __attribute__((__nothrow__));
size_t strlcat(char *, const char *, size_t) __attribute__((__nothrow__));
char *strncat(char *, const char *, size_t) __attribute__((__nothrow__));
int strncmp(const char *, const char *, size_t) __attribute__((__nothrow__));
size_t strlcpy(char *, const char *, size_t) __attribute__((__nothrow__));
char *strncpy(char *, const char *, size_t) __attribute__((__nothrow__));
size_t strspn(const char *, const char *) __attribute__((__nothrow__));
char *strtok(char *, const char *) __attribute__((__nothrow__));
char *strsep(char **, const char *) __attribute__((__nothrow__));
size_t strxfrm(char *, const char *, size_t) __attribute__((__nothrow__));


char *strdup(const char *) __attribute__((__nothrow__));
int strcasecmp(const char *, const char *) __attribute__((__nothrow__));
int strncasecmp(const char *, const char *, size_t) __attribute__((__nothrow__));
char *strtok_r(char *, const char *, char **) __attribute__((__nothrow__));

void *memccpy (void *, const void *, int, size_t) __attribute__((__nothrow__));
int strerror_r (int, char *, size_t) __attribute__((__nothrow__));
# 106 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/string.h" 3 4
char *strchr(const char *, int) __attribute__((__nothrow__));
char *strpbrk(const char *, const char *) __attribute__((__nothrow__));
char *strrchr(const char *, int) __attribute__((__nothrow__));
char *strstr(const char *, const char *) __attribute__((__nothrow__));
# 130 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/string.h" 3 4
void *memchr(const void *, int, size_t) __attribute__((__nothrow__));
# 83 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/qw.h" 2
# 94 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/qw.h"
typedef unsigned long qword[ 2 ];



  typedef unsigned long qc_qword[ 2 ];
# 184 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/qw.h"
void qw_equ_misaligned
(
  qc_qword qw1,
  qc_qword qw2
);
# 216 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/qw.h"
void qw_set
(
  qc_qword qw,
  uint32 hi,
  uint32 lo
);
# 237 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/qw.h"
void qw_equ
(
  qc_qword qw1,

  qc_qword qw2

);
# 257 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/qw.h"
uint32 qw_hi
(
  qc_qword qw
);
# 274 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/qw.h"
uint32 qw_lo
(
  qc_qword qw
);
# 292 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/qw.h"
void qw_inc
(
  qc_qword qw1,
  uint32 val
);
# 311 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/qw.h"
void qw_dec
(
  qc_qword qw1,
  uint32 val
);
# 332 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/qw.h"
void qw_add
(
  qc_qword sum,
  qc_qword addend,
  qc_qword adder
);
# 355 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/qw.h"
void qw_sub
(
  qc_qword difference,
  qc_qword subtrahend,
  qc_qword subtractor
);
# 377 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/qw.h"
void qw_mul
(
  qc_qword product,
  qc_qword multiplicand,
  uint32 multiplier
);
# 400 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/qw.h"
word qw_div
(
  qc_qword quotient,
  qc_qword dividend,
  word divisor
);
# 427 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/qw.h"
word qw_div_by_power_of_2
(
  qc_qword quotient,
  qc_qword dividend,
  unsigned short num_bits
);
# 449 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/qw.h"
void qw_shift
(
  qc_qword shifticand,
  int shiftidend
);
# 471 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/qw.h"
int qw_cmp
(
  qc_qword qw1,
  qc_qword qw2
);
# 91 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h" 1
# 80 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_pkt_defs.h" 1
# 102 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_pkt_defs.h"
typedef struct
{
  uint8 cmd_code;
  uint8 sub_cmd;
}
msg_get_ssid_ranges_req_type;
# 118 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_pkt_defs.h"
typedef struct
{
  uint8 cmd_code;
  uint8 sub_cmd;
  uint8 status;
  uint8 rsvd;

  uint32 range_cnt;

  struct
  {
    uint16 ssid_first;
    uint16 ssid_last;
  }
  ssid_ranges[1];
}
msg_get_ssid_ranges_rsp_type;
# 146 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_pkt_defs.h"
typedef struct
{
  uint8 cmd_code;
  uint8 sub_cmd;
  uint16 ssid_start;
  uint16 ssid_end;
}
msg_get_mask_req_type;
# 165 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_pkt_defs.h"
typedef struct
{
  uint8 cmd_code;
  uint8 sub_cmd;
  uint16 ssid_start;
  uint16 ssid_end;

  uint8 status;
  uint8 pad;

  uint32 bld_mask[1];


}
msg_get_mask_rsp_type;
# 190 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_pkt_defs.h"
typedef struct
{
  uint8 cmd_code;
  uint8 sub_cmd;
  uint16 ssid_start;
  uint16 ssid_end;
  uint16 pad;
  uint32 rt_mask[1];

}
msg_set_rt_mask_req_type;
# 211 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_pkt_defs.h"
typedef struct
{
  uint8 cmd_code;
  uint8 sub_cmd;
  uint16 ssid_start;
  uint16 ssid_end;
  uint8 status;
  uint8 pad;
  uint32 rt_mask[1];

}
msg_set_rt_mask_rsp_type;
# 233 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_pkt_defs.h"
typedef struct
{
  uint8 cmd_code;
  uint8 sub_cmd;
  uint16 rsvd;
  uint32 rt_mask;
}
msg_set_all_masks_req_type;
# 251 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_pkt_defs.h"
typedef struct
{
  uint8 cmd_code;
  uint8 sub_cmd;
  uint8 status;
  uint8 rsvd;
  uint32 rt_mask;
}
msg_set_all_masks_rsp_type;
# 272 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_pkt_defs.h"
typedef struct
{
  uint16 line;
  uint16 ss_id;
  uint32 ss_mask;
}
msg_desc_type;






typedef struct
{
  uint8 cmd_code;
  uint8 ts_type;
  uint8 num_args;
  uint8 drop_cnt;
  qword ts;
}
msg_hdr_type;






typedef enum
{
  MSG_TS_TYPE_CDMA_FULL = 0,
  MSG_TS_TYPE_WIN32,
  MSG_TS_TYPE_GW
}
msg_ts_type;
# 323 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_pkt_defs.h"
typedef struct
{
  msg_hdr_type hdr;
  msg_desc_type desc;
  uint32 args[1];


}
msg_ext_type;
# 355 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_pkt_defs.h"
typedef struct __attribute__((__packed__))
{
  byte cmd_code;
  word msg_level;
}
msg_legacy_req_type;






typedef struct __attribute__((__packed__))
{
  byte cmd_code;
  word qty;



  dword drop_cnt;
  dword total_msgs;
  byte level;
  char file[(12+1)];
  word line;
  char fmt[40];
  dword code1;
  dword code2;
  dword code3;
  qword time;
}
msg_legacy_rsp_type;
# 81 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msgcfg.h" 1
# 132 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msgcfg.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msgtgt.h" 1
# 139 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msgtgt.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_mask.h" 1
# 140 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msgtgt.h" 2
# 133 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msgcfg.h" 2
# 82 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h" 2
# 151 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
typedef struct
{
  msg_desc_type desc;
  const char *fmt;
  const char *fname;
}
msg_const_type;







typedef struct
{
  msg_hdr_type hdr;
  const msg_const_type *const_data_ptr;
  uint32 args[1];
}
msg_ext_store_type;

typedef struct
{
  uint16 rt_mask;
  uint16 ss_id;
  uint32 ss_mask;




} msg_v3_ssid_table_entry_type;

typedef struct
{
  uint32 start_address;
  uint32 index_to_v4_ssid_table;
} msg_v4_ss_uam_table_entry_type;


typedef struct
{
  uint32 index_to_v4_ssid_table;
} msg_v4_ss_aam_table_entry_type;




typedef msg_v3_ssid_table_entry_type msg_v3_ssid_table_type[1];
# 212 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
typedef msg_v3_ssid_table_entry_type msg_v4_ssid_table_type[512];
typedef msg_v4_ss_aam_table_entry_type msg_v4_ss_aam_table_type[512];
typedef msg_v4_ss_uam_table_entry_type msg_v4_ss_uam_table_type[100];
typedef char* msg_v4_dbase_name_type[50];
# 239 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
typedef struct
{
  msg_desc_type desc;
  uint32 msg_hash;
}
msg_qsr_const_type;
# 259 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
typedef struct
{
  msg_qsr_const_type qsr_const_blk;
  const char *fname;
} err_msg_qsr_const_type;
# 279 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
typedef struct
{
  msg_hdr_type hdr;
  const msg_qsr_const_type* qsr_const_data_ptr;
  uint32 qsr_flag;
  uint32 args[1];
}
msg_qsr_store_type;
# 303 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
typedef struct
{
  uint16 ssid_index;
  uint16 line;
  uint32 msg;

}
msg_v3_const_hash;



typedef struct
{
  uint32 ss_mask;
  const char *msg;
}
msg_v3_const_string;


typedef union
{
  msg_v3_const_string msg_const_string;
  msg_v3_const_hash qsr_const_blk;
}
msg_v3_const_type;







typedef union
{
  const msg_qsr_const_type* msg_desc;
  const msg_v3_const_hash* v3_index;
}
msg_qsr_const_ptr;
# 350 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
typedef struct
{
  msg_hdr_type hdr;
  msg_qsr_const_ptr qsr_const_data_ptr;
  uint32 qsr_flag;
  uint32 args[1];
}
msg_v3_store_type;

typedef struct
{
  uint8 cmd_code;
  uint8 version;
  uint16 proc_id;
  uint32 id;
  msg_v3_store_type msg;
}
msg_v3_multi_client_store_type;

typedef struct
{
  msg_v3_const_type msg_v3_const_blk;
  const char *fname;
} err_msg_v3_const_type;





 typedef struct
 {
   msg_v3_const_string msg_const_string;
 }
 msg_v4_const_type;

typedef struct __attribute__((__packed__)) {
  msg_hdr_type hdr;
  uint32 qsr_hash;
  uint8 dbase_guid[2];
  uint32 args[1];
 }msg_v4_store_type;

typedef struct
{
  uint8 cmd_code;
  uint8 version;
  uint16 proc_id;
  uint32 id;
  msg_v4_store_type msg;
}
msg_v4_multi_client_store_type;
# 458 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
typedef struct
{
  msg_desc_type desc;
  const char * msg;

}
msg_v2_const_type;

typedef struct
{
  msg_v2_const_type msg_v2_const_blk;
  const char *fname;
} err_msg_v2_const_type;
# 760 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v2_send ( const msg_v2_const_type * xx_msg_const_ptr);
# 787 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v2_send_dll ( const msg_v2_const_type * xx_msg_const_ptr);
# 813 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v3_send ( const msg_v3_const_type * xx_msg_const_ptr) __attribute__((hexmsgabi));
# 839 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v4_send (const msg_v4_const_type * xx_msg_const_ptr);
# 878 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v3_send_ext ( const msg_v3_const_type * xx_msg_const_ptr, uint32 version, void* parameter);
# 915 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v4_send_ext ( const msg_v4_const_type * xx_msg_const_ptr, uint32 version, void* parameter);
# 968 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v2_send_1 (const msg_v2_const_type * xx_msg_const_ptr, uint32 xx_arg1);
# 995 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v2_send_1_dll (const msg_v2_const_type * xx_msg_const_ptr, uint32 xx_arg1);
# 1022 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v3_send_1 (const msg_v3_const_type * xx_msg_const_ptr, uint32 xx_arg1)__attribute__((hexmsgabi));
# 1048 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v4_send_1 (const msg_v4_const_type * xx_msg_const_ptr, uint32 xx_arg1);
# 1087 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v3_send_1_ext (const msg_v3_const_type * xx_msg_const_ptr, uint32 version, void* parameter, uint32 xx_arg1);
# 1126 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v4_send_1_ext (const msg_v4_const_type * xx_msg_const_ptr, uint32 version, void* parameter, uint32 xx_arg1);
# 1183 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v2_send_2 ( const msg_v2_const_type * xx_msg_const_ptr,uint32 xx_arg1,
    uint32 xx_arg2);
# 1213 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v2_send_2_dll ( const msg_v2_const_type * xx_msg_const_ptr,uint32 xx_arg1,
    uint32 xx_arg2);
# 1242 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v3_send_2 ( const msg_v3_const_type * xx_msg_const_ptr,uint32 xx_arg1,uint32 xx_arg2) __attribute__((hexmsgabi));
# 1269 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v4_send_2 (const msg_v4_const_type * xx_msg_const_ptr, uint32 xx_arg1,uint32 xx_arg2);
# 1309 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v3_send_2_ext ( const msg_v3_const_type * xx_msg_const_ptr, uint32 version, void* parameter,
                           uint32 xx_arg1, uint32 xx_arg2);
# 1350 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v4_send_2_ext ( const msg_v4_const_type * xx_msg_const_ptr, uint32 version, void* parameter,
                           uint32 xx_arg1, uint32 xx_arg2);
# 1411 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v2_send_3 ( const msg_v2_const_type * xx_msg_const_ptr, uint32 xx_arg1,
    uint32 xx_arg2, uint32 xx_arg3);
# 1442 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v2_send_3_dll ( const msg_v2_const_type * xx_msg_const_ptr, uint32 xx_arg1,
    uint32 xx_arg2, uint32 xx_arg3);
# 1472 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v3_send_3 ( const msg_v3_const_type * xx_msg_const_ptr, uint32 xx_arg1,
    uint32 xx_arg2, uint32 xx_arg3)__attribute__((hexmsgabi));
# 1501 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v4_send_3 ( const msg_v4_const_type * xx_msg_const_ptr, uint32 xx_arg1,uint32 xx_arg2, uint32 xx_arg3);
# 1542 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v3_send_3_ext ( const msg_v3_const_type * xx_msg_const_ptr, uint32 version, void* parameter,
                           uint32 xx_arg1, uint32 xx_arg2, uint32 xx_arg3);
# 1584 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v4_send_3_ext ( const msg_v4_const_type * xx_msg_const_ptr, uint32 version, void* parameter,
                           uint32 xx_arg1, uint32 xx_arg2, uint32 xx_arg3);
# 1642 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v2_send_var ( const msg_v2_const_type * xx_msg_const_ptr, uint32 num_args, ...);
# 1670 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v2_send_var_dll ( const msg_v2_const_type * xx_msg_const_ptr, uint32 num_args, ...);
# 1698 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v3_send_var ( const msg_v3_const_type * xx_msg_const_ptr, uint32 num_args, ...);
# 1725 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v4_send_var(const msg_v4_const_type * xx_msg_const_ptr, uint32 num_args, ...);
# 1765 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v3_send_var_ext ( const msg_v3_const_type * xx_msg_const_ptr, uint32 version, void* parameter,
                              uint32 num_args, ...);
# 1806 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
  void msg_v4_send_var_ext ( const msg_v4_const_type * xx_msg_const_ptr, uint32 version, void* parameter,
                              uint32 num_args, ...);
# 2066 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
void msg_v3_errlog_3 (const err_msg_v3_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3);
# 2095 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
void msg_v4_errlog_3 ( const msg_v4_const_type * const_blk, uint32 code1, uint32 code2, uint32 code3,const char * file,uint32 line);
# 2127 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
void msg_v3_errlog_3_ext (const err_msg_v3_const_type* const_blk, uint32 version, void* parameter, uint32 code1, uint32 code2, uint32 code3);
# 2155 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
void msg_v3_errlog_2 (const err_msg_v3_const_type* const_blk, uint32 code1, uint32 code2);
# 2184 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
void msg_v4_errlog_2 ( const msg_v4_const_type * const_blk, uint32 code1, uint32 code2,const char * file, uint32 line);
# 2216 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
void msg_v3_errlog_2_ext (const err_msg_v3_const_type* const_blk, uint32 version, void* parameter, uint32 code1, uint32 code2);
# 2244 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
void msg_v3_errlog_1 (const err_msg_v3_const_type* const_blk, uint32 code1);
# 2273 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
void msg_v4_errlog_1 ( const msg_v4_const_type * const_blk, uint32 code1,const char * file,uint32 line);
# 2305 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
void msg_v3_errlog_1_ext (const err_msg_v3_const_type* const_blk, uint32 version, void* parameter, uint32 code1);
# 2333 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
void msg_v3_errlog_0 (const err_msg_v3_const_type* const_blk);
# 2363 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
void msg_v4_errlog_0 ( const msg_v4_const_type * const_blk,const char * file,uint32 line);
# 2395 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
void msg_v3_errlog_0_ext (const err_msg_v3_const_type* const_blk, uint32 version, void* parameter);
# 2434 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
void msg_v4_errlog_3_ext ( const msg_v4_const_type * const_blk, uint32 version, void* parameter, uint32 code1, uint32 code2, uint32 code3, const char * file,uint32 line);
# 2471 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
void msg_v4_errlog_2_ext ( const msg_v4_const_type * const_blk, uint32 version, void* parameter, uint32 code1, uint32 code2, const char * file,uint32 line);
# 2507 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
void msg_v4_errlog_1_ext ( const msg_v4_const_type * const_blk, uint32 version, void* parameter, uint32 code1, const char * file,uint32 line);
# 2542 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_qsr.h"
void msg_v4_errlog_0_ext ( const msg_v4_const_type * const_blk, uint32 version, void* parameter, const char * file,uint32 line);
# 92 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h" 2
# 112 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h"
typedef struct __attribute__((__packed__))
{
  uint8 cmd_code;
  uint8 version;
  uint16 proc_id;
  uint32 id;
  msg_ext_store_type msg;
}
msg_send_multi_client_type;
# 1208 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h"
   void msg_init (void);
# 1234 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h"
  void msg_send (const msg_const_type * xx_msg_const_ptr);
# 1261 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h"
  void msg_send_1 (const msg_const_type * xx_msg_const_ptr, uint32 xx_arg1);
# 1289 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h"
  void msg_send_2 (const msg_const_type * xx_msg_const_ptr, uint32 xx_arg1,
    uint32 xx_arg2);
# 1319 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h"
  void msg_send_3 (const msg_const_type * xx_msg_const_ptr, uint32 xx_arg1,
    uint32 xx_arg2, uint32 xx_arg3);
# 1348 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h"
  void msg_send_var (const msg_const_type * xx_msg_const_ptr, uint32 num_args,
    ...);
# 1376 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h"
  void msg_sprintf(const msg_const_type * const_blk,...);
# 1411 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h"
  void msg_sprintf_ext (uint32 version, void * reserved, uint32 num_args, const msg_const_type * const_blk, ...);
# 1440 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h"
  void msg_send_ts(const msg_const_type *const_blk, uint64 timestamp);
# 1474 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h"
void msg_send_ts_ext (const msg_const_type * const_blk, uint32 version, void *parameter, uint64 timestamp);
# 1502 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h"
void msg_save_3(const msg_const_type* const_blk,
                uint32 xx_arg1, uint32 xx_arg2, uint32 xx_arg3,
                msg_ext_store_type* msg);
# 1532 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h"
void msg_errlog_3 (const msg_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3);
# 1560 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h"
void msg_errlog_2 (const msg_const_type* const_blk, uint32 code1, uint32 code2);
# 1587 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h"
void msg_errlog_1 (const msg_const_type* const_blk, uint32 code1);
# 1613 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h"
void msg_errlog_0 (const msg_const_type* const_blk);
# 1735 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h"
boolean msg_status(uint16 ss_id, uint32 ss_mask);
# 1757 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg_diag_service.h"
boolean msg_get_qsr_table_entry(uint16 ssid_index, uint16 *msg_ssid, uint32 *msg_mask);
# 78 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/msg.h" 2
# 126 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h" 2
# 137 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h"
typedef void (*err_cb_ptr)(void);
typedef void (*err_cb_ptr_u32)(uint32);



typedef enum
{
  ERR_DONE_S,
  ERR_NONE_S
}
err_stat_type;


extern const char err_generic_msg[];
extern const char err_generic_msg_dynamic[];
# 189 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h"
  void err_init (void);
# 213 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h"
boolean err_crash_cb_dereg(err_cb_ptr cb);
boolean err_crash_cb_register(err_cb_ptr cb);
# 240 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h"
  err_stat_type err_clr_log (byte log_num);
# 262 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h"
  void err_put_log (word line, const char *file_ptr);
# 273 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h"
void err_enable_fs_log (void);
# 284 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h"
void err_disable_fs_log (void);
# 294 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h"
void err_fatal_lock( void );
# 397 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h"
  extern void jettison_core( void );


typedef struct __attribute__((packed))
{
  uint16 line;
  const char *fmt;
  const char *fname;
}
err_const_type;
# 422 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h"
#pragma GCC diagnostic ignored "-Wstatic-in-inline"
static const char __THIS_FILENAME__[] __attribute__ ((aligned(1),section (".fatal.file.rodata." ))) = "cvd_devcfg.c";
# 443 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h"
void err_Fatal_internal3 (const err_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3) __attribute__((noreturn)) ;
void err_Fatal_internal2 (const err_const_type* const_blk, uint32 code1, uint32 code2) __attribute__((noreturn)) ;
void err_Fatal_internal1 (const err_const_type* const_blk, uint32 code1) __attribute__((noreturn)) ;
void err_Fatal_internal0 (const err_const_type* const_blk) __attribute__((noreturn)) ;
# 506 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h"
void err_Fatal_aux_msg
(
  const err_const_type* const_blk,
  const char * dynamic_msg,
  uint16 size_dynamic_msg
);
# 543 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h"
void err_exception_handler(void) __attribute__((noreturn));
# 563 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h"
boolean err_crash_cb_postflush_register (err_cb_ptr cb);
# 583 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h"
boolean err_crash_cb_postflush_deregister (err_cb_ptr cb);
# 609 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h"
boolean err_crash_cb_reg_pre_STM(err_cb_ptr_u32 cb);
# 630 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h"
boolean err_crash_cb_dereg_pre_STM(err_cb_ptr_u32 cb);
# 657 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h"
boolean err_crash_cb_reg_next_to_STM(err_cb_ptr cb);
# 677 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/debugtools/err.h"
boolean err_crash_cb_dereg_next_to_STM(err_cb_ptr cb);
# 143 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/amssassert.h" 2
# 123 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h" 2




# 1 "/pkg/qct/software/hexagon/releases/tools/8.3.03/Tools/bin/../target/hexagon/include/stddef.h" 1 3 4
# 128 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h" 2
# 172 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
struct mem_heap_struct;
# 195 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
typedef void (*mem_allocator_failed_proc_type)(
   struct mem_heap_struct *heap_ptr,
   size_t request_size
);
# 216 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
typedef void (*mem_lock_fnc_type)( void * ptr);
# 230 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
typedef void (*mem_free_fnc_type)( void * ptr);


typedef struct {
   void *caller_ptr;
   uint32 stats_index;
}mem_block_header_debug_info;
# 246 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
typedef struct mem_block_header_struct {
  uint16 header_guard;

  unsigned char extra;
  unsigned char section_index:5;
  unsigned char start_flag:1;
  unsigned char free_flag:1;

  unsigned char last_flag:1;

  unsigned long forw_offset;



  mem_block_header_debug_info header_debug_info;

} mem_block_header_type;

typedef struct mem_block_header_free_struct {
  uint16 header_guard;

  unsigned char extra;
  unsigned char section_index:5;
  unsigned char start_flag:1;
  unsigned char free_flag:1;

  unsigned char last_flag:1;

  unsigned long forw_offset;


} mem_block_header_free_struct;






typedef struct freeBlockList_struct {
   struct mem_block_header_free_struct freeBlock;
   struct freeBlockList_struct *nextPtr;
}freeBlockList;

typedef struct _BIN{
  freeBlockList *firstNode;
  freeBlockList *lastNode;
  uint16 binToUSeForMalloc;
  uint32 totalNodes;
}binStruct;

typedef struct _mem_heap_section
{
   mem_block_header_type *start_addr;
   uint32 size;
} mem_heap_section;






typedef struct
{
  uint32 current_usage;
  uint32 max_usage;
  uint32 quota;
} memheap_task_stats_type;
# 330 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
typedef uint32 (*memheap_get_quota_func_type)(void);



typedef void (*memheap_quota_exceeded_cb_type)(
   struct mem_heap_struct *heap_ptr,
   uint32 tid,
   size_t max_used
);


typedef struct
{
   uint32 task_quota;
}memheap_quota_type;


typedef struct __attribute__ ((aligned(8))) mem_heap_struct {
  uint32 magic_num;
  mem_block_header_type *first_block;

  mem_block_header_type *next_block;

  unsigned long total_blocks;

  unsigned long total_bytes;

  unsigned long used_bytes;

  unsigned long max_used;

  unsigned long max_request;

  mem_allocator_failed_proc_type fail_fnc_ptr;

  mem_lock_fnc_type lock_fnc_ptr;
  mem_free_fnc_type free_fnc_ptr;



  memheap_get_quota_func_type mem_get_quota_fnc_ptr;

  memheap_quota_exceeded_cb_type quota_cb;

  uint8 memheap_crit_sect[64] __attribute__ ((aligned(8)));
  binStruct BIN[32];
  uint32 legacy_check;


  uint16 magic_num_free;


  uint16 magic_num_used;



  uint16 magic_num_index;
  mem_heap_section sections[(16)];
  uint16 number_of_sections;


  memheap_task_stats_type *heapStatistics;

  unsigned int mem_region;
  void *incomingBlock;


} mem_heap_type;
# 412 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
enum {
  kUseLegacyImpl = 0,
  kUseOnlyLLImpl = 1,
  kUseBINsImpl = 2,
};

enum
{
   MEM_TLS_CREATE_ERR = 0x1,
   MEM_TLS_SET_ERR = 0x2,
   MEM_TLS_GET_ERR = 0x4
};
# 469 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
void mem_init_heap(
   mem_heap_type *heap_ptr,
   void *heap_mem_ptr,
   unsigned long heap_mem_size,
   mem_allocator_failed_proc_type fail_fnc_ptr
);
# 511 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
void mem_init_heap_lite(
   mem_heap_type *heap_ptr,
   void *heap_mem_ptr,
   unsigned long heap_mem_size,
   mem_allocator_failed_proc_type fail_fnc_ptr
);
# 531 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
void mem_deinit_heap(
   mem_heap_type *heap_ptr
);
# 545 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
void print_memheap_free_error_log(void);
# 572 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
void* mem_calloc(
  mem_heap_type *heap_ptr,
  size_t elt_count,
  size_t elt_size
);
# 608 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
void* mem_malloc(
  mem_heap_type *heap_ptr,
  size_t size
);
# 672 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
void *mem_realloc(
  mem_heap_type *heap_ptr,
  void *ptr,
  size_t size
);
# 707 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
void mem_free(
  mem_heap_type *heap_ptr,
  void *ptr
);
# 732 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
size_t mem_heap_used (mem_heap_type *heap_ptr);
# 746 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
size_t mem_heap_overhead (mem_heap_type *heap_ptr);
# 760 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
size_t mem_heap_consumed (mem_heap_type *heap_ptr);
# 773 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
extern mem_allocator_failed_proc_type mem_allocator_failed_hook;


extern uint32 memheap_tid_to_tls[1024];





enum {
  kBlockFree = 1,
  kBlockUsed = 0,
  kLastBlock = 1,
  kStartBlock = 1,
  kMinChunkSize = 16,
  kMinBlockSize = 32,
};
# 806 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
unsigned long mem_get_block_size (
 mem_heap_type *heap_ptr,
 void *ptr
);
# 824 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
unsigned long mem_get_block_logical_size(
   const mem_block_header_type *block_hdr_ptr,
   mem_heap_type *heap_ptr
);



typedef struct mem_heap_block_iterator_struct {
  const mem_heap_type *mAllocator;

  mem_block_header_type *mBlock;

} mem_heap_block_iterator_type;



typedef struct mem_heap_totals_struct {
  unsigned long total_physical_bytes;
  unsigned long current_block_count;
  unsigned long used_blocks;
  unsigned long header_bytes;
  unsigned long used_bytes;
  unsigned long free_bytes;
  unsigned long wasted_bytes;


  unsigned long largest_free_block;
  unsigned long max_logical_used;

  unsigned long max_logical_request;
} mem_heap_totals_type;
# 872 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
void mem_heap_block_iterator_init(
   mem_heap_block_iterator_type *block_iter_ptr,
   const mem_heap_type *heap_ptr
);
# 890 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
void mem_heap_block_iterator_reset(
   mem_heap_block_iterator_type *block_iter_ptr


);
# 929 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
int mem_heap_block_iterator_next(
   mem_heap_block_iterator_type *block_iter_ptr,
   mem_block_header_type *out_block_ptr,
   int fake_free_coalesced
);
# 949 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
void mem_heap_get_totals(
   mem_heap_type *heap_ptr,
   mem_heap_totals_type *totals
);
# 968 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
int
mem_heap_set_int_lock( mem_heap_type *heap_ptr );
# 985 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
int
mem_heap_set_task_lock( mem_heap_type *heap_ptr );
# 1002 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
int
mem_heap_set_no_lock( mem_heap_type *heap_ptr );
# 1020 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
int
mem_heap_set_crit_sect( mem_heap_type *heap_ptr );
# 1040 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
void
mem_heap_get_freeblock_info(
  mem_heap_type *heap_ptr,
  unsigned int num_blocks,
  unsigned int *buf
);
# 1059 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
unsigned int
mem_heap_get_chunk_size( mem_heap_type *heap_ptr );
# 1078 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
int memheap_collect_heap_statistics(mem_heap_type *heap_ptr);
# 1096 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
static __inline int memheap_get_task_stats(mem_heap_type *heap_ptr, uint32 thread_id, memheap_task_stats_type **stats) {

   uint32 tls_index;
   if(((0) == stats) || ((0) == heap_ptr))
   { return 2; }
   if ((0) == heap_ptr->heapStatistics)
   { return 4; }
   thread_id=thread_id&0xFFF;
   if(thread_id >= 1024)
  {
     return 2;
   }

   tls_index = memheap_tid_to_tls[thread_id];
   if(tls_index >= 384)
   {
      return 3;
   }

   *stats = &heap_ptr->heapStatistics[tls_index];
   return 0;
}
# 1134 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
int memheap_get_task_name(mem_heap_type *heap_ptr, uint32 client_id,char **tname);
# 1152 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
void* mem_memalign(
  mem_heap_type *heap_ptr,
  size_t block_size,
  size_t size
);
# 1170 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/memheap.h"
void mem_heap_add_section(

   mem_heap_type *heap_ptr,

   void *section_start,

   unsigned long section_size);
# 99 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_memmgr.h" 2
# 126 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_memmgr.h"
typedef struct apr_memmgr_type{
 char opaque_data[( 2048 )];
}apr_memmgr_type;
# 142 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_memmgr.h"
typedef void (*apr_memmgr_allocator_failed_proc_type)(
   struct apr_memmgr_type *heap_ptr,


   size_t request_size


);





typedef int (*apr_memmgr_heap_deny_access_fnc_type)( void );
# 173 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_memmgr.h"
typedef void (*apr_memmgr_lock_fnc_type)( void * ptr);
# 187 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_memmgr.h"
typedef void (*apr_memmgr_free_fnc_type)( void * ptr);
# 215 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_memmgr.h"
extern void apr_memmgr_init_heap(
   apr_memmgr_type *heap_ptr,


   void *heap_mem_ptr,


   unsigned long heap_mem_size,


   apr_memmgr_allocator_failed_proc_type fail_fnc_ptr,


   apr_memmgr_heap_deny_access_fnc_type deny_fnc_ptr


);
# 265 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_memmgr.h"
   extern void* apr_memmgr_calloc(
      apr_memmgr_type *heap_ptr,


      size_t elt_count,


      size_t elt_size


   );
# 317 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_memmgr.h"
   extern void* apr_memmgr_malloc(
      apr_memmgr_type *heap_ptr,


      size_t size


   );
# 384 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_memmgr.h"
   extern void *apr_memmgr_realloc(
      apr_memmgr_type *heap_ptr,


      void *ptr,


      size_t size


   );
# 435 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_memmgr.h"
   extern void apr_memmgr_free(
      apr_memmgr_type *heap_ptr,


      void *ptr


   );
# 463 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_memmgr.h"
   extern int apr_memmgr_heap_set_lock(
   apr_memmgr_type *heap_ptr,
   apr_memmgr_lock_fnc_type lock_fnc_ptr,
   apr_memmgr_free_fnc_type free_fnc_ptr
);
# 477 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_memmgr.h"
   extern int apr_memmgr_heap_set_no_lock( apr_memmgr_type *heap_ptr );
# 19 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_objmgr.h" 1
# 17 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_objmgr.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_list.h" 1
# 15 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_list.h"
typedef struct apr_list_node_t apr_list_node_t;
typedef struct apr_list_t apr_list_t;

typedef void ( *apr_list_lock_enter_fn_t ) ( void );
typedef void ( *apr_list_lock_leave_fn_t ) ( void );

struct apr_list_node_t
{
  apr_list_t* list;

  apr_list_node_t* prev;

  apr_list_node_t* next;

};

struct apr_list_t
{
  apr_list_node_t dummy;

  uint32_t size;


  apr_list_lock_enter_fn_t lock_fn;

  apr_list_lock_leave_fn_t unlock_fn;


  bool_t en_secure;

};

extern int32_t apr_list_init ( apr_list_t* list, apr_list_lock_enter_fn_t lock_fn, apr_list_lock_leave_fn_t unlock_fn );
extern int32_t apr_list_init_v2 ( apr_list_t* list, apr_list_lock_enter_fn_t lock_fn, apr_list_lock_leave_fn_t unlock_fn );
extern int32_t apr_list_destroy ( apr_list_t* list );
extern int32_t apr_list_init_node ( apr_list_node_t* node );
extern int32_t apr_list_add_head ( apr_list_t* list, apr_list_node_t* node );
extern int32_t apr_list_add_tail ( apr_list_t* list, apr_list_node_t* node );
extern int32_t apr_list_peak_head ( apr_list_t* list, apr_list_node_t** ret_node );
extern int32_t apr_list_peak_tail ( apr_list_t* list, apr_list_node_t** ret_node );
extern int32_t apr_list_remove_head ( apr_list_t* list, apr_list_node_t** ret_node );
extern int32_t apr_list_remove_tail ( apr_list_t* list, apr_list_node_t** ret_node );
extern int32_t apr_list_get_next( apr_list_t* list, apr_list_node_t* pivot_node, apr_list_node_t** ret_node );
extern int32_t apr_list_get_prev( apr_list_t* list, apr_list_node_t* pivot_node, apr_list_node_t** ret_node );
extern int32_t apr_list_delete( apr_list_t* list, apr_list_node_t* node );
extern int32_t apr_list_clear ( apr_list_t* list );
# 18 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_objmgr.h" 2
# 29 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_objmgr.h"
typedef union apr_objmgr_any_pod_type_t
{
  int8_t i8;
  uint8_t u8;
  int16_t i16;
  uint16_t u16;
  int32_t i32;
  uint32_t u32;
  void* ptr;
}
  apr_objmgr_any_pod_type_t;




typedef struct apr_objmgr_object_t
{
  apr_list_node_t link;
  uint32_t handle;
# 93 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_objmgr.h"
  uint32_t type;

  apr_objmgr_any_pod_type_t any;

}
  apr_objmgr_object_t;





typedef void ( *apr_objmgr_lock_fn_type )( void );
typedef void ( *apr_objmgr_unlock_fn_type )( void );

typedef struct apr_objmgr_t
{
  apr_list_t free_q;

  apr_objmgr_object_t* table;

  uint32_t total_num;
# 141 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_objmgr.h"
  uint32_t index_mask;

  uint32_t overflow_mask;




  apr_objmgr_lock_fn_type lock_fn;
  apr_objmgr_unlock_fn_type unlock_fn;
}
  apr_objmgr_t;

typedef struct apr_objmgr_setup_params_t
{
  apr_objmgr_object_t* table;

  uint32_t total_bits;

  uint32_t index_bits;

  apr_objmgr_lock_fn_type lock_fn;

  apr_objmgr_unlock_fn_type unlock_fn;

}
  apr_objmgr_setup_params_t;
# 179 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_objmgr.h"
extern int32_t apr_objmgr_construct (
  apr_objmgr_t* objmgr,
  apr_objmgr_setup_params_t* params
);
# 191 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_objmgr.h"
extern int32_t apr_objmgr_destruct (
  apr_objmgr_t* objmgr
);
# 203 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_objmgr.h"
extern int32_t apr_objmgr_alloc_object (
  apr_objmgr_t* objmgr,
  apr_objmgr_object_t** ret_object
);
# 217 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_objmgr.h"
extern int32_t apr_objmgr_find_object (
  apr_objmgr_t* objmgr,
  uint32_t handle,
  apr_objmgr_object_t** ret_object
);
# 231 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/utils/inc/apr_objmgr.h"
extern int32_t apr_objmgr_free_object (
  apr_objmgr_t* objmgr,
  uint32_t handle
);
# 20 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h" 2
# 39 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h"
typedef union cvd_cal_na_value_t
{
  uint8_t uint8_val;

  uint16_t uint16_val;

  uint32_t uint32_val;

  uint64_t uint64_val;

}
cvd_cal_na_value_t;

typedef struct cvd_cal_column_descriptor_t cvd_cal_column_descriptor_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 55 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h" 2

struct cvd_cal_column_descriptor_t
{
  uint32_t id;
  uint32_t type;
  cvd_cal_na_value_t na_value;
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 63 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h" 2
;

typedef struct cvd_cal_table_descriptor_t cvd_cal_table_descriptor_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 68 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h" 2

struct cvd_cal_table_descriptor_t
{
  void* start_ptr;
  uint32_t size;
  uint32_t data_mem_handle;
  uint32_t indexing_mem_handle;
  uint32_t num_columns;
  cvd_cal_column_descriptor_t* columns;
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 79 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h" 2
;

typedef struct cvd_cal_column_t cvd_cal_column_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 84 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h" 2

struct cvd_cal_column_t
{
  uint32_t id;
  uint32_t value;
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 91 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h" 2
;

typedef struct cvd_cal_table_handle_t cvd_cal_table_handle_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 96 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h" 2

struct cvd_cal_table_handle_t
{
  cvd_cal_table_descriptor_t table_descriptor;

  uint8_t reorder_key_buf[ ( 80 ) ];
  void* aux_info;

  uint32_t meta_size;
  bool_t is_parsed;
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 108 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h" 2
;

typedef struct cvd_cal_key_t cvd_cal_key_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 113 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h" 2

struct cvd_cal_key_t
{
  uint32_t num_columns;
  cvd_cal_column_t* columns;
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 120 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h" 2
;

typedef struct cvd_cal_entry_t cvd_cal_entry_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 125 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h" 2

struct cvd_cal_entry_t
{
  void* start_ptr;
  uint32_t size;
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 132 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h" 2
;




typedef enum cvd_cal_object_type_enum_t
{
  CVD_CAL_OBJECT_TYPE_ENUM_UNINITIALIZED,
  CVD_CAL_OBJECT_TYPE_ENUM_TABLE,
  CVD_CAL_OBJECT_TYPE_ENUM_QUERY,
  CVD_CAL_OBJECT_TYPE_ENUM_INVALID
}
  cvd_cal_object_type_enum_t;

typedef struct cvd_cal_object_header_t
{
  uint32_t handle;

  cvd_cal_object_type_enum_t type;






}
  cvd_cal_object_header_t;

typedef struct cvd_cal_table_object_t cvd_cal_table_object_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 163 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h" 2

struct cvd_cal_table_object_t
{
  cvd_cal_object_header_t header;
  uint32_t major_version;
  uint32_t minor_version;
  void* start_ptr;
  uint32_t size;
  uint32_t data_mem_handle;
  uint32_t num_columns;
  cvd_cal_column_descriptor_t* columns;
  uint8_t reorder_key_buf[ ( 80 ) ];
  void* lut_start;
  uint32_t lut_size;
  void* cdft_start;
  uint32_t cdft_size;
  void* cdot_start;
  uint32_t cdot_size;
  void* data_start;
  uint32_t data_size;
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 185 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h" 2
;

typedef struct cvd_cal_query_object_t cvd_cal_query_object_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 190 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h" 2

struct cvd_cal_query_object_t
{
  cvd_cal_object_header_t header;
  void* matching_entries_buffer;
  uint32_t matching_entries_buffer_size;
  uint32_t num_matching_entries;
  uint32_t table_handle;
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 200 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h" 2
;





typedef union cvd_cal_object_t
{
  cvd_cal_object_header_t header;
  cvd_cal_table_object_t table;
  cvd_cal_query_object_t query;
}
  cvd_cal_object_t;

typedef struct cvd_cal_lut_work_item_t cvd_cal_lut_work_item_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 217 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h" 2

struct cvd_cal_lut_work_item_t{
  apr_list_node_t link;
  uint32_t lut_entry_index;
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 223 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h" 2
;
# 235 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h"
extern int32_t cvd_cal_init (
  void
);







extern int32_t cvd_cal_deinit (
  void
);
# 260 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h"
extern int32_t cvd_cal_eval_indexing_mem_size (
  cvd_cal_table_descriptor_t* cal_table_descriptor,
  uint32_t* ret_size
);
# 277 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h"
extern int32_t cvd_cal_parse_table (
  void* indexing_mem_ptr,
  uint32_t indexing_mem_size,
  cvd_cal_table_descriptor_t* table_descriptor,
  cvd_cal_table_handle_t** ret_table_handle
);
# 299 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h"
extern int32_t cvd_cal_query_table (
  cvd_cal_table_handle_t* table_handle,
  cvd_cal_key_t* key,
  uint32_t entries_buf_size,
  cvd_cal_entry_t* ret_entries,
  uint32_t* ret_num_entries
);
# 316 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h"
extern int32_t cvd_cal_discard_table (
  cvd_cal_table_handle_t* table_hanlde
);
# 331 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h"
extern int32_t cvd_cal_find_min_max_column_value (
  cvd_cal_table_handle_t* table_handle,
  uint32_t column_id,
  uint32_t* ret_min_value,
  uint32_t* ret_max_value
);
# 349 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h"
extern int32_t cvd_cal_find_min_max_column_value_v2 (
  uint32_t table_handle,
  uint32_t column_id,
  uint32_t* ret_min_value,
  uint32_t* ret_max_value
);
# 367 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h"
extern int32_t cvd_cal_find_vol_cal_format (
  uint32_t table_handle,
  bool_t* ret_is_v1_vol_format
);
# 380 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h"
extern int32_t cvd_cal_parse_table_v2 (
  cvd_cal_table_descriptor_t* table_descriptor,
  uint32_t* ret_table_handle
);
# 392 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h"
extern int32_t cvd_cal_discard_table_v2 (
  uint32_t table_handle
);
# 412 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h"
extern int32_t cvd_cal_query_init (
  uint32_t table_handle,
  cvd_cal_key_t* key,
  void* matched_entry_buffer,
  uint32_t matched_entry_buffer_size,
  uint32_t* ret_cal_query_handle
);
# 431 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h"
extern int32_t cvd_cal_query (
  uint32_t cal_query_handle,
  cvd_cal_query_cb_fn_t cb_fn,
  const void* client_data
);
# 446 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h"
extern int32_t cvd_cal_query_deinit (
  uint32_t cal_query_handle
);
# 459 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h"
extern int32_t cvd_cal_utility_remove_na_matching_entries(
  uint32_t query_handle,
  uint32_t column_id
);
# 471 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_cal_private_i.h"
extern uint32_t cvd_cal_obj_stat (
  uint32_t *objects_used
);
# 30 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_devcfg_i.h" 1
# 17 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_devcfg_i.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 1
# 28 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/cvd_adsp_version.h" 1
# 29 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h" 1
# 507 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h"
typedef union vss_param_cal_na_value_t vss_param_cal_na_value_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 510 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h" 2





union vss_param_cal_na_value_t
{
  uint8_t uint8_val;

  uint16_t uint16_val;

  uint32_t uint32_val;

  uint64_t uint64_val;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 527 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h" 2
;



typedef struct vss_param_cal_column_t vss_param_cal_column_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 534 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h" 2



struct vss_param_cal_column_t
{
  uint32_t id;




  uint32_t type;





  vss_param_cal_na_value_t na_value;





}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 558 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h" 2
;



typedef struct vss_param_hdvoice_config_data_t vss_param_hdvoice_config_data_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 565 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h" 2



struct vss_param_hdvoice_config_data_t
{
  uint32_t feature_id;




  uint32_t enable_mode;
# 585 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h"
  uint32_t rx_sampling_rate;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 591 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h" 2
;



typedef struct vss_param_custom_bwe_config_data_t vss_param_custom_bwe_config_data_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 598 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h" 2



struct vss_param_custom_bwe_config_data_t
{
  uint32_t feature_id;






  uint32_t enable_mode;
# 619 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h"
  uint32_t rx_sampling_rate;




  uint32_t tx_sampling_rate;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 630 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h" 2
;
# 686 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h"
typedef struct vss_param_clock_control_params_t vss_param_clock_control_params_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 689 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h" 2






struct vss_param_clock_control_params_t
{
   uint32_t direction;







  uint32_t sampling_rate;
# 715 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h"
  uint32_t network_id;







  uint32_t media_type_id;







  uint32_t mpps_scale_factor;







  uint32_t bus_bw_scale_factor;






}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 748 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h" 2
;
# 788 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h"
typedef struct vss_param_port_endpoint_media_info_t vss_param_port_endpoint_media_info_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 791 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h" 2





struct vss_param_port_endpoint_media_info_t
{
   uint32_t port_id;







   uint16_t num_channels;




   uint16_t bits_per_sample;





   uint32_t sampling_rate;





   uint8_t channel_mapping[8];







}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 833 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h" 2
;
# 873 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h"
typedef struct vss_param_vocproc_dev_channel_info_t vss_param_vocproc_dev_channel_info_t ;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 876 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h" 2





struct vss_param_vocproc_dev_channel_info_t
{
   uint32_t num_channels;
# 896 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h"
   uint32_t bits_per_sample;




   uint8_t channel_mapping[ ( ( 8 ) ) ];




}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 908 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_public_if.h" 2
;
# 31 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iavsync_public_if.h" 1
# 78 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iavsync_public_if.h"
typedef struct vss_iavsync_evt_rx_path_delay_t vss_iavsync_evt_rx_path_delay_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 81 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iavsync_public_if.h" 2





struct vss_iavsync_evt_rx_path_delay_t
{
   uint32_t delay_us;
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 91 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iavsync_public_if.h" 2
;
# 129 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iavsync_public_if.h"
typedef struct vss_iavsync_evt_tx_timestamp_t vss_iavsync_evt_tx_timestamp_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 132 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iavsync_public_if.h" 2





struct vss_iavsync_evt_tx_timestamp_t
{
   uint64_t timestamp_us;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 143 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iavsync_public_if.h" 2
;
# 172 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iavsync_public_if.h"
typedef struct vss_iavsync_evt_path_delay_t vss_iavsync_evt_path_delay_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 175 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iavsync_public_if.h" 2





struct vss_iavsync_evt_path_delay_t
{
   uint32_t tx_path_delay_us;
   uint32_t rx_path_delay_us;
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 186 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iavsync_public_if.h" 2
;
# 32 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iavtimer_public_if.h" 1
# 87 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iavtimer_public_if.h"
typedef struct vss_iavtimer_rsp_get_time_t vss_iavtimer_rsp_get_time_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 90 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iavtimer_public_if.h" 2






struct vss_iavtimer_rsp_get_time_t
{
  uint64_t timestamp_us;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 102 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iavtimer_public_if.h" 2
;
# 144 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iavtimer_public_if.h"
typedef struct vss_iavtimer_rsp_get_offset_t vss_iavtimer_rsp_get_offset_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 147 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iavtimer_public_if.h" 2






struct vss_iavtimer_rsp_get_offset_t
{
  int64_t offset_us;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 159 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iavtimer_public_if.h" 2
;
# 33 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ihdvoice_public_if.h" 1
# 34 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h" 1
# 119 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h"
typedef struct vss_imvm_cmd_set_policy_dual_control_t vss_imvm_cmd_set_policy_dual_control_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 122 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h" 2





struct vss_imvm_cmd_set_policy_dual_control_t
{
  bool_t enable_flag;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 133 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h" 2
;
# 152 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h"
typedef struct vss_imvm_cmd_attach_stream_t vss_imvm_cmd_attach_stream_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 155 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h" 2





struct vss_imvm_cmd_attach_stream_t
{
  uint16_t handle;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 166 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h" 2
;
# 185 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h"
typedef struct vss_imvm_cmd_detach_stream_t vss_imvm_cmd_detach_stream_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 188 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h" 2





struct vss_imvm_cmd_detach_stream_t
{
  uint16_t handle;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 199 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h" 2
;
# 218 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h"
typedef struct vss_imvm_cmd_attach_vocproc_t vss_imvm_cmd_attach_vocproc_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 221 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h" 2





struct vss_imvm_cmd_attach_vocproc_t
{
  uint16_t handle;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 232 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h" 2
;
# 251 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h"
typedef struct vss_imvm_cmd_detach_vocproc_t vss_imvm_cmd_detach_vocproc_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 254 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h" 2





struct vss_imvm_cmd_detach_vocproc_t
{
  uint16_t handle;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 265 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h" 2
;
# 405 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h"
typedef struct vss_imvm_cmd_set_cal_network_t vss_imvm_cmd_set_cal_network_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 408 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h" 2





struct vss_imvm_cmd_set_cal_network_t
{
  uint32_t network_id;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 421 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h" 2
;
# 460 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h"
typedef struct vss_imvm_cmd_set_cal_media_type_t vss_imvm_cmd_set_cal_media_type_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 463 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h" 2





struct vss_imvm_cmd_set_cal_media_type_t
{
  uint32_t media_id;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 476 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h" 2
;
# 518 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h"
typedef struct vss_imvm_cmd_set_max_var_voc_sampling_rate_t vss_imvm_cmd_set_max_var_voc_sampling_rate_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 521 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h" 2





struct vss_imvm_cmd_set_max_var_voc_sampling_rate_t
{
  uint32_t rx;
# 538 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h"
  uint32_t tx;
# 547 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 549 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_public_if.h" 2
;
# 36 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_inotify_public_if.h" 1
# 63 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_inotify_public_if.h"
typedef struct vss_inotify_cmd_listen_for_event_class_t vss_inotify_cmd_listen_for_event_class_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 66 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_inotify_public_if.h" 2





struct vss_inotify_cmd_listen_for_event_class_t
{
   uint32_t class_id;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 77 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_inotify_public_if.h" 2
;
# 101 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_inotify_public_if.h"
typedef struct vss_inotify_cmd_cancel_event_class_t vss_inotify_cmd_cancel_event_class_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 104 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_inotify_public_if.h" 2





struct vss_inotify_cmd_cancel_event_class_t
{
   uint32_t class_id;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 115 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_inotify_public_if.h" 2
;
# 37 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 1
# 82 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h"
typedef struct vss_ipktexg_cmd_set_mode_t vss_ipktexg_cmd_set_mode_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 85 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2





struct vss_ipktexg_cmd_set_mode_t
{
   uint32_t mode;






}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 101 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2
;
# 124 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h"
typedef struct vss_ipktexg_evt_in_band_send_enc_buffer_t vss_ipktexg_evt_in_band_send_enc_buffer_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 127 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2






struct vss_ipktexg_evt_in_band_send_enc_buffer_t
{
   uint32_t media_id;





   uint8_t content[];

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 145 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2
;
# 182 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h"
typedef struct vss_ipktexg_evt_in_band_send_dec_buffer_t vss_ipktexg_evt_in_band_send_dec_buffer_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 185 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2






struct vss_ipktexg_evt_in_band_send_dec_buffer_t
{
   uint32_t media_id;





   uint8_t content[];

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 203 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2
;
# 244 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h"
typedef struct vss_ipktexg_cmd_oob_set_config_t vss_ipktexg_cmd_oob_set_config_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 247 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2





struct vss_ipktexg_cmd_oob_set_config_t
{
  uint32_t mem_handle;



  uint64_t enc_buf_addr;


  uint32_t enc_buf_size;





  uint64_t dec_buf_addr;


  uint32_t dec_buf_size;




}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 277 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2
;
# 569 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h"
typedef struct vss_ipktexg_mailbox_enc_packet_t vss_ipktexg_mailbox_enc_packet_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 572 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2





struct vss_ipktexg_mailbox_enc_packet_t
{
  uint16_t minor_version;




  uint64_t capture_timestamp_us;





  uint32_t status_mask;







  uint32_t media_id;




  uint32_t data_size;



  uint8_t data[];



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 613 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2
;



typedef struct vss_ipktexg_mailbox_dec_packet_t vss_ipktexg_mailbox_dec_packet_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 620 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2





struct vss_ipktexg_mailbox_dec_packet_t
{
  uint16_t minor_version;




  uint64_t render_timestamp_us;


  uint32_t status_mask;







  uint32_t media_id;




  uint32_t data_size;



  uint8_t data[];



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 658 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2
;


typedef struct vss_ipktexg_mailbox_enc_request_t vss_ipktexg_mailbox_enc_request_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 664 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2





struct vss_ipktexg_mailbox_enc_request_t
{
  uint64_t timestamp_us;






  uint64_t mem_address;



  uint32_t mem_size;
# 696 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 698 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2
;



typedef struct vss_ipktexg_mailbox_dec_request_t vss_ipktexg_mailbox_dec_request_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 705 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2





struct vss_ipktexg_mailbox_dec_request_t
{
  uint64_t timestamp_us;






  uint64_t mem_address;



  uint32_t mem_size;






}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 732 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2
;



typedef struct vss_ipktexg_mailbox_voc_req_circ_buffer_t vss_ipktexg_mailbox_voc_req_circ_buffer_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 739 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2






struct vss_ipktexg_mailbox_voc_req_circ_buffer_t
{
  uint64_t read_offset;



  uint8_t padding_1[ 120 ];



  uint64_t write_offset;



  uint8_t padding_2[ 120 ];




  uint8_t data[];

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 768 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2
;




typedef struct vss_ipktexg_cmd_mailbox_set_config_t vss_ipktexg_cmd_mailbox_set_config_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 776 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2






struct vss_ipktexg_cmd_mailbox_set_config_t
{
  uint32_t mem_handle;



  uint64_t tx_circ_buf_mem_address;





  uint32_t tx_circ_buf_mem_size;
# 808 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h"
  uint64_t rx_circ_buf_mem_address;





  uint32_t rx_circ_buf_mem_size;
# 827 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 829 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2
;
# 869 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h"
typedef struct vss_ipktexg_rsp_mailbox_get_voc_packet_properties_t vss_ipktexg_rsp_mailbox_get_voc_packet_properties_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 872 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2






struct vss_ipktexg_rsp_mailbox_get_voc_packet_properties_t
{
  uint32_t tx_duration_us;





  uint32_t tx_max_size;





  uint32_t rx_duration_us;





  uint32_t rx_max_size;




}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 905 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ipktexg_public_if.h" 2
;
# 38 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iplayback_public_if.h" 1
# 81 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iplayback_public_if.h"
typedef struct vss_iplayback_cmd_start_t vss_iplayback_cmd_start_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 84 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iplayback_public_if.h" 2





struct vss_iplayback_cmd_start_t
{
  uint16_t port_id;
# 100 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iplayback_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 102 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iplayback_public_if.h" 2
;
# 39 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_irecord_public_if.h" 1
# 91 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_irecord_public_if.h"
typedef struct vss_irecord_cmd_start_t vss_irecord_cmd_start_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 94 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_irecord_public_if.h" 2





struct vss_irecord_cmd_start_t
{
  uint32_t rx_tap_point;






  uint32_t tx_tap_point;






  uint16_t port_id;
# 128 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_irecord_public_if.h"
  uint32_t mode;
# 137 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_irecord_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 139 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_irecord_public_if.h" 2
;
# 40 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_itty_public_if.h" 1
# 60 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_itty_public_if.h"
typedef struct vss_itty_cmd_set_tty_mode_t vss_itty_cmd_set_tty_mode_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 63 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_itty_public_if.h" 2





struct vss_itty_cmd_set_tty_mode_t
{
  uint32_t mode;







}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 80 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_itty_public_if.h" 2
;
# 42 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ittyoob_public_if.h" 1
# 87 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ittyoob_public_if.h"
typedef struct vss_ittyoob_evt_tx_char_t vss_ittyoob_evt_tx_char_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 90 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ittyoob_public_if.h" 2





struct vss_ittyoob_evt_tx_char_t
{
  uint16_t tty_char;


}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 102 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ittyoob_public_if.h" 2
;
# 122 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ittyoob_public_if.h"
typedef struct vss_ittyoob_cmd_rx_char_t vss_ittyoob_cmd_rx_char_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 125 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ittyoob_public_if.h" 2





struct vss_ittyoob_cmd_rx_char_t
{
  uint16_t tty_char;


}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 137 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ittyoob_public_if.h" 2
;
# 43 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iversion_public_if.h" 1
# 80 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iversion_public_if.h"
typedef struct vss_iversion_rsp_get_t vss_iversion_rsp_get_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 83 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iversion_public_if.h" 2





struct vss_iversion_rsp_get_t
{
  char_t version[31];

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 94 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iversion_public_if.h" 2
;
# 44 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivolume_public_if.h" 1
# 88 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivolume_public_if.h"
typedef struct vss_ivolume_cmd_mute_v2_t vss_ivolume_cmd_mute_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 91 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivolume_public_if.h" 2





struct vss_ivolume_cmd_mute_v2_t
{
  uint16_t direction;
# 108 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivolume_public_if.h"
  uint16_t mute_flag;






  uint16_t ramp_duration_ms;






}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 124 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivolume_public_if.h" 2
;
# 149 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivolume_public_if.h"
typedef struct vss_ivolume_cmd_set_number_of_steps_t vss_ivolume_cmd_set_number_of_steps_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 152 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivolume_public_if.h" 2





struct vss_ivolume_cmd_set_number_of_steps_t
{
  uint32_t value;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 163 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivolume_public_if.h" 2
;
# 219 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivolume_public_if.h"
typedef struct vss_ivolume_cmd_set_step_t vss_ivolume_cmd_set_step_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 222 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivolume_public_if.h" 2





struct vss_ivolume_cmd_set_step_t
{
  uint16_t direction;




  uint32_t value;



  uint16_t ramp_duration_ms;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 244 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivolume_public_if.h" 2
;
# 46 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivp3_public_if.h" 1
# 82 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivp3_public_if.h"
typedef struct vss_ivp3_rsp_get_size_t vss_ivp3_rsp_get_size_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 85 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivp3_public_if.h" 2





struct vss_ivp3_rsp_get_size_t
{
  uint32_t size;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 96 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivp3_public_if.h" 2
;
# 125 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivp3_public_if.h"
typedef struct vss_ivp3_cmd_get_data_t vss_ivp3_cmd_get_data_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 128 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivp3_public_if.h" 2





struct vss_ivp3_cmd_get_data_t
{
  uint32_t mem_handle;


  uint64_t mem_address;


  uint32_t mem_size;
# 150 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivp3_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 152 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivp3_public_if.h" 2
;
# 180 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivp3_public_if.h"
typedef struct vss_ivp3_cmd_set_data_t vss_ivp3_cmd_set_data_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 183 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivp3_public_if.h" 2





struct vss_ivp3_cmd_set_data_t
{
  uint32_t mem_handle;


  uint64_t mem_address;


  uint32_t mem_size;






}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 205 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivp3_public_if.h" 2
;
# 47 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivpcm_public_if.h" 1
# 112 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivpcm_public_if.h"
typedef struct vss_ivpcm_tap_point_t vss_ivpcm_tap_point_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 115 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivpcm_public_if.h" 2





struct vss_ivpcm_tap_point_t
{
  uint32_t tap_point;






  uint16_t direction;







  uint16_t sampling_rate;
# 146 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivpcm_public_if.h"
  uint16_t duration;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 152 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivpcm_public_if.h" 2
;



typedef struct vss_ivpcm_cmd_start_v2_t vss_ivpcm_cmd_start_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 159 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivpcm_public_if.h" 2





struct vss_ivpcm_cmd_start_v2_t
{
  uint32_t mem_handle;



  uint32_t num_tap_points;



  vss_ivpcm_tap_point_t tap_points[];


}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 179 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivpcm_public_if.h" 2
;
# 231 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivpcm_public_if.h"
typedef struct vss_ivpcm_evt_push_buffer_v2_t vss_ivpcm_evt_push_buffer_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 234 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivpcm_public_if.h" 2





struct vss_ivpcm_evt_push_buffer_v2_t
{
  uint32_t tap_point;






  uint32_t push_buf_mask;
# 257 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivpcm_public_if.h"
  uint64_t out_buf_mem_address;



  uint16_t out_buf_mem_size;
# 272 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivpcm_public_if.h"
  uint64_t in_buf_mem_address;



  uint16_t in_buf_mem_size;
# 285 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivpcm_public_if.h"
  uint16_t sampling_rate;







  uint16_t num_in_channels;




}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 300 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivpcm_public_if.h" 2
;
# 416 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivpcm_public_if.h"
typedef struct vss_ivpcm_evt_notify_v2_t vss_ivpcm_evt_notify_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 419 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivpcm_public_if.h" 2




struct vss_ivpcm_evt_notify_v2_t
{
  uint32_t tap_point;



  uint32_t notify_mask;
# 449 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivpcm_public_if.h"
  uint64_t out_buf_mem_address;



  uint64_t in_buf_mem_address;



  uint16_t filled_out_size;




  uint16_t request_buf_size;


  uint16_t sampling_rate;






  uint16_t num_out_channels;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 476 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivpcm_public_if.h" 2
;
# 48 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_isoundfocus_public_if.h" 1
# 66 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_isoundfocus_public_if.h"
typedef struct vss_isoundfocus_cmd_set_sectors_t vss_isoundfocus_cmd_set_sectors_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 69 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_isoundfocus_public_if.h" 2





struct vss_isoundfocus_cmd_set_sectors_t
{
  uint16_t start_angles[ 8 ];
# 96 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_isoundfocus_public_if.h"
  uint8_t enables[ 8 ];
# 109 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_isoundfocus_public_if.h"
  uint16_t gain_step;
# 123 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_isoundfocus_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 125 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_isoundfocus_public_if.h" 2
;
# 169 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_isoundfocus_public_if.h"
typedef struct vss_isoundfocus_rsp_get_sectors_t vss_isoundfocus_rsp_get_sectors_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 172 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_isoundfocus_public_if.h" 2





struct vss_isoundfocus_rsp_get_sectors_t
{
  uint16_t start_angles[ 8 ];
# 198 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_isoundfocus_public_if.h"
  uint8_t enables[ 8 ];







  uint16_t gain_step;







}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 216 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_isoundfocus_public_if.h" 2
;
# 49 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_isourcetrack_public_if.h" 1
# 80 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_isourcetrack_public_if.h"
typedef struct vss_isourcetrack_cmd_get_activity_t vss_isourcetrack_cmd_get_activity_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 83 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_isourcetrack_public_if.h" 2





struct vss_isourcetrack_cmd_get_activity_t
{
  uint32_t mem_handle;



  uint64_t mem_address;


  uint32_t mem_size;
# 107 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_isourcetrack_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 109 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_isourcetrack_public_if.h" 2
;



typedef struct vss_isourcetrack_activity_data_t vss_isourcetrack_activity_data_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 116 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_isourcetrack_public_if.h" 2





struct vss_isourcetrack_activity_data_t
{
  uint8_t voice_active[ 8 ];
# 132 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_isourcetrack_public_if.h"
  uint16_t talker_doa;






  uint16_t interferer_doa[ 3 ];







  uint8_t sound_strength[ 360 ];



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 153 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_isourcetrack_public_if.h" 2
;
# 50 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2






# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_deprecated_public_if.h" 1
# 230 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_deprecated_public_if.h"
typedef struct vss_icommon_cmd_set_network_t vss_icommon_cmd_set_network_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 233 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_deprecated_public_if.h" 2





struct vss_icommon_cmd_set_network_t
{
  uint32_t network_id;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 246 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_deprecated_public_if.h" 2
;
# 270 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_deprecated_public_if.h"
typedef struct vss_icommon_cmd_set_voice_timing_t vss_icommon_cmd_set_voice_timing_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 273 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_deprecated_public_if.h" 2





struct vss_icommon_cmd_set_voice_timing_t
{
  uint16_t mode;






  uint16_t enc_offset;





  uint16_t dec_req_offset;





  uint16_t dec_offset;







}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 309 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_common_deprecated_public_if.h" 2
;
# 57 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_deprecated_public_if.h" 1
# 77 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_deprecated_public_if.h"
typedef struct vss_param_num_dev_channels_t vss_param_num_dev_channels_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 80 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_deprecated_public_if.h" 2





struct vss_param_num_dev_channels_t
{
   uint16_t tx_num_channels;
# 100 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_deprecated_public_if.h"
   uint16_t rx_num_channels;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 106 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_params_deprecated_public_if.h" 2
;
# 58 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_deprecated_public_if.h" 1
# 65 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_deprecated_public_if.h"
typedef struct vss_imvm_cmd_set_modem_voice_control_t vss_imvm_cmd_set_modem_voice_control_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 68 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_deprecated_public_if.h" 2





struct vss_imvm_cmd_set_modem_voice_control_t
{
  bool_t enable_flag;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 79 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_imvm_deprecated_public_if.h" 2
;
# 59 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h" 1
# 134 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h"
typedef struct vss_istream_cmd_register_calibration_data_v2_t vss_istream_cmd_register_calibration_data_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 137 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h" 2






struct vss_istream_cmd_register_calibration_data_v2_t
{
  uint32_t cal_mem_handle;



  uint64_t cal_mem_address;


  uint32_t cal_mem_size;







  uint32_t num_columns;




  vss_istream_cal_column_t columns[];




}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 172 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h" 2
;
# 228 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h"
typedef struct vss_istream_cmd_start_record_t vss_istream_cmd_start_record_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 231 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h" 2





struct vss_istream_cmd_start_record_t
{
   uint32_t rx_tap_point;






   uint32_t tx_tap_point;





   }

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 253 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h" 2
;
# 354 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h"
typedef struct vss_istream_cmd_set_packet_exchange_mode_t vss_istream_cmd_set_packet_exchange_mode_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 357 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h" 2






struct vss_istream_cmd_set_packet_exchange_mode_t
{
   uint32_t mode;





}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 373 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h" 2
;
# 397 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h"
typedef struct vss_istream_evt_send_enc_buffer_t vss_istream_evt_send_enc_buffer_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 400 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h" 2





struct vss_istream_evt_send_enc_buffer_t
{
   uint32_t media_id;



   uint8_t content[];


}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 416 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h" 2
;
# 452 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h"
typedef struct vss_istream_evt_send_dec_buffer_t vss_istream_evt_send_dec_buffer_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 455 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h" 2





struct vss_istream_evt_send_dec_buffer_t
{
   uint32_t media_id;



   uint8_t content[];


}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 471 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h" 2
;
# 515 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h"
typedef struct vss_istream_cmd_set_oob_packet_exchange_config_t vss_istream_cmd_set_oob_packet_exchange_config_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 518 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h" 2






struct vss_istream_cmd_set_oob_packet_exchange_config_t
{
  uint32_t mem_handle;



  uint64_t enc_buf_addr;


  uint32_t enc_buf_size;





  uint64_t dec_buf_addr;


  uint32_t dec_buf_size;




}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 549 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h" 2
;
# 730 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h"
typedef struct vss_istream_cmd_set_var_voc_sampling_rate_t vss_istream_cmd_set_var_voc_sampling_rate_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 733 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h" 2





struct vss_istream_cmd_set_var_voc_sampling_rate_t
{
  uint32_t rx;




  uint32_t tx;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 751 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h" 2
;
# 849 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h"
typedef struct vss_ipktexg_cmd_mailbox_start_t vss_ipktexg_cmd_mailbox_start_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 852 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h" 2





struct vss_ipktexg_cmd_mailbox_start_t
{
  uint64_t tx_ref_timstamp_us;
# 873 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h"
  uint64_t rx_ref_timstamp_us;
# 886 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 888 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_istream_deprecated_public_if.h" 2
;
# 60 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 1
# 76 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
typedef struct vss_ivocproc_cmd_create_full_control_session_t vss_ivocproc_cmd_create_full_control_session_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 79 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2






struct vss_ivocproc_cmd_create_full_control_session_t
{
  uint16_t direction;







  uint32_t tx_port_id;







  uint32_t tx_topology_id;
# 112 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
  uint32_t rx_port_id;







  uint32_t rx_topology_id;
# 129 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
  uint32_t network_id;
# 138 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
  char_t session_name[];






}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 147 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2
;
# 168 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
typedef struct vss_ivocproc_cmd_set_device_t vss_ivocproc_cmd_set_device_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 171 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2





struct vss_ivocproc_cmd_set_device_t
{
  uint32_t tx_port_id;







  uint32_t tx_topology_id;
# 199 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
  uint32_t rx_port_id;
# 208 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
  uint32_t rx_topology_id;
# 220 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 222 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2
;
# 241 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
typedef struct vss_ivocproc_cmd_create_full_control_session_v2_t vss_ivocproc_cmd_create_full_control_session_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 244 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2





struct vss_ivocproc_cmd_create_full_control_session_v2_t
{
  uint16_t direction;







  uint16_t tx_port_id;







  uint32_t tx_topology_id;
# 276 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
  uint16_t rx_port_id;







  uint32_t rx_topology_id;
# 293 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
  uint32_t profile_id;


  uint32_t vocproc_mode;







  uint16_t ec_ref_port_id;
# 317 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
  char_t session_name[];






}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 326 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2
;
# 350 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
typedef struct vss_ivocproc_cmd_set_device_v2_t vss_ivocproc_cmd_set_device_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 353 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2





struct vss_ivocproc_cmd_set_device_v2_t
{
  uint16_t tx_port_id;







  uint32_t tx_topology_id;
# 377 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
  uint16_t rx_port_id;







  uint32_t rx_topology_id;
# 394 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
  uint32_t vocproc_mode;







  uint16_t ec_ref_port_id;
# 413 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 415 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2
;
# 514 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
typedef struct vss_ivocproc_cmd_register_calibration_data_v2_t vss_ivocproc_cmd_register_calibration_data_v2_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 517 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2






struct vss_ivocproc_cmd_register_calibration_data_v2_t
{
  uint32_t cal_mem_handle;



  uint64_t cal_mem_address;


  uint32_t cal_mem_size;







  uint32_t num_columns;




  vss_ivocproc_cal_column_t columns[];




}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 552 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2
;
# 660 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
typedef struct vss_ivocproc_cmd_register_volume_calibration_data_t vss_ivocproc_cmd_register_volume_calibration_data_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 663 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2






struct vss_ivocproc_cmd_register_volume_calibration_data_t
{
  uint32_t cal_mem_handle;



  uint64_t cal_mem_address;


  uint32_t cal_mem_size;







  uint32_t num_columns;




  vss_ivocproc_cal_column_t columns[];




}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 698 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2
;
# 742 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
typedef struct vss_ivocproc_cmd_set_rx_volume_index_t vss_ivocproc_cmd_set_rx_volume_index_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 745 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2






struct vss_ivocproc_cmd_set_rx_volume_index_t
{
  uint16_t vol_index;


}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 758 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2
;
# 778 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
typedef struct vss_ivocproc_cmd_set_mute_t vss_ivocproc_cmd_set_mute_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 781 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2





struct vss_ivocproc_cmd_set_mute_t
{
  uint16_t direction;







  uint16_t mute_flag;





}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 804 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2
;
# 875 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
typedef struct vss_ivocproc_cmd_topology_register_params_t vss_ivocproc_cmd_topology_register_params_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 878 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2





struct vss_ivocproc_cmd_topology_register_params_t
{
  uint32_t mem_handle;



  uint64_t mem_address;


  uint32_t mem_size;




}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 899 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2
;
# 947 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
typedef struct vss_ivocproc_cmd_set_sampling_rate_t vss_ivocproc_cmd_set_sampling_rate_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 950 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2




struct vss_ivocproc_cmd_set_sampling_rate_t
{
  uint32_t rx;




  uint32_t tx;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 967 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2
;
# 995 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
typedef struct vss_ivocproc_cmd_topology_set_dev_channels_t vss_ivocproc_cmd_topology_set_dev_channels_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 998 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2





struct vss_ivocproc_cmd_topology_set_dev_channels_t
{
  uint16_t tx_num_channels;
# 1014 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h"
  uint16_t rx_num_channels;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 1020 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_ivocproc_deprecated_public_if.h" 2
;
# 61 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iwidevoice_deprecated_public_if.h" 1
# 63 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iwidevoice_deprecated_public_if.h"
typedef struct vss_iwidevoice_cmd_set_widevoice_t vss_iwidevoice_cmd_set_widevoice_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 66 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iwidevoice_deprecated_public_if.h" 2





struct vss_iwidevoice_cmd_set_widevoice_t
{
  uint32_t enable;





}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 81 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_iwidevoice_deprecated_public_if.h" 2
;
# 62 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2






# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_deleted_public_if.h" 1
# 69 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/api/cvd/vss_public_if.h" 2
# 18 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_devcfg_i.h" 2
# 102 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_devcfg_i.h"
typedef struct cvd_devcfg_voice_use_case_t cvd_devcfg_supported_voice_use_case_t;






typedef struct cvd_devcfg_voice_use_case_t cvd_devcfg_voice_use_case_na_values_t;

struct cvd_devcfg_voice_use_case_t
{
  uint16_t max_num_voice_sessions;

  uint16_t max_num_nb_streams;

  uint16_t max_num_wb_streams;

  uint16_t max_num_swb_streams;

  uint16_t max_num_fb_plus_streams;

  uint16_t max_num_nb_vocprocs;

  uint16_t max_num_wb_vocprocs;

  uint16_t max_num_swb_vocprocs;

  uint16_t max_num_fb_plus_vocprocs;


  uint32_t tx_topology_id;



  uint32_t rx_topology_id;



  uint32_t media_id;



  uint16_t vfr_mode;






};

typedef struct cvd_devcfg_clock_level_t cvd_devcfg_clock_level_t;

struct cvd_devcfg_clock_level_t
{
  uint32_t core_floor_clock_hz;

  uint16_t num_supported_use_cases;

  cvd_devcfg_supported_voice_use_case_t* supported_use_cases;


};

typedef struct cvd_devcfg_clock_frequency_level_t cvd_devcfg_clock_frequency_level_t;

struct cvd_devcfg_clock_frequency_level_t
{
  uint32_t clock_freq_hz;

};

typedef struct cvd_devcfg_clock_table_t cvd_devcfg_clock_table_t;

struct cvd_devcfg_clock_table_t
{
  uint16_t num_clock_levels;

  cvd_devcfg_clock_level_t* clock_levels;



};

typedef struct cvd_devcfg_clock_frequency_level_table_t cvd_devcfg_clock_frequency_level_table_t;

struct cvd_devcfg_clock_frequency_level_table_t
{
  uint32_t num_clock_frequency_levels;

  cvd_devcfg_clock_frequency_level_t* clock_frequency_levels;

};

typedef struct cvd_devcfg_preemption_safety_margins_values_t cvd_devcfg_preemption_safety_margins_values_t;

struct cvd_devcfg_preemption_safety_margins_values_t
{
 uint32_t vptx_preemption_safety_margin;
 uint32_t vprx_preemption_safety_margin;
 uint32_t venc_preemption_safety_margin;
 uint32_t vdec_preemption_safety_margin;
};

typedef struct cvd_devcfg_preemption_safety_margins_level_t cvd_devcfg_preemption_safety_margins_level_t;

struct cvd_devcfg_preemption_safety_margins_level_t
{
  cvd_devcfg_preemption_safety_margins_values_t preemption_safety_margins_values;

  uint16_t num_supported_use_cases;

  cvd_devcfg_supported_voice_use_case_t* supported_use_cases;


};

typedef struct cvd_devcfg_preemption_safety_margins_table_t cvd_devcfg_preemption_safety_margins_table_t;

struct cvd_devcfg_preemption_safety_margins_table_t
{
  uint16_t num_preemption_margin_levels;

  cvd_devcfg_preemption_safety_margins_level_t* preemption_margin_levels;

};



typedef struct cvd_devcfg_mmpm_core_info_t cvd_devcfg_mmpm_core_info_t;

struct cvd_devcfg_mmpm_core_info_t
{
  MmpmCoreIdType core_id;

  MmpmCoreInstanceIdType instance_id;

};

typedef struct cvd_devcfg_mmpm_bw_value_t cvd_devcfg_mmpm_bw_value_t;

struct cvd_devcfg_mmpm_bw_value_t
{
  MmpmGenBwValType bw_val;


  uint16_t num_supported_use_cases;

  cvd_devcfg_supported_voice_use_case_t* supported_use_cases;


};

typedef struct cvd_devcfg_mmpm_bw_table_t cvd_devcfg_mmpm_bw_table_t;

struct cvd_devcfg_mmpm_bw_table_t
{
  uint32_t num_bw_values;

  cvd_devcfg_mmpm_bw_value_t* bw_values;

};
# 31 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_devcfg_parser_i.h" 1
# 31 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_devcfg_parser_i.h"
typedef struct cvd_devcfg_parser_voice_use_case_t cvd_devcfg_parser_voice_use_case_t;

struct cvd_devcfg_parser_voice_use_case_t
{
  uint16_t num_voice_sessions;

  uint16_t num_nb_streams;

  uint16_t num_wb_streams;

  uint16_t num_swb_streams;

  uint16_t num_fb_plus_streams;

  uint16_t num_nb_vocprocs;

  uint16_t num_wb_vocprocs;

  uint16_t num_swb_vocprocs;

  uint16_t num_fb_plus_vocprocs;


  uint32_t tx_topology_id;



  uint32_t rx_topology_id;



  uint32_t media_id;



  uint16_t vfr_mode;






};



typedef struct cvd_devcfg_parser_mmpm_core_info_t cvd_devcfg_parser_mmpm_core_info_t;

struct cvd_devcfg_parser_mmpm_core_info_t
{
  MmpmCoreIdType core_id;

  MmpmCoreInstanceIdType instance_id;

};

typedef struct cvd_devcfg_parser_target_specific_config_t cvd_devcfg_parser_target_specific_config_t;

struct cvd_devcfg_parser_target_specific_config_t
{
  uint32_t core_floor_clock_hz;

  uint32_t cycles_per_thousand_instr_packets;

  MmpmGenBwReqType bw_requirement;

  uint32_t sleep_latency_us;

  uint32_t vptx_preemption_safety_margin;

  uint32_t vprx_preemption_safety_margin;

  uint32_t venc_preemption_safety_margin;

  uint32_t vdec_preemption_safety_margin;

};
# 121 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_devcfg_parser_i.h"
extern int32_t cvd_devcfg_parser_init ( void );






extern int32_t cvd_devcfg_parser_deinit ( void );
# 142 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_devcfg_parser_i.h"
extern int32_t cvd_devcfg_parser_get_clock_and_bus_preempt_margin_config (
  cvd_devcfg_parser_voice_use_case_t* use_case,
  cvd_devcfg_parser_target_specific_config_t* ret_clock_and_bus_preempt_margin_config
);
# 155 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_devcfg_parser_i.h"
extern int32_t cvd_devcfg_parser_get_mmpm_core_info (
  cvd_devcfg_parser_mmpm_core_info_t* ret_mmpm_core_info
);
# 168 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_devcfg_parser_i.h"
extern int32_t cvd_devcfg_parser_get_mmpm_cpp (
  uint32_t* ret_mmpm_cpp
);
# 181 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_devcfg_parser_i.h"
extern int32_t cvd_devcfg_parser_get_max_q6_core_clock(
  uint32_t* ret_max_q6_core_clock
);
# 193 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_devcfg_parser_i.h"
extern int32_t cvd_devcfg_parser_get_min_q6_core_clock(
 uint32_t* ret_min_q6_core_clock
);
# 205 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_devcfg_parser_i.h"
extern int32_t cvd_devcfg_parser_get_max_bw(
  uint64_t* ret_max_bw
);
# 217 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_devcfg_parser_i.h"
extern int32_t cvd_devcfg_parser_get_csm_sleep_threshold(
  uint32_t* ret_csm_sleep_threshold
);
# 229 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_devcfg_parser_i.h"
extern int32_t cvd_devcfg_parser_get_csm_safety_margin(
  uint32_t* ret_csm_safety_margin
);
# 242 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_devcfg_parser_i.h"
extern int32_t cvd_devcfg_parser_get_clk_perf_levels(
  uint32_t* ret_num_clk_perf_levels, uint32_t* ret_clk_perf_levels
);
# 32 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_itty_private_if.h" 1
# 33 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ipktexg_private_if.h" 1
# 52 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ipktexg_private_if.h"
typedef struct vss_ipktexg_rsp_get_mode_t vss_ipktexg_rsp_get_mode_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 55 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ipktexg_private_if.h" 2




struct vss_ipktexg_rsp_get_mode_t
{
  uint32_t mode;




}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 68 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ipktexg_private_if.h" 2
;
# 149 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ipktexg_private_if.h"
typedef struct vss_ipktexg_rsp_mailbox_get_time_reference_t vss_ipktexg_rsp_mailbox_get_time_reference_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 152 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ipktexg_private_if.h" 2




struct vss_ipktexg_rsp_mailbox_get_time_reference_t
{
  uint64_t tx_timstamp_us;







  uint64_t rx_timstamp_us;







  uint16_t enc_offset_margin_us;




  uint16_t dec_req_offset_margin_us;





  uint16_t dec_offset_margin_us;




}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 192 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ipktexg_private_if.h" 2
;
# 34 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivfr_private_if.h" 1
# 38 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivfr_private_if.h"
typedef struct vss_ivfr_cmd_open_t vss_ivfr_cmd_open_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 41 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivfr_private_if.h" 2



struct vss_ivfr_cmd_open_t
{
  uint16_t mode;


}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 51 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivfr_private_if.h" 2
;
# 70 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivfr_private_if.h"
typedef struct vss_ivfr_rsp_open_t vss_ivfr_rsp_open_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 73 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivfr_private_if.h" 2



struct vss_ivfr_rsp_open_t
{
  uint64_t ref_timestamp_us;


}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 83 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ivfr_private_if.h" 2
;
# 35 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ihdvoice_private_if.h" 1
# 53 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ihdvoice_private_if.h"
typedef struct vss_ihdvoice_cmd_get_config_t vss_ihdvoice_cmd_get_config_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 56 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ihdvoice_private_if.h" 2



struct vss_ihdvoice_cmd_get_config_t
{
  uint32_t network_id;



  uint32_t media_id;


  uint32_t rx_voc_op_mode;



  uint32_t tx_voc_op_mode;



  uint32_t rx_pp_sr;

  uint32_t tx_pp_sr;

  uint32_t feature_id;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 86 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ihdvoice_private_if.h" 2
;
# 105 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ihdvoice_private_if.h"
typedef struct vss_ihdvoice_rsp_get_config_t vss_ihdvoice_rsp_get_config_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 108 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ihdvoice_private_if.h" 2



struct vss_ihdvoice_rsp_get_config_t
{
  uint32_t feature_id;



  uint32_t enable_mode;





  uint32_t rx_pp_sr;

  uint32_t tx_pp_sr;


}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 130 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ihdvoice_private_if.h" 2
;
# 149 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ihdvoice_private_if.h"
typedef struct vss_ihdvoice_cmd_set_config_t vss_ihdvoice_cmd_set_config_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 152 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ihdvoice_private_if.h" 2



struct vss_ihdvoice_cmd_set_config_t
{
  uint32_t feature_id;



}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 163 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_ihdvoice_private_if.h" 2
;
# 36 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_params_private_if.h" 1
# 37 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_diag_i.h" 1
# 35 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_diag_i.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/osal/inc/apr_lock.h" 1
# 15 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/osal/inc/apr_lock.h"
typedef void* apr_lock_t;




extern int32_t apr_lock_create ( uint32_t lock_type, apr_lock_t* ret_lock );
extern int32_t apr_lock_destroy ( apr_lock_t lock );
extern int32_t apr_lock_enter ( apr_lock_t lock );
extern int32_t apr_lock_leave ( apr_lock_t lock );
# 36 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_diag_i.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_errcodes.h" 1
# 37 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_diag_i.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_ids_domains.h" 1
# 38 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_diag_i.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api_inline.h" 1
# 32 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api_inline.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api.h" 1
# 31 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_api.h" 1
# 68 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_api.h"
extern int32_t apr_init ( void );
# 103 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_api.h"
extern int32_t apr_deinit ( void );
# 148 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_api.h"
extern int32_t apr_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
);
# 220 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_api.h"
typedef struct apr_cmd_get_version_t apr_cmd_get_version_t;



struct apr_cmd_get_version_t
{
  char_t* buffer;
  uint32_t buf_size;
  uint32_t* ret_size;

};
# 32 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_packet.h" 1
# 174 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_packet.h"
typedef struct aprv2_packet_t aprv2_packet_t;
# 200 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_packet.h"
struct aprv2_packet_t
{
  uint32_t header;
# 237 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_packet.h"
  uint16_t src_addr;
# 261 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_packet.h"
  uint16_t src_port;
# 271 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_packet.h"
  uint16_t dst_addr;
# 295 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_packet.h"
  uint16_t dst_port;
# 306 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_packet.h"
  uint32_t token;



  uint32_t opcode;


};
# 33 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api.h" 2
# 86 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api.h"
typedef int32_t ( *aprv2_dispatch_fn_t ) ( aprv2_packet_t* packet,
                                           void* dispatch_data );
# 199 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api.h"
typedef struct aprv2_cmd_register_t aprv2_cmd_register_t;


struct aprv2_cmd_register_t
{
  uint32_t* ret_handle;


  uint16_t addr;


  aprv2_dispatch_fn_t dispatch_fn;


  void* dispatch_data;

};
# 348 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api.h"
typedef struct aprv2_cmd_register2_t aprv2_cmd_register2_t;


struct aprv2_cmd_register2_t
{
  uint32_t* ret_handle;


  char_t* name;



  uint32_t name_size;


  uint16_t addr;




  aprv2_dispatch_fn_t dispatch_fn;


  void* dispatch_data;


  uint16_t* ret_addr;

};
# 436 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api.h"
typedef struct aprv2_cmd_deregister_t aprv2_cmd_deregister_t;


struct aprv2_cmd_deregister_t
{
  uint32_t handle;
};
# 534 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api.h"
typedef struct aprv2_cmd_async_send_t aprv2_cmd_async_send_t;


struct aprv2_cmd_async_send_t
{
  uint32_t handle;
  aprv2_packet_t* packet;
};
# 581 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api.h"
typedef struct aprv2_cmd_alloc_t aprv2_cmd_alloc_t;



struct aprv2_cmd_alloc_t
{
  uint32_t handle;
  uint32_t alloc_type;


  uint32_t alloc_size;
  aprv2_packet_t** ret_packet;
};
# 622 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api.h"
typedef struct aprv2_cmd_free_t aprv2_cmd_free_t;



struct aprv2_cmd_free_t
{
  uint32_t handle;
  aprv2_packet_t* packet;
};
# 674 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api.h"
typedef struct aprv2_cmd_query_address_t aprv2_cmd_query_address_t;


struct aprv2_cmd_query_address_t
{
  uint32_t handle;
  uint16_t* ret_addr;
};
# 730 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api.h"
typedef struct aprv2_cmd_local_dns_lookup_t aprv2_cmd_local_dns_lookup_t;


struct aprv2_cmd_local_dns_lookup_t
{
  char_t* name;
  uint32_t name_size;

  uint16_t* ret_addr;
};
# 819 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api.h"
typedef struct aprv2_cmd_forward_t aprv2_cmd_forward_t;


struct aprv2_cmd_forward_t
{
  uint32_t handle;
  aprv2_packet_t* packet;
};
# 892 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api.h"
typedef struct aprv2_cmd_alloc_ext_t aprv2_cmd_alloc_ext_t;


struct aprv2_cmd_alloc_ext_t
{
  uint32_t handle;


  uint8_t msg_type;







  uint16_t src_addr;


  uint16_t src_port;


  uint16_t dst_addr;


  uint16_t dst_port;


  uint32_t token;




  uint32_t opcode;


  uint32_t payload_size;


  aprv2_packet_t** ret_packet;

};
# 1020 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api.h"
typedef struct aprv2_cmd_alloc_send_t aprv2_cmd_alloc_send_t;


struct aprv2_cmd_alloc_send_t
{
  uint32_t handle;


  uint8_t msg_type;







  uint16_t src_addr;


  uint16_t src_port;


  uint16_t dst_addr;


  uint16_t dst_port;


  uint32_t token;




  uint32_t opcode;



  void* payload;


  uint32_t payload_size;

};
# 1097 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api.h"
typedef struct aprv2_cmd_accept_command_t aprv2_cmd_accept_command_t;


struct aprv2_cmd_accept_command_t
{
  uint32_t handle;
  aprv2_packet_t* packet;
};
# 1142 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api.h"
typedef struct aprv2_cmd_end_command_t aprv2_cmd_end_command_t;


struct aprv2_cmd_end_command_t
{
  uint32_t handle;
  aprv2_packet_t* packet;
  uint32_t status;


};
# 33 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api_inline.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_msg_if.h" 1
# 53 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_msg_if.h"
typedef struct aprv2_ibasic_rsp_result_t aprv2_ibasic_rsp_result_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 56 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_msg_if.h" 2



struct aprv2_ibasic_rsp_result_t
{
  uint32_t opcode;
  uint32_t status;

}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 66 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_msg_if.h" 2
;
# 86 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_msg_if.h"
typedef struct aprv2_ibasic_evt_accepted_t aprv2_ibasic_evt_accepted_t;


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_begin.h" 1
# 89 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_msg_if.h" 2



struct aprv2_ibasic_evt_accepted_t
{
  uint32_t opcode;
}

# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/apr_pack_end.h"
  __attribute__((packed))
# 97 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_msg_if.h" 2
;
# 34 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api_inline.h" 2
# 43 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/apr/api/aprv2_api_inline.h"
static inline int32_t __aprv2_cmd_register (
  uint32_t* ret_handle,
  uint16_t addr,
  aprv2_dispatch_fn_t dispatch_fn,
  void* dispatch_data
)
{
  aprv2_cmd_register_t args;

  args.ret_handle = ret_handle;
  args.addr = addr;
  args.dispatch_fn = dispatch_fn;
  args.dispatch_data = dispatch_data;

  return apr_call( ( 0x000110D5 ), &args, sizeof( args ) );
}

static inline int32_t __aprv2_cmd_register2 (
  uint32_t* ret_handle,
  char_t* name,
  uint32_t name_size,
  uint16_t addr,
  aprv2_dispatch_fn_t dispatch_fn,
  void* dispatch_data,
  uint16_t* ret_addr
)
{
  aprv2_cmd_register2_t args;

  args.ret_handle = ret_handle;
  args.name = name;
  args.name_size = name_size;
  args.addr = addr;
  args.dispatch_fn = dispatch_fn;
  args.dispatch_data = dispatch_data;
  args.ret_addr = ret_addr;

  return apr_call( ( 0x000100BD ), &args, sizeof( args ) );
}

static inline int32_t __aprv2_cmd_deregister (
  uint32_t handle
)
{
  aprv2_cmd_deregister_t args;

  args.handle = handle;

  return apr_call( ( 0x000110D6 ), &args, sizeof( args ) );
}

static inline int32_t __aprv2_cmd_async_send (
  uint32_t handle,
  aprv2_packet_t* packet
)
{
  aprv2_cmd_async_send_t args;

  args.handle = handle;
  args.packet = packet;

  return apr_call( ( 0x000110D7 ), &args, sizeof( args ) );
}

static inline int32_t __aprv2_cmd_alloc (
  uint32_t handle,
  uint32_t alloc_type,
  uint32_t alloc_size,
  aprv2_packet_t** ret_packet
)
{
  aprv2_cmd_alloc_t args;

  args.handle = handle;
  args.alloc_type = alloc_type;
  args.alloc_size = alloc_size;
  args.ret_packet = ret_packet;

  return apr_call( ( 0x000110D8 ), &args, sizeof( args ) );
}

static inline int32_t __aprv2_cmd_free (
  uint32_t handle,
  aprv2_packet_t* packet
)
{
  aprv2_cmd_free_t args;

  args.handle = handle;
  args.packet = packet;

  return apr_call( ( 0x000110D9 ), &args, sizeof( args ) );
}





static inline int32_t __aprv2_cmd_query_address (
  uint32_t handle,
  uint16_t* ret_addr
)
{
  aprv2_cmd_query_address_t args;

  args.handle = handle;
  args.ret_addr = ret_addr;

  return apr_call( ( 0x000110E9 ), &args, sizeof( args ) );
}


static inline int32_t __aprv2_cmd_local_dns_lookup (
  char_t* name,
  uint32_t name_size,
  uint16_t* ret_addr
)
{
  aprv2_cmd_local_dns_lookup_t args;

  args.name = name;
  args.name_size = name_size;
  args.ret_addr = ret_addr;

  return apr_call( ( 0x000110EA ), &args, sizeof( args ) );
}





static inline int32_t __aprv2_cmd_forward (
  uint32_t handle,
  aprv2_packet_t* packet
)
{
  aprv2_cmd_forward_t args;

  args.handle = handle;
  args.packet = packet;

  return apr_call( ( 0x000110DA ), &args, sizeof( args ) );
}

static inline int32_t __aprv2_cmd_alloc_ext (
  uint32_t handle,
  uint8_t msg_type,
  uint16_t src_addr,
  uint16_t src_port,
  uint16_t dst_addr,
  uint16_t dst_port,
  uint32_t token,
  uint32_t opcode,
  uint32_t payload_size,
  aprv2_packet_t** ret_packet
)
{
  aprv2_cmd_alloc_ext_t args;

  args.handle = handle;
  args.msg_type = msg_type;
  args.src_addr = src_addr;
  args.src_port = src_port;
  args.dst_addr = dst_addr;
  args.dst_port = dst_port;
  args.token = token;
  args.opcode = opcode;
  args.payload_size = payload_size;
  args.ret_packet = ret_packet;

  return apr_call( ( 0x000110DB ), &args, sizeof( args ) );
}

static inline int32_t __aprv2_cmd_alloc_send (
  uint32_t handle,
  uint8_t msg_type,
  uint16_t src_addr,
  uint16_t src_port,
  uint16_t dst_addr,
  uint16_t dst_port,
  uint32_t token,
  uint32_t opcode,
  void* payload,
  uint32_t payload_size
)
{
  aprv2_cmd_alloc_send_t args;

  args.handle = handle;
  args.msg_type = msg_type;
  args.src_addr = src_addr;
  args.src_port = src_port;
  args.dst_addr = dst_addr;
  args.dst_port = dst_port;
  args.token = token;
  args.opcode = opcode;
  args.payload = payload;
  args.payload_size = payload_size;

  return apr_call( ( 0x000110DC ), &args, sizeof( args ) );
}

static inline int32_t __aprv2_cmd_accept_command (
  uint32_t handle,
  aprv2_packet_t* packet
)
{
  aprv2_cmd_accept_command_t args;

  args.handle = handle;
  args.packet = packet;

  return apr_call( ( 0x000112C7 ), &args, sizeof( args ) );
}

static inline int32_t __aprv2_cmd_end_command (
  uint32_t handle,
  aprv2_packet_t* packet,
  uint32_t status
)
{
  aprv2_cmd_end_command_t args;

  args.handle = handle;
  args.packet = packet;
  args.status = status;

  return apr_call( ( 0x000110DD ), &args, sizeof( args ) );
}
# 39 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_diag_i.h" 2


# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagbuf.h" 1
# 106 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagbuf.h"
void
diagbuf_shorten (void * __attribute__((__packed__)) ptr, unsigned int length);
# 129 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagbuf.h"
void
diagbuf_flush (void);
# 42 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_diag_i.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h" 1
# 109 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h" 1
# 138 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
typedef struct
{
  unsigned int event_id;

  qword ts;

  uint8 length;

  uint8 payload[255];

}
diag_event_type;


typedef union
{

  msg_ext_type * ext_sprintf_msg;

  msg_ext_store_type * ext_msg;


  msg_qsr_store_type * qsr_msg;


  msg_v3_store_type * qsr_v3_msg;


  msg_v4_store_type * qsr_v4_msg;
}
diag_msg_store_type;


typedef struct
{
  unsigned int cmd_code;

  diag_msg_store_type msg_store;
}
diag_msg_type;

typedef struct
{
  unsigned int ssid;
  unsigned int ss_mask;
}
f3_listener_ext_key_type;


typedef enum
{
  DIAG_EXT_API_VERSION_0 = 0,


  DIAG_EXT_API_VERSION_1 = 1


} diag_ext_api_version_type;

typedef uint32(*diag_wcdma_wl1_frame_cb_type)(void);
typedef uint32(*diag_wcdma_wl1_frame_cb_ext_type)(uint32 id);




typedef uint8 diag_sp_type;



typedef uint8 diag_security_type;
# 221 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  diag_security_type diag_get_security_state (void);
# 236 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  int diag_alloc_check (unsigned int num_allocs, uint32 *alloc_sizes);
# 250 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  diag_sp_type diag_get_sp_state (void);
# 263 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
boolean diag_check_password (void *code_ptr);
# 279 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  void diag_timeout (void);
# 288 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  void diag_free (void *mem_ptr);
# 306 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  void diag_pause (unsigned int duration, boolean enable_rx_processing);
# 322 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  boolean diag_masks_enabled (void);
# 335 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
boolean diag_f3_trace_enabled_check(void);
# 347 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  typedef void (*diag_idle_processing_ftype) (void *param);
# 365 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  boolean diag_idle_processing_register (diag_idle_processing_ftype fp,
                     void *param);
# 386 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  boolean diag_idle_processing_unregister (diag_idle_processing_ftype fp,
                       void *param);
# 404 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  void diag_tx_notify (void);
# 427 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  typedef void (*diag_event_listener) (uint32 seq_num,
                                       const diag_event_type * event,
                                       void *param);
# 448 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  typedef void (*diag_event_listener_ext) (const diag_event_type * event,
                                       void *param);
# 469 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  boolean diag_add_event_listener (const unsigned int event_id,
                   const diag_event_listener listener,
                   void *param);
# 492 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  boolean diag_add_event_listener_ext (const uint16 *events, const unsigned int num_events,
                         const diag_event_listener_ext listener, void *param);
# 516 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  boolean diag_remove_event_listener (const unsigned int event_id,
                      const diag_event_listener listener,
                      void *param);
# 540 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  boolean diag_remove_event_listener_ext (const diag_event_listener_ext listener);
# 563 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  boolean diag_remove_event_listener_extp (const diag_event_listener_ext listener, void* param);
# 587 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  typedef void (*diag_log_listener) (uint32 seq_num, const byte *log,
                                     unsigned int length, void *param);
# 608 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  typedef void (*diag_log_listener_ext) (const byte *log,
                                     unsigned int length, void *param);
# 629 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  boolean diag_add_log_listener (const unsigned int log_code,
                 const diag_log_listener listener,
                 void *param);
# 651 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  boolean diag_add_log_listener_ext (const uint16* logs, const unsigned int num_logs,
                       const diag_log_listener_ext listener, void *param);
# 675 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  boolean diag_remove_log_listener (const unsigned int log_code,
                    const diag_log_listener listener,
                    void *param);
# 699 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  boolean diag_remove_log_listener_ext (const diag_log_listener_ext listener);
# 723 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  boolean diag_remove_log_listener_extp (const diag_log_listener_ext listener, void *param);
# 751 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  typedef void (*diag_f3_listener) (uint32 seq_num,
                                    const diag_msg_type * msg,
                                    void *param);
# 777 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  typedef void (*diag_f3_listener_ext) (const diag_msg_type * msg,
                                          void *param);
# 798 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  boolean diag_add_f3_listener (const unsigned int ssid,
                                const unsigned int ss_mask,
                                const diag_f3_listener listener,
                                void *param);
# 822 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
boolean diag_add_f3_listener_ext (f3_listener_ext_key_type *keys,
         const unsigned int num_keys,
         const diag_f3_listener_ext listener,
         void *param);
# 845 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  boolean diag_remove_f3_listener (const unsigned int ssid,
                                   const unsigned int ss_mask,
                                   const diag_f3_listener listener,
                                   void *param);
# 866 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  boolean diag_remove_f3_listener_ext (const diag_f3_listener_ext listener, void* param);
# 887 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  typedef void (*diag_cmd_rsp) (const byte *rsp, unsigned int length,
                                void *param);
# 905 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  boolean diag_cmd_req (const byte *req, unsigned int length,
                        const diag_cmd_rsp rsp_cb, void *param);
# 938 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  void diag_otasp_update (void);
# 956 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  void diag_task (uint32 param);
# 969 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
  void diag_early_init(void);
# 993 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
void diag_wcdma_wl1_frame_reg(diag_wcdma_wl1_frame_cb_type ptr);
# 1016 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
void diag_wcdma_wl1_frame_reg_ext(diag_wcdma_wl1_frame_cb_ext_type ptr);
# 1040 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
uint8 diag_set_tx_mode(uint8 mode);
# 1060 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
uint8 diag_get_tx_mode();
# 1079 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diag.h"
void diag_drain_tx_buffer();
# 110 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h" 2
# 128 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
typedef uint8 diagpkt_cmd_code_type;





typedef uint8 diagpkt_subsys_id_type;





typedef uint16 diagpkt_subsys_cmd_code_type;





typedef uint32 diagpkt_subsys_status_type;





typedef uint16 diagpkt_subsys_delayed_rsp_id_type;





typedef uint16 diagpkt_subsys_rsp_cnt;
# 167 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
typedef struct __attribute__((__packed__))
{
  byte opaque_header;
}
diagpkt_header_type;




typedef struct __attribute__((__packed__))
{
  byte opaque_header[4];
}
diagpkt_subsys_header_type;




typedef struct __attribute__((__packed__))
{
  byte opaque_header[12];
}
diagpkt_subsys_header_v2_type;
# 232 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
  typedef struct
  {
    word cmd_code_lo;
    word cmd_code_hi;
    void * __attribute__((__packed__)) (*func_ptr) (void * __attribute__((__packed__)) req_pkt_ptr, uint16 pkt_len);


  }
  diagpkt_user_table_entry_type;
# 249 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
  typedef struct
  {
    uint16 delay_flag;
    uint16 cmd_code;
    word subsysid;

    word count;
    uint16 proc_id;
    uint16 is_fwded;
    int port;
    const diagpkt_user_table_entry_type *user_table;
  } diagpkt_master_table_type;
# 296 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
  void diagpkt_tbl_reg (diagpkt_master_table_type * tbl_ptr, const diagpkt_user_table_entry_type * user_tbl_ptr);
# 323 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
  void diagpkt_tbl_dereg (diagpkt_master_table_type * tbl_ptr);
# 630 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
void diagpkt_rsp_q_init (void);
# 654 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
uint32 getWrapcount(uint16 wrap_flag);
# 693 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
void * __attribute__((__packed__)) diagpkt_alloc (diagpkt_cmd_code_type code, unsigned int length);
# 732 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
void * __attribute__((__packed__)) diagpkt_subsys_alloc (diagpkt_subsys_id_type id,
    diagpkt_subsys_cmd_code_type code, unsigned int length);
# 775 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
  void * __attribute__((__packed__)) diagpkt_subsys_alloc_v2 (diagpkt_subsys_id_type id,
    diagpkt_subsys_cmd_code_type code, unsigned int length);
# 816 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
  void * __attribute__((__packed__)) diagpkt_subsys_alloc_v2_delay (
    diagpkt_subsys_id_type id,
    diagpkt_subsys_cmd_code_type code,
    diagpkt_subsys_delayed_rsp_id_type delayed_rsp_id,
    unsigned int length);
# 851 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
void diagpkt_shorten (void * __attribute__((__packed__)) ptr, unsigned int length);
# 876 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
void diagpkt_free(void * __attribute__((__packed__))pkt);
# 911 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
  void diagpkt_commit (void * __attribute__((__packed__)) ptr);
# 938 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
  void diagpkt_delay_commit (void * __attribute__((__packed__)) pkt);
# 963 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
  diagpkt_cmd_code_type diagpkt_get_cmd_code (void * __attribute__((__packed__)) ptr);
# 989 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
void diagpkt_set_cmd_code (void * __attribute__((__packed__)) ptr, diagpkt_cmd_code_type cmd_code);
# 1019 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
diagpkt_subsys_id_type diagpkt_subsys_get_id (void * __attribute__((__packed__)) ptr);
# 1051 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
  diagpkt_subsys_cmd_code_type diagpkt_subsys_get_cmd_code (void * __attribute__((__packed__)) ptr);
# 1081 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
  diagpkt_subsys_status_type diagpkt_subsys_get_status (void * __attribute__((__packed__)) ptr);
# 1110 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
  void diagpkt_subsys_set_status (void * __attribute__((__packed__)) ptr,
      diagpkt_subsys_status_type status);
# 1142 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
diagpkt_subsys_delayed_rsp_id_type diagpkt_subsys_get_delayed_rsp_id
      (void * __attribute__((__packed__)) ptr);
# 1171 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
  void diagpkt_subsys_reset_delayed_rsp_id (void * __attribute__((__packed__)) ptr);
# 1205 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
  void diagpkt_subsys_set_rsp_cnt (void * __attribute__((__packed__)) ptr,
      diagpkt_subsys_rsp_cnt rsp_cnt);
# 1234 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
  void * __attribute__((__packed__)) diagpkt_err_rsp (diagpkt_cmd_code_type code,
    void * __attribute__((__packed__)) req_ptr, uint16 req_len);
# 1256 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
  void diagpkt_async_block (void);
# 1277 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagpkt.h"
  void diagpkt_async_release (void);
# 43 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_diag_i.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagcmd.h" 1
# 547 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagcmd.h"
typedef enum {
  DIAG_SUBSYS_OEM = 0,
  DIAG_SUBSYS_ZREX = 1,
  DIAG_SUBSYS_SD = 2,
  DIAG_SUBSYS_BT = 3,
  DIAG_SUBSYS_WCDMA = 4,
  DIAG_SUBSYS_HDR = 5,
  DIAG_SUBSYS_DIABLO = 6,
  DIAG_SUBSYS_TREX = 7,
  DIAG_SUBSYS_GSM = 8,
  DIAG_SUBSYS_UMTS = 9,
  DIAG_SUBSYS_HWTC = 10,
  DIAG_SUBSYS_FTM = 11,
  DIAG_SUBSYS_REX = 12,
  DIAG_SUBSYS_OS = DIAG_SUBSYS_REX,
  DIAG_SUBSYS_GPS = 13,
  DIAG_SUBSYS_WMS = 14,
  DIAG_SUBSYS_CM = 15,
  DIAG_SUBSYS_HS = 16,
  DIAG_SUBSYS_AUDIO_SETTINGS = 17,
  DIAG_SUBSYS_DIAG_SERV = 18,
  DIAG_SUBSYS_FS = 19,
  DIAG_SUBSYS_PORT_MAP_SETTINGS = 20,
  DIAG_SUBSYS_MEDIAPLAYER = 21,
  DIAG_SUBSYS_QCAMERA = 22,
  DIAG_SUBSYS_MOBIMON = 23,
  DIAG_SUBSYS_GUNIMON = 24,
  DIAG_SUBSYS_LSM = 25,
  DIAG_SUBSYS_QCAMCORDER = 26,
  DIAG_SUBSYS_MUX1X = 27,
  DIAG_SUBSYS_DATA1X = 28,
  DIAG_SUBSYS_SRCH1X = 29,
  DIAG_SUBSYS_CALLP1X = 30,
  DIAG_SUBSYS_APPS = 31,
  DIAG_SUBSYS_SETTINGS = 32,
  DIAG_SUBSYS_GSDI = 33,
  DIAG_SUBSYS_UIMDIAG = DIAG_SUBSYS_GSDI,
  DIAG_SUBSYS_TMC = 34,
  DIAG_SUBSYS_USB = 35,
  DIAG_SUBSYS_PM = 36,
  DIAG_SUBSYS_DEBUG = 37,
  DIAG_SUBSYS_QTV = 38,
  DIAG_SUBSYS_CLKRGM = 39,
  DIAG_SUBSYS_DEVICES = 40,
  DIAG_SUBSYS_WLAN = 41,
  DIAG_SUBSYS_PS_DATA_LOGGING = 42,
  DIAG_SUBSYS_PS = DIAG_SUBSYS_PS_DATA_LOGGING,
  DIAG_SUBSYS_MFLO = 43,
  DIAG_SUBSYS_DTV = 44,
  DIAG_SUBSYS_RRC = 45,
  DIAG_SUBSYS_PROF = 46,
  DIAG_SUBSYS_TCXOMGR = 47,
  DIAG_SUBSYS_NV = 48,
  DIAG_SUBSYS_AUTOCONFIG = 49,
  DIAG_SUBSYS_PARAMS = 50,
  DIAG_SUBSYS_MDDI = 51,
  DIAG_SUBSYS_DS_ATCOP = 52,
  DIAG_SUBSYS_L4LINUX = 53,
  DIAG_SUBSYS_MVS = 54,
  DIAG_SUBSYS_CNV = 55,
  DIAG_SUBSYS_APIONE_PROGRAM = 56,
  DIAG_SUBSYS_HIT = 57,
  DIAG_SUBSYS_DRM = 58,
  DIAG_SUBSYS_DM = 59,
  DIAG_SUBSYS_FC = 60,
  DIAG_SUBSYS_MEMORY = 61,
  DIAG_SUBSYS_FS_ALTERNATE = 62,
  DIAG_SUBSYS_REGRESSION = 63,
  DIAG_SUBSYS_SENSORS = 64,
  DIAG_SUBSYS_FLUTE = 65,
  DIAG_SUBSYS_ANALOG = 66,
  DIAG_SUBSYS_APIONE_PROGRAM_MODEM = 67,
  DIAG_SUBSYS_LTE = 68,
  DIAG_SUBSYS_BREW = 69,
  DIAG_SUBSYS_PWRDB = 70,
  DIAG_SUBSYS_CHORD = 71,
  DIAG_SUBSYS_SEC = 72,
  DIAG_SUBSYS_TIME = 73,
  DIAG_SUBSYS_Q6_CORE = 74,
  DIAG_SUBSYS_COREBSP = 75,
# 638 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/core/api/services/diagcmd.h"
  DIAG_SUBSYS_MFLO2 = 76,




  DIAG_SUBSYS_ULOG = 77,
  DIAG_SUBSYS_APR = 78,
  DIAG_SUBSYS_QNP = 79 ,
  DIAG_SUBSYS_STRIDE = 80 ,
  DIAG_SUBSYS_OEMDPP = 81 ,
  DIAG_SUBSYS_Q5_CORE = 82 ,
  DIAG_SUBSYS_USCRIPT = 83 ,
  DIAG_SUBSYS_NAS = 84 ,
  DIAG_SUBSYS_CMAPI = 85 ,
  DIAG_SUBSYS_SSM = 86,
  DIAG_SUBSYS_TDSCDMA = 87,
  DIAG_SUBSYS_SSM_TEST = 88,
  DIAG_SUBSYS_MPOWER = 89,
  DIAG_SUBSYS_QDSS = 90,
  DIAG_SUBSYS_CXM = 91,
  DIAG_SUBSYS_GNSS_SOC = 92,
  DIAG_SUBSYS_TTLITE = 93,
  DIAG_SUBSYS_FTM_ANT = 94,
  DIAG_SUBSYS_MLOG = 95,
  DIAG_SUBSYS_LIMITSMGR = 96,
  DIAG_SUBSYS_EFSMONITOR = 97,
  DIAG_SUBSYS_DISPLAY_CALIBRATION = 98,
  DIAG_SUBSYS_VERSION_REPORT = 99,
  DIAG_SUBSYS_DS_IPA = 100,
  DIAG_SUBSYS_SYSTEM_OPERATIONS = 101,
  DIAG_SUBSYS_CNSS_POWER = 102,
  DIAG_SUBSYS_LWIP = 103,
  DIAG_SUBSYS_IMS_QVP_RTP = 104,
  DIAG_SUBSYS_STORAGE = 105,
  DIAG_SUBSYS_WCI2 = 106,
  DIAG_SUBSYS_AOSTLM_TEST = 107,
  DIAG_SUBSYS_CFCM = 108,
  DIAG_SUBSYS_CORE_SERVICES = 109,
  DIAG_SUBSYS_CVD = 110,
  DIAG_SUBSYS_MCFG = 111,
  DIAG_SUBSYS_MODEM_STRESSFW = 112,
  DIAG_SUBSYS_DS_DS3G = 113,
  DIAG_SUBSYS_TRM = 114,
  DIAG_SUBSYS_IMS = 115,
  DIAG_SUBSYS_OTA_FIREWALL = 116,
  DIAG_SUBSYS_I15P4 = 117,
  DIAG_SUBSYS_QDR = 118,
  DIAG_SUBSYS_MCS = 119,
  DIAG_SUBSYS_MODEMFW = 120,
  DIAG_SUBSYS_QNAD = 121,
  DIAG_SUBSYS_F_RESERVED = 122,
  DIAG_SUBSYS_V2X = 123,
  DIAG_SUBSYS_QMESA = 124,
  DIAG_SUBSYS_SLEEP = 125,
  DIAG_SUBSYS_QUEST = 126,
  DIAG_SUBSYS_CDSP_QMESA = 127,
  DIAG_SUBSYS_QMCT = 128,

  DIAG_SUBSYS_LAST,


  DIAG_SUBSYS_RESERVED_OEM_0 = 250,
  DIAG_SUBSYS_RESERVED_OEM_1 = 251,
  DIAG_SUBSYS_RESERVED_OEM_2 = 252,
  DIAG_SUBSYS_RESERVED_OEM_3 = 253,
  DIAG_SUBSYS_RESERVED_OEM_4 = 254,
  DIAG_SUBSYS_LEGACY = 255
} diagpkt_subsys_cmd_enum_type;
# 44 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_diag_i.h" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 1
# 45 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_diag_i.h" 2
# 65 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/cvd_diag_i.h"
typedef struct __attribute__((__packed__)) cvd_diag_cmd_req_t
{
  diagpkt_subsys_header_type header;
  uint8_t cmd;
  uint8_t payload1;
  uint8_t payload2;
  uint8_t payload3;
  uint8_t payload4;
  uint8_t payload5;
}
  cvd_diag_cmd_req_t;

typedef struct __attribute__((__packed__)) cvd_diag_rsp_t
{
  diagpkt_subsys_header_type header;
  uint32_t result;
}
  cvd_diag_rsp_t;





 void* __attribute__((__packed__)) cvd_diag_cmd_processing (
  void* __attribute__((__packed__)) req_pkt,
  uint16 pkt_len
);
# 38 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/inc/private/vss_private_if.h" 2
# 14 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/common/cvd/cvd_devcfg/mdm9x07/cvd_devcfg.c" 2
# 123 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/common/cvd/cvd_devcfg/mdm9x07/cvd_devcfg.c"
cvd_devcfg_voice_use_case_na_values_t cvd_devcfg_voice_use_case_na_values =
  { ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_0_use_cases[] =
  {




    { 1, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0x00010F70 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0 ) },
    { 1, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0x00010F71 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0 ) },
    { 1, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0x00010F89 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0 ) },




    { 1, ( 0xFFFF ), 0, 0, 0, ( 0xFFFF ), 0, 0, 0, ( 0x00010F70 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },
    { 1, ( 0xFFFF ), 0, 0, 0, ( 0xFFFF ), 0, 0, 0, ( 0x00010F71 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },
    { 1, ( 0xFFFF ), 0, 0, 0, ( 0xFFFF ), 0, 0, 0, ( 0x00010F89 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },
    { 1, ( 0xFFFF ), 0, 0, 0, ( 0xFFFF ), 0, 0, 0, ( 0x00010F73 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },
    { 1, ( 0xFFFF ), 0, 0, 0, ( 0xFFFF ), 0, 0, 0, ( 0x00010F72 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },
    { 1, ( 0xFFFF ), 0, 0, 0, ( 0xFFFF ), 0, 0, 0, ( 0x00010F86 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_1_use_cases[] =
  {




    { 1, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0x00010F70 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },
    { 1, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0x00010F71 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },
    { 1, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0x00010F89 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },
    { 1, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0x00010F86 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_2_use_cases[] =
  {




    { 1, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0x00010F73 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },
    { 1, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0x00010F86 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },
    { 1, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0x00010F72 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },
    { 1, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0x00010F74 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },
    { 1, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0x00010F88 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },
    { 1, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0x00010F87 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_clock_level_3_use_cases[] =
  {



    { ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) }
  };

static cvd_devcfg_clock_level_t cvd_devcfg_clock_levels[] =
  {
    { ( 384000000 ), ( sizeof(cvd_devcfg_clock_level_0_use_cases) / sizeof( (cvd_devcfg_clock_level_0_use_cases)[0] ) ), cvd_devcfg_clock_level_0_use_cases },
    { ( 480000000 ), ( sizeof(cvd_devcfg_clock_level_1_use_cases) / sizeof( (cvd_devcfg_clock_level_1_use_cases)[0] ) ), cvd_devcfg_clock_level_1_use_cases },
    { ( 576000000 ), ( sizeof(cvd_devcfg_clock_level_2_use_cases) / sizeof( (cvd_devcfg_clock_level_2_use_cases)[0] ) ), cvd_devcfg_clock_level_2_use_cases },
    { ( 576000000 ), ( sizeof(cvd_devcfg_clock_level_3_use_cases) / sizeof( (cvd_devcfg_clock_level_3_use_cases)[0] ) ), cvd_devcfg_clock_level_3_use_cases }
  };

cvd_devcfg_clock_table_t cvd_devcfg_clock_table =
  { ( sizeof(cvd_devcfg_clock_levels) / sizeof( (cvd_devcfg_clock_levels)[0] ) ), cvd_devcfg_clock_levels };

static cvd_devcfg_clock_frequency_level_t cvd_devcfg_clock_frequency_levels[] = { {( 384000000 )}, {( 480000000 )}, {( 576000000 )}, {( 614000000 )}, {( 691000000 )} } ;

cvd_devcfg_clock_frequency_level_table_t cvd_devcfg_clock_frequency_level_table = {( sizeof(cvd_devcfg_clock_frequency_levels) / sizeof( (cvd_devcfg_clock_frequency_levels)[0] ) ), cvd_devcfg_clock_frequency_levels } ;


cvd_devcfg_mmpm_core_info_t cvd_devcfg_mmpm_core_info =
  { MMPM_CORE_ID_LPASS_ADSP, MMPM_CORE_INSTANCE_0 };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_0_use_cases[] =
  {




    { 1, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0x00010F70 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },
    { 1, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0x00010F71 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },
    { 1, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0x00010F89 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },



    { 1, 1, 0, 0, 0, 1, 0, 0, 0, ( 0x00010F73 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, ( 0x00010F72 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, ( 0x00010F86 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, ( 0x00010F74 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_1_use_cases[] =
  {




    { 1, ( 0xFFFF ), ( 0xFFFF ), 0, 0, 0, 1, 0, 0, ( 0x00010F73 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },
    { 1, ( 0xFFFF ), ( 0xFFFF ), 0, 0, 0, 1, 0, 0, ( 0x00010F72 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },
    { 1, ( 0xFFFF ), ( 0xFFFF ), 0, 0, 0, 1, 0, 0, ( 0x00010F86 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_2_use_cases[] =
  {



   { 1, ( 0xFFFF ), ( 0xFFFF ), 0, 0, 0, 1, 0, 0, ( 0x00010F74 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },



   { 1, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0xFFFF ), ( 0xFFFF ), 0, 0, ( 0x00010F88 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) },

   { 1, ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0x00010F87 ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) }
  };

static cvd_devcfg_supported_voice_use_case_t cvd_devcfg_bw_level_3_use_cases[] =
  {

    { ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFF ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFFFFFF ), ( 0xFFFF ) }
  };

static cvd_devcfg_mmpm_bw_value_t cvd_devcfg_mmpm_bw_values[] =
  {
    {
      {
         .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { ( ( 40 ) << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      ( sizeof(cvd_devcfg_bw_level_0_use_cases) / sizeof( (cvd_devcfg_bw_level_0_use_cases)[0] ) ),
      cvd_devcfg_bw_level_0_use_cases
    },
    {
      {
         .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { ( ( 50 ) << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      ( sizeof(cvd_devcfg_bw_level_1_use_cases) / sizeof( (cvd_devcfg_bw_level_1_use_cases)[0] ) ),
      cvd_devcfg_bw_level_1_use_cases
    },
    {
      {
         .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { ( ( 80 ) << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      ( sizeof(cvd_devcfg_bw_level_2_use_cases) / sizeof( (cvd_devcfg_bw_level_2_use_cases)[0] ) ),
      cvd_devcfg_bw_level_2_use_cases
    },
    {
      {
         .busRoute = { MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE },
         .bwValue.busBwValue = { ( ( 130 ) << 20 ), 100, MMPM_BW_USAGE_LPASS_DSP }
      },
      ( sizeof(cvd_devcfg_bw_level_3_use_cases) / sizeof( (cvd_devcfg_bw_level_3_use_cases)[0] ) ),
      cvd_devcfg_bw_level_3_use_cases
    }
  };

cvd_devcfg_mmpm_bw_table_t cvd_devcfg_parser_bw_table =
  { ( sizeof(cvd_devcfg_mmpm_bw_values) / sizeof( (cvd_devcfg_mmpm_bw_values)[0] ) ), cvd_devcfg_mmpm_bw_values };
