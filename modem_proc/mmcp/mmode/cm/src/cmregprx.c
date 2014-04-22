/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
                        C M    R E G P R O X Y

                 I M P L E M E N T A T I O N    F I L E

GENERAL DESCRIPTION
  This file is the main implementation for the RegProxy. The purpose of
  RegProxy is to act as a proxy to interface with the System determination
  for the events from REG and also interpreting the SD action for REG.
  This way, there would be no impact on CM <=> REG interface for multi-mode
  operation.

EXTERNALIZED FUNCTIONS

  Command Interface:
    cmregprx_cmd_gwl_activate_protocol
    cmregprx_cmd_gwl_deactivate_protocol
    cmregprx_cmd_gwl_generic_cmd
    cmregprx_cmd_gwl_ph_stat_chgd


  Report Interface:
    cmregprx_rpt_proc

  Functional Interface:
    cmregprx_proc_gw_resel_ok
    cmregprx_proc_gw_resel_not_ok
    cmregprx_cmd_proc
    cmregprx_init
    cmregprx_sanity_timeout

REGIONAL FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The cmregprx_init() should be called before any APIs are called.


Copyright (c) 2002 - 2013 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/cm/src/cmregprx.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/31/14   jvo     Refactored message macros
01/24/14   rm      Adding debug buffers
11/14/13   ss      Accept NO SRV stat ind from NAS during Merge state. Update
                   srv req from CMMSC if SD out of sync with REGPRX
11/14/13   ss      Update UE mode switch flag
10/07/13   jvo     Replace FEATURE_UICC_RAT_INDICATOR_SUPPORT with
                   is_rat_bal flag
08/13/13   cl      CM supports explicit PS_DETACH when capability from PM changed
07/29/13   xs      CM notify NAS when SIM becomes not available
07/29/13   cl      CM merging CS+PS and handle LIMITED srv on STAT_IND
07/22/13   fj      Add support for DSDX L+G
07/21/13   skk     FR2921: Network initiated Authentication Procedure Support
                   during emergency call When SIM is perso Locked
07/19/13   jvo     Remove all MDM/MSM Fusion features
07/17/13   xs      Lu reject info notification
07/17/13   xs      Take ext IMS emerg into consider of emergnecy call active checking
07/10/13   ar      Checking Reprt pointer is NULL or NOT NULL
07/08/13   cl      Update client about PS Domain Transfer when receives SRV_CNF
07/05/13   cl      Clear LTE CS Domain info when in SGLTE-Home mode.
06/24/13   cl      Adding SGLTE support, fixing sending multiple UE_MODE
06/18/13   gm      FR 2617 - OMADM IMS and ADMIN PDN Disable Requirement
05/01/13   sb      In function cmregprx_map_srv_ind_to_sd_ind(), on getting a service indication
                   with FULL SERVICE, change reg proxy substate to NULL.
04/23/13   xs      FR3273: handling RPLMN
04/05/13   gm      Send SIM refresh even when MAIN stack in NAS is dormant
02/03/13   cl      FR1975: Dynamic Switch SVLTE <-> CSFB
01/25/13   gm      FR 3059 - ONLINE optimization - skip duplicate subscription
12/21/12   xs      reset target RAT info to None when LTE acquired successfully
12/11/12   aj      update emerg attached state to SD
11/30/12   xs      Don't activate HDR tunnel upon resel fail
10/31/12   mg      Fix crash on receiving LTE resel report immediately after DO call end
10/15/12   aj      restore LTE based on top of stack and not ph object
09/18/12   qf      Remove the EOOS feature limitation for NAS scan status
07/26/12   aj/mg   selectively process suitable search end indication
07/27/12   jh      Not alter stop reason to ALT_SCAN if gwl mode is not avail.
06/05/12   br      CMREGPRX should immediately return CNF to MMOC without issuing STOP MODE REQ
                   if one stack is active and the deact reason is DEACT FROM DORMANT.
05/09/12   xs      Use ss in sending PS detach instead of MAIN
05/08/12   cl      Use correct SD API for acq-fail
04/30/12   xs      Add support of sending FPLMN list
04/04/12   sg      Add support for new mode pref TDS_LTE
03/22/12   sg      Enable WRLF functionality for TD-SCDMA RAT
03/19/12   ak/gm   Fix ETM to wait for detach
03/19/12   xs      Clear reject cause when get service indication
03/05/12   xs      Added support of avoid unnecessary attach
02/29/12   rk      Adding support for deativating dormant protocol through MMOC
02/24/12   rm      Update SD for protocol deactivate before stop mode req
11/30/11   jh      Added feature for dynamic rat acq order change
01/31/12   vk      Reset lte_disable_uptime during mode change to any but LTE.
01/24/12   vk      Changes for 1XCSFB RLF handling.
01/17/12   aj      SC SVLTE SO 33 suspend changes
01/12/12   sg      Made changes for TGWL configuration.
12/22/11   gm      New power save reason for Hybr BSR reselection
12/21/11   xs      Fix compilation error when FEATURE_LTE is not defined
12/16/11   vk      Reset domain selection variables while performing PLMN blocking
11/16/11   jh      Replace cmcall CTXT_TRANSFER event to cmss event
11/04/11   jh      Send REFRESH_OK when deactivate gw is not neccessary
11/02/11   ak      MSM to not return scan perm if GW pending.
11/02/11   ak      Fix issues discovered in MMSS arbitration.
10/31/11   rk      Use CMSS info instead of SD info for Domain selection algorithm
09/29/11   xs      Fix HDR tunnel early activate issue
09/28/11   vk      Multimode CSFB activation changes
09/15/11   vk      New MMOC Transaction "MMOC_TRANS_DEACT_1XCSFB_PROT_CMD" related changes.
09/15/11   rk      Remove the check for GWL subsc for stop mode reason during
                   ALT scan
03/15/11   skp     Adding extra parameters in sd_ss_ind_gw_acq_plmn_camped().
09/13/11   rm      While doing reslection, if call has been end force the pref's
                   right away and not wait for 4 seconds
08/28/11   vk      changes for LTE connected mode Reset procedure.
08/25/11   xs      Added EPS network feature support definition (PS voice indication)
07/25/11   cl      Remove cm_mm_grant_scan_perm_if_pending() functionality
07/24/11   aj      SC SVLTE 1.0 merge to mainline
07/12/11   xs      HDR optimize handoff enhancement
07/05/11   rn      Terminate Get Net only when reqd and abort seach at LL too
07/05/11   rk      Support for sending voice domain preference to NAS
06/14/11   fj      Add support to suspend and resume IDM timer.
06/14/11   xs      HDR optmize handoff: activate HDR tunnel when attach complete
06/14/11   aj      add support for sd_misc_is_lte_full_srv_allowed()
06/08/11   gm      Send ALT_SCAN as stop reason to NAS while in LTE->HDR BSR
05/23/11   xs      Reset target RAT to None when active RAT is not LTE
05/20/11   xs      Add support for LTE->eHRPD optimized handoff
05/19/11   rm      If stop mode req is due to LPM, reset the srv req counter
05/16/11   xs      Added another stop mode rquest to deactivate NAS in suspend state
05/11/11   skp     Lint Issue Fixed.
04/21/11   sv      Correct Lint Errors
04/18/11   xs      Reset target RAT in stop mode cnf handler
04/18/11   xs      Restore LTE during DO->LTE if it was removed before
04/18/11   sg/rm   If "deact_reason == PROT_DEACT_LPM", MMOC to send GW protocol
                   deact even in PWR_SAVE mode.
04/14/11   xs      Added support for target_rat notification
04/12/11   fj      Add support for PLMN blocking.
04/12/11   rk      Changing the featurization
04/11/11   am      Cleaning stubs and runnning qtf SUTE
03/30/11   rk      Fix to do service aqusition after  PS detach
03/29/11   rk      Fix for not doing PPCSFB after receiving AttachAccept with
                   SMSOnly set
03/24/11   cl      FUSION2 MMSS-Arbitration centralization rework
03/16/11   aj      send pseudo camped IND during DO to LTE reselection
03/15/11   cl      Allow MDM to grant permission to MSM when permission is pending
03/09/11   cl      Add support for SVLTE2 MMSS arbitration
03/02/11   rk      Domain selection phase 2 requirements
02/28/11   gm      Add thermal emergency support
02/28/11   aj      Updates to DO->LTE reselection
02/25/11   sn      Support for graceful pdp deactivation before detach.
02/22/11   rk      SMS only registation
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/14/11   rm      LTE RAT-B changes
02/10/11   am      Removing prototypes which are not needed by QTF
02/04/11   skp     Correcting typo mistake: using %d in debug statement
02/03/11   sg      While deactivating prot, terminate pending get net cmd if any.
01/31/11   sv      Send MMOC Auto-deact rpt with correct SS
01/12/11   fj      Add new indication for call object transfer during IRAT.
01/04/11   rm      CMI-4: Removing #ifdef's
12/23/10   sv      Integrate DSDS feature
12/17/10   rk      Deativate 1XCP from CSFB when LTE is deativated
12/16/10   ay      Handling of back to back LTE RLFs
12/10/10   rk      Changing the featurization for 1XCSFB
12/03/10   gm      Fix to compiler warning.
11/30/10   sv      Replace TG==T_PC with CM_DEBUG
11/16/10   aj      Suppress deact report to MMOC when regprx is not active
11/15/10   rm      Memory optimization changes
11/08/10   aj      Adding cmi.h
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
11/05/10   aj      DO->LTE reselection code
10/27/10   rk      Adding support for deativating dormant protocol through MMOC
10/05/10   rk      Merging 1XCSFB pre-registration code
10/01/10   xs      Superlint critical/high error fixed
09/30/10   fj      Put SYS_MODE_PREF_LTE_RADIO_ACCESS_ONLY under FEATURE_LTE.
09/29/10   fj      Add GWL and LTE mode_pref mapping in
                   cmregprx_map_sd_mode_pref_to_sys().
09/20/10   aj      sim_avail/sim_not_avail sent only if subsc state has toggled
09/03/10   jqi     SM ABORT is only called in the context of GSM/UMTS PS call.
08/19/10   aj      support for LTE detach when mode pref does not incl LTE
08/11/10   ay      Fixed an EOOS related bug
08/03/10   fj      Featurize LTE to GW call object transfer code.
08/03/10   aj      Send stop mode reason as IRAT when 3gpp2 has been acquired
07/26/10   sg      Go to ALT_SCAN only if GWL subsc is available, and send
                   PH_STAT_chgd to NAS even when in dormant state
07/23/10   sg      CM not to send another STOP_MODE_REQ to NAS, if it already sent,
                   and CM waiting for confirmation from NAS.
07/06/10   gm      Fix for Klocwork deferred critical errors.
06/28/10   ay      Added EOOS
06/24/10   rn      Update RAT priority list in NETWORK_LIST_REQ in MEAS_BACK
06/23/10   rk      Set cs_for_request to TRUE  on emergency call orig.
06/15/10   fj      Add support for LTE APN timer expire - reattach requirement.
06/03/10   aj      deact dormant protocol only if more pref acq is complete
05/19/10   rn      Send Rat priority list in Network_list_req
04/29/10   sv      Add NAS stubs for QTF
04/14/10   rn      Removed lte_band_pref from reg_service_req interface
03/24/10   fj      Added support for LAC/TAC/RAC in ss event.
03/23/10   rn      Replaced ue_capability with rat_pri_list
03/22/10   sg      Updated trace info for band_pref sent in CM_SERVICE_REQ
01/27/10   fj      Added stub for WCDMA/LTE builds.
01/08/10   ay      Added support for LTE Connected mode
01/08/10   rn      LTE-eHRPD redirection support
01/05/10   pm      Removing UOOS & sd_ss_ind_gw_opr_srv_lost2()
12/16/09   np      Workaround for MDM9k for GWL mode to set ACQ order with
                   LTE radio access only unitl NAS checks in the their code
11/17/09   rn      Add UE_CAPABILITY in Service_Req
11/13/09   fj      Change cmregprx_map_prot_oprt_mode_to_sys() and
                   cmregprx_map_deact_to_stop_reason() to EXTERN APIs.
11/05/09   fj      Report to SD based on active_rat instead of featurization.
12/22/08   fj      Enabled APIs for LTE.
09/01/09   sv      Moving from clk API to time API
04/24/09   rm      Including cmlog_v.h
02/25/09   sv      Correct Lint errors
02/23/09   rm      Removing cm_log_v.h
02/20/09   rm      Lint clean post CMI changes
12/17/08   jd      Fixed Lint errors
09/09/08   cl      Unconditionally send CM_SIM_AVAILABLE or
                   CM_SIM_UNAVAILABLE when there is a subscription change.
10/01/08   sv      Featurizing cmregprx_map_sd_mode_pref_to_sys to remove
                   compiler warning
09/18/08   rm      Adding acq_order_pref in sd_ss_act_get_net_gw_s_type.
                   Have to consider acq_order_pref when responding to get_networks
09/17/08   ks      When OOS in CELL_FACH, UE needs to follow UOOS scanning
                   pattern to re-acquire service
09/16/08   pk      Added support for WLAN UOoS
09/10/08   ak      Fixed scenario of data call drop during GSM scan
07/30/08   ak      Fix for the HICPS scenario.
06/18/08   sn      Adding Managed Roaming changes
04/26/08   rn      To send deact_cnf to mmoc in dormant state
04/24/08   rm      Removing feature flag FEATURE_GW_DORMANT_DURING_ALT_SCAN
04/24/08   rm      Removing feature flag FEATURE_CM_NAS_PH_STAT_CHGD
04/24/08   rm      Removing feature flag FEATURE_CM_NAS_SERVICE_REQ_TYPE
03/19/08   rm      Corrected featurisation of FEATURE_CM_NAS_SERVICE_REQ_TYPE
03/19/08   rn      Added feature FEATURE_CM_NAS_PH_STAT_CHGD
03/14/08   rm      CM-NAS interface for new service req type
03/13/08   sk      Fix to allow CDMA scans during dormant GW PS data calls
                   without tearing down the PS data call.
02/15/08   rn      Added support for new command CM_PH_STATUS_CHGD_REQ
02/11/08   ic      Fixed compiler warnings
09/12/07   pk/rm   Merge to mainline - Adding support for handling calls in origination state
                   while service is lost
08/07/07   ic      Lint clean up
05/03/07   ic      Fixed compile errors on GSM-only builds (with respect to
                   FEATURE_UMTS_UNIFORM_OOS_HANDLING_PWR_UP_ENH)
06/19/07   ic/cl   Merge CL# 468160
06/15/07   rp      Adding support for
                   FEATURE_CONNECTED_MODE_UMTS_UNIFORM_OOS_HANDLING_UI_UPDATE
06/07/07   pk      UOOS enhancements
05/08/07   rp/cl   Passing is_resel_allowed as TRUE after background
                   service search is done.
04/14/06   ka/rp   Removed filtering of SERVICE_REQUESTS when the difference
                   is only in FOR fields.
11/30/06   rp      Added support for background PLMN search request.
11/16/06   pk      Lint Fixes
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
08/25/06   pk      Added support for sd_ss_ind_gw_acq_plmn_failed2
08/18/06   pk      Added support for GW UOOS
07/18/06   ka      Sending SYS_STOP_MODE_REASON_LPM for prot deact due to LPM.
06/12/06   jqi     Lint fix.
06/09/06   jqi     Only cleared the GW network list pending flag if SD stops
                   manual list search.
06/05/06   ka/ic   Sending SYS_STOP_MODE_REASON_POWER_OFF for prot deact due
                   to LPM.
04/01/06   ka      Adding sys to reg mapping function for net_list_cnf status.
01/26/06   ka      Adding cmregprxt_nam_sel_offline_sends_no_sim_avail_req()
                   Supporting success/abort result with network list cnf for
                   get networks request
01/12/06   ka      Correcting name of a field in CC, SS abort request.
01/12/06   ka      Sending abort reason along with SS, CC abort on
                   STOP_MODE_REQ.
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup
12/02/05   ka      Changes to handle new data suspend primitive.
11/30/05   ka      Adding support for HPLMN SRCH REQ and HPLMN SRCH CNF.
11/14/05   ic      Added missing CMREGPRX_ASSERT().
                   Added 613 Lint inhibit comments.
                   Lint80 clean up.
11/08/05   sk      Separated CM reports and commands.
11/01/05   ka      Clearing mmoc_trans_id when stop_mode_req is sent due to
                   either a default sd action or a PWR_SAVE sd action.
10/23/05   jqi     Hold ps data call orig or data avail request if there is
                   a pending get network list request.
08/02/05   ic      Print value that caused error in default switch case
07/15/05   ic      Featurization changes to support CDMA + GSM build flavor
06/16/05   ka      Sending SMREG_PDP_ABORT_REQ to SM during stop_mode_request
06/02/05   ic      Mainlined FEATURE_EXTENDED_PRL and
                   FEATURE_LIMITED_MANUAL_ACQ
05/18/05   ka      Adding feature for Limited manual acquistion.
03/18/05   ka      Correcting condition to always check for
                   PROT_PH_STAT_CHG_SUBSC before sending sim avail request.
03/03/05   ka      Adding support to let mmoc know of protocol that has a
                   subs change when calling mmoc_cmd_subscription_chg
01/07/05   ic      Mainlined FEATURE_MANUAL_HPLMN_SELECTION
11/15/04   ic      Lint cleanup with CM_DEBUG on
10/21/04   sj      Changed throttling mechanism for CM_SERVICE_CNF.
                   Removed handling of unused sanity timer mechanism.
08/10/04   sj      Fixed issue with not reseting last_service_req buffer
                   during NETWORK_LIST_REQ.
08/04/04   ic      Removed enabling/disabling of Lint error 788 as it is
                   globally suppressed in Lint config files.
                   Enabling it locally in CM files caused inconsistent
                   Lint issues reports when a file was linted separately
                   versus when it was linted as part of *.c batch
08/04/04   ic      CM Lint clean up - cmregprx.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/30/04   sj      Added support for camped_ind.
07/26/04   ic      Added cmregprx_dummy() to get rid of ADS 1.2 compiler
                   warning
07/05/04   sj      Added msg_high for manual PLMN in service_req.
06/28/04   sj      Lint cleanup.
05/21/04   ka      Code for handling CM_CAMPED_IND from REG.
                   New fields being populated for both cs and ps follow on requests
                   to denote origination pending during service request.
04/20/04   jqi     Added debugging message for srv_req, act_req and
                   get_net_work.
03/31/04   sj      Fixed lockup issue where srv. cnf is buffered if hold_uptime
                   has expired.
03/17/04   ka      Merge from 6250. Added Support for Follow on request
                   Fixed Lint errors.
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
01/27/04   SJ      Changed stop_mode reason for FTM.
01/13/04   ka      Merged changes from MSM6200
12/10/03   jqi     Set user_select_mode to false for power up user orig mode.
                   Replaced FEATURE_QUAD_BAND_SUPPORT with
                   FEATURE_GSM_BAND_PREF and FEATURE_MANUAL_HPLMN_SELECTION.
11/03/03   ws      Fixed LTK compile problems.
10/30/03   prk     Added support for new limited_regional service.
10/23/03   ws      Added jqi's changes for band preference enum.
10/21/03   RI      Fixed merges from MSM6200.
10/15/03   SJ      Added support for FTM deactivate reason.
08/21/03   SJ      Added support to distinguish entering power save due to
                   no service or no systems to acquire.
08/20/03   SJ      Added srv cnf hold period, if srv cnf comes back within
                   1 second.
                   Added support for moving phone to power save after
                   early power scan.
07/28/03   SJ      Support for circulating data suspend flag thru' SD.
07/10/03   jqi     Added Extended PRL support
                   -Passing down band pref to GSM/UMTS stack.
                   -Populate LAC, band and chan from GSM/UMTS stack.
04/28/03   AT      Lint cleanup.
04/15/03   prk     Replaced CMREGPRX_MSG_ERROR with CMREGPRX_MSG_HIGH
03/21/03   ATM     Added event logging for svc_cnf and nw_list_cnf
03/19/03   sj      Added pwr_save support.
03/18/03   sj      Added support to send _activation_req( early power scan).
02/28/03   prk     Mainlined FEATURE_MULTIMODE_ARCH
02/21/03   sj      Sending tc_stop_mode_req + abort connections before
                   sending stop_mode_req to REG.
02/05/03   sj      Modified cmregprx_process_sd_action() to send the correct
                   stop mode reason to REG.
01/21/03   ws      Updated copyright information for 2003
10/18/02   vt      Compiles for GSM/WCDMA only.
04/22/02   SJ      Initial release.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(714,cmregprx_dummy)
**     714: Symbol 'cmregprx_dummy(unsigned short)' not referenced
**     Need the function or we get compiler warnings about empty file on 1X
**     targets
*/

/*lint -esym(766,customer.h) */
#include "mmcp_variation.h"
#include "customer.h" /* Customer configuration file */
#include "comdef.h"   /* Definition for basic types and macros */

#ifdef FEATURE_CM_LTE
#include "cmltecall.h"/* Interface to CM ltecall interface declarations. */
#else
#include "cmcall.h"   /* Interface to CM call object */
#endif

#include "cmss.h"
#include "cmsds.h"

#if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)||defined(FEATURE_TDSCDMA))
#include "msg.h"      /* Message output services */
#include "err.h"      /* Error and error fatal services */
#include "cmd.h"      /* Command header for inter-task communication */
#include "rex.h"      /* Rex related services */

#include "sd.h"       /* Interface for System Determination services */
#include "sd_v.h"
#include "prot.h"     /* Common header for mmoc <-> protocol communications*/
#include "prot_v.h"
#include "mmoc.h"     /* External interface definitions for MMoC */
#include "mmoc_v.h"
#include "task.h"     /* Interface for global task resources */
#include "cmwll.h"    /* REG <=> CM interface declarations */
#include "cmregprx.h" /* RegProxy interface declarations. */
#include "cmregprxi.h"/* Regproxy implementation related declarations. */
#include "cmlog.h"    /* For logging CM events */
#include "cmlog_v.h"
#include "cmdbg.h"    /* For using ASSERTs, etc. */
#include "cmph.h"     /* Interface for CM phone object*/
#include "cmaccessctrl.h"
#include "sys.h"      /* For using phone status chgd enums */
#include "sys_v.h"
#include "sys_plmn_selection.h"

#include "cmtask.h"
#include "cmtaski.h"
#include <limits.h>

#ifdef CM_DEBUG
#error code not present
#endif

#include "mm.h"

#ifdef CM_DEBUG
#error code not present
#endif

#if defined(FEATURE_LTE_TO_1X)
#include "cmcsfbcall.h"
#endif

#ifdef FEATURE_HDR_HANDOFF
#include "cmssidmi.h"
#endif

#ifdef FEATURE_CM_MMGSDI_PROCEED_WITH_REFRESH
#include "cmmmgsdi.h"
#endif
#if defined(FEATURE_HDR)
#include "hdrcp_msg.h"
#endif
#ifdef FEATURE_CM_LTE
#include "cm_msgr_msg.h"
#endif

#include "cmemgext.h"

#include "cmmsc.h"
#include "cmipappi.h"
#include "policyman_cm.h"
#include "cm_msgr_msg.h"
#include "cmpmprx.h"
#include "cmwcall.h"

/*===========================================================================
=============================================================================
=============================================================================
============================== REQUIRED FEATURE DEFINES =====================
=============================================================================
=============================================================================
===========================================================================*/


/* Feature SD20 must be defined for Multi-mode operation.
*/
#ifndef FEATURE_SD20
#error FEATURE_SD20 must be defined.
#endif



/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* Definition for RegProxy's info.
*/
static  cmregprx_info_s_type           cmregprx_info;
static  cmregprx_sim_state_update_s_type  cmregprx_sim_state_update;

#ifdef MMODE_ADDITIONAL_DEBUG_INFO
/* Instance for cmregprx debug buffer
*/
  static cmregprx_debug_buffer_s_type cmregprx_debug_buffer;
#endif

#define IS_PH_STATUS_CHG_OPRT_MODE(mode) ( \
        (PROT_OPRT_MODE_OFFLINE == (mode) || \
         PROT_OPRT_MODE_OFFLINE_CDMA == (mode) || \
         PROT_OPRT_MODE_LPM == (mode) || \
         PROT_OPRT_MODE_RESET == (mode) || \
         PROT_OPRT_MODE_FTM == (mode) || \
         PROT_OPRT_MODE_PWROFF == (mode) \
         ) ? TRUE : FALSE \
        )

/*===========================================================================
=============================================================================
=============================================================================
==================== REGPROXY IMPLEMENTATION RELATED HELPER FUNCTIONS =======
=============================================================================
=============================================================================
===========================================================================*/

//Forward declaration
static  void                           cmregprx_send_service_req_permission
(
       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */
);

static void                            cmregprx_send_mmoc_deactd_rpt
(

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

);

/*===========================================================================

FUNCTION cmregprx_reset_sim_state_update_info

DESCRIPTION
  Reset the cmregprx_sim_state_update state info on GW voice call end flag
  after handling SIM_STATE_UPDATE_IND.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmregprx_reset_sim_state_update_info( void )
{
  memset(&cmregprx_sim_state_update, 0, sizeof(cmregprx_sim_state_update_s_type));
}

/*===========================================================================

FUNCTION cmregprx_get_sim_state_update_info

DESCRIPTION

  Return cmregprx_sim_state_update state.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cmregprx_sim_state_update_s_type* cmregprx_get_sim_state_update_info( void )
{
  return &cmregprx_sim_state_update;
}

/*===========================================================================

FUNCTION cmregprx_get_active_band()

DESCRIPTION
  This function returns the correct active_band member, based on the RAT.

DEPENDENCIES
  None.

RETURN VALUE
  sys_band_mask_type for the specified RAT,

SIDE EFFECTS
  None.
===========================================================================*/
static sd_band_u_type           cmregprx_get_active_band
(
    sys_radio_access_tech_e_type           active_rat,
       /* Active RAT for the associated active_band
       */

    sys_active_band_u_type                 active_band
       /* Active band union
       */

)
{
  sd_band_u_type local_band;

   switch(active_rat)
   {
      case SYS_RAT_GSM_RADIO_ACCESS:
         local_band.chgwt_band = active_band.active_band;
         break;

      case SYS_RAT_LTE_RADIO_ACCESS:
         local_band.lte_band = active_band.lte_active_band;
         break;
      default:
         local_band.lte_band = SYS_LTE_BAND_MASK_CONST_NONE;
   }

   return local_band;
}

/*===========================================================================

FUNCTION cmregprx_map_prot_oprt_mode_to_sys

DESCRIPTION
  This function maps the mmoc enum prot_oprt_mode_e_type to sys enum
  sys_oprt_mode_e_type

DEPENDENCIES
  None.

RETURN VALUE
  sys type operating mode status.

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN sys_oprt_mode_e_type           cmregprx_map_prot_oprt_mode_to_sys
(

    prot_oprt_mode_e_type               prot_oprt_mode
       /* Operating mode - prot type.
       */
)
{
  sys_oprt_mode_e_type  sys_oprt_mode = SYS_OPRT_MODE_MAX;

  CM_ASSERT_ENUM_IS_INRANGE( prot_oprt_mode, PROT_OPRT_MODE_MAX );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Switch on the sys_oprt_mode.
  */
  switch ( prot_oprt_mode )
  {
    case PROT_OPRT_MODE_PWROFF:
      sys_oprt_mode = SYS_OPRT_MODE_PWROFF;
      break;

    case PROT_OPRT_MODE_OFFLINE:
      sys_oprt_mode = SYS_OPRT_MODE_OFFLINE;
      break;

    case PROT_OPRT_MODE_OFFLINE_CDMA:
      sys_oprt_mode = SYS_OPRT_MODE_OFFLINE_CDMA;
      break;

    case PROT_OPRT_MODE_ONLINE:
      sys_oprt_mode = SYS_OPRT_MODE_ONLINE;
      break;

    case PROT_OPRT_MODE_FTM:
      sys_oprt_mode = SYS_OPRT_MODE_FTM;
      break;

    case PROT_OPRT_MODE_RESET:
      sys_oprt_mode = SYS_OPRT_MODE_RESET;
      break;


    case PROT_OPRT_MODE_LPM:
      sys_oprt_mode = SYS_OPRT_MODE_LPM;
      break;

    case PROT_OPRT_MODE_PSM:
      sys_oprt_mode = SYS_OPRT_MODE_PSM;
      break;


    default:
      sys_err_fatal_invalid_value_exception((uint8)sys_oprt_mode);
      break;

  } /* switch( prot_oprt_mode ) */


  return sys_oprt_mode;

} /* cmregprx_map_prot_oprt_mode_to_sys() */


/*===========================================================================

FUNCTION cmregprx_get_info_ptr

DESCRIPTION
  This function will fetch the pointer to the RegProxy's info.

DEPENDENCIES
  None.

RETURN VALUE
  cmregprx_info_s_type* - Pointer to Regproxy's info.
  Note! Cannot be NULL

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN cmregprx_info_s_type*           cmregprx_get_info_ptr
(

       void

)
{
  /* Return the pointer to the local definition of Regproxy's info.
  */
  return &cmregprx_info;

} /* cmregprx_get_info_ptr() */


/*===========================================================================

FUNCTION cmregprx_get_stack_info_ptr

DESCRIPTION
  This function will fetch the pointer to the RegProxy's stack info for particular SS

DEPENDENCIES
  None.

RETURN VALUE
  cmregprx_info_s_type* - Pointer to Regproxy's info.
  Note! Cannot be NULL

SIDE EFFECTS
  None.
===========================================================================*/
cmregprx_stack_info_s_type*           cmregprx_get_stack_info_ptr
(

      sd_ss_e_type   ss

)
{
  /* Return the pointer to the local definition of Regproxy's info.
  */
  return &(cmregprx_info.stack_info);

} /* cmregprx_get_stack_info_ptr() */


/*===========================================================================

FUNCTION cmregprx_no_service_event_needed

DESCRIPTION
  This function will return GWL protocol is active or non-active on a particular SS

DEPENDENCIES
  None.

RETURN VALUE
  cmregprx_info_s_type* - Pointer to Regproxy's info.
  Note! Cannot be NULL

SIDE EFFECTS
  None.
===========================================================================*/
 boolean           cmregprx_no_service_event_needed
(

      sd_ss_e_type   ss

)

{
  cmregprx_stack_info_s_type   * cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(ss);
  cmph_s_type                 *ph_ptr        = cmph_ptr();

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );
  CM_ASSERT( ph_ptr != NULL );


  CMREGPRX_MSG_HIGH_2("cmregprx_no_service_event_needed(),state %d, sub state %d",
                    cmregprx_stack_info_ptr->state,
                    cmregprx_stack_info_ptr->substate);

  #if defined(FEATURE_MMODE_GW) && !defined(FEATURE_MMODE_GPRS_LTE)

  if ( ph_ptr->is_rat_bal == TRUE &&
       cmregprx_stack_info_ptr->state == CMREGPRX_STATE_INACTIVE &&
       cmregprx_stack_info_ptr->substate ==CMREGPRX_SUBSTATE_NULL &&
       ph_ptr->is_uim_usim_rat_present == TRUE &&
       ph_ptr->main_stack_info.pref_info.mode_pref == CM_MODE_PREF_GSM_ONLY
     )
  {
    CMREGPRX_MSG_HIGH_0("no service event is needed()");
    /* Already inactive or dormant */
    return TRUE;
  }
  #endif /* CM_GW_SUPPORTED */
 
  CMREGPRX_MSG_HIGH_0("no service event is not needed()");
  return FALSE;


} /* cmregprx_no_service_event_needed */



/*===========================================================================

FUNCTION cmregprx_is_gwl_protocol_active

DESCRIPTION
  This function will return GWL protocol is active or non-active on a particular SS

DEPENDENCIES
  None.

RETURN VALUE
  cmregprx_info_s_type* - Pointer to Regproxy's info.
  Note! Cannot be NULL

SIDE EFFECTS
  None.
===========================================================================*/
 boolean           cmregprx_is_gwl_protocol_active
(

      sd_ss_e_type   ss

)

{
  cmregprx_stack_info_s_type   * cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(ss);

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  if ((cmregprx_stack_info_ptr->state == CMREGPRX_STATE_INACTIVE) ||
      (cmregprx_stack_info_ptr->state == CMREGPRX_STATE_ACTIVE &&
          cmregprx_stack_info_ptr->substate  == CMREGPRX_SUBSTATE_WAIT_STOP_CNF))
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }

} /* cmregprx_is_gwl_protocol_active */


/*===========================================================================

FUNCTION cmregprx_next_trans_id

DESCRIPTION
  This function gets the next valid REG transaction Id. Also, sets the value
  in the Regproxy information.

DEPENDENCIES
  None.

RETURN VALUE
  cmregprx_trans_type - Next REG transaction Id.

SIDE EFFECTS
  None.
===========================================================================*/
static cmregprx_trans_type             cmregprx_next_trans_id
(

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );


  CMREGPRX_MSG_LOW_2("next_trans_id(),Curr state %d,Curr sub state %d",
                    cmregprx_stack_info_ptr->state,
                    cmregprx_stack_info_ptr->substate);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmregprx_get_info_ptr()->unique_trans_id++;

  cmregprx_stack_info_ptr->reg_trans_id =
              cmregprx_get_info_ptr()->unique_trans_id;

  /* Increment the transaction id and return it.
  */
  return cmregprx_stack_info_ptr->reg_trans_id;

} /* cmregprx_next_trans_id() */


/*===========================================================================

FUNCTION cmregprx_update_state

DESCRIPTION
  This function sets the RegProxy's state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_update_state
(

       cmregprx_state_e_type           new_state,
          /* The new state of the RegProxy.
          */

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT_ENUM_IS_INRANGE(new_state, CMREGPRX_STATE_MAX);
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  CMREGPRX_MSG_LOW_3("update_state(), state %d, sub state %d,new state %d",
                    cmregprx_stack_info_ptr->state,
                    cmregprx_stack_info_ptr->substate,
                    new_state);
  cmregprx_stack_info_ptr->state = new_state;


  return;

} /* cmregprx_update_state() */



/*===========================================================================

FUNCTION cmregprx_update_substate

DESCRIPTION
  This function sets the RegProxy's substate, resets the insanity count and
  starts the Sanity timer if substate != NULL.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_update_substate
(

       cmregprx_substate_e_type        new_substate,
          /* The new state of the RegProxy.
          */

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT_ENUM_IS_INRANGE(new_substate, CMREGPRX_SUBSTATE_MAX);
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cmregprx_stack_info_ptr->sys_sel_pref_req_id != CM_DEFAULT_VALUE &&
    new_substate != CMREGPRX_SUBSTATE_WAIT_SRV_CNF )
  {
    /* Update SD with sys_sel_pref_req_id in stack info so that clients get notified aout the serving status */
    sd_ss_ind_misc_update_req_id(SD_SS_MAIN, 
                                    cmregprx_stack_info_ptr->sys_sel_pref_req_id);
    cmregprx_stack_info_ptr->sys_sel_pref_req_id = CM_DEFAULT_VALUE;
  }



  CMREGPRX_MSG_HIGH_3("update_substate(), state %d, sub state %d,new substate %d",
  	                cmregprx_stack_info_ptr->state,
                    cmregprx_stack_info_ptr->substate,
                    new_substate);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Update the substate.
  */
  cmregprx_stack_info_ptr->substate = new_substate;

  return;

} /* cmregprx_update_substate() */


/*===========================================================================

FUNCTION cmregprx_map_reg_scan_status_sd_scan_status

DESCRIPTION
  This function maps Reg scan status to SD scan status

DEPENDENCIES
  None.

RETURN VALUE
  SD scan status

SIDE EFFECTS
  None.
===========================================================================*/

static sd_ss_gwl_scan_status_e_type cmregprx_map_reg_scan_status_sd_scan_status
(
       reg_cm_scan_status_e_type   scan_status
           /* The reg scan status
           */
)
{
  switch( scan_status)
  {
    case REG_CM_SCAN_STATUS_COMPLETE_NO_PLMN:
      return( SD_SS_GWL_SCAN_STATUS_COMPLETE_NO_PLMN );

    case REG_CM_SCAN_STATUS_COMPLETE_PLMN_FOUND:
      return( SD_SS_GWL_SCAN_STATUS_COMPLETE_PLMN_FOUND );

    case REG_CM_SCAN_STATUS_ABORTED:
      return( SD_SS_GWL_SCAN_STATUS_ABORTED );

    case REG_CM_SCAN_STATUS_MAX:
    default:
      return( SD_SS_GWL_SCAN_STATUS_MAX );
  }
}

/*===========================================================================

FUNCTION cmregprx_map_sys_rat_to_sd_mode

DESCRIPTION
  This function maps the RAT to sd_mode_e_type.

DEPENDENCIES
  None.

RETURN VALUE
  SD_MODE_GSM   - mode is GSM.
  SD_MODE_WCDMA - mode is WCDMA.
  SD_MODE_INACT - invalid mode.

SIDE EFFECTS
  None.
===========================================================================*/
sd_mode_e_type                  cmregprx_map_sys_rat_to_sd_mode
(

       sys_radio_access_tech_e_type    rat
          /* RAT type.
          */

)
{
  sd_mode_e_type  sd_mode = SD_MODE_INACT;

  CM_ASSERT_ENUM_IS_INRANGE( rat, SYS_RAT_MAX );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Switch on the RAT.
  */
  switch ( rat )
  {
    case SYS_RAT_GSM_RADIO_ACCESS:
      sd_mode = SD_MODE_GSM;
      break;

    case SYS_RAT_LTE_RADIO_ACCESS:
      sd_mode = SD_MODE_LTE;
      break;

    #if defined(FEATURE_CIOT)
    case SYS_RAT_LTE_M1_RADIO_ACCESS:
      sd_mode = SD_MODE_LTE_M1;
      break;

    case SYS_RAT_LTE_NB1_RADIO_ACCESS:
      sd_mode = SD_MODE_LTE_NB1;
      break;
    #endif

    case SYS_RAT_NONE:
    case SYS_RAT_MAX:
    default:
      break;

  } /* switch ( rat ) */

  return sd_mode;

} /* cmregprx_map_sys_rat_to_sd_mode() */

