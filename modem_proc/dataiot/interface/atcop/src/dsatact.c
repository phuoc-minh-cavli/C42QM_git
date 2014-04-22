/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                ( C O M M O N   A C T I O N  
                  C O M M A N D S )

                P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes the AT commands. It mainly executes the common 
  (across all modes) action commands.

EXTERNALIZED FUNCTIONS
  dsatact_exec_amps_cmd
    This function takes the result from the command line parser
    and executes it. This function executes configuration (AT&S)
    command. AT&S is used to manage the DSR control. 

  dsatact_exec_ampf_cmd
    This function takes the result from the command line parser
    and executes it. This function executes configuration (AT&F)
    command. AT&F is used to set to factory defined configuration. 

  dsatact_exec_atz_cmd
    This function takes the result from the command line parser
    and executes it. This function executes configuration (ATZ) 
    command. ATZ is used to reset to default configuration.

  dsatact_exec_ampw_cmd
    This function takes the result from the command line parser
    and executes it. It executes at&w command.
    AT&W is used to store S-registers and V.250 registers into NV.

  dsatact_exec_slashs_cmd
    This function takes the result from the command line parser
    and executes it. It executes at\s command.
    AT\S is used to print AT command settings.

  dsatact_exec_slash_q_cmd
    This function takes the result from the command line parser
    and executes it. This function executes AT\Q command.
    AT\Q is used to set serial port flow control.

  dsatact_exec_atd_cmd
    This function takes the result from the command line parser
    and executes it. It executes ATD command.
    ATD command is used to dial a call (ASYNC, PACKET, VOICE) using AT 
    commands

  dsatact_exec_ata_cmd
    This function takes the result from the command line parser
    and executes it. It executes ATA command.
    ATA command is used to answer a incoming call using AT commands

  dsatact_exec_ath_cmd
    This function takes the result from the command line parser
    and executes it. It executes ATH command.
    ATH command is used to hangupall using AT commands.
    Will not end a voice call.

  dsatact_exec_gmi_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CGMI/+GMI command.
    This command returns manufacturer information.

  dsatact_exec_gmm_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CGMM/+GMM command.
    This command returns model information.

  dsatact_exec_gmr_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CGMR/+GMR command.
    This command returns revision information.

  dsatact_exec_gsn_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CGSN command.
    This command returns serial number information.

  dsatact_exec_gcap_cmd
    This function takes the result from the command line parser
    and executes it. It executes +CGSN command.
    This command returns complete capabilities list information.

  dsatact_exec_ws46_cmd
    This function takes the result from the command line parser
    and executes it. It executes +WS46 command.
    This command manages wireless data service (WDS) side-stack selection.

  dsatact_call_abort_handler
    This function does nothing but return a result to indicate the
    call should be aborted.

  dsatact_exec_ato_cmd
    This function takes the result from the command line parser
    and executes it. It executes ATO command.
    ATO command is used to return from command to online data mode.
    
  dsatact_exec_ati_cmd
    This function takes the result from the command line parser
    and executes it. It executes I command.
    This command returns identification information.

  dsatact_exec_ate_cmd
    This function takes the result from the command line parser
    and executes it. It executes E command.
    This command controls echoing of the command line to TE.

  dsat_get_imei
    This function queries the IMEI value in NV and
    decodes it into an ASCII string.  The result is
    sent back to the caller in the passed buffer pointer.

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001 - 2019 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/interface/atcop/src/dsatact.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/19   sp      Added changes to fix WS46 issue.
01/02/19   skc     Changes made to save WS46 value properly.
05/01/17   skc     Featurizing PBM Code.
05/01/17   skc     Fixed escape sequence detection handling for
                   multi-port configuration.
04/17/17   skc     Enabled requested AT Commands for IOE Build.
01/19/17   skc     Added enhancement on CHLD and CLCC for VoLTE MPTY Call.
09/30/16   skc     Fixed issue related to ATZ.
11/24/14   pg      Fixed ws46 featurization issue.
07/31/14   pg      Fixed Async data call on 3gpp2.
07/29/14   sc      Added support for NV Refresh.
06/27/14   tk      Added support for Dynamic ATCoP.
03/14/14   tk/sc   Optimized debug macros usage in ATCoP.
10/19/13   tk      Fixed issues in +CMUX command.
09/13/13   sc      Fixed KW errors.
05/08/13   tk      Renamed 3GPP2 +CMUX command to +QCMUX command.
02/22/13   tk      ATCoP - CM interface changes for Triton DSDA.
01/17/13   tk      Fixed KW errors.
01/04/13   tk      ATCoP changes for DSDX C/G+G with True World Mode Support.
11/30/12   tk      ATCoP changes for Triton DSDA.
08/10/12   sk      Fixed CLCC issue when PS call state is not valid.
08/22/12   sk      Mainlining Nikel Changes.
06/12/12   tk      Fixed +CGMM issue.
05/18/12   tk      Migrated to MSG 2.0 macros
03/29/12   sk      Fixed errors & warnings when FEATURE_DSAT_EXTENDED_CMD is enabled.
03/26/12   sk      Removed check for emergency numbers.
03/07/12   sk      Made CLCC common for all modes.
05/03/12   sk      Dun call failure fix on LTE.
02/17/12   sb      Fixed Compiler and KW Warnings.
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
01/19/12   sk      Feature cleanup.
01/13/12   sa      Added support for CSVT for CSFB targets.
11/03/11   sb      Fixed KW Warnings.
10/25/11   nc      Adopted prototype change for the usage of 'ds_cnedsd_if_get_preferred_sys'.
09/02/11   ad      Having SYS mode check only for Data call.
05/26/11   dvk     Global Variables Cleanup
04/27/11   bs      Added NIKEL Phase I Support
03/04/11   ad      Reverted RIM changes for this file .
02/17/11   nc      Ignoring ending Waiting calls for ATH when CVHU is 1.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
12/24/10   nc      Ignoring CM Mode check for ATD for DSDS.
10/25/10   ad      Init Changes for DSDS. 
09/28/10   ad      Added LTE support in +WS46.
08/22/10   vk      Using DSD for command mode check for Fusion
06/29/10   kk      Removing bbver.h dependency for BOOT_BLOCK_VERSION.
05/31/10   ad      Added support for S7 command.
05/10/10   kk      Fixes for CT-specific formatting in ATI.
01/06/10   ua      Extending CGMI, CGMM, CGMR for GOBI in CDMA mode. 
09/22/09   vrk	   Merged LTE changes.
06/11/09   nc      Modified the waiting call end using ATH to 
                   reflect the correct cause code.
06/14/09   nc      Fixed the border case of PDP activation for cid equals 16.
06/29/09   vg	   Replacing MSG_XXX with MSG_SPRINTF_X.
04/20/09   ua      Fixed lint errors.
01/30/09   ua      Added support of CGEV for DUN calls.
12/19/08   sa      Added check to dis-allow ATD call if there is mode mismatch
                   in case of Multi-mode target.
11/24/08   ss      Off target Lint fix.
10/23/08   bs      Added support for 1X AT Phonebook commands.
04/16/08   sa      Allow voice call in ETSI mode when a data call is present.
02/18/08   sa      Added modification for Passport Feature support.
12/20/07   sa      Fixed feature wrap issues.
11/24/07   ua      Fixed Lint-lows. 
10/26/07   ua      Added support for AT&S command. 
10/09/07   ss      Correcting the behaviour for GSM+1x targets.
09/27/07   sa      Added support for concurrent registration for CS data
                   and MT PDP handlers.
09/18/07   ss      Added support for ATDI and ATDL commands.
09/13/07   sa      Feature wrapped S2 register in FEATURE_DATA_S2_REG.
08/22/07   sa      Added new register function dsat_register_pdp_handlers() and  
                   modified existing dsat_register_handlers(),ATA and ATH command
                   handling functions for MT PDP support. 
08/06/07   pp      Added support for AT\Q command.
02/07/07   pp      New commands: AT&W, AT\S. Also, modified ATZ command.
06/20/07   ua      Fixes lint errors. 
06/12/07   ss      Fixes for +WS46 read on power up.
05/30/07   ua      Correcting +WS46 report result.
05/09/07   az      Changed FEATURE_JCDMA_2 to FEATURE_JCDMA_1X
05/02/07   ua      Fixed lint medium errors. 
04/11/07   ua      Modifications as per KDDI requirements. 
01/30/07   ss      Replaced banned string API calls
10/11/06   rsj     Modified dsati_dial_table for JCDMA_2 - ignore async circuit
                   origination.
08/03/06   ua      Adding 911 to the list of Emergency numbers
02/21/06   sa      Merged from Sirius Branch.
                   -Use global strings for manufacturer and model
                   -Added +CLIR support                                 
01/17/06   snb     Correct direct dial to accept quoted strings.
11/11/05   ar      Accomodate interface change to dsatvoice_is_voice_call().
10/14/05   ar      Set the SIOLOB modem port on ATA.
11/04/05   snb     Correct ATH command handler's setting of 
                   etsicall_cmd_pending.
11/03/05   ar      Make +WS46 responses consistent with spec CR, Lint fixes.
09/12/05   snb     Make get_imei() external function dsat_get_imei().
05/25/05   gr      Unfeaturized the ATA command handling so that it also 
                   works for CDMA 
04/21/05   snb     If CDMA-only build and no hangup CB registered ATH should
                   not end voice call(s).
04/13/05   snb     Add +DS to +GCAP response in WCDMA mode, correct +GCAP
                   response in Sirius builds to include +CGSM instead of 
                   +CGSMS.
04/05/05   ar      Add +CIND support for ATD command.
03/31/05   sb      Fixes from testing the serializer changes.
03/30/05   tkk     Featurized ATB command.
03/15/05   sb      Fixes from testing the serializer changes.
03/03/05   tkk     Added interdependency information for ATB command.
03/02/05   tkk     Added support for ATB command.
02/18/05   dvp     Removed the port check when ATD is executed.
02/18/05   dvp     Changed +CGMM command response to SXG75.
02/15/05   snb     Fix CDMA-only build issues.
02/14/05   dvp     Removed compilation warning.
02/10/05   sb      Accessories related changes.
01/17/05   sb      Merge from development branch
01/11/05   ar      Add support for +WS46 command.
01/05/05   pdv     Correcting +CGMM response.
12/15/04   snb     Allow ATH to hangup voice calls under +CVHU control.
12/29/04   snb     Correct behavior when packet dial strings contains
                   multiple CIDs.
10/22/04   ar      Stop timers on reset commands.  Lint cleanup.
10/21/04   snb     Correct behavior when packet dial strings contains
                   multiple CIDs.
09/13/04   snb     Fix 6500 build warning, add ATE command handler to allow
                   echo enable as default.
09/03/04   snb     Add +CHLD support.
04/20/04   snb     Reset PDP context info and PDP call setup delay tolerance 
                   on AT&F only if FEATURE_DATA_AMPF_RESET defined.
02/24/04   snb     Allow ATZ command to terminate call when its callback in 
                   dial_handler is not defined.
10/31/03   rsl     Increased info array size in +gcap processing. Fixed
                   bug where cap_list_2000 was overwriting +GCAP:
10/29/03   snb     Added support for $QCPINSTAT command and for async IMSI 
                   read from either card slot by +CIMI command.
09/02/03   ar      Added further validation to IMSI data processing.
08/24/03   sb      Range check for optional argument in ATI command
08/22/03   sb      Fixed compile issue
08/21/03   sb      Removed : AT&F should behave exactly like AT&Z.
08/20/03   ar      Adjust I cmd handler to accept but ignore arguments.
08/13/03   snb     Added support for $QCPDPLT command and for reading IMSI
                   from UIM card
