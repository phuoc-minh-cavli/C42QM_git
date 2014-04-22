
/*!
  @file
  esmtask.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*==============================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mmcp.mpss/6.1.10/nas/sm/src/esmtask.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/17/12   cogolt  Registered NAS_ESM_UNBLOCK_ALL_APNS_IND with MSGR for Rel-10 ESM backoff
                   timer (T3396) support
07/05/12   us      Removing all static buffers allocated in ESM & adding dynamic mechanism of
                    of allocating buffers when needed.
07/19/12   us      Increasing ESM_CMD_Q_SIZE to 35.
07/17/12   us      Declaring only specific esm structure in esm_post_timer_expired() 
                   instead of whole union esm_msg.
10/10/11   HC      Removed AEEstd.h inclusion as aeestdlib is obsoleted 
                   Replaced the STD_OFFSETOF with local defintion NAS_STD_OFFSETOF
08/04/11   zren    Updated ESM to propagate SM group ID to CM for befault Bearer
                   context activation
11/29/10   MNK     More 1XCSFB changes including adversarial scenarios
10/21/10   MNK     Deactivation of bearer contexts not transferable to GW upon IRAT 
10/08/10   MNK     1XCSFB manager introduction 
10/07/10   MNK     1XCSFB introduction 
06/11/10   zren    Updated to set TI valid flag to TRUE when SM is updating ESM 
                   with PDP/Bearer context activation 
06/01/10   zren    Updated ESM to sync bearer contexts status with EMM when SM 
                   updated ESM
03/26/10   MNK     Add more i-RAT(GSM) support
03/25/10   MNK     i-RAT updates
03/11/10   MNK     Changes for transaction_id optional param in def & ded bearer context requests
02/19/10   MNK     Make some functions static
02/17/10   MNK     Add support for context updates when in UMTS
02/17/10   zren    Discard pending msgs uopon new attach procedure
02/05/10   MNK     Handle error scenario where SIG_CONN release is getting buffered while default bearer is being setup, 
                   resulting in processing the SIG_CONN release after attach/def bearer is successful
02/03/10   zren    Added ESM new timer machanism
12/10/09   zren    Made feature NAS_PDN_CONNECTIVITY_REQ_RSP permanent
11/18/09   zren    Updated to fix bearer context state change logging and detach
                   trigger in case of no default bearer context existing 
11/18/09   zren    Added support for ESM Information procedure
11/03/09   zren    Added support for UE and NW initiated EPS bearer context 
                   modification procedures
10/15/09   MNK     Changes due to CM's new msgr definitions
10/14/09   MNK     Interface change for building and sending the detach cmd
09/29/09   zren    Removed MUTEX and enabled debug messages
09/25/09   MNK     Fix warnings
09/24/09   zren    Updated PDN_CONNECTIVITY and PDN_DISCONNECT features 
09/21/09   MNK     Changes due to IRAT design
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
09/14/09   zren    Added bearer ID and ESM cause check for PDN DISCONNECT 
                   REQ/REJ messages
08/18/09   MNK     CM-NAS interface changes
08/14/09   MNK     Add Support for PDN disconnect request
05/21/09   MNK     Added support to reply back to CM/DS in case the NW sent bad messages as a response to UE initiated request
05/20/09   MNK     Provide a callback function for the default contexts to call to deactivate associated dedicated conetxts
05/08/09   MNK     Added support for bearer resource allocation procedures
05/01/09   MNK     Add support for N/W initiated context deactivation support
04/29/09   MNK     Registering NAS_ESM_EPS_BEARER_STATUS_IND with msgr
04/28/09   MNK     Added support for EPS_BEARER_STATUS message generation
04/24/09   MNK     Added code to generate detach_cmd when no context exists
04/22/09   MNK     Changes for compiling on Linux and temporary fix to lock ESM processing to avoid race condition 
                   if a re-attach by EMM begins
04/16/09   MNK     More code added to support different scenarios for handling multiple bearers upon attach
04/02/09   MNK     Added support for multiple bearer setup support at the attach 
02/05/09   MNK     Added initialization function for initializing the function pointer 
                   to esm_init_ds_eps_get_pdn_connectivity_msg
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "rex.h"
#ifndef FEATURE_MODEM_RCINIT_PHASE2
#include "tmc.h"
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
#include "pthread.h"
#include "msgr_rex.h"
//#include "ms.h"
#include "esm_v.h" 
#include "esm_cm_msgr_enum_msg.h" 
#include "esm_emm_msgr_enum_msg.h" 
#include "esm_bcm.h"
#include "esm_bpm.h"
#include "esm_utils.h"
#include "gs_v.h"
#include "esm_common.h"
#include "cm_msgr_msg.h"
#include "sm_multimode_common.h"

#include "esm_onex_csfb.h"
#include "stringl.h"
#ifdef FEATURE_LTE_REL9
#include "fs_lib.h"
#endif
#ifdef FEATURE_CIOT
#include "emm_utility.h"
#endif
#include "msgr_nas.h"

typedef union{
  timer_data_T data;
  unsigned long value;
} esmtask_timer_data_u;

/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/
#if defined(FEATURE_NAS_GW) && defined(FEATURE_NAS_SM_IRAT)
extern byte sm_get_nsapi_from_lti(byte ti);
#endif
extern esm_header_T* esm_get_esm_header
(
  mm_cmd_type* cmd_ptr   
);

extern void bcm_get_bc_status
(
  const esm_bcm_T* mgr_ptr
);

extern void bpm_get_bp_status
(
  const esm_bpm_T* mgr_ptr
);

/*extern void ESM_PRINT_MSG
(
  esm_cmd_type* cmd_ptr   
);*/

extern void esm_reset_msg_seq_num(void);

extern boolean esm_bcm_check_context_status(const esm_bcm_T* mgr_ptr);
#if defined(FEATURE_NAS_GW) && defined(FEATURE_NAS_SM_IRAT)
extern void sm_esm_deact_assoc_non_transferrable_ctxts
(
  esm_bcm_T* mgr_ptr,
  byte linked_bearer_id,
  cc_cause_T current_cause
);
#endif 
extern void esm_update_bcm_info
(
  esm_bcm_T* mgr_ptr,
  byte linked_bearer_id,
  bearer_ctrl_mode_T bcm
);

/*! @brief REQUIRED one-sentence brief description of this structure typedef
*/
typedef struct
{
  esm_bcm_T  esm_context_mgr;  /*!< ESM context manager */
  esm_bpm_T  esm_proc_mgr;  /*!< ESM procedure manager */
#ifndef FEATURE_NAS_DISABLE_VOICE
#if defined(FEATURE_LTE_TO_1X)
  esm_1xcsfbm_T esm_1xcsfb_mgr;
#endif
#endif
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
  lte_nas_emc_srv_status_type  emc_srv_status;
#endif
} esm_T;

/* MNK REDESIGN BEGIN*/


//extern q_type mm_cmd_q;

LOCAL mm_cmd_type *esm_pend_msg_ptr_list[ESM_MAX_PENDING_MSGS];
/* MNK REDESIGN END*/

/*==============================================================================

                         LOCAL VARIABLES

==============================================================================*/


/*! @brief Typedef variables internal to module esmtask.c
*/
/* -----------------------------------------------------------------------
** Constant/Define Declarations
** ----------------------------------------------------------------------- */
/*#define ESM_CMD_Q_SIG         ((rex_sigs_type)0x00000002)
#define ESM_REX_SIG_MASK  (ESM_CMD_Q_SIG | TASK_STOP_SIG)*/


#define ESM_CMD_Q_SIZE        35
#define MS_TIMER              6         /*Remove when ms.h is included*/


/*! @brief Variables internal to module oo.c
*/


q_type esm_cmd_q_msg;    /* Msg CMD QUEUE for ESM where the received messages 
                                  are put by the message router*/


/*! ESM instance 
*/

LOCAL esm_T esm;           

//static msgr_client_t* local_client_ptr = NULL;
static msgr_client_t            client;


/* Pointer to a function pointer*/
static esm_bcm_allocate_ctxt_fp_type* bcm_allocate_ctxt_for_multimode = NULL;

/*==============================================================================

                    INTERNAL FUNCTION PROTOTYPES

==============================================================================*/



/*==============================================================================

                                FUNCTIONS

==============================================================================*/





