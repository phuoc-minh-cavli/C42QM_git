/*===========================================================================


              G S T K    T E R M I N A L    R E S P O N S E



GENERAL DESCRIPTION
  This source file contains the major terminal response handling, based on
  various type of proactive command, the corresponding terminal response
  function handlers will be called.


FUNCTIONS
  gstk_populate_terminal_response_cnf
    This function build terminal response confirm type which will be put onto
    the GSTK command task later

  gstk_process_terminal_response
    This function switches on the command id and call the corresponding
    terminal response cnf function so that GSTK can build the corrresponding
    terminal response tlvs and send it to UIM Server

  gstk_uim_terminal_response_report
    This is the callback function for terminal response response from UIM.

  gstk_util_error_check_terminal_rsp
    This function will perform some error checking to ensure that
    the Application sending the Terminal Response has an
    opportunity to resend the Terminal Response incase it is
    invalid

  gstk_process_raw_terminal_response
    This function packs raw terminal response and sends response to the card
    through the UIM server.

  gstk_get_term_rsp_timer_exp_error_code
    This function returns appropriate term rsp result code when a proactive
    command timer expires.

  gstk_process_term_rsp_timer_expire
    This function process timer expiration while waiting for the terminal
    for a proactive command already sent to the client.

INITIALIZATION AND SEQUENCING REQUIREMENTS

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //components/rel/uim.mpss/3.3.1/gstk/src/gstk_terminal_rsp.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
05/13/16   gm      Fix compilation warning
05/10/16   gm      Fixing compiler warning
03/07/16   gs      Send the failure TR to the Card instead of requeue
11/19/15   vr      Update client userdata before sending the sw1_sw2
11/12/15   vr      F3 Logging inprovements
10/28/15   vr      Send SW1 report for the clients for RAW TR
08/05/15   gs      Usage of scws flag during timer expiry
07/02/15   vr      Fix potential buffer overflow while building Envelope/TR
06/26/15   gs      Update the SCWS flag based on transport Protocol 
04/27/15   vr      Non UI and non NW proactive command parallel processing
11/14/14   vr      Update cmd_ptr userdata with client id before sending TR
10/07/14   vr      Fix SW1 SW2 TR REPORT failure for setup event list
09/23/14   hn      Support for 3GPP2 Advanced and HDR Location Info 
09/09/14   vr      Reduce stack usage
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
07/23/14   vr      Add Rel99, Rel4 and Rel5 cat versions
07/11/14   shr     Add Support for NV Refresh
06/30/14   gm      Remove unnecessary clear of timer
05/28/14   gm      Support of CSG Rel-11 feature
05/23/14   gm      Verifying TR response report
05/06/14   gs      GSTK Featurization enhancements and clean up
04/03/14   dy      Replace gstk_malloc() with macro
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
03/27/14   gm      Update ref_table_idx with ref_id
02/20/14   dy      Verify TR response for raw TRs
01/30/14   vr      SIM initiated call control handling
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
01/09/14   kb      Updated GSTK_NULL_CHECK_PTR macro
12/05/13   hn      Support DNS query and Channel reuse
11/01/13   shr     Fixed porting error w.r.t. PLI date and time TR expiry
10/29/13   jd      PLI date and time handling w.r.t. TR timer expiry fixed
10/04/13   gs      F3 Message reduction
09/23/13   gm      used macro for masking command ref table index
09/23/13   vr      Fix crash because of alpha zero length in voice to ussd cc
09/13/13   gm      Support for Recovery and hot-swap
09/12/13   gm      Setting GSTK_ESTK_CMD_SIG when UIM sent TR report
06/26/13   sw      Send Error Terminal Response incase sending of TR fails.
06/12/13   gm      Setting GSTK_ESTK_CMD_SIG everytime when command is posted
                   to GSTK task
04/09/13   vr      Use safer versions of memcpy() and memmove()
05/13/13   gm      Changes to make gstk shared variable "gstk_requested_mask"
                   thread safe
03/06/13   vr      Fix for No response of get inkey command when TR timer expired
12/18/12   vr      Caching and updating the gstk_requested_mask for terminal
                   response report
09/24/12   hk      Convert all ERRs to MSG_ERROR to save memory
07/26/12   sg      Set ESTK SIG to reduce delay in processing commands from queue
06/08/12   dd      Fix SCWS Open Ch TR if call is rejected
05/22/12   nb      Convert setting and resetting of gstk_proactive_cmd_in_progress
                   flag to MACRO
04/19/12   nb      Fix issue of proactive cmd not being fetched from card after
                   skipping BIP envelope cmd
03/28/12   av      Replaced feature flags with NVITEM
02/18/12   av      Cleaned up remaining ALWAYS ON features
01/03/12   nb      Changing FEATURE_GSTK_REPORT_SW1_SW2 to a bit mask
12/20/11   nb      GSDI Cleanup
12/02/11   dd      Fix security issues
11/11/11   dd      Fix KW issues
10/31/11   sg      Class L and M support
10/18/11   av      Removed features that are always OFF
09/14/11   nb      Reverting changes to block polling
08/24/11   xz      Fix compiler warnings and LINT/KW errors
07/08/11   xz      Check UIM slot in gstk_uim_terminal_response_report
05/14/11   bd      Moved access to pending_responses to GSTK task context
04/28/11   nk      Remove include tmc.h to resolve compiler warning
04/24/11   dd      SCWS support
02/11/11   ea      Added check other_info->present before sending duration.
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
12/02/10   ssr     Fixed command detail parsing
11/28/10   ssr     Fix to allow UIM driver to fetch only after it reports back
                   TR response
09/08/10   nb      Inform UIM to start polling after TR for PLI(IMEI)
08/26/10   xz      Add support of EPS Call Control
05/28/10   yt      Lint fixes
05/11/10   xz      Lint fixes
04/14/10   bd      Fixed issue of not sending call connected event download envelope
02/15/10   nb      Moved Dual Slot Changes
02/04/10   sg      Add open channel timeout TR extra parameters
09/26/09   kp      ZI memory reduction changes
03/17/09   xz      Fix issue of not sending NO RESPONSE FROM USER TR
03/16/09   yb      Fixed error in parameter checking and struct access
03/13/09   gg      Fixed error in parameter checking and struct access
03/02/09   xz      Provide SW1 and SW2 support for raw APDU responses
01/16/09   xz      Fix issue of not waiting for setup event list terminal responses
                   from multiple clients
11/20/08   sk      Allow extra param to be a part of the get inkey terminal
                   response even if the result is not a success
10/21/08   xz      Fix an issue of failing to send TR if term rsp timer expires
09/14/08   sk      Fixed lint errors
09/08/08   sk      reset timer for client response when a TR is being sent to
                   the card.
08/22/08   sk      Fixed paramter for gstk_process_term_rsp_timer_expire()
08/21/08   sk      Fixed warning and compilation errors.
08/14/08   sk      Fixed warnings
08/08/08   xz      Add support for raw terminal response
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
10/23/06   sk      Fixed lint errors.
10/19/06   sk      Fixed lint errors.
10/11/06   sk      Fixed integer conversions.
10/08/06   sk      Added support for unknown unsupported proactive commands.
06/09/06   sk      Lint fixes
06/06/06   sk      Added support for network search mod
05/23/06   tml     GPRS Call Control support and lint
04/04/06   sk      Added support for 1X related to location information
                   Added support for immediate digit response and variable
                   timeout duration object for get inkey cmd.
04/04/05   sk      Added support for pli - battery status
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
09/30/05   sk      Added deregistration for CM events after receiving response
                   for Access Technlogy PLI.
09/01/05   sk      Added additional result field to
                   gstk_util_error_check_terminal_rsp_other_
                   info() to support extra information for
                   GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR
07/29/05   sk      Fixed compilation warnings.
07/13/05   sk      Added support for Access Technology.
07/11/05   sk      Reverted changes in gstk_util_error_check_terminal_rsp_other_
                   info() to correct problems with different result values.
06/17/05   jar     Fixed Compiler Error for Missing "{"
06/16/05   sk      Added support for BEARER_INDEPENDANT PROTOCOL
                   ERROR in gstk_util_error_check_terminal_rsp_other_info()
05/11/05   sst     Lint fixes
01/10/04  sk/tml   Open channel and get channel status terminal response fixes
12/06/04   sk      Changed case 'GSTK_GET_CH_STATUS_CNF' under function
                   gstk_populate_terminal_response_cnf() to include
                   status of all the open and dropped channels.
11/23/04  sk/tml   Added SAT/USAT data support
09/20/04   tml     Added support for get inkey UCSII input support
08/10/04   tml     Added checking in terminal response to determine if
                   any CALL CONNECTED envelope is required to be sent to the
                   card
08/05/04   tml     Fixed send USSD cc population issue
07/22/04   tml     Changed to use pointer for additional info
07/14/04   tml     Allow help request to have other info for Get Inkey, Get
                   Input and Select Item
06/14/04   tml     Added support to differentiate between GSTK generated END
                   proactive session verse card generated END
05/26/04   tml     Added lang notification, timer management, send dtmf,
                   timer expiration, browser term evt dl and lang selection
                   evt dl supports
03/01/04   tml     Added provide language support
09/03/03   tml     Fixed Select Item terminal response extra param info
07/07/03   tml     Removed meaningless message
06/04/03   tml     Remove checking for 0 length get inkey response
05/18/03   tml     Linted
04/21/03   tml     Added address NPI support
03/19/03   tml     Added data_present boolean assignment for get inkey, get
                   input and select item
03/06/03   tml     Updated enum names
02/24/03   jar     Added Refresh Code.
02/13/03   tml     Added/Changed DM messages
02/07/03   tml     Initial Version


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "intconv.h"
#include "gstk.h"
#include "gstk_terminal_rsp.h"
#include "gstk_proactive_cmd.h"

#include "gstkutil.h"

#include "string.h"
#include <stringl/stringl.h>
#include "err.h"
#include "task.h"
#include "cm.h"
#include "uim_msg.h"

/*===========================================================================
FUNCTION gstk_populate_tr_additional_res

DESCRIPTION
  This function build terminal response confirm type which will be put onto
  the GSTK command task later

PARAMETERS
  adtnl_inf_ptr: [Input / Output] Pointer of additional info ptr for response
  additional_res_ptr: [Input] Pointer for additional response

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
gstk_status_enum_type gstk_populate_tr_additional_res(gstk_additional_info_ptr_type *adtnl_inf_ptr, 
	              const gstk_additional_info_ptr_type      *additional_res_ptr)
{
  adtnl_inf_ptr->length = 0;
  adtnl_inf_ptr->additional_info_ptr = NULL;
  
  if (additional_res_ptr->length != 0) 
  {
    adtnl_inf_ptr->additional_info_ptr = GSTK_CALLOC(additional_res_ptr->length);
    if (adtnl_inf_ptr->additional_info_ptr == NULL) 
    {
      return GSTK_MEMORY_ERROR;
    }
    (void)memscpy(adtnl_inf_ptr->additional_info_ptr,
           additional_res_ptr->length,
           additional_res_ptr->additional_info_ptr,
           additional_res_ptr->length);
    adtnl_inf_ptr->length = additional_res_ptr->length;
  }
  return GSTK_SUCCESS;
}

/*===========================================================================
FUNCTION gstk_populate_terminal_response_cnf

DESCRIPTION
  This function build terminal response confirm type which will be put onto
  the GSTK command task later

PARAMETERS
  command: [Input/Output] Pointer to the task_cmd_type to which the terminal
                          response is going to be populated
  terminal_response_enum: [Input] Response type for thie terminal response
  general_result: [Input] General response
  additional_result: [Input] Pointer of additional info ptr for response
  other_info: [Input] Pointer of extra info for some terminal response type
  cmd_num: [Input] command number of the proactive command that this terminal
                   response is responding
  cmd_details_ref_id: [Input] reference id in the command details table
  user_data: [Input] client user data

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
gstk_status_enum_type gstk_populate_terminal_response_cnf(
        gstk_task_cmd_type                       *command,
        gstk_cmd_enum_type                       terminal_response_enum,
        gstk_general_result_enum_type            general_result,
        const gstk_additional_info_ptr_type      *additional_result,
        const gstk_terminal_rsp_extra_param_type *other_info,
        uint8                                    cmd_num,
        uint32                                   cmd_details_ref_id,
        uint32                                   user_data
)
{
  gstk_status_enum_type     gstk_status        = GSTK_SUCCESS;
  gstk_local_info_rsp_type *local_info_rsp_ptr = NULL;
  uint32                    i                  = 0;
  gstk_nv_items_data_type  nv_data;

  if (command == NULL || other_info == NULL || additional_result == NULL)
  {
    UIM_MSG_ERR_3("Null Ptr, cmd = 0x%x, oth info = 0x%x, adtnl_res = 0x%x",
		command, other_info, additional_result);
    return GSTK_MEMORY_ERROR;
  }

  /* Read the CAT version */
  (void)gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                            gstk_curr_inst_ptr->slot_id,
                            &nv_data);

  UIM_MSG_HIGH_1("Other_info is 0x%x", other_info->present);

   /* Using the Refresh Cnf as only a place holder, since the refresh_term_rsp_cnf under an
     union of multiple structures. Though we are filling the refresh_term_rsp_cnf since this 
     is part of multiple strutures under an union it is expected that the data is rightly filled
     by making the the variables of struture are common.*/
  command->cmd.refresh_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
  command->cmd.refresh_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
  command->cmd.refresh_term_rsp_cnf.message_header.user_data = user_data;
  command->cmd.refresh_term_rsp_cnf.command_number = cmd_num;
  command->cmd.refresh_term_rsp_cnf.command_result = general_result;
  command->cmd.refresh_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;

  switch(terminal_response_enum) 
  {
    case GSTK_REFRESH_CNF:
        UIM_MSG_HIGH_2("REFRESH rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        return gstk_populate_tr_additional_res(
			  &(command->cmd.refresh_term_rsp_cnf.result_additional_info), 
			    additional_result);
        break;
    case GSTK_MORE_TIME_CNF:
        UIM_MSG_HIGH_2("MORE TIME rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        return gstk_populate_tr_additional_res(
			  &(command->cmd.more_time_term_rsp_cnf.result_additional_info), 
			    additional_result);
        break;
    case GSTK_POLLING_OFF_CNF:
    case GSTK_POLL_INTERVAL_CNF:
        UIM_MSG_HIGH_2("POLLING rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        if((gstk_status = gstk_populate_tr_additional_res(
			  &(command->cmd.poll_interval_term_rsp_cnf.result_additional_info), 
			    additional_result)) != GSTK_SUCCESS)
        {
          return gstk_status;
        }
 			    
        /* duration */
        if(other_info->present) {
          command->cmd.poll_interval_term_rsp_cnf.duration.present = TRUE;
          command->cmd.poll_interval_term_rsp_cnf.duration.length = other_info->extra_param.poll_duration.length;
          command->cmd.poll_interval_term_rsp_cnf.duration.units = other_info->extra_param.poll_duration.units;
        }
        else {
          command->cmd.poll_interval_term_rsp_cnf.duration.present = FALSE;
          command->cmd.poll_interval_term_rsp_cnf.duration.length = 0;
          command->cmd.poll_interval_term_rsp_cnf.duration.units = 0;
        }
        break;
    case GSTK_SETUP_EVENT_LIST_CNF:
        UIM_MSG_HIGH_2("SETUP EVT LIST rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        return gstk_populate_tr_additional_res(
			  &(command->cmd.setup_evt_list_term_rsp_cnf.result_additional_info), 
			    additional_result);
        break;
    case GSTK_SETUP_CALL_CNF:
        UIM_MSG_HIGH_2("SETUP CALL rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        if((gstk_status = gstk_populate_tr_additional_res(
			  &(command->cmd.setup_call_term_rsp_cnf.result_additional_info), 
			    additional_result)) != GSTK_SUCCESS)
        {
          return gstk_status;
        }
        return gstk_populate_setup_call_tr_cnf(command, additional_result, other_info);
        break;
    case GSTK_SEND_SS_CNF:
        UIM_MSG_HIGH_2("SEND SS rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        if((gstk_status = gstk_populate_tr_additional_res(
			  &(command->cmd.send_ss_term_rsp_cnf.result_additional_info), 
			    additional_result)) != GSTK_SUCCESS)
        {
          return gstk_status;
        }
        return gstk_populate_send_ss_tr_cnf(command, additional_result, other_info);
        break;
    case GSTK_SEND_USSD_CNF:
        UIM_MSG_HIGH_2("SEND USSD rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        if((gstk_status = gstk_populate_tr_additional_res(
			  &(command->cmd.send_ussd_term_rsp_cnf.result_additional_info), 
			    additional_result)) != GSTK_SUCCESS)
        {
          return gstk_status;
        }
        return gstk_populate_send_ussd_tr_cnf(command, additional_result, other_info);
        break;
    case GSTK_SEND_SMS_CNF:
        UIM_MSG_HIGH_2("SEND SMS rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        return gstk_populate_tr_additional_res(
			  &(command->cmd.send_sms_term_rsp_cnf.result_additional_info), 
			    additional_result);
        break;
    case GSTK_LAUNCH_BROWSER_CNF:
        UIM_MSG_HIGH_2("LAUNCH BROWSER rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        return gstk_populate_tr_additional_res(
			  &(command->cmd.launch_browser_term_rsp_cnf.result_additional_info), 
			    additional_result);
        break;
    case GSTK_PLAY_TONE_CNF:
        UIM_MSG_HIGH_2("PLAY TONE rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        return gstk_populate_tr_additional_res(
			  &(command->cmd.play_tone_term_rsp_cnf.result_additional_info), 
			    additional_result);
        break;
    case GSTK_DISPLAY_TEXT_CNF:
        UIM_MSG_HIGH_2("DISPLAY TXT rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        return gstk_populate_tr_additional_res(
			  &(command->cmd.display_text_term_rsp_cnf.result_additional_info), 
			    additional_result);
        break;
    case GSTK_GET_INKEY_CNF:
        UIM_MSG_HIGH_2("GET INKEY rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        if((gstk_status = gstk_populate_tr_additional_res(
			  &(command->cmd.get_inkey_term_rsp_cnf.result_additional_info), 
			    additional_result)) != GSTK_SUCCESS)
        {
          return gstk_status;
        }
        return gstk_populate_get_inkey_tr_cnf(command, additional_result, other_info,
			           terminal_response_enum, general_result, nv_data.cat_version);
        break;
    case GSTK_GET_INPUT_CNF:
        UIM_MSG_HIGH_2("GET INPUT rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        if((gstk_status = gstk_populate_tr_additional_res(
			  &(command->cmd.get_input_term_rsp_cnf.result_additional_info), 
			    additional_result)) != GSTK_SUCCESS)
        {
          return gstk_status;
        }		
        return gstk_populate_get_input_tr_cnf(command, additional_result, other_info,
			           terminal_response_enum, general_result, nv_data.cat_version);
        break;
    case GSTK_SELECT_ITEM_CNF:
        UIM_MSG_HIGH_2("SELECT ITEM rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        if((gstk_status = gstk_populate_tr_additional_res(
			  &(command->cmd.select_item_term_rsp_cnf.result_additional_info), 
			    additional_result)) != GSTK_SUCCESS)
        {
          return gstk_status;
        }
        return gstk_populate_select_item_tr_cnf(command, additional_result, other_info,
			           terminal_response_enum, general_result, nv_data.cat_version);
        break;
    case GSTK_SETUP_MENU_CNF:
        UIM_MSG_HIGH_2("SETUP MENU rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        return gstk_populate_tr_additional_res(
			  &(command->cmd.setup_menu_term_rsp_cnf.result_additional_info), 
			    additional_result);
        break;
    case GSTK_PROVIDE_LOCAL_INFO_CNF:
    case GSTK_PROVIDE_CSG_INFO_CNF:
    case GSTK_PROVIDE_NW_SEARCH_MODE_CNF:
        UIM_MSG_HIGH_2("PROVIDE LOCAL INFO rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);

        local_info_rsp_ptr = (gstk_local_info_rsp_type *)GSTK_CALLOC(sizeof(gstk_local_info_rsp_type));
        if(local_info_rsp_ptr == NULL)
        {
          gstk_status = GSTK_MEMORY_ERROR;
          break;
        }

        local_info_rsp_ptr->info_type = GSTK_INVALID_LOC_INFO;

        gstk_status = gstk_populate_provide_local_info_terminal_rsp_cnf(
                        command,
                        cmd_details_ref_id,
                        cmd_num,
                        general_result,
                        additional_result,
                        local_info_rsp_ptr,
                        user_data);

        gstk_free(local_info_rsp_ptr);
        local_info_rsp_ptr = NULL;
        break;
    case GSTK_TIMER_MANAGEMENT_CNF:
        /* internal */
        break;
    case GSTK_PROVIDE_BATT_STATUS_CNF:
      if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
      {
        UIM_MSG_HIGH_2("PROVIDE BATTERY STATUS rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);

        if((gstk_status = gstk_populate_tr_additional_res(
			  &(command->cmd.provide_local_info_term_rsp_cnf.result_additional_info), 
			    additional_result)) != GSTK_SUCCESS)
        {
          return gstk_status;
        } 
        gstk_status = gstk_util_error_check_terminal_rsp_other_info(
                            terminal_response_enum,
                            general_result,
                            other_info,
                            additional_result,
                            nv_data.cat_version);
        /* initialize the extra param for local info to no info */
        command->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_type = GSTK_INVALID_LOC_INFO;
        if(gstk_status == GSTK_SUCCESS)
        {
          if (other_info->present)
          {
            command->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_type = GSTK_BATTERY_STATUS_INFO;
            command->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.battery_status =
                other_info->extra_param.battery_status_info_extra_param;
          }
        }
        else
        {
          if (command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL)
          {
            gstk_free(command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr);
            command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
          }
        }
      }
      break;
    case GSTK_PROVIDE_LANG_INFO_CNF:
        UIM_MSG_HIGH_2("PROVIDE LANG INFO rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        if((gstk_status = gstk_populate_tr_additional_res(
			  &(command->cmd.provide_local_info_term_rsp_cnf.result_additional_info), 
			    additional_result)) != GSTK_SUCCESS)
        {
          return gstk_status;
        }

        gstk_status = gstk_util_error_check_terminal_rsp_other_info(
                            terminal_response_enum,
                            general_result,
                            other_info,
                            additional_result,
                            nv_data.cat_version);
        /* initialize the extra param for local info to no info */
        command->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_type = GSTK_INVALID_LOC_INFO;
        memset(command->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.lang_code,0x00,
               2);
        if(gstk_status == GSTK_SUCCESS) {
          if (other_info->present) {
            command->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_type = GSTK_LANGUAGE_SETTING_INFO;
            (void)memscpy(command->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.lang_code,
               sizeof(command->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.lang_code),
               other_info->extra_param.provide_lang_info_extra_param.lang_code,
               2);
          }
        }
        else {
          if (command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
            gstk_free(command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr);
            command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
          }
        }
        break;

    case GSTK_SETUP_IDLE_TEXT_CNF:
        UIM_MSG_HIGH_2("SETUP IDLE TXT rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        return gstk_populate_tr_additional_res(
			  &(command->cmd.setup_idle_text_term_rsp_cnf.result_additional_info), 
			    additional_result);
        break;
    case GSTK_RUN_AT_CMDS_CNF:
        break;
    case GSTK_LANG_NOTIFICATION_CNF:
        UIM_MSG_HIGH_2("LANG NOTIF rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        return gstk_populate_tr_additional_res(
			  &(command->cmd.lang_notification_term_rsp_cnf.result_additional_info), 
			    additional_result);
        break;
    case GSTK_SEND_DTMF_CNF:
        UIM_MSG_HIGH_2("SEND DTMF rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
         return gstk_populate_tr_additional_res(
			  &(command->cmd.send_dtmf_term_rsp_cnf.result_additional_info), 
			    additional_result);
        break;

     case GSTK_OPEN_CH_CNF:
        UIM_MSG_HIGH_2("OPEN CHANNEL rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);

        if((gstk_status = gstk_populate_tr_additional_res(
			  &(command->cmd.open_ch_term_rsp_cnf.result_additional_info), 
			    additional_result)) != GSTK_SUCCESS)
        {
          return gstk_status;
        } 
        gstk_status = gstk_util_error_check_terminal_rsp_other_info(
                            terminal_response_enum,
                            general_result,
                            other_info,
                            additional_result,
                            nv_data.cat_version);
        /* initialize the extra param for open channel rsp */
        memset(&command->cmd.open_ch_term_rsp_cnf.ch_status, 0x00, sizeof(gstk_ch_status_type));
        memset(&command->cmd.open_ch_term_rsp_cnf.bearer_description, 0x00, sizeof(gstk_bearer_description_type));
        memset(&command->cmd.open_ch_term_rsp_cnf.dns_addrs, 0x00, sizeof(gstk_dns_addrs_type));
        command->cmd.open_ch_term_rsp_cnf.buffer_size = 0;
        command->cmd.open_ch_term_rsp_cnf.other_data_present = FALSE;
        command->cmd.open_ch_term_rsp_cnf.ch_status_present = FALSE;
        command->cmd.open_ch_term_rsp_cnf.is_scws_ch        = FALSE;
        if(gstk_status == GSTK_SUCCESS) {
          if (other_info->present) {
            command->cmd.open_ch_term_rsp_cnf.other_data_present = TRUE;
            switch(general_result) {
            case GSTK_COMMAND_PERFORMED_SUCCESSFULLY:
            case GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION:
            case GSTK_COMMAND_PERFORMED_WITH_MISSING_INFORMATION:
            case GSTK_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED:
            case GSTK_COMMAND_PERFORMED_WITH_MODIFICATION:
              command->cmd.open_ch_term_rsp_cnf.ch_status_present = TRUE;
              (void)memscpy(&command->cmd.open_ch_term_rsp_cnf.ch_status,
                sizeof(gstk_ch_status_type),
                &other_info->extra_param.open_ch_extra_param.ch_status,
                sizeof(gstk_ch_status_type));
              break;
            default:
              /* no need to include ch status in case of failure */
              break;
            }
            if(other_info->extra_param.open_ch_extra_param.ch_status.is_scws_ch)
            {
              command->cmd.open_ch_term_rsp_cnf.is_scws_ch = TRUE;
            }
            UIM_MSG_HIGH_2("other_info->extra_param.open_ch_extra_param.ch_status.is_scws_ch 0x%x; command->cmd.open_ch_term_rsp_cnf.is_scws_ch 0x%x",
                           other_info->extra_param.open_ch_extra_param.ch_status.is_scws_ch,
                           command->cmd.open_ch_term_rsp_cnf.is_scws_ch);
            /* bearer description and buffer size */
            if(!command->cmd.open_ch_term_rsp_cnf.is_scws_ch)
            {
              (void)memscpy(&command->cmd.open_ch_term_rsp_cnf.bearer_description,
                    sizeof(gstk_bearer_description_type),
                    &other_info->extra_param.open_ch_extra_param.bearer_description,
                    sizeof(gstk_bearer_description_type));
            }
            command->cmd.open_ch_term_rsp_cnf.buffer_size = other_info->extra_param.open_ch_extra_param.buffer_size;
            if (other_info->extra_param.open_ch_extra_param.dns_addrs.num_addresses > 0)
            {
              command->cmd.open_ch_term_rsp_cnf.dns_addrs.num_addresses = other_info->extra_param.open_ch_extra_param.dns_addrs.num_addresses;

              if (command->cmd.open_ch_term_rsp_cnf.dns_addrs.num_addresses > 0)
              {
                command->cmd.open_ch_term_rsp_cnf.dns_addrs.addresses =
                  GSTK_CALLOC(command->cmd.open_ch_term_rsp_cnf.dns_addrs.num_addresses * sizeof(gstk_data_address_type));
                if (command->cmd.open_ch_term_rsp_cnf.dns_addrs.addresses == NULL)
                {
                  return GSTK_MEMORY_ERROR;
                }
                (void)memscpy(command->cmd.open_ch_term_rsp_cnf.dns_addrs.addresses,
                  command->cmd.open_ch_term_rsp_cnf.dns_addrs.num_addresses * sizeof(gstk_data_address_type),
                  other_info->extra_param.open_ch_extra_param.dns_addrs.addresses,
                  command->cmd.open_ch_term_rsp_cnf.dns_addrs.num_addresses * sizeof(gstk_data_address_type));

                for (i = 0; i < command->cmd.open_ch_term_rsp_cnf.dns_addrs.num_addresses; i++)
                {
                  command->cmd.open_ch_term_rsp_cnf.dns_addrs.addresses[i].address =
                    GSTK_CALLOC(other_info->extra_param.open_ch_extra_param.dns_addrs.addresses[i].length);

                  if (command->cmd.open_ch_term_rsp_cnf.dns_addrs.addresses[i].address)
                  {
                    other_info->extra_param.open_ch_extra_param.dns_addrs.addresses[i].length =
                    command->cmd.open_ch_term_rsp_cnf.dns_addrs.addresses[i].length;
                    (void)memscpy(command->cmd.open_ch_term_rsp_cnf.dns_addrs.addresses[i].address,
                      other_info->extra_param.open_ch_extra_param.dns_addrs.addresses[i].length,
                      other_info->extra_param.open_ch_extra_param.dns_addrs.addresses[i].address,
                      other_info->extra_param.open_ch_extra_param.dns_addrs.addresses[i].length);
                  }
                }
              }
            }
          }
        }
        else {
          if (command->cmd.open_ch_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
            gstk_free(command->cmd.open_ch_term_rsp_cnf.result_additional_info.additional_info_ptr);
            command->cmd.open_ch_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
          }
        }
        break;

     case GSTK_CLOSE_CH_CNF:
        UIM_MSG_HIGH_2("CLOSE CHANNEL rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        return gstk_populate_tr_additional_res(
			  &(command->cmd.close_ch_term_rsp_cnf.result_additional_info), 
			    additional_result);
        break;

     case GSTK_RECEIVE_DATA_CNF:
        UIM_MSG_HIGH_2("RECEIVE DATA rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        if((gstk_status = gstk_populate_tr_additional_res(
			  &(command->cmd.receive_data_term_rsp_cnf.result_additional_info), 
			   additional_result)) != GSTK_SUCCESS)
        {
          return gstk_status;
        }
        gstk_status = gstk_util_error_check_terminal_rsp_other_info(
                            terminal_response_enum,
                            general_result,
                            other_info,
                            additional_result,
                            nv_data.cat_version);
        /* initialize the extra param for receive data rsp */
        command->cmd.receive_data_term_rsp_cnf.ch_data.data_len = 0;
        command->cmd.receive_data_term_rsp_cnf.ch_data.data = NULL;
        command->cmd.receive_data_term_rsp_cnf.ch_data_remaining_len = 0;
        command->cmd.receive_data_term_rsp_cnf.data_present = FALSE;

        if(gstk_status == GSTK_SUCCESS) {
          if (other_info->present) {
            command->cmd.receive_data_term_rsp_cnf.data_present = TRUE;
            if ((other_info->extra_param.receive_data_extra_param.ch_data.data_len > 0) &&
                (other_info->extra_param.receive_data_extra_param.ch_data.data != NULL)) {
              command->cmd.receive_data_term_rsp_cnf.ch_data.data = GSTK_CALLOC(
                       int32touint32(other_info->extra_param.receive_data_extra_param.ch_data.data_len));
              if (command->cmd.receive_data_term_rsp_cnf.ch_data.data != NULL) {
                (void)memscpy(command->cmd.receive_data_term_rsp_cnf.ch_data.data,
                  int32touint32(other_info->extra_param.receive_data_extra_param.ch_data.data_len),
                  other_info->extra_param.receive_data_extra_param.ch_data.data,
                  int32touint32(other_info->extra_param.receive_data_extra_param.ch_data.data_len));
                command->cmd.receive_data_term_rsp_cnf.ch_data.data_len =
                  other_info->extra_param.receive_data_extra_param.ch_data.data_len;
                if (other_info->extra_param.receive_data_extra_param.ch_data_remaining_len > 0xFF) {
                  command->cmd.receive_data_term_rsp_cnf.ch_data_remaining_len = 0xFF;
                }
                else {
                  command->cmd.receive_data_term_rsp_cnf.ch_data_remaining_len =
                    other_info->extra_param.receive_data_extra_param.ch_data_remaining_len;
                }
              }
              else {
                gstk_status = GSTK_MEMORY_ERROR;
              }
            }
          }
        }

        if (gstk_status != GSTK_SUCCESS) {
          if (command->cmd.receive_data_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
            gstk_free(command->cmd.receive_data_term_rsp_cnf.result_additional_info.additional_info_ptr);
            command->cmd.receive_data_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
          }
        }
        break;

     case GSTK_SEND_DATA_CNF:
        UIM_MSG_HIGH_2("SEND DATA rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        if((gstk_status = gstk_populate_tr_additional_res(
			  &(command->cmd.send_data_term_rsp_cnf.result_additional_info), 
			    additional_result)) != GSTK_SUCCESS)
        {
          return gstk_status;
        }

        gstk_status = gstk_util_error_check_terminal_rsp_other_info(
                           terminal_response_enum,
                           general_result,
                           other_info,
                           additional_result,
                           nv_data.cat_version);
        /* initialize the extra param for send data rsp */
        command->cmd.send_data_term_rsp_cnf.ch_data_remain_buf_len = 0;
        command->cmd.send_data_term_rsp_cnf.data_present = FALSE;

        if(gstk_status == GSTK_SUCCESS) {
          if (other_info->present) {
            command->cmd.send_data_term_rsp_cnf.data_present = TRUE;
            if (other_info->extra_param.send_data_extra_param.ch_data_remain_buf_len > 0xFF) {
              command->cmd.send_data_term_rsp_cnf.ch_data_remain_buf_len = 0xFF;
            }
            else {
              command->cmd.send_data_term_rsp_cnf.ch_data_remain_buf_len =
                other_info->extra_param.send_data_extra_param.ch_data_remain_buf_len;
            }
          }
        }
        else {
          if (command->cmd.send_data_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
            gstk_free(command->cmd.send_data_term_rsp_cnf.result_additional_info.additional_info_ptr);
            command->cmd.send_data_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
          }
        }

        break;

     case GSTK_GET_CH_STATUS_CNF:
        UIM_MSG_HIGH_2("GET CH STATUS rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        if((gstk_status = gstk_populate_tr_additional_res(
			  &(command->cmd.get_ch_status_term_rsp_cnf.result_additional_info), 
			    additional_result)) != GSTK_SUCCESS)
        {
          return gstk_status;
        }
        gstk_status = gstk_util_error_check_terminal_rsp_other_info(
                            terminal_response_enum,
                            general_result,
                            other_info,
                            additional_result,
                            nv_data.cat_version);
        /* initialize the extra param for open channel rsp */
        memset(command->cmd.get_ch_status_term_rsp_cnf.ch_status, 0x00, sizeof(gstk_ch_status_type));
        command->cmd.get_ch_status_term_rsp_cnf.data_present = FALSE;

        if(gstk_status == GSTK_SUCCESS)
        {
          if (other_info->present)
          {
            command->cmd.get_ch_status_term_rsp_cnf.data_present = TRUE;
            if((other_info->extra_param.get_ch_status_extram_param.cnt > GSTK_MAX_GPRS_CHANNELS) ||
               (other_info->extra_param.get_ch_status_extram_param.cnt < 0))
            {
              UIM_MSG_ERR_0("No of Channels cannot be greater than MAX 0x%x or negative");
              return GSTK_BAD_PARAM;
            }
            command->cmd.get_ch_status_term_rsp_cnf.ch_cnt = other_info->extra_param.get_ch_status_extram_param.cnt;
            (void)memscpy(command->cmd.get_ch_status_term_rsp_cnf.ch_status,
                    sizeof(command->cmd.get_ch_status_term_rsp_cnf.ch_status),
                    other_info->extra_param.get_ch_status_extram_param.ch_status,
                    sizeof(gstk_ch_status_type) *
                    int32touint32(command->cmd.get_ch_status_term_rsp_cnf.ch_cnt));
          }
        }
        else {
          if (command->cmd.get_ch_status_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
            gstk_free(command->cmd.get_ch_status_term_rsp_cnf.result_additional_info.additional_info_ptr);
            command->cmd.get_ch_status_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
          }
        }
        break;

    case GSTK_ACTIVATE_CNF:
        UIM_MSG_HIGH_2("ACTIVATE rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        return gstk_populate_tr_additional_res(
			  &(command->cmd.activate_term_rsp_cnf.result_additional_info), 
			    additional_result);
        break;

    case GSTK_UNSUPPORTED_UNKNOWN_CMD_CNF:
        UIM_MSG_HIGH_2("UNSUPPORTED/UNKNOWN rsp, result=0x%x additional info len=0x%x",
                       general_result, additional_result->length);
        break;
    default:
        UIM_MSG_ERR_1("TR enum unexpected 0x%x", terminal_response_enum);
        return GSTK_BAD_PARAM;
    }

    return gstk_status;
} /* gstk_populate_terminal_response_cnf*/