/*===========================================================================

FUNCTION cmregprx_map_sd_mode_to_sys_rat

DESCRIPTION
  This function maps the sd_mode to rat.

DEPENDENCIES
  None.

RETURN VALUE
  SD_MODE_GSM   - mode is GSM.
  SD_MODE_WCDMA - mode is WCDMA.
  SD_MODE_INACT - invalid mode.

SIDE EFFECTS
  None.
===========================================================================*/
sys_radio_access_tech_e_type           cmregprx_map_sd_mode_to_sys_rat
(

       sd_mode_e_type  sd_mode
          /* RAT type.
          */

)
{
  sys_radio_access_tech_e_type    sys_rat = SYS_RAT_NONE;


  CM_ASSERT_ENUM_IS_INRANGE( sd_mode, SD_MODE_MAX );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Switch on the RAT.
  */
  switch ( sd_mode )
  {
    case SD_MODE_GSM:
      sys_rat =  SYS_RAT_GSM_RADIO_ACCESS;
      break;

    case SD_MODE_LTE:
      sys_rat = SYS_RAT_LTE_RADIO_ACCESS;
      break;

    #if defined(FEATURE_CIOT)
    case SD_MODE_LTE_M1:
      sys_rat =  SYS_RAT_LTE_M1_RADIO_ACCESS;
      break;

    case SD_MODE_LTE_NB1:
      sys_rat = SYS_RAT_LTE_NB1_RADIO_ACCESS;
      break;
    #endif

    default:
      break;

  } /* switch ( rat ) */

  return sys_rat;

} /* cmregprx_map_sys_rat_to_sd_mode() */

/*===========================================================================

FUNCTION cmregprx_map_sd_mode_pref_to_sys

DESCRIPTION
  This function maps the SD mode pref type sd_ss_mode_pref_e_type to
  sys_mode_pref_e_type.

  Note! The acq_order_pref has no bearing on the final sys_mode_pref_e_type.

DEPENDENCIES
  None.

RETURN VALUE
  SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED - acquire either GSM or WCDMA
                                             mode systems.
  SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY      - only GSM mode systems.
  SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY     - only WCDMA mode systems.
  SYS_MODE_PREF_LTE_RADIO_ACCESS_ONLY        - only LTE mode systems.


SIDE EFFECTS
  None.
===========================================================================*/
static sys_mode_pref_e_type            cmregprx_map_sd_mode_pref_to_sys
(

       sd_ss_mode_pref_e_type          gw_mode_pref
          /* GSM/WCDMA mode pref.
          */

)
{
  sys_mode_pref_e_type         sys_mode_pref = SYS_MODE_PREF_MAX;
  CM_ASSERT_ENUM_IS_INRANGE( gw_mode_pref, SD_SS_MODE_PREF_MAX );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Switch the input SD mode preference.
  */
  switch ( gw_mode_pref )
  {
    case SD_SS_MODE_PREF_ANY:
    case SD_SS_MODE_PREF_GW:
    case SD_SS_MODE_PREF_GWL:
          sys_mode_pref = SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED;
          break;

    case SD_SS_MODE_PREF_GSM:
      sys_mode_pref = SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY;
      break;

    #ifdef FEATURE_LTE
    case SD_SS_MODE_PREF_LTE:
      sys_mode_pref = SYS_MODE_PREF_LTE_RADIO_ACCESS_ONLY;
      break;
    #endif

    default:
      sys_mode_pref = SYS_MODE_PREF_NONE;
      break;

  } /* switch ( gw_mode_pref) */


  return sys_mode_pref;

} /*  cmregprx_map_sd_mode_pref_to_sys() */

/*===========================================================================

FUNCTION cmregprx_map_sd_srv_req_type_to_reg_type

DESCRIPTION
  This function maps the SD mode pref type sd_ss_srv_req_type_e_type to
  reg_cm_service_req_e_type.

DEPENDENCIES
  None.

RETURN VALUE
  reg_cm_service_req_e_type

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,type)*/
/*lint -save -e533 */
reg_cm_service_req_e_type     cmregprx_map_sd_srv_req_type_to_reg_type
(
      sd_ss_srv_req_type_e_type  type
)
{
  reg_cm_service_req_e_type       srv_req_type = REG_CM_SERVICE_REQ_NORMAL;
  CM_ASSERT_ENUM_IS_INRANGE( type, SD_SS_SRV_REQ_TYPE_MAX );

  switch( type )
  {
    case SD_SS_SRV_REQ_TYPE_NONE:
      srv_req_type = REG_CM_SERVICE_REQ_NONE;
      break;

    case SD_SS_SRV_REQ_TYPE_SYNC_UP:
      srv_req_type = REG_CM_SERVICE_REQ_SYNC_UP;
      break;

    case SD_SS_SRV_REQ_TYPE_USER_SELECTION:
       srv_req_type = REG_CM_SERVICE_REQ_USER_SELECTION;
       break;

    case SD_SS_SRV_REQ_TYPE_SYSTEM_CHANGE:
      srv_req_type = REG_CM_SERVICE_REQ_SYSTEM_CHANGE;
      break;

    case SD_SS_SRV_REQ_TYPE_REQ_PERIODIC:
      srv_req_type = REG_CM_SERVICE_REQ_PERIODIC;
      break;

    case SD_SS_SRV_REQ_TYPE_NORMAL:
      srv_req_type = REG_CM_SERVICE_REQ_NORMAL;
      break;

    case SD_SS_SRV_REQ_TYPE_NORMAL_RPLMN:
      srv_req_type = REG_CM_SERVICE_REQ_NORMAL_RPLMN;
      break;

    case SD_SS_SRV_REQ_TYPE_BSR:
      srv_req_type = REG_CM_SERVICE_REQ_BETTER_SYSTEM_RESELECTION;
      break;

    case SD_SS_SRV_REQ_TYPE_EF_RAT_REFRESH:
      srv_req_type = REG_CM_SERVICE_REQ_EF_RAT_REFRESH;
      break;

    #if defined (FEATURE_CM_LTE) || defined (FEATURE_WRLF_SYSTEM_SEL)
     case SD_SS_SRV_REQ_TYPE_LTE_SUITABLE_SEARCH:
     case SD_SS_SRV_REQ_TYPE_WCDMA_SUITABLE_SEARCH:
      srv_req_type = REG_CM_SERVICE_REQ_SUITABLE_SEARCH;
      break;
    #endif /* FEATURE_CM_LTE || FEATURE_WRLF_SYSTEM_SEL */

    case SD_SS_SRV_REQ_TYPE_USER_SELECTION_CSG_RPLMN:
      srv_req_type = REG_CM_SERVICE_REQ_USER_SELECTION_CSG_RPLMN;
      break;

    case SD_SS_SRV_REQ_TYPE_CAMPED_PLMN:
      srv_req_type = REG_CM_SERVICE_REQ_CAMPED_PLMN;
      break;

    case SD_SS_SRV_REQ_TYPE_USER_PLMN_RAT:
      srv_req_type = REG_CM_SERVICE_REQ_USER_PREFERRED_PLMN_RAT;
      break;

    case SD_SS_SRV_REQ_TYPE_SRV_MODE_UPDATE:
      srv_req_type = REG_CM_SERVICE_REQ_SRV_MODE_UPDATE;
      break;

    case SD_SS_SRV_REQ_TYPE_CSG_SELECTION_APP:
      srv_req_type = REG_CM_SERVICE_REQ_CSG_SELECTION_APP;
      break;

    case SD_SS_SRV_REQ_TYPE_ROAMING_PLMN:
      srv_req_type = REG_CM_SERVICE_REQ_ROAMING_PLMN;
      break;

  case SD_SS_SRV_REQ_TYPE_LAST_CAMPED_RAT:
    srv_req_type = REG_CM_SERVICE_REQ_LAST_CAMPED_RAT;
    break;

  case SD_SS_SRV_REQ_TYPE_FULL_RAT:
    srv_req_type = REG_CM_SERVICE_REQ_FULL_RAT_MODE;
    break;

    case SD_SS_SRV_REQ_TYPE_MAX:
    default:
      /* All invalid cases, CM does not send to NAS.
      ** Change the type to NORMAL.
      */
      srv_req_type = REG_CM_SERVICE_REQ_NORMAL;
      sys_err_fatal_invalid_value_exception((uint8) srv_req_type);
  }

  return srv_req_type;

}
/*lint +esym(715,type)*/
/*lint -restore */


/*===========================================================================

FUNCTION cmregprx_map_sd_srv_domain_pref_to_sys

DESCRIPTION
  This function maps the SD service domain pref type
  sd_ss_srv_domain_pref_e_type to sys_mode_pref_e_type.

DEPENDENCIES
  None.

RETURN VALUE
  SYS_SRV_DOMAIN_CS_ONLY - Acquire circuit switched service.
  SYS_SRV_DOMAIN_PS_ONLY - Acquire packet switched service.
  SYS_SRV_DOMAIN_CS_PS   - Acquire both circuit & packet switched
                           service.



SIDE EFFECTS
  None.
===========================================================================*/
sys_srv_domain_e_type           cmregprx_map_sd_srv_domain_pref_to_sys
(

       sd_ss_srv_domain_pref_e_type    srv_domain_pref
          /* SD service domain preference.
          */

)
{
  sys_srv_domain_e_type sys_srv_domain_pref =
                                       SYS_SRV_DOMAIN_MAX;

  CM_ASSERT_ENUM_IS_INRANGE( srv_domain_pref,
                                   SD_SS_SRV_DOMAIN_PREF_MAX
                                 );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Switch on the SD service domain preference.
  */
  switch ( srv_domain_pref )
  {
    case SD_SS_SRV_DOMAIN_PREF_CS_ONLY:
      sys_srv_domain_pref = SYS_SRV_DOMAIN_CS_ONLY;
      break;

    case SD_SS_SRV_DOMAIN_PREF_PS_ONLY:
      sys_srv_domain_pref = SYS_SRV_DOMAIN_PS_ONLY;
      break;

    case SD_SS_SRV_DOMAIN_PREF_CS_PS:
      sys_srv_domain_pref = SYS_SRV_DOMAIN_CS_PS;
      break;

    case SD_SS_SRV_DOMAIN_PREF_MAX:
    default:
      sys_err_fatal_invalid_value_exception( (uint8) srv_domain_pref);

  } /* switch ( srv_domain_pref ) */


  return sys_srv_domain_pref;

} /* cmregprx_map_sd_srv_domain_pref_to_sys() */



/*===========================================================================

FUNCTION cmregprx_map_list_cnf_from_reg_to_sys

DESCRIPTION
  Function maps List abort status from REG to sys.

DEPENDENCIES
  None.

RETURN VALUE
  SYS_PLMN_LIST_SUCCESS   - PLMN search success.
  SYS_PLMN_LIST_AS_ABORT  - Access startum aborted PLMN search.


SIDE EFFECTS
  None.
===========================================================================*/
sys_plmn_list_status_e_type    cmregprx_map_list_cnf_from_reg_to_sys
(

       reg_cm_network_list_status_e_type  net_list_status
          /* SD service domain preference.
          */

)
{

  CM_ASSERT_ENUM_IS_INRANGE( net_list_status,
                                   REG_CM_NETWORK_LIST_MAX
                                 );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Switch to sys net list status
  */
  switch ( net_list_status )
  {
    case REG_CM_NETWORK_LIST_SUCCESS:    return SYS_PLMN_LIST_SUCCESS;

    case REG_CM_NETWORK_LIST_AS_ABORT:   return SYS_PLMN_LIST_AS_ABORT;

    case REG_CM_NETWORK_LIST_PARTIAL:    return SYS_PLMN_LIST_PARTIAL;

    case REG_CM_NETWORK_LIST_FAIL_IN_CONNECTED_MODE :  return SYS_PLMN_LIST_FAIL_IN_CONNECTED_MODE ;

    case REG_CM_NETWORK_LIST_MAX:
    default:
      CMREGPRX_ERR_1 ("Incorrect REG network list cnf %d",
                          net_list_status);

                                         return SYS_PLMN_LIST_NONE;

  } /* switch ( net_list_status ) */

} /* cmregprx_map_list_cnf_from_reg_to_sys() */


/*===========================================================================

FUNCTION cmregprx_map_list_cnf_from_sys_to_reg

DESCRIPTION
  This function maps the SD service domain pref type
  sd_ss_srv_domain_pref_e_type to sys_mode_pref_e_type.

DEPENDENCIES
  None.

RETURN VALUE

  REG_CM_NETWORK_LIST_SUCCESS   - PLMN search success.
  REG_CM_NETWORK_LIST_AS_ABORT  - Access startum aborted PLMN search.


SIDE EFFECTS
  None.
===========================================================================*/
reg_cm_network_list_status_e_type  cmregprx_map_list_cnf_from_sys_to_reg
(

       sys_plmn_list_status_e_type     net_list_status
          /* SD service domain preference.
          */

)
{

  CM_ASSERT_ENUM_IS_INRANGE( net_list_status,
                                   SYS_PLMN_LIST_MAX
                                 );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Switch to REG net list status
  */
  switch ( net_list_status )
  {
    case SYS_PLMN_LIST_SUCCESS:           return REG_CM_NETWORK_LIST_SUCCESS;

    case SYS_PLMN_LIST_AS_ABORT:          return REG_CM_NETWORK_LIST_AS_ABORT;

    case SYS_PLMN_LIST_PARTIAL :          return REG_CM_NETWORK_LIST_PARTIAL;

    case SYS_PLMN_LIST_FAIL_IN_CONNECTED_MODE :  return REG_CM_NETWORK_LIST_FAIL_IN_CONNECTED_MODE ;

    case SYS_PLMN_LIST_MAX:
    default:
      CMREGPRX_ERR_1 ("Incorrect SYS network list cnf %d",
                          net_list_status);

                                         return REG_CM_NETWORK_LIST_NODE;

  } /* switch ( net_list_status ) */

} /* cmregprx_map_list_cnf_from_sys_to_reg() */




/*===========================================================================

FUNCTION cmregprx_map_sd_manually_selected_plmn_to_reg_manual

DESCRIPTION
  This function maps the SD manually selected plmn type
  sd_manually_selected_plmn_e_type to reg user selection type for automatic
  network selection type.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Acquire specified PLMN.
  FALSE - Acquire registed PLMN.

  Use following value when stack is ready.

  REG_CM_MANUALLY_SELECTED_RPLMN - Acquire registed PLMN.
  REG_CM_MANUALLY_SELECTED_SPECIFIED_PLMN - Acquire specified PLMN.
  REG_CM_MANUALLY_SELECTED_HPLMN - Acquire home plmn.

SIDE EFFECTS
  None.
===========================================================================*/
static reg_cm_manually_selected_plmn_e_type
                         cmregprx_map_sd_manually_selected_plmn_to_reg_manual
(
       sd_manually_selected_plmn_e_type sd_selected_plmn,

       sys_plmn_id_s_type               plmn
)
/*lint -esym(715,plmn)*/
{
  reg_cm_manually_selected_plmn_e_type reg_selected_plmn =
                                               REG_CM_MANUALLY_SELECTED_NONE;

  CM_ASSERT_ENUM_IS_INRANGE( sd_selected_plmn,
                                   SD_MANUALLY_SELECTED_MAX
                                 );

  switch ( sd_selected_plmn )
  {
    case SD_MANUALLY_SELECTED_SPECIFIED_PLMN:
      reg_selected_plmn = REG_CM_MANUALLY_SELECTED_SPECIFIED_PLMN;
      break;

    case SD_MANUALLY_SELECTED_HPLMN:
      reg_selected_plmn = REG_CM_MANUALLY_SELECTED_HPLMN;
      break;

    case SD_MANUALLY_SELECTED_MAX:
      sys_err_fatal_invalid_value_exception( (uint8) sd_selected_plmn);
    /*lint -fallthrough */
    default:
      break;
  }

  return reg_selected_plmn;
}
/*lint +esym(715,plmn)*/


#if defined( FEATURE_RAT_PRIORITY_LIST )
/*===========================================================================

FUNCTION cmregprx_is_lte_but_no_g

DESCRIPTION
  This function evaluates if RAT prioroty list contains only LTE / TGW & 1x RAT. sms_only flag is set based on the RAT priority list
  If LTE only, we need to set the sms_only flag

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - LTE_ONLY
  else
  FALSE.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean                            cmregprx_is_lte_but_no_g
(
 sys_rat_pri_list_info_s_type       *rat_pri_list
 )
{
  uint32 i = 0;

  boolean is_rat_prio_lte = FALSE;
  boolean is_rat_prio_g   = FALSE;
  for( i = 0; i < rat_pri_list->num_items; i++)
  {

    /* checking if the RAT priority list has valis LTE band  */
    if((rat_pri_list->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE) ||
       (rat_pri_list->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE_M1) ||
       (rat_pri_list->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE_NB1)
      )
    {
      is_rat_prio_lte = TRUE;
    }

    /* checking if the RAT priority list has valis TGW band along with LTE band, then SMS only should be false*/
    if(rat_pri_list->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_GSM)

    {
      is_rat_prio_g = TRUE;
    }

  }

  /*  no TGW bands.  only LTE band is present  sms only should be true.*/
  if(  is_rat_prio_lte && !is_rat_prio_g)
  {
    return TRUE;
  }

  return FALSE;

}

/*===========================================================================

FUNCTION cmregprx_check_lte_in_rat_pri_kist

DESCRIPTION
  This function evaluates if RAT prioroty list contains  LTE .only RAT's. sms_only flag is set based on the RAT priority list
  If LTE only, we need to set the sms_only flag

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - LTE_ONLY
  else
  FALSE.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean                            cmregprx_check_lte_in_rat_pri_list
(
 sys_rat_pri_list_info_s_type       *rat_pri_list
 )
{
  uint32 i = 0;

  boolean is_rat_prio_lte = FALSE;

  for( i = 0; i < rat_pri_list->num_items; i++)
  {
    /* checking if the RAT priority list has valis LTE band  */
    if((rat_pri_list->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE) ||
       (rat_pri_list->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE_M1) ||
       (rat_pri_list->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE_NB1)
      )
    {
      is_rat_prio_lte = TRUE;
    }
  }

  /*  no TGW bands.  only LTE band is present  sms only should be true.*/
  if(is_rat_prio_lte == TRUE)
  {
    return TRUE;
  }

  return FALSE;

}

#endif

