/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
               M U L T I M O D E   C O N T R O L L E R ( MMoC )

                 I M P L E M E N T A T I O N    F I L E

GENERAL DESCRIPTION
  This module is the main implementation for the Multi-Mode Controller
  (MMoC) task. The MMoC is responsible for facilitating the switch between
  different modes, initializing the system determination (SD) module and
  interfacing with SD and various protocols to handle the operating mode
  change, subscription change and other commands. It also provides a task
  context for SD reselection timer.

  The MMoC task has 2 seperate message queues, command queue for messages
  from CM and report queue for messages from the various protocols.

EXTERNALIZED FUNCTIONS

  Command Interface:
    mmoc_cmd_subscription_chgd
    mmoc_cmd_oprt_mode_chgd
    mmoc_cmd_pref_sys_chgd
    mmoc_cmd_get_networks_gw
    mmoc_cmd_get_term_networks_gw
    mmoc_cmd_term_get_networks_wlan
    mmoc_cmd_term_get_networks_wlan
    mmoc_cmd_wakeup_from_pwr_save

  Report Interface:
    mmoc_rpt_prot_deactd_cnf
    mmoc_rpt_prot_auto_deactd_ind
    mmoc_rpt_ph_stat_chgd_cnf
    mmoc_rpt_prot_gen_cmd_cnf

  Functional Interface:
    mmoc_proc_prot_gen_cmd

  Others:
    mmoc_task

REGIONAL FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The MMoC task should be started before MMoC APIs are called.


Copyright (c) 2002 - 2013 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/mmoc/src/mmoc.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/31/14   rm      Adding debug buffers 
01/15/14   jvo     Fix compilation warning in
                   mmoc_process_subsc_chgd_ph_stat_enter()
12/20/13   jvo     Changed failure to queue message router message to
                   ERR_FATAL in mmoc_msgr_send_else_err_fatal()
10/04/13   jvo     MMoC always calls MMOC_ERR_FATAL after sanity timeout in
                   mmoc_print_sanity_err_fatal()
09/05/13   jvo     Replaced calls to MMOC_PRINT_MSG_HIGH with mmocdbg
                   functions
08/28/13   jvo     Cleanup remaining Fusion pieces
07/19/13   jvo     Remove all MDM/MSM Fusion features
07/21/13   skk     FR2921: Network initiated Authentication Procedure Support
                   during emergency call When SIM is perso Locked
07/15/13   ss      FR1860 - T/G on DSDS
06/19/13   gm      Send pref to SD-ALL stacks before ONLINE/PWRUP ind
06/14/13   gm      EFRAT ind should be sent on subscription change event
06/13/13   jqi     Add of TTL markers to prfile the boot up time.
06/11/13   dk      replace ERR_FATAL_REPORT with ERR_FATAL
05/22/13   qf      Handle auto_deact_ind while in lte->do redir trans
05/15/13   jqi     Memory optimization for suspend command processing.
03/15/13   gm      Do not register for STOP_SIG with RCINIT
03/15/13   gm      UT fix for sending pref only on MAIN at ONLINE
03/09/13   aj      Allow FTM reactivation after subsc processing
02/27/13   ss      Using New Watchdog Heartbeat Interface instead of the old Watchdog
02/21/13   cl      Re-initialize SD core during power-down event
01/25/13   gm      Avoid duplicate initialization of SD
02/03/13   cl      FR1975: Dynamic Switch SVLTE <-> CSFB
01/25/13   gm      FR 3059 - ONLINE optimization - skip duplicate subscription
09/27/12   vm      Removing DIME featurization and cleanup
09/20/12   vm      Check if MMOC is already in suspend during suspend processing
09/12/12   ag/mg   Update service state in CM to NO SRV when LTE gets deactivated by MMOC
08/08/12   vm     Removing processing for rf initialization as cm handles it
05/30/12   ab      MMOC is recording abort from HDRCP when MMOC is in
                   MMOC_TRANS_STATE_WAIT_AUTO_DEACTD_IND
05/18/12   xs      Reset redir sanity timer
05/10/12   cl      Send resume report to CM regardless the HDR stack state
05/02/12   rk      Moving the funxtionality of setting SD context to 1XCP
                   upon HO_IND to MMOC
05/01/12   gm      Add offline notify send from MMOC
04/20/12   sk      Fixing compiler warnings
04/17/12   cl      Remove software-switch-9 usage on hybr-2 stacks
04/17/12   xs      Convert MMOC BSR timer to ms in LTE->DO redir
04/17/12   sk      Donot allow operating mode switches after PWR_OFF operating mode command is processed.
04/16/12   gm      Add new init before task for RCINIT.
11/11/11   fj      Fix KW errors.
03/28/12   xs      Extend MMOC timer to BSR timer in LTE->DO redir
03/24/12   gm      DIME integration support - fix to compilation errors
03/21/12   ab      Added support for handling Auto deact ind
                   whem MMOC is waiting for open session confirm
02/15/12   vk      Replace usage of AEE library function with corresponding
                   CoreBSP library
11/30/11   jh      Added feature for dynamic rat acq order change
02/12/12   aj      send suspend ind to CM even for duplicate requests
01/17/12   aj/cl   SC SVLTE SO 33 changes
01/11/12   xs      Remove MMOC_TRANS_PWR_SAVE_EXIT transaction state
01/11/12   aj      Call sd_init only once during subsc chgd transaction
01/10/12   gm      Mainline HDR API changes integration
12/15/11   sg      Add TDS band pref to mmoc_cmd_get_networks_gw().
11/02/11   ak      Enable manual PLMN search.
11/02/11   gm      Changes for HDR API cleanup
10/31/11   ak      Recover from miscommunication between HDRCP and MMOC.
10/21/11   aj      update HYBR2 stack to ONLINE state when oprt mode is ONLINE
09/14/11   vk      New MMOC transaction "MMOC_TRANS_DEACT_1XCSFB_CMD" related changes.
08/27/11   aj      Clear LTE srv status during LTE-DO redir
08/19/11   fc      Fix featurization issue.
08/10/11   rn      Handle DO redire abort during offline/power down
06/10/11   rn      Qshrink2 changes
08/10/11   aj      clear MMOC transaction on getting IRAT in non-ONLINE mode
08/11/11   xs      Remove rflib.h
08/04/11   xs      Replace FEATURE_MMODE_NIKEL with FEATURE_MMODE_SC_SVLTE
08/02/11   aj      update sanity expiry debug messages for GWL deact cnf state
07/22/11   xs      MMOC SC_SVLTE 1.0 merging to Mainline
07/13/11   nk      Updated stubs in UT environment to be in sync with QTF.
07/06/11   aj      Enable postprocessing for HDR to LTE resel
06/10/11   xs      Change order to deactivate active protocol first
05/20/11   ak      Updating the buffering mechanism in MMOC to kick blocked
                   RAT when permission rcvd in SVLTE II.
05/16/11   xs      Added another stop mode rquest to deactivate NAS in suspend state
05/14/11   rk      Passing user mode preference from CM to SD to update RAT
                   priority list
05/12/11   mp      BPLMN - LTE Only capable is not able to perform Manual Search
05/04/11   mp      Enhanced MMOC sanity timer expiry function for GWL.
05/04/11   rk      Passing CDMA time related info to SD during handover from
                   LTE to CDMA
05/04/11   rk      revert change for IRAT indication to domain selection
05/03/11   rk      Indicate about redirection to domain selection on getting
                   the redirection indication itself
04/25/11   sv      Remove including test_mc.h
04/14/11   cl      MSM Crash due to prot-state mis-sycnh when 1X service lost
04/09/11   cl/ak   Current state from the mmoc_info_ptr instead of gen_cmd.
04/05/11   sv      Ind SD to unlock CDMA, if locked, when going ONLINE
04/04/11   ak      DIfferentiate hdr_acq_blocked for HICPS and MMSS
04/01/11   xs      After move to MMOC_TRANS_STATE_WAIT_PS_DETACH_CNF
                   MMOC transaction should return MMOC_EVT_STATUS_CONSUMED
03/25/11   xs      Extend MMOC sanity timer to 82 sec for PS detach operation
03/09/11   cl      Add support for SVLTE2 MMSS arbitration
02/28/11   aj      Add wait for activation_cnf state in DO to LTE reselection
02/23/11   rk      Passing correct sid and nid received in HO complete ind
02/02/11   rm      Adding LTE RAT-B changes
02/08/11   aj      Update SD regarding HDR deactivation due to GWL acq
01/13/11   aj      send correct LTE band pref to SD after 1x subscription chgd
01/11/11   am      MUTE migration to qtf changes
01/05/10   rk      Send CM_LTE_1X_IRAT_F report to CM before entering into
                   protocol deactivatio transaction state
12/22/10   rm      Fixing lint errors
12/10/10   rk      MO/MT 1XCSFB call successful scenarios support
12/02/10   gm      Fix to MOB compiler warnings.
11/30/10   aj      Revert change to inform SD about HDR deactivation
11/24/10   xs      Fusion featurization clean up
11/29/10   rm      Added missing 'break' in wake-up scenario
11/15/10   rm      Memory optimization changes
11/10/10   am      Changes for MUTE to MOB migration
11/06/10   aj      DO -> LTE reselection code
11/05/10   sv/gm   Changes to wait for session open confirmation
10/27/10   rk      Reverting support for deativating dormant protocol
10/28/10   gm      Fix to compiler warning.
10/27/10   rk      Adding support for deativating dormant protocol through MMOC
10/15/10   ak      Featurizing the call to err_reset_modem API.
10/08/10   ak      Disable error handling on reset in MSM.
10/05/10   pm      Remove clk.h & FEATURE_USE_TIME_VU
10/01/10   ak      Merged the Fusion code from Dev branch.
09/27/10   xs      Fixed target compilation errors
09/22/10   aj      Update SD regarding HDR deactivation due to GWL acq
09/17/10   cl      Fixing lint error
09/17/10   cl      Adding support for dynamic watch dog
08/02/10   nm      Removed EXTERN
07/14/10   mh      Fixed compile warnings
07/06/10   gm      Fix to Klocwork deferred critical errors.
07/05/10   sg      CM should send SIM_AVAILBLE_REQ to NAS, even if UE goes to
                   power_save mode during subscription change.
07/08/10   sv      Added mmoc_cmd_msm_sys_info_chgd handling
06/10/10   ak      Mainlining memory optimization changes.
06/10/10   cl/ak   Update MMOC state machine to ignore postprocessing when receiving
                   autodeactivate while waiting for gen_cmd_cnf
05/24/10   mp      Added support for session close and
                   MMOC_TRANS_MMGSDI_INFO_IND transaction to avoid nested callbacks
05/04/10   mh      Removed RF device from RF API
04/27/10   rn      Send CM_LTE_DO_IRAT_F to CM to indicate LTE->eHRPD IRAT
03/21/10   mp      MLPL/MSPL/PLMN Interface for MMSS 3.1 support
03/19/10   am      Fixing Compilation Warning
02/11/10   rn      LTE-1x redirection/reselection
01/28/10   mp      EF_EPRL Implementation
01/19/10   rn      Updated code for LTE-eHRPD redirection
01/12/10   mh      Fixed memory leak related to power save
01/06/10   ay      LTE support : subscription for GW
12/31/09   ay      Added LTE support
12/28/09   rn      LTE-eHRPD Redirection support
12/08/09   fj      Added lte_band_pref.
12/04/09   sg      Added a counter "mmoc_modem_heap_blocks" to keep track of memory
                   buf allocated through new API modem_mem_alloc()/modem_mem_free().
12/02/09   mh      Added LTE support when FEATURE_UIM_RUIM is defined
11/09/09   sg      Add non-demand paging macro to function that are not to be
                   demand-paged10/30/09   sv      Updating RF API as part of CMI change.
10/21/09   mh      Added code for GW/LTE Support
10/13/09   sg      ZI memory reduction. Mmode should call new Heap Manager API to
                   allocate memory dynamically.
07/30/09   fj      Removed FEATURE_LTE_PENDING for 2G/3G UT environment.
07/13/09   np      Added code for LTE support
08/21/09   rm      Including uim_v.h
08/14/09   rm      Making bio.h related changes
05/19/09   rm      Mainlining of FEATURE_DIAG_NON_STREAMING
05/05/09   aj      When MMOC and protocol are not in sync,print error
                   instead of crashing.
04/13/09   aj      Change behavior to process auto deact rpt while waiting for
                   ph stat chgd cnf or deact_cnf
03/16/09   rm      Removed inclusion of hdrmc.h. Added hdrmc_v.h.
02/25/09   sv      Removing inclusion of cmxll_v.h
02/23/09   rm      Removing inclusion of cmxll.h and sd.h
                   Including mc_v.h
02/06/09   aj      WLAN subscription decoupling
02/05/09   aj      Modify mmoc_is_hdr_acq_allowed() to allow hdr acq in DED_MEAS mode
12/22/08   rn      Added support for TMC memory utilization
10/17/08   sv      Corrected featurization MMOC_LOW_MEM_TARGET to
                   FEATURE_MMOC_LOW_MEM_TARGET.
10/09/08   fj      Fixed Klocwork defects
09/19/08   jd      Memory reduction changes
09/12/08   fj/aj   Added support for Modem Reset
09/04/08   sv      Implemented WPS support for WLAN
09/05/08   ak      Fixing compiler warnings.
08/25/08   ak      Fixing compiler warnings.
08/20/08   st      Resolve Klockwork defects
07/30/08   sv      Added FEATURE_HICPS_STACK_SYNC_UP to buffer the dropped
                   HDR acq
06/16/08   sv      Fixed Klocwork defects
04/28/08   fj      Lint cleanup, CR 144389
03/22/08   ic      Undo of "Replace custmmode.h with modem_mmode_cust.h" and
                   "FEATURE_MMODE_CUSTMMODE_H with FEATURE_MODEM_MMODE_CUST_H"
01/31/08   ic      Replace custmmode.h with custmmode.h
                   Replace FEATURE_MMODE_CUSTMMODE_H with FEATURE_CUSTMMODE_H
01/31/08   cl      Added in FEATURE_MMOC_DETECT_HDR_HW to support hardware
                   run-time detection.
09/11/07   pk      Added support for FEATURE_DISABLE_DEEP_SLEEP
08/20/07   ic      Lint cleanup
04/16/07   rp      Corrected handling of FTM mode
03/19/07   ka      Replacing banned apis
01/31/07   ka      Adding new state in MMOC to wait for TMC to ack operating
                   mode change.
01/04/07   pk      Added support for VoIP -> 1X Handoff
11/30/06   rp      Added support for background BPLMN search in limited service.
11/15/06   sk      Added hybrid GW support.
08/29/06   sk      Added WLAN Adhoc support.
04/24/06   ka      Featurizing ftm.h under GW flags.
04/03/06   ka      Changing copyright.
12/30/05   ka      Adding string maps to mmoc commands, reports and state
                   transitions.
11/28/05   ka      Fixing klockwork errors.
07/27/05   ka      Fixing implementation of common_hdr_deactd_cnf()
07/21/05   ka      Adding MMOC_DUP_CMD_REM_SIG to the list of signals
                   that mmoc task waits for.
07/07/05   ka      Request to activate GW on main when HDR is online in
                   hybrid instance proceeds with request to deactivate hdr.
                   On hdr deactivate cnf GW acquisition gets sent.
06/27/05   ic      Fixed RVDS 2.1 compiler warnings
06/17/05   ka      Rejecting Generic cmd to activate HDR hybrid instance
                   when GW is on Main instance.
                   Fixing issue where sd_ss_act_get() was called for Auto
                   deactivation indication from HDR hybrid instance resulting
                   in HDR hybrid staying online (sanity error fatal).
03/28/05   ka      Correcting Lint errors.
03/02/05   ka      Handling subscription available along with protocol name
                   that has subscription available.
02/19/05   dk      Fixed MMOC Pref_sys_cmd processing. MMOC does not queue
                   pref_sys_chgd_cmd for WLAN SS if WLAN feature is not
                   enabled.
01/12/05   sj      Fixed issue of mmoc activating ss-main after reset
                   because of SS timer expiry after processing RESET cmd.
12/21/04   sj      Fixed lint errors.
12/03/04   sj      Updated to issue standby sleep only after all instances
                   enter pwr_save.
09/27/04   ajn     Created an mmoc_timer_group for MMOC timers.
08/20/04   sj      Added support for prl_pref.
08/13/04   sj      Do not err_fatal if online command is received after
                   poweroff.
08/06/04   sj      Converted calls to _true_curr_act to sd_ss_act_get().
08/04/04   sj      For subsc_changed or sd_inited case, call user_pwr_up ind.
07/29/04   sj      Do not reset sd_initialized flag after LPM or FTM.
06/21/04   jqi     Use sd nam selection indication when there is a
                   subscription change.
06/07/04   sj      Send CM_STANDBY_SLEEP_F report upon entering power save.
05/25/04   jqi     Fixed mode preference reset issue after power up.
05/10/04   sj      Merged in changes for srch inactive state.
04/22/04   sj      Fixed issue of not starting pwr_save timer when next act
                   is also PWR_SAVE for SS-Main.
03/18/04   jqi     Used the true act type from SD System selection.
                   Replaced diag_enter_panic_mode with
                   ERR_FATAL_REPORT().
03/28/04   sj      Do not move srch to START upon getting ONLINE in LPM.
03/12/04   sj      Do not place srch in standby if entering pwr save from
                   HDR.
03/01/04   sj      Changed the order of deactivating protocols, SS_HDR first
                   followed by SS_MAIN.
02/24/04   sj      Removed LINT warnings.
01/13/03   sj      Merged in duplicate pref_sys_chgd cmd detection code.
                   Send HDR unlock RF command if 1x doesn't respond for
                   sometime.
                   Fixed sanity err_fatal when trans_state is AUTO_DEACT_IND.
10/22/03   sj      Send GPS_MODE_EXIT_F to CM before activating other
                   protocols.
09/15/03   sj      Fixed issues with HDR <-> 1x runtime switching.
08/02/03   dyc     Support for FEATURE_MULTI_RX_CHAIN
07/20/03   sj      Added hybrid support.
06/05/03   ht      Removed extra comma
07/08/03   sj      Changes for thin_ui support.
06/18/03   sj/ck   Vote off UIM power ctl after processing subscription
                   available.
05/14/03   sj      Fixed compiler error when FEATURE_FACTORY_TESTMODE is
                   not defined.
04/24/03   sj      Added support for early power scan.
03/18/03   sj      Process online even if subscription is not present.
03/18/03   lcl     Added reference to zrf library.
03/10/03   SJ      Added support for runtime FTM in leopard.
03/06/03   JQ/SJ   Moving phone to offline if rf init fails.
03/05/03   SJ      Fixed bug dring handling reset command wrt SD term.
02/26/03   SJ      Modified sanity timeout err_fatal to print the offending
                   protocol task's name instead of mmoc.c.
02/24/03   SJ      Handling GET_NET_GW in activate_prot_online().
02/24/03   SJ      Sending ONLINE command to TMC during online processing.
02/21/03   SJ      Calling sd_ss_ind_user_pwr_down() before powering down
                   the phone.
02/19/03   SJ      Handling subsc_chgd command only in powerup or online.
02/14/03   SJ      Added new MMOC API for wakeup from power save.
02/11/03   SJ/JQI  Added support for runtime FTM.
02/05/03   PK      Fixed compiler problem (missing end comment terminator).
02/05/03   SJ      Temporarily turning off rfr & rfl before activating GW.
01/23/03   SJ      Calling nam_sel SD indication only if the subsc_chgd CM
                   cmd was received, otherwise invoke pwr_up SD indication.
01/16/03   SJ      Updated _send_cm_rpt() to use cm_sd_rpt_u_type always.
01/16/03   SJ      Featurized DB accesses for non 1x targets.
12/19/02   SJ      Fixed activate_prot_offline for targets not supporting
                   CDMA or AMPS.
12/18/02   SJ      Added more debug messages.
12/06/02   SJ      Added fix to move protocol to same state after processing
                   subscription changed command for FTM/OFFLINE oprt mode.
11/22/02   SJ      Modified call to SD user_ss_pref -> user_ss_pref2().
09/17/02   SJ      Updated to send CM report on completion of LPM processing.
07/24/02   SJ      Cleared rex signal before processing event.
06/10/02   SJ      Cleared rex signal after processing event.
06/07/02   SJ      Renamed PROT_TRANS_NONE_PENDING to PROT_TRANS_NONE.
03/28/02   SJ      Initial release.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h" /* Customer configuration file */
#ifndef FEATURE_CUSTMMODE_H
#error Need to pull in custmmode.h!!! /* Make sure custmmode.h is pulled in */
#endif

#include "comdef.h"   /* Definition for basic types and macros             */
#include "msg.h"      /* Message output services.                          */
#include "cmd.h"      /* Command header for inter-task communication.      */
#include <stdio.h>    /* For calling sprintf.                               */

#include "sd_v.h"
#include "prot.h"     /* Common header for mmoc <-> protocol communications*/
#include "prot_v.h"
#include "mmoc.h"     /* External interface definitions for MMoC.          */
#include "mmoci.h"    /* Internal interface definitions for MMoC.          */
#ifndef FEATURE_MMOC_UNIT_TEST
#include "DDISoftwareSwitches.h" /* Interface for GPIO &other bit i/o services */
#include "DALSys.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"
#endif
#include "task.h"     /* Interface for global task resources.              */
#include "dog_hb_rex.h"    /*Interface for the new heartbeat watchdog services*/

#include "cm.h"       /* Interface to CM client service.                   */
#include "rex.h"
#include "cm_v.h"
#include "cm_i.h"
#include "sd_v.h"


#if defined(MMOC_GWL_SUPPORTED)
#include "cmregprx.h" /* Interface for RegProxy module.                    */
#include "mm.h"       /* Interface for MM module.                          */
#endif 



#ifdef FEATURE_UIM
#include "uim.h"
#include "uim_v.h"
#endif

#ifdef FEATURE_NV_RUIM
#include "nvruimi.h"
#endif

#include "event.h"    /* Interface for system event report service.        */
#include "modem_mem.h"   /* interface for modem_mem_alloc */
#include "cmll_v.h"

#ifdef MMOC_FTM_SUPPORTED
#include "ftm.h"
#endif /* MMOC_FTM_SUPPORTED */


#if defined(RF_MULTIMODE) || defined(FEATURE_MULTIMODE_RFAPI)
#include "rfm.h"      /* Interface for multimode RF APIs                  */
#endif /* defined(RF_MULTIMODE) || defined(FEATURE_MULTIMODE_RFAPI)  */


#include "mmocdbg.h"  /* For mmocdbg_print_message support */

#ifdef FEATURE_MMOC_UNIT_TEST
#error code not present
#endif

#ifndef FEATURE_MMOC_UNIT_TEST
static DalDeviceHandle  *switches_handle;
static DALResult         switches_dal_attach;
#endif /* FEATURE_MMOC_UNIT_TEST */

#ifdef MMODE_ADDITIONAL_DEBUG_INFO
static uint8           curr_rpt_idx = 0;
#endif

#include <string.h>

#include <stringl/stringl.h>


#ifdef FEATURE_MMODE_QTF
#error code not present
#endif /* FEATURE_MMODE_QTF */
#include "rcevt_sigosal.h"

/* Header file Time Test Lite (TTL) framework for profiles modem boot up time
*/
#include "ttl_map.h"
#include "policyman.h"
#include "subs_prio.h"
#include "mm.h"

#ifdef CM_DEBUG
#error code not present
#endif
/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== DEPENDENT FEATURES ===========================
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC Forward declarations ==============================
=============================================================================
=============================================================================
===========================================================================*/

/* Dog Variables - Handle for DOG HB interface */
#ifndef MMOC_DEBUG
#undef DOG_MMOC_RPT
#define DOG_MMOC_RPT mmoc_dog_rpt_var
static dog_report_type mmoc_dog_rpt_var             = 0;        /*Initial Safety*/
#endif

static void mmoc_reg_multimode_sanity_err_fatal( mmoc_sanity_err_e_type reason);

extern rex_tcb_type *cm_tcb_ptr;
static void set_mmoc_tcb(void);

/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* List of modes for which the phone status changed command has to be sent.
*/
static mmoc_mode_info_s_type ph_stat_cmd_tbl= {
#ifdef MMOC_GWL_SUPPORTED
    "NAS(REG Task)",  mmoc_reg_multimode_sanity_err_fatal,PROT_TRANS_NONE 
 #else
    "NAS(REG Task)",  NULL,                            PROT_TRANS_NONE 
 #endif
};


/* MMoC's task info.
*/

static mmoc_task_info_s_type           mmoc_task_info;

/* Local buffer to printing debug messages.
*/
#ifdef FEATURE_MODEM_HEAP_PHASE_2
static char*                           mmoc_debug_str;
#else
static char                            mmoc_debug_str[256];
#endif

/* Static data structure used to store state related variable in MMOC
*/
static mmoc_state_info_s_type   mmoc_state_info;

/* Timer group which will not be disabled during sleep.
** This group ensures MMOC can wake the mobile up from deep sleep.
*/
timer_group_type                mmoc_timer_group;

#ifdef FEATURE_MODEM_HEAP_PHASE_2
#define MMOC_MAX_CHAR   256
#endif

/* No. of blocks currently allocated from the MODEM heap.
*/
/*lint -esym(765, mmoc_modem_heap_blocks)
** Accessed by unit test cases.
*/
/*lint -esym(552, mmoc_modem_heap_blocks)
** Doesn't consider MMOC_MSG_HIGH as an access.
*/
uint8 mmoc_modem_heap_blocks;


#ifdef MMODE_ADDITIONAL_DEBUG_INFO
/* Global variable for mmoc debug buffer
*/
mmoc_debug_buffer_s_type  mmoc_debug_buffer;
#endif

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC IMPLEMENATATION RELATED HELPER FUNCTIONS ==========
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_mem_free

DESCRIPTION
   This function returuns the specified memory back to the Heap.

   If the memory came from MMOC heap then it is returned there else
   it is returned to the TMC heap.
   If feature FEATURE_MMOC_MODEM_HEAP is defined then it return the
   memory to the main heap.

DEPENDENCIES

   MMOC and TMC heap must have been initialized.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void mmoc_mem_free(

  void *ptr

)
{
  if(ptr != NULL)
  {
  MMOC_MSG_HIGH_2("MMOC free: addr = %x, Heap Used = %d", ptr, mmoc_modem_heap_blocks);
      mmoc_modem_heap_blocks--;
  /* Deallocate the memory to heap */
  modem_mem_free(ptr, MODEM_MEM_CLIENT_MMODE);
  }
  else
  {
	  return;
  }

} /* mmoc_mem_free() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_get_task_info_ptr

DESCRIPTION
  This function will return the pointer to the MMoC's task info struct.

DEPENDENCIES
  None.

RETURN VALUE
  mmoc_task_info_s_type* - Pointer to the local definition of MMoC's task
                           info struct.
  Note! Cannot be NULL.

SIDE EFFECTS
  None.
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
static mmoc_task_info_s_type*          mmoc_get_task_info_ptr
(

       void

)
{
  /* Return the pointer to the MMoC's task info.
  */
  return &mmoc_task_info;
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_get_state_info_ptr

DESCRIPTION
  This function will return the pointer to the MMoC's state info struct.

DEPENDENCIES
  None.

RETURN VALUE
  mmoc_state_info_s_type* - Pointer to the local definition of MMoC's state
                            info struct.
  Note! Cannot be NULL.

SIDE EFFECTS
  None.
===========================================================================*/
mmoc_state_info_s_type*  mmoc_get_state_info_ptr(void)
{
  return &mmoc_state_info;
}

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_init_state_info

DESCRIPTION
  This function will initialize the MMoC state info to the power up state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_init_state_info
(

       mmoc_state_info_s_type          *mmoc_info_ptr
          /* Pointer to MMoC's state information.
          */
)
{

  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize the state info.
  */
  mmoc_info_ptr->is_gwl_subs_avail       = FALSE;
  mmoc_info_ptr->curr_oprt_mode          = PROT_OPRT_MODE_NULL;
  mmoc_info_ptr->true_oprt_mode          = SYS_OPRT_MODE_NONE;
  mmoc_info_ptr->curr_trans              = MMOC_TRANS_NULL;
  mmoc_info_ptr->trans_state             = MMOC_TRANS_STATE_NULL;
  mmoc_info_ptr->trans_id                = 0;
  mmoc_info_ptr->prot_state  = PROT_STATE_NULL;
  
  mmoc_info_ptr->is_sd_initialized       = FALSE;
  mmoc_info_ptr->is_sd_init_called       = FALSE;
  mmoc_info_ptr->exit_fn_ptr             = NULL;
  mmoc_info_ptr->is_standby_sleep        = FALSE;
  mmoc_info_ptr->prot_subsc_chg          = PROT_SUBSC_CHG_NONE;
  mmoc_info_ptr->trans_info.gen_prot_cmd.is_activate_main = FALSE;
  
  
  mmoc_info_ptr->max_sanity_time_multiple = 1;

  mmoc_info_ptr->last_stop_req_sent_timestamp = 0;
  
  mmoc_info_ptr->is_gwl_deact_sent = FALSE;
  
  /* Initialize suspend state to FALSE (not suspended)*/
  mmoc_info_ptr->is_suspend      = FALSE;

  mmoc_info_ptr->is_buffer_auto_deact_ind    = FALSE;

  mmoc_info_ptr->buffered_auto_deact      = NULL;


  mmoc_info_ptr->main_last_prot_state = PROT_STATE_NULL;

  mmoc_info_ptr->emerg_susp_ss_mask = 0;

  

}

#ifdef MMODE_ADDITIONAL_DEBUG_INFO
/*===========================================================================

FUNCTION mmoc_dbg_buf_init_rpt_queue

DESCRIPTION
  Initialize the report queue for current transaction in debug_buffer
  This is done to ensure that reports last stored at the same index
  are cleared.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                            mmoc_dbg_buf_init_rpt_queue
(
       uint8       mmoc_dbg_buf_idx
)
{
  int i;
  
  for( i = 0 ; i < MMOC_MAX_RPT_COUNT; i++ )
  {
    mmoc_debug_buffer.dbg_buf[mmoc_dbg_buf_idx].rpt_queue[i].rpt_name = 
                                                                  MMOC_RPT_MAX;
    mmoc_debug_buffer.dbg_buf[mmoc_dbg_buf_idx].rpt_queue[i].task_name = 
                                                           MMOC_TASK_NAME_NONE;
   

    mmoc_debug_buffer.dbg_buf[mmoc_dbg_buf_idx].rpt_queue[i].prot_state = 
                                                               PROT_STATE_NULL;
      

  }
 
}/*mmoc_dbg_buf_init_rpt_queue()*/


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_dbg_buf_init

DESCRIPTION
  This function will initialize the MMOC _Debug_buffer info
  at time of power up.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_dbg_buf_init
(

       void
)
{
  int i = 0;
	
  mmoc_debug_buffer.dbg_buf_idx = -1;
	
  for( i = 0 ; i < MMOC_MAX_DEBUG_BUFFER_SIZE ; i++ ) 
  {
    mmoc_debug_buffer.dbg_buf[i].trans_name = MMOC_TRANS_NULL;
    mmoc_debug_buffer.dbg_buf[i].trans_id = PROT_TRANS_NONE;
    memset(mmoc_debug_buffer.dbg_buf[i].addl_info, 0, MMOC_DBG_BUF_TRANS_ADDL_INFO_SIZE);
    mmoc_dbg_buf_init_rpt_queue(i);
  }

}/*mmoc_dbg_buf_init()*/

#endif /* MMODE_ADDITIONAL_DEBUG_INFO */

/*===========================================================================

FUNCTION mmoc_sanity_timer_reset

DESCRIPTION
  Clear the insanity_count and reset the timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_sanity_timer_reset(

       mmoc_state_info_s_type         *mmoc_info_ptr
)
{

  MMOC_ASSERT( mmoc_info_ptr != NULL );

  mmoc_info_ptr->insanity_count = 0;

  if(rex_get_timer(&mmoc_info_ptr->sanity_timer) != 0)
  {
   (void) rex_clr_timer( &mmoc_info_ptr->sanity_timer );
  }
}

/*===========================================================================

FUNCTION mmoc_sanity_timer_set

DESCRIPTION
  Set the MMOC sanity timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_sanity_timer_set(

       mmoc_state_info_s_type         *mmoc_info_ptr,

       rex_timer_cnt_type              sanity_timeout
)
{
  MMOC_ASSERT( mmoc_info_ptr != NULL );

  if (sanity_timeout != MMOC_SANITY_TIME)
  {
    MMOC_MSG_MED_1("Set the Sanity Time out %ld",sanity_timeout);
  }

  (void) rex_set_timer( &mmoc_info_ptr->sanity_timer,
                        sanity_timeout
                      );
}

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_next_trans_id

DESCRIPTION
  This function gets the next valid transaction Id. Also, sets the value
  in the MMoC's state information.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static prot_trans_type                 mmoc_next_trans_id
(

       mmoc_state_info_s_type          *mmoc_info_ptr
          /* Pointer to MMoC's state information.
          */

)
{
  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Print out debug messages.
  */
  MMOC_MSG_LOW_2("Curr. transaction %d trans. state %d", mmoc_info_ptr->curr_trans, mmoc_info_ptr->trans_state);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ++mmoc_info_ptr->trans_id;

  /* PROT_TRANS_NONE is used by protocols when autodeactivating themselves and
  ** reporting to MMOC, and PROT_TRANS_ID_RESERVED is a generic reserved 
  ** trans_id (currently only used by HDR to send response to MMOC when CM 
  ** sends a tunnel deactivate request to it), so keep incrementing trans_id 
  ** if it is equal to either of them.
  */
  while ( (mmoc_info_ptr->trans_id) == PROT_TRANS_NONE || 
          (mmoc_info_ptr->trans_id) == PROT_TRANS_ID_RESERVED )
  {
    ++mmoc_info_ptr->trans_id;
  }


  return mmoc_info_ptr->trans_id;

} /* mmoc_next_trans_id() */





/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_update_standby_sleep_wakeup_state

DESCRIPTION
  This function checks if transitioned in/out of standby sleep. If so
  informs CM of the new standby state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_update_standby_sleep_wakeup_state