/*===========================================================================
FUNCTION gstk_process_terminal_response

DESCRIPTION

  This function switches on the command id and call the corresponding
  terminal response cnf function so that GSTK can build the corrresponding
  terminal response tlvs and send it to UIM Server

PARAMETERS
  cmd: [Input] Pointer to the terminal response confirm types to be processed

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
gstk_status_enum_type gstk_process_terminal_response(
    gstk_cmd_type* cmd )
{
    gstk_status_enum_type               gstk_status          = GSTK_SUCCESS;
    uint32                              type_of_command;
    uint32                              ref_id;
    gstk_nv_items_data_type             nv_data;

    GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

    if(cmd == NULL)
    {
      UIM_MSG_ERR_0("cmd ERR:NULL");
      return GSTK_BAD_PARAM;
    }

    /* Read the CAT version */
    (void)gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                              gstk_curr_inst_ptr->slot_id,
                              &nv_data);

    /* Get the type of command */
    type_of_command = cmd->general_cmd.message_header.command_id;

    switch( type_of_command ) {
    case GSTK_REFRESH_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->refresh_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_refresh_cnf(&(cmd->refresh_term_rsp_cnf));
        break;
    case GSTK_MORE_TIME_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->more_time_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_more_time_cnf(&(cmd->more_time_term_rsp_cnf));
        break;
    case GSTK_POLLING_OFF_CNF:
    case GSTK_POLL_INTERVAL_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->poll_interval_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_polling_cnf(&(cmd->poll_interval_term_rsp_cnf));
        break;
    case GSTK_SETUP_EVENT_LIST_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->setup_evt_list_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
       command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd->setup_evt_list_term_rsp_cnf.cmd_details_ref_id)].pending_responses--;
        gstk_status = gstk_consolidate_setup_evt_list_cnf(
                        &(cmd->setup_evt_list_term_rsp_cnf));
        break;
    case GSTK_SETUP_CALL_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->setup_call_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_setup_mo_call_cnf(&(cmd->setup_call_term_rsp_cnf));
        break;
    case GSTK_SEND_SS_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->send_ss_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_send_ss_cnf(&(cmd->send_ss_term_rsp_cnf));
        break;
    case GSTK_SEND_USSD_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->send_ussd_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_send_ussd_cnf(&(cmd->send_ussd_term_rsp_cnf));
        break;
    case GSTK_SEND_SMS_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->send_sms_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_send_sms_cnf(&(cmd->send_sms_term_rsp_cnf));
        break;
    case GSTK_LAUNCH_BROWSER_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->launch_browser_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_launch_browser_cnf(&(cmd->launch_browser_term_rsp_cnf));
        break;
    case GSTK_PLAY_TONE_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->play_tone_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_play_tone_cnf(&(cmd->play_tone_term_rsp_cnf));
        break;
    case GSTK_DISPLAY_TEXT_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->display_text_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_display_text_cnf(&(cmd->display_text_term_rsp_cnf));
        break;
    case GSTK_GET_INKEY_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->get_inkey_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_get_inkey_cnf(&(cmd->get_inkey_term_rsp_cnf));
        break;
    case GSTK_GET_INPUT_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->get_input_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_get_input_cnf(&(cmd->get_input_term_rsp_cnf));
        break;
    case GSTK_SELECT_ITEM_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->select_item_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_select_item_cnf(&(cmd->select_item_term_rsp_cnf));
        break;
    case GSTK_SETUP_MENU_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->setup_menu_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_setup_menu_cnf(&(cmd->setup_menu_term_rsp_cnf));
        break;
    case GSTK_PROVIDE_ACCESS_TECH_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->provide_local_info_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        UIM_MSG_HIGH_0("GSTK_PROVIDE_ACCESS_TECH_CNF...");
        gstk_status = gstk_provide_local_info_cnf(&(cmd->provide_local_info_term_rsp_cnf));
        break;
    case GSTK_PROVIDE_LOCAL_INFO_CNF:
    case GSTK_PROVIDE_CSG_INFO_CNF:
    case GSTK_PROVIDE_NW_SEARCH_MODE_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->provide_local_info_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        UIM_MSG_HIGH_0("GSTK_PROVIDE_LOCAL_INFO_CNF...");
        gstk_status = gstk_provide_local_info_cnf(&(cmd->provide_local_info_term_rsp_cnf));
        break;
    case GSTK_PROVIDE_LANG_INFO_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->provide_local_info_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_provide_local_info_cnf(&(cmd->provide_local_info_term_rsp_cnf));
        break;
    case GSTK_PROVIDE_BATT_STATUS_CNF:
        if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
        {
          gstk_status = gstk_util_compare_ref_id(cmd->provide_local_info_term_rsp_cnf.cmd_details_ref_id);
          if(gstk_status != GSTK_SUCCESS)
          {
            break;
          }
          gstk_status = gstk_provide_local_info_cnf(&(cmd->provide_local_info_term_rsp_cnf));
        }
        break;
    case GSTK_SETUP_IDLE_TEXT_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->setup_idle_text_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_setup_idle_mode_text_cnf(&(cmd->setup_idle_text_term_rsp_cnf));
        break;
    case GSTK_TIMER_MANAGEMENT_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->timer_manage_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_timer_management_cnf(&(cmd->timer_manage_term_rsp_cnf));
        break;
    case GSTK_LANG_NOTIFICATION_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->lang_notification_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_lang_notification_cnf(&(cmd->lang_notification_term_rsp_cnf));
        break;
    case GSTK_SEND_DTMF_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->send_dtmf_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_send_dtmf_cnf(&(cmd->send_dtmf_term_rsp_cnf));
        break;
    case GSTK_OPEN_CH_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->open_ch_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_open_ch_cnf(&(cmd->open_ch_term_rsp_cnf));
        break;
    case GSTK_CLOSE_CH_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->close_ch_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_close_ch_cnf(&(cmd->close_ch_term_rsp_cnf));
        break;
    case GSTK_SEND_DATA_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->send_data_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_send_data_cnf(&(cmd->send_data_term_rsp_cnf));
        break;
    case GSTK_RECEIVE_DATA_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->receive_data_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_receive_data_cnf(&(cmd->receive_data_term_rsp_cnf));
        break;
    case GSTK_GET_CH_STATUS_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->activate_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_get_ch_status_cnf(&(cmd->get_ch_status_term_rsp_cnf));
        break;
    case GSTK_ACTIVATE_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->activate_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_activate_cnf(&(cmd->activate_term_rsp_cnf));
        break;
    case GSTK_RUN_AT_CMDS_CNF:
    case GSTK_UNSUPPORTED_UNKNOWN_CMD_CNF:
        gstk_status = gstk_util_compare_ref_id(cmd->unknown_unsupported_term_rsp_cnf.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          break;
        }
        gstk_status = gstk_unsupported_unknown_cnf(&(cmd->unknown_unsupported_term_rsp_cnf),
			                      (cmd->unknown_unsupported_term_rsp_cnf.command_result));
        break;
    case GSTK_RAW_TERMIMAL_RESPONSE:
        gstk_status = gstk_util_compare_ref_id(cmd->client_raw_term_resp.cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS)
        {
          gstk_shared_data.gstk_client_table[cmd->client_raw_term_resp.client_id - 1].user_data =
                                                cmd->client_raw_term_resp.message_header.user_data;
          gstk_util_send_sw_to_client(NULL);
          break;
        }
        gstk_status = gstk_process_raw_terminal_response(&(cmd->client_raw_term_resp));
        break;
    default:
        gstk_status = GSTK_INVALID_COMMAND;
        break;
    }

    if(gstk_status != GSTK_SUCCESS)
    {
       ref_id = cmd->unknown_unsupported_term_rsp_cnf.cmd_details_ref_id;  
       gstk_status = gstk_util_compare_ref_id(ref_id);
       if(gstk_status != GSTK_SUCCESS)
       {
         UIM_MSG_ERR_0("REF_ID Mismatch");
         return gstk_status;
       }
       gstk_status = gstk_unsupported_unknown_cnf(&(cmd->unknown_unsupported_term_rsp_cnf),
	   	                             GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES);
    }
    return(gstk_status);
} /* gstk_process_terminal_response */


