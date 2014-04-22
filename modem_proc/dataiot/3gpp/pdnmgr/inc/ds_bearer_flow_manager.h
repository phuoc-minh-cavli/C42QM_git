#ifndef DS_BEARER_FLOW_MANAGER_H
#define DS_BEARER_FLOW_MANAGER_H

/*===========================================================================
                      DS_BEARER_FLOW_MANAGER.H
DESCRIPTION
This file exposes Bearer flow manager through API'S and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_bearer_flow_manager.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_flow.h"
#include "ds_gprs_bearer_manager.h"
#include "ds_pdn_tft.h"


// Macros for filter id bit mask
#define DS_SET_FI_BIT(mask, fi_id)  (mask |= (0x1 << fi_id))
#define DS_CLEAR_FI_BIT(mask, fi_id)  (mask &= (~(0x1 << fi_id)))
#define DS_IS_FI_BIT_SET(mask, fi_id)  ((mask & (0x1 << fi_id)) != 0)

/*===========================================================================
MACRO ARE_PORT_PARAM_VALID()

DESCRIPTION
  Validates if given port number and range params are valid

PARAMETERS
  port  : starting port number of the range
  range : ending port number of the range

RETURN VALUE
   TRUE  : if port > range 
   FALSE : otherwise
===========================================================================*/
#define ARE_PORT_PARAM_VALID(port, range)                                 \
  ((port > range)? FALSE : TRUE)

/*---------------------------------------------------------------------------
  enum type to identify the cause code to be sent to network.
---------------------------------------------------------------------------*/
typedef enum
{
  DS_BEARER_FLOW_INTERNAL_CC_NONE                    = 0x0,
  DS_BEARER_FLOW_INTERNAL_INSUFFICIENT_RESOURCES     = 0x1,
  DS_BEARER_FLOW_INTERNAL_FEATURE_NOT_SUPPORTED      = 0x2,
  DS_BEARER_FLOW_INTERNAL_QOS_NOT_ACCEPTED           = 0x3,
  DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_TFT        = 0x4,
  DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_TFT     = 0x5,
  DS_BEARER_FLOW_INTERNAL_SEMANTIC_ERR_IN_FILTER     = 0x6,
  DS_BEARER_FLOW_INTERNAL_SYNTACTICAL_ERR_IN_FILTER  = 0x7,
  DS_BEARER_FLOW_INTERNAL_BEARER_WITH_NO_TFT_ALREADY_PRESENT = 0x8,
  DS_BEARER_FLOW_INTERNAL_INVALID_MANDATORY_INFO     = 0x9,
  DS_BEARER_FLOW_INTERNAL_ACTIVATION_REJECTED        = 0xA,
  DS_BEARER_FLOW_INTERNAL_SERV_OPT_NOT_SUPPORTED     = 0xB,
  DS_BEARER_FLOW_INTERNAL_BCM_VIOLATION              = 0xC,
  DS_BEARER_FLOW_INTERNAL_CC_INVALID                 = 0xFF
} ds_bearer_flow_internal_cause_code_enum_type;

/*-----------------------------------------------------------------------------
  This structure contains UMTS specific info for the Bearer Flow manager.
-----------------------------------------------------------------------------*/
typedef struct
{ 
  ds_gprs_qos_params_type gprs_netw_alloc_qos;  /* Network granted QoS */
                                                /* Converted to Granted */
                                                /* QoS (to UMTS)*/
  qos_T                   req_qos;               /* requested QOS NW format  */
  qos_T                   min_qos;               /* req Min QOS in NW format */

  uint32                  src_stat_desc_netw_alloc;
                                                /*Source Statistics Descriptor*/
                                                /* for N/W allocated QOS*/
}ds_gprs_bearer_flow_manager_s;


/*-----------------------------------------------------------------------------
  This structure contains EPS specific info for the Bearer Flow manager.
-----------------------------------------------------------------------------*/
typedef struct
{ 
  sdf_qos_T      sdf_qos;  /* QOS in EPS Format */
  qos_T          neg_qos;  /* QOS in 3G Format */
} ds_eps_bearer_flow_manager_s;


/*-----------------------------------------------------------------------------
  Each bearer will have one bearer flow manager.
-----------------------------------------------------------------------------*/
struct ds_bearer_flow_manager
{ 
  list_link_type    link;
  uint8             index;    /* Index where the memory is allocated*/
  ds_bearer_mgr_s  *bearer_ptr;/* bearer associated with flow manager*/
  ds_pdn_mgr_s     *pdn_ptr;/* PDN associated with the bearer*/
  uint32            fi_identifier; /*bitmask of filter identifiers in use*/
  list_type         flow_context;/*Flow contexts associated with bearer*/
  qos_T             neg_qos; /* QOS in WCDMA format */
  ip_flow_type      rx_flow;  /* NW RX Flow in App format */
  ip_flow_type      tx_flow;  /* NW TX Flow in App Format */
  ds_gprs_bearer_flow_manager_s  *gprs_flow;
  ds_eps_bearer_flow_manager_s   *eps_flow;  
} ;

/*-----------------------------------------------------------------------------
  Flow context list for bearer flow manager will maintained in the below format.
-----------------------------------------------------------------------------*/
typedef struct
{
  list_link_type         bflow_link;
  ds_flow_type          *flow_context_p;
}ds_bearer_flow_manager_list_type;

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_ALLOCATE_MEM

DESCRIPTION
  Allocate a new flow manager for this bearer.

PARAMETERS
 bearer_mgr_p     - Pointer to bearer context.

DEPENDENCIES
  None.

RETURN VALUE
 ds_bearer_flow_manager_s * - Pointer to empty flow manager.
  
SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_flow_manager_s* ds_bearer_flow_manager_allocate_mem
( 
  ds_bearer_mgr_s *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_RELEASE_FLOW

DESCRIPTION
  Releases/frees the bearer flow manager.
  
PARAMETERS
 bearer_mgr_p     - Pointer to bearer context.

DEPENDENCIES
  None.

RETURN VALUE
 None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_flow_manager_release_flow
( 
  ds_bearer_mgr_s *bearer_mgr_p
);

/*===========================================================================
FUNCTION ds_bearer_flow_manager_find_entry

DESCRIPTION
  Finds bearer flow manager entry corresponding to given bearer

PARAMETERS
  bearer_mgr_p - Bearer context pointer
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_flow_manager_s* ds_bearer_flow_manager_find_entry
(
  ds_bearer_mgr_s *bearer_mgr_p
);


/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_BIND_FLOW

DESCRIPTION
  Bind the flows associated with the bearer.

PARAMETERS
 bearer_mgr_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_bind_flow
( 
  ds_bearer_mgr_s *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_FIND_FLOW_CONTEXT

DESCRIPTION
  Find the Flow context associated with the Qos.

PARAMETERS
 bearer_context_p     - Pointer to Bearer information.
 Flow_type               - Indicates V4 or V6 flow.

DEPENDENCIES
  None.

RETURN VALUE
 flow_context_p       - Pointer to flow context.
  
SIDE EFFECTS
  None.
===========================================================================*/
ds_flow_type* ds_bearer_flow_mgr_find_flow_context
( 
  ds_bearer_mgr_s *bearer_mgr_p,
  uint8            flow_type
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_GO_NULL

DESCRIPTION
  Post go Null on the flows associated with the bearer.

PARAMETERS
 bearer_mgr_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_flow_manager_go_null
( 
  ds_bearer_mgr_s *bearer_mgr_p
);


/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_DELETE_ALL_FLOWS

DESCRIPTION
  Delets all flows associated with the bearer.

PARAMETERS
 bearer_mgr_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_flow_manager_delete_all_flows
( 
  ds_bearer_mgr_s *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_CONVERT_DATA_RATE

DESCRIPTION
  Convert the NW FLOW to APP Flow. Invoke the mode specific handlers to 
  convert the value.

PARAMETERS
  bearer_context_p -  Bearer information
  mode_info_p      -  mode specific call info. 
  app_qos_p        -  Pointer to App Qos structure.

DEPENDENCIES
  None.

RETURN VALUE
boolean - return DS3G_FAILURE or DS3G_SUCCESS
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean  ds_bearer_flow_manager_convert_data_rate
(
 ds_bearer_mgr_s                              *bearer_mgr_p,
 sdf_qos_T                                    *sdf_qos,
 qos_T                                        *neg_qos,
 qos_spec_type                                *app_qos,
 ds_bearer_flow_internal_cause_code_enum_type *icause_code
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_UPDATE_FILTER_IDENTIFIER_BIT_MASK

DESCRIPTION
  Update filter identifier bit mask for all the filters in the flow context.

PARAMETERS
 bearer_context_p    - Pointer to bearer Context.
 flow_context_p      - Pointer to flow context.
 set_identifier      - If true, set the bit else clear the bit.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_update_filter_identifier_bit_mask
( 
  ds_bearer_mgr_s  *bearer_context_p,
  ds_flow_type     *flow_context_p,
  boolean           set_identifier
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_INST_DOWN_HDLR

DESCRIPTION
  The instance has gone out of use. Check for an UE initiated bearers and
  clean them up. Update the TFT information that needs to be sent out to 
  the network.

PARAMETERS
  pdn_context_p            -  Pointer to PDN context.
  bearer_context_p         -  Pointer to Bearer context.
  flow_type                -  IP Type.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_flow_manager_inst_down_hdlr
(
  ds_pdn_mgr_s                *pdn_context_p,
  ds_bearer_mgr_s             *bearer_context_p,
  uint8                        flow_type
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_CONVERT_CAUSE_CODE

DESCRIPTION
  Convert the generic cause codes to specific cause code.

PARAMETERS
  mode_info_ptr      -  CM passed mode information.
  ds_int_cause_ptr   -  DS generic cause  

DEPENDENCIES
  None.

RETURN VALUE
  Returns True if the cause code conversion is successful.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_convert_cause_code
(
  esm_cause_T                                  *esm_cause,
  ds_bearer_flow_internal_cause_code_enum_type *icause_ptr
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_REMOVE_FLOW_CONTEXT_FROM_FLOW_MANAGER

DESCRIPTION
  Remove a flow context from the bearer flow manager list.

PARAMETERS
 flow_context_s       - Pointer to Flow Context infomation.
 bearer_context_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_flow_manager_remove_flow_context_from_flow_manager
( 
  ds_bearer_mgr_s            *bearer_context_p,
  ds_flow_type               *flow_context_p
);


/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_CHECK_SDF_ID

DESCRIPTION
  Handle the bearer modification response.

PARAMETERS
  sdf_id - Flow identifier

DEPENDENCIES
  None.
    
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_flow_manager_check_sdf_id
(
  word   sdf_id
);

/*===========================================================================
FUNCTION DS_BEARER_FLOW_MANAGER_ADD_FLOW_CONTEXT_TO_FLOW_MANAGER

DESCRIPTION
  Add a flow context to the bearer flow manager list.

PARAMETERS
 flow_context_s       - Pointer to Flow Context infomation.
 bearer_context_p     - Pointer to Bearer information.

DEPENDENCIES
  None.

RETURN VALUE
 boolean              - Indicates true or false.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_flow_manager_add_flow_context_to_flow_manager
( 
  ds_bearer_mgr_s          *bearer_context_p,
  ds_flow_type             *flow_context_p
);

#endif /* DS_BEARER_FLOW_MANAGER_H */
