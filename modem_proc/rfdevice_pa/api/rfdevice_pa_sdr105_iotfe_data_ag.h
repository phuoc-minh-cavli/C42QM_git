
#ifndef RFDEVICE_PA_SDR105_IOTFE_DATA_AG
#define RFDEVICE_PA_SDR105_IOTFE_DATA_AG


#ifdef __cplusplus
extern "C" {
#endif
/*
WARNING: This file is auto-generated.

Generated using: pa_autogen.pl
Generated from:  2.8 of RFDevice_PA_IOTFE.xlsm
*/

/*=============================================================================

          R F C     A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the RF Card.

Copyright (c) 2018-2023 by Qualcomm Technologies, Inc.  All Rights Reserved.

$Header: //components/rel/rfdevice_pa.mpss/1.30/api/rfdevice_pa_sdr105_iotfe_data_ag.h#2 $ 


=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "comdef.h"

#include "rfdevice_pa_data.h"

class rfdevice_pa_sdr105_iotfe_data_ag:public rfdevice_pa_data
{
public:
    static rfdevice_pa_data * get_instance();
    boolean settings_data_get( rfdevice_pa_cfg_params_type *cfg, 
                               rfdevice_pa_reg_settings_type *settings);                               
    boolean sequence_data_get( rfdevice_pa_cfg_params_type *cfg, 
                               rfdevice_pa_cmd_seq_type *cmd_seq);
    boolean device_info_get( rfdevice_pa_info_type *pa_info );
    boolean get_temperature_lut( int16 **lut_addr, uint8 *lut_size );     
       boolean get_port(rfcom_lte_cat_type category, rfcom_lte_band_type band, uint8 *port);
       boolean timing_info_get( rfdevice_pa_timing_info_type *pa_timing_info ); 
       boolean execute_therm_sequence(uint8 rffe_bus, uint8 usid); 
    ~rfdevice_pa_sdr105_iotfe_data_ag(void);  /*  Destructor  */
protected:
  rfdevice_pa_sdr105_iotfe_data_ag(void);  /*  Constructor  */

private:
  static rfdevice_pa_data *rfdevice_pa_sdr105_iotfe_data_ptr;

};


#ifdef __cplusplus
}
#endif



#endif

