/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                   G S T K   T E R M I N A L   P R O F I L E


GENERAL DESCRIPTION
  This source file contains functions for handling terminal profile download



FUNCTIONS
  gstk_uim_terminal_profile_report
    This is the callback function for terminal profile response.

  gstk_cache_tp_info
    This function caches the terminal profile data received from GSTK clients

  gstk_pack_tp_cmd_to_uim
    This function puts the gstk_terminal_profile_cache info into uim format
    and put onto UIM's command queue

  gstk_toolkit_mode_change
    Inform clients that there is a mode change

  gstk_process_mode_change
    This function will perform UIM reset and toolkit change based on new CM
    mode if required

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2019 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/gstk_term_profile.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/13/19   ks      Added support for refresh enforcement policy
05/24/19   dt      Set default value for PLI lang bit in TP to 0
10/12/18   ks      Split ME and slot specific NV
10/12/17   dt      Removing incorrect entries in default app specific TP
08/25/17   shr     Disable Call Event Envelope TP bits if GSTK_CFG_FEATURE_ME_
                   DISABLE_ESTK_CM_VOICE_CALL_INTERFACE is enabled
03/27/17   dt      Add psm status to TP from MMGSDI
03/17/17   dt      Enabling ND and NK TP Bits for SAT
03/13/17   gs      Update the right index for PLI TP bit
11/15/16   shr     Use GBA utility function to check GBA support
09/12/16   shr     Add support for Poll Interval Envelope
09/15/16   gs      Changes in TP based on RAT and App Type
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
05/26/16   gm      Setting proper TP for IOE devices
05/11/16   gm      Setting proper TP for data card
05/09/16   gs      Removal of Dynamic TP Mechanism
03/08/16   vr      Split Activate and contactless proactive command files
03/07/16   gs      Disable the TP based on the Terminal Type
07/13/15   lm      Fixed the condition to check number of TP DL 
05/12/15   dy      Enabling Toolkit-initiated GBA TP bit (byte 25 bit 5)
04/27/15   vr      Non UI and non NW proactive command parallel processing
11/13/14   shr     Fixed compilation warnings
11/11/14   shr     Fixed compilation warnings
10/21/14   shr     Add GSTK NV support to control IMS bits in TP
10/07/14   shr     Memory optmization to the GSTK NV logic
09/09/14   bd      Features clean up
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
08/08/14   gm      Free memory allocated for sending command to UIM
08/05/14   lxu     Use NV Bitmask to control the Dual STK App
08/05/14   lxu     Support feature FEATURE_MODE_SWITCH_STK_SWITCH
07/23/14   vr      Add Rel99, Rel4 and Rel5 cat versions
07/11/14   shr     Add Support for NV Refresh
05/20/14   dy      Fixed handling of apps_enabled in mmgsdi_session_changed
04/25/14   hn      Add GSTK_CFG_CAT_VER12 for DNS query/Channel Reuse TP
04/03/14   dy      Replace gstk_malloc() with macro
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
03/17/14   dy      Print correct num of TP bytes in gstk_pack_tp_cmd_to_uim()
02/03/14   dy      Remove usage of GSTK_CFG_FEATURE_CUST_TP_31_32_BYTE
01/09/14   lxu     Send 29 bytes TP when cat version is 6, set 25th TP to 0 when
                   GSTK_CFG_FEATURE_CUST_TP_25_BYTE is enabled
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
01/09/14   kb      Updated GSTK_NULL_CHECK_PTR macro
08/27/13   jd      Fix of data card TP issue
07/25/13   hn      Enable FEATURE_ESTK_BIP_SERVER_MODE, add control bitmask
05/10/13   hh      Fix KW Critical error
04/26/13   gm      Not allowing fetch before TP download
04/09/13   vr      Use safer versions of memcpy() and memmove()
11/13/12   bd      Support for Closed Subscriber Group feature
09/24/12   hk      Convert all ERRs to MSG_ERROR to save memory
05/22/12   nb      Convert setting and resetting of gstk_proactive_cmd_in_progress
                   flag to MACRO
05/17/12   nb      Update for correct current app type in GSM case
03/28/12   av      Replaced feature flags with NVITEM
02/24/12   sg      Set custom TP values for 31 and 32 byte
12/20/11   nb      GSDI Cleanup
10/26/11   sg      Added TP handling for USAT only items
09/28/11   xz      Send AcT and Loc envelope when receive SETUP EVT LIST cmd
09/20/11   bd      Fixed compiler error
04/25/11   bd      Fixed issue of proactive commands not being fetched
                   from slot 2 after REFRESH RESET
05/11/11   nb      Support for Async TP
04/25/11   bd      Fixed issue of proactive commands not being fetched
                   after REFRESH RESET
04/20/11   ea      Changes for Enable/Disable support
01/20/10   dd      Enable FEATURE_CAT_REL7
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
10/05/10   bd      Fixed issue of TP DL
06/18/10   xz      Moving num of expected TPs to config file
05/06/10   dd      Send larger terminal profile
02/19/10   bd      Ported the fix for issue of gstk_profile_received not being updated
                   when client sends TP
02/15/10   nb      Moved Dual Slot Changes
12/29/09   rn      Fixed klocwork errors.
10/14/09   xz      Dismiss pending proactive cmd before do TP DL
09/26/09   kp      ZI memory reduction changes
05/01/09   dd      Fixing lint errors
04/20/09   yb      Implemented a mechanism to notify client when TP DL fails
03/26/09   xz      Fix the issue of TP DL failing if GSTK fails to get app_id
                   when parse client TP and simplify TP download procedure
11/24/08   sk      Determine card type as one of the first steps before
                   processing client terminal profile updates.
10/22/08   sk      Handle tp dl from multiple clients
                   Removed gstk_terminal_profile_data_from_client
09/14/08   sk      Fixed lint errors
08/06/08   xz      Add debug info of if default or updated term profile is sent
02/06/08   sk      Lint fixes
11/29/07   sk      Added support for differentiating SAT and USAT TP dl
10/23/06   sk      Fixed lint errors.
05/17/06   sk      Lint fixes
11/14/05   sk      Fixed lint errors
10/18/05   tml     Added card apps
05/11/05   sst     Lint fixes
01/07/05   jk      Fixed Compiler warnings
10/01/04   tml     Change function prototype for terminal profile caching
08/30/04   tml     Added gstk_profile_received
06/29/04   tml     Added fake imsi support for reset the new slot after mode
                   change
06/28/04   tml     Added fake imsi support
06/10/04   tml     Added terminal profile version support
05/26/04   tml     Set the terminal profile bits for Get Inkey and Setup Call
03/05/04   tml     Allowed single slot to operate individually in Dual Slot
                   solution
09/04/03   tml     dual slot support
07/07/03   tml     Removed unnecessary messages
05/16/03   tml     linted
04/21/03   tml     Added UTK support
02/13/03   tml     Added/Changed DM messages
02/07/03   tml     Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "intconv.h"
#include "gstk_term_profile.h"
#include "gstkutil.h"
#include "err.h"
#include "string.h"
#include <stringl/stringl.h>
#include "uim_msg.h"
#include "qmi_cat.h"
#include "gstk_nv.h"
#include "gstk_proactive_cmd.h"
#include "gstk_envelope_cmd.h"
#include "gstk_csg.h"
#include "uim_common_lib.h"
#include "gba_lib.h"

