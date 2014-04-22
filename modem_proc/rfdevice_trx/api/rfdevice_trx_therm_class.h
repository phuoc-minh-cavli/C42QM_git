#ifndef RFDEVICE_TRX_THERM_CLASS_H
#define RFDEVICE_TRX_THERM_CLASS_H
/*!
  @file rfdevice_trx_therm_class.h 

  @brief
  This file defines the RF device transceiver therm interface class and its APIs

*/

/*===========================================================================

  Copyright (c) 2016 - 2018 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/rfdevice_trx.mpss/1.20/api/rfdevice_trx_therm_class.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------- 
02/06/18   up    Mofified for SDR105, FR 46405: SDR105 TRX (IOTRF).
                 Renamed rfdevice_rxtx_common_class to  
                 rfdevice_trx_common_intf_class, Modified therm class APIs
10/25/16  hm     API to indicate dedicated ADC support
09/27/16  hm     Script based therm interface
01/27/16  ash    Initial version

============================================================================*/

#include "rfdevice_trx_common_intf_class.h"
#include "rfc_common.h"
#include "rfdevice_therm.h"
#include "rfdevice_physical_device.h"
#include "rfdevice_class.h"
#include "rfdevice_type_defs.h"
#include "rfdevice_trx_typedefs.h"


class rfdevice_trx_therm_class: public rfdevice_therm
{

public:

  /*===========================================================================*/
  /*                      Constructor/Destructor                               */
  /*===========================================================================*/
  rfdevice_trx_therm_class
  (
    rfdevice_trx_common_intf_class*                        trx_cmn_ptr,
    const rfc_logical_device_info_type&                    logical_device_cfg, 
    const rfc_phy_device_info_type&                        phy_device_cfg
  );
    
  virtual ~rfdevice_trx_therm_class();

  /*===========================================================================*/
  /*                      Member methods                                      */
  /*===========================================================================*/

boolean setup_therm (rfdevice_therm_setup_cfg* therm_setup_cfg_p);
boolean clean_up_therm (rfdevice_therm_clean_up_cfg* therm_clean_up_cfg_p);
boolean read_therm_immediate (rfdevice_read_imm_cfg* therm_read_cfg_p);
boolean read_therm_script (rfdevice_read_script_cfg* therm_read_cfg_p);
boolean process_therm_read_script (rfdevice_process_read_script_cfg* therm_read_cfg_p);


boolean update_transceiver_ptr(rfdevice_trx_common_intf_class *transceiver_ptr);


protected:

private:    
  /*----------------------------------------------------------------------------*/
  /*                              Data members                                  */
  /*----------------------------------------------------------------------------*/

  /*! Ptr to the associated Cmn device object */
  rfdevice_trx_common_intf_class* trx_cmn_dev_ptr;
};


#endif /* RFDEVICE_TRX_THERM_CLASS_H */
