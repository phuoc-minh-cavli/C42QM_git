/*===========================================================================


            G S T K   E N V E L O P E   C B S

GENERAL DESCRIPTION
  This source file contains functions to handle Cell Broadcast Data Download


FUNCTIONS
  gstk_cell_broadcast_dl_ind
    This function builds the cell broadcast download command and sends it
    to UIM

  gstk_cell_broadcast_dl_ind_cleanup
    This function does releases the memory allocated for this command

  gstk_cell_broadcast_dl_rsp
    This function builds the cell broadcast download rsp and sends it to client

  gstk_non_idle_st_cell_change_brcst_err_rsp
    This function builds the cell brcst rsp error message in non idle state
    and sends it to client.  GSTK will set the status to SUCCESS and no MOD
    in the data so that the mobile can proceed with the call as the Toolkit
    is no longer valid  

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


$Header: //components/rel/uim.mpss/3.3.1/gstk/src/gstk_envelope_cbs.c#1 $

when         who     what, where, why
--------   ---     --------------------------------------------------------------------
09/15/16   gs      Changes in TP based on RAT and App Type
06/03/16   shr     GSTK/ESTK globals memory optimization
05/31/16   shr     F3 reduction: Dumping Arrays
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
01/28/16   gs      Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include <stringl/stringl.h>
#include "gstk_envelope_cmd.h"
#include "gstk_envelope_rsp.h"
#include "uim.h"
#include "string.h"
#include "err.h"
#include "gstkutil.h"
#include "uim_msg.h"

/*===========================================================================
FUNCTION gstk_cell_broadcast_dl_ind_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  cb_dl_cmd: [Input] Pointer to message that is required to
                            be processed and sent to UIM
  STK_envelope_cmd: [Input] Pointer to the STK intermediate message

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_cell_broadcast_dl_ind_cleanup(
    gstk_envelope_cell_broadcast_download_ind_type*     cb_dl_cmd,
    gstk_envelope_cell_broadcast_download_command_type* STK_envelope_cmd)
{

  if(STK_envelope_cmd == NULL)
  {
    UIM_MSG_ERR_0("STK_envelope cmd ERR : NULL");
    return;
  }
  /* free envelope page dataind and tlv */
  /* free page tlv */
  gstk_util_release_upper_layer_cmd_memory((gstk_cmd_type*)((void*)cb_dl_cmd));
  if(STK_envelope_cmd->cb_page.cb_page != NULL) {
    gstk_free(STK_envelope_cmd->cb_page.cb_page);
    STK_envelope_cmd->cb_page.cb_page = NULL;
  }
} /* gstk_cell_broadcast_dl_ind_cleanup */