/*==============================================================================

  FUNCTION:  dispatch_cm_cmd_to_the_handler

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what dispatch_cm_cmd_to_the_handler() does goes here.

    @detail
      An OPTIONAL detailed description of dispatch_cm_cmd_to_the_handler() goes here.
      CM commands are dispatched to either ESM context manager or ESM procedure manager

*/
/*============================================================================*/
static void dispatch_cm_cmd_to_the_handler
(
  mm_cmd_type* esm_cmd_ptr  /*!< REQUIRED short parameter description */
)
{
  boolean result = TRUE;
  esm_header_T* hdr_ptr = NULL;

  /*if the connection id is valid then. it means this command is for an existing context,
  so the context manager should handle it. PDN Connectivity is a special case where connection id is valid,but
  it is also a begining of a new procedure
  Option A:
   1. In the case of the message being PDN Connectivity, the nessage first goes to BCM
   2. BCM checks the message type and if it is PDN connectivity, then, returns a code
   3. The code tells the ESM that this message should go to BPM
   4. When the BPM is invoked in this case, the BPM should know to store this connection id
   5. When a matching default context activation request is recived from the network, the BPM should pass
      this connection id back to ESM which in turn passes this to BCM to use instaed of generating a new
      connection id for default bearer context activation in the case of UE initiated PDN connectivity request
   Option B:
      In the CM message Cm would include a flag to indicate that the connection Id is created by the CM. The ESM
      checks that flag and determines if the message should go to BPM or BCM */

#ifndef FEATURE_NAS_DISABLE_VOICE
#if defined(FEATURE_LTE_TO_1X)
  if(esm.esm_1xcsfb_mgr.check_and_handle_cm_message_ptr(esm_cmd_ptr,&(esm.esm_1xcsfb_mgr)) == TRUE)
  {
    MSG_HIGH_DS_0(SM_SUB, "ESM: CSFB message from CM is processed.. Skip regular LTE processing");
    return;
  }
#endif
#endif

  if((esm_cmd_ptr->cmd.hdr.message_id == EMM_RESTORE_BEARER_CTXT_REQ) ||
     (esm_cmd_ptr->cmd.hdr.message_id == MM_RESTORE_PSM_CTXT_RSP))
  {
    esm.esm_context_mgr.handle_cm_message_ptr(
                              esm_cmd_ptr,&(esm.esm_context_mgr),ESM_INVALID_CON_ID);
    return;
  }

   if(esm_cmd_ptr->cmd.hdr.message_id == MMCM_CALL_CTRL_RSP)
   {
       result = esm.esm_proc_mgr.handle_message_ptr(esm_cmd_ptr,&(esm.esm_proc_mgr) );
       if(result == FALSE)
      {
          MSG_ERROR_DS_0(SM_SUB, "ESM: esm_dispatch_cm_cmd_to_the_handler failed for MMCM_CALL_CTRL_RSP\n");
       }

	return;
   }


   // DS is sending Invalid Connection ID for EMM_RAB_REESTAB_REQ; Mapping right connection here.
   if(esm_cmd_ptr->cmd.hdr.message_id == EMM_RAB_REESTAB_REQ)
   {
       esm_cmd_ptr->cmd.emm_rab_reestab_req.esm_hdr.connection_id = esm_get_connection_id(esm_cmd_ptr->cmd.emm_rab_reestab_req.eps_bearer_id);
       MSG_HIGH_DS_2(SM_SUB, "ESM: Mapped RAB Re-est REQ Connection Id : %d, for EPS Bearer Id : %d", esm_cmd_ptr->cmd.emm_rab_reestab_req.esm_hdr.connection_id,esm_cmd_ptr->cmd.emm_rab_reestab_req.eps_bearer_id );
   }

  hdr_ptr = esm_get_esm_header(esm_cmd_ptr);
  if(hdr_ptr == NULL)
  {
    MSG_ERROR_DS_0(SM_SUB, "ESM: ESM Header is NULL in the received message from CM. Shouldn't happen");
    return;
  }

  //ESM_PRINT_MSG(esm_cmd_ptr);
  if(hdr_ptr->connection_id != ESM_INVALID_CON_ID)
  {
    esm.esm_context_mgr.handle_cm_message_ptr(
                              esm_cmd_ptr,&(esm.esm_context_mgr),hdr_ptr->connection_id );
  }
  else
  {
    /* There are two cases. 
    1. The connection id is invalid i.e there is no context yet
    2. The connection id is valid, but is generated by the CM. In this case also there is no context yet
    */
    result = esm.esm_proc_mgr.handle_message_ptr(esm_cmd_ptr,&(esm.esm_proc_mgr) );
    if(result == FALSE)
    {
      MSG_ERROR_DS_0(SM_SUB, "ESM: esm_dispatch_cm_cmd_to_the_handler failed\n");
    }
    if((esm_cmd_ptr->cmd.hdr.message_id == EMM_PDN_CONNECTIVITY_REQ) &&
       (esm.esm_context_mgr.on_attach_def_bearer_pending == ESM_BC_ON_ATTACH_DEF_BEARER_NONE))
    {
      esm.esm_context_mgr.on_attach_def_bearer_pending = ESM_BC_ON_ATTACH_DEF_BEARER_PENDING;
    }
  }
} /* dispatch_cm_cmd_to_the_handler() */

/*==============================================================================

  FUNCTION:  dispatch_emm_cmd_to_the_handler

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what dispatch_emm_cmd_to_the_handler() does goes here.

    @detail
       emm commands are first passed to ESM procedure manager and then depending on the result
       passed to ESM context manager
*/
/*============================================================================*/
static void dispatch_emm_cmd_to_the_handler
(
  mm_cmd_type* esm_cmd_ptr  /*!< REQUIRED short parameter description */
)
{
  boolean result = TRUE;

  //ESM_PRINT_MSG(esm_cmd_ptr);
#ifndef FEATURE_NAS_DISABLE_VOICE
#if defined(FEATURE_LTE_TO_1X)
  if(esm.esm_1xcsfb_mgr.check_and_handle_emm_message_ptr(esm_cmd_ptr,&(esm.esm_1xcsfb_mgr)) == TRUE)
  {
    MSG_HIGH_DS_0(SM_SUB, "ESM: CSFB message from EMM processed.. Skip regular LTE processing");
    return;
  }
#endif
#endif
  result = esm.esm_proc_mgr.handle_message_ptr(esm_cmd_ptr,&(esm.esm_proc_mgr ) );

  /* Result usually should always be true except in the cases when ESM is asked to 
  abort a procedure or when a message with INVALID PTI is received from the network, 
  in which case the OTA message should be discarded after the 
  proc manager processes it and should not be forwarded to the context manager*/
  if(result == TRUE)
  {
   /*If PTI is valid or not the bearer context manager needs to handle this message*/
    esm.esm_context_mgr.handle_emm_message_ptr (esm_cmd_ptr,&(esm.esm_context_mgr ) );
  }
} /* dispatch_emm_cmd_to_the_handler() */

/*==============================================================================

  FUNCTION:  esm_process_esm_timer_expiry

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_process_esm_timer_expiry() does goes here.

    @detail
      Handles the expiration of ESM timers
*/
/*============================================================================*/
static void esm_process_esm_timer_expiry
(
  mm_cmd_type* esm_cmd_ptr    /*!< REQUIRED short parameter description */
)
{
  esm_bpm_process_timer_expiry(esm_cmd_ptr, &(esm.esm_proc_mgr));
} /* esm_process_esm_timer_expiry_cmd() */

static void esm_discard_pending_messages(void)
{
  byte index = 0;

  for(index = 0; index < ESM_MAX_PENDING_MSGS; index++)
  {
    if(esm_pend_msg_ptr_list[index]!=NULL)
    {
      modem_mem_free(esm_pend_msg_ptr_list[index],MODEM_MEM_CLIENT_NAS);
      esm_pend_msg_ptr_list[index]=NULL;
    }  
  }
}


