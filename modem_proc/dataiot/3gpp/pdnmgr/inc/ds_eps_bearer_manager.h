#ifndef DS_EPS_BEARER_MANAGER_H
#define DS_EPS_BEARER_MANAGER_H

/*===========================================================================
                      DS_EPS_BEARER_MANAGER.H
DESCRIPTION
This file exposes bearer manager for LTE through APIs and data structures.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_eps_bearer_manager.h#5 $
  $DateTime: 2021/07/02 03:09:50 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_bearer_manager.h"
#include "ps_meta_info.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define DS_EPS_1KB_BITRATE_MINIMUM_VALUE      01
#define DS_EPS_1KB_BITRATE_START_RANGE        01
#define DS_EPS_1KB_BITRATE_END_RANGE          63

#define DS_EPS_8KB_BITRATE_MINIMUM_VALUE      64
#define DS_EPS_8KB_BITRATE_START_RANGE        64
#define DS_EPS_8KB_BITRATE_END_RANGE          127
#define DS_EPS_8KB_BITRATE_MULTIPLE           8 

#define DS_EPS_64KB_BITRATE_MINIMUM_VALUE     576
#define DS_EPS_64KB_BITRATE_START_RANGE       128
#define DS_EPS_64KB_BITRATE_END_RANGE         254
#define DS_EPS_64KB_BITRATE_MULTIPLE          64 

#define DS_EPS_100KB_BITRATE_MINIMUM_VALUE    8700
#define DS_EPS_100KB_BITRATE_BASE_RATE        8600
#define DS_EPS_100KB_BITRATE_START_RANGE      01
#define DS_EPS_100KB_BITRATE_END_RANGE        74
#define DS_EPS_100KB_BITRATE_MULTIPLE         100 

#define DS_EPS_1MB_BITRATE_MINIMUM_VALUE      17000
#define DS_EPS_1MB_BITRATE_START_RANGE        75
#define DS_EPS_1MB_BITRATE_END_RANGE          186
#define DS_EPS_1MB_BITRATE_MULTIPLE           1000 

#define DS_EPS_2MB_BITRATE_MINIMUM_VALUE      130000
#define DS_EPS_2MB_BITRATE_START_RANGE        187
#define DS_EPS_2MB_BITRATE_END_RANGE          250
#define DS_EPS_2MB_BITRATE_MULTIPLE           2000

#define DS_EPS_4MB_BITRATE_MINIMUM_VALUE      260000
#define DS_EPS_4MB_BITRATE_START_RANGE        01
#define DS_EPS_4MB_BITRATE_END_RANGE          61
#define DS_EPS_4MB_BITRATE_MULTIPLE           4000

#define DS_EPS_10MB_BITRATE_MINIMUM_VALUE      510000
#define DS_EPS_10MB_BITRATE_START_RANGE        62
#define DS_EPS_10MB_BITRATE_END_RANGE          161
#define DS_EPS_10MB_BITRATE_MULTIPLE           10000

#define DS_EPS_100MB_BITRATE_MINIMUM_VALUE      1600000
#define DS_EPS_100MB_BITRATE_START_RANGE        162
#define DS_EPS_100MB_BITRATE_END_RANGE          246
#define DS_EPS_100MB_BITRATE_MULTIPLE           100000

#define DS_EPS_EXT_BITRATE_MAXIMUM_VALUE      256000
#define DS_EPS_EXT2_BITRATE_MAXIMUM_VALUE     10000000
#define DS_EPS_APP_BITRATE_MAXIMUM_VALUE      DS_EPS_EXT2_BITRATE_MAXIMUM_VALUE
#define DS_EPS_INVALID_EXT_BITRATE            0xFFFFFF
#define DS_EPS_MAX_1_OCTET_BITRATE            8640

#define DS_EPS_MAX_1_OCTET_BIT_VALUE          254
#define DS_EPS_0KB_BITRATE_VALUE              255
#define DS_EPS_EXT_MAX_BIT_VALUE              250
#define DS_EPS_EXT2_MAX_BIT_VALUE             250
#define DS_EPS_EXT2_MIN_BIT_VALUE             0

#define DS_EPS_QOS_QCI_MIN_LEN_FOR_DATA       1
#define DS_EPS_QOS_QCI_LEN_FOR_DATA           5
#define DS_EPS_QOS_QCI_LEN_FOR_EXT_DATA       9 
#define DS_EPS_QOS_QCI_LEN_FOR_EXT2_DATA      13 

/*-------------------------------------------------------------------------
Cookie to ensure that dynamic memory allocated is valid
---------------------------------------------------------------------------*/
#define EPS_BEARER_MANAGER_VALID_COOKIE  0xebeaebea