/*===========================================================================

FUNCTION cmregprx_fill_srv_req_parms

DESCRIPTION
  This function fills in the parms in CM_SERVICE_REQ.
  Does not fill in the following,
  a) trans_id
  b) message headers

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                               cmregprx_fill_srv_req_parms
(
       const sd_ss_act_s_type             *ss_act_ptr,
         /* Pointer to the SD action.
         */

       cm_service_req_s_type              *srv_req_ptr,
         /* Pointer to REG command buffer
         */

       cmregprx_stack_info_s_type         *cmregprx_stack_info
         /* Info of stack/sub on which srv_req being sent */

)
{
  sd_ss_e_type ss = SD_SS_MAIN;

  cmph_s_type                 *ph_ptr        = cmph_ptr();
      /* Point at phone object */

#ifdef FEATURE_LTE
  cmsds_sub_config_s_type   *cmsds_config = cmsds_get_config_ptr(SYS_MODEM_AS_ID_1);  
  #endif /* FEATURE_LTE */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( srv_req_ptr != NULL );
  CM_ASSERT( ss_act_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  
  /* Set default values for Follow on request parameters
  */
  srv_req_ptr->cs_for_request = FALSE;
  
  #ifdef FEATURE_LTE
  srv_req_ptr->sms_only       = FALSE;
  #endif
  srv_req_ptr->additional_info = 0;

  srv_req_ptr->network_selection_mode =
                               ss_act_ptr->prm.acq_gwl.net_sel_type;

  srv_req_ptr->scan_scope = ss_act_ptr->prm.acq_gwl.scan_scope;

  
  {
    srv_req_ptr->req_service_domain =
                             cmregprx_map_sd_srv_domain_pref_to_sys
                             (
                               ss_act_ptr->prm.acq_gwl.srv_domain_pref
                             );
    CMREGPRX_MSG_HIGH_4("cmregprx_fill_srv_req_parms(): Updating from SD ACT | network_selection_mode = %d scan_scope = %d, final_srv_domain %d, disable_call_type_mask %d",
                 srv_req_ptr->network_selection_mode, srv_req_ptr->scan_scope,srv_req_ptr->req_service_domain,
                 CMPH_SS_DISABLE_CALL_TYPE_MASK(cmregprx_stack_info->as_id,ph_ptr));
  }

  /* Passing down band pref to stack.
  */
  #ifdef FEATURE_GSM_BAND_PREF
  srv_req_ptr->band_pref =
                        (sys_band_mask_type) ss_act_ptr->prm.acq_gwl.gw_band_pref;
  #endif

  #ifdef FEATURE_RAT_PRIORITY_LIST
  srv_req_ptr->rat_pri_list_info = ss_act_ptr->prm.acq_gwl.rat_pri_list_info;
  
 
  #endif

  #ifdef FEATURE_LTE
  CMREGPRX_MSG_HIGH_4("FILL_SRV_REQ: srv req %d, network_selection_mode %d->%d, disable mask %x",
     (cmregprx_stack_info->last_srv_req.message_header.message_id == (byte)CM_SERVICE_REQ),
     cmregprx_stack_info->last_srv_req.network_selection_mode,
     srv_req_ptr->network_selection_mode, 
     CMPH_SS_DISABLE_CALL_TYPE_MASK(cmregprx_stack_info->as_id,ph_ptr));
  #endif
  
  /* Fill in network selection type specific parameters.
  */
  if ( (ss_act_ptr->prm.acq_gwl.net_sel_type == SYS_NETWORK_SELECTION_MODE_MANUAL )
       ||
       (ss_act_ptr->prm.acq_gwl.net_sel_type == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)
     )
  {
    /* Mapping the sd manually selected plmn type to reg cm manually selected
    ** type.
    */
    srv_req_ptr->parameters.manual.selected_plmn_type =
      cmregprx_map_sd_manually_selected_plmn_to_reg_manual
      (
        ss_act_ptr->prm.acq_gwl.selected_plmn_type,
        ss_act_ptr->prm.acq_gwl.plmn
      );

    *(&(srv_req_ptr->parameters.manual.plmn)) = *(&(ss_act_ptr->prm.acq_gwl.plmn));

    srv_req_ptr->parameters.manual.type =
    cmregprx_map_sd_srv_req_type_to_reg_type( ss_act_ptr->prm.acq_gwl.srv_req_type);
    srv_req_ptr->parameters.manual.rat    = ss_act_ptr->prm.acq_gwl.csg_rat;

    CMREGPRX_MSG_HIGH_6("FILL_SRV_REQ:  Manual: srv_req_type=%d, CSG_ID=%d, CSG_RAT=%d, plmn id [0]=%x, [1]=%x, [2]=%x",
                        srv_req_ptr->parameters.manual.type,
                        srv_req_ptr->parameters.manual.csg_id,
                        srv_req_ptr->parameters.manual.rat,
                        srv_req_ptr->parameters.manual.plmn.identity[0],
                        srv_req_ptr->parameters.manual.plmn.identity[1],
                        srv_req_ptr->parameters.manual.plmn.identity[2]);
  }
  else if ( ss_act_ptr->prm.acq_gwl.net_sel_type == SYS_NETWORK_SELECTION_MODE_LIMITED )
  {
    
      srv_req_ptr->parameters.limited.type =
      cmregprx_map_sd_srv_req_type_to_reg_type( ss_act_ptr->prm.acq_gwl.srv_req_type);
   
      CMREGPRX_MSG_HIGH_1("FILL_SRV_REQ:  Limited: srv_req_type=%d", srv_req_ptr->parameters.limited.type);
  }
  else
  {
   srv_req_ptr->parameters.automatic.type =
     cmregprx_map_sd_srv_req_type_to_reg_type( ss_act_ptr->prm.acq_gwl.srv_req_type);

   srv_req_ptr->parameters.automatic.rat    = ss_act_ptr->prm.acq_gwl.csg_rat;

   if(srv_req_ptr->parameters.automatic.csg_id != SYS_CSG_ID_INVALID)
   {
      CMREGPRX_MSG_HIGH_6("FILL_SRV_REQ: Auto: srv_req_type=%d, CSG_ID=%d, CSG_RAT=%d, plmn id [0]=%x, [1]=%x, [2]=%x",
                       srv_req_ptr->parameters.automatic.type,
                       srv_req_ptr->parameters.automatic.csg_id,
                       srv_req_ptr->parameters.automatic.rat,
                       srv_req_ptr->parameters.automatic.plmn.identity[0],
                       srv_req_ptr->parameters.automatic.plmn.identity[1],
                       srv_req_ptr->parameters.automatic.plmn.identity[2]);
  }
  }

  srv_req_ptr->additional_info = ss_act_ptr->prm.acq_gwl.additional_info;

  /* check if emergency call is originated at CM level or if UE is in callback mode
  ** This information is used by NAS/ LTE RRC to give priority to systems that support
  ** emergency services.
  */
  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  if ( (cmcall_is_there_a_call_type(CM_SS_MAX,CM_CALL_TYPE_EMERGENCY,
                                 CM_CALL_ID_INVALID) != CM_CALL_ID_INVALID ) ||
       cmph_is_in_emergency_cb()
     )
  {
    CMREGPRX_MSG_HIGH_0("FILL_SRV_REQ: srv_req: emerg_call");
    srv_req_ptr->additional_info |= BM(SYS_ADDTNL_INFO_EMERG_CALL);
  }
  #endif


  /* Set follow on request if Voice or CS Data  or emergency call is pending
  */
  #if !(defined(FEATURE_MMODE_LTE_ONLY) || defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE))
  if ((cmcall_check_call_type_in_call_state_ss
                         (CM_CALL_TYPE_EMERGENCY, CM_CALL_STATE_ORIG, (cm_ss_e_type)ss)) ||
      (cmcall_check_call_type_in_call_state_ss
                         (CM_CALL_TYPE_VOICE, CM_CALL_STATE_ORIG, (cm_ss_e_type)ss))     ||
      (cmcall_check_call_type_in_call_state_ss
                         (CM_CALL_TYPE_CS_DATA, CM_CALL_STATE_ORIG, (cm_ss_e_type)ss))
#if !(defined(FEATURE_CM_DISABLE_IMS_EXT) || defined(FEATURE_CATM1_DISABLE))
|| (cmemg_get_reason_per_sub(cmregprx_stack_info->as_id) == CM_AC_REASON_THIRD_PARTY_IMS_E911)
#endif
  )
  {
    /* Fill in the field which indicates if origination is pending.
    */
    srv_req_ptr->cs_for_request = TRUE;
  }

  #endif

  #ifdef FEATURE_LTE

  if( (cmregprx_is_lte_but_no_g (&(srv_req_ptr->rat_pri_list_info))&&
     (CMPH_SS_SRV_DOMAIN_PREF(cmregprx_stack_info->as_id,ph_ptr) == CM_SRV_DOMAIN_PREF_CS_PS)) ||
     ((CMPH_SS_DISABLE_CALL_TYPE_MASK(cmregprx_stack_info->as_id,ph_ptr) & CM_CALL_TYPE_VOICE_MASK) &&
      cmsds_config->is_sms_only)
    )
  {
    srv_req_ptr->sms_only       = TRUE;
    CMREGPRX_MSG_HIGH_1("Domsel:forcing SMS only to TRUE %d", srv_req_ptr->sms_only);
  }

  #ifndef FEATURE_CIOT
  if( cmregprx_is_lte_but_no_g (&(srv_req_ptr->rat_pri_list_info))&&
     (CMPH_SS_SRV_DOMAIN_PREF(cmregprx_stack_info->as_id,ph_ptr) == CM_SRV_DOMAIN_PREF_CS_PS)
    )
  {
    srv_req_ptr->sms_only       = TRUE;
    CMREGPRX_MSG_HIGH_1("FILL_SRV_REQ: Domsel:forcing SMS only to TRUE %d", srv_req_ptr->sms_only);
  }
  #else
  /* In IOE flavors, there is no CS voice support. So set sms_only to TRUE by default */
  if((CMPH_SS_SRV_DOMAIN_PREF(cmregprx_stack_info->as_id,ph_ptr) == CM_SRV_DOMAIN_PREF_CS_PS) &&
     cmregprx_check_lte_in_rat_pri_list(&(srv_req_ptr->rat_pri_list_info))
    )
  {
    srv_req_ptr->sms_only       = TRUE;
  }
  #endif

  srv_req_ptr->lte_cs_domain_param.voice_domain_pref = cmsds_config->voice_domain_pref;
  srv_req_ptr->lte_cs_domain_param.voice_or_data_centric = cmsds_config->ue_usage_setting;
  srv_req_ptr->lte_cs_domain_param.is_voice_domain_ie_valid  = FALSE;
  
  /* Check if there is connected IMS voice/emeergency call, indicate if so.
  */
  {
    cm_call_id_type ims_call_id = cmcall_is_there_volte_call_obj_except_wlan((CM_CALL_TYPE_VOICE_MASK | 
                             CM_CALL_TYPE_VS_MASK | CM_CALL_TYPE_VT_MASK | CM_CALL_TYPE_EMERG_MASK ));
    cmcall_s_type   *call_ptr = cmcall_ptr(ims_call_id);	
    if(call_ptr != NULL && call_ptr->call_state == CM_CALL_STATE_CONV)
    {
      CM_MSG_HIGH_2("FILL_SRV_REQ: Call id %d type %d in connected ", ims_call_id, call_ptr->call_type );
      srv_req_ptr->additional_info |= BM(SYS_ADDTNL_INFO_IMS_CALL_CONNECTED);
    }
  }
  
#ifndef FEATURE_MMODE_DISABLE_LTE_VOICE

  if(cmsds_config->is_ims_enabled == TRUE)
  {
    srv_req_ptr->lte_cs_domain_param.is_voice_domain_ie_valid = TRUE;
  }
#endif
   /* Fill device type */
  #endif /* FEATURE_LTE */

   if(
   #if defined FEATURE_CM_THERMAL_EMERGENCY_SUPPORT && !defined FEATURE_GPS_WWAN_PRIORITY_SEARCH
   cmaccessctrl_ptr()->state == CMAC_STATE_EMERGENCY_ONLY && 
  #endif
   ph_ptr->mo_exception_status ==TRUE)
   {
      srv_req_ptr->additional_info |= BM(SYS_ADDTNL_INFO_START_MO_EXCEPTION);
   }

   #ifdef FEATURE_FMD_SPEED_INFO
   if(ph_ptr->is_oprt_modes_pseudo_online == TRUE)
   	{
   	  srv_req_ptr->additional_info |= BM(SYS_ADDTNL_INFO_PSEUDO_ONLINE_MODE);
   	}
    #endif
   
} /* cmregprx_fill_srv_req_parms() */


#ifdef FEATURE_RAT_PRIORITY_LIST
#if 0
/*===========================================================================

FUNCTION cmregprx_is_rat_pri_list_info_same

DESCRIPTION
  This function compares the new srv_req RAT priority list info versus the
  previous one sent.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.
===========================================================================*/
static boolean       cmregprx_is_rat_pri_list_info_same
(
       const  sys_rat_pri_list_info_s_type      *curr_rat_pri_list_info_ptr,
         /* Pointer to current RAT priority list info in Service request
         */

       const  sys_rat_pri_list_info_s_type      *last_rat_pri_list_info_ptr
         /* Pointer to last RAT priority list info in Service request
         */

)
{

  boolean is_rat_prio_same = TRUE;
  int i = 0;
  int num_items = 0;
  uint32 num_3gpp_items = 0;

  sys_rat_pri_list_info_s_type  *curr_3gpp_rat_pri_list_info_ptr = NULL;
  sys_rat_pri_list_info_s_type  *last_3gpp_rat_pri_list_info_ptr = NULL;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( curr_rat_pri_list_info_ptr != NULL );
  CM_ASSERT( last_rat_pri_list_info_ptr!= NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* All parameters are compared
  */
  if ( curr_rat_pri_list_info_ptr->num_items !=
                       last_rat_pri_list_info_ptr->num_items  )
  {
    return FALSE;
  }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Try allocating buffer.
    */
    curr_3gpp_rat_pri_list_info_ptr = (sys_rat_pri_list_info_s_type *)
                                       cm_mem_malloc (
                                          sizeof (sys_rat_pri_list_info_s_type)
                                       );
    /* If allocation failed, error fatal.
    */
    if( curr_3gpp_rat_pri_list_info_ptr == NULL )
    {
      sys_err_fatal_null_ptr_exception();
    }

    last_3gpp_rat_pri_list_info_ptr = (sys_rat_pri_list_info_s_type *)
                                       cm_mem_malloc (
                                          sizeof (sys_rat_pri_list_info_s_type)
                                       );

    //Preparing temp current rat list with only 3gpp rats.
    #ifndef FEATURE_MMODE_LTE_ONLY
    //curr_3gpp_rat_pri_list_info_ptr->next_acq_sys_index = curr_rat_pri_list_info_ptr->next_acq_sys_index;
    #endif
    
    num_items = (int) curr_rat_pri_list_info_ptr->num_items;
    num_3gpp_items = 0;

    for ( i = 0; i < num_items; i++ )
    {
      if(CM_IS_MODE_3GPP(curr_rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode))
      {
        memscpy( &curr_3gpp_rat_pri_list_info_ptr->priority_list_info[num_3gpp_items++],
                 sizeof(curr_3gpp_rat_pri_list_info_ptr->priority_list_info),
                 &curr_rat_pri_list_info_ptr->priority_list_info[i],
                 sizeof (curr_rat_pri_list_info_ptr->priority_list_info[i]));
      }
    }
    curr_3gpp_rat_pri_list_info_ptr->num_items = num_3gpp_items;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    //Preparing temp last rat list with only 3gpp rats.
    #ifndef FEATURE_MMODE_LTE_ONLY
    //last_3gpp_rat_pri_list_info_ptr->next_acq_sys_index = last_rat_pri_list_info_ptr->next_acq_sys_index;
    #endif

    num_items = (int) last_rat_pri_list_info_ptr->num_items;
    num_3gpp_items = 0;

    for ( i = 0; i < num_items; i++ )
    {
      if(CM_IS_MODE_3GPP(last_rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode))
      {
        memscpy( &last_3gpp_rat_pri_list_info_ptr->priority_list_info[num_3gpp_items++],
                 sizeof(last_3gpp_rat_pri_list_info_ptr->priority_list_info),
                 &last_rat_pri_list_info_ptr->priority_list_info[i],
                 sizeof (last_rat_pri_list_info_ptr->priority_list_info[i]));
      }
    }
    last_3gpp_rat_pri_list_info_ptr->num_items = num_3gpp_items;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* All parameters are compared
  */
  if ( curr_3gpp_rat_pri_list_info_ptr->num_items !=
                       last_3gpp_rat_pri_list_info_ptr->num_items  )
  {
    CMREGPRX_MSG_HIGH_2("COMP_SRV_REQ: num_items %d->%d",
                     last_3gpp_rat_pri_list_info_ptr->num_items,
                     curr_3gpp_rat_pri_list_info_ptr->num_items);

    is_rat_prio_same = FALSE;
  }

  if( is_rat_prio_same )
  {
    if( curr_3gpp_rat_pri_list_info_ptr->num_items >
                                              SYS_MAX_ACQ_PREF_RAT_LIST_NUM )
    {
      num_items = SYS_MAX_ACQ_PREF_RAT_LIST_NUM;
      CMREGPRX_ERR_1("IRAT: Curr UE cap num items = %d > MAX",
                      curr_3gpp_rat_pri_list_info_ptr->num_items);
    }
    else
    {
      num_items = (int) curr_3gpp_rat_pri_list_info_ptr->num_items;
    }

    for ( i = 0; i < num_items; i++ )
    {
      if( curr_3gpp_rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode !=
          last_3gpp_rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode
          ||
          /*curr_3gpp_rat_pri_list_info_ptr->priority_list_info[i].acq_sys_time_interval !=
          last_3gpp_rat_pri_list_info_ptr->priority_list_info[i].acq_sys_time_interval
          ||*/
          curr_3gpp_rat_pri_list_info_ptr->priority_list_info[i].bst_rat_acq_required !=
          last_3gpp_rat_pri_list_info_ptr->priority_list_info[i].bst_rat_acq_required
          ||
          (curr_3gpp_rat_pri_list_info_ptr->priority_list_info[i].bst_rat_acq_required  == TRUE
            &&
          ((curr_3gpp_rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode !=SYS_SYS_MODE_LTE &&
            curr_3gpp_rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap.chgwt_band_cap !=
            last_3gpp_rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap.chgwt_band_cap)
               ||
           (curr_3gpp_rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE &&
            !SYS_LTE_BAND_MASK_CHK_IF_EQUAL(&(curr_3gpp_rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap.lte_band_cap),
                                            &(last_3gpp_rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap.lte_band_cap)
                                           )
            )
          )
          )
        )

      {
  	    CMREGPRX_MSG_HIGH_5("COMP_SRV_REQ: diff rat pri, index %d last info %d %d %d %d",
  				   i, 
  				   last_3gpp_rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode,
  				   last_3gpp_rat_pri_list_info_ptr->priority_list_info[i].bst_rat_acq_required,
  				   (curr_3gpp_rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap.chgwt_band_cap !=
  					last_3gpp_rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap.chgwt_band_cap),
  				   SYS_LTE_BAND_MASK_CHK_IF_EQUAL(&(curr_3gpp_rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap.lte_band_cap),
  										  &(last_3gpp_rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap.lte_band_cap)
  										 ));

        is_rat_prio_same = FALSE;
        break;
      }
    }
    #ifdef FEATURE_EOOS
    #error code not present
#endif
  }
  if(curr_3gpp_rat_pri_list_info_ptr)
  {
    cm_mem_free (curr_3gpp_rat_pri_list_info_ptr);
  }
  if(last_3gpp_rat_pri_list_info_ptr)
  {
    cm_mem_free (last_3gpp_rat_pri_list_info_ptr);
  }

  return is_rat_prio_same;

}
#endif
#endif /* #ifdef FEATURE_RAT_PRIORITY_LIST */

/*===========================================================================

FUNCTION cmregprx_compare_srv_req

DESCRIPTION
  This function compares the new srv_req versus the previous one sent.

DEPENDENCIES
  None.

RETURN VALUE
  cmregprx_srv_req_diff_e_type

SIDE EFFECTS
  None.
===========================================================================*/
static cmregprx_srv_req_diff_e_type       cmregprx_compare_srv_req
(
       const sd_ss_act_s_type             *ss_act_ptr,
         /* Pointer to the SD action.
         */

       const cmregprx_stack_info_s_type   *cmregprx_stack_info_ptr,
          /* Pointer to RegProxy's information.
          */

       const cm_service_req_s_type        *srv_req_buff
)
{
  #ifdef FEATURE_RAT_PRIORITY_LIST
  boolean rat_pri_list_info_is_same = FALSE;
  boolean is_b2b_suitable_srch = FALSE;
  #endif

  CM_ASSERT( ss_act_ptr != NULL );
  CM_ASSERT( ss_act_ptr->act == SD_SS_ACT_ACQ_GWL );
  CM_ASSERT( srv_req_buff != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the action is ACQ_GW and there is a pending CM_SERVICE_CNF.
  */
  if ( ss_act_ptr->act != SD_SS_ACT_ACQ_GWL ||
       cmregprx_stack_info_ptr->last_srv_req.message_header.message_id !=
                                                         (byte)CM_SERVICE_REQ
     )
  {
    return CMREGPRX_SRV_REQ_DIFF_OTHER_PARMS;
  }
  
  if (memcmp(&(cmregprx_stack_info_ptr->last_srv_req.network_selection_mode), &(srv_req_buff->network_selection_mode), 
      (sizeof(cm_service_req_s_type)) - (sizeof(IMH_T) + sizeof(reg_cm_transaction_id_type))))
  {

    return CMREGPRX_SRV_REQ_DIFF_OTHER_PARMS;
    
  }

  #if 0

  if ( srv_req_buff->cs_for_request != cmregprx_stack_info_ptr->last_srv_req.cs_for_request)
  {
   CMREGPRX_MSG_HIGH_0("COMP_SRV_REQ: last srv_req. params same except for_request");
   return CMREGPRX_SRV_REQ_DIFF_FOR_REQ_ONLY;
  }  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_RAT_PRIORITY_LIST
  rat_pri_list_info_is_same = cmregprx_is_rat_pri_list_info_same(&(srv_req_buff->rat_pri_list_info),
                              &cmregprx_stack_info_ptr->last_srv_req.rat_pri_list_info);

  /* check if it is back to back suitable srch */
  if (srv_req_buff->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC &&
     srv_req_buff->parameters.automatic.type == REG_CM_SERVICE_REQ_SUITABLE_SEARCH &&
     srv_req_buff->network_selection_mode == cmregprx_stack_info_ptr->last_srv_req.network_selection_mode &&
     srv_req_buff->parameters.automatic.type == cmregprx_stack_info_ptr->last_srv_req.parameters.automatic.type
  )
  {
    is_b2b_suitable_srch = TRUE;
    CMREGPRX_MSG_HIGH_0("COMP_SRV_REQ: b2b suitable srch");
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* All parameters are compared except for_requests.
  */
  if ( (srv_req_buff->req_service_domain != cmregprx_stack_info_ptr->last_srv_req.req_service_domain && !is_b2b_suitable_srch) ||
       #ifdef FEATURE_GSM_BAND_PREF
       srv_req_buff->band_pref != cmregprx_stack_info_ptr->last_srv_req.band_pref ||
       #endif
       srv_req_buff->network_selection_mode != cmregprx_stack_info_ptr->last_srv_req.network_selection_mode ||
       ( srv_req_buff->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL  &&
         (
           srv_req_buff->parameters.manual.selected_plmn_type !=
               cmregprx_stack_info_ptr->last_srv_req.parameters.manual.selected_plmn_type ||
           srv_req_buff->parameters.manual.type !=
               cmregprx_stack_info_ptr->last_srv_req.parameters.manual.type ||
           !sys_plmn_match( srv_req_buff->parameters.manual.plmn,
                            cmregprx_stack_info_ptr->last_srv_req.parameters.manual.plmn) 

           ||
             srv_req_buff->parameters.manual.rat     != cmregprx_stack_info_ptr->last_srv_req.parameters.manual.rat
         )

       ) 
       ||
       ( srv_req_buff->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC &&
         (srv_req_buff->parameters.automatic.type    != cmregprx_stack_info_ptr->last_srv_req.parameters.automatic.type 
         ||
         srv_req_buff->parameters.automatic.rat     != cmregprx_stack_info_ptr->last_srv_req.parameters.automatic.rat
         )
       )
       #ifdef FEATURE_RAT_PRIORITY_LIST
       || (!rat_pri_list_info_is_same && !is_b2b_suitable_srch)
       #endif
       #ifdef FEATURE_LTE
        || srv_req_buff->sms_only !=  cmregprx_stack_info_ptr->last_srv_req.sms_only
        || srv_req_buff->additional_info != cmregprx_stack_info_ptr->last_srv_req.additional_info
       #endif
       || ( (srv_req_buff->scan_scope !=  cmregprx_stack_info_ptr->last_srv_req.scan_scope))
	   )
  {
    CMREGPRX_MSG_HIGH_6("COMP_SRV_REQ: last srv_req. params not same, req_service_domain %d->%d, rat_pri_list_info_is_same %d, last addl %d->%d, last num_items %d",
                            cmregprx_stack_info_ptr->last_srv_req.req_service_domain,
                            srv_req_buff->req_service_domain,                            
                            rat_pri_list_info_is_same,
                            cmregprx_stack_info_ptr->last_srv_req.additional_info,
                            srv_req_buff->additional_info,
                            cmregprx_stack_info_ptr->last_srv_req.rat_pri_list_info.num_items);
    CMREGPRX_MSG_HIGH_5("COMP_SRV_REQ: last srv_req. params not same, scan_sope: %d->%d, chgd %d %d %d",
                            cmregprx_stack_info_ptr->last_srv_req.scan_scope,
                            srv_req_buff->scan_scope,                            
                            (srv_req_buff->sms_only != cmregprx_stack_info_ptr->last_srv_req.sms_only),
                            (srv_req_buff->parameters.automatic.type    != cmregprx_stack_info_ptr->last_srv_req.parameters.automatic.type),
                            (cmregprx_stack_info_ptr->last_srv_req.band_pref != srv_req_buff->band_pref)
                            );
    
    return CMREGPRX_SRV_REQ_DIFF_OTHER_PARMS;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Now check if only cs_for_request or ps_for_request differs.
  */

  #ifdef FEATURE_CM_LTE
  if(cmregprx_stack_info_ptr->lte_connected_mode == TRUE &&
     cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_SRV_CNF)
  {
    CMREGPRX_MSG_HIGH_0("COMP_SRV_REQ: UE is in connected mode and waiting for srv cnf");
  }
  else
  #endif
  #endif
  return CMREGPRX_SRV_REQ_DIFF_NONE;
} /* cmregprx_compare_srv_req() */

/*===========================================================================

FUNCTION cmregprx_send_reg_service_req

DESCRIPTION
  This function sends the cm_service_req to REG based on the SD action
  parameters.
  This will update the substate upon sending the REG request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_send_reg_service_req
(

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{
  /* Pointer to REG command buffer
  */
  reg_cmd_type *reg_ptr;

  /* SD act parameters.
  */
  sd_ss_act_s_type  sd_act;

  cm_service_req_s_type srv_req_local;

  IMH_T header_temp;
  /* Variables for printing PLMN.
  */
  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  mcc;
  uint32  mnc;
  

  /*variable for stroing rpt_type*/
  cm_rpt_type* camped_ind_buf_ptr = NULL;


  /* Difference in new srv_req versus previous one.
  */
  cmregprx_srv_req_diff_e_type  srv_req_diff;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  memset(&srv_req_local, 0, sizeof(cm_service_req_s_type));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  camped_ind_buf_ptr = cmregprx_stack_info_ptr->camped_ind_buf;

  (void)sd_ss_act_get( &sd_act );

  if ( sd_act.act  !=  SD_SS_ACT_ACQ_GWL )
  {
    /* SD action should be Acquire GSM/WCDMA network.
    */
    CMREGPRX_ERR_0("Incorrect sd_action received");

    return;

  } /* if ( sd_ss_gw_act_get( &sd_act )  !=  SD_SS_ACT_ACQ_GWL ) */

  /*
  ** Update the sys sel pref req id only when it is non zero
  */
  if(CM_DEFAULT_VALUE != sd_act.prm.acq_gwl.sys_sel_pref_req_id)
  {
    cmregprx_stack_info_ptr->sys_sel_pref_req_id = sd_act.prm.acq_gwl.sys_sel_pref_req_id;
    CMREGPRX_MSG_HIGH_2("Now: cmregprx_stack_info_ptr->sys_sel_pref_req_id=%u, sd_act.prm.acq_gwl.sys_sel_pref_req_id=%u",cmregprx_stack_info_ptr->sys_sel_pref_req_id,sd_act.prm.acq_gwl.sys_sel_pref_req_id);
  }

#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
  if (cmregprx_stack_info_ptr->list_search_info.list_search_type == REG_CM_NETWORK_LIST_REQ_GPS_ADVANCE_SEARCH)
  {
    cmph_ptr()->is_gps_search_high_priority = FALSE;
  }
#endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_CM_LTE
  /* This flags is set to TRUE in RLF indication, and to
  ** FALSE on T311 expiry indication and on a full service cnf
  */
  if (cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
  {
     sd_act.prm.acq_gwl.srv_req_type = SD_SS_SRV_REQ_TYPE_LTE_SUITABLE_SEARCH;
  }
  
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  
  /* Fill service_req parms.
  */
  cmregprx_fill_srv_req_parms(&sd_act, &srv_req_local, cmregprx_stack_info_ptr);

  /* Check if Service_request needs to be sent.
  */
  srv_req_diff = cmregprx_compare_srv_req( &sd_act, cmregprx_stack_info_ptr, &srv_req_local );

  /*
  ** Removed the check srv_req_diff == CMREGPRX_SRV_REQ_DIFF_FOR_REQ_ONLY.
  ** CM shouldn't be filtering SERVICE_REQUEST when only the FOR fields are
  ** different. This filtering is now being done in REG.
  */
  if (srv_req_diff == CMREGPRX_SRV_REQ_DIFF_NONE &&
      cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_GEN_CMD)
  {
    CMREGPRX_MSG_HIGH_0("Dupl.srv_req action ");

    /* Check if camped_ind is stored.
    */

    if (
         ( cmregprx_stack_info_ptr->camped_ind_buf != NULL) &&
         ( cmregprx_stack_info_ptr->camped_ind_buf->hdr.cmd == CM_CAMPED_IND )
       )
    {
      cm_rpt_type *cmd_ptr;
      {
        /* CAMPED_IND is buffered, simulate CM_CAMPED_IND from REG.
      */
      CMREGPRX_MSG_HIGH_0("generating local camped_ind");

      /* Get free rpt buffer from CM.
      */
      if ((cmd_ptr = cm_get_cmd_buf()) == NULL)
      {
        CMREGPRX_MSG_HIGH_0("Failed to get cm rpt buffer");
        return;
      }

       /* Queue camped_ind rpt command to CM.
      */

      *cmd_ptr = *camped_ind_buf_ptr;
      cm_put_cmd(cmd_ptr);

      }


      /* Release the memory held by camped-ind_buffer
      */
      if ( cmregprx_stack_info_ptr->camped_ind_buf != NULL )
      {
        cm_mem_free (cmregprx_stack_info_ptr->camped_ind_buf);
        cmregprx_stack_info_ptr->camped_ind_buf = NULL;
      }


    } /* if ( cmregprx_info_ptr->camped_ind_buf.hdr.cmd == CM_CAMPED_IND ) */

    /* None of the parms changed, so don't send any requests.
    */
    CMREGPRX_MSG_HIGH_0("Same CM_SERVICE_REQ, not sent");
    return;

  }  /* if ( ) */
  
  


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( cmregprx_stack_info_ptr->start_thrtl_ref_uptime > time_get_uptime_secs() )
  {
    /* The 32K timer expired the range 36.4 hrs, reset the start time of CM_SERVICE_REQ.
      */
    cmregprx_stack_info_ptr->start_thrtl_ref_uptime = time_get_uptime_secs();
  }

  /* Count the number of CM_SERVICE_REQ if within this period.
  */
  cmregprx_stack_info_ptr->cnt_cm_srv_req++;

  /* Process CM_SERVICE_REQ if within the maximum count.
  */
  if ( cmregprx_stack_info_ptr->cnt_cm_srv_req > CMREGPRX_MAX_SRV_CNF_REQ )
  {
    CMREGPRX_MSG_HIGH_2("Buffering srv. req, start cnt period %d, uptime %d",
                                           cmregprx_stack_info_ptr->start_thrtl_ref_uptime,
                                           time_get_uptime_secs());

    /* Set the RegProxy's substate to waiting for service confirmation.
    */
    cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_SRV_CNF,
                              cmregprx_stack_info_ptr
                            );

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get reg cmd. buffer and populate params.
  */
  reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_SERVICE_REQ );

  header_temp = reg_ptr->cmd.hdr;
  reg_ptr->cmd.cm_service_req = srv_req_local;
  reg_ptr->cmd.hdr = header_temp;

  reg_ptr->cmd.cm_service_req.transaction_id =
                             cmregprx_next_trans_id
                             (
                               cmregprx_stack_info_ptr
                             );

  /* Fill active subscription id */
  #if !(defined(FEATURE_MMODE_LTE_ONLY) || defined(FEATURE_MMODE_GPRS_LTE) || defined(FEATURE_CATM1_DISABLE))
  reg_ptr->cmd.cm_service_req.as_id = cmregprx_stack_info_ptr->as_id;
  reg_ptr->cmd.cm_service_req.stack_id = cmregprx_stack_info_ptr->nas_stack_id;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_MSG_HIGH_3("CMREG->NAS:  addl %d , cs_for_request = %d,sms_only = %d Send SERVICE_REQ",
                     reg_ptr->cmd.cm_service_req.additional_info,
                     reg_ptr->cmd.cm_service_req.cs_for_request,
                     reg_ptr->cmd.cm_service_req.sms_only);
  

  
  #ifdef FEATURE_GSM_BAND_PREF

  CMREGPRX_MSG_HIGH_5("net_sel_mode %d srv_domain %d, req_id = %u band_pref =0x%x 0x%x",
                    reg_ptr->cmd.cm_service_req.network_selection_mode,
                    reg_ptr->cmd.cm_service_req.req_service_domain,
                    cmregprx_stack_info_ptr->sys_sel_pref_req_id,
                    QWORD_HIGH(reg_ptr->cmd.cm_service_req.band_pref),
                    QWORD_LOW(reg_ptr->cmd.cm_service_req.band_pref));

  #else

  CMREGPRX_MSG_HIGH_3(" net_sel_mode %d srv_domain %d, req_id = %u ",
                    reg_ptr->cmd.cm_service_req.network_selection_mode,
                    reg_ptr->cmd.cm_service_req.req_service_domain,
                    cmregprx_stack_info_ptr->sys_sel_pref_req_id);
  

  #endif
 
  

  if( (reg_ptr->cmd.cm_service_req.network_selection_mode ==
                       SYS_NETWORK_SELECTION_MODE_MANUAL)
      ||
      (reg_ptr->cmd.cm_service_req.network_selection_mode ==
                       SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)
      #ifndef FEATURE_MMODE_3GPP2
      ||
      (reg_ptr->cmd.cm_service_req.network_selection_mode ==
                       SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY)
      #endif
    )
  {
    CMREGPRX_MSG_HIGH_3("manual selected plmn type %d, req_id %u srv_req_type %d",
                      sd_act.prm.acq_gwl.selected_plmn_type,
                      cmregprx_stack_info_ptr->sys_sel_pref_req_id,
                      reg_ptr->cmd.cm_service_req.parameters.manual.type);


    sys_plmn_get_mcc_mnc
    (
      reg_ptr->cmd.cm_service_req.parameters.manual.plmn,
      &plmn_id_is_undefined,
      &mnc_includes_pcs_digit,
      &mcc,
      &mnc
    );
    CMREGPRX_MSG_HIGH_6("PLMN - undefined %d pcs_digit %d PLMN(%d-%d) | CSG ID = %d, CSG RAT=%d",
                      plmn_id_is_undefined,
                      mnc_includes_pcs_digit,
                      mcc,
                      mnc,
                      reg_ptr->cmd.cm_service_req.parameters.manual.csg_id,
                      reg_ptr->cmd.cm_service_req.parameters.manual.rat);

  }
  else if(reg_ptr->cmd.cm_service_req.network_selection_mode ==
           SYS_NETWORK_SELECTION_MODE_LIMITED)
  {
    CMREGPRX_MSG_HIGH_1("srv_req_type %d", reg_ptr->cmd.cm_service_req.parameters.limited.type);
  }
  else
    if(reg_ptr->cmd.cm_service_req.network_selection_mode ==
       SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
    {
      
      CMREGPRX_MSG_HIGH_4("user_selection %d srv_req_type %d, CSG ID=%d, CSG RAT=%d",
                         sd_act.prm.acq_gwl.is_user_resel,
                         reg_ptr->cmd.cm_service_req.parameters.automatic.type,
                         reg_ptr->cmd.cm_service_req.parameters.automatic.csg_id,
                         reg_ptr->cmd.cm_service_req.parameters.automatic.rat);
    }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Save the CM_SERVICE_REQ in local buffer &
  ** Post the command to REG.
  */
  cmregprx_stack_info_ptr->last_srv_req = reg_ptr->cmd.cm_service_req;

  cm_reg_send_cmd( reg_ptr );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cm_get_ulog_handle() != NULL)
  {
    ULOG_RT_PRINTF_1(cm_get_ulog_handle(), "=CM= Sending service request to NAS, net sel mode:%d",
                                  reg_ptr->cmd.cm_service_req.network_selection_mode);
  }
  

  /* Set the RegProxy's substate to waiting for service confirmation.
  */
  cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_SRV_CNF,
                            cmregprx_stack_info_ptr
                          );

  return;
} /* cmregprx_send_reg_service_req() */

/*===========================================================================

FUNCTION cmregprx_send_reg_net_list_req

DESCRIPTION
  This function sends the cm_network_list_req to REG based on the SD action
  parameters.
  This will update the substate upon sending the REG request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_send_reg_net_list_req
(

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{
  /* Allocate a REG command buffer
  */
  reg_cmd_type *reg_ptr =  cm_reg_get_buf_else_err_fatal(CM_NETWORK_LIST_REQ);

  /* SD act parameters.
  */
  sd_ss_act_s_type  sd_act;
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH_TEST
  uint8 num_ad = 0;
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if ( sd_ss_act_get( &sd_act )  !=  SD_SS_ACT_GET_NET_GW )
  {
    sys_err_fatal_invalid_value_exception(sd_act.act);
  } /* if ( sd_ss_gw_act_get( &sd_act )  !=  SD_SS_ACT_GET_NET_GW ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset the camped_ind buffer.
  */

  if (cmregprx_stack_info_ptr->camped_ind_buf  != NULL)
  {
    cm_mem_free ( cmregprx_stack_info_ptr->camped_ind_buf );
    cmregprx_stack_info_ptr->camped_ind_buf  = NULL;
  }
  cmregprx_stack_info_ptr->last_srv_req.message_header.message_id =
                                                      (byte)CM_REG_CMD_NONE;

  CMREGPRX_MSG_MED_1("In cmregprx_send_reg_net_list_req %d", sd_act.prm.get_net_gw.network_list_type );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( SD_NETWORK_LIST_PRIORITY_PLMN_SEARCH_BGND == sd_act.prm.get_net_gw.network_list_type )
  {
            cmregprx_stack_info_ptr->list_search_info.list_search_type =
                                         REG_CM_NETWORK_LIST_REQ_PRIORITY_PLMN_SEARCH_BGND;
            reg_ptr->cmd.cm_network_list_req.list_search_type =
                                         REG_CM_NETWORK_LIST_REQ_PRIORITY_PLMN_SEARCH_BGND;           
  }
  else if( SD_NETWORK_LIST_CELL_SEARCH == sd_act.prm.get_net_gw.network_list_type )
  {
    cmregprx_stack_info_ptr->list_search_info.list_search_type =
      REG_CM_NETWORK_LIST_REQ_CELL_SEARCH;

    reg_ptr->cmd.cm_network_list_req.list_search_type =
      REG_CM_NETWORK_LIST_REQ_CELL_SEARCH;
    reg_ptr->cmd.cm_network_list_req.cell_search_num_cells = sd_act.prm.get_net_gw.cell_search_num_cells;
  }
  else if(SD_NETWORK_LIST_GPS_ADV_SEARCH == sd_act.prm.get_net_gw.network_list_type)
  {
     cmregprx_stack_info_ptr->list_search_info.list_search_type =
      REG_CM_NETWORK_LIST_REQ_GPS_ADVANCE_SEARCH;

    reg_ptr->cmd.cm_network_list_req.list_search_type =
      REG_CM_NETWORK_LIST_REQ_GPS_ADVANCE_SEARCH;
    reg_ptr->cmd.cm_network_list_req.gps_wwan_search_info =sd_act.prm.get_net_gw.gps_wwan_scan_params;
	reg_ptr->cmd.cm_network_list_req.gsp_wwan_scan_mask = cmph_ptr()->gps_wwan_scan_act_mask;
  }
  else
  {
  /* Set list_search_type. To be featurized ??
  */

  cmregprx_stack_info_ptr->list_search_info.list_search_type =
                               REG_CM_NETWORK_LIST_REQ_PLMN_LIST_SEARCH;
  reg_ptr->cmd.cm_network_list_req.list_search_type =
                               REG_CM_NETWORK_LIST_REQ_PLMN_LIST_SEARCH;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* Passing down band pref to stack
  */
  #ifdef FEATURE_GSM_BAND_PREF
  reg_ptr->cmd.cm_network_list_req.band_pref =
                    (sys_band_mask_type)sd_act.prm.get_net_gw.gw_band_pref;
  #endif
  reg_ptr->cmd.cm_network_list_req.req_service_domain =
                                      cmregprx_map_sd_srv_domain_pref_to_sys
                                      (
                                        sd_act.prm.get_net_gw.srv_domain_pref
                                      );
  #ifdef FEATURE_RAT_PRIORITY_LIST
  reg_ptr->cmd.cm_network_list_req.rat_pri_list_info = sd_act.prm.get_net_gw.rat_pri_list_info;
  //reg_ptr->cmd.cm_network_list_req.mcc_mode_band_filter = sd_act.prm.get_net_gw.mcc_mode_band_filter;
  #endif

  reg_ptr->cmd.cm_network_list_req.transaction_id =
                                      cmregprx_next_trans_id
                                      (
                                        cmregprx_stack_info_ptr
                                      );

  /* Fill active subscription id */
  #if !(defined(FEATURE_MMODE_LTE_ONLY) || defined(FEATURE_MMODE_GPRS_LTE) || defined(FEATURE_CATM1_DISABLE))
  reg_ptr->cmd.cm_network_list_req.as_id = cmregprx_stack_info_ptr->as_id;
  reg_ptr->cmd.cm_network_list_req.stack_id = cmregprx_stack_info_ptr->nas_stack_id;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print debug stuff.
  */
  CMREGPRX_MSG_HIGH_3("Send NETWORK_LIST_REQ mode_pref %d, srv_domain %d cell_search_num_cells %d",
                        reg_ptr->cmd.cm_network_list_req.mode_pref, reg_ptr->cmd.cm_network_list_req.req_service_domain,
                        reg_ptr->cmd.cm_network_list_req.cell_search_num_cells);

  #ifdef FEATURE_GSM_BAND_PREF
  CMREGPRX_MSG_HIGH_1("band_pref 0%x ",
                    reg_ptr->cmd.cm_network_list_req.band_pref);
  #endif
  
  CMREGPRX_MSG_HIGH_1("nw_list_req %d ",
                    reg_ptr->cmd.cm_network_list_req.list_search_type);
  #ifdef FEATURE_RAT_PRIORITY_LIST
  CMREGPRX_MSG_HIGH_1("rat_pri_list num = %d",
                    reg_ptr->cmd.cm_network_list_req.rat_pri_list_info.num_items);
 
  #endif

#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH_TEST

  if(reg_ptr->cmd.cm_network_list_req.gps_wwan_search_info.assist_param.rat == SYS_RAT_LTE_M1_RADIO_ACCESS)
  {
    CMREGPRX_MSG_HIGH_1(" M1 sending The no of element is %d ",reg_ptr->cmd.cm_network_list_req.gps_wwan_search_info.assist_param.wwan_rat_scan_param.m1_scan_params.num_m1_ad_element);
        num_ad = reg_ptr->cmd.cm_network_list_req.gps_wwan_search_info.assist_param.wwan_rat_scan_param.m1_scan_params.num_m1_ad_element;
  }
  if(reg_ptr->cmd.cm_network_list_req.gps_wwan_search_info.assist_param.rat == SYS_RAT_LTE_NB1_RADIO_ACCESS)
  {
    CMREGPRX_MSG_HIGH_1(" NB1 sending The no of element is %d ",reg_ptr->cmd.cm_network_list_req.gps_wwan_search_info.assist_param.wwan_rat_scan_param.nb1_scan_params.num_nb1_ad_element);
      num_ad = reg_ptr->cmd.cm_network_list_req.gps_wwan_search_info.assist_param.wwan_rat_scan_param.nb1_scan_params.num_nb1_ad_element;
  }
  if(reg_ptr->cmd.cm_network_list_req.gps_wwan_search_info.assist_param.rat == SYS_RAT_GSM_RADIO_ACCESS)
  {
    CMREGPRX_MSG_HIGH_1(" GSM sending The no of element is %d ",reg_ptr->cmd.cm_network_list_req.gps_wwan_search_info.assist_param.wwan_rat_scan_param.gsm_params.num_gsm_ad_element);
      num_ad = reg_ptr->cmd.cm_network_list_req.gps_wwan_search_info.assist_param.wwan_rat_scan_param.gsm_params.num_gsm_ad_element;
  }
  if (num_ad > SYS_MAX_EARFCN_SCAN_REQ)
  {
    if(cm_get_ulog_handle() != NULL)
    {
      ULOG_RT_PRINTF_2(cm_get_ulog_handle(), "=CM= Net list request  - num ad element = :%d - rat = :%d ",
                                                   num_ad,reg_ptr->cmd.cm_network_list_req.gps_wwan_search_info.assist_param.rat );
    }
    CMREGPRX_ERR_FATAL_0(" netlist crash");
  }
  
#endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the command to REG.
  */
  cm_reg_send_cmd( reg_ptr );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set the RegProxy's substate to waiting for network list confirmation.
  */
  cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_NET_LIST_CNF,
                            cmregprx_stack_info_ptr
                          );

  return;

} /* cmregprx_send_reg_net_list_req() */




/*===========================================================================

FUNCTION cmregprx_send_stop_mode_req

DESCRIPTION
  This function sends STOP_MODE_REQ.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_send_stop_mode_req
(
        sys_stop_mode_reason_e_type     reason,
          /* Reason for deactivating REG.
          */

        cmregprx_stack_info_s_type      *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{

  boolean is_reason_switch = FALSE;
  /* Allocate a REG command buffer
  */
  reg_cmd_type *reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_STOP_MODE_REQ );
  /*lint -esym(550, act)*/

  cmregprx_stack_info_s_type   *cmregprx_stack_info_ptr2  = NULL;
  sys_modem_as_id_e_type        as_id = SYS_MODEM_AS_ID_1;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we are stopping the stack with reasons other than the ALT_SCAN,
  ** reset the flags that indicate that we are not in LTE connected mode
  */
  #if defined (FEATURE_CM_LTE)
  if ( reason != SYS_STOP_MODE_REASON_DSDS_SUSPEND)
  {
    #if defined(FEATURE_CM_LTE)
    if (cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
    {
      cmregprx_stack_info_ptr->lte_connected_mode = FALSE;
      /* This SD indication clears the SD flags */
      CMREGPRX_MSG_HIGH_1("reset L conn state %d",reason);
      sd_ss_reset_rlf_conn_mode_status(SD_SS_MAIN);
    }
    #endif

  }
  #endif /* FEATURE_CM_LTE) ||  (FEATURE_WRLF_SYSTEM_SEL)*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Send STOP_MODE_REQ to REG.
  */
  reg_ptr->cmd.cm_stop_mode_req.stop_mode_reason = reason;

  reg_ptr->cmd.cm_stop_mode_req.transaction_id   =
                             cmregprx_next_trans_id
                             (
                               cmregprx_stack_info_ptr
                             );

  #if !(defined(FEATURE_MMODE_LTE_ONLY) || defined(FEATURE_MMODE_GPRS_LTE))
  reg_ptr->cmd.cm_stop_mode_req.as_id = cmregprx_stack_info_ptr->as_id;
  reg_ptr->cmd.cm_stop_mode_req.stack_id = cmregprx_stack_info_ptr->nas_stack_id;
  #endif
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print debug stuff.
  */
  CMREGPRX_MSG_HIGH_1("Send STOP_MODE_REQ, reason=%d",
                        reg_ptr->cmd.cm_stop_mode_req.stop_mode_reason );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command to REG.
  */
  cm_reg_send_cmd( reg_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmregprx_stack_info_ptr->stop_mode_reason = reason;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If stop mode req is due to LPM, reset the srv req counter
  */
  if (cmregprx_stack_info_ptr->stop_mode_reason ==
                                     SYS_STOP_MODE_REASON_LPM)
  {

    cmregprx_stack_info_ptr->cnt_cm_srv_req = 0;
    
    CMREGPRX_MSG_HIGH_0 ("LPM. cnt_cm_srv_req set to 0");
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Set the RegProxy's substate to waiting for stop mode confirmation.
  */
    cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_STOP_CNF,
                              cmregprx_stack_info_ptr
                            );

  return;
} /* cmregprx_send_stop_mode_req */

/*===========================================================================

FUNCTION cmregprx_send_reg_stop_req

DESCRIPTION
  This function sends the cm_stop_req to REG.
  This will update the substate upon sending the REG request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_send_reg_stop_req
(
       sys_stop_mode_reason_e_type     reason,
          /* Reason for deactivating REG.
          */

       cmregprx_stack_info_s_type      *cmregprx_stack_info_ptr,
          /* Pointer to RegProxy's information.
          */

       prot_trans_type                 mmoc_trans_id
          /* Trans_id for the MMoC's request, can handle only one request
          ** at a time.
          */
)
{

  #ifdef FEATURE_TC
  /* Pointer to test Control command buffer
  */
  tc_cmd_type   *tc_ptr  = cm_tc_get_buf_else_err_fatal( CMTC_STOP_MODE_REQ );
  #endif /* FEATURE_TC */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset the camped_ind buffer.
  */

  if (cmregprx_stack_info_ptr->camped_ind_buf  != NULL)
  {
    cm_mem_free ( cmregprx_stack_info_ptr->camped_ind_buf );
    cmregprx_stack_info_ptr->camped_ind_buf  = NULL;
  }
  cmregprx_stack_info_ptr->last_srv_req.message_header.message_id =
                                                      (byte)CM_REG_CMD_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Save the MMoC's trans_id.
  */
  cmregprx_stack_info_ptr->mmoc_trans_id = mmoc_trans_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Shutdown TC.
  */
  #ifdef FEATURE_TC
  tc_ptr->cmd.cm_stop_mode_req.stop_mode = CMTC_UE_POWER_OFF;

  cm_tc_send_cmd( tc_ptr );
  #endif /* FEATURE_TC */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Just as a precaution send a ABORT request. No need to wait for abort
  ** confirmation. CM waits for abort confirmation during oprt_mode change.
  */
  #if defined(FEATURE_MMODE_GW) && !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
  {
    cmregprx_send_abort_cc_req( reason, SYS_MODEM_AS_ID_1);
  }
  #endif
  
  /* Send STOP_MODE_REQ to NAS. */
  cmregprx_send_stop_mode_req(reason, cmregprx_stack_info_ptr);

  return;

} /* cmregprx_send_reg_stop_req() */
/*lint +esym(550, act)*/


/*===========================================================================

FUNCTION cmregprx_send_reg_sim_avail_req

DESCRIPTION
  This function sends the cm_sim_avail_req to REG.
  This will update the substate upon sending the REG request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_send_reg_sim_avail_req
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr,
           /* Pointer to the command event that was received.
          */

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{
  /* REG command buffer.
  */
  reg_cmd_type *reg_ptr;
  cm_subs_avail_e_type gwl_subs_avail = cmd_ptr->param.gw_ph_stat_cmd.is_gwl_subs_avail;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Determine if cm_sim_available_req or cm_sim_not_available_req has to
    ** sent.
    */
    if ( CM_SUBS_NOT_AVAIL != gwl_subs_avail )
    {
      /* cm_sim_available_req command should be sent. Fill in the fields.
      */
      reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_SIM_AVAILABLE_REQ );
      reg_ptr->cmd.cm_sim_available_req.transaction_id =
                                    cmregprx_next_trans_id( cmregprx_stack_info_ptr );
	    
      reg_ptr->cmd.cm_sim_available_req.mode_pref =
              cmd_ptr->param.gw_ph_stat_cmd.mode_pref;
      
      CMREGPRX_MSG_HIGH_2("Send SIM_AVAILABLE_REQ, gwl_subs_avail %d, mode_pref %d",
        gwl_subs_avail, 
        reg_ptr->cmd.cm_sim_available_req.mode_pref);

    }
    else
    {
      /* cm_sim_not_available_req command should be sent. Fill in the fields.
      */
      reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_SIM_NOT_AVAILABLE_REQ );
      reg_ptr->cmd.cm_sim_not_available_req.transaction_id =
                                   cmregprx_next_trans_id( cmregprx_stack_info_ptr );
    }


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Post the command.
    */
    cm_reg_send_cmd( reg_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Set the RegProxy's substate to waiting for sim availability
    ** confirmation.
    */
    cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_SIM_AVL_CNF,
                              cmregprx_stack_info_ptr
                            );

  return;

} /* cmregprx_send_reg_sim_avail_req() */


/*===========================================================================

FUNCTION cmregprx_send_reg_ph_status_chgd_req

DESCRIPTION
  This function sends the cm_ph_status_chgd_req to REG and update the
  substate upon sending the REG request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_send_reg_ph_status_chgd_req
(
  const cmregprx_cmd_msg_s_type    *cmd_ptr,
     /* Pointer to the command event that was received.
           */

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{
  /* REG command buffer.
  */
  reg_cmd_type *reg_ptr;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* cm_ph_status_chgd_req command should be sent to REG.
    */
    reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_PH_STATUS_CHGD_REQ );
    reg_ptr->cmd.cm_ph_status_chgd_req.transaction_id =
                              cmregprx_next_trans_id( cmregprx_stack_info_ptr );
    reg_ptr->cmd.cm_ph_status_chgd_req.oprt_mode =
                                cmregprx_map_prot_oprt_mode_to_sys(
                                            cmd_ptr->param.gw_ph_stat_cmd.oprt_mode);

    CMREGPRX_MSG_HIGH_1("Send CM_PH_STATUS_CHGD_REQ oprt_mode = %d", \
                          reg_ptr->cmd.cm_ph_status_chgd_req.oprt_mode);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Post the command.
    */
    cm_reg_send_cmd( reg_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Set the RegProxy's substate to waiting for ph status chgd
    ** confirmation.
    */
    cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_PH_STATUS_CHGD_CNF,
                              cmregprx_stack_info_ptr
                            );

  return;

} /* cmregprx_send_reg_ph_status_chgd_req() */


#ifdef FEATURE_LTE
/*===========================================================================

FUNCTION cmregprx_send_ps_detach_req

DESCRIPTION
  This function sends the cm_ps_detach_req to REG.
  This will update the substate upon sending the REG request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_send_ps_detach_req
(

       sys_ps_detach_e_type                     type,

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{
  /* REG command buffer.
  */
  reg_cmd_type *reg_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* cm_ps_detach_req command should be sent. Fill in the fields.
  */
  reg_ptr = cm_reg_get_buf_else_err_fatal( CM_PS_DETACH_REQ );
  reg_ptr->cmd.cm_ps_detach_req.detach_type = type;

  CMREGPRX_MSG_HIGH_1("Send PS_DETACH_REQ type:%d",
                       type );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command.
  */
  cm_reg_send_cmd( reg_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set the RegProxy's substate to waiting for sim availability
  ** confirmation.
  */
  cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_PS_DETACH_CNF,
                            cmregprx_stack_info_ptr
                          );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset the SERVICE_REQ cnt within 2 seconds
  */
  if ( cmregprx_stack_info_ptr->cnt_cm_srv_req > CMREGPRX_MAX_SRV_CNF_REQ )
  {
    cmregprx_stack_info_ptr->cnt_cm_srv_req          = 0;

    cmregprx_stack_info_ptr->last_srv_req.message_header.message_id =
                                                 (byte)CM_REG_CMD_NONE;
    cmregprx_stack_info_ptr->last_srv_req.parameters.automatic.type = REG_CM_SERVICE_REQ_NORMAL;
    cmregprx_stack_info_ptr->last_srv_req.parameters.manual.type    = REG_CM_SERVICE_REQ_NORMAL;
  }

  return;

} /* cmregprx_send_ps_detach_req() */
#endif

/*===========================================================================

FUNCTION cmregprx_send_exit_from_pwr_save_ind

DESCRIPTION
  This function sends the cm_exit_from_pwr_save  to REG.
  CMREGPRXY substate will not change with this ind.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void               cmregprx_send_exit_from_pwr_save_ind
(
  sd_ss_e_type  ss
)
{
  #ifdef FEATURE_OOSC_USER_ACTION
  cmregprx_stack_info_s_type* cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(ss);
  /* Allocate a REG command buffer
  */
  reg_cmd_type *reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_EXIT_FROM_PWR_SAVE );

  /* Print debug stuff.
  */
  CMREGPRX_MSG_HIGH_0("Send CM_EXIT_FROM_PWR_SAVE");

  /* Post the command to REG.
  */
  cm_reg_send_cmd( reg_ptr );
  return;
  #else
  CMREGPRX_MSG_HIGH_0("FEATURE_OOSC_USER_ACTION not defined");
  #endif /* #if defined (FEATURE_OOSC_USER_ACTION) */
}

/*===========================================================================

FUNCTION cmregprx_map_srv_cnf_to_sd_ind

DESCRIPTION
  This function will call the appropriate SD indication based on the service
  state info.

  Note! This function should be called only when the report is CM_SERVICE_CNF

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static sd_ss_act_e_type                cmregprx_map_srv_cnf_to_sd_ind
(

       sys_plmn_service_state_s_type   *srv_info_ptr,
       /* Pointer to the service state info received in the report.
           */

       reg_cm_scan_status_e_type        scan_status,
       /* The scan status returned by Reg
           */

       cmregprx_stack_info_s_type      *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