static gstk_status_enum_type gstk_update_base_tp(
  uint8 cat_version
);

static gstk_status_enum_type gstk_update_tp_dl_cache(
  gstk_terminal_profile_cmd_type  *term_profile_ptr
);

/*===========================================================================
FUNCTION gstk_uim_terminal_profile_report

DESCRIPTION

  This is the callback function for terminal profile response.  UIM will call
  this function when it receives terminal profile response from the Card
  Based on the return status of the terminal profile response, this function
  will build the gstk_term_profile_rsp_type and set GSTK_CMD_Q_SIG so that
  this command can be processed in the gstk state machine

PARAMETERS
  report: [Input] Pointer to the UIM terminal profile report

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
void gstk_uim_terminal_profile_report (
  uim_rpt_type *report
)
{
  gstk_task_cmd_type   * task_cmd;

  if(report == NULL)
  {
    UIM_MSG_ERR_0("report ptr ERR:NULL");
    return;
  }

  UIM_MSG_HIGH_2("GSTK recv terminal profile uim report, 0x%x, 0x%x",
                 report->sw1, report->sw2);

  if (report->rpt_type != UIM_TERMINAL_PROFILE_R) {
      // build error message to GSTK?????????????
      UIM_MSG_ERR_0("TP expected in CB");
      return;
  }


  /* get command buf */
  task_cmd = gstk_task_get_cmd_buf();
  if ( task_cmd != NULL ) { /* successfully obtained command buffer */
      /* build the terminal profile response */
    task_cmd->cmd.terminal_profile_rsp.message_header.sim_slot_id =
      (gstk_slot_id_enum_type)report->slot;
      task_cmd->cmd.terminal_profile_rsp.message_header.command_group = GSTK_TERMINAL_PROFILE_RSP;
      task_cmd->cmd.terminal_profile_rsp.message_header.command_id = (int)GSTK_PROFILE_DL_IND_RSP;
      task_cmd->cmd.terminal_profile_rsp.message_header.user_data = report->user_data;
      if (report->rpt_status == UIM_PASS) {
          task_cmd->cmd.terminal_profile_rsp.success = TRUE;
          if (GSTK_IS_VALID_SLOT_ID(report->slot)) {
             GSTK_RETURN_IF_NULL_PTR(gstk_instances_ptr[(uint8)report->slot - 1]);
             gstk_instances_ptr[(uint8)report->slot - 1]->is_tp_rsp_received = TRUE;
          }
      }
      else {
          task_cmd->cmd.terminal_profile_rsp.success = FALSE;
      }
      /* set GSTK_CMD_Q_SIG and put on GSTK queue */
      gstk_task_put_cmd_buf(task_cmd);
  }
}/*lint !e818 *report suppression for externalized function */
/* gstk_uim_terminal_profile_report */

/*===========================================================================
FUNCTION gstk_update_base_tp

DESCRIPTION
  This function updates the base terminal profile into the TP cache

PARAMETERS
  None

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
static gstk_status_enum_type gstk_update_base_tp(
  uint8 cat_version
)
{
  uint32                 modem_cap = 0;
  boolean                local_cap = FALSE;
  gstk_status_enum_type  status    = GSTK_SUCCESS;
  
  /* Definition of what ME Support */
  uint8 gstk_default_tp[] =
  {
    0x23, /* Profile Download, SMS-PP, Timer expiration */
    0x0F, /* Command result, Call control by NAA, MO short message control */
    0xE8, /* More time, Poll Int, Poll Off, Refresh */
    0x00, 
    0x10, /* Location Status */
    0x10, /* Access tech */
    0x00,
    0x07, /* Timer Management(start, stop), Timer Management(get current value), PLI (date, timezone) */
    0x80, /* PLI(Access Technology) */
    0x00,

    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    
    0x00,
    0x00,
    0x00, 
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
	
    0x00,
    0x00,
    0x00
  };  

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  /* Find the Modem Capabilities */  
  modem_cap = uim_common_get_modem_rat_capabilities();  
  
  local_cap = gstk_check_cbs_capability();
  if(local_cap)
  {
    /* Support for CBS */
    gstk_default_tp[0] |= (GSTK_CELL_BROADCAST_DATA_DL_SUPPORT & 0xFF);
  }

  status = gstk_nv_get_proactive_feature_enable(GSTK_FEATURE_ENABLE_NET_LOC, &local_cap);
  if((status == GSTK_SUCCESS) && (local_cap))
  {
    /* Support for PLI LI */
    gstk_default_tp[3] |= (GSTK_PROVIDE_LOCAL_INFO_LOCATION_INFO_SUPPORT & 0xFF);
  }

  status = gstk_nv_get_proactive_feature_enable(GSTK_FEATURE_ENABLE_EVT_LST_STATUS, &local_cap);
  if((status == GSTK_SUCCESS) && (local_cap))
  {
    /* Support for SETUP Event list */
    gstk_default_tp[4] |= (GSTK_SET_UP_EVT_LIST_SUPPORT & 0xFF);
  }

  local_cap = gstk_check_evt_dl_voice_cap();
  if(local_cap &&
     (gstk_nv_get_me_feature_status(GSTK_CFG_FEATURE_ME_DISABLE_ESTK_CM_VOICE_CALL_INTERFACE) == FALSE))
  {
    /* MT Call, Call Connected, Call Disconnected */
    gstk_default_tp[4] |= 0x0E;
  }

  if(modem_cap & UIM_MODEM_CAP_FEATURE_GSM_MASK)
  {
    gstk_default_tp[3] |= 
    (GSTK_PROVIDE_LOCAL_INFO_NMR_SUPPORT & 0xFF); /* Enable PLI-NMR */

    gstk_default_tp[8] |= 
    (GSTK_PROVIDE_LOCAL_INFO_TIME_ADV_SUPPORT & 0xFF); /*Enable PLI-Timing Advance */
  }

  if (cat_version >= GSTK_CFG_CAT_VER6)
  {
    gstk_default_tp[5] |=
      (GSTK_NETWORK_SEARCH_MODE_CHANGE & 0xFF); /* Enable NW Search mode change */

    gstk_default_tp[17] |=
      (GSTK_PROVIDE_LOCAL_INFO_NETWORK_SEARCH_MODE_SUPPORT & 0xFF);

  if((modem_cap & UIM_MODEM_CAP_FEATURE_LTE_MASK) ||
       (modem_cap & UIM_MODEM_CAP_FEATURE_GSM_MASK) || 
       (modem_cap & UIM_MODEM_CAP_FEATURE_WCDMA_MASK) ||
       (modem_cap & UIM_MODEM_CAP_FEATURE_TDSCDMA_MASK))
    {
      /*Enable IMEISV and search mode*/
      gstk_default_tp[17] |= 
        (GSTK_PROVIDE_LOCAL_INFO_IMEISV_SUPPORT & 0xFF);
    }
  }
  
  if((cat_version >= GSTK_CFG_CAT_VER4) && 
     (modem_cap & UIM_MODEM_CAP_FEATURE_1X_MASK))
  {
    gstk_default_tp[17] |= 
    	(GSTK_PROVIDE_LOCAL_INFO_ESN_SUPPORT & 0xFF); /*Enable PLI ESN */
  }  

  if(modem_cap & UIM_MODEM_CAP_FEATURE_1X_MASK)
  {    
    /* Enable CDMA SMS-PP */
    gstk_default_tp[19] |= 
     (GSTK_CDMA_SMS_PP_SUPPORT & 0xFF);

    if(gstk_check_cbs_capability() == TRUE)
    {
      /* Enable CDMA CBS*/
    gstk_default_tp[19] |= 
        (GSTK_CDMA_SMS_BROADCASE_SUPPORT & 0xFF);
    }

    /* PLI -MEID */
    gstk_default_tp[22] |= 
    (GSTK_PROVIDE_LOCAL_INFO_MEID_SUPPORT & 0xFF); 
  }

  UIM_MSG_HIGH_3("gstk_update_base_tp, slot_id: 0x%x, cat_version: 0x%x, mod_cap 0x%x",
                 gstk_curr_inst_ptr->slot_id, cat_version, modem_cap);
  
  memset(gstk_curr_inst_ptr->gstk_terminal_profile_cache,
         0x00,
         sizeof(gstk_curr_inst_ptr->gstk_terminal_profile_cache));

  gstk_memcpy(gstk_curr_inst_ptr->gstk_terminal_profile_cache,
                   gstk_default_tp,
                   sizeof(gstk_default_tp),
                   GSTK_TERMINAL_PROFILE_SIZE,
                   sizeof(gstk_default_tp));
                   
  return GSTK_SUCCESS;
} /* gstk_update_base_tp */

