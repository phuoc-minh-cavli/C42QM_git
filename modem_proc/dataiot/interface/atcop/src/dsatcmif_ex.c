/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( C A L L  M A N A G E R  I N T E R F A C E )
                
                P R O C E S S I N G

GENERAL DESCRIPTION
  This software unit contains functions for interfacing to Call Manager.

EXTERNALIZED FUNCTIONS

EXTERNALIZED FUNCTIONS INTERNAL TO DSAT UNIT
  dsatcmif_cm_call_cmd_handler_ex
    This function is the handler function for the CM call related commands

  dsatcmif_cm_call_event_handler_ex
    This function is the handler function for the CM call related events

  dsatcmif_cm_ss_event_handler_ex
    This function is the handler function for the CM serving system
    related events

  dsatcmif_end_calls
    This function issues a request to CM to end call(s).

  dsatcmif_signal_handler
    This function processes the asynchronous signals assosiacted with
    messages from Call Manager.

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2019 - 2023 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/interface/atcop/src/dsatcmif_ex.c#11 $ $DateTime: 2023/08/09 05:51:58 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/15/23   ks      Enhancing +COPS, +CEREG commands to support NTN RAT.
02/03/22   ks      Temporarily Mapping NTN RAT to NB1. Need to handle NTN RAT 
                   separately once 3GPP 27007 spec is updated for +COPS.
10/10/22   ks      Made changes to look for CIOT_SYS_MODE_MASK which is
                   needed for updating cell-id properly.
02/16/22   asa     Added support for suppresing CGEV URC during RLF
11/08/21   asa     For CAMPED_DOMAIN indication, update the CGATT status only
                   when there no active PDNs to avoid invalid NW_DETACH URC.
01/25/21   ks      Added support for FSK related AT commands.
11/24/20   ks      last_dereg_success variable will also be updated based on 
                   PH INFO event in case SUBS AVAILABLE event is missed after 
                   the PSM exit due to race condition between CM event and DS init.
01/22/19   skc     Changes made to save WS46 value properly.
01/15/18   rk      Fixed issue related to COPS cmd.
07/06/17   rk      Fixed issue related to network registration status.
06/13/17   rk      Added support for +CEDRXS & +CEDRXRDP command.
1/19/17   skc     Added enhancement on CHLD and CLCC for VoLTE MPTY Call.
12/16/16   skc     Added support for AT+VTS before connect event.
12/09/16   skc     Fixed CEREG URC issue to display cell info in LTE Only build.
10/05/16   skc     Fixed KW issues.
08/18/16   skc     Fixed issue with at+chld=2 during MPTY calls
12/16/15   skc     Fixed issue where wrong call id is stored during one 
                   active call and one held call and at+chld=2 is issued.
12/10/15   skc     Adding condition for enabling of dsat_power_off only when
                   operating mode is not online.
05/04/15   sc      Fixed +CHLD command issue to use requesting client id
                   instead of call client id. 
03/05/15   sc      Fixed +CHLD command IP call issue.
02/13/15   sc      Fixed +CHLD AT command issue.
11/24/14   pg      Fixed ws46 featurization issue.
07/29/14   sc      Added support for NV Refresh.
08/01/14   sc      Added support for linked MO and MT USSD sessions.
06/27/14   tk/sc   Initial revision (created file for Dynamic ATCoP).
07/08/14   sc      Fixed SGLTE+G issue on SUB2.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>


#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

/* this file should be included only in GSM or WCDMA mode */
#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsime.h"
#include "dsatetsictab.h"
#include "dsatetsicmif.h"
#include "dsatetsicall.h"

#include "dstask.h"
#include "queue.h"
#include "stdio.h"
#include "ds_pdn_manager.h"

#ifdef FEATURE_DSAT_ETSI_DATA
#include "dsatetsipkt.h"

/* needed for receiving RAB events and inter RAT HO*/
#include "dsatvend.h"

#include "gsndcp.h"
#endif /* FEATURE_DSAT_ETSI_DATA */

#include "mn_cm_exp.h"

#include "mmgsdilib.h"
#include "mmgsdilib_v.h"

#endif /* FEATURE_DSAT_ETSI_MODE */

#include "dsatme.h"
#include "dsati.h"
#include "dsatact.h"
#include "dsatctab.h"
#include "dsatdl.h"
#include "dsatcmif.h"
#include "dsatvoice.h"
#include "dsatparm.h"
#include "cm.h"
#include "amssassert.h"
#include "err.h"
#include "msg.h"
#include "dsm.h"

#include <stringl/stringl.h>

//Featurize for now... just bring up AT interface now

/*===========================================================================

            PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains public definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
/* +CGEREP variables */
#ifdef FEATURE_ETSI_ATTACH
dsat_num_item_type     dsat_gprs_attach_status;
#endif /* FEATURE_ETSI_ATTACH */

#ifdef FEATURE_DSAT_ETSI_DATA
boolean                dsat_power_off = FALSE;
#endif /* FEATURE_DSAT_ETSI_DATA */

#define AT_MODE_NONE 0

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
    import definitions:
-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
    Local definitions:
-------------------------------------------------------------------------*/
/* CM client (ATCOP) Identifier */
cm_client_id_type dsatcm_client_id;

/* Call Manager message watermark queue */
LOCAL dsm_watermark_type  dsat_cm_msg_wm;
LOCAL q_type              dsat_cm_msg_q;
LOCAL boolean             dsat_cm_msg_wm_full;
LOCAL boolean             dsat_cm_msg_wm_inited = FALSE;

#ifdef IMAGE_QDSP6_PROC
rex_crit_sect_type dsat_cm_wm_crit_sect;
#endif /* IMAGE_QDSP6_PROC*/
/* Lo Water, Hi Water and Don't Exceed counts for the   */
/* watermark used for select events from Call Manager.   */
#define DSAT_CM_MSG_SIZE            (sizeof(dsat_cm_msg_s_type))
#define DSAT_FROM_CM_LOWATER_CNT          (DSAT_CM_MSG_SIZE * 2)
#define DSAT_FROM_CM_HIWATER_CNT          (DSAT_CM_MSG_SIZE * 9)
#define DSAT_FROM_CM_DONT_EXCEED_CNT      (DSAT_CM_MSG_SIZE * 10)
#define DSAT_RSSI     1
#define DSAT_HDR_RSSI 2

/* Cache of RX signal data */
dsat_num_item_type dsat_rssi_enqueue = 0;
#ifdef IMAGE_QDSP6_PROC
rex_crit_sect_type dsat_rssi_crit_sect;
#endif /* IMAGE_QDSP6_PROC*/

/*Change field 1 mapping*/
const LOCAL dsatcmif_ss_mask_map_s_type dsatcmif_ss_mask_map[] =
{
  { CM_SS_EVT_PS_DATA_SUSPEND_MASK  , DSAT_SS_PS_DATA_SUSPEND_MASK  },
  { CM_SS_EVT_SRV_STATUS_MASK       , DSAT_SS_SRV_STATUS_MASK       },
  { CM_SS_EVT_SRV_DOMAIN_MASK       , DSAT_SS_SRV_DOMAIN_MASK       },
  { CM_SS_EVT_SYS_MODE_MASK         , DSAT_SS_SYS_MODE_MASK         },
  { CM_SS_EVT_SYS_ID_MASK           , DSAT_SS_SYS_ID_MASK           },
  { CM_SS_EVT_SRV_IND_MASK          , DSAT_SS_SRV_IND_MASK          },
  { CM_SS_EVT_MOBILITY_MGMT_MASK    , DSAT_SS_MOBILITY_MGMT_MASK    },
  { CM_SS_EVT_SIM_STATE_MASK        , DSAT_SS_SIM_STATE_MASK        },
  { CM_SS_EVT_CELL_INFO_MASK        , DSAT_SS_CELL_INFO_MASK        },
  { CM_SS_LAC_TAC_RAC_CHGD_MASK     , DSAT_SS_LAC_TAC_RAC_CHGD_MASK },
  { CM_SS_EVT_ROAM_STATUS_MASK      , DSAT_SS_ROAM_STATUS_MASK      },
  { CM_SS_EVT_SRV_CAPABILITY_MASK   , DSAT_SS_SRV_CAPABILITY_MASK   },
  { CM_SS_EVT_CIOT_LTE_SYS_MODE_MASK, DSAT_SS_CIOT_LTE_SYS_MODE_MASK},
  /*{ CM_SS_EVT_CQI_INFO_MASK         , DSAT_SS_CQI_INFO_MASK },
  { CM_SS_EVT_CELL_SRV_IND_MASK     , DSAT_SS_CELL_SRV_IND_MASK      },*/  
};

/*Change field 2 mapping - TBD*/

/*-------------------------------------------------------------------------
    Local functions:
-------------------------------------------------------------------------*/

void cmif_call_event_cb_func 
(
  cm_call_event_e_type            event,            /* Event ID              */
  const cm_mm_call_info_s_type   *event_ptr         /* Pointer to Event info */
);

LOCAL void cmif_ss_event_cb_func 
(
  cm_ss_event_e_type            event,                  /* Event ID              */
  const cm_mm_ss_info_s_type   *event_ptr         /* Pointer to Event info */
);

LOCAL void cmif_cm_ph_event_cb_func 
(
  cm_ph_event_e_type         event,            /* Event ID              */
  const cm_ph_info_s_type   *event_ptr         /* Pointer to Event info */
);

boolean cmif_is_gw_subscription_available
(
  sys_modem_as_id_e_type     subs_id,
  ds_at_cm_ph_event_type    *ph_event
);

LOCAL dsat_result_enum_type cmif_process_cm_reg_data_ex
(
  const ds_at_cm_ss_event_type * event_ptr        /* DS event pointer */
);

LOCAL void cmif_report_mode_ex
(
  sys_srv_status_e_type     srv_status,
  sys_srv_domain_e_type     srv_domain,
  sys_sys_mode_e_type       sys_mode
);

#ifdef FEATURE_DSAT_EXTENDED_CMD
LOCAL void cmif_report_mode_3gpp2_ex
(
  ds_at_ss_info_s_type *ss_info_ptr
);
#endif /* FEATURE_DSAT_EXTENDED_CMD */

LOCAL void cmif_process_rssi_info_3gpp
(
  const ds_at_cm_ss_event_type *event_ptr /* DS event pointer */
);

LOCAL void cmif_setup_msg_watermarks( void );

LOCAL void cmif_register_cb_func_ex ( void );

#ifdef FEATURE_DSM_WM_CB
LOCAL void cmif_cm_msg_wm_hiwater_cb 
(
  struct dsm_watermark_type_s * wm_ptr, 
  void * cb_ptr
);
LOCAL void cmif_cm_msg_wm_lowater_cb 
(
  struct dsm_watermark_type_s * wm_ptr, 
  void * cb_ptr
);
LOCAL void cmif_cm_msg_wm_non_empty_cb
(
  struct dsm_watermark_type_s * wm_ptr, 
  void * cb_ptr
);
#else
LOCAL void cmif_cm_msg_wm_hiwater_cb ( void );
LOCAL void cmif_cm_msg_wm_lowater_cb ( void );
LOCAL void cmif_cm_msg_wm_non_empty_cb ( void );
#endif /* FEATURE_DSM_WM_CB */

LOCAL void cmif_start_s_seven_timer
( 
  ds_profile_num_type     profile_id,
  cm_call_type_e_type     call_type,
  cm_call_id_type         call_id,
  cm_client_type_e_type   client_id
);
LOCAL void cmif_stop_s_seven_timer
( 
  ds_profile_num_type    profile_id,
  cm_call_type_e_type    call_type,
  cm_call_id_type        call_id,
  cm_client_type_e_type  client_id
);

/*===========================================================================
FUNCTION  CMIF_CALL_EVENT_CB_FUNC

DESCRIPTION
  CM Call status event callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer
===========================================================================*/
void dsatcmif_event_cb_func
(
  dssysmgr_cm_event_type event_type,
  dssysmgr_cm_common_event_info cm_common_ev_info
);


/*===========================================================================
FUNCTION  DSATCMIF_CALL_CMD_CB_FUNC

DESCRIPTION
  CM Call status command callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
void dsatcmif_call_cmd_cb_func 
(
  void                         *data_ptr,         /* Data block pointer    */
  cm_call_cmd_e_type            cmd,              /* Command ID            */
  cm_call_cmd_err_e_type        cmd_err           /* Command error code    */
)
{
  ds_command_type            *cmd_buf;
  ds_at_cm_call_cmd_type *call_cmd = NULL;

  cmd_buf  = dsat_get_cmd_buf(sizeof(ds_at_cm_call_cmd_type), FALSE);

  /* send the message to ATCOP */
  cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_CM_CALL_CMD;
  call_cmd = cmd_buf->cmd_payload_ptr;
  call_cmd->cmd                    = cmd;
  call_cmd->cmd_err                = cmd_err;
  call_cmd->data_ptr               = data_ptr;
  ds_put_cmd(cmd_buf);
}  /* dsatcmif_call_cmd_cb_func */



/*===========================================================================

FUNCTION DSATCMIF_END_CALLS

DESCRIPTION
  This function issues a request to CM to end call(s).
  Num_call_ids should specify thee size of the call id array pointed to by
  call_id_ptr.

DEPENDENCIES

RETURN VALUE
  TRUE:     The end request is sent, callback function
              will be called.
  FALSE:    The end request failed.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
boolean dsatcmif_end_calls( 
  const uint8 num_call_ids, 
  const cm_call_id_type * call_id_ptr 
)
{
  /* Hangup the call */
  boolean                 call_status;
  cm_end_params_s_type    *end_params_ptr = NULL;
  uint8 index;

  DS_ASSERT( call_id_ptr != NULL );

  if ( num_call_ids > CM_CALL_ID_MAX )
  {
    return FALSE;
  }
  end_params_ptr = (cm_end_params_s_type *)dsat_alloc_memory(num_call_ids * sizeof(cm_end_params_s_type),FALSE);

  for ( index = 0; index < num_call_ids; index++ )
  {
    memset( &end_params_ptr[index],
            CM_CALL_CMD_PARAM_DEFAULT_VALUE,
            sizeof ( cm_end_params_s_type ));

    end_params_ptr[index].info_type = voice_state[index].call_mode;

    end_params_ptr[index].call_id = call_id_ptr[index];
  }

  call_status = cm_mm_call_cmd_end(dsatcmif_call_cmd_cb_func,
                                   NULL,
                                   dsatcm_client_id,
                                   num_call_ids,
                                   end_params_ptr);

  dsatutil_free_memory((void *)end_params_ptr);
  return call_status;
} /* dsatcmif_end_calls */

/*===========================================================================

FUNCTION DSATCMIF_ANSWER_VOICE_CALL

DESCRIPTION
  This command answer a voice call by calling cm_call_cmd_answer

DEPENDENCIES

RETURN VALUE
  TRUE : If the command succeeds.
  FALSE: If the command fails.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatcmif_answer_voice_call
(
  uint8                    call_id_ptr,
  boolean                  answer_call,
  cm_call_mode_info_e_type call_mode
)
{
  boolean                 call_status;
  cm_ans_params_s_type    *answer_params_ptr = NULL;

  answer_params_ptr = (cm_ans_params_s_type *)dsat_alloc_memory(sizeof(cm_ans_params_s_type),FALSE);
  memset( (void *)answer_params_ptr, CM_CALL_CMD_PARAM_DEFAULT_VALUE, sizeof( cm_ans_params_s_type));
  
if( call_mode == CM_CALL_MODE_INFO_IP )
  {
    answer_params_ptr->info_type = CM_CALL_MODE_INFO_IP;
    answer_params_ptr->ans_params.ip_ans.accept = answer_call;
    answer_params_ptr->ans_params.ip_ans.call_reject_reason = answer_call ? 
                                                         CM_CALL_END_NONE : 
                                                         CM_CALL_END_SIP_486_BUSY_HERE;
    answer_params_ptr->ans_params.ip_ans.call_type = CM_CALL_TYPE_VOICE;
  }
  else
  {
    answer_params_ptr->info_type = CM_CALL_MODE_INFO_GW_CS;
    answer_params_ptr->ans_params.gw_cs_ans.accept = answer_call;
    answer_params_ptr->ans_params.gw_cs_ans.call_type = CM_CALL_TYPE_VOICE;
  }

  call_status = cm_mm_call_cmd_answer(dsatcmif_call_cmd_cb_func,
                                   NULL,
                                   dsatcm_client_id,
                                   call_id_ptr,
                                   answer_params_ptr
                                   );
  dsatutil_free_memory((void*)answer_params_ptr);
  return call_status;
} /* dsatcmif_answer_voice_call */