/*lint -esym(818,cmregprx_stack_info_ptr) */
{
  sd_ss_act_e_type sd_act = SD_SS_ACT_CONTINUE;
  sd_si_gwl_mode_info_s_type  lac_tac_rac;
  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  sys_sim_state_e_type        curr_sim_state;
  cmregprx_sim_state_update_s_type  *cmregprx_sim_state_update_ptr = cmregprx_get_sim_state_update_info();
  #endif
  #ifdef FEATURE_CM_LTE
  sd_mode_e_type              mode;
  #endif

  sys_neg_ciot_cap  neg_ciot_cap;
  boolean epco_supported = FALSE;
  sys_cp_sms_service_status_e_type cp_sms_service_status = SYS_CP_SMS_STATUS_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );
  CM_ASSERT( srv_info_ptr != NULL );

  #ifdef FEATURE_FEMTO_CSG
  CMREGPRX_MSG_HIGH_3("cmregprx_map_srv_cnf_to_sd_ind(): scan_status = %d, CSG_ID = %d active_rat %d",
                      scan_status, srv_info_ptr->csg_info.csg_id,
                      srv_info_ptr->active_rat);
  #endif
  
  memset(&neg_ciot_cap, 0, sizeof(sys_neg_ciot_cap));
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmss_report_raw_3gpp_service(srv_info_ptr->service_status, srv_info_ptr->reg_service_domain,
                                      srv_info_ptr->active_rat);

  /** If a sub is perso locked, we can set sim_state as NOT_AVAIL, 
  ** as sim is effectively not present
  */ 
  if(cmregprx_stack_info_ptr->is_gwl_subs_avail == CM_SUBS_PERSO_LOCK)
  {
     srv_info_ptr->sim_state = SYS_SIM_STATE_NOT_AVAILABLE;
  } 

  /* Switch on the service status.
  */
  switch ( srv_info_ptr->service_status )

  {
    case SYS_SRV_STATUS_NO_SRV:
      /* No service was reported.
      */
     #if defined (FEATURE_CM_LTE ) ||  defined (FEATURE_WRLF_SYSTEM_SEL)
      /* To notify DS the target RAT info so DS will do ATTACH */
      if(srv_info_ptr->no_service_cause == SYS_NO_SVC_CAUSE_IRAT_TO_LTE)
      {
        cmss_inform_target_rat(SYS_SYS_MODE_LTE);
      }
      else
      {
        /* reset target RAT info to None when LTE attach fail */
        cmss_inform_target_rat(SYS_SYS_MODE_NONE);
      }

      if ((srv_info_ptr->no_service_cause == SYS_NO_SVC_CAUSE_RLF) &&
          (srv_info_ptr->suitable_search_time != 0))
      {
        #ifdef FEATURE_CM_LTE
        /* Verify if the loss of service is due to RLF (LTE only). This
        ** would indicate that the system is in LTE connected mode.
        */
        if( (srv_info_ptr->active_rat == SYS_RAT_LTE_RADIO_ACCESS)
            #if defined(FEATURE_CIOT)
            || (srv_info_ptr->active_rat == SYS_RAT_LTE_M1_RADIO_ACCESS) 
            || (srv_info_ptr->active_rat == SYS_RAT_LTE_NB1_RADIO_ACCESS)
            #endif
          )
        {
          if (cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
          {
            CMREGPRX_MSG_HIGH_1("LTE connected Mode at CM_SERVICE_CNF : repeated, T311 is : %d",
                  srv_info_ptr->suitable_search_time);
            #if defined(FEATURE_MMODE_9X05_EMERG_CALL)
            sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
            #endif
            sd_act = sd_ss_ind_gw_acq_plmn_failed_ss_2
                     (
                       srv_info_ptr->sim_state,
                       srv_info_ptr->ps_data_suspend,
                       cmregprx_map_reg_scan_status_sd_scan_status( scan_status ),
                       srv_info_ptr->acq_status,
                       SD_SS_MAIN,
                       #ifdef FEATURE_CM_LTE
                       srv_info_ptr->lte_cs_capability
                       #else
                       SYS_LTE_CS_CAPABILITY_NONE
                       #endif
                       ,cmregprx_stack_info_ptr->sys_sel_pref_req_id,
                       srv_info_ptr->extend_srv_info,
                       #ifdef FEATURE_FEMTO_CSG
                       &srv_info_ptr->csg_info,
                       #endif
                       &srv_info_ptr->registered_plmn,
                       NULL
                     );
          }
          else
          {
             if(srv_info_ptr->sim_state != SYS_SIM_STATE_PS_INVALID &&
        srv_info_ptr->sim_state != SYS_SIM_STATE_CS_PS_INVALID)
             {
               cmregprx_stack_info_ptr->lte_connected_mode = TRUE;
             }
            else
             {
               srv_info_ptr->suitable_search_time = 0;
             }
            
             CMREGPRX_MSG_HIGH_3("LTE Connected Mode set at CM_SERVICE_CNF, T311 is : %d, sim_state=%d, acq_state=%d",
                srv_info_ptr->suitable_search_time,srv_info_ptr->sim_state, srv_info_ptr->acq_status);
             #if defined(FEATURE_MMODE_9X05_EMERG_CALL)
             sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
             #endif
             sd_act = sd_ss_ind_lte_opr_srv_lost_1
               (
                 srv_info_ptr->sim_state,
                 srv_info_ptr->ps_data_suspend,
                 NULL,
                 srv_info_ptr->suitable_search_time,
                 srv_info_ptr->acq_status,
                 SD_SS_MAIN,
                 NULL
               );

             if(cmregprx_stack_info_ptr->sys_sel_pref_req_id > 0 )
             {
               /* Update SD with sys_sel_pref_req_id in stack info so that clients get notified aout the serving status */
               sd_ss_ind_misc_update_req_id(SD_SS_MAIN, 
                                               cmregprx_stack_info_ptr->sys_sel_pref_req_id);
             }
          }
        }
        #endif /* FEATURE_CM_LTE */

      }
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      else
      #endif  /*  (FEATURE_LTE ) ||  (FEATURE_WRLF_SYSTEM_SEL) */
      {
        #ifdef FEATURE_EOOS
        #error code not present
#endif

        #if defined(FEATURE_MMODE_9X05_EMERG_CALL)
        sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
        #endif
        
        sd_act = sd_ss_ind_gw_acq_plmn_failed_ss_2
                 (
                   srv_info_ptr->sim_state,
                   srv_info_ptr->ps_data_suspend,
                   cmregprx_map_reg_scan_status_sd_scan_status( scan_status ),
                   srv_info_ptr->acq_status,
                   SD_SS_MAIN,
                   #ifdef FEATURE_CM_LTE
                   srv_info_ptr->lte_cs_capability,
                   #else
                   SYS_LTE_CS_CAPABILITY_NONE,
                   #endif
                   cmregprx_stack_info_ptr->sys_sel_pref_req_id,
                   srv_info_ptr->extend_srv_info,
                   #ifdef FEATURE_FEMTO_CSG
                   &srv_info_ptr->csg_info,
                   #endif
                   &srv_info_ptr->registered_plmn,
                   NULL
                 );

      }

      break;

    case SYS_SRV_STATUS_LIMITED:
    case SYS_SRV_STATUS_LIMITED_REGIONAL:
    case SYS_SRV_STATUS_SRV:
      /* If the crrent sim state is CS/PS invalid and in service cnf, if the sim state changes
      ** to AVAILABLE, a new service request will be sent.
      ** During call origination, if a service request is sent to NAS and service cnf
      ** is received with sim state AVAILBLE, do not send a service request immediately.
      ** Buffer the new sim state and notify SD when the call is deleted from priority queue.
      */	
      #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
      curr_sim_state = cmss_get_gw_sim_state_ss(CM_SS_MAIN);
	    
      if ( (curr_sim_state == SYS_SIM_STATE_PS_INVALID ||
            curr_sim_state == SYS_SIM_STATE_CS_INVALID) &&
           (srv_info_ptr->sim_state == SYS_SIM_STATE_AVAILABLE) )
      {	 
         /* If there is a voice call or emergency call, then buffer the
         ** sim_state_available from NAS.
         */
      if((cmcall_is_there_a_call_type_per_sub(SYS_MODEM_AS_ID_1,
          CM_CALL_TYPE_VOICE, NULL)!= CM_CALL_ID_INVALID) ||
         (cmcall_is_there_a_call_type_per_sub(SYS_MODEM_AS_ID_1,
          CM_CALL_TYPE_EMERGENCY, NULL)!= CM_CALL_ID_INVALID))
         {
 
           cmregprx_sim_state_update_ptr->update_pending = TRUE;
           cmregprx_sim_state_update_ptr->sim_state = srv_info_ptr->sim_state;
           srv_info_ptr->sim_state = curr_sim_state;
         }

      }	  
      #endif
      
      if(cm_get_ulog_handle() != NULL)
      {
        ULOG_RT_PRINTF_1(cm_get_ulog_handle(), "=CM= Service confirmation from NAS, service status:%d",
                                                           srv_info_ptr->service_status);
      }
      /* Some service was reported from REG.
      **
      ** Populate LAC, band and channel from REG.
      **
      ** Fill dummy value with LAC, band and chan field until STACK is ready.
      **
      */
      /* Reset reject value*/
      if(srv_info_ptr->service_status == SYS_SRV_STATUS_SRV)
      {
        cmss_clear_reg_reject_info(srv_info_ptr , SD_SS_MAIN);
      }

        /*
        **  Reset the LTE connected mode flag if it was set
        */
      #ifdef FEATURE_CM_LTE
        if (cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
        {
             cmregprx_stack_info_ptr->lte_connected_mode = FALSE;
        }
      #endif


      #ifdef FEATURE_CM_LTE
      if( (srv_info_ptr->active_rat == SYS_RAT_LTE_RADIO_ACCESS)
          #if defined(FEATURE_CIOT)
          || (srv_info_ptr->active_rat == SYS_RAT_LTE_M1_RADIO_ACCESS)
          || (srv_info_ptr->active_rat == SYS_RAT_LTE_NB1_RADIO_ACCESS)
          #endif
        )
      {
        sd_band_u_type local_band_pref;
        local_band_pref.lte_band    = srv_info_ptr->active_band.lte_active_band;


        lac_tac_rac.lac             = CMSS_INVALID_LAC_INFO;
        lac_tac_rac.tac             = srv_info_ptr->tac;
        lac_tac_rac.rac_or_mme_code = srv_info_ptr->rac_or_mme_code;

        #if defined(FEATURE_MMODE_9X05_EMERG_CALL)
        sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
        #endif

        mode = cmregprx_map_sys_rat_to_sd_mode(srv_info_ptr->active_rat);
        #if defined(FEATURE_CIOT)
        if ((mode == SD_MODE_LTE_M1) ||
            (mode == SD_MODE_LTE_NB1))
        {
          mode = SD_MODE_LTE;
        }
        
        neg_ciot_cap.cp_ciot_supported = srv_info_ptr->eps_nw_feature_support.cp_ciot_supported;
        neg_ciot_cap.up_ciot_supported = srv_info_ptr->eps_nw_feature_support.up_ciot_supported;
        
        neg_ciot_cap.esr_without_pdn_supported = srv_info_ptr->eps_nw_feature_support.er_without_pdn_supported;
        neg_ciot_cap.hc_cp_ciot_supported = srv_info_ptr->eps_nw_feature_support.hc_cp_ciot_supported;
        neg_ciot_cap.s1_u_data_supported = srv_info_ptr->eps_nw_feature_support.s1_u_data_supported;
        cp_sms_service_status = srv_info_ptr->cp_sms_service_status;
        #endif
        epco_supported = srv_info_ptr->eps_nw_feature_support.epco_supported;

        sd_act = sd_ss_ind_gw_acq_plmn_success_ss_2
              (
                srv_info_ptr->plmn,
                lac_tac_rac,
                local_band_pref,
                0,
                srv_info_ptr->plmn_service_capability,
                srv_info_ptr->plmn_forbidden,
                srv_info_ptr->sim_state,
                srv_info_ptr->service_status,
                srv_info_ptr->roaming_ind,
                srv_info_ptr->reg_service_domain,
                mode,
                srv_info_ptr->ps_data_suspend,
                srv_info_ptr->cs_service_status,
                SD_SS_MAIN,
                srv_info_ptr->lte_cs_capability,
                #if defined(FEATURE_MMODE_3GPP2) || defined(SD_DEBUG)
                srv_info_ptr->sib8_available,
                #endif
                srv_info_ptr->eps_nw_feature_support.ims_vops_supported,
                srv_info_ptr->eps_nw_feature_support.emc_bs_supported,
                cmregprx_stack_info_ptr->sys_sel_pref_req_id,
                srv_info_ptr->extend_srv_info,
                srv_info_ptr->plmn_reg_type,
                #ifdef FEATURE_FEMTO_CSG
                &srv_info_ptr->csg_info,
                #endif
                &srv_info_ptr->registered_plmn,
                cmregprx_map_sys_rat_to_sd_mode(srv_info_ptr->active_rat),
                neg_ciot_cap,
                epco_supported,
                cp_sms_service_status,
                NULL
              );
    /* reset target RAT info to None when LTE acquired successfully */
        if(srv_info_ptr->service_status == SYS_SRV_STATUS_SRV)
        {
          cmss_inform_target_rat(SYS_SYS_MODE_NONE);
        }

        #ifdef FEATURE_LTE_TO_HDR_OH
        /* Activate HDR tunnel when get LTE full service. This means attach_accept with full service
        ** at NAS side. It is to handle case when LTE lose service then reacquire service but NAS did
        ** not perform re-attach case.
        */
        if(srv_info_ptr->service_status == SYS_SRV_STATUS_SRV)
        {
          cmsds_s_type *sds_ptr = cmsds_ptr();

          if(sds_ptr->is_act_hdr_on_srv_cnf)
          {
            cmsds_activate_hdr_tunnel_req();
          }
        }
        #endif
      }
      else
      #endif /* FEATURE_CM_LTE */
      {
        #ifdef FEATURE_MMODE_GW
        sd_band_u_type active_band ;
      #ifdef FEATURE_GSM_BAND_PREF
        lac_tac_rac.lac             = srv_info_ptr->lac;
        #else
        lac_tac_rac.lac             = 0xffff;
        #endif
        lac_tac_rac.tac             = CMSS_INVALID_TAC_INFO;
        lac_tac_rac.rac_or_mme_code = srv_info_ptr->rac_or_mme_code;

        #ifdef FEATURE_GSM_BAND_PREF
        active_band = cmregprx_get_active_band(srv_info_ptr->active_rat, srv_info_ptr->active_band);
        #else
        active_band.chgwt_band = SD_SS_BAND_PREF_GSM_DCS_1800;
        #endif
       
        #if defined(FEATURE_MMODE_9X05_EMERG_CALL)
        sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
        #endif
        
        #if defined(FEATURE_CIOT)
        neg_ciot_cap.cp_ciot_supported = srv_info_ptr->eps_nw_feature_support.cp_ciot_supported;
        neg_ciot_cap.up_ciot_supported = srv_info_ptr->eps_nw_feature_support.up_ciot_supported;
        
        neg_ciot_cap.esr_without_pdn_supported = srv_info_ptr->eps_nw_feature_support.er_without_pdn_supported;
        neg_ciot_cap.hc_cp_ciot_supported = srv_info_ptr->eps_nw_feature_support.hc_cp_ciot_supported;
        neg_ciot_cap.s1_u_data_supported = srv_info_ptr->eps_nw_feature_support.s1_u_data_supported;
        cp_sms_service_status = srv_info_ptr->cp_sms_service_status;
        #endif
        epco_supported = srv_info_ptr->eps_nw_feature_support.epco_supported;

        sd_act = sd_ss_ind_gw_acq_plmn_success_ss_2
                 (
                   srv_info_ptr->plmn,
                   lac_tac_rac,
                   active_band,
                   0, /* dummy channel */
                   srv_info_ptr->plmn_service_capability,
                   srv_info_ptr->plmn_forbidden,
                   srv_info_ptr->sim_state,
                   srv_info_ptr->service_status,
                   srv_info_ptr->roaming_ind,
                   srv_info_ptr->reg_service_domain,
                   cmregprx_map_sys_rat_to_sd_mode
                   (
                     srv_info_ptr->active_rat
                   ),
                   srv_info_ptr->ps_data_suspend,
                   srv_info_ptr->cs_service_status,
                   SD_SS_MAIN,
                   SYS_LTE_CS_CAPABILITY_NONE,
                   #if defined(FEATURE_MMODE_3GPP2) || defined(SD_DEBUG)
                   srv_info_ptr->sib8_available,
                   #endif
                   FALSE, //eps_nw_feature_support. ims_vops_supported,
                   FALSE, //eps_nw_feature_support.emc_bs_supported,
                   cmregprx_stack_info_ptr->sys_sel_pref_req_id,
                   srv_info_ptr->extend_srv_info,
                   srv_info_ptr->plmn_reg_type,
                   //srv_info_ptr->reg_service_domain,
                   #ifdef FEATURE_FEMTO_CSG
                   &srv_info_ptr->csg_info,
                   #endif
                   &srv_info_ptr->registered_plmn,
                   cmregprx_map_sys_rat_to_sd_mode(srv_info_ptr->active_rat),
                   neg_ciot_cap,
                   epco_supported,
                   cp_sms_service_status,
                   NULL
                 );

        /* reset target RAT info to None when target RAT is not LTE*/
        cmss_inform_target_rat(SYS_SYS_MODE_NONE);
        #endif
      }

      /* Log reported service */
      cmlog_service_cnf_event( srv_info_ptr, SD_SS_MAIN );
      break;

    case SYS_SRV_STATUS_MAX:
    case SYS_SRV_STATUS_NONE:
    default:
      sys_err_fatal_invalid_value_exception((uint8)(srv_info_ptr->service_status));
      break;

  } /* switch( srv_info_ptr->service_status ) */

  cmregprx_stack_info_ptr->sys_sel_pref_req_id = CM_DEFAULT_VALUE;

  SYS_ARG_NOT_USED(cmregprx_stack_info_ptr);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the SD20 action.
  */
  return sd_act;
} /* cmregprx_map_srv_cnf_to_sd_ind() */
/*lint +esym(818,cmregprx_stack_info_ptr) */

/*===========================================================================

FUNCTION cmregprx_map_srv_ind_to_sd_ind

DESCRIPTION
  This function will call the appropriate SD indication based on the service
  state info.

  Note! This function should be called only when the report is CM_SERVICE_IND

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static sd_ss_act_e_type                cmregprx_map_srv_ind_to_sd_ind
(

       sys_plmn_service_state_s_type   *srv_info_ptr,
          /* Pointer to the service state info received in the report.
          */

       cmregprx_stack_info_s_type      *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
/*lint -esym(818,cmregprx_stack_info_ptr) */
{
  sd_ss_act_e_type sd_act = SD_SS_ACT_CONTINUE;
  sd_si_gwl_mode_info_s_type  lac_tac_rac;
  #ifdef FEATURE_CM_LTE
  sd_mode_e_type              mode;
  #endif
  sys_neg_ciot_cap  neg_ciot_cap;
  boolean epco_supported = FALSE;
  sys_cp_sms_service_status_e_type cp_sms_service_status = SYS_CP_SMS_STATUS_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );
  CM_ASSERT( srv_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_MSG_HIGH_1("active_rat %d", cmregprx_map_sys_rat_to_sd_mode(srv_info_ptr->active_rat));
  
  memset(&neg_ciot_cap, 0, sizeof(sys_neg_ciot_cap));
  cmss_report_raw_3gpp_service(srv_info_ptr->service_status, srv_info_ptr->reg_service_domain,
                                      srv_info_ptr->active_rat);  
  #ifdef FEATURE_CM_LTE
  /* LTE APN timer expire - reattach requirement.
  */
  if (( srv_info_ptr->active_rat == SYS_RAT_LTE_RADIO_ACCESS 
        #if defined(FEATURE_CIOT)
        || (srv_info_ptr->active_rat == SYS_RAT_LTE_M1_RADIO_ACCESS)
        || (srv_info_ptr->active_rat == SYS_RAT_LTE_NB1_RADIO_ACCESS)
        #endif
      ) &&
       srv_info_ptr->service_status == SYS_SRV_STATUS_SRV    &&
       srv_info_ptr->reg_service_domain == SYS_SRV_DOMAIN_CAMPED
    )
  {
    srv_info_ptr->service_status = SYS_SRV_STATUS_NO_SRV;
  }
  #endif
  /** If a sub is perso locked, we can set sim_state as NOT_AVAIL, 
  ** as sim is effectively not present
  */ 
  if(cmregprx_stack_info_ptr->is_gwl_subs_avail == CM_SUBS_PERSO_LOCK)
  {
     srv_info_ptr->sim_state = SYS_SIM_STATE_NOT_AVAILABLE;
  } 

  /* Switch on the service status.
  */
  switch ( srv_info_ptr->service_status )
  {
    case SYS_SRV_STATUS_NO_SRV:
      /* No service was reported.
      */
      #if defined (FEATURE_CM_LTE ) || defined (FEATURE_WRLF_SYSTEM_SEL)

      /* Reset target rat info when LTE attach fail */
      cmss_inform_target_rat(SYS_SYS_MODE_NONE);

      if ((srv_info_ptr->no_service_cause == SYS_NO_SVC_CAUSE_RLF) &&
          (srv_info_ptr->suitable_search_time != 0))
      {

        #ifdef FEATURE_CM_LTE
        /* Verify if the loss of service is due to RLF (LTE only). This
            ** would indicate that the system is in LTE connected mode.
           */
        if ( srv_info_ptr->active_rat == SYS_RAT_LTE_RADIO_ACCESS 
             #if defined(FEATURE_CIOT)
             || (srv_info_ptr->active_rat == SYS_RAT_LTE_M1_RADIO_ACCESS)
             || (srv_info_ptr->active_rat == SYS_RAT_LTE_NB1_RADIO_ACCESS)
             #endif
           )
        {
          if (cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
          {
            CMREGPRX_MSG_HIGH_2("LTE connected Mode at CM_SERVICE_IND : repeated, T311 is : %d acq_status %d",
                  srv_info_ptr->suitable_search_time,srv_info_ptr->acq_status);
            #if defined(FEATURE_MMODE_9X05_EMERG_CALL)
            sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
            #endif
            sd_act = sd_ss_ind_gw_opr_srv_lost_ss_3
               (
                 srv_info_ptr->sim_state,
                 srv_info_ptr->ps_data_suspend,
                     NULL,
                     0,
                     SD_SS_MAIN,
                     srv_info_ptr->acq_status,
                     &srv_info_ptr->registered_plmn
               );
          }
          else
          {
             if(srv_info_ptr->sim_state != SYS_SIM_STATE_PS_INVALID &&
        srv_info_ptr->sim_state != SYS_SIM_STATE_CS_PS_INVALID)
             {
               cmregprx_stack_info_ptr->lte_connected_mode = TRUE;
             }
             else
             {
               srv_info_ptr->suitable_search_time = 0;
             }
             CMREGPRX_MSG_HIGH_3("LTE in Connected Mode at CM_SERVICE_IND, T311: %d, sim_state=%d, acq_state=%d",
                srv_info_ptr->suitable_search_time,srv_info_ptr->sim_state,srv_info_ptr->acq_status );
             #if defined(FEATURE_MMODE_9X05_EMERG_CALL)
             sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
             #endif
             sd_act = sd_ss_ind_lte_opr_srv_lost_1
                                         (
                                           srv_info_ptr->sim_state,
                                           srv_info_ptr->ps_data_suspend,
                                           NULL,
                                           srv_info_ptr->suitable_search_time,
                                           srv_info_ptr->acq_status,
                                           SD_SS_MAIN,
                                           &srv_info_ptr->registered_plmn
                                         );
          }
        }
        #endif /* FEATURE_CM_LTE */

      }
      else
      #endif /* FEATURE_CM_LTE  || FEATURE_WRLF_SYSTEM_SEL */
      {
        CM_MSG_HIGH_1("srv_lost: acq_status %d",
                       srv_info_ptr->acq_status);
        #if defined(FEATURE_MMODE_9X05_EMERG_CALL)
        sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
        #endif
        sd_act = sd_ss_ind_gw_opr_srv_lost_ss_3
               (
                 srv_info_ptr->sim_state,
                 srv_info_ptr->ps_data_suspend,
                 NULL,
                 0,
                 SD_SS_MAIN,
                 srv_info_ptr->acq_status,
                 &srv_info_ptr->registered_plmn
               );
      }

      /* No service is reported, check if calls in silent re-dial should be
      ** updated
      */
      #if defined (FEATURE_LTE_TO_1X) && defined (FEATURE_CM_LTE)
      if (srv_info_ptr->no_service_cause == SYS_NO_SVC_CAUSE_RLF)
      {
        cmwcall_update_calls_on_srv_lost(cmregprx_stack_info_ptr->ss, TRUE);
      }
      else
      {
        cmwcall_update_calls_on_srv_lost(cmregprx_stack_info_ptr->ss, FALSE);
      }
      #else
      #if defined(FEATURE_MMODE_GW)
      cmwcall_update_calls_on_srv_lost(SD_SS_MAIN);
      #endif
      #endif

      break;
    case SYS_SRV_STATUS_LIMITED:
    case SYS_SRV_STATUS_LIMITED_REGIONAL:
    case SYS_SRV_STATUS_SRV:
      /* Some service was reported from REG.
      **
      ** Populate LAC, band and channel from REG.
      **
      ** Feed dummy value with LAC, band and chan field until stack is ready.
      **
      */
      /* Reset reject value*/
      if(srv_info_ptr->service_status == SYS_SRV_STATUS_SRV)
      {
        cmss_clear_reg_reject_info(srv_info_ptr , SD_SS_MAIN);
      }

      #ifdef FEATURE_CM_LTE


	  /*
      **  Reset the LTE connected mode flag if it was set
      */
      if (cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
      {
        cmregprx_stack_info_ptr->lte_connected_mode = FALSE;
      }

      if ( srv_info_ptr->active_rat == SYS_RAT_LTE_RADIO_ACCESS 
           #if defined(FEATURE_CIOT)
           || (srv_info_ptr->active_rat == SYS_RAT_LTE_M1_RADIO_ACCESS)
           || (srv_info_ptr->active_rat == SYS_RAT_LTE_NB1_RADIO_ACCESS)
           #endif
         )
      {
        sd_band_u_type local_band_pref;
        local_band_pref.lte_band = srv_info_ptr->active_band.lte_active_band;


        lac_tac_rac.lac             = CMSS_INVALID_LAC_INFO;
       
        lac_tac_rac.tac             = srv_info_ptr->tac;
        lac_tac_rac.rac_or_mme_code = srv_info_ptr->rac_or_mme_code;
        /*
        **  Reset the LTE connected mode flag if it was set
        */
        if (cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
        {
          cmregprx_stack_info_ptr->lte_connected_mode = FALSE;
        }
        #if defined(FEATURE_MMODE_9X05_EMERG_CALL)
        sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
        #endif

        
        mode = cmregprx_map_sys_rat_to_sd_mode(srv_info_ptr->active_rat);
        #if defined(FEATURE_CIOT)
        if ((mode == SD_MODE_LTE_M1) ||
            (mode == SD_MODE_LTE_NB1))
        {
          mode = SD_MODE_LTE;
        }
        
        neg_ciot_cap.cp_ciot_supported = srv_info_ptr->eps_nw_feature_support.cp_ciot_supported;
        neg_ciot_cap.up_ciot_supported = srv_info_ptr->eps_nw_feature_support.up_ciot_supported;
        
        neg_ciot_cap.esr_without_pdn_supported = srv_info_ptr->eps_nw_feature_support.er_without_pdn_supported;
        neg_ciot_cap.hc_cp_ciot_supported = srv_info_ptr->eps_nw_feature_support.hc_cp_ciot_supported;
        neg_ciot_cap.s1_u_data_supported = srv_info_ptr->eps_nw_feature_support.s1_u_data_supported;
        cp_sms_service_status = srv_info_ptr->cp_sms_service_status;
        #endif
        epco_supported = srv_info_ptr->eps_nw_feature_support.epco_supported;

        sd_act = sd_ss_ind_gw_opr_srv_info_ss_1
                 (
                   srv_info_ptr->plmn,
                   lac_tac_rac,
                   local_band_pref,
                   0,
                   srv_info_ptr->plmn_service_capability,
                   srv_info_ptr->plmn_forbidden,
                   srv_info_ptr->sim_state,
                   srv_info_ptr->service_status,
                   srv_info_ptr->roaming_ind,
                   srv_info_ptr->reg_service_domain,
                   mode,
                   srv_info_ptr->ps_data_suspend,
                   srv_info_ptr->cs_service_status,
                   SD_SS_MAIN,
                   srv_info_ptr->lte_cs_capability,
                   #if defined(FEATURE_MMODE_3GPP2) || defined(SD_DEBUG)
                   srv_info_ptr->sib8_available,
                   #endif
                   srv_info_ptr->eps_nw_feature_support.ims_vops_supported,
                   srv_info_ptr->eps_nw_feature_support.emc_bs_supported,
                   srv_info_ptr->extend_srv_info,
                   srv_info_ptr->plmn_reg_type,
                   #ifdef FEATURE_FEMTO_CSG
                   &srv_info_ptr->csg_info,
                   #endif
                   &srv_info_ptr->registered_plmn,
                   cmregprx_map_sys_rat_to_sd_mode(srv_info_ptr->active_rat),
                   neg_ciot_cap,
                   epco_supported,
                   cp_sms_service_status,
                   NULL
                 );
    /* reset target RAT info to None when LTE acquired successfully */
        if(srv_info_ptr->service_status == SYS_SRV_STATUS_SRV)
        {
          cmss_inform_target_rat(SYS_SYS_MODE_NONE);
        }

        #ifdef FEATURE_LTE_TO_HDR_OH
        /* Activate HDR tunnel when get LTE full service. It is to handle case when LTE get limited srv
        ** in srv cnf followed by full srv in srv_ind
        */
        if(srv_info_ptr->service_status == SYS_SRV_STATUS_SRV)
        {
          cmsds_s_type *sds_ptr = cmsds_ptr();

          if(sds_ptr->is_act_hdr_on_srv_cnf)
          {
            cmsds_activate_hdr_tunnel_req();
          }
        }
        #endif
      }
      else
      #endif /* FEATURE_CM_LTE */
      {

        #ifdef FEATURE_MMODE_GW
        sd_band_u_type active_band;
        #ifdef FEATURE_GSM_BAND_PREF
        lac_tac_rac.lac       = srv_info_ptr->lac;
        #else
        lac_tac_rac.lac       = 0xffff;
        #endif

        lac_tac_rac.tac       = CMSS_INVALID_TAC_INFO;
        lac_tac_rac.rac_or_mme_code = srv_info_ptr->rac_or_mme_code;


        #ifdef FEATURE_GSM_BAND_PREF
        active_band = cmregprx_get_active_band(srv_info_ptr->active_rat, srv_info_ptr->active_band);
        #else
        active_band.chgwt_band = SD_SS_BAND_PREF_GSM_DCS_1800;
        #endif /* FEATURE_GSM_BAND_PREF */
        #if defined(FEATURE_MMODE_9X05_EMERG_CALL)
        sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
        #endif

        
        #if defined(FEATURE_CIOT)
        neg_ciot_cap.cp_ciot_supported = srv_info_ptr->eps_nw_feature_support.cp_ciot_supported;
        neg_ciot_cap.up_ciot_supported = srv_info_ptr->eps_nw_feature_support.up_ciot_supported;
        
        neg_ciot_cap.esr_without_pdn_supported = srv_info_ptr->eps_nw_feature_support.er_without_pdn_supported;
        neg_ciot_cap.hc_cp_ciot_supported = srv_info_ptr->eps_nw_feature_support.hc_cp_ciot_supported;
        neg_ciot_cap.s1_u_data_supported = srv_info_ptr->eps_nw_feature_support.s1_u_data_supported;
        cp_sms_service_status = srv_info_ptr->cp_sms_service_status;
        #endif
        epco_supported = srv_info_ptr->eps_nw_feature_support.epco_supported;

        sd_act = sd_ss_ind_gw_opr_srv_info_ss_1
                 (
                   srv_info_ptr->plmn,
                   lac_tac_rac,
                   active_band,
                   0, /* dummy channel*/
                   srv_info_ptr->plmn_service_capability,
                   srv_info_ptr->plmn_forbidden,
                   srv_info_ptr->sim_state,
                   srv_info_ptr->service_status,
                   srv_info_ptr->roaming_ind,
                   srv_info_ptr->reg_service_domain,
                   cmregprx_map_sys_rat_to_sd_mode
                   (
                     srv_info_ptr->active_rat
                   ),
                   srv_info_ptr->ps_data_suspend,
                   srv_info_ptr->cs_service_status,
                   SD_SS_MAIN,
                   SYS_LTE_CS_CAPABILITY_NONE,
                   #if defined(FEATURE_MMODE_3GPP2) || defined(SD_DEBUG)
                   srv_info_ptr->sib8_available,
                   #endif
                   FALSE, //eps_nw_feature_support. ims_vops_supported
                   FALSE,//eps_nw_feature_support.emc_bs_supported
                   srv_info_ptr->extend_srv_info,
                   srv_info_ptr->plmn_reg_type,
                   //srv_info_ptr->reg_service_domain,
                   #ifdef FEATURE_FEMTO_CSG
                   &srv_info_ptr->csg_info,
                   #endif
                   &srv_info_ptr->registered_plmn,
                   cmregprx_map_sys_rat_to_sd_mode(srv_info_ptr->active_rat),
                   neg_ciot_cap,
                   epco_supported,
                   cp_sms_service_status,
                   NULL
                 );

        /* reset target RAT info to None when target RAT is not LTE*/
        cmss_inform_target_rat(SYS_SYS_MODE_NONE);
        #endif
      }

      #if defined (FEATURE_LTE_TO_1X) && defined (FEATURE_LTE)
      if((srv_info_ptr->service_status == SYS_SRV_STATUS_LIMITED_REGIONAL) &&
         (srv_info_ptr->active_rat == SYS_RAT_LTE_RADIO_ACCESS) &&
         (cmcall_is_there_csfb_call(CM_SS_MAIN,CM_CALL_CSFB_TYPE_1XCSFB,
                                         CM_CALL_ID_INVALID)
                                         != CM_CALL_ID_INVALID))
      {
        CMREGPRX_MSG_HIGH_0("Marking ok_to_orig as FALSE for CSFB call with LIMITED_REGIONAL srv");
        cmwcall_update_calls_on_srv_lost(cmregprx_stack_info_ptr->ss, FALSE);
      }
      #endif

      /* Log reported service */
      cmlog_service_cnf_event( srv_info_ptr, SD_SS_MAIN );
      break;

    case SYS_SRV_STATUS_MAX:
    case SYS_SRV_STATUS_NONE:
    default:
      sys_err_fatal_invalid_value_exception((uint8) srv_info_ptr->service_status);

  } /* switch( srv_info_ptr->service_status ) */

  SYS_ARG_NOT_USED(cmregprx_stack_info_ptr);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the SD20 action.
  */
  return sd_act;
} /* cmregprx_map_srv_ind_to_sd_ind() */
/*lint +esym(818,cmregprx_stack_info_ptr) */
/*===========================================================================

FUNCTION cmregprx_map_camped_ind_to_sd_ind

DESCRIPTION
  This function calls sd_ss_ind_gw_acq_plmn_camped ()

  Note! This function should be called only when the report is CM_CAMPED_IND

DEPENDENCIES
  None.

RETURN VALUE
  SD action.

SIDE EFFECTS
  None.
===========================================================================*/
static sd_ss_act_e_type                cmregprx_map_camped_ind_to_sd_ind
(
      sys_radio_access_tech_e_type     active_rat,
          /* RAT on which UE camped.
          */
      sd_band_u_type                   active_band,
          /* Active RAT's band.
          */

      const cmregprx_stack_info_s_type      *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
/*lint -esym(715,cmregprx_info_ptr)*/
{
  sd_ss_act_e_type sd_act = SD_SS_ACT_CONTINUE;
  sd_mode_e_type   mode;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_MSG_HIGH_3("mapping nas -> sd camp_ind: plmn [%u] [%u] [%u] ",
              cmregprx_stack_info_ptr->camped_ind_buf->cmd.camped_ind.plmn.identity[0],
              cmregprx_stack_info_ptr->camped_ind_buf->cmd.camped_ind.plmn.identity[1],
              cmregprx_stack_info_ptr->camped_ind_buf->cmd.camped_ind.plmn.identity[2]);

  mode = cmregprx_map_sys_rat_to_sd_mode(active_rat);

  #if defined(FEATURE_CIOT)
  if ((mode == SD_MODE_LTE_M1) ||
      (mode == SD_MODE_LTE_NB1)
     )
  {
    mode = SD_MODE_LTE;
	cmregprx_stack_info_ptr->camped_ind_buf->cmd.camped_ind.lac_id = CMSS_INVALID_LAC_INFO;
	CMREGPRX_MSG_HIGH_0("Resetting the  lac id in camp ind");
  }
  #endif
  
  sd_act = sd_ss_ind_gw_acq_plmn_camped_ss_1(mode,
                                        active_band,
                                        SD_SS_MAIN,
                                        NULL,
                                        cmregprx_stack_info_ptr->camped_ind_buf->cmd.camped_ind.plmn,
                                        cmregprx_stack_info_ptr->camped_ind_buf->cmd.camped_ind.lac_id,
                                        cmregprx_map_sys_rat_to_sd_mode(active_rat)
                                        );



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Return the SD20 action.
  */
  return sd_act;
} /* cmregprx_map_camped_ind_to_sd_ind() */
/*lint +esym(715,cmregprx_info_ptr)*/




/*===========================================================================

FUNCTION cmregprx_process_sd_action

DESCRIPTION
  This function will process the SD action.

  Note! This function should be called only when the RegProxy is in
  ACTIVE state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_process_sd_action
(
       sd_ss_act_e_type                sd_act,
          /* The SD action that has to be processed.
          */

       cmregprx_stack_info_s_type      *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{
  cmph_s_type                          *ph_ptr = cmph_ptr();
  sd_ss_mode_pref_e_type               sd_mode_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find if any GWL mode is supported by the current mode preference of
  ** the phone
  */
  /* Use new architecture for SGLTE */
  {
    sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(ph_ptr->main_stack_info.pref_info.mode_pref);
  }
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If SD action is get network list request or continue, return now.
  */
  if( sd_act != SD_SS_ACT_CONTINUE   &&
      sd_act != SD_SS_ACT_GET_NET_GW )
  {

    /* If CM is waiting for GW get network list cnf and not on HYBR2 stack
    ** for SVLTE, clear the network_type.
    */
    if(cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_NET_LIST_CNF)
    {

       CMREGPRX_MSG_HIGH_1(" Reset network_type=%d", ph_ptr->main_stack_info.pref_info.network_type);
       /* Terminate any pending get networks request. */
       
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
       if (cmregprx_stack_info_ptr->list_search_info.list_search_type == REG_CM_NETWORK_LIST_REQ_GPS_ADVANCE_SEARCH &&
	   	   ph_ptr->is_gps_search_mplmn_abort_event_sent == FALSE)
       {
		 cmph_term_pending_get_networks_req_client_only(CM_SS_MAIN);
       }
#endif

    }
  }


  /* Follow the SD return action.
  */
  switch ( sd_act )
  {
    case SD_SS_ACT_CONTINUE:
      /* Current selection is ok.
      */
      break;

    case SD_SS_ACT_ACQ_GWL:
      /* SD20 action is ACQ_GWL, Send the REG service request.
      */
      cmregprx_send_service_req_permission( cmregprx_stack_info_ptr );
      break;

    case SD_SS_ACT_GET_NET_GW:
      /* SD20 action is GET_NET_GW, send the Network list request.
      */
      cmregprx_send_reg_net_list_req( cmregprx_stack_info_ptr );
	#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
	  ph_ptr->is_gps_search_mplmn_abort_event_sent = FALSE;
	#endif
	  
      break;

    case SD_SS_ACT_PWR_SAVE:
      {
        /* SD20 action is PWR_SAVE, so put stack in pwr_save.
        */
        sd_ss_act_s_type  sd_act_info;
        sys_stop_mode_reason_e_type stop_reas;
        (void)sd_ss_act_get( &sd_act_info );
        
        /* local deactivation, we don't need to send STOP-MODE to NAS */
        if ( sd_act_info.prm.pwr_save.reas == SD_SS_PWR_SAVE_REAS_PROT_DEACT )
        {
          /* Send the deactivated indication or confirmation to MMoC to bring
          */
          cmregprx_send_mmoc_deactd_rpt( cmregprx_stack_info_ptr );
          return;
        }

        /* Set the stop_reas according to SD pwr save parameters.
        */
        switch ( sd_act_info.prm.pwr_save.reas )
        {
          case SD_SS_PWR_SAVE_REAS_NO_SRV:
            stop_reas = SYS_STOP_MODE_REASON_DEEP_SLEEP;

            /* Ending VOLTE call since LTE stack gets deactivated after ACQ fail (RLF during VOLTE)*/
            break;


          case SD_SS_PWR_SAVE_REAS_NO_SYS:

             #if defined(FEATURE_MMODE_GW) && !defined(FEATURE_MMODE_GPRS_LTE)
             if (ph_ptr->is_rat_bal == TRUE &&
                 ph_ptr->is_uim_usim_rat_present == TRUE)
             {
               if(ph_ptr->main_stack_info.pref_info.mode_pref == CM_MODE_PREF_GSM_ONLY ||
                 ph_ptr->main_stack_info.pref_info.mode_pref == CM_MODE_PREF_NONE)
               {
                 /*IF RAT balacing feature enabled */
                 stop_reas = SYS_STOP_MODE_REASON_NO_SYS;
               }
               else
               {
                 stop_reas = SYS_STOP_MODE_REASON_MODE_CHANGE;
               }
               CMREGPRX_MSG_HIGH_2("pwr_save.reas=NO_SYS change stop_reas to = %d, mode_pref=%d",
                           stop_reas,ph_ptr->main_stack_info.pref_info.mode_pref );
               break;
             }
             #endif
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
             if(ph_ptr->is_gps_search_high_priority == TRUE )
             {
               /* Don't fallback to default for GPS search case so break
                after setting the cause*/
			   CMREGPRX_MSG_HIGH_1("GPS suspend request status = %d",ph_ptr->is_wwan_suspend_req_send );
			   if( ph_ptr->is_wwan_suspend_req_send == TRUE )
			   {                 
				  CMREGPRX_MSG_HIGH_0("WWAN  suspend request already sent, looping back stop request from cmregprx");
                  return;
			   }
               stop_reas = SYS_STOP_MODE_REASON_SUSPEND_STACK;
			   ph_ptr->is_wwan_suspend_req_send = TRUE ;
               break;
             }
#endif
             /*lint -fallthrough */
             /* Intentional Fall-through */

          case SD_SS_PWR_SAVE_REAS_MAX:
          default:
            stop_reas = SYS_STOP_MODE_REASON_MODE_CHANGE;
            break;

        } /* switch ( sd_act.prm_pwr_save.reas ) */

        /* on Nikel , on hybr 2 stack even if it is requested to go to
        ** pwr save, send stop mode reason as ALT_SCAN
        ** IF GWL mode is not enabled, no need to alter the stop mode reason.
        ** use the one we decide above.
        */

        /* Send the stop_mode request to REG.
        */
        cmregprx_send_reg_stop_req( stop_reas,
                                   cmregprx_stack_info_ptr,
                                   PROT_TRANS_NONE
                                 );
      }
      break;



    default:

      /* Find if any GWL mode is supported by the current mode preference of
      ** the phone
      */
      {
        /* SD20 action is not ACQ_GWL, so send stop req to REG
        */
        cmregprx_send_reg_stop_req( SYS_STOP_MODE_REASON_MODE_CHANGE,
                                    cmregprx_stack_info_ptr,
                                    PROT_TRANS_NONE
                                  );
      }
      break;
  } /* switch( sd_act ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

} /* cmregprx_process_sd_action() */


/*===========================================================================

FUNCTION cmregprx_map_deact_to_stop_reason

DESCRIPTION
  This function will map the MMoC's protocol deactivate reason to
  REG's stop mode reason.

DEPENDENCIES
  None.

RETURN VALUE
  REG's stop mode reason.

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN sys_stop_mode_reason_e_type     cmregprx_map_deact_to_stop_reason
(
       prot_deact_e_type               deact_reason
          /* MMoC's deactivate reason.
          */

)
{
  sys_stop_mode_reason_e_type  stop_reason = SYS_STOP_MODE_REASON_MAX;
  CM_ASSERT_ENUM_IS_INRANGE( deact_reason, PROT_DEACT_MAX );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Based on the deact_reason, set the appropriate stop_reason.
  */
  switch ( deact_reason )

  {

    case PROT_DEACT_PWR_DOWN:
    case PROT_DEACT_RESET:
      stop_reason = SYS_STOP_MODE_REASON_POWER_OFF;
      break;

    case PROT_DEACT_OFFLINE:
      stop_reason = SYS_STOP_MODE_REASON_OFFLINE;
      break;

    case PROT_DEACT_SUBSC_CHGD:
      stop_reason = SYS_STOP_MODE_REASON_MODE_CHANGE;
      break;

    case PROT_DEACT_ACQ_OTHERS:
      stop_reason = SYS_STOP_MODE_REASON_ALT_SCAN;
      break;

    case PROT_DEACT_LPM:
      stop_reason = SYS_STOP_MODE_REASON_LPM;
      break;

    case PROT_DEACT_FTM:
      stop_reason = SYS_STOP_MODE_REASON_MODE_CHANGE;
      break;

    case PROT_DEACT_FROM_DORMANT:
    case PROT_DEACT_GWL_REDIR:
      stop_reason = SYS_STOP_MODE_REASON_IRAT;
      break;

    case PROT_DEACT_DUAL_SWITCH:
      stop_reason = SYS_STOP_MODE_REASON_DUAL_SWITCH;
      break;
    case PROT_DEACT_NO_SYS_TO_ACQ:
      stop_reason = SYS_STOP_MODE_REASON_NO_SYS;

    break;

    case PROT_DEACT_LOCAL_DETACH:
      stop_reason = SYS_STOP_MODE_REASON_LOCAL_DETACH;
      break;

    case PROT_DEACT_DSDX_SUSP:
      stop_reason = SYS_STOP_MODE_REASON_DSDS_SUSPEND;
      break;

    case PROT_DEACT_PSM:
      stop_reason = SYS_STOP_MODE_REASON_PSM;
      break;

    case PROT_DEACT_DEEP_SLEEP:
      stop_reason = SYS_STOP_MODE_REASON_DEEP_SLEEP;
      break;

    default:
      sys_err_fatal_invalid_value_exception((uint8) deact_reason);
      break;

  } /* switch ( deact_reason ) */



  return stop_reason;

} /* cmregprx_map_deact_to_stop_reason() */


/*===========================================================================

FUNCTION cmregprx_send_mmoc_deactd_rpt

DESCRIPTION
  This function will send the deactivated report(either indication or
  confirmation) to MMoC and updates the RegProxy's state and substate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_send_mmoc_deactd_rpt
(

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{
  #ifdef FEATURE_MMODE_SC_SVLTE
  cmregprx_stack_info_s_type            *cmregprx_stack2_info_ptr;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_MSG_HIGH_2("CMREGPRX->MMOC:      deactd_rpt, state= %d, substate =%d",
                     cmregprx_stack_info_ptr->state,
                     cmregprx_stack_info_ptr->substate );

  /* Check if we have send indication or confirmation.
  */
  if ( cmregprx_stack_info_ptr->mmoc_trans_id == PROT_TRANS_NONE )
  {
    /* No pending MMoC transaction, so send autonomous deactivated report.
    ** This has to be done only if MMOC thinks that CMREGPRX is active.
    ** If REGPRX was in DORMANT state, no need to send this info to MMOC.
    */
    if( cmregprx_stack_info_ptr->state == CMREGPRX_STATE_ACTIVE )
    {
      mmoc_rpt_prot_auto_deactd_ind_ss( PROT_DEACT_ACQ_OTHERS,
                                        PROT_STATE_ONLINE_GWL);
    }
  }
  else
  {
     /* MMoC transaction pending, send deactivated confirmation report.
     */
     mmoc_rpt_prot_deactd_cnf( cmregprx_stack_info_ptr->mmoc_trans_id );

     /* MMoC initiated deactivation, check for proceed for refresh for this ss.
     */
     #ifdef FEATURE_CM_MMGSDI_PROCEED_WITH_REFRESH
     if (cmph_is_msim())
     {
     cmmmgsdi_proceed_with_refresh_subs(SYS_MODEM_AS_ID_1);
     }
     else
     {
     cmmmgsdi_proceed_with_refresh(CM_SS_MAIN);
     }
     #endif

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  {
    /* Set RegProxy's state to INACTIVE.
    */
    cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                              cmregprx_stack_info_ptr
                            );
  }

  /* Set no MMoC transaction pending.
  */
  cmregprx_stack_info_ptr->mmoc_trans_id = PROT_TRANS_NONE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Set RegProxy's state to INACTIVE.
    */
    cmregprx_update_state( CMREGPRX_STATE_INACTIVE,
                           cmregprx_stack_info_ptr
                         );

    
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

} /*  cmregprx_send_mmoc_deactd_rpt() */


/*===========================================================================

FUNCTION cmregprx_get_cmd_buf_else_err_fatal

DESCRIPTION
  Allocate a Regproxy command buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to allocated RegProxy command buffer.

SIDE EFFECTS
  None
===========================================================================*/
static cmregprx_cmd_msg_s_type*        cmregprx_get_cmd_buf_else_err_fatal
(

       void

)
/*lint -esym(818, cmregprx_info_ptr) */
{
  cmregprx_cmd_msg_s_type *cmd_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Try allocating a command buffer.
  */

  cmd_ptr = (cmregprx_cmd_msg_s_type *)
              cm_mem_malloc (sizeof (cmregprx_cmd_msg_s_type));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return cmd_ptr;
} /* cmregprx_get_cmd_buf_else_err_fatal() */
/*lint +esym(818, cmregprx_info_ptr) */

/*===========================================================================

FUNCTION cmregprx_send_cmd

DESCRIPTION
  This function takes an already filled out cmregprx_cmd_msg_type and places it
  on the RegProxy's command queue.

DEPENDENCIES
  The cmregprx_init() must have been called up already.  All of the items in
  the cmregprx_cmd_msg_s_type must be already set.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_send_cmd
(

       cmregprx_cmd_msg_s_type         *msg_ptr,
          /* Pointer to command message buffer.
          */

       cmregprx_info_s_type            *cmregprx_info_ptr
          /* Pointer to the RegProxy's Info.
          */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( msg_ptr != NULL );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize and link the command onto the RegProxy command queue.
  */
  (void) q_link(msg_ptr, &msg_ptr->hdr.link );
  q_put(&cmregprx_info_ptr->cmd_q, &msg_ptr->hdr.link);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set the REX signal corresponding to the RegProxy's command queue and
  ** return.
  */
  (void) rex_set_sigs( cmregprx_info_ptr->rex_tcb_ptr,
                       cmregprx_info_ptr->cmd_q_sig
                     );

  return;

} /* cmregprx_send_cmd() */




/*===========================================================================
=============================================================================
=============================================================================
==================== ACTIVATE PROTOCOL EVENT HANDLER ========================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_inact_act_activate_prot_hdlr

DESCRIPTION
  This function handles the activate protocol command from MMoC in INACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Activate the REG based on the SD action.
  2) Set the RegProxy's state as ACTIVE.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_inact_act_activate_prot_hdlr
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr,
           /* Pointer to the command event that was received.
           */

        cmregprx_stack_info_s_type     *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
/*lint -esym(715,cmd_ptr)*/
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  && cmd_ptr->name == PROT_CMD_ACTIVATE );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


   cmregprx_process_sd_action( sd_ss_act_get( NULL ),
                                cmregprx_stack_info_ptr
                              );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set the RegProxy's state to ACTIVE.
  */
  cmregprx_update_state( CMREGPRX_STATE_ACTIVE,
                         cmregprx_stack_info_ptr
                       );

  return;

} /*  cmregprx_inact_act_activate_prot_hdlr() */
/*lint +esym(715,cmd_ptr)*/



/*===========================================================================

FUNCTION cmregprx_activate_prot_hdlr

DESCRIPTION
  This function handles the activate protocol command from MMoC.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) Invoke the state handler corresponding to this event.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_activate_prot_hdlr
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr
           /* Pointer to the command event that was received.
           */
)
{

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  && cmd_ptr->name == PROT_CMD_ACTIVATE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get stack ptr as per sent SS
  */
  cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(SD_SS_MAIN);

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_INACTIVE:
    case CMREGPRX_STATE_ACTIVE:
      cmregprx_inact_act_activate_prot_hdlr( cmd_ptr,
                                            cmregprx_stack_info_ptr
                                          );
      break;

    case CMREGPRX_STATE_MAX:
    default:
      CMREGPRX_MSG_HIGH_1("Received activate command in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;
} /*  cmregprx_activate_prot_hdlr() */



/*===========================================================================
=============================================================================
=============================================================================
==================== DEACTIVATE PROTOCOL EVENT HANDLER ======================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_active_deactivate_prot_hdlr

DESCRIPTION
  This function handles the deactivate protocol command from MMoC in ACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Send the cm_stop_mode_req to REG.
  2) Set the RegProxy's substate as WAIT_STOP_CNF.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_active_deactivate_prot_hdlr
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr,
           /* Pointer to the command event that was received.
           */

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  #ifdef FEATURE_GRACEFUL_PDP_DEACTIVATION_B4_DETACH
  sys_stop_mode_reason_e_type stop_mode_reason;
  /* Pointer to sm cmd buffer */
  sm_cmd_type     *sm_ptr = NULL;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  && cmd_ptr->name == PROT_CMD_DEACTIVATE );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* To take care of case where a dormant to inactive request from CM comes
  ** after an activate request from MMOC.
  **
  ** Example scenario:
  *  1. GW is put in dormant to scan for CDMA.
  ** 2. CDMA is acquired for a very short time and then CDMA service is lost.
  ** 3. GW stack is activated to continue GW scans.
  ** 4. Due to task priorities, CM handles the CDMA service report sent due
  **    to service acquired in step 2.
  */
  if( cmd_ptr->param.deact_prot.reason == PROT_DEACT_FROM_DORMANT )
  {
    CMREGPRX_MSG_HIGH_0( "Unexpected deact reason in Active state");
    /* Anyhow, send the Deactivated confirmation report.
    */
    mmoc_rpt_prot_deactd_cnf( cmd_ptr->param.deact_prot.trans_id );
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if CM is waiting for the network list cnf. and send get net terminate to CM clients
  */
  if ( cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_NET_LIST_CNF )
  {

     /* Network list cnf. not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_1("Deactivate while waiting for network list cnfsubstate %d",
                       cmregprx_stack_info_ptr->substate);

     /* Terminate any pending get networks request. */
     cmph_term_pending_get_networks_req_client_only(CM_SS_MAIN);
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
     if (cmregprx_stack_info_ptr->list_search_info.list_search_type == REG_CM_NETWORK_LIST_REQ_GPS_ADVANCE_SEARCH)
     {
       cmph_ptr()->is_gps_search_high_priority = FALSE;
     }
#endif

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if defined(FEATURE_GRACEFUL_PDP_DEACTIVATION_B4_DETACH) && defined(CM_GW_SUPPORTED)
    /* Graceful PDP deactivation. Wait for SM response, keep
    ** stop mode req in pending and move substate to WAIT FOR
    ** PDP ABORT CNF
    */
  stop_mode_reason = cmregprx_map_deact_to_stop_reason
                                (
                                  cmd_ptr->param.deact_prot.reason
                                );

  #endif
  {
    #ifdef FEATURE_DOMAIN_SELECTION
    cmsds_reset_during_stop_mode(cmd_ptr->param.deact_prot.reason);
    #endif

    cmregprx_send_reg_stop_req( cmregprx_map_deact_to_stop_reason
                              (
                                cmd_ptr->param.deact_prot.reason
                              ),
                              cmregprx_stack_info_ptr,
                              cmd_ptr->param.deact_prot.trans_id
                             );
  }

  return;

} /*  cmregprx_active_deactivate_prot_hdlr() */


/*===========================================================================

FUNCTION cmregprx_inactive_deactivate_prot_hdlr

DESCRIPTION
  This function handles the deactivate protocol command from MMoC in INACTIVE
  RegProxy's state. There is case when NAS received ALT_SCAN/DEEP_SLEEP/IRAT
  stop mode request then it is indeed in suspend state. And CMREGPRX at this time
  may be in inactive state.

  In this state handler function, the RegProxy will
  1) Send the cm_stop_mode_req to REG.
  2) Set the RegProxy's substate as WAIT_STOP_CNF.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_inactive_deactivate_prot_hdlr
(
        const cmregprx_cmd_msg_s_type  *cmd_ptr,
           /* Pointer to the command event that was received.
           */

        cmregprx_stack_info_s_type      *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */
)
{
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  && cmd_ptr->name == PROT_CMD_DEACTIVATE );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  if( cmd_ptr->param.deact_prot.reason == PROT_DEACT_FROM_DORMANT )
  {
    CMREGPRX_MSG_HIGH_0( "Unexpected deact reason in Inactive state");
    /* Anyhow, send the Deactivated confirmation report.
    */
    mmoc_rpt_prot_deactd_cnf( cmd_ptr->param.deact_prot.trans_id );
    return;
  }

  if(cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_STOP_CNF)
  {
    CMREGPRX_MSG_HIGH_1( "Unexpected deact in Inactive state - substate = %d",
                                              cmregprx_stack_info_ptr->substate);
    return;
  }
  if(cmd_ptr->param.deact_prot.reason == PROT_DEACT_NO_SYS_TO_ACQ )
  {
     CMREGPRX_MSG_HIGH_0( "Stop_mode_req sent with  PROT_DEACT_NO_SYS_TO_ACQ");

     /* reset  the MMoC's trans_id.
       */
     cmregprx_stack_info_ptr->mmoc_trans_id = PROT_TRANS_NONE;

     /* Send STOP_MODE_REQ to NAS. */

     cmregprx_send_reg_stop_req( cmregprx_map_deact_to_stop_reason(
                                           cmd_ptr->param.deact_prot.reason),
                                      cmregprx_stack_info_ptr,
                                      cmd_ptr->param.deact_prot.trans_id
                                    );
     return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_DOMAIN_SELECTION
  cmsds_reset_during_stop_mode(cmd_ptr->param.deact_prot.reason);
  #endif

  /* Send the REG stop request, if not yet sent and it is MMOC transaction
  */
  switch(cmregprx_stack_info_ptr->stop_mode_reason)
  {
    /* send STOP MODE when NAS is in suspend state
    */
  case SYS_STOP_MODE_REASON_DEEP_SLEEP:
  case SYS_STOP_MODE_REASON_IRAT:
  case SYS_STOP_MODE_REASON_UE_MODE_CHANGE:
  case SYS_STOP_MODE_REASON_ALT_SCAN:
    cmregprx_send_reg_stop_req( cmregprx_map_deact_to_stop_reason
                              (
                                cmd_ptr->param.deact_prot.reason
                              ),
                              cmregprx_stack_info_ptr,
                              cmd_ptr->param.deact_prot.trans_id
                            );
    CMREGPRX_MSG_HIGH_0( "Stop_mode_req sent in Inactive state");
    break;

  default:
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      
    /* -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Send deact rpt to MMOC immediately if stop mode req has not been sent
    */
    mmoc_rpt_prot_deactd_cnf( cmd_ptr->param.deact_prot.trans_id );
    CMREGPRX_MSG_HIGH_1( "STOP req not sent, last reason = %d",
                         cmregprx_stack_info_ptr->stop_mode_reason);

    /* If there is no STOP REQ to send, report refresh ok event right away
    ** MMoC initiated deactivation, check for proceed for refresh for this ss.
    */
    #ifdef FEATURE_CM_MMGSDI_PROCEED_WITH_REFRESH
    cmmmgsdi_proceed_with_refresh(CM_SS_MAIN);
    #endif

    break;

  } //switch(cmregprx_stack_info_ptr->stop_mode_reason)
  return;

}
/*===========================================================================

FUNCTION cmregprx_deactivate_prot_hdlr

DESCRIPTION
  This function handles the deactivate protocol command from MMoC.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) Invoke the state handler corresponding to this event or if the
     event was received in incorrect state, RegProxy will deactivated report
     to MMoC.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_deactivate_prot_hdlr
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr
           /* Pointer to the command event that was received.
           */
)
{

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  && cmd_ptr->name == PROT_CMD_DEACTIVATE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get stack ptr as per sent SS */
  cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(SD_SS_MAIN);

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
      cmregprx_active_deactivate_prot_hdlr( cmd_ptr,
                                            cmregprx_stack_info_ptr
                                          );
      break;


    case CMREGPRX_STATE_INACTIVE:
      cmregprx_inactive_deactivate_prot_hdlr( cmd_ptr,
                                         cmregprx_stack_info_ptr
                                         );
      break;
    case CMREGPRX_STATE_MAX:
    default:
      CMREGPRX_MSG_HIGH_1("Received deactivate command in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      {
        /* Anyhow, send the Deactivated confirmation report.
        */
        mmoc_rpt_prot_deactd_cnf( cmd_ptr->param.deact_prot.trans_id );
      }

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;

} /*  cmregprx_deactivate_prot_hdlr() */



