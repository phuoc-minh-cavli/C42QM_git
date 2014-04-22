#ifndef SDR105_PHYSICAL_DEVICE_H
#define SDR105_PHYSICAL_DEVICE_H
/*!
  @file
  sdr105_physical_device.h 

  @brief
  Software abstraction of a SDR105 physical device.
*/

/*===========================================================================

  Copyright (c) 2016 - 2018 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rfdevice_sdr105.mpss/1.10/api/sdr105_physical_device.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/16/18   up      Added a member variable hdet_instance_ptr
02/22/18   up      Modified for SDR105, FR 46604: NBPDET support on SDR105 
                   Added a member function create_sdr105_hdet_device
02/06/18   up      Mofified for SDR105, FR 46405: SDR105 TRX (IOTRF) 
                   renamed rfdevice_rxtx_common_class to rfdevice_trx_common_intf_class
04/18/17   ka      Remove precs 
03/12/16   ka      Add support for SDR105 precs and latest AG 
07/14/16   hm     Merge changes from WTR6955 component
06/30 16   ssr     Changes to remove duplicate function declarations
04/27/16   hm      Port mapping with sig path dimension(RX + CDMA Tx)
03/28/16   hm      v1/v2 co-existence and V2 device creation
02/10/16   ash     Added support for TRX Therm
09/03/15   snk     Changed rfc_phy_device_info_type to const 
08/14/15   snk     Updated create_sdr105_devices method
08/07/15   shb     Added sdr105_physical_device class 
06/05/15   aks     Added API: sdr105_create_device
05/26/15   aks     Initial revision
===========================================================================*/

#include "rfdevice_physical_device.h"
#include "rfdevice_trx_common_intf_class.h"
#include "rfdevice_trx_therm_class.h"
#include "sdr105_typedef_ag.h"


class sdr105_physical_device : public rfdevice_physical_device
{
public:
  sdr105_physical_device(const rfc_phy_device_info_type *sdr105_cfg_info);

  ~sdr105_physical_device();

  virtual bool load_self_cal(const char* str);
  
  virtual bool perform_self_cal(const char* str);
  
#ifdef TEST_FRAMEWORK
#error code not present
#else
rfdevice_logical_component* get_component(
  rfc_logical_device_info_type *logical_device_cfg);
#endif

private:

  boolean create_sdr105_devices
  (
    const rfc_logical_device_info_type&        logical_device_cfg,
    const rfc_phy_device_info_type&            phy_device_cfg
  );

  //Create the TRX Therm device
  boolean create_sdr105_therm_device
  (
    const rfc_logical_device_info_type&        logical_device_cfg,
    const rfc_phy_device_info_type&            phy_device_cfg
  );
  
  //Create SDR105 tech objects
  boolean create_sdr105v1_devices
  (
    const rfc_logical_device_info_type& logical_device_cfg,
    const rfc_phy_device_info_type& phy_device_cfg
  );

  rfdevice_class* create_sdr105_hdet_device ();

  /* Pointer to the TRx Common Device */
  rfdevice_trx_common_intf_class *transceiver_ptr;

  /* Pointer to the RFC AG table containing the physical
     device info */
  const rfc_phy_device_info_type *phy_device_cfg;

  // Pointer to the TRX Therm Device
  rfdevice_trx_therm_class  *therm_ptr;

  //Pointer to the HDET Device
  rfdevice_class *hdet_instance_ptr;
  
  /* Private copy constructor and assignment operator prevent copying
     sdr105_rffe_physical_device objects */
  sdr105_physical_device& operator=(const sdr105_physical_device& dev)
  {
    return *this;
  }

  sdr105_physical_device(const sdr105_physical_device& src){}
};

#endif /* SDR105_PHYSICAL_DEVICE_H */