07/31/03   ar      Added dial string dependence on +CSTA value.
06/27/03   snb     Only init LD record on successful call origination.
06/10/03   ar      Change interface to proc_dial_string.  Relocate packet 
                   call detection.
06/09/03   ar      Adjust dial string modifier test for CLIR.
05/01/03   sb      new return code (DSAT_ATH_CMD) when we are processing 
                   ATH command. Mainly to indicate that it is a delayed 
                   result code (from DSMGR) and we need to process the 
                   command line after we are doen executing ATH command.
04/23/03   dgy     Merged GSM and WCDMA CS Data mode-specific handlers.
04/18/03   vsk     changed to use the dsumts_rmsm interface rather than 
                   gen_rmsm 
03/12/03   ar      Scan entire dial string for I/G modifiers
02/26/03   wx      Put dialed number into last dial ph book.
02/25/03   rsl     Fixed typo., use mode instead of current_mode.
02/25/03   rsl     Execute atd only when in AT_CMD mode, otherwise return ERROR.
02/14/03   wx      Add ABORTING state to the abortable command to 
                   handle the aborting in async command processing.
02/03/03   ar      Add +ES in +GCAP list in WCDMA mode.
01/24/03   ar      Add dial string I/G/CID modifier support.
01/22/03   ar      Changed +GCAP to display +FCLASS only in GSM mode.
01/16/03   ar      AT&F now calls dsatetsipkt_reset_pdp_context_profiles()
11/22/02   sb      ATH Should not end Voice call.
11/14/02   jd      Simplified/optimized strcmp_pkt_dial_str, cleanup
11/11/02   sb      Restricted +GCAP command based on the current mode.
11/08/02   sb      Concurrent call changes.
11/08/02   sb      Return NO_CARRIER when Answer Call Back Handler is NULL.
11/07/02   ar      Incorporated code review comments
11/07/02   ar      Added FEATURE_GSM_GPRS to FEATURE_DATA_WCDMA_PS wrappers
10/29/02   sb      Fixed a merge issue that occured due to Voice changes
10/28/02   sb      Opened Voice call related handlers (moved changes to APIs)
10/23/02   ar      Added FEATURE_DATA_MM wrapper around packet rmsm items 
10/09/02   sb      Populated the dial table (GSM CS, WCDMA CS, WCDMA PS )
09/18/02   ar      Added/adjusted FEATURE_DATA_WCDMA_PS wrapper for GSM build
09/10/02   ak      Compilation for 1x changes.
09/10/02   atp     Added 707 async & pkt dial cb functions to dial table. 
09/10/02   ar      Wrap GSDI interface change under FEATURE_MMGSDI_<mode>
09/09/02   ar      Move dial character validation to utility function
                   dsatutil_validate_dial_char().
09/06/02   sb      Fix atd_cmd handler bugs.
09/05/02   sb      ATCOP changes based on new Multi mode Data services design
07/22/02   ak      made Proc_dial_string non-LOCAL
06/05/02   ar      Relocated dsatact_enter_online_data_mode from DS3GMGR
04/17/02   rc      Added stack_type as parameter to function call
                   ds3g_initiate_call().
08/27/01   sb      ATCOP code is now partitioned into multiple VUs. As a 
                   result of the partitioning, some of the files have to be
                   renamed. So making the file/interface name changes.
08/10/01   sjd     Initial release to MSM5200 archives.
06/01/01   sb      Created module.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>


#include <stdio.h>
#include <string.h>

#include "dsati.h"
#include "dsatact.h"
#include "dsatctab.h"
#include "dsatparm.h"
#include "dsatvoice.h"
#include "dsatcmif.h"
#include "ds_cap.h" /* ITC_SPEECH */
#include "dstask.h"
#include "dstaski.h"
#include "err.h"
#include "mobile.h"
#include "msg.h"
#include "ds3gsiolib_ex.h"

#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsime.h"
#include "dsatetsicall.h"
#include "dsatetsicmif.h"
#include "dsatetsictab.h"

#ifdef FEATURE_DSAT_ETSI_DATA
#include "dsatetsipkt.h"
#endif /* FEATURE_DSAT_ETSI_DATA*/
#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_DATA_MUX
#include "ds_mux_ext_defs.h"
#endif /* FEATURE_DATA_MUX */

#include "ds_rmsmi.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
/*-------------------------------------------------------------------------
            Constants:
-------------------------------------------------------------------------*/

#ifdef FEATURE_DSAT_ETSI_MODE
/*-------------------------------------
  Dial string for ETSI Emergency calls.
---------------------------------------*/
/* Allowing both 112 and 911 as Emergency numbers */
LOCAL const byte dsat_emergency_call_str_1[] = "112";
LOCAL const byte dsat_emergency_call_str_2[] = "911";

#endif /* FEATURE_DSAT_ETSI_MODE */

/*-------------------------------------
  Local Variables
---------------------------------------*/
/* ATCOP and PS split up support */

/* ATD Dial Table */
/* This table has different call handlers in different mode        */
/* Row is for different mode ( GSM, WCDMA, CDMA in that order)     */
/* Column is for different calls ( Circuit, Packet in that order ) */
/* Example:                                                        */
/*  { gsm_ckt_f,   gsm_pkt_f   },                                  */
/*  { wcdma_ckt_f, wcdma_pkt_f },                                  */
/*  { cdma_ckt_f,  cdma_pkt_f  }                                   */
/*                                                                 */ 
/* gsm_ckt_f will have the following prototype                     */
/* dsat_call_abort_cb_type gsm_ckt_f                               */
/* (                                                               */
/*  const byte            *dial_string,                            */
/*  boolean                digit_mode                              */
/* );                                                              */

dsat_dial_type dsati_dial_table[DSAT_MODE_MAX]
= 
{
    /* GSM Packet Call handler   */
    #ifdef FEATURE_GSM_GPRS
    ds_rmsm_dial_str_cb,
    #else
    NULL,  
    #endif /* (FEATURE_GSM_GPRS) */
    /* LTE Packet call handler */
    #ifdef FEATURE_DATA_LTE
    ds_rmsm_dial_str_cb
    #else
    NULL
    #endif
};

/*-------------------------------------
  Regional Variables
---------------------------------------*/
/* indicates the current abort handler */
dsat_call_abort_cb_type dsat_curr_abort_handler = NULL;
#ifdef FEATURE_MDM_FUSION
/* indicates current Rm-Sm proxy abort handler */
dsat_proxy_call_abort_cb_type dsat_proxy_curr_abort_handler = NULL;
#endif /* FEATURE_MDM_FUSION */

#ifdef FEATURE_DSAT_ETSI_MODE
extern const def_list_type dsat_ws46_list[];
#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_DSAT_GOBI_MAINLINE
#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
#if defined (FEATURE_OTASP)
/*----------------------------------
  Dial string for OTASP calls
------------------------------------*/
LOCAL const byte dsat_otasp_call_str[]="*22899";
extern cm_call_id_type   voice_call_id;
#endif/*defined (FEATURE_OTASP)*/
#endif /*defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)*/
#endif /*FEATURE_DSAT_GOBI_MAINLINE*/

#ifdef FEATURE_DATA_MUX
static ds_mux_cmux_info_type dsat_cmux_info ;
static ds3g_siolib_port_e_type dsat_curr_cmux_port = DS3G_SIOLIB_PORT_MAX ;

/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/

LOCAL void dsatact_sio_flush_tx_cb(void);

LOCAL void dsatact_cmux_notify_cb
(
  int port_state, 
  int port_operation_state
);

LOCAL void dsatact_rdm_notify_cb
(
  rdm_assign_status_type status,
  rdm_service_enum_type service,
  rdm_device_enum_type device
);
#endif /* FEATURE_DATA_MUX */

LOCAL void enter_online_data_mode
(
  boolean flush_tx,               /* Flag to flush SIO TX fifo prior to switch */
  ds3g_siolib_port_e_type port_id /* Associated port */
);

LOCAL dsat_result_enum_type process_amps_cmd
(
  void
);
LOCAL boolean proc_dial_str
(
  /* cmd token containing the atd argument       */
  const tokens_struct_type  *   tok_ptr,    

  /* dial string contain the parsed information
     This is output                              */
  dsati_dial_str_type       *   dial_ptr,

  /* current system operating mode               */
  dsati_mode_e_type         *   cur_mode_ptr
);

#ifdef FEATURE_DSAT_GOBI_MAINLINE
#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
#if defined (FEATURE_OTASP)
LOCAL dsat_result_enum_type dsatact_exec_atd_otasp_call
(
  dsati_dial_str_type* dial_str_ptr
);
#endif/*defined (FEATURE_OTASP)*/
#endif /*defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)*/
#endif /*FEATURE_DSAT_GOBI_MAINLINE*/

/*-------------------------------------------------------------------------
            Function Definitions:
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION DSATACT_EXEC_AMPS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT&S command. 
  AT&S is used to set the management of  DSR control.
  
DEPENDENCIES
  NONE

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  If the DUN fails, check the value of &C command. This has been verified
  on UART and USB which worked with &C set to 1 and 2. If different DUN 
  application is used and the call doesn't go through then try different 
  options of &C command. 

===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatact_exec_amps_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr       /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  /*------------------------------------------------
              Standard Processing 
  --------------------------------------------------*/
  if ( DSAT_OK != dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  ))
  {
    return DSAT_ERROR;
  }
  result = process_amps_cmd();

  return result;
}/* dsatact_exec_amps_cmd */

/*===========================================================================

FUNCTION DSATACT_EXEC_AMPF_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT&F command. 
  AT&F is used to set to factory defined configuration. 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Sets parameter value for "SET"  command.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ampf_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
  dsat_num_item_type temp_ipr_val;
  
  result = dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  );

  if ( result == DSAT_OK )
  {
    /* Initialize all AT parameters to their default values */
    dsatcmdp_init_config ();
    temp_ipr_val = (dsat_num_item_type)dsatutil_get_val(DSAT_EXT_IPR_IDX,0,NUM_TYPE);
    (void) ds3g_siolib_change_baud_rate((sio_bitrate_type)temp_ipr_val , FALSE );

  }
  else 
  {
    result = DSAT_ERROR;
  }
  return result;
}/* dsatact_exec_ampf_cmd */

/*===========================================================================

FUNCTION DSATACT_EXEC_ATZ_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes ATZ, command. 
  ATZ is used to reset to default configuration. 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Sets parameter value for "SET"  command.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_atz_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_num_item_type retained_ipr_val;
  dsat_result_enum_type result = DSAT_OK;
#ifdef FEATURE_DS_IS707A_CMUX
  word  fwd_mux;  /* Forward multiplex option */
  uint8 temp_val;