/*===========================================================================
FUNCTION gstk_cell_broadcast_dl_ind

DESCRIPTION

  This function builds the cell broadcast download command and sends it to UIM

PARAMETERS
  cb_dl_cmd: [Input] Pointer to the envelope command to be processed

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
gstk_status_enum_type gstk_cell_broadcast_dl_ind(
          gstk_envelope_cell_broadcast_download_ind_type * cb_dl_cmd)
{
   gstk_status_enum_type                              gstk_status = GSTK_SUCCESS;
   gstk_envelope_cell_broadcast_download_command_type STK_envelope_cmd;
   uim_cmd_type                                      *uim_cmd_ptr;
   int                                                offset = 0;

   UIM_MSG_HIGH_0("In gstk_cell_broadcast_dl_ind()");
   if(cb_dl_cmd == NULL)
   {
     UIM_MSG_ERR_0("cb_dl_cmd ERR : NULL");
     return GSTK_BAD_PARAM;
   }

   /* initialize STK_envelope_cmd */
   memset(&STK_envelope_cmd, 0, sizeof(gstk_envelope_cell_broadcast_download_command_type));

   /* Pack various TLVs */
   STK_envelope_cmd.tag = GSTK_CELL_BROADCAST_DOWNLOAD_TAG;

   /* device ID */
   gstk_status = gstk_packer_dev_id_tlv(
       GSTK_NETWORK_DEVICE,
       GSTK_UICC_SIM_DEVICE,
       &STK_envelope_cmd.device_id );

   if (gstk_status != GSTK_SUCCESS) {
       gstk_cell_broadcast_dl_ind_cleanup(cb_dl_cmd, &STK_envelope_cmd);
       return gstk_status;
   }

   /* cb page tag */
   gstk_status = gstk_packer_cell_broadcast_page_tlv(
         &cb_dl_cmd->cb_page,
         &STK_envelope_cmd.cb_page);

   if (gstk_status != GSTK_SUCCESS) {
       gstk_cell_broadcast_dl_ind_cleanup(cb_dl_cmd, &STK_envelope_cmd);
       return gstk_status;
   }

   /* Send command to STK application on SIM */
   /* Send Message to UIM */
   /* get a buffer for the request */
   uim_cmd_ptr = gstk_get_uim_buf();
   if (uim_cmd_ptr != NULL ) {
     gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
     if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
         /* set user_data */
         uim_cmd_ptr->hdr.user_data               = cb_dl_cmd->message_header.user_data;

         /* initialized to 3 for value portion of the TLVs
            i.e., the final offset will be either 0 or 1 depends
            on whether the total length is > 0x7F or not */
         offset = 3;

         /* copy device */
         /* STK_envelope_cmd.device_id.device_tag_length fixed in packer function */
         if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                    &(STK_envelope_cmd.device_id),
                                    offset,
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
         offset = (int)(offset + STK_envelope_cmd.device_id.device_tag_length +
                        GSTK_TAG_LENGTH_LEN);

         /* total length: + device tlv */
         STK_envelope_cmd.length = STK_envelope_cmd.device_id.device_tag_length +
                                   GSTK_TAG_LENGTH_LEN;

         /* copy cellbroadcast page */
         /* copy tag, length */
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.cb_page.tag;
         if (STK_envelope_cmd.cb_page.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
           uim_cmd_ptr->envelope.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
           STK_envelope_cmd.length++;
         }
         uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.cb_page.length;

         /* STK_envelope_cmd.cb_page.length fixed in packer function */
         (void)memscpy(&(uim_cmd_ptr->envelope.data[offset]),
                sizeof(uim_cmd_ptr->envelope.data) - offset,
                STK_envelope_cmd.cb_page.cb_page,
                (int)(STK_envelope_cmd.cb_page.length));
         offset = (int)(offset + STK_envelope_cmd.cb_page.length);
         /* total length: + cb page tlv */
         STK_envelope_cmd.length += STK_envelope_cmd.cb_page.length + GSTK_TAG_LENGTH_LEN;

         /* populate the envelope command info */
         if(STK_envelope_cmd.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
           /* account for 0x80 */
           uim_cmd_ptr->envelope.offset = 0;
           uim_cmd_ptr->envelope.data[0] = STK_envelope_cmd.tag;
           uim_cmd_ptr->envelope.data[1] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
           uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.length;
           STK_envelope_cmd.length++;
           uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;
         }
         else {
           uim_cmd_ptr->envelope.offset = 1;
           uim_cmd_ptr->envelope.data[1] = STK_envelope_cmd.tag;
           uim_cmd_ptr->envelope.data[2] = STK_envelope_cmd.length;
           uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.length + GSTK_TAG_LENGTH_LEN;
         }

         gstk_util_dump_byte_array("Env: Cell Broadcast DL",
                                   &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                                   uim_cmd_ptr->envelope.num_bytes);   
         gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
       }
     }
   } /* uim_cmd_ptr != NULL */
   else {
     gstk_status = GSTK_MEMORY_ERROR;
   }

   gstk_cell_broadcast_dl_ind_cleanup(cb_dl_cmd, &STK_envelope_cmd);
   return gstk_status;
} /* gstk_cell_broadcast_dl_ind */