(

    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */

)
{
   
  MMOC_ASSERT( mmoc_info_ptr != NULL );

  

  /* Check if we are in pwr_save.
  */
  if ( mmoc_info_ptr->prot_state == PROT_STATE_PWR_SAVE)
  {
    /* Check if mmoc state is not in standby sleep.
    */
    if ( !mmoc_info_ptr->is_standby_sleep )
    {
      /* MMoc state in standby wakeup,
      ** Send the STANDBY SLEEP report to CM.
      */
      mmoc_send_cm_rpt( CM_STANDBY_SLEEP_F , NULL);
      mmoc_info_ptr->is_standby_sleep = TRUE;

    }

  }
  else
  {
    /* Phone is not in standby sleep, check mmoc state.
    */
    if ( mmoc_info_ptr->is_standby_sleep )
    {
      /* MMOC state in standby sleep,
      ** Send the STANDBY WAKEUP report to CM.
      */
      mmoc_send_cm_rpt( CM_STANDBY_WAKE_F, NULL );
      mmoc_info_ptr->is_standby_sleep = FALSE;

    }

  }


} /* mmoc_update_standby_sleep_wakeup_state() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_update_prot_state

DESCRIPTION
  This function sets the MMoC's protocol state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                            mmoc_update_prot_state
(

    prot_state_e_type                  new_prot_state,
       /* The new protocol state of the MMoC.
       */
    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */

)
{
  MMOC_ASSERT_ENUM_IS_INRANGE(new_prot_state, PROT_STATE_MAX);
  MMOC_ASSERT( mmoc_info_ptr != NULL );



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print out debug messages.
  */
  
  MMOC_MSG_LOW_4("Curr. trans  %d Curr. trans. state  %d Curr. prot. state %d New prot. state %d", 
  mmoc_info_ptr->curr_trans, 
  mmoc_info_ptr->trans_state, 
  mmoc_info_ptr->prot_state, 
  new_prot_state
  );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Update the new protocol state.
  */
  mmoc_info_ptr->prot_state= new_prot_state;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update standby sleep /wakeup transition, if changed.
  */
  mmoc_update_standby_sleep_wakeup_state( mmoc_info_ptr );

  /* Each time we set prot_state to NULL, we will check if we have any auto_deact 
  ** buffered for that stack. Setting prot_state to NULL means we are deactivating 
  ** prot.Hence, if any auto_deact report found, we will discard it */
  if(new_prot_state == PROT_STATE_NULL )
  {
    mmoc_is_auto_deact_ind_rpt_buf_per_stack(mmoc_info_ptr,TRUE);
  }


} /* mmoc_update_prot_state() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_update_db_dmss_state

DESCRIPTION
  This function sets the DB DMSS state based on the current protocol
  state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,mmoc_info_ptr)*/
static void                            mmoc_update_db_dmss_state
(

       const  mmoc_state_info_s_type   *mmoc_info_ptr
          /* Pointer to MMoC's state information.
          */

)
{
  #ifdef MMOC_USES_DB_SERVICES
  static db_items_value_type  db_item;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Determine what the DB state is.
  */
  switch( mmoc_info_ptr->prot_state )
  {

    case PROT_STATE_NULL:
      db_item.dmss_state =  DB_DMSS_POWERUP_S;
      break;

    case PROT_STATE_ONLINE_GWL:
    case PROT_STATE_ONLINE_DED_MEAS:
      db_item.dmss_state = DB_DMSS_ONLINE_DIGITAL_S;
      break;

    case PROT_STATE_OFFLINE:
      db_item.dmss_state = DB_DMSS_OFFLINE_DIGITAL_S;
      break;

    case PROT_STATE_RESET:
      db_item.dmss_state = DB_DMSS_RESET_S;
      break;

    case PROT_STATE_LPM:
    case PROT_STATE_PWR_DOWN:
      db_item.dmss_state = DB_DMSS_POWERDOWN_S;
      break;

    case PROT_STATE_GPSONE_MSBASED:
      db_item.dmss_state = DB_DMSS_GPS_S;
      break;

    default:
      /* No DB state defined for these protocol state, don't update
      ** the DB state.
      */
      break;

  } /* switch( mmoc_info_ptr->prot_state ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Update the dmss state.
  */
  db_put(DB_DMSS_STATE, &db_item);

  #endif /* defined(MMOC_USES_DB_SERVICES) */

} /* mmoc_update_db_dmss_state() */
/*lint +esym(715,mmoc_info_ptr)*/

 /*===========================================================================

FUNCTION is_pref_chng_restore_req

DESCRIPTION
  Checks if gen cmd is pref change due to reason RESTORE.
===========================================================================*/
static boolean is_pref_chng_restore_req
(
  const prot_gen_cmd_s_type *gen_cmd_ptr
)
{
  boolean is_restore_pref = FALSE;

  if(gen_cmd_ptr == NULL)
  {
    return is_restore_pref;
  }

  if(gen_cmd_ptr->cmd_type == PROT_GEN_CMD_PREF_SYS_CHGD)
  {
    sd_ss_pref_reas_e_type pref_reas =
                                  gen_cmd_ptr->param.pref_sys_chgd.pref_reas;

    is_restore_pref = (pref_reas == SD_SS_PREF_REAS_RESTORE) ? TRUE : FALSE;
  }

  return is_restore_pref;
}

/*===========================================================================

FUNCTION mmoc_print_lte_band_mask

DESCRIPTION
Print the LTE band mask.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
none

===========================================================================*/
static void mmoc_print_lte_band_mask(sys_lte_band_mask_e_type lte_band_pref)
{
  #if (LTE_BAND_NUM == 256)
  if(lte_band_pref.bits_193_256 || lte_band_pref.bits_129_192)
  {
    MMOC_MSG_MED_4("LTE bands 193_256: 0x%08x %08x, LTE bands 129_192: 0x%08x %08x",
               QWORD_HIGH(lte_band_pref.bits_193_256),
                   QWORD_LOW(lte_band_pref.bits_193_256),
               QWORD_HIGH(lte_band_pref.bits_129_192),
               QWORD_LOW(lte_band_pref.bits_129_192));
  }
  
  MMOC_MSG_MED_4("LTE bands 65_128: 0x%08x %08x, LTE bands 1_64: 0x%08x %08x",
               QWORD_HIGH(lte_band_pref.bits_65_128),
                 QWORD_LOW(lte_band_pref.bits_65_128),
               QWORD_HIGH(lte_band_pref.bits_1_64),
               QWORD_LOW(lte_band_pref.bits_1_64));
  #else
  MMOC_MSG_MED_2("LTE bands    1_64: 0x%08x %08x", QWORD_HIGH(lte_band_pref),
                                                   QWORD_LOW(lte_band_pref));
  #endif
}

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_update_trans_state

DESCRIPTION
  This function sets the MMoC's transaction state as per the value passed in.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                            mmoc_update_trans_state
(
    mmoc_trans_state_e_type            new_trans_state,
       /* The new transaction state of the MMoC.
       */

    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{
  MMOC_ASSERT_ENUM_IS_INRANGE(new_trans_state, MMOC_TRANS_STATE_MAX);
  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  
  if (mmoc_info_ptr->trans_state != new_trans_state)
  {
  /* Print out debug messages.
  */
  MMOC_MSG_HIGH_3("curr_trans %d curr_trans_state %d new_trans_state %d",
  	             mmoc_info_ptr->curr_trans, mmoc_info_ptr->trans_state, new_trans_state);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  

  /* Update the MMoC's new transaction state.
  */
  mmoc_info_ptr->trans_state = new_trans_state;
  }


} /* mmoc_update_trans_state() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_update_curr_trans

DESCRIPTION
  This function sets the current MMoC transaction given the new transaction.
  Also, the new transaction state is set to NULL.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_update_curr_trans
(

    mmoc_trans_e_type                  new_trans,
       /* The new transaction state of the MMoC.
       */

    mmoc_state_info_s_type             *mmoc_info_ptr,
       /* Pointer to MMoC's state information.
       */

    sys_oprt_mode_e_type               current_oprt_mode
      /* Flag to indicate if RESET_MODEM cmd is being processed.
      */
)
{
  rex_timer_cnt_type  sanity_timeout = MMOC_SANITY_TIME;
  

  MMOC_ASSERT_ENUM_IS_INRANGE(new_trans, MMOC_TRANS_MAX);
  MMOC_ASSERT( mmoc_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(mmoc_info_ptr->curr_trans >= MMOC_TRANS_MAX)
  {
      MMOC_ERR_1( "curr_trans out-of-bound: %d, Aborting!!!",
                mmoc_info_ptr->curr_trans);
      return;
  }

  if(new_trans >= MMOC_TRANS_MAX)
  {
      MMOC_ERR_1( "invalid new_trans: %d", new_trans);
      return;
  }

  if(mmoc_info_ptr->trans_state >= MMOC_TRANS_STATE_MAX)
  {
      MMOC_ERR_1( "invalid trans_state: %d",
                mmoc_info_ptr->trans_state);
      return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the new transaction.
  */
  mmoc_info_ptr->curr_trans  = new_trans;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the new true_oprt_mode
  */
  mmoc_info_ptr->true_oprt_mode = current_oprt_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* When curr_trans is changed, always start from NULL trans_state.
  */
  mmoc_info_ptr->trans_state = MMOC_TRANS_STATE_NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do common initialization.
  */
  mmoc_info_ptr->is_ph_stat_sent       = FALSE;
  mmoc_info_ptr->is_buffer_auto_deact_ind = FALSE;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear the insanity_count and reset the timer.
  */
  mmoc_sanity_timer_reset(mmoc_info_ptr);


  mmoc_info_ptr->max_sanity_time_multiple = 1;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Start the sanity timer if the current transaction is not NULL.
  */
  if ( mmoc_info_ptr->curr_trans != MMOC_TRANS_NULL )
  {
    /* If it is powerdown transaction, load the special timeout value.
    */
    if ( mmoc_info_ptr->curr_trans == MMOC_TRANS_PWR_DOWN )
    {
      sanity_timeout = MMOC_PWRDOWN_SANITY_TIME;
    }

    /* If it is waiting for  PS detach conf, load special timeout value.
    */
    if( mmoc_info_ptr->curr_trans == MMOC_TRANS_PROT_GEN_CMD &&
       mmoc_info_ptr->trans_state == MMOC_TRANS_STATE_WAIT_PS_DETACH_CNF)
    {
      sanity_timeout = MMOC_PS_DETACH_SANITY_TIME;
    }
    

    /* Start the sanity timer.
    */
    mmoc_sanity_timer_set(mmoc_info_ptr,sanity_timeout);
  } /* if ( mmoc_info_ptr != MMOC_TRANS_NULL ) */




} /* mmoc_update_curr_trans() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_clear_transaction

DESCRIPTION
  This function clears the MMoC's transaction.

DEPENDENCIES
  None.

RETURN VALUE
 MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
 MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
mmoc_evt_status_e_type          mmoc_clear_transaction
(
    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{

  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if exit fn. needs to be called.
  */
  if ( mmoc_info_ptr->exit_fn_ptr )
  {
    /* Invoke exit transaction fn.
    */
    if ( (mmoc_info_ptr->exit_fn_ptr)( mmoc_info_ptr ) ==
                         MMOC_EVT_STATUS_NOT_CONSUMED )
    {
      /* New transaction needs to be invoked.
      */
      mmoc_info_ptr->exit_fn_ptr = NULL;
      return MMOC_EVT_STATUS_NOT_CONSUMED;
    }

  } /*  if ( mmoc_info_ptr->exit_fn_ptr ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Print out debug messages.
  */
  MMOC_MSG_LOW_1("clear_transaction,prot_state %d ",
               mmoc_info_ptr->prot_state);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Need to reset this at the end of transaction */

  mmoc_info_ptr->trans_info.gen_prot_cmd.is_activate_main = FALSE;

  mmoc_info_ptr->is_gwl_deact_sent = FALSE;

 
  /* Clear the transaction and its state.
  */
  mmoc_update_curr_trans( MMOC_TRANS_NULL,
                          mmoc_info_ptr,
                          SYS_OPRT_MODE_NONE
                         );

  mmoc_update_trans_state( MMOC_TRANS_STATE_NULL,
                           mmoc_info_ptr
                         );
  mmoc_info_ptr->exit_fn_ptr = NULL;

  return MMOC_EVT_STATUS_CONSUMED;

} /* mmoc_clear_transaction() */

/*===========================================================================

FUNCTION MMOC_REG_MULTIMODE_SANITY_ERR_FATAL

DESCRIPTION
   This function is called since the GSM/WCDMA/LTE stack did not respond to
   CM_STOP_MODE_REQ or CM_SIM_AVAILABLE_REQ or CM_SIM_NOT_AVAILABLE_REQ.
   Response timeout is 72 seconds(144 seconds for DSDS target).

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmoc_reg_multimode_sanity_err_fatal(

     mmoc_sanity_err_e_type     reason
)
{
  int sanity_time =
                ( mmoc_state_info.insanity_count * ( MMOC_SANITY_TIME / 1000 ) );



  /* This feature is owned by NAS team and used to log
   * MM, EMM, REG variables during crash.
   */
  #ifdef MMOC_GWL_SUPPORTED
  cmregprx_print_state_variables();
  #ifdef MMODE_ADDITIONAL_DEBUG_INFO
    cmregprx_dbg_buf_print_info();
  #endif
  #endif
  switch ( reason )
  {
    case MMOC_SANITY_ERR_DEACT_CMD_CNF:
      /* NAS(REG Task) stack did not respond to CM_STOP_MODE_REQ.
      */
      MMOC_ERR_FATAL_1("NAS(REG Task) did not respond to CM_STOP_MODE_REQ for %dsec", sanity_time);
      break;

    case MMOC_SANITY_ERR_PH_STAT_CMD_CNF:
       /* NAS(REG Task) stack did not respond to CM_SIM_AVAILABLE_REQ or
      ** CM_SIM_NOT_AVAILABLE_REQ.
      */
      MMOC_ERR_FATAL_1("NAS(REG Task) did not respond to CM_SIM_XXXX_REQ for %dsec", sanity_time); 

      break;

    default:
     break;

  } /* switch ( reason ) */

} /* mmoc_reg_multimode_sanity_err_fatal */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_active_protocol_name

DESCRIPTION
  This function returns the active protocol name.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static char*                           mmoc_active_protocol_name