/*===========================================================================
=============================================================================
=============================================================================
==================== IRAT TO GWL RESELECTION EVENT HANDLER =================
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================
=============================================================================
=============================================================================
==================== GENERIC PROTOCOL COMMAND EVENT HANDLER =================
=============================================================================
=============================================================================
===========================================================================*/

#ifndef FEATURE_MMODE_QTF
extern sys_gw_conn_status_e_type mm_per_subs_get_connection_status(
                                                  sys_modem_as_id_e_type as_id);
extern sys_gw_conn_status_e_type mm_per_stacks_get_connection_status(
                                                  sys_modem_as_id_e_type sub_id,
                                                  sys_modem_stack_id_e_type stack_id);
#endif



/*===========================================================================

FUNCTION cmregprx_active_generic_prot_cmd_hdlr

DESCRIPTION
  This function handles the generic protocol command from MMoC in ACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Process the generic command by calling mmoc function.
  2) Follow the return SD action.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_active_generic_prot_cmd_hdlr
(

        const cmregprx_cmd_msg_s_type        *cmd_ptr,
           /* Pointer to the command event that was received.
           */

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  boolean allow_resel = TRUE;
  sys_gw_conn_status_e_type     conn_status  = SYS_GW_CONN_STATUS_NONE;

  CM_ASSERT( cmd_ptr != NULL  && cmd_ptr->name == PROT_CMD_GENERIC );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /* check if reselection is allowed based on connection status of MM*/  
  conn_status             = mm_per_stacks_get_connection_status( SYS_MODEM_AS_ID_1, SYS_MODEM_STACK_ID_1 );
      
  if(conn_status == SYS_GW_CONN_STATUS_BPLMN_SRCH )
  {
       allow_resel = FALSE;
  }

  CMREGPRX_MSG_HIGH_2("is_resel_allowed %d ,Conn status : %d ", allow_resel, conn_status);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the generic command and follow return action.
  */
  cmregprx_process_sd_action( mmoc_proc_prot_gen_cmd( allow_resel,
                                     TRUE,
                                     &cmd_ptr->param.gen_cmd
                                                     ),
                              cmregprx_stack_info_ptr
                             );

  return;

} /*  cmregprx_active_generic_prot_cmd_hdlr() */



/*===========================================================================

FUNCTION cmregprx_generic_prot_cmd_hdlr

DESCRIPTION
  This function handles the generic protocol command from MMoC.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will acknowledge the MMoC with command processed flag
     set to FALSE.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_generic_prot_cmd_hdlr
(

        const cmregprx_cmd_msg_s_type        *cmd_ptr
           /* Pointer to the command event that was received.
           */
)
{

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  && cmd_ptr->name == PROT_CMD_GENERIC);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get stack ptr as per sent SS */
  cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(SD_SS_MAIN);

  CMREGPRX_MSG_HIGH_1("cmregprx_generic_prot_cmd_hdlr(),cmd_type=%d",cmd_ptr->param.gen_cmd.cmd_type);
  
  CMREGPRX_MSG_MED_5("cmregprx_stack_info->state %d, substate %d, mmoc_trans %d, gwl_subs_avail %d, resel_allow %d",
                     cmregprx_stack_info_ptr->state,
                     cmregprx_stack_info_ptr->substate,
                     cmregprx_stack_info_ptr->mmoc_trans_id,
                     cmregprx_stack_info_ptr->is_gwl_subs_avail,
                     cmregprx_stack_info_ptr->is_resel_allowed);

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
      cmregprx_active_generic_prot_cmd_hdlr( cmd_ptr,
                                             cmregprx_stack_info_ptr
                                           );
      break;

    case CMREGPRX_STATE_INACTIVE:
    case CMREGPRX_STATE_MAX:
    default:
      CMREGPRX_MSG_HIGH_1("Received generic command in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      /* Send the Generic command acknowledgement report with
      ** is_cmd_processed = FALSE.
      */
      (void) mmoc_proc_prot_gen_cmd( FALSE, FALSE, &cmd_ptr->param.gen_cmd);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;

} /* cmregprx_generic_prot_cmd_hdlr() */

/*===========================================================================
==================== SIM REFRESH IN LPM FUNCTIONS ===========================
===========================================================================*/

/*===========================================================================

FUNCTION cmregprx_sim_refresh_is_pending

DESCRIPTION
  Checks to see if CMREGPRX is in between sending SIM refresh commands to
  NAS while modem has transitioned to LPM state. This is needed to avoid
  duplicate SUB processing for all protocols when modem moves between
  ONLINE->LPM->ONLINE without SUB update. 3GPP2 protocols don't need to
  receive SUB update on above operation mode transition. Whereas 3GPP protocols
  and NAS need to recieve SIM not available followed by SIM available ( If 3GPP
  SUB is available) during LPM transitions so that security information is
  purged.

  So a mechanism is implemented in CMREGPRX to implement following flow:

  UI/UIM/SIM      CM        MMOC      CMREGPRX      NAS
  =====================================================
      |            |          |          |           |
      |--SUB------>|--SUB---->|--SUB---->|--SUB----->|
      |            |          |          |           |
      |--ONLINE--->|--ONLINE->|--ONLINE->|--ONLINE-->|
      |            |          |          |           |
     ############ MODE IS INLINE OPERATION ############
      |            |          |          |           |
      |--LPM------>|--LPM---->|--LPM---->|--LPM----->|
      |            |          |          |           |
     ############ MODEM IS LPM OPERATION ##############
      |            |          |          |           |
     ############ NEW BEHAVIOR ########################
      |            |          |          |           |
      |            |          |          |--SIM NOT->|
      |            |          |          |           |
      |            |          |          |<--CNF-----|
      |            |  ** ANY SUB/OPRT ** |           |
      |            |  ** CMD DEFERRED ** |--SIM AVL->|
      |            |          |          |           |
      |            |          |          |<--CNF-----|
      |            |          |          |           |
     ------------ PROCESS DEFERRED CMD ---------------
      |            |          |          |           |

RETURN VALUE
  TRUE - If SIM refresh in progress
  FALSE - No SIM refresh in progress - normal operation
===========================================================================*/

static boolean cmregprx_sim_refresh_is_pending
(
  cmregprx_stack_info_s_type* cmregprx_info_ptr
)
{
  cmregprx_substate_e_type substate = cmregprx_info_ptr->substate;

  if(substate == CMREGPRX_SUBSTATE_SIM_REFRESH_WAIT_NOT_AVL_CNF ||
     substate == CMREGPRX_SUBSTATE_SIM_REFRESH_WAIT_AVL_CNF)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION cmregprx_send_ph_stat_cnf_rpt

DESCRIPTION
  Sends mmoc_rpt_ph_stat_chgd_cnf and update substate to NULL.
===========================================================================*/

static void cmregprx_send_ph_stat_cnf_rpt
(
  cmregprx_stack_info_s_type* cmregprx_stack_info_ptr
)
{
  cmregprx_sim_refresh_state_s_type* sim_refresh =
                                            &cmregprx_stack_info_ptr->sim_refresh;

  /* Send the phone status ack. report and clear the MMoC trans_id.
  */

  mmoc_rpt_ph_stat_chgd_cnf( cmregprx_stack_info_ptr->mmoc_trans_id, SD_MODE_GWL,MMOC_PH_STAT_NO_ERROR );
  
  cmregprx_stack_info_ptr->mmoc_trans_id = PROT_TRANS_NONE;
  
  /* Update the substate to NULL.
  */
  cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                            cmregprx_stack_info_ptr
                          );
  sim_refresh->oprt_mode = PROT_OPRT_MODE_NULL;

}

/*===========================================================================

FUNCTION cmregprx_sim_refresh_completed

DESCRIPTION
  Perform steps needed to complete SIM refresh procedure. This completes new
  behavior introduced for SIM refresh.
===========================================================================*/

static void cmregprx_sim_refresh_completed
(
  cmregprx_sim_refresh_state_s_type* sim_refresh,

  cmregprx_stack_info_s_type* cmregprx_stack_info_ptr
)
{
  CMREGPRX_MSG_MED_0("SIM refresh completed");

  cmregprx_update_substate(CMREGPRX_SUBSTATE_NULL,
                           cmregprx_stack_info_ptr);

  if(sim_refresh->oprt_mode != PROT_OPRT_MODE_NULL)
  {
    cmregprx_send_ph_stat_cnf_rpt(cmregprx_stack_info_ptr);
  }
  if(sim_refresh->pending_cmd)
  {
    CMREGPRX_MSG_MED_1("SIM refresh: process deferred %d cmd",
          sim_refresh->pending_cmd->param.gw_ph_stat_cmd.chg_type);
    cmregprx_send_cmd( sim_refresh->pending_cmd,
                       cmregprx_get_info_ptr());
    sim_refresh->pending_cmd = NULL;
  }
}


/*===========================================================================

FUNCTION cmregprx_sim_refresh_ph_stat_cmd

DESCRIPTION
  Remember operation mode and SUB from MMOC, to be used while handling
  CM_PH_STATUS_CHGD_CNF/CM_SIM_NOT_AVAILABLE_CNF.
  This handles ph stat changed commands in 2 scenarios:
  1. While CMREGPRX is in between SIM refresh
     - Defer/Buffer PH stat changed commands, so that they can reprocessed
       once getting out of SIM refresh state.
  2. No SIM refresh pending - normal operation
     - No processing needed.

===========================================================================*/

static void cmregprx_sim_refresh_ph_stat_cmd
(
  cmregprx_stack_info_s_type* cmregprx_info_ptr,

  const cmregprx_cmd_msg_s_type *cmd_ptr
)
{
  cmregprx_sim_refresh_state_s_type* sim_refresh =
                                            &cmregprx_info_ptr->sim_refresh;

  if(cmd_ptr->param.gw_ph_stat_cmd.chg_type == PROT_PH_STAT_CHG_OPRT_MODE)
  {
    sim_refresh->oprt_mode = cmd_ptr->param.gw_ph_stat_cmd.oprt_mode;
  }
  else if(cmd_ptr->param.gw_ph_stat_cmd.chg_type == PROT_PH_STAT_CHG_SUBSC)
  {
    sim_refresh->is_gwl_subs_avail =
                              cmd_ptr->param.gw_ph_stat_cmd.is_gwl_subs_avail;
    sim_refresh->mode_pref = cmd_ptr->param.gw_ph_stat_cmd.mode_pref;
    
  }

  if(cmregprx_sim_refresh_is_pending(cmregprx_info_ptr))
  {
    if(sim_refresh->pending_cmd == NULL)
    {
      sim_refresh->pending_cmd =
                cmregprx_get_cmd_buf_else_err_fatal();

      memscpy( sim_refresh->pending_cmd,
               sizeof(cmregprx_cmd_msg_s_type),
               cmd_ptr,
               sizeof(cmregprx_cmd_msg_s_type) );

      CMREGPRX_MSG_HIGH_1("SIM refresh: cmd %d is bufferred",
                              cmd_ptr->param.gw_ph_stat_cmd.chg_type);
    }
    else
    {
      CMREGPRX_MSG_HIGH_1("SIM refresh: cmd %d is dropped",
                              cmd_ptr->param.gw_ph_stat_cmd.chg_type);
    }
  }
}

/*===========================================================================

FUNCTION cmregprx_sim_refresh_ph_stat_rpt

DESCRIPTION
  This handles reports from NAS in the context of SIM refresh state and LPM.
  1. CM_PH_STATUS_CHGD_CNF: Once NAS sends CNF for LPM status change,
                            SIM refresh procedure is initiated by sending
                            SIM not AVL request to NAS.
  2. CM_SIM_NOT_AVAILABLE_CNF: Once NAS sends CNF for SIM not AVL in
                               SIM refresh state, SIM AVL is sent based on
                               GWL SUB availability to NAS.
  3. CM_SIM_AVAILABLE_CNF: SIM AVL CNF from NAS in SIM refresh state completes
                           SIM refresh procedure. If there were any PH state
                           change cmd recvd during SIM refresh state, they're
                           processed.
===========================================================================*/

static void cmregprx_sim_refresh_ph_stat_rpt
(
  cmregprx_stack_info_s_type* cmregprx_info_ptr,

  const cm_rpt_type *rpt_ptr
)
{
  reg_cmd_type *reg_ptr;
  cmregprx_sim_refresh_state_s_type* sim_refresh =
                                            &cmregprx_info_ptr->sim_refresh;

  switch(rpt_ptr->hdr.cmd)
  {
    case CM_PH_STATUS_CHGD_CNF:
      /* When modem shutdown is received its is not necessary to sent SIM NOT AVAIL/SIM AVAIL
          ** to NAS. So ignore SIM refresh when task stop signal is received.
          */
      if((sim_refresh->oprt_mode == PROT_OPRT_MODE_LPM) && (cmtask_is_stop_sig_rcvd() == FALSE))
      {
        reg_ptr =  cm_reg_get_buf_else_err_fatal(CM_SIM_NOT_AVAILABLE_REQ);
        reg_ptr->cmd.cm_sim_not_available_req.transaction_id =
                                   cmregprx_next_trans_id(cmregprx_info_ptr);
        #if !(defined(FEATURE_MMODE_LTE_ONLY) || defined(FEATURE_MMODE_GPRS_LTE))
        reg_ptr->cmd.cm_sim_not_available_req.as_id = cmregprx_info_ptr->as_id;
        reg_ptr->cmd.cm_sim_not_available_req.stack_id = cmregprx_info_ptr->nas_stack_id;
        #endif
        
        cm_reg_send_cmd( reg_ptr );
        cmregprx_update_substate(CMREGPRX_SUBSTATE_SIM_REFRESH_WAIT_NOT_AVL_CNF,
                               cmregprx_info_ptr);
        CMREGPRX_MSG_MED_0("SIM refresh: SIM_NOT_AVL is sent");
      }
      break;

    case CM_SIM_NOT_AVAILABLE_CNF:
      if(cmregprx_info_ptr->reg_trans_id ==
                           rpt_ptr->cmd.sim_not_available_cnf.transaction_id)
      {
        if(sim_refresh->is_gwl_subs_avail || CM_SUBS_PERSO_LOCK == cmregprx_info_ptr->is_gwl_subs_avail)
        {
          reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_SIM_AVAILABLE_REQ );
          reg_ptr->cmd.cm_sim_available_req.transaction_id =
                                    cmregprx_next_trans_id(cmregprx_info_ptr);
          reg_ptr->cmd.cm_sim_available_req.mode_pref = sim_refresh->mode_pref;
          
          cm_reg_send_cmd( reg_ptr );

          cmregprx_update_substate(CMREGPRX_SUBSTATE_SIM_REFRESH_WAIT_AVL_CNF,
                                   cmregprx_info_ptr);
          CMREGPRX_MSG_MED_1("SIM refresh: SIM_AVL is sent %d", cmregprx_info_ptr->is_gwl_subs_avail);
        }
        else
        {
          cmregprx_sim_refresh_completed(sim_refresh, cmregprx_info_ptr);
        }
      }
      break;

    case CM_SIM_AVAILABLE_CNF:
      if(cmregprx_info_ptr->reg_trans_id ==
           rpt_ptr->cmd.sim_available_cnf.transaction_id)
      {
        cmregprx_sim_refresh_completed(sim_refresh, cmregprx_info_ptr);
      }
      break;

    default:
      CMREGPRX_MSG_MED_1("SIM refresh: skip %d proc", rpt_ptr->hdr.cmd);
  }
}