/*===========================================================================
FUNCTION gstk_update_default_app_specific_tp

DESCRIPTION
  This function sends app specific terminal profile that GSTK supports by
  default

PARAMETERS
  None

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
gstk_status_enum_type gstk_update_default_app_specific_tp(
  uint8 gstk_instance_idx
)
{
  gstk_status_enum_type             gstk_status               = GSTK_SUCCESS;
  gstk_profile_dl_support_ext_type *estk_profile_val_list     = NULL;
  gstk_profile_dl_support_ext_type *qmi_cat_profile_val_list  = NULL;
  gstk_profile_dl_support_ext_type *combined_profile_val_list = NULL;
  uint8                             num_of_default_tps        = 0;
  uint8                             num_of_estk_tps           = 0;
  uint8                             num_of_qmi_cat_tps        = 0;
  uint8                             num_of_total_tps          = 0;
  gstk_nv_items_data_type           nv_data;
  gstk_profile_dl_support_ext_type *selected_tp               = NULL;

  /* GSTK default app specific terminal profiles */
  gstk_profile_dl_support_ext_type default_tp[] =
  {
    {1, GSTK_9EXX_RSP_CODE_SUPPORT,               (uint8)GSTK_APP_SIM},
    {1, GSTK_CC_ALWAYS_SENT_DURING_REDIAL,        (uint8)GSTK_APP_SIM},
    {1, GSTK_USAT_SMS_PP_DL_SUPPORT_BYTE1_BIT_5,  (uint8)GSTK_APP_USIM},
    {1, GSTK_USAT_CC_SUPPORT_BYTE_1_BIT_8,        (uint8)GSTK_APP_USIM},
    {1, GSTK_PLI_NMR_UTRAN,                       (uint8)GSTK_APP_USIM},
    {1, GSTK_USAT_CC_SUPPORT_BYTE_8_BIT_8,        (uint8)GSTK_APP_USIM},
    {1, GSTK_SECOND_CAP_CONFIG_PARAM_SUPPORT,     (uint8)GSTK_APP_SIM},
    {1, GSTK_USAT_CC_SUPPORT_BYTE_2_BIT_5,        (uint8)GSTK_APP_USIM},
    {1, GSTK_HANDLING_OF_ALPHA_BYTE_2_BIT_5,      (uint8)GSTK_APP_SIM},
    {1, GSTK_TOOLKIT_INITIATED_GBA_SUPPORT,       (uint8)GSTK_APP_USIM},
    {1, GSTK_DNS_SERVER_ADRS_SUPPORT,             (uint8)GSTK_APP_USIM},
    {1, GSTK_CLOSE_CHANNEL_REUSE_SUPPORT,         (uint8)GSTK_APP_USIM},
    {1, GSTK_POLL_INTERVAL_EVT_SUPPORT,           (uint8)GSTK_APP_USIM},    
    {1, GSTK_COMMUNICATION_CONTROL_FOR_IMS_SUPPORT,(uint8)GSTK_APP_USIM},
    {1, GSTK_IMS_REGISTRATION_EVENT,              (uint8)GSTK_APP_USIM},
    {1, GSTK_STEERING_ROAMING_SUPPORT,            (uint8)GSTK_APP_USIM},
    {1, GSTK_NW_REJECTION_EUTRAN_SUPPORT,         (uint8)GSTK_APP_USIM},
    {1, GSTK_NW_REJ_GERAN_UTRAN_SUPPORT,          (uint8)GSTK_APP_USIM},
    {1, GSTK_PROVIDE_LOCAL_INFO_UTRAN_EUTRAN,     (uint8)GSTK_APP_USIM},
    {1, GSTK_GPRS_CALL_CONTROL,                   (uint8)GSTK_APP_USIM},
    {1, GSTK_USAT_CC_SUPPORT_BYTE_1_BIT_7,        (uint8)GSTK_APP_USIM},
    {1, GSTK_PLI_NMR_BCCH,                        (uint8)GSTK_APP_SIM},
    {1, GSTK_USSD_DO_IN_CC_SUPPORT,               (uint8)GSTK_APP_SIM},
    {1, GSTK_REFRESH_ENFORCEMENT_POLICY_SUPPORT,  (uint8)GSTK_APP_NONE}   
  };

  /* GSTK default app specific terminal profiles */
  gstk_profile_dl_support_ext_type default_data_card_tp[] =
  {
    {1, GSTK_9EXX_RSP_CODE_SUPPORT,                  (uint8)GSTK_APP_SIM},
    {1, GSTK_CC_ALWAYS_SENT_DURING_REDIAL,           (uint8)GSTK_APP_SIM},
    {1, GSTK_USAT_SMS_PP_DL_SUPPORT_BYTE1_BIT_5,     (uint8)GSTK_APP_USIM},
    {1, GSTK_USAT_CC_SUPPORT_BYTE_1_BIT_8,           (uint8)GSTK_APP_USIM},
    {1, GSTK_PLI_NMR_UTRAN,                          (uint8)GSTK_APP_USIM},
    {1, GSTK_USAT_CC_SUPPORT_BYTE_8_BIT_8,           (uint8)GSTK_APP_USIM},
    {1, GSTK_SECOND_CAP_CONFIG_PARAM_SUPPORT,        (uint8)GSTK_APP_SIM},  
    {1, GSTK_USAT_CC_SUPPORT_BYTE_2_BIT_5,           (uint8)GSTK_APP_USIM},
    {1, GSTK_HANDLING_OF_ALPHA_BYTE_2_BIT_5,         (uint8)GSTK_APP_SIM},
    {1, GSTK_NO_DISPLAY_SUPPORT,                     (uint8)GSTK_APP_NONE},
    {1, GSTK_NO_KEYBOARD_SUPPORT,                    (uint8)GSTK_APP_NONE},
    {1, GSTK_TOOLKIT_INITIATED_GBA_SUPPORT,          (uint8)GSTK_APP_USIM},
    {1, GSTK_DNS_SERVER_ADRS_SUPPORT,                (uint8)GSTK_APP_USIM},
    {1, GSTK_CLOSE_CHANNEL_REUSE_SUPPORT,            (uint8)GSTK_APP_USIM},
    {1, GSTK_POLL_INTERVAL_EVT_SUPPORT,              (uint8)GSTK_APP_USIM},    
    {1, GSTK_COMMUNICATION_CONTROL_FOR_IMS_SUPPORT,  (uint8)GSTK_APP_USIM},
    {1, GSTK_IMS_REGISTRATION_EVENT,                 (uint8)GSTK_APP_USIM},
    {1, GSTK_STEERING_ROAMING_SUPPORT,               (uint8)GSTK_APP_USIM},
    {1, GSTK_NW_REJECTION_EUTRAN_SUPPORT,            (uint8)GSTK_APP_USIM},
    {1, GSTK_NW_REJ_GERAN_UTRAN_SUPPORT,             (uint8)GSTK_APP_USIM},
    {1, GSTK_PROVIDE_LOCAL_INFO_UTRAN_EUTRAN,        (uint8)GSTK_APP_USIM},
    {1, GSTK_GPRS_CALL_CONTROL,                      (uint8)GSTK_APP_USIM},
    {1, GSTK_USAT_CC_SUPPORT_BYTE_1_BIT_7,           (uint8)GSTK_APP_USIM},
    {1, GSTK_PLI_NMR_BCCH,                           (uint8)GSTK_APP_SIM},
    {1, GSTK_USSD_DO_IN_CC_SUPPORT,                  (uint8)GSTK_APP_SIM},
    {1, GSTK_REFRESH_ENFORCEMENT_POLICY_SUPPORT,     (uint8)GSTK_APP_NONE}
  };

  UIM_MSG_HIGH_1("gstk_update_default_app_specific_tp(0x%x)",
                 gstk_instance_idx);

  if (!GSTK_IS_VALID_SLOT_ID(gstk_instance_idx + 1))
  {
    return GSTK_ERROR;
  }

  /* Retrieve CAT Version */
  gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                      (gstk_slot_id_enum_type)(gstk_instance_idx + 1),
                      &nv_data);

  /* Number of default GSTK TP values and total length of TP value list that
     will go to the card */
  if (gstk_nv_get_me_feature_status(GSTK_CFG_FEATURE_ME_DATA_CARD) == TRUE)
  {
    num_of_default_tps = sizeof(default_data_card_tp) / sizeof(gstk_profile_dl_support_ext_type);
    selected_tp = default_data_card_tp;
  }
  else
  {
    num_of_default_tps = sizeof(default_tp) / sizeof(gstk_profile_dl_support_ext_type);
    selected_tp = default_tp;
  }