(
    const mmoc_state_info_s_type       *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{
  char *s;
  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Determine the active protocol name using the protocol state.
  */
  switch ( mmoc_info_ptr->prot_state)
  {

    case PROT_STATE_ONLINE_GWL:
      s = "REG(NAS)";
      break;

    case PROT_STATE_FTM:
      s = "MC(FTM)";
      break;

    case PROT_STATE_GPSONE_MSBASED:
      s = "GPS-MSBASED";
      break;

    default:
      s = "No active protocols";
      break;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the active protocol name.
  */
  return s;

} /* mmoc_active_protocol_name() */


#ifndef FEATURE_MMOC_NO_SANITY_ERR_FATAL
/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_print_active_prot_sanity_err_fatal

DESCRIPTION
  This function returns the active protocol file name.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_print_active_prot_sanity_err_fatal
(
    mmoc_sanity_err_e_type             reason,
       /* Reason for "insanity".
       */
    const mmoc_state_info_s_type      *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{
  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Determine the active protocol.
  */
  switch ( mmoc_info_ptr->prot_state)
  {

    case PROT_STATE_ONLINE_GWL:
      #if defined MMOC_GWL_SUPPORTED 
      mmoc_reg_multimode_sanity_err_fatal( reason );
      #endif
      break;

    case PROT_STATE_FTM:
      /* Insert FTM fn here.
      */
      break;

    default:
      break;
  }

} /* mmoc_print_active_prot_sanity_err_fatal() */

#endif /* FEATURE_MMOC_NO_SANITY_ERR_FATAL */

#ifdef MMODE_ADDITIONAL_DEBUG_INFO
/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_dbg_buf_print_before_err_fatal

DESCRIPTION
  This function displays the mmoc debug buffer as F3 messages.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                 mmoc_dbg_buf_print_before_err_fatal( void )
{

#ifdef MMODE_ADDITIONAL_DEBUG_INFO_EXT

  int i = 0, j = 0;
  
  MMOC_MSG_HIGH_1("MMOC DEBUG BUFFER: Current index: %d", mmoc_debug_buffer.dbg_buf_idx);

  for(i = 0 ; i < MMOC_MAX_DEBUG_BUFFER_SIZE ; i++ ) 
  { 
    /* Print a transaction only if it has some valid value
    */
    if(mmoc_debug_buffer.dbg_buf[i].trans_name != MMOC_TRANS_NULL)
    {
      MMOC_MSG_HIGH_3("MMOC DEBUG BUFFER: Transaction[%d] : %d | Trans_ID = %d",
                     i, 
                     mmoc_debug_buffer.dbg_buf[i].trans_name, 
                     mmoc_debug_buffer.dbg_buf[i].trans_id);
    
      for(j = 0 ; j < MMOC_MAX_RPT_COUNT; j++)
      {
        /* Print a report only if it is not same as intial defult value
        */
        if(mmoc_debug_buffer.dbg_buf[i].rpt_queue[j].rpt_name != MMOC_RPT_MAX)
        {
          MMOC_MSG_HIGH_3("MMOC DEBUG BUFFER: Report[%d] : %d from task %d",
                             j, 
                             mmoc_debug_buffer.dbg_buf[i].rpt_queue[j].rpt_name, 
	  	                     mmoc_debug_buffer.dbg_buf[i].rpt_queue[j].task_name);
	      

        MMOC_MSG_HIGH_3(" MMOC DEBUG BUFFER:  Prot States : [MAIN] %d | [HYBR_GW] %d | [HYBR_HDR] %d",
                               mmoc_debug_buffer.dbg_buf[i].rpt_queue[j].prot_state,
                               mmoc_debug_buffer.dbg_buf[i].rpt_queue[j]., 
                               mmoc_debug_buffer.dbg_buf[i].rpt_queue[j].prot_state[SD_SS_HYBR_1]);

        }
      }
    }
  }
  
#endif /* MMODE_ADDITIONAL_DEBUG_INFO_EXT */

}/* mmoc_dbg_buf_print_before_err_fatal() */
#endif /* MMODE_ADDITIONAL_DEBUG_INFO */

static void mmoc_print_pwrup_parameters(mmoc_state_info_s_type *mmoc_info_ptr)
{
  MMOC_MSG_HIGH_2("go pwrup subs: chg_mask x%x, gwl %d",
                 mmoc_info_ptr->prot_subsc_chg,
                 mmoc_info_ptr->is_gwl_subs_avail);

}

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_display_debug_info

DESCRIPTION
  This function displays the debug info as F3 messages.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_display_debug_info
(
    const mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{
  #ifdef FEATURE_MODEM_HEAP_PHASE_2
  mmoc_debug_str = (char*) modem_mem_alloc (MMOC_MAX_CHAR, MODEM_MEM_CLIENT_MMODE);
  #endif

  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  switch ( mmoc_info_ptr->trans_state )
  {
    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF:
         (void) snprintf( mmoc_debug_str, sizeof(mmoc_debug_str),
                      "Deactivate request sent to %s ",
                      mmoc_active_protocol_name(mmoc_info_ptr )
                    );

      break;

    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF_GWL:
      (void) snprintf( mmoc_debug_str, sizeof(mmoc_debug_str),
                              "GWL Deactivate request sent to %s",
                              mmoc_active_protocol_name(mmoc_info_ptr ));
       break;

    case MMOC_TRANS_STATE_WAIT_PH_STAT_CNF:
      {
        int i;
        /* Search for pending confirmation.
        */
        for (i=0; i < (int)MMOC_MODE_MAX; i++ )
        {
          if ( ph_stat_cmd_tbl.trans_id != PROT_TRANS_NONE )
          {
            (void) snprintf( mmoc_debug_str, sizeof(mmoc_debug_str),
                        "Ph. Status request sent to %s ",
                        ph_stat_cmd_tbl.task_name
                      );

            /* there is pending cnf.
            */
            break;
          }
        }

      }
      break;

    case MMOC_TRANS_STATE_WAIT_GEN_CMD_CNF:
      (void) snprintf( mmoc_debug_str, sizeof(mmoc_debug_str),
                             "Generic cmd request sent to %s ",
                             mmoc_active_protocol_name(                               
                               mmoc_info_ptr )
                           );
      break;

    case MMOC_TRANS_STATE_WAIT_AUTO_DEACTD_IND:
      (void) snprintf( mmoc_debug_str, sizeof(mmoc_debug_str),
                             "Wait for deactd ind from %s",
                             mmoc_active_protocol_name(
                               mmoc_info_ptr )
                           );
      break;

    case MMOC_TRANS_STATE_PROT_DEACT_ENTER:
      (void) snprintf( mmoc_debug_str, sizeof(mmoc_debug_str),
                             "Deactivate req. entry state"
                           );
      break;

    case MMOC_TRANS_STATE_PROT_PH_STAT_ENTER:
        (void) snprintf( mmoc_debug_str, sizeof(mmoc_debug_str),
                               "Phone status req. entry state"
                             );
        break;

    case MMOC_TRANS_STATE_GEN_CMD_ENTER:
      (void) snprintf( mmoc_debug_str, sizeof(mmoc_debug_str),
                             "Generic cmd req. entry state"
                           );
      break;

    case MMOC_TRANS_STATE_NULL:
      (void) snprintf( mmoc_debug_str, sizeof(mmoc_debug_str),
                             "In NULL transaction state"
                           );
      break;

    case MMOC_TRANS_STATE_MAX:
    default:
      (void) snprintf( mmoc_debug_str, sizeof(mmoc_debug_str),
                             "Invalid transaction state %d",
                             mmoc_info_ptr->trans_state
                           );
      break;
  } /* switch ( mmoc_info_ptr->trans_state ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print the debug string as ERROR message.
  */
  MSG_SPRINTF_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "%s", mmoc_debug_str);

  #ifdef FEATURE_MODEM_HEAP_PHASE_2
  modem_mem_free ((void*)mmoc_debug_str, MODEM_MEM_CLIENT_MMODE);
  #endif

} /* mmoc_display_debug_info() */

#ifndef FEATURE_MMOC_NO_SANITY_ERR_FATAL
/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_print_sanity_err_fatal

DESCRIPTION
  This function calls the insanity timeout err_fatal.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_print_sanity_err_fatal
(
    const mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{

  word   err_code_no;

  MMOC_ASSERT( mmoc_info_ptr != NULL );
  #ifdef FEATURE_MODEM_HEAP_PHASE_2
  mmoc_debug_str = (char*) modem_mem_alloc (MMOC_MAX_CHAR, MODEM_MEM_CLIENT_MMODE);
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize it as mmoc.c
  */
  (void) snprintf( mmoc_debug_str, sizeof(mmoc_debug_str), "mmoc.c" );

  #ifdef MMOC_GWL_SUPPORTED
  /*Log NAS debug info*/
  mm_log_mm_reg_variables();
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  switch ( mmoc_info_ptr->trans_state )
  {
    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF:
      mmoc_print_active_prot_sanity_err_fatal( MMOC_SANITY_ERR_DEACT_CMD_CNF,                                              
                                               mmoc_info_ptr
                    );
      err_code_no = (word)__LINE__;
      break;

    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF_GWL:
      (void)snprintf(mmoc_debug_str,
                           sizeof(mmoc_debug_str),
                           "NAS did not respond to deact");
      err_code_no = (word)__LINE__;
      break;

    case MMOC_TRANS_STATE_WAIT_PH_STAT_CNF:
      {
        int i;
        /* Search for pending confirmation.
        */
        for (i=0; i < (int)MMOC_MODE_MAX; i++ )
        {
          if ( ph_stat_cmd_tbl.trans_id != PROT_TRANS_NONE )
          {
            if ( ph_stat_cmd_tbl.sanity_err_fn )
            {
              (*ph_stat_cmd_tbl.sanity_err_fn)
              ( MMOC_SANITY_ERR_PH_STAT_CMD_CNF );
            }

            /* there is pending cnf.
            */
            break;
          }
        }


        err_code_no = (word)__LINE__;
      }
      break;

    case MMOC_TRANS_STATE_WAIT_GEN_CMD_CNF:
      mmoc_print_active_prot_sanity_err_fatal( MMOC_SANITY_ERR_GENERIC_CMD_CNF,
            mmoc_info_ptr
          );
      err_code_no = (word)__LINE__;
      break;


    case MMOC_TRANS_STATE_WAIT_AUTO_DEACTD_IND:
      mmoc_print_active_prot_sanity_err_fatal( MMOC_SANITY_ERR_GENERIC_CMD_CNF,
            mmoc_info_ptr
          );
      err_code_no = (word)__LINE__;
      break;

    case MMOC_TRANS_STATE_PROT_DEACT_ENTER:
      err_code_no = (word)__LINE__;
      break;

    case MMOC_TRANS_STATE_PROT_PH_STAT_ENTER:
        err_code_no = (word)__LINE__;
        break;

    case MMOC_TRANS_STATE_GEN_CMD_ENTER:
      err_code_no = (word)__LINE__;
      break;

    case MMOC_TRANS_STATE_NULL:
      err_code_no = (word)__LINE__;
      break;

    case MMOC_TRANS_STATE_MAX:
    default:
      err_code_no = (word)__LINE__;
      break;
  } /* switch ( mmoc_info_ptr->trans_state ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef MMODE_ADDITIONAL_DEBUG_INFO
  /*Print Mmoc_Debug_buffer information before error fatal
  */
  mmoc_dbg_buf_print_before_err_fatal();
  #endif

  /* Call err_fatal function.
  */

  #ifdef MMOC_DEBUG
  #error code not present
#endif

  /* Always call MMOC_ERR_FATAL, even if not in debug */
  MMOC_ERR_FATAL_1("MMOC Sanity timer expired, err_code_no %d",
                 err_code_no);

  #ifdef FEATURE_MODEM_HEAP_PHASE_2
  modem_mem_free ((void*)mmoc_debug_str, MODEM_MEM_CLIENT_MMODE);
  #endif

} /* mmoc_print_sanity_err_fatal() */

#endif /* FEATURE_MMOC_NO_SANITY_ERR_FATAL */






/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_set_sd_initialized

DESCRIPTION
  This function sets the MMoC's state to SD initialized.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_set_sd_initialized
(

    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{
  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  MMOC_MSG_LOW_2("sd_initialized(),curr_trans %d,trans_state %d",
               mmoc_info_ptr->curr_trans,
               mmoc_info_ptr->trans_state);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set the MMoC's is_sd_initialized to TRUE,
  */
  mmoc_info_ptr->is_sd_initialized = TRUE;


} /* mmoc_set_sd_initialized() */

#ifdef MMODE_ADDITIONAL_DEBUG_INFO
/*===========================================================================

FUNCTION mmoc_dbg_buf_map_rex_name_to_mmoc

DESCRIPTION
  This function maps the task_ name returned by REX to MMOC's TASK_NAME_ENUM.

DEPENDENCIES
  None.

RETURN VALUE
  mmoc Task Name

SIDE EFFECTS
  None.
===========================================================================*/
static mmoc_rpt_task_name_e_type mmoc_dbg_buf_map_rex_name_to_mmoc
(

    char         *rex_task_name
       
 
)
{
 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* No need to compare rex_task_name in case it is an empty string
  */
  if(strlen(rex_task_name) != 0)
  {
    /* Switch on the rex task name
    */
    if(strcmp(rex_task_name, "mmoc") == 0) 
    {
      return MMOC_TASK_NAME_MMOC;
    }
    if(strcmp(rex_task_name, "cm") == 0) 
    {
      return MMOC_TASK_NAME_CM;
    }
  }

  MMOC_MSG_LOW_1("Unknown task context %s", rex_task_name);
      
  return MMOC_TASK_NAME_MAX;

} /* mmoc_dbg_buf_map_rex_name_to_mmoc() */
#endif /* MMODE_ADDITIONAL_DEBUG_INFO */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_map_sys_oprt_mode_to_mmoc

DESCRIPTION
  This function sets the MMoC's state to SD uninitialized.

DEPENDENCIES
  None.

RETURN VALUE
  mmoc type operating mode status.

SIDE EFFECTS
  None.
===========================================================================*/
static prot_oprt_mode_e_type           mmoc_map_sys_oprt_mode_to_mmoc
(

    sys_oprt_mode_e_type               sys_oprt_mode
       /* Operating mode - SYS type.
       */
)
{
  prot_oprt_mode_e_type  mmoc_oprt_mode = PROT_OPRT_MODE_MAX;

  MMOC_ASSERT_ENUM_IS_INRANGE( sys_oprt_mode, SYS_OPRT_MODE_MAX );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Switch on the sys_oprt_mode.
  */
  switch ( sys_oprt_mode )
  {
    case SYS_OPRT_MODE_PWROFF:
      mmoc_oprt_mode = PROT_OPRT_MODE_PWROFF;
      break;

    case SYS_OPRT_MODE_OFFLINE:
      mmoc_oprt_mode = PROT_OPRT_MODE_OFFLINE;
      break;

    case SYS_OPRT_MODE_ONLINE:
      mmoc_oprt_mode = PROT_OPRT_MODE_ONLINE;
      break;

    case SYS_OPRT_MODE_FTM:
      mmoc_oprt_mode = PROT_OPRT_MODE_FTM;
      break;

    case SYS_OPRT_MODE_PSM:
      mmoc_oprt_mode = PROT_OPRT_MODE_PSM;
      break;

    case SYS_OPRT_MODE_RESET:
    case SYS_OPRT_MODE_RESET_MODEM:
      mmoc_oprt_mode = PROT_OPRT_MODE_RESET;
      break;


    case SYS_OPRT_MODE_LPM:
      mmoc_oprt_mode = PROT_OPRT_MODE_LPM;
      break;

    default:
     sys_err_fatal_invalid_value_exception((int)sys_oprt_mode);
     break;

  } /* switch( sys_oprt_mode ) */


  return mmoc_oprt_mode;

} /* mmoc_map_sys_oprt_mode_to_mmoc() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_accept_cmds

DESCRIPTION
  This function will determine if the commands have to be dequeued and
  processed. Based on the return value, the REX signal corresponding to the
  command queue has to be set or reset.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - MMoC can accept commands.
  FALSE - do not accept any commands at this time.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean                         mmoc_accept_cmds
(
       const mmoc_state_info_s_type    *mmoc_info_ptr
          /* Pointer to MMoC's state information.
          */
)
{
  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if transaction is in progress.
  */
  if ( mmoc_info_ptr->curr_trans  == MMOC_TRANS_NULL )
  {
    /* No transaction in progress, so can process next command.
    */
    return TRUE;
  }

  return FALSE;

} /* mmoc_accept_cmds() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_is_prot_deactivating

DESCRIPTION
  This function will determine if the protocol needs to be deactivated based
  on the SS action.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  -  Protocol is deactivating.
  FALSE -  No protocol is not deactivating.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean                         mmoc_is_prot_deactivating
(
       prot_state_e_type               prot_state,
          /* Active protocol.
          */

       sd_ss_act_e_type                next_act
          /* Next SD SS-Action.
          */
)
{
  boolean is_prot_deactivating = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if protocol should deactivate.
  */
  switch ( prot_state ) 
  {
    case PROT_STATE_ONLINE_GWL:
      if ( next_act != SD_SS_ACT_ACQ_GWL  &&
           next_act != SD_SS_ACT_GET_NET_GW  &&
           next_act != SD_SS_ACT_CONTINUE
         )
      {
        is_prot_deactivating = TRUE;
      }
      break;

    default:
      break;

  } /* switch( act_protocol ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* return.
  */
  return is_prot_deactivating;
  /*lint -esym(715,ss) */ /* not referenced */

} /* mmoc_is_prot_deactivating() */


/* <EJECT> */
/*===========================================================================

FUNCTION  mmoc_send_cm_rpt

DESCRIPTION
  This is a utility function called by the transaction state handlers to send
  the report to CM.

  Note! Only CM reports with no parameters can use this function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                            mmoc_send_cm_rpt
(
    cm_name_type                         rpt_name,
       /* Type of report to send.
       */

    const cm_sd_rpt_u_type               * mmoc_rpt_ptr
       /* Payload of report to send.
       */
)
{

  /* Pointer to CM report buffer. */
  cm_sd_rpt_u_type *rpt_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the message buffer.
  */
  rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in the message buffer and place it in CM report queue.
  */
  MMOC_MSG_HIGH_1("Sending CM report %d", rpt_name);

  /* copy payload contents
  */
  if(mmoc_rpt_ptr != NULL)
  {
    switch(rpt_name)
    {
      case CM_SUSPEND_SS_F:
        rpt_ptr->suspend_ss.is_suspend = mmoc_rpt_ptr->suspend_ss.is_suspend;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_MMOC_SUBSCRIPTION_FAILURE:
    case CM_STANDBY_PREF_CHG_OK_F:
      default:
        break;
    }
  }

  /* copy rpt name
  */
  rpt_ptr->hdr.cmd        = rpt_name;

  /* queue report to CM
  */
  cm_sd_rpt(rpt_ptr);


} /* mmoc_send_cm_rpt() */

/*===========================================================================

FUNCTION  mmoc_msgr_send_else_err_fatal

DESCRIPTION
  This is a utility function that initializes message header and sends the
  message through MSGR primitives.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mmoc_msgr_send_else_err_fatal
(
  msgr_umid_type             umid,

  msgr_hdr_struct_type*      msg_ptr,
    /* Pointer to message to be sent. Note that the first member of the message
     *  that is pointed to by this pointer must be of msgr_hdr_struct_type type
     */
  uint32                     msg_len
    /* Total message length (header and payload) in bytes */
)
{
  errno_enum_type msgr_error = E_SUCCESS;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( msg_ptr != NULL )
  {
    msgr_init_hdr( msg_ptr, MSGR_MM_MMOC, umid );

    msgr_error = msgr_send( msg_ptr, msg_len );

    if ( msgr_error != E_SUCCESS )
    {
      MMOC_ERR_FATAL_2( "MSGR send error %d to %d", msgr_error, umid);
    }

    #ifdef FEATURE_MMODE_QTF
    #error code not present
#endif
  }
} /* mmoc_msgr_send_else_err_fatal */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_sanity_timer_exp

DESCRIPTION
  This function will check if a particular transaction is stuck in MMoC
  thereby validating the MMoC sanity.
  The function will do ERR_FATAL if it determines that the MMoC is insane.

  Note! This function should be called at the expiration of the sanity timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                              mmoc_sanity_timer_exp
(

  mmoc_state_info_s_type          *mmoc_state_ptr
    /* Pointer to MMoC's state information.
    */

)
{
  rex_timer_cnt_type  sanity_timeout = MMOC_SANITY_TIME;

  MMOC_ASSERT( mmoc_state_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Transaction has lasted more than sanity time, print debug messages.
  */
  MMOC_MSG_HIGH_3("Curr_trans %d, Trans_state %d, Prot_state %d",
                  mmoc_state_ptr->curr_trans,
                  mmoc_state_ptr->trans_state,
                  mmoc_state_ptr->prot_state);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if this was a race condition, sanity timer expired at the
  ** same time the transaction was made NULL.
  */
  if ( mmoc_state_ptr->curr_trans == MMOC_TRANS_NULL )
  {

    /* Clear insanity_count.
    */
    mmoc_state_ptr->insanity_count = 0;
    return;

  } /* if ( mmoc_state_ptr->curr_trans == MMOC_TRANS_NULL ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print debug info about the offending protocol stack.
  */
  MMOC_MSG_HIGH_0( "No response yet for...");
  mmoc_display_debug_info( mmoc_state_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if MAX insanity count was reached.
  */
  mmoc_state_ptr->insanity_count++;

  
  if ( mmoc_state_ptr->insanity_count == MMOC_MAX_INSANITY_COUNT )
  {
    #ifdef FEATURE_MMOC_NO_SANITY_ERR_FATAL

    /* Simulate the lower layer response and allow the transaction to
    ** proceed.
    */
    if ( mmoc_state_ptr->trans_state == MMOC_TRANS_STATE_WAIT_DEACTD_CNF )
    {
      /* Send deactivated confirmation.
      */
      mmoc_rpt_prot_deactd_cnf( mmoc_state_ptr->trans_id );
      MMOC_ERR_0( "Simulating deactd_cnf");
    }
    else if( mmoc_state_ptr->trans_state == MMOC_TRANS_STATE_WAIT_DEACTD_CNF_GWL )
    {
      /* Send deactivated confirmation.
      */
      mmoc_rpt_prot_deactd_cnf( mmoc_state_ptr->trans_id );
      MMOC_ERR_0( "Simulating gwl deactd_cnf");
    }
    else if ( mmoc_state_ptr->trans_state == MMOC_TRANS_STATE_WAIT_PH_STAT_CNF )
    {
      /* Send Phone status changed confirmation
      */
      mmoc_rpt_ph_stat_chgd_cnf( mmoc_state_ptr->trans_id, SD_MODE_INACT ,MMOC_PH_STAT_NO_ERROR);
      MMOC_ERR_0( "Simulating ph_stat_chgd_cnf");
    }
    else if ( mmoc_state_ptr->trans_state == MMOC_TRANS_STATE_WAIT_GEN_CMD_CNF )
    {
      /* Send protocol generic command confirmation.
      */
      mmoc_rpt_prot_gen_cmd_cnf( FALSE, mmoc_state_ptr->trans_id, FALSE );
      MMOC_ERR_0( "Simulating prot_gen_cmd_cnf");

    }
    #else
    /* MMoC stuck at this transaction for more than MAX_COUNT.
    */
    MMOC_MSG_HIGH_4("Sanity timer expired, Last Stop Mode sent %ld, Current timestamp %ld"
    				"insanity_count %d, max_sanity_time_multiple %d",
                  mmoc_state_ptr->last_stop_req_sent_timestamp, time_get_uptime_secs(),
                  mmoc_state_ptr->insanity_count, mmoc_state_ptr->max_sanity_time_multiple);
    mmoc_print_sanity_err_fatal( mmoc_state_ptr );
    #endif
  }
  else
  {
    /* If it is powerdown transaction, load the special timeout value.
    */
    if ( mmoc_state_ptr->curr_trans == MMOC_TRANS_PWR_DOWN )
    {
      sanity_timeout = MMOC_PWRDOWN_SANITY_TIME;
    }

    /* If it is waiting for  PS detach conf, load special timeout value.
    */
    if( mmoc_state_ptr->curr_trans == MMOC_TRANS_PROT_GEN_CMD &&
       mmoc_state_ptr->trans_state == MMOC_TRANS_STATE_WAIT_PS_DETACH_CNF)
    {
      sanity_timeout = MMOC_PS_DETACH_SANITY_TIME;
    }


    /* Restart the sanity timer for the new transaction.
    */
    mmoc_sanity_timer_set(mmoc_state_ptr,sanity_timeout);

  } /* else */



} /* mmoc_sanity_timer_exp() */



/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_activate_prot_online_ss_main

DESCRIPTION
  This is a utility function called by the transaction state handlers to
  activate the protocol based on the SD20 action.
  It would set the MMoC's protocol state after activating the protocol based
  on main SS_instance.

  Note! For some SD actions( example power save ), new transactions have to
  be invoked. In those cases, this function will change the current
  transaction and return EVT_STATUS_NOT_CONSUMED.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion. No new
                                 transaction needs to be invoked.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the new transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
mmoc_evt_status_e_type          mmoc_activate_prot_online_ss_main
(
    sd_ss_act_e_type                   sd_act,
       /* Activate protocol based on this SD action.
       */

    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{
  mmoc_evt_status_e_type  evt_status = MMOC_EVT_STATUS_CONSUMED;

#ifndef FEATURE_MMOC_UNIT_TEST
#ifndef FEATURE_DISABLE_DEEP_SLEEP
  /* Change in bio(Basic IP/OP) interface.
  ** Now MMOC will use DAL interface instead of bio.
  */
  uint32           sw_switch9_state    = 0;

  if (switches_handle == NULL)
  {
  switches_dal_attach =
          DAL_DeviceAttach(DALDEVICEID_SOFTWARESWITCHES,&switches_handle);
  }

  if((switches_dal_attach == DAL_SUCCESS) && (switches_handle != NULL))
  {
    (void)DalSoftwareSwitches_GetMask(
           switches_handle,(uint32)DAL_SOFTWARE_SWITCH_9,&sw_switch9_state);
  }
#endif /* FEATURE_DISABLE_DEEP_SLEEP */
#endif /* FEATURE_MMOC_UNIT_TEST */


  MMOC_ASSERT( mmoc_info_ptr != NULL );


  MMOC_MSG_HIGH_2("mmoc_activate_prot_online_ss_main(),sd_act=%d, em_suspend_mask %d",sd_act, mmoc_info_ptr->emerg_susp_ss_mask);
  if(mmoc_info_ptr->emerg_susp_ss_mask)
  {
    return MMOC_EVT_STATUS_CONSUMED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check of gps_exit rpt needs to be sent.
  */
  #ifdef FEATURE_GPSONE_OOS
  if ( mmoc_info_ptr->prot_state == PROT_STATE_GPSONE_MSBASED &&
       sd_act != SD_SS_ACT_CONTINUE &&
       sd_act != SD_SS_ACT_ACQ_GPS
     )
  {
    /* send GPS exit to CM.
    */
    mmoc_send_cm_rpt( CM_GPS_MODE_EXIT_F , NULL);
  }
  #endif

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 /* Check if acq is allowed to prevent acquisition on MAIN in suspended state
 */
 if(sd_act != SD_SS_ACT_PWR_SAVE &&
    sd_act != SD_SS_ACT_CONTINUE)
  {
     if(mmoc_info_ptr->is_suspend)
     {
       MMOC_MSG_HIGH_0("MAIN in suspended state ignore acq on MAIN ");
       return evt_status;
     }
  }


  /* Activate the protocol based on the SD action or return
  ** EVT_STATUS_NOT_CONSUMED to invoke new transaction.
  */
  switch ( sd_act )
  {

    #ifdef MMOC_GWL_SUPPORTED
    case SD_SS_ACT_ACQ_GWL:
    case SD_SS_ACT_GET_NET_GW:

      /* With GW acquisition request on main and if HDR is active on
      ** the hybrid instance deactivate hdr.
      */

      cmregprx_cmd_gwl_activate_protocol( PROT_ACT_ONLINE_ACQ, SD_SS_MAIN );
      mmoc_update_prot_state( PROT_STATE_ONLINE_GWL,
                                mmoc_info_ptr
                               );
      mmoc_update_db_dmss_state( mmoc_info_ptr );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  -*/
      break;
    #endif 


    #ifdef FEATURE_GPSONE_OOS
    case SD_SS_ACT_ACQ_GPS:
      /* Notify CM that the protocol is placed in GPS state.
      */
      mmoc_send_cm_rpt( CM_GPS_MODE_GRANTED_F , NULL);
      mmoc_update_prot_state( PROT_STATE_GPSONE_MSBASED,
                              mmoc_info_ptr
                            );
      mmoc_update_db_dmss_state( mmoc_info_ptr );
      break;
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_PWR_SAVE:
      /* New "power save enter transaction" needs to be invoked,
      ** curr_trans should be "pwr save enter".
      */
      (void)sd_ss_ind_misc_pwr_save_enter(NULL);
      MMOC_ASSERT( mmoc_info_ptr->curr_trans != MMOC_TRANS_PWR_SAVE_ENTER);

      #if defined(MMOC_GW_SUPPORTED) 
        if( (mmoc_info_ptr->curr_trans == MMOC_TRANS_ONLINE ||
             mmoc_info_ptr->curr_trans == MMOC_TRANS_SUBSC_CHGD) &&
            cmregprx_no_service_event_needed(SD_SS_MAIN) )
        {

          MMOC_MSG_HIGH_0("deact NAS with NO_SYS_TO_ACQ");
          cmregprx_cmd_gwl_deactivate_protocol( PROT_DEACT_NO_SYS_TO_ACQ,
                                              PROT_TRANS_NONE
                                             );
        }
      #endif

      mmoc_update_prot_state( PROT_STATE_PWR_SAVE,                              
                              mmoc_info_ptr
                            );
      /* Check if power save is not allowed, exit immediately */
  #ifndef FEATURE_MMOC_UNIT_TEST
      MMOC_MSG_HIGH_1("sw_switch9_state [main] = 0x%x",sw_switch9_state);
      /* Check on GPIO to see if power save mode is allowed.
      */
      if (
  #ifndef FEATURE_DISABLE_DEEP_SLEEP
               sw_switch9_state  &&
  #endif
          sd_ss_ind_user_pwr_save_exit( NULL ) != SD_SS_ACT_PWR_SAVE
         )

      {
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Power save mode is not allowed and SD can activate a new protocol.
        ** Aborted power save enter, activate the protocol or invoke
        ** transaction handler if this function returns EVT_STATUS_NOT_CONSUMED.
      */
        mmoc_activate_prot_online_ss_main( sd_ss_act_get(NULL),
                                            mmoc_info_ptr
                                          );
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        } /* if (BIO_GET_SW(BIO_SW9_M)) */

  #endif /* #if (!mmoc unit test) */
      break;

    default:
      MMOC_ERR_1(" Err: SD act %d", sd_act);
      break;

  } /* switch( sd_act ) ) */


  return evt_status;

} /* mmoc_activate_prot_online_ss_main */




/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_activate_prot_online

DESCRIPTION
  This is a utility function called by the transaction state handlers to
  activate the protocol based on the SD20 action.


DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion. No new
                                 transaction needs to be invoked.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the new transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static mmoc_evt_status_e_type          mmoc_activate_prot_online
(
    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{
  mmoc_evt_status_e_type  evt_status = MMOC_EVT_STATUS_CONSUMED;

  MMOC_ASSERT( mmoc_info_ptr != NULL );


  evt_status =  mmoc_activate_prot_online_ss_main(
                                         sd_ss_act_get( NULL ),
                                         mmoc_info_ptr
                                                   );

  return evt_status;

} /* mmoc_activate_prot_online() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_activate_prot_offline

DESCRIPTION
  This is a utility function called by the transaction state handlers to
  activate the protocol based the current operating mode.
  It would set the MMoC's protocol state after activating the protocol.

  Note! This function should be called only when the operating mode is
  OFFLINE_CDMA or OFFLINE_AMPS.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_activate_prot_offline
(

    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{

  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Activate the protocol based on the offline operating mode.
  */
  switch ( mmoc_info_ptr->curr_oprt_mode )
  {

    case PROT_OPRT_MODE_OFFLINE:
    case PROT_OPRT_MODE_PSM:
      mmoc_update_prot_state( PROT_STATE_OFFLINE,
                              mmoc_info_ptr
                            );
      /* Need a seperate db_dmss_state here.
      */
      mmoc_update_db_dmss_state( mmoc_info_ptr );
      break;

    case PROT_OPRT_MODE_FTM:
      /* Activate the protocol stack in FTM mode.
      */
      TTLBOOTUP(MMOC_FTM);

      #if defined(MMOC_FTM_SUPPORTED)
      #if defined(MMOC_GWL_SUPPORTED) || defined(MMOC_CDMA_SUPPORTED)
      ftm_activate_protocol(rex_self(),MMOC_FTM_RESUME_MMOC_SIG, (void (*)( rex_sigs_type )) cm_wait, PROT_ACT_FTM, mmoc_next_trans_id(mmoc_info_ptr));
      #endif
      mmoc_update_prot_state( PROT_STATE_FTM,
                              mmoc_info_ptr
                            );
      mmoc_update_db_dmss_state( mmoc_info_ptr );
      #else
      MMOC_ERR_FATAL_0("FTM not supported");
      #endif /* defined(MMOC_FTM_SUPPORTED) */
      break;

    case PROT_OPRT_MODE_NULL:
    case PROT_OPRT_MODE_PWROFF:
    case PROT_OPRT_MODE_ONLINE:
    case PROT_OPRT_MODE_LPM:
    case PROT_OPRT_MODE_RESET:
    case PROT_OPRT_MODE_MAX:
    default:
      sys_err_fatal_invalid_value_exception((int)mmoc_info_ptr->curr_oprt_mode);
      break;

  } /* switch ( mmoc_info_ptr->curr_oprt_mode ) */

} /* mmoc_activate_prot_offline() */

/*===========================================================================

FUNCTION mmoc_deactivate_gwl_protocol

DESCRIPTION
  This is a utility function called by transaction state handlers to
  deactivate GWL protocol when protocol could be in suspend state.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - A protocol deactivate request was sent to the active protocol.
  FALSE - No request was sent.

SIDE EFFECTS
  None.
===========================================================================*/
#ifdef MMOC_GWL_SUPPORTED

static boolean                         mmoc_deactivate_gwl_protocol
(

       prot_deact_e_type               deact_reason,
           /* Reason for deactivating the protocol.
           */

       mmoc_state_info_s_type          *mmoc_info_ptr
          /* Pointer to MMoC's state information.
          */
)
{

  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*Set flag to avoid sending deactivate GWL request multiple times
  */

  if(mmoc_info_ptr->is_gwl_deact_sent)
  {
    return FALSE;
  }

   /* Send GWL deactivate command when subscription change or oprt mode change
   ** This is added to solve issue when ALT_SCAN/DEEP_SLEEP/IRAT happens, NAS
   ** goes to suspend state. It needs another STOP_MODE_REQ to go to deactivate
   ** state
   */
  switch (mmoc_info_ptr->prot_state)
  {
    case PROT_STATE_NULL:
      switch(mmoc_info_ptr->curr_trans)
      {
        case MMOC_TRANS_SUBSC_CHGD:
         /* Only need to deactivate GWL when GW subsc changed
         **
         */
         if( CHECK_PROTO(mmoc_info_ptr->prot_subsc_chg, PROT_SUBSC_CHG_GW))
         {
           cmregprx_cmd_gwl_deactivate_protocol( deact_reason, //PROT_DEACT_SUBSC_CHGD
                                                  mmoc_next_trans_id(mmoc_info_ptr)
                                                );
           mmoc_info_ptr->is_gwl_deact_sent = TRUE;

           mmoc_info_ptr->main_last_prot_state = mmoc_info_ptr->prot_state;

         }
        break;

        /* Handle LPM/OFFLINE command here */
        case MMOC_TRANS_OFFLINE:
        case MMOC_TRANS_PWR_DOWN:

        cmregprx_cmd_gwl_deactivate_protocol( deact_reason, 
                                                mmoc_next_trans_id(mmoc_info_ptr)
                                               );
        mmoc_info_ptr->is_gwl_deact_sent = TRUE;

        mmoc_info_ptr->main_last_prot_state = mmoc_info_ptr->prot_state;
        break;

      default:
        break;
      }
      break;


    default:
      break;
   }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the last deactivate request sent timestamp with current time */
  mmoc_info_ptr->last_stop_req_sent_timestamp = time_get_uptime_secs();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (mmoc_info_ptr->is_gwl_deact_sent)
  {
    MMOC_MSG_HIGH_4("Deact req sent(gwl), deact_reason %d, insanity_count %d, curr_trans %d, prot_state %d",
                      deact_reason, mmoc_info_ptr->insanity_count,
                      mmoc_info_ptr->curr_trans, mmoc_info_ptr->prot_state);
    /* when sending deact to GW protocol on a stack, check for any buffered
    ** auto_deact only if some GW protocol is active on that stack.
    ** If yes, discard the report */
    if(mmoc_info_ptr->prot_state == PROT_STATE_ONLINE_GWL)
    {
      mmoc_is_auto_deact_ind_rpt_buf_per_stack(mmoc_info_ptr,TRUE);
    }  
  }

  return mmoc_info_ptr->is_gwl_deact_sent;


} /* mmoc_deactivate_gwl_protocol() */

 #endif


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_deactivate_active_protocol

DESCRIPTION
  This is a utility function called by transaction state handlers to
  deactivate the active protocol if one exists.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - A protocol deactivate request was sent to the active protocol.
  FALSE - No request was sent.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean                         mmoc_deactivate_active_protocol
(
          prot_deact_e_type               deact_reason,
           /* Reason for deactivating the protocol.
           */

       mmoc_state_info_s_type          *mmoc_info_ptr
          /* Pointer to MMoC's state information.
          */
)
{
  boolean is_deact_req_sent = FALSE;

  MMOC_ASSERT( mmoc_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* check on the current MMoC's protocol state.
  */
  switch (mmoc_info_ptr->prot_state)
  {

    #ifdef MMOC_FTM_SUPPORTED
    case PROT_STATE_FTM:

    /* Preserve the deactivate order ( FTM first, MC next ), this
    ** FTM activity is completely deactivated, when we get the confirmation
    ** from MC. Note that ftm_deactivate_protocol() is executed in MMOC's
    ** context.
    */
    #if defined(MMOC_GWL_SUPPORTED) || defined(MMOC_CDMA_SUPPORTED)
    ftm_deactivate_protocol(rex_self(),MMOC_FTM_RESUME_MMOC_SIG, (void (*)( rex_sigs_type )) cm_wait, deact_reason, mmoc_next_trans_id(mmoc_info_ptr));
    
    #endif

    break;
  #endif /* MMOC_FTM_SUPPORTED */


    #ifdef MMOC_GWL_SUPPORTED
    case PROT_STATE_ONLINE_GWL:
      cmregprx_cmd_gwl_deactivate_protocol( deact_reason,
                                           mmoc_next_trans_id(mmoc_info_ptr)                                           
                                         );

      mmoc_info_ptr->is_gwl_deact_sent = TRUE;
      is_deact_req_sent = TRUE;
      break;
    #endif


    #ifdef FEATURE_GPSONE_OOS
    case PROT_STATE_GPSONE_MSBASED:
      /* Notify CM that GPS state is aborted.
      */
      mmoc_send_cm_rpt( CM_GPS_MODE_EXIT_F , NULL);

      break;
    #endif /* FEATURE_GPSONE_OOS */


    default:
      MMOC_MSG_LOW_0(" No active protocol to deactivate");
      break;
  } /* switch( mmoc_info_ptr->prot_state ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(is_deact_req_sent)
  {
    MMOC_MSG_HIGH_2(" Sending deact to protocol: %d, reason: %d",
        mmoc_info_ptr->prot_state, deact_reason);
  }


  /* Store the protocol state of deactivated index in main_last_prot_state  */
  if ( is_deact_req_sent)
  {
    mmoc_info_ptr->main_last_prot_state = mmoc_info_ptr->prot_state;
  }

  if ( !is_deact_req_sent )
  {
  /* If the prot state is already in OFFLINE/RESET/LPM/PWR_DOWN we don't need to
    ** send deact gwl again since. These states are updated in ph_state_enter
    */
    if(mmoc_info_ptr->prot_state != PROT_STATE_NULL &&
      mmoc_info_ptr->prot_state != PROT_STATE_PWR_SAVE
      )
    {
      mmoc_info_ptr->is_gwl_deact_sent = TRUE;
    }

    mmoc_update_prot_state( PROT_STATE_NULL,
                            mmoc_info_ptr
                          );

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the last deactivate request sent timestamp with current time */
  mmoc_info_ptr->last_stop_req_sent_timestamp = time_get_uptime_secs();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (is_deact_req_sent)
  {
    mmoc_sanity_timer_reset(mmoc_info_ptr);

    mmoc_sanity_timer_set(mmoc_info_ptr,MMOC_PWRDOWN_SANITY_TIME);

    MMOC_MSG_HIGH_2("Deactivate request to be sent, deact_reason %d, insanity_count %d",
                      deact_reason, mmoc_info_ptr->insanity_count);

    /* While sending deactivate req to a protocol, check if we have any 
    ** auto_deact buffered from that protocol. If yes, discard the report
    */
    mmoc_is_auto_deact_ind_rpt_buf_per_stack(mmoc_info_ptr, TRUE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return is_deact_req_sent;

} /* mmoc_deactivate_active_protocol() */


/*===========================================================================

FUNCTION  mmoc_notify_offline_mode

DESCRIPTION
  Notifies offline mode change command other modules in modem.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void mmoc_notify_offline_mode( void )
{
  mmoc_offline_ind_s_type *offline_req;

  offline_req = (mmoc_offline_ind_s_type*)
                      modem_mem_alloc(sizeof(mmoc_offline_ind_s_type),
                                      MODEM_MEM_CLIENT_MMODE);
  if(offline_req == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  memset(offline_req, 0, sizeof(mmoc_offline_ind_s_type));

  mmoc_msgr_send_else_err_fatal( MM_MMOC_OFFLINE_IND,
                                 &(offline_req->hdr),
                                 sizeof(mmoc_offline_ind_s_type));
  mmoc_mem_free(offline_req);
}

/* <EJECT> */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_send_ph_stat_chgd_cmd

DESCRIPTION
  This is a utility function called by the transaction state handlers to send
  the phone status changed command to the various supported modes. The command
  will be sent to one mode for every function call.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Phone status command was sent to support mode.
  FALSE - Completed sending the phone status command to all supported modes.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean                         mmoc_send_ph_stat_chgd_cmd
(
    prot_ph_stat_chg_e_type            chg_type,
       /* Phone status changed type.
       */

    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{

  mmoc_cmd_subsc_chgd_s_type       *subsc_chgd_ptr =
                                      &mmoc_info_ptr->trans_info.subsc_chgd;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the ph. status changed commands have already been sent.
  */
  if (mmoc_info_ptr->is_ph_stat_sent )
  {
    /* Already sent, return FALSE,
    */
    return FALSE;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Decide to whom the next command should be sent.
  */
  mmoc_info_ptr->is_ph_stat_sent = TRUE;



  #ifdef MMOC_GWL_SUPPORTED
  {
    cmregprx_cmd_gwl_ph_stat_chgd( chg_type,
                                  mmoc_info_ptr->is_gwl_subs_avail,
                                  mmoc_info_ptr->curr_oprt_mode,
                                  ph_stat_cmd_tbl.trans_id =
                                         mmoc_next_trans_id(mmoc_info_ptr),
                                  subsc_chgd_ptr->prot_subsc_chg,SD_SS_MODE_PREF_GSM);
  }
  #endif



  mmoc_info_ptr->is_ph_stat_sent = TRUE;
  return mmoc_info_ptr->is_ph_stat_sent;

} /* mmoc_send_ph_stat_chgd_cmd() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_send_subsc_chg_ph_stat_chgd_cmd

DESCRIPTION
  This is a utility function called by the SUBSCRIPTION CHANGE transaction
  handler to send   the phone status changed command to the various supported
   modes. The command will be sent to one mode for every function call.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Phone status command was sent to support mode.
  FALSE - Completed sending the phone status command to all supported modes.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean                         mmoc_send_subsc_chg_ph_stat_chgd_cmd
(
    prot_ph_stat_chg_e_type            chg_type,
       /* Phone status changed type.
       */

    mmoc_state_info_s_type             *mmoc_info_ptr
       /* Pointer to MMoC's state information.
       */
)
{

  mmoc_cmd_subsc_chgd_s_type       *subsc_chgd_ptr =
                                      &mmoc_info_ptr->trans_info.subsc_chgd;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MMOC_ASSERT( mmoc_info_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the ph. status changed commands have already been sent.
  */
  if (mmoc_info_ptr->is_ph_stat_sent )
  {
    /* Already sent, return FALSE,
    */
    return FALSE;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Decide to whom the next command should be sent.
  */
  mmoc_info_ptr->is_ph_stat_sent = TRUE;





  if(mmoc_info_ptr->prot_subsc_chg == PROT_SUBSC_CHG_NONE)
  {
    return mmoc_info_ptr->is_ph_stat_sent;
  }


  #ifdef MMOC_GWL_SUPPORTED
  {
    cm_subs_avail_e_type subs_avail = CM_SUBS_NOT_AVAIL;

    #ifndef FEATURE_MMODE_LTE_ONLY
    if((mmoc_info_ptr->curr_trans == MMOC_TRANS_SUBSC_CHGD) && (subsc_chgd_ptr->gw.is_perso_locked == TRUE))
    {
      subs_avail = CM_SUBS_PERSO_LOCK;
    }
    else
    #endif
    {
      subs_avail = mmoc_info_ptr->is_gwl_subs_avail ? CM_SUBS_AVAIL : CM_SUBS_NOT_AVAIL;
    }
    MMOC_MSG_HIGH_1("subs_avail = %d", subs_avail);
    cmregprx_cmd_gwl_ph_stat_chgd( chg_type,
                                  subs_avail,
                                mmoc_info_ptr->curr_oprt_mode,
                                ph_stat_cmd_tbl.trans_id =
                                       mmoc_next_trans_id(mmoc_info_ptr),
                                subsc_chgd_ptr->prot_subsc_chg,mmoc_info_ptr->trans_info.subsc_chgd.gw.mode_pref
                              );
  }
  #endif

  mmoc_info_ptr->is_ph_stat_sent = TRUE;
  return mmoc_info_ptr->is_ph_stat_sent;

} /* mmoc_send_subsc_chg_ph_stat_chgd_cmd() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_send_prot_gen_cmd

DESCRIPTION
  This is a utility function called by the transaction state handlers to send
  the generic command to the active protocol.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Command was sent to the active protocol.
  FALSE - Command was not sent, because no protocol was active.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean                         mmoc_send_prot_gen_cmd
(
       prot_gen_cmd_s_type             *gen_cmd_ptr,
          /* Pointer to the generic protocol command.
          */

       mmoc_state_info_s_type          *mmoc_info_ptr
          /* Pointer to MMoC's state information.
          */
)
{
  boolean is_cmd_sent = TRUE;

  MMOC_ASSERT( mmoc_info_ptr != NULL );
  MMOC_ASSERT( gen_cmd_ptr != NULL );

  /* check if we have auto_deact from protcol to which we want to send 
  ** prot_gen_cmd. If yes, protocol has already deactivated itself.
  ** So, set prot_state to NULL and discard buffered auto_deact.
  */
  mmoc_is_auto_deact_ind_rpt_buf_per_stack(mmoc_info_ptr,TRUE);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (mmoc_info_ptr->prot_state)
  {
    #ifdef MMOC_GWL_SUPPORTED
    case PROT_STATE_ONLINE_GWL:
      gen_cmd_ptr->trans_id = mmoc_next_trans_id(mmoc_info_ptr);
      cmregprx_cmd_gwl_generic_cmd( gen_cmd_ptr );
      break;
    #endif


    case PROT_STATE_FTM:
    case PROT_STATE_LPM:
    case PROT_STATE_OFFLINE:
    case PROT_STATE_PWR_DOWN:
    case PROT_STATE_RESET:
      sys_err_fatal_invalid_value_exception((int)(mmoc_info_ptr->prot_state));
      break;

    default:
      is_cmd_sent = FALSE;
      MMOC_MSG_LOW_0(" No active protocol to send the generic command");
      break;
  } /* switch( mmoc_info_ptr->prot_state ) */


  MMOC_MSG_HIGH_2("mmoc_send_prot_gen_cmd(),prot_state=%d, is_cmd_sent=%d",
                      mmoc_info_ptr->prot_state,
                      is_cmd_sent);


  return is_cmd_sent;

} /* mmoc_send_prot_generic_cmd() */



/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC TASK RELATED HELPER FUNCTIONS =====================
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_get_cmd_buf_else_err_fatal

DESCRIPTION
  Allocate a MMoC task command buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated MMoC command buffer.

SIDE EFFECTS
  none
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
static mmoc_cmd_msg_s_type*            mmoc_get_cmd_buf_else_err_fatal
(

       void

)
{

  /* Try allocating a MMoC Task command buffer.
  */
  mmoc_cmd_msg_s_type *mmoc_cmd_ptr = NULL;

  if((mmoc_cmd_ptr = modem_mem_alloc(sizeof(mmoc_cmd_msg_s_type), MODEM_MEM_CLIENT_MMODE)) == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
    memset(mmoc_cmd_ptr, 0, sizeof(mmoc_cmd_msg_s_type));
    mmoc_modem_heap_blocks++;
    MMOC_MSG_HIGH_2 ( "MMOC Alloc: cmd addr= %x, Heap Used = %d", mmoc_cmd_ptr, mmoc_modem_heap_blocks);
  }

  /* If we got here, allocation is successful,
  ** so no queue to place command buffer on when done,
  ** and specify NO task to be signalled when done
  */
  mmoc_cmd_ptr->cmd.hdr.done_q_ptr      = NULL;
  mmoc_cmd_ptr->cmd.hdr.task_ptr        = NULL;


  return mmoc_cmd_ptr;

} /* mmoc_get_cmd_buf_else_err_fatal() */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_send_cmd

DESCRIPTION
  This function takes an already filled out mmoc_cmd_msg_type and places it
  on the mmoc command queue.

DEPENDENCIES
  The MMoC task must have been started up already.  All of the items in
  the mmoc_cmd_msg_s_type must be already set.

RETURN VALUE
  None.

SIDE EFFECTS
  The mmoc_cmd_msg_s_type is placed on the MMoC command queue.  It must not be
  modified until the command has been processed.
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
static void                            mmoc_send_cmd
(

       mmoc_cmd_msg_s_type             *msg_ptr
          /* Pointer to MMoC command message buffer.
          */

)
{
  /* Pointer to MMoC's task info.
  */
  mmoc_task_info_s_type*  mmoc_task_ptr = mmoc_get_task_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize and link the command onto the MC command queue.
  */
  (void) q_link(msg_ptr, &msg_ptr->cmd.hdr.link);
  q_put(&mmoc_task_ptr->cmd_q, &msg_ptr->cmd.hdr.link);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set the REX signal corresponding to the MMoC's command queue and
  ** return.
  */
  (void) rex_set_sigs(cm_tcb_ptr, MMOC_CMD_Q_SIG);


} /* mmoc_send_cmd() */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_get_rpt_buf_else_err_fatal

DESCRIPTION
  Allocate a MMoC task report buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated MMoC report buffer.

SIDE EFFECTS
  none
===========================================================================*/
mmoc_rpt_msg_s_type*            mmoc_get_rpt_buf_else_err_fatal
(

       void

)
{

  mmoc_rpt_msg_s_type *mmoc_rpt_ptr = NULL;

  if((mmoc_rpt_ptr = modem_mem_alloc(sizeof(mmoc_rpt_msg_s_type), MODEM_MEM_CLIENT_MMODE)) == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
    memset(mmoc_rpt_ptr, 0, sizeof(mmoc_rpt_msg_s_type));
    mmoc_modem_heap_blocks++;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If we got here, allocation is successful
  */
  mmoc_rpt_ptr->rpt.hdr.done_q_ptr      = NULL;
  mmoc_rpt_ptr->rpt.hdr.task_ptr        = NULL;


  return mmoc_rpt_ptr;

} /* mmoc_get_rpt_buf_else_err_fatal() */



/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_send_rpt

DESCRIPTION
  This function takes an already filled out mmoc_rpt_msg_type and places it
  on the mmoc report queue.

DEPENDENCIES
  The MMoC task must have been started up already.  All of the items in
  the mmoc_rpt_msg_s_type must be already set.

RETURN VALUE
  None.

SIDE EFFECTS
  The mmoc_rpt_msg_s_type is placed on the MMoC report queue.  It must not be
  modified until the command has been processed.
===========================================================================*/
void                            mmoc_send_rpt
(

       mmoc_rpt_msg_s_type             *msg_ptr
          /* Pointer to MMoC report message buffer.
          */

)
{
  /* Pointer to MMoC's task info.
  */
  mmoc_task_info_s_type*  mmoc_task_ptr = mmoc_get_task_info_ptr();
 #ifdef MMODE_ADDITIONAL_DEBUG_INFO
 rex_tcb_type  *temp = NULL;
  char rtask_name[MMOC_MAX_TASK_NAME_LEN]; 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* Get the task name of the protocol that sent the report
  */

  temp = rex_self();
  if(temp != NULL)
  {
    #ifdef FEATURE_REX_OPAQUE_TCB_APIS
      /*lint -esym(119,rex_get_task_name) complains of false prototype mismatch */
      (void)rex_get_task_name(temp, rtask_name,
                                    sizeof(rtask_name), NULL);
    #else
      (void)strlcpy(rtask_name, temp->task_name, ARR_SIZE(rtask_name) );
      rtask_name[(ARR_SIZE(rtask_name))-1] = '\0';
    #endif

    /* Map the task name received from rex to mmoc_rpt_task_name_e_type
    ** and store it in task_name field of rpt_ptr
    */    
    msg_ptr->rpt.task_name = mmoc_dbg_buf_map_rex_name_to_mmoc(rtask_name);
  }
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /** Also, set is_reused flag for every report to FALSE by default
  */
  msg_ptr->rpt.is_reused = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize and link the command onto the MC report queue.
  */
  (void) q_link(msg_ptr, &msg_ptr->rpt.hdr.link);
  q_put(&mmoc_task_ptr->rpt_q, &msg_ptr->rpt.hdr.link);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set the REX signal corresponding to MMoC's report queue and return.
  */
  (void) rex_set_sigs(cm_tcb_ptr, MMOC_RPT_Q_SIG);


} /* mmoc_send_rpt() */

/*===========================================================================

FUNCTION get_mmoc_tcb

DESCRIPTION
  Gets pointer MMOC TCB.

DEPENDENCIES
  set_mmoc_tcb.

RETURN VALUE
  Pointer to MMOC TCB.

SIDE EFFECTS
  None.
===========================================================================*/
rex_tcb_type* get_mmoc_tcb(void)
{
  return cm_tcb_ptr;
}

/*===========================================================================

FUNCTION get_mmoc_tcb

DESCRIPTION
  Sets pointer to MMOC TCB.

DEPENDENCIES
  Should be called after CM Task is started by RC Init.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

static void set_mmoc_tcb(void)
{
  cm_tcb_ptr = rex_self();
}

/*===========================================================================

FUNCTION mmoc_init_before_task_start

DESCRIPTION
  This function initializes MMOC global data before MMOC task is created.
  This function is called by RCinit framework if it is enabled. Otherwise
  this will be called from MMOC task.

DEPENDENCIES
  This should be called only before task start.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
void mmoc_init_before_task_start( void )
{

  mmoc_task_info_s_type          *mmoc_task_ptr = mmoc_get_task_info_ptr();
  mmoc_state_info_s_type         *mmoc_state_ptr = &mmoc_state_info;

  /* Initialize report queues. */
  ( void ) q_init( &mmoc_task_ptr->rpt_q );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize command queue.
  */
  (void) q_init( &mmoc_task_ptr->cmd_q );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*
  ** Initialize MMOC Heap.
  */
  mmoc_modem_heap_blocks = 0;

  /* Initialize err_fatal debug info.
  */
  #ifdef FEATURE_MMOC_UNIT_TEST
  #error code not present
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialization for mmoc_state_info.
  */
  mmoc_init_state_info( mmoc_state_ptr );
  #ifdef MMODE_ADDITIONAL_DEBUG_INFO
  /* Initialization for  mmoc_debug_buffer values.
  */
  mmoc_dbg_buf_init();
  #endif

  #ifdef FEATURE_MMOC_UNIT_TEST
  #error code not present
#endif
}

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_task_init

DESCRIPTION
  This function is responsible for initializing the MMoC task before task
  start. This includes
  1. Initializing command and report queues
  2. Performing system determination initializing before task start.

DEPENDENCIES
  This should be called only after task start.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
void                           mmoc_task_init ( void )
{

  /* Pointer to MMoC's task information.
  */
  mmoc_task_info_s_type          *mmoc_task_ptr = mmoc_get_task_info_ptr();
  mmoc_state_info_s_type         *mmoc_state_ptr = &mmoc_state_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  set_mmoc_tcb();

  rex_def_timer( &mmoc_task_ptr->rex_wait_timer,
                 get_mmoc_tcb(),
                 MMOC_REX_WAIT_SIG
               );

  rex_def_timer( &mmoc_state_ptr->sanity_timer,
                 get_mmoc_tcb(),
                 MMOC_SANITY_TIMER_SIG
               );

} /* mmoc_task_init() */



/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_ss_main_timer_done_cb

DESCRIPTION
  This function is the callback procedure used by system determination when
  system selection SS_MAIN timer expires.
  This function will queue the reselection timer expired generic command to
  the MMoC command queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
void                            mmoc_ss_main_timer_done_cb
(

  int4                            ms_interval
  /*lint -esym(715,ms_interval)
  ** Have lint not complain about the ignored parameter 'ms_interval'
  ** which is specified to make this routine match the template for
  ** clk_Reg().
  */

)
{

  mmoc_cmd_msg_s_type                 *msg_ptr;

  MMOC_MSG_HIGH_0("main: system selection timer expired");


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the command buffer.
  */
  msg_ptr = mmoc_get_cmd_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->cmd.name                = MMOC_CMD_PROT_GEN_CMD;
  msg_ptr->param.gen_prot_cmd.cmd_type = PROT_GEN_CMD_SS_TIMER_EXP;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the command.
  */
  mmoc_send_cmd( msg_ptr );


} /* mmoc_ss_main_timer_done_cb() */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
/*lint +esym(715,ms_interval)*/

/*===========================================================================

FUNCTION mmoc_ss_main_timer_cb

DESCRIPTION
  This callback function enables or disables system selection SS_MAIN timer.
  It is used by system determination.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            mmoc_ss_main_timer_cb
(

       int4                            ss_timer_duration
          /* Duration in ms for the timer to be active.
          */

)
{

  /* Pointer to MMoC's task information.
  */
  mmoc_task_info_s_type          *mmoc_task_ptr = mmoc_get_task_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Determine whether to cancel the timer or start it.
  */
  if (ss_timer_duration == 0)
  {
    /* Disable System Selection timer.
    */
    (void) timer_clr(&mmoc_task_ptr->ss_main_timer, T_NONE);
  }
  else
  {
    /* Start System Selection timer.
    */
    timer_reg(&mmoc_task_ptr->ss_main_timer,
              (timer_t2_cb_type)mmoc_ss_main_timer_done_cb,
              (timer_cb_data_type) NULL,
              (timetick_type)ss_timer_duration,
              0
             );
  }


} /* mmoc_ss_main_timer_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_subsc_chgd_deact_enter


DESCRIPTION
  This is a function for handling the transaction state DEACT_ENTER for the
  subscription changed transaction . If a protocol needs to be deactivated,
  it sets the next trans_state to DEACTD_CNF else sets the next trans_state
  PH_STAT_ENTER/MMGSDI_READ_ENTER. Unlike mmoc_process_common_deact_enter
  it need not deactivate all active protocols

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Should wait for deactivated confirmation.
  MMOC_EVT_STATUS_NOT_CONSUMED - Start ph_stat_enter processing.


SIDE EFFECTS
  None
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_subsc_chgd_deact_enter
(
        prot_deact_e_type              deact_reason,
           /* Reason for deactivating the active protocol.
           */

        mmoc_state_info_s_type         *mmoc_info_ptr
           /* Pointer to MMoC's state information.
           */

)
{
  boolean is_deact_req_sent = FALSE;

   if ( !is_deact_req_sent )
  {
    if( CHECK_PROTO(mmoc_info_ptr->prot_subsc_chg, PROT_SUBSC_CHG_GW))
    {
     
     /* Deactivate MAIN */
      if ( !is_deact_req_sent )
      {
        is_deact_req_sent = mmoc_deactivate_active_protocol(
                        deact_reason,
                        mmoc_info_ptr
                        );
      }

           
      #if defined MMOC_GWL_SUPPORTED

      if(!is_deact_req_sent )
      {
        is_deact_req_sent = mmoc_deactivate_gwl_protocol(
                                deact_reason,
                                mmoc_info_ptr
                                );


      if (is_deact_req_sent)
      {
         mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_DEACTD_CNF_GWL,
                                   mmoc_info_ptr
                                 );

         return MMOC_EVT_STATUS_CONSUMED;
      }
     }

      #endif //MMOC_GWL_SUPPORTED

    }
  }

   if (is_deact_req_sent)
  {
       mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_DEACTD_CNF,
                                mmoc_info_ptr
                              );

     return MMOC_EVT_STATUS_CONSUMED;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* No active protocols present, start sending the phone status
  ** changed command.
  */
  mmoc_update_trans_state( MMOC_TRANS_STATE_PROT_PH_STAT_ENTER,
                           mmoc_info_ptr
                         );


  return MMOC_EVT_STATUS_NOT_CONSUMED;
  /*lint -restore */

} /* mmoc_process_subsc_chgd_deact_enter() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_common_deact_enter

DESCRIPTION
  This is a common function for handling in the transaction state DEACT_ENTER.
  If protocol needs to be deactivated, it sets the next trans_state to
  DEACTD_CNF else sets the next trans_state PH_STAT_ENTER.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Should wait for deactivated confirmation.
  MMOC_EVT_STATUS_NOT_CONSUMED - Start ph_stat_enter processing.


SIDE EFFECTS
  None
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_common_deact_enter
(
        prot_deact_e_type              deact_reason,
           /* Reason for deactivating the active protocol.
           */

        mmoc_state_info_s_type         *mmoc_info_ptr
           /* Pointer to MMoC's state information.
           */

)
{

  /* Send the deactivate request to active protocol.
  */
  if ( mmoc_deactivate_active_protocol( deact_reason,
                                            mmoc_info_ptr
                                          )
     )
  {
     /* Protocol deactivate request sent, wait for confirmation.
     */
     mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_DEACTD_CNF,
                              mmoc_info_ptr
                            );
     return MMOC_EVT_STATUS_CONSUMED;

  }

#if defined MMOC_GWL_SUPPORTED
  if ( mmoc_deactivate_gwl_protocol(   deact_reason,
                                       mmoc_info_ptr
                                    )
     )
  {
     /* Protocol deactivate request sent, wait for confirmation.
     */
     mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_DEACTD_CNF_GWL,
                              mmoc_info_ptr
                            );


     return MMOC_EVT_STATUS_CONSUMED;

  }

#endif //MMOC_GWL_SUPPORTED


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* No active protocols present, start sending the phone status
  ** changed command.
  */
  mmoc_update_trans_state( MMOC_TRANS_STATE_PROT_PH_STAT_ENTER,
                           mmoc_info_ptr
                         );


  return MMOC_EVT_STATUS_NOT_CONSUMED;


} /* mmoc_process_common_deact_enter() */

/*===========================================================================

FUNCTION mmoc_buf_auto_deact_ind_rpt

DESCRIPTION
 This function will buffer the auto_deact_ind report, by making 
 mmoc_info_ptr->buffered_auto_deact point to it.
 
DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.

SIDE EFFECTS
  None.
===========================================================================*/
mmoc_evt_status_e_type  mmoc_buf_auto_deact_ind_rpt
(

       const mmoc_rpt_msg_s_type      *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For the received auto_deact_ind report, check:
  ** 1.If waiting for DEACT_CNF on same stack.
  ** 2.If waiting for DEACT_CNF from GW protocol and a GW protocol is active
       on same stack,
  ** set prot_state to NULL and discard auto_deact.
  ** Else, buffer the auto_deact and set is_reused flag to TRUE so report is not
  ** from rpt queue. */
  if((mmoc_info_ptr->trans_state == MMOC_TRANS_STATE_WAIT_DEACTD_CNF ||
     (mmoc_info_ptr->trans_state == MMOC_TRANS_STATE_WAIT_DEACTD_CNF_GWL
      && mmoc_info_ptr->prot_state == PROT_STATE_ONLINE_GWL)))
  {
    mmoc_update_prot_state(PROT_STATE_NULL,
                           mmoc_info_ptr );

  }
  else
  {
    mmoc_info_ptr->buffered_auto_deact = (mmoc_rpt_msg_s_type *)rpt_ptr;
    mmoc_info_ptr->buffered_auto_deact->rpt.is_reused = TRUE;

    MMOC_MSG_HIGH_2("BUF_AUTO_DEACT: rpt_ptr->is_reused:%d prot_stt: %d",
                           mmoc_info_ptr->buffered_auto_deact->rpt.is_reused,
                           mmoc_info_ptr->prot_state);
  }

  return MMOC_EVT_STATUS_CONSUMED;
  
}/* mmoc_buf_auto_deact_ind_rpt() */

/*===========================================================================
FUNCTION mmoc_is_auto_deact_ind_rpt_buf

DESCRIPTION
 This function will check if auto_deact_ind is buffered for any stack.
 If yes, it will return stack index, else it will return SD_SS_MAX.

DEPENDENCIES
  None.

RETURN VALUE
  stack_idx - on which auto deact is buffered
  SD_SS_MAX - else

SIDE EFFECTS
  None.
===========================================================================*/
sd_ss_e_type mmoc_is_auto_deact_ind_rpt_buf
(

       mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */
)
{

  if(mmoc_is_auto_deact_ind_rpt_buf_per_stack(mmoc_info_ptr, FALSE))
  {
    return SD_SS_MAIN;
  }
  return SD_SS_MAX;
   
}/* mmoc_is_auto_deact_ind_rpt_buf() */

/*===========================================================================
FUNCTION mmoc_is_auto_deact_ind_rpt_buf

DESCRIPTION
 This function will check if auto_deact_ind is buffered for particular ss.
 If yes, it return TRUE, else by default it will return FALSE .
 If is_discard is TRUE and buffered auto deact ind is found, it will
 discard that stored indication and set prot_state to NULL.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if auto_deact report was buffered for ss
  FALSE - else   

SIDE EFFECTS
  None.
===========================================================================*/
boolean         mmoc_is_auto_deact_ind_rpt_buf_per_stack
(

       mmoc_state_info_s_type         *mmoc_info_ptr,
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */
        boolean                        is_discard

)
{

  if(mmoc_info_ptr->buffered_auto_deact != NULL)
  {
    MMOC_MSG_HIGH_1("AUTO_DEACT buffered ,discard_flag:%d",is_discard);
    if(is_discard)
    {  
      /* to discard the report, free the memory and set the buffer to NULL
      */
      mmoc_mem_free( mmoc_info_ptr->buffered_auto_deact );
      mmoc_info_ptr->buffered_auto_deact = NULL;
	  		
      mmoc_update_prot_state(PROT_STATE_NULL,
		                     mmoc_info_ptr );
     }
	   
     return TRUE;
  }
  return FALSE;
}/* mmoc_is_auto_deact_ind_rpt_buf() */

/*===========================================================================

FUNCTION mmoc_process_deactd_cnf_gwl

DESCRIPTION
 This function handles the processing  in the WAIT_DEACTD_CNF_GWL
 transaction state.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_deactd_cnf_gwl
(

        const mmoc_rpt_msg_s_type      *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  MMOC_ASSERT( rpt_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the rpt ptr is NULL
  */
  if ( rpt_ptr == NULL )
  {
    /* Not the expected report.
    */
    return MMOC_EVT_STATUS_CONSUMED;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MMOC_MSG_HIGH_3("deact_cnf_gwl, rpt name %d trans_id %d %d",rpt_ptr->rpt.name, mmoc_info_ptr->trans_id, rpt_ptr->param.prot_deactd_cnf.trans_id );
  /* Process the received report.
  */
  switch (rpt_ptr->rpt.name )
  {
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --*/
    case MMOC_RPT_PROT_AUTO_DEACTD_IND:
      return  mmoc_buf_auto_deact_ind_rpt(rpt_ptr,mmoc_info_ptr);

    case MMOC_RPT_PROT_DEACTD_CNF:
      if(mmoc_info_ptr->trans_id != rpt_ptr->param.prot_deactd_cnf.trans_id)
      {
        /* Not the expected report. Go back to waiting for cnf
        */
        return MMOC_EVT_STATUS_CONSUMED;
      }

      /* Expected rpt.
      */

      /* Multiple protocols have to be deactivated, trans_state has to be
      ** set to DEACT_ENTER.
      */
      mmoc_update_trans_state( MMOC_TRANS_STATE_PROT_DEACT_ENTER,
                               mmoc_info_ptr
                             );

      /* If we reached here, prot state needs to be updated.
      */
      if(mmoc_info_ptr->prot_state == PROT_STATE_ONLINE_GWL ||
        mmoc_info_ptr->prot_state == PROT_STATE_PWR_SAVE
        )
      {
        mmoc_update_prot_state( PROT_STATE_NULL,
                               mmoc_info_ptr
                             );
      }

      return MMOC_EVT_STATUS_NOT_CONSUMED;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    default:
      return MMOC_EVT_STATUS_CONSUMED;

  } /* switch ( rpt_ptr->rpt.name ) */

} /* mmoc_process_deactd_cnf_gwl() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_common_deactd_cnf

DESCRIPTION
 This function handles the common processing  in the WAIT_DEACTD_CNF
 transaction state.

  In this state, the state handler will
  1. Validate the deactivated confirmation report received
  2. Move the trans. state to send the phone status command.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_common_deactd_cnf
(

        const mmoc_rpt_msg_s_type      *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
 
  MMOC_ASSERT( rpt_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the rpt ptr is NULL
  */
  if ( rpt_ptr == NULL )
  {
    /* Not the expected report.
    */
    return MMOC_EVT_STATUS_CONSUMED;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Process the received report.
  */
  switch (rpt_ptr->rpt.name )
  {
    case MMOC_RPT_PROT_AUTO_DEACTD_IND:
      return mmoc_buf_auto_deact_ind_rpt(rpt_ptr,mmoc_info_ptr);

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - --*/
    case MMOC_RPT_PROT_DEACTD_CNF:
      if(mmoc_info_ptr->trans_id != rpt_ptr->param.prot_deactd_cnf.trans_id)
      {
        /* Not the expected report. Go back to waiting for cnf
        */
        MMOC_MSG_HIGH_2("Unexpected event received; mmoc_info_ptr->trans_id: %d, \
                        rpt_ptr->param.prot_deactd_cnf.trans_id: %d",
                        mmoc_info_ptr->trans_id,
                        rpt_ptr->param.prot_deactd_cnf.trans_id);
        return MMOC_EVT_STATUS_CONSUMED;
      }

      /* Expected rpt.
      */

      mmoc_sanity_timer_reset(mmoc_info_ptr);

      /* Multiple protocols have to be deactivated, trans_state has to be
      ** set to DEACT_ENTER.
      */
      mmoc_update_trans_state( MMOC_TRANS_STATE_PROT_DEACT_ENTER,
                               mmoc_info_ptr
                             );

      if (mmoc_info_ptr->curr_trans == MMOC_TRANS_SUBSC_CHGD)
      {
        boolean is_subsc_available = 0;
        {
          is_subsc_available = mmoc_info_ptr->is_gwl_subs_avail;

        }


        MMOC_MSG_HIGH_2("common_deact_cnf: subav=%d sub avail info:gwl %d, power save enter called",
                            is_subsc_available,
                            mmoc_info_ptr->is_gwl_subs_avail
                            );



        (void) sd_ss_ind_user_prot_deactivate(NULL, SD_SS_PROT_DEACT_INFO_SUBS_CHANGE_TRANS);
        (void) sd_ss_ind_misc_pwr_save_enter(NULL);

      }
     

      MMOC_MSG_HIGH_2( "main_last_prot_state:%d,prot_state:%d",                      
                      mmoc_info_ptr->main_last_prot_state, mmoc_info_ptr->prot_state);

       /* Check if the deact cnf is for MAIN stack and the last protocol state of MAIN when deact_req
              was sent and the current protocol state of MAIN is same or not, if not , dont update the
              protocol state of MAIN to NULL 
          */
       if(!(mmoc_info_ptr->main_last_prot_state != mmoc_info_ptr->prot_state))

      {
        mmoc_info_ptr->main_last_prot_state = PROT_STATE_NULL;
        mmoc_update_prot_state( PROT_STATE_NULL,
                               mmoc_info_ptr
                             );
      }


      return MMOC_EVT_STATUS_NOT_CONSUMED;

    default:
      MMOC_MSG_HIGH_1( "Unexpected report received for deact_cnf %d", rpt_ptr->rpt.name);
      return MMOC_EVT_STATUS_CONSUMED;

  } /* switch ( rpt_ptr->rpt.name ) */

} /* mmoc_process_common_deactd_cnf() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_common_ps_detach_cnf

DESCRIPTION
 This function handles the common processing  in the PS DETACH CNF
 transaction state.

  In this state, the state handler will
  1. Validate the detach confirmation report received
  2. Move the trans. state to send the phone status command.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_common_ps_detach_cnf
(

        const mmoc_rpt_msg_s_type      *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  MMOC_ASSERT( rpt_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the rpt ptr is NULL
  */
  if ( rpt_ptr == NULL )
  {
    /* Not the expected report.
    */
    return MMOC_EVT_STATUS_CONSUMED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Process the received report.
  */
  switch (rpt_ptr->rpt.name )
  {
    case MMOC_RPT_PROT_AUTO_DEACTD_IND:
      return  mmoc_buf_auto_deact_ind_rpt(rpt_ptr,mmoc_info_ptr);

    case MMOC_RPT_PS_DETACH_CNF:

      if(mmoc_info_ptr->trans_id != rpt_ptr->param.ps_detach_cnf.trans_id)
      {
        /* Not the expected report. Go back to waiting for cnf
        */
        MMOC_ERR_0(" Unexpected event received ");
        return MMOC_EVT_STATUS_CONSUMED;
      }

      /* Move back to GEN CMD enter state
      */
      mmoc_update_trans_state( MMOC_TRANS_STATE_GEN_CMD_ENTER,
                               mmoc_info_ptr
                             );


      return MMOC_EVT_STATUS_NOT_CONSUMED;


    default:
        MMOC_ERR_1( "Unexpected report received %d",
                        rpt_ptr->rpt.name);
        return MMOC_EVT_STATUS_CONSUMED;


  } /* switch ( rpt_ptr->rpt.name ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
}// mmoc_process_common_ps_detach_cnf

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_common_ph_stat_cnf

DESCRIPTION
   This function handles the common processing in the
 WAIT_PH_STAT_CNF transaction state.

  In this state, the state handler will
  1. Validate the phone status command ack received from the protocols.
  2. Move to PH_STAT_ENTER state to determine if phone status command
     has to be sent.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_common_ph_stat_cnf
(

        const mmoc_rpt_msg_s_type      *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{

  MMOC_ASSERT( rpt_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if this is the correct ack.
  */
  if ( rpt_ptr == NULL ||
       (rpt_ptr->rpt.name == MMOC_RPT_PH_STAT_CHGD_CNF &&
       ph_stat_cmd_tbl.trans_id  !=
                                        rpt_ptr->param.ph_stat_cnf.trans_id)
     )
  {
    /* Not the expected report.
    */
    MMOC_MSG_HIGH_0("Ph_stat_chgd_cnf: Trans id mismatch");
    return MMOC_EVT_STATUS_CONSUMED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!MMOC_ENUM_IS_INRANGE(rpt_ptr->param.ph_stat_cnf.prot,MMOC_MODE_MAX))
  {
      /* Not the expected report.
      */
      MMOC_ERR_0("Ph_stat_chgd_cnf:prot name error");
      return MMOC_EVT_STATUS_CONSUMED;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(rpt_ptr->rpt.name)
  {
    case MMOC_RPT_PROT_AUTO_DEACTD_IND:
      return   mmoc_buf_auto_deact_ind_rpt(rpt_ptr,mmoc_info_ptr);

     /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case MMOC_RPT_PH_STAT_CHGD_CNF:
      if(rpt_ptr->param.ph_stat_cnf.mc_status == MMOC_PH_STAT_ERR_SIM_BUSY)
      {
        mmoc_send_cm_rpt( CM_MMOC_SUBSCRIPTION_FAILURE, NULL);
      }
      /* Clear the pending cnf for protocol.
      */
      ph_stat_cmd_tbl.trans_id
                                                           = PROT_TRANS_NONE;
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
      /* Check if all the confirmation have been received.
      */
      {

        if (ph_stat_cmd_tbl.trans_id != PROT_TRANS_NONE )
        {
          /* Still waiting for additional "cnf".
          */
          return MMOC_EVT_STATUS_CONSUMED;
        }

      }
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* All cnf. recvd. move to PH_STAT_ENTER.
      */
      mmoc_update_trans_state( MMOC_TRANS_STATE_PROT_PH_STAT_ENTER,
                               mmoc_info_ptr
                             );


      return MMOC_EVT_STATUS_NOT_CONSUMED;
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    default:
      /* Not the expected report.
      */
      MMOC_ERR_0("Unknown report");
      return MMOC_EVT_STATUS_CONSUMED;

  } //switch(rpt_ptr->rpt.name)

} /* mmoc_process_common_ph_stat_cnf() */

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== SUBSCRIPTION CHANGED TRANSACTION =======================
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_subsc_chgd_null

DESCRIPTION
  This function handles the subscription changed transaction in the NULL
  transaction state.

  In this state, the state handler will

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,rpt_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_subsc_chgd_null
(

        const mmoc_cmd_msg_s_type      *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type      *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{


  MMOC_ASSERT( cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The subscription changed command from CM should only create this
  ** transaction.
  */
  if ( cmd_ptr == NULL ||
       cmd_ptr->cmd.name != MMOC_CMD_SUBSCRIPTION_CHGD
     )
  {
    MMOC_ERR_0("Subsc chgd trans incorrectly initiated");

    return mmoc_clear_transaction( mmoc_info_ptr );
  }

  /* Print mode pref in subsc command from CM */


  
  MMOC_MSG_HIGH_4("MMOC_SUB_DATA: MAIN: orig_mode=%d, band_pref=0x%x 0x%x prot_subsc_chg 0x%x",
                cmd_ptr->param.subsc_chgd.gw.orig_mode,
                QWORD_HIGH(cmd_ptr->param.subsc_chgd.gw.band_pref),
                QWORD_LOW(cmd_ptr->param.subsc_chgd.gw.band_pref),
                cmd_ptr->param.subsc_chgd.prot_subsc_chg);
  mmoc_print_lte_band_mask(cmd_ptr->param.subsc_chgd.gw.lte_band_pref);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Update the NAM and subscription availability status in
  ** the MMoC state info.
  */
  mmoc_info_ptr->prot_subsc_chg = cmd_ptr->param.subsc_chgd.prot_subsc_chg;
  mmoc_info_ptr->is_sd_init_called = FALSE;

  #if !(defined(FEATURE_MMODE_LTE_ONLY) || defined(FEATURE_MMODE_GPRS_LTE))
  mmoc_info_ptr->trans_info.subsc_chgd.gw.is_perso_locked = cmd_ptr->param.subsc_chgd.gw.is_perso_locked;
  #endif


  if(mmoc_info_ptr->prot_subsc_chg & PROT_SUBSC_CHG_GW)
  {
    mmoc_info_ptr->is_gwl_subs_avail =
                 cmd_ptr->param.subsc_chgd.gw.is_subs_avail;
  }


  /*lint +e655 */
  /* Initialize the transaction info.
  */
  mmoc_info_ptr->trans_info.subsc_chgd = cmd_ptr->param.subsc_chgd;

  /* Deactivate the active protocols, Move to DEACT_ENTER trans.
  ** state.
  */
  mmoc_update_trans_state( MMOC_TRANS_STATE_PROT_DEACT_ENTER,
                           mmoc_info_ptr
                         );


  return MMOC_EVT_STATUS_NOT_CONSUMED;

} /* mmoc_process_subsc_chgd_null() */
/*lint +esym(715,rpt_ptr)*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_subsc_ph_stat_enter

DESCRIPTION
  This function handles the offline transaction in the PH_STAT_ENTER
  transaction state.

  In this transaction state, the handler will
  1. Send the Phone status changed command to the next supported protocol
     and move to WAIT_PH_STAT_CNF.
     If the phone status changed command was sent to all the protocols,
     then SD is initialized with new subscription and activate the protocol
     if the operating mode is online.


DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,cmd_ptr)*/
/*lint -esym(715,rpt_ptr)*/
static  mmoc_evt_status_e_type       mmoc_process_subsc_chgd_ph_stat_enter(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  /* Phone status chg type.
  */
  prot_ph_stat_chg_e_type chg_type = PROT_PH_STAT_CHG_MAX;

  /* SD action.
  */
  sd_ss_act_e_type sd_act;
  boolean is_subsc_chg = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Determine the phone status change type
  */
  if ( mmoc_info_ptr->trans_info.subsc_chgd.chg_type
                 == MMOC_SUBSC_CHG_SUBSC_AVAIL )
  {
    chg_type = PROT_PH_STAT_CHG_SUBSC;
  }
  else if ( mmoc_info_ptr->trans_info.subsc_chgd.chg_type
                  == MMOC_SUBSC_CHG_NAM_SEL )
  {
    chg_type = PROT_PH_STAT_CHG_NAM;
  }
  MMOC_MSG_HIGH_3("chg_type=PH_STAT_CHG, chg_type %d, sd_initialized %d, sd_init_called %d",
                   chg_type,
                   mmoc_info_ptr->is_sd_initialized,
                   mmoc_info_ptr->is_sd_init_called);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Intialise SD only if it is not intialised before*/
  if(!mmoc_info_ptr->is_sd_initialized &&
       !mmoc_info_ptr->is_sd_init_called)
  {

    sd_init2( MMOC_NV_CMD_SIG, CM_RPT_TIMER_SIG, 
                 cm_kick_dog,
                 mmoc_ss_main_timer_cb, 
                 NULL,                 
                 NULL,
                 NULL);

    mmoc_info_ptr->is_sd_init_called = TRUE;
  }

  /* Send the phone status changed command to the next supported
  ** protocol.
  */
  if ( mmoc_send_subsc_chg_ph_stat_chgd_cmd( chg_type,
                                             mmoc_info_ptr
                                            )
     )
  {
    /* Phone status changed command sent, so wait for ack.
    */
    mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_PH_STAT_CNF,
                             mmoc_info_ptr
                           );
    return MMOC_EVT_STATUS_CONSUMED;
  }

  /* If either CDMA or GW subscription has changed, update SD with this info
  */
  if (CHECK_PROTO(mmoc_info_ptr->prot_subsc_chg ,PROT_SUBSC_CHG_GW))
  {
    sd_nam_sel5 ( (sd_nam_e_type) mmoc_info_ptr->trans_info.subsc_chgd.nam,
                      mmoc_info_ptr->trans_info.subsc_chgd.gw.orig_mode,
                      mmoc_info_ptr->trans_info.subsc_chgd.gw.mode_pref,
                      mmoc_info_ptr->trans_info.subsc_chgd.gw.band_pref,
                      mmoc_info_ptr->trans_info.subsc_chgd.gw.lte_band_pref,
                      mmoc_info_ptr->trans_info.subsc_chgd.gw.tds_band_pref,
                      SD_SS_PRL_PREF_NONE,
                      mmoc_info_ptr->trans_info.subsc_chgd.gw.roam_pref,
                      mmoc_info_ptr->trans_info.subsc_chgd.gw.hybr_pref,
                      &mmoc_info_ptr->trans_info.subsc_chgd.gw.gw.manual_sys_info,
                      mmoc_info_ptr->trans_info.subsc_chgd.gw.gw.srv_domain_pref,
                      mmoc_info_ptr->trans_info.subsc_chgd.gw.gw.acq_order_pref,
                      mmoc_info_ptr->is_gwl_subs_avail,
                      0,
                      0,
                      0,
                      mmoc_info_ptr->trans_info.subsc_chgd.gw.ciot_lte_pref
                              );

  

  }/* If either CDMA or GW subscription has changed */

  /* Set MMoC's state info to SD initialized.
  */
  mmoc_set_sd_initialized( mmoc_info_ptr );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if we can vote off UIM.
  */
  #ifdef FEATURE_UIM_RUIM
  if(mmoc_info_ptr->is_gwl_subs_avail
  )
  {
    /* Indicate to power down the RUIM */
    (void)uim_power_control(UIM_MC, FALSE);

  }
  #endif /* FEATURE_UIM_RUIM */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If the current operating mode status is ONLINE, activate the protocol.
  */
  if ( mmoc_info_ptr->curr_oprt_mode == PROT_OPRT_MODE_ONLINE )
  {
    mmoc_print_pwrup_parameters(mmoc_info_ptr);

    /* Call the SD indication depending on whether sd was initialized here
    ** or if the subsc. changed type is because the subscription is
    ** available or not.
    */
    if (mmoc_info_ptr->trans_info.subsc_chgd.chg_type ==
                                        MMOC_SUBSC_CHG_SUBSC_AVAIL
       )
    {
  
      /* If Main subscription has changed (or)
      ** Main is still active based on standby preferences
      */
      if(mmoc_info_ptr->prot_subsc_chg != PROT_SUBSC_CHG_NONE)
      {
         /* Power-up without unlocking CDMA */
         sd_act = sd_ss_ind_user_pwr_up( FALSE, NULL );

         if ( mmoc_activate_prot_online_ss_main( sd_act, mmoc_info_ptr)
                                           == MMOC_EVT_STATUS_NOT_CONSUMED)
         {
           return MMOC_EVT_STATUS_NOT_CONSUMED;
        }

      }
    }
    }
  else if (mmoc_info_ptr->curr_oprt_mode == PROT_OPRT_MODE_FTM
          )
  {
    /* Activate the protocol, if needed in the offline mode.
    */
    mmoc_activate_prot_offline( mmoc_info_ptr );

  }

  /* Clear the transaction.
  */
  return mmoc_clear_transaction( mmoc_info_ptr );


} /* mmoc_process_subsc_chgd_ph_stat_enter() */
/*lint +esym(715,cmd_ptr)*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_subsc_chgd

DESCRIPTION
  This function is the transaction handler for the subscription changed
  transaction.

  The transaction state transition will be,

 NULL->DEACT_ENTER->WAIT_DEACTD_CNF->MMGSDI_READ_ENTER->MMGSDI_READ_CNF->PH_STAT_ENTER<->WAIT_PH_STAT_CNF
              |             |               ^                                 ^   |
              |_____________|_______________|                                 |   |
              |             |                                                 |   |
              |_____________|_________________________________________________|   |______>NULL

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_subsc_chgd
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  mmoc_evt_status_e_type evt_status = MMOC_EVT_STATUS_CONSUMED;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Invoke the appropriate transaction state handlers.
  */
  switch(mmoc_info_ptr->trans_state)
  {
    case MMOC_TRANS_STATE_NULL:
      evt_status = mmoc_process_subsc_chgd_null( cmd_ptr,
                                                 rpt_ptr,
                                                 mmoc_info_ptr
                                               );
      break;

    case MMOC_TRANS_STATE_PROT_DEACT_ENTER:
      evt_status =  mmoc_process_subsc_chgd_deact_enter( PROT_DEACT_SUBSC_CHGD,
                                                         mmoc_info_ptr
                                                        );
      break;

    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF:
      evt_status = mmoc_process_common_deactd_cnf( rpt_ptr,
                                                   mmoc_info_ptr
                                                 );
      break;

    case MMOC_TRANS_STATE_PROT_PH_STAT_ENTER:
      evt_status = mmoc_process_subsc_chgd_ph_stat_enter( cmd_ptr,
                                                          rpt_ptr,
                                                          mmoc_info_ptr
                                                        );
      break;

    case MMOC_TRANS_STATE_WAIT_PH_STAT_CNF:
      evt_status = mmoc_process_common_ph_stat_cnf( rpt_ptr,
                                                    mmoc_info_ptr
                                                  );
      break;
    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF_GWL:
      evt_status = mmoc_process_deactd_cnf_gwl( rpt_ptr,
                                                   mmoc_info_ptr
                                                 );
      break;

    default:
      MMOC_ERR_1("trans_state", mmoc_info_ptr->trans_state);
      evt_status = mmoc_clear_transaction( mmoc_info_ptr );
      break;
  }


  return evt_status;

} /* mmoc_process_subsc_chgd() */



/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== ONLINE COMMAND TRANSACTION =============================
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_online_null

DESCRIPTION
  This function handles the online transaction in the NULL transaction state.

  In this state, the state handler will
  1. Call the power up SD indication if SD was initialized earlier and
     activate the protocol was indicated by SD action.


DEPENDENCIES

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,rpt_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_online_null
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  MMOC_ASSERT( cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* The online operating mode changed command from CM should only create this
  ** transaction.
  */
  if ( cmd_ptr == NULL ||
       cmd_ptr->cmd.name != MMOC_CMD_OPRT_MODE_CHGD  ||
       cmd_ptr->param.oprt_mode_chgd.oprt_mode != SYS_OPRT_MODE_ONLINE
     )
  {
    MMOC_ERR_0("Online trans incorrectly initiated");

    return mmoc_clear_transaction( mmoc_info_ptr );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Should be in power off or online mode or LPM currently.
  */
  if ( mmoc_info_ptr->curr_oprt_mode != PROT_OPRT_MODE_NULL   &&
       mmoc_info_ptr->curr_oprt_mode != PROT_OPRT_MODE_ONLINE &&
       mmoc_info_ptr->curr_oprt_mode != PROT_OPRT_MODE_LPM &&
       mmoc_info_ptr->curr_oprt_mode != PROT_OPRT_MODE_FTM
     )
  {
    MMOC_ERR_1( "Incorrect current oprt mode %d",
                    mmoc_info_ptr->curr_oprt_mode);
    return mmoc_clear_transaction( mmoc_info_ptr );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize the operating mode info.
  */
  mmoc_info_ptr->curr_oprt_mode    = PROT_OPRT_MODE_ONLINE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Deactivate any protocols that are active.
  */
  mmoc_update_trans_state( MMOC_TRANS_STATE_PROT_DEACT_ENTER,
                           mmoc_info_ptr
                         );

  return MMOC_EVT_STATUS_NOT_CONSUMED;


} /* mmoc_process_online_null() */
/*lint +esym(715,rpt_ptr)*/


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_online_ph_stat_enter

DESCRIPTION
  This function handles the offline transaction in the PH_STAT_ENTER
  transaction state.

  In this transaction state, the handler will
  1. Send the Phone status changed command to the next supported protocol
     and move to WAIT_PH_STAT_CNF.
     If the phone status changed command was sent to all the protocols,
     then SD is initialized with new subscription and activate the protocol
     if the operating mode is online.


DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,cmd_ptr)*/
/*lint -esym(715,rpt_ptr)*/
static  mmoc_evt_status_e_type       mmoc_process_online_ph_stat_enter(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  /* Phone status chg type.
  */
  prot_ph_stat_chg_e_type         chg_type = PROT_PH_STAT_CHG_OPRT_MODE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Send the phone status changed command to the next supported
  ** protocol.
  */
  if ( mmoc_send_ph_stat_chgd_cmd( chg_type,
                                   mmoc_info_ptr
                                 )
     )
  {
    /* Phone status changed command sent, so wait for ack.
    */
    mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_PH_STAT_CNF,
                             mmoc_info_ptr
                           );
    return MMOC_EVT_STATUS_CONSUMED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if SD was initialized earlier.
  */
  if ( mmoc_info_ptr->is_sd_initialized )
  {
     TTLBOOTUP(MMOC_ONLINE);
     mmoc_print_pwrup_parameters(mmoc_info_ptr);

    /* SD subscription init was successful, start from pwr_up.
    ** and unlock CDMA if locked.
    */

     if(mmoc_activate_prot_online_ss_main(
          sd_ss_ind_user_pwr_up(TRUE, NULL),mmoc_info_ptr) ==
            MMOC_EVT_STATUS_NOT_CONSUMED)
     {
       /* Invoke the new transaction handler.
       */
       return MMOC_EVT_STATUS_NOT_CONSUMED;
     }


  } /*  if ( mmoc_info_ptr->is_sd_initialized ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Clear the transaction.
  */
  return mmoc_clear_transaction( mmoc_info_ptr );


} /* mmoc_process_online_ph_stat_enter() */
/*lint +esym(715,cmd_ptr)*/
/*lint +esym(715,rpt_ptr)*/



/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_online

DESCRIPTION
  This function is the transaction handler for the online transaction.

  The transaction state transition will be,

     NULL -> NULL


DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_online
(

        const mmoc_cmd_msg_s_type      *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type      *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  mmoc_evt_status_e_type  evt_status = MMOC_EVT_STATUS_CONSUMED;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Invoke the appropriate transaction state handlers.
  */
  switch(mmoc_info_ptr->trans_state)
  {
    case MMOC_TRANS_STATE_NULL:
      evt_status = mmoc_process_online_null( cmd_ptr,
                                             rpt_ptr,
                                             mmoc_info_ptr
                                           );
      break;

    case MMOC_TRANS_STATE_PROT_DEACT_ENTER:
      evt_status = mmoc_process_common_deact_enter( PROT_DEACT_SUBSC_CHGD,
                                                    mmoc_info_ptr
                                                  );
      break;

    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF:
      evt_status = mmoc_process_common_deactd_cnf( rpt_ptr,
                                                   mmoc_info_ptr
                                                 );
      break;

    case MMOC_TRANS_STATE_PROT_PH_STAT_ENTER:
      evt_status = mmoc_process_online_ph_stat_enter( cmd_ptr,
                                                      rpt_ptr,
                                                      mmoc_info_ptr
                                                    );
      break;

    case MMOC_TRANS_STATE_WAIT_PH_STAT_CNF:
      evt_status = mmoc_process_common_ph_stat_cnf( rpt_ptr,
                                                    mmoc_info_ptr
                                                  );
      break;

    default:
      MMOC_ERR_1("error Trans_state %d", mmoc_info_ptr->trans_state);
      evt_status = mmoc_clear_transaction( mmoc_info_ptr );
      break;
  }

  return evt_status;

} /* mmoc_process_online() */

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== OFFLINE COMMAND TRANSACTION ============================
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================

FUNCTION mmoc_process_offline_null

DESCRIPTION
  This function handles the offline transaction in the NULL transaction
  state.

  In this state, the state handler will
  1. Verify that offline oprt mode command invoked this transaction.
  2. Initialize the current operating mode.
  3. Move the Deact. Enter state.


DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,rpt_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_offline_null
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  MMOC_ASSERT( cmd_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* The offline operating mode changed command from CM should only create
  ** this transaction.
  */
  if ( cmd_ptr == NULL ||
       cmd_ptr->cmd.name != MMOC_CMD_OPRT_MODE_CHGD ||
       ( cmd_ptr->param.oprt_mode_chgd.oprt_mode !=
                                               SYS_OPRT_MODE_OFFLINE &&
         cmd_ptr->param.oprt_mode_chgd.oprt_mode !=
                                               SYS_OPRT_MODE_FTM &&
         cmd_ptr->param.oprt_mode_chgd.oprt_mode !=
                                            SYS_OPRT_MODE_PSM

       )
     )
  {
    MMOC_ERR_0("error cmd");

    return mmoc_clear_transaction( mmoc_info_ptr );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize the current operating mode and phone status sent index.
  */
  mmoc_info_ptr->curr_oprt_mode = mmoc_map_sys_oprt_mode_to_mmoc
                                  (
                                    cmd_ptr->param.oprt_mode_chgd.oprt_mode
                                  );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Move to DEACT_ENTER trans. state.
  */
  mmoc_update_trans_state( MMOC_TRANS_STATE_PROT_DEACT_ENTER,
                           mmoc_info_ptr
                         );


  return MMOC_EVT_STATUS_NOT_CONSUMED;

} /* mmoc_process_offline_null() */
/*lint +esym(715,rpt_ptr)*/


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_offline_deact_enter

DESCRIPTION
  This function is responsible for the sending the deactivate protocol
  request to the active protocol if present. Else, moves the transaction
  to the state which sends the Phone status change command.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,rpt_ptr)*/
/*lint -esym(715,cmd_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_offline_deact_enter
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  prot_deact_e_type  deact_reason = PROT_DEACT_MAX;


  /* Send the deactivate request to active protocol.
  */
  switch ( mmoc_info_ptr->curr_oprt_mode)
  {
    case PROT_OPRT_MODE_OFFLINE:
      deact_reason = PROT_DEACT_OFFLINE;
      break;

    case PROT_OPRT_MODE_FTM:
      deact_reason = PROT_DEACT_FTM;
      break;

    case PROT_OPRT_MODE_PSM:
      deact_reason = PROT_DEACT_PSM;
      break;

    case PROT_OPRT_MODE_NULL:
    case PROT_OPRT_MODE_PWROFF:
    case PROT_OPRT_MODE_ONLINE:
    case PROT_OPRT_MODE_LPM:
    case PROT_OPRT_MODE_RESET:
    case PROT_OPRT_MODE_MAX:
    default:
      sys_err_fatal_invalid_value_exception((int)mmoc_info_ptr->curr_oprt_mode);
      break;

   } /* switch( mmoc_info_ptr->curr_oprt_mode) */



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Perform the common deactivated enter state handling.
  */
  return mmoc_process_common_deact_enter( deact_reason,
                                          mmoc_info_ptr
                                        );

} /* mmoc_process_offline_deact_enter() */
/*lint +esym(715,rpt_ptr)*/
/*lint +esym(715,cmd_ptr)*/



/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_offline_ph_stat_enter

DESCRIPTION
  This function handles the offline transaction in the  PH_STAT_ENTER
  transaction state.

  In this transaction state, the handler will
  1. Send the Phone status changed command to the next supported protocol
     and move to WAIT_PH_STAT_CNF.
     If the phone status changed command was sent to all the protocols,
     SD is informed of the offline oprt mode and activate the protocol
     in offline mode if needed.


DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,rpt_ptr)*/
/*lint -esym(715,cmd_ptr)*/
static  mmoc_evt_status_e_type       mmoc_process_offline_ph_stat_enter(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Send the phone status changed command to the next supported
  ** protocol.
  */
  if ( mmoc_send_ph_stat_chgd_cmd( PROT_PH_STAT_CHG_OPRT_MODE,
                                   mmoc_info_ptr
                                 )
     )
  {
    /* Phone status changed command sent, so wait for ack.
    */
    mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_PH_STAT_CNF,
                             mmoc_info_ptr
                           );
    return MMOC_EVT_STATUS_CONSUMED;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If SD was previously initialized, inform SD of offline mode.
  */
  if ( mmoc_info_ptr->is_sd_initialized )
  {
    /* Done with sending the Phone status changed commands, Call SD
    ** indication to inform of offline mode.
    */
    switch (mmoc_info_ptr->curr_oprt_mode)
    {
      case PROT_OPRT_MODE_OFFLINE:
      case PROT_OPRT_MODE_PSM:
        (void) sd_ss_ind_user_offline_cdma(NULL);
        break;

      case PROT_OPRT_MODE_FTM:
        (void) sd_ss_ind_user_offline_cdma(NULL);
        break;

      default:
        sys_err_fatal_invalid_value_exception((int)mmoc_info_ptr->curr_oprt_mode);
        break;

    } /* switch (mmoc_info_ptr->curr_oprt_mode) */

  } /* if ( mmoc_info_ptr->is_sd_initialized ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Activate the protocol, if needed in the offline mode */
  mmoc_activate_prot_offline( mmoc_info_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mmoc_send_cm_rpt( CM_OFFLINE_OK_F , NULL);
  mmoc_notify_offline_mode();

  MMOC_MSG_HIGH_0("OFFLINE processing complete");

  /* Clear the transaction.
  */
  return mmoc_clear_transaction( mmoc_info_ptr );



} /* mmoc_process_offline_ph_stat_enter() */
/*lint +esym(715,rpt_ptr)*/
/*lint +esym(715,cmd_ptr)*/


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_offline

DESCRIPTION
  This function is the transaction handler for the offline transaction.

  The transaction state transition will be,

 NULL -> DEACT_ENTER -> WAIT_DEACTD_CNF -> PH_STAT_ENTER <-> WAIT_PH_STAT_CNF
              |                              ^    |
              |______________________________|    |-----> NULL
DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_offline
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  mmoc_evt_status_e_type  evt_status = MMOC_EVT_STATUS_CONSUMED;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Invoke the appropriate transaction state handler.
  */
  switch(mmoc_info_ptr->trans_state)
  {
    case MMOC_TRANS_STATE_NULL:
      evt_status =  mmoc_process_offline_null( cmd_ptr,
                                               rpt_ptr,
                                               mmoc_info_ptr
                                             );
      break;

    case MMOC_TRANS_STATE_PROT_DEACT_ENTER:
      evt_status =  mmoc_process_offline_deact_enter( cmd_ptr,
                                                      rpt_ptr,
                                                     mmoc_info_ptr
                                                   );
      break;

    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF:
      evt_status =  mmoc_process_common_deactd_cnf( rpt_ptr,
                                                    mmoc_info_ptr
                                                   );
      break;

    case MMOC_TRANS_STATE_PROT_PH_STAT_ENTER:
      evt_status = mmoc_process_offline_ph_stat_enter( cmd_ptr,
                                                       rpt_ptr,
                                                       mmoc_info_ptr
                                                     );
      break;


    case MMOC_TRANS_STATE_WAIT_PH_STAT_CNF:
      evt_status = mmoc_process_common_ph_stat_cnf( rpt_ptr,
                                                    mmoc_info_ptr
                                                  );
      break;

    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF_GWL:
      evt_status =  mmoc_process_deactd_cnf_gwl( rpt_ptr,
                                                    mmoc_info_ptr
                                                   );
      break;

   default:
      MMOC_ERR_0("Received event in unknown state");
      evt_status = mmoc_clear_transaction( mmoc_info_ptr );
      break;
  }


  return evt_status;

} /* mmoc_process_offline() */


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== POWER DOWN/RESET/LPM COMMAND TRANSACTION ===============
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================

FUNCTION mmoc_process_pwr_down_null

DESCRIPTION
  This function handles the power down/reset/lpm transaction in the NULL
  transaction state.

  In this state, the state handler will
  1. Verify that offline oprt mode command invoked this transaction.
  2. Initialize the current operating mode.
  3. Move the Deact. Enter state.


DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,rpt_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_pwr_down_null
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{

  MMOC_ASSERT( cmd_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* The pwroff/reset/lpm operating mode changed command from CM should only
  ** create this transaction.
  */
  if ( cmd_ptr == NULL  ||
       cmd_ptr->cmd.name != MMOC_CMD_OPRT_MODE_CHGD ||
        ( cmd_ptr->param.oprt_mode_chgd.oprt_mode != SYS_OPRT_MODE_PWROFF       &&
          cmd_ptr->param.oprt_mode_chgd.oprt_mode != SYS_OPRT_MODE_RESET        &&
          cmd_ptr->param.oprt_mode_chgd.oprt_mode != SYS_OPRT_MODE_RESET_MODEM  &&
          cmd_ptr->param.oprt_mode_chgd.oprt_mode != SYS_OPRT_MODE_LPM
        )
     )
  {
    MMOC_ERR_0("Powerdown/Reset/LPM command transaction incorrectly initiated");
    return mmoc_clear_transaction( mmoc_info_ptr );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize the current operating mode and phone status sent index.
  */
  mmoc_info_ptr->curr_oprt_mode = mmoc_map_sys_oprt_mode_to_mmoc
                                  (
                                    cmd_ptr->param.oprt_mode_chgd.oprt_mode
                                  );



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Move to DEACT_ENTER trans. state.
  */
  mmoc_update_trans_state( MMOC_TRANS_STATE_PROT_DEACT_ENTER,
                           mmoc_info_ptr
                         );


  return MMOC_EVT_STATUS_NOT_CONSUMED;

} /* mmoc_process_pwr_down_null() */
/*lint +esym(715,rpt_ptr)*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_pwr_down_deact_enter

DESCRIPTION
  This function is responsible for the sending the deactivate protocol
  request to the active protocol if present. Else, moves the transaction
  to the state which sends the Phone status change command.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,rpt_ptr)*/
/*lint -esym(715,cmd_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_pwr_down_deact_enter
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  prot_deact_e_type  deact_reason = PROT_DEACT_MAX;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Send the deactivate request to active protocol.
  */
  switch ( mmoc_info_ptr->curr_oprt_mode)
  {
    case PROT_OPRT_MODE_PWROFF:
      deact_reason = PROT_DEACT_PWR_DOWN;
      break;

    case PROT_OPRT_MODE_RESET:
      deact_reason = PROT_DEACT_RESET;
      break;

    case PROT_OPRT_MODE_LPM:
      deact_reason = PROT_DEACT_LPM;
      break;

   default:
      sys_err_fatal_invalid_value_exception((int)mmoc_info_ptr->curr_oprt_mode);
      break;

   } /* switch( mmoc_info_ptr->curr_oprt_mode) */



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the system event reporting service.
  */
  event_report( EVENT_POWER_DOWN );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Perform the common deactivated enter state handling.
  */
  return mmoc_process_common_deact_enter( deact_reason,
                                          mmoc_info_ptr
                                        );

} /* mmoc_process_pwr_down_deact_enter() */
/*lint +esym(715,rpt_ptr)*/
/*lint +esym(715,cmd_ptr)*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_pwr_down_ph_stat_enter

DESCRIPTION
  This function handles the offline transaction in the  PH_STAT_ENTER
  transaction state.

  In this transaction state, the handler will
  1. Send the Phone status changed command to the next supported protocol
     and move to WAIT_PH_STAT_CNF.
     If the phone status changed command was sent to all the protocols,
     Inform the SD with the  reset/lpm/powerdown indication  and
     Shutdown the SD if it is LPM or power down.



DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,rpt_ptr)*/
/*lint -esym(715,cmd_ptr)*/
static  mmoc_evt_status_e_type       mmoc_process_pwr_down_ph_stat_enter(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{

  /* Send the phone status changed command to the next supported
  ** protocol.
  */
  if ( mmoc_send_ph_stat_chgd_cmd( PROT_PH_STAT_CHG_OPRT_MODE,
                                   mmoc_info_ptr
                                 )
     )
  {
    /* Phone status changed command sent, so wait for ack.
    */
    mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_PH_STAT_CNF,
                             mmoc_info_ptr
                           );
    return MMOC_EVT_STATUS_CONSUMED;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Done with sending the Phone status changed commands, Inform SD of the
  ** operating mode change.
  */
  switch ( mmoc_info_ptr->curr_oprt_mode)
  {
    case PROT_OPRT_MODE_PWROFF:
      /* Inform system report service
      */
      event_report( EVENT_POWER_DOWN );
      mmoc_update_prot_state( PROT_STATE_PWR_DOWN,
                              mmoc_info_ptr
                            );
      mmoc_update_db_dmss_state( mmoc_info_ptr );
      /* Send thePWR OFF OK report to CM.
      */
      mmoc_send_cm_rpt( CM_PWROFF_OK_F , NULL);
      break;

    case PROT_OPRT_MODE_LPM:
      /* Inform system report service
      */
      event_report( EVENT_POWER_DOWN );
      mmoc_update_prot_state( PROT_STATE_LPM,
                              mmoc_info_ptr
                            );
      mmoc_update_db_dmss_state( mmoc_info_ptr );

      /* Send the LPM OK report to CM.
      */
      mmoc_send_cm_rpt( CM_LPM_OK_F , NULL);
      break;

    case PROT_OPRT_MODE_RESET:
      mmoc_update_prot_state( PROT_STATE_RESET,
                             mmoc_info_ptr
                            );
      mmoc_update_db_dmss_state( mmoc_info_ptr );

      /* Send the RESET OK report to CM.
      */
      mmoc_send_cm_rpt( CM_RESET_OK_F, NULL );

      break;

    default:
      sys_err_fatal_invalid_value_exception((int)mmoc_info_ptr->curr_oprt_mode);
      break;

  } /* switch( mmoc_info_ptr->curr_oprt_mode) */



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Inform and terminate System Determination.
  */
  if ( mmoc_info_ptr->is_sd_initialized &&
       mmoc_info_ptr->curr_oprt_mode !=  PROT_OPRT_MODE_RESET )
  {
    (void)sd_ss_ind_user_pwr_down( NULL );
    sd_term();
 }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Clear the transaction.
  */
  return mmoc_clear_transaction( mmoc_info_ptr );

} /* mmoc_process_pwr_down_ph_stat_enter() */
/*lint +esym(715,rpt_ptr)*/
/*lint +esym(715,cmd_ptr)*/


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_pwr_down

DESCRIPTION
  This function is the transaction handler for the power down transaction.

  The transaction state transition will be,

 NULL -> DEACT_ENTER -> WAIT_DEACTD_CNF -> PH_STAT_ENTER <-> WAIT_PH_STAT_CNF
              |                              ^    |
              |______________________________|    |-----> NULL


DEPENDENCIES
  None.


RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_pwr_down
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  mmoc_evt_status_e_type  evt_status = MMOC_EVT_STATUS_CONSUMED;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Invoke the appropriate transaction state handler.
  */
  switch(mmoc_info_ptr->trans_state)
  {
    case MMOC_TRANS_STATE_NULL:
      evt_status = mmoc_process_pwr_down_null( cmd_ptr,
                                               rpt_ptr,
                                               mmoc_info_ptr
                                             );
      break;

    case MMOC_TRANS_STATE_PROT_DEACT_ENTER:
      evt_status =  mmoc_process_pwr_down_deact_enter( cmd_ptr,
                                                       rpt_ptr,
                                                       mmoc_info_ptr
                                                     );
      break;

    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF:
      evt_status = mmoc_process_common_deactd_cnf( rpt_ptr,
                                                   mmoc_info_ptr
                                                 );
      break;

    case MMOC_TRANS_STATE_PROT_PH_STAT_ENTER:
      evt_status = mmoc_process_pwr_down_ph_stat_enter( cmd_ptr,
                                                        rpt_ptr,
                                                        mmoc_info_ptr
                                                      );
      break;

    case MMOC_TRANS_STATE_WAIT_PH_STAT_CNF:
      evt_status = mmoc_process_common_ph_stat_cnf( rpt_ptr,
                                                    mmoc_info_ptr
                                                  );
      break;


    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF_GWL:
      evt_status = mmoc_process_deactd_cnf_gwl( rpt_ptr,
                                                   mmoc_info_ptr
                                                 );
      break;

    default:
      MMOC_ERR_1("incorrect state", mmoc_info_ptr->trans_state);
      evt_status = mmoc_clear_transaction( mmoc_info_ptr );
      break;
  }


  return evt_status;

} /* mmoc_process_pwr_down() */

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== GENERIC PROTOCOL COMMAND TRANSACTION ===================
=============================================================================
=============================================================================
===========================================================================*/
/*===========================================================================

FUNCTION mmoc_process_prot_gen_cmd_null

DESCRIPTION
  This function handles the generic protocol command transaction in the NULL
  transaction state.

  In this state, the state handler will
  1. Send the generic command to  the active protocol if one is present.
     Else, if no protocol is active will call the mmoc_proc_prot_gen_cmd()
     and follow the return SD action.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,rpt_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_prot_gen_cmd_null
(

        mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type      *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{

  MMOC_ASSERT( cmd_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* The generic protocol command should only create this transaction.
  */
  if ( cmd_ptr == NULL ||
       cmd_ptr->cmd.name != MMOC_CMD_PROT_GEN_CMD
     )
  {
    return mmoc_clear_transaction( mmoc_info_ptr );
  }


  if(cmd_ptr->param.gen_prot_cmd.param.pref_sys_chgd.addl_action.action
    || mmoc_info_ptr->emerg_susp_ss_mask
    || mmoc_info_ptr->is_suspend)
  {
  MMOC_MSG_HIGH_3("addl_action: %d, emerg_susp_ss_mask %d, is_suspend %d",
    cmd_ptr->param.gen_prot_cmd.param.pref_sys_chgd.addl_action.action,
    mmoc_info_ptr->emerg_susp_ss_mask,
    mmoc_info_ptr->is_suspend);
  }
  if (cmd_ptr->param.gen_prot_cmd.param.pref_sys_chgd.addl_action.action
             & MMOC_PREF_SYS_ADDL_ACTION_RESUME)
  {
    mmoc_info_ptr->emerg_susp_ss_mask = 0;
    mmoc_info_ptr->is_suspend       = FALSE;
    cmd_ptr->param.gen_prot_cmd.param.pref_sys_chgd.addl_action.action &= ~(MMOC_PREF_SYS_ADDL_ACTION_RESUME);
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if phone is online.
  */
  if ( mmoc_info_ptr->curr_oprt_mode != PROT_OPRT_MODE_ONLINE )
  {
    /* Update pref in CM to SD even when not in ONLINE. So that latest
    ** pref from user are used before going ONLINE */
       if(cmd_ptr->param.gen_prot_cmd.cmd_type == PROT_GEN_CMD_PREF_SYS_CHGD &&
       ((cmd_ptr->param.gen_prot_cmd.param.pref_sys_chgd.pref_reas ==
                                                      SD_SS_PREF_REAS_RESTORE) ||
        (cmd_ptr->param.gen_prot_cmd.param.pref_sys_chgd.pref_reas ==
                                                      SD_SS_PREF_REAS_USER)))
    {
      /* Inform SD protocols about new pref, even though protocols are not
      ** active. Set trans to invalid, as gen_cmd_cnf isn't needed**
      */
      mmoc_info_ptr->trans_info.gen_prot_cmd.cmd_info.trans_id = PROT_TRANS_NONE;
      (void)mmoc_proc_prot_gen_cmd(FALSE, FALSE,&cmd_ptr->param.gen_prot_cmd);

    }
    else
    {
      /* Phone not ONLINE, ignore the generic command.
      */
      MMOC_MSG_MED_0("incorrect phone state");
    }

    return mmoc_clear_transaction( mmoc_info_ptr );
  }



  if( cmd_ptr->param.gen_prot_cmd.cmd_type == PROT_GEN_CMD_WAKEUP_FROM_PWR_SAVE )
  {
    /* do the checks for wakeup from power save command
    */
    MMOC_MSG_HIGH_1("gen cmd is WAKEUP_FROM_PWR_SAVE: prot_state: %d",
                    mmoc_info_ptr->prot_state);
    if( (mmoc_info_ptr->prot_state != PROT_STATE_PWR_SAVE)
         && (mmoc_info_ptr->prot_state != PROT_STATE_NULL) )
    {

        /* non-multisim:
        ** valid protocol states are NULL and PWR_SAVE.
        ** other protocol states are invalid because there are active protocols,
        ** or phone is not in ONLINE mode.
        */

        return mmoc_clear_transaction( mmoc_info_ptr );
    }	
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the protocol state field and transaction info.
  */
  cmd_ptr->param.gen_prot_cmd.prot_state =
                 mmoc_info_ptr->prot_state;
  mmoc_info_ptr->trans_info.gen_prot_cmd.cmd_info =
                 cmd_ptr->param.gen_prot_cmd;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Move the trans. state to send the Generic command to the active
  ** protocol.
  */
  mmoc_update_trans_state( MMOC_TRANS_STATE_GEN_CMD_ENTER, mmoc_info_ptr );

  return MMOC_EVT_STATUS_NOT_CONSUMED;

} /* mmoc_process_prot_gen_cmd_null() */
/*lint +esym(715,rpt_ptr)*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_prot_gen_cmd_enter

DESCRIPTION
  Indicates whether PS_DETACH is needed or not

DEPENDENCIES
  FALSE

RETURN VALUE
 TRUE: PS_DETCAH needed.
 FALSE: PS_DETACH not needed.
 
SIDE EFFECTS
  None.
===========================================================================*/
static boolean mmoc_is_ps_detach_required (mmoc_state_info_s_type  *mmoc_info_ptr)
{
  #if defined(FEATURE_MMODE_LTE_ONLY) || defined(FEATURE_MMODE_GPRS_LTE)
  return TRUE;
  #else
  boolean ret = FALSE;

  if (mmoc_info_ptr == NULL)
  {
    return FALSE;
  }

  if (mmoc_info_ptr->is_gwl_subs_avail)
  {
    ret = TRUE;
  }
 

  MMOC_MSG_HIGH_1"is_ps_detach_required=%d";
  
  return ret;
  #endif
}

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_prot_gen_cmd_enter

DESCRIPTION
  This function is responsible for the sending the Generic command to the
  active protocol, if none is present calls into SD directly.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,rpt_ptr)*/
/*lint -esym(715,cmd_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_prot_gen_cmd_enter
(

        const mmoc_cmd_msg_s_type      *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type      *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  prot_gen_cmd_s_type                * gen_cmd_ptr   = NULL;
  prot_cmd_pref_sys_chgd_s_type      * pref_sys_chgd = NULL;
  sys_ps_detach_e_type                ps_detach_type = SYS_PS_DETACH_TYPE_MODE_CHANGE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MMOC_ASSERT( (cmd_ptr != NULL ) || (rpt_ptr != NULL) );
  gen_cmd_ptr = &(mmoc_info_ptr->trans_info.gen_prot_cmd.cmd_info);

  #ifdef FEATURE_LTE
  pref_sys_chgd = &(gen_cmd_ptr->param.pref_sys_chgd);

  if(gen_cmd_ptr->cmd_type      == PROT_GEN_CMD_PREF_SYS_CHGD           &&
     (pref_sys_chgd->addl_action.action & MMOC_PREF_SYS_ADDL_ACTION_PS_DETACH) &&
     mmoc_info_ptr->curr_oprt_mode == PROT_OPRT_MODE_ONLINE &&
     (mmoc_is_ps_detach_required(mmoc_info_ptr) == TRUE))
  {

    /* if detach is because of explicit PS detach command from CM client by removal
    ** of PS from domain preference the detach type would be Domain Change
    */
     if((pref_sys_chgd->domain_pref == SD_SS_SRV_DOMAIN_PREF_CS_ONLY) ||(pref_sys_chgd->domain_pref == SD_SS_SRV_DOMAIN_PREF_NONE))
    {
        ps_detach_type = SYS_PS_DETACH_TYPE_SRV_DOMAIN_CHANGE;
    }
    else
    {
        ps_detach_type = SYS_PS_DETACH_TYPE_MODE_CHANGE;
    }
    cmregprx_cmd_ps_detach(ps_detach_type,
                           mmoc_next_trans_id(mmoc_info_ptr)
                           );

    pref_sys_chgd->addl_action.action &= ~(MMOC_PREF_SYS_ADDL_ACTION_PS_DETACH);

    mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_PS_DETACH_CNF,
                             mmoc_info_ptr
                           );

    #if defined(FEATURE_CIOT)
    if((mmoc_info_ptr->curr_trans  == MMOC_TRANS_PROT_GEN_CMD) &&
       (mmoc_info_ptr->trans_state == MMOC_TRANS_STATE_WAIT_PS_DETACH_CNF))
    {
      mmoc_sanity_timer_set(mmoc_info_ptr, MMOC_PS_DETACH_SANITY_TIME);
    }
    #endif

    return MMOC_EVT_STATUS_CONSUMED;
  }
  #else
  SYS_ARG_NOT_USED(pref_sys_chgd);
  SYS_ARG_NOT_USED(ps_detach_type);
  #endif //FEATURE_LTE

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Send the generic command to the active protocol, if there is one.
  */
  if ( mmoc_send_prot_gen_cmd( gen_cmd_ptr,
                               mmoc_info_ptr
                             )
     )
  {
    /* Generic command sent, wait for acknowledgement.
    */
    mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_GEN_CMD_CNF,
                             mmoc_info_ptr
                           );
  }
  else
  {
    
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Process the generic command in MMoC itself.
    ** Set the trans_id to NONE, since no generic command ACK is required.
    */
    mmoc_info_ptr->trans_info.gen_prot_cmd.cmd_info.trans_id = PROT_TRANS_NONE;

    (void) mmoc_proc_prot_gen_cmd( TRUE, TRUE,gen_cmd_ptr);

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   {
      /* Activate the protocol or invoke
      ** transaction handler if this function returns EVT_STATUS_NOT_CONSUMED.
      */
      if ( mmoc_activate_prot_online(mmoc_info_ptr
                                    )
           == MMOC_EVT_STATUS_NOT_CONSUMED
         )
      {
        /* Invoke the new transaction handler.
        */
        return MMOC_EVT_STATUS_NOT_CONSUMED;

      }
   }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Done with processing, clear the transaction.
    */
    return mmoc_clear_transaction( mmoc_info_ptr );
  }

  return MMOC_EVT_STATUS_CONSUMED;

} /* mmoc_process_prot_gen_cmd_enter() */
/*lint +esym(715,rpt_ptr)*/
/*lint +esym(715,cmd_ptr)*/


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_prot_gen_cmd_activation_cnf

DESCRIPTION
 This function handles the processing  of the ACTIVATION_CNF rpt ( used in
 reslection from 3gpp2 to LTE) in the WAIT for ACT CNF trans state.

  In this state, the state handler will
  1. Validate the activation confirmation report received
  2. Clear the transaction

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_prot_gen_cmd_activation_cnf
(

        const mmoc_rpt_msg_s_type      *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  MMOC_ASSERT( rpt_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the rpt ptr is NULL
  */
  if ( rpt_ptr == NULL )
  {
    /* Not the expected report.
    */
    return MMOC_EVT_STATUS_CONSUMED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Process the received report.
  */
  switch (rpt_ptr->rpt.name )
  {
    case MMOC_RPT_PROT_AUTO_DEACTD_IND:
      return  mmoc_buf_auto_deact_ind_rpt(rpt_ptr,mmoc_info_ptr);;

    case MMOC_RPT_ACTIVATION_CNF:

      if(mmoc_info_ptr->prot_state !=
                                    rpt_ptr->param.activation_cnf.prot_state)
      {
        /* Not the expected report. Go back to waiting for cnf
        */
        MMOC_ERR_2(" Unexpected event received %d %d ",
                       mmoc_info_ptr->prot_state,
                       rpt_ptr->param.activation_cnf.prot_state);
        return MMOC_EVT_STATUS_CONSUMED;
      }
      /* If it is the correct trans id, clear the transaction
      */
      return mmoc_clear_transaction(mmoc_info_ptr);


    default:
        MMOC_ERR_1( "Unexpected report received %d",
                        rpt_ptr->rpt.name);
        return MMOC_EVT_STATUS_CONSUMED;


  } /* switch ( rpt_ptr->rpt.name ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
} //mmoc_process_prot_gen_cmd_activation_cnf


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_prot_gen_activation_enter

DESCRIPTION
 This function moves MMOC into the wait for activation CNF state


DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_prot_gen_activation_enter
(

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if phone is online.
  */
  if ( mmoc_info_ptr->curr_oprt_mode != PROT_OPRT_MODE_ONLINE )
  {
    /* Phone not ONLINE, ignore the generic command.
    */
    MMOC_ERR_0("incorrect phone state");
    return mmoc_clear_transaction( mmoc_info_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Move the trans. state to wait for activation CNF
  */
  mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_ACTIVATION_CNF, mmoc_info_ptr );

  return MMOC_EVT_STATUS_CONSUMED;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
} //mmoc_process_prot_gen_activation_enter



/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_prot_gen_cmd_cnf

DESCRIPTION
  This function handles the generic protocol command transaction in the
  WAIT_GEN_CMD_CNF transaction state.

  In this state, the state handler will
  1. Validate the Ack received. Done with the transaction if ack is correct
     and ack says that the generic command was processed. If the ack says
     that generic command was not processed, it is race condition wherein
     the MMoC recvd the deactivated indication while waiting for the ack.

  Note! Race condition, if a protocol deactivated indication is received
  while waiting for ACK, the handler will activate the AMPS protocol only
  if it is CDMA to AMPS handoff case. Else the deactivated indication
  report will be dropped.

DEPENDENCIES
  Non

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,cmd_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_prot_gen_cmd_cnf
(

        const mmoc_cmd_msg_s_type      *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type      *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  MMOC_ASSERT( rpt_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if a report is received.
  */
  if ( rpt_ptr == NULL )
  {
    /* Not expecting a command.
    */

    return MMOC_EVT_STATUS_CONSUMED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the received report.
  */
  switch (rpt_ptr->rpt.name )
  {
    case MMOC_RPT_PROT_AUTO_DEACTD_IND:
      /* Autonomous deactivated report received.
      */
      MMOC_MSG_HIGH_1("Received autonomous deactivated report  reason: %d",rpt_ptr->param.prot_deactd_ind.reason);
      {
        /*  Activate main at the end of transaction.
        */
         if(sd_ss_act_get( NULL ) == SD_SS_ACT_PWR_SAVE)
         {
           return mmoc_activate_prot_online_ss_main( sd_ss_act_get( NULL ),
                                                mmoc_info_ptr
                                              );
         }
         else
         {
            mmoc_info_ptr->trans_info.gen_prot_cmd.is_activate_main = TRUE;
            mmoc_update_prot_state( PROT_STATE_NULL,
                                 mmoc_info_ptr
                                );
            return MMOC_EVT_STATUS_CONSUMED;
          }
       }

    case MMOC_RPT_PROT_GEN_CMD_CNF:

      /* Check if it is the right acknowledgement.
      */
      if ( mmoc_info_ptr->trans_id != rpt_ptr->param.gen_cmd_cnf.trans_id )
      {
        /* Not the expected transaction Id.
        */
        MMOC_MSG_LOW_0("Received incorrect trans_id for Gen cmd ack");
        return MMOC_EVT_STATUS_CONSUMED;
      }
      break;

    default:
      MMOC_MSG_HIGH_1(" Received unexpected report %d", rpt_ptr->rpt.name);

      return MMOC_EVT_STATUS_CONSUMED;
  } /* switch (rpt_ptr->rpt.name ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the generic command was processed by the protocols.
  */
  if ( !rpt_ptr->param.gen_cmd_cnf.is_cmd_processed )
  {
    /* Race condition: Protocol was deactivated just before the generic
    ** command was posted, either send generic command or process it in
    ** MMoC itself.
    */
    mmoc_update_trans_state( MMOC_TRANS_STATE_GEN_CMD_ENTER,
                             mmoc_info_ptr
                           );
    return MMOC_EVT_STATUS_NOT_CONSUMED;

  } /* if ( !rpt_ptr->param.gen_cmd_cnf.is_cmd_processed ) */



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the protocol is deactivating.
  */

  if ( rpt_ptr->param.gen_cmd_cnf.is_prot_deactivating )
  {
     MMOC_MSG_HIGH_1("is_prot_deactivating=%d",
                     rpt_ptr->param.gen_cmd_cnf.is_prot_deactivating);

     /* Restart the sanity timer when MMOC is waiting for deactivation indication */
     mmoc_sanity_timer_reset(mmoc_info_ptr);
     mmoc_sanity_timer_set(mmoc_info_ptr, MMOC_PWRDOWN_SANITY_TIME);
     
     /* Wait for deactivated indication from protocol.
     */
     mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_AUTO_DEACTD_IND,
                              mmoc_info_ptr
                            );
     return MMOC_EVT_STATUS_CONSUMED;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Generic command was processed, end the transaction.
  */
  return mmoc_clear_transaction(mmoc_info_ptr);


} /* mmoc_process_prot_gen_cmd_cnf() */
/*lint +esym(715,cmd_ptr)*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_prot_gen_cmd_auto_deactd_ind

DESCRIPTION
  This function handles the generic protocol command transaction in the
  WAIT_AUTO_DEATD_IND transaction state.

  In this state, the state handler will


DEPENDENCIES
  Non

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,cmd_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_prot_gen_cmd_auto_deactd_ind
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  MMOC_ASSERT( rpt_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if we got the autonomous deactivated report.
  */
  if (rpt_ptr == NULL)
  {
    /* Unexpected report received.
    */

    return MMOC_EVT_STATUS_CONSUMED;

  }

  if (rpt_ptr->rpt.name  != MMOC_RPT_PROT_AUTO_DEACTD_IND )
  {
    /* Unexpected report received.
    */
    MMOC_ERR_1("unexpected report %d", rpt_ptr->rpt.name);
    return MMOC_EVT_STATUS_CONSUMED;

  }

  MMOC_MSG_HIGH_1("mmoc_process_prot_gen_cmd_auto_deactd_ind, reason=%d",
                 rpt_ptr->param.prot_deactd_ind.reason);


    /* Check if SS_MAIN got deactivated.
    */
    if ( rpt_ptr->param.prot_deactd_ind.prot_state !=
                                    mmoc_info_ptr->prot_state )
    {
      /* Bogus report.
      */
      MMOC_MSG_MED_0("Invalid report");
      return MMOC_EVT_STATUS_CONSUMED;
    }

    /* SS_MAIN protocol got deactivated.
    */
    mmoc_info_ptr->trans_info.gen_prot_cmd.is_activate_main = TRUE;
    mmoc_update_prot_state( PROT_STATE_NULL,
                            mmoc_info_ptr
                          );
  /* End transaction.
  */
  return mmoc_clear_transaction(mmoc_info_ptr);

} /* mmoc_process_prot_gen_cmd_auto_deactd_ind() */
/*lint +esym(715,cmd_ptr)*/


/*===========================================================================

FUNCTION mmoc_postprocess_prot_gen_cmd

DESCRIPTION
  This function handles the generic protocol command transaction cleanup
  function.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_postprocess_prot_gen_cmd
(

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if MAIN has to be activated.
  */
  if ( mmoc_info_ptr->trans_info.gen_prot_cmd.is_activate_main )
  {

    /* Since main is being kicked online turn the flag OFF
    */
    mmoc_info_ptr->trans_info.gen_prot_cmd.is_activate_main = FALSE;

    /* MAIN needs to be activated.
    */
    if(mmoc_info_ptr->prot_state == PROT_STATE_NULL)
    {
      return mmoc_activate_prot_online_ss_main( sd_ss_act_get( NULL ),
                                           mmoc_info_ptr
                                              );
    }
  } //is_activate_main

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return event consumed.
  */
  return MMOC_EVT_STATUS_CONSUMED;

} /* mmoc_postprocess_prot_gen_cmd() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_prot_gen_cmd

DESCRIPTION
  This function is the transaction handler for the power down transaction.

  The transaction state transition will be,

     NULL <-> GEN_CMD_ENTER -> WAIT_GEN_CMD_CNF  -> NULL
                 ^                 |
                 |                 |
                 ------------------

DEPENDENCIES
  None.

DEPENDENCIES

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_prot_gen_cmd
(

        mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type      *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  mmoc_evt_status_e_type  evt_status = MMOC_EVT_STATUS_CONSUMED;
  prot_gen_cmd_e_type     cmd_type   = PROT_GEN_CMD_MAX;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Invoke the appropriate the transaction state handler.
  */
  switch(mmoc_info_ptr->trans_state)
  {
    case MMOC_TRANS_STATE_NULL:
      evt_status = mmoc_process_prot_gen_cmd_null( cmd_ptr,
                                                   rpt_ptr,
                                                   mmoc_info_ptr
                                                 );
      break;

    case MMOC_TRANS_STATE_GEN_CMD_ENTER:
      evt_status = mmoc_process_prot_gen_cmd_enter( cmd_ptr,
                                                    rpt_ptr,
                                                    mmoc_info_ptr
                                                  );
      break;

    case MMOC_TRANS_STATE_WAIT_GEN_CMD_CNF:
      evt_status = mmoc_process_prot_gen_cmd_cnf( cmd_ptr,
                                                  rpt_ptr,
                                                  mmoc_info_ptr
                                                );
      break;


    case MMOC_TRANS_STATE_WAIT_AUTO_DEACTD_IND:
      evt_status = mmoc_process_prot_gen_cmd_auto_deactd_ind( cmd_ptr,
                                                              rpt_ptr,
                                                              mmoc_info_ptr
                                                            );
      break;


    case MMOC_TRANS_STATE_GEN_CMD_ACTIVATION_ENTER:
      evt_status = mmoc_process_prot_gen_activation_enter (
                                                              mmoc_info_ptr
                                                            );
      break;



    case MMOC_TRANS_STATE_WAIT_ACTIVATION_CNF:
      evt_status = mmoc_process_prot_gen_cmd_activation_cnf ( rpt_ptr,
                                                              mmoc_info_ptr
                                                            );
      break;

    case MMOC_TRANS_STATE_WAIT_PS_DETACH_CNF:
      evt_status = mmoc_process_common_ps_detach_cnf( rpt_ptr,
                                                      mmoc_info_ptr );
      break;

   
    default:
      MMOC_ERR_0("Received event in unknown state");
      evt_status = mmoc_clear_transaction( mmoc_info_ptr );
      break;
  }


  return evt_status;

} /* mmoc_process_prot_gen_cmd() */


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== AUTONOMOUS PROTOCOL DEACTIVATED TRANSACTION ============
=============================================================================
=============================================================================
===========================================================================*/


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_auto_deactd_ind_null

DESCRIPTION
  This function handles the autonomous protocol deactivated report
  transaction in the NULL transaction state.

  In this state, the state handler will
  1. based on the deactivated reason either call into SD to determine the
     next protocol to be activated or if the deactivated reason is CDMA to
     AMPS handoff, it will activate the AMPS protocol.


DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,cmd_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_auto_deactd_ind
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  if(rpt_ptr == NULL)
  {
        return MMOC_EVT_STATUS_CONSUMED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MMOC_MSG_HIGH_2("mmoc_process_auto_deactd_ind, prot_deactd_ind \
                 reason=%d,prot_state=%d",
                                   rpt_ptr->param.prot_deactd_ind.reason,
                                   rpt_ptr->param.prot_deactd_ind.prot_state);

  /* Check if phone is online.
  */
  if ( mmoc_info_ptr->curr_oprt_mode != PROT_OPRT_MODE_ONLINE )
  {
    MMOC_ERR_FATAL_0("Phone should be in online mode");
  }

  /* Initialize the trans info.
  */
  switch (rpt_ptr->param.prot_deactd_ind.reason)
  {
    case PROT_DEACT_SUBSC_CHGD:
    case PROT_DEACT_ACQ_OTHERS:

      /* Activate protocol on ss instance
      */
      if ( mmoc_activate_prot_online( mmoc_info_ptr ) ==
                                           MMOC_EVT_STATUS_NOT_CONSUMED )
      {
        /* Invoke new transaction.
        */
        return MMOC_EVT_STATUS_NOT_CONSUMED;
      }

      break;

    default:
      MMOC_ERR_1( "deact reason %d",
                      rpt_ptr->param.prot_deactd_ind.reason);
      break;
  }  /* switch (rpt_ptr->param.prot_auto_deactd_ind.reason) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Transaction completed.
  */
  return mmoc_clear_transaction( mmoc_info_ptr );

} /* mmoc_process_auto_deactd_ind_null() */


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== POWER SAVE ENTER TRANSACTION ===========================
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_pwr_save_enter

DESCRIPTION
  This function handles the power save transaction in the NULL transaction
  state.

  In this state, the state handler will
  1. Check if GPIO allows phone to go in power save mode, if so informs
     SD of power save enter and calls into SRCH or Sleep Controller.
     to send the phone to power save mode.
  2. If power save mode is not allowed, follows SD actions and activates the
     next protocol.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,cmd_ptr)*/
/*lint -esym(715,rpt_ptr)*/
static  mmoc_evt_status_e_type         mmoc_process_pwr_save_enter
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  #ifndef FEATURE_MMOC_UNIT_TEST
  #ifndef FEATURE_DISABLE_DEEP_SLEEP
  /* Change in bio(Basic IP/OP) interface.
  ** Now MMOC will use DAL interface instead of bio.
  */
  uint32           sw_switch9_state    = 0;

  if (switches_handle == NULL)
  {
  switches_dal_attach =
          DAL_DeviceAttach(DALDEVICEID_SOFTWARESWITCHES,&switches_handle);
  }

  if((switches_dal_attach == DAL_SUCCESS) && (switches_handle != NULL))
  {
    (void)DalSoftwareSwitches_GetMask(
           switches_handle,(uint32)DAL_SOFTWARE_SWITCH_9,&sw_switch9_state);
  }
  #endif /* FEATURE_DISABLE_DEEP_SLEEP */
  #endif /* FEATURE_MMOC_UNIT_TEST */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Phone should be in ONLINE mode and SD action should be power save.
  */
  if ( (mmoc_info_ptr->curr_oprt_mode != PROT_OPRT_MODE_ONLINE) &&
       (sd_ss_act_get( NULL ) !=  SD_SS_ACT_PWR_SAVE )
     )
  {
    MMOC_ERR_FATAL_0("Phone should be in online mode");
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_MMOC_UNIT_TEST

  MMOC_MSG_HIGH_1("sw_switch9_state = 0x%x",sw_switch9_state);
  /* Check on GPIO to see if power save mode is allowed.
  */
  if (
      #ifndef FEATURE_DISABLE_DEEP_SLEEP
      sw_switch9_state  &&
      #endif
       sd_ss_ind_user_pwr_save_exit( NULL ) != SD_SS_ACT_PWR_SAVE
     )

  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Power save mode is not allowed and SD can activate a new protocol.
    ** Aborted power save enter, activate the protocol or invoke
    ** transaction handler if this function returns EVT_STATUS_NOT_CONSUMED.
    */
    if ( mmoc_activate_prot_online_ss_main( sd_ss_act_get(NULL),
                                            mmoc_info_ptr
                                          )
         == MMOC_EVT_STATUS_NOT_CONSUMED
       )
    {
      /* Invoke the new transaction handler.
      */
      return MMOC_EVT_STATUS_NOT_CONSUMED;

    }


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Clear the transaction and return
    */
    return mmoc_clear_transaction( mmoc_info_ptr );

  } /* if (sw_switch9_state) */

  #endif /* #if FEATURE_MMOC_UNIT_TEST */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Update the MMoC's protocol state and clear the transaction.
  */
  mmoc_update_prot_state( PROT_STATE_PWR_SAVE,
                          mmoc_info_ptr
                        );

  return mmoc_clear_transaction( mmoc_info_ptr );


} /* mmoc_process_pwr_save_enter_null() */



/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== SUSPEND SS TRANSACTION =================================
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_suspend_ss_null

DESCRIPTION
  This function handles the suspend_ss transaction in the NULL
  transaction state.

  In this state, the state handler will


DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_suspend_ss_null
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */


        mmoc_state_info_s_type              *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{

  if( cmd_ptr == NULL )
  {
    return mmoc_clear_transaction( mmoc_info_ptr );
  }


  /* Only the Suspend SS command from CM should create this transaction
  */
  if ( cmd_ptr->cmd.name != MMOC_CMD_SUSPEND_SS )
  {
    /* Transaction created incorrectly.
    */
    MMOC_ERR_0("suspend ss trans incorrectly initiated");
    return mmoc_clear_transaction( mmoc_info_ptr );
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If CM has already initiated SUSPEND_SS transacation initialize
  ** MMOC state information and return.
  */
  if((mmoc_info_ptr->is_suspend == \
     cmd_ptr->param.suspend_ss.is_suspend ) ||
     (cmd_ptr->param.suspend_ss.ignore_protocol_activate == TRUE ) ||
      (mmoc_info_ptr->curr_oprt_mode!=PROT_OPRT_MODE_ONLINE)
      )
  {

   /* ignore_protocol_activate will inform if CM has already sent indication to
    * activate Protocol.
    */
    MMOC_MSG_HIGH_3("already in suspend status %d \
                   ignore_protocol_activate %d curr_oprt_mode %d",                   
                   cmd_ptr->param.suspend_ss.is_suspend,
                   cmd_ptr->param.suspend_ss.ignore_protocol_activate,
                   mmoc_info_ptr->curr_oprt_mode);

    /* Inform CM of suspend immediately
    */
    {
      cm_suspend_ss_s_type    suspend_ss;
      suspend_ss.is_suspend = cmd_ptr->param.suspend_ss.is_suspend;
      mmoc_info_ptr->is_suspend = cmd_ptr->param.suspend_ss.is_suspend;
      mmoc_send_cm_rpt( CM_SUSPEND_SS_F, (cm_sd_rpt_u_type *)&suspend_ss );
    }
    return mmoc_clear_transaction( mmoc_info_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* copy transaction info
  */
  mmoc_info_ptr->trans_info.suspend_ss.is_suspend =
                                        cmd_ptr->param.suspend_ss.is_suspend;
  mmoc_info_ptr->trans_info.suspend_ss.susp_reason =
                                        cmd_ptr->param.suspend_ss.susp_reason;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if (cmd_ptr->param.suspend_ss.is_suspend)
  {
     mmoc_update_trans_state( MMOC_TRANS_STATE_SUSPEND_SS_ENTER,
                              mmoc_info_ptr
                            );

  }
  else
  {
     mmoc_update_trans_state( MMOC_TRANS_STATE_RESUME_SS_ENTER,
                              mmoc_info_ptr
                            );
  }

  return MMOC_EVT_STATUS_NOT_CONSUMED;

} /* mmoc_process_suspend_ss_null() */

/*===========================================================================
FUNCTION mmoc_process_suspend_ss_enter

DESCRIPTION
  This function handles the suspend_ss transaction in the SUSPEND_SS_ENTER
  transaction state.

  In this state, the state handler will deactivate the protocol.
  After getting confirmation, a report will be sent to CM confirming that the
  protocol has been deactivated.


DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_suspend_ss_enter
(

        mmoc_state_info_s_type              *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  prot_deact_e_type               deact_reason = PROT_DEACT_ACQ_OTHERS;
  boolean                         is_deact_req_sent = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* update the deact reason if prot is GWL
  */
  if(mmoc_info_ptr->prot_state ==
                                                      PROT_STATE_ONLINE_GWL)
  {
    deact_reason = mmoc_info_ptr->trans_info.suspend_ss.susp_reason;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MMOC_MSG_HIGH_3("Suspend ss enter susp_reason %d deact_reason %d, Suspended SS mask %d",
                  mmoc_info_ptr->trans_info.suspend_ss.susp_reason,deact_reason, mmoc_info_ptr->emerg_susp_ss_mask);


  /* Send deact request if protocol is active
  */
  is_deact_req_sent = mmoc_deactivate_active_protocol
                      (
                        deact_reason,
                        mmoc_info_ptr
                      );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Wait for confirmation if deact request has been sent
  */
  if (is_deact_req_sent)
  {
     mmoc_update_trans_state( MMOC_TRANS_STATE_WAIT_DEACTD_CNF,
                              mmoc_info_ptr
                            );

     /* Since SS has been deactivated , send indication to SD that service
     ** is lost. SD will then inform CM of srv loss
     */
     sd_ss_ind_misc_srv_lost();

     return MMOC_EVT_STATUS_CONSUMED;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Inform CM of suspend
  */
  {
    cm_suspend_ss_s_type    suspend_ss;
    suspend_ss.is_suspend = TRUE;
    mmoc_send_cm_rpt( CM_SUSPEND_SS_F, (cm_sd_rpt_u_type *)&suspend_ss );
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update state in MMOC
  */
  mmoc_info_ptr->is_suspend = TRUE;
  if( mmoc_info_ptr->trans_info.suspend_ss.susp_reason == PROT_DEACT_DSDX_SUSP)
  {
    mmoc_info_ptr->emerg_susp_ss_mask = 1;

  }
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear the transaction.
  */
  return mmoc_clear_transaction( mmoc_info_ptr );

} /* mmoc_process_suspend_ss_enter() */


/*===========================================================================
FUNCTION mmoc_process_resume_ss_enter

DESCRIPTION
  This function handles the suspend_ss transaction in the RESUME_SS_ENTER
  transaction state.



DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_resume_ss_enter
(

        mmoc_state_info_s_type              *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  mmoc_evt_status_e_type        evt_status = MMOC_EVT_STATUS_CONSUMED;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update state in MMOC
  */
  mmoc_info_ptr->is_suspend = FALSE;


  MMOC_MSG_HIGH_2("Resume ss enter suspended reason %d ss %d, emerg_susp_ss_mask %d",
                  mmoc_info_ptr->trans_info.suspend_ss.susp_reason,                   
                   mmoc_info_ptr->emerg_susp_ss_mask);


  if( mmoc_info_ptr->trans_info.suspend_ss.susp_reason == PROT_DEACT_DSDX_SUSP)
  {
    mmoc_info_ptr->emerg_susp_ss_mask = 0;

    /* SD core is not in sync with CM. Power up SD so that it is aligned*/
    if (mmoc_info_ptr->is_gwl_subs_avail)
    {
      (void)sd_ss_ind_user_pwr_up(FALSE,NULL);
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Activate the protocol.
  */
  evt_status = mmoc_activate_prot_online(mmoc_info_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Always inform CM of resume even without waiting for protocol to completely
  ** being brought ONLINE, because at this point, Turbo Decoder has been released
  ** and no conflict would happened.
  */
  {
    cm_suspend_ss_s_type    suspend_ss;
    suspend_ss.is_suspend = FALSE;

    mmoc_send_cm_rpt( CM_SUSPEND_SS_F, (cm_sd_rpt_u_type *)&suspend_ss );
  }

  if ( evt_status == MMOC_EVT_STATUS_NOT_CONSUMED )
  {
    /* Invoke new transaction.
    */
    return MMOC_EVT_STATUS_NOT_CONSUMED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear the transaction.
  */
  return mmoc_clear_transaction( mmoc_info_ptr );

} /* mmoc_process_resume_ss_enter() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_suspend_ss

DESCRIPTION
  This function is the transaction handler for the wake up from pwr save
  transaction.

  The transaction state transition will be,

     NULL -> NULL

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_process_suspend_ss
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type               *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  mmoc_evt_status_e_type evt_status = MMOC_EVT_STATUS_CONSUMED;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Invoke the current transaction state handler.
  */
  switch(mmoc_info_ptr->trans_state)
  {
    case MMOC_TRANS_STATE_NULL:
      evt_status = mmoc_process_suspend_ss_null( cmd_ptr,
                                                 mmoc_info_ptr
                                               );
      break;

    case MMOC_TRANS_STATE_SUSPEND_SS_ENTER:
    case MMOC_TRANS_STATE_PROT_DEACT_ENTER:
       evt_status = mmoc_process_suspend_ss_enter (mmoc_info_ptr);
       break;


     case MMOC_TRANS_STATE_RESUME_SS_ENTER:
       evt_status = mmoc_process_resume_ss_enter (mmoc_info_ptr);
       break;

    case MMOC_TRANS_STATE_WAIT_DEACTD_CNF:
      evt_status = mmoc_process_common_deactd_cnf( rpt_ptr,
                                                   mmoc_info_ptr
                                                );
      break;

    default:
      MMOC_ERR_0("Received event in unknown state");
      evt_status = mmoc_clear_transaction( mmoc_info_ptr );
      break;
  }


  return evt_status;

} /* mmoc_process_suspend_ss() */
/* <EJECT> */

#ifdef MMODE_ADDITIONAL_DEBUG_INFO
/*===========================================================================

FUNCTION mmoc_dbg_buf_add_mesage

DESCRIPTION
  This function adds the transaction being processed to the debug buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                            mmoc_dbg_buf_add_message
(
       mmoc_state_info_s_type         *mmoc_info_ptr,
   
       mmoc_dbg_buf_rpt_s_type          *rpt_queued_ptr,

       uint8			               rpt_idx
)
{
  /* Pointer to current transaction
  */
  mmoc_dbg_buf_trans_s_type *current_trans_ptr = NULL;

  int8 curr_idx = mmoc_debug_buffer.dbg_buf_idx;
  
  /* For commands this ptr shold be null and hence add a new transaction here
  */
  if(rpt_queued_ptr == NULL)
  {
    /* Increment debug_buffer_index and set it to 0 in case it becomes same as
    ** MMOC_MAX_DEBUG_BUFFER_SIZE as transactions are added in cyclic manner
    */
    curr_idx++;

    if(curr_idx >= MMOC_MAX_DEBUG_BUFFER_SIZE)
    {
      curr_idx = 0;      
    }
    /* Assign pointer to current trans to current_trans_ptr
    */
    current_trans_ptr = ((mmoc_debug_buffer.dbg_buf)+curr_idx);
    /* Add transaction here
    */
    mmoc_dbg_buf_init_rpt_queue(curr_idx);
    current_trans_ptr->trans_id = mmoc_info_ptr->trans_id;
    current_trans_ptr->trans_name = mmoc_info_ptr->curr_trans;

    /* Add additional data for particular transaction types */
    switch(mmoc_info_ptr->curr_trans)
    {
      case MMOC_TRANS_SUBSC_CHGD:
        current_trans_ptr->addl_info[0] = mmoc_info_ptr->prot_subsc_chg;
        break;

      case MMOC_TRANS_PROT_GEN_CMD:
        current_trans_ptr->addl_info[0] = mmoc_info_ptr->trans_info.gen_prot_cmd.cmd_info.cmd_type;
        if(current_trans_ptr->addl_info[0] == PROT_GEN_CMD_PREF_SYS_CHGD)
        {
          current_trans_ptr->addl_info[2] = mmoc_info_ptr->trans_info.gen_prot_cmd.cmd_info.param.pref_sys_chgd.pref_reas;
          current_trans_ptr->addl_info[3] = mmoc_info_ptr->trans_info.gen_prot_cmd.cmd_info.param.pref_sys_chgd.addl_action.action;
          current_trans_ptr->addl_info[4] = mmoc_info_ptr->trans_info.gen_prot_cmd.cmd_info.param.pref_sys_chgd.addl_action.ue_mode;
          current_trans_ptr->addl_info[5] = mmoc_info_ptr->trans_info.gen_prot_cmd.cmd_info.param.pref_sys_chgd.addl_action.is_ue_mode_substate_srlte;
        }
        break;

      case MMOC_TRANS_SUSPEND_SS:
        current_trans_ptr->addl_info[0] = mmoc_info_ptr->trans_info.suspend_ss.is_suspend;
        current_trans_ptr->addl_info[1] = mmoc_info_ptr->trans_info.suspend_ss.ignore_protocol_activate;
        current_trans_ptr->addl_info[2] = mmoc_info_ptr->trans_info.suspend_ss.susp_reason;
        break;

      default:
        break;
    }


	mmoc_debug_buffer.dbg_buf_idx = curr_idx;
    return;
  }	
  /* For reports this ptr shold NOT be null and hence add reports 
  ** for current transaction here
  */	
  if((rpt_queued_ptr != NULL && rpt_idx < MMOC_MAX_RPT_COUNT) && 
     (curr_idx >= 0) && 
     (curr_idx < MMOC_MAX_DEBUG_BUFFER_SIZE)
    )
  {

    /* Assign pointer to current trans to current_trans_ptr
    */
    current_trans_ptr = ((mmoc_debug_buffer.dbg_buf)+curr_idx);
    current_trans_ptr->rpt_queue[rpt_idx].prot_state = rpt_queued_ptr->prot_state;       

    current_trans_ptr->rpt_queue[rpt_idx].rpt_name = rpt_queued_ptr->rpt_name;	
    current_trans_ptr->rpt_queue[rpt_idx].task_name = rpt_queued_ptr->task_name;
   
  }
  
} /* mmoc_dbg_buf_add_mesage() */
#endif /* MMODE_ADDITIONAL_DEBUG_INFO */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_setup_trans

DESCRIPTION
  The purpose of this function is to determine which transaction should be
  invoked in order to process the event. This will be based on the type of
  event received.

DEPENDENCIES
  None.

RETURN VALUE
  MMOC_EVT_STATUS_CONSUMED     - Event was processed to completion.
  MMOC_EVT_STATUS_NOT_CONSUMED - Event was not processed. Caller should
                                  invoke the appropriate transaction handler
                                  again.

SIDE EFFECTS
  None.
===========================================================================*/
static  mmoc_evt_status_e_type         mmoc_setup_trans
(

        const mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{
  mmoc_evt_status_e_type  evt_status = MMOC_EVT_STATUS_NOT_CONSUMED;
  #ifdef MMODE_ADDITIONAL_DEBUG_INFO
    mmoc_dbg_buf_rpt_s_type	 rpt_to_queue;
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set cleanup function to NULL.
  */
  mmoc_info_ptr->exit_fn_ptr         = NULL;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the event is a command or report.
  */
  if ( cmd_ptr != NULL )
  {
    /* Event is a command.
    */
    switch (cmd_ptr->cmd.name)
    {
      case MMOC_CMD_OPRT_MODE_CHGD:
        {
          if(mmoc_info_ptr->prot_state == PROT_STATE_PWR_DOWN)
          {
            MMOC_ERR_0("Unexpected OPRT_MODE_CHGD received in PROT_STATE_PWR_DOWN So skip processing");
            evt_status = MMOC_EVT_STATUS_CONSUMED;
          }
          else
          {
            switch ( cmd_ptr->param.oprt_mode_chgd.oprt_mode)
            { 
              case SYS_OPRT_MODE_OFFLINE:
              case SYS_OPRT_MODE_FTM:
              case SYS_OPRT_MODE_PSM:
                mmoc_update_curr_trans( MMOC_TRANS_OFFLINE,
                                        mmoc_info_ptr,
                                        cmd_ptr->param.oprt_mode_chgd.oprt_mode
                                      );
                break;

              case SYS_OPRT_MODE_RESET:
              case SYS_OPRT_MODE_RESET_MODEM:
              case SYS_OPRT_MODE_LPM:
              case SYS_OPRT_MODE_PWROFF:
                mmoc_update_curr_trans( MMOC_TRANS_PWR_DOWN,
                                        mmoc_info_ptr,
                                        cmd_ptr->param.oprt_mode_chgd.oprt_mode
                                       );
                break;

              case SYS_OPRT_MODE_ONLINE:
                mmoc_update_curr_trans( MMOC_TRANS_ONLINE,
                                        mmoc_info_ptr,
                                        cmd_ptr->param.oprt_mode_chgd.oprt_mode
                                      );
                break;

              default:
                MMOC_ERR_1("Incorrect oprt_mode received %d",
                               cmd_ptr->param.oprt_mode_chgd.oprt_mode);
                evt_status = MMOC_EVT_STATUS_CONSUMED;
                break;

            } /* switch( cmd_ptr->param.oprt_mode_chgd.oprt_mode) */
          }/* else of (mmoc_info_ptr->prot_state == PROT_STATE_PWR_DOWN) */
        }
        break;

      case MMOC_CMD_SUBSCRIPTION_CHGD:
        mmoc_update_curr_trans( MMOC_TRANS_SUBSC_CHGD,
                                mmoc_info_ptr,
                                SYS_OPRT_MODE_NONE
                              );
        break;

      case MMOC_CMD_PROT_GEN_CMD:
        mmoc_update_curr_trans( MMOC_TRANS_PROT_GEN_CMD,
                                mmoc_info_ptr,
                                SYS_OPRT_MODE_NONE
                              );
        mmoc_info_ptr->exit_fn_ptr = (mmoc_exit_fn_f_type *)mmoc_postprocess_prot_gen_cmd;
        mmoc_info_ptr->trans_info.gen_prot_cmd.is_activate_main = FALSE;
        break;        

      case MMOC_CMD_SUSPEND_SS:
        mmoc_update_curr_trans( MMOC_TRANS_SUSPEND_SS,
                                mmoc_info_ptr,
                                SYS_OPRT_MODE_NONE
                              );
        break;

      default:
        MMOC_ERR_1( "Received unknown command event %d",
                        cmd_ptr->cmd.name);
        evt_status = MMOC_EVT_STATUS_CONSUMED;
        break;

    } /* switch ( cmd_ptr->cmd.name ) */

  } /* if ( cmd_ptr ) */

  else if ( rpt_ptr != NULL )
  {
    /* Event is a report.
    */
    switch ( rpt_ptr->rpt.name )
    {
      case MMOC_RPT_PROT_AUTO_DEACTD_IND:
        mmoc_update_curr_trans( MMOC_TRANS_PROT_AUTO_DEACTD_IND,
                                mmoc_info_ptr,
                                mmoc_info_ptr->true_oprt_mode
                              );
        break;

      case MMOC_RPT_PROT_AUTO_ACTD_IND:
      case MMOC_RPT_PROT_DEACTD_CNF:
      case MMOC_RPT_PH_STAT_CHGD_CNF:
      case MMOC_RPT_PROT_GEN_CMD_CNF:
      default:
        MMOC_MSG_MED_1( "Unexpected report received %d",
                        rpt_ptr->rpt.name);
        evt_status = MMOC_EVT_STATUS_CONSUMED;
        break;

    } /* switch ( rpt_ptr->rpt.name ) */

  } /* else if ( rpt_ptr ) */
  else
  {
    sys_err_fatal_null_ptr_exception();
  } /* else */

  #ifdef MMODE_ADDITIONAL_DEBUG_INFO
  curr_rpt_idx = 0;

  if (rpt_ptr == NULL)
  {
    rpt_to_queue.rpt_name = 0;
    rpt_to_queue.task_name = 0;
  }
  else
  {
    rpt_to_queue.rpt_name = rpt_ptr->rpt.name;
    rpt_to_queue.task_name = rpt_ptr->rpt.task_name;
  }
  

  rpt_to_queue.prot_state = mmoc_info_ptr->prot_state;

  mmoc_dbg_buf_add_message(mmoc_info_ptr, 
                                   NULL, 
                                   0);
  
  mmoc_dbg_buf_add_message(mmoc_info_ptr, &(rpt_to_queue), 0);
  #endif

  return evt_status;

} /* mmoc_setup_trans() */




/*===========================================================================

FUNCTION mmoc_call_trans_hndlr

DESCRIPTION
  This function will process the events( either command or report) until the
  event was processed to completion or consumed. This is done by calling the
  appropriate transaction handlers based on the current transaction.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           mmoc_call_trans_hndlr
(

        mmoc_cmd_msg_s_type            *cmd_ptr,
            /* Pointer to the command event that was received.
            ** if this was not called because of command event, then
            ** cmd_ptr = null.
            */

        const mmoc_rpt_msg_s_type            *rpt_ptr,
            /* Pointer to the report event that was received.
            ** if this was called because of report event, then
            ** rpt_ptr = null.
            */

        mmoc_state_info_s_type         *mmoc_info_ptr
            /* Pointer to MMoC's state information, may/may not be updated
            ** by the transaction state handlers.
            */

)
{

  mmoc_evt_status_e_type   evt_status = MMOC_EVT_STATUS_NOT_CONSUMED;
  mmoc_trans_e_type        last_trans;
  mmoc_trans_state_e_type  last_trans_state;

  if( ((cmd_ptr == NULL) && (rpt_ptr == NULL)) ||
      ((cmd_ptr != NULL) && (rpt_ptr != NULL)) ||
      (mmoc_info_ptr == NULL))
  {
    MMOC_ERR_3( "cmd_ptr=%d, rpt_ptr=%d, mmoc_info_ptr=%d",
                    cmd_ptr, rpt_ptr, mmoc_info_ptr);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(mmoc_info_ptr->curr_trans >= MMOC_TRANS_MAX)
  {
    MMOC_ERR_1( "invalid curr_trans: %d", mmoc_info_ptr->curr_trans);
    return;
  }

  if(mmoc_info_ptr->trans_state >= MMOC_TRANS_STATE_MAX)
  {
    MMOC_ERR_1( "invalid trans_state: %d", mmoc_info_ptr->trans_state);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_MMOC_LOW_MEM_TARGET
  /* Print debug info before calling transaction handler.
  */
  mmocdbg_print_before_trans( mmoc_info_ptr );
  #endif /* FEATURE_MMOC_LOW_MEM_TARGET */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if transaction is already in progress.
  */
  if ( mmoc_info_ptr->curr_trans == MMOC_TRANS_NULL )
  {
    /* No transaction in progress, start new transaction.
    */
    evt_status = mmoc_setup_trans( cmd_ptr,
                                   rpt_ptr,
                                   mmoc_info_ptr
                                 );

  } /* if ( mmoc_info_ptr->curr_trans == MMOC_TRANS_NULL )  */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Call the transaction handlers until the event is consumed.
  */
  while ( evt_status != MMOC_EVT_STATUS_CONSUMED )
  {
    last_trans = mmoc_info_ptr->curr_trans;
    last_trans_state = mmoc_info_ptr->trans_state;


    switch( mmoc_info_ptr->curr_trans )
    {

      case MMOC_TRANS_PROT_GEN_CMD:
        if (cmd_ptr == NULL &&
            mmoc_info_ptr->trans_state == MMOC_TRANS_STATE_NULL)
        {
          MMOC_ERR_0( "cmd_ptr is NULL for GEN_CMD");
          evt_status = mmoc_clear_transaction( mmoc_info_ptr );
        }
        else
        {
          evt_status = mmoc_process_prot_gen_cmd( cmd_ptr,
                                                rpt_ptr,
                                                mmoc_info_ptr
                                              );
        }
        break;

      case MMOC_TRANS_SUBSC_CHGD:
        evt_status = mmoc_process_subsc_chgd( cmd_ptr,
                                              rpt_ptr,
                                              mmoc_info_ptr
                                            );
        break;

      case MMOC_TRANS_ONLINE:
        evt_status = mmoc_process_online( cmd_ptr,
                                          rpt_ptr,
                                          mmoc_info_ptr
                                        );
        break;


      case MMOC_TRANS_OFFLINE:
        evt_status = mmoc_process_offline( cmd_ptr,
                                           rpt_ptr,
                                           mmoc_info_ptr
                                         );
        break;

      case MMOC_TRANS_PWR_DOWN:
        evt_status = mmoc_process_pwr_down( cmd_ptr,
                                            rpt_ptr,
                                            mmoc_info_ptr
                                          );
        break;

      case MMOC_TRANS_PWR_SAVE_ENTER:
        evt_status = mmoc_process_pwr_save_enter( cmd_ptr,
                                                  rpt_ptr,
                                                  mmoc_info_ptr
                                                );
        break;


      case MMOC_TRANS_PROT_AUTO_DEACTD_IND:
        evt_status = mmoc_process_auto_deactd_ind( cmd_ptr,
                                                   rpt_ptr,
                                                   mmoc_info_ptr
                                                 );
        break;

      case MMOC_TRANS_SUSPEND_SS:
        evt_status = mmoc_process_suspend_ss( cmd_ptr,
                                              rpt_ptr,
                                              mmoc_info_ptr
                                             );
        break;

      case MMOC_TRANS_MAX:
      case MMOC_TRANS_NULL:
      default:
        MMOC_MSG_HIGH_1("Invalid transaction %d", mmoc_info_ptr->curr_trans);
        evt_status = mmoc_clear_transaction( mmoc_info_ptr );
        break;

    } /* switch ( mmoc_info_ptr->curr_trans ) */

    /* ASSERT: if the event is not consumed, at least the transaction
    ** should have been changed.
    */
    if ( evt_status == MMOC_EVT_STATUS_NOT_CONSUMED &&
         last_trans == mmoc_info_ptr->curr_trans &&
         last_trans_state == mmoc_info_ptr->trans_state
       )
    {
      MMOC_ERR_2("Stuck in trans %d %d", last_trans, last_trans_state);

      /*lint -save -e506 -e774*/
      MMOC_ASSERT( FALSE );
      /*lint -restore*/
    }

  } /* while ( evt_status != MMOC_EVT_STATUS_CONSUMED ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if(mmoc_info_ptr->curr_trans >= MMOC_TRANS_MAX)
  {
      MMOC_ERR_1( "invalid curr_trans: %d", mmoc_info_ptr->curr_trans);
      return;
  }

  if(mmoc_info_ptr->trans_state >= MMOC_TRANS_STATE_MAX)
  {
      MMOC_ERR_1( "invalid trans_state: %d", mmoc_info_ptr->trans_state);
      return;
  }

  #ifndef FEATURE_MMOC_LOW_MEM_TARGET
  /* Print debug info after calling transaction handler.
  */
  mmocdbg_print_after_trans( mmoc_info_ptr );
  #endif /* FEATURE_MMOC_LOW_MEM_TARGET */

  return;

} /* mmoc_call_trans_hndlr() */

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== CM -> MMOC commands ==========================
=============================================================================
=============================================================================
===========================================================================*/

/* The following section defines the API when the clients ( example CM )
** have to communicate to MMoC using the command queue.
*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_cmd_subscription_chgd3

DESCRIPTION
  This function should be called when the subscription changed because of NAM
  change or GSM/WCDMA subscription availability change or CDMA/AMPS/HDR
  subscription availability change. This function will queue the subscription
  changed command to the MMoC task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           mmoc_cmd_subscription_chgd3
(
  mmoc_subsc_chg_e_type          chg_type,
  /* Subscription change type.
  */

  byte                           nam,
  /* Current NAM selection (NAM-1, NAM-2, etc.).
  */

  sd_ss_orig_mode_e_type         orig_mode,
  /* The origination mode (normal, OTASP, emergency, other, etc.)
  ** that is associated with the selected NAM.
  */

  sd_ss_mode_pref_e_type         mode_pref,
  /* The mode preference that is associated with the selected NAM.
  */

  sd_ss_band_pref_e_type         band_pref,
  /* Band preference that is associated with the selected NAM.
  */

  sys_lte_band_mask_e_type       lte_band_pref,
  /* LTE band preference that is associated with the selected NAM.
  */

  sd_ss_band_pref_e_type         tds_band_pref,
  /* TD-SCDMA band preference that is associated with the selected NAM.
  */

  sd_ss_prl_pref_e_type          prl_pref,
  /* PRL preference that is associated with the selected NAM.
  */

  sd_ss_roam_pref_e_type         roam_pref,
  /* Roam preference that is associated with the selected NAM.
  */

  sd_ss_hybr_pref_e_type         hybr_pref,
  /* New hybrid preference (none, CDMA/HDR).
  */

  const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
  /* Pointer to struct that defines the manual system.
  */

  sd_ss_srv_domain_pref_e_type   srv_domain_pref,
  /* Service domain preference that is associated with the selected
  ** NAM.
  ** Applies only for GSM/WCDMA modes.
  */

  sd_ss_acq_order_pref_e_type    acq_order_pref,
  /* Preference for the order of acquisition ( WCDMA before GSM,
  ** GSM before WCDMA etc).
  */


  boolean                        is_gwl_subsc_avail,
  /* Current GSM/WCDMA/LTE subscription availability status.
  ** true = subscription available,
  ** false = subscription not available.
  */

  boolean                        is_cdma_subsc_avail,
  /* Current CDMA/AMPS/HDR subscription availability status.
  ** true = subscription available,
  ** false = subscription not available.
  */


  prot_subsc_chg_e_type          prot_subsc_chg
  /* Enumeration of protocols that have a change in
  ** subscription available status
  */
)
{
  mmoc_subs_data_s_type                gw;
  mmoc_state_info_s_type*              mmoc_info_ptr = mmoc_get_state_info_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
    gw.is_subs_avail          = is_gwl_subsc_avail;

    #ifndef FEATURE_MMODE_LTE_ONLY
    gw.is_perso_locked      = FALSE;
    #endif
    
    
    gw.orig_mode              = orig_mode;
    gw.mode_pref              = mode_pref;
    gw.lte_band_pref          = lte_band_pref;
    #ifndef FEATURE_MMODE_LTE_ONLY
    gw.band_pref              = band_pref;
    gw.tds_band_pref          = tds_band_pref;
    gw.gw.acq_order_pref  = acq_order_pref;
    #endif
    gw.roam_pref              = roam_pref;
    gw.hybr_pref              = hybr_pref;
    gw.gw.srv_domain_pref = srv_domain_pref;

    gw.gw.manual_sys_info.sys_id.id_type
                                                = SYS_SYS_ID_TYPE_UNDEFINED;
    if ( manual_sys_info_ptr != NULL )
    {
      gw.gw.manual_sys_info   = *manual_sys_info_ptr;
    }
    
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mmoc_cmd_subscription_chgd_new(chg_type,
                  prot_subsc_chg,
                  &gw
                 );
  return;


} /* mmoc_cmd_subscription_chgd() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_cmd_subscription_chgd_new

DESCRIPTION
  This function should be called when the subscription changed because of NAM
  change or GSM/WCDMA subscription availability change or CDMA/AMPS/HDR
  subscription availability change. This function will queue the subscription
  changed command to the MMoC task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           mmoc_cmd_subscription_chgd_new
(
        mmoc_subsc_chg_e_type                  chg_type,
             /* Subscription change type.
             */
        prot_subsc_chg_e_type                     prot_subsc_chg,


        const mmoc_subs_data_s_type          *gw_subs_ptr
           /* GW subscription info */

)
{
  mmoc_cmd_msg_s_type                 *msg_ptr;
  mmoc_state_info_s_type*              mmoc_info_ptr = mmoc_get_state_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef CM_DEBUG
  #error code not present
#endif

  /* Get the command buffer.
  */
  msg_ptr = mmoc_get_cmd_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->cmd.name                             = MMOC_CMD_SUBSCRIPTION_CHGD;

  msg_ptr->param.subsc_chgd.chg_type            = chg_type;
  msg_ptr->param.subsc_chgd.prot_subsc_chg  = prot_subsc_chg;

  if(gw_subs_ptr != NULL)
  {
    msg_ptr->param.subsc_chgd.gw                   = *gw_subs_ptr;
  }

  MMOC_MSG_HIGH_2("CM->MMOC: chg_type %d, gw_subs_ptr=0x%x",
                        chg_type,
                        gw_subs_ptr);


  /* Post the command.
  */
  mmoc_send_cmd( msg_ptr );


  return;

} /* mmoc_cmd_subscription_chgd_new() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_cmd_oprt_mode_chgd

DESCRIPTION
  This function will be used to command the MMoC to do operating mode change,
  the new operating mode is specified in the oprt_mode parameter.  This
  function will queue the operating mode changed command to the MMoC task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           mmoc_cmd_oprt_mode_chgd
(

  sys_oprt_mode_e_type           oprt_mode
  /* The operating mode to be switched to.
  */

)
{
  mmoc_cmd_msg_s_type                 *msg_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef CM_DEBUG
  #error code not present
#endif

  /* Get the command buffer.
  */
  msg_ptr = mmoc_get_cmd_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->cmd.name                       = MMOC_CMD_OPRT_MODE_CHGD;
  msg_ptr->param.oprt_mode_chgd.oprt_mode = oprt_mode;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the command.
  */
  mmoc_send_cmd( msg_ptr );

  return;

} /* mmoc_cmd_oprt_mode_chgd() */



/*===========================================================================

FUNCTION mmoc_cmd_pref_sys_chgd7

DESCRIPTION
  This function is used to command the MMoC to do Preferred system selection
  change. This function will queue the preferred system selection changed
  command to the MMoC Task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           mmoc_cmd_pref_sys_chgd7
(


  sd_ss_pref_reas_e_type         pref_reas,
  /* Reason for changing the SS-Preference.
  */

  sd_ss_orig_mode_e_type         orig_mode,
  /* The origination mode (normal, OTASP, emergency, other, etc.)
  ** that is associated with the selected NAM.
  */

  sd_ss_mode_pref_e_type         mode_pref,
  /* The mode preference that is associated with the selected NAM.
  */

  sd_ss_band_pref_e_type         band_pref,
  /* Band preference that is associated with the selected NAM.
  */

  sys_lte_band_mask_e_type         lte_band_pref,
  /* LTE band preference that is associated with the selected NAM.
  */

  sd_ss_band_pref_e_type         tds_band_pref,
  /* TD-SCDMA band preference that is associated with the selected NAM.
  */

  sd_ss_prl_pref_e_type          prl_pref,
  /* PRL preference that is associated with the selected NAM.
  */

  sd_ss_roam_pref_e_type         roam_pref,
  /* Roam preference that is associated with the selected NAM.
  */

  sd_ss_hybr_pref_e_type         hybr_pref,
  /* New hybrid preference (none, CDMA/HDR).
  */

  sd_band_e_type                 otasp_band,
  /* The band-class that is associated with OTASP origination mode.
  ** Note that this parameter is ignored if orig_mode != OTASP.
  */

  sd_blksys_e_type                otasp_blksys,
  /* The PCS frequency block/Cellular system that is associated
  ** with OTASP origination mode. Note that this parameter is ignored
  ** if orig_mode != OTASP.
  */

  sd_ss_avoid_sys_e_type          avoid_type,
  /* Type of the system to be avoided, valid only
  ** if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
  */

  dword                            avoid_time,
  /* Time in seconds for which the system is to be avoided, valid
  ** only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
  */

  const sys_manual_sys_info_s_type *manual_sys_info_ptr,
  /* Pointer to struct that defines the manual system.
  */

  sd_ss_srv_domain_pref_e_type      srv_domain_pref,
  /* Service domain preference that is associated with the selected
  ** NAM.
  ** Applies only for GSM/WCDMA modes.
  */

  sd_ss_acq_order_pref_e_type       acq_order_pref,
  /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

  sd_ss_pref_update_reason_e_type    pref_update_reas,
  /* Preference change reason
  */

  sys_addtl_action_s_type            *addl_action,
  /*
  To indicate sub action to be performed by MMOC during pref sys chgd cmd
  */

  sd_ss_mode_pref_e_type             user_mode_pref,
  /**< The user mode preference that is associated with the selected NAM.
  */

  const sd_rat_acq_order_s_type     *rat_acq_order_ptr,
  /**< Rat acquisition order including LTE.
  */

  uint16                             req_id,
  /**< System Selection Preferences request id.
  */

  sd_ss_pref_camp_mode_e_type  camp_only_pref,

  sys_csg_id_type                    csg_id,
  /**< CSG identifier */

  sys_radio_access_tech_e_type       csg_rat,
  /**< RAT specified for CSG */

  sys_voice_domain_pref_e_type       voice_domain_pref,
  /**< Voice domain pref */

  sd_ss_ciot_lte_pref_s_type        *ciot_lte_pref
  /**< CIOT lte preferences */

)
{
  mmoc_cmd_msg_s_type                 *msg_ptr;
  prot_cmd_pref_sys_chgd_s_type       pref_sys_chgd;
  uint8                               i = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef CM_DEBUG
  #error code not present
#endif

  /* Get the command buffer.
  */
  msg_ptr = mmoc_get_cmd_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MMOC_MSG_HIGH_6( "mmoc_cmd_pref_sys_chgd6: pref_reas: %d, orig_mode: %d,mode_pref: %d, band_pref: %d, pref_update_reas: %d, user_mode_pref: %d",
                   pref_reas, orig_mode,mode_pref, band_pref,pref_update_reas, user_mode_pref );
  mmoc_print_lte_band_mask(lte_band_pref);
  MMOC_MSG_HIGH_6( "mmoc_cmd_pref_sys_chgd6:tds_band_pref: %d, prl_pref: %d, roam_pref: %d,hybr_pref: %d, srv_domain_pref: %d, acq_order_pref: %d", tds_band_pref, prl_pref, roam_pref,
                    hybr_pref, srv_domain_pref, acq_order_pref );

  /* Fill in all the fields.
  */
  msg_ptr->cmd.name                              = MMOC_CMD_PROT_GEN_CMD;
  msg_ptr->param.gen_prot_cmd.cmd_type           = PROT_GEN_CMD_PREF_SYS_CHGD;

  pref_sys_chgd.pref_reas                        = pref_reas;
  pref_sys_chgd.orig_mode                        = orig_mode;
  pref_sys_chgd.mode_pref                        = mode_pref;
  pref_sys_chgd.lte_band_pref                    = lte_band_pref;
  #ifndef FEATURE_MMODE_LTE_ONLY
  pref_sys_chgd.band_pref                        = band_pref;
  pref_sys_chgd.tds_band_pref                    = tds_band_pref;
  pref_sys_chgd.acq_order_pref                   = acq_order_pref;
  #else
  pref_sys_chgd.band_pref                        = SD_SS_BAND_PREF_NONE;
  pref_sys_chgd.tds_band_pref                    = SD_SS_BAND_PREF_NONE;
  pref_sys_chgd.acq_order_pref                   = SD_SS_ACQ_ORDER_PREF_AUTO;
  #endif
  pref_sys_chgd.roam_pref                        = roam_pref;

  pref_sys_chgd.prl_pref                         = SD_SS_PRL_PREF_NONE;
  pref_sys_chgd.hybr_pref                        = SD_SS_HYBR_PREF_NONE;
  pref_sys_chgd.otasp_band                       = SYS_BAND_CLASS_NONE;
  pref_sys_chgd.otasp_blksys                     = SD_BLKSYS_PCS_A;
  pref_sys_chgd.avoid_type                       = SD_SS_AVOID_SYS_IDLE_IF_UZ;
  pref_sys_chgd.avoid_time                       = 0;
  pref_sys_chgd.camp_mode_pref                   = SD_SS_PREF_CAMP_MODE_NONE;

  pref_sys_chgd.manual_sys_info.sys_id.id_type   = SYS_SYS_ID_TYPE_UNDEFINED;
  if ( manual_sys_info_ptr != NULL )
  {
    pref_sys_chgd.manual_sys_info                = *manual_sys_info_ptr;
  }
  pref_sys_chgd.domain_pref                      = srv_domain_pref;
  pref_sys_chgd.pref_update_reas                 = pref_update_reas;
  if (addl_action != NULL)
  {
    pref_sys_chgd.addl_action                      = *addl_action;
  }
  else
  {
    pref_sys_chgd.addl_action.action             = MMOC_PREF_SYS_ADDL_ACTION_NONE;   
    pref_sys_chgd.addl_action.ue_mode            = SYS_UE_MODE_NONE;
    pref_sys_chgd.addl_action.is_ue_mode_substate_srlte = FALSE;
  }
  pref_sys_chgd.user_mode_pref                   = user_mode_pref;
  pref_sys_chgd.sys_sel_pref_req_id              = req_id;

  #ifndef FEATURE_MMODE_DISABLE_CSG
  pref_sys_chgd.csg_id                           = csg_id;
  #else
  pref_sys_chgd.csg_id                           = SYS_CSG_ID_INVALID;
  #endif
  pref_sys_chgd.voice_domain_pref                = voice_domain_pref;
  pref_sys_chgd.csg_rat                          = csg_rat;

  if(rat_acq_order_ptr != NULL)
  {
    pref_sys_chgd.rat_acq_order.version = rat_acq_order_ptr->version;
    pref_sys_chgd.rat_acq_order.num_rat = rat_acq_order_ptr->num_rat;

    for(i=0; i < SYS_MAX_ACQ_PREF_RAT_LIST_NUM; i++)
    {
      pref_sys_chgd.rat_acq_order.acq_sys_mode[i] = \
                  rat_acq_order_ptr->acq_sys_mode[i];
    }
  }
  else
  {
    /* If no acquisition order, just fill up the acq_order with NONE */
    pref_sys_chgd.rat_acq_order.version = 0xFF;
    pref_sys_chgd.rat_acq_order.num_rat = 0;

    for(i=0; i < SYS_MAX_ACQ_PREF_RAT_LIST_NUM; i++)
    {
      pref_sys_chgd.rat_acq_order.acq_sys_mode[i] = SYS_SYS_MODE_NONE;
    }

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ciot_lte_pref != NULL )
  {
    pref_sys_chgd.ciot_lte_pref  = *ciot_lte_pref;
  }
  else
  {
   
   pref_sys_chgd.prl_pref                         = SD_SS_PRL_PREF_NONE;
   pref_sys_chgd.hybr_pref                        = SD_SS_HYBR_PREF_NONE;
   pref_sys_chgd.otasp_band                       = SYS_BAND_CLASS_NONE;
   pref_sys_chgd.otasp_blksys                     = SD_BLKSYS_PCS_A;
   pref_sys_chgd.avoid_type                       = SD_SS_AVOID_SYS_IDLE_IF_UZ;
   pref_sys_chgd.avoid_time                       = 0;
   pref_sys_chgd.camp_mode_pref                   = SD_SS_PREF_CAMP_MODE_NONE;
   pref_sys_chgd.ciot_lte_pref.lte_op_mode_pref   = SD_SS_LTE_IOT_OP_MODE_PREF_NONE;
   pref_sys_chgd.ciot_lte_pref.lte_nb1_band_pref  = SYS_LTE_BAND_MASK_CONST_NONE;
   pref_sys_chgd.ciot_lte_pref.lte_m1_band_pref   = SYS_LTE_BAND_MASK_CONST_NONE;
   pref_sys_chgd.ciot_lte_pref.lte_band_pref      = SYS_LTE_BAND_MASK_CONST_NONE;

  }
  /* Copy the parameters into generic command struct.
  */
  msg_ptr->param.gen_prot_cmd.param.pref_sys_chgd  = pref_sys_chgd;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MMOC_MSG_HIGH_6("Sending PROT_GEN_CMD_PREF_SYS_CHGD,pref_reas=%d, mode_pref=%d"
                    "is_mode_change_due_to_ef_rat:%d, req id=%d, orig_mode=%d csg_rat=%d",
                 pref_reas, mode_pref, pref_update_reas, req_id, orig_mode, csg_rat);


  /* Post the command.
  */
  mmoc_send_cmd( msg_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* To detect duplicate pref_sys_chgd commands, set the SIG.
  */
  (void)rex_set_sigs( cm_tcb_ptr, MMOC_DUP_CMD_REM_SIG );

  return;

} /* mmoc_cmd_pref_sys_chgd7() */


/* <EJECT> */
/**===========================================================================

@FUNCTION mmoc_cmd_get_networks_gw

@DESCRIPTION
  The function will be used to command the MMoC to get the networks on
  GSM/WCDMA/TD-SCDMA/LTE technologies. This function will queue the Get networks
  command to the MMoC task.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
void                           mmoc_cmd_get_networks_gw
(

  sd_ss_mode_pref_e_type         mode_pref,
  /* Mode preference that is associated with the Get networks
  ** command.
  */


   sd_ss_band_pref_e_type         band_pref,
   /* Band preference that is associated with the Get networks
   ** command.
   */

   sys_lte_band_mask_e_type       lte_band_pref,
   /* Band preference that is associated with the Get networks
   ** command.
   */


   sd_ss_band_pref_e_type         tds_band_pref,
   /* TD-SCDMA band preference that is associated with the Get networks
   ** command.
   */

   sd_network_list_type_e_type    list_type,
   /**< Network List Type */

   sd_ss_ciot_lte_pref_s_type     ciot_lte_pref,
   /* ciot lte pref  */

   uint8                            cell_search_num_cells,
  /* Indicates the maximum number of cells to be reported in LIST_CELL_SEARCH type 
    ** 0 Indicates no restrictions on the number of cells. */

   sys_wwan_adavnce_scan_pram_s_type* gps_wwan_scan_params

   /* advance scan paramter passed by GPS for scanning purpose */
)
{
  mmoc_cmd_msg_s_type                 *msg_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef CM_DEBUG
    #error code not present
#endif


  /* Get the command buffer.
  */
  msg_ptr = mmoc_get_cmd_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->cmd.name                    = MMOC_CMD_PROT_GEN_CMD;
  msg_ptr->param.gen_prot_cmd.cmd_type = PROT_GEN_CMD_GET_NETWORKS_GW;
  msg_ptr->param.gen_prot_cmd.param.gw_get_net.mode_pref         = mode_pref;
  msg_ptr->param.gen_prot_cmd.param.gw_get_net.band_pref         = band_pref;
  msg_ptr->param.gen_prot_cmd.param.gw_get_net.lte_band_pref     = lte_band_pref;
  msg_ptr->param.gen_prot_cmd.param.gw_get_net.tds_band_pref     = tds_band_pref;
  msg_ptr->param.gen_prot_cmd.param.gw_get_net.network_list_type = list_type;
  msg_ptr->param.gen_prot_cmd.param.gw_get_net.ciot_lte_pref     = ciot_lte_pref;
  msg_ptr->param.gen_prot_cmd.param.gw_get_net.cell_search_num_cells = cell_search_num_cells;
  #ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
  if( gps_wwan_scan_params != NULL )
  {
    memscpy((void *) &(msg_ptr->param.gen_prot_cmd.param.gw_get_net.gps_wwan_scan_params),sizeof(sys_wwan_adavnce_scan_pram_s_type),(void *) gps_wwan_scan_params,sizeof(sys_wwan_adavnce_scan_pram_s_type));
  }
  #endif
  /* Post the command.
  */
  mmoc_send_cmd( msg_ptr );


  return;

} /* mmoc_cmd_get_networks_gw() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_cmd_term_get_networks_gw

DESCRIPTION
  This function will be used to inform the MMoC that the user has terminated
  the Get networks request on GSM/WCDMA networks. This function will queue
  the Terminate get networks command to the MMoC task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           mmoc_cmd_term_get_networks_gw(void)
{

  mmoc_cmd_msg_s_type                  *msg_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the command buffer.
  */
  msg_ptr = mmoc_get_cmd_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->cmd.name                    = MMOC_CMD_PROT_GEN_CMD;
  msg_ptr->param.gen_prot_cmd.cmd_type = PROT_GEN_CMD_TERM_GET_NETWORKS_GW;

 
  /* Post the command.
  */
  mmoc_send_cmd( msg_ptr );


  return;

} /* mmoc_cmd_term_get_networks_gw(SD_SS_MAIN) */

/* <EJECT> */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_cmd_term_get_networks_wlan

DESCRIPTION
  This function will be used to inform the MMoC that the user has terminated
  the Get networks request on WLAN networks. This function will queue
  the Terminate get networks command to the MMoC task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

/*===========================================================================

FUNCTION mmoc_cmd_prot_gw_sim_state_update_ind

DESCRIPTION
   This function will be used by cmregprx to inform MMoC that it needs
   to update the sim state on T3245 timer expiry indication from NAS.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void                           mmoc_cmd_prot_gw_sim_state_update_ind
(
   prot_cmd_gw_sim_state_e_type         sim_state
   /* New sim state reported from REG
   */
)
{

  mmoc_cmd_msg_s_type                  *msg_ptr;

  #ifdef CM_DEBUG
    #error code not present
#endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the command buffer.
  */
  msg_ptr = mmoc_get_cmd_buf_else_err_fatal();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in all the fields.
  */
  msg_ptr->cmd.name                    = MMOC_CMD_PROT_GEN_CMD;
  msg_ptr->param.gen_prot_cmd.cmd_type = PROT_GEN_CMD_GW_SIM_STATE;
  msg_ptr->param.gen_prot_cmd.param.gw_sim_state = sim_state;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command.
  */
  mmoc_send_cmd( msg_ptr );

  return;

} /*  mmoc_cmd_prot_gw_sim_state_update_ind() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_cmd_wakeup_from_pwr_save

DESCRIPTION
  This function will be used to command the MMoC to wakeup the phone from
  power save.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           mmoc_cmd_wakeup_from_pwr_save(void)
{
  mmoc_cmd_msg_s_type                 *msg_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the command buffer.
  */
  msg_ptr = mmoc_get_cmd_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->cmd.name                       = MMOC_CMD_PROT_GEN_CMD;
  msg_ptr->param.gen_prot_cmd.cmd_type    = PROT_GEN_CMD_WAKEUP_FROM_PWR_SAVE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the command.
  */
  mmoc_send_cmd( msg_ptr );

  return;

} /* mmoc_cmd_wakeup_from_pwr_save() */


/*===========================================================================

FUNCTION  mmoc_cmd_suspend_ss_new

DESCRIPTION
  This function will be used to command the MMoC to suspend or resume
  operation on a given system selection instance when emergency call is dialed on other instance.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                            mmoc_cmd_suspend_ss_new
(

  boolean                        is_suspend,
    /* whether to suspend or resume
    */
  boolean                        ignore_protocol_activate,
    /* Whether to activate protocol or not during resume
    */
  prot_deact_e_type              susp_reason
  /* Reason for suspension
  */

)
{
  mmoc_cmd_msg_s_type            *msg_ptr;
#ifdef CM_DEBUG
  #error code not present
#endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the command buffer.
  */
  msg_ptr = mmoc_get_cmd_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->cmd.name                          = MMOC_CMD_SUSPEND_SS;
  msg_ptr->param.suspend_ss.is_suspend       = is_suspend;
  msg_ptr->param.suspend_ss.ignore_protocol_activate = ignore_protocol_activate ;
  msg_ptr->param.suspend_ss.susp_reason      = susp_reason;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MMOC_MSG_HIGH_2("suspend_resume is_suspend %d, susp_reason %d",
    is_suspend, susp_reason);

  /* Post the command.
  */
  mmoc_send_cmd( msg_ptr );

  return;

} /* mmoc_cmd_suspend_ss_new() */


/*===========================================================================

FUNCTION  mmoc_cmd_suspend_ss

DESCRIPTION
  This function will be used to command the MMoC to suspend or resume
  operation on a given system selection instance.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                            mmoc_cmd_suspend_ss
(

  boolean                        is_suspend,
    /* whether to suspend or resume
    */
  boolean                        ignore_protocol_activate
    /* Whether to activate protocol or not during resume
    */
)
{

  mmoc_cmd_suspend_ss_new(is_suspend, ignore_protocol_activate, PROT_DEACT_LOCAL_DETACH);

  return;

} /* mmoc_cmd_suspend_ss() */

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== Protocols -> MMOC reports ====================
=============================================================================
=============================================================================
===========================================================================*/

/* The following section defines the API when the clients ( example protocols
** ) have to communicate to MMoC using the report queue.
*/

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_prot_deactd_cnf

DESCRIPTION
  This function will be used to confirm that the MMoC that the protocol has
  been deactivated. This function will queue the protocol deactivated report
  to the MMoC task.

  Note! This function should be called only in response to protocol
  deactivate request from MMoC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           mmoc_rpt_prot_deactd_cnf
(

  prot_trans_type                trans_id
  /* Protocols set this same as the trans_id received in their
  ** Deactivate request.
  */

)
{
  mmoc_rpt_msg_s_type                  *msg_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the report buffer.
  */
  msg_ptr = mmoc_get_rpt_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->rpt.name                       = MMOC_RPT_PROT_DEACTD_CNF;
  msg_ptr->param.prot_deactd_cnf.trans_id = trans_id;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the report.
  */
  mmoc_send_rpt( msg_ptr );


  return;

} /* mmoc_rpt_prot_deactd_cnf() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_prot_auto_deactd_ind

DESCRIPTION
   This function will be used to inform the MMoC that the protocol has been
  deactivated autonomously. This function will queue the deactivated
  indication report to the MMoC task.

  Note! This function should not called in response to protocol deactivate
  request from MMoC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           mmoc_rpt_prot_auto_deactd_ind
(

  prot_deact_e_type              deactd_reason,
  /* Reason that the protocol was deactivated.
  */

  prot_state_e_type              prot_state
  /* Protocol which sent the autonomous deactivation.
  */

)
{

  /* Set ss = SD_SS_MAX. MMOC will consider the stack on which this protocol is active */
  mmoc_rpt_prot_auto_deactd_ind_ss(deactd_reason, prot_state);
  return;

} /* mmoc_rpt_prot_auto_deactd_ind() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_prot_auto_deactd_ind_ss

DESCRIPTION
   This function will be used to inform the MMoC that the protocol has been
  deactivated autonomously. This function will queue the deactivated
  indication report to the MMoC task.

  Note! This function should not called in response to protocol deactivate
  request from MMoC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           mmoc_rpt_prot_auto_deactd_ind_ss
(

        prot_deact_e_type              deactd_reason,
           /* Reason that the protocol was deactivated.
           */

        prot_state_e_type              prot_state
           /* Protocol which sent the autonomous deactivation.
           */

)
{
  mmoc_rpt_msg_s_type                  *msg_ptr;

  MMOC_ASSERT( deactd_reason != PROT_DEACT_PWR_DOWN   &&
               deactd_reason != PROT_DEACT_RESET      &&
               deactd_reason != PROT_DEACT_LPM        &&
               deactd_reason != PROT_DEACT_OFFLINE    &&
               deactd_reason != PROT_DEACT_MAX
             );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the report buffer.
  */
  msg_ptr = mmoc_get_rpt_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->rpt.name                     = MMOC_RPT_PROT_AUTO_DEACTD_IND;
  msg_ptr->param.prot_deactd_ind.reason = deactd_reason;
  msg_ptr->param.prot_deactd_ind.prot_state   = prot_state;

  MMOC_MSG_HIGH_2("Sending MMOC_RPT_PROT_AUTO_DEACTD_IND, prot_deactd_ind \
                 reason=%d,prot_state=%d", deactd_reason,
                 prot_state);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the report.
  */
  mmoc_send_rpt( msg_ptr );


  return;

} /* mmoc_rpt_prot_auto_deactd_ind_ss() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_ph_stat_chgd_cnf2

DESCRIPTION
  This function should be used by the protocols to acknowledge the MMoC that
  the "phone status changed (due to either NAM change, Subscription
  availability change(GSM/WCDMA or CDMA/AMPS/HDR) or operating mode change
  request. This function will queue the phone status changed confirmation
  report to the MMoC task.

  Note! This should be called only after completing the necessary
  housekeeping actions based on the request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           mmoc_rpt_ph_stat_chgd_cnf2
(

        prot_trans_type                trans_id,
          /* Protocols set this same as the trans_id received in the phone
          ** status changed ack. trans_id cannot be MMOC_TRANS_AUTONOMOUS.
          */

        mmoc_mode_e_type                 prot
          /* Protocol responding to the Phone status changed command.
          */


)
{
  mmoc_rpt_msg_s_type                  *msg_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the report buffer.
  */
  msg_ptr = mmoc_get_rpt_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->rpt.name                   = MMOC_RPT_PH_STAT_CHGD_CNF;
  msg_ptr->param.ph_stat_cnf.trans_id = trans_id;
  msg_ptr->param.ph_stat_cnf.prot     = prot;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the report.
  */
  mmoc_send_rpt( msg_ptr );


  return;

} /* mmoc_rpt_ph_stat_chgd_cnf2() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_prot_auto_actd_ind

DESCRIPTION
  This function will be used to inform the MMoC that the protocol has been
  activated autonomously. This function will queue the activated
  indication report to the MMoC task.

  Note! This function should not called in response to protocol activate
  request from MMoC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           mmoc_rpt_prot_auto_actd_ind
(

  prot_act_e_type              actd_reason,
  /* Reason that the protocol was deactivated.
  */

  prot_state_e_type              prot_state
  /* Protocol which sent the autonomous deactivation.
  */

)
{
  mmoc_rpt_msg_s_type                  *msg_ptr;

  MMOC_ASSERT( actd_reason == PROT_ACT_GWL_REDIR );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the report buffer.
  */
  msg_ptr = mmoc_get_rpt_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->rpt.name                     = MMOC_RPT_PROT_AUTO_ACTD_IND;
  msg_ptr->param.prot_actd_ind.reason   = actd_reason;
  msg_ptr->param.prot_actd_ind.prot_state   = prot_state;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the report.
  */
  mmoc_send_rpt( msg_ptr );


  return;

} /* mmoc_rpt_prot_auto_actd_ind() */


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_ph_stat_chgd_cnf

