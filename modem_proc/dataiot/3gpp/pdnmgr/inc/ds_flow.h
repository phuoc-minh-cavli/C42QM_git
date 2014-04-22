#ifndef DS_FLOW_H
#define DS_FLOW_H

/*===========================================================================
                      DS_FLOW.H
DESCRIPTION
This file exposes DS flow APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_flow.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_pdn_manager.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define         DS_FLOW_NW_FLOW_TYPE                  0x1
#define         DS_FLOW_V4_FLOW_TYPE                  0x2
#define         DS_FLOW_V6_FLOW_TYPE                  0x4

/*---------------------------------------------------------------------------
  Maximum number of filters allowed in each QOS request. UMTS can fit atmost
  8 filters in one message. By restricting this to 8, UMTS need not worry
  about whether network rejected previous message.
---------------------------------------------------------------------------*/
#define MAX_FLTR_PER_REQ  16

/*---------------------------------------------------------------------------
  Maximum number of secondary flows, an iface can have. Since a block of
  MAX_FLTR_PER_REQ is reserved in filter precedence space for each QOS
  request only these many flows can be supported
---------------------------------------------------------------------------*/
#define DS_MAX_SEC_FLOWS  (256 / MAX_FLTR_PER_REQ)

/*---------------------------------------------------------------------------
  Instance of default flow of an iface and minimum instance that a secondary
  flow can use

  NOTE : PS_IFACE_MAX_SEC_FLOW MUST be <= PS_IFACE_MAX_SEC_FLOW_INST
---------------------------------------------------------------------------*/
#define DS_DEFAULT_FLOW_INST  0
#define DS_MIN_SEC_FLOW_INST  1
#define DS_MAX_SEC_FLOW_INST  127

/*===========================================================================
TYPEDEF DS_FLOW_CAPABILITY_ENUM_TYPE

DESCRIPTION
  Lists various capabilities of a flow. If a particular bit is ON, then
  flow will have corresponding capability

  NOTE : Enums are not bit masks. They represent bit position in a mask which
         represents a particular capability

    DS_FLOW_CAPABILITY_DEFAULT         - if this bit is ON, flow is a default
                                         flow
    DS_FLOW_CAPABILITY_DELAY_SENSITIVE - if this bit is ON, packets sent on
                                         this flow are delay sensitive
    DS_FLOW_CAPABILITY_DATA_DISALLOWED - if this bit is ON, all data packets
                                         are dropped on this flow by the
                                         framework

  NOTE: If more than DS_FLOW_CAPABILITY_MAX_ALLOWED capabilities are
        defined then one may have to change type definition of
        capability_mask in ds_flow control block
===========================================================================*/
typedef enum
{
  DS_FLOW_CAPABILITY_MIN              = 0x0,
  DS_FLOW_CAPABILITY_DEFAULT          = DS_FLOW_CAPABILITY_MIN,
  DS_FLOW_CAPABILITY_DELAY_SENSITIVE  = 0x1,
  DS_FLOW_CAPABILITY_DATA_DISALLOWED  = 0x2,
  DS_FLOW_CAPABILITY_MAX,
  DS_FLOW_CAPABILITY_MAX_ALLOWED      = 0x8,
  DS_FLOW_CAPABILITY_FORCE_SIGNED_32_BIT = 0x7FFFFFFF
} ds_flow_capability_enum_type;

typedef uint8  ds_flow_capability_mask_type;

/*---------------------------------------------------------------------------
TYPEDEF FLOW_STATE_TYPE

DESCRIPTION
  Flow state enum - what is the flow doing
---------------------------------------------------------------------------*/
typedef enum
{
  DS_FLOW_STATE_DOWN,
  DS_FLOW_STATE_UP,
  DS_FLOW_STATE_PENDING_CONFIRM,
  DS_FLOW_STATE_MAX
} ds_flow_state_enum_type;