/*===========================================================================
FUNCTION gstk_uim_terminal_response_report

DESCRIPTION

  This is the callback function for terminal response response from UIM.
  UIM will call this function when it receives terminal response response
  from the Card.
  Based on the return status of the terminal response response, this function
  will decide whether an end proactive session request should be sent
  to the client or not

PARAMETERS
  report: [Input] Pointer to the UIM terminal response report

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
void gstk_uim_terminal_response_report (uim_rpt_type *report)
{
    gstk_task_cmd_type      *task_cmd;

    if(report == NULL)
    {
      UIM_MSG_ERR_0("report ERR:NULL");
      return;
    }

    if (!GSTK_IS_VALID_SLOT_ID(report->slot))
    {
      UIM_MSG_ERR_1("Invalid UIM slot 0x%x", report->slot);
      return;
    }

    if (gstk_instances_ptr[(uint8)(report->slot) - 1]->tr_unique_ref_id != report->user_data)
    {
      UIM_MSG_HIGH_0("Ignoring duplicate/invalid TR rsp report");
      return;
    }

    /* Get UIM response for Terminal Response proactive cmd  */
    UIM_MSG_HIGH_2 ("Current status 0x%x in uim report for slot 0x%x",
                    gstk_instances_ptr[(uint8)(report->slot)-1]->gstk_proactive_cmd_in_progress,
                    report->slot);

    GSTK_SET_PROACTIVE_CMD_IN_PROGRESS_FLAG(
      gstk_instances_ptr[(uint8)(report->slot) - 1]->gstk_proactive_cmd_in_progress, FALSE);

    UIM_MSG_HIGH_2("GSTK rec'd Term Rsp from UIM, 0x%x, 0x%x",
                   report->sw1, report->sw2);

    if (report->rpt_type != UIM_TERMINAL_RESPONSE_R) {
        // build error message to GSTK?????????????
        UIM_MSG_ERR_0("TR expected in CB");
        return;
    }

    task_cmd = gstk_task_get_cmd_buf();

    if (task_cmd != NULL)
    {
      task_cmd->cmd.general_cmd.message_header.sim_slot_id =
        (gstk_slot_id_enum_type)report->slot;
      task_cmd->cmd.general_cmd.message_header.command_group =
        GSTK_TR_REPORT;
      task_cmd->cmd.general_cmd.message_header.command_id = 0; /* doesn't matter */
      task_cmd->cmd.general_cmd.length = 3;
      task_cmd->cmd.general_cmd.data[0] = (uint8)report->sw1;
      task_cmd->cmd.general_cmd.data[1] = (uint8)report->sw2;
      if (report->rpt.terminal_response.proactive_cmd_pending)
      {
        task_cmd->cmd.general_cmd.data[2] = 1;
      }
      else
      {
        task_cmd->cmd.general_cmd.data[2] = 0;
      }
      task_cmd->cmd.general_cmd.message_header.user_data = report->user_data;
      gstk_task_put_cmd_buf(task_cmd);
      /* TR response SW1 is not 91, then check the other slots for pending proactive commands */
      if(!report->rpt.terminal_response.proactive_cmd_pending)
      {
        /* Reset the ui_nw_proactive_session_slot_in_progress if the session ended is a UI or NW session */
        if(gstk_shared_data.ui_nw_proactive_session_slot_in_progress == (gstk_slot_id_enum_type)report->slot)
        {
          gstk_shared_data.ui_nw_proactive_session_slot_in_progress = GSTK_SLOT_ID_MAX;
        }
        gstk_util_check_slot_with_fetch_rejected();
      }
    }
}/*lint !e818 *report suppression for externalized function */
/* gstk_uim_terminal_response_report */