/*===========================================================================

FUNCTION  DSAT_UPDATE_CALL_INFO_GLOBAL

DESCRIPTION
  This function update call related global variables for the CM call-related 
  events.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void dsat_update_call_info_global
(
  ds_at_cm_call_event_type *call_event /* Call event pointer */
)
{
  dsati_mode_e_type current_mode;
  cm_call_event_e_type      event;
  ds_at_call_info_s_type   *event_ptr =NULL;
  dsatetsicall_call_state_info    *call_val = NULL;

  event_ptr = &call_event->event_info;
  event     =  call_event->event;     
  current_mode = dsatcmdp_get_current_mode();
  call_val = dsat_get_base_addr(DSAT_CALL_VALS, TRUE);
  if (NULL == call_val)  {
    return ;
  }

  if ( dsatcmif_is_voice_call_type(event_ptr->call_type)
      /* outgoing packet data calls(ETSI only), */
#ifdef FEATURE_DSAT_ETSI_DATA
      || ((CM_CALL_TYPE_PS_DATA == event_ptr->call_type) &&
          (CM_CALL_STATE_INCOM != event_ptr->call_state))
#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_DSAT_ETSI_MODE
       /* Incoming, connected or end events for CS data calls */
      || (CM_CALL_TYPE_CS_DATA == event_ptr->call_type &&
         (CM_CALL_STATE_INCOM == event_ptr->call_state ||
          CM_CALL_STATE_CONV  == event_ptr->call_state ||
          CM_CALL_STATE_IDLE  == event_ptr->call_state ) )

       /* CM_CALL_EVENT_MNG_CALLS_CONF
          For ATCOP originated events: 
            Need to check success and update state or give CME ERROR.
          For other client:
            Need to update held calls info as no other events are posted. */
      || event == CM_CALL_EVENT_MNG_CALLS_CONF
      || event == CM_CALL_EVENT_PROGRESS_INFO_IND
      || event == CM_CALL_EVENT_CALL_BARRED
      || event == CM_CALL_EVENT_CALL_FORWARDED
      || event == CM_CALL_EVENT_CALL_BEING_FORWARDED
      || event == CM_CALL_EVENT_INCOM_FWD_CALL
      || event == CM_CALL_EVENT_CALL_ON_HOLD 
      || event == CM_CALL_EVENT_CALL_RETRIEVED
      || event == CM_CALL_EVENT_ORIG_FWD_STATUS
      || event == CM_CALL_EVENT_ORIG
      || event == CM_CALL_EVENT_ANSWER
#endif /* FEATURE_DSAT_ETSI_MODE */
      || event == CM_CALL_EVENT_INFO
     )
  {
    DS_AT_MSG3_HIGH("ATCOP: cm event cb, event: %d, type: %d, state: %d",
            event, event_ptr->call_type, event_ptr->call_state);
    if( NULL != event_ptr )
    {
#ifdef FEATURE_DSAT_ETSI_MODE
      if ( IS_ETSI_MODE(current_mode) )
      {
#ifdef FEATURE_DSAT_ETSI_DATA
        if(CM_CALL_EVENT_END == event)
        {
          call_val->dsat_pdp_deactivation_cause = event_ptr->end_status;
        }
#endif /* FEATURE_DSAT_ETSI_DATA */
      }
#endif /* FEATURE_DSAT_ETSI_MODE */
    }
  }
  #ifdef FEATURE_DSAT_ETSI_MODE
  /* Capture call end status */
  if( IS_ETSI_MODE(current_mode) &&
      (	(CM_CALL_EVENT_END == event) ||
        (CM_CALL_EVENT_CALL_CONF == event) ||
        (CM_CALL_EVENT_PDN_CONN_REJ_IND == event) ||
        (CM_CALL_EVENT_PDN_CONN_FAIL_IND == event)
      ) 
    )
  {
    dsatetsicmif_capture_call_end_status( event, event_ptr );
  }
#endif /* FEATURE_DSAT_ETSI_MODE */
}/* dsat_update_call_info_global */


/*===========================================================================

FUNCTION  DSATCMIF_CM_CALL_EVENT_HANDLER_EX

DESCRIPTION
  This function is the handler function for the CM call-related events.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :       if there was any problem in executing the command
    DSAT_ASYNC_EVENT : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatcmif_cm_call_event_handler_ex
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type            result = DSAT_ASYNC_EVENT;
  dsati_mode_e_type                current_mode;
  ds_at_cm_call_event_type        *call_event = dsat_get_cmd_payload_ptr(cmd_ptr);
  dsatetsicall_call_state_info    *call_val = NULL;

  current_mode = dsatcmdp_get_current_mode();

  if ( (call_event->event_info.dsat_info_list_avail == TRUE) &&
       (call_event->event ==  CM_CALL_EVENT_INFO) )
  {
  }
  else
  {
    dsat_update_call_info_global(call_event);
  }

  call_val = dsat_get_base_addr(DSAT_CALL_VALS, TRUE);
  if (NULL == call_val)
  {
    return DSAT_ASYNC_EVENT;
  }

  DS_AT_MSG1_HIGH("etsicall_was_call_active = %d",
                 call_val->etsicall_was_call_active);

  switch (call_event->event)
  {
    case CM_CALL_EVENT_INCOM:
      /* Assign a sequence number (as described in 3GPP TS 22.030 
         section 6.5.5.1) to this incoming call */
      if((call_event->event_info.call_type == CM_CALL_TYPE_VOICE ) ||
         (call_event->event_info.call_type == CM_CALL_TYPE_CS_DATA)||
         (call_event->event_info.call_type == CM_CALL_TYPE_PS_DATA))
      {
        dsatact_track_sequence_number(
            call_event->event_info.call_id,
            call_event->event,
            call_event->event_info.call_type);
      }
      /* Maintain call state info: add waiting call id and
         pass event to dsatvoice for RINGing or dsatetsicmif for +CCWA 
         unsolicited result code dependent upon active call or not */
      if (call_event->event_info.dsat_mode_info.info_type == CM_CALL_MODE_INFO_CDMA)
      {
        result = dsatvoice_call_event_incom(&call_event->event_info);
      }
#ifdef FEATURE_DSAT_ETSI_MODE
      else
      {
        result = dsatetsicall_call_event_incom(call_event);
      }
#endif /* FEATURE_DSAT_ETSI_MODE */
      break;
     case CM_CALL_EVENT_CONNECT:
      /* Reset the S7 timer if running  */
      cmif_stop_s_seven_timer(call_event->event_info.profile_number,
                              call_event->event_info.call_type,
                              call_event->event_info.call_id,
                              call_event->event_info.call_client_id);
      /* Assign a sequence number (as described in 3GPP TS 22.030 
              section 6.5.5.1) to this connected call unless already done when
              it was incoming */
      if((call_event->event_info.call_type == CM_CALL_TYPE_VOICE ) || 
        (call_event->event_info.call_type == CM_CALL_TYPE_CS_DATA) || 
        (call_event->event_info.call_type == CM_CALL_TYPE_PS_DATA))
      {
        dsatact_track_sequence_number(
                         call_event->event_info.call_id,
                         call_event->event,
                         call_event->event_info.call_type);
      }
      result = dsatetsicall_call_event_connect_ex(call_event);
      break;

    case CM_CALL_EVENT_PROGRESS_INFO_IND:
      /* The MO (originating) call moved to alerting state, 
         started ringing on the other side */
      dsat_mo_dailing_state_set(FALSE);
#ifdef FEATURE_DSAT_ETSI_MODE 
      /* Update +CIND callsetup indicator */
      (void)dsatetsime_change_indicator_state( DSAT_CIND_INDEX_CALLSETUP,
                                               DSAT_CIND_CSETUP_MORING );
#endif /* FEATURE_DSAT_ETSI_MODE */
      break;
    case CM_CALL_EVENT_END:
      /* As call got ended make dsat_mo_dailing_state true */
      dsat_mo_dailing_state_set(TRUE);

      /* Reset the S7 timer if running  */
      cmif_stop_s_seven_timer(call_event->event_info.profile_number,
                              call_event->event_info.call_type,
                              call_event->event_info.call_id,
                              call_event->event_info.call_client_id);
    /* Free call sequence number (as described in 3GPP TS 22.030 
                  section 6.5.5.1) of this ending call */
         if((call_event->event_info.call_type == CM_CALL_TYPE_VOICE ) || 
           (call_event->event_info.call_type == CM_CALL_TYPE_CS_DATA) || 
           (call_event->event_info.call_type == CM_CALL_TYPE_PS_DATA))
         {
           dsatact_track_sequence_number(
                            call_event->event_info.call_id,
                            call_event->event,
                            call_event->event_info.call_type);
         }
        result = dsatetsicall_call_event_end_ex(call_event);
      break;
    case CM_CALL_EVENT_ANSWER:
    case CM_CALL_EVENT_ORIG:
      {
        /* Start the S7 timer iff it is ATCoP originated/answered Call */
        if( (dsat_num_item_type)dsatutil_get_val(
                                    DSAT_SREG_S7_IDX,0,NUM_TYPE) > 0)
        {
          cmif_start_s_seven_timer(call_event->event_info.profile_number,
                                   call_event->event_info.call_type,
                                   call_event->event_info.call_id,
                                   call_event->event_info.call_client_id);
        }

        if( call_event->event == CM_CALL_EVENT_ORIG )
        {
          result = dsatvoice_call_event_orig(&call_event->event_info);
        }
      }
      break;
    case CM_CALL_EVENT_SETUP_IND:

      if((call_event->event_info.call_type == CM_CALL_TYPE_VOICE ) ||
         (call_event->event_info.call_type == CM_CALL_TYPE_CS_DATA)||
         (call_event->event_info.call_type == CM_CALL_TYPE_PS_DATA))
      {
        dsatact_track_sequence_number(
          call_event->event_info.call_id,
          call_event->event,
          call_event->event_info.call_type);
      }
      break;

    case CM_CALL_EVENT_ACT_BEARER_IND:
    case CM_CALL_EVENT_PDN_CONN_REJ_IND:
    case CM_CALL_EVENT_PDN_CONN_FAIL_IND:
    case CM_CALL_EVENT_RES_ALLOC_REJ_IND:
    case CM_CALL_EVENT_RES_ALLOC_FAIL_IND:
      if(call_event->event_info.call_type == CM_CALL_TYPE_PS_DATA)
      {
        dsatact_track_sequence_number(
          call_event->event_info.call_id,
          call_event->event,
          call_event->event_info.call_type);
      }
      break;
    case CM_CALL_EVENT_HO_COMPLETE:
    case CM_CALL_EVENT_SRVCC_COMPLETE_IND:
      {
        result = dsatvoice_call_event_ho_complete(&call_event->event_info);
      }
      break;
    default:
      break;
  }

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatcmif_cm_call_event_handler_fp))
  {
    result = dsatdl_vtable.dsatcmif_cm_call_event_handler_fp(call_event, result );
  }

  return result;
} /* dsatcmif_cm_call_event_handler_ex */


/*===========================================================================

FUNCTION  DSATCMIF_CM_CALL_CMD_HANDLER_EX

DESCRIPTION
  This function is the handler function for the CM call-related commands

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatcmif_cm_call_cmd_handler_ex
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type   result = DSAT_NO_RSP;
  ds_at_cm_call_cmd_type *call_cmd = dsat_get_cmd_payload_ptr(cmd_ptr);

  DS_AT_MSG3_HIGH("Call cmd handler cmd_err %d [Applicable for only voice] cause %d alpha len %d",
            call_cmd->cmd_err, call_cmd->err_cause, call_cmd->alpha.len);

  switch (call_cmd->cmd)
  {
    case CM_CALL_CMD_ANSWER:
      result = dsatvoice_cmd_answer_cb_handler(call_cmd->cmd_err);
      break;

    default:
      break;
  }

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatcmif_cm_call_cmd_handler_fp))
  {
    result = dsatdl_vtable.dsatcmif_cm_call_cmd_handler_fp(call_cmd, result);
  }

  return result;
} /* dsatcmif_cm_call_cmd_handler_ex */

/*===========================================================================

FUNCTION  DSATCMIF_CM_SS_EVENT_HANDLER_EX

DESCRIPTION
  This function is the handler function for the CM serving system
  related events

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatcmif_cm_ss_event_handler_ex
(
  dsat_cm_msg_s_type *msg_ptr
)
{
  dsat_result_enum_type         result = DSAT_ASYNC_EVENT;
  dsat_result_enum_type         result_tmp = DSAT_ASYNC_EVENT;
  ds_at_cm_ss_event_type       *event_ptr = NULL;
  dsat_num_item_type            rssi_enqueue = 0;

  if (NULL == msg_ptr)
  {
    DS_AT_MSG0_ERROR("NULL msg_ptr");
    return result;
  }

  /*Assign  SS event info  pointer*/
  event_ptr = &msg_ptr->msg.ss_event;

  DS_AT_MSG1_HIGH("Processing CM SS event: %d", event_ptr->event);

  switch (event_ptr->event)
  {
    case CM_SS_EVENT_SRV_CHANGED:
    {         
             switch(event_ptr->event_info.sys_mode)
            {
              /*Process 3GPP information*/
              case SYS_SYS_MODE_NO_SRV:
              case SYS_SYS_MODE_GSM:
              case SYS_SYS_MODE_LTE:
                if ( ( event_ptr->event_info.changed_fields & 
                        (DSAT_SS_SRV_CHG_MASK 
#if defined(FEATURE_GSM_RR_CELL_CALLBACK) || defined(FEATURE_DSAT_LTE)
                         | DSAT_SS_CELL_INFO_MASK 
#endif /* FEATURE_GSM_RR_CELL_CALLBACK || FEATURE_DSAT_LTE */
                        )) || 
                      ( DSAT_CM_REQ_ID == event_ptr->event_info.cm_req_id ) )
                {
                  /*Process CM registration information */
                  result_tmp = cmif_process_cm_reg_data_ex(event_ptr);
                }
                if ( event_ptr->event_info.changed_fields & DSAT_SS_MOBILITY_MGMT_MASK )
                {
                  (void)dsatetsicmif_process_cm_mm_info_data_ex( &event_ptr->event_info.mm_info,
                  	                                           &event_ptr->event_info);
                }
              break;
              /*Undefined system mode for ATCoP*/
              default:
              break;
            }
        	
           /*check the result from both the stack */
        if (DSAT_ASYNC_EVENT != result_tmp)
        {
          result = result_tmp;
        }
#if defined(FEATURE_DSAT_SP_CMDS)
     cmif_determine_spservice(&event_ptr->event_info);
#endif /* defined(FEATURE_DSAT_SP_CMDS) */
    }
    break;
    case CM_SS_EVENT_RSSI:
    {
      rssi_enqueue = DSAT_RSSI;
      /* Pre process RSSI data */
          switch(event_ptr->event_info.sys_mode)
          {
            case SYS_SYS_MODE_NO_SRV:
            case SYS_SYS_MODE_GSM:
            case SYS_SYS_MODE_LTE:
              cmif_process_rssi_info_3gpp(event_ptr);
            break;
           default:
             break;
          }
    }
    break;
    case CM_SS_EVENT_REG_REJECT:
    {
          switch(event_ptr->event_info.sys_mode)
          {
            case SYS_SYS_MODE_NO_SRV:
            case SYS_SYS_MODE_GSM:
            case SYS_SYS_MODE_LTE:
              dsatetsicall_creg_state_machine( event_ptr);
            break;
           default:
             break;
          }
    }
    break;

    default:
      break;
  }

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatcmif_cm_ss_event_handler_fp))
  {
    result = dsatdl_vtable.dsatcmif_cm_ss_event_handler_fp(msg_ptr, result);
  }

  if ( rssi_enqueue )
  {
#ifdef IMAGE_QDSP6_PROC
    rex_enter_crit_sect(&dsat_rssi_crit_sect);
#else
    INTLOCK();
#endif /* IMAGE_QDSP6_PROC*/
    dsat_rssi_enqueue = dsat_rssi_enqueue & ~rssi_enqueue;
#ifdef IMAGE_QDSP6_PROC
    rex_leave_crit_sect(&dsat_rssi_crit_sect);
#else
    INTFREE();
#endif /* IMAGE_QDSP6_PROC*/
  }

  return result;
} /* dsatcmif_cm_ss_event_handler_ex */

/*===========================================================================

FUNCTION   CMIF_MAP_SS_CHANGED_FIELDS

DESCRIPTION
  This function maps CM SS changed fields to ATCoP specific changed fields.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL uint16 cmif_map_ss_changed_fields
(
  uint64                     cm_changed_fields
)
{
  uint16 dsat_changed_fields = 0;
  uint8  idx;
  const dsatcmif_ss_mask_map_s_type *ss_mask_map;
  size_t                      ss_mask_map_size;

  DS_AT_MSG2_HIGH("CM change fields 0x%X 0x%X",
  	              (cm_changed_fields >> 32),
  	               cm_changed_fields);
 
  /* Convert and Store per Stack changes field */
    ss_mask_map       = &dsatcmif_ss_mask_map[0];
    ss_mask_map_size  = ARR_SIZE( dsatcmif_ss_mask_map );
 
  for (idx = 0; idx < ss_mask_map_size; idx++)
  {
    if (cm_changed_fields & ss_mask_map[idx].cm_ss_mask)
    {
      dsat_changed_fields |= ss_mask_map[idx].dsat_ss_mask;
    }
  }    

  return dsat_changed_fields;
} /* cmif_map_ss_changed_fields */

