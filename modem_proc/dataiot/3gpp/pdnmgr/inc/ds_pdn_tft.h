#ifndef DS_PDN_TFT_H
#define DS_PDN_TFT_H

/*===========================================================================
                      DS_PDN_TFT.H
DESCRIPTION
This file manages flow context and TFT..

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_tft.h#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
10/25/18    sd      First checkin
===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_flow.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define DS_FLOW_E_BIT_DISABLE    0
#define DS_FLOW_E_BIT_ENABLE     1
#define DS_MAX_FI_ID             16
#define DS_MAX_FI_PRECED         255
#define DS_MIN_FI_PRECED         1
#define DS_FI_PRECED_ARR_SIZE    8
#define DS_DEF_FILTER_PRECD      256
#define DS_DEF_FILTER_ID         256

#define DS_PDN_TFT_MAX_FILTERS                    16

/*---------------------------------------------------------------------------
  The enum type for TFT operation code  (per 3GPP TS24.008 10.5.6.12)
---------------------------------------------------------------------------*/
typedef enum
{
  DS_PDN_TFT_OPCODE_SPARE      = 0x0,  /* Spare                           */
  DS_PDN_TFT_OPCODE_CREATE     = 0x1,  /* Create new TFT                  */
  DS_PDN_TFT_OPCODE_DEL_TFT    = 0x2,  /* Delete existing TFT             */
  DS_PDN_TFT_OPCODE_ADD        = 0x3,  /* Add pkt ftr to exiting TFT      */
  DS_PDN_TFT_OPCODE_REPLACE    = 0x4,  /* Replace pkt ftr in existing TFT */
  DS_PDN_TFT_OPCODE_DEL_FILTER = 0x5,  /* Delete pkt ftr in existing TFT  */
  DS_PDN_TFT_OPCODE_NO_OP      = 0x6,  /* No TFT Operation                */
  DS_PDN_TFT_OPCODE_RESERVED   = 0xFF  /* Reserved                        */
} ds_pdn_tft_opcode_enum_type;

/*---------------------------------------------------------------------------
  The enum type for TFT operation code  (per 3GPP TS24.008 10.5.6.12)
---------------------------------------------------------------------------*/
typedef enum
{
  DS_PDN_TFT_AUTHORIZATION_TOKEN      = 0x1,  /* Authorizationt token     */
  DS_PDN_TFT_FLOW_IDENTIFIER          = 0x2,  /* Flow identifier          */
  DS_PDN_TFT_PACKET_FILTER_IDENTIFIER = 0x3,  /* Filter identifier        */
  DS_PDN_TFT_PARAM_RESERVED           = 0xFF  /* Reserved                 */
} ds_pdn_tft_param_enum_type;

/*---------------------------------------------------------------------------
  The enum type for TFT packet filter component type identifier  
  (per 3GPP TS24.008 10.5.6.12)
---------------------------------------------------------------------------*/
typedef enum
{
  DS_PDN_TFT_EMPTY_TYPE          = 0x00,   /* Empty type                            */
  DS_PDN_TFT_IPV4_ADDR           = 0x10,   /* IPV4 Address type                     */
  DS_PDN_TFT_IPV4_LOCAL_ADDR     = 0x11,   /* IPV4 local Address type               */
  DS_PDN_TFT_IPV6_ADDR           = 0x20,   /* IPV6 Address type                     */
  DS_PDN_TFT_IPV6_REM_PRE_ADDR   = 0x21,   /* IPV6 remote address/prefix lengthtype */
  DS_PDN_TFT_IPV6_LOCAL_PRE_ADDR = 0x23,   /* IPV6 local address/prefix lengthtype  */
  DS_PDN_TFT_NEXT_HDR_TYPE       = 0x30,   /* Protocol identifier                   */
  DS_PDN_TFT_SINGLE_LOCAL_PORT   = 0x40,   /* Single local port                     */
  DS_PDN_TFT_LOCAL_PORT_RANGE    = 0x41,   /* local port range                      */
  DS_PDN_TFT_SINGLE_REMOTE_PORT  = 0x50,   /* Single remote port                    */
  DS_PDN_TFT_REMOTE_PORT_RANGE   = 0x51,   /* remote port range                     */
  DS_PDN_TFT_SEC_PARAM_IDX       = 0x60,   /* Security parameter index              */
  DS_PDN_TFT_TYPE_OF_SRVC        = 0x70,   /* Type of service                       */
  DS_PDN_TFT_IP6_TRAFFIC_CLASS   = DS_PDN_TFT_TYPE_OF_SRVC, 
                                            /* IPV6 traffic class                    */
  DS_PDN_TFT_FLOW_LABEL          = 0x80    /* Flow Label                            */
} ds_pdn_tft_comp_type_id_enum_type;

