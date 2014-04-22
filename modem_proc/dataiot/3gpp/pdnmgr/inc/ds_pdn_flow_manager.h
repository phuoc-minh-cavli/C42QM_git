#ifndef DS_PDN_FLOW_MANAGER_H
#define DS_PDN_FLOW_MANAGER_H

/*===========================================================================
                      DS_PDN_FLOW_MANGER.H
DESCRIPTION
This file exposes PDN flow manager through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_flow_manager.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_pdn_manager.h"
#include "ds_pdn_tft.h"

/* Bearer control mode info */
typedef enum
{
  DS_3GPP_BCM_INVALID   = 0,
  DS_3GPP_BCM_UE_ONLY   = 1,
  DS_3GPP_BCM_UE_NW     = 2
} ds_pdn_bcm_info_e;

/*-----------------------------------------------------------------------------
  Pending flow context list for PDN flow manager will maintained in the below format.
-----------------------------------------------------------------------------*/
typedef struct
{
  list_link_type         pflow_link;
  ds_flow_type          *flow_context_p;
}ds_pdn_flow_manager_pending_list_type;

/*-----------------------------------------------------------------------------
  Each PDN will have one flow manager.
-----------------------------------------------------------------------------*/
struct ds_pdn_flow_manager
{  
  ds_pdn_mgr_s       *pdn_mgr_p;/* PDN associated with the flow manager */
  uint32              fi_preced[DS_FI_PRECED_ARR_SIZE];/*bitmask of the Filter precedence in use*/
  uint32              req_fi_preced[DS_FI_PRECED_ARR_SIZE];/*bitmask of the requested Filter prece*/
  list_type           bearer_flow_mgr;/*bearers associated with this PDN*/
  list_type           pending_flow_cntx;/*Pending UE initiated requests*/
  ds_pdn_bcm_info_e   bcm_info;   /* bearer control mode info */
  apn_ambr_T          eps_pdn_ambr; /* PDN AMBR - applicable only for LTE */ 
};

typedef struct
{
  ds_flow_type    *flow_context_p;
  ds_bearer_mgr_s *bearer_context_p;
  uint32           fltr_id_conflict;
  uint32           preced_conflict[8];
  boolean          prec_conf_on_same_bearer_modify;
}ds_pdn_flow_manager_cmd_info_type;

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_ALLOCATE_MEM

DESCRIPTION
  Allocate a new flow manager for this PDN.

PARAMETERS
 ds_pdn_mgr_s     - Pointer to PDN context information.

DEPENDENCIES
  None.

RETURN VALUE
 ds_pdn_flow_manager_s * - Pointer to empty flow manager.
  
SIDE EFFECTS
  None.
===========================================================================*/
ds_pdn_flow_manager_s * ds_pdn_flow_manager_allocate_mem
( 
  ds_pdn_mgr_s *pdn_mgr_p
);

/*===========================================================================
FUNCTION DS_PDN_FLOW_MGR_RELEASE_FLOW

DESCRIPTION
  Release the flow manager associated with the PDN connection.

PARAMETERS
  pdn_context_p  - Pointer to the PDN context.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_flow_mgr_release_flow_manager
(
  ds_pdn_mgr_s *pdn_mgr_p
);

/*===========================================================================
FUNCTION DS_PDN_FLOW_MGR_GET_ENTRY

DESCRIPTION
  Return PDN flow manager pointer

PARAMETERS
 uint8     - Index for which pointer is required.

DEPENDENCIES
  None.

RETURN VALUE
 ds_pdn_flow_manager_s * - Pointer to PDN flow manager.
  
SIDE EFFECTS
  None.
===========================================================================*/
ds_pdn_flow_manager_s * ds_pdn_flow_mgr_get_entry
( 
  uint8   index
);

/*===========================================================================
FUNCTION DS_PDN_FLOW_MGR_ADD_BEARER_TO_PDN_FLOW

DESCRIPTION
  Add a bearer context to the flow manager list.

PARAMETERS
 pdn_context_s        - Pointer to PDN Context.
 bearer_context_p     - Pointer to Bearer.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_mgr_add_bearer_to_pdn_flow
( 
  ds_pdn_mgr_s    *pdn_mgr_p,
  ds_bearer_mgr_s *bearer_mgr_p  
);

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_REMOVE_BEARER_FROM_PDN_FLOW

DESCRIPTION
  Remove a bearer from the PDN flow manager list.

PARAMETERS
 pdn_context_s       - Pointer to PDN Contex.
 bearer_context_p     - Pointer to Bearer.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_manager_remove_bearer_from_pdn_flow
( 
  ds_pdn_mgr_s    *pdn_mgr_p,
  ds_bearer_mgr_s *bearer_mgr_p
);

/*===========================================================================
FUNCTION      ds_pdn_flow_mgr_decode_bcm

DESCRIPTION
  This function decodes the bearer control mode info

PARAMETERS
  Protocol info ptr
  Protocol length
  PDN index
  
DEPENDENCIES  None.

RETURN VALUE  TRUE - IF BCM decoded successfully
                       FALSE - otherwise

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_pdn_flow_mgr_decode_bcm
(
  const byte       *protocol_pkt_ptr, /* Pointer to the buffer for bcm info */
  int               protocol_pkt_len,  /* Length of the bcm container */
  uint8             index
);