/*==============================================================================

  FUNCTION:  esm_process

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_process() does goes here.

    @detail
      Depending upon the sender the command is routed to the appropriate handler
*/
/*============================================================================*/
void esm_process
(
  mm_cmd_type* emm_cmd_ptr  /*!< REQUIRED short parameter description */
)
{
  byte index = 0;
  //esm_cmd_type* esm_cmd_ptr;

  /*MNK REDESIGN BEGIN*/
  /* Following messages should not be buffered during attach procedure.
     - NAS_ESM_ACT_DEFAULT_BEARER_CONTEXT_ACCEPT_CNF: activate default context 
       accepted. Need to process it to complete attach. 
     - NAS_ESM_ACT_DEFAULT_BEARER_CONTEXT_REJ_IND: activate default context 
       rejected. Need to process it to fail attach. 
     - NAS_ESM_FAILURE_IND: Attach request is rejected by NW. Need to process
       it to reset BPM/BCM resources.
     - NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND: Attach needs to be restarted (cell 
       change) after ESM gets activate default context req and before ESM gets 
       activate default context accept/reject. Need to process this ind to get
       pdn connectivity req for the new attach  */
  if((emm_cmd_ptr->cmd.hdr.message_id== MMCM_CALL_CTRL_RSP) &&
     (esm.esm_context_mgr.on_attach_def_bearer_pending == ESM_BC_ON_ATTACH_DEF_BEARER_PENDING))
  {
    MSG_HIGH_DS_0(SM_SUB, "ESM: received MMCM_CALL_CTRL_RSP from CM");
  }
  else if((esm.esm_context_mgr.on_attach_def_bearer_pending == ESM_BC_ON_ATTACH_DEF_BEARER_PENDING) && 
     (emm_cmd_ptr->cmd.hdr.message_id != EMM_ACT_DEFAULT_BEARER_CONTEXT_ACCEPT) && 
     (emm_cmd_ptr->cmd.hdr.message_id != EMM_ACT_DEFAULT_BEARER_CONTEXT_REJ) &&
     (emm_cmd_ptr->cmd.hdr.message_id != EMM_ESM_FAILURE_IND) &&
     (emm_cmd_ptr->cmd.hdr.message_id != EMM_ESM_GET_PDN_CONNECTIVITY_REQ_IND) && 
     (emm_cmd_ptr->cmd.hdr.message_id != EMM_ESM_SIG_CONNECTION_RELEASED_IND) &&
     (emm_cmd_ptr->cmd.hdr.message_id != EMM_ESM_DETACH_IND) &&
     (emm_cmd_ptr->cmd.hdr.message_id != EMM_PDN_CONNECTIVITY_REQ)&&
    // (emm_cmd_ptr->cmd.hdr.message_id != NAS_ESM_1XCSFB_CALL_REQ)&&
    //(emm_cmd_ptr->cmd.hdr.message_id != NAS_ESM_1XCSFB_ESR_CALL_RSP)&&
     (emm_cmd_ptr->cmd.hdr.message_id != EMM_ESM_RESTORE_CTXT_PSM_REQ) &&
     (emm_cmd_ptr->cmd.hdr.message_id != EMM_ESM_SAVE_CTXT_PSM_REQ) &&
     (emm_cmd_ptr->cmd.hdr.message_id != EMM_RESTORE_BEARER_CTXT_REQ) &&
     (emm_cmd_ptr->cmd.hdr.message_id != MM_RESTORE_PSM_CTXT_RSP))
  {
    if(((emm_cmd_ptr->cmd.hdr.message_id == EMM_ESM_DATA_IND) && 
        (emm_cmd_ptr->cmd.esm_data_ind.esm_incoming_msg.nas_hdr.msg_id != 
                           ACTIVATE_DEFAULT_EPS_BEARER_CONTEXT_REQUEST) &&
        (emm_cmd_ptr->cmd.esm_data_ind.esm_incoming_msg.nas_hdr.msg_id != 
                           PDN_CONNECTIVITY_REJECT) &&
        (emm_cmd_ptr->cmd.esm_data_ind.esm_incoming_msg.nas_hdr.msg_id !=
                                     ESM_INFORMATION_REQUEST)
#ifdef FEATURE_CIOT
         &&
         (emm_cmd_ptr->cmd.esm_data_ind.esm_incoming_msg.nas_hdr.msg_id !=
                                     ESM_DUMMY_MESSAGE)

#endif
                                     ) ||
        (emm_cmd_ptr->cmd.hdr.message_id != EMM_ESM_DATA_IND))
    {
      if(esm.esm_context_mgr.first_def_bearer_ctxt_received == FALSE)
      {
        MSG_HIGH_DS_1(SM_SUB, "ESM: Ignoring msg 0x%x as on attach default bearer is not received",
                      emm_cmd_ptr->cmd.hdr.message_id);
        return;
      }

      for(index = 0; index < ESM_MAX_PENDING_MSGS; index++)
      {
        if(esm_pend_msg_ptr_list[index]==NULL)
        {
          esm_pend_msg_ptr_list[index] = (mm_cmd_type *)modem_mem_calloc(1,sizeof(mm_cmd_type),MODEM_MEM_CLIENT_NAS);
          esm_copy_esm_cmd_msg(&(emm_cmd_ptr->cmd),&(esm_pend_msg_ptr_list[index]->cmd));          
          return;
        }
      }/* for */
    } /* if-else*/
  }
/* MNK REDESIGN END*/ 

  if((emm_cmd_ptr->cmd.hdr.message_id == EMM_ESM_GET_PDN_CONNECTIVITY_REQ_IND) ||
     (emm_cmd_ptr->cmd.hdr.message_id == EMM_ESM_DETACH_IND))
  {
    /* Attach is restarted. Discard all pending msgs from last attempt. */
    esm_discard_pending_messages();
  }

    /* There may be commands that are not dealt by the context manager or the proc manager 
    e.g. DRB_REESTABLISH_REQ How to handle them. They may need to be processed in this 
    function itslef*/
  switch (emm_cmd_ptr->cmd.hdr.message_set) //Figure out based on the sender?
  {
    case MS_MM_DS:
    case MS_CM_MM:  //RAVI_9x05_CHECK
      MSG_HIGH_DS_1(SM_SUB, "ESM: Message %x from the CM received",emm_cmd_ptr->cmd.hdr.message_id);
      dispatch_cm_cmd_to_the_handler ( emm_cmd_ptr);
      break;
    //case MSGR_NAS_EMM:   //RAVI_9x05_CHECK
    case MS_EMM_ESM : 
      MSG_HIGH_DS_1(SM_SUB, "ESM: Message %x from the EMM received",emm_cmd_ptr->cmd.hdr.message_id );
      dispatch_emm_cmd_to_the_handler ( emm_cmd_ptr);
      break;
    case MS_TIMER:
      MSG_HIGH_DS_0(SM_SUB, "ESM: Message from the TIMER received");
      esm_process_esm_timer_expiry( emm_cmd_ptr );
      break;
    default:
      /* Wrong command received on the ESM CMD Q */
      MSG_ERROR_DS_1(SM_SUB, "ESM: Unknown cmd %d received by ESM %d %d",
                     emm_cmd_ptr->cmd.hdr.message_id);
      break;
  }/* end switch */

  /* May be temp solution to find if we have no more contexts*/
  if(esm.esm_context_mgr.on_attach_def_bearer_pending == ESM_BC_ON_ATTACH_DEF_BEARER_SUCCESS)
  {
   /* ESM has no contexts active so, trigger a detach towards EMM. EMM will turn back and send a detach ind but that should be fine*/
    if(esm_bcm_check_context_status(&esm.esm_context_mgr) == TRUE
#ifdef FEATURE_CIOT
        && (emm_is_attach_without_pdn_supported() == FALSE)
#endif
		)
    {
       /*
      Detach Cause: 
                   ESM_LOCAl_CUASE_NONE - Last PDN Disconnect from DS
                   ESM_NO_ACTIVE_BEARER - SR/ESR/TAU by EMM resulting in no bearers at ESM
                   ESM_CONTEXT_TRANSFERED_DUE_TO_IRAT - PDN disconnect by DS due to IRAT to 3GPP2
      */
      lte_nas_local_cause_enum_T local_cause = ESM_NO_ACTIVE_BEARER ;

      if((emm_cmd_ptr->cmd.hdr.message_id == EMM_PDN_DISCONNECT_REQ) 
         &&
         (emm_cmd_ptr->cmd.emm_disconnect_req.esm_local_cause.valid == TRUE)
         &&
         (emm_cmd_ptr->cmd.emm_disconnect_req.esm_local_cause.local_cause 
            == ESM_CONTEXT_TRANSFERED_DUE_TO_IRAT || 
          emm_cmd_ptr->cmd.emm_disconnect_req.esm_local_cause.local_cause 
            == ESM_LOCAL_DEACT))
      {
        local_cause = emm_cmd_ptr->cmd.emm_disconnect_req.esm_local_cause.local_cause;
      }
      MSG_HIGH_DS_0(SM_SUB, "ESM: No default bearer context existing. Need to detach.");
      esm_build_and_send_emm_detach_cmd(local_cause);
    }
  }
} /* esm_process() */

void esm_process_pending_messages(void)
{
  byte index = 0;

  switch(esm.esm_context_mgr.on_attach_def_bearer_pending)
  {
    case ESM_BC_ON_ATTACH_DEF_BEARER_SUCCESS:
#ifdef FEATURE_CIOT
    case ESM_BC_ON_ATTACH_ATTACH_WITHOUT_PDN:
#endif
      for(index = 0; index < ESM_MAX_PENDING_MSGS; index++)
      {
        if(esm_pend_msg_ptr_list[index]!=NULL)
        {          
          esm_process(esm_pend_msg_ptr_list[index]);
          modem_mem_free(esm_pend_msg_ptr_list[index],MODEM_MEM_CLIENT_NAS);
          esm_pend_msg_ptr_list[index]=NULL;
        }
      }
      break;
    case ESM_BC_ON_ATTACH_DEF_BEARER_FAILED:
      esm_discard_pending_messages();
      break;
    default:
      /* Do nothing here */
      break;
  }
} /* esm_process_pending_messages */

/*==============================================================================

  FUNCTION:  esm_emm_context_status_update_func

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_emm_context_status_update_func() does goes here.

    @detail
      Function that creates the context status map and sends it to EMM
*/
/*============================================================================*/
static void esm_emm_context_status_update_func(void)
{
#if defined(FEATURE_NAS_GW) && defined(FEATURE_NAS_SM_IRAT)
  lte_nas_isr_status_type  isr_status;

  isr_status.valid = FALSE;
  isr_status.isr_status = FALSE;

  esm_build_and_send_eps_bearer_status_update_ind(&esm.esm_context_mgr, 
                                                   isr_status); 
#else
  esm_build_and_send_eps_bearer_status_update_ind(&esm.esm_context_mgr); 
#endif
} /*esm_emm_context_status_update_func*/

