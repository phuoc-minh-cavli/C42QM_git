#ifndef DS_PDN_CP_DATA_HDLR_H
#define DS_PDN_CP_DATA_HDLR_H

/*===========================================================================
                      DS_PDN_CP_DATA_HDLR.H
DESCRIPTION
This file exposes PDN CP data handler through APIs and data structures.

Copyright (c) 2019 - 2020 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_pdn_cp_data_hdlr.h#9 $
  $DateTime: 2023/03/23 03:53:38 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_pdn_manager.h"
#include "ds_pdn_rate_ctrl_common.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*-----------------------------------------------------------------------
  Enum which lists the status codes returned when Uplink data packet is
  processed.
  DS_PDN_CP_DATA_SUCCESS        - Uplink data packet transmitted over SRB
  DS_PDN_CP_DATA_DRB_SETUP_REQD - DRB Setup required as CP Data threshold
                                   was reached
  DS_PDN_CP_DATA_WAIT_FOR_ACK   - Cannot send CP Data as Ack for previous
    							   packet is not received
  DS_PDN_CP_DATA_INVALID_ARG    - Invalid Argument Passed to the function
  DS_PDN_CP_DATA_EXCEEDED_SPLMN_RATE_CTRL - MO CP Data Transmissions 
                                             exceeded the Serving PLMN's
                                             Rate Control value - cannot
                                             send over SRB.
------------------------------------------------------------------------*/
typedef enum
{
  DS_PDN_CP_DATA_NOT_SPECIFIED = 0,
  DS_PDN_CP_DATA_SUCCESS,
  DS_PDN_CP_DATA_DRB_SETUP_REQD,
  DS_PDN_CP_DATA_WAIT_FOR_ACK,
  DS_PDN_CP_DATA_INVALID_ARG,
  DS_PDN_CP_DATA_EXCEEDED_SPLMN_RATE_CTRL,
  DS_PDN_CP_DATA_ROHC_FAILED,
  DS_PDN_CP_DATA_MO_EXCEPTION_DATA_ONLY,
  DS_PDN_CP_DATA_INVALID_STATE
} ds_pdn_cp_data_send_status_type;

/*-----------------------------------------------------------------------
 Defines the Maximum number of Bearers supported for NB-IOT
------------------------------------------------------------------------*/
#define DS_PDN_CP_DATA_MAX_BEARER_SUPPORTED 2

/*-----------------------------------------------------------------------
 Defines the max number of control plane data messages that can be sent 
 without receiving L2 ack. The max value is calculated using the below
 formula: 2 * ( [Filtered uplink TB size] / [smallest uplink packet size] ).
 Maximum Filtered uplink TB size for NB1 = 125 bytes
 Smallest uplink packet size = 42 bytes
------------------------------------------------------------------------*/
#define DS_PDN_CP_DATA_MAX_SENDING_WINDOW_SIZE 32

/*-----------------------------------------------------------------------
 Defines the default number of control plane data messages that can 
 be sent without receiving L2 ack
------------------------------------------------------------------------*/
#define DS_PDN_CP_DATA_DEF_SENDING_WINDOW_SIZE 6

#define DS_PDN_CP_ONLY_IE_BIT 0x01

#define DS_PDN_CP_DATA_DEF_MTU_LEN 1358 

#define DS_PDN_CP_DATA_MAX_SEND_CPDATA_RETRIES 4

#define DS_PDN_CP_ONLY_ENABLED(cp_only_ie) \
  (((cp_only_ie.valid == TRUE) && \
    ((cp_only_ie.cp_only_indication & DS_PDN_CP_ONLY_IE_BIT) != 0)) \
     ?TRUE:FALSE) 

#define DS_PDN_IS_MO_EX_PKT(item_ptr)      \
  (((item_ptr != NULL) && \
    (item_ptr->app_field == DS_PDN_MO_EXCEPTIONAL_DATA)) \
     ?TRUE:FALSE)

/*-----------------------------------------------------------------------
  DS_PDN_CP_DATA_MIN_SPLMN_RATE_CTRL_VAL - If network provides any
                      value for the Serving PLMN Rate Control that is less
                      than 10 or 0xA, then hardcode the value to 0xA.
  DS_PDN_CP_DATA_MAX_SPLMN_RATE_CTRL_VAL - This represents the maximum
                      value that the Serving PLMN Rate Control could be.
                      This is essentially saying no rate control.
  DS_PDN_CP_DATA_SPLMN_RATE_CTRL_TMR_VAL - This is the hardcoded timer
                      value for Serving PLMN Rate Control.  We can only
                      send Serving PLMN Rate Control Value number of
                      data packets every 6 minutes according to
                      3GPP TS 23.401.
------------------------------------------------------------------------*/
#define DS_PDN_CP_DATA_MIN_SPLMN_RATE_CTRL_VAL   0xA
#define DS_PDN_CP_DATA_MAX_SPLMN_RATE_CTRL_VAL   0xFFFF
#define DS_PDN_CP_DATA_SPLMN_RATE_CTRL_TMR_VAL   6*60000
#define DS_PDN_CP_DATA_RETRY_TIMER_VAL           500  /* in mili-seconds */

