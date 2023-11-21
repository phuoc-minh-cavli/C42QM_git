/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( C A L L  M A N A G E R  I N T E R F A C E )
                
                P R O C E S S I N G

GENERAL DESCRIPTION
  This software unit contains functions for interfacing to Call Manager.

EXTERNALIZED FUNCTIONS

EXTERNALIZED FUNCTIONS INTERNAL TO DSAT UNIT
  dsatcmif_cm_call_cmd_handler
    This function is the handler function for the CM call related commands

  dsatcmif_cm_call_event_handler
    This function is the handler function for the CM call related events

  dsatcmif_cm_ss_cmd_handler
    This function is the handler function for the CM serving system commands

  dsatcmif_cm_ss_event_handler
    This function is the handler function for the CM serving system
    related events

  dsatcmif_get_cm_ss_info
    This function issue query to Call Manager to get serving system (SS) info

  dsatcmif_change_operating_mode
    This function submits request to Call Manager to change the phone
    operating mode.  The results of the call are handled as asynch
    events.

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2002 - 2022 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatcmif.c_v   1.4   12 Nov 2002 13:00:24   sramacha  $
  $Header: //components/rel/dataiot.mpss/2.0/interface/atcop/src/dsatcmif.c#5 $ $DateTime: 2022/03/28 05:16:35 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/28/22   ks      Removed incorrect SET_PENDING flag used for CEDRXS URC.
07/19/21   sp      Added support to handle COPS Dereg folowed by AUTO case.
01/25/21   ks      Added support for FSK related AT commands.
07/07/20   ks      Fixed incorrect cell-id on attaching to LTE network.
06/13/17   rk      Added support for +CEDRXS & +CEDRXRDP command.
26/04/17   rk      Fixed CVMOD issue to set the Voice call mode pref to VOIP only.
12/09/16   skc     Fixed CEREG URC issue to display cell info in LTE Only build.
10/05/16   skc     Fixed KW issues.
12/10/15   skc     Adding condition for enabling of dsat_power_off only when
                   operating mode is not online.
02/19/15   tk      Fixed issue in subscription not available event handling.
02/10/15   tk      Fixed +COPS issue in handling of CM PH command errors.
11/24/14   pg      Fixed ws46 featurization issue.
08/12/14   pg      Modified DS Profile APIs to per subs.
06/27/14   tk/sc   Added support for Dynamic ATCoP.
06/26/14   sc      Fixed $QCSIMAPP issue for C+G
05/27/14   pg      Fixed memory leak for emergency number.
04/03/14   pg      Fixed subs feature mode updation.
02/15/14   pg      Added support for SGLTE+G.
05/07/14   sc      Fixed +CLCC issue.
04/29/14   sc      Added csg_list_cat support to $QCCSGCOPS command.
04/23/14   sc      Fixed +CFUN issue.
04/15/14   sc      Fixed +COPS DSDS issue.
03/27/14   pg      Fixed NOCARRIER response for command issue.
03/26/14   sc      Fixed SYSINFO display issue on SUB2.
03/20/14   sc      Fixed SYSCONFIG issue on SUB2.
03/14/14   tk      Optimized debug macros usage in ATCoP.
01/15/14   sc      Fixed +COPS issue in invalid state for subs proc.
01/10/14   sc      Fixed $QCSIMAPP issue in 3GPP2 service mode.
01/09/14   tk      Fixed +CFUN issue in C+G DSDS configuration at boot-up.
12/25/13   tk      Enhanced $QCSIMAPP command for better user experience.
11/22/13   sc      Fixed Bolt MOB3.0 compiler warnings.
11/13/13   sc      Added support for $QCCSGCOPS command.
11/01/13   sc      LTE related CM call events support for AT+CEER
10/21/13   sc      Fixed +CFUN and +COPS issue.
10/18/13   tk      Added support for OnChip SIM in +CFUN command.
10/11/13   sc      Added support for $QCNSP, $QCSIMT, $QCRCIND commands and
                   REMOTE CALL END, REMOTE RING, REMOTE ANSWER
                   unsolicited result codes.
10/03/13   tk      ATCoP changes for L+G DSDS.
09/25/13   sc      Added support for +CNMPSD command.
09/13/13   tk      ATCoP changes for LTE and DSDS concurrency solution.
09/13/13   sc      Fixed KW errors.
08/20/13   sc      Fixed +COPS issue with subscription already available error.
07/26/13   pg      Fixed no response for ^PREFMODE=2.
07/27/13   pg      Fixed CLCC issue on SUB2.
07/24/13   tk      Mainlined FEATURE_DSAT_HIGH_TIER for +CGATT command.
07/16/13   pg      Fixed no ^MODE URC when UE goes out of service and comes back
07/16/13   pg      Fixed ^SYSINFO o/p for "SIM is not inserted"
06/05/13   tk      ATCoP changes for SGLTE support on Dime Plus.
05/24/13   sd      TSTS changes for DS3G.
05/16/13   sk      $QCCOPS command for Extension of network search.
04/19/13   tk      Fixed issue in DSDS device mode.
04/08/13   tk      Fixed issue with restricted mode in few scenarios.
03/27/13   tk      Fixed +CGEV URC issue.
03/27/13   tk      Fixed double free issue with emergency number list.
03/27/13   tk      Fixed issues in dual stack commands.
03/17/13   tk      Fixed stack overflow issue with emergency number list.
03/15/13   tk      Fixed +CFUN issue in CDMA in single SIM device mode.
03/01/13   tk      Fixed COPS issue in DSDS.
02/22/13   tk      ATCoP - CM interface changes for Triton DSDA.
02/07/13   tk      ATCoP changes for DSDx Co-existence with TDS.
02/07/13   tk      Fixed CGATT issue when ciphering is enabled.
02/01/13   tk      Fixed DSCI URC issue with call end.
01/23/13   sk      DS command buffer memory reduction.
01/09/13   tk      Fixed issues found in DTF CI runs.
01/04/13   tk      Fixed +CPAS and +CEER issues.
01/04/13   tk      ATCoP changes for DSDX C/G+G with True World Mode Support.
11/30/12   tk      ATCoP changes for Triton DSDA.
11/21/12   sk      Fixed CSQ issue due to invalid updation of RSSI,BER values.
09/24/12   tk      Migrated to CM APIs for MRU table access.
08/22/12   sk      Mainlining Nikel Changes.
08/10/12   tk      Fixed ^MODE issue in reporting CDMA/HDR hybrid mode.
08/06/12   tk      Added support for $QCDRX command.
07/26/12   sb      Fixed KW Errors.
07/16/12   nc      New interface changes for Manual PLMN selection.
06/15/12   nc      Fixed CGATT in no service.
06/20/12   tk      Fixed issues related to ^HDRCSQ and ^SYSINFO commands.
06/12/12   tk      Extended $QCSYSMODE command for 3GPP2 modes.
06/01/12   sk      TDS mode changes.
05/21/12   tk      Fixed COPS manual PLMN selection issue.
05/18/12   tk      Migrated to MSG 2.0 macros
05/11/12   sk      Fixed CLCC MT calls reporting in CDMA mode.
04/19/12   sk      Resolved compilation error when FEATURE_HS_USB_SER3_PORT enabled.
04/11/12   sk      Adding support for forwarding CFUN.
03/22/12   sk      Fixed CFUN command when executed without a card.
03/07/12   sk      Made CLCC common for all modes.
02/20 12   sb      Fixed Compiler Warnings.
02/17/12   sb      Fixed Compiler and KW Warnings.
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
02/06/12   ad      Added CSTF/CEN/CGPIAF/CVMOD command support.
02/06/12   nc      Added support for ^SPN and URC Queue.
01/27/12   sk      Fixed +CLCC in CDMA mode.
12/26/11   ad      Added Support the New Call Control Interface in Call Manager.
01/19/12   sk      Feature cleanup.
01/04/12   sb      Fixed KW Warnings.
12/06/11   sk      Fixed AT$QCBANDPREF write and read issue CR:321373
11/28/11   sb      Fixed the mode check for answering voice calls when mode is in LTE 
11/25/11   sb      Fixed Not considering SYS_SRV_STATUS_LIMITED_REGIONAL as a valid srv_status.
11/05/11   nc      Feature wrapped '^MODE' URC with FEATURE_TDSCDMA.
09/06/10   nc      Fixed the RSSI and RSCP display when 
                   FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY is enabled.
09/09/11   nc      Fixed $QCDGEN over IRAT scenarios.
09/01/11   ad      Fixed +CEMODE issue. 
07/21/11   sa      Added support for 64QAM configuration indication while roaming.
07/01/11   bs      Fixed +CGATT issue in PS only mode.
07/05/11   nc      Added support for ^MODE,^CARDMODE,^SYSCONFIG,^SYSINFO,^DSCI.
05/17/11   bs      Fixed +COPS issue.
04/29/11   bs      Fixed $QCDGEN command to use COMPTASK.
04/27/11   bs      Added NIKEL Phase I support.
03/29/11   nc      Fixed +VTS command.
03/01/11   nc      Modified +CEMODE to be inline with CM Design.
02/24/11   bs      Fixed Klocwork errors.
02/10/11   ad      Fixed CM related commands .
02/11/10   bs      Fixed incorrect PNN display for LTE.
01/11/11   ad      Removed pending variable .
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/25/10   ad      Init Changes for DSDS. 
10/19/10   bs      Fixed +COPS handling for power save.
10/19/10   ad      Relocated MSG_HIGH before memory free.  
10/06/10   ad      Added Error msg before Assert(0).
10/12/10   bs      Fixed +CREG issue.
09/20/10   bs      Fixed +CGATT issue.
07/09/10   bs      Fixed +CGACT for LTE.
06/04/10   bs      MMGSDI SESSION Migration.
06/02/10   bs      Fixed compilation issues.
05/31/10   ad      Added support for S7 command.
05/17/10   ad      Added Support for +CEREG for LTE.
05/10/10   kk      Added support for $$GPS* and $SP* commands.
05/10/10   kk      Added support for ^HC SMS commands. 
05/10/10   kk      Added support for ^DSDORMANT.
05/10/10   kk      Added ^SYSINFO, ^HDRCSQ, ^HRSSILVL commands. 
03/15/10   bs      Added +CQI support.
03/04/10   kk      Featurization fixes for multiple commands.
01/20/10   kk      Extended +CFUN to CDMA/MM targets.
01/15/10   ua      Added support for CDMA CLCC command. 
06/10/09   ua      Added support for ^PREFMODE command. 
06/12/09   ua      Added support for $QCBANDPREF command.
12/15/09   nc      Featurisation changes for LTE.
10/12/09   nc      Using memcpy() to copy dsat_incom_call_num from event_ptr.
10/05/09   ua\bs   Fixed +COPS issue.
10/30/09   nc      Returning OK after STOP_CONT_DTMF_CONF from CM for +VTS.
09/22/09   vrk	   Merged LTE changes.
09/09/09   ua      Added support for EONS (spec 22.101).
08/21/09   sa      Fixed Lint Errors.
08/04/09   nc      Added support for *CNTI.
07/15/09   sa      Added support for $CSQ command.
06/11/09   nc      Modified the prototype of dsatcmif_answer_voice_call 
                   to include answer or reject.
