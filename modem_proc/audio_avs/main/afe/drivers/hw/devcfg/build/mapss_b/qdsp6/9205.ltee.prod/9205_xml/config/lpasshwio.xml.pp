# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/afe/drivers/hw/devcfg/config/lpasshwio.xml"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 328 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.CT.2.0-00396-9205_LTENBGE_PACK-1_20231101_135339/b/modem_proc/audio_avs/main/afe/drivers/hw/devcfg/config/lpasshwio.xml" 2
<!--
  Copyright (c) 2012 Qualcomm Technologies Incorporated.
           All Rights Reserved.

   DESCRIPTION: LPASS HWIO driver.
   $Header: $
 -->

<!-- NULL Driver does not require Dal Driver Interface APIs, since none of AVS HWD drivers uses DAL Device framework -->
<driver name="NULL">
   <!-- LPAIF -->
   <device id="LPAIF">
      <props name="LpaifPropStructPtr" type=DALPROP_ATTR_TYPE_STRUCT_PTR>lpaif_prop</props>
   </device>
   <!-- AUDIOIFDMA -->
   <device id="AUDIOIFDMA">
      <props name="AudioIfDmaPropStructPtr" type=DALPROP_ATTR_TYPE_STRUCT_PTR>audioif_dma_prop</props>
   </device>
   <!-- HDMIOUTDMA -->
   <device id="HDMIOUTDMA">
      <props name="HdmiOutDmaPropStructPtr" type=DALPROP_ATTR_TYPE_STRUCT_PTR>hdmiout_dma_prop</props>
   </device>
   <!-- GenericClk -->
   <device id="GENERICCLK">
      <props name="GenericclkPropStructPtr" type=DALPROP_ATTR_TYPE_STRUCT_PTR>genericclk_prop</props>
   </device>
   <!-- AVTimer -->
   <device id="AVTIMER">
      <props name="AvtimerPropStructPtr" type=DALPROP_ATTR_TYPE_STRUCT_PTR>avtimer_prop</props>
   </device>
   <!-- Resampler -->
   <device id="RESAMPLER">
      <props name="ResamplerPropStructPtr" type=DALPROP_ATTR_TYPE_STRUCT_PTR>resampler_prop</props>
   </device>
   <!-- VFR -->
   <device id="VFR">
      <props name="VfrPropStructPtr" type=DALPROP_ATTR_TYPE_STRUCT_PTR>vfr_prop</props>
   </device>
   <device id="AFERiva">
      <props name="AFERivaPropStructPtr" type=DALPROP_ATTR_TYPE_STRUCT_PTR>afe_riva_prop</props>
   </device>
   <device id="AFESlimbus">
      <props name="AFESlimbusPropStructPtr" type=DALPROP_ATTR_TYPE_STRUCT_PTR>afe_slimbus_prop</props>
   </device>
   <device id="LPASS">
      <props name="LpassPropStructPtr" type=DALPROP_ATTR_TYPE_STRUCT_PTR>lpass_prop</props>
   </device>
   <!--SPDIFTX -->
   <device id="SPDIFTX">
      <props name="SpdifTxPropStructPtr" type=DALPROP_ATTR_TYPE_STRUCT_PTR>spdiftx_prop</props>
   </device>
   <!--HDMITX -->
   <device id="HDMITX">
      <props name="HdmiOutputPropStructPtr" type=DALPROP_ATTR_TYPE_STRUCT_PTR>hdmi_output_prop</props>
   </device>
  <!-- LPM -->
  <device id="LPM">
    <props name="LPMPropStructPtr" type=DALPROP_ATTR_TYPE_STRUCT_PTR>lpm_prop</props>
  </device>
</driver>
