/*===========================================================================


            G S T K   E V T  D L  T E R M I N A L  C A P

GENERAL DESCRIPTION
  This source file contains functions to handle Event Downloads based on the Terminal Capabilities

INITIALIZATION AND SEQUENCING REQUIREMENTS

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //components/rel/uim.mpss/3.3.1/gstk/src/gstk_evt_dl_terminal_cap.c#1 $

when         who     what, where, why
--------   ---     --------------------------------------------------------------------
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
05/12/16   gs       Move Voice Capabilities to different files
02/05/16   gs       Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk_envelope_cmd.h"
#include "gstk_envelope_rsp.h"
#include "uim.h"
#include "gstkutil.h"
#include "uim_msg.h"

/*===========================================================================
FUNCTION gstk_idle_scrn_evt_ind

DESCRIPTION

  This function builds the idle screen command and sends it to UIM

PARAMETERS
  idle_scrn_cmd: [Input] Pointer to the envelope command to be processed

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
gstk_status_enum_type gstk_idle_scrn_evt_ind(
  const gstk_envelope_idle_scrn_evt_ind_type * idle_scrn_cmd)
{
   gstk_status_enum_type                         gstk_status = GSTK_SUCCESS;
   gstk_envelope_idle_scrn_evt_dl_command_type   STK_envelope_cmd;
   uim_cmd_type                                  *uim_cmd_ptr;
   int                                           offset = 0;

   if(idle_scrn_cmd == NULL)
   {
     UIM_MSG_ERR_0("idle_scrn_cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_idle_scrn_evt_dl_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_IDLE_SCRN_AVAILABLE,
       &STK_envelope_cmd.evt_list );

   if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
   }

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
       GSTK_DISPLAY_DEVICE,
       GSTK_UICC_SIM_DEVICE,
       &STK_envelope_cmd.device_id );


   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }


   /* total data length:  evt list data + 2 +
                          device id data + 2 (2 is for the tag and length field)

   */
   STK_envelope_cmd.length =
                             STK_envelope_cmd.evt_list.length +
                             STK_envelope_cmd.device_id.device_tag_length +
                             4;


   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set to 0 so that uim will get the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0x00;

       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = idle_scrn_cmd->message_header.user_data;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.length;

       /* copy evt list */
       if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                  &(STK_envelope_cmd.evt_list),
                                  offset,
                                  (size_t)(STK_envelope_cmd.evt_list.length + GSTK_TAG_LENGTH_LEN),
                                  sizeof(uim_cmd_ptr->envelope.data),
                                  sizeof(STK_envelope_cmd.evt_list)) <
          (size_t)(STK_envelope_cmd.evt_list.length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }
       /* copy device */
       else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                       &(STK_envelope_cmd.device_id),
                                       offset +=
                                       (int)(STK_envelope_cmd.evt_list.length + GSTK_TAG_LENGTH_LEN),
                                       (size_t)(STK_envelope_cmd.device_id.device_tag_length +
                                                GSTK_TAG_LENGTH_LEN),
                                       sizeof(uim_cmd_ptr->envelope.data),
                                       sizeof(STK_envelope_cmd.device_id)) <
               (size_t)(STK_envelope_cmd.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }

       if (gstk_status == GSTK_SUCCESS)
       {
         gstk_util_dump_byte_array("Idle Screen Env",
                                   &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                                   uim_cmd_ptr->envelope.num_bytes);

         /* nothing to free envelope idle screen evt ind and tlv */
         gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
       }
       else
       {
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
       }
     }
   } /* uim_cmd_ptr != NULL */
   else {
     gstk_status = GSTK_MEMORY_ERROR;
   }

   return gstk_status;

} /* gstk_idle_scrn_evt_ind */