#endif /* FEATURE_DS_IS707A_CMUX */

  result = dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  );

  if ( result == DSAT_OK )
  {
    if ( mode != DSAT_ONLINE_DATA )
    {
      nv_stat_enum_type    nv_status;
      /* Data Port's Baud Rate should not be changed in ATZ command */
      /* Retain the IPR value and restore it later for ATZ command  */
      retained_ipr_val = (dsat_num_item_type)dsatutil_get_val(
                                        DSAT_EXT_IPR_IDX,0,NUM_TYPE);

      /* Initialize all AT parameters to their default values */
      dsatcmdp_init_config ();

      DSATUTIL_SET_VAL(DSAT_EXT_IPR_IDX,0,0,retained_ipr_val,NUM_TYPE)
      /* Restore S registers & V.250 registers setting from NV */

      /*-----------------------------------------------------------------------
      Read the stored V.250 registers from the NV.
      -----------------------------------------------------------------------*/
      nv_status = dsatutil_get_nv_item(NV_DS_AT_V250_REGISTERS_I, &ds_nv_item);

      if( nv_status == NV_DONE_S)
      {
//TODO: Sanjaykc
#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
        /*each modem port uses 1 bit of the e_reg_val, modem port 1 uses bit 0, modem port 2 uses bit 1*/
        DSATUTIL_SET_VAL(DSAT_BASIC_E_IDX,0,0,ds_nv_item.ds_at_v250_registers.e_reg_val &0x01,NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT_BASIC_E_IDX,1,0,ds_nv_item.ds_at_v250_registers.e_reg_val &0x02) >> 1,NUM_TYPE)
#else
        DSATUTIL_SET_VAL(DSAT_BASIC_E_IDX,0,0,ds_nv_item.ds_at_v250_registers.e_reg_val,NUM_TYPE)
#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/
        DSATUTIL_SET_VAL(DSAT_BASIC_Q_IDX,0,0,ds_nv_item.ds_at_v250_registers.q_reg_val,NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT_BASIC_V_IDX,0,0,ds_nv_item.ds_at_v250_registers.v_reg_val,NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT_BASIC_X_IDX,0,0,ds_nv_item.ds_at_v250_registers.x_reg_val,NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT_BASIC_N_C_IDX,0,0,ds_nv_item.ds_at_v250_registers.amp_c_reg_val,NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT_BASIC_N_D_IDX,0,0,ds_nv_item.ds_at_v250_registers.amp_d_reg_val,NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT_BASIC_N_S_IDX,0,0,ds_nv_item.ds_at_v250_registers.amp_s_reg_val,NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT_EXT_IFC_IDX,0,0,ds_nv_item.ds_at_v250_registers.ifc_reg_val[0],NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT_EXT_IFC_IDX,1,0,ds_nv_item.ds_at_v250_registers.ifc_reg_val[1],NUM_TYPE)
      }

      /*-----------------------------------------------------------------------
      Read the stored S registers from the NV.
      -----------------------------------------------------------------------*/
      nv_status = dsatutil_get_nv_item(NV_DS_AT_S_REGISTERS_I, &ds_nv_item);
  
      if( nv_status == NV_DONE_S)
      {
        DSATUTIL_SET_VAL(DSAT_SREG_S0_IDX,0,0,ds_nv_item.ds_at_s_registers.s0_reg_val,NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT_SREG_S7_IDX,0,0,ds_nv_item.ds_at_s_registers.s7_reg_val,NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT_SREG_S10_IDX,0,0,ds_nv_item.ds_at_s_registers.s10_reg_val,NUM_TYPE)

      }
      /* AT&S is restored. Process &S value */
      if ( DSAT_OK != process_amps_cmd())
      {
        return DSAT_ERROR;
      }

#ifdef FEATURE_DS_IS707A_CMUX
      /* Get the forward multiplex option */
      ds707_pkt_mgr_get_1x_profile_val(DS707_CMUX_VAL,(void *)&temp_val,1);
      fwd_mux = dsat707_cmux_fwd_table[(dsat_num_item_type)temp_val];

      /* Set the maximum multiplex option for service option 22 */
      if( fwd_mux & 0x01 )
      {
        /* Set the maximum number of forward supplementals as per the
           AT+QCMUX default */
        (void)snm_set_max_for_sups( CAI_SO_MDR_PKT_DATA_FRS1_RRS1,
                            (byte)((fwd_mux-1)/2) );
      }
      else
      {
        /* Set the maximum number of forward supplementals as per the
                     CAI default */
        (void)snm_set_max_for_sups( CAI_SO_MDR_PKT_DATA_FRS1_RRS1,
                            (byte)((CAI_MAX_USABLE_MUX_RS1-1)/2) );
      }

      /* Set the maximum multiplex option for service option 25 */
      if( !(fwd_mux & 0x01) )
      {
        /* Set the maximum number of forward supplementals as per the
           AT+QCMUX default */
        (void)snm_set_max_for_sups( CAI_SO_MDR_PKT_DATA_FRS2_RRS2,
                            (byte)((fwd_mux-2)/2) );
      }
      else
      {
        /* Set the maximum number of forward supplementals as per the
                     CAI default */
        (void)snm_set_max_for_sups( CAI_SO_MDR_PKT_DATA_FRS2_RRS2,
                            (byte)((CAI_MAX_USABLE_MUX_RS2-2)/2) );
      }
#endif  /* FEATURE_DS_IS707A_CMUX */

    }

    /* If ATZ handler available use it, 
       assuming here that it will correctly terminate call */
    if (  dial_string_handler.atz_cb != NULL )
    {
      dial_string_handler.atz_cb();
    }    
    /* Else rely on hangup handler */
    else if ( dial_string_handler.hangup_cb != NULL )
    {
      dial_string_handler.hangup_cb();
      result = DSAT_ATH_CMD;
    }
  }
  else 
  {
    result = DSAT_ERROR;
  }

  return result;

}/* dsatact_exec_atz_cmd */

/*===========================================================================

FUNCTION DSATACT_EXEC_ATD_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ATD command.
  ATD command is used to dial a call (ASYNC, PACKET, VOICE, Direct Dial) 
  using AT commands

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR          : if there was any problem in executing the command
    DSAT_NO_RSP         : if ds3g_initiate_call() is called.
    DSAT_ASYNC_CMD      : if voice call is made.

SIDE EFFECTS
  Data dial handler or voice call handler invoked to init a call.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_atd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type         result;
  dsati_dial_str_type           dial_str;
  dsati_mode_e_type             current_mode;
  dsat_dial_modifier_info_type  modifier_info;
  ds3g_siolib_port_e_type port_count = DS3G_SIOLIB_DATA_PORT;
  boolean dtr_status = FALSE;
  ds3g_siolib_state_info_type *state = NULL;
  uint8  flag = FALSE;
#ifdef FEATURE_DSAT_ETSI_MODE
  dsat_num_item_type csta_val ;
#endif /*FEATURE_DSAT_ETSI_MODE*/
  dsatetsicall_call_state_info *call_val = NULL;
  call_val = dsat_get_base_addr(DSAT_CALL_VALS, FALSE);

/*------------------------------------------------------------------*/

  current_mode = dsatcmdp_get_current_mode();
  memset(&dial_str, 0, sizeof(dsati_dial_str_type));
  if(current_mode != DSAT_MODE_LTE && current_mode != DSAT_MODE_GSM )
  {
    return DSAT_ERROR;
  }

  if (mode == DSAT_CMD) 
  {

    /* process the dial string tok_ptr->arg[0],
       Get the dial number's attribute in dial_str 
    */
    if (!proc_dial_str(tok_ptr, &dial_str, &current_mode))
    {
    /* illegal dial string */
      return DSAT_ERROR;
    }

    /* error for empty dial number */
    if (dial_str.len == 0)
    {
      DS_AT_MSG0_HIGH("Dial number empty.");
      return DSAT_ERROR;
    }

    if(dsatcmdp_block_indications() == TRUE)
    { 
       while( port_count < DS3G_SIOLIB_PORT_MAX )
       { 
         (void)ds3g_siolib_ex_is_dtr_asserted(&dtr_status, port_count); 
         state = ds3g_siolib_get_ds3g_siolib_state(port_count); 
         if(state->serial_info.serial_state == DS3G_SIOLIB_AUTODETECT_SERIAL_STATE && dtr_status == TRUE) 
         {
           flag = TRUE;
           break;
         }\
         port_count++;
       }
       if(flag == FALSE)
       { /* If there is an existing DATA call
          **  -- Do not allow DATA calls in ETSI mode
          */
        if (!( dial_str.attrib & DIAL_ATTRIB_SEMICOLON ))
        {
          DS_AT_MSG0_HIGH("Data call issued while a data call is already in session");
          return DSAT_ERROR;
        }
      }
    }

    /* do special dial checking in ETSI modes,
       1. Check restricted mode.
       2. Check if the call is a direct dial.
       3. Check if the call is a voice call.
    */

      /* Override address type if +CSTA set to international calls. */
      /* Skip step for packet calls and phonebook direct dial.                  */
      csta_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSTA_IDX,0,NUM_TYPE);
      if ( (DSAT_TA_INTERNATIONAL == csta_val) &&
           !(dial_str.attrib & DIAL_ATTRIB_START_PLUS) && 
           !(dial_str.attrib & DIAL_ATTRIB_GREAT) &&
           !(dial_str.attrib & DIAL_ATTRIB_PKT) )
      {
        /* Prepend number with international call prefix. */
        /* String buffer can be extended as its already been saved in
         * previous command buffer and there is at least 'ATD' before
         * dial string */
        *(--dial_str.num) = DSAT_TA_INTER_PREFIX;
        dial_str.len++;
      }
      
      if (dial_str.attrib & DIAL_ATTRIB_GREAT)
      {
#ifdef FEATURE_ETSI_PBM      
        /* if DIAL_ATTRIB_GREAT is set in ETSI mode, 
           we do direct dialing process                     */

        /* Allocate command memory pointer for ATD command for direct dial */
        (void) dsat_alloc_cmd_mem_ptr(sizeof(etsime_dd_type), DSAT_BASIC_ACT_D_IDX);

        return dsatme_direct_dial(&dial_str, res_buff_ptr);
#else
        /* we don't suppurt direct dial */
        return DSAT_ERROR;
#endif /* FEATURE_ETSI_PBM */
      }
      else if (dial_str.attrib & DIAL_ATTRIB_SEMICOLON)
      {
        /* if DIAL_ATTRIB_SEMICOLON is set in ETSI mode,
           this is a voice call.                            */
        result = dsatetsicall_voice_dial_cmd(&dial_str);

#ifdef FEATURE_ETSI_PBM
        /* prepare the last dial number for saving to the last dial ph book */
        if(result == DSAT_ASYNC_CMD)
        {
          /* the number was dialed, init the ld_rec */
          dsatme_ld_init_record((char *)dial_str.num, -1, NULL);

          /* Save last dial record to phone book */
          dsatme_ld_save();
        }
#endif /* defined(FEATURE_ETSI_PBM) */

        return result;
      }


    /* dial through the dial table, depending on mode, CS or PKT call
     */
    if (!(dial_str.attrib & DIAL_ATTRIB_PKT) )
    {
      return DSAT_ERROR;
    }

    memset(&modifier_info, 0, sizeof(dsat_dial_modifier_info_type));

      /* Fill out dial string modifiers */
      if (DSAT_OK != dsatutil_fill_modifier_info(&dial_str, &modifier_info))
      {
        /* Problem with modifier setup */
        return DSAT_ERROR;
      }


    /* Update +CIND callsetup indicator */
    /* Updatation  only for GSM/WCDMA Subscription */
      (void)dsatetsime_change_indicator_state( DSAT_CIND_INDEX_CALLSETUP,
                                               DSAT_CIND_CSETUP_MOINIT);


    DS_AT_MSG1_HIGH("current_mode is %d",current_mode);

    if ( dsati_dial_table[current_mode] == NULL )
    {
      DS_AT_MSG0_HIGH("unregistered call handler");
      result = DSAT_ERROR;
    }
    else
    {
#ifdef FEATURE_DSAT_ETSI_DATA
      if ( (IS_ETSI_MODE(current_mode)) )
      {
        byte temp_cid = 1; /* default profile id*/

        /* Update CID if provided  */
        if ( ( modifier_info.modifier_present ) &&
             ( modifier_info.modifier_info.cid.cid_val > 0) )
        {
          temp_cid  =  modifier_info.modifier_info.cid.cid_val;
          if ( !DSAT_IS_PROFILE_ID_WITHIN_RANGE(temp_cid) )
          {
            DS_AT_MSG1_ERROR("Invalid CID %d",temp_cid);
            return DSAT_ERROR;
          }
        }
      }
#endif /* FEATURE_DSAT_ETSI_DATA */
    /* if there is a mode mismatch between current ATCoP 
       operating mode and current network mode,return DSAT_ERROR 
    */

      dsat_curr_abort_handler =
        dsati_dial_table[current_mode](dial_str.num, (dial_str.attrib & DIAL_ATTRIB_ASCII),
                                       &modifier_info );
#ifdef FEATURE_ETSI_PBM
      if ( dsat_curr_abort_handler != NULL )
      {
        dsatme_ld_init_record((char *) dial_str.num, -1, NULL);

        /* Save last dial record to phone book */
        dsatme_ld_save();
      }
#endif /* defined(FEATURE_ETSI_PBM) */

      result = DSAT_NO_RSP;
      /* Copy the curr_abort_handler for using it when S7 timer expires.
      This is required for PDP-IP calls where CONNECT is sent to TE before 
      CALL_EVENT_CONNECT from CM is received and we clear the curr_abort_handler.*/
      if(NULL == call_val->dsatetsicall_s7_info.data_s7_abort_handler)
      {
        call_val->dsatetsicall_s7_info.data_s7_abort_handler = dsat_curr_abort_handler;
        if(modifier_info.modifier_present)
        {
          call_val->dsatetsicall_s7_info.is_profile_id_valid = TRUE;
        }
      }
    }
  }
  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatact_exec_atd_cmd */