/*===========================================================================
TYPEDEF DS_FLOW_TYPE

DESCIPTION
  Control block for a flow. An instance of this type is created when a flow
  is created and is destroyed when flow is deleted.
===========================================================================*/
struct ds_flow_s
{
  list_link_type          link;             /* Ptr to next node in a list  */
  ds_flow_type           *this_flow;        /* If NULL, flow is invalid    */
  qos_spec_type          *qos_info_ptr;     /* Ptr to QOS info structure   */
  qos_spec_type          *qos_modify_info_ptr; /* QOS_MODIFY info structure*/
  ds_bearer_mgr_s        *bearer_ptr;          /* Associated bearer        */
  ds_pdn_mgr_inst_type_s *inst_ptr;        /* Associated instance  */ 
  boolean                 tx_flow_enabled; /* Flag used to flow control   */

// Parametrs from flow context structure
  tft_type_T              tft_mod;     /* TFT Modify request sent to NW.*/
  ds_bearer_flow_manager_s *bearer_flow_mgr_ptr;/* Bearer Flow manager Pointer */
  uint32                  fi_identifier; /*bitmask of filter identifiers*/  
  uint32                  ignore_fi_id;  /* bitmask of filter id to ignore.*/ 
  uint8                   flow_type; /* Indicates if it is a NW Flow,v4,v6 */
               /*bits 2-4 identify v4 or V6 flow 4, 6)  bit 1 is for NW FLOW */
  uint32                  cid;  /* static or dynamic profile no */


  /*-----------------------------------------------------------------------
      Since flows can be created and deleted arbitrarily, one memory buffer
      could be used to represent more than one flow while mobile is powered
      on. So cookie is used to uniquely identify a flow across incarnations,
      which is useful in detecting if app is trying to use a flow it has
      already deleted
   -----------------------------------------------------------------------*/
  uint8                 cookie;

  /* Mask identifying verious capabilities of this flow */
  uint8                 capability_mask;

  ds_flow_state_enum_type        state;   /* This flow's state           */
  uint8                          index;
  word                           sdf_id;

};

struct ds_flow_list
{
  ds_flow_type     * default_flow_ptr;       /* Ptr to default flow       */
  list_type       sec_flow_list;          /* List of secondary flows   */
  uint8           num_sec_flows;          /* Number of secondary flows */
  uint8           next_cookie; /* Cookie assigned to the next new flow */
};


/*===========================================================================
FUNCTION DS_FLOW_IS_VALID()

DESCRIPTION
  This function checks for validity of flow ptr.

PARAMETERS
  flow_ptr

RETURN VALUE
  boolean.

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_flow_is_valid
(
  ds_flow_type                  *flow_p
);

/*===========================================================================
 FUNCTION PS_FLOW_GET_CAPABILITY()
  
DESCRIPTION
  Returns TRUE if a capability of a flow is turned on.
 
  NOTE : Only one capability can be fetched each time.

PARAMETERS
  flow_ptr   : pointer to flow
  capability : capability which needs to be tested
 
RETURN VALUE
  TRUE  : if capability is turned on
  FALSE : otherwise
===========================================================================*/
boolean ds_flow_get_capability
(
  ds_flow_type                  * flow_ptr,
  ds_flow_capability_enum_type    capability
);

