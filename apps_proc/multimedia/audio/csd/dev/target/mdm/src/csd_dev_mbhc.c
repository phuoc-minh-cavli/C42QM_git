/*==============================================================================
  @file csd_dev_mbhc.c

  OVERVIEW:     Multi button headset detection

  Dependencies: WCD codec
  
  Copyright (c) 2008-13 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/csd/dev/target/mdm/src/csd_dev_mbhc.c#1 $
  $DateTime: 2018/12/25 22:39:07 $
  $Author: pwbldsvc $
  $Change: 17929171 $
  $Revision: #1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  04/12/13   unni    Initial version  
==============================================================================*/

#include "csd_dev_mbhc.h"
#include "csd_status.h"
#include "csd_dev_ioctl.h"

/*=============================================================================

  FUNCTION      csd_dev_control_device_mbhc

  DESCRIPTION   Unsupported IOCTL call

  DEPENDENCIES  n/a

  PARAMETERS    none

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
uint32_t csd_dev_control_device_mbhc(struct csd_dev_mbhc_config* mbhc_ctrl)
{
  return CSD_EUNSUPPORTED;
}

/*=============================================================================

  FUNCTION      csd_dev_mbhc_init

  DESCRIPTION   Stubbed

  DEPENDENCIES  n/a

  PARAMETERS    none

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_mbhc_init()
{
  return CSD_EOK;
}
/*=============================================================================

  FUNCTION      csd_dev_mbhc_deinit

  DESCRIPTION   Stubbed

  DEPENDENCIES  n/a

  PARAMETERS    none

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_mbhc_deinit()
{
  return CSD_EOK;
}