/*===========================================================================
FUNCTION gstk_user_act_evt_ind

DESCRIPTION

  This function builds the user activity command and sends it to UIM

PARAMETERS
  user_act_cmd: [Input] Pointer to the envelope command to be processed

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
gstk_status_enum_type gstk_user_act_evt_ind(
  const gstk_envelope_user_act_evt_ind_type * user_act_cmd)
{
   gstk_status_enum_type                         gstk_status = GSTK_SUCCESS;
   gstk_envelope_user_act_evt_dl_command_type    STK_envelope_cmd;
   uim_cmd_type                                  *uim_cmd_ptr;
   int                                           offset = 0;

   if(user_act_cmd == NULL)
   {
     UIM_MSG_ERR_0("user_act_cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_user_act_evt_dl_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_USER_ACTIVITY,
       &STK_envelope_cmd.evt_list );

   if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
   }

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
       GSTK_ME_DEVICE,
       GSTK_UICC_SIM_DEVICE,
       &STK_envelope_cmd.device_id );


   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* total data length:  evt list data + 2 +
                          device id data + 2 (2 is for the tag and length field)

   */
   STK_envelope_cmd.length =
                             STK_envelope_cmd.evt_list.length +
                             STK_envelope_cmd.device_id.device_tag_length +
                             4;


   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set to 0 so that uim will get the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0x00;

       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = user_act_cmd->message_header.user_data;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.length;

       /* copy evt list */
       if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                  &(STK_envelope_cmd.evt_list),
                                  offset,
                                  (size_t)(STK_envelope_cmd.evt_list.length + GSTK_TAG_LENGTH_LEN),
                                  sizeof(uim_cmd_ptr->envelope.data),
                                  sizeof(STK_envelope_cmd.evt_list)) <
          (size_t)(STK_envelope_cmd.evt_list.length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }
       /* copy device */
       else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                       &(STK_envelope_cmd.device_id),
                                       offset +=
                                       (int)(STK_envelope_cmd.evt_list.length + GSTK_TAG_LENGTH_LEN),
                                       (size_t)(STK_envelope_cmd.device_id.device_tag_length +
                                                GSTK_TAG_LENGTH_LEN),
                                       sizeof(uim_cmd_ptr->envelope.data),
                                       sizeof(STK_envelope_cmd.device_id)) <
               (size_t)(STK_envelope_cmd.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }

       if (gstk_status == GSTK_SUCCESS)
       {
         gstk_util_dump_byte_array("User Activity Env",
                                   &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                                   uim_cmd_ptr->envelope.num_bytes);

         /* nothing to free envelope user ativity evt ind and tlv */
         gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
       }
       else
       {
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
       }
     }
   } /* uim_cmd_ptr != NULL */
   else {
     gstk_status = GSTK_MEMORY_ERROR;
   }

   return gstk_status;

} /* gstk_user_act_evt_ind */


/*===========================================================================
FUNCTION gstk_browser_term_evt_ind

DESCRIPTION

  This function builds the browser termination event download envelope command
  and sends it to UIM

PARAMETERS
  browser_term_evt_cmd: [Input] Pointer to the envelope command to be processed

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
gstk_status_enum_type gstk_browser_term_evt_ind(
  const gstk_envelope_browser_term_evt_ind_type    * browser_term_evt_cmd)
{
   gstk_status_enum_type                                   gstk_status = GSTK_SUCCESS;
   gstk_envelope_browser_termination_evt_dl_command_type   STK_envelope_cmd;
   uim_cmd_type                                            *uim_cmd_ptr;
   int                                                     offset = 0;

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_browser_termination_evt_dl_command_type));
   if (browser_term_evt_cmd == NULL) {
      return GSTK_ERROR;
   }

   /* Pack various TLVs */
   STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_BROWSER_TERMINATION,
       &STK_envelope_cmd.event_download_hdr.evt_list );

   if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
   }

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
         GSTK_ME_DEVICE,
         GSTK_UICC_SIM_DEVICE,
         &STK_envelope_cmd.event_download_hdr.device_id );

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }


   /* browser termination cause tag */
   gstk_status = gstk_packer_browser_term_cause_tlv(
       &browser_term_evt_cmd->browser_termination_cause,
       &STK_envelope_cmd.cause);

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* total data length:  evt id data + 2
                        + device id data + 2 (2 is for the tag and length field)
                        + termination cause data + 2
   */
   STK_envelope_cmd.event_download_hdr.length =
                             STK_envelope_cmd.event_download_hdr.evt_list.length +
                             STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                             STK_envelope_cmd.cause.length +
                             6;

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set to 0 so that uim will get the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0x00;

       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = browser_term_evt_cmd->message_header.user_data;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length
                                        + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.length;

       /* copy evt list */
       if (gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                   &STK_envelope_cmd.event_download_hdr.evt_list,
                                   offset,
                                   (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length
                                            + GSTK_TAG_LENGTH_LEN),
                                   sizeof(uim_cmd_ptr->envelope.data)-offset,
                                   sizeof(STK_envelope_cmd.event_download_hdr.evt_list)) <
           (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }
       /* copy device */
       else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                       &(STK_envelope_cmd.event_download_hdr.device_id),
                                       offset +=
                                       (int)(STK_envelope_cmd.event_download_hdr.evt_list.length +
                                             GSTK_TAG_LENGTH_LEN),
                                       (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                                                GSTK_TAG_LENGTH_LEN),
                                       sizeof(uim_cmd_ptr->envelope.data),
                                       sizeof(STK_envelope_cmd.event_download_hdr.device_id)) <
               (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                        GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }
       
       offset += (int)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN);
       /* copy cause */
       if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                       &(STK_envelope_cmd.cause),
                                       offset,
                                       (size_t)(STK_envelope_cmd.cause.length + GSTK_TAG_LENGTH_LEN),
                                       sizeof(uim_cmd_ptr->envelope.data)-offset,
                                       sizeof(STK_envelope_cmd.cause)) <
               (size_t)(STK_envelope_cmd.cause.length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }

       if (gstk_status == GSTK_SUCCESS)
       {
         gstk_util_dump_byte_array("Browser Term Env",
                                   &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                                   uim_cmd_ptr->envelope.num_bytes);

         /* no free for envelope location info ind and tlv */
         gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
       }
       else
       {
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
       }
     }
   } /* uim_cmd_ptr != NULL */
   else {
     gstk_status = GSTK_MEMORY_ERROR;
   }

   return gstk_status;

} /* gstk_browser_term_evt_ind*/