#define EPS_BEARER_KBPS_TO_BPS_MULTIPLIER 1000

/*-----------------------------------------------------------------------------
  enum to indentify the flow direction
-----------------------------------------------------------------------------*/
typedef enum
{
  DS_EPS_FLOW_DIR_INVALID        = 0,
  DS_EPS_FLOW_DIR_DOWNLINK       = 1,
  DS_EPS_FLOW_DIR_UPLINK         = 2,
  DS_EPS_FLOW_DIR_BIDIRECTIONAL  = 3
} ds_eps_bearer_mgr_flow_dir_e;

/** LTE QoS Class Identifier (QCI) as defined in 3GPP TS 23.401 Rel8.
*/
typedef enum
{
  DS_PDN_LTE_QCI_0 = 0,   /**< LTE QoS QCI is 0. */
  DS_PDN_LTE_QCI_1 = 1,   /**< LTE QoS QCI is 1. */ 
  DS_PDN_LTE_QCI_2 = 2,   /**< LTE QoS QCI is 2. */
  DS_PDN_LTE_QCI_3 = 3,   /**< LTE QoS QCI is 3. */ 
  DS_PDN_LTE_QCI_4 = 4,   /**< LTE QoS QCI is 4. */
  DS_PDN_LTE_QCI_5 = 5,   /**< LTE QoS QCI is 5. */ 
  DS_PDN_LTE_QCI_6 = 6,   /**< LTE QoS QCI is 6. */
  DS_PDN_LTE_QCI_7 = 7,   /**< LTE QoS QCI is 7. */ 
  DS_PDN_LTE_QCI_8 = 8,   /**< LTE QoS QCI is 8. */
  DS_PDN_LTE_QCI_9 = 9,   /**< LTE QoS QCI is 9. */
  DS_PDN_LTE_QCI_INVALID  /**< LTE QoS QCI is invalid. */
} ds_pdn_lte_qci_e_type;

/*-----------------------------------------------------------------------------
  This structure contains all info needed in the EPS bearer context.
-----------------------------------------------------------------------------*/
struct ds_eps_bearer_manager
{  
  byte                 lbi;                /* linked bearer id */
  byte                 rb_id;              /* RBID corresponding to this EPS bearer */
  boolean              is_registered_pdcp; /* Indicates if bearer is reg'ed with PDCP */
  boolean              pdcp_reg_req_sent;  /* Indicates if bearer dereg with PDCP is requested */
  boolean              pdcp_dereg_req_sent;/* Indicates if bearer dereg with PDCP is requested */
  boolean              dl_wm_reg_with_a2;  /* Indicates if DL wm reg'ed with A2*/
  boolean              deact_req_cached;
  ds_bearer_deact_params_s* cached_deact_params;
} ;

/** @brief Stores the Long Term Evolution (LTE) QoS parameters.
*/
typedef struct
{
  boolean                valid_flg;       /**< Whether the parameters are set 
                                               and valid. */
  ds_pdn_lte_qci_e_type  qci;             /**< QCI value. */
  uint32                 g_dl_bit_rate;   /**< Guaranteed DL bitrate. */
  uint32                 max_dl_bit_rate; /**< Maximum DL bitrate. */
  uint32                 g_ul_bit_rate;   /**< Guaranteed UL bitrate. */
  uint32                 max_ul_bit_rate; /**< Maximum UL bitrate. */
} ds_pdn_lte_qos_params_type;

#ifdef TEST_FRAMEWORK
#error code not present
#endif
/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_INIT

DESCRIPTION
  This function is used to initialize the EPS bearer manager. 

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_bearer_mgr_init( void );

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_IS_ANY_BEARER_NOT_DOWN

