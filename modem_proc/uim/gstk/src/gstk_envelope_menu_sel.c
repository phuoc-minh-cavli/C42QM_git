/*===========================================================================


            G S T K   E N V E L O P E   M E N U   S E L


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //components/rel/uim.mpss/3.3.1/gstk/src/gstk_envelope_menu_sel.c#1 $

when         who     what, where, why
--------   ---     --------------------------------------------------------------------
12/04/17   vr      End proactive session when MENU SELECTION command response is not 91XX
06/03/16   shr     GSTK/ESTK globals memory optimization
05/31/16   shr     F3 reduction: Dumping Arrays
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
02/05/16   gs       Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk_envelope_cmd.h"
#include "gstk_envelope_rsp.h"
#include "gstkutil.h"

/*===========================================================================
FUNCTION gstk_menu_selection_ind

DESCRIPTION

  This function builds the menu selection command and sends it to UIM

PARAMETERS
  menu_sel_cmd: [Input] Pointer to the envelope command to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_menu_selection_ind(
  const gstk_envelope_menu_selection_ind_type * menu_sel_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_envelope_menu_selection_command_type   STK_envelope_cmd;
   uim_cmd_type                                *uim_cmd_ptr;
   int                                         offset = 0;

   if(menu_sel_cmd == NULL)
   {
     UIM_MSG_ERR_0("menu_sel_cmd ERR : NULL");
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_menu_selection_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.tag = GSTK_MENU_SELECTION_TAG;

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
       GSTK_KEYPAD_DEVICE,
       GSTK_UICC_SIM_DEVICE,
       &STK_envelope_cmd.device_id );

   if (gstk_status != GSTK_SUCCESS) 
   {
       return gstk_status;
   }

   /* item ID tag */
   gstk_status = gstk_packer_item_id_tlv(
       menu_sel_cmd->chosen_item_id,
       &STK_envelope_cmd.item_chosen);

   if (gstk_status != GSTK_SUCCESS) 
   {
       return gstk_status;
   }

   /* help request tag */
   if(menu_sel_cmd->request_help) 
   { 
     /* user requested help */
     gstk_status = gstk_packer_help_request_tlv(
         menu_sel_cmd->request_help,
         &STK_envelope_cmd.request_help);

     if (gstk_status != GSTK_SUCCESS) 
     {
         return gstk_status;
     }
   }

   /* total data length:  device id data + 2 (2 is for the tag and length field)
                        + item chosen data + 2
                        + request help data + 2
   */
   STK_envelope_cmd.length = STK_envelope_cmd.device_id.device_tag_length +
                             STK_envelope_cmd.item_chosen.length +
                             4;
   if(menu_sel_cmd->request_help) 
   { 
     /* user requested help */
     STK_envelope_cmd.length = STK_envelope_cmd.length +
                               STK_envelope_cmd.request_help.length +
                               2;
   }

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) 
   {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) 
     {
         /* required so that uim will grap the data from index 0 */
         uim_cmd_ptr->envelope.offset = 0;

         /* set user_data */
         uim_cmd_ptr->hdr.user_data               = menu_sel_cmd->message_header.user_data;

         /* populate the envelope command info */
         uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;

         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.tag;
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.length;

         /* copy device */
         /*STK_envelope_cmd.device_id.device_tag_length fixed in packer function */
         if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                    &STK_envelope_cmd.device_id,
                                    offset,
                                    (size_t)(STK_envelope_cmd.device_id.device_tag_length +
                                             GSTK_TAG_LENGTH_LEN),
                                    sizeof(uim_cmd_ptr->envelope.data),
                                    sizeof(gstk_device_identities_tag_tlv_type)) <
            (size_t)(STK_envelope_cmd.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN))
         {
           gstk_status = GSTK_MEMORY_ERROR;
         }
         /* copy item id */
         /* STK_envelope_cmd.item_chosen.length fixed in packer function */
         else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                         &STK_envelope_cmd.item_chosen,
                                         offset +=
                                         (int)(STK_envelope_cmd.device_id.device_tag_length +
                                               GSTK_TAG_LENGTH_LEN),
                                         (size_t)(STK_envelope_cmd.item_chosen.length +
                                                  GSTK_TAG_LENGTH_LEN),
                                         sizeof(uim_cmd_ptr->envelope.data),
                                         sizeof(gstk_item_identifier_tlv_type)) <
                 (size_t)(STK_envelope_cmd.item_chosen.length + GSTK_TAG_LENGTH_LEN))
         {
           gstk_status = GSTK_MEMORY_ERROR;
         }

         offset = (int)(offset + STK_envelope_cmd.item_chosen.length + GSTK_TAG_LENGTH_LEN);

         /* copy help requested */
         /* STK_envelope_cmd.request_help.length fixed in packer function */
         if(gstk_status == GSTK_SUCCESS &&
          offset < (STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN) ) 
         { /* has help */
           if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                      &STK_envelope_cmd.request_help,
                                      offset,
                                      (size_t)(STK_envelope_cmd.request_help.length +
                                               GSTK_TAG_LENGTH_LEN),
                                      sizeof(uim_cmd_ptr->envelope.data),
                                      sizeof(gstk_help_request_tlv_type)) <
              (size_t)(STK_envelope_cmd.request_help.length + GSTK_TAG_LENGTH_LEN))
            {
              gstk_status = GSTK_MEMORY_ERROR;
            }
         }

         if (gstk_status == GSTK_SUCCESS)
         {
           gstk_util_dump_byte_array("Env: Menu Sel",
                                     &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                                     uim_cmd_ptr->envelope.num_bytes);            
           /* enable gstk_is_curr_session_ui_related to send end proactive session 
              if session is started due to MENU SELECTION */
           gstk_curr_inst_ptr->gstk_is_curr_session_ui_related = TRUE;
           gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
         }
         else
     {
       gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
       uim_cmd_ptr = NULL;
     }
     }
   } /* uim_cmd_ptr != NULL */
   else 
   {
     gstk_status = GSTK_MEMORY_ERROR;
   }

   /* nothing to free for menu selection ind */
   return gstk_status;

} /* gstk_menu_selection_ind */