/*-----------------------------------------------------------------------
  DS_PDN_CP_DATA_RAI - Mask used to indicate QMI is sending RAI info.
  DS_PDN_CP_DATA_NAT - Mask used to indicate QMI is sending Next Activity timer.
------------------------------------------------------------------------*/
#define DS_PDN_CP_DATA_RAI   0x1
#define DS_PDN_CP_DATA_NAT   0x2

/*----------------------------------------------------------------------- 
  Command structure for commands posted to cp_data_hdlr 
------------------------------------------------------------------------*/
typedef struct
{
  int bearer_index;
} ds_pdn_cp_data_hdlr_cmd_type; 

/*----------------------------------------------------------------------- 
  Structure to maintain all MO Exception Data related information per bearer.
  - blocked/throttled apn associated with this bearer
  - whether global plmn is blocked (T3396 due to
          cause code #26 insufficient resources)
  - number of mo exception data packets enqueued
------------------------------------------------------------------------*/ 
typedef struct
{
  boolean                           global_plmn_is_blocked; 
  uint64                            num_mo_ex_data_pkts;
} ds_pdn_cp_data_mo_ex_data_info_s;

typedef enum
{
  DS_PDN_CP_DATA_MO_EXCP_FORCE_ATTACH_NOT_ALLOWED = 0,
  DS_PDN_CP_DATA_MO_EXCP_FORCE_ATTACH_ALLOWED,
  DS_PDN_CP_DATA_MO_EXCP_FORCE_ATTACH_IN_PROGRESS,
  DS_PDN_CP_DATA_MO_EXCP_FORCE_ATTACH_UNSUPPORTED
} ds_pdn_cp_data_mo_ex_force_attach_result_type;

typedef enum
{
  DS_PDN_CP_DATA_V6_EXTN_HDR_H2H      = 0,  /* Hop-by-Hop Options */
  DS_PDN_CP_DATA_V6_EXTN_HDR_TCP      = 6,  /* Upper Layer TCP */
  DS_PDN_CP_DATA_V6_EXTN_HDR_UDP      = 17, /* Upper Layer UDP */
  DS_PDN_CP_DATA_V6_EXTN_HDR_ICMP6    = 58, /* Upper Layer ICMP6 */
  DS_PDN_CP_DATA_V6_EXTN_HDR_ROUTING  = 43,  /* Routing */
  DS_PDN_CP_DATA_V6_EXTN_HDR_FRAGMENT = 44, /* Fragment*/
  DS_PDN_CP_DATA_V6_EXTN_HDR_AH       = 51, /* Authentication Header (AH) */
  DS_PDN_CP_DATA_V6_EXTN_HDR_ESP      = 50, /* Encapsulating Security Payload (ESP) */
  DS_PDN_CP_DATA_V6_EXTN_HDR_DEST_OP  = 60, /* Destination Options (before upper-layer header) */
  DS_PDN_CP_DATA_V6_EXTN_HDR_MOBILITY = 135,/* Mobility (currently without upper-layer header) */
  DS_PDN_CP_DATA_V6_EXTN_HDR_HIP      = 139,/* Host Identity Protocol */
  DS_PDN_CP_DATA_V6_EXTN_HDR_SHIM6    = 140,/* Shim6 Protocol */
  DS_PDN_CP_DATA_V6_EXTN_HDR_INVALID  = 255
} ds_pdn_cp_data_v6_extn_hdr_type;


/*----------------------------------------------------------------------- 
  CP Data Hdlr Serving PLMN Rate Control Information
  valid;                --- whether the SPLMN timer value is valid.  If
                            SPLMN Rate Control value is not valid, then
                            code should behave as if there is no rate
                            control.  Otherwise, the value, counter,
                            timer, etc all become relevant.  It is
                            considered to be valid when NW provides
                            valid SPLMN information and supports CP CIOT.
  rate_ctrl_info       --- container to store rate control related data.
  pkt_counter          --- used strictly only for IOCTL 
                           DS_PDN_IOCTL_GET_SPLMN_RATE_CONTROL_INFO
------------------------------------------------------------------------*/
typedef struct
{
  boolean                   valid;
  ds_pdn_rate_ctrl_info_s   rate_ctrl_info;
  uint16                    pkt_counter;
} ds_pdn_cp_data_splmn_rate_ctrl_info_s;

/*----------------------------------------------------------------------- 
  Structure to maintain all CP Only related information per bearer
  cp_only_ind_T structure contains to fields
  - valid - indicates whether the cp only value is valid or not
  - value - indicates what the cp only value is
------------------------------------------------------------------------*/ 
typedef struct
{
  cp_only_ind_T cp_only;
} ds_pdn_cp_data_cp_info_s;