DESCRIPTION
  This function should be used by the protocols to acknowledge the MMoC that
  the "phone status changed (due to either NAM change, Subscription
  availability change(GSM/WCDMA or CDMA/AMPS/HDR) or operating mode change
  request. This function will queue the phone status changed confirmation
  report to the MMoC task.

  Note! This should be called only after completing the necessary
  housekeeping actions based on the request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           mmoc_rpt_ph_stat_chgd_cnf
(

  prot_trans_type                trans_id,
            /**< Protocols set this same as the trans_id received in the phone
  ** status changed ack. trans_id cannot be MMOC_TRANS_AUTONOMOUS.
  */

          sd_mode_e_type                 prot,
            /**< Protocol responding to the Phone status changed command. */

          mmoc_ph_stat_e_type        req_status
  /**< status of the command - success or error code. */          

)


{
  mmoc_rpt_msg_s_type                  *msg_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the report buffer.
  */
  msg_ptr = mmoc_get_rpt_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->rpt.name                   = MMOC_RPT_PH_STAT_CHGD_CNF;
  msg_ptr->param.ph_stat_cnf.trans_id = trans_id;
  msg_ptr->param.ph_stat_cnf.prot     = (mmoc_mode_e_type)prot;
  msg_ptr->param.ph_stat_cnf.mc_status     = req_status;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the report.
  */
  mmoc_send_rpt( msg_ptr );


  return;

} /* mmoc_rpt_ph_stat_chgd_cnf() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_activation_cnf

DESCRIPTION
  This function should be used by the protocol ( CMREGPRX) to inform MMOC that
  activation is complete.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           mmoc_rpt_activation_cnf
(

        prot_state_e_type              prot_state
          /* Protocol sending the report
          */
)
{

  #ifdef FEATURE_MMODE_LTE_RESEL
  #error code not present
#else
  SYS_ARG_NOT_USED(prot_state);
  #endif // FEATURE_MMODE_LTE_RESEL
  return;

} /* mmoc_rpt_activation_cnf() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_ps_detach_cnf

DESCRIPTION
  This function should be used by the protocols to acknowledge the MMoC that
  the "PS detach is done".
  This function will queue the ps detach confirmation
  report to the MMoC task.

  Note! This should be called only after completing the necessary
  housekeeping actions based on the request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           mmoc_rpt_ps_detach_cnf
(

        prot_trans_type                trans_id
          /* Protocols set this same as the trans_id received in the phone
          ** status changed ack. trans_id cannot be MMOC_TRANS_AUTONOMOUS.
          */
)
{
  mmoc_rpt_msg_s_type                  *msg_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the report buffer.
  */
  msg_ptr = mmoc_get_rpt_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->rpt.name                   = MMOC_RPT_PS_DETACH_CNF;
  msg_ptr->param.ps_detach_cnf.trans_id = trans_id;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the report.
  */
  mmoc_send_rpt( msg_ptr );


  return;

} /* mmoc_rpt_ps_detach_cnf() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_rpt_prot_gen_cmd_cnf

DESCRIPTION
  This function is used to acknowledge the generic protocol command.

  Note!  This is a local function and should be called by
  mmoc_proc_prot_gen_cmd.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           mmoc_rpt_prot_gen_cmd_cnf
(

  boolean                        is_cmd_processed,
  /* If the protocols received the generic command in the incorrect
  ** state, then this will be set to FALSE.
  */

  prot_trans_type                trans_id,
  /* Protocols set this same as the trans_id received in the generic
  ** command. trans_id cannot be MMOC_TRANS_AUTONOMOUS.
  */

  boolean                        is_prot_deactivating
  /* Is the protocol deactiavting because of processing the generic
  ** command. Valid when when is_cmd_processed = TRUE.
  */

)
{
  mmoc_rpt_msg_s_type                  *msg_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the report buffer.
  */
  msg_ptr = mmoc_get_rpt_buf_else_err_fatal();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  msg_ptr->rpt.name                           = MMOC_RPT_PROT_GEN_CMD_CNF;
  msg_ptr->param.gen_cmd_cnf.is_cmd_processed = is_cmd_processed;
  msg_ptr->param.gen_cmd_cnf.trans_id         = trans_id;
  msg_ptr->param.gen_cmd_cnf.is_prot_deactivating = is_prot_deactivating;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the report.
  */
  mmoc_send_rpt( msg_ptr );


  return;

} /*  mmoc_rpt_prot_gen_cmd_cnf() */


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== Protocols -> MMOC functional interface =======
=============================================================================
=============================================================================
===========================================================================*/

