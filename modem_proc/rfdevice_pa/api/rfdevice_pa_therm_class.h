#ifndef RFDEVICE_PA_THERM_CLASS_H
#define RFDEVICE_PA_THERM_CLASS_H
/*!
  @file rfdevice_pa_therm_class.h 

  @brief
  This file defines the RF device transceiver therm interface class and its APIs

*/

/*===========================================================================

  Copyright (c) 2019 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/


/*===========================================================================


                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
-------------------------------------------------------------------------------- 
07/11/19   dh      IOTFE therm mitigation fixes
02/25/19   dh      Initial version

============================================================================*/
 
#ifdef __cplusplus
 extern "C"
 {
#endif


#include "rfc_common.h"
#include "rfdevice_therm.h"
#include "rfdevice_physical_device.h"
#include "rfdevice_class.h"
#include "rfdevice_type_defs.h"
#include "rfdevice_pa_common.h"

rfdevice_logical_component* create_gen_pa_therm_device_object(rfdevice_physical_device *rfdevice_physical_ptr,rfc_logical_device_info_type *logical_device_cfg);


class rfdevice_pa_therm_common: public rfdevice_therm
{

public:

  /*===========================================================================*/
  /*                      Constructor/Destructor                               */
  /*===========================================================================*/
  rfdevice_pa_therm_common
  (
	   rfdevice_pa_common*                        pa_device_ptr,
	   rfdevice_physical_device*                  rfdevice_physical_ptr,
	   rfc_phy_device_info_type*            phy_device_cfg,
	   rfc_logical_device_info_type*        logical_device_cfg

  );
    
  ~rfdevice_pa_therm_common();

  /*===========================================================================*/
  /*                      Member methods                                      */
  /*===========================================================================*/

boolean setup_therm (rfdevice_therm_setup_cfg* therm_setup_cfg_p);
boolean clean_up_therm (rfdevice_therm_clean_up_cfg* therm_clean_up_cfg_p);
boolean read_therm_immediate (rfdevice_read_imm_cfg* therm_read_cfg_p);
boolean execute_pa_on_therm_sequence ();
boolean read_therm_script (rfdevice_read_script_cfg* therm_read_cfg_p);
boolean process_therm_read_script (rfdevice_process_read_script_cfg* therm_read_cfg_p);


protected:
	
 uint8 rffe_bus;  /*! RFFE channel/bus information */ 
 uint8 default_usid; /*! RFFE Default Unique Slave ID */
 uint8 assigned_usid;  /*! RFFE Programmable Unique Slave ID*/

 rfdevice_physical_device* parent_physical_device_ptr;
private:    
  /*----------------------------------------------------------------------------*/
  /*                              Data members                                  */
  /*----------------------------------------------------------------------------*/

  /*! Ptr to the associated Cmn device object */
  rfdevice_pa_common* associated_pa_device_ptr;
};


#ifdef __cplusplus
}/* extern "C"*/
#endif


#endif /* RFDEVICE_PA_THERM_CLASS_H */