07/22/09   nc      Fixed async state after +COPS query followed by an Emergency call.
06/25/09   vg      Fixed +COPS related issue.
06/14/09   nc      Fixed the border case of PDP activation for cid equals 16.
04/20/09   ua      Fixed issues related with call commands followed by +CIND query.
05/20/09   bs      Fixed wrong CREG reporting in flight mode.
04/29/09   ua      Fixed compiler warnings. 
04/23/09   bs      Fixed +CREG issue.
03/11/09   ua      Correcting dsatcmif_is_socket_call function.
01/30/09   ua      Added support of CGEV for DUN calls.
02/16/09   nc      Added support to handle CM_CALL_EVENT_CALL_FORWARDED
01/19/09   bs      Fixed +COPS issue associated with DSAT_CFUN_CARD_POWER_CTL enabled.
01/09/09   nc      Fixing Compilation issue in dsatcmif.c
12/29/08   nc      Added support for +VTS Command.
12/29/08   ua      Processing CM_PH_EVENT_OPRT_MODE in both ETSI and CDMA mode.
12/23/08   sa      Replaced q_init with dsm_queue_init for watermark initialization.
12/17/08   dh      Lint fixes
12/15/08   ua      Register with CM to receive event notifications during 
                   power collapse. 
11/26/08   ss      Off target Lint fix.
10/22/08   bs      Fixed +CHLD UI-ATCOP corner case scenario.
10/13/08   bs      Fixing compilation issues.
09/02/08   sa      Updating RSSI value in SS event info.
07/21/08   bs      Added support for +CREG=2 command.
06/13/08   ua      Removing the changes for reading the NV RTRE control 
                   through CM. 
04/24/08   bs      Added support for power collapse registration for SS events.
02/20/08   sa      Added support for $QCSQ command.
02/05/08   bs      Fixing +COPS initial query command.
01/25/08   bs      Fixing COPS test command behaviour on SMS arrival scenario.
11/24/07   ua      Fixed Lint-lows. 
10/30/07   sa      Fixed initial CFUN query command.
10/12/07   sa      Correcting CREG unsolicited response and COPS issues.
10/09/07   ss      Correcting the behaviour for GSM+1x targets.
10/05/07   ss      Correcting featurization. 
09/21/07   sa      Fixed +CLCC reporting when call is in set-up indication phase.
09/05/07   ua      Correcting featurization. 
08/20/07   ua      Correcting error handling of COPS cmd and handling events
                   for COPS de-registration. 
08/01/07   sa      Fixed inter RAT Handover support for Multi Processor builds.
07/12/07   pp      Fixed Lint high error.
07/02/07   ua      Modifications to have rtre_control update through CM API. 
06/12/07   ss      Fixes for +WS46 read on power on.
04/23/07   pp      Lint Medium fixes.
04/24/07   sa      Correcting ETSI feature wrapping
04/05/07   ua      Added support for COPS=2 (deregistration )
04/09/07   ua      Corrected the error handling of CGATT command.
04/04/07   sa      Correcting COPS abort handling.
03/28/07   sa      Correcting +CHLD for 2x send.
03/23/07   ua      Correcting COPS considering LIMITED SERVICE as not registered.
03/08/07   ua      Handle REGISTRATION_DENIED in CREG/CGREG when SIM state is INVALID.
03/02/07   sa      Added outgoing voice call support when there is an active call  
                   for ATD command.
02/15/07   ss      Fixed lint high errors
02/13/07   pkp     Fixed RVCT compiler warnings.  
01/30/07   ss      Replaced banned string API calls.
11/17/06   sa      Correcting +CFUN command.
11/03/06   snb     Dual processor changes.
10/27/06   ua      Correcting ETSI feature wrapping 
09/26/06   sa      Inter RAT handover support for $QCDGEN.
09/01/06   ua      Correcting COPS Manual registration and CREG URC
07/11/06   ua      CACM/CAMM modifications according to the new CM APIs
06/08/06   ua      Eliminating usage of CM internal function cmph_info_get.
05/22/06   rsl     Add support for at$qcsysmode to return current sysmode, 
				   possible values returned: HSDPA/HSUPA/HSDPA+HSUPA/WCDMA/GSM.
05/12/06   ua      CPOL command is wrapped under FEATURE_MMGSDI				   
04/27/06   snb     Attempt to allocate first large DSM item or on failing that
                   try small item for CM message.
02/28/06   snb     CDMA-only build issue fix.
02/21/06   sa      Merged changes from Sirius Branch.
                   -Removed "dsat_clir_subscription_option" global variable.
01/27/06   snb     Correct indexing into dsatvend_datagen_info
01/16/06   ua      Added support for +CDIP command
12/19/05   snb     Lint corrections.
11/30/05   snb     Add support for +CGCMOD command.
11/11/05   ar      Return immediately to command mode for voice calls.
10/02/05   snb     On +CGATT detach command when already detached still send 
                   user preference to lower layers.
09/13/05   snb     Support for +CFUN=4 LPM mode.
08/29/05   snb     Add SIM card power control to +CFUN.
08/15/05   snb     Add support for reset via +CFUN command.
08/03/05   hap     Modified cmif_call_event_cb_func 
07/29/05   ar      Migrate CM SS events to watermark queue and discard 
                   redundant RSSI notices. 
07/22/05   snb     Register for RAB events and direct them to proper handler.
06/27/05   snb     Add call event stop filter: don't allow manage call 
                   confirmation events when no un-confirmed +CHLD call in 
                   progress.
06/20/05   snb     Don't register for SS or call confirmation events in CDMA 
                   only builds.
05/05/05   iv      Bug fix for +CLCC command
04/29/05   ar      Add support for capturing call end status.
04/22/05   snb     Changes to CM_CALL_EVENT_MNG_CALLS_CONF behavior needed
                   because of changes to CM reporting.
04/12/05   ar      Add support for +COPS access technology selection.
04/06/05   hap     Corrections for +CSSN command
04/05/05   iv      Modified +CLCC to support CS data calls.
04/01/05   tkk     dsat_pdp_deactivation_cause is wrapped under 
                   (WCDMA_PS || GSM_GPRS)
04/01/05   ar      Add support for +CIND service reporting
03/15/05   tkk     Added featurization for ETSI PS data items, corrected detach
                   report handling for +CGEREP and moved queue initialization 
                   routines to ETSI files.
03/03/05   snb     Redo Corrections to +CTZR/+CTZU event handling.
03/02/05   snb     On PS data call events copy nsapi and nsapi validity 
                   indicator to DS command.
02/28/05   hap     Added support for +CSSN command
02/15/05   snb     Fix CDMA-only build issues.
02/03/05   tkk     Added support for +CLIR command.
02/03/05   iv      Added support for +CPOL command.
01/27/05   pdv     Merge from Sirius-ATCoP development.
01/27/05   pdv     Rebase from Mainline.
01/17/05   tkk     Added support for +CGEREP command.
01/10/05   tkk     Added support for +CGPADDR command.
01/06/05   iv      Add support for ^SCNI command.
01/05/05   ar      Suppress duplicate registration domain notifications.
                   Code review action items.
01/05/05   iv      Added support for +CLCC command
12/30/04   tkk     Added CLIP and COLP support.
12/29/04   snb     Allow additional events through call event filter to fix
                   LD entry problem.
12/15/04   snb     Register with CM to receive CM_CALL_EVENT_ORIG so that call
                   originations by UI can be tracked and ended by ATH and 
                   +CHUP commands, correct +CGATT behavior on entry into power
                   save mode from PS_ONLY domain.
12/07/04   ar      Add support for +COPS command.
10/27/04   tkk     "cmif_ss_event_cb_func", "dsatcmif_cm_ss_event_handler" 
                   functions are modified to handle support for timezone 
                   information (for AT+CTZV)
10/15/04   ar      Add Advice of Charge support.
09/13/04   snb     Fix 6500 build warning.
09/03/04   snb     Add +CHLD support.
07/16/04   snb     Correct behavior when compound command requests CM call info
06/21/04   snb     Fix CM call command response when no error and removed
                   ERROR result code for unsupported CM call command.
05/12/04   snb     Fix +CGACT response when deactivating contexts activated via
                   embedded sockets and use local buffer for +CCWA unsolicited
                   response number.
04/15/04   snb     Fix for Jaguar build, using older CM API.
03/19/04   snb     Added support for +CCWA command.
11/07/03   snb     Fix out of order responses due to local formatted response
                   buffers.
10/22/03   ar      Move Call Manager PH & SS API calls from ETSI to COMMON.
10/07/03   ar      Add network registration reporting support
10/10/03   ar      Adjust call event filter to include emergency calls.
09/23/03   snb     Add support for PS attach timeout failure
09/16/03   snb     Added registration for CM_PH_EVENT_PS_ATTACH_FAILED event.
09/05/03   ar      Process only voice or outgoing packet data call events 
                   for ETSI mode.
08/04/03   ar      Removed obsolete dsatcmif_get_cm_call_info()
06/26/03   sb      CM_CALL_CMD_ORIG is now valid in both ETSI and 1X modes.
                   Now the event handler is in common files. Hence the API
                   name has changed from dsatetsicall* to dsatvoice*
03/12/03   ar      Ensure call end event processed by both voice and packet 
                   call handlers.
03/07/03   ar      Adjust CM event reg sequence for better error reporting
                   Remove FEATURE_DATA_ETSI_SUPSERV wrappers
02/26/03   ar      Add support for PDP context activation
02/21/03   ar      Adjust +CPAS no call ID error handling.
02/12/03   wx      Accommodate cm_mm_call_cmd_end api change and 
                   cm_end_params_s_type struct change
01/16/03   ar      Featurized USSD event registration.
11/14/02   jd      Featurized gw_cs_ans under (GSM|WCDMA) for other builds
11/11/02   sb      Added CM Supplementary Services Registration.
11/08/02   sb      Initialize cm_status to CM_CLIENT_OK.
11/07/02   ar      Added FEATURE_GSM_GPRS to FEATURE_DATA_WCDMA_PS wrappers
10/28/02   sb      Created module.


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
#include "dsatcmif.h"
#include "dsatvoice.h"
#include "dsatparm.h"
#include "cm.h"
#include "amssassert.h"
#include "err.h"
#include "msg.h"
#include "dsm.h"

#include <stringl/stringl.h>



/*===========================================================================

            PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains public definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/* Flag to indicate pending WRITE operation of attach command */
/* TRUE if it is a WRITE and FALSE otherwise                  */ 
cgatt_write_e_type dsatcmif_attach_write = DSAT_CGATT_MAX;

/* Flag for detach writes intended for local stack only:
   This change for GCF test cases allows user preference to be given to 
   local stack only and lets async event processing terminate once CMPH
   event is received as no serving system event is expected.  */
boolean dsatcmif_ss_evt_not_expected = FALSE;

#ifdef FEATURE_DSAT_ETSI_DATA
extern boolean                dsat_power_off;
#endif /* FEATURE_DSAT_ETSI_DATA */

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
extern uint8 is_card_action;

/* PREFMODE extern variable */
#ifdef FEATURE_DSAT_ETSI_MODE
/* Holding buffer for CM incoming call event number. */
/* Used in place of DS task cmd buffer to conserve storage and */
/* required for +CCWA unsolicited result code */
extern boolean                   dsatetsime_cind_pending;