/*===========================================================================

FUNCTION DSATACT_EXEC_ATA_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ATA command.
  ATA command is used to answer a incoming call using AT commands

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_NO_RSP : if ds3g_answer_call() is called.

SIDE EFFECTS
  Calls ds3g_answer_call() function to answer a call

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ata_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
  if ( mode == DSAT_CMD )
  {
     /* ATA preference is in the following order:
        1. Voice call
        2. CS data call
        3. PS data call */
    if( dsatvoice_is_voice_call( DSAT_CALLID_ALL ) )
    {
      result = dsatvoice_answer_cmd();
    }
    else
    {
      if ( dial_string_handler.answer_cb == NULL)
      {
        result = DSAT_NO_CARRIER;
        DS_AT_MSG0_HIGH("unregistered incoming_cb handler");
      }
      else 
      {
        /* either answer_cb != NULL or pdp_answer_cb != NULL */
        result = DSAT_NO_RSP;
        
        /* Slam the modem port to the current active port */
        ds3g_siolib_set_answer_port ( DS3G_SIOLIB_ANSWER_MODE_MANUAL );

        if( dial_string_handler.answer_cb != NULL)
        {
          dsat_curr_abort_handler = dial_string_handler.answer_cb();
        }
        if ( dsat_curr_abort_handler == NULL )
        {
          DS_AT_MSG0_HIGH("unregistered ATA Abort handler");
        }
      } /* Answer callbacks are not NULL */
    } /* Not voice call */
  }
  else
  {
    result = DSAT_ERROR;
  }

  return result;

}/* dsatact_exec_ata_cmd () */

/*===========================================================================

FUNCTION DSATACT_EXEC_ATH_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ATH command.
  ATH command is used to hangup both data and voice calls
  using AT commands.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ATH_CMD : if hangup pointer is invoked.
    DSAT_OK : if CDMA-only and no hangup CB registered

SIDE EFFECTS
  Calls ds3g_hangup_call() function to terminate a call

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ath_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result  = DSAT_OK;
  dsatetsicall_call_state_info  *call_val = NULL;

  call_val = dsat_get_base_addr(DSAT_CALL_VALS, FALSE);
  /*---------------------------------------------  
    Processing for WRITE syntax
  ----------------------------------------------*/
  if ((tok_ptr->op == NA) || (tok_ptr->op == (NA|EQ|AR)))
  {
    if ((tok_ptr->op == (NA|EQ|AR)) && ( *tok_ptr->arg[0] != '0'))
    {
      /* check to see if the argument is valid */
      /* for ATH only 0 is defined */
      /* any other value would cause an error */
      DS_AT_MSG_SPRINTF_1_LOW( 
                     "ATH parameter not supported: %s",*tok_ptr->arg);
      return DSAT_ERROR;
    }
     /* ATH preference is in the following order:
        1. CS data call
        2. PS data call
        3. Voice call (if +CVHU=0) */
    if ( dial_string_handler.hangup_cb != NULL )
    {
      dial_string_handler.hangup_cb ();
      result = DSAT_ATH_CMD;
    }
    else
    {
#ifdef FEATURE_DSAT_ETSI_MODE
      dsati_mode_e_type current_mode = dsatcmdp_get_current_mode();
      if ( IS_ETSI_MODE(current_mode) )
      {
        uint8 num_calls = 0;
        cm_call_id_type call_ids[CM_CALL_ID_MAX];
        uint8 index;
        cm_call_mode_info_e_type call_mode = CM_CALL_MODE_INFO_GW_CS;

        memset ((void*)call_ids, CM_CALL_ID_INVALID, sizeof(call_ids));
          /* Waiting calls need to end(Only if +CVHU is 0) using call_cmd_answer 
             with reply as false */
        if( (call_val->etsicall_wait_call_present) && (((dsat_num_item_type)
                 dsatutil_get_val(DSATETSI_EXT_CVHU_IDX,0,NUM_TYPE)) == 0 ) )
        {
          DS_AT_MSG0_ERROR("End call request chvu set");
          etsicall_cmd_pending = CMD_ATH;
          for ( index = 0; 
                index < CM_CALL_ID_MAX; 
                index++ )
          {
            if(DSAT_VOICE_STATE_WAITING == voice_state[index].state)
            {
              call_mode = voice_state[index].call_mode;
              break;
            }
          }
          if( dsatcmif_answer_voice_call(call_val->etsicall_wait_call_id,FALSE,call_mode) )
          {
            result = DSAT_ASYNC_CMD;
          }
          else
          {
            etsicall_cmd_pending = CMD_NONE;
            DS_AT_MSG0_ERROR("End call request failed");
            result = DSAT_ERROR;
            return result;
          }
        }
        /* search for calls that should be ended... */
        for ( index = 0; 
              index < CM_CALL_ID_MAX; 
              index++ )
        {
          /* in ETSI builds active and held call lists are maintained for +CHLD
             command but these types must be searched for */
          if ( voice_state[index].state == DSAT_VOICE_STATE_CONNECTING )
          {
            call_ids[num_calls++] = index;
          }
        }

        if ( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CVHU_IDX,0,NUM_TYPE) == 0 )
        {
      
          DS_AT_MSG0_ERROR("End call request chvu set");
          /* Terminate any (voice) call in progress per v.25 ter */
          if ( num_calls > 0 ||
               call_val->etsicall_num_active_calls != 0 ||
               call_val->etsicall_num_held_calls != 0 ) 
          {
            /* Fill call_ids[] with all active & held voice calls that 
               should be ended by ATH */
            if ( call_val->etsicall_num_active_calls )
            {
              for ( index = 0; (index < call_val->etsicall_num_active_calls && 
                               num_calls < CM_CALL_ID_MAX); index++ )
              {
                call_ids[num_calls++] = call_val->etsicall_active_call_ids[index];
              }
            }

            if ( call_val->etsicall_num_held_calls )
            {
              for ( index = 0; (index < call_val->etsicall_num_held_calls &&
                               num_calls < CM_CALL_ID_MAX); index++ )
              {
                call_ids[num_calls++] = call_val->etsicall_held_call_ids[index];
              }
            }

            etsicall_cmd_pending = CMD_ATH;

            /* Make CM call to release them all */
            if ( dsatcmif_end_calls(num_calls, &call_ids[0]) )
            {
              /* the command callback function will be called */
              result = DSAT_ASYNC_CMD;
            }
            else
            {
              etsicall_cmd_pending = CMD_NONE;
              DS_AT_MSG0_ERROR("End call request failed");
              result = DSAT_ERROR;
             }
          }
          else
          {
            /* ATH is intended to hangup voice call.  If there is no
               active call return ok. */
            result = DSAT_OK;
          }

        }
        else
        {
          result = DSAT_OK;
          DS_AT_MSG0_HIGH("ATH cmd not in online_cmd_mode and +CVHU != 1");
        }
      }
      else
#endif /* FEATURE_DSAT_ETSI_MODE */
      {
        /* CDMA-only builds should not release voice call(s) on ATH */
        result = DSAT_OK;
      }
    }
  }
  else 
  {
    result = DSAT_ERROR;
  }
  return result;
}/* dsatact_exec_ath_cmd () */



/*===========================================================================

FUNCTION DSATACT_EXEC_ATO_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes O command.
  This command causes the DCE to return to online data state.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_NO_RSP : if ds3g_answer_call() is called.

SIDE EFFECTS
  None  

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ato_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;

  /*---------------------------------------------  
    Processing for WRITE syntax
  ----------------------------------------------*/
  if ((tok_ptr->op == NA) || (tok_ptr->op == (NA|EQ|AR)))
  {
    if ( tok_ptr->op == (NA|EQ|AR) )
    {
      /* check to see if the argument is valid  */
      if( DSAT_OK != dsatparm_exec_param_cmd(mode,
                                                 parse_table,
                                                 tok_ptr,
                                                 res_buff_ptr) )
      {
        return DSAT_ERROR;
      }
      if ( (dsat_num_item_type)dsatutil_get_val(
                               DSAT_BASIC_ACT_O_IDX,0,NUM_TYPE) > 0 )
      {
        /* for ATO only 0 is defined at this time */
        /* any other value would cause an error   */
        return DSAT_ERROR;

      }/* dsat_o_val > 0 . */
    }
    /* Check mode for correct state */
    if ( DSAT_ONLINE_CMD == mode )
    {
      /* Currently in ONLINE-COMMAND mode */
      enter_online_data_mode(TRUE, ds3g_siolib_get_active_port() );
      result = DSAT_CONNECT;
    }
    else
    {
      /* Currently in COMMAND mode */
      result = DSAT_NO_CARRIER;
    }
  }
  else
  {
    result = DSAT_ERROR;
  }

  return result;

} /* dsatact_exec_ato_cmd () */



/*===========================================================================

FUNCTION DSATACT_EXEC_GMI_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +GMI/+CGMI command.
  This command returns manufacturer information.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_gmi_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  static char info[32] = "QUALCOMM INCORPORATED";


  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
#ifdef FEATURE_DSAT_EXTENDED_CMD
    /*-----------------------------------------------------------------------
    Read the stored MFR. info  from the NV.
    -----------------------------------------------------------------------*/
    if (NV_DONE_S == dsatutil_get_nv_item(NV_UNDP_HSU_MFGSTR_I, &ds_nv_item))
    {
      res_buff_ptr->used =(word) strlcpy((char*)res_buff_ptr->data_ptr,
                                       (const char*)ds_nv_item.undp_hsu_mfgstr,
                                       res_buff_ptr->size);
    }
    else
#endif /* FEATURE_DSAT_EXTENDED_CMD */
    {
      res_buff_ptr->used =(word) strlcpy((char*)res_buff_ptr->data_ptr,
                                             (const char*)info,
                                             res_buff_ptr->size);
    }
    result = DSAT_OK;
    if( !(strncmp((const char *)parse_table->name,
        (const char *)"+CGMI" ,strlen((const char *)"+CGMI"))))
    {
      result = DSAT_NO_RESULT_CODE;
    }
  }

  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }

  return result;
}  /* dsatact_exec_gmi_cmd () */


