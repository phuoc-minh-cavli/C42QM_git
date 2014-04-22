#ifndef RFDEVICE_PHYSICAL_DEVICE_H
#define RFDEVICE_PHYSICAL_DEVICE_H
/*!
  @file
  rfdevice_physical_device.h 

  @brief
  Common interface to an instance of a physical chip in the system.
*/

/*===========================================================================

  Copyright (c) 2012 - 2018 Qualcomm Technologies Incorporated. All Rights Reserved

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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rfdevice_interface.mpss/1.40/api/rfdevice_physical_device.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/25/18   vp      Added support for RFFE device get register address list
10/01/14   dbc     Remove XSW APIs 
08/14/14   dbc     Added support for physical device tracking of XSW position
07/28/14   vv      Added mutex protection support for all physical devices
06/19/14   vv      Added support for physical device based sleep/wakeup voting
05/12/14   vv      Update physical device interface
04/08/13   bmg     Changed self-calibration interface to character string
11/06/12   bmg     Created

===========================================================================*/

#include "comdef.h"
#include "rfdevice_logical_component.h"
#include "rfc_common.h"
#include "modem_mem.h"

#ifdef __cplusplus
extern "C" {
#include "rfcommon_locks.h"
}
#endif

#define RFDEVICE_PHY_DEV_REG_LIST_SIZE 50
#define RFDEVICE_PHY_DEV_REG_FLAG_LIST_SIZE 0xFF
#define RFDEVICE_REG_ADDR_INVALID 0xFFFF

class rfdevice_physical_device
{
public:

  rfdevice_physical_device();
  virtual rfdevice_logical_component* get_component(rfdevice_type_enum_type type, int instance);

  /*Will make the below function pure virtual once we have physical device creation enabled for all devices*/
  virtual rfdevice_logical_component* get_component(rfc_logical_device_info_type* device_cfg);

  virtual bool perform_self_cal(const char* str) = 0;
  virtual bool load_self_cal(const char* str) = 0;

  virtual ~rfdevice_physical_device();

 /*To support physical device based sleep/wakeup voting*/
 virtual bool vote_sleep_wakeup(rfc_logical_device_info_type *logical_device_cfg, bool sleep_wakeup);

 /* To get the status of the RFFE register address list */
  virtual rfdevice_reg_addr_return_enum_type rfdevice_phy_dev_get_reg_list_status();

  /* To get the valid size of the RFFE register address list */
  virtual uint32 rfdevice_phy_dev_get_reg_list_size();

  /* To get the RFFE register address dump that is already saved into physical device class */
  virtual void rfdevice_phy_dev_get_reg_list(uint16* buffer_ptr, uint16 size);

  /* Free the register address buffer memory after using */
  virtual void rfdevice_phy_dev_free_reg_addr_buffer();

 /* Initialise the RFFE register address list relevant variables, internal to device, techs don't need to call it */
  virtual void initialize_reg_addr_list();
 
 /* Record the status of the register address dump */
  rfdevice_reg_addr_return_enum_type reg_addr_dump_status;
 
 /* Store the RFFE register address */
  uint16* rffe_reg_addr_list;
 
 /* Store the current index in reg_addr_list */
  uint16 reg_addr_list_index;
  
  /* Record the occurrence of the register address */
  boolean* reg_addr_occur_table;

  /*! rfdevice_physical global mutex - one per instance.Can be used by all physical devices to protect critical section code. 
 Should be initialized during device configuration before any other usage */
 rf_lock_data_type rfdevice_physical_critical_sect;
};

#endif /* RFDEVICE_PHYSICAL_DEVICE_H */