/*===========================================================================
FUNCTION gstk_util_error_check_terminal_rsp

DESCRIPTION
  This function will perform some error checking to ensure that
  the Application sending the Terminal Response has an
  opportunity to resend the Terminal Response incase it is
  invalid

PARAMETERS
  terminal_response_enum: [Input] Used to indicate which Terminal Response
                                   to check.
  general_result:         [Input] Used to check the result of the command
  *other_info             [Input] Union of Proactive Commands
                                  Poll Interval
                                  Get Inkey
                                  Get Input
                                  Select Item
                                  Setup Call
                                  Send SS
                                  Send USSD



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
gstk_status_enum_type gstk_util_error_check_terminal_rsp_other_info (
    gstk_cmd_enum_type                          terminal_response_enum,
    gstk_general_result_enum_type               general_result,
    const gstk_terminal_rsp_extra_param_type   *other_info,
    const gstk_additional_info_ptr_type        *additional_result,
    uint8                                       cat_version
)
{
    if(other_info == NULL)
    {
      UIM_MSG_ERR_0("other_info ERR:NULL");
      return GSTK_BAD_PARAM;
    }


    switch(terminal_response_enum) {

    case GSTK_POLL_INTERVAL_CNF:
      switch(general_result) {
      case GSTK_COMMAND_PERFORMED_SUCCESSFULLY:
      case GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION:
      case GSTK_COMMAND_PERFORMED_WITH_MISSING_INFORMATION:
      case GSTK_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED:
      case GSTK_COMMAND_PERFORMED_WITH_MODIFICATION:
        /* check for input length */
        if(!other_info->present) {
          return GSTK_EXTRA_PARAM_MISSING;
        }
        else { /* other info present, check input len */
          if(!other_info->extra_param.poll_duration.present) {
            return GSTK_EXTRA_PARAM_MISSING;
          }
          else {
            if(other_info->extra_param.poll_duration.units != GSTK_DURATION_MINUTES &&
               other_info->extra_param.poll_duration.units != GSTK_DURATION_SECONDS &&
               other_info->extra_param.poll_duration.units != GSTK_DURATION_TENTHS_OF_SECONDS) {
              return GSTK_BAD_PARAM;
            }
          }
        }
        break;
      default: /* other error cases */
        if(other_info->present) {
          return GSTK_EXTRA_PARAM_NOT_REQUIRED;
        }
        break;
      }
      break;

    case GSTK_SELECT_ITEM_CNF:
      switch(general_result) {
      case GSTK_HELP_INFORMATION_REQUIRED_BY_THE_USER:
      case GSTK_COMMAND_PERFORMED_SUCCESSFULLY:
      case GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION:
      case GSTK_COMMAND_PERFORMED_WITH_MISSING_INFORMATION:
      case GSTK_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED:
      case GSTK_COMMAND_PERFORMED_WITH_MODIFICATION:
        /* check for input length */
        if(!other_info->present) {
          return GSTK_EXTRA_PARAM_MISSING;
        }
        break;
      default: /* other error cases */
        if(other_info->present) {
          return GSTK_EXTRA_PARAM_NOT_REQUIRED;
        }
        break;
      }
      break;

    case GSTK_PROVIDE_LANG_INFO_CNF:
    case GSTK_PROVIDE_BATT_STATUS_CNF:
      if(cat_version < GSTK_CFG_CAT_VER6)
      {
        UIM_MSG_ERR_1("unknown confirm type, 0x%x", terminal_response_enum);
        break;
      }
      else
      {
        /* Allow fallthrough */
      }
    case GSTK_GET_INKEY_CNF:
    case GSTK_GET_INPUT_CNF:
    case GSTK_SEND_DATA_CNF:
    case GSTK_GET_CH_STATUS_CNF:
    case GSTK_RECEIVE_DATA_CNF:
      switch(general_result) {
      case GSTK_COMMAND_PERFORMED_SUCCESSFULLY:
      case GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION:
      case GSTK_COMMAND_PERFORMED_WITH_MISSING_INFORMATION:
      case GSTK_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED:
      case GSTK_COMMAND_PERFORMED_WITH_MODIFICATION:
        /* check for input length */
        if(!other_info->present) {
          return GSTK_EXTRA_PARAM_MISSING;
        }
        break;
      case GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR:
       if(additional_result != NULL)
       {
          if(additional_result->length == 0)
          {
            UIM_MSG_HIGH_0("No other_info");
            return GSTK_EXTRA_PARAM_MISSING;
          }
        }
        else
        {
          UIM_MSG_HIGH_0("No other_info");
          return GSTK_EXTRA_PARAM_MISSING;
        }
        break;
      default: /* other error cases */
        if(other_info->present
           &&
           (terminal_response_enum != GSTK_GET_INKEY_CNF) )
        {
          return GSTK_EXTRA_PARAM_NOT_REQUIRED;
        }
        break;
      }
      break;

    case GSTK_OPEN_CH_CNF:  /* at least have bearer and buffer size */
      if(!other_info->present) {
        if (general_result != GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME) {
          return GSTK_EXTRA_PARAM_MISSING;
        }
      }
      break;

    default:
      UIM_MSG_ERR_1("unknown confirm type, 0x%x", terminal_response_enum);
      break;

    }

    return GSTK_SUCCESS;
}