/*===========================================================================
FUNCTION DS_FLOW_SET_CAPABILITY()

DESCRIPTION
  This function sets capability for flow.

PARAMETERS
  flow_ptr
  flow capability

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds_flow_set_capability
(
  ds_flow_type                  *flow_p,
  ds_flow_capability_enum_type   flow_cap
);

/*===========================================================================
FUNCTION DS_FLOW_RESET_CAPABILITY()

DESCRIPTION
  This function resets capability for flow.

PARAMETERS
  flow_ptr
  flow capability

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds_flow_reset_capability
(
  ds_flow_type                  *flow_p,
  ds_flow_capability_enum_type   flow_cap
);

/*===========================================================================
FUNCTION DS_FLOW_ALLOCATE_CONTEXT()

DESCRIPTION
  This function creates a new flow and initializes it.

PARAMETERS
  inst_ptr

RETURN VALUE
  ds_flow_type - Flow ptr.

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
ds_flow_type* ds_flow_allocate_context
(
  ds_pdn_mgr_inst_type_s  *inst_p
);

/*===========================================================================
FUNCTION DS_FLOW_GO_NULL_IND

DESCRIPTION
  Indicates that a flow is torn down

PARAMETERS
  flow_ptr  : ptr to flow on which to operate on.

RETURN VALUE
   None

DEPENDENCIES
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void ds_flow_go_null_ind
(
  ds_flow_type      *flow_ptr
);

/*===========================================================================
FUNCTION DS_FLOW_MGR_CONFIGURE_DEFAULT_FLOW

DESCRIPTION
  This function is invoked to configure the default flow for a given instance.
  Here we bind flow to the default bearer 

PARAMETERS
  PDN ptr, bearer ptr, and instance type

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_flow_mgr_configure_default_flow
(  
  ds_pdn_mgr_s               *pdn_mgr_p,
  ds_bearer_mgr_s            *bearer_mgr_p,
  ds_pdn_mgr_inst_type_s	 *inst_p
);

/*===========================================================================
FUNCTION DS_FLOW_UNBIND_DEFAULT_FLOWS

DESCRIPTION
  This function is used to unbind the phys link from all the default flows
  associated with the instances of a given PDN context

PARAMETERS   
  pointer to the PDN context
    
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_flow_unbind_default_flows
(
  ds_pdn_mgr_s    *pdn_mgr_p
);

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_ACTIVATE_IND

DESCRIPTION
  Post activated indication on the DS flow associated with the flow context.

PARAMETERS
  flow_context_p   - Flow Context information.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_context_activate_ind
( 
  ds_flow_type                      *flow_context_p
);

/*===========================================================================
FUNCTION DS_FLOW_DELETE

DESCRIPTION
  This function deletes the flow and removes it from instance.

PARAMETERS
  flow_ptr       

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds_flow_delete
(
  ds_flow_type           *flow_ptr
);

/*===========================================================================
FUNCTION DS_FLOW_CREATE_FLOW_CONTEXT

DESCRIPTION
  This function creates a flow, processes the requested QOS spec and stores
  it in the flow control block for later use. Both flow spec and filter spec
  are processed and validated for each direction. Both Rx and Tx flow specs are
  stored as is after making sure all specified parameters are correct. Tx
  filter spec is validated and stored in decreasing order of fltr precedence.
  Tx filter spec is passed to IP filtering library, where it is sorted and stored 
  to be applied to the outgoing IP traffic.

PARAMETERS


RETURN VALUE
   boolean - TRUE if creation passes, FALSE otherwise

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_flow_create_flow_context
(
 ds_flow_type               *flow_context,
 qos_spec_type              *qos_spec,
 ds_pdn_mgr_s               *pdn_mgr_p,
 ds_pdn_mgr_instance_type_e  inst_type,
 ds_bearer_mgr_s            *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_FLOW_ENABLE_DISABLE_TX
 
DESCRIPTION
  This function processes the TX flow enable/disable event posted to DS flow

PARAMETERS 
  Flow pointer
  boolean - Flow enabled/disabled
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  None
 
SIDE EFFECTS 
  None
===========================================================================*/
void ds_flow_enable_disable_tx
(
  ds_flow_type        *flow_p,
  boolean              enable_flow
);

/*===========================================================================
FUNCTION DS_FLOW_CONFIGURE_FLOW_CONTEXT

DESCRIPTION
  This function is invoked to configure the flow context.

PARAMETERS
  PDN ptr, bearer ptr, and instance type

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_flow_configure_flow_context
( 
  ds_flow_type               *flow_context_p,
  ds_bearer_mgr_s            *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_FLOW_RELEASE_FLOW_CONTEXT

DESCRIPTION
  Return the flow context.

PARAMETERS
  flow_context_p  - Pointer to the flow context.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_release_flow_context
(
  ds_flow_type *flow_context_p
);

/*===========================================================================
FUNCTION DS_FLOW_MODIFY_FLOW_CONTEXT

DESCRIPTION
  This function processes the QOS_MODIFY spec and stores it in the flow
  control block for later use. Both flow spec and filter spec
  are processed and validated for each direction. If Rx and/or Tx flow specs are
  modified, they are stored as is after making sure all specified parameters
  are correct. Tx filter spec is validated and stored in decreasing order
  of fltr precedence. Tx filters are disabled until network comes back with
  a response.

PARAMETERS
  iface_ptr         : corresponding iface on which QOS is requested
  flow_ptr          : flow to be modified
  qos_modify_params : structure containing qos_modified_spec and all the
                      information needed to process it
 
RETURN VALUE
   boolean - TRUE if modification passes, FALSE otherwise

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_flow_modify_flow_context
(
 ds_flow_type               *flow_context,
 qos_spec_type              *qos_spec
);

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_SET_GRANTED_FLOW

DESCRIPTION
 Set the granted flow.

PARAMETERS
  flow_context_p   - Flow Context information.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_flow_context_set_granted_flow
( 
  ds_flow_type    *flow_context_p
);

/*===========================================================================
FUNCTION DS_FLOW_MODIFY_FLOW_ACCEPTED

DESCRIPTION
  Indicates that QOS_MODIFY/PRIMARY_QOS_MODIFY is accepted by network.
  If it is QOS_MODIFY, migrates all state associated with QOS_MODIFY to
  ps_flow so that ps_flow represents QOS granted by network and cleans old QOS.

PARAMETERS
  iface_ptr : iface to which flow_ptr belongs
  flow_ptr  : ptr to flow
  info_code : reason for accepting QOS_MODIFY
  ps_errno  : specific error code in case operation fails

RETURN VALUE
   TRUE : on success
  FALSE : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  Previously granted QOS is not valid anymore and FLOW_MODIFY_ACCEPTED_EV is
  posted
===========================================================================*/
boolean ds_flow_modify_flow_accepted
(
  ds_flow_type                     * flow_ptr
);