/*===========================================================================
FUNCTION  CMIF_SS_EVENT_MSIM_CB_FUNC

DESCRIPTION
  CM serving system status event callback function for multi-sim targets.
  CM sends SS_events per subscription 
  Single subscription can have mutiple stacks.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
/*ARGSUSED*/
LOCAL void cmif_ss_event_cb_func
(
  cm_ss_event_e_type            event,            /* Event ID              */
  const cm_mm_ss_info_s_type   *event_ptr         /* Pointer to Event info */
)
{
  boolean               post_ss_event = FALSE;
  dsat_num_item_type    rssi_enqueue = 0;
  dsat_num_item_type    local_dsat_rssi_enqueue;
  boolean               local_cm_msg_wm_full;
  dsat_cm_msg_s_type   *msg_ptr = NULL;
  ds_at_ss_info_s_type *info_ptr = NULL;
  dsm_item_type        *dsm_item_ptr = NULL;

  /* Invalid Number of stacks */
  if( NULL == event_ptr )
  {
    return;
  }

  DS_AT_MSG1_HIGH("In cmif_ss_event_cb_func: event  %d", event);

  /* Allocate memory for the message. This will be freed at the end of the function.
     So there is no return until the end of the function. */
  msg_ptr = dsat_alloc_memory(sizeof(dsat_cm_msg_s_type), FALSE);

  switch (event)
  {
      case CM_SS_EVENT_SRV_CHANGED:
        /*Unified CM change fields into ATCoP change fields*/
        DS_AT_MSG1_MED("sys_sel_pref_req_id = %d", event_ptr->sys_sel_pref_req_id);
        info_ptr = &msg_ptr->msg.ss_event.event_info;
        info_ptr->changed_fields =
        cmif_map_ss_changed_fields(event_ptr->changed_fields);
           

        if ( ( 0 != info_ptr->changed_fields ) ||
             ( DSAT_CM_REQ_ID == event_ptr->sys_sel_pref_req_id ) )
        {
          post_ss_event = TRUE;
        }
        break;

      case CM_SS_EVENT_RSSI:
        rssi_enqueue = DSAT_RSSI;
        post_ss_event = TRUE;
        break;

      case CM_SS_EVENT_INFO:
        post_ss_event = TRUE;
        break;

      case CM_SS_EVENT_HDR_RSSI:
        rssi_enqueue = DSAT_HDR_RSSI;
        post_ss_event = TRUE;
        break;
      case CM_SS_EVENT_REG_REJECT:
        post_ss_event = TRUE;
        break;
#ifdef FEATURE_DSAT_LTE
      case CM_SS_EVENT_LTE_RRC_TX_INFO:
        post_ss_event = TRUE;
        break;
#endif /* FEATURE_DSAT_LTE */
      default:
        break;
    }

  if ( rssi_enqueue )
  {
#ifdef IMAGE_QDSP6_PROC
    rex_enter_crit_sect(&dsat_rssi_crit_sect);
#else
    INTLOCK();
#endif /* IMAGE_QDSP6_PROC*/
    local_dsat_rssi_enqueue = dsat_rssi_enqueue;
#ifdef IMAGE_QDSP6_PROC
    rex_leave_crit_sect(&dsat_rssi_crit_sect);
#else
    INTFREE();
#endif /* IMAGE_QDSP6_PROC*/

    /* Do not post multiple RSSI events (only one required) */
    if ( local_dsat_rssi_enqueue & rssi_enqueue )
    {
      DS_AT_MSG0_MED("Dropping redundant RSSI event");
      post_ss_event = FALSE;
    }
  }

  if ( TRUE == post_ss_event )
  {
#ifdef IMAGE_QDSP6_PROC
    rex_enter_crit_sect(&dsat_cm_wm_crit_sect);
#else
    INTLOCK();
#endif /* IMAGE_QDSP6_PROC*/
    local_cm_msg_wm_full = dsat_cm_msg_wm_full;
#ifdef IMAGE_QDSP6_PROC
    rex_leave_crit_sect(&dsat_cm_wm_crit_sect);
#else
    INTFREE();
#endif /* IMAGE_QDSP6_PROC*/

    /* Ensure there is capacity in message watermark for new one */
    if( FALSE == local_cm_msg_wm_full )
    {
      /* Populate the message from CM event */
      msg_ptr->cmd = DS_CMD_ATCOP_CM_SS_INFO_CMD;
      msg_ptr->msg.ss_event.event = event;

        info_ptr = &msg_ptr->msg.ss_event.event_info;
        
        info_ptr->roam_status = event_ptr->roam_status;
        info_ptr->hdr_roam_status = event_ptr->roam_status;
        info_ptr->srv_status = event_ptr->srv_status;
        info_ptr->srv_domain = event_ptr->srv_domain;
        info_ptr->rssi = event_ptr->rssi;
        info_ptr->sys_id = event_ptr->sys_id;

        info_ptr->mm_info.plmn_avail =
          event_ptr->mm_information.plmn_avail;
        info_ptr->mm_info.univ_time_and_time_zone_avail =
          event_ptr->mm_information.univ_time_and_time_zone_avail;
        info_ptr->mm_info.time_zone_avail =
          event_ptr->mm_information.time_zone_avail;
        info_ptr->mm_info.plmn =
          event_ptr->mm_information.plmn;
        info_ptr->mm_info.univ_time_and_time_zone =
          event_ptr->mm_information.univ_time_and_time_zone;
        info_ptr->mm_info.time_zone =
          event_ptr->mm_information.time_zone;

        info_ptr->sim_state = event_ptr->sim_state;
        dsatutil_memscpy(&info_ptr->cell_srv_ind,
                         sizeof(cm_cell_srv_ind_s_type),
                         &event_ptr->cell_srv_ind,
                         sizeof(cm_cell_srv_ind_s_type));
        
        info_ptr->sys_mode = event_ptr->sys_mode;
        info_ptr->ciot_lte_mode = event_ptr->ciot_lte_mode;
        info_ptr->hdr_srv_status = event_ptr->srv_status;        
        info_ptr->ps_data_suspend = event_ptr->ps_data_suspend;
        info_ptr->srv_capability = event_ptr->srv_capability;
        info_ptr->cm_req_id = event_ptr->sys_sel_pref_req_id;
#ifdef FEATURE_DSAT_LTE
        info_ptr->lte_tac = event_ptr->tac;
       info_ptr->lte_rac_or_mme_code = event_ptr->rac_or_mme_code;
#endif /* FEATURE_DSAT_LTE */
#if defined(FEATURE_GSM_RR_CELL_CALLBACK) || defined(FEATURE_DSAT_LTE)
        dsatutil_memscpy(&info_ptr->cell_info,
                         sizeof(sys_cell_info_s_type),
                         &event_ptr->cell_info,
                         sizeof(sys_cell_info_s_type));
#endif /* FEATURE_GSM_RR_CELL_CALLBACK || FEATURE_DSAT_LTE */
        info_ptr->dsatcmif_signal_reporting.rssi = event_ptr->rssi;
        info_ptr->dsatcmif_signal_reporting.hdr_rssi = event_ptr->rssi;
        info_ptr->dsatcmif_signal_reporting.rssi2 = event_ptr->rssi2;
        //info_ptr->dsatcmif_signal_reporting.rscp = event_ptr->rscp;
        info_ptr->dsatcmif_signal_reporting.bit_err_rate = 
          event_ptr->bit_err_rate;
#ifdef FEATURE_UMTS_REPORT_ECIO
        //info_ptr->dsatcmif_signal_reporting.ecio = event_ptr->ecio;
#endif /* FEATURE_UMTS_REPORT_ECIO */
#ifdef FEATURE_UMTS_SIR_PATHLOSS
        //info_ptr->dsatcmif_signal_reporting.pathloss = event_ptr->pathloss;
        //info_ptr->dsatcmif_signal_reporting.sir = event_ptr->sir;
#endif /* FEATURE_UMTS_SIR_PATHLOSS */
        info_ptr->dsatcmif_signal_reporting.sys_mode = event_ptr->sys_mode;
        dsatutil_memscpy(&info_ptr->reg_reject_info,
                         sizeof(cm_reg_reject_info_s_type),
                         &event_ptr->reg_reject_info,
                         sizeof(cm_reg_reject_info_s_type));

#ifdef FEATURE_DSAT_LTE
#ifdef FEATURE_DSAT_ATCOP_VEND

       /*for $QCRATSTATE indication and command response*/
       info_ptr->dsat_lte_tx_info.lte_tx_status = event_ptr->lte_rrc_tx_info.lte_tx_is_active;
       info_ptr->dsat_lte_tx_info.lte_tx_band = event_ptr->lte_rrc_tx_info.lte_tx_band;
#endif       
#endif /* FEATURE_DSAT_LTE */
           
      /* Post message to CM watermark */
      dsm_item_ptr = dsat_dsm_create_packet(msg_ptr, sizeof(dsat_cm_msg_s_type), TRUE);
      if (NULL != dsm_item_ptr)
      {    
        DS_AT_MSG1_ERROR("CM SS w message queue inited - dropping event: %d", dsat_cm_msg_wm_inited);
        if(dsat_cm_msg_wm_inited == TRUE )
      {
        /* Add to queue and raise subtask signal */
        dsm_enqueue(&dsat_cm_msg_wm, &dsm_item_ptr);
        }
		else
        {
          dsm_free_packet( &dsm_item_ptr );		  
        }
        if ( rssi_enqueue )
        {
          /* Flag one RSSI event in queue */
#ifdef IMAGE_QDSP6_PROC
          rex_enter_crit_sect(&dsat_rssi_crit_sect);
#else
          INTLOCK();
#endif /* IMAGE_QDSP6_PROC*/
          dsat_rssi_enqueue = dsat_rssi_enqueue | rssi_enqueue;
#ifdef IMAGE_QDSP6_PROC
          rex_leave_crit_sect(&dsat_rssi_crit_sect);
#else
          INTFREE();
#endif /* IMAGE_QDSP6_PROC*/
        }
      }
      else
      {
        post_ss_event = FALSE;
      }
    }
    else
    {
      /* Watermark flow control engaged */
      DS_AT_MSG1_ERROR("CM message queue full - dropping event: %d", event);
      post_ss_event = FALSE;
    }
  }

  dsatutil_free_memory( msg_ptr );

  return;
} /* cmif_ss_event_cb_func */


/*===========================================================================
FUNCTION  DSATCMIF_PH_CMD_CB_FUNC

DESCRIPTION
  CM phone command callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
/* ARGSUSED */
void dsatcmif_ph_cmd_cb_func 
(
  void                         *data_ptr,         /* Data block pointer    */
  cm_ph_cmd_e_type              cmd,              /* Command ID            */
  cm_ph_cmd_err_e_type          cmd_err           /* Command error code    */
)
{
  ds_command_type               * cmd_buf  = NULL;
  boolean                     post_cmd = FALSE;
  ds_at_cm_ph_cmd_type*       ph_cmd   = NULL;
#ifdef FEATURE_DSAT_ETSI_MODE
  dsati_mode_e_type current_mode       = dsatcmdp_get_current_mode();
#endif /* FEATURE_DSAT_ETSI_MODE */

  DS_AT_MSG3_MED("dsatcmif_ph_cmd_cb_func: data_ptr = %p, cmd = %d, cmd_err = %d",
                  data_ptr, cmd, cmd_err);

  /* Filter supported commands */
  switch (cmd)
  {
  case CM_PH_CMD_SET_PREFERRED_NETWORKS:
#ifdef FEATURE_DSAT_ETSI_MODE
    if ( IS_ETSI_MODE(current_mode) )
    {
      post_cmd = TRUE;
    }
#endif /* FEATURE_DSAT_ETSI_MODE */
    break;
  case CM_PH_CMD_INFO_GET:
  case CM_PH_CMD_SYS_SEL_PREF:
  case CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE:
  case CM_PH_CMD_SUBSCRIPTION_AVAILABLE:
  case CM_PH_CMD_OPRT_MODE:
  case CM_PH_CMD_GET_NETWORKS:
#ifdef FEATURE_DSAT_LTE
  case CM_PH_CMD_GET_DRX_REQ:
  case CM_PH_CMD_SET_DRX_REQ:
#endif/* FEATURE_DSAT_LTE*/
/*#ifdef FEATURE_DSAT_EDRX
  case CM_PH_CMD_GET_EDRX_REQ:
  case CM_PH_CMD_SET_EDRX_REQ:
#endif    FEATURE_DSAT_EDRX */
    /* Verify there is no error condition */
    if (CM_PH_CMD_ERR_NOERR != cmd_err)
    {
      post_cmd = TRUE;
    }
    break;
  case CM_PH_CMD_RESET_ACM:
  case CM_PH_CMD_SET_ACMMAX:
  case CM_PH_CMD_MRU_UPDATE:
  case CM_PH_CMD_READ_MRU:
    post_cmd = TRUE;
    break;
  default:
    /* Do nothing for unsupported commands */
    break;
  }

  if (TRUE == post_cmd)
  {
    /* Send result to DS task */
    cmd_buf    = dsat_get_cmd_buf(sizeof(ds_at_cm_ph_cmd_type), FALSE);
    ph_cmd = cmd_buf->cmd_payload_ptr;

    /* Send the message to ATCOP */
    cmd_buf->hdr.cmd_id  = DS_CMD_ATCOP_CM_PH_CMD;
    ph_cmd->data_ptr = data_ptr;
    ph_cmd->cmd      = cmd;
    ph_cmd->cmd_err  = cmd_err;

    ds_put_cmd(cmd_buf);
  }

  return;
} /* dsatcmif_ph_cmd_cb_func */

/*===========================================================================

FUNCTION  DSATCMIF_CM_PH_CMD_HANDLER_EX

DESCRIPTION
  This function is the handler function for the CM phone-related commands

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_ASYNC_CMD :  if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatcmif_cm_ph_cmd_handler_ex
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type         result = DSAT_ASYNC_CMD;
  ds_at_cm_ph_cmd_type*         ph_cmd = dsat_get_cmd_payload_ptr(cmd_ptr);

#ifdef FEATURE_DSAT_ETSI_MODE
  dsati_mode_e_type             current_mode = dsatcmdp_get_current_mode();
#endif /* FEATURE_DSAT_ETSI_MODE */
  dsatcmif_servs_state_info     *ph_val = NULL;

  ph_val = dsat_get_base_addr(DSAT_PH_VALS, TRUE);

  switch (ph_cmd->cmd)
  {
  case CM_PH_CMD_SYS_SEL_PREF:

    /* Verify there is no error condition */
    if (CM_PH_CMD_ERR_NOERR != ph_cmd->cmd_err)
    {
      DS_AT_MSG1_ERROR("CM PH command error: %d",ph_cmd->cmd_err);
      
#ifdef FEATURE_DSAT_ETSI_MODE
      if ( IS_ETSI_MODE(current_mode) )
      {
        /* Clear flags on failure of attach write */
        if ((CM_PH_CMD_SYS_SEL_PREF == ph_cmd->cmd) && (NULL != ph_val))
        {
          if(DSAT_COPS_ASTATE_NULL != ph_val->dsatetsicall_network_list.cmd_state)
          {
            DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_COPS_ETSI_IDX,0,0,
             (dsat_num_item_type)ph_val->dsatetsicall_network_list.previous_pref.mode,MIX_NUM_TYPE)
          }
          /* Asynch command processing is done */
          ph_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_NULL;
          ph_val->dsatetsicall_network_list.cmd_idx = CMD_IDX_NONE;
        }
      }/* IS_ETSI_MODE() */
#endif /* FEATURE_DSAT_ETSI_MODE */
    }
 
  default:
    result = DSAT_ERROR;
    break;
  }

  if(DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatcmif_cm_ph_cmd_handler_fp) )
  {
    result = dsatdl_vtable.dsatcmif_cm_ph_cmd_handler_fp(ph_cmd, result);
  }

  return result;
} /* dsatcmif_cm_ph_cmd_handler_ex */



