/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


       U I M   POLLING  IMPLEMENTATION

GENERAL DESCRIPTION
  This module contains the procedures for the polling implementation.

Copyright (c) 2015 - 2016, 2018-20 by QUALCOMM Technologies, Inc. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/uimdrv/src/uim_polling.c#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/24/19   ykr     Data exchange check in uim_power_control_ds
06/17/19   nr      Updated correct aid_length
05/24/19   ng      Fixing rel12_polling_enhancement_enable issue
04/03/19   ng      Compiler warning fixes
10/12/18   ku      FR�49111: Reducing RCInit Timeline
08/20/18   dd      Updating DS APIs
11/21/16   gm      Setting poll interval properly
07/19/16   ks      Allow poll command incase of flaky switch
06/03/16   ks      Remove the feature flag UIM_TEMP_DISABLED_POLL_FR
05/16/16   lm      Fix compiler warnings
05/13/16   ssr     Reduce the UIM_MAX_INSTANCES to 1 for single slot build
04/19/16   ssr     Fix compilation error
04/14/16   ks      Removing deregistration from the DS event call back
12/10/15   ks      Fixing issue with the ICC card polling
11/25/15   ks      Setting channel to default channel before POLL command
11/25/15   ks      Fix compiler warnings
11/05/15   ks      Rel12 polling enhancement
===========================================================================*/

#include "uim_polling.h"
#include "mmgsdisessionlib_v.h"
#include "uimdrv_msg.h"
#include "uim_common_efs.h"

/*===========================================================================
  Forward declarations
===========================================================================*/
static void uim_ds_registered_call_back
(
  ds_sys_tech_enum_type                          tech_type,
  uint64                                         event_name,
  void                                         * event_info_ptr,
  void                                         * user_data_ptr
);

/*===========================================================================

FUNCTION UIM_GET_PRESENCE_DETECTION_POLLING_RETURN_TYPE

DESCRIPTION
  This procedure returns the return_type for the presence detection polling

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_get_presence_detection_polling_return_type (
                                  uim_instance_global_type *uim_ptr,
                                  uim_status_return_data_type *return_data )
{
  gstk_slot_id_enum_type	   uim_gstk_slot = GSTK_SLOT_AUTO;

  if(uim_ptr == NULL)
  {
    return;
  }

  /* Slot info to be passed to gstk while checking if Bip is in progress */
  uim_gstk_slot = (uim_ptr->id + 1);

  /* We are in traffic channel request for the directory information
     * and check to make sure that the response match
     */
  if ( ((uim_ptr->state.current_path[uim_ptr->card_cmd.cur_channel].path.len     == 2) &&
        (uim_ptr->state.current_path[uim_ptr->card_cmd.cur_channel].path.path[1] == 0x7FFF)) || 

       (uim_ptr->state.current_path[uim_ptr->card_cmd.cur_channel].path.len     == 3 &&
        uim_ptr->state.current_path[uim_ptr->card_cmd.cur_channel].path.path[1] == 0x7FFF &&
        uim_ptr->state.current_path[uim_ptr->card_cmd.cur_channel].is_ef        == TRUE)
     )
  {
    *return_data = UIM_RETURN_DF_NAME_OF_CURR_APP;
    UIMDRV_MSG_HIGH_2(uim_ptr->id,"Internal command to Poll the UIM cur_channel 0x%x aid_length 0x%x",
                      uim_ptr->card_cmd.cur_channel,
                      uim_ptr->state.current_path[uim_ptr->card_cmd.cur_channel].curr_app.aid_length);
    uim_ptr->command.static_cmd_buf.status.aid.aid_length =  
             uim_ptr->state.current_path[uim_ptr->card_cmd.cur_channel].curr_app.aid_length;
  }
  else
  {
    *return_data = UIM_RETURN_RSP_IDENTICAL_TO_SELECT;
  }
  if ((uim_ptr->flag.disable_return_df_fcp_inpoll)
       || (estk_uim_is_bip_session_in_progress(uim_gstk_slot))
     )
  {
    /*
     * Before card activation or during BIP session,
     * do not send STATUS command that returns FCP or current DF.
     */
    *return_data = UIM_RETURN_NONE;
  }
} /* uim_get_presence_detection_polling_return_type */