/*---------------------------------------------------------------------------
  enum type to help verify that the Network did not send wrong params.
---------------------------------------------------------------------------*/
typedef enum
{
  DS_PDN_TFT_FILTER_NONE                  = 0x0,
  DS_PDN_TFT_FILTER_IP_ADDR_TYPE          = 0x1,
  DS_PDN_TFT_FILTER_NXT_HDR_TYPE          = 0x2,
  DS_PDN_TFT_FILTER_LOCAL_PORT_TYPE       = 0x4,
  DS_PDN_TFT_FILTER_REMOTE_PORT_TYPE      = 0x8,
  DS_PDN_TFT_FILTER_SEC_TYPE              = 0x10,
  DS_PDN_TFT_FILTER_TOS_TYPE              = 0x20,
  DS_PDN_TFT_FILTER_FLOW_LABEL_TYPE       = 0x40,
  DS_PDN_TFT_FILTER_IP_LOCAL_ADDR_TYPE    = 0x80,
  DS_PDN_TFT_FILTER_IP_REMOTE_PREFIX_TYPE = 0x100,
  DS_PDN_TFT_FILTER_IP_LOCAL_PREFIX_TYPE  = 0x200,
  DS_PDN_TFT_FILTER_ALL                   = 0x3FF
} ds_pdn_tft_filter_mask_enum_type;

/*---------------------------------------------------------------------------
  The enum type for TFT packet filter direction  
  (per 3GPP TS24.008 10.5.6.12)
---------------------------------------------------------------------------*/
typedef enum
{
  DS_PDN_TFT_PRE_REL_7        = 0,    /* PRE REL 7 Filter                 */
  DS_PDN_TFT_DOWNLINK         = 1,    /* Downlink only Filter             */
  DS_PDN_TFT_UPLINK           = 2,    /* Uplink only Filter               */
  DS_PDN_TFT_BIDIRECTIONAL    = 3     /*  Bi-Directional Filter           */
} ds_pdn_tft_pkt_filter_direction_enum_type;


/*-----------------------------------------------------------------------------
  Book keeping structure when processing NW modification indication
-----------------------------------------------------------------------------*/
typedef struct
{ 
  uint32   flow_cntx_fltr_id;/* Bitmask of FLTR ID matching flow             */
  uint32   bearer_fltr_id;/* Bit mask of FLTR ID matching bearer,not flow    */
  uint32   non_match_fltr_id;/* Bit mask of FLTR non matching                */
  uint32   preced_conflict[DS_FI_PRECED_ARR_SIZE];/* bit mask of precedence  */
  uint32   preced_conflict_cnt;
  qos_spec_type   curr_qos; /* current qos flow spec */
  ip_filter_type  curr_tx_fltr[MAX_PACKET_FILTERS];/* current tx fltrs*/
  ip_filter_type  curr_rx_fltr[MAX_PACKET_FILTERS];/* current rx fltrs*/
  qos_spec_type   mod_qos; /* modified qos flow spec */
  ip_filter_type  mod_tx_fltr[MAX_PACKET_FILTERS];/* modified tx fltrs*/
  ip_filter_type  mod_rx_fltr[MAX_PACKET_FILTERS];/* modified rx fltrs*/
  void     *conflict_bearer[7];/*pointers to bearers with 
                                                          conflicting preced */
  uint32   conflict_bearer_cnt; /* counter to keep track of above array      */
  boolean  new_flow;            /* track if new flow needs to be created.    */ 
  boolean  is_add;          /* track if this is replace or add           */
  boolean  prec_conf_on_same_bearer_create;
  boolean  prec_conf_on_same_bearer_modify;
} ds_pdn_tft_nw_modify_qos_bk_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================
FUNCTION DS_PDN_TFT_SET_FILTER_PRECED_BIT

DESCRIPTION
  Set the filter precedence bit in the given precedence bit mask.

PARAMETERS
  fi_preced         - Filter precedence.
  preced_bit_mask   - Precedence bit mask to be updated.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_tft_set_filter_preced_bit
(
  uint16      fi_preced,
  uint32     *preced_bit_mask
);