/*===========================================================================
FUNCTION  DSATCMIF_UPDATE_NETWORK_INFO

DESCRIPTION


DEPENDENCIES


RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL boolean dsatcmif_update_network_info
(
  ds_at_ph_info_u_type    *event_info_ptr
)
{
  dsatcmif_servs_state_info   *ph_val = NULL;

  ph_val = dsat_get_base_addr(DSAT_PH_VALS,TRUE);
  if (NULL == ph_val)
  {
    return FALSE;
  }
  
  ph_val->dsat_prev_ms_service_preference = ph_val->dsat_ms_service_preference;
  ph_val->dsat_ms_service_preference = event_info_ptr->pref_info.service_domain_pref;

  if( event_info_ptr->pref_info.network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_NO_CHANGE )
  {
    event_info_ptr->pref_info.network_sel_mode_pref =
                   ph_val->dsat_net_reg_state.cmph_pref.network_sel_mode_pref ;
  }

  if(event_info_ptr->pref_info.network_rat_mode_pref == CM_MODE_PREF_NO_CHANGE )
  {
    event_info_ptr->pref_info.network_rat_mode_pref = 
                    ph_val->dsat_net_reg_state.cmph_pref.network_rat_mode_pref ;
  }

  if( event_info_ptr->pref_info.network_roam_pref == CM_ROAM_PREF_NO_CHANGE )
  {
    event_info_ptr->pref_info.network_roam_pref =
                    ph_val->dsat_net_reg_state.cmph_pref.network_roam_pref ;
  }

  if(event_info_ptr->pref_info.network_band_pref == CM_BAND_PREF_NO_CHANGE )
  {
    event_info_ptr->pref_info.network_band_pref = 
                    ph_val->dsat_net_reg_state.cmph_pref.network_band_pref ;
  }
  return TRUE;
}


/*===========================================================================
FUNCTION  DSATCMIF_CM_PH_EVENT_HANDLER_EX

DESCRIPTION
  This function is the event handler invoked by CM in response to Phone
  group commands:
    - PS domain attach or detach triggered by +CGATT command
    - functionality level/operating mode (power setting)
    - Available/preferred network reporting
    - Phone preference changes

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.
    DSAT_ASYNC_EVENT : if success and no response required.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatcmif_cm_ph_event_handler_ex
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  ds_at_cm_ph_event_type*    ph_event = dsat_get_cmd_payload_ptr(cmd_ptr);
  
#ifdef FEATURE_DSAT_LTE
  dsat_eps_mode_e_type      eps_mode = DSAT_EPS_MODE_MAX;
#endif /* FEATURE_DSAT_LTE */  

  dsatcmif_servs_state_info     *ph_val = NULL;

  DS_AT_MSG2_HIGH("Processing CM PH event: %d, OPRT mode %d",
                  ph_event->event,
                  ph_event->event_info.oprt_mode);

  ph_val = dsat_get_base_addr(DSAT_PH_VALS, TRUE);

  if (NULL == ph_val)
  {
    return DSAT_ASYNC_EVENT;

  }
  DS_AT_MSG2_HIGH("Processing CM PH event: %d, OPRT mode %d",
                  ph_event->event,
                  ph_event->event_info.oprt_mode);
  switch(ph_event->event)
  {
    case CM_PH_EVENT_OPRT_MODE:
    {
      if (ph_val->dsat_oprt_mode != ph_event->event_info.oprt_mode)
      {
        ph_val->dsat_oprt_mode = ph_event->event_info.oprt_mode;
        ph_event->event_info.oprt_mode_changed = TRUE;
      }
      else
      {
        ph_event->event_info.oprt_mode_changed = FALSE;
      }

#ifdef FEATURE_DSAT_ETSI_DATA
      if( ph_event->event_info.oprt_mode != SYS_OPRT_MODE_ONLINE )
      {
        dsat_power_off = TRUE; /* Added to indicate power on detach */
      }
      else
      {
        dsat_power_off = FALSE;
      }
#endif /* FEATURE_DSAT_ETSI_DATA */
      break;
    }
    
#ifdef FEATURE_DSAT_ETSI_MODE 
    case CM_PH_EVENT_SYS_SEL_PREF:
    {
      ds_at_cm_ph_pref_type *pref_info_ptr = &ph_event->event_info.pref_info;

      if(FALSE == dsatcmif_update_network_info(&ph_event->event_info))
      { 
        return DSAT_ASYNC_EVENT;
      }

      /* Update network registration state tracking */
      ph_val->dsat_net_reg_state.cmph_pref.network_sel_mode_pref =
      pref_info_ptr->network_sel_mode_pref;
      ph_val->dsat_net_reg_state.cmph_pref.network_rat_mode_pref =
      pref_info_ptr->network_rat_mode_pref;

      /* Set current value */
      dsatact_set_ws46_current_value(pref_info_ptr->network_rat_mode_pref);
      
      if( (!(( DSAT_COPS_ASTATE_PREF == ph_val->dsatetsicall_network_list.cmd_state) ||
             ( DSAT_COPS_ASTATE_PREF_ONLY == ph_val->dsatetsicall_network_list.cmd_state))) &&
          (TRUE == cmif_is_gw_subscription_available(0, ph_event) ) )
      {
        /* Client other than ATCOP may have made change so update
         * +COPS mode.  The enum values correspond to the command
         * table list indexes so this simplifies update. */
        switch (ph_val->dsat_net_reg_state.cmph_pref.network_sel_mode_pref)
        {
          case CM_NETWORK_SEL_MODE_PREF_AUTOMATIC:
            DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_COPS_ETSI_IDX,0,0,
                                     (dsat_num_item_type)DSAT_COPS_MODE_AUTO,MIX_NUM_TYPE)
            break;
          case CM_NETWORK_SEL_MODE_PREF_MANUAL:
          case CM_NETWORK_SEL_MODE_PREF_LIMITED_SRV:
            DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_COPS_ETSI_IDX,0,0,
                                     (dsat_num_item_type)DSAT_COPS_MODE_MANUAL,MIX_NUM_TYPE)
            break;
          default:
            DS_AT_MSG1_ERROR(" network_sel_mode_pref = %d not handled",
                                   ph_val->dsat_net_reg_state.cmph_pref.network_sel_mode_pref);
            /* No update required */
            break;
        }
      }
      break;
    }
    case CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE:
    if (FALSE == cmif_is_gw_subscription_available(0, ph_event))
    {
     if(FALSE == dsatcmif_update_network_info(&ph_event->event_info ))
     { 
       return DSAT_ASYNC_EVENT;
     }
      DS_AT_MSG0_HIGH("CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE event is successful");
#ifdef FEATURE_FSK_TX
      ph_val->is_gw_subscription_available = FALSE;
#endif /* FEATURE_FSK_TX */
      ph_val->net_pref.last_dereg_success = TRUE;
      ph_val->net_pref.mode = DSAT_COPS_MODE_DEREG;
      DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_COPS_ETSI_IDX,0,0,
                                   (dsat_num_item_type)DSAT_COPS_MODE_DEREG,MIX_NUM_TYPE)
      memset ((void*)&ph_val->dsat_net_reg_state.net_id_info, 0x0,
                               sizeof(ph_val->dsat_net_reg_state.net_id_info));
            
      dsatetsicall_set_net_reg(DSAT_NET_REG_DOMAIN_CS ,DSAT_NET_REG_NONE);
      dsatetsicall_set_net_reg(DSAT_NET_REG_DOMAIN_PS ,DSAT_NET_REG_NONE);
      break;
    }
    case CM_PH_EVENT_SUBSCRIPTION_AVAILABLE:
    if (TRUE == cmif_is_gw_subscription_available(0, ph_event))
    {
     if(FALSE == dsatcmif_update_network_info(&ph_event->event_info ))
     { 
       return DSAT_ASYNC_EVENT;
     }
#ifdef FEATURE_FSK_TX
      ph_val->is_gw_subscription_available = TRUE;
#endif /* FEATURE_FSK_TX */
      ph_val->net_pref.last_dereg_success = FALSE;
      if ( ph_val->net_pref.subscription_invoked == FALSE )
      {
        if(ph_event->event_info.pref_info.network_sel_mode_pref
                                           == CM_NETWORK_SEL_MODE_PREF_MANUAL)
        {
          DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_COPS_ETSI_IDX,0,0,
                                   (dsat_num_item_type)DSAT_COPS_MODE_MANUAL,MIX_NUM_TYPE)
        }
        else
        {
          DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_COPS_ETSI_IDX,0,0,
                                   (dsat_num_item_type)DSAT_COPS_MODE_AUTO,MIX_NUM_TYPE)
        }
      }
      break;
    }

#endif /* FEATURE_DSAT_ETSI_MODE */

    case CM_PH_EVENT_STANDBY_SLEEP:
    {
      ph_val->cm_ph_deep_sleep = TRUE;
      break;
    }
    
    case CM_PH_EVENT_STANDBY_WAKE:
    {
      ph_val->cm_ph_deep_sleep = FALSE;
      break;
    }
    case CM_PH_EVENT_INFO:
    {
#ifdef FEATURE_DSAT_ETSI_MODE 
      if(FALSE == dsatcmif_update_network_info(&ph_event->event_info ))
      { 
        return DSAT_ASYNC_EVENT;
      }
#endif /* FEATURE_DSAT_ETSI_MODE */
 
      if (ph_val->dsat_oprt_mode != ph_event->event_info.oprt_mode)
      {
        ph_val->dsat_oprt_mode = ph_event->event_info.oprt_mode;
        ph_event->event_info.oprt_mode_changed = TRUE;
      }
      else
      {
        ph_event->event_info.oprt_mode_changed = FALSE;
      }
      
#ifdef FEATURE_DSAT_ETSI_MODE      
#ifdef FEATURE_DSAT_LTE
     switch(ph_event->event_info.eps_mode)
     {
       case CM_LTE_UE_MODE_PREF_PS_MODE1:
         eps_mode = DSAT_EPS_PS_MODE_1;
         break;
       case CM_LTE_UE_MODE_PREF_PS_MODE2:  
         eps_mode = DSAT_EPS_PS_MODE_2;
         break;
       case CM_LTE_UE_MODE_PREF_CS_PS_MODE1:
         eps_mode = DSAT_EPS_CS_PS_MODE_1;
         break;
       case CM_LTE_UE_MODE_PREF_CS_PS_MODE2:
         eps_mode = DSAT_EPS_CS_PS_MODE_2;
         break;
       default:
         break;
     }
     if(((dsat_num_item_type)
            dsatutil_get_val( DSATETSI_EXT_CEMODE_IDX,0,NUM_TYPE) != eps_mode)&& 
            (eps_mode != DSAT_EPS_MODE_MAX))
     {
       DSATUTIL_SET_VAL(DSATETSI_EXT_CEMODE_IDX,0,0,eps_mode,NUM_TYPE);
       DS_AT_MSG1_HIGH(" UPDATED CEMODE as %d ",eps_mode);
     }
#endif /* FEATURE_DSAT_LTE */
      /* last_dereg_success needs to be set to FALSE in case SUBSCRIPTION AVAILABLE
         EVENT is missed out after PSM exit due to race condition between CM events and
         ATCOP initializations.*/
      if (TRUE == cmif_is_gw_subscription_available(0, ph_event))
      {
        ph_val->net_pref.last_dereg_success = FALSE;
#ifdef FEATURE_FSK_TX
        ph_val->is_gw_subscription_available = TRUE;
#endif /* FEATURE_FSK_TX */
      }
        /* This condition is satisfied only during initial phone boot-up.
              Update +COPS <mode> using network_sel_mode_pref. */
      if( (dsat_num_item_type)DSAT_COPS_MODE_MAX == 
                              (dsat_num_item_type)dsatutil_get_val(
                              DSATETSI_EXT_ACT_COPS_ETSI_IDX,0,MIX_NUM_TYPE))
      {
        if(ph_event->event_info.pref_info.network_sel_mode_pref
                                              == CM_NETWORK_SEL_MODE_PREF_MANUAL)
        {
          DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_COPS_ETSI_IDX,0,0,
                                            (dsat_num_item_type)DSAT_COPS_MODE_MANUAL,MIX_NUM_TYPE)
        }
        else
        {
          DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_COPS_ETSI_IDX,0,0,
                                            (dsat_num_item_type)DSAT_COPS_MODE_AUTO,MIX_NUM_TYPE)
        }
      }
#endif /* FEATURE_DSAT_ETSI_MODE */
      break;
    }
    default:
    {
      break;
    }
  }

  if (DSAT_DL_VALIDATE_SYMBOL_ADDR(dsatdl_vtable.dsatcmif_cm_ph_event_handler_fp))
  {
    result = dsatdl_vtable.dsatcmif_cm_ph_event_handler_fp(ph_event, result);
  }

  return result;

}/* dsatcmif_cm_ph_event_handler_ex */

/*===========================================================================

FUNCTION DSATCMIF_SIGNAL_HANDLER

DESCRIPTION
  This function processes the asynchronous signals assosiacted with
  messages from Call Manager.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ERROR: if we find an error in procssing the event
  DSAT_OK: if processing of the event is complete and right
  DSAT_ASYNC_CMD : if we still have more events to process
  DSAT_ASYNC_EVENT : if the event is ignored

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatcmif_signal_handler
(
 dsat_mode_enum_type at_mode
)
{
  dsm_item_type         *dsm_item_ptr;
  dsat_cm_msg_s_type    *msg_ptr = NULL;
  dsat_result_enum_type  result = DSAT_ASYNC_EVENT;

  msg_ptr = dsat_alloc_memory(sizeof(dsat_cm_msg_s_type), TRUE);
  if (NULL == msg_ptr)
  {
    return result;
  }

  while ( (dsm_item_ptr = dsm_dequeue( &dsat_cm_msg_wm )) != NULL)
  {
    (void)dsm_pullup( &dsm_item_ptr, msg_ptr, sizeof(dsat_cm_msg_s_type) );

    DS_AT_MSG1_MED("ATCOP:Freed 0x%p DSM item, signal handler", dsm_item_ptr);
    dsm_free_packet( &dsm_item_ptr );
    
    /* Dispatch event message to handler */
    switch( msg_ptr->cmd )
    {
      case DS_CMD_ATCOP_CM_SS_INFO_CMD:
        result = dsatcmif_cm_ss_event_handler_ex( msg_ptr );
        break;

    default:
      DS_AT_MSG1_ERROR("Command = %d", msg_ptr->cmd);
    }

    /* Check for response reporting requirement */
    if( result != DSAT_ASYNC_CMD && result != DSAT_ASYNC_EVENT )
    {
      /* If pending msgs in queue, set signal again */
      if( dsat_cm_msg_wm.current_cnt != 0 )
      {
#ifdef FEATURE_MODEM_RCINIT_PHASE2
        (void)rex_set_sigs( rcinit_lookup_rextask("ds"), GET_SIG(DS_AT_CM_MSG_SIG) );
#else
        (void)rex_set_sigs( &ds_tcb, GET_SIG(DS_AT_CM_MSG_SIG) );
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
      }

      break;
    }
  } /* while */

  dsatutil_free_memory( msg_ptr );

  return result;
} /* dsatcmif_signal_handler */


/*===========================================================================

FUNCTION  DSATCMIF_INIT_CMIF_EX

DESCRIPTION
  CM Interface initialization function. This function does the following:
  - Initializes ATCOP as one of the clients to Call Manager.
  - Setup the Call Manager message watermark.
  - Registers the client call back functions.
  - Requests initial phone info (for preferences).
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmif_init_cmif_ex ( void )
{
  /* Register ATCoP as a client to DS SYS Manager for 
     CM  SS, PH and CALL events  */
  uint8  event_type_mask = DSAT_BM(DSSYSMGR_EVT_TYPE_SS) |
                           DSAT_BM(DSSYSMGR_EVT_TYPE_PH) |
                           DSAT_BM(DSSYSMGR_EVT_TYPE_CALL);

  dssysmgr_client_reg_event(DSSYSMGR_CLIENT_ATCOP,
  	                        event_type_mask,
  	                        dsatcmif_event_cb_func,NULL);

  /* Setup the Call Manager message watermark */
  cmif_setup_msg_watermarks();

  /* Call the registration function to register call back functions */
  cmif_register_cb_func_ex ( );

  dsatcm_client_id = dssysmgr_get_cm_client_id(); 
  DS_AT_MSG2_ERROR("dsatcm_client_id %d %d ",dsatcm_client_id,dssysmgr_get_cm_client_id());


  /* Request phone settings for preferences */
  if (TRUE != cm_ph_cmd_get_ph_info( dsatcmif_ph_cmd_cb_func,
                                     NULL,
                                     dsatcm_client_id ))
  {
    DS_AT_MSG0_ERROR("Problem requesting initial CM PH info ");
  }

  return;
}/* dsatcmif_init_cmif_ex */


/************************ LOCAL FUNCTIONS *************************/

/*===========================================================================
FUNCTION  CMIF_CALL_EVENT_CB_FUNC

DESCRIPTION
  CM Call status event callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer
===========================================================================*/
void dsatcmif_event_cb_func
(
  dssysmgr_cm_event_type event_type,
  dssysmgr_cm_common_event_info cm_common_ev_info
)
{
  switch(event_type)
  {
    case DSSYSMGR_EVT_TYPE_SS:
      cmif_ss_event_cb_func(cm_common_ev_info.ss_ev_info.ss_event,
			                     cm_common_ev_info.ss_ev_info.ss_info_ptr);
    break;
	case DSSYSMGR_EVT_TYPE_PH:
      cmif_cm_ph_event_cb_func(cm_common_ev_info.ph_ev_info.ph_event,
			                   cm_common_ev_info.ph_ev_info.ph_info_ptr);
    break;
	case DSSYSMGR_EVT_TYPE_CALL:
      cmif_call_event_cb_func(cm_common_ev_info.call_ev_info.call_event,
			                  cm_common_ev_info.call_ev_info.call_info_ptr);
    break;
	default:
     DS_AT_MSG1_ERROR("Unsupported event type %d", event_type);
     break;
  }
}