/*===========================================================================

FUNCTION UIM_POLL

DESCRIPTION
  This procedure builds the Internal Poll command to ping the UIM and issues
  the command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void uim_poll(uim_instance_global_type *uim_ptr,
	                  uim_polling_type          polling_type,
                   	  rex_sigs_type            *mask)
{
  uim_protocol_type protocol;
  uim_status_return_data_type polling_return_data = UIM_RETURN_NONE;

  protocol = uim_ptr->state.dfs_1.protocol;

  /* Always do the polling on the default channel */
  uim_ptr->card_cmd.cur_channel = UIM_DEFAULT_CHANNEL;

  if(polling_type == UIM_UICC_PRESENCE_DETECTION_POLLING)
  {
    uim_get_presence_detection_polling_return_type(uim_ptr, &polling_return_data); 
  }
  else if (polling_type == UIM_ICC_CARD_POLLING)
  {
    polling_return_data =  UIM_RETURN_RSP_IDENTICAL_TO_SELECT; 
  }
  
  uim_ptr->command.static_cmd_buf.status.return_data = polling_return_data;

  if (mask != NULL)
	*mask &= ~UIM_CMD_Q_SIG;

  uim_ptr->flag.static_buffer_used = TRUE;

  uim_ptr->command.static_cmd_buf.hdr.command = UIM_INTERNAL_POLL_F;
  uim_ptr->command.static_cmd_buf.hdr.channel = UIM_DEFAULT_CHANNEL;
  uim_ptr->command.static_cmd_buf.hdr.cmd_hdr.task_ptr = uim_ptr->tcb_ptr;
  uim_ptr->command.static_cmd_buf.hdr.cmd_hdr.sigs = NULL;
  uim_ptr->command.static_cmd_buf.hdr.cmd_hdr.link.next_ptr = NULL;
  uim_ptr->command.static_cmd_buf.hdr.cmd_hdr.done_q_ptr = NULL;
  uim_ptr->command.static_cmd_buf.hdr.rpt_function = NULL;
  /* Reset the static command count */
  uim_ptr->command.static_cmd_buf.hdr.cmd_count = 0;

  UIMDRV_MSG_HIGH_0(uim_ptr->id,"Internal command to Poll the UIM");

  if (protocol == UIM_UICC)
  {
    uim_ptr->command.static_cmd_buf.hdr.protocol  = UIM_UICC;
    uim_ptr->command.static_cmd_buf.status.status = UIM_NO_INDICATION;
  }
  else
  {
    uim_ptr->command.static_cmd_buf.hdr.protocol = UIM_ICC;
  }

  /* Clear the flag */
  uim_ptr->flag.poll_pending = FALSE;

  /* Make command pointer point to static command buffer */
  uim_ptr->command.cmd_ptr = &uim_ptr->command.static_cmd_buf;

  /* Process the command */
  uim_process_command( mask, uim_ptr);
} /* uim_poll */