/* The following section defines the API when the clients ( example
** protocols ) have to communicate to MMoC using the functional interface.
*/


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_proc_prot_gen_cmd

DESCRIPTION
  This function should be used by protocols to process the Generic command
  received from MMoC.  The Generic command could be received because of
  Reselection timeout, Get networks request, Terminate get networks or
  preferred system selection change.

  Note! This function will also send the Generic command ack to MMoC task.

DEPENDENCIES
  None.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system.

    SD_SS_ACT_ACQ_GWL  -   Attempt acquiring an HDR system.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH.

    SD_SS_ACT_PWR_SAVE  - Enter power save mode.


SIDE EFFECTS
  None.
===========================================================================*/
sd_ss_act_e_type               mmoc_proc_prot_gen_cmd
(

  boolean                        is_resel_allowed,
  /* Protocol indicated if reselection is allowed at this time or not.
  ** TRUE = reselection allowed. This flag is passed on to SD20 in the
  ** call to sd_ss_ind_misc_timer() or sd_ss_ind_user_irat_to_gwl().
  ** So, this parameter is used only if the gen_ss_cmd type is Reselection
  ** timer expired or IRAT reselection to LTE. For other types it is not used.
  */

  boolean                        is_prot_active,
  /* Flag which specifies if the protocol calling this function is
  ** ACTIVE or not.
  */


  const prot_gen_cmd_s_type           *gen_cmd_ptr
  /* Pointer to generic system selection command information.
  ** Same as what was received by the protocols.
  */

)
{
  sd_ss_act_e_type    ss_act = SD_SS_ACT_CONTINUE;
  boolean             is_cmd_processed = FALSE;
  boolean             is_prot_deactivating = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MMOC_MSG_HIGH_5("MMOC->SD: GEN_CMD: is_resel_allowed: %d, is_prot_active %d, trans_id %d,cmd_type %d, mode_pref %d",
                       is_resel_allowed, is_prot_active, 
                       gen_cmd_ptr->trans_id, gen_cmd_ptr->cmd_type, gen_cmd_ptr->param.pref_sys_chgd.mode_pref );


  /* Check if the protocol is active or pref need to be restored at SD. In
     case of restore, MMOC internally calls this API to update pref to SD */
  if ( is_prot_active || is_pref_chng_restore_req(gen_cmd_ptr))
  {

    switch (gen_cmd_ptr->cmd_type )
    {
      case PROT_GEN_CMD_SS_TIMER_EXP:
        ss_act = sd_ss_ind_misc_timer( is_resel_allowed, NULL );
        break;

      case PROT_GEN_CMD_PREF_SYS_CHGD:
        ss_act = sd_ss_ind_user_ss_pref8
                 (
                   gen_cmd_ptr->param.pref_sys_chgd.pref_reas,
                   gen_cmd_ptr->param.pref_sys_chgd.orig_mode,
                   gen_cmd_ptr->param.pref_sys_chgd.mode_pref,
                   gen_cmd_ptr->param.pref_sys_chgd.band_pref,
                   gen_cmd_ptr->param.pref_sys_chgd.lte_band_pref,
                   gen_cmd_ptr->param.pref_sys_chgd.tds_band_pref,
                   gen_cmd_ptr->param.pref_sys_chgd.prl_pref,
                   gen_cmd_ptr->param.pref_sys_chgd.roam_pref,
                   gen_cmd_ptr->param.pref_sys_chgd.hybr_pref,
                   gen_cmd_ptr->param.pref_sys_chgd.otasp_band,
                   gen_cmd_ptr->param.pref_sys_chgd.otasp_blksys,
                   gen_cmd_ptr->param.pref_sys_chgd.avoid_type,
                   gen_cmd_ptr->param.pref_sys_chgd.avoid_time,
                   &gen_cmd_ptr->param.pref_sys_chgd.manual_sys_info,
                   gen_cmd_ptr->param.pref_sys_chgd.domain_pref,
                   gen_cmd_ptr->param.pref_sys_chgd.acq_order_pref,
                   gen_cmd_ptr->param.pref_sys_chgd.pref_update_reas,
                   gen_cmd_ptr->param.pref_sys_chgd.user_mode_pref,
                   &gen_cmd_ptr->param.pref_sys_chgd.rat_acq_order,
                   gen_cmd_ptr->param.pref_sys_chgd.sys_sel_pref_req_id,
                   gen_cmd_ptr->param.pref_sys_chgd.camp_mode_pref,
                   gen_cmd_ptr->param.pref_sys_chgd.csg_id,
                   gen_cmd_ptr->param.pref_sys_chgd.csg_rat,
                   gen_cmd_ptr->param.pref_sys_chgd.voice_domain_pref,
                   gen_cmd_ptr->param.pref_sys_chgd.ciot_lte_pref,
                   NULL
                 );

        
        break;

      case PROT_GEN_CMD_GET_NETWORKS_GW:
        #ifdef MMOC_GWL_SUPPORTED
        ss_act = sd_ss_ind_user_get_net_gw_ss
                 (
                   gen_cmd_ptr->param.gw_get_net.mode_pref,
                   gen_cmd_ptr->param.gw_get_net.band_pref,
                   gen_cmd_ptr->param.gw_get_net.lte_band_pref,
                   gen_cmd_ptr->param.gw_get_net.tds_band_pref,
                   #ifdef SD_DEBUG
                   #error code not present
#endif
                   gen_cmd_ptr->param.gw_get_net.network_list_type,
                   gen_cmd_ptr->param.gw_get_net.ciot_lte_pref,
                   gen_cmd_ptr->param.gw_get_net.cell_search_num_cells,
                   &(gen_cmd_ptr->param.gw_get_net.gps_wwan_scan_params),
                   
                   NULL
                 );
        #endif
        break;

      case PROT_GEN_CMD_TERM_GET_NETWORKS_GW:
        #ifdef MMOC_GWL_SUPPORTED
        ss_act = sd_ss_ind_user_term_get_net_gw
                 (
                   NULL
                 );
        #endif
        break;

    case PROT_GEN_CMD_GW_SIM_STATE:
      #ifdef MMOC_GWL_SUPPORTED
      ss_act = sd_ss_ind_misc_gw_sim_state_update(gen_cmd_ptr->param.gw_sim_state, NULL );
      #endif
      break;

    case PROT_GEN_CMD_WAKEUP_FROM_PWR_SAVE:
      if ( ( ss_act = sd_ss_ind_user_pwr_save_exit( NULL ) ) == SD_SS_ACT_PWR_SAVE )
      {
        /* Put SD back in power save mode.
        */
        ss_act = sd_ss_ind_misc_pwr_save_enter( NULL );
      }
      break;

default:
        MMOC_ERR_0(" Unknown generic command ");
        ss_act = SD_SS_ACT_CONTINUE;
        break;
    } /*switch (gen_cmd_ptr->cmd_type ) */

    /* Store command was processed.
    */
    is_cmd_processed = TRUE;

    is_prot_deactivating = mmoc_is_prot_deactivating( mmoc_state_info.prot_state,
                                                      ss_act
                                                    );
   
  } /* if ( is_prot_active ) */
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Send the Generic command ack to MMoC if trans_id != TRANS_NONE.
  */
  if ( gen_cmd_ptr->trans_id != PROT_TRANS_NONE )
  {
    mmoc_rpt_prot_gen_cmd_cnf( is_cmd_processed,
                               gen_cmd_ptr->trans_id,
                               is_prot_deactivating
                             );
  } /*  if ( gen_cmd_ptr->trans_id != PROT_TRANS_NONE ) */


  /*if(ss_act != SD_SS_ACT_CONTINUE)
  {
  MMOC_MSG_HIGH_1( "GEN_CMD: ss_act %d", ss_act);
  }*/
  
  return ss_act;

} /* mmoc_proc_prot_gen_cmd() */