/*===========================================================================
FUNCTION  CMIF_CALL_EVENT_CB_FUNC

DESCRIPTION
  CM Call status event callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer
===========================================================================*/
void cmif_call_event_cb_func
(
  cm_call_event_e_type            event,            /* Event ID              */
  const cm_mm_call_info_s_type   *event_ptr         /* Pointer to Event info */
)
{
  ds_command_type * cmd_buf;
  ds_at_cm_call_event_type*  call_event;
  static cm_call_sups_type_e_type  dsat_sups_type;

  if (event_ptr == NULL)
  {
    DS_AT_MSG0_ERROR("event_ptr is NULL");
    return;
  }

  if( (event == CM_CALL_EVENT_END_REQ) ||
      (event == CM_CALL_EVENT_MOD_BEARER_IND) ||
      (event == CM_CALL_EVENT_GET_PDN_CONN_IND) ||
      (event == CM_CALL_EVENT_PDN_DISCONNECT_REJECT_IND) ||
      (event == CM_CALL_EVENT_SETUP_RES ) ||
      (event == CM_CALL_EVENT_MT_CALL_PAGE_FAIL ) )
  {
    DS_AT_MSG1_LOW("%d Event not required for ATCoP client",event);
    return;
  }
  /* Filter to pass the following events... */
      /* voice/emergency calls,*/
if ( dsatcmif_is_voice_call_type(event_ptr->call_type)

      /* outgoing packet data calls(ETSI only), */
#ifdef FEATURE_DSAT_ETSI_DATA
      || ((CM_CALL_TYPE_PS_DATA == event_ptr->call_type) &&
          (CM_CALL_STATE_INCOM != event_ptr->call_state))
#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_DSAT_ETSI_MODE
       /* Incoming, connected or end events for CS data calls */
      || (CM_CALL_TYPE_CS_DATA == event_ptr->call_type &&
           ( CM_CALL_STATE_INCOM == event_ptr->call_state ||
             CM_CALL_STATE_CONV  == event_ptr->call_state ||
             CM_CALL_STATE_IDLE  == event_ptr->call_state ) )

      || event == CM_CALL_EVENT_PROGRESS_INFO_IND      
      || event == CM_CALL_EVENT_ABRV_ALERT
      || event == CM_CALL_EVENT_CALL_BARRED
      || event == CM_CALL_EVENT_CALL_FORWARDED
      || event == CM_CALL_EVENT_CALL_BEING_FORWARDED
      || event == CM_CALL_EVENT_INCOM_FWD_CALL
      || event == CM_CALL_EVENT_CALL_ON_HOLD 
      || event == CM_CALL_EVENT_CALL_RETRIEVED
      || event == CM_CALL_EVENT_ORIG_FWD_STATUS
      || event == CM_CALL_EVENT_ORIG
      || event == CM_CALL_EVENT_PDN_CONN_REJ_IND
      || event == CM_CALL_EVENT_PDN_CONN_FAIL_IND
      || event == CM_CALL_EVENT_RES_ALLOC_REJ_IND
      || event == CM_CALL_EVENT_RES_ALLOC_FAIL_IND
      || event == CM_CALL_EVENT_ANSWER      
      || event == CM_CALL_EVENT_SETUP_IND 
#endif /* FEATURE_DSAT_ETSI_MODE */
      || event == CM_CALL_EVENT_INFO
      ||( event == CM_CALL_EVENT_PS_SIG_REL_CNF)
      ||( event == CM_CALL_EVENT_HO_COMPLETE)
      ||( event == CM_CALL_EVENT_SRVCC_COMPLETE_IND)
     )
  {
    DS_AT_MSG4_MED("ATCOP: cm event cb, event: %d, type: %d, state: %d, "
                   "mode info type: %d",
                   event, event_ptr->call_type, event_ptr->call_state,
                   event_ptr->mode_info.info_type);

     if( CM_CALL_EVENT_SUPS == event)
     {
       if (CM_CALL_MODE_INFO_GW_CS == event_ptr->mode_info.info_type)
       {
         dsat_sups_type = event_ptr->mode_info.info.gw_cs_call.sups_type;
         DS_AT_MSG1_MED("dsat_sups_type: %d", dsat_sups_type);
       }
       return;
     }

    cmd_buf       = dsat_get_cmd_buf(sizeof(ds_at_cm_call_event_type), TRUE);
    
    if ((NULL != cmd_buf) && (cmd_buf->cmd_payload_ptr != NULL))
    {
      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_CM_CALL_INFO_CMD;
      call_event = cmd_buf->cmd_payload_ptr;

      /* If event_ptr is NULL we shouldn't get this far */
      /* Pleasing lint here                          */
      if( NULL != event_ptr )  /*lint !e774 */
      {
        call_event->event_info.dsat_info_list_avail = FALSE;
        call_event->event_info.call_client_id = (cm_client_type_e_type)event_ptr->call_client_id;
        call_event->event_info.requesting_client_id = (cm_client_type_e_type)event_ptr->requesting_client_id;
        call_event->event_info.call_type = event_ptr->call_type;
        call_event->event_info.call_state = event_ptr->call_state;

        (void) dsatutil_memscpy((void*)&call_event->event_info.num,
                                sizeof(cm_num_s_type),
                                (void*)&event_ptr->num,
                                sizeof(cm_num_s_type));
        call_event->event_info.call_direction =  event_ptr->direction;
        call_event->event_info.sys_mode =  event_ptr->sys_mode;
        call_event->event_info.dsat_mode_info.info_type = event_ptr->mode_info.info_type;

        //TODO: sanjaykc: what mode info type needs to store
        switch (event_ptr->mode_info.info_type)
        {
          case CM_CALL_MODE_INFO_GW_CS:
            call_event->event_info.dsat_mode_info.info.gw_cs_call.bearer_capability_1 =
              event_ptr->mode_info.info.gw_cs_call.bearer_capability_1;
            call_event->event_info.dsat_mode_info.info.gw_cs_call.called_party_bcd_number =
              event_ptr->mode_info.info.gw_cs_call.called_party_bcd_number;
            call_event->event_info.dsat_mode_info.info.gw_cs_call.called_party_subaddress =
              event_ptr->mode_info.info.gw_cs_call.called_party_subaddress;
            call_event->event_info.dsat_mode_info.info.gw_cs_call.calling_party_bcd_number =
              event_ptr->mode_info.info.gw_cs_call.calling_party_bcd_number;
            call_event->event_info.dsat_mode_info.info.gw_cs_call.calling_party_subaddress =
              event_ptr->mode_info.info.gw_cs_call.calling_party_subaddress;
            call_event->event_info.dsat_mode_info.info.gw_cs_call.cc_cause =
              event_ptr->mode_info.info.gw_cs_call.cc_cause;
            call_event->event_info.dsat_mode_info.info.gw_cs_call.cc_reject =
              event_ptr->mode_info.info.gw_cs_call.cc_reject;
            call_event->event_info.dsat_mode_info.info.gw_cs_call.active_calls_list =
              event_ptr->mode_info.info.gw_cs_call.active_calls_list;
            call_event->event_info.dsat_mode_info.info.gw_cs_call.connected_subaddress =
              event_ptr->mode_info.info.gw_cs_call.connected_subaddress;
            call_event->event_info.dsat_mode_info.info.gw_cs_call.cause_of_no_cli =
              event_ptr->mode_info.info.gw_cs_call.cause_of_no_cli;
            call_event->event_info.call_ss_success = event_ptr->mode_info.info.gw_cs_call.call_ss_success;;
            break;

          case CM_CALL_MODE_INFO_IP:
            call_event->event_info.call_ss_success = event_ptr->mode_info.info.ip_call.call_ss_success;
            break;

          case CM_CALL_MODE_INFO_GW_PS:
          case CM_CALL_MODE_INFO_GSM_PS:
          case CM_CALL_MODE_INFO_WCDMA_PS:
            call_event->event_info.dsat_mode_info.info.gw_ps_call.nsapi =
              event_ptr->mode_info.info.gw_ps_call.nsapi;
            call_event->event_info.dsat_mode_info.info.gw_ps_call.pdp_cause_type =
              event_ptr->mode_info.info.gw_ps_call.pdp_cause_type;
            call_event->event_info.dsat_mode_info.info.gw_ps_call.cause =
              event_ptr->mode_info.info.gw_ps_call.cause;
            call_event->event_info.dsat_mode_info.info.gw_ps_call.cause =
              event_ptr->mode_info.info.gw_ps_call.cause;
            call_event->event_info.dsat_mode_info.info.gw_ps_call.ps_sig_rel_status =
              event_ptr->mode_info.info.gw_ps_call.ps_sig_rel_status;

            call_event->event_info.profile_number =
              event_ptr->mode_info.info.gw_ps_call.profile_number;
            break;

          case CM_CALL_MODE_INFO_LTE:
            call_event->event_info.dsat_mode_info.info.lte_call.esm_cause =
              event_ptr->mode_info.info.lte_call.esm_cause;
            call_event->event_info.dsat_mode_info.info.lte_call.esm_local_cause =
              event_ptr->mode_info.info.lte_call.esm_local_cause;

            call_event->event_info.profile_number =
              event_ptr->mode_info.info.lte_call.profile_number;
            break;

          default:
            break;
        }

#ifdef FEATURE_DSAT_ETSI_MODE
        if ( IS_ETSI_MODE(dsatcmdp_get_current_mode()) )
        {
          call_event->event_info.end_status = event_ptr->end_status;
          call_event->event_info.call_direction = event_ptr->direction;
          call_event->event_info.call_id = event_ptr->call_id;
        }
        else
#endif /* FEATURE_DSAT_ETSI_MODE */
        {
          call_event->event_info.call_id = event_ptr->call_id;
        }
      }
      call_event->event      = event;
      ds_put_cmd(cmd_buf);
    }
  }
} /* cmif_call_event_cb_func */

/*===========================================================================
FUNCTION  CMIF_CM_PH_EVENT_CB_FUNC

DESCRIPTION
  CM PH event callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
/* ARGSUSED */
LOCAL void cmif_cm_ph_event_cb_func 
(
  cm_ph_event_e_type         event,            /* Event ID              */
  const cm_ph_info_s_type   *event_ptr         /* Pointer to Event info */
)
{
  ds_command_type           * cmd_buf;
  ds_at_cm_ph_event_type    *ph_event = NULL;
  dsatcmif_servs_state_info *ph_val;

  DS_ASSERT (event_ptr != NULL);

  DS_AT_MSG2_HIGH("IN cmif_cm_ph_event_cb_func oprt mode = %d  event = %d",
                  event_ptr->oprt_mode, event);
  ph_val = dsat_get_base_addr(DSAT_PH_VALS, TRUE);

  /* Filter events to DS task */
  switch (event)
  {
  case CM_PH_EVENT_OPRT_MODE:
  case CM_PH_EVENT_INFO:
  case CM_PH_EVENT_SYS_SEL_PREF:
  case CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE:
  case CM_PH_EVENT_SUBSCRIPTION_AVAILABLE:
  case CM_PH_EVENT_AVAILABLE_NETWORKS_CONF:
  case CM_PH_EVENT_TERMINATE_GET_NETWORKS:
#ifdef FEATURE_ETSI_ATTACH
  case CM_PH_EVENT_PS_ATTACH_FAILED:
#endif /* FEATURE_ETSI_ATTACH */
  case CM_PH_EVENT_STANDBY_SLEEP:
  case CM_PH_EVENT_STANDBY_WAKE:
#ifdef FEATURE_DUAL_SIM
  case CM_PH_EVENT_DUAL_STANDBY_PREF:
#endif /* FEATURE_DUAL_SIM */
#ifdef FEATURE_DSAT_EDRX
  //case CM_PH_EVENT_GET_EDRX_CNF:
  //case CM_PH_EVENT_SET_EDRX_CNF:
  case CM_PH_EVENT_EDRX_CHANGE_INFO_IND:
#endif /* FEATURE_DSAT_EDRX */
    /* Post event to DSTASK */
    cmd_buf = dsat_get_cmd_buf(sizeof(ds_at_cm_ph_event_type), FALSE);

    /* Send the message to ATCOP */
    cmd_buf->hdr.cmd_id           = DS_CMD_ATCOP_CM_PH_INFO_CMD;
    ph_event = cmd_buf->cmd_payload_ptr;
    ph_event->event = event;

    switch (event)
    {
#ifdef FEATURE_DSAT_ETSI_MODE
    case CM_PH_EVENT_AVAILABLE_NETWORKS_CONF:
      if(ph_val == NULL)
      {
        DS_AT_MSG0_HIGH("ph_val is NULL ");
		break;
      }
      if (DSAT_COPS_ASTATE_NULL != ph_val->dsatetsicall_network_list.cmd_state)
      {
        DS_AT_MSG2_MED("available_networks_list_cnf = %d, length = %d",
                        event_ptr->available_networks_list_cnf, 
                        event_ptr->available_networks.length);

     /* Copy the PLMN list only if list_cnf is SYS_PLMN_LIST_SUCCESS else treat the lenght as -1*/        
      if(event_ptr->available_networks_list_cnf == 
          SYS_PLMN_LIST_SUCCESS)
      {
        uint8 i;

        ph_event->event_info.available_networks.length = event_ptr->available_networks.length;

        for (i = 0; (i < event_ptr->available_networks.length) && (i < SYS_PLMN_LIST_MAX_LENGTH); i++)
        {
          ph_event->event_info.available_networks.info[i].plmn =
            event_ptr->available_networks.info[i].plmn;
          ph_event->event_info.available_networks.info[i].plmn_forbidden =
            event_ptr->available_networks.info[i].plmn_forbidden;
          ph_event->event_info.available_networks.info[i].list_category =
            event_ptr->available_networks.info[i].list_category;
          if ( event_ptr->available_networks.info[i].rat == SYS_RAT_LTE_RADIO_ACCESS)
          {
            switch ( event_ptr->available_networks.info[i].ciot_lte_mode )
            {
              case SYS_SYS_MODE_LTE_M1:
                ph_event->event_info.available_networks.info[i].rat= SYS_RAT_LTE_M1_RADIO_ACCESS;
                break; 

              case SYS_SYS_MODE_LTE_NB1:
                ph_event->event_info.available_networks.info[i].rat = SYS_RAT_LTE_NB1_RADIO_ACCESS;
                break;

#ifdef FEATURE_NBIOT_NTN
              case SYS_SYS_MODE_LTE_NB1_NTN:
                ph_event->event_info.available_networks.info[i].rat = SYS_RAT_LTE_NB1_NTN_RADIO_ACCESS;
                break;
#endif /* FEATURE_NBIOT_NTN */

              default:
                ph_event->event_info.available_networks.info[i].rat = SYS_RAT_LTE_RADIO_ACCESS;
             }
           }
          }  
        }
        else
        {
          ph_event->event_info.available_networks.length = INVALID_AVAIL_NET_LENGTH;
        }
      }

#ifdef FEATURE_DSAT_ATCOP_VEND
      else
      {
        dsatutil_memscpy(&(ph_event->event_info.dsat_cell_serach_info),sizeof(sys_cell_search_list_s_type),
                         &(event_ptr->cell_info_list),sizeof(sys_cell_search_list_s_type));
        ph_event->event_info.cell_cnf = event_ptr->available_networks_list_cnf;
      }
#endif
      break;
#endif /* FEATURE_DSAT_ETSI_MODE  */
      
    case CM_PH_EVENT_INFO:
    case CM_PH_EVENT_SYS_SEL_PREF:
#ifdef FEATURE_DSAT_ETSI_MODE 
    case CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE:
    case CM_PH_EVENT_SUBSCRIPTION_AVAILABLE:
#ifdef FEATURE_DSAT_LTE
       if( event == CM_PH_EVENT_INFO )
       {
         ph_event->event_info.eps_mode = event_ptr->lte_ue_mode_pref;
         DS_AT_MSG1_HIGH(" EPS UE MODE - %d ",event_ptr->lte_ue_mode_pref);
       }
#endif /* FEATURE_DSAT_LTE */
      ph_event->event_info.pref_info.service_domain_pref= event_ptr->srv_domain_pref;
      ph_event->event_info.pref_info.network_sel_mode_pref = event_ptr->network_sel_mode_pref;
      ph_event->event_info.pref_info.network_rat_mode_pref = event_ptr->mode_pref;
      ph_event->event_info.pref_info.network_ciot_user_lte_pref = event_ptr->ciot_lte_pref;
#ifdef FEATURE_ATCOP_TEMP_CM_ROAM
      ph_event->event_info.pref_info.network_roam_pref = event_ptr->roam_pref;
#endif
      ph_event->event_info.pref_info.network_band_pref = event_ptr->band_pref;
      ph_event->event_info.pref_info.acq_order_pref = event_ptr->acq_order_pref;
      ph_event->event_info.pref_info.acq_pri_order_pref = event_ptr->acq_pri_order_pref;

#endif /* FEATURE_DSAT_ETSI_MODE */

      ph_event->event_info.oprt_mode = event_ptr->oprt_mode;
      ph_event->event_info.pref_info.network_rat_mode_pref = event_ptr->mode_pref;
      ph_event->event_info.pref_info.network_band_pref = event_ptr->band_pref;

      /* Subscription available info */
      ph_event->event_info.is_gw_subscription_available = 
                                  event_ptr->is_gw_subscription_available;
      break;

    case CM_PH_EVENT_STANDBY_SLEEP:
    case CM_PH_EVENT_STANDBY_WAKE:
      break;
#ifdef FEATURE_DSAT_EDRX
    //case CM_PH_EVENT_GET_EDRX_CNF:
    //case CM_PH_EVENT_SET_EDRX_CNF:
    case CM_PH_EVENT_EDRX_CHANGE_INFO_IND:
      DS_AT_MSG4_HIGH("edrx_enabled = %d rat_type_edrx = %d edrx_cycle_length = %d edrx_ptw = %d", 
      event_ptr->edrx_enabled, event_ptr->rat_type_edrx, event_ptr->edrx_cycle_length, event_ptr->edrx_ptw);
      ph_event->event_info.edrx_info.edrx_enabled = event_ptr->edrx_enabled;
      ph_event->event_info.edrx_info.rat_type_edrx = event_ptr->rat_type_edrx;
      ph_event->event_info.edrx_info.edrx_cycle_length = event_ptr->edrx_cycle_length;
      ph_event->event_info.edrx_info.edrx_ptw = event_ptr->edrx_ptw;
      break;
#endif /* FEATURE_DSAT_EDRX */
    default:  
      ph_event->event_info.oprt_mode = event_ptr->oprt_mode;
      break;
    }

    ds_put_cmd(cmd_buf);
    break;

  default:
    DS_AT_MSG1_ERROR("Unsupported CM PH event: %d", event);
  }
  return;
} /* cmif_cm_ph_event_cb_func */

