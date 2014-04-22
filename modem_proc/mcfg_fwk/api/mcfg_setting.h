/*===========================================================================
                        mcfg_setting.h

DESCRIPTION

   MCFG setting for the MCFG module
 
Copyright (c) 2012-2015 by QUALCOMM Technologies Incorporated.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/mcfg_fwk.mpss/7.10.1.1/api/mcfg_setting.h#1 $ $DateTime: 2020/02/19 23:07:19 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
11/2/15      cys   Create

===========================================================================*/

#ifndef MCFG_SETTING_H
#define MCFG_SETTING_H

#include "comdef.h"
#include "mcfg_common.h"
#include "persistent_device_configuration_v01.h"

/*===========================================================================

  FUNCTION mcfg_set_feature

  DESCRIPTION
    Process the change request of feature setting for specified slot index

  DEPENDENCIES
    None

  PARAMETERS
    feature      [in]  feature setting for specified slot index

  RETURN VALUE
    mcfg_error_e_type mcfg error code

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_set_feature
(
  pdc_set_feature_req_msg_v01 *feature
);

/*===========================================================================

  FUNCTION mcfg_set_feature

  DESCRIPTION
    Queries the change request of feature setting for specified slot index

  DEPENDENCIES
    None

  PARAMETERS
    sloti        [in]  specified slot index
    feature      [in]  pointer of feature settings

  RETURN VALUE
    mcfg_error_e_type mcfg error code

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_get_feature
(
  mcfg_slot_index_type_e_type sloti, 
  pdc_get_feature_ind_msg_v01 *feature
);

/*===========================================================================

FUNCTION mcfg_setting_space_limit

DESCRIPTION
  return EFS space limit allowed for MCFG in kilobytes, 0 means not configured
  
DEPENDENCIES
  None.

RETURN VALUE
  uint16

SIDE EFFECTS

===========================================================================*/
uint16 mcfg_setting_space_limit
(
  void
);

/*===========================================================================

FUNCTION mcfg_setting_remote_discover

DESCRIPTION
  return remote auto discover feature is enabled or not
  
DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS

===========================================================================*/
boolean mcfg_setting_remote_discover
(
  void
);

#endif /* MCFG_SETTING_H */