/* variable which stores whether listing of preffered PLMN or
   PLMN list stored on ME need to be aborted(TRUE) or not(FALSE) */
extern boolean dsat_abort_plmn_listing;
#endif /* FEATURE_DSAT_ETSI_MODE */
extern dsat_global_state dsat_pending_state[];

#define AT_MODE_NONE 0

/*-------------------------------------------------------------------------
    Local functions:
-------------------------------------------------------------------------*/


extern void cmif_call_event_cb_func
(
  cm_call_event_e_type            event,            /* Event ID              */
  const cm_mm_call_info_s_type   *event_ptr         /* Pointer to Event info */
);

LOCAL void cmif_call_info_list_cb_func 
(
  const cm_call_state_info_list_s_type *list_ptr  /* Pointer to info list */
);

LOCAL void cmif_ss_cmd_cb_func 
(
  void                         *data_ptr,         /* Data block pointer    */
  cm_ss_cmd_e_type              cmd,              /* Command ID            */
  cm_ss_cmd_err_e_type          cmd_err           /* Command error code    */
);

extern void cmif_cm_ph_event_cb_func 
(
  cm_ph_event_e_type         event,            /* Event ID              */
  const cm_ph_info_s_type   *event_ptr         /* Pointer to Event info */
);

extern boolean cmif_is_gw_subscription_available
(
  sys_modem_as_id_e_type     subs_id,
  ds_at_cm_ph_event_type    *ph_event
);

LOCAL dsat_result_enum_type cmif_process_cm_reg_data
(
  const ds_at_cm_ss_event_type * event_ptr        /* DS event pointer */
);

#ifdef FEATURE_DSAT_EXTENDED_CMD
LOCAL void cmif_report_mode_3gpp2
(
  ds_at_ss_info_s_type *ss_info_ptr
);
#endif /* FEATURE_DSAT_EXTENDED_CMD */

#if defined(FEATURE_DSAT_SP_CMDS)
LOCAL void cmif_determine_spservice
(
  ds_at_ss_info_s_type *ss_info_ptr
);
#endif /* defined(FEATURE_DSAT_SP_CMDS) */

/*===========================================================================
FUNCTION  DSATCMIF_CALL_CMD_CB_EXTEN_FUNC

DESCRIPTION
  CM Call status command callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

=========================================================================== */
void dsatcmif_call_cmd_cb_exten_func 
(
  void                            *data_ptr,          /* Data block pointer    */
  cm_call_cmd_e_type               cmd,               /* Command ID            */
  cm_call_cmd_err_e_type           cmd_err,           /* Command error code    */
  cm_call_cmd_err_cause_e_type     call_cmd_err_cause,/* Cause of the error */
  cm_alpha_s_type                  alpha              /* Alpha value */
)
{
  ds_command_type            *cmd_buf;
  ds_at_cm_call_cmd_type *call_cmd = NULL;

  cmd_buf = dsat_get_cmd_buf(sizeof(ds_at_cm_call_cmd_type), FALSE);

  /* send the message to ATCOP */
  cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_CM_CALL_CMD;
  call_cmd = cmd_buf->cmd_payload_ptr;
  call_cmd->cmd = cmd;
  call_cmd->cmd_err = cmd_err;
  call_cmd->data_ptr = data_ptr;
/* As of now we are not using err_cause and alpha fields and it can be used for
extracting Cause of Error .Cause of Error to be used when cmd_err is other than CM_SUPS_CMD_ERR_NOERR */
  call_cmd->err_cause = (cm_sups_cmd_err_cause_e_type)call_cmd_err_cause;
  call_cmd->alpha.len = alpha.len;
  if(alpha.len > 0)
  {
  (void) dsatutil_memscpy((void*)&call_cmd->alpha.buf[0],
          CM_MAX_ALPHA_TAG_CHARS,(void*)&alpha.buf[0],MIN(alpha.len,CM_MAX_ALPHA_TAG_CHARS));     
  }

  ds_put_cmd(cmd_buf);
} /* dsatcmif_call_cmd_cb_exten_func  */

/*===========================================================================

FUNCTION  DSATCMIF_CM_CALL_EVENT_HANDLER

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
dsat_result_enum_type dsatcmif_cm_call_event_handler
(
  ds_at_cm_call_event_type *call_event, /* Call event pointer */
  dsat_result_enum_type     result
)
{
  dsati_mode_e_type                current_mode;
  dsatetsicall_call_state_info *call_val = NULL;

  current_mode = dsatcmdp_get_current_mode();
  call_val = dsat_get_base_addr(DSAT_CALL_VALS, TRUE);
  if (NULL == call_val)
  {
    return DSAT_ASYNC_EVENT;
  }

  switch (call_event->event)
  {
    case CM_CALL_EVENT_INFO:
      if(CHECK_PENDING(DSATETSI_EXT_ACT_CPAS_ETSI_IDX,0,DSAT_PENDING_TRUE))
      {
        /* Process phone activity results */
        SET_PENDING(DSATETSI_EXT_ACT_CPAS_ETSI_IDX ,0, DSAT_PENDING_FALSE);
        result = dsatetsime_report_cpas (dsatcmdp_at_state, call_event, NULL);
      }
      else
      {
        result = DSAT_ASYNC_EVENT;
      }
      break;

    case CM_CALL_EVENT_CONNECT:
#ifdef FEATURE_DSAT_ETSI_MODE
      /* ETSI builds */
      if ( IS_ETSI_MODE(current_mode) )
      {
        result = dsatetsicall_call_event_connect(call_event , result);
      }
#endif /* FEATURE_DSAT_ETSI_MODE */
      break;

    case CM_CALL_EVENT_END:
#ifdef FEATURE_DSAT_ETSI_MODE 
      if ( IS_ETSI_MODE(current_mode) )
      {
        result = dsatetsicall_call_event_end(call_event);
      }
#endif /* FEATURE_DSAT_ETSI_MODE */
      break;
    //TODO: Diff and review: sanjaykc
    default:
      break;
  }
  return result;
} /* dsatcmif_cm_call_event_handler*/



/*===========================================================================

FUNCTION  DSATCMIF_CM_CALL_CMD_HANDLER

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
dsat_result_enum_type dsatcmif_cm_call_cmd_handler
(
  ds_at_cm_call_cmd_type *call_cmd, /* Call command pointer */
  dsat_result_enum_type   result
)
{
  dsati_mode_e_type                current_mode;
  dsatetsicmif_sups_state_info *sups_val = NULL;

#ifdef FEATURE_DSAT_ETSI_MODE
  current_mode = dsatcmdp_get_current_mode();
#endif /* FEATURE_DSAT_ETSI_MODE */
  
  sups_val = dsat_get_base_addr(DSAT_SUPS_VALS, FALSE);

  switch (call_cmd->cmd)
  {
    case CM_CALL_CMD_INFO_GET:
      if (CM_CALL_CMD_ERR_NOERR != call_cmd->cmd_err)
      {
#ifdef FEATURE_DSAT_ETSI_MODE
        if ( IS_ETSI_MODE(current_mode) )
        {
          if( sups_val->dsat_interr_ss_service == clip || 
              sups_val->dsat_interr_ss_service == colp ||
              sups_val->dsat_interr_ss_service == allLineIdentificationSS
            )
          {
            sups_val->dsat_interr_ss_service = (ss_operation_code_T)DSAT_INVALID_OP_CODE;
            return dsat_send_cme_error(DSAT_CME_PHONE_FAILURE);
          }
          else 
          {
            DS_AT_MSG1_HIGH("Failed on CM call info list query: %d",
                    call_cmd->cmd_err);
            return dsat_send_cme_error(DSAT_CME_UNKNOWN);
          }
        }
        else
#endif /* FEATURE_DSAT_ETSI_MODE */
        {
          result = DSAT_ERROR;
        }
      }
      else
      {
        result = DSAT_ASYNC_CMD;
      }
      break;

    case CM_CALL_CMD_INFO_LIST_GET:

      if (CM_CALL_CMD_ERR_NOERR != call_cmd->cmd_err)
      {
        DS_AT_MSG1_HIGH("Failed on CM call info list query: %d",
                  call_cmd->cmd_err);
#ifdef FEATURE_DSAT_ETSI_MODE
        dsatetsime_cind_pending = FALSE;
#endif /* FEATURE_DSAT_ETSI_MODE */
#ifdef FEATURE_DSAT_ETSI_DATA
        dsat_pdp_state.pending = FALSE;
#endif /* FEATURE_DSAT_ETSI_DATA */
        result = DSAT_ERROR;
      }
      else
      {
        result = DSAT_ASYNC_CMD;
      }
      break;

    case CM_CALL_CMD_ORIG:
      /* Check for call cmd originator */
#ifdef FEATURE_DSAT_ETSI_DATA
      if ( IS_ETSI_MODE(current_mode) )
      {
        if (FALSE == dsat_pdp_state.pending)
        {
          /* Voice call */
          result = dsatvoice_cmd_orig_cb_handler(call_cmd->cmd_err);
        }
      }
      else
#endif /* FEATURE_DSAT_ETSI_DATA */
      {
        /* Voice call */
        result = dsatvoice_cmd_orig_cb_handler(call_cmd->cmd_err);
      }
      break;

    case CM_CALL_CMD_END:
      /* Check for call cmd originator */
      if ( FALSE == dsat_pdp_state.pending )
      {
        /* Voice call */
        result = dsatvoice_cmd_end_cb_handler(call_cmd->cmd_err);
      }
      break;
    case CM_CALL_CMD_SUPS:
#ifdef FEATURE_DSAT_ETSI_MODE
      if ( IS_ETSI_MODE(current_mode) )
      {
        result = dsatetsicall_sups_cmd_cb_handler(call_cmd);
      }
#endif /* FEATURE_DSAT_ETSI_MODE */
      break;
    default:
    break;
  }

  return result;
} /* dsatcmif_cm_call_cmd_handler */


/*===========================================================================

FUNCTION  DSATCMIF_CM_SS_CMD_HANDLER

DESCRIPTION
  This function is the handler function for the CM service system-related
  commands

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
dsat_result_enum_type dsatcmif_cm_ss_cmd_handler
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type result                = DSAT_ASYNC_CMD;
  ds_at_cm_ss_cmd_type*           ss_cmd = dsat_get_cmd_payload_ptr(cmd_ptr);

  switch (ss_cmd->cmd)
  {
#ifdef FEATURE_DSAT_LTE
  case CM_SS_CMD_GET_LTE_RRC_TX_INFO:
    if (CM_SS_CMD_ERR_NOERR != ss_cmd->cmd_err)
    {
      DS_AT_MSG1_HIGH("CM SS command error: %d",ss_cmd->cmd_err);
      SET_PENDING(DSAT_VENDOR_QCRATSTATE_IDX, 0, DSAT_PENDING_FALSE)
      result = DSAT_ERROR;
    }
    break;
  case CM_SS_CMD_SEND_GNSS_SESSION_STATUS:
    if (CM_SS_CMD_ERR_NOERR != ss_cmd->cmd_err)
    {
      DS_AT_MSG1_HIGH("CM SS command error: %d",ss_cmd->cmd_err);
      result = DSAT_ASYNC_EVENT;
    }
    break;
#endif /* FEATURE_DSAT_LTE */
  default:
    DS_AT_MSG1_HIGH("Unsupport CM SS command: %d",ss_cmd->cmd);
    result = DSAT_ERROR;
    break;
  }

  return result;
} /* dsatcmif_cm_ss_cmd_handler */