/*===========================================================================
=============================================================================
=============================================================================
==================== PHONE STATUS CHANGED EVENT HANDLER =====================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_inactive_ph_stat_chgd_hdlr

DESCRIPTION
  This function handles the Phone status changed command from MMoC in
  INACTIVE RegProxy's state.

  In this state handler function,

     If the chg_type = PROT_PH_STAT_CHG_OPRT_MODE
     the RegProxy will send cm_ph_status_chgd_req to REG if phone status
     changed command was received because of oprt_mode change.

     If the chg_type = PROT_PH_STAT_CHG_SUBSC
     the RegProxy will send the cm_sim_available_req() or
     cm_sim_not_available_req()

     Else, the RegProxy will acknowledge the MMoC of phone status changed
     command.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_inactive_ph_stat_chgd_hdlr
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr,
           /* Pointer to the command event that was received.
           */

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{

  const cmregprx_ph_stat_chgd_s_type  *ph_stat_ptr = &(cmd_ptr->param.gw_ph_stat_cmd);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  &&
                   cmd_ptr->name == PROT_CMD_PH_STAT_CHGD );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmregprx_sim_refresh_ph_stat_cmd(cmregprx_stack_info_ptr, cmd_ptr);
  if(cmregprx_sim_refresh_is_pending(cmregprx_stack_info_ptr))
  {
    CMREGPRX_MSG_HIGH_0("Wait for SIM refresh to complete");
    return;
  }

  CMREGPRX_MSG_HIGH_2("ph_stat_chgd:is_gwl_subs_avail %d->%d",                     
                     cmregprx_stack_info_ptr->is_gwl_subs_avail,
                     ph_stat_ptr->is_gwl_subs_avail);

  /* Check if the  status change was because of subscription change.
  ** If the subscription status has not toggled, do not send sim_avail/sim_not_avail
  ** indication to REG. back to back SIM_AVAIL indications to NAS result in
  ** duplication of sec context leading to issues communicating with n/w
  */
  if( ph_stat_ptr->chg_type          == PROT_PH_STAT_CHG_SUBSC &&
      ph_stat_ptr->is_gwl_subs_avail != cmregprx_stack_info_ptr->is_gwl_subs_avail
    )
  {



     cmregprx_send_reg_sim_avail_req
                          (
                            cmd_ptr,
                            cmregprx_stack_info_ptr
                          );


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


     /* Update the GW subscription availability flag.
     */
    cmregprx_stack_info_ptr->is_gwl_subs_avail = ph_stat_ptr->is_gwl_subs_avail;



    #if defined(FEATURE_MMGSDI_SESSION_LIB) && defined(FEATURE_CM_MMGSDI_PROCEED_WITH_REFRESH)
    /* Check if MMGSDI needs to send OK TO PROCEED */
    cmmmgsdi_proceed_with_refresh(CM_SS_MAIN);
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Save the MMoC's trans_id.
    */
     cmregprx_stack_info_ptr->mmoc_trans_id = cmd_ptr->param.gw_ph_stat_cmd.trans_id;
    CMREGPRX_MSG_HIGH_1("trans_id:%d", cmregprx_stack_info_ptr->mmoc_trans_id);

    return;
  }  /* if () */

  /* Check if the  status change was because of operating mode change.
  */
  else if (cmd_ptr->param.gw_ph_stat_cmd.chg_type == PROT_PH_STAT_CHG_OPRT_MODE)
  {
    if( IS_PH_STATUS_CHG_OPRT_MODE(cmd_ptr->param.gw_ph_stat_cmd.oprt_mode) )
    {
      cmregprx_send_reg_ph_status_chgd_req
            ( cmd_ptr,
              cmregprx_stack_info_ptr );

      /* Save the MMoC's trans_id.
      */
      cmregprx_stack_info_ptr->mmoc_trans_id = cmd_ptr->param.gw_ph_stat_cmd.trans_id;

      //cmregprx_stack_info_ptr->as_id = cmd_ptr->param.gw_ph_stat_cmd.as_id;
      //cmregprx_stack_info_ptr->ss = cmd_ptr->ss;

      return;

    }
  }


  /* Nothin' to do in RegProxy.
  */
 
  mmoc_rpt_ph_stat_chgd_cnf( cmd_ptr->param.gw_ph_stat_cmd.trans_id, SD_MODE_GWL ,MMOC_PH_STAT_NO_ERROR);
 
} /*  cmregprx_inactive_ph_stat_chgd_hdlr() */

/*===========================================================================

FUNCTION cmregprx_ph_stat_chgd_hdlr

DESCRIPTION
  This function handles the Phone status changed command that is sent by MMoC

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will do ERR_FATAL.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_ph_stat_chgd_hdlr
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr
           /* Pointer to the command event that was received.
           */

)
{

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  &&
                   cmd_ptr->name == PROT_CMD_PH_STAT_CHGD );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get stack ptr as per sent SS
  */
  cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(SD_SS_MAIN);

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_INACTIVE:
      cmregprx_inactive_ph_stat_chgd_hdlr( cmd_ptr,
                                           cmregprx_stack_info_ptr
                                         );
      break;


    case CMREGPRX_STATE_ACTIVE:
    case CMREGPRX_STATE_MAX:
    default:
      CMREGPRX_MSG_HIGH_1("Received generic command in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      mmoc_rpt_ph_stat_chgd_cnf( cmd_ptr->param.gw_ph_stat_cmd.trans_id, SD_MODE_GWL,MMOC_PH_STAT_NO_ERROR );
      break;

  } /* switch (cmregprx_info_ptr->state ) */


  return;
} /*  cmregprx_ph_stat_chgd_hdlr() */


#ifdef FEATURE_LTE
/*===========================================================================

FUNCTION cmregprx_ps_detach_hdlr

DESCRIPTION
  This function handles the PS detach req command that is sent by MMoC

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will do ERR_FATAL.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_ps_detach_hdlr
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr
           /* Pointer to the command event that was received.
           */

)
{

  cmregprx_stack_info_s_type        *cmregprx_stack_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  &&
                   cmd_ptr->name == PROT_CMD_PS_DETACH);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Get stack ptr as per sent SS
  */
  cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(SD_SS_MAIN);

  cmregprx_send_ps_detach_req
                          (
                            cmd_ptr->param.ps_detach_cmd.type,
                            cmregprx_stack_info_ptr
                          );

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* Save the MMoC's trans_id in stack 1
   ** <common message cnf will be stored in stack 1>
   */
   cmregprx_stack_info_ptr->mmoc_trans_id
                                     = cmd_ptr->param.ps_detach_cmd.trans_id;

  return;

}
#endif

static  void                           cmregprx_send_service_req_permission
(
       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */
)
{
  #ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
  sys_modem_as_id_e_type  as_id = SYS_MODEM_AS_ID_1;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if (cmph_is_msim())
  {
    as_id = cmregprx_stack_info_ptr->as_id;
  }
  #endif
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmregprx_send_reg_service_req( cmregprx_stack_info_ptr );

  return;
}

/*===========================================================================
=============================================================================
=============================================================================
==================== REG SERVICE CNF EVENT HANDLER ==========================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_active_srv_cnf_hdlr

DESCRIPTION
  This function handles the service confirmation report from REG in ACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if the service cnf report was expected.
  2) Map the service cnf report to appropriate SD indication and process
     return SD action.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_active_srv_cnf_hdlr
(

        cm_rpt_type                    *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  uint16   sys_sel_pref_req_id = 0;;
  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_SERVICE_CNF );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the service conf. is expected and is it the right one?.
  */
  if ( cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_SRV_CNF ||
       cmregprx_stack_info_ptr->reg_trans_id !=
                                 rpt_ptr->cmd.service_cnf.transaction_id
     )
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


     /* Service conf. not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_2("Unexpected srv conf. recvd, substate %d, trans_id %d",
                       cmregprx_stack_info_ptr->substate,
                       cmregprx_stack_info_ptr->reg_trans_id);
     #if defined(FEATURE_MMODE_9X05_EMERG_CALL)
     sd_ss_update_emc_attached(rpt_ptr->cmd.service_cnf.service_state.emc_attached);
     #endif
     return;

  } /* if ( ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Received the correct cnf. Update the substate to NULL.
  */

  if (cmregprx_stack_info_ptr->camped_ind_buf  != NULL)
  {
    cm_mem_free ( cmregprx_stack_info_ptr->camped_ind_buf );
    cmregprx_stack_info_ptr->camped_ind_buf  = NULL;
  }
  cmregprx_stack_info_ptr->last_srv_req.message_header.message_id =
                                               (byte)CM_REG_CMD_NONE;

  // Preserving req_id so that here cmregprx should not update req_id to SD, which will cause incorrect srv_status update to CM
  sys_sel_pref_req_id = cmregprx_stack_info_ptr->sys_sel_pref_req_id;
  cmregprx_stack_info_ptr->sys_sel_pref_req_id = CM_DEFAULT_VALUE;
  cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                            cmregprx_stack_info_ptr
                          );
  cmregprx_stack_info_ptr->sys_sel_pref_req_id = sys_sel_pref_req_id;


  /* Inform System determination of service conf. report and follow return
  ** action.
  */
  cmregprx_process_sd_action( cmregprx_map_srv_cnf_to_sd_ind
                              (
                                &rpt_ptr->cmd.service_cnf.service_state,
                                rpt_ptr->cmd.service_cnf.scan_status,
                                cmregprx_stack_info_ptr
                              ),
                              cmregprx_stack_info_ptr
                            );



  return;

} /*  cmregprx_active_srv_cnf_hdlr() */



/*===========================================================================

FUNCTION cmregprx_srv_cnf_hdlr

DESCRIPTION
  This function handles the service indication report from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_srv_cnf_hdlr
(

        cm_rpt_type                    *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
 {

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_SERVICE_CNF );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_MSG_HIGH_1("as_id=%d, CM_SERVICE_CNF",
                        rpt_ptr->cmd.service_cnf.as_id);

  cmregprx_stack_info_ptr = &(cmregprx_info_ptr->stack_info);

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
      cmregprx_active_srv_cnf_hdlr( rpt_ptr,
                                    cmregprx_stack_info_ptr
                                  );
	  
      break;

    case CMREGPRX_STATE_INACTIVE:
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in ACTIVE state.
      */
      CMREGPRX_MSG_HIGH_1("Received service cnf in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;
} /* cmregprx_srv_cnf_hdlr() */



/*===========================================================================
=============================================================================
=============================================================================
==================== REG SERVICE IND EVENT HANDLER ==========================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_active_srv_ind_hdlr

DESCRIPTION
  This function handles the service indication report from REG in ACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if the service ind report was expected.
  2) Map the service ind report to appropriate SD indication and process
     return SD action.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_active_srv_ind_hdlr
(

        cm_rpt_type                    *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type     *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{

  #if defined (FEATURE_CM_LTE ) || defined (FEATURE_WRLF_SYSTEM_SEL)
  const sys_plmn_service_state_s_type  *srv_info_ptr = NULL;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd ==  CM_SERVICE_IND );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined (FEATURE_CM_LTE ) || defined (FEATURE_WRLF_SYSTEM_SEL)
  if (cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_PS_DETACH_CNF)
  {
    CMREGPRX_MSG_HIGH_0("Recvd CM_SERVICE_IND while expecting CM_PS_DETACH_CNF" );
    srv_info_ptr = &rpt_ptr->cmd.service_ind.service_state;

    if ((srv_info_ptr->no_service_cause == SYS_NO_SVC_CAUSE_RLF) &&
        (srv_info_ptr->suitable_search_time != 0))
    {
      #if defined (FEATURE_CM_LTE )
      if ((srv_info_ptr->active_rat == SYS_RAT_LTE_RADIO_ACCESS ||
           srv_info_ptr->active_rat == SYS_RAT_LTE_M1_RADIO_ACCESS ||
           srv_info_ptr->active_rat == SYS_RAT_LTE_NB1_RADIO_ACCESS ) &&
          (cmregprx_stack_info_ptr->lte_connected_mode == FALSE))
      {
        cmregprx_stack_info_ptr->lte_connected_mode = TRUE;

        sd_ss_set_lte_connected_mode(srv_info_ptr->suitable_search_time, SD_SS_MAIN);

        CMREGPRX_MSG_HIGH_1("LTE in Connected Mode at CM_SERVICE_IND, T311: %d",
        srv_info_ptr->suitable_search_time);
      }
      #endif

      return;
    }
  }
  #endif

  /* Check if the service ind. is expected and is it the right one?.
  */
  if ( ! ( cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_NULL))
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


     /* Service ind. not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_1("Unexpected srv ind. recvd, substate %d",
                       cmregprx_stack_info_ptr->substate);
     #if defined(FEATURE_MMODE_9X05_EMERG_CALL)
     sd_ss_update_emc_attached(rpt_ptr->cmd.service_cnf.service_state.emc_attached);
     #endif
     return;

  } /* if ( ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Inform System determination of service ind. report and follow return
  ** action
  */
  cmregprx_process_sd_action( cmregprx_map_srv_ind_to_sd_ind
                              (
                                &rpt_ptr->cmd.service_ind.service_state,
                                cmregprx_stack_info_ptr
                              ),
                              cmregprx_stack_info_ptr
                            );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
} /*  cmregprx_active_srv_ind_hdlr() */


#if defined(FEATURE_MMODE_GW)
 /*===========================================================================

FUNCTION cmregprx_plmn_list_change_hdlr

DESCRIPTION
   Handler to pass on plmn list type from NAS to DS and other clients 
  through MSGR


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static void cmregprx_plmn_list_change_hdlr
(
    cm_rpt_type                    *rpt_ptr
       /* Pointer to the CM report message.
       */
)
{

  errno_enum_type msgr_error = E_SUCCESS;

  cm_plmn_list_chngd_ind_s_type *plmn_chnage = (cm_plmn_list_chngd_ind_s_type *)cm_mem_malloc
                                      (sizeof(cm_plmn_list_chngd_ind_s_type));

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_PLMN_LIST_CHANGE_IND );


  plmn_chnage->changed_list_type  = rpt_ptr->cmd.plmn_list_chng_ind.changed_list_type;
  plmn_chnage->msg_hdr.inst_id= SYS_AS_ID_TO_INST_ID(rpt_ptr->cmd.plmn_list_chng_ind.as_id);

  msgr_error =  cm_msgr_send( MM_CM_PLMN_LIST_CHANGE_IND, MSGR_MM_CM,
            (msgr_hdr_s*)plmn_chnage, sizeof(cm_plmn_list_chngd_ind_s_type) );

  CM_MSG_HIGH_1 ( "CM->MSGR: cmregprx_plmn_list_change_hdlr(), msgr_error=%d", msgr_error );

  cm_mem_free(plmn_chnage);

  return;

}
#endif


/*===========================================================================

FUNCTION cmregprx_srv_ind_hdlr

DESCRIPTION
  This function handles the service indication report from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_srv_ind_hdlr
(

        cm_rpt_type                    *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_SERVICE_IND );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_MSG_HIGH_1("as_id=%d, CM_SERVICE_IND",
                        rpt_ptr->cmd.service_ind.as_id);

  cmregprx_stack_info_ptr = &(cmregprx_info_ptr->stack_info);

  #ifdef FEATURE_CM_DEBUG_BUFFER
  // Disabling as of now, eats up many entries
  //cmdbg_add_to_dbg_buffer(CM_BUFF_RPT_TYPE, CM_SERVICE_IND, ((void*)&(rpt_ptr->cmd.service_ind)));
  #endif

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
      cmregprx_active_srv_ind_hdlr( rpt_ptr,
                                    cmregprx_stack_info_ptr
                                  );
      break;

    case CMREGPRX_STATE_INACTIVE:
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in ACTIVE state.
      */
      CMREGPRX_MSG_HIGH_1("Received service ind in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;
} /* cmregprx_srv_ind_hdlr() */




/*===========================================================================
=============================================================================
=============================================================================
==================== REG NETWORK LIST CNF EVENT HANDLER =====================
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================

FUNCTION cmregprx_handle_plmn_list_search_cnf

DESCRIPTION
  This function handles the cm_network_list_cnf for the GET_NET request
  from SD.

  This functions gets next action from SD and calls  cmregprx_process_sd_action
  to perform the SD action

DEPENDENCIES
  Error checking of the current state should have been done already.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static void cmregprx_handle_plmn_list_search_cnf
(
  cm_rpt_type                    *rpt_ptr,
     /* Pointer to the CM report message.
     */

  cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
     /* Pointer to RegProxy's  information, may/may not be updated
     ** by the state handlers.
     */
)
{
 cm_orig_q_s_type                   * ph_orig_top_ptr = NULL;
    cmph_s_type                        * ph_ptr = cmph_ptr();
    sd_ss_mode_pref_e_type               sd_mode_pref;
    sys_modem_as_id_e_type               asubs_id;
  cm_ss_e_type                           cm_ss;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_NETWORK_LIST_CNF );
  CMREGPRX_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_NETWORK_LIST_CNF );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cm_ss = CM_SS_MAIN;
  asubs_id = cmph_map_cm_ss_to_subs(cm_ss);
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);


   sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(
                                                  CMPH_SS_MODE_PREF(asubs_id, ph_ptr));
   #ifndef FEATURE_MMODE_DISABLE_CSG
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Check if there is any activity of higher priority on the main stack
  */
  ph_orig_top_ptr = cmtask_orig_para_get_top(cm_ss);

  if (ph_orig_top_ptr == NULL)
  {
    MSG_ERROR ("Top element on priority queue of MAIN Stack is NULL",0,0,0);
  }
  /* If csg app search on MAIN, W/L is not part of mode pref, get net is successful
  ** restore mode pref and force pref to sd.
  */
  else if ((!sd_misc_is_mode_pref(SD_SS_MODE_PREF_WCDMA,sd_mode_pref) ||
            !sd_misc_is_mode_pref(SD_SS_MODE_PREF_LTE,sd_mode_pref)) &&
           (cmregprx_map_list_cnf_from_reg_to_sys (rpt_ptr->cmd.net_list_cnf.status) == SYS_PLMN_LIST_SUCCESS) &&
           (rpt_ptr->cmd.net_list_cnf.found_plmn_list.plmn_list.length > 0 )&&
           (ph_ptr->network_list_type == CM_NETWORK_LIST_CSG_USER ||
           ph_ptr->network_list_type == CM_NETWORK_LIST_CSG_SELECTION_APP) &&
           ph_orig_top_ptr->act_type == CM_ACT_TYPE_PH_OBJ )
  {
    /* force the pref's */


     (void)cmph_force_pref_on_the_fly(
                                 cm_ss,
                                 cmph_ptr(),
                                 SD_SS_PREF_REAS_RESTORE,
                                 CM_ACT_TYPE_PH_OBJ,
                                 cmph_ptr()->main_stack_info.pref_info.pref_term,
                                 cmph_ptr()->main_stack_info.pref_info.mode_pref,
                                 cmph_ptr()->main_stack_info.pref_info.band_pref,
                                 cmph_ptr()->main_stack_info.pref_info.lte_band_pref,
                                 cmph_ptr()->main_stack_info.pref_info.tds_band_pref,
                                 cmph_ptr()->main_stack_info.pref_info.plmn,
                                 cmph_ptr()->main_stack_info.pref_info.srv_domain_pref,
                                 cmph_ptr()->main_stack_info.pref_info.network_sel_mode_pref,
                                 (cm_act_id_type)cmph_ptr(),
                                 CM_ACT_UPDATE_REAS_USER,
                                 FALSE,
                                 cmph_get_acq_pri_order_per_sub(cm_ss),
                                 CM_DEFAULT_VALUE,
                                 SYS_CSG_ID_INVALID,
                                 ph_ptr->main_stack_info.pref_info.csg_rat,
                                 ph_ptr->main_stack_info.pref_info.ciot_lte_pref
                                 );


  }
  #endif
  /* Inform System determination of network list cnf report and follow
  ** return action. Added get net req return reason (success/abort)
  */

#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
  if(cmregprx_stack_info_ptr->list_search_info.list_search_type == REG_CM_NETWORK_LIST_REQ_GPS_ADVANCE_SEARCH)
  {
     if(rpt_ptr->cmd.net_list_cnf.available_plmn_list.gps_wwan_scan_result != NULL )
	 {
	   ref_cnt_obj_add_ref(rpt_ptr->cmd.net_list_cnf.available_plmn_list.gps_wwan_scan_result); 
	 }     
  }
#endif

  cmregprx_process_sd_action(
    sd_ss_ind_misc_gw_net_list_rpt_ss
    (
      &rpt_ptr->cmd.net_list_cnf.rplmn,
      &rpt_ptr->cmd.net_list_cnf.available_plmn_list,
      cmregprx_map_list_cnf_from_reg_to_sys (rpt_ptr->cmd.net_list_cnf.status),
      SD_SS_MAIN,
      NULL
    ),
    cmregprx_stack_info_ptr
  );

  return;
} /* cmregprx_handle_plmn_list_search_cnf */

/*===========================================================================

FUNCTION cmregprx_active_net_list_cnf_hdlr

DESCRIPTION
  This function handles the service indication report from REG in ACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if the network list cnf report was expected.
  2) Call the sd_ss_ind_misc_gw_net_list_rpt() SD indication and process
     return SD action.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_active_net_list_cnf_hdlr
(

        cm_rpt_type                    *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_NETWORK_LIST_CNF );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the network list cnf. is expected and is it the right one?.
  */
  if ( cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_NET_LIST_CNF ||
       cmregprx_stack_info_ptr->reg_trans_id !=
                               rpt_ptr->cmd.net_list_cnf.transaction_id
     )
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


     /* Network list cnf. not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_1("Unexpected network list cnf. recvd, substate %d",
                       cmregprx_stack_info_ptr->substate);
     return;

  } /* if ( ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
       If the net_list respnse received from NAS is not Partial , then only then update
       the state to NULL
  */

  if (rpt_ptr->cmd.net_list_cnf.status != REG_CM_NETWORK_LIST_PARTIAL)\
  {

  /* Received the net. list cnf, update the substate to NULL.
  */
  cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                            cmregprx_stack_info_ptr
                          );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ( cmregprx_stack_info_ptr->list_search_info.list_search_type )
  {

    /* Handle CSG list search same way as that of PLMN LIST SEARCH */
    case REG_CM_NETWORK_LIST_REQ_CSG_LIST_SEARCH:
    case REG_CM_NETWORK_LIST_REQ_PLMN_LIST_SEARCH:
    case REG_CM_NETWORK_LIST_REQ_PRIORITY_PLMN_SEARCH_BGND:
    case REG_CM_NETWORK_LIST_REQ_CELL_SEARCH:
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
    case REG_CM_NETWORK_LIST_REQ_GPS_ADVANCE_SEARCH:
#endif

      cmregprx_handle_plmn_list_search_cnf(rpt_ptr,cmregprx_stack_info_ptr);
      break;

    default:
      CMREGPRX_MSG_HIGH_1("Unexpected list_search_type recvd %d",
                        cmregprx_stack_info_ptr->list_search_info.list_search_type);

      break;

  }

  if(cmregprx_stack_info_ptr->list_search_info.list_search_type != 
      REG_CM_NETWORK_LIST_REQ_CELL_SEARCH && 
      cmregprx_stack_info_ptr->list_search_info.list_search_type != 
      REG_CM_NETWORK_LIST_REQ_GPS_ADVANCE_SEARCH)
  {
    cmlog_plmn_list_event( &rpt_ptr->cmd.net_list_cnf.available_plmn_list.plmn_list );
  }

  return;
} /*  cmregprx_active_net_list_cnf_hdlr() */



/*===========================================================================

FUNCTION cmregprx_net_list_cnf_hdlr

DESCRIPTION
  This function handles the service indication report from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_net_list_cnf_hdlr
(

        cm_rpt_type                    *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_NETWORK_LIST_CNF );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmregprx_stack_info_ptr = &(cmregprx_info_ptr->stack_info);

  #ifdef FEATURE_CM_DEBUG_BUFFER
  cmdbg_add_to_dbg_buffer(CM_BUFF_RPT_TYPE, CM_NETWORK_LIST_CNF, ((void*)&(rpt_ptr->cmd.net_list_cnf)));
  #endif
  
  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
      cmregprx_active_net_list_cnf_hdlr( rpt_ptr,
                                         cmregprx_stack_info_ptr
                                       );
      break;

    case CMREGPRX_STATE_INACTIVE:
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in ACTIVE state.
      */
      CMREGPRX_MSG_HIGH_1("Received network list conf. in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;
} /* cmregprx_net_list_cnf_hdlr() */
/*===========================================================================
FUNCTION cmregprx_active_sim_state_update_ind_hdlr

DESCRIPTION
  This function handles the sim state update ind report from REG in ACTIVE
  RegProxy's state.

  In this state handler function, RegProxy will
  1) Validate if the sim state update indication report was expected.
  2) Map the sim state update ind report to appropriate SD indication and process
     return SD action.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_active_sim_state_update_ind_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type     *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmregprx_sim_state_update_s_type  *cmregprx_sim_state_update_ptr = cmregprx_get_sim_state_update_info();

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_SIM_STATE_UPDATE_IND );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );
  CM_ASSERT( cmregprx_sim_state_update_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the sim state update indication is received at the right time
  */
  if ( cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_SRV_CNF )
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* sim state update ind not expected in WAIT_SRV_CNF, NAS needs to send SRV_CNF first before sending
     ** sim state update indication. So ignore the report.
     */
     CMREGPRX_MSG_HIGH_1("Unexpected sim state update ind. recvd, substate %d",
                       cmregprx_stack_info_ptr->substate);
     return;

  }
  else
  {
    /* If there is a voice call or a emergency call, buffer the report
    ** sim_state_update_ind from NAS.
    */

    if((cmcall_is_there_a_call_type_per_sub(SYS_MODEM_AS_ID_1,
          CM_CALL_TYPE_VOICE, NULL)!= CM_CALL_ID_INVALID) ||
       (cmcall_is_there_a_call_type_per_sub(SYS_MODEM_AS_ID_1,
          CM_CALL_TYPE_EMERGENCY, NULL)!= CM_CALL_ID_INVALID))
    {

      cmregprx_sim_state_update_ptr->update_pending = TRUE;
      cmregprx_sim_state_update_ptr->sim_state = rpt_ptr->cmd.sim_state_update_ind.sim_state;

    }
    else{

      mmoc_cmd_prot_gw_sim_state_update_ind(rpt_ptr->cmd.sim_state_update_ind.sim_state);

    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

}



/*===========================================================================

FUNCTION cmregprx_active_camped_ind_hdlr

DESCRIPTION
  This function handles the camped indication report from REG in ACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if the camped indication report was expected.
  2) Map the camped ind report to appropriate SD indication and process
     return SD action.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_active_camped_ind_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  /* Camped ind buffer pointer */
   cm_rpt_type *camped_ind_buf_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_CAMPED_IND );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Notify Policy Manager on CAMP_IND. During DOMAIN_CAMPED, policyman will
  ** ignored the PLMN_ID stored in the PLMN field in ss_event because this is the old
  ** PLMN, instead it will use the PLMN reported from this function.
  **
  ** The logic is that if we're moving from no-service, we will notify limited service
  ** However, if we currently have service, we will notify the old service status                                                                                                                                         .
  ** Currently, PM doesn't differentiate the limited/full srv in this case, but we just                                                                                                                                                                                       .
  ** make sure this compatible with future expansion.                                                                                 .                                                                                                                                       .
  */

  {
  
  #ifndef FEATURE_TRIM_CMPMPRX  
    policyman_cm_serving_info_t   camp_ind;

    cmss_get_camped_srv_info(SD_SS_MAIN, &camp_ind.srv_status);

    camp_ind.id_type    = SYS_SYS_ID_TYPE_UMTS;
    camp_ind.srv_domain = SYS_SRV_DOMAIN_CAMPED;
    camp_ind.plmn       = rpt_ptr->cmd.camped_ind.plmn;
    camp_ind.sys_mode   = cmregprx_map_sys_rat_to_sd_mode(
                                        rpt_ptr->cmd.camped_ind.active_rat);
  

    CMREGPRX_MSG_HIGH_1("PM callback: srv_status=%d",camp_ind.srv_status);

    policyman_cm_serving_info_cb(&camp_ind);
#endif

    CMREGPRX_MSG_MED_2("intl_srv_status=%d, no_srv_uptime=%ld",
                cmss_intl_srv_info_ptr(CM_SS_MAIN)->srv_status,cmss_get_no_srv_uptime(CM_SS_MAIN));

    #if defined(CM_GW_SUPPORTED)

    /* If UE is currently OOS, shorten SR on ongoing CS calls. */
    if ( cmss_ptr()->info.srv_status == SYS_SRV_STATUS_LIMITED ||
         (!sys_srv_status_is_srv(cmss_intl_srv_info_ptr(CM_SS_MAIN)->srv_status)&&
          cmss_get_no_srv_uptime(CM_SS_MAIN) != CMSS_NO_SRV_UPTIME_NONE )
       )
    {
        cmcall_shorten_sr_duration( CM_SS_MAIN, cmregprx_map_sys_rat_to_sd_mode(rpt_ptr->cmd.camped_ind.active_rat) );
    }
	#endif
  }

  
  
  /* Check if the camped indication is received at the right time
  */
  if ( cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_SRV_CNF )
  {
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* Camped ind. not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_1("Unexpected camped ind. recvd, substate %d",
                       cmregprx_stack_info_ptr->substate);
     return;
  } /* if ( ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cmregprx_stack_info_ptr->camped_ind_buf == NULL)
  {
    cmregprx_stack_info_ptr->camped_ind_buf = (cm_rpt_type*)
             cm_mem_malloc (sizeof (cm_rpt_type));
  }

  camped_ind_buf_ptr = cmregprx_stack_info_ptr->camped_ind_buf;

  *camped_ind_buf_ptr = *rpt_ptr;

  CMREGPRX_MSG_MED_0 ("Allocated camped ind and copied");

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform System determination of camped indication and follow return
  ** action.
  */
  #ifdef FEATURE_MMODE_GW
  if ((rpt_ptr->cmd.camped_ind.active_rat != SYS_RAT_LTE_RADIO_ACCESS)
      #if defined(FEATURE_CIOT)
      && (rpt_ptr->cmd.camped_ind.active_rat != SYS_RAT_LTE_M1_RADIO_ACCESS)
      && (rpt_ptr->cmd.camped_ind.active_rat != SYS_RAT_LTE_NB1_RADIO_ACCESS) 
      #endif
     )
  {
    sd_band_u_type active_band ;
    #ifdef FEATURE_GSM_BAND_PREF
    active_band = cmregprx_get_active_band(rpt_ptr->cmd.camped_ind.active_rat,
                                           rpt_ptr->cmd.camped_ind.active_band);
    #else
    active_band.chgwt_band = SD_SS_BAND_PREF_GSM_DCS_1800;
    #endif /* FEATURE_GSM_BAND_PREF */

    cmregprx_process_sd_action( cmregprx_map_camped_ind_to_sd_ind
                                ( rpt_ptr->cmd.camped_ind.active_rat,
                                  active_band,
                                cmregprx_stack_info_ptr
                                ),
                                cmregprx_stack_info_ptr
                              );
  }
  else
  #endif
  {
    #ifdef FEATURE_CM_LTE
    sd_band_u_type active_band ;
    active_band.lte_band = rpt_ptr->cmd.camped_ind.active_band.lte_active_band;

    if (cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
    {
      cmregprx_stack_info_ptr->lte_connected_mode = FALSE;
    }
    cmregprx_process_sd_action( cmregprx_map_camped_ind_to_sd_ind
                                  (
                                    rpt_ptr->cmd.camped_ind.active_rat,
                                    active_band,
                                    cmregprx_stack_info_ptr
                                  ),
                                cmregprx_stack_info_ptr
                              );
    #endif

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
} /*  cmregprx_active_camped_ind_hdlr() */

/*===========================================================================

FUNCTION cmregprx_active_data_suspended_ind_hdlr

DESCRIPTION
  This function handles ps data suspend from MM in the active state.
  sd api is called to pass data suspend to clients

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void               cmregprx_active_data_suspended_ind_hdlr
(

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform System determination of data suspend and follow return
  ** action.
  */
  cmregprx_process_sd_action(
                      sd_ss_ind_gw_data_suspended_ss(SD_SS_MAIN),
                      cmregprx_stack_info_ptr
                            );

  return;

} /* cmregprx_active_data_suspended_ind_hdlr() */


#if defined (FEATURE_CM_LTE) || defined (FEATURE_WRLF_SYSTEM_SEL)
/*===========================================================================

FUNCTION cmregprx_active_suitable_search_ind_hdlr

DESCRIPTION
  This function passes the indication that ends the suitable cell search
  in LTE/WCDMA Connected mode.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void               cmregprx_active_suitable_search_end_ind_hdlr
(

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  cmph_s_type                  *ph_ptr = cmph_ptr();
  cmph_sub_config_s_type       *ph_sub_config_ptr;


  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  ph_sub_config_ptr = cmph_get_sub_config_ptr(SYS_MODEM_AS_ID_1);

  if(ph_sub_config_ptr == NULL)
  {
    return;
  }


  #if defined (FEATURE_CM_LTE)
   /* Check whether the received IND can be processed in the current substate or not */
  /* Suitable search end indication is expected only in CMREGPRX_SUBSTATE_WAIT_SRV_CNF
  ** or NULL substate.If it is received in other states, reset LTE connected
  ** mode state in SD and CM without invoking SD scripts and process SD return
  ** action. If not, this could cause cmregprx to move into a different state or lose
  ** track of current MMOC transaction state
  */
  if (cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_NULL  &&
      cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_SRV_CNF)
  {
    CMREGPRX_MSG_HIGH_1("Received CM_SUITABLE_SEARCH_END_IND in wrong state %d",
                       cmregprx_stack_info_ptr->substate);

    if(cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
    {
      cmregprx_stack_info_ptr->lte_connected_mode = FALSE;
      sd_ss_reset_rlf_conn_mode_status ( SD_SS_MAIN );
    }
    if(cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_STOP_CNF)
    {
      sd_ss_ind_misc_srv_lost();
    }
    return;
  }
  
  if(cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
  {
    cmregprx_stack_info_ptr->lte_connected_mode = FALSE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Inform System determination of the end of LTE Connected state and
    ** follow return action.
    */
    cmregprx_process_sd_action(
        sd_ss_ind_lte_suitable_search_end (SD_SS_MAIN,NULL),
                                cmregprx_stack_info_ptr  );
    return;
  }
  #endif /* FEATURE_CM_LTE */


  /* Indication not expected or incorrect, ignore report.
  */
  CMREGPRX_MSG_HIGH_0("UE not in LTE or WCDMA Connected mode,suitable search end ind ignored");
  return;


} /* cmregprx_active_suitable_search_end_ind_hdlr() */
#endif /* FEATURE_CM_LTE) || FEATURE_WRLF_SYSTEM_SEL */

#if defined ( FEATURE_WRLF_SYSTEM_SEL) && defined(FEATURE_WCDMA)
/*===========================================================================

FUNCTION cmregprx_active_disable_bplmn_ind_hdlr

DESCRIPTION
  This function passes the indication that diable/enable the bplmn search
  during WCDMA connected mode.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void               cmregprx_active_disable_bplmn_ind_hdlr
(
        const cm_rpt_type        *rpt_ptr,
        /* Pointer to the CM report message.*/


        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  return;

} /* cmregprx_active_disable_bplmn_ind_hdlr() */
#endif /* FEATURE_WRLF_SYSTEM_SEL*/

/*===========================================================================

FUNCTION cmregprx_data_suspended_ind_hdlr

DESCRIPTION
  This function handles the data suspend == TRUE indication from MM.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                cmregprx_data_suspended_ind_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */
)
{

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmregprx_stack_info_ptr = &(cmregprx_info_ptr->stack_info);

  CMREGPRX_MSG_HIGH_1 ("CM_MM_PS_DATA_SUSPEND_IND recvd in %d substate",
                          cmregprx_stack_info_ptr->substate);
  

  #ifdef FEATURE_CM_DEBUG_BUFFER
  cmdbg_add_to_dbg_buffer(CM_BUFF_RPT_TYPE, CM_MM_PS_DATA_SUSPEND_IND, ((void*)&(rpt_ptr->cmd.data_suspend_ind)));
  #endif

  /* New data suspend primitive will be given only when CMREGPRX is
  ** waiting for CMREGPRX_SUBSTATE_WAIT_NET_LIST_CNF
  */

  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
      cmregprx_active_data_suspended_ind_hdlr (cmregprx_stack_info_ptr);

      break;

    case CMREGPRX_STATE_INACTIVE:
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in ACTIVE state.
      */
      CMREGPRX_MSG_HIGH_1("Received ps data suspend ind in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;

} /* cmregprx_data_suspended_ind_hdlr () */

#if defined (FEATURE_CM_LTE)|| defined (FEATURE_WRLF_SYSTEM_SEL)
/*===========================================================================

FUNCTION cmregprx_reset_connected_mode_status

DESCRIPTION
  Reset connected mode status

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/

static void cmregprx_reset_connected_mode_status (

cmregprx_stack_info_s_type* cmregprx_stack_info_ptr

)
{
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_CM_LTE
  if(cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
  {
    cmregprx_stack_info_ptr->lte_connected_mode = FALSE;
  }
  #endif


  sd_ss_reset_rlf_conn_mode_status (SD_SS_MAIN);
}
/*===========================================================================

FUNCTION cmregprx_suitable_search_end_hdlr

DESCRIPTION
  This function handles the indication that ends the LTE Connected mode

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                      cmregprx_suitable_search_end_hdlr
(

        const cm_rpt_type         *rpt_ptr,
          /* Pointer to the CM report message.
          */

        cmregprx_info_s_type      *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */
)
{
  cmregprx_stack_info_s_type* cmregprx_stack_info_ptr;
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_MSG_HIGH_2("AS_ID=%d, stack_id %d, CM_SUITABLE_SEARCH_END_IND", 
                        rpt_ptr->cmd.lte_connected_mode_ind.as_id,
                        rpt_ptr->cmd.lte_connected_mode_ind.stack_id );

  cmregprx_stack_info_ptr = &(cmregprx_info_ptr->stack_info);

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
      cmregprx_active_suitable_search_end_ind_hdlr (cmregprx_stack_info_ptr);

      break;

    case CMREGPRX_STATE_INACTIVE:
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in ACTIVE state.
      */
      CMREGPRX_MSG_HIGH_1("Received suitable search end in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_stack_info_ptr->state ) */

  return;

} /* cmregprx_suitable_search_end_hdlr () */
#endif /* (FEATURE_CM_LTE)||(FEATURE_WRLF_SYSTEM_SEL) */


#if defined ( FEATURE_WRLF_SYSTEM_SEL) && defined(FEATURE_WCDMA)
/*===========================================================================

FUNCTION cmregprx_disable_bplmn_ind_hdlr

DESCRIPTION
  This function handles the indication that ends the WCDMA Connected mode

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                cmregprx_disable_bplmn_ind_hdlr
(

       const cm_rpt_type         *rpt_ptr,
       /* Pointer to the CM report message.
            */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */
)
{

  cmregprx_stack_info_s_type   *cmregprx_stack_info_ptr;

  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmregprx_stack_info_ptr = &(cmregprx_info_ptr->stack_info);;


  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
    case CMREGPRX_STATE_INACTIVE:


      /* CMREGPRX will be in INACTIVE state when in deep sleep.
      ** this report needs to be handled deep sleep aswell.
      */
      CMREGPRX_MSG_HIGH_1("Handling disable bplmn ind in  state %d",
                         cmregprx_stack_info_ptr->state);
      cmregprx_active_disable_bplmn_ind_hdlr (rpt_ptr,cmregprx_stack_info_ptr);

      break;

    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in ACTIVE state.
      */
      CMREGPRX_MSG_HIGH_1("Received disable bplmn ind in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_stack_info_ptr->state ) */

  return;

} /* cmregprx_disable_bplmn_ind_hdlr () */

#endif /* FEATURE_WRLF_SYSTEM_SEL */


/*===========================================================================

FUNCTION cmregprx_sim_state_update_ind_hdlr

DESCRIPTION
  This function handles the sim state update indication report from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_sim_state_update_ind_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_SIM_STATE_UPDATE_IND );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmregprx_stack_info_ptr = &(cmregprx_info_ptr->stack_info);

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
    case CMREGPRX_STATE_INACTIVE:
      cmregprx_active_sim_state_update_ind_hdlr( rpt_ptr,
                                       cmregprx_stack_info_ptr
                                     );
      break;

    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should not receive sim state update indication from REG in this state
      */
      CMREGPRX_MSG_HIGH_1("Received sim state update ind in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;
} /* cmregprx_sim_state_update_ind_hdlr() */


/*===========================================================================

FUNCTION cmregprx_camped_ind_hdlr

DESCRIPTION
  This function handles the camped indication report from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_camped_ind_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_CAMPED_IND );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_MSG_HIGH_1("AS_ID=%d, CM_CAMPED_IND",
                        rpt_ptr->cmd.camped_ind.as_id);

  cmregprx_stack_info_ptr = &(cmregprx_info_ptr->stack_info);

  #ifdef FEATURE_CM_DEBUG_BUFFER
  cmdbg_add_to_dbg_buffer(CM_BUFF_RPT_TYPE, CM_CAMPED_IND, ((void*)&(rpt_ptr->cmd.camped_ind)));
  #endif

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
      cmregprx_active_camped_ind_hdlr( rpt_ptr,
                                       cmregprx_stack_info_ptr
                                     );
      break;

    case CMREGPRX_STATE_INACTIVE:
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in ACTIVE state.
      */
      CMREGPRX_MSG_HIGH_1("Received camped ind in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;
} /* cmregprx_camped_ind_hdlr() */

#if defined(FEATURE_GW_ON_DEMAND_HPLMN_SRCH)

/*===========================================================================

FUNCTION cmregprx_hplmn_srch_cnf_hdlr

DESCRIPTION
  This function handles hplmn srch cnf from REG.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                cmregprx_hplmn_srch_cnf_hdlr
(
        const   cm_rpt_type            *cm_rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */
)
/*lint -esym(715, cm_rpt_ptr, cmregprx_info_ptr) */
/*lint -save -e818 */
{

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmregprx_stack_info_ptr = &(cmregprx_info_ptr->stack_info);

  CMREGPRX_MSG_HIGH_4("AS_ID=%d, state = %d, substate = %d Recvd HPLMN_SRCH_CNF srch init status = %d",
                        cm_rpt_ptr->cmd.hplmn_serach_cnf.as_id,
                        cmregprx_stack_info_ptr->state,
                        cmregprx_stack_info_ptr->substate,
                 cm_rpt_ptr->cmd.hplmn_serach_cnf.search_initiated);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED(cmregprx_stack_info_ptr);

  return;

} /* cmregprx_hplmn_srch_cnf_hdlr () */
/*lint +esym(715, cm_rpt_ptr, cmregprx_info_ptr) */
/*lint -restore */