/*===========================================================================
=============================================================================
=============================================================================
============================== MMGSDI -> MMOC functional interface ==========
=============================================================================
=============================================================================
===========================================================================*/

/* The following section defines the API when MMOC-MMGSDI has to communicate
** to MMoC using the functional interface.
*/


/*===========================================================================

FUNCTION mmoc_is_pref_reas_considered_non_dup_pref_sys_cmd

DESCRIPTION
  This function will look for the sd_ss_pref_reas_e_type pref_reas, which shall
  be ignored for Duplicate Pref Sys Cmds removal from the MMOC queue.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If Cmds has to be ignored for deletion.
  FASLE: Pref reason for Cmd is entitled for deletion.

SIDE EFFECTS
  None
===========================================================================*/

boolean mmoc_is_pref_reas_considered_non_dup_pref_sys_cmd( sd_ss_pref_reas_e_type pref_reas)
{
   boolean ret_val = FALSE;
   MMOC_ASSERT_ENUM_IS_INRANGE(pref_reas, SD_SS_PREF_REAS_MAX);
   
   switch(pref_reas)
   {
	 case SD_SS_PREF_REAS_AVOID_SYS:
         case SD_SS_PREF_REAS_ORIG_START_CS:
         case SD_SS_PREF_REAS_ORIG_START_PS:
         case SD_SS_PREF_REAS_USER_RESEL:
	 ret_val = TRUE;
	 break;
	 
	 default:
	 ret_val = FALSE;
	 break;   
   }
   
   return ret_val;
}

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_remove_dup_pref_sys_cmds