/*===========================================================================
FUNCTION  CMIF_SS_CMD_CB_FUNC

DESCRIPTION
  CM serving system status command callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
/* ARGSUSED */
LOCAL void cmif_ss_cmd_cb_func 
(
  void                         *data_ptr,         /* Data block pointer    */
  cm_ss_cmd_e_type              cmd,              /* Command ID            */
  cm_ss_cmd_err_e_type          cmd_err           /* Command error code    */
)
{
  ds_command_type * cmd_buf;
    
  /* Filter supported commands */
  switch (cmd)
  {
  case CM_SS_CMD_INFO_GET:
#ifdef FEATURE_DSAT_LTE
  case CM_SS_CMD_GET_LTE_RRC_TX_INFO:
  case CM_SS_CMD_SEND_GNSS_SESSION_STATUS:
#endif /* FEATURE_DSAT_LTE */
    /* Verify there is no error condition */
    if (CM_SS_CMD_ERR_NOERR != cmd_err)
    {
      ds_at_cm_ss_cmd_type *ss_cmd = NULL;

      /* Send error to DS task context */
      cmd_buf = dsat_get_cmd_buf(sizeof(ds_at_cm_ss_cmd_type), FALSE);

      /* send the message to ATCOP */
      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_CM_SS_CMD;
      ss_cmd = cmd_buf->cmd_payload_ptr;
      ss_cmd->cmd = cmd;
      ss_cmd->cmd_err = cmd_err;
      ds_put_cmd(cmd_buf);
    }
    break;
    
  default:
    /* Do nothing for unsupported commands */
    break;
  }

  return;
} /* cmif_ss_cmd_cb_func */

#ifdef FEATURE_DSAT_ETSI_MODE
/*===========================================================================

FUNCTION  DSATCMIF_CM_INBAND_CMD_CB_FUNC

DESCRIPTION
  This function is the callback function for the CM inband events 

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
 void dsatcmif_cm_inband_cmd_cb_func
(
  void*                     data_ptr,  /*  Not used.  */
  cm_inband_cmd_e_type      cmd,
  cm_inband_cmd_err_e_type  cmd_err
)
{
  if ( cmd_err != CM_INBAND_CMD_ERR_NOERR )
  {
    ds_command_type              *cmd_buf;
    ds_at_cm_inband_cmd_type     *inband_cmd = NULL;

    DS_AT_MSG2_ERROR(" CM Inband Cmd Err cmd=%x err=%x", cmd, cmd_err);

    cmd_buf  = dsat_get_cmd_buf(sizeof(ds_at_cm_inband_cmd_type), FALSE);

    cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_CM_INBAND_CMD;
    inband_cmd = cmd_buf->cmd_payload_ptr;
    inband_cmd->cmd = cmd;
    inband_cmd->cmd_err = cmd_err;
    ds_put_cmd( cmd_buf );
  }
} /* dsatcmif_cm_inband_cb_func */

/*===========================================================================

FUNCTION  DSATCMIF_CM_INBAND_CMD_HANDLER

DESCRIPTION
  This function is the handler function for the CM inband events related
  commands

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_ASYNC_EVENT :  if it is a success.
SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatcmif_cm_inband_cmd_handler
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type        result         = DSAT_ASYNC_EVENT;
  dsati_mode_e_type            current_mode   = dsatcmdp_get_current_mode();
  ds_at_cm_inband_cmd_type*    inband_cmd     = dsat_get_cmd_payload_ptr(cmd_ptr);

  if ( IS_ETSI_MODE(current_mode) )
  {
    switch (inband_cmd->cmd)
    {
      case CM_INBAND_CMD_START_CONT_DTMF:
      case CM_INBAND_CMD_STOP_CONT_DTMF:
        SET_PENDING(DSATETSI_EXT_ACT_VTS_ETSI_IDX ,0, DSAT_PENDING_FALSE)
        result = DSAT_ERROR;
        break;
      default:
        DS_AT_MSG1_HIGH("Unsupported CM command: %d", inband_cmd->cmd);
        result = DSAT_ERROR;
        break;
    }
  }

  return result;
}/*dsatcmif_cm_inband_event_handler*/

/*===========================================================================
FUNCTION  DSATCMIF_INBAND_EVENT_CB_FUNC

DESCRIPTION
  CM Inband event command callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
/*ARGSUSED*/
void dsatcmif_cm_inband_event_cb_func
(
 cm_inband_event_e_type        inband_event,
 const cm_inband_info_s_type*  inband_info_ptr
)
{
  ds_command_type * cmd_buf;

  ASSERT( inband_info_ptr != NULL );
  DS_AT_MSG2_HIGH("In inband_event_cb_func due to event %d Dtmf tone played %d",inband_event,inband_info_ptr->success);

  switch( inband_event )
  {
  case CM_INBAND_EVENT_REV_START_CONT_DTMF_CONF:
  case CM_INBAND_EVENT_REV_STOP_CONT_DTMF_CONF:
    if( CHECK_PENDING(DSATETSI_EXT_ACT_VTS_ETSI_IDX,0,DSAT_PENDING_TRUE))
    {
      ds_at_cm_inband_event_type*  inband_event_ptr = NULL;
      
      cmd_buf = dsat_get_cmd_buf(sizeof(ds_at_cm_inband_event_type), FALSE);
      /* send the event to ATCOP */
      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_CM_INBAND_INFO_CMD;
      inband_event_ptr = cmd_buf->cmd_payload_ptr;
      inband_event_ptr->event = inband_event;
      inband_event_ptr->event_info = inband_info_ptr;
      ds_put_cmd( cmd_buf );
    }
    break;
  default :
     /* To Please the Lint*/
    break;
  }
}/*dsatcmif_cm_inband_event_cb_func*/


/*===========================================================================

FUNCTION  DSATCMIF_CM_INBAND_EVENT_HANDLER

DESCRIPTION
  This function is the handler function for the CM info inband events related
  commands

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_ASYNC_EVENT :  if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatcmif_cm_inband_event_handler
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  dsat_timer_s_type *t_ptr;
  const cm_inband_info_s_type *event_info;
  ds_at_cm_inband_event_type*  inband_event = dsat_get_cmd_payload_ptr(cmd_ptr);

  if(!dsatcmdp_is_etsi_cmd())
  {
    return result;
  }
  
  DS_AT_MSG1_HIGH("In inband_event_handler due to event %d", inband_event->event);
  event_info = inband_event->event_info;
  
  switch( inband_event->event )
  {
    case CM_INBAND_EVENT_REV_START_CONT_DTMF_CONF:
      if(event_info->success)
      {
        DS_AT_MSG1_HIGH("Dtmf tone played %d",event_info->digits[0]);
        t_ptr = &dsatutil_timer_table[DSAT_INBAND_DTMF_RING];
		if ( ps_timer_is_running(t_ptr->timer_handle) == TRUE )
        {
          ps_timer_cancel(t_ptr->timer_handle);
          DS_AT_MSG0_HIGH(" In inband_event_handler due to event ");
        }
        ps_timer_start(t_ptr->timer_handle, DSAT_INBAND_DTMF_RING_DELAY );
      }
      else
      {
        DS_AT_MSG0_HIGH("DTMF Tone Could not be played");
        SET_PENDING(DSATETSI_EXT_ACT_VTS_ETSI_IDX ,0, DSAT_PENDING_FALSE)
        result = DSAT_ERROR;
      }
      break;
    case CM_INBAND_EVENT_REV_STOP_CONT_DTMF_CONF:   
      SET_PENDING(DSATETSI_EXT_ACT_VTS_ETSI_IDX ,0, DSAT_PENDING_FALSE)
      result = DSAT_OK;
    break;
    default:
      break;
  }

  return result;

} /* dsatcmif_cm_inband_event_handler */

#endif /* FEATURE_DSAT_ETSI_MODE */

/*===========================================================================

FUNCTION   CMIF_SUBSCRIPTION_AVAILABLE_PROCESS_COPS

DESCRIPTION
  This function processes cops after the subscription
  is available.

DEPENDENCIES
  None

RETURN VALUE
  dsat_result_enum_type

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type cmif_subscription_available_process_cops
(
  dsatcmif_servs_state_info *ph_val
)
{
  dsat_result_enum_type result = DSAT_ERROR;

  if ( ( DSAT_COPS_MODE_MANUAL== ph_val->net_pref.mode ) ||
       ( DSAT_COPS_MODE_MANAUTO == ph_val->net_pref.mode ))
  {
     result = dsatetsicmif_change_network_registration ( 
                          ph_val->net_pref.mode,
                          &ph_val->net_pref.plmn,
                          ph_val->net_pref.act);
  }
  else if ( DSAT_COPS_MODE_AUTO == ph_val->net_pref.mode )
  {
    result = dsatetsicmif_change_network_registration ( 
                             ph_val->net_pref.mode,
                             NULL,
                             ph_val->net_pref.act);
   }
   else
   {
      DS_AT_MSG0_ERROR(" Other modes should not invoke subcription available");
   }
              
   return result;
} /* cmif_subscription_available_process_cops */

/*===========================================================================

FUNCTION  DSATCMIF_CM_SS_EVENT_HANDLER

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
dsat_result_enum_type dsatcmif_cm_ss_event_handler
(
  dsat_cm_msg_s_type    *msg_ptr,
  dsat_result_enum_type  result
)
{
  dsat_result_enum_type   result_tmp = DSAT_ASYNC_EVENT;
  ds_at_cm_ss_event_type *event_ptr = NULL;

  if (NULL == msg_ptr)
  {
    DS_AT_MSG0_ERROR("Either msg ptr is NULL or No of Stack is 0: Ignored SS event processing");
    return result;
  }

  /*Assign  SS event info  pointer*/
  event_ptr = &msg_ptr->msg.ss_event;

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
                  result_tmp = cmif_process_cm_reg_data(event_ptr);
                }
                if ( event_ptr->event_info.changed_fields & DSAT_SS_MOBILITY_MGMT_MASK )
                {
                  (void)dsatetsicmif_process_cm_mm_info_data(&event_ptr->event_info);
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
      
    }
    break;

#ifdef FEATURE_DSAT_LTE
#ifdef FEATURE_DSAT_ATCOP_VEND

    case CM_SS_EVENT_LTE_RRC_TX_INFO:
    {
      DS_AT_MSG2_MED("LTE status %d LTE band %d",
	  	              msg_ptr->msg.ss_event.event_info.dsat_lte_tx_info.lte_tx_status,
                      msg_ptr->msg.ss_event.event_info.dsat_lte_tx_info.lte_tx_band);
      result = dsatvend_process_qcratstate_cmd( msg_ptr );
    }
    break;
#endif
#endif /* FEATURE_DSAT_LTE */

    default:
      DS_AT_MSG0_ERROR("Unexpected CM Serving System event");
      break;
  }

  return result;
} /* dsatcmif_cm_ss_event_handler */

