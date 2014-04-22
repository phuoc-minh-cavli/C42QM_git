#ifndef HSU_CONF_SEL_RDM_H
#define HSU_CONF_SEL_RDM_H
/*==============================================================================

   High Speed Configuration Selector RDM APIs
        I N T E R N A L  H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    This file defines APIs for RDM and compostion management for HS USB.

  EXTERNALIZED FUNCTIONS
    TBD.

  INITALIZATION AND SEQUENCING REQUIREMENTS
    None.
    
  Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/inc/hsu_conf_sel_rdm.h#1 $
  $DateTime: 2020/01/30 22:49:35 $

    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  4/22/09  tk   created 
==============================================================================*/

#include "customer.h" 
#include "hsu_common_int.h"
#include "hsu_conf_sel_stack_utils_int.h"
#include "jtypes_int.h"
#include "jos_int.h"
#include "jslave_init.h"
#include "hsu_conf_sel_comp_utils.h"
#include "rdevmap.h"

/*==============================================================================
  Typedefs
==============================================================================*/
typedef void (*hsu_set_comp_cb_type)(boolean);

typedef struct
{
  rdm_device_enum_type  rdm_device;
  rdm_service_enum_type rdm_service;
} hsu_rdm_dev_svc_pare_type;

/*===========================================================================
FUNCTION              HSU_SET_COMPOSITON

DESCRIPTION
  None
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hsu_set_compositon
(
  hsu_conf_sel_composition_id_enum  composition_id,
  hsu_set_comp_cb_type complete_cb,
  hsu_rdm_dev_svc_pare_type *old_rdm_map,
  juint8_t old_rdm_map_size,
  hsu_rdm_dev_svc_pare_type *new_rdm_map,
  juint8_t new_rdm_map_size
);

/*===========================================================================
FUNCTION              hsu_get_rdm_map

DESCRIPTION
  None
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
juint8_t hsu_get_rdm_map
(
  const hsu_conf_sel_comp_info_type * comp,
  hsu_rdm_dev_svc_pare_type *rdm_map,
  juint8_t max_rdm_map_size
);

#endif /* !HSU_CONF_SEL_RDM_H */