#endif /* FEATURE_GW_ON_DEMAND_HPLMN_SRCH */





/*===========================================================================
=============================================================================
=============================================================================
==================== REG STOP CNF. REPORT HANDLER ===========================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_active_stop_cnf_hdlr

DESCRIPTION
  This function handles the stop cnf report from REG in ACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if the stop mode cnf report was expected.
  2) If validated correctly, the RegProxy will send the deactivated report
     to MMoC.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_active_stop_cnf_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  #ifdef FEATURE_MMODE_SC_SVLTE
  cmph_s_type                *ph_ptr  = cmph_ptr();
  sys_modem_as_id_e_type      as_id = SYS_MODEM_AS_ID_1;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_STOP_MODE_CNF );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the stop cnf. is received in SUSBSTATE_NULL.
  */
  if ( cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_NULL )
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


     /* Stop cnf. received in SUBSTATE_NULL, do ERR_FATAL.
     */
     CMREGPRX_MSG_HIGH_0("Unexpected stop cnf. recvd in substate null");
     return;

  } /* if ( ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the stop cnf. is expected and is it the right one?.
  */
  if ( !(cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_STOP_CNF)
       ||
       cmregprx_stack_info_ptr->reg_trans_id !=
                               rpt_ptr->cmd.stop_mode_cnf.transaction_id
     )
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


     /* Stop cnf. not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_1("Unexpected stop cnf. recvd, substate %d",
                        cmregprx_stack_info_ptr->substate);
     return;

  } /* if ( ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If CM sent target RAT as LTE to client during DO->LTE reselection,
  ** need to reset it to NONE indicating LTE is shut down
  */
  cmss_inform_target_rat(SYS_SYS_MODE_NONE);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* indicate that UE is no longer emergency attached
  */
  #if defined(FEATURE_MMODE_9X05_EMERG_CALL)
  sd_ss_update_emc_attached(FALSE);
  #endif


  /* Send the deactivated indication or confirmation to MMoC.
  */
  cmregprx_send_mmoc_deactd_rpt( cmregprx_stack_info_ptr );


  return;
} /*  cmregprx_active_stop_cnf_hdlr() */


/*===========================================================================

FUNCTION cmregprx_inactive_stop_cnf_hdlr

DESCRIPTION
  This function handles the stop cnf report from REG in INACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if the stop mode cnf report was expected.
  2) The RegProxy will sent the deactivated report to MMoC.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_inactive_stop_cnf_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_STOP_MODE_CNF );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the stop cnf. is received in SUSBSTATE_NULL.
  */
  if ( cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_NULL )
  {
     /* Stop cnf. received in SUBSTATE_NULL, do ERR_FATAL.
     */
     CMREGPRX_MSG_HIGH_0("Unexpected stop cnf. recvd in substate null");
     return;
  } /* if ( ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the stop cnf. is expected and is it the right one?.
  */
  if ( !(cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_STOP_CNF)
       ||
       cmregprx_stack_info_ptr->reg_trans_id !=
                               rpt_ptr->cmd.stop_mode_cnf.transaction_id
     )
  {
     /* Stop cnf. not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_1("Unexpected stop cnf. recvd, substate %d",
                        cmregprx_stack_info_ptr->substate);
     return;
  } /* if ( ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the deactivated indication or confirmation to MMoC.
  */
  cmregprx_send_mmoc_deactd_rpt(cmregprx_stack_info_ptr);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
} /*  cmregprx_inactive_stop_cnf_hdlr() */


/*===========================================================================

FUNCTION cmregprx_stop_cnf_hdlr

DESCRIPTION
  This function handles the stop cnf. report from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_stop_cnf_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr=NULL;

  #ifdef FEATURE_LTE_TO_HDR_OH
  cmsds_s_type *sds_ptr = cmsds_ptr();
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_STOP_MODE_CNF );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  if(cmregprx_stack_info_ptr == NULL)
  {
    cmregprx_stack_info_ptr = &(cmregprx_info_ptr->stack_info);
  }

  CMREGPRX_MSG_HIGH_4("STOP_MODE_CNF: tran-id=%d, state=%d, sub_state=%d, stop mode reas: %d",
                          rpt_ptr->cmd.stop_mode_cnf.transaction_id,
                          cmregprx_stack_info_ptr->state,
                          cmregprx_stack_info_ptr->substate,
                          cmregprx_stack_info_ptr->stop_mode_reason);

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
      cmregprx_active_stop_cnf_hdlr( rpt_ptr,
                                     cmregprx_stack_info_ptr
                                   );
      break;

    
    case CMREGPRX_STATE_INACTIVE:
      cmregprx_inactive_stop_cnf_hdlr( rpt_ptr,
                                      cmregprx_stack_info_ptr
                                    );
      break;
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in ACTIVE state.
      */
      CMREGPRX_MSG_HIGH_1("Received stop conf. in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */
  #ifdef FEATURE_LTE_TO_HDR_OH
  /*Deactivate HDR tunnel*/
  cmsds_deactivate_hdr_tunnel_req();
  switch(cmregprx_stack_info_ptr->stop_mode_reason)
  {
     case SYS_STOP_MODE_REASON_DEEP_SLEEP:
     case SYS_STOP_MODE_REASON_ALT_SCAN:
     case SYS_STOP_MODE_REASON_IRAT:
       sds_ptr->is_act_hdr_on_srv_cnf = TRUE;
       break;

     default:
       sds_ptr->is_act_hdr_on_srv_cnf = FALSE;
       break;
  }
  #endif

  return;
} /* cmregprx_stop_cnf_hdlr() */



/*===========================================================================
=============================================================================
=============================================================================
==================== REG PH STATUS CHGD CNF. REPORT HANDLER =================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_inactive_ph_status_chgd_cnf_hdlr

DESCRIPTION
  This function handles the stop cnf report from REG in ACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if the phone status changed cnf report was expected.
  2) The RegProxy will send the confirmation report to MMoC.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                        cmregprx_inactive_ph_status_chgd_cnf_hdlr
(

        const cm_rpt_type           *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type        *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  /*lint -esym(613, cmregprx_info_ptr, rpt_ptr)
  ** NOTE -save/-restore do not work with -esym()
  */

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  if( rpt_ptr == NULL )
  {
    CMREGPRX_ERR_0("NULL ptr for rpt_ptr is recvd");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( rpt_ptr->hdr.cmd != CM_PH_STATUS_CHGD_CNF)
  {
    /* Got wrong report. Ph status chgd cnf. was expected.
    */
    CMREGPRX_MSG_HIGH_0("Unexpected rpt recvd. Expected CM_PH_STATUS_CHGD_CNF" );
    return;
  }/* if() */

  CM_ASSERT( rpt_ptr->hdr.cmd == CM_PH_STATUS_CHGD_CNF );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the ph status chgd cnf. is received in SUSBSTATE_NULL.
  */
  if ( cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_NULL )
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


     /* Ph status chgd cnf. received in SUBSTATE_NULL, do ERR_FATAL.
     */
     CMREGPRX_MSG_HIGH_0("Unexpected ph status chgd cnf. recvd in substate null");
     return;

  } /* if ( ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Send the phone status ack to MMoC, check if transaction is pending.
  */
  if ( cmregprx_stack_info_ptr->mmoc_trans_id == PROT_TRANS_NONE )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* ERROR: No MMoC transaction is pending, clear the substate.
    */
    CMREGPRX_MSG_HIGH_0("No MMoC transaction pending");
    cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                              cmregprx_stack_info_ptr
                            );

    return;
  }

  /* Check if the ph status chgd cnf. is expected and is it the right one.
  */
  if(
    cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_PH_STATUS_CHGD_CNF
    ||
    cmregprx_stack_info_ptr->reg_trans_id !=
                             rpt_ptr->cmd.ph_status_chgd_cnf.transaction_id
    )
  {
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* ph status chgd cnf not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_3("Unexpected ph status chgd cnf recvd, substate %d expected reg_trans_id = %d got rpt trans_id = %d",
                       cmregprx_stack_info_ptr->substate,
                       cmregprx_stack_info_ptr->reg_trans_id,
                       rpt_ptr->cmd.ph_status_chgd_cnf.transaction_id);
     return;

  } /* if ( ) */


  if( rpt_ptr->cmd.ph_status_chgd_cnf.ph_status_chgd_status != \
                                         SYS_PH_STATUS_CHGD_SUCCESS )
  {
    CMREGPRX_MSG_HIGH_1("Ph stat chgd cnf failure, reason = %d", \
                      rpt_ptr->cmd.ph_status_chgd_cnf.ph_status_chgd_status );
    /* Common handling for either case. This is just a debug message here
    */
  }



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Substate is updated if SIM refresh is to be done on LPM oprt mode */
  cmregprx_sim_refresh_ph_stat_rpt(cmregprx_stack_info_ptr, rpt_ptr);

  if(cmregprx_sim_refresh_is_pending(cmregprx_stack_info_ptr) == FALSE)
  {
    cmregprx_send_ph_stat_cnf_rpt(cmregprx_stack_info_ptr);
  }  

  return;

  /*lint +esym(613, cmregprx_info_ptr, rpt_ptr)
  ** NOTE -save/-restore do not work with -esym()
  */

} /* cmregprx_inactive_ph_status_chgd_cnf_hdlr */

/*===========================================================================

FUNCTION cmregprx_ph_status_chgd_cnf_hdlr

DESCRIPTION
  This function handles the phone status changed cnf. report from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_ph_status_chgd_cnf_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{

  /*lint -esym(613, cmregprx_info_ptr, rpt_ptr)
  ** NOTE -save/-restore do not work with -esym()
  */
  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_PH_STATUS_CHGD_CNF );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmregprx_stack_info_ptr = &(cmregprx_info_ptr->stack_info);

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_INACTIVE:
      cmregprx_inactive_ph_status_chgd_cnf_hdlr( rpt_ptr,
                                                 cmregprx_stack_info_ptr
                                               );
      break;

    case CMREGPRX_STATE_ACTIVE:
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in INACTIVE state.
      */
      CMREGPRX_MSG_HIGH_1("Received ph status chgd conf. in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;

  /*lint +esym(613, cmregprx_info_ptr, rpt_ptr)
  ** NOTE -save/-restore do not work with -esym()
  */

} /* cmregprx_ph_status_chgd_cnf_hdlr() */


/*===========================================================================

FUNCTION cmregprx_cmd_perform_plmn_blocking

DESCRIPTION
  This function send PLMN blocking request to NAS.


DEPENDENCIES


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void cmregprx_cmd_perform_plmn_blocking (

  sys_block_plmn_e_type              plmn_block_pref,

  uint32                             blocking_interval,

  sys_modem_as_id_e_type             asubs_id

)
{
  reg_cmd_type                        *reg_ptr = NULL;
    /* Pointer to REG command buffer*/
  cmph_s_type                 *ph_ptr        = cmph_ptr();

  CM_ASSERT( ph_ptr != NULL );

  /* Get reg cmd. buffer and populate params.*/
  reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_BLOCK_PLMN_REQ );

  /* Fill in parameters. */
  if (reg_ptr != NULL)
  {
    reg_ptr->cmd.cm_block_plmn_req.type = plmn_block_pref;
    reg_ptr->cmd.cm_block_plmn_req.plmn = cmph_ptr()->main_stack_info.pref_info.plmn;
    reg_ptr->cmd.cm_block_plmn_req.timer_count = blocking_interval;
    reg_ptr->cmd.cm_block_plmn_req.cause = ph_ptr->main_stack_info.plmn_blocking_info.cause;

   
    CMREGPRX_MSG_MED_6("Send PLMN blocking request to NAS. type %d, cause:%d, timer_count %d plmn %d, %d, %d ",
               reg_ptr->cmd.cm_block_plmn_req.type,
               reg_ptr->cmd.cm_block_plmn_req.cause,
               reg_ptr->cmd.cm_block_plmn_req.timer_count,
               reg_ptr->cmd.cm_block_plmn_req.plmn.identity[0],
               reg_ptr->cmd.cm_block_plmn_req.plmn.identity[1],
               reg_ptr->cmd.cm_block_plmn_req.plmn.identity[2]);

    /* Send the request to NAS */
    cm_reg_send_cmd( reg_ptr );
  }
}

/*===========================================================================

FUNCTION cmregprx_ps_detach_cnf_hdlr

DESCRIPTION
  This function handles the ps detach cnf. report from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_ps_detach_cnf_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{

  /*lint -esym(613, cmregprx_info_ptr, rpt_ptr)
  ** NOTE -save/-restore do not work with -esym()
  */

  cmregprx_stack_info_s_type          *stack_info_ptr ;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_PS_DETACH_CNF );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  if(rpt_ptr == NULL)
  {
    CMREGPRX_MSG_HIGH_0("Unexpected rpt recvd. rpt_ptr is NULL" );
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( rpt_ptr != NULL && rpt_ptr->hdr.cmd != CM_PS_DETACH_CNF)
  {
    /* Got wrong report. PS detach cnf. was expected.
    */
    CMREGPRX_MSG_HIGH_0("Unexpected rpt recvd. Expected CM_PS_DETACH_CNF" );
    return;
  }/* if() */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  stack_info_ptr = &(cmregprx_info_ptr->stack_info);

  #ifdef FEATURE_CM_DEBUG_BUFFER
  cmdbg_add_to_dbg_buffer(CM_BUFF_RPT_TYPE, CM_PS_DETACH_CNF, ((void*)&(rpt_ptr->cmd.ps_detach_cnf)));
  #endif

  CMREGPRX_MSG_HIGH_2("CM_PS_DETACH_CNF, state=%d, substate=%d", 
                        stack_info_ptr->state,
                        stack_info_ptr->substate);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the PS DETACH CNF. is received in SUSBSTATE_NULL.
  */
  if ( stack_info_ptr->substate == CMREGPRX_SUBSTATE_NULL )
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* PS detach cnf. received in SUBSTATE_NULL, do ERR_FATAL.
     */
     CMREGPRX_MSG_HIGH_0("Unexpected ps_detach_cnf. recvd in substate null" );
     return;

  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the PS detach cnf ack to MMoC, check if transaction is pending.
  */
  if ( stack_info_ptr->mmoc_trans_id == PROT_TRANS_NONE )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* ERROR: No MMoC transaction is pending, clear the substate.
    */
    CMREGPRX_MSG_HIGH_0("No MMoC transaction pending");
    cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                                  stack_info_ptr
                            );

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the ps detach cnf. is expected and is it the right one.
  */
  if(stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_PS_DETACH_CNF)
  {
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* ps detach cnf not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_1("Unexpected ps detach cnf recvd, substate %d",
                                              stack_info_ptr->substate);
     return;

  } /* if ( ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_CM_LTE
  /* Check if PLMN blocking request is buffered to be sent.
  */
  {
   cmph_stack_info_s_type * ph_stack_info_ptr = CMPH_SS_PREF_INFO_PTR(rpt_ptr->cmd.ps_detach_cnf.as_id, cmph_ptr() );

    if(ph_stack_info_ptr->plmn_blocking_info.request_pref == \
       CM_PLMN_BLOCKING_PREF_LTE_DETACH_PLMN_BLOCKING)
    {
      cmregprx_cmd_perform_plmn_blocking(SYS_BLOCK_PLMN_BLOCK,
          ph_stack_info_ptr->plmn_blocking_info.blocking_interval,
          ph_stack_info_ptr->plmn_blocking_info.asubs_id);

      /* Reset buffer. */
      ph_stack_info_ptr->plmn_blocking_info.request_pref=CM_PLMN_BLOCKING_PREF_NONE;
      sys_plmn_undefine_plmn_id( &(ph_stack_info_ptr->plmn_blocking_info.plmn) );
      ph_stack_info_ptr->plmn_blocking_info.blocking_interval = 0;

    }
  }
  #endif


  #if defined(FEATURE_IP_CALL)
  cmipapp_send_srv_domain (CM_PS_DETACH_BY_MMODE_COMPLETE, SYS_MODEM_AS_ID_1);
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform SD */
  (void)sd_ss_ind_gw_ps_detach(SD_SS_MAIN, NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the PS detach ack. report and clear the MMoC trans_id.
  */
  mmoc_rpt_ps_detach_cnf( stack_info_ptr->mmoc_trans_id);
  stack_info_ptr->mmoc_trans_id = PROT_TRANS_NONE;

  #if defined(FEATURE_DOMAIN_SELECTION)
  cmsds_reset_during_detach ();
  #endif

  /* Update the substate to CMREGPRX_SUBSTATE_WAIT_GEN_CMD.
  */
  if (stack_info_ptr->state == CMREGPRX_STATE_ACTIVE)
  {
    cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_GEN_CMD,
                              stack_info_ptr
                            );
  }
  else
  {
    /* Set RegProxy's state to INACTIVE */
    cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                               stack_info_ptr
                             );
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

  /*lint +esym(613, cmregprx_info_ptr, rpt_ptr)
  ** NOTE -save/-restore do not work with -esym()
  */

} /* cmregprx_ps_detach_cnf_hdlr() */

/*===========================================================================
=============================================================================
=============================================================================
==================== REG SIM AVAIL CNF. REPORT HANDLER ======================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_inactive_sim_avail_cnf_hdlr

DESCRIPTION
  This function handles the SIM available cnf report from REG in INACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if the sim avail cnf report was expected.
  2) If validated correctly, the RegProxy will send the phone status cnf
     report to MMoC.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_inactive_sim_avail_cnf_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( rpt_ptr != NULL &&
                   ( rpt_ptr->hdr.cmd == CM_SIM_AVAILABLE_CNF ||
                     rpt_ptr->hdr.cmd == CM_SIM_NOT_AVAILABLE_CNF)
                 );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cmregprx_sim_refresh_is_pending(cmregprx_stack_info_ptr))
  {
    cmregprx_sim_refresh_ph_stat_rpt(cmregprx_stack_info_ptr, rpt_ptr);
    return;
  }

  /* Check if the cnf. is expected and is it the right one?.
  */
  if ( cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_SIM_AVL_CNF ||
       ( rpt_ptr->hdr.cmd == CM_SIM_AVAILABLE_CNF &&
         cmregprx_stack_info_ptr->reg_trans_id !=
                           rpt_ptr->cmd.sim_available_cnf.transaction_id
       ) ||
       ( rpt_ptr->hdr.cmd == CM_SIM_NOT_AVAILABLE_CNF &&
         cmregprx_stack_info_ptr->reg_trans_id !=
                            rpt_ptr->cmd.sim_not_available_cnf.transaction_id
       )
     )
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


     /* SIM availability cnf. not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_1("Unexpected SIM availability cnf. recvd, substate %d",
                       cmregprx_stack_info_ptr->substate);
     return;

  } /* if ( ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Received the sim. avail cnf, update the substate to NULL.
  */
  cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                            cmregprx_stack_info_ptr
                          );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Send the phone status ack to MMoC, check if transaction is pending.
  */
  if ( cmregprx_stack_info_ptr->mmoc_trans_id == PROT_TRANS_NONE )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* ERROR: No MMoC transaction is pending, clear the substate.
    */
    CMREGPRX_MSG_HIGH_0("No MMoC transaction pending");
    cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                              cmregprx_stack_info_ptr
                            );

    return;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Send the phone status ack. report and clear the MMoC trans_id.
    */

    mmoc_rpt_ph_stat_chgd_cnf( cmregprx_stack_info_ptr->mmoc_trans_id,
                               SD_MODE_GWL ,MMOC_PH_STAT_NO_ERROR);
    cmregprx_stack_info_ptr->mmoc_trans_id = PROT_TRANS_NONE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Update the RegProxy's substate
  */
  cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                            cmregprx_stack_info_ptr
                          );

  return;
} /*  cmregprx_inactive_sim_avail_cnf_hdlr() */



/*===========================================================================

FUNCTION cmregprx_sim_avail_cnf_hdlr

DESCRIPTION
  This function handles the SIM avail. cnf. report from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_sim_avail_cnf_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL &&
               ( rpt_ptr->hdr.cmd == CM_SIM_AVAILABLE_CNF ||
                 rpt_ptr->hdr.cmd == CM_SIM_NOT_AVAILABLE_CNF
               )
             );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  CMREGPRX_MSG_HIGH_2("NAS->CM: SIM_AVAILABLE_CNF: as_id=%d, stack_id=%d", 
                        rpt_ptr->cmd.sim_available_cnf.as_id, 
                        rpt_ptr->cmd.sim_available_cnf.stack_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmregprx_stack_info_ptr = &(cmregprx_info_ptr->stack_info);

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_INACTIVE:
      cmregprx_inactive_sim_avail_cnf_hdlr( rpt_ptr,
                                            cmregprx_stack_info_ptr
                                          );
      break;

    case CMREGPRX_STATE_ACTIVE:
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in INACTIVE state.
      */
      CMREGPRX_MSG_HIGH_1("Received sim avail conf. in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Notify this event to ATCOP */
  if ( rpt_ptr->hdr.cmd == CM_SIM_AVAILABLE_CNF )
  {
    cmph_send_sim_available_ph_event();
  }

  return;
} /* cmregprx_sim_avail_cnf_hdlr() */


/*===========================================================================
=============================================================================
=============================================================================
==============================  MMOC -> RegProxy commands ===================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_cmd_gwl_activate_protocol

DESCRIPTION
  This function should be called when the protocol related to GSM/WCDMA/LTE
  needs to be activated.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  void                           cmregprx_cmd_gwl_activate_protocol
(
        prot_act_e_type                act_reason,
           /* Reason for activating the protocol.
           */

        sd_ss_e_type                   ss
          /* system selection type
          */
)
{

  cmregprx_cmd_msg_s_type  *cmd_ptr;
  cmregprx_info_s_type*    cmregprx_info_ptr = cmregprx_get_info_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the command buffer.
  */
  cmd_ptr = cmregprx_get_cmd_buf_else_err_fatal( );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in all the fields.
  */
  cmd_ptr->name                  = PROT_CMD_ACTIVATE;
  cmd_ptr->param.act_prot.reason = act_reason;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command.
  */
  cmregprx_send_cmd( cmd_ptr,
                     cmregprx_info_ptr
                   );

  return;

} /* cmregprx_cmd_gwl_activate_protocol() */

/*===========================================================================

FUNCTION cmregprx_cmd_gwl_deactivate_protocol

DESCRIPTION
  This function should be called when the protocol related to GSM/WCDMA/LTE
  needs to be deactivated.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  void                           cmregprx_cmd_gwl_deactivate_protocol
(
        prot_deact_e_type              deact_reason,
           /* Reason for deactivating the protocol.
           */

        prot_trans_type                trans_id
           /* Trans_id for the deactivate protocol request.
           */
)
{
  cmregprx_cmd_msg_s_type  *cmd_ptr;
  cmregprx_info_s_type*    cmregprx_info_ptr = cmregprx_get_info_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the command buffer.
  */
  cmd_ptr = cmregprx_get_cmd_buf_else_err_fatal( );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in all the fields.
  */
  cmd_ptr->name                      = PROT_CMD_DEACTIVATE;
  cmd_ptr->param.deact_prot.reason   = deact_reason;
  cmd_ptr->param.deact_prot.trans_id = trans_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command.
  */
  cmregprx_send_cmd( cmd_ptr,
                     cmregprx_info_ptr
                   );

  return;

} /* cmregprx_cmd_gwl_deactivate_protocol() */


/*===========================================================================

FUNCTION cmregprx_cmd_gwl_generic_cmd

DESCRIPTION
  This function should be called when the generic command needs to be
  processed by the RegProxy. The generic command could be because of Pref.
  system selection change, reselection time out, Get GSM/WCDMA/LTE networks
  list or Terminate Get GSM/WCDMA/LTE networks list.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  void                           cmregprx_cmd_gwl_generic_cmd
(
        const prot_gen_cmd_s_type            *gen_cmd_ptr
           /* Pointer to the generic protocol command.
           */
)
{
  cmregprx_cmd_msg_s_type  *cmd_ptr;
  cmregprx_info_s_type*    cmregprx_info_ptr = cmregprx_get_info_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( gen_cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the command buffer.
  */
  cmd_ptr = cmregprx_get_cmd_buf_else_err_fatal( );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  cmd_ptr->name                      = PROT_CMD_GENERIC;
  cmd_ptr->param.gen_cmd             = *gen_cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_MSG_HIGH_1("Sending PROT_CMD_GENERIC from mmoc to cmregprx, trans_id=%d",
                          gen_cmd_ptr->trans_id);


  /* Post the command.
  */
  cmregprx_send_cmd( cmd_ptr,
                     cmregprx_info_ptr
                   );

  return;

} /* cmregprx_cmd_gwl_generic_cmd() */


/*===========================================================================

FUNCTION cmregprx_cmd_gwl_ph_stat_chgd

DESCRIPTION
  This function should be called to inform the RegProxy of the phone status
  changed. The phone status change could be because of operating mode change
  or subscription change.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           cmregprx_cmd_gwl_ph_stat_chgd
(

        prot_ph_stat_chg_e_type        chg_type,
           /* Type of the phone status change.
           */

        cm_subs_avail_e_type           is_gwl_subs_avail,
           /* Current GSM/WCDMA subscription available flag.
           */

        prot_oprt_mode_e_type          oprt_mode,
           /* Current operating mode of the phone.
           */

        prot_trans_type                trans_id,
           /* Transaction id for the phone status change command.
           */

        prot_subsc_chg_e_type          prot_subsc_chg,
         /* Enumeration of protocols that have a change in
         ** subscription available status
         */
        sd_ss_mode_pref_e_type         mode_pref
         /* mode preference of this subscription */
)
{
  cmregprx_cmd_msg_s_type  *cmd_ptr;
  cmregprx_info_s_type*    cmregprx_info_ptr = cmregprx_get_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the command buffer.
  */
  cmd_ptr = cmregprx_get_cmd_buf_else_err_fatal( );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_0("ph_stat_chgd: MMOC->CM");

  /* Fill in all the fields.
  */
  cmd_ptr->name                                  = PROT_CMD_PH_STAT_CHGD;
  cmd_ptr->param.gw_ph_stat_cmd.chg_type          = chg_type;
  cmd_ptr->param.gw_ph_stat_cmd.is_gwl_subs_avail = is_gwl_subs_avail;
  cmd_ptr->param.gw_ph_stat_cmd.oprt_mode        = oprt_mode;
  cmd_ptr->param.gw_ph_stat_cmd.trans_id         = trans_id;
  cmd_ptr->param.gw_ph_stat_cmd.prot_subsc_chg   = prot_subsc_chg;

  cmd_ptr->param.gw_ph_stat_cmd.mode_pref        =
                                   cmregprx_map_sd_mode_pref_to_sys(mode_pref);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command.
  */
  cmregprx_send_cmd( cmd_ptr, cmregprx_info_ptr );

  return;

} /* cmregprx_cmd_gw_ph_stat_chgd() */

/*===========================================================================

FUNCTION cmregprx_cmd_ps_detach

DESCRIPTION
  This function should be called to inform the RegProxy of the ps detach req to be sent to NAS.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void                          cmregprx_cmd_ps_detach
(
          sys_ps_detach_e_type           type,
          /* Type of detach requested
          */

          prot_trans_type                trans_id
          /* Transaction id for the detach cmd
          */

)
{
  cmregprx_cmd_msg_s_type  *cmd_ptr;
  cmregprx_info_s_type*    cmregprx_info_ptr = cmregprx_get_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the command buffer.
  */
  cmd_ptr = cmregprx_get_cmd_buf_else_err_fatal( );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  cmd_ptr->name                                  = PROT_CMD_PS_DETACH;
  cmd_ptr->param.ps_detach_cmd.type              = type;
  cmd_ptr->param.ps_detach_cmd.trans_id          = trans_id;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the command.
  */
  cmregprx_send_cmd( cmd_ptr,
                     cmregprx_info_ptr
                   );


  return;

}


#ifdef MMODE_ADDITIONAL_DEBUG_INFO
/*===========================================================================

FUNCTION cmregprx_dbg_buf_init

DESCRIPTION
  This function should be called to initialize the Cmregproxy debug buffer
  values at time of power up.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  void                           cmregprx_dbg_buf_init
(
       void
)
{

  int i = 0, j = 0;
  cmregprx_debug_buffer.dbg_buf_idx = 0;

  for(i = 0 ; i < CMREGPRX_MAX_DEBUG_BUFFER_SIZE ; i++ )
  {
    cmregprx_debug_buffer.dbg_buf[i].message_name = CMREGPRX_MESSAGE_NONE;
    cmregprx_debug_buffer.dbg_buf[i].is_cmd = FALSE;
    cmregprx_debug_buffer.dbg_buf[i].cnt = 0;       
    cmregprx_debug_buffer.dbg_buf[i].cmregprx_state = CMREGPRX_STATE_MAX;
    cmregprx_debug_buffer.dbg_buf[i].cmregprx_substate = CMREGPRX_SUBSTATE_MAX;
    
  }

}/*cmregprx_dbg_buf_init()*/

/*===========================================================================

FUNCTION cmregprx_dbg_buf_same_cmd

DESCRIPTION
  This function returns a boolean indicating whether the new command is identical to the most recently
  received command in the CMREGPRX debug buffer.

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.
===========================================================================*/
boolean                          cmregprx_dbg_buf_same_cmd
(

    cmregprx_cmd_msg_s_type  *cmd_ptr,
    /* Pointer to the new command */
    cmregprx_dbg_buf_message_s_type *prev_msg
    /* Pointer to the debug buffer message most recently received */
)
{
  int i;

  /* Pointer to CMREGPRX info */
  cmregprx_info_s_type       *cmregprx_info_ptr = cmregprx_get_info_ptr();

  if(cmd_ptr == NULL || prev_msg->message_name != cmd_ptr->name || prev_msg->is_cmd == FALSE)
  {
    return FALSE;
  }
  
  for(i=0; i<(int)CMREGPRX_NUM_OF_GW_STACKS; i++)
  {
    if(prev_msg->cmregprx_state != cmregprx_info_ptr->stack_info.state
      || prev_msg->cmregprx_substate != cmregprx_info_ptr->stack_info.substate)
    {
      return FALSE;
    }
  }
  
  return TRUE;
}

/*===========================================================================

FUNCTION cmregprx_dbg_buf_same_rpt

DESCRIPTION
  This function returns a boolean indicating whether the new report is identical to the most recently
  received report in the CMREGPRX debug buffer.

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.
===========================================================================*/
boolean                          cmregprx_dbg_buf_same_rpt
(

    cm_rpt_type              *rpt_ptr,
    /* Pointer to the new report */
    cmregprx_dbg_buf_message_s_type *prev_msg
    /* Pointer to the debug buffer message most recently received */
)
{
  int i;

  /* Pointer to CMREGPRX info */
  cmregprx_info_s_type       *cmregprx_info_ptr = cmregprx_get_info_ptr();
  cmregprx_stack_info_s_type *current_stack_info_ptr = NULL;
  cm_name_type cmd = CM_RPT_NONE;

  if(rpt_ptr != NULL)
  {
    cmd = rpt_ptr->hdr.cmd;
  }

  current_stack_info_ptr = &(cmregprx_info_ptr->stack_info);

  if(rpt_ptr == NULL || prev_msg->message_name != rpt_ptr->hdr.cmd
    || prev_msg->is_cmd == TRUE)
  {
    return FALSE;
  }
  
  for(i=0; i<(int)CMREGPRX_NUM_OF_GW_STACKS; i++)
  {
    if(prev_msg->cmregprx_state != cmregprx_info_ptr->stack_info.state
      || prev_msg->cmregprx_substate != cmregprx_info_ptr->stack_info.substate)
    {
      return FALSE;
    }
  }
  
  return TRUE;
}


/*===========================================================================

FUNCTION cmregprx_dbg_buf_add_message

DESCRIPTION
  This function adds the command or report being processed to the debug buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                          cmregprx_dbg_buf_add_message
(

    cmregprx_cmd_msg_s_type  *cmd_ptr,

    cm_rpt_type              *rpt_ptr

)
{

  int i;

  /* Pointer to the RegProxy's info.
  */
  cmregprx_info_s_type       *cmregprx_info_ptr = cmregprx_get_info_ptr();
  cmregprx_stack_info_s_type *current_stack_info_ptr = NULL;
  uint8 curr_idx = cmregprx_debug_buffer.dbg_buf_idx;
  uint8 prev_idx = (curr_idx == 0) ? CMREGPRX_MAX_DEBUG_BUFFER_SIZE-1 : curr_idx-1;
  
  /* Pointer to entry at current index of cmregprx debug buffer
  */
  cmregprx_dbg_buf_message_s_type *debug_buffer_ptr = NULL;
  cmregprx_dbg_buf_message_s_type *prev_msg = NULL;

  /*REGPRX anyway doesnt handle generic cmds, in any state other
  ** than active. So it is no-op. Need not maintain in buffer
  */
  if(cmd_ptr != NULL && cmd_ptr->name == PROT_CMD_GENERIC
    && cmregprx_get_stack_info_ptr(SD_SS_MAIN)->state != CMREGPRX_STATE_ACTIVE)
  {
    CMREGPRX_MSG_HIGH_3("debug_buffer: Skipping command %d, Prot state %d, ss: %d",
      cmd_ptr->name, cmregprx_get_stack_info_ptr(SD_SS_MAIN)->state, SD_SS_MAIN );
    return;
  }
  
  /* Increment debug_buffer_index and set it to 0 in case it becomes same as
  ** CMREGPRX_MAX_DEBUG_BUFFER_SIZE as messages are added in cyclic manner
  */
  if(curr_idx >= CMREGPRX_MAX_DEBUG_BUFFER_SIZE)
  {
    curr_idx = cmregprx_debug_buffer.dbg_buf_idx = 0;
  }

  debug_buffer_ptr = (cmregprx_debug_buffer.dbg_buf)+curr_idx;
  prev_msg = (cmregprx_debug_buffer.dbg_buf)+prev_idx;
  
  /* Add command to cmregprx debug buffer
  */
  if(cmd_ptr != NULL)
  {
    /* Check to see if this command was most recently received by the debug buffer */
    if(cmregprx_dbg_buf_same_cmd(cmd_ptr,prev_msg) && cmregprx_debug_buffer.dbg_buf[prev_idx].cnt < USHRT_MAX)
    {
      cmregprx_debug_buffer.dbg_buf[prev_idx].cnt++;
      return;
    }
    else
    {
      debug_buffer_ptr->message_name = cmd_ptr->name;
      debug_buffer_ptr->is_cmd = TRUE;
      CMREGPRX_MSG_LOW_2("Cmregprx_debug_buffer: Adding command %d to index %d",
                        cmd_ptr->name, curr_idx);
    }
  }

  /* Add report to cmregprx debug buffer
  */
  else if(rpt_ptr != NULL)
  {
    /* Check to see if this report was most recently received by the debug buffer */
    if(cmregprx_dbg_buf_same_rpt(rpt_ptr,prev_msg) && cmregprx_debug_buffer.dbg_buf[prev_idx].cnt < USHRT_MAX)
    {
      cmregprx_debug_buffer.dbg_buf[prev_idx].cnt++;
      return;
    }
    else
    {
      debug_buffer_ptr->message_name = rpt_ptr->hdr.cmd;
      debug_buffer_ptr->is_cmd = FALSE;
      current_stack_info_ptr = &(cmregprx_info_ptr->stack_info);
    }
  }

  /* Storing state and substate of each stack to current debug buffer entry
  */
  for( i=0 ; i < (int)CMREGPRX_NUM_OF_GW_STACKS ; i++)
  {
    debug_buffer_ptr->cmregprx_state = cmregprx_info_ptr->stack_info.state;
    debug_buffer_ptr->cmregprx_substate = cmregprx_info_ptr->stack_info.substate;
    CMREGPRX_MSG_LOW_3("Cmregprx_debug_buffer: Stack %d -> State:%d, substate:%d ",
                      i,
                      cmregprx_info_ptr->stack_info.state,
                      cmregprx_info_ptr->stack_info.substate );
  }

  (cmregprx_debug_buffer.dbg_buf_idx)++;

}/*cmregprx_dbg_buf_add_message()*/