#ifdef FEATURE_DSAT_LTE

#ifdef FEATURE_DSAT_ATCOP_VEND

/*===========================================================================

FUNCTION  DSATCMIF_GET_CM_SS_LTE_CPHY_TX_INFO  

DESCRIPTION
  This function issue query to Call Manager to get lte tx info

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
dsat_result_enum_type dsatcmif_get_cm_ss_lte_cphy_tx_info ( void )
{
  dsat_result_enum_type result = DSAT_OK;
  if (TRUE != cm_ss_cmd_get_lte_rrc_tx_info (
                                          cmif_ss_cmd_cb_func,
                                          NULL,
                                          dsatcm_client_id
                                          ))

  {
    DS_AT_MSG0_ERROR("Could not request CM SS lte cphy tx info");
    result = DSAT_ERROR;
  }
  return result;
} /* dsatcmif_get_cm_ss_info */


/*===========================================================================

FUNCTION  DSATCMIF_SET_CM_SS_GNSS_TX_ACTIVITY  


DESCRIPTION
  This function issue query to Call Manager to set gnss tx activity

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
dsat_result_enum_type dsatcmif_set_cm_ss_gnss_tx_activity ( boolean gnss_stat )
{
  dsat_result_enum_type result = DSAT_OK;
  if (TRUE != cm_ss_cmd_send_gnss_session_status (
                                          cmif_ss_cmd_cb_func,
                                          NULL,
                                          dsatcm_client_id,
                                          gnss_stat
                                         ))

  {
    DS_AT_MSG0_ERROR("Could not request CM SS lte cphy tx info");
    result = DSAT_ERROR;
  }
  return result;
}
#endif

#endif /* FEATURE_DSAT_LTE */

/*===========================================================================

FUNCTION  DSATCMIF_CHANGE_OPERATING_MODE

DESCRIPTION
  This function submits request to Call Manager to change the phone
  operating mode.  The results of the call are handled as asynch events.

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
dsat_result_enum_type dsatcmif_change_operating_mode
(
  sys_oprt_mode_e_type  new_mode            /* Operating mode */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_CMD;
  boolean cm_result;

  if (dsatcmif_get_oprt_mode() == new_mode)
  {
    DS_AT_MSG0_HIGH("Requested operating mode is same as current");
    return DSAT_OK;
  }

  DS_AT_MSG1_HIGH("Requesting CM operating mode change: %d",new_mode);
  cm_result = cm_ph_cmd_oprt_mode ( dsatcmif_ph_cmd_cb_func,
                                    NULL,
                                    dsatcm_client_id,
                                    new_mode );

  if (!cm_result)
  {
    /* ERROR tell user about it */
    result = DSAT_ERROR ;                
  }

  return result;
} /* dsatcmif_change_operating_mode */


/*===========================================================================

FUNCTION  DSATCMIF_CM_PH_CMD_HANDLER

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
dsat_result_enum_type dsatcmif_cm_ph_cmd_handler
(
  ds_at_cm_ph_cmd_type *ph_cmd, /* PH command pointer */
  dsat_result_enum_type result
)
{
#ifdef FEATURE_DSAT_ETSI_MODE
  dsati_mode_e_type                 current_mode = dsatcmdp_get_current_mode();
#endif /* FEATURE_DSAT_ETSI_MODE */
  dsatcmif_servs_state_info     *ph_val = NULL;
  dsatetsicall_call_state_info  *call_val  = NULL;
  dsatme_mmgsdi_state_info      *me_val    = NULL;

  ph_val = dsat_get_base_addr(DSAT_PH_VALS, TRUE);
  call_val = dsat_get_base_addr(DSAT_CALL_VALS, TRUE);
  me_val = dsat_get_base_addr(DSAT_MMGSDI_VALS, FALSE);

  switch (ph_cmd->cmd)
  {
  case CM_PH_CMD_SYS_SEL_PREF:
  case CM_PH_CMD_OPRT_MODE:
  case CM_PH_CMD_GET_NETWORKS:
  case CM_PH_CMD_INFO_GET:
    /* Verify there is no error condition */
    if (CM_PH_CMD_ERR_NOERR != ph_cmd->cmd_err)
    {
#ifdef FEATURE_DSAT_ETSI_MODE
      if ( IS_ETSI_MODE(current_mode) )
      {
        /* Clear flags on failure of attach write */
        if ( dsatcmif_attach_write != DSAT_CGATT_MAX )
        {
          dsatcmif_ss_evt_not_expected = FALSE;
          dsatcmif_attach_write = DSAT_CGATT_MAX;
        }
        if (NULL != ph_val)
        {
          if ( (ph_val->dsat_ws46_res_type != DSAT_WS46_NONE) && 
               (CM_PH_CMD_INFO_GET == ph_cmd->cmd) )
          {
            ph_val->dsat_ws46_res_type = DSAT_WS46_NONE;
            ph_val->dsat_ws46_curr_val = DSAT_WS46_FUNC_MAX;
          }
	}
        if ((CM_PH_CMD_SYS_SEL_PREF == ph_cmd->cmd) ||
            (CM_PH_CMD_GET_NETWORKS == ph_cmd->cmd))
        {
          if (NULL != ph_val)
          {
            if (DSAT_COPS_ASTATE_ABORT == ph_val->dsatetsicall_network_list.cmd_state)
            {
              /* Abort handler responsible for sending result code */
              /* COPS state will be updated by abort event handler */
              return DSAT_ASYNC_EVENT;
            }
            else
            {
              ph_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_NULL;
              ph_val->dsatetsicall_network_list.cmd_idx = CMD_IDX_NONE;
              result = DSAT_ERROR;
            }
          }
        }
      }/* IS_ETSI_MODE() */
#endif /* FEATURE_DSAT_ETSI_MODE */

      if (CHECK_PENDING(DSAT707_VEND_ACT_RESET_IDX, 0, DSAT_PENDING_RESET_WRITE))
      {
        SET_PENDING(DSAT707_VEND_ACT_RESET_IDX, 0, DSAT_PENDING_RESET_NONE);
        result = DSAT_ERROR;
      }
      if (CHECK_NOT_PENDING(DSAT_VENDOR_QCBANDPREF_IDX,0,DSAT_PENDING_PREFMODE_NONE))
      {
        SET_PENDING(DSAT_VENDOR_QCBANDPREF_IDX ,0, DSAT_PENDING_PREFMODE_NONE)
      }
      
      if(CHECK_NOT_PENDING(DSATETSI_EXT_ACT_CVMOD_ETSI_IDX ,0,DSAT_PENDING_FALSE) )
      {    
        SET_PENDING(DSATETSI_EXT_ACT_CVMOD_ETSI_IDX ,0, DSAT_PENDING_FALSE)
      }
      if (CM_PH_CMD_ERR_NOERR != ph_cmd->cmd_err)
      {
        if ( CHECK_NOT_PENDING(DSAT_EXT_CFUN_IDX,0,DSAT_PENDING_CFUN_NONE))
        {
          SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_NONE)
          if (CM_PH_CMD_INFO_GET == ph_cmd->cmd)
          {
            DSATUTIL_SET_VAL(DSAT_EXT_CFUN_IDX,0,0,DSAT_ME_FUNC_MAX,MIX_NUM_TYPE)
            result = dsat_send_cme_error (DSAT_CME_PHONE_FAILURE);
          }
          else if (CM_PH_CMD_OPRT_MODE == ph_cmd->cmd)
          {
            if (CM_PH_CMD_ERR_OPRT_MODE_S == ph_cmd->cmd_err)
            {
              result = dsat_send_cme_error (DSAT_CME_OP_NOT_SUPPORTED);
            }
            else
            {
              result = dsat_send_cme_error (DSAT_CME_PHONE_FAILURE);
            }
          }
        }
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
    break;

  case CM_PH_CMD_SET_PREFERRED_NETWORKS:
#ifdef FEATURE_DSAT_ETSI_MODE
    if ( IS_ETSI_MODE(current_mode) && dsat_abort_plmn_listing == TRUE)
    {
      /* User aborted +CPOL command */
      dsat_abort_plmn_listing = FALSE;

      /* Abort handler responsible for sending error */
      return DSAT_ASYNC_EVENT;
    }
#endif /* FEATURE_DSAT_ETSI_MODE */
    /* Deliberately falls through to case below */    
    break;
  case CM_PH_CMD_SUBSCRIPTION_AVAILABLE:
    if ((CM_PH_CMD_ERR_NOERR != ph_cmd->cmd_err) && (NULL != ph_val))
    {
      if ( CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S == ph_cmd->cmd_err )
      {
        if ( ph_val->net_pref.subscription_invoked == TRUE )
             /* To make sure this is not after phone boot up but after deregistration command */
        {
          if ( DSAT_COPS_MODE_AUTO == ph_val->net_pref.mode && 
		  	   DSAT_COPS_ASTATE_PREF_ONLY == ph_val->dsatetsicall_network_list.cmd_state )
          {
            /* Subscription is already available and in COPS=0 case return early.
               Ideally scenario should not hit as ATCOP reads subscrition available events already*/
            ph_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_NULL;
            ph_val->dsatetsicall_network_list.cmd_idx = CMD_IDX_NONE;
            ph_val->net_pref.last_dereg_success = FALSE;
            result = DSAT_OK;
          }
          else
          {
            result = cmif_subscription_available_process_cops(ph_val);
          }
          /* reset the net_pref structure and make the boolean false */
          ph_val->net_pref.subscription_invoked = FALSE; 
        }
      }
      else
      {
#ifdef FEATURE_DSAT_ETSI_MODE
      if ( IS_ETSI_MODE(current_mode) )
      {
        
        /* If there is an ERROR for SUBSCRIPTION AVAILABLE, then reset
           back subscription_invoked */
        if ( ph_val->net_pref.subscription_invoked == TRUE )
        {
          ph_val->net_pref.subscription_invoked = FALSE;
        }
        if ( DSAT_COPS_ASTATE_NULL == ph_val->dsatetsicall_network_list.cmd_state)
        {
          return DSAT_ASYNC_EVENT;
        }
        ph_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_NULL;
        ph_val->dsatetsicall_network_list.cmd_idx = CMD_IDX_NONE;
      }
#endif /* FEATURE_DSAT_ETSI_MODE */
      result = DSAT_ERROR;
    }
  }
    break;
  case CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE:
    if ( CM_PH_CMD_ERR_NOERR != ph_cmd->cmd_err && NULL != ph_val)
    {
      /* Error has occured while deregistration.  */
#ifdef FEATURE_DSAT_ETSI_MODE
      if ( IS_ETSI_MODE(current_mode) )
      {
        if( ( DSAT_COPS_ASTATE_NULL == ph_val->dsatetsicall_network_list.cmd_state ) ||
            ( DSAT_COPS_ASTATE_ABORT == ph_val->dsatetsicall_network_list.cmd_state) )
        {
          return DSAT_ASYNC_EVENT;
        }
        else
        {
          DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_COPS_ETSI_IDX,0,0,
             (dsat_num_item_type)ph_val->dsatetsicall_network_list.previous_pref.mode,MIX_NUM_TYPE)
        }
        ph_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_NULL;
        ph_val->dsatetsicall_network_list.cmd_idx = CMD_IDX_NONE;
      }
#endif /* FEATURE_DSAT_ETSI_MODE */
      result = DSAT_ERROR;
    }
    break;
/*#ifdef FEATURE_DSAT_EDRX
  case CM_PH_CMD_GET_EDRX_REQ:
  case CM_PH_CMD_SET_EDRX_REQ:
    if ( CM_PH_CMD_ERR_NOERR != ph_cmd->cmd_err )
    {
      SET_PENDING( DSATETSI_EXT_ACT_CEDRXS_ETSI_IDX, 0, DSAT_PENDING_CEDRXS_NONE );
      SET_PENDING( DSATETSI_EXT_ACT_CEDRXRDP_ETSI_IDX, 0, DSAT_PENDING_CEDRXRDP_NONE );
      result = DSAT_ERROR;
    }
    break;
#endif     FEATURE_DSAT_EDRX */

  default:
    DS_AT_MSG1_ERROR("Unsupport CM PH command: %d",ph_cmd->cmd);
    result = DSAT_ERROR;
    break;
  }
  
  return result;
} /* dsatcmif_cm_ph_cmd_handler */



