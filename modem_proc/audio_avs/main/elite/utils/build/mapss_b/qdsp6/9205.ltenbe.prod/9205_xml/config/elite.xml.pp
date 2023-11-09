# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/elite/utils/devcfg/config/elite.xml"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 328 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/elite/utils/devcfg/config/elite.xml" 2
<!--
Copyright (c) 2016 QUALCOMM Technologies, Inc. All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary. Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.

   DESCRIPTION: Elite Device Configuration.
   $Header: $
 -->

<!-- NULL Driver does not require Dal Driver Interface APIs, since none of AVS HWD drivers uses DAL Device framework -->
<driver name="NULL">
   <!-- LPA -->
   <device id="LPA">
      <props name="LPAPropStructPtr" type=DALPROP_ATTR_TYPE_STRUCT_PTR>lpa_prop</props>
   </device>
   <!-- Thread Priority -->
   <device id="THRD_PRIO">
      <props name="ThrdPrioPropStructPtr" type=DALPROP_ATTR_TYPE_STRUCT_PTR>elite_thrd_prio_prop</props>
   </device>
</driver>