/*===========================================================================

FUNCTION  CMIF_PROCESS_CM_NET_REG_DATA

DESCRIPTION
  This function processes the Call Manager registration event data to
  set network registration state information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmif_process_cm_net_reg_data
(
  const ds_at_cm_ss_event_type *event_ptr        /* DS event pointer */
)
{
  dsatcmif_servs_state_info *ph_val = NULL;
  dsat_net_reg_state_s_type    *net_reg_state_ptr = NULL;  
#ifdef FEATURE_DSAT_ATCOP_VEND
  dsat_num_item_type                  current_mode = 0;
#endif
  if ( NULL == event_ptr )
  {
    DS_AT_MSG0_ERROR("NULL Event pointerInvalid Arg: event_ptr =");
    return;
  }

  DS_AT_MSG4_HIGH("PLMN structure: 0x%X 0x%X 0x%X %d",
                  event_ptr->event_info.sys_id.id.plmn.identity[0],
                  event_ptr->event_info.sys_id.id.plmn.identity[1],
                  event_ptr->event_info.sys_id.id.plmn.identity[2] ,
                  event_ptr->event_info.srv_status);

  ph_val = dsat_get_base_addr(DSAT_PH_VALS, TRUE);
  if (NULL == ph_val)
  {
    return;
  }
  net_reg_state_ptr = &ph_val->dsat_net_reg_state;

  /* Check for full service and capture operator information.
     If it is a non-full service, the COPS read command should not show
     the PLMN as it is not registerd so reset the PLMN */
  if ( SYS_SRV_STATUS_SRV == event_ptr->event_info.srv_status )
  {

    #define COPS_NULL_PLMN 0xFF

    /* Update locally cached network identifier info */
    if( COPS_NULL_PLMN != event_ptr->event_info.sys_id.id.plmn.identity[0] )
    {
      net_reg_state_ptr->net_id_info.present = TRUE;
      /* sys_id.id is a union, so plmn and plmn_lac would have valid
         values. for readibility, copy it as they are differnt. */
      net_reg_state_ptr->net_id_info.plmn =
        event_ptr->event_info.sys_id.id.plmn;

      /* For now keep it though it is redundant information */
      (void) dsatutil_memscpy((void*)&net_reg_state_ptr->net_id_info.plmn_lac,
                              sizeof(sys_plmn_lac_id_s_type),
                              (void*)&event_ptr->event_info.sys_id.id.plmn_lac,
                              sizeof(sys_plmn_lac_id_s_type));
#ifdef FEATURE_DSAT_LTE
      /* In LTE, LAC is TAC */
      if(event_ptr->event_info.sys_mode == SYS_SYS_MODE_LTE)
      {
        net_reg_state_ptr->net_id_info.plmn_lac.lac = event_ptr->event_info.lte_tac;
      }
#endif /* FEATURE_DSAT_LTE */
    }
    else
    {
      net_reg_state_ptr->net_id_info.present = FALSE;
    }

    net_reg_state_ptr->sys_mode = event_ptr->event_info.sys_mode;    
    net_reg_state_ptr->prev_sys_mode = event_ptr->event_info.sys_mode;
    DS_AT_MSG2_HIGH("Cached Sys mode is %d ,New Sys mode changed from event_ptr %d ",
                     net_reg_state_ptr->prev_sys_mode,net_reg_state_ptr->sys_mode);
    net_reg_state_ptr->ciot_lte_mode = event_ptr->event_info.ciot_lte_mode;
    /* Derive AcT based on system mode */
    switch ( event_ptr->event_info.sys_mode )
    {
    case SYS_SYS_MODE_GSM:
      net_reg_state_ptr->act = DSAT_COPS_ACT_GSM;
      break;
#ifdef FEATURE_DSAT_LTE
    case SYS_SYS_MODE_LTE:
      net_reg_state_ptr->act = DSAT_COPS_ACT_EUTRAN;
      if(event_ptr->event_info.ciot_lte_mode == SYS_SYS_MODE_LTE_M1)
      {
        net_reg_state_ptr->act = DSAT_COPS_ACT_EUTRAN;
      }
      else if(event_ptr->event_info.ciot_lte_mode == SYS_SYS_MODE_LTE_NB1)
      {
        net_reg_state_ptr->act = DSAT_COPS_ACT_EUTRAN_NB_S1;
      }
#ifdef FEATURE_NBIOT_NTN
      else if(event_ptr->event_info.ciot_lte_mode == SYS_SYS_MODE_LTE_NB1_NTN)
      {
        net_reg_state_ptr->act = DSAT_COPS_ACT_SATELLITE_EUTRAN_NB_S1;
      }
#endif /* FEATURE_NBIOT_NTN */

      break;
#endif /* FEATURE_DSAT_LTE */
    default:
      net_reg_state_ptr->act = DSAT_COPS_ACT_MAX;
      break;
    }
  }
  /* Clear network identifier on non-full service */
  else
  {
    /* Clean PLMN & network name information */
    memset((void*)&net_reg_state_ptr->net_id_info, 0x0,
           sizeof(net_reg_state_ptr->net_id_info));
#ifdef FEATURE_DSAT_ATCOP_VEND
     current_mode = (dsat_num_item_type)dsatutil_get_val(
                                                         DSAT_VENDOR_QCJDCFG, 0,NUM_TYPE);
     ph_val->jdstate[DSAT_JD_ACT_GSM].jamming_status = 0;
     ph_val->jdstate[DSAT_JD_ACT_EUTRAN].jamming_status = 0;
     ph_val->jdstate[DSAT_JD_ACT_EUTRAN_NB_S1].jamming_status = 0;
     DS_AT_MSG2_HIGH("net_reg_state_ptr->ciot_lte_mode %d act is %d ",
                      net_reg_state_ptr->ciot_lte_mode,net_reg_state_ptr->act);
     if((current_mode) &&(net_reg_state_ptr->act != DSAT_COPS_ACT_MAX))
     {
        dsat_jamming_status_notify(net_reg_state_ptr->act,
                                   DS_JAMMING_STATE_NOT_DETECTED,
                                   FALSE);
     }
     //Clear CEINFO     
     memset ((void *)&ph_val->ceinfo, 0x0,sizeof(dsat_ceinfo_s_type));
#endif

    net_reg_state_ptr->sys_mode = SYS_SYS_MODE_NO_SRV;

    net_reg_state_ptr->act = DSAT_COPS_ACT_MAX;
    DS_AT_MSG2_HIGH("Old Sys mode is event_ptr %d New Sys mode changed from event_ptr %d ",
                     net_reg_state_ptr->prev_sys_mode,net_reg_state_ptr->sys_mode);

  }

  return;
} /* cmif_process_cm_net_reg_data */


/*===========================================================================

FUNCTION  CMIF_PROCESS_CM_REG_DATA_EX

DESCRIPTION
  This function processes the Call Manager registration event data to
  set PS attach and general network registration state information.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type cmif_process_cm_reg_data_ex
(
  const ds_at_cm_ss_event_type * event_ptr        /* DS event pointer */
)
{
  #define PLMN_NULL 0xFF
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  dsatcmif_servs_state_info  *ph_val = NULL;
  sys_srv_domain_e_type          domain;
  sys_roam_status_e_type         roaming;
  if (NULL == event_ptr)
  {
    DS_AT_MSG0_ERROR("Null event_ptr");
    return result;
  }
  domain = event_ptr->event_info.srv_domain;
  roaming = event_ptr->event_info.roam_status;

#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_DSAT_DEV_CMDS
  /* Update CNTI technology as NONE as UE has not camped yet*/
  dsat_qccnti_tech = DSAT_CNTI_NONE;
#endif /* FEATURE_DSAT_DEV_CMDS */
#endif /* FEATURE_DSAT_ETSI_MODE */

  DS_AT_MSG4_HIGH("Processing registration update: chgfld=%p %p srv=%d dom=%d",
                  ((event_ptr->event_info.changed_fields) >> 32),
                  event_ptr->event_info.changed_fields,
                  event_ptr->event_info.srv_status, domain);

#ifdef FEATURE_DSAT_UMTS_UNDP 
  /* Update CNTI technology as NONE as it has not camped yet*/
  dsat_qccnti_tech = DSAT_CNTI_NONE;
#endif

  ph_val = dsat_get_base_addr(DSAT_PH_VALS, TRUE);
  if (NULL == ph_val)
  {
    return DSAT_ASYNC_EVENT;
  }

  /* Check for network service. */
  /* Use the registration domain (CS,PS,CS+PS) to set the various
   * indicators within the subsystem. */
  if ( SYS_SRV_STATUS_NO_SRV != event_ptr->event_info.srv_status &&
       SYS_SRV_STATUS_PWR_SAVE != event_ptr->event_info.srv_status )
  {
    /* Update camped-only state */
    /* DOMAIN_CAMPED is not exactly registration success. Update PS
       attach state only when there are no active PDNs to avoid unnecessary 
       NW_DETACH CGEV URC.*/
    /* In limited service and no domain, update the net domain. */
    if ( (SYS_SRV_DOMAIN_CAMPED == domain && 
          FALSE == ds_pdn_mgr_is_any_pdn_up()) ||
         (SYS_SRV_DOMAIN_NO_SRV == domain) )
    {
#ifdef FEATURE_ETSI_ATTACH
      /* Set PS attach state */
      DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CGATT_ETSI_IDX,0,0,0,NUM_TYPE)
#endif /*  FEATURE_ETSI_ATTACH */
    }
    /* Update CS+PS registration */
    else if (SYS_SRV_DOMAIN_CS_PS == domain)
    {
#ifdef FEATURE_ETSI_ATTACH
      /* Set PS attach state */
      DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CGATT_ETSI_IDX,0,0,1,NUM_TYPE)
#endif /* FEATURE_ETSI_ATTACH */

      /* Set CS+PS registration state */
      if( SYS_SRV_STATUS_SRV == event_ptr->event_info.srv_status )
      {
#ifdef FEATURE_DSAT_DEV_CMDS
#ifdef FEATURE_DSAT_ETSI_MODE
        if ( event_ptr->event_info.sys_mode == SYS_SYS_MODE_GSM )
        {
          if ( event_ptr->event_info.cell_srv_ind.egprs_supp 
                                          == SYS_EGPRS_SUPPORT_AVAIL )
          {
            dsat_qccnti_tech = DSAT_CNTI_EGPRS;
          }
          else 
          {
            dsat_qccnti_tech = DSAT_CNTI_GPRS;
          }
        }		
#endif /* FEATURE_DSAT_ETSI_MODE */
#endif /* FEATURE_DSAT_DEV_CMDS */
      }
    }
    /* Update CS only registration */
    else if (SYS_SRV_DOMAIN_CS_ONLY == domain)
    {
      /* Set CS registration state */
      if( SYS_SRV_STATUS_SRV == event_ptr->event_info.srv_status )
      {
#ifdef FEATURE_DSAT_DEV_CMDS
#ifdef FEATURE_DSAT_ETSI_MODE
        if ( event_ptr->event_info.sys_mode == SYS_SYS_MODE_GSM )
        {
          dsat_qccnti_tech = DSAT_CNTI_GSM;
        }
#endif /* FEATURE_DSAT_ETSI_MODE */
#endif /* FEATURE_DSAT_DEV_CMDS */
      }
#ifdef FEATURE_ETSI_ATTACH
      /* Set PS attach state */
      DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CGATT_ETSI_IDX,0,0,0,NUM_TYPE)
#endif /*  FEATURE_ETSI_ATTACH */
    }
    /* Update PS only registration */
    else if (SYS_SRV_DOMAIN_PS_ONLY == domain)
    {
#ifdef FEATURE_ETSI_ATTACH
      /* Set PS attach state */
      DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CGATT_ETSI_IDX,0,0,1,NUM_TYPE)
#endif /*  FEATURE_ETSI_ATTACH */
    
      /* Set PS registration state */
      if( SYS_SRV_STATUS_SRV == event_ptr->event_info.srv_status )
      {
      
#ifdef FEATURE_DSAT_DEV_CMDS
#ifdef FEATURE_DSAT_ETSI_MODE
        if ( event_ptr->event_info.sys_mode == SYS_SYS_MODE_GSM)
        {
          if ( event_ptr->event_info.cell_srv_ind.egprs_supp 
                                          == SYS_EGPRS_SUPPORT_AVAIL )
          {
            dsat_qccnti_tech = DSAT_CNTI_EGPRS;
          }
          else 
          {
            dsat_qccnti_tech = DSAT_CNTI_GPRS;
          }
        }
#endif /* FEATURE_DSAT_ETSI_MODE */
#endif /* FEATURE_DSAT_DEV_CMDS */
      }
    }

#ifdef FEATURE_DSAT_ETSI_MODE

    if( SYS_SRV_STATUS_SRV == event_ptr->event_info.srv_status )
    {
      #define COPS_NULL_PLMN 0xFF

      if( COPS_NULL_PLMN != event_ptr->event_info.sys_id.id.plmn.identity[0] )
      {
         /* Update PLMN for emergency list update*/
         (void) dsatutil_memscpy((void*)&ph_val->dsat_cen_num_list.plmn, 
                                  sizeof(sys_plmn_id_s_type),
                                 (void*)&event_ptr->event_info.sys_id.id.plmn,
               sizeof(sys_plmn_id_s_type));
         ph_val->dsat_cen_num_list.plmn_num_flag |= DSAT_CEN_PLMN_PRESENT;
      }

      /* Update service indicator */
      (void)dsatetsime_change_indicator_state( DSAT_CIND_INDEX_SERVICE, 
                                               DSAT_CIND_BINARY_SET );
    }
    else
    {
      /* Update service indicator */
      (void)dsatetsime_change_indicator_state( DSAT_CIND_INDEX_SERVICE,
                                               DSAT_CIND_BINARY_UNSET );
    }

    /* Update +CIND status indicators */
    (void)dsatetsime_change_indicator_state(
              DSAT_CIND_INDEX_ROAM,
              ((SYS_ROAM_STATUS_OFF == roaming)? 
                DSAT_CIND_BINARY_UNSET : DSAT_CIND_BINARY_SET) );
    (void)dsatetsime_change_indicator_state(
              DSAT_CIND_INDEX_PACKET,
              ( ( 1 == (dsat_num_item_type)dsatutil_get_val(
                          DSATETSI_EXT_ACT_CGATT_ETSI_IDX,0,NUM_TYPE) )?
                DSAT_CIND_BINARY_SET : DSAT_CIND_BINARY_UNSET )  );
    
#endif /* FEATURE_DSAT_ETSI_MODE */
  }
  /* Update no service state, thus no registration */
  else 
  {
#ifdef FEATURE_ETSI_ATTACH
    /* Set PS state when not in RLF*/
    if(SYS_SRV_STATUS_NO_SRV != event_ptr->event_info.srv_status ||
       FALSE == ds_pdn_mgr_is_any_pdn_up())
    {
      DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CGATT_ETSI_IDX,0,0,0,NUM_TYPE)
    }      
#endif /*  FEATURE_ETSI_ATTACH */
#ifdef FEATURE_DSAT_ETSI_MODE

    /* Update service indicator */
    (void)dsatetsime_change_indicator_state( DSAT_CIND_INDEX_SERVICE, 0);
    (void)dsatetsime_change_indicator_state( DSAT_CIND_INDEX_PACKET, 0);
    (void)dsatetsime_change_indicator_state( DSAT_CIND_INDEX_ROAM, 0);

#endif /* FEATURE_DSAT_ETSI_MODE */

  }

#ifdef FEATURE_DSAT_ETSI_MODE

  cmif_process_cm_net_reg_data(event_ptr);

  /* Process registration data for CREG and generate URC */
  /* Do not report if +COPS is underway as the lower layers do a few
   * transitions before things settle down. */
  if (DSAT_COPS_ASTATE_NULL == ph_val->dsatetsicall_network_list.cmd_state || 
      SYS_SRV_STATUS_SRV == event_ptr->event_info.srv_status )
  {
    dsatetsicall_creg_state_machine(event_ptr);
  } /* Update CELL ID ,LAC ID TAC ID when cops is under process */
