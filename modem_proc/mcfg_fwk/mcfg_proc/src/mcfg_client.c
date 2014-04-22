
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

	mcfg_client.c

GENERAL DESCRIPTION

  MCFG src file for client registration
  
Copyright (c) 2016 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/mcfg_fwk.mpss/7.10.1.1/mcfg_proc/src/mcfg_client.c#1 $
  $DateTime: 2020/02/19 23:07:19 $


  when        who     what, where, why
  ---------   ---     ---------------------------------------------------------------------------
 2016/03/22   cys     Create API file

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "list.h"
#include "mcfg_common.h"
#include "mcfg_int.h"
#include "mcfg_osal.h"
#include "mcfg_refresh.h"
#include "mcfg_task.h"
#include "mcfg_refresh_i.h"
#include "mcfg_client.h"


/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/


/*==============================================================================

                    PUBLIC FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/
/*===========================================================================

  FUNCTION mcfg_client_event_reg_type_valid

  DESCRIPTION
    check the validity of event reg type for mcfg_client_register

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    True:  If valid
    False: Otherwise

  SIDE EFFECTS
    None

===========================================================================*/

boolean mcfg_client_event_reg_type_validity
(
  mcfg_client_event_e_type          event_reg_type
)
{
   boolean  evt_cb = ((event_reg_type & MCFG_CLIENT_EVENT_REFRESH_CB) == MCFG_CLIENT_EVENT_REFRESH_CB);
   boolean  evt_start_comp = (((event_reg_type & MCFG_CLIENT_EVENT_REFRESH_START) == MCFG_CLIENT_EVENT_REFRESH_START) 
                              || ((event_reg_type & MCFG_CLIENT_EVENT_REFRESH_COMPLETE) == MCFG_CLIENT_EVENT_REFRESH_COMPLETE));
   boolean  evt_ssr = ((event_reg_type &MCFG_CLIENT_EVENT_MODEM_RESTART) == MCFG_CLIENT_EVENT_MODEM_RESTART);

   if (event_reg_type == 0) {
      return FALSE;
   }else if(evt_cb && evt_start_comp && evt_ssr){/*Invalid input:client registered with all mutually exclusive event type bit set */
      return FALSE;
   }else if(((evt_cb && evt_start_comp) || (evt_start_comp && evt_ssr) || (evt_cb && evt_ssr)) == TRUE){/*Invalid input:client registered with two mutually exclusive 
                                                                                                         event type bit set */
      return FALSE;
   }else{/*valid input: only one mutually exclusive event type bit is set*/
      return TRUE;
   }
}

/* ==========================================================================
FUNCTION:       MCFG_CLIENT_REGISTER
========================================================================== */
/**
  This function is used to register with MCFG to receive event update through
  registered callback function.
 
  @param[in]
  client_id:       mcfg client ID, use anonymous if not listed or request one.
  event_reg_type:  may register multiple events sharing same registration info structions
                  (like EVENT_REFRESH_START/EVENT_REFRESH_COMPLETE shares same structure, but
                  EVENT_REFRESH_START/COMPLETE and EVENT_REFRESH_CB doesn't share same structure).
  event_reg_info:  Registration info, it should be zero initialized for extension

  @return
  TRUE for success, otherwise FALSE
*/
boolean mcfg_client_register
(
  mcfg_client_id_type               client_id,
  mcfg_client_event_e_type          event_reg_type,
  mcfg_client_event_reg_s_type     *event_reg_info
)
{
   mcfg_task_cmd_s_type *cmd;
   mcfg_refresh_cmd_data_s_type *refresh_cmd_data;

   //Sanity checks for the client registration input
   if (client_id == MCFG_CLIENT_ID_INVALID)
   {
      MCFG_MSG_ERROR_1("Invalid clientID %d ! ", client_id);
      return FALSE;
   }
   if (mcfg_client_event_reg_type_validity(event_reg_type) == FALSE)
   {
      MCFG_MSG_ERROR_1("Invalid event_reg_type %d ! ", event_reg_type);
      return FALSE;
   }
   MCFG_CHECK_NULL_PTR_RET_FALSE(event_reg_info);
 
   if ((event_reg_type & MCFG_CLIENT_EVENT_MODEM_RESTART) == MCFG_CLIENT_EVENT_MODEM_RESTART) 
   {
      //ToDo:Need to revisit this for SSR based client registration/implementation.
      //cmd = mcfg_task_cmd_alloc(MCFG_TASK_CMD_CLIENT_REGISTER, sizeof(mcfg_client_cmd_data_s_type));
   }
   else 
   {
      cmd = mcfg_task_cmd_alloc(MCFG_TASK_CMD_REFRESH, sizeof(mcfg_refresh_cmd_data_s_type));
      if (cmd != NULL)
      {
          refresh_cmd_data = (mcfg_refresh_cmd_data_s_type *) cmd->data;
          refresh_cmd_data->cmd_id = MCFG_REFRESH_CMD_REGISTER_EXT;
          refresh_cmd_data->client_id = client_id;
          refresh_cmd_data->event_reg_type = event_reg_type;
          if ((event_reg_type & MCFG_CLIENT_EVENT_REFRESH_CB) == MCFG_CLIENT_EVENT_REFRESH_CB)
          {
             memscpy(&(refresh_cmd_data->data.register_req),
                  sizeof(mcfg_refresh_registration_s_type),
                  event_reg_info, 
                  sizeof(mcfg_refresh_registration_s_type));
          }
          else
          {
             memscpy(&(refresh_cmd_data->data.refresh_event_reg),
                  sizeof(mcfg_refresh_event_reg_s_type),
                  event_reg_info, 
                  sizeof(mcfg_refresh_event_reg_s_type));
          }
                   
          if (!mcfg_task_cmd_send(cmd))
          {
            MCFG_MSG_ERROR("Couldn't send command for refresh client register!");
            mcfg_task_cmd_free(cmd);
            return FALSE;
          }
      }
   }
   
   return TRUE;  

}