/*===========================================================================

FUNCTION DSATACT_EXEC_GMM_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +GMM/+CGMM command.
  This command returns model information.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_gmm_cmd
(
 dsat_mode_enum_type mode,               /*  AT command mode:            */
 const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
 const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
 dsm_item_type *res_buff_ptr             /*  Place to put response       */
 )
{
  dsat_result_enum_type result = DSAT_OK;
#ifdef FEATURE_DSAT_EXTENDED_CMD
  nv_stat_enum_type nv_status;
#endif /* FEATURE_DSAT_EXTENDED_CMD */
  byte info[11];

  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
    /* Query NV for model identification */
#ifdef FEATURE_DSAT_EXTENDED_CMD
    nv_status = dsatutil_get_nv_item(NV_UNDP_HSU_PRODSTR_I, &ds_nv_item);
    if( ( nv_status == NV_DONE_S ) &&
         ( 0 != strlen((char*)ds_nv_item.undp_hsu_prodstr)))
    {
      (void) strlcpy((char*)res_buff_ptr->data_ptr,
                         (const char*)ds_nv_item.undp_hsu_prodstr,
                         res_buff_ptr->size);
      res_buff_ptr->used += (word)strlen((char*)res_buff_ptr->data_ptr);
      result = DSAT_OK;
    }
    else
#endif /* FEATURE_DSAT_EXTENDED_CMD */
    {
#ifdef CUST_MOB_MODEL_EXT
      (void) dsatutil_itoa(mob_model_ext, info, 10);
#else
      (void) dsatutil_itoa((uint32)mob_model, info, 10);
#endif /* CUST_MOB_MODEL_EXT */

      (void) strlcpy((char*)res_buff_ptr->data_ptr,
                     (const char*)info,
                     res_buff_ptr->size);
      res_buff_ptr->used += (word)strlen((char*)res_buff_ptr->data_ptr);
      result = DSAT_OK;
    }/* NV_UNDP_HSU_PRODSTR_I = !NV_DONE_S */
    if( !(strncmp((const char *)parse_table->name,
        (const char *)"+CGMM" ,strlen((const char *)"+CGMM"))))
    {
      result = DSAT_NO_RESULT_CODE;
    }

  }/* NAME */
 
  
  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }
  return result;
}  /* dsatact_exec_gmm_cmd () */


/*===========================================================================

FUNCTION DSATACT_EXEC_GMR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CGMR/+GMR command.
  This command returns revision information.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_gmr_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
/* boot_block_version isn't defined for NAND-based (6k and later) targets.
 * We need to send the default NOR boot block version in our response, for
 * backward compatibility. */
#define DSAT_BOOT_BLOCK_VERSION 0x1
  dsat_result_enum_type result = DSAT_OK;

  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
#ifdef FEATURE_DSAT_EXTENDED_CMD
    if( !strncmp((const char *)parse_table->name,
        (const char *)"+CGMR" ,strlen((const char *)"+CGMR")))
    {
      res_buff_ptr->used =
        (word)snprintf ((char*)res_buff_ptr->data_ptr,res_buff_ptr->size,
                             "%s: %s", parse_table->name, mob_sw_rev);
    }
    else
#endif/* FEATURE_DSAT_EXTENDED_CMD */
    {
      res_buff_ptr->used =
        (word)snprintf ((char*)res_buff_ptr->data_ptr,res_buff_ptr->size,
                              "%s  %d  [%s %s]", mob_sw_rev,
                              DSAT_BOOT_BLOCK_VERSION, rel_date, rel_time);
    }
    if( !(strncmp((const char *)parse_table->name,
        (const char *)"+CGMR" ,strlen((const char *)"+CGMR"))))
    {
      result = DSAT_NO_RESULT_CODE;
    }
  }
  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsatact_exec_gmr_cmd() */



/*===========================================================================

FUNCTION DSATACT_EXEC_GSN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +GSN/+CGSN command.
  This command returns serial number information.  In ETSI mode,
  the IMEI value is reported.  Otherwise, the ESN is reported.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_gsn_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  nv_stat_enum_type nv_status;
  static byte info[32];

  memset(info, 0, 32);
  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
#ifdef FEATURE_DSAT_ETSI_MODE
    /*-------------------------------------------------------
    Determine the response based on the operating mode.
    Report IMEI for ETSI mode, ESN otherwise
    ---------------------------------------------------------*/
    if (ETSI_CMD_MODE == dsatcmdp_get_operating_cmd_mode())
    {
      if (DSAT_OK == dsat_get_imei (info,sizeof(info)))
      {
        (void) strlcpy((char*)res_buff_ptr->data_ptr,
                           (const char*)info,res_buff_ptr->size);
        res_buff_ptr->used += (word)strlen((const char*)info);
        result = DSAT_OK;
      }
      else
      {
        dsatme_set_cme_error(DSAT_CME_MEMORY_FAILURE, res_buff_ptr);
        result = DSAT_CMD_ERR_RSP;
      }
    }
    else
#endif /* FEATURE_DSAT_ETSI_MODE */
    {
      /* ESN Reporting */
      nv_status = dsatutil_get_nv_item (NV_ESN_I, &ds_nv_item);
      if( nv_status == NV_DONE_S )
      {
        /* Convert the 32 bit unsigned number to hex */
        (void)dsatutil_itoa(ds_nv_item.esn.esn, info, 16);
        res_buff_ptr->used = 
                   (word)snprintf((char*)res_buff_ptr->data_ptr,
                                         res_buff_ptr->size,"0x%s", info);
        result = DSAT_OK;
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
    if( !(strncmp((const char *)parse_table->name,
        (const char *)"+CGSN" ,strlen((const char *)"+CGSN"))))
    {
      result = DSAT_NO_RESULT_CODE;
    }
  }

  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsatact_exec_gsn_cmd() */



/*===========================================================================

FUNCTION DSATACT_EXEC_GCAP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +GCAP command.
  This command returns complete capabilities list information.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_gcap_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  #define SIZE_OF_INFO 65   /* Size of info array */
  dsat_result_enum_type result = DSAT_OK;
  static char info[SIZE_OF_INFO]; 
  word cnt = 0;
  
const char cap_list_2000 [] = "+CIS707-A, CIS707, +MS, +ES, +DS, +FCLASS";

  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
    dsati_mode_e_type curr_mode = dsatcmdp_get_current_mode();
    dsati_operating_cmd_mode_type oper_cmd_mode = 
                            dsatcmdp_get_operating_cmd_mode();

    /*--------------------------------------
    Setup response based on operating mode
    ----------------------------------------*/
    /* Initialize response */
 
    (void) strlcpy((char*)info, (char*)"+GCAP: ", SIZE_OF_INFO);

    if (ETSI_CMD_MODE == oper_cmd_mode)
    {

      (void)snprintf (info, SIZE_OF_INFO,"%s%s%s", info, 
                            (!cnt++) ? "" : ",", "+CGSM");
    }
#ifndef FEATURE_MDM_FUSION
    /* For Fusion Targets cap_list_2000 should be present by default  */
    else if (CDMA_CMD_MODE == oper_cmd_mode)
#endif/* FEATURE_MDM_FUSION */
    {
      (void) strlcat(info, cap_list_2000, SIZE_OF_INFO); 
    }

    if ( DSAT_MODE_GSM == curr_mode )
    {
      (void)snprintf (info, SIZE_OF_INFO, "%s%s%s", info, 
                            (!cnt++) ? "" : ",",
                            "+DS");
    }

    (void) strlcpy((char*)res_buff_ptr->data_ptr, (const char*)info,
                       res_buff_ptr->size);
    res_buff_ptr->used += (word)strlen((const char*)info);
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatact_exec_gcap_cmd() */


#ifdef FEATURE_DSAT_ETSI_MODE
/* Exclude CDMA (for now, may be required later) */

/*===========================================================================

FUNCTION DSATACT_EXEC_WS46_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +WS46 command.
  This command manages wireless data service (WDS) side-stack selection.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.
    DSAT_ASYNC_CMD : if this function invokes an async command

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ws46_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsatcmif_servs_state_info  *ph_val = NULL;

  ph_val = dsat_get_base_addr(DSAT_PH_VALS, FALSE);
  
  /*-------------------------------------------------------
    WRITE & TEST command syntax
  ---------------------------------------------------------*/
  if( (tok_ptr->op == (NA|EQ|AR)) ||
      (tok_ptr->op == (NA|EQ|QU)) )      
  {
    /* For write syntax, validate parameter but do nothing else. RAT
     * preference is currently set via +COPS command. */
    result = dsatparm_exec_param_cmd( mode,
                                      parse_table,
                                      tok_ptr,
                                      res_buff_ptr );
  }
  /*-------------------------------------------------------
    READ command syntax
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|QU))
  {
    /* If functionality unknown, query CM */
    if ( DSAT_WS46_FUNC_MAX == ph_val->dsat_ws46_curr_val )
    {
       ph_val->dsat_ws46_res_type = DSAT_WS46_READ;
      /* The return value is ignored as both success and failure cases
       * are handled by the call back function */
      (void)cm_ph_cmd_get_ph_info (dsatcmif_ph_cmd_cb_func,
                                   NULL,
                                   dsatcm_client_id
                                   );
      result = DSAT_ASYNC_CMD;
    }
    else
    {
      /* Report the +WS46 side stack value based on current RAT mode
       * preference. */
      result = dsatact_process_ws46_cmd( &ph_val->dsat_net_reg_state.cmph_pref );
    }
  }
  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatact_exec_ws46_cmd() */

/*===========================================================================

FUNCTION DSATACT_SET_WS46_CURRENT_VALUE

DESCRIPTION
  This function set current value of WS46.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsatact_set_ws46_current_value
(
  cm_mode_pref_e_type  network_rat_mode_pref
)
{
  #define SYS_MODE_GERAN           0
#ifdef FEATURE_DSAT_LTE
  #define SYS_MODE_EUTRAN          1
  #define SYS_MODE_GERAN_EUTRAN    2
#endif /* defined (FEATURE_DSAT_LTE) */

  dsatcmif_servs_state_info  *ph_val = dsat_get_base_addr(DSAT_PH_VALS, FALSE);

  switch(network_rat_mode_pref)
  {
    case CM_MODE_PREF_GSM_ONLY:
      ph_val->dsat_ws46_curr_val = SYS_MODE_GERAN;
      break;
#ifdef FEATURE_DSAT_LTE
    case CM_MODE_PREF_LTE_ONLY:
      ph_val->dsat_ws46_curr_val = SYS_MODE_EUTRAN;
      break;
    case CM_MODE_PREF_GSM_LTE:
      ph_val->dsat_ws46_curr_val = SYS_MODE_GERAN_EUTRAN;
      break;
#endif /* defined (FEATURE_DSAT_LTE) */
  /* The cases CM_MODE_PREF_AUTOMATIC etc are taken 
   * care by the default case */
    default:
#ifdef FEATURE_DSAT_LTE
      ph_val->dsat_ws46_curr_val = SYS_MODE_GERAN_EUTRAN;
#else
      ph_val->dsat_ws46_curr_val = SYS_MODE_GERAN;
#endif /* defined (FEATURE_DSAT_LTE) */
      break;
  }

  /* Setting current value for ws46 */
  DSATUTIL_SET_VAL(DSAT_EXT_WS46_IDX,0,0,ph_val->dsat_ws46_curr_val,NUM_TYPE);
}