/*===========================================================================
FUNCTION gstk_cell_broadcast_dl_rsp

DESCRIPTION

  This function builds the cell broadcast rsp and sends it to client

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
gstk_status_enum_type gstk_cell_broadcast_dl_rsp(
  const gstk_cmd_type * cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_cmd_from_card_type                     response;

   /* the envelope response is enum, so there is no need to initialize
   response */

   UIM_MSG_HIGH_0(" IN GSTK_CELL_BROADCAST_DL_RSP ");

   if(cmd == NULL)
   {
     UIM_MSG_ERR_0("cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }

   /* build the command */
   response.hdr_cmd.command_id = GSTK_CELL_BROADCAST_DOWNLOAD_IND_RSP;
   response.hdr_cmd.cmd_detail_reference = 0; /* doens't matter */
   response.cmd.envelope_general_rsp = cmd->envelope_response.rsp_status;
   if(cmd->envelope_response.message_header.user_data < GSTK_MAX_ENVELOPE_COMMAND)
   {
     response.hdr_cmd.user_data =
       gstk_shared_data.envelope_cmd_ref_table[cmd->envelope_response.message_header.user_data].user_data;
   }

   gstk_status = gstk_util_send_response_to_clients(&response,
     cmd->envelope_response.message_header.user_data);

   return gstk_status;

} /* gstk_cell_broadcast_dl_rsp */

/*===========================================================================
FUNCTION gstk_non_idle_st_cell_change_brcst_err_rsp

DESCRIPTION

  This function builds the cell brcst rsp error message in non idle state
  and sends it to client.  GSTK will set the status to SUCCESS and no MOD
  in the data so that the mobile can proceed with the call as the Toolkit
  is no longer valid

PARAMETERS
  cell_brcst_cmd_ptr: [Input] Pointer to the envelope rsp to be processed

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
gstk_status_enum_type gstk_non_idle_st_cell_change_brcst_err_rsp(
  const gstk_envelope_cell_broadcast_download_ind_type * cell_brcst_cmd_ptr)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_cmd_from_card_type                     response;

   UIM_MSG_HIGH_0(" IN GSTK_NON_IDLE_ST_CELL_CHANGE_BRCST_ERR_RSP ");

   if(cell_brcst_cmd_ptr == NULL)
   {
     UIM_MSG_ERR_0("cell_brcst_cmd ERR:NULL");
     return GSTK_BAD_PARAM;
   }

   memset(&response, 0, sizeof(response));

   /* build the command */
   response.hdr_cmd.sim_slot_id          = cell_brcst_cmd_ptr->message_header.sim_slot_id;
   response.hdr_cmd.command_id           = GSTK_CELL_BROADCAST_DOWNLOAD_IND_RSP;
   response.hdr_cmd.cmd_detail_reference = 0; /* doens't matter */
   if(gstk_curr_inst_ptr->gstk_sub_state == GSTK_RECOVERY_SUB_S)
   {
     response.cmd.envelope_general_rsp     = GSTK_ENVELOPE_CMD_CARD_BUSY;
   }
   else
   {
     response.cmd.envelope_general_rsp     = GSTK_ENVELOPE_CMD_FAIL;
   }
   if(cell_brcst_cmd_ptr->message_header.user_data < GSTK_MAX_ENVELOPE_COMMAND)
   {
     response.hdr_cmd.user_data =
       gstk_shared_data.envelope_cmd_ref_table[cell_brcst_cmd_ptr->message_header.user_data].user_data;
   }

   gstk_status = gstk_util_send_response_to_clients(&response,
     cell_brcst_cmd_ptr->message_header.user_data);

   return gstk_status;
} /* gstk_non_idle_st_cell_change_brcst_err_rsp */

/*===========================================================================
FUNCTION gstk_check_cbs_capability

DESCRIPTION
  This function checks the CBS capability

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

===========================================================================*/
boolean gstk_check_cbs_capability(void)
{
  return TRUE;
} /* gstk_check_cbs_capability */