/*===========================================================================

FUNCTION UIM_CONVERT_SLOT_TO_SUBID

DESCRIPTION
  This procedure maps the slot parameter passed as input with the sub id
  the command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void uim_convert_slot_to_subid(uim_slot_type slot, ds_sys_subscription_enum_type *subscription_id)
{
  mmgsdi_session_info_query_type query;
  mmgsdi_session_get_info_type info;
	
  query.query_type = MMGSDI_SESSION_INFO_QUERY_BY_TYPE;

  query.query_key.session_type = MMGSDI_GW_PROV_PRI_SESSION;	
  mmgsdi_session_get_info_sync(query, &info);
  if(info.slot_id == (mmgsdi_slot_id_enum_type)slot)
  {
    *subscription_id = DS_SYS_PRIMARY_SUBS;
    return;
  }

  query.query_key.session_type = MMGSDI_GW_PROV_SEC_SESSION;
  mmgsdi_session_get_info_sync(query, &info);
  if(info.slot_id == (mmgsdi_slot_id_enum_type)slot)
  {
    *subscription_id = DS_SYS_SECONDARY_SUBS;
    return;
  }

  query.query_key.session_type = MMGSDI_GW_PROV_TER_SESSION;
  mmgsdi_session_get_info_sync(query, &info);
  if(info.slot_id == (mmgsdi_slot_id_enum_type)slot)
  {
    *subscription_id = DS_SYS_TERTIARY_SUBS;
    return;
  }

  query.query_key.session_type = MMGSDI_1X_PROV_PRI_SESSION;	
  mmgsdi_session_get_info_sync(query, &info);
  if(info.slot_id == (mmgsdi_slot_id_enum_type)slot)
  {
    *subscription_id = DS_SYS_PRIMARY_SUBS;
    return;
  }

  query.query_key.session_type = MMGSDI_1X_PROV_SEC_SESSION;
  mmgsdi_session_get_info_sync(query, &info);
  if(info.slot_id == (mmgsdi_slot_id_enum_type)slot)
  {
    *subscription_id = DS_SYS_SECONDARY_SUBS;
    return;
  }

  query.query_key.session_type = MMGSDI_1X_PROV_TER_SESSION;
  mmgsdi_session_get_info_sync(query, &info);
  if(info.slot_id == (mmgsdi_slot_id_enum_type)slot)
  {
    *subscription_id = DS_SYS_TERTIARY_SUBS;
    return;
  }
}


/*===========================================================================

FUNCTION UIM_IS_PS_CALL_DATA_EXCHANGE_HAPPENING

DESCRIPTION
  This procedure calls the DS IOCTL and indicates if there is a data exchange happening

DEPENDENCIES
  None

RETURN VALUE
  Boolean - TRUE/FALSE based on whether data exchange is happening or not

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_is_ps_call_data_exchange_happening(uim_instance_global_type *uim_ptr)
{
  int16                                              ds_result                  =  -1;
  int16                                              dss_errno                  =  0;
  ds_sys_ioctl_global_stats_type                     data_stats                 = {FALSE};
  boolean                                            data_exchange_happening    = FALSE;
  
  ds_result = ds_sys_ioctl( DS_SYS_TECH_ALL,
                            DS_SYS_IOCTL_GET_GLOBAL_STATS,
                            &data_stats,
                            &dss_errno 
                           );

  if(ds_result == 0)
  {
    UIMDRV_MSG_HIGH_2( uim_ptr->id,"the uim_is_ps_call_data_exchange_happening passed with glb cnt 0x%x ds count 0x%x",
		uim_ptr->state.uim_call_info.cached_data_stats.global_pkt_cnt,data_stats.global_pkt_cnt);
    if(uim_ptr->state.uim_call_info.cached_data_stats.global_pkt_cnt != data_stats.global_pkt_cnt)
    { 
      data_exchange_happening = TRUE;
    }
    uim_ptr->state.uim_call_info.cached_data_stats.global_pkt_cnt = data_stats.global_pkt_cnt;
  }
  return data_exchange_happening;

}

/*===========================================================================

FUNCTION UIM_REGISTER_FOR_DS_NOTIFICATION_FOR_DATA_EXCHANGE

DESCRIPTION
  This procedure registers with the DS interface so that UIM gets indication when
  there is a data exchange

DEPENDENCIES
  None

RETURN VALUE
  Boolean - TRUE/FALSE based on the registration being succesful or not

SIDE EFFECTS
  None

===========================================================================*/
static boolean uim_register_for_ds_notification_for_data_exchange(uim_instance_global_type *uim_ptr)
{
  int16 ds_result,dss_errno;

  boolean ret_value = TRUE;


  if(uim_ptr->state.uim_call_info.registered_for_data_exchange_notfication == FALSE)
  {
  
    ds_result = ds_system_event_reg( DS_SYS_TECH_ALL,
                                     DS_SYS_EVENT_DATA_ACTIVITY_RESUMED,
                                     uim_ds_registered_call_back,
                                     uim_ptr,
                                     &dss_errno
                                   );

    if(ds_result != 0)
    {
      UIMDRV_MSG_HIGH_0( uim_ptr->id,"the uim_register_for_ds_notification_for_data_exchange failed");
      ret_value = FALSE; 
    }
    else
    {
      uim_ptr->state.uim_call_info.registered_for_data_exchange_notfication = TRUE;
	  uim_ptr->poll.presence_detection_state = UIM_PD_POLLING_SUSPENDED;
    }

    /* Setting back to proactive polling as data exchange is not happening */
    uim_util_set_proactive_poll_timer(uim_ptr->setting.proactive_time_delay,
                        uim_ptr->setting.wakeup_poll_offset,
                        UIM_CHANGE_PROACTIVE_POLLING_INTERVAL_INTERNAL_REQ,
                        uim_ptr);
  }
  else
  {
    UIMDRV_MSG_HIGH_0 ( uim_ptr->id,"Did not Register as we already registered with the DS notification");
  }
  return ret_value;
}