/*===========================================================================
FUNCTION DS_PDN_TFT_CLEAR_FILTER_PRECED_BIT

DESCRIPTION
  Clear the filter precedence bit in the given precedence bit mask

PARAMETERS
  fi_preced         - Filter precedence.
  preced_bit_mask   - Precedence bit mask to be updated.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_tft_clear_filter_preced_bit
(
  uint16      fi_preced,
  uint32     *preced_bit_mask
);

/*===========================================================================
FUNCTION ds_pdn_tft_is_filter_preced_bit_set

DESCRIPTION
  Return whether the filter precedence is in use for a give precedence bit mask

PARAMETERS
  fi_preced         - Filter precedence.
  preced_bit_mask   - Precedence bit mask to be updated.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - the bit for this filter precedence is set
  FALSE - otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_tft_is_filter_preced_bit_set
(
  uint16      fi_preced,
  uint32     *preced_bit_mask
);

/*===========================================================================
FUNCTION DS_PDN_TFT_IS_FILTER_IDENTIFIER_REQUESTED

DESCRIPTION
  Return whether this filter identifier is already in use.

PARAMETERS
  fi_identifier       - Filter identifier.
  fi_bit_mask         - Filter bitmask to check filter identifier 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - the bit for this filter identifier is set
  FALSE - otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_tft_is_filter_identifier_requested
(
  uint16 fi_identifier,
  uint32    *fi_bit_mask
);

/*===========================================================================
FUNCTION DS_PDN_TFT_SET_FILTER_IDENTIFIER_BIT

DESCRIPTION
  Set the bit for this filter identifier.

PARAMETERS
  fi_identifier       - Filter identifier.
  fi_bit_mask         - Filter bitmask to check filter identifier 

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_tft_set_filter_identifier_bit
(
  uint16     fi_identifier,
  uint32    *fi_bit_mask
);

/*===========================================================================
FUNCTION DS_PDN_CLEAR_FILTER_IDENTIFIER_BIT

DESCRIPTION
  unset the bit for this filter identifier.

PARAMETERS
  fi_identifier       - Filter identifier.
  fi_bit_mask         - Filter bitmask to check filter identifier 

DEPENDENCIES
  None.

RETURN VALUE
 None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_tft_clear_filter_identifier_bit
(
  uint16 fi_identifier,
  uint32    *fi_bit_mask
);

/*===========================================================================
FUNCTION DS_PDN_TFT_CHECK_MOD_FLTR_SEMANTIC_ERROR

DESCRIPTION
  Verify if the input TFT has any semantic errors.

PARAMETERS
  nw_tft                -  TFT information  

DEPENDENCIES
  None.

RETURN VALUE
  boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_tft_check_mod_fltr_semantic_error
(
  tft_type_T                            *nw_tft  
);

/*===========================================================================
FUNCTION DS_PDN_TFT_CHECK_FOR_REPEATED_PRECEDENCE

DESCRIPTION
  Verify if the input TFT has any semantic errors.

PARAMETERS
  nw_tft                -  TFT information  

DEPENDENCIES
  None.

RETURN VALUE
  boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_tft_check_for_repeated_precedence
(
  tft_type_T                            *nw_tft  
);

/*===========================================================================
FUNCTION DS_PDN_TFT_CONFIGURE_NW_BK_FLOW_SPEC

DESCRIPTION
  Configure the flow spec and filters in the Network book keeping 
  data strutcure.

PARAMETERS
  nw_bk                       -  Boook keeping info.
                      
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_tft_configure_nw_bk_flow_spec
(
  ds_pdn_tft_nw_modify_qos_bk_s      *nw_bk
);

/*===========================================================================
FUNCTION DS_PDN_TFT_CHECK_NW_FLTR_ID_PRECED

DESCRIPTION
  Verify the filter ID and precedence for each filter passed by the network.
  This checks if the filter ID or precedence is repeated.
  Filter ID conflict is detected. Precedence conflict is detected.
  Returns the filters with conflict, without conflict.
  Returns the precedence values with conflict.
  This function is called for NW initiated Qos Modification check.

PARAMETERS
  pdn_context_p               -  PDN information
  bearer_context_p            -  Bearer information
  flow_context_p              -  Flow context
  nw_tft_p                    -  TFT information
  nw_bk                       -  Boook keeping info.
  del_filter                  -  Indicate delete filter operation
                      
DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_tft_check_nw_fltr_id_preced
(
  ds_pdn_mgr_s                       *pdn_context_p,
  ds_bearer_mgr_s                    *bearer_context_p,
  ds_flow_type                       *flow_context_p,
  tft_type_T                         *nw_tft,
  ds_pdn_tft_nw_modify_qos_bk_s      *nw_bk,
  boolean                             del_filter
);

/*===========================================================================
FUNCTION DS_PDN_TFT_FIND_BEARER_WITH_FLTR_PRECED_CONFLICT

DESCRIPTION
  Find the bearers on which the precedence conflict occurs.
  This checks if the filter ID or precedence is repeated.
  Filter ID conflict is detected. Precedence conflict is detected.
  Returns the filters with conflict, without conflict.
  Returns the precedence values with conflict.
  This function is called for NW initiated Qos Modification check.

PARAMETERS
  pdn_context_p               -  PDN information.
  bearer_context_p            -  Bearer information.
  flow_context_p              -  Flow context.
  nw_bk                       -  Boook keeping info.
                      
DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_tft_find_bearer_with_preced_conflict
(
  ds_pdn_mgr_s                       *pdn_context_p,
  ds_bearer_mgr_s                    *bearer_context_p,
  ds_pdn_tft_nw_modify_qos_bk_s      *nw_bk
);

/*===========================================================================
FUNCTION DS_PDN_TFT_PRECED_CONFLICT_ON_BEARER

DESCRIPTION
  Verifies if the input bearer is in the conflict bearer array.

PARAMETERS
  nw_bk                       -  Boook keeping info.
  input                       -  bearer_ptr
                      
DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_tft_preced_conflict_on_bearer
(
  ds_pdn_tft_nw_modify_qos_bk_s      *nw_bk,
  void                               *input
);

#endif /* DS_PDN_TFT_H */

