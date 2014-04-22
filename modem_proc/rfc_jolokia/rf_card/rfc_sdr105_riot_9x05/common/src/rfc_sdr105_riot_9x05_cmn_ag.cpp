
/*
WARNING: This file is auto-generated.

Generated using: rfc_autogen.exe
Generated from:  V5.9.414 of RFC_HWSWCD.xlsm
*/

/*=============================================================================

          R F C     A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the RF Card.

Copyright (c) 2017 Qualcomm Technologies Incorporated.  All Rights Reserved.

$Header: //components/rel/rfc_jolokia.mpss/2.10/rf_card/rfc_sdr105_riot_9x05/common/src/rfc_sdr105_riot_9x05_cmn_ag.cpp#2 $ 


=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include <stringl/stringl.h>
#include "rfc_msm_signal_info_ag.h" 
#include "rfc_sdr105_riot_9x05_cmn_ag.h" 
#include "DDITlmm.h" 
#include "rfc_common.h" 
#include "rfcommon_core_sbi.h" 

#ifdef FEATURE_LTE
#include "rfc_sdr105_riot_9x05_lte_config_ag.h" 
#endif 

#ifdef FEATURE_CGPS
#include "rfc_sdr105_riot_9x05_gnss_config_ag.h" 
#endif 



rfc_phy_device_info_type rfc_sdr105_riot_9x05_phy_devices_list[] = 
{
  { /*Device: SDR105 */ 
    SDR105, /* PHY_DEVICE_NAME */ 
    0, /* PHY_DEVICE_INSTANCE */ 
    RFC_NO_ALTERNATE_PART, /* PHY_DEVICE_ALT_PART_NUM_OF_INSTANCE */ 
    RFDEVICE_COMM_PROTO_RFFE, /* PHY_DEVICE_COMM_PROTOCOL */ 
    RFDEVICE_COMM_PROTO_VERSION_DEFAULT, /* PHY_DEVICE_COMM_PROTOCOL_VERSION */ 
    {    0,0 /* 0 not specified */,}, /* PHY_DEVICE_COMM_BUS */ 
    0X217, /* PHY_DEVICE_MANUFACTURER_ID */ 
    0xC6, /* PHY_DEVICE_PRODUCT_ID */ 
    ( 1 | RFC_SKIP_RFFE_DETECT_BIT_IND ), /* PHY_DEVICE_PRODUCT_REV */ 
    0x09, /* DEFAULT USID RANGE START */
    0x09, /* DEFAULT USID RANGE END */
    0x09, /* PHY_DEVICE_ASSIGNED_USID */ 
    0 /*Warning: Not specified*/, /* RFFE_GROUP_ID */ 
    FALSE, /* INIT */ 
    RFC_INVALID_PARAM, /* ASSOCIATED_DAC */ 
  }, /* END - Device: SDR105 */ 

  { /*Device: IOTFE */ 
    GEN_DEVICE /* IOTFE */, /* PHY_DEVICE_NAME */ 
    1, /* PHY_DEVICE_INSTANCE */ 
    RFC_NO_ALTERNATE_PART, /* PHY_DEVICE_ALT_PART_NUM_OF_INSTANCE */ 
    RFDEVICE_COMM_PROTO_RFFE, /* PHY_DEVICE_COMM_PROTOCOL */ 
    RFDEVICE_COMM_PROTO_VERSION_DEFAULT, /* PHY_DEVICE_COMM_PROTOCOL_VERSION */ 
    {    1,0 /* 0 not specified */,}, /* PHY_DEVICE_COMM_BUS */ 
    0X217, /* PHY_DEVICE_MANUFACTURER_ID */ 
    0xC7, /* PHY_DEVICE_PRODUCT_ID */ 
    1, /* PHY_DEVICE_PRODUCT_REV */ 
    0x0A, /* DEFAULT USID RANGE START */
    0x0A, /* DEFAULT USID RANGE END */
    0x0A, /* PHY_DEVICE_ASSIGNED_USID */ 
    0 /*Warning: Not specified*/, /* RFFE_GROUP_ID */ 
    FALSE, /* INIT */ 
    RFC_INVALID_PARAM, /* ASSOCIATED_DAC */ 
  }, /* END - Device: IOTFE */ 

  { /*Device: QAT3516 */ 
    QAT3516, /* PHY_DEVICE_NAME */ 
    2, /* PHY_DEVICE_INSTANCE */ 
    RFC_NO_ALTERNATE_PART, /* PHY_DEVICE_ALT_PART_NUM_OF_INSTANCE */ 
    RFDEVICE_COMM_PROTO_RFFE, /* PHY_DEVICE_COMM_PROTOCOL */ 
    RFDEVICE_COMM_PROTO_VERSION_2_BURST, /* PHY_DEVICE_COMM_PROTOCOL_VERSION */ 
    {    2,0 /* 0 not specified */,}, /* PHY_DEVICE_COMM_BUS */ 
    0X217, /* PHY_DEVICE_MANUFACTURER_ID */ 
    0X585, /* PHY_DEVICE_PRODUCT_ID */ 
    0, /* PHY_DEVICE_PRODUCT_REV */ 
    0X7, /* DEFAULT USID RANGE START */
    0X7, /* DEFAULT USID RANGE END */
    0x07, /* PHY_DEVICE_ASSIGNED_USID */ 
    0 /*Warning: Not specified*/, /* RFFE_GROUP_ID */ 
    FALSE, /* INIT */ 
    RFC_INVALID_PARAM, /* ASSOCIATED_DAC */ 
  }, /* END - Device: QAT3516 */ 

  { /*Device: RFDEVICE_INVALID */ 
    RFDEVICE_INVALID, /* PHY_DEVICE_NAME */ 
    0 /*Warning: Not specified*/, /* PHY_DEVICE_INSTANCE */ 
    0 /*Warning: Not specified*/, /* PHY_DEVICE_ALT_PART_NUM_OF_INSTANCE */ 
    RFDEVICE_COMM_PROTO_INVALID, /* PHY_DEVICE_COMM_PROTOCOL */ 
    RFDEVICE_COMM_PROTO_VERSION_DEFAULT, /* PHY_DEVICE_COMM_PROTOCOL_VERSION */ 
    {    0 /* 0 not specified */,0 /* 0 not specified */,}, /* PHY_DEVICE_COMM_BUS */ 
    0 /*Warning: Not specified*/, /* PHY_DEVICE_MANUFACTURER_ID */ 
    0 /*Warning: Not specified*/, /* PHY_DEVICE_PRODUCT_ID */ 
    0 /*Warning: Not specified*/, /* PHY_DEVICE_PRODUCT_REV */ 
    0 /*Warning: Not specified*/, /* DEFAULT USID RANGE START */
    0 /*Warning: Not specified*/, /* DEFAULT USID RANGE END */
    0 /*Warning: Not specified*/, /* PHY_DEVICE_ASSIGNED_USID */ 
    0 /*Warning: Not specified*/, /* RFFE_GROUP_ID */ 
    FALSE, /* INIT */ 
    RFC_INVALID_PARAM, /* ASSOCIATED_DAC */ 
  }, /* END - Device: RFDEVICE_INVALID */ 

};


