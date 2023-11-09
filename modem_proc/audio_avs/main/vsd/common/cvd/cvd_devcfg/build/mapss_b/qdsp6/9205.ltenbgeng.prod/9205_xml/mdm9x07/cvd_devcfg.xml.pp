# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/common/cvd/cvd_devcfg/mdm9x07/cvd_devcfg.xml"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 328 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/vsd/common/cvd/cvd_devcfg/mdm9x07/cvd_devcfg.xml" 2
<!--
  Copyright (c) 2013 - 2016 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: $
  $Author: $
 -->

<!-- Non-DAL drivers uses NULL driver name. -->
<driver name="NULL">
<!-- CVD voice use case power management configurations. -->
<device id="QCOM_CVD">

<props name="CLOCK_TABLE" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
cvd_devcfg_clock_table
</props>
<props name="USE_CASE_NA_VAL" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
cvd_devcfg_voice_use_case_na_values
</props>
<props name="MMPM_CORE_INFO" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
cvd_devcfg_mmpm_core_info
</props>
<props name="MMPM_BW_REQUIREMENT" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
cvd_devcfg_parser_bw_table
</props>
<props name="MMPM_SLEEP_LATENCY" type=DALPROP_ATTR_TYPE_UINT32>
25
</props>
<props name="CYCLES_PER_KP_FACTOR" type=DALPROP_ATTR_TYPE_UINT32>
9600
</props>
<props name="MMPM_CYCLES_PER_PACKET" type=DALPROP_ATTR_TYPE_UINT32>
4
</props>
<props name="NUMBER_OF_HW_THREADS" type=DALPROP_ATTR_TYPE_UINT32>
4
</props>
<props name="MAX_Q6_CORE_CLOCK_MHZ" type=DALPROP_ATTR_TYPE_UINT32>
691
</props>
<props name="CSM_SLEEP_THRESHOLD_US" type=DALPROP_ATTR_TYPE_UINT32>
3000
</props>
<props name="CSM_SAFETY_MARGIN_US" type=DALPROP_ATTR_TYPE_UINT32>
1500
</props>
<props name="CLOCK_FREQUENCY_TABLE" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
cvd_devcfg_clock_frequency_level_table
</props>

</device>

</driver>