/*===========================================================================
FUNCTION DS_FLOW_CONTEXT_GET_FILTER_SPEC

DESCRIPTION
  Retrieve the current filters assigned to the flow context.

PARAMETERS
  flow_context_p   - Flow Context information.
  modify           - boolean indicates current copy or modify set.
  app_fltr_spec    - Application filter spec to be populated.
  
DEPENDENCIES
  None.

RETURN VALUE
  boolean          - Returns true if filters are retrieved
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_context_get_filter_spec
( 
 ds_flow_type         *flow_context_p,
 boolean               modify,
 qos_spec_type        *app_fltr_spec
);

/*===========================================================================
FUNCTION DS_FLOW_GET_DEFAULT_FLOW_FROM_BEARER

DESCRIPTION
  Return the flow context.

PARAMETERS
  bearer_mgr_p  - Pointer to the bearer.
  instance type - v4/v6

DEPENDENCIES
  None.

RETURN VALUE
  Flow ptr
  
SIDE EFFECTS
  None.
===========================================================================*/
ds_flow_type* ds_flow_mgr_get_default_flow_from_bearer
(  
  ds_bearer_mgr_s              *bearer_mgr_p,
  ds_pdn_mgr_instance_type_e    inst_type
);

/*===========================================================================
FUNCTION DS_FLOW_DELETE_FILTER_FROM_FLOW

DESCRIPTION
  Removes the filter from the passed ps qos spec.

PARAMETERS
  flow_context_p   - Flow Context information.
  filter_id        - Filter to be removed.
  app_qos_spec    - filter spec from which the filter has to be deleted.
  modified_qos_spec  - filter spec which has the modified filter changes
  
DEPENDENCIES
  None.

RETURN VALUE
  boolean          - Returns true if filters are retrieved
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_flow_delete_filter_from_flow
( 
  ds_flow_type        *flow_context_p,
  uint32               filter_id,
  qos_spec_type       *app_qos_spec,
  qos_spec_type       *modified_qos_spec
);

/*===========================================================================
FUNCTION DS_FLOW_CREATE_FLOW_CONTEXT

DESCRIPTION
  This function creates a flow, processes the requested QOS spec and stores
  it in the flow control block for later use. Both flow spec and filter spec
  are processed and validated for each direction. Both Rx and Tx flow specs are
  stored as is after making sure all specified parameters are correct. Tx
  filter spec is validated and stored in decreasing order of fltr precedence.
  Tx filter spec is passed to IP filtering library, where it is sorted and stored 
  to be applied to the outgoing IP traffic.

PARAMETERS


RETURN VALUE
   boolean - TRUE if creation passes, FALSE otherwise

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_flow_create_flow_context
(
 ds_flow_type               *flow_context,
 qos_spec_type              *qos_spec,
 ds_pdn_mgr_s               *pdn_mgr_p,
 ds_pdn_mgr_instance_type_e  inst_type,
 ds_bearer_mgr_s            *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_FLOW_UPDATE_FILTER_IDENTIFIER_BIT_MASK

DESCRIPTION
  Update requested filter identifier bit mask.

PARAMETERS
 flow_context_p      - Pointer to flow context.
 set_identifier      - If true, set the bit else clear the bit.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_flow_update_filter_identifier_bit_mask
( 
  ds_flow_type     *flow_context_p,
  boolean           set_identifier
);
#endif /* DS_FLOW_H */