rfc_logical_device_info_type rfc_sdr105_riot_9x05_logical_devices_list[] = 
{
  { /*Device: SDR105 */ 
    RFDEVICE_TRANSCEIVER, /* DEVICE_MODULE_TYPE */ 
    SDR105, /* DEVICE_MODULE_NAME */ 
    0, /* DEVICE_MODULE_TYPE_INSTANCE */ 
    0, /* ASSOCIATED_PHY_DEVICE_INSTANCE */ 
  }, /* END - Device: SDR105 */ 

  { /*Device: IOTFE */ 
    RFDEVICE_PA, /* DEVICE_MODULE_TYPE */ 
    GEN_PA /* IOTFE */, /* DEVICE_MODULE_NAME */ 
    0, /* DEVICE_MODULE_TYPE_INSTANCE */ 
    1, /* ASSOCIATED_PHY_DEVICE_INSTANCE */ 
  }, /* END - Device: IOTFE */ 

  { /*Device: IOTFE_ASM */ 
    RFDEVICE_ASM, /* DEVICE_MODULE_TYPE */ 
    GEN_ASM /* IOTFE_ASM */, /* DEVICE_MODULE_NAME */ 
    0, /* DEVICE_MODULE_TYPE_INSTANCE */ 
    1, /* ASSOCIATED_PHY_DEVICE_INSTANCE */ 
  }, /* END - Device: IOTFE_ASM */ 

  { /*Device: QAT3516 */ 
    RFDEVICE_TUNER, /* DEVICE_MODULE_TYPE */ 
    QAT3516, /* DEVICE_MODULE_NAME */ 
    0, /* DEVICE_MODULE_TYPE_INSTANCE */ 
    2, /* ASSOCIATED_PHY_DEVICE_INSTANCE */ 
  }, /* END - Device: QAT3516 */ 

  { /*Device: TRX_HDET */ 
    RFDEVICE_HDET, /* DEVICE_MODULE_TYPE */ 
    TRX_HDET, /* DEVICE_MODULE_NAME */ 
    0, /* DEVICE_MODULE_TYPE_INSTANCE */ 
    0, /* ASSOCIATED_PHY_DEVICE_INSTANCE */ 
  }, /* END - Device: TRX_HDET */ 

  { /*Device: PA_THERM */ 
    RFDEVICE_THERM_MITIGATION, /* DEVICE_MODULE_TYPE */ 
    PA_THERM, /* DEVICE_MODULE_NAME */ 
    0, /* DEVICE_MODULE_TYPE_INSTANCE */ 
    1, /* ASSOCIATED_PHY_DEVICE_INSTANCE */ 
  }, /* END - Device: PA_THERM */ 

  { /*Device: RFDEVICE_INVALID */ 
    RFDEVICE_TYPE_INVALID, /* DEVICE_MODULE_TYPE */ 
    RFDEVICE_INVALID, /* DEVICE_MODULE_NAME */ 
    0 /*Warning: Not specified*/, /* DEVICE_MODULE_TYPE_INSTANCE */ 
    0 /*Warning: Not specified*/, /* ASSOCIATED_PHY_DEVICE_INSTANCE */ 
  }, /* END - Device: RFDEVICE_INVALID */ 

};