/*==============================================================================

  FUNCTION:  esm_deactivate_associated_dedicated_ctxts

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_deactivate_associated_dedicated_ctxts() does goes here.

    @detail
      Function that requests the BCM to deactivate the dedicated ctxts associated with a default ctxt

    NOTE: bearer_id here is the default bearer context id. This function is used by the BCM/Context state machine
*/
/*============================================================================*/
static void esm_deactivate_associated_dedicated_ctxts
(
  lte_nas_local_cause_T local_cause,esm_cause_T ota_cause,eps_bearer_id_T bearer_id
)
{
  esm_bcm_deactivate_associated_ded_ctxts(&(esm.esm_context_mgr),local_cause,ota_cause,bearer_id); 
} /*esm_deactivate_associated_dedicated_ctxts*/

/*==============================================================================

  FUNCTION:  esm_deactivate_ctxts

==============================================================================*/
/*!
    @brief
    This function deactivates given bearer context.
    If given bearer context is default bearer context, this function also 
    deactivate the associated dedicated bearer contexts

    @detail

    NOTE: This function is used by the BPM/Procedure state machine

*/
/*============================================================================*/
static void esm_deactivate_ctxts
(
  lte_nas_local_cause_T   local_cause,
  esm_cause_T             ota_cause,
  eps_bearer_id_T         bearer_id
)
{

  struct esm_eps_bearer_context* ctxt_ptr = NULL;


  ctxt_ptr = esm_bcm_find_bearer_context(&(esm.esm_context_mgr),bearer_id);
  if(ctxt_ptr != NULL)
  {
    if(ctxt_ptr->default_bearer_ctxt == TRUE)
    {
      /* Deactivate all dedicated bearer contexts assciated to the given default 
         bearer context */
      esm_bcm_deactivate_associated_ded_ctxts(&esm.esm_context_mgr,
                                               local_cause,
                                               ota_cause,
                                               bearer_id);
    }
    /* Deactivate the given default bearer context */
    esm_build_and_send_deactivate_ind(ctxt_ptr,local_cause,ota_cause);
    esm_bcm_reset_ctxt(ctxt_ptr);

	ctxt_ptr = NULL;
  }
  else
  {
    MSG_HIGH_DS_1(SM_SUB, "ESM: Bearer context %d not existing for deactivating",
                  bearer_id);
  }
} /*esm_deactivate_associated_dedicated_ctxts*/

static gs_status_T esm_dummy_do_nothing_routine( void *msg_p )
{
  return GS_SUCCESS;
}

/*==============================================================================

  FUNCTION:  esm_check_if_detach_to_be_triggered

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_check_if_detach_to_be_triggered() does goes here.

    @detail
      Function that calls ds to get the pdn connect data and builds the PDN Connectivity message
*/
/*============================================================================*/
boolean esm_check_if_detach_to_be_triggered(byte bearer_id)
{
  return esm_is_detach_required(bearer_id, &(esm.esm_context_mgr) );
}/* esm_check_if_detach_to_be_triggered */

/*==============================================================================

  FUNCTION:  esm_find_bearer_context

==============================================================================*/
/*!
    @brief
    This function checks if a given bearer context is existing and the following
    info if the bearer context is existing.
    - default_bearer_ctxt  
    - lbi

    @detail
*/
/*============================================================================*/
static void  esm_find_bearer_context
(
  byte                                bearer_id,
  esm_bearer_context_info_shared_T   *bc_info_shared_ptr
)
{
  esm_eps_bearer_context_T    *esm_bc_ptr;

  /* Must between 5 and 15 */
  if((bearer_id < ESM_STARTING_BEARER_ID)||(bearer_id > ESM_ENDING_BEARER_ID))
  {
    bc_info_shared_ptr->bearer_id_exist = FALSE;
    return;
  }

  esm_bc_ptr = esm_bcm_find_bearer_context(&esm.esm_context_mgr,
                                           bearer_id);
  if(esm_bc_ptr != NULL)
  {
    /* Bearer context is existing */
    bc_info_shared_ptr->bearer_id_exist = TRUE;
    bc_info_shared_ptr->default_bearer_ctxt = esm_bc_ptr->default_bearer_ctxt;
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
    bc_info_shared_ptr->emc_bc = esm_bc_ptr->emc_bc;
#endif
    bc_info_shared_ptr->lbi = esm_bc_ptr->lbi;
    bc_info_shared_ptr->pti = esm_bc_ptr->pti;
    bc_info_shared_ptr->last_msg_sent = esm_bc_ptr->last_msg_sent;
    bc_info_shared_ptr->state = esm_bc_ptr->eps_bearer_state_ptr->state();
  }
  else
  {
    /* Bearer context is not existing */
    bc_info_shared_ptr->bearer_id_exist = FALSE;
  }

}/* esm_find_bearer_context */

#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)

#ifdef FEATURE_NAS_E911_ENABLE

/*===========================================================================

FUNCTION  ESM_GET_EMC_SRV_STATUS

DESCRIPTION
  This function returns emergency bearer service status

DEPENDENCIES
  None

RETURN VALUE
  EMC BS status

SIDE EFFECTS
  None

===========================================================================*/
lte_nas_emc_srv_status_type  esm_get_emc_srv_status(void)
{

  return esm.emc_srv_status;

}/* esm_get_emc_srv_status */

/*===========================================================================

FUNCTION  ESM_SEND_EMC_SRV_STATUS_CMD

DESCRIPTION
  This function sends EMC_SRV_STATUS_CMD to EMM to update EMC service status

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void esm_send_emc_srv_status_cmd
(
  lte_nas_emc_srv_status_type  emc_srv_status
)
{
  emm_emc_srv_status_cmd_T*  emm_cmd;
  emm_cmd = (emm_emc_srv_status_cmd_T *)esm_modem_mem_alloc(sizeof(emm_emc_srv_status_cmd_T));
 
  memset((void *)emm_cmd, 0x0, sizeof(emm_emc_srv_status_cmd_T)); 
  
  msgr_init_hdr(&emm_cmd->msg_hdr,
                 MSGR_NAS_ESM,
                 NAS_EMM_EMC_SRV_STATUS_CMD);

  emm_cmd->emc_srv_status = emc_srv_status;

  esm_msgr_send(&emm_cmd->msg_hdr, sizeof(emm_emc_srv_status_cmd_T));

  MSG_HIGH_DS_1(SM_SUB, "ESM: Sent EMC_SRV_STATUS_CMD w/ status %d", emc_srv_status);
  esm_modem_mem_free(emm_cmd);
}

/*===========================================================================

FUNCTION  ESM_SET_EMC_SRV_STATUS

DESCRIPTION
  This function set emergency bearer service status

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  esm_set_emc_srv_status
(
  lte_nas_emc_srv_status_type  emc_srv_status
)
{
  if(esm.emc_srv_status != emc_srv_status)
  {
    esm.emc_srv_status = emc_srv_status;
    esm_send_emc_srv_status_cmd(emc_srv_status);
  }

}/* esm_set_emc_srv_status */
#endif
#endif


#ifdef FEATURE_CIOT
void esm_set_attach_without_pdn(esm_bc_on_attach_def_bearer_state_T attach_without_pdn)
{
  MSG_HIGH_DS_1(SM_SUB, "ESM: attach without PDN value = %d", attach_without_pdn);
  esm.esm_context_mgr.on_attach_def_bearer_pending = attach_without_pdn;
}

esm_bc_on_attach_def_bearer_state_T esm_get_attach_without_pdn(void)
{
  return esm.esm_context_mgr.on_attach_def_bearer_pending;
}

#endif

/*==============================================================================*/
/*!
    @brief
    This funtion checks if on attach default bearer context has been setup

    @detail

*/
/*============================================================================*/
static boolean  esm_chk_on_attach_def_bc_setup(void)
{
  switch(esm.esm_context_mgr.on_attach_def_bearer_pending)
  {
    case ESM_BC_ON_ATTACH_DEF_BEARER_SUCCESS:
#ifdef FEATURE_CIOT
    case ESM_BC_ON_ATTACH_ATTACH_WITHOUT_PDN:
#endif
      return TRUE;
    default:
      return FALSE;
  }

} /* esm_chk_on_attach_def_bc_setup() */

/*===========================================================================

FUNCTION ESM_ABORT_PROC_BY_BID

DESCRIPTION
  This function calls BPM API to abort ongoing ESM procedure which is associated
  to the given bearer context 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void  esm_abort_proc_by_bid
(
  byte  bearer_id
)
{
  esm_bpm_abort_proc_by_bid(bearer_id, &esm.esm_proc_mgr);

}/* esm_abort_proc_by_bid */

/*==============================================================================

  FUNCTION:  esm_post_timer_expired

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_post_timer_expired() does goes here.

    @detail
    This function sends the timer expired signal to MSGR to be sent to ESM Q
    It registeres NAS_ESM_TIMER_EXPIRED_TMRI before sending and deregisters NAS_ESM_TIMER_EXPIRED_TMRI
    after sending dynamically

*/
/*============================================================================*/

