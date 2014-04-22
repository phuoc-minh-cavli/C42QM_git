
#ifndef RFC_SDR105_RIOT_9X05_CMN_AG
#define RFC_SDR105_RIOT_9X05_CMN_AG


#ifdef __cplusplus
extern "C" {
#endif

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

$Header: //components/rel/rfc_jolokia.mpss/2.10/rf_card/rfc_sdr105_riot_9x05/common/inc/rfc_sdr105_riot_9x05_cmn_ag.h#2 $ 


=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfc_common.h" 



typedef enum
{
  RFC_SDR105_RIOT_9X05_TIMING_PA_CTL,
  RFC_SDR105_RIOT_9X05_TIMING_PA_RANGE,
  RFC_SDR105_RIOT_9X05_TIMING_ASM_CTL,
  RFC_SDR105_RIOT_9X05_TIMING_TUNER_CTL,
  RFC_SDR105_RIOT_9X05_TIMING_PAPM_CTL,
  RFC_SDR105_RIOT_9X05_TIMING_TX_TX_RF_ON0,
  RFC_SDR105_RIOT_9X05_TIMING_TX_RX_RF_ON0,
  RFC_SDR105_RIOT_9X05_TIMING_ASM_TRIGGER,
  RFC_SDR105_RIOT_9X05_TIMING_PAPM_TX_TX_TRIGGER,
  RFC_SDR105_RIOT_9X05_TIMING_PAPM_OFF_TX_RX_TX_TRIGGER,
  RFC_SDR105_RIOT_9X05_TIMING_PA_TRIGGER,
  RFC_SDR105_RIOT_9X05_TIMING_PAPM_OFF_TX_RX_TX_CTL,
  RFC_SDR105_RIOT_9X05_TIMING_PAPM_MULTISLOT_CTL,
  RFC_SDR105_RIOT_9X05_TIMING_PAPM_TX_TX_CTL,
  RFC_SDR105_RIOT_9X05_UTIMER_SYNC,
  RFC_SDR105_RIOT_9X05_GPDATA0_0,
  RFC_SDR105_RIOT_9X05_RFFE1_CLK,
  RFC_SDR105_RIOT_9X05_RFFE1_DATA,
  RFC_SDR105_RIOT_9X05_RFFE2_CLK,
  RFC_SDR105_RIOT_9X05_RFFE2_DATA,
  RFC_SDR105_RIOT_9X05_RFFE3_CLK,
  RFC_SDR105_RIOT_9X05_RFFE3_DATA,
  RFC_SDR105_RIOT_9X05_SIG_NUM,
  RFC_SDR105_RIOT_9X05_SIG_INVALID,
}sdr105_riot_9x05_sig_type;


#ifdef __cplusplus

#include "rfc_common_data.h"

class rfc_sdr105_riot_9x05_cmn_ag:public rfc_common_data
{
  public:
    uint32 sig_info_table_get(rfc_signal_info_type **rfc_info_table);
    rfc_phy_device_info_type* get_phy_device_cfg( void );
    rfc_logical_device_info_type* get_logical_device_cfg( void );
    boolean get_logical_path_config(rfm_devices_configuration_type* dev_cfg);
    boolean get_cmn_properties(rfc_cmn_properties_type **ptr);
    static rfc_common_data * get_instance(rf_hw_type rf_hw);

  protected:
    rfc_sdr105_riot_9x05_cmn_ag(rf_hw_type rf_hw);
};

#endif   /*  __cplusplus  */


#ifdef __cplusplus
}
#endif



#endif