/*----------------------------------------------------------------------- 
  Structure to maintain all ROHC related information per bearer as passed
  to use from MMCP
  hc_config_T structure contains the fields
  boolean     valid;
  byte        len;
  byte        rohc_profile;
  byte        max_cid[2];
  byte        hc_additional_parameters[1];
  byte        hc_ctx_setup_container[MAX_CP_SETUP_LEN];
------------------------------------------------------------------------*/ 
typedef struct
{
  hc_config_T hc_config;
} ds_pdn_cp_data_rohc_info_s;

typedef PACKED struct PACKED_POST
{
  uint8                   rai_timer;       /* Used as hysteresis timer before discarding RAI indication (value in seconds) */
  uint8                   rai_delay_thres; /* If next activity time is earlier than this timer value, RAI will be dropped (value in seconds) */
  uint16                  reserved_byte;
} ds_pdn_rai_config_type;

/*---------------------------------------------------------------------------
  Argument to be passed with  PS_IFACE_IOCTL_SET_SUBSEQUENT_DATA_TRANSFER_STATUS
---------------------------------------------------------------------------*/
typedef enum
{
  DS_PDN_RAI_NO_INFO_AVAIL            = 0x00,
  DS_PDN_RAI_NO_UL_DL_DATA           = 0x01,
  DS_PDN_RAI_SINGLE_DL_DATA           = 0x02,
  DS_PDN_RAI_MAX_VAL                  = DS_PDN_RAI_SINGLE_DL_DATA,
  DS_PDN_RAI_INFO_INVALID             = 0xff
}ds_pdn_rai_ind_enum_type;

/*----------------------------------------------------------------------- 
  Structure to maintain DDXinfor per bearer
  - value         - RAI value 
  - bearer_inst   - indicates the bearer for which curr RAI bit is set
  - rai_timer     - hysteresis timer for sending RAI
  - timer_running - flag to indicate RAI timer is running
  - next_act_time - provide the absolute next activity time
  - rai_pkt_seq_num contains the seq num of pkt in which RAI is included
------------------------------------------------------------------------*/ 
typedef struct
{
  ds_pdn_rai_ind_enum_type                    rai_val;
  int                                         bearer_inst;
  ps_timer_handle_type                        rai_timer;
  boolean                                     timer_running;
  uint64                                      next_act_time;
  uint8                                       rai_pkt_seq_num;
} ds_pdn_cp_data_ddx_info_s;

/*----------------------------------------------------------------------- 
  Structure to maintain informaton for an individual bearer
------------------------------------------------------------------------*/ 
typedef struct
{
  ds_pdn_cp_data_splmn_rate_ctrl_info_s      splmn_rate_ctrl_info;
  ds_pdn_cp_data_cp_info_s                   cp_info;
  ds_pdn_cp_data_rohc_info_s                 rohc_info;
  ds_pdn_cp_data_mo_ex_data_info_s           mo_ex_data_info;
} ds_pdn_cp_data_per_bearer_info_s;

/*----------------------------------------------------------------------- 
  Structure to send RAI & next activity timer from QMI
------------------------------------------------------------------------*/
typedef struct
{
  uint8                       subseq_data_mask;
  ds_pdn_rai_ind_enum_type    rai_val;
  uint8                       next_activity_timer;
} ds_pdn_cp_data_subseq_data_info_s;

/*----------------------------------------------------------------------- 
  App field definitions for enqueueing DSM packets with
  special app fields defined
-----------------------------------------------------------------------*/
#define DS_PDN_CP_DATA_ROHC_FB      1
#define DS_PDN_MO_EXCEPTIONAL_DATA  2

/*=============================================================================
  FUNCTION DS_3GPP_CP_DATA_ROHC_IS_ENABLED

  DESCRIPTION
    This function tells whether CP Data ROHC is enabled
  
  PARAMETERS   
    None.
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    TRUE, if enabled
    FALSE, otherwise
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/        
boolean ds_pdn_cp_data_rohc_is_enabled
(
  void
);

/*=============================================================================
  FUNCTION DS_3GPP_CP_DATA_IS_ENABLED

  DESCRIPTION
    This function tells whether CP Data is enabled
  
  PARAMETERS   
    None.
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    TRUE, if enabled
    FALSE, otherwise
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/        
boolean ds_pdn_cp_data_is_enabled
(
  void
);

/*=============================================================================
  FUNCTION DS_3GPP_CP_DATA_SPLMN_RATE_CTRL_IS_VALID

  DESCRIPTION
    This function checks to see if the current splmn rate control value is
    exceeded.
  
  PARAMETERS   
    None.
 
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
=============================================================================*/
boolean ds_pdn_cp_data_splmn_rate_ctrl_is_valid
(
  int bearer_index
);

