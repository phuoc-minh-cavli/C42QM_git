#ifndef RFDEVICE_PA_CFG_H
#define RFDEVICE_PA_CFG_H
/*!
   @file
   rfdevice_pa_cfg.h

   @brief


*/

/*==============================================================================

Copyright (c) 2012-2018 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rfdevice_interface.mpss/1.40/api/rfdevice_pa_cfg.h#1 $

when       who   what, where, why
--------   ---   ---------------------------------------------------------------
07/13/18   dh    Added lte_cat_type param
03/17/14   sn    Initial version
==============================================================================*/

/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "rfcom.h"
#include "rfdevice_cmn_type_defs.h"

#ifdef __cplusplus
class rfdevice_pa_cfg: public rfa
{
public:

    rfdevice_pa_cfg();

	rfdevice_pa_cfg(rfm_device_enum_type rfm_device, rfcom_mode_enum_type mode, rfcom_band_type_u band);
	
    virtual ~rfdevice_pa_cfg();

    rfm_device_enum_type rfm_device;
    rfcom_mode_enum_type mode;
    rfcom_band_type_u band;
    rfdevice_pa_bw_enum_type bw;
    boolean on_off;
    uint16 pa_gain_range;
    boolean is_mod_type_gmsk;
    boolean use_default_icq_pdm;
    uint16* pa_icq_array_p;
    uint16  pa_icq_array_count;
    rfdevice_pa_trigger_enum_type  trigger_type;
    rfcom_lte_cat_type lte_cat;

};

#endif
#endif /* RFDEVICE_PA_CFG_H */