/*===========================================================================

FUNCTION cmregprx_dbg_buf_print_info

DESCRIPTION
  This function should be invoked by the MMOC when sanity timer expired to
  print value of cmregprx_debug_buffer

DEPENDENCIES

RETURN VALUE


SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  void                        cmregprx_dbg_buf_print_info(
void
)
{

#ifdef MMODE_ADDITIONAL_DEBUG_INFO_EXT

  int i = 0, j = 0;

  
  CMREGPRX_MSG_HIGH_1("CMREGPRX DEBUG BUFFER: Current_index : %d", cmregprx_debug_buffer.dbg_buf_idx);

  for(i = 0 ; i < CMREGPRX_MAX_DEBUG_BUFFER_SIZE ; i++ )
  {
    if(cmregprx_debug_buffer.dbg_buf[i].message_name != CMREGPRX_MESSAGE_NONE)
    {
      if(cmregprx_debug_buffer.dbg_buf[i].is_cmd == TRUE)
      {
        CMREGPRX_MSG_HIGH_3("CMREGPRX DEBUG BUFFER: Command[%d]->%d | ss = %d",
                           i,
                           cmregprx_debug_buffer.dbg_buf[i].message_name,
                           cmregprx_debug_buffer.dbg_buf[i].ss);
      }
      else
      {
        CMREGPRX_MSG_HIGH_3("CMREGPRX DEBUG BUFFER: Report[%d]->%d | ss = %d",
                           i,
                           cmregprx_debug_buffer.dbg_buf[i].message_name,
                           cmregprx_debug_buffer.dbg_buf[i].ss);
      }
      for(j=0; j< CMREGPRX_NUM_OF_GW_STACKS;j++)
      {
        CMREGPRX_MSG_HIGH_3("CMREGPRX DEBUG BUFFER:   PROT Stack[%d]:: State:%d  Substate:%d",
                              j,
                              cmregprx_debug_buffer.dbg_buf[i].cmregprx_state[j],
                              cmregprx_debug_buffer.dbg_buf[i].cmregprx_substate[j]);
      }
    }
  }

#endif /* MMODE_ADDITIONAL_DEBUG_INFO_EXT */

}/*cmregprx_dbg_buf_print_info()*/

#endif /* MMODE_ADDITIONAL_DEBUG_INFO */


/*===========================================================================

FUNCTION cmregprx_cmd_proc

DESCRIPTION
  This function should be called when the REX signal corresponding to
  RegProxy's command queue is set. This function will be responsible for
  dequeuing and getting the command processed.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  void                           cmregprx_cmd_proc
(
        void

)
{
  /* Pointer to the RegProxy's info.
  */
  cmregprx_info_s_type  *cmregprx_info_ptr = cmregprx_get_info_ptr();

  /* Pointer to the command buffer.
  */
  cmregprx_cmd_msg_s_type *cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Process all the commands present in the queue.
  */
  while ( (cmd_ptr = q_get( &cmregprx_info_ptr->cmd_q )) != NULL )
  {
    CMREGPRX_MSG_HIGH_1("Recvd command: %d", cmd_ptr->name);


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Invoke the event specific handler.
    */
    switch ( cmd_ptr->name )
    {
      case PROT_CMD_ACTIVATE:
        cmregprx_activate_prot_hdlr( cmd_ptr );
        break;

      case PROT_CMD_DEACTIVATE:
        cmregprx_deactivate_prot_hdlr( cmd_ptr );
        break;

      case PROT_CMD_GENERIC:
        cmregprx_generic_prot_cmd_hdlr( cmd_ptr );
        break;

      case PROT_CMD_PH_STAT_CHGD:
        cmregprx_ph_stat_chgd_hdlr( cmd_ptr );
        break;

      #ifdef FEATURE_LTE
      case PROT_CMD_PS_DETACH:
        cmregprx_ps_detach_hdlr(cmd_ptr);
        break;
      #endif

      case PROT_CMD_MAX:
      default:
        CMREGPRX_MSG_HIGH_1("Unknown command received %d", cmd_ptr->name);
        break;

    } /* switch ( cmd_ptr->name ) */


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    #ifdef MMODE_ADDITIONAL_DEBUG_INFO
    /* Add command to cmregprx_debug_buffer
    */
    cmregprx_dbg_buf_add_message(cmd_ptr , NULL);
    #endif
    /* Return command buffer to free_q.
    */

    cm_mem_free (cmd_ptr);
    cmd_ptr = NULL;

  } /* while ( ) */

  return;

} /* cmregprx_cmd_proc() */



/*===========================================================================
=============================================================================
=============================================================================
============================== REG -> REGPROXY reports ======================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_rpt_proc

DESCRIPTION
  This function should be called to process the reports from REG like
  cm_service_cnf, cm_service_ind , cm_network_list_cnf etc. This function
  would be responsible for processing the REG report by calling the
  appropriate state handlers.


DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  TRUE  - Report was processed.
  FALSE - Report was not processed.

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  boolean                        cmregprx_rpt_proc
(
  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */
)
{
  /* Pointer to LL report message.
  */
  cm_rpt_type           *cm_rpt_ptr        = NULL;

  /* Pointer to the RegProxy's info.
  */
  cmregprx_info_s_type  *cmregprx_info_ptr = cmregprx_get_info_ptr();

  /* Flag to indicate if the report was processed.
  */
  boolean is_rpt_processed                 = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL );
  CM_ASSERT( cmregprx_get_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_rpt_ptr = (cm_rpt_type *) rpt_ptr;

  /* Invoke the appropriate event report handler.
  */
  switch ( cm_rpt_ptr->hdr.cmd )
  {
    case CM_SERVICE_CNF:
      cmregprx_srv_cnf_hdlr( cm_rpt_ptr,
                             cmregprx_info_ptr
                           );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_SERVICE_IND:
      cmregprx_srv_ind_hdlr( cm_rpt_ptr,
                             cmregprx_info_ptr
                           );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_NETWORK_LIST_CNF:
      cmregprx_net_list_cnf_hdlr( cm_rpt_ptr,
                                  cmregprx_info_ptr
                                );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_SIM_AVAILABLE_CNF:
    case CM_SIM_NOT_AVAILABLE_CNF:
      cmregprx_sim_avail_cnf_hdlr( cm_rpt_ptr,
                                   cmregprx_info_ptr
                                 );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_STOP_MODE_CNF:
      cmregprx_stop_cnf_hdlr( cm_rpt_ptr,
                              cmregprx_info_ptr
                            );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_PH_STATUS_CHGD_CNF:
      cmregprx_ph_status_chgd_cnf_hdlr( cm_rpt_ptr,
                                        cmregprx_info_ptr
                                      );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CAMPED_IND:
      cmregprx_camped_ind_hdlr( cm_rpt_ptr,
                                cmregprx_info_ptr
                              );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   #ifndef FEATURE_MMODE_LTE_ONLY
   case CM_HPLMN_SEARCH_CNF:

      #if defined(FEATURE_GW_ON_DEMAND_HPLMN_SRCH)
      cmregprx_hplmn_srch_cnf_hdlr ( cm_rpt_ptr,
                                     cmregprx_info_ptr
                                   );
      #endif
      break;
    #endif
    
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   case CM_MM_PS_DATA_SUSPEND_IND:

      cmregprx_data_suspended_ind_hdlr (cm_rpt_ptr,
                                     cmregprx_info_ptr
                                   );

      break;

   case CM_SIM_STATE_UPDATE_IND:
      cmregprx_sim_state_update_ind_hdlr (cm_rpt_ptr,
                                     cmregprx_info_ptr
                                   );
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   #if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)
   case CM_SUITABLE_SEARCH_END_IND:
     cmregprx_suitable_search_end_hdlr ( cm_rpt_ptr, cmregprx_info_ptr );

    break;
   #endif /* (FEATURE_LTE) || (FEATURE_WRLF_SYSTEM_SEL) */
   #if defined ( FEATURE_WRLF_SYSTEM_SEL) && defined(FEATURE_WCDMA)
   case CM_DISABLE_BPLMN_IND:

     cmregprx_disable_bplmn_ind_hdlr ( cm_rpt_ptr,cmregprx_info_ptr );

    break;
   #endif /* FEATURE_WRLF_SYSTEM_SEL */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_PS_DETACH_CNF:
      cmregprx_ps_detach_cnf_hdlr(cm_rpt_ptr,
                                  cmregprx_info_ptr
                                  );
      break;

   
    case CM_NAS_T3346_TIMER_IND:
      CMREGPRX_MSG_HIGH_3("T3346 timer indication: t3346_notification = %d, active_rat = %d, as_id = %d",
                          cm_rpt_ptr->cmd.t3346_timer_ind.t3346_notification,
                          cm_rpt_ptr->cmd.t3346_timer_ind.active_rat,
                          cm_rpt_ptr->cmd.t3346_timer_ind.as_id);
	  
      /* Send T3346 timer info to clients */
      cmregprx_send_t3346_timer_ind(
                          cm_rpt_ptr->cmd.t3346_timer_ind.t3346_notification,
                          cm_rpt_ptr->cmd.t3346_timer_ind.active_rat,
                          cm_rpt_ptr->cmd.t3346_timer_ind.as_id);

      break;


    #ifdef FEATURE_MMODE_GW
    case CM_PLMN_LIST_CHANGE_IND:
    {
      CMREGPRX_MSG_HIGH_2("RXD: CM_PLMN_LIST_CHANGE_IND, list_type=%d, as_id=%d",
                              cm_rpt_ptr->cmd.plmn_list_chng_ind.changed_list_type,
                              cm_rpt_ptr->cmd.plmn_list_chng_ind.as_id);
      cmregprx_plmn_list_change_hdlr(cm_rpt_ptr);
      break;
    }
    #endif    

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    default:
      is_rpt_processed = FALSE;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  } /* switch ( ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  #ifdef MMODE_ADDITIONAL_DEBUG_INFO
  /* Add the report to cmregprx_debug_buffer
  */
  if(is_rpt_processed)
  {
    cmregprx_dbg_buf_add_message(NULL, cm_rpt_ptr);
  }
  #endif
  /* Done with report processing.
  */
  return is_rpt_processed;

} /* cmregprx_rpt_proc() */


/*===========================================================================
=============================================================================
=============================================================================
============================== Timer handler ================================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_timer_proc

DESCRIPTION
  This function should be called to process the timer tick.


DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
   void

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  void                        cmregprx_timer_proc
(
        void

)
{
  /* Pointer to the RegProxy's info.
  */
  cmregprx_stack_info_s_type* stack_info_ptr;

  /* Uptime.
  */
  dword uptime = time_get_uptime_secs();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check for stack 1
  */
  stack_info_ptr = cmregprx_get_stack_info_ptr(SD_SS_MAIN);

  /* Check if reference period expired.
  */
  if ( (stack_info_ptr->start_thrtl_ref_uptime + CMREGPRX_SRV_REQ_CNT_PERIOD )
     <= uptime
   )
  {

    /* Check if still in right state and there is buffered service_req.
    */
    if ( stack_info_ptr->state == CMREGPRX_STATE_ACTIVE &&
         stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_SRV_CNF &&
         stack_info_ptr->cnt_cm_srv_req > CMREGPRX_MAX_SRV_CNF_REQ
       )
    {
      /* Processing buffered CM_SERVICE_REQ.
      */
      CMREGPRX_MSG_HIGH_2("Processing buffered SD action uptime %d %d",
                         uptime, stack_info_ptr->cnt_cm_srv_req);
      stack_info_ptr->cnt_cm_srv_req = 0;
      cmregprx_send_service_req_permission( stack_info_ptr );

    }

    /* Reset counter and restart ref. period.
    */
    stack_info_ptr->start_thrtl_ref_uptime = uptime;
    stack_info_ptr->cnt_cm_srv_req = 0;

  }

  /* No need to do further process if not svlte/sglte/multi-sim */
  if (!cmph_is_feature_mask(FEATURE_MODE_MASK_MSTACK))
  {
    return;
  }

  /* If second stack is operational then check for stack 2 */

  /* Check for stack 2
  */
  stack_info_ptr = cmregprx_get_stack_info_ptr(SD_SS_HYBR_2);

  /* Check if reference period expired.
  */
  if ( (stack_info_ptr->start_thrtl_ref_uptime + CMREGPRX_SRV_REQ_CNT_PERIOD )
     <= uptime
   )
  {

    /* Check if still in right state and there is buffered service_req.
    */
    if ( stack_info_ptr->state == CMREGPRX_STATE_ACTIVE &&
         stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_SRV_CNF &&
         stack_info_ptr->cnt_cm_srv_req > CMREGPRX_MAX_SRV_CNF_REQ
       )
    {
      /* Processing buffered CM_SERVICE_REQ.
      */
      CMREGPRX_MSG_HIGH_2("Processing buffered SD action uptime %d %d",
                         uptime, stack_info_ptr->cnt_cm_srv_req);
      stack_info_ptr->cnt_cm_srv_req = 0;
      cmregprx_send_service_req_permission( stack_info_ptr );

    }

    /* Reset counter and restart ref. period.
    */
    stack_info_ptr->start_thrtl_ref_uptime = uptime;
    stack_info_ptr->cnt_cm_srv_req = 0;

  }


  #ifdef FEATURE_MMODE_HYBR3_ENABLED
  /* If third stack is operational then check for stack 3 */

  /* Check for stack 3 */
  stack_info_ptr = cmregprx_get_stack_info_ptr(SD_SS_HYBR_3);

  /* Check if reference period expired.
  */
  if ( (stack_info_ptr->start_thrtl_ref_uptime + CMREGPRX_SRV_REQ_CNT_PERIOD )
     <= uptime
   )
  {

    /* Check if still in right state and there is buffered service_req.
    */
    if ( stack_info_ptr->state == CMREGPRX_STATE_ACTIVE &&
         stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_SRV_CNF &&
         stack_info_ptr->cnt_cm_srv_req > CMREGPRX_MAX_SRV_CNF_REQ
       )
    {
      /* Processing buffered CM_SERVICE_REQ.
      */
      CMREGPRX_MSG_HIGH_2("Processing buffered SD action uptime %d %d",
                         uptime, stack_info_ptr->cnt_cm_srv_req);
      stack_info_ptr->cnt_cm_srv_req = 0;
      cmregprx_send_reg_service_req( stack_info_ptr );

    }

    /* Reset counter and restart ref. period.
    */
    stack_info_ptr->start_thrtl_ref_uptime = uptime;
    stack_info_ptr->cnt_cm_srv_req = 0;

  }
  #endif // defined FEATURE_MMODE_HYBR3_ENABLED


} /* cmregprx_timer_proc() */




/*===========================================================================
=============================================================================
=============================================================================
============================== RegProxy functional interface ================
=============================================================================
=============================================================================
===========================================================================*/

EXTERN  void         cmregprx_q_init(void)
{

   /* Pointer to RegProxy's information.
    */
  cmregprx_info_s_type   *cmregprx_info_ptr = cmregprx_get_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize command queues.
   */
  (void) q_init( &cmregprx_info_ptr->cmd_q );
}

EXTERN void cmregprx_sig_init
(
  rex_tcb_type				   *tcb_ptr,
  /* Pointer to the task control block for the task in which
  ** RegProxy runs.
  */

  rex_sigs_type					  cmd_q_sig
  /* Signal to be set when a command is posted in the RegProxy's
  ** command queue.
  */
)
{

   /* Pointer to RegProxy's information.
    */
  cmregprx_info_s_type   *cmregprx_info_ptr = cmregprx_get_info_ptr();

  /* Initialize the Signal associated with the cmd q.
   */
  cmregprx_info_ptr->cmd_q_sig               = cmd_q_sig;
  cmregprx_info_ptr->rex_tcb_ptr             = tcb_ptr;

}


/*===========================================================================

FUNCTION cmregprx_init

DESCRIPTION
   This function should be called to initialize the each RegProxy stack .

   Note! This should be called before any of the APIs of Regproxy are
   invoked.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void  cmregprx_init_stack
(
        uint8         stack_id          /* stack id */
)
{
  cmregprx_info_s_type   *cmregprx_info_ptr = cmregprx_get_info_ptr();
  cmregprx_stack_info_s_type* cmregprx_stack_info_ptr;
  cmregprx_stack_info_ptr  = &(cmregprx_info_ptr->stack_info);

    /* Initialize UE mode */
  cmregprx_stack_info_ptr->is_gwl_subs_avail        = FALSE;
  cmregprx_stack_info_ptr->is_resel_allowed        = TRUE;
  cmregprx_stack_info_ptr->mmoc_trans_id           = PROT_TRANS_NONE;
  cmregprx_stack_info_ptr->reg_trans_id            = 0;
  cmregprx_stack_info_ptr->state                   = CMREGPRX_STATE_INACTIVE;
  cmregprx_stack_info_ptr->substate                = CMREGPRX_SUBSTATE_NULL;
  cmregprx_stack_info_ptr->start_thrtl_ref_uptime = time_get_uptime_secs();
  cmregprx_stack_info_ptr->cnt_cm_srv_req          = 0;

  cmregprx_stack_info_ptr->camped_ind_buf          = NULL;
  cmregprx_stack_info_ptr->last_srv_req.message_header.message_id =
                                               (byte)CM_REG_CMD_NONE;
  cmregprx_stack_info_ptr->list_search_info.pos = CMREGPRX_POS_DEFAULT;
  cmregprx_stack_info_ptr->list_search_info.meas_id = CMREGPRX_MEAS_ID_DEFAULT;
  cmregprx_stack_info_ptr->stop_mode_reason = SYS_STOP_MODE_REASON_NONE;

  cmregprx_stack_info_ptr->last_srv_req.parameters.automatic.type = REG_CM_SERVICE_REQ_NORMAL;
  cmregprx_stack_info_ptr->last_srv_req.parameters.manual.type    = REG_CM_SERVICE_REQ_NORMAL;
  cmregprx_stack_info_ptr->last_srv_req.sms_only = FALSE;

  cmregprx_stack_info_ptr->is_rlf_reset_delayed = FALSE;
  
  cmregprx_stack_info_ptr->sys_sel_pref_req_id = CM_DEFAULT_VALUE;


  #ifdef FEATURE_CM_LTE
  cmregprx_stack_info_ptr->lte_connected_mode = FALSE;
  #endif
  
}
/*===========================================================================

FUNCTION cmregprx_init

DESCRIPTION
   This function should be called to initialize the RegProxy module.

   Note! This should be called before any of the APIs of Regproxy are
   invoked.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  void                           cmregprx_init
(
  void
)
{
  /* Pointer to RegProxy's information.
  */
  cmregprx_info_s_type   *cmregprx_info_ptr = cmregprx_get_info_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For TG==T_PC, stop on error fatal
  */
  #ifdef CM_DEBUG
  #error code not present
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the Regproxy's state info
  */

  cmregprx_info_ptr->subs_chgd_trans_id = PROT_TRANS_NONE;
  cmregprx_info_ptr->unique_trans_id = 0;


  {
    cmregprx_info_s_type   *cmregprx_info_ptr = cmregprx_get_info_ptr();
    cmregprx_stack_info_s_type* cmregprx_stack_info_ptr;
    cmregprx_stack_info_ptr  = &(cmregprx_info_ptr->stack_info);
  
      /* Initialize UE mode */
    cmregprx_stack_info_ptr->is_gwl_subs_avail        = FALSE;
    cmregprx_stack_info_ptr->is_resel_allowed        = TRUE;
    cmregprx_stack_info_ptr->mmoc_trans_id           = PROT_TRANS_NONE;
    cmregprx_stack_info_ptr->reg_trans_id            = 0;
    cmregprx_stack_info_ptr->state                   = CMREGPRX_STATE_INACTIVE;
    cmregprx_stack_info_ptr->substate                = CMREGPRX_SUBSTATE_NULL;
    cmregprx_stack_info_ptr->start_thrtl_ref_uptime = time_get_uptime_secs();
    cmregprx_stack_info_ptr->cnt_cm_srv_req          = 0;
  
    cmregprx_stack_info_ptr->camped_ind_buf          = NULL;
    cmregprx_stack_info_ptr->last_srv_req.message_header.message_id =
                                                 (byte)CM_REG_CMD_NONE;
    cmregprx_stack_info_ptr->list_search_info.pos = CMREGPRX_POS_DEFAULT;
    cmregprx_stack_info_ptr->list_search_info.meas_id = CMREGPRX_MEAS_ID_DEFAULT;
    cmregprx_stack_info_ptr->stop_mode_reason = SYS_STOP_MODE_REASON_NONE;
  
    cmregprx_stack_info_ptr->last_srv_req.parameters.automatic.type = REG_CM_SERVICE_REQ_NORMAL;
    cmregprx_stack_info_ptr->last_srv_req.parameters.manual.type    = REG_CM_SERVICE_REQ_NORMAL;
    cmregprx_stack_info_ptr->last_srv_req.sms_only = FALSE;
  
    cmregprx_stack_info_ptr->is_rlf_reset_delayed = FALSE;
    
    cmregprx_stack_info_ptr->sys_sel_pref_req_id = CM_DEFAULT_VALUE;

  #ifdef FEATURE_CM_LTE
    cmregprx_stack_info_ptr->lte_connected_mode = FALSE;
  #endif
    
  }


  #ifdef MMODE_ADDITIONAL_DEBUG_INFO
  /* Initialize the cmregprx_debug_buffer field values.
  */
  cmregprx_dbg_buf_init();
  #endif
  return;

} /* cmregprx_init() */


/*===========================================================================

FUNCTION cmregprx_send_hplmn_srch_req

DESCRIPTION
  Function is called to inform REG of user initiated HPLMN search request.
  As a result of this request REG initiates a PLMN search. IF HPLMN is found
  in the return result of this search REG attemps to register on the HPLMN.

  CAUTION: Traditional approach to talk to REG is CM->MMOC->CM->REG.
  For HPLMN srch requirement (CM->REG) CM directly talks to REG. Race
  can occur when user selects Automatic mode and then the HPLMN Key. HPLMN key
  will reach REG first but would be killed when Service request reaches REG
  later. Need to design using approach involving MMOC.

  REG can handle this request only when in idle state, for all other states
  it rejects this request. No confirmation is sent from CM to UI.

  Note! This function should only be called from the same REX task context as
  RegProxy's.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  void               cmregprx_send_hplmn_srch_req (
  sd_ss_e_type       ss
)
{

  #if defined (FEATURE_GW_ON_DEMAND_HPLMN_SRCH)

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Allocate a REG command buffer
  */
  reg_cmd_type *reg_ptr =  cm_reg_get_buf_else_err_fatal(CM_HPLMN_SEARCH_REQ);

  /* Pointer to RegProxy's information.
  */
  cmregprx_stack_info_s_type   *cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_MSG_HIGH_2("state = %d, substate = %d", cmregprx_stack_info_ptr->state,
                     cmregprx_stack_info_ptr->substate);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If CMREGPRX is not in active state reject the hplmn srch request
  ** To take care of conditions when stack is not in operation
  */
  if (cmregprx_stack_info_ptr->state != CMREGPRX_STATE_ACTIVE)
  {
    CMREGPRX_MSG_HIGH_0 (" not in active state, rej HPLMN SRCH req");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print debug stuff.
  */
  CMREGPRX_MSG_HIGH_0("Send HPLMN_SEARCH_REQ" );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command to REG.
  */
  cm_reg_send_cmd( reg_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #else
  SYS_ARG_NOT_USED(ss);
  #endif /* FEATURE_GW_ON_DEMAND_HPLMN_SRCH */

  return;

} /* cmregprx_send_hplmn_srch_req*/

boolean cm_is_plmn_rat_allowed (
        sys_plmn_id_s_type                 plmn_id,
         /* PLMN id
         */
        sys_radio_access_tech_e_type       rat
         /* Mode of the PLMN under query
         */
)
{
  return TRUE;
}

/*===========================================================================

@FUNCTION cm_per_subs_is_plmn_rat_allowed

@DESCRIPTION
  Check if the plmn is allowed for registration.

@DEPENDENCIES

None

@RETURNS
  TRUE if allowed.
  FALSE otherwise.

===========================================================================*/
extern boolean  cm_per_subs_is_plmn_rat_allowed (
        sys_plmn_id_s_type                 plmn_id,
         /* PLMN id
         */
        sys_radio_access_tech_e_type       rat,
         /* Mode of the PLMN under query
         */
        sys_modem_as_id_e_type         as_id,
        /* subscription id
        */
        sys_modem_stack_id_e_type         stack_id
)
{
  sd_ss_e_type   ss  = SD_SS_MAIN;

  //return sd_is_plmn_rat_allowed(plmn_id, cmregprx_map_sys_rat_to_sd_mode(rat), ss);
  return FALSE;
}

/*===========================================================================

@FUNCTION cm_per_subs_is_reg_allowed

@DESCRIPTION
  Check if the plmn is allowed for registration.  For now, only as_id_1 is valid input
  parameter.

@DEPENDENCIES

None

@RETURNS
  TRUE if allowed.
  FALSE otherwise.

@SIDE_EFFECT

None

===========================================================================*/
extern boolean  cm_per_subs_is_reg_allowed (
        sys_plmn_id_s_type                 plmn_id,
         /* PLMN id
         */
        sys_radio_access_tech_e_type       rat,
         /* Mode of the PLMN under query
         */
        sys_modem_as_id_e_type         as_id,
        /* subscription id
        */
        sys_modem_stack_id_e_type         stack_id
)
{
  boolean            plmn_is_undefined;
  boolean            mnc_includes_pcs_digit;
  sys_mcc_type       mcc;
  sys_mnc_type       mnc;

  /* For multi-sim, if other than as_id_1 is provided, return true.
  ** For ssim, ignore as_id.
  */
  if ( (cm_get_operator_info(as_id) != OPERATOR_CT) || (cmph_is_msim() && (as_id != SYS_MODEM_AS_ID_1))||
	    (rat == SYS_RAT_LTE_RADIO_ACCESS))
  {
    CM_MSG_HIGH_2("is_reg_allowed: msim as_id = %d,rat:%d", as_id,rat);
    return TRUE;
  }

  /* Decode MCC and MNC from PLMN */

  sys_plmn_get_mcc_mnc(plmn_id, &plmn_is_undefined, &mnc_includes_pcs_digit, &mcc, &mnc);

  CM_MSG_HIGH_2("mcc=%d, mnc=%d", mcc, mnc);

  if ( mcc == 460 || mcc == 455 )
  {
    CM_MSG_HIGH_0("is_reg_allowed:Avoid Registration-nonpreferred Network");
    return FALSE;
  }
  else
  {
    CM_MSG_HIGH_0("is_reg_allowed:Proceed Registration-preferred Network");
    return TRUE;
  }
}

#if defined(FEATURE_MMODE_GW) && !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
/*===========================================================================

FUNCTION cmregprx_send_abort_req

DESCRIPTION
  This function sends the ABORT requests to REG.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN void                            cmregprx_send_abort_cc_req
(
       sys_stop_mode_reason_e_type     reason,
          /* Reason for deactivating REG.
          */

       sys_modem_as_id_e_type          as_id
)
{
  /* Pointer to mn command buffer */
  mn_cnm_cmd_type *mn_ptr = NULL;

  /* For MN-CM interface, as_id indicates subscription id not stack id */
  if (cmph_is_ssim())
    as_id = SYS_MODEM_AS_ID_1;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CMREGPRX_MSG_HIGH_1("Send ABORT_CC_REQ and ABORT_SS_REQ reason=%d",reason);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Send abort requests to CC and SS.
  */
  mn_ptr = cm_mn_get_buf_else_err_fatal(CM_ABORT_CC_REQ);

  mn_ptr->cmd.cm_abort_cc_req.cause = (byte)reason;

  mn_ptr->cmd.cm_abort_cc_req.as_id = as_id;

  cm_mn_send_cmd( mn_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if defined (FEATURE_MMODE_SS_SUPS) || defined(FEATURE_MMODE_USS_SUPS)
  mn_ptr = cm_mn_get_buf_else_err_fatal(CM_ABORT_SS_REQ);

  mn_ptr->cmd.cm_abort_ss_req.cause = (byte)reason;

  mn_ptr->cmd.cm_abort_ss_req.as_id = as_id;

  cm_mn_send_cmd( mn_ptr );
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
}
#endif 

#if (defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE)

/*===========================================================================

FUNCTION cmregprx_send_data_roaming_ind

DESCRIPTION
  This function sends the data roaming indication to REG

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmregprx_send_data_roaming_ind(

      sys_data_roaming_e_type  roam_type,
      /* Data roaming setting to be sent to NAS */

      sys_modem_as_id_e_type  as_id
)
{
  /* Allocate a REG command buffer
  */
  reg_cmd_type *reg_ptr =  cm_reg_get_buf_else_err_fatal(CM_DATA_ROAMING_IND);

  reg_ptr->cmd.cm_data_roaming_ind.data_roaming = roam_type;
  reg_ptr->cmd.cm_data_roaming_ind.as_id = as_id;

  /* Send data roaming ind to Reg */
  CMREGPRX_MSG_HIGH_2("send data_roaming_ind, roam_type=%d, as_id=%d",
                          roam_type,as_id);
  cm_reg_send_cmd( reg_ptr );
}

#endif

/*===========================================================================

FUNCTION cmregprx_send_irat_complete_rpt

DESCRIPTION
  Sends CM ip app registration status

DEPENDENCIES
  cmipapp_init

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmregprx_send_irat_complete_rpt( void )
{
  cm_sd_rpt_u_type      *rpt_ptr = NULL;

  /* Using sd report queue cmregprx notified CM that this service indication
  ** concludes the iRAT process. This allows CM to start notifying clients
  ** with new service status.
  */
  rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();

  /* Fill in command type */
  rpt_ptr->hdr.cmd                            = CM_IRAT_COMPLETE;

  /* Put message in destination task queue */
  cm_sd_rpt (rpt_ptr);
} /* cmipapp_send_reg_status () */

/*===========================================================================

FUNCTION cmregprx_send_t3346_timer_ind

DESCRIPTION
  This function processes T3346 timer info from LL and sends the  
  timer info to clients via the message router interface.


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmregprx_send_t3346_timer_ind
(
  sys_nas_t3346_status_e_type         t3346_notification,
  /* the status of timer T3346 */

  sys_radio_access_tech_e_type        active_rat,
  /* the RAT for which timer T3346 status is sent */

  sys_modem_as_id_e_type              subs_id
  /* active subscription */
)
{
  sys_sys_mode_e_type  sys_mode;

  /* validate input parameters */
  if(t3346_notification <= SYS_NAS_T3346_STATUS_UNKNOWN || 
     t3346_notification >= SYS_NAS_T3346_STATUS_MAX)
  {
    CMREGPRX_ERR_1("t3346_notification not in valid range: %d", 
                   t3346_notification);
    return;
  }
  if(active_rat <= SYS_RAT_NONE || active_rat >=  SYS_RAT_MAX)
  {
    CMREGPRX_ERR_1("active_rat not in valid range: %d", 
                   active_rat);
    return;
  }
  if(subs_id <= SYS_MODEM_AS_ID_NONE || subs_id >= SYS_MODEM_AS_ID_MAX)
  {
    CMREGPRX_ERR_1("subs_id not in valid range: %d", 
                   subs_id);
    return;
  }

  switch(active_rat)
  {
    #ifdef FEATURE_MMODE_GW
    case SYS_RAT_GSM_RADIO_ACCESS:
      sys_mode = SYS_SYS_MODE_GSM;
      break;

    #ifndef FEATURE_MMODE_GPRS_LTE
    case SYS_RAT_UMTS_RADIO_ACCESS:
      sys_mode = SYS_SYS_MODE_WCDMA;
      break;

    case SYS_RAT_TDS_RADIO_ACCESS:
      sys_mode = SYS_SYS_MODE_TDS;
      break;
    #endif
    #endif
    
    case SYS_RAT_LTE_RADIO_ACCESS:
      sys_mode = SYS_SYS_MODE_LTE;
      break;
	  
    #if defined(FEATURE_CIOT)
    case SYS_RAT_LTE_NB1_RADIO_ACCESS:
      sys_mode = SYS_SYS_MODE_LTE_NB1;
    break;

    case SYS_RAT_LTE_M1_RADIO_ACCESS:
      sys_mode = SYS_SYS_MODE_LTE_M1;
    break;
    #endif

    default:
      sys_mode = SYS_SYS_MODE_NONE;
      CMREGPRX_ERR_1("Wrong active rat:%d in T3346 timer ind", active_rat);
      break;
  }

  if(sys_mode != SYS_SYS_MODE_NONE)
  {
    errno_enum_type      msgr_error;
    cm_t3346_info_s_type *timer_info;
    timer_info = (cm_t3346_info_s_type *)cm_mem_malloc(
                                       sizeof(cm_t3346_info_s_type));

    if(timer_info == NULL) 
    {
      CMREGPRX_ERR_FATAL_0("Error: cm_mem_malloc() returned NULL");
      return;
    }

    /* copy indication values to timer_info */
    timer_info->timer_status = t3346_notification;
    timer_info->active_rat = sys_mode;
    timer_info->as_id = subs_id;
    timer_info->msg_hdr.inst_id= SYS_AS_ID_TO_INST_ID(cmph_get_sub_with_lte_cap());
  
    /*
    ** Inform the clients about the T3346 timer information
    */
    msgr_error = cm_msgr_send( MM_CM_T3346_INFO_IND, 
                               MSGR_MM_CM,
                               &(timer_info->msg_hdr), 
                               sizeof(cm_t3346_info_s_type) );

    cm_mem_free(timer_info);

    CMREGPRX_MSG_HIGH_4("CM->MSGR: T3346_INFO_IND sent: msgr_error %d, status %d, rat %d, subs_id %d",
						msgr_error,
                        t3346_notification,
                        sys_mode,
                        subs_id);
  }

} /* cmregprx_send_t3346_timer_ind */

/*===========================================================================

FUNCTION cmregprx_print_state_variables

DESCRIPTION
  This function should be invoked by the MMOC when sanity timer expired

DEPENDENCIES

RETURN VALUE


SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  void                        cmregprx_print_state_variables(
void
)
{

  /* Pointer to RegProxy info.
  */
  cmregprx_info_s_type* cmregprx_info_ptr = cmregprx_get_info_ptr();
  uint8                           i;

  mm_log_mm_reg_variables ();

  for(i=0; i< CMREGPRX_NUM_OF_GW_STACKS;i++)
  {
    CMREGPRX_MSG_HIGH_4("state:%d, substate:%d, reg_trans_id:%d, mmoc_trans_id:%d", 
						cmregprx_info_ptr->stack_info.state,
                        cmregprx_info_ptr->stack_info.substate,
                        cmregprx_info_ptr->stack_info.reg_trans_id,
                        cmregprx_info_ptr->stack_info.mmoc_trans_id);
  }
} /* cmregprx_dbg_stop_on_err_fatal() */


/*===========================================================================

FUNCTION cmregprx_is_acq_db_in_progress

DESCRIPTION
  This function checks if Acq DB Scan is in progress during RLF

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
boolean cmregprx_is_acq_db_in_progress
(
   sd_ss_e_type          ss
)
{
  cmregprx_stack_info_s_type*      cmregprx_stack_info_ptr = NULL;

  if( ss == SD_SS_HYBR_1 )
  {
    CMREGPRX_ERR_1( "Invalid ss %d", ss);
    return FALSE;
  }

  cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(ss);

  if ( cmregprx_stack_info_ptr == NULL )
  {
    CMREGPRX_ERR_1( "Invalid State Info ptr %d", ss);
    return FALSE;
  }

  CMREGPRX_MSG_HIGH_4( "is_acq_db ss %d, scanscope:%d stack state:%d, stack substate:%d", 
  						ss,
                        cmregprx_stack_info_ptr->last_srv_req.scan_scope,
                        cmregprx_stack_info_ptr->state,
                        cmregprx_stack_info_ptr->substate);

  if ( (cmregprx_stack_info_ptr->state == CMREGPRX_STATE_ACTIVE) &&
       (cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_SRV_CNF) &&
       (cmregprx_stack_info_ptr->last_srv_req.scan_scope == SYS_SCAN_SCOPE_ACQ_DB)
     )
  {
    return TRUE;
  }

  return FALSE;
} /* cmregprx_is_acq_db_in_progress() */


#ifdef CM_DEBUG
#error code not present
#endif /*   #ifdef CM_DEBUG  */

/*===========================================================================

FUNCTION cmregprx_is_wait_srv_cnf

DESCRIPTION
  Tells if cmregprx is waiting for service confirmation.

DEPENDENCIES
  None.

RETURN VALUE
  



SIDE EFFECTS
  None.
===========================================================================*/
EXTERN boolean  cmregprx_is_wait_srv_cnf(void)
{
  sd_ss_e_type   ss  = SD_SS_MAIN;
  boolean is_wait_srv_cnf = FALSE;
  cmregprx_stack_info_s_type*           stack_info_ptr;
  
  

  stack_info_ptr = cmregprx_get_stack_info_ptr(ss);

  if(stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_SRV_CNF)
  {
    is_wait_srv_cnf = TRUE;
  }

  CMREGPRX_MSG_HIGH_2("is_wait_srv_cnf: %d, wait for srv cnf on ss %d",is_wait_srv_cnf,ss);
  return is_wait_srv_cnf;
}


#else /* FEATURE_GSM || FEATURE_WCDMA || FEATURE_CM_LTE */

/*===========================================================================
FUNCTION cmregprx_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
word cmregprx_dummy( word dummy_input )
/*lint -esym(714,cmregprx_dummy) */
/*lint -esym(765,cmregprx_dummy)
** Can't be static as it would result in compiler warnings
*/
{
  return dummy_input;
}

#endif /* FEATURE_GSM || FEATURE_WCDMA || FEATURE_CM_LTE */