void esm_post_timer_expired(unsigned long param)
{
  esmtask_timer_data_u timer_param;
  mm_cmd_type *cmd_ptr;

  timer_param.value = param; 
  cmd_ptr = mm_get_cmd_buf();
  if (cmd_ptr != NULL)
  {
	cmd_ptr->cmd.esm_timer_expired_ind.message_header.message_set  = MS_TIMER;
	cmd_ptr->cmd.esm_timer_expired_ind.message_header.message_id	    =  MID_TIMER_EXPIRY;
	PUT_IMH_LEN( sizeof( esm_self_signal_timer_expired_T ) - sizeof(IMH_T),
				 &cmd_ptr->cmd.esm_timer_expired_ind.message_header );
  
	cmd_ptr->cmd.esm_timer_expired_ind.timer_id = timer_param.data.timer_id;
	cmd_ptr->cmd.esm_timer_expired_ind.instance_id = timer_param.data.data;

	//Put cmd into MM Queue and sent ESM_CMD_Q_Sig
	esm_put_cmd ((void *)cmd_ptr);
  }

  #if 0
  esm_self_signal_timer_expired_T cmd;
  errno_enum_type    e_status;

  timer_param.value = param; 
  cmd.timer_id = timer_param.data.timer_id;
  cmd.instance_id = timer_param.data.data;

  /* Register NAS_ESM_TIMER_EXPIRED_TMRI */

  lte_nas_msgr_register(MSGR_NAS_ESM, &client,
                           MSGR_ID_REX,NAS_ESM_TIMER_EXPIRED_TMRI);

  /* Init the MSGR header*/
  msgr_init_hdr(&cmd.msg_hdr,MS_TIMER,NAS_ESM_TIMER_EXPIRED_TMRI);
#ifdef FEATURE_DUAL_SIM
  msgr_set_hdr_inst(&cmd.msg_hdr,(uint8)SYS_AS_ID_TO_INST_ID(SYS_MODEM_AS_ID_1));
#endif
  e_status = msgr_send( &cmd.msg_hdr, sizeof(cmd) );
  nas_assert_check((boolean)(e_status == E_SUCCESS));
  MSG_HIGH_DS_0(SM_SUB, "ESM: Post NAS_ESM_TIMER_EXPIRED_TMRI to ESM");

  /* DEREGISTER NAS_ESM_TIMER_EXPIRED_TMRI */
  lte_nas_msgr_deregister(MSGR_NAS_ESM, &client,
                            NAS_ESM_TIMER_EXPIRED_TMRI);
  #endif
}

/*==============================================================================

  FUNCTION:  esm_post_timer_expired_sub2

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_post_timer_expired_sub2() does goes here.

    @detail
    This function sends the timer expired signal to MSGR to be sent to ESM Q
    It registeres NAS_ESM_TIMER_EXPIRED_TMRI before sending and deregisters NAS_ESM_TIMER_EXPIRED_TMRI
    after sending dynamically

*/
/*============================================================================*/
#ifdef FEATURE_DUAL_SIM
void esm_post_timer_expired_sub2(unsigned long param)
{
  esmtask_timer_data_u timer_param;
  esm_self_signal_timer_expired_T cmd;
  errno_enum_type    e_status;

  timer_param.value = param; 
  cmd.timer_id = timer_param.data.timer_id;
  cmd.instance_id = timer_param.data.data;

  /* Register NAS_ESM_TIMER_EXPIRED_TMRI */

  lte_nas_msgr_register(MSGR_NAS_ESM, &client,
                           MSGR_ID_REX,NAS_ESM_TIMER_EXPIRED_TMRI);

  /* Init the MSGR header*/
  msgr_init_hdr(&cmd.msg_hdr,MS_TIMER,NAS_ESM_TIMER_EXPIRED_TMRI);
  msgr_set_hdr_inst(&cmd.msg_hdr,(uint8)SYS_AS_ID_TO_INST_ID(SYS_MODEM_AS_ID_2));
  e_status = msgr_send( &cmd.msg_hdr, sizeof(cmd) );
  nas_assert_check((boolean)(e_status == E_SUCCESS));
  MSG_HIGH_DS_0(SM_SUB, "ESM: Post NAS_ESM_TIMER_EXPIRED_TMRI to ESM");

  /* DEREGISTER NAS_ESM_TIMER_EXPIRED_TMRI */
  lte_nas_msgr_deregister(MSGR_NAS_ESM, &client,
                            NAS_ESM_TIMER_EXPIRED_TMRI);
}
#endif

/*==============================================================================

  FUNCTION:  esm_register_msgs_with_msgr

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_register_msgs_with_msgr() does goes here.

    @detail
      Function registers all the messages expetced to be received by the ESM

*/
/*============================================================================*/
static void esm_register_msgs_with_msgr
(
  msgr_client_t* client_ptr   /*!< REQUIRED short parameter description */
)
{

  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,NAS_ESM_ACT_DEFAULT_BEARER_CONTEXT_ACCEPT_CNF);

  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,NAS_ESM_ACT_DEFAULT_BEARER_CONTEXT_REJ_IND);

  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,NAS_ESM_BEARER_RESOURCE_ALLOC_REQ);

  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,NAS_ESM_BEARER_RESOURCE_REL_REQ);

  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,NAS_ESM_PDN_CONNECTIVTY_REQ);

  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,NAS_ESM_PDN_DISCONNECT_REQ);

  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,NAS_ESM_BEARER_RESOURCE_ALLOC_ABORT_REQ);
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,NAS_ESM_PDN_CONNECTIVITY_ABORT_REQ);

  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,NAS_ESM_DRB_REESTABLISH_REQ);

  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,NAS_ESM_ACT_DEDICATED_BEARER_CONTEXT_ACCEPT_CNF);

  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,NAS_ESM_ACT_DEDICATED_BEARER_CONTEXT_REJ_IND);

  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,NAS_ESM_MODIFY_BEARER_CONTEXT_REJ_IND);

  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,NAS_ESM_MODIFY_BEARER_CONTEXT_ACCEPT_CNF);

  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,NAS_ESM_DATA_IND);
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,NAS_ESM_FAILURE_IND);
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,NAS_ESM_SIG_CONNECTION_RELEASED_IND);
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,NAS_ESM_ACTIVE_EPS_IND);
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,NAS_ESM_DETACH_IND);
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,NAS_ESM_EPS_BEARER_STATUS_IND);
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,
                           NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND);
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,
                           NAS_ESM_ATTACH_STATUS_IND);
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,
                           NAS_ESM_BEARER_RESOURCE_MODIFICATION_REQ);
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,
                           NAS_ESM_PDN_CONNECT_PACKED_CNF);
#ifdef FEATURE_CIOT
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,
                           NAS_ESM_DATA_TRANSPORT_REQ);
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,
                           NAS_ESM_DATA_CNF);
#endif
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,
                           NAS_ESM_GET_ISR_STATUS_IND);
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,
                           NAS_ESM_ISR_STATUS_CHANGE_IND);

  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,
                           NAS_ESM_1XCSFB_CALL_REQ);
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,
                           NAS_ESM_1XCSFB_ABORT_REQ);
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,
                           NAS_ESM_1XCSFB_ESR_CALL_RSP);
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,
                           NAS_ESM_1XCSFB_ESR_CALL_ABORT_RSP);

#ifdef FEATURE_LTE_REL10
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,
                           NAS_ESM_UNBLOCK_ALL_APNS_IND);
#endif
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,
                           NAS_ESM_RESTORE_CTXT_PSM_REQ);
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,
                           NAS_ESM_SAVE_CTXT_PSM_REQ);
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,
                           NAS_ESM_RESTORE_BEARER_CTXT_REQ);
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,
                           NAS_ESM_RESTORE_CTXT_PSM_RSP);
} /* esm_register_msgs_with_msgr() */


/*==============================================================================

  FUNCTION:  esm_deregister_msgs_with_msgr

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_register_msgs_with_msgr() does goes here.

    @detail
      Function deregisters all the messages expetced to be received by the ESM

*/
/*============================================================================*/
static void esm_deregister_msgs_with_msgr
(
  msgr_client_t* client_ptr   /*!< REQUIRED short parameter description */
)
{

  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                            NAS_ESM_ACT_DEFAULT_BEARER_CONTEXT_ACCEPT_CNF);

  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_ACT_DEFAULT_BEARER_CONTEXT_REJ_IND);

  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_BEARER_RESOURCE_ALLOC_REQ);

  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_BEARER_RESOURCE_REL_REQ);

  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_PDN_CONNECTIVTY_REQ);

  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_PDN_DISCONNECT_REQ);

  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_BEARER_RESOURCE_ALLOC_ABORT_REQ);
  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_PDN_CONNECTIVITY_ABORT_REQ);

  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_DRB_REESTABLISH_REQ);

  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_ACT_DEDICATED_BEARER_CONTEXT_ACCEPT_CNF);

  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_ACT_DEDICATED_BEARER_CONTEXT_REJ_IND);

  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_MODIFY_BEARER_CONTEXT_REJ_IND);

  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_MODIFY_BEARER_CONTEXT_ACCEPT_CNF);

  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_DATA_IND);
  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_FAILURE_IND);
  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_SIG_CONNECTION_RELEASED_IND);
  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_ACTIVE_EPS_IND);
  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_DETACH_IND);
  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_GET_PDN_CONNECTIVITY_REQ_IND);
  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_PDN_CONNECT_PACKED_CNF);
  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_1XCSFB_CALL_REQ);