/*=============================================================================
  FUNCTION ds_pdn_cp_data_get_splmn_rate_ctrl_info

  DESCRIPTION
    This function gets current splmn rate control info for given bearer.
  
  PARAMETERS   
    Bearer index.
    Pointer to SPMN info.
 
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
=============================================================================*/
void ds_pdn_cp_data_get_splmn_rate_ctrl_info
(
  int                                     bearer_index,
  ds_pdn_cp_data_splmn_rate_ctrl_info_s  *splmn_info
);

/*===========================================================================
  FUNCTION DS_3GPP_CP_DATA_GET_CURRENT_PLMN

  DESCRIPTION
    This function returns the current plmn stored
  
  PARAMETERS   
    None.
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
sys_plmn_id_s_type ds_pdn_cp_data_get_current_plmn
(
  void
);
/*===========================================================================
  FUNCTION DS_3GPP_CP_DATA_SET_CURRENT_PLMN

  DESCRIPTION
    This function returns the current plmn stored.  This is only to restore the
    previous PLMN state when we exit PSM so we have something to compare to to
    detect that a PLMN has changed.  In all other cases, PLMN change should
    go through the proper PLMN changed processing.  
  
  PARAMETERS   
    None.
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_set_current_plmn
(
  sys_plmn_id_s_type new_plmn
);


/*=========================================================================
  FUNCTION DS_3GPP_CP_DATA_GET_BEARER_STATE_INFO

  DESCRIPTION
    This function returns the CP CIOT and Serving PLMN Rate Control state
    for the current bearer that the UE is using to send data over the
    Control Plane.
  
  PARAMETERS   
    integer representing the bearer index that we are looking for
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    ds_pdn_cp_data_per_bearer_info_s - contains all the current CP CIOT
          and Serving PLMN Rate Control Information for the current bearer.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
ds_pdn_cp_data_per_bearer_info_s * ds_pdn_cp_data_get_bearer_state_info
(
  int bearer_index
);

/*===========================================================================
  FUNCTION DS_3GPP_CP_DATA_PROCESS_PLMN_CHANGED

  DESCRIPTION
    This function determines whether the plmn has changed since the last
    bearer activation where all the CP CIOT SPLMN information was saved.
  
  PARAMETERS   
    None.
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_process_plmn_changed
(
  sys_plmn_id_s_type   new_plmn_id
);

/*===========================================================================
  FUNCTION ds_pdn_cp_data_ack_is_pending

  DESCRIPTION
    This function tells whether Ack for the previous UL ESM Data transport
    message is pending.
  
  PARAMETERS   
    None.
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    TRUE, if Ack is pending
    FALSE, otherwise
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
boolean ds_pdn_cp_data_ack_is_pending
(
  void
);

/*===========================================================================
  FUNCTION DS_3GPP_CP_DATA_SET_ACK_IS_PENDING

  DESCRIPTION
    This function records whether Ack is pending for the previous UL ESM Data
    Transport Message
  
  PARAMETERS   
    ack_is_pending flag
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
 
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_set_ack_is_pending
(
  boolean ack_is_pending
);

/*===========================================================================
  FUNCTION DS_3GPP_CP_DATA_SEND_OVER_SRB

  DESCRIPTION
    This function checks whether Data can be sent over Control Plane. If yes,
    it send data to CM. If not, it returns an appropriate error code to
    the caller of this function.
  
  PARAMETERS   
    wm_p           - Pointer to the Um tx Watermark
    call_back_data - Callback data which has the Bearer Context Pointer
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    ds_pdn_cp_data_send_status_type - Enum indicating whether Data was
    successfully sent over Control Plane
 
  SIDE EFFECTS 
    None.
  
===========================================================================*/
ds_pdn_cp_data_send_status_type ds_pdn_cp_data_send_over_srb
(
  dsm_watermark_type                *wm_p,
  void                              *call_back_data
);

/*===========================================================================
  FUNCTION DS_PDN_CP_DATA_PROCESS_CIOT_STATUS_CHG

  DESCRIPTION
    This function processes CIOT Status Change Information received in
	CM SS Event
  
  PARAMETERS   
   ciot_status - CIOT Status FLag
 
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_process_ciot_status_chg
(
  sys_neg_ciot_cap  neg_ciot_cap
);

/*===========================================================================
  FUNCTION DS_3GPP_CP_DATA_POWER_UP_INIT

  DESCRIPTION
    Power up Initialization of CP Data Handler
  
  PARAMETERS
  is_init          : Is it part of power up init
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    Bearer Context Pointer on which to send data
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_power_up_init
(
  boolean is_init
);

/*===========================================================================
  FUNCTION DS_PDN_CP_DATA_ALLOC_BEARER_INFO

  DESCRIPTION
    Allocate and initialize per bearer info.
  
  PARAMETERS   
    Bearer ptr
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None
  
  SIDE EFFECTS 
    None.  
===========================================================================*/
void ds_pdn_cp_data_alloc_bearer_info
(
  ds_bearer_mgr_s*     bearer_mgr_p
);