/*===========================================================================

FUNCTION DSATACT_PROCESS_WS46_CMD

DESCRIPTION
  This function process the response for the +WS46 command on return from
  asynchronous processing / synchronous function call. 
 
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
dsat_result_enum_type dsatact_process_ws46_cmd
(
  ds_at_cm_ph_pref_type  *pref_mode    /* Network preference mode */
)
{
  dsm_item_type * res_buff_ptr;
  const dsat_string_item_type (*list_ptr)[8] = dsat_ws46_list[0].list_v;
  dsatcmif_servs_state_info  *ph_val = NULL;

  ph_val = dsat_get_base_addr(DSAT_PH_VALS, FALSE);

  /* Process the READ command */
  if ( ph_val->dsat_ws46_res_type == DSAT_WS46_READ )
  {
    ph_val->dsat_ws46_res_type = DSAT_WS46_NONE;
    ph_val->dsat_net_reg_state.cmph_pref.network_rat_mode_pref = 
                                               pref_mode->network_rat_mode_pref;
  }

  res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
  /* Only the functionality value is output */
  /* Report the +WS46 side stack value based on current RAT mode
   * preference. dsat_ws46_curr_val is assigned the index of the 
   * actual network mode preference */

  dsatact_set_ws46_current_value(pref_mode->network_rat_mode_pref);

  res_buff_ptr->used = (word)snprintf ( (char*)res_buff_ptr->data_ptr,
                                              res_buff_ptr->size,
                                              "%s",
                                              list_ptr[ph_val->dsat_ws46_curr_val]);
  dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);
  return DSAT_OK;
} /* dsatact_process_ws46_cmd */

#endif /* FEATURE_DSAT_ETSI_MODE */

/*===========================================================================

FUNCTION DSATACT_EXEC_ATI_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes I command.
  This command returns identification information.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ati_cmd  
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type ati_val;

  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string.
  Arguments are accepted but ignored (to accomodate
  external software).
  ---------------------------------------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    if ( dsatutil_atoi(&ati_val, tok_ptr->arg[0], 10) )
    {
      /* error in converting the number */
      return DSAT_ERROR;
    }

    /* Even if it is an optional parameter check the range */
    if ( ati_val > 255 )
    {
      return DSAT_ERROR;
    }
  }

  if ( (tok_ptr->op == NA) ||
       (tok_ptr->op == (NA|EQ|AR)) )
  {
    dsm_item_type * temp_buff_ptr;
    unsigned int *attrib_ptr = (unsigned int *)&tok_ptr->op;
#ifndef FEATURE_DSAT_EXTENDED_CMD
    nv_stat_enum_type nv_status;
#endif /* FEATURE_DSAT_EXTENDED_CMD */
    *attrib_ptr = NA;
    
    /*-------------------------------------------------------
      Allocate a temporary buffer to hold composite response
    ---------------------------------------------------------*/
    temp_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
    temp_buff_ptr->data_ptr[temp_buff_ptr->used] = '\0';

    /* Manufacturer information */
    if (DSAT_OK == dsatact_exec_gmi_cmd (mode, parse_table, 
                                         tok_ptr, res_buff_ptr))
    {
#ifdef FEATURE_DSAT_EXTENDED_CMD
      temp_buff_ptr->used = (word)snprintf((char*)temp_buff_ptr->data_ptr,
                                          temp_buff_ptr->size,
                                          "%s: %s: %s",
                                          "Manufacturer",
                                          "+GMI",
                                          res_buff_ptr->data_ptr);
#else
      temp_buff_ptr->used = (word)snprintf((char*)temp_buff_ptr->data_ptr,
                                          temp_buff_ptr->size,
                                          "%s: %s",
                                          "Manufacturer",
                                          res_buff_ptr->data_ptr);
#endif/* FEATURE_DSAT_EXTENDED_CMD */
    }

    /* Model information */
    if (DSAT_OK == dsatact_exec_gmm_cmd (mode, parse_table, 
                                         tok_ptr, res_buff_ptr))
    {
      temp_buff_ptr->used = (word)snprintf((char*)temp_buff_ptr->data_ptr,
                                          temp_buff_ptr->size,
                                          "%s\n%s: %s",
                                          temp_buff_ptr->data_ptr,
                                          "Model",
                                          res_buff_ptr->data_ptr);
    }

    /* Revision information */
    if (DSAT_OK == dsatact_exec_gmr_cmd (mode, parse_table, 
                                         tok_ptr, res_buff_ptr))
    {
      temp_buff_ptr->used = (word)snprintf((char*)temp_buff_ptr->data_ptr,
                                          temp_buff_ptr->size,
                                          "%s\n%s: %s",
                                          temp_buff_ptr->data_ptr,
                                          "Revision",
                                          res_buff_ptr->data_ptr);
    }
#ifdef FEATURE_DSAT_EXTENDED_CMD
    temp_buff_ptr->used = (word)snprintf((char*)temp_buff_ptr->data_ptr,
                                        temp_buff_ptr->size,
                                        "%s\n%s: ",
                                        temp_buff_ptr->data_ptr,
                                        (ETSI_CMD_MODE ==
                                        dsatcmdp_get_operating_cmd_mode()) ? 
                                        "IMEI" : "ESN: +GSN" );
#else
    /* SVN information */
    nv_status = dsatutil_get_nv_item(NV_UE_IMEISV_SVN_I, &ds_nv_item);
    if (nv_status == NV_DONE_S)
    {
       temp_buff_ptr->used = (word)snprintf((char*)temp_buff_ptr->data_ptr,
                                          temp_buff_ptr->size,
                                          "%s\n%s: %02d",
                                          temp_buff_ptr->data_ptr,
                                          "SVN",
                                          ds_nv_item.ue_imeisv_svn);
    }
    /* ESN/IMEI information */
    temp_buff_ptr->used = (word)snprintf((char*)temp_buff_ptr->data_ptr,
                                        temp_buff_ptr->size,
                                        "%s\n%s: ",
                                        temp_buff_ptr->data_ptr,
                                        (ETSI_CMD_MODE ==
                                        dsatcmdp_get_operating_cmd_mode()) ? 
                                        "IMEI" : "ESN" );
#endif/* FEATURE_DSAT_EXTENDED_CMD */    
    if (DSAT_OK == dsatact_exec_gsn_cmd (mode, parse_table, 
                                         tok_ptr, res_buff_ptr))
    {
      temp_buff_ptr->used = (word)snprintf((char*)temp_buff_ptr->data_ptr,
                                          temp_buff_ptr->size,
                                          "%s%s",
                                          temp_buff_ptr->data_ptr,
                                          res_buff_ptr->data_ptr);
    }

    /* Capabilities information (heading already in response buffer) */
    if (DSAT_OK == dsatact_exec_gcap_cmd (mode, parse_table, 
                                          tok_ptr, res_buff_ptr))
    {
      temp_buff_ptr->used = (word)snprintf((char*)temp_buff_ptr->data_ptr,
                                          temp_buff_ptr->size,
                                          "%s\n%s",
                                          temp_buff_ptr->data_ptr,
                                          res_buff_ptr->data_ptr);
    }

    /* Flag if one DSM item not enough  (should never happen) */
    if (DSM_BUFFER_THRESHOLD < temp_buff_ptr->used)
    {
      DS_AT_MSG0_ERROR("Response too long for one DSM item");
    }

    /* Put composite response in result buffer */
    (void) dsatutil_memscpy((void*)res_buff_ptr->data_ptr,
                                   res_buff_ptr->size,
                            (void*)temp_buff_ptr->data_ptr,
                                   temp_buff_ptr->size);

    res_buff_ptr->used = temp_buff_ptr->used;
    (void) dsm_free_buffer( temp_buff_ptr );
  }

  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }

  return result;
}  /* dsatact_exec_ati_cmd() */



/*===========================================================================

FUNCTION DSATACT_EXEC_ATE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes E command.
  This command controls echoing of the command line to TE.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatact_exec_ate_cmd  
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  
#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
  char* arg=(char*) tok_ptr->arg[0];
  ds3g_siolib_port_e_type open_port = ds3g_siolib_get_active_port();
  dsat_modem_port_e_type modem_port;

  if( open_port == DS3G_SIOLIB_PORT_NONE )
  {
    return DSAT_ERROR;
  }
  modem_port = dsatctab_get_modem_port_id(open_port);
  if( modem_port == DSAT_MODEM_PORT_NONE )
  {
    return DSAT_ERROR;
  }
#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/

  /*-------------------------------------------------------
  Set command with a value
  ---------------------------------------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
    if( (tok_ptr->arg != NULL) && (tok_ptr->args_found !=0) )
    {
      if( (strlen(arg)== 1) && ( arg[0] == '0' || arg[0] == '1' ) )
      {
        DSATUTIL_SET_VAL(DSAT_BASIC_E_IDX,modem_port,0,arg[0] - '0',NUM_TYPE)
      }
      else
      {
        /*ignore garbage input data*/
        result = DSAT_ERROR;
      }
    }
    else
    {
      result = DSAT_ERROR;
    }
#else
    result = dsatparm_exec_param_cmd(mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr);
#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/
  }

  /*-------------------------------------------------------
  Set default value command
  ---------------------------------------------------------*/
  else if ( tok_ptr->op == NA )
  {
#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
    DSATUTIL_SET_VAL(DSAT_BASIC_E_IDX,modem_port,0,1,NUM_TYPE)
#else
    DSATUTIL_SET_VAL(DSAT_BASIC_E_IDX,0,0,1,NUM_TYPE)
#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/
  }

  /*-------------------------------------------------------
  Anything else
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }

  return result;
}  /* dsatact_exec_ate_cmd() */

/*===========================================================================

FUNCTION DSATACT_CALL_ABORT_HANDLER

DESCRIPTION
  This function does nothing but return a result to indicate the
  call should be aborted.  It should be installed in the command table
  as the abort handler function for abortable AT commands which bring
  up a call.

DEPENDENCIES
  None

RETURN VALUE
  Value indicating whether or not call should be aborted:
      TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  Call Manager is requested to end voice calls.

===========================================================================*/
/* ARGSUSED */
boolean dsatact_call_abort_handler
(
  const dsati_cmd_type *cmd_table_entry_ptr  /* Ptr to cmd entry in table. */
)
{
  dsati_mode_e_type  current_mode = dsatcmdp_get_current_mode();

  if ( IS_ETSI_MODE(current_mode) )
  {

    cm_call_id_type call_id;

    /* look for DSAT_VOICE_STATE_CONNECTING call ID... */
    for ( call_id = 0; call_id < CM_CALL_ID_MAX; call_id++ )
    {
      /* If DSAT_VOICE_STATE_CONNECTING call ID is found 
          (there should be only one)... */
      if ( voice_state[call_id].state == DSAT_VOICE_STATE_CONNECTING )
      {
        dsatvoice_voice_call_abort_handler();
        return FALSE;
      }
    }
  }

  if ( dsat_curr_abort_handler == NULL )
  {
    /* the abort handler hasn't been set and the user 
       directs to abort the call.
    */
    dsatcmdp_abortable_state = ABORTING;
  }
  else
  {
    (*dsat_curr_abort_handler)();
  }
  return TRUE;
} /* dsatact_call_abort_handler */


