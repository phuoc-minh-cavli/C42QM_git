/*!
  @file
  ds_eps_tlb.h

  @brief
  Test loopback interface

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009 - 2017 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_eps_tlb.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/02/12   sa      Fixed issue with V4V6 data path.
11/17/11   sa      Added support for V6 Loopback Mode B.
08/10/10   sa      Added support of LTE_TLB_LB_OPEN_IND.
04/21/10   sa      Added support for Loopback Mode B.
08/31/09   ml      Initial revision
===========================================================================*/
#ifdef FEATURE_MODE_TLB

#ifndef DS_EPS_TLB_H
#define DS_EPS_TLB_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_pdn_defs.h"
#include "dsm_pool.h"
#include "lte_tlb.h"
#include "wms.h"
#include "ds_bearer_manager.h"
#include "ps_svc.h"
#include "ds_fwk.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define DS_MAX_TLB_RM_NUM_IFACES 1


typedef  enum
{
  DS_EPS_PDN_EVT = 0x0,
  DS_EPS_MODE_B_TMR_EXP,
  DS_EPS_RM_FLOW_EVT,
  DS_EPS_TLB_CMD_MAX = 0xFF
}ds_eps_tlb_cmd_e_type;


#define DS_EPS_RM_FLOW_ENABLE  0
#define DS_EPS_RM_FLOW_DISABLE 1


/*-----------------------------------------------------------------------------
  This enum describes the current state of test loopback mode
-----------------------------------------------------------------------------*/
typedef enum
{
  DS_EPS_BEARER_TLB_INACTIVE,   /* Test loopback is inactive */
  DS_EPS_BEARER_TLB_ACTIVE,     /* Test loopback is active */
  DS_EPS_BEARER_TLB_A_CLOSED,   /* Test loopback is closed for mode A */
  DS_EPS_BEARER_TLB_B_CLOSED,   /* Test loopback is closed for mode B */
  DS_EPS_BEARER_TLB_C_CLOSED,   /* Test loopback is closed for mode C */
  DS_EPS_BEARER_TLB_G_CLOSED,   /* Test loopback is closed for mode G */
  DS_EPS_BEARER_TLB_H_CLOSED,   /* Test loopback is closed for mode H */
  DS_EPS_BEARER_TLB_I_CLOSED,   /* Test loopback is closed for mode I */
  DS_EPS_BEARER_TLB_B_OPEN,     /* Test loopback is open for Mode B   */
  DS_EPS_BEARER_TLB_C_OPEN,     /* Test loopback is open for Mode C   */
  DS_EPS_BEARER_TLB_I_OPEN,     /* Test loopback is open for Mode I   */
  DS_EPS_BEARER_TLB_MAX
} ds_eps_bearer_tlb_state_e;


/*-----------------------------------------------------------------------------
  This structure holds the state info of test loopback mode B
-----------------------------------------------------------------------------*/
typedef struct
{
  ds_eps_bearer_tlb_state_e  ds_eps_bearer_tlb_state;
  lte_tlb_mode_e       mode; /*!< TLB mode: Mode A or B or C*/
  boolean              buffer_ip_pdu;      /* Buffer_IP_PDU  */
  dword                ip_pdu_delay_time;  /* IP delay in ms */
  boolean              tmr_delay_modeB_running;
  ps_timer_handle_type tmr_delay_modeB;
  boolean              sent_close_loopback_ind;
  ds_bearer_mgr_wm_s   tlb_rx_wm;
}ds_eps_tlb_state_type;


#define DS_EPS_TLB_IPV4_CLIENT 0 /* IPv4 */
#define DS_EPS_TLB_IPV6_CLIENT 1 /* IPv6 */
#define DS_TLB_MODE_B_I 4    /* app type */