/*===========================================================================
  FUNCTION DS_PDN_CP_DATA_FREE_BEARER_INFO

  DESCRIPTION
    Free per bearer info.
  
  PARAMETERS   
    Bearer index
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None
  
  SIDE EFFECTS 
    None.  
===========================================================================*/
void ds_pdn_cp_data_free_bearer_info
(
  uint8 index
);

/*=============================================================================
  FUNCTION DS_3GPP_CP_DATA_SAVE_SPLMN_RATE_CTRL_INFO_PER_BEARER

  DESCRIPTION
    This function processes the Serving PLMN Rate Information from the LTE
    Call Info that CM sends to DS.  It decodes the Rate Control Value, and
    saves it per bearer instance.  It also initializes/resets counters, etc.
  
  PARAMETERS   
   int              - Bearer index
   splmn_rate_ctrl  - Serving PLMN Rate Information (raw, not decoded yet)
 
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
=============================================================================*/
void ds_pdn_cp_data_save_cpciot_info_per_bearer
(
 emm_act_default_bearer_context_request_ind_s_type *info_ptr,
 ds_bearer_mgr_s                                   *bearer_mgr_p,
 boolean                                            is_psm_mode
);

/*=============================================================================
  FUNCTION DS_PDN_CP_DATA_RESET_SPLMN_RATE_CTRL_INFO_PER_BEARER

  DESCRIPTION
    This function resets the Serving PLMN Rate Control Information per bearer.
  
  PARAMETERS   
    bearer_index - the index corresponding to the bearer instance we are
                   resetting.
 
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
=============================================================================*/
void ds_pdn_cp_data_reset_splmn_rate_ctrl_info_per_bearer
(
  ds_bearer_mgr_s    *bearer_mgr_p
);

/*=============================================================================
  FUNCTION DS_3GPP_CP_DATA_RESET_ALL_BEARERS_SPLMN_RATE_CTRL_INFO

  DESCRIPTION
    This function resets the Serving PLMN Rate Control Information for all
    bearers stored in the Serving PLMN Rate Control Bearer Table
  
  PARAMETERS   
    None
 
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
=============================================================================*/
void ds_pdn_cp_data_reset_all_bearers_splmn_rate_ctrl_info
(
   void
);

/*=========================================================================
  FUNCTION DS_PDN_CP_DATA_SPLMN_RATE_CTRL_TIMER_CB

  DESCRIPTION
    This function is the callback function that posts the command
    DS_CMD_3GPP_CP_DATA_SPLMN_RATE_CTRL_TMR_EXP when the Serving PLMN
    Rate Control timer has expired.
  
  PARAMETERS
    None.
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_splmn_rate_ctrl_timer_cb
(
  void* cb_data /* Pass bearer index here */
);

/*=========================================================================
  FUNCTION DS_3GPP_CP_DATA_PROCESS_SPLMN_RATE_CTRL_TIMER_EXPIRY

  DESCRIPTION
    This function processes the Serving PLMN Rate Control Timer Expiry
    and restarts the timer
  
  PARAMETERS
    None.
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_process_splmn_rate_ctrl_timer_expiry
(
  ds_pdn_rate_ctrl_info_s    *rate_ctrl_info_ptr
);

/*=========================================================================
  FUNCTION DS_PDN_CP_DATA_PROCESS_RAI_TIMER_EXPIRY

  DESCRIPTION
    This function processes the CP data rai Timer Expiry
  
  PARAMETERS
    None.
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/

void ds_pdn_cp_data_process_rai_timer_expiry
(
  void
);

/*=========================================================================
  FUNCTION DS_PDN_CP_DATA_PROCESS_RETRY_TIMER_EXPIRY

  DESCRIPTION
    This function processes the CP data retry Timer Expiry
  
  PARAMETERS
    None.
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_process_retry_timer_expiry
(
  void *cmd_payload_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_CP_DATA_HANDLE_BEARER_ACTIVATION

  DESCRIPTION
    This function ensures that when bearer activate is called, special
    call flow for CP CIOT for bringing up the bearer is executed
  
  PARAMETERS   
   bearer context
 
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    boolean for success or failure
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
boolean ds_pdn_cp_data_handle_bearer_activation
(
  ds_bearer_mgr_s                   *bearer_context_p
);

/*===========================================================================
FUNCTION DS_3GPP_CP_DATA_PROCESS_CALL_END

DESCRIPTION
  This function processes the CAll END command for the give bearer instance

PARAMETERS
  Bearer Instance
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_cp_data_process_call_end
(
 ds_bearer_mgr_s    *bearer_mgr_p
);

/*===========================================================================
FUNCTION DS_3GPP_CP_DATA_POPULATE_PDN_CONN_PARAMS

DESCRIPTION
  This function populates the Header compressition configuration info
  in PDN connectivity request

PARAMETERS
  Bearer Instance
  Call Origination Pointer

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_cp_data_populate_pdn_conn_params
(
 ds_bearer_mgr_s                 *bearer_mgr_p,
 emm_pdn_connectivity_req_s_type *lte_ps_orig_params_p
);

/*===========================================================================
FUNCTION DS_3GPP_CP_DATA_PROCESS_RAB_RELEASE

DESCRIPTION
  This function processes the RAB Release indication for the given bearer
  instance

PARAMETERS
  Bearer Instance
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_cp_data_process_rab_release
(
  int                     bearer_inst
);

/*===========================================================================
FUNCTION DS_3GPP_CP_DATA_PROCESS_RAB_REESTAB

DESCRIPTION
  This function processes the RAB Re-establish indication for the given bearer
  instance

PARAMETERS
  Bearer Instance
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_cp_data_process_rab_reestab
(
 uint8                     bearer_inst
);

/*===========================================================================
FUNCTION DS_3GPP_CP_DATA_PROCES_BEARER_MODIFY_REQ

DESCRIPTION
  This function processes the RAB Re-establish indication for the given bearer
  instance

PARAMETERS
  Bearer Instance
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_cp_data_process_bearer_modify_req
(
  int                     bearer_inst
);

/*===========================================================================
  FUNCTION DS_PDN_CP_DATA_SET_MO_EXCEPTION_SUPPORT

  DESCRIPTION
    This function saves the state of whether MO Exception is supported or not.
  
  PARAMETERS   
    MO exception flag - boolean
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_set_mo_exception_support
(
  boolean                mo_exception_supported
);

/*===========================================================================
  FUNCTION DS_PDN_CP_DATA_IS_MO_EXCEPTION_DATA_SUPPORTED

  DESCRIPTION
    This function returns whether MO Exception data is enabled in EFNAS
  
  PARAMETERS   
    None.
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    Boolean
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
boolean ds_pdn_cp_data_is_mo_exception_data_supported
(
  void
);


/*===========================================================================
  FUNCTION DS_PDN_CP_DATA_IS_MO_EXCEPTION_DATA_SUPPORTED_ON_PDN

  DESCRIPTION
    This function returns whether MO Exception data is supported by this PDN
  
  PARAMETERS   
    PDN ptr.
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    Boolean
  
  SIDE EFFECTS 
    None.
===========================================================================*/
boolean ds_pdn_cp_data_is_mo_exception_data_supported_on_pdn
(
  ds_pdn_mgr_s  *pdn_mgr_p
);