/*===========================================================================
FUNCTION gstk_menu_selection_rsp

DESCRIPTION

  This function builds the menu selection rsp and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_menu_selection_rsp(
  const gstk_cmd_type * cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_cmd_from_card_type                     response;

   /* the envelope response is enum, so there is no need to initialize
   response */

   if(cmd == NULL)
   {
     UIM_MSG_ERR_0("cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }
   UIM_MSG_HIGH_0(" IN GSTK_MENU_SELECTION_RSP ");

   /* build the command */
   response.hdr_cmd.command_id = GSTK_MENU_IND_RSP;
   response.hdr_cmd.cmd_detail_reference = 0; /* doens't matter */
   response.cmd.envelope_general_rsp = cmd->envelope_response.rsp_status;
   response.hdr_cmd.sim_slot_id = cmd->envelope_response.message_header.sim_slot_id;
   if(cmd->envelope_response.message_header.user_data < GSTK_MAX_ENVELOPE_COMMAND) {
     response.hdr_cmd.user_data =
       gstk_shared_data.envelope_cmd_ref_table[cmd->envelope_response.message_header.user_data].user_data;
   }

   gstk_status = gstk_util_send_response_to_clients(&response,
     cmd->envelope_response.message_header.user_data);

   /* send end proactive session if session is started due to MENU SELECTION */
   if(cmd->envelope_response.uim_original_sw1 != SW1_END_FETCH)
   {      
     gstk_util_find_and_build_proactive_end_session(GSTK_TAG_MAX);
   }
   return gstk_status;

} /* gstk_menu_selection_rsp */

/*===========================================================================
FUNCTION gstk_non_idle_st_menu_selection_err_rsp

DESCRIPTION

  This function builds the menu selection error rsp in non idle state
  and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
  GSTK_BAD_PARAM,
  GSTK_SUCCESS

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_non_idle_st_menu_selection_err_rsp(
  const gstk_envelope_menu_selection_ind_type * menu_sel_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_cmd_from_card_type                     response;

   /* the envelope response is enum, so there is no need to initialize
   response */

   UIM_MSG_HIGH_0(" IN GSTK_NON_IDLE_ST_MENU_SELECITON_ERR_RSP ");

   if(menu_sel_cmd == NULL)
   {
     UIM_MSG_ERR_0("menu_sel_cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }

   /* build the command */
   response.hdr_cmd.command_id           = GSTK_MENU_IND_RSP;
   response.hdr_cmd.cmd_detail_reference = 0; /* doesn't matter */
   response.cmd.envelope_general_rsp     = GSTK_ENVELOPE_CMD_FAIL;
   response.hdr_cmd.sim_slot_id          = menu_sel_cmd->message_header.sim_slot_id;

   if(menu_sel_cmd->message_header.user_data < GSTK_MAX_ENVELOPE_COMMAND) {
     response.hdr_cmd.user_data =
       gstk_shared_data.envelope_cmd_ref_table[menu_sel_cmd->message_header.user_data].user_data;
   }

   gstk_status = gstk_util_send_response_to_clients(&response,
     menu_sel_cmd->message_header.user_data);

   return gstk_status;

} /* gstk_non_idle_st_menu_selection_err_rsp */