#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_ETSI_ATTACH

#ifdef FEATURE_DSAT_ETSI_DATA
  /* Check if there is a change in the PS state */
  if(dsat_gprs_attach_status != (dsat_num_item_type)dsatutil_get_val(
                              DSATETSI_EXT_ACT_CGATT_ETSI_IDX,0,NUM_TYPE))
  {
    if (dsat_gprs_attach_status == 1)
    {
      /* PS detach: report TE depending on +CGEREP buffer mode settings */
      if((ph_val->dsat_prev_ms_service_preference == CM_SRV_DOMAIN_PREF_CS_PS && 
          ph_val->dsat_ms_service_preference == CM_SRV_DOMAIN_PREF_CS_ONLY) ||
         (dsat_power_off == TRUE) ||
         (ph_val->dsat_ms_service_preference == CM_SRV_DOMAIN_PREF_PS_DETACH))
      {
        /* Detach was initiated by MS */
        dsatetsipkt_report_gprs_event_to_te(
                                         DSAT_MS_INITIATED_DETACH, 
                                         (void *)NULL);
        dsat_power_off = FALSE;
      }
      else
      {
        /* Detach was initiated by N/W */
        dsatetsipkt_report_gprs_event_to_te(
                               DSAT_NW_INITIATED_DETACH, 
                               (void *)NULL);
      }
    }

    dsat_gprs_attach_status = (dsat_num_item_type)dsatutil_get_val(
                              DSATETSI_EXT_ACT_CGATT_ETSI_IDX,0,NUM_TYPE);
  }
#endif /* FEATURE_DSAT_ETSI_DATA */
#endif /* FEATURE_ETSI_ATTACH */

  return result;
} /* cmif_process_cm_reg_data_ex */

#ifdef FEATURE_DSAT_EXTENDED_CMD
/*===========================================================================

FUNCTION  CMIF_REPORT_MODE_3GPP2_EX

DESCRIPTION
  This function reports 3GPP2 system mode when it is changed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmif_report_mode_3gpp2_ex
(
  ds_at_ss_info_s_type *ss_info_ptr
)
{
  dsm_item_type                *res_buff_ptr = NULL;
  sys_sys_mode_e_type           sys_mode = ss_info_ptr->sys_mode;
  dsatcmif_servs_state_info *ph_val = NULL;

  if ( NULL == ss_info_ptr )
  {
    return;
  }

  /* If hybrid is turned ON and any protcol running 
        on the hybrid stack(Secondary) then the sys_mode would be Hybrid 
        otherwise the sys_mode based on the primary stack. */
  if ( TRUE == ss_info_ptr->hdr_hybrid )
  {
    if ( SYS_SRV_STATUS_SRV == ss_info_ptr->hdr_srv_status )
    {
      /* Include HDR in the system mode if HDR has service */
      if ( SYS_SYS_MODE_CDMA == sys_mode )
      {
        /* Type casting it to sys mode e type as hybrid 
          is not available in sys mode tyep */
        sys_mode = (sys_sys_mode_e_type)DSAT_SYS_HYBRID;
      }
      else
      {
        sys_mode = SYS_SYS_MODE_HDR;
      }
    }
  }
  ph_val = dsat_get_base_addr(DSAT_SS_ph_valS, TRUE);
  if (NULL == ph_val)
  {
    return;
  }
  ph_val->dsat_sys_mode = sys_mode;

  return;
} /* cmif_report_mode_3gpp2_ex */
#endif /* FEATURE_DSAT_EXTENDED_CMD */

#if defined(FEATURE_DSAT_SP_CMDS)
/*===========================================================================

FUNCTION  CMIF_DETERMINE_SPSERVICE

DESCRIPTION
  This function determines current service capabilities for $SPSERVICE command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmif_determine_spservice
(
  ds_at_ss_info_s_type *ss_info_ptr
)
{
  if ( NULL == ss_info_ptr )
  {
    return;
  }

  switch ( ss_info_ptr->sys_mode )
  {
    case SYS_SYS_MODE_CDMA:
      dsat_spservice = DSAT_SRV_CDMA;
      break;

    case SYS_SYS_MODE_HDR:
      switch ( ss_info_ptr->hdr_active_prot )
      {
        case SYS_ACTIVE_PROT_HDR_REL0:
          dsat_spservice = DSAT_SRV_EVDO;
          break;

        case SYS_ACTIVE_PROT_HDR_RELA:
          dsat_spservice = DSAT_SRV_EVDO_REV_A;
          break;

        default:
          dsat_spservice = DSAT_SRV_NONE;
          break;
      }
      break;

    default:
      dsat_spservice = DSAT_SRV_NONE;
      break;
  }

  return;
} /* cmif_determine_spservice */
#endif /* defined(FEATURE_DSAT_SP_CMDS) */

/*===========================================================================

FUNCTION  CMIF_PROCESS_RSSI_INFO_3GPP

DESCRIPTION
  This function processes RSSI information for 3GPP mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmif_process_rssi_info_3gpp
(
  const ds_at_cm_ss_event_type *event_ptr /* DS event pointer */
)
{
  dsatcmif_servs_state_info *ph_val = NULL;

  ph_val = dsat_get_base_addr(DSAT_PH_VALS,TRUE);
  if ( ( NULL == event_ptr )||
       ( NULL == ph_val ) )
  {
    DS_AT_MSG2_ERROR("Invalid Arg: event_ptr = %p, ph_val %d",
                     event_ptr, ph_val);
    return;
  }

  dsatutil_memscpy(&ph_val->dsatcmif_signal_reporting,
                   sizeof(dsatcmif_sigrep_s_type),
                   &event_ptr->event_info.dsatcmif_signal_reporting,
                   sizeof(dsatcmif_sigrep_s_type));

  DS_AT_MSG7_MED("rssi = %d, rssi2 = %d, rscp = %d, "
                 "bit_err_rate = %d, ecio = %d, pathloss = %d, sir = %d",
                 ph_val->dsatcmif_signal_reporting.rssi,
                 ph_val->dsatcmif_signal_reporting.rssi2,
                 ph_val->dsatcmif_signal_reporting.rscp,
                 ph_val->dsatcmif_signal_reporting.bit_err_rate,
                 ph_val->dsatcmif_signal_reporting.ecio,
                 ph_val->dsatcmif_signal_reporting.pathloss,
                 ph_val->dsatcmif_signal_reporting.sir);

  (void)dsatetsicmif_process_rssi_event(event_ptr);

  return;
} /* cmif_process_rssi_info_3gpp */

/*===========================================================================

FUNCTION DSATCMIF_GET_OPRT_MODE

DESCRIPTION
  This function return cfun operating mode status.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/

sys_oprt_mode_e_type dsatcmif_get_oprt_mode(void)
{
  dsatcmif_servs_state_info  *ph_val = NULL;
  ph_val = dsat_get_base_addr(DSAT_PH_VALS, TRUE);
  if (NULL == ph_val)
  {
    return SYS_OPRT_MODE_ONLINE;
  }
   return ph_val->dsat_oprt_mode;
   
}/*dsatcmif_get_dsat_oprt_mode */

/*===========================================================================

FUNCTION CMIF_CM_MSG_WM_HIWATER_CB

DESCRIPTION
  This is the high watermark function registered with the watermark for 
  flow control.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_DSM_WM_CB
/*ARGSUSED*/
LOCAL void cmif_cm_msg_wm_hiwater_cb 
(
  struct dsm_watermark_type_s * wm_ptr, 
  void * cb_ptr
)
#else
LOCAL void cmif_cm_msg_wm_hiwater_cb ( void )
#endif
{
#ifdef IMAGE_QDSP6_PROC
  rex_enter_crit_sect(&dsat_cm_wm_crit_sect);
#else
  INTLOCK();
#endif /* IMAGE_QDSP6_PROC*/
  if( FALSE == dsat_cm_msg_wm_full )
  {
    /* Set flag indicating received messages should be dropped */
    dsat_cm_msg_wm_full = TRUE;
    DS_AT_MSG0_HIGH("Enable flow control on CM event queue");
  }  
#ifdef IMAGE_QDSP6_PROC
    rex_leave_crit_sect(&dsat_cm_wm_crit_sect);
#else
    INTFREE();
#endif /* IMAGE_QDSP6_PROC*/

}


/*===========================================================================

FUNCTION CMIF_CM_MSG_WM_LOWATER_CB

DESCRIPTION
  This is the low watermark function registered with the watermark for 
  flow control.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_DSM_WM_CB
/*ARGSUSED*/
LOCAL void cmif_cm_msg_wm_lowater_cb 
(
  struct dsm_watermark_type_s * wm_ptr, 
  void * cb_ptr
)
#else
LOCAL void cmif_cm_msg_wm_lowater_cb ( void )
#endif
{
#ifdef IMAGE_QDSP6_PROC
  rex_enter_crit_sect(&dsat_cm_wm_crit_sect);
#else
  INTLOCK();
#endif /* IMAGE_QDSP6_PROC*/
  if( TRUE == dsat_cm_msg_wm_full )
  {
    /* Set flag indicating received messages should no longer be dropped */
    dsat_cm_msg_wm_full = FALSE;
    DS_AT_MSG0_HIGH("Disable flow control on CM event queue");
  }
#ifdef IMAGE_QDSP6_PROC
  rex_leave_crit_sect(&dsat_cm_wm_crit_sect);
#else
  INTFREE();
#endif /* IMAGE_QDSP6_PROC*/

}


/*===========================================================================

FUNCTION CMIF_CM_MSG_WM_NON_EMPTY_CB

DESCRIPTION
  Callback function invoked when the received message watermark goes non-empty.

  This function simply sets the event received subtask signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_DSM_WM_CB
/*ARGSUSED*/
LOCAL void cmif_cm_msg_wm_non_empty_cb
(
  struct dsm_watermark_type_s * wm_ptr, 
  void * cb_ptr
)
#else
LOCAL void cmif_cm_msg_wm_non_empty_cb ( void )
#endif
{
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  (void)rex_set_sigs( rcinit_lookup_rextask("ds"), GET_SIG(DS_AT_CM_MSG_SIG) );
#else
  (void)rex_set_sigs( &ds_tcb, GET_SIG(DS_AT_CM_MSG_SIG));
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

} /* cmif_cm_msg_wm_non_empty_cb */



/*===========================================================================
FUNCTION  CMIF_SETUP_MSG_WATERMARKS

DESCRIPTION
  This function initializes the event watermarks used for receiving event 
  messages from CM. The lowater, hiwater and don't exceed counts are 
  configured. Queue to be used by WM is also initialized

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void cmif_setup_msg_watermarks( void )
{
//TODO:sanjaykc : critical section
#ifdef IMAGE_QDSP6_PROC
        rex_enter_crit_sect(&dsat_cm_wm_crit_sect);
#else
        INTLOCK();
#endif /* IMAGE_QDSP6_PROC*/
  dsat_cm_msg_wm_full = FALSE;
#ifdef IMAGE_QDSP6_PROC
      rex_leave_crit_sect(&dsat_cm_wm_crit_sect);
#else
      INTFREE();
#endif /* IMAGE_QDSP6_PROC*/

  /*-------------------------------------------------------------------------
    Initialize , set Don't Exceed count and register the associated queue
  -------------------------------------------------------------------------*/
  dsm_queue_init(&dsat_cm_msg_wm, DSAT_FROM_CM_DONT_EXCEED_CNT, &dsat_cm_msg_q);

  /*-------------------------------------------------------------------------
    Set the Lo Water and Hi Water  counts for the watermark.
  -------------------------------------------------------------------------*/
  dsat_cm_msg_wm.lo_watermark    =  DSAT_FROM_CM_LOWATER_CNT;
  dsat_cm_msg_wm.hi_watermark    =  DSAT_FROM_CM_HIWATER_CNT;

  /*-------------------------------------------------------------------------
    Register callback functions for the watermark flow control.
  -------------------------------------------------------------------------*/
  /*lint -save -e64 suppress error 64*/
  dsat_cm_msg_wm.lowater_func_ptr      = cmif_cm_msg_wm_lowater_cb;
  dsat_cm_msg_wm.hiwater_func_ptr      = cmif_cm_msg_wm_hiwater_cb;
  dsat_cm_msg_wm.non_empty_func_ptr    = cmif_cm_msg_wm_non_empty_cb;
  /*lint -restore suppress error 64*/
  dsat_cm_msg_wm_inited = TRUE;
} /* cmif_setup_msg_watermarks */

#if 0
LOCAL uint8 dsat_list_of_cm_ss_event[] =
{
  CM_SS_EVENT_SRV_CHANGED,
  CM_SS_EVENT_RSSI,
  CM_SS_EVENT_INFO,
  CM_SS_EVENT_REG_REJECT,
  CM_SS_EVENT_LTE_RRC_TX_INFO
};

LOCAL uint8 dsat_list_of_cm_ph_event[] =
{
  CM_PH_EVENT_OPRT_MODE,
  CM_PH_EVENT_SYS_SEL_PREF,
  CM_PH_EVENT_STANDBY_SLEEP,
  CM_PH_EVENT_STANDBY_WAKE,
  CM_PH_EVENT_INFO,
  CM_PH_EVENT_PACKET_STATE,
  CM_PH_EVENT_SUBSCRIPTION_AVAILABLE,
  CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE,
  CM_PH_EVENT_AVAILABLE_NETWORKS_CONF,
  CM_PH_EVENT_PS_ATTACH_FAILED,
  CM_PH_EVENT_GET_EDRX_CNF,
  CM_PH_EVENT_SET_EDRX_CNF
  CM_PH_EVENT_EDRX_CHANGE_INFO_IND  
};

LOCAL uint8 dsat_list_of_cm_call_event[] =
{
  CM_CALL_EVENT_ORIG,
  CM_CALL_EVENT_ANSWER,
  CM_CALL_EVENT_END,
  CM_CALL_EVENT_SUPS,
  CM_CALL_EVENT_INCOM,
  CM_CALL_EVENT_CONNECT,
  CM_CALL_EVENT_PDN_CONN_REJ_IND,
  CM_CALL_EVENT_PDN_CONN_FAIL_IND,
  CM_CALL_EVENT_HO_COMPLETE,
  CM_CALL_EVENT_SRVCC_COMPLETE_IND,
  CM_CALL_EVENT_PROGRESS_INFO_IND,  
  CM_CALL_EVENT_CALL_CONF,
  CM_CALL_EVENT_CALL_BARRED
};

LOCAL uint8 dsat_list_of_cm_supl_event[] =
{
  CM_SUPS_EVENT_REGISTER,
  CM_SUPS_EVENT_REGISTER_CONF,
  CM_SUPS_EVENT_ERASE,
  CM_SUPS_EVENT_ERASE_CONF,
  CM_SUPS_EVENT_ACTIVATE,
  CM_SUPS_EVENT_ACTIVATE_CONF,
  CM_SUPS_EVENT_DEACTIVATE,
  CM_SUPS_EVENT_DEACTIVATE_CONF,
  CM_SUPS_EVENT_INTERROGATE,
  CM_SUPS_EVENT_INTERROGATE_CONF,
  CM_SUPS_EVENT_REG_PASSWORD,
  CM_SUPS_EVENT_REG_PASSWORD_CONF,
  CM_SUPS_EVENT_PROCESS_USS,
  CM_SUPS_EVENT_PROCESS_USS_CONF,
  CM_SUPS_EVENT_RELEASE,
  CM_SUPS_EVENT_ABORT,
  CM_SUPS_EVENT_FWD_CHECK_IND,
  CM_SUPS_EVENT_USS_NOTIFY_IND,
  CM_SUPS_EVENT_USS_NOTIFY_RES,
  CM_SUPS_EVENT_USS_IND,
  CM_SUPS_EVENT_USS_RES,
  CM_SUPS_EVENT_RELEASE_USS_IND,
  CM_SUPS_EVENT_GET_PASSWORD_IND,
  CM_SUPS_EVENT_GET_PASSWORD_RES
};