/*---------------------------------------------------------------------------

  Structure used for Loopback Mode B command
---------------------------------------------------------------------------*/
typedef struct
{
  ds_pdn_event_enum_type     event;
  ds_pdn_mgr_s*              pdn_ptr;
  ds_fwk_clnt_handle_type    client_handle;
}ds_eps_tlb_iface_event_type;
/*-----------------------------------------------------------------------------
  Subscription Specific Informatio of the TLB Module
------------------------------------------------------------------------------*/
typedef struct
{
  uint32                       cookie;
  /* Test loopback mode state variables */
  ds_eps_tlb_state_type        tlb_state;

  dsm_watermark_type           rm_lo_rx_wmk; 
                        /* watermark for RM loop rx operation */
  q_type                       rm_lo_rx_wmk_q;
                        /* queue for RM Loop rx watermark     */
  boolean                      rm_flow_enabled;
  ps_sig_enum_type             rx_sig;
  boolean                      is_flow_enabled;
  boolean                      init;
                         /* Repetition factor from TLB */
  uint8                        mode_h_repetitions;
  uint8                        mode_h_repetitions_current_cnt;
  ds_bearer_mgr_wm_s           mode_h_attach_wo_pdn_wm;              
} ds_eps_tlb_per_subs_info_type;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*==========================================================================
FUNCTION ds_eps_tlb_get_info_hndl

DESCRIPTION
  This function is used to fetch the TLB information corresponding to
  the given Subs Id
 
PARAMETERS 
  Subs Id - Subscription Id
 
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the TLB Info
  
SIDE EFFECTS
  None.
  
===========================================================================*/
ds_eps_tlb_per_subs_info_type* ds_eps_tlb_get_info_hndl();

/*===========================================================================
FUNCTION DSEPSTLB_INIT

DESCRIPTION
  Initializes test loopback specific handler
  
PARAMETERS  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void dsEpsTlb_init (void);

/*===========================================================================
FUNCTION DSEPSTLB_PDP_IP_TX_UM_DATA_CB

DESCRIPTION
  This function handles uplink data for bearers established in TLB mode. 
  For TLB mode A, all data will be simply discarded. TLB mode B is currently 
  not supported
  
PARAMETERS  
  mode_info_p: mode specific info pointer
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void dsEpsTlb_pdp_ip_tx_um_data_cb
(
  ds_bearer_mgr_s 	  *bearer_mgr_p,
  dsm_item_type		 **item_ptr,
  ps_meta_info_type	  *meta_info_ptr

);

/*===========================================================================
FUNCTION DSEPSTLB_PDP_IP_RX_UM_DROP_DATA_CB

DESCRIPTION
  This funciton dequeues RX watermar and drops data
  
PARAMETERS  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean dsEpsTlb_pdp_ip_rx_um_drop_data_cb
(
  ps_sig_enum_type sig,
  void             *user_data_p
);


/*===========================================================================
FUNCTION DSEPSTLB_MODEB_HDLR_CMD()

DESCRIPTION
  This function is called when we process the command posted to DS as a result
  of loopback ModeB .

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
void dsEpsTlb_modeB_hdlr_cmd
(
  const ds_command_type *cmd_ptr                         /* Pointer to Command */
);


/*===========================================================================
FUNCTION dsEpsTlb_set_tlb_mode_A_params

DESCRIPTION
  This function is used to set the tlb rx watermark nonempty call back 
  function and set the signal handler for TLB Mode A

PARAMETERS
 Bearer Index Pointer

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
  
===========================================================================*/
void dsEpsTlb_set_tlb_mode_A_params
(
  int index
);

/*===========================================================================
FUNCTION ds_eps_tlb_set_tlb_mode_G_params()

DESCRIPTION
  This function is used to set up mode G related parameters and watermarks.

PARAMETERS
 Bearer Index Pointer

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
  
===========================================================================*/

void ds_eps_tlb_set_tlb_mode_G_params
( 
  int index
);

/*===========================================================================
  FUNCTION DS_EPS_TLB_PROCESS_LTE_DATA_TRANSPORT_IND_MODE_G

  DESCRIPTION
    This function processes the LTE Data transport indication received from
    NAS.
  
  PARAMETERS   
   *lte_call_info_p - LTE Call Info Pointer
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_eps_tlb_process_lte_data_transport_ind_mode_G_I
(
  emm_data_transport_ind_type   *lte_call_info_p
);

/*===========================================================================
  FUNCTION DS_EPS_TLB_PROCESS_LTE_DATA_TRANSPORT_CNF_MODE_I

  DESCRIPTION
    This function processes the LTE Data transport confirmation received from
    NAS.
  
  PARAMETERS   
   *lte_call_info_p - LTE Call Info Pointer
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_eps_tlb_process_lte_data_transport_cnf_mode_I
(
  emm_data_transport_cnf_type   *lte_call_info_p
);

/*=========================================================================
  FUNCTION ds_eps_tlb_mode_G_H_data_ack_is_pending

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
boolean ds_eps_tlb_mode_G_H_data_ack_is_pending
(
  void
);

/*===========================================================================
FUNCTION ds_eps_tlb_mode_h_sms_msg_callback()

DESCRIPTION
  This function is a callback from WMS to indicate MT SMS and submit
  report(ACK) for MO SMS.

PARAMETERS
 Event ID
 Info Ptr
 boolean - sharing flag

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
  
===========================================================================*/
void ds_eps_tlb_mode_h_sms_msg_callback
(
  wms_msg_event_e_type             event,        /* Event ID              */
  const wms_msg_event_info_s_type *info_ptr,     /* Pointer to Event info */
  boolean                         *shared
);