/*===========================================================================
FUNCTION  DSATCMIF_CM_PH_EVENT_HANDLER

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
dsat_result_enum_type dsatcmif_cm_ph_event_handler
(
  ds_at_cm_ph_event_type *ph_event, /* PH event pointer */
  dsat_result_enum_type   result
)
{
  dsat_mixed_param_val_type       *temp_mix_type;
  dsati_mode_e_type                current_mode = dsatcmdp_get_current_mode();
  dsatcmif_servs_state_info    *ph_val = NULL;
  dsatetsicall_call_state_info *call_val = NULL;

  ph_val = dsat_get_base_addr(DSAT_PH_VALS, TRUE);
  call_val = dsat_get_base_addr(DSAT_CALL_VALS,TRUE);

  if ((NULL == ph_val) || (NULL == call_val))
  {
    return DSAT_ASYNC_EVENT;
  }
  

  switch(ph_event->event)
  {
    case CM_PH_EVENT_OPRT_MODE:
    {
        if (TRUE == ph_event->event_info.oprt_mode_changed)
        {
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
          /* Operating mode event updates +CFUN parameter */
          DS_AT_MSG3_HIGH("current CFUN = %d, received state = %d, is_card_action mode =%d",
                          (dsat_num_item_type)dsatutil_get_val(DSAT_EXT_CFUN_IDX,0,MIX_NUM_TYPE),
                          ph_event->event_info.oprt_mode,
                          is_card_action );

          temp_mix_type = (dsat_mixed_param_val_type *)dsatutil_get_val(
                                        DSAT_EXT_CFUN_IDX,0,MIX_INDEX_TYPE);
          if (FALSE == dsatme_convert_phone_state
                        (TRUE, &ph_event->event_info.oprt_mode,
                        (dsat_me_func_e_type*)&(temp_mix_type->num_item) ))
          {
            if ( CHECK_NOT_PENDING(DSAT_EXT_CFUN_IDX,0,DSAT_PENDING_CFUN_NONE) )
            {
              SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_NONE)
              result = DSAT_ERROR;
            }
          }
        }

        /* Check for report requirement */
        if (  CHECK_NOT_PENDING(DSAT_EXT_CFUN_IDX,0,DSAT_PENDING_CFUN_NONE) )
        {
#if defined(FEATURE_DSAT_CFUN_CARD_POWER_CTL) && defined (FEATURE_DSAT_ETSI_MODE)
          switch (dsat_pending_state[PORT_ZERO].cmd_pending[0])
          {
            case DSAT_PENDING_CFUN_WRITE_MIN:
            {
              /*
                        * Do card operation when EFS enabled and Received mode is equal to requested mode
                        */
              if (is_card_action == TRUE)
              {
                if( DSAT_ME_FUNC_MIN == (dsat_num_item_type)dsatutil_get_val(
                                            DSAT_EXT_CFUN_IDX,0,MIX_NUM_TYPE))
                {
                  SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_NONE)
                  result = dsatme_process_cfun_card_pdown_cmd();
                }
                else
                {
                  /*Will wait for next operating mode change event(Hint: SGLTE case)*/
                  result = DSAT_ASYNC_CMD;
                }
              }
              else
              {
                SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_NONE)
                result = DSAT_OK;
              }
           }
           break;
           case DSAT_PENDING_CFUN_WRITE_FULL:
           {
             if (is_card_action == TRUE)
             {
               if( DSAT_ME_FUNC_FULL == (dsat_num_item_type)dsatutil_get_val(
                                               DSAT_EXT_CFUN_IDX,0,MIX_NUM_TYPE))
               {
                 result = dsatme_process_cfun_card_pup_cmd(TRUE);
               }
               else
               {
                 /*Will wait for next operating mode change event(Hint: SGLTE case)*/
                 result = DSAT_ASYNC_CMD;
               }
             }
             else
             {
               SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_NONE);
               result = DSAT_OK;
             }
           }
           break;
          case DSAT_PENDING_CFUN_WRITE_RFOFF:
          case DSAT_PENDING_CFUN_WRITE_FTM:
          case DSAT_PENDING_CFUN_WRITE_RESET:
          case DSAT_PENDING_CFUN_WRITE_OFFLINE:
          {
            SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_NONE)
            result = DSAT_OK;
            break;
          }
          case DSAT_PENDING_CFUN_WRITE:
            break;
          default:
            break;
        }
#else
        SET_PENDING(DSAT_EXT_CFUN_IDX ,0, DSAT_PENDING_CFUN_NONE)
        result = DSAT_OK;
#endif /* FEATURE_DSAT_CFUN_CARD_POWER_CTL */
        }

#ifdef FEATURE_FSK_TX
      if ( (CHECK_PENDING( DSAT_VENDOR_QCFSKSTART_IDX, 0, DSAT_PENDING_TRUE )  ||
             CHECK_PENDING( DSAT_VENDOR_QCFSKDBG_IDX, 0, DSAT_PENDING_TRUE )) && 
           ( ph_event->event_info.oprt_mode == SYS_OPRT_MODE_LPM ) )
      {
        if(ph_val->is_gw_subscription_available)
        {
          DS_AT_MSG0_HIGH(" FSK start/dbg - subs_not_available command invoked");     
          (void) cm_ph_cmd_subscription_not_available3 ( dsatcmif_ph_cmd_cb_func,
                                                NULL,
                                                dsatcm_client_id,
                                                CM_SUBSCRIPTION_STATUS_NO_CHANGE,
                                                CM_SUBSCRIPTION_STATUS_CHANGE,
                                                CM_SUBSCRIPTION_STATUS_NO_CHANGE,
                                                CM_SUBSCRIPTION_STATUS_NO_CHANGE);
          result = DSAT_ASYNC_CMD;
        }
        else
        {
          result = dsatvend_send_fsk_start();
        }
      }
      else if ( CHECK_PENDING( DSAT_VENDOR_QCFSKSTOP_IDX, 0, DSAT_PENDING_TRUE ) && 
                ( ph_event->event_info.oprt_mode == SYS_OPRT_MODE_ONLINE ) )
      {
        if(!ph_val->is_gw_subscription_available)
        {
          DS_AT_MSG0_HIGH(" FSK stop - subs_available command invoked");     
          (void) cm_ph_cmd_subscription_available ( dsatcmif_ph_cmd_cb_func,
                                      NULL,
                                      dsatcm_client_id,
                                      CM_SUBSCRIPTION_STATUS_NO_CHANGE,
                                      CM_SUBSCRIPTION_STATUS_CHANGE,
                                      CM_SUBSCRIPTION_STATUS_NO_CHANGE);
          result = DSAT_ASYNC_CMD;
        }
        else
        {
          SET_PENDING(DSAT_VENDOR_QCFSKSTOP_IDX, 0, DSAT_PENDING_FALSE);
          result = DSAT_OK;
        }
      }
#endif /* FEATURE_FSK_TX */
      break;
    }
    
#ifdef FEATURE_DSAT_ETSI_MODE 
    case CM_PH_EVENT_SYS_SEL_PREF:
    {
      if( (DSAT_COPS_ASTATE_PREF == ph_val->dsatetsicall_network_list.cmd_state) ||
          (DSAT_COPS_ASTATE_PREF_ONLY == ph_val->dsatetsicall_network_list.cmd_state) )
      {
        result = dsatetsicall_cops_async_handler(ph_val->dsatetsicall_network_list.cmd_state, ph_event);
      }
      
      if ( (dsatcmif_attach_write != DSAT_CGATT_MAX) && dsatcmif_ss_evt_not_expected )
      {
        dsatcmif_attach_write = DSAT_CGATT_MAX;
        dsatcmif_ss_evt_not_expected = FALSE;

        result = DSAT_OK;
      }  
	  
#ifdef FEATURE_DSAT_ATCOP_VEND
      if ( CHECK_PENDING(DSAT_VENDOR_QCBANDPREF_IDX,0,DSAT_PENDING_PREFMODE_BANDPREF_WRITE) )
      {
        /* BandPref setting completed Fill the result to return Write status.*/
        SET_PENDING(DSAT_VENDOR_QCBANDPREF_IDX ,0, DSAT_PENDING_PREFMODE_NONE)
        result = dsatvend_process_prefmode_cmd(DSAT_PENDING_PREFMODE_BANDPREF_WRITE,
                           &ph_event->event_info.pref_info);
      }
#endif
      if ( CHECK_PENDING(DSATETSI_EXT_ACT_CVMOD_ETSI_IDX ,0,DSAT_PENDING_TRUE) )
      {  
        SET_PENDING(DSATETSI_EXT_ACT_CVMOD_ETSI_IDX ,0, DSAT_PENDING_FALSE)
        result = DSAT_OK;
      }
      break;
    }
    case CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE:
    {
#ifdef FEATURE_FSK_TX
      if ( (CHECK_PENDING( DSAT_VENDOR_QCFSKSTART_IDX, 0, DSAT_PENDING_TRUE ) ||
             CHECK_PENDING( DSAT_VENDOR_QCFSKDBG_IDX, 0, DSAT_PENDING_TRUE )) && 
             ( FALSE == cmif_is_gw_subscription_available(0, ph_event) ) )
      {
        result = dsatvend_send_fsk_start();
      }
      else 
      {
#endif  /* FEATURE_FSK_TX */
        if (FALSE == cmif_is_gw_subscription_available(0, ph_event))
        {
          /*Update the COPS status accordingly */
          if (DSAT_COPS_ASTATE_DEREG == ph_val->dsatetsicall_network_list.cmd_state)
          {
            result = DSAT_OK;
          }
          else if (DSAT_COPS_ASTATE_NULL != ph_val->dsatetsicall_network_list.cmd_state)
          {
            result = DSAT_ERROR;
          }

          ph_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_NULL;
          ph_val->dsatetsicall_network_list.cmd_idx = CMD_IDX_NONE;
        }
#ifdef FEATURE_FSK_TX
      }
#endif /* FEATURE_FSK_TX */
      break;
    }
    case CM_PH_EVENT_SUBSCRIPTION_AVAILABLE:
    {
#ifdef FEATURE_FSK_TX
      if ( CHECK_PENDING( DSAT_VENDOR_QCFSKSTOP_IDX, 0, DSAT_PENDING_TRUE ) && 
             ( TRUE == cmif_is_gw_subscription_available( 0, ph_event) ) )
      {
        SET_PENDING(DSAT_VENDOR_QCFSKSTOP_IDX, 0, DSAT_PENDING_FALSE);
        result = DSAT_OK;
      }
      else 
      {
#endif  /* FEATURE_FSK_TX */
        if (TRUE == cmif_is_gw_subscription_available( 0, ph_event))
        {
          if ( ph_val->net_pref.subscription_invoked == TRUE && DSAT_COPS_MODE_AUTO != ph_val->net_pref.mode ) 
             /* To make sure this is not after phone boot up
                but after deregistration command */
          {
            result = cmif_subscription_available_process_cops(ph_val);
            /* reset the net_pref structure and make the boolean false */
            ph_val->net_pref.subscription_invoked = FALSE;
          }
          if ( ( DSAT_COPS_ASTATE_NULL == ph_val->dsatetsicall_network_list.cmd_state)||
               ( DSAT_COPS_ASTATE_ABORT == ph_val->dsatetsicall_network_list.cmd_state) )
          {
            result = DSAT_ASYNC_EVENT;
          }
          else if ( DSAT_COPS_MODE_AUTO == ph_val->net_pref.mode && 
                    DSAT_COPS_ASTATE_PREF_ONLY == ph_val->dsatetsicall_network_list.cmd_state && 
                    ph_val->net_pref.subscription_invoked == TRUE )
          {
             /* Check for the current network selection after Subscription available.
             If it is AUTOMATIC we can return early otherwise initiate ph cmd1 for AUTO case as well*/
            if ( ph_event->event_info.pref_info.network_sel_mode_pref
                                           != CM_NETWORK_SEL_MODE_PREF_AUTOMATIC )
            {
              result = cmif_subscription_available_process_cops(ph_val);
            }
            else
            {
              ph_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_NULL;
              ph_val->dsatetsicall_network_list.cmd_idx = CMD_IDX_NONE;
              result = DSAT_OK;
            }		
            ph_val->net_pref.subscription_invoked = FALSE;
          }
        }
#ifdef FEATURE_FSK_TX
      }
#endif  /* FEATURE_FSK_TX */
      break;
    }