//Move this function to dsatutil file if this approach gets approved
void dsatutil_set_cm_mask(bit_mask_256_t *mask_ptr, int *dsat_list_of_cm_event, uint8 list_size )
{
  uint8 index = 0;

  for(index = 0; index < list_size; index++)
  {
    if(dsat_list_of_cm_event[index] < 64)
    {
      mask_ptr->bits_1_64 |= (1 << dsat_list_of_cm_event[index]);
    }
    else if(dsat_list_of_cm_event[index] < 128)
    {
      mask_ptr->bits_65_128 |= (1 << (dsat_list_of_cm_event[index] - 64));
    }
    else if(dsat_list_of_cm_event[index] < 192)
    {
      mask_ptr->bits_129_192 |= (1 << (dsat_list_of_cm_event[index] - 128));
    }
    else if(dsat_list_of_cm_event[index] < 256)
    {
      mask_ptr->bits_193_256 |= (1 << (dsat_list_of_cm_event[index] - 192));
    }
  }
}
#endif
/*===========================================================================
FUNCTION      DSATCMIF_SET_CM_EVEVNT_MASK

DESCRIPTION
  This function set event mask that needs to registered to CM through DSSYSMGR.

PARAMETERS
  CM event Type.
  CM event Mask pointer

DEPENDENCIES
  CM event mask pointer needs to be appended. Never overwrite.

RETURN VALUE
  None.

SIDE EFFECTS  None.
===========================================================================*/
void dsatcmif_set_cm_event_mask
(
  dssysmgr_cm_event_type event,
  bit_mask_256_t *mask_ptr
)
{
  ds_local_err_val_e_type local_err = DS_LOCAL_ERR_VAL_NONE;
  
  if (mask_ptr == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  if (event < DSSYSMGR_EVT_TYPE_MIN || event >= DSSYSMGR_EVT_TYPE_MAX)
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  switch(event)
  {
    case DSSYSMGR_EVT_TYPE_SS:
    {
	  bit_mask_256_set_bit(mask_ptr,CM_SS_EVENT_SRV_CHANGED);
      bit_mask_256_set_bit(mask_ptr,CM_SS_EVENT_RSSI);
	  bit_mask_256_set_bit(mask_ptr,CM_SS_EVENT_INFO);
	  bit_mask_256_set_bit(mask_ptr,CM_SS_EVENT_REG_REJECT);
	  bit_mask_256_set_bit(mask_ptr,CM_SS_EVENT_LTE_RRC_TX_INFO);
    }
    break;
    case DSSYSMGR_EVT_TYPE_PH:
    {
      bit_mask_256_set_bit(mask_ptr, CM_PH_EVENT_OPRT_MODE);
      bit_mask_256_set_bit(mask_ptr, CM_PH_EVENT_SYS_SEL_PREF);
      bit_mask_256_set_bit(mask_ptr, CM_PH_EVENT_STANDBY_SLEEP);
      bit_mask_256_set_bit(mask_ptr, CM_PH_EVENT_STANDBY_WAKE);
      bit_mask_256_set_bit(mask_ptr, CM_PH_EVENT_INFO);
      bit_mask_256_set_bit(mask_ptr, CM_PH_EVENT_PACKET_STATE);
      bit_mask_256_set_bit(mask_ptr, CM_PH_EVENT_SUBSCRIPTION_AVAILABLE);
      bit_mask_256_set_bit(mask_ptr, CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE);
      bit_mask_256_set_bit(mask_ptr, CM_PH_EVENT_AVAILABLE_NETWORKS_CONF);
      bit_mask_256_set_bit(mask_ptr, CM_PH_EVENT_PS_ATTACH_FAILED);
      bit_mask_256_set_bit(mask_ptr, CM_PH_EVENT_EDRX_CHANGE_INFO_IND);
      bit_mask_256_set_bit(mask_ptr, CM_PH_EVENT_TERMINATE_GET_NETWORKS); 
    }
    break;

    case DSSYSMGR_EVT_TYPE_CALL:
    {
      bit_mask_256_set_bit(mask_ptr, CM_CALL_EVENT_ORIG);
      bit_mask_256_set_bit(mask_ptr, CM_CALL_EVENT_ANSWER);
      bit_mask_256_set_bit(mask_ptr, CM_CALL_EVENT_END);
      bit_mask_256_set_bit(mask_ptr, CM_CALL_EVENT_SUPS);
      bit_mask_256_set_bit(mask_ptr, CM_CALL_EVENT_INCOM);
      bit_mask_256_set_bit(mask_ptr, CM_CALL_EVENT_CONNECT);
      bit_mask_256_set_bit(mask_ptr, CM_CALL_EVENT_PDN_CONN_REJ_IND);
      bit_mask_256_set_bit(mask_ptr, CM_CALL_EVENT_PDN_CONN_FAIL_IND);
      bit_mask_256_set_bit(mask_ptr, CM_CALL_EVENT_HO_COMPLETE);
      bit_mask_256_set_bit(mask_ptr, CM_CALL_EVENT_SRVCC_COMPLETE_IND);
      bit_mask_256_set_bit(mask_ptr, CM_CALL_EVENT_PROGRESS_INFO_IND);
      bit_mask_256_set_bit(mask_ptr, CM_CALL_EVENT_CALL_CONF);
      bit_mask_256_set_bit(mask_ptr, CM_CALL_EVENT_CALL_BARRED);
    }
    break;

	case DSSYSMGR_EVT_TYPE_SUPL:
    {
    }
    break;

    default:
      break;
  }

func_exit:
  if (local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_AT_MSG1_ERROR(" dsatcmif_set_cm_event_mask : err %d", local_err);
  }
  return;
}/* dssysmgr_set_cm_ev_mask_mh */

/*===========================================================================

FUNCTION  CMIF_REGISTER_CB_FUNC_EX

DESCRIPTION
  Registers the callback functions.with other tasks.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmif_register_cb_func_ex ( void )
{
  cm_client_status_e_type cm_status = CM_CLIENT_OK;

  /* Register for blocks of supserv events */
  //TODO: sanjaykc for SUPS
  cm_status = (cm_client_status_e_type)
                  ((int32)cm_status |
                   (int32)cm_client_sups_reg(
                               dsatcm_client_id,
                               dsatetsicmif_sups_event_cb_func,
                               CM_CLIENT_EVENT_REG,
                               CM_SUPS_EVENT_REGISTER,
                               CM_SUPS_EVENT_GET_PASSWORD_RES,
                               NULL
                              ));
  /* This should not happen, raise an error */
  /* Workaround: do not fatal instead log message */
  if (cm_status!= CM_CLIENT_OK)
  {
    DS_AT_MSG1_MED("ATCOP unable to register SUPS events: %d", (int)cm_status);
  }
  return;
}/* cmif_register_cb_func_ex */


/*===========================================================================

FUNCTION  CMIF_START_S_SEVEN_TIMER

DESCRIPTION
  This function Starts S7 timer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmif_start_s_seven_timer
( 
  ds_profile_num_type     profile_id,
  cm_call_type_e_type     call_type,
  cm_call_id_type         cm_call_id,
  cm_client_type_e_type   client_id
)
{
  dsat_timer_s_type *t_ptr = NULL;  

  dsatetsicall_call_state_info * call_val = NULL;
  
  call_val = dsat_get_base_addr(DSAT_CALL_VALS, TRUE);
  if (NULL == call_val)
  {
    return;
  }

  if( ( client_id == (cm_client_type_e_type)dsatcm_client_id ) && 
      ( call_type == CM_CALL_TYPE_VOICE ))
  {
    t_ptr = &dsatutil_timer_table[DSAT_TIMER_VOICE_CALL_TIMEOUT]; 

    DS_ASSERT(t_ptr->user_data != NULL);

    if ( ps_timer_is_running(t_ptr->timer_handle) == TRUE )
    {
      DS_AT_MSG0_HIGH(" Ignoring Start of new S7 timer ");
      return;
    }

    ps_timer_start(t_ptr->timer_handle, (dsat_num_item_type)dsatutil_get_val(
                         DSAT_SREG_S7_IDX,0,NUM_TYPE) * 1000 );

    *(cm_call_id_type*)(t_ptr->user_data) = cm_call_id;
    DS_AT_MSG2_HIGH("VOICE S7 Timer Started for Call ID %d - Duration %d",
              cm_call_id,
              (dsat_num_item_type)dsatutil_get_val(
              DSAT_SREG_S7_IDX,0,NUM_TYPE));
  }
  else if(call_type == CM_CALL_TYPE_PS_DATA)
  {
    if( ( client_id == (cm_client_type_e_type)dsatcm_client_id )
#ifdef FEATURE_ATCOP_TEMP_3GPP_ALT
          || (TRUE == ds3g_msh_is_atcop_call( call_type,cm_call_id,
                    ds_pdn_hdlr_get_current_network_mode())) 
#endif
      )
    {
      t_ptr = &dsatutil_timer_table[DSAT_TIMER_DATA_CALL_TIMEOUT];
      if (ps_timer_is_running(t_ptr->timer_handle) == FALSE)
      {
        call_val->dsatetsicall_s7_info.profile_id = profile_id;
      }
      DS_ASSERT(t_ptr->user_data != NULL);

      ps_timer_start(t_ptr->timer_handle, (dsat_num_item_type)dsatutil_get_val(
                           DSAT_SREG_S7_IDX,0,NUM_TYPE) * 1000);

      *(cm_call_id_type*)(t_ptr->user_data) = cm_call_id;
      DS_AT_MSG2_HIGH("DATA S7 Timer Started for Call ID %d - Duration %d",
                 cm_call_id,
                (dsat_num_item_type)dsatutil_get_val(
                DSAT_SREG_S7_IDX,0,NUM_TYPE));
    }
  }
  else
  {
    DS_AT_MSG1_HIGH("Ignoring S7 Start request for call id %d ", cm_call_id);
  }
  return;
}/* cmif_start_s_seven_timer */

/*===========================================================================

FUNCTION  CMIF_STOP_S_SEVEN_TIMER

DESCRIPTION
  This functions clears the S7 timer if running.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmif_stop_s_seven_timer
( 
  ds_profile_num_type     profile_id,
  cm_call_type_e_type     call_type,
  cm_call_id_type         cm_call_id,
  cm_client_type_e_type   client_id
)
{
  dsat_timer_s_type *t_ptr = NULL;
  dsatetsicall_call_state_info * call_val = NULL;

  call_val = dsat_get_base_addr(DSAT_CALL_VALS, TRUE);
  if (NULL  == call_val)
  {
    return;
  }

  if(( call_type == CM_CALL_TYPE_VOICE ) &&
     (voice_state[cm_call_id].state == DSAT_VOICE_STATE_CONNECTING ))
  {
    t_ptr = &dsatutil_timer_table[DSAT_TIMER_VOICE_CALL_TIMEOUT];

    if(( ps_timer_is_running(t_ptr->timer_handle)) && ( *(cm_call_id_type *)(t_ptr->user_data) == cm_call_id))
    {
      ps_timer_cancel(t_ptr->timer_handle);
      DS_AT_MSG1_HIGH("VOICE S7 Timer Stopped for Call ID - %d",cm_call_id);
    }
  }
  else if(call_type == CM_CALL_TYPE_PS_DATA)
  {
    /* Call is profile and subs based */
    if ((FALSE == call_val->dsatetsicall_s7_info.is_profile_id_valid) ||
        (call_val->dsatetsicall_s7_info.profile_id == profile_id))
    {
      t_ptr = &dsatutil_timer_table[DSAT_TIMER_DATA_CALL_TIMEOUT];
      call_val->dsatetsicall_s7_info.data_s7_abort_handler = NULL;
      call_val->dsatetsicall_s7_info.is_profile_id_valid = FALSE;
    }

    if ( (NULL != t_ptr) && (NULL != t_ptr->user_data) && 
          (cm_call_id == *(cm_call_id_type *)(t_ptr->user_data)) )
    {
      if( ps_timer_is_running(t_ptr->timer_handle) )
      {
        ps_timer_cancel(t_ptr->timer_handle);
        DS_AT_MSG1_HIGH("DATA S7 Timer Stopped for Call ID - %d",cm_call_id);
      }
      *(cm_call_id_type*)(t_ptr->user_data) = CM_CALL_ID_INVALID;
    }
  }
  else
  {
    DS_AT_MSG1_HIGH("Ignoring S7 stop request for call id %d ",cm_call_id);
  }

  return;
}/* cmif_stop_s_seven_timer */

/*===========================================================================

FUNCTION  DSATCMIF_IS_VOICE_CALL_TYPE

DESCRIPTION
  This function determines if the given call is voice call or not

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatcmif_is_voice_call_type( cm_call_type_e_type type)
{
  if(   (CM_CALL_TYPE_VOICE == type)
     || (CM_CALL_TYPE_EMERGENCY == type) 
#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
     || (CM_CALL_TYPE_STD_OTASP == type) 
     || (CM_CALL_TYPE_NON_STD_OTASP == type) 
#endif /*defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)*/
     )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* dsatcmif_is_voice_call_type */
/*===========================================================================
FUNCTION DSATCMIF_INITIALIZE_TO_DEFAULT

DESCRIPTION
  .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmif_initialize_to_default( dsat_mem_type mem_type,void* ptr)
{
#define INVALID_TZ 97
  
  if(NULL == ptr)
  {
    DS_AT_MSG0_ERROR("NULL pointer");
    return;
  }
  switch(mem_type)
  {
    case DSAT_PH_VALS:
     {
       dsatcmif_servs_state_info  *ss = (dsatcmif_servs_state_info  *)ptr;
       ss->cm_ph_deep_sleep = FALSE;
       ss->dsat_oprt_mode = SYS_OPRT_MODE_NONE;
       
       ss->net_pref.last_dereg_success = TRUE;
       memset ((void *)&ss->dsat_net_reg_state, 0x0,sizeof(dsat_net_reg_state_s_type));
       memset ((void *)&ss->dsat_cen_num_list,0x0,sizeof(dsat_cen_emerg_num_list_s_type));
       memset ((void *)&ss->dsatcmif_signal_reporting, 0x0,sizeof(dsatcmif_sigrep_s_type));
#ifdef FEATURE_DSAT_ATCOP_VEND
       memset ((void *)&ss->ceinfo, 0x0,sizeof(dsat_ceinfo_s_type));
       ss->ceinfo.ceinfo_valid = FALSE;
#endif
#ifdef FEATURE_DSAT_ETSI_MODE
       ss->dsatcmif_signal_reporting.rssi = RSSI_NO_SIGNAL;
       ss->dsatcmif_signal_reporting.bit_err_rate = DSAT_BER_NO_SIGNAL;
       ss->dsatcmif_signal_reporting.ecio = DSAT_ECIO_NO_SIGNAL;
       ss->dsatcmif_signal_reporting.pathloss = DSAT_PATHLOSS_NO_SIGNAL;
       ss->dsatcmif_signal_reporting.sir = DSAT_SIR_NO_SIGNAL;
       dsatetsicall_reset_last_reported_creg(ss);
#endif /* FEATURE_DSAT_ETSI_MODE */
  /* HDR CSQ Initailzation */
       ss->dsatcmif_signal_reporting.hdr_hybrid = FALSE;
       ss->dsatcmif_signal_reporting.sys_mode = SYS_SYS_MODE_NO_SRV;
       ss->dsat_ws46_res_type = DSAT_WS46_NONE;
       ss->dsat_ws46_curr_val = DSAT_WS46_FUNC_MAX;
       ss->last_reported_tz = INVALID_TZ;
       ss->dsat_mode_urc_info.mode_val    = AT_MODE_NONE;
       ss->dsat_mode_urc_info.report_flag = FALSE;
#ifdef FEATURE_FSK_TX
       ss->is_gw_subscription_available = FALSE;
#endif /* FEATURE_FSK_TX */
     }
     break;
    case DSAT_CALL_VALS:
     {     
       dsatetsicall_call_state_info *ss = (dsatetsicall_call_state_info *)ptr;
       ss->dsatetsicall_s7_info.profile_id = 0;
       ss->dsatetsicall_s7_info.is_profile_id_valid = FALSE;
       ss->dsatetsicall_s7_info.data_s7_abort_handler = NULL;

       ss->etsicall_was_waiting = FALSE;
       ss->etsicall_was_call_active = FALSE;
       ss->call_info_before_connect.call_id = CM_CALL_ID_INVALID;
       ss->call_info_before_connect.info_type = CM_CALL_MODE_INFO_NONE;
       /* Initialize +CEER reporting */
       memset((void*)&ss->dsatetsicmif_call_end_status, 0x0, 
                             sizeof(dsatetsicmif_ces_info_s_type));
       
       
     }
     break;
    case DSAT_SUPS_VALS:
     {
       dsatetsicmif_sups_state_info  *info = (dsatetsicmif_sups_state_info  *)ptr;
       info->dsat_ss_abort_status =  FALSE;
       memset ((void*)&info->dsat_ss_cm_data, 0x0, sizeof(supserv_cm_op_s_type));  
       dsatetsicmif_set_cusd_state(&(info->dsat_ss_cm_data), DSAT_CUSD_STATE_NULL, 0);
     }
     break;
   default:
    break;
  }
}

/*===========================================================================
FUNCTION CMIF_IS_GW_SUBSCRIPTION_AVAILABLE 

DESCRIPTION
   This function is used to check if we have subscription available
   for the subs_id passed to this function.

DEPENDENCIES
   None

RETURN VALUE
.  TRUE : if the subscription is available for the subs_id passed.
   FALSE: if the subscription is unavailable for the subs_id passed.

SIDE EFFECTS
   None

===========================================================================*/
boolean cmif_is_gw_subscription_available
(
  sys_modem_as_id_e_type     subs_id,
  ds_at_cm_ph_event_type     *ph_event
)
{
  boolean is_gw_subscription_available = FALSE;
  switch(subs_id)
  {
    case SYS_MODEM_AS_ID_1:
    {
      is_gw_subscription_available = ph_event->event_info.is_gw_subscription_available;
      break;
    }
    default:
     is_gw_subscription_available = FALSE;
  }
  
  DS_AT_MSG2_HIGH("subs_id: %d,is_gw_subscription_available:%d",
                  subs_id, is_gw_subscription_available);

  return is_gw_subscription_available;
}/* cmif_is_gw_subscription_available */