#ifdef FEATURE_ESTK
  /* Get terminal profile value list from estk */
  (void)estk_get_terminal_profile_value_list(&estk_profile_val_list, &num_of_estk_tps);
#endif

  /* Get terminal profile value list from qmi_cat */
  qmi_cat_profile_val_list = qmi_cat_get_terminal_profile_value_list(&num_of_qmi_cat_tps);

  num_of_total_tps = num_of_default_tps + num_of_estk_tps + num_of_qmi_cat_tps;

  UIM_MSG_HIGH_3("Number of TPs: GSTK, ESTK, QMI_CAT=0x%x, 0x%x, 0x%x",
                 num_of_default_tps, num_of_estk_tps, num_of_qmi_cat_tps);

  combined_profile_val_list =
    (gstk_profile_dl_support_ext_type*) GSTK_CALLOC(num_of_total_tps * sizeof(gstk_profile_dl_support_ext_type));

  if (combined_profile_val_list == NULL)
  {
    return GSTK_ERROR;
  }

  /* Create the combined TP value list that will go to the card:
     combined TP list =  (default_data_card_tp OR default_tp) +
                         estk profile value list +            +
                         qmi cat profile value list */
  if(gstk_memcpy(combined_profile_val_list,
                 selected_tp,
                 num_of_default_tps * sizeof(gstk_profile_dl_support_ext_type),
                 num_of_total_tps * sizeof(gstk_profile_dl_support_ext_type),
                 num_of_default_tps * sizeof(gstk_profile_dl_support_ext_type)) <
     num_of_default_tps * sizeof(gstk_profile_dl_support_ext_type))
  {
    gstk_free(combined_profile_val_list);
    return GSTK_MEMORY_ERROR;
  }

  if (estk_profile_val_list)
  {
    if(gstk_memcpy((combined_profile_val_list + num_of_default_tps),
                   estk_profile_val_list,
                   num_of_estk_tps * sizeof(gstk_profile_dl_support_ext_type),
                   (num_of_total_tps - num_of_default_tps) * sizeof(gstk_profile_dl_support_ext_type),
                   num_of_estk_tps * sizeof(gstk_profile_dl_support_ext_type)) <
       (size_t)(num_of_estk_tps * sizeof(gstk_profile_dl_support_ext_type)))
    {
      gstk_free(combined_profile_val_list);
      return GSTK_MEMORY_ERROR;
    }
  }

  if (qmi_cat_profile_val_list)
  {
    if(gstk_memcpy((combined_profile_val_list + num_of_default_tps + num_of_estk_tps),
                   qmi_cat_profile_val_list,
                   num_of_qmi_cat_tps * sizeof(gstk_profile_dl_support_ext_type),
                   (num_of_total_tps - num_of_default_tps - num_of_estk_tps) * sizeof(gstk_profile_dl_support_ext_type),
                   num_of_qmi_cat_tps * sizeof(gstk_profile_dl_support_ext_type)) <
       (size_t)(num_of_qmi_cat_tps * sizeof(gstk_profile_dl_support_ext_type)))
    {
      gstk_free(combined_profile_val_list);
      return GSTK_MEMORY_ERROR;
    }
  }

  gstk_status =  gstk_update_profile_download_value_ext(
                   gstk_instances_ptr[gstk_instance_idx]->client_id,
                   0,
                   num_of_total_tps,
                   combined_profile_val_list);

  gstk_free(combined_profile_val_list);
  return gstk_status;
} /* gstk_update_default_app_specific_tp */