#endif /* FEATURE_DSAT_ETSI_MODE */
    case CM_PH_EVENT_INFO:
    {
#ifdef FEATURE_DSAT_ETSI_MODE 
      if ( ph_val->dsat_ws46_res_type != DSAT_WS46_NONE )
      {
        result = dsatact_process_ws46_cmd (&ph_event->event_info.pref_info);
      }
#endif /* FEATURE_DSAT_ETSI_MODE */
      
      if(CHECK_NOT_PENDING(DSAT_EXT_CFUN_IDX,0,DSAT_PENDING_CFUN_NONE))
      {
        DS_AT_MSG3_HIGH(" dsat_cfun_res_type = %d oprt mode = %d operating mode = %d", 
                                                 dsat_pending_state[0].cmd_pending[0],
                                                 ph_event->event_info.oprt_mode, 
                                                 current_mode );
        result = dsatme_process_cfun_cmd (ph_event->event_info.oprt_mode, (dsat_me_func_e_type)NULL);
      }
      else
      {
         /* This is during Init unless we miss CM_PH_EVENT_OPRT_MODE event */
        DS_AT_MSG2_HIGH(" dsat_cfun_val_type = %d Recvd oprt mode = %d ",
                                     (dsat_num_item_type)dsatutil_get_val(DSAT_EXT_CFUN_IDX,0,MIX_NUM_TYPE),
                                      ph_event->event_info.oprt_mode);
        temp_mix_type = (dsat_mixed_param_val_type *)dsatutil_get_val(
                                                      DSAT_EXT_CFUN_IDX,0,MIX_INDEX_TYPE);
 
        (void)dsatme_convert_phone_state (TRUE, &ph_event->event_info.oprt_mode, 
                                            (dsat_me_func_e_type*)&(temp_mix_type->num_item) );
		
#ifdef FEATURE_DSAT_ATCOP_VEND
        if ( CHECK_PENDING(DSAT_VENDOR_QCBANDPREF_IDX,0,DSAT_PENDING_PREFMODE_BANDPREF_READ) )
        {
          SET_PENDING(DSAT_VENDOR_QCBANDPREF_IDX ,0, DSAT_PENDING_PREFMODE_NONE)
          result = dsatvend_process_prefmode_cmd( DSAT_PENDING_PREFMODE_BANDPREF_READ,
                                           &ph_event->event_info.pref_info);
        }
        if ( CHECK_PENDING(DSAT_VENDOR_QCBANDPREF_IDX,0,DSAT_PENDING_PREFMODE_BANDPREF_VERIFY) )
        {
          /* BandPref setting needs two phases 1. get the current mode 
                    now set the band pref after validation */
          SET_PENDING(DSAT_VENDOR_QCBANDPREF_IDX ,0, DSAT_PENDING_PREFMODE_BANDPREF_WRITE)
          result = dsatvend_process_prefmode_cmd(DSAT_PENDING_PREFMODE_BANDPREF_VERIFY,
                                           &ph_event->event_info.pref_info);
        }
#endif
      }
      break;
    }
    
#ifdef FEATURE_ETSI_ATTACH
    case CM_PH_EVENT_PS_ATTACH_FAILED:
    {
      /* Attach timeout event handling */
      if ( dsatcmif_attach_write != DSAT_CGATT_MAX )
      {
        dsatcmif_attach_write = DSAT_CGATT_MAX;
        /* Report +CME error asynchronously */
        return dsat_send_cme_error(DSAT_CME_NO_NETWORK_SERVICE);
      }
      break;
    }
#endif /* FEATURE_ETSI_ATTACH */
  
#ifdef FEATURE_DSAT_ETSI_MODE
    case CM_PH_EVENT_AVAILABLE_NETWORKS_CONF:
    {
      if (DSAT_COPS_ASTATE_ANETS == ph_val->dsatetsicall_network_list.cmd_state)
      {
        result = dsatetsicall_cops_async_handler (ph_val->dsatetsicall_network_list.cmd_state, ph_event);
      }
#ifdef FEATURE_DSAT_ATCOP_VEND
     else if(DSAT_COPS_ASTATE_NULL == ph_val->dsatetsicall_network_list.cmd_state)     
     {                 
        result = dsatetsicall_qccellscan_async_handler(ph_event);
     }
#endif
      break;
    }
      
    case CM_PH_EVENT_TERMINATE_GET_NETWORKS:
    {
      if( ph_val->dsatetsicall_network_list.cmd_state == DSAT_COPS_ASTATE_ANETS )
      {
        DS_AT_MSG0_ERROR("Received Terminate GET_NETWORKS event from CM");
        ph_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_NULL;
        ph_val->dsatetsicall_network_list.cmd_idx = CMD_IDX_NONE;
        result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
      }
      break;
    }
#endif /* FEATURE_DSAT_ETSI_MODE */
  
 
#ifdef FEATURE_DSAT_EDRX
    /*case CM_PH_EVENT_GET_EDRX_CNF:
    {
    
    DS_AT_MSG0_HIGH("Processing event: OPRT mode");
      if ( CHECK_PENDING( DSATETSI_EXT_ACT_CEDRXRDP_ETSI_IDX, 0, DSAT_PENDING_CEDRXRDP_READ ) )
      {
        result = dsatme_process_cedrx_cmd ( DSAT_PENDING_CEDRXRDP_READ, &ph_event->event_info );
      }
      break;
    }
    case CM_PH_EVENT_SET_EDRX_CNF:
    {
      if ( CHECK_PENDING( DSATETSI_EXT_ACT_CEDRXS_ETSI_IDX, 0, DSAT_PENDING_CEDRXS_WRITE ) )
      {
        result = dsatme_process_cedrx_cmd ( DSAT_PENDING_CEDRXS_WRITE, &ph_event->event_info );
      }
      break;
    }*/
    case CM_PH_EVENT_EDRX_CHANGE_INFO_IND:
    {
      ph_val->dsat_edrx_info.edrx_enabled = ph_event->event_info.edrx_info.edrx_enabled;
      ph_val->dsat_edrx_info.rat_type_edrx = ph_event->event_info.edrx_info.rat_type_edrx;
      ph_val->dsat_edrx_info.edrx_cycle_length = ph_event->event_info.edrx_info.edrx_cycle_length;
      ph_val->dsat_edrx_info.edrx_ptw = ph_event->event_info.edrx_info.edrx_ptw;
	  
      if ((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_ACT_CEDRXS_ETSI_IDX,
                                         0,MIX_NUM_TYPE) == 2 && 
         !CHECK_PENDING(DSATETSI_EXT_ACT_CEDRXRDP_ETSI_IDX, 0, DSAT_PENDING_CEDRXRDP_READ))
      {
        result = dsatme_process_cedrx_cmd ( DSAT_PENDING_CEDRXS_URC, &ph_event->event_info );
      }
      break;
    }
#endif /* FEATURE_DSAT_EDRX */

    default:
    { 
      DS_AT_MSG1_ERROR("Unsupported CM PH event: %d", ph_event->event);
      break;
    }
  }
  return result;
}/* dsatcmif_cm_ph_event_handler */

/*===========================================================================

FUNCTION  DSATCMIF_INIT_CMIF

DESCRIPTION
  CM Interface initialization function. This function does the following:
  - Registers the client call back functions.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmif_init_cmif ( void )
{
  dsat_mixed_param_val_type    *temp_mix_type;
  dsatcmif_servs_state_info *ph_val = NULL;  
  
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  cm_client_status_e_type cm_status = CM_CLIENT_OK;
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
  ph_val = dsat_get_base_addr(DSAT_PH_VALS, TRUE);
  
  if (NULL != ph_val)
  {
    temp_mix_type = (dsat_mixed_param_val_type *)dsatutil_get_val(
                                                   DSAT_EXT_CFUN_IDX,0,MIX_INDEX_TYPE);

    (void)dsatme_convert_phone_state (TRUE, &ph_val->dsat_oprt_mode,
                                     (dsat_me_func_e_type*)&(temp_mix_type->num_item));
  }
  
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
  /* Registration for INBAND EVENTS */
   cm_status = (cm_client_status_e_type)
					((int32)cm_status | 
					 (int32)cm_mm_client_inband_reg(
							   dsatcm_client_id,
							   dsatcmif_cm_inband_event_cb_func,
							   CM_CLIENT_EVENT_REG,
							   CM_INBAND_EVENT_REV_START_CONT_DTMF,
							   CM_INBAND_EVENT_REV_STOP_CONT_DTMF_CONF,
							   NULL )); 
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

  return;
} /* dsatcmif_init_cmif */

/*===========================================================================

FUNCTION  DSATCMIF_DEINIT_CMIF

DESCRIPTION
  CM Interface deinitialization function. This function does the following:
  - Deregisters the client call back functions.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmif_deinit_cmif ( void )
{
  /* Deregister from DSSYSMGR to get CM events */
  dssysmgr_client_dereg_event(DSSYSMGR_CLIENT_ATCOP);

  return;
} /* dsatcmif_deinit_cmif */