#ifdef FEATURE_CIOT
  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                          NAS_ESM_DATA_TRANSPORT_REQ);
  lte_nas_msgr_register(MSGR_NAS_ESM,client_ptr,
                           MSGR_ID_REX,
                           NAS_ESM_DATA_CNF);
#endif
  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_1XCSFB_ABORT_REQ);
  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_RESTORE_CTXT_PSM_REQ);
  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_SAVE_CTXT_PSM_REQ);
  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_RESTORE_BEARER_CTXT_REQ);
  lte_nas_msgr_deregister(MSGR_NAS_ESM,client_ptr,
                           NAS_ESM_RESTORE_CTXT_PSM_RSP);
} /* esm_deregister_msgs_with_msgr() */

/*==============================================================================

  FUNCTION:  esm_init

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_init() does goes here.

    @detail
    ESM initialization function. Initializes the queues and all the data structures

*/
/*============================================================================*/
void esm_init
(
  void /*!< void */
)
{
  byte count = 0;
  gs_status_T ret_val = GS_FAILURE;

  /* Init */
  memset((void *)&esm, 0x0, sizeof(esm_T));

  init_esm_bpm (&(esm.esm_proc_mgr),
                esm_check_if_detach_to_be_triggered,
                esm_deactivate_ctxts,
                esm_find_bearer_context,
                esm_chk_on_attach_def_bc_setup);
  init_esm_bcm (&(esm.esm_context_mgr),
                esm_emm_context_status_update_func,
                esm_deactivate_associated_dedicated_ctxts,
                esm_deactivate_ctxts,
                &bcm_allocate_ctxt_for_multimode,
                esm_abort_proc_by_bid);

#ifndef FEATURE_NAS_DISABLE_VOICE 
#if defined(FEATURE_LTE) && defined(FEATURE_LTE_TO_1X)
  init_esm_1xcsfbm(&esm.esm_1xcsfb_mgr);
#endif
#endif 

   /* re-initialize seq number between each test for cit*/
   esm_reset_msg_seq_num();
#if defined(FEATURE_LTE_REL9) && !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
  esm.emc_srv_status = LTE_NAS_EMC_SRV_NONE;
#endif

  (void)q_init(&esm_cmd_q_msg);

  


/* MNK REDESIGN BEGIN*/
  for(count = 0; count < ESM_MAX_EPS_BEARERS*2; count++)
  {
    esm_pend_msg_ptr_list[count] = NULL;
  }
/* MNK REDESIGN END*/
  /* Initialize gs message passing for this queue */
  ret_val = gs_init_for_send_only(GS_QUEUE_ESM, (gs_put_handler_ptr)esm_dummy_do_nothing_routine); 
  nas_assert_check((boolean)(ret_val == GS_SUCCESS));

  lte_nas_msgr_client_create(&client);

  lte_nas_msgr_client_set_rex_q(&client,rex_self(),
                                  ESM_CMD_Q_SIG,&esm_cmd_q_msg,
                                  MSGR_NO_QUEUE,NAS_STD_OFFSETOF(esm_cmd_type, cmd ),MSGR_NO_OFFSET,
                                  (word)sizeof(esm_cmd_type));

  /* Register messages with MSGR */
  esm_register_msgs_with_msgr(&client);


} /* esm_init() */


/*==============================================================================

  FUNCTION:  esmtask

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_task() does goes here.

    @detail
    ESM task main function. Receives the signals and delivers thme to the ESM main processing function

*/
/*============================================================================*/
void esm_task
(
void  
)
{
      mm_cmd_type   *esm_cmd_ptr;     /* To hold received ESM command */ 
#ifdef FEATURE_DUAL_SIM
      boolean esm_valid_msg = TRUE;
#endif
      MSG_LOW_DS_0(SM_SUB, "ESM: NAS TASK Receivd ESM_CMD_Q_SIG \n");

       /* Get the command from the SM_CMD_Q and process it */
      while ((esm_cmd_ptr = q_get(&esm_cmd_q_msg)) != NULL)
      {
        /* MNK Temporary fix to avoid race conditions between EMM and ESM for protecting the ESM/contexts until
        EMM is ready to support signal based synchronization for starting the attach procedure*/
#ifdef FEATURE_DUAL_SIM
        esm_valid_msg = esm_validate_and_set_sm_as_id(&(esm.esm_context_mgr),&(esm.esm_proc_mgr),esm_cmd_ptr);
#endif
 
         mm_add_message_to_debug_buffer(((mm_cmd_type*)esm_cmd_ptr)->cmd.hdr.message_set,
                                         ((mm_cmd_type*)esm_cmd_ptr)->cmd.hdr.message_id, TO_MM
                                         ,(sys_modem_as_id_e_type)mm_as_id
                                       );
#ifdef FEATURE_DUAL_SIM
        if(esm_valid_msg == TRUE)
#endif
        {
          esm_process (esm_cmd_ptr);

          esm_process_pending_messages();
        }

        //esm_cmd_ptr->request_direction = ESM_NONE_INITIATED;
        //  msgr_rex_free_msg_buf(&(esm_cmd_ptr->link));
		
#ifdef FEATURE_MODEM_HEAP  
       modem_mem_free(esm_cmd_ptr, MODEM_MEM_CLIENT_NAS);
#endif

      }/* end while */
    
} /* esmtask() */

/*==============================================================================

  FUNCTION:  esmtask

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_task() does goes here.

    @detail
    ESM task main function. Receives the signals and delivers thme to the ESM main processing function

*/
/*============================================================================*/
void esm_process_emm_message
(
mm_int_cmd_type_T *msg,
uint32 msg_len
)
{
      mm_cmd_type *cmd_type = NULL;
	  void *data_ptr = NULL;
	  cmd_type = (mm_cmd_type*)modem_mem_alloc(sizeof(mm_cmd_type),MODEM_MEM_CLIENT_NAS);
	  mm_check_for_null_ptr((void*)cmd_type);
	  data_ptr = (uint8 *)&(cmd_type->cmd);
	  memscpy ( data_ptr, msg_len, msg, msg_len);
	  MSG_HIGH_DS_0(SM_SUB, "ESM: ESM TASK Receivd EMM Mesage \n");
	  esm_process (cmd_type);
	  esm_process_pending_messages();
	  modem_mem_free(cmd_type,MODEM_MEM_CLIENT_NAS);
}