/*===========================================================================

  FUNCTION mcfg_client_cmd_proc

  DESCRIPTION
    process client command

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_client_cmd_proc
(
  mcfg_task_cmd_s_type *cmd
)
{
}

/*===========================================================================

  FUNCTION  mcfg_client_cmd_proc_register

  DESCRIPTION
    process client register cmd

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
/*mcfg_error_e_type mcfg_client_cmd_proc_register
(
  mcfg_client_event_reg_s_type *p_reg
)
{
}*/



/* ==========================================================================
FUNCTION:       MCFG_CLIENT_DEREGISTER
========================================================================== */
/**
  This function is used to deregister with MCFG to stop receiving event updates.
  Note:This api should only be used when using new api "mcfg_client_register" and not with
  the previous api "mcfg_refresh_register".
 
  @param[in]
  client_id:       clientID for the client deregistration,
  event_reg_type:  event registration type(START/COMPLETE) for deregistration of the client.

  @return
  TRUE for success, otherwise FALSE
*/
boolean mcfg_client_deregister
(
  mcfg_client_id_type               client_id,
  mcfg_client_event_e_type          event_reg_type
)
{
   mcfg_task_cmd_s_type *cmd;
   mcfg_refresh_cmd_data_s_type *refresh_cmd_data;

   if (client_id == MCFG_CLIENT_ID_INVALID || client_id == MCFG_CLIENT_ID_ANONYMOUS)
   {
      MCFG_MSG_ERROR_1("Invalid clientID for deregister %d ! ", client_id);
      return FALSE;
   }

   if ((event_reg_type & MCFG_CLIENT_EVENT_MODEM_RESTART) == MCFG_CLIENT_EVENT_MODEM_RESTART) 
   {
      //ToDo:Need to revisit this for SSR based client de-register/implementation.
   }
   else
   {
      cmd = mcfg_task_cmd_alloc(MCFG_TASK_CMD_REFRESH, sizeof(mcfg_refresh_cmd_data_s_type));
      if (cmd != NULL)
      {
          refresh_cmd_data = (mcfg_refresh_cmd_data_s_type *) cmd->data;
          refresh_cmd_data->cmd_id = MCFG_REFRESH_CMD_DEREGISTER;
          refresh_cmd_data->client_id = client_id;
          refresh_cmd_data->event_reg_type = event_reg_type;
                   
          if (!mcfg_task_cmd_send(cmd))
          {
            MCFG_MSG_ERROR("Couldn't send command for refresh client register!");
            mcfg_task_cmd_free(cmd);
            return FALSE;
          }
      }
   }
   return TRUE;
}