/*===========================================================================

FUNCTION DSATACT_PROC_DIAL_STR

DESCRIPTION
  Parses a dial string, looking for allowed non-digits, and
  illegal non-digits.  Digits, and allowed non-digits are copied
  to the output.  Illegal characters terminate processing.  All
  other characters are ignore.  Upper and lower case are treated
  as being the same.

  Since the output will be no longer than the input, the output
  string may be the same as the input string.

DEPENDENCIES
  None

RETURN VALUE
  Returns enum value indicating dial string content or error

SIDE EFFECTS
  None

===========================================================================*/
dsat_dial_val_e_type dsatact_proc_dial_str
(
  const byte * in_ptr,          /*  Input string, null terminated   */
  byte * out_ptr                /*  Output string, null terminated  */
)
{
  dsat_dial_val_e_type result = DSAT_DIAL_EMPTY;
  byte c;
  
  while (((c = *in_ptr++) != '\0') &&
         (result != DSAT_DIAL_ERROR))
  {
    dsat_dial_val_e_type vresult = dsatutil_validate_dial_char(&c);
    switch (vresult)
    {
      case DSAT_DIAL_DIGIT:
      case DSAT_DIAL_POUND:
      case DSAT_DIAL_STAR:
        *out_ptr++ = c;
        if (result == DSAT_DIAL_EMPTY)
        {
          result = DSAT_DIAL_DIGIT;
        }
        break;

      case DSAT_DIAL_ASCII_ABC:
        /* Allowed ASCII characters */
        *out_ptr++ = c;
        result = DSAT_DIAL_ASCII;
        break;

      case DSAT_DIAL_ASCII_D:
      case DSAT_DIAL_STRIP:
        /* Stripped characters */
        result = DSAT_DIAL_ASCII;
        break;

      case DSAT_DIAL_SEMICOLON:
        /* Do not pass semicolon to output */
        result = (ETSI_CMD_MODE == dsatcmdp_get_operating_cmd_mode()) ?
                  vresult : DSAT_DIAL_ERROR;
        break;

      case DSAT_DIAL_ASCII_E:
      case DSAT_DIAL_IGNORE:
        break;

      default:
        result = DSAT_DIAL_ERROR;
        break;
        
    } /* switch */
  } /* while */
  *out_ptr = '\0';
  return result;

} /* dsatact_proc_dial_str () */


#ifdef FEATURE_DSAT_ETSI_MODE
/*===========================================================================

FUNCTION DSAT_GET_IMEI

DESCRIPTION
  This function queries the IMEI value in NV and
  decodes it into an ASCII string.  The result is
  sent back to the caller in the passed buffer pointer.

DEPENDENCIES
  NV task must be available

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat_get_imei
(
  byte * rb_ptr,                  /* Pointer to return buffer */
  dsat_num_item_type  len         /* Buffer MAX len*/
)
{
  dsat_result_enum_type result = DSAT_OK;
  byte imei_bcd_len = 0;
  byte n = 0;
  byte digit;
  char imei_ascii[(NV_UE_IMEI_SIZE-1)*2];

  nv_stat_enum_type nv_status;

  rb_ptr[0] = '\0';

  /*  Read the IMEI  from NV */
  nv_status = dsatutil_get_nv_item(NV_UE_IMEI_I, &ds_nv_item);
  if (nv_status == NV_DONE_S)
  {
    /* Convert it to ASCII */
    imei_bcd_len = ds_nv_item.ue_imei.ue_imei[0];

    if( imei_bcd_len <= (NV_UE_IMEI_SIZE-1) )
    {
      /* This is a valid IMEI */
      memset(imei_ascii, 0, (NV_UE_IMEI_SIZE-1)*2);

      for( n = 1; n <= imei_bcd_len; n++ )
      {
        digit = ds_nv_item.ue_imei.ue_imei[n] & 0x0F;
        if( ( digit <= 9 ) || ( n <= 1 ) )
        {
          imei_ascii[ (n - 1) * 2 ] = digit + '0';
        }
        else
        {
          imei_ascii[ (n - 1) * 2 ] = '\0';
          break;
        }
        
        digit = ds_nv_item.ue_imei.ue_imei[n] >> 4;
        if( ( digit <= 9 ) || ( n <= 1 ) )
        {
          imei_ascii[ ((n - 1) * 2) + 1 ] = digit + '0';
        }
        else
        {
          imei_ascii[ ((n - 1) * 2) + 1 ] = '\0';
          break;
        }
      }
      
      /* Lose the first byte because it is just the ID */
       (void) dsatutil_memscpy((void*)rb_ptr,
                                      len,
                               (void*)(imei_ascii + 1),
                                     (NV_UE_IMEI_SIZE-1)*2-1);
      
    }
    else
    {
      /* This is an invalid IMEI */
      DS_AT_MSG0_ERROR("Invalid IMEI value from NV");
      result = DSAT_ERROR;
    }
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
}

#endif /* FEATURE_DSAT_ETSI_MODE */


/*===========================================================================

FUNCTION  DSATACT_CHECK_DIAL_PREFIX

DESCRIPTION
  This function checks if supplementary prefix is with dial string.

DEPENDENCIES
  None

RETURN VALUE 
    TRUE :    If supplementary dial prefix found
    FALSE:    Otherwise
    dial_prefix_type:  Type of supplementary service
    len_dial_prefix:   If dial prefix is valid, return length of dial prefix.

SIDE EFFECTS
  None

===========================================================================*/

boolean dsatact_check_dial_prefix
(
  const byte                   *dial_number,
  dsat_dial_prefix_e_type      *dial_prefix_type,
  byte                         *len_dial_prefix
)
{
#define MAX_DIAL_PREFIX_SIZE 5
  /* We can add more dial prefix as required and
     need to enum dsat_dial_prefix_e_type accordingly */
  const byte dial_prefix[DSAT_DIAL_PREFIX_MAX][MAX_DIAL_PREFIX_SIZE] = {"*31#", "#31#"};	
  int i, j;

  if(dial_number == NULL ||
     dial_prefix_type == NULL ||
     len_dial_prefix == NULL )
  {
    return FALSE;
  }

  for(i = 0; i < DSAT_DIAL_PREFIX_MAX; i++ )
  { 
    j = 0;
    /* This loop search if dial string starts with valid dial prefix or not. */
    while(*(*(dial_prefix  + i) + j) != '\0' && *(dial_number + j) !='\0')
    {
      if(*(dial_number + j) != *(*(dial_prefix + i) + j))
      {
        /* Does not match.. check for next */
        break;
      }
      j++;
    }
    /* Return dial prefix type and length of dial prefix if it is valid */
    if(j == strlen((char*)(*(dial_prefix + i))))
    {
      *dial_prefix_type = i ;
      *len_dial_prefix = j;
      return TRUE;
    }
  }
  return FALSE;
}

/*===========================================================================

FUNCTION  DSATACT_CHECK_VALID_DIAL_PREFIX

DESCRIPTION
  This function checks if supplementary prefix is within allowed list or not.

DEPENDENCIES
  None

RETURN VALUE 
    TRUE :    If supplementary dial prefix found
    FALSE:    Otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean dsatact_check_valid_dial_prefix
(
  const byte                   *dial_number
)
{
  dsat_dial_prefix_e_type      dial_prefix_type;
  byte                         len_dial_prefix;

  return dsatact_check_dial_prefix(dial_number, &dial_prefix_type, &len_dial_prefix);
}

/*===========================================================================

FUNCTION PROC_DIAL_STR

DESCRIPTION
  Parses a dial string in atd cmd token.  The number is in 
  tok_ptr->arg[0].

  Upon function returns, tok_ptr->arg[0] contains the processed/altered
  number.  The addtional information from the parsing process are in
  dial_ptr.

  The parser looks for chars of different category, see 
  dsat_dial_val_e_type.  It stripps out chars not needed depending on
  operating mode and on whether it is for voice call.
  It set the corresponding attrib bit on when encounters special chars
  or find a packet call pattern.

  Illegal characters terminate processing.  All other characters are ignore.

DEPENDENCIES
  None

RETURN VALUE
  TRUE          Processing finished without problem.
  FALSE         There is illegal char in the dial string, 
                or the mode is not right.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean proc_dial_str
(
  /* cmd token containing the atd argument       */
  const tokens_struct_type  *   tok_ptr,    

  /* dial string contain the parsed information
     This is output                              */
  dsati_dial_str_type       *   dial_ptr,

  /* current system operating mode               */
  dsati_mode_e_type         *   cur_mode_ptr
)
{
#ifdef FEATURE_DSAT_ETSI_MODE
  /*-------------------------------------------------------
    char sets in ETSI mode
    -------------------------------------------------------*/
  /* Illegal characters, currently no illegal chars.  
     Non-legal chars will be stripped out */
  static const byte illegal_etsi[]      = "";

  /* Cause DIAL_ATTRIB_ASCII be on */
  static const byte ascii_etsi[]        = "ABC";

  /* The chars the subsystem needs for dialing, + is kept
     only if it is at the beginning of the dial string */
  /* If PS data call add ',' to allowed ETSI dial characters as 
     multiple <cid>s in dial string can be separated by commas.
     Multiple <cid>s in dial string not currently supported and 
     this will exceed supported number of options in 
     dsatutil_check_opt_dial_str() and err. */
  static const byte keep_etsi_pkt[]     = "0123456789*#+ABC,";
  static const byte keep_etsi[]         = "0123456789*#+ABC";

  /*-------------------------------------------------------
    char sets in ETSI mode for voice call numbers
    -------------------------------------------------------*/
  /* Illegal characters, currently no illegal chars.  
     Non-legal chars will be stripped out */
  static const byte illegal_voice[]      = "";

  /* Cause DIAL_ATTRIB_ASCII be on */
  static const byte ascii_voice[]        = "";

  /* The chars the subsystem needs for dialing, + is kept
     only if it is at the beginning of the dial string */
  static const byte keep_voice[]        = "0123456789*#";

#endif /* FEATURE_DSAT_ETSI_MODE */

  const byte  *       illegal_set;
  const byte  *       ascii_set;
  const byte  *       keep_set;

  /* wrt_ptr points to the destination of processing
     rd_ptr  points to the source of processing
  */
  byte  *       wrt_ptr;
  byte  *       rd_ptr;
  byte          tmp_c;

  /* eol_ptr is writable end of line pointer */
  byte  * eol_ptr;
  /* Track plus if number starts with dial prefix */
  boolean       found_plus = FALSE;

/*------------------------------------------------------------------*/

  /* init attrib */
  dial_ptr->attrib = 0;
  dial_ptr->num    = tok_ptr->arg[0];

  DS_ASSERT(*tok_ptr->end_of_line == '\0');

  eol_ptr = tok_ptr->end_of_line;

  DS_AT_MSG1_HIGH("Curr mode = %d",*cur_mode_ptr);

  if ( dsatutil_strcmp_pkt_dial_str( dial_ptr->num, cur_mode_ptr) )
  {
    dial_ptr->attrib |= DIAL_ATTRIB_PKT;
  }
  
#ifdef FEATURE_DSAT_ETSI_MODE
  /* In ETSI mode, check for num[len-1,2,3] for ; [G|g], [I|i],
     set attrib accordingly strip them out and update len */
  if ( IS_ETSI_MODE(*cur_mode_ptr) )
  {
    /* if the last char of dial string (pointed by tok_ptr->end_of_line)
       is ;.  Turn on attrib DIAL_ATTRIB_SEMICOLON bit.
       Set ; be null and move eol_ptr one char */
    if ( (eol_ptr - dial_ptr->num > 0)
        && *(eol_ptr-1) == ';')
    {
      dial_ptr->attrib |= DIAL_ATTRIB_SEMICOLON;
      *(--eol_ptr) = '\0';
    }

    /* Check if Redial  i.e if the first char in dial string is L 
       then 'ATDL' is converted to its equivalent 'ATD>LD1'.
       i.e Dial the first entry from LD phone book
    */
    if (*dial_ptr->num == 'L')
    {
      DS_AT_MSG0_HIGH("Processing command -- ATDL");
      if (*(dial_ptr->num + 1)!= '\0')
      {
        return FALSE;
      }
      (void)strlcpy( (char *)dial_ptr->num, "LD1", 4);
      dial_ptr->len = 3;
      dial_ptr->attrib |= DIAL_ATTRIB_GREAT;
      return TRUE;
    }

    /* Check if ISDN dial  i.e if the first char in dial string is I */
    if (*dial_ptr->num == 'I')
    {
       DS_AT_MSG0_HIGH("Processing command -- ATDI");
       /* 'ATDI<dial-string>' is converted to its equivalent 'ATD<dial-string>' */
       dial_ptr->num++;
    }

    /* Check if this is a direct dialing number
       i.e if the first char in dial string is >
    */
    if (*dial_ptr->num == '>')
    {
      dial_ptr->attrib |= DIAL_ATTRIB_GREAT;
      dial_ptr->num++;
      if ( dsatutil_strip_quotes_out(dial_ptr->num, dial_ptr->num, 
                                     (uint16)(eol_ptr - dial_ptr->num)) )
      {
        dial_ptr->len = (uint16)strlen( (const char *)dial_ptr->num );
      }
      else
      {
        dial_ptr->len = (uint16)(eol_ptr - dial_ptr->num);
      }

      return TRUE;
    }
  }

#endif /* FEATURE_DSAT_ETSI_MODE */

  /* process chars one by one
     1. if encoutners non digit # * turn on DIAL_ATTRIB_ASCII
     2. strip out chars according to ETSI, CDMA, or voice */

#ifdef FEATURE_DSAT_ETSI_MODE
  if ( IS_ETSI_MODE(*cur_mode_ptr) && 
       (dial_ptr->attrib & DIAL_ATTRIB_SEMICOLON) )
  {
    illegal_set =       illegal_voice;
    ascii_set   =       ascii_voice;
    keep_set    =       keep_voice;
  }
  else if ( IS_ETSI_MODE(*cur_mode_ptr) )
  {
    illegal_set = illegal_etsi;
    ascii_set   = ascii_etsi;
    keep_set    = (dial_ptr->attrib & DIAL_ATTRIB_PKT) ?
                    keep_etsi_pkt : keep_etsi;
  }
#endif /* FEATURE_DSAT_ETSI_MODE */
  else
  {
    /* Unexpected mode, don't know how to process the dial string */
    return FALSE;
  }    

  wrt_ptr = dial_ptr->num;
  rd_ptr = dial_ptr->num;

  /* process '+' for DIAL_ATTRIB_START_PLUS attribute*/
  if (*rd_ptr == '+')
  {
    dial_ptr->attrib |= DIAL_ATTRIB_START_PLUS;
    rd_ptr++;
    wrt_ptr++;
    found_plus = TRUE;
  }

  /* Dial string scan loop */
  while (*rd_ptr != '\0')
  {
    /* In ETSI mode, check for I/G modifiers */
    if ( IS_ETSI_MODE(*cur_mode_ptr) )
    {
      /* Check for CUG control */
      if ('G' == *rd_ptr)
      {
        dial_ptr->attrib |= DIAL_ATTRIB_UP_G;  /* set bit */
        dial_ptr->attrib &= ~DIAL_ATTRIB_DN_G; /* clear bit */
      }
      else if ('g' == *rd_ptr)
      {
        dial_ptr->attrib |= DIAL_ATTRIB_DN_G;  /* set bit */
        dial_ptr->attrib &= ~DIAL_ATTRIB_UP_G; /* clear bit */
      }
      /* Check for CLIR control */
      else if ('I' == *rd_ptr)
      {
        dial_ptr->attrib |= DIAL_ATTRIB_UP_I;  /* set bit */
        dial_ptr->attrib &= ~DIAL_ATTRIB_DN_I; /* clear bit */
      }
      else if ('i' == *rd_ptr)
      {
        dial_ptr->attrib |= DIAL_ATTRIB_DN_I;  /* set bit */
        dial_ptr->attrib &= ~DIAL_ATTRIB_UP_I; /* clear bit */
      }
    }
    
    tmp_c = UPCASE(*rd_ptr);

    /* keep chars */
    if (strchr((char *)keep_set, (char) tmp_c) != NULL)
    {
      /* see if the char is in ascii_set */
      if (strchr((char *)ascii_set, (char) tmp_c) != NULL)
      {
        dial_ptr->attrib |= DIAL_ATTRIB_ASCII;
      }

      /* the *rd_ptr is in the keep_set */
      *wrt_ptr = *rd_ptr;
      wrt_ptr++;
      rd_ptr++;
    }
    else if (strchr((char *)illegal_set, (char) *rd_ptr) != NULL)
    {
    /* current rd_ptr points to an illegal chars */
      return FALSE;
    }
    else
    {
      /* strip this char out */
      if(!found_plus && *rd_ptr == '+')
      {
        if(dsatact_check_valid_dial_prefix(dial_ptr->num))
        {
          dial_ptr->attrib |= DIAL_ATTRIB_START_PLUS_AFTER_VALID_DIAL_PREFIX;
          /* Keep this + and discard if came later */
          *wrt_ptr = *rd_ptr;
          wrt_ptr++;
          rd_ptr++;
        }
        found_plus = TRUE;
      }
      else
      {
        rd_ptr++;
      }
    }
  }

#ifdef FEATURE_DSAT_ETSI_MODE

  /* Verify the attributes of the call.
     If atleast one of them is set; 
     it becomes per call CLIR invocation/suppression
     Don't apply CLIR (dsat_clir_val) settings 
     Otherwise apply the appropriate CLIR settings */
  if ( IS_ETSI_MODE(*cur_mode_ptr) )
  {
    if(((dial_ptr->attrib & DIAL_ATTRIB_UP_I) == FALSE) &&
       ((dial_ptr->attrib & DIAL_ATTRIB_DN_I) == FALSE))
    {
      dsat_dial_prefix_e_type  dial_prefix_type = DSAT_DIAL_PREFIX_NONE;
      /* Update attribute if dial string starts with valid prefix */
      if(dsatact_check_dial_prefix(dial_ptr->num, &dial_prefix_type, &dial_ptr->len_dial_prefix))
      {
        if(DSAT_DIAL_PREFIX_CLIR_SUPPRESSION == dial_prefix_type)
        {
          dial_ptr->attrib |= DIAL_ATTRIB_DN_I;  /* set bit */
        }
        if(DSAT_DIAL_PREFIX_CLIR_INVOCATION == dial_prefix_type)
        {
          dial_ptr->attrib |= DIAL_ATTRIB_UP_I;  /* set bit */
        }
        dial_ptr->valid_dial_prefix = TRUE;
      }
      /* Else update with AT+CLIR setting */
      else
      {
        if((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_ACT_CLIR_ETSI_IDX,
               0,NUM_TYPE) == (dsat_num_item_type)DSAT_CLIR_INVOCATION_OPTION)
        {
          dial_ptr->attrib |= DIAL_ATTRIB_UP_I;  /* set bit */
          dial_ptr->attrib &= ~DIAL_ATTRIB_DN_I; /* clear bit */
        }
        else if((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_ACT_CLIR_ETSI_IDX,
                 0,NUM_TYPE) == (dsat_num_item_type)DSAT_CLIR_SUPPRESSION_OPTION)
        {
          dial_ptr->attrib |= DIAL_ATTRIB_DN_I;  /* set bit */
          dial_ptr->attrib &= ~DIAL_ATTRIB_UP_I; /* clear bit */
        }
      }
    }
  }

#endif /* FEATURE_DSAT_ETSI_MODE*/

  /* null terminate the dial_str */
  *wrt_ptr = '\0';
  eol_ptr = wrt_ptr;
  dial_ptr->len = (uint16) (eol_ptr - dial_ptr->num);
    
  return TRUE;
} /* proc_dial_str */