/*===========================================================================
FUNCTION gstk_unsupported_unknown_cnf

DESCRIPTION
  This function packs the Terminal response for an unsupported or unknown
  proactive command the command to the card through the UIM server.

PARAMETERS
  gstk_unsupported_unknown_cnf: [Input] Pointer to message that is required to
                                be processed and sent to UIM

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
gstk_status_enum_type gstk_unsupported_unknown_cnf (
  gstk_unsupported_proactive_cmd_cnf_type* unsupported_unknown_cnf,
  gstk_general_result_enum_type            command_result)
{
  gstk_status_enum_type                     gstk_status    = GSTK_SUCCESS;
  uim_cmd_type                              *uim_cmd_ptr   = NULL;
  uint32                                     term_resp_len = 0;
  int32                                      offset        = 0;
  gstk_cmd_term_rsp_common_type              mandatory_tlv_info;

  UIM_MSG_HIGH_0("IN gstk_unsupported_unknown_cnf ");

  if(unsupported_unknown_cnf == NULL)
  {
    UIM_MSG_ERR_0("gstk_unsupported_unknown_cnf  ERR:NULL");
    return GSTK_BAD_PARAM;
  }

  uim_cmd_ptr = gstk_get_uim_buf();

  if(uim_cmd_ptr == NULL)
  {
    return GSTK_MEMORY_ERROR;
  }

  gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_RESPONSE_F,  &uim_cmd_ptr);
  if (gstk_status != GSTK_SUCCESS)
  {
    /* Release the upper cmd layer memory and the uim_cmd_ptr */
    gstk_util_release_upper_layer_cmd_memory(
      (gstk_cmd_type*)((void*)unsupported_unknown_cnf));/*lint !e826 area too small */
    return GSTK_ERROR;
  }

  /* set user_data */
  uim_cmd_ptr->hdr.user_data               = gstk_curr_inst_ptr->client_id;
  /* populate the terminal_response info */

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */

  offset = 0;

  /* Populate
   ** Command details tlv
   ** Device identity tlv
   ** Result tlv
  */

  /* Command details info */
  mandatory_tlv_info.cmd_info_data.ref_id           = unsupported_unknown_cnf->cmd_details_ref_id;
  mandatory_tlv_info.cmd_info_data.command_rsp_type = unsupported_unknown_cnf->message_header.command_id;
  mandatory_tlv_info.cmd_info_data.command_number   = unsupported_unknown_cnf->command_number;

  /* device ID */
  mandatory_tlv_info.dev_id_data.src = GSTK_ME_DEVICE;
  mandatory_tlv_info.dev_id_data.dest = GSTK_UICC_SIM_DEVICE;

  /* result tag */
  mandatory_tlv_info.result_info_data.command_result = command_result;
  mandatory_tlv_info.result_info_data.result_additional_info.length = 0;
  mandatory_tlv_info.result_info_data.result_additional_info.additional_info_ptr = NULL;

  /* Populate the command details, device identity and the result tlvs */
  gstk_status = gstk_util_populate_term_rsp_common_tlv(
                  offset,
                  &term_resp_len,
                  mandatory_tlv_info,
                  uim_cmd_ptr->terminal_response.data);

  if (gstk_status != GSTK_SUCCESS)
  {
    gstk_util_release_upper_layer_cmd_memory(
      (gstk_cmd_type*)((void*)unsupported_unknown_cnf));/*lint !e826 area too small */
    gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
    uim_cmd_ptr = NULL;
    return GSTK_ERROR;
  }

  offset = offset + uint32toint32(term_resp_len);

  /* populate the terminal_response info - total number of bytes*/
  uim_cmd_ptr->terminal_response.num_bytes = uint32touint8(term_resp_len);

  /* Send response to STK application on SIM */
  /* Send Message to UIM */
  gstk_util_dump_byte_array("Unknown Unsupported command: TR",
                            uim_cmd_ptr->terminal_response.data,
                            uim_cmd_ptr->terminal_response.num_bytes);

  /* clear the command detail table reference slot before since we are about to reply to
  the card */
  gstk_util_cmd_details_ref_table_free_slot(unsupported_unknown_cnf->cmd_details_ref_id);

  gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */

  return gstk_status;

} /* gstk_unsupported_unknown_cnf */

