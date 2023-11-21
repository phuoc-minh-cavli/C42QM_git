/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   C A L L   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager Call Object for WCDMA/GSM.

  The Call Object is responsible for:
  1. Processing clients' call commands.
  2. Processing Lower Layer replies for clients' call commands.
  3. Processing Lower Layer call related notifications.
  4. Notifying the client list of call events.

EXTERNALIZED FUNCTIONS

  cmcall_init
    Initializing the call object.

  cmwcall_info_get
    Copy the current call state information into a specified buffer.

  cmcall_client_cmd_proc
    Process clients' call commands.

  cmcall_timer_proc
    Process call timer events


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmcall_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2001 - 2015 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/cm/src/cmcall.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/09/14   xs      Reset ssac timer upon cell change
05/05/14   xs      VOLTE call handling during L->D IRAT
04/22/14   xs      hVOLTE SSAC support
04/02/14   sm      End calls not transferred in SRVCC cache
03/13/14   xs      SSAC Rel12 requirement
01/09/14   jvo     Put redial case for AS_REJ_RR_REL_IND under GCF flag
12/19/13   sm      bSRVCC changes to include new ORIG substate
11/14/13   ss      Add transferring of CS DATA calls from HYBR_2 to MAIN during
                   SGLTE Merge
11/13/13   sm      Add API to fetch active call based on midcall NV set
11/13/13   jvo     Add redial case for AS_REJ_RR_REL_IND in function
                   cmcall_process_act_proc_end_reason()
11/13/13   xs      Add control for VOLTE SR LTE failure
10/21/13   jvo     Added F3 message to print backoff time calculation
10/08/13   xs      Fix issue of FDN present in CSIM but not in USIM
07/22/13   fj      Add support for DSDX L+G
05/15/13   fj      Add support for segment loading.
09/12/13   jvo     Fix 2 static analysis errors
09/05/13   xs      Remove LTe from mode pref is CSFB is not supported during silent
                   redial fallback
09/04/13   mh/jvo  Mainline the modem statistics code
08/25/13   sm      VOLTE concurrency during e911 orig
08/23/13   sm      Update mng cnf state to NONE for VOLTE call ended during LPM.
08/20/13   jvo     Added check on bound of as_id in cm_unreserve_connection_id
                   to propagate fix from static analysis warning
08/08/13   xs      Remove emg handling from CMAC
08/08/13   jvo     Changed calls to rand_next() to more secure secapi source
                   and added cmcall_get_random_number_in_range()
07/19/13   jvo     Remove all MDM/MSM Fusion features
07/17/13   xs      Take ext IMS emerg into consider of emergnecy call active
                   checking
07/15/13   ss      FR1860 - T/G on DSDS
07/10/13   ar      checking range of call_context_trans_id[index]
06/25/13   dk      CR470025 stack reduction effort
06/18/13   gm      FR 2617 - OMADM IMS and ADMIN PDN Disable Requirement
06/12/13   sm      Set SS_MAX only during Emergency hybr pref toggle
06/05/13   qf      Added support for IMS rat_change between LTE and WLAN
05/03/13   fj      Reject mode change request if there's PS data call and SMS
                   in progress.
03/27/13   dk      providing a new API cm_mm_call_cmd_end_all_calls() in
                   CR 447701
                   verified in cmdiag, using QXDM command window to end call
                   send_data 54
03/27/13   sm      Fix to change sys mode to TDS from GSM during ss event
03/06/13   xs      Make srv domain PS_ONLY when srv_type is PS_ONLY
02/14/13   th      Change existing function to identify if call obj exists or
                   not.  This is for ext IMS purpose.
01/22/13   xs      Added video share support
01/07/13   qf      FR 2564 - World mode support for DSDA
01/07/12   xs      Added PS supplemetary service support
01/04/13   th      Copy modify_error_code in current call state information
                   into a specified buffer.
01/04/13   th      Handling retry-after failure (Type 5) from IMS
01/02/13   xs      Fix issue of not sending IMS SRVCC complete ind when
                   alerting call
12/11/12   dk      Fix for CR419781 - on handling CS_DATA over LTE, w/o CSFB
12/04/12   aj      e911 domain selection
12/03/12   xs      Fix sRVCC issue for MT volte call
11/02/12   xs      Fix VOLTE call alllow with modification issue
11/02/12   vm      Old cc api use new centralized call control
10/24/12   fj      Added a new call end_status.
10/18/12   xs      Don't transfer call not in active state after SRVCC
10/18/12   ak/xs   Fix not sending MNG_CNF issue
10/18/12   xs      Add silent redial support for PS only call
10/10/12   qf      Force mode pref to SD without holding for HDR data call
                   while in optimized lte->ehrpd handoff
09/26/12   skk     Changes of ESM SM backoff timer requirements
09/21/12   vk      FIX to unreserve connection id in CSFB mode.
09/18/12   qf      Reset SSAC backoff timer if G/W full service is acquired
09/05/12   fj      If there's OTASP call ongoing, end the originated data call
09/07/12   xs      Update VOLTE restricted call checking condition
08/28/12   ak      Increase the SR hold period for HDR sess_neg failure.
08/08/12   ak      Update call type from IS-707B to PS_DATA before queuing
                   the call command.
07/24/12   ak      Dont enforce incompatible call preference for PS_DATA call.
07/19/12   jh      Fix memleak on callend before CC finish.
07/06/12   qf      FR1730 - service specif access control
05/19/12   vk      Allocate MN connection ID for a call transfer from
                   3GPP2 to 3GPP.
05/17/12   xs      Don't end LTE call when card error/remove
05/14/12   cl      Allow emergency call with originated Voice Type before
                   PRL_INIT
05/11/12   ab      Fix for wrong throttling information is returned when
                   hybrid is off and HDR is on the main stack
04/24/12   sg      Add required changes to support TDS in Automatic mode
04/15/12   ab      FR1681: Location Information support in Call Control
                   envelope command
04/10/12   sk      Fixing the CW and KW warnings
04/04/12   sg      Add support for new mode pref TDS_LTE
04/04/12   xs      Codec info propagation
11/01/11   fj      Fixed KW critical error.
03/23/12   gm      DIME integration fix: TMC deprecation and others.
03/08/12   xs      Added codec info support for VOLTE
11/30/11   jh      Added feature for dynamic rat acq order change
02/13/12   xs      VT upgrade/downgrade support
02/12/12   aj      update DCTM algo to use hybrid_active_band for HDR system
02/07/12   xs      VOLTE silent redial support
01/24/12   vk      Changes for 1XCSFB RLF handling.
01/20/12   gm      Data synchronization for CM Call objects
01/17/12   aj      SC SVLTE SO 33 suspend changes
01/06/12   gm      Data synchronization changes for freefloating platform
01/05/12   ab      Fixed Compilation Error
01/03/12   vk      Removal of feature flag for CSFB CS DATA calls
01/03/12   xs      Set mode info to IP when VOLTE call is determined
12/20/11   vk      Changes for CSFB CS DATA call feature
12/20/11   sg      Added a check for TDS_GSM_WCDMA / TDS_GSM_WCDMA_LTE
12/06/11   ab      Connection Id Changes
12/05/11   vk      Setting SR max time to 30 secs for PS call to pass GCF test
12/01/11   vk      PS voice domain selection migration from CNE to CM
11/01/11   rn      End data call if new SMS call is originated
10/19/11   xs      Fix issue of accessing orig_plan_tbl exceed array bounday
10/19/11   xs      Shorten BSR to force LTE scan after HDR srv lost
09/09/11   jh      Handling DRB_RELEASE_IND after IRAT to GW
09/09/11   rk      Fix to update ok to orig value only if service status is
                   NO_SERVICE
09/02/11   rk      S102 less 1XCSFB
08/22/11   vk      Input parameter check for CALL_ORIG cmd
08/12/11   xs      VOLTE support for VT
08/05/11   jh      Do not end LTE call upon MMGSDI_CARD_ERROR_EVT
08/05/11   xs      VOLTE phase 1 support
07/26/11   jh      Marking GW connection IDs on LTE Activate bearer request
                   Adding additional logs for connection_ids
07/24/11   aj     SC SVLTE 1.0 merge to mainline
07/19/11   jh      Revert previous change: move 2 macros back to cmcall.c
06/30/11   fj      Move 2 macros to cmcall.h.
06/27/11   fj      For oprt_mode LPM request, don't end LTE calls.
06/15/11   rk      End the GW PS calls when mode is changed to LTE only and
                   PDP activate request is not yet send to NAS.
06/14/11   gm      Thermal emergency fixes - resuming HDR, LPM->Online
06/10/11   mj      Renamed existing & added new function to manage SM
                   pdp_context_trans_id
06/09/11   rk      Fix to orig legacy CDMA/GW call by leaving LTE if CSFB call
                   is not allowed
06/03/11   rk      Reject normal/emerge voice call originated when UE set to
                   LTE only mode and VoLTE is not possible
05/27/11   sg      Allocate connection id for emergency call.
05/18/11   rm      Migrating to Session based GSTK APIs.
05/18/11   rn      WCDMA Channel locking feature
05/10/11   sv      In case of 1XCSFB call silent redial fix to hold the call
                   when SRV is not full service
05/03/11   sv      Add JCDMA specific action table
04/29/11   rm      CMI-4:defeaturise cm_reserve/unreserve_connection_id()
04/24/11   rk      Handle EMM reject cause CS domain not avail for CSFB call
                   and end the call
04/19/11   rk      Changing the declaration to extern for pdp_context_trans_id
04/19/11   sv      Use IDM value only for data calls with DRS bit as 0
04/15/11   rk      Handling EMM srv reject cause to do silent redial
04/11/11   am      Cleaning stubs and runnning qtf SUTE
04/06/11   rk      Fixing 1XCSFB emergency call issue not to enter to
                   emergency callback mode
04/05/11   rk      Correcting CSFB call silent redial behavior
03/30/11   rk      revising handling of failure cases
03/29/11   rk      Fix for not doing PPCSFB after receiving AttachAccept with
                   SMSOnly set
03/24/11   fj      Check whether the call_id still exists when processing
                   RAB_REESTAB_REQ.
03/23/11   skp     conn. id is reserved when emerg. call is on GSM AS_ID_1.
03/22/11   gm      Fix to waiting 4s after call end to force THERM EMERG
03/22/11   rk      Support for handling negative scenarios
03/16/11   anm     sm connection id not be unreserved in LPM and Graceful PDP.
03/11/11   fj      Process RAB_REESTAB_REQ based on the mode DS indicates.
03/11/11   rm      GSDI clean-up
03/07/11   skp     Fixing issue for CDMA emergency call in C+G with C
                   unavailable.
03/02/11   rk      Fixing issues for coming back to LTE after CSFB call end.
02/28/11   gm      Changes for thermal emergency feature
02/25/11   sn      Support for graceful pdp deactivation before detach.
02/23/11   mh      Removed FEATURE_JCDMA_CM
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/11/11   rk      On no service report update ok_to_orig value for calls in
                   silent re-dial.
02/03/11   fj      Add cm_find_call_id_gwl().
01/28/11   xs      Change SR after access failure.
01/28/11   xs      Change SR after access failure.
01/27/11   fj      Add debug message.
01/25/11   fj      Add LTE mode back in cmcall_proc_srv_status().
01/24/11   ak      Merging the Fusion type I code to mainline.
01/13/11   fj      After sending the call object transfer event to DS,
                   change the mode info_type back to GW_PS.
01/12/11   fj      Add new indication for call object transfer during IRAT.
01/04/11   rk      Fix to unforce CSFB call pref immediately after call end
12/28/10   sv      Fix DSDS code
12/23/10   sv      Integrate DSDS feature
12/17/10   gm      Changed user_data parameter to const type in CC callback.
12/16/10   am      Cleaning nas_stubs_ref.h inclusion
12/10/10   fj      Not reserve connection_id if will not make GW call.
12/10/10   rk      Adding new orig mode for CSFB call
12/10/10   rk      Sucessful MO call initial support
11/24/10   sg      CM should add cause value SM_PS_DETACHED, as a
                   SM_INTERNAL_CAUSE, when PS call get terminated due to
                   PS_DETACH.
11/16/10   xs      Added con_ref notification support
11/15/10   rm      Memory optimization changes
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
10/19/10   rk      Fixing issue in handling CSFB call reject scenario.
10/08/10   ak      Reset propagation from MSM to MDM.
10/08/10   sg      Add check to block CS_DATA calls
10/05/10   rk      On MO call orig removing LTE from mode pref only if
                   3GPP CSFB is not possible.
10/01/10   ak      Merged the Fusion code from Dev branch.
10/01/10   xs      Superlint critical/high error fixed
09/28/10   rk      Correcting the featurization for FEATURE_3GPP_CSFB
09/27/10   xs      Fixed target compiler warning
09/22/10   rn      Added Silent redial support for 3GPPCSFB.
09/21/10   sg      Deallocate cmd_ptr, from call_ptr->reply, while
                   deallocating call id.
09/17/10   xs      Fixed compilation issue when define FEATURE_CALL_THROTTLE
                   in CDMA only mode
09/16/10   gm      Fixed function cmcall_get_otasp_call_obj to allow
                   OTAPA/OTASP reports in all types of 1x calls.
09/06/10   rn      Defined FEATURE_3GPP_CSFB
09/02/10   rn      Resolve call type before subsc check in call control
09/01/10   rn      Add support for FEATURE_3GPP_CSFB MO call
08/31/10   sv      Return call err when favored_mode_pref becomes NONE
08/03/10   fj      Featurize LTE to GW call object transfer code.
07/15/10   fj      Enhancement in cmcall_event_2().
07/21/10   sg      CM should initiate silent redial (SR) for emergency call,
                   if failure cause received from NAS set as
                   "TEMPORARY_FAILURE".
07/14/10   mh      Fixed compile warnings
07/12/10   am      Changes are made to handle subaddress in modem cc.
07/06/10   gm      Fix for Klocwork deferred critical errors.
06/29/10   gm      Handle HDR errors for e911 VoIP call.
06/16/10   gm      VoIP SR disabled while a PS is in orig state.
06/15/10   fj      Fix cmcall_event_2() call_ptr check.
06/10/10   ak      Mainlining memory optimization changes.
06/09/10   gm      Fix to Klocwork errors.
05/26/10   jqi     EHRPD VOIP silent redial support.
05/20/10   fj      Fix for GW to LTE call transfer.
05/17/10   fj      Added support for GW to LTE call transfer.
05/07/10   fj      Added cmcall_is_no_call_in_lte_ps().
05/07/10   gm      Handling invalid call-id for call_cmd_end.
05/03/10   rm      Block emergency calls when disable call type NV mask is
                   set to Emergency
04/29/10   sv      Add NAS stubs for QTF
04/29/10   rm      Adding Modem Statistics changes for 1x/HDR
04/16/10   fj      Added support for LTE to GW call object transfer.
02/24/10   aj      Add support for PS optimized silent redial
02/17/10   mp      Added CM_CALL_EVENT_OTASP_STATUS to wakeup apps in SUSPEND
                   state
02/16/10   sg      Added support for eCall feature.
02/01/10   aj      Add support for DCTM 4.0
01/19/10   aj      Added VOIP support for emergency call
01/13/10   sv      KW Fixes
01/05/10   pm      Removing UOOS
12/16/09   fj      Modified cmcall_event_2().
12/08/09   fj      Added lte_band_pref in necessary APIs.
11/13/09   fj      Added handling for CM_SRV_TYPE_LTE &
                   CM_SRV_TYPE_GSM_WCDMA_LTE.
11/12/09   fj      Fixed lint errors for CFG_LTE.
11/05/09   fj      For LTE calls, set connection id src to ESM in cmltecall.c
11/09/09   sg      Add non-demand paging macro to function that are not to be
                   demand-paged.
11/09/09   rn      reset cmd_cc_cb_func
10/14/09   sg      ZI memory reduction. Mmode should call new Heap Manager API
                   to allocate memory dynamically.
08/31/09   fj      Added support for LTE UE initiated call funtionality.
06/17/09   fj      Call cmltecall_reset() in cmcall_reset().
04/17/09   fj      Fixed lint warnings.
04/06/09   fj      Added cmltecall.h for LTE interfaces.
09/23/09   sv      KW fixes
09/01/09   mh      Removed FEATURE_IP_CALL from cmcall_orig_proc
09/01/09   sv      Moving from clk API to time API
08/21/09   rn      Check NULL client ptr before call_info_list_get processing
08/19/09   ks      CM is not allowing three primary pdp call at a time
08/07/09   rm      CMI phase-2: Removing featurisation from /api files
08/07/09   rn/rm   Centralised call control changes
07/31/09   mp      Added call_orig_fail layer debugging aid for epzip update
07/16/09   mh      Updated cmcall_timer_proc() not to access ended call
07/15/09   ks      added default values for ext_max_bit_rate_uplink and
                   ext_gtd_bit_rate_uplink fields of qos structure
06/03/09   rk      Reverting the change for unreserve connection id only if
                   orig req already not sent to NAS.
05/27/09   aj      Adding support for passing otasp status to clients
05/14/09   aj      Code cleanup in cmcall_proc_srv_status
05/11/09   mh      Remove WLAN for SR after CSNA hand-down
04/23/09   aj      Prevent referencing deallocated memory
04/29/09   rk      For PS call Unreserve connection id only if orig req
                   already not sent to NAS.
04/17/09   ks      Removing FEATURE_CM_DELAY_SETUPRES functionality
                   to avoid tear down of sec pdp due to MT voice call.
04/24/09   rm      Including cmlog_v.h
04/17/09   ks      Added support for blocking MO/MT voice call on Data Card
04/06/09   mj      Update BOOLEAN macro w/ namespace prefix to CM_BOOLEAN
03/24/09   sv      updated to cmtaski.h
03/23/09   rm      Adding _v.h files from NAS
03/12/09   rm      Adding srch_v.h
03/09/09   rm      Made cmcall_misc_get_common_mode_pref non-static
03/05/09   rk      Fixing silent redial issue by restoring ok to orig value
                   after scheduling next redial.
02/27/09   rm      Adding header files of other MOBs
02/23/09   rm      Adding /inc file and removing cm_log_v.h
02/13/09   am      CM changes for UIM header decoupling
02/12/09   mh      Added support for no SR after MAP for IS-707B
01/29/09   fj      Remove FEATURE_HDR_HYBRID for call events
12/28/08   rn      FDN checking option in call control
12/10/08   jd      Corrected Lint errors
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
11/13/08   aj      Added API cmcall_end_each_call_with_type
10/31/08   rn      On going offline, send PDP_ABORT to NAS
10/27/08   cl      Added support for KTFT Voip Call Waiting Feature
10/15/08   rn      Corrected max line chars
10/09/08   fj      Fixed klocwork defects
10/01/08   sv      Corrected featurization in cmcall_gsdi_card_status_ind_proc
                   to remove lint errors.
09/10/08   ak      Fixed scenario of data call drop during GSM scan
09/05/08   sv      Memory Reduction for ULC
08/25/08   ak      Fixed the general compiler warnings.
08/20/08   st      Resolve Klockwork defects
08/21/08   rk      Scheduling the SR for E911 call after AS or MM reject.
08/08/08   pk      Not changing the srv_domain pref and gw_acq_order for
                   Silent redials
08/05/08   aj      Added api cmcall_check_for_call_state_on_ss()
06/12/08   sv      Resolve Klocwork defects
06/10/08   aj      For a ps data call in hybrid mode with uoos, force
                   preferences on both main and hybrid stack
06/06/08   rn      Added cmcall_overall_ps_call_end()
06/06/08   sv      Removing unneccesary snd.h
05/28/08   ks      Updating the associated primary profile number when
                   secondary PDP is promoted to Primary
05/22/08   vk/sv   added functionality to handle roam only preference
05/12/08   sg      AMR WB vocoder support for GERAN
05/08/08   sv      Featurizing snd.h under CDMA
04/29/08   ks      Reverting the changes of Updating the associated primary
                   profile number when secondary PDP is promoted to Primary
03/31/08   ks      Updating the associated primary profile number
                   when secondary PDP is promoted to Primary
03/13/08   sk      Fix to allow CDMA scans during dormant GW PS data calls
                   without tearing down the PS data call.
03/05/08   vk      corrected code for HYBR_GW configuration
03/05/08   vs      Handle CM_CALL_MODE_INFO_NONE for HDR call in
                   CM_ACQ_FAIL_F report
02/21/08   rm      Added code for not tearing down secondary PDP when
                   a VoIP call comes
12/13/07   sk/ak   Added DCM.
12/12/07   rm      For - Adding RAT-balancing feature support
                   Correcting the error caused by ens_alpha_string.
12/12/07   th      Addition of CM_CALL_EVENT_END to wake up events in
                   cmcall_dem_is_apps_wakeup_from_suspend_event()
12/07/07   ra      Added support for IMS VCC in HOMER files
11/26/07   sv      Removing Feature FEATURE_GW_CNAP_CODING_SCHEME
                   and add initialization to enc_alpha.coding_scheme
11/19/07   rn      Added support for UE initiated dormancy
11/05/07   sg/vs   Renaming FEATURE_CM_TEMP_NAS_DEP_TBD To
                   FEATURE_GW_CNAP_CODING_SCHEME
10/31/07   rm      Modifying- CM attempts silent-redial when call is still
                   in progress in NAS.
10/01/07   rm      Handling of GSDI_CARD_ERR_POLL_ERROR in CM
09/10/07   rn      Updated field enc_alpha of cmcall_s_type to send
                   coding scheme
08/07/07   ic      Lint clean up
06/22/07   sk      DDTM rewrite and added support for ATOM.
06/14/07   pk      Added support for power suspend
05/11/07   rp/cl   Deallocating cmd_ptr if call_ptr is already deallocated
                   in call control complete call back function.
04/17/07   rp      Making cmcall_get_favored_srv_domain_pref() public
04/13/07   rp      Filtering non-voice calls in cmcall_incom_proc
04/13/06   rp      Returning correct cmd_err from cmcall_process_orig_mode
03/09/07   sk      RAM usage reduction.
02/02/07   pk      Featurizing cmcall_get_voip_call_id under FEATURE_IP_CALL
02/01/07   pk      Removed extra cmcall_client_cmd_err from
                   cmcall_send_mode_spec_end
01/22/07   rp      Support for copying result from cc to call_info_ptr
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
12/19/06   pk      Modified forcing of preferences for PS_DATA calls
11/17/06   jqi     Added support for HDR active protocol revision information.
11/17/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/16/06   pk      Lint Fixes
11/11/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/09/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
10/12/06   ka/rp   Rejecting call originations when Phone is waiting for
                   LPM_OK
10/03/06   ka      Adding support for IP call on WCDMA
09/14/06   ka      Adding support for call control on secondary ps calls
08/28/06   ka      Adding pdp header comp and data comp enums.
08/25/08   ka      Adding support for delaying setup response to let QTV
                   PAUSE streaming session.
08/22/06   ic      Unused variables compiler warning fixes
08/16/06   ic      Lint fixes
08/11/06   pk      Added support for UOOS
08/11/06   ka      Modifying cmcall_check_if_gw_cs_call_active() to check
                   for calls in any state.
08/08/06   sk      Added redial action CMCALL_ACT_FORCE_NON_IP.
                   Made sure mode_usage computed in cmcall_client_orig_proc()
                   is stored in the call object (field main_ss_mode).
                   Added cmcall_remove_mode_and_force()
07/31/06   ka      Adding Explicit call transfer and Multiparty support for
                   VoIP. Forcing force_top to TRUE for VoIP calls.
07/19/06   ic      Lint fixes
07/18/06   ka      Increasing silent redial time for AS_REJ_LOW_LEVEL_FAIL.
                   Fixing the issue where call cmd cb for call end api was
                   being called more than once.
07/07/06   pk      Removed cmcall_add_cmd_to_hold_q
07/06/06   pk      Added support for FEATURE_CM_TRIBAND_ORIG_PLAN_TBL
07/03/06   jqi     Only send the data avail when there is a PS call in orig
                   state and the data is suspend.
07/06/06   ka/ic   Featurization fixes for PS call control
07/04/06   ka      Changes to support call control on PS calls.
07/03/06   ic      Lint fixes in cmcall_gstk_evt_cb
06/27/06   ka      Adding feature flag FEATURE_IP_CALL
06/15/06   sk      Fixed memory leak that occurred when number classification
                   is out of memory and returns FALSE.
06/05/06   ka/ic   Adding cmcall_is_gw_cs_call_type(). Increasing silent
                   redial for AS_REJ_LOW_LEVEL_FAIL under GCF NV flag.
06/02/06   pk      For CMCALL_ACT_WAIT_OTHER_END setting the orig sent on to
                   NONE
05/24/06   pk      Changed FEATURE_VOICE_CALL_LOW_POWER to FEATURE_DCVS_LPM
05/10/06   ka      mode_spec_end used to bunch calls of particular info_type
                   and then send them to wcall, ipcall or xcall. Modifying
                   wcall, ipcall and xcall to end calls of their typeonly.
05/08/06   pk      Modified cmcall_is_there_a_cdma_call_in_conv, added check
                   for sys_mode
05/03/06   pk      Lint Fixes
05/02/06   pk      Modified the featurization for clk regime function calls
04/20/06   pk      Added clk_regime_register_for_cpu_resource and
                   clk_regime_deregister_for_cpu_resource
04/19/06   pk      Added meta comments for 7200
04/14/06   pk      Added support for CMCALL_ACT_WAIT_OK_TO_ORIG_SHORT
04/12/06   pk      Initialized the cmcall_mode_info.info_type for the GW
                   calls in cmcall_process_orig_mode to the correct type
04/03/06   pk      If HDR starts throttling during a Silent Redial, give it
                   one more chance
                   Once the system is throttled, don't send call end
                   to clients immediately, buffer the call end for 1 second
04/03/06   jqi     Mainline FEATURE_SILENT_REDIAL_20
04/01/06   ka      Changes for emerg serv category during orig, aborting
                   manual search and returning current band with srv ind.
03/27/06   pk      Modified cmcall_get_favored_srv_domain_pref to return
                   CS_ONLY or PS_ONLY depending on the call type
03/24/06   ka      Added VideoTelephony fallback to voice feature for UMTS.
03/14/06   pk      Added support for high PSIST
03/13/06   sk      End Hold orig period when we are in a non-usable HDR
                   system and we are faking no service to clients.
03/02/06   pk      Modified redial actions for KDDI, based on the latest
                   requirements
03/01/06   pk      Corrected cmcall_hybr_redial for the case when SD was not
                   forced because a more preferred acquisition was going and
                   CM gets a acq_fail. The priority queue get fails and CM
                   forces the preferences by calling
                   cmcall_force_pref_on_the_fly
02/17/06   ka      Adding support for videoshare
01/30/06   sk      Added debug info to help catch CM cmd buffer memory leaks.
01/12/06   ka      Removing error fatal when all call ids get used up.
                   Handling call end request with incorrect info_type.
01/12/06   ic      Trimmed to minimum the list of lint errors that were
                   suppressed for va_list, va_start(), va_arg(), va_end()
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/09/06   ic      Adjusted for cm_num_init() / cm_num_copy() changes in cm.h
01/06/06   ka      Increasing silent redial timer for AS_REJ_LOW_LEVEL_FAIL
                   based on GCF NV setting.
01/06/06   ic      Lint cleanup
12/13/05   ic      To fix compiler warnings, put cmcall_check_if_orig_thr*()
                   functions under FEATURE_CALL_THROTTLE
12/11/05   ic      Lint cleanup
12/09/05   ic      Lint cleanup
11/23/05   ka      Modification to allow hold/active for ip calls.
11/17/05   pk      Added checks for 1X data calls while SMS is waiting for
                   confirmation
11/08/05   sk      Separated CM reports and commands.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
10/27/05   pk      Added support for CM_CALL_EVENT_CALL_ORIG_THR
10/23/05   jqi     Hold ps data call orig or data avail request if there is a
                   pending get network list request.
10/21/05   sk      Removed stripping of the "+" from the start of a number
                   and the modification of the digit_mode.
10/20/05   pk      Cleaned RVCT compiler warnings and added mapping functions
10/20/05   pk      Added support for CM_MODE_PREF_INTERSECT_OR_FORCE
10/20/05   pk      Added support for CM_CALL_END_ACCESS_BLOCK_ALL
10/20/05   sk      Added hook in cmcall_event() to estimate base station IDM.
10/18/05   pk      Added check for cmpd_call_start, before forcing the mode
                   for the call
10/13/05   ic      Replaced INTLOCK / INTFREE with rex_enter_crit_sect() /
                   rex_leave_crit_sect()
10/02/05   ic      Corrected handling of end_params in call object.
09/14/05   pk      Modified cmcall_hybr_redial, if call origination
                   parameters are not found on the Queue, the call is ended
09/12/05   pk      Added check for GPS session clean up in call originations
                   GPS session related changes
09/08/05   ka      Support for VoIP over HDR on the Main instance.
08/26/05   pk      Modified cmcall_hybr_redial(), when switching to 1X,
                   the call_end reason in not immediately reset to NONE.
08/25/05   ic      NULL ptr check fix in cmcall_call_control_complete_cb()
08/23/05   sk      Removing support for data calls while in emergency
                   callback mode or in emergency call.
08/19/05   pk      Modified the handling of ACQ_FAIL end reason in
                   cmcall_orig_proc. The hold_orig time is reset only if we
                   have 1X service and the next_orig_time is not reset
08/16/05   pk      Added support for priority Queues for origination
                   parameters
08/16/05   ka      Adding support for voice call on HDR (using VoIP app) on
                   the hybrid instance. Support for VoIP on HDR in the Main
                   instance needs to be done.
08/12/05   ka      Functions to check voice/emerg in origination and decide
                   on call types.
08/11/05   ka      Added silent redial for AS_REJ_LOW_LEVEL_IMMED_RETRY
08/02/05   ic      Print value that caused error in default switch case
08/02/05   pk      Modified redial action table for HDR call failures.
                   For all connection deny call failures the redial action
                   is CMCALL_ACT_REDIAL_SHORT.
07/30/05   ka      Reverting modifications in cmcall_orig_start done for CCBS
07/29/05   dk      Added support to not let HDR call send pref_sys_Chgd while
                   sms activity is in progress.
07/27/05   ic      Removed from origination tables duplicate listings for
                   CM_CALL_END_NO_RESPONSE_FROM_BS and
                   CM_CALL_END_INCOMPATIBLE
07/21/05   pk      Modified the printing of Silent redial count and the
                   loging of the redial event
07/21/05   sk      Removed internal use of is_jcdma_emergency flag.
                   Changed mode_pref_for_orig_table so that hybrid is not
                   turned off during an emergency call for JCDMA targets.
07/12/05   ic      Print call type that caused error in switch default in
                   cmcall_is_call_type_valid()
07/11/05   pk      Modified call to cmss_check_if_orig_thr_true(), added
                   packet_zone id as additional parameter
07/08/05   pk      Modified the silent redial behavoir for HDR
                   Added helper functions for the redial enhancements
07/08/05   ic      Report highest layer call origination failure as part of
                   call events.
                   Removed dead code
06/28/05   pk      Modified cmcall_check_if_orig_thr_and_add_entry().
                   In HDR only mode, throttle table entry is not made for
                   DRS = 0 call failures
06/27/05   ic      Print call_id that cmcall_ptr() failed to look up
06/27/05   ic      Removed misleading comment from cm_call_id_allocate()
06/26/05   ic      Fixed Lint and compiler warnings
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
06/21/05   dk      Added cmcall_is_active_on_mode() to find if there is any
                   call on a given mode.
06/13/06   ic      Fixed Lint warnings - removed external declaration of
                   cm_unreserve_connection_id()
06/10/05   ic      Lint clean up
06/09/05   ic      Removed all varieties of EJECT comment
                   Cleaned up cmcall_cmd_copy_fields()
06/08/06   pk      Modified cmcall_check_if_orig_thr(). Not throttling for
                   DRS=0 even for DO systems.
06/06/05   pk      Added call throttling support. Modified cmcall_orig_proc.
06/03/05   ic      Mainlined FEATURE_SILENT_REDIAL_20
                   Removed FEATURE_FAVOR_REV_LINK_ROAM_IND
05/18/05   ka      Merging CIPHER_IND feature and GCF fix.
05/17/05   sk      Allow data calls while in emergency callback mode when
                   SVD is on.
05/04/05   jqi     Fixed for hybr data switch over.
04/18/05   dk      Changed Silent Redial action from REDIAL_SHORT to
                   REDIAL_SHORT_SAME_SR for HOLD_DBM_IN_PROG
04/12/05   ic      Removed FEATURE_HDR_HICPS20 featurization from processing
                   of CM_ACQ_FAIL_F
04/06/05   ka      Adding ASSERT's and header files
03/31/05   ic      Fixed Lint warning (negative indentation)
03/24/05   ic      Fixed processing of CM_CALL_CMD_INFO_GET (CR#59955)
                   Rewrote cmcall_client_cmd_proc to use switch()
03/14/05   ka      Handling CM_CALL_MODE_INFO_IP
02/24/05   dk      Added cmcall_count_on_ss() function to address CR 57350.
02/24/05   ic      Removed cmcall_overall_call_state_get()
02/23/05   pk      Changed assertion CM_ERR to CM_ERR in function
                   cmcall_client_orig_proc() to reproduce the CR# 53864
                   This test case verifies the integrity of the call ids i.e
                   verifies the allocation and deallocation routines for
                   call ids.
02/22/05   ka      Changing is_user_data_present flag value in cmcall_event()
02/15/05   pk      Initialized the call event info buffer
                   Initialized the cmcall_mode_info.info_type for the GW calls
                   to the correct type
02/11/05   jqi     Check if Hybird is allowed in loading the orig plan for
                   none hybird mode.
02/09/05   ic      Lint cleanup
02/03/05   dk      Allow PS_DATA call while SMS call is in progress in
                   SVD.
02/01/05   dk      Added WLAN Support.
01/24/05   ic      Replaced cmph_is_ph_in_emerg_cb_mode() with
                   cmph_is_in_emergency_cb()
01/10/05   sj      Adding call failure handling for no hdr service & no hdr
                   rf lock.
12/20/04   ic      Fixed RVCT 2.1 compiler warning 'variable "client_id" was
                   set but never used'.
                   Fixed lint warning related to CM_DEBUG being off on target
12/19/04   ic      Added cmcall_assert() and cmcall_info_get()
                   In cmcall_reset() initialized fields favored_prl_pref and
                   favored_roam_pref fields
12/14/04   sj      Moved "determine_ss.." after xcall & wcall check fns.
12/13/04   sj      Always start with call_id allocation of 0.
12/02/04   dk      Copy call substate in cmcall_get_call_info_list_proc().
11/23/04   ic      Put user-to-user signaling code under FEATURE_UUS
11/20/04   ic      Fixed typo found at lint suppression verification (list to
                   lint)
11/20/04   ic      Fixed compiler warning in
                   cmcall_call_cmd_answer_para_check(), removed unnecessary
                   Lint comment, added assertion
11/19/04   ka      Adding support for user to user signaling
11/18/04   ka      Adding support for MT PDP feature
11/15/04   ic      Lint cleanup with CM_DEBUG on
11/11/04   ic      Formatting fixes only
11/09/04   dk      Increased Silent Redial Next Orig Uptime to 5 seconds for
                   LOW_LEVEL_FAIL error code for GW Voice Call Failure. This
                   is required to address issue in CR 50990.
11/04/04   sj      Fixed issue in force_pref_on_the_fly() preferences
                   computation during 4 secs unforce period.
10/21/04   ic      Fixed code that caused e774 Lint complaints
10/18/04   ka      Adding call origination plan for PS calls with
                   CMCALL_PS_DATA_SR_MAX_TIME redial time.
10/19/04   jqi     Sent the data available request upon the ps call orig
                   when the data is suspended.
10/11/04   ka      For concurrent calls using system selection corresp to
                   the call to check for orig_mode.
10/11/04   ka      Added gw_voice_init, gw_ps_init and cdma_init functions.
10/10/04   sj      Fixed issue with ending the hold orig upon no srv.
09/17/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.04)
09/14/04   ic      Call direction clean up
09/08/04   jqi     More ps dormant optimization.
09/07/04   ka      Setting orig_sent_on_mode to None when call gets rejected
                   by lower layers and as a consequence CM decides to redial.
                   Replacing is_orig_sent and mode_in_use with
                   orig_sent_on_mode
09/03/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.03)
08/30/04   sj      Added orig_mode in call object and subsequent changes for
                   it. Call determine_ss after CC is over.
08/27/04   jqi     Added ps dormant optimization.
08/26/04   dk      Fixed CR 47179. Added Redial for CC Cause
                   CNM_MM_REL_PENDING and removed redial for TEMPORARY FAILURE
08/20/04   sj      Added support for PRL Preference in sys_sel_pref().
08/19/04   dk      Added check for SMS calls while making 1X or HDR data calls
                   in cmcall_orig_proc().
08/18/04   ka      Call direction now is common to system modes.
                   Setting call direction using a function.
08/17/04   ic      Undoing P4 change list #121638
08/11/04   ka      Updating call system mode for incoming origination.
08/09/04   ic      Lint clean up for SNAR configuration
08/09/04   ws      Delay fatal error when running out of call/connection ids.
08/05/04   dk      Kick the GSDI at the call start for CDMA. Remove the check
                   for UIM presence during call.
08/04/04   dk      Added support to hold Packet Data Call origination in HDR
                   if SMS/DBM is in progress.
08/02/04   ic      CM Lint clean up - cmcall.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/28/04   ka      Added support for secondary PDP context
07/30/04   sj      Added support for origination on camped_ind.
07/27/04   ws      Call Control support for SUPS.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
                   Added (void) in front of CM_DEC() macro to get rid of
                   ADS 1.2 compiler warning
                   Fixed ADS 1.2 compiler warning 'unsigned comparison with 0
                   is always true' by not using INRANGE() macro (casting its
                   parameters to int32 does the trick on RVCT 2.1 compiler but
                   not on ADS 1.2)
07/22/04   ic      Rewrote cmxcall_cdma_orig_possible() as generic
                   cmcall_misc_is_mode_pref().
                   Fixed cmcall_set_favored_mode_pref_for_origination() so
                   that DRS = 0 originations use correctly current IDM
                   (CR#45216)
07/21/04   ic/sj   Moved the call to call_ptr->ss =
                   cmcall_determine_ss_for_origination(...) from
                   cmxcall_client_orig_proc() in cmxcall.c to
                   cmcall_client_orig_proc() in cmcall.c so that
                   call_ptr->ss field gets correctly populated for GW calls.
07/21/04   sj      Added support for umts to 1x handover.
07/20/04   ka      Added function to clean up call id and connection id if
                   call origination cmd fails during one of its steps.
                   Using TEMPORARY_FAILURE instead of
                   RESOURCES_UNAVAILABLE_UNSPECIFIED for redial.
07/20/04   ic      Fixed RVCT 2.1 compiler warning
07/15/04   sj      Specified pref_reason as orig_start_cs or orig_start_ps
                   depending on type of call.
07/15/04   sj      Added logic to end call if we get ACQ_FAIL_F while in
                   limited service.
07/14/04   ws      Fixed GSTK dependencies.
07/12/04   ic      Added support for FEATURE_ALS
07/07/04   dk      Added wait for PS Signaling Down indication in
                   cmcall_orig_proc() for CR 43512.
07/06/04   dk      Modified signature of cm_find_connection_id().
07/06/04   ws      Delay 1x origination for hdr call end.
07/05/04   dk      Changed signature of
                   cmcall_update_gsdi_and_log_call_state().
07/05/04   dk      Changed implementation of Call Object Iterator to refer
                   to the next in the Iterator Type.
07/01/04   dk      Added Call Object Iterator.
07/01/04   sj      Fixed issue with orig_failure no_session.
07/01/04   ws      Remove duplicate calls to client cmd callbacks.
06/30/04   ic      Fixed obvious RVCT 2.1 compiler warnings (Jaguar build)
06/30/04   dk      Beautify function header description of call_id_in_range
                   function.
06/30/04   dk      Lint Cleanup.
06/29/04   sj/ic   In cmcall_orig_proc() test call_ptr->mode_in_use instead of
                   sr_ptr->is_orig_sent to check whether origination was sent
                   (CR#45468)
06/25/04   dk      Modified call object so that cm_call_obj_array just
                   contains the pointer to the call object. Moved all call
                   relevent information inside the call object.
                   Removed explicit call object allocation/de-allocation.
                   Now it is done as part of call_id allocate/deallocate.
                   Modified in general to support this new behavior.
                   Moved certain generic call obj array walk through functions
                   from cmxcall.c
06/22/04   ic      Comments clean up
06/21/04   ws      Mainlined FEATURE_GPSONE_DBM
06/16/04   ic      Restored cmcall_event() to not modify call object in any
                   way, moved all the modifications needed to outside of the
                   function.
                   Added cmcall_reset() to reset common call fields.
                   Cleaned up cmxcall_reset() to call cmcall_reset() and reset
                   only 1X specific fields.
                   Added calls to cmcall_update_call_end_info() before
                   cmcall_event() is called for CM_CALL_EVENT_END event.
                   Fixed obvious lint errors (-e715 - variable not referenced;
                   bad enum type used)
06/15/04   dk      Removed cmcall_update_overall_state() and added
                   cmcall_update_gsdi_and_log_call_state(). Modified
                   cmcall_overall_call_state to get rid of FEATURE
                   dependency.
06/10/04   dk      Added cmcall_check_each_call_in_call_state function.
06/09/04   dk      Initialized info_type in call object for MODE_ANY in
                   cmcall_client_orig_proc function. Also initialized
                   info_type in cmcall_callobject_allocate.
06/09/04   dk      Replaced mem_malloc with cm_mem_malloc to support CM Heap.
06/09/04   ka      Corrected call_end_time check
06/04/04   ka      Fixed Lint errors.
06/03/04   ic      Fixed featurization error in cmcall_gstk_evt_cb()
06/02/04   ka      Added function to check if a call of particular type is in
                   a particular state.
06/01/04   ws      Handle '+' dialing for 1x.
05/27/04   ic      Fixed featurization errors so it compiles without
                   FEATURE_HDR defined.
                   In cmcall_client_cmd_proc() call cmcall_client_cmd_err()
                   irrespective of whether error occured or not (client is
                   supposed to be always notified about command processing
                   status)
05/26/04   ka      Added shorter redial for CM_CALL_END_NO_GW_SRV and added
                   unreserving connection ids when silent redial fails
05/19/04   ka      System mode updated in calls for ORIG and IDLE states.
05/17/04   ic      Cleaned up cmcall_callobject_allocate() and
                   cmcall_client_orig_proc()
05/12/04   ka      Added check for client supplied call type when answering
                   MT call.
05/10/04   ka      Fixed Lint errors.
05/06/04   ic      Removed sr_ptr->is_orig_sent = TRUE from cmcall_orig_proc()
                   as it needs to be set in cmwcall_send_orig() /
                   cmxcall_send_orig() respectively.
                   Added debug info for favored mode for origination
                   computation / forcing of origination mode.
                   Used cmcall_remove_mode_pref_components() to remove SD mode
                   preference components in cmcall_client_orig_proc()
04/30/04   ws      Merge FDN Restriction fixes from Leoaprd.
04/30/04   ws      num type and num plan modifications
04/29/04   ic      Fixes in cmcall_set_favored_mode_pref_for_origination() to
                   allow for origination of PS data calls in WCDMA, GSM and
                   WCDMA/GSM mode only.
                   Added support for CM_MODE_PREF_CURRENT_LESS_AMPS
                   Added assertions in cmcall_remove_mode_pref_components()
04/29/04   ic      Fixes in cmcall_set_favored_mode_pref_for_origination() to
                   allow for origination of PS data calls in WCDMA, GSM and
                   WCDMA/GSM mode only.
                   Added support for CM_MODE_PREF_CURRENT_LESS_AMPS
                   Added assertions in cmcall_remove_mode_pref_components()
                   In cmcall_gsdi_card_status_ind_proc() removed comments that
                   were not formatted correctly and caused stripsrc tool to
                   break.
04/28/04   ka      Fixed Lint errors.
04/21/04   ic      In cmcall_orig_start() fixed change that speeds up
                   origination of the second call
04/08/04   ws      Merged from MSMSHARED_CM.01.00.32.00.03
04/06/04   jqi     Fixed a typo.
04/13/04   ka      Fixing delay in origination of second concurrent call.
                   Setting favored_mode_pref to EMERGENCY for emergency calls.
04/03/04   jqi     Populate favored service domain for both orig mode none
                   and normal.
04/02/04   ic      Merged over to CM mainline the delta between
                   MSMSHARED_CM.01.00.10.00.23 and MSMSHARED_CM.01.00.10.00.28
04/01/04   ic      Changed calls to
                   cmdbg_print_fav_mode_pref_for_origination() as per function
                   definition changes
04/01/04   ws      Fixed favored_mode_pref for GW.
04/01/04   ws      Added cmcall_ptr_callobject_allocate function.
03/31/04   ic      Mainlined FEATURE_REX_DYNA_MEM in CM and unit test.
                   Removed all code related to static allocation of call
                   objects.
03/30/04   jqi     Populated favored srv domain on every origination trial in
                   cmcall_force_pref_on_the_fly.
03/26/04   ic      Undoing changes to cmcall_init() from 03/24/04 as it was
                   not correct.
                   Fixed compiler warning - removed unused local variable
                   from cmcall_orig_proc()
03/26/04   ka      Added a new function for processing
                   CMCALL_ACT_PROC_END_REASON
03/24/04   ic      In cmcall_force_pref_on_the_fly() replaced
                   ph_ptr->orig_srv_domain_pref with ph_ptr->main_stack_info.pref_info.srv_domain_pref
                   in call to cmph_force_pref_on_the_fly() as
                   ph_ptr->orig_srv_domain_pref is not supposed to be used
                   until actually set inside of cmph_force_pref_on_the_fly().
                   Removed unnecessary comments.
                   In cmcall_init() made sure we allocate memory only if
                   cm_call_obj_array[i] == NULL.
03/23/04   ws      Merged chagnes from 6300 branch.
03/19/04   ka      Updating the current system mode for all calls in
                   CONVERSATION.
03/18/04   sj      Fixed featurization problems.
03/15/04   ka      Merged Changes from 6250 branch
                   Fixed call reject in case of MO back to back calls.
                   Use service domain form service indicator for call orig.
                   Fixed Lint errors
                   Propagated service domain for call origination.
                   Fixed forcing service domain on the fly for GW system.
                   Unreserve connection ids even if call is not originated.
                   Reject PS data call origination when MB is in GSM voice
                   call.
                   Fixed Lint errors.
03/12/04   jqi     Use service domain form service indicator for call orig.
03/10/04   jqi     Propagated service domain for call origination.
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/13/04   ws      FEATURE_SILENT_REDIAL_20 support.
02/12/04   ic      Merged in changes from MSMSHARED_CM.01.00.10.00.20 to
                   MSMSHARED_CM.01.00.10.00.23
02/10/04   ws      Modified gsdi call to include mode with tc status.
02/09/04   ic      Changes so the code compiles for unit test.
02/06/04   ws      Initial Jaguar/Mainline merge.
01/29/04   ws      Modified gsdi call to include mode with tc status.
01/29/04   ws      Added function for handling gsdi events in both modes.
01/23/04   ws      Fixed bug with handling restricted calls.
01/21/04   RI      Modifications for attach on demand for GSM/WCDMA modes.
01/19/04   ka      Getting traffic in bytes from data services on call end.
                   Function to get call info list on CM command. Registration
                   function for data services to register a call back.
                   Multiple PDP support.
01/16/04   ws      Handle NULL gw_cc_modified_info for CDMA only calls.
01/13/04   ka      Merged changes from MSM6200 and CM Quad Band.
01/13/04   sj      Fixed bug in checking for sufficient service.
01/09/04   ws      Take a slot number when sending gstk voice envelopes.
12/16/03   rc/aaj  Support for FEATURE_HDR_HICPS20
12/16/03   ws      Return end status even if there is no mode yet.
                   Unreserve connection id regardless of call type.
                   Mapping between GSTK and CM NPI, TON types.
12/16/03   prk     Removed processing of CNM_REJ_NO_RESOURCES cause during
                   silent redial due to 11.1.2 test case failure.
12/15/03   tml     Added revision history comment
12/15/03   tml     Fixed call control
12/11/03   ic      Replaced is_hybr_redialled with is_hybr_redialed
12/11/03   ws      Check for call type instead of mode when sending
                   otasp_act_code.
12/08/03   ws      Reset cmd to orig after call control.
11/06/03   ws      Check for rejected calls from call control.
11/03/03   ws      Fix LTK compile errors.
10/30/03   ws      Merged ACT_END_ORIG handling from Cheetah
                   Merged ACT_PROC_END_REASON handling from Cheetah
                   Picked up call type changes in cc processing.
10/20/03   ws      Dual Slot Support.
                   Added calls to cmcall_client_cmd_err for success cases too.
                   Added privacy_pref handling without call ids.
09/03/03   jds     Correct bug when SIM fails (or is removed) during an
                   emergency call (do NOT end the call).
08/08/03   RI      Modified allocation of call ID for an incoming call in GSM
10/06/03   vt      Modified to support common cm.h across all targets.
10/01/03   ws      Merged changes from GW Targets
09/23/03   ws      Merged Emergency call dropped when SIM card removed fix.
09/18/03   ws      Updated handling of proc_end_reason.
09/10/03   ws      Fixed problem where if no mode info, calls not ending
                   correctly.
08/12/03   ws      Support for MM silent redial and automatic mode
07/22/03   vt      Fixed the function to call the right overall state function
05/14/03   ic      Under FEATURE_JCDMA_CM set actions for CM_CALL_END_MC_ABORT
                   and CM_CALL_END_PSIST_NG to CMCALL_ACT_END_ORIG
04/22/03   prk     Modified cmcall_proc_srv_status so only GSM/WCDMA calls
                   are processed.
04/11/03   prk     Removed references to FEATURE_AOC and FEATURE_MMGSDI.
03/25/03   kar     Fixed compile error on LTK
03/21/03   RI      Modified cmcall_proc_srv_status() to end the PS data calls
                   if there is no service.
03/05/03   RI      Modified cmcall_check_if_cs_voice_calls_active() to check
                   for calls in the active state instead of idle state.
02/27/03   cs      Added support for PS Continuity.
02/27/03   ws      Fixed cmcall_answer_para_check range
02/25/03   RI      Added cmcall_check_if_cs_voice_calls_active() and
                   cmcall_check_if_ps_call_active().
                   Modified cmcall_proc_srv_status() for PS attach on demand.
02/24/03   vt      Fixed a compile problem with HDR.
02/18/03   vt      Fixed problems to build for HDR.
02/13/03   prk     Mainlined the FEATURE_CONC_SRVC_NEW_API feature.
02/11/03   prk     Modified cmcall_client_cmd_proc() to handle supplementary
                   service commands.
02/04/03   prk     Modified cmcall_proc_srv_status() to end the emergency call
                   if a no service status is received from the lower layer.
           cs      Fixed cm_gsdi_in_traffic to use overall call state.
           cs      Extended service state change handling for regular call
                   silent origination.
01/24/03   ws      Merged changes for E911 calls to not tell searcher that
                   origination is pending.
01/21/03   ws      Updated copyright information for 2003
12/17/02   vt      Fixed a INTLOCK bug and a typo.
11/20/02   vt      Changed returned error code on invalid call id in client's
                   command.
11/19/02   vt      Changed error fatal to just error when call id is invalid
                   for a client command.
11/14/02   vt      Merged changes from GW mainline.
                   [PRK]Re-featured support for invalid SIM handling so it get
                   compiled for WCDMA/GSM only.
                   [ws]Merged skipped resetting orig error count if it is
                   OTHER (not L3).
                   [CS]Added support for invalid SIM handling. Terminate
                   active calls when SIM becomes invalid.
10/28/02   vt      Removed handling of NDSS origination failure.
10/22/02   ic      Under FEATURE_JCDMA_CM added special handling of
                   CM_CALL_END_REORDER call end in silent redial tables.
10/22/02   vt      Created a function to return call_id from call object ptr.
                   Removed is_allocated and call id from call object for
                   dynamic memory allocation.
10/21/02   vt      Replaced checking of call id with call object.
10/11/02   vt      Fixed couple of typo's for RUIM build.
10/09/02   vt      Decoupled call object reset from call id allocation.
                   Removed current mode determination functions.
10/05/02   vt      Modified to call the right handler(1x/GW) on orig command.
                   Added debug messages when allocating and deallocating a
                   call_id.
06/14/02   PRK     Removed initialization of call_type_ptr in
                   cm_find_connection_id.
06/11/02   RI      Modified (for GSM/WCDMA modes)
                    - cmcall_init() to check for call object array
                   allocation error.
                    - cm_new_incom_call_id() to set NULL ptr if call
                   object allocation fails.
                    - cm_call_id_allocate() to be called in different
                   modes for call ID allocation.
                    - cm_call_id_deallocate() to deallocate only the
                   call IDs.
                    - cmcall_client_cmd_check(). Deleted the call object
                   pointer parameter to this function.
                    - cm_find_connection_id() to set invalid values
                   for connection ID and call types if there is no
                   connection ID to call ID mapping.
                   Added cmcall_object_deallocate() to deallocate a
                   call object.
                   Renamed cm_new_call_id() to cm_new_connection_id().
05/28/02   RI      Fixed the debug message in cmcall_event() to print
                   the correct call event and call ID.
05/16/02   PRK     Checked the ptr returned in cmcall_ptr for null.  The
                   check was made in only the mode independent calls of
                   cmcall_ptr.
                   Modified cmcall_call_cmd_get_call_info_para_check to check
                   client_ptr and not client.
05/05/02   vt      Fixed compile problem with new mm cm.h
04/26/02   ATM     Added calls to cmlog_call_state().
03/11/02   RI      Used default value defintions for cause value and
                   CLIR from mn_cm_exp.h
02/19/02   PRK     Removed check of service state for CM_CALL_CMD_ANSWER
                   commands.  Client is allowed to answer incoming call with
                   no service.
02/06/02   PRK     Added function cmcall_overall_call_state.
01/28/02   PRK     Replaced cmutil_ prefix with cm_util_ prefix.
                   Modified cmcall_client_cmd_check to check is_subscription_-
                   available flag.
01/25/02   RI      Modified NotifySS processing for Call Forwarding
                   Removed the parameter check for N SEND and
                   N X SEND commands.
01/04/02   HD      Merge to Mainline
                   Do not reset call at end_req and copy cc_cause for Disc msg
11/21/01   RI      Modified cmwcall_info_get() to copy rab_id for CS Data.
                   Modified PS Data ind/conf processing.
                   Modified cm_find_call_id() to include the connection ID src
11/15/01   AT      Updated AOC to respond to MN.
11/06/01   RI      Modified cmwcall_cmd_copy_fields().
11/01/01   AT      Cleaning up AOC at end of call.
10/18/01   PRK     Added changes for WCDMA/GSM mode.
                   Modified code due to srv_state moving from phone group
                   to serving system group (FEATURE_NEWSS).
10/17/01   AT      Added Support for Advice of Charge (AOC)
10/01/01   AT      Added check for E911 changes.
08/23/01   HD      NAS header file reorg changes.
04/27/01   HD      Initial release for WCDMA/GSM.

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
#include "mmcp_variation.h"
#include "customer.h"  /* Customer configuration file */
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "sys_gw.h"
#include "sys_gw_v.h"
#include "cmi.h"       /* Internal interface to cm.c */

#include <string.h>    /* Interface to string library */
#include "rex.h"       /* Interface to rex services */
#include "comdef.h"    /* Definition for basic types and macros */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmclient.h"  /* Interface to CM Client Interface Block */
#include "cmph.h"      /* Interface to CM phone object */
#include "cmaccessctrl.h"
#include "cmlog.h"     /* Interface to diag services */
#include "cmlog_v.h"
#include "cmreply.h"
#include "cmtaski.h"
#include "time_svc.h"  /* Interface to clock services */
#include "prl.h"       /* Interface to PRL services */
#include "prl_v.h"
#include "cmcall.h"    /* Interface to CM call object */
#include "sd_v.h"      /* Interface to system Determination */
#include "cmpmprx.h"
#include "cmregprxi.h"

#ifdef FEATURE_CM_LTE
#include "cmltecall.h" /* Interface to CM LTE call interfaces */
#endif
#include "cmsds.h"      /* Interface to Domain selection object*/
#if defined( FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X) || defined (FEATURE_DOMAIN_SELECTION)
#include "cmcsfbcall.h" /* Interface to CM CSFB call object */
#endif

#if( defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
#include "cmxcall.h"   /* Interface to CM CDMA call object */
#include "cmxsms.h"    /* Interface to CM CDMA SMS */
#include "srch.h"      /* Interface to Search Task */
#include "srch_v.h"      /* Interface to Search Task */
#endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

#ifdef FEATURE_UIM_RUIM
#include "cmnv.h"
#endif /* FEATURE_UIM_RUIM */

/*lint -esym(766,cmgsdi.h)*/
#ifdef CM_GW_SUPPORTED
#include "smtask.h"
/*lint -esym(766,smtask_v.h)*/
#include "smtask_v.h"
#include "cmwaoc.h"
#include "cmsups.h"
#endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

#ifdef FEATURE_MMODE_3GPP
#include "cmwcall.h"
#endif

#if (defined(FEATURE_GSTK) || !defined(FEATURE_DUAL_SLOTS))
#include "gstk_exp.h"
#include "gstk_exp_v.h"
#endif /* (defined(FEATURE_GSTK) || !defined(FEATURE_DUAL_SLOTS)) */

#include "cmutil.h"
#include "cmss.h"
#include "cmmsc.h"

#ifdef FEATURE_HDR_HANDOFF
#include "cmssidm.h"
#endif

#ifdef FEATURE_GPSONE
#include "cmxpd.h"
#endif

#ifdef FEATURE_IP_CALL
#include "cmipcall.h"    /* CM IP call interface */
#include "cmipappi.h"    /* Interface to query registered apps */
#endif

#ifdef FEATURE_DCVS_LPM
#include "clkregim.h"
#endif

#ifdef FEATURE_ECALL_IVS
#include "ecall_modem_apis.h"
#endif

#include "cmstats.h"


#include "cmmmgsdi.h"


#if (defined (FEATURE_MMODE_CDMA_800) || defined (FEATURE_MMODE_CDMA_1900))
#include"cmxll.h"
#include "cmxdbm.h"
#endif

#include "cmefs.h"

#ifdef FEATURE_GSM
#include "geran_eng_mode_read_api.h"
#endif

#ifdef FEATURE_WCDMA
#include "rrcmmif.h"

#ifdef FEATURE_SEGMENT_LOADING
#include "ITDSCDMA.h"
#include "IWCDMA.h"
#endif

#endif
#include "cmtaski.h"

#include "cmph.h"
#ifdef CM_DEBUG
#error code not present
#endif

#include "cmcc.h"
#include "cmpsm.h"
#include "ran.h"        /* for ran_dist */
#include "secapi.h"     /* for secure random */
#include "mm.h"
#include "cmemgext.h"
#include "subs_prio.h"
#include "cmpmprx.h"
#include "cmclnup.h"

/**--------------------------------------------------------------------------
** Forward declarations
** --------------------------------------------------------------------------
*/

cm_band_pref_e_type  cmcall_get_favored_band_pref(
    cm_call_type_e_type    call_type,
    cm_srv_type_e_type     srv_type,
    cm_band_pref_e_type    band_pref
);

sys_lte_band_mask_e_type        cmcall_get_favored_lte_band_pref(
    cm_call_type_e_type    call_type,
    sys_lte_band_mask_e_type    lte_band_pref
);

cm_band_pref_e_type  cmcall_get_favored_tds_band_pref(
    cm_call_type_e_type    call_type,
    cm_band_pref_e_type    tds_band_pref
);

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/
#define CMCALL_TIMER_INACTIVE                ((dword)(-1))
   /* This is the max value of the timer.  Indicates the timer is not
   ** currently used right now.
   */

#define CMCALL_RAND_LIMITER 2147483647
/* Used in preparing a random number for use in ran_dist */

/*---------------------------------------------------------------------------
                         TUNABLE CALL PARAMETERS
---------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Powerup-hold-origination feature to hold origination right after powerup
** giving service a chance to appear.
*/
#ifdef FEATURE_CM_PWRUP_HOLD_ORIG

#define CMCALL_PWRUP_HOLD_ORIG_UPTIME        12  /* seconds */
    /* Max uptime (i.e. time since powerup) to hold origination
    ** giving service a chance to appear right after powerup */

#else /* #ifdef FEATURE_CM_PWRUP_HOLD_ORIG */

#define CMCALL_PWRUP_HOLD_ORIG_UPTIME        0  /* seconds */

#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Preferred-service-hold-origination feature to hold origination after
** powerup or mode preference change giving preferred service a chance
** to appears once sufficient service is available.
*/
#ifdef FEATURE_CM_PREF_SRV_HOLD_ORIG

#define CMCALL_PREF_SRV_HOLD_ORIG_TIME       5  /* seconds */
    /* Max time to hold origination giving preferred service a chance
    ** to appears once sufficient service is available */

#else /* #ifdef FEATURE_CM_PREF_SRV_HOLD_ORIG */

#define CMCALL_PREF_SRV_HOLD_ORIG_TIME       0  /* seconds */

#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define CMCALL_ACQ_HOLD_SS_PREF_TIME         3  /* seconds */
    /* Max time to hold system selection preference change during origination
    ** in case system determination is currently trying to acquire service */

#define CMCALL_ACC_HOLD_ORIG_TIME            20  /* seconds */
    /* Max time to hold origination after receiving origination failure
    ** reason of CM_CALL_ORIG_ACC_IN_PROG (i.e., origination fails do to
    ** redirection, handoff, etc) */

#define CMCALL_CALL_COLLISION_HOLD_ORIG_TIME      2  /* seconds */
    /* Max time to hold origination after receiving origination failure
    ** reason of CM_CALL_ORIG_CALL_COLLISION (i.e., origination fails do to
    ** E911 and MT Call Collision in MC) */


#ifdef CM_GW_SUPPORTED
#define CMCALL_MN_CONNECTION_ID_HOLD_FATAL_ERR_TIME     30  /* seconds */
    /* Max time to hold from error fataling if CM can no longer allocate
    ** connectionids from MN.
    */

#define CMCALL_SM_CONNECTION_ID_HOLD_FATAL_ERR_TIME     30  /* seconds */
    /* Max time to hold from error fataling if CM can no longer allocate
    ** connectionids from SM.
    */

#define CMCALL_IMMED_REDIAL_TIME                        1 /* seconds */
    /* Certain NAS failures are due to short term states and a fast redial
    ** is possible
    */

#define CMCALL_DS_VOICE_HOLD_ORIG_TIME                  3 /* seconds */
    /* Max time the voice call is under hold, when a ps data call is connected
    ** on another subscription.
    */

#endif /* FEATURE_GSM || FEATURE_WCDMA */

#if defined(FEATURE_GPSONE)
#define CMCALL_PD_SESSION_ACK_HOLD_ORIG_TIME           1 /* seconds */
  /* Time to wait for PD session end ack
  */

#else
#define CMCALL_PD_SESSION_ACK_HOLD_ORIG_TIME           0 /* seconds */
#endif /* FEATURE_GPSONE */

#if((defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
     defined(FEATURE_CDSMS))

#define CMCALL_SMS_ACK_HOLD_ORIG_TIME                  6 /* seconds */
  /* Time to wait for SMS ack
  */
#endif

#if (defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE)
#define CMCALL_PS_CALL_HOLD_ORIG_UPTIME                5 /* seconds */
    /* Time to increase PS call hold origination time */
#endif

#define CMCALL_PS_CALL_WAIT_SRV_CNF_HOLD_ORIG_UPTIME                10 /* seconds */
	/* Time to hold PS call when other SUB is in WAIT_SRV_CNF state */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

// instance pointer for segment loading of TDS or WCDMA
#ifdef FEATURE_SEGMENT_LOADING
extern interface_t *pi_mmode_tdscdma;
extern interface_t *pi_mmode_wcdma;
#endif

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/* A list of Transaction IDs are maintained to fix the race scenarios b/w CM and SM.
** Race scenario exists only for MO calls.
** Even though the array is defined for MAX_PDP_NUM, indexes upto MAX_MO_TI (including)
** are used to keep track of PDP transaction id allocated.
** Actual memory allocation for PDP context is done at the time of receiving
** PDP activation request for Primary and Secondary calls.
*/
static boolean pdp_context_trans_id[CM_MAX_PDP_NUM][MAX_SIMS];

static boolean call_context_trans_id[MAX_MO_CONNECTION_ID+1][MAX_SIMS];

/* array of call objects */
/* in_CS for non-CM: Array only in critical section, not call objects. */
static cmcall_s_type        *cm_call_obj_array[CMCALL_MAX_CALL_OBJ];

#ifdef CM_GW_SUPPORTED
static cm_overall_calls_s_type  cmwcall_overall_calls;
#endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

/* Defination for SSAC (service specific access control) info.
** SSAC feature is introduced in E-UTRA Release 9.
** SSAC facilitates independent access control of Multimedia Telephony (MMTel) [1] video
** and audio sessions over E-UTRA.
** Specs: 3GPP TS 24.173 v9.1 and 3GPP TS 36.331, Rel 9.
*/

static cm_voice_call_sr_timer_s_type cmcall_sr_timer_info[MAX_SIMS];

#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
static boolean cdma_ps_call_hold_timer_set = FALSE;
#endif

/* Pointer to the function that is called to get the total number of bytes transmitted in a data call */

#ifdef FEATURE_CM_PWRUP_HOLD_ORIG
#ifdef FEATURE_CM_PREF_SRV_HOLD_ORIG
#if (defined FEATURE_GSTK && defined FEATURE_MMODE_3GPP)
typedef struct cmcall_gstk_cc_info_s
{
   cm_nc_call_control_complete_cb_f_type   *cm_callback_func;
   cm_cmd_type                           *new_cmd_ptr;
   cm_call_id_type                          call_id;

} cmcall_gstk_cc_info_s_type;

/* in_CS for All tasks */
static cmcall_gstk_cc_info_s_type    cmcall_gstk_cc_info =
                                      { NULL, NULL, CMCALL_MAX_CALL_OBJ};
#endif
#endif
#endif

/**--------------------------------------------------------------------------
** Foward declerations
** -------------------------------------------------------------------------
*/

#ifdef CM_GW_SUPPORTED

static void cmcall_process_act_proc_end_reason (

  cmcall_s_type *call_ptr,
         /* pointer to a call object */

  cmcall_act_e_type  redial_act
         /* The redial action */
);
#endif
#if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE)
static void cmcall_end_call(

   cmcall_s_type  *call_ptr
     /* pointer to a call object */
);


#endif /* #ifdef CM_GW_SUPPORTED */

static void cmcall_client_cmd_err_deallocate(

    cm_call_cmd_s_type       *call_cmd_ptr,
        /* Pointer to a CM command */

    cm_call_cmd_err_e_type    cmd_err
        /* Indicate the call command error */
);

static void cmcall_check_and_end_calls_on_other_subs
(
  sys_modem_as_id_e_type asubs_id,
  cm_call_type_e_type call_type
);

/*===========================================================================

FUNCTION sr_reset

DESCRIPTION
  Reset a silent redial component, so it is ready for next call.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void sr_reset(

    cm_sr_s_type  *sr_ptr
        /* pointer to silent redial object to reset */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sr_ptr->orig_plan_ptr              = NULL;
  sr_ptr->sr_start_uptime            = 0;
  sr_ptr->sr_max_uptime              = 0;

  sr_ptr->is_ok_to_orig              = FALSE;

  sr_ptr->orig_sent_on_mode          = SYS_SYS_MODE_NONE;
  sr_ptr->hold_orig_uptime           = 0;
  sr_ptr->next_orig_uptime           = 0;
  sr_ptr->sr_left                    = 0;

  #ifdef FEATURE_IS2000_REL_A
  sr_ptr->is_re_orig                 = FALSE;
  sr_ptr->re_orig_reason             = FALSE;
  sr_ptr->re_orig_count              = 0;
  sr_ptr->orig_fail_layer            = CM_CALL_ORIG_ERR_LAYER_OTHER;
  #endif /* FEATURE_IS2000_REL_A */

  #ifdef FEATURE_IP_CALL
  sr_ptr->fall_back_to_wifi_from_cs  = -1;
  sr_ptr->ignore_sr_max_uptime = FALSE;  
  sr_ptr->wlan_fallback_timer = -1;
  sr_ptr->emerg_call_attempt_state = CMCALL_ATTEMPT_STATE_NONE;
  #endif


  #ifdef FEATURE_HDR_HYBRID
  sr_ptr->hdr_is_ok_to_orig          = FALSE;
  sr_ptr->is_hdr_not_avail           = FALSE;
  #endif /* FEATURE_HDR_HYBRID */

  sr_ptr->is_hybr_redialed          = FALSE;

  #ifdef FEATURE_CALL_THROTTLE
  sr_ptr->is_call_throttled         = FALSE;
  #endif

  /* VOIP silent redial initialization.
  */
  #ifdef  FEATURE_VOIP_E911_SUPPORT
  sr_ptr->voip_sr.e911_voip_fallback_uptime      = 0;
  sr_ptr->voip_sr.e911_voip_redial  = CMCALL_VOIP_REDIAL_NONE;
  #endif

  sr_ptr->voip_sr.is_valid = FALSE;

  sr_ptr->voip_sr.lte_soft_failure_count = 0;
  sr_ptr->voip_sr.is_lte_hard_failure = FALSE;
} /* sr_reset() */

/**--------------------------------------------------------------------------
** Functions - internal
** --------------------------------------------------------------------------
*/


#ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
/*===========================================================================
FUNCTION cmcall_ssac_ptr

DESCRIPTION
  Return a pointer to the SSAC object


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cmcall_ssac_s_type  *cmcall_ssac_ptr( void )
{

  /* Defination for SSAC (service specific access control) info.
  ** SSAC feature is introduced in E-UTRA Release 9.
  ** SSAC facilitates independent access control of Multimedia Telephony (MMTel) [1] video
  ** and audio sessions over E-UTRA.
  ** Specs: 3GPP TS 24.173 v9.1 and 3GPP TS 36.331, Rel 9.
  */
  static cmcall_ssac_s_type cmcall_ssac_info;
  return &cmcall_ssac_info;

} /* cmcall_ssac_ptr() */

/*===========================================================================
FUNCTION cmcall_get_r12_ssac_info

DESCRIPTION
  Return r12_ssac based on asubs_id


DEPENDENCIES
  none

RETURN VALUE
  r12_ssac

SIDE EFFECTS
  none

===========================================================================*/

uint8 cmcall_get_r12_ssac_info(
  sys_modem_as_id_e_type asubs_id,
  cmcall_ssac_s_type  *cmcall_ssac_info
)
{
  if(cmcall_ssac_info == NULL)
  {
    return 0;
  }
  if(!(asubs_id >= SYS_MODEM_AS_ID_1 && asubs_id < MAX_SIMS
    && asubs_id < SYS_MODEM_AS_ID_NO_CHANGE))
  {
    asubs_id = SYS_MODEM_AS_ID_1;
  }
  
 CM_MSG_MED_1("r12_ssac %d", cmcall_ssac_info->r12_ssac);

  return cmcall_ssac_info->r12_ssac[asubs_id];

}

/*===========================================================================

FUNCTION cmcall_ssac_init

DESCRIPTION
  Initialize ssac related params.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void cmcall_ssac_init(sys_modem_as_id_e_type asubs_id)
{
  int32 read_size = 0;
  cmcall_ssac_s_type *cmcall_ssac_info = cmcall_ssac_ptr();
  cmph_s_type           *ph_ptr        = cmph_ptr();


  if(!(asubs_id >= SYS_MODEM_AS_ID_1 && asubs_id < MAX_SIMS
    && asubs_id < SYS_MODEM_AS_ID_NO_CHANGE))
  {
    asubs_id = SYS_MODEM_AS_ID_1;
  }

  /* Initialize ssac params to no barring */
  cmcall_ssac_info->ssac_params.barring_factor_mmtel_voice
                       = LTE_RRC_ACCESS_BARRING_PROB_p100;
  cmcall_ssac_info->ssac_params.barring_time_mmtel_voice = 0;
  cmcall_ssac_info->ssac_params.barring_factor_mmtel_video
                       = LTE_RRC_ACCESS_BARRING_PROB_p100;
  cmcall_ssac_info->ssac_params.barring_time_mmtel_video = 0;

  /* Initialize ssac timers */
  cmcall_ssac_info->ssac_voice_uptime = CMCALL_SSAC_UPTIME_NONE;
  cmcall_ssac_info->ssac_video_uptime = CMCALL_SSAC_UPTIME_NONE;

  /* Initialize plmn */
  cmcall_ssac_info->plmn.num_mnc_digits= 3;
  cmcall_ssac_info->plmn.mcc[0] = 0xF;
  cmcall_ssac_info->plmn.mcc[1] = 0xF;
  cmcall_ssac_info->plmn.mcc[2] = 0xF;
  cmcall_ssac_info->plmn.mnc[0] = 0xF;
  cmcall_ssac_info->plmn.mnc[1] = 0xF;
  cmcall_ssac_info->plmn.mnc[2] = 0xF;
  cmcall_ssac_info->cell_id = 0xFFFFFFFF;

  read_size = sizeof (uint8);

    cmcall_ssac_info->r12_ssac[asubs_id] = 1;
    CM_MSG_HIGH_0("Can't read r12_ssac");


  cmcall_ssac_info->backoff_timer_accumulate = 0;
  /*Define the SSAC delay timer for RRC 20ms barring time handling*/
  rex_def_timer_ex(&cmcall_ssac_info->ssac_delay_timer, cmcall_ssac_delay_timer_cb, 0);
  sys_plmn_undefine_plmn_id(&(cmcall_ssac_info->ssac_associated_plmn));
  return;
}

/*===========================================================================

FUNCTION cmcall_start_ssac_timer

DESCRIPTION
  This function starts ssac timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void  cmcall_start_ssac_timer(

  cmcall_ssac_mmtel_e_type  mmtel_type,
  /* MMTEL type: voice or video */

  dword                     timeout
  /* Timeout value */
)
{
  dword curr_uptime  = time_get_uptime_secs();
  cmcall_ssac_s_type  *cmcall_ssac_info = cmcall_ssac_ptr();

  switch (mmtel_type)
  {
    case SSAC_MMTEL_TYPE_VOICE:

      /* Start ssac_voice backoff timer if not running */
      if ( cmcall_ssac_info->ssac_voice_uptime == CMCALL_SSAC_UPTIME_NONE )
      {
        /* Timer not running, start it */
        cmcall_ssac_info->ssac_voice_uptime = curr_uptime + timeout;

        CM_MSG_HIGH_2("Start SSAC voice barring timer: curr=%d timeout=%d",
                     curr_uptime,
                     cmcall_ssac_info->ssac_voice_uptime);

      }

      /* This should not happen.  The new timer should start only if the old one expires */
      else
      {
        CM_ERR_2("SSAC voice barring timer already running: curr=%d timeout=%d",
                curr_uptime,
                cmcall_ssac_info->ssac_voice_uptime);
      }
      break;

    case SSAC_MMTEL_TYPE_VIDEO:

      /* Start ssac_video backoff timer if not running */
      if ( cmcall_ssac_info->ssac_video_uptime == CMCALL_SSAC_UPTIME_NONE )
      {
        /* Timer not running, start it */
        cmcall_ssac_info->ssac_video_uptime = curr_uptime + timeout;

        CM_MSG_HIGH_2("Start SSAC video barring timer: curr=%d timeout=%d",
                     curr_uptime,
                     cmcall_ssac_info->ssac_video_uptime);
      }

      /* This should not happen.  The new timer should start only if the old one expires */
      else
      {
        CM_ERR_2("SSAC video barring timer already running: curr=%d timeout=%d",
                curr_uptime,
                cmcall_ssac_info->ssac_video_uptime);
      }
      break;

    default:
      CM_ERR_1("Invalid mmtel_type: %d for starting ssac timer", mmtel_type);
      return;
  }

  /* Send QXDM ssac_timer event
   */
  cmlog_ssac_timer_event_handler(SSAC_TIMER_ACTION_START,
                                 mmtel_type,
                                 (uint32)timeout);

}
/*===========================================================================

FUNCTION cmcall_reset_ssac_delay_timer

DESCRIPTION
  This function resets ssac delay backoff timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void cmcall_reset_ssac_delay_timer()
{
  cmcall_ssac_s_type  *cmcall_ssac_info = cmcall_ssac_ptr();
   /*be causcous resetting this timer. there might be call in pending state which needs to be cleaned up*/
  (void) rex_clr_timer(&cmcall_ssac_info->ssac_delay_timer);
  cmcall_ssac_info->backoff_timer_accumulate = 0;
  cmcall_ssac_info->call_cmd_ptr = NULL;
}
/*===========================================================================

FUNCTION cmcall_reset_ssac_timer

DESCRIPTION
  This function resets ssac timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void  cmcall_reset_ssac_timer(
  cmcall_ssac_mmtel_e_type          mmtel_type,
  /* voice/video */

  cmcall_ssac_timer_action_e_type   timer_action
  /* start/stop/cancel ssac timer */
)
{
  cmcall_ssac_s_type  *cmcall_ssac_info = cmcall_ssac_ptr();
  /* Return if timer_action is not to stop/cancel timer
   */
  if (timer_action != SSAC_TIMER_ACTION_STOP &&
      timer_action != SSAC_TIMER_ACTION_CANCEL)
  {
    CM_ERR_1("SSAC: invalid timer_action %d for reset ssac timer",timer_action);
    return;
  }

  /* Reset the timer based on mmtel_type
   */
  switch (mmtel_type)
  {
    case SSAC_MMTEL_TYPE_VOICE:
      if(cmcall_ssac_info->ssac_voice_uptime != CMCALL_SSAC_UPTIME_NONE)
      {
        cmcall_ssac_info->ssac_voice_uptime = CMCALL_SSAC_UPTIME_NONE;
      }
      break;

    case SSAC_MMTEL_TYPE_VIDEO:
      if(cmcall_ssac_info->ssac_video_uptime != CMCALL_SSAC_UPTIME_NONE)
      {
        cmcall_ssac_info->ssac_video_uptime = CMCALL_SSAC_UPTIME_NONE;
      }
      break;

    case SSAC_MMTEL_TYPE_VOICE_VIDEO:
      if(cmcall_ssac_info->ssac_voice_uptime != CMCALL_SSAC_UPTIME_NONE)
      {
        cmcall_ssac_info->ssac_voice_uptime = CMCALL_SSAC_UPTIME_NONE;
      }

      if(cmcall_ssac_info->ssac_video_uptime != CMCALL_SSAC_UPTIME_NONE)
      {
        cmcall_ssac_info->ssac_video_uptime = CMCALL_SSAC_UPTIME_NONE;
      }

      break;

    default:
      return;
  }

  /* Send QXDM ssac_timer event
   */
  cmlog_ssac_timer_event_handler(timer_action, mmtel_type, 0);

}

/*===========================================================================

FUNCTION cmcall_get_ssac_call_type

DESCRIPTION
  Determine wheter the ssac calll type is voice/video/none.
  SSAC checking only applies to those call types.

DEPENDENCIES
  None

RETURN VALUE
  mmtel_type (MMTEL voice/video over LTE/none).

SIDE EFFECTS
  None

===========================================================================*/
static cmcall_ssac_mmtel_e_type cmcall_get_ssac_call_type (
  const cmcall_s_type  *call_ptr
  /* pointer to Call cmd */
)
{
  cmcall_ssac_mmtel_e_type mmtel_type = SSAC_MMTEL_TYPE_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check cmd_ptr - should not be NULL */
  if (call_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* MMTEL voice over LTE */
  if ( call_ptr->is_volte == TRUE
       &&
       call_ptr->call_type == CM_CALL_TYPE_VOICE)
  {
    mmtel_type = SSAC_MMTEL_TYPE_VOICE;

  }

  /* MMTEL voice over LTE */
  else if (  call_ptr->is_volte == TRUE
             &&
             (call_ptr->call_type == CM_CALL_TYPE_VT
             ||
             call_ptr->call_type == CM_CALL_TYPE_VS
             ||
             call_ptr->call_type == CM_CALL_TYPE_VT_LOOPBACK))
  {
    mmtel_type = SSAC_MMTEL_TYPE_VIDEO;
  }

  return mmtel_type;

}

/*===========================================================================

FUNCTION cmcall_is_ssac_barring_diabled

DESCRIPTION
  Check if ssac barring is disabled.  ssac barring factor =100 indicates ssac is disabled and
  UE allows the call attempt.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if ssac is disabled.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
static boolean cmcall_is_ssac_barring_diabled (
  cmcall_ssac_mmtel_e_type  mmtel_type
)
{
  cmcall_ssac_s_type  *cmcall_ssac_info = cmcall_ssac_ptr();
  CM_ASSERT_ENUM_IS_INRANGE(SSAC_MMTEL_TYPE_NONE, SSAC_MMTEL_TYPE_MAX);
  
  switch (mmtel_type)
  {
    case SSAC_MMTEL_TYPE_VOICE:

      if(cmcall_ssac_info->ssac_params.barring_factor_mmtel_voice
         != LTE_RRC_ACCESS_BARRING_PROB_p100)
      {
        return FALSE;
      }
      break;

    case SSAC_MMTEL_TYPE_VIDEO:

      if(cmcall_ssac_info->ssac_params.barring_factor_mmtel_video
         != LTE_RRC_ACCESS_BARRING_PROB_p100)
      {
        return FALSE;
      }
      break;

    default:
      CM_ERR_1("SSAC - Invalid mmtel_type: %d", mmtel_type);
      return TRUE;

  }

  return TRUE;

}

/*===========================================================================
FUNCTION cmcall_is_ssac_timer_running

DESCRIPTION
  Check if the ssac timer is running.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the timer is running.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
static boolean cmcall_is_ssac_timer_running (
  cmcall_ssac_mmtel_e_type  mmtel_type
)
{
  cmcall_ssac_s_type  *cmcall_ssac_info = cmcall_ssac_ptr();
  dword               ssac_uptime = CMCALL_SSAC_UPTIME_NONE;
  CM_ASSERT_ENUM_IS_INRANGE(SSAC_MMTEL_TYPE_NONE, SSAC_MMTEL_TYPE_MAX);

  switch (mmtel_type)
  {
    case SSAC_MMTEL_TYPE_VOICE:

      if(cmcall_ssac_info->ssac_voice_uptime!= CMCALL_SSAC_UPTIME_NONE)
      {
        ssac_uptime = cmcall_ssac_info->ssac_voice_uptime;
      }
      break;

    case SSAC_MMTEL_TYPE_VIDEO:

      if(cmcall_ssac_info->ssac_video_uptime!= CMCALL_SSAC_UPTIME_NONE)
      {
        ssac_uptime = cmcall_ssac_info->ssac_video_uptime;
      }
      break;

    default:
      break;
  }

  if(ssac_uptime != CMCALL_SSAC_UPTIME_NONE)
  {
    CM_MSG_HIGH_2("SSAC: backoff timer is running, mmtel_type = %d, ssac_uptime = %d",
                mmtel_type ,ssac_uptime);
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================

FUNCTION cmcall_get_random_number_in_range

DESCRIPTION
  Returns a random number:
    lo_val <= return value < hi_val
  Randomness taken from more secure source secapi

DEPENDENCIES
  None

RETURN VALUE
  The random number requested

SIDE EFFECTS
  Uses up some random information in Secapi

===========================================================================*/

word cmcall_get_random_number_in_range (
    word  lo_val,
    word  hi_val
)
{
  dword secureRandom;
  secerrno_enum_type errorVal;

  /* SECAPI_SECURE_RANDOM determines the source of randomness and whether to
  ** filter out zero-values.  We are asking for high-security here, but we
  ** zero-values are ok for us.
  */
  errorVal = secapi_get_random(SECAPI_SECURE_RANDOM,
                               (uint8*)&secureRandom,
                               sizeof(*(&secureRandom)));

  if (errorVal != E_SUCCESS) {
    /* Documentation for secapi says:
    ** "On Handling error conditions:
    **  all errors should be treated as FATAL and exited."
    */
    CM_ERR_FATAL_1("Error getting HW random from Secapi: %d", errorVal);
  }

  /* Make this number a left-justified 31-bit int for ran_dist()
  ** by copying clean-up behavior of previous api rand_next()
  ** (steps 4 and 5 of rand_next).
  */
  if (secureRandom >= CMCALL_RAND_LIMITER){
    secureRandom -= CMCALL_RAND_LIMITER;
  }

  secureRandom = secureRandom << 1;

  return ran_dist(secureRandom, lo_val, hi_val);
}

/*===========================================================================

FUNCTION cmcall_perform_ssac_barring_check

DESCRIPTION
  Perform ssac barring check based on ssac params.
  I)  draw a new random number "rand1" that is uniformly distributed in the
      range 0 < rand1 < 1; and
  II) if the random number "rand1" is lower than BarringFactorForMMTEL-Video,
      then skip the rest of steps below and continue with session
      establishment as described in subclause 5.2;
  III) else, then;
  i)  draw a new random number "rand2" that is uniformly distributed in the
      range 0 < rand2 < 1; and
  ii) start back-off timer Tx with the timer value calculated using the
      formula:
      Tx = (0,7 + 0,6*rand2) * BarringTimeForMMTEL-Video; and
  iii) reject the multimedia telephony communication session establishment and
       skip the rest of steps.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the ssach check results indicating pass/fail and backoff time if
  fail.

SIDE EFFECTS
  None

===========================================================================*/
static void cmcall_perform_ssac_barring_check (
  uint8  barring_factor,
  uint16 barring_time,
  cmcall_ssac_check_result_s_type *ssac_check_result_ptr
)
{
  uint16 random_number;
  uint32 backoff_time;
  uint32 timer_to_set;
  cmcall_ssac_s_type  *cmcall_ssac_info = cmcall_ssac_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Should not be null */
  if (ssac_check_result_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Verify the ranges of input parameters
  ** barring factor: 0 -100, 100 - allow always, 0 -barred
  ** barring time:   0 - 512 secs
  */
  if (barring_factor > LTE_RRC_ACCESS_BARRING_PROB_p100
    || barring_time > CMCALL_SSAC_MAX_BACKOFF_TIME)
  {
    CM_ERR_FATAL_2("SSAC barring check: invalid barring factor = %d or barring_time = %d",
                 barring_factor, barring_time);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*Draw a random number between 0 and 100*/

  random_number = cmcall_get_random_number_in_range(0, 100);

  CM_MSG_HIGH_2("SSAC check, rand1=%d, barring_factor=%d",
               random_number,
               barring_factor);

  /* Pass the ssac barring check if rand1 < probability */
  if(random_number < (uint16)barring_factor)
  {
    ssac_check_result_ptr->is_ssac_check_pass = SSAC_CHK_PASS;
    ssac_check_result_ptr->ssac_backoff_time = 0;
  }

  /* If reach here, ssac barring check failed.
   ** Draw another random number to calculate the backoff time.
   ** Backoff timer Ty =  (0.7 + 0.6*rand2) * BarringTime
   */
  else
  {
    random_number = cmcall_get_random_number_in_range(0, 100);
    backoff_time = (700+6*random_number)*barring_time/1000 ;
    CM_MSG_HIGH_1("Backoff random: %d/100", random_number);

   /* Need special handling for barring_timer 20. RRC will read the EFS: 
   ** and only then would it send the special barring time value of 20
   ** when RRC send 20ms as barring time for mmtel, it needs to be treated as delayed success. Cm will loop 
   ** N times to draw a random number if it is greater than barring factor. until drawed random number is smaller
   ** than Barring factor or N times loop end, CM will return success for SSAC checking. for barring factor 0, 
   ** also need to start back off timer. RRC will make sure that barring factor 0 and barring time 20 will never come together. 
   ** If barring factor p00 is received by CM, it means legacy behavior should be applied. Thus, stop delay timer and start backoff timer.
   ** for(i=0;i<N;i++)
   ** {
   **   if (Rand < barring_factor)
   **     return;
   **   else
   **   {
   **     calculate backoff timer T based on 20ms;
   **     delay Tms;
   **   }
   ** }
   */
    if(barring_time == 20)
    {
      /*start the delay timer with 1s as cap*/
      if(cmcall_ssac_info->backoff_timer_accumulate >= 1000)
      {
        ssac_check_result_ptr->is_ssac_check_pass = SSAC_CHK_PASS;
        ssac_check_result_ptr->ssac_backoff_time = 0;
      }
      else
      {
        timer_to_set = MIN(backoff_time*10, 1000-cmcall_ssac_info->backoff_timer_accumulate);
        (void) rex_set_timer(&cmcall_ssac_info->ssac_delay_timer, timer_to_set);
        cmcall_ssac_info->backoff_timer_accumulate += timer_to_set;
        ssac_check_result_ptr->is_ssac_check_pass = SSAC_CHK_WAIT;
        ssac_check_result_ptr->ssac_backoff_time = 0;
      }
    }
    else
    {
      ssac_check_result_ptr->is_ssac_check_pass = SSAC_CHK_FAIL;
      ssac_check_result_ptr->ssac_backoff_time = backoff_time;
    }
  }
}

/*===========================================================================

FUNCTION cmcall_ssac_delay_timer_cb
  callback function invoked when ssac_delay_timer expire
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmcall_ssac_delay_timer_cb(unsigned long dummy)
{
  (void) dummy;

  if(cmcall_ssac_ptr()->call_cmd_ptr != NULL)
  {
    cm_cmd_queue ((cm_cmd_type *)cmcall_ssac_ptr()->call_cmd_ptr);
    return;
  }
}


/*===========================================================================

FUNCTION cmcall_is_ssac_barring_check_pass

DESCRIPTION
  Perform ssac based on ssac params and return TURE if pass and FALSE if fail.  Also start
  backoff timer if fail.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if ssac passes barring check.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
static cmcall_ssac_chk_e_type cmcall_is_ssac_barring_check_pass (
  cmcall_ssac_mmtel_e_type mmtel_type
)
{
  uint8  barring_factor;
  uint16 barring_time;
  cmcall_ssac_check_result_s_type ssac_check_result;
  cmcall_ssac_s_type  *cmcall_ssac_info = cmcall_ssac_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Determine barring factor and barring time */
  switch (mmtel_type)
  {
    case SSAC_MMTEL_TYPE_VOICE:

      barring_factor = cmcall_ssac_info->ssac_params.barring_factor_mmtel_voice;
      barring_time = cmcall_ssac_info->ssac_params.barring_time_mmtel_voice;
      break;

    case SSAC_MMTEL_TYPE_VIDEO:
      barring_factor = cmcall_ssac_info->ssac_params.barring_factor_mmtel_video;
      barring_time = cmcall_ssac_info->ssac_params.barring_time_mmtel_video;
      break;

    default:
      CM_ERR_1("SSAC - Invalid mmtel_type: %d", mmtel_type);
      return SSAC_CHK_PASS;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Perform ssac barring check
   */
  cmcall_perform_ssac_barring_check(barring_factor,
                                    barring_time,
                                    &ssac_check_result);

  /* Generate ssac call event
   */
  cmlog_ssac_call_event_handler(ssac_check_result.is_ssac_check_pass,
                                mmtel_type,
                                cmcall_ssac_info);

  /* Return ssac check result
   */
  switch(ssac_check_result.is_ssac_check_pass)
  {
  case SSAC_CHK_FAIL:
    cmcall_start_ssac_timer(mmtel_type, ssac_check_result.ssac_backoff_time);
    break;
  default:
    break;
  }
  
  return ssac_check_result.is_ssac_check_pass;

}

/*===========================================================================

FUNCTION cmcall_is_call_allowed_per_ssac

DESCRIPTION
  This function checks if the LTE MMTEL call origination attempt is allowed in terms of ssac.

  The call will be allowed if
  - ssac timer is not running.
  - not barred based on barring factor and time calculation.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the call attempt is allowed.
  FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
static cmcall_ssac_chk_e_type  cmcall_is_call_allowed_per_ssac(
  cmcall_s_type  *call_ptr
  /* pointer to Call cmd */
)
{

  cmcall_ssac_mmtel_e_type mmtel_type;
  cmregprx_stack_info_s_type      *cmregprx_stack_info_ptr = NULL;
  cmcall_ssac_s_type  *cmcall_ssac_info = cmcall_ssac_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check cmd_ptr - should not be NULL */
  if (call_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!cmph_is_lte_capable_on_sub(call_ptr->asubs_id))
  {
    return SSAC_CHK_PASS;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if emergency call - no need to perform ssac check */
  if (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
  {
    return SSAC_CHK_PASS;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check ssac call type - no need to perform ssac check if it is not.
   ** ssac_call_type: MMTEL voice/video over LTE
   */
  mmtel_type = cmcall_get_ssac_call_type(call_ptr);

  if (mmtel_type == SSAC_MMTEL_TYPE_NONE)
  {
    return SSAC_CHK_PASS;
  }

  /* Check RRC connected mode - allow the call if it is in connected mode */
  #ifdef FEATURE_LTE


  if ((cmcall_get_r12_ssac_info(call_ptr->asubs_id, cmcall_ssac_info) == 0) &&
    mm_per_stacks_get_connection_status(SYS_MODEM_AS_ID_1,0)
     == SYS_GW_CONN_STATUS_CONN)
  {
    return SSAC_CHK_PASS;
  }
  #endif


  /* Check the ssac timer - reject the call if it is running */
  if (cmcall_is_ssac_timer_running(mmtel_type))
  {
    return SSAC_CHK_FAIL;
  }

  /* Check the barring factor - allow the call if factor == 1 */
  if (cmcall_is_ssac_barring_diabled(mmtel_type))
  {
    return SSAC_CHK_PASS;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If reach here - need to perform ssac barring check based on ssac params.
   ** The function also starts the backoff timer if ssac barring check fails.
   */
  return cmcall_is_ssac_barring_check_pass(mmtel_type);

}
#endif

#ifdef FEATURE_MMODE_SC_SVLTE
/*===========================================================================

FUNCTION cmcall_is_route_hybr2_volte_call_to_1x

DESCRIPTION

Check if VoLTE call can be routed to 1x, when LTE service is not present and 1x is present (SV config).


DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the call attempt is allowed.
  FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean  cmcall_is_route_hybr2_volte_call_to_1x(
  cmcall_s_type  *call_ptr
)
{
  cm_service_status_s_type srv_info;
  boolean is_lte_srv = FALSE, is_1x_srv = FALSE;

  if (cmph_is_subs_feature_mode_1x_sxlte(call_ptr->asubs_id) && 
      call_ptr->ss == CM_SS_HYBR_2 && call_ptr->is_volte)
  {
    cmss_get_service_status_per_stack(CM_SS_HYBR_2, &srv_info);

    if (srv_info.sys_mode == SYS_SYS_MODE_LTE &&
		srv_info.srv_status == SYS_SRV_STATUS_SRV)
    {
	  is_lte_srv = TRUE;
	}

	cmss_get_service_status_per_stack(CM_SS_MAIN, &srv_info);

	if (srv_info.sys_mode == SYS_SYS_MODE_CDMA &&
		srv_info.srv_status == SYS_SRV_STATUS_SRV)
    {
	  is_1x_srv = TRUE;
	}

	return (!is_lte_srv && is_1x_srv);
  }

  return FALSE;
}
#endif

/*===========================================================================

FUNCTION cmcall_is_active_call_other_than_ss

DESCRIPTION
  Checks is there is a requested call type. active on ss other than passed

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_active_call_other_than_ss
(
  cm_ss_e_type        ss,

  cm_call_type_e_type call_type

)
/*lint -esym(715, ss) */
{

  #ifdef FEATURE_MMODE_DUAL_SIM

   cmcall_s_type   *call_ptr;                        /* Call object pointer */
   cm_iterator_type call_itr;
       /* call Object Iterator */
 
   /*-----------------------------------------------------------------------*/
 
   cmcall_obj_iterator_init(&call_itr);
 
   call_ptr = cmcall_obj_get_next(&call_itr);
 
   while(call_ptr != NULL)
   {
       if (
            #ifdef FEATURE_MMODE_DUAL_SIM
            (call_ptr->ss != ss ) &&
            #endif
            call_ptr->call_type == call_type
          )
       {
         return call_ptr->call_id;
       } /* if ( call_ptr->call_type == call_type ) */
 
       call_ptr = cmcall_obj_get_next(&call_itr);
   } /* while */
   #endif
   return CM_CALL_ID_INVALID;

} /* cmcall_is_there_a_call_type() */

#ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
/*===========================================================================

FUNCTION cmcall_ssac_process_plmn_change

DESCRIPTION
  Update the ssac params.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void  cmcall_ssac_process_plmn_cell_change (
  const lte_rrc_camped_svc_info_s *svc_info_ptr
)
{
  lte_rrc_plmn_s  old_plmn;
  boolean is_changed = FALSE;
  int i;
  cmcall_ssac_s_type  *cmcall_ssac_info = cmcall_ssac_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (svc_info_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if plmn is changed */
  old_plmn = cmcall_ssac_info->plmn;

  if (old_plmn.num_mnc_digits != svc_info_ptr->selected_plmn.num_mnc_digits)
  {
    is_changed = TRUE;
  }
  else
  {
    for (i = 0; i < 3; i ++)
    {
      if (old_plmn.mcc[i] != svc_info_ptr->selected_plmn.mcc[i])
      {
        is_changed = TRUE;
        break;
      }
      if (old_plmn.mnc[i] != svc_info_ptr->selected_plmn.mnc[i])
      {
        is_changed = TRUE;
        break;
      }
    }
  }

  if(cmcall_ssac_info->cell_id != svc_info_ptr->cell_identity)
  {
    is_changed = TRUE;
  }
  /* Reset backoff timers */
  if (is_changed)
  {
    CM_MSG_HIGH_1( "plmn changed, Tvolte_hys=%d",cmsds_ptr()->tssac_hys);
    cmcall_reset_ssac_timer(SSAC_MMTEL_TYPE_VOICE_VIDEO,
                            SSAC_TIMER_ACTION_CANCEL);

    // Store the new plmn
    cmcall_ssac_info->plmn = svc_info_ptr->selected_plmn;
    cmcall_ssac_info->cell_id = svc_info_ptr->cell_identity;
    if(cmsds_ptr()->tssac_hys != 0)
    {
      /* update t_ssac to current uptime if PLMN change */
      cmsds_ptr()->tssac_hys = time_get_uptime_secs();
    }
  }

}
#endif

#if (defined( FEATURE_UMTS_1X_HANDOVER_1XMSM ))
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_MSM1X */


/*===========================================================================

FUNCTION sr_is_more_redial

DESCRIPTION
  Check whether more silent redials are left.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if more silent redials are left. FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean sr_is_more_redial(

    cm_sr_s_type  *sr_ptr,
        /* pointer to silent redial object
        ** which to check for more redials */

    dword         uptime
        /* The current uptime */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If more silent redial attempts left and we are not over the silent
  ** redial max time, return TRUE. Else return FALSE.
  */
  if( sr_ptr->sr_left > 1 && uptime <= sr_ptr->sr_max_uptime )
  {
    return TRUE;
  }
  #ifdef FEATURE_IP_CALL
  else if((sr_ptr->sr_left > 1) && (sr_ptr->ignore_sr_max_uptime))
  {
    CM_MSG_HIGH_0("Due to ignore_sr_max_uptime, allow SR");
    sr_ptr->ignore_sr_max_uptime = FALSE;
    return TRUE;
  }
  #endif

  CM_MSG_HIGH_3("sr_is_more_redial SR expired, sr_left %d, uptime %d, sr_max_uptime %d",
    sr_ptr->sr_left, uptime, sr_ptr->sr_max_uptime);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;
} /* sr_is_more_redial() */



/*===========================================================================

FUNCTION sr_sched_next

DESCRIPTION
  Schedule next silent redial.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void sr_sched_next(


    cm_sr_s_type           *sr_ptr,
        /* Pointer to silent redial object */

    dword                   sr_period,
        /* Silent redial period (i.e. when next redial attempt should take
        ** place */

    dword                   hold_orig_period,
        /* Hold orig period */

    cm_sr_act_e_type        sr_act,
        /* Indicate whether the silent redial sequence should be restarted,
        ** decremented or left as is.
        */
    cm_call_end_e_type      end_status,
        /* the call end status */

    cmcall_act_e_type       redial_act
        /* The redial action */

)
/*lint -esym(715, redial_act) */
/*lint -esym(715, end_status) */
{
  dword                    uptime               = time_get_uptime_secs();
    /* get the current uptime */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Schedule the next silent redial uptime.
  */
  sr_ptr->next_orig_uptime = uptime + sr_period;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If hold orig != 0, set the hold-orig uptime.
  */
  if( hold_orig_period != 0 )
  {
    sr_ptr->hold_orig_uptime = MAX( sr_ptr->hold_orig_uptime,
                                    uptime + hold_orig_period );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the silent redial sequence should be restarted, reset the number of
  ** silent redial left. Else, decrement the number of silent redial left.
  */
  switch ( sr_act )
  {
    case CM_SR_ACT_RESTART:
      sr_ptr->sr_left = sr_ptr->orig_plan_ptr->sr_max_dial;
      break;

    case CM_SR_ACT_DEC:
      (void)(CM_DEC( sr_ptr->sr_left ));
      break;

    case CM_SR_ACT_NO_CHANGE:
      break;

    case CM_SR_ACT_NONE:
    case CM_SR_ACT_MAX:
    default:
      sys_err_fatal_invalid_value_exception((uint8) sr_act);

  } /* switch ( sr_act ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Reset is_ok_to_orig and orig_sent_on_mode to FALSE.
  */
  sr_ptr->is_ok_to_orig   = FALSE;

  sr_ptr->orig_sent_on_mode = SYS_SYS_MODE_NONE;

  #ifdef FEATURE_HDR_HYBRID
  sr_ptr->hdr_is_ok_to_orig = FALSE;
  #endif

  if ( redial_act != CMCALL_ACT_END_ORIG )
  {
    cmlog_call_orig_redial( end_status,
                            redial_act,
                            sr_ptr->sr_left,
                            sr_ptr->orig_plan_ptr->sr_max_dial);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_5( "SRFailed EndStatus=%d Redial act %d SRLeft=%d, Next SR orig uptime=%ld Num of SR Attempts=%d",
                end_status, redial_act, sr_ptr->sr_left,
               MAX(sr_ptr->next_orig_uptime, sr_ptr->hold_orig_uptime),
               (sr_ptr->orig_plan_ptr->sr_max_dial - sr_ptr->sr_left) );

} /* sr_sched_next() */
/*lint +esym(715, redial_act) */
/*lint +esym(715, end_status) */

/*===========================================================================

FUNCTION cmcall_check_if_no_srv
DESCRIPTION
  Function to check if we both the HDR and the main instance have no service

DEPENDENCIES

RETURN VALUE
  True  If both HDR and 1X do not have service
  False Otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_check_if_no_srv( boolean check_hdr )
{

  cmss_s_type *ss_ptr = cmss_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( ss_ptr   != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if defined(FEATURE_HDR)
  if (ss_ptr->info.hdr_hybrid && check_hdr )
  {
    return ( ( ss_ptr->main_srv_status == SYS_SRV_STATUS_NO_SRV ||
               ss_ptr->main_srv_status == SYS_SRV_STATUS_PWR_SAVE ) &&
             ( ss_ptr->hybr_1_srv_status == SYS_SRV_STATUS_NO_SRV ||
               ss_ptr->hybr_1_srv_status == SYS_SRV_STATUS_PWR_SAVE ) );
  }
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return( ss_ptr->main_srv_status == SYS_SRV_STATUS_NO_SRV ||
          ss_ptr->main_srv_status == SYS_SRV_STATUS_PWR_SAVE );

} /* cmcall_check_if_no_srv */

/*===========================================================================

FUNCTION cmcall_misc_is_mode_pref

DESCRIPTION

Check whether specified fav_mode_pref_1 complies with fav_mode_pref_2

DEPENDENCIES

   None

RETURN VALUE

  TRUE if so,
  FALSE otherwise and when any of mapping of CM enum to SD enum failed

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_misc_is_mode_pref(

   cm_mode_pref_e_type fav_mode_pref_1,

   cm_mode_pref_e_type fav_mode_pref_2

)
{

  sd_ss_mode_pref_e_type sd_mode_pref_1 = SD_SS_MODE_PREF_NONE;
  sd_ss_mode_pref_e_type sd_mode_pref_2 = SD_SS_MODE_PREF_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  
  CM_ASSERT( fav_mode_pref_1 < CM_MODE_PREF_MAX );
  CM_ASSERT( fav_mode_pref_2 < CM_MODE_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ----- - - - - -*/

  sd_mode_pref_1 = cmph_map_cm_mode_pref_to_sd_mode_pref(fav_mode_pref_1);

  if( sd_mode_pref_1 == SD_SS_MODE_PREF_MAX )
  {
    CM_ERR_1("Mapping failed for %d! mode pref!", fav_mode_pref_1 );
    return FALSE;
  }

  sd_mode_pref_2 = cmph_map_cm_mode_pref_to_sd_mode_pref(fav_mode_pref_2);

  if( sd_mode_pref_2 == SD_SS_MODE_PREF_MAX )
  {
    CM_ERR_1("Mapping failed for %d! mode pref!", fav_mode_pref_2 );
    return FALSE;
  }

  return sd_misc_is_mode_pref( sd_mode_pref_1,
                               sd_mode_pref_2 ) ;
}




/*===========================================================================
FUNCTION cmcall_misc_get_common_mode_pref

DESCRIPTION

  Gets the common SD mode preference component(s) from given CM mode preference.

  Example:

  The below will add all single mode pref components from
  CM_MODE_PREF_AUTOMATIC and end up with CM_MODE_PREF_NONE.

  cm_mode_pref = cmcall_misc_get_common_mode_pref(
                                           CM_MODE_PREF_AUTOMATIC,
                                           3,// because 6 components follow
                                           CM_MODE_PREF_CDMA_ONLY,
                                           CM_MODE_PREF_CDMA_HDR_ONLY,
                                           CM_MODE_PREF_DIGITAL_ONLY,
                                           );

DEPENDENCIES
  sd_misc_get_common_mode_pref()

RETURN VALUE
  Return new preference or CM_MODE_PREF_MAX if any of the mapping
  functions failed.

SIDE EFFECTS
  None

===========================================================================*/
cm_mode_pref_e_type cmcall_misc_get_common_mode_pref
(
  cm_mode_pref_e_type cm_mode_pref,
    /* CM mode pref
    */

  int number_of_components,
  ...
)
{

  va_list arg_list;
  sd_ss_mode_pref_e_type sd_mode_pref;
  sd_ss_mode_pref_e_type sd_mode_pref_common = SD_SS_MODE_PREF_NONE;
  cm_mode_pref_e_type    cm_mode_pref_common = CM_MODE_PREF_NONE;

  /*
  ** First map given CM mode pref to SD mode pref
  */
  sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(cm_mode_pref);
  if( sd_mode_pref == SD_SS_MODE_PREF_NONE)
  {
    return CM_MODE_PREF_NONE;
  }


  /*
  ** Initialize arg_list and make an internal pointer point
  ** at the first variable function argument.
  ** This will be the second parameter
  */
  /*lint -e{64} -e{718} -e{530} -e{516} */
  va_start(arg_list, number_of_components);

  /*
  ** Process second and subsequent function arguments.
  ** These are the SD components to be removed from given
  ** CM mode pref
  */

  while(number_of_components--)
  {

    /*lint -e{10} -e{662} -e{826} -e{718} -e{124}*/
    cm_mode_pref_common = (cm_mode_pref_e_type)va_arg(arg_list, int);

    if(!BETWEEN( cm_mode_pref_common,
                 CM_MODE_PREF_NONE,
                 CM_MODE_PREF_MAX))
    {
      CM_ERR_1( "Bad cm_mode_pref = %d",cm_mode_pref_common);
      CM_ASSERT(FALSE);
    }

    sd_mode_pref_common = cmph_map_cm_mode_pref_to_sd_mode_pref(cm_mode_pref_common);

    if( sd_mode_pref_common == SD_SS_MODE_PREF_NONE)
    {
      return CM_MODE_PREF_NONE;
    }

    /* Remove it */
    sd_mode_pref = sd_misc_get_common_mode_pref( sd_mode_pref,
                                                 sd_mode_pref_common );

  }

  /* Clean up */
  va_end(arg_list);

  /* Now map result back to CM mode pref enum value */
  cm_mode_pref = cmph_map_sd_mode_pref_to_cm_mode_pref(sd_mode_pref);

  return cm_mode_pref;
}

/*===========================================================================

FUNCTION cmcall_intersect_or_force_mode_pref

DESCRIPTION
  Computes the intersection of the mode preferences for call origination.

  This intersection is forced upon the phone as the new mode_preference for
  call origination.

  In case there is no common mode portion in the 2 mode preferences, the mode
  preference derived from service type is forced upon the phone based on
  is_force_allowed flag.

DEPENDENCIES


RETURN VALUE
Returns the mode preference

SIDE EFFECTS
  none

===========================================================================*/

static cm_mode_pref_e_type cmcall_misc_intersect_or_force_mode_pref(

  cm_srv_type_e_type srv_type,
    /* The srv type requested
    */

       cm_mode_pref_e_type  mode_pref,
       /* The current mode preference
       */

       boolean              is_force_allowed
        /* Flag to indicate if call preferences should be forced upon the
        ** in the event of intersection with phone preferences being NONE.
        */
)
{

  cm_mode_pref_e_type requested_mode_pref = CM_MODE_PREF_NONE;
  cm_mode_pref_e_type intersect_mode_pref = CM_MODE_PREF_NONE;

  switch( srv_type )
  {
    case CM_SRV_TYPE_HDR:
    case CM_SRV_TYPE_HDR_MORE_PREF:

      requested_mode_pref = CM_MODE_PREF_HDR_ONLY;
      break;

    case CM_SRV_TYPE_CDMA_SPECIFIC:
    case CM_SRV_TYPE_CDMA_AUTOMATIC:

      requested_mode_pref = CM_MODE_PREF_CDMA_ONLY;
      break;

    case CM_SRV_TYPE_CDMA_HDR:

      requested_mode_pref = CM_MODE_PREF_CDMA_HDR_ONLY;
      break;


    case CM_SRV_TYPE_GSM_WCDMA:

      requested_mode_pref = CM_MODE_PREF_GSM_WCDMA_ONLY;
      break;

    case CM_SRV_TYPE_WCDMA:

      requested_mode_pref = CM_MODE_PREF_WCDMA_ONLY;
      break;

    case CM_SRV_TYPE_GSM:

      requested_mode_pref = CM_MODE_PREF_GSM_ONLY;
      break;

    case CM_SRV_TYPE_LTE:

      requested_mode_pref = CM_MODE_PREF_LTE_ONLY;
      break;

    case CM_SRV_TYPE_GSM_WCDMA_LTE:

      requested_mode_pref = CM_MODE_PREF_GWL;
      break;

    case CM_SRV_TYPE_TDS:

      requested_mode_pref = CM_MODE_PREF_TDS_ONLY;
      break;

    case CM_SRV_TYPE_TDS_GSM:

      requested_mode_pref = CM_MODE_PREF_TDS_GSM;
      break;

    case CM_SRV_TYPE_TDS_GSM_LTE:

      requested_mode_pref = CM_MODE_PREF_TDS_GSM_LTE;
      break;

    case CM_SRV_TYPE_TDS_LTE:

      requested_mode_pref = CM_MODE_PREF_TDS_LTE;
      break;

    case CM_SRV_TYPE_TDS_GSM_WCDMA_LTE:

      requested_mode_pref = CM_MODE_PREF_TDS_GSM_WCDMA_LTE;
      break;

    case CM_SRV_TYPE_TDS_GSM_WCDMA:

      requested_mode_pref = CM_MODE_PREF_TDS_GSM_WCDMA;
      break;

    case CM_SRV_TYPE_CS_ONLY:
      requested_mode_pref = CM_MODE_PREF_ANY_BUT_HDR_LTE;
      break;

    case CM_SRV_TYPE_PS_ONLY:
      requested_mode_pref = CM_MODE_PREF_HDR_WCDMA_LTE;
      break;

      /* This should be never occur
      */
    default:

      CM_ERR_1( "srv_type %d", (uint8) srv_type);
      /*
      ** 527: Unreachable
      ** This is because CM_ERR() evaluates to error fatal
      ** (i.e exit program)
      ** But for different feature set it may not to, so we want to
      ** supress the warning here
      */
      /*lint -save -e527 */
      requested_mode_pref = mode_pref;
      /*lint -restore */
      break;
  }


  /* Find the intersection and if there is no intersection then
  ** return the requested preference
  */

  if( requested_mode_pref == CM_MODE_PREF_NONE )
  {
    return ( requested_mode_pref );
  }

  intersect_mode_pref = cmcall_misc_get_common_mode_pref(
                                         requested_mode_pref, 1, mode_pref );


  /* If intersection is invalid, send the call preference if the flag
  ** is_force_allowed is set. If not it implies that the phone preferences
  ** should not be overwritten completely.
  */
  if( intersect_mode_pref == CM_MODE_PREF_NONE ||
      intersect_mode_pref >= CM_MODE_PREF_MAX )
  {
    if( is_force_allowed )
    {
      return ( requested_mode_pref );
    }
    else
    {
      return CM_MODE_PREF_NONE;
    }
  }

  /* If intersection is valid, send it back
  */
  return ( intersect_mode_pref );


} /* cmcall_misc_intersect_or_force_mode_pref */



/*===========================================================================

FUNCTION cmcall_copy_ss_info

DESCRIPTION
  Copies the current serving system info to the call object

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

static void cmcall_copy_ss_info(

   cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{
  cmss_s_type *ss_ptr      = cmss_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ss_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_ptr->prev_srv_status[CM_SS_MAIN] = ss_ptr->info.srv_status;
  
  #if defined(FEATURE_HDR)
  if (ss_ptr->info.hdr_hybrid )
  {
    call_ptr->prev_srv_status[CM_SS_HDR] =  ss_ptr->info.hdr_srv_status;
  }
  #endif

  #ifdef FEATURE_MMODE_DUAL_SIM
  call_ptr->prev_srv_status[CM_SS_HYBR_2] = ss_ptr->info.gw_srv_status;
  #endif

  #ifdef FEATURE_MMODE_HYBR3_ENABLED
  call_ptr->prev_srv_status[CM_SS_HYBR_3] = ss_ptr->info.gw3_srv_status;
  #endif

  return;

}/* cmcall_copy_ss_info */


#ifdef FEATURE_HDR_HYBRID

/*===========================================================================

FUNCTION cmcall_is_redial_on_1x
DESCRIPTION
  Function to check if we need to switch to 1x for redialing

DEPENDENCIES

RETURN VALUE
  True  If we need to switch to 1x
  False Otherwise

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmcall_is_redial_on_1x(
  const cmcall_s_type    *call_ptr
        /* pointer to a call object
        */
)
{
  cmss_s_type *ss_ptr = cmss_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( ss_ptr   != NULL );
  CM_ASSERT( call_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if (defined FEATURE_CM_LTE) && (defined FEATURE_HDR_HANDOFF)
  /* If shorten bsr timer is running which means we are trying force LTE scan,
  ** don't allow redial on 1X. otherwise LTE scan might be overriden.
  */
  if(cmph_ptr()->shorten_bsr_timer != 0)
  {
    CM_MSG_HIGH_0("Don't redial over 1x due to shorten BSR timer running");
    return FALSE;
  }
  #endif


  /* Step 1:
  ** If the faliure is origination throttling, return true
  ** If the failure is not colloc acq fail return false or if
  ** we don't have service on 1X return FALSE
  */

  if( call_ptr->end_status    == CM_CALL_END_ORIG_THR
      #ifdef FEATURE_CALL_THROTTLE
        && call_ptr->sr.is_call_throttled
      #endif
      )
  {
    return TRUE;
  }

  if( call_ptr->end_status    != CM_CALL_END_CM_COLLOC_ACQ_FAIL )
  {
    return FALSE;
  }

  if( ss_ptr->info.srv_status == SYS_SRV_STATUS_NO_SRV          ||
      ss_ptr->info.srv_status == SYS_SRV_STATUS_PWR_SAVE )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step 2: We have service, check if we immediately need to go to 1x
  **         or stay on HDR for one more ACQ_FAIL
  */

  /* We started with no service, give DO one more chance
  */
  if( call_ptr->prev_srv_status[CM_SS_MAIN] == SYS_SRV_STATUS_NO_SRV   ||
      call_ptr->prev_srv_status[CM_SS_MAIN] == SYS_SRV_STATUS_PWR_SAVE )
  {
    return FALSE;
  }

  /* For all other cases, return TRUE
  */
  return TRUE;
}/* cmcall_is_redial_on_1x */


/*===========================================================================

FUNCTION cmcall_hybr_redial

DESCRIPTION
  select either CDMA or HDR for the next origination

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

  This function is expected to be called during the origination process
  before call is connected.

RETURN VALUE
  A ballpark estimation for the number of seconds the phone is going
  to be without service in the passed pointer

  True if the call is ended because call parameters were not found in the
  origination Queue

  False otherwise

SIDE EFFECTS
  none

===========================================================================*/
static boolean  cmcall_hybr_redial(

  cmcall_s_type    *call_ptr,
        /* pointer to a call object
        */
  cmcall_act_e_type redial_act,
        /* The redial action
        */
  dword            *no_srv_time
       /* No service time
       */
)
{
  cmph_s_type           *ph_ptr         = cmph_ptr();
      /* Point at phone object
      */

  cm_sr_s_type          *sr_ptr         = NULL;
    /* point at silent redial component */

  sd_ss_pref_reas_e_type pref_reas             = SD_SS_PREF_REAS_REDIAL_NORM;
  dword                  sr_period             = 0;
  dword                  sr_off_period         = 1;
  cmss_s_type           *ss_ptr                = cmss_ptr();
  cm_ss_e_type           prev_ss;
  cm_orig_q_s_type      *ph_orig_para_top_ptr;

  cm_orig_q_s_type      *computed_orig_para    =  NULL;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  
  CM_ASSERT( ss_ptr      != NULL );
  CM_ASSERT( call_ptr    != NULL );
  CM_ASSERT( no_srv_time != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ----- - - - - -*/

  /* point at silent redial component */
  sr_ptr                 = &call_ptr->sr;
  prev_ss                = call_ptr->ss;
  computed_orig_para    =  cmtask_orig_para_search_act_id(
                                                         prev_ss,
                                        (cm_act_id_type) call_ptr->call_id );

  /* If computed orig para is NULL,
  ** Either the origination was never forced, and so we don't have it in
  ** the queue, or this is genuine error condition.
  ** If the preferences were not forced, then force the call to 1X
  */

  if( computed_orig_para == NULL && call_ptr->orig_mode != SD_SS_ORIG_MODE_NONE )
  {
    CM_MSG_HIGH_1(" Unable to get orig param, call id: %d",call_ptr->call_id);
    cmcall_end(call_ptr);
    return( TRUE );
  }

 /*
  ** If we keep failing origination on HDR and CDMA origination is possible,
  ** we'll attempt to switch origination attempts to 1X here.
  ** CDMA origination is possible iff favored_mode_pref has CDMA component in it
  */
  if(call_ptr->ss == CM_SS_HDR)
  {
    if( cmcall_misc_is_mode_pref( call_ptr->favored_mode_pref,
                                  CM_MODE_PREF_CDMA_ONLY )
        &&
        (
          ( ( sr_ptr->orig_plan_ptr->sr_max_dial - (sr_ptr->sr_left-1 ) ) >=
                                                CMCALL_SR_HYBR_MAX_HDR_DIALS
          ) ||
          ( sr_ptr->is_hdr_not_avail == TRUE )
            ||
          cmcall_is_redial_on_1x( call_ptr )
        )
      )
    {
      /*
      ** Use CM_SS_MAIN after CMCALL_SR_HYBR_MAX_HDR_DIALS orig attempts on HDR
      ** or if HDR became not available.
      */
      call_ptr->ss                  = CM_SS_MAIN;
      call_ptr->favored_mode_pref   = CM_MODE_PREF_CDMA_ONLY;
      pref_reas                     = SD_SS_PREF_REAS_ORIG_START_PS;
      call_ptr->sr.is_hybr_redialed = TRUE;
      #ifdef FEATURE_CALL_THROTTLE
      call_ptr->sr.is_call_throttled = FALSE;
      #endif
      /*
      ** If we are redialling on 1x, no need to hold back on sending
      ** origination_f upon ok_to_orig.
      */
      sr_off_period = 0;

      #ifdef FEATURE_DEDICATED_C2K_MODE
      if( ss_ptr->is_hdr_session_open )
      {
        CM_MSG_HIGH_0("DCM: Closing HDR session");
        cmph_send_hdrmc_session_close_cmd();
        call_ptr->waiting_for_hdr_session_close = TRUE;
      }
      #endif

      /* Update the DDTM status
      */
      cmph_update_ddtm_status(ph_ptr);
    }
    else if( call_ptr->end_status    == CM_CALL_END_ORIG_THR )
    {
      #ifdef FEATURE_CALL_THROTTLE
      call_ptr->sr.is_call_throttled = TRUE;
      #endif
    }
  }

  if( redial_act == CMCALL_ACT_REDIAL )
  {
    sr_off_period = call_ptr->sr.orig_plan_ptr->sr_period_time;
    sr_period     = 0;
  }
  else if( redial_act == CMCALL_ACT_REDIAL_HDR_THR && call_ptr->ss == CM_SS_HDR )
  {
    sr_period     = CMCALL_ACC_HOLD_ORIG_TIME;
    sr_off_period = 0;
  }
  else
  {
    sr_period = call_ptr->sr.orig_plan_ptr->sr_period_time; /* Hold orig period */
  }

  cmcall_copy_ss_info( call_ptr );

  /* Dec SR left only if we have 1x ser */

  if(  ss_ptr->info.srv_status == SYS_SRV_STATUS_NO_SRV ||
       ss_ptr->info.srv_status == SYS_SRV_STATUS_PWR_SAVE )
  {
    /* Schedule the next SR.
    */
    sr_sched_next( &call_ptr->sr,
                   sr_off_period,           /* SR off period */
                   sr_period,               /* SR hold period */
                   CM_SR_ACT_NO_CHANGE,
                   call_ptr->end_status,
                   redial_act
                 );
  }
  else
  {
    /* Schedule the next SR.
    */
    sr_sched_next( &call_ptr->sr,
                   sr_off_period,           /* SR off period */
                   sr_period,               /* SR hold period */
                   CM_SR_ACT_DEC,
                   call_ptr->end_status,
                   redial_act
                 );
  }/* else */

  /*- - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_ptr->end_status = CM_CALL_END_NONE;

  CM_MSG_HIGH_1( "Using ss=%d for HybrRedial",call_ptr->ss);

  /* Step1: Get the parameter at the top of the queue
  ** Step2: If the top is not NULL and top's priority is higher than current
  **        Activity, then use the top's parameters
  ** Step3: Else use the computed orig parameters
  **        To get the computed orig parameters, use the old ss and the orig_q
  */

  if( call_ptr->ss != prev_ss )
  {
    ph_orig_para_top_ptr = cmtask_orig_para_get_top( call_ptr->ss );
    if( ph_orig_para_top_ptr != NULL &&
        (
          cmph_cmp_act_priorities(
                                   ph_orig_para_top_ptr->orig->act_priority,
                                   cmph_get_act_priority(
                                     cmph_map_call_type_to_act_type(
                                       call_ptr->call_type      )
                                                        )
                                 ) > 0

        )
      )
    {

      *no_srv_time =  cmph_force_pref_on_the_fly
                (
                 call_ptr->ss,
                 ph_ptr,
                 pref_reas,
                 cmph_map_call_type_to_act_type( call_ptr->call_type ),
                 CM_PREF_TERM_CM_1_CALL_PLUS,
                 call_ptr->favored_mode_pref,
                 ph_orig_para_top_ptr->orig->orig_band_pref,
                 ph_orig_para_top_ptr->orig->orig_lte_band_pref,
                 ph_orig_para_top_ptr->orig->orig_tds_band_pref,
                 ph_orig_para_top_ptr->orig->orig_prl_pref,
                 ph_orig_para_top_ptr->orig->orig_roam_pref,
                 ph_orig_para_top_ptr->orig->orig_hybr_pref,
                 ph_ptr->main_stack_info.pref_info.plmn,
                 ph_orig_para_top_ptr->orig->orig_srv_domain_pref,
                 call_ptr->cmcall_mode_info.info.cdma_info.otasp_info.otasp_act_code,
                 ph_ptr->main_stack_info.pref_info.acq_order_pref,
                 CM_NETWORK_SEL_MODE_PREF_NONE,
                 (cm_act_id_type) call_ptr->call_id,
                 CM_ACT_UPDATE_REAS_ACT_CONT,
                 call_ptr->force_top_pref,
                 cmph_get_acq_pri_order_per_sub(call_ptr->ss),
                 CM_DEFAULT_VALUE,
                 SYS_CSG_ID_INVALID,
                 SYS_RAT_NONE,
                 ph_orig_para_top_ptr->orig->ciot_lte_pref
               );
      return( FALSE );
    } /*  if( ph_orig_para_top_ptr != NULL ... */

  } /*  if( call_ptr->ss != prev_ss ) */


  if( computed_orig_para == NULL )
  {
    /* We are forcing the preferences for the first time
    */

    *no_srv_time = cmcall_force_pref_on_the_fly
              ( call_ptr,
                (call_ptr->call_type == CM_CALL_TYPE_PS_DATA) ?
                                SD_SS_PREF_REAS_ORIG_START_PS :
                                SD_SS_PREF_REAS_ORIG_START_CS
              );

    return (FALSE);

  }

  *no_srv_time =  cmph_force_pref_on_the_fly
               (
                 call_ptr->ss,
                 ph_ptr,
                 pref_reas,
                 cmph_map_call_type_to_act_type( call_ptr->call_type ),
                 CM_PREF_TERM_CM_1_CALL_PLUS,
                 call_ptr->favored_mode_pref,
                 computed_orig_para->orig->orig_band_pref,
                 computed_orig_para->orig->orig_lte_band_pref,
                 computed_orig_para->orig->orig_tds_band_pref,
                 computed_orig_para->orig->orig_prl_pref,
                 computed_orig_para->orig->orig_roam_pref,
                 computed_orig_para->orig->orig_hybr_pref,
                 ph_ptr->main_stack_info.pref_info.plmn,
                 computed_orig_para->orig->orig_srv_domain_pref,
                 call_ptr->cmcall_mode_info.info.cdma_info.otasp_info.otasp_act_code,
                 ph_ptr->main_stack_info.pref_info.acq_order_pref,
                 CM_NETWORK_SEL_MODE_PREF_NONE,
                 (cm_act_id_type) call_ptr->call_id,
                 CM_ACT_UPDATE_REAS_ACT_CONT,
                 call_ptr->force_top_pref,
                 cmph_get_acq_pri_order_per_sub(call_ptr->ss),
                 CM_DEFAULT_VALUE,
                 SYS_CSG_ID_INVALID,
                 SYS_RAT_NONE,
                 computed_orig_para->orig->ciot_lte_pref
               );

  return( FALSE );
} /* cmcall_hybr_redial */

#endif /* FEATURE_HDR_HYBRID */



/*===========================================================================

FUNCTION cmcall_is_ok_to_end_hold_orig_period

DESCRIPTION
  Determine whetehr it is ok to end hold orig period for call orig

DEPENDENCIES


RETURN VALUE
  Boolean

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmcall_is_ok_to_end_hold_orig_period
(
  const cmcall_s_type    *call_ptr
)
/*lint -esym(715, call_ptr) */

{
  cmss_s_type   *ss_ptr = cmss_ptr();

  boolean       status = FALSE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  #if defined (FEATURE_HDR_HYBRID)
  if( call_ptr->ss == CM_SS_HYBR_1 )
  {

      if( (ss_ptr->info.hdr_srv_status == SYS_SRV_STATUS_SRV &&
            !ss_ptr->hdr_is_pref_srv_acq) ||
            /* Check for faked no service due to not usable system.
            */
            (ss_ptr->info.hdr_srv_status == SYS_SRV_STATUS_NO_SRV &&
             ss_ptr->hybr_hdr_usable == SD_SYS_USABLE_NOT_OK) ||
             ss_ptr->info.hdr_srv_status == SYS_SRV_STATUS_PWR_SAVE
          )
      {
        status = TRUE;
      }


  }
  else
  #endif
  #if defined FEATURE_MMODE_DUAL_SIM
  if( call_ptr->ss == CM_SS_HYBR_2 )
  {
    if( (sys_srv_status_is_srv(ss_ptr->info.gw_srv_status) &&
         !ss_ptr->hybr_gw_is_pref_srv_acq ) ||
          /* Check for faked no service due to not usable system.
          */
          (ss_ptr->info.gw_srv_status == SYS_SRV_STATUS_NO_SRV &&
           ss_ptr->hybr_gw_usable == SD_SYS_USABLE_NOT_OK) ||
          (ss_ptr->info.gw_srv_status == SYS_SRV_STATUS_PWR_SAVE) ||
          /* Satisfying GCF 26.7.4.3.4 to end hold orig period for
          ** call during camped indication. This solves only
          ** power up acquisiton cases, need to think about
          ** camped indication in between full service.
          */
          (ss_ptr->info.gw_srv_domain == SYS_SRV_DOMAIN_CAMPED &&
           ss_ptr->hybr_gw_is_pref_srv_acq)
      )
    {
      status = TRUE;
    }
  }
  else
  #ifdef FEATURE_MMODE_HYBR3_ENABLED
  if( call_ptr->ss == CM_SS_HYBR_3 )
  {
    if( (sys_srv_status_is_srv(ss_ptr->info.gw3_srv_status) &&
         !ss_ptr->hybr_3_gw_is_pref_srv_acq ) ||
          /* Check for faked no service due to not usable system.
          */
          (ss_ptr->info.gw3_srv_status == SYS_SRV_STATUS_NO_SRV &&
           ss_ptr->hybr_3_gw_usable == SD_SYS_USABLE_NOT_OK) ||
          (ss_ptr->info.gw3_srv_status == SYS_SRV_STATUS_PWR_SAVE) ||
          /* Satisfying GCF 26.7.4.3.4 to end hold orig period for
          ** call during camped indication. This solves only
          ** power up acquisiton cases, need to think about
          ** camped indication in between full service.
          */
          (ss_ptr->info.gw3_srv_domain == SYS_SRV_DOMAIN_CAMPED &&
           ss_ptr->hybr_3_gw_is_pref_srv_acq)
      )
    {
      status = TRUE;
    }
  }
  else
  #endif
  #endif
  {
    if( (sys_srv_status_is_srv(ss_ptr->main_srv_status) &&
        !ss_ptr->is_pref_srv_acq ) ||
        /* Check for faked no service due to not usable system.
        */
        (ss_ptr->info.srv_status == SYS_SRV_STATUS_NO_SRV /*&&
         ss_ptr->ss_main_usable == SD_SYS_USABLE_NOT_OK*/) ||
        (ss_ptr->info.srv_status == SYS_SRV_STATUS_PWR_SAVE) ||
        /* Satisfying GCF 26.7.4.3.4 to end hold orig period for
        ** call during camped indication. This solves only
        ** power up acquisiton cases, need to think about
        ** camped indication in between full service.
        */
        (ss_ptr->info.srv_domain == SYS_SRV_DOMAIN_CAMPED &&
         ss_ptr->is_pref_srv_acq)
      )
    {
      status = TRUE;
    }
  }

  return status;
} /* cmcall_is_ok_to_end_hold_orig_period() */
/*lint +esym(715, call_ptr) */




/*===========================================================================

FUNCTION cmcall_is_ok_to_orig

DESCRIPTION
  Determine whetehr it is ok to orig

DEPENDENCIES


RETURN VALUE
  Boolean

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmcall_is_ok_to_orig
(
  const cmcall_s_type   *call_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #if (defined (FEATURE_HDR_HYBRID))
  if(call_ptr->ss == CM_SS_HDR)
  {
    return call_ptr->sr.hdr_is_ok_to_orig;
  }
  else
  #endif  /* FEATURE_HDR_HYBRID */
  {
    return call_ptr->sr.is_ok_to_orig;
  }
} /* cmcall_is_ok_to_orig() */

#if defined(CM_GW_SUPPORTED) || defined(FEATURE_IP_CALL)

/*===========================================================================

FUNCTION cmcall_update_ok_to_orig

DESCRIPTION
  Updates the ok to orig value to given value

DEPENDENCIES


RETURN VALUE
  Boolean

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_update_ok_to_orig
(
   cmcall_s_type   *call_ptr,
   /* Call pointer */

   boolean ok_to_orig_value

)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(call_ptr->ss == CM_SS_HDR)
  {
     call_ptr->sr.hdr_is_ok_to_orig = ok_to_orig_value;
  }
  else

  {
     call_ptr->sr.is_ok_to_orig = ok_to_orig_value;
  }

  CM_MSG_HIGH_2("Updating ok_to_orig = %d for ss=%d",\
             ok_to_orig_value, call_ptr->ss );

} /* cmcall_is_ok_to_orig() */

#endif

#if defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)


/*===========================================================================

FUNCTION cmcall_update_orig_mode

DESCRIPTION

  Updates the origmode based on CSFB type

DEPENDENCIES


RETURN VALUE
  NONE

SIDE EFFECTS
  none

===========================================================================*/

void  cmcall_update_orig_mode (cmcall_s_type  *call_ptr)
{

  sd_ss_orig_mode_e_type             orig_mode = call_ptr->orig_mode;
  
  if (call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB)
  {
    /* Not a CSFB call */
    if (call_ptr->orig_mode == SD_SS_ORIG_MODE_NORMAL )
    {
      /* Change the orig mode to refect that it is emergency CSFB call*/
      call_ptr->orig_mode = SD_SS_ORIG_MODE_1XCSFB_NORMAL_ORIG;

    }else if (call_ptr->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG)
    {
      /* Change the orig mode to refect that it is emergency CSFB call*/
      call_ptr->orig_mode = SD_SS_ORIG_MODE_1XCSFB_EMERG_ORIG;
    }
  }
  else if (call_ptr->csfb_type == CM_CALL_CSFB_TYPE_PPCSFB)
  {
    /* Not a CSFB call */
    if (call_ptr->orig_mode == SD_SS_ORIG_MODE_NORMAL )
    {
      /* Change the orig mode to refect that it is emergency CSFB call*/
      call_ptr->orig_mode = SD_SS_ORIG_MODE_PPCSFB_NORMAL_ORIG;

    }else if (call_ptr->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG)
    {
      /* Change the orig mode to refect that it is emergency CSFB call*/
      call_ptr->orig_mode = SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG;
    }
  }

  CM_MSG_HIGH_2(" updating orig_mode, from  %d to %d",orig_mode, call_ptr->orig_mode);

}

#endif


#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

/*===========================================================================

FUNCTION cmcall_is_redir_or_ho

DESCRIPTION
  Determine whether it is ok to orig

DEPENDENCIES


RETURN VALUE
  Boolean

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmcall_is_redir_or_ho
(
  const cmcall_s_type   *call_ptr
)
/*lint -esym(715, call_ptr) */
{
  cmss_s_type   *ss_ptr = cmss_ptr();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #ifdef FEATURE_HDR_HYBRID
  if(call_ptr->ss == CM_SS_HDR)
  {
    return ss_ptr->hdr_is_redir_or_ho;
  }
  else
  #endif  /* FEATURE_HDR_HYBRID */
  {
    return ss_ptr->is_redir_or_ho;
  }
} /* cmcall_is_redir_or_ho() */
/*lint +esym(715, call_ptr) */

/*===========================================================================

FUNCTION  cmcall_end_active_1x_datacalls

DESCRIPTION
 Ends all active data calls on 1X

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to the call obj if CDMA data call present
  NULL otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_end_active_1x_datacalls (boolean is_force)
{
  cmcall_s_type   *call_ptr;
    /* Call object pointer */

  cm_iterator_type call_itr;
    /* call Object Iterator */
  boolean is_ps_call_on_1x = FALSE;
  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);
  /*lint -e655 */
  while(call_ptr != NULL)
  {
    if ((call_ptr->call_type == CM_CALL_TYPE_PS_DATA )&&
              ((call_ptr->sys_mode == SYS_SYS_MODE_CDMA)||
         ((call_ptr->sys_mode == SYS_SYS_MODE_NO_SRV)&&
     (cmph_map_cm_mode_pref_to_sd_mode_pref(call_ptr->favored_mode_pref) & SD_SS_MODE_PREF_CDMA))))
    {
      CM_MSG_HIGH_0("Ending PS_DATA calls on 1X as voice call is originated");
      cmxcall_end_call(call_ptr->call_id,
               CM_CALL_END_CLIENT_END,
               CAI_REL_NORMAL);
      is_ps_call_on_1x = TRUE;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while  */
  /* Remove the objects with priority end and above PH obj */
  if (cmph_remove_orig_mode_with_priority_end(CM_SS_MAIN) == TRUE)
  {
    /* Force the preference to SD */
    if (is_force)
    {
      CM_MSG_HIGH_0( "cmcall:Unforce  call preferences now without 4 sec wait ");
      cmph_force_orig_mode_on_the_fly(CM_SS_MAIN);
    }
  }
  return is_ps_call_on_1x;
  /*lint +e655 */
} /* cmcall_end_active_1x_datacalls () */

/*lint +esym(715, call_ptr) */

/*===========================================================================

FUNCTION  cmcall_end_active_HDR_datacalls

DESCRIPTION
 Ends all active data calls on HDR

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to the call obj if CDMA data call present
  NULL otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean  cmcall_end_active_HDR_datacalls (void)
{
  cmcall_s_type   *call_ptr;
    /* Call object pointer */

  cm_iterator_type call_itr;
    /* call Object Iterator */

  /*-----------------------------------------------------------------------*/
	boolean is_ps_call_on_HDR = FALSE;

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);
  /*lint -e655 */
  while(call_ptr != NULL)
  {
    if ((call_ptr->call_type == CM_CALL_TYPE_PS_DATA )&&
              ((call_ptr->sys_mode == SYS_SYS_MODE_HDR)||
         ((call_ptr->sys_mode == SYS_SYS_MODE_NO_SRV)&&
     (cmph_map_cm_mode_pref_to_sd_mode_pref(call_ptr->favored_mode_pref) & SD_SS_MODE_PREF_HDR))))
    {
      CM_MSG_HIGH_0("Ending PS_DATA calls on HDR as voice call is originated");
      cmxcall_end_call(call_ptr->call_id,
               CM_CALL_END_CLIENT_END,
               CAI_REL_NORMAL);
      is_ps_call_on_HDR = TRUE;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while  */
   return is_ps_call_on_HDR;
} /* cmcall_end_active_HDR_datacalls () */

#endif

/*===========================================================================

FUNCTION cmcall_is_sufficient_srv

DESCRIPTION
  Check whether the specified service is sufficient relatively
  to the specified mode preference in order to originate a call.

  E.g. if mode preference is DIGITAL-ONLY, digital service is sufficient
  to originate a call, whereas analog service is NOT.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if the specified service is sufficient relatively to the specified
  mode preference in order to originate a call. FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(765, cmcall_is_sufficient_srv)
** Can't be static, unit test uses it
*/
boolean cmcall_is_sufficient_srv
(
  const cmcall_s_type *call_ptr,
  cm_mode_pref_e_type mode_pref,
  sys_sys_mode_e_type sys_mode

)
{
  cmss_s_type   *ss_ptr = cmss_ptr();
  /*lint -esym(715, ss_ptr,call_ptr) */

  sd_ss_mode_pref_e_type sd_mode_pref;
  sd_mode_e_type         sd_mode;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref( mode_pref );
  sd_mode = cmph_map_sys_sys_mode_to_sd_mode( sys_mode );

  if(( sd_mode_pref == SD_SS_MODE_PREF_NONE ) ||
     ( sd_mode == SD_MODE_MAX ))
  {
    /* Mapping function failed */
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* We have 3 fields in ss that tell us the current service status.
     ss_ptr->info.srv_status tells whether we have service on MAIN.
     If we do, ss_ptr->sys_mode tell us what that service is (1x/GSM/etc).
     ss_ptr->info.hdr_srv_status tells whether we have service on HDR.
     We don't need corresponding hdr_sys_mode field since having service
     means HDR only.
  */
  #if (defined( FEATURE_HDR ))
  if(call_ptr->ss == CM_SS_HDR)
  {

      if(( ss_ptr->info.hdr_srv_status == SYS_SRV_STATUS_NO_SRV ) ||
         ( ss_ptr->info.hdr_srv_status == SYS_SRV_STATUS_PWR_SAVE ))

      {
        /* The call is to be serviced by HDR system but we have no HDR
           service or HDR is in power save mode */
        return FALSE;
      }
      else
      {
          /* The call is to be serviced by HDR system, we have HDR service,
             gotta only figure out whether it's sufficiant for current mode
             preference */
          sd_mode = SD_MODE_HDR;
      }
    }

  #endif  /* FEATURE_HDR */
 #if defined FEATURE_MMODE_DUAL_SIM
  if( call_ptr->ss == CM_SS_HYBR_2 )
  {
    if( ss_ptr->info.gw_srv_status != SYS_SRV_STATUS_SRV )
    {
      return FALSE;
    }
    else
    {
      sd_mode = SD_MODE_GWL;
    }
  }
  #ifdef FEATURE_MMODE_HYBR3_ENABLED
  if( call_ptr->ss == CM_SS_HYBR_3 )
  {
    if( ss_ptr->info.gw3_srv_status != SYS_SRV_STATUS_SRV )
    {
      return FALSE;
    }
    else
    {
      sd_mode = SD_MODE_GWL;
    }
  }
  #endif
  #endif

  SYS_ARG_NOT_USED( ss_ptr )
  /* Print what we got */
  CM_MSG_HIGH_3( "cm_mode_pref = %d, sys_mode = %d, sd_mode_pref = %d",
                 mode_pref, sys_mode, sd_mode );

  /* This checks whether given SD mode is sufficient for given SD mode pref */
  return ((BM( sd_mode ) & (dword)sd_mode_pref) != 0);
} /* cmcall_is_sufficient_srv() */
/*lint +esym(715, ss_ptr,call_ptr) */

#ifndef FEATURE_MMODE_DISABLE_LTE_VOICE

/*===========================================================================

FUNCTION cmcall_is_call_type_supp_for_ipcall

DESCRIPTION

  Returns TRUE or FALSE depending on if given call type can be originated as
  a ip call. This gives CM control over what call types can get
  originated as ip call.

DEPENDENCIES

   None

RETURN VALUE

  TRUE  - Given call type can be originated as ip call.
  FALSE - Given call type cannot be originated as ip call.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmcall_is_call_type_supp_for_ipcall (

  cm_call_type_e_type   call_type
    /* call_type Voice, CS_DATA */

)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( BETWEEN(call_type, CM_CALL_TYPE_NONE, CM_CALL_TYPE_MAX ));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch (call_type)
  {
    case CM_CALL_TYPE_VOICE:                          return TRUE;

    case CM_CALL_TYPE_VS:                             return TRUE;

    case CM_CALL_TYPE_EMERGENCY:                      return TRUE;

    case CM_CALL_TYPE_VT:                             return TRUE;

    default:                                          return FALSE;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

} /* cmcall_is_call_type_supp_for_ipcall () */

/*===========================================================================

FUNCTION cmcall_is_ipcall_over_mode_poss

DESCRIPTION

  Returns TRUE if given call can be placed on given mode as a IP call.

DEPENDENCIES

   None

RETURN VALUE

  TRUE  -  Call can be placed as a IP call on given mode
  FALSE -  Call cannot be placed as a IP call.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_ipcall_over_mode_poss(

  sys_sys_mode_e_type              sys_mode,
     /* Specific technology - WCDMA */

  const cmcall_s_type              *call_ptr
    /* Call object */
)
{

  sd_ss_mode_pref_e_type   sd_mode_pref         = SD_SS_MODE_PREF_NONE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if given call type is allowed to be originated as a ip call */
  if (!cmcall_is_call_type_supp_for_ipcall (call_ptr->call_type))
  {
    return FALSE;
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if given call type is allowed to be originated as a ip call */
  if (!cmph_is_lte_capable_on_sub(call_ptr->asubs_id) && 
      (sys_mode == SYS_SYS_MODE_WLAN || sys_mode == SYS_SYS_MODE_LTE)
  )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* if voip call state is FALLBACK indicate ip call  not possible */
  if(call_ptr->sr.voip_sr.state == CMCALL_VOIP_SR_STATE_FALLBACK_CS &&
     sys_mode == SYS_SYS_MODE_LTE)
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sd_mode_pref =
        cmph_map_cm_mode_pref_to_sd_mode_pref( call_ptr->favored_mode_pref );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (sys_mode)
  {

    case SYS_SYS_MODE_HDR:
     /* Check made to see if current call can be originated using a VoIP
     ** app on given sys_mode.
     */
     if( sd_misc_is_mode_pref   (sd_mode_pref, SD_SS_MODE_PREF_HDR)  &&
         cmcall_is_active_ip_app(SYS_SYS_MODE_HDR, call_ptr->call_type)
       )
     {
       return TRUE;
     }

     break;

   case SYS_SYS_MODE_WCDMA:
     /* Check made to see if current call can be originated using a VoIP
     ** app on given sys_mode.
     */
     if( sd_misc_is_mode_pref   (sd_mode_pref, SD_SS_MODE_PREF_WCDMA)  &&
         cmcall_is_active_ip_app(SYS_SYS_MODE_WCDMA, call_ptr->call_type)
       )
     {
       if((call_ptr->call_type == CM_CALL_TYPE_EMERGENCY) && (call_ptr->is_em_vt == FALSE))
       {
         CM_MSG_HIGH_0("EM Voice call not possible over IMSonWCDMA");
         return FALSE;
       }
       else
       {
         return TRUE;
       }
     }

     break;


   case SYS_SYS_MODE_LTE:
    /* Check made to see if current call can be originated using a VoIP
    ** app on given sys_mode.
    */
     if( sd_misc_is_mode_pref   (sd_mode_pref, SD_SS_MODE_PREF_LTE)  &&
         cmcall_is_active_ip_app(SYS_SYS_MODE_LTE, call_ptr->call_type)
       )
     {
       return TRUE;
     }
     break;

   case SYS_SYS_MODE_WLAN:

    /* For Multimode sub only, Check made to see if current call can be originated using a VoIP app on given sys_mode. */
     if(cmph_is_lte_capable_on_sub(call_ptr->asubs_id) && 
       cmcall_is_active_ip_app(SYS_SYS_MODE_WLAN, call_ptr->call_type))
     {
       return TRUE;
     }
     break;

   default:

     CM_MSG_HIGH_1 ("ipcall not possible on sys %d", sys_mode);
     break;

  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;
} /* cmcall_is_ipcall_over_mode_poss () */

 
/*===========================================================================

FUNCTION cmcall_is_ipcall_possible

DESCRIPTION

  Checks if given call can be placed as a IP call
  ( over any possible mode)

DEPENDENCIES

   None

RETURN VALUE

  Valid sys mode   -  Call can be placed as a IP call over the returned mode
  SYS_SYS_MODE_NONE -  Call cannot be placed as a IP call.

SIDE EFFECTS
  none

===========================================================================*/
static sys_sys_mode_e_type cmcall_is_ipcall_possible(

   const cmcall_s_type        *call_ptr
     /* Call object */

)
{

  /* IP call is currently possible on only these technologies
  **
  ** - HDR
  ** - WCDMA
  */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);
  CM_ASSERT( BETWEEN(call_ptr->call_type, CM_CALL_TYPE_NONE, CM_CALL_TYPE_MAX ));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check if given call type is allowed to be originated as a ip call */
  if (!cmcall_is_call_type_supp_for_ipcall (call_ptr->call_type))
  {
    return SYS_SYS_MODE_NONE;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Find if there is a voip app on any mode which can support this call
  */
  if (cmcall_is_ipcall_over_mode_poss (SYS_SYS_MODE_LTE, call_ptr) &&
      call_ptr->sr.voip_sr.state != CMCALL_VOIP_SR_STATE_FALLBACK_CS)
  {
    return SYS_SYS_MODE_LTE;
  }
  else if (cmcall_is_ipcall_over_mode_poss (SYS_SYS_MODE_WLAN, call_ptr) &&
    call_ptr->sr.voip_sr.state != CMCALL_VOIP_SR_STATE_FALLBACK_CS)
  {
    return SYS_SYS_MODE_WLAN;
  }
  else if(cmcall_is_ipcall_over_mode_poss (SYS_SYS_MODE_HDR, call_ptr))
  {
    return SYS_SYS_MODE_HDR;
  }
  else if (cmcall_is_ipcall_over_mode_poss (SYS_SYS_MODE_WCDMA, call_ptr))
  {
    return SYS_SYS_MODE_WCDMA;
  }
  else
  {
    return SYS_SYS_MODE_NONE;
  }

} /* cmcall_is_ipcall_possible () */
 

/*===========================================================================

FUNCTION cmcall_det_ss_for_volte_call

DESCRIPTION

 Returns SS based on MSIM/Single SIM configuration

DEPENDENCIES

   None

RETURN VALUE

  Instance.

SIDE EFFECTS
  none

===========================================================================*/
cm_ss_e_type cmcall_det_ss_for_volte_call(

   const cmcall_s_type        *call_ptr
     /* Call object */
)

{
  cm_ss_e_type         ss            = CM_SS_NONE;

  #ifdef FEATURE_MMODE_HYBR2_ENABLED
  if((cmss_ptr()->info.is_hybr_gw_operational) && 
      cmph_is_subs_feature_mode_1x_sxlte(call_ptr->asubs_id))
  {
	ss =CM_SS_HYBR_2;
  }
  else
  #endif
  {
    ss = cmph_map_subs_to_ss(call_ptr->asubs_id);
  }

  return ss;

}

/*===========================================================================

FUNCTION cmcall_instance_for_ipcall

DESCRIPTION

 Returns a valid instance if the given call can be originated over IP on the
 given mode at the given time (based on the state of the call ptr)

DEPENDENCIES

   None

RETURN VALUE

  Instance.

SIDE EFFECTS
  none

===========================================================================*/
static cm_ss_e_type cmcall_instance_for_ipcall(

   const cmcall_s_type        *call_ptr
     /* Call object */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_ss_e_type         ss            = CM_SS_NONE;

  sys_sys_mode_e_type  sys_mode      = SYS_SYS_MODE_NONE;
  sys_modem_as_id_e_type  as_id      = SYS_MODEM_AS_ID_NONE;

  /* IP call is currently possible on only these technologies
  **
  ** - HDR
  ** - WCDMA
  ** - LTE
  */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  CM_ASSERT( BETWEEN(call_ptr->call_type, CM_CALL_TYPE_NONE, CM_CALL_TYPE_MAX ));
  as_id= call_ptr->asubs_id;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check if given call type is allowed to be originated as a ip call */
  sys_mode = cmcall_is_ipcall_possible(call_ptr);
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* If we get here, the call is possible over IP. Now determine the ss
  */

  switch(sys_mode)
  {
    /* If there is a registered VoIP app on hdr and if the voice call has
    ** HDR in it's favored mode preference, return the instance that has
    ** HDR service
    */
    case SYS_SYS_MODE_HDR:
      if (cmss_is_hybr_status_on())
      {
        ss = CM_SS_HDR;
      }
      else
      {
        ss = CM_SS_MAIN;
      }
      break;

   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    /* If there is a registered VoIP app on WCDMA and if the voice call has
    ** WCDMA in it's favored mode preference, return the instance that has
    ** WCDMA service
    */
    /* Determine the ss for WCDMA
    */
    case SYS_SYS_MODE_WCDMA:
      ss = CM_SS_MAIN;
      break;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


    case SYS_SYS_MODE_LTE:
      ss = cmcall_det_ss_for_volte_call(call_ptr);
      break;

    /* FOR WLAN the SS is going to be MAIN */
    case SYS_SYS_MODE_WLAN:
      ss = cmph_map_subs_to_ss(as_id);
      break;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    default:
     return CM_SS_NONE;
  }

  CM_MSG_HIGH_2( "check ip call possibility, sys_mode = %d --> ss = %d",
                 sys_mode, ss );
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* For normal voice call, voip is more preferred than circuit switched call
  ** So return immediately
  ** Also, if it is an EMERGENCY call and a WLAN app is registered with IMS return.
  */
  if(call_ptr->call_type != CM_CALL_TYPE_EMERGENCY || call_ptr->is_vowlan)
  {
    return ss;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* For 911 call over LTE , return immediately
  */
  if(sys_mode == SYS_SYS_MODE_LTE)
  {
    return ss;
  }
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  #ifdef FEATURE_VOIP_E911_SUPPORT
  /* 911 over EHRPD VOIP should not be allowed in non hybrid operation
  */
  if(sys_mode == SYS_SYS_MODE_HDR)
  {
  if (ss == CM_SS_MAIN)
  {
    CM_MSG_HIGH_0("voip911:non hybrid disallowed");
    return CM_SS_NONE;
  }
  /* If the cancel retry timer is running, allow the 911 call
  ** to proceed over VOIP at call start
  */
  if( call_ptr->sr.sr_start_uptime == 0 )
  {
    if(time_get_uptime_secs() < cmph_ptr()->voip_cancel_retry_uptime)
    {
      return ss;
    }
  }
  else
  {
    /* This is not the first silent redial attempt. So allow 911 to proceed
    ** over VOIP.
    */
    if(call_ptr->sr.voip_sr.e911_voip_redial == CMCALL_VOIP_REDIAL_PENDING &&
       call_ptr->end_status == CM_CALL_END_IP_RETRY )
    {
      return ss;
    }
  }
  }
  #endif /* FEATURE_VOIP_E911_SUPPORT */

  return CM_SS_NONE;

} /* cmcall_instance_for_ipcall () */
 

/*===========================================================================

FUNCTION cmcall_send_ipcall_orig

DESCRIPTION
  Send call Origination on IP system.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_send_ipcall_orig
(
  cmcall_s_type       *call_ptr
    /* Call object */
)
{
  sys_sys_mode_e_type sys_mode = SYS_SYS_MODE_NO_SRV;
  boolean orig_send_status = FALSE;
    /* Holds the mode on given instance */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL );

  CM_ASSERT(BETWEEN(call_ptr->call_type, CM_CALL_TYPE_NONE, CM_CALL_TYPE_MAX));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call is sent to this function only after receiving ok_to_orig on
  ** call_ptr->ss. Mode present on call_ptr's instance will be the one
  ** on which origination gets sent.
  ** We do not check for srv of VoWLAN, as CM does not control the SS.
  */
  if (call_ptr->is_vowlan)
  {
    sys_mode = SYS_SYS_MODE_WLAN;
  }
  else if ((sys_mode = cmcall_det_sys_mode_from_ss (call_ptr->ss))
                                                  == SYS_SYS_MODE_NO_SRV)
  {
    CM_ERR_0 (" NO srv, IP origination not possible");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_ptr->sr.orig_sent_on_mode = sys_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_IP_CALL) 

  switch (call_ptr->call_type)
  {
    case CM_CALL_TYPE_VOICE:
    case CM_CALL_TYPE_VS:
    case CM_CALL_TYPE_EMERGENCY:
    case CM_CALL_TYPE_VT:

      orig_send_status = cmipcall_send_orig (call_ptr);
      break;

    default:

      CM_ERR_1 ("Unhandled call type %d", call_ptr->call_type);
      break;

  }

  #endif

  /* reset orig_sent_on_mode only if the origination command was failed 
  while being sent to IP APP */
  if(orig_send_status == FALSE)
  {
    call_ptr->sr.orig_sent_on_mode = SYS_SYS_MODE_NONE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}  /* cmcall_send_ipcall_orig */
 
/*===========================================================================

FUNCTION cmcall_update_voip_redial_state

DESCRIPTION

  Updates the state of the call ptr silent redial object's voip redial
  parameters.

DEPENDENCIES

   None

RETURN VALUE

  None

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_update_voip_redial_state(

 cmcall_s_type             *call_ptr,
    /* pointer to call object */

 cmcall_voip_redial_e_type new_state
     /* new voip redial state */
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  CM_ASSERT(call_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_MSG_HIGH_2("Update voip redial state old %d new %d",
                call_ptr->sr.voip_sr.e911_voip_redial,new_state);
  call_ptr->sr.voip_sr.e911_voip_redial = new_state;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

} /* cmcall_update_voip_redial_state () */

#ifdef FEATURE_VOIP_E911_SUPPORT
/*===========================================================================

FUNCTION cmcall_emergency_voip_redial_init

DESCRIPTION

  Initializes the 911 voip redial parameters.

DEPENDENCIES

   None

RETURN VALUE

  None

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_emergency_voip_redial_init(

  cmcall_s_type             *call_ptr
    /* pointer to call object */
)
{
  cmph_s_type  * ph_ptr = cmph_ptr();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If cancel retry period is running, the first attempt is over VOIP
  ** There is no need to start cs to voip completion timer.
  */
  if(call_ptr->sr.sr_start_uptime < ph_ptr->voip_cancel_retry_uptime)
  {
    cmcall_update_voip_redial_state(call_ptr,CMCALL_VOIP_REDIAL_COMPLETE);
    call_ptr->sr.voip_sr.e911_voip_fallback_uptime =
                                                  call_ptr->sr.sr_max_uptime;
  }
  /* cancel retry timer is not running.
  */
  else
  {
    /* Indicate that voip retry is pending.
    */
    cmcall_update_voip_redial_state(call_ptr,CMCALL_VOIP_REDIAL_PENDING);

    /* If the voip fallback period is not invalid,start the cs to voip
    ** fallback timer.
    */
    if(ph_ptr->cs_to_voip_fallback_period != 0)
    {
      call_ptr->sr.voip_sr.e911_voip_fallback_uptime =
            call_ptr->sr.sr_start_uptime + ph_ptr->cs_to_voip_fallback_period;
    }
    else
    {
      call_ptr->sr.voip_sr.e911_voip_fallback_uptime =
                                                  call_ptr->sr.sr_max_uptime;
    }
  }
  /* Reset the cancel retry timer
  */
  ph_ptr->voip_cancel_retry_uptime = 0 ;
  CM_MSG_HIGH_2("voip911: start %d, cs_to_voip %d",
                   call_ptr->sr.sr_start_uptime,
                   call_ptr->sr.voip_sr.e911_voip_fallback_uptime);
  return;

} /*cmcall_emergency_voip_redial_init() */

#endif // FEATURE_VOIP_E911_SUPPORT
 

/*===========================================================================

FUNCTION cmcall_update_voip_sr_ps_failure

DESCRIPTION

  Updates the ps failure of voip silent redial parameters in the call object.

DEPENDENCIES

   None

RETURN VALUE

  None

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_update_voip_sr_ps_failure(

 cmcall_s_type             *call_ptr,
    /* pointer to call object */

 cmcall_ps_failure_e_type  ps_failure
     /* new call type */
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  CM_MSG_HIGH_5("VOIP SR call id %d type %d is valid %d, ps_failure old %d new %d",
         call_ptr->call_id,
         call_ptr->call_type,
         call_ptr->sr.voip_sr.is_valid, call_ptr->sr.voip_sr.ps_failure, ps_failure);
  

  if( ! call_ptr->sr.voip_sr.is_valid  )
  {
    return;

  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_ptr->sr.voip_sr.ps_failure = ps_failure;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

} /* cmcall_update_voip_sr_ps_failure */


/*===========================================================================

FUNCTION cmcall_update_voip_sr_state

DESCRIPTION

  Updates the state of voip silent redial parameters in the call object.

DEPENDENCIES

   None

RETURN VALUE

  None

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_update_voip_sr_state(

 cmcall_s_type             *call_ptr,
    /* pointer to call object */

 cmcall_voip_sr_state_e_type new_state
     /* new voip redial state */
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  CM_ASSERT(call_ptr != NULL);
  CM_MSG_HIGH_5("VOIP SR call id %d type %d is valid %d, voip sr_state old %d new %d",
         call_ptr->call_id,
         call_ptr->call_type,
         call_ptr->sr.voip_sr.is_valid, call_ptr->sr.voip_sr.state, new_state );

  if( ! call_ptr->sr.voip_sr.is_valid  )
  {
    return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_ptr->sr.voip_sr.state = new_state;

  return;

} /* cmcall_update_voip_sr_state () */


/*===========================================================================

FUNCTION cmcall_update_voip_sr_cs_fallback_uptime

DESCRIPTION

  Updates the cs fallback uptime of voip silent redial parameters in the call
  object.

DEPENDENCIES

   None

RETURN VALUE

  None

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_update_voip_sr_cs_fallback_uptime(

  cmcall_s_type                     *call_ptr,
     /* pointer to call object */

  dword                             cs_fallback_uptime
     /* cs fallback uptime */

)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  CM_MSG_HIGH_5("VOIP SR call id %d type %d is valid %d cs_fallback_uptime old %d new %d",
         call_ptr->call_id,
         call_ptr->call_type,
         call_ptr->sr.voip_sr.is_valid,
         call_ptr->sr.voip_sr.type.voice.cs_fallback_uptime,
         cs_fallback_uptime);

  if( ! call_ptr->sr.voip_sr.is_valid  )
  {
    return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_ptr->sr.voip_sr.type.voice.cs_fallback_uptime = cs_fallback_uptime;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

}

/*===========================================================================

FUNCTION cmcall_update_voip_sr_is_abort

DESCRIPTION

  Updates abort indication of voip silent redial parameters in the call object.

DEPENDENCIES

   None

RETURN VALUE

  None

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_update_voip_sr_is_abort(

  cmcall_s_type                     *call_ptr,
     /* pointer to call object */

  boolean                           is_abort
     /* Abort indication */

)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  CM_ASSERT(call_ptr != NULL);

  CM_MSG_HIGH_5("VOIP SR call id %d type %d is valid %d is_abort old %d new %d",
         call_ptr->call_id,
         call_ptr->call_type,
         call_ptr->sr.voip_sr.is_valid,
         call_ptr->sr.voip_sr.type.voice.is_abort,
         is_abort);

  if( ! call_ptr->sr.voip_sr.is_valid  )
  {
    return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_ptr->sr.voip_sr.type.voice.is_abort = is_abort;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

}


/*===========================================================================

FUNCTION cmcall_voip_sr_is_abort

DESCRIPTION

  Return the abort indication of voip silent redial parameters in
  the call object.

DEPENDENCIES

   None

RETURN VALUE

  None

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmcall_voip_sr_is_abort(

  const cmcall_s_type         *call_ptr
     /* pointer to call object */
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  CM_ASSERT(call_ptr != NULL);

  if( ! call_ptr->sr.voip_sr.is_valid  )
  {
    CM_MSG_HIGH_3("VOIP SR call id %d type %d is valid %d ",
           call_ptr->call_id,
           call_ptr->call_type,
           call_ptr->sr.voip_sr.is_valid );

    return FALSE;

  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  return ( call_ptr->sr.voip_sr.type.voice.is_abort );

}

/*===========================================================================

FUNCTION cmcall_voip_redial_init

DESCRIPTION

  Initializes the voip redial parameters. Currnet this operation is limited
  to E911 and voice call over IP.


DEPENDENCIES

   None

RETURN VALUE

  None

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(818, call_ptr)*/
static void cmcall_voip_redial_init(

  cmcall_s_type             *call_ptr
    /* Pointer to call object */
)
{
  cmmsc_stack_s_type			*stack_pointer;
  cmmsc_state_machine_s_type           *state_machine = NULL;
  dword        cs_fallback_uptime = CM_INVALID_VOIP_TO_CS_FALLBACK_TIME;
  sys_sys_mode_e_type   sys_mode_ip_call   = SYS_SYS_MODE_NONE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL );

  state_machine = cmmsimc_state_machine_ptr(SYS_MODEM_AS_ID_1);

  if (state_machine == NULL)
    return;

  stack_pointer = &state_machine->stack[0];

  /* Bail out if IP call is not possible over HDR or LTE .
  */
  sys_mode_ip_call = cmcall_is_ipcall_possible(call_ptr);
  if( !(  sys_mode_ip_call == SYS_SYS_MODE_HDR  ||
          sys_mode_ip_call == SYS_SYS_MODE_LTE))
  {
    return;
  }

  /* Bail out if call is CSFB type && call is an emergency call
  */
  #if defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)
  if (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY &&
      call_ptr->csfb_type != CM_CALL_CSFB_TYPE_NONE)
  {
    return;
  }
  #endif //#if defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)

  /* Initialize e911 related parameters, if it is emergency call.
  ** Here check is required, if we decide to remove the featurization.
  */
  
#ifdef  FEATURE_MMODE_EMC_SUPPORT
  if( call_ptr->call_type == CM_CALL_TYPE_EMERGENCY )
  {
    /* To Avoid back to back originations sent to SD, do not reduce hold
    ** orig , if emerg prefs were forced immediately prior to this
    */
    if(cmph_get_last_pref_sent(call_ptr->ss) != NULL)
    {
    if( ! (cmph_get_last_pref_sent(call_ptr->ss)->orig_mode ==
                                                SD_SS_ORIG_MODE_EMERG_ORIG ))
    {
      call_ptr->sr.hold_orig_uptime = time_get_uptime_secs();
      CM_MSG_HIGH_1( "voip911:Ending hold-orig period, uptime=%ld",
                                             time_get_uptime_secs());
    }
  }

    #ifdef FEATURE_VOIP_E911_SUPPORT
    /* Initialization for 911 call over EHRPD */
    if( sys_mode_ip_call == SYS_SYS_MODE_HDR)
    {
      cmcall_emergency_voip_redial_init(call_ptr);
      return;
    }
  #endif

  }
#endif

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    /* For non E911 voice call over IP, set the voip sr to be valid.
    */

    call_ptr->sr.voip_sr.is_valid = TRUE;

    /* If VOIP call is supported, set the voip to cs fallback timer and flag.
    */
    /* if call type is emergency, set cs fallback timer to invalid, as cs fallback timer would
    be running in the context of IMS i.e. Tlte-911 timer. IMS will run this timer when the call origination
    has been sent to IMS . When timer expires without call getting connected, IMS will report failure -FALLBACK_TO_CS to CM
    ** IF volte_sr_control is TRUE (KDDI Req) disable CS FALLBACK TIMER
    */
    if(call_ptr->call_type == CM_CALL_TYPE_EMERGENCY ||
         cmph_get_sub_config_ptr(call_ptr->asubs_id)->volte_sr_control)
    {
      cs_fallback_uptime = CM_INVALID_VOIP_TO_CS_FALLBACK_TIME;
    }
    else if(!cmcall_is_volte_restricted_call(call_ptr) && 
		SD_GET_COMMON_MODE(
		   cmph_map_cm_mode_pref_to_sd_mode_pref(call_ptr->favored_mode_pref),
		   stack_pointer->capablity.mode) != SD_SS_MODE_PREF_LTE
		)
    {
      cs_fallback_uptime =
           call_ptr->sr.sr_start_uptime + CM_DEFAULT_VOIP_TO_CS_FALLBACK_TIME;
    }
    cmcall_update_voip_sr_cs_fallback_uptime( call_ptr, cs_fallback_uptime );

    /* Reset the PS call failure flag.
    */
    cmcall_update_voip_sr_ps_failure(call_ptr, CMCALL_PS_FAILURE_MAX );

    /* Reset the VOIP SR state.
    */
    cmcall_update_voip_sr_state( call_ptr, CMCALL_VOIP_SR_STATE_NONE );

    /* Reset the abort state.
    */
    cmcall_update_voip_sr_is_abort( call_ptr, FALSE );
} /*cmcall_voip_redial_init() */





/*===========================================================================

FUNCTION cmcall_voip_redial_action_update

DESCRIPTION

  Update the voip redial action based on the other criteria.

  Here is the list of criteria for VOIP call fallback to 1X

  1) There is no data connection and voip to cs fallback timer
     expires. Or
  2) Upon receiving CMIPAPP_END_CAUSE_TEMPORARY_FAILURE from VOIP
     and voip to cs fallback timer hasexpired or
  3) Upon receiving CMIPAPP_END_CAUSE_FALLBACK_TO_CS failure from
     VOIP or
  4) Upon receiving HDR connection failure due to general
     (CM_CD_GENERAL) or auth/bill(CM_CD_AUTH_BILLING_FAIL) failures or
  5) Upon receiving HDR origination failure due to access failure
      (CM_CALL_ORIG_ERR_ACC_FAIL) or acess timer out
      (CM_CALL_ORIG_ERR_RTCACK_TIMEOUT).


DEPENDENCIES

   None

RETURN VALUE

  None

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_voip_redial_action_update (

  cmcall_s_type             *call_ptr,
    /* pointer to call object */

  dword                     uptime
    /*  Uptime */

)
{

  sys_sys_mode_e_type           curr_sys_mode = SYS_SYS_MODE_NONE;

  /* Store the PS call failure
  ** Initialize to NONE.
  */
   cmcall_ps_failure_e_type      ps_call_failure = CMCALL_PS_FAILURE_MAX;

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

   cm_hdr_connection_deny_e_type conn_deny = CM_CD_MAX;

   cm_call_orig_fail_e_type      orig_err = CM_CALL_ORIG_ERR_MAX;

  #endif

   cm_call_end_e_type            new_end_status = CM_CALL_END_MAX;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #ifdef  FEATURE_VOIP_E911_SUPPORT

  /*    If we are still in the hold-orig period and it is an emergency call
  **    with voip available at the time of origination and
  **    1. the CS to voip fallback timer has expired  or,
  **    2. If there is an acq fail on main stack or,
  **    3. Permanent call failure reason from 1x cp
  **        end the hold-orig period now.
  **   Set the call end status to IP_RETRY to ensure that the call is
  **   attempted over VOIP
  */
  if( call_ptr->sr.voip_sr.e911_voip_redial == CMCALL_VOIP_REDIAL_PENDING
       && (uptime >= call_ptr->sr.voip_sr.e911_voip_fallback_uptime ||
           call_ptr->end_status == CM_CALL_END_IP_RETRY)
    )
  {
    if(call_ptr->sr.hold_orig_uptime > uptime)
    {
      call_ptr->sr.hold_orig_uptime = uptime;
      CM_MSG_LOW_1( "voip911:Ending hold-orig(fallback timer),uptime=%ld",
                                                     uptime);
    }

    /* If the origination has not been sent on any mode yet
    ** (fallback timer expired) or if the call failed over CS,
    ** force the call to IP
    */
    if(call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE ||
       call_ptr->end_status != CM_CALL_END_NONE )
    {
      call_ptr->end_status =  CM_CALL_END_IP_RETRY;
    }
  }

  #endif // FEATURE_VOIP_E911_SUPPORT

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Bail out if VOIP SR is not valid.
  */

  if( !call_ptr->sr.voip_sr.is_valid )
  {
    CM_MSG_HIGH_0("VOIP sr is_valid FALSE");
    return;
  }

  /* Processing for 911 over ehrpd */
  /* this portion needs to be updated, if in case
  E911 over hybrid-2 changes are done, and hybrid preferences are ON
  when E911 is placed over Hybrid-2 */

  #ifdef  FEATURE_MMODE_EMC_SUPPORT
  if (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY &&
      call_ptr->favored_hybr_pref == CM_HYBR_PREF_ON)
  {
    /* If the emergency call is attempted on PS and received any failure,
    ** then fallback to CS and indicate to IP APP
    */
    if( call_ptr->sr.voip_sr.ps_failure != CMCALL_PS_FAILURE_MAX )
    {
      call_ptr->end_status = CM_CALL_END_IP_FAIL;
      cmcall_update_voip_sr_state(call_ptr, CMCALL_VOIP_SR_STATE_FALLBACK_CS);
      cmcall_update_voip_sr_is_abort( call_ptr, TRUE );

      /* Reset the PS failure to make place for future failures
      */
      cmcall_update_voip_sr_ps_failure( call_ptr, CMCALL_PS_FAILURE_MAX );

      CM_MSG_HIGH_1("voip911: IP Fail for call id=%d", call_ptr->call_id);
    }

    return;
  }
  #endif /* FEATURE_VOIP_E911_SUPPORT  */
  /* Bail out if the state is CMCALL_VOIP_SR_STATE_FALLBACK_CS
  */

  if( call_ptr->sr.voip_sr.state == CMCALL_VOIP_SR_STATE_FALLBACK_CS )
  {
    /* If we receive any type of failures from IMS while call has fallen back
    ** to CS mode, then no further call processing required. This may occur
    ** when both CM & IMS try to stop attempting on IP for different reasons
    ** around the same time.
    */
    if( call_ptr->end_status == CM_CALL_END_IP_END ||
        call_ptr->end_status == CM_CALL_END_IP_FAIL ||
        call_ptr->end_status == CM_CALL_END_IP_RETRY )
    {
      call_ptr->end_status = CM_CALL_END_NONE;
    }
    CM_MSG_HIGH_0("VOIP sr state is FALLBALCK");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* First process the IMS failures if any.
  */

  /* Upon receiving permanet failure (Type 4) from IMS, Call shall be ended.
  ** Then bear out from here.
  */
  CM_MSG_HIGH_1("VOIP redial update: call end status =%d",call_ptr->end_status);
  if( call_ptr->end_status == CM_CALL_END_IP_END )
  {
    cmcall_update_voip_sr_state( call_ptr,
                             CMCALL_VOIP_SR_STATE_IP_END );

   return;
  }


  /* Upon receiving permanet failure (Type 3) from IMS, fallback to 1x.
  ** Then bear out from here.
  */

  if( call_ptr->end_status == CM_CALL_END_IP_FAIL )
  {

    CM_MSG_HIGH_1("is_volte_restricted_call %d", cmcall_is_volte_restricted_call(call_ptr));
    if(cmcall_is_volte_restricted_call(call_ptr))
    {
      call_ptr->end_status = CM_CALL_END_IP_RETRY;
      cmcall_update_voip_sr_state( call_ptr,
                             CMCALL_VOIP_SR_STATE_IP_RETRY );
    }
    else
    {
      call_ptr->is_volte = FALSE;
      cmcall_update_voip_sr_state( call_ptr,
                             CMCALL_VOIP_SR_STATE_FALLBACK_CS );
      cmph_send_trm_priority_req(LTE_TRM_PRIORITY_LOW,
        call_ptr->asubs_id);
      cmcall_check_and_send_mmtel_status(call_ptr,FALSE);
    }

    return;
  }

  /* Upon receiving retry-after failure (Type 5) from IMS, fallback to 1x.in
  ** mmode or set IP end for volte restricted mode.
  */
  if( call_ptr->end_status == CM_CALL_END_IP_RETRY_AFTER_FAIL )
  {

    if(cmcall_is_volte_restricted_call(call_ptr))
    {
      call_ptr->end_status = CM_CALL_END_IP_END;
      cmcall_update_voip_sr_state( call_ptr,
                             CMCALL_VOIP_SR_STATE_IP_END);
    }
    else
    {
      call_ptr->end_status = CM_CALL_END_IP_FAIL;
      call_ptr->is_volte = FALSE;
      CM_MSG_HIGH_0("fallback: is_volte = FALSE");
      cmcall_update_voip_sr_state( call_ptr,
                             CMCALL_VOIP_SR_STATE_FALLBACK_CS );
    }

    return;
  }

  /* Upon receiving temporary failure (Type 1) from IMS, if there is no data
  ** connection and voip to cs fallback timer has expired, fallback to 1x.
  */

  if( call_ptr->end_status == CM_CALL_END_IP_RETRY )
  {
    /* If fallback to cs timer expires, fallback to 1x if call is not PS only and not VOLTE restricted call
    ** Bear out from here.
    */
    if(!cmcall_is_volte_restricted_call(call_ptr))
    {
      if( call_ptr->sr.voip_sr.type.voice.cs_fallback_uptime < uptime )
      {
        /* Fall back to CS after CS fallback timer expired */
        new_end_status = CM_CALL_END_IP_FAIL;
        CM_MSG_HIGH_3("VOIP SR call id %d end status old %d new %d",
                     call_ptr->call_id,
                     call_ptr->end_status,
                     new_end_status);

        call_ptr->end_status = new_end_status;
        cmcall_update_voip_sr_state( call_ptr,
                                  CMCALL_VOIP_SR_STATE_FALLBACK_CS );
        return;

       }
    }

    CM_MSG_HIGH_2("uptime=%d, sr_max_time=%d",uptime, call_ptr->sr.sr_max_uptime);
    if(uptime > call_ptr->sr.sr_max_uptime)
    {
    /* Else update the state and return.
    */
      call_ptr->end_status = CM_CALL_END_IP_END;
      cmcall_update_voip_sr_state( call_ptr,
                             CMCALL_VOIP_SR_STATE_IP_END );
    }
    else
    {
      cmcall_update_voip_sr_state( call_ptr,
                             CMCALL_VOIP_SR_STATE_IP_RETRY );
    }

    return;

  }

  if(call_ptr->end_status == CM_CALL_END_IP_HARD_FAILURE)
  {
    if(uptime > call_ptr->sr.sr_max_uptime)
    {
    /* Else update the state and return.
    */
      call_ptr->end_status = CM_CALL_END_IP_END;
      cmcall_update_voip_sr_state( call_ptr,
                             CMCALL_VOIP_SR_STATE_IP_END );
    }
    else
    {
      /* reusing IP_RETRY state*/
      cmcall_update_voip_sr_state( call_ptr,
                             CMCALL_VOIP_SR_STATE_IP_RETRY );
    }
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Secondly, process the HDR connection deny and origination failures
  ** if any.
  **
  **  1)fall back to 1x upon receiving HDR connection failure
  **    (general/auth/bill) or origination failure( RTC ACk time out or
  **    access time out)
  **
  **  2) Retry over IMS uon receving HDR connection failure( network busy or
  **     preferred channel not available.)
  */


  if ( call_ptr->sr.voip_sr.ps_failure != CMCALL_PS_FAILURE_MAX )
  {

    ps_call_failure = call_ptr->sr.voip_sr.ps_failure;

    /* Reset the ps call failure cause.
    */
    cmcall_update_voip_sr_ps_failure( call_ptr, CMCALL_PS_FAILURE_MAX );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if( ps_call_failure == CMCALL_PS_FAILURE_HDR_CONNECTION_FAILURE )
    {

      #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

      conn_deny = call_ptr->sr.voip_sr.failure.conn_deny;

      switch ( conn_deny )
      {

        case CM_CD_GENERAL:
        case CM_CD_AUTH_BILLING_FAIL:

          /* Force the VOIP fall back to cs.
          */
          new_end_status = CM_CALL_END_IP_FAIL;

          CM_MSG_HIGH_3("VOIP SR call id %d end status curr %d new %d",
                      call_ptr->call_id,
                      call_ptr->end_status,
                      new_end_status);

          call_ptr->end_status = new_end_status;

          cmcall_update_voip_sr_is_abort( call_ptr, TRUE );

          cmcall_update_voip_sr_state( call_ptr,
                                       CMCALL_VOIP_SR_STATE_FALLBACK_CS );

          return;

        case CM_CD_NETWORK_BUSY:
        case CM_CD_PREF_CH_NOT_AVAIL:
        case CM_CD_MAX:
        default:

          break;
      } /* VOIP connection deny */

      #endif

    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Update the redial action based on PS call origination failure..
    */
    if( ps_call_failure ==  CMCALL_PS_FAILURE_ORIG )
    {

      #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

      orig_err = call_ptr->sr.voip_sr.failure.ps_orig;

      if( orig_err == CM_CALL_ORIG_ERR_ACC_FAIL ||
          orig_err == CM_CALL_ORIG_ERR_RTCACK_TIMEOUT
        )
      {
        /* Force the VOIP fall back to cs.
        */
        new_end_status = CM_CALL_END_IP_FAIL;

        CM_MSG_HIGH_3("VOIP SR call id %d end status old %d new %d",
                    call_ptr->call_id,
                    call_ptr->end_status,
                    new_end_status);

        call_ptr->end_status = new_end_status;

        cmcall_update_voip_sr_is_abort( call_ptr, TRUE );

        cmcall_update_voip_sr_state( call_ptr,
                                     CMCALL_VOIP_SR_STATE_FALLBACK_CS );

        return;

      }
      #endif

    }/* PS call origination failure */


    /* Continue over IP.
    */
    cmcall_update_voip_sr_state( call_ptr,
                             CMCALL_VOIP_SR_STATE_IP_RETRY );

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Third check if there is no data connection and voip to cs fallback timer
  ** expires.
  ** If IP orig is already sent to IMS, do not force non-IP from CM even if
  ** VOIP was deregistered during the call. Let IMS decide to end the call
  ** or reinstate VOIP support.
  */
  CM_MSG_HIGH_3("REDIAL: VOIP SR is_ip_orig_sent %d uptime %d cs_fallback_uptime %d",
                call_ptr->is_ip_orig_sent,
                uptime,
                call_ptr->sr.voip_sr.type.voice.cs_fallback_uptime);

  if(!call_ptr->is_ip_orig_sent || 
      (call_ptr->sr.voip_sr.type.voice.cs_fallback_uptime < uptime &&
      (!cmcall_check_call_type_in_call_state( CM_CALL_TYPE_PS_DATA,
                                              CM_CALL_STATE_CONV )))
    )
  {
  curr_sys_mode = cmcall_is_ipcall_possible(call_ptr);

  /* The criterial to redial over CS
  **
  ** 1) If voip is deregistered, or
  ** 2) If the CS fallback timer expire and there is no PS data call in
  **    converstation state, force to none IP.
  */
  if( (( curr_sys_mode != SYS_SYS_MODE_HDR )
                &&
      ( curr_sys_mode != SYS_SYS_MODE_LTE )
      && 
      ( curr_sys_mode != SYS_SYS_MODE_WLAN ))
               ||
      (  call_ptr->sr.voip_sr.type.voice.cs_fallback_uptime < uptime &&
         (!cmcall_check_call_type_in_call_state( CM_CALL_TYPE_PS_DATA,
                                                   CM_CALL_STATE_CONV ))
      )
    )
  {

      if(call_ptr->call_state == CM_CALL_STATE_ORIG &&
         call_ptr->call_subst.orig == CM_CALL_ORIG_SUBST_ALERTING
         )
      {
        CM_MSG_HIGH_0("MT party alerted so don't redial");
        return;
      }
    if(call_ptr->srv_type == CM_SRV_TYPE_PS_ONLY &&
       call_ptr->call_type != CM_CALL_TYPE_EMERGENCY)
    {
      call_ptr->end_status = CM_CALL_END_IP_END;
      cmcall_update_voip_sr_state( call_ptr,
                                 CMCALL_VOIP_SR_STATE_IP_END);
      CM_MSG_HIGH_0("PS only call so don't fallback");
      return;

    }
    new_end_status = CM_CALL_END_IP_FAIL;

    CM_MSG_HIGH_3("VOIP SR call id %d end status old %d new %d",
                call_ptr->call_id,
                call_ptr->end_status,
                new_end_status);

    call_ptr->end_status = new_end_status;

    cmcall_update_voip_sr_is_abort( call_ptr, TRUE );

    cmcall_update_voip_sr_state( call_ptr,
                                 CMCALL_VOIP_SR_STATE_FALLBACK_CS );

   return;

  }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
}




/*===========================================================================

FUNCTION cmcall_voip_redial

DESCRIPTION
  Forces the appropriate SS for call origination to attempt VOIP

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

  This function is expected to be called during the origination process
  before call is connected.

RETURN VALUE
  A ballpark estimation for the number of seconds the phone is going
  to be without service. If mode is not forced, return 0.

SIDE EFFECTS
  none

===========================================================================*/
static dword        cmcall_voip_redial(

  cmcall_s_type    *call_ptr
        /* pointer to a call object */
)
{
  cmph_s_type           *ph_ptr               = cmph_ptr();

  sd_ss_pref_reas_e_type pref_reas            = SD_SS_PREF_REAS_REDIAL_NORM;

  cm_orig_q_s_type      *computed_orig_para   = NULL;

  cm_orig_q_s_type      *ph_orig_para_top_ptr = NULL;

  cm_ss_e_type           new_ss               = CM_SS_NONE;
  sys_modem_as_id_e_type   asubs_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr           != NULL );
  CM_ASSERT( ph_ptr             != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get ss and asubs_id for the call */
  asubs_id = cmcall_get_asubs_id(call_ptr);
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find the previously used origination parameters */

  computed_orig_para =  cmtask_orig_para_search_act_id  ( call_ptr->ss,
                                  (cm_act_id_type) call_ptr->call_id );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If there is already a PS call in originating state, then CM doesn't
  ** redial due to any HDR failure for VoIP call
  */
  if(cmcall_check_call_type_in_call_state( CM_CALL_TYPE_PS_DATA,
                                           CM_CALL_STATE_ORIG))
  {
    CM_MSG_HIGH_0( "No VoIP SR as PS call is in ORIG state");
    return 0;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If computed ss has a preference of higher priority use those preferences */
  ph_orig_para_top_ptr = cmtask_orig_para_get_top( call_ptr->ss );

  if( ph_orig_para_top_ptr != NULL &&
          cmph_cmp_act_priorities( ph_orig_para_top_ptr->orig->act_priority,
                                   cmph_get_act_priority(
                                  cmph_map_call_type_to_act_type(call_ptr->call_type))) > 0
    )
    {
      return cmph_force_pref_on_the_fly
                (
                 call_ptr->ss,
                 ph_ptr,
                 pref_reas,
                 cmph_map_call_type_to_act_type( call_ptr->call_type ),
                 CM_PREF_TERM_CM_1_CALL_PLUS,
                 call_ptr->favored_mode_pref,
                 ph_orig_para_top_ptr->orig->orig_band_pref,
                 ph_orig_para_top_ptr->orig->orig_lte_band_pref,
                 ph_orig_para_top_ptr->orig->orig_tds_band_pref,
                 CMPH_SS_PLMN_PREF(asubs_id,ph_ptr),
                 ph_orig_para_top_ptr->orig->orig_srv_domain_pref,
                 CM_NETWORK_SEL_MODE_PREF_NONE,
                 (cm_act_id_type) call_ptr->call_id,
                 CM_ACT_UPDATE_REAS_ACT_CONT,
                 call_ptr->force_top_pref,
                 NULL,
                 CM_DEFAULT_VALUE,
                 ph_orig_para_top_ptr->orig->csg_id,
                 ph_orig_para_top_ptr->orig->csg_rat,
                 ph_orig_para_top_ptr->orig->ciot_lte_pref
               );
  } /*  if( ph_orig_para_top_ptr != NULL ... */

  /* Else force the preferences that were forced previously on main*/

  if( computed_orig_para != NULL )
  {
    return cmph_force_pref_on_the_fly
           (
             call_ptr->ss,
             ph_ptr,
             pref_reas,
             cmph_map_call_type_to_act_type( call_ptr->call_type ),
             CM_PREF_TERM_CM_1_CALL_PLUS,
             call_ptr->favored_mode_pref,
             computed_orig_para->orig->orig_band_pref,
             computed_orig_para->orig->orig_lte_band_pref,
             computed_orig_para->orig->orig_tds_band_pref,
             CMPH_SS_PLMN_PREF(asubs_id,ph_ptr),
             computed_orig_para->orig->orig_srv_domain_pref,
             CM_NETWORK_SEL_MODE_PREF_NONE,
             (cm_act_id_type) call_ptr->call_id,
             CM_ACT_UPDATE_REAS_ACT_CONT,
             TRUE,
             NULL,
             CM_DEFAULT_VALUE,
             computed_orig_para->orig->csg_id,
             computed_orig_para->orig->csg_rat,
             computed_orig_para->orig->ciot_lte_pref
           );
  }

  return 0;

}/* cmcall_voip_redial */

#endif

#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

/*===========================================================================

FUNCTION cmcall_force_sr_on_the_fly

DESCRIPTION
  Force optimized silent redial if necessary conditions apply.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

  This function is expected to be called during the origination process
  before call is connected.

RETURN VALUE
  A ballpark estimation for the number of seconds the phone is going
  to be without service. If mode is not forced, return 0.

SIDE EFFECTS
  none

===========================================================================*/
static dword  cmcall_force_sr_on_the_fly(

    const cmcall_s_type    *call_ptr,
        /* pointer to a call object */

    word                   rssi_thr,
        /* RSSI threshold for deciding whether to force optimized silent
        ** redial. 0 implies force optimized silent redial regardless of RSSI
        ** reading */

    boolean                is_force_roam_pref
        /* Indicate whether the roam preference should be forced to ensure
        ** no change of roaming indication.
        */
)
{
  const cdma_cmcall_type *call_mode_info_ptr = NULL;
      /* Pointer to call mode info */

  cmph_s_type           *ph_ptr              = call_ptr->ph_ptr;
      /* Point at phone object */

  boolean               is_optimized_sr      = TRUE;
      /* Indicate whether optimized silent redial should be forced */

  #ifdef FEATURE_HDR_HYBRID
  cmss_s_type           *ss_ptr              =  cmss_ptr();
  #endif  /* FEATURE_HDR_HYBRID */
  cm_ciot_lte_pref_s_type       ciot_lte_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  
  CM_ASSERT( call_ptr != NULL );
  CM_ASSERT( ph_ptr != NULL );

  #ifdef FEATURE_MMODE_REMOVE_1X
  #error code not present
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_mode_info_ptr = CALL_MODE_INFO_PTR( call_ptr );
  
  ciot_lte_pref.lte_op_mode_pref  = CM_LTE_IOT_OP_MODE_PREF_NO_CHANGE;
  ciot_lte_pref.lte_m1_band_pref = SYS_LTE_BAND_MASK_CONST_NO_CHG;
  ciot_lte_pref.lte_nb1_band_pref = SYS_LTE_BAND_MASK_CONST_NO_CHG;
  ciot_lte_pref.lte_band_pref     = SYS_LTE_BAND_MASK_CONST_NO_CHG;
  
  /* Do NOT force optimized silent redial if RSSI level is high enough.
  ** Note that 0 rssi_thr implies force optimized SR regardless of RSSI reading.
  **
  ** NOTE! RSSI is implicitly negative, so smaller number means
  ** bigger value.
  */
  #ifdef FEATURE_HDR_HYBRID
  /* only read search CDMA rssi if the current system is CDMA */
  if(ss_ptr->info.sys_mode == SYS_SYS_MODE_CDMA)
  #endif
  {
    /* Only check if CDMA is supported */
    #ifndef FEATURE_MMODE_REMOVE_1X
    if( (word) srch_get_best_rssi() < rssi_thr )
    #endif
    {
      is_optimized_sr = FALSE;
    }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do NOT force optimized silent redial if this is HDR only call
  ** or the current system is HDR. The only exception to this rule is
  ** 1. The srv type is CM_SRV_TYPE_HDR_MORE_PREF
  */
  #ifdef FEATURE_HDR_HYBRID
  if(call_ptr->favored_mode_pref == CM_MODE_PREF_HDR_ONLY ||
     ss_ptr->info.sys_mode == SYS_SYS_MODE_HDR)
  {
    is_optimized_sr = FALSE;
  }
  #endif

  #ifdef FEATURE_CM_HDR_OPTI_SILENT_REDIAL
  if(call_ptr->srv_type == CM_SRV_TYPE_HDR_MORE_PREF)
  {
    is_optimized_sr = TRUE;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If optimized silent redial is required, force the SS-Preference with
  ** optimized silent redial.
  */
  if( is_optimized_sr )
  {
    cm_roam_pref_e_type   favored_roam_pref;
    cm_pref_s_type         *hybr_2_pref_ptr  = NULL;
    cm_pref_s_type         *hybr_3_pref_ptr  = NULL;

    dword                        rescan_time = 0;

    CM_MSG_HIGH_0( "Optimized SR");

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If the roam preference should be forced to ensure no change of
    ** roaming indication, get the favored roam preference. Else use existing
    ** roaming preference.
    */
    if( is_force_roam_pref )
    {
      favored_roam_pref = call_ptr->favored_roam_pref;
    }
    else
    {
      favored_roam_pref = CM_ROAM_PREF_NO_CHANGE;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if defined(FEATURE_MMODE_DUAL_SIM)
    if (cmph_is_msim())
    {
      /* Allocate buffer only if multi-sim */
      if (!cmph_no_hybr2())
      {
      hybr_2_pref_ptr = cm_pref_ptr();
      memscpy(hybr_2_pref_ptr, sizeof(cm_pref_s_type),
                  &(ph_ptr->hybr_2_stack_info.pref_info), sizeof(cm_pref_s_type));

      hybr_2_pref_ptr->network_sel_mode_pref   = CM_NETWORK_SEL_MODE_PREF_NONE;
      hybr_2_pref_ptr->acq_order_pref          = CM_GW_ACQ_ORDER_PREF_NO_CHANGE;
      hybr_2_pref_ptr->prl_pref                = CM_PRL_PREF_NO_CHANGE;
      hybr_2_pref_ptr->srv_domain_pref         = CM_SRV_DOMAIN_PREF_NO_CHANGE;
      hybr_2_pref_ptr->roam_pref               = CM_ROAM_PREF_NO_CHANGE;
      hybr_2_pref_ptr->mode_pref               = CM_MODE_PREF_NO_CHANGE;
      hybr_2_pref_ptr->band_pref               = CM_BAND_PREF_NO_CHANGE;
      hybr_2_pref_ptr->pref_term               = CM_PREF_TERM_CM_1_CALL_PLUS;
      hybr_2_pref_ptr->ciot_lte_pref.lte_op_mode_pref = CM_LTE_IOT_OP_MODE_PREF_NO_CHANGE;
      hybr_2_pref_ptr->ciot_lte_pref.lte_m1_band_pref = SYS_LTE_BAND_MASK_CONST_NO_CHG;
      hybr_2_pref_ptr->ciot_lte_pref.lte_nb1_band_pref = SYS_LTE_BAND_MASK_CONST_NO_CHG;
      hybr_2_pref_ptr->ciot_lte_pref.lte_band_pref     = SYS_LTE_BAND_MASK_CONST_NO_CHG;
      }

      #ifdef FEATURE_MMODE_HYBR3_ENABLED

      hybr_3_pref_ptr = cm_pref_ptr();
      memscpy(hybr_3_pref_ptr, sizeof(cm_pref_s_type),
                  &(ph_ptr->hybr_3_stack_info.pref_info), sizeof(cm_pref_s_type));

      hybr_3_pref_ptr->network_sel_mode_pref   = CM_NETWORK_SEL_MODE_PREF_NONE;
      hybr_3_pref_ptr->acq_order_pref          = CM_GW_ACQ_ORDER_PREF_NO_CHANGE;
      hybr_3_pref_ptr->prl_pref                = CM_PRL_PREF_NO_CHANGE;
      hybr_3_pref_ptr->srv_domain_pref         = CM_SRV_DOMAIN_PREF_NO_CHANGE;
      hybr_3_pref_ptr->roam_pref               = CM_ROAM_PREF_NO_CHANGE;
      hybr_3_pref_ptr->mode_pref               = CM_MODE_PREF_NO_CHANGE;
      hybr_3_pref_ptr->band_pref               = CM_BAND_PREF_NO_CHANGE;
      hybr_3_pref_ptr->pref_term               = CM_PREF_TERM_CM_1_CALL_PLUS;
      hybr_3_pref_ptr->ciot_lte_pref.lte_op_mode_pref = CM_LTE_IOT_OP_MODE_PREF_NO_CHANGE;
      hybr_3_pref_ptr->ciot_lte_pref.lte_m1_band_pref = SYS_LTE_BAND_MASK_CONST_NO_CHG;
      hybr_3_pref_ptr->ciot_lte_pref.lte_nb1_band_pref = SYS_LTE_BAND_MASK_CONST_NO_CHG;
      hybr_3_pref_ptr->ciot_lte_pref.lte_band_pref     = SYS_LTE_BAND_MASK_CONST_NO_CHG;
      #endif

        switch(call_ptr->ss)
        {
          case CM_SS_HYBR_2:
            if(!cmph_is_sxlte() && hybr_2_pref_ptr)
            {
              hybr_2_pref_ptr->roam_pref = favored_roam_pref;
            }
            break;

          #ifdef FEATURE_MMODE_HYBR3_ENABLED
          case CM_SS_HYBR_3:
            hybr_3_pref_ptr->roam_pref = favored_roam_pref;
            break;
          #endif
          default:
            
          break;
        }
     }
     #endif
     /* force preferences based on favored mode pref computed for the call
     */
     rescan_time = cmph_force_pref_on_the_fly2(
                                     call_ptr->ss,
                                     ph_ptr,
                                     SD_SS_PREF_REAS_REDIAL_OPTI,
                                     cmph_map_call_type_to_act_type( call_ptr->call_type ),
                                     CM_PREF_TERM_CM_1_CALL_PLUS,
                                     call_ptr->favored_mode_pref,
                                     CM_BAND_PREF_NO_CHANGE,
                                     SYS_LTE_BAND_MASK_CONST_NO_CHG,
                                     CM_BAND_PREF_NO_CHANGE,
                                     CM_PRL_PREF_NO_CHANGE,
                                     favored_roam_pref,
                                     CM_HYBR_PREF_NO_CHANGE,
                                     ph_ptr->main_stack_info.pref_info.plmn,
                                     CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                     CM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                     CM_NETWORK_SEL_MODE_PREF_NONE,
                                     hybr_2_pref_ptr,
                                     #ifdef FEATURE_MMODE_HYBR3_ENABLED
                                     hybr_3_pref_ptr,
                                     #endif
                                     call_mode_info_ptr->otasp_info.otasp_act_code,
                                     (cm_act_id_type) call_ptr->call_id,
                                     CM_ACT_UPDATE_REAS_ACT_CONT,
                                     call_ptr->force_top_pref,
                                     NULL,
                                     CM_DEFAULT_VALUE,
                                     ph_ptr->main_stack_info.pref_info.csg_id,
                                     ph_ptr->main_stack_info.pref_info.csg_rat,
                                     ph_ptr->main_stack_info.pref_info.ciot_lte_pref
                                    );

     /* Free allocated buffer */
     if (hybr_2_pref_ptr)
     {
       cm_mem_free (hybr_2_pref_ptr);
     }

     /* Free allocated buffer */
     if (hybr_3_pref_ptr)
     {
       cm_mem_free (hybr_3_pref_ptr);
     }

     return rescan_time;
  } /* if( optimized silent redial */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (no optimized silent redial) force the SS-Preference with normal
  ** silent redial.
  */
  (void) cmph_force_pref_on_the_fly(
                                    call_ptr->ss,
                                     ph_ptr,
                                     SD_SS_PREF_REAS_REDIAL_NORM,
                                     cmph_map_call_type_to_act_type( call_ptr->call_type ),
                                     CM_PREF_TERM_CM_1_CALL_PLUS,
                                     CM_MODE_PREF_NO_CHANGE,
                                     CM_BAND_PREF_NO_CHANGE,
                                     SYS_LTE_BAND_MASK_CONST_NO_CHG,
                                     CM_BAND_PREF_NO_CHANGE,
                                     CM_PRL_PREF_NO_CHANGE,
                                     CM_ROAM_PREF_NO_CHANGE,
                                     CM_HYBR_PREF_NO_CHANGE,
                                     ph_ptr->main_stack_info.pref_info.plmn,
                                     CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                     call_mode_info_ptr->otasp_info.otasp_act_code,
                                     CM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                     CM_NETWORK_SEL_MODE_PREF_NONE,
                                     (cm_act_id_type) call_ptr->call_id,
                                     CM_ACT_UPDATE_REAS_ACT_CONT,
                                     call_ptr->force_top_pref,
                                     NULL,
                                     CM_DEFAULT_VALUE,
                                     #if !defined(FEATURE_MMODE_DISABLE_CSG)
                                     ph_ptr->main_stack_info.pref_info.csg_id,
                                     #else
                                     SYS_CSG_ID_INVALID,
                                     #endif
                                     ph_ptr->main_stack_info.pref_info.csg_rat,
                                     ciot_lte_pref
                                     );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return 0;

} /* cmcall_force_sr_on_the_fly() */
#endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */



/*===========================================================================

FUNCTION cmcall_act_get

DESCRIPTION
  Indicating the action to take when call origination fails (due to
  no-service, fade, reorder, intercept, or release) in accordance with
  the currently loaded call origination plan.

DEPENDENCIES
  Origination plan must have already been loaded with
  cmcall_load_orig_plan()

RETURN VALUE
  Origination action to take, such as REDIAL, FORCE_AMPS, etc.

SIDE EFFECTS
  none

===========================================================================*/
cmcall_act_e_type  cmcall_act_get(

    const cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{
  const cmcall_orig_plan_s_type  *orig_plan_ptr = NULL;
      /* pointer to origination plan component */

  unsigned int i;
      /* loop counter */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL );
  CM_ASSERT( call_ptr->sr.orig_plan_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  orig_plan_ptr = call_ptr->sr.orig_plan_ptr;

  /* Look for a match in the origination plan action table
  ** and return the corresponding action.
  */
  for( i=0; i < orig_plan_ptr->action_table->action_table_entry_count; i++ )
  {
    if( orig_plan_ptr->action_table->action_table_entry[i].end_status == call_ptr->end_status )
    {
      return orig_plan_ptr->action_table->action_table_entry[i].action;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* No match was found - this should never happen!
  ** Return NONE.
  */
  CM_MSG_HIGH_1("NO MATCH FOUND for call end status %d",call_ptr->end_status);
  CM_ASSERT( FALSE );
  return CMCALL_ORIG_ACT_NONE;

}

#ifdef FEATURE_CALL_THROTTLE
/*===========================================================================

FUNCTION cmcall_check_if_orig_thr

DESCRIPTION
  Checks if the call origination needs to be throttled. Sets the end status
  to CM_CALL_END_ORIG_THR if the call is throttled

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(714, cmcall_check_if_orig_thr) not referenced */
/*lint -esym(528, cmcall_check_if_orig_thr) not referenced */
/*
** called when FEATURE_CALL_THROTTLE on
*/
static void cmcall_check_if_orig_thr(

  cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{
  cmss_s_type *ss_ptr      = cmss_ptr();
  boolean      is_orig_thr = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(call_ptr != NULL);
  CM_ASSERT( ss_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is a DRS=0 call, don't throttle
  */

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  if( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA &&
      !call_ptr->cmcall_mode_info.info.cdma_info.drs_bit )
  {
    return;
  }
  #endif

  /* If CM has determined that the call should go over hybrid HDR, check if
  ** orig over the currently acquired hybrid HDR system is being throttled
  */
  #ifdef FEATURE_MMODE_3GPP2
  if( ( call_ptr->ss == CM_SS_HDR ) ||
                      ( ss_ptr->info.hdr_hybrid                   &&
                        call_ptr->ss          == CM_SS_MAIN       &&
                        ss_ptr->info.sys_mode == SYS_SYS_MODE_HDR
                      )

    )
  {
    is_orig_thr = cmss_check_if_orig_thr_true(
                            ss_ptr->info.hdr_sys_id,
                            ( (call_ptr->ss == CM_SS_HDR)?
                              ss_ptr->info.hybrid_active_band:
                              ss_ptr->info.active_band),
                            ss_ptr->cmss_orig_thr_tbl_ptr,
                            call_ptr->call_type,
                            ss_ptr->info.mode_info.cdma_info.packet_zone );

  }
  /* If CM has determined that the call should go over main SS, check if
  ** orig over the currently acquired system on main stack is being throttled
  */
  else
  {
    is_orig_thr = cmss_check_if_orig_thr_true(
                            ss_ptr->info.sys_id,
                            ss_ptr->info.active_band,
                            ss_ptr->cmss_orig_thr_tbl_ptr,
                            call_ptr->call_type,
                            ss_ptr->info.mode_info.cdma_info.packet_zone );

  } /* else  call_ptr->ss == CM_SS_HDR ...*/
  #endif

  if( is_orig_thr )
  {
    call_ptr->end_status           = CM_CALL_END_ORIG_THR;
    cmcall_event( call_ptr, CM_CALL_EVENT_CALL_ORIG_THR );
    CM_MSG_HIGH_1(" Call Orig throttled call_id %d",call_ptr->call_id);
  }

  return;
}/* cmcall_check_if_orig_thr */
#if 0
/*===========================================================================

FUNCTION cmcall_check_if_orig_thr_and_add_entry

DESCRIPTION
  Checks if the call needs to be throttled. Sets the end status to
  CM_CALL_END_ORIG_THR if the call is throttled

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(714, cmcall_check_if_orig_thr_and_add_entry) not referenced */
/*lint -esym(528, cmcall_check_if_orig_thr_and_add_entry) not referenced */
/*
** called when FEATURE_CALL_THROTTLE on
*/
static void cmcall_check_if_orig_thr_and_add_entry(

  cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{

  cmss_s_type *ss_ptr      = cmss_ptr();
  boolean      is_orig_thr = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( call_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the end status is CM_CALL_END_ORIG_THR, return as the entry will
  ** already present in the table
  */

  if( call_ptr->end_status == CM_CALL_END_ORIG_THR )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is a DRS=0 call, don't add throttle entry
  */
  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  if( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA &&
     !call_ptr->cmcall_mode_info.info.cdma_info.drs_bit )
  {
    return;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_MMODE_3GPP2
  if( call_ptr->ss == CM_SS_HDR ||
                               ( ss_ptr->info.hdr_hybrid                  &&
                                 call_ptr->ss          == CM_SS_MAIN      &&
                                 ss_ptr->info.sys_mode == SYS_SYS_MODE_HDR
                               )

    )

  {
    /* Check if we need to add a entry in the origination throttle table
    ** if needed a new entry is added to the table
    ** This was a HDR call, add the hdr_sys_id to the table
    */
    is_orig_thr = cmss_check_if_orig_thr_and_add_entry(
                                                      ss_ptr,
                                                      call_ptr->call_type,
                                                      call_ptr->end_status,
                                                      TRUE );
  }/* */
  else
  #endif
  {

   is_orig_thr  = cmss_check_if_orig_thr_and_add_entry(
                                                        ss_ptr,
                                                        call_ptr->call_type,
                                                        call_ptr->end_status,
                                                        FALSE );
  }/* else */

  if( is_orig_thr )
  {
    call_ptr->end_status           = CM_CALL_END_ORIG_THR;
    cmcall_event( call_ptr, CM_CALL_EVENT_CALL_ORIG_THR );
  }
  return;
}/* cmcall_check_if_orig_thr_and_add_entry */
#endif
#endif // FEATURE_CALL_THROTTLE
#if (defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE)
/*===========================================================================

FUNCTION cmcall_hold_ps_call_origination

DESCRIPTION
  In MSIM cases, this function checks whether PS call needs to be held due to service cnf pending on other sub.
  In both SSIM and MSIM cases, it will decide the PS call should be held if manual PLMN selection is sent to NAS
  and response is not yet received.

DEPENDENCIES
  Call object must have already been initialized with cmcall_init().

RETURN VALUE
  Boolean indicating whether or not PS call origination should be held.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_hold_ps_call_origination(
  cmcall_s_type  *call_ptr  /* pointer to a call object */
)
{
  cmph_s_type*  ph_ptr = cmph_ptr();
  cmregprx_substate_e_type other_sub_state1 = CMREGPRX_SUBSTATE_NULL;
  cmregprx_substate_e_type other_sub_state2 = CMREGPRX_SUBSTATE_NULL;
  dword                    uptime               = time_get_uptime_secs();

  if(call_ptr->call_type != CM_CALL_TYPE_PS_DATA
    || cmtask_is_there_a_high_priority_activity(call_ptr->ss,CM_ACT_TYPE_DATA_CALL))
  {
    CM_MSG_LOW_1("Not holding call: call_type:%d",\
               call_ptr->call_type);
    return FALSE;
  }

  #ifdef FEATURE_MMODE_DUAL_SIM
  #ifdef FEATURE_MMODE_TRIPLE_SIM
  if((cmph_is_tsts() && call_ptr->asubs_id == ph_ptr->hybr_3_stack_info.asubs_id))
  {
    other_sub_state1 =
      cmregprx_get_info_ptr()->stack_info[CMREGPRX_GW_STACK_1].substate;

    other_sub_state2 =
      cmregprx_get_info_ptr()->stack_info[CMREGPRX_GW_STACK_2].substate;
  }
  else
  #endif
  {
    if(!cmph_is_sxlte())
    {
      if(call_ptr->asubs_id == ph_ptr->hybr_2_stack_info.asubs_id)
      {
        other_sub_state1 =
          cmregprx_get_info_ptr()->stack_info[CMREGPRX_GW_STACK_1].substate;
      }
      else
      {
        other_sub_state1 =
          cmregprx_get_info_ptr()->stack_info[CMREGPRX_GW_STACK_2].substate;
      }

      #ifdef FEATURE_MMODE_TRIPLE_SIM
      if(cmph_is_tsts())
      {
        other_sub_state2 =
         cmregprx_get_info_ptr()->stack_info[CMREGPRX_GW_STACK_3].substate;
      }
      #endif
    }
    #ifdef FEATURE_MMODE_SXLTE_G
    else
    {
      /* Here it is assumed that if SXLTE is ON & device mode is not DSDA
          ** then UE mode wouldnt be SVLTE/SGLTE. So need not bother about hybr2 stack
          */
      if(call_ptr->asubs_id == ph_ptr->hybr_3_stack_info.asubs_id)
      {
        other_sub_state1 =
          cmregprx_get_info_ptr()->stack_info[CMREGPRX_GW_STACK_1].substate;
      }
      else
      {
        other_sub_state1 =
          cmregprx_get_info_ptr()->stack_info[CMREGPRX_GW_STACK_3].substate;
      }
    }
    #endif
  }

  CM_MSG_HIGH_3("PS_CALL_HOLD: Sub_state1:%d, Sub_state2:%d, call_ptr->sr.sr_start_uptime:%d",
  	            other_sub_state1, other_sub_state2, call_ptr->sr.sr_start_uptime);
  #endif

  /* If last service request was MANUAL with type USER_PLMN_RAT, delay forcing PS call orig.
  ** This way, the PS call will not be ended on IRAT change.
  */
  if(cmregprx_get_stack_info_ptr(call_ptr->ss)->substate == CMREGPRX_SUBSTATE_WAIT_SRV_CNF &&
    cmregprx_get_stack_info_ptr(call_ptr->ss)->last_srv_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL && 
    cmregprx_get_stack_info_ptr(call_ptr->ss)->last_srv_req.parameters.manual.type == REG_CM_SERVICE_REQ_USER_PREFERRED_PLMN_RAT)
  {
    return TRUE;
  }

  /* PS CALL is originated when other SUB is waiting for SRV_CNF 
         * Hold PS call for 10sec if we did not get service on other SUB */
  if( (other_sub_state1 == CMREGPRX_SUBSTATE_WAIT_SRV_CNF ||
    other_sub_state2 == CMREGPRX_SUBSTATE_WAIT_SRV_CNF ) &&
    ( call_ptr->sr.sr_start_uptime == 0 || 
    uptime < (call_ptr->sr.sr_start_uptime + CMCALL_PS_CALL_WAIT_SRV_CNF_HOLD_ORIG_UPTIME)) )
  {
    return TRUE;
  }

  return FALSE;
}
#endif

#if 0
/*===========================================================================

FUNCTION cmcall_check_and_update_wlan_e911_settings

DESCRIPTION
  Checks call end reason
  and update the Twwan timer and e911 call attempt state accordingly.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  Given call_ptr is modified.

===========================================================================*/
void cmcall_check_and_update_wlan_e911_settings (

  cmcall_s_type    *call_ptr
    /* pointer to a call object */
)
{
  sd_ss_mode_pref_e_type sd_mode_pref = SD_SS_MODE_PREF_NONE;
      /* Mode preference */

  sys_sys_mode_e_type  sys_mode      = SYS_SYS_MODE_NONE;

  cmipapp_id_type app_id = CMIPAPP_ID_INVALID;
     /* App ID*/

  cmph_sub_config_s_type *ph_sub_config_ptr = cmph_get_sub_config_ptr(call_ptr->asubs_id);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);
  CM_ASSERT(ph_sub_config_ptr != NULL);
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#if defined (FEATURE_IP_CALL) && defined (FEATURE_CM_LTE) 
  /* Check if given call type is allowed to be originated as a ip call */
  app_id = cmipapp_find_active_app (SYS_SYS_MODE_WLAN, call_ptr->call_type);
  /* Param: Need to check if ph_ptr->ecbm_rat would be set properly, in back to back calls state 
  else need to set call state before reaching this code leg and device new mechanism */
  switch (call_ptr->end_status)
  {
    case CM_CALL_END_IP_HARD_FAILURE:
    case CM_CALL_END_IP_END:
    case CM_CALL_END_IP_FAIL:
        {
          CM_MSG_HIGH_3("E911: call_attempt_state %d, wlan_fallback_timer %d, twwan_timer %d",\
           call_ptr->sr.emerg_call_attempt_state,call_ptr->sr.wlan_fallback_timer,ph_sub_config_ptr->twwan_timer);
          if(call_ptr->sr.emerg_call_attempt_state == CMCALL_ATTEMPT_STATE_WLAN_ECBM)
          {
            call_ptr->sr.wlan_fallback_timer = time_get_uptime_secs() + ph_sub_config_ptr->twwan_timer;
            call_ptr->sr.emerg_call_attempt_state = CMCALL_ATTEMPT_STATE_WWAN_WLAN;
          }
          else if(call_ptr->sr.emerg_call_attempt_state == CMCALL_ATTEMPT_STATE_WWAN_WLAN)
          {
            call_ptr->sr.wlan_fallback_timer = CM_INVALID_WWAN_TO_WLAN_FALLBACK_TIME;
            call_ptr->sr.emerg_call_attempt_state = CMCALL_ATTEMPT_STATE_WWAN_ONLY;
          }
        }
      break;

    default:
      break;
  }
#endif
} /* cmcall_check_and_update_wlan_e911_settings */
#endif

#if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
/*===========================================================================

FUNCTION cmcall_orig_proc

DESCRIPTION
  Perform origination processing in accordance with the loaded
  call origination plan.

  During the origination process this function is being called as follows:
  1. When the call is first originated.
  2. Periodically while the origination is in process.
  3. When receiving an origination failure indication from MC, such as
     a FADE or a REORDER.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_orig_proc(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{

  cm_sr_s_type             *sr_ptr              = NULL;
      /* point at silent redial component */

  cmph_s_type              *ph_ptr              = cmph_ptr();
      /* point at phone object */

  const cmcall_orig_plan_s_type  *orig_plan_ptr = NULL;
      /* Pointer to origination plan component */

  cmcall_act_e_type        redial_act           = CMCALL_ORIG_ACT_NONE;
      /* action to be taken on silent redial for given call end */

  dword                    no_srv_time          = 0;
      /* Ballpark estimation for no-service-time due to forcing the mode */

  dword                    uptime               = time_get_uptime_secs();
      /* get the current uptime */

  cmss_s_type              *ss_ptr              = cmss_ptr();

  boolean                  is_call_ended       = FALSE;

  boolean                  is_pref_changed     = FALSE;
      /* Indication if the mode preference is changed.
      */

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
      defined( FEATURE_LTE_TO_1X )

  cdma_cmcall_type        *call_mode_info_ptr  =  NULL;
 #endif

  int                     redial_time = 0;

  #ifdef CM_GW_SUPPORTED
  cm_mode_pref_e_type      network_type        = CM_MODE_PREF_NONE;
  #endif

  sys_sys_mode_e_type      sys_mode = SYS_SYS_MODE_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr      != NULL );

  CM_ASSERT( call_ptr->sr.orig_plan_ptr != NULL );
  CM_ASSERT( ph_ptr        != NULL);
  CM_ASSERT( ss_ptr        != NULL);

  CM_MSG_HIGH_1( "cmcall_orig_proc start, uptime=%ld", uptime);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sr_ptr        = &call_ptr->sr;
  orig_plan_ptr = sr_ptr->orig_plan_ptr;

  /*If the emergency calling over Wifi feature is enabled,
  if the attempt of call over CS domain did not receive an 
  ok_to_orig in the stipulated (NV defined) time, the call
  needs to be attempted over WLAN if app available*/
  /*While doing so, the object in the priority queue for the
  CS call needs to be removed*/
  #ifdef FEATURE_MMODE_EMERG_SUPPORT 
  if(cmph_get_sub_config_ptr(call_ptr->asubs_id)->wifi_e911_support &&
     call_ptr->call_type == CM_CALL_TYPE_EMERGENCY &&
     cmcall_is_active_ip_app(SYS_SYS_MODE_WLAN, CM_CALL_TYPE_EMERGENCY))
  {
    /*Timer has expired and ok_to_orig is still not received also  
    there could be race condition between ok to orig and waiting for 
    next orig uptime to expire. In that case CM should wait for 
    next orig uptime to expire and send orig to cellular first*/
    if((uptime >= sr_ptr->fall_back_to_wifi_from_cs) && 
        (call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE)&&
        !((call_ptr->sr.is_ok_to_orig == TRUE) && (uptime <= sr_ptr->next_orig_uptime) )
      )
    {
      /* add print here*/ 
      
      /*set vowlan flag to TRUE*/ 
      call_ptr->is_vowlan = TRUE;
      call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_IP;
      /* since call is going over WLAN, we can mark sys_mode also as WLAN. */
      call_ptr->sys_mode = SYS_SYS_MODE_WLAN;
      /*set hold orig up time */
      sr_ptr->hold_orig_uptime = uptime;

      /*remove the cs preferences if any as WLAN call does not have any preferences on queue*/
      cmph_remove_act_id_and_force_orig_mode(call_ptr->ss,(cm_act_id_type)call_ptr->call_id, TRUE);
   }
  }

  /* if WLAN e911 support is there, and WLAN attempt can be made, then route
  ** call to IMS-WLAN, move CM and SD out of emergency mode */
  if(cmph_get_sub_config_ptr(call_ptr->asubs_id)->is_wlan_e911_call_supp && 
     call_ptr->call_type == CM_CALL_TYPE_EMERGENCY &&     
     cmcall_is_active_ip_app(SYS_SYS_MODE_WLAN, CM_CALL_TYPE_EMERGENCY))
  {
    /*Timer has expired and ok_to_orig is still not received*/
    if((uptime >= sr_ptr->wlan_fallback_timer) && 
      (sr_ptr->emerg_call_attempt_state == CMCALL_ATTEMPT_STATE_WWAN_WLAN)&&
        (call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE))
    {
      /* add print here*/ 
      CM_MSG_HIGH_1("E911: Routing call %d to WLAN",call_ptr->call_id);
      /*set vowlan flag to TRUE*/ 
      call_ptr->is_vowlan = TRUE;
      call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_IP;

      /*set hold orig up time */
      sr_ptr->hold_orig_uptime = uptime;

      /*remove the cs preferences if any as WLAN call does not have any preferences on queue
      Unforce preferences from SD as well */
      cmph_remove_act_id_and_force_orig_mode(call_ptr->ss,(cm_act_id_type)call_ptr->call_id, TRUE);
   }
  }
  #endif

  /*- - - -  - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is the first origination attempt, set the
  ** silent redial start time to the current uptime
  */
  if( sr_ptr->sr_start_uptime == 0 )
  {
    sr_ptr->sr_start_uptime = uptime;
    sr_ptr->sr_max_uptime   = uptime + orig_plan_ptr->sr_max_time;

    CM_MSG_LOW_1( "Silent redial starting, uptime=%ld",
                sr_ptr->sr_start_uptime);

    #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
    cmcall_voip_redial_init( call_ptr );
    #endif

  }
  
  /* If we did not force the origination mode yet, check whether we need to
  ** shorten the hold orig period as follows:
  */
  if( call_ptr->orig_mode == SD_SS_ORIG_MODE_NONE )
  {
    /* If preferences havent yet been forced and waitinf for srv cnf on other sub
       ** then just return from here
      */
    #ifdef FEATURE_MMODE_HYBR3_ENABLED
    if(call_ptr->ss == CM_SS_HYBR_3)
    {
      sys_mode = cmss_ptr()->info.gw3_sys_mode;
    }
    else 
    #endif
    #ifdef FEATURE_MMODE_HYBR2_ENABLED
    if(call_ptr->ss == CM_SS_HYBR_2)
    {
      sys_mode = cmss_ptr()->info.gw_sys_mode;
    }
    else
    #endif
    {
      sys_mode = cmss_ptr()->info.sys_mode;
    }

    /* If we have service and are not looking for more preferred service, end
    ** the hold orig period now.
    ** OR 'is_vowlan' flag is set to TRUE.
    */
    if( call_ptr->is_vowlan == TRUE || ( ( cmcall_is_ok_to_end_hold_orig_period(call_ptr) )
       ))
    {
      sr_ptr->hold_orig_uptime = uptime;
      CM_MSG_HIGH_1( "Ending hold-orig period, uptime=%ld", uptime);
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else, if we have sufficient service to originate, only give a limited
    ** amount of additional time for the preferred service to be acquired.
    */
    else if( cmcall_is_sufficient_srv( call_ptr,
                                       call_ptr->favored_mode_pref,
                                       sys_mode
                                     )
           )
    {
      sr_ptr->hold_orig_uptime = MIN(sr_ptr->hold_orig_uptime,
                                     uptime+CMCALL_PREF_SRV_HOLD_ORIG_TIME );

      CM_MSG_HIGH_2( "Shortening hold-orig period %ld, uptime=%ld",
                   sr_ptr->hold_orig_uptime, uptime);
    }
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( call_ptr->end_status == CM_CALL_END_NO_FULL_SRV )
  {
    sr_ptr->hold_orig_uptime  = uptime;
    sr_ptr->next_orig_uptime  = uptime;
    CM_MSG_HIGH_0( "No_full_srv ");
  }

  /*- - - -  - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we are still in the hold-orig period and already received the ok to
  ** originate indication, end the hold-orig period now.
  */
  if(( uptime < sr_ptr->hold_orig_uptime &&
       cmcall_is_ok_to_orig(call_ptr) )
    )
  {
    sr_ptr->hold_orig_uptime = uptime;
    CM_MSG_HIGH_1( "Ending hold-orig period, uptime=%ld", uptime);
  }


  /* Update the redial action for VOIP call.
  */
  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE  
  cmcall_voip_redial_action_update( call_ptr, uptime);
  #if 0
  if(call_ptr->call_type == CM_CALL_TYPE_EMERGENCY && cmph_get_sub_config_ptr(call_ptr->asubs_id)->is_wlan_e911_call_supp)
  {
    cmcall_check_and_update_wlan_e911_settings(call_ptr);
  }
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(  call_ptr->end_status == CM_CALL_END_IP_FAIL )
  {
    /* End the hold orig uptime.
    */
    if(sr_ptr->hold_orig_uptime > uptime)
    {
      sr_ptr->hold_orig_uptime = uptime;

      CM_MSG_HIGH_3( "VOIP SR call id %d Ending hold orig %ld uptime %ld",
                  call_ptr->call_id, sr_ptr->hold_orig_uptime, uptime );
    }

    /* Shorten the next orig uptime.
    */
    if( sr_ptr->next_orig_uptime > uptime )
    {
      sr_ptr->next_orig_uptime = uptime;

      CM_MSG_HIGH_3( "VOIP SR call id %d Ending next orig %ld uptime %ld",
                  call_ptr->call_id, sr_ptr->next_orig_uptime, uptime );

    }
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we are still in the hold-orig period, return now.
  */
  if( uptime < sr_ptr->hold_orig_uptime )
  {
    CM_MSG_HIGH_2("Still in HO period, ho_uptime=%ld, uptime=%ld",
                 sr_ptr->hold_orig_uptime, uptime);
    return;
  }

  /*-------------------------------------------------------------------------
               If we got here, the hold-orig period is over.
  -------------------------------------------------------------------------*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If we did not force the origination mode yet, do it now.
  ** No forcing the preferences, if the ORIG SYS_MODE is WLAN
  */
  if ( call_ptr->orig_mode  == SD_SS_ORIG_MODE_NONE &&
      call_ptr->end_status == CM_CALL_END_NONE)
  {

    no_srv_time = cmcall_force_pref_on_the_fly
                  ( call_ptr,
                    SD_SS_PREF_REAS_ORIG_START_CS
                  );

      /* If forcing to mode preference is causing a short period of
      ** no service (during which MC is acquiring the new service),
      ** hold the origination for the appropriate amount of time.
      */
      sr_ptr->hold_orig_uptime = MAX( sr_ptr->hold_orig_uptime,
                                      uptime + no_srv_time );

      CM_MSG_HIGH_2( "Hold orig=%d, uptime=%d",
                    sr_ptr->hold_orig_uptime, uptime);

    return;

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we are currently waiting for the next silent redial period, return
  ** now.
  */
  if( uptime < sr_ptr->next_orig_uptime )
  {
    CM_MSG_HIGH_3("Wait for SR period=%ld, uptime=%ld, OK=%d",
                sr_ptr->next_orig_uptime, uptime, sr_ptr->is_ok_to_orig );
    return;
  }

  /*- - - -  - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If call end status is none, do the following:
  **
  ** For GSM/WCDMA
  **
  **   - If NOT ok_to_orig, fail the origination attempt and wait
  **     for the service to appear. Enter into Silent Redial.
  **
  **
  ** For CM_CALL_MODE_INFO_IP
  **
  **   - Call mode is set to IP only when ok_to_orig is recvd from HDR
  **     or WLAN system. Other info_type's are set even before waiting for
  **     ok_to_orig. So "if check" is not required for INFO_IP.
  */

  if( call_ptr->end_status == CM_CALL_END_NONE 
    && ! cmcall_is_ok_to_orig(call_ptr)
    && call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE)
  {
    call_ptr->end_status = CM_CALL_END_NO_GW_SRV;
  } /* if( call_ptr->end_status == CM_CALL_END_NONE ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If origination attempt did not fail yet, send origination as necessary
  ** and return.
  ** The origination has not failed if the end status is NONE
  ** and
  ** either we got ok to orig
  ** or
  ** the call origination is for a GW call and the call origination is
  ** alredy sent to the lower layers ( NAS )
  */

  if( ( call_ptr->end_status == CM_CALL_END_NONE )
      &&
      (
        ( cmcall_is_ok_to_orig(call_ptr) )
        ||
        ( ( ( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS )
            ||
            ( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS )
            ||
            ( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP )
          )
          &&
          ( call_ptr->sr.orig_sent_on_mode != SYS_SYS_MODE_NONE )
        )
      )
    )
  {

    /* If origination was already sent, return now.
    */
    if( call_ptr->sr.orig_sent_on_mode != SYS_SYS_MODE_NONE )
    {
      CM_MSG_HIGH_1("Orig already sent on mode = %d",call_ptr->sr.orig_sent_on_mode);
      return;
    }
 
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* If origination command was not already sent and it is ok to originate,
    ** send the origination now.
    */
    if( call_ptr->end_status != CM_CALL_END_ORIG_THR )
    {
      CM_MSG_HIGH_1( "Send orig cmd, uptime=%ld", uptime);

      if (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP)
      {
        #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
        /* Since info_type is IP and OK_TO_ORIG has been received, call is
        ** sent to ip origination.
        */
        cmcall_send_ipcall_orig (call_ptr);
        #endif

      }
      else
      {
        #if defined(CM_GW_SUPPORTED)

        /* If call is initiated on non 3GPP systems, CM will not allocate MN connection id as it is GW speific.
             ** Later during silent redial if the call is placed on GW, CM should allocated connection id and
             ** and send origination request to MN.
             */
        #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
        if ( (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS) &&
             ( (CM_CALL_TYPE_EMERGENCY == call_ptr->call_type) ||
               (CM_CALL_TYPE_CS_DATA == call_ptr->call_type) ||
               (CM_CALL_TYPE_VOICE == call_ptr->call_type)
             )
           )
        {
          /* Allocate connection id if it is not done already*/
          if(!BETWEEN((int8)call_ptr->gw_connection_id, -1, (int)UNKNOWN_CONNECTION_ID))
          {
            if (!cmcall_generate_connection_id(call_ptr))
            {
              CM_ERR_1("cmcall_orig_proc: Can not set connection Id Info",call_ptr->call_id);
            }
          }
        }
        #endif

        cmwcall_send_orig( call_ptr );
        #endif //     #ifdef CM_GW_SUPPORTED
      } /* else */

      return;

    } /* call_ptr->end_status != CM_CALL_END_ORIG_THR */

  }/* call_ptr->end_status == CM_CALL_END_NONE */

  /*-------------------------------------------------------------------------
                 If we got here, origination attempt failed
  -------------------------------------------------------------------------*/

  #if 0

  /* Check if call throttle table needs to be modified
  */
  cmcall_check_if_orig_thr_and_add_entry( call_ptr );

  #endif /*FEATURE_CALL_THROTTLE*/

  /* Figure out what to do next now that origination attempt failed */
  redial_act = cmcall_act_get(call_ptr);

  CM_MSG_HIGH_3( "SRFailed EndStatus=%d Redial act=%d, call_id=%d",
               call_ptr->end_status, redial_act, call_ptr->call_id );

  /* Take the next step as indicated by the origination plan.
  */
  switch( redial_act )
  {
    case CMCALL_ORIG_ACT_NONE:

      /* We found no action in silent redial table for the received
         call end status.
         This is an error condition and should never happen.
         But if it does, flag an error and end the call.
      */
      CM_ERR_1("No action found for call end status %d", call_ptr->end_status);
      call_ptr->end_status = CM_CALL_END_INTERNAL_ERROR;
      cmcall_end( call_ptr );

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CMCALL_ACT_END_ORIG:

      /* End the call origination.
      */
     /* Incase number is emergency on other sub, return perm redial cause*/

      #if defined(CM_GW_SUPPORTED)
      if (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS )
      {

        /*  clear the SR if call state is still origination.
        */
        if ( ( sr_is_more_redial(sr_ptr, uptime ) )&&
             ( call_ptr->call_state == CM_CALL_STATE_ORIG)
           )
        {
          call_ptr->sr.sr_left = 0;
        }

        /* If the call is not in idle state (e.g. due to a race condition),
        ** first end the call then start the new call.
        */
        if( (call_ptr->call_state != CM_CALL_STATE_IDLE) &&
            (call_ptr->call_subst.idle != CM_CALL_IDLE_SUBST_ENDWAIT) &&
            (call_ptr->sr.orig_sent_on_mode != SYS_SYS_MODE_NONE)
           )
        {
          cmwcall_end_req(call_ptr);
        }
        else
        {
          /* End the call rightaway.
          */
          cmcall_end( call_ptr );
        }

        /* Wait for the final end indication from the lower layer */
      }
      else
      #endif  /* FEATURE_GSM || FEATURE_WCDMA */
      {
        cmcall_end( call_ptr );
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CMCALL_ACT_REDIAL:
    case CMCALL_ACT_REDIAL_SHORT:

      /* If no more silent redials are left, end the call origination.
      */
      if( ! sr_is_more_redial(sr_ptr, uptime) )
      {

        #if defined (CM_GW_SUPPORTED)
        if (call_ptr->cmcall_mode_info.info_type != CM_CALL_MODE_INFO_IP)
        {
          cmwcall_end_req ( call_ptr );
        }
        #endif  /* FEATURE_GSM || FEATURE_WCDMA */
        cmcall_end( call_ptr );

        break;
      }

      /* 1. Reset call end status to none.
      ** 2. Force the mode preference. (to get the ok_to_orig on more time).
      ** 3. Schedule next silent redial.
      */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      {
        (void) cmcall_force_pref_on_the_fly( call_ptr,
                                             SD_SS_PREF_REAS_REDIAL_NORM );
        
        if(redial_act == CMCALL_ACT_REDIAL_SHORT)
        {
          redial_time = 1;
        }
        else
        {
          redial_time = orig_plan_ptr->sr_period_time;
        }
        
        sr_sched_next( sr_ptr,
                       redial_time,                              /* SR off period */
                       orig_plan_ptr->sr_period_time,  /* Hold orig period */
                       CM_SR_ACT_DEC,
                       call_ptr->end_status,
                       redial_act
                     );
        is_call_ended = FALSE;
      }

      /*lint -save -e774 Boolean within 'if' always evaluates to True
      **                 Not true, Depending on feature flags it might
      **                 not be true always
      */

      if( !is_call_ended )
      {
        call_ptr->end_status = CM_CALL_END_NONE;
      }

      /*lint -restore */

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CMCALL_ACT_WAIT_OK_TO_ORIG:

      /* 1. Reset call end status to none.
      ** 2. Schedule next silent redial without decrementing SRLeft.
      */

      sr_sched_next( sr_ptr,
                     1,                              /* SR off period */
                     CMCALL_ACC_HOLD_ORIG_TIME,      /* Hold orig period */
                     CM_SR_ACT_NO_CHANGE,
                     call_ptr->end_status,
                     redial_act
                   );
      call_ptr->end_status = CM_CALL_END_NONE;


      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if defined(CM_GW_SUPPORTED)
    case CMCALL_ACT_PROC_END_REASON:
      /*
      ** Try to redial when there is a MM or AS reject cause listed below,
      ** which indicate that there is a potential that the next call might go through.
      ** If the reject cause is not present or not listed below, just end the call.
      */
      {

       cmcall_process_act_proc_end_reason (call_ptr,redial_act);

      }
      break;

    #endif  /* FEATURE_GSM || FEATURE_WCDMA */
    #if (defined(FEATURE_IP_CALL) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)) || defined(FEATURE_HDR_HYBRID)
    case CMCALL_ACT_FORCE_NON_IP:
    {

      sd_ss_mode_pref_e_type  mode_to_remove = SD_SS_MODE_PREF_HDR_LTE;

      /* If no more silent redials are left, end the call origination.
      */
      if( ! sr_is_more_redial(sr_ptr, uptime) )
      {
        cmcall_end( call_ptr );
        break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Send abort IP orig to IMS if we already sent orig before
      */
      #if defined(FEATURE_IP_CALL) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)      
      if( cmcall_voip_sr_is_abort( call_ptr ) &&
          call_ptr->is_ip_orig_sent )
      {
        cmipcall_send_abort( call_ptr, CMIPAPP_ABORT_CAUSE_CS_FALLBACK );
      }
      #endif


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* 1. Reset call end status to none.
      ** 2. Use CDMA or GW for orig.
      */

      cmcall_process_orig_mode(call_ptr->main_ss_mode, call_ptr);
      /* Update call orig mode during redial */
      call_ptr->orig_mode =
                       cmph_map_cm_call_type_to_sd_orig_mode( call_ptr->call_type,
                                                              call_ptr->ss );

      /* csfb_type could be updated during orig mode process */


      #if defined (FEATURE_CM_LTE) && defined (FEATURE_IP_CALL)
      if(call_ptr->sr.voip_sr.state == CMCALL_VOIP_SR_STATE_FALLBACK_CS)
      {
          cm_mode_pref_e_type temp_new_mode_pref;
          
          temp_new_mode_pref = cm_remove_mode_pref_components(
                                 call_ptr->favored_mode_pref,
                                 1,
                                 mode_to_remove);


         if (call_ptr->call_type != CM_CALL_TYPE_EMERGENCY)         
         {
         
          cm_orig_q_s_type        *p_orig_ptr = NULL;
          cmmsc_state_machine_s_type   *state_machine_ptr = cmmsimc_state_machine_ptr(call_ptr->asubs_id);
          sd_ss_mode_pref_e_type        temp_policy_man_ue_mode_in_sd_mode_pref, temp_new_mode_pref_in_sd_mode_pref;
          
          temp_policy_man_ue_mode_in_sd_mode_pref = cmph_map_sys_mode_mask_to_sd_mode_pref(state_machine_ptr->stack_common_info.ue_mode_capability);
          temp_new_mode_pref_in_sd_mode_pref  = cmph_map_cm_mode_pref_to_sd_mode_pref(temp_new_mode_pref);



         p_orig_ptr = cmtask_orig_para_search_act_id(call_ptr->ss,(cm_act_id_type)call_ptr->call_id);
         CM_ASSERT(p_orig_ptr != NULL);
             /* IF new mode pref after removal of the last tried mode has no mode left to try.
                       *    OR
                       * IF new mode pref after removal of the last tried mode and the device modes have no mode in common,
                       * End the call.
                       */
                 if( (temp_new_mode_pref == CM_MODE_PREF_NONE)||
                     (sd_misc_is_mode_pref(temp_policy_man_ue_mode_in_sd_mode_pref,temp_new_mode_pref_in_sd_mode_pref) == FALSE)||
                     ( p_orig_ptr != NULL && sd_misc_is_target_supp_mode_band(
                         cmph_map_cm_mode_pref_to_sd_mode_pref(temp_new_mode_pref),
                         cmph_map_cm_band_pref_to_sd_band_pref(p_orig_ptr->orig->orig_band_pref),
                         p_orig_ptr->orig->orig_lte_band_pref,
                         cmph_map_cm_tds_band_pref_to_sd_tds_band_pref(p_orig_ptr->orig->orig_tds_band_pref)
                       ) == FALSE ))

           {
             CM_MSG_HIGH_1("New mode_pref & Policy_man UE mode pref = {NULL}; ending the call, %d",temp_new_mode_pref);
             cmcall_end( call_ptr );
             break;
           }
         }            
         #ifdef FEATURE_MMODE_EMC_SUPPORT
         else if (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
         {

          /* IF new mode pref after removal of the last tried mode has no mode left to try.
                    *    OR
                    * IF new mode pref after removal of the last tried mode and the device modes have no mode in common,
                    * End the call.
                    */
           if( (temp_new_mode_pref == CM_MODE_PREF_NONE)||
               ( sd_misc_is_target_supp_mode_band(
                                 temp_new_mode_pref,
                                 cmph_map_cm_band_pref_to_sd_band_pref(CMPH_SS_BAND_PREF(call_ptr->asubs_id, ph_ptr)),
                                 CMPH_SS_LTE_BAND_PREF(call_ptr->asubs_id, ph_ptr),
                                 cmph_map_cm_tds_band_pref_to_sd_tds_band_pref(CMPH_SS_TDS_BAND_PREF(call_ptr->asubs_id, ph_ptr))
                                            ) == FALSE ))
           {        
             CM_MSG_HIGH_1("No CS mode in New mode_pref OR New Mode pref & device modes = {NULL}; ending the call, %d",temp_new_mode_pref);
             cmcall_end( call_ptr );
             break;
           }
         }
         #endif
       }

      #endif
      

      {
        cmcall_remove_mode_and_force_and_sched_sr(call_ptr);

      }
#if defined(FEATURE_IP_CALL)
      /* Send intermediate event to the upper layers informing call has been moved to CS Domain
      */
      if (cmcall_check_if_end_status_to_propagate(call_ptr->end_cause_to_client))
      {
       cm_call_end_e_type   end_status_temp = call_ptr->end_status;
       CM_MSG_MED_2("Sending VOIP_CALL_END_CNF with end_status: %d, orig_end_status: %d",
                     call_ptr->end_cause_to_client, end_status_temp);
       call_ptr->end_status = call_ptr->end_cause_to_client;
       cmcall_event( call_ptr, CM_CALL_EVENT_VOIP_CALL_END_CNF );
       call_ptr->end_status = end_status_temp;
      }
#endif 
      break;
    }
    #endif

    #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
    case CMCALL_ACT_HARD_FAILURE:
      if( ! sr_is_more_redial(sr_ptr, uptime) )
      {
        cmcall_end( call_ptr );
        break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      cmcall_redial_after_hard_failure(call_ptr,&no_srv_time);
      /* Schedule the next SR.
      ** If the preference is changed, shorten the SR off period.
      ** Otherwise, use the value from the origination table.
      */
      sr_sched_next( &call_ptr->sr,
                     ((call_ptr->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG)?
                     1:orig_plan_ptr->sr_period_time), /* SR off period */
                      no_srv_time,                     /* Hold orig period */
                      CM_BOOLEAN(no_srv_time)?CM_SR_ACT_RESTART:CM_SR_ACT_DEC,
                      call_ptr->end_status,
                      redial_act
                   );
      call_ptr->end_status = CM_CALL_END_NONE;
      break;
    #endif
        

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
    case CMCALL_ACT_FORCE_TO_IP:

      /* If no more silent redials are left, end the call origination.
      */
      if( ! sr_is_more_redial(sr_ptr, uptime) )
      {
        cmcall_end( call_ptr );
        break;
      }
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /*
      ** 1. Decide whether to redial on voip and force origination on
      ** appropriate stack
      ** 2. If voip is not available, redial on main stack
      ** 3. Schedule next SR period and reset call end status to none.
      */
      
      no_srv_time = cmcall_voip_redial(call_ptr);

      /* Schedule the next SR.
      **
      ** If the call is emergency call, shorten the SR off period to 1.
      ** Otherwise, use the value form the origination table.
      */
      sr_sched_next( &call_ptr->sr,
                     ((call_ptr->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG)?
                      1:orig_plan_ptr->sr_period_time), /* SR off period */
                     orig_plan_ptr->sr_period_time,  /* Hold orig period */
                     CM_SR_ACT_DEC,
                     call_ptr->end_status,
                     redial_act
                   );
      call_ptr->end_status = CM_CALL_END_NONE;

      break;
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CMCALL_ORIG_ACT_MAX:
    default:
      CM_ERR_1("Bad redial_act = %d",(uint8) redial_act);
      /*
      ** 527: Unreachable
      ** This is because CM_ERR() evaluates to error fatal
      ** (i.e exit program)
      ** But for different feature set it may not to, so we want to
      ** supress the warning here
      */
      /*lint -save -e527 */
      cmcall_end( call_ptr );
      /*lint -restore */
      break;
  } /* switch( cmcall_act_get(call_ptr) ) */

} /* cmcall_orig_proc() */



/*===========================================================================

FUNCTION cmcall_load_orig_plan

DESCRIPTION
  Load an origination plan in accordance with the call type and/or
  a particular dialed number we are trying to originate.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_load_orig_plan(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{

  cm_sr_s_type             *sr_ptr        = NULL;
      /* point silent redial component */

  unsigned int i;
      /* loop counter */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Call origination plan table.
  **
  ** This table indicate the SR parameters and origination action that
  ** should be used for the various call types and or dial strings.
  */
  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  static const cmcall_action_table_entry_s_type e911_action_table_entry[] = {

        { CM_CALL_END_FADE,                    CMCALL_ACT_REDIAL              },
        { CM_CALL_END_REL_NORMAL,              CMCALL_ACT_REDIAL              },
        { CM_CALL_END_NO_GW_SRV,               CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_NETWORK_END,             CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_NO_FULL_SRV,             CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_LL_CAUSE,                CMCALL_ACT_PROC_END_REASON     },
        { CM_CALL_END_IP_FAIL,                 CMCALL_ACT_FORCE_NON_IP        },
        { CM_CALL_END_IP_RETRY,                CMCALL_ACT_FORCE_TO_IP         },
        { CM_CALL_END_IP_END,                  CMCALL_ACT_FORCE_NON_IP        },
        { CM_CALL_END_IP_HARD_FAILURE,         CMCALL_ACT_HARD_FAILURE        }
  };

  static const cmcall_orig_plan_action_table_s_type e911_action_table = {
     ARR_SIZE(e911_action_table_entry),
     (cmcall_action_table_entry_s_type*) e911_action_table_entry
  };
#endif



#if ( ( defined(FEATURE_MMODE_CDMA_800) && defined(FEATURE_MMODE_CDMA_1900) ) || \
      ( defined (FEATURE_CM_TRIBAND_ORIG_PLAN_TBL) ) || \
      ( defined (FEATURE_MMODE_LTE_ONLY) ))

  static const cmcall_action_table_entry_s_type trimode_action_table_entry [] = {
        { CM_CALL_END_FADE,                    CMCALL_ACT_FORCE_AUTO_RSSI     },
        { CM_CALL_END_REL_NORMAL,              CMCALL_ACT_END_ORIG            },
         { CM_CALL_END_NO_GW_SRV,               CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_NETWORK_END,             CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_NO_FULL_SRV,             CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_LL_CAUSE,                CMCALL_ACT_PROC_END_REASON     },
        { CM_CALL_END_IP_FAIL,                 CMCALL_ACT_FORCE_NON_IP        },
        { CM_CALL_END_IP_RETRY,                CMCALL_ACT_FORCE_TO_IP         },
        { CM_CALL_END_IP_END,                  CMCALL_ACT_END_ORIG            }
  };

  static const cmcall_orig_plan_action_table_s_type trimode_action_table = {
    ARR_SIZE(trimode_action_table_entry),
    (cmcall_action_table_entry_s_type*) trimode_action_table_entry
  };

#elif (defined CM_GW_SUPPORTED) && !defined (FEATURE_MMODE_CDMA_800) && !defined (FEATURE_MMODE_CDMA_1900)
  static const cmcall_action_table_entry_s_type gw_action_table_entry [] = {
        { CM_CALL_END_FADE,                    CMCALL_ACT_PROC_END_REASON  },
        { CM_CALL_END_REL_NORMAL,              CMCALL_ACT_END_ORIG         },
        { CM_CALL_END_NO_GW_SRV,               CMCALL_ACT_REDIAL_SHORT     },
        { CM_CALL_END_NETWORK_END,             CMCALL_ACT_END_ORIG         },
        { CM_CALL_END_NO_FULL_SRV,             CMCALL_ACT_END_ORIG         },
        { CM_CALL_END_LL_CAUSE,                CMCALL_ACT_PROC_END_REASON  },
        { CM_CALL_END_IP_RETRY,                CMCALL_ACT_FORCE_TO_IP      },
        { CM_CALL_END_IP_FAIL,                 CMCALL_ACT_FORCE_NON_IP     },
        { CM_CALL_END_IP_END,                  CMCALL_ACT_END_ORIG         }
   };

  static const cmcall_orig_plan_action_table_s_type gw_action_table = {
    ARR_SIZE(gw_action_table_entry),
    (cmcall_action_table_entry_s_type*) gw_action_table_entry
  };
#else

  #if !defined (FEATURE_GSM) && !defined (FEATURE_WCDMA) && !defined (FEATURE_JCDMA)

  static const cmcall_action_table_entry_s_type non_trimode_action_table_entry [] = {
        { CM_CALL_END_FADE,                    CMCALL_ACT_REDIAL              },
        { CM_CALL_END_REL_NORMAL,              CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_NO_GW_SRV,               CMCALL_ACT_REDIAL_SHORT        },
        { CM_CALL_END_NETWORK_END,             CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_NO_FULL_SRV,             CMCALL_ACT_END_ORIG            },
        { CM_CALL_END_LL_CAUSE,                CMCALL_ACT_PROC_END_REASON     },
        { CM_CALL_END_IP_FAIL,                 CMCALL_ACT_FORCE_NON_IP        },
        { CM_CALL_END_IP_RETRY,                CMCALL_ACT_FORCE_TO_IP         },
        { CM_CALL_END_IP_END,                  CMCALL_ACT_END_ORIG            }
  };

  static const cmcall_orig_plan_action_table_s_type non_trimode_action_table = {
    ARR_SIZE(non_trimode_action_table_entry),
    (cmcall_action_table_entry_s_type*) non_trimode_action_table_entry
  };

  #endif /* !FEATURE_GSM && !FEATURE_WCDMA && !FEATURE_JCDMA */

#endif  /* FEATURE_MMODE_CDMA_800 && FEATURE_MMODE_CDMA_1900 */

  static cmcall_orig_plan_s_type orig_plan_tbl[] = {

    #ifdef FEATURE_MMODE_EMC_SUPPORT
    /* E911 call origination plan.
    */
    {
      /* Condition */
      CM_FEATURE_NONE,               /* SR with JCDMA feature (don't care) */
      CM_CALL_TYPE_EMERGENCY,             /* call type (E911)*/
      NULL,                          /* particular dial string (none) */
      FALSE,                         /* hybrid call orig for pkt data call is
                                        allowed or not */
      CM_CALL_MODE_INFO_NONE,        /* Ignore the call mode */
      FALSE,                         /*is emerg on other sub*/
      
      /* Origination plan */
      TRUE,                          /* honor MC hold-orig */
      0,                             /* power-up hold orig uptime */
      CMCALL_SR_PERIOD_TIME,         /* silent redial off period */
      1000000,                       /* max numbers of dial attempts */
      4000000,                       /* max time for silent redial sequence */
      (cmcall_orig_plan_action_table_s_type*) &e911_action_table
    },

      /* E911 call origination plan for multi-sim
      */
      {
        /* Condition */
        CM_FEATURE_NONE,                    /* SR with JCDMA feature (don't care) */
        CM_CALL_TYPE_EMERGENCY,             /* call type (E911)*/
        NULL,                               /* particular dial string (none) */
        FALSE,                              /* hybrid call orig for pkt data call is
                                          allowed or not */
        CM_CALL_MODE_INFO_NONE,             /* Ignore the call mode */
        TRUE,                               /*is emerg on other sub*/
        
        /* Origination plan */
        TRUE,                               /* honor MC hold-orig */
        0,                                  /* power-up hold orig uptime */
        CMCALL_SR_PERIOD_TIME,              /* silent redial off period */
        1000000,                            /* max numbers of dial attempts */
        CMCALL_SR_EMERGENCY_MAX_TIME_MSIM,  /* max time for silent redial sequence */
        (cmcall_orig_plan_action_table_s_type*) &e911_action_table
      },
      #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if defined (FEATURE_MMODE_CDMA_800)|| defined (FEATURE_MMODE_CDMA_1900)
    /* Standard OTASP call origination plan for targets with System
    ** Determination 2.0 support.
    */
    {
      /* Condition */
      CM_FEATURE_NONE,               /* SR with JCDMA feature (don't care) */
      CM_CALL_TYPE_STD_OTASP,        /* call type ( standard OTASP)*/
      NULL,                          /* particular dial string (none) */
      FALSE,                         /* hybrid call orig for pkt data call is
                                        allowed or not */
      CM_CALL_MODE_INFO_NONE,        /* Ignore the call mode */
      FALSE,                         /*is emerg on other sub*/

      /* Origination plan */
      FALSE,                         /* Honor MC hold-orig */
      0,                             /* power-up hold orig uptime */
      CMCALL_SR_PERIOD_TIME,         /* silent redial off period */
      1000000,                       /* max numbers of dial attempts */
      4000000,                       /* max time for silent redial sequence*/
      (cmcall_orig_plan_action_table_s_type*) &otasp_action_table
    },

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_OTASP_2

    /* Special "*2" none-standard OTASP call origination plan
    ** for silent redial with AMPS.
    */
    {
      /* Condition */
      CM_FEATURE_ON,                 /* SR with JCDMA feature (ON) */
      CM_CALL_TYPE_NON_STD_OTASP,    /* call type (any) */
      "*2",                          /* particular dial string ("*2") */
      FALSE,                         /* hybrid call orig for pkt data call is
                                        allowed or not */
      CM_CALL_MODE_INFO_NONE,        /* Ignore the call mode */
      FALSE,                         /*is emerg on other sub*/
          

      /* Origination plan */
      TRUE,                          /* Honor MC hold-orig */
      CMCALL_PWRUP_HOLD_ORIG_UPTIME, /* power-up hold orig uptime */
      CMCALL_SR_PERIOD_TIME,         /* silent redial off period */
      CMCALL_SR_MAX_DIALS,           /* max numbers of dial attempts */
      CMCALL_SR_MAX_TIME,            /* max time for silent redial sequence*/
      (cmcall_orig_plan_action_table_s_type*) &otasp2_action_table          },
    #endif /* FEATURE_OTASP_2 */


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Force to collocated HDR call origination plan
    **
    */
    {
      /* Condition */
      CM_FEATURE_NONE,               /* SR with JCDMA feature NONE */
      CM_CALL_TYPE_PKT_DATA,          /* call type (any) */
      NULL,                          /* particular dial string(none) */
      TRUE,                          /* hybrid call orig for pkt data call
                                        is allowed or not */
      CM_CALL_MODE_INFO_GW_PS,        /* Ignore the call mode */
      FALSE,                         /*is emerg on other sub*/
            

      /* Origination plan */
      TRUE,                          /* Honor MC hold-orig */
      CMCALL_PWRUP_HOLD_ORIG_UPTIME, /* power-up hold orig uptime */
      CMCALL_SR_PERIOD_TIME,         /* silent redial off period */
      CMCALL_SR_MAX_DIALS,           /* max numbers of dial attempts */
      CMCALL_PS_DATA_SR_MAX_TIME,    /* max time for silent redial sequence*/
      (cmcall_orig_plan_action_table_s_type*) &trimode_action_table },


    /* Force to collocated HDR call origination plan
    **
    */
    {
      /* Condition */
      CM_FEATURE_NONE,               /* SR with JCDMA feature NONE */
      CM_CALL_TYPE_PKT_DATA,          /* call type (any) */
      NULL,                          /* particular dial string(none) */
      TRUE,                          /* hybrid call orig for pkt data call
                                        is allowed or not */
      CM_CALL_MODE_INFO_NONE,        /* Ignore the call mode */
      FALSE,                         /*is emerg on other sub*/
            

      /* Origination plan */
      TRUE,                          /* Honor MC hold-orig */
      CMCALL_PWRUP_HOLD_ORIG_UPTIME, /* power-up hold orig uptime */
      CMCALL_SR_PERIOD_TIME,         /* silent redial off period */
      CMCALL_SR_MAX_DIALS,           /* max numbers of dial attempts */
      CMCALL_SR_MAX_TIME,            /* max time for silent redial sequence*/
      (cmcall_orig_plan_action_table_s_type*) &hdr_pkt_data_action_table    },

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #endif /* FEATURE_MMODE_CDMA_800) || (FEATURE_MMODE_CDMA_1900)*/


    #if defined (FEATURE_JCDMA)

    /* Default call origination plan for silent redial with JCDMA */
    {
      /* Condition */
      CM_FEATURE_ON,                 /* SR with JCDMA feature (ON) */
      CM_CALL_TYPE_NONE,             /* call type (any) */
      NULL,                          /* particular (none) */
      FALSE,                         /* hybrid call orig for pkt data call is
                                        allowed or not */
      CM_CALL_MODE_INFO_NONE,        /* Ignore the call mode */
      FALSE,                         /*is emerg on other sub*/

      /* Origination plan */
      TRUE,                          /* Honor MC hold-orig */
      CMCALL_PWRUP_HOLD_ORIG_UPTIME, /* power-up hold orig uptime */
      CMCALL_SR_PERIOD_TIME,         /* silent redial off period */
      CMCALL_SR_MAX_DIALS,           /* max numbers of dial attempts */
      CMCALL_SR_MAX_TIME,            /* max time for silent redial sequence*/
      (cmcall_orig_plan_action_table_s_type*) &jcdma_action_table },

    #elif ( ( defined(FEATURE_MMODE_CDMA_800) && defined(FEATURE_MMODE_CDMA_1900) ) || \
        ( defined (FEATURE_CM_TRIBAND_ORIG_PLAN_TBL) ))
    /* Default call origination plan for silent redial with NO AMPS, Trimode.
    */
    {
      /* Condition */
      CM_FEATURE_NONE,                /* SR with JCDMA feature (NONE) */
      CM_CALL_TYPE_PS_DATA,          /* call type (any) */
      NULL,                          /* particular (none) */
      FALSE,                          /* hybrid call orig for pkt data call is
                                        allowed or not */
      CM_CALL_MODE_INFO_GW_PS,       /* GW_PS call */
      FALSE,                         /*is emerg on other sub*/


      /* Origination plan */
      TRUE,                          /* Honor MC hold-orig */
      CMCALL_PWRUP_HOLD_ORIG_UPTIME, /* power-up hold orig uptime */
      CMCALL_SR_PERIOD_TIME,         /* silent redial off period */
      CMCALL_SR_MAX_DIALS,           /* max numbers of dial attempts */
      CMCALL_PS_DATA_SR_MAX_TIME,    /* max time for silent redial sequence*/
      (cmcall_orig_plan_action_table_s_type*) &trimode_action_table },

    /* Default call origination plan for silent redial with NO AMPS, Trimode.
    */
    {
      /* Condition */
      CM_FEATURE_NONE,                /* SR with JCDMA feature (NONE) */
      CM_CALL_TYPE_NONE,             /* call type (any) */
      NULL,                          /* particular (none) */
      FALSE,                         /* hybrid call orig for pkt data call is
                                        allowed or not */
      CM_CALL_MODE_INFO_NONE,        /* Ignore the call mode */
      FALSE,                         /*is emerg on other sub*/


      /* Origination plan */
      TRUE,                          /* Honor MC hold-orig */
      CMCALL_PWRUP_HOLD_ORIG_UPTIME, /* power-up hold orig uptime */
      CMCALL_SR_PERIOD_TIME,         /* silent redial off period */
      CMCALL_SR_MAX_DIALS,           /* max numbers of dial attempts */
      CMCALL_SR_MAX_TIME,            /* max time for silent redial sequence*/
      (cmcall_orig_plan_action_table_s_type*) &trimode_action_table },

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #elif (defined(CM_GW_SUPPORTED) && \
          !defined (FEATURE_MMODE_CDMA_800) && !defined (FEATURE_MMODE_CDMA_1900))

    /* Default call origination plan for silent redial with NO AMPS, Trimode.
    */#if 0
    {
      /* Condition */
      CM_FEATURE_NONE,               /* JCDMA feature not applicab for GW */
      CM_CALL_TYPE_PS_DATA,          /* call type (PS data) */
      NULL,                          /* particular (none) */
      FALSE,                         /* hybrid call orig for pkt data call is
                                        allowed or not */
      CM_CALL_MODE_INFO_NONE,        /* Ignore the call mode */
      FALSE,                         /*is emerg on other sub*/


      /* Origination plan */
      TRUE,                          /* Honor MC hold-orig, GW doesn't use*/
      CMCALL_PWRUP_HOLD_ORIG_UPTIME, /* power-up hold orig uptime */
      CMCALL_SR_PERIOD_TIME,         /* silent redial off period */
      CMCALL_SR_MAX_DIALS,           /* max numbers of dial attempts */
      CMCALL_PS_DATA_SR_MAX_TIME,    /* max time for silent redial sequence*/
      (cmcall_orig_plan_action_table_s_type*) &gw_action_table },

    #endif
    /* Default call origination plan for silent redial with NO AMPS, Trimode.
    */
    {
      /* Condition */
      CM_FEATURE_NONE,               /* JCDMA feature not applicab for GW */
      CM_CALL_TYPE_NONE,             /* call type (any) */
      NULL,                          /* particular (none) */
      FALSE,                         /* hybrid call orig for pkt data call is
                                        allowed or not */
      CM_CALL_MODE_INFO_NONE,        /* Ignore the call mode */
      FALSE,                         /*is emerg on other sub*/


      /* Origination plan */
      TRUE,                          /* Honor MC hold-orig, GW doesn't use */
      CMCALL_PWRUP_HOLD_ORIG_UPTIME, /* power-up hold orig uptime */
      CMCALL_SR_PERIOD_TIME,         /* silent redial off period */
      CMCALL_SR_MAX_DIALS,           /* max numbers of dial attempts */
      CMCALL_SR_MAX_TIME,            /* max time for silent redial sequence*/
      (cmcall_orig_plan_action_table_s_type*) &gw_action_table },

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #elif defined (FEATURE_MMODE_CDMA_800) || defined (FEATURE_MMODE_CDMA_1900)
    /* Default call origination plan for silent redial with non-Trimode.
    */
    {
      /* Condition */
      CM_FEATURE_OFF,                /* SR with JCDMA feature (OFF) */
      CM_CALL_TYPE_NONE,             /* call type (any) */
      NULL,                          /* particular (none) */
      FALSE,                         /* hybrid call orig for pkt data call is
                                        allowed or not */
      CM_CALL_MODE_INFO_NONE,        /* Ignore the call mode */
      FALSE,                         /*is emerg on other sub*/

      /* Origination plan */
      TRUE,                          /* Honor MC hold-orig */
      CMCALL_PWRUP_HOLD_ORIG_UPTIME, /* power-up hold orig uptime */
      CMCALL_SR_PERIOD_TIME,         /* silent redial off period */
      CMCALL_SR_MAX_DIALS,           /* max numbers of dial attempts */
      CMCALL_SR_MAX_TIME,            /* max time for silent redial sequence*/
      (cmcall_orig_plan_action_table_s_type*) &non_trimode_action_table },

    #elif defined(FEATURE_MMODE_LTE_ONLY) || defined(FEATURE_MMODE_GPRS_LTE)
    {
      /* Condition */
      CM_FEATURE_NONE,                /* SR with JCDMA feature (NONE) */
      CM_CALL_TYPE_NONE,             /* call type (any) */
      NULL,                          /* particular (none) */
      FALSE,                         /* hybrid call orig for pkt data call is
                                        allowed or not */
      CM_CALL_MODE_INFO_NONE,        /* Ignore the call mode */
      FALSE,                         /*is emerg on other sub*/


      /* Origination plan */
      TRUE,                          /* Honor MC hold-orig */
      CMCALL_PWRUP_HOLD_ORIG_UPTIME, /* power-up hold orig uptime */
      CMCALL_SR_PERIOD_TIME,         /* silent redial off period */
      CMCALL_SR_MAX_DIALS,           /* max numbers of dial attempts */
      CMCALL_SR_MAX_TIME,            /* max time for silent redial sequence*/
      (cmcall_orig_plan_action_table_s_type*) &trimode_action_table },
      
    #endif /* ! TriMode */

  };

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(call_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sr_ptr        = &call_ptr->sr;

  /* If Custom SR Timer is enabled, update orig plan tbl with the runtime
  ** value of Silent Redial timer. This needs to be done here because C90 
  ** compilation rules allow arrays to only be initialized with constant 
  */
  for( i=0; i < ARR_SIZE(orig_plan_tbl); i++ )
  {
    if( orig_plan_tbl[i].call_type == CM_CALL_TYPE_VOICE ||
        orig_plan_tbl[i].call_type == CM_CALL_TYPE_NONE
      )
    {
      if(call_ptr->asubs_id > SYS_MODEM_AS_ID_NONE && call_ptr->asubs_id < MAX_SIMS)
      {
        orig_plan_tbl[i].sr_max_time = cmcall_sr_timer_info[call_ptr->asubs_id].sr_max_time;
      }
      else
      {
        orig_plan_tbl[i].sr_max_time = cmcall_sr_timer_info[SYS_MODEM_AS_ID_1].sr_max_time;
      }
    }
  }

  /* Look for a match in the origination plan table.
  */
  for( i=0; i < ARR_SIZE(orig_plan_tbl); i++ )
  {
    if( 
        #ifdef FEATURE_MMODE_3GPP2
        (orig_plan_tbl[i].sr_with_jcdma == CM_FEATURE_NONE ||
         CM_BOOLEAN(orig_plan_tbl[i].sr_with_jcdma) == cm_is_jcdma_enable())
                                   &&
        #endif
        (orig_plan_tbl[i].call_type == CM_CALL_TYPE_NONE ||
         orig_plan_tbl[i].call_type == call_ptr->call_type)
                                   &&
        (orig_plan_tbl[i].dial_str == NULL ||
         (cm_num_cmp_exact(&call_ptr->num,
                           orig_plan_tbl[i].dial_str,
                           strlen((char*)orig_plan_tbl[i].dial_str))))
                                   &&
        #if (defined (FEATURE_HDR_HYBRID))
        (! orig_plan_tbl[i].is_hybr_allowed || call_ptr->is_hybr_allowed )
        #else
        ( !orig_plan_tbl[i].is_hybr_allowed )
        #endif
        &&
        ( orig_plan_tbl[i].call_mode_info == CM_CALL_MODE_INFO_NONE ||
          orig_plan_tbl[i].call_mode_info ==
                                       call_ptr->cmcall_mode_info.info_type )
        #ifdef FEATURE_MMODE_DUAL_SIM
        &&
        /* No explicit emergency call check needed as this is TRUE only for emergency call*/
        (orig_plan_tbl[i].is_emerg_on_other_sub == 
                       call_ptr->is_emerg_on_other_sub)
        #endif
      )
    {
      break;
    }
  }

  /* Make sure we find a match in the table and point the call
  ** origination plan pointer at the selected origination plan.
  */
  CM_ASSERT( i < ARR_SIZE(orig_plan_tbl) );
  i = MIN( i, ARR_SIZE(orig_plan_tbl)-1 );
  sr_ptr->orig_plan_ptr = &orig_plan_tbl[i];

  CM_MSG_HIGH_1( "Load orig plan #%d",i );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Setup the power-up hold orig time and the max number
  ** of dials in accordance with the origination plan.
  */
  sr_ptr->hold_orig_uptime   = sr_ptr->orig_plan_ptr->pwrup_hold_orig_time;
  sr_ptr->sr_left            = sr_ptr->orig_plan_ptr->sr_max_dial;

} /* cmcall_load_orig_plan() */
#endif
#if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
/*===========================================================================

FUNCTION cmcall_sent_on_1x_while_lte_active_in_single_mode

DESCRIPTION
  Check if the call is being dialed on 1X , though LTE is active currently

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE or FALSE based on above conditon

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_sent_on_1x_while_lte_active_in_single_mode(

    const cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{
  cmss_s_type    *ss_ptr  =  cmss_ptr();
  cmmsc_state_machine_s_type *state_machine = cmmsimc_state_machine_ptr(call_ptr->asubs_id);

  CM_MSG_HIGH_3( "sys_mode %d hybr2_operational %d favored_mode_pref %d ",
                ss_ptr->info.sys_mode,
                cmss_is_hybr2_operational(),
                call_ptr->favored_mode_pref );
  
  if(ss_ptr->info.sys_mode == SYS_SYS_MODE_LTE &&
      cmss_is_hybr2_operational() == FALSE &&
      cmph_is_subs_feature_mode_1x_sxlte(call_ptr->asubs_id) &&
      state_machine->stack_common_info.ue_mode == SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED 
      &&
      ((cmcall_misc_is_mode_pref(call_ptr->favored_mode_pref,CM_MODE_PREF_LTE_ONLY) == FALSE 
         && call_ptr->call_type == CM_CALL_TYPE_VOICE) ||
       (call_ptr->favored_mode_pref == CM_MODE_PREF_EMERGENCY
         && call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)) 
       &&
      call_ptr->direction == CM_CALL_DIRECTION_MO)
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION cmcall_sent_mo_call_on_1x_currently_in_single_mode

DESCRIPTION
 Check if the call is being dialed on 1X , And currently in single stack 
 SRLTE mode

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE or FALSE based on above conditon

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_sent_mo_call_on_1x_currently_in_single_mode(

    const cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{

  CM_MSG_HIGH_3( "call_type %d hybr2_operational %d favored_mode_pref %d ",
                call_ptr->call_type,
                cmss_is_hybr2_operational(),
                call_ptr->favored_mode_pref );
  
  if( (cmss_is_hybr2_operational() == FALSE &&
      cmph_is_subs_feature_mode_1x_sxlte(call_ptr->asubs_id))
      &&
      ((cmcall_misc_is_mode_pref(call_ptr->favored_mode_pref,CM_MODE_PREF_LTE_ONLY) == FALSE 
         && call_ptr->call_type == CM_CALL_TYPE_VOICE) ||
       (call_ptr->favored_mode_pref == CM_MODE_PREF_EMERGENCY
         && call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)) 
       &&
      call_ptr->direction == CM_CALL_DIRECTION_MO)
  {
    return TRUE;
  }
  return FALSE;
}
#endif

#if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
/*===========================================================================

FUNCTION cmcall_remove_mode_and_force_and_sched_sr

DESCRIPTION
  Check if the call is being dialed on 1X , though LTE is active currently

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE or FALSE based on above conditon

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_remove_mode_and_force_and_sched_sr(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{
  dword                    no_srv_time          = 0;
  boolean                  is_pref_changed     = FALSE;
  const cmcall_orig_plan_s_type  *orig_plan_ptr = NULL;
  sd_ss_mode_pref_e_type  mode_to_remove = SD_SS_MODE_PREF_HDR_LTE;

#if defined( FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)
  /* If CSFB is possible don't remove LTE */
  if(call_ptr->csfb_type != CM_CALL_CSFB_TYPE_NONE)
  {
    mode_to_remove = SD_SS_MODE_PREF_HDR;
    cmcall_update_orig_mode (call_ptr);
  }
#endif

  is_pref_changed = cmcall_remove_mode_and_force(call_ptr,
                                                 mode_to_remove,
                                                 &no_srv_time);

  orig_plan_ptr = call_ptr->sr.orig_plan_ptr;

  CM_MSG_HIGH_2("force non_ip: favord_mode_pref=%d, is_pref_changed=%d",
                      call_ptr->favored_mode_pref,is_pref_changed);

  /* Schedule the next SR.
  ** If the preference is changed, shorten the SR off period.
  ** Otherwise, use the value from the origination table.
  */
  sr_sched_next( &call_ptr->sr,
                 CM_BOOLEAN(is_pref_changed)? 1:orig_plan_ptr->sr_period_time,
                                                      /* SR off period */
                  no_srv_time,                     /* Hold orig period */
                  CM_BOOLEAN(no_srv_time)?CM_SR_ACT_RESTART:CM_SR_ACT_DEC,
                  call_ptr->end_status,
                  cmcall_act_get(call_ptr));
  call_ptr->end_status = CM_CALL_END_NONE;
}
#endif

#if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
/*===========================================================================

FUNCTION cmcall_force_mode_and_send_event

DESCRIPTION
  Check if the call is being dialed on 1X , though LTE is active currently

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE or FALSE based on above conditon

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_force_mode_and_send_event(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{
  cm_sr_s_type                *sr_ptr = &call_ptr->sr;
  cmph_s_type                 *ph_ptr = cmph_ptr();
    /* Point at phone object */
  sys_modem_as_id_e_type   as_id = call_ptr->asubs_id;

  dword                       no_srv_time;

  dword                       uptime = time_get_uptime_secs();
    /* Get the current uptime */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* If the favored mode preference is not in the current mode preference,
    ** force over to the favored mode preference now.
    */
    if(!call_ptr->is_vowlan && (!(cmcall_misc_is_mode_pref(call_ptr->favored_mode_pref,
                                    CMPH_SS_MODE_PREF(as_id, ph_ptr)))
      #ifdef FEATURE_HDR_HYBRID
        || ( call_ptr->favored_hybr_pref != CMPH_SS_HYBR_PREF(call_ptr->asubs_id, ph_ptr))
      #endif
      #if (defined FEATURE_MMODE_3GPP)
        || (( cmregprx_get_stack_info_ptr(cmph_map_cm_ss_type_to_sd_ss_type(call_ptr->ss))->state ==
             CMREGPRX_STATE_ACTIVE )
          #ifdef FEATURE_HICPS_STACK_SYNC_UP
            &&
            ( ph_ptr->is_hdr_activity == FALSE ) /* IRAT not in progress */
          #endif
           )
      #endif
        )
      #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
        && !cdma_ps_call_hold_timer_set
      #endif

      #if (defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE)
        && !cmcall_hold_ps_call_origination(call_ptr)
      #endif
    )
    {

      CM_MSG_HIGH_3("Force mode pref, call's=%d,current=%d, ss=%d",
                   call_ptr->favored_mode_pref,
                   CMPH_SS_MODE_PREF(as_id, ph_ptr),call_ptr->ss);
      no_srv_time = cmcall_force_pref_on_the_fly
                      ( call_ptr,
                        (call_ptr->call_type == CM_CALL_TYPE_PS_DATA) ?
                                        SD_SS_PREF_REAS_ORIG_START_PS :
                                        SD_SS_PREF_REAS_ORIG_START_CS
                      );

      /* If forcing to mode preference is causing a short period of
      ** no service (during which MC is acquiring the new service),
      ** hold the origination for the appropriate amount of time.
      */
      sr_ptr->hold_orig_uptime = MAX( sr_ptr->hold_orig_uptime,
                                      uptime + no_srv_time );
    }

    /* If last service request was MANUAL with type USER_PLMN_RAT, delay forcing PS call orig
    */
    #if (defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE)
    if(cmregprx_get_stack_info_ptr(call_ptr->ss)->substate == CMREGPRX_SUBSTATE_WAIT_SRV_CNF &&
      cmregprx_get_stack_info_ptr(call_ptr->ss)->last_srv_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL && 
      cmregprx_get_stack_info_ptr(call_ptr->ss)->last_srv_req.parameters.manual.type == REG_CM_SERVICE_REQ_USER_PREFERRED_PLMN_RAT)
    {
      call_ptr->sr.hold_orig_uptime = MAX(call_ptr->sr.hold_orig_uptime, uptime + CMCALL_PS_CALL_HOLD_ORIG_UPTIME);
      CM_MSG_HIGH_1("Waiting for manual PLMN srv_cnf. SR hold_orig_uptime = %ld", call_ptr->sr.hold_orig_uptime);
    }
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If this is not a standard OTASP call origination, hold the origination
    ** (and the system selection preference change) for up to
    ** CMCALL_ACQ_HOLD_SS_PREF_TIME in case system determination is currently
    ** trying to acquire service.
    */
    if(( call_ptr->call_type != CM_CALL_TYPE_STD_OTASP ) &&
       (call_ptr->call_type != CM_CALL_TYPE_EMERGENCY))
    {
      sr_ptr->hold_orig_uptime = MAX( sr_ptr->hold_orig_uptime,
                                      uptime + CMCALL_ACQ_HOLD_SS_PREF_TIME);

    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Update the flag about this call being a user originated one.
    */
    call_ptr->is_user_originated_call = TRUE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Tell phone about call start.
    */
    cmph_call_start( call_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If a PS data call on 1X is attempted and SMS is waiting for confirmation
    ** from 1X, wait till SMS gets the ACK and then send the origination down
    */

  #if((defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && defined(FEATURE_CDSMS))
    {
      cmsms_s_type  *sms_ptr = cmsms_ptr();
      CM_ASSERT( sms_ptr != NULL );
      if( ( sms_ptr->sms_state != CM_SMS_STATE_IDLE ) &&
          ( call_ptr->call_type == CM_CALL_TYPE_PS_DATA ) &&
          ( call_ptr->ss == CM_SS_MAIN ) )
      {
        sr_ptr->hold_orig_uptime = MAX( sr_ptr->hold_orig_uptime,
                                        uptime + CMCALL_SMS_ACK_HOLD_ORIG_TIME );
        call_ptr->sms_status     = CMCALL_SMS_STATUS_HOLD_ORIG;
      }
    }

  #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Notify client list of origination event.
    */
    /*If cc_object_ptr is not NULL that means envelope command has been
        called and call_event will be sent in Reorig_phase_2*/
    if(call_ptr->cc_object_ptr == NULL)
    {
      if(call_ptr->is_vowlan)
      {
        CM_MSG_HIGH_1("Set sys_mode as WLAN from %d",call_ptr->sys_mode);
        call_ptr->sys_mode = SYS_SYS_MODE_WLAN;
      }
      cmcall_event( call_ptr, CM_CALL_EVENT_ORIG );
    }


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined (FEATURE_LTE_TO_1X)
    if (call_ptr->csfb_type != CM_CALL_CSFB_TYPE_1XCSFB)
    {
      cmph_update_ddtm_status(ph_ptr);                   /* update DDTM status */
    }
  #else
    cmph_update_ddtm_status(ph_ptr);
  #endif

     /*
     * 1)VOLTE concurrency NV is set
     * 2)VOLTE call connected and put on hold in HYBR2
     * 3)1st e911 call originated,connected and ended in MAIN( over 1X)
     * 4)ECBM is still on.
     * 5)2nd e911 call when originated, will exit ECBM and force Hybr pref
        ON due to VOLTE call object lying abover phone
     * 6)To avoid the scenario, we put VOLTE call obj below PH when we do
        e911 orig by sending FALSE flag to the API. VOLTE will be  back up
      when we there is no e911 call and phone not in ECBM.
     */

  #if defined(FEATURE_IP_CALL) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE) 
    if(call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
    {
    cmph_change_volte_call_priority(FALSE, call_ptr->asubs_id);
    }
  #endif

  cmcall_orig_proc( call_ptr );

}



/*===========================================================================

FUNCTION cmcall_orig_start

DESCRIPTION
  Kick-start the origination process.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_orig_start(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{

  cmph_s_type                 *ph_ptr             = cmph_ptr();
    /* Point at phone object */

  cmss_s_type                 *ss_ptr             = cmss_ptr();
    /* point at SS object */

  cm_sr_s_type                *sr_ptr             = NULL;
    /* Point silent redial component */

  dword                       uptime              = time_get_uptime_secs();
    /* Get the current uptime */

  sys_modem_as_id_e_type   as_id;




  #if defined(FEATURE_MMGSDI_CDMA)
  sys_sys_mode_e_type sys_mode = SYS_SYS_MODE_NONE;
  #endif

   /* sub on which the call is originated */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( call_ptr != NULL );
  CM_ASSERT( ph_ptr != NULL );
  CM_ASSERT( ss_ptr != NULL );

  #ifdef CM_DEBUG
  #error code not present
#endif  /* CM_DEBUG */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get sub on which the call is originated */
  as_id = call_ptr->asubs_id;
  CM_ASSERT_ENUM_IS_INRANGE(as_id, SYS_MODEM_AS_ID_MAX);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sr_ptr       = &call_ptr->sr;

  /* Load the call origination plan
  ** and start the origination process.
  */
  cmcall_load_orig_plan( call_ptr );

  #ifdef FEATURE_HDR_HYBRID
  if( call_ptr->waiting_for_hdr_call_end )
  {
    call_ptr->sr.hold_orig_uptime = CMCALL_HDR_WAIT_ORIG_UPTIME;
  }
  #endif    /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_ptr->call_state = CM_CALL_STATE_ORIG;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Inform GSDI about the call start.
  */

 if(call_ptr->cc_object_ptr == NULL)
 {
  cmcall_update_gsdi_and_log_call_state(call_ptr->ss);
 }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  

  #if (defined (FEATURE_MMODE_CDMA_800) || defined (FEATURE_MMODE_CDMA_1900)) && !defined (FEATURE_GSM) && !defined (FEATURE_WCDMA)
  CM_MSG_HIGH_3("CallType=%d SrvTyep=%d SO=%d",
               call_ptr->call_type,
               call_ptr->srv_type,
               call_ptr->cmcall_mode_info.info.cdma_info.srv_opt );
  #endif

  CM_MSG_HIGH_1("MERGED cmcall_orig_start Favord mode=%d", call_ptr->favored_mode_pref);

  #if defined( FEATURE_HDR_HANDOFF)  && defined(FEATURE_IS2000)
  CM_MSG_HIGH_2("DRS=%d LADN=%d",
               call_ptr->cmcall_mode_info.info.cdma_info.drs_bit,
               call_ptr->last_act_data_net);
  #endif /* FEATURE_HDR_HYBRID && FEATURE_IS2000 */

  cmlog_call_orig_start(call_ptr);

  if (ph_ptr->oprt_mode_send_time != CMPH_MAX_UPTIME)
  {
    CM_MSG_HIGH_1("oprt_mode_hold: Operating Mode is on hold, sending online to MMOC, ph_ptr->oprt_mode_send_time %d",
      ph_ptr->oprt_mode_send_time);
    cmph_send_oprt_mode_online_to_mmoc();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_MMODE_DUAL_SIM
  if(( call_ptr->call_type == CM_CALL_TYPE_VOICE ||
      call_ptr->call_type == CM_CALL_TYPE_EMERGENCY )
      && cmph_is_msim()
  )
  {

   /* If current subs is not default voice, then change it*/
    if((ph_ptr->curr_voice_subs != as_id) &&
        ((cmcall_is_there_a_call_type(cmph_map_subs_to_ss(ph_ptr->curr_voice_subs),
         CM_CALL_TYPE_VOICE, CM_CALL_ID_INVALID)== CM_CALL_ID_INVALID)
         ||call_ptr->call_type == CM_CALL_TYPE_EMERGENCY))
    {
      ph_ptr->curr_voice_subs = as_id;

      /* Update MCS on voice priority change*/
      cmph_sp_subs_info_update();
    }
  }
  #endif

  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  /* Send emergency mode started on emergency call origination if UE is not in
  ** emergency mode already*/
  if(call_ptr->call_type == CM_CALL_TYPE_EMERGENCY && !cmph_is_emerg_mode_on(call_ptr->asubs_id))
  {
    cmph_update_send_emerg_status_ind(CM_EMERGENCY_MODE_STARTED, call_ptr->asubs_id);
  }
  #endif

  #ifdef FEATURE_MMODE_DUAL_SIM
  /* When emergency call is being dialed in a dual standby scenario (DSDS/TSTS) and there is already an
      actve cs call on other subs, end the cs call on other subs to give priority to emergency call. */
  if( (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY) &&
       (cmph_is_dsds() || cmph_is_tsts()) &&
       (cmph_ptr()->internal_standby_pref != SYS_MODEM_DS_PREF_SINGLE_STANDBY))
  {
    cmcall_check_and_end_calls_on_other_subs(call_ptr->asubs_id, CM_CALL_TYPE_VOICE);
    cmcall_check_and_end_calls_on_other_subs(call_ptr->asubs_id, CM_CALL_TYPE_EMERGENCY);
    cmcall_check_and_end_calls_on_other_subs(call_ptr->asubs_id, CM_CALL_TYPE_CS_DATA);
  }

  /* If a voice call is being originated, and there is an existing PS Data call
  ** on another subscription. Hold the PS Call origination until
  ** CMCALL_DS_VOICE_HOLD_ORIG_TIME
  */
  if(( call_ptr->call_type == CM_CALL_TYPE_VOICE ||
      call_ptr->call_type == CM_CALL_TYPE_EMERGENCY )
      && cmph_is_dsds()
    )
  {
    sys_modem_as_id_e_type ps_asubs_id = SYS_MODEM_AS_ID_NONE;

    /* Get the subscription id on which there is an active PS call.
    */
    ps_asubs_id = cmcall_get_subs_id_with_call_type(CM_CALL_TYPE_PS_DATA);

    CM_MSG_HIGH_1("PS_DATA call found on SubsId %d", ps_asubs_id);

    if( ps_asubs_id != SYS_MODEM_AS_ID_NONE &&
        ps_asubs_id != as_id
      )
    {

      #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
      /*End if there are any active datacalls on 1x*/
      boolean               ps_call_on_1x;
      ps_call_on_1x = cmcall_end_active_1x_datacalls(TRUE);
      if(ps_asubs_id == SYS_MODEM_AS_ID_2 || ps_call_on_1x == FALSE )
      #endif
      {
        #ifdef FEATURE_UE_INITIATED_DORMANCY

        /* Send the PS release indication to NAS
        */
        sm_cmd_type *sm_ptr = cm_sm_get_buf_else_err_fatal(SMREG_PS_SIGNALING_REL_REQ);
        sm_ptr->cmd.ps_signalling_rel_req.as_id = ps_asubs_id;
        sm_ptr->cmd.ps_signalling_rel_req.cause = SMREG_DUAL_STANDBY_CS_MO_CALL;
        cm_sm_send_cmd( sm_ptr );

        CM_MSG_HIGH_1("SMREG_PS_SIGNALING_REL_REQ sent on SubsId %d", ps_asubs_id);

        /* Update the data suspend state in CM
        */
        #ifdef FEATURE_MMODE_HYBR3_ENABLED
        if( ps_asubs_id == SYS_MODEM_AS_ID_3 || (cmph_is_sxlte() && ps_asubs_id == ph_ptr->hybr_3_stack_info.asubs_id))
        {
          cmss_process_hybr_3_data_suspend_rpt(TRUE);
        }
        else
        #endif
        #if defined(FEATURE_MMODE_DUAL_SIM)
        if( cmph_is_msim() && (ps_asubs_id == ph_ptr->hybr_2_stack_info.asubs_id) && !cmph_is_sxlte())
        {
          cmss_process_hybr_2_data_suspend_rpt(TRUE);
        }
        else
        #endif
        {
          cmss_process_data_suspend_rpt(TRUE);
        }

        cmss_ptr()->ue_init_ps_data_suspend = cmph_map_subs_to_ss(ps_asubs_id);

        /* Bump down the priority of the data call.
        */
        (void)cmtask_orig_para_change_act_priority( cmph_map_subs_to_ss(ps_asubs_id),
                                                    CM_ACT_TYPE_DATA_CALL,
                                                    CM_ACT_PRIORITY_80,
                                                    CM_ACT_PRIORITY_BELOW_PH,
                                                    TRUE
                                                   );
        /* Update the Hold Orig period
        */
        sr_ptr->next_orig_uptime = MAX( sr_ptr->next_orig_uptime,
                                 uptime + CMCALL_DS_VOICE_HOLD_ORIG_TIME );

        #endif /* FEATURE_UE_INITIATED_DORMANCY */
      }
    }
  }
  #endif
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifndef FEATURE_CM_DISABLE_GPS
  #if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE))

  cmpd_call_start( call_ptr->call_type, call_ptr->ss, call_ptr->sys_mode,
                   call_ptr->cmcall_mode_info.info_type, NULL );

  if( call_ptr->pd_status == CMCALL_PD_STATUS_END_ORIG )
  {
    call_ptr->end_status = CM_CALL_END_HDR_ORIG_DURING_GPS_FIX;
    cmcall_end( call_ptr );
    return;
  }

  /* If GPS session was ended, give some time for GPS to clean up
  */
  if( call_ptr->pd_status == CMCALL_PD_STATUS_HOLD_ORIG )
  {
    sr_ptr->hold_orig_uptime = MAX( sr_ptr->hold_orig_uptime,
                             uptime + CMCALL_PD_SESSION_ACK_HOLD_ORIG_TIME );
  }

  #endif /* FEATURE_GPSONE_MSBASED */
#endif

  // Update hold orig uptime if BSR has to be given more priority than 3GPP2 data calls
#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  if(cmxcall_is_cdma_ps_call_optimization_enabled(call_ptr) &&
     cmss_ptr()->info.sys_mode == SYS_SYS_MODE_CDMA)
  {

    sr_ptr->hold_orig_uptime = MAX( sr_ptr->hold_orig_uptime,
                                    ( uptime + cmcall_voice_sr_max_timer_get(call_ptr->asubs_id) ) );
    cdma_ps_call_hold_timer_set = TRUE;
    CM_MSG_HIGH_2(" hold_orig_uptime %d, hold_timer_set %d",sr_ptr->hold_orig_uptime,cdma_ps_call_hold_timer_set);

  }
#endif


  /* When emerg call is being dialed, deactivate other subs  so as to avoid coexistence
        issues in DSDA */
  #ifdef FEATURE_MMODE_DUAL_ACTIVE
  if(cmph_is_dsda() && (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY) )
  {
    cmmsimc_suspend_or_resume_other_subs(call_ptr->asubs_id, TRUE);
  }
  #endif


  /* srv domain check is needed for fixing below usecase 
  ** 1xcsfb call released
  ** sd immediately tries acquire lte sys
  ** NAS send camped indication on LTE
  ** Now user originates another voice call
  ** Call will be sent on 1x , since cmsds_is_1xcsfb_call_allowed fails , since srv domain is SYS_SRV_DOMAIN_CAMPED 
  ** Now in this case , we shouldnt switch to SRLTE after call end , as we might acquire previous LTE sys which is 1xcsfb capable .
  */
#if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
  if(cmcall_sent_on_1x_while_lte_active_in_single_mode(call_ptr)
     && cmss_intl_srv_info_ptr(CM_SS_MAIN)->srv_domain != SYS_SRV_DOMAIN_CAMPED
     && call_ptr->call_type == CM_CALL_TYPE_VOICE)
  {
    /* hvolte req:
    ** switch to SRLTE after WPS call ends
    */
    call_ptr->switch_to_srlte = TRUE;
  }

  if(cmcall_sent_on_1x_while_lte_active_in_single_mode(call_ptr)
     && cmcall_is_esr_allowed_in_hvolte()
     && call_ptr->call_type == CM_CALL_TYPE_VOICE)
  {
    cmcsfbcall_send_orig_req(call_ptr);
    call_ptr->is_waiting_on_esr_rsp = TRUE;
  }
  else
#endif
  {
    /* Actually start the origination process.
    */
    cmcall_force_mode_and_send_event( call_ptr );
  }


} /* cmcall_orig_start() */
#endif

#ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
/*===========================================================================

FUNCTION cmcall_check_if_end_status_to_propagate

DESCRIPTION
  Checks if the current end_status is one to be sent to inform FALLBACK_TO_CS to Clients.

DEPENDENCIES

RETURN VALUE
  TRUE: if event has to be sent
  FALSE: otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_check_if_end_status_to_propagate(
       cm_call_end_e_type      end_status
)
{
   switch (end_status)
   {
      case CM_CALL_END_PEER_NOT_REACHABLE:
      case CM_CALL_END_SIP_403_FORBIDDEN:
      case CM_CALL_END_NETWORK_NO_RESP_TIME_OUT:
      case CM_CALL_END_SIP_486_BUSY_HERE:
      case CM_CALL_END_CAUSE_SERVER_INTERNAL_ERROR:
      case CM_CALL_END_NOT_IMPLEMENTED:
      case CM_CALL_END_BAD_GATEWAY:
      case CM_CALL_END_NO_NETWORK_RESP:
      case CM_CALL_END_SERVER_TIME_OUT:
      case CM_CALL_END_VERSION_NOT_SUPPORTED:
      case CM_CALL_END_MESSAGE_TOO_LARGE:
      case CM_CALL_END_PRECONDITION_FAILURE:
         return TRUE;
      default:
         return FALSE;
   }

   return FALSE;
} /* cmcall_check_if_end_status_to_propagate () */
#endif

/*===========================================================================

FUNCTION cmcall_get_voip_call_id

DESCRIPTION
  Return the call id of the existing VoIP call, if present
  Else, return invalid call id

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  Return the call id of the existing VoIP call, if present
  Else, return invalid call id


SIDE EFFECTS
  None

===========================================================================*/
cm_call_id_type cmcall_get_voip_call_id()
{
  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  cmcall_s_type    *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */
  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP)
    {
       return call_ptr->call_id;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  }
  #endif
  return CM_CALL_ID_INVALID;
} /* cmcall_get_voip_call_id */

#if defined(FEATURE_MMODE_DUAL_SIM) || (defined(CM_GW_SUPPORTED) && !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE))
/*===========================================================================

FUNCTION cmcall_check_if_gw_cs_call_active

DESCRIPTION
  Checks if there is a cs call in given call state. CM_CALL_STATE_NONE
  checks for call in any state.

  NOTE:- Typically there will not be a call object with call type SUPS.
  cmcall_is_gw_cs_call_type() checks SUPS call type for completeness.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if CS voice calls active
  FALSE: otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_check_if_gw_cs_call_active(

       cm_call_state_e_type      call_state,
        /* Call state */

       cm_ss_e_type              ss

)
{

  cmcall_s_type  *call_ptr;
    /* Pointer to the call object */

  cm_iterator_type call_itr;

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    /* check the call state and the call type */
    if( ( cmcall_is_gw_cs_call_type (call_ptr->call_type)) &&
        ( call_state           == CM_CALL_STATE_NONE ||
          call_ptr->call_state == call_state
        ) &&
        ( call_ptr->ss       == ss )
      )
    {
      return TRUE;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  return FALSE;

} /* cmcall_check_if_gw_cs_call_active () */
#endif

#if defined(CM_GW_SUPPORTED)
#ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
/*===========================================================================

FUNCTION cmcall_check_voice_emerg_in_orig

DESCRIPTION
  Checks if current call is voice or emergency in origination

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE : if voice or emergency call is in origination
  FALSE: otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean cmcall_check_voice_emerg_in_orig(

    const cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{
  CM_ASSERT (call_ptr != NULL);

  if( ( (call_ptr->call_type == CM_CALL_TYPE_VOICE ) ||
        ( call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
      )
      &&

      ( call_ptr->call_state == CM_CALL_STATE_ORIG)
    )
  {
    return TRUE;
  }

  return FALSE;

} /* cmcall_check_voice_emerg_in_orig */


/*===========================================================================

FUNCTION cmcall_check_if_csdata_and_voice

DESCRIPTION
  Checks if one of the call type is CM_CALL_TYPE_VOICE and the other
  CM_CALL_TYPE_CS_DATA

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE : one is voice and the other is CS_DATA
  FALSE: otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean cmcall_check_if_csdata_and_voice(

    cm_call_type_e_type  call_type_1,
        /* call type */

    cm_call_type_e_type  call_type_2
        /* call type */
)
{

    if (((call_type_1 == CM_CALL_TYPE_VOICE) ||
                                    (call_type_1 == CM_CALL_TYPE_EMERGENCY))
        &&

        (call_type_2 == CM_CALL_TYPE_CS_DATA)
       )
    {
      return TRUE;
    }

    if (((call_type_2 == CM_CALL_TYPE_VOICE) ||
                                    (call_type_2 == CM_CALL_TYPE_EMERGENCY))
        &&
        (call_type_1 == CM_CALL_TYPE_CS_DATA)
       )
    {
      return TRUE;
    }

    return FALSE;

} /* cmcall_check_if_csdata_and_voice() */
#endif

/*===========================================================================

FUNCTION cmcall_check_if_ps_call_active

DESCRIPTION
  Checks if any PS call is active

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if PS call active
  FALSE: otherwise

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cmcall_check_if_ps_call_active(

        cm_ss_e_type              ss

)
{
  boolean ps_call_active = FALSE;
    /* flag to indicate whether PS call is active or not
    */

  cmcall_s_type  *call_ptr;
    /* Pointer to the call object */

  cm_iterator_type call_itr;

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    /* check the call state and the call type */
    if( ( call_ptr->call_type == CM_CALL_TYPE_PS_DATA ) &&
        ( call_ptr->call_state != CM_CALL_STATE_IDLE ) &&
        ( call_ptr->ss       == ss )
      )
    {
      ps_call_active = TRUE;
      break;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  return ps_call_active;

}

/*===========================================================================

FUNCTION cmcall_check_if_ps_call_active_as_id

DESCRIPTION
  Checks if any PS call is active on the specified asubs_id

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if PS call active
  FALSE: otherwise

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cmcall_check_if_ps_call_active_as_id (
        sys_modem_as_id_e_type  asubs_id
        /* sub on which to check if ps call is active */
)
{
  boolean ps_call_active = FALSE;
    /* flag to indicate whether PS call is active or not
    */

  cmcall_s_type  *call_ptr;
    /* Pointer to the call object */

  cm_iterator_type call_itr;

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    /* check the call state and the call type */
    if( ( call_ptr->call_type == CM_CALL_TYPE_PS_DATA ) &&
        ( call_ptr->call_state != CM_CALL_STATE_IDLE ) &&
        ( call_ptr->asubs_id == asubs_id )
      )
    {
      ps_call_active = TRUE;
      break;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  return ps_call_active;

}
#endif

#if (defined (FEATURE_MMODE_CDMA_800) || defined (FEATURE_MMODE_CDMA_1900) )
/*===========================================================================

FUNCTION cmcall_get_ps_call_obj_in_orig_mode

DESCRIPTION
  Get the call object corresponding to the PS call.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  Pointer to the call object if there is a PS call in origination state.
  NULL otherwise.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
cmcall_s_type *cmcall_get_ps_call_obj_in_orig_mode( void )
{
  cmcall_s_type  *call_ptr = NULL;
  boolean found=FALSE;
  cm_iterator_type call_itr;

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    /* check the call state and the call type */

    if(( call_ptr->call_type == CM_CALL_TYPE_PS_DATA ) &&
       ( call_ptr->call_state == CM_CALL_STATE_ORIG ) )
    {
      found = TRUE;
      break;
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  if(found)
  {
    return call_ptr;
  }
  else
  {
    return NULL;
  }
}
#endif
/*===========================================================================

FUNCTION cmcall_process_dom_sel_params

DESCRIPTION
  Decide if the call can be originated as a CSFB call depending on
  the current voice domain settings.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  the csfb type that can be used for the call.

SIDE EFFECTS
  None

===========================================================================*/
cm_call_csfb_e_type cmcall_process_dom_sel_params (

  const cmcall_s_type    *call_ptr
    /* pointer to a call object */
)
{

  #if (defined( FEATURE_3GPP_CSFB) || defined(FEATURE_LTE_TO_1X))
  cm_call_csfb_e_type csfb_type = CM_CALL_CSFB_TYPE_NONE;
  boolean is_lte_mode_avail     = FALSE;

  if (!cmph_is_lte_capable_on_sub(call_ptr->asubs_id))
  {
    return CM_CALL_CSFB_TYPE_NONE;
  }

  is_lte_mode_avail = sd_misc_is_mode_pref(cmph_map_cm_mode_pref_to_sd_mode_pref
                           (call_ptr->favored_mode_pref), SD_SS_MODE_PREF_LTE);

  if (((call_ptr->call_type == CM_CALL_TYPE_VOICE)||
       (call_ptr->call_type == CM_CALL_TYPE_PD) ||
      (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)||
       (call_ptr->call_type == CM_CALL_TYPE_CS_DATA)) &&
      (is_lte_mode_avail == TRUE)
      )
  {
    csfb_type = cmsds_dom_sel_get_csfb_type_allowed(call_ptr->asubs_id);
  }
  CM_MSG_HIGH_1 ("CSFB :csfb_type = %d ",csfb_type);
  return csfb_type;
  #else
  return CM_CALL_CSFB_TYPE_NONE;
  #endif
}


#ifndef FEATURE_MMODE_DISABLE_LTE_VOICE

/*===========================================================================

FUNCTION cmcall_process_ipcall_params

DESCRIPTION
  Remove HDR component from favored mode pref of a voice call if ss
  instance of voice call is different from that of HDR service that has
  VoIP.

  Setting force_top to TRUE for voice calls that can be possibly sent
  over IP. This takes care of multi call scenarios (MPTY, ECT on voip)
  where more than one voice call needs to be connected.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  Given call_ptr is modified.

===========================================================================*/
void cmcall_process_ipcall_params (

  cmcall_s_type    *call_ptr
    /* pointer to a call object */
)
{
  sd_ss_mode_pref_e_type sd_mode_pref = SD_SS_MODE_PREF_NONE;
    /* Mode preference */

  sys_sys_mode_e_type active_app_sys_mode = SYS_SYS_MODE_NONE;
  cm_ss_e_type ss_for_ip;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* checks if call type is supported on IP */
  if (!cmcall_is_call_type_supp_for_ipcall (call_ptr->call_type))
  {
    return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Mode preference for the current call */
  sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref
                                       (call_ptr->favored_mode_pref);

  ss_for_ip = cmcall_instance_for_ipcall (call_ptr);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  CM_MSG_HIGH_2("ip param proc: sd_mode_pref 0x%x, ss_for_ip %d",
                                                      sd_mode_pref,ss_for_ip);
  /* Forces the top if current call can be made as a IP call
  */
  if ( ss_for_ip != CM_SS_NONE)
  {
    call_ptr->force_top_pref = TRUE;

    #ifdef FEATURE_IP_CALL
    active_app_sys_mode = cmipapp_find_active_app_sys_mode(call_ptr->call_type);
    /* set flag if VOLTE call is possible */
    if(sd_misc_is_mode_pref(sd_mode_pref, SD_SS_MODE_PREF_LTE) &&
        cmcall_is_active_ip_app(SYS_SYS_MODE_LTE, call_ptr->call_type)
       )
    {
      call_ptr->is_volte = TRUE;
    } /* If WLAN is registered and no GW CS call active, only then set is_vowlan flag to TRUE */
    else if(active_app_sys_mode != SYS_SYS_MODE_WLAN &&\
             (sd_misc_is_mode_pref(sd_mode_pref,
              BM(cmph_map_sys_sys_mode_to_sd_mode(active_app_sys_mode))) &&
             (call_ptr->call_type == CM_CALL_TYPE_VT || call_ptr->call_type == CM_CALL_TYPE_VS))
      )
    {
      call_ptr->is_ip_call = TRUE;
    }
    CM_MSG_HIGH_2("is_volte is TRUE=%d, is_ip_call is TRUE=%d",
        call_ptr->is_volte,  call_ptr->is_ip_call);
    #endif
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If there is no ipapp (on HDR )for voice origination, remove HDR
  ** component.
  **
  ** Favored mode preference for Voice calls now carry HDR mode. If there is
  ** no ipapp registered on HDR then HDR has to be removed from favored
  ** mode preference. This is done so that SD does not end up camping on
  ** HDR when voice call is pending and when there is no HDR based ipapp.
  */
  if(! (sd_misc_is_mode_pref(sd_mode_pref, SD_SS_MODE_PREF_HDR)  &&
        cmcall_is_active_ip_app(SYS_SYS_MODE_HDR, call_ptr->call_type)
       )
    )

  {

    call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                     call_ptr->favored_mode_pref,
                                     1,
                                     SD_SS_MODE_PREF_HDR );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  return;

} /* cmcall_process_ipcall_params */


#if 0

/*===========================================================================

FUNCTION cmcall_process_wlan_e911_settings

DESCRIPTION
  Sets required settings for WLAN E911 state and Twwan timer.
  2 case to be handled.
    Idle mode E911 call handling
    WLAN ECBM E911 MO call handling

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  Given call_ptr is modified.

===========================================================================*/
void cmcall_process_wlan_e911_settings (

  cmcall_s_type    *call_ptr
    /* pointer to a call object */
)
{
  sd_ss_mode_pref_e_type sd_mode_pref = SD_SS_MODE_PREF_NONE;
      /* Mode preference */

  sys_sys_mode_e_type  sys_mode      = SYS_SYS_MODE_NONE;

  cmipapp_id_type app_id = CMIPAPP_ID_INVALID;

  cm_sr_s_type             *sr_ptr              = NULL;
     /* App ID*/
  cmph_sub_config_s_type *ph_sub_config_ptr = cmph_get_sub_config_ptr(call_ptr->asubs_id);
  cmph_s_type                  *ph_ptr         = cmph_ptr();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);
  CM_ASSERT(ph_sub_config_ptr != NULL);

  CM_ASSERT( BETWEEN(call_ptr->call_type, CM_CALL_TYPE_NONE, CM_CALL_TYPE_MAX ));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* checks if call type is supported on IP */
  if (!cmcall_is_call_type_supp_for_ipcall (call_ptr->call_type))
  {
    return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #if defined FEATURE_MMODE_EMC_SUPPORT

  /* Check if given call type is allowed to be originated as a ip call */
  app_id = cmipapp_find_active_app (SYS_SYS_MODE_WLAN, call_ptr->call_type);
  
  if(ph_ptr->ecbm_rat == SYS_SYS_MODE_WLAN && app_id != CMIPAPP_ID_INVALID)
  {
    /* for WLAN no need of inserting new instance in the priority queue*/    
    call_ptr->force_top_pref = TRUE;
    /* First call attempt has to be over WLAN, set WWAN timer to infinite 
    and state to WLAN_ECBM */
    call_ptr->sr.wlan_fallback_timer = -1;
    call_ptr->sr.emerg_call_attempt_state = CMCALL_ATTEMPT_STATE_WLAN_ECBM;
    call_ptr->is_vowlan = TRUE;
    /* Exit out of ECBM for call orig case in ECBM */
    cmph_exit_emergency_cb(cmph_ptr(),FALSE, CM_SS_MAIN, FALSE);
  }
  else
  {
    /* Regular case set timer to NV configurable value and state to WWAN_WLAN */
    call_ptr->sr.wlan_fallback_timer = time_get_uptime_secs() + ph_sub_config_ptr->twwan_timer;
    call_ptr->sr.emerg_call_attempt_state = CMCALL_ATTEMPT_STATE_WWAN_WLAN;
  }
  CM_MSG_HIGH_3("E911: call_attempt_state %d, wlan_fallback_timer %d, twwan_timer %d",\
           call_ptr->sr.emerg_call_attempt_state,call_ptr->sr.wlan_fallback_timer,ph_sub_config_ptr->twwan_timer);
  #endif

} /* cmcall_process_wlan_e911_settings */



/*===========================================================================

FUNCTION cmcall_process_custom_wifi_settings

DESCRIPTION
  When emergency call on wifi is supported, this function will set 
  some wifi call configurations.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  Given call_ptr is modified.

===========================================================================*/
void cmcall_process_custom_wifi_settings (

  cmcall_s_type    *call_ptr
    /* pointer to a call object */
)
{ 
  sys_sys_mode_e_type  sys_mode      = SYS_SYS_MODE_NONE;

  cmipapp_id_type app_id = CMIPAPP_ID_INVALID;

  cm_sr_s_type             *sr_ptr              = NULL;
     /* App ID*/

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  CM_ASSERT( BETWEEN(call_ptr->call_type, CM_CALL_TYPE_NONE, CM_CALL_TYPE_MAX ));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* checks if call type is supported on IP */
  if (!cmcall_is_call_type_supp_for_ipcall (call_ptr->call_type))
  {
    return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check if given call type is allowed to be originated as a ip call */
  sys_mode = cmcall_is_ipcall_possible(call_ptr);
  CM_MSG_HIGH_1("check ip call possibility, sys_mode=%d",sys_mode);
  if(sys_mode == SYS_SYS_MODE_WLAN)
  {
    /* for WLAN no need of inserting new instance in the priority queue*/    
    call_ptr->force_top_pref = TRUE;
  }

  /* check if IPAPP Is regsitered with WLAN and if ECMP value is set*/
  #if defined (FEATURE_IP_CALL) && defined (FEATURE_CM_LTE) && !defined (FEATURE_MMODE_DISABLE_LTE_VOICE)
  
  app_id = cmipapp_find_active_app (SYS_SYS_MODE_WLAN, call_ptr->call_type);

  if(app_id != CMIPAPP_ID_INVALID)
  {
     if(cmipapp_get_ecmp_val(app_id) == CMIPAPP_ECMP_WLAN)
     {
       /*set the is_vowlan flag for the call if there is an IPAPP registered supporting wlan call and if emergency is supported*/
       call_ptr->is_vowlan = TRUE;
     }
     else /*since wlan app is registed, fall back to wlan if cs call does not start in NV defined time*/
     {
       call_ptr->is_vowlan = FALSE;
       sr_ptr        = &call_ptr->sr;
       /*time till which call will be attempted over cs domain. if ok_to_orig is not recieved by this time, call needs to be attempted over wifi*/
       sr_ptr->fall_back_to_wifi_from_cs = time_get_uptime_secs() +
         cmph_get_sub_config_ptr(call_ptr->asubs_id)->wifi_cs_scan_timer;
       CM_MSG_HIGH_0("ECMP is 3gpp, IMS registered over WLAN, trying it as CS initially");
     }
  }
  /* if sys_mode is LTE, and LTE sys is E911 capable , App is registered over LTE for E911, and ECMP is 3GPP 
  call shall be made over IMS over LTE first, csfb_type shall be marked as NONE, it will be recalculated in case of CSBF later*/
  else  
  {
    app_id = cmipapp_find_active_app (SYS_SYS_MODE_WLAN, CM_CALL_TYPE_VOICE);
    if(app_id != CMIPAPP_ID_INVALID )
    {
      if( (cmipapp_get_ecmp_val(app_id) == CMIPAPP_ECMP_3GPP) &&
          sys_mode == SYS_SYS_MODE_LTE && 
          cmsds_is_current_sys_lte_ims_emergency_capable(call_ptr->asubs_id))
      {
        CM_MSG_HIGH_0("ECMP is 3gpp, IMS registered over WLAN, E911 over LTE, mark csfb type as none");
        call_ptr->csfb_type = CM_CALL_CSFB_TYPE_NONE;
      }
    }
  }
  #endif

} /* cmcall_process_custom_wifi_settings */
#endif
#endif

/**--------------------------------------------------------------------------
** Functions - internal: other than silent redial
** --------------------------------------------------------------------------
*/

#if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
/*===========================================================================

FUNCTION cmcall_is_call_type_valid

DESCRIPTION
  Check whether a call type is valid

DEPENDENCIES
  none

RETURN VALUE
  TRUE if call type is valid, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_call_type_valid(

    cm_call_type_e_type    call_type
        /* call type */
)
{

  switch( call_type )
  {
    case CM_CALL_TYPE_VOICE:
    case CM_CALL_TYPE_EMERGENCY:
    case CM_CALL_TYPE_CS_DATA:
    case CM_CALL_TYPE_PS_DATA:
    case CM_CALL_TYPE_SMS:
    case CM_CALL_TYPE_TEST:

    #ifdef FEATURE_OTASP
    case CM_CALL_TYPE_STD_OTASP:
    case CM_CALL_TYPE_NON_STD_OTASP:
    #endif

    #ifdef FEATURE_GPSONE
    case CM_CALL_TYPE_PD:
    #endif

    case CM_CALL_TYPE_VS:
    case CM_CALL_TYPE_VT:

      return TRUE;

    default:
      break;

  } /* switch( call_type ) */

  return FALSE;

} /* cmcall_is_call_type_valid() */
#endif

/*===========================================================================

FUNCTION cmcall_is_mobile_in_hdr_call()

DESCRIPTION
  Checks to see if any call is in HDR.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_mobile_in_hdr_call
(
  void
)
{
  boolean         ret_stat = FALSE;

  #ifdef FEATURE_HDR
  cmcall_s_type   *call_ptr;
  cm_iterator_type call_itr;

  /*-----------------------------------------------------------------------*/

  /*
  ** If 'so' of the call is HDR return TRUE else FALSE. If there are no
  ** active calls, return FALSE.
  */

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if(call_ptr->cmcall_mode_info.info.cdma_info.srv_opt ==
                                                   CAI_SO_HDR_PKT_DATA)
    {
      ret_stat = TRUE;
      break;
    }

   call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while */

  #endif  /* FEATURE_HDR */

  return ret_stat;

} /* cmcall_is_mobile_in_hdr_call() */

/*===========================================================================

FUNCTION cmcall_overall_call_state_per_subs

DESCRIPTION
  Returns overall call state of the subscription that ss is mappped to.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_overall_call_state_e_type  cmcall_overall_call_state_per_subs
(
  cm_ss_e_type      ss
)
{
  cm_call_id_type call_id;                        /* Loop index */
  cmcall_s_type *call_ptr;                        /* Call object pointer */
  cm_overall_call_state_e_type   overall_call_state
        = CM_OVERALL_CALL_STATE_IDLE;/* Initialize to default return value */
  /* Get which sub ss is mapping to */
  sys_modem_as_id_e_type   asubs_id = cmph_map_cm_ss_to_subs(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rex_enter_crit_sect(cmtask_crit_sect_ptr());
  /* Serialize data access cm_call_obj_array[] */

  for ( call_id = 0; call_id < ARR_SIZE(cm_call_obj_array); call_id++)
  {
    call_ptr = cm_call_obj_array[call_id];
    if ( call_ptr == NULL )
    {
      continue;
    }

    /* This check will make sure that the following happens
    ** - When asubs_id is 1, consider calls on MAIN, HDR stacks
    ** - When asubs_id is 2, consider calls only on HYBR_2
    */
    if( (asubs_id == SYS_MODEM_AS_ID_2 && call_ptr->asubs_id != asubs_id) ||
        (asubs_id == SYS_MODEM_AS_ID_1 && call_ptr->asubs_id == SYS_MODEM_AS_ID_2)
      )
    {
      continue;
    }

    if ( call_ptr->call_state == CM_CALL_STATE_CONV )
    {
      overall_call_state = CM_OVERALL_CALL_STATE_CONV;
      break;
    }
    else if ( call_ptr->call_state == CM_CALL_STATE_ORIG 
      || call_ptr->call_state == CM_CALL_STATE_CC_IN_PROGRESS)
    {
      if ( overall_call_state == CM_OVERALL_CALL_STATE_INCOM )
      {
        overall_call_state = CM_OVERALL_CALL_STATE_ORIG_INCOM;
      }
      else if ( (overall_call_state == CM_OVERALL_CALL_STATE_IDLE)
                ||
                /* When call obj array has a smaller call id in
                ** RELEASE state and higher call id in orig, the
                ** overall state should be ORIG
                */
                (overall_call_state == CM_OVERALL_CALL_STATE_RELEASE)
              )
            {
              overall_call_state = CM_OVERALL_CALL_STATE_ORIG;
            }
    }
    else if ( call_ptr->call_state == CM_CALL_STATE_INCOM )
    {
      if ( overall_call_state == CM_OVERALL_CALL_STATE_ORIG )
      {
        overall_call_state = CM_OVERALL_CALL_STATE_ORIG_INCOM;
      }
      else if ( overall_call_state == CM_OVERALL_CALL_STATE_IDLE )
      {
        overall_call_state = CM_OVERALL_CALL_STATE_INCOM;
      }
    }
    else if ( call_ptr->call_state == CM_CALL_STATE_RECALL_RSP_PEND)
    {
      if ( overall_call_state == CM_OVERALL_CALL_STATE_IDLE )
      {
        overall_call_state = CM_OVERALL_CALL_STATE_RECALL_RSP_PEND;
      }
    }
    else if (call_ptr->call_state == CM_CALL_STATE_IDLE)
    {
      if(overall_call_state > CM_OVERALL_CALL_STATE_IDLE)
      {
        continue;
      }
      else if(call_ptr->call_subst.idle == CM_CALL_IDLE_SUBST_ENDWAIT)
      {
        overall_call_state = CM_OVERALL_CALL_STATE_RELEASE;
      }
    }
    /* ( call_ptr->call_state == CM_CALL_STATE_CONV ) */
  } /* for ( call_id = 0; call_id < CM_CALL_ID_MAX; call_id++) */

  rex_leave_crit_sect(cmtask_crit_sect_ptr());

  return overall_call_state;

} /* cmcall_overall_call_state_per_subs() */

/*===========================================================================

FUNCTION cmcall_overall_call_state

DESCRIPTION
  Returns overall call state.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_overall_call_state_e_type  cmcall_overall_call_state
(
  void
)
{
  cm_overall_call_state_e_type  overall_state = CM_OVERALL_CALL_STATE_NONE;

  overall_state = cmcall_overall_call_state_per_subs(CM_SS_MAIN);

  /* If the main stack is IDLE, also check HYBR_2 stack
  */
  #ifdef FEATURE_MMODE_DUAL_SIM
  if(cmph_is_msim() && overall_state ==  CM_OVERALL_CALL_STATE_IDLE && !cmph_is_sxlte())
  {
    overall_state = cmcall_overall_call_state_per_subs(CM_SS_HYBR_2);
  }
  #endif

  #ifdef FEATURE_MMODE_HYBR3_ENABLED
  /* If the main/HYBR_2 stack is IDLE, also check HYBR_3 stack
  */
  if(cmph_is_msim() && overall_state ==  CM_OVERALL_CALL_STATE_IDLE )
  {
    overall_state = cmcall_overall_call_state_per_subs(CM_SS_HYBR_3);
  }
  #endif /* FEATURE_MMODE_HYBR3_ENABLED */

  return overall_state;
} /* cmcall_overall_call_state() */

#if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE) && defined(FEATURE_MMODE_3GPP2)

/*===========================================================================

FUNCTION cmcall_overall_call_state_per_ss

DESCRIPTION
  Returns overall call state for the specified ss

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_overall_call_state_e_type  cmcall_overall_call_state_per_ss
(
  cm_ss_e_type      ss
)
{
  cm_call_id_type call_id;                        /* Loop index */
  cmcall_s_type *call_ptr;                        /* Call object pointer */
  cm_overall_call_state_e_type overall_call_state
        = CM_OVERALL_CALL_STATE_IDLE;/* Initialize to default return value */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rex_enter_crit_sect(cmtask_crit_sect_ptr());
  /* Serialize data access cm_call_obj_array[] */

  for ( call_id = 0; call_id < ARR_SIZE(cm_call_obj_array); call_id++)
  {
    call_ptr = cm_call_obj_array[call_id];
    if ( call_ptr == NULL )
    {
      continue;
    }

    /* This check will make sure that the following happens
    ** - When ss = MAIN, consider calls on MAIN, HDR stacks
    ** - When ss = HYBR_2, consider calls only on HYBR_2
    */
    if( ((ss == CM_SS_HYBR_2 || ss == CM_SS_HYBR_3) && call_ptr->ss != ss) ||
        (ss == CM_SS_MAIN  && (call_ptr->ss == CM_SS_HYBR_2 || call_ptr->ss == CM_SS_HYBR_3))
      )
    {
      continue;
    }

    if ( call_ptr->call_state == CM_CALL_STATE_CONV )
    {
      overall_call_state = CM_OVERALL_CALL_STATE_CONV;
      break;
    }
    else if ( call_ptr->call_state == CM_CALL_STATE_ORIG )
    {
      if ( overall_call_state == CM_OVERALL_CALL_STATE_INCOM )
      {
        overall_call_state = CM_OVERALL_CALL_STATE_ORIG_INCOM;
      }
      else if ( (overall_call_state == CM_OVERALL_CALL_STATE_IDLE)
                ||
                /* When call obj array has a smaller call id in
                ** RELEASE state and higher call id in orig, the
                ** overall state should be ORIG
                */
                (overall_call_state == CM_OVERALL_CALL_STATE_RELEASE)
              )
            {
              overall_call_state = CM_OVERALL_CALL_STATE_ORIG;
            }
    }
    else if ( call_ptr->call_state == CM_CALL_STATE_INCOM )
    {
      if ( overall_call_state == CM_OVERALL_CALL_STATE_ORIG )
      {
        overall_call_state = CM_OVERALL_CALL_STATE_ORIG_INCOM;
      }
      else if ( overall_call_state == CM_OVERALL_CALL_STATE_IDLE )
      {
        overall_call_state = CM_OVERALL_CALL_STATE_INCOM;
      }
    }
    else if ( call_ptr->call_state == CM_CALL_STATE_RECALL_RSP_PEND)
    {
      if ( overall_call_state == CM_OVERALL_CALL_STATE_IDLE )
      {
        overall_call_state = CM_OVERALL_CALL_STATE_RECALL_RSP_PEND;
      }
    }
    else if (call_ptr->call_state == CM_CALL_STATE_IDLE)
    {
      if(overall_call_state > CM_OVERALL_CALL_STATE_IDLE)
      {
        continue;
      }
      else if(call_ptr->call_subst.idle == CM_CALL_IDLE_SUBST_ENDWAIT)
      {
        overall_call_state = CM_OVERALL_CALL_STATE_RELEASE;
      }
    }
    /* ( call_ptr->call_state == CM_CALL_STATE_CONV ) */
  } /* for ( call_id = 0; call_id < CM_CALL_ID_MAX; call_id++) */

  rex_leave_crit_sect(cmtask_crit_sect_ptr());

  return overall_call_state;

} /* cmcall_overall_call_state_per_subs() */
#endif
/*===========================================================================

FUNCTION cmcall_set_call_object

DESCRIPTION

  Sets the call type.


DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None.

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
void cmcall_set_type
(
  cmcall_s_type             *call_ptr,
  cm_call_type_e_type     call_type
)
{
  CM_ASSERT(call_ptr != NULL);

  call_ptr->call_type  = call_type;

} /* cmcall_set_type() */


/*===========================================================================

FUNCTION cmcall_print_all_calls

DESCRIPTION
  Prints call type and call state of all calls in CM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_print_all_calls (void)
{

  cmcall_s_type   *call_ptr;
  cm_iterator_type call_itr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmcall_obj_iterator_init(&call_itr);

  rex_enter_crit_sect(cmtask_crit_sect_ptr());
  /* Serialize data access cm_call_obj_array[] */
  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {

    CM_MSG_HIGH_3   (" Call %d of type %d is in state %d",
                     call_ptr->call_id, call_ptr->call_type,
                     call_ptr->call_state );

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while */
  rex_leave_crit_sect(cmtask_crit_sect_ptr());

} /* cmcall_print_all_calls () */

#if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
/*===========================================================================

FUNCTION cmcall_update_call_cmd_end

DESCRIPTION
  Update call cmd end with latest call information from call object

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Modifies parameters in end cmd at certain conditions

===========================================================================*/
static void cmcall_update_call_cmd_end
(
   cm_call_cmd_s_type *call_cmd_ptr
     /* client cmd */
)
{

  int                              end_call_loop = 0;
    /* Loops around calls that have to be ended
    */

  cm_end_params_s_type             *call_end_params_ptr = NULL;
    /* Points to end cmd parameters of a particular call
    */

  cm_call_mode_info_e_type          cm_call_info_type    = CM_CALL_MODE_INFO_NONE;
    /* info type as carried by CM
    */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (call_cmd_ptr->info.num_end_call_ids > CMCALL_MAX_CALL_OBJ)
  {
    CM_ERR_1 (" Number of calls to be ended %d exceeds CM_CALL_ID_MAX",
              call_cmd_ptr->info.num_end_call_ids);

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Loop through calls that have been listed for an end
  */
  for (end_call_loop=0; end_call_loop < call_cmd_ptr->info.num_end_call_ids;
       ++end_call_loop)
  {

    call_end_params_ptr = &call_cmd_ptr->info.end_params[end_call_loop];

    /* end cmd array can have call ids belonging to different info_type's.
    ** For ex - CDMA call id can be ended with same end cmd
    */

    /* info_type of the call from CM
    */
    cm_call_info_type = cmcall_call_id_mode (call_end_params_ptr->call_id);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if (cm_call_info_type == call_end_params_ptr->info_type)
    {
      /* No processing required if info_type matches */
      continue;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* - If call in CM has a non NONE info_type populate default end parameters
    **   in call end command
    ** - info_type given by client != CM's info_type
    */
    if (cm_call_info_type != CM_CALL_MODE_INFO_NONE)
    {

      CM_MSG_HIGH_3 ("info_type mismatch on call %d, client %d, CM %d",
               call_end_params_ptr->call_id,
               call_end_params_ptr->info_type,
               cm_call_info_type);

	  if (call_end_params_ptr->info_type == CM_CALL_MODE_INFO_GW_PS && cm_call_info_type == CM_CALL_MODE_INFO_LTE)
      {
         /* - Possibly IRAT has changed the Data call context to LTE, End the call with 
	     **  reject cause CM_CALL_CMD_ERR_MODE_INFO_CHG_LTE, DS will trigger
	     **  PDN disconnect Request
         */	  
          cmcall_client_cmd_err( call_cmd_ptr, CM_CALL_CMD_ERR_MODE_INFO_CHG_LTE );

          /* Reset the cmd cb func ptr so that further calls
           ** to cmcall_client_cmd_err does not call the cmd cb again
          */
          call_cmd_ptr->cmd_cb_func = NULL;
          call_cmd_ptr->cmd_cc_cb_func = NULL;
          
          return;
      }	   
			   
      /* Populate default end parameters depending on info type
      */
      switch (cm_call_info_type)
      {

        #ifdef CM_GW_SUPPORTED
        #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
        case CM_CALL_MODE_INFO_GW_CS:

          call_end_params_ptr->end_params.gw_cs_end.reserved = 0;
          break;
        #endif

        case CM_CALL_MODE_INFO_GW_PS:

          call_end_params_ptr->end_params.gw_ps_end.nsapi.valid = FALSE;
          call_end_params_ptr->end_params.gw_ps_end.tear_down_ind.valid
                                                                = FALSE;
          call_end_params_ptr->end_params.gw_ps_end.cause
                                                  = SM_REGULAR_DEACTIVATION;
          break;

        #endif /* #ifdef CM_GW_SUPPORTED */


        #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
        case CM_CALL_MODE_INFO_CDMA:

          call_end_params_ptr->end_params.cdma_end.end_reason_included = TRUE;
          call_end_params_ptr->end_params.cdma_end.end_reason
                                            = (uint8) CAI_REL_NORMAL;
          break;

        #endif /* #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */

        #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
        case CM_CALL_MODE_INFO_IP:

          #if defined(FEATURE_IP_CALL)
          call_end_params_ptr->end_params.ip_cause.cause =
                                           (uint32) CMIPAPP_END_CAUSE_NORMAL;
          #endif

          break;
        #endif

        case CM_CALL_MODE_INFO_NONE:
        default:
          CM_ERR_2 ("info_type %d invalid for call %d",
             cm_call_info_type, call_cmd_ptr->info.end_params[0].call_id);

          break;
      }

    }
    /* If call in CM has a NONE as info_type clear the end params for the call
    ** id
    */
    else
    {

      memset ( &call_end_params_ptr->end_params, 0,
                                  sizeof(call_end_params_ptr->end_params));


    }

    /* info_type given by client is overwritten to overcome mismatch
    ** between client supplied info_type and the one maintained by CM
    */
    call_end_params_ptr->info_type = cm_call_info_type;


  } /* for loop */


} /* cmcall_update_call_cmd_end () */


/*===========================================================================

FUNCTION cmcall_send_mode_spec_end

DESCRIPTION
  End calls with mode_info INFO_NONE by calling cmcall_end. GW, 1x and ipcall
  specific functions filter out call id of interest and process call end.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Modifies parameters in end cmd.

===========================================================================*/
static void cmcall_send_mode_spec_end
(
   cm_call_cmd_s_type *call_cmd_ptr
     /* client cmd */
)

{

  int                              end_call_loop     = 0;
    /* Loops around calls that have to be ended
    */

  cmcall_s_type                   *call_ptr          = NULL;
   /* call_ptr for call objects
   */

  cm_call_cmd_err_e_type           call_cmd_err      = CM_CALL_CMD_ERR_NOERR;
    /* Call cmd error */

  boolean                          is_call_found = FALSE;
    /* Indicates the need of sending call-end to lower layers
    */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT (call_cmd_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Loop through calls that have been listed for an end
  */
  for (end_call_loop=0; end_call_loop < call_cmd_ptr->info.num_end_call_ids;
       ++end_call_loop)
  {

    CM_MSG_HIGH_2 (" End cmd for call %d info_type = %d",
                     call_cmd_ptr->info.end_params[end_call_loop].call_id,
                     call_cmd_ptr->info.end_params[end_call_loop].info_type);


    call_ptr =
       cmcall_ptr (call_cmd_ptr->info.end_params[end_call_loop].call_id);

    if (call_ptr != NULL)
    {
      is_call_found = TRUE;
    }

    /* End calls of info_type NONE by directly calling cmcall_end().
    */
    if (call_cmd_ptr->info.end_params[end_call_loop].info_type !=
                                                   CM_CALL_MODE_INFO_NONE)
    {
      continue;
    }

    if (call_ptr != NULL)
    {
      call_ptr->end_status = CM_CALL_END_CLIENT_END;
      call_ptr->prot_end_status = CM_CALL_END_CLIENT_END; /* as user has ended the call */
      cmcall_end( call_ptr );
    }


  } /* for () */


  /* If CM doesn't find call objects for any calls sent from CM-client,
  ** then Call-Ids are not valid, we don't need to send end-call to
  ** lower layers
  */
  if (is_call_found != TRUE)
  {
    cmcall_client_cmd_err( call_cmd_ptr, CM_CALL_CMD_ERR_CALL_ID_P );

    /* Reset the cmd cb func ptr so that further calls
    ** to cmcall_client_cmd_err does not call the cmd cb again
    */
    call_cmd_ptr->cmd_cb_func = NULL;
    call_cmd_ptr->cmd_cc_cb_func = NULL;
    call_cmd_ptr->cmd_call_id_cb_func = NULL;

    CM_MSG_HIGH_1("None of %d calls have valid call object",
                 call_cmd_ptr->info.num_end_call_ids);

    return;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Cmd check procedure
  ** - If any access technology returns error, cmd cb is immed called
  **   and control returns.
  **
  ** - If there is no error in any access tech then cmd cb is called
  **   with cb func ptr forced to be NULL
  */


  #if defined(FEATURE_MMODE_GW)
  call_cmd_err = cmwcall_client_cmd_check( call_cmd_ptr );

  if( call_cmd_err != CM_CALL_CMD_ERR_NOERR )
  {
    cmcall_client_cmd_err( call_cmd_ptr, call_cmd_err );
    return;
  }
  #endif

  /* Checks CDMA call cmd errors */
  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  call_cmd_err = cmxcall_client_cmd_check( call_cmd_ptr );

  if( call_cmd_err != CM_CALL_CMD_ERR_NOERR )
  {
    cmcall_client_cmd_err( call_cmd_ptr, call_cmd_err );
    return;
  }
  #endif

  /* Checks IP call cmd errors */
  #if defined(FEATURE_IP_CALL) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
  call_cmd_err = cmipcall_client_cmd_check( call_cmd_ptr );

  if( call_cmd_err != CM_CALL_CMD_ERR_NOERR )
  {
    cmcall_client_cmd_err( call_cmd_ptr, call_cmd_err );
    return;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* No cmd error is present if code reaches this point */
  cmcall_client_cmd_err( call_cmd_ptr, call_cmd_err );

  /* Reset the cmd cb func ptr so that further calls to cmcall_client_cmd_err
  ** does not call the cmd cb again
  */
  call_cmd_ptr->cmd_cb_func = NULL;
  call_cmd_ptr->cmd_cc_cb_func = NULL;
  call_cmd_ptr->cmd_call_id_cb_func = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extracts calls of info_type GW_CS, GW_PS to end them */
  #if defined(CM_GW_SUPPORTED)
  cmwcall_client_cmd_proc  (call_cmd_ptr);
  #endif

  /* Extracts calls of info_type CDMA to end */
  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  cmxcall_client_cmd_proc(call_cmd_ptr);
  #endif

  #if defined(FEATURE_IP_CALL)&& !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
  /* Extracts calls of info_type IP to end */
  cmipcall_client_cmd_proc(call_cmd_ptr);
  #endif



} /* cmcall_send_mode_spec_end () */
#endif

#if defined(FEATURE_MMODE_DUAL_SIM) || defined(CM_GW_SUPPORTED)
#ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
/*===========================================================================

FUNCTION cmcall_is_gw_cs_call_type

DESCRIPTION
  Checks if the call type is used on cs domain

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - Call type used on gw cs domain
  FALSE - Call type not being used on gw cs domain

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_gw_cs_call_type
(
  cm_call_type_e_type call_type
)
{

  if ( (call_type == CM_CALL_TYPE_VOICE) ||

       (call_type == CM_CALL_TYPE_EMERGENCY) ||

       (call_type == CM_CALL_TYPE_SUPS) ||

       (call_type == CM_CALL_TYPE_CS_DATA) ||

       (call_type == CM_CALL_TYPE_VT) ||

       (call_type == CM_CALL_TYPE_VT_LOOPBACK)
     )
  {

    return TRUE;
  }

  return FALSE;

} /* cmcall_is_gw_cs_call_type () */
#endif

/*lint +esym(818, cmd_ptr)*/
/*lint +esym(818, ps_cc_params)*/


/*===========================================================================

FUNCTION cmcall_get_sec_pdp_callid

DESCRIPTION
  Gives the call id of a secondary PDP .

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  call id of secondary pdp call.

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_get_sec_pdp_callid (void)
{

  #ifdef CM_GW_SUPPORTED

  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( (call_ptr->call_type                  == CM_CALL_TYPE_PS_DATA)    &&
         (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS) &&
         (!call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.primary_pdp)
       )
    {
      return call_ptr->call_id;
    } /* if ( call_ptr->call_type == call_type ) */

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  /*-----------------------------------------------------------------------*/

  #endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

  return CM_CALL_ID_INVALID;

} /* cmcall_get_sec_pdp_callid () */

#endif

/**--------------------------------------------------------------------------
** Functions - external: call state, call object, & call id
** --------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION  cmcall_get_call_obj_in_call_state

DESCRIPTION
  Returns the call obj in call_state.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to the call obj in call_state if present
  NULL otherwise

SIDE EFFECTS
  none

===========================================================================*/

cmcall_s_type* cmcall_get_call_obj_in_call_state (

                  cm_call_state_e_type   call_state
                  /* Given call state */
)
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if (call_ptr->call_state == call_state)
    {
      return call_ptr;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while  */

  return NULL;
} /* cmcall_get_call_obj_in_call_state () */


/*===========================================================================

FUNCTION  cmcall_get_subs_id_with_call_type

DESCRIPTION
  Returns the subscription id on which a call with this call_type exists

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to the call obj in call_state if present
  NULL otherwise

SIDE EFFECTS
  none

===========================================================================*/

sys_modem_as_id_e_type cmcall_get_subs_id_with_call_type (

                  cm_call_type_e_type   call_type
                  /* Given call type */
)
{
  cmcall_s_type   *call_ptr;
    /* Call object pointer */

  cm_iterator_type call_itr;
    /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if (call_ptr->call_type == call_type )
    {
      return call_ptr->asubs_id;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while  */

  return SYS_MODEM_AS_ID_NONE;

} /* cmcall_get_subs_id_with_call_type () */

/*===========================================================================

FUNCTION cmcall_data_suspend

DESCRIPTION
 Ends all data calls & marks ps suspend true/ resumes data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmcall_data_suspend
(
   cm_call_cmd_s_type  *call_cmd_ptr
     /* client cmd */
)
{
  #ifndef FEATURE_MMODE_DUAL_SIM
  SYS_ARG_NOT_USED(call_cmd_ptr);
  #else

  boolean      data_suspend = call_cmd_ptr->info.data_suspend;

  cm_ss_e_type           ss = cmph_map_subs_to_ss(call_cmd_ptr->cmd_subs);
  cmph_s_type            *ph_ptr         = cmph_ptr();

  /* only allow suspend on DDS sub; always allow resume */
  if ( ((ph_ptr->internal_standby_pref != SYS_MODEM_DS_PREF_SINGLE_STANDBY) &&
        (ph_ptr->default_data_subs == call_cmd_ptr->cmd_subs)) ||
        data_suspend == FALSE )
  {
    if (data_suspend == TRUE)
    {
      if((cmcall_is_there_a_call_type(ss,CM_CALL_TYPE_VOICE,
                                        CM_CALL_ID_INVALID)!= CM_CALL_ID_INVALID) ||
            (cmcall_is_there_a_call_type(ss,CM_CALL_TYPE_EMERGENCY,
                                        CM_CALL_ID_INVALID) != CM_CALL_ID_INVALID) ||
            (cmcall_is_there_a_call_type(ss,CM_CALL_TYPE_CS_DATA,
                                        CM_CALL_ID_INVALID) != CM_CALL_ID_INVALID)
           )
      {
        if(call_cmd_ptr->cmd_cb_func != NULL)
        {
          CM_MSG_HIGH_0("Rejecting the cmd as there is active CS call");
          call_cmd_ptr->cmd_cb_func( call_cmd_ptr->data_block_ptr,
                               call_cmd_ptr->cmd,
                               CM_CALL_CMD_ERR_IN_USE_S);
        }
        return;
      }
      CM_MSG_HIGH_1("Suspending data on SubsId %d", call_cmd_ptr->cmd_subs);
      if( cmcall_get_subs_id_with_call_type(CM_CALL_TYPE_PS_DATA)
                                                  !=SYS_MODEM_AS_ID_NONE)
      {
        #if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
        boolean is_1x_call_active = FALSE;
        if(ss == CM_SS_MAIN )
        {
          is_1x_call_active = cmcall_end_active_1x_datacalls(TRUE);
        }
        if ((ss == CM_SS_HYBR_2)
        #ifdef FEATURE_MMODE_HYBR3_ENABLED
        || (ss == CM_SS_HYBR_3)
        #endif
        ||(is_1x_call_active == FALSE))
        #endif
        {
          #ifdef FEATURE_UE_INITIATED_DORMANCY
          sm_cmd_type *sm_ptr = cm_sm_get_buf_else_err_fatal(SMREG_PS_SIGNALING_REL_REQ);
          sm_ptr->cmd.ps_signalling_rel_req.as_id = call_cmd_ptr->cmd_subs;
          sm_ptr->cmd.ps_signalling_rel_req.cause = SMREG_DUAL_STANDBY_CS_MO_CALL;
          cm_sm_send_cmd( sm_ptr );
          CM_MSG_HIGH_1("SMREG_PS_SIGNALING_REL_REQ sent on SubsId %d", call_cmd_ptr->cmd_subs);
          #endif
        }
      }
      cmss_ptr()->ue_init_ps_data_suspend = ss;
    }
    else
    {
      cmss_ptr()->ue_init_ps_data_suspend = CM_SS_NONE;
    }

    #ifdef FEATURE_MMODE_HYBR3_ENABLED
    if(ss == CM_SS_HYBR_3)
    {
      cmss_process_hybr_3_data_suspend_rpt(data_suspend);
    }
    else
    #endif
    #ifdef FEATURE_MMODE_HYBR2_ENABLED
    if(ss == CM_SS_HYBR_2)
    {
      cmss_process_hybr_2_data_suspend_rpt(data_suspend);
    }
    else
    #endif
    {
      cmss_process_data_suspend_rpt(data_suspend);
    }
  }
  else
  {
    CM_MSG_HIGH_3("Ignoring CM_CALL_CMD_DATA_SUSPEND, wrong state on asubs_id %d, standby_pref %d, default data %d",
                  call_cmd_ptr->cmd_subs,
                      ph_ptr->internal_standby_pref,ph_ptr->default_data_subs );
  }

  if(call_cmd_ptr->cmd_cb_func != NULL)
  {
    call_cmd_ptr->cmd_cb_func( call_cmd_ptr->data_block_ptr,
                               call_cmd_ptr->cmd,
                               CM_CALL_CMD_ERR_NOERR );
  }

  #endif
}

#ifdef FEATURE_OTASP
/*===========================================================================

FUNCTION  cmcall_get_otasp_call_obj

DESCRIPTION
  Returns a call for any active 1x call (OTASP/OTAPA/data/voice/SMS).
  There can be only one call obj of this type

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to the call obj for a 1x call in CONV. mode.
  NULL otherwise

SIDE EFFECTS
  none

===========================================================================*/

cmcall_s_type* cmcall_get_otasp_call_obj()
{
  cmcall_s_type     *call_ptr = NULL;
      /* Call object pointer  */
  cm_iterator_type  call_itr;
      /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if (call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_CDMA &&
         call_ptr->call_state == CM_CALL_STATE_CONV)
    {
      return call_ptr;
    }
    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while  */
  return NULL;
} /* cmcall_get_otasp_call_obj() */
#endif
/*===========================================================================

FUNCTION cmcall_call_id_get

DESCRIPTION
  Get the call id corresponding to a call object pointer.

DEPENDENCIES
  None

RETURN VALUE
  call_id

SIDE EFFECTS
  None

===========================================================================*/
cm_call_id_type cmcall_call_id_get( const cmcall_s_type *call_ptr )
{
  if(call_ptr != NULL)
  {
    return call_ptr->call_id;
  }
  else
  {
    return CM_CALL_ID_INVALID;
  }
} /* cmcall_call_id_get() */

#if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE)
/*===========================================================================

FUNCTION cmcall_end_call

DESCRIPTION
  Do the graceful call end activities.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(818, call_ptr)*/
static void cmcall_end_call(

   cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{
  cm_call_mode_info_e_type  info_type   = CM_CALL_MODE_INFO_NONE;
  cm_call_id_type           actv_callid = CM_CALL_ID_INVALID;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (call_ptr == NULL)
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  info_type   = call_ptr->cmcall_mode_info.info_type;
  actv_callid = call_ptr->call_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  if( info_type == CM_CALL_MODE_INFO_CDMA )
  {
    cmxcall_end_call( call_ptr->call_id,
                      call_ptr->end_status,
                      CAI_REL_NORMAL );
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(CM_GW_SUPPORTED)
  if( info_type == CM_CALL_MODE_INFO_GW_CS  ||
      info_type == CM_CALL_MODE_INFO_GW_PS  ||
      info_type == CM_CALL_MODE_INFO_GSM_PS ||
      info_type == CM_CALL_MODE_INFO_WCDMA_PS)
  {
    cmwcall_end_call(call_ptr);
  }
  #endif /* CM_GW_SUPPORTED */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined (FEATURE_IP_CALL) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
  if(info_type == CM_CALL_MODE_INFO_IP )
  {
    cmipcall_send_end(call_ptr);
}
  #endif /* FEATURE_IP_CALL */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* By this time, call object may be deallocated depending on call type.
  ** If active call object, then free it along with its call pref on
  ** priority queue.
  */
  if( cmcall_ptr(actv_callid) != NULL )
  {
    cmcall_end( call_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
}
/*lint +esym(818, call_ptr)*/
#endif

/*===========================================================================

FUNCTION cmcall_update_gsdi_and_log_call_state

DESCRIPTION
  This function updates the GSDI status about traffic/non-traffic
  status.

  This function also logs the call state.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_update_gsdi_and_log_call_state
(
  cm_ss_e_type      ss
)
{
  cmss_s_type                  *ss_ptr             = cmss_ptr();
  cm_overall_call_state_e_type overall_state;
  sys_sys_mode_e_type      sys_mode = SYS_SYS_MODE_NONE;

  overall_state = cmcall_overall_call_state_per_subs(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  #ifdef FEATURE_MMODE_HYBR3_ENABLED
  if(ss == CM_SS_HYBR_3)
  {
    sys_mode = cmss_ptr()->info.gw3_sys_mode;
  }
  else 
  #endif
  #ifdef FEATURE_MMODE_HYBR2_ENABLED
  if(ss == CM_SS_HYBR_2)
  {
    sys_mode = cmss_ptr()->info.gw_sys_mode;
  }
  else
  #endif
  {
    sys_mode = cmss_ptr()->info.sys_mode;
  }

  /* Tell GSDI when the phone goes into/outoff traffic */
  if(overall_state == CM_OVERALL_CALL_STATE_IDLE)
  {

    cm_mmgsdi_in_traffic_channel_w_mode(FALSE,
       sys_mode, ss );

  }
  else
  {

    cm_mmgsdi_in_traffic_channel_w_mode(TRUE,
      sys_mode, ss);
  }


  /*
  ** Notify DIAG of call state change
  */
  cmlog_call_state(ss);

} /* cmcall_update_gsdi_and_log_call_state() */

#if (defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE)
/*===========================================================================

FUNCTION cm_find_call_id_per_subs

DESCRIPTION
  Find the call ID using the connection ID.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_find_call_id_per_subs
(
  cm_conn_id_src_e_type   ll_src,
  byte                    connection_id,
  cm_call_id_type         *call_id_ptr,
  sys_modem_as_id_e_type  asubs_id
)
{
  cmcall_s_type   *call_ptr = NULL;
  cm_iterator_type  call_itr;
  boolean           ignore_asubs_id = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
// used for both cs and ps call. UNKNOW_CONNECTION_ID only apply for cs.
//  CM_ASSERT( connection_id != UNKNOWN_CONNECTION_ID );


  if(!call_id_ptr)
  {
    return FALSE;
  }

  /* Ignore asubs_id for single sim */
  if (cmph_is_ssim())
    ignore_asubs_id = TRUE;

  *call_id_ptr = (byte)CM_CALL_ID_INVALID;

  /*
  ** find the matching call ID
  */

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {

    if( (call_ptr->gw_conn_id_src   == ll_src) &&
        (call_ptr->gw_connection_id == connection_id) &&
        (ignore_asubs_id
         ||
        (call_ptr->asubs_id == asubs_id ||
         asubs_id == SYS_MODEM_AS_ID_MAX))
      )
    {
      *call_id_ptr = call_ptr->call_id;
      CM_MSG_HIGH_3("Call id:%d found for connection id:%d, asubs_id: %d",
                            call_ptr->call_id, connection_id, asubs_id);
      return TRUE;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);

  }

  CM_MSG_HIGH_2("Call id not found for connection id:%d, asubs_id: %d",
                         connection_id, asubs_id);
  return FALSE;

} /* cm_find_call_id_per_subs() */

/*===========================================================================

FUNCTION cm_find_call_id

DESCRIPTION
  Find the call ID using the connection ID.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_find_call_id
(
  cm_conn_id_src_e_type   ll_src,
  byte                    connection_id,
  cm_call_id_type         *call_id_ptr
)
{
  return cm_find_call_id_per_subs
               (ll_src,
                connection_id,
                call_id_ptr,
                SYS_MODEM_AS_ID_MAX);

} /* cm_find_call_id() */



#if (defined(FEATURE_MMODE_GW) && defined(FEATURE_CM_LTE))
/*===========================================================================

FUNCTION cm_find_call_id_gwl

DESCRIPTION
  Find the call ID during LTE to GW IRAT.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_find_call_id_gwl
(
  byte                    connection_id,
  cm_call_id_type         *call_id_ptr,
  sys_modem_as_id_e_type  asubs_id
)
{
  cmcall_s_type      *call_ptr = NULL;
  cm_iterator_type   call_itr;
  cmcall_info_s_type *info_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!call_id_ptr)
  {
    return FALSE;
  }

  *call_id_ptr = (byte)CM_CALL_ID_INVALID;

  /*
  ** find the matching call ID
  */

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    info_ptr = &(call_ptr->cmcall_mode_info.info);

    if((call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS) &&
       (info_ptr->gsm_wcdma_ps_info.esm_conn_id == connection_id)
       #ifdef FEATURE_MMODE_DUAL_SIM
     && (call_ptr->asubs_id == asubs_id)
       #endif
      )
    {
      CM_MSG_HIGH_3("LTE conn_id(%d) found in GW: call_id=%d, gw conn_id=%d",
               connection_id, call_ptr->call_id, call_ptr->gw_connection_id);

      *call_id_ptr = call_ptr->call_id;
      return TRUE;
    }

    if((call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_LTE) &&
       (info_ptr->lte_info.gsm_umts_connection_id.valid) &&
       (info_ptr->lte_info.gsm_umts_connection_id.connection_id == \
        connection_id)
       #ifdef FEATURE_MMODE_DUAL_SIM
       && (call_ptr->asubs_id == asubs_id)
       #endif
      )
    {
      CM_MSG_HIGH_3("GW conn_id(%d) found in LTE: call_id=%d, lte conn_id=%d",
               connection_id, call_ptr->call_id, call_ptr->gw_connection_id);

      *call_id_ptr = call_ptr->call_id;
      return TRUE;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);

  }

  return FALSE;

} /* cm_find_call_id_gwl() */


/*===========================================================================

FUNCTION cm_find_call_id_nsapi

DESCRIPTION
  Find the call ID for non-transferrable dedicated bearer based on nsapi/eps_bearer_id.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_find_call_id_nsapi
(
  byte                    nsapi,
  cm_call_id_type         *call_id_ptr,
  sys_modem_as_id_e_type  asubs_id
)
{
  cmcall_s_type      *call_ptr = NULL;
  cm_iterator_type   call_itr;
  cmcall_info_s_type *info_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if(!call_id_ptr)
  {
    return FALSE;
  }

  *call_id_ptr = (byte)CM_CALL_ID_INVALID;

  /*
  ** find the matching call ID
  */

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    info_ptr = &(call_ptr->cmcall_mode_info.info);

    CM_MSG_LOW_2("call_ptr info_type=%d,gsm_umts_connection_id valid=%d",
               (int)call_ptr->cmcall_mode_info.info_type,
               (int)info_ptr->lte_info.gsm_umts_connection_id.valid);

    if((call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_LTE) &&
       (info_ptr->lte_info.gsm_umts_connection_id.valid == FALSE) &&
       (info_ptr->lte_info.eps_bearer_id == nsapi)
       #ifdef FEATURE_MMODE_DUAL_SIM
     && (call_ptr->asubs_id == asubs_id)
       #endif
      )
    {
      CM_MSG_HIGH_3("LTE eps_bearer_id(%d) found in GW: call_id=%d,nsapi=%d",
                  info_ptr->lte_info.eps_bearer_id,
                  call_ptr->call_id, nsapi);

      *call_id_ptr = call_ptr->call_id;
      return TRUE;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);

  }

  return FALSE;

} /* cm_find_call_id_nsapi() */
#endif

/*===========================================================================

FUNCTION cm_find_call_id_gw

DESCRIPTION
  Find the call ID with gw(SM) connection_id
  This will search for if there is any call_ptr which has gw connection id
  including LTE or GW call.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_find_call_id_gw
(
  byte                    connection_id,
  cm_call_id_type         *call_id_ptr
)
{
  cmcall_s_type      *call_ptr = NULL;
  cm_iterator_type   call_itr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!call_id_ptr)
  {
    return FALSE;
  }

  *call_id_ptr = (byte)CM_CALL_ID_INVALID;

  /*
  ** find the matching call ID
  */

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if((call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS) &&
       (call_ptr->gw_connection_id == connection_id))
    {
      *call_id_ptr = call_ptr->call_id;
      return TRUE;
    }
    #if defined(FEATURE_CM_LTE)
    else if((call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_LTE) &&
       (call_ptr->cmcall_mode_info.info.lte_info.gsm_umts_connection_id.valid) &&
       (call_ptr->cmcall_mode_info.info.lte_info.gsm_umts_connection_id.connection_id == \
        connection_id))
    {
      *call_id_ptr = call_ptr->call_id;
      return TRUE;
    }
    #endif

    call_ptr = cmcall_obj_get_next(&call_itr);

  }

  return FALSE;

} /* cm_find_call_id_gw() */

/*===========================================================================

FUNCTION cm_find_connection_id

DESCRIPTION
  Find the connection ID using the call ID.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_find_connection_id
(
  cm_call_id_type         call_id,
  byte                    *connection_id_ptr
)
{

  cmcall_s_type  *call_ptr = NULL;

  if(connection_id_ptr == NULL)
  {
    return FALSE;
  }

  *connection_id_ptr = CM_UNKNOWN_CONN_ID;

  call_ptr = cmcall_ptr(call_id);

  if(call_ptr == NULL)
  {
    return FALSE;
    }

  *connection_id_ptr = call_ptr->gw_connection_id;

  return TRUE;
} /* cm_find_connection_id() */



/*===========================================================================

FUNCTION cm_set_connection_id

DESCRIPTION
  Set connection ID using the call ID.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
void cm_set_connection_id
(
  cmcall_s_type           *call_ptr,
  byte                    connection_id
)
{
  CM_ASSERT(call_ptr != NULL);

  call_ptr->gw_connection_id = connection_id;

} /* cm_set_connection_id() */


/*===========================================================================

FUNCTION cm_set_connection_id_src

DESCRIPTION
  Set connection ID src using the call ID.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
void cm_set_connection_id_src
(
  cmcall_s_type           *call_ptr,
  cm_conn_id_src_e_type   conn_id_src
)
{
  CM_ASSERT(call_ptr != NULL);

  call_ptr->gw_conn_id_src = conn_id_src;
}

#endif /* defined (FEATURE_GSM) || defined (FEATURE_WCDMA) || defined(FEATURE_CM_LTE)*/



/*===========================================================================
FUNCTION CMCALL_CALL_IDS_ALLOCATED

DESCRIPTION
  This function returns the number of call IDs allocated.

DEPENDENCIES
  Call object must be initialized

RETURNS
  none

SIDE EFFECTS
  none

===========================================================================*/
byte cmcall_call_ids_allocated
(
  void
)
{

  byte              num_allocated_call_ids = 0;
  cmcall_s_type    *call_ptr = NULL;
  cm_iterator_type  call_itr;

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);


  while(call_ptr != NULL)
  {
    if(call_ptr->call_id != CM_CALL_ID_INVALID)
    {
      num_allocated_call_ids++;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  }
  return num_allocated_call_ids;

} /* cmcall_call_ids_allocated() */


/*===========================================================================
FUNCTION CMCALL_CALL_IDS_ALLOCATED_PER_SS

DESCRIPTION
  This function returns the number of call IDs allocated.

DEPENDENCIES
  Call object must be initialized

RETURNS
  none

SIDE EFFECTS
  none

===========================================================================*/
byte cmcall_call_ids_allocated_per_ss
(
  cm_ss_e_type    ss
)
{

  byte              num_allocated_call_ids = 0;
  cmcall_s_type    *call_ptr = NULL;
  cm_iterator_type  call_itr;

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if(call_ptr->call_id != CM_CALL_ID_INVALID &&
       call_ptr->ss      == ss
      )
    {
      num_allocated_call_ids++;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  }
  return num_allocated_call_ids;

} /* cmcall_call_ids_allocated_per_ss() */

#if defined(FEATURE_MMODE_DUAL_ACTIVE) || defined(FEATURE_MMODE_3GPP)
/*===========================================================================
FUNCTION cmcall_call_ids_allocated_per_as_id

DESCRIPTION
  This function returns the number of call IDs allocated per asubs_id.

DEPENDENCIES
  Call object must be initialized

RETURNS
  num of call ids allocated

SIDE EFFECTS
  none

===========================================================================*/
byte cmcall_call_ids_allocated_per_as_id
(
  sys_modem_as_id_e_type   asubs_id
)
{

  byte              num_allocated_call_ids = 0;
  cmcall_s_type    *call_ptr = NULL;
  cm_iterator_type  call_itr;
  boolean           ignore_asubs_id = FALSE;

  /* Ignore asubs_id for single sim */
  if (cmph_is_ssim())
    ignore_asubs_id = TRUE;

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if(call_ptr->call_id != CM_CALL_ID_INVALID
       &&
       (call_ptr->asubs_id == asubs_id || ignore_asubs_id)
      )
    {
      num_allocated_call_ids++;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  }
  return num_allocated_call_ids;

} /* cmcall_call_ids_allocated_per_ss() */
#endif

/*===========================================================================

FUNCTION cm_number_of_allocated_calls

DESCRIPTION

  Returns number of allocated calls.

DEPENDENCIES


RETURN VALUE
  Number of allocated calls

SIDE EFFECTS
  none

===========================================================================*/
int cm_number_of_allocated_calls( void )
{
  return cmcall_call_ids_allocated();

} /* cm_number_of_allocated_calls() */



/*===========================================================================

FUNCTION cm_number_of_allocated_calls_with_type

DESCRIPTION

  Returns number of allocated calls with the given call_type.

DEPENDENCIES


RETURN VALUE
  Number of allocated calls

SIDE EFFECTS
  none

===========================================================================*/
int cm_number_of_allocated_calls_with_type( cm_call_type_e_type call_type)
{
  int               num_call_ids = 0;
  cmcall_s_type     *call_ptr = NULL;
  cm_iterator_type  call_itr;

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);


  while(call_ptr != NULL)
  {
    if((call_ptr->call_id != CM_CALL_ID_INVALID)&&
       (call_ptr->call_type == call_type))
    {
      num_call_ids++;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  return num_call_ids;


} /* cm_number_of_allocated_calls_with_type() */



/*===========================================================================

FUNCTION cm_number_of_allocated_calls_with_type_substate

DESCRIPTION

  Returns number of allocated calls with the given call subtype.

DEPENDENCIES


RETURN VALUE
  Number of allocated calls

SIDE EFFECTS
  none

===========================================================================*/
int cm_number_of_allocated_calls_with_type_substate( cm_call_type_e_type call_type, cm_call_incom_subst_e_type substate)
{
  int			    num_call_ids = 0;
  cmcall_s_type	    *call_ptr = NULL;
  cm_iterator_type  call_itr;

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);


  while(call_ptr != NULL)
  {
    if((call_ptr->call_id != CM_CALL_ID_INVALID)&&
       (call_ptr->call_type == call_type)&&
       (call_ptr->call_state == CM_CALL_STATE_INCOM)&&
       (call_ptr->call_subst.incom == substate ))
    {
      num_call_ids++;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  return num_call_ids;


} /* cm_number_of_allocated_calls_with_type_substate() */


#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
/*============================================================================

FUNCTION: cm_mm_call_cmd_end_all_calls

DESCRIPTION
  Processes a request to end all calls.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  If the command is successful, CM clients are notified of the corresponding
  CM event.

===========================================================================*/
void cm_mm_call_cmd_end_all_calls (void)
{
    int i,j=0;
    uint8 num_call_ids = 0;
    cm_end_params_s_type *end_params = NULL;

    for (i = 0; i < CMCALL_MAX_CALL_OBJ; i++)
    {
      if ( cm_call_id_is_allocated ((cm_call_id_type) i) )
      {
         num_call_ids++;
      } /* if  cm_call_id_is_allocated  */
    } /* for i < CM_CALL_ID_MAX */

    if ( num_call_ids > 0 )
    {
      end_params = (cm_end_params_s_type *)
                   cm_mem_malloc(num_call_ids*sizeof(cm_end_params_s_type));

      for (i = 0; i < CMCALL_MAX_CALL_OBJ && j < num_call_ids; i++)
      {
        if ( cm_call_id_is_allocated ((cm_call_id_type) i) )
        {
           end_params[j].call_id = (cm_call_id_type) i;
           end_params[j].info_type = CM_CALL_MODE_INFO_CDMA;
           end_params[j].end_params.cdma_end.end_reason_included = FALSE;
           end_params[j].end_params.cdma_end.end_reason = CAI_REL_NORMAL;
           CM_MSG_HIGH_1 ("call end request: adding call_id %d to the end list",
                          end_params[j].call_id);
           j++;
        }
      }
      (void) cm_mm_call_cmd_end( NULL, NULL, CM_CLIENT_ID_ANONYMOUS,
                               num_call_ids, end_params );
      cm_mem_free(end_params);
    }

}
#endif



/*===========================================================================

FUNCTION cm_call_id_is_allocated

DESCRIPTION

  Returns TRUE if given call id is allocated and FALSE otherwise.

DEPENDENCIES
  none

RETURN VALUE
  Success/Failure

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_call_id_is_allocated( cm_call_id_type call_id )
{

  cmcall_s_type *call_ptr = NULL;

  call_ptr = cmcall_ptr(call_id);

  if(call_ptr != NULL)
  {
    return TRUE;
  }

  return FALSE;

} /* cm_call_id_is_allocated() */

#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
/*===========================================================================

FUNCTION cmcall_ptr_is_allocated

DESCRIPTION

  Returns TRUE if given call_ptr is allocated and FALSE otherwise.

DEPENDENCIES
  none

RETURN VALUE
  Success/Failure

SIDE EFFECTS
  None

===========================================================================*/
boolean cmcall_ptr_is_allocated
(
  const cmcall_s_type   *call_ptr
)
{
  boolean  found = FALSE;
  word     index = 0;

  while( call_ptr != NULL &&
         index < ARR_SIZE(cm_call_obj_array) )
  {
     if( call_ptr == cm_call_obj_array[index++] )
     {
       found = TRUE;
       break;
     }
  }

  return found;
} /* cmcall_ptr_is_allocated */
#endif

#if defined (CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)

/*===========================================================================

FUNCTION cm_new_incom_call_id

DESCRIPTION
  Allocate a new call id and a call object for an incoming call.

  It also generates a conenction id from SM/MN as per the case
  depending on PKT call or CIRCUIT call respectively.

  Assigns the connection Id to the call object.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_new_incom_call_id
(
  cm_call_type_e_type     call_type,
  cm_call_id_type         *call_id_ptr,
  byte                    connection_id
)
{
  cm_call_id_type               call_id = CM_CALL_ID_INVALID;
  cmcall_s_type                *call_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_id_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** Allocate a Call Id and a Call Object.
  */
  call_id = cm_call_id_allocate();

  if( call_id == CM_CALL_ID_INVALID )
  {
    return FALSE;
  }

  /*
  ** set the call id
  */
  *call_id_ptr = call_id;

  call_ptr = cmcall_ptr(call_id);
  if(call_ptr == NULL)
  {
    return FALSE;
  }

  /* save the information in the call object array */

  cm_set_connection_id(call_ptr,connection_id);

  if(call_type == CM_CALL_TYPE_PS_DATA)
  {
    cm_set_connection_id_src(call_ptr,CONN_ID_SM);
  }
  else
  {
    cm_set_connection_id_src(call_ptr,CONN_ID_MN);
  }

  cmcall_set_type(call_ptr,call_type);

  cmcall_set_gw_cc_result (call_ptr, CM_CC_RESULT_NONE);

  return TRUE;
} /* cm_new_incom_call_id() */
#endif /* defined (FEATURE_WCDMA) || defined (FEATURE_GSM) || defined(FEATURE_CM_LTE) */

/*===========================================================================

FUNCTION cm_call_id_allocate

DESCRIPTION

  Allocate a call Id and get a call object.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
cm_call_id_type cm_call_id_allocate
(
  void
)
{
  cmcall_s_type            *call_ptr;
  unsigned int             i,j;
  cm_call_id_type          call_id=CM_CALL_ID_INVALID;

  static dword             call_id_failure_uptime = CMCALL_TIMER_INACTIVE;
                                                  /* Time since last failure */

  /* Initialized to "arr_size() - 1", so that first call id allocation
  ** starts with 0. Some AST scripts are dependent on this.
  */
  static cm_call_id_type last_call_id = ARR_SIZE(cm_call_obj_array) - 1;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** If FEATURE CLADE HEAP is defined dynamically allocate a call object through clade heap alloc
  */
  call_ptr = (cmcall_s_type *)cm_mem_malloc_ext(sizeof(cmcall_s_type));

  if(call_ptr == NULL)
  {
    CM_ERR_FATAL_0("modem_mem_malloc_ext() failed!!");
  }

  cmcall_reset(call_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Search through call object array for an unallocated call object
  ** starting from last call_id allocated.
  */
  rex_enter_crit_sect(cmtask_crit_sect_ptr());
  /* Serialize data access cm_call_obj_array[] */
  i= last_call_id;
  for ( j = 0; j < ARR_SIZE(cm_call_obj_array); j++ )
  {
    i = (i+1)%ARR_SIZE(cm_call_obj_array);
    if (cm_call_obj_array[i] == NULL )
    {
      call_id = (cm_call_id_type)i;
      last_call_id = call_id;
      call_ptr->call_id = call_id;
      call_ptr->call_owner = rex_self()->thread_id;
      cm_call_obj_array[i] = call_ptr;
      break;
    }

  }
  rex_leave_crit_sect(cmtask_crit_sect_ptr());

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we're out of call ids, and this is the first time we are out of call ids
  ** since a successful allocation, let's wait some time before error fataling.
  ** If time is up, we should error fatal, otherwise return the allocated call id.
  */
  if(!cmcall_is_call_id_in_range(call_id))
  {
    call_id = CM_CALL_ID_INVALID;
    cm_mem_free_ext(call_ptr);
    if( call_id_failure_uptime < time_get_uptime_secs ())
    {
      /* CM still does not have a free call id to allocate. Error fatal for
      ** this condition has been removed. This function returns
      ** CM_CALL_ID_INVALID for this case.
      */

      CM_MSG_HIGH_0 ( "CM is out of call ids -- Fatal Error!");

      cmcall_print_all_calls ();
    }
    else if( call_id_failure_uptime == CMCALL_TIMER_INACTIVE )
    {
      /* First failure after a successful allocation.  Set the timer.
      */
      CM_MSG_HIGH_0( "CM is out of call ids -- Holding on for 5 ...");
      call_id_failure_uptime = time_get_uptime_secs() +
                               CMCALL_CALL_ID_HOLD_FATAL_ERR_TIME;
    }
    else
    {
      /* Failure again but we are still holding on, since the timer is not up yet...
      */
      CM_MSG_HIGH_0( "CM is out of call ids again -- Still holding on...");
    }
  }
  else
  {
    /* Reset the timer to show a successful allocation.
    */
    call_id_failure_uptime  = CMCALL_TIMER_INACTIVE;

    CM_MSG_HIGH_2( "Object[0x%x] allocated for id[%d]", call_ptr, call_id);
  }

  return call_id;

} /* cm_call_id_allocate() */


/*===========================================================================

FUNCTION cm_call_id_deallocate

DESCRIPTION
  Deallocates a call ID

DEPENDENCIES
  none

RETURN VALUE
  Success/Failure

SIDE EFFECTS
  none

===========================================================================*/
void cm_call_id_deallocate
(
    cm_call_id_type call_id
        /* Call id */
)
{
  cmcall_s_type            *call_ptr;
  cm_cmd_type            *cmd_ptr = NULL;

  if( call_id >= CMCALL_MAX_CALL_OBJ )
  {
    return;
  }


  call_ptr = cmcall_ptr(call_id);

  if(call_ptr == NULL )
  {
    return;
  }

  if(call_ptr->cc_object_ptr != NULL)
  {
    cmcc_object_deallocate( call_ptr->cc_object_ptr );
  }

  cmd_ptr = cm_reply_get(&call_ptr->reply);
  if( cmd_ptr != NULL )
  {
    /* Deallocate cmd_ptr, otherwise it will leads to memory leak */
    cm_cmd_dealloc(cmd_ptr);
  }


  rex_enter_crit_sect(cmtask_crit_sect_ptr());
  /* Serialize data access cm_call_obj_array[] */
  cm_call_obj_array[call_id] = NULL;
  #if defined (FEATURE_IP_CALL) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
  REF_CNT_OBJ_RELEASE_IF(call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.ip_gen_param);
  /*whenever a call gets ended clean up mng state to NONE */
  cmipcall_update_mng_state_per_call(call_id, CMIPCALL_SUPS_ST_NONE);
  #endif
  /*
  ** If FEATURE CLADE HEAP is defined deallocate a call object through clade heap free
  */
  cm_mem_free_ext(call_ptr);
  rex_leave_crit_sect(cmtask_crit_sect_ptr());

  CM_MSG_HIGH_1( "Call id (%d) deallocated", call_id );

} /* cm_call_id_deallocate() */


/**--------------------------------------------------------------------------
** Functions - external:commands from other CM objects (ph,ss,etc.)
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cmcall_call_id_mode

DESCRIPTION

  Returns call mode if given call id is allocated.

DEPENDENCIES
  none

RETURN VALUE
  cm_call_mode_info_e_type

SIDE EFFECTS
  None

===========================================================================*/
cm_call_mode_info_e_type cmcall_call_id_mode
(
  cm_call_id_type call_id
)
{
  cmcall_s_type   *call_ptr = cmcall_ptr( call_id );

  /*-----------------------------------------------------------------------*/

  if ( call_ptr == NULL )
  {
    return CM_CALL_MODE_INFO_NONE;
  }
  else
  {
    /* After sending the call object transfer event to DS,
    ** change the mode back to GW
    */
    if ((call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GSM_PS)||
        (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_WCDMA_PS))
    {
      CM_MSG_HIGH_0("Change info_type from GSM_PS/WCDMA_PS to GW_PS");

      call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_PS;
    }

    return( call_ptr->cmcall_mode_info.info_type );
  }

} /* cmcall_call_id_mode() */


#if 0 /* Not being used */
/*===========================================================================

FUNCTION cmcall_overall_call_end

DESCRIPTION
  Ends all calls.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_overall_call_end
(
  void
)
{

  cmcall_s_type *call_ptr;                        /* Call object pointer */
  cm_iterator_type  call_itr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( call_ptr->call_state != CM_CALL_STATE_IDLE )
    {
      call_ptr->end_status = CM_CALL_END_OFFLINE;
      cmcall_end( call_ptr );
    } /* if ( call_ptr->call_state != CM_CALL_STATE_IDLE ) */

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while */

} /* cmcall_overall_call_end() */
#endif

#if !defined(FEATURE_MMODE_DISABLE_ACCESS_CONTROL) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
/*===========================================================================

FUNCTION cmcall_endall_non_emerg_calls

DESCRIPTION
  Ends all non-emergency calls.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_endall_non_emerg_calls
(
  cm_call_end_e_type end_reason,

  boolean local_hold_calls_only
  /*Indictes if only data call needs to be ended*/
)
{
  cmcall_s_type *call_ptr;                        /* Call object pointer */
  cm_iterator_type  call_itr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( call_ptr->call_state != CM_CALL_STATE_IDLE &&
         call_ptr->call_type  != CM_CALL_TYPE_EMERGENCY )
    {

      if(local_hold_calls_only == TRUE &&
         call_ptr->call_hold_state != CM_CALL_HOLD_STATE_LOCAL_HOLD)
      {
        call_ptr = cmcall_obj_get_next(&call_itr);
        continue;
      }

      call_ptr->end_status = end_reason;

      cmcall_end_call(call_ptr);
    }

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while */

} /* cmcall_endall_non_emerg_calls() */
#endif

#if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
/*===========================================================================

FUNCTION cmcall_endall_non_emerg_cs_calls

DESCRIPTION
  Ends all non-emergency calls.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_endall_non_emerg_cs_calls
(
  cm_call_end_e_type       end_reason,

  uint32                   asubs_id_mask
)
{
    cmcall_s_type *call_ptr;                        /* Call object pointer */
    cm_iterator_type  call_itr;
    cm_call_mode_info_e_type info_type;
    cm_call_id_type actv_callid = CM_CALL_ID_INVALID;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
    CM_MSG_HIGH_1("Ending non emergency CS calls on sub_mask: %d ",
                         asubs_id_mask);

    cmcall_obj_iterator_init(&call_itr);
  
    call_ptr = cmcall_obj_get_next(&call_itr);
  
    while(call_ptr != NULL)
    {
      if ( call_ptr->call_type  != CM_CALL_TYPE_EMERGENCY &&
           call_ptr->call_type  != CM_CALL_TYPE_PS_DATA &&
           BM(call_ptr->asubs_id) & asubs_id_mask)
      {
  
        call_ptr->end_status = end_reason;
        info_type = call_ptr->cmcall_mode_info.info_type;
        actv_callid = call_ptr->call_id;

        if(call_ptr->call_state != CM_CALL_STATE_IDLE)
        {
          #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
          if( info_type == CM_CALL_MODE_INFO_CDMA )
          {
            cmxcall_end_call(call_ptr->call_id,
                             call_ptr->end_status,
                             CAI_REL_NORMAL);
          }
          #endif
    
          
          #ifdef CM_GW_SUPPORTED
          if( info_type == CM_CALL_MODE_INFO_GW_CS )
          {
            cmwcall_end_call(call_ptr);
          }
          #endif /* CM_GW_SUPPORTED */
    
          #ifdef FEATURE_IP_CALL
          if(info_type == CM_CALL_MODE_INFO_IP )
          {
            cmipcall_send_end(call_ptr);
          }
          #endif /* FEATURE_IP_CALL */
        }
  
        /* By this time, call object may be deallocated depending on call type.
             ** If active call object, then free it along with its call pref on
             ** priority queue.
             */
        if( 			cmcall_ptr(actv_callid) != NULL 		)
        {

         #if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)

         cmcall_end( call_ptr );

          #endif
          
        }
      }
  
      call_ptr = cmcall_obj_get_next(&call_itr);
  
    } /* while */

  }


/*===========================================================================

FUNCTION cmcall_clean_voice_call_from_q

DESCRIPTION
  Ends all non-emergency calls.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_clean_voice_call_from_q()
{

  if ( cmcall_find_active_voice_call() == CM_CALL_ID_INVALID )
  {
    cm_orig_q_s_type  *call_stck_obj_ptr  =
                          cmtask_orig_para_search_act_type( CM_SS_MAIN,
                                                            CM_ACT_TYPE_VOICE_CALL);

    while( call_stck_obj_ptr != NULL )
    {
      cmph_delete_and_evaluate_ps_detach( CM_SS_MAIN,
                               call_stck_obj_ptr);
      call_stck_obj_ptr = cmtask_orig_para_search_act_type( CM_SS_MAIN,
                                        CM_ACT_TYPE_VOICE_CALL);
    }

    call_stck_obj_ptr = cmtask_orig_para_search_act_type( CM_SS_HYBR_2,
                                                          CM_ACT_TYPE_VOICE_CALL);
    while( call_stck_obj_ptr != NULL )
    {
      cmph_delete_and_evaluate_ps_detach( CM_SS_HYBR_2,
                               call_stck_obj_ptr);
      call_stck_obj_ptr = cmtask_orig_para_search_act_type( CM_SS_HYBR_2,
                                        CM_ACT_TYPE_VOICE_CALL);
    }

  }

} /* cmcall_clean_voice_call_from_q() */
#ifdef FEATURE_MMODE_3GPP2
/*===========================================================================

FUNCTION cmcall_clean_sms_call_from_q

DESCRIPTION
  Clear priority queue of SMS Calls if no SMS call in progress

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_clean_sms_call_from_q()
{

  if ( cmcall_find_active_sms_call() == CM_CALL_ID_INVALID )
  {
    cm_orig_q_s_type  *call_stck_obj_ptr  =
                          cmtask_orig_para_search_act_type( CM_SS_MAIN,
                                                            CM_ACT_TYPE_SMS_CALL);

    while( call_stck_obj_ptr != NULL )
    {
      cmph_delete_and_evaluate_ps_detach( CM_SS_MAIN,
                               call_stck_obj_ptr);
      call_stck_obj_ptr = cmtask_orig_para_search_act_type( CM_SS_MAIN,
                                        CM_ACT_TYPE_SMS_CALL);
    }

    call_stck_obj_ptr = cmtask_orig_para_search_act_type( CM_SS_HYBR_2,
                                                          CM_ACT_TYPE_SMS_CALL);
    while( call_stck_obj_ptr != NULL )
    {
      cmph_delete_and_evaluate_ps_detach( CM_SS_HYBR_2,
                               call_stck_obj_ptr);
      call_stck_obj_ptr = cmtask_orig_para_search_act_type( CM_SS_HYBR_2,
                                        CM_ACT_TYPE_SMS_CALL);
    }

  }

} /* cmcall_clean_sms_call_from_q() */
#endif
/*===========================================================================

FUNCTION cmcall_clean_cs_data_call_from_q

DESCRIPTION
  Clear priority queue of cs data Calls if no cs data call in progress

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_clean_cs_data_call_from_q()
{

  #if defined (FEATURE_3GPP_CSFB)
  if ( cmcall_find_active_cs_data_call() == CM_CALL_ID_INVALID )
  {
    cm_orig_q_s_type  *call_stck_obj_ptr  =
                          cmtask_orig_para_search_act_type( CM_SS_MAIN,
                                                            CM_ACT_TYPE_CS_DATA_CALL);

    while( call_stck_obj_ptr != NULL )
    {
      cmph_delete_and_evaluate_ps_detach( CM_SS_MAIN,
                               call_stck_obj_ptr);
      call_stck_obj_ptr = cmtask_orig_para_search_act_type( CM_SS_MAIN,
                                        CM_ACT_TYPE_CS_DATA_CALL);
    }

    call_stck_obj_ptr = cmtask_orig_para_search_act_type( CM_SS_HYBR_2,
                                                          CM_ACT_TYPE_CS_DATA_CALL);
    while( call_stck_obj_ptr != NULL )
    {
      cmph_delete_and_evaluate_ps_detach( CM_SS_HYBR_2,
                               call_stck_obj_ptr);
      call_stck_obj_ptr = cmtask_orig_para_search_act_type( CM_SS_HYBR_2,
                                        CM_ACT_TYPE_CS_DATA_CALL);
    }

  }
  #endif

} /* cmcall_clean_cs_data_call_from_q() */
#endif
#if 0 /* Not being used */
/*===========================================================================

FUNCTION cmcall_end_each_call_with_mode

DESCRIPTION
  Ends calls based on mode info.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_end_each_call_with_mode
(

  cm_call_mode_info_e_type call_mode_info,
  /* Given call mode info */

  cm_call_end_e_type call_end_reason
  /* Given reason for ending the call */
)
{

  cmcall_s_type *call_ptr;                        /* Call object pointer */
  cm_iterator_type  call_itr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( call_ptr->call_state != CM_CALL_STATE_IDLE &&
         call_ptr->cmcall_mode_info.info_type == call_mode_info )
    {
      call_ptr->end_status = call_end_reason;

      #ifdef CM_GW_SUPPORTED
      call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_PS;
      call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.cause_type = SM_INTERNAL_CAUSE;
      call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.cause.int_cause = SM_PS_DETACHED;
      #endif

      cmcall_end( call_ptr );
    }

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while */

} /* cmcall_end_each_call_with_mode() */
#endif

#if defined(FEATURE_MMODE_GW)
/*===========================================================================

FUNCTION cmcall_end_each_call_with_mode

DESCRIPTION
  Ends calls based on mode info.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_end_each_call_with_mode_per_sub
(

  cm_call_mode_info_e_type call_mode_info,
  /* Given call mode info */

  cm_call_end_e_type call_end_reason,
  /* Given reason for ending the call */

 sys_modem_as_id_e_type asubs_id
  /*SUB ID on which calls need to be ended
  ** None is wild-card, ends all*/

)
{

  cmcall_s_type *call_ptr;                        /* Call object pointer */
  cm_iterator_type  call_itr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( call_ptr->call_state != CM_CALL_STATE_IDLE &&
		 call_ptr->asubs_id == asubs_id             &&
         call_ptr->cmcall_mode_info.info_type == call_mode_info )
    {
      call_ptr->end_status = call_end_reason;

      #ifdef CM_GW_SUPPORTED
      call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_PS;
      call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.cause_type = SM_INTERNAL_CAUSE;
      call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.cause.int_cause = SM_PS_DETACHED;
      #endif

      cmcall_end( call_ptr );
    }

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while */

} /* cmcall_end_each_call_with_mode_per_sub() */
#endif

#if !defined(FEATURE_TRIM_CMPMPRX) && (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
/*===========================================================================

FUNCTION cmcall_end_each_call_with_type

DESCRIPTION
  Ends calls based on call type

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_end_each_call_with_type
(

  cm_call_type_e_type call_type,
  /* Given call mode info */

  cm_call_end_e_type call_end_reason
  /* Given reason for ending the call */
)
{

  /* Call object pointer */
  cmcall_s_type     *call_ptr;

  cm_iterator_type  call_itr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( call_ptr->call_type == call_type )
    {
      call_ptr->end_status = call_end_reason;
      /* If there is an active 1x data call send end to MC */
      #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
      if( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA &&
        call_ptr->call_state != CM_CALL_STATE_IDLE &&
        call_ptr->call_state != CM_CALL_STATE_CC_IN_PROGRESS
        )
      {
        cmxcall_end_call(call_ptr->call_id,
            call_ptr->end_status,
            CAI_REL_NORMAL);
      }
      else
      #endif
      {
      cmcall_end( call_ptr );
    }
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

} /* cmcall_end_each_call_with_type() */
#endif

#if (defined(FEATURE_MMODE_DUAL_SIM) || defined(FEATURE_MMODE_3GPP2)) || (defined(FEATURE_MMODE_GW))
/*===========================================================================

FUNCTION cmcall_end_each_call_with_type_per_sub

DESCRIPTION
  Ends calls based on call type

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_end_each_call_with_type_per_sub
(

  cm_call_type_e_type call_type,
  /* Given call mode info */

  cm_call_end_e_type call_end_reason,
  /* Given reason for ending the call */

  sys_modem_as_id_e_type asubs_id
  /*SUB ID on which calls need to be ended
  ** None is wild-card, ends all*/

)
{

  /* Call object pointer */
  cmcall_s_type     *call_ptr;

  cm_iterator_type  call_itr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( call_ptr->call_type == call_type &&
        (call_ptr->asubs_id == asubs_id ||
        call_ptr->asubs_id == SYS_MODEM_AS_ID_NONE))
    {
      call_ptr->end_status = call_end_reason;
      /* If there is an active 1x data call send end to MC */
      #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
      if( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA &&
	  	call_ptr->call_state != CM_CALL_STATE_IDLE &&
	  	call_ptr->call_state != CM_CALL_STATE_CC_IN_PROGRESS
	  	)
      {
        cmxcall_end_call(call_ptr->call_id,
            call_ptr->end_status,
            CAI_REL_NORMAL);
      }
      else
      #endif
      {
      cmcall_end( call_ptr );
    }
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

} /* cmcall_end_each_call_with_type() */
#endif

#if defined(FEATURE_MMODE_GW) || defined(FEATURE_CDMA)
/*===========================================================================

FUNCTION cmcall_end_gw_ps_calls_on_subsc_not_avail
DESCRIPTION
  Ends GW PS calls based on call type and call state.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_end_gw_ps_calls_on_subsc_not_avail(cm_ss_e_type ss)
{
  /* Call object pointer */
  cmcall_s_type     *call_ptr;

  cm_iterator_type  call_itr;

  boolean gw_ps_call_present = FALSE;
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  subs_id =  cmph_map_cm_ss_to_subs(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    /* End GW PS call if call is in silent redial or hold orig at CM */
    if ( call_ptr->call_type == CM_CALL_TYPE_PS_DATA  &&
         (call_ptr->asubs_id == subs_id ||
          call_ptr->asubs_id == SYS_MODEM_AS_ID_NONE) &&
         call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS  &&
         call_ptr->call_state == CM_CALL_STATE_ORIG &&
         call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE )
    {

      CM_MSG_HIGH_1 ("Ending GW PS call with call_id=%d due to subscription not available ",
                    call_ptr->call_id);

      call_ptr->end_status = CM_CALL_END_CLIENT_END;

      gw_ps_call_present = TRUE;
      cmcall_end( call_ptr );
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  if (gw_ps_call_present == TRUE)
  {
     cmph_remove_orig_mode( ss, CM_ACT_TYPE_DATA_CALL, CM_ACT_PRIORITY_MAX );

     if(ss == CM_SS_MAIN) {
       cmph_remove_orig_mode( CM_SS_HDR ,
                             CM_ACT_TYPE_DATA_CALL,
                             CM_ACT_PRIORITY_MAX);
     }
  }
} // cmcall_end_gw_ps_calls_on_subsc_not_avail()

/*===========================================================================

FUNCTION cmcall_end_gw_ps_call_in_hold_orig_or_sr
DESCRIPTION
  Ends 1x or GW ps data calls in orig state

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_end_ps_call_in_hold_orig_or_sr
(
 cm_mode_pref_e_type mode_pref,

 cm_ss_e_type                ss

)
{

  /* Call object pointer */
  cmcall_s_type     *call_ptr;

  cm_iterator_type  call_itr;

  boolean ps_call_present = FALSE;
  boolean ignore_asubs_id = FALSE;

  if (cmph_is_ssim())
  {
    ignore_asubs_id = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    /* End GW PS call if user changed mode pref while
    ** call is in silent redial or hold orig at CM
    */
    if ( call_ptr->call_type == CM_CALL_TYPE_PS_DATA  &&
         ( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS ||
           call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA )&&
         call_ptr->call_state == CM_CALL_STATE_ORIG &&
         call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE &&
         (call_ptr->asubs_id == cmph_map_cm_ss_to_subs(ss) ||
          ignore_asubs_id)
        )
    {
      CM_MSG_HIGH_2 ("Ending PS call with call_id=%d due to mode change %d",
                    call_ptr->call_id, mode_pref);

      call_ptr->end_status = CM_CALL_END_CLIENT_END;

      ps_call_present = TRUE;
      cmcall_end( call_ptr );
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  if (ps_call_present == TRUE)
  {
     cmph_remove_orig_mode( ss, CM_ACT_TYPE_DATA_CALL, CM_ACT_PRIORITY_MAX );

     cmph_remove_orig_mode( CM_SS_HDR ,
                             CM_ACT_TYPE_DATA_CALL,
                             CM_ACT_PRIORITY_MAX);
  }

} /* cmcall_end_ps_call_in_hold_orig_or_sr() */
#endif


/**--------------------------------------------------------------------------
** Functions - external:commands from bottom (cm?call)
** --------------------------------------------------------------------------
*/


/*===========================================================================

FUNCTION cmcall_ptr

DESCRIPTION
  Return a pointer to call object with given call id or NULL
  if such an object does not exist if there is a call Id mismatch.


DEPENDENCIES
  CM must be initialized

RETURN VALUE
  Pointer to requested call object or NULL if not found

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
cmcall_s_type  *cmcall_ptr( cm_call_id_type call_id )
{

  if(!cmcall_is_call_id_in_range(call_id))
  {
    return NULL;
  }

  return cm_call_obj_array[call_id];
} /* cmcall_ptr() */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

#if defined(FEATURE_MMODE_GW) && !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
/*===========================================================================

FUNCTION cmcall_srvcc_info_init

DESCRIPTION
  Initialize a call info buffer.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_srvcc_info_init( cm_mm_srvcc_call_info_s_type *srvcc_call_info_ptr )
{
  CM_ASSERT (srvcc_call_info_ptr       != NULL);

  /* First initialize the entire buffer to 0
  */
  memset( srvcc_call_info_ptr,
           CM_CALL_EVENT_DEFAULT_FIELD_VALUE,
           sizeof(cm_mm_srvcc_call_info_s_type) );

  /* Initialize the fields to default values
  */

  srvcc_call_info_ptr->call_id                     = CM_CALL_ID_UNASSIGNED;
  srvcc_call_info_ptr->parent_call_id              = CM_CALL_ID_UNASSIGNED;
  srvcc_call_info_ptr->is_mpty_call                = FALSE;
  srvcc_call_info_ptr->connection_id               = -1;

  srvcc_call_info_ptr->call_state                  = CM_CALL_STATE_NONE;
  srvcc_call_info_ptr->call_type                   = CM_CALL_TYPE_NONE;

  cm_num_init(&(srvcc_call_info_ptr->num));



  srvcc_call_info_ptr->sys_mode                    = SYS_SYS_MODE_NONE;

  srvcc_call_info_ptr->mode_info_type              = CM_CALL_MODE_INFO_NONE;

  srvcc_call_info_ptr->direction                   = CM_CALL_DIRECTION_NONE;

}
#endif

/*===========================================================================

FUNCTION cmcall_info_init

DESCRIPTION
  Initialize a call info buffer.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_info_init( cm_mm_call_info_s_type *call_info_ptr )
{
  CM_ASSERT (call_info_ptr       != NULL);

  /* First initialize the entire buffer to 0
  */
  memset( call_info_ptr,
           CM_CALL_EVENT_DEFAULT_FIELD_VALUE,
           sizeof(cm_mm_call_info_s_type) );

  /* Initialize the fields to default values
  */
  call_info_ptr->call_client_id              = CM_CLIENT_ID_ANONYMOUS;
  call_info_ptr->call_id                     = CM_CALL_ID_UNASSIGNED;
  call_info_ptr->call_state                  = CM_CALL_STATE_NONE;
  call_info_ptr->call_type                   = CM_CALL_TYPE_NONE;

  cm_num_init(&(call_info_ptr->num));
  #ifdef FEATURE_IP_CALL
  cm_num_init(&(call_info_ptr->waiting_num));
  cm_num_init(&(call_info_ptr->child_num));
  #endif

  #ifdef FEATURE_MMODE_SRVCC
  call_info_ptr ->mid_call_supported = FALSE;
  #endif
  
  call_info_ptr->alpha.len                   = CM_ALPHA_LEN_NONE;
  memset(call_info_ptr->alpha.buf,0,sizeof( call_info_ptr->alpha.buf) );

  call_info_ptr->enc_alpha.num_bytes         = CM_ALPHA_LEN_NONE;
  call_info_ptr->enc_alpha.coding_scheme     = CM_ALPHA_CODING_SCHEME_NONE;
  memset(call_info_ptr->enc_alpha.buf,0,sizeof( call_info_ptr->enc_alpha.buf) );

  cm_num_init(&(call_info_ptr->redirecting_number.num));

  call_info_ptr->signal.is_signal_info_avail = FALSE;
  call_info_ptr->signal.signal_type          = 0;
  call_info_ptr->signal.alert_pitch          = 0;
  call_info_ptr->signal.signal               = 0;


  call_info_ptr->end_status                  = CM_CALL_END_NONE;
  call_info_ptr->num_call_ids_ended          = 0;

  memset(&(call_info_ptr->end_params), 0,  sizeof( cm_end_params_s_type) );
  call_info_ptr->end_params.call_id          = CM_CALL_ID_UNASSIGNED;
  call_info_ptr->end_params.info_type        = CM_CALL_MODE_INFO_NONE;

  call_info_ptr->sys_mode                    = SYS_SYS_MODE_NONE;
  call_info_ptr->call_connect_time           = 0;
  call_info_ptr->call_end_time               = 0;

  memset(&(call_info_ptr->mode_info), 0, sizeof( call_info_ptr->mode_info) );
  call_info_ptr->mode_info.info_type         = CM_CALL_MODE_INFO_NONE;


  call_info_ptr->line                        = CM_ALS_LINE_NONE;
  call_info_ptr->requesting_client_id        = CM_CLIENT_ID_ANONYMOUS;

  #if (defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
       defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM))
    #error code not present
#endif

  call_info_ptr->direction                   = CM_CALL_DIRECTION_NONE;
  call_info_ptr->modify                      = CM_CALL_MODIFY_NONE;

  call_info_ptr->mode_info.info.cdma_call.line_ctrl_polarity_included
                                        = FALSE;
  call_info_ptr->mode_info.info.cdma_call.line_ctrl_toggle
                                        = FALSE;
  call_info_ptr->mode_info.info.cdma_call.line_ctrl_reverse
                                        = FALSE;
  call_info_ptr->mode_info.info.cdma_call.line_ctrl_power_denial
                                        = 0;
  call_info_ptr->mode_info.info.cdma_call.orig_fail_layer
                                        = CM_CALL_ORIG_ERR_LAYER_NONE;
  call_info_ptr->mode_info.info.cdma_call.con_ref
                                        = CM_NULL_CON_REF;

  call_info_ptr->voice_ho_type          = SYS_VOICE_HO_NONE;

  call_info_ptr->client_categ           = CM_ORIG_CLIENT_CATEG_NONE;

  call_info_ptr->vsid           = 0;

}

/*===========================================================================

FUNCTION cmcall_info_alloc

DESCRIPTION
  Allocate a call info buffer.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated call info struct.

SIDE EFFECTS
  none

===========================================================================*/
cm_mm_call_info_s_type  *cmcall_info_alloc( void )
{
  cm_mm_call_info_s_type  *call_info_ptr;

  /*
  ** Dynamically allocate a call info object
  */
  call_info_ptr = (cm_mm_call_info_s_type *)cm_mem_malloc(sizeof(cm_mm_call_info_s_type));
  
  cmcall_info_init(call_info_ptr);

  /* Simply return a pointer to the dynamically allocated
  ** call info buffer.
  */
  return call_info_ptr;

} /* cmcall_info_alloc() */

/*===========================================================================

FUNCTION cmcall_get_asubs_id

DESCRIPTION
  Get asubs_id for the call obj.

DEPENDENCIES
  None.

RETURN VALUE
  sys_modem_as_id_e_type.

SIDE EFFECTS
  None.
===========================================================================*/
sys_modem_as_id_e_type cmcall_get_asubs_id (
  const  cmcall_s_type  *call_ptr
)
{
  CM_ASSERT(call_ptr    != NULL);

  if (call_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
    return SYS_MODEM_AS_ID_NONE;
  }
  
  #ifdef FEATURE_MMODE_DUAL_SIM
  if (cmph_is_msim())
  {
    return call_ptr->asubs_id;
  }
  else
  #endif
  {
    return SYS_MODEM_AS_ID_1;
  }
}

#if defined(FEATURE_MMODE_GW) && !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
/*===========================================================================

FUNCTION cmcall_srvcc_info_alloc

DESCRIPTION
  Allocate a srvcc call info buffer.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated call info struct.

SIDE EFFECTS
  none

===========================================================================*/
cm_mm_srvcc_call_info_s_type  *cmcall_srvcc_info_alloc( void )
{
  cm_mm_srvcc_call_info_s_type  *srvcc_call_info_ptr;

  /*
  ** Dynamically allocate a call info object
  */
  srvcc_call_info_ptr = (cm_mm_srvcc_call_info_s_type *)cm_mem_malloc(sizeof(cm_mm_srvcc_call_info_s_type));

  cmcall_srvcc_info_init(srvcc_call_info_ptr);

  /* Simply return a pointer to the dynamically allocated
  ** srvcc call info buffer.
  */
  return srvcc_call_info_ptr;

} /* cmcall_srvcc_info_alloc() */
#endif
/*===========================================================================

FUNCTION cmcall_update_call_end_info

DESCRIPTION
  This function updates the call object information related to the
  CM_CALL_EVENT_END_REQ and CM_CALL_EVENT_END call events.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_update_call_end_info(

  cmcall_s_type           *call_ptr
      /* pointer to a call object. */
)
{
  cm_get_data_session_total_byte_ctrs_cb_f_type *ctrs_cb_func_ptr = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** In GSM/WCDMA, it is possible that the CM_CALL_EVENT_END_REQ and
  ** the CM_CALL_EVENT_END events will be sent during the end of a
  ** call.  If the call end time is not zero, then this implies that
  ** the CM_CALL_EVENT_END_REQ call event was previously sent.  If so,
  ** then is is not necessary to update the call object information.
  */
  if ( call_ptr->call_end_time == 0 )
  {
    call_ptr->call_end_time     = time_get_uptime_secs();
  } /* if ( call_ptr->call_end_time != 0 */

} /* cmcall_update_call_end_info */

/*===========================================================================

FUNCTION cmcall_assert

DESCRIPTION
  Check a series of assertions over the call object to make sure that
  none of its fields are corrupted and to validate its state.

  Check fields that are common across modes of operation.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_assert(

    const cmcall_s_type    *call_ptr
        /* pointer to a call object */
)
/*lint -esym(715, call_ptr)
** NOTE -save/-restore do not work with -esym()
*/
{

  #ifdef CM_DEBUG
  #error code not present
#endif /* CM_DEBUG */

} /* cmcall_assert() */
/*lint +esym(715, call_ptr)
** NOTE -save/-restore do not work with -esym()
*/
#ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
static uint8 cmcall_map_ton_to_num_type(uint8 ton_value)
{
  uint8 number_type;

  switch(ton_value)
  {
  case CM_TON_UNKNOWN:
  case CM_TON_INTERNATIONAL:
  case CM_TON_NATIONAL:
  case CM_TON_NETWORK_SPECIFIC:
    number_type = (ton_value >> 4);
    break;

  default:
    number_type = (uint8)CM_NUM_TYPE_UNKNOWN;
    break;
  }
  CM_MSG_HIGH_2 ("cmcall_map_ton_to_num_type: ton_value = %d to  number_type = %d",
                          ton_value, number_type);
  return number_type;
}
#endif
/*===========================================================================

FUNCTION cmcall_copy_cc_result

DESCRIPTION
  Copy the call caontrol result into
  call_ptr


DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if call information is valid, FALSE otherwise.

  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_copy_cc_result
(
  const cmcall_s_type          *call_ptr,
      /* Point at call object */

  cm_mm_call_info_s_type       *call_info_ptr
      /* Copy call state info into this buffer */

)

{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_info_ptr != NULL );
  CM_ASSERT( call_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ( call_ptr->cmcall_mode_info.info_type )
  {
    #ifdef FEATURE_MMODE_3GPP
    case CM_CALL_MODE_INFO_GW_CS:
    case CM_CALL_MODE_INFO_GW_PS:
    case CM_CALL_MODE_INFO_IP:

      call_info_ptr->result_from_cc.alpha = call_ptr->gw_cc_modified_info.alpha;
      call_info_ptr->result_from_cc.num = call_ptr->gw_cc_modified_info.num;

      #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
      if( (call_ptr->is_modem_cc_api == FALSE) &&
          (call_ptr->gw_cc_modified_info.call_control_result !=
                                       CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS) )
      {
        call_info_ptr->result_from_cc.num.number_type =
          cmcall_map_ton_to_num_type(call_ptr->gw_cc_modified_info.num.number_type);
      }
      #endif
      call_info_ptr->result_from_cc.call_control_result =
        call_ptr->gw_cc_modified_info.call_control_result;
      break;
    #endif

    default:
      break;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
}
#ifdef FEATURE_MMODE_SRVCC
/*===========================================================================

FUNCTION cmcall_srvcc_info_get

DESCRIPTION
  Copy the current call state information into a specified buffer to
  during L->GW SRVCC

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if call information is valid, FALSE otherwise.

  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_srvcc_info_get(

    const cmcall_s_type          *call_ptr,
        /* Point at call object */

    cm_mm_srvcc_call_info_s_type       *srvcc_call_info_ptr
        /* Copy call state info into this buffer */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( srvcc_call_info_ptr != NULL );
  CM_ASSERT( call_ptr != NULL );

  /* Verify that object was properly initialized.
  */
  CM_ASSERT( CM_INIT_CHECK(call_ptr->init_mask) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Verify the integrity of call object's common fields.
  */
  cmcall_assert( call_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy common call parameters into call_info struct.
  */

  srvcc_call_info_ptr->call_id                = cmcall_call_id_get( call_ptr );
  srvcc_call_info_ptr->parent_call_id     = call_ptr->parent_call_id;
  srvcc_call_info_ptr->call_state             = call_ptr->call_state;
  srvcc_call_info_ptr->call_type              = call_ptr->call_type;
  srvcc_call_info_ptr->num                    = call_ptr->num;
  srvcc_call_info_ptr->connection_id           = call_ptr->gw_connection_id;


  srvcc_call_info_ptr->sys_mode         = call_ptr->sys_mode;

  srvcc_call_info_ptr->direction              = call_ptr->direction;

  srvcc_call_info_ptr->mode_info_type     = call_ptr->cmcall_mode_info.info_type;

  srvcc_call_info_ptr->call_sub_state         =call_ptr->call_subst;

  srvcc_call_info_ptr->is_mpty_call           =call_ptr->is_mpty;

  srvcc_call_info_ptr->alert_media_type     = call_ptr->alert_media_type;

  #ifdef FEATURE_MMODE_DUAL_SIM
  /* If client_asubs_id is unassigned, then continue to use asubs_id */
  if(call_ptr->client_asubs_id == SYS_MODEM_AS_ID_NONE)
  {
    srvcc_call_info_ptr->asubs_id               = call_ptr->asubs_id;
  }
  else
  {
    srvcc_call_info_ptr->asubs_id = call_ptr->client_asubs_id;
  }
  #endif

  /* Return boolean indicating whether call information is valid.
  */
  return CM_BOOLEAN( CM_INIT_CHECK(call_ptr->init_mask) );

} /* cmcall_srvcc_info_get() */

#endif

/*===========================================================================

FUNCTION cmcall_info_get

DESCRIPTION
  Copy the current call state information into a specified buffer.
  Copy fields that are common accross modes of operation.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if call information is valid, FALSE otherwise.

  Note that call information only becomes valid after cmcall_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_info_get(

    const cmcall_s_type          *call_ptr,
        /* Point at call object */

    cm_mm_call_info_s_type       *call_info_ptr
        /* Copy call state info into this buffer */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_info_ptr != NULL );
  CM_ASSERT( call_ptr != NULL );

  /* Verify that object was properly initialized.
  */
  CM_ASSERT( CM_INIT_CHECK(call_ptr->init_mask) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Verify the integrity of call object's common fields.
  */
  cmcall_assert( call_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy common call parameters into call_info struct.
  */
  call_info_ptr->call_client_id         = cmclient_get_client_id( call_ptr->call_client_ptr );
  call_info_ptr->call_id                = cmcall_call_id_get( call_ptr );
  call_info_ptr->call_state             = call_ptr->call_state;
  call_info_ptr->call_type              = call_ptr->call_type;
  call_info_ptr->num                    = call_ptr->num;

  #ifdef FEATURE_IP_CALL
  call_info_ptr->waiting_num            = call_ptr->waiting_num;
  call_info_ptr->mode_info.info.ip_call.modify_error_code = call_ptr->cmcall_mode_info.info.ip_info.modify_error_code;
  call_info_ptr->mode_info.info.ip_call.modify_cause_code = call_ptr->cmcall_mode_info.info.ip_info.modify_cause_code;
  call_info_ptr->child_num              = call_ptr->child_num;
  call_info_ptr->mode_info.info.ip_call.call_attrib_info.display_text
      = call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.display_text;
  call_info_ptr->mode_info.info.ip_call.caller_info
  	  = call_ptr->cmcall_mode_info.info.ip_info.caller_info;
  #endif
  
  call_info_ptr->local_hold_state       = call_ptr->call_hold_state;
  call_info_ptr->redirecting_number     = call_ptr->redirecting_number;
  call_info_ptr->alpha                  = call_ptr->alpha;

  call_info_ptr->enc_alpha              = call_ptr->enc_alpha;

  call_info_ptr->signal                 = call_ptr->signal;
  call_info_ptr->end_status             = call_ptr->end_status;
  call_info_ptr->num_call_ids_ended     = 1;
  call_info_ptr->sys_mode               = call_ptr->sys_mode;
  call_info_ptr->call_connect_time      = call_ptr->call_connect_time;
  call_info_ptr->call_end_time          = call_ptr->call_end_time;
  call_info_ptr->line                   = call_ptr->line;
  call_info_ptr->direction              = call_ptr->direction;
  call_info_ptr->mode_info.info_type    = call_ptr->cmcall_mode_info.info_type;
  call_info_ptr->end_params.call_id     = call_ptr->end_params.call_id;
  call_info_ptr->end_params.info_type   = call_ptr->end_params.info_type;
  call_info_ptr->end_params             = call_ptr->end_params;
  call_info_ptr->speech_codec_info      = call_ptr->speech_codec_info;
  call_info_ptr->voice_ho_type          = call_ptr->voice_ho_type;
  call_info_ptr->client_categ           = call_ptr->client_categ;

  call_info_ptr->cm_mt_verstat.is_nw_support_unwanted_call = (uint8)call_ptr->mt_call_verstat.is_nw_support_unwanted_call;
  call_info_ptr->cm_mt_verstat.call_verstat= (uint8)call_ptr->mt_call_verstat.call_verstat;

  /* If client_asubs_id is unassigned, then continue to use asubs_id */
  if(call_ptr->client_asubs_id == SYS_MODEM_AS_ID_NONE)
  {
    call_info_ptr->asubs_id               = call_ptr->asubs_id;
  }
  else
  {
    call_info_ptr->asubs_id = call_ptr->client_asubs_id;
  }

  cmcall_copy_cc_result(call_ptr,call_info_ptr);


  switch ( call_ptr->cmcall_mode_info.info_type )
  {
    case CM_CALL_MODE_INFO_NONE:
      break;

    #if defined (FEATURE_IP_CALL) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
    case CM_CALL_MODE_INFO_IP:
      /* copying info specific to ip call
      */
      call_info_ptr->mode_info.info.ip_call.call_prog_info =
         call_ptr->cmcall_mode_info.info.ip_info.call_prog_info;
      call_info_ptr->mode_info.info.ip_call.call_attrib_info =
         call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info;

      if(call_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
      {
        CM_MSG_HIGH_2("Get em_type %d and is_em_vt %d",
        call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.em_type,
        call_ptr->is_em_vt); 
      }

      call_info_ptr->mode_info.info.ip_call.ipcall_local_call_cap_attrib_info =
          call_ptr->cmcall_mode_info.info.ip_info.ipcall_local_cap_attrib_info;

      call_info_ptr->mode_info.info.ip_call.ipcall_peer_call_cap_attrib_info =
          call_ptr->cmcall_mode_info.info.ip_info.ipcall_peer_cap_attrib_info;

      call_info_ptr->mode_info.info.ip_call.call_modif_err_status =
         call_ptr->cmcall_mode_info.info.ip_info.call_modif_err_status;

      call_info_ptr->mode_info.info.ip_call.req_modify_call_attrib =
         call_ptr->cmcall_mode_info.info.ip_info.req_modify_call_attrib;
      call_info_ptr->mode_info.info.ip_call.req_modify_call_type=
         call_ptr->cmcall_mode_info.info.ip_info.req_modify_call_type;
		 
	  call_info_ptr->mode_info.info.ip_call.ims_end_reason=
         call_ptr->cmcall_mode_info.info.ip_info.ims_end_reason;
      call_info_ptr->mode_info.info.ip_call.switch_codec_status =
	  	call_ptr->cmcall_mode_info.info.ip_info.switch_codec_cmd_status;

      call_info_ptr->mode_info.info.ip_call.sip_error_code =
        call_ptr->cmcall_mode_info.info.ip_info.sip_error_code;

      call_info_ptr->mode_info.info.ip_call.alternate_uri=
        call_ptr->cmcall_mode_info.info.ip_info.alternate_uri;

      break;
    #endif /* FEATURE_IP_CALL */

    #if defined(CM_GW_SUPPORTED)
    case CM_CALL_MODE_INFO_GW_CS:
      (void) cmwcall_info_get( call_info_ptr, call_ptr);
      break;
    #endif

    #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
    case CM_CALL_MODE_INFO_CDMA:
      (void) cmxcall_info_get( call_ptr, call_info_ptr );
      break;
    #endif

    default:
      sys_err_fatal_invalid_value_exception( (uint8) call_ptr->cmcall_mode_info.info_type );
      /*
      ** 527: Unreachable
      ** This is because CM_ERR_FATAL() evaluates to error fatal
      ** (i.e exit program)
      ** But for different feature set it may not to, so we want to
      ** supress the warning here
      */
      /*lint -save -e527 */
      return FALSE;
      /*lint -restore */
  } /* switch ( call_ptr->cmcall_mode_info.info_type ) */

  /* Fill voice to ss params */
  call_info_ptr->result_from_cc.voice_to_ss.ss_code = call_ptr->voice_to_ss.ss_code;
  call_info_ptr->result_from_cc.voice_to_ss.ss_oper = call_ptr->voice_to_ss.ss_oper;
  call_info_ptr->result_from_cc.voice_to_ss.ss_ref  = call_ptr->voice_to_ss.ss_ref;
  call_info_ptr->result_from_cc.voice_to_ss.uss_data = call_ptr->voice_to_ss.uss_data;

  /* Return boolean indicating whether call information is valid.
  */


  return CM_BOOLEAN( CM_INIT_CHECK(call_ptr->init_mask) );

} /* cmcall_info_get() */
/*===========================================================================

FUNCTION cmcall_call_state_on_ss

DESCRIPTION
  Checks if call event related to call state change

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static sp_call_states_enum_t cmcall_call_state_on_ss(

  cm_ss_e_type    ss

)
{

  cmcall_s_type   *call_ptr;
  /* Call object pointer
   */
  cm_iterator_type call_itr;
   /* call Object Iterator
    */

  sp_call_states_enum_t call_state = SP_NO_CALL_STATE;

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if((call_ptr->ss == ss ) && (call_ptr->call_type == CM_CALL_TYPE_VOICE)
        && (call_ptr->call_state != CM_CALL_STATE_IDLE))
    {
      if(call_ptr->call_hold_state == CM_CALL_HOLD_STATE_LOCAL_HOLD)
      {
        return SP_VOICE_CALL_HOLD_STATE;
      }

      if((call_ptr->call_state == CM_CALL_STATE_CONV)
           && call_ptr->call_subst.conv == CM_CALL_CONV_CALL_ON_HOLD)
      {
        call_state = SP_VOICE_CALL_HOLD_STATE;
      }

      return SP_VOICE_CALL_ACTIVE_STATE;
    }

    if((call_ptr->ss == ss ) && (call_ptr->call_type == CM_CALL_TYPE_PS_DATA)
       && (call_state == SP_NO_CALL_STATE))
    {
      call_state = SP_DATA_CALL_STATE;
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while  */

  return call_state;
}


#if defined(FEATURE_MMODE_DUAL_SIM) || defined(FEATURE_MMODE_SC_SVLTE)
/*===========================================================================
FUNCTION cmcall_is_lte_active_status_ind_reqd

DESCRIPTION
   Tells if lte active status needs to be sent to IMS.
   Currently indication is to be sent only for 
   DSDS (Sub2 GSM call)  and SRLTE (1x call)

DEPENDENCIES
  

RETURN VALUE
  boolean status
  True : Send indication to IMS
  False : Do not send indication to IMS

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmcall_is_lte_active_status_ind_reqd(
    cmcall_s_type    *call_ptr)
{
  boolean ret_val = FALSE;
  cmcall_s_type *call_ptr_itr = NULL;
  cm_iterator_type call_itr;
  boolean call_itr_ptr_ret_status = FALSE;
  sys_modem_as_id_e_type mm_asubs_id = cmph_get_sub_with_lte_cap();

  /*-----------------------------------------------------------------------*/
  cmcall_obj_iterator_init(&call_itr);
  call_ptr_itr = cmcall_obj_get_next(&call_itr);

  while(call_ptr_itr != NULL)
  {
    if(call_ptr_itr->call_type == CM_CALL_TYPE_VOICE || 
      call_ptr_itr->call_type == CM_CALL_TYPE_EMERGENCY ||
      call_ptr_itr->call_type == CM_CALL_TYPE_CS_DATA)
    {
      if((cmph_is_dsds() || cmph_is_tsts()) &&
         (cmph_ptr()->internal_standby_pref != SYS_MODEM_DS_PREF_SINGLE_STANDBY) &&
         (call_ptr_itr->asubs_id != mm_asubs_id)
        )
      {
        CM_MSG_HIGH_0("Multi Sim non mm sub CS call");
        ret_val = call_itr_ptr_ret_status = TRUE;
      }
      else if(cmph_is_oprting_in_1xsrlte_mode(call_ptr_itr->asubs_id) &&
             (call_ptr_itr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA)
             )
      {
        CM_MSG_HIGH_0("SRLTE 1X call");
        ret_val = call_itr_ptr_ret_status = TRUE;
      }

      if((call_itr_ptr_ret_status == TRUE) && 
        call_ptr_itr->call_id != call_ptr->call_id)
      {
        CM_MSG_HIGH_2("call id %d, exists other than current call with id %d",call_ptr_itr->call_id,call_ptr->call_id);
        ret_val = FALSE;
        break;
      }
    }
    call_itr_ptr_ret_status = FALSE;
    call_ptr_itr = cmcall_obj_get_next(&call_itr);
  }
    
  return ret_val;
}


/*===========================================================================
FUNCTION cmcall_send_lte_active_status_ind

DESCRIPTION
   Sends LTE active status indication

DEPENDENCIES
  

RETURN VALUE
  boolean status
  True : Send indication to IMS
  False : Do not send indication to IMS

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_send_lte_active_status_ind(
    boolean status,
    sys_modem_as_id_e_type as_id
)
{
  cm_lte_active_status_ind_s_type  *lte_act_stat_ind = NULL;

  CM_MSG_HIGH_2("CM->MSGR: LTE_ACTIVE_STATUS_IND with status %d, as_id %d",
                status,as_id);

  lte_act_stat_ind = (cm_lte_active_status_ind_s_type *)cm_mem_malloc
                      (sizeof(cm_lte_active_status_ind_s_type));
  if(lte_act_stat_ind != NULL)
  {
    lte_act_stat_ind->status = status;
    lte_act_stat_ind->hdr.inst_id = SYS_AS_ID_TO_INST_ID(as_id);
     (void) cm_msgr_send( MM_CM_LTE_ACTIVE_STATUS_IND, MSGR_MM_CM,
                         (msgr_hdr_s*)lte_act_stat_ind, sizeof(cm_lte_active_status_ind_s_type) );
    cm_mem_free (lte_act_stat_ind);
  }
  else
  {
    CM_ERR_0("Unable to allocate memory for lte_act_stat_ind");
  }
}


/*===========================================================================
FUNCTION cmcall_check_and_send_lte_active_status_ind

DESCRIPTION
   Check is lte active status ind needs to be sent or not
    and calls indication send function.
DEPENDENCIES
  

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_check_and_send_lte_active_status_ind(
    cmcall_s_type    *call_ptr,
    cm_call_event_e_type    call_event)
{
  if(call_ptr != NULL)
  {  
    if(cmcall_is_lte_active_status_ind_reqd(call_ptr))
    {
      if(call_event == CM_CALL_EVENT_CONNECT)
      {
        cmcall_send_lte_active_status_ind(FALSE, call_ptr->asubs_id);
      }
      else
      {
        cmcall_send_lte_active_status_ind(TRUE, call_ptr->asubs_id);
      }
    }
  }
}
#endif

/*===========================================================================
FUNCTION cmcall_send_mt_call_end_auto_reject_ind

DESCRIPTION
   Sends MT call end indication auto rejected by modem to QMI. 

DEPENDENCIES


RETURN VALUE
None

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_send_mt_call_end_auto_reject_ind(
	sys_modem_as_id_e_type      as_id,
	cm_call_type_e_type         call_type,
	uint16						sip_error_code,
	cm_call_end_e_type			end_cause_to_client,
	cm_num_s_type			    calling_party_num,
	cm_call_mode_info_e_type    mode_info_type
)
{
  cm_mt_call_end_auto_reject_modem_ind_s_type  *mt_call_end_auto_reject_ind = NULL;
  errno_enum_type msgr_error;

  CM_MSG_HIGH_2("CM->MSGR: MM_CM_MT_CALL_END_AUTO_REJECT sent to QMI_VOICE for call_type %d,  client_end_cause %d ",\
                 call_type, end_cause_to_client);


  mt_call_end_auto_reject_ind  = (cm_mt_call_end_auto_reject_modem_ind_s_type *) cm_mem_malloc
                                (sizeof(cm_mt_call_end_auto_reject_modem_ind_s_type));
  
  if(mt_call_end_auto_reject_ind != NULL)
  {

	mt_call_end_auto_reject_ind->sip_error_code = sip_error_code;
	mt_call_end_auto_reject_ind->end_cause_to_client = end_cause_to_client;
	mt_call_end_auto_reject_ind->call_type = call_type;
	mt_call_end_auto_reject_ind->as_id = as_id;
	mt_call_end_auto_reject_ind->mode_info_type = mode_info_type;

	memset((byte *)&mt_call_end_auto_reject_ind->num, 0, sizeof(mt_call_end_auto_reject_ind->num));
	
	if(calling_party_num.len != 0 )
	{
	  mt_call_end_auto_reject_ind->num.len = memscpy(mt_call_end_auto_reject_ind->num.buf,
								  sizeof(mt_call_end_auto_reject_ind->num.buf),
								  calling_party_num.buf,
								  calling_party_num.len);
	}
	
	/* Initialize pi in call object
	*/
	mt_call_end_auto_reject_ind->num.pi = calling_party_num.pi;

    msgr_error = cm_msgr_send( MM_CM_MT_CALL_END_AUTO_REJECT_IND, MSGR_MM_CM,
                         (msgr_hdr_s *)mt_call_end_auto_reject_ind, sizeof(cm_mt_call_end_auto_reject_modem_ind_s_type));

    CM_MSG_HIGH_2("CM->MSGR: MM_CM_MT_CALL_END_AUTO_REJECT msgr_error %d, client_end_cause %d ",\
                   msgr_error, mt_call_end_auto_reject_ind->end_cause_to_client);

    cm_mem_free (mt_call_end_auto_reject_ind);
  }
  else
  {
    CM_ERR_0("Unable to allocate memory for mt_call_end_auto_reject_ind");
  }
}/*cmcall_send_mt_call_end_auto_reject_ind*/


/*===========================================================================

FUNCTION cmcall_event

DESCRIPTION
  Notify the client list of a specified call event.
  This function is for events that apply to a particular call
  (call with a specific call id)

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_event(

    cmcall_s_type    *call_ptr,
        /* pointer to a call object. */

    cm_call_event_e_type    call_event
        /* notify client list of this call event */
)
/*lint -esym(818, call_ptr) could be declared as pointing to const
** Nope, not when GW flags are defined as then we set fields in call_ptr
** through wcall_mode_ptr and info_mode_ptr
*/
{

  cmcall_event_2( call_ptr,
                  call_event,
                  NULL );
}
/*lint +esym(818, call_ptr) could be declared as pointing to const */


/*===========================================================================

FUNCTION cmcall_event_2

DESCRIPTION
  Notify the client list of a specified call event.
  This function is for events that apply to a particular call
  (call with a specific call id)
  For the following events, there's no need to specify the call id:
  CM_CALL_EVENT_GET_PDN_CONN_IND,  CM_CALL_EVENT_PDN_CONN_REJ_IND,
  CM_CALL_EVENT_PDN_CONN_FAIL_IND, CM_CALL_EVENT_PDN_DISCONNECT_REJECT_IND,
  CM_CALL_EVENT_RES_ALLOC_REJ_IND, CM_CALL_EVENT_RES_ALLOC_FAIL_IND, and
  CM_CALL_EVENT_BEARER_MOD_REJ_IND.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_event_2(

    cmcall_s_type    *call_ptr,
        /* pointer to a call object. */

    cm_call_event_e_type    call_event,
        /* notify client list of this call event */

    const void   *cm_rpt_ptr
        /* report pointer */
)
/*lint -esym(818, call_ptr) could be declared as pointing to const
** Nope, not when GW flags are defined as then we set fields in call_ptr
** through wcall_mode_ptr and info_mode_ptr
*/
{
  cm_mm_call_info_s_type     *call_info_ptr = NULL;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!BETWEEN(call_event, CM_CALL_EVENT_NONE, CM_CALL_EVENT_MAX) )
  {
    CM_ERR_1("Invalid call_event %d", call_event);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_info_ptr = cmcall_info_alloc();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(call_event)
  {
    /* When incoming page indiction comes in, CM does not create call id or
    ** call obj. CM will report incoming page indication event to clients.
    ** The only valid field in the call event payload is the service option.
    */
    case CM_CALL_EVENT_INCOMING_PAGE:
    {
      #if (defined (FEATURE_MMODE_CDMA_800) || defined (FEATURE_MMODE_CDMA_1900))
      const cm_mc_rpt_type        *cm_mc_rpt_ptr = cm_rpt_ptr;

      call_info_ptr->call_type = CM_CALL_TYPE_PS_DATA;
      call_info_ptr->sys_mode  = SYS_SYS_MODE_CDMA;
      call_info_ptr->mode_info.info_type = CM_CALL_MODE_INFO_CDMA;
      if(cm_mc_rpt_ptr)
      {
        call_info_ptr->mode_info.info.cdma_call.srv_opt = cm_mc_rpt_ptr->incoming_page.so;
      }
      #endif
      break;
    }
    default:
    if( call_ptr == NULL )
    {
      return;
    }

    #ifdef FEATURE_HDR_HANDOFF
    /* Update our opinion of the base station's IDM.
    */
    if( call_ptr->call_type == CM_CALL_TYPE_PS_DATA )
    {
      cmssidm_estimate_base_station_idm( call_ptr );
    }
    #endif

    /* Copy call information from call object into call info structure
    */
    (void) cmcall_info_get(call_ptr, call_info_ptr);

    #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  /* if MC has send the SRV_OPT along with the call end reports, then derive the call type from SO */
  if( (call_event == CM_CALL_EVENT_END) && (call_info_ptr->mode_info.info_type == CM_CALL_MODE_INFO_CDMA)
    && (call_ptr->cmcall_mode_info.info.cdma_info.is_so_to_call_type_req == TRUE) )
  {
     call_info_ptr->call_type = cmxcall_determine_call_type_from_so(call_ptr->cmcall_mode_info.info.cdma_info.srv_opt);
     call_ptr->cmcall_mode_info.info.cdma_info.is_so_to_call_type_req = FALSE;
  }
  #endif

    #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA))
    if (call_event == CM_CALL_EVENT_ORIG_MOD_TO_SS )
    {
      call_info_ptr->result_from_cc.alpha = call_ptr->gw_cc_modified_info.alpha;
      call_info_ptr->result_from_cc.num   = call_ptr->gw_cc_modified_info.num;
      call_info_ptr->result_from_cc.call_control_result =
                  call_ptr->gw_cc_modified_info.call_control_result;

      CM_MSG_HIGH_2 ( "ORIG_MOD_TO_SS: cc_result = %d,Alpha length = %d",
                     call_info_ptr->result_from_cc.call_control_result,
                     call_info_ptr->result_from_cc.alpha.len );
    }
    #endif

    SYS_ARG_NOT_USED(cm_rpt_ptr);
      break;
  }//switch(call_event)

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For backwards compatibility issues....                                */
  switch (call_info_ptr->end_status)
  {
    case CM_CALL_END_NO_GW_SRV:
    case CM_CALL_END_NO_CDMA_SRV:
      call_info_ptr->end_status = CM_CALL_END_NO_SRV;
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case CM_CALL_END_HDR_FADE:
    case CM_CALL_END_HDR_CS_TIMEOUT:
      call_info_ptr->end_status = CM_CALL_END_FADE;
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case CM_CALL_END_HDR_ACC_FAIL:
      call_info_ptr->end_status = CM_CALL_END_ACC_FAIL;
      break;

    default:
      break;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_DCVS_LPM
  if( call_event == CM_CALL_EVENT_CONNECT && call_ptr->call_type ==  CM_CALL_TYPE_VOICE )
  {
    clk_regime_register_for_cpu_resource( CLKRGM_CDMA_VOICE_CALL );
  }

  if(call_event == CM_CALL_EVENT_END && call_ptr->call_type ==  CM_CALL_TYPE_VOICE )
  {
    clk_regime_deregister_for_cpu_resource( CLKRGM_CDMA_VOICE_CALL );
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( call_event == CM_CALL_EVENT_CONNECT)
  {
    /* since call got connected, prot_end_status buffered is not valid anymore */
    call_ptr->prot_end_status = CM_CALL_END_NONE;
  }

  #ifdef FEATURE_IP_CALL
  if( (call_event == CM_CALL_EVENT_MODIFY_COMPLETE_CONF) &&
  (call_ptr->cmcall_mode_info.info.ip_info.call_modif_err_status) &&
  (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS))
  {
    /* this is a temp fix, and design should be discussed with IMS to do changes at their end, not to send modify_complete
    after call is transfered to GW*/
    /* even if mode_info is changed to GW_CS we need to send proper modif_err_status and code to QMI-V*/
    call_info_ptr->mode_info.info.ip_call.call_modif_err_status =
         call_ptr->cmcall_mode_info.info.ip_info.call_modif_err_status;
    call_info_ptr->mode_info.info.ip_call.modify_error_code = call_ptr->cmcall_mode_info.info.ip_info.modify_error_code;
    CM_MSG_HIGH_2("Overwriting call info for modify_comp, modify cnf err status = %d, err_code=%d ",
                           call_info_ptr->mode_info.info.ip_call.call_modif_err_status,
                           call_info_ptr->mode_info.info.ip_call.modify_error_code);
  }
  if (call_event == CM_CALL_EVENT_MODIFY_COMPLETE_CONF)
  {
    call_info_ptr->mode_info.info.ip_call.modify_cause_code = call_ptr->cmcall_mode_info.info.ip_info.modify_cause_code;
  }
  #endif


  #if defined(FEATURE_MMODE_DUAL_SIM) || defined(FEATURE_MMODE_SC_SVLTE)
  /* check if need to send lte active status to IMS */
  if(call_event == CM_CALL_EVENT_CONNECT || 
    call_event == CM_CALL_EVENT_END)
  {
    cmcall_check_and_send_lte_active_status_ind(call_ptr, call_event);
  }
  #endif

  if(cm_get_ulog_handle() != NULL && call_event == CM_CALL_EVENT_RESTORE_CTXT_PSM_REQ)
  {
    ULOG_RT_PRINTF_0(cm_get_ulog_handle(), "=CM= CM_CALL_EVENT_RESTORE_CTXT_PSM_REQ to clients");
  }

  
  /** Notify clients, list of call event.
  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmclient_list_call_event_ntfy( call_event, call_info_ptr );

  /* Deallocate the memory after sending the call event. */
  cm_mem_free(call_info_ptr);


} /* cmcall_event_2() */
/*lint +esym(818, call_ptr) could be declared as pointing to const */

/*===========================================================================
FUNCTION cmcall_unreserve_connection_id

DESCRIPTION
   Unreserve SM/MN connection ids

DEPENDENCIES
  Call object must have already been initialized with cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(715, call_ptr) */
void cmcall_unreserve_connection_id(

    const cmcall_s_type                  *call_ptr
        /* pointer to a call object */
)
{
  /*lint -esym(550,nsapi)*/

  #ifdef CM_GW_SUPPORTED

  uint8                 connection_id;
  /* Conn id for ps call */
  cm_call_id_type       call_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT (call_ptr != NULL);

  call_id = cmcall_call_id_get( call_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get connection ID */
  if (!cm_find_connection_id(call_id,&connection_id))
  {
    CM_ERR_0( "Could not find connection id.");
  }

  if (connection_id == CM_UNKNOWN_CONN_ID)
  {
    /* No connection id allocated to unreserve */
    return;
  }

  CM_MSG_HIGH_2( "Realeasing conn_id:%d of call_id:%d",
                 connection_id, call_id);
  
  (void)cm_unreserve_connection_id( connection_id, call_ptr->asubs_id );
  #endif /* CM_GW_SUPPORTED */
}  /* cmcall_unreserve_connection_id */
/*lint +esym(715, call_ptr) */
/*lint +esym(550,nsapi) */

/*===========================================================================
FUNCTION cmcall_hvolte_switch_srlte

DESCRIPTION
  Hvolte requirement - When call fails on VOLTE and being silent redialed on CDMA, Switch to SRLTE after call ends .    
  1)  redial through CMCALL_ACT_FORCE_NON_IP	
  2)  redial through CMCALL_ACT_HARD_FAILURE	

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_hvolte_switch_srlte(
  boolean                            switch_to_srlte,
sys_modem_as_id_e_type        as_id,
  cm_ss_e_type                       ss,
cm_call_id_type call_id,
cm_call_mode_info_e_type     info_type
 )
{
#if (defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE))    
  CM_MSG_HIGH_2( "curr soa state %d %d ",
                 cmsoa_get_current_sv_oprt_state(),
                 switch_to_srlte);

  /* switch to srlte immediately for below case as well
  **  IMS reg failed during volte call .
  **  But SRLTE switch was postponed due to call object.
  **  Call ended , now switch to srlte immediately ,
  **  This helps UE acquire 1x (voice service) quickly without waiting 4 secs .
  */     
  if(switch_to_srlte == TRUE
     || 
     (cmsoa_is_oprting_in_csfb_pref_srlte(as_id) &&
     cmss_is_hybr2_operational() == FALSE &&
     info_type == CM_CALL_MODE_INFO_IP)
 )
{
    cm_orig_q_s_type      *q_ptr;
  /* hvolte req:
  ** switch_to_srlte when volte call fails and being redialed on legacy rats
  */     
  if(switch_to_srlte == TRUE)
  {
    cmsoa_sv_switch_base(CMSOA_SV_OPRT_MODE_SVLTE,ss,FALSE);
    } 

    q_ptr = cmtask_orig_para_search_act_id(CM_SS_MAIN,call_id);
    
    if (q_ptr)
    {
      q_ptr->orig->orig_mode_uptime = time_get_uptime_secs();
    }
    cmph_check_and_unforce_orig_mode(CM_SS_MAIN);
  } 
#endif
}

#ifdef FEATURE_MMODE_DUAL_SIM
/*===========================================================================
FUNCTION cmcall_unforce_call_end_standby_chgd

DESCRIPTION
  unforce orig end before processing standby changed

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_unforce_call_end_standby_chgd(cm_ss_e_type cm_ss)
{
  cm_orig_q_s_type      *q_ptr;
  cm_orig_q_s_type      *q_next_ptr;
  dword                  uptime         = time_get_uptime_secs();
  boolean                is_unforce_req = FALSE;

  q_ptr = cmtask_orig_para_get_first(cm_ss);

  while(q_ptr != NULL)
  {
    q_next_ptr = cmtask_orig_para_get_next(q_ptr, cm_ss);
    q_ptr->orig->orig_mode_uptime = uptime;
    is_unforce_req = TRUE;
    CM_MSG_HIGH_0(" Setting uptime to current time");
    q_ptr = q_next_ptr;
  }

  if(is_unforce_req)
  {
    cmph_check_and_unforce_orig_mode(cm_ss);
  }

}
#endif

#if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
/*===========================================================================

FUNCTION cmcall_end

DESCRIPTION
  Does call end necessary activities.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_end(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{
  cm_act_update_reas_e_type update_reason = CM_ACT_UPDATE_REAS_ACT_END;
  sys_modem_as_id_e_type asubs_id = SYS_MODEM_AS_ID_1;
  cm_call_mode_info_e_type     info_type = call_ptr->cmcall_mode_info.info_type;
  cm_ss_e_type cm_ss = call_ptr->ss;
  cm_ss_e_type unforce_ss = CM_SS_MAIN;
  cm_call_id_type call_id = call_ptr->call_id;
  cmregprx_sim_state_update_s_type *cmregprx_sim_state_update_ptr = cmregprx_get_sim_state_update_info();
  #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)  
  boolean switch_to_srlte = call_ptr->switch_to_srlte;
  #endif
  #ifdef FEATURE_MMODE_DUAL_SIM
  cmph_s_type * ph_ptr = cmph_ptr();
  #endif
#ifndef FEATURE_CM_DISABLE_GPS
  #ifdef FEATURE_GPSONE
  cmpd_session_s_type  *pd_ptr      = cmpd_ptr(CMPD_SESSION_OBJ_MAIN);
  cmpd_session_s_type  *pd_dmod_ptr = cmpd_ptr(CMPD_SESSION_OBJ_DMOD);
  CM_ASSERT( pd_ptr      != NULL );
  CM_ASSERT( pd_dmod_ptr != NULL );
  #endif
#endif
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  #ifdef FEATURE_MMODE_DUAL_SIM
  if (cmph_is_msim())
  {
    asubs_id = call_ptr->asubs_id;
  }
  #endif

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Call is ending due to acquisition failure,
  ** call was waiting for sim cc till now,
  ** so calling callback here with no srv error*/
  if(call_ptr->cc_object_ptr != NULL)
  {
    cm_cmd_type              *cmd_ptr;
    cmd_ptr = (cm_cmd_type *) call_ptr->cc_object_ptr->user_data;

    /* freeing call cmd here, if ending from call control state*/
    if(cmd_ptr != NULL)
    {
      cmcall_client_cmd_err( CALL_CMD_PTR(cmd_ptr), CM_CALL_CMD_ERR_SRV_STATE_S );
      CM_MSG_HIGH_0("cmcall_end, CM_CALL_CMD_ERR_SRV_STATE_S");
      cm_cmd_dealloc( cmd_ptr );
    }
  }


  /* Need to move concurrent VOLTE call up to VOICE priority
  ** in case if it was moved below PH during e911 orig
  */
  #if defined( FEATURE_IP_CALL ) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
  if(cmph_ptr()->ph_state != CM_PH_STATE_EMERG_CB && call_ptr->call_type == CM_CALL_TYPE_EMERGENCY )
  {
    cmph_change_volte_call_priority(TRUE, call_ptr->asubs_id);
  }
  #endif

  #ifdef CM_GW_SUPPORTED

  /* Unreserve connection ids associated with this call id */
  cmcall_unreserve_connection_id (call_ptr);

  #endif  /* FEATURE_GSM || FEATURE_WCDMA */

  /* Send emergency mode end status on emergency call end
  ** if UE is currently not in emergency callback mode
  */
  if(!cmph_is_in_emergency_cb() && call_ptr->call_type == CM_CALL_TYPE_EMERGENCY )
  {
    cmph_update_send_emerg_status_ind(CM_EMERGENCY_MODE_ENDED, call_ptr->asubs_id);
  }

  #ifdef FEATURE_MMODE_DUAL_SIM
  /* if all voice calls have been ended, then change
     curr voice subs to default_voice_subs*/

  if(( call_ptr->call_type == CM_CALL_TYPE_VOICE ||
      call_ptr->call_type == CM_CALL_TYPE_EMERGENCY )
      && cmph_is_msim()
      && ph_ptr->curr_voice_subs != ph_ptr->default_voice_subs)
  {
    if(cmcall_is_there_a_call_type(cm_ss,
         CM_CALL_TYPE_VOICE, call_ptr->call_id)== CM_CALL_ID_INVALID
       &&cmcall_is_there_a_call_type(cm_ss,
         CM_CALL_TYPE_EMERGENCY, call_ptr->call_id)== CM_CALL_ID_INVALID)
    {
      ph_ptr->curr_voice_subs = ph_ptr->default_voice_subs;
      cmph_sp_subs_info_update();
    }
  }
  #endif
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(call_ptr->end_status == CM_CALL_END_EMERGENCY_ONLY
    || call_ptr->end_status == CM_CALL_END_THERMAL_EMERGENCY)
  {
    update_reason = CM_ACT_UPDATE_REAS_EMERG_ENTER;
  }
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_MSG_HIGH_6("cmcall_end() as_id %d, call_id %d, call_type %d info_type %d end_status %d prot_end_status %d",call_ptr->asubs_id, call_ptr->call_id,\
   call_ptr->call_type, call_ptr->cmcall_mode_info.info_type, call_ptr->end_status, call_ptr->prot_end_status);
  

  if(call_ptr->prot_end_status != CM_CALL_END_NONE)
  {
      call_ptr->end_status = call_ptr->prot_end_status ;
      call_ptr->prot_end_status = CM_CALL_END_NONE;
  }

  switch ( call_ptr->cmcall_mode_info.info_type )
  {
    #if defined(CM_GW_SUPPORTED)
    case CM_CALL_MODE_INFO_GW_CS:
    case CM_CALL_MODE_INFO_GW_PS:
      cmwcall_end(call_ptr);
     break;
    #endif /* defined (FEATURE_GSM) || defined (FEATURE_WCDMA) */

    #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
    case CM_CALL_MODE_INFO_CDMA:
      cmxcall_end(call_ptr);
      break;
    #endif /* FEATURE_MMODE_CDMA_800 || FEATURE_MMODE_CDMA_1900 */


    case CM_CALL_MODE_INFO_IP:

      #if defined(FEATURE_IP_CALL) && !defined (FEATURE_MMODE_DISABLE_LTE_VOICE)
      /* WLAN VoIP calls are made using CM IP interface
      */
      /* delete the reference  */
      REF_CNT_OBJ_RELEASE_IF(call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.ip_gen_param);
      REF_CNT_OBJ_RELEASE_IF(call_ptr->cmcall_mode_info.info.ip_info.ipcall_attrib_info.history_info);
      REF_CNT_OBJ_RELEASE_IF(call_ptr->cmcall_mode_info.info.ip_info.caller_info);

      cmipcall_end (call_ptr);
      #endif
      break;

    #ifdef FEATURE_CM_LTE
    case CM_CALL_MODE_INFO_LTE:
      CM_MSG_HIGH_0("cmcall_end, info type LTE, no action");

      break;
    #endif


    case CM_CALL_MODE_INFO_NONE:
      /* In this case, no actual call was made.
      ** We were in the origination process and had to end the call
      ** before mode was determined.
      ** Since this is the case, just deallocate callobject and clean
      ** up the common, non-mode specific stuff.
      */

      /* Change the call state to idle.
      */
      call_ptr->call_state = CM_CALL_STATE_IDLE;
      call_ptr->call_subst.idle= CM_CALL_IDLE_SUBST_NONE;

      /* Update the queues for this activity
      ** search the Queues and if the activity is found, update its uptime a
      ** and the update reason
      */

      (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                     CM_SS_MAIN,
                                     update_reason );

      #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE
      (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                     CM_SS_HYBR_2,
                                     update_reason );
       #endif

      #ifdef FEATURE_MMODE_HYBR3_ENABLED
      (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                     CM_SS_HYBR_3,
                                     update_reason );
      #endif

      (void) cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                     CM_SS_HDR,
                                     update_reason );



      /* Tell phone that call is over if applicable.
      */
      cmph_call_end_process(call_ptr);

      /* Notify clients of call end event.
      */
      cmcall_update_call_end_info( call_ptr );

      /* Donot send end event, when call waiting for reorig phase2*/
      if(call_ptr->call_type == CM_CALL_TYPE_PS_DATA || call_ptr->cc_object_ptr == NULL)
      {
        CM_MSG_HIGH_1("Sending call end evt , call_type=%d",call_ptr->call_type);
        cmcall_event( call_ptr, CM_CALL_EVENT_END );
      }

      #if defined (FEATURE_LTE_TO_1X)
      if (call_ptr->csfb_type != CM_CALL_CSFB_TYPE_1XCSFB)
      {
        /* update DDTM status */
        cmph_update_ddtm_status(cmph_ptr());
      }
      #else
      cmph_update_ddtm_status(cmph_ptr());
      #endif


      cm_call_id_deallocate( cmcall_call_id_get( call_ptr ) );
      break;

    default:
      sys_err_fatal_invalid_value_exception((uint8) call_ptr->cmcall_mode_info.info_type);
  } /* switch ( call_ptr->cmcall_mode_info.info_type ) */

  /* If SIM_STATE_UPDATE_IND report is buffered in cmregprx , process the report now.
  */
  
  if((cmcall_is_there_a_call_type_per_sub(SYS_MODEM_AS_ID_1,
      CM_CALL_TYPE_VOICE, NULL)== CM_CALL_ID_INVALID) &&
     (cmcall_is_there_a_call_type_per_sub(SYS_MODEM_AS_ID_1,
      CM_CALL_TYPE_EMERGENCY, NULL)== CM_CALL_ID_INVALID)
      && cmregprx_sim_state_update_ptr->update_pending == TRUE)
   {    
      mmoc_cmd_prot_gw_sim_state_update_ind(cmregprx_sim_state_update_ptr->sim_state);
      cmregprx_reset_sim_state_update_info();
   }

  #ifdef FEATURE_MMODE_DUAL_SIM
  if( ph_ptr->is_standby_pref_buffered
    && (ph_ptr->user_standby_pref == SYS_MODEM_DS_PREF_AUTO ||
    ph_ptr->user_standby_pref == SYS_MODEM_DS_PREF_AUTO_NO_TUNEAWAY)
    )
  {
    if(cmcall_is_there_a_call_type(CM_SS_MAX,
         CM_CALL_TYPE_VOICE, call_ptr->call_id)== CM_CALL_ID_INVALID
       &&cmcall_is_there_a_call_type(CM_SS_MAX,
         CM_CALL_TYPE_EMERGENCY, call_ptr->call_id)== CM_CALL_ID_INVALID
       #if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
       && ph_ptr->ph_state != CM_PH_STATE_EMERG_CB
       #endif
         #if (defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE)
         && !(mm_per_stacks_is_mt_page_in_progress(SYS_MODEM_AS_ID_1, SYS_MODEM_STACK_ID_1) ||
         mm_per_stacks_is_mt_page_in_progress(SYS_MODEM_AS_ID_2, SYS_MODEM_STACK_ID_1)
         #ifdef FEATURE_MMODE_TRIPLE_SIM
         || mm_per_stacks_is_mt_page_in_progress(SYS_MODEM_AS_ID_3, SYS_MODEM_STACK_ID_1)
         #endif
         )
         #endif
       )

    {
      CM_MSG_HIGH_1("Unforce orig pref on sub %d, process standby_chgd", asubs_id);
      unforce_ss = cmph_map_subs_to_ss(asubs_id);
      if(unforce_ss == CM_SS_MAIN)
      {
        cmcall_unforce_call_end_standby_chgd(CM_SS_MAIN);
        cmcall_unforce_call_end_standby_chgd(CM_SS_HYBR_1);
        if(cmph_is_subs_feature_mode_1x_sxlte(asubs_id) ||
          cmph_is_subs_feature_mode_sglte(asubs_id))
        {
          cmcall_unforce_call_end_standby_chgd(CM_SS_HYBR_2);
        }
      }
      else
      {
        cmcall_unforce_call_end_standby_chgd(unforce_ss);
      }
      cm_ph_cmd_dual_standby_pref( NULL,
                                   NULL,
                                   CM_CLIENT_ID_ANONYMOUS,
                                   ph_ptr->user_standby_pref,
                                   SYS_MODEM_AS_ID_NO_CHANGE,
                                   SYS_MODEM_AS_ID_NO_CHANGE,
                                   SYS_MODEM_AS_ID_NO_CHANGE,
                                   SYS_MODEM_AS_ID_NO_CHANGE,
                                   DDS_CAUSE_PERMANENT
                                   );

    }
  }
  #endif
  
  #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
  cmcall_hvolte_switch_srlte(switch_to_srlte,asubs_id,
                             cm_ss,call_id,info_type);
  #endif
  
#ifndef FEATURE_CM_DISABLE_GPS
  #ifdef FEATURE_GPSONE
  /* Check if we need to force the mode for the PD session
  */
  if( !pd_ptr->is_mode_forced                                  &&
       ( pd_ptr->session_state == CMPD_SESSION_STATE_INITIATED ||
         pd_ptr->session_state == CMPD_SESSION_STATE_GRANTED
       ))
  {
    cmpd_session_grant_proc( pd_ptr->session_type );
  }

  if( !pd_dmod_ptr->is_mode_forced                                  &&
       ( pd_dmod_ptr->session_state == CMPD_SESSION_STATE_INITIATED ||
         pd_dmod_ptr->session_state == CMPD_SESSION_STATE_GRANTED
       ))
  {
    cmpd_session_grant_proc( pd_dmod_ptr->session_type );
  }
  #endif
#endif
  if (cmph_is_subs_feature_mode_1x_sxlte(asubs_id))
  {
    CM_MSG_HIGH_0("CM -> PM:   enforce policy upon callend");
    cmpmprx_notify_call_end(FALSE);
  }

} /* cmcall_end() */
#endif



/*===========================================================================

FUNCTION cmcall_client_cmd_err_deallocate

DESCRIPTION
  Wrapper around cmcall_client_cmd_err () that deallocates call id and .
  unreserves connection id. Added to free call id and connection id
  when CM_CALL_CMD_ORIG fails

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  call id and connection id get released.

===========================================================================*/
static void cmcall_client_cmd_err_deallocate(

    cm_call_cmd_s_type  *call_cmd_ptr,
        /* Pointer to a CM command */

    cm_call_cmd_err_e_type    cmd_err
        /* Indicate the call command error */
)
{
  cm_call_id_type call_id  = CM_CALL_ID_INVALID;
  cmcall_s_type   *call_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  call_id = call_cmd_ptr->info.call_id;

  if(call_cmd_ptr->cmd_type == CM_CMD_TYPE_CALL)
  {
    REF_CNT_OBJ_RELEASE_IF(call_cmd_ptr->info.ip_call.ipcall_attrib_info.ip_gen_param);
  }

  if( call_cmd_ptr->cmd == CM_CALL_CMD_CC_COMPLETE_REORIG ||
    call_cmd_ptr->cmd == CM_CALL_CMD_CC_COMPLETE_REORIG_PHASE2 )
  {
    /* Reset the cmd to be the original origination command.
      ** CM should not send out the cmd as a REORIG or REORIG_PHASE2.
      */
    call_cmd_ptr->cmd = CM_CALL_CMD_ORIG;
  }

  if( (call_ptr = cmcall_ptr (call_id)) != NULL )
  {
    boolean is_emerg = (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY);

    cmph_delete_and_unforce_activity(call_ptr->ss, call_ptr->call_id);

    cmcall_unreserve_connection_id (call_ptr);
    if(call_ptr->cc_object_ptr != NULL)
    {
      CM_MSG_HIGH_0("Freeing cc_object_ptr in cmcall_client_cmd_err_deallocate");
      cmcc_object_deallocate( call_ptr->cc_object_ptr );
    }
    cm_call_id_deallocate (call_id);

    /* If no other emergency call, notify cmpmprx to resume pm */
    if(is_emerg &&
      cmcall_is_there_a_call_type(CM_SS_MAX, CM_CALL_TYPE_EMERGENCY,
                                 CM_CALL_ID_INVALID) == CM_CALL_ID_INVALID)
    {
      cmpmprx_notify_call_end(TRUE);
    }
  }

  cmcall_client_cmd_err (call_cmd_ptr, cmd_err);

} /* cmcall_client_cmd_err_deallocate () */


/*===========================================================================

FUNCTION cmcall_client_cmd_err

DESCRIPTION
  Notify clients of a specified call command error.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_client_cmd_err(

    const cm_call_cmd_s_type       *call_cmd_ptr,
        /* Pointer to a CM command */

    cm_call_cmd_err_e_type    cmd_err
        /* Indicate the call command error */
)
{
  cm_call_cmd_err_cause_e_type     cmd_err_cause;
  cm_alpha_s_type             alpha;
  cmcall_s_type               *call_ptr = NULL;
  int                         end_call_loop     = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_cmd_ptr != NULL );
  CM_ASSERT( call_cmd_ptr->cmd_type == CM_CMD_TYPE_CALL );
  CM_ASSERT( BETWEEN( cmd_err, CM_CALL_CMD_ERR_NONE, CM_CALL_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at call command component */
  CM_MSG_LOW_3("START cmcall_client_cmd_err() call_cmd=%d, cmd_err=%d, client=%d",
                 call_cmd_ptr->cmd, cmd_err, call_cmd_ptr->client_id );

  cmd_err_cause = call_cmd_ptr->info.error_cause;
  alpha.len = 0;

  if( call_cmd_ptr->is_modem_cc_api == TRUE )
  {
    /* With feature defined, for call cmd Orig, fill in alpha and
    ** error cause and invoke new callback
    */
    if((call_ptr = cmcall_ptr (call_cmd_ptr->info.call_id)) != NULL )
    {
      #ifdef FEATURE_MMODE_3GPP
      if (call_ptr->gw_cc_modified_info.error_cause ==
                       CM_CALL_CMD_ERR_CAUSE_CARD_CC_FAIL
          ||
          call_ptr->gw_cc_modified_info.error_cause ==
                           CM_CALL_CMD_ERR_CAUSE_NO_ERROR)
      {
        alpha = call_ptr->gw_cc_modified_info.alpha;
      }
      #endif
    }

    if (call_cmd_ptr->cmd_cc_cb_func != NULL)
    {
      call_cmd_ptr->cmd_cc_cb_func( call_cmd_ptr->data_block_ptr,
                                    call_cmd_ptr->cmd,
                                    cmd_err ,
                                    cmd_err_cause,
                                    alpha);
    }
    /* Legacy call cmd orig is modified to use new centralized call control.
       Old call control call back needs to be called if its TRUE.
       This ensures clients who use legacy call control api will
       expect the same behavior as before.*/
    if( call_cmd_ptr->cmd_cb_func != NULL )
    {
      call_cmd_ptr->cmd_cb_func( call_cmd_ptr->data_block_ptr,
                                call_cmd_ptr->cmd,
                                cmd_err );
    }

    else if( call_cmd_ptr->cmd_call_id_cb_func != NULL )
    {
      for (end_call_loop=0; end_call_loop < call_cmd_ptr->info.num_end_call_ids;
        ++end_call_loop) 
      {
        call_cmd_ptr->cmd_call_id_cb_func( call_cmd_ptr->data_block_ptr,
                                           call_cmd_ptr->cmd,
                                           cmd_err,
                                           call_cmd_ptr->info.end_params[end_call_loop].call_id );
      }
    }
  }
  else if( call_cmd_ptr->cmd_cb_func != NULL )
  {
     call_cmd_ptr->cmd_cb_func( call_cmd_ptr->data_block_ptr,
                                call_cmd_ptr->cmd,
                                cmd_err );
  }
  else if( call_cmd_ptr->cmd_call_id_cb_func != NULL )
  {
    for (end_call_loop=0; end_call_loop < call_cmd_ptr->info.num_end_call_ids;
       ++end_call_loop) 
    {
      call_cmd_ptr->cmd_call_id_cb_func( call_cmd_ptr->data_block_ptr,
                                         call_cmd_ptr->cmd,
                                         cmd_err,
                                         call_cmd_ptr->info.end_params[end_call_loop].call_id );       
    }
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If error status is other than NO ERROR,
  ** notify the client list of command error, as well.
  */
  if( cmd_err != CM_CALL_CMD_ERR_NOERR )
  {
    CM_MSG_HIGH_3( "call cmd err, cmd=%d, err=%d, client=%ld",
            call_cmd_ptr->cmd, cmd_err, call_cmd_ptr->client_id );

    /* Notify clients about the error */
    cmclient_list_call_cmd_err_ntfy( cmd_err, call_cmd_ptr );
  }

} /* cmcall_client_cmd_err() */


#if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
/*===========================================================================

FUNCTION  cmcall_call_cmd_answer_para_check

DESCRIPTION
  Check parameter errors for call answer command

DEPENDENCIES
  none
RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
cm_call_cmd_err_e_type  cmcall_call_cmd_answer_para_check
(
  const cm_call_cmd_s_type       *call_cmd_ptr
)
{
  #ifdef CM_GW_SUPPORTED
  cmss_s_type   *ss_ptr = cmss_ptr();
     /* Serving system object */
  #endif

  cm_call_cmd_err_e_type   cmd_err       = CM_CALL_CMD_ERR_NOERR;
  cmcall_s_type            *call_ptr     = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(call_cmd_ptr != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_ptr = cmcall_ptr( call_cmd_ptr->info.call_id );

  /* Check integrity of command parameters */

  if( call_ptr == NULL )
  {
    cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
    CM_ERR_0( "CM_CALL_CMD_ERR_CALL_ID_P");
  }
  else if( !BETWEEN(call_cmd_ptr->info.call_type, CM_CALL_TYPE_NONE, CM_CALL_TYPE_MAX) )
  {
    cmd_err  = CM_CALL_CMD_ERR_CALL_TYPE_P;
    CM_ERR_0( "CM_CALL_CMD_ERR_CALL_TYPE_P");
  }
  else if (call_ptr->call_type != call_cmd_ptr->info.call_type &&
          call_ptr->cmcall_mode_info.info_type != CM_CALL_MODE_INFO_IP)
  {
    cmd_err = CM_CALL_CMD_ERR_CALL_TYPE_P;
    CM_MSG_HIGH_0 ("call type mismatch between answering client and CM \n");
  }

  return cmd_err;

} /* cmcall_call_cmd_answer_para_check() */
#endif


/*===========================================================================

FUNCTION  cmcall_call_cmd_get_call_info_para_check

DESCRIPTION
  Check parameter errors for get call info command

DEPENDENCIES
  none

RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
cm_call_cmd_err_e_type  cmcall_call_cmd_get_call_info_para_check
(
  const cm_call_cmd_s_type       *call_cmd_ptr
)
{
  cm_call_cmd_err_e_type   cmd_err       = CM_CALL_CMD_ERR_NOERR;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_cmd_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check integrity of command parameters */

  if( cmcall_ptr( call_cmd_ptr->info.call_id ) == NULL )
  {
    cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
  }
  else if( call_cmd_ptr->client_ptr == CM_CLIENT_PTR_NONE )
  {
    cmd_err  = CM_CALL_CMD_ERR_CLIENT_ID_P;
  }
  else if( (call_cmd_ptr->client_ptr)->init_mask != CM_INIT_MASK )
  {
    cmd_err  = CM_CALL_CMD_ERR_CLIENT_ID_P;
  }

  CM_MSG_HIGH_1( "call_cmd_err:%d",cmd_err);

  return cmd_err;
} /* cmcall_call_cmd_get_call_info_para_check() */


#if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
/*===========================================================================

FUNCTION cmcall_get_favored_roam_pref

DESCRIPTION
  Return the favored roam preference to be used for a specified call-type,
  service-state, roam-status and a roam-preference.

DEPENDENCIES
  None.

RETURN VALUE
  The favored roam preference to be used with a specified call-type,
  service-state, roam-status and a roam-preference.

SIDE EFFECTS
  None.

===========================================================================*/
cm_roam_pref_e_type  cmcall_get_favored_roam_pref(

    cm_call_type_e_type    call_type,
        /* Call which we try to originate */

    sys_sys_mode_e_type    srv_mode,
        /* The current service mode */

    sys_roam_status_e_type roam_status,
        /* The current roaming status */

    cm_roam_pref_e_type    roam_pref
        /* The current roaming preference */

)
{

  #if defined(FEATURE_MMODE_CDMA)
  /* If this is an emergency call, relax the roaming preference.
  */
  if( call_type == CM_CALL_TYPE_EMERGENCY )
  {
    return CM_ROAM_PREF_ANY;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we don't have service at the moment, use the existing roaming
  ** preference.
  */
  if( srv_mode == SYS_SYS_MODE_NO_SRV )
  {
    return roam_pref;
  }

  /*-------------------------------------------------------------------------
      If we got here, this is not an emergency call and we have service.
  -------------------------------------------------------------------------*/

  /* To ensures that the phone will not suddenly switch to a less preferred
  ** roaming while doing silent redials, limit the roaming preference in
  ** accordance with the current roaming status and roaming preference, as
  ** necessary.
  */
  switch( roam_pref )
  {
    case CM_ROAM_PREF_HOME:
      break;

    case CM_ROAM_PREF_ROAM_ONLY:
      break;

    case CM_ROAM_PREF_AFFIL:
      if( roam_status == SYS_ROAM_STATUS_OFF )
      {
        return CM_ROAM_PREF_HOME;
      }
      break;

    case CM_ROAM_PREF_ANY:
      if( roam_status == SYS_ROAM_STATUS_OFF )
      {
        return CM_ROAM_PREF_HOME;
      }
      if( roam_status == SYS_ROAM_STATUS_ON )
      {
        return CM_ROAM_PREF_AFFIL;
      }
      break;

    case CM_ROAM_PREF_NONE:
    case CM_ROAM_PREF_NO_CHANGE:
    case CM_ROAM_PREF_MAX:
    default:
      break;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, it is not necessary to limit the roaming preference -
  ** return the existing roaming preference.
  */
  return roam_pref;
  #else
  return roam_pref;
  #endif

} /* cm_call_get_favored_roam_pref() */



/*===========================================================================

FUNCTION cmcall_get_favored_prl_pref

DESCRIPTION
  Return the favored prl preference to be used for a specified call-type.


DEPENDENCIES
  None.

RETURN VALUE
  The favored prl preference to be used with a specified call-type.


SIDE EFFECTS
  None.

===========================================================================*/
cm_prl_pref_e_type         cmcall_get_favored_prl_pref(

    cm_call_type_e_type    call_type
        /* Call which we try to originate */

)
{
  /* If this is an emergency call, relax the prl preference.
  */
  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  if( call_type == CM_CALL_TYPE_EMERGENCY )
  {
    return CM_PRL_PREF_ANY;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else use the current prl preference setting.
  */
  return CM_PRL_PREF_NO_CHANGE;

} /* cm_call_get_favored_prl_pref() */



/*===========================================================================

FUNCTION cmcall_get_favored_band_pref

DESCRIPTION
  Return the favored band preference to be use with a specified
  call type.

DEPENDENCIES
  none

RETURN VALUE
  The favored band preference to be use with a specified call type.

SIDE EFFECTS
  none

===========================================================================*/
cm_band_pref_e_type  cmcall_get_favored_band_pref(

    cm_call_type_e_type    call_type,
        /* Call type */

    cm_srv_type_e_type     srv_type,
        /* Service type */

    cm_band_pref_e_type    band_pref
        /* The current band preference */
)
{
  /* For standard-OTASP or emergency call types relax the band preference to
  ** ensure that service can be acquired.
  */
  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  if( call_type == CM_CALL_TYPE_STD_OTASP ||
      call_type == CM_CALL_TYPE_EMERGENCY )
  {
    return CM_BAND_PREF_ANY;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, it is ok to use the existing band preference.
  */
  return band_pref;

} /* cmcall_get_favored_band_pref() */


/*===========================================================================

FUNCTION cmcall_get_favored_lte_band_pref

DESCRIPTION
  Return the favored band preference to be use with a specified
  call type.

DEPENDENCIES
  none

RETURN VALUE
  The favored band preference to be use with a specified call type.

SIDE EFFECTS
  none

===========================================================================*/
sys_lte_band_mask_e_type        cmcall_get_favored_lte_band_pref(

    cm_call_type_e_type    call_type,
        /* Call type */

    sys_lte_band_mask_e_type    lte_band_pref
        /* The current LTE band preference */
)
{
  /* For standard-OTASP or emergency call types relax the band preference to
  ** ensure that service can be acquired.
  */
  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  if( call_type == CM_CALL_TYPE_STD_OTASP ||
      call_type == CM_CALL_TYPE_EMERGENCY )
  {
    return SYS_LTE_BAND_MASK_CONST_ANY;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, it is ok to use the existing band preference.
  */
  return lte_band_pref;

} /* cmcall_get_favored_lte_band_pref() */


/*===========================================================================

FUNCTION cmcall_get_favored_tds_band_pref

DESCRIPTION
  Return the favored band preference to be use with a specified
  call type.

DEPENDENCIES
  none

RETURN VALUE
  The favored band preference to be use with a specified call type.

SIDE EFFECTS
  none

===========================================================================*/
cm_band_pref_e_type  cmcall_get_favored_tds_band_pref(

    cm_call_type_e_type    call_type,
        /* Call type */

    cm_band_pref_e_type    tds_band_pref
        /* The current LTE band preference */
)
{
  /* For standard-OTASP or emergency call types relax the band preference to
  ** ensure that service can be acquired.
  */
  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  if( call_type == CM_CALL_TYPE_STD_OTASP ||
      call_type == CM_CALL_TYPE_EMERGENCY )
  {
    return CM_BAND_PREF_TDS_ANY;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, it is ok to use the existing band preference.
  */
  return tds_band_pref;

} /* cmcall_get_favored_lte_band_pref() */


/*===========================================================================

FUNCTION cmcall_get_favored_srv_domain_pref

DESCRIPTION
  Return the favored service domain preference to be use with a specified
  call type.

DEPENDENCIES
  none

RETURN VALUE
  The favored service domain preference to be use with a specified call type.

SIDE EFFECTS
  none

===========================================================================*/
cm_srv_domain_pref_e_type        cmcall_get_favored_srv_domain_pref(

    const cmcall_s_type            *call_ptr
        /* The pointer to the call object*/
)
{

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Force the service domain preferences in accordance with the
  ** call-type and service domain preferences.
  */
  if( call_ptr->call_type ==  CM_CALL_TYPE_PS_DATA )
  {
    return CM_SRV_DOMAIN_PREF_PS_ONLY;
  }
  #if defined (FEATURE_LTE) && defined(FEATURE_LTE_TO_1X)
  else if  ((call_ptr->csfb_type== CM_CALL_CSFB_TYPE_1XCSFB) &&
     ((call_ptr->call_type ==  CM_CALL_TYPE_VOICE) ||
      (call_ptr->call_type ==  CM_CALL_TYPE_EMERGENCY) ||
      (call_ptr->call_type ==  CM_CALL_TYPE_PD)))
  {
    return CM_SRV_DOMAIN_PREF_PS_ONLY;
  }
  #endif
  #ifdef FEATURE_IP_CALL
  else if(call_ptr->srv_type == CM_SRV_TYPE_PS_ONLY)
  {
    CM_MSG_HIGH_0("determine srv domain for PS only type");
    return CM_SRV_DOMAIN_PREF_PS_ONLY;
  }
  /* LTE only set srv domain pref to ps only 
  ** This is to avoid srv domain pref change from cs+ps for call to ps only to trigger new srv request to 
  ** NAS.
  */
  else if (call_ptr->is_volte && call_ptr->call_type ==  CM_CALL_TYPE_VOICE)
  {
    return CM_SRV_DOMAIN_PREF_PS_ONLY;
  }
  #endif
  else
  {
    return CM_SRV_DOMAIN_PREF_CS_ONLY;
  }
} /* cmcall_get_favored_srv_domain_pref() */
#endif

#if defined(FEATURE_MMODE_GW) && !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
/*===========================================================================

FUNCTION cmcall_cmd_info_gw_voice_init

DESCRIPTION
  Fill default gw voice call origination parameters.

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_cmd_info_gw_voice_init (

     cm_call_cmd_s_type  *call_cmd_ptr
       /* pointer to a CM command */

)
{

  cm_call_cmd_info_gw_cs_s_type   *cs_cmd_mode_info_ptr = NULL;
  cm_call_cmd_info_s_type         *cmd_info_ptr         = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_cmd_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cs_cmd_mode_info_ptr                    = WCALL_CMD_INFO_PTR( call_cmd_ptr );
  cmd_info_ptr                            = CMD_INFO_PTR (call_cmd_ptr);

  /* accept is used only for MT calls */
  cs_cmd_mode_info_ptr->accept                                 = FALSE;

  /* success and complete do not seem to be handled in lower layers */
  cs_cmd_mode_info_ptr->success                                = FALSE;
  cs_cmd_mode_info_ptr->complete                               = FALSE;

  /* Set bc repeat indicator, 9.3.23.1.1, 24.008
  ** present is set to FALSE since only bearer_capability_1
  ** is used
  */
  cs_cmd_mode_info_ptr->bc_repeat_indicator.present            = FALSE;
  cs_cmd_mode_info_ptr->bc_repeat_indicator.repeat_indication  = 0;


  /* Set default bearer capability, 9.3.3.2 */
    cm_util_set_default_voice_bearer_cap_per_sub
                       (&cs_cmd_mode_info_ptr->bearer_capability_1, call_cmd_ptr->cmd_subs);

  cs_cmd_mode_info_ptr->bearer_capability_2.present            = FALSE;

  /* Lower layer repeat indicators */
  cs_cmd_mode_info_ptr->llc_repeat_indicator.present           = FALSE;
  cs_cmd_mode_info_ptr->llc_repeat_indicator.repeat_indication = 0;

  /* Lower layer compatability message
  ** Fields are filled only if UE decides to send lower layer compatibility
  ** messages to the called user
  */
  cs_cmd_mode_info_ptr->low_layer_compatibility_1.present      = FALSE;
  cs_cmd_mode_info_ptr->low_layer_compatibility_2.present      = FALSE;

  /* Higher layer repeat indicators
  ** 9.3.23.2.6, HLC repeat indicator should be the same as
  ** bc repeat indicator if bc repeat indicator IE and high
  ** layer compatibility IE is present
  */
  cs_cmd_mode_info_ptr->hlc_repeat_indicator.present           = FALSE;

  cs_cmd_mode_info_ptr->hlc_repeat_indicator.repeat_indication = 0;

  /* Higher layer compatability message */
  cs_cmd_mode_info_ptr->high_layer_compatibility_1.present    = FALSE;

  cs_cmd_mode_info_ptr->high_layer_compatibility_2.present    = FALSE;

  /* Called and calling party sub address */
  cs_cmd_mode_info_ptr->calling_party_bcd_number.present      = FALSE;
  cs_cmd_mode_info_ptr->calling_party_bcd_number.length       = 0;

  cs_cmd_mode_info_ptr->calling_party_subaddress.present      = FALSE;
  cs_cmd_mode_info_ptr->calling_party_subaddress.length       = 0;

  /* forward cug info present */
  cs_cmd_mode_info_ptr->forward_cug_info.present              = FALSE;

  /* Initialize clir */
  cs_cmd_mode_info_ptr->cm_clir                               =
                                                     CM_CLIR_DEFAULT;
  /* cause for gw cs responses */
  cs_cmd_mode_info_ptr->cc_cause.present                      = FALSE;

  /* Call dep supp service cmd type */
  cmd_info_ptr->sups_cmd_type                                 =
                                                     CM_CALL_SUPS_TYPE_NONE;

  cs_cmd_mode_info_ptr->sups_cmd_params.call_id               =
                                                     CM_CALL_ID_INVALID;

  /* user to user signaling data */
  #ifdef FEATURE_UUS
  cs_cmd_mode_info_ptr->mo_user_data.present                  = FALSE;
  #endif

  cm_num_init (&cs_cmd_mode_info_ptr->sups_cmd_params.sups_number);

} /* cmcall_cmd_info_gw_voice_init () */
#endif

#ifdef FEATURE_MMODE_GW
/*===========================================================================

FUNCTION cmcall_cmd_info_gw_ps_init

DESCRIPTION
  Fill default gw packet call origination parameters

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

static void cmcall_cmd_info_gw_ps_init (

  cm_call_cmd_s_type  *call_cmd_ptr                       /* pointer to a CM command */

)
{

  cm_call_cmd_info_gw_ps_s_type   *ps_cmd_mode_info_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_cmd_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ps_cmd_mode_info_ptr                 = WCALL_PS_CMD_INFO_PTR( call_cmd_ptr );

  /* accept is not handled by lower layers */
  ps_cmd_mode_info_ptr->accept                                = FALSE;

  /* By default ps call is a primary pdp activation */
  ps_cmd_mode_info_ptr->primary_pdp                           = TRUE;


  /*------------------------------------------------------------------------
  request Subscribed QOS : NOTE: All values are set to 0x0 implying
  subscribed value : ref TS24.008 sec 10.5.6.5
  -------------------------------------------------------------------------*/
  ps_cmd_mode_info_ptr->qos.valid                             = TRUE;
  ps_cmd_mode_info_ptr->qos.delay_class                       = 0x0;
  ps_cmd_mode_info_ptr->qos.reliability_class                 = 0x0;
  ps_cmd_mode_info_ptr->qos.peak_throughput                   = 0x0;
  ps_cmd_mode_info_ptr->qos.precedence_class                  = 0x0;
  ps_cmd_mode_info_ptr->qos.mean_throughput                   = 0x0;
  ps_cmd_mode_info_ptr->qos.traffic_class                     = 0x0;
  ps_cmd_mode_info_ptr->qos.deliver_order                     = 0x0;
  ps_cmd_mode_info_ptr->qos.deliver_err_sdu                   = 0x0;
  ps_cmd_mode_info_ptr->qos.max_sdu_size                      = 0x0;
  ps_cmd_mode_info_ptr->qos.max_bit_rare_uplink               = 0x0;
  ps_cmd_mode_info_ptr->qos.max_bit_rare_downlink             = 0x0;
  ps_cmd_mode_info_ptr->qos.residual_ber                      = 0x0;
  ps_cmd_mode_info_ptr->qos.sdu_err_ratio                     = 0x0;
  ps_cmd_mode_info_ptr->qos.tranfer_delay                     = 0x0;
  ps_cmd_mode_info_ptr->qos.traffic_priority                  = 0x0;
  ps_cmd_mode_info_ptr->qos.guarantee_ber_uplink              = 0x0;
  ps_cmd_mode_info_ptr->qos.guarantee_ber_dwnlink             = 0x0;
  ps_cmd_mode_info_ptr->qos.sig_ind                           = FALSE;
  ps_cmd_mode_info_ptr->qos.src_stat_desc                     = 0x0;
  ps_cmd_mode_info_ptr->qos.ext_max_bit_rate_downlink         = 0x0;
  ps_cmd_mode_info_ptr->qos.ext_gtd_bit_rate_downlink         = 0x0;
  ps_cmd_mode_info_ptr->qos.ext_max_bit_rate_uplink           = 0x0;
  ps_cmd_mode_info_ptr->qos.ext_gtd_bit_rate_uplink           = 0x0;
  ps_cmd_mode_info_ptr->qos.ext2_gtd_bit_rate_uplink           = 0x0;
  ps_cmd_mode_info_ptr->qos.ext2_max_bit_rate_uplink           = 0x0;
  ps_cmd_mode_info_ptr->qos.ext2_gtd_bit_rate_downlink         = 0x0;
  ps_cmd_mode_info_ptr->qos.ext2_max_bit_rate_downlink         = 0x0;

  /*------------------------------------------------------------------------
  Minimum Subscribed QOS : NOTE: All values are set to 0x0 implying
  subscribed value : ref TS24.008 sec 10.5.6.5
  -------------------------------------------------------------------------*/
  ps_cmd_mode_info_ptr->minimum_qos.valid                     = TRUE;
  ps_cmd_mode_info_ptr->minimum_qos.delay_class               = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.reliability_class         = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.peak_throughput           = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.precedence_class          = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.mean_throughput           = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.traffic_class             = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.deliver_order             = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.deliver_err_sdu           = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.max_sdu_size              = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.max_bit_rare_uplink       = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.max_bit_rare_downlink     = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.residual_ber              = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.sdu_err_ratio             = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.tranfer_delay             = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.traffic_priority          = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.guarantee_ber_uplink      = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.guarantee_ber_dwnlink     = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.sig_ind                   = FALSE;
  ps_cmd_mode_info_ptr->minimum_qos.src_stat_desc             = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.ext_max_bit_rate_downlink = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.ext_gtd_bit_rate_downlink = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.ext_max_bit_rate_uplink   = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.ext_gtd_bit_rate_uplink   = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.ext2_gtd_bit_rate_uplink   = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.ext2_max_bit_rate_uplink   = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.ext2_gtd_bit_rate_downlink = 0x0;
  ps_cmd_mode_info_ptr->minimum_qos.ext2_max_bit_rate_downlink = 0x0;

  /* pdp address TS24.0.0.8, 10.5.6.4*/
  ps_cmd_mode_info_ptr->pdp_addr.valid                        = TRUE;

  /* IETF allocated address */
  ps_cmd_mode_info_ptr->pdp_addr.pdp_type_org                 = 1;

  /* PDP type number value IPv4 address*/
  ps_cmd_mode_info_ptr->pdp_addr.pdp_type_num                 = 0x21;
  ps_cmd_mode_info_ptr->pdp_addr.pdp_addr_len                 = 0;

  /* APN */
  ps_cmd_mode_info_ptr->apn_name.valid                        = FALSE;
  ps_cmd_mode_info_ptr->apn_name.apn_addr_len                 = 0;
  ps_cmd_mode_info_ptr->apn_name.address[0]                   = 0;

  /* config options */
  ps_cmd_mode_info_ptr->config_options.valid                  = FALSE;

  ps_cmd_mode_info_ptr->h_comp                     = SYS_PDP_HEADER_COMP_OFF;
  ps_cmd_mode_info_ptr->d_comp                     = SYS_PDP_DATA_COMP_OFF;

  /* Origination tft parameters */
  ps_cmd_mode_info_ptr->tft.valid                             = FALSE;
  ps_cmd_mode_info_ptr->tft.tft_total_length                  = 0;
  ps_cmd_mode_info_ptr->tft.operation_code                    = 0;
  ps_cmd_mode_info_ptr->tft.num_filters                       = 0;
  ps_cmd_mode_info_ptr->tft.filter_list[0].identifier         = 0;
  ps_cmd_mode_info_ptr->tft.filter_list[0].eval_precedence    = 0;
  ps_cmd_mode_info_ptr->tft.filter_list[0].content_length     = 0;
  ps_cmd_mode_info_ptr->tft.filter_list[0].filter_contents[0] = 0;

  /* Tear down indicator is valid only when deactivating pdp contexts */
  ps_cmd_mode_info_ptr->tear_down_ind.valid                   = FALSE;

  /* rab_id and cause are not used for call origination */
  /* ps_cmd_mode_info_ptr->cause                              = 0; */

  ps_cmd_mode_info_ptr->rabid                                 = 0;
  /* primary_call_id has been replaced with pdp_group_id for
  ** secondary pdp support. Still maintaing for backward
  ** compatability
  */

  ps_cmd_mode_info_ptr->pdp_group_id                          = 0;

  ps_cmd_mode_info_ptr->assoc_pri_profile_num                 = 0;

  ps_cmd_mode_info_ptr->profile_number                        = 0;

  ps_cmd_mode_info_ptr->low_priority_signaling                = FALSE;
  
} /* cmcall_cmd_info_gw_ps_init () */


#endif /* (defined (FEATURE_GSM) || defined (FEATURE_WCDMA)) */


#if( defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))


/*===========================================================================

FUNCTION cmcall_cmd_info_cdma_init

DESCRIPTION
   Function to initialize default cdma voice call origination parameters.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_cmd_info_cdma_init (

  cm_call_cmd_s_type  *call_cmd_ptr             /* pointer to a CM command */

)
{

  cm_call_cmd_info_cdma_s_type    *cdma_info_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( call_cmd_ptr != NULL );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cdma_info_ptr                     = CALL_CMD_MODE_INFO_PTR (call_cmd_ptr);

  cdma_info_ptr->srv_opt            = CM_SRV_OPT_NONE;
  cdma_info_ptr->is_special_srv_data_call = FALSE;
  cdma_info_ptr->otasp_act_code     = CM_OTASP_ACT_CODE_NONE;

  #ifdef FEATURE_IS2000
  cdma_info_ptr->drs_bit            = TRUE;
  /* set the DRS bit or not */
  #endif /* FEATURE_IS2000 */

  cdma_info_ptr->privacy_pref       = CM_PRIVACY_PREF_NONE;

  #ifdef FEATURE_HDR_HANDOFF
  cdma_info_ptr->last_act_data_net  = SYS_SYS_MODE_NO_SRV;
  /* last active data network */
  #endif /* FEATURE_HDR_HANDOFF */

  #ifdef FEATURE_IS2000_REL_A
  cdma_info_ptr->sr_id_included     = FALSE;
  /* sr_id indicator */

  cdma_info_ptr->qos_parms_incl     = FALSE;
  /* Qos Parameters indicator */

  cdma_info_ptr->qos_parms_len      = 0;
  /* Length of QOS parameters */

  #endif /* FEATURE_IS2000_REL_A */

  cdma_info_ptr->flash_type         = CM_CALL_SUPS_TYPE_NONE;
  /* type of flash */

  #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
  #error code not present
#endif

} /* cmcall_cmd_info_cdma_init () */

#endif // #if( defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

#ifdef FEATURE_HDR
/*===========================================================================

FUNCTION cmcall_is_hybr_data_switchover_call

DESCRIPTION
 Check if the ps call is a hybrid data switchover call.

DEPENDENCIES
 None

RETURN VALUE
 TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
static boolean  cmcall_is_hybr_data_switch_over_call(

  const cmcall_s_type               *call_ptr,
      /* Pointer to a phone object */
  cm_mode_pref_e_type         favored_mode_pref
  /* Favored mode preference for this call if it is the only call being originated */
)
{

  cmph_s_type               *ph_ptr = cmph_ptr();
      /* Pointer to a phone object */

  cmss_s_type               *ss_ptr = cmss_ptr();
      /* Pointer to a serving system object */

  boolean                   is_hybr_data_switch_over;
      /* Indicate if this is a hybrid data switch over call. */


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  

  CM_ASSERT( call_ptr != NULL );

  CM_ASSERT( favored_mode_pref < CM_MODE_PREF_MAX );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If call type is PS data and the call is originated on hdr and
  ** current service is TGW and the mode preference does not contain CDMA and
  ** HDR and the favored mode preference does not contain TGW,
  ** set the hybrid data switch over to TRUE.
  */
  if( ( call_ptr->call_type == CM_CALL_TYPE_PS_DATA ) &&

      ( call_ptr->ss == CM_SS_HDR )                   &&

      ( ss_ptr->info.sys_mode == SYS_SYS_MODE_GSM ||
        ss_ptr->info.sys_mode == SYS_SYS_MODE_WCDMA ) &&

      ( ph_ptr->main_stack_info.pref_info.mode_pref  == cmcall_remove_mode_pref_components(
          ph_ptr->main_stack_info.pref_info.mode_pref, 1, SD_SS_MODE_PREF_CDMA_HDR)
                                                      &&
        favored_mode_pref == cmcall_remove_mode_pref_components(
          favored_mode_pref, 1, SD_SS_MODE_PREF_TDS_GSM_WCDMA) ))
  {
     is_hybr_data_switch_over = TRUE;
  }
  else
  {
    is_hybr_data_switch_over = FALSE;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_MSG_HIGH_2( "Hybrid Data switch over= %d, call state = %d",
               is_hybr_data_switch_over,
               call_ptr->call_state);

  return is_hybr_data_switch_over;

}
#endif


#if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))

/*===========================================================================

FUNCTION cmcall_force_pref_on_the_fly

DESCRIPTION
  Force the system selection preferences in accordance with the call-type
  and its favored mode preferences.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

  This function is expected to be called during the origination process
  before call is connected.

RETURN VALUE
  A ballpark estimation for the number of seconds the phone is going
  to be without service.

SIDE EFFECTS
  None.

===========================================================================*/
dword  cmcall_force_pref_on_the_fly(

    cmcall_s_type             *call_ptr,
        /* Pointer to a call object */

    sd_ss_pref_reas_e_type    force_reason
        /* Reason for forcing the SS-preference.
        */
)
{

  cmph_s_type              *ph_ptr            = cmph_ptr();
    /* Point at phone object */

  cm_band_pref_e_type       favored_band_pref;
    /* Favored band preference for this call */

  sys_lte_band_mask_e_type  favored_lte_band_pref;
    /* Favored band preference for this call */

  cm_band_pref_e_type       favored_tds_band_pref;
    /* Favored band preference for this call */

  cm_srv_domain_pref_e_type favored_srv_pref  = CMPH_SS_SRV_DOMAIN_PREF(call_ptr->asubs_id, ph_ptr);
    /* Favored service domain preference for this call */

  cm_mode_pref_e_type       favored_mode_pref = CM_MODE_PREF_NONE;
    /* Favored mode preference for this call if it is the only call being originated */

  dword                     rescan_time = 0;
  sys_modem_as_id_e_type    asubs_id;
  cm_ciot_lte_pref_s_type   ciot_lte_pref;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 
  CM_ASSERT( call_ptr    != NULL );
  CM_ASSERT( force_reason < SD_SS_PREF_REAS_MAX );
  CM_ASSERT( ph_ptr      != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  asubs_id = cmcall_get_asubs_id(call_ptr);
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ciot_lte_pref.lte_op_mode_pref = CMPH_SS_LTE_IOT_OP_MODE( asubs_id, ph_ptr );
  ciot_lte_pref.lte_m1_band_pref = CMPH_SS_LTE_M1_BAND_PREF( asubs_id, ph_ptr );
  ciot_lte_pref.lte_nb1_band_pref = CMPH_SS_LTE_NB1_BAND_PREF( asubs_id, ph_ptr );
  ciot_lte_pref.lte_band_pref    = CMPH_SS_LTE_WB_BAND_PREF(asubs_id,ph_ptr);

  /* Set the ok to orig indication to FALSE.
  */
  call_ptr->sr.is_ok_to_orig      = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is NOT the first time we force the preferences for this
  ** origination attempt, just use the existing call origination preferences.
  */
  {
    cm_orig_q_s_type *computed_orig_para =  cmtask_orig_para_search_act_id  (
                                                         CM_SS_MAIN,
                                         (cm_act_id_type)call_ptr->call_id );
    if( computed_orig_para != NULL &&
        computed_orig_para->orig->orig_mode != SD_SS_ORIG_MODE_NONE )
    {
      /* fetch the current srv_pref in use.
      */
      favored_srv_pref = computed_orig_para->orig->orig_srv_domain_pref;
    }
    else
    {
      favored_srv_pref = CMPH_SS_SRV_DOMAIN_PREF(call_ptr->asubs_id, ph_ptr);
    }

    if( call_ptr->orig_mode != SD_SS_ORIG_MODE_NONE ||
        ( computed_orig_para != NULL                &&
          computed_orig_para->orig->orig_mode != SD_SS_ORIG_MODE_NONE &&
          cmcall_count_on_ss(call_ptr->ss) > 1
        )
      )
    {
      /* Force the service domain preferences in accordance with the
      ** call-type and originate service domain preferences.
      */
      favored_srv_pref = cmcall_get_favored_srv_domain_pref( call_ptr );

      call_ptr->orig_mode =
                     cmph_map_cm_call_type_to_sd_orig_mode( call_ptr->call_type,
                                                            call_ptr->ss );

      if( computed_orig_para != NULL )
      {
        rescan_time = cmph_force_pref_on_the_fly2(
                             call_ptr->ss,
                             ph_ptr,
                             force_reason,
                             cmph_map_call_type_to_act_type( call_ptr->call_type ),
                             CM_PREF_TERM_CM_1_CALL_PLUS,
                             computed_orig_para->orig->orig_mode_pref,
                             computed_orig_para->orig->orig_band_pref,
                             computed_orig_para->orig->orig_lte_band_pref,
                             computed_orig_para->orig->orig_tds_band_pref,
                             ph_ptr->main_stack_info.pref_info.plmn,
                             favored_srv_pref,
                             CM_NETWORK_SEL_MODE_PREF_NONE,
                             (cm_act_id_type) call_ptr->call_id,
                             CM_ACT_UPDATE_REAS_ACT_START,
                             call_ptr->force_top_pref,
                             cmph_get_acq_pri_order_per_sub(call_ptr->ss),
                             CM_DEFAULT_VALUE,
                             computed_orig_para->orig->csg_id,
                             computed_orig_para->orig->csg_rat,
                             computed_orig_para->orig->ciot_lte_pref);
         return rescan_time;
      } /* if( computed_orig_para != NULL ) */
      else
      {
        CM_ERR_0(" Error in cmcall_force_pref_on_th_fly()");
        return 0;
      }

    } /* call_ptr->orig_mode */

  } /* if((call_ptr->ss > CM_SS_NONE) && (call_ptr->ss < CM_SS_MAX)) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, this is the first time we force the preferences for this
  ** call origination.
  */

  /* Get the favored band preference to be used for this call-type and
  ** service-type.
  */
  favored_band_pref = cmcall_get_favored_band_pref( call_ptr->call_type,
                                                    call_ptr->srv_type,
                                                    CMPH_SS_BAND_PREF(asubs_id, ph_ptr)
                                                  );

  favored_lte_band_pref = cmcall_get_favored_lte_band_pref(
                                                call_ptr->call_type,
                                                CMPH_SS_LTE_BAND_PREF(asubs_id, ph_ptr)
                                                );

  favored_tds_band_pref = cmcall_get_favored_tds_band_pref(
                                                call_ptr->call_type,
                                                CMPH_SS_TDS_BAND_PREF(asubs_id, ph_ptr)
                                                );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Force the service domain preferences in accordance with the
  ** call-type and service domain preferences.
  */
  favored_srv_pref = cmcall_get_favored_srv_domain_pref( call_ptr);

  #ifdef FEATURE_MMODE_EMC_SUPPORT
  if ( call_ptr->call_type == CM_CALL_TYPE_EMERGENCY &&
       (call_ptr->ss != CM_SS_HYBR_2
        #ifdef FEATURE_MMODE_HYBR3_ENABLED
        ||
        call_ptr->ss != CM_SS_HYBR_3
        #endif
       )
        &&
       (!cmph_is_msim() ||
        cmph_is_subs_feature_mode_1x_sxlte(call_ptr->asubs_id))
     )
  {
  #ifdef FEATURE_MMODE_3GPP
      cm_cc_mode_pref_e_type    mode_usage = CM_CC_MODE_PREF_ANY;
  
      switch( mode_usage )
      {
        case CM_CC_MODE_PREF_ANY:
          favored_mode_pref = CM_MODE_PREF_EMERGENCY;
          break;

        case CM_CC_MODE_PREF_GW_ONLY:

          #if (defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)) \
            && defined(FEATURE_DOMAIN_SELECTION)
          if ((call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB) ||
              (call_ptr->csfb_type == CM_CALL_CSFB_TYPE_PPCSFB) ||
               cmsds_is_LTE_emerg_call_possible(call_ptr->asubs_id))
          {
            if (call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB)
            {
              favored_mode_pref = call_ptr->favored_mode_pref;
            }
            else
            {
            favored_mode_pref = CM_MODE_PREF_TDS_GSM_WCDMA_LTE;
            }
          }
          else
          #endif
          {
          favored_mode_pref = CM_MODE_PREF_TDS_GSM_WCDMA;
          }
          break;

        default:
          favored_mode_pref = CM_MODE_PREF_NONE;
          break;
      }
  #else
      favored_mode_pref = CM_MODE_PREF_EMERGENCY;
  #endif
  }
  else
  #endif
  {
    favored_mode_pref = call_ptr->favored_mode_pref;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Force the system selection preferences in accordance with the
  ** call-type and its favored mode and band preferences.
  */
  call_ptr->orig_mode =
               cmph_map_cm_call_type_to_sd_orig_mode( call_ptr->call_type,
                                                      call_ptr->ss );

  rescan_time = cmph_force_pref_on_the_fly2(
                                     call_ptr->ss,
                                     ph_ptr,
                                     force_reason,
                                     cmph_map_call_type_to_act_type( call_ptr->call_type ),
                                     CM_PREF_TERM_CM_1_CALL_PLUS,
                                     favored_mode_pref,
                                     favored_band_pref,
                                     favored_lte_band_pref,
                                     favored_tds_band_pref,
                                     ph_ptr->main_stack_info.pref_info.plmn,
                                     favored_srv_pref,
                                     CM_NETWORK_SEL_MODE_PREF_NONE,
                                     (cm_act_id_type) call_ptr->call_id,
                                     CM_ACT_UPDATE_REAS_ACT_START,
                                     call_ptr->force_top_pref,
                                     cmph_get_acq_pri_order_per_sub(call_ptr->ss),
                                     CM_DEFAULT_VALUE,
                                     #if !defined(FEATURE_MMODE_DISABLE_CSG)
                                     ph_ptr->main_stack_info.pref_info.csg_id,
                                     #else
                                     SYS_CSG_ID_INVALID,
                                     #endif
                                     ph_ptr->main_stack_info.pref_info.csg_rat,
                                     ciot_lte_pref
                                     );


   return rescan_time;
} /* cmcall_force_pref_on_the_fly() */
#endif

/*===========================================================================

FUNCTION cmcall_emergency_call_active

DESCRIPTION
  Check all call id's to see if there is an emergency call active

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean  cmcall_emergency_call_active
(
    void
)
{

  cmcall_s_type   *call_ptr;                    /* Call object pointer */
  cm_iterator_type call_itr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( call_ptr->call_state != CM_CALL_STATE_IDLE &&
        call_ptr->call_type  == CM_CALL_TYPE_EMERGENCY )
    {
      return TRUE;
    } /* if ( call_ptr->call_state != CM_CALL_STATE_IDLE ) */

    call_ptr = cmcall_obj_get_next(&call_itr);
  }
#ifndef FEATURE_CM_DISABLE_IMS_EXT
  if(cmemg_is_ext_e911_mode())
  {
    return TRUE;
  }
#endif
  return FALSE;

} /* cmcall_emergency_call_active() */


#if  defined(FEATURE_MMODE_GW) && !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
/*===========================================================================

FUNCTION cmcall_overall_end_calls

DESCRIPTION
  Ends all calls, except emergency calls when card is removed/in error.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_overall_end_calls
(
    void
)
{
  cmcall_overall_end_calls_per_subs(CM_SS_MAIN);
  #ifdef FEATURE_MMODE_HYBR2_ENABLED
  cmcall_overall_end_calls_per_subs(CM_SS_HYBR_2);
  #endif
  #ifdef FEATURE_MMODE_HYBR3_ENABLED
  cmcall_overall_end_calls_per_subs(CM_SS_HYBR_3);
  #endif

}
#endif

#if  defined(FEATURE_MMODE_GW) || !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
/*===========================================================================

FUNCTION cmcall_overall_end_calls_per_subs

DESCRIPTION
  Ends all calls for given ss, except emergency calls when card is removed/in error.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmcall_overall_end_calls_per_subs
(
    cm_ss_e_type ss
)
{
  cmcall_s_type   *call_ptr;                    /* Call object pointer */
  cm_iterator_type call_itr;

  #ifdef CM_GW_SUPPORTED
  gsm_wcdma_cmcall_ps_type *wcall_ps_mode_ptr = NULL;
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( call_ptr->call_state != CM_CALL_STATE_IDLE &&
         call_ptr->call_type  != CM_CALL_TYPE_EMERGENCY &&
         call_ptr->ss == ss
       )
    {
      call_ptr->end_status = CM_CALL_END_UIM_NOT_PRESENT;

      if (call_ptr->call_type == CM_CALL_TYPE_PS_DATA)
      {
        #ifdef CM_GW_SUPPORTED
        wcall_ps_mode_ptr =
                   &call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info;

        wcall_ps_mode_ptr->tear_down_ind.valid   = TRUE;
        wcall_ps_mode_ptr->tear_down_ind.ti_flag = TRUE;

        #endif
      }
      cmcall_end_call( call_ptr );
    } /* if ( call_ptr->call_state != CM_CALL_STATE_IDLE ) */

    call_ptr = cmcall_obj_get_next(&call_itr);
  }

}

#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */



#if (defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE)
/*===========================================================================

FUNCTION cmcall_proc_srv_status

DESCRIPTION
  Process service status change from no service to limited service.
  Note: In the case of PS attach on demand, the service status
        change and the service domain change are verified before
        originating a PS data call.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_proc_srv_status
(
  const cmss_s_type *ss_ptr,

  cm_ss_e_type      ss
)
{
  cmcall_s_type     *call_ptr = NULL;
  cm_iterator_type   call_itr;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ss_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*  Stop further processing if the overall state is idle.
  */
  if ( cmcall_overall_call_state_per_subs(ss) == CM_OVERALL_CALL_STATE_IDLE )
  {
    return;
  }

  cmcall_obj_iterator_init(&call_itr);

  while( (call_ptr = cmcall_obj_get_next(&call_itr)) != NULL)
  {
    if ( ( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS ||
           call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS
         ) &&
         call_ptr->call_state == CM_CALL_STATE_ORIG
           &&
         call_ptr->ss == ss
       )
    {
      /* Indicate that is now ok to send the origination command to lower layer
      ** and call on the origination processing function to process this
      ** indication.
      */
      /* Depending on SS process the correct service indicators
      */
      switch( call_ptr->ss )
      {
        case CM_SS_MAIN:
          if( call_ptr->sr.is_ok_to_orig )
          {
            break;
          }

          if ( ss_ptr->main_srv_status == SYS_SRV_STATUS_NO_SRV )
          {
            /*
            ** No service was established for the call.  End
            ** the call.
            */
            if ( call_ptr->call_type != CM_CALL_TYPE_EMERGENCY )
            {
              call_ptr->end_status = CM_CALL_END_NO_GW_SRV;
              #ifdef FEATURE_MMODE_GW
              cmwcall_end_req( call_ptr );
              #endif
			  #if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
              cmcall_end(call_ptr);
			  #endif
              continue;
            }
          } /* if srv_status == SYS_SRV_STATUS_NO_SRV ... */
          break;

        case CM_SS_HYBR_2:
            #ifdef FEATURE_MMODE_DUAL_SIM
            if (!cmph_is_msim() || cmph_is_sxlte())
              break;
  
            if( call_ptr->sr.is_ok_to_orig )
            {
              break;
            }
  
            if ( ss_ptr->info.gw_srv_status == SYS_SRV_STATUS_NO_SRV )
            {
              /*
              ** No service was established for the call.  End
              ** the call.
              */
              #ifndef FEATURE_MMODE_GW
              if ( call_ptr->call_type != CM_CALL_TYPE_EMERGENCY )
              {
                call_ptr->end_status = CM_CALL_END_NO_GW_SRV;
                cmwcall_end_req( call_ptr );
                cmcall_end(call_ptr);
                continue;
              }
              #endif
            } /* if srv_status == SYS_SRV_STATUS_NO_SRV ... */

            #endif
          break;

          case CM_SS_HYBR_3:
              #ifdef FEATURE_MMODE_HYBR3_ENABLED
              if( call_ptr->sr.is_ok_to_orig )
              {
                break;
              }

             if ( ss_ptr->info.gw3_srv_status == SYS_SRV_STATUS_NO_SRV )
             {
            /*
            ** No service was established for the call.  End
            ** the call.
            */
              if ( call_ptr->call_type != CM_CALL_TYPE_EMERGENCY )
              {
                call_ptr->end_status = CM_CALL_END_NO_GW_SRV;
                cmwcall_end_req( call_ptr );
                cmcall_end(call_ptr);
                continue;
              }
           } /* if srv_status == SYS_SRV_STATUS_NO_SRV ... */
          #endif
          break;

        default:
          if( call_ptr->sr.hdr_is_ok_to_orig )
          {
            CM_ASSERT( call_ptr->ss == CM_SS_HYBR_1 );
            continue;
          }
          break;
      } /* switch */
    } /* if call state != ORIG ...*/

    if(call_ptr->call_type == CM_CALL_TYPE_VOICE &&
      call_ptr->call_state == CM_CALL_STATE_ORIG)
    {
      CM_MSG_HIGH_1("ACTIVE_CALL: sr orig_sent_on_mode %d",
                    call_ptr->sr.orig_sent_on_mode);
    }

    /* To update sys_mode of all the calls depending on change in RAT */
    if( (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS
        || call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS
        || call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_LTE)
        &&
        ((call_ptr->call_state == CM_CALL_STATE_CONV)  ||
         (call_ptr->call_state == CM_CALL_STATE_IDLE)  ||
         (call_ptr->call_state == CM_CALL_STATE_INCOM) ||
         (call_ptr->call_state == CM_CALL_STATE_ORIG)  ||
         (call_ptr->call_state == CM_CALL_STATE_RECALL_RSP_PEND)
        )
        &&
        (
         (call_ptr->ss == CM_SS_MAIN
          && ss_ptr->main_srv_status != SYS_SRV_STATUS_NO_SRV
          && (ss_ptr->info.changed_fields & CM_SS_EVT_SYS_MODE_MASK) != 0
          && ( ss_ptr->info.sys_mode == SYS_SYS_MODE_GSM
               || ss_ptr->info.sys_mode == SYS_SYS_MODE_WCDMA
               || ss_ptr->info.sys_mode == SYS_SYS_MODE_LTE
               || ss_ptr->info.sys_mode == SYS_SYS_MODE_TDS )
         )
        #ifdef FEATURE_MMODE_HYBR2_ENABLED
         ||
         (call_ptr->ss == CM_SS_HYBR_2
          && ss_ptr->info.gw_srv_status != SYS_SRV_STATUS_NO_SRV
          && (ss_ptr->info.hybr_gw_changed_fields & CM_SS_GW_SYS_MODE_MASK) != 0
          && ( ss_ptr->info.gw_sys_mode == SYS_SYS_MODE_GSM
               || ss_ptr->info.gw_sys_mode == SYS_SYS_MODE_WCDMA
               || ss_ptr->info.gw_sys_mode == SYS_SYS_MODE_LTE
               || ss_ptr->info.gw_sys_mode == SYS_SYS_MODE_TDS )
         )
        #endif
        #ifdef FEATURE_MMODE_HYBR3_ENABLED
         ||
         (call_ptr->ss == CM_SS_HYBR_3
          && ss_ptr->info.gw3_srv_status != SYS_SRV_STATUS_NO_SRV
          && (ss_ptr->info.gw3_changed_fields & CM_SS_GW_SYS_MODE_MASK) != 0
          && ( ss_ptr->info.gw3_sys_mode == SYS_SYS_MODE_GSM
               || ss_ptr->info.gw3_sys_mode == SYS_SYS_MODE_WCDMA
               || ss_ptr->info.gw3_sys_mode == SYS_SYS_MODE_LTE
               || ss_ptr->info.gw3_sys_mode == SYS_SYS_MODE_TDS )
         )
         #endif
        )
      )
    {
      call_ptr->sys_mode = ( call_ptr->ss == CM_SS_MAIN ?
                             ss_ptr->info.sys_mode      :
                #ifdef FEATURE_MMODE_HYBR3_ENABLED
                (call_ptr->ss == CM_SS_HYBR_3?
                ss_ptr->info.gw3_sys_mode:
                ss_ptr->info.gw_sys_mode)
                #elif defined(FEATURE_MMODE_DUAL_SIM)
                ss_ptr->info.gw_sys_mode
                #else
                ss_ptr->info.sys_mode
                #endif
                             );
    }
  } /* while */
}
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_CM_LTE) */


#if defined FEATURE_MMODE_CALL_SUPS
/*===========================================================================
FUNCTION CMCALL_SUPSCMD_UPDATE_END_STATUS

DESCRIPTION
  This function sets the end_status of particular calls based on SUPS commands
  received when call is in different conversation states.

DEPENDENCIES
  Call object must be initialized
  cmcall_init().

RETURNS
  none

SIDE EFFECTS
  none

===========================================================================*/



void cmcall_supscmd_update_end_status
(
  cm_call_sups_type_e_type sups_type,

  cm_call_id_type call_id
)
{

  cmcall_s_type     *call_ptr = NULL;
  cm_iterator_type   call_itr;

  cmcall_obj_iterator_init(&call_itr);
  while((call_ptr = cmcall_obj_get_next(&call_itr)) != NULL)
  {
    switch(sups_type)
    {
       case CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB :

         if(
          (call_ptr->call_state == CM_CALL_STATE_CONV && call_ptr->call_subst.conv == CM_CALL_CONV_CALL_ON_HOLD ) ||
          ((call_ptr->call_state == CM_CALL_STATE_INCOM ) &&
          (call_ptr->call_type == CM_CALL_TYPE_VOICE || call_ptr -> call_type == CM_CALL_TYPE_EMERGENCY) )
           )
         {
           call_ptr->end_status = CM_CALL_END_CLIENT_END;
         }
         break;

       case CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL :

         if( call_ptr->call_state == CM_CALL_STATE_CONV && call_ptr->call_subst.conv == CM_CALL_CONV_CALL_ACTIVE)
         {
           call_ptr->end_status = CM_CALL_END_CLIENT_END;
         }
         break;

       case CM_CALL_SUPS_TYPE_ACTIVE_CALL_X_REL :
         if((call_ptr -> call_id == call_id) && (call_ptr->call_state == CM_CALL_STATE_CONV && call_ptr->call_subst.conv == CM_CALL_CONV_CALL_ACTIVE) )
         {
           call_ptr->end_status = CM_CALL_END_CLIENT_END;
         }
         break;

       default:
         break;
    }

    CM_MSG_HIGH_2("Updating end status for call_id = %d end_status= %d",call_ptr->call_id, call_ptr->end_status);

  }

}

#endif




/**--------------------------------------------------------------------------
** Functions - external:commands from top (cmtask)
** --------------------------------------------------------------------------
*/

#if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
/*===========================================================================

FUNCTION cmcall_timer_proc

DESCRIPTION
  Process timer events

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_timer_proc(

  cm_timer_event_type    timer_event
    /* Indicate specific timer event.
    **
    ** Note that currently timer event is not being used
    */
)
/*lint -esym(715,timer_event) */
{
  cmcall_s_type       *call_ptr;
      /* Point at call object */
  cm_iterator_type    call_itr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
    #error code not present
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do call state specific timer processing.
  */
    switch( call_ptr->call_state )
    {
      case CM_CALL_STATE_IDLE:
        break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case CM_CALL_STATE_ORIG:

        /* Call the orig function periodically to check the
        ** no-service, hold-orig, and silent redial situation.
        */
        cmcall_orig_proc( call_ptr );
        break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case CM_CALL_STATE_INCOM:
        break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case CM_CALL_STATE_CC_IN_PROGRESS:
        break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case CM_CALL_STATE_CONV:
        break; /* Nothing for GW; Processed after the switch for 1x */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case CM_CALL_STATE_RECALL_RSP_PEND:
        break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      default:
        CM_ERR_1( "Call state=%d",(uint8)call_ptr->call_state);
    } /* switch( call_ptr->call_state ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
    /* Do NON call state specific timer processing (and conv state).
    */
    if ( cmcall_ptr_is_allocated( call_ptr ) == TRUE  &&
         call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA )
    {
      cmxcall_timer_proc( call_ptr );
    } /* if ( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA ) */
    #endif /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while */

} /* cmcall_timer_proc() */
/*lint +esym(715,timer_event) */
#endif

#if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
/*===========================================================================
FUNCTION CMCALL_OFFLINE

DESCRIPTION
  This function performs offline processing for the call object

DEPENDENCIES
  Call object must be initialized

RETURNS
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_offline (
  sys_oprt_mode_e_type              oprt_mode
      /* Indicate the requested operating mode */
)
{
  cmcall_s_type       *call_ptr;
      /* Point at call object */

  cm_iterator_type    call_itr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  /*
  ** Send PDP_ABORT_REQ so that all PDP calls are locally deactivated by NAS,
  ** before CM unreserves SM connection id.
  */
  #if defined(CM_GW_SUPPORTED)
  #ifdef FEATURE_GRACEFUL_PDP_DEACTIVATION_B4_DETACH
  if(call_ptr != NULL && (*(cmph_get_pdp_deactivate_before_detach_supported_ptr())) == FALSE)
  #else
  if(call_ptr != NULL)
  #endif
  {
    cmwcall_send_pdp_abort_req(call_ptr->ss);
  }
  #endif /* CM_GW_SUPPORTED */

  while(call_ptr != NULL)
  {
    if(call_ptr->call_state != CM_CALL_STATE_IDLE)
    {
      /* If LPM request, don't end LTE calls. NAS will end them.
      */
     if((oprt_mode != SYS_OPRT_MODE_LPM)||
         (call_ptr->cmcall_mode_info.info_type != CM_CALL_MODE_INFO_LTE))
      {
        call_ptr->end_status = CM_CALL_END_OFFLINE;
        /* if an IP call, update mng cnf during LPM to NONE*/
     #if defined(FEATURE_IP_CALL) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
     if(call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP)
     {
       cmipcall_mng_state_update(CM_IP_CALL_IND_CALL_END,FALSE,call_ptr->call_id,CM_IPAPP_CAUSE_NORMAL);
     }
      #endif

     #ifndef FEATURE_TRIM_CMPMPRX    
        if((call_ptr->call_type == CM_CALL_TYPE_EMERGENCY) && (cmpmprx_is_special_volte_e911_calls() == TRUE) && cmpm_ptr()->is_pm_suspend)
	      {
	        /* this is to make sure that if E911 call is ending due to LPM/offline  mode 
	          then resume PM */
	        policyman_resume();
          cmpm_ptr()->is_pm_suspend = FALSE;
	      }
     #endif 
	      
        cmcall_end( call_ptr );
      }
    } /* if(call_ptr->call_state != CM_CALL_STATE_IDLE) */
    #ifdef FEATURE_MMODE_DUAL_ACTIVE
    /* if call state is idle i.e.  end request has been sent and if LPM mode has come, then on call end resume may not happen.
    so need to do resume flow for suspended stacks forcefully here */
    if(cmph_is_dsda() && (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY) && call_ptr->call_state == CM_CALL_STATE_IDLE
    && call_ptr->call_subst.idle == CM_CALL_IDLE_SUBST_ENDWAIT)
    {
      cmmsimc_suspend_or_resume_other_subs(call_ptr->asubs_id, FALSE);
    }
    #endif

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while */
#ifndef FEATURE_CM_DISABLE_IMS_EXT
  #ifdef FEATURE_CM_LTE
  cmemg_exit_emgergency_state (FALSE, NULL, SYS_MODEM_AS_ID_1);
  if (cmph_is_msim())
  {
    cmemg_exit_emgergency_state (FALSE, NULL, SYS_MODEM_AS_ID_2);
  }
  #endif
#endif
} /* cmcall_offline() */
#endif

#if 0
/*===========================================================================

FUNCTION cmcall_get_call_info_list_proc

DESCRIPTION
  This function processes the get call info list command.  This function
  returns to the requesting cilent a sanpshot of the state of all call objects
  maintained by CM.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  This function returns the state of the call objects in the info parameter.

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_get_call_info_list_proc
(
  cm_call_cmd_s_type  *call_cmd_ptr                       /* pointer to a CM command */
)
{


    cm_call_state_info_list_s_type  *info_list_ptr =
                      (cm_call_state_info_list_s_type*)
                      cm_mem_malloc(sizeof(cm_call_state_info_list_s_type));
      /* The state of the call objects maintained by CM. */

  cmcall_s_type    *call_ptr;
  uint8             active_call_index = 0;
  cm_iterator_type  call_itr;

  info_list_ptr->number_of_active_calls = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( call_cmd_ptr != NULL );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if((call_cmd_ptr->cmd_subs == SYS_MODEM_AS_ID_MAX)||
         (call_cmd_ptr->cmd_subs == call_ptr->asubs_id))
    {
      info_list_ptr->number_of_active_calls++;
      info_list_ptr->info[active_call_index].call_id           = cmcall_call_id_get( call_ptr );
      info_list_ptr->info[active_call_index].call_type         = call_ptr->call_type;
      info_list_ptr->info[active_call_index].call_state        = call_ptr->call_state;
      info_list_ptr->info[active_call_index].call_subst        = call_ptr->call_subst;
      info_list_ptr->info[active_call_index].sys_mode          = call_ptr->sys_mode;
      info_list_ptr->info[active_call_index].call_connect_time = call_ptr->call_connect_time;
      info_list_ptr->info[active_call_index].alpha             = call_ptr->alpha;
      info_list_ptr->info[active_call_index].call_direction    = CM_CALL_DIRECTION_NONE;
      info_list_ptr->info[active_call_index].primary_pdp       = FALSE;

      info_list_ptr->info[active_call_index].assoc_pri_profile_num = 0;
      #if defined(FEATURE_MMODE_GW)
      info_list_ptr->info[active_call_index].pdp_group_id          = 0;
      #endif
      info_list_ptr->
            info[active_call_index].is_net_alloc_qos_diff_from_req = FALSE;


      info_list_ptr->info[active_call_index].profile_number        = 0;

      #ifdef FEATURE_MMODE_GW
      #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
      if ( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS )
      {
        info_list_ptr->info[active_call_index].call_direction  = call_ptr->direction;

      }
      else 
      #endif
      if ( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS )
      {
        info_list_ptr->info[active_call_index].primary_pdp     = call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.primary_pdp;


        info_list_ptr->info[active_call_index].assoc_pri_profile_num          =
         call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.
          assoc_pri_profile_num;

        info_list_ptr->info[active_call_index].pdp_group_id                   =
         call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.pdp_group_id;
        
        info_list_ptr->info[active_call_index].is_net_alloc_qos_diff_from_req =
         call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.
          is_net_alloc_qos_diff_from_req;

        info_list_ptr->info[active_call_index].profile_number  = call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.profile_number;
      }
      #endif /* defined (FEATURE_GSM ) || defined (FEATURE_WCDMA ) */

      ++active_call_index;
    }
    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while */

  /* Callback to inform client with call info list */
  cmclient_call_info_list_ntfy (call_cmd_ptr->client_ptr, info_list_ptr);

  /* Free the allocated memory to heap after call back to notify is complete */
  cm_mem_free(info_list_ptr);

} /* cmcall_get_call_info_list_proc */
#endif

#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)

/*===========================================================================

FUNCTION cmcall_mc_rpt_proc

DESCRIPTION
  Process MC reports (i.e. notifications of call activity or
  replies to clients call commands).

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
)
{
  cmxcall_mc_rpt_proc( rpt_ptr );

} /* cmcall_mc_rpt_proc() */
#endif /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */



/*===========================================================================

FUNCTION cmcall_rpt_proc

DESCRIPTION
  Process lower layer reports (i.e. notifications of call activity or
  replies to clients call commands).

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */
)
/*lint -esym(715, rpt_ptr)*/
{
  #if defined(FEATURE_MMODE_3GPP)
  cmwcall_rpt_proc( rpt_ptr );
  #else
  CM_ERR_FATAL_0("Illegal mode. CM not compiled to support GSM/CDMA");
  #endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

} /* cmcall_rpt_proc() */
/*lint +esym(715,rpt_ptr)*/

#if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
/*===========================================================================

FUNCTION cmcall_process_ok_to_orig_gw

DESCRIPTION
  Process ok_to_orig for GW mode.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  info_type in call_ptr gets modified.

===========================================================================*/
static void cmcall_process_ok_to_orig_gw(

  sys_sys_mode_e_type ok_to_orig_mode,
    /* mode CDMA, WCDMA ... */

  cmcall_s_type       *call_ptr
    /* Call object */
)
{



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT (call_ptr != NULL);

  CM_ASSERT (BETWEEN (ok_to_orig_mode, SYS_SYS_MODE_NONE, SYS_SYS_MODE_MAX));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  if (cmcall_is_ipcall_over_mode_poss (ok_to_orig_mode, call_ptr))
  {
    /* For non emergency calls, if call can be originated over IP allow it to proceed
    ** over IP, if call is not SSAC barred over LTE*/
    if (call_ptr->call_type != CM_CALL_TYPE_EMERGENCY &&
      /* After SSAC check if is_volte flag is set to false, CM should not 
      send MMTEL voice/video orig req to IMS over LTE */
      !(call_ptr->is_volte == FALSE && ok_to_orig_mode == SYS_SYS_MODE_LTE))
    {
      call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_IP;
      return;
    }
    else
    {
      /* for emergency calls, check if curr system supports emerg. If CSFB is
       ** not the preferred mode for call origination, allow LTE IMS to be used
       */
      if ((ok_to_orig_mode                      == SYS_SYS_MODE_LTE
          #if (defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X) )
          && cmsds_ptr()->lte_ims_emerg_avail  == SYS_IMS_LTE_EMERG_SUPP_AVAILABLE
          && call_ptr->csfb_type               ==  CM_CALL_CSFB_TYPE_NONE
          #endif
          )||(call_ptr->is_em_vt))
      {
        call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_IP;
        return;
      }
    }
  }
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Given call will be originated as Packet call */
  if (call_ptr->call_type == CM_CALL_TYPE_PS_DATA)
  {
    call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_PS;
    return;
  }
  #if defined (FEATURE_LTE) && defined(FEATURE_LTE_TO_1X)

  if ((ok_to_orig_mode == SYS_SYS_MODE_LTE) &&
     ((call_ptr->call_type == CM_CALL_TYPE_VOICE) ||
      (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY) ||
      (call_ptr->call_type == CM_CALL_TYPE_PD)) &&
      (call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB))
  {
    call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_CDMA;
    CM_MSG_HIGH_1("1XCSFB: Updated ok_to_orig = %d",call_ptr->sr.is_ok_to_orig);
    return;
  }
  #endif

  
  /* Favoured mode_pref doesn't have LTE and currently camped RAT supports PPCSFB
  **recaculate mode_pref and csfb-type to treat the call as PPCSFB call.
  */
  #if defined (FEATURE_3GPP_CSFB)
  if ((call_ptr->csfb_type == CM_CALL_CSFB_TYPE_NONE) &&
      (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)  &&
      (cmsds_dom_sel_get_csfb_type_allowed(call_ptr->asubs_id) == CM_CALL_CSFB_TYPE_PPCSFB))
  {
    CM_MSG_HIGH_0("PPCSFB: Convert emergency call to PPCSFB emergency call");
    cmcall_convert_call_to_ppcsfb_call(call_ptr);
  }
  #endif
  
  /* If csfb and IP is not possible, this is an invalid ok to orig
  */
  if (ok_to_orig_mode == SYS_SYS_MODE_LTE
     #if defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)
     && call_ptr->csfb_type == CM_CALL_CSFB_TYPE_NONE
     #endif
  )
  {
    call_ptr->sr.is_ok_to_orig = FALSE;
    CM_MSG_HIGH_0("invalid lte ok to orig");
    return;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If origination was already sent, waiting for response from IMS
** So not updating info type.
*/
  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  if( call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_LTE
    && call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP)
  {
    CM_MSG_HIGH_0("Orig already sent on mode LTE on IP domain");
    return;
  }
  #endif

  /* All default call types to be originated as CS calls */
  call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_CS;
  CM_MSG_HIGH_1("cmcall_process_ok_to_orig_gw()-infotype:%d", call_ptr->cmcall_mode_info.info_type);

  return;

} /* cmcall_process_ok_to_orig_gw () */

/*===========================================================================

FUNCTION cmcall_convert_call_to_ppcsfb_call

DESCRIPTION
  Process system determination reports

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_convert_call_to_ppcsfb_call(cmcall_s_type   *call_ptr)
{
  #if defined(FEATURE_3GPP_CSFB)
  cm_orig_q_s_type      *q_ptr       = NULL;
  sd_ss_mode_pref_e_type mode_to_add = SD_SS_MODE_PREF_MAX;

  if (call_ptr == NULL)
  {
    return;
  }

   /*  SRVCCed calls/calls not in SR should not be converted from normal to PPCSFB 
     upon LTE OK_TO_ORIG */
  if(call_ptr->call_state != CM_CALL_STATE_ORIG
   #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
   || call_ptr->is_srvcc_call 
    #endif 
	)
  {
    return;
  }
  

  /* Update csfb_type */
  call_ptr->csfb_type = CM_CALL_CSFB_TYPE_PPCSFB;

  /* Update orig_mode */
  cmcall_update_orig_mode (call_ptr);

  /*  Favoured mode pref doent not have LTE so update it now */
	 
  if(!(sd_misc_is_mode_pref(cmph_map_cm_mode_pref_to_sd_mode_pref(call_ptr->favored_mode_pref),
	                    SD_SS_MODE_PREF_LTE )))
  {
  mode_to_add    = cmph_determine_mode_to_add(call_ptr->favored_mode_pref);
    CM_MSG_HIGH_2(" PPCSFB:adding LTE to mode_pref %d, calculated mode_to_add:%d", 
           call_ptr->favored_mode_pref,mode_to_add);

  if (call_ptr->call_type != CM_CALL_TYPE_EMERGENCY)
  {
    call_ptr->favored_mode_pref = cm_add_mode_pref_components(call_ptr->favored_mode_pref,
                                                         1,
                                                         mode_to_add);

  }
  
  /* Update priority queue preferences with new orig_mode and mode_pref */
  q_ptr = cmtask_orig_para_search_act_id(call_ptr->ss, call_ptr->call_id);
  if(q_ptr == NULL)
  {
    return;
  }
  
  q_ptr->orig->orig_mode = call_ptr->orig_mode;
  q_ptr->orig->orig_mode_pref = call_ptr->favored_mode_pref;
	
	 /* Force preferences again as top could be updated */
    cmcall_force_pref_on_the_fly(call_ptr, SD_SS_PREF_REAS_REDIAL_NORM);
  }
  #endif
}

/*===========================================================================

FUNCTION cmcall_recalculate_csfb_type

DESCRIPTION
  Process system determination reports

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_recalculate_csfb_type(cm_ss_e_type  ss, sys_modem_as_id_e_type asub_id)
{
  #if defined(FEATURE_3GPP_CSFB)
  cm_iterator_type call_itr;
      /* call Object Iterator */

  cmcall_s_type   *call_ptr = NULL;
     /* Pointer to call object */

  cmcall_obj_iterator_init(&call_itr);
  
  call_ptr = cmcall_obj_get_next(&call_itr);
  
  while(call_ptr != NULL)
  {
    if( ((call_ptr->call_type != CM_CALL_TYPE_VOICE) &&
         (call_ptr->call_type != CM_CALL_TYPE_CS_DATA)) ||
        (call_ptr->ss != CM_SS_MAX && call_ptr->ss != ss) ||
        ((call_ptr->ss == CM_SS_MAX ) && (ss == CM_SS_HYBR_2)) ||
        ((call_ptr->ss == CM_SS_MAX ) && (asub_id != call_ptr->asubs_id))
      ) 
    {
      call_ptr = cmcall_obj_get_next(&call_itr);
      continue;
    }

    /* Favoured mode_pref doesn't have LTE and currently camped RAT supports PPCSFB
    **recaculate mode_pref and csfb-type to treat the call as PPCSFB call.
    */
    if ((!cmcall_misc_is_mode_pref(call_ptr->favored_mode_pref,
                                  CM_MODE_PREF_LTE_ONLY)) &&
        (cmsds_dom_sel_get_csfb_type_allowed(call_ptr->asubs_id) == CM_CALL_CSFB_TYPE_PPCSFB))
    {
      CM_MSG_HIGH_0("Convert normal voice call to PPCSFB call");
      cmcall_convert_call_to_ppcsfb_call(call_ptr);
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  }
  #endif
}

/*===========================================================================

FUNCTION cmcall_sd_rpt_proc

DESCRIPTION
  Process system determination reports

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD report */
)
{
  /*lint -save -e826 */
  const cm_sd_rpt_u_type  *sd_rpt_ptr = (cm_sd_rpt_u_type *) rpt_ptr;
  /*lint -restore */
    /* Pointer to SD report */

  cmcall_s_type   *call_ptr = NULL;
      /* Pointer to call object */

  cmss_s_type   *ss_ptr = cmss_ptr();
      /* Serving System pointer */

  cm_iterator_type call_itr;
      /* call Object Iterator */

  sd_mode_e_type   ok_to_orig_mode = SD_MODE_NONE;
     /* Carries mode on which SD is giving ok to orig */

  cmph_s_type   *ph_ptr = cmph_ptr();
  cmmsc_state_machine_s_type *state_machine = NULL;

      /* Phone pointer */
  #ifdef FEATURE_MMODE_SC_SVLTE
  cm_call_id_type  call_id = CM_CALL_ID_INVALID;
  #endif
  cm_ss_e_type  ok_to_orig_cm_ss = CM_SS_MAIN;
  sys_modem_as_id_e_type  sd_asubs_id = SYS_MODEM_AS_ID_1;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sd_rpt_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ok_to_orig_mode = sd_rpt_ptr->ok_to_orig.si_ok_to_orig.mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( sd_rpt_ptr->hdr.cmd )
  {
                 /*----------------------------------------*/
                 /*  OK to send origination command to LL  */
                 /*----------------------------------------*/

    case CM_OK_TO_ORIG_F:
      #ifdef FEATURE_CM_DEBUG_BUFFER
      cmdbg_add_to_dbg_buffer(CM_BUFF_RPT_TYPE, CM_OK_TO_ORIG_F, ((void*)&(sd_rpt_ptr->ok_to_orig)));
      #endif
      ok_to_orig_cm_ss = cmph_map_sd_ss_type_to_cm_ss_type(sd_rpt_ptr->ok_to_orig.ss);
      sd_asubs_id = cmph_map_cm_ss_to_subs(ok_to_orig_cm_ss);

      CM_MSG_HIGH_6("ok_to_orig, ss %d, mode %d, srv_domain %d, asubs_id %d, ps_data_suspend %d, srv_status %d",
                  sd_rpt_ptr->ok_to_orig.ss,
                  sd_rpt_ptr->ok_to_orig.si_ok_to_orig.mode,
                  sd_rpt_ptr->ok_to_orig.si_ok_to_orig.srv_domain,
                  sd_asubs_id,
                  sd_rpt_ptr->ok_to_orig.si_ok_to_orig.ps_data_suspend,
                  sd_rpt_ptr->ok_to_orig.si_ok_to_orig.srv_status);

      if (ok_to_orig_mode == SD_MODE_LTE)
      {
        cmcall_recalculate_csfb_type(ok_to_orig_cm_ss, sd_asubs_id);
      }
      
      cmcall_obj_iterator_init(&call_itr);

      call_ptr = cmcall_obj_get_next(&call_itr);

      while(call_ptr != NULL)
      {

        /* If call state is not origination, discard this command.
        */

        /* Need to verify that the ok to orig is for this call
        ** with the priority Queues, it might happen that we force to the
        ** top of the Queue and the top is not conforming with the call
        ** No checks for roam and band as CM doesn't get those values witjh
        ** the OK to ORIG report
        */


        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Do not process the ok to orig for a particular call object if
        ** any of the following criteria are met
        ** 1. The call is not in origination state
        ** 2. The ok to orig mode is not part of the call's favored mode
        **    preference
        ** 3. The ok to orig ss is not the call's ss (exception being the
        **    case when call_ptr->ss is CM_SS_MAX. This happens whenever the
        **    hybrid preferences are toggled to make the call )
		** 4. If call already transferred to CS as part of SRVCC,ignore updating
		      mode info again as IMS still in process of deregistering
			  voice support
        */

        if( call_ptr->call_state != CM_CALL_STATE_ORIG
            ||!cmcall_misc_is_mode_pref(call_ptr->favored_mode_pref,
               cmph_map_sd_mode_pref_to_cm_mode_pref((sd_ss_mode_pref_e_type)
                             BM(sd_rpt_ptr->ok_to_orig.si_ok_to_orig.mode)))
            || (   call_ptr->ss != CM_SS_MAX
                && call_ptr->ss != cmph_map_sd_ss_type_to_cm_ss_type(
                                                  sd_rpt_ptr->ok_to_orig.ss))
            || ( ( call_ptr->ss == CM_SS_MAX ) &&
                 ( sd_rpt_ptr->ok_to_orig.ss == SD_SS_HYBR_2) )
            || ( ( call_ptr->ss == CM_SS_MAX ) &&
                 ( sd_asubs_id != call_ptr->asubs_id) )
#ifdef FEATURE_MMODE_SRVCC
            || (call_ptr->is_srvcc_call &&  (call_ptr->call_type == CM_CALL_TYPE_VOICE ||
                call_ptr->call_type == CM_CALL_TYPE_VT || call_ptr->call_type ==  CM_CALL_TYPE_EMERGENCY)
                && call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS )
#endif
            || (call_ptr->is_vowlan)
          )
        {
          call_ptr = cmcall_obj_get_next(&call_itr);
          continue;
        }

        CM_MSG_HIGH_6("Matching call found for ok_to_orig: callid:%d,ss: %d,callst=%d,call_type: %d ,favored_mode=%d ,orig_mode=%d", 
                                     call_ptr->call_id,
                                     call_ptr->ss,call_ptr->call_state,
                                     call_ptr->call_type,
                                     call_ptr->favored_mode_pref,
                                     sd_rpt_ptr->ok_to_orig.si_ok_to_orig.mode);



        state_machine = cmmsimc_state_machine_ptr(call_ptr->asubs_id);

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Indicate that is now ok to send the origination command to MC
        ** and call on the origination processing function to process this
        ** indication.
        */

        switch(sd_rpt_ptr->ok_to_orig.ss)
        {

          #if (defined (FEATURE_HDR_HYBRID))
          case SD_SS_HYBR_HDR:
            if( sd_rpt_ptr->ok_to_orig.si_ok_to_orig.mode == SD_MODE_HDR )
            {
              call_ptr->sr.hdr_is_ok_to_orig = TRUE;
              break;
            }
            /* Deliberate fall through to the default case
            */
            /*lint -fallthrough */
          #endif

          case SD_SS_MAIN:
          default:

            if (ss_ptr != NULL)
            {
              #ifdef FEATURE_ECALL_IVS
              /* For ecall modem, allow call origination irrespective of srv_domain
              */
              if( ecall_get_ecall_operating_mode() ==  ECALL_MODE_ECALL_ONLY )
              {
                if( sd_rpt_ptr->ok_to_orig.ss == SD_SS_MAIN )
                {
                  call_ptr->sr.is_ok_to_orig = TRUE;
                }
              }
              /* We are already camped, so go ahead with origination.
              */
              else if ( sd_rpt_ptr->ok_to_orig.si_ok_to_orig.srv_domain ==
                                                      SYS_SRV_DOMAIN_CAMPED )
              #else

              /* We are already camped, so go ahead with origination.
              */
              if ( sd_rpt_ptr->ok_to_orig.si_ok_to_orig.srv_domain ==
                                                      SYS_SRV_DOMAIN_CAMPED )
             #endif   /* FEATURE_ECALL_IVS */
              {
                if( sd_rpt_ptr->ok_to_orig.ss == SD_SS_MAIN ||
                    sd_rpt_ptr->ok_to_orig.ss == SD_SS_HYBR_2 ||
                    sd_rpt_ptr->ok_to_orig.ss == SD_SS_HYBR_3 )
                {
                  call_ptr->sr.is_ok_to_orig = TRUE;
                }
                else
                {
                  call_ptr->sr.hdr_is_ok_to_orig = TRUE;
                }
              }
              else
              {
                /* If PS call and service domain is cs only, does not allow to
                ** origination.
                */
                if(! (call_ptr->call_type == CM_CALL_TYPE_PS_DATA   &&
                      sd_rpt_ptr->ok_to_orig.si_ok_to_orig.srv_domain ==
                                                      SYS_SRV_DOMAIN_CS_ONLY)
                )
                {
                  #ifdef FEATURE_MMODE_SC_SVLTE
                  /* if call requires hybrid stack shutdown, do so and break
                  ** If not, continue with processing.
                  ** Hybrid stack shutdown is required only for calls that satisfy
                  ** the following conditions -
                  ** a) ps data call
                  ** b) originated over CDMA
                  ** c) should not be an MO special_srv_data_call originated by DS to
                  **    clean up existing data session over 1x.
                  */
                  if(call_ptr->call_type  == CM_CALL_TYPE_PS_DATA &&
                     ok_to_orig_mode      == SYS_SYS_MODE_CDMA    &&
                     cmcall_is_1x_data_srv_opt(call_ptr->cmcall_mode_info.\
                                                 info.cdma_info.srv_opt) &&
                     !call_ptr->cmcall_mode_info.info.cdma_info.is_special_srv_data_call &&
                     !cmxcall_outgoing_data_call_proceed())
                  {
                    break;
                  }
                  #endif //FEATURE_MMODE_SC_SVLTE
                  /*
                  ** Does not allow origination if call type is Voice or CS_DATA
                  ** and ok_to_orig srv_domain is PS_ONLY
                  */
                  if ( !(((call_ptr->call_type == CM_CALL_TYPE_CS_DATA) ||
                          (call_ptr->call_type == CM_CALL_TYPE_VOICE))  &&
                          (sd_rpt_ptr->ok_to_orig.si_ok_to_orig.srv_domain ==
                                                    SYS_SRV_DOMAIN_PS_ONLY)))
                  {
                    if( sd_rpt_ptr->ok_to_orig.ss == SD_SS_MAIN ||
                        sd_rpt_ptr->ok_to_orig.ss == SD_SS_HYBR_2 ||
                        sd_rpt_ptr->ok_to_orig.ss == SD_SS_HYBR_3
                      )
                    {
                      call_ptr->sr.is_ok_to_orig = TRUE;
                    }
                    else
                    {
                      call_ptr->sr.hdr_is_ok_to_orig = TRUE;
                    }
                  }
                  /* Exception to the check above is when hdr is present on
                  ** Main and call can be originated through a Voip app
                  ** registered on HDR
                  **
                  ** MODE_INFO_IP and (ok_to_orig == TRUE) are checked to send
                  ** call orig on HDR as Voip. cmcall_is_voice_over_hdr_poss()
                  ** is not required here, since it is being done before
                  ** setting info_type.
                  */
                  else if ( (call_ptr->call_type == CM_CALL_TYPE_VOICE) &&
                            (call_ptr->ss        == CM_SS_MAIN)         &&
                            (sd_rpt_ptr->ok_to_orig.si_ok_to_orig.mode ==
                                                    SD_MODE_HDR )
                          )
                  {
                    call_ptr->sr.is_ok_to_orig = TRUE;
                  }
                  #if defined (FEATURE_LTE) && defined(FEATURE_LTE_TO_1X)
                  else if(((call_ptr->call_type == CM_CALL_TYPE_VOICE) ||
                           (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY) ||
                           (call_ptr->call_type == CM_CALL_TYPE_PD)) &&
                          (call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB)  &&
                          (sd_rpt_ptr->ok_to_orig.si_ok_to_orig.srv_domain ==
                                                          SYS_SRV_DOMAIN_PS_ONLY &&
                           sd_rpt_ptr->ok_to_orig.si_ok_to_orig.srv_status ==
                                                          SYS_SRV_STATUS_SRV) &&
                              (sd_rpt_ptr->ok_to_orig.ss == SD_SS_MAIN)
                         )
                  {
                    call_ptr->sr.is_ok_to_orig = TRUE;
                    CM_MSG_HIGH_1("1XCSFB: Updating ok_to_orig = %d",call_ptr->sr.is_ok_to_orig);
                  }
                  #endif
                  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
                  else if(cmcall_is_ipcall_over_mode_poss (ok_to_orig_mode, call_ptr))
                  {
                    call_ptr->sr.is_ok_to_orig = TRUE;
                    CM_MSG_HIGH_2("IP call: Updating ok_to_orig = %d for call_type=%d",call_ptr->sr.is_ok_to_orig, call_ptr->call_type);
                  }
                  #endif


                  #if defined( FEATURE_LTE_TO_1X ) && \
                      (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

                  if (((call_ptr->call_type == CM_CALL_TYPE_VOICE) ||
                       (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY) ||
                       (call_ptr->call_type == CM_CALL_TYPE_PD)) &&
                      (call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB) &&
                      (call_ptr->sr.is_ok_to_orig == TRUE)              &&
                      (sd_rpt_ptr->ok_to_orig.ss == SD_SS_MAIN)         &&
                      (sd_rpt_ptr->ok_to_orig.si_ok_to_orig.srv_status ==
                                             SYS_SRV_STATUS_LIMITED_REGIONAL) &&
                      (sd_rpt_ptr->ok_to_orig.si_ok_to_orig.mode == SD_MODE_LTE)
                     )
                  {
                    call_ptr->sr.is_ok_to_orig = FALSE;
                    CM_MSG_HIGH_1("1XCSFB: Frocing ok_to_orig = %d",call_ptr->sr.is_ok_to_orig);
                  }

                  #endif
                }

              } /* else */

            } /* if (ss_ptr != NULL) */

            break; /* case SD_SS_MAIN: default: */

        }

        switch(ok_to_orig_mode)
        {
          #if defined(FEATURE_MMODE_3GPP2)
          case SD_MODE_HDR:

            if (cmcall_is_ipcall_over_mode_poss (ok_to_orig_mode, call_ptr))
            {
              call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_IP;
            }
            else
            {
              call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_CDMA;
            }
            break;

          case SD_MODE_CDMA:
            if(call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE)
            {
              call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_CDMA;
            }
      /* If LTE to DO redirection flag is set, clear it on receiving ok_to_orig
            ** on 1x for the clients to know the true service status.
            */   
            if(ph_ptr->lte_do_redir)
            {
              cmph_reset_lte_do_redir();

              if(ss_ptr->main_buffer_srv_uptime != CMSS_INFORM_SRV_CHANGE_INTER_NONE)
              {
                cmss_update_cdma_srv_lte_do_redir();
              }
            }
            #endif
            #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
            if( call_ptr->is_waiting_on_esr_rsp == TRUE &&
                cmcall_sent_mo_call_on_1x_currently_in_single_mode(call_ptr))
            {
                CM_MSG_HIGH_0("Flag cleared is_waiting_on_esr_rsp");
                call_ptr->is_waiting_on_esr_rsp = FALSE;
            }
            #endif

            break;

          case SD_MODE_GSM:

            cmcall_process_ok_to_orig_gw (SYS_SYS_MODE_GSM, call_ptr);
            break;

          #ifndef FEATURE_MMODE_GPRS_LTE
          case SD_MODE_WCDMA:

            cmcall_process_ok_to_orig_gw (SYS_SYS_MODE_WCDMA, call_ptr);
            break;

          case SD_MODE_TDS:

            cmcall_process_ok_to_orig_gw (SYS_SYS_MODE_TDS, call_ptr);
            break;
          #endif

            #if defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X) || defined (FEATURE_IP_CALL)
          case SD_MODE_LTE:

            cmcall_process_ok_to_orig_gw (SYS_SYS_MODE_LTE, call_ptr);
            break;
          #endif

          case SD_MODE_GPS:
          default:
            CM_ERR_1 ("OK_TO_ORIG returned invalid mode %d",
                 call_ptr->cmcall_mode_info.info_type);
            cmcall_end (call_ptr);
          call_ptr = cmcall_obj_get_next(&call_itr);
            continue;
        }

        if(call_ptr->cmcall_mode_info.info_type != CM_CALL_MODE_INFO_IP)
        {
          /* since call mode info is not IP, set voip_sr.is_valid as FALSE
                      this is to make sure that IP silent redial is not triggered*/
          call_ptr->sr.voip_sr.is_valid = FALSE;
		  /* also mark is_volte flag as FALSE, to make sure that call is not treated as Volte anymore */
          call_ptr->is_volte = FALSE;
        }
        /* set is_valid flag as true for VOIP calls over LTE or HDR, exception is
                 E911 voip over HDR  */
        else if( ok_to_orig_mode == SD_MODE_LTE || ok_to_orig_mode == SD_MODE_HDR)
        {
          if(!( (ok_to_orig_mode == SD_MODE_HDR) && (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY) ))
          {
            call_ptr->sr.voip_sr.is_valid = TRUE;
          }
        }

        if((call_ptr->call_type == CM_CALL_TYPE_VOICE)|| (call_ptr->call_type == CM_CALL_TYPE_VT) || (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY))
        {
          CM_MSG_HIGH_2("sys_mode Change from %d to %d",call_ptr->sys_mode, ok_to_orig_mode);
          call_ptr->sys_mode = ok_to_orig_mode;
        }

        /* if WLAN e911 setting is enabled for OP1, and call type is emergency, and WWAN ok to orig 
                  is received then set call attempt state and twwan timer accordingly */
        #if 0
        if(cmph_get_sub_config_ptr(call_ptr->asubs_id)->is_wlan_e911_call_supp && call_ptr->call_type == CM_CALL_TYPE_EMERGENCY && 
          call_ptr->sr.is_ok_to_orig == TRUE)
        {
          call_ptr->sr.wlan_fallback_timer = -1;
          call_ptr->sr.emerg_call_attempt_state = CMCALL_ATTEMPT_STATE_WWAN_ONLY;
        }
        #endif

        if((call_ptr->cc_object_ptr!=NULL)&&
           (call_ptr->cc_object_ptr->is_envelope_command_sent == FALSE))
        {
          call_ptr->cc_object_ptr->is_envelope_command_sent = TRUE;
          /* Send the envelope command*/
          CM_MSG_HIGH_0( "phase 2 CC, send envelope command" );
          cmcc_send_envelope_cmd(call_ptr);
        }
        else
        {
          cmcall_orig_proc( call_ptr );
        }

        call_ptr = cmcall_obj_get_next(&call_itr);
      } /* while */

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

               /*-------------------------------------------*/
               /*   Acquisition on all systems has failed   */
               /*-------------------------------------------*/

    case CM_ACQ_FAIL_F:

      cmcall_obj_iterator_init(&call_itr);

      call_ptr = cmcall_obj_get_next(&call_itr);

      while(call_ptr != NULL)
      {

        /* If this report is not for this call, continue.
        */
        if( sd_rpt_ptr->acq_fail.ss != cmph_map_cm_ss_type_to_sd_ss_type(call_ptr->ss)
           #ifdef FEATURE_MMODE_DUAL_SIM
           && (!call_ptr->is_emerg_on_other_sub || sd_rpt_ptr->acq_fail.ss != SD_SS_MAIN
               || call_ptr->ss != CM_SS_MAX)
           #endif
          )
        {
              call_ptr = cmcall_obj_get_next(&call_itr);
          continue;
        }
        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        /* If standard OTASP origination is in progress, indicate the
        ** call-end status and end the call.
        */
        if( call_ptr->call_state  == CM_CALL_STATE_ORIG &&
            call_ptr->call_type   == CM_CALL_TYPE_STD_OTASP )
        {
          call_ptr->end_status = CM_CALL_END_ACTIVATION;
          cmcall_end( call_ptr );
        }
        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        /* If voip is available and 1x acquisition fails, the emergency call
        ** should be attempted over voip
        */
        #ifdef  FEATURE_VOIP_E911_SUPPORT
        else if( call_ptr->call_state  == CM_CALL_STATE_ORIG   &&
                 call_ptr->call_type == CM_CALL_TYPE_EMERGENCY &&
                 sd_rpt_ptr->acq_fail.ss == SD_SS_MAIN         &&
                 call_ptr->sr.voip_sr.e911_voip_redial ==
                   CMCALL_VOIP_REDIAL_PENDING )
        {
          call_ptr->end_status = CM_CALL_END_IP_RETRY;
          cmcall_orig_proc( call_ptr );
        }
        #endif // FEATURE_VOIP_E911_SUPPORT
        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        /* If we get ACQ_FAIL when we have LIMITED service,
        ** it means that full service was not found, so inform
        ** orig_proc of NO_GW_SRV.
        */
        else if( call_ptr->call_state  == CM_CALL_STATE_ORIG &&
                 sd_rpt_ptr->acq_fail.ss == SD_SS_MAIN &&
                 (cmss_intl_srv_info_ptr(CM_SS_MAIN)->srv_status == SYS_SRV_STATUS_LIMITED ||
                  cmss_intl_srv_info_ptr(CM_SS_MAIN)->srv_status == SYS_SRV_STATUS_LIMITED_REGIONAL)
               )
        {
          call_ptr->end_status = CM_CALL_END_NO_FULL_SRV;
          cmcall_orig_proc( call_ptr );
        }
        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        else if( call_ptr->call_state  == CM_CALL_STATE_ORIG &&
                 sd_rpt_ptr->acq_fail.ss == SD_SS_HYBR_2 &&
                (cmss_intl_srv_info_ptr(CM_SS_HYBR_2)->srv_status == SYS_SRV_STATUS_LIMITED ||
                 cmss_intl_srv_info_ptr(CM_SS_HYBR_2)->srv_status == SYS_SRV_STATUS_LIMITED_REGIONAL)
               )
        {
          call_ptr->end_status = CM_CALL_END_NO_FULL_SRV;
          cmcall_orig_proc( call_ptr );
        }
        else if( call_ptr->call_state  == CM_CALL_STATE_ORIG &&
                 sd_rpt_ptr->acq_fail.ss == SD_SS_HYBR_3 &&
                 ( cmss_intl_srv_info_ptr(CM_SS_HYBR_3)->srv_status  == SYS_SRV_STATUS_LIMITED ||
                   cmss_intl_srv_info_ptr(CM_SS_HYBR_3)->srv_status  == SYS_SRV_STATUS_LIMITED_REGIONAL
                 )
               )
        {
          call_ptr->end_status = CM_CALL_END_NO_FULL_SRV;
          cmcall_orig_proc( call_ptr );
        }
        /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        #ifdef FEATURE_HDR_HYBRID
        else if( call_ptr->call_state  == CM_CALL_STATE_ORIG &&
                 call_ptr->ss == CM_SS_HDR )
        {
          call_ptr->end_status = CM_CALL_END_CM_COLLOC_ACQ_FAIL;
          cmcall_orig_proc( call_ptr );
        }
        #endif

        #ifdef FEATURE_MMODE_DUAL_SIM
        else if(call_ptr->is_emerg_on_other_sub
          && call_ptr->call_state  == CM_CALL_STATE_ORIG)
        {
          call_ptr->end_status = CM_CALL_END_NO_FULL_SRV;
          cmcall_orig_proc( call_ptr );
        }
        #endif
          /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
        call_ptr = cmcall_obj_get_next(&call_itr);
      } /* while */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_SRV_IND_INFO_F:

    /* No service is reported, check if calls in silent re-dial should be
    ** updated
    */
    if (sd_rpt_ptr->srv_ind_info.si_info.srv_status == SYS_SRV_STATUS_NO_SRV)
    {
      #if defined (FEATURE_LTE_TO_1X) && defined (FEATURE_LTE)
      cmwcall_update_calls_on_srv_lost(sd_rpt_ptr->srv_ind_info.ss, FALSE);
      #elif defined (CM_GW_SUPPORTED)
      cmwcall_update_calls_on_srv_lost(sd_rpt_ptr->srv_ind_info.ss);
      #endif
    }

    break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_SUSPEND_SS_F:
      /* update the suspend status
      */
      #if defined(FEATURE_MMODE_SC_SVLTE) || defined(FEATURE_MMODE_DUAL_SIM)
      #ifdef FEATURE_CM_DEBUG_BUFFER
      cmdbg_add_to_dbg_buffer(CM_BUFF_RPT_TYPE, CM_SUSPEND_SS_F, ((void*)&(sd_rpt_ptr->suspend_ss)));
      #endif

      cmph_update_suspend_status(CM_SS_MAIN,
                                    FALSE, // indicates this is a response
                                    sd_rpt_ptr->suspend_ss.is_suspend);

      #if defined(FEATURE_MMODE_SC_SVLTE)
      /* If there is no pending suspend cmd and current suspend status is
      ** suspend, indicate to 1xcp to proceed
      */
      if( ph_ptr->ss_susp_info[CM_SS_HYBR_1].num_requests == 0 &&
          ph_ptr->ss_susp_info[CM_SS_HYBR_2].num_requests == 0 &&
         (!ph_ptr->is_svdo_supported ||
           ph_ptr->ss_susp_info[CM_SS_HYBR_1].is_suspend == TRUE)&&
         (!ph_ptr->is_svlte_supported ||
           ph_ptr->ss_susp_info[CM_SS_HYBR_2].is_suspend == TRUE)
         )
      {
        /* if there is an incoming page, indication to 1x to proceed
        */
        if(ph_ptr->is_1x_await_response)
        {
          cmxcall_handle_page_proceed_response(TRUE);
        }
        /* else if there is an so33 call active, proceed with the call
           origination
        */
        else
        {
          call_id = cmxcall_is_end_suspend_status();
          if(call_id != CM_CALL_ID_INVALID)
          {
            call_ptr = cmcall_ptr(call_id);

            if(call_ptr == NULL)
            {
              CM_ERR_1("call_ptr is NULL for call_id=%d",call_id);
              break;
            }

            if(call_ptr->call_state == CM_CALL_STATE_ORIG)
            {
              call_ptr->sr.is_ok_to_orig = TRUE;
              /* info_type should be filled by now. Safeside addition*/
              call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_CDMA;
              cmcall_orig_proc(call_ptr);
            }
          }
        }
      }
      #endif //FEATURE_MMODE_SC_SVLTE
      #endif
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_BSR_STAT_CHGD_F:

      #ifdef FEATURE_CM_DEBUG_BUFFER
      //cmdbg_add_to_dbg_buffer(CM_BUFF_RPT_TYPE, CM_BSR_STAT_CHGD_F, ((void*)&(sd_rpt_ptr->cdma_bsr_stat)));
      #endif
	  
      #if defined(FEATURE_MMODE_SC_SVLTE) || defined(FEATURE_MMODE_CDMA)

      CM_MSG_HIGH_1("BSR status %d", sd_rpt_ptr->cdma_bsr_stat.bsr_status );

      if(sd_rpt_ptr->cdma_bsr_stat.bsr_status)
      {
        /* Start the timer on receiving BSR start indication from SD. BSR status is set to TRUE in SD
        ** only if the current sysmode is CDMA. Once the timer expires, even
        ** if BSR is in progress, 3GPP2 calls are not holded and call origination will be forced
        ** to SD
        */
           dword curr_uptime  = time_get_uptime_secs();

           ss_ptr->cdma_ps_call_bsr_uptime = curr_uptime + ph_ptr->ps_call_optimized.timer_value;
           CM_MSG_HIGH_2("curr uptime %d, bsr_uptime %d",curr_uptime,ss_ptr->cdma_ps_call_bsr_uptime);
      }
      else if(ss_ptr->cdma_ps_call_bsr_uptime > 0)
      {
        ss_ptr->cdma_ps_call_bsr_uptime = 0;

        /* After BSR complete, if UE moved to a CDMA system, continue with
        ** call origination.
        */
        if(ss_ptr->info.sys_mode == SYS_SYS_MODE_CDMA){
          cdma_ps_call_hold_timer_set = FALSE;
          return;
        }
        else if(cdma_ps_call_hold_timer_set){

          cmcall_obj_iterator_init(&call_itr);

          cdma_ps_call_hold_timer_set = FALSE;

          /* After BSR complete, as UE moved to a 3GPP system, end the calls that were
          ** put in hold while the BSR search was going on.
          ** TDS is not considered as there is no roaming agreement between TDS and C2K operator.
          */
          while( (call_ptr = cmcall_obj_get_next(&call_itr)) != NULL)
          {
            if(cmss_ptr()->info.sys_mode == SYS_SYS_MODE_GSM)
            {
              call_ptr->end_status = CM_CALL_END_BSR_COMPLETE_GSM;
              cmcall_end(call_ptr);
            }
            #ifndef FEATURE_MMODE_GPRS_LTE
            else if(cmss_ptr()->info.sys_mode == SYS_SYS_MODE_WCDMA)
            {
               call_ptr->end_status = CM_CALL_END_BSR_COMPLETE_WCDMA;
               cmcall_end(call_ptr);
            }
            #endif
            
            CM_MSG_HIGH_1("call end status: %d ",call_ptr->end_status);
          }
        }
      }
      #endif
	  
      break;

    default:
      call_ptr = NULL;
      break;

  } /* switch( cm_mc_rpt_ptr->hdr.cmd ) */

} /* cmcall_sd_rpt_proc() */
#endif
/*===========================================================================
FUNCTION cmcall_remove_mode_pref_components

DESCRIPTION

  Remove given SD mode preference component(s) from given CM mode preference.

  Example:

  The below will remove all single mode pref components from
  CM_MODE_PREF_AUTOMATIC and end up with CM_MODE_PREF_NONE.

  cm_mode_pref = cmcall_remove_mode_pref_components(
                                           CM_MODE_PREF_AUTOMATIC,
                                           6,// because 6 components follow
                                           SD_SS_MODE_PREF_WCDMA,
                                           SD_SS_MODE_PREF_GSM,
                                           SD_SS_MODE_PREF_GPS,
                                           SD_SS_MODE_PREF_HDR,
                                           SD_SS_MODE_PREF_CDMA,
                                           SD_SS_MODE_PREF_AMPS );

DEPENDENCIES
  sd_misc_remove_mode_pref()

RETURN VALUE
  Return new preference or CM_MODE_PREF_MAX if any of the mapping
  functions failed.

SIDE EFFECTS
  None

===========================================================================*/
cm_mode_pref_e_type cmcall_remove_mode_pref_components(cm_mode_pref_e_type cm_mode_pref,
                                                        int number_of_components_to_remove,
                                                        ... )
{

  va_list arg_list;
  sd_ss_mode_pref_e_type sd_mode_pref;
  sd_ss_mode_pref_e_type sd_mode_pref_to_remove = SD_SS_MODE_PREF_NONE;

  /*
  ** First map given CM mode pref to SD mode pref
  */
  sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(cm_mode_pref);
  if( sd_mode_pref == SD_SS_MODE_PREF_NONE) {
    CM_ERR_0( "Mapping #1 failed !!!");
    return CM_MODE_PREF_NONE;
  }

  /*
  ** Initialize arg_list and make an internal pointer point
  ** at the first variable function argument.
  ** This will be the second parameter
  */
  /*lint -e{64} -e{530} -e{516} */
  va_start(arg_list, number_of_components_to_remove);

  /*
  ** Process second and subsequent function arguments.
  ** These are the SD components to be removed from given
  ** CM mode pref
  */

  while(number_of_components_to_remove--)
  {

    /* Point at next function argument */
    /*lint -e{10} -e{662} -e{826} -e{124}*/
    sd_mode_pref_to_remove = (sd_ss_mode_pref_e_type)va_arg(arg_list, int);

    if(!BETWEEN( sd_mode_pref_to_remove,
                 SD_SS_MODE_PREF_NONE,
                 SD_SS_MODE_PREF_MAX))
    {
      CM_ERR_1( "Bad sd_mode_pref = %d",sd_mode_pref_to_remove);
      CM_ASSERT(FALSE);
    }

    /* Remove it */
    sd_mode_pref = sd_misc_remove_mode_pref( sd_mode_pref,
                                             sd_mode_pref_to_remove );

  }

  /* Clean up */
  va_end(arg_list);

  /* Now map result back to CM mode pref enum value */
  cm_mode_pref = cmph_map_sd_mode_pref_to_cm_mode_pref(sd_mode_pref);
  if(cm_mode_pref == CM_MODE_PREF_MAX) {
    CM_ERR_0( "Mapping #2 failed !!!");
  }

  return cm_mode_pref;
}

#if defined(FEATURE_GPSONE) && !defined(FEATURE_CM_DISABLE_GPS)
/*===========================================================================
FUNCTION cmcall_add_mode_pref_components

DESCRIPTION

  Add given SD mode preference component(s) from given CM mode preference.

  Example:

  The below will add all single mode pref components from
  CM_MODE_PREF_AUTOMATIC and end up with CM_MODE_PREF_NONE.

  cm_mode_pref = cmcall_add_mode_pref_components(
                                           CM_MODE_PREF_AUTOMATIC,
                                           6,// because 6 components follow
                                           SD_SS_MODE_PREF_WCDMA,
                                           SD_SS_MODE_PREF_GSM,
                                           SD_SS_MODE_PREF_GPS,
                                           SD_SS_MODE_PREF_HDR,
                                           SD_SS_MODE_PREF_CDMA,
                                           SD_SS_MODE_PREF_AMPS );

DEPENDENCIES
  sd_misc_add_mode_pref()

RETURN VALUE
  Return new preference or CM_MODE_PREF_MAX if any of the mapping
  functions failed.

SIDE EFFECTS
  None

===========================================================================*/
cm_mode_pref_e_type cmcall_add_mode_pref_components(cm_mode_pref_e_type cm_mode_pref,
                                                        int number_of_components_to_add,
                                                        ... )
{
  va_list arg_list;
  sd_ss_mode_pref_e_type sd_mode_pref;
  sd_ss_mode_pref_e_type sd_mode_pref_to_add = SD_SS_MODE_PREF_NONE;

  /*
  ** First map given CM mode pref to SD mode pref
  */
  sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(cm_mode_pref);
  if( sd_mode_pref == SD_SS_MODE_PREF_NONE) {
    CM_ERR_0( "Mapping #1 failed !!!");
    return CM_MODE_PREF_NONE;
  }


  /*
  ** Initialize arg_list and make an internal pointer point
  ** at the first variable function argument.
  ** This will be the second parameter
  */
  /*lint -e{64} -e{530} -e{516} */
  va_start(arg_list, number_of_components_to_add);

  /*
  ** Process second and subsequent function arguments.
  ** These are the SD components to be removed from given
  ** CM mode pref
  */

  while(number_of_components_to_add--)
  {

    /* Point at next function argument */
    /*lint -e{10} -e{662} -e{826} -e{124}*/
    sd_mode_pref_to_add = (sd_ss_mode_pref_e_type)va_arg(arg_list, int);

    if(!BETWEEN( sd_mode_pref_to_add,
                 SD_SS_MODE_PREF_NONE,
                 SD_SS_MODE_PREF_MAX))
    {
      CM_ERR_1( "Bad sd_mode_pref = %d",sd_mode_pref_to_add);
      CM_ASSERT(FALSE);
    }

    /* Remove it */
    sd_mode_pref = sd_misc_add_mode_pref( sd_mode_pref,
                                             sd_mode_pref_to_add );
   }

  /* Clean up */
  va_end(arg_list);

  /* Now map result back to CM mode pref enum value */
  cm_mode_pref = cmph_map_sd_mode_pref_to_cm_mode_pref(sd_mode_pref);
  if(cm_mode_pref == CM_MODE_PREF_MAX) {
    CM_ERR_0( "Mapping #2 failed !!!");
    CM_ASSERT(FALSE);
  }

  return cm_mode_pref;

}
#endif
#ifndef FEATURE_CM_DISABLE_IMS_EXT
/*===========================================================================

  FUNCTION cmcall_ext_e911_clean_up

  DESCRIPTION
    Clean external e911 call state and object upon qmi_voice_dial

  DEPENDENCIES

  RETURN VALUE
    none

  SIDE EFFECTS
    none

===========================================================================*/

static void cmcall_ext_e911_clean_up(
  sys_modem_as_id_e_type  as_id
)
{
  cm_ss_e_type ss = CM_SS_NONE;

  /* Before we start to push the emergency call into queue, make sure
  ** access control for 3 party e911 is removed.
  */
  if (cmemg_get_reason_per_sub(as_id) == CM_AC_REASON_THIRD_PARTY_IMS_E911)
  {
    cm_orig_q_s_type  *ph_orig_ptr;
    /* Search the ptr
    */
    uint8 i = 0;

    CM_MSG_HIGH_0("Remove   ext E911 from queue. ");

    for (i = 0; i < CM_SS_MAX; i++)
    {
      ss = (cm_ss_e_type)i;
      if (cmph_map_cm_ss_to_subs(ss) == as_id)
      {
        ph_orig_ptr = cmtask_orig_para_search_act_id(ss, 
                                cmemg_get_act_id(ss));
    
        if( ph_orig_ptr != NULL )
        {
          /* Delete the priority object identified by act_id */
          cmtask_orig_para_delete(ss, ph_orig_ptr, TRUE );
        }
      }
    }

    cmemg_set_reason_per_sub( CM_AC_REASON_NONE, as_id);

    if((cmpmprx_is_special_volte_e911_calls() == TRUE) && cmpm_ptr()->is_pm_suspend)
    {
	    /* when call is originated via DIAL command, that time 
	    CM will suspend PM again, after verifying that call originated is indeed E911 call 
	    so resume PM here once */ 
	    policyman_resume();
      cmpm_ptr()->is_pm_suspend = FALSE;
    }
  }
}
#endif

#if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
/*===========================================================================

FUNCTION cmcall_set_favored_mode_pref_for_origination

DESCRIPTION

  Given call type, srv_type, DRS bit, compute favored mode preference and
  favored hybrid preference.

DEPENDENCIES

  None

RETURN VALUE

  TRUE if favored mode and hybrid preference were computed successfully,
  in which case they are written into fav_mode_pref_ptr and
  fav_hybr_pref_ptr, FALSE otherwise.

  If the function retured FALSE then CM wrote the values of CM_HYBR_PREF_NONE
  and CM_MODE_PREF_NONE into fav_hybr_pref_ptr and fav_mode_pref_ptr

SIDE EFFECTS

  None

===========================================================================*/
boolean  cmcall_set_favored_mode_pref_for_origination(

         cm_ss_e_type           ss,
         cm_call_type_e_type    call_type,
         cm_srv_type_e_type     srv_type,
         boolean                drs_bit,
         word                   srv_opt,
           /* In */

         cm_mode_pref_e_type   *fav_mode_pref_ptr
           /* Out */

)
/*lint -esym(715, drs_bit)*/
/*lint -esym(715, srv_opt)*/
/*lint -esym(715,ss)*/
{

  /* Table entry type */
  typedef struct {

    /* In */
    cm_call_type_e_type    call_type;
    cm_srv_type_e_type     srv_type;
    cm_mode_pref_e_type    mode_pref;

    /* Out */
    cm_mode_pref_e_type    favored_mode_pref;
  } cm_fav_mode_pref_table_entry;

  int i;

  const cmph_s_type   *ph_ptr = cmph_ptr();
    /* point at phone object */

  const cm_fav_mode_pref_table_entry *tep;
    /* point at table entry */

  boolean found_and_set;
    /* whether we found a match in the table and set favored_mode_pref
       and favored_hybr_pref in cmd_info_ptr */

  sys_modem_as_id_e_type   asubs_id = cmph_map_cm_ss_to_subs(ss);
  cmph_sub_config_s_type  *cmph_sub_info = cmph_get_sub_config_ptr(asubs_id);

  /*

    Conventions used in the table:

    - special cases are pulled out of the table so they are processed first,
      without the need of having to search the entire table
    - *_NONE means "Don't care" (wildcard), we don't look at it while searching the table
    - *_NO_CHANGE is mapped to current, it means "Don't change anything"
    - CM_MODE_PREF_CURRENT_LESS_HDR means current mode preference with HDR component removed

   */
  static const cm_fav_mode_pref_table_entry mode_pref_for_orig_table[] =
  {

    /* Special call types, where we know instantly what favored mode and hybrid preference for origination should be */
    #ifdef FEATURE_MMODE_EMC_SUPPORT
    /* If voip is available over hybrid HDR & FEATURE_VOIP_E911_SUPPORT is defined, the following preferences kick in
    { CM_CALL_TYPE_EMERGENCY,     CM_SRV_TYPE_NONE,           CM_MODE_PREF_NONE,           CM_MODE_PREF_EMERGENCY,                 CM_HYBR_PREF_ON        },
    else
     */
    { CM_CALL_TYPE_EMERGENCY,     CM_SRV_TYPE_NONE,           CM_MODE_PREF_NONE,           CM_MODE_PREF_EMERGENCY},
    #endif
    /*-----paragk
    ** Commented the entries on 10/20/2005, the entries here are for reference, the INTERSECT_OR_FORCE entry takes
    ** care of all the special conditions
    */

    /*
    ** User being picky about service type required, so we don't care about call type
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_HDR,            CM_MODE_PREF_NONE,           CM_MODE_PREF_HDR_ONLY,                  CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_CDMA_SPECIFIC,  CM_MODE_PREF_NONE,           CM_MODE_PREF_CDMA_ONLY,                 CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_CDMA_AUTOMATIC, CM_MODE_PREF_NONE,           CM_MODE_PREF_CDMA_ONLY,                 CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_CDMA_HDR,       CM_MODE_PREF_CDMA_ONLY,      CM_MODE_PREF_CDMA_ONLY,                 CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_CDMA_HDR,       CM_MODE_PREF_CDMA_AMPS_ONLY, CM_MODE_PREF_CDMA_ONLY,                 CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_CDMA_HDR,       CM_MODE_PREF_HDR_ONLY,       CM_MODE_PREF_HDR_ONLY,                  CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_CDMA_HDR,       CM_MODE_PREF_NONE,           CM_MODE_PREF_CDMA_HDR_ONLY,             CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_WCDMA,          CM_MODE_PREF_NONE,           CM_MODE_PREF_WCDMA_ONLY,                CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_GSM_WCDMA,      CM_MODE_PREF_GSM_ONLY,       CM_MODE_PREF_GSM_ONLY,                  CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_GSM_WCDMA,      CM_MODE_PREF_WCDMA_ONLY,     CM_MODE_PREF_WCDMA_ONLY,                CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_GSM_WCDMA,      CM_MODE_PREF_NONE,           CM_MODE_PREF_GSM_WCDMA_ONLY,            CM_HYBR_PREF_NO_CHANGE },
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_GSM,            CM_MODE_PREF_NONE,           CM_MODE_PREF_GSM_ONLY,                  CM_HYBR_PREF_NO_CHANGE },

    */

    /* We need to compute the intersection of the service requested and the current mode.
    ** If the intersection is not NONE then we need to use the intersection, otherwise we need to set the mode to what
    ** the user is requesting
    */

    /* Catch all when user specifies a particular service type */
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_NON_AUTOMATIC,  CM_MODE_PREF_NONE,           CM_MODE_PREF_INTERSECT_OR_FORCE},

    /* Catch all */
    { CM_CALL_TYPE_NONE,          CM_SRV_TYPE_NONE,           CM_MODE_PREF_NONE,           CM_MODE_PREF_NO_CHANGE}
  };

  /*-------------------------------------------------------------------------*/

  CM_ASSERT(fav_mode_pref_ptr != NULL);
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  /*
  ** If WLAN App is registered with IMS, we do NOT need to do fav mode calculations!
  */
  #if 0
  #if defined (FEATURE_IP_CALL) && defined (FEATURE_CM_LTE) 
  if((call_type != CM_CALL_TYPE_EMERGENCY) || 
     (call_type == CM_CALL_TYPE_EMERGENCY && cmph_sub_info->emcall_to_voicecall))
  #endif
  {
  if (cmcall_is_active_ip_app(SYS_SYS_MODE_WLAN, CM_CALL_TYPE_VOICE) ||
      cmcall_is_active_ip_app(SYS_SYS_MODE_WLAN, CM_CALL_TYPE_VT))
  {
    *fav_mode_pref_ptr = CMPH_SS_MODE_PREF(asubs_id, ph_ptr);
    *fav_hybr_pref_ptr = CMPH_SS_HYBR_PREF(asubs_id, ph_ptr);
    return TRUE;
  }
  }
  #endif

  /*-------------------------------------------------------------------------*/

  /* For PS data calls, check if the mode requested is a sub-set of the
  ** current mode, if it is then set the mode to the subset
  */

  /* Find match in the table */
  found_and_set = FALSE;
  for(i = 0; i < (int) (ARR_SIZE(mode_pref_for_orig_table)); i++)
  {

    /* Point at table entry */
    tep = &mode_pref_for_orig_table[i];

    /* Match against wildcard or specific value */
    if( ((tep->call_type == CM_CALL_TYPE_NONE) || (tep->call_type  == call_type)) &&
        ((tep->srv_type  == CM_SRV_TYPE_NONE)  || (tep->srv_type   == srv_type)
         ||((tep->srv_type == CM_SRV_TYPE_NON_AUTOMATIC) && (srv_type != CM_SRV_TYPE_AUTOMATIC))) &&
        ((tep->mode_pref == CM_MODE_PREF_NONE) || (tep->mode_pref  == CMPH_SS_MODE_PREF(asubs_id, ph_ptr))))
    {

      if(tep->favored_mode_pref == CM_MODE_PREF_INTERSECT_OR_FORCE )
      {
        /* Compute the intersection of the current mode and the service type
        ** requested and if the intersection is not NULL then use the intersection.
        ** If the intersection is NULL, then use the type requested by the client
        */
        cm_mode_pref_e_type cm_mode_pref;
        boolean is_force_allowed = (tep->call_type != CM_CALL_TYPE_PS_DATA );

        cm_mode_pref = cmcall_misc_intersect_or_force_mode_pref( srv_type, CMPH_SS_MODE_PREF(asubs_id, ph_ptr), is_force_allowed );

        if(cm_mode_pref == CM_MODE_PREF_NONE)
        {

          /*
          ** Quit if we failed
          */
          found_and_set = FALSE;
          break;
        }
        else
        {
          *fav_mode_pref_ptr = cm_mode_pref;
        }

      }
      /* Found a match, so set appropriate fields in the command and quit the loop */
      else if(tep->favored_mode_pref == CM_MODE_PREF_NO_CHANGE)
      {

          /* _NO_CHANGE means "use current".
          ** Gotta set it to current so it can be correctly mapped to SD enum later
          */
        *fav_mode_pref_ptr = CMPH_SS_MODE_PREF(asubs_id, ph_ptr);
      }
      else
      {

        /* Copy any other value directly */
        *fav_mode_pref_ptr = tep->favored_mode_pref;

      }
      /* Indicate that we found and processed it */
      found_and_set = TRUE;
      break;
    }
  }


  /* If match not found by the code above, flag an error and return */
  if(!found_and_set)
  {
    *fav_mode_pref_ptr = CM_MODE_PREF_NONE;

    CM_MSG_LOW_0("No match !!!");
    return found_and_set;
  }



  /*----------------------------------------------------------------------*/
  /*----------------------------------------------------------------------*/
  /*----------------------------------------------------------------------*/

  #if 0
  #if defined(FEATURE_CIOT)
  if ((call_type == CM_CALL_TYPE_PS_DATA) &&
       (srv_type != CM_SRV_TYPE_NONE)
     )
  {
    *fav_mode_pref_ptr = cm_remove_mode_pref_components(*fav_mode_pref_ptr, 1,
                                            SD_SS_MODE_PREF_LTE);
  }
  #endif
  #endif
  
  /* If mode pref is not part of stack capability - return call failure
  ** For emergency call, all capability is allowed.
  */

  #ifdef CM_DEBUG
  #error code not present
#endif
  return found_and_set;

}
/*lint +esym(715,drs_bit)*/
/*lint +esym(715, srv_opt) */
/*lint +esym(715,ss)*/



/*===========================================================================

FUNCTION cmcall_determine_mode_usage

DESCRIPTION
  This function given mode_pref, info_present in the command, target_capability,
  and srv_type will determine which mode is possible for origination.

DEPENDENCIES
  none

RETURN VALUE
  mode usage

SIDE EFFECTS
  none

===========================================================================*/
static cm_cc_mode_pref_e_type cmcall_determine_mode_usage
(
  cm_call_cmd_s_type  *call_cmd_ptr                       /* pointer to a CM command */
)
{

  cm_call_cmd_info_s_type      *cmd_info_ptr      = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_cmd_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_info_ptr      = CMD_INFO_PTR( call_cmd_ptr );

  CM_MSG_HIGH_3("gw_ps_present=%d, gw_cs_present=%d, cdma_present=%d",
                   cmd_info_ptr->is_gsm_wcdma_ps_info_present,
                   cmd_info_ptr->is_gsm_wcdma_cs_info_present,
                   cmd_info_ptr->is_cdma_info_present);

  /* First, check on the information given by the client.  If client only
  ** gives CM information for one mode, than we use that mode and there is no
  ** need to check any conditions further.
  */
  if ((cmd_info_ptr->is_gsm_wcdma_ps_info_present ||
       cmd_info_ptr->is_gsm_wcdma_cs_info_present)
       && (!cmd_info_ptr->is_cdma_info_present)
     )
  {
    return CM_CC_MODE_PREF_GW_ONLY;
  }

  if ( cmd_info_ptr->is_cdma_info_present
    && !cmd_info_ptr->is_gsm_wcdma_ps_info_present
    && !cmd_info_ptr->is_gsm_wcdma_cs_info_present
    )
  {
    return CM_CC_MODE_PREF_CDMA_ONLY;
  }

  /*-----------------------------------------------------------------------*/
  /* Second, the client sent CM both sets of parameters.
  ** Check the srv_type, and if that limits it to one mode than there is no
  ** need to check any conditions further.
  */

  /* Make sure srv_type is valid first
  */
  if (!BETWEEN(cmd_info_ptr->srv_type, CM_SRV_TYPE_NONE, CM_SRV_TYPE_MAX))
  {
    CM_ERR_1 ("Invalid srv type in originaion %d", cmd_info_ptr->srv_type);
    return CM_CC_MODE_PREF_NONE;
  }

  switch (cmd_info_ptr->srv_type)
  {
     case CM_SRV_TYPE_HDR:
     case CM_SRV_TYPE_CDMA_SPECIFIC:
     case CM_SRV_TYPE_CDMA_AUTOMATIC:
     case CM_SRV_TYPE_CDMA_HDR:
       return CM_CC_MODE_PREF_CDMA_ONLY;

     case CM_SRV_TYPE_GSM_WCDMA:
       return CM_CC_MODE_PREF_GW_ONLY;

     case CM_SRV_TYPE_AUTOMATIC:
     default:
        break;
  }

  /*-----------------------------------------------------------------------*/
  /* Third, the srv_type is automatic, so lets check favored mode pref to see
  ** if we can can limit it to one mode.  If so, than there is no need to
  ** check any more conditions.
  */

  /* To do this, we need to resolve the call type first
  ** We don't want to actually change anything in the call object,
  ** but we need to figure out whether this is a special emergency or otasp call.
  */


   if(!cmcall_misc_is_mode_pref( cmd_info_ptr->favored_mode_pref, CM_MODE_PREF_TDS_GSM_WCDMA_LTE))
   {
      return CM_CC_MODE_PREF_CDMA_ONLY;
   }   
   else if(!cmcall_misc_is_mode_pref( cmd_info_ptr->favored_mode_pref, CM_MODE_PREF_CDMA_HDR_ONLY))
   {
      return CM_CC_MODE_PREF_GW_ONLY;
   }
   else if(cmd_info_ptr->favored_mode_pref == CM_MODE_PREF_NONE)
   {
     return CM_CC_MODE_PREF_NONE;
  }

  /*-----------------------------------------------------------------------*/
  /* Fourth, the target capability is checked.
  ** If we still can not narrow it down
  ** to one mode, than it will be automatic.
  */

  /* Check whether GSM/WCDMA/LTE is supported
  */
  if (!sd_misc_is_supp_pref (SD_SS_MODE_PREF_GWL, SD_SS_BAND_PREF_ANY) &&
     !sd_misc_is_supp_pref_lte (SD_SS_MODE_PREF_LTE, SYS_LTE_BAND_MASK_CONST_ANY))
  {
    return CM_CC_MODE_PREF_CDMA_ONLY;
  }

  if (!sd_misc_is_supp_pref (SD_SS_MODE_PREF_CDMA, SD_SS_BAND_PREF_ANY))
  {
    return CM_CC_MODE_PREF_GW_ONLY;
  }

  return CM_CC_MODE_PREF_ANY;

} /* cmcall_determine_mode_usage */

#if defined(FEATURE_MMODE_GW) && (defined(FEATURE_MMODE_USS_SUPS) || defined(FEATURE_MMODE_SS_SUPS))
/*===========================================================================

FUNCTION cmcall_stop_orig_start_sups

DESCRIPTION
  This function will handle stopping the origination process and kicking off a
  sups command as dictated by call control.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmcall_stop_orig_start_sups
(
  cm_ss_e_type          ss,                     /* ss type */
  cmcall_s_type         *call_ptr,              /* pointer to call object */
  cm_cmd_type      *cmd_ptr                /* pointer to a CM command */
)
/*lint -esym(715,ss) */
/*lint -esym(550, parse_result) */
/*lint -esym(550, as_id) */
{
  boolean                         status                    = TRUE;
  cm_call_cmd_s_type              *call_cmd_ptr             = NULL;
  cm_orig_call_control_s_type     *gw_call_control_info_ptr = NULL;
  cmutil_parse_sups_result_e_type parse_result              = CMUTIL_PARSE_SUPS_RESULT_NONE;
  cmutil_sups_params_s_type       sups_cmd_params;
  sys_modem_as_id_e_type          as_id                     = SYS_MODEM_AS_ID_1;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);
  CM_ASSERT(cmd_ptr  != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_cmd_ptr             = CALL_CMD_PTR( cmd_ptr );
  gw_call_control_info_ptr = &call_ptr->gw_cc_modified_info;

  as_id = cmph_map_cm_ss_to_subs(ss);

  /* Initiliase voice_to_ss params */
  call_ptr->voice_to_ss.ss_code          = 0;
  call_ptr->voice_to_ss.ss_oper          = 0;
  call_ptr->voice_to_ss.ss_ref           = 0;
  call_ptr->voice_to_ss.uss_data.present = 0;
  call_ptr->voice_to_ss.uss_data.size    = 0;
  call_ptr->voice_to_ss.uss_data.uss_data_coding_scheme = 0;
  call_ptr->voice_to_ss.uss_data.orig_dcs_from_sim = 0;

  /* Initialize sups_cmd_params */
  memset(&sups_cmd_params, 0, sizeof(cmutil_sups_params_s_type));

  CM_MSG_HIGH_5 ("Invoking parse utility: alpha len %d num len %d is_modem_cc_api %d cc_result %d sups_cmd %d",
                gw_call_control_info_ptr->alpha.len,
                gw_call_control_info_ptr->num.len,
                call_ptr->is_modem_cc_api,
                gw_call_control_info_ptr->call_control_result,
                gw_call_control_info_ptr->sups_cmd);

  /* Parsing not required for USSD*/
  if (call_ptr->is_modem_cc_api == TRUE &&
      gw_call_control_info_ptr->call_control_result != CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_USSD )
  {
    if(gw_call_control_info_ptr->num.len > CM_MAX_NUMBER_CHARS)
    {
      CM_ERR_0("gw_call_control_info_ptr->num.len is out of boundary");
      status = FALSE;
      return status;
    }

    if((parse_result = cmutil_parse_sups_string(
                         gw_call_control_info_ptr->num,
                         gw_call_control_info_ptr->num.len,
                         &sups_cmd_params)) != CMUTIL_PARSE_SUPS_RESULT_SUCCESS )
    {
      /* Error in parsing */
      CM_MSG_HIGH_1(" Parsing error %d in cmcall_stop_orig_start_sups. voice_to_ss set to 0",
                   parse_result);
    }
  }
  else
  {
    sups_cmd_params.sups_cmd = gw_call_control_info_ptr->sups_cmd;
    switch (sups_cmd_params.sups_cmd)
    {
#if defined(FEATURE_MMODE_SS_SUPS)

      case CM_SUPS_CMD_ACTIVATE:
        *(&(sups_cmd_params.sups_params.activate_ss_params)) = *(&(gw_call_control_info_ptr->sups_params.activate_ss_params));
        break;

      case CM_SUPS_CMD_DEACTIVATE:
        *(&(sups_cmd_params.sups_params.deactivate_ss_params)) = *(&(gw_call_control_info_ptr->sups_params.deactivate_ss_params));
        break;

      case CM_SUPS_CMD_ERASE:
        *(&(sups_cmd_params.sups_params.erase_ss_params)) = *(&(gw_call_control_info_ptr->sups_params.erase_ss_params));
        break;

      case CM_SUPS_CMD_INTERROGATE:
       *(&(sups_cmd_params.sups_params.interrogate_ss_params)) = *(&(gw_call_control_info_ptr->sups_params.interrogate_ss_params));
        break;

      case CM_SUPS_CMD_REG_PASSWORD:
        *(&(sups_cmd_params.sups_params.reg_pwd_params)) = *(&(gw_call_control_info_ptr->sups_params.reg_pwd_params));
        break;

      case CM_SUPS_CMD_REGISTER:
        *(&(sups_cmd_params.sups_params.reg_ss_params)) = *(&(gw_call_control_info_ptr->sups_params.reg_ss_params));
        break;
#endif
#if defined(FEATURE_MMODE_USS_SUPS) 

      case CM_SUPS_CMD_PROCESS_USS:
        *(&(sups_cmd_params.sups_params.uss_params)) = *(&(gw_call_control_info_ptr->sups_params.uss_params));
        break;
#endif
      default:
        CM_ERR_0 ("Wrong SUPS cmd");
    }
  }

  /* Now let's figure out which Sups command to call.
  */
  CM_MSG_HIGH_2 ("cc modified sups_cmd: %d,cc_result = %d ", \
                gw_call_control_info_ptr->sups_cmd, \
                call_ptr->gw_cc_modified_info.call_control_result);

  switch( sups_cmd_params.sups_cmd )
  {

#if defined(FEATURE_MMODE_SS_SUPS)

    case CM_SUPS_CMD_ERASE:
       call_ptr->voice_to_ss.ss_code =
                       sups_cmd_params.sups_params.erase_ss_params.ss_code;
      call_ptr->voice_to_ss.ss_ref =
                                     sups_cmd_params.sups_params.erase_ss_params.ss_ref;
      call_ptr->voice_to_ss.ss_oper = eraseSS;

      status  = cm_sups_cmd_erase_cc_per_subs( SUPS_CMD_PTR(cmd_ptr)->cmd_cb_func,
                            SUPS_CMD_PTR(cmd_ptr)->data_block_ptr,
                            SUPS_CMD_PTR(cmd_ptr)->client_id,
                          &sups_cmd_params.sups_params.erase_ss_params,
                            NULL,
                            TRUE,
                            as_id);
      break;

    case CM_SUPS_CMD_REGISTER:
       call_ptr->voice_to_ss.ss_code =
                 sups_cmd_params.sups_params.reg_ss_params.ss_code;
      call_ptr->voice_to_ss.ss_ref =
                               sups_cmd_params.sups_params.reg_ss_params.ss_ref;
      call_ptr->voice_to_ss.ss_oper = registerSS;

       status = cm_sups_cmd_register_internal( SUPS_CMD_PTR(cmd_ptr)->cmd_cb_func,
                                               NULL,
                                               SUPS_CMD_PTR(cmd_ptr)->data_block_ptr,
                                               SUPS_CMD_PTR(cmd_ptr)->client_id,
                                               &sups_cmd_params.sups_params.reg_ss_params,
                                               NULL,
                                               CM_ORIG_CLIENT_CATEG_USER,
                                               TRUE,
                                               FALSE,
                                               as_id,
                                               &gw_call_control_info_ptr->alpha );
    break;
    case CM_SUPS_CMD_ACTIVATE:
      call_ptr->voice_to_ss.ss_code =
                  sups_cmd_params.sups_params.activate_ss_params.ss_code;
      call_ptr->voice_to_ss.ss_ref =
                               sups_cmd_params.sups_params.activate_ss_params.ss_ref;
      call_ptr->voice_to_ss.ss_oper = activateSS;

      status = cm_sups_cmd_activate_cc_per_subs( SUPS_CMD_PTR(cmd_ptr)->cmd_cb_func,
                               SUPS_CMD_PTR(cmd_ptr)->data_block_ptr,
                               SUPS_CMD_PTR(cmd_ptr)->client_id,
                               &sups_cmd_params.sups_params.activate_ss_params,
                               NULL,
                               TRUE,
                               as_id);
      break;

    case CM_SUPS_CMD_DEACTIVATE:
      call_ptr->voice_to_ss.ss_code =
                 sups_cmd_params.sups_params.deactivate_ss_params.ss_code;
      call_ptr->voice_to_ss.ss_ref =
                               sups_cmd_params.sups_params.deactivate_ss_params.ss_ref;
      call_ptr->voice_to_ss.ss_oper = deactivateSS;

      status = cm_sups_cmd_deactivate_cc_per_subs( SUPS_CMD_PTR(cmd_ptr)->cmd_cb_func,
                                 SUPS_CMD_PTR(cmd_ptr)->data_block_ptr,
                                 SUPS_CMD_PTR(cmd_ptr)->client_id,
                                 &sups_cmd_params.sups_params.deactivate_ss_params,
                                 NULL,
                                 TRUE,
                                 as_id);
      break;

    case CM_SUPS_CMD_INTERROGATE:
      call_ptr->voice_to_ss.ss_code =
                 sups_cmd_params.sups_params.interrogate_ss_params.ss_code;
      call_ptr->voice_to_ss.ss_ref =
                               sups_cmd_params.sups_params.interrogate_ss_params.ss_ref;
      call_ptr->voice_to_ss.ss_oper = interrogateSS;

      status = cm_sups_cmd_interrogate_cc_per_subs( SUPS_CMD_PTR(cmd_ptr)->cmd_cb_func,
                                  SUPS_CMD_PTR(cmd_ptr)->data_block_ptr,
                                  SUPS_CMD_PTR(cmd_ptr)->client_id,
                                  &sups_cmd_params.sups_params.interrogate_ss_params,
                                  NULL,
                                  TRUE,
                                  as_id);
      break;

    case CM_SUPS_CMD_REG_PASSWORD:
    call_ptr->voice_to_ss.ss_code =
    sups_cmd_params.sups_params.reg_pwd_params.ss_code.ss_code;
    call_ptr->voice_to_ss.ss_ref =
    sups_cmd_params.sups_params.reg_pwd_params.ss_ref;
    call_ptr->voice_to_ss.ss_oper = registerPassword;
    
    status = cm_sups_cmd_reg_password_cc_per_subs( SUPS_CMD_PTR(cmd_ptr)->cmd_cb_func,
                     SUPS_CMD_PTR(cmd_ptr)->data_block_ptr,
                     SUPS_CMD_PTR(cmd_ptr)->client_id,
                     &sups_cmd_params.sups_params.reg_pwd_params,
                     NULL,
                     TRUE,
                     as_id);
     break;

#endif
#if defined(FEATURE_MMODE_USS_SUPS)
    case CM_SUPS_CMD_PROCESS_USS:
     call_ptr->voice_to_ss.ss_oper = processUnstructuredSS_Request;
     call_ptr->voice_to_ss.ss_ref  = sups_cmd_params.sups_params.uss_params.ss_ref;
     call_ptr->voice_to_ss.uss_data = sups_cmd_params.sups_params.uss_params.uss_data;

     CM_MSG_HIGH_2 ("voice_to_ussd. ss_ref: %d ss_oper: processUnstructuredSS_Request (%d)",
                    call_ptr->voice_to_ss.ss_ref, processUnstructuredSS_Request);
      status = cm_sups_cmd_process_uss_internal( SUPS_CMD_PTR(cmd_ptr)->cmd_cb_func,
                                                 NULL,
                                                 SUPS_CMD_PTR(cmd_ptr)->data_block_ptr,
                                                 SUPS_CMD_PTR(cmd_ptr)->client_id,
                                                 &gw_call_control_info_ptr->sups_params.uss_params,
                                                 NULL,
                                                 CM_ORIG_CLIENT_CATEG_USER,
                                                 TRUE,
                                                 FALSE,
                                                 as_id,
                                                 &gw_call_control_info_ptr->alpha );
    break;
#endif
#if defined (FEATURE_MMODE_USS_SUPS) || defined (FEATURE_MMODE_SS_SUPS)
    case CM_SUPS_CMD_RELEASE:

      CM_MSG_HIGH_0 ("Sups release. No ss params");

    status = cm_sups_cmd_release_cc_per_subs( SUPS_CMD_PTR(cmd_ptr)->cmd_cb_func,
                               SUPS_CMD_PTR(cmd_ptr)->data_block_ptr,
                               SUPS_CMD_PTR(cmd_ptr)->client_id,
                               &gw_call_control_info_ptr->sups_params.release_ss_params,
                               NULL,
                               TRUE,
                               as_id);
      break;
#endif
    default:
      CM_ERR_1( "SS Call Control -- Unsupport mod to SS %d",
                      gw_call_control_info_ptr->sups_cmd);
      /*lint -save -e527 Unreachable
      **             When CM_DEBUG is defined, CM_ERR evaluates to exit(1)
      */
      status = FALSE;
      /*lint -restore */
  }

#if defined(FEATURE_MMODE_USS_SUPS) || defined FEATURE_MMODE_SS_SUPS

  if(sups_cmd_params.sups_cmd != CM_SUPS_CMD_RELEASE
    && sups_cmd_params.sups_cmd != CM_SUPS_CMD_PROCESS_USS)
  {
    CM_MSG_HIGH_3 ("voice_to_ss. ss_code: %d, ss_ref: %d ss_oper: %d",
                              call_ptr->voice_to_ss.ss_code,
                               call_ptr->voice_to_ss.ss_ref,
                               call_ptr->voice_to_ss.ss_oper);
  }
  /* Notify clients of what is happening */
  CM_MSG_HIGH_1 ("status: %d CM_CALL_EVENT_ORIG_MOD_TO_SS", status);
  cmcall_event( call_ptr, CM_CALL_EVENT_ORIG_MOD_TO_SS );

  /* The origination hasn't started yet, but we do have to clean up the call object */
  cmcall_unreserve_connection_id( call_ptr );

  /* Deallocate call id */
  cm_call_id_deallocate (call_cmd_ptr->info.call_id);
#endif
  return status;

}  /* cmcall_stop_orig_start_sups( call_ptr, cmd_ptr )  */
/*lint +esym(715,ss) */
/*lint +esym(550, parse_result) */
/*lint +esym(550, as_id) */


#endif  /* FEATURE_GSM || FEATURE_WCDMA */


static boolean is_GWT_srv_type
(
  cm_srv_type_e_type srv_type
)
{
  return    srv_type == CM_SRV_TYPE_WCDMA
         || srv_type == CM_SRV_TYPE_GSM
         || srv_type == CM_SRV_TYPE_GSM_WCDMA
         || srv_type == CM_SRV_TYPE_TDS
         || srv_type == CM_SRV_TYPE_TDS_GSM
         ;
}

/*===========================================================================

FUNCTION cmcall_client_reorig_phase2

DESCRIPTION

  This function will be invoked after CM gets ok_to_orig from SD
  and handles the second phase of call control.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_client_reorig_phase2
(
  cm_cmd_type  *cmd_ptr                       /* pointer to a CM command */
)
/*lint -esym(715,q_ptr) */
{
  cm_call_cmd_s_type           *call_cmd_ptr   = NULL;
  cm_call_cmd_info_s_type      *cmd_info_ptr   = NULL;
  cmss_s_type                  *ss_ptr         = cmss_ptr();

  cm_call_cmd_err_e_type        cmd_err        = CM_CALL_CMD_ERR_NOERR;

  cmcall_s_type                *call_ptr       = NULL;
  cm_cc_mode_pref_e_type        mode_usage     = CM_CC_MODE_PREF_NONE;
  cmph_s_type                  *ph_ptr         = cmph_ptr();


  word                          srv_opt = 0;

  cm_ss_e_type                  ss;
    /* ss on which this origination cmd is issued */

  sys_modem_as_id_e_type   asubs_id;
    /* sub on which this origination cmd is issued */

  cm_mode_pref_e_type        prev_favored_mode_pref;

  cm_orig_q_s_type * q_ptr = NULL;

  sys_sys_mode_e_type sys_mode = SYS_SYS_MODE_NONE; 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cmd_ptr != NULL);
  

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_cmd_ptr         = CALL_CMD_PTR( cmd_ptr );
  cmd_info_ptr         = CMD_INFO_PTR( call_cmd_ptr );


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** Get a pointer to call Object.
  */
  call_ptr = cmcall_ptr(cmd_info_ptr->call_id);

  if (call_ptr == NULL)
  {
    CM_MSG_HIGH_1("REOIG PHASE_2: Can not retrieve call pointer for call id = %d",
       cmd_info_ptr->call_id);
    return;
  }

  if(call_ptr->cc_object_ptr != NULL)
  {
    /* This sequence is complete, so free the memory. */
    CM_MSG_HIGH_0("REORIG_PHASE_2:call_ptr->cc_object_ptr freeing should be freed already");
    cmcc_object_deallocate( call_ptr->cc_object_ptr );
  }

  /* Reset the cmd to be the original origination command.
  ** If an error should occur, CM should not send out the cmd as a REORIG.
  */
  call_cmd_ptr->cmd = CM_CALL_CMD_ORIG;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Verify Service Type */
  if( (!BETWEEN(cmd_info_ptr->srv_type, CM_SRV_TYPE_NONE, CM_SRV_TYPE_MAX)) ||
      (cmd_info_ptr->srv_type == CM_SRV_TYPE_NON_AUTOMATIC))
  {
      cmd_err = CM_CALL_CMD_ERR_SRV_TYPE_P;
      CM_MSG_HIGH_1("REORIG2:Invalid Service Type = %d", cmd_info_ptr->srv_type);
      cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
      return;
  }

    /* Verify Call Type  */
  if(!BETWEEN(cmd_info_ptr->call_type, CM_CALL_TYPE_NONE, CM_CALL_TYPE_MAX))
  {
      cmd_err = CM_CALL_CMD_ERR_CALL_TYPE_P;
      CM_MSG_HIGH_1("REORIG2:Invalid Call Type = %d", cmd_info_ptr->call_type);
      cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
      return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  #ifdef FEATURE_MMODE_DUAL_SIM
  if (cmph_is_msim())
  {
    /* Get the SS, based on the subscription-id, on which this call is
    ** originated.
    */
    ss = cmph_map_subs_to_ss(call_cmd_ptr->cmd_subs);
    asubs_id = call_ptr->asubs_id = call_cmd_ptr->cmd_subs;
    call_ptr->ss = ss;
  }
  else
  #endif
  {
    ss = CM_SS_MAIN;
    asubs_id = call_ptr->asubs_id = SYS_MODEM_AS_ID_1;
  }
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  if((call_ptr->call_type == CM_CALL_TYPE_VT) && (cmd_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY))
  {
    call_ptr->is_em_vt = TRUE;
    CM_MSG_HIGH_1("Set is_em_vt %d in CC Ph2",call_ptr->is_em_vt);
  }
  else
  #endif
  {
    call_ptr->is_em_vt = FALSE;
  }

  /*-----------------------------------------------------------------------*/

  /* In case the SIM has changed the call_type*/
  cmcall_set_type(call_ptr,cmd_info_ptr->call_type);

  /*-----------------------------------------------------------------------*/

  

  /* For SGLTE, update ss based on call type */
  #ifdef FEATURE_MMODE_SC_SGLTE
  if (cmph_is_subs_feature_mode_sglte(asubs_id))
  {
    cm_ss_e_type domain_ss;
    cmd_err
      = cmcall_sglte_determine_ss_per_call_type(asubs_id,cmd_info_ptr->call_type, &domain_ss);
    if (cmd_err != CM_CALL_CMD_ERR_NOERR)
    {
      SYS_ARG_NOT_USED(domain_ss);
      CM_MSG_HIGH_0 ("SC_SGLTE: Ending the call in Reorig Phase 2");
      cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
      return;
    }
    else
    {
      call_ptr->ss = ss = domain_ss;
    }
  }
  #endif

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

  srv_opt = cmd_info_ptr->cdma_info.srv_opt;

  #endif

  prev_favored_mode_pref = call_ptr->favored_mode_pref;
  /*----------------------------------------------------------------------*/
  /* For an emergency call, check the domain.
  ** if call should be made over HYBR2 stack, allow the hybrid prefs
  ** to remain unchanged
  */
  #ifdef FEATURE_MMODE_EMC_SUPPORT
  if( cmd_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY )
  {
    if (cmsds_emergency_call_dom_selection(&call_ptr->csfb_type,&call_ptr->dom_sel,asubs_id) 
        == CM_SS_HYBR_2)
    {
      ss = CM_SS_HYBR_2;
      call_ptr->ss = CM_SS_HYBR_2;
      CM_MSG_HIGH_0("LTE_911: hybr2 call");
    }
  }
  #endif
  /*----------------------------------------------------------------------*/



  /* This function will write CM_MODE_PREF_NONE into favored_mode_pref
     if it was not able to compute it */
  if (!cmcall_set_favored_mode_pref_for_origination ( ss,
                                                      cmd_info_ptr->call_type,
                                                      cmd_info_ptr->srv_type,
                                                      #ifdef FEATURE_IS2000
                                                      cmd_info_ptr->cdma_info.drs_bit,
                                                      #else
                                                      TRUE,
                                                      #endif
                                                      srv_opt,
                                                      &cmd_info_ptr->favored_mode_pref ))
  {
      cmd_err = CM_CALL_CMD_ERR_FAVORED_MODE_PREF_FOR_ORIGINATION;
      CM_MSG_HIGH_0("Ending the call in Reorig Phase 2 as set_favored_mode_pref returned false");
      cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
      return;
  }


  CM_MSG_HIGH_0("Fav mode pref in phase 2 as computed:");
  cmdbg_print_fav_mode_pref_for_origination( call_ptr->asubs_id,
                                             cmd_info_ptr->call_type,
                                             cmd_info_ptr->srv_type,
                                             CMPH_SS_MODE_PREF(asubs_id, ph_ptr),
                                             cmd_info_ptr->favored_mode_pref );

  /*-----------------------------------------------------------------------*/

  /* Only able to originate if the phone/PRL supports the
  ** originating mode preference.
  */
  if( !cmph_is_valid_call_mode_pref_on_ss(ss, cmd_info_ptr->favored_mode_pref, NULL) )
  {
    CM_MSG_HIGH_0 ("Ending the call in Reorig Phase 2 as phone doesn't support the mode preference");
    cmd_err =  CM_CALL_CMD_ERR_SRV_TYPE_S;
    cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
    return;
  }

  /*-----------------------------------------------------------------------*/
  #if defined(FEATURE_MMODE_GW) && !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
  /*
  ** Call originations from Diag will have bearer capability
  ** populated by CM
  */
  if (call_cmd_ptr->client_id == CM_CLIENT_ID_ANONYMOUS)
  {
    if (cmd_info_ptr->is_gsm_wcdma_cs_info_present == TRUE)
    {
      cm_util_set_default_voice_bearer_cap_per_sub (
        &(cmd_info_ptr->cm_gw_mode_info_u.gsm_wcdma_info.bearer_capability_1),
        call_cmd_ptr->cmd_subs);
    }
  }
  #endif

  /*-----------------------------------------------------------------------*/

  mode_usage = cmcall_determine_mode_usage (CALL_CMD_PTR(cmd_ptr));

  /*-----------------------------------------------------------------------*/

    if(call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
    {
      mode_usage = CM_CC_MODE_PREF_ANY;
    }

    /* If SIM Call control is actually done then send the result of call control here */
    if( 
        #ifdef FEATURE_MMODE_3GPP
        call_ptr->gw_cc_modified_info.is_sim_cc_performed
        #endif
      )
    {
      cmcall_event( call_ptr, CM_CALL_EVENT_CC_RESULT_INFO);
    }

    /* If call is not allowed on either modes,
    ** Or if the call is not allowed on the one technology that is available for use.....
    ** then the call is effectively rejected by call control.
    */
    if ((
         #ifdef FEATURE_MMODE_3GPP
          (call_ptr->gw_cc_modified_info.call_control_result ==
                           CM_CC_RESULT_NOT_ALLOWED)
         #endif
        )
                                   ||
        (
        #ifdef FEATURE_MMODE_3GPP

        ((call_ptr->gw_cc_modified_info.call_control_result ==
                           CM_CC_RESULT_NOT_ALLOWED) &&
         (mode_usage == CM_CC_MODE_PREF_GW_ONLY))
        #endif
        )
       )
    {
      cmd_info_ptr->call_control_cmd_err = CM_CALL_CMD_ERR_CALL_CONTROL_REJECTED;
      cmd_info_ptr->error_cause = CM_CALL_CMD_ERR_CAUSE_NO_SUBSC;
     /* CM_MSG_HIGH_3 ("Ending the call in Reorig Phase 2 due to  "
                   "CC Rejected gw call_control result %d"
                   "cdma call_control result %d"
                   "mode usage %d",
                   call_ptr->gw_cc_modified_info.call_control_result,
                   call_ptr->cdma_cc_modified_info.call_control_result, mode_usage);
     */
      #ifdef FEATURE_MMODE_3GPP
      if(call_ptr->gw_cc_modified_info.error_cause !=
                            CM_CALL_CMD_ERR_CAUSE_NO_SUBSC)
      {
        cmd_info_ptr->error_cause = call_ptr->gw_cc_modified_info.error_cause;
      }
      #endif
      cmcall_client_cmd_err_deallocate (call_cmd_ptr, CM_CALL_CMD_ERR_CALL_CONTROL_REJECTED);
      return;
    }


    /* Process the call control results.
    */
    mode_usage = cm_nc_proc_call_control_result
    (    
      #ifdef FEATURE_MMODE_3GPP
      call_ptr->gw_cc_modified_info.call_control_result == CM_CC_RESULT_NOT_ALLOWED ? NULL: &call_ptr->gw_cc_modified_info,
      #else
      NULL,
      #endif
      &call_cmd_ptr->info.num,
      cmd_info_ptr->call_type,
      ss_ptr->info.sys_mode
    );

    if( mode_usage == CM_CC_MODE_PREF_NONE )
    {
      CM_MSG_HIGH_0 ("Ending the call in Reorig Phase 2 as mode_usage is coming as NONE");
      cmd_err =  CM_CALL_CMD_ERR_MODE_PREF_P;
      cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
      return;
    }

    #ifdef CM_GW_SUPPORTED
    /* Check for SS/USS modifications.
    ** Two cases CM needs to check for (1) 1x-Allowed GW-SS/USS
    **                                 (2) 1x-NotAllowed GW-SS/USS
    ** (1) If the NC preferences function still chose ANY, choose 1x only as CM
    **     will not be able to switch to SS/USS mid-origination.
    ** (2) End the call and switch over to a SS/USS command.
    */
    if ((call_ptr->gw_cc_modified_info.call_control_result ==
                                         CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_SS) ||
        (call_ptr->gw_cc_modified_info.call_control_result ==
                                         CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_USSD))
    {
      cm_mode_pref_e_type    mode_pref;
      switch( mode_usage )
      {
        case CM_CC_MODE_PREF_ANY:
          mode_pref = CM_MODE_PREF_AUTOMATIC;
          break;

        case CM_CC_MODE_PREF_CDMA_ONLY:
          mode_pref = CM_MODE_PREF_CDMA_ONLY;
          break;

        case CM_CC_MODE_PREF_GW_ONLY:
          mode_pref = CM_MODE_PREF_TDS_GSM_WCDMA;
          break;

        default:
          mode_pref = CM_MODE_PREF_NONE;
          break;
      }
#if 0
      if ((cmcall_misc_is_mode_pref(mode_pref, CM_MODE_PREF_CDMA_ONLY)) &&
          (call_ptr->cdma_cc_modified_info.call_control_result != CM_CC_RESULT_NOT_ALLOWED))
      {
        mode_usage = CM_CC_MODE_PREF_CDMA_ONLY;
      }
      else
#endif 
      {
        q_ptr = cmtask_orig_para_search_act_id(ss, (uint32)call_ptr->call_id);

        if (q_ptr != NULL)
        {
          if (cmtask_orig_para_delete( ss, q_ptr, TRUE) )
          {
            cmph_force_orig_mode_on_the_fly(ss);
          }
        }

        /* Voice call converted to SUPS, calling callback now with no error*/
        cmcall_client_cmd_err( call_cmd_ptr, CM_CALL_CMD_ERR_NOERR );

        #if defined(FEATURE_MMODE_GW) && (defined(FEATURE_MMODE_SS_SUPS) || defined(FEATURE_MMODE_USS_SUPS))
        if (!cmcall_stop_orig_start_sups( ss, call_ptr, cmd_ptr ))
        {
          CM_ERR_0( "Unable to start SUPS operations!");
        }
        #endif
        return;
      }
    }

     /* When call control modifies a normal voice call into an emergency
    ** call emergency call service type also gets set.
    ** Would be better after call_type is assigned to EMERG but
    ** grouping it here in featurization.
    */
    if (cmd_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
    {
      /* Copy emergency service category from call control result
       */
      call_ptr->num.emerg_srv_categ = cmd_info_ptr->num.emerg_srv_categ;

      CM_MSG_HIGH_1 ("Emerg serv categ returned by CC = %d",
                                   call_ptr->num.emerg_srv_categ);
    }

    #endif  /* FEATURE_WCDMA || FEATURE_GSM */

  /* By this time CC is done and calltype (OTA/CDMA type) is resolved */
  if ( CMPH_SS_DISABLE_CALL_TYPE_MASK(asubs_id, ph_ptr) & SYS_BM_64BIT(call_ptr->call_type))
  {
      CM_MSG_HIGH_0 ("Ending the call in Reorig Phase 2 ");
      /* Block this MO voice call */
      cmd_err =  CM_CALL_CMD_ERR_SRV_TYPE_S;
      cmcall_client_cmd_err_deallocate (CALL_CMD_PTR(cmd_ptr), cmd_err);
      return;
  }

   /*-----------------------------------------------------------------------*/

  /* If the call type is an emergency number,
  ** we should use any mode
  */
  if (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
  {
  #ifdef FEATURE_MMODE_3GPP
  #if 0
      if(call_ptr->cdma_cc_modified_info.call_control_result == CM_CC_RESULT_NOT_ALLOWED)
      {
        mode_usage = CM_CC_MODE_PREF_GW_ONLY;
      }
      else if(call_ptr->gw_cc_modified_info.call_control_result == CM_CC_RESULT_NOT_ALLOWED)
      {
        mode_usage = CM_CC_MODE_PREF_CDMA_ONLY;
      }
   #endif   
  #else
      mode_usage = CM_CC_MODE_PREF_ANY;
  #endif
  }

  #ifdef FEATURE_MMODE_3GPP2
  /* If the call type returned from call control is OTASP,
  ** then force the mode over to CDMA only.
  */
  if( call_ptr->call_type == CM_CALL_TYPE_NON_STD_OTASP ||
      call_ptr->call_type == CM_CALL_TYPE_STD_OTASP
    )
  {
    mode_usage = CM_CC_MODE_PREF_CDMA_ONLY;
  }
  #endif

  #ifdef FEATURE_MMODE_HYBR2_ENABLED
  if( ss == CM_SS_HYBR_2 )
  {
    /* Make sure that the mode_usage matches with the Services supported
    ** on HYBR 2
    */
    if( mode_usage == CM_CC_MODE_PREF_GW_ONLY ||
        mode_usage == CM_CC_MODE_PREF_ANY )
    {
      /* Originate the call on HYBR2
      */
      call_ptr->ss = CM_SS_HYBR_2;

      /* Also update the mode_usage to GW_ONLY
      ** cmcall_process_orig_mode will remove 1x/hdr from mode_pref
      */
      mode_usage = CM_CC_MODE_PREF_GW_ONLY;

    } /* mode_usage */
    else
    {
      /* If mode_usage of GW is not possible, then this call cannot be
      ** originated on Hybr2 stack.
      */
      CM_MSG_HIGH_0 ("Ending the call in Reorig Phase 2 Mode usage is not GW and ss is Hybr2");
      cmd_err =  CM_CALL_CMD_ERR_MODE_PREF_P;
      cmcall_client_cmd_err_deallocate (CALL_CMD_PTR(cmd_ptr), cmd_err);
      return;
    }
  }
  else
  #endif
    #ifdef FEATURE_MMODE_HYBR3_ENABLED
    if( ss == CM_SS_HYBR_3 )
    {
    /* Make sure that the mode_usage matches with the Services supported
        ** on HYBR 3
   */
    if( mode_usage == CM_CC_MODE_PREF_GW_ONLY ||
      mode_usage == CM_CC_MODE_PREF_ANY )
    {
      /* Originate the call on HYBR3
            */
            call_ptr->ss = CM_SS_HYBR_3;

      /* Also update the mode_usage to GW_ONLY
      ** cmcall_process_orig_mode will remove 1x/hdr from mode_pref
      */
            mode_usage = CM_CC_MODE_PREF_GW_ONLY;

            } /* mode_usage */
     else
    {
      /* If mode_usage of GW is not possible, then this call cannot be
      ** originated on Hybr3 stack.
            */
            CM_MSG_HIGH_0 ("Ending the call in Reorig Phase 2 Mode usage is not GW and ss is Hybr3");
      cmd_err =  CM_CALL_CMD_ERR_MODE_PREF_P;
      cmcall_client_cmd_err_deallocate (CALL_CMD_PTR(cmd_ptr), cmd_err);
      return;
          }
  }
        else
        #endif /* FEATURE_MMODE_HYBR3_ENABLED */
  {
    call_ptr->ss = cmcall_determine_ss_for_origination( cmd_info_ptr->favored_mode_pref,
                                                        cmd_info_ptr->favored_hybr_pref,
                                                        mode_usage,
                                                        call_ptr
                                                      );
  }


  if (call_ptr->ss == CM_SS_NONE)
  {
      CM_MSG_HIGH_0 ("Ending the call in Reorig Phase 2 as ss in NONE ");
      cmd_err = CM_CALL_CMD_ERR_SRV_STATE_S;
      cmcall_client_cmd_err_deallocate (CALL_CMD_PTR(cmd_ptr), cmd_err);
      return;

  }

  #if defined( FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)
    /* Force mode_usage to Any for 1xcsfb call if gw info is not present in the call
     ** orig command - this is to support making 1xcsfb call from QXDM
     */
  if (call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB
      && mode_usage == CM_CC_MODE_PREF_CDMA_ONLY
      && !cmd_info_ptr->is_gsm_wcdma_ps_info_present
      && !cmd_info_ptr->is_gsm_wcdma_cs_info_present)
  {
    mode_usage = CM_CC_MODE_PREF_ANY;
  }
  #endif


  /*-----------------------------------------------------------------------*/
  CM_MSG_HIGH_1( "mode_usage=%d",mode_usage );

  cmd_err = cmcall_process_orig_mode(mode_usage,call_ptr);

  if (cmd_err != CM_CALL_CMD_ERR_NOERR)
  {
    if(call_ptr != NULL) /*lint !e774 */
    {
      CM_MSG_HIGH_1 ("Ending the call in Reorig Phase 2 cmd_err:%d ",cmd_err);
      cmcall_client_cmd_err_deallocate (CALL_CMD_PTR(cmd_ptr), cmd_err);
      return;
    }
  }

  /* Check if previous mode preferences are subset of current mode usage
        if not then reject the call as SD has already the service based on the
        previous fav mode prev*/
  {
    sd_ss_mode_pref_e_type sd_mode_pref_1, sd_mode_pref_2;
    sd_mode_pref_1 = cmph_map_cm_mode_pref_to_sd_mode_pref(call_ptr->favored_mode_pref);
    sd_mode_pref_2 = cmph_map_cm_mode_pref_to_sd_mode_pref(prev_favored_mode_pref);
    if(!(SD_MODE_CONTAIN (sd_mode_pref_1, sd_mode_pref_2)))
    {
      CM_MSG_HIGH_2("Ending the call in Reorig Phase 2, call mode pref doesn't match with new calculated mode pref",
                  call_ptr->call_type, cmd_err);
      cmd_err =  CM_CALL_CMD_ERR_MODE_PREF_P;
      cmcall_client_cmd_err_deallocate (CALL_CMD_PTR(cmd_ptr), cmd_err);
      return;
    }

  }
  if (cmd_err != CM_CALL_CMD_ERR_NOERR)
  {
    cmcall_client_cmd_err_deallocate (CALL_CMD_PTR(cmd_ptr), cmd_err);
  }
  else
  {
    /* No errors, we can try to originate the call now */
    /* Always notify client of command error code
    ** (even if command is successful).
    */
    cmcall_client_cmd_err( CALL_CMD_PTR(cmd_ptr), cmd_err );

    /*
    ** Inform GSDI about the call start.
    */
    cmcall_update_gsdi_and_log_call_state(call_ptr->ss);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    call_ptr->call_state = CM_CALL_STATE_ORIG;
    /* Notify client list of origination event.
    */
    #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
    if(call_ptr->is_vowlan)
    {
      CM_MSG_HIGH_1("Set sys_mode as WLAN from %d",call_ptr->sys_mode);
      call_ptr->sys_mode = SYS_SYS_MODE_WLAN;
    }
    #endif
    cmcall_event( call_ptr, CM_CALL_EVENT_ORIG );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    cmcall_orig_proc( call_ptr );
  }



    SYS_ARG_NOT_USED(q_ptr);


}  /* cmcall_client_reorig_phase2 () */
/*lint +esym(715,q_ptr) */

#ifdef FEATURE_MMODE_EMC_SUPPORT
/*===========================================================================

FUNCTION cmcall_end_concurrent_volte_call

DESCRIPTION

  Handles concurrent volte call when e911 call is originated,
  based on the NV, VOLTE call is either terminated or will be on hold.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_end_concurrent_volte_call(
  sys_modem_as_id_e_type asubs_id
)
{

  #if defined(FEATURE_IP_CALL) 
  cm_iterator_type				end_call_itr;

  cmcall_s_type                 *end_call_ptr    = NULL;
  cmph_sub_config_s_type *ph_sub_config_ptr = cmph_get_sub_config_ptr(asubs_id);

  if(ph_sub_config_ptr == NULL)
  {
    return;
  }

  /* If VoIP call is up or in progress and it is not an emergency call
    ** and  concurrent voicecall NV is set to false, end call.
	   Otherwise continue without ending.
  */

  CM_MSG_HIGH_1("E911 call is orignated,hold_concurrent_voicecall %d",
    ph_sub_config_ptr->hold_concurrent_voicecall_during_emergency);
  cmcall_obj_iterator_init(&end_call_itr);
  if(!ph_sub_config_ptr->hold_concurrent_voicecall_during_emergency)
  {
    end_call_ptr = cmcall_obj_get_next(&end_call_itr);
	while(end_call_ptr != NULL)
    {

      if ( end_call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP
		&& end_call_ptr->call_type != CM_CALL_TYPE_EMERGENCY )
	  {
        cmipcall_send_end( end_call_ptr );
        /* End the call
        */
        end_call_ptr->end_status = CM_IP_CALL_END_EMERG_ORIG;
        CM_MSG_HIGH_1("E911 call is orignated, end VOLTE call:%d",end_call_ptr->call_id);
        cmcall_end (end_call_ptr);
      }
	  end_call_ptr = cmcall_obj_get_next(&end_call_itr);

	}
  }

  #endif

}
#endif

/*===========================================================================

FUNCTION cmcall_client_orig_proc

DESCRIPTION

  This function handles the origination command.
  This function first determines what mode is possible to originate the call on.
  Then depending on the possible modes, call the appropriate 1X and/or GW calls.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_client_orig_proc
(
  cm_call_cmd_s_type  *call_cmd_ptr                       /* pointer to a CM command */
)
{
  cm_call_cmd_info_s_type      *cmd_info_ptr   = NULL;
  cmss_s_type                  *ss_ptr         = cmss_ptr();

  cm_call_cmd_err_e_type        cmd_err        = CM_CALL_CMD_ERR_NOERR;

  cmcall_s_type                *call_ptr       = NULL;
  cm_cc_mode_pref_e_type        mode_usage     = CM_CC_MODE_PREF_NONE;
  cmph_s_type                  *ph_ptr         = cmph_ptr();
  cm_cc_subscription_source_e_type cdma_subscription_source;
  cm_cc_subscription_source_e_type gw_subscription_source;

  boolean                       cm_nc_status = TRUE;
  word                          srv_opt = 0;

  cm_ss_e_type                  ss;
    /* ss on which this origination cmd is issued */

  sys_modem_as_id_e_type   asubs_id;

  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE 	
    /* sub on which this origination cmd is issued */
  cmcall_ssac_chk_e_type   ssac_chk_result;
  #endif
  
  #ifdef FEATURE_MMODE_DUAL_SIM 
  boolean cdma_data_call_allowed = TRUE; 
  boolean cm_pm_cdma_call_poll = FALSE;
  #endif
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_cmd_ptr != NULL);
  

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_info_ptr         = CMD_INFO_PTR( call_cmd_ptr );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check that originating client is correct
  */
  if (!BETWEEN(call_cmd_ptr->client_categ,
               CM_ORIG_CLIENT_CATEG_NONE, CM_ORIG_CLIENT_CATEG_MAX))
  {
    CM_MSG_HIGH_1 ("ORIG: Originating client %d, is not User or Card", \
                  call_cmd_ptr->client_categ);
    cmd_err = CM_CALL_CMD_ERR_INVALID_ORIG_CLIENT_P;
    cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
    return;
  }

  /*
  ** Get a pointer to call Object.
  */
  call_ptr = cmcall_ptr(cmd_info_ptr->call_id);
  if (call_ptr == NULL)
  {
    CM_MSG_HIGH_3("ORIG: Can not retrieve call pointer for call id = %d call_type %d call_cmd %d",
       cmd_info_ptr->call_id, call_cmd_ptr->info.call_type, call_cmd_ptr->cmd );

    if( call_cmd_ptr->info.call_type != CM_CALL_TYPE_PS_DATA || call_cmd_ptr->cmd  == CM_CALL_CMD_ORIG)
    {
    cmd_err = CM_CALL_CMD_ERR_NO_CALL_ID_S;
    cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
    }
    return;
  }

  /* Set the client_categ of the call originator */
  call_ptr->client_categ = call_cmd_ptr->client_categ;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Verify Service Type  */
  if( (!BETWEEN(cmd_info_ptr->srv_type, CM_SRV_TYPE_NONE, CM_SRV_TYPE_MAX)) ||
      (cmd_info_ptr->srv_type == CM_SRV_TYPE_NON_AUTOMATIC))
  {
    cmd_err = CM_CALL_CMD_ERR_SRV_TYPE_P;
    CM_MSG_HIGH_1("ORIG:Invalid Service Type = %d", cmd_info_ptr->srv_type);
    cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
    return;
  }

    /* Verify Call Type  */
  if(!BETWEEN(cmd_info_ptr->call_type, CM_CALL_TYPE_NONE, CM_CALL_TYPE_MAX))
  {
    cmd_err = CM_CALL_CMD_ERR_CALL_TYPE_P;
    CM_MSG_HIGH_1("ORIG:Invalid Call Type = %d", cmd_info_ptr->call_type);
    cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
    return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #ifdef FEATURE_MMODE_DUAL_SIM
  if (cmph_is_msim())
  {
    /* Get the SS, based on the subscription-id, on which this call is
    ** originated.
    */
    ss = cmph_map_subs_to_ss(call_cmd_ptr->cmd_subs);
    asubs_id = call_ptr->asubs_id = call_cmd_ptr->cmd_subs;
    CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);
    CM_ASSERT_ENUM_IS_INRANGE(ss, CM_SS_MAX);

    /* Set client asubs_id to the subs id as sent by client orig cmd */
    call_ptr->client_asubs_id = call_cmd_ptr->cmd_subs;
    call_ptr->ss = ss;
  }
  else
  #endif
  {
    ss = CM_SS_MAIN;
    asubs_id = call_ptr->asubs_id = SYS_MODEM_AS_ID_1;
  }
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmcall_set_type(call_ptr,cmd_info_ptr->call_type);

  /* Set the direction of the call here */
  call_ptr->direction = call_cmd_ptr->direction;

  call_ptr->is_modem_cc_api = call_cmd_ptr->is_modem_cc_api;

  /* This check will eliminate the need for call control for non-CDMA
  ** voice calls, this check needs to be repeated for CDMA and automatic
  ** calls after call control returnns
  */
  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  if( ( CMPH_SS_DISABLE_CALL_TYPE_MASK(asubs_id, ph_ptr) & CM_CALL_TYPE_VOICE_MASK ) &&
        is_GWT_srv_type(cmd_info_ptr->srv_type) &&
        ( cmd_info_ptr->call_type == CM_CALL_TYPE_VOICE ||
          cmd_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY )
    )
  {
    /* Block this MO voice call */
    cmd_err =  CM_CALL_CMD_ERR_SRV_TYPE_S;
    cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
    return;
  }

  else if( ( CMPH_SS_DISABLE_CALL_TYPE_MASK(asubs_id, ph_ptr) &
                                              CM_CALL_TYPE_CS_DATA_MASK ) &&
      is_GWT_srv_type(cmd_info_ptr->srv_type) &&
      ( cmd_info_ptr->call_type == CM_CALL_TYPE_CS_DATA )
    )
  {
    /* Block this CS Data call */
      cmd_err =  CM_CALL_CMD_ERR_SRV_TYPE_S;
      cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
      return;
  }
  #endif

  /*-----------------------------------------------------------------------*/

  /*
  ** This function will be called on two occassions.....  When there is an
  ** origination and again when call control is complete.  First, we check
  ** which case it is .....
  ** If this is the first time CM has seen this origination, it will call
  ** cm_nc_call_control_processing and then exit this function (deallocating
  ** the cmd_ptr as well)....  Once call control is complete, the client will
  ** call a CM callback function which will requeue another origination
  ** (CM_CALL_CMD_CC_COMPLETE_REORIG).  Then CM will pick up and continue
  ** with the origination process.
  */

 

  /* Check if call origination parameters (GW PS, GW CS, CDMA) have
  ** been supplied by the client. If origination parameters are missing
  ** intialize the fields with default values
  */
  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

  if (cmd_info_ptr->is_cdma_info_present == FALSE)
  {
    /* Init cs call origination parameters */
    cmcall_cmd_info_cdma_init (call_cmd_ptr);
  }

  srv_opt = cmd_info_ptr->cdma_info.srv_opt;

  #endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */


  #if defined(CM_GW_SUPPORTED)
  if (cmd_info_ptr->call_type == CM_CALL_TYPE_PS_DATA)
  {
    if (cmd_info_ptr->is_gsm_wcdma_ps_info_present == FALSE)
    {
      /* Init ps data call origination parameters */
      cmcall_cmd_info_gw_ps_init (call_cmd_ptr);
    }
  }
  else
  #endif
  {
    #if !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE) && defined(FEATURE_MMODE_3GPP) && !defined(FEATURE_MMODE_LTE_ONLY) && !defined(FEATURE_MMODE_GPRS_LTE)
    if (cmd_info_ptr->is_gsm_wcdma_cs_info_present == FALSE)
    {
      /* Init cs call origination parameters */
      cmcall_cmd_info_gw_voice_init (call_cmd_ptr);
    }
    #endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */
  }

  #ifdef FEATURE_MMODE_SC_SGLTE
  if (cmph_is_subs_feature_mode_sglte(call_ptr->asubs_id))
  {
    cm_ss_e_type domain_ss;
    cmd_err
      = cmcall_sglte_determine_ss_per_call_type(asubs_id, cmd_info_ptr->call_type, &domain_ss);
    if (cmd_err != CM_CALL_CMD_ERR_NOERR)
    {
      SYS_ARG_NOT_USED(domain_ss);
      cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
      return;
    }
    else
    {
      call_ptr->ss = ss = domain_ss;
    }
  }
  #endif
  
  /*----------------------------------------------------------------------*/
  /* For an emergency call, check the domain.
  ** if call should be made over HYBR2 stack, allow the hybrid prefs
  ** to remain unchanged
  */
#ifndef FEATURE_MMODE_EMC_SUPPORT
  if( cmd_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY  )
  {
    /* Block Emergency call if no support */
    cmd_err =  CM_CALL_CMD_ERR_SRV_TYPE_S;
    cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
    return;
  }
#endif
  /*----------------------------------------------------------------------*/

  /* First determine which modes we can originate on */

  /* This function will write CM_MODE_PREF_NONE into favored_mode_pref
     if it was not able to compute it */
  if (!cmcall_set_favored_mode_pref_for_origination ( ss,
                                                      cmd_info_ptr->call_type,
                                                      cmd_info_ptr->srv_type,
                                                      #ifdef FEATURE_IS2000
                                                      cmd_info_ptr->cdma_info.drs_bit,
                                                      #else
                                                      TRUE,
                                                      #endif
                                                      srv_opt,
                                                      &cmd_info_ptr->favored_mode_pref ))
  {
    cmd_err = CM_CALL_CMD_ERR_FAVORED_MODE_PREF_FOR_ORIGINATION;
    cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
    return;
  }


  CM_MSG_HIGH_0("Fav mode pref as computed:");
  cmdbg_print_fav_mode_pref_for_origination( call_ptr->asubs_id,
                                             cmd_info_ptr->call_type,
                                             cmd_info_ptr->srv_type,
                                             CMPH_SS_MODE_PREF(asubs_id, ph_ptr),
                                             cmd_info_ptr->favored_mode_pref );

  /*-----------------------------------------------------------------------*/
#ifndef FEATURE_CM_DISABLE_IMS_EXT  
  if(call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
  {
    cmcall_ext_e911_clean_up(call_ptr->asubs_id);
  }
#endif
  /* Only able to originate if the phone/PRL supports the
  ** originating mode preference.
  */
  if( !cmph_is_valid_call_mode_pref_on_ss(ss, cmd_info_ptr->favored_mode_pref, (cm_cmd_type*)call_cmd_ptr) )
  {
    call_cmd_ptr->cmd = CM_CALL_CMD_ORIG;
    cmd_err =  CM_CALL_CMD_ERR_SRV_TYPE_S;
    cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
    return;
  }

  /*-----------------------------------------------------------------------*/
  #if defined(FEATURE_MMODE_GW) && !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
  /*
  ** Call originations from Diag will have bearer capability
  ** populated by CM
  */
  if (call_cmd_ptr->client_id == CM_CLIENT_ID_ANONYMOUS)
  {
    if (cmd_info_ptr->is_gsm_wcdma_cs_info_present == TRUE)
    {
      cm_util_set_default_voice_bearer_cap_per_sub (
        &(cmd_info_ptr->cm_gw_mode_info_u.gsm_wcdma_info.bearer_capability_1),
        call_cmd_ptr->cmd_subs);
    }
  }
  #endif

  /*-----------------------------------------------------------------------*/

  mode_usage = cmcall_determine_mode_usage (call_cmd_ptr);

  /*-----------------------------------------------------------------------*/

  if (call_cmd_ptr->cmd == CM_CALL_CMD_ORIG)
  {
    /* Save favored mode prefs.
    */
    call_ptr->favored_mode_pref = cmd_info_ptr->favored_mode_pref;
#ifdef FEATURE_MODEM_STATISTICS
    /* Increment the number of times user made a call */
    cmstats_increment_send_key_press_count();
    /* Update the stats */
    cmstats_update_stats_and_post_event( SYS_CM_CALL_STAT_BIT_MASK );
#endif
    /* Pulled out of cmwcall_client_cmd_proc(). trans_id and nsapi are tied
    ** to connection id and are needed by Call control.
    **
    ** Return cmd_err only if there are no CDMA origination parameters and
    ** connection id cannot be obtained.
    */

    /* Only when GW will be used for the call, reserve connection_id */
    if ((mode_usage == CM_CC_MODE_PREF_GW_ONLY) ||
        (mode_usage == CM_CC_MODE_PREF_ANY))
    {
      if (!BETWEEN((int8)call_ptr->gw_connection_id, -1, (int8)UNKNOWN_CONNECTION_ID))
      {
        if (!cmcall_generate_connection_id(call_ptr))
        {
          CM_ERR_1("ORIG: Can not set connection Id Info",call_ptr->call_id);

          cmcall_client_cmd_err_deallocate (call_cmd_ptr, CM_CALL_CMD_ERR_CONNECTION_ID_OUT_L);
          return;
        }
      }
    }

    /*-------------------------------------------------------------------*/

    /*
    ** cmd_info_ptr->num.subaddr will be used by call control module, hence
    ** updating the values in cmd_info_ptr->num.subaddr.
    */

    #ifdef CM_GW_SUPPORTED
    if(!(cmd_info_ptr->num.subaddr.num_char > 0))
    {
      if(cmd_info_ptr->cm_gw_mode_info_u.gsm_wcdma_info.called_party_subaddress.present
                                                                    == TRUE)
      {
        cmcall_unpack_subaddress(
          (&(cmd_info_ptr->num.subaddr)),
          /*lint -e{826} */
          ((gstk_subaddress_type *)&(cmd_info_ptr->cm_gw_mode_info_u.gsm_wcdma_info.called_party_subaddress.length)));
      }
      else
      {
        cmd_info_ptr->num.subaddr.num_char = 0;
      }
    }

    /*
    ** This subaddress value will be used if cc is results with allowed
    ** no modification on GW side
    */
    if(cmd_info_ptr->num.subaddr.num_char >0)
    {
      call_ptr->cmcall_mode_info.info.gsm_wcdma_info.called_party_subaddress.present
                                                                    = TRUE;
      cmcall_pack_subaddress(
          /*lint -e{826} */
         (gstk_subaddress_type *) &(call_ptr->cmcall_mode_info.info.gsm_wcdma_info.called_party_subaddress.length),
         &(cmd_info_ptr->num.subaddr));
    }
    else
    {
      call_ptr->cmcall_mode_info.info.gsm_wcdma_info.called_party_subaddress.present
                                                                    = FALSE;
    }

    #endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

    /*--------------------------------------------------------------------*/

    /*
    ** This subaddress value will be used if cc is results with allowed
    ** no modification on 1x side
    */
    if(cmd_info_ptr->num.subaddr.num_char >0)
    {
      *(&(call_ptr->num.subaddr)) = *(&(cmd_info_ptr->num.subaddr));
    }
    else
    {
      call_ptr->num.subaddr.num_char = 0;
    }

    /*--------------------------------------------------------------------*/

    /* Check if we need to do any call control */
    CM_MSG_HIGH_1 ("is cc proc complete:%d",
                 call_cmd_ptr->info.call_control_processing_complete);

    if (!call_cmd_ptr->info.call_control_processing_complete)
    {

      /* if we can't do automatic mode, no use have call control do any
         processing it need not do.
      */
      
      #if defined(FEATURE_MMODE_3GPP2)
      if (mode_usage == CM_CC_MODE_PREF_CDMA_ONLY)
      {
        cdma_subscription_source = cmph_get_cdma_subscription_source();
        gw_subscription_source = CM_CC_SUBSCRIPTION_SOURCE_NONE;
      }
      else
      #endif
      if (mode_usage == CM_CC_MODE_PREF_GW_ONLY)
      {
        cdma_subscription_source = CM_CC_SUBSCRIPTION_SOURCE_NONE;
        gw_subscription_source = cmph_get_gw_subscription_source (ss);
      }
      else
      {
        #if defined(FEATURE_MMODE_3GPP2)
        cdma_subscription_source = cmph_get_cdma_subscription_source ();
        #else
        cdma_subscription_source = CM_CC_SUBSCRIPTION_SOURCE_NONE;
        #endif
        gw_subscription_source = cmph_get_gw_subscription_source (ss);
      }

      CM_MSG_HIGH_3("subscription source cdma %d, gw %d, mode_usage %d",
                   cdma_subscription_source,
                   gw_subscription_source, mode_usage);

      cm_nc_status = cm_nc_call_control_processing
      (
        cmd_info_ptr->call_id,
        cdma_subscription_source,
        gw_subscription_source,
        cmd_info_ptr->call_type,
        &call_cmd_ptr->info.num,
        (void*) call_cmd_ptr,
        cmcall_call_control_complete_cb,
        NULL,
        &(cmd_info_ptr->cc_generic_params)
      );

      if (cm_nc_status == TRUE)
      {
        call_ptr->call_state = CM_CALL_STATE_CC_IN_PROGRESS;
        CM_MSG_HIGH_0("ORIG: Starting Call Control Processing .... ");
      }
      else
      {
        
        /* Since Number classification returned false, it's upto CM to free
        ** the command buffer that was sent to it.
        */
        call_cmd_ptr->is_reused = FALSE;
        CM_MSG_HIGH_2("cm_nc_call_control_processing returns FALSE: cmd_ptr->is_reused %d, call_id %d",
          call_cmd_ptr->is_reused, cmd_info_ptr->call_id);

        cmd_err = CM_CALL_CMD_ERR_CALL_CONTROL_REJECTED;

        cmd_info_ptr->call_control_cmd_err = CM_CALL_CMD_ERR_CALL_CONTROL_REJECTED;
        cmd_info_ptr->error_cause = CM_CALL_CMD_ERR_CAUSE_OTHER;

        cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
        /*lint -restore */
      }

      return;
    }

    #ifdef FEATURE_MMODE_3GPP
    call_ptr->gw_cc_modified_info.call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;
    #endif
    #if 0
    call_ptr->cdma_cc_modified_info.call_control_result = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;
    #endif  
  }
  else if (call_cmd_ptr->cmd == CM_CALL_CMD_CC_COMPLETE_REORIG)
  {
    /* Reset the cmd to be the original origination command.
    ** If an error should occur, CM should not send out the cmd as a REORIG.
    */
    call_cmd_ptr->cmd = CM_CALL_CMD_ORIG;
    #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
    if(call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
    {
      mode_usage = CM_CC_MODE_PREF_ANY;
    }
    #endif

    /* If call is not allowed on either modes,
    ** Or if the call is not allowed on the one technology that is available for use.....
    ** then the call is effectively rejected by call control.
    */
    if ((call_ptr->gw_cc_modified_info.call_control_result ==
                           CM_CC_RESULT_NOT_ALLOWED)
                                   ||
        ((call_ptr->gw_cc_modified_info.call_control_result ==
                           CM_CC_RESULT_NOT_ALLOWED) &&
         (mode_usage == CM_CC_MODE_PREF_GW_ONLY))
       )
    {
      cmd_info_ptr->call_control_cmd_err = CM_CALL_CMD_ERR_CALL_CONTROL_REJECTED;
      cmd_info_ptr->error_cause = CM_CALL_CMD_ERR_CAUSE_NO_SUBSC;
      /* If call is rejected because of a reason other than Subscription,
      ** indicate that cause here
      */
      #ifdef FEATURE_MMODE_3GPP
      if(call_ptr->gw_cc_modified_info.error_cause !=
                            CM_CALL_CMD_ERR_CAUSE_NO_SUBSC)
      {
        cmd_info_ptr->error_cause = call_ptr->gw_cc_modified_info.error_cause;
      }
      #endif
    }

    /* If the call is rejected by call control, no need to proceed,
    ** stop the origination right away and return an error.
    */
    if (cmd_info_ptr->call_control_cmd_err == CM_CALL_CMD_ERR_CALL_CONTROL_REJECTED)
    {
      cmd_err = cmd_info_ptr->call_control_cmd_err;
      CM_MSG_HIGH_1("ORIG: CC Rejected %d", cmd_info_ptr->call_id);

      /* Make sure the connection ids are unreserved.
      ** Clean up gets done in cmcall_client_cmd_err_deallocate ().
      */

      cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
      return;
    }


    /* Process the call control results.
     */
    mode_usage= cm_nc_proc_call_control_result
    (
    #if 0
      call_ptr->cdma_cc_modified_info.call_control_result == CM_CC_RESULT_NOT_ALLOWED ? NULL: &call_ptr->cdma_cc_modified_info,
    #endif
      #ifdef FEATURE_MMODE_3GPP
      call_ptr->gw_cc_modified_info.call_control_result == CM_CC_RESULT_NOT_ALLOWED ? NULL: &call_ptr->gw_cc_modified_info,
      #else
      NULL,
      #endif
      &call_cmd_ptr->info.num,
      cmd_info_ptr->call_type,
      ss_ptr->info.sys_mode
    );
    CM_MSG_HIGH_1("CC results return mode_usage=%d", mode_usage);

    /* Return error if mode_usage returned is none/mismatched
      */

    if(mode_usage == CM_CC_MODE_PREF_NONE)
    {
      cmd_err =  CM_CALL_CMD_ERR_MODE_PREF_P;
      cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
      return;
    }

    #ifdef FEATURE_MMODE_DUAL_SIM
    cm_pm_cdma_call_poll = POLICYMAN_SUCCEEDED(policyman_get_boolean_value("cm:1xdata_allowed", &cdma_data_call_allowed));

    if( cm_pm_cdma_call_poll == TRUE
	    && call_ptr->asubs_id == SYS_MODEM_AS_ID_1
	    && cmph_is_dsds()
	    && ph_ptr->dds_switch_type == DDS_CAUSE_PERMANENT
	    && cdma_data_call_allowed == FALSE
	    && cmcall_misc_is_mode_pref( call_ptr->favored_mode_pref,
                                  CM_MODE_PREF_CDMA_ONLY )
	    && call_ptr->call_type == CM_CALL_TYPE_PS_DATA
	   )
	{
	  CM_MSG_HIGH_0("rejecting 1x call as per cmcc subsidized requirement");
	  cmd_err =  CM_CALL_CMD_ERR_OP_NOT_SUPPORTED_ON_SUBS;
	  cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
          return;
	}
    #endif
  
    #ifdef CM_GW_SUPPORTED
    /* Check for SS/USS modifications.
    ** Two cases CM needs to check for (1) 1x-Allowed GW-SS/USS
    **                                 (2) 1x-NotAllowed GW-SS/USS
    ** (1) If the NC preferences function still chose ANY, choose 1x only as CM
    **     will not be able to switch to SS/USS mid-origination.
    ** (2) End the call and switch over to a SS/USS command.
    */
    if ((call_ptr->gw_cc_modified_info.call_control_result ==
                                         CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_SS) ||
        (call_ptr->gw_cc_modified_info.call_control_result ==
                                         CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_USSD))
    {
      cm_mode_pref_e_type    mode_pref;
      switch( mode_usage )
      {
        case CM_CC_MODE_PREF_ANY:
          mode_pref = CM_MODE_PREF_AUTOMATIC;
          break;

        case CM_CC_MODE_PREF_CDMA_ONLY:
          mode_pref = CM_MODE_PREF_CDMA_ONLY;
          break;

        case CM_CC_MODE_PREF_GW_ONLY:
          mode_pref = CM_MODE_PREF_TDS_GSM_WCDMA;
          break;

        default:
          mode_pref = CM_MODE_PREF_NONE;
          break;
      }
#if 0
      if ((cmcall_misc_is_mode_pref(mode_pref, CM_MODE_PREF_CDMA_ONLY)) &&
          (call_ptr->cdma_cc_modified_info.call_control_result != CM_CC_RESULT_NOT_ALLOWED))
      {
        mode_usage = CM_CC_MODE_PREF_CDMA_ONLY;
      }
      else
 #endif       
      {
        cmcall_client_cmd_err( call_cmd_ptr, CM_CALL_CMD_ERR_NOERR );
        #if defined(FEATURE_MMODE_SS_SUPS) || defined(FEATURE_MMODE_USS_SUPS)
        if (!cmcall_stop_orig_start_sups( ss, call_ptr, (cm_cmd_type*)call_cmd_ptr ))
        {
          CM_ERR_0( "Unable to start SUPS operations!");
        }
        #endif
        return;
      }
    }

     /* When call control modifies a normal voice call into an emergency
    ** call emergency call service type also gets set.
    ** Would be better after call_type is assigned to EMERG but
    ** grouping it here in featurization.
    */
    #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
    if (cmd_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
    {
      /* Copy emergency service category from call control result
       */
      call_ptr->num.emerg_srv_categ = cmd_info_ptr->num.emerg_srv_categ;

      CM_MSG_HIGH_1 ("Emerg serv categ returned by CC = %d",
                                   call_ptr->num.emerg_srv_categ);
    }
    #endif
    #endif  /* FEATURE_WCDMA || FEATURE_GSM */

  }

  /* By this time CC is done and calltype (OTA/CDMA type) is resolved */

  // VoLTE is existing, check if there is ims callback. If not, reject the call
  // We have to check here after the call type is resolved after cc to avoid E911 got rejected

  /* If already VOLTE call is ongoing , but no IMS registration.
  ** in such cases , reject the new call .
  ** Also reject 1X voice calls, while another VOLTE call is ongoing
  */  
  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  if (cmcall_is_present_ip_call() != CM_CALL_ID_INVALID)
  {
    // Check if we have IMS callback
    if ((call_ptr->call_type == CM_CALL_TYPE_VOICE  ||
         call_ptr->call_type == CM_CALL_TYPE_VT ||
         call_ptr->call_type == CM_CALL_TYPE_VS) &&
         (!cmcall_is_active_ip_app(SYS_SYS_MODE_LTE, call_ptr->call_type) ||
         cmcall_misc_is_mode_pref(call_ptr->favored_mode_pref,CM_MODE_PREF_LTE_ONLY) == FALSE))         
    {
      CM_MSG_HIGH_0("IMS callback is not registered");

      cmd_err = CM_CALL_CMD_ERR_CALL_ORIG_FAIL_S;
      cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
      return;
    }
  }

  if( (( CMPH_SS_DISABLE_CALL_TYPE_MASK(asubs_id, ph_ptr) & CM_CALL_TYPE_VOICE_MASK ) &&
         ( call_ptr->call_type == CM_CALL_TYPE_VOICE ||
            call_ptr->call_type == CM_CALL_TYPE_EMERGENCY ) )
          ||
       ( ( CMPH_SS_DISABLE_CALL_TYPE_MASK(asubs_id, ph_ptr) & CM_CALL_TYPE_EMERG_MASK ) &&
           call_ptr->call_type == CM_CALL_TYPE_EMERGENCY )
    )
  {
      call_cmd_ptr->cmd = CM_CALL_CMD_ORIG;
      /* Block this MO voice call */
      cmd_err =  CM_CALL_CMD_ERR_SRV_TYPE_S;
      cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
      return;
  }
  else if( ( CMPH_SS_DISABLE_CALL_TYPE_MASK(asubs_id, ph_ptr) & CM_CALL_TYPE_CS_DATA_MASK ) &&
              ( call_ptr->call_type == CM_CALL_TYPE_CS_DATA )
    )
  {
      call_cmd_ptr->cmd = CM_CALL_CMD_ORIG;
      /* Block this CS Data call */
      cmd_err =  CM_CALL_CMD_ERR_SRV_TYPE_S;
      cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
      return;
  }
  #endif

   /*-----------------------------------------------------------------------*/

  /* Reset the call state */
  call_ptr->call_state      = CM_CALL_STATE_IDLE;
  call_ptr->call_subst.idle = CM_CALL_IDLE_SUBST_NONE;
  call_ptr->call_type       = cmd_info_ptr->call_type;

  #if defined(FEATURE_ALS)
  /*
  ** All MO voice calls go on currently selected line.
  ** For all other call types use the default line
  ** the call object was initialized with
  */
  if( call_ptr->call_type == CM_CALL_TYPE_VOICE ||
      call_ptr->call_type == CM_CALL_TYPE_EMERGENCY
    )
  {
    call_ptr->line = cmph_ptr()->line;
  }
  #endif

  #ifdef FEATURE_MMODE_EMC_SUPPORT
  /* If the call type is an emergency number,
  ** we should use any mode
  */
  if (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
  {
  #ifdef FEATURE_MMODE_3GPP
    mode_usage = CM_CC_MODE_PREF_ANY;
  #endif
  }
  #endif

  /*-----------------------------------------------------------------------*/
    if (call_ptr->ss == CM_SS_NONE)
  {
    cmd_err = CM_CALL_CMD_ERR_SRV_STATE_S;
    CM_MSG_HIGH_2 ("Call type %d cannot be placed, err %d",
                  call_ptr->call_type, cmd_err);
    cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
    return;
  }

  /*-----------------------------------------------------------------------*/

  cmd_err = cmcall_cmd_orig_para_check(call_cmd_ptr);

  /*-----------------------------------------------------------------------*/

  if (cmd_err != CM_CALL_CMD_ERR_NOERR)
  {
    CM_MSG_HIGH_1("call orig cmd err = %d",cmd_err);
    cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
    return;
  }

  /*-----------------------------------------------------------------------*/

  /* Copy cmd into call object.
  */

  cmcall_cmd_copy_fields( call_ptr, call_cmd_ptr );

  /*-----------------------------------------------------------------------*/

  /* Copy the current serving system status
  */

  cmcall_copy_ss_info( call_ptr );

  /*-----------------------------------------------------------------------*/

  /*
  ** If there is an active app reg with WLAN, then as per IMS requirement,
  ** emergency calls over WLAN will also have their call_type as CM_CALL_TYPE_VOICE
  ** As of now, CM_CALL_TYPE_VOICE is only registered with IMS, so checking for VOICE only.
  */
  /* If emcall_to_voicecall is enabled, 
  ** then emergency call can be sent as normal voice call over WLAN
  */
  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  #if 0
  if (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY  &&
      cmcall_is_active_ip_app(SYS_SYS_MODE_WLAN, CM_CALL_TYPE_VOICE)
#if defined (FEATURE_IP_CALL) && defined (FEATURE_CM_LTE)
      && (cmph_get_sub_config_ptr(asubs_id)->emcall_to_voicecall)
#endif
  )
  {
    call_ptr->call_type = CM_CALL_TYPE_VOICE;
  }
  #endif
  /*
   ** If service type is AUTOMATIC and call_type is VT with no IPAPP registered, change call_type to VOICE
   */
  if (call_ptr->call_type == CM_CALL_TYPE_VT  &&
      cmd_info_ptr->srv_type == CM_SRV_TYPE_AUTOMATIC &&
      !cmcall_is_active_ip_app(SYS_SYS_MODE_LTE, CM_CALL_TYPE_VT) &&
      !cmcall_is_active_ip_app(SYS_SYS_MODE_WLAN, CM_CALL_TYPE_VT) &&
      !cmcall_is_active_ip_app(SYS_SYS_MODE_WCDMA, CM_CALL_TYPE_VT))
  {
    call_ptr->call_type = CM_CALL_TYPE_VOICE;
  }

  /*
  ** Force top pref is set to TRUE, if the current call is possible over IP
  ** If there is no ipapp (on HDR )for voice origination, remove HDR
  ** component from favored mode preference.
  ** Both these steps are not needed for emergency call over EHRPD case.
  ** Hybr pref will not be OFF during 911 call only for emerg call over ehrpd case.
  */
  if(! ( call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
    )
  {
    cmcall_process_ipcall_params (call_ptr);
  }
  #endif

  #ifdef FEATURE_MMODE_SC_SVLTE

  if( cmcall_is_route_hybr2_volte_call_to_1x(call_ptr) )
  {
    CM_MSG_HIGH_0("1xSXLTE config, no LTE service, 1x service present");
    call_ptr->is_volte = FALSE;
    call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
    								   call_ptr->favored_mode_pref,
    								   1,
    								   SD_SS_MODE_PREF_LTE
    								   );
    if (call_ptr->asubs_id == SYS_MODEM_AS_ID_1)
    {
      call_ptr->ss = CM_SS_MAIN;
    }
  }
  #endif

  /*-----------------------------------------------------------------------*/
  /* Check if the call passes SSAC checking.  If not, reject the call right away.
  ** The SSAC checking Only applies to MMTEL voice/video over LTE.
  */
  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE  
  ssac_chk_result = cmcall_is_call_allowed_per_ssac(call_ptr);
  CM_MSG_HIGH_1("ssac_chk_result=%d",ssac_chk_result);
  if(ssac_chk_result != SSAC_CHK_WAIT && 
     cmph_is_lte_capable_on_sub(call_ptr->asubs_id))
  {
    cmcall_reset_ssac_delay_timer();
  }
  if (ssac_chk_result == SSAC_CHK_FAIL)
  {
    if(cmph_get_volte_sub_info_ptr(call_ptr->asubs_id)->volte_ssac_sr_control &&
         call_ptr->call_type == CM_CALL_TYPE_VOICE)
    {
      CM_MSG_HIGH_0("call orig over CS upon ssac fail");
      call_ptr->is_volte = FALSE;
      call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                         call_ptr->favored_mode_pref,
                                         1,
                                         SD_SS_MODE_PREF_HDR
                                         );
      if (call_ptr->asubs_id == SYS_MODEM_AS_ID_1)
      {
        call_ptr->ss = CM_SS_MAIN;
      }
      
     #if (defined( FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X))
      /*
          With SSAC redial feature, CM can retry call over CS after VOLTE/VT call is SSAC barred. 
          In this case IMS is not aware of the call yet and it still keeps registration. 
          Since SSAC barred VOLTE call can still go through CSFB, 
          we need calculate CSFB type without checking IMS reg status. 
          To minimize the impact, we can compute CSFB without checking IMS reg status after SSAC decided to redial over CS*/
      call_ptr->csfb_type = cmsds_get_csfb_type_allowed(call_ptr->asubs_id);
      CM_MSG_HIGH_2("SSAC fail: CSFB :csfb_type = %d,fav mode_pref=%d ",call_ptr->csfb_type,call_ptr->favored_mode_pref);
	  #endif
    }
    else
    {
      cmcall_client_cmd_err_deallocate (call_cmd_ptr, CM_CALL_CMD_ERR_SSAC_REJECT);
      return;
    }

  }
  else if(ssac_chk_result == SSAC_CHK_WAIT)
  {
    call_cmd_ptr->is_reused = TRUE;
    cmcall_ssac_ptr()->call_cmd_ptr = call_cmd_ptr;
    return;
  }
  else if (cmph_is_lte_capable_on_sub(call_ptr->asubs_id))
  {
    cmcall_reset_ssac_timer(cmcall_get_ssac_call_type(call_ptr),SSAC_TIMER_ACTION_STOP);
  }
  #endif

  /*CM_MSG_HIGH_2("Eval CS/IP: csfb_type=%d, ip_emg_poss=%d",
               call_ptr->csfb_type,
               cmsds_is_current_sys_lte_ims_emergency_capable(call_ptr->asubs_id));*/

  /* Run domain selection */
  #if (defined( FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X))
  /* Need to recompute csfb if csfb_type is NONE  i.e not already computed before */
  /* don't recompute csfb_type for emergency call. Please rely on emergency call
  ** domain selection in cmsds.
   */

  if(call_ptr->csfb_type == CM_CALL_CSFB_TYPE_NONE
     && !(call_ptr->is_volte||call_ptr->is_vowlan||call_ptr->is_ip_call)
     && call_ptr->cmcall_mode_info.info_type != CM_CALL_MODE_INFO_IP
     && call_ptr->call_type != CM_CALL_TYPE_EMERGENCY
    )
  {
    call_ptr->csfb_type = cmcall_process_dom_sel_params(call_ptr);
  }


  /* Force mode_usage to Any for 1xcsfb call if gw info is not present in the call
   ** orig command - this is to support making 1xcsfb call from QXDM
   */
  if (call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB
      && mode_usage == CM_CC_MODE_PREF_CDMA_ONLY
      && !cmd_info_ptr->is_gsm_wcdma_ps_info_present
      && !cmd_info_ptr->is_gsm_wcdma_cs_info_present)
  {
    mode_usage = CM_CC_MODE_PREF_ANY;
  }
  #endif

    /* If emergency call over wifi is enabled,
     1) is_vowlan flag needs to be set depending on the app availability
     2) timer would be started if ecmp is 3gpp
     3) top_ptr to be set
     4) As per ECMP, first call attempt shall be decided
  */   
  #if defined (FEATURE_IP_CALL) && defined (FEATURE_CM_LTE) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
  
  CM_MSG_HIGH_1("Eval CS/IP: ip_poss=%d", cmcall_is_ipcall_possible(call_ptr));  
  #if 0
  if(cmph_get_sub_config_ptr(asubs_id)->wifi_e911_support &&
       call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
  {
    cmcall_process_custom_wifi_settings(call_ptr);
  }
  else if(cmph_get_sub_config_ptr(call_ptr->asubs_id)->is_wlan_e911_call_supp && call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
  {
    cmcall_process_wlan_e911_settings(call_ptr);
  }
  #endif
  #endif

  CM_MSG_HIGH_2("mode_usage=%d, call_ptr->ss=%d",mode_usage, call_ptr->ss);

  cmd_err = cmcall_process_orig_mode(mode_usage,call_ptr);

  /*-----------------------------------------------------------------------*/


  if (cmd_err != CM_CALL_CMD_ERR_NOERR)
  {
    cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_err);
  }
  else
  {
    /*---------------------------------------------------------------------*/
  
    /* No errors, we can try to originate the call now */
    /* Always notify client of command error code
    ** (even if command is successful).
    */

    /* End VOLTE call during E911 Origination */
    #ifdef FEATURE_MMODE_EMC_SUPPORT
    if(call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
    {
      cmcall_end_concurrent_volte_call(call_ptr->asubs_id);
    }
    #endif

    if(call_ptr->cc_object_ptr != NULL)
    {
      /*Command will be reused for posting reorig phase_2*/
      call_cmd_ptr->is_reused = TRUE;
       /* Storing cmd_ptr address to post after 2nd stage call control*/
      call_ptr->cc_object_ptr->user_data = call_cmd_ptr;
      CM_MSG_HIGH_2 ("cmd_ptr->is_reused = %d, call_ptr->call_id = %d",call_cmd_ptr->is_reused, call_ptr->call_id);
    }
        else
    {
      /*Calling callback here, only if reoirg phase2 is not needed*/
      cmcall_client_cmd_err( call_cmd_ptr, cmd_err );
    }

    #if defined(FEATURE_MMODE_DUAL_SIM) && ( defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) || \
    defined(FEATURE_MMODE_CDMA) || defined(FEATURE_CM_LTE))
    /*
    **Removing WCDMA/TDSCDMA/LTE from mode pref, if other subs has WCDMA/TDSCDMA/LTE in its preference
    */
    if(call_ptr->call_type == CM_CALL_TYPE_EMERGENCY &&
       cmph_is_msim() && !cmph_is_sxlte())
    {
      if (!cmph_is_lte_capable_on_sub(call_ptr->asubs_id)
          && (ph_ptr->internal_standby_pref == SYS_MODEM_DS_PREF_DUAL_STANDBY|| 
              ph_ptr->internal_standby_pref == SYS_MODEM_DS_PREF_TRIPLE_STANDBY)
      )
      {
        call_ptr->favored_mode_pref = CM_MODE_PREF_GSM_ONLY;
      }
    }
    #endif

    /* If emergency call is originated, then delete the act priority = priority_end from
    ** priority queue on other subs.
    */
    #ifndef FEATURE_TRIM_CMPMPRX

    /* Notify cmpmprx that EMG call is originated
    */
    if (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
    {
      cmpmprx_notify_emg_call_start();
    }
    #endif


    cmcall_orig_start (call_ptr);


  }

}  /* cmcall_client_orig_proc () */

/*===========================================================================

FUNCTION cmcall_check_if_emer_call_allowed

DESCRIPTION

Validating if emergency call allowed.
chk if subsc not avaib on the sub 
where emergency call is received or sim state is PS_INVALID
if yes, then reject the emergency call 

NOTE: SGLTE/SVLTE will be OFF for this FR.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmcall_check_if_emer_call_allowed(

    sys_modem_as_id_e_type         cmd_subs
        /* Pointer to a CM command */
)
{
  #ifndef FEATURE_TRIM_CMPMPRX

  boolean                 is_call_allowed = TRUE;
  sys_sim_state_e_type    sim_state       = SYS_SIM_STATE_NONE;

  switch(cmd_subs)
  {
   case SYS_MODEM_AS_ID_1:
    if((!cmph_ptr()->is_gwl_subscription_available)||
         cmss_ptr()->info.sim_state == SYS_SIM_STATE_PS_INVALID ||
         cmss_ptr()->info.sim_state == SYS_SIM_STATE_CS_PS_INVALID)
    {
      is_call_allowed = FALSE;
      sim_state = cmss_ptr()->info.sim_state;
    }
    break;
   default:
      is_call_allowed = TRUE;
      break;
  }
  
  CM_MSG_HIGH_3("E911_LTE_ONLY: is_call_allowed=%d, sim_state=%d, asub_id=%d",
                is_call_allowed,sim_state,cmd_subs);
  return is_call_allowed;
  #else
  return FALSE;
  #endif
  
}
#endif

#ifdef FEATURE_MMODE_DUAL_SIM
/*===========================================================================

FUNCTION cmcall_check_and_calculate_subs

DESCRIPTION

Calculating subs for call origination, if not specified in orig request.


DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static cm_call_cmd_err_e_type cmcall_check_and_calculate_subs(

    cm_call_cmd_s_type          *call_cmd_ptr
        /* Pointer to a CM command */
)
{

  cmph_s_type                 *ph_ptr        = cmph_ptr();
  sys_modem_as_id_e_type       active_subs = SYS_MODEM_AS_ID_1;

  CM_ASSERT( call_cmd_ptr           != NULL );

  CM_ASSERT( call_cmd_ptr->cmd_type == CM_CMD_TYPE_CALL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether command is possible.
  */
  if(ph_ptr->internal_standby_pref == SYS_MODEM_DS_PREF_SINGLE_STANDBY)
  {
    switch(ph_ptr->active_subs)
    {
      case SYS_MODEM_AS_ID_2_MASK:
        active_subs = SYS_MODEM_AS_ID_2;
        break;

	  #ifdef FEATURE_MMODE_TRIPLE_SIM
      case SYS_MODEM_AS_ID_3_MASK:
        active_subs = SYS_MODEM_AS_ID_3;
        break;
	  #endif

      default:
        active_subs = SYS_MODEM_AS_ID_1;
    }
  }
  else
  {
    /* Picking any sub which is active, will be used only when expected sub is inactive*/
    if(ph_ptr->active_subs & SYS_MODEM_AS_ID_1_MASK)
    {
      active_subs = SYS_MODEM_AS_ID_1;
    }
    else if(ph_ptr->active_subs & SYS_MODEM_AS_ID_2_MASK)
    {
      active_subs = SYS_MODEM_AS_ID_2;
    }
	#ifdef FEATURE_MMODE_TRIPLE_SIM
    else
    {
      active_subs = SYS_MODEM_AS_ID_3;
    }
	#endif
  }

 
  if(call_cmd_ptr->cmd_subs != SYS_MODEM_AS_ID_NONE)
  {
    if(!(ph_ptr->active_subs & BM(call_cmd_ptr->cmd_subs)))
    {
      return CM_CALL_CMD_ERR_OP_NOT_SUPPORTED_ON_SUBS;
    }

   if(( call_cmd_ptr->info.call_type == CM_CALL_TYPE_PS_DATA ||
         call_cmd_ptr->info.call_type == CM_CALL_TYPE_PS_DATA_IS707B )
        && call_cmd_ptr->cmd_subs != ph_ptr->default_data_subs
        && ph_ptr->active_subs & BM(ph_ptr->default_data_subs)
		&& (cmpmprx_get_num_of_active_data() != 2)      
       )
    {
      return CM_CALL_CMD_ERR_OP_NOT_SUPPORTED_ON_SUBS;
    }
  }

  switch( call_cmd_ptr->cmd )
  {
    case CM_CALL_CMD_ORIG:

      if( ph_ptr->internal_standby_pref == SYS_MODEM_DS_PREF_SINGLE_STANDBY )
      {
        call_cmd_ptr->cmd_subs        = active_subs;
      }
      else if(call_cmd_ptr->cmd_subs == SYS_MODEM_AS_ID_NONE)
      {
          if(( call_cmd_ptr->info.call_type == CM_CALL_TYPE_PS_DATA ||
             call_cmd_ptr->info.call_type == CM_CALL_TYPE_PS_DATA_IS707B)
           && (ph_ptr->active_subs & BM(ph_ptr->default_data_subs)) )
          {
            call_cmd_ptr->cmd_subs      = ph_ptr->default_data_subs;
          }
          else if(ph_ptr->active_subs & BM(ph_ptr->default_voice_subs))
          {
            call_cmd_ptr->cmd_subs      = ph_ptr->default_voice_subs;
          }
          else
          {
            call_cmd_ptr->cmd_subs        = active_subs;
          }
      }
      break;

    case CM_CALL_CMD_RAB_REESTAB:
    case CM_CALL_CMD_PS_DATA_AVAILABLE:
    case CM_CALL_CMD_PS_SIG_REL:
      if( ph_ptr->internal_standby_pref == SYS_MODEM_DS_PREF_SINGLE_STANDBY )
      {
        call_cmd_ptr->cmd_subs    = active_subs;
      }
      else if( call_cmd_ptr->cmd_subs == SYS_MODEM_AS_ID_NONE )
      {
          if(ph_ptr->active_subs & BM(ph_ptr->default_data_subs))
          {
            call_cmd_ptr->cmd_subs      = ph_ptr->default_data_subs;
          }
          else
          {
            call_cmd_ptr->cmd_subs        = active_subs;
          }
      }
      break;

    #if defined (FEATURE_MMODE_CALL_SUPS) 
    case CM_CALL_CMD_SUPS:
      if( ph_ptr->internal_standby_pref != SYS_MODEM_DS_PREF_DUAL_STANDBY
       #ifdef FEATURE_MMODE_TRIPLE_SIM
        && (ph_ptr->internal_standby_pref != SYS_MODEM_DS_PREF_TRIPLE_STANDBY)
       #endif
       )
      {
        call_cmd_ptr->cmd_subs      = active_subs;
      }
      else if( call_cmd_ptr->cmd_subs == SYS_MODEM_AS_ID_NONE )
      {
          if(ph_ptr->active_subs & BM(ph_ptr->default_voice_subs))
          {
            call_cmd_ptr->cmd_subs      = ph_ptr->default_voice_subs;
          }
          else
          {
            call_cmd_ptr->cmd_subs        = active_subs;
          }
      }
      break;
    #endif

    default:
      break;
  }

  return  CM_CALL_CMD_ERR_NOERR;

  }
#endif

#ifdef FEATURE_CMCC_SECURE_CALL
/*===========================================================================

FUNCTION  cmcall_get_secure_call_enabled

DESCRIPTION
  get secure call enabled by asubs_id

DEPENDENCIES
  None

RETURN VALUE
  secure call enabled

SIDE EFFECTS
  None.

===========================================================================*/
boolean cmcall_get_secure_call_enabled(
  sys_modem_as_id_e_type asubs_id,
  call_secure_info_s_type *cmcall_secure_info_ptr
)
{
  if(cmcall_secure_info_ptr == NULL)
  {
    return FALSE;
  }
  if(!(asubs_id >= SYS_MODEM_AS_ID_1 && asubs_id < MAX_SIMS
    && asubs_id < SYS_MODEM_AS_ID_NO_CHANGE))
  {
    asubs_id = SYS_MODEM_AS_ID_1;
  }
  return cmcall_secure_info_ptr->secure_call_enabled[asubs_id];
}
#endif

/*===========================================================================

FUNCTION  cmcall_client_cmd_check

DESCRIPTION

  Process commands that are generic, ie. processed in the same way no matter
  whether it's a GW or 1X call.

  Check for call command parameter errors and whether a specified call
  command is allowed in the current state of the call/phone.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if command is allowed in the current state
  of the call/phone, specific cm_call_cmd_err_e_type otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static cm_call_cmd_err_e_type  cmcall_client_cmd_check(

    cm_call_cmd_s_type          *call_cmd_ptr
        /* Pointer to a CM command */
)
{
  cm_call_cmd_err_e_type       cmd_err       = CM_CALL_CMD_ERR_NOERR;
    /* Command error (if any) */

  cmph_s_type                 *ph_ptr        = cmph_ptr();

  cm_mode_pref_e_type		 network_type;

  sys_modem_as_id_e_type  asubs_id = SYS_MODEM_AS_ID_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_cmd_ptr           != NULL );
  CM_ASSERT( call_cmd_ptr->cmd_type == CM_CMD_TYPE_CALL );

  /*If PSM is in enter_window, reject upper layer cmds*/
  if(call_cmd_ptr->cmd == CM_CALL_CMD_ORIG || call_cmd_ptr->cmd == CM_CALL_CMD_SUPS )
  {
    cm_psm_enter_req_status_e_type psm_status =  cmpsm_get_psm_status();
    if (psm_status == CM_PSM_ENTER_STATUS_WAIT_ENTER_RSP || 
        psm_status == CM_PSM_ENTER_STATUS_ENTER_IN_PROGRESS || 
        psm_status == CM_PSM_ENTER_STATUS_COMPLETE ||
        psm_status == CM_PSM_ENTER_STATUS_WAIT_MMGSDI_RSP)
        {
           CM_MSG_HIGH_1("Rejecting cmd because psm is in enter window :%d", psm_status);
           return CM_CALL_CMD_ERR_OTHER;
        }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   if(ph_ptr->main_stack_info.pref_info.network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_AUTO_CAMP_ONLY ||
      ph_ptr->main_stack_info.pref_info.network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL_CAMP_ONLY)
   {
     cmd_err  =   CM_CALL_CMD_ERR_INVALID_MODE_S;
     CM_MSG_HIGH_0 ("Call cmd not supported in CAMP Only mode ");
     return cmd_err;
   }
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH

   if(cmph_ptr()->is_gps_search_high_priority == TRUE )
   {
     CM_MSG_HIGH_0("Rejecting call due to high priority GPS search");
     cmd_err =  CM_CALL_CMD_ERR_IN_USE_S;
	 return cmd_err;
   }
#endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cmph_is_ssim())
  {
    asubs_id = SYS_MODEM_AS_ID_1;
  }
  else if (call_cmd_ptr->cmd_subs >= SYS_MODEM_AS_ID_1 &&
      call_cmd_ptr->cmd_subs < SYS_MODEM_AS_ID_NO_CHANGE)
  {
    asubs_id = call_cmd_ptr->cmd_subs;
  }

  #if defined(FEATURE_MMODE_DUAL_SIM)
  if(cmph_is_dsda() && asubs_id == SYS_MODEM_AS_ID_2)
  {
#ifdef FEATURE_MMODE_SXLTE_G
    if (cmph_is_sxlte())
    {
      network_type = ph_ptr->hybr_3_stack_info.pref_info.network_type;
    }
  	else
#endif
    {
      network_type = ph_ptr->hybr_2_stack_info.pref_info.network_type;
    }
  }
  else
  #endif
  {
    network_type = ph_ptr->main_stack_info.pref_info.network_type;
  }



  /* Check whether command is possible.
  */
  switch( call_cmd_ptr->cmd )
  {

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*------------------------------------*/
                     /* Get a snapshot of call information */
                     /*------------------------------------*/

    case CM_CALL_CMD_INFO_GET:

      /* Check command parameter errors
    */
      cmd_err = cmcall_call_cmd_get_call_info_para_check(call_cmd_ptr);
      break;

    case CM_CALL_CMD_INFO_LIST_GET:

      /*  Make sure the client is valid
      */
      if( ( call_cmd_ptr->client_ptr == CM_CLIENT_PTR_NONE ) ||
          ((call_cmd_ptr->client_ptr)->init_mask != CM_INIT_MASK ) )
      {
        cmd_err  = CM_CALL_CMD_ERR_CLIENT_ID_P;
      }
      break;
    #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
    case CM_CALL_CMD_ORIG:
	case CM_CALL_CMD_CC_COMPLETE_REORIG:

      if( call_cmd_ptr->info.srv_type == CM_SRV_TYPE_WLAN )
      {
         cmd_err =  CM_CALL_CMD_ERR_SRV_TYPE_P;
         break;
      }
      if ((ph_ptr->mm_nv_conn_mode_manual_search== TRUE) &&
          (cmph_is_valid_network_type_on_ss(cmph_map_subs_to_ss(call_cmd_ptr->cmd_subs), network_type)) &&
          ( call_cmd_ptr->info.call_type == CM_CALL_TYPE_PS_DATA))
      {
        CM_MSG_HIGH_0("Rejecting PS DATA orig due to ongoing plmn search");
        cmd_err =  CM_CALL_CMD_ERR_IN_USE_S;
        break;
      }
    #endif
    case CM_CALL_CMD_RAB_REESTAB:
    case CM_CALL_CMD_PS_DATA_AVAILABLE:
    case CM_CALL_CMD_PS_SIG_REL:
    case CM_CALL_CMD_SUPS:
        /* CM shall allow DATA_CALL origination while going OFFLINE for IMS deregistration purpose */	
        if( ph_ptr->is_oprt_mode_change_ongoing
            && call_cmd_ptr->cmd == CM_CALL_CMD_RAB_REESTAB
            && (!cmclnup_is_ongoing_subsc(BM(call_cmd_ptr->cmd_subs)))
          )
        {
            CM_MSG_HIGH_1 ("CALL CMD %d rej as oprt_mode_chg ongoing to LPM/OFFLINE", 
			               call_cmd_ptr->cmd);
            cmd_err = CM_CALL_CMD_ERR_OFFLINE_S;
            break;
        }

        #ifdef FEATURE_MMODE_DUAL_SIM
        if(cmph_is_msim())
        {
          cmd_err = cmcall_check_and_calculate_subs(call_cmd_ptr);

        #ifdef FEATURE_MMODE_DUAL_ACTIVE
        if(cmd_err == CM_CALL_CMD_ERR_NOERR)
        {
          /* If stack is suspeded due to emergency call on other subs, do not allow any command */
          if(cmph_is_dsda() &&
              cmph_ss_susp_due_to_emerg(cmph_map_subs_to_ss(call_cmd_ptr->cmd_subs)))
          {
            CM_MSG_HIGH_0 ("Cmd not allowed due to Emerg call on other subs");
            cmd_err = CM_CALL_CMD_ERR_IN_USE_S;
          }
        }
        #endif /* FEATURE_MMODE_DUAL_ACTIVE */
        }
        else
        #endif
        {
          call_cmd_ptr->cmd_subs = SYS_MODEM_AS_ID_1;
        }

       #if defined(FEATURE_MMODE_CALL_SUPS)
       if ( call_cmd_ptr->cmd == CM_CALL_CMD_SUPS &&
            cmd_err == CM_CALL_CMD_ERR_NOERR &&
            cmclnup_is_ongoing_subsc(BM(call_cmd_ptr->cmd_subs)) )
       {
         CM_MSG_HIGH_0 ("CLNUP in prog. Cmd not allowed");
         cmd_err = CM_CALL_CMD_ERR_OFFLINE_S;
       }
       #endif
	   
        break;

    #ifdef FEATURE_MMODE_DUAL_ACTIVE
    case CM_CALL_CMD_LOCAL_HOLD:
      if(cmcall_is_there_a_call_type(cmph_map_subs_to_ss(call_cmd_ptr->cmd_subs),
           CM_CALL_TYPE_EMERGENCY, CM_CALL_ID_INVALID)
                            != CM_CALL_ID_INVALID
         &&
         cmph_is_dsda())
      {
        /*If emergency call has to be put on local hold, reject*/
        cmd_err = CM_CALL_CMD_ERR_EMERGENCY_CALL_ACTIVE;
      }
      break;
    #endif

    case CM_CALL_CMD_PDN_CONNECTIVITY_REQ:
      #if defined(FEATURE_MMODE_DUAL_SIM)
      if (call_cmd_ptr->cmd_subs != ph_ptr->default_data_subs &&
           ph_ptr->active_subs & BM(ph_ptr->default_data_subs) &&
           cmpmprx_get_num_of_active_data() != 2)
      {
        CM_MSG_HIGH_0 ("Cmd not allowed");
        cmd_err = CM_CALL_CMD_ERR_OP_NOT_SUPPORTED_ON_SUBS;
      }
      break;
      #endif
    default:
      break;

  }

  return cmd_err;
} /* cmcall_client_cmd_check() */

#ifdef FEATURE_MMODE_DUAL_ACTIVE
/*==========================================================================

FUNCTION cmcall_cmd_local_hold_proc

DESCRIPTION
Process local call hold cmd, and push all active calls to local hold/unhold on that subs

DEPENDENCIES
  none.

RETURN VALUE
  none

SIDE EFFECTS
  none

==========================================================================*/
static void cmcall_cmd_local_hold_proc
(
  cm_call_cmd_s_type  *call_cmd_ptr
)
{
  #ifndef FEATURE_MMODE_DUAL_ACTIVE
  SYS_ARG_NOT_USED(call_cmd_ptr);
  return;

  #else
  cm_call_hold_state_e_type      call_hold_state =
                                         call_cmd_ptr->info.call_hold_state;
  cmph_s_type                     *ph_ptr = cmph_ptr();

  sys_modem_as_id_e_type         other_subs = (call_cmd_ptr->cmd_subs ==
                       SYS_MODEM_AS_ID_1)?SYS_MODEM_AS_ID_2:SYS_MODEM_AS_ID_1;

  cmcall_s_type     *call_ptr; /* Call object pointer */
  cm_iterator_type   call_itr; /* call Object Iterator */
  boolean            ims_call = FALSE;
  cm_call_id_type    active_ims_call_id;
  
  /*-----------------------------------------------------------------------*/
  
  if(call_cmd_ptr->cmd_cb_func != NULL)
  {
    call_cmd_ptr->cmd_cb_func( call_cmd_ptr->data_block_ptr,
                               call_cmd_ptr->cmd,
                               CM_CALL_CMD_ERR_NOERR );
  }

  /*-----------------------------------------------------------------------*/
  /* Call has to satisfy following conditions to be put on local hold
  ** loop through all IMS calls, if active call, send that call ID
  ** if no active call, send first ims call ID
  ** if no ims call, send cmcall event without waiting for ims cnf
  */
  cmcall_obj_iterator_init(&call_itr);
  call_ptr = cmcall_obj_get_next(&call_itr);
  /* Check if there is active and in convo IMS call*/
  while(call_ptr != NULL)
  {
    if(call_ptr->asubs_id == call_cmd_ptr->cmd_subs
      && (call_ptr->call_type == CM_CALL_TYPE_VT 
        || call_ptr->call_type == CM_CALL_TYPE_VOICE)
      && call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP
    )
    {
      /* Initialize call id to be the first IMS call*/
      if(!ims_call)
      {
        ims_call = TRUE;
        active_ims_call_id = call_ptr->call_id;
      }

      /* Update call id if call is active and in convo */
      if(call_ptr->call_state== CM_CALL_STATE_CONV
        && call_ptr->call_subst.conv== CM_CALL_CONV_CALL_ACTIVE)
      {
        active_ims_call_id = call_ptr->call_id;
      }

      /*update local hold state*/
      call_ptr->call_hold_state = call_hold_state;
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  }
  
  /*-----------------------------------------------------------------------*/
  /* CS call */
  if(!ims_call)
  {
    cmcall_obj_iterator_init(&call_itr);
    call_ptr = cmcall_obj_get_next(&call_itr);
  
    while(call_ptr != NULL)
    {
      if((call_cmd_ptr->cmd_subs == call_ptr->asubs_id)
          &&(call_hold_state != call_ptr->call_hold_state)
          && call_ptr->call_type == CM_CALL_TYPE_VOICE)
      {
        call_ptr->call_hold_state = call_hold_state;
        cmcall_event( call_ptr,CM_CALL_EVENT_LOCAL_HOLD );
      }

      call_ptr = cmcall_obj_get_next(&call_itr);
    }

    if(ph_ptr->curr_voice_subs == call_cmd_ptr->cmd_subs)
    {
      if(call_hold_state == CM_CALL_HOLD_STATE_LOCAL_HOLD)
      {
        ph_ptr->curr_voice_subs = other_subs;

        cmph_sp_subs_info_update();
      }
    }
    else
    {
      if(call_hold_state == CM_CALL_HOLD_STATE_ACTIVE)
      {
        ph_ptr->curr_voice_subs = call_cmd_ptr->cmd_subs;
        cmph_sp_subs_info_update();
      }

      #if !defined(FEATURE_MMODE_DISABLE_ACCESS_CONTROL) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
      if(cmph_is_dsda()
      && cmac_is_in_thermal_emergency_level2())
      {
        /* End all local hold calls */
        CM_MSG_HIGH_0("Ending all local held calls, due to thermal emergency");
        cmcall_endall_non_emerg_calls(CM_CALL_END_THERMAL_EMERGENCY, TRUE);
      }
	  #endif
    }
  }
  #ifdef FEATURE_IP_CALL
  else
  {
    if(ph_ptr->curr_voice_subs != call_cmd_ptr->cmd_subs
      && cmph_is_dsda()
      && cmac_is_in_thermal_emergency_level2())
    {
      CM_MSG_HIGH_0("Ending all local held calls, due to thermal emergency");
      cmcall_endall_non_emerg_calls(CM_CALL_END_THERMAL_EMERGENCY, TRUE);
    }
    /* Get the call pointer which points to the active IMS call
    ** if no active, then get the first IMS call.
    */
    call_ptr = cmcall_ptr ( active_ims_call_id );

    if(call_ptr != NULL)
    {
      /* IMS call: first indicate to IMS app. 
      ** cmcall event will be sent after receiving IMS cnf 
      */
      CM_MSG_HIGH_3("sending local hold/retrieve cmd with call id: %d, asubs_id: %d, call_hold_state=%d",
      		active_ims_call_id, call_ptr->asubs_id, call_hold_state);
      if(call_hold_state == CM_CALL_HOLD_STATE_LOCAL_HOLD)
      {
        cmipapp_send_local_hold_cmd(call_ptr->ipapp_id, active_ims_call_id, call_ptr->asubs_id);
      }
      else if(call_hold_state == CM_CALL_HOLD_STATE_ACTIVE)
      {
        cmipapp_send_local_retrieve_cmd(call_ptr->ipapp_id, active_ims_call_id, call_ptr->asubs_id);
      }
    }
    else
    {
      CM_ERR_0("call ptr is NULL, not sending local hold/resume cmd");
    }
  }
  #endif

  #endif /*FEATURE_MMODE_DUAL_ACTIVE*/
}
#endif
/*===========================================================================

FUNCTION cmcall_client_cmd_forward_ll

DESCRIPTION

  Process commands that are generic, ie. processed in the same way no matter
  whether it's a GW or 1X call.

  Forward a client call commands to lower layer.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE if CM needs to wait for a replay from MC before continuing
  processing this command, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmcall_client_cmd_forward_ll(

    const cm_call_cmd_s_type    *call_cmd_ptr
      /* Pointer to a CM command */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_cmd_ptr != NULL );
  CM_ASSERT( call_cmd_ptr->cmd_type == CM_CMD_TYPE_CALL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( call_cmd_ptr->cmd )
  {

                     /*------------------------------------*/
                     /* Get a snapshot of call information */
                     /*------------------------------------*/

    case CM_CALL_CMD_INFO_GET:

      /* Do nothing - this request terminated at CM
      */
      break;

    default:
      CM_ERR_1( "Bad call cmd = %d",(uint8) call_cmd_ptr->cmd);

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the caller of this function to whether
  ** the call object is now waiting for reply from MC.
  */
  return FALSE;

} /* cmcall_client_cmd_forward_ll() */


/*===========================================================================

FUNCTION cmcall_client_cmd_ll_reply

DESCRIPTION
  Process commands that are generic, ie. processed in the same way no matter
  whether it's a GW or 1X call.

  Process Lower Layer reply to clients call commands.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_client_cmd_ll_reply(

    const cm_call_cmd_s_type       *call_cmd_ptr,
        /* Pointer to a CM command */

    cm_call_cmd_err_e_type     cmd_err
        /* Indicate call command error */
)
{
  cmcall_s_type               *call_ptr      = NULL;
    /* Pointer to a call object */

  cm_mm_call_info_s_type      *call_info_ptr = NULL;
    /* Call information pointer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_cmd_ptr != NULL );
  CM_ASSERT( BETWEEN( cmd_err, CM_CALL_CMD_ERR_NONE, CM_CALL_CMD_ERR_MAX));
  CM_ASSERT( call_cmd_ptr->cmd_type == CM_CMD_TYPE_CALL );

  SYS_ARG_NOT_USED(cmd_err);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_2("START cmcall_client_cmd_ll_reply, cmd=%d, err=%d",
                  call_cmd_ptr->cmd, cmd_err );

  /* If necessary, copy specific command fields into call struct,
  ** update call state, and notify clients of event.
  */
  switch( call_cmd_ptr->cmd )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                   /*-----------------------------------------*/
                     /* Send call info to the requesting client */
                     /*-----------------------------------------*/

    case CM_CALL_CMD_INFO_GET:

      /*
      ** No need to copy any fields from command to call object
      ** Copy call state information into allocated call info buffer.
      */
      call_ptr = cmcall_ptr( call_cmd_ptr->info.call_id );

      if(call_ptr == NULL)
      {
        cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
        break;
      }

      /* Allocate and Initialize the info */
      call_info_ptr = cmcall_info_alloc();
      (void) cmcall_info_get( call_ptr, call_info_ptr );

      /* Notify only the requesting client
      */
      cm_client_call_event_ntfy(call_cmd_ptr->client_ptr,
                                CM_CALL_EVENT_INFO,
                                call_info_ptr );

      /* Deallocate the memory after sending the call event. */
      cm_mem_free(call_info_ptr);

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      default:
        CM_ERR_1( "Bad call command %d", (uint8) call_cmd_ptr->cmd);

  } /* switch */
} /* cmcall_client_cmd_ll_reply() */

/*===========================================================================

FUNCTION CMCALL_CLIENT_CMD_PROC

DESCRIPTION
  Dispatches to the right call cmd handler based on system operating mode

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_client_cmd_proc
(
  cm_call_cmd_s_type  *call_cmd_ptr                       /* pointer to a CM command */
)
{
  cm_call_cmd_info_s_type      *cmd_info_ptr = NULL;
  cm_call_cmd_err_e_type        call_cmd_err;
  boolean                       wait_for_reply;
  cm_call_mode_info_e_type      call_mode;
  cmcall_s_type                *call_ptr     = NULL;
  cmph_s_type           *ph_ptr         = cmph_ptr();

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_cmd_ptr != NULL);
  

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_info_ptr = CMD_INFO_PTR( call_cmd_ptr );

  #ifdef CM_DEBUG
  #error code not present
#endif

  /* If error found, notify clients of command error and return.
  */
  if (cmd_info_ptr->cmd_err != CM_CALL_CMD_ERR_NOERR)
  {
    cmcall_client_cmd_err_deallocate (call_cmd_ptr, cmd_info_ptr->cmd_err);
    return;
  }


  call_cmd_err = cmcall_client_cmd_check( call_cmd_ptr );

  /*
    ** Notify clients if error found, then return.
    */
  if( call_cmd_err != CM_CALL_CMD_ERR_NOERR )
  {
    if (call_cmd_ptr->cmd == CM_CALL_CMD_ORIG ||
       call_cmd_ptr->cmd == CM_CALL_CMD_CC_COMPLETE_REORIG)
    {
    cmcall_client_cmd_err_deallocate( call_cmd_ptr, call_cmd_err );
    }
    else
    {
      cmcall_client_cmd_err( call_cmd_ptr, call_cmd_err );
    }
    return;
  }
  #ifdef FEATURE_CM_DEBUG_BUFFER
  cmdbg_add_to_dbg_buffer(CM_BUFF_CALL_CMD_TYPE, call_cmd_ptr->cmd, (void*)call_cmd_ptr);
  #endif

  if(call_cmd_ptr->cmd == CM_CALL_CMD_ORIG ||
    call_cmd_ptr->cmd == CM_CALL_CMD_RAB_REESTAB ||
    call_cmd_ptr->cmd == CM_CALL_CMD_LTE_DATA_TRANSPORT_REQ)
  {
    cmpsm_process_deep_sleep_timer_expiry();
  }

  if(call_cmd_ptr->cmd == CM_CALL_CMD_ORIG || call_cmd_ptr->cmd == CM_CALL_CMD_SUPS )
  {  
     cm_psm_enter_req_status_e_type psm_status =  cmpsm_get_psm_status();     
     if (psm_status == CM_PSM_ENTER_STATUS_WAIT_READY_RSP )
     {
          cmpsm_set_high_activity_timer(CMPSM_HIGH_ACT_TIMER_VAL);
     }
  }
  switch(call_cmd_ptr->cmd)
  {

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    /* Process here all generic commands, ie. those
    ** that are processed the same way for both GW and 1X calls
    */
    case CM_CALL_CMD_INFO_GET:


      /*
      ** Notify clients of command processing status.
      */
      cmcall_client_cmd_err( call_cmd_ptr, call_cmd_err );

      /* If we got here, no errors were found,
      ** so forward the client command to MC.
      */
      wait_for_reply = cmcall_client_cmd_forward_ll( call_cmd_ptr );

      /* If NO need to wait for replay from MC,
      ** call the reply function right a way.
      */
      if( ! wait_for_reply )
      {
        cmcall_client_cmd_ll_reply( call_cmd_ptr, CM_CALL_CMD_ERR_NOERR );
      }

      /* We're done for generic commands
      */
      break;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case CM_CALL_CMD_ORIG:
    case CM_CALL_CMD_CC_COMPLETE_REORIG:

      /* Origination is a bit more complicated, so we'll handle it here
      */
      #if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
      cmcall_client_orig_proc (call_cmd_ptr);
	  #endif
	  
      break;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #if defined FEATURE_MMODE_CALL_SUPS  
    case CM_CALL_CMD_SUPS:

      /*
      ** Find out what mode we are in
      */
      call_mode = cmcall_call_id_mode( cmd_info_ptr->call_id );

      /* Process command based on call mode
      */
      switch(call_mode)
      {
        case CM_CALL_MODE_INFO_GW_CS:
          #if defined(CM_GW_SUPPORTED) 
          cmwcall_client_cmd_proc(call_cmd_ptr);
          #endif
          break;

        case CM_CALL_MODE_INFO_CDMA:
          #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
          cmxcall_client_cmd_proc( call_cmd_ptr );
          #endif
          break;

        case CM_CALL_MODE_INFO_IP:
          #if defined(FEATURE_IP_CALL) 
          cmipcall_client_cmd_proc ( call_cmd_ptr );
          #endif
          break;

          /* NOTE - Fall through on purpose */

        default:
          cmcall_client_cmd_err( call_cmd_ptr, CM_CALL_CMD_ERR_CALL_ID_P );
          CM_ERR_1("Invalid call id %d with sups cmd",
                  cmd_info_ptr->call_id);
          break;

      } /*  switch(call_mode)  */

      break;
 #endif

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#if 0
    case CM_CALL_CMD_INFO_LIST_GET:

      /*
      ** Notify clients of command processing status.
      */
      cmcall_client_cmd_err( call_cmd_ptr, call_cmd_err );

      cmcall_get_call_info_list_proc( call_cmd_ptr );
      break;
#endif 
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    #if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
    case CM_CALL_CMD_END:
      /* Update the end cmd with current state of calls
      */
      cmcall_update_call_cmd_end (call_cmd_ptr);

      cmcall_send_mode_spec_end  (call_cmd_ptr);

      break;
    #endif
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    #if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
    case CM_CALL_CMD_CC_COMPLETE_REORIG_PHASE2:
      cmcall_client_reorig_phase2 ((cm_cmd_type*)call_cmd_ptr);
      break;
    #endif
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case CM_CALL_CMD_BLOCKED_APN_LIST:
      #if defined(FEATURE_MMODE_3GPP)
      cmwcall_client_cmd_proc( call_cmd_ptr );
      #endif
      break;

    #ifdef FEATURE_MMODE_DUAL_ACTIVE
    case CM_CALL_CMD_LOCAL_HOLD:
      cmcall_cmd_local_hold_proc(call_cmd_ptr);
      break;
	#endif

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    default:
      /*
      ** Find out what mode we are in
      */

      call_mode = cmcall_call_id_mode( call_cmd_ptr->info.call_id );


      /* Process command based on call mode
      */
      switch(call_mode)
      {
        case CM_CALL_MODE_INFO_GW_CS:
        case CM_CALL_MODE_INFO_GW_PS:
          #if defined(CM_GW_SUPPORTED) 
          cmwcall_client_cmd_proc(call_cmd_ptr);
          #endif
          break;

        case CM_CALL_MODE_INFO_CDMA:
          #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
          cmxcall_client_cmd_proc( call_cmd_ptr );
          #endif
          break;

        case CM_CALL_MODE_INFO_IP:

          #if defined(FEATURE_IP_CALL) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
          cmipcall_client_cmd_proc ( call_cmd_ptr );
          #endif
          break;

          /*lint -fallthrough */
          /* NOTE - Fall through on purpose for cmds other than call end
          */

        default:
          cmcall_client_cmd_err( call_cmd_ptr, CM_CALL_CMD_ERR_CALL_ID_P );
          CM_ERR_1("Invalid call ptr for call id %d for call ending",
                  call_cmd_ptr->info.end_params[0].call_id);
          break;

      } /*  switch(call_mode)  */
  } /* switch(CALL_CMD_PTR(cmd_ptr)->cmd) */

  SYS_ARG_NOT_USED(call_ptr);

} /* cmcall_client_cmd_proc() */

/*===========================================================================

FUNCTION cmcall_nv_refresh

DESCRIPTION
  Refresh CMcall NVs


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_nv_refresh(

   uint8             refresh_subs_mask
    /* Subs mask whose NVs need to be refreshed
    */
 )
{
  uint8                                     i = 0;

  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
   if (SYS_MODEM_AS_ID_1_MASK & refresh_subs_mask)
   {
     /* Initialize ssac info */
     cmcall_ssac_init(SYS_MODEM_AS_ID_1);
   }
   else if(MAX_SIMS > 1 && (SYS_MODEM_AS_ID_2_MASK & refresh_subs_mask))
   {
     cmcall_ssac_init(SYS_MODEM_AS_ID_2);
   }
   else if(MAX_SIMS > 2 && (SYS_MODEM_AS_ID_3_MASK & refresh_subs_mask))
   {
     cmcall_ssac_init(SYS_MODEM_AS_ID_3);
   }
  #endif

} /* cmcall_nv_refresh() */


/*===========================================================================

FUNCTION cmcall_init

DESCRIPTION
  Initializing the call object.

  This function must be called before the call object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_init( void )
{

  unsigned int i;
      /* Loop index */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  

  /* Initialize all call object array
  */
  for ( i = 0; i < ARR_SIZE(cm_call_obj_array); i++ )
  {
    cm_call_obj_array[i] = NULL;
  } /* for  */

  /* Initialize the array of PDP context transation ids
  ** Set all reservation flags to FALSE .
  ** Array of boolean of size CM_MAX_PDP_NUM.
  */
  memset(pdp_context_trans_id, FALSE, sizeof(pdp_context_trans_id));

  memset(call_context_trans_id, FALSE, sizeof(call_context_trans_id));
  #ifdef CM_GW_SUPPORTED
  memset (&cmwcall_overall_calls, 0, sizeof(cm_overall_calls_s_type));

  /*
  ** Notify DIAG of call state change
  */
  cmlog_call_state(CM_SS_MAIN);
  #endif /* defined (FEATURE_GSM ) || defined (FEATURE_WCDMA ) */

  #ifdef FEATURE_GSM
  /*lint -e{611} */
  #ifdef FEATURE_MMODE_DUAL_SIM
  for (i=SYS_MODEM_AS_ID_1; i < cmpmprx_get_num_of_sims();i++)
  {
    if(geran_multisim_eng_mode_register_callback(ENG_MODE_CODEC_HO_INFO, (void*)cm_gsm_codec_info_cb,i) == FALSE)
    {
      CM_ERR_1("GERAN eng mode reg fail for Sub %d",i);
    }
    else
    {
      CM_MSG_MED_2("GERAN eng mode reg = 0x%x for Sub %d",cm_gsm_codec_info_cb,i);
    }
  }
  #else
  if(geran_eng_mode_register_callback(ENG_MODE_CODEC_HO_INFO, (void*)cm_gsm_codec_info_cb) == FALSE)
  {
    CM_ERR_0("GERAN eng mode reg fail");
  }
  #endif

  #endif

  #if defined(FEATURE_WCDMA) && defined (WCDMA_RRC_INDICATE_CODEC_TO_CM)
  #ifdef FEATURE_SEGMENT_LOADING
  if(pi_mmode_wcdma == NULL)
  {
    CM_ERR_0("pi_mmode_wcdma is NULL");
  }
  else
  {
    IWCDMA_rrc_register_rrc_codec_ho_info_cb(pi_mmode_wcdma,
          cm_wcdma_codec_info_cb);
  }
  #else
  rrc_register_rrc_codec_ho_info_cb(cm_wcdma_codec_info_cb);
  #endif
  #endif

  #ifdef FEATURE_TDSCDMA
  #ifdef FEATURE_SEGMENT_LOADING
  if(pi_mmode_tdscdma == NULL)
  {
    CM_ERR_0("pi_mmode_tdscdma is NULL");
  }
  else
  {
    // Call the corresponding ITDSCDMA API once it's ready.
    //ITDSCDMA_rrc_register_rrc_codec_ho_info_cb(pi_mmode_tdscdma,
    //      cm_td_codec_info_cb);
    CM_MSG_HIGH_0("ITDSCDMA_rrc_register_rrc_codec_ho_info_cb is not available.");
  }
  #else
  tdsrrc_register_rrc_codec_ho_info_cb(cm_td_codec_info_cb);
  #endif
  #endif

  #if defined(FEATURE_IP_CALL) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
  /* MNG cnf state reset */
  cmipcall_reset_mng_call_cnf();
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize NV items */
  cmcall_nv_refresh(SYS_MODEM_AS_ID_ALL_ACTIVE_MASK);

} /* cmcall_init() */

#ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
/*===========================================================================
FUNCTION cmcall_hybr_pref_was_toggled_for_origination

DESCRIPTION

  Determine whether hybrid preference was toggled for origination.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if hybrid preference was toggled for origination,
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean cmcall_hybr_pref_was_toggled_for_origination(

     cm_hybr_pref_e_type        requested_hybr_pref,
     /* Hybrid preference requested for the next call */

     sys_modem_as_id_e_type     as_id
      /* as id for call */
)
{

  /* Step 1: Check top_ptr's hybr_pref
  ** Step 2: Do the processing
  */
  cm_ss_e_type     cm_ss = CM_SS_MAIN;
  cm_orig_q_s_type *ph_orig_para_top_ptr = NULL;

  if (cmph_is_ssim())
  {
    cm_ss = CM_SS_MAIN;
  }
  else
  {
    cm_ss = cmph_map_subs_to_ss(as_id);
  }

  if (!cmph_msim_is_3gpp2_allowed(as_id))
  {
    return FALSE;
  }

  ph_orig_para_top_ptr = cmtask_orig_para_get_top( cm_ss );

  if( ph_orig_para_top_ptr != NULL &&
      ph_orig_para_top_ptr->orig->orig_hybr_pref != CM_HYBR_PREF_NONE )
  {
    return requested_hybr_pref != ph_orig_para_top_ptr->orig->orig_hybr_pref;
  }
  else
  {
     /* We're forcing mode for origination */
    return (requested_hybr_pref != CMPH_SS_HYBR_PREF(as_id,cmph_ptr()));
  }
}
#endif

#if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
/*===========================================================================

FUNCTION cmcall_determine_ss_for_origination

DESCRIPTION

  Based on mode and hybrid preference, determine ss for call object /
  cmpref_proc_cmd_pref_sys_chgd() function.

  The assumption is that for call originations sys pref change will be sent
  to either 1X or HDR (depending on where the origination will go) but not both.
  Sys pref change goes to both only when hybr_pref is toggled from on
  to off or vice versa.

DEPENDENCIES
  None

RETURN VALUE
  ss

SIDE EFFECTS
  None

===========================================================================*/
cm_ss_e_type cmcall_determine_ss_for_origination(
  cm_mode_pref_e_type    mode_pref,

  cm_hybr_pref_e_type    hybr_pref,

  cm_cc_mode_pref_e_type mode_usage,

  const cmcall_s_type    *call_ptr
)
{
  cm_ss_e_type               ss = CM_SS_NONE;
  sd_ss_mode_pref_e_type     sd_mode_pref;
  cmcall_s_type              *call_ptr_temp = NULL;
  boolean                    is_hybr_pref_toggled = FALSE;
  cm_orig_q_s_type  *e911_stck_obj_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  CM_MSG_HIGH_2("cmcall_determine_ss_for_origination mode_pref %d hybr_pref %d",mode_pref,hybr_pref);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Determine mode preference for the given call */
  sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref( mode_pref );

  CM_MSG_HIGH_1("sd_mode_pref %d",sd_mode_pref);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Find if the call can be made over IP
  */
  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE   
  ss = cmcall_instance_for_ipcall (call_ptr);
  
  /* VS call can only be made over IP. So return ss = NONE immediately if it
  ** is not possible .
  */
  if( call_ptr->call_type == CM_CALL_TYPE_VS && ss == CM_SS_NONE  )
  {
    return ss;
  }

  /* If ss != NONE, it means call is possible over IP.   Return immediately
     provided hybr pref was not toggled for origination. This can happen
     during 911 call orig over IMS
  */
  is_hybr_pref_toggled = cmcall_hybr_pref_was_toggled_for_origination(hybr_pref,
                                      call_ptr->asubs_id);
  if( (FALSE == is_hybr_pref_toggled ) &&
      (ss != CM_SS_NONE) )
  {
    return ss;
  }

  /* Don't need to continue HDR call processing below. return immediately if ss is
  hybr 2 */
  if((FALSE == is_hybr_pref_toggled ) &&
      (ss == CM_SS_HYBR_2
      #ifdef FEATURE_MMODE_HYBR3_ENABLED
      || ss == CM_SS_HYBR_3
      #endif
      ))
  {
    return ss;
  }
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If we toggle hybr_pref from ON to OFF or from OFF to ON,
     set ss to SD_SS_MAX and return.

     Otherwise
     - if mode_pref has CDMA component only, use SD_SS_MAIN
     - if mode_pref has HDR component only, use SD_SS_HDR
     - if it has both, call cmpref_is_hybr_hdr_call_allowed()
                       - yes: use SD_SS_HDR
                       - no:  use SD_SS_MAIN
     - if it has none, use SD_SS_MAIN
  */

  if(is_hybr_pref_toggled  &&
    call_ptr->call_type == CM_CALL_TYPE_EMERGENCY )
  {

    return CM_SS_MAX;
  }
  #endif

  sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref( mode_pref );
  
  CM_MSG_HIGH_2("sd_mode_pref %d ss %d",sd_mode_pref, ss);
  ss = CM_SS_MAIN;

  /* Set the ss for multi-sim targets */
  #ifdef FEATURE_MMODE_DUAL_SIM
  if (cmph_is_msim() && !cmph_is_subs_feature_mode_1x_sxlte(call_ptr->asubs_id))
  {
    CM_MSG_HIGH_1("msim call_ptr->ss=%d",call_ptr->ss);

    if(call_ptr->ss != CM_SS_HYBR_1 && call_ptr->ss != CM_SS_MAX)
    {
      ss = call_ptr->ss;
    }
  }
  else 
  #endif
   /* In case of back to back e911 calls, insert 2nd e911 preferences on HYBR2 stack
     to avoid unnecessary unforce orig mode of 1st call after 4s */
  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  if (!is_hybr_pref_toggled &&  call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
  {
    e911_stck_obj_ptr =cmtask_orig_para_search_act_type( CM_SS_MAIN,
                                                         CM_ACT_TYPE_EMERG_CALL); 

    if(e911_stck_obj_ptr!= NULL)
    {
      /* check for previous e911 call being present in HYBR2 and MAIN */
      if( (cmtask_orig_para_search_act_id(CM_SS_HYBR_2, e911_stck_obj_ptr->orig->act_id))!=NULL && e911_stck_obj_ptr->orig->act_id !=call_ptr->call_id)  
      {
	 CM_MSG_HIGH_1("USE SS MAX for successive e911, prev e911 act id  %d",e911_stck_obj_ptr->orig->act_id);
         return CM_SS_MAX;
       }
    }
  }
  #endif

  if ( !cmss_is_hybr_status_on() )
  {
    /* Hybrid is off, have to attempt on SS_MAIN.
    */
    return ss;
  }

  #ifdef FEATURE_MMODE_3GPP2
  if( sd_misc_is_mode_pref(sd_mode_pref, SD_SS_MODE_PREF_HDR) &&
      !(sd_misc_is_mode_pref(sd_mode_pref, SD_SS_MODE_PREF_CDMA )))
  {
    /* HDR system bit set and CDMA system bit not set */
    CM_MSG_HIGH_0("HDR system bit set and CDMA system bit not set");
    ss = CM_SS_HDR;
  }
  else if( (sd_misc_is_mode_pref(sd_mode_pref, SD_SS_MODE_PREF_CDMA)) &&
           (sd_misc_is_mode_pref(sd_mode_pref, SD_SS_MODE_PREF_HDR)))
  {
    sys_modem_as_id_e_type asubs_id = cmph_map_cm_ss_to_subs(CM_SS_HYBR_1);

    /* CDMA system bit set and HDR system bit set */
    CM_MSG_HIGH_0("CDMA system bit set and HDR system bit set");

    /* If packet call is getting orig and hybrid hdr call is allowed, put
    ** the call on HYBR HDR instance
    */
    if( cmmsc_auto_is_hybr_hdr_call_allowed(asubs_id) &&
        (call_ptr->call_type == CM_CALL_TYPE_PS_DATA)
      )
    {
      CM_MSG_HIGH_0("Hybr HDR call allowed");
      ss = CM_SS_HDR;
    }
  }
  #endif
  
  /* If we are in hybrid and already have a voice call up on 1X system,
  ** then the data call should go by definition on 1X system  unless the
  ** voice call service option can support SVDO.
  */
  #ifdef FEATURE_HDR
  if( (ss == CM_SS_HDR) &&
      (call_ptr->call_type == CM_CALL_TYPE_PS_DATA) &&
      cmss_is_hybr_status_on())
  {

      cm_call_id_type voice_call_id;
      voice_call_id = cmcall_is_there_a_call_type ( CM_SS_MAIN,
                                                    CM_CALL_TYPE_VOICE,
                                                    CM_CALL_ID_INVALID);

      call_ptr_temp = cmcall_ptr(voice_call_id) ;
      if(call_ptr_temp == NULL)
      {
        CM_MSG_HIGH_0( "call_ptr_NULL");
        return ss;
      }

      if(voice_call_id != CM_CALL_ID_INVALID
         #if (defined (FEATURE_MMODE_CDMA_800) || defined (FEATURE_MMODE_CDMA_1900))
         && !cmxcall_is_svdo_capable(call_ptr_temp)
         #endif
         && (call_ptr_temp->cmcall_mode_info.info_type != CM_CALL_MODE_INFO_IP)
        )
      {
        if(call_ptr_temp->ss == CM_SS_MAIN)
        {
          ss = CM_SS_MAIN;
        }
      }
  }
  #endif

  SYS_ARG_NOT_USED (mode_usage);
  return ss;
}
#endif


#if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
/*===========================================================================

FUNCTION cmcall_call_control_complete_phase2_cb

DESCRIPTION
  Callback function provided to a client that is performing call control for CM.
  Is called when phase 1 call control is complete.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_call_control_complete_phase2_cb(

  cm_call_id_type                         call_id,
    /* call id */

  cm_cc_status_e_type                     status,
    /* status of command */

  cm_call_type_e_type                     call_type,
    /* call type */

  const cm_orig_call_control_s_type      *cdma_call_control_info_ptr,
    /* pointer to an info struct - NULL if not allowed on cdma */

  const cm_orig_call_control_s_type       *gw_call_control_info_ptr,
    /* pointer to an info struct - NULL if not allowed on gw */

  const void                              *user_data


)
/*lint -esym(715, status) */
{
  cm_cmd_type              *cmd_ptr;
  cmcall_s_type              *call_ptr;
  cm_call_cmd_s_type         *call_cmd_ptr;
  cm_call_cmd_info_s_type    *cmd_info_ptr;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(user_data                  != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*lint -save -e774 Boolean within 'if' always evaluates to False
  ** Yup, but only when CM_DEBUG is on
  */
  /* We expect user_data to point to a call command
  */
  if (user_data == NULL)
  {
    CM_ERR_1("NULL cmd_ptr in cc callback for %d", call_id);
    return;
  }
  /*lint -restore */

  if(cdma_call_control_info_ptr != NULL)
  {
    CM_MSG_HIGH_5("CC_complete_cb2:cdma call_control_result=%d, alpha.len=%d, cdam cc num len=%d, buf[0]=%d, buf[1]=%d",
                    cdma_call_control_info_ptr->call_control_result,
                    cdma_call_control_info_ptr->alpha.len,
                    cdma_call_control_info_ptr->num.len,
                    cdma_call_control_info_ptr->num.buf[0],cdma_call_control_info_ptr->num.buf[1]);
  }

  if(gw_call_control_info_ptr != NULL)
  {
    CM_MSG_HIGH_5("CC_complete_cb2:gw call_control_result=%d, alpha.len=%d, gw cc num len=%d, buf[0]=%d, buf[1]=%d",
                    gw_call_control_info_ptr->call_control_result,
                    gw_call_control_info_ptr->alpha.len, 
                    gw_call_control_info_ptr->num.len,
                    gw_call_control_info_ptr->num.buf[0],gw_call_control_info_ptr->num.buf[1]);
  }

  cmd_ptr = (cm_cmd_type *) user_data;
  call_cmd_ptr = CALL_CMD_PTR (cmd_ptr);
  cmd_info_ptr = CMD_INFO_PTR (call_cmd_ptr);

  /* Copy info to call object and queue it back to CM */
  call_ptr = cmcall_ptr (call_id);

  if (call_ptr == NULL)
  {
    CM_ERR_1("No call object found for call id %d in cc callback", call_id);
    /* Deallocate cmd_ptr. Otherwise leads to memory leak */
    cm_cmd_dealloc( cmd_ptr );
    return;
  }

  CALL_CMD_PTR(cmd_ptr)->info.call_control_processing_complete = TRUE;


  cmd_info_ptr->call_type = call_type;
  #ifdef FEATURE_OTASP
  /* If this is an otasp call then we gotta copy the activation code as well. */
  if (((call_type == CM_CALL_TYPE_STD_OTASP)     ||
      (call_type == CM_CALL_TYPE_NON_STD_OTASP)) &&
      (NULL != cdma_call_control_info_ptr))
  {
       cmd_info_ptr->cdma_info.otasp_act_code = cdma_call_control_info_ptr->activate_code;
  }
  #endif  /* FEATURE_OTASP */

  /*lint -save -e774 Boolean within 'if' always evaluates to False
  ** Yup, but only when CM_DEBUG is on
  */
  #if 0
  if (cdma_call_control_info_ptr != NULL)
  /*lint -restore */
  {
     *(&(call_ptr->cdma_cc_modified_info)) = *cdma_call_control_info_ptr;
  }
  else
  {
      /* Null is passed by call control for the case where Subscription is not available
      ** only
      */
      call_ptr->cdma_cc_modified_info.call_control_result =
        CM_CC_RESULT_NOT_ALLOWED;
      call_ptr->cdma_cc_modified_info.error_cause =
                                              CM_CALL_CMD_ERR_CAUSE_NO_SUBSC;
  }
#endif
  #ifdef FEATURE_MMODE_3GPP
  /*lint -save -e774 Boolean within 'if' always evaluates to False
  ** Yup, but only when CM_DEBUG is on
  */
  if (gw_call_control_info_ptr != NULL)
  /*lint -restore */
  {
    /******************************************************************
    ** Parameters returned by call control are not directly copied into
    ** Call object fields. They are used only during call origination
    ** if call_control_result says modified.
    ******************************************************************/

    *(&(call_ptr->gw_cc_modified_info)) = *gw_call_control_info_ptr;

    /* Copy emergency service category if present for emergency calls
    */
    #if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
    if (call_type == CM_CALL_TYPE_EMERGENCY)
    {

      /* Copy emergency service category for call control
      */
      cmd_info_ptr->num.emerg_srv_categ =
                         gw_call_control_info_ptr->num.emerg_srv_categ;

      CM_MSG_HIGH_1 ("CC ret call type EMERG emerg srv categ %d",
                    cmd_info_ptr->num.emerg_srv_categ);
    }
    else
    #endif
    if (call_type == CM_CALL_TYPE_PS_DATA)
    {
      CM_MSG_HIGH_1 (" CC result for PS_DATA is %d",
                   gw_call_control_info_ptr->call_control_result);
    }
  }
  else
  {
      /* Null is passed by call control for the case where Subscription is not available
      ** only
      */
      call_ptr->gw_cc_modified_info.call_control_result =
        CM_CC_RESULT_NOT_ALLOWED;
      call_ptr->gw_cc_modified_info.error_cause =
                                              CM_CALL_CMD_ERR_CAUSE_NO_SUBSC;
  }
  #else
  SYS_ARG_NOT_USED(gw_call_control_info_ptr);
  #endif


  cmd_ptr->cmd_type = CM_CMD_TYPE_CALL;
  CALL_CMD_PTR(cmd_ptr)->cmd = CM_CALL_CMD_CC_COMPLETE_REORIG_PHASE2;

  cm_cmd_queue (cmd_ptr);
}  /* cmcall_call_control_complete_phase2_cb */
/*lint +esym(715, status) */

/*===========================================================================

FUNCTION cmcall_call_control_complete_cb

DESCRIPTION
  Callback function provided to a client that is performing call control for CM.
  Is called when call control is complete.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_call_control_complete_cb(

  cm_call_id_type                         call_id,
    /* call id */

  cm_cc_status_e_type                     status,
    /* status of command */

  cm_call_type_e_type                     call_type,
    /* call type */

  const cm_orig_call_control_s_type      *cdma_call_control_info_ptr,
    /* pointer to an info struct - NULL if not allowed on cdma */

  const cm_orig_call_control_s_type       *gw_call_control_info_ptr,
    /* pointer to an info struct - NULL if not allowed on gw */

  const void                              *user_data
    /* data passed from original call */

)
/*lint -esym(715, status) */
{
  cm_cmd_type              *cmd_ptr;
  cmcall_s_type              *call_ptr;
  cm_call_cmd_s_type         *call_cmd_ptr;
  cm_call_cmd_info_s_type    *cmd_info_ptr;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(user_data                  != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*lint -save -e774 Boolean within 'if' always evaluates to False
  ** Yup, but only when CM_DEBUG is on
  */
  /* We expect user_data to point to a call command
  */
  if (user_data == NULL)
  {
    CM_ERR_1("NULL cmd_ptr in cc callback for %d", call_id);
    return;
  }
  /*lint -restore */

  if(cdma_call_control_info_ptr != NULL)
  {
    CM_MSG_HIGH_6("CC_complete_cb:status %d, call_id %d, cdma call_control_result=%d, alpha.len=%d, cdma cc num len=%d, buf[0]=%d",
                    status, call_id,
                    cdma_call_control_info_ptr->call_control_result,
                    cdma_call_control_info_ptr->alpha.len,
                    cdma_call_control_info_ptr->num.len,
                    cdma_call_control_info_ptr->num.buf[0]);
  }

  if(gw_call_control_info_ptr != NULL)
  {
    CM_MSG_HIGH_6("CC_complete_cb:status %d, call_id %d, gw call_control_result=%d, alpha.len=%d, gw cc num len=%d, buf[0]=%d",
                    status, call_id,
                    gw_call_control_info_ptr->call_control_result,
                    gw_call_control_info_ptr->alpha.len,
                    gw_call_control_info_ptr->num.len,
                    gw_call_control_info_ptr->num.buf[0]);
  }

  cmd_ptr = (cm_cmd_type *) user_data;
  call_cmd_ptr = CALL_CMD_PTR (cmd_ptr);
  cmd_info_ptr = CMD_INFO_PTR (call_cmd_ptr);

  /* Copy info to call object and queue it back to CM */
  call_ptr = cmcall_ptr (call_id);
  if (call_ptr == NULL)
  {
    CM_MSG_HIGH_2("No call object found for call id %d in cc callback, info.call_type %d",
                     call_id, call_cmd_ptr->info.call_type);

    /* Report to Client about error */
	if( call_cmd_ptr->info.call_type != CM_CALL_TYPE_PS_DATA)
	{
      cmcall_client_cmd_err( call_cmd_ptr, CM_CALL_CMD_ERR_OFFLINE_S );
    }

    /* Deallocate the cmd_ptr */
    cm_cmd_dealloc( call_cmd_ptr );

    return;
  }
  CALL_CMD_PTR(cmd_ptr)->info.call_control_processing_complete = TRUE;


  cmd_info_ptr->call_type = call_type;
  #ifdef FEATURE_OTASP
  /* If this is an otasp call then we gotta copy the activation code as well. */
  if (((call_type == CM_CALL_TYPE_STD_OTASP)     ||
      (call_type == CM_CALL_TYPE_NON_STD_OTASP)) &&
      (NULL != cdma_call_control_info_ptr))
  {
       cmd_info_ptr->cdma_info.otasp_act_code = cdma_call_control_info_ptr->activate_code;
  }
  #endif  /* FEATURE_OTASP */

  /*lint -save -e774 Boolean within 'if' always evaluates to False
  ** Yup, but only when CM_DEBUG is on
  */
  #if 0
  if (cdma_call_control_info_ptr != NULL)
  /*lint -restore */
  {
     *(&(call_ptr->cdma_cc_modified_info)) = *cdma_call_control_info_ptr;
  }
  else
  {
      /* Null is passed by call control for the case where Subscription is not available
      ** only
      */
      call_ptr->cdma_cc_modified_info.call_control_result =
        CM_CC_RESULT_NOT_ALLOWED;
      call_ptr->cdma_cc_modified_info.error_cause =
                                              CM_CALL_CMD_ERR_CAUSE_NO_SUBSC;
  }
#endif
  #ifdef FEATURE_MMODE_3GPP
  /*lint -save -e774 Boolean within 'if' always evaluates to False
  ** Yup, but only when CM_DEBUG is on
  */
  if (gw_call_control_info_ptr != NULL)
  /*lint -restore */
  {
    /******************************************************************
    ** Parameters returned by call control are not directly copied into
    ** Call object fields. They are used only during call origination
    ** if call_control_result says modified.
    ******************************************************************/

    *(&(call_ptr->gw_cc_modified_info)) = *gw_call_control_info_ptr;

    /* Copy emergency service category if present for emergency calls
    */
    #if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
    if (call_type == CM_CALL_TYPE_EMERGENCY)
    {

      /* Copy emergency service category for call control
      */
      if (gw_call_control_info_ptr->num.emerg_srv_categ)
      {
        cmd_info_ptr->num.emerg_srv_categ =
                         gw_call_control_info_ptr->num.emerg_srv_categ;
      }

      CM_MSG_HIGH_1 ("CC ret call type EMERG emerg srv categ %d",
                    cmd_info_ptr->num.emerg_srv_categ);
    }
    #endif
  }
  else
  {
      /* Null is passed by call control for the case where Subscription is not available
      ** only
      */
      call_ptr->gw_cc_modified_info.call_control_result =
        CM_CC_RESULT_NOT_ALLOWED;
      call_ptr->gw_cc_modified_info.error_cause =
                                              CM_CALL_CMD_ERR_CAUSE_NO_SUBSC;
  }
  #else
  SYS_ARG_NOT_USED(gw_call_control_info_ptr);
  #endif


  cmd_ptr->cmd_type = CM_CMD_TYPE_CALL;
  CALL_CMD_PTR(cmd_ptr)->cmd = CM_CALL_CMD_CC_COMPLETE_REORIG;

  cm_cmd_queue (cmd_ptr);
}  /* cmcall_call_control_complete_cb */
/*lint +esym(715, status) */
#if 0
/*===========================================================================

FUNCTION cmcall_is_cc_required

DESCRIPTION
  Check whether call control is required.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_cc_required
(
  cm_cc_subscription_source_e_type     subscription_source
)
{
  #if (defined FEATURE_GSTK && defined FEATURE_MMGSDI_SESSION_LIB)

  mmgsdi_app_enum_type                     app_type = MMGSDI_APP_NONE;
  mmgsdi_service_enum_type                 cc_type = MMGSDI_NONE;
  boolean                                  is_srv_available = FALSE;
  cm_mmgsdi_session_info_s_type            sess_info =
                                          {MMGSDI_MAX_SESSION_TYPE_ENUM, 0 };
  if(subscription_source != CM_CC_SUBSCRIPTION_SOURCE_NONE)
  {
    if(cm_mmgsdi_ssn_tbl_get_ssn_info_for_gw_slot(
          #if defined(FEATURE_MMODE_TRIPLE_SIM)
          (subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_3)?MMGSDI_SLOT_3:
          #endif
         ((subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_2)?MMGSDI_SLOT_2\
         :MMGSDI_SLOT_1),
         &sess_info) != CM_MMGSDI_SUCCESS)
    {
       CM_MSG_HIGH_0("MMGSDI session info read failed");
       return FALSE;
    }

    app_type = cm_mmgsdi_ssn_tbl_get_app_type_for_type(sess_info.session_type);
    CM_MSG_HIGH_1 ("sess type: %d", sess_info.session_type);
  }

  CM_MSG_HIGH_2 ("session id: %d app_type: %d", sess_info.session_id, app_type);

  switch (app_type)
  {
    case MMGSDI_APP_SIM:
      cc_type = MMGSDI_GSM_SRV_CALL_CONTROL;
      break;

    case MMGSDI_APP_RUIM:
      cc_type = MMGSDI_CDMA_SRV_CALL_CONTROL;
      break;

    case MMGSDI_APP_USIM:
      cc_type = MMGSDI_USIM_SRV_CALL_CONTROL;
      break;

    case MMGSDI_APP_CSIM:
      cc_type = MMGSDI_CSIM_SRV_CALL_CONTROL;
      break;

    default:
      cc_type = MMGSDI_NONE;
      CM_ERR_0 ("Invalid app type from MMGSDI");
  }

  if (mmgsdi_session_is_service_available (
                   sess_info.session_id,
                   cc_type,
                   cmcc_service_available_cb,
                   (uint32)NULL) != MMGSDI_SUCCESS)
  {
    CM_MSG_HIGH_0 ("MMGSDI return failure for is_service_available");
    return FALSE;
  }

  if (cm_wait_for_mmgsdi() == FALSE)
  {
    return FALSE;
  }

  if (cm_mmgsdi_srv_available_cnf_ptr == NULL)
  {
    CM_MSG_HIGH_0 ("srv_available_cnf_ptr is null, returning FALSE");
    return FALSE;
  }

  is_srv_available = cm_mmgsdi_srv_available_cnf_ptr->srv_available;

  cm_mem_free(cm_mmgsdi_srv_available_cnf_ptr);
  cm_mmgsdi_srv_available_cnf_ptr = NULL;

  CM_MSG_HIGH_1 ("srv enabled: %d", is_srv_available);
  return (is_srv_available);

  #else
  SYS_ARG_NOT_USED(subscription_source);
  #endif  /* FEATURE_GSTK  && FEATURE_MMGSDI_SESSION_LIB */
  /*lint -save -e527 */
  return FALSE;
}
#endif
#endif

#if  defined(FEATURE_MMODE_GW)

/*==========================================================================

FUNCTION CMCALL_MMGSDI_CARD_STATUS_IND_PROC

DESCRIPTION
  Process MMGSDI Card Status Indication, this may involve
  bringing down calls

DEPENDENCIES
  none.

RETURN VALUE
  none

SIDE EFFECTS
  none

==========================================================================*/
void cmcall_mmgsdi_card_status_ind_proc
(
  mmgsdi_events_enum_type           card_status_event,
    /* MMGSDI card status indication */

  mmgsdi_slot_id_enum_type          slot
    /* MMGSDI slot ID */

)
{
  cmss_s_type                              *ss_ptr = cmss_ptr ();
  #if ((defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
       !defined(FEATURE_MMODE_REMOVE_1X))

  #if defined (FEATURE_UIM_RUIM) && (!defined (FEATURE_UIM_RUIM_W_GSM_ACCESS)  \
                                 || ( defined (FEATURE_UIM_RUN_TIME_ENABLE)))
  mc_msg_type                              *mc_ptr;
  cmcall_s_type                            *call_ptr;
  cm_call_id_type                           call_id;
  #endif /* FEATURE_UIM_RUIM */

  cm_cc_subscription_source_e_type          cdma_subscription_source;
  #endif
  cm_cc_subscription_source_e_type          gw_subscription_source;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Nothing to do if we are not in a call.
  */
  if (cmcall_overall_call_state() == CM_OVERALL_CALL_STATE_CONV)
  {
    gw_subscription_source = cmph_get_gw_subscription_source(CM_SS_MAIN);
    #if ((defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
         !defined(FEATURE_MMODE_REMOVE_1X))
    cdma_subscription_source = cmph_get_cdma_subscription_source();
    #endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

    if((cmph_is_msim() && slot == cmph_ptr()->valid_card_1)
        ||
        !cmph_is_msim()
      )
    {
      /* For Main stack */

      /* Figure out which slot this event is for
      */
      if ( (ss_ptr->info.sys_mode == SYS_SYS_MODE_GSM) ||
           (ss_ptr->info.sys_mode == SYS_SYS_MODE_WCDMA )||
           (ss_ptr->info.sys_mode == SYS_SYS_MODE_LTE ) )
      {
        if ( (ss_ptr->info.sys_mode == SYS_SYS_MODE_LTE ) ||
              (((slot == MMGSDI_SLOT_1)

                      &&

               (card_status_event == MMGSDI_CARD_ERROR_EVT ||
                card_status_event == MMGSDI_CARD_REMOVED_EVT))

                      &&

               (gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_NONE ||
                gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_1))

                       ||

             (((slot == MMGSDI_SLOT_2)
                      &&
               (card_status_event == MMGSDI_CARD_ERROR_EVT ||
                card_status_event == MMGSDI_CARD_REMOVED_EVT))
                      &&
               (gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_NONE ||
                gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_2))

                       ||

               (((slot == MMGSDI_SLOT_3)
                      &&
               (card_status_event == MMGSDI_CARD_ERROR_EVT ||
                card_status_event == MMGSDI_CARD_REMOVED_EVT))
                      &&
               (gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_NONE ||
                gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_3))
           )
        {

          cmcall_overall_end_calls_per_subs(cmph_map_as_id_to_cm_ss(SYS_MODEM_AS_ID_1));
        }  /* End if ( (((slot ==     */
      }  /* if ( (ss_ptr->info.sys_mode ==      */

      #if ((defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
           !defined(FEATURE_MMODE_REMOVE_1X))
      else if ( (((slot == MMGSDI_SLOT_1) &&
             (card_status_event == MMGSDI_CARD_ERROR_EVT ||
              card_status_event == MMGSDI_CARD_REMOVED_EVT))
                   &&
             (cdma_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_1))

                                       ||

           (((slot == MMGSDI_SLOT_2) &&
             (card_status_event == MMGSDI_CARD_ERROR_EVT ||
              card_status_event == MMGSDI_CARD_REMOVED_EVT))
                   &&
             (cdma_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_2))

                                       ||

           (((slot == MMGSDI_SLOT_3) &&
          (card_status_event == MMGSDI_CARD_ERROR_EVT ||
           card_status_event == MMGSDI_CARD_REMOVED_EVT))
                &&
          (cdma_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_3))
         )
      {
        #if defined(FEATURE_UIM_RUIM) && (!defined(FEATURE_UIM_RUIM_W_GSM_ACCESS) \
                                   || ( defined(FEATURE_UIM_RUN_TIME_ENABLE)))
        #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
            /* Determine if the R-UIM is available */

        if (NV_RTRE_POLLING_CONTROL_POLL == nv_rtre_polling_control())
        #endif /* FEATURE_UIM_RUN_TIME_ENABLE */
        {
          if (!cmcall_emergency_call_active())
          {
            for (call_id = 0; call_id < CM_CALL_ID_MAX; call_id++)
            {
              call_ptr = cmcall_ptr((cm_call_id_type) call_id);

              if ( (call_ptr != NULL ) && (call_ptr->call_state != CM_CALL_STATE_IDLE)
              #ifdef FEATURE_MMODE_DUAL_SIM
              && (call_ptr->asubs_id == SYS_MODEM_AS_ID_1)
              #endif
              )
              {
                mc_ptr = cm_mc_get_buf_else_err_fatal();

                /* need to abort the call and return */

                /* Get MC command buffer and indicate the
                ** appropriate MC command and send the command to MC.
                */
                mc_ptr->hdr.cmd = MC_END_F;
                mc_ptr->end.num_call_ids = 1;
                mc_ptr->end.call_ids[0] = (cm_call_id_type ) call_id;

                #ifdef FEATURE_IS2000
                mc_ptr->end.end_reasons[0] = CAI_REL_NORMAL;
                #endif /* FEATURE_IS2000 */
                cm_mc_send_cmd( mc_ptr );

                /* Notify the client that the call is dropped
                */
                call_ptr->end_status = CM_CALL_END_UIM_NOT_PRESENT;
                cmcall_end(call_ptr);
              }  /* if ( call_ptr != NULL ) && */
            }  /* for (call_id = 0; call_id < CM_CALL_ID_MAX; call_id++) */
          }  /* if (!cmcall_emergency_call_active()) */
        }
        #endif  /* FEATURE_UIM_RUIM........*/
      }
      #endif  /* #if (defined(FEATURE_MMODE_CDMA_800)..... */
    }/* if (slot == valid card 1) */
    #if defined(FEATURE_MMODE_DUAL_SIM)
    else if(cmph_is_msim() && slot == cmph_ptr()->valid_card_2)
    {
      /* For HYBR_2 stack */
      if(card_status_event == MMGSDI_CARD_ERROR_EVT ||
                card_status_event == MMGSDI_CARD_REMOVED_EVT)
      {
        cmcall_overall_end_calls_per_subs(cmph_map_as_id_to_cm_ss(SYS_MODEM_AS_ID_2));
      }
    }
    #endif
    #if defined(FEATURE_MMODE_TRIPLE_SIM)
    else if(slot == cmph_ptr()->valid_card_3)
    {
      /* For HYBR_3 stack */
      if(card_status_event == MMGSDI_CARD_ERROR_EVT ||
                card_status_event == MMGSDI_CARD_REMOVED_EVT)
      {
        cmcall_overall_end_calls_per_subs(CM_SS_HYBR_3);
      }
    }
    #endif

  }  /* if (cmcall_overall_call_state() ==..... */
}  /* cmcall_mmgsdi_card_status_ind_proc */

/*===========================================================================

FUNCTION cmcall_process_act_proc_end_reason

DESCRIPTION
  To process CMCALL_ACT_PROC_END_REASON action for an originating call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_process_act_proc_end_reason (

         cmcall_s_type     *call_ptr,
           /* pointer to a call object */
         cmcall_act_e_type  redial_act
           /* The redial action */
)
{
  cm_sr_s_type           *sr_ptr                          = NULL;
    /* point at silent redial component */

  dword                   uptime                       = time_get_uptime_secs();
    /* get the current uptime */

  gsm_wcdma_cmcall_type  *wcall_info_p                    = NULL;
    /* To change AMR settings in the call */

  int                    redial_time = 2;
    /* silent redial interval is increased if MN returned resources unavailable
    ** for a call
    */

  byte                   connection_id                    = 0;
    /* connection ID */

  boolean                redial                           = FALSE;
  boolean                curr_is_ok_to_orig                    = FALSE;

  dword                  no_srv_time          = 0;
      /* Ballpark estimation for no-service-time due to forcing the mode */
  boolean                is_pref_changed     = FALSE;
      /* Indication if the mode preference is changed.
      */

  #ifdef FEATURE_3GPP_CSFB
  boolean                is_t303_timer_expiry = FALSE;

  boolean                is_leave_lte     = FALSE;
  #endif

  cmph_sub_config_s_type *ph_sub_config_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( call_ptr != NULL );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** Try to redial when there is a MM or AS reject cause listed below,
  ** which indicate that there is a potential that the next call might go
  ** through.
  ** If the reject cause is not present or not listed below, just end the call.
  */
  sr_ptr = &call_ptr->sr;
  redial = FALSE;
  ph_sub_config_ptr = cmph_get_sub_config_ptr(call_ptr->asubs_id);

  if(ph_sub_config_ptr == NULL)
  {
    return;
  }

  /* Check if cc_reject is present */
  if (call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_reject.present)
  {
    CM_MSG_HIGH_2 ("cc_reject is present, rej_type = %d, rej_value = %d",
             call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_reject.rej_type,
             call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_reject.rej_value);
    switch (call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_reject.rej_type)
    {
      case AS_REJECT_CAUSE:
        switch (call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_reject.rej_value)
        {
          case AS_REJ_RR_RANDOM_ACCESS_FAILURE:
          case AS_REJ_RRC_OPEN_SESSION_FAILURE:
          case AS_REJ_RRC_CLOSE_SESSION_IND:
          case AS_REJ_RRC_REL_IND:
            redial = TRUE;
            break;

          case AS_REJ_RR_REL_IND:
            redial = TRUE;
            if (*(cmph_get_gprs_anite_gcf_ptr()))
            {
               redial = FALSE;
            }
            break;

         case AS_REJ_LOW_LEVEL_FAIL:
            redial = TRUE;

            /* IF NV_GPRS_ANITE_GCF_I flag is turned on then silent redial
            ** timer gets increased to 15 secs. This is in place only for
            ** passing GCF test cases and will not be required in the field.
            */
            if (*(cmph_get_gprs_anite_gcf_ptr()))
            {
              redial_time = 15;
            }
            else
            {
              redial_time = 5;
            }
            break;

         /* Call can be retried within the smallest time possible.
         */
         case AS_REJ_LOW_LEVEL_IMMED_RETRY:
         case AS_REJ_ABORT_RADIO_UNAVAILABLE:
            redial = TRUE;
            redial_time = CMCALL_IMMED_REDIAL_TIME;
            break;
         #ifdef FEATURE_3GPP_CSFB
         case AS_REJ_LRRC_LOWER_LAYER_FAILURE:

            if(call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_LTE)
            {
              is_leave_lte = TRUE;
            }
            redial = TRUE;
            redial_time = CMCALL_IMMED_REDIAL_TIME;

            break;

         case AS_REJ_LRRC_UL_DATA_CNF_FAILURE_CONN_REL:
            redial = TRUE;
            redial_time = CMCALL_IMMED_REDIAL_TIME;
            break;

         #endif

         case AS_REJ_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED:
          default:
            redial = FALSE;
            break;
        }
        break;

      case MM_REJECT_CAUSE:
        switch (call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_reject.rej_value)
        {
          case MM_REJ_NO_SERVICE:
          case MM_REJ_TIMER_T3230_EXP:
          case MM_REJ_NO_CELL_AVAILABLE:
          case MM_REJ_WRONG_STATE:
            redial = TRUE; /*okay to redial*/
            break;
          default:
            redial = FALSE; /*okay to redial*/
            break;
         }
         break;

      case CNM_MN_REJECT_CAUSE:
        switch (call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_reject.rej_value)
        {
          case CNM_REJ_TIMER_T303_EXP:
            redial = TRUE; /*okay to redial*/
            #ifdef FEATURE_3GPP_CSFB
            is_t303_timer_expiry = TRUE;
            #endif
            break;

          case CNM_REJ_NO_RESOURCES:
          default:
            redial = FALSE;
            break;
        }
        break;
      #ifdef FEATURE_3GPP_CSFB
      case EMM_REJECT_CAUSE:
        switch (call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_reject.rej_value)
        {
          case EMM_REJ_TIMER_T3417_EXT_EXP:
            if(call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_LTE)
            {
              redial = TRUE;
              redial_time = CMCALL_IMMED_REDIAL_TIME;
              is_leave_lte = TRUE;
            }
            break;

          case EMM_REJ_SERVICE_REQ_FAILURE_LTE_NW_REJECT:
           redial = TRUE; /*okay to redial*/
           break;

           case EMM_REJ_SERVICE_REQ_FAILURE_CS_DOMAIN_NOT_AVAILABLE:
             /* End the call on ESR reject with CS domain not avail */
             redial = FALSE;
             break;

          default:
            redial = FALSE;
            break;
        }
        break;
      #endif
      case OTA_REJECT_CAUSE:
      default:
        redial = FALSE;
        break;
    } /* reject type */

  } /* cc reject */

  /* Check if CC_cause is present */
  if (call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause.present)
  {
    CM_MSG_HIGH_1 ("cc_cause present, cause_value = %d",
       call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause.cause_value);

    /* Pointer to GW cs call info */
    wcall_info_p  = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

    switch (call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause.cause_value)
    {
      /* Signalling connection not yet released for previous call */
      case CNM_MM_REL_PENDING:

        redial_time = 4;
        redial = TRUE;
        break;

       /* FUNCTION IS ALREADY UNDER FEATURE Additional GW feat flag not reqd*/
       /* Received when AMR is not supported by the network
       */
       case SERVICE_OR_OPTION_NOT_AVAILABLE:

        /* Changing the bearer capability to non AMR based one */
        /* gsm_speech_ver_pref_0  is added only for the GSM_AMR_WB feature, if this is defined
           and User wants to turn off AMR, then our speech preferences should start from 0 */
        #ifdef FEATURE_GSM_AMR_WB
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_0 = GSM_FR_SPEECH_VER_2;
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_1 = GSM_FR_SPEECH_VER_1;

        #ifdef FEATURE_GSM_HALF_RATE
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_2 = GSM_HR_SPEECH_VER_1;
        #else
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_2 = GSM_INVALID_SPEECH_VER;
        #endif /* FEATURE_GSM_HALF_RATE */
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_3 = GSM_INVALID_SPEECH_VER;

        #ifdef FEATURE_GSM_EXT_SPEECH_PREF_LIST
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_4 = GSM_INVALID_SPEECH_VER;
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_5 = GSM_INVALID_SPEECH_VER;
        #endif /* FEATURE_GSM_EXT_SPEECH_PREF_LIST */
        #else
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_1 = GSM_FR_SPEECH_VER_2;
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_2 = GSM_FR_SPEECH_VER_1;

        #ifdef FEATURE_GSM_HALF_RATE
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_3 = GSM_HR_SPEECH_VER_1;
        #else
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_3 = GSM_INVALID_SPEECH_VER;
        #endif /* FEATURE_GSM_HALF_RATE */

        #ifdef FEATURE_GSM_EXT_SPEECH_PREF_LIST
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_4 = GSM_INVALID_SPEECH_VER;
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_5 = GSM_INVALID_SPEECH_VER;
        #endif /* FEATURE_GSM_EXT_SPEECH_PREF_LIST */
        #endif /* FEATURE_GSM_AMR_WB */
        redial = TRUE;
        break;

      /* Change bearer to non-AMR */
      case RR_SEMANTICALLY_INCORRECT_MESSAGE :

        /* Changing the bearer capability to non AMR based one */
        /* gsm_speech_ver_pref_0  is added only for the GSM_AMR_WB feature, if this is defined
           and User wants to turn off AMR, then our speech preferences should start from 0 */
        #ifdef FEATURE_GSM_AMR_WB
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_0 = GSM_FR_SPEECH_VER_2;
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_1 = GSM_FR_SPEECH_VER_1;

        #ifdef FEATURE_GSM_HALF_RATE
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_2 = GSM_HR_SPEECH_VER_1;
        #else
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_2 = GSM_INVALID_SPEECH_VER;
        #endif /* FEATURE_GSM_HALF_RATE */
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_3 = GSM_INVALID_SPEECH_VER;

        #ifdef FEATURE_GSM_EXT_SPEECH_PREF_LIST
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_4 = GSM_INVALID_SPEECH_VER;
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_5 = GSM_INVALID_SPEECH_VER;
        #endif /* FEATURE_GSM_EXT_SPEECH_PREF_LIST */
        #else
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_1 = GSM_FR_SPEECH_VER_2;
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_2 = GSM_FR_SPEECH_VER_1;

        #ifdef FEATURE_GSM_HALF_RATE
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_3 = GSM_HR_SPEECH_VER_1;
        #else
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_3 = GSM_INVALID_SPEECH_VER;
        #endif /* FEATURE_GSM_HALF_RATE */

        #ifdef FEATURE_GSM_EXT_SPEECH_PREF_LIST
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_4 = GSM_INVALID_SPEECH_VER;
        wcall_info_p->bearer_capability_1.gsm_speech_ver_pref_5 = GSM_INVALID_SPEECH_VER;
        #endif /* FEATURE_GSM_EXT_SPEECH_PREF_LIST */
        #endif /* FEATURE_GSM_AMR_WB */
        redial = TRUE;
        break;

      /* Network cannot accept call with 2 BC's.
      ** Redial is performed with only 1 BC.
      **
      ** !Still not clear if call has to be retried with BC2 values
      ** only if origination with just BC1 Fails.
      */
      case CONDITIONAL_IE_ERROR:

        #if defined( FEATURE_MULTIMEDIA_FALLBACK)
        /* Invalidate bc repeat indicator and 2nd Bearer capability */
        wcall_info_p->bc_repeat_indicator.present = FALSE;

        wcall_info_p->bearer_capability_2.present = FALSE;

        /* Invalidate hlc repeat indicator and 2nd hlc */
        wcall_info_p->hlc_repeat_indicator.present = FALSE;

        wcall_info_p->high_layer_compatibility_2.present = FALSE;

        /* Invalidate llc repeat indicator and 2nd llc */
        wcall_info_p->llc_repeat_indicator.present = FALSE;

        wcall_info_p->low_layer_compatibility_2.present = FALSE;

        redial = TRUE;

        /* CONDITIONAL_IE_ERROR occurs when network rejects call setup that
        ** contains valid BCIE1 and BCIE2. CM retries with only a valid BCIE1
        ** and BCIE2 set to NONE. It is desireable to get this operation done
        ** quickly
        */
        redial_time = 1;


        #endif

        break;

      case TEMPORARY_FAILURE:

        if(call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
        {
          redial = TRUE;
        }

        break;

      default:
        redial = FALSE;
        break;

    } /* cause_value */
  } /* cc cause */

  /* Get the connection id of the call */
  if (cm_find_connection_id (cmcall_call_id_get( call_ptr ),&connection_id))
  {
    /* unreserve connection id */
    if (cm_unreserve_connection_id (connection_id, call_ptr->asubs_id))
    {
      CM_MSG_HIGH_1 ("Unreserving connection id %d works", connection_id);
    }
    else
    {
      CM_MSG_HIGH_1 ("Connection id %d has been already unreserved", connection_id);
    }
  }
  else
  {
    CM_MSG_HIGH_1 ("connection id could not be found for call id = %d", cmcall_call_id_get (call_ptr));
  }


  if(redial)
  {
      /* If no more silent redials are left, end the call origination.  */
    if( ! sr_is_more_redial(sr_ptr, uptime) )
    {
      #ifdef FEATURE_MMODE_DUAL_SIM
      if(call_ptr->is_emerg_on_other_sub)
      {
        call_ptr->end_status = CM_CALL_END_CAUSE_TEMP_REDIAL_ALLOWED;
        call_ptr->prot_end_status              = CM_CALL_END_NONE;
        CM_MSG_HIGH_1("end_status being changed to %d",call_ptr->end_status);
      }
      #endif

      if( call_ptr->call_state != CM_CALL_STATE_IDLE )
      {
        cmwcall_end_req(call_ptr);
      }
      cmcall_end(call_ptr);
      return;
    }
    CM_MSG_HIGH_2 ("Silent redial for call %d, gets resent after %d secs",
                 cmcall_call_id_get(call_ptr),
                 redial_time);

    /* Taking backup of sr_ptr->is_ok_to_orig or sr_ptr->hdr_is_ok_to_orig
    ** as it will be reset in sr_sched_next call below.
    */
    if( call_ptr->ss == CM_SS_MAIN ||
        call_ptr->ss == CM_SS_MAX ||
        (cmph_is_msim() &&
          (call_ptr->ss == CM_SS_HYBR_2
             #ifdef FEATURE_MMODE_HYBR3_ENABLED
              || call_ptr->ss == CM_SS_HYBR_3
             #endif
      )
         ))
    {
      curr_is_ok_to_orig = sr_ptr->is_ok_to_orig;
    }
    else
    {
      curr_is_ok_to_orig = sr_ptr->hdr_is_ok_to_orig;
    }

    CM_MSG_HIGH_1 ("orig_sent_on_mode %d",\
      call_ptr->sr.orig_sent_on_mode );


    #ifdef FEATURE_3GPP_CSFB

    if((call_ptr->csfb_type == CM_CALL_CSFB_TYPE_PPCSFB) &&
       (is_t303_timer_expiry == FALSE) &&
       (uptime >= (call_ptr->sr.sr_start_uptime + CM_CSFB_CALL_MAX_SR_TIME_SOFT_FAIL_RETRIES)))
    {
      CM_MSG_HIGH_2 ("Forcing is_leave_lte to TRUE as as 20 seconds timer expired on LTE start_uptime %d curr uptime %d",\
        call_ptr->sr.sr_start_uptime, uptime);
      is_leave_lte = TRUE;
    }

    if(is_leave_lte && call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_LTE)
    {
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Use GW for orig.
      */

      is_pref_changed = cmcall_remove_mode_and_force(call_ptr,
                                                     SD_SS_MODE_PREF_LTE,
                                                     &no_srv_time);
      CM_MSG_HIGH_2("pref_changed = %d updtd call fav mode pref %d",
                            is_pref_changed, call_ptr->favored_mode_pref);
      SYS_ARG_NOT_USED(is_pref_changed); /* print is not considered as access
                                            by lint */
    }
    #endif

    #if defined(FEATURE_MMODE_GW) && !defined(FEATURE_MMODE_GPRS_LTE)
    if( call_ptr->call_type == CM_CALL_TYPE_EMERGENCY 
        && (ph_sub_config_ptr->wcdma_freq_lock != 0)
      )
      {
        /* Force call preferences again , so that dual mode service
        ** Pure Limited service req goes to LL again
        */
        (void) cmcall_force_pref_on_the_fly( call_ptr,
                                             SD_SS_PREF_REAS_REDIAL_OPTI );

        is_pref_changed = TRUE;
     }
    #endif

      /* 1. Reset call end status to none.
      ** 2. Force the mode preference. (to get the ok_to_orig one more time).
      ** 3. Schedule next silent redial.
      */
    sr_sched_next( sr_ptr,
                   (dword) redial_time,           /* SR off period */
                   no_srv_time,                   /* Hold orig period */
                   CM_SR_ACT_DEC,
                   call_ptr->end_status,
                   redial_act
                 );
    call_ptr->end_status  = CM_CALL_END_NONE;

    /* Restoring sr_ptr->is_ok_to_orig or sr_ptr->hdr_is_ok_to_orig
    ** from curr_is_ok_to_orig.
    */

    if (is_pref_changed)
    {
      /* Preferences are changed so force ok to orig to FALSE */
      cmcall_update_ok_to_orig (call_ptr, FALSE);
    }
    else
    {
      /* restore the  ok to orig value */
      cmcall_update_ok_to_orig (call_ptr, curr_is_ok_to_orig);
    }

    /*
    ** Since the connection ID is marked by MN as unreserved when
    ** it rejects the call. CM needs to reserve another ID.
    */
    if(!cmcall_generate_connection_id(call_ptr))
    {
      CM_ERR_1("Redial: Can not set connection Id for call %d...ending call",
             cmcall_call_id_get(call_ptr));

      if( call_ptr->call_state != CM_CALL_STATE_IDLE )
      {
        cmwcall_end_req(call_ptr);
      }

      cmcall_end( call_ptr );
    }
    else
    {

      CM_MSG_HIGH_2("MN Connection Id %d reserved for %d",
                    call_ptr->gw_connection_id,
                    cmcall_call_id_get(call_ptr));
    }

  }
  else
  {
    #ifdef FEATURE_MMODE_DUAL_SIM
    if(call_ptr->is_emerg_on_other_sub)
    {
      call_ptr->end_status = CM_CALL_END_CAUSE_PERM_REDIAL_NOT_NEEDED;
      call_ptr->prot_end_status              = CM_CALL_END_NONE;
      CM_MSG_HIGH_1("end_status being changed to %d",call_ptr->end_status);
    }
    #endif

    if( call_ptr->call_state != CM_CALL_STATE_IDLE )
    {
      cmwcall_end_req(call_ptr);
    }
    cmcall_end( call_ptr );
  }

  return;
}/* cmcall_process_act_proc_end_reason () */
#if FEATURE_MMODE_DISABLE_PS
/*===========================================================================

FUNCTION cmcall_count_primary_pdp_calls

DESCRIPTION

  Counts total number of primary pdp calls

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  total number of primary pdp calls

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
int cmcall_count_primary_pdp_calls (

  cm_ss_e_type      ss
    /* stack whose calls need to be counted */
)
{
  int total_primary_pdp_calls = 0;
      /* counts the total number of primary pdp calls */
  cm_iterator_type call_itr;
      /* call Object Iterator */
  cmcall_s_type   *temp_call_ptr = NULL;
      /* call_ptr */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  temp_call_ptr = cmcall_obj_get_next(&call_itr);

  while(temp_call_ptr != NULL)
  {
    if ( temp_call_ptr->ss == ss &&
         temp_call_ptr->call_type == CM_CALL_TYPE_PS_DATA &&
         temp_call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.primary_pdp
       )
    {
      ++total_primary_pdp_calls;
    }

    temp_call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  return total_primary_pdp_calls;

} /* cmcall_count_primary_pdp_calls () */
#endif

/*===========================================================================

FUNCTION cmcall_get_call_obj_mo_in_orig

DESCRIPTION

  Returns

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  call pointer for the call type in origination state or NULL if none

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
cmcall_s_type *cmcall_get_call_obj_mo_in_orig (

  cm_call_type_e_type call_type
    /* call type */
)
{
  cm_iterator_type call_itr;
      /* call Object Iterator */
  cmcall_s_type   *call_ptr = NULL;
      /* call_ptr */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if (( call_ptr->call_type == call_type ) &&
        (call_ptr->call_state == CM_CALL_STATE_ORIG) &&
        (call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.primary_pdp ))
    {
      return call_ptr;
    } /* if ( call_ptr->call_type == call_type ) */

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  return NULL;
} /* cmcall_get_call_obj_mo_in_orig () */

#endif // #ifdef CM_GW_SUPPORTED

/*===========================================================================

FUNCTION  cmcall_check_call_type_in_call_state

DESCRIPTION
  Checks if there is a call of the given type and in the given state.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if a call can be found with the requested parameters.

SIDE EFFECTS
  none

===========================================================================*/

boolean cmcall_check_call_type_in_call_state (

                  cm_call_type_e_type   call_type,
                  /* Given call type */

                  cm_call_state_e_type   call_state
                  /* Given call state */
)
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ((call_ptr->call_type == call_type) && (call_ptr->call_state == call_state))
    {
      return TRUE;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while  */

  return FALSE;
} /* cmcall_check_call_type_in_call_state () */

/*===========================================================================

FUNCTION  cmcall_check_call_type_in_call_state_ss

DESCRIPTION
  Checks if there is a call of the given type and in the given state
  per stack.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if a call can be found with the requested parameters.

SIDE EFFECTS
  none

===========================================================================*/

boolean cmcall_check_call_type_in_call_state_ss (

                  cm_call_type_e_type   call_type,
                  /* Given call type */

                  cm_call_state_e_type   call_state,
                  /* Given call state */

                  cm_ss_e_type    ss
                  /* Given call on stack */
)
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
  /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ((call_ptr->call_type  == call_type)    &&
        (call_ptr->call_state == call_state)   &&
        ( (call_ptr->ss == ss) || ( (call_ptr->ss == CM_SS_MAX) && (ss!= CM_SS_HYBR_2) && (ss!= CM_SS_HYBR_3)) )
        )
    {
      return TRUE;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while  */

  return FALSE;
} /* cmcall_check_call_type_in_call_state () */

/*===========================================================================

FUNCTION  cmcall_check_for_call_state_on_ss

DESCRIPTION
  Checks if there is a call in a particular state on a given ss.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if there is a call in a particular state on a specified Serving system

SIDE EFFECTS
  none

===========================================================================*/

boolean cmcall_check_for_call_state_on_ss (
                  cm_ss_e_type    ss,
                  /* Given call type   */

                  cm_call_state_e_type   call_state
                  /* Given call state   */
)
{
  cmcall_s_type   *call_ptr;
  /* Call object pointer
   */
  cm_iterator_type call_itr;
   /* call Object Iterator
    */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ((call_ptr->ss == ss ) && (call_ptr->call_state == call_state))
    {
      return TRUE;
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while  */

  return FALSE;
} /* cmcall_check_for_call_state_on_ss() */


/*===========================================================================

FUNCTION  cmcall_check_each_call_in_call_state

DESCRIPTION
  Checks if each call is in the given state.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if each call can be found in the requested state.

SIDE EFFECTS
  none

===========================================================================*/

boolean cmcall_check_each_call_in_call_state (

                  cm_call_state_e_type   call_state
                  /* Given call state */
)
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */
  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  /* Search all the calls to find the first match with call_type and call_state */
  while(call_ptr != NULL)
  {
    if (call_ptr->call_state != call_state)
    {
      return FALSE;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while  */

  return TRUE;
} /* cmcall_check_each_call_in_call_state () */


/*===========================================================================

FUNCTION  cmcall_check_each_call_in_call_state_ss

DESCRIPTION
  Checks if each call is in the given state for the specified asubs_id.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if each call can be found in the requested state.

SIDE EFFECTS
  none

===========================================================================*/

boolean cmcall_check_each_call_in_call_state_ss (

        cm_call_state_e_type   call_state,
        /* Given call state */

        sys_modem_as_id_e_type asubs_id
)
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */
  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  /* Search all the calls to find the first match with call_type and call_state */
  while(call_ptr != NULL)
  {
    if (call_ptr->call_state != call_state && call_ptr->asubs_id == asubs_id)
    {
      return FALSE;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while  */

  return TRUE;
} /* cmcall_check_each_call_in_call_state () */


/*===========================================================================

FUNCTION cmcall_reset

DESCRIPTION
  Reset common call fields to default values reflecting a call in idle state.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_reset(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{

  unsigned int i=0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL )
 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Mark object as initialized.
  */
  CM_INIT_MARK( call_ptr->init_mask );

  /* Initialize common call fields
  */

  call_ptr->call_id                      = CM_CALL_ID_INVALID;
  call_ptr->call_type                    = CM_CALL_TYPE_NONE;
  call_ptr->call_state                   = CM_CALL_STATE_IDLE;
  call_ptr->call_subst.incom             = CM_CALL_INCOM_SUBST_NONE;
  call_ptr->call_subst.conv              = CM_CALL_CONV_SUBST_NONE;
  call_ptr->call_subst.idle              = CM_CALL_IDLE_SUBST_NONE;
  call_ptr->call_subst.orig              = CM_CALL_ORIG_SUBST_NONE;
  call_ptr->call_client_ptr              = CM_CLIENT_PTR_NONE;
  call_ptr->call_type                    = CM_CALL_TYPE_NONE;
  call_ptr->alpha.len                    = CM_ALPHA_LEN_NONE;
  call_ptr->enc_alpha.num_bytes          = CM_ALPHA_LEN_NONE;
  call_ptr->signal.is_signal_info_avail  = FALSE;
  call_ptr->end_status                   = CM_CALL_END_NONE;
  call_ptr->prot_end_status              = CM_CALL_END_NONE;
  call_ptr->sys_mode                     = SYS_SYS_MODE_NO_SRV;
  call_ptr->srv_type                     = CM_SRV_TYPE_NONE;
  call_ptr->orig_func                    = NULL;
  call_ptr->is_user_originated_call      = FALSE;
  call_ptr->orig_mode                    = SD_SS_ORIG_MODE_NONE;
  call_ptr->call_connect_time            = 0;
  call_ptr->call_end_time                = 0;
  call_ptr->send_call_event_end          = TRUE;
  call_ptr->ss                           = CM_SS_MAIN;
  call_ptr->favored_mode_pref            = CM_MODE_PREF_NONE;
  call_ptr->line                         = CM_ALS_LINE_NONE;
  call_ptr->ho_type                      = SYS_HO_TYPE_NONE;
  call_ptr->ho_status                    = SYS_HO_STATUS_NONE;
  call_ptr->umts_cdma_ho_status          = SYS_UMTS_CDMA_HANDOVER_NONE;
  call_ptr->direction                    = CM_CALL_DIRECTION_NONE;
  call_ptr->force_top_pref               = FALSE;
  call_ptr->gw_conn_id_src               = CONN_ID_UNKNOWN_SRC;
  call_ptr->gw_connection_id             = ( byte )CM_UNKNOWN_CONN_ID;
  call_ptr->cc_object_ptr                = NULL;
  #if defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)
  call_ptr->csfb_type                    = CM_CALL_CSFB_TYPE_NONE;
  #endif
  call_ptr->dom_sel                      = SYS_CM_DOMAIN_SEL_DOMAIN_MAX;
  call_ptr->asubs_id       = SYS_MODEM_AS_ID_1;
  call_ptr->client_asubs_id = SYS_MODEM_AS_ID_NONE;
  #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)  
  call_ptr->is_waiting_on_esr_rsp = FALSE;
  #endif

  /* Reset end_params
  */
  memset(&call_ptr->end_params,
         CM_CALL_CMD_PARAM_DEFAULT_VALUE,
         sizeof(cm_end_params_s_type));

  call_ptr->end_params.call_id   = CM_CALL_ID_UNASSIGNED;
  call_ptr->end_params.info_type = CM_CALL_MODE_INFO_NONE;

  /* Reset call mode info fields.
  */
  memset(&call_ptr->cmcall_mode_info,
         CM_CALL_CMD_PARAM_DEFAULT_VALUE,
         sizeof(cmcall_mode_info_type));

  call_ptr->answer_num_pi = 0;

  /* Initialize the number component.
  */
  cm_num_init( &call_ptr->num );
  cm_num_init( &call_ptr->calling_num );
  cm_num_init( &call_ptr->redirecting_number.num );

  #ifdef FEATURE_IP_CALL
  cm_num_init( &call_ptr->waiting_num );
  #endif
  /* Reset the silent redial component.
  */
  sr_reset( &call_ptr->sr );

  /* Initialize the phone pointer to point at the phone object.
  */
  call_ptr->ph_ptr = cmph_ptr();

  /* Initialize command reply object
  */
  cm_reply_init ( &call_ptr->reply );

  for( i=0; i < ARR_SIZE(call_ptr->prev_srv_status); i++)
  {
    call_ptr->prev_srv_status[i] = SYS_SRV_STATUS_NONE;
  }

  call_ptr->main_ss_mode  = CM_CC_MODE_PREF_NONE;

  #if (defined (FEATURE_MMODE_CDMA_800) || defined (FEATURE_MMODE_CDMA_1900))
  cmxcall_reset(call_ptr);
  #endif

  #if defined(CM_GW_SUPPORTED)
  cmwcall_reset(call_ptr);
  #endif

  #if defined(FEATURE_IP_CALL)&& !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
  cmipcall_reset(call_ptr);
  #endif

  #if defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)
  cmcsfbcall_reset(call_ptr);
  #endif

  call_ptr->speech_codec_info.network_mode = SYS_NETWORK_TYPE_NONE;
  call_ptr->speech_codec_info.speech_codec_type = SYS_SPEECH_CODEC_NONE;
  call_ptr->speech_codec_info.speech_enc_samp_freq = 0;
  call_ptr->is_em_vt = FALSE;  
  call_ptr->is_ip_orig_sent = FALSE;
}

/*===========================================================================

FUNCTION cmcall_is_call_id_in_range

DESCRIPTION
  Checks if a given Call Id is in a valid range from 0 to CALL_ID_MAX.

DEPENDENCIES
  Call Id must have already been allocated.

RETURN VALUE
  TRUE if valid.
  FALSE if not valid.

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean cmcall_is_call_id_in_range(cm_call_id_type call_id)
{
  /*
  ** cm_call_id_type is an unsigned type so smallest value that call_id
  ** can have is 0
  ** 0 is a valid call id so it's sufficient to test against CM_CALL_ID_MAX
  ** only
  */
  return (call_id < CMCALL_MAX_CALL_OBJ);
}/* cmcall_is_call_id_in_range() */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

#if defined(FEATURE_HDR_HYBRID) || (defined (FEATURE_MMODE_CDMA_800) || defined (FEATURE_MMODE_CDMA_1900) )
/*===========================================================================

FUNCTION cmcall_is_hdr_call

DESCRIPTION
  Checks whether there is an HDR call up.

DEPENDENCIES
  None

RETURN VALUE
  Valid call id of a HDR call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  None

===========================================================================*/
 cm_call_id_type cmcall_is_hdr_call
(
  void
)
{
  cmcall_s_type   *call_ptr;
    /* Call object pointer */
  cm_iterator_type call_itr;
    /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);


  while(call_ptr != NULL)
  {

    if ( call_ptr->call_type == CM_CALL_TYPE_PS_DATA  &&
         call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_HDR
       )
    {
      return (call_ptr->call_id);
    }

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while */

  return CM_CALL_ID_INVALID;

} /* cmcall_is_there_a_hdrdata_call() */


/*===========================================================================
FUNCTION CMCALL_IS_HYBR_HDR_CALL

DESCRIPTION
  Check whether this is a hybrid HDR call

DEPENDENCIES
  Call object must be initialized

RETURNS
  Boolean

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_hybr_hdr_call (cm_call_id_type *call_id_ptr)
{

  #ifdef FEATURE_HDR_HYBRID
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */
  #endif

  /*-----------------------------------------------------------------------*/

  #ifdef FEATURE_HDR_HYBRID
  if ( cmss_is_hybr_status_on() )
  {
    cmcall_obj_iterator_init(&call_itr);

    call_ptr = cmcall_obj_get_next(&call_itr);


    while(call_ptr != NULL)
    {
      if( (call_ptr->call_state  != CM_CALL_STATE_IDLE) &&
          (call_ptr->cmcall_mode_info.info.cdma_info.srv_opt
                                             == CAI_SO_HDR_PKT_DATA)
        )
      {
        if(call_id_ptr != NULL)
        {
          *call_id_ptr = call_ptr->call_id;
        }
        return TRUE;
      }
      call_ptr = cmcall_obj_get_next(&call_itr);
    } /*while */
  } /* if ( cmss_is_hybr_status_on() ) */
  #endif // FEATURE_HDR_HYBRID

  if(call_id_ptr != NULL)
  {
    *call_id_ptr = (cm_call_id_type) CM_CALL_ID_NONE;
  }
  return FALSE;

} /* cmxcall_is_hybr_hdr_call() */


/*===========================================================================
FUNCTION CMCALL_IS_HYBR_HDR_ORIG

DESCRIPTION
  Check whether there is a hybrid HDR call origination

DEPENDENCIES
  Call object must be initialized

RETURNS
  Boolean

SIDE EFFECTS
  none

===========================================================================*/
cmcall_s_type* cmcall_is_hybr_hdr_orig (void)
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */
  /*-----------------------------------------------------------------------*/

  if ( cmss_is_hybr_status_on() )
  {

    cmcall_obj_iterator_init(&call_itr);

    call_ptr = cmcall_obj_get_next(&call_itr);

    while(call_ptr != NULL)
    {
      if( call_ptr->call_state  == CM_CALL_STATE_ORIG   &&
          call_ptr->ss          == CM_SS_HDR            &&
          call_ptr->call_type   == CM_CALL_TYPE_PS_DATA
        )
      {
        return call_ptr;
      }

      call_ptr = cmcall_obj_get_next(&call_itr);
    }
  } /* if ( cmss_is_hybr_status_on() ) */

  return NULL;

} /* cmxcall_is_hybr_hdr_orig() */
#endif

#if  defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)

/*===========================================================================

FUNCTION cmcall_is_there_a_cdma_call_except

DESCRIPTION
  Checks is there is a call object allocated for CDMA,
  other than the passed call-id

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_a_cdma_call_except (cm_call_id_type call_id)
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */
  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA
         && call_ptr->call_id != call_id)
    {
        return call_ptr->call_id;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  return CM_CALL_ID_INVALID;

} /* cmcall_is_there_a_cdma_call */

/*===========================================================================

FUNCTION cmcall_is_there_hvolte_call_waiting_for_esr_rsp

DESCRIPTION
  Checks is there is a call object allocated for HVOLTE 
  call , which is waiting for ESR response from NAS

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_hvolte_call_waiting_for_esr_rsp ()
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */
  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if (cmcall_sent_on_1x_while_lte_active_in_single_mode(call_ptr)
        && call_ptr->is_waiting_on_esr_rsp == TRUE)
    {
        return call_ptr->call_id;
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  return CM_CALL_ID_INVALID;

} /* cmcall_is_there_a_cdma_call */
#endif


/*===========================================================================

FUNCTION cmcall_is_there_a_cdma_call

DESCRIPTION
  Checks is there is a call object allocated for CDMA.

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_a_cdma_call ()
{

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
 
   cmcall_s_type   *call_ptr;                        /* Call object pointer */
   cm_iterator_type call_itr;
       /* call Object Iterator */
   /*-----------------------------------------------------------------------*/
 
   cmcall_obj_iterator_init(&call_itr);
 
   call_ptr = cmcall_obj_get_next(&call_itr);
 
   while(call_ptr != NULL)
   {
     if ( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA)
     {
         return call_ptr->call_id;
     }
 
     call_ptr = cmcall_obj_get_next(&call_itr);
   }
 
   return CM_CALL_ID_INVALID;
  #else
   return CM_CALL_ID_INVALID;
  #endif

} /* cmcall_is_there_a_cdma_call */

/*===========================================================================

FUNCTION cmcall_is_there_a_cdma_call_in_conv

DESCRIPTION
  Checks is there is a call object allocated for CDMA.

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_a_cdma_call_in_conv ()
{
  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
   cmcall_s_type   *call_ptr;                        /* Call object pointer */
   cm_iterator_type call_itr;
       /* call Object Iterator */
   /*-----------------------------------------------------------------------*/
 
   cmcall_obj_iterator_init(&call_itr);
 
   call_ptr = cmcall_obj_get_next(&call_itr);
 
   while(call_ptr != NULL)
   {
     if ( call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA &&
          call_ptr->sys_mode == SYS_SYS_MODE_CDMA )
     {
       if (call_ptr->call_state == CM_CALL_STATE_CONV)
       {
         return call_ptr->call_id;
       }
     } /* if ( call_ptr->call_type == call_type ) */
 
     call_ptr = cmcall_obj_get_next(&call_itr);
   }
 
   return CM_CALL_ID_INVALID;
  #else
   return CM_CALL_ID_INVALID;
  #endif

} /* cmcall_is_there_a_cdma_call_in_conv */


/*===========================================================================
FUNCTION cmcall_is_there_a_call_activity

DESCRIPTION
  Checks if there is a any call activity (voice/emergency/CS_DATA/PS_DATA/SMS call)
  in UE.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if any call is active

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_there_a_call_activity
(
  cm_ss_e_type        ss
)
{
  cm_call_id_type    ps_data_call_id = CM_CALL_ID_INVALID;
  cm_call_id_type    cs_data_call_id = CM_CALL_ID_INVALID;

  cmcall_s_type     *ps_data_call_ptr = NULL;
  cmcall_s_type     *cs_data_call_ptr = NULL;

  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  if ( cmcall_is_there_a_call_type_ss( ss,
                                       CM_CALL_TYPE_VOICE,
                                       CM_CALL_ID_INVALID) != CM_CALL_ID_INVALID )
  {
    CM_MSG_HIGH_0( "Voice call activity" );
    return TRUE;
  }

  /* For E911 calls, need to check CM_SS_MAX since the SS value will
  ** be set to MAX for LTE E911 calls
  */
  if ( ( cmcall_is_there_a_call_type_ss( ss,
                                         CM_CALL_TYPE_EMERGENCY,
                                         CM_CALL_ID_INVALID) != CM_CALL_ID_INVALID )
                                        ||
       ( cmcall_is_there_a_call_type_ss( CM_SS_MAX,
                                         CM_CALL_TYPE_EMERGENCY,
                                         CM_CALL_ID_INVALID) != CM_CALL_ID_INVALID ) )
  {
    CM_MSG_HIGH_0( "Emergency call activity" );
    return TRUE;
  }

  if ( cmcall_is_there_a_call_type_ss( ss,
                                       CM_CALL_TYPE_CS_DATA,
                                       CM_CALL_ID_INVALID) != CM_CALL_ID_INVALID )
  {
    CM_MSG_HIGH_0( "CS DATA call activity" );
    return TRUE;
  }

#ifndef FEATURE_CM_DISABLE_IMS_EXT
  if( cmemg_get_reason_per_sub(cmph_map_cm_ss_to_subs(ss)) == CM_AC_REASON_THIRD_PARTY_IMS_E911 )
  {
    CM_MSG_HIGH_0( "Third party IMS E911" );
    return TRUE;
  }
#endif
  if( cmph_is_in_emergency_cb() )
  {
    CM_MSG_HIGH_0( "In emergency call back mode" );
    return TRUE;
  }
  #endif

  #if (defined (FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

  if ( cmcall_is_there_a_call_type ( ss,
                                     CM_CALL_TYPE_SMS,
                                     CM_CALL_ID_INVALID) != CM_CALL_ID_INVALID )
  {
    CM_MSG_HIGH_0( "SMS Call Activity" );
    return TRUE;
  }


  /* Check for any on-going PS DATA Calls */
  ps_data_call_id = cmcall_is_there_a_call_type ( ss,
                                                  CM_CALL_TYPE_PS_DATA,
                                                  CM_CALL_ID_INVALID );
  if ( ps_data_call_id != CM_CALL_ID_INVALID )
  {
    ps_data_call_ptr = cmcall_ptr( ps_data_call_id );
  }

  /* Check for any on-going CS DATA Calls */
  cs_data_call_id = cmcall_is_there_a_call_type ( ss,
                                                  CM_CALL_TYPE_CS_DATA,
                                                  CM_CALL_ID_INVALID );

  if ( cs_data_call_id != CM_CALL_ID_INVALID )
  {
    cs_data_call_ptr = cmcall_ptr( cs_data_call_id );
  }

  /* If the call mode info and the sys mode is CDMA return TRUE */
  if ( ( ps_data_call_ptr != NULL &&
         ps_data_call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA &&
         ps_data_call_ptr->sys_mode == SYS_SYS_MODE_CDMA )
                                            ||
       ( cs_data_call_ptr != NULL &&
         cs_data_call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA &&
         cs_data_call_ptr->sys_mode == SYS_SYS_MODE_CDMA )
     )
  {
    CM_MSG_HIGH_0( "Data Call Activity" );
    return TRUE;
  }
  #endif

  return FALSE;
}

/*===========================================================================

FUNCTION cmcall_is_there_a_call_type_ss

DESCRIPTION
  Checks is there is a requested call type in the particular SS.

DEPENDENCIES
  None

RETURN VALUE
  valid call id of a requested call type, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_a_call_type_ss
(
  cm_ss_e_type        ss,

  cm_call_type_e_type call_type,

  cm_call_id_type     ignore_call_id

)
/*lint -esym(715, ss) */
{

  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */
  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( (call_ptr->ss == ss ||
          ss == CM_SS_MAX
         ) &&
         call_ptr->call_type == call_type
       )
    {
      if (ignore_call_id == CM_CALL_ID_INVALID)
      {
        return call_ptr->call_id;
      }
      else
      {
        if (ignore_call_id != call_ptr->call_id)
        {
          return call_ptr->call_id;
        }
      }
    } /* if ( call_ptr->call_type == call_type ) */

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */
#ifndef FEATURE_CM_DISABLE_IMS_EXT
  /* If this is ext IMS e911, we will not have call obj. So give 3rd part e911 call id */
  if(call_type == CM_CALL_TYPE_EMERGENCY 
        && 
     ((ss != CM_SS_MAX && cmemg_get_reason_per_sub(cmph_map_cm_ss_to_subs(ss)) 
       == CM_AC_REASON_THIRD_PARTY_IMS_E911) ||
      (ss == CM_SS_MAX && cmemg_is_ext_e911_mode()))
  )
  {
    
    CM_MSG_HIGH_0("Return ext IMS E911 call ID 0xFE");

    return EXT_E911_CALL_ID;
  }
#endif
  return CM_CALL_ID_INVALID;

}/* cmcall_is_there_a_call_type_ss() */
/*lint +esym(715, ss) */

/*===========================================================================

FUNCTION cmcall_is_there_a_call_type

DESCRIPTION
  Checks is there is a requested call type.

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_a_call_type
(
  cm_ss_e_type        ss,

  cm_call_type_e_type call_type,

  cm_call_id_type     ignore_call_id

)
/*lint -esym(715, ss) */
{


  /* ss is only meanful lfor sglte and msim.  For all the others, set ss to max so
  ** that it can't be ignored while search for the call_type.
  */
  if (!cmph_is_feature_mask(FEATURE_MODE_MASK_SGLTE_MSIM))
  {
    ss = CM_SS_MAX;
  }

  return(cmcall_is_there_a_call_type_ss(ss, call_type, ignore_call_id));

} /* cmcall_is_there_a_call_type() */
/*lint +esym(715, ss) */

/*===========================================================================

FUNCTION cmcall_is_there_a_call_type_per_sub

DESCRIPTION
  Checks is there is a requested call type.on the specified sub

DEPENDENCIES
  None

RETURN VALUE
  valid call id, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_a_call_type_per_sub
(
  sys_modem_as_id_e_type       asubs_id,
  cm_call_type_e_type          call_type,
  cm_ss_e_type                *cm_ss
)
{

  cmcall_s_type    *call_ptr;
  cm_iterator_type  call_itr;

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( call_ptr->asubs_id == asubs_id && call_ptr->call_type == call_type)
    {
      if (cm_ss != NULL)
      {
        *cm_ss = call_ptr->ss;
      }
      return call_ptr->call_id;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  return CM_CALL_ID_INVALID;
}


/*===========================================================================

FUNCTION cmcall_is_there_a_call_activity_per_sub

DESCRIPTION
  Checks is there is a requested call type on the specified sub

DEPENDENCIES
  None

RETURN VALUE
  active calls' bit-mask

SIDE EFFECTS
  none

===========================================================================*/
uint64 cmcall_is_there_a_call_activity_per_sub
(
  sys_modem_as_id_e_type       asubs_id,
  uint64                       call_type_mask
)
{
  uint64           active_call_type_mask = 0;
  cmcall_s_type    *call_ptr;
  cm_iterator_type  call_itr;

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( call_ptr->asubs_id == asubs_id && call_ptr->call_state != CM_CALL_STATE_IDLE
       && (SYS_BM_64BIT(call_ptr->call_type) & call_type_mask))
    {
      active_call_type_mask |= SYS_BM_64BIT(call_ptr->call_type);
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  return active_call_type_mask;
}

#if defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)

/*===========================================================================

FUNCTION cmcall_is_there_csfb_call

DESCRIPTION
  Checks is there is a  requested CSFB call type.

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_csfb_call
(
  cm_ss_e_type        ss,

  cm_call_csfb_e_type csfb_type,

  cm_call_id_type     ignore_call_id

)
/*lint -esym(715, ss) */
{

  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
    /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if (
         (call_ptr->ss == ss ||
          call_ptr->ss == CM_SS_MAX ||
          ss == CM_SS_MAX
         ) &&
         call_ptr->csfb_type == csfb_type
       )
    {
      if (ignore_call_id == CM_CALL_ID_INVALID)
      {
        return call_ptr->call_id;
      }
      else
      {
        if (ignore_call_id != call_ptr->call_id)
        {
          return call_ptr->call_id;
        }
      }
    } /* if ( call_ptr->call_type == call_type ) */

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  return CM_CALL_ID_INVALID;

} /* cmcall_is_there_csfb_call() */
/*lint +esym(715, ss) */

/*===========================================================================

FUNCTION cmcall_are_there_csfb_call_preferences

DESCRIPTION
  Checks whether CM priority quieue contains any CSFB call preferences.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If CSFB call exist in Priority queue.
  FALSE  If there is no CSFB call.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_are_there_csfb_call_preferences
(
  cm_ss_e_type           ss
)
{
  sd_si_info_s_type   si_info_ptr;

  sd_si_info_ptr_get(cmph_map_cm_ss_type_to_sd_ss_type(ss), &si_info_ptr);
  /* Point at SD SI info */

  if ((cmtask_orig_para_search_orig_mode(ss,
        SD_SS_ORIG_MODE_1XCSFB_EMERG_ORIG) == NULL) &&
       (cmtask_orig_para_search_orig_mode(ss,
              SD_SS_ORIG_MODE_1XCSFB_NORMAL_ORIG) == NULL)&&
       (cmtask_orig_para_search_orig_mode(ss,
              SD_SS_ORIG_MODE_PPCSFB_NORMAL_ORIG) == NULL)&&
       (cmtask_orig_para_search_orig_mode(ss,
              SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG) == NULL)
      )

  {
    CM_MSG_HIGH_0("NO CSFB calls");
    return FALSE;
  }

  return TRUE;
} /* cmcall_are_there_csfb_call_preferences() */

#endif /*(FEATURE_3GPP_CSFB) ||  (FEATURE_LTE_TO_1X)*/

#ifndef FEATURE_MMODE_DISABLE_LTE_VOICE



/*===========================================================================

FUNCTION cmcall_vowlan_call_not_in_online_oprt_mode()

DESCRIPTION
  Checks if call is over vowlan and oprt_mode is not online

DEPENDENCIES
  None

RETURN VALUE
  TRUE if conditions are met

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_vowlan_call_not_in_online_oprt_mode(cmcall_s_type   *call_ptr)
{
  if (call_ptr->is_vowlan        == TRUE && 
      cmph_ptr()->true_oprt_mode != SYS_OPRT_MODE_ONLINE)
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION cmcall_is_there_volte_call_sub

DESCRIPTION
  Checks is there is a  requested VOLTE call type.

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_volte_call_sub
(
   cm_call_type_e_type         call_type,
        /* Call type to search */
   sys_modem_as_id_e_type      as_id
)
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
    /* call Object Iterator */

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
      if (( call_ptr->is_volte || call_ptr->is_ip_call ) &&
          (call_ptr->asubs_id == as_id || !cmph_is_msim())
      )
      {
          return call_ptr->call_id;
      }
      call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  return CM_CALL_ID_INVALID;

} /* cmcall_is_there_volte_call() */


boolean cmcall_is_volte_restricted_call
(
   const cmcall_s_type   *call_ptr
)
{
#if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
  /* if call mode for current plmn is VOLTE Only */
if(call_ptr->srv_type == CM_SRV_TYPE_LTE &&
       call_ptr->call_type == CM_CALL_TYPE_VOICE)
  {
    CM_MSG_HIGH_0("volte restricted call");
    return TRUE;
  }
  else if(call_ptr->srv_type == CM_SRV_TYPE_PS_ONLY)
  {
    CM_MSG_HIGH_0("PS only call");
    return TRUE;
  }
#else
  SYS_ARG_NOT_USED(call_ptr);
#endif
  return FALSE;

} /* cmcall_is_volte_restricted_call() */


/*===========================================================================

FUNCTION cmcall_get_active_volte_call

DESCRIPTION
  Find VOLTe call which is in active state

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_get_active_volte_call
(
   cm_call_type_mask_type         call_type_mask
        /* Call type to search */
)
{
  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(CM_GW_SUPPORTED)
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
    /* call Object Iterator */

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
       if ( cmcall_is_call_type_supp_for_ipcall (call_ptr->call_type) &&
           call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP &&
           ((SYS_BM_64BIT(call_ptr->call_type)) & call_type_mask) &&
           call_ptr->call_state == CM_CALL_STATE_CONV &&
           call_ptr->call_subst.conv == CM_CALL_CONV_CALL_ACTIVE)
      {
          return call_ptr->call_id;
      }
      call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  #else
  SYS_ARG_NOT_USED(call_type_mask);
  #endif
  return CM_CALL_ID_INVALID;

} /* cmcall_is_there_volte_call_obj() */
#ifdef FEATURE_MMODE_SRVCC
/*===========================================================================

FUNCTION cmcall_get_active_srvcc_volte_call

DESCRIPTION
  Find VOLTE  call which is in active CONV state for legacy SRVCC
  Otherwise CONV/INCOMING/Outgoing ORIG

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_get_active_srvcc_volte_call
(
   cm_call_type_mask_type         call_type_mask,
        /* Call type to search */
   sys_modem_as_id_e_type         asubs_id
)
{
  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(FEATURE_MMODE_GW) \
    && !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
    /* call Object Iterator */

  cmcall_obj_iterator_init(&call_itr);

  if(!cmph_get_sub_config_ptr(asubs_id)->mid_call_support)
  {

   call_ptr = cmcall_obj_get_next(&call_itr);

   while(call_ptr != NULL)
   {
     if ( cmcall_is_call_type_supp_for_ipcall (call_ptr->call_type) &&
           call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP &&
           ((SYS_BM_64BIT(call_ptr->call_type)) & call_type_mask) &&
           call_ptr->call_state == CM_CALL_STATE_CONV &&
           call_ptr->call_subst.conv == CM_CALL_CONV_CALL_ACTIVE)
     {
        return call_ptr->call_id;
     }
     call_ptr = cmcall_obj_get_next(&call_itr);
   } /* while */

  }
  else
  {
    call_ptr = cmcall_obj_get_next(&call_itr);

    while(call_ptr != NULL)
    {
      if( cmcall_is_call_type_supp_for_ipcall (call_ptr->call_type) &&
          call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP &&
           ((SYS_BM_64BIT(call_ptr->call_type)) & call_type_mask) &&
           (call_ptr->call_state == CM_CALL_STATE_CONV ||
		   call_ptr->call_state == CM_CALL_STATE_ORIG ||
		   call_ptr->call_state == CM_CALL_STATE_INCOM))
      {
          return call_ptr->call_id;
      }
      call_ptr = cmcall_obj_get_next(&call_itr);
    }
  }
  #else
  SYS_ARG_NOT_USED(call_type_mask);
  #endif
  return CM_CALL_ID_INVALID;

} /* cmcall_get_active_srvcc_volte_call() */

/*===========================================================================

FUNCTION cmcall_end_non_srvcc_call_ho_complete

DESCRIPTION
  1)Finds and ends calls not in SRVCC HO Procedure. This is done
  after sending ho_complete list of calls to clients.
  2)Calls which got originated after SRVCC HO Start and yet to
    identify a mode are ended.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_end_non_srvcc_call_ho_complete(void)
{
  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(CM_GW_SUPPORTED) \
  	  && !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
    /* call Object Iterator */

  cmcall_obj_iterator_init(&call_itr);


  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {

	/*1) End MO calls originated after SRVCC start
	  2) Locally end conference call in IMS domain */
    if(((call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE && call_ptr->call_type == CM_CALL_TYPE_VOICE  &&
		  !(call_ptr->direction == CM_CALL_DIRECTION_MT ))  ||
      (call_ptr->is_conf_call == TRUE )) && !call_ptr->is_srvcc_call)
    {
	  CM_MSG_HIGH_1("cmcall_get_active_srvcc_volte_call, removed call with no mode/conf call -call id %d",
       call_ptr->call_id);
	  if(call_ptr->is_conf_call == TRUE )
	  {
        call_ptr->end_status = CM_CALL_END_SRVCC_IMS_END;
	  }
      cmcall_end(call_ptr);

    }
	call_ptr = cmcall_obj_get_next(&call_itr);
  }

#endif
} /* cmcall_end_non_srvcc_call_ho_complete() */

#endif
#endif
/*===========================================================================

FUNCTION cmcall_find_active_ip_call_per_sub_except_wlan

DESCRIPTION
  Check if there is any IP(e911/voice/vt/vs) call on given sub, 
  except for WLAN call.
  for E911 call it also considers the case where call wsa originated 
  but ok_to_orig not yet received (info_type=NONE)

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/    
cm_call_id_type cmcall_find_active_ip_call_per_sub_except_wlan
(
   sys_modem_as_id_e_type		as_id
)
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
    /* call Object Iterator */

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ((call_ptr->call_type == CM_CALL_TYPE_VOICE ||
         call_ptr->call_type == CM_CALL_TYPE_EMERGENCY ||
         call_ptr->call_type == CM_CALL_TYPE_VT ||
         call_ptr->call_type == CM_CALL_TYPE_VS ) &&
        (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP || 
         call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_NONE) &&
           (call_ptr->asubs_id == as_id || !cmph_is_msim()) &&
           !call_ptr->is_vowlan)
      {
          return call_ptr->call_id;
      }
      call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  return CM_CALL_ID_INVALID;
} /* cmcall_find_active_ip_call_per_sub_except_wlan() */

/*===========================================================================

FUNCTION cmcall_is_there_volte_call_obj

DESCRIPTION
  Function used to distinguish internal/ext IMS

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_volte_call_obj
(
   cm_call_type_mask_type         call_type_mask
        /* Call type to search */
)
{
  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && !defined( FEATURE_MMODE_DISABLE_LTE_VOICE)
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
    /* call Object Iterator */

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
       if ( cmcall_is_call_type_supp_for_ipcall (call_ptr->call_type) &&
           call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP &&
           cmcall_is_active_ip_app(SYS_SYS_MODE_LTE, call_ptr->call_type) &&
           ((SYS_BM_64BIT(call_ptr->call_type)) & call_type_mask))
      {
          return call_ptr->call_id;
      }
      call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  #else
  SYS_ARG_NOT_USED(call_type_mask);
  #endif
  return CM_CALL_ID_INVALID;

} /* cmcall_is_there_volte_call_obj() */

/*===========================================================================

FUNCTION cmcall_is_there_volte_call_obj_except_wlan

DESCRIPTION
  Function used to distinguish internal/ext IMS

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_volte_call_obj_except_wlan
(
  cm_call_type_mask_type         call_type_mask
  /* Call type to search */
)
{
#if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && !defined( FEATURE_MMODE_DISABLE_LTE_VOICE)
  cmcall_s_type    *call_ptr; /* Call object pointer */
  cm_iterator_type  call_itr; /* Call object iterator */

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if (((SYS_BM_64BIT(call_ptr->call_type)) & call_type_mask) &&
         cmcall_is_call_type_supp_for_ipcall (call_ptr->call_type) &&
         call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP &&
         !call_ptr->is_vowlan)
    {
      return call_ptr->call_id;
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

#else
  SYS_ARG_NOT_USED(call_type_mask);
#endif
  return CM_CALL_ID_INVALID;
} /* cmcall_is_there_volte_call_obj_except_wlan() */

/*===========================================================================

FUNCTION cmcall_is_present_ip_call

DESCRIPTION
  Check whether an VOLTE/IP call present without
  check for call state or substate

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_present_ip_call(void)
{
  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
    /* call Object Iterator */

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
       if ( call_ptr->is_volte ||
          (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY &&
           call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP))
	  {
          return call_ptr->call_id;
      }
      call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  #endif
  return CM_CALL_ID_INVALID;

} /* cmcall_is_present_ip_call() */

/*===========================================================================

FUNCTION cmcall_is_there_vowlan_call

DESCRIPTION
  Checks is there is a  requested VOWLAN call type.

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_vowlan_call()
{
  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
    /* call Object Iterator */

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
      if ( call_ptr->is_vowlan)
      {
          return call_ptr->call_id;
      }
      call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */
  #endif
  return CM_CALL_ID_INVALID;

}


/*===========================================================================

FUNCTION cmcall_is_there_volte_call

DESCRIPTION
  Checks is there is a  requested VOLTE call type.

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_is_there_volte_call
(
   cm_call_type_e_type         call_type
        /* Call type to search */
)
{
  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
    /* call Object Iterator */

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
      if ( call_ptr->is_volte || call_ptr->is_ip_call )
      {
          return call_ptr->call_id;
      }
      call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */
  #else
  SYS_ARG_NOT_USED(call_type);
  #endif
  return CM_CALL_ID_INVALID;

} /* cmcall_is_there_volte_call() */

/*===========================================================================

FUNCTION cmcall_generate_connection_id

DESCRIPTION
  This function generates a Connection Id and sets the connection Id and its
  Source in the call Objet.

  Please note that Connection Id makes sense only for the GSM/WCDMA modes.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None.

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
boolean cmcall_generate_connection_id
/*lint -esym(715, call_ptr) */
/*lint -esym(818, call_ptr) could be declared as pointing to const
** Nope, not for GW flags
*/
(
  cmcall_s_type             *call_ptr
)
{
  #ifdef CM_GW_SUPPORTED
  byte   conn_id                                             = CM_UNKNOWN_CONN_ID;
  static dword               mn_connection_id_failure_uptime = CMCALL_TIMER_INACTIVE;
  static dword               sm_connection_id_failure_uptime = CMCALL_TIMER_INACTIVE;

  if(call_ptr == NULL)
  {
    return FALSE;
  }

  /*
  ** Get new connection id from the lower layer
  ** If we're out of connection ids, and this is the first time we are out of
  ** them since a successful allocation, let's wait some time before error fataling.
  ** If time is up, we should error fatal, otherwise return the allocated connection id.
  */
  #ifdef FEATURE_MMODE_PS_ENABLE
  if(call_ptr->call_type == CM_CALL_TYPE_PS_DATA)
  {
    if( !cmcall_reserve_sm_connection_id( & conn_id, call_ptr->asubs_id ) )
    {
      if( sm_connection_id_failure_uptime < time_get_uptime_secs ())
      {
        /* Timer is up!  Time to fatal error ....
        */
        cm_print_connection_ids();
        sys_err_fatal_invalid_value_exception((uint8)call_ptr->call_id);
      }
      else if( sm_connection_id_failure_uptime == CMCALL_TIMER_INACTIVE )
      {
        /* First failure after a successful allocation.  Set the timer.
        */
        sm_connection_id_failure_uptime = time_get_uptime_secs()
                                          + CMCALL_SM_CONNECTION_ID_HOLD_FATAL_ERR_TIME;
        CM_ERR_1("Can not get SM connection id for %d -- Holding on for 30 ...", call_ptr->call_id);
      }
      else
      {
        /* Failure again but we are still holding on, since the timer is not up yet...
        */
        CM_ERR_1("Can not get SM connection id for %d -- Holding on ...", call_ptr->call_id);
      }
      return FALSE;
    }

    /* Reset the uptime since we have a successful allocation.
    */
    sm_connection_id_failure_uptime = CMCALL_TIMER_INACTIVE;

    CM_MSG_HIGH_2("SM CONN_ID %d reserved for call_id %d", conn_id, call_ptr->call_id);

    cm_set_connection_id(call_ptr,conn_id);

    cm_set_connection_id_src(call_ptr,CONN_ID_SM);

    /* RESERVING NSAPI SHOULD BE DONE ONLY AFTER RESERVING CONN ID */
    call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.nsapi    =
                                               cmwcall_calculate_nsapi( conn_id,call_ptr->asubs_id );
  }
  else
  #endif
  {
  
    #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
    conn_id = cm_reserve_connection_id(call_ptr->asubs_id);

    if( conn_id == UNKNOWN_CONNECTION_ID )
    {
      if( mn_connection_id_failure_uptime < time_get_uptime_secs ())
      {
        /* Remove error fatal since it would cause crash if call is originated
        ** while UE is in LPM or waiting for LPM_OK
        */
        CM_ERR_1( "Cannot reserve MN connection id for %d", call_ptr->call_id);
      }
      else if( mn_connection_id_failure_uptime == CMCALL_TIMER_INACTIVE )
      {
        /* First failure after a successful allocation.  Set the timer.
        */
        mn_connection_id_failure_uptime = time_get_uptime_secs()
                                          + CMCALL_MN_CONNECTION_ID_HOLD_FATAL_ERR_TIME;
        CM_MSG_HIGH_1("Can not get MN connection id for %d -- Holding on for 30 secs...", call_ptr->call_id);
      }
      else
      {
        /* Failure again but we are still holding on, since the timer is not up yet...
        */
        CM_MSG_HIGH_1("Can not get MN connection id for %d -- Holding on ...", call_ptr->call_id);
      }
      return FALSE;
    }

    /* Reset the uptime since we have a successful allocation.
    */
    mn_connection_id_failure_uptime = CMCALL_TIMER_INACTIVE;

    CM_MSG_HIGH_2("MN Connection id %d reserved for %d", conn_id, call_ptr->call_id);

    cm_set_connection_id(call_ptr,conn_id);

    cm_set_connection_id_src(call_ptr,CONN_ID_MN);
  #endif
  }
  #endif // (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
  return TRUE;
} /* cmcall_generate_connection_id() */
/*lint +esym(715, call_ptr) */
/*lint +esym(818, call_ptr) */

#if (defined (FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
/*===========================================================================

FUNCTION cmcall_set_cdma_cc_result

DESCRIPTION
  This function sets the Call Control (CC) result to the specifid value for a
  given call object.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None.

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
void cmcall_set_cdma_cc_result
(
  cmcall_s_type             *call_ptr,
  cm_cc_result_e_type       cc_result
)
{
  if(call_ptr == NULL)
  {
    CM_ERR_0("Can not set CDMA CC Result, Null call_ptr");
    return;
  }
  #if 0
  call_ptr->cdma_cc_modified_info.call_control_result = cc_result;
  #endif
}/* cmcall_set_cdma_cc_result() */
#endif

/*===========================================================================

FUNCTION cmcall_set_gw_cc_result

DESCRIPTION
  This function sets the Call Control (CC) result to the specifid value for a
  given call object.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None.

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
void cmcall_set_gw_cc_result
(
  cmcall_s_type             *call_ptr,
  cm_cc_result_e_type       cc_result
)
{
  SYS_ARG_NOT_CONST(call_ptr);

  if(call_ptr == NULL)
  {
    CM_ERR_0("Can not set GW CC Result, Null call_ptr");
    return;
  }
  #ifdef CM_GW_SUPPORTED
  call_ptr->gw_cc_modified_info.call_control_result = cc_result;
  #else
  SYS_ARG_NOT_USED(cc_result);
  #endif
} /* cmcall_set_gw_cc_result() */


/*===========================================================================

FUNCTION cmcall_obj_iterator_init

DESCRIPTION
  This function initializes the Call Object Iterator.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None.

SIDE EFFECTS
  CM Call Object Iterator Current Index will be modified.

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
void cmcall_obj_iterator_init(cm_iterator_type *iterator_ptr)
{
  CM_ASSERT(iterator_ptr != NULL);

  iterator_ptr->next = 0;
  return;
}/* cmcall_obj_iterator_init() */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*===========================================================================

FUNCTION cmcall_obj_get_next

DESCRIPTION
  This function gets the next call object in the collection of call objects.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  Pointer to Call Object structure.

SIDE EFFECTS
  Call Object Iterator current index will be modified.

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
cmcall_s_type* cmcall_obj_get_next(cm_iterator_type *iterator_ptr)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  unsigned int   index;
  cmcall_s_type  *call_ptr = NULL;
  boolean found = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(iterator_ptr != NULL)
  CM_ASSERT(iterator_ptr->next <= ARR_SIZE(cm_call_obj_array))

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  index = iterator_ptr->next;


  /*
  ** if we have exhausted all call objects, we always
  ** return NULL.
  */
  if(index >= ARR_SIZE(cm_call_obj_array))
  {
    return NULL;
  }


  while((index < ARR_SIZE(cm_call_obj_array)) && !found)
  {
     call_ptr = cm_call_obj_array[index];
     if(call_ptr != NULL)
     {
       found = TRUE;
     }
     index++;
     iterator_ptr->next = index;
  }

  return call_ptr;
}/* cmcall_obj_get_next() */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION cmcall_obj_get_next_voip_silent_redial_call

DESCRIPTION
  This function gets the next call object in the collection of call objects
  which is VOIP and in the silent redial state.


DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  Pointer to Call Object structure.

SIDE EFFECTS
  Call Object Iterator current index will be modified.

===========================================================================*/
cmcall_s_type*            cmcall_obj_get_next_voip_silent_redial_call(

    cm_iterator_type      *iterator_ptr
)
{

  #ifdef FEATURE_MMODE_3GPP2

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  unsigned int   index;
  cmcall_s_type  *call_ptr = NULL;
  boolean found = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(iterator_ptr != NULL)
  CM_ASSERT(iterator_ptr->next <= ARR_SIZE(cm_call_obj_array))

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  index = iterator_ptr->next;


  /*
  ** if we have exhausted all call objects, we always
  ** return NULL.
  */
  if(index >= ARR_SIZE(cm_call_obj_array))
  {
    return NULL;
  }


  while((index < ARR_SIZE(cm_call_obj_array)) && !found)
  {
     call_ptr = cm_call_obj_array[index];


     /* Bear out if the call_ptr is NULL or it is not a IPCALL or call state
     ** is not as specified or VOIP SR is invalid.
     */
     if( (call_ptr != NULL )                   &&
         cmcall_is_ipcall_possible(call_ptr)   &&
         (call_ptr->call_state == CM_CALL_STATE_ORIG ) &&
         call_ptr->sr.voip_sr.is_valid
       )
     {
       found = TRUE;
     }

     index++;

     iterator_ptr->next = index;
  }

  /* If ipcall is found, return the call pointer.
  ** Otherwise return NULL.
  */
  if( found )
  {
    return call_ptr;
  }

  return NULL;
  #else
  return NULL;
  #endif
}/* cmcall_obj_get_next_voip_silent_redial_call */


/*===========================================================================

FUNCTION cmcall_set_call_direction

DESCRIPTION

  Sets the call direction.


DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  None.

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
void cmcall_set_call_direction
(
  cmcall_s_type             *call_ptr,
  cm_call_direction_e_type   call_direction
)
{

  CM_ASSERT(call_ptr != NULL);

  CM_ASSERT( BETWEEN(call_direction,
                     CM_CALL_DIRECTION_NONE,
                     CM_CALL_DIRECTION_MAX));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_ptr->direction  = call_direction;

} /* cmcall_set_call_direction () */


/*===========================================================================
FUNCTION cmcall_count_calls_with_call_type

DESCRIPTION
  Counts total number of calls of a particular call type

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  total number of calls

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
int cmcall_count_calls_with_call_type (

    cm_call_type_e_type call_type
      /* call type */
)
{
  int              total_calls = 0;
    /* counts the total number of calls */

  cm_iterator_type call_itr;
    /* call Object Iterator */

  cmcall_s_type    *call_ptr = NULL;
    /* call_ptr */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( call_ptr->call_type == call_type )
    {
      ++total_calls;
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  return total_calls;

} /* cmcall_count_calls_with_call_type () */

#if 0 /*Not being used*/
/*===========================================================================
FUNCTION cmcall_count_calls_with_call_type_per_ss

DESCRIPTION
  Counts total number of calls of a particular call type

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  total number of calls

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
int cmcall_count_calls_with_call_type_per_ss (

    cm_call_type_e_type call_type,
      /* call type */

    cm_ss_e_type   ss
     /* ss for which calls are to be counted */
)
{
  int              total_calls = 0;
    /* counts the total number of calls */

  cm_iterator_type call_itr;
    /* call Object Iterator */

  cmcall_s_type    *call_ptr = NULL;
    /* call_ptr */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( call_ptr->call_type == call_type &&
          call_ptr->ss == ss)
    {
      ++total_calls;
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  return total_calls;

} /* cmcall_count_calls_with_call_type () */
#endif

#if defined(FEATURE_MMODE_3GPP)
/*===========================================================================
FUNCTION cmcall_count_calls_with_call_type_per_as_id

DESCRIPTION
  Counts total number of calls of a particular call type for the specified
  subscription id.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  total number of calls

SIDE EFFECTS
  CM Call Object will be modified.
===========================================================================*/
int cmcall_count_calls_with_call_type_per_as_id (

    cm_call_type_e_type call_type,
      /* call type */

    sys_modem_as_id_e_type   asubs_id
     /* sub for which calls are to be counted */
)
{
  int              total_calls = 0;
    /* counts the total number of calls */

  cm_iterator_type call_itr;
    /* call Object Iterator */

  cmcall_s_type    *call_ptr = NULL;
    /* call_ptr */

  boolean           ignore_asubs_id = FALSE;

  /* Ignore asubs_id for single sim */
  if (cmph_is_ssim())
    ignore_asubs_id = TRUE;

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( call_ptr->call_type == call_type
         &&
         ( call_ptr->asubs_id == asubs_id || ignore_asubs_id)
       )
    {
      ++total_calls;
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  return total_calls;

} /* cmcall_count_calls_with_call_type () */
#endif
#ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
/*===========================================================================

FUNCTION cmcall_redial_after_hard_failure

DESCRIPTION
  .

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

  This function is expected to be called during the origination process
  before call is connected.

RETURN VALUE

  Return TRUE, the new mode preference is changed.
  Otherwise, return FALSE.

  no_srv_time_ptr set to:
  A ballpark estimation for the number of seconds the phone is going
  to be without service. If mode is not forced, the no servie timer is set to
  0.

SIDE EFFECTS
  none

===========================================================================*/
boolean  cmcall_redial_after_hard_failure(

  cmcall_s_type    *call_ptr,
        /* pointer to a call object */

  dword            *no_srv_time_ptr
        /* Pointer to no service time */

)
{
  cmph_s_type           *ph_ptr             = cmph_ptr();
  sd_ss_pref_reas_e_type pref_reas          = SD_SS_PREF_REAS_REDIAL_OPTI;
  cm_ss_e_type           prev_ss            = CM_SS_MAIN;
  cm_orig_q_s_type      *computed_orig_para_on_orig_stack = NULL;
  cmmsc_state_machine_s_type *state_machine = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr           != NULL );
  CM_ASSERT( ph_ptr             != NULL);

  if (call_ptr == NULL || ph_ptr == NULL || no_srv_time_ptr == NULL)
  {
    CM_ERR_3("NULL call_ptr(%p) or ph_ptr(%p) or no_srv_time_prt(%p)",
           call_ptr, ph_ptr, no_srv_time_ptr);
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  state_machine = cmmsimc_state_machine_ptr(call_ptr->asubs_id);

  /* This handler of redial act hard failure ran is only valid for emergency
  ** calls.
  */
  CM_ASSERT( call_ptr->call_type == CM_CALL_TYPE_EMERGENCY );

  #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
  if( state_machine != NULL &&
      state_machine->stack_common_info.ue_mode == SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED )
  {
    /* hvolte req:
    ** switch to SRLTE after WPS call ends
    */
    call_ptr->switch_to_srlte = TRUE;
  }
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find the previously used origination parameters on ss on which call got
  ** originated
  */

  prev_ss = call_ptr->ss;

  computed_orig_para_on_orig_stack = cmtask_orig_para_search_act_id  (
                                       prev_ss,
                                       (cm_act_id_type) call_ptr->call_id );

  CM_ASSERT(computed_orig_para_on_orig_stack != NULL);

  if ( computed_orig_para_on_orig_stack == NULL )
  {
    CM_ERR_0("NULL pointer computed_orig_para_on_orig_stack");
    return FALSE;
  }

  /* Update call orig mode during redial */
  call_ptr->orig_mode =
                     cmph_map_cm_call_type_to_sd_orig_mode(
                                                          call_ptr->call_type,
                                                          call_ptr->ss );

  #if defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)
  if (call_ptr->csfb_type != CM_CALL_CSFB_TYPE_NONE)
  {
    cmcall_update_orig_mode (call_ptr);
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If call ptr ss is hybr 2 that would mean it is first hard failure RAN
  ** for SV, with LTE on hybrid-2 stack
  */

  if(call_ptr->ss == CM_SS_HYBR_2)
  {
    /* Change ss to main as call would be moved to main stack
    ** but while forcing the preferences s swould be changed to MAX
    ** as  hybrid prefrence would be turned off and preferences need to
    ** be forced on all the stacks
    */

    /* Pref_reason needs to be sent as orig_start_cs
    ** as on main stack emergency call preferences are being forced for the
    ** first time
    */
    pref_reas = SD_SS_PREF_REAS_ORIG_START_CS;

    /* In case if main stack has act type of priority more than emergency call
    ** in that case force top of main stack, but since emergency call as
    ** highest priority we can ignore this logic as of now
    */

    /* Toggle the hybrid preference to off */
    computed_orig_para_on_orig_stack->orig->orig_hybr_pref = CM_HYBR_PREF_OFF;

    call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_NONE;

    cmcall_set_favored_mode_pref_for_origination ( CM_SS_MAX,
                 call_ptr->call_type,
                 call_ptr->srv_type ,
                 TRUE, //drs bit - not used
                 #ifdef FEATURE_IS2000
                 call_ptr->cmcall_mode_info.info.cdma_info.srv_opt,//not used
                 #else
                 TRUE,
                 #endif
                 &call_ptr->favored_mode_pref );

    /* Find the SS to use for call origination  This would return CM_SS_MAX*/
    call_ptr->ss = cmcall_determine_ss_for_origination(
                                                 call_ptr->favored_mode_pref,
                                                 CM_HYBR_PREF_OFF,
                                                 CM_CC_MODE_PREF_ANY,
                                                 call_ptr );
  }
  else if(call_ptr->ss == CM_SS_MAIN || call_ptr->ss == CM_SS_MAX)
  {
    /* This could mean either it is 2nd or more hard failure RAN for SV
    ** or it could be any hard failure RAN on non -SV
    ** for both of these scenarios we need to send pref_reason as redial_opti
    ** as SD needs to enter the core and attempt call on next system in
    ** emergency scan list
    */
  }
  else
  {
    CM_ASSERT(0);
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2("Using ss %d for Redial after hard_failure_ran, favored_mode %d",
               call_ptr->ss,
               call_ptr->favored_mode_pref );

  call_ptr->end_status = CM_CALL_END_NONE;

  *no_srv_time_ptr = cmph_force_pref_on_the_fly(
                 call_ptr->ss,
                 ph_ptr,
                 pref_reas,
                 cmph_map_call_type_to_act_type( call_ptr->call_type ),
                 CM_PREF_TERM_CM_1_CALL_PLUS,
                 call_ptr->favored_mode_pref,
                 computed_orig_para_on_orig_stack->orig->orig_band_pref,
                 computed_orig_para_on_orig_stack->orig->orig_lte_band_pref,
                 computed_orig_para_on_orig_stack->orig->orig_tds_band_pref,
                 ph_ptr->main_stack_info.pref_info.plmn,
                 computed_orig_para_on_orig_stack->orig->orig_srv_domain_pref,
                 CM_NETWORK_SEL_MODE_PREF_NONE,
                 (cm_act_id_type) call_ptr->call_id,
                 CM_ACT_UPDATE_REAS_ACT_CONT,
                 TRUE,
                 cmph_get_acq_pri_order_per_sub(call_ptr->ss),
                 CM_DEFAULT_VALUE,
                 #if !defined(FEATURE_MMODE_DISABLE_CSG)
                 ph_ptr->main_stack_info.pref_info.csg_id,
                 #else
                 SYS_CSG_ID_INVALID,
                 #endif
                 ph_ptr->main_stack_info.pref_info.csg_rat,
                 computed_orig_para_on_orig_stack->orig->ciot_lte_pref
                 );


  return TRUE;
} /* cmcall_redial_after_hard_failure */

/*===========================================================================

FUNCTION cmcall_remove_mode_and_force

DESCRIPTION
  Removes the given mode_pref from the call's favored_mode_pref and forces
  the appropriate SS for call origination.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

  This function is expected to be called during the origination process
  before call is connected.

RETURN VALUE
  A ballpark estimation for the number of seconds the phone is going
  to be without service. If mode is not forced, the no servie timer is set to
  0.

  Return TRUE, the new mode preference is changed.
  Otherwise, return FALSE.

SIDE EFFECTS
  none

===========================================================================*/
boolean  cmcall_remove_mode_and_force(

  cmcall_s_type    *call_ptr,
        /* pointer to a call object */

  sd_ss_mode_pref_e_type mode_pref_to_remove,
        /* CM mode pref to be removed */

  dword            *no_srv_time_ptr
        /* Pointer to no service time */

)
{
  cmph_s_type           *ph_ptr             = cmph_ptr();

  sd_ss_pref_reas_e_type pref_reas          = SD_SS_PREF_REAS_REDIAL_NORM;

  boolean                force_new_mode     = TRUE;

  cm_orig_q_s_type      *computed_orig_para = NULL;

  cm_mode_pref_e_type    new_mode_pref      = CM_MODE_PREF_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr           != NULL );
  CM_ASSERT( ph_ptr             != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Compute new mode pref */
  new_mode_pref = cm_remove_mode_pref_components(
                      call_ptr->favored_mode_pref,
                      1,
                        mode_pref_to_remove
                    );

  if( new_mode_pref == CM_MODE_PREF_NONE )
  {
    CM_MSG_HIGH_0("No new mode forcing, mode_pref None");
    force_new_mode = FALSE;
  }

  /* If we decide to force with the new mode pref, check if the other mode
  ** prefs can be used for origination without any errors.
  */
  if( force_new_mode )
  {
    if( cmcall_process_orig_mode(call_ptr->main_ss_mode, call_ptr) !=
          CM_CALL_CMD_ERR_NOERR )
    {
      CM_MSG_HIGH_0("No new mode forcing, process orig mode Err");
      force_new_mode = FALSE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find the previously used origination parameters */

  computed_orig_para =  cmtask_orig_para_search_act_id  (
                                                     call_ptr->ss,
                                    (cm_act_id_type) call_ptr->call_id );



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no other mode left for origination, might as well retry on the
  ** original mode pref
  */
  if( !force_new_mode )
  {
    #if defined(FEATURE_MMODE_DUAL_SIM)
    if( call_ptr->ss == CM_SS_HYBR_2 && cmph_is_msim() && !cmph_is_sxlte()
        && hybr_2_pref_ptr)
    {
      hybr_2_pref_ptr->srv_domain_pref =
                        ( computed_orig_para != NULL ?
                          computed_orig_para->orig->orig_srv_domain_pref :
                          CMPH_HYBR_2_PREF(ph_ptr).srv_domain_pref
                        );
      hybr_2_pref_ptr->network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_NONE;
      hybr_2_pref_ptr->pref_term = CM_PREF_TERM_CM_1_CALL_PLUS;
      hybr_2_pref_ptr->mode_pref = CM_MODE_PREF_NO_CHANGE;
      hybr_2_pref_ptr->band_pref = CM_BAND_PREF_NO_CHANGE;
      hybr_2_pref_ptr->prl_pref  = CM_PRL_PREF_NO_CHANGE;
      hybr_2_pref_ptr->roam_pref = CM_ROAM_PREF_NO_CHANGE;
      hybr_2_pref_ptr->hybr_pref = CM_HYBR_PREF_NO_CHANGE;
      hybr_2_pref_ptr->ciot_lte_pref.lte_op_mode_pref = CM_LTE_IOT_OP_MODE_PREF_NO_CHANGE;
      hybr_2_pref_ptr->ciot_lte_pref.lte_m1_band_pref = SYS_LTE_BAND_MASK_CONST_NO_CHG;
      hybr_2_pref_ptr->ciot_lte_pref.lte_nb1_band_pref = SYS_LTE_BAND_MASK_CONST_NO_CHG;
      hybr_2_pref_ptr->ciot_lte_pref.lte_band_pref = SYS_LTE_BAND_MASK_CONST_NO_CHG;
    }
    #endif

    #ifdef FEATURE_MMODE_HYBR3_ENABLED

    if( call_ptr->ss == CM_SS_HYBR_3 && hybr_3_pref_ptr)
    {
      hybr_3_pref_ptr->srv_domain_pref =
                        ( computed_orig_para != NULL ?
                          computed_orig_para->orig->orig_srv_domain_pref :
                          CMPH_HYBR_3_PREF(ph_ptr).srv_domain_pref
                        );
      hybr_3_pref_ptr->network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_NONE;
      hybr_3_pref_ptr->pref_term = CM_PREF_TERM_CM_1_CALL_PLUS;
      hybr_3_pref_ptr->mode_pref = CM_MODE_PREF_NO_CHANGE;
      hybr_3_pref_ptr->band_pref = CM_BAND_PREF_NO_CHANGE;
      hybr_3_pref_ptr->prl_pref  = CM_PRL_PREF_NO_CHANGE;
      hybr_3_pref_ptr->roam_pref = CM_ROAM_PREF_NO_CHANGE;
      hybr_3_pref_ptr->hybr_pref = CM_HYBR_PREF_NO_CHANGE;
      hybr_3_pref_ptr->ciot_lte_pref.lte_op_mode_pref = CM_LTE_IOT_OP_MODE_PREF_NO_CHANGE;
      hybr_3_pref_ptr->ciot_lte_pref.lte_m1_band_pref = SYS_LTE_BAND_MASK_CONST_NO_CHG;
      hybr_3_pref_ptr->ciot_lte_pref.lte_nb1_band_pref = SYS_LTE_BAND_MASK_CONST_NO_CHG;
      hybr_3_pref_ptr->ciot_lte_pref.lte_band_pref = SYS_LTE_BAND_MASK_CONST_NO_CHG;
    }
    #endif /* FEATURE_MMODE_DUAL_SIM */

    *no_srv_time_ptr = cmph_force_pref_on_the_fly2
          (
           call_ptr->ss,
           ph_ptr,
           pref_reas,
           cmph_map_call_type_to_act_type( call_ptr->call_type ),
           CM_PREF_TERM_CM_1_CALL_PLUS,
           CM_MODE_PREF_NO_CHANGE,
           CM_BAND_PREF_NO_CHANGE,
           SYS_LTE_BAND_MASK_CONST_NO_CHG,
           CM_BAND_PREF_NO_CHANGE,
           ph_ptr->main_stack_info.pref_info.plmn,
           ( computed_orig_para != NULL ?
                       computed_orig_para->orig->orig_srv_domain_pref :
                       ph_ptr->main_stack_info.pref_info.srv_domain_pref
           ),
           CM_NETWORK_SEL_MODE_PREF_NONE,
           (cm_act_id_type) call_ptr->call_id,
           CM_ACT_UPDATE_REAS_ACT_START,
           call_ptr->force_top_pref,
           cmph_get_acq_pri_order_per_sub(call_ptr->ss),
           CM_DEFAULT_VALUE,
           #if !defined(FEATURE_MMODE_DISABLE_CSG)
           ph_ptr->main_stack_info.pref_info.csg_id,
           #else
           SYS_CSG_ID_INVALID,
           #endif
           ph_ptr->main_stack_info.pref_info.csg_rat,
           ph_ptr->main_stack_info.pref_info.ciot_lte_pref
           );


    /* The value is always FALSE
    */
    CM_ASSERT( !force_new_mode );
    return force_new_mode;

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Recompute favored mode pref for call after cmcall_process_orig_mode() */
  call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                  call_ptr->favored_mode_pref,
                                  1,
                                    mode_pref_to_remove
                                );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find the SS to use for call origination */

  call_ptr->ss = cmcall_determine_ss_for_origination(
                                                 call_ptr->favored_mode_pref,
                                                 CM_HYBR_PREF_OFF,
                                                 CM_CC_MODE_PREF_MAX,
                                                 call_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_3("Using ss %d for Redial after removing mode_pref %d,favored_mode %d",
                                                       call_ptr->ss,
                                                       mode_pref_to_remove,
                                                       call_ptr->favored_mode_pref );

  /*For CSFB orig, use orig mode start_cs */
  if((BM(call_ptr->orig_mode) & SD_SS_ORIG_MODE_VAL_CSFB_ORIG) &&
     cmcall_misc_is_mode_pref(call_ptr->favored_mode_pref,CM_MODE_PREF_LTE_ONLY))
  {
     pref_reas = SD_SS_PREF_REAS_ORIG_START_CS;
  }

  /*After VOLTE hard FAILURE, call should be treated as regular GW e911 call */
  if((call_ptr->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG) &&
     ((call_ptr->end_status == CM_CALL_END_IP_FAIL) ||
       (call_ptr->end_status == CM_CALL_END_IP_END)))
  {
    pref_reas = SD_SS_PREF_REAS_ORIG_START_CS;
    CM_MSG_HIGH_0( "Marking pref_reason forcefully to START_CS due to VOLTE hard failure");
  }

  *no_srv_time_ptr = cmph_force_pref_on_the_fly2(
                       call_ptr->ss,
                       ph_ptr,
                       pref_reas,
                       cmph_map_call_type_to_act_type( call_ptr->call_type ),
                       CM_PREF_TERM_CM_1_CALL_PLUS,
                       call_ptr->favored_mode_pref,
                       ( computed_orig_para != NULL ?
                                   computed_orig_para->orig->orig_band_pref :
                                   CMPH_SS_BAND_PREF(call_ptr->asubs_id, ph_ptr)
                       ),
                       ( computed_orig_para != NULL ?
                                   computed_orig_para->orig->orig_lte_band_pref :
                                   CMPH_SS_LTE_BAND_PREF(call_ptr->asubs_id, ph_ptr)
                       ),
                       ( computed_orig_para != NULL ?
                                   computed_orig_para->orig->orig_tds_band_pref :
                                   CMPH_SS_TDS_BAND_PREF(call_ptr->asubs_id, ph_ptr)
                       ),
                       ph_ptr->main_stack_info.pref_info.plmn,
                       ( computed_orig_para != NULL ?
                                   computed_orig_para->orig->orig_srv_domain_pref :
                                   CMPH_SS_SRV_DOMAIN_PREF(call_ptr->asubs_id, ph_ptr)
                       ),
                       CM_NETWORK_SEL_MODE_PREF_NONE,
                       (cm_act_id_type) call_ptr->call_id,
                       CM_ACT_UPDATE_REAS_ACT_CONT,
                       TRUE,
                       cmph_get_acq_pri_order_per_sub(call_ptr->ss),
                       CM_DEFAULT_VALUE,
                       #if !defined(FEATURE_MMODE_DISABLE_CSG)
                       ph_ptr->main_stack_info.pref_info.csg_id,
                       #else
                       SYS_CSG_ID_INVALID,
                       #endif
                       ph_ptr->main_stack_info.pref_info.csg_rat,
                       ( computed_orig_para != NULL ?
                                   computed_orig_para->orig->ciot_lte_pref
 :
                                   CMPH_SS_CIOT_LTE_PREF(call_ptr->asubs_id, ph_ptr)
                       )
                       );


  CM_ASSERT( force_new_mode );

  /* Always return TRUE
  */

  return force_new_mode;
}
#endif


/*===========================================================================

FUNCTION cmcall_is_lte_only_mode
DESCRIPTION
  Validating if lte mode is supported for the band and mode pref combination

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  CMD ERR

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_lte_only_mode
(
  cm_mode_pref_e_type    mode_pref,

  cm_band_pref_e_type band_pref,

  sys_lte_band_mask_e_type lte_band_pref,

  cm_band_pref_e_type tds_band_pref

)
{
 if((sd_ss_get_supp_mode_pref(
                  cmph_map_cm_mode_pref_to_sd_mode_pref(mode_pref),
                  cmph_map_cm_band_pref_to_sd_band_pref(band_pref),
                  lte_band_pref,
                  cmph_map_cm_tds_band_pref_to_sd_tds_band_pref(tds_band_pref))& SD_SS_MODE_PREF_DIGITAL_LESS_HDR) == SD_SS_MODE_PREF_LTE)
 {
     return TRUE;
 }
   return FALSE;
}

#ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
/*===========================================================================

FUNCTION  cmcall_cmd_orig_para_check

DESCRIPTION
  Check parameter errors for call origination command

DEPENDENCIES
  none
RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
static boolean  cmcall_is_call_allowed_in_offline_mode
(
  cmcall_s_type       *call_ptr
)
{
  cmph_p_type ph_ptr                    = cmph_ptr();
  boolean ret_val = TRUE;

  CM_ASSERT( call_ptr     != NULL );
  CM_ASSERT( ph_ptr     != NULL );
  
  /* if voice_in_wifi_only feature is defined then only allow 
  voice/VT/VS calls */

#if 0
  if(cmph_get_sub_config_ptr(call_ptr->asubs_id)->voice_in_wifi_only == TRUE)
  {
      if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE   ||
          cm_reply_check (&ph_ptr->reply, CM_LPM_OK_F)||
          cm_reply_check (&ph_ptr->reply, CM_RESET_OK_F) ||
          cm_reply_check (&ph_ptr->reply, CM_PWROFF_OK_F)
        ) 
      {
        /* if WLAN APP of IMS is registered to CM for call type Voice VT and VS
        then allow the call else do not */
        if( (call_ptr->call_type == CM_CALL_TYPE_VOICE ||
             call_ptr->call_type == CM_CALL_TYPE_VT ||
             call_ptr->call_type == CM_CALL_TYPE_VS) &&
            cmcall_is_active_ip_app(SYS_SYS_MODE_WLAN, call_ptr->call_type))
        {
          ret_val = TRUE;     
        }
        else
        {
          ret_val = FALSE;
        }
       
      }
  }
  else
#endif      
  {

    if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE   ||
          cm_reply_check (&ph_ptr->reply, CM_LPM_OK_F)||
          cm_reply_check (&ph_ptr->reply, CM_RESET_OK_F) ||
          cm_reply_check (&ph_ptr->reply, CM_PWROFF_OK_F)
        )
    {
      ret_val = FALSE;
    }    
  }
  return ret_val;   

}
#endif

#if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
/*===========================================================================

FUNCTION  cmcall_cmd_orig_para_check

DESCRIPTION
  Check parameter errors for call origination command

DEPENDENCIES
  none
RETURN VALUE
  CM_CALL_CMD_ERR_NOERR if no parameter related errors,
  otherwise specific cm_call_cmd_err_e_type.

SIDE EFFECTS
  none

===========================================================================*/
cm_call_cmd_err_e_type  cmcall_cmd_orig_para_check
(
  cm_call_cmd_s_type       *call_cmd_ptr

)
{
  cm_call_cmd_err_e_type  cmd_err       = CM_CALL_CMD_ERR_NOERR;
  cm_call_cmd_err_e_type  cdma_cmd_err  = CM_CALL_CMD_ERR_NOERR;
  cm_call_cmd_err_e_type  gw_cmd_err    = CM_CALL_CMD_ERR_NOERR;

  cm_call_cmd_info_s_type *cmd_info_ptr = NULL;
  cm_ss_e_type            ss            = CM_SS_MAIN;

  cmph_p_type ph_ptr                    = cmph_ptr();

  cmcall_s_type           *call_ptr     = NULL;
  cm_orig_q_s_type        *ph_orig_para_top_ptr = NULL;
    /* The pointer to the top element of the priority queue */
  boolean                      is_call_allowed   = TRUE;
  sys_call_mode_t          call_mode_buffer = SYS_CALL_MODE_NORMAL;
  cmph_sub_config_s_type *ph_sub_config_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_cmd_ptr     != NULL );
  CM_ASSERT( ph_ptr           != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_info_ptr = CALL_CMD_INFO_PTR(call_cmd_ptr);;

  #ifdef FEATURE_MMODE_DUAL_SIM
  ss            = cmph_map_subs_to_ss(call_cmd_ptr->cmd_subs);
  #endif

  ph_orig_para_top_ptr = cmtask_orig_para_get_top( ss );

  /* Perform basic checks for a call origination.
  */
  call_ptr = cmcall_ptr( call_cmd_ptr->info.call_id );

  if(call_ptr != NULL)
  {
    ph_sub_config_ptr = cmph_get_sub_config_ptr(call_ptr->asubs_id);

    if(ph_sub_config_ptr == NULL)
    {
      return CM_CALL_CMD_ERR_CALL_ID_P;
    }
    #if defined(FEATURE_MMODE_3GPP2)
    CM_MSG_HIGH_1("Allow SMS in ECBM %d",
      ph_sub_config_ptr->is_allow_sms_in_ecbm_enabled);
    #endif
  }

  /* hack, need to use new NV here, and only check if call is voice call 
  will introduce one function if call should be rejected due to offlien errors */
  if( call_ptr == NULL )
  {
    return CM_CALL_CMD_ERR_CALL_ID_P;
  }
  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  else if(!cmcall_is_call_allowed_in_offline_mode(call_ptr))
  {
    return CM_CALL_CMD_ERR_OFFLINE_S;
  }
  #endif
  else if( !cmcall_is_call_type_valid(call_cmd_ptr->info.call_type) )
  {
    return CM_CALL_CMD_ERR_CALL_TYPE_P;
  }
  #if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE) && defined(FEATURE_MMODE_3GPP2)
  else if(cmph_is_in_emergency_cb()&&
             (cmd_info_ptr->call_type != CM_CALL_TYPE_EMERGENCY))
  {
     /* Let SMS call go through in ECBM if is_allow_sms_in_ecbm_enabled is set */
    if ((!(cmd_info_ptr->call_type == CM_CALL_TYPE_SMS &&\
       ph_sub_config_ptr->is_allow_sms_in_ecbm_enabled == TRUE))
     )
    {
      CM_MSG_HIGH_1 ("Call type %d not supported",cmd_info_ptr->call_type);
      return CM_CALL_CMD_ERR_EMERGENCY_MODE_S;
    }
  }
  #endif
  #ifdef FEATURE_MMODE_SC_SGLTE
  else if( (ph_ptr->is_sglte_device) &&
           (ph_ptr->main_stack_info.pref_info.network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_LIMITED_SRV ||
            ph_ptr->main_stack_info.pref_info.network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_AUTO_LIMITED_SRV ||
            ph_ptr->main_stack_info.pref_info.network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL_LIMITED_SRV) &&
           (cmd_info_ptr->call_type != CM_CALL_TYPE_EMERGENCY) )
  {
    CM_MSG_HIGH_2 ("Call type %d not supported in Limited_Srv mode %d",
                 cmd_info_ptr->call_type,
                 ph_ptr->main_stack_info.pref_info.network_sel_mode_pref);

    return CM_CALL_CMD_ERR_EMERGENCY_MODE_S;
  }
  else if((ph_orig_para_top_ptr != NULL) &&
     (cmd_info_ptr->call_type != CM_CALL_TYPE_EMERGENCY) &&
     (cmtask_orig_para_search_act_type(ss, CM_ACT_TYPE_POLICYMAN_RESTRICT) != NULL) &&
     (ph_orig_para_top_ptr->act_type == CM_ACT_TYPE_EMERG_CALL) &&
     (ph_orig_para_top_ptr->orig->act_priority == CM_ACT_PRIORITY_END) &&
     (cmph_is_subs_feature_mode_sglte(call_cmd_ptr->cmd_subs)))
  {

    /* CM should reject all calls except emergency calls while policyman restric mode object is in the priority queue
    and emergency call is present with priority end. If we don't do this then priority queue insertions will not be
    as per priority and CM may not transfer the UE to dual stack mode while unforcing emergencyu call preferences
    DATA call will be retried  from APPS if always ON DATA is set */

    CM_MSG_HIGH_3 ("Call type %d not supported in policyman restric mode %d in E911 call end case %d",
                 cmd_info_ptr->call_type, ph_orig_para_top_ptr->act_type,
                 ph_orig_para_top_ptr->orig->act_priority);

      return CM_CALL_CMD_ERR_EMERGENCY_MODE_S;
  }
  #endif
  else if((cmcall_is_there_a_call_type_ss( CM_SS_MAX,
                                         CM_CALL_TYPE_EMERGENCY,
                                         call_cmd_ptr->info.call_id) != CM_CALL_ID_INVALID ) &&
         (cmd_info_ptr->call_type == CM_CALL_TYPE_VOICE||
          cmd_info_ptr->call_type == CM_CALL_TYPE_VT ||
          cmd_info_ptr->call_type == CM_CALL_TYPE_VS))
  /*data call has reject cause CM_CALL_CMD_ERR_IN_USE_S */
  {
    return CM_CALL_CMD_ERR_EMERGENCY_MODE_S;
  }
  #ifndef FEATURE_TRIM_CMPMPRX
  if((call_cmd_ptr->info.call_type == CM_CALL_TYPE_EMERGENCY) && (cmpmprx_is_special_volte_e911_calls() == TRUE) )
  {
    /* if special_volte_e911_calls is enabled, and call mode is volte only.
      ** check if call is allowed or not
      */
    /* since this is CSFB device, using AS_ID_1 to check if call can be allowed or not */
    is_call_allowed = cmcall_check_if_emer_call_allowed(call_cmd_ptr->cmd_subs);
    if(is_call_allowed == FALSE)
    {
      cmd_err = CM_CALL_CMD_ERR_INVALID_SIM_STATE_S;
      CM_MSG_HIGH_1("E911_LTE_ONLY: Reject Emerg call asubid=%d", call_cmd_ptr->cmd_subs);
      return CM_CALL_CMD_ERR_INVALID_SIM_STATE_S;
    }
  }
  #endif

  if(ph_ptr->main_stack_info.pref_info.network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL  &&
     cmd_info_ptr->call_type != CM_CALL_TYPE_EMERGENCY &&
     (cmd_info_ptr->srv_type == CM_SRV_TYPE_CDMA_SPECIFIC ||
      cmd_info_ptr->srv_type == CM_SRV_TYPE_CDMA_AUTOMATIC))
  {
    CM_MSG_HIGH_1 ("CDMA Specific/Auto srv type not supported in manual mode for call type %d",
                 cmd_info_ptr->call_type);

    return CM_CALL_CMD_ERR_SRV_TYPE_S;
  }

  #if !defined(FEATURE_MMODE_DISABLE_ACCESS_CONTROL)
  cmd_err = cmac_call_cmd_check(call_cmd_ptr);
  #endif

  if ( cmd_err != CM_CALL_CMD_ERR_NOERR )
  {
    return cmd_err;
  }

  #ifdef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
    
  /* Reject voice oe EMergency call on LTE_ONLY or LTE+GPRS only 
  ** if VoLTE is not possible
  */
  if((cmd_info_ptr->call_type ==  CM_CALL_TYPE_VOICE ||
     cmd_info_ptr->call_type ==  CM_CALL_TYPE_CS_DATA ||
     cmd_info_ptr->call_type ==  CM_CALL_TYPE_VT ||
     cmd_info_ptr->call_type ==  CM_CALL_TYPE_EMERGENCY ) 
     #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
     && !cmcall_is_ipcall_over_mode_poss(SYS_SYS_MODE_LTE,  call_ptr) 
     #endif
  )
  {
    CM_MSG_HIGH_0 ("Reject the call as UE set to LTE only config and VoLTE not possible");
    return CM_CALL_CMD_ERR_LTE_ONLY_NO_IMS;
  }
  #endif

  #if defined(FEATURE_CM_LTE)
  /* Reject voice (normal and emergency) call when UE
  ** is set to LTE only mode and VoLTE/VoWLAN is not possible
  */
  if((cmd_info_ptr->call_type ==  CM_CALL_TYPE_VOICE ||
     cmd_info_ptr->call_type ==  CM_CALL_TYPE_CS_DATA ||
     cmd_info_ptr->call_type ==  CM_CALL_TYPE_VT )&&
     cmcall_is_lte_only_mode(CMPH_SS_MODE_PREF(call_cmd_ptr->cmd_subs, ph_ptr),
     CMPH_SS_BAND_PREF(call_cmd_ptr->cmd_subs, ph_ptr), CMPH_SS_LTE_BAND_PREF(call_cmd_ptr->cmd_subs, ph_ptr),
     CMPH_SS_TDS_BAND_PREF(call_cmd_ptr->cmd_subs, ph_ptr)) 
     #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
     && (!cmcall_is_ipcall_over_mode_poss(SYS_SYS_MODE_LTE,  call_ptr) &&
         !cmcall_is_ipcall_over_mode_poss(SYS_SYS_MODE_WLAN, call_ptr))
     #endif
     )
  {
    CM_MSG_HIGH_0 ("Reject the call as UE set to LTE only and VoLTE/VoWLAN not possible");
    return CM_CALL_CMD_ERR_LTE_ONLY_NO_IMS;

  }
  #endif
  #if defined FEATURE_CM_LTE || defined FEATURE_IP_CALL

  if((cmd_info_ptr->srv_type == CM_SRV_TYPE_PS_ONLY) &&
     (cmd_info_ptr->call_type ==  CM_CALL_TYPE_VOICE ) &&
     ((cmcall_is_active_ip_app(SYS_SYS_MODE_LTE,CM_CALL_TYPE_VOICE) == FALSE)&&
      (cmcall_is_active_ip_app(SYS_SYS_MODE_WLAN,CM_CALL_TYPE_VOICE)  == FALSE) &&
          (cmcall_is_active_ip_app(SYS_SYS_MODE_HDR,CM_CALL_TYPE_VOICE) == FALSE)))
  {
      CM_MSG_HIGH_0 ("Reject the call as UE as ipapp cap does not support voice call");
      return CM_CALL_CMD_ERR_SRV_TYPE_P;

  }

  if((cmd_info_ptr->srv_type == CM_SRV_TYPE_PS_ONLY)   &&
     (cmd_info_ptr->call_type ==  CM_CALL_TYPE_VT ) &&
     ((cmcall_is_active_ip_app(SYS_SYS_MODE_LTE, CM_CALL_TYPE_VT)  == FALSE) &&
      (cmcall_is_active_ip_app(SYS_SYS_MODE_WCDMA,CM_CALL_TYPE_VT) == FALSE) &&
      (cmcall_is_active_ip_app(SYS_SYS_MODE_WLAN,CM_CALL_TYPE_VT)  == FALSE)))
  {
      CM_MSG_HIGH_0 ("Reject the call as UE as ipapp cap does not support VT call");
      return CM_CALL_CMD_ERR_SRV_TYPE_P;


  }

  #if defined(FEATURE_CIOT)
  if(((cmd_info_ptr->call_type ==  CM_CALL_TYPE_VT ) ||
      (cmd_info_ptr->call_type ==  CM_CALL_TYPE_VOICE)) &&
     (sys_srv_status_is_srv(cmss_intl_srv_info_ptr(CM_SS_MAIN)->srv_status)) &&
     (cmss_intl_srv_info_ptr(CM_SS_MAIN)->mode == SD_MODE_LTE_NB1)
    )
  {
      CM_MSG_HIGH_0 ("Reject the call as UE is camped on NB1");
      return CM_CALL_CMD_ERR_SRV_TYPE_P;
  }
  #endif
  
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( call_cmd_ptr->info.is_cdma_info_present)
  {
    #if (defined (FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
    cdma_cmd_err =  cmxcall_call_cmd_orig_para_check(call_cmd_ptr);
    #endif

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( (call_cmd_ptr->info.is_gsm_wcdma_ps_info_present ||
       call_cmd_ptr->info.is_gsm_wcdma_cs_info_present))
  {
    #if defined(CM_GW_SUPPORTED)
    gw_cmd_err =  cmwcall_call_cmd_orig_para_check(call_cmd_ptr);
    #endif
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( cdma_cmd_err == CM_CALL_CMD_ERR_NOERR &&   /*lint !e774 */
      gw_cmd_err   != CM_CALL_CMD_ERR_NOERR)     /*lint !e774 */
  {
    cmd_err = gw_cmd_err;
  }
  else
  {
    cmd_err = cdma_cmd_err;
  }

  return cmd_err;
}

/*===========================================================================

FUNCTION cmcall_process_orig_mode

DESCRIPTION
  Process call command pertaining to CDMA or GW Orig Mode.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  CMD ERR

SIDE EFFECTS
  none

===========================================================================*/
cm_call_cmd_err_e_type cmcall_process_orig_mode
(
  cm_cc_mode_pref_e_type    mode_usage,
  cmcall_s_type            *call_ptr

)
{
  cm_call_cmd_err_e_type  cmd_err      = CM_CALL_CMD_ERR_NOERR;

  #if ((defined (FEATURE_MMODE_CDMA_800) || defined (FEATURE_MMODE_CDMA_1900)) && \
        (defined CM_GW_SUPPORTED))
  cm_call_cmd_err_e_type  cdma_cmd_err = CM_CALL_CMD_ERR_NOERR;
  cm_call_cmd_err_e_type  gw_cmd_err   = CM_CALL_CMD_ERR_NOERR;
  #endif

  CM_MSG_HIGH_1("cmcall_process_orig_mode mode_usage=%d",mode_usage);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  if(call_ptr->is_ip_call || call_ptr->is_vowlan)
  {
    call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_IP;
    call_ptr->main_ss_mode = mode_usage;
    return CM_CALL_CMD_ERR_NOERR;
  }
  #endif

  switch (mode_usage)
  {
    #if (defined (FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
    case CM_CC_MODE_PREF_CDMA_ONLY:
      call_ptr->main_ss_mode = CM_CC_MODE_PREF_CDMA_ONLY;
      call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_CDMA;


      #if defined (FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
      if(call_ptr->is_volte)
      {
        call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_IP;
        call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                         call_ptr->favored_mode_pref,
                                         2,
                                         SD_SS_MODE_PREF_TDS_GSM_WCDMA,
                                         SD_SS_MODE_PREF_HDR);
            CM_MSG_HIGH_2("proc_orig_mode: favor_mode=%d, main_ss_mode=%d", call_ptr->favored_mode_pref, call_ptr->main_ss_mode);

            if(call_ptr->favored_mode_pref == CM_MODE_PREF_NONE)
                  {
                    cmd_err = CM_CALL_CMD_ERR_MODE_PREF_P;
                  }
        break;
      }
      #endif

      /* Remove GW from the favored mode pref.
      */
      call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                         call_ptr->favored_mode_pref,
                                         1,
                                         SD_SS_MODE_PREF_TDS_GSM_WCDMA_LTE);

    cmd_err = cmxcall_client_orig_proc (call_ptr);
      break;
    #endif  /* FEATURE_MMODE_CDMA_800 || FEATURE_MMODE_CDMA_1900 */

    case CM_CC_MODE_PREF_GW_ONLY:
      call_ptr->main_ss_mode = CM_CC_MODE_PREF_GW_ONLY;
      #if defined(FEATURE_MMODE_GW)
      if (call_ptr->call_type == CM_CALL_TYPE_PS_DATA)
      {
        call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_PS;
      }
      else
      {
        call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_CS;
      }
      #endif
      #if defined(FEATURE_MMODE_GW)
      cmd_err = cmwcall_client_orig_proc (call_ptr);
      #endif

      #if defined (FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
      if(call_ptr->is_volte )
      {
        call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_IP;
        /* Remove CDMA/AMPS/HDR from the favored mode pref.
        */
        call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                             call_ptr->favored_mode_pref,
                                             2,
                                             SD_SS_MODE_PREF_HDR,
                                             SD_SS_MODE_PREF_CDMA                                             
                                             );
        CM_MSG_HIGH_2("proc_orig_mode: favor_mode=%d, main_ss_mode=%d", call_ptr->favored_mode_pref, call_ptr->main_ss_mode);
        if(call_ptr->favored_mode_pref == CM_MODE_PREF_NONE)
        {
          cmd_err = CM_CALL_CMD_ERR_MODE_PREF_P;
        }
        break;

      }
      #endif

      /* Remove CDMA/AMPS/HDR from the favored mode pref.
      */
      #if defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)
      /* Do not remove LTE mode preference for voice call if 3GPP CSFB is supported
      */
      if( call_ptr->csfb_type == CM_CALL_CSFB_TYPE_PPCSFB)
      {
        CM_MSG_HIGH_0("CSFB:mode_usage = GW and PPCSFB is possible so removing CDMA and HDR modes");
        call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                             call_ptr->favored_mode_pref,
                                             2,
                                             SD_SS_MODE_PREF_HDR,
                                             SD_SS_MODE_PREF_CDMA
                                             );
      }
      else if (call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB)
      {
        /* TODO: Need to check if HDR acquisition need to be throttled
        */
        CM_MSG_HIGH_0("CSFB:mode_usage = GW and 1XCSFB is possible so removing HDR modes");
        call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                             call_ptr->favored_mode_pref,
                                             2,
                                             SD_SS_MODE_PREF_TDS_GSM_WCDMA,
                                             SD_SS_MODE_PREF_HDR);

        call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_CDMA;
      }
      else
      #endif /* FEATURE_3GPP_CSFB */
      {
        /* Remove LTE only if call type = voice/cs_data if LTE feature is defined
            */
        #if defined FEATURE_CM_LTE
        if (!(call_ptr->call_type == CM_CALL_TYPE_VOICE||
              call_ptr->call_type == CM_CALL_TYPE_PD||
              call_ptr->call_type == CM_CALL_TYPE_CS_DATA))
        {
          call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                             call_ptr->favored_mode_pref,
                                             2,
                                             SD_SS_MODE_PREF_HDR,
                                             SD_SS_MODE_PREF_CDMA
                                             );
        }
        else
        #endif
        {
          call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                             call_ptr->favored_mode_pref,
                                             2,
                                             SD_SS_MODE_PREF_CDMA_HDR,
                                             SD_SS_MODE_PREF_LTE
                                             );
        }
      }


      if(call_ptr->favored_mode_pref == CM_MODE_PREF_NONE)
      {
        cmd_err = CM_CALL_CMD_ERR_MODE_PREF_P;
      }
      break;

    case CM_CC_MODE_PREF_ANY:
      #if ((defined (FEATURE_MMODE_CDMA_800) || defined (FEATURE_MMODE_CDMA_1900)) && \
            (defined FEATURE_MMODE_3GPP) )
      cdma_cmd_err = cmxcall_client_orig_proc (call_ptr);

      /* At this point, if the cdma_cmd_err returned is CM_CALL_CMD_ERR_IN_USE_S
      ** we know that there is another on-going MO CDMA call. It is safe to assume
      ** here that second call cannot be originated on GW on the same subscrition
      ** so we can return from here.
      ** We have already verified that second call is an MO call on the same SS
      ** as the first call. We verify here that srv is CDMA and return from here.
      */

      if( cdma_cmd_err == CM_CALL_CMD_ERR_IN_USE_S &&
          cmss_ptr()->last_system_reported == SYS_SYS_MODE_CDMA &&
          cmss_ptr()->main_srv_status == SYS_SRV_STATUS_SRV
        )
      {
        CM_MSG_HIGH_3("SS Info: Last Sys rpted %d, main_srv_status %d, CDMA_CMD_ERR %d, Another MO Call in Progress",
                     cmss_ptr()->last_system_reported,
                      cmss_ptr()->main_srv_status, cdma_cmd_err);
        return cdma_cmd_err;
      }

#if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE) && !defined(FEATURE_MMODE_LTE_ONLY) && !defined(FEATURE_MMODE_GPRS_LTE)
      gw_cmd_err = cmwcall_client_orig_proc (call_ptr);
#endif
      call_ptr->main_ss_mode = CM_CC_MODE_PREF_ANY;

      CM_MSG_HIGH_2 ("cdma_cmd_err=%d gw_cmd_err=%d", cdma_cmd_err, gw_cmd_err);

      #if defined (FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
      if(call_ptr->is_volte)
      {
        call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_IP;
        CM_MSG_HIGH_2("proc_orig_mode: favor_mode=%d, main_ss_mode=%d", call_ptr->favored_mode_pref, call_ptr->main_ss_mode);
        return CM_CALL_CMD_ERR_NOERR;
      }
      #endif

      #if defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)
      if ((cdma_cmd_err == CM_CALL_CMD_ERR_NOERR) &&
          (gw_cmd_err == CM_CALL_CMD_ERR_NOERR))
      {

        /* Do not remove LTE mode preference for voice call if 3GPP CSFB is supported
        */
        if( call_ptr->csfb_type == CM_CALL_CSFB_TYPE_PPCSFB)
        {
          CM_MSG_HIGH_0("CSFB:mode_usage = ANY and PPCSFB is possible so removing CDMA and HDR modes");
          call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                            call_ptr->favored_mode_pref,
                                             2,
                                             SD_SS_MODE_PREF_HDR,
                                             SD_SS_MODE_PREF_CDMA
                                             );
         call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_CS;
        }
        else if (call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB)
        {
          /* TODO: Need to check if HDR acquisition need to be throttled
          */
          CM_MSG_HIGH_0("CSFB:mode_usage = ANY and 1XCSFB is possible so removing HDR modes");
          call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                             call_ptr->favored_mode_pref,
                                               2,
                                               SD_SS_MODE_PREF_TDS_GSM_WCDMA,
                                               SD_SS_MODE_PREF_HDR);

          call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_CDMA;
        }
        else if ((call_ptr->call_type == CM_CALL_TYPE_VOICE)||
                 (call_ptr->call_type == CM_CALL_TYPE_CS_DATA)||
                 (call_ptr->call_type == CM_CALL_TYPE_PD)
                )
        {
          /* Remove LTE from the favored mode pref. */
          if ( sd_misc_is_mode_pref(cmph_map_cm_mode_pref_to_sd_mode_pref(call_ptr->favored_mode_pref),
                         SD_SS_MODE_PREF_LTE) == TRUE)
          {
            CM_MSG_HIGH_0("CSFB:mode_usage = ANY and CSFB is not possible so removing LTE");

            call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                         call_ptr->favored_mode_pref,
                                         1,
                                         SD_SS_MODE_PREF_LTE);
          }
        }

        if(call_ptr->favored_mode_pref == CM_MODE_PREF_NONE)
        {
          cmd_err = CM_CALL_CMD_ERR_MODE_PREF_P;
        }
        break;

      }
      #endif /* (FEATURE_3GPP_CSFB) ||  (FEATURE_LTE_TO_1X) */
      if ((cdma_cmd_err == CM_CALL_CMD_ERR_NOERR) &&
          (gw_cmd_err != CM_CALL_CMD_ERR_NOERR))
      {
        call_ptr->main_ss_mode = CM_CC_MODE_PREF_CDMA_ONLY;
        call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_CDMA;

        #if defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)
        if (call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB)
        {
          /* Remove GW from the favored mode pref.
          */
          CM_MSG_HIGH_0("CSFB:mode_usage = ANY and 1XCSFB is possible so removing TGW modes");
          call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                          call_ptr->favored_mode_pref,
                                          1,
                                          SD_SS_MODE_PREF_TDS_GSM_WCDMA );
        } else
        {
          /* Remove GWL from the favored mode pref.
          */
          CM_MSG_HIGH_0("CSFB:mode_usage = ANY and 1XCSFB is not possible so removing GW modes");
          call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                          call_ptr->favored_mode_pref,
                                          1,
                                          SD_SS_MODE_PREF_TDS_GSM_WCDMA_LTE );
          call_ptr->csfb_type  = CM_CALL_CSFB_TYPE_NONE;
        }
        #else
        {
          /* Remove GWL from the favored mode pref.
          */
          call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                          call_ptr->favored_mode_pref,
                                          1,
                                          SD_SS_MODE_PREF_TDS_GSM_WCDMA_LTE );
        }
        #endif

        /* If the mode preference to originate the call is NONE
        ** then return ERR
        */
        if( call_ptr->favored_mode_pref == CM_MODE_PREF_NONE )
        {
          CM_MSG_HIGH_1 ("Returning gw_cmd_err %d", gw_cmd_err);
          return gw_cmd_err;
        }
      }
      else if ((cdma_cmd_err != CM_CALL_CMD_ERR_NOERR) &&
               (gw_cmd_err == CM_CALL_CMD_ERR_NOERR))
      {
        call_ptr->main_ss_mode = CM_CC_MODE_PREF_GW_ONLY;
        if (call_ptr->call_type == CM_CALL_TYPE_PS_DATA)
        {
          call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_PS;
        }
        else
        {
          call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_CS;
        }

        /* Remove CDMA/AMPS/HDR from the favored mode pref.
        */
        #if defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)
        /* Do not remove LTE mode preference for voice call if 3GPP CSFB is supported
        */
        if( call_ptr->csfb_type == CM_CALL_CSFB_TYPE_PPCSFB)
        {
          CM_MSG_HIGH_0("CSFB:mode_usage = ANY and PPCSFB is possible so removing CDMA and HDR modes");
          call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                             call_ptr->favored_mode_pref,
                                             2,
                                             SD_SS_MODE_PREF_HDR,
                                             SD_SS_MODE_PREF_CDMA
                                             );
        }
        else if (((call_ptr->call_type   == CM_CALL_TYPE_VOICE ) ||
                 (call_ptr->call_type == CM_CALL_TYPE_CS_DATA) ||
                 (call_ptr->call_type == CM_CALL_TYPE_PD)) && //cr419781
                 ( call_ptr->csfb_type == CM_CALL_CSFB_TYPE_NONE) )
        {
          CM_MSG_HIGH_0("mode_usage = ANY and CSFB is not possible so removing CDMA,HDR and LTE modes");
          call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                           call_ptr->favored_mode_pref,
                                           2,
                                           SD_SS_MODE_PREF_CDMA_HDR,
                                           SD_SS_MODE_PREF_LTE
                                           );
        }else
        {
          CM_MSG_HIGH_2("mode_usage = ANY, csfb_type= %d & call_type=%d removing CDMA,HDR",
                        call_ptr->csfb_type, call_ptr->call_type);
          /* We can make emergency call GW or data call on GWL */
          call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                             call_ptr->favored_mode_pref,
                                             2,
                                             SD_SS_MODE_PREF_HDR,
                                             SD_SS_MODE_PREF_CDMA );
		  
          if (call_ptr->csfb_type != CM_CALL_CSFB_TYPE_NONE)
          {
            CM_MSG_HIGH_0 ("CSFB:Resetting csfb_type");
            call_ptr->csfb_type  = CM_CALL_CSFB_TYPE_NONE;
          }

        }
        #else

        call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                             call_ptr->favored_mode_pref,
                                             2,
                                             SD_SS_MODE_PREF_HDR,
                                             SD_SS_MODE_PREF_CDMA );
        #endif

        /* If the mode preference to originate the call is NONE
        ** then return ERR
        */
        if( call_ptr->favored_mode_pref == CM_MODE_PREF_NONE )
        {
          CM_MSG_HIGH_1 ("Returning cdma_cmd_err %d", cdma_cmd_err);
          return cdma_cmd_err;
        }
       }
      else if ((cdma_cmd_err != CM_CALL_CMD_ERR_NOERR) &&
                (gw_cmd_err != CM_CALL_CMD_ERR_NOERR) )
      {
       /* Return either cdma_cmd_err or gw_cmd_err depending on
       ** current favored mode
       */

       CM_MSG_HIGH_1 ("call_ptr->favored_mode_pref = %d", call_ptr->favored_mode_pref);
       if (cmcall_misc_is_mode_pref(call_ptr->favored_mode_pref,
                                    CM_MODE_PREF_GSM_ONLY) ||
           cmcall_misc_is_mode_pref(call_ptr->favored_mode_pref,
                                    CM_MODE_PREF_WCDMA_ONLY)
          )
       {
         CM_MSG_HIGH_1 ("Returning gw_cmd_err %d", gw_cmd_err);
         return gw_cmd_err;
       }
       else
       {
         CM_MSG_HIGH_1 ("Returning cdma_cmd_err %d", cdma_cmd_err);
         return cdma_cmd_err;
       }
      }
      #elif(!defined (FEATURE_MMODE_CDMA_800) && !defined (FEATURE_MMODE_CDMA_1900) && \
              (defined FEATURE_MMODE_3GPP ))

      call_ptr->main_ss_mode = CM_CC_MODE_PREF_GW_ONLY;

      if (call_ptr->call_type == CM_CALL_TYPE_PS_DATA)
      {
        call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_PS;
      }
      else
      {
        #if defined(FEATURE_MMODE_LTE_ONLY) || defined(FEATURE_MMODE_GPRS_LTE)
		if(call_ptr->call_type != CM_CALL_TYPE_EMERGENCY)
	    #endif
	    {
          call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_CS;		
	    }
      }

      #if defined (FEATURE_IP_CALL) && defined (FEATURE_CM_LTE)
      if(call_ptr->is_volte)
      {
          call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_IP;
          call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                               call_ptr->favored_mode_pref,
                                               2,
                                               SD_SS_MODE_PREF_HDR,
                                               SD_SS_MODE_PREF_CDMA
                                               );
          CM_MSG_HIGH_2("proc_orig_mode: favor_mode=%d, main_ss_mode=%d", call_ptr->favored_mode_pref, call_ptr->main_ss_mode);
          if(call_ptr->favored_mode_pref == CM_MODE_PREF_NONE)
          {
            cmd_err = CM_CALL_CMD_ERR_MODE_PREF_P;
          }
          break;

      }
      #endif
      #if defined(FEATURE_3GPP_CSFB) && defined(FEATURE_DOMAIN_SELECTION)
      /* Remove LTE mode preference for voice call if 3GPP CSFB is not possible */
      if(( (call_ptr->call_type == CM_CALL_TYPE_VOICE)||
            (call_ptr->call_type == CM_CALL_TYPE_CS_DATA)
         ) &&( !cmsds_is_ppcsfb_call_allowed (call_ptr->asubs_id) )
        )
      {
        /* Remove GWL from the favored mode pref.
        */
        CM_MSG_HIGH_0 ("PPCSFB not possible so make legacy  GW call");
        call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                           call_ptr->favored_mode_pref,
                                           2,
                                           SD_SS_MODE_PREF_CDMA_HDR,
                                           SD_SS_MODE_PREF_LTE
                                           );
      }
      #endif
#if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE) && !defined(FEATURE_MMODE_LTE_ONLY) && !defined(FEATURE_MMODE_GPRS_LTE)
      cmd_err = cmwcall_client_orig_proc (call_ptr);   
#endif
      #elif(defined (FEATURE_MMODE_CDMA_800) || defined (FEATURE_MMODE_CDMA_1900) && \
            !defined(FEATURE_GSM) && !defined(FEATURE_WCDMA))

      call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_CDMA;
      call_ptr->main_ss_mode = CM_CC_MODE_PREF_CDMA_ONLY;

      #if defined (FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
      if(call_ptr->is_volte)
      {
        call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_IP;
        call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                         call_ptr->favored_mode_pref,
                                         2,
                                         SD_SS_MODE_PREF_TDS_GSM_WCDMA,
                                         SD_SS_MODE_PREF_HDR);
		
        CM_MSG_HIGH_2("proc_orig_mode: favor_mode=%d, main_ss_mode=%d", call_ptr->favored_mode_pref, call_ptr->main_ss_mode);
        if(call_ptr->favored_mode_pref == CM_MODE_PREF_NONE)
        {
          cmd_err = CM_CALL_CMD_ERR_MODE_PREF_P;
        }
        break;
      }
      #endif

      cmd_err = cmxcall_client_orig_proc (call_ptr);

      #endif
      break;

    default:
      CM_ERR_1 ("bad mode_usage = %d", mode_usage);
      cmd_err = CM_CALL_CMD_ERR_CALL_ORIG_FAIL_S;
  }

  #ifdef FEATURE_MMODE_DUAL_SIM  
  if (cmd_err == CM_CALL_CMD_ERR_NOERR && cmph_is_msim())
  {
    sd_ss_mode_pref_e_type fav_mode_pref
      = cmph_map_cm_mode_pref_to_sd_mode_pref(call_ptr->favored_mode_pref);
    sd_ss_mode_pref_e_type mode_pref_mask = cmmsimc_get_sim_cap(call_ptr->ss);

    fav_mode_pref
      = (sd_ss_mode_pref_e_type)((word)fav_mode_pref & (word)mode_pref_mask);

    if (fav_mode_pref == SD_SS_MODE_PREF_NONE)
    {
      cmd_err = CM_CALL_CMD_ERR_MODE_PREF_P;
    }
    else
    {
      if (!cmph_is_sxlte())
      {
        call_ptr->favored_mode_pref
          = cmph_map_sd_mode_pref_to_cm_mode_pref(fav_mode_pref);
      }
    }
  }
  #endif
  
  if ( call_ptr->call_type != CM_CALL_TYPE_EMERGENCY
       && call_ptr->call_type != CM_CALL_TYPE_PS_DATA
       && cmclnup_is_ongoing_subsc(BM(call_ptr->asubs_id)))
  {
    CM_MSG_HIGH_0 ("CLNUP in prog, Cmd not allowed");
    cmd_err = CM_CALL_CMD_ERR_OFFLINE_S;
  }

  return cmd_err;
}
#endif

/*===========================================================================

FUNCTION  cmcall_is_active_ip_app

DESCRIPTION
  Determines if there is a active VOIP app for a given System.

DEPENDENCIES
  none
RETURN VALUE
  BOOLEAN
  TRUE  - If VOIP app is active.
  FALSE - Otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_active_ip_app
(
  sys_sys_mode_e_type  mode,
    /* system mode than can be serviced by app */

  cm_call_type_e_type  call_type
    /* call type that can be serviced by app */
)
/*lint -esym(715, call_type) */
/*lint -esym(715, mode) */
{

  #if (defined(FEATURE_IP_CALL)) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
  if (cmipapp_find_active_app (mode, call_type) != CMIPAPP_ID_INVALID)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
  #else
  CM_MSG_HIGH_0 ("FEATURE_IP_CALL not defined, did not attempt VoIP");
  return FALSE;
  #endif
}
/*lint +esym(715, call_type) */
/*lint +esym(715, mode) */

/*===========================================================================

FUNCTION cmcall_cmd_copy_fields

DESCRIPTION

  Copy the active fields of a call command into a call object.
  Copy common fields and then call cmxcall_cmd_copy_fields() and
  cmwcall_cmd_copy_fields() to copy mode specific fields

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_cmd_copy_fields(

    cmcall_s_type               *call_ptr,
        /* Pointer to a call object */

    const cm_call_cmd_s_type    *cmd_ptr
        /* Pointer to a call command */

)
{
  /* Point at call command information.
  */
  const cm_call_cmd_info_s_type *cmd_info_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL )
  CM_ASSERT( cmd_ptr  != NULL )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_info_ptr = CALL_CMD_INFO_PTR(cmd_ptr);
  call_ptr->call_client_ptr = cmd_ptr->client_ptr;

  /* Only copy active command fields in order to avoid overwriting
  ** call fields that are irrelevant to this command.
  */
  if( cmd_ptr->cmd_subs != SYS_MODEM_AS_ID_NONE )
  {
    call_ptr->asubs_id  =   cmd_ptr->cmd_subs;
  }

  if( cmd_info_ptr->call_type     != CM_CALL_TYPE_NONE )
  {
    call_ptr->call_type        = cmd_info_ptr->call_type;
  }

  if( cmd_info_ptr->srv_type     != CM_SRV_TYPE_NONE )
  {
    call_ptr->srv_type         = cmd_info_ptr->srv_type;
  }

  if( cmd_info_ptr->favored_mode_pref  != CM_MODE_PREF_NONE )
  {
    call_ptr->favored_mode_pref = cmd_info_ptr->favored_mode_pref;
  }
  /* for removal or addition of a party in IP conf call, 
  ** do not update call_ptr->num with num received in 
  ** cmd(removed/added party num) 
  */
  if( CM_NUM_LEN(&cmd_info_ptr->num)    != CM_NUMBER_LEN_NONE && 
      !( cmd_ptr->cmd == CM_CALL_CMD_SUPS &&
        (cmd_info_ptr->sups_cmd_type == CM_CALL_SUPS_TYPE_RELEASE_CALL_X_FROM_CONFERENCE 
        || cmd_info_ptr->sups_cmd_type == CM_CALL_SUPS_TYPE_ADD_PARTICIPANT)))
  {
    call_ptr->num              = cmd_info_ptr->num;
  }

  if( CM_NUM_LEN(&cmd_info_ptr->calling_num)    != CM_NUMBER_LEN_NONE )
  {
    call_ptr->calling_num      = cmd_info_ptr->calling_num;
  }

  if( cmd_info_ptr->alpha.len     != CM_ALPHA_LEN_NONE )
  {
    call_ptr->alpha            = cmd_info_ptr->alpha;
  }

  if( cmd_info_ptr->end_status    != CM_CALL_END_NONE )
  {
     call_ptr->end_status      = cmd_info_ptr->end_status;
  }

  if( cmd_ptr->info.info_type != CM_CALL_MODE_INFO_NONE )
  {
    call_ptr->cmcall_mode_info.info_type = cmd_ptr->info.info_type;

  }

  if( cmd_ptr->direction != CM_CALL_DIRECTION_NONE )
  {
    call_ptr->direction = cmd_ptr->direction;
  }

  #if defined(CM_GW_SUPPORTED)
  cmwcall_cmd_copy_fields( call_ptr, cmd_ptr );
  #endif

  #if (defined (FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  cmxcall_cmd_copy_fields(call_ptr,cmd_ptr);
  #endif

  #if defined(FEATURE_IP_CALL) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
  cmipcall_cmd_copy_fields(call_ptr, cmd_ptr);
  #endif
} /* cmcall_cmd_copy_fields() */

#if 0 /*Not being used */
/*===========================================================================

FUNCTION cmcall_is_hdr_waiting_for_end_req

DESCRIPTION

  Checks if any call is waiting to received END_CNF from HDR stack.
DEPENDENCIES

RETURN VALUE
  TRUE: If there exist a call for which we are waiting for END_CNF from
        HDR stack.
  FALSE: Otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_hdr_waiting_for_end_req
(
  void
)
{

  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */
  /*-----------------------------------------------------------------------*/
  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( call_ptr->call_type == CM_CALL_TYPE_PS_DATA  &&
         call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_HDR &&
         call_ptr->call_state == CM_CALL_STATE_IDLE &&
         call_ptr->call_subst.idle == CM_CALL_IDLE_SUBST_ENDWAIT
       )
    {
      if(call_ptr->end_req_wait_uptime <= time_get_uptime_secs())
      {
        return TRUE;
      }
    }

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while */

  return FALSE;
}
#endif

/*===========================================================================

FUNCTION cmcall_count_on_ss

DESCRIPTION

  Returns number of calls on a given SS.

DEPENDENCIES


RETURN VALUE
  Number of calls on a given SS.

SIDE EFFECTS
  none

===========================================================================*/
int cmcall_count_on_ss
(
  cm_ss_e_type    ss
)
{
  byte              num_allocated_call_ids = 0;
  cmcall_s_type    *call_ptr = NULL;
  cm_iterator_type  call_itr;

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);


  while(call_ptr != NULL)
  {
    if(call_ptr->call_id != CM_CALL_ID_INVALID  &&
       call_ptr->ss == ss)
    {
      num_allocated_call_ids++;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
    }
  return num_allocated_call_ids;

} /* cmcall_count_on_ss () */


/*===========================================================================

FUNCTION cmcall_is_no_call_in_gw_cs

DESCRIPTION
  Checks if there are no calls in gw cs domain

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  - No calls in cs domain
  FALSE - There is/are calls in cs domain

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_no_call_in_gw_cs(

  cm_ss_e_type      ss

)
{

  #if (defined (FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) \
    || (!defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE))

  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if( (ss == CM_SS_MAX &&
         call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS
        ) ||
        (call_ptr->ss == ss &&
         call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS
        )
      )
    {
      return FALSE;
    } /* if ( call_ptr->call_type == call_type ) */

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  /*-----------------------------------------------------------------------*/

  return TRUE;
  #else
  return TRUE;
  #endif

} /* cmcall_is_no_call_in_gw_cs() */


#if (defined (FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

/*===========================================================================

FUNCTION cmcall_is_no_call_in_gw_cs_per_subs

DESCRIPTION
  Checks if there are no calls in gw cs domain

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  - No calls in cs domain
  FALSE - There is/are calls in cs domain

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_no_call_in_gw_cs_per_subs(

  cm_ss_e_type      ss,

  sys_modem_as_id_e_type asubs_id

)
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if( ((ss == CM_SS_MAX &&
         call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS
        ) ||
        (call_ptr->ss == ss &&
         call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS
        )) &&
        asubs_id == call_ptr->asubs_id
      )
    {
      return FALSE;
    } /* if ( call_ptr->call_type == call_type ) */

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  /*-----------------------------------------------------------------------*/

  return TRUE;

} /* cmcall_is_no_call_in_gw_cs() */
#endif

/*===========================================================================

FUNCTION cmcall_is_no_call_in_gw_ps

DESCRIPTION
  Checks if there are no calls in gw ps domain

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  - No calls in ps domain
  FALSE - There is/are calls in ps domain

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_no_call_in_gw_ps (

  cm_ss_e_type    ss
    /* Stack on which the abort request has to be sent */

)
{

  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if( (ss == CM_SS_MAX &&
         call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS
        ) ||
        (call_ptr->ss == ss &&
         call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS
        )
      )
    {
      return FALSE;
    } /* if ( call_ptr->call_type == call_type ) */

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  /*-----------------------------------------------------------------------*/

  return TRUE;

} /* cmcall_is_no_call_in_gw_ps () */


/*===========================================================================

FUNCTION cmcall_is_no_call_in_gw_ps_per_subs

DESCRIPTION
  Checks if there are no calls in gw ps domain

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  - No calls in ps domain
  FALSE - There is/are calls in ps domain

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_no_call_in_gw_ps_per_subs (

  cm_ss_e_type    ss,

  sys_modem_as_id_e_type asubs_id

)
{
  #ifdef FEATURE_MMODE_GW
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if(((ss == CM_SS_MAX &&
         call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS
        ) ||
        (call_ptr->ss == ss &&
         call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS
        )) &&
        (asubs_id == call_ptr->asubs_id )
      )
    {
      return FALSE;
    } /* if ( call_ptr->call_type == call_type ) */

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  /*-----------------------------------------------------------------------*/
  return TRUE;
  #else
  return TRUE;
  #endif
} /* cmcall_is_no_call_in_gw_ps () */

#if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)

/*===========================================================================

FUNCTION cmcall_get_num_call_in_lte_ps

DESCRIPTION
  Get number of calls in the specified call_mode

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  uint8 - num of calls

SIDE EFFECTS
  none

===========================================================================*/
uint8 cmcall_get_num_call_in_call_mode (
      cm_call_mode_info_e_type  call_mode
)
{

  cmcall_s_type   *call_ptr;         /* Call object pointer */
  cm_iterator_type call_itr;         /* call Object Iterator */
  uint8            num_calls = 0;

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if (call_ptr->cmcall_mode_info.info_type == call_mode)
    {
      num_calls ++;
    } /* if ( call_ptr->call_type == call_type ) */

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  /*-----------------------------------------------------------------------*/

  return num_calls;

} /* cmcall_is_no_call_in_lte_ps () */
#endif

/*===========================================================================

FUNCTION cmcall_is_no_call_in_lte_ps

DESCRIPTION
  Checks if there are no calls in LTE ps domain

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE  - No calls in ps domain
  FALSE - There is/are calls in ps domain

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_no_call_in_lte_ps (void)

{

  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_LTE)
    {
      return FALSE;
    } /* if ( call_ptr->call_type == call_type ) */

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  /*-----------------------------------------------------------------------*/

  return TRUE;

} /* cmcall_is_no_call_in_lte_ps () */

#if 0 /*NOT being used */
/*===========================================================================

FUNCTION cmcall_is_active_on_mode

DESCRIPTION
  Checks if there is a call on a given mode.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  FASLE  - No calls
  TRUE   - Otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_active_on_mode
(
  sys_sys_mode_e_type  sys_mode
)
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
     /* call Object Iterator */

 /*-----------------------------------------------------------------------*/

 cmcall_obj_iterator_init(&call_itr);

 call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if(sys_mode == SYS_SYS_MODE_GSM   ||
       sys_mode == SYS_SYS_MODE_WCDMA ||
       sys_mode == SYS_SYS_MODE_GW)
    {
        if(call_ptr->sys_mode ==  SYS_SYS_MODE_GSM    ||
           call_ptr->sys_mode ==  SYS_SYS_MODE_WCDMA  ||
           call_ptr->sys_mode ==  SYS_SYS_MODE_GW)
        {
          return TRUE;
        }
    }
    else if(sys_mode == SYS_SYS_MODE_GWL)
    {
        if(call_ptr->sys_mode ==  SYS_SYS_MODE_GSM    ||
           call_ptr->sys_mode ==  SYS_SYS_MODE_WCDMA  ||
           call_ptr->sys_mode ==  SYS_SYS_MODE_GW     ||
           call_ptr->sys_mode ==  SYS_SYS_MODE_LTE)
        {
          return TRUE;
        }
    }
    else if(call_ptr->sys_mode == sys_mode)
    {
        return TRUE;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  /*-----------------------------------------------------------------------*/

  return FALSE;

}
#endif

#if defined(FEATURE_MMODE_GW)

/*===========================================================================

FUNCTION cmcall_get_prim_with_groupid

DESCRIPTION
  Gives the conn id of primary pdp with given group id.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  Connection id of primary pdp. Unknown id if no primary pdp is found.

SIDE EFFECTS
  none

===========================================================================*/
byte cmcall_get_prim_with_groupid  (

  byte pdp_group_id
    /* Group id of primary pdp */
)
{

  #if (defined CM_GW_SUPPORTED )

  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( (call_ptr->call_type                  == CM_CALL_TYPE_PS_DATA)    &&
         (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS) &&
         (call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.pdp_group_id
                                               == pdp_group_id)
       )
    {
      return call_ptr->gw_connection_id;
    } /* if ( call_ptr->call_type == call_type ) */

    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  /*-----------------------------------------------------------------------*/
  #else

  SYS_ARG_NOT_USED(pdp_group_id);

  #endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

  /*-----------------------------------------------------------------------*/

  return CM_UNKNOWN_CONN_ID;

} /* cmcall_is_no_call_in_gw_ps () */
#endif /* #if defined(FEATURE_MMODE_GW) */

#if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)

/*===========================================================================

FUNCTION cmcall_det_sys_mode_from_ss

DESCRIPTION
  Returns sys mode that is currently present in the given ss

DEPENDENCIES
  None.

RETURN VALUE
  SS instance

SIDE EFFECTS
  none

===========================================================================*/
sys_sys_mode_e_type  cmcall_det_sys_mode_from_ss(

    cm_ss_e_type   ss
      /* system instance, MAIN, HYBR_HDR ... */
)
{
  cmss_s_type                *ss_ptr              = cmss_ptr();
  /* Pointer to the serving system object */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT (INRANGE(ss, CM_SS_NONE, CM_SS_MAX));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Returns system mode that can be carried by a ss */


  switch( ss )
  {
    case CM_SS_MAIN:
    case CM_SS_MAX:
      return ss_ptr->info.sys_mode;

    case CM_SS_HYBR_1:

      return SYS_SYS_MODE_HDR;

    #ifdef FEATURE_MMODE_HYBR2_ENABLED
    case CM_SS_HYBR_2:
      return ss_ptr->info.gw_sys_mode;
    #endif
    #ifdef FEATURE_MMODE_HYBR3_ENABLED
    case CM_SS_HYBR_3: 
      return ss_ptr->info.gw3_sys_mode;
    #endif
    default:
      CM_ERR_1 ("Unknown ss %d", ss);
      return SYS_SYS_MODE_NONE;
   }
} /* cmcall_det_sys_mode_from_ss () */

/*===========================================================================
FUNCTION cmcall_det_ss_from_sys_mode

DESCRIPTION
  Determines ss instance on which the given sys_mode is present. currently
  supports WCDMA, HDR only.

DEPENDENCIES
  None.

RETURN VALUE
  SS instance

SIDE EFFECTS
  none

===========================================================================*/
cm_ss_e_type  cmcall_det_ss_from_sys_mode(

    cmcall_s_type             *call_ptr
        /* sys mode */
)
{
  cmmsc_state_machine_s_type *state_machine = NULL;
  sys_modem_as_id_e_type      as_id = SYS_MODEM_AS_ID_NONE;
  cm_ss_e_type                cm_ss = CM_SS_MAIN;
    
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT(call_ptr);
  CM_ASSERT (BETWEEN(call_ptr->sys_mode, SYS_SYS_MODE_NONE, SYS_SYS_MODE_MAX));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  as_id = call_ptr->asubs_id;
  state_machine = cmmsimc_state_machine_ptr(as_id);
  cm_ss = cmph_map_subs_to_ss(as_id);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Function should return valid instance for NO_SRV. Function is typically
  ** called after receiving a msg on a system, so chances of NO_SRV should
  ** be low.
  */
  if (call_ptr->sys_mode == SYS_SYS_MODE_NO_SRV)
  {
    CM_MSG_HIGH_0 ("sys mode passed is NO_SRV, ret MAIN");
    return cm_ss;
  }

  if (as_id == SYS_MODEM_AS_ID_2 || as_id == SYS_MODEM_AS_ID_3)
  {
    return cm_ss;
  }

  /* sometimes MMODE might have switched from SRLTE -> SingleMode ater
  ** MT INVITE was sent and before CM_IP_CALL_IND_CONNECTED
  ** hence for such cases make sure we move the call to CM_SS_MAIN
  */
  #ifdef FEATURE_MMODE_SC_SVLTE  
  if(cmph_is_subs_feature_mode_1x_sxlte(as_id) &&
     state_machine->stack_common_info.ue_mode == SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED &&
     cmss_is_hybr2_operational() == FALSE)
  {
    if (call_ptr->sys_mode == SYS_SYS_MODE_LTE)
    {
      /* Determine ss for this call */
      return CM_SS_MAIN;
    }
  }
  #endif
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the instance carrying given sys mode
  */
  if(call_ptr->sys_mode == SYS_SYS_MODE_WLAN)
  {
    return cm_ss;
  }
  else if(call_ptr->sys_mode == SYS_SYS_MODE_LTE)
  {
    return cmcall_det_ss_for_volte_call(call_ptr);
  }  
  else if (cmss_get_true_sys_mode(cm_ss) == call_ptr->sys_mode)
  {
    return cm_ss;
  }
 
  else if( call_ptr->sys_mode == SYS_SYS_MODE_HDR )
  {
    return CM_SS_HDR;
  }
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ERR_1 ("Unknown sys mode %d", call_ptr->sys_mode);
  return CM_SS_NONE;

} /* cmcall_det_ss_from_sys_mode () */
#endif

/*===========================================================================
FUNCTION cmcall_dem_is_apps_wakeup_from_suspend_event

DESCRIPTION
  Determines if the event should wakeup the processor from the suspend state

DEPENDENCIES
  None.

RETURN VALUE
  True : If the apps processor needs to be woken up
  False: Otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean  cmcall_dem_is_apps_wakeup_from_suspend_event(

    cm_call_event_e_type   call_event
        /* Call event */
)
/*lint -esym(715, call_event) */
{
  #ifndef FEATURE_APPS_POWER_COLLAPSE
  /* If Apps power collapse is not defined, return ture
  */
  return TRUE;
  #else

  switch ( call_event )
  {
    case CM_CALL_EVENT_INCOM:
    case CM_CALL_EVENT_CONNECT:
    case CM_CALL_EVENT_SRV_OPT:
    case CM_CALL_EVENT_PRIVACY:
    case CM_CALL_EVENT_CALLER_ID:
    case CM_CALL_EVENT_ABRV_ALERT:
    case CM_CALL_EVENT_SIGNAL:
    case CM_CALL_EVENT_DISPLAY:
    case CM_CALL_EVENT_CALLED_PARTY:
    case CM_CALL_EVENT_CONNECTED_NUM:
    case CM_CALL_EVENT_EXT_DISP:
    case CM_CALL_EVENT_EXT_BRST_INTL:
    case CM_CALL_EVENT_NSS_CLIR_REC:
    case CM_CALL_EVENT_NSS_REL_REC:
    case CM_CALL_EVENT_NSS_AUD_CTRL:
    case CM_CALL_EVENT_SETUP_IND:
    case CM_CALL_EVENT_MNG_CALLS_CONF:
    case CM_CALL_EVENT_CALL_BARRED:
    case CM_CALL_EVENT_CALL_IS_WAITING:
    case CM_CALL_EVENT_CALL_ON_HOLD:
    case CM_CALL_EVENT_CALL_RETRIEVED:
    case CM_CALL_EVENT_ORIG_FWD_STATUS:
    case CM_CALL_EVENT_CALL_FORWARDED:
    case CM_CALL_EVENT_CALL_BEING_FORWARDED:
    case CM_CALL_EVENT_INCOM_FWD_CALL:
    case CM_CALL_EVENT_CALL_RESTRICTED:
    case CM_CALL_EVENT_CUG_INFO_RECEIVED:
    case CM_CALL_EVENT_CNAP_INFO_RECEIVED:
    case CM_CALL_EVENT_PROGRESS_INFO_IND:
    case CM_CALL_EVENT_CALL_DEFLECTION:
    case CM_CALL_EVENT_TRANSFERRED_CALL:
    case CM_CALL_EVENT_REDIRECTING_NUMBER:
    case CM_CALL_EVENT_USER_DATA_IND:
    case CM_CALL_EVENT_MODIFY_IND:
    case CM_CALL_EVENT_LINE_CTRL:
    case CM_CALL_EVENT_ACT_CCBS_CNF:
    case CM_CALL_EVENT_CCBS_RECALL_IND:
    case CM_CALL_EVENT_CONNECT_ORDER_ACK:
    case CM_CALL_EVENT_END:
    case CM_CALL_EVENT_OTASP_STATUS:

      return TRUE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      return FALSE;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #endif
} /* cmcall_dem_is_apps_wakeup_from_suspend_event */
/*lint +esym(715, call_event) */

/*===========================================================================

FUNCTION cmcall_unpack_subaddress

DESCRIPTION
  This function takes, subaddress in the format defined in 3GPP 24.008 spec
  in the section 10.5.4.8, as input and unpack it in the cm_subaddress_s_type
  strcuture format.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_unpack_subaddress(

    cm_subaddress_s_type        *cm_subaddress_ptr,
        /* called party subaddress in struct format */

    const gstk_subaddress_type *gstk_subaddress_ptr
        /* gstk subaddress in stream format */

)
{
  CM_ASSERT( gstk_subaddress_ptr != NULL );

  if(gstk_subaddress_ptr->length > 0)
  {

    /* Octate 3 first bit (from MSB) is copied in extension bit */
    cm_subaddress_ptr->extension_bit =
          (boolean)((gstk_subaddress_ptr->subaddress[0] & 0x80) >> 7);

    /*
    ** Octate 3 second, third and fourth bit (from MSB) is copied
    ** in subadd_type
    */
    cm_subaddress_ptr->subadd_type =
          ((gstk_subaddress_ptr->subaddress[0] & 0x70) >> 4);

    /* Octate 3 fifth bit (from MSB) is copied in odd_even_ind */
    cm_subaddress_ptr->odd_even_ind =
          ((gstk_subaddress_ptr->subaddress[0] & 0x08) >> 3);

    cm_subaddress_ptr->num_char = memscpy( cm_subaddress_ptr->chari,
                                           sizeof(cm_subaddress_ptr->chari),
                                           &(gstk_subaddress_ptr->subaddress[1]),
                                           ((uint8)(MIN(
                                              (sizeof(gstk_subaddress_ptr->subaddress)-sizeof(gstk_subaddress_ptr->subaddress[0])),
                                              (gstk_subaddress_ptr->length) - 1))) );

  }
  else
  {
    cm_subaddress_ptr->num_char = 0;
  }
} /* cmcall_unpack_subaddress() */


/*===========================================================================

FUNCTION cmcall_pack_subaddress

DESCRIPTION
  This function takes, called party subaddress in the cm_subaddress_s_type
  struct format, as input and packs it in the format defined in 3GPP 24.008
  spec in the section 10.5.4.8.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_pack_subaddress(

    gstk_subaddress_type *gstk_subaddress_ptr,
        /* gstk subaddress in stream format */

    const cm_subaddress_s_type    *cm_subaddress_struct_ptr
        /* called party subaddress in struture format */

)
{
  byte octet = 0;

  CM_ASSERT( cm_subaddress_struct_ptr != NULL );

  if(cm_subaddress_struct_ptr->num_char > 0)
  {
    /* OCTET 3 first bit (from MSB) is copied from extension bit */
    octet |= (byte)((cm_subaddress_struct_ptr->extension_bit & 0x01) << 7);

    /*
    ** OCTET 3 second, third and fourth bit (from MSB)
    ** is copied from subadd_type
    */
    octet |= (byte)((cm_subaddress_struct_ptr->subadd_type & 0x07) << 4);

    /* OCTET 3 fifth bit (from MSB) is copied from odd_even_ind */
    octet |= (byte)((cm_subaddress_struct_ptr->odd_even_ind & 0x01) << 3);

    gstk_subaddress_ptr->subaddress[0] = (uint8)octet;

    /* First byte of destination is used for octet, hence 1 is subtracted from destination size */
    gstk_subaddress_ptr->length = ( memscpy( &(gstk_subaddress_ptr->subaddress[1]),
                                    (sizeof(gstk_subaddress_ptr->subaddress) - 1),
                                             cm_subaddress_struct_ptr->chari,
                                             cm_subaddress_struct_ptr->num_char ) + 1 );
  }
  else
  {
    gstk_subaddress_ptr->length = 0;
  }

} /* cmcall_pack_subaddress() */

/*===========================================================================
FUNCTION cm_reserve_connection_id

DESCRIPTION
  This function is called by the Call Manager in order to get a connection
  Id for a CS call.

DEPENDENCIES

  The transmission of primitives between the CM and MN depend on this
  connection id.

RETURN VALUE

  Returns connection id if a connection Id is found, otherwise returns
  UNKNOWN_CONNECTION_ID.

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -esym(529, as_id) not referenced */
byte cm_reserve_connection_id(sys_modem_as_id_e_type as_id)
{

  byte index;
  byte connection_id = UNKNOWN_CONNECTION_ID;

   if(as_id != SYS_MODEM_AS_ID_NONE)
   {
       // Get an available transaction Id from the list
       for(index = MO; index <= MAX_MO_CONNECTION_ID; index++)
       {
          if( as_id >=0 && as_id < MIN(MAX_SIMS, ((int)cmpmprx_get_num_of_sims()) ) )
          {
           if (call_context_trans_id[index][as_id] == FALSE)
           {
               call_context_trans_id[index][as_id] = TRUE;
               connection_id = index;
               CM_MSG_HIGH_2("Reserve MN CONN_ID = %d, asubs_id = %d",
                                         connection_id, as_id);
               break;
            }
       }
       }
       SYS_ARG_NOT_USED(as_id)
       return connection_id;
   }
   else
   {
     CM_MSG_HIGH_1("Received Invalid as_id=%d",as_id);
     return UNKNOWN_CONNECTION_ID;
   }
}
/*lint +esym(529, as_id) not referenced */


/*===========================================================================
FUNCTION cm_unreserve_connection_id

DESCRIPTION
  This function is called by the Call Manager in order to unreserve a
  previously secured MN connection ID. This will allow CM only to unreserve
  connection for MO Inactive call.

DEPENDENCIES

  Call Manager must use this function only to unserve a previously
  reserved connection_id from MN.

RETURN VALUE

  Returns TRUE if a connection Id is unreserved, otherwise returns FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
boolean cm_unreserve_connection_id(

    byte connection_id,

    sys_modem_as_id_e_type as_id

)
{
  boolean    return_value = TRUE;

  /* For single sim, always use the default as_id */
  if (cmph_is_ssim())
  {
    as_id = SYS_MODEM_AS_ID_1;
  }

  if(as_id != SYS_MODEM_AS_ID_NONE && as_id < MIN(MAX_SIMS, ((int)cmpmprx_get_num_of_sims()) ) )
  {
    if(connection_id <= MAX_MO_CONNECTION_ID)
    {
     call_context_trans_id[connection_id][as_id] = FALSE;
     CM_MSG_HIGH_2("Releasing connection id = %d, asubs_id= %d",
                 connection_id, as_id);
     return_value = TRUE;
    }
  }

  else
  {
    return_value = FALSE;    
  }

  SYS_ARG_NOT_USED(as_id)
  return return_value;
}



/*===========================================================================
FUNCTION cm_is_connection_id_in_use

DESCRIPTION
  Accessor function to determine if a connection ID is currently in use.

DEPENDENCIES

  cmcall_init() must have previously been called.

RETURN VALUE

  Returns TRUE if a connection Id is in use, otherwise returns FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
boolean cm_is_connection_id_in_use(

  byte connection_id,

  sys_modem_as_id_e_type asubs_id
    /**< Indicates the asubs_id */
)
{
  // If valid, return TRUE/FALSE indicating if connection_id is in use
  if (connection_id <= MAX_MO_CONNECTION_ID)
  {
    return (call_context_trans_id[connection_id][asubs_id]);
  }

  CM_MSG_HIGH_1("Unknown connection_id %d", connection_id);
  return FALSE;
}

/*===========================================================================
FUNCTION cmcall_reserve_sm_connection_id

DESCRIPTION
  This function is called by the Call Manager in order to reserve a
  connection ID to be passed to SM for a Packet Data call. Each PDP context
  is identified by this connection Id during Primitive exchange.

DEPENDENCIES
  cmcall_init() must have been called before using this function.

  The transmission of primitives between the CM and SM depend on this
  connection ID.

RETURN VALUE

  Returns TRUE if a connection Id is reserved, otherwise returns FALSE.

  connection_id

SIDE EFFECTS
  None.

===========================================================================*/
boolean cmcall_reserve_sm_connection_id(byte* connection_id,sys_modem_as_id_e_type as_id)
{
  byte i = CM_MIN_MO_TI;
  
  #ifndef FEATURE_DUAL_DATA
  as_id = SYS_MODEM_AS_ID_1;
  #endif

  // Get an available transaction ID from the list
  if(as_id >=SYS_MODEM_AS_ID_1 && as_id <= SYS_MODEM_AS_ID_MAX )
  {
   for (i = CM_MIN_MO_TI; i <= CM_MAX_MO_TI; i++)
   {
    if (pdp_context_trans_id[i][as_id] == FALSE)
    {
      pdp_context_trans_id[i][as_id] = TRUE;
      *connection_id = i;
      CM_MSG_HIGH_1("Reserve SM CONN_ID = %d", *connection_id);
      break;
    }
   }
  }
  else 
  {
     CM_MSG_HIGH_1("Received Invalid as_id=%d",as_id);
  }

  return pdp_context_trans_id[i][as_id];

}


/*===========================================================================
FUNCTION cmcall_unreserve_sm_connection_id

DESCRIPTION
  This function is called by Call Manager in order to unreserve a
  previously reserved connection ID. This will allow CM only to unreserve
  connection for MO Inactive call.

DEPENDENCIES
  cmcall_init() must have been previously called.

  Call Manager uses this function to unreserve a previously
  reserved connection_id.

RETURN VALUE

  Returns TRUE if a connection ID is unreserved, otherwise returns FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
boolean cmcall_unreserve_sm_connection_id(byte connection_id,sys_modem_as_id_e_type as_id)
{
  CM_MSG_HIGH_1 ("Unreserving SM CONN_ID = %d", connection_id);

  #ifndef FEATURE_DUAL_DATA
  as_id = SYS_MODEM_AS_ID_1;
  #endif

  if ((connection_id <= CM_MAX_MO_TI) && 
    (as_id >=SYS_MODEM_AS_ID_1 && as_id <= SYS_MODEM_AS_ID_MAX))
  {
    pdp_context_trans_id[connection_id][as_id] = FALSE;
    return TRUE;
  }

  CM_MSG_HIGH_2 ("Invalid PDP context ID/AS_ID, CON_ID = %d, AS ID=%d", connection_id, as_id);
  return FALSE;

}  /* End of function cmcall_unreserve_sm_connection_id() */


/*===========================================================================
FUNCTION cmcall_is_sm_connection_id_in_use

DESCRIPTION
  Accessor function to determine if a connection ID is currently in use.
  Connection ID represents a PDP context and is used between CM and SM.

DEPENDENCIES
  cmcall_init() must have previously been called.

RETURN VALUE

  Returns TRUE if a connection ID is in use, otherwise returns FALSE.
  Returns FALSE for invalid/unknown connection IDs.

SIDE EFFECTS
  None.

===========================================================================*/
boolean cmcall_is_sm_connection_id_in_use(byte connection_id,sys_modem_as_id_e_type as_id)
{

  #ifndef FEATURE_DUAL_DATA
  as_id = SYS_MODEM_AS_ID_1;
  #endif

  // If valid, return TRUE/FALSE indicating if connection_id is in use
  if (connection_id <= CM_MAX_MO_TI)
  {
    return (pdp_context_trans_id[connection_id][as_id]);
  }

  CM_ERR_1("Unknown connection_id", connection_id);
  return FALSE;
}


#if (defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE)
/*===========================================================================
FUNCTION cmcall_mark_sm_connection_id_reserved

DESCRIPTION
  This function is called by the Call Manager in order to mark a SM connection
  Id for a Packet Data call when LTE call is originated. When the PS data call
  originated while active rat is LTE, we receives connection_id from NW and we
  should use this connection id instead of generating our own.

DEPENDENCIES

  The transmission of primitives between the CM and SM depend on this
  connection id.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void cmcall_mark_sm_connection_id_reserved(byte connection_id,sys_modem_as_id_e_type as_id)
{

  #ifndef FEATURE_DUAL_DATA
  as_id = SYS_MODEM_AS_ID_1;
  #endif
  
  // Get an available transaction Id from the list
  if((connection_id <= CM_MAX_MO_TI) && 
	  (as_id >=SYS_MODEM_AS_ID_1) && (as_id <= SYS_MODEM_AS_ID_MAX))
  {
    if(pdp_context_trans_id[connection_id][as_id] == FALSE)
    {
      pdp_context_trans_id[connection_id][as_id] = TRUE;
      CM_MSG_HIGH_1("Marking SM CONN_ID: %d", connection_id);
    }
    else
    {
      CM_ERR_1("CONN_ID: %d already been used",connection_id);
    }
  }
}


/*===========================================================================
FUNCTION cm_print_connection_id

DESCRIPTION
  This function is utility funtion to print out all GW connection_ids

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void cm_print_connection_ids()
{
  cmcall_s_type            *call_ptr;
  cm_call_id_type          call_id = CM_CALL_ID_INVALID;
  byte i;
  short as_id = 0;

  for(as_id=0;as_id < MIN(MAX_SIMS, ((int)cmpmprx_get_num_of_sims())); as_id++) 
  {
   for (i = CM_MIN_MO_TI; i <= CM_MAX_MO_TI; i++)
   {
    if(pdp_context_trans_id[i][as_id] == TRUE)
    {
      #ifndef FEATURE_DUAL_DATA
      /* if FEATURE_DUAL_DATA not defined, 
          call id can be returned FALSE for SUB1 calls if we use cm_find_call_id_gw_per_sub*/
      if(cm_find_call_id_gw(i, &call_id))
      #else
      if(cm_find_call_id_gw_per_sub(i, &call_id,as_id))
      #endif
      {
        call_ptr = cmcall_ptr(call_id);

        if(call_ptr == NULL)
        {
          CM_ERR_1("call_ptr is NULL for call_id=%d",call_id);
          continue;
        }

        CM_MSG_HIGH_3("SM CONN_ID[%d]: call_id:%d, info_type:%d", i, call_id,
                     call_ptr->cmcall_mode_info.info_type);

        if(call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS)
        {
          #if defined(FEATURE_MMODE_GW)
          CM_MSG_HIGH_2("conn_src:%d, nsapi:%d",
              call_ptr->gw_conn_id_src,
              call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.nsapi.nsapi);
          #elif defined(FEATURE_MMODE_GW) && defined(FEATURE_CM_LTE)
          CM_MSG_HIGH_3("conn_src:%d, esm_conn_id:%d, nsapi:%d",
              call_ptr->gw_conn_id_src,
              call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.esm_conn_id,
              call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.nsapi.nsapi);
          #endif
        }
        #ifdef FEATURE_CM_LTE
        else if(call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_LTE)
        {
          CM_MSG_HIGH_3("conn_src:%d, esm_conn_id:%d, eps_bearer_id:%d",
              call_ptr->gw_conn_id_src, call_ptr->gw_connection_id,
              call_ptr->cmcall_mode_info.info.lte_info.eps_bearer_id);
        }
        #endif
      }
      else
      {
        CM_ERR_1("SM CONN_ID[%d]: RESERVED, but NOT FOUND", i);
      }
    }
  }
 }
}

#endif

/*===========================================================================

FUNCTION cmcall_lte_emerg_call_failure_processing_alternate2

DESCRIPTION
  Process LTE NAS/RRC failure causes during emergency call to decide whether
  to redial over CS or continue over LTE.
  differences from cmcall_lte_emerg_call_failure_processing are
  1. EMM causes that are being treated as sof failures now
    LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATED
    LTE_NAS_CONGESTION
  2. est_status that are considered as hard failure
    LTE_RRC_CONN_EST_FAILURE_TIMER_EXPIRED
  3. for hard failures, call end status would be updated as
  CM_CALL_END_IP_HARD_FAILURE

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
#if defined (FEATURE_CM_LTE) && defined(FEATURE_IP_CALL) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
#ifdef FEATURE_MMODE_EMC_SUPPORT

static void cmcall_lte_emerg_call_failure_processing_alternate2(

  const cm_esm_cmd_u_type   *cm_rpt_ptr,
    /* Pointer to LL report */

  cmcall_s_type             *call_ptr
    /* Pointer to call object  */

)
{
  boolean                is_emerg_retry_req = TRUE;
  emm_failure_type       emm_failure_cause;
  lte_nas_local_cause_T  esm_local_cause;
  boolean                is_emerg_bearer_fail = FALSE;
  cmcall_s_type          *call_ptr_itr = NULL;
  cm_call_id_type        call_id = CM_CALL_ID_INVALID;
  boolean                is_found = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_rpt_ptr != NULL );
  CM_ASSERT( call_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* initialization */
  emm_failure_cause.cause_type = LTE_NAS_IRAT_NONE;
  esm_local_cause.valid = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* check the reject causes and decide action need to be taken
  */
  switch( cm_rpt_ptr->msg_hdr.id )
  {
    case MM_CM_PDN_CONNECTIVITY_REJECT_IND:
      emm_failure_cause = cm_rpt_ptr->cm_pdn_connectivity_reject_ind.emm_failure_cause;
      esm_local_cause = cm_rpt_ptr->cm_pdn_connectivity_reject_ind.esm_local_cause;
      is_emerg_bearer_fail = cm_rpt_ptr->cm_pdn_connectivity_reject_ind.emc_type;
      CM_MSG_MED_3("E911_LTE_ONLY: PDN_REJECT_IND, cause_type=%d, esm valid=%d, is_emerg_bearer=%d",
      emm_failure_cause.cause_type,esm_local_cause.valid,is_emerg_bearer_fail);
      break;

    case MM_CM_DRB_REESTABLISH_REJECT_IND:
      emm_failure_cause = cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause;
      esm_local_cause = cm_rpt_ptr->cm_drb_reestablish_reject_ind.esm_local_cause;
      /* here need to extract call id associated with the eps_bearer_id in DRB_REJECT
       Check if found call id is emergency bearer or not */
      is_found = cm_find_call_id(CONN_ID_ESM,
                 cm_rpt_ptr->cm_drb_reestablish_reject_ind.esm_hdr.connection_id,
                 &call_id);
      if(is_found)
      {
        call_ptr_itr = cmcall_ptr ( call_id );
        if(call_ptr_itr != NULL &&
           call_ptr_itr->cmcall_mode_info.info.lte_info.is_emergency_bearer == TRUE)
        {
          is_emerg_bearer_fail = TRUE;
        }
      }

      CM_MSG_MED_3("E911_LTE_ONLY: DRB_REJ_IND, cause_type=%d esm valid=%d, connection id = %d",
      emm_failure_cause.cause_type,esm_local_cause.valid,cm_rpt_ptr->cm_drb_reestablish_reject_ind.esm_hdr.connection_id);
      break;

    default:
      CM_MSG_HIGH_1( "lte_emerg: ignore msg id = %d",cm_rpt_ptr->msg_hdr.id);
      return;

  } /* switch */
   /* if failure reported is not for emergency call then no need for silent redial */
  if(!is_emerg_bearer_fail)
  {
    CM_MSG_HIGH_0( "E911_LTE_ONLY: PDN Reject or DRB_REESTABLISH_REJECT_IND is not for emergency bearer");
    /* in this case shall we simply return */
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Process the EMM failure cause
  */
  switch(emm_failure_cause.cause_type)
  {
    case LTE_NAS_IRAT_SR_REJECT:
      CM_MSG_MED_1("E911_LTE_ONLY: emm_cause=%d",emm_failure_cause.cause.emm_cause);
      switch(emm_failure_cause.cause.emm_cause)
      {
        case LTE_NAS_ILLEGAL_UE:
        case LTE_NAS_ILLEGAL_ME:
        case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
        case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
        case LTE_NAS_CONGESTION:
        case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:
          is_emerg_retry_req = FALSE;
          break;

        default:
          break;
      }
      break;

    case LTE_NAS_IRAT_NONE:
    case LTE_NAS_IRAT_CONN_REL:
    case LTE_NAS_IRAT_UL_MSG_CNF_FAILURE:
    case LTE_NAS_IRAT_LOWER_LAYER_FAILURE:
    default:
      break;
  } // emm_failure_cause

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If there is no EMM failure cause, check ESM cause
  */
  if(esm_local_cause.valid)
  {
    CM_MSG_MED_1("E911_LTE_ONLY: local_cause=%d",esm_local_cause.local_cause);
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!is_emerg_retry_req)
  {
      cmipcall_send_end(call_ptr);
  }
  else
  {
    if(call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE)
    {
      call_ptr->end_status = CM_CALL_END_IP_RETRY;
      cmcall_orig_proc(call_ptr);
      CM_MSG_MED_0("E911_LTE_ONLY: LTE soft failure, re-sending IMS orig request");
    }
  }
  
  return;
}


/*===========================================================================

FUNCTION cmcall_lte_emerg_call_failure_processing_alternate

DESCRIPTION
  Process LTE NAS/RRC failure causes during emergency call to decide whether
  to redial over CS or continue over LTE.
  differences from cmcall_lte_emerg_call_failure_processing are
  1. EMM causes that are being treated as sof failures now
    LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATED
    LTE_NAS_CONGESTION
  2. est_status that are considered as hard failure
    LTE_RRC_CONN_EST_FAILURE_TIMER_EXPIRED
  3. for hard failures, call end status would be updated as 
  CM_CALL_END_IP_HARD_FAILURE

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/


static void cmcall_lte_emerg_call_failure_processing_alternate(

  const cm_esm_cmd_u_type   *cm_rpt_ptr,
    /* Pointer to LL report */

  cmcall_s_type             *call_ptr
    /* Pointer to call object  */

)
{

  boolean                is_hard_failure_ran = FALSE;
  emm_failure_type       emm_failure_cause;
  lte_nas_local_cause_T  esm_local_cause;
  boolean is_emerg_bearer_fail = FALSE;
  cmcall_s_type   *call_ptr_itr = NULL;
  cm_call_id_type          call_id = CM_CALL_ID_INVALID;
  boolean                  is_found = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_rpt_ptr != NULL );
  CM_ASSERT( call_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* initialization */
  emm_failure_cause.cause_type = LTE_NAS_IRAT_NONE;
  esm_local_cause.valid = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* check the reject causes and decide action need to be taken
  */
  switch( cm_rpt_ptr->msg_hdr.id )
  {
    case MM_CM_PDN_CONNECTIVITY_REJECT_IND:
      emm_failure_cause = cm_rpt_ptr->cm_pdn_connectivity_reject_ind.emm_failure_cause;
      esm_local_cause = cm_rpt_ptr->cm_pdn_connectivity_reject_ind.esm_local_cause;
      is_emerg_bearer_fail = cm_rpt_ptr->cm_pdn_connectivity_reject_ind.emc_type;
      CM_MSG_HIGH_3("PDN_REJECT_IND, cause_type=%d, esm valid=%d, is_emerg_bearer=%d",
      emm_failure_cause.cause_type,esm_local_cause.valid,is_emerg_bearer_fail);
      break;

    case MM_CM_DRB_REESTABLISH_REJECT_IND:
      emm_failure_cause = cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause;
      esm_local_cause = cm_rpt_ptr->cm_drb_reestablish_reject_ind.esm_local_cause;
      /* here need to extract call id associated with the eps_bearer_id in DRB_REJECT
       Check if found call id is emergency bearer or not */
    is_found = cm_find_call_id(CONN_ID_ESM,
               cm_rpt_ptr->cm_drb_reestablish_reject_ind.esm_hdr.connection_id,
               &call_id);
    if(is_found)
    {
      call_ptr_itr = cmcall_ptr ( call_id );
      if(call_ptr_itr != NULL &&
         call_ptr_itr->cmcall_mode_info.info.lte_info.is_emergency_bearer == TRUE)
      {
        is_emerg_bearer_fail = TRUE;
      }
    }

    CM_MSG_HIGH_3("DRB_REJ_IND, cause_type=%d esm valid=%d, connection id = %d",
      emm_failure_cause.cause_type,esm_local_cause.valid,cm_rpt_ptr->cm_drb_reestablish_reject_ind.esm_hdr.connection_id);
      break;

    default:
      CM_MSG_HIGH_1( "lte_emerg: ignore msg id = %d",cm_rpt_ptr->msg_hdr.id);
      return;

  } /* switch */
   /* if failure reported is not for emergency call then no need for silent redial */
  if(!is_emerg_bearer_fail)
  {
    CM_MSG_HIGH_0( "PDN Reject or DRB_REESTABLISH_REJECT_IND is not for emergency bearer");
    /* in this case shall we simply return */
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Process the EMM failure cause
  */
  switch(emm_failure_cause.cause_type)
  {
    case LTE_NAS_IRAT_SR_REJECT:
      CM_MSG_HIGH_1("emm_cause=%d",emm_failure_cause.cause.emm_cause);
      switch(emm_failure_cause.cause.emm_cause)
      {
        case LTE_NAS_ILLEGAL_UE:
        case LTE_NAS_ILLEGAL_ME:
        case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
        case LTE_NAS_UE_ID_CANNOT_BE_DERIVED_BY_NW:
        case LTE_NAS_IMPLICITLY_DETACHED:
        case LTE_NAS_PLMN_NOT_ALLOWED:
        case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
        case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
        case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
        case LTE_NAS_CS_DOMAIN_NOT_AVAILABLE:
        case LTE_NAS_CS_DOMAIN_TEMP_NOT_AVAILABLE:
        case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
          is_hard_failure_ran = TRUE;
          break;

        default:
          break;
      }
      break;

    case LTE_NAS_IRAT_CONN_EST_FAILURE:
      CM_MSG_HIGH_1("est_status=%d",emm_failure_cause.cause.est_status);
      switch(emm_failure_cause.cause.est_status)
      {
        //hard failures
        case LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED:
        case LTE_RRC_CONN_EST_FAILURE_CONN_REJECT:
        case LTE_RRC_CONN_EST_FAILURE_TIMER_EXPIRED:
          is_hard_failure_ran = TRUE;
          break;

        //soft failures

        default:
          break;
      }
      break;

    case LTE_NAS_IRAT_T3417_EXPIRY:
    case LTE_NAS_IRAT_ACCESS_BARRING:
      is_hard_failure_ran = TRUE;
      break;

    case LTE_NAS_IRAT_INVALID_STATE:
      CM_MSG_MED_1("lte_emerg_alt: IRAT_INVALID_STATE, state=%d",
                cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.cause.state);
      switch(cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.cause.state)
      {
      case EMM_INVALID_STATE_PERSISTENT:
        is_hard_failure_ran = TRUE;
        break;
      default:
        break;
      }
      break;

    case LTE_NAS_IRAT_NONE:
    case LTE_NAS_IRAT_CONN_REL:
    case LTE_NAS_IRAT_UL_MSG_CNF_FAILURE:
    case LTE_NAS_IRAT_LOWER_LAYER_FAILURE:
    default:
      break;
  } // emm_failure_cause

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If there is no EMM failure cause, check ESM cause
  */
  if(!is_hard_failure_ran &&
     esm_local_cause.valid)
  {
    CM_MSG_HIGH_1("local_cause=%d",esm_local_cause.local_cause);
    switch(esm_local_cause.local_cause)
    {
      case EMM_T3417_EXPIRED:
      case EMM_T3417_EXT_EXPIRED:
        is_hard_failure_ran = TRUE;
        break;
      default:
        break;
    }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(is_hard_failure_ran ||
     call_ptr->sr.voip_sr.lte_soft_failure_count >= CM_LTE_EMERG_SOFT_FAIL_RETRY_COUNT
    )
  {
    call_ptr->sr.voip_sr.is_lte_hard_failure = TRUE;
    /* End IP call and wait for end indication from IMS to fallback to CS.
    ** clear volte call flag so SD orig mode will become normal.
    */
    if(call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_LTE)
    {
      cmipcall_send_end(call_ptr);
    }
    else
    {
      CM_MSG_HIGH_1("LTE hard failure RAN %d",call_ptr->sr.orig_sent_on_mode);
      call_ptr->end_status = CM_CALL_END_IP_HARD_FAILURE;
      cmcall_orig_proc(call_ptr);
    }
  }
  else
  {
    call_ptr->sr.voip_sr.lte_soft_failure_count ++;
    CM_MSG_HIGH_1( "lte_soft_failure_count = %d",call_ptr->sr.voip_sr.lte_soft_failure_count);

    if(call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE)
    {
      call_ptr->end_status = CM_CALL_END_IP_RETRY;
      cmcall_orig_proc(call_ptr);
      CM_MSG_HIGH_0("LTE soft failure, re-sending IMS orig request");
    }
  }
  return;
} /* cmcall_lte_emerg_call_failure_processing */

/*===========================================================================

FUNCTION cmcall_lte_emerg_call_failure_processing

DESCRIPTION
  Process LTE NAS/RRC failure causes during emergency call to decide whether
  to redial over CS or continue over LTE

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_lte_emerg_call_failure_processing(

  const cm_esm_cmd_u_type   *cm_rpt_ptr,
    /* Pointer to LL report */

  cmcall_s_type             *call_ptr
    /* Pointer to call object  */

)
{

   boolean                is_hard_failure = FALSE;
   emm_failure_type       emm_failure_cause;
   lte_nas_local_cause_T  esm_local_cause;
  boolean is_emerg_bearer_fail = FALSE;
  cmcall_s_type   *call_ptr_itr = NULL;
  cm_call_id_type          call_id = CM_CALL_ID_INVALID;
  boolean                  is_found = FALSE;
  cmph_sub_config_s_type *ph_sub_config_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_rpt_ptr != NULL );
  CM_ASSERT( call_ptr != NULL );

  ph_sub_config_ptr = cmph_get_sub_config_ptr(call_ptr->asubs_id);

  if(ph_sub_config_ptr == NULL)
  {
    CM_MSG_HIGH_1("cmph sub config pointer[%d] is NULL",call_ptr->asubs_id);
    return;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* initialization */
  emm_failure_cause.cause_type = LTE_NAS_IRAT_NONE;
  esm_local_cause.valid = FALSE;

  #ifdef FEATURE_MMODE_EMC_SUPPORT
   /* if alternate_redial_algorithm is enabled, call alternate call failure processing */
  if(ph_sub_config_ptr->is_alternate_redial_algorithm)
  {
    cmcall_lte_emerg_call_failure_processing_alternate(cm_rpt_ptr, call_ptr);
    return;
  }
  #endif
  
  #ifndef FEATURE_TRIM_CMPMPRX
  /* Emergency call on LTE only special is enabled, call mode for current plmn is VOLTE Only */
  if( (cmpmprx_is_special_volte_e911_calls() == TRUE) && 
  	  (cmmsc_get_stack_common_call_mode_per_subs(call_ptr->asubs_id) == SYS_CALL_MODE_VOLTE_ONLY))  	
  {
    cmcall_lte_emerg_call_failure_processing_alternate2(cm_rpt_ptr, call_ptr);
    return;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* check the reject causes and decide action need to be taken
  */
  switch( cm_rpt_ptr->msg_hdr.id )
  {
    case MM_CM_PDN_CONNECTIVITY_REJECT_IND:
      emm_failure_cause = cm_rpt_ptr->cm_pdn_connectivity_reject_ind.emm_failure_cause;
      esm_local_cause = cm_rpt_ptr->cm_pdn_connectivity_reject_ind.esm_local_cause;
    is_emerg_bearer_fail = cm_rpt_ptr->cm_pdn_connectivity_reject_ind.emc_type;
    CM_MSG_HIGH_3("PDN_REJECT_IND, cause_type=%d, esm valid=%d, is_emerg_bearer=%d",
      emm_failure_cause.cause_type,esm_local_cause.valid,is_emerg_bearer_fail);
      break;

    case MM_CM_DRB_REESTABLISH_REJECT_IND:
      emm_failure_cause = cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause;
      esm_local_cause = cm_rpt_ptr->cm_drb_reestablish_reject_ind.esm_local_cause;

    /* here need to extract call id associated with the eps_bearer_id in DRB_REJECT
    Check if found call id is emergency bearer or not */
    is_found = cm_find_call_id(CONN_ID_ESM,
               cm_rpt_ptr->cm_drb_reestablish_reject_ind.esm_hdr.connection_id,
               &call_id);
    if(is_found)
    {
      call_ptr_itr = cmcall_ptr ( call_id );
      if(call_ptr_itr != NULL &&
         call_ptr_itr->cmcall_mode_info.info.lte_info.is_emergency_bearer == TRUE)
      {
        is_emerg_bearer_fail = TRUE;
      }
    }

    CM_MSG_HIGH_3("DRB_REJ_IND, cause_type=%d esm valid=%d, connection id = %d",
      emm_failure_cause.cause_type,esm_local_cause.valid,cm_rpt_ptr->cm_drb_reestablish_reject_ind.esm_hdr.connection_id);
      break;

    default:
      CM_MSG_HIGH_1( "lte_emerg: ignore msg id = %d",cm_rpt_ptr->msg_hdr.id);
      return;

  } /* switch */
  /* if failure reported is not for emergency call then no need for silent redial */
  if(!is_emerg_bearer_fail)
  {
    CM_MSG_HIGH_0( "PDN Reject or DRB_REESTABLISH_REJECT_IND is not for emergency bearer");
    /* in this case shall we simply return */
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Process the EMM failure cause
  */


  switch(emm_failure_cause.cause_type)
  {
    case LTE_NAS_IRAT_SR_REJECT:
      CM_MSG_HIGH_1( "emm_cause=%d",emm_failure_cause.cause.emm_cause );
      switch(emm_failure_cause.cause.emm_cause)
      {
        case LTE_NAS_ILLEGAL_UE:
        case LTE_NAS_ILLEGAL_ME:
        case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
        case LTE_NAS_UE_ID_CANNOT_BE_DERIVED_BY_NW:
        case LTE_NAS_IMPLICITLY_DETACHED:
        case LTE_NAS_PLMN_NOT_ALLOWED:
        case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
        case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
        case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
        case LTE_NAS_CS_DOMAIN_NOT_AVAILABLE:
        case LTE_NAS_CS_DOMAIN_TEMP_NOT_AVAILABLE:
        case LTE_NAS_CONGESTION:
        case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
        case LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATED:
          is_hard_failure = TRUE;
          break;

        default:
          break;
      }
      break;

    case LTE_NAS_IRAT_CONN_EST_FAILURE:
      CM_MSG_HIGH_1("est_status=%d",emm_failure_cause.cause.est_status);
      switch(emm_failure_cause.cause.est_status)
      {
        //hard failures
        case LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED:
        case LTE_RRC_CONN_EST_FAILURE_CONN_REJECT:
          is_hard_failure = TRUE;
          break;

        //soft failures
        case LTE_RRC_CONN_EST_FAILURE_TIMER_EXPIRED:
        default:
          break;
      }
      break;

    case LTE_NAS_IRAT_T3417_EXPIRY:
    case LTE_NAS_IRAT_ACCESS_BARRING:
      is_hard_failure = TRUE;
      break;

    case LTE_NAS_IRAT_INVALID_STATE:
      CM_MSG_MED_1("lte_emerg: IRAT_INVALID_STATE, state=%d",
                cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.cause.state);
      switch(cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.cause.state)
      {
      case EMM_INVALID_STATE_PERSISTENT:
        is_hard_failure = TRUE;
        break;
      default:
        break;
      }
      break;

    case LTE_NAS_IRAT_NONE:
    case LTE_NAS_IRAT_CONN_REL:
    case LTE_NAS_IRAT_UL_MSG_CNF_FAILURE:
    case LTE_NAS_IRAT_LOWER_LAYER_FAILURE:
    default:
      break;
  } // emm_failure_cause

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If there is no EMM failure cause, check ESM cause
  */
  if(!is_hard_failure &&
     esm_local_cause.valid)
  {
    CM_MSG_HIGH_1("local_cause=%d",esm_local_cause.local_cause);
    switch(esm_local_cause.local_cause)
    {
      case EMM_T3417_EXPIRED:
      case EMM_T3417_EXT_EXPIRED:
        is_hard_failure = TRUE;
        break;
      default:
        break;
    }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(is_hard_failure ||
     call_ptr->sr.voip_sr.lte_soft_failure_count >= CM_LTE_EMERG_SOFT_FAIL_RETRY_COUNT
    )
  {
    call_ptr->sr.voip_sr.is_lte_hard_failure = TRUE;
    /* End IP call and wait for end indication from IMS to fallback to CS.
    ** clear volte call flag so SD orig mode will become normal.
    */
    if(call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_LTE)
    {
      cmipcall_send_end(call_ptr);
    }
    else
    {
      CM_MSG_HIGH_1("LTE hard failure %d",call_ptr->sr.orig_sent_on_mode);
      call_ptr->end_status = CM_CALL_END_IP_FAIL;
      if(call_ptr->is_em_vt)
      {
        CM_MSG_HIGH_0("Reset is_em_vt");
        call_ptr->is_em_vt = FALSE;
      }
      cmcall_orig_proc(call_ptr);
    }
  }
  else
  {
    call_ptr->sr.voip_sr.lte_soft_failure_count ++;
    CM_MSG_HIGH_1( "lte_soft_failure_count = %d",call_ptr->sr.voip_sr.lte_soft_failure_count);

    if(call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE)
    {
      call_ptr->end_status = CM_CALL_END_IP_RETRY;
      cmcall_orig_proc(call_ptr);
      CM_MSG_HIGH_0("LTE soft failure, re-sending IMS orig request");
    }
  }
  return;
} /* cmcall_lte_emerg_call_failure_processing */
#endif
/*===========================================================================

FUNCTION cmcall_is_drb_rej_on_acb

DESCRIPTION
  If DRB reject is due to access barring

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmcall_is_drb_rej_on_acb(const cm_esm_cmd_u_type *cm_rpt_ptr)
{
  /* If barring timer is 0s, no fallack and stay on LTE otherwise fallback */
  /* DRB_REJ_IND && ((EST_FAILURE && (ACB||ACB_EAB))  ||  IRAT_ACB) && (timer !=0) or
  ** PDN_REJ_IND && ((EST_FAILURE && (ACB||ACB_EAB))  || IRAT_ACB) && (timer!=0)
  */
  if(((cm_rpt_ptr->msg_hdr.id == MM_CM_DRB_REESTABLISH_REJECT_IND) &&
    ((cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.cause_type == LTE_NAS_IRAT_CONN_EST_FAILURE &&
      (cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.cause.est_status == LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED ||
       cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.cause.est_status == LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB)) ||
     cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.cause_type ==LTE_NAS_IRAT_ACCESS_BARRING) &&
    (cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.access_barring_rem_time != 0)) ||
    ((cm_rpt_ptr->msg_hdr.id == MM_CM_PDN_CONNECTIVITY_REJECT_IND) &&
    ((cm_rpt_ptr->cm_pdn_connectivity_reject_ind.emm_failure_cause.cause_type == LTE_NAS_IRAT_CONN_EST_FAILURE &&
      (cm_rpt_ptr->cm_pdn_connectivity_reject_ind.emm_failure_cause.cause.est_status == LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED ||
       cm_rpt_ptr->cm_pdn_connectivity_reject_ind.emm_failure_cause.cause.est_status == LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB)) ||
     cm_rpt_ptr->cm_pdn_connectivity_reject_ind.emm_failure_cause.cause_type ==LTE_NAS_IRAT_ACCESS_BARRING) &&
    (cm_rpt_ptr->cm_pdn_connectivity_reject_ind.emm_failure_cause.access_barring_rem_time != 0)))
  {
    return TRUE;
  }
  return FALSE;
}
/*===========================================================================

FUNCTION cmvoltecall_rpt_proc

DESCRIPTION
  Process lower layer reports (i.e. replies to  call commands).

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmvoltecall_rpt_proc(

  const cm_esm_cmd_u_type   *cm_rpt_ptr
    /* Pointer to LL report */
)
{
  cmcall_s_type            *call_ptr;
      /* Point at call object */

  cm_call_id_type          call_id = CM_CALL_ID_INVALID;
      /* Call ID */

   boolean                is_hard_failure = FALSE;
   boolean                is_volte_restrict = FALSE;
   boolean                is_non_em_lte_restrict_call = FALSE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_rpt_ptr != NULL );

  call_id = cmcall_get_voip_call_id();
  if(call_id == CM_CALL_ID_INVALID)
  {
    CM_MSG_HIGH_0("VOLTE call proc skip: call not found");
    return;
  }

  call_ptr = cmcall_ptr ( call_id );
  if(call_ptr == NULL)
  {
    CM_MSG_HIGH_0("VOLTE call proc skip: call ptr NULL");
    return;
  }

  if(cmph_get_sub_config_ptr(call_ptr->asubs_id)->volte_sr_control)
  {
    /* if ACB SR control is ON do CM silent redial for ACB */
    if(!(cmph_get_volte_sub_info_ptr(call_ptr->asubs_id)->volte_acb_sr_control &&
           cmcall_is_drb_rej_on_acb(cm_rpt_ptr)))
    {
      /*no fallback*/
      return;
    }
  }

  if (!(( call_ptr->call_type == CM_CALL_TYPE_VOICE
         && call_ptr->is_volte)||
         call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
         ||call_ptr->direction != CM_CALL_DIRECTION_MO
    || call_ptr->call_state != CM_CALL_STATE_ORIG)
  {
    CM_MSG_HIGH_4("VOLTE call proc skip: call type %d is_volte %d, dir=%d, state=%d",\
        call_ptr->call_type,call_ptr->is_volte,call_ptr->direction,call_ptr->call_state);
    return;
  }

  #ifdef FEATURE_MMODE_EMC_SUPPORT
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* special processing for emergency calls. Consider both PDN rejection and DRB reject
  */
  if (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
  {
    cmcall_lte_emerg_call_failure_processing(cm_rpt_ptr, call_ptr);
    return;
  }
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cmcall_is_volte_restricted_call(call_ptr))
  {
    is_volte_restrict = TRUE;
  }

  /* For non EM calls in LTE only mode, all failures from NAS are to be
  ** treated as soft failure. To keep the call in IMS domain, until
  ** either call succeeds when connection estb./RAB estb. succeeds or call 
  ** terminates when max redial time for call is reached.
  ** We do not want to incorporate this logic in cmcall_is_volte_restricted_call 
  ** as that is used even for cases when IMS gives call end - with fallback to cs.
  ** For end reasons from IMS that map to fallback to cs, expectation is call
  ** is never redialled back to IMS. Since if any redialis applicable, it would 
  ** have already been performed by IMS before giving end to MMODE 
  */
  #if (defined (FEATURE_MMODE_LTE_ONLY) || defined (FEATURE_MMODE_GPRS_LTE) )
  if(call_ptr->call_type != CM_CALL_TYPE_EMERGENCY )
  #else
  if(call_ptr->call_type != CM_CALL_TYPE_EMERGENCY &&
     cmph_is_lte_only_sub(call_ptr->ss))
  #endif
  {
    CM_MSG_HIGH_0(" Set volte restricted call, as in LTE only mode");
    is_non_em_lte_restrict_call = TRUE;
  }

  /* check the reject causes and decide action need to be taken
  */
  switch( cm_rpt_ptr->msg_hdr.id )
  {
    case MM_CM_DRB_REESTABLISH_REJECT_IND:
      CM_MSG_HIGH_2("DRB_REJ_IND, cause_type=%d, time=%d",
                    cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.cause_type,
                     cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.access_barring_rem_time);

      switch(cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.cause_type)
      {
        case LTE_NAS_IRAT_SR_REJECT:
          CM_MSG_HIGH_1("DRB_REJ_IND, emm_cause=%d",
                    cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.cause.emm_cause);
          switch(cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.cause.emm_cause)
          {
            case LTE_NAS_ILLEGAL_UE:
            case LTE_NAS_ILLEGAL_ME:
            case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
            case LTE_NAS_UE_ID_CANNOT_BE_DERIVED_BY_NW:
            case LTE_NAS_IMPLICITLY_DETACHED:
            case LTE_NAS_PLMN_NOT_ALLOWED:
            //35 tbd
            case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
            case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
            case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
            case LTE_NAS_CONGESTION:
            case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
            case LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATED:
              is_hard_failure = TRUE;
              break;

            default:
              break;
          }
          break;

        case LTE_NAS_IRAT_CONN_EST_FAILURE:
          CM_MSG_HIGH_1("DRB_REJ_IND, est_status=%d",
                    cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.cause.est_status);
          switch(cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.cause.est_status)
          {
            //hard failures
            case LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED:
            case LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB:
            case LTE_RRC_CONN_EST_FAILURE_CONN_REJECT:
              is_hard_failure = TRUE;
              break;
            //soft failures
            case LTE_RRC_CONN_EST_FAILURE_TIMER_EXPIRED:
            default:
              break;
          }
          break;

        case LTE_NAS_IRAT_T3417_EXPIRY:
        case LTE_NAS_IRAT_ACCESS_BARRING:
              is_hard_failure = TRUE;
              break;

        case LTE_NAS_IRAT_INVALID_STATE:
          CM_MSG_MED_1("DRB_REJ_IND, IRAT_INVALID_STATE, state=%d",
                    cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.cause.state);
          switch(cm_rpt_ptr->cm_drb_reestablish_reject_ind.emm_failure_cause.cause.state)
          {
          case EMM_INVALID_STATE_PERSISTENT:
            is_hard_failure = TRUE;
            break;
          default:
            break;
          }
          break;

        case LTE_NAS_IRAT_NONE:
        case LTE_NAS_IRAT_CONN_REL:
        case LTE_NAS_IRAT_UL_MSG_CNF_FAILURE:
        case LTE_NAS_IRAT_LOWER_LAYER_FAILURE:

        default:
          break;
      }

      if(cm_rpt_ptr->cm_drb_reestablish_reject_ind.esm_local_cause.valid)
      {
        CM_MSG_HIGH_1("DRB_REJ_IND, local_cause=%d",
                    cm_rpt_ptr->cm_drb_reestablish_reject_ind.esm_local_cause.local_cause);
        switch(cm_rpt_ptr->cm_drb_reestablish_reject_ind.esm_local_cause.local_cause)
        {
          case EMM_T3417_EXPIRED:
          case EMM_T3417_EXT_EXPIRED:
            is_hard_failure = TRUE;
            break;
          default:
            break;
        }
      }

      break;  //case MM_CM_DRB_REESTABLISH_REJECT_IND

    default:
      CM_MSG_HIGH_1( "ignore msg id = %d",cm_rpt_ptr->msg_hdr.id);
      return;

  } /* switch */

  if((is_hard_failure ||
     call_ptr->sr.voip_sr.lte_soft_failure_count >= cmph_get_volte_sub_info_ptr(call_ptr->asubs_id)->lte_soft_fail_retry_count
     ) &&
     (is_volte_restrict == FALSE) && (is_non_em_lte_restrict_call == FALSE))
  {
    /* End IP call and wait for end indication from IMS to fallback to CS.
    ** clear volte call flag so SD orig mode will become normal.
    */
    call_ptr->end_status = CM_CALL_END_DRB_REJ;    
    cmipcall_send_end(call_ptr);
    call_ptr->sr.voip_sr.is_lte_hard_failure = TRUE;
  }
  else
  {
    call_ptr->sr.voip_sr.lte_soft_failure_count ++;
    CM_MSG_HIGH_2( "lte_soft_failure_count = %d orig_sent_on_mode %d",
                   call_ptr->sr.voip_sr.lte_soft_failure_count,
                   call_ptr->sr.orig_sent_on_mode );

    if( call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE )
    {
      call_ptr->end_status = CM_CALL_END_IP_RETRY;
      cmcall_orig_proc(call_ptr);
      CM_MSG_HIGH_0("LTE soft failure, re-sending IMS orig request");
    }

  }

  return;
} /* cmvoltecall_rpt_proc */
#endif

/*===========================================================================

FUNCTION cmcall_find_active_voice_call

DESCRIPTION
  Checks is there is active voice call

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_find_active_voice_call(void)
{
  cmcall_s_type   *call_ptr;
  cm_iterator_type call_itr;
    /* call Object Iterator */

  cmcall_obj_iterator_init(&call_itr);
  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if(call_ptr->call_type == CM_CALL_TYPE_VOICE ||
       call_ptr->call_type == CM_CALL_TYPE_EMERGENCY ||
	   call_ptr->call_type == CM_CALL_TYPE_VT )
    {
      return call_ptr->call_id;
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  return CM_CALL_ID_INVALID;

} /* cmcall_find_active_voice_call() */

#ifndef FEATURE_TRIM_CMPMPRX
#ifdef FEATURE_MMODE_3GPP2
/*===========================================================================

FUNCTION cmcall_find_active_sms_call

DESCRIPTION
  Checks is there is active sms call

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_find_active_sms_call(void)
{
  cmcall_s_type   *call_ptr;
  cm_iterator_type call_itr;
    /* call Object Iterator */

  cmcall_obj_iterator_init(&call_itr);
  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if(call_ptr->call_type == CM_CALL_TYPE_SMS)
    {
      return call_ptr->call_id;
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  return CM_CALL_ID_INVALID;

} /* cmcall_find_active_sms_call() */
#endif
/*===========================================================================

FUNCTION cmcall_find_active_cs_data_call

DESCRIPTION
  Checks is there is active cs data call

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_find_active_cs_data_call(void)
{
  cmcall_s_type   *call_ptr;
  cm_iterator_type call_itr;
    /* call Object Iterator */

  cmcall_obj_iterator_init(&call_itr);
  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if(call_ptr->call_type == CM_CALL_TYPE_CS_DATA)
    {
      return call_ptr->call_id;
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  return CM_CALL_ID_INVALID;

} /* cmcall_find_active_cs_data_call() */
/*===========================================================================

FUNCTION cmcall_find_active_voice_call

DESCRIPTION
  Checks is there is active voice call

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_find_active_voice_call_1(

  sys_modem_as_id_e_type		 asubs_id

  /* subs on which to find active CS call */

 )
{
  cmcall_s_type   *call_ptr;
  cm_iterator_type call_itr;
    /* call Object Iterator */

  cmcall_obj_iterator_init(&call_itr);
  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
     if( ( call_ptr->call_type == CM_CALL_TYPE_VOICE)
        &&
        call_ptr->asubs_id == asubs_id
      )
    {
      return call_ptr->call_id;
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  return CM_CALL_ID_INVALID;

} /* cmcall_find_active_voice_call_1() */
#endif


/*===========================================================================

FUNCTION cmcall_find_active_cs_call_per_subs

DESCRIPTION
  Checks is there is active voice/emergency/vt call per sub

DEPENDENCIES
  None

RETURN VALUE
  valid call id of requested call, CM_CALL_ID_INVALID otherwise

SIDE EFFECTS
  none

===========================================================================*/
cm_call_id_type cmcall_find_active_cs_call_per_subs(

   sys_modem_as_id_e_type         asubs_id

   /* subs on which to find active CS call */

   )
{
  #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
  cmcall_s_type   *call_ptr;
  cm_iterator_type call_itr;
    /* call Object Iterator */

  cmcall_obj_iterator_init(&call_itr);
  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if( ( call_ptr->call_type == CM_CALL_TYPE_VOICE ||
          call_ptr->call_type == CM_CALL_TYPE_EMERGENCY ||
          call_ptr->call_type == CM_CALL_TYPE_SMS ||
          call_ptr->call_type == CM_CALL_TYPE_VT 
         ) &&
        call_ptr->asubs_id == asubs_id
      )
    {
      return call_ptr->call_id;
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  return CM_CALL_ID_INVALID;
  #else
  SYS_ARG_NOT_USED(asubs_id);
  return CM_CALL_ID_INVALID;  
  #endif

} /* cmcall_find_active_cs_call_per_subs() */


#ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
/*===========================================================================
FUNCTION cmcall_ssac_p00_change

DESCRIPTION
  Check if LTE MMTEL voice SSAC barring factor has change between p00 and non-p00

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean cmcall_ssac_p00_change(uint8 old_barr_factor, uint8 new_barr_factor)
{
  if((old_barr_factor == LTE_RRC_AC_BARRING_FACTOR_P00 && new_barr_factor != LTE_RRC_AC_BARRING_FACTOR_P00) ||
    (old_barr_factor != LTE_RRC_AC_BARRING_FACTOR_P00 && new_barr_factor == LTE_RRC_AC_BARRING_FACTOR_P00))
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION cmcall_ssac_process_lte_ind

DESCRIPTION
  Process SSAC parameters update from LTE RRC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmcall_ssac_process_lte_ind(
  const lte_rrc_service_ind_s  * rrc_srv_ind_ptr
)
{
  
  if (rrc_srv_ind_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SSAC params and selected_plmn are valid if srv_status is available */
  if (rrc_srv_ind_ptr->svc_status != LTE_RRC_SVC_STATUS_AVAILABLE)
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update plmn and reset the backoff timer if there is a change  */
  cmcall_ssac_process_plmn_cell_change(&(rrc_srv_ind_ptr->camped_svc_info));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
}
/*===========================================================================

FUNCTION cmcall_read_barring_params

DESCRIPTION
  read SSAC parameters from LTE RRC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cmcall_read_barring_params
(
  lte_rrc_barring_update_ind_s  *rrc_barring_params
)
{

  cmcall_ssac_s_type  *cmcall_ssac_info = cmcall_ssac_ptr();
  uint8 prev_ssac_factor = cmcall_ssac_info->ssac_params.barring_factor_mmtel_voice;
  uint8 inst_id = rrc_barring_params->msg_hdr.inst_id;
  sys_modem_as_id_e_type as_id = cmph_get_as_id_from_ll_ind(INST_ID_TO_SYS_AS_ID(inst_id));

  
  /* Update ssac params */
  cmcall_ssac_info->ssac_params = rrc_barring_params->ssac_params;
  cmcall_ssac_info->ssac_associated_plmn = cmss_convert_rrc_plmn_id_to_sys_plmn_id(rrc_barring_params->camped_plmn);

  CM_MSG_HIGH_6("Read SSAC, prev_sib2_barring_factor %d, inst_id %d, lte as_id %d, plmn %d %d %d ",
                 prev_ssac_factor,
                 inst_id, cmph_get_sub_with_lte_cap(),
                 cmcall_ssac_info->ssac_associated_plmn.identity[0],
                 cmcall_ssac_info->ssac_associated_plmn.identity[1],
                 cmcall_ssac_info->ssac_associated_plmn.identity[2]);


  CM_MSG_HIGH_6("Voice: valid %d barr_fact %d sib2_barr_fact %d, Video: valid %d barr_fact %d sib2_barr_fact %d",
                    cmcall_ssac_info->ssac_params.ssac_param_voice_valid,
                    cmcall_ssac_info->ssac_params.barring_factor_mmtel_voice,
                cmcall_ssac_info->ssac_params.sib2_barring_factor_mmtel_voice,
                    cmcall_ssac_info->ssac_params.ssac_param_video_valid,
                    cmcall_ssac_info->ssac_params.barring_factor_mmtel_video,
                    cmcall_ssac_info->ssac_params.sib2_barring_time_mmtel_video);

  /*when RRC send valid FALSE, CM convert it to internal default value*/
  if(!cmcall_ssac_info->ssac_params.ssac_param_voice_valid)
  {
    cmcall_ssac_info->ssac_params.sib2_barring_factor_mmtel_voice= LTE_RRC_ACCESS_BARRING_PROB_p100;
    cmcall_ssac_info->ssac_params.barring_factor_mmtel_voice= LTE_RRC_ACCESS_BARRING_PROB_p100;
    cmcall_ssac_info->ssac_params.sib2_barring_time_mmtel_voice = 0;
    cmcall_ssac_info->ssac_params.barring_time_mmtel_voice = 0;
  }
  if(!cmcall_ssac_info->ssac_params.ssac_param_video_valid)
  {
    cmcall_ssac_info->ssac_params.sib2_barring_factor_mmtel_video= LTE_RRC_ACCESS_BARRING_PROB_p100;
    cmcall_ssac_info->ssac_params.barring_factor_mmtel_video= LTE_RRC_ACCESS_BARRING_PROB_p100;
    cmcall_ssac_info->ssac_params.sib2_barring_time_mmtel_video = 0;
    cmcall_ssac_info->ssac_params.barring_time_mmtel_video = 0;
  }

  /* If SSAC delay timer is running, When cell resel happens, run through the ssac barring check from beginning
  ** If barring time is 20ms, run through the ssac wait procedure 
  ** If barring time is not 20ms, recalculate the ssac timer and go to legacy behavior(start new backoff
  ** timer and rejec the call)
  ** If barring factor is barred, reject the call immediately same as legacy does
  */
  if ((rex_get_timer(&(cmcall_ssac_info->ssac_delay_timer)) > 0 )&&
     (cmcall_ssac_ptr()->call_cmd_ptr != NULL)
    )  
  {
    cm_cmd_queue ((cm_cmd_type *)cmcall_ssac_ptr()->call_cmd_ptr);
  }

  /* If NV enabled and SSAC MMTEL voice barring factor has change between p00 and non-p00,
  ** run LTE domain sel
  */
  #if defined(FEATURE_DOMAIN_SELECTION) && !defined (FEATURE_MMODE_DISABLE_LTE_VOICE)
  if(cmph_get_volte_sub_info_ptr(as_id)->is_ssac_hvolte &&
     cmcall_ssac_p00_change(prev_ssac_factor, cmcall_ssac_info->ssac_params.barring_factor_mmtel_voice))
  {
     sd_si_info_s_type  si_info;
     cm_ss_e_type       lte_ss = cmph_determine_ss_per_rat(as_id, SYS_SYS_MODE_LTE);

     sd_si_info_ptr_get( lte_ss, &si_info);
     if(si_info.srv_status == SYS_SRV_STATUS_SRV && si_info.mode == SYS_SYS_MODE_LTE)
     {
       cmsds_process_srv_ind(lte_ss, &si_info);
     }
  }
  #endif

}
/*===========================================================================

FUNCTION cmcall_process_ssac_timer

DESCRIPTION
  This function should be called to process ssac timers.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmcall_process_ssac_timer(

  cm_timer_event_type    timer_event
  /* Indicate specific timer event.
   ** Note that currently timer event is not being used
   */

)
/*lint -esym(715,timer_event) */
{
  cmcall_ssac_s_type  *cmcall_ssac_info = cmcall_ssac_ptr();

  /* Check if ssac voice timer expired and reset the timer
  */
  if( cmcall_ssac_info->ssac_voice_uptime <= time_get_uptime_secs())
  {
    CM_MSG_HIGH_0("SSAC voice timer expired");

    /* reset the expired timer */
    cmcall_reset_ssac_timer(SSAC_MMTEL_TYPE_VOICE,
                            SSAC_TIMER_ACTION_STOP);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if ssac video timer expired and reset the timer
  */
  if( cmcall_ssac_info->ssac_video_uptime <= time_get_uptime_secs())
  {
    CM_MSG_HIGH_0("SSAC video timer expired");

    /* reset the expired timer */
    cmcall_reset_ssac_timer(SSAC_MMTEL_TYPE_VIDEO,
                            SSAC_TIMER_ACTION_STOP);
  }

}


/*===========================================================================

  FUNCTION cmcall_ssac_process_srv_update

  DESCRIPTION
    Reset ssac timers if LTE has no srv but GWT is in service.

  DEPENDENCIES

  RETURN VALUE
    none

  SIDE EFFECTS
    none

===========================================================================*/
void cmcall_ssac_process_srv_update(void)
{
  boolean lte_in_srv = FALSE;
  boolean g_in_srv = FALSE;
  boolean w_in_srv = FALSE;
  boolean tds_in_srv = FALSE;
  cmcall_ssac_s_type  *cmcall_ssac_info = cmcall_ssac_ptr();

  /* Return if both ssac timers are not running
   */
  if (cmcall_ssac_info->ssac_voice_uptime == CMCALL_SSAC_UPTIME_NONE &&
      cmcall_ssac_info->ssac_video_uptime == CMCALL_SSAC_UPTIME_NONE)
  {
    return;
  }

  /* Get the current 3GPP service status
   ** For GWT, check if it has full service
   */
  lte_in_srv = cmss_is_3gpp_in_srv(SYS_SYS_MODE_LTE, FALSE);
  g_in_srv = cmss_is_3gpp_in_srv(SYS_SYS_MODE_GSM, TRUE);
  w_in_srv = cmss_is_3gpp_in_srv(SYS_SYS_MODE_WCDMA, TRUE);
  tds_in_srv = cmss_is_3gpp_in_srv(SYS_SYS_MODE_TDS, TRUE);

  /* If LTE is not service, but GWT is in full service, reset ssac timers
   */
  if(!lte_in_srv && (g_in_srv || w_in_srv || tds_in_srv))
  {
    CM_MSG_HIGH_0("GWT is in service");
    cmcall_reset_ssac_timer(SSAC_MMTEL_TYPE_VOICE_VIDEO,
                            SSAC_TIMER_ACTION_CANCEL);
  }
}
#endif

#ifdef FEATURE_MMODE_SC_SGLTE
cm_call_cmd_err_e_type    cmcall_sglte_determine_ss_per_call_type(
    sys_modem_as_id_e_type as_id,
    cm_call_type_e_type   call_type,
    cm_ss_e_type         *ss
)
{
  sd_ss_srv_domain_pref_e_type domain = SD_SS_SRV_DOMAIN_PREF_NONE;

  /* Determine which domain the call type will utilize */
  domain = cmcall_get_domain_for_call_type(call_type);

  if (domain == SD_SS_SRV_DOMAIN_PREF_NONE)
  {
    CM_ERR_1("SGLTE: Invalid call type %d",call_type);
    return CM_CALL_CMD_ERR_CALL_TYPE_P;
  }

  /* Determine which domain will be utilized for the call type */
  *ss = cmmsc_sglte_get_ss_for_domain(as_id, domain);
  if(*ss == CM_SS_NONE)
  {
    CM_ERR_1("SGLTE: Invalid call type %d",call_type);
    return CM_CALL_CMD_ERR_SRV_STATE_S;
  }

  return CM_CALL_CMD_ERR_NOERR;
}
#endif

#if defined(CM_GW_SUPPORTED) || defined(FEATURE_MMODE_SC_SGLTE)

sd_ss_srv_domain_pref_e_type cmcall_get_domain_for_call_type(
    cm_call_type_e_type      call_type
)
{
  switch(call_type)
  {

    case CM_CALL_TYPE_VOICE:
    case CM_CALL_TYPE_CS_DATA:
    case CM_CALL_TYPE_SMS:
    case CM_CALL_TYPE_SUPS:
    case CM_CALL_TYPE_EMERGENCY:
      return SD_SS_SRV_DOMAIN_PREF_CS_ONLY;

    case CM_CALL_TYPE_PS_DATA:
      return SD_SS_SRV_DOMAIN_PREF_PS_ONLY;

    case CM_CALL_TYPE_TEST:
    case CM_CALL_TYPE_VT:
    case CM_CALL_TYPE_VT_LOOPBACK:
    case CM_CALL_TYPE_VS:
      return SD_SS_SRV_DOMAIN_PREF_CS_PS;

    /* Non 3gpp call type */
    case CM_CALL_TYPE_OTAPA:
    case CM_CALL_TYPE_STD_OTASP:
    case CM_CALL_TYPE_PS_DATA_IS707B:
    case CM_CALL_TYPE_PD:
    default:
      return SD_SS_SRV_DOMAIN_PREF_NONE;

  }
}
#endif


#ifndef FEATURE_MMODE_LTE_ONLY
/*===========================================================================

FUNCTION cmcall_overall_non_ps_call_state_per_subs

DESCRIPTION
  Returns overall call state except ps call.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_overall_call_state_e_type  cmcall_overall_non_ps_call_state_per_subs
(
  cm_ss_e_type      ss
)
{
  cm_call_id_type call_id;                        /* Loop index */
  cmcall_s_type *call_ptr;                        /* Call object pointer */
  cm_overall_call_state_e_type overall_call_state
        = CM_OVERALL_CALL_STATE_IDLE;/* Initialize to default return value */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( call_id = 0; call_id < ARR_SIZE(cm_call_obj_array); call_id++)
  {
    call_ptr = cm_call_obj_array[call_id];
    if ( call_ptr == NULL )
    {
      continue;
    }

    /* This check will make sure that the following happens
    ** - When ss = MAIN, consider calls on MAIN, HDR, WLAN stacks
    ** - When ss = HYBR_2, consider calls only on HYBR_2
    */

    if( ( (ss == CM_SS_HYBR_2 || ss == CM_SS_HYBR_3) && (call_ptr->ss != ss) ) ||
        ( (ss == CM_SS_MAIN)   && (call_ptr->ss == CM_SS_HYBR_2 || call_ptr->ss == CM_SS_HYBR_3) )
      )
    {
      continue;
    }

    if( call_ptr->call_type == CM_CALL_TYPE_PS_DATA )
    {
      continue;
    }

    if ( call_ptr->call_state == CM_CALL_STATE_CONV )
    {
      overall_call_state = CM_OVERALL_CALL_STATE_CONV;
      break;
    }
    else if ( call_ptr->call_state == CM_CALL_STATE_ORIG )
    {
      if ( overall_call_state == CM_OVERALL_CALL_STATE_INCOM )
      {
        overall_call_state = CM_OVERALL_CALL_STATE_ORIG_INCOM;
      }
      else if ( (overall_call_state == CM_OVERALL_CALL_STATE_IDLE)
                ||
                /* When call obj array has a smaller call id in
                ** RELEASE state and higher call id in orig, the
                ** overall state should be ORIG
                */
                (overall_call_state == CM_OVERALL_CALL_STATE_RELEASE)
              )
            {
              overall_call_state = CM_OVERALL_CALL_STATE_ORIG;
            }
    }
    else if ( call_ptr->call_state == CM_CALL_STATE_INCOM )
    {
      if ( overall_call_state == CM_OVERALL_CALL_STATE_ORIG )
      {
        overall_call_state = CM_OVERALL_CALL_STATE_ORIG_INCOM;
      }
      else if ( overall_call_state == CM_OVERALL_CALL_STATE_IDLE )
      {
        overall_call_state = CM_OVERALL_CALL_STATE_INCOM;
      }
    }
    else if ( call_ptr->call_state == CM_CALL_STATE_RECALL_RSP_PEND)
    {
      if ( overall_call_state == CM_OVERALL_CALL_STATE_IDLE )
      {
        overall_call_state = CM_OVERALL_CALL_STATE_RECALL_RSP_PEND;
      }
    }
    else if (call_ptr->call_state == CM_CALL_STATE_IDLE)
    {
      if(overall_call_state > CM_OVERALL_CALL_STATE_IDLE)
      {
        continue;
      }
      else if(call_ptr->call_subst.idle == CM_CALL_IDLE_SUBST_ENDWAIT)
      {
        overall_call_state = CM_OVERALL_CALL_STATE_RELEASE;
      }
    }
    /* ( call_ptr->call_state == CM_CALL_STATE_CONV ) */
  } /* for ( call_id = 0; call_id < CM_CALL_ID_MAX; call_id++) */

  return overall_call_state;

} /* cmcall_overall_call_state() */
#endif

#ifdef FEATURE_MMODE_DUAL_SIM
/*===========================================================================

FUNCTION cmcall_check_and_end_calls_on_other_subs

DESCRIPTION
  End all the CS calls on another sub to give preference to emergency call on
  this sub. Applicable for DSDS/TSTS config. Not needed for DSDA.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
static void cmcall_check_and_end_calls_on_other_subs
(
  sys_modem_as_id_e_type asubs_id,
  cm_call_type_e_type call_type
)
{

  cmcall_s_type *call_ptr;                        /* Call object pointer */
  cm_iterator_type  call_itr;
  cm_call_mode_info_e_type info_type;
  cm_call_id_type actv_callid = CM_CALL_ID_INVALID;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if ( (call_ptr->asubs_id != asubs_id) &&
         (call_ptr->call_type  == call_type) )
    {

      call_ptr->end_status = CM_CALL_END_CLIENT_END;
      info_type = call_ptr->cmcall_mode_info.info_type;
      actv_callid = call_ptr->call_id;

      CM_MSG_HIGH_2("Ending call with id %d, type %d",actv_callid,call_ptr->call_type);

      #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
      if( info_type == CM_CALL_MODE_INFO_CDMA )
      {
        cmxcall_end_call(call_ptr->call_id,
                         call_ptr->end_status,
                         CAI_REL_NORMAL);
      }
      else
      #endif

      #if defined(FEATURE_MMODE_GW) && !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
      if( info_type == CM_CALL_MODE_INFO_GW_CS )
      {
        cmwcall_end_req( call_ptr );   /* send END_REQ event */
        if( (call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE) &&
                 (call_ptr->call_state == CM_CALL_STATE_ORIG))
        {
          cmcall_end( call_ptr );  /* No origination pending, just end */
        }
        else
        {
          cmwcall_send_cs_end_req(call_ptr->gw_connection_id, call_ptr->ss);
        }
      }
      else
      #endif /* #if defined(FEATURE_MMODE_GW) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE) */
      {
        cmcall_end( call_ptr );
      }
    }

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while */

}/* cmcall_check_and_end_voice_call_on_other_subs */
#endif

#ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
/*===========================================================================

FUNCTION cmcall_is_ssac_p00

DESCRIPTION
  Check if the SSAC barring factor for MMTEL voice is p00

DEPENDENCIES
  None

RETURN VALUE
  TRUE - mmtel voice barring factor is p00
  FALSE - mmtel voice barring factor is not p00

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_ssac_p00(void)
{
  return (cmcall_ssac_ptr()->ssac_params.barring_factor_mmtel_voice == LTE_RRC_AC_BARRING_FACTOR_P00);
}
#endif


#if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
/*===========================================================================

FUNCTION cmcall_is_esr_allowed_in_hvolte

DESCRIPTION
  This function tells whether ESR is allowed in hvolte(single mode)

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_is_esr_allowed_in_hvolte( void )
{
  cmsds_s_type *sds_ptr = cmsds_ptr();
  if((sds_ptr->is_esr_supported || sds_ptr->supportfordualrxues)
#if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)||defined(FEATURE_TDSCDMA))
     && cmregprx_get_stack_info_ptr(SD_SS_MAIN)->state == CMREGPRX_STATE_ACTIVE
#endif
     && cmss_ptr()->info.srv_status == SYS_SRV_STATUS_SRV
     && cmss_ptr()->info.sys_mode == SYS_SYS_MODE_LTE)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
#endif

/*===========================================================================

FUNCTION cm_find_call_id_gw_per_sub

DESCRIPTION
  Find the call ID with gw(SM) connection_id per sub
  This will search for if there is any call_ptr which has gw connection id
  including LTE or GW call.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE: if operation succeded
  FALSE: If operation failed.

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
boolean cm_find_call_id_gw_per_sub
(
  byte                    connection_id,
  cm_call_id_type         *call_id_ptr,
  sys_modem_as_id_e_type  asubs_id
)
{
  cmcall_s_type      *call_ptr = NULL;
  cm_iterator_type   call_itr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!call_id_ptr)
  {
    return FALSE;
  }

  *call_id_ptr = (byte)CM_CALL_ID_INVALID;

  /*
  ** find the matching call ID
  */

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if((call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS) &&
	   (call_ptr->asubs_id == asubs_id) &&
       (call_ptr->gw_connection_id == connection_id))
    {
      *call_id_ptr = call_ptr->call_id;
      return TRUE;
    }
    #if defined(FEATURE_CM_LTE)
    else if((call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_LTE) &&
       (call_ptr->cmcall_mode_info.info.lte_info.gsm_umts_connection_id.valid) &&
	   (call_ptr->asubs_id == asubs_id) &&
       (call_ptr->cmcall_mode_info.info.lte_info.gsm_umts_connection_id.connection_id == \
        connection_id))
    {
      *call_id_ptr = call_ptr->call_id;
      return TRUE;
    }
    #endif

    call_ptr = cmcall_obj_get_next(&call_itr);

  }

  return FALSE;

} /* cm_find_call_id_gw_per_sub() */

#if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
/*===========================================================================

FUNCTION cmcall_end_all_non_emerg_non_data_calls

DESCRIPTION
  Ends all non-emergency non-Data calls.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void                                  cmcall_end_all_non_emerg_non_data_calls(

       uint8                          subs_id_bitmask,

       cm_call_end_e_type             end_status
)
  {

  CMCALL_FOR_EACH_CALL_DO
     if( this_call->call_state != CM_CALL_STATE_IDLE
         &&
      (!( this_call->call_type == CM_CALL_TYPE_PS_DATA ||
           this_call->call_type == CM_CALL_TYPE_EMERGENCY ))
        &&
        (BM( this_call->asubs_id ) & subs_id_bitmask )
      )
    {
      cm_ss_e_type      ss     = this_call->ss;
      cm_act_id_type    act_id = (cm_act_id_type)this_call->call_id;
      cm_orig_q_s_type *call_stck_obj_ptr = NULL;

      this_call->end_status = end_status;

      if((end_status == CM_CALL_END_OFFLINE) && (this_call->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP))
      {
        this_call->end_status = CM_CALL_END_LPM;
      }
	  
      cmcall_end_call(this_call);

      call_stck_obj_ptr  = cmtask_orig_para_search_act_id(ss,act_id);
      if ( call_stck_obj_ptr != NULL )
      {
        cmtask_orig_para_delete(ss,call_stck_obj_ptr,TRUE);
      }
    }

  CMCALL_END_FOR
}
#endif

#ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
/*===========================================================================

FUNCTION  cmcall_check_existing_call_with_type

DESCRIPTION
  Checks if there is a call of the given type and in the given state which is
  different from the provided call id 

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if a call can be found with the requested parameters.

SIDE EFFECTS
  none

===========================================================================*/

boolean cmcall_check_existing_call_with_type (

                  cm_call_type_e_type   call_type,
                  /* Given call type */

                  cm_call_state_e_type   call_state,
                  /* Given call state */
                  cm_call_id_type       new_call_id
				  /* to check for call other than given call id */
)
{
  cmcall_s_type   *call_ptr;                        /* Call object pointer */
  cm_iterator_type call_itr;
      /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    if (call_ptr->call_type == call_type && call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_IP 
		&& call_ptr->call_state != call_state && call_ptr->call_id !=new_call_id)
    {
      return TRUE;
    }

    call_ptr = cmcall_obj_get_next(&call_itr);

  } /* while  */

  return FALSE;
} /* cmcall_check_existing_call_with_type () */



/*===========================================================================

FUNCTION cmcall_send_mmtel_status

DESCRIPTION
  Send the mmtel audio/video status to MM

DEPENDENCIES

RETURN VALUE
  Return TRUE if the command is successfully send. Otherwise FALSE.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmcall_send_mmtel_status(

 boolean      voice_status,

 boolean      video_status,
	
 boolean      ip_sms_staus,

 sys_modem_as_id_e_type as_id
       
)
/*lint -esym(715,is_ims_voice_avail)*/
{

  #ifdef FEATURE_CM_LTE

  mm_cmd_type                   *mm_ptr;
      /* MM command pointer */

  mm_ptr = mm_cm_get_cmd_buf(MMCM_MMTEL_CALL_INFO);

  if (mm_ptr == NULL)
  {
    CM_ERR_0("MMCM_MMTEL_CALL_INFO: out of MM cmd buffer");
    return FALSE;
  }

  mm_ptr->cmd.cm_mm_mmtel_call_info.voice_call_status = voice_status;

  mm_ptr->cmd.cm_mm_mmtel_call_info.video_call_status = video_status;
  
  mm_ptr->cmd.cm_mm_mmtel_call_info.sms_status = ip_sms_staus;

  mm_put_cmd( mm_ptr );

  CM_MSG_HIGH_2("Send MMTEL status to MM, voide: %d,video: %d ", voice_status ,video_status );

  return TRUE;

  #else 

  return TRUE;

  #endif

}

/*===========================================================================

FUNCTION cmcall_send_msgr_voice_video_status

DESCRIPTION
 Send voice/video status during volte/vt origination and end
 Sent throught MSGR

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_send_msgr_voice_video_status( 

  boolean      voice_status,

  boolean      video_status,
  
  boolean      ip_sms_status,

  sys_modem_as_id_e_type as_id

)
{
  errno_enum_type msgr_error = E_SUCCESS;

  cm_volte_call_status_ind_s_type *volte_status = (cm_volte_call_status_ind_s_type *)cm_mem_malloc
                                      (sizeof(cm_volte_call_status_ind_s_type));

  volte_status->sms_status  = ip_sms_status;
  volte_status->voice_call_status  = voice_status;
  volte_status->video_call_status   = video_status;
  volte_status->msg_hdr.inst_id= SYS_AS_ID_TO_INST_ID(as_id);

  msgr_error =  cm_msgr_send( MM_CM_VOLTE_STATUS_IND, MSGR_MM_CM,
            (msgr_hdr_s*)volte_status, sizeof(cm_volte_call_status_ind_s_type) );

  CM_MSG_HIGH_3("CM->MSGR: VOLTE_STATUS_IND, msgr_error=%d, inst_id=%d, voice_status=%d", 
                 msgr_error,
                 volte_status->msg_hdr.inst_id,
                 voice_status);
  cm_mem_free(volte_status);
  return;

}

/*===========================================================================

FUNCTION cmcall_check_and_send_mmtel_status

DESCRIPTION
  This function checks VOLTE call in progress and provides
  updated mmtel status to REG
  1)If VOLTE started, update voice status =1
  2)if VOLTE ended, update voice status =0
  3)if VT started, update video status =1
  4)if VT ended, updated video status =0
  5)During SRVCC, update voice/video =0
  6)During CSFB, update voice =0.


 call_ptr = call which gets started or ended 

 call_start = call ending or starting


DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_check_and_send_mmtel_status( 
 cmcall_s_type  *call_ptr, 

 boolean call_start
 
 )
{
#if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
  boolean vt_call =cmcall_check_existing_call_with_type(CM_CALL_TYPE_VT,CM_CALL_STATE_IDLE, call_ptr->call_id);

  boolean voice_call =cmcall_check_existing_call_with_type(CM_CALL_TYPE_VOICE,CM_CALL_STATE_IDLE, call_ptr->call_id);

  boolean e911_call  = cmcall_check_existing_call_with_type(CM_CALL_TYPE_EMERGENCY,CM_CALL_STATE_IDLE, call_ptr->call_id);

  boolean voice_mmtel = FALSE;

  boolean video_mmtel = FALSE;

  boolean ip_sms_mmtel = FALSE; /* SMS status not sent from CM for internal IMS */

  boolean status_change = FALSE;


  /* during SRVCC of VT call, end VT call type */
  cm_call_type_e_type call_type = call_ptr->call_type;

  /* indication status change for VOLTE call only */
  if(call_ptr->sys_mode != SYS_SYS_MODE_LTE )
  {
	CM_MSG_HIGH_1("call ptr sysmode:%d ,not VOLTE" ,call_ptr->sys_mode );
	return;
  }

CM_MSG_HIGH_4("vt_call %d, volte %d, call type %d, e911_call %d", 
               vt_call ,voice_call ,call_ptr->call_type, e911_call);

  /* Send volte/vt call start or end per call type */
  switch( call_type)
  {
    case CM_CALL_TYPE_VOICE:
    case CM_CALL_TYPE_EMERGENCY:
    if(call_start)
    {
      if((!voice_call && !e911_call))
      {
        voice_mmtel= TRUE;
	status_change= TRUE;
      }
    }
    else
    {
      if((!voice_call && !e911_call) )
      {
	voice_mmtel= FALSE;
	status_change= TRUE;
      }

    }
    video_mmtel  =vt_call;
    break;
    case CM_CALL_TYPE_VT:
     if(call_start)
     {
       if (!vt_call )
       {
	 video_mmtel = TRUE;
	 status_change= TRUE;
        }
      }
      else
      {
	if((!vt_call ))
	{
          video_mmtel = FALSE;
          status_change= TRUE;
	}

       }
       voice_mmtel  = voice_call;
       break;

    default:
	  break;

  }

  if(status_change)
  {
    cmcall_send_mmtel_status(voice_mmtel,video_mmtel,ip_sms_mmtel,call_ptr->asubs_id);
    cmcall_send_msgr_voice_video_status(voice_mmtel,video_mmtel,ip_sms_mmtel,call_ptr->asubs_id);
  }
#endif

}

/*===========================================================================

FUNCTION cmcall_send_ext_volte_state

DESCRIPTION
  Updates external VOLTE call status to Lower layers

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_send_ext_volte_state
(
  const  cm_ext_volte_call_state_ind_s_type      *volte_state
)
{

  boolean voice_mmtel = FALSE;

  boolean video_mmtel = FALSE;

  boolean sms_mmtel = FALSE;

  CM_MSG_HIGH_2("MSGR->CM: EXT_VOLTE_STATE, call type set: %d, inst_id=%d",  
                   volte_state->call_type, volte_state->msg_hdr.inst_id);
 
  if(volte_state->call_type & CM_CALL_TYPE_VOICE_MASK)
  {

    voice_mmtel = volte_state->call_state ;
  }

  if(volte_state->call_type & CM_CALL_TYPE_VT_MASK)
  {

    video_mmtel = volte_state->call_state ;
  }
  
  /* IP SMS over external IMS */
  if(volte_state->call_type & CM_CALL_TYPE_SMS_MASK)
  {

     sms_mmtel = volte_state->call_state ;
   }

  cmcall_send_mmtel_status(voice_mmtel,video_mmtel,sms_mmtel, volte_state->asubs_id);
  cmcall_send_msgr_voice_video_status(voice_mmtel,video_mmtel, sms_mmtel,volte_state->asubs_id);
 
}

#endif
/*===========================================================================

FUNCTION cmcall_shorten_sr_duration

DESCRIPTION
 If UE is moving from no service to service on CAMPED_IND, shorten the silent
 redial timer on pending calls so that NAS does not start T3240

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void                            cmcall_shorten_sr_duration(

       cm_ss_e_type                    ss,

       sys_sys_mode_e_type             mode
)
{
  #if defined(CM_GW_SUPPORTED) 
  cmcall_s_type   *call_ptr; /* Call object pointer */
  cm_iterator_type call_itr; /* call Object Iterator */

  /*-----------------------------------------------------------------------*/

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {
    cm_sr_s_type *sr_ptr = &call_ptr->sr ;
    dword         uptime = time_get_uptime_secs();
    sd_ss_srv_domain_pref_e_type srv_domain = SD_SS_SRV_DOMAIN_PREF_NONE;

    /* Check if the call is CS domain */
    srv_domain = cmmsc_get_common_domain( SD_SS_SRV_DOMAIN_PREF_CS_ONLY,
                        cmcall_get_domain_for_call_type(call_ptr->call_type));

    /* If there is a CS call currently in Silent redial, send the origination
    ** command down immediately.
    ** if call is never sent to NAS no need to do shorten SR
    */
    if ( call_ptr->call_state == CM_CALL_STATE_ORIG &&
         call_ptr->ss == ss                         &&
         sr_is_more_redial(sr_ptr, uptime)          &&
         (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS) &&
         (call_ptr->sr.orig_sent_on_mode != SYS_SYS_MODE_NONE || call_ptr->sr.sr_left < call_ptr->sr.orig_plan_ptr->sr_max_dial) &&
         (srv_domain != SD_SS_SRV_DOMAIN_PREF_NONE)   &&
         (!call_ptr->is_vowlan)                     &&
         ((call_ptr->call_type == CM_CALL_TYPE_VOICE) || (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)) &&
         call_ptr->cc_object_ptr == NULL )
        /* For Call type Voice and Emergency only, we need to kick the logic of shortening of SR*/
        /* As of now, VT Call triggered over IMS will not be handled because VT over GW isn't supported */
    {
      CM_MSG_HIGH_3( "Reduce next_orig_uptime from %d to %d for call %d",sr_ptr->next_orig_uptime,uptime,call_ptr->call_id);
      /* Shorten the SR timer.*/
      sr_ptr->next_orig_uptime = uptime;
      /* Fake ok_to_orig. */
      cmcall_update_ok_to_orig(call_ptr, TRUE);

      /*Update the info_type based on the mode of CAMPED_IND*/
      cmcall_process_ok_to_orig_gw(mode, call_ptr);

        /* Call origination sent to NAS. */
      cmcall_orig_proc(call_ptr);
    }
    call_ptr = cmcall_obj_get_next(&call_itr);
  }
  #endif
}

/*===========================================================================

FUNCTION cmcall_set_cutom_voice_sr_timer_update_orig_plan

DESCRIPTION
  This function updates the custom voice SR timer info read from
  NV to the static structure. These value will be used to increase
  voice call silent redial duration at run time.
  
DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmcall_voice_sr_max_timer_set( boolean is_enabled,
                                    dword   custom_voice_sr_timer,
                                    sys_modem_as_id_e_type asubs_id)
{

  if(!(asubs_id >= SYS_MODEM_AS_ID_1 && asubs_id < MAX_SIMS
    && asubs_id < SYS_MODEM_AS_ID_NO_CHANGE))
  {
    asubs_id = SYS_MODEM_AS_ID_1;
  }
  /* Update the flag value to the static structure */
  cmcall_sr_timer_info[asubs_id].custom_sr_timer_enabled = is_enabled;

  if ( cmcall_sr_timer_info[asubs_id].custom_sr_timer_enabled &&
       custom_voice_sr_timer > 0 &&
       custom_voice_sr_timer <= 1200
     )
  {
    /* If NV is enabled, and the voice timer value falls in the 0 - 1200 secs
    ** range, update the static struct sr timer value & the orig plan table 
    ** with the custom sr timer value 
    */
    cmcall_sr_timer_info[asubs_id].sr_max_time = custom_voice_sr_timer;
  }
  else
  {
    /* If NV is disabled or value is out of range CM will use the default 30 secs
    ** silent redial timer as defined by CMCALL_SR_MAX_TIME. No need to update 
    ** orig plan table here as it has already been compiled with default value 
    */

    cmcall_sr_timer_info[asubs_id].sr_max_time = CMCALL_SR_MAX_TIME;
  }
}

#if (defined(FEATURE_MMODE_3GPP2) || defined(FEATURE_MMODE_GW) )
/*===========================================================================
FUNCTION cmcall_voice_sr_max_time

DESCRIPTION
  This function returns the Silent redial max time value for voice calls
  
DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
dword cmcall_voice_sr_max_timer_get(sys_modem_as_id_e_type asubs_id)
{
  if(!(asubs_id >= SYS_MODEM_AS_ID_1 && asubs_id < MAX_SIMS
    && asubs_id < SYS_MODEM_AS_ID_NO_CHANGE))
{
    asubs_id = SYS_MODEM_AS_ID_1;
  }
  return cmcall_sr_timer_info[asubs_id].sr_max_time;
}
#endif

#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH

/*===========================================================================
FUNCTION cmcall_gps_search_call_cleanup_required

DESCRIPTION
  This function returns the TRUE/FALSE if call clean up required
  
DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

boolean cmcall_gps_search_call_cleanup_required( void)
{
  boolean res = FALSE;
  cmph_s_type           *ph_ptr = cmph_ptr();
  CM_ASSERT( ph_ptr != NULL );
#if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
  if( cmcall_find_active_cs_call_per_subs(SYS_MODEM_AS_ID_1) != CM_CALL_ID_INVALID )
  {
    cmcall_endall_non_emerg_cs_calls(CM_CALL_END_REL_NORMAL,BM(SYS_MODEM_AS_ID_1));
    res = TRUE;
  }
#endif
  return res;

}

#endif