#if defined(FEATURE_NAS_GW) && defined(FEATURE_NAS_SM_IRAT)
/*==============================================================================

  FUNCTION:  esm_update_esm_context_from_sm

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_update_esm_context_from_sm() does goes here.

    @detail
    i-RAT conversion function between SM and ESM

*/
/*============================================================================*/
byte  esm_update_esm_context_from_sm(const pdp_context_info* pdp_ctxt_ptr)
{
  esm_eps_bearer_context_T    *esm_bc_ptr = NULL;


  MSG_HIGH_DS_0(SM_SUB, "ESM: SM called to update ESM context for i-RAT");

#ifdef FEATURE_DUAL_SIM
  if(sm_is_multimode_sub() == FALSE)
  {
    MSG_HIGH_DS_0(SM_SUB, "ESM: Non multimode stack return from esm_update_esm_context_from_sm");
    return esm_get_connection_id(pdp_ctxt_ptr->nsapi.nsapi);
  }
#endif 
  sm_check_for_null_ptr((void *)pdp_ctxt_ptr);

  esm_bc_ptr = esm_bcm_find_bearer_context(&esm.esm_context_mgr,pdp_ctxt_ptr->nsapi.nsapi);

  if(pdp_ctxt_ptr->pdp_state == PDP_ACTIVE) 
  {
    if(esm_bc_ptr == NULL)
    {
      MSG_HIGH_DS_0(SM_SUB, "ESM: Allocating a new ESM context to be updated for i-RAT");
      esm_bc_ptr = bcm_allocate_ctxt_for_multimode(&esm.esm_context_mgr,pdp_ctxt_ptr->nsapi.nsapi);
      sm_check_for_null_ptr((void *)esm_bc_ptr);
    }
    else
    {
      MSG_HIGH_DS_0(SM_SUB, "ESM: ESM context to be updated for i-RAT already exists");
    }

  /*For inter-system change from A/Gb mode or Iu mode to S1 mode, ESM uses the following parameters from each active PDP context: 
  NSAPI to map to EPS bearer identity, linked NSAPI (if available) to map to linked EPS bearer identity, PDP address, APN, 
  TFT (if available), APN-AMBR if it is the default PDP context, and R99 QoS. 
  The mapping from pre-Rel-8 to EPS QoS parameters is given in 3GPP TS 23.401 [10], annex E.*/

    esm_bc_ptr->apn_name         = pdp_ctxt_ptr->apn_name;
    esm_bc_ptr->eps_bearer_id    = pdp_ctxt_ptr->nsapi.nsapi;
    if(pdp_ctxt_ptr->is_primary_pdp != TRUE)
    {
      byte pri_nsapi = sm_get_nsapi_from_lti(pdp_ctxt_ptr->linked_ti); 
      if ((pri_nsapi >= MIN_NSAPI_NUM) && (pri_nsapi < MAX_NSAPI_NUM))
      {
        esm_bc_ptr->lbi = pri_nsapi; 
      }
      else
      {
        MSG_ERROR_DS_1(SM_SUB, "ESM: SM PDP has invalid NSAPI value %d", pri_nsapi);
      }

      /* Not default bearer context */
      esm_bc_ptr->default_bearer_ctxt = FALSE;

      if(esm_bcm_check_context_status(&esm.esm_context_mgr) == TRUE)
      {
        /* Should not happen. Print an error message. */
        MSG_ERROR_DS_0(SM_SUB, "ESM: Non primary PDP activated while ESM has no default ctxt");
      }
    }
    else
    {
      esm_bc_ptr->lbi = 0;

      esm_bc_ptr->sm_group_id = pdp_ctxt_ptr->group_id;

      /* Default bearer context */
      esm_bc_ptr->default_bearer_ctxt = TRUE;

      /* Set on attach pending flag if it is first default bearer context to be
         activated */
      if(esm_bcm_check_context_status(&esm.esm_context_mgr) == TRUE)
      {
        MSG_HIGH_DS_1(SM_SUB, "ESM: First def BC. Setting on_attach_flag from %d to SUCCESS",
                      esm.esm_context_mgr.on_attach_def_bearer_pending);
        esm.esm_context_mgr.on_attach_def_bearer_pending = 
                                           ESM_BC_ON_ATTACH_DEF_BEARER_SUCCESS;
        esm.esm_context_mgr.first_def_bearer_ctxt_received = TRUE;
      }
    }

    esm_bc_ptr->neg_llc_sapi     = pdp_ctxt_ptr->llc_sapi;
    esm_bc_ptr->neg_qos          = pdp_ctxt_ptr->qos;
    esm_bc_ptr->packet_flow_id   = pdp_ctxt_ptr->flow_id;
    esm_bc_ptr->pdn_address.valid = pdp_ctxt_ptr->pdp_addr.valid;
    /* PDN address in LTE is 12 bytes. The lower order 4 bytes corresponds to IPv4 an higher order 8 bytes corresponds to 
    the IID of IPv6. In LTE the lower order 8 bytes of IPv6 are ignored
    +-------------------------------------+
    |   (8  +  8)IPv6  |      4(IPv4)    | = 20 bytes in UMTS
    +------------------------------------+
    */
    memset(&esm_bc_ptr->pdn_address.address[0],0x0,MAX_PDN_ADR_LEN);
    if(pdp_ctxt_ptr->pdp_addr.pdp_type_num == 33)
    {
      memscpy((void*)&esm_bc_ptr->pdn_address.address[0],
              sizeof(esm_bc_ptr->pdn_address.address),
              (void*)&pdp_ctxt_ptr->pdp_addr.address[0],4);
      esm_bc_ptr->pdn_address.pdn_addr_len  = 4;
      esm_bc_ptr->pdn_address.pdn_type_val      = NAS_ESM_IPV4;
    }
    if(pdp_ctxt_ptr->pdp_addr.pdp_type_num == 65)
    {
      /* IID bytes of IPV6 address*/
      memscpy((void*)&esm_bc_ptr->pdn_address.address[4],
              sizeof(esm_bc_ptr->pdn_address.address) - 4,
              (void*)&pdp_ctxt_ptr->pdp_addr.address[12],
              sizeof(pdp_ctxt_ptr->pdp_addr.address) - 12);
      esm_bc_ptr->pdn_address.pdn_addr_len = 12;
      esm_bc_ptr->pdn_address.pdn_type_val      = NAS_ESM_IPV6;
    }

    esm_bc_ptr->radio_pri        = pdp_ctxt_ptr->radio_pri;
    /* Convert QoS. DS will provide a function to be dropped into nas\services and we should call that here*/
    //esm_bc_ptr->sdf_qos          = 
    if(pdp_ctxt_ptr->tft.valid == TRUE)
    {
      esm_bc_ptr->tft              = pdp_ctxt_ptr->tft;
    }
    else
    {
      esm_bc_ptr->tft.valid       = FALSE;
    }

    if(esm_isr_status_get() == TRUE)
    {
      esm_bc_ptr->isr_bearer_ctxt_act_status = CTXT_ACT_AFTER_ISR_IN_GW;
    }
    else
    {
      esm_bc_ptr->isr_bearer_ctxt_act_status = CTXT_ACT_BEFORE_ISR;
    }

    /* Need to change this later*/
    esm_bc_ptr->ti.valid = TRUE;
    esm_bc_ptr->ti.connection_id = pdp_ctxt_ptr->trans_id;
    esm_bc_ptr->eps_bearer_state_ptr = &esm_ctxt_state_active;

#if defined(FEATURE_NAS_GW) && defined(FEATURE_NAS_SM_IRAT)
    esm_bc_ptr->h_comp = pdp_ctxt_ptr->h_comp;
    esm_bc_ptr->d_comp = pdp_ctxt_ptr->d_comp;
    // Update the BCM info
    esm_bc_ptr->bcm = pdp_ctxt_ptr->bcm;
#endif 

	if (esm_bc_ptr->default_bearer_ctxt == TRUE)
    {
      esm_update_bcm_info(&(esm.esm_context_mgr),esm_bc_ptr->eps_bearer_id,esm_bc_ptr->bcm); 
    }	
    else
    {
      esm_update_bcm_info(&(esm.esm_context_mgr),esm_bc_ptr->lbi,esm_bc_ptr->bcm); 
    }

    /* Update EMM */
    esm_bc_ptr->context_status_update_func_ptr();

    return esm_bc_ptr->connection_id;
  }
  else if ((pdp_ctxt_ptr->pdp_state == PDP_INACTIVE) ||
              (pdp_ctxt_ptr->pdp_state == PDP_INACTIVE_PENDING))
  {
    MSG_HIGH_DS_0(SM_SUB, "ESM: Resetting the ESM context for i-RAT as SM context is dectivated");

    if(esm_bc_ptr != NULL)
    {
      if(esm_bc_ptr->default_bearer_ctxt == TRUE)
      {
        /* Deactivate all Non transferrable dedicated bearers. SM should have  
           already deactivated dedicated transferrable bearers */
        sm_esm_deact_assoc_non_transferrable_ctxts(&esm.esm_context_mgr,
                                                         pdp_ctxt_ptr->nsapi.nsapi,
                                                         pdp_ctxt_ptr->current_cause);
      }
      /* Reset all data */
      
      /* Update state */
	  esm_bc_ptr->eps_bearer_state_ptr = &esm_ctxt_state_inactive;
      /* Update EMM */
      esm_bc_ptr->context_status_update_func_ptr();
	  esm_bcm_reset_ctxt(esm_bc_ptr);
      

      if(esm_bcm_check_context_status(&esm.esm_context_mgr) == TRUE)
      {
        MSG_HIGH_DS_1(SM_SUB, "ESM: No def BC left. Setting on_attach_flag from %d to NONE",
                      esm.esm_context_mgr.on_attach_def_bearer_pending);
        esm.esm_context_mgr.on_attach_def_bearer_pending = 
                                              ESM_BC_ON_ATTACH_DEF_BEARER_NONE;
        esm.esm_context_mgr.first_def_bearer_ctxt_received = FALSE;
      }

    }
    else
    {
      MSG_ERROR_DS_0(SM_SUB, "ESM: Bearer doesn't exists in LTE during W-->L i-RAT");
    }
    return ESM_INVALID_CON_ID;
  }
  else
  {
    MSG_ERROR_DS_0(SM_SUB, "ESM: Invalid PDP context state... Do nothing for now..and Debug...");
    return ESM_INVALID_CON_ID;
  }
}

/*===========================================================================

FUNCTION  ESM_BC_DEACT_PENDING_UPDATE_SM

DESCRIPTION
  This function updates SM to reset corresponding PDP contexts when bearer 
  context(s) is(are) going to be deactivated

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  esm_bc_deact_pending_update_sm
(
  eps_bearer_id_T  bearer_id
)
{
  byte                       index = 0;
  esm_bcm_T                 *mgr_ptr = &esm.esm_context_mgr;
  esm_eps_bearer_context_T  *bc_ptr = NULL;

  bc_ptr = esm_bcm_find_bearer_context(mgr_ptr, bearer_id);

  if(bc_ptr != NULL)
  {
    if(bc_ptr->default_bearer_ctxt == TRUE)
    {
      /* For default bearer, update all secondary PDPs which are corresponding 
         to the associated dedicated bearers. */
      for(index = 0; index < ESM_MAX_EPS_BEARERS; index++)
      {
        if(mgr_ptr->esm_bc[index] != NULL && mgr_ptr->esm_bc[index]->lbi == bearer_id)
        {
          (void)sm_update_sm_context_from_esm(mgr_ptr->esm_bc[index], 
                                               ESM_BC_INACTIVE);
        }
      }
    }    
    (void)sm_update_sm_context_from_esm(bc_ptr, ESM_BC_INACTIVE);
  }
  else
  {
    MSG_HIGH_DS_1(SM_SUB, "ESM: Deact pending BC %d not exist", bearer_id);
  }

} /* esm_bc_deact_pending_update_sm */