/*===========================================================================

FUNCTION UIM_DEREGISTER_FOR_DS_NOTIFICATION

DESCRIPTION
  This procedure de-registers with the DS interface

DEPENDENCIES
  None

RETURN VALUE
  Boolean - TRUE/FALSE based on the de-registration being succesful or not

SIDE EFFECTS
  None

===========================================================================*/
static boolean uim_deregister_for_ds_notification(uim_instance_global_type *uim_ptr)
{
  int16 ds_result,dss_errno;
  boolean ret_value = TRUE;
		
  if(uim_ptr->state.uim_call_info.registered_for_data_exchange_notfication == TRUE)
  {

    ds_result = ds_system_event_dereg( DS_SYS_TECH_ALL,
                                       DS_SYS_EVENT_DATA_ACTIVITY_RESUMED,
                                       uim_ds_registered_call_back,
                                       &dss_errno
                                     );

    if(ds_result != 0)
    {
      UIMDRV_MSG_HIGH_0( uim_ptr->id,"the uim_deregister_for_ds_notification failed");
      ret_value = FALSE; 
    }
    else
    {
      uim_ptr->state.uim_call_info.registered_for_data_exchange_notfication = FALSE;
    }
    
    /* Setting back to presence detection polling as data exchange is happening */
    uim_util_set_proactive_poll_timer(UIM_POLL_TIME,
                        uim_ptr->setting.wakeup_poll_offset,
                        UIM_CHANGE_PROACTIVE_POLLING_INTERVAL_INTERNAL_REQ,
                        uim_ptr);    
  }
  return ret_value;
}