/*===========================================================================
FUNCTION  CMIF_CALL_INFO_LIST_CB_FUNC

DESCRIPTION
  CM Call info list query callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer
===========================================================================*/
LOCAL void cmif_call_info_list_cb_func
(
  const cm_call_state_info_list_s_type    *list_ptr
)
{
  ds_command_type * cmd_buf;
  ds_at_cm_call_event_type*  call_event = NULL;
  dsat_cm_call_state_info_list_s_type *info_list_ptr = NULL;
  uint8 i;

  DS_ASSERT (list_ptr != NULL);

  DS_AT_MSG1_MED("Call info list cb: number of active calls = %d",
                 list_ptr->number_of_active_calls);

  cmd_buf       = dsat_get_cmd_buf(sizeof(ds_at_cm_call_event_type), FALSE);
  /* send the message to ATCOP */
  cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_CM_CALL_INFO_CMD;
  call_event = cmd_buf->cmd_payload_ptr;

  info_list_ptr = &call_event->event_info.dsat_cm_call_info_list;
  info_list_ptr->number_of_active_calls = list_ptr->number_of_active_calls;
  for (i = 0; i < list_ptr->number_of_active_calls; i++)
  {
    DS_AT_MSG3_MED("call_id = %d, call_type = %d, call_state = %d",
                   list_ptr->info[i].call_id, list_ptr->info[i].call_type,
                   list_ptr->info[i].call_state);

    info_list_ptr->info[i].call_id    = list_ptr->info[i].call_id;
    info_list_ptr->info[i].call_type  = list_ptr->info[i].call_type;
    info_list_ptr->info[i].call_state = list_ptr->info[i].call_state;
    info_list_ptr->info[i].call_subst = list_ptr->info[i].call_subst;
  }

  call_event->event_info.dsat_info_list_avail = TRUE;
  call_event->event =  CM_CALL_EVENT_INFO;
  ds_put_cmd(cmd_buf);
} /* cmif_call_info_list_cb_func */


/*===========================================================================

FUNCTION  CMIF_PROCESS_CM_REG_DATA

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
LOCAL dsat_result_enum_type cmif_process_cm_reg_data
(
  const ds_at_cm_ss_event_type * event_ptr        /* DS event pointer */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  dsatcmif_servs_state_info  *ph_val = NULL;
  
  if (NULL == event_ptr)
  {
    return result;
  }

  ph_val = dsat_get_base_addr(DSAT_PH_VALS, FALSE);

#ifdef FEATURE_DSAT_ETSI_MODE
  /* Check for active +COPS command */
  DS_AT_MSG2_HIGH("COPS: state=%d reg_mode=%d",
            ph_val->dsatetsicall_network_list.cmd_state,
            ph_val->dsatetsicall_network_list.requested_pref.mode);

  /* Process registration data for CREG and generate URC */
  /* Do not report if +COPS is underway as the lower layers do a few
   * transitions before things settle down. */
  if (DSAT_COPS_ASTATE_NULL != ph_val->dsatetsicall_network_list.cmd_state ||
      ph_val->dsat_net_reg_state.dsat_creg_reported.pending_cell_info_update == TRUE)
  {
    dsatetsicall_update_creg_lac_cell_id(event_ptr,
                                         &ph_val->dsat_net_reg_state.dsat_creg_reported.cops_pending_report_creg);
  }
  if ( ( DSAT_COPS_ASTATE_WAIT == ph_val->dsatetsicall_network_list.cmd_state ) ||
       ( DSAT_COPS_ASTATE_WAIT_2 == ph_val->dsatetsicall_network_list.cmd_state ) )
  {
    /* 
       Match of req_id will confirm the associated SS event to look for.
     */
    if(event_ptr->event_info.cm_req_id == DSAT_CM_REQ_ID)
    {
      DS_AT_MSG1_HIGH ("REQ_ID: %d Match",event_ptr->event_info.cm_req_id);

      {
        switch( event_ptr->event_info.srv_status )
        {
          case SYS_SRV_STATUS_PWR_SAVE:
          case SYS_SRV_STATUS_NO_SRV:
          case SYS_SRV_STATUS_LIMITED:
          case SYS_SRV_STATUS_LIMITED_REGIONAL:
          if( DSAT_COPS_MODE_MANAUTO ==
              ph_val->dsatetsicall_network_list.requested_pref.mode )
          {
    /* Change to auto registration on mismatch. Set COPS mode accordingly */
            DS_AT_MSG0_HIGH ("COPS manual reg PLMN mismatch, changing to automatic");
            ph_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_PREF;
            /* in case of QCCOPS plmn need to be paased. will try to camp on passed PLMN first 
               in auto mode*/
            result =
              dsatetsicmif_change_network_registration( DSAT_COPS_MODE_AUTO,
                                                        NULL,
                                                        DSAT_COPS_ACT_AUTO );

            DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_COPS_ETSI_IDX,0,0,
                                                 DSAT_COPS_MODE_AUTO,MIX_NUM_TYPE)
          }
          /* For manual, mismatch means no requested network service */
          else if ( DSAT_COPS_MODE_MANUAL ==
                    ph_val->dsatetsicall_network_list.requested_pref.mode )
          {
            DS_AT_MSG0_ERROR ("COPS manual reg NO SRV");
            ph_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_NULL;
            ph_val->dsatetsicall_network_list.cmd_idx = CMD_IDX_NONE;
            result = dsat_send_cme_error(DSAT_CME_NO_NETWORK_SERVICE);
          }
          /* Otherwise, asynch command processing is done */
          else
          {
            ph_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_NULL;
            ph_val->dsatetsicall_network_list.cmd_idx = CMD_IDX_NONE;
            DS_AT_MSG0_HIGH ("COPS automatic reg NO SRV");
            result = DSAT_OK;
           }
           break;
          case SYS_SRV_STATUS_SRV:
            if( DSAT_COPS_MODE_MANAUTO ==
               ph_val->dsatetsicall_network_list.requested_pref.mode )
            { 
              /* Update COPS state based on the current network state.*/ 
              dsat_cops_mode_e_type curr_cops_mode = (dsat_cops_mode_e_type)
                ph_val->dsat_net_reg_state.cmph_pref.network_sel_mode_pref;
              DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_COPS_ETSI_IDX,0,0,
                                 (dsat_num_item_type)curr_cops_mode,MIX_NUM_TYPE)
            }
            DS_AT_MSG0_HIGH ("COPS processing complete.");
            ph_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_NULL;
            ph_val->dsatetsicall_network_list.cmd_idx = CMD_IDX_NONE;
            result = DSAT_OK;
            break;
          default:
           DS_AT_MSG0_HIGH ("Unknown state");
        }
      }
    }
    else
    {
      if(ph_val->net_pref.mode == DSAT_COPS_MODE_DEREG)
      {
        DS_AT_MSG0_HIGH ("COPS dereg processing complete.");
        ph_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_NULL;
        ph_val->dsatetsicall_network_list.cmd_idx = CMD_IDX_NONE;
        result = DSAT_OK;
      }
    }
  }
  else if (DSAT_COPS_ASTATE_ABORT == ph_val->dsatetsicall_network_list.cmd_state)
  {
    /* Asynch command processing is done */
    ph_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_NULL;
    ph_val->dsatetsicall_network_list.cmd_idx = CMD_IDX_NONE;
    
    /* Abort handler responsible for sending result code */
    result = DSAT_ASYNC_EVENT;
  }

#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_ETSI_ATTACH 
  /* Activate request should not bail out on Camped domain or No Service
     should rather wait till power save is received */
  if((dsatcmif_attach_write == DSAT_CGATT_ACTIVATE )&&
     (( SYS_SRV_DOMAIN_CAMPED == event_ptr->event_info.srv_domain)||
      ( SYS_SRV_STATUS_NO_SRV == event_ptr->event_info.srv_status )))
  {
    DS_AT_MSG0_HIGH("Waiting for POWER SAVE (or) Service available");
    return DSAT_ASYNC_EVENT;
  }

  /* For +CGATT command, check to see if we are in write operation */
  if ( dsatcmif_attach_write != DSAT_CGATT_MAX )
  {
    /* Only CGATT = 1 Pass cases are handled here; Failure case is reported via 
       ATTACHED_FAILED event*/
    if(dsatcmif_attach_write == DSAT_CGATT_ACTIVATE)
    {
      if((event_ptr->event_info.srv_domain == SYS_SRV_DOMAIN_CS_PS) ||
         (event_ptr->event_info.srv_domain == SYS_SRV_DOMAIN_PS_ONLY) )
      {
        DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CGATT_ETSI_IDX,0,0,1,NUM_TYPE)
        dsatcmif_attach_write = DSAT_CGATT_MAX;
        result = DSAT_OK; 
      }
    }
    else if( (dsatcmif_attach_write == DSAT_CGATT_DEACTIVATE) && 
             ( ( event_ptr->event_info.changed_fields ) & 
               ( DSAT_SS_SRV_DOMAIN_MASK |
                 DSAT_SS_SRV_STATUS_MASK ) ) )
    {
      if((event_ptr->event_info.srv_domain == SYS_SRV_DOMAIN_CS_ONLY) ||
         (event_ptr->event_info.srv_domain == SYS_SRV_DOMAIN_NO_SRV) )
      {
        DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CGATT_ETSI_IDX,0,0,0,NUM_TYPE);
        dsatcmif_attach_write = DSAT_CGATT_MAX;
        result = dsatetsicmif_deactivate_all_contexts();
      }
    }

  }
#endif /* FEATURE_ETSI_ATTACH */
  return result;
} /* cmif_process_cm_reg_data*/

#ifdef FEATURE_DSAT_EXTENDED_CMD
/*===========================================================================

FUNCTION  CMIF_REPORT_MODE_3GPP2

DESCRIPTION
  This function reports 3GPP2 system mode when it is changed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmif_report_mode_3gpp2
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

  ph_val = dsat_get_base_addr(DSAT_PH_VALS, TRUE);
  if (NULL == ph_val)
  {
    return;
  }
   sys_mode = ph_val->dsat_sys_mode;

  /* If sys mode is changed and report an indication */
  if ( (ph_val->dsat_sys_prev_mode != sys_mode) && 
       ( ( SYS_SYS_MODE_NO_SRV == sys_mode ) ||
         ( SYS_SYS_MODE_CDMA == sys_mode ) ||
         ( (sys_sys_mode_e_type)DSAT_SYS_HYBRID == sys_mode ) ||
         ( SYS_SYS_MODE_HDR == sys_mode ) ) )
  {
    /* need to report the system node status*/
    res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, TRUE);
    if (NULL != res_buff_ptr)
    {
      res_buff_ptr->used = (word)snprintf ((char*)res_buff_ptr->data_ptr,
                                           res_buff_ptr->size,
                                           "^MODE:%d",
                                           sys_mode);
      dsatcmdp_send_urc(res_buff_ptr, DSAT_COMPLETE_RSP);
    }

    ph_val->dsat_sys_prev_mode = sys_mode; 
  }

  return;
} /* cmif_report_mode_3gpp2 */
#endif /* FEATURE_DSAT_EXTENDED_CMD */