/*==============================================================================

  FUNCTION:  esm_isr_status_get

==============================================================================*/
boolean esm_isr_status_get(void)
{
  if(sm_is_multimode_sub())
  {
    return esm.esm_context_mgr.isr_status;
  }
  return FALSE;
}

/*==============================================================================

  FUNCTION:  esm_isr_status_set

==============================================================================*/
void esm_isr_status_set(boolean isr_status)
{
  if(sm_is_multimode_sub())
  {
    MSG_HIGH_DS_1(SM_SUB, "ESM: ISR status set to %d", isr_status);
    esm.esm_context_mgr.isr_status = isr_status;
  }
}

/*==============================================================================

  FUNCTION:  esm_reset_isr_bearer_ctxt_act_status

==============================================================================*/
/*!
    @brief
    This function resets the ISR context status of all the bearer contexts..
*/
/*============================================================================*/
void esm_reset_isr_bearer_ctxt_act_status(void)
{
  uint16 index;

    for(index = 0; index < ESM_MAX_EPS_BEARERS; index++)
    {
      if (esm.esm_context_mgr.esm_bc[index] != NULL)
      {
        esm.esm_context_mgr.esm_bc[index]->isr_bearer_ctxt_act_status = CTXT_ACT_BEFORE_ISR;
      }	
    }
}
#endif 


#if defined(FEATURE_NAS_GW)

/*===========================================================================

FUNCTION  SM_ABORT_PENDING_ESM_PROC

DESCRIPTION
  This function aborts all pending ESM procedures

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  sm_abort_pending_esm_proc(void)
{
  if(sm_is_multimode_sub())
  {
    lte_nas_local_cause_T  local_cause;
    MSG_HIGH_DS_0(SM_SUB, "ESM: RAT change - Abort pending ESM procedure");
    local_cause.valid = TRUE;  
    local_cause.local_cause = ESM_LOWER_LAYER_FAILURE;
    esm_bpm_clean_up(&esm.esm_proc_mgr,local_cause);
  }
}

/*==============================================================================

  FUNCTION:  esm_deactivate_bearer_ctxts_irat_to_GW

==============================================================================*/
void esm_deactivate_bearer_ctxts_irat_to_GW(void)
{
  if(sm_is_multimode_sub())
  {
    esm_bcm_deactivate_non_transferable_ctxts_to_GW(&esm.esm_context_mgr);
  }
}

#endif

/*==============================================================================

  FUNCTION:  esm_stop_task

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_task() does goes here.

    @detail
  
*/
/*============================================================================*/

void esm_stop_task
(
  void
)
{
   errno_enum_type    e_status;
   esm_cmd_type   *esm_cmd_ptr;     /* To hold received ESM command */ 
   
      if ((esm_cmd_ptr = q_get(&esm_cmd_q_msg)) != NULL)
      {
        esm_deregister_msgs_with_msgr(&client);
        msgr_rex_free_msg_buf(&(esm_cmd_ptr->link));
      }
      e_status = msgr_client_delete( &client );
      nas_assert_check((boolean)(e_status == E_SUCCESS));
}

void esm_bpm_get_bp_status(void)
{
  bpm_get_bp_status(&esm.esm_proc_mgr);
}
void esm_bcm_get_bc_status(void)
{
  bcm_get_bc_status(&esm.esm_context_mgr);
}

void esm_get_status(void)
{
  esm_bpm_get_bp_status();
  esm_bcm_get_bc_status();
}


/*==============================================================================

                                UNIT TEST

==============================================================================*/


void esm_init_bcm_context_fn_ptr(esm_eps_bearer_context_T * esm_bc)
{

  // Need to make this extra function call only because the 2 functions being
  // passed as arguments are statics in this file. Thses functions are not
  // available in esm_bcm.c where we are calling esm_bcm_allocate_context() to 
  // allocate the memory for the contexts.
  esm_init_bcm_context_fn(esm_bc,
                          esm_emm_context_status_update_func,
                          esm_deactivate_associated_dedicated_ctxts,
                          esm_check_if_detach_to_be_triggered);
}


void esm_init_bcm_context_fn(esm_eps_bearer_context_T * esm_bc,
                             esm_emm_context_status_update_func_fp *update_func_ptr,
                             esm_bcm_deactivate_associated_ded_ctxts_fp     *deact_func_ptr,
                             esm_chk_if_detach_to_be_triggered_fp   *chk_trigger_ptr)
{
  esm_bc->context_status_update_func_ptr = update_func_ptr;
  esm_bc->deactivate_associated_dedicated_ctxts_func_ptr = deact_func_ptr;
  esm_bc->check_detach_trigger = chk_trigger_ptr;
}

/*==============================================================================

  FUNCTION:  esm_bcm_reset_ctxt

==============================================================================*/
/*!
    @brief
    A REQUIRED brief one-sentence description of what esm_bcm_reset_ctxt() does goes here.

    @detail
    Reset a given context and free up its memory.
*/
/*============================================================================*/
void esm_bcm_reset_ctxt
(
  struct esm_eps_bearer_context* ctxt_ptr   /*!< REQUIRED short parameter description */
)
{
  byte index;
  sm_check_for_null_ptr((void *)ctxt_ptr);
  index = ctxt_ptr->eps_bearer_id - ESM_STARTING_BEARER_ID;

  /* Change the context state and update SM and EMM */
  ctxt_ptr->eps_bearer_state_ptr->ctxt_change_state(ctxt_ptr,
                                                    &esm_ctxt_state_inactive);
  /* RESET all the data */
  esm_bcm_reset_ctxt_data(ctxt_ptr);

  esm_modem_mem_free(esm.esm_context_mgr.esm_bc[index]);
  esm.esm_context_mgr.esm_bc[index] = NULL;
} /* esm_bcm_reset_ctxt() */
#ifdef TEST_FRAMEWORK
#error code not present
#endif


/*===========================================================================

FUNCTION  ESM_IGNORE_MOD_DUE_TO_COLLISION

DESCRIPTION
  This function checks if PDN DISCONNECT is on-going for corresponding 
  default bearer context 

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None

===========================================================================*/


boolean esm_ignore_mod_due_to_collision(mm_cmd_type *cmd_ptr)
{
  byte index = 0;
  byte bid;
  esm_bearer_context_info_shared_T    bc_info_shared;

  bid = cmd_ptr->cmd.emm_modify_bearer_context_rej.eps_bearer_id;
  

  /* Find the bearer context w/ the given bearer ID */
  esm.esm_proc_mgr.find_bearer_context_ptr(bid, &bc_info_shared);

  for(index=0; index < ESM_MAX_BP_PROCS; index++)
  {
    if(esm.esm_proc_mgr.esm_bp[index].inuse == TRUE)
    {
      if((esm.esm_proc_mgr.esm_bp[index].pending_msg_ptr != NULL) &&
        (esm.esm_proc_mgr.esm_bp[index].pending_msg_ptr->esm_out_msg.nas_hdr.msg_id == PDN_DISCONNECT_REQUEST) &&
        ((bc_info_shared.bearer_id_exist == TRUE) && 
        (esm.esm_proc_mgr.esm_bp[index].lbi == bc_info_shared.lbi)))
      {
        MSG_HIGH_DS_0(SM_SUB, "ESM: Proc collision - ignore MODIFY REJ as PDN DISCONNECT pending");
        /* Proc collision found, need to ignore the message */
        return TRUE;
      }
    }
  }
  return FALSE;
  
}
/*===========================================================================

FUNCTION  ESM_CHECK_PDN_DISCONNECT_SENT_BY_BID

DESCRIPTION
  This function checks if UE sent PDN disconnect request for the given
  bearer id

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean esm_check_pdn_disconnect_sent_by_bid(byte bearer_id)
{
  uint8 index;
  boolean result = FALSE;

  for (index=0; index < ESM_MAX_BP_PROCS; index++)
  {
      if ( (esm.esm_proc_mgr.esm_bp[index].lbi == bearer_id) &&
           (esm.esm_proc_mgr.esm_bp[index].pending_msg_ptr->esm_out_msg.nas_hdr.msg_id == PDN_DISCONNECT_REQUEST) )
       
      {
	  result = TRUE;
          break; 
      }
  }

  MSG_HIGH_DS_2(SM_SUB, "ESM: esm_check_pdn_disconnect_sent_by_bid() returns %d for BID %d", result, bearer_id);
  return result;
}
#endif /*FEATURE_LTE*/
