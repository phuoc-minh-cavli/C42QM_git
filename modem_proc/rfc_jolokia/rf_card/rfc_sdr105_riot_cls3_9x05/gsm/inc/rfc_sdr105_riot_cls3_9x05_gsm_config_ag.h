
#ifndef RFC_SDR105_RIOT_CLS3_9X05_GSM_CONFIG_AG
#define RFC_SDR105_RIOT_CLS3_9X05_GSM_CONFIG_AG


#ifdef __cplusplus
extern "C" {
#endif

/*
WARNING: This file is auto-generated.

Generated using: rfc_autogen.exe
Generated from:  V5.9.415 of RFC_HWSWCD.xlsm
*/

/*=============================================================================

          R F C     A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the RF Card.

Copyright (c) 2017 Qualcomm Technologies Incorporated.  All Rights Reserved.

$Header: //components/rel/rfc_jolokia.mpss/2.10/rf_card/rfc_sdr105_riot_cls3_9x05/gsm/inc/rfc_sdr105_riot_cls3_9x05_gsm_config_ag.h#4 $ 


=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfc_msm_typedef.h" 
#include "rfc_common.h" 
#include "rfc_gsm_data.h" 



class rfc_sdr105_riot_cls3_9x05_gsm_ag:public rfc_gsm_data
{
public:
  static rfc_gsm_data * get_instance();
    boolean sig_cfg_data_get( rfc_cfg_params_type *cfg, rfc_sig_cfg_type **ptr );
    boolean devices_cfg_data_get( rfc_cfg_params_type *cfg, rfc_device_info_type **ptr );
    boolean band_split_cfg_data_get( rfc_cfg_params_type *cfg, rfc_band_split_info_type **ptr );
    boolean get_gsm_properties(rfc_gsm_properties_type **ptr);

protected:
  rfc_sdr105_riot_cls3_9x05_gsm_ag(void);  /*  Constructor  */
};


#ifdef __cplusplus
}
#endif



#endif


