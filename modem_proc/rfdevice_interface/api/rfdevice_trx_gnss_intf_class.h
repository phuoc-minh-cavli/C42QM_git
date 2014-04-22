
#ifndef RFDEVICE_TRX_GNSS_INTF_CLASS
#define RFDEVICE_TRX_GNSS_INTF_CLASS
/*!
  @file rfdevice_trx_gnss_intf_class.h 

  @brief
  This file defines the RF device transceiver GNSS interface class and its APIs

*/

/*===========================================================================

  Copyright (c) 2015 - 2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //components/rel/rfdevice_interface.mpss/1.40/api/rfdevice_trx_gnss_intf_class.h#1 $

when       who     what, where, why
--------------------------------------------------------------------------------
12/21/16   xf     gnss RF cfg mode and fine gain cal support
03/18/16   xf     gnss wxe implementation
09/24/15   xf     Add gnss specific external interface
09/10/15   hs     Initial revision.

===========================================================================*/

#include "rfdevice_type_defs.h"
//#include "rfc_device_info_types.h"   //RB: Commented as JO PL does not have this header file.
//#include "rfc_common.h"

#ifdef __cplusplus
#include "rfdevice_class.h"
//#include "rfdevice_trx_common_intf_class.h"  //RB: to see if compilation issue could be resolved.

class rfdevice_trx_gnss_intf_class:public rfdevice_class
{
  public:
  rfdevice_trx_gnss_intf_class()
  {
  
  }
  rfdevice_trx_gnss_intf_class(uint16 manufacturer_id, uint16 product_id, uint16 product_rev)
  : rfdevice_class(manufacturer_id, product_id, product_rev)
  {
  
  }
  virtual ~rfdevice_trx_gnss_intf_class()
  {

  }
 
/*===========================================================================*/
/*                 Member functions- Pure Virtual                            */
/*===========================================================================*/

/*! 
  @brief
  This function will initialize GNSS RF device
  
  @param rfgnss_mode_config_type rf_mode
         LNA and saw filter mode config
  
  @return
  boolean Success/Failure 
*/
virtual boolean init_dev(rfgnss_mode_config_type rf_mode) = 0;

/*! 
  @brief
  This function will set PGA gain
  
  @param gain_hwu
  HW unit need to be written into PGA register
  
  @return
  boolean Success/Failure 
*/
virtual boolean set_pga(uint16 gain_hwu) = 0;

/*! 
  @brief
  This function will dump GNSS RF key registers for debug purpose
  
  @param
 
  @return
  boolean Success/Failure 
*/
virtual boolean reg_dump(uint32 *data) = 0;

/*! 
  @brief
  This function will congfiure LTE B13 2H mode setting
  
  @param boolean cfg
         True - enter mode
         False - exit mode
  @param rfgnss_mode_config_type rf_mode
         LNA and saw filter mode config
 
  @return
  boolean Success/Failure 
*/
virtual boolean configure_lte_b13_2h_mode(boolean cfg, rfgnss_mode_config_type rf_mode)= 0;

/*! 
  @brief
  This function will turn GNSS RF chip off
  
  @param 
  
  @return
  boolean Success/Failure 
*/
virtual boolean chip_off(void) = 0;

/*! 
  @brief
  This function will enable GPS only
  
  @param 
  
  @return
  boolean Success/Failure 
*/
virtual boolean only_enable(void) = 0;

/*! 
  @brief
  This function will set Rx fine gain code
  
  @param fine_gain_hwu
  HW unit need to be written into Rx fine gain register
  
  @return
  boolean Success/Failure 
*/
virtual boolean set_rx_fine_gain(uint16 fine_gain_hwu) = 0;
};

#else 

struct rfdevice_trx_gnss_intf_struct;
typedef struct rfdevice_trx_gnss_intf_struct rfdevice_trx_gnss_intf_class;

#endif  /* #ifdef __cplusplus */

#endif /* #ifndef RFDEVICE_TRX_GNSS_INTF_CLASS */
