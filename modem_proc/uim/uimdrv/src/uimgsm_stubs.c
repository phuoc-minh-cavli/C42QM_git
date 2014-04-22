/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 U I M   G S M   S T A T E   M A C H I N E

GENERAL DESCRIPTION
  This module contains the state machine for the gsm protocol of the UIM
  interface.

EXTERNALIZED FUNCTIONS

  uim_process_gsm_command
    Processes a gsm UIM server command

  uim_process_gsm_response
    Processes the responses for a gsm UIM server command

  uim_send_gsm_err_rpt
    Processes an error condition for a gsm UIM server command

  uim_gsm_command
    This function contains the state machine that processes APDUs
    for gsm commands.

  uim_gsm_command_response
    This function processes the UIM responses from APDUs.

Copyright (c) 2016 by QUALCOMM Technologies, Inc.
All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/uimdrv/src/uimgsm_stubs.c#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/08/16   na      Stubing out the CDMA\GSM\ICC specific functionality 
05/19/16   sam     Added api to check GSM support in UIM
02/01/16   ssr     Adding uimgsm stubs for IOE variant
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "comdef.h"
#include "uimdrv_msg.h"
#include "err.h"
#include "uimdrv.h"
#include "uimi.h"
#include "uimgsm.h"
#include <memory.h>

#include "uimglobals.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================
              F U N C T I O N   P R O T O T Y P E S
===========================================================================*/


/*===========================================================================

FUNCTION UIM_SEND_GSM_ERR_RPT

DESCRIPTION
  This function sends an error report for a gsm command. We pass cmd_ptr
  explicitly here since we may be sending err report for am unprocessed
  command that was present in command queue while processing hotswap
  card removal or card error.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
void uim_send_gsm_err_rpt
(
  uim_cmd_type             *cmd_ptr,
  uim_instance_global_type *uim_ptr
)
{
  if (cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("uim_send_gsm_err_rpt is NULL");
    return;
  }
  /* Copy user data from received command to report buffer */
  uim_ptr->command.rpt_buf.user_data = cmd_ptr->hdr.user_data;

  /* We will reset the report options flag in the command to NONE for
     backward compatibility purposes */

  cmd_ptr->hdr.options = UIM_OPTION_NONE;

  /* Report back the status as UIM_FAIL indicating failure in the R-UIM */
  uim_ptr->command.rpt_buf.rpt_status = UIM_FAIL;

  switch(cmd_ptr->hdr.command)
  {
    case UIM_RUN_GSM_ALGO_F:

      if (cmd_ptr->hdr.rpt_function != NULL)
      {
        uim_ptr->command.rpt_buf.rpt_type = UIM_RUN_GSM_ALGO_R;

        uim_ptr->command.rpt_buf.rpt.run_gsm.data_length = 0;

        /* Fill in necessary fields of command block */
        uim_ptr->command.rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Report to requesting task */
        (*(cmd_ptr->hdr.rpt_function))( &uim_ptr->command.rpt_buf );
        UIMDRV_MSG_MED_0(uim_ptr->id, "Reporting error for RUN GSM ALGO " );
      }

      break;

    default:
      UIMDRV_MSG_ERR_1(uim_ptr->id,
                    "Unknown command 0x%x in uim_send_gsm_err_rpt",
                       cmd_ptr->hdr.command );
      break;
  }

  /* We only allocate cmd_ptr memory for the external commands that have
     valid callback. Internal UIM commands that use static buffers
     have callback set to NULL. */

  if (cmd_ptr->hdr.rpt_function != NULL)
  {
    if (cmd_ptr == uim_ptr->command.cmd_ptr)
    {
      /* If cmd_ptr is same as current command in progress set global
         cmd_ptr to NULL as well */
      uim_ptr->command.cmd_ptr = NULL;
    }
    UIM_FREE(cmd_ptr);
  }
} /* uim_send_gsm_err_rpt */


/*===========================================================================

FUNCTION UIM_PROCESS_GSM_COMMAND

DESCRIPTION
  This procedure processes a command sent to the UIM task. It sets the UIM
  state variable and calls the UIM state machine to process the command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  It sets the UIM state variable amongst others.

===========================================================================*/
boolean uim_process_gsm_command
(
  uim_instance_global_type *uim_ptr
)
{
  /* This indicates that the command is processed by the state machine */
  if (uim_ptr->command.cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("uim_process_gsm_command cmd_ptr is NULL");
    return FALSE;
  }

  /* Copy user data from received command to report buffer */
  uim_ptr->command.rpt_buf.user_data = uim_ptr->command.cmd_ptr->hdr.user_data;
  /* Send an error report to the client and release the buffer */
  uim_ptr->command.rpt_buf.sw1 = (uim_sw1_type) 0;
  uim_ptr->command.rpt_buf.sw2 = (uim_sw2_type) 0;
  uim_ptr->command.rpt_buf.cmd_transacted  = FALSE;
  uim_send_gsm_err_rpt(uim_ptr->command.cmd_ptr, uim_ptr);

  return(FALSE);
} /* uim_process_gsm_command() */


/*===========================================================================

FUNCTION UIM_GSM_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response to a gsm comamnd that has been
  recd from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  Status which is of apdu_status_type

SIDE EFFECTS
  Changes the UIM state variable uim_ptr->command.gsm_state.

===========================================================================*/
uim_cmd_status_type uim_gsm_command_response
(
  uim_rsp_buf_type const   *rsp_ptr,
  uim_instance_global_type *uim_ptr
)
{

  if (rsp_ptr == NULL || uim_ptr == NULL)
  {
    UIM_MSG_ERR_0("uim_cdma_command_response either uim_ptr or rsp_ptr is NULL");
    return UIM_CMD_ERR;	
  }
  uim_ptr->command.cmd_ptr->hdr.cmd_count = uim_ptr->setting.max_num_cmd_attempts;
  return UIM_CMD_ERR;
} /* uim_gsm_command_response */


/*===========================================================================

FUNCTION UIM_CURRENT_DIR_GSM_ALGO

DESCRIPTION
  This procedure determines if an intermediate select is needed for GSM
  authentication algorithm

===========================================================================*/
void uim_current_dir_gsm_algo
(
  uim_instance_global_type     *uim_ptr,
  uint8                         curr_ch,
  uim_intermediate_select_type *intermediate_sel_ptr
)
{
  intermediate_sel_ptr->is_needed    = FALSE;
} /* uim_current_dir_gsm_algo */


/*===========================================================================

FUNCTION UIM_IS_GSM_SUPPORTED

DESCRIPTION
  This procedure determines if GSM is supported in the build

===========================================================================*/
boolean uim_is_gsm_supported(void)
{
  /*GSM functionality is not present. so return FALSE*/
  return FALSE;
}/*uim_is_gsm_supported*/


/*===========================================================================

FUNCTION UIM_UPDATE_CURRENT_DIR_ICC

DESCRIPTION
  Update the current directory after an ICC select command

===========================================================================*/
void uim_update_current_dir_icc
(
  uim_instance_global_type *uim_ptr,
  boolean                   is_ef,
  uim_path_type            *path_ptr,
  uint8                     path_position
)
{
  (void) uim_ptr;
  (void) is_ef;
  (void) path_ptr;
  (void) path_position;

} /* uim_update_current_dir_icc */