/*===========================================================================
  FUNCTION DS_PDN_CP_DATA_SET_GLOBALLY_THROTTLED_STATE_PER_APN

  DESCRIPTION
    This function sets the globally throttled state per bearer matching
    specified APN name.
  
  PARAMETERS   
    APN name
    Throttling flag
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_set_globally_throttled_state_per_apn
(
  byte    *throt_apn,
  boolean  global_throttle_state
);

/*===========================================================================
  FUNCTION DS_3GPP_CP_DATA_BEARER_CNTXT_REORIG_DORM_FROM_APN

  DESCRIPTION
    This function finds the bearer context matching the APN and is used to
    dequeue from watermark after T3396 timer is cleared so that normal
    data can be triggered to be sent over control plane.
  
  PARAMETERS   
    APN
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_bearer_cntxt_reorig_dorm_from_apn
(
   byte    *apn
);

/*===========================================================================
  FUNCTION DS_3GPP_CP_DATA_PENDING_MO_EX_DATA_AVAIL

  DESCRIPTION
    This function returns whether or not there is pending MO exception data
    in the watermark for control plane.
  
  PARAMETERS   
    bearer index
    subs id
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    TRUE if MO exception data is supported and CP MO exception data is pending
    FALSE otherwise
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
boolean ds_pdn_cp_data_pending_mo_ex_data_avail
(
  int bearer_index
);

/*===========================================================================
  FUNCTION DS_PDN_CP_DATA_FLUSH_NUM_MO_EX_DATA_CNT

  DESCRIPTION
    This function flushes the counter holding number of MO exception data.
    Currently, this is used when DRB is successfully brought up.
  
  PARAMETERS   
    bearer index
    subs id
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_flush_num_mo_ex_data_cnt
(
 uint8             bearer_index
);

/*===========================================================================
  FUNCTION DS_3GPP_CP_DATA_INCREMENT_NUM_MO_EX_DATA_CNT

  DESCRIPTION
    This function increments the counter holding number of MO exception data.
    Currently, this is used for when MO exception data packets are enqueued
    and IFACE is in UP DORMANT state for control plane data transfer.
  
  PARAMETERS   
    bearer index
    subs id
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_increment_num_mo_ex_data_cnt
(
  int bearer_index
);

/*===========================================================================
FUNCTION DS_3GPP_CP_DATA_CLEANUP_INST_AFTER_FORCE_ATTACH_FAILURE

DESCRIPTION
  This function cleans up the ifaces after force attach failure

PARAMETERS
  None 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_cp_data_cleanup_inst_after_force_attach_failure
(
  ds_net_down_reason_type down_reason
);

/*===========================================================================
FUNCTION DS_PDN_CP_DATA_RESUME_INST_UP_FOR_FORCE_ATTACH

DESCRIPTION
  This function calls instance up command again to continue bringing up the
  instance in the below cases
  1. Force attach is successful 
      a. if PDN is different than attach PDN or
      b. we are in attach w/o PDN state with UE is access barred. 
  2. As part of force attach trigger where attach PDN itself is the MoEX PDN

PARAMETERS
  None 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_cp_data_resume_inst_up_for_force_attach
(
  void
);

/*===========================================================================
FUNCTION DS_PDN_CP_DATA_SAVE_FORCE_ATTACH_PARAMS

DESCRIPTION
  This function saves the necessary force attach params for restoration

PARAMETERS
  IFACE pointer - iface to bring up
  INFO pointer - call params. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_cp_data_save_force_attach_params
(
 ds_pdn_mgr_s                     *pdn_mgr_p,
 ds_pdn_mgr_instance_type_e        inst_type,
 ds_pdn_network_params_info_type  *info_p
);

/*===========================================================================
FUNCTION DS_PDN_CP_DATA_IS_FORCE_ATTACH_IN_PROGRESS

DESCRIPTION
  This function returns the flag ds3gpp_is_force_attach_in_progress

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  True if force attach is in progress, False otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_cp_data_is_force_attach_in_progress
(
  void
);

/*===========================================================================
FUNCTION DS_PDN_CP_DATA_SKIP_PDN_REQ_AFTER_FORCE_ATTACH

DESCRIPTION
  This function cleans up the force attach params and avoids
  PDN connectivity after force attach without PDN is complete.

PARAMETERS
  PDN context 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_cp_data_skip_pdn_req_after_force_attach
(
  ds_pdn_mgr_s   *pdn_mgr_p
);

/*===========================================================================
FUNCTION DS_PDN_CP_DATA_TRIGGER_MO_EX_FORCE_ATTACH

DESCRIPTION
  This function triggers force attach by calling the NAS API to trigger
  the force attach.  

PARAMETERS
  PDN context
  Subs ID

DEPENDENCIES
  None.

RETURN VALUE
  DS_PDN_CP_DATA_MO_EXCP_FORCE_ATTACH_NOT_ALLOWED = 0,
  DS_PDN_CP_DATA_MO_EXCP_FORCE_ATTACH_ALLOWED,
  DS_PDN_CP_DATA_MO_EXCP_FORCE_ATTACH_IN_PROGRESS,
  DS_PDN_CP_DATA_MO_EXCP_FORCE_ATTACH_UNSUPPORTED

SIDE EFFECTS
  None.

===========================================================================*/
ds_pdn_cp_data_mo_ex_force_attach_result_type ds_pdn_cp_data_trigger_mo_ex_force_attach
(
  ds_pdn_mgr_s              *pdn_cntx_p
);
/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
  FUNCTION DS_PDN_CP_DATA_IS_EPCO_NV_ENABLED

  DESCRIPTION
    This function checks if EPCO bit in NV is enabled
  
  PARAMETERS   
    None.
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
boolean ds_pdn_cp_data_is_epco_nv_enabled
(
  void
);