DESCRIPTION
  This function checks if any of the bearer is not in DOWN state
  
PARAMETERS  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Returns TRUE if any bearer is not in down state
  FALSE - Otherwise
   
SIDE EFFECTS 
  None.  
===========================================================================*/
boolean ds_eps_bearer_mgr_is_any_bearer_not_down
(
  void
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_REQ_DEACT_FOR_PENDING_BEARERS

DESCRIPTION
  This function initiates call end for bearers that are in COMING_UP state. 
  
PARAMETERS  
  None.
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Returns TRUE if call end was initiated with CM for any pending bearers.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
boolean ds_eps_bearer_mgr_req_deact_for_pending_bearers
(
  void
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_BRING_UP_PDN

DESCRIPTION
  This function initiates call bring up for LTE.
  
PARAMETERS  
  bearer ptr
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
boolean ds_eps_bearer_mgr_bring_up_pdn
(
  ds_bearer_mgr_s   *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_GET_ORIG_PARAMS

DESCRIPTION
  This function retrives the origination parameters for EPS bearer.
  
PARAMETERS  
  bearer ptr
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_eps_bearer_get_orig_params
(
  ds_bearer_mgr_s                 *bearer_mgr_p,
  emm_pdn_connectivity_req_s_type *lte_orig_params
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_TX_UM_DATA_CB

DESCRIPTION
  This function forwards pkts received to the lower layers for
  pdp-ip call.
  
PARAMETERS  
  Bearer ptr 
  DSM item ptr
  meta_info_ptr  - PS meta info ptr
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_eps_bearer_mgr_tx_um_data_cb
(
  ds_bearer_mgr_s    *bearer_mgr_p,
  dsm_item_type     **item_ptr,
  ps_meta_info_type  *meta_info_ptr
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_DEFAULT_BEARER_ACTIVATE_IND

DESCRIPTION
  This function sets up the bearer when bearer activation indication
  is processed.
  
PARAMETERS  
  info_p: LTE info pointer from NAS
    
DEPENDENCIES 
  None.

RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
boolean ds_eps_bearer_mgr_default_bearer_activate_ind
(
  emm_act_default_bearer_context_request_ind_s_type  *info_ptr, 
  ds_bearer_mgr_s                                     *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_GET_FLOW_DIRECTION

DESCRIPTION
  This function returns the direction of the flow in a particular bearer.

PARAMETERS
  bearer_mgr_p - pointer to the bearer context
  
DEPENDENCIES
  None.

RETURN VALUE
ds_eps_bearer_mgr_flow_dir_e - Flow direction for this bearer
  
SIDE EFFECTS
  None.
===========================================================================*/
ds_eps_bearer_mgr_flow_dir_e ds_eps_bearer_mgr_get_flow_direction
(
  ds_bearer_mgr_s*  bearer_mgr_p
);

/*===========================================================================
FUNCTION ds_eps_bearer_mgr_send_qos_indication

DESCRIPTION
  This function sends tehe QCI information of a particular bearer to interested
  clients.
  
PARAMETERS  
    bearer_mgr_p - Bearer Context.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_eps_bearer_mgr_send_qos_indication
(
   ds_bearer_mgr_s *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_SETUP_UL_WM

DESCRIPTION
  This function sets up UL watermark i.e. PS to PDCP watermark.
  
PARAMETERS  
  bearer_mgr_p
    
DEPENDENCIES 
  None.

RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None. 
===========================================================================*/
void ds_eps_bearer_mgr_setup_ul_wm
(
  ds_bearer_mgr_s       *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_SETUP_DL_WM

DESCRIPTION
  This function sets up the downlink watermark
  
PARAMETERS  
  bearer_mgr_p
      
DEPENDENCIES 
  None.

RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_eps_bearer_mgr_setup_dl_wm
(
  ds_bearer_mgr_s       *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_SEND_TRM_PRIORITY_IND

DESCRIPTION  
  This function is used to send TRM Priority Indication to RRC on
  RAB Resstablishment / Release.
      
PARAMETERS 
  1. bearer_mgr_p 
  2. TRM Priority
 
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_bearer_mgr_send_trm_priority_ind
(
  ds_bearer_mgr_s     *bearer_mgr_p,
  lte_trm_priority_e   trm_priority
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_RAB_RE_ESTAB_IND_HDLR

DESCRIPTION
  This function does LTE specific operations that have to be performed on
  RAB re-establishment.

  
PARAMETERS  
  bearer_mgr_p
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_eps_bearer_mgr_rab_re_estab_ind_hdlr
(
  ds_bearer_mgr_s           *bearer_mgr_p,
  emm_drb_setup_ind_s_type  *info_ptr  
);

/*===========================================================================
FUNCTION  DS_EPS_BEARER_MGR_PDCPDL_RAB_REG_CNF_MSG_HDLR

DESCRIPTION
  This function handles the confirmation of the DL watermark registration 
  with PDCP
  
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_mgr_pdcpdl_rab_reg_cnf_msg_hdlr 
(
  msgr_umid_type              msgrtype,
  const msgr_hdr_struct_type *dsmsg
);

/*===========================================================================
FUNCTION  DS_EPS_BEARER_MGR_PDCPUL_RAB_REG_CNF_MSG_HDLR

DESCRIPTION
  This function handles the confirmation of the UL watermark registration 
  with PDCP
  
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_mgr_pdcpul_rab_reg_cnf_msg_hdlr 
(
  msgr_umid_type              msgrtype,
  const msgr_hdr_struct_type *dsmsg
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_DEREGISTER_DL_WITH_PDCP

DESCRIPTION
  This function deregisters downlink WM with PDCP
  
PARAMETERS  
  bearer_mgr_p  - bearer context pointer
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_eps_bearer_mgr_deregister_dl_with_pdcp
(
  ds_bearer_mgr_s       *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_DEREGISTER_UL_WITH_PDCP

DESCRIPTION
  This function deregisters uplink WM with PDCP
  
PARAMETERS  
  bearer_mgr_p  - bearer context pointer
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_eps_bearer_mgr_deregister_ul_with_pdcp
(
  ds_bearer_mgr_s       *bearer_mgr_p
);

/*===========================================================================
FUNCTION  DS_EPS_BEARER_MGR_PDCPDL_RAB_DEREG_CNF_MSG_HDLR

DESCRIPTION
  This function handles the confirmation of the DL watermark deregistration 
  with PDCP
  
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_eps_bearer_mgr_pdcpdl_rab_dereg_cnf_msg_hdlr 
(
  msgr_umid_type              msgrtype,
  const msgr_hdr_struct_type *dsmsg
);

/*===========================================================================
FUNCTION  DS_EPS_BEARER_MGR_PDCPUL_RAB_DEREG_CNF_MSG_HDLR

DESCRIPTION
  This function handles the confirmation of the UL watermark deregistration 
  with PDCP
  
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_bearer_mgr_pdcpul_rab_dereg_cnf_msg_hdlr 
(
  msgr_umid_type              msgrtype,
  const msgr_hdr_struct_type *dsmsg
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_TEAR_DOWN_PDN_CONNECTION

DESCRIPTION
  This function is used to tear down a PDN connection
  This function will decide if it has to send out a PDN Abort request or a 
  PDN Disconnect request 

PARAMETERS
  bearer_context_p  - pointer to the bearer context that is to be used
                      to end the call

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_bearer_mgr_tear_down_pdn_connection
(
  ds_pdn_mgr_s              *pdn_mgr_p,
  ds_bearer_mgr_s           *bearer_mgr_p
);

/*===========================================================================
FUNCTION ds_eps_bearer_mgr_deactivate_last_bearer_if_needed

DESCRIPTION
  This function checks 
  1. If all bearers are down
  2. If all PDN's are down

  If one of the above is not fulfilled a fresh attach is not tried
 
PARAMETERS 
 None
 
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_eps_bearer_mgr_deactivate_last_bearer_if_needed
(
  void
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_DEREG_PDCP

DESCRIPTION
  This function is used to deregister with PDCP.
  
PARAMETERS 
 bearer_mgr_p  - pointer to the bearer context
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
 None
   
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_eps_bearer_mgr_dereg_pdcp
(
  ds_bearer_mgr_s     *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_DEREG_WMK

DESCRIPTION
  This function is used to deregister DS watermarks.
  
PARAMETERS 
 bearer_mgr_p  - pointer to the bearer context that is to be used
                        to tear down the call  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
 None
   
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_eps_bearer_mgr_dereg_wmk
(
  ds_bearer_mgr_s      *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_GO_NULL_HDLR

DESCRIPTION
  This function is used clean up a bearer 
  
PARAMETERS 
 bearer_id  - EPS bearer ID
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
 None
   
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_eps_bearer_mgr_go_null_hdlr
(
  uint8   bearer_id
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_RE_REGISTER_WM_WITH_PDCP

DESCRIPTION
  This function is used to reestablish the Data path when Flow direction has got modified due to Bearer Modify indication
  from NW

PARAMETERS
  flow_dir: previous firection
  modified_flow_dir :new direction after modify
  call_id: Callid for which modify occured

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is all is successful, 
  FALSE otherwise. 
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_bearer_mgr_re_register_wm_with_pdcp
(
  ds_bearer_mgr_s                     *bearer_mgr_p,
  ds_eps_bearer_mgr_flow_dir_e         bearer_flow_dir,
  ds_eps_bearer_mgr_flow_dir_e         modified_bearer_flow_dir
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_GET_DATA_ACTIVITY

DESCRIPTION
  This function is used to get the data activity per bearer

PARAMETERS
  bearer_mgr_p     - pointer to the bearer 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if any activity
  FALSE otherwise. 
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_bearer_mgr_get_data_activity
(
  ds_bearer_mgr_s  *bearer_mgr_p,
  ds_pdn_mgr_s     *pdn_mgr_p
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_MAP_EMM_CAUSE_CODES_TO_DS

DESCRIPTION
  Converts NAS EMM cause codes to DS net down reason type.  

PARAMETERS
  *ds_down_reason  - ptr to location holding converted cause-code for DS
  esm_cause           - ESM cause code to be converted
  local_esm_cause   - local ESM cause code to be converted

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_bearer_mgr_map_emm_cause_codes_to_ds
(
  ds_net_down_reason_type *ds_down_reason,
  emm_failure_type         emm_failure_cause
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_MAP_ESM_CAUSE_CODES_TO_DS

DESCRIPTION
  Converts NAS ESM cause codes to DS net down reason type.  

PARAMETERS
  *ds_down_reason  - ptr to location holding converted cause-code for DS
  esm_cause           - ESM cause code to be converted
  local_esm_cause   - local ESM cause code to be converted

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_bearer_mgr_map_esm_cause_codes_to_ds
(
  ds_net_down_reason_type *ds_down_reason,
  esm_cause_T              esm_cause,
  lte_nas_local_cause_T    esm_local_cause
);


/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_ISIM_REFERSH_VOTING_HDLR

DESCRIPTION
  This function handles isim refresh voting event for LTE mode.

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
boolean - Whether ISIM fresh can proceed or not
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_mgr_isim_refresh_voting_hdlr();

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_EXAMINE_IF_THROT_NEEDED

DESCRIPTION
  This function is used to examine the cause code of rejection and determine
  if we need to perform throttling.

PARAMETERS
  call_info_ptr  - Pointer to the call info to find out rejection cause

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if throttling is needed
  FALSE otherwise. 
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_bearer_mgr_examine_if_throt_needed
(
  ds_bearer_deact_params_s *lte_call_info_ptr,
  ds_3gpp_throt_rab_info_s  *rab_throt_info_ptr
);

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_DEDICATED_BEARER_ACTIVATE_IND

DESCRIPTION
  This function sets up the bearer when dedicated bearer activation indication
  is processed.
  
PARAMETERS  
  info_p: LTE info pointer from NAS
    
DEPENDENCIES 
  None.

RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
boolean ds_eps_bearer_mgr_dedicated_bearer_activate_ind
(
  emm_act_dedicated_bearer_context_request_ind_s_type  *info_ptr, 
  ds_bearer_mgr_s                                      *bearer_mgr_p
);

#endif /* DS_EPS_BEARER_MANAGER_H */