/*===========================================================================

FUNCTION ENTER_ONLINE_DATA_MODE

DESCRIPTION
  This function is used to initiate an asynchronous SIO/ATCOP processing mode
  change to online data mode. The flush_tx parameter is used to indicate
  whether the SIO TX fifo should be flushed prior to switching to online
  data mode as follows:

    If flush_tx == TRUE, the SIO TX fifo will be flushed.
    If flush_tx == FALSE, the SIO TX fifo will not be flushed, any data 
      currently in the queue will be lost.

  The function sends either a DS_ENTER_ONLINE_DATA_TX_FLUSH_CMD or 
  DS_ENTER_ONLINE_DATA_NO_TX_FLUSH_CMD depending on the value of flush_tx to
  3G Dsmgr to initiate the mode change.  If no TX flushing is required, the
  mode change will occur immediately, otherwise, 3G Dsmgr will schedule the 
  SIO TX fifo to be flushed.  When the flush completes, the switch to online 
  data mode will be made.

DEPENDENCIES
  Must be able to allocate a DS Task command buffer.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void enter_online_data_mode
(
  boolean flush_tx,
  ds3g_siolib_port_e_type port_id
)
{
  ds_command_type * cmd_ptr;

  /* Allocate the command buffer and queue the command to the DS Task */
  cmd_ptr = dsat_get_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO, FALSE);
  /* Need to determine the appropriate command based on whether or
  ** not the TX buffer should be flushed.
  */
  if (flush_tx)
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_SIOLIB_ENTER_ONLINE_DATA_TX_FLUSH;
    cmd_ptr->cmd_payload_ptr = (void*) port_id;
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_SIOLIB_ENTER_ONLINE_DATA_NO_TX_FLUSH;
    cmd_ptr->cmd_payload_ptr = (void*) port_id;
  }

  /* Send the command on its way */
  ds_put_cmd(cmd_ptr);
} /* enter_online_data_mode() */
/*===========================================================================

FUNCTION PROCESS_AMPS_CMD

DESCRIPTION

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL dsat_result_enum_type process_amps_cmd ( void )
{
  dsat_num_item_type  current_amp_s = (dsat_num_item_type)dsatutil_get_val(
                                      DSAT_BASIC_N_S_IDX,0,NUM_TYPE);
  ds3g_siolib_port_e_type open_port = ds3g_siolib_get_active_port();

  /*-------------------------------------------------------
          DSR should be always Asserted
  -------------------------------------------------------*/
  /* Change from "ON during Connection" to "Always ON" */
  if ( (dsat_num_item_type)dsatutil_get_val(
               DSAT_BASIC_N_S_IDX,0,NUM_TYPE) == DS3G_DSR_ALWAYS_ON )
  {
    if ( ds3g_siolib_dsr_assert( open_port ) != DS3G_SIOLIB_OK )
    {
      DS_AT_MSG0_ERROR("AT&S0 setting failed");
      /* Re-setting dsat_amp_s_val */

      DSATUTIL_SET_VAL(DSAT_BASIC_N_S_IDX,0,0,current_amp_s,NUM_TYPE)
      return DSAT_ERROR;
    }
  }
  /*---------------------------------------------------------------
     DSR should be De-asserted and assert at CONNECT 
  ----------------------------------------------------------------*/
  /* Change from "Always ON" to "ON during Connection"*/
  else /* DS3G_DSR_CONNECT_ON */
  {
    if ( ds3g_siolib_dsr_deassert ( open_port ) != DS3G_SIOLIB_OK)
    {
      DS_AT_MSG0_ERROR("AT&S1 setting failed");
      /* Re-setting dsat_amp_s_val */
      DSATUTIL_SET_VAL(DSAT_BASIC_N_S_IDX,0,0,current_amp_s,NUM_TYPE)
      return DSAT_ERROR;
    }
  }
  DS_AT_MSG1_LOW("Changed AT&S to %d",(dsat_num_item_type)dsatutil_get_val(
                                 DSAT_BASIC_N_S_IDX,0,NUM_TYPE));
  return DSAT_OK;
}