/*===========================================================================
FUNCTION gstk_cache_tp_info

DESCRIPTION
  This function caches the terminal profile data received from GSTK clients

PARAMETERS
  term_profile_ptr: [Input] Mobile Terminal Profile info from GSTK client

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
gstk_status_enum_type gstk_cache_tp_info(
  const gstk_cmd_type  *term_profile_ptr
)
{
  uint8   idx   = 0;
  uint32  size  = sizeof(gstk_terminal_profile_cmd_type);

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if(term_profile_ptr == NULL)
  {
    return GSTK_ERROR;
  }

  gstk_curr_inst_ptr->client_tp_ptr[idx] = GSTK_CALLOC(size);
  if (!gstk_curr_inst_ptr->client_tp_ptr[idx])
  {
    return GSTK_MEMORY_ERROR;
  }

  if(gstk_memcpy(gstk_curr_inst_ptr->client_tp_ptr[idx],
                 term_profile_ptr,
                 size,
                 size,
                 size) <
     size)
  {
    gstk_free(gstk_curr_inst_ptr->client_tp_ptr[idx]);
    gstk_curr_inst_ptr->client_tp_ptr[idx] = NULL;
    return GSTK_MEMORY_ERROR;
  }

  if(term_profile_ptr->terminal_profile_cmd.length > GSTK_MAX_TP_DATA_SIZE)
  {
    UIM_MSG_HIGH_1("term profile len 0x%x > GSTK_MAX_TP_DATA_SIZE",
                   term_profile_ptr->terminal_profile_cmd.length);
    gstk_curr_inst_ptr->client_tp_ptr[idx]->length = GSTK_MAX_TP_DATA_SIZE;
  }

  /* update the gstk_profile_received to TRUE only if TP is
     not sent by GSTK itself */
  if (!gstk_util_is_client_id_internal(
              term_profile_ptr->terminal_profile_cmd.message_header.client_id))
  {
    gstk_curr_inst_ptr->gstk_profile_received = TRUE;
  }
  return GSTK_SUCCESS;
}/* gstk_cache_tp_info */

/*===========================================================================
FUNCTION gstk_pack_tp_cmd_to_uim_with_df

DESCRIPTION

  This function puts the gstk_curr_inst_ptr->gstk_terminal_profile_cache info into uim format
  and put onto UIM's command queue

PARAMETERS
  dir  :  GSM DF or CDMA DF

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
gstk_status_enum_type gstk_pack_tp_cmd_to_uim_with_df (
  uim_df_type df,
  boolean exit_from_psm_status)
{
  gstk_status_enum_type           gstk_status      = GSTK_SUCCESS;
  uim_cmd_type                   *uim_cmd_ptr      = NULL;
  gstk_nv_items_data_type         nv_data;

  UIM_MSG_HIGH_1("Sending card specific terminal profile with df, df is 0x%x",df);

  /* check for Null ptr */
  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  gstk_curr_inst_ptr->setup_evt_list_cnt = 0;

  /* Get the current card protocol from MMGSDI */
  gstk_get_and_store_curr_card_protocol();
  /* If NV items had been updated due to non-standard NV refresh in
   the previous card power-cycle, re-read the same. Terminal Profile
   handling is the earliest that this re-read can be attempted
   in a card power-cycle for UICC cards */
  if(gstk_curr_inst_ptr->card_protocol == MMGSDI_UICC)
  {
    gstk_nv_init_slot_cache(
      GSTK_NV_INIT_CATEG_POWER_UP_INIT,
      gstk_curr_inst_ptr->slot_id);
    /* Start the VPE timer */
    if (gstk_nv_get_feature_status(
          GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
          gstk_curr_inst_ptr->slot_id) == TRUE)
    {
      (void) rex_clr_timer(&(gstk_curr_inst_ptr->gstk_loc_info_envelope_timer));
      /* VPE timer is not set if modem wakes up in psm mode. psm_status from MMGSDI gives this info */
      if(!exit_from_psm_status)
      {
        (void) rex_set_timer(&(gstk_curr_inst_ptr->gstk_loc_info_envelope_timer),
                         GSTK_LOC_INFO_ENVELOPE_WAIT_TIME);
      }
    }
  }
  /* Retrieve CAT Version */
  gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                      gstk_curr_inst_ptr->slot_id,
                      &nv_data);
  /* Update the base TP in the GSTK TP cache */
  gstk_status = gstk_update_base_tp(nv_data.cat_version);
  if(gstk_status != GSTK_SUCCESS)
  {
    return gstk_status;
  }

  if (gstk_curr_inst_ptr->client_tp_ptr[0] != NULL)
  {
    (void)gstk_update_tp_dl_cache(gstk_curr_inst_ptr->client_tp_ptr[0]);
  }
  else
  {
    UIM_MSG_ERR_0("NULL ptr");
    return GSTK_MEMORY_ERROR;
  }

  /* Send terminal response to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL)
  {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_PROFILE_F, &uim_cmd_ptr);
    if (gstk_status == GSTK_SUCCESS)
    {
      /* set user_data */
      uim_cmd_ptr->hdr.user_data = 0;
      uim_cmd_ptr->terminal_profile.df = df;

      switch(nv_data.cat_version)
      {
        case GSTK_CFG_CAT_VER99:
          uim_cmd_ptr->terminal_profile.num_bytes = 19;
          break;
        case GSTK_CFG_CAT_VER4:
        case GSTK_CFG_CAT_VER5:
          uim_cmd_ptr->terminal_profile.num_bytes = 20;
          break;
        case GSTK_CFG_CAT_VER6:
          uim_cmd_ptr->terminal_profile.num_bytes = 29;
          break;
        case GSTK_CFG_CAT_VER7:
        case GSTK_CFG_CAT_VER8:
          uim_cmd_ptr->terminal_profile.num_bytes = 30;
          break;
        case GSTK_CFG_CAT_VER9:
          uim_cmd_ptr->terminal_profile.num_bytes = 31;
          break;
        case GSTK_CFG_CAT_VER12:
          uim_cmd_ptr->terminal_profile.num_bytes = 33;
          break;
        default:
          uim_cmd_ptr->terminal_profile.num_bytes = 30;
          break;
      }
      (void)gstk_memcpy(uim_cmd_ptr->terminal_profile.data,
             gstk_curr_inst_ptr->gstk_terminal_profile_cache,
             uim_cmd_ptr->terminal_profile.num_bytes,
             sizeof(uim_cmd_ptr->terminal_profile.data),
             sizeof(gstk_curr_inst_ptr->gstk_terminal_profile_cache));

      /* For one China Telecom card, it works with 25th byte of TP set to 0 */
      if ((gstk_nv_get_feature_status(
             GSTK_CFG_FEATURE_SLOT_CUST_TP_25_BYTE,
             gstk_curr_inst_ptr->slot_id) == TRUE) &&
          (uim_cmd_ptr->terminal_profile.num_bytes > 24))
      {
        UIM_MSG_HIGH_0("Custom set 25th byte to 0x00");
        uim_cmd_ptr->terminal_profile.data[GSTK_TP_BYTE_25] = GSTK_CUST_TP_BYTE_25;
      }

      /* dump TP contents*/
      gstk_util_dump_byte_array("gstk_terminal_profile_cache",
                                gstk_curr_inst_ptr->gstk_terminal_profile_cache,
                                (uint32)(uim_cmd_ptr->terminal_profile.num_bytes));

      gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */

      if (gstk_nv_get_feature_status(
            GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
            gstk_curr_inst_ptr->slot_id) == TRUE)
      {
        gstk_curr_inst_ptr->is_tp_dl_done = FALSE;
      }
    }
    else
    {
      UIM_MSG_HIGH_0("Never here");
      gstk_util_notify_tp_dl_fail();
    }
  }
  else
  {
    gstk_util_notify_tp_dl_fail();
  }
  return gstk_status;

} /*gstk_pack_tp_cmd_to_uim_with_df*/