rfc_signal_info_type rfc_sdr105_riot_9x05_sig_info[RFC_SDR105_RIOT_9X05_SIG_NUM + 1] = 
{
  { RFC_MSM_TIMING_PA_CTL                   , RFC_LOW, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_TIMING_PA_CTL */ 
  { RFC_MSM_TIMING_PA_RANGE                 , RFC_LOW, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_TIMING_PA_RANGE */ 
  { RFC_MSM_TIMING_ASM_CTL                  , RFC_LOW, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_TIMING_ASM_CTL */ 
  { RFC_MSM_TIMING_TUNER_CTL                , RFC_LOW, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_TIMING_TUNER_CTL */ 
  { RFC_MSM_TIMING_PAPM_CTL                 , RFC_LOW, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_TIMING_PAPM_CTL */ 
  { RFC_MSM_TIMING_TX_TX_RF_ON0             , RFC_LOW, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_TIMING_TX_TX_RF_ON0 */ 
  { RFC_MSM_TIMING_TX_RX_RF_ON0             , RFC_LOW, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_TIMING_TX_RX_RF_ON0 */ 
  { RFC_MSM_TIMING_ASM_TRIGGER              , RFC_CONFIG_ONLY, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_TIMING_ASM_TRIGGER */ 
  { RFC_MSM_TIMING_PAPM_TX_TX_TRIGGER       , RFC_CONFIG_ONLY, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_TIMING_PAPM_TX_TX_TRIGGER */ 
  { RFC_MSM_TIMING_PAPM_OFF_TX_RX_TX_TRIGGER, RFC_CONFIG_ONLY, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_TIMING_PAPM_OFF_TX_RX_TX_TRIGGER */ 
  { RFC_MSM_TIMING_PA_TRIGGER               , RFC_CONFIG_ONLY, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_TIMING_PA_TRIGGER */ 
  { RFC_MSM_TIMING_PAPM_OFF_TX_RX_TX_CTL    , RFC_CONFIG_ONLY, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_TIMING_PAPM_OFF_TX_RX_TX_CTL */ 
  { RFC_MSM_TIMING_PAPM_MULTISLOT_CTL       , RFC_CONFIG_ONLY, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_TIMING_PAPM_MULTISLOT_CTL */ 
  { RFC_MSM_TIMING_PAPM_TX_TX_CTL           , RFC_CONFIG_ONLY, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_TIMING_PAPM_TX_TX_CTL */ 
  { RFC_MSM_UTIMER_SYNC                     , RFC_LOW, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_UTIMER_SYNC */ 
  { RFC_MSM_GPDATA0_0                       , RFC_CONFIG_ONLY, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_GPDATA0_0 */ 
  { RFC_MSM_RFFE1_CLK                       , RFC_CONFIG_ONLY, DAL_GPIO_PULL_DOWN, DAL_GPIO_6MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_RFFE1_CLK */ 
  { RFC_MSM_RFFE1_DATA                      , RFC_CONFIG_ONLY, DAL_GPIO_PULL_DOWN, DAL_GPIO_6MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_RFFE1_DATA */ 
  { RFC_MSM_RFFE2_CLK                       , RFC_CONFIG_ONLY, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_RFFE2_CLK */ 
  { RFC_MSM_RFFE2_DATA                      , RFC_CONFIG_ONLY, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_RFFE2_DATA */ 
  { RFC_MSM_RFFE3_CLK                       , RFC_CONFIG_ONLY, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_RFFE3_CLK */ 
  { RFC_MSM_RFFE3_DATA                      , RFC_CONFIG_ONLY, DAL_GPIO_PULL_DOWN, DAL_GPIO_2MA, (DALGpioIdType)NULL }, /* RFC_SDR105_RIOT_9X05_RFFE3_DATA */ 
  { (rfc_msm_signal_type)RFC_SIG_LIST_END   , (rfc_logic_type)RFC_ENCODED_REVISION, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, (DALGpioIdType)NULL } /* LAST SIG INDICATOR */ 
};


rfc_common_data* rfc_sdr105_riot_9x05_cmn_ag::get_instance(rf_hw_type rf_hw)
{
  if (rfc_common_data_ptr == NULL)
  {
    rfc_common_data_ptr = (rfc_common_data *)new rfc_sdr105_riot_9x05_cmn_ag(rf_hw);
  }
  return( (rfc_common_data *)rfc_common_data_ptr);
}

//constructor
rfc_sdr105_riot_9x05_cmn_ag::rfc_sdr105_riot_9x05_cmn_ag(rf_hw_type rf_hw)
  :rfc_common_data(rf_hw)
{
}


uint32 rfc_sdr105_riot_9x05_cmn_ag::sig_info_table_get(rfc_signal_info_type **rfc_info_table)
{
  if (NULL == rfc_info_table)
  {
    return 0;
  }

  *rfc_info_table = &rfc_sdr105_riot_9x05_sig_info[0];

  #ifdef FEATURE_LTE
  // Create LTE RFC AG Data Object
  rfc_lte_data *rfc_lte_data = rfc_sdr105_riot_9x05_lte_ag::get_instance(); 
  if (rfc_lte_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC LTE Data Object is NOT Created", 0);
    return 0;
  }
  #endif /* FEATURE_LTE */

  #ifdef FEATURE_CGPS
  // Create GNSS RFC AG Data Object
  rfc_gnss_data *rfc_gnss_data = rfc_sdr105_riot_9x05_gnss_ag::get_instance(); 
  if (rfc_gnss_data == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RFC GNSS Data Object is NOT Created", 0);
    return 0;
  }
  #endif /* FEATURE_GNSS */

  return RFC_SDR105_RIOT_9X05_SIG_NUM;
}

rfc_phy_device_info_type* rfc_sdr105_riot_9x05_cmn_ag::get_phy_device_cfg( void )
{
  return (&rfc_sdr105_riot_9x05_phy_devices_list[0]);
}

rfc_logical_device_info_type* rfc_sdr105_riot_9x05_cmn_ag::get_logical_device_cfg( void )
{
  return (&rfc_sdr105_riot_9x05_logical_devices_list[0]);
}


rfm_devices_configuration_type rfc_sdr105_riot_9x05_logical_device_properties = 
{
  538, /* HWID */
  { /* Mode Concurrencies: Bit0 represents Rx concurrency and Bit1 represents Tx concurrency */
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,    },
  },
  { /* device_support */ 
    { /* Logical Device 0 */ 
      /* bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          ( ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND1 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND2 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND3 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND4 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND5 - 64) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND8 - 64) ) 
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          ( ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND12 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND13 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND14 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND18 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND19 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND20 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND25 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND26 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND27 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND28 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND66 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND71 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND85 - 128) ) | ( (uint64)1 << (SYS_BAND_LTE_EUTRAN_BAND86 - 128) ) 
          ),  /* End Bit mask element 2 */
        },
      },
      /* ca1_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* dr_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* ho_rxd_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      ( RFM_DEVICE_RX_SUPPORTED | RFM_DEVICE_TX_SUPPORTED),
      RFM_DEVICE_1, /* Associated Slave Device for RFM_DEVICE_0 */
    }, /* End Logical Device 0 */ 
    { /* Logical Device 1 */ 
      /* bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* ca1_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* dr_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* ho_rxd_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      ( 0),
      RFM_INVALID_DEVICE, /* Associated Slave Device for RFM_DEVICE_1 */
    }, /* End Logical Device 1 */ 
    { /* Logical Device 2 */ 
      /* bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* ca1_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* dr_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* ho_rxd_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      ( 0),
      RFM_DEVICE_3, /* Associated Slave Device for RFM_DEVICE_2 */
    }, /* End Logical Device 2 */ 
    { /* Logical Device 3 */ 
      /* bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* ca1_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* dr_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* ho_rxd_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      ( 0),
      RFM_INVALID_DEVICE, /* Associated Slave Device for RFM_DEVICE_3 */
    }, /* End Logical Device 3 */ 
    { /* Logical Device 4 */ 
      /* bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* ca1_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* dr_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* ho_rxd_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      ( 0),
      RFM_INVALID_DEVICE, /* Associated Slave Device for RFM_DEVICE_4 */
    }, /* End Logical Device 4 */ 
    { /* Logical Device 5 */ 
      /* bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* ca1_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* dr_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* ho_rxd_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      ( 0),
      RFM_INVALID_DEVICE, /* Associated Slave Device for RFM_DEVICE_5 */
    }, /* End Logical Device 5 */ 
    { /* Logical Device 6 */ 
      /* bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* ca1_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* dr_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* ho_rxd_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      ( 0),
      RFM_INVALID_DEVICE, /* Associated Slave Device for RFM_DEVICE_6 */
    }, /* End Logical Device 6 */ 
    { /* Logical Device 7 */ 
      /* bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* ca1_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* dr_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      /* ho_rxd_bands_supported */ 
      {
        {
          /* Bit mask element 0 */ 
          (
            0
          ),  /* End Bit mask element 0 */
          /* Bit mask element 1 */ 
          (
            0
          ),  /* End Bit mask element 1 */
          /* Bit mask element 2 */ 
          (
            0
          ),  /* End Bit mask element 2 */
        },
      },
      ( 0),
      RFM_INVALID_DEVICE, /* Associated Slave Device for RFM_DEVICE_7 */
    }, /* End Logical Device 7 */ 
  },
  {
    0, /* num_concurrency_restrictions */ 
    NULL, /* No Concurrency Restrictions */ 
  },
  {
    0, /* num_3mode_entries */ 
    NULL, /* No 3Mode Support */ 
  },
  FALSE, /* gps_has_independent_receive_chain */ 
  FALSE, /* antenna_swap_supported */ 
  0, /* concurrency_features */ 
};

boolean rfc_sdr105_riot_9x05_cmn_ag::get_logical_path_config(rfm_devices_configuration_type* dev_cfg)
{
  if ( dev_cfg == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfm_get_devices_configuration: Invalid Container", 0);
    return FALSE;
  }

  memscpy(dev_cfg,
          sizeof(rfm_devices_configuration_type),
          &rfc_sdr105_riot_9x05_logical_device_properties,
          sizeof(rfm_devices_configuration_type));

  return TRUE;
}



rfc_cmn_properties_type rfc_sdr105_riot_9x05_cmn_properties = 
{
  RFC_ENCODED_REVISION,   
  RFC_ATTENUATION_STATE_MID,
};

boolean rfc_sdr105_riot_9x05_cmn_ag::get_cmn_properties(rfc_cmn_properties_type **ptr)
{
  if (NULL==ptr)
  {
    return FALSE;
  }

  *ptr = &rfc_sdr105_riot_9x05_cmn_properties;
  return TRUE;
}