/*===========================================================================
FUNCTION ds_eps_tlb_set_tlb_mode_H_params()

DESCRIPTION
  This function is used to set up mode H related parameters and watermarks.

PARAMETERS
 Bearer Index Pointer

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
  
===========================================================================*/

void ds_eps_tlb_set_tlb_mode_H_params
( 
  int index
);

/*===========================================================================
  FUNCTION DS_EPS_TLB_SEND_UL_DATA_MODE_I

  DESCRIPTION
    This function processes the UL data which is looped back from TLB.
  
  PARAMETERS   
   wm       - Watermark ptr.
   cb_data  - Bearer context ptr
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_eps_tlb_send_ul_data_mode_I
(
  dsm_watermark_type   *wm,
  void                 *cb_data
);

/*===========================================================================
  FUNCTION DS_EPS_TLB_SEND_UL_DATA_MODE_H

  DESCRIPTION
    This function processes the UL data which is looped back from TLB.
  
  PARAMETERS   
   wm       - Watermark ptr.
   cb_data  - Bearer context ptr
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_eps_tlb_send_ul_data_mode_H
(
  dsm_watermark_type   *wm,
  void                 *cb_data
);

/*==============================================================================

FUNCTION DS_3GPP_EMBMS_PER_SUBS_INFO_INIT

DESCRIPTION
  This function allocates and initializes TLB information for the given Subs Id.

PARAMETERS
  None
  
DEPENDENCIES  
  None

RETURN VALUE
  None

SIDE EFFECTS  
  None
==============================================================================*/
void ds_eps_tlb_per_subs_info_init
();


/*==========================================================================
FUNCTION ds_eps_tlb_validate_info_hndl

DESCRIPTION
  This function is used to validate the TLB information handle.
 
PARAMETERS 
  hndl    - Pointer to the TLB Information  
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if the handle is valid
  FALSE, otherwise
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_tlb_validate_info_hndl
(
  ds_eps_tlb_per_subs_info_type*  hndl
);

/*==============================================================================
FUNCTION      DS_EPS_TLB_FREE_CTRL_BLK

DESCRIPTION
  This function is used to free the TLB Info block.
  

PARAMETERS
  None

DEPENDENCIES  
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
==============================================================================*/
void ds_eps_tlb_free_info_hndl();


/*==========================================================================
FUNCTION ds_eps_tlb_is_tlb_mode_active

DESCRIPTION
  This function is used to find if the given TLB mode is active or not.
 
PARAMETERS 
  TLB handle
  TLB mode
 
DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE if TLB mode is active
                 FALSE if otherwise.
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_tlb_is_tlb_mode_active
(
  ds_eps_tlb_per_subs_info_type* hndl,
  lte_tlb_mode_e                 tlb_mode
);

/*===========================================================================
FUNCTION DSEPSTLB_PDP_IP_RX_UM_DATA_CB

DESCRIPTION
  This function handles rx data on the bearers which were established in 
  TLB mode. 
  In TLB is closed in mode A, data will be looped back. Otherwise, data will
  be dropped
  
PARAMETERS  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
 boolean dsEpsTlb_pdp_ip_rx_um_data_cb
(
  ps_sig_enum_type sig,
  void             *user_data_p
);

 /*===========================================================================
FUNCTION ds_EpsTlb_register_a2_to_ps

DESCRIPTION
  This function registers the bearer WM with a2 module in TLB Mode A

PARAMETERS
  bearer_context_p       - bearer_context to be registered

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None

===========================================================================*/
void ds_EpsTlb_register_a2_to_ps
(
   ds_bearer_mgr_s                *bearer_ptr
);

#endif /* FEATURE_MODE_TLB */
#endif /* DS_EPS_TLB_H */