/*===========================================================================

FUNCTION UIM_DS_REGISTERED_CALL_BACK

DESCRIPTION
  This procedure is a callback from DS when there is a data exchange

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void uim_ds_registered_call_back
(
  ds_sys_tech_enum_type                          tech_type,
  uint64                                         event_name,
  void                                         * event_info_ptr,
  void                                         * user_data_ptr
)
{
  uim_instance_global_type *uim_ptr;
  uim_ptr = (uim_instance_global_type *) user_data_ptr;

  if(uim_ptr == NULL)
  {
     return;
  }

  UIMDRV_MSG_HIGH_1 ( uim_ptr->id,"In uim_ds_registered_call_back 0x%x", event_name);

  if(event_name == DS_SYS_EVENT_DATA_ACTIVITY_RESUMED)
  {    
    (void) rex_set_sigs(uim_ptr->tcb_ptr, UIM_POLL_TIMER_SIG);
  }
  return;
}

/*===========================================================================

FUNCTION UIM_PROCESS_POLLING

DESCRIPTION
  This procedure decides on the return type of the POLL command
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_process_polling
(
  rex_sigs_type            *mask_ptr,
  uim_instance_global_type *uim_ptr
)
{
  uim_polling_type polling_type = UIM_POLLING_NONE;
  boolean          rel12_polling_enhancement_enable = FALSE;

  if(uim_ptr == NULL)
  {
    return;
  }

  if(uim_ptr->state.dfs_1.protocol == UIM_ICC)
  {
    /* If it is a ICC card, then STATUS cmd will expect 22 bytes of return data 'identical to select'  */
    uim_poll(uim_ptr, UIM_ICC_CARD_POLLING, mask_ptr);
    return;
  }
  
  if (uim_ptr->poll.rel12_polling_enhancement_enable == UIM_REL12_POLLING_ENHANCHMENT_ENABLE_UNKNOWN)
  {	
    if(UIM_COMMON_EFS_SUCCESS == uim_common_efs_read(UIM_COMMON_EFS_UIMDRV_REL12_POLLING_OPTIMIZATION_FILE,
                                                    UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                                    UIM_COMMON_EFS_DEVICE,
                                                    (uint8 *)&(rel12_polling_enhancement_enable),
                                                    sizeof(rel12_polling_enhancement_enable)))
    {
      if(rel12_polling_enhancement_enable == TRUE)
      {
        uim_ptr->poll.rel12_polling_enhancement_enable = UIM_REL12_POLLING_ENHANCHMENT_ENABLE_ON;
      }
      else
      {
        uim_ptr->poll.rel12_polling_enhancement_enable = UIM_REL12_POLLING_ENHANCHMENT_ENABLE_OFF;
      }
      UIMDRV_MSG_HIGH_1(uim_ptr->id, "Successfully read the UIM_COMMON_EFS_UIMDRV_REL12_POLLING_OPTIMIZATION_FILE from EFS and enable state is 0x%x [1 = disabled, 2 = enabled]", uim_ptr->poll.rel12_polling_enhancement_enable);
    }
    else
    {
      uim_ptr->poll.rel12_polling_enhancement_enable = UIM_REL12_POLLING_ENHANCHMENT_ENABLE_OFF;
      UIMDRV_MSG_HIGH_0(uim_ptr->id, "Failed to read UIM_COMMON_EFS_UIMDRV_REL12_POLLING_OPTIMIZATION_FILE, force set to disabled");
    } 
  }
   
  if((uim_ptr->poll.is_proactive_polling_enabled == TRUE) ||
     (uim_ptr->hotswap.hotswap_card_removed_suspicious == TRUE))
  {
    UIMDRV_MSG_HIGH_0 ( uim_ptr->id,"Pro act polling is ON");
    polling_type = UIM_UICC_PROACTIVE_POLLING;
  }

  if ((int)uim_ptr->state.ctl & UIM_TRAF_CHAN) /* For presence detection polling - start */
  {
    UIMDRV_MSG_HIGH_0 ( uim_ptr->id,"In uim_process_polling under traf channel");
    if(uim_ptr->poll.rel12_polling_enhancement_enable == UIM_REL12_POLLING_ENHANCHMENT_ENABLE_OFF)
    {
      uim_poll(uim_ptr, UIM_UICC_PRESENCE_DETECTION_POLLING, mask_ptr);
      return;
    }

    /* Any non PS call on going scenario */
    if(uim_ptr->state.uim_call_info.call_mask & MMGSDI_MASK_NON_PS_CALL_ONGOING)
    {
      uim_ptr->poll.presence_detection_state = UIM_PD_POLLING_ACTIVE;
      polling_type = UIM_UICC_PRESENCE_DETECTION_POLLING;
      UIMDRV_MSG_HIGH_1 ( uim_ptr->id,"Non PS data traffic ON with mask 0x%x", uim_ptr->state.uim_call_info.call_mask );
    }

    /* PS call on going scenario - start */
    if(uim_ptr->state.uim_call_info.call_mask & MMGSDI_MASK_PS_CALL_ONGOING)
    {
      UIMDRV_MSG_HIGH_1 ( uim_ptr->id,"PS data traffic ON with mask 0x%x", uim_ptr->state.uim_call_info.call_mask );
      if (uim_is_ps_call_data_exchange_happening(uim_ptr)) /* Is data exchange happening - start */
      {
        UIMDRV_MSG_HIGH_0 ( uim_ptr->id,"Confirmed that data exchange is happening");
        uim_deregister_for_ds_notification(uim_ptr);
        uim_ptr->poll.presence_detection_state = UIM_PD_POLLING_ACTIVE;
        polling_type = UIM_UICC_PRESENCE_DETECTION_POLLING;
      }
      else
      {
        UIMDRV_MSG_HIGH_0 ( uim_ptr->id,"No data exchange is happening");
        if(polling_type != UIM_UICC_PRESENCE_DETECTION_POLLING)
        {          
         uim_register_for_ds_notification_for_data_exchange(uim_ptr);
        }
      } /* Is data exchange happening - end */
    } /*  PS call on going scenario   - end */
  } /* For presence detection polling - end */

  if(polling_type != UIM_POLLING_NONE)
  {
    UIMDRV_MSG_HIGH_2 ( uim_ptr->id,"Polling type 0x%x and pd state 0x%x ", polling_type, uim_ptr->poll.presence_detection_state );
    uim_poll(uim_ptr, polling_type, mask_ptr);
  }
  else
  {
    uim_ptr->flag.poll_pending = FALSE;
    UIMDRV_MSG_HIGH_1 ( uim_ptr->id,"Polling is not required pd state 0x%x", uim_ptr->poll.presence_detection_state );
  }

} /* uim_process_polling() */