/*===========================================================================
FUNCTION gstk_lang_selection_evt_ind

DESCRIPTION

  This function builds the language selection event download envelope command
  and sends it to UIM

PARAMETERS
  lang_select_evt_cmd: [Input] Pointer to the envelope command to be processed

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
gstk_status_enum_type gstk_lang_selection_evt_ind(
  const gstk_envelope_lang_sel_evt_ind_type    * lang_select_evt_cmd)
{
   gstk_status_enum_type                                   gstk_status = GSTK_SUCCESS;
   gstk_envelope_language_evt_dl_command_type              STK_envelope_cmd;
   uim_cmd_type                                            *uim_cmd_ptr;
   int                                                     offset = 0;

   if(lang_select_evt_cmd == NULL)
   {
     UIM_MSG_ERR_0("lang_select_evt_cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_language_evt_dl_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

   /* event list */
   gstk_status = gstk_packer_evt_list_tlv(
       GSTK_LANGUAGE_SELECTION,
       &STK_envelope_cmd.event_download_hdr.evt_list );

   if (gstk_status != GSTK_SUCCESS) {
      return gstk_status;
   }

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
         GSTK_ME_DEVICE,
         GSTK_UICC_SIM_DEVICE,
         &STK_envelope_cmd.event_download_hdr.device_id );

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }


   /* lang code tag */
   gstk_status = gstk_packer_lang_sel_tlv(
       lang_select_evt_cmd->lang_code.lang_code,
       &STK_envelope_cmd.language);

   if (gstk_status != GSTK_SUCCESS) {
       return gstk_status;
   }

   /* total data length:  evt id data + 2
                        + device id data + 2 (2 is for the tag and length field)
                        + lang sel data + 2
   */
   STK_envelope_cmd.event_download_hdr.length =
                             STK_envelope_cmd.event_download_hdr.evt_list.length +
                             STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                             STK_envelope_cmd.language.length +
                             6;

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
       /* set to 0 so that uim will get the data from index 0 */
       uim_cmd_ptr->envelope.offset = 0x00;

       /* set user_data */
       uim_cmd_ptr->hdr.user_data               = lang_select_evt_cmd->message_header.user_data;

       /* populate the envelope command info */
       uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length
                                        + GSTK_TAG_LENGTH_LEN;

       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.tag;
       uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.length;

       /* copy evt list */
       (void)memscpy(&(uim_cmd_ptr->envelope.data[offset]),
              sizeof(uim_cmd_ptr->envelope.data)-offset,
              &STK_envelope_cmd.event_download_hdr.evt_list,
              (int)(STK_envelope_cmd.event_download_hdr.evt_list.length
                    + GSTK_TAG_LENGTH_LEN));
       offset = (int)(offset + STK_envelope_cmd.event_download_hdr.evt_list.length
                   + GSTK_TAG_LENGTH_LEN);
                   
       /* copy device */
       if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                  &(STK_envelope_cmd.event_download_hdr.device_id),
                                  offset,
                                  (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                                           GSTK_TAG_LENGTH_LEN),
                                  sizeof(uim_cmd_ptr->envelope.data),
                                  sizeof(STK_envelope_cmd.event_download_hdr.device_id)) <
          (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }
       
       offset += (int)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN);
       /* copy language */
       if (gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                        &STK_envelope_cmd.language,
                                        offset,
                                        (size_t)(STK_envelope_cmd.language.length + GSTK_TAG_LENGTH_LEN),
                                        sizeof(uim_cmd_ptr->envelope.data)-offset,
                                        sizeof(STK_envelope_cmd.language)) <
                (size_t)(STK_envelope_cmd.language.length + GSTK_TAG_LENGTH_LEN))
       {
         gstk_status = GSTK_MEMORY_ERROR;
       }

       if (gstk_status == GSTK_SUCCESS)
       {
         gstk_util_dump_byte_array("Lang select Env",
                                   &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                                   uim_cmd_ptr->envelope.num_bytes);

         /* no free for envelope location info ind and tlv */
         gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
       }
       else
       {
         gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
         uim_cmd_ptr = NULL;
       }
     }
   } /* uim_cmd_ptr != NULL */
   else {
     gstk_status = GSTK_MEMORY_ERROR;
   }

   return gstk_status;

} /* gstk_lang_selection_evt_ind*/
