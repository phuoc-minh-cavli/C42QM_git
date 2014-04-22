#ifndef QAT3516_PHYSICAL_DEVICE_H
#define QAT3516_PHYSICAL_DEVICE_H

/*===========================================================================

Copyright (c) 2016 by QUALCOMM Technologies, Inc.  All Rights 3516.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rfdevice_qat3516.mpss/1.20/api/qat3516_physical_device.h#1 $
  $DateTime: 2020/04/16 12:14:54 $
  $Author: pwbldsvc $

when         who   what, where, why
--------   ---   --------------------------------------------------------------- 
07/17/17   dh    Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rfdevice_antenna_tuner.h"
#include "rfdevice_qtuner_api.h"
#include "rfc_common.h"

#include "rfdevice_physical_device.h"


class qat3516_physical_device : public rfdevice_physical_device
{
public:

  virtual rfdevice_logical_component* get_component(rfc_logical_device_info_type *logical_device_cfg);

  qat3516_physical_device* qat3516_physical_device_p;

  rfc_phy_device_info_type* phy_device_cfg;

  qat3516_physical_device(rfc_phy_device_info_type* cfg_req,
  	                      boolean ext_cmd = TRUE );

  ~qat3516_physical_device();

  virtual bool load_self_cal(const char* str);

  virtual bool perform_self_cal(const char* str);

private:
  /* Device config */
  rfc_device_cfg_info_type* cfg;
  boolean extended_cmd;

  /* QTUNER */
  void create_tuner_object( rfc_logical_device_info_type *logical_device_info ); 

  rfdevice_antenna_tuner* qat3516_qtuner_obj_ptr;
  
};
#endif