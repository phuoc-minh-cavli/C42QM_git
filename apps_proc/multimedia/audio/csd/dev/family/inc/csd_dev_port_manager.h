#ifndef _CSD_DEV_PORT_MANAGER_H_
#define _CSD_DEV_PORT_MANAGER_H_
/*==============================================================================
  @file csd_dev_port_manager.h

  The objective of this file is to provide prototype for dynamic slimbus port
  assignment and management.

  Copyright (c) 2008-13 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                      EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/csd/dev/family/inc/csd_dev_port_manager.h#1 $
  $DateTime: 2018/12/25 22:39:07 $
  $Author: pwbldsvc $
  $Change: 17929171 $
  $Revision: #1 $
  $Id: //components/rel/audio.tx/2.0/audio/csd/dev/family/inc/csd_dev_port_manager.h#1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  04/15/13   unni    Initial Version
==============================================================================*/
#include "csd_dev_common.h"

/*==============================================================================

  FUNCTION      csd_dev_pm_alloc_sb_port

  DESCRIPTION   Allocate slimbus port based on direction

  DEPENDENCIES  @see adsp_afe_service_commands.h

  PARAMETERS    @param direction   : indicates whether Rx or Tx
                @param afe_port_id : reference to be populated with dynamic
                                     slimbus port id

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_pm_alloc_sb_port(int8_t direction, uint16_t* afe_port_id);

/*==============================================================================

  FUNCTION      csd_dev_pm_sb_port_inc_ref_count

  DESCRIPTION   Incrementing reference count

  DEPENDENCIES  @see adsp_afe_service_commands.h

  PARAMETERS    @param afe_port_id : port for which reference is incremented

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_pm_sb_port_inc_ref_count(uint16_t afe_port_id);

/*==============================================================================

  FUNCTION      csd_dev_pm_free_sb_port

  DESCRIPTION   Decrement reference count and free when reference hits zero

  DEPENDENCIES

  PARAMETERS   @param afe_port_id : port which is being freed
               @param freed: flag to indicated whether freed or not

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

==============================================================================*/
int32_t csd_dev_pm_free_sb_port(uint16_t afe_port_id,bool_t* freed);

#endif /* _CSD_DEV_PORT_MANAGER_H_ */