/*===========================================================================
FUNCTION gstk_update_tp_dl_cache

DESCRIPTION
  This function updates the terminal profile cache if the client ui
  client differentiates between sat and usat tp dl

PARAMETERS
  term_profile: [Input] Mobile Terminal Profile info from GSTK client or from
                GSTK itself (once the card type has been determined)

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
static gstk_status_enum_type gstk_update_tp_dl_cache(
  gstk_terminal_profile_cmd_type  *term_profile_ptr
)
{
  int                      i                   = 0;
  uint32                   byte_offset         = 0;
  uint8                    bit_offset          = 0;
  uint8                    bit_shift           = 0;
  uint32                   location_mask       = 0;
  gstk_status_enum_type    gstk_status         = GSTK_SUCCESS;
  gstk_app_enum_type       app_type            = GSTK_APP_NONE;
  boolean                  is_estk_bip_disable = FALSE;
  boolean                  bip_cmd_enable      = FALSE;
  gstk_nv_items_data_type  nv_data;
  uint32                   modem_cap;
  boolean                  command_enable;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if (!GSTK_IS_VALID_SLOT_ID(gstk_curr_inst_ptr->slot_id))
  {
    return GSTK_ERROR;
  }

  if(term_profile_ptr == NULL)
  {
      UIM_MSG_ERR_0("term_profile_ptr is NULL");
      return GSTK_ERROR;
  }

  /* Find the Modem Capabilities */
  modem_cap = uim_common_get_modem_rat_capabilities();  

  gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                      gstk_curr_inst_ptr->slot_id,
                      &nv_data);

  if((gstk_nv_get_me_feature_status(GSTK_CFG_FEATURE_ME_DISABLE_ESTK_BIP_INTERFACE) == TRUE))
  {
    is_estk_bip_disable = TRUE;
  }

  gstk_status = gstk_nv_get_proactive_feature_enable(GSTK_FEATURE_ENABLE_BIP,&bip_cmd_enable);

  while (i < (int)term_profile_ptr->length)
  {
      app_type      = (gstk_app_enum_type)term_profile_ptr->data[i++];
      byte_offset   = term_profile_ptr->data[i++];
      bit_offset    = term_profile_ptr->data[i++];
      location_mask = ((byte_offset << 0x0008) | bit_offset);

      if (byte_offset > 0)
      {
        byte_offset--;
      }

      /* Find the number of bits needed to be shifted in the data before bitwise and with
      ** the gstk_Terminal_profile_cache */
      bit_shift = 0;
      while (bit_offset > 0 )
      {
          bit_offset = bit_offset >> 0x01;
          bit_shift++;
      }
      bit_shift--;

      if (byte_offset < GSTK_TERMINAL_PROFILE_SIZE)
      {
        if (app_type == GSTK_APP_NONE)
        {
          switch(location_mask) {
          case GSTK_SET_UP_CALL_SUPPORT:
              term_profile_ptr->data[i] &= 0x01;
              gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
                (uint8)(term_profile_ptr->data[i] << bit_shift);
              gstk_curr_inst_ptr->gstk_terminal_profile_cache[7] |=
                uint16touint8(term_profile_ptr->data[i] << 6);
              break;
          case GSTK_GET_INKEY_SUPPORT:
              term_profile_ptr->data[i] &= 0x01;
              gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
                (uint8)(term_profile_ptr->data[i] << bit_shift);
              gstk_curr_inst_ptr->gstk_terminal_profile_cache[7] |=
                uint16touint8(term_profile_ptr->data[i] << 3);
              break;
          case GSTK_DISPLAY_TEXT_SUPPORT:
              term_profile_ptr->data[i] &= 0x01;
              gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
                (uint8)(term_profile_ptr->data[i] << bit_shift);
              gstk_curr_inst_ptr->gstk_terminal_profile_cache[8] |=
                term_profile_ptr->data[i];
              break;
          case GSTK_NUM_CHAR_SUPPORT_DOWN_ME: /* 5 bits of info */
              term_profile_ptr->data[i] &= 0x1F;
              bit_shift -= 0x04;
              gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
                (uint8)(term_profile_ptr->data[i] << bit_shift);
              break;
          case GSTK_NUM_CHAR_SUPPORT_ACROSS_ME:    /* 7 bits of info */
              term_profile_ptr->data[i] &= 0x7F;
              bit_shift -= 0x06;
              gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
                (uint8)(term_profile_ptr->data[i] << bit_shift);
              break;
          case GSTK_WIDTH_REDUCE_IN_MENU_SUPPORT: /* 3 bits of info */
              term_profile_ptr->data[i] &= 0x07;
              bit_shift -= 0x02;
              gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
                (uint8) (term_profile_ptr->data[i] << bit_shift);
              break;
          case GSTK_NUM_CH_SUPPORT: /* 3 bits of info */
              term_profile_ptr->data[i] &= 0x07;
              bit_shift -= 0x02;
              gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
                (uint8) (term_profile_ptr->data[i] << bit_shift);

              /* Check if TP needs to be disabled */
              gstk_status = gstk_nv_get_proactive_feature_enable(GSTK_FEATURE_ENABLE_BIP,&command_enable);        
              if((gstk_status == GSTK_SUCCESS) && (!command_enable))
              {
                gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] &= ~((uint8)(0x07 << bit_shift));
              }
              break;
          case GSTK_ACTIVATE_SUPPORT:
              if(gstk_update_class_l_tp())
              {
                term_profile_ptr->data[i] &= 0x01;
                gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
                  (uint8) (term_profile_ptr->data[i] << bit_shift);
              }
              break;
          case GSTK_MAX_SK_SUPPORT: /* 8 bits of info */
              term_profile_ptr->data[i] &= 0xFF;
              bit_shift -= 0x07;
              gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
                (uint8) (term_profile_ptr->data[i] << bit_shift);
              break;
          case GSTK_PROTOCOL_VERSION: /* 4 bits of info */
              term_profile_ptr->data[i] &= 0x0F;
              bit_shift -= 0x03;
              gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
                (uint8) (term_profile_ptr->data[i] << bit_shift);
              break;
          case GSTK_LAUNCH_BROWSER_SUPPORT:
              gstk_status = gstk_nv_get_proactive_feature_enable(GSTK_FEATURE_ENABLE_LAUNCH_BROWSER,&command_enable);        
              if((gstk_status == GSTK_SUCCESS) && (!command_enable))
              {
                gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] &= ~((uint8)(0x1 << bit_shift));
              }
              break;
          default: /* 1, 0 values */
              term_profile_ptr->data[i] &= 0x01;
              gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
                (uint8) (term_profile_ptr->data[i] << bit_shift);              
              break;
          }
        }
        if (app_type == GSTK_APP_USIM && gstk_curr_inst_ptr->card_protocol == MMGSDI_UICC)
        {
          switch(location_mask)
          {
            /* USAT */
            case GSTK_USAT_SMS_PP_DL_SUPPORT_BYTE1_BIT_5:
            case GSTK_PLI_NMR_UTRAN:
            case GSTK_USAT_CC_SUPPORT_BYTE_1_BIT_7:
            case GSTK_USAT_CC_SUPPORT_BYTE_2_BIT_5:
            case GSTK_USAT_CC_SUPPORT_BYTE_1_BIT_8:
            case GSTK_USAT_DISPLAY_SUPPORT_BYTE_2_BIT_8:
            case GSTK_USAT_DISP_TEXT_SUPPORT_BYTE_9_BIT_1:
            case GSTK_USAT_CC_SUPPORT_BYTE_8_BIT_8:
            case GSTK_INKEY_HELP_FOR_VARIABLE_TIMEOUT_AND_IMMEDIATE_DIGIT_RSP:
            case GSTK_INKEY_VARIABLE_TIMEOUT_SUPPORT:
            case GSTK_DISPLAY_TXT_VARIABLE_TIMEOUT_SUPPORT:
            case GSTK_DNS_SERVER_ADRS_SUPPORT:
            case GSTK_CLOSE_CHANNEL_REUSE_SUPPORT:
            case GSTK_POLL_INTERVAL_EVT_SUPPORT:
            case GSTK_STEERING_ROAMING_SUPPORT:
              term_profile_ptr->data[i] &= 0x01;
              gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
              (uint8) (term_profile_ptr->data[i] << bit_shift);
              break;
            case GSTK_COMMUNICATION_CONTROL_FOR_IMS_SUPPORT:
            case GSTK_IMS_REGISTRATION_EVENT:
              if (gstk_nv_get_me_feature_status(GSTK_CFG_FEATURE_ME_TP_IMS_ENABLE) == TRUE)
              { 
                term_profile_ptr->data[i] &= 0x01;
                gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
                    (uint8) (term_profile_ptr->data[i] << bit_shift);
              }
              break;
            case GSTK_CSG_CELL_SELECTION_SUPPORT:
              if((nv_data.cat_version >= GSTK_CFG_CAT_VER8) &&
			  	  gstk_check_csg_capability())
              {
                term_profile_ptr->data[i] &= 0x01;
                gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
                    (uint8) (term_profile_ptr->data[i] << bit_shift);
              }
              break;
            case GSTK_NW_REJECTION_EUTRAN_SUPPORT:
              if((nv_data.cat_version >= GSTK_CFG_CAT_VER8) &&
                 (modem_cap & UIM_MODEM_CAP_FEATURE_LTE_MASK))
              {
                term_profile_ptr->data[i] &= 0x01;
                gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
                    (uint8) (term_profile_ptr->data[i] << bit_shift);
              }
              break;
            case GSTK_NW_REJ_GERAN_UTRAN_SUPPORT:
              if((modem_cap & UIM_MODEM_CAP_FEATURE_GSM_MASK) ||
                 (modem_cap & UIM_MODEM_CAP_FEATURE_WCDMA_MASK) ||
                 (modem_cap & UIM_MODEM_CAP_FEATURE_TDSCDMA_MASK))
              {
                 term_profile_ptr->data[i] &= 0x01;
                 gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
                     (uint8) (term_profile_ptr->data[i] << bit_shift);
              }
              break;
            case GSTK_PROVIDE_LOCAL_INFO_UTRAN_EUTRAN:
              if((modem_cap & UIM_MODEM_CAP_FEATURE_LTE_MASK) ||
                 (modem_cap & UIM_MODEM_CAP_FEATURE_WCDMA_MASK))
              {
                 term_profile_ptr->data[i] &= 0x01;
                 gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
    			    (uint8) (term_profile_ptr->data[i] << bit_shift);
              }
              break;
            case GSTK_GPRS_CALL_CONTROL:
              if((nv_data.cat_version >= GSTK_CFG_CAT_VER5) &&
                 (modem_cap & UIM_MODEM_CAP_FEATURE_GSM_MASK))
              {
#ifdef FEATURE_GPRS_CALLCONTROL
                 term_profile_ptr->data[i] &= 0x01;
                 gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
    			    (uint8) (term_profile_ptr->data[i] << bit_shift);
#endif
              }
              break;
            case GSTK_TOOLKIT_INITIATED_GBA_SUPPORT:
              if (gba_is_supported())
              {
                 term_profile_ptr->data[i] &= 0x01;
                 gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
    			    (uint8) (term_profile_ptr->data[i] << bit_shift);
              }
              break;
            case GSTK_USAT_UTRAN_PS_EXT_PARAM_CLASS_E_SUPPORT_BYTE_22_BIT_1:
              if((bip_cmd_enable) || !(is_estk_bip_disable) || 
                 (modem_cap & UIM_MODEM_CAP_FEATURE_WCDMA_MASK) ||
                 (modem_cap & UIM_MODEM_CAP_FEATURE_TDSCDMA_MASK))
              {
                term_profile_ptr->data[i] &= 0x01;
                gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
    			    (uint8) (term_profile_ptr->data[i] << bit_shift);
              }
              break;
            case GSTK_USAT_EUTRAN_CLASS_E_SUPPORT_BYTE_17_BIT_7:
              if((bip_cmd_enable) || !(is_estk_bip_disable) ||
                 (modem_cap & UIM_MODEM_CAP_FEATURE_LTE_MASK))
              {
                term_profile_ptr->data[i] &= 0x01;
                gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
    			    (uint8) (term_profile_ptr->data[i] << bit_shift);
              }
              break;
            case GSTK_USAT_HDPA_CLASS_E_SUPPORT_BYTE_17_BIT_8:
              if((bip_cmd_enable) || !(is_estk_bip_disable) ||
                 (modem_cap & UIM_MODEM_CAP_FEATURE_HSPA_MASK))
              {
                term_profile_ptr->data[i] &= 0x01;
                gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
    			    (uint8) (term_profile_ptr->data[i] << bit_shift);
              }
              break;
            default:
              break;             
          }
        }
        else if((app_type == GSTK_APP_SIM) &&
                (gstk_curr_inst_ptr->card_protocol ==  MMGSDI_ICC))
        {
          switch(location_mask)
          {
            case GSTK_9EXX_RSP_CODE_SUPPORT:
            case GSTK_SUPPORT_FOR_ALPHA_IN_CC:
            case GSTK_CC_SUPPORT:
            case GSTK_SECOND_ALPHA_IN_SETUP_CALL_SUPPORT:
            case GSTK_CC_ALWAYS_SENT_DURING_REDIAL:
            case GSTK_DISPLAY_OF_EXTN_TEXT:
            case GSTK_SUSTAINED_DISP_TEXT_SUPPORT:
            case GSTK_SECOND_CAP_CONFIG_PARAM_SUPPORT:
              term_profile_ptr->data[i] &= 0x01;
              gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
                (uint8) (term_profile_ptr->data[i] << bit_shift);
              break;
            case GSTK_PLI_NMR_BCCH:
    	      if(modem_cap & UIM_MODEM_CAP_FEATURE_GSM_MASK)
              {
                term_profile_ptr->data[i] &= 0x01;
                gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
                  (uint8) (term_profile_ptr->data[i] << bit_shift);
    	      }
              break;
            case GSTK_USSD_DO_IN_CC_SUPPORT:
              if((gstk_nv_get_me_feature_status(GSTK_CFG_FEATURE_ME_DISABLE_ESTK_CM_SUPPS_INTERFACE) == FALSE) ||
                  (gstk_check_ussd_capability() == TRUE))
              {
                term_profile_ptr->data[i] &= 0x01;
                gstk_curr_inst_ptr->gstk_terminal_profile_cache[byte_offset] |=
                  (uint8) (term_profile_ptr->data[i] << bit_shift);
              }
            default:
              break;
          }
        }
      }
      i++;
  }
  return gstk_status;
} /* gstk_update_tp_dl_cache */