/*=========================================================================
  FUNCTION DS_3GPP_CP_DATA_DRB_SETUP_IS_REQD

  DESCRIPTION
    This function determines whether DRB setup is required for the
    given bearer instance.
  
  PARAMETERS
    Bearer Instance
    Tx Watermark Count
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    TRUE, if DRB setup is required
    FALSE, otherwise
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
boolean ds_pdn_cp_data_drb_setup_is_reqd
(
  int    bearer_inst,
  uint32 current_cnt
);

/*===========================================================================
  FUNCTION DS_PDN_CP_DATA_SET_NEXT_ACT_TIMER_IND

  DESCRIPTION
    This function handles the Release Assistance Indication Command from
    the application. 
  
  PARAMETERS   
    Bearer Instance
    Subscription Id
    Release Assistance Indication
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_set_next_act_timer_ind
(
  uint8              next_act_timer
);

/*===========================================================================
  FUNCTION DS_PDN_CP_DATA_SET_RELEASE_ASSISTANCE_IND

  DESCRIPTION
    This function handles the Release Assistance Indication Command from
    the application. 
  
  PARAMETERS   
    Bearer Instance
    Subscription Id
    Release Assistance Indication
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_set_release_assistance_ind
(
  int                                         bearer_inst,
  ds_pdn_rai_ind_enum_type                    rel_assistance_ind
);

/*===========================================================================
  FUNCTION DS_PDN_CP_DATA_SET_NEXT_ACT_TIMER_IND

  DESCRIPTION
    This function handles the Release Assistance Indication Command from
    the application.
  
  PARAMETERS   
    next_act_timer
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_set_next_act_timer_ind
(
  uint8              next_act_timer
);

/*===========================================================================
  FUNCTION DS_PDN_CP_DATA_GET_RELEASE_ASSISTANCE_IND

  DESCRIPTION
    This function handles the Release Assistance Indication Command from
    the application.
  
  PARAMETERS   
    Bearer Instance
    Subscription Id
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    Current Release assistance indication value.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
ds_pdn_rai_ind_enum_type  ds_pdn_cp_data_get_release_assistance_ind
(
  int                      bearer_inst
);

/*===========================================================================
  FUNCTION DS_PDN_CP_DATA_SAVE_ROHC_INFO_TO_PER_BEARER_TBL

  DESCRIPTION
    This function saves the HC info based in bearer activation to be
    saved in our per_bearer_tbl.  This is at least used for PSM
    purposes for saving the HC config and restoring it upon PSM exit.
  
  PARAMETERS   
    None.
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_save_rohc_info_to_per_bearer_tbl
(
   int          bearer_index,
   hc_config_T *new_rohc_info
);

/*===========================================================================
  FUNCTION DS_PDN_CP_DATA_RESET_RELEASE_ASSISTANCE_IND

  DESCRIPTION
    This function handles reset Release Assistance Indication.
  
  PARAMETERS   
    Bearer Instance
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    Current Release assitance indication value.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_reset_release_assistance_ind
(
  uint8              bearer_inst
);

/*===========================================================================
  FUNCTION DS_PDN_CP_DATA_PROCESS_LTE_DATA_TRANSPORT_IND

  DESCRIPTION
    This function processes the LTE Data transport indication received from
    CM.
  
  PARAMETERS   
   *lte_call_info_p - LTE Call Info Pointer
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_process_lte_data_transport_ind
(
  emm_data_transport_ind_type   *data_trans_ind
);


/*===========================================================================
  FUNCTION DS_PDN_CP_DATA_PROCESS_LTE_DATA_TRANSPORT_CNF_INTERNAL

  DESCRIPTION
    This function processes the LTE Data transport confirmation received from
    CM.
  
  PARAMETERS   
   boolean          - Prev data send success
   uint8            - Cnf index               
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_process_lte_data_transport_cnf_internal
(
  boolean                 prev_data_send_success,
  uint8                   cnf_index
);

/*===========================================================================
  FUNCTION DS_PDN_CP_DATA_PROCESS_LTE_DATA_TRANSPORT_CNF

  DESCRIPTION
    This function processes the LTE Data transport confirmation received from
    NAS.
  
  PARAMETERS   
   *data_trans_cnf - Data transport confirmation information
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_process_lte_data_transport_cnf
(
  emm_data_transport_cnf_type   *data_trans_cnf
);

/*===========================================================================
FUNCTION DS_PDN_CP_DATA_IS_DL_DATA_EXPECTED

DESCRIPTION
  This function checks if DL data expected for given bearer index.
 
PARAMETERS
  Bearer index

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_cp_data_is_dl_data_expected
(
  int                      bearer_inst
);

/*===========================================================================
FUNCTION DS_PDN_CP_DATA_IS_UE_CP_ONLY

DESCRIPTION
  This function checks if CP only is enabled for all the bearers.
 
PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_cp_data_is_ue_cp_only();

/*===========================================================================
FUNCTION ds_pdn_cp_data_need_keep_flow_enabled_for_mox_data

DESCRIPTION
  This function checks if rat is NB1 & t3346 timer is running
  and flow needs to be kept enabled for MOX data.
 
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_cp_data_need_keep_flow_enabled_for_mox_data
(
  void
);

/*=========================================================================
  FUNCTION DS_PDN_CP_DATA_PROCESS_L2_NOTIFY_CMD

  DESCRIPTION
    This function processes the notification from L2 to send additional
    UL Packets.
  
  PARAMETERS
    uint8 - max_ul_packets 
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_pdn_cp_data_process_l2_notify_cmd
(
  uint8 max_ul_packets
);

void ds_pdn_cp_data_reset_max_ul_packet_cap
(
  void
);

/*=========================================================================
  FUNCTION DS_PDN_CP_DATA_PROCESS_CPSR_STATUS_CMD

  DESCRIPTION
    This function processes the CPSR status indication from NAS.
  
  PARAMETERS
    emm_cpsr_status indication
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.  
===========================================================================*/
void ds_pdn_cp_data_process_cpsr_status_cmd
(
  emm_cpsr_status_ind_type  *cpsr_status_ind
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif  /* TEST_FRAMEWORK */

uint8 ds_pdn_cp_data_get_barring_cnf_index();

void ds_pdn_cp_data_set_barring_cnf_index(uint8 value);

#ifdef FEATURE_NBIOT_NTN
/*=========================================================================
  FUNCTION ds_pdn_cp_data_trigger_pending_data

  DESCRIPTION
    This function check if any pending data present and retriggers.
  
  PARAMETERS
    None
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    boolean
  
  SIDE EFFECTS 
    None.  
===========================================================================*/
void ds_pdn_cp_data_trigger_pending_data();
#endif /* FEATURE_NBIOT_NTN */
#endif /* DS_PDN_CP_DATA_HDLR_H */