/*===========================================================================
FUNCTION ds_pdn_flow_mgr_update_uplink_flow_capability 

DESCRIPTION
  This function is used to enable or disable uplink flow based on 
  flow direction (This is based on GBR values granted by NW) 

PARAMETERS
 bearer_mgr_p: Bearer for which data path is to be setup
 boolean:  to Enable or Disable flow
 
DEPENDENCIES
  None.

RETURN VALUE
None
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_flow_mgr_update_uplink_flow_capability
(
  ds_bearer_mgr_s *bearer_mgr_p,
  boolean          ul_flow_enable
);

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_BEARER_ALLOCATION_IND

DESCRIPTION
  Handle the bearer allocation Indication.

PARAMETERS
  pdn_ptr         -  Ptr to the PDN context.
  bearer_ptr      -  Ptr to the bearer context.
  mode_info_ptr   -  CM passed mode information.

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS or DS3G_FAILURE.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_flow_manager_bearer_allocation_ind
(
  ds_pdn_mgr_s               *pdn_mgr_p,
  ds_bearer_mgr_s            *bearer_mgr_p,
  emm_act_dedicated_bearer_context_request_ind_s_type   
                             *info_ptr,
  esm_cause_T                *esm_cause                            
);

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_INST_DOWN_HDLR

DESCRIPTION
  The instance has gone out of use. clean up any pending flow contexts and
  update the TFT information to inform network about filter cleanup.

PARAMETERS
  pdn_context_p     -  Pointer to PDN context.
  inst_p                  -  Instance pointer

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -e655 Bitwise operation on compatible enums */
void ds_pdn_flow_manager_inst_down_hdlr
(
  ds_pdn_mgr_s               *pdn_context_p,
  ds_pdn_mgr_inst_type_s     *inst_p
);

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_BEARER_MODIFICATION_IND

DESCRIPTION
  Handle the bearer modification Indication.

PARAMETERS
  mode_info_ptr   - NAS passed mode information.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_flow_manager_bearer_modification_ind
(
  emm_modify_bearer_context_request_ind_s_type   *info_ptr
);

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_PROCESS_FILTER_ID_CONFLICT

DESCRIPTION
  Handle the filter ID conflict.

PARAMETERS
  info_ptr   - Flow command passed information.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_flow_manager_process_filter_id_conflict
(
  ds_pdn_flow_manager_cmd_info_type   *cmd_info_ptr
);

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_HANDLE_FILTER_PRECED_CONFLICT

DESCRIPTION
  Handle the filter precedence conflict.

PARAMETERS
  info_ptr   - Flow command passed information.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_flow_manager_process_filter_preced_conflict
(
  ds_pdn_flow_manager_cmd_info_type   *cmd_info_ptr
);

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_UPDATE_PRECEDENCE_BIT_MASK

DESCRIPTION
  Update precednce bit mask for the given flow context.

PARAMETERS
 pdn_context_s       - Pointer to PDN Context.
 flow_context_p      - Pointer to flow context.
 req_modify          - boolean. If true, request from modify filter set.
 set_precedence      - boolean. If true, set the bit else clear the bit.
 req_precedence      - boolean. If true, update requested precedence else
                       update precedence bit mask.  

DEPENDENCIES
  None.

RETURN VALUE
 boolean             - Indicates true or false.
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_flow_manager_update_precedence_bit_mask
( 
  ds_pdn_mgr_s      *pdn_mgr_p,
  ds_flow_type      *flow_context_p,
  boolean            req_modify,
  boolean            set_precedence,
  boolean            req_precedence     
);

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_FIND_PENDING_FLOW_CONTEXT

DESCRIPTION
  Return a pending flow context based on SDF_ID.

PARAMETERS
 pdn_cntx_p       -  PDN pointer - Output pointer
 sdf_id              -  SDF ID to find.

DEPENDENCIES
  None.

RETURN VALUE
 ds_flow_type * - Return valid pointer if flow context exists. Else NULL.
  
SIDE EFFECTS
  None.
===========================================================================*/
ds_flow_type* ds_pdn_flow_manager_find_pending_flow_context
( 
  ds_pdn_mgr_s   **pdn_cntx_p,
  uint16           sdf_id
);

/*===========================================================================
FUNCTION DS_PDN_FLOW_MANAGER_REMOVE_PENDING_FLOW_CONTEXT

DESCRIPTION
  Remove a pending flow context from the flow manager list.

PARAMETERS
 pdn_context_s       - Pointer to PDN Contex.
 flow_context_p     - Pointer to Bearer.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_flow_manager_remove_pending_flow_context
( 
  ds_pdn_mgr_s    *pdn_context_p,
  ds_flow_type    *flow_context_p
);

#endif /* DS_PDN_FLOW_MANAGER_H */