/*===========================================================================
FUNCTION gstk_pack_tp_cmd_to_uim

DESCRIPTION

  This function puts the gstk_curr_inst_ptr->gstk_terminal_profile_cache info into uim format
  and put onto UIM's command queue

PARAMETERS
  psm_status - TRUE while waking up in PSM mode
             - FALSE while not waking up in PSM mode
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
gstk_status_enum_type gstk_pack_tp_cmd_to_uim (
  boolean exit_from_psm_status
)
{
  gstk_status_enum_type           gstk_status       = GSTK_SUCCESS;
  uim_cmd_type                   *uim_cmd_ptr       = NULL;
  uint8                           i                 = 0;
  gstk_slot_id_enum_type          slot              = GSTK_SLOT_ID_MAX;
  gstk_nv_items_data_type         nv_data;

  UIM_MSG_HIGH_0("Sending card specific terminal profile");

  /* check for Null ptr */
  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  /* Cleanup any pending proactive command */
  for (i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; ++i)
  {
    slot = command_details_ref_table_ptr[i].sim_slot_id;
    if (!GSTK_IS_VALID_SLOT_ID(slot))
    {
      continue;
    }
    if (!command_details_ref_table_ptr[i].free_slot &&
        gstk_instances_ptr[(uint32)slot - 1] == gstk_curr_inst_ptr)
    {
      if (command_details_ref_table_ptr[i].command_details.type_of_command != GSTK_CMD_STK_REFRESH ||
          GSTK_SIM_REFRESH_RESET == command_details_ref_table_ptr[i].command_details.qualifier)
      {
        UIM_MSG_HIGH_1("Dismiss pending proactive cmd (ref_id = 0x%x)!", i);
        gstk_util_cmd_details_ref_table_free_slot(i);

        /* Enable GSTK to allow fetch, in case there was a proactive command
           in progress except all types of REFRESH except REFRESH RESET */
        GSTK_SET_PROACTIVE_CMD_IN_PROGRESS_FLAG(
          gstk_curr_inst_ptr->gstk_proactive_cmd_in_progress, FALSE);
        if(gstk_shared_data.ui_nw_proactive_session_slot_in_progress == gstk_curr_inst_ptr->slot_id)
        {
          gstk_shared_data.ui_nw_proactive_session_slot_in_progress = GSTK_SLOT_ID_MAX;
        }
      }
    }
  }
  gstk_curr_inst_ptr->setup_evt_list_cnt = 0;

  /* Get the current card protocol from MMGSDI */
  gstk_get_and_store_curr_card_protocol();

  /* If NV items had been updated due to non-standard NV refresh in
     the previous card power-cycle, re-read the same. Terminal Profile
     handling is the earliest that this re-read can be attempted
     in a card power-cycle for UICC cards */
  if(gstk_curr_inst_ptr->card_protocol == MMGSDI_UICC)
  {
    gstk_nv_init_slot_cache(
      GSTK_NV_INIT_CATEG_POWER_UP_INIT,
      gstk_curr_inst_ptr->slot_id);

    /* Start the VPE timer */
    if (gstk_nv_get_feature_status(
        GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
        gstk_curr_inst_ptr->slot_id) == TRUE)
    {
      (void) rex_clr_timer(&(gstk_curr_inst_ptr->gstk_loc_info_envelope_timer));
      /* VPE timer is not set if modem wakes up in psm mode. psm_status from MMGSDI gives this info */
      if(!exit_from_psm_status)
      {
        (void) rex_set_timer(&(gstk_curr_inst_ptr->gstk_loc_info_envelope_timer),
                             GSTK_LOC_INFO_ENVELOPE_WAIT_TIME);
      }    
    }
  }

  /* Retrieve CAT Version */
  gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                      gstk_curr_inst_ptr->slot_id,
                      &nv_data);

  /* Update the base TP in the GSTK TP cache */
  gstk_status = gstk_update_base_tp(nv_data.cat_version);

  if(gstk_status != GSTK_SUCCESS)
  {
    return gstk_status;
  }

  /* update all cached TPs from clients to gstk_terminal_profile_cache */
  if (gstk_curr_inst_ptr->client_tp_ptr[0] != NULL)
  {
    (void)gstk_update_tp_dl_cache(gstk_curr_inst_ptr->client_tp_ptr[0]);
  }
  else
  {
    UIM_MSG_ERR_0("NULL ptr");
    return GSTK_MEMORY_ERROR;
  }

  /* Send terminal response to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_PROFILE_F, &uim_cmd_ptr);
  if (uim_cmd_ptr != NULL)
  {
    if (gstk_status == GSTK_SUCCESS)
    {
      /* set user_data */
      uim_cmd_ptr->hdr.user_data = 0;

      switch(nv_data.cat_version)
      {
        case GSTK_CFG_CAT_VER99:
          uim_cmd_ptr->terminal_profile.num_bytes = 19;
          break;
        case GSTK_CFG_CAT_VER4:
        case GSTK_CFG_CAT_VER5:
          uim_cmd_ptr->terminal_profile.num_bytes = 20;
          break;
        case GSTK_CFG_CAT_VER6:
          uim_cmd_ptr->terminal_profile.num_bytes = 29;
          break;
        case GSTK_CFG_CAT_VER7:
        case GSTK_CFG_CAT_VER8:
          uim_cmd_ptr->terminal_profile.num_bytes = 30;
          break;
        case GSTK_CFG_CAT_VER9:
          uim_cmd_ptr->terminal_profile.num_bytes = 31;
          break;
        case GSTK_CFG_CAT_VER12:
          uim_cmd_ptr->terminal_profile.num_bytes = 33;
          break;
        default:
          uim_cmd_ptr->terminal_profile.num_bytes = 30;
          break;
      }

      (void)memscpy(uim_cmd_ptr->terminal_profile.data,
             sizeof(uim_cmd_ptr->terminal_profile.data),
             gstk_curr_inst_ptr->gstk_terminal_profile_cache,
             uim_cmd_ptr->terminal_profile.num_bytes);

      /* For one China Telecom card, it works with 25th byte of TP set to 0 */
      if ((gstk_nv_get_feature_status(
             GSTK_CFG_FEATURE_SLOT_CUST_TP_25_BYTE,
             gstk_curr_inst_ptr->slot_id) == TRUE) &&
          (uim_cmd_ptr->terminal_profile.num_bytes > 24))
      {
        UIM_MSG_HIGH_0("Custom set 25th byte to 0x00");
        uim_cmd_ptr->terminal_profile.data[GSTK_TP_BYTE_25] = GSTK_CUST_TP_BYTE_25;
      }

      /* dump TP contents*/
      gstk_util_dump_byte_array("gstk_terminal_profile_cache",
                                gstk_curr_inst_ptr->gstk_terminal_profile_cache,
                                (uint32)(uim_cmd_ptr->terminal_profile.num_bytes));

      gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */

      if (gstk_nv_get_feature_status(
            GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
            gstk_curr_inst_ptr->slot_id) == TRUE)
      {
        gstk_curr_inst_ptr->is_tp_dl_done = FALSE;
      }
    }
    else
    {
      UIM_MSG_HIGH_0("Never here");
      gstk_util_notify_tp_dl_fail();
    }
  }
  else
  {
    gstk_util_notify_tp_dl_fail();
  }
  return gstk_status;

} /*gstk_pack_tp_cmd_to_uim*/
