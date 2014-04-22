
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

$Header: //components/rel/rfc_jolokia.mpss/2.10/rf_card/rfc_sdr105_riot_9x05/gnss/src/rfc_sdr105_riot_9x05_gnss_config_data_ag.c#2 $ 


=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfc_sdr105_riot_9x05_cmn_ag.h" 
#include "rfc_common.h" 
#include "rfcom.h" 
#include "sdr105_typedef_ag.h" 
#include "rfdevice_qtuner_api.h" 



rfc_device_info_type rf_card_sdr105_riot_9x05_gnss_device_info = 
{
  RFC_ENCODED_REVISION, 
  0 /* Warning: Not Specified */,   /* Modem Chain */
  0 /* Warning: Not Specified */,   /* NV Container */
  RFC_INVALID_PARAM /* Warning: Not Specified */,   /* Antenna */
  2,  /* NUM_DEVICES_TO_CONFIGURE */
  {
    {
      RFDEVICE_TRANSCEIVER,
      SDR105,  /* NAME */
      0,  /* DEVICE_MODULE_TYPE_INSTANCE */
      0,  /* PHY_PATH_NUM */
      {
        0 /*Warning: Not specified*/,  /* INTF_REV */
        (int)SDR105_GNSS_RFC_PORT_COMMON_INVALID,  /* PORT */
        ( RFDEVICE_RX_GAIN_STATE_MAPPING_INVALID ),  /* RF_ASIC_BAND_AGC_LUT_MAPPING */
        FALSE,  /* TXAGC_LUT */
        0,  /* FBRX_ATTN_STATE */
        0,  /* Array Filler */
      },
    },
    {
      RFDEVICE_ASM,
      GEN_ASM /* IOTFE_ASM */,  /* NAME */
      0,  /* DEVICE_MODULE_TYPE_INSTANCE */
      0 /*Warning: Not specified*/,  /* PHY_PATH_NUM */
      {
        0  /* Orig setting:  */,  /* INTF_REV */
        (0X217 << 22)/*mfg_id*/ | (0xC7 << 14)/*prd_id*/ | (3)/*port_num*/,  /* PORT_NUM */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
        0,  /* Array Filler */
      },
    },
  },
};


rfc_sig_info_type rf_card_sdr105_riot_9x05_gnss_sig_cfg = 
{
  RFC_ENCODED_REVISION, 
  {
    { (int)RFC_SIG_LIST_END,   { RFC_LOW, 0 }, {RFC_LOW, 0 } }
  },
};