DESCRIPTION
  This function will detect duplicate preferred system changed commands from
  CM and remove them from the queue. It will retain the last preferred system
  changed command.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
/*lint -esym(715,mmoc_info_ptr)*/
LOCALF void                            mmoc_remove_dup_pref_sys_cmds
(


       const mmoc_state_info_s_type          *mmoc_info_ptr
          /* Pointer to MMoC's state information.
          */
)
{
  /* Pointer to MMoC's task information.
  */
  mmoc_task_info_s_type          *mmoc_task_ptr = mmoc_get_task_info_ptr();
  mmoc_cmd_msg_s_type            *next_cmd_ptr =
                   (mmoc_cmd_msg_s_type *)q_check( &mmoc_task_ptr->cmd_q );
  mmoc_cmd_msg_s_type            *pref_sys_cmd_ptr = NULL;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Search for first pref_sys_chgd command in the queue.
  */
  while ( next_cmd_ptr != NULL &&
          ( next_cmd_ptr->cmd.name != MMOC_CMD_PROT_GEN_CMD ||
            next_cmd_ptr->param.gen_prot_cmd.cmd_type !=
                                            PROT_GEN_CMD_PREF_SYS_CHGD
          )
        )
  {
    next_cmd_ptr = (mmoc_cmd_msg_s_type *)q_next(
                        &mmoc_task_ptr->cmd_q, &next_cmd_ptr->cmd.hdr.link);
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*  Check if pref_sys_chgd cmd was found in the queue.
  */
  if ( next_cmd_ptr == NULL )
  {
    /* None found.
    */
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize for duplicate pref_sys_chgd commands detection.
  */
  pref_sys_cmd_ptr = next_cmd_ptr;
  next_cmd_ptr = (mmoc_cmd_msg_s_type *)
                 q_next(&mmoc_task_ptr->cmd_q, &next_cmd_ptr->cmd.hdr.link);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Search for duplicate pref_sys_chgd command in the queue.
  */
  for ( ;
        next_cmd_ptr != NULL;
        next_cmd_ptr = (mmoc_cmd_msg_s_type *)
                 q_next(&mmoc_task_ptr->cmd_q, &next_cmd_ptr->cmd.hdr.link)
      )
  {

     if ( next_cmd_ptr->cmd.name == pref_sys_cmd_ptr->cmd.name
          &&
          next_cmd_ptr->param.gen_prot_cmd.cmd_type ==
                            pref_sys_cmd_ptr->param.gen_prot_cmd.cmd_type
          &&
          next_cmd_ptr->param.gen_prot_cmd.param.pref_sys_chgd.hybr_pref ==
                            pref_sys_cmd_ptr->param.gen_prot_cmd.param.pref_sys_chgd.hybr_pref
          &&
          !mmoc_is_pref_reas_considered_non_dup_pref_sys_cmd(
			    next_cmd_ptr->param.gen_prot_cmd.param.pref_sys_chgd.pref_reas )
          &&
          !mmoc_is_pref_reas_considered_non_dup_pref_sys_cmd(
		       pref_sys_cmd_ptr->param.gen_prot_cmd.param.pref_sys_chgd.pref_reas )
          &&
          pref_sys_cmd_ptr->param.gen_prot_cmd.param.pref_sys_chgd.addl_action.action
           == next_cmd_ptr->param.gen_prot_cmd.param.pref_sys_chgd.addl_action.action
          &&
          /* Do not remove USER PREF cmds */
          !(pref_sys_cmd_ptr->param.gen_prot_cmd.param.pref_sys_chgd.pref_reas == SD_SS_PREF_REAS_USER || 
             pref_sys_cmd_ptr->param.gen_prot_cmd.param.pref_sys_chgd.pref_reas == SD_SS_PREF_REAS_USER_RESEL)
           )
     {
        MMOC_MSG_HIGH_0(" Duplicate pref_sys_chgd");

        q_delete( &mmoc_task_ptr->cmd_q, &pref_sys_cmd_ptr->cmd.hdr.link );

        mmoc_mem_free( pref_sys_cmd_ptr );

        pref_sys_cmd_ptr = next_cmd_ptr;

     }

  }

} /* mmoc_remove_dup_pref_sys_cmds() */
/*lint +esym(715,mmoc_info_ptr)*/
#ifdef FEATURE_MMOC_UNIT_TEST
#error code not present
#endif


/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_process_event

DESCRIPTION
  This function will process the received event by calling the transaction
  handlers.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void                            mmoc_process_event
(

  mmoc_state_info_s_type          *mmoc_info_ptr
  /* Pointer to MMoC's state information.
  */
)
{
  mmoc_cmd_msg_s_type  *cmd_ptr = NULL;
  mmoc_rpt_msg_s_type  *rpt_ptr = NULL;

  #ifdef MMODE_ADDITIONAL_DEBUG_INFO
  mmoc_dbg_buf_rpt_s_type	 rpt_to_queue;

  #endif
  
  sd_ss_e_type auto_deact_ss = SD_SS_MAX;
  
  /* Pointer to MMoC's task information.
  */
  mmoc_task_info_s_type          *mmoc_task_ptr = mmoc_get_task_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Process the events.
  */
  while ( ( mmoc_accept_cmds(mmoc_info_ptr ) &&
          ((cmd_ptr = q_get(&mmoc_task_ptr->cmd_q)) != NULL)) ||
          ((rpt_ptr = q_get(&mmoc_task_ptr->rpt_q)) != NULL)
         )
  {
    if ( cmd_ptr != NULL )
    {
      if(cmd_ptr->cmd.name < MMOC_CMD_MAX)
      {
        #ifndef FEATURE_MMOC_LOW_MEM_TARGET
        /* Invoke the transaction handler normally and dequeue the command.
        */
        mmocdbg_print_command_received ( cmd_ptr );
        #endif /* FEATURE_MMOC_LOW_MEM_TARGET */
      }
      else
      {
        MMOC_ERR_1 ("invalid command: %d", cmd_ptr->cmd.name);
      }

      mmoc_call_trans_hndlr(cmd_ptr, NULL, mmoc_info_ptr );

      mmoc_mem_free( cmd_ptr );

      cmd_ptr = NULL;
    }
    else if ( rpt_ptr != NULL )
    {

      if(rpt_ptr->rpt.name < MMOC_RPT_MAX)
      {
        #ifndef FEATURE_MMOC_LOW_MEM_TARGET
        mmocdbg_print_report_received ( rpt_ptr );
        #endif /* FEATURE_MMOC_LOW_MEM_TARGET */
      }
      else
      {
        MMOC_ERR_1 ("invalid report: %d", rpt_ptr->rpt.name);
      }
      #ifdef MMODE_ADDITIONAL_DEBUG_INFO
        rpt_to_queue.prot_state = mmoc_info_ptr->prot_state;
      /* Add the report to current transaction 
      */
      
      if(mmoc_info_ptr->curr_trans != MMOC_TRANS_NULL)
      {
        rpt_to_queue.rpt_name = rpt_ptr->rpt.name;
        rpt_to_queue.task_name = rpt_ptr->rpt.task_name;
        
        mmoc_dbg_buf_add_message(mmoc_info_ptr, &(rpt_to_queue), curr_rpt_idx);

        curr_rpt_idx++;
        /* if we get PROT_AUTO_DEACT_IND report as part of MMOC_TRANS_PROT_AUTO_DEACTD_IND,
        ** we should add report here as part of ongoing trans and not start new trans.
        ** Hence, we set rpt_added flag to TRUE here so it would not enter next if cond.
        ** and start new trans as a result of PROT_AUTO_DEACT_IND report.
        */

      } 
      #endif /* MMODE_ADDITIONAL_DEBUG_INFO */

      mmoc_call_trans_hndlr(NULL, rpt_ptr, mmoc_info_ptr );
      /* Free memory for a report only if is_resued is FALSE, its not 
      ** buffered for resue 
      */
      MMOC_MSG_LOW_1("rpt_ptr->rpt.is_reused:%d",rpt_ptr->rpt.is_reused);
      if(!rpt_ptr->rpt.is_reused)
      {
        mmoc_mem_free( rpt_ptr );
        rpt_ptr = NULL;
      }
    }
    /*At end of each transaction, scan buffered_auto_deact[] array. Process
    ** buffered auto deact reports for each stack one by one
    */
    if(mmoc_info_ptr->curr_trans == MMOC_TRANS_NULL && 
      (auto_deact_ss = mmoc_is_auto_deact_ind_rpt_buf(mmoc_info_ptr)) != SD_SS_MAX)
    {
      MMOC_MSG_HIGH_1("Process buffered auto_deact on ss:%d", auto_deact_ss);
      mmoc_info_ptr->buffered_auto_deact->rpt.is_reused = FALSE;
      rpt_ptr = mmoc_info_ptr->buffered_auto_deact;
	  
      mmoc_info_ptr->buffered_auto_deact = NULL;
	  	
      if(rpt_ptr->rpt.name < MMOC_RPT_MAX)
      {
        #ifndef FEATURE_MMOC_LOW_MEM_TARGET
        mmocdbg_print_report_received ( rpt_ptr );
        #endif /* FEATURE_MMOC_LOW_MEM_TARGET */
      }
      else
      {
        MMOC_ERR_1 ("invalid report: %d", rpt_ptr->rpt.name);
      }
      /* If stack having  buffered auto deact indication already has prot state as NULL,
      ** do not process the auto_deact report, as prot must have already been deactivated
      */
      if(mmoc_info_ptr->prot_state != PROT_STATE_NULL )
      {
        mmoc_call_trans_hndlr(NULL, rpt_ptr, mmoc_info_ptr );
      }
      else
      {
        MMOC_MSG_HIGH_1("Do not process as prot_state for %d ",auto_deact_ss);
      }
	 
      MMOC_MSG_LOW_1("rpt_ptr->rpt.is_reused:%d",rpt_ptr->rpt.is_reused);
      if(!rpt_ptr->rpt.is_reused)
      {
        mmoc_mem_free( rpt_ptr );
        rpt_ptr = NULL;
      }

    }
	

    /* After mmoc processes each event, report back to the watchdog.  This is to avoid
    ** dog timeout when mmoc doing back to back event processing without exiting the while
    ** loop.
    */

  } /* while (  mmoc_accept_cmds() ... ) */

  return;

} /* mmoc_process_event() */

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_init

DESCRIPTION
  This function initializes mmoc state

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/

void mmoc_init()
{

  rex_sigs_type                   mask = MMOC_CMD_Q_SIG       |
                                         MMOC_RPT_Q_SIG       |
                                         MMOC_DUP_CMD_REM_SIG |
                                         #ifdef FEATURE_MMODE_QTF
                                         #error code not present
#endif /* FEATURE_MMODE_QTF */
                                         MMOC_SANITY_TIMER_SIG;

  /* Pointer to MMoC's task information.
  */
  mmoc_task_info_s_type          *mmoc_task_ptr = mmoc_get_task_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Perform MMoC task initialization.
  */
  mmoc_task_init();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize the DB state to Powerup.
  */
  mmoc_update_db_dmss_state( &mmoc_state_info );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Send the first watchdog report and set the timer for report interval
  ** and start the MMoC sanity timer.
  */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize System Selection for Main & HDR timer */

  /* Defining this timer in mmoc_timer_group guarantees it will be called
  ** even when clocks are disabled.  This is necessary when this timer
  ** is used during deep sleep, since it _is_ the event that enables
  ** clocks again.
  */
  timer_def2( &mmoc_task_ptr->ss_main_timer, &mmoc_timer_group );


  /* MMOC initialiation after task is started */
  mmoc_init_after_task_start(&mmoc_state_info);






}

/* <EJECT> */
/*===========================================================================

FUNCTION mmoc_handle_sigs

DESCRIPTION
  This function is the entrance for the MMoC task. This is an infinite loop
  outside of the state machine which handles the input signals.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,ignored)*/
void                           mmoc_handle_sigs(rex_sigs_type sigs)
{

  /* Pointer to MMoC's task information.
  */
  mmoc_task_info_s_type          *mmoc_task_ptr = mmoc_get_task_info_ptr();
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check if dup. command removal sig is set.
    */
    if (sigs & MMOC_DUP_CMD_REM_SIG)
    {
      /* Clear the rex sigs associated with sanity timer.
      */
      (void)rex_clr_sigs( cm_tcb_ptr, MMOC_DUP_CMD_REM_SIG );

      /* Remove duplicate pref_sys commands for SD_SS_MAIN.
      */
      mmoc_remove_dup_pref_sys_cmds( &mmoc_state_info );

    } /* if ( sigs & MMOC_DUP_CMD_REM_SIG ) */


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Check if it was an event.
    */
    if ( sigs & MMOC_CMD_Q_SIG || sigs & MMOC_RPT_Q_SIG )
    {
       /* Clear the rex sigs associated with cmd & rpt.
       */
       (void)rex_clr_sigs( cm_tcb_ptr, MMOC_CMD_Q_SIG );
       (void)rex_clr_sigs( cm_tcb_ptr, MMOC_RPT_Q_SIG );

       /* Process the received event( command or report ).
       */
       mmoc_process_event( &mmoc_state_info );


       /* Check if MMoC can accept commands at this time.
       */
       if ( mmoc_accept_cmds( &mmoc_state_info ) )
       {
         /* Enable REX signal mask for CMD q.
         */
         sigs |= MMOC_CMD_Q_SIG;
       }
       else
       {
         /* Disable REX signal mask for CMD Q.
         */
         sigs &= ~MMOC_CMD_Q_SIG;
       }


    } /* if ( sigs & MMOC_CMD_Q_SIG || sigs & MMOC_RPT_Q_SIG) */


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Check if Sanity timer expired.
    */
    if (sigs & MMOC_SANITY_TIMER_SIG)
    {
      /* Clear the rex sigs associated with sanity timer.
      */
      (void)rex_clr_sigs( cm_tcb_ptr, MMOC_SANITY_TIMER_SIG );

      /* Sanity timer expired.
      */
      mmoc_sanity_timer_exp( &mmoc_state_info );

    } /* if ( sigs & MMOC_SANITY_TIMER_SIG ) */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    #ifdef FEATURE_MMODE_QTF
    #error code not present
#endif /* FEATURE_MMODE_QTF */

} /* mmoc_task() */
/*lint +esym(715,ignored)*/

/*===========================================================================
FUNCTION MMOC_INIT_AFTER_TASK_START

DESCRIPTION
  Initilize MMOC after task is started

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
/*lint -esym(715,mmoc_state_info_ptr)*/
/*lint -esym(715,mmgsdi_info_ptr)*/
/*lint -esym(529,mmgsdi_info_ptr)*/
/*lint -esym(818,mmoc_state_info_ptr)*/
void mmoc_init_after_task_start
(
     mmoc_state_info_s_type   *mmoc_state_info_ptr
        /* Pointer to MMoC's state information.
        */
)
{
  return;
}  /* mmoc_init_after_task_start() */


#ifdef FEATURE_MMOC_UNIT_TEST
#error code not present
#endif
#ifdef CM_DEBUG
#error code not present
#endif