/*===========================================================================
FUNCTION gstk_get_term_rsp_timer_exp_error_code

DESCRIPTION
  This function returns appropriate terminal response error code when a proactive
  command timer expires. If the proactive command whose timer expires is
  DISPLAY TEXT, GET INKEY, GET INPUT or SELECT ITEM, the function returns
  GSTK_NO_RESPONSE_FROM_USER; otherwise, the function returns
  GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND.

PARAMETERS
  cmd_type: [Input] Type of command

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_NO_RESPONSE_FROM_USER,
    GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_general_result_enum_type gstk_get_term_rsp_timer_exp_error_code (
  gstk_cmd_enum_type cmd_type
)
{
  switch (cmd_type)
  {
  case GSTK_DISPLAY_TEXT_REQ:
  case GSTK_GET_INKEY_REQ:
  case GSTK_GET_INPUT_REQ:
  case GSTK_SELECT_ITEM_REQ:
    return GSTK_NO_RESPONSE_FROM_USER;
  default:
    return GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
  }
} /* gstk_get_term_rsp_timer_exp_error_code */

/*===========================================================================
FUNCTION gstk_process_term_rsp_timer_expire

DESCRIPTION
  For each proactive command sent to client, there is a timer associated with
  it. The corresponding terminal response has to be received within this period.
  And if the timer expires before the terminal response is revceived, this function
  will free up the entry in command details reference table and send "No Response"
  terminal response down to UIM server.

PARAMETERS
  ref_id: [Input] The reference id of the entry in command_details_ref_table
                  whose timer expired.

DEPENDENCIES
  None

RETURN VALUE
  GSTK_SUCCESS,
  GSTK_ERROR,
  GSTK_CLIENT_NOT_REGISTERED,
  GSTK_MEMORY_ERROR,
  GSTK_BAD_PARAM,
  GSTK_NULL_INPUT_PARAM,
  GSTK_PARAM_EXCEED_BOUNDARY,
  GSTK_INVALID_LENGTH,
  GSTK_EXTRA_PARAM_MISSING,
  GSTK_EXTRA_PARAM_NOT_REQUIRED,
  GSTK_UNSUPPORTED_COMMAND,
  GSTK_UNKNOWN_COMMAND,
  GSTK_INSUFFICIENT_INFO,
  GSTK_NOT_INIT

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_process_term_rsp_timer_expire (
  uint32  ref_id
)
{
  gstk_status_enum_type              gstk_status    = GSTK_SUCCESS;
  gstk_cmd_enum_type                 cmd_type       = GSTK_CMD_ENUM_MAX;
  gstk_additional_info_type          additional_info;
  gstk_terminal_rsp_extra_param_type extra_param;
  uint32                             index          = GSTK_CMD_REF_TABLE_INDEX(ref_id);

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);
  UIM_MSG_HIGH_0("gstk_process_term_rsp_timer_expire");
  if (index >= GSTK_MAX_PROACTIVE_COMMAND)
  {
    UIM_MSG_ERR_1("Invalid timer ref id %d!", ref_id);
    return GSTK_BAD_PARAM;
  }

  memset(&additional_info, 0x00, sizeof(gstk_additional_info_type));
  memset(&extra_param, 0x00, sizeof(gstk_terminal_rsp_extra_param_type));
  additional_info.length = 0;
  extra_param.present    = FALSE;

  gstk_status = gstk_util_convert_from_stk_cmd_type_to_gstk_cmd_type(
    command_details_ref_table_ptr[index].command_details.type_of_command,
    command_details_ref_table_ptr[index].command_details.qualifier,
    &cmd_type);
  if (gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_0("Failed to convert stk cmd to gstk cmd!");
    return GSTK_ERROR;
  }

  UIM_MSG_HIGH_2(" Command details type 0x%x, qualifier 0x%x",
                 command_details_ref_table_ptr[index].command_details.type_of_command,
                 command_details_ref_table_ptr[index].command_details.qualifier);

  /* clean up memory if there is any */
  switch (command_details_ref_table_ptr[index].command_details.type_of_command)
  {
  case GSTK_CMD_STK_SET_UP_EVENT_LIST:
    /* set pending_response to 1 so that we can send terminal response below */
    command_details_ref_table_ptr[index].pending_responses = 1;
    break;

  case GSTK_CMD_STK_OPEN_CHANNEL:
    UIM_MSG_HIGH_0("Adding extra param for open channel error terminal response");
    extra_param.present = TRUE;
    extra_param.rsp_type = GSTK_OPEN_CH_CNF;
    if(command_details_ref_table_ptr[index].proc_cmd_info.open_channel.is_scws_status == TRUE)
    {
       extra_param.extra_param.open_ch_extra_param.ch_status.is_scws_ch = TRUE;
    }
    (void)memscpy(&extra_param.extra_param.open_ch_extra_param.bearer_description,
           sizeof(gstk_bearer_description_type),
           &gstk_curr_inst_ptr->gstk_curr_bearer_description,
           sizeof(gstk_bearer_description_type));
    extra_param.extra_param.open_ch_extra_param.buffer_size =
                          gstk_curr_inst_ptr->gstk_curr_bearer_buffer_size;
    break;

  default:
    /* do nothing */
    break;
  }

  /* send NO RESPONSE FROM USER terminal response */
  gstk_status = gstk_send_terminal_response(
                  gstk_curr_inst_ptr->client_id,
                  0,  /* user data doesn't matter */
                  ref_id,
                  command_details_ref_table_ptr[index].command_details.command_number,
                  command_details_ref_table_ptr[index].expected_cmd_rsp_type,
                  gstk_get_term_rsp_timer_exp_error_code(cmd_type),
                  &additional_info,
                  &extra_param);
  if (gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_0("Failed to send NO RESPONSE term rsp!");
    return gstk_status;
  }

  return GSTK_SUCCESS;
} /* gstk_process_term_rsp_timer_expire */
