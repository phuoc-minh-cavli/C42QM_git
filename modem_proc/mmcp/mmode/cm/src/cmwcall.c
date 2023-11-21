/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R   W C D M A   C A L L   M O D U L E

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

  cmcall_is_waiting_for_reply
    Check whether the call object is currently waiting for a reply.

  cmwcall_info_get
    Copy the current call state information into a specified buffer.

  cmwcall_client_cmd_proc
    Process clients' call commands.

  cmwcall_rpt_proc
    Process Lower Layer call commands.

  cmcall_timer_proc
    Process call timer events


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmcall_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2001 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/cm/src/cmwcall.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/02/14   ss      Check for on-going voice or emergency calls before forcing 
                   user prefs down to MMoC in SGLTE 
12/18/13   sm      Update call preferences to GW upon SRVCC complete
12/13/13   sm      Moved active SRVCC call API to HO_START  indication
10/24/13   ss      FR17289 No two Simultaneous 1x MO Calls
09/16/13   sm      fixed KW errors in srvcc handler
08/28/13   jvo     Added 2 checks for NULL to fix static analysis errors
08/20/13   jvo     Added 2 checks for NULL on call_ptr to propagate a fix
                   of static analysis errors in cmwcall_client_cmd_forward_ll
08/16/13   xs      VT SRVCC(video session dropped)
08/13/13   sm      Midcall SRVCC feature - Phase2
07/15/13   sm      Mid Call SRVCC feature Support-phase1
05/21/13   xs      No E911_EXIT after SRVCC
06/23/13   xs      Drop duplicate HO complete indication for L->G SRVCC
06/18/13   gm      FR 2617 - OMADM IMS and ADMIN PDN Disable Requirement
06/07/13   xs      Add voice ho type in HO_COMPLETE
05/21/13   xs      No NAS_E911_EXIT after SRVCC
05/08/13   xs      KW error fix
05/02/13   ss      New API for converting ASCII to BCD
04/27/13   xs      Reject HO events and codec info event when phone is not online
04/24/13   ss      New API for BCD to ASCII Conversion
03/29/13   xs      Restore ph mode pref after SRVCC complete
12/27/13   th      Notify client CM_CALL_EVENT_HO_ events. Send with payload
                   if event is CM_CALL_EVENT_HO_COMPLETE
02/02/12   xs      Add support to set sys mode in L2G SRVCC suppport
01/24/13   mg      Update call type before sending the
                   CM_CALL_EVENT_PS_SIG_REL_CNF event.
01/02/12   xs      Fix issue of not sending IMS SRVCC complete ind when
                   alerting call
12/21/12   xs      Fix issue of not sending IMS SRVCC failure ind
12/04/12   cl      Reject MO call if there's 1X MT call in progress
12/03/12   xs      Added SRVCC Start indication support
11/07/12   mg      Set ctm bit from thin UI if auto answer is enabled.
11/06/12   cl      Reject PS_DATA_AVAIL command during 1X.Voice or CS_only
                   domain
10/18/12   xs      Don't transfer call not in active state after SRVCC
09/26/12   skk     Changes of ESM SM backoff timer requirements
05/28/12   skk     Passing correct subs ID to the SM for
                   SMREG_PS_SIGNALING_REL_REQ command.
05/18/12   jh      Update is_primary_pdp regardless of NW_INITIATED_QOS
04/14/12   xs      Codec info propagation
04/10/12   sk      Fixing the CW and KW warnings
04/02/12   skk     Make DS updated every time when RAT changes.
03/23/12   qf      Ignore CM_MT_END_CALL_IND from NAS when call_state is
                   CM_CALL_STATE_CC_IN_PROGRESS
02/13/12   xs      Unreserve MO connection id when SRVCC HO complete.
01/12/12   aj      update cmpref2_proc_cmd_pref_sys_chgd() param to fix mem
                   issue
01/12/12   fj      Added RAB_RELEASE_IND handling during GWL IRAT.
01/24/12   fj      Added PDP_DEACTIVATE_CNF handling during GWL IRAT.
01/24/12   vk      Changes for 1XCSFB RLF handling.
01/20/12   gm      Data synchronization for CM Call objects
01/12/12   sg      Make changes for TGWL configuration.
01/06/12   gm      Data synchronization changes for freefloating platform
12/21/11   xs      L->W SRVCC support
11/30/11   ab      Support for alerting pattern IE in the MT CC SETUP
10/10/11   jh      Removing profile_number check for SPDP
10/14/11   rm      Pass profile number of PS call to NAS
07/26/11   jh      Marking GW connection IDs on LTE Activate bearer request
                   Adding additional logs for connection_ids
07/24/11   aj      SC SVLTE 1.0 merge to mainline
07/14/11   jh      Added PDP_DEACTIVATE_IND/REJ ACTIVATE_REJ/SEC_REJ handling
                   during IRAT(Still in LTE)
05/24/11   rm      Update SYS_MAX_SEC_PDP_CONTEXT_* values correctly
04/29/11   rm      CMI-4:defeaturise cm_reserve/unreserve_connection_id()
04/21/11   sv      Correct Lint Errors
04/11/11   am      Cleaning stubs and runnning qtf SUTE
03/22/11   gm      Fix to waiting 4s after call end to force THERM EMERG
03/11/11   fj      During GWL IRAT, send SM RAB_REESTAB_REQ if DS indicates
                   GW mode.
02/07/11   fj      Add support to handle CM_PDP_DEACTIVATE_REJ during IRAT.
02/03/11   fj      During IRAT, search both old RAT and target RAT connection
                   id for reject responses.
01/12/11   fj      Add new indication for call object transfer during IRAT.
12/28/10   sv      Fix DSDS code
12/23/10   sv      Integrate DSDS feature
12/02/10   gm      Fix to compiler warnings
11/15/10   rm      Memory optimization changes
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
10/08/10   sg      Add check to block CS_DATA calls
10/04/10   xs      Superlint critical/high error fixed
10/01/10   ak      Merged the Fusion code from Dev branch.
09/22/10   sg      Correcting typo error.
08/26/10   gm      Removed calls to cmnv_read_wait from frequently called APIs
08/02/10   am      Ending the pdp call when unpacking pdp returns error
07/26/10   cl      Initialize stream_id.present
07/22/10   fj      Remove duplicate fields in gsm_wcdma_cmcall_ps_type.
07/12/10   am      Adding pack and unpack functions for subsddress struct.
07/06/10   gm      Fix for Klocwork deferred critical errors.
06/11/10   fj      Add esm_conn_id support for CM_PDP_ACTIVATE_SEC_CNF.
06/10/10   ak      Mainlining memory optimization changes.
05/26/10   fj      Add cmcall_is_no_call_in_lte_ps().
05/21/10   fj      Expose cmwcall_reset_gw_wcdma_ps_info().
05/17/10   fj      Add support for GW to LTE call transfer.
05/05/10   mj      MMGSDI_SESSION_LIB: sub NA only for app type in refresh evt
04/29/10   sv      Add NAS stubs for QTF
04/01/10   mj      Propagate refresh event session ID
03/11/10   xs      Fix TON NPI all zero issue.
02/16/10   sg      CM to send handover complete info to CM client (eCall feature requirement)
02/15/10   rk      Removing is_user_data_present check in cmwcall_get_user_data
01/25/10   am      Adding code to unpack and copy CCP paramater in mn_orig_p.
01/13/10   sv      KW Fixes
12/11/09   sg      Correcting memory leak issue in CM.
12/04/09   rm      PS call info is wrongly overwritten by CS call info
11/17/09   sg      Convert local variable pdp_info into dynamic mem.
10/19/09   ks      returning error immediately when CM not allow
                   3rd sec pdp assocaited with same primary
10/14/09   sg      ZI memory reduction. Mmode should call new Heap Manager API to
                   allocate memory dynamically.
09/23/09   sv      KW Fixes
09/25/09   ks      Adding changes for Nw Initiated QOS
08/04/09   np      Added handling pref mode according to LTE system mode addition
07/01/09   fj      Changes due to LTE merge.
09/01/09   sv      Moving from clk API to time API
09/01/09   ks      CM is not copy tft field when CC modified PS parameter
08/19/09   ks      CM is not allowing three primary pdp call at a time
08/07/09   rn/rm   Centralised call control changes
08/07/09   am      Adding code changing subscription availability on receiving
                   MMGSDI refresh event.
07/15/09   ks      Added fix to indicate call type as CS_DATA on when
                   empty bearer capability is received in MT setup message.
06/10/09   rm      CM should not error fatal for non-EF_RAT refresh.
                   Also CM should handle MMGSDI_REFRESH_NAA_INIT_FCN
06/01/09   rk      correcting cmwcall_send_pdp_abort_req adding condition to
                   check if GW PS call exist,then only send PDP abort req.
04/17/09   ks      Removing FEATURE_CM_DELAY_SETUPRES functionality
                   to avoid tear down of sec pdp due to MT voice call.
04/17/09   ks      Added support for blocking MO/MT voice call on Data Card
04/06/09   mj      Update BOOLEAN macro w/ namespace prefix to CM_BOOLEAN
03/24/09   sv      updated to cmtaski.h
03/23/09   rm      Adding _v.h files from NAS
02/27/09   rm      Adding header files of other MOBs
02/13/09   am      CM changes for UIM header decoupling
01/29/09   rm      RAT balancing phase-2 - Vote YES to EF_RAT refresh during
                   CS/PS calls - MMGSDI changes
01/28/09   rk      Fixing the compilation issue.
01/22/09   rm      Fixing compilation issue when FEATURE_SECONDARY_PDP
                   is disabled
12/17/08   jd      Fixed Lint errors
11/21/08   am      Fixing compilation error for Running off-target tests
11/18/08   am      If FEATURE_DATA_REFRESH_VOTING is defined CM will not
                   vote for PS calls.(moving this from GSDI to MMGSDI)
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
10/31/08   rn      On going offline, send PDP_ABORT to NAS
08/20/08   st      Klockwork fixes
08/05/08   rk      Reverting the call_type and BC when call modification failed.
06/12/08   sv      Resolve Klocwork defects
05/28/08   ks      Updating the associated primary profile number when
                   secondary PDP is promoted to Primary
05/19/08   rk      Added multiparty call indication CM_CALL_EVENT_MPTY_CALL_IND
05/12/08   sg      AMR WB vocoder support for GERAN
05/08/08   sv      Removed snd.h as it is not required for GW
05/02/08   vk      fixed messages for debug
04/29/08   ks      Reverting the changes of Updating the associated primary
                   profile number when secondary PDP is promoted to Primary
04/17/08   ks      allow voice->VT switch MODIFY based on
                   bc_repeat_indicator.repeat_indication
04/17/08   ks      Allow 2nd Primary PDP to use the default profile when
                   1st Primary PDP uses it
03/31/08   ks      Updating the associated primary profile number
                   when secondary PDP is promoted to Primary
03/05/08   vk      corrected code for HYBR_GW configuration
12/12/07   rm      Adding RAT-balancing feature support.
12/07/07   ra      Added support for IMS VCC in HOMER files
11/27/07   sv      Removing Feature FEATURE_GW_CNAP_CODING_SCHEME
11/22/07   sg      Updated handling of CM_MT_MODIFY_IND for nw_initiated_ind
11/19/07   rn      Updated call type to Voice for ITC_AUX_SPEECH
11/19/07   rn      Added support for UE initiated dormancy
11/05/07   sg/vs   Renaming FEATURE_CM_TEMP_NAS_DEP_TBD To
                   FEATURE_GW_CNAP_CODING_SCHEME
10/01/07   rm      Handling of GSDI_CARD_ERR_POLL_ERROR in CM
09/17/07   rn      Updated field enc_alpha of cmcall_s_type to send
                   coding scheme
09/12/07   pk/rm   Merge to mainline - Adding support for handling calls in origination state
                   while service is lost
08/07/07   ic      Lint clean up
06/27/07   cl      Add new CM API to expose cmwcall_get_user_data() to RPC
06/22/07   sk      DDTM rewrite.
06/07/07   rp      Making pdp_info static
04/15/07   rp      Revising logic to set reverse_call_setup_direction
04/15/07   rp      Updating call_ptr->sys_mode
                   with ss_ptr->last_system_reported.
04/13/07   rp      Initializing gw_ps_info.
03/29/07   rp      Setting mode_info.info_type for CM_MANAGE_CALLS_CONF
12/28/06   ka      Correcting logic to set is_call_mod_possible field
11/21/06   ka      Passing rab_id details in CM_CALL_PROGRESS_INFO_IND.
11/09/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
10/12/06   ka/rp   Fixing issue where MO_CALL_CONF changes call type from
                   EMERGENCY to VOICE.
10/03/06   ka      Adding support for IP call on WCDMA.
09/14/06   ka      Adding support for call control on secondary ps calls
08/25/08   ka      Adding support for delaying setup response to let QTV
                   PAUSE streaming session.
08/11/06   ka      Adding CS_DATA call check to the list of calls that are
                   reviewed before sending PS_DATA origination in GSM.
07/06/06   ka/ic   Featurization fixes for PS call control
07/04/06   ka      Changes to support call control on PS calls.
06/05/06   ka/ic   Extending logic for sending user busy with setup response
                   to CS_DATA calls.
05/26/06   pk      Forcing the top origination prerefences for ccbs calls
05/10/06   ka      mode_spec_end used to bunch calls of particular info_type
                   and then send them to wcall, ipcall or xcall. Modifying
                   wcall, ipcall and xcall to end calls of their typeonly.
04/17/06   ka      Allowing PS calls during voice call for GSM in DTM mode.
04/13/06   ka      Adding Config options to deactivate req and pdp_act_rej
                   request. PDP activate reject moves the call to end wait.
                   Explicit call end is received to clear the call.
04/01/06   ka      Changes for emerg serv category during orig, aborting
                   manual search and returning current band with srv ind.
03/24/06   ka      Added VideoTelephony fallback to voice feature for UMTS.
02/08/06   ka      Call end sent for MT ps call that has been answered and is
                   waiting to be connected translates to
                   SMREG_PDP_DEACTIVATE_REQ.
01/13/06   ka      Removed logic to reject MT PS call when MO voice call is
                   in origination.
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/06/06   ka      Adding Release 5 changes including emerg call list,
                   traffic flow template for PDP modify and protocol config
                   options for some of the GW PS call reports.
01/06/06   ic      Lint cleanup
11/30/05   ka      Removed sending nsapi with pdp modify request.
                   Modifying check for user to user data length to be within
                   MAX_USER_USER_DATA_LENGTH only for gw cs originations.
11/23/05   ka      Modification to allow hold/active for ip calls.
11/08/05   sk      Separated CM reports and commands.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
10/24/05   ka      Send call end request to MN on receipt of bearer release
                   indication for CS calls.
09/12/05   pk      Calling PD session grant algorithm, if a session is
                   pending, afer CM_CALL_EVENT_CONNECT
08/16/05   pk      Added support for priority Queues for origination
                   parameters
08/16/05   ka      Correcting favored_mode_pref computation for CCBS recall.
08/02/05   ic      Print value that caused error in default switch case
07/30/05   ka      Handling silent redial for CCBS.
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
06/06/05   ka      Setting correct end_status for PDP_DEACTIVATE_CNF.
                   Sending CALL_EVENT_PROGRESS_INFO_IND for all GW CS calls.
06/13/06   ic      Fixed Lint warnings - removed external declaration of
                   cm_unreserve_connection_id()
06/02/05   ic      Cleaned up cmwcall_sups_cmd_check()
                   Cleaned up cmwcall_cmd_copy_fields()
                   Use cmcall_event() to send CM_CALL_EVENT_SUPS
                   Copy SUPS information in cmwcall_info_get()
06/02/05   ic      Mainlined FEATURE_CIPHER_IND
05/31/05   ic      Fix Lint/compiler warnings in cmwcall_print_pdp_details()
05/19/05   ka      Merging CIPHER_IND feature.
03/30/05   dk      Generate CALL_RETRIEVED event whenever MN informs CM.
03/24/05   ic      Moved processing of CM_CALL_CMD_INFO_GET to cmcall.c
                   (CR#59955)
03/17/05   ic      Send CM_MO_END_X_CALL_REQ for CM_CALL_TYPE_CS_DATA and
                   CM_CALL_TYPE_EMERGENCY
02/22/05   ka      Cleaned up ps orig state check function. Removed copying
                   of cmd fields into call object for rab reestablish
                   request. checking for is_user_data_present in
                   cmwcall_get_user_data.
02/01/05   dk      Added WLAN Support.
01/21/05   ka      end_status in call object is used to record the requestor
                   of call end for a data call (UE terminated or NW
                   terminated). LL_CAUSE refers to UE ending the call and
                   NETWORK_END refers to network ending the call.
01/24/05   ic      Replaced cmph_is_ph_in_emerg_cb_mode() with
                   cmph_is_in_emergency_cb()
01/20/05   ka      Removing condition check where non PS calls were not
                   allowed when SS reported PS_ONLY service. There are cases
                   when PS_ONLY service is reported but emergency call has
                   to be tried for origination.
12/19/04   ic      Moved common call fields from cmwcall_assert() to cmcall.c
                   cmcall_assert()
                   In cmwcall_end_req() coupled setting of call state together
                   with setting of call substate (or otherwise cmcall_assert()
                   would assert that call state is idle but call substate is
                   not set correctly)
12/15/04   jqi     Merged Changing MT_SETUP_IND reject cause to USER_BUSY
                   from NORMAL_CALL_CLEARING.
12/10/04   ic      Removed GET_NSAPI() now that all targets have it defined
                   in NAS
12/06/04   ic      Restored GET_NSAPI(), suppressed Lint warning as
                   depending on target, NAS sm.h may or may not have it
                   defined
12/06/04   ic      Lint cleanup (build 6500 TNAR), removed GET_NSAPI
12/02/04   dk      Added call substates for Packet Data Calls.
11/23/04   ic      Put user-to-user signaling code under FEATURE_UUS
11/20/04   ic      Corrected lint warnings
11/19/04   ka      Adding support for user to user signaling
11/18/04   ka      Adding support for MT PDP feature
11/15/04   ic      Lint cleanup with CM_DEBUG on
11/11/04   ic      Lint cleanup
09/28/04   ka      Unforcing preferences at end of all calls for all cases.
                   ps detach for attach on demand was being done at the end of
                   last ps call. Modified this behavior to end of all calls.
                   Removed functions force and unforce_pref_on_the_fly.
09/22/04   ka      Call substate should not be changed to either HOLD or
                   RETRIEVE depending on SS_NOTIFY indications. Should be
                   changed only by CM_MANAGE_CALLS_CONF.
09/17/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.04)
09/14/04   ic      Call direction clean up.
                   Added calls to cmcall_set_call_direction()
09/07/04   ka      Removed usage of is_orig_sent, mode_in_use and added
                   orig_sent_on_mode
09/03/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.03)
09/03/04   dk      Removed call to ps_sig_down_wait_flag set function.
08/30/04   ka      Added code for handling Sec PDP reject message.
08/27/04   jqi     Added ps dormant optimization.
08/18/04   ka      Call direction now is common to system modes.
                   Setting call direction using a function.
08/11/04   ka      Bearer capability for Anonymous clients (Diag) is populated
                   by CM. System mode for a call is not updated when in
                   NO_SERVICE.
08/05/04   ic      Lint clean up - removed !e641
08/04/04   ic      CM Lint clean up - cmwaoc.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
08/02/04   dk      Initilaize mode_in_use for a particular call while sending
                   origination.
07/28/04   ka      Added support for Secondary PDP context
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
                   Added cmwcall_dummy() to get rid of ADS 1.2 compiler warning
07/21/04   sj      Added support for umts to 1x handover.
07/19/04   ka      Removed handling of error check for CM_CALL_CMD_ORIG, since
                   it is being done in cmcall_client_orig_proc ()
07/14/04   ic      In cmwcall_rpt_proc() moved call to ALS function
                   resolve_mt_call_ind_func() to after call type for the
                   incoming call has been determined
07/12/04   ic      Added support for FEATURE_ALS
07/07/04   dk      Activated support for handling PS_SIG_REL_INDf for CR 43512.
07/06/04   dk      Modified signature of cm_find_connection_id().
07/05/04   dk      Changed signature of cmcall_update_gsdi_and_log_call_state().
                   Changed cmcall_update_gsdi_and_log_call_state() to indicate
                   that CM is not in traffic only when Overall Call state is IDLE.
07/01/04   dk      Added Call Object Iterator.
06/30/04   dk      Lint cleanup and support for handling PS_SIG_REL_IND report.
06/25/04   ws      Preserve number type/number plan after modifications from CC.
06/25/04   dk      Modified to support new call object structure. Removed
                   references to call_object_deallocate() since it is
                   part of call_id_deallcoate. Also changed the references
                   to cmcall_id_deallocate().
06/16/04   ic      Added calls to cmcall_update_call_end_info() before
                   cmcall_event() is called for CM_CALL_EVENT_END_REQ and
                   CM_CALL_EVENT_END events.
                   Set call_connect_time field in call object before cmcall_event()
                   with CM_CALL_EVENT_CONNECT is called.
                   Cleaned up cmwcall_reset() to call cmcall_reset() and reset
                   only GW specific fields.
                   Fixed obvious lint errors (-e715 - variable not referenced;
                   bad enum type used)
06/15/04   dk      Removed calls to cmcall_update_overall_state() and replaced
                   with cmcall_update_gsdi_and_log_call_state(). Initialized
                   IDLE_SUBSTATE of call to NONE during cmwcall_end.
06/10/04   dk      Modified cmwcall_end_req to call the cmph_call_end_req.
06/09/04   ka      Removed conditions that checked for atleast one PS call to
                   be up before PS_DATA_AVAILABLE command can be issued.
06/01/04   npr     Added IP Header and Data compression requirements to GW PS
                   call origination and information structures for GPRS SNDCP.
05/27/04   ic      In cmwcall_client_cmd_proc() call cmcall_client_cmd_err()
                   irrespective of whether error occured or not (client is
                   supposed to be always notified about command processing
                   status)
05/26/04   ka      Removed check that stopped simultaneous CS-DATA + PS_DATA calls.
05/19/04   ka      Removed calls and commented function defintions for forcing and unforcing
                   preferences on the fly.
05/17/04   ic      In cmwcall_reset() removed assertion on call_ptr->init_mask
                   to match behavior of cmxcall_reset()
                   [ needed for CM unit test ].
05/14/04   ka      Fixed Lint errors.
05/06/04   ic      Added assertions on call_ptr->sr.is_orig_sent in
                   cmwcall_send_orig().
                   In cmwcall_reset() initialized srv_type, favored_mode_pref
                   and favored_hybr_pref fields.
                   In cmwcall_cmd_copy_fields() added copying of favored_mode_pref
                   and favored_hybr_pref fields.
04/20/04   ka      Passing the right rab_id down to rabm for a rab-reestablish
04/13/04   ic      CM_ASSERT() clean up
04/08/04   ws      Merged from MSMSHARED_CM.01.00.32.00.03
03/31/04   ka      Removed setting orig_srv_domain_pref to None to
                   avoid failing subsequent PS calls
03/16/04   ka      Merged changes from 6250
                   Signaling connection is set only after CM_CALL_CONNECTED_IND is recvd.
                   Reject PS data call origination when MB is in GSM voice call.
                   Fixed Lint errors.
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
01/29/04   ws      Added call of gsdi event handling for both modes.
01/20/04   ka      Added support for Multiple PDP contexts.
01/13/04   ka      Merged changes from MSM6200
01/06/03   ws      Added handling of gsdi events for card 2.
12/15/03   ws      Set the bcd number present when sending called number to nas.
                   Set signaling_conn_rel_req during originations.
12/12/03   ws      Added cmwcall_set_lmtd_srvc_req
12/08/03   ws      Mainlined umts concurrent services feature.
10/31/03   ws      Check call direction in handling CM_MT_DISC_IND.
                   Merged 6200 changes for handling proc_end_reason.
10/30/03   prk     Updated sys_mode field in call object when the
                   CM_CALL_EVENT_SETUP_IND or CM_CALL_EVENT_CALL_CONF event
                   is sent.
10/27/03   ws      Fix compiler warnings.
10/21/03   ws      Dual Slot Support.
10/21/03   ws      Fixed kc's fix for LTK compiler error.
10/07/03   prk     Added processing of CNM_MN_REJECT_CAUSEs and
                   AS_REJ_RRC_REL_IND during silent redial.
10/01/03   ws      Merged from GW Targets
                   Check for call type instead of mode info in cmd copy fields
09/23/03   ws      Merged fix for SIM fails during an emergency call
                   do not end the call.
09/22/03   RI      Removed AS REL IND (RR/RRC) for Silent redial.
09/18/03   prk     Removed references to ph_ptr in cmwcall_end() to fix
                   compiler warning.
09/18/03   RI      Changes to support CS signaling release during call end.
                   Added AS reasons for silent redial.
09/17/03   ws      Dual valid card support.
09/15/03   tml     call control GCF fixes
09/04/03   RI      Modified SR not to end the origination, when the call is
                   ended by the lower layers.
09/03/03   jds     Correct bug when SIM fails (or is removed) during an
                   emergency call (do NOT end the call).
08/26/03   ws      Picked up call control functions that were lost in Automatic Mode
                   Support Changes.
08/25/03   RI      Modified silent redial, not to end the call if
                   origination request is sent to lower layers.
08/21/03   ws      Added support for Silent Redial and Automatic mode.
08/20/03   AT      Fixed the problem where we tried to reuse the cm_cmd_ptr
                   during GSTK.  This resulted in a double free.
08/18/03   RI      Added checks to determine if call is allowed for
                   MT and MO calls.
08/12/03   prk     Added processing of minimum_qos field.
08/08/03   prk     Called cmwcall_unforce_pref_on_the_fly( ) whenever a PDP
                   deactivation occurs.
07/28/03   vt      Set sys mode on incoming call.
07/25/03   prk     Removed the check of service status during processing of
                   the data available command.
07/21/03   sun     Silent Redial should occur only for MM errors
07/21/03   RI      Added normal call clearing cause for CM_MO_END_X_CALL_REQ.
07/09/03   prk     Checked for a valid SIM state during call origination.
07/08/03   vt      Set sys mode for call events.
06/10/03   RI      Deleted the check for active CS voice calls when a PS data
                   call is originated.
06/09/03   AT      In multimode, the 1x version of cmcall_overall_call_state
                   changed the timing of when the overall call state changes
                   from IDLE to originating.  Made a change to reset the CCM
                   whenever we have a new call and are not in a CONVERSATION
                   instead of when we are in IDLE.  This allows the CCM to
                   reset to 0 before the first call, but removes the race
                   condition of overall_call_state changing from IDLE to
                   either incoming or originating.
06/04/03   RI      Modified the processing of ECT information in the
                   NotifySS message.
                   Added checks to wake up the protocol stack when a RAB
                   re-establish request is received from the client in the
                   deep sleep mode.
05/22/03   sun     Made gw_cs and gw_ps a union in call_cmd_info_s_type
                   Check for CM_CALL_CMD_PS_DATA_AVAILABLE in client_cmd_ll
                   check for rej values instead of cc_cause values for SR
05/13/03   sun     Fixed Silent redial reason. Check for cc reject type instead
                   of cc reject value
05/06/03   kar     Fixed compile error on LTK
05/01/03   jar     Added Handling of CM_GSDI_REFRESH_FCN_IND
04/29/03   sun     Added support for cc reject
04/28/03   AT      Lint cleanup.
04/25/03   prk     Fixed problem where a pending call origination was not
                   being completely being ended if the client ends the call.
04/11/03   prk     Removed references to FEATURE_SUPS, FEATURE_AOC and
                   FEATURE_MMGSDI.
03/27/03   CS      Checked overall state for CM_CALL_CMD_PS_DATA_AVAILABLE.
03/21/03   RI      Call origination request is sent without checking the
                   service status ( CS domain )
03/19/03   RI      Modified the call end handling for emergency call.
                    - restores the original service preference when the
                      emergency call is ended in the limited service mode.
                   Modified PS attach on demand in the limited service mode.
03/18/03   PRK     Modified cmwcall_force_pref_on_the_fly() to allow emergency
                   calls to be made if there is full/limited service.
03/14/03   AT      Added the gstk_is_cc_required function to determine if we
                   should preform call control.
03/13/03   RI      Added sm_unreserve_connection_id() to unreserve connection
                   IDs for PS data calls.
                   Modified cmwcall_unforce_pref_on_the_fly() to change the
                   service domain only in CS_ONLY preference.
03/11/03   ATT     Added support for call control performed by CM clients.
03/06/03   PRK     Fixed a bug in the call origination parameter checking.
                   The call info type was incorrectly being set for PS calls.
02/28/03   AT      Added call control for voice calls.
02/27/03   CS      Added MT/MO indication and transaction ID in CS GSM/WCDMA
                   call info for USAT support.
02/25/03   RI      Modified processing of CM_CALL_CMD_ORIG to support
                   PS attach/detach on demand.
                   Added cmwcall_force_pref_on_the_fly() and
                   cmwcall_unforce_pref_on_the_fly().
                   Modified handling of multiple PS data call origination.
02/13/03   PRK     Mainlined the FEATURE_CONC_SRVC_NEW_API feature.
02/10/03   AT      Added support for CAT_APP clients
02/11/03   PRK     Updated cmwcall_cmd_copy_fields() to not copy the mode info
                   type if it is set to"none".
02/04/03   PRK     Removed static from cmwcall_end_req().
           RI      Corrected the PS command information when ending the call.
           RI      Added support for concurrent services.
           CS      Added silent redial and call blocking for non-emergency calls
                   when card is invalid.
                   Set CM_CALL_END_LL_CAUSE for all call rejection cases.
11/22/02   CS      Added support to allow emergency call to go through when
                   there is a voice call.
01/21/03   ws      Updated copyright information for 2003
12/05/02   PRK     Removed usage of is_allocated  in call object under the
                   FEATURE_CM_ABORT_X_CALL.
11/26/02   vt      Forward declared a function to fix a compile problem.
11/14/02   vt      Merged MM and GW versions
                   Merged mm build changes.
                   Fixed a compile error when RUIM is defined.
                   Set the mode to GW when processing CM_CALL_CONNECTED_IND.
                   Removed usage of is_allocated & call_id in call object.
                   Replaced checking of call id with call object.
                   Added mode param to cmcall_callobject_allocate().
11/5/02    RI      Fixed problem where emergency calls could not be ended.
11/05/02   PRK     Modified cmwcall_client_cmd_check to allow origination of
                   non-emergency calls in a limited service state.  The lower
                   layers are responsible for handling this case.
10/23/02   RI      Modified the handling of CM_CALL_CMD_END.
10/30/02   AT      Added additional support for handling no funds situations.
10/17/02   PRK     Modified cmwcall_end to clear cm_active_ps_call_id if the
                   PS data call is being ended.
10/14/02   PRK     Initialized SMREG_PDP_DEACTIVATE_REQ message in
                   cmwcall_client_cmd_forward_ll().
10/11/02   CS      Added support for invalid SIM handling. Terminate active
                   calls when SIM becomes invalid.
                   Modified silent redial handling.
                   Block out normal calls in limisted service mode.
10/11/02   RI      Modified processing of call barring notification.
09/30/02   RI      Updated the processing of SS Notifications for
                   call forwarding.
09/16/02   PRK     Added FEATURE_UI_CM_NON_SVD.
09/12/02   RI      Updated the copying of forward_cug_info in
                   cmwcall_cmd_copy_fields().
09/11/02   RI      Updated the processing of NotifySS message.
09/06/02   RI      Modified the call cmd handling for CS data calls.
08/30/02   vt      RTRE changes.
08/15/02   RI      Modified the max. forwarded to number size for ECT.
08/13/02   RI      Updated cmwcall_rpt_proc to process CD and ECT.
08/12/02   RI      Added support for ECT CD features.
08/08/02   AT      Added support for CUG and CNAP features.
07/29/02   RI      Modified processing of CM_MT_CALL_SS_IND.
                    - the NotifySS is received only after the Setup Indication.
                      message.
                   Added support for processing cause of no CLI in
                   Setup Indication.
                   Modified processing of CM_CALL_PROGRESS_INFO_IND.
                   to support inband information.
07/23/02   RI      Modified cmwcall_client_cmd_ll_reply()
                    - call ptr is not required for CM_CALL_CMD_SUPS.
07/11/02   PRK     Included cmll.h.
06/27/02   RI      Modified cmwcall_client_cmd_ll_reply()
                    - call ptr is not required for CM_CALL_CMD_SUPS.
06/14/02   AT      Removed silent redial for WCDMA.
06/11/02   RI      Deleted the pointer to the call object parameter in
                   cmwcall_client_cmd_forward_ll(), cmwcall_client_cmd_ll_reply()
                   and cmwcall_client_cmd_check();
                   Updated the procesing of call commands and call event
                   notifications.
                   Modified cmwcall_end() to deallocate call object.

05/31/02   RI      Modified cmwcall_client_cmd_ll_reply to copy sups parameters.
05/28/02   RI      Modified the cmwcall_end() to include the deallocation
                   of call IDs and freeing of call object.
05/23/02   RI      Removed the presentation allowed check in CM_MT_SETUP_IND and
                   CM_CALL_CONNECTED_NUM_IND.
05/16/02   PRK     Changed reference to cmd_info_ptr->call_type to
                   call_cmd_ptr->info.call_type due to FEATURE_AOC being
                   defined.
05/10/02   PRK     Made modifications to support multi-mode.
04/26/02   PRK     Modified cmwcall_reset to "zero out" all fields in the
                   cmcall_s_type structure.
04/24/02   RI      Added processing of CM_RAB_REESTAB_FAIL in cmcall_rpt_proc()
                   Renamed CM_RAB_ESTAB_IND to CM_RAB_REESTAB_IND.
                   Temporary fix in cmcall_client_cmd_check() to bypass the
                   client cmd check for PS call cmds.
04/16/02   CS      Block calls when the current serving system does not suppor the call.
                   Block calls with different service when a call is in progress since
                   simultaneous voice and data calls are not supported.
03/14/02   AT      Modified processing of CM_CALL_CMD_END event in
                   cmwcall_client_cmd_ll_reply to call cmcall_end if
                   no origination is pending.
09/17/01   VT      Main-lined FEATURE_NEWCM
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
                   Do not reset call at end_req and copy cc_cause for Disc msg.
11/21/01   RI      Modified cmwcall_info_get() to copy rab_id for CS Data.
                   Modified PS Data ind/conf processing.
                   Modified cm_find_call_id() to include the connection ID src.
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

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(766,customer.h) */
#include "mmcp_variation.h"
#include "customer.h"  /* Customer configuration file */

#if defined(FEATURE_MMODE_3GPP)
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"       /* Internal interface to cm.c */
#include "comdef.h"    /* Definition for basic types and macros */

#include <string.h>    /* Interface to string library */
#include "rex.h"       /* Interface to rex services */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmclient.h"  /* Interface to CM Client Interface Block */
#include "cmemgext.h"
#include "cmph.h"      /* Interface to CM phone object */
#include "cmaccessctrl.h"
#include "time_svc.h"       /* Interface to clock services */

#include "cmwcall.h"
#include "cmcall.h"    /* Interface to CM call object */
#include "cmll.h"      /* Interface to cm_call_id_allocate/deallocate */
#include "cmll_v.h"

#include "smtask.h"
/*lint -esym(766,smtask_v.h)*/
#include "smtask_v.h"
#include "sm_cm.h"     /* Contains CM - Session Management interface
                       ** primitives
                       */

#include "cmregprx.h"  /* Interface to RegProxy services */
#include "cmregprxi.h"

#include "mmtask.h"    /* MM command buffer allocation */
#include "mmtask_v.h"
#include "cmreply.h"

#if defined(FEATURE_MMODE_3GPP2) && defined(FEATURE_UIM_RUIM)
#include "auth.h"
#endif /* FEATURE_UIM_RUIM */
#include "cmwaoc.h"

#include "cmss.h"
#include "cmutil.h"

#include "mmoc.h"
#include "mmoc_v.h"

#ifdef FEATURE_GSTK
#include "cmtaski.h"
#include "gstk_exp.h"
#include "gstk_exp_v.h"
#endif

#if (defined FEATURE_ALS) && (defined FEATURE_GSM)
#include "cmals.h"
#endif

#ifdef FEATURE_GPSONE
#include "cmxpd.h"
#endif /* FEATURE_GPSONE */

#include "nv.h"
#include "cmnv.h"

#ifdef FEATURE_GSTK
#include "cmmmgsdi.h"

#endif  /* FEATURE_GSTK */

#include "bearer_capability_utils.h"

#ifdef FEATURE_CM_LTE
#include "cmltecall.h" /* Interface to CM LTE call interfaces */
#endif
#ifdef FEATURE_GSM
#include "geran_eng_mode_read_api.h"
#endif

#include "cmpmprx.h"
#include "cmlog_v.h"

#ifdef CM_DEBUG
#error code not present
#endif

#include "cmmsc.h"
#include "cmcc.h"
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/
#if defined (FEATURE_MULTIMEDIA_FALLBACK)
#define REPEAT_INDICATION_NO_MODIFY                  2
#endif

/**--------------------------------------------------------------------------
** Forward declarations
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/
#if defined(FEATURE_MMODE_EMERG_SUPPORT)
static boolean cmwcall_emergency_call_no_sim = FALSE;
#endif

  /* flag to indicate whether we are in an emergency call
     with no SIM */

static boolean lmtd_srvc_requested = FALSE;
  /* flag to indicate whether the service preference was
     changed for the emergency call or not */

#define MAX_BCD_NUMBER_LEN          ( CM_MAX_DIALED_DIGITS / 2 )
  /* Maximum length of the BCD number that can be converted
     to ASCII. This is derived from NV */

#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
/* Mapping table to be used during G/W/L HO */
static cm_ho_type_mapping_table cmwcall_ho_mapping_table[] =

/*{sys_ho_type_e_type ,sys_voice_handover_e_type } */

{
  {SYS_HO_TYPE_GSM_UMTS, SYS_VOICE_HO_G_2_W},

  {SYS_HO_TYPE_GSM_GSM, SYS_VOICE_HO_G_2_G},

  {SYS_HO_TYPE_UMTS_GSM, SYS_VOICE_HO_W_2_G},

  {SYS_HO_TYPE_UMTS_UMTS,SYS_VOICE_HO_W_2_W},

  {SYS_HO_TYPE_LTE_GSM_SRVCC, SYS_VOICE_HO_SRVCC_L_2_G},

  {SYS_HO_TYPE_LTE_UMTS_SRVCC, SYS_VOICE_HO_SRVCC_L_2_W},

  {SYS_HO_TYPE_NONE,SYS_VOICE_HO_NONE}

};

/* identifies IMS call for SRVCC */
static cm_call_id_type ims_call_id;

#if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)

static boolean is_int_ims_enabled_for_srvcc = TRUE;

static cm_srvcc_tid_list srvcc_tid_table[] =


/*{call type combinations,active_call_tid,held_call_tid,incoming_call_tid,outgoing_call_tid,
   transient_call_tid,{conf_participant1_tid,conf_participant2_tid,conf_participant3_tid;
  conf_participant4_tid,conf_participant5_tid}} */

{   /*single call Scenarios */

  /* Single Active Call */
  {CM_SRVCC_CALL_CONV, MT_TID_8,TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID,
    {TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID}},

  /* Single Held Call */
  {CM_SRVCC_CALL_HELD, TID_INVALID,MT_TID_8,TID_INVALID,TID_INVALID,TID_INVALID,
    {TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID}},

  /* Single Incoming Call */
  {CM_SRVCC_CALL_INCOM, TID_INVALID,TID_INVALID,MT_TID_8,TID_INVALID,TID_INVALID,
     {TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID}},

  /* Single Outgoing Call */
  {CM_SRVCC_CALL_OUTGOING, TID_INVALID,TID_INVALID,TID_INVALID,MT_TID_8,TID_INVALID,
    {TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID}},

  /* Active Conference Call */
  {CM_SRVCC_CALL_ACTIVE_CONFERENCE, TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID,
    {MT_TID_8,MT_TID_9,MT_TID_10,MT_TID_11,MT_TID_12}},

  /* Held Conference Call */
  {CM_SRVCC_CALL_HELD_CONFERENCE, TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID,
    {MT_TID_8,MT_TID_9,MT_TID_10,MT_TID_11,MT_TID_12}},

  /*  Active and Held Call */
  {CM_SRVCC_CALL_ACTIVE_AND_HELD, MT_TID_8,MT_TID_9,TID_INVALID,TID_INVALID,TID_INVALID,
    {TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID}},

  /* Active and Alerting incoming  Call */
  {CM_SRVCC_CALL_INCOMING_AND_ACTIVE, MT_TID_8,TID_INVALID,MT_TID_9,TID_INVALID,TID_INVALID,
  {TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID}},

  /* Held and Alerting outgoing Call */
  {CM_SRVCC_CALL_OUTGOING_AND_HELD, TID_INVALID,MT_TID_8,TID_INVALID,MT_TID_9,TID_INVALID,
  {TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID}},

  /* Active and Conference Held Call */
  { CM_SRVCC_CALL_ACTIVE_AND_HELD_CONFERENCE, MT_TID_8,TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID,
  {MT_TID_9,MT_TID_10,MT_TID_11,MT_TID_12,MT_TID_13}},

  /* Held and Conference Active Call */
  { CM_SRVCC_CALL_HELD_AND_ACTIVE_CONFERENCE,TID_INVALID,MT_TID_9,TID_INVALID,TID_INVALID,TID_INVALID,
  {MT_TID_8,MT_TID_10,MT_TID_11,MT_TID_12,MT_TID_13}},

  /* Held and Alerting Incoming Call */
  { CM_SRVCC_CALL_INCOMING_AND_HELD,TID_INVALID,MT_TID_8,MT_TID_9,TID_INVALID,TID_INVALID,
  {TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID,TID_INVALID}},

   /* Transient Conference call scnearios */
  /*  Incoming Call and Conf Held Request   */
  {CM_SRVCC_CALL_INCOMING_AND_CONFERENCE_HELD_REQ, TID_INVALID,TID_INVALID,MT_TID_9,TID_INVALID,TID_INVALID,
  {MT_TID_8,MT_TID_10,MT_TID_11,MT_TID_12,MT_TID_13}},

  /* Conf Retrieve Request and Incoming Call  */
  { CM_SRVCC_CALL_INCOMING_AND_CONFERENCE_RET_REQ,TID_INVALID,TID_INVALID,MT_TID_9,TID_INVALID,TID_INVALID,
  {MT_TID_8,MT_TID_10,MT_TID_11,MT_TID_12,MT_TID_13}},

  /* Conf Held Request  and Outgoing Call */
  { CM_SRVCC_CALL_OUTGOING_AND_CONFERENCE_HELD_REQ,TID_INVALID,TID_INVALID,TID_INVALID,MT_TID_9,TID_INVALID,
  {MT_TID_8,MT_TID_10,MT_TID_11,MT_TID_12,MT_TID_13}},

  /* Conf Retrieve Request and Outgoing Call  */
  {CM_SRVCC_CALL_OUTGOING_AND_CONFERENCE_RET_REQ, TID_INVALID,TID_INVALID,TID_INVALID,MT_TID_9,TID_INVALID,
  {MT_TID_8,MT_TID_10,MT_TID_11,MT_TID_12,MT_TID_13}},

    /* Conference call combinations */

  /* Conference Active  and Incoming Call */
  {CM_SRVCC_CALL_INCOMING_AND_ACTIVE_CONFERENCE, TID_INVALID,TID_INVALID,MT_TID_9,TID_INVALID,TID_INVALID,
  {MT_TID_8,MT_TID_10,MT_TID_11,MT_TID_12,MT_TID_13}},

  /* Conference Held  and Incoming Call  */
  {CM_SRVCC_CALL_INCOMING_AND_HELD_CONFERENCE, TID_INVALID,TID_INVALID,MT_TID_9,TID_INVALID,TID_INVALID,
  {MT_TID_8,MT_TID_10,MT_TID_11,MT_TID_12,MT_TID_13}},

  /* Conference Active   and Outgoing Call */
  {CM_SRVCC_CALL_OUTGOING_AND_ACTIVE_CONFERENCE, TID_INVALID,TID_INVALID,TID_INVALID,MT_TID_9,TID_INVALID,
  {MT_TID_8,MT_TID_10,MT_TID_11,MT_TID_12,MT_TID_13}},

  /* Conference Held   and Outgoing Call  */
  {CM_SRVCC_CALL_OUTGOING_AND_HELD_CONFERENCE, TID_INVALID,TID_INVALID,TID_INVALID,MT_TID_9,TID_INVALID,
    {MT_TID_8,MT_TID_10,MT_TID_11,MT_TID_12,MT_TID_13}}


};

static cm_srvcc_nas_call_mapping_table srvcc_ims_to_nas_call_mapping[] =
{
  /************SINGLE CALL SCENARIO***********************************/
  /*Call is in Connected state -ACTIVE */   
    {CM_CALL_CONV_CALL_ACTIVE,CM_CALL_INCOM_SUBST_NONE,CM_CALL_ORIG_SUBST_NONE,FALSE,CC_STATE_U10, HOLD_IDLE, MPTY_IDLE}, 

    /*Call is in Connected state -HELD */   
    {CM_CALL_CONV_CALL_ON_HOLD,CM_CALL_INCOM_SUBST_NONE,CM_CALL_ORIG_SUBST_NONE,FALSE,CC_STATE_U10,HOLD_CALL_HELD, MPTY_IDLE },

    /*  UE initiated HOLD REQ call  */
    {CM_CALL_CONV_HOLD_REQ_BY_UE,CM_CALL_INCOM_SUBST_NONE,CM_CALL_ORIG_SUBST_NONE,FALSE,CC_STATE_U10, HOLD_REQUEST, MPTY_IDLE },

    /*  UE initiated RETRIEVE REQ call  */
    {CM_CALL_CONV_RETRIEVE_REQ_BY_UE,CM_CALL_INCOM_SUBST_NONE,CM_CALL_ORIG_SUBST_NONE,FALSE,CC_STATE_U10, HOLD_RETRIEVE_REQUEST, MPTY_IDLE},
    

    /************CONF CALL SCENARIO***********************************/
    /*Conf Call is in Connected state -ACTIVE */    
    {CM_CALL_CONV_CALL_ACTIVE,CM_CALL_INCOM_SUBST_NONE,CM_CALL_ORIG_SUBST_NONE,TRUE,CC_STATE_U10, HOLD_IDLE, MPTY_CALL_IN_MPTY}, 

    /*Conf Call is in Connected state -HELD */
    {CM_CALL_CONV_CALL_ON_HOLD,CM_CALL_INCOM_SUBST_NONE,CM_CALL_ORIG_SUBST_NONE,TRUE,CC_STATE_U10,HOLD_CALL_HELD,MPTY_CALL_IN_MPTY },

    /* UE initiated HOLD REQ for Conf call */
    {CM_CALL_CONV_HOLD_REQ_BY_UE,CM_CALL_INCOM_SUBST_NONE,CM_CALL_ORIG_SUBST_NONE,TRUE,CC_STATE_U10, HOLD_REQUEST, MPTY_CALL_IN_MPTY },

    /*UE initiated RETRIEVE REQ for Conf call */
    {CM_CALL_CONV_RETRIEVE_REQ_BY_UE,CM_CALL_INCOM_SUBST_NONE,CM_CALL_ORIG_SUBST_NONE,TRUE,CC_STATE_U10, HOLD_RETRIEVE_REQUEST, MPTY_CALL_IN_MPTY},


    /************INCOM CALL***********************************/
    /*INCOM CALL - user not answered */
    {CM_CALL_CONV_SUBST_NONE,CM_CALL_INCOM_SUBST_ALERTING,CM_CALL_ORIG_SUBST_NONE,FALSE,CC_STATE_U7, HOLD_IDLE, MPTY_IDLE },

    /*INCOM CALL - user  answered */
    {CM_CALL_CONV_SUBST_NONE,CM_CALL_INCOM_SUBST_ANSWERED,CM_CALL_ORIG_SUBST_NONE,FALSE,CC_STATE_U8, HOLD_IDLE, MPTY_IDLE },

    /************OUTGOING CALL***********************************/
    /* Outgoing Pre-Alerting */
    {CM_CALL_CONV_SUBST_NONE,CM_CALL_INCOM_SUBST_NONE,CM_CALL_ORIG_SUBST_PRE_ALERTING,FALSE,CC_STATE_U3, HOLD_IDLE, MPTY_IDLE },

    /*Outgoing Alerting */
    {CM_CALL_CONV_SUBST_NONE,CM_CALL_INCOM_SUBST_NONE,CM_CALL_ORIG_SUBST_ALERTING,FALSE,CC_STATE_U4, HOLD_IDLE, MPTY_IDLE }


};

#endif
#endif
/**--------------------------------------------------------------------------
** Functions - internal
** --------------------------------------------------------------------------
*/

#ifdef FEATURE_MMODE_EMERG_SUPPORT

/*===========================================================================

FUNCTION cmwcall_lmtd_srvc_requsted

DESCRIPTION
  Check if a Limited Service Request was sent for an Emergency call or not.

DEPENDENCIES

  None.

RETURN VALUE
  TRUE - if a Lmtd Service request was sent.
  FALSE - otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmwcall_lmtd_srvc_requested( void )
{
  return lmtd_srvc_requested;

}


/*===========================================================================

FUNCTION cmwcall_reset_lmtd_srvc_req

DESCRIPTION
  Reset the Limited Service Request flag set for an Emergency call.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_reset_lmtd_srvc_req( void )
{
  lmtd_srvc_requested = FALSE;
}

/*===========================================================================

FUNCTION cmwcall_set_lmtd_srvc_req

DESCRIPTION

  Set the Limited Service Request flag set for an Emergency call.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_set_lmtd_srvc_req( void )
{
  lmtd_srvc_requested = TRUE;
}
#endif

/*===========================================================================

FUNCTION cmwcall_fill_call_obj_for_ccbs

DESCRIPTION
  Fill relevent parameters of call object for CCBS recall indication.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  Call object gets modifed.

===========================================================================*/
#if defined (FEATURE_CCBS)
static void cmwcall_fill_call_obj_for_ccbs
(
       cmcall_s_type  *call_ptr
         /* pointer to a call object */
)
{
  cmph_s_type              *ph_ptr         = cmph_ptr();
    /* Pointer to the phone object */

  cmss_s_type              *ss_ptr         = cmss_ptr();
    /* Pointer to the serving system object */

  cm_ss_e_type             ss              = CM_SS_MAIN;
    /* Stack on which this call is originated */

  sys_modem_as_id_e_type   asubs_id;
    /* Sub on which this call is originated */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);
  CM_ASSERT(ph_ptr   != NULL);
  CM_ASSERT(ss_ptr   != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_CS;

  call_ptr->call_state                 = CM_CALL_STATE_RECALL_RSP_PEND;

  /* Set call direction
  */
  cmcall_set_call_direction (call_ptr, CM_CALL_DIRECTION_MO);

  /* Mode in which call is connected
  */
  ss = call_ptr->ss;
  asubs_id = cmcall_get_asubs_id(call_ptr);
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  if(cmss_get_true_sys_mode(ss) != SYS_SYS_MODE_NO_SRV)
  {
    call_ptr->sys_mode = cmss_get_true_sys_mode(ss);
  }
  /* Setting srv_type to automatic
  */
  call_ptr->srv_type                     = CM_SRV_TYPE_AUTOMATIC;

  /* Set favored mode preference.
  ** This function will write CM_MODE_PREF_NONE into favored_mode_pref
  ** if it was not able to compute it.
  */
  if (!cmcall_set_favored_mode_pref_for_origination ( ss,
                                                      call_ptr->call_type,
                                                      call_ptr->srv_type,
                                                      TRUE, /* drs_bit always
                                                            ** set to TRUE.
                                                            */
                                                      0, /* dummy srv_opt
                                                         ** not used for
                                                         ** GW calls
                                                         */
                                                      &call_ptr->favored_mode_pref ))
  {
    CM_ERR_0 (" Err in exeucting cmcall_set_favored_mode_pref_for_origination");
  }

  /* Remove CDMA/AMPS/HDR/LTE/TDS from the favored mode pref.
  */
  call_ptr->favored_mode_pref = cm_remove_mode_pref_components(
                                        call_ptr->favored_mode_pref,
                                        2,
                                        SD_SS_MODE_PREF_CDMA_HDR,
                                        SD_SS_MODE_PREF_TDS_LTE
                                        );

  /* Compute ss for origination.
  */
  if( ss != CM_SS_HYBR_2
    #ifdef FEATURE_MMODE_HYBR3_ENABLED
  && ss != CM_SS_HYBR_3
      #endif
  )
  {
    ss =                cmcall_determine_ss_for_origination
                                       ( call_ptr->favored_mode_pref,
                                         CM_HYBR_PREF_OFF,
                                         CM_CC_MODE_PREF_MAX,
                                         //call_ptr->cmcall_mode_info.info_type,
                                         call_ptr
                                       );
  }

  call_ptr->ss = ss;

  /* Set the force top to true, for concurrent calls, we need OK to orig
  ** for this ccbs response
  */
  call_ptr->force_top_pref = TRUE;

} /* cmwcall_fill_call_obj_for_ccbs () */
#endif

/*===========================================================================

FUNCTION cmwcall_is_user_data_len_within_limit

DESCRIPTION
  For a CS call checks if user to user data length is in range.

DEPENDENCIES
  None

RETURN VALUE
  TRUE   - user data length is in range
  FALSE  - user data length exceeds MAX_USER_USER_DATA_LENGTH

SIDE EFFECTS
  None

===========================================================================*/
static boolean cmwcall_is_user_data_len_within_limit
(
    cm_call_cmd_s_type       *call_cmd_ptr
      /* Pointer to call cmd */
)
/*lint -esym(818, call_cmd_ptr)
** Can't change *ptr to const *ptr as then at the very bottom
** of call stack I'd have to change mem_free() to take const void *ptr
** and it's not the right change as memheap.c might decide to change that
** memory (like clear it for example) before freeing it.
** This is because we allocate commands from a heap and then return that
** memory to the heap.
*/
{
  #ifdef FEATURE_UUS
  cm_call_cmd_info_gw_cs_s_type   *cs_cmd_info_ptr =
                                          WCALL_CMD_INFO_PTR( call_cmd_ptr );

  /* Checking parameters specific to user data (user to user signaling)
  */
  if ( (cs_cmd_info_ptr->mo_user_data.present == TRUE) &&
       (cs_cmd_info_ptr->mo_user_data.user_user_data_length >
                                     MAX_USER_USER_DATA_LENGTH)
     )
  {
     CM_ERR_0 (" user_user_data_length exceeds MAX_USER_USER_DATA_LENGTH");

     return FALSE;
  }
  #else

  SYS_ARG_NOT_USED (call_cmd_ptr);

  #endif

  return TRUE;

} /* cmwcall_is_user_data_len_within_limit () */
/*lint +esym(818, call_cmd_ptr) */

#if !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE) 
/*===========================================================================

FUNCTION cmwcall_check_if_mo_call_possible

DESCRIPTION
  Check if an MO voice/data call is allowed.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE - if allowed, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmwcall_check_if_mo_call_possible( const cmcall_s_type *call_ptr )
{
  cmcall_s_type           *temp_call_ptr;

  boolean orig_call       = TRUE;

  cm_iterator_type        call_itr;
   /* Call Object Iterator */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* do not allow PS data call and CS data call at the same time */
  /* reject a CS data call origination, if there is CS voice call,
     reject a CS voice call origination, if there is a CS data call
  */

  cmcall_obj_iterator_init(&call_itr);

  temp_call_ptr = cmcall_obj_get_next(&call_itr);

  while(temp_call_ptr != NULL)
  {
    if (( ( ( temp_call_ptr->call_type == CM_CALL_TYPE_CS_DATA ) &&
            ( call_ptr->call_type == CM_CALL_TYPE_VOICE )
         )  ||
         ( ( temp_call_ptr->call_type == CM_CALL_TYPE_VOICE) &&
           ( call_ptr->call_type == CM_CALL_TYPE_CS_DATA )
         ))
       )
    {
      #if defined(FEATURE_MMODE_DUAL_SIM)
      if (cmph_is_msim())
      {
        if (temp_call_ptr->asubs_id == call_ptr->asubs_id ||
            cmph_is_dsds()
       #ifdef FEATURE_MMODE_TRIPLE_SIM
       || cmph_is_tsts()
       #endif
      )
        {
          orig_call = FALSE;
        }
      }
      else
      #endif
      {
        orig_call = FALSE;
      }
      break;
    }

    /* Reject voice/cs data calls on subs1, if voice/cs data calls already
    ** exist on subs2
    */
    if( (temp_call_ptr->call_type == CM_CALL_TYPE_VOICE||
         temp_call_ptr->call_type == CM_CALL_TYPE_CS_DATA||
         temp_call_ptr->call_type == CM_CALL_TYPE_EMERGENCY
        ) &&
        (call_ptr->call_type == temp_call_ptr->call_type 
        || temp_call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)&&
        call_ptr->asubs_id  != temp_call_ptr->asubs_id
         &&
         ((cmph_is_dsds())
         #ifdef FEATURE_MMODE_TRIPLE_SIM
         || cmph_is_tsts()
         #endif
         )
      )
    {
      orig_call = FALSE;
      break;
    }

    temp_call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while */

  return orig_call;
}
#endif
#if defined(FEATURE_MMODE_GW) || !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
/*===========================================================================

FUNCTION cmwcall_check_if_mt_call_possible

DESCRIPTION
  Check if a MT voice call is allowed.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE - if allowed, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
static boolean cmwcall_check_if_mt_call_possible( const cmcall_s_type *call_ptr )
{
  cmcall_s_type           *temp_call_ptr;

  boolean                 accept_call = TRUE;

  int                     total_primary = 0;
      /* The total number of primary PDP contexts */

  cmss_s_type             *ss_ptr = cmss_ptr();
      /* Get pointer to serving system object */

  cm_iterator_type        call_itr;
    /* Call Object Iterator */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
  /*If the UE is in a voice or VT call, any incoming call must be provided the busy treatment, 
  ** if the CW is set to be UE based and it s disabled.
  */
  if(cmph_get_volte_sub_info_ptr(call_ptr->asubs_id)->ue_based_cw.use_ue_based_cw &&
     !cmph_get_volte_sub_info_ptr(call_ptr->asubs_id)->ue_based_cw.is_enabled &&
    call_ptr->call_type == CM_CALL_TYPE_VOICE &&
    ((cmcall_is_there_a_call_type(call_ptr->ss, CM_CALL_TYPE_VOICE,call_ptr->call_id) != CM_CALL_ID_INVALID)||
    (cmcall_is_there_a_call_type(call_ptr->ss, CM_CALL_TYPE_VT, CM_CALL_ID_INVALID) != CM_CALL_ID_INVALID))
   )
  {
    accept_call = FALSE;
    CM_MSG_HIGH_0("UE   based CW reject MT voice call");
  }
  #endif
  
  cmcall_obj_iterator_init(&call_itr);

  temp_call_ptr = cmcall_obj_get_next(&call_itr);

  while(temp_call_ptr != NULL)
  {

    {

      /*
      ** Reject incoming voice call if voice or emergency in origination.
      ** Reject incoming CS_DATA call if voice or emergency in origination.
      ** Reject incoming voice if there is already a CS_DATA call
      ** Reject incoming CS_DATA call if there is already a voice call.
      */
      #if !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
      if (cmcall_check_voice_emerg_in_orig (temp_call_ptr)
          #ifdef FEATURE_MMODE_DUAL_ACTIVE
          && (!cmph_is_dsda() || (temp_call_ptr->asubs_id == call_ptr->asubs_id))
          #endif
          )
      {
        CM_MSG_HIGH_0 ("Rejecting MT, Voice/Emergency in ORIG state");

        accept_call = FALSE;
        break;

      } else if (cmcall_check_if_csdata_and_voice (call_ptr->call_type,
                                                     temp_call_ptr->call_type)
          #ifdef FEATURE_MMODE_DUAL_ACTIVE
          && (!cmph_is_dsda() && (temp_call_ptr->asubs_id == call_ptr->asubs_id))
          #endif
                )
      {

          #if (!defined(FEATURE_DUAL_SERVICE_IN_DATA) && \
               !defined(FEATURE_DUAL_SERVICE_IN_VOICE))
          CM_MSG_HIGH_2 ("Rejecting MT %d, since %d call is active",
                        call_ptr->call_type, temp_call_ptr->call_type);

          accept_call = FALSE;
          #endif

          break;
      } /* check if there is any vowlan call active, reject the MT CS GW call */      
      else 
      if(cmcall_is_there_vowlan_call() != CM_CALL_ID_INVALID)
      {
        CM_MSG_HIGH_0 ("Rejecting MT, VOWLAN call currently exist");
        accept_call = FALSE;
        break;
      }
      #endif

    } /* else */

    temp_call_ptr = cmcall_obj_get_next(&call_itr);
  } /* while(temp_call_ptr != NULL) */

  return accept_call;
}
#endif
#if defined (FEATURE_MULTIMEDIA_FALLBACK)

/*===========================================================================

FUNCTION cmwcall_is_bearer_type_speech

DESCRIPTION
   Determine if given bearer capability is for circuit switched speech call.

DEPENDENCIES
  none

RETURN VALUE
  TRUE
  FALSE

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmwcall_is_bearer_type_speech
(
  const cm_bearer_capability_s_type  *bearer_cap_ptr
    /* pointer to a bearer capability */
)
{

  if ( (bearer_cap_ptr->present) &&
       ((bearer_cap_ptr->information_transfer_capability == ITC_SPEECH)
        #if (defined FEATURE_ALS) && (defined FEATURE_GSM)
        ||
        (bearer_cap_ptr->information_transfer_capability == cmph_ptr()->itc_aux_val_to_use)
        #endif
        )
     )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

}/* cmwcall_is_bearer_type_speech */


/*===========================================================================

FUNCTION cmwcall_is_bearer_type_csdata

DESCRIPTION
   Determine if given bearer capability is for circuit switched data call.

DEPENDENCIES
  none

RETURN VALUE
  TRUE
  FALSE

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmwcall_is_bearer_type_csdata
(
  const cm_bearer_capability_s_type  *bearer_cap_ptr
    /* pointer to a bearer capability */
)
{
  #if defined(FEATURE_MMODE_GW)
  if ( (bearer_cap_ptr->present) &&
       (bearer_cap_ptr->information_transfer_capability != ITC_SPEECH)
       #if (defined FEATURE_ALS) && (defined FEATURE_GSM)
       &&
       (bearer_cap_ptr->information_transfer_capability != cmph_ptr()->itc_aux_val_to_use)
       #endif
     )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
  #endif

}/* cmwcall_is_bearer_type_csdata */


/*===========================================================================

FUNCTION cmwcall_is_call_type_partof_bearer

DESCRIPTION
   Checks if given call type is accomodated by the given bearer.

DEPENDENCIES
  none

RETURN VALUE
  TRUE
  FALSE

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmwcall_is_call_type_partof_bearer
(

  cm_call_type_e_type           call_type,
    /* call type */

  const cm_bearer_capability_s_type  *bearer_cap_ptr
    /* pointer to a bearer capability */
)
{

  if ( cmwcall_is_bearer_type_speech (bearer_cap_ptr))
  {
    if (call_type == CM_CALL_TYPE_VOICE )      return TRUE;

    if (call_type == CM_CALL_TYPE_EMERGENCY )  return TRUE;

    return FALSE;
  }

  if ( cmwcall_is_bearer_type_csdata (bearer_cap_ptr))
  {
    if (call_type == CM_CALL_TYPE_CS_DATA )      return TRUE;

    if (call_type == CM_CALL_TYPE_VT )           return TRUE;

    if (call_type == CM_CALL_TYPE_VT_LOOPBACK )  return TRUE;

  }

  return FALSE;

}/* cmwcall_is_call_type_partof_bearer */


/*===========================================================================

FUNCTION cmwcall_determine_call_type

DESCRIPTION
   Determine call type associated with the Bearer capability. For Emergency
   calls call type is not changed.

DEPENDENCIES
  none

RETURN VALUE
  Call type associated with this Bearer Capability (BC)

SIDE EFFECTS
  none

===========================================================================*/
static cm_call_type_e_type cmwcall_determine_call_type
(
  const cmcall_s_type                *call_ptr,
     /* Call object */

  const cm_bearer_capability_s_type  *bearer_cap_ptr
    /* pointer to a bearer capability */
)
{

  /*---------------------------------------------------------------------*/

  CM_ASSERT (call_ptr != NULL);

  /*---------------------------------------------------------------------*/

  /* For emergency calls call type is returned as emergency */
  if (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
  {
    return CM_CALL_TYPE_EMERGENCY;
  }

  /*---------------------------------------------------------------------*/

  /* Return call type as CS_DATA for bearer containing CS_DATA information*/
  if (cmwcall_is_bearer_type_csdata (bearer_cap_ptr))
                                              return CM_CALL_TYPE_CS_DATA;

  /* Return call type as speech for bearer containing speech information*/
  if (cmwcall_is_bearer_type_speech (bearer_cap_ptr))
                                              return CM_CALL_TYPE_VOICE;

  /*---------------------------------------------------------------------*/

  /* By default return voice */
  return CM_CALL_TYPE_VOICE;

}/* cmwcall_determine_call_type */

/*===========================================================================

FUNCTION cmwcall_set_incall_modif_field

DESCRIPTION
  Boolean value that tells if can be modified between Voice and VT.
  TRUE   - Call can be modified. Voice to VT and VT to Voice.
  FALSE  - Call cannot be modified.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  Changes value of is_call_mod_possible in call object

===========================================================================*/
static void cmwcall_set_incall_modif_field
(
  cmcall_s_type    *call_ptr
    /* pointer to a call object */
)
{

  gsm_wcdma_cmcall_type    *wcall_info_ptr = NULL;

  /*---------------------------------------------------------------------*/


  /* Pointer to wcall info
  */
  wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

  if ( (!wcall_info_ptr->bearer_capability_1.present)  ||
       (!wcall_info_ptr->bearer_capability_2.present)  ||
       ((wcall_info_ptr->bc_repeat_indicator.present) &&
        (wcall_info_ptr->bc_repeat_indicator.repeat_indication == REPEAT_INDICATION_NO_MODIFY)) )
  {
    CM_MSG_HIGH_3 ("call id = %d is_call_mod_possible = %d (BC1/BC2 not pres), Repeat indication = %d"
                  , call_ptr->call_id
                  , wcall_info_ptr->is_call_mod_possible
                  , wcall_info_ptr->bc_repeat_indicator.repeat_indication );
    wcall_info_ptr->is_call_mod_possible = FALSE;
    return;
  }
  /*---------------------------------------------------------------------*/

  /* There should be one BC supporting Voice and another BC supporting
  ** CS_DATA
  */
  if ( ( (cmwcall_is_bearer_type_csdata (&wcall_info_ptr->bearer_capability_1))
         &&
         (cmwcall_is_bearer_type_speech(&wcall_info_ptr->bearer_capability_2))
       )

       ||

       ( (cmwcall_is_bearer_type_speech (&wcall_info_ptr->bearer_capability_1))
         &&
         (cmwcall_is_bearer_type_csdata (&wcall_info_ptr->bearer_capability_2))

       )

     )
  {
    wcall_info_ptr->is_call_mod_possible = TRUE;
  }
  else
  {
    wcall_info_ptr->is_call_mod_possible = FALSE;
  }


  /*---------------------------------------------------------------------*/

  CM_MSG_HIGH_2 ("call id = %d is_call_mod_possible = %d", call_ptr->call_id,
                wcall_info_ptr->is_call_mod_possible);

  return;

} /* cmwcall_set_incall_modif_field () */


/*===========================================================================

FUNCTION cmwcall_copy_bc_hlc_llc_from_cmd

DESCRIPTION
  Copy BC1, HLC1, LLC1 and BC2, LLC2 and HLC2 fields from given cmd ptr

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  Copies BC, HLC and LLC into call object

===========================================================================*/
static void cmwcall_copy_bc_hlc_llc_from_cmd
(
  cmcall_s_type                        *call_ptr,
    /* pointer to a call object */

  const cm_call_cmd_info_gw_cs_s_type  *cmd_info_ptr
    /* pointer to gw cs cmd */
)
{

  gsm_wcdma_cmcall_type    *wcall_info_ptr = NULL;

  /*---------------------------------------------------------------------*/

  /* Pointer to wcall info
  */
  wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;


  wcall_info_ptr->bc_repeat_indicator = cmd_info_ptr->bc_repeat_indicator;

  wcall_info_ptr->bearer_capability_1 = cmd_info_ptr->bearer_capability_1;

  wcall_info_ptr->bearer_capability_2 = cmd_info_ptr->bearer_capability_2;


  wcall_info_ptr->hlc_repeat_indicator = cmd_info_ptr->hlc_repeat_indicator;

  wcall_info_ptr->high_layer_compatibility_1 =
                                    cmd_info_ptr->high_layer_compatibility_1;

  wcall_info_ptr->high_layer_compatibility_2 =
                                    cmd_info_ptr->high_layer_compatibility_2;


  wcall_info_ptr->llc_repeat_indicator = cmd_info_ptr->llc_repeat_indicator;

  wcall_info_ptr->low_layer_compatibility_1 =
                                    cmd_info_ptr->low_layer_compatibility_1;

  wcall_info_ptr->low_layer_compatibility_2 =
                                    cmd_info_ptr->low_layer_compatibility_2;


  /*---------------------------------------------------------------------*/

  return;

} /* cmwcall_copy_bc_hlc_llc_from_cmd () */


/*===========================================================================

FUNCTION cmwcall_update_on_modify_complete

DESCRIPTION
   Updates call object when in call modification gets completed with success.
   BC1, HLC1 and LLC1 values are copied into BC2, HLC2 and LLC2. Values
   of BC, HLC and LLC returned with modification complete are copied into
   BC1, HLC1 and LLC1.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  Copies BC, LLC and HLC values into call object

===========================================================================*/
static void cmwcall_update_on_modify_complete
(
  gsm_wcdma_cmcall_type             *wcall_info_ptr,
    /* pointer to gw cs cmd */

  const cm_modify_complete_conf_T  *modify_comp_conf_ptr
    /* pointer to modify complete conf structure */

)
{

  /* Copy BC1, HLC1 and LLC1 values into BC2, HLC2 and LLC2
  */
  wcall_info_ptr->bearer_capability_2        =
                              wcall_info_ptr->bearer_capability_1;

  wcall_info_ptr->high_layer_compatibility_2 =
                              wcall_info_ptr->high_layer_compatibility_1;

  wcall_info_ptr->low_layer_compatibility_2  =
                              wcall_info_ptr->low_layer_compatibility_1;

  /*----------------------------------------------------------------------*/

  /* Copy modify complete values into BC1, HLC1 and LLC1
  */
  wcall_info_ptr->bearer_capability_1        =
                          modify_comp_conf_ptr->cm_bearer_capability;

  wcall_info_ptr->high_layer_compatibility_1 =
                          modify_comp_conf_ptr->cm_high_layer_compatibility;

  wcall_info_ptr->low_layer_compatibility_1  =
                          modify_comp_conf_ptr->cm_low_layer_compatibility;


} /* cmwcall_update_on_modify_complete */



/*===========================================================================

FUNCTION cmwcall_is_modify_ind_accep

DESCRIPTION
  Checks if modify indication is acceptable for current call in connected
  state.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  -  Modify indication is acceptable.
  FALSE -  Modify indication is not acceptable.

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmwcall_is_modify_ind_accep
(
  const cm_mt_modify_ind_T        *modify_ind_ptr,
    /* pointer to modify complete conf structure */

  cmcall_s_type                   *call_ptr
    /* Pointer to call object */
)
{
  gsm_wcdma_cmcall_type    *wcall_info_ptr = NULL;


  /*------------------------------------------------------------------*/

  wcall_info_ptr   = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

  /* Call has to be in conversation for modify indication to work
  */
  if (call_ptr->call_state != CM_CALL_STATE_CONV)
  {
    CM_ERR_1 ("Mod ind rej, call not in conv %d", call_ptr->call_id);
    return FALSE;
  }


  /* If modify indication has no Bearer capability or given call does
  ** not have BC2, reject the indication
  */
  if ( (!modify_ind_ptr->bearer_capability.present) ||
       (!wcall_info_ptr->bearer_capability_2.present)
     )
  {
    CM_ERR_0 ("Mod ind rej, BC not present");
    return FALSE;
  }


  /* BC of given indication should match as that of
  ** call object
  */
  if (  memcmp ( &modify_ind_ptr->bearer_capability,
                 &wcall_info_ptr->bearer_capability_2,
                 sizeof(cm_bearer_capability_s_type) )  != 0
     )
  {
    CM_ERR_0 ("Mod ind rej, BC's do not match");
    return FALSE;
  }

  /*------------------------------------------------------------------*/

  /* If modify indication carries lower layer data and call object
  ** does not or the other way round, reject the modification.
  */
  if ( modify_ind_ptr->low_layer_compatibility.present  !=
       wcall_info_ptr->low_layer_compatibility_2.present
     )
  {
    CM_ERR_0 ("Mod ind rej, llc presence mismatch");
    return FALSE;
  }


  /* If Lower layer info is present in modify ind, then it should
  ** match with call obj's llc2
  */
  if ( modify_ind_ptr->low_layer_compatibility.present &&

       ( memcmp ( &modify_ind_ptr->low_layer_compatibility,
                  &wcall_info_ptr->low_layer_compatibility_2,
                  sizeof (cm_low_layer_compatibility_s_type) ) != 0)
     )
  {
    CM_ERR_0 ("Mod ind rej, llc's do not match");
    return FALSE;
  }

  /*------------------------------------------------------------------*/

  /* If modify indication carries high layer data and call object
  ** does not or the other way round, reject the modification.
  */
  if ( modify_ind_ptr->high_layer_compatibility.present  !=
       wcall_info_ptr->high_layer_compatibility_2.present
     )
  {
    CM_ERR_0 ("Mod ind rej, hlc presence mismatch");
    return FALSE;
  }


  /* If High layer info is present in modify ind, then it should
  ** match with call obj's HLC
  */
  if ( modify_ind_ptr->high_layer_compatibility.present &&

       ( memcmp ( &modify_ind_ptr->high_layer_compatibility,
                &wcall_info_ptr->high_layer_compatibility_2,
                sizeof (cm_high_layer_compatibility_s_type) ) != 0)
     )
  {
    CM_ERR_0 ("Mod ind rej, hlc's do not match");
    return FALSE;
  }

  /*------------------------------------------------------------------*/

  return TRUE;

} /* cmwcall_is_modify_ind_accep () */



/*===========================================================================

FUNCTION cmwcall_swap_bc_llc_hlc

DESCRIPTION
  Swaps BC, LLC and HLC with one another in the call object.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmwcall_swap_bc_llc_hlc
(
  cmcall_s_type                   *call_ptr
    /* pointer to call object */
)
{

  cm_low_layer_compatibility_s_type  low_layer_comp_for_swap;
  cm_high_layer_compatibility_s_type high_layer_comp_for_swap;
  cm_bearer_capability_s_type        bearer_cap_for_swap;


  /*------------------------------------------------------------------*/

  gsm_wcdma_cmcall_type             *wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;


  /* Swap BC1 and BC2
  */
  bearer_cap_for_swap        = wcall_info_ptr->bearer_capability_1;
  wcall_info_ptr->bearer_capability_1 =
                            wcall_info_ptr->bearer_capability_2;
  wcall_info_ptr->bearer_capability_2 =
                            bearer_cap_for_swap;


  /*------------------------------------------------------------------*/

  /* Swap LLC1 and LLC2
  */
  low_layer_comp_for_swap    = wcall_info_ptr->low_layer_compatibility_1;
  wcall_info_ptr->low_layer_compatibility_1 =
                            wcall_info_ptr->low_layer_compatibility_2;
  wcall_info_ptr->low_layer_compatibility_2 = low_layer_comp_for_swap;

  /*------------------------------------------------------------------*/

  /* Swap HLC1 and HLC2
  */
  high_layer_comp_for_swap = wcall_info_ptr->high_layer_compatibility_1;
  wcall_info_ptr->high_layer_compatibility_1 =
                               wcall_info_ptr->high_layer_compatibility_2;
  wcall_info_ptr->high_layer_compatibility_2 = high_layer_comp_for_swap;


} /* cmwcall_swap_bc_llc_hlc */



/*===========================================================================

FUNCTION cmwcall_update_call_comp_cap

DESCRIPTION
  Checks if call confirmation has BCs flipped, In that case it flips LLC and
  HLC. Copies BC's as returned by the network.

   BCs returned by Call Confirmation is copied and call type is recalculated.

  1> If neither BC1 or BC2 is present in MO_CALL_CONF return.

  2> If MO_CALL_CONF has BC1 and BC2 that is flipped version of MO setup
     flip BC, LLC and HLC in call object.

  3> If MO_CALL_CONF has only one BC (BC1), invalidate BC2 in call object

  4> If MO_CALL_CONF has one only one BC (BC1) and this is BC2 of call object,
     Copy BC2 to BC1 in call object and invalidate BC2.

  5> Assumption:- MO_CALL_CONF cannot have BC1 as NONE and a valid BC2.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  Changes values in Call object

===========================================================================*/
static void cmwcall_update_call_comp_cap
(
  cmcall_s_type                   *call_ptr,
    /* pointer to call object */

  const cm_mo_call_conf_T         *call_conf_ptr
    /* pointer to modify complete conf structure */
)
{

  gsm_wcdma_cmcall_type             *wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

  /*------------------------------------------------------------------*/

  /* If neither BC1 or BC2 is present, do not update call object
  */
  if ( (!call_conf_ptr->cm_mo_call_details.present)  ||

       ((call_conf_ptr->cm_mo_call_details.present)
        &&
        (!call_conf_ptr->cm_mo_call_details.bearer_capability_1.present)
        &&
        (!call_conf_ptr->cm_mo_call_details.bearer_capability_2.present)
       )
     )
  {
    CM_MSG_HIGH_0 ("mo_call_details not present / BC1 and BC2 not present");

    return;
  }

  /*------------------------------------------------------------------*/

  /* Following conditions are handled
  **
  ** 1> and 2> If BC1 and BC2 from CALL_CONF are interchanged as compared
  **    to call_ptr then swap bc, llc and hlc.
  **
  ** 3> Invalidate LLC2, HLC2 if MC from CALL_CONF is NULL
  **
  ** 4> CALL_CONF carries only BC2 of the call setup. BC1 and BC2 are swapped
  **    in call_ptr and BC2 is invalidated.
  **
  */
  if ( (cmwcall_is_bearer_type_speech (&wcall_info_ptr->bearer_capability_1) &&
        cmwcall_is_bearer_type_speech ( (cm_bearer_capability_s_type *)
          &call_conf_ptr->cm_mo_call_details.bearer_capability_2)
       )
       &&
       (cmwcall_is_bearer_type_csdata (&wcall_info_ptr->bearer_capability_2) &&
        cmwcall_is_bearer_type_csdata ( (cm_bearer_capability_s_type*)
          &call_conf_ptr->cm_mo_call_details.bearer_capability_1)
       )
     )
  {
    /* Swapping LLC and HLC. Function also swaps BC but we overwrite it
    ** anyway
    */
    cmwcall_swap_bc_llc_hlc (call_ptr);

  }
  else if
     ( (cmwcall_is_bearer_type_csdata (&wcall_info_ptr->bearer_capability_1) &&
        cmwcall_is_bearer_type_csdata ((cm_bearer_capability_s_type *)
          &call_conf_ptr->cm_mo_call_details.bearer_capability_2)
       )
       &&
       (cmwcall_is_bearer_type_speech (&wcall_info_ptr->bearer_capability_2) &&
        cmwcall_is_bearer_type_speech ( (cm_bearer_capability_s_type *)
          &call_conf_ptr->cm_mo_call_details.bearer_capability_1)
       )
     )

  {

    /* Swapping LLC and HLC. Function also swaps BC but we overwrite it
    ** anyway
    */
    cmwcall_swap_bc_llc_hlc (call_ptr);

  }
  /* Network accepts only the first BCIE from setup request
  ** LLC2 and HLC2 being cleaned because call proceeding  does not return
  ** cleaned up LLC and HLC values to reflect that only one BCIE is possible.
  */
  else if ( ( (cmwcall_is_bearer_type_speech (&wcall_info_ptr->bearer_capability_1) &&
                 cmwcall_is_bearer_type_speech ((cm_bearer_capability_s_type *)
                 &call_conf_ptr->cm_mo_call_details.bearer_capability_1)
                )
                ||
                (cmwcall_is_bearer_type_csdata (&wcall_info_ptr->bearer_capability_1) &&
                 cmwcall_is_bearer_type_csdata ((cm_bearer_capability_s_type *)
                 &call_conf_ptr->cm_mo_call_details.bearer_capability_1)
               )
            )
            &&
            (!call_conf_ptr->cm_mo_call_details.bearer_capability_2.present)
          )
  {

    /* Repeat indicator for BC, LLC and HLC is turned to FALSE
    */
    wcall_info_ptr->bc_repeat_indicator.present        = FALSE;

    wcall_info_ptr->llc_repeat_indicator.present       = FALSE;
    wcall_info_ptr->low_layer_compatibility_2.present  = FALSE;

    wcall_info_ptr->hlc_repeat_indicator.present       = FALSE;
    wcall_info_ptr->high_layer_compatibility_2.present = FALSE;
  }
  /* Network accepts Second BCIE from setup request.
  ** LLC1 and HLC1 being cleaned up because call proceeding does not return
  ** cleaned up LLC and HLC values to reflect that only one BCIE is possible.
  ** NAS it is a limitation.
  */
  else if ( ( (cmwcall_is_bearer_type_speech (&wcall_info_ptr->bearer_capability_2) &&
               cmwcall_is_bearer_type_speech ((cm_bearer_capability_s_type *)
                 &call_conf_ptr->cm_mo_call_details.bearer_capability_1)
              )
              ||
              (cmwcall_is_bearer_type_csdata (&wcall_info_ptr->bearer_capability_2) &&
               cmwcall_is_bearer_type_csdata ((cm_bearer_capability_s_type *)
                 &call_conf_ptr->cm_mo_call_details.bearer_capability_1)
              )
            )

            &&
            (!call_conf_ptr->cm_mo_call_details.bearer_capability_2.present)
          )
  {

    /* Swapping LLC and HLC. Function also swaps BC but we overwrite it
    ** anyway. This copies BC2, LLC2, HLC2 to BC1, LLC1 and HLC1.
    */
    cmwcall_swap_bc_llc_hlc (call_ptr);

    /* Repeat indicator for BC, LLC and HLC is turned to FALSE
    */
    wcall_info_ptr->bc_repeat_indicator.present        = FALSE;

    wcall_info_ptr->llc_repeat_indicator.present       = FALSE;
    wcall_info_ptr->low_layer_compatibility_2.present  = FALSE;

    wcall_info_ptr->hlc_repeat_indicator.present       = FALSE;
    wcall_info_ptr->high_layer_compatibility_2.present = FALSE;
  }

  /*------------------------------------------------------------------*/

  /* Copy bearer capability information from call conf message
  */
  wcall_info_ptr->bearer_capability_1 =
          call_conf_ptr->cm_mo_call_details.bearer_capability_1;

  wcall_info_ptr->bearer_capability_2 =
          call_conf_ptr->cm_mo_call_details.bearer_capability_2;

  /*------------------------------------------------------------------*/

  /* Call type is always calculated from BC1
  */
  call_ptr->call_type = cmwcall_determine_call_type
                                     (call_ptr,&wcall_info_ptr->bearer_capability_1);


  return;


} /* cmwcall_update_call_comp_cap () */



/*===========================================================================

FUNCTION cmwcall_check_for_BC1_if_BC2_present

DESCRIPTION
  BC1 should be present in call orig structure if BC2 is present.

DEPENDENCIES
  None

RETURN VALUE
  TRUE   - IF both BC's are present/ BC2 is not present
  FALSE  - If BC2 is present and BC1 is not present.

SIDE EFFECTS
  Changes values in Call object

===========================================================================*/
static boolean cmwcall_check_for_BC1_if_BC2_present
(
    const cm_bearer_capability_s_type     *bearer_capability_1_ptr,
      /* Pointer to bearer capability */

    const cm_bearer_capability_s_type     *bearer_capability_2_ptr
      /* Pointer to bearer capability */
)
{

  if ( (  bearer_capability_2_ptr->present) &&
       ( !bearer_capability_1_ptr->present)
     )
  {
    return FALSE;
  }

  return TRUE;

} /* cmwcall_check_for_BC1_if_BC2_present () */

/*===========================================================================

FUNCTION cmwcall_set_codec_list

DESCRIPTION
  IF NV is enabled to add codec list, umts and gsm codecs are added.

DEPENDENCIES
  NV_UMTS_CALL_VT_CODEC_LIST_I should be available.

RETURN VALUE
  None

SIDE EFFECTS
  Populates codec list to given structure.

===========================================================================*/
static void cmwcall_set_codec_list
(
    cm_supported_codecs_list_s_type  *codecs_list,
      /* Pointer to codec list struct */

    sys_modem_as_id_e_type            asubs_id
)
{
  #if defined(FEATURE_MMODE_GW) && defined(FEATURE_MULTIMEDIA_FALLBACK)
  /* Spec 26.103 (v6.1.0) sec 6.1 and 6.2
  */
  cmph_sub_config_s_type *ph_sub_config_ptr = cmph_get_sub_config_ptr(asubs_id);

  if(ph_sub_config_ptr == NULL)
  {
    CM_MSG_HIGH_1("cmph sub config pointer[%d] is NULL",asubs_id);
    return;
  }

  #if defined(FEATURE_MMODE_GW) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
  if (ph_sub_config_ptr->is_umts_call_vt_codec_list_on)
  {

    codecs_list->present          = TRUE;

    /* Two codecs being added one for UMTS, another for GSM
    */
    codecs_list->length           = 2;

    /*------------------------------------------------------------------*/

    /* UMTS is listed first since it is the preferred codec
    */
    codecs_list->codec_list[0].sysID = SYSID_UMTS;

    /* length 2 indicates 2 bitmaps being added bitmap_1_8 and bitmap_9_16
    */
    codecs_list->codec_list[0].bitmap_length = 2;
    codecs_list->codec_list[0].bitmap_1_8    =  0;
    codecs_list->codec_list[0].bitmap_9_16   =  0;

    /*------------------------------------------------------------------*/

    /* GSM is the next codec
    */
    codecs_list->codec_list[1].sysID = SYSID_GSM;

    /* length 2 indicates 2 bitmaps being added bitmap_1_8 and bitmap_9_16
    */
    codecs_list->codec_list[1].bitmap_length = 2;
#ifdef FEATURE_ENABLE_GSM_VOCODER
    codecs_list->codec_list[1].bitmap_1_8    =  CODEC_BITMAP_GSM_FULL_RATE
      | CODEC_BITMAP_GSM_HALF_RATE |  CODEC_BITMAP_GSM_ENHANCED_FULL_RATE
      | CODEC_BITMAP_GSM_FULL_RATE_AMR | CODEC_BITMAP_GSM_HALF_RATE_AMR;

    #ifdef FEATURE_GSM_AMR_WB
    codecs_list->codec_list[1].bitmap_9_16   =  CODEC_BITMAP_GSM_AMR_WB_FULL_RATE;
    #else
    codecs_list->codec_list[1].bitmap_9_16   =  0;
    #endif
#else 
	codecs_list->codec_list[1].bitmap_1_8    =   CODEC_BITMAP_GSM_FULL_RATE_AMR | CODEC_BITMAP_GSM_HALF_RATE_AMR;
    codecs_list->codec_list[1].bitmap_9_16   =  0;
#endif
    /*------------------------------------------------------------------*/

  }
  else
  #endif
  {
    codecs_list->present = FALSE;
  }
  #endif
} /* cmwcall_set_codec_list () */

#endif /* FEATURE_MULTIMEDIA_FALLBACK */

#if defined(FEATURE_MMODE_GW)
/*===========================================================================

FUNCTION cmwcall_is_cond_for_ps_rej_rsp

DESCRIPTION
  Checks if PDP call is in a state where reject response has to be sent to SM

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmwcall_is_cond_for_ps_rej_rsp(

    const cmcall_s_type    *call_ptr
        /* pointer to a call object */
)
/*lint -esym(715,call_ptr) */
{

  CM_ASSERT( call_ptr != NULL );


  if ( (call_ptr->call_state       == CM_CALL_STATE_INCOM) &&
       (call_ptr->call_subst.incom == CM_CALL_INCOM_SUBST_ALERTING)
     )
  {
    return TRUE;
  }

  return FALSE;

} /* cmwcall_is_cond_for_ps_rej_rsp () */
/*lint +esym(715,call_ptr) */



/*===========================================================================

FUNCTION cmwcall_send_ps_rej_rsp

DESCRIPTION
  Sends PDP reject response to SM.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmwcall_send_ps_rej_rsp(

    cm_call_id_type      call_id,
      /* call id of the call to be cleared */

    sm_status_T          cause
      /* Reject cause to be sent with PS reject resp */
)
/*lint -esym(715,call_ptr) */
{

  sm_cmd_type                          *sm_ptr = NULL;
    /* SM command pointer */

  byte                                  connection_id = 0;
    /* Connection id to talk with SM */

  #if defined(FEATURE_REL5)
  cmcall_s_type                        *call_ptr = cmcall_ptr( call_id );
    /* Call object */
  #else
  SYS_ARG_NOT_USED (call_id);
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!cm_find_connection_id( call_id, &connection_id ))
  {
    CM_ERR_0( "Could not find connection id.");
    return;
  }

  sm_ptr = cm_sm_get_buf_else_err_fatal(SMREG_PDP_ACTIVATE_REJ_RSP);

  if (!sm_ptr)
  {
    CM_ERR_0 ("No SM buff for SMREG_PDP_ACTIVATE_REJ_RSP");
    return;
  }

  sm_ptr->header.connection_id            = connection_id;
  sm_ptr->cmd.pdp_activate_rej_resp.cause = cause;

  #if defined(FEATURE_REL5)

  if( call_ptr != NULL )
  {
    sm_ptr->cmd.pdp_activate_rej_resp.as_id = call_ptr->asubs_id;
    sm_ptr->cmd.pdp_activate_rej_resp.config_options =
      call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.config_options;
  }
  #endif

  cm_sm_send_cmd( sm_ptr );

} /* cmwcall_send_ps_rej_rsp() */
/*lint +esym(715,call_ptr) */

/*===========================================================================

FUNCTION cmwcall_is_ps_data_allowed

DESCRIPTION
  checks if ps data call can be allowed.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmwcall_is_ps_data_allowed ( void )
{

  cmss_s_type                     *ss_ptr          = cmss_ptr();
    /* Pointer to the serving system object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Simultaneous voice and ps calls are possible in GSM only if
  ** DTM (Dual Transfer Mode) support is available.
  */
  #if !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
  if ( ((ss_ptr->info.sys_mode == SYS_SYS_MODE_GSM)
       #if defined(FEATURE_GSM_DTM)
       && (ss_ptr->info.cell_srv_ind.dtm_supp != SYS_DTM_SUPPORT_AVAIL)
       #endif
       && cmcall_check_if_gw_cs_call_active (CM_CALL_STATE_NONE, CM_SS_MAIN)) 
       #if !defined(FEATURE_CM_DISABLE_IMS_EXT)
       ||
       (cmemg_get_reason_per_sub(SYS_MODEM_AS_ID_1) == CM_AC_REASON_THIRD_PARTY_IMS_E911)
       #endif
  )
  {
    return FALSE;
  }
  #endif
  
  return TRUE;

} /* cmwcall_is_ps_data_allowed */

/*===========================================================================

FUNCTION cmwcall_is_ps_data_allowed_hybr_gw

DESCRIPTION
  checks if ps data call can be allowed.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmwcall_is_ps_data_allowed_hybr_gw (void)
{

  cmss_s_type                     *ss_ptr          = cmss_ptr();
    /* Pointer to the serving system object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Simultaneous voice and ps calls are possible in GSM only if
  ** DTM (Dual Transfer Mode) support is available.
  */
  #if defined(FEATURE_MMODE_HYBR2_ENABLED)
  if ( (ss_ptr->info.gw_sys_mode == SYS_SYS_MODE_GSM) &&
       #if defined(FEATURE_GSM_DTM)
       (ss_ptr->info.gw_cell_srv_ind.dtm_supp != SYS_DTM_SUPPORT_AVAIL) &&
       #endif
       cmcall_check_if_gw_cs_call_active (CM_CALL_STATE_NONE, CM_SS_HYBR_2 )
     )
  {
    return FALSE;
  }
  #endif
  return TRUE;

} /* cmwcall_is_ps_data_allowed_on_hybr_gw */

#ifdef FEATURE_MMODE_HYBR3_ENABLED
/*===========================================================================

FUNCTION cmwcall_is_ps_data_allowed_hybr_gw3

DESCRIPTION
  checks if ps data call can be allowed.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmwcall_is_ps_data_allowed_hybr_gw3 (void)
{

  cmss_s_type                     *ss_ptr          = cmss_ptr();
    /* Pointer to the serving system object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Simultaneous voice and ps calls are possible in GSM only if
  ** DTM (Dual Transfer Mode) support is available.
  */
  if ( (ss_ptr->info.gw3_sys_mode == SYS_SYS_MODE_GSM) &&
       #if defined(FEATURE_GSM_DTM)
       (ss_ptr->info.gw3_cell_srv_ind.dtm_supp != SYS_DTM_SUPPORT_AVAIL) &&
       #endif
       cmcall_check_if_gw_cs_call_active (CM_CALL_STATE_NONE, CM_SS_HYBR_3 )
     )
  {
    return FALSE;
  }

  return TRUE;

} /* cmwcall_is_ps_data_allowed_on_hybr_gw */
#endif /* FEATURE_MMODE_HYBR3_ENABLED */
  /*lint +esym(715,call_ptr) */
  
  
  /*===========================================================================
  
  FUNCTION  cmwcall_call_cmd_orig_para_check
  
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
  cm_call_cmd_err_e_type  cmwcall_call_cmd_orig_para_check
  (
    cm_call_cmd_s_type       *call_cmd_ptr
  )
  {
    cm_call_cmd_err_e_type    cmd_err        = CM_CALL_CMD_ERR_NOERR;
  
    cm_call_cmd_info_s_type  *cmd_info_ptr   = CMD_INFO_PTR( call_cmd_ptr );
        /* Pointer to call cmd info */
  
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
    CM_ASSERT(call_cmd_ptr           != NULL);
  
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
    if( (call_cmd_ptr->info.info_type == CM_CALL_MODE_INFO_GW_PS) &&
        (call_cmd_ptr->info.call_type != CM_CALL_TYPE_PS_DATA) )
    {
      cmd_err = CM_CALL_CMD_ERR_MISMATCH_P;
      CM_ERR_0( "CM_CALL_CMD_ERR_MISMATCH_P");
    }
  
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
    /* Checks if user to user data length is within range only if the
    ** origination cmd has cs parameters. Assuming cs and ps cmd orig
    ** parameters will not be present at the same time
    */
  #ifdef FEATURE_UUS
    if ( cmd_info_ptr->is_gsm_wcdma_cs_info_present &&
         !cmwcall_is_user_data_len_within_limit (call_cmd_ptr)
       )
    {
      cmd_err = CM_CALL_CMD_ERR_USER_DATA_LENGTH_P;
  
      CM_ERR_0 (" user_user_data_length exceeds MAX_USER_USER_DATA_LENGTH");
    }
  #endif
  
    return cmd_err;
  
  } /* cmwcall_call_cmd_orig_para_check() */

  /*===========================================================================
  
  FUNCTION cmcall_map_recall_rsp_cm_to_mn
  
  DESCRIPTION
    Map CM recall response to MN recall response.
  
  DEPENDENCIES
    none
  
  RETURN VALUE
    MN recall response value .
  
  SIDE EFFECTS
    none
  
  ===========================================================================*/
static cm_recall_rsp_e_type        cmcall_map_recall_rsp_cm_to_mn (
  
      cm_ccbs_recall_rsp_e_type      recall_rsp
          /* Recall response type to be mapped  */
  )
  {
  
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
    CM_ASSERT (BETWEEN(recall_rsp, CM_RECALL_RSP_CCBS_NONE, CM_RECALL_RSP_CCBS_MAX));
  
  
    switch( recall_rsp )
    {
      case CM_RECALL_RSP_CCBS_ACCEPT:               return CCBS_ACCEPT;
  
      case CM_RECALL_RSP_CCBS_REJECT:               return CCBS_REJECT;
  
      case CM_RECALL_RSP_CCBS_HOLD_ACCEPT:          return CCBS_HOLD_ACCEPT;
  
      case CM_RECALL_RSP_CCBS_REL_ACCEPT:           return CCBS_REL_ACCEPT;
  
      default:
        /*lint -save -e571 Suspicious cast */
        CM_ERR_1( "unsupported recall_rsp = %d", (uint8)recall_rsp);
        /*lint -restore */
  
        /* Assuming default should be ccbs reject
        */
        /*lint -save -e527 Unreachable */
        return CCBS_REJECT;
        /*lint -restore */
    } /* switch */
  
  } /* cmcall_map_recall_rsp_cm_to_mn() */

/**--------------------------------------------------------------------------
** Functions - external
** --------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION cmwcall_send_orig

DESCRIPTION
  Send an origination command to MN.
  For CM-MN interface, always send as_id with subscription id information.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_send_orig(

    cmcall_s_type    *call_ptr
        /* pointer to a call object */
)
{
      /* allocate a MN command buffer */
  #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE    
   mn_cnm_cmd_type           *mn_ptr;
  #ifdef FEATURE_MMODE_EMC_SUPPORT
   cm_mo_emergency_call_req_T *mn_e911_orig_p;
  #endif
   cm_mo_normal_call_req_T  *mn_orig_p;
   #ifdef FEATURE_UUS
   cm_mo_user_data_req_T     *mn_user_data_p;
   #endif
   gsm_wcdma_cmcall_type     *wcall_info_p;
   byte                       bcdbuf[CM_CALLED_PARTY_BCD_WITH_BCD_LEN];
  #endif
  #ifdef FEATURE_CCBS
  cm_recall_rsp_T           *mn_recall_p = NULL; /* CCBS recall response */
  #endif

  cmss_s_type   *ss_ptr  =  cmss_ptr();
  byte                      connection_id = 0;
  int length_unpack = 0;
  #if defined(FEATURE_GPRS_CALLCONTROL)
  pdp_context_info          *pdp_info_ptr = NULL;
  #endif

  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL );
  CM_ASSERT( call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( call_ptr->call_type )
  {
    /* The following case is used when the call type matched an emergency
    ** number.  This builds the Emergency call request and issues that
    ** request.
    */
    #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE    
    #ifdef FEATURE_MMODE_EMC_SUPPORT
    case CM_CALL_TYPE_EMERGENCY:

      mn_ptr = cm_mn_get_buf_else_err_fatal(CM_MO_EMERGENCY_CALL_REQ);

      mn_e911_orig_p = &(mn_ptr->cmd.cm_mo_emergency_call_req);

      /* assign the active subscription-id  belonging to the call */
      mn_e911_orig_p->as_id = call_ptr->asubs_id;
      wcall_info_p  = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

      if (!cm_find_connection_id(cmcall_call_id_get( call_ptr ),
                                 &mn_e911_orig_p->connection_id ))
      {
        CM_ERR_0( "Could not find connection id.");
      }
      mn_e911_orig_p->bearer_capability = wcall_info_p->bearer_capability_1;

      #if defined(FEATURE_MULTIMEDIA_FALLBACK)
      mn_e911_orig_p->stream_id.present =FALSE;
      cmwcall_set_codec_list (&mn_e911_orig_p->codecs_list, call_ptr->asubs_id);
      #endif

      /* Sets present to TRUE if srv categ is not CM_EMERG_NUM_TYPE_NONE */
      mn_e911_orig_p->service_category.present =
                                     CM_BOOLEAN(call_ptr->num.emerg_srv_categ);

      /* Copy emerg srv categ value */
      mn_e911_orig_p->service_category.emergency_category =
                                       (byte) call_ptr->num.emerg_srv_categ;

      CM_MSG_HIGH_1 ("Emerg srv category being sent = %d",
                                       call_ptr->num.emerg_srv_categ);



      cm_mn_send_cmd( mn_ptr );

      break;
  #endif
  
    case CM_CALL_TYPE_VS:

      CM_ERR_0 ("Videoshare not supported through CM-NAS interfaces");

      break;

    case CM_CALL_TYPE_VOICE:
    case CM_CALL_TYPE_CS_DATA:

      if (!cm_find_connection_id( cmcall_call_id_get( call_ptr ),
                                &connection_id ))
      {
       CM_ERR_0( "Could not find connection id.");
      }

      wcall_info_p  = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

      #ifdef FEATURE_CCBS

      /* If this origination is for CCBS recall response, send specific
      ** primitive to MN.
      */
      if (wcall_info_p->is_ccbs_recall_resp)
      {
        /* Clear the CCBS response pending flag
        */
        wcall_info_p->is_ccbs_recall_resp = FALSE;

        mn_ptr = cm_mn_get_buf_else_err_fatal(CM_RECALL_RSP);
        mn_recall_p     = &(mn_ptr->cmd.cm_recall_rsp);

        /* Set the subscription-id of the call */
        mn_recall_p->as_id  = call_ptr->asubs_id;
        mn_recall_p->connection_id = connection_id;

        /* Copy CCBS response to mn cmd structure
        */
        mn_recall_p->response =
              cmcall_map_recall_rsp_cm_to_mn (wcall_info_p->ccbs_recall_rsp);

        CM_MSG_HIGH_1 ("CM -> MN Recall rsp = %d",
                                    mn_recall_p->response);

        cm_mn_send_cmd( mn_ptr );

        break;
      }

      #endif /* FEATURE_CCBS */

      #ifdef FEATURE_UUS
      /* Check made for mo user user data, if present needs to be sent before
      ** CM_MO_NORMAL_CALL_REQ.
      */

      if (wcall_info_p->mo_mt_user_data.present)
      {
        mn_ptr        = cm_mn_get_buf_else_err_fatal(CM_MO_USER_DATA_REQ);

        CM_MSG_HIGH_0 ("MO user data present");

        if (mn_ptr == NULL)
        {
          CM_ERR_0 ("Error getting MN buffer CM_MO_USER_DATA_REQ");
          break;
        }

        mn_user_data_p = &(mn_ptr->cmd.cm_mo_user_data_req);

        /* Set the subscription-id of the call */
        mn_user_data_p->as_id  = call_ptr->asubs_id;
        mn_user_data_p->connection_id = connection_id;

        /* indicates if more data will follow */
        mn_user_data_p->more_data_indicator   =
                 wcall_info_p->mo_mt_user_data.more_data_indicator;

        /* Copies actual user user data and returns the data length */
        mn_user_data_p->data_length = memscpy ( mn_user_data_p->data,
                                                sizeof(mn_user_data_p->data),
                                                wcall_info_p->mo_mt_user_data.user_user_data,
                                                wcall_info_p->mo_mt_user_data.user_user_data_length);

        /* Copies protocol discriminator */
        mn_user_data_p->protocol_discriminator =
                 wcall_info_p->mo_mt_user_data.user_user_protocol_disc;

        /* Copies user data type */
        mn_user_data_p->type        =
                 wcall_info_p->mo_mt_user_data.user_user_type;

        cm_mn_send_cmd (mn_ptr);

      }
      #endif /* FEATURE_UUS */


      if (wcall_info_p->bearer_capability_2.present && !wcall_info_p->bc_repeat_indicator.present)
      {
        CM_MSG_HIGH_1("bearer_capability_2 present but repeat indicator is false, setting it as TRUE, repeat_indication %d",\
          wcall_info_p->bc_repeat_indicator.repeat_indication );
        wcall_info_p->bc_repeat_indicator.present           = TRUE;
        wcall_info_p->bc_repeat_indicator.repeat_indication = REPEAT_SERVICE_CHANGE_FALLBACK;
      }

      mn_ptr        = cm_mn_get_buf_else_err_fatal(CM_MO_NORMAL_CALL_REQ);
      mn_orig_p     = &(mn_ptr->cmd.cm_mo_normal_call_req);

      /* Set the subscription-id of the call */
      mn_orig_p->as_id  = call_ptr->asubs_id;
      mn_orig_p->connection_id = connection_id;

      /* Copy data fields */
      mn_orig_p->bc_repeat_indicator          = wcall_info_p->bc_repeat_indicator;
      mn_orig_p->bearer_capability_1          = wcall_info_p->bearer_capability_1;
      mn_orig_p->bearer_capability_2          = wcall_info_p->bearer_capability_2;
      mn_orig_p->llc_repeat_indicator         = wcall_info_p->llc_repeat_indicator;
      mn_orig_p->low_layer_compatibility_1    = wcall_info_p->low_layer_compatibility_1;
      mn_orig_p->low_layer_compatibility_2    = wcall_info_p->low_layer_compatibility_2;
      mn_orig_p->hlc_repeat_indicator         = wcall_info_p->hlc_repeat_indicator;
      mn_orig_p->high_layer_compatibility_1   = wcall_info_p->high_layer_compatibility_1;
      mn_orig_p->high_layer_compatibility_2   = wcall_info_p->high_layer_compatibility_2;
      mn_orig_p->cm_calling_party_subaddress  = wcall_info_p->calling_party_subaddress;
      mn_orig_p->forward_cug_info             = wcall_info_p->forward_cug_info;
      mn_orig_p->cm_clir                      = wcall_info_p->cm_clir;
      mn_orig_p->cm_called_party_bcd_number   = wcall_info_p->called_party_bcd_number;
      mn_orig_p->cm_called_party_subaddress   = wcall_info_p->called_party_subaddress;

      #if defined(FEATURE_MULTIMEDIA_FALLBACK)
    mn_orig_p->stream_id.present =FALSE;
      cmwcall_set_codec_list (&mn_orig_p->codecs_list, call_ptr->asubs_id);
      #endif

      if (call_ptr->gw_cc_modified_info.call_control_result !=
                                       CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS)
      {
        if( call_ptr->gw_cc_modified_info.ccp1.length > 0 )
        {
          /*
          ** Original CCP parameter were copied before considering
          ** the changes since if there is invalid values in the sent CCP,
          ** those should be retained
          */
          init_bearear_capability(&(mn_orig_p->bearer_capability_1));
          if(!(unpack_bearer_capability(
                 (byte*)call_ptr->gw_cc_modified_info.ccp1.ccp,
                 &mn_orig_p->bearer_capability_1,
                 &length_unpack,
                 (byte*)&(call_ptr->gw_cc_modified_info.ccp1.ccp[CM_MAX_CCP_LEN-1]))))
          {
            CM_MSG_HIGH_0("CCP unpacked failed");
          }
        }
        if( call_ptr->gw_cc_modified_info.ccp2.length > 0 )
        {
          /*
          ** Original CCP parameter were copied before considering
          ** the changes since if there is invalid values in the sent CCP,
          ** those should be retained
          */
          init_bearear_capability(&(mn_orig_p->bearer_capability_2));
          if(!(unpack_bearer_capability(
                 (byte*)call_ptr->gw_cc_modified_info.ccp2.ccp,
                 &mn_orig_p->bearer_capability_2,
                 &length_unpack,
                 (byte*)&(call_ptr->gw_cc_modified_info.ccp2.ccp[CM_MAX_CCP_LEN-1]))))
          {
            CM_MSG_HIGH_0("CCP unpacked failed");
          }

        }

        if( call_ptr->gw_cc_modified_info.num.len > 0)
        {
          memset (mn_orig_p->cm_called_party_bcd_number.data,
                0xff,
                sizeof(mn_orig_p->cm_called_party_bcd_number.data));

          cm_util_ascii_num_to_bcd_num( &call_ptr->gw_cc_modified_info.num, bcdbuf, sizeof( bcdbuf ) );

          if(call_ptr->is_modem_cc_api == FALSE)
          {
            /* This needs further investigation. UI is sending this in wrong
            ** format in callcontrol. So this wrokaround seems to be there
            */
            /*
            ** Re-encode call control's settings for ton/npi.
            */
            bcdbuf[BCD_NT_NPI] = (byte) 0x80 | (call_ptr->gw_cc_modified_info.num.number_type) |
                                             (call_ptr->gw_cc_modified_info.num.number_plan);
          }

          if (bcdbuf[0] != 0)
          {
            mn_orig_p->cm_called_party_bcd_number.length = memscpy( mn_orig_p->cm_called_party_bcd_number.data,
                                                                    sizeof(mn_orig_p->cm_called_party_bcd_number.data),
                                                                    &bcdbuf[1], bcdbuf[0] ); /*lint !e670 */
            mn_orig_p->cm_called_party_bcd_number.present = TRUE;
          }
          else
          {
            mn_orig_p->cm_called_party_bcd_number.present = FALSE;
          }

        }
        if( call_ptr->gw_cc_modified_info.num.subaddr.num_char > 0 )
        {
          mn_orig_p->cm_called_party_subaddress.present = TRUE;
          cmwcall_pack_subaddress(
           /*lint -e{826} */
            &(mn_orig_p->cm_called_party_subaddress),
           &(call_ptr->gw_cc_modified_info.num.subaddr));
        }
        else
        {
          mn_orig_p->cm_called_party_subaddress.present = FALSE;
        }
      }

      #if (defined FEATURE_ALS) && (defined FEATURE_GSM)
      /*
      ** All MO voice calls go on currently selected line.
      ** Map CM's currently selected line to GSM LL fields in MN command
      ** and set those fields accordingly in the command before you send it
      */
      if(cmph_als_supported() && call_ptr->call_type == CM_CALL_TYPE_VOICE)
      {
        (void)cmals_ptr_mmgsdi()->resolve_mo_call_req_func( cmph_ptr()->line,
                                               mn_ptr );


      }
      #endif
      CM_MSG_HIGH_1("Send MN Orig CTM : %d", mn_ptr->cmd.cm_mo_normal_call_req.bearer_capability_1.ctm);

      cm_mn_send_cmd( mn_ptr );


      break;
	#endif

	default:
	  CM_MSG_HIGH_1("Call type %d not valid for CS domain", call_ptr->call_type);
	  break;
	  

  } // switch

  /* Set the orig_sent_on_mode for the call based on the asubs_id */
  if (cmph_is_msim())
  {
    #ifdef FEATURE_MMODE_HYBR3_ENABLED
      if(call_ptr->ss == CM_SS_HYBR_3)
      {
        call_ptr->sr.orig_sent_on_mode = ss_ptr->info.gw3_sys_mode;
      }
      else
      #endif
      #ifdef FEATURE_MMODE_DUAL_SIM
        if(call_ptr->ss == CM_SS_HYBR_2)
        {
          call_ptr->sr.orig_sent_on_mode = ss_ptr->info.gw_sys_mode;
        }
      else
      #endif

      {
        call_ptr->sr.orig_sent_on_mode = ss_ptr->info.sys_mode;
      }

  }
  else
  {
    call_ptr->sr.orig_sent_on_mode = ss_ptr->info.sys_mode;
  }

}

#ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
/*===========================================================================

FUNCTION cmwcall_send_cs_end_req

DESCRIPTION
  Send call end request to MN.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_send_cs_end_req (

  byte              connection_id,
    /* Connection id of the call to be cleared
    */

  cm_ss_e_type      ss
    /* Stack on which the call is present */
)
{

  mn_cnm_cmd_type                      *mn_ptr = NULL;
      /* MN command pointer */

  /*-----------------------------------------------------------------------*/

  mn_ptr = cm_mn_get_buf_else_err_fatal(CM_MO_END_X_CALL_REQ);

  /* Set the subscription id on which request is being sent */
  mn_ptr->cmd.cm_n_x_send_req.as_id = cmph_determine_as_id_to_ll_per_ss(ss);

  mn_ptr->cmd.cm_n_x_send_req.connection_id   = connection_id;
  mn_ptr->cmd.cm_n_x_send_req.cause_value     = NORMAL_CALL_CLEARING;
  cm_mn_send_cmd( mn_ptr );

  /*-----------------------------------------------------------------------*/

} /* cmwcall_send_cs_end_req () */

/*===========================================================================

FUNCTION cmwcall_process_mt_setup_response

DESCRIPTION
  This function processes the client's CM_CALL_CMD_SETUP_RES command.
  This function constructs a CM_MT_SETUP_RES command and sends it to
  MN.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  Call_ptr is changed

===========================================================================*/
void cmwcall_process_mt_setup_response
(
  cmcall_s_type                        *call_ptr,
  const boolean                         is_accept,
  const ie_cm_cc_cause_T               *cc_cause_ptr,
  const cm_repeat_indicator_T          *bc_repeat_indicator_ptr,
  const cm_bearer_capability_T         *bearer_capability_1_ptr,
  const cm_bearer_capability_T         *bearer_capability_2_ptr
)
{
  mn_cnm_cmd_type                *mn_ptr;
    /* MN command pointer */

  cm_call_id_type                 call_id = CM_CALL_ID_UNASSIGNED;
    /* Pointer to a call object */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(call_ptr == NULL)
  {
    CM_ERR_0( "Invalid call_ptr");
    return;
  }

  call_id = call_ptr->call_id;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cc_cause_ptr             != NULL);
  CM_ASSERT(bc_repeat_indicator_ptr  != NULL);
  CM_ASSERT(bearer_capability_1_ptr  != NULL);
  CM_ASSERT(bearer_capability_2_ptr  != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  mn_ptr = cm_mn_get_buf_else_err_fatal(CM_MT_SETUP_RES);

  if (!cm_find_connection_id( call_id,
                             &mn_ptr->cmd.cm_mt_setup_res.connection_id))
  {
    CM_ERR_0( "Could not find connection id.");
  }

  mn_ptr->cmd.cm_mt_setup_res.cm_accept = is_accept;

  mn_ptr->cmd.cm_mt_setup_res.as_id     = call_ptr->asubs_id;

  /* Note: This check must be done in cmcall_client_cmd_check()
  ** and the cc_cause to be copied into the call object
  ** if required
  */
  if ( is_accept )
  {
    #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
    /* check if there is a gw cs call in conversation */	
    if( cmcall_check_if_gw_cs_call_active(CM_CALL_STATE_CONV, call_ptr->ss) )
    /* user busy = TRUE irrespective if CS voice calls active */
    {
      mn_ptr->cmd.cm_mt_setup_res.cm_cc_cause.present = TRUE;
      mn_ptr->cmd.cm_mt_setup_res.cm_cc_cause.coding_standard = STD_CODESTANDARD;
      mn_ptr->cmd.cm_mt_setup_res.cm_cc_cause.location = STD_LOCATION;
      mn_ptr->cmd.cm_mt_setup_res.cm_cc_cause.recommendation_present = FALSE;
      mn_ptr->cmd.cm_mt_setup_res.cm_cc_cause.diagnostic_length = 0;
      mn_ptr->cmd.cm_mt_setup_res.cm_cc_cause.cause_value = USER_BUSY;
    }
    else
    #endif
    {
      mn_ptr->cmd.cm_mt_setup_res.cm_cc_cause.present = FALSE;
    }
  }
  else
  {
    /* otherwise there are other cause reasons */
    mn_ptr->cmd.cm_mt_setup_res.cm_cc_cause = *cc_cause_ptr;
  }

  /* Copy the information to the call object */
  call_ptr->cmcall_mode_info.info.gsm_wcdma_info.cc_cause =
      mn_ptr->cmd.cm_mt_setup_res.cm_cc_cause;
  /*lint -restore */

  mn_ptr->cmd.cm_mt_setup_res.bc_repeat_indicator = *bc_repeat_indicator_ptr;

  mn_ptr->cmd.cm_mt_setup_res.bearer_capability_1 = *bearer_capability_1_ptr;

  mn_ptr->cmd.cm_mt_setup_res.bearer_capability_2 = *bearer_capability_2_ptr;

  #if defined(FEATURE_MULTIMEDIA_FALLBACK)
  cmwcall_set_codec_list (&mn_ptr->cmd.cm_mt_setup_res.codecs_list, call_ptr->asubs_id);
  #endif

  CM_MSG_HIGH_1("Send MT setup response CTM: %d  ", mn_ptr->cmd.cm_mt_setup_res.bearer_capability_1.ctm);
  cm_mn_send_cmd( mn_ptr );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_ptr->call_subst.incom =  CM_CALL_INCOM_SUBST_SETUP_PROCESSED;

} /* cmwcall_process_mt_setup_response */
#endif

/*===========================================================================

FUNCTION cmwcall_end_req

DESCRIPTION
  Does call end necessary activities.
  Sends end_req event.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_end_req(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_0( "cmwcall_end_req()");
  CM_ASSERT( call_ptr != NULL );

  /* call state IDLE means call end request already sent
     just return. no need for another call end request event*/
  if(call_ptr->call_state      == CM_CALL_STATE_IDLE)
  {
    return;
  }

  /* Change the call state to idle.*/
  call_ptr->call_state      = CM_CALL_STATE_IDLE;
  call_ptr->call_subst.idle = CM_CALL_IDLE_SUBST_ENDWAIT;

  if(call_ptr->cc_object_ptr != NULL)
  {
    return;
  }
  /* Notify clients of call end req event.*/
  cmcall_update_call_end_info( call_ptr );
  cmcall_event( call_ptr, CM_CALL_EVENT_END_REQ );

  /*
  ** Tell phone that Call End procedure has been
  ** started.
  */
  cmph_call_end_req(call_ptr);


}

#ifdef FEATURE_MMODE_GW

/*===========================================================================

FUNCTION cmwcall_reset_gw_wcdma_ps_info

DESCRIPTION
  Resets the gw_ps_info in call_ptr

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmwcall_reset_gw_wcdma_ps_info
(
  gsm_wcdma_cmcall_ps_type  *gsm_wcdma_ps_info
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(gsm_wcdma_ps_info != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize all fields to zero*/

  memset(gsm_wcdma_ps_info, 0, sizeof(gsm_wcdma_cmcall_ps_type));

  /* profile_no is set only when DS originates a call.
  ** In case of MT PDP need to know states between when
  ** DS is given a call setup indication and after DS
  ** accepts the call, profile_no is used for that
  */

  gsm_wcdma_ps_info->assoc_pri_profile_num
                          = CMCALL_PROFILE_NUM_INVALID;
  gsm_wcdma_ps_info->is_net_alloc_qos_diff_from_req  = FALSE;
  gsm_wcdma_ps_info->pdp_group_id = CM_DEFAULT_VALUE;


  gsm_wcdma_ps_info->profile_number = CMCALL_PROFILE_NUM_INVALID;
  gsm_wcdma_ps_info->low_priority_signaling = FALSE;

  gsm_wcdma_ps_info->apn_name.valid = FALSE;
  //gsm_wcdma_ps_info->cause = CM_DEFAULT_VALUE;
  //gsm_wcdma_ps_info->cause_type = CM_DEFAULT_VALUE;
  gsm_wcdma_ps_info->config_options.valid = FALSE;
  gsm_wcdma_ps_info->d_comp = SYS_PDP_DATA_COMP_NONE;
  gsm_wcdma_ps_info->h_comp =  SYS_PDP_HEADER_COMP_NONE;


  gsm_wcdma_ps_info->llc_sapi.valid = FALSE;
  gsm_wcdma_ps_info->minimum_qos.valid = FALSE;
  gsm_wcdma_ps_info->msg_type = CM_DEFAULT_VALUE;
  gsm_wcdma_ps_info->nsapi.valid = FALSE;
  gsm_wcdma_ps_info->pdp_addr.valid = FALSE;

  gsm_wcdma_ps_info->primary_pdp = FALSE;

  gsm_wcdma_ps_info->qos.valid = 0;
  gsm_wcdma_ps_info->rab_cause = CM_DEFAULT_VALUE;
  gsm_wcdma_ps_info->rabid = CM_DEFAULT_VALUE;
  gsm_wcdma_ps_info->sys_rat = SYS_RAT_NONE;
  gsm_wcdma_ps_info->sm_pd = CM_DEFAULT_VALUE;
  gsm_wcdma_ps_info->tear_down_ind.valid = FALSE;

  gsm_wcdma_ps_info->tft.valid = FALSE;
  gsm_wcdma_ps_info->trans_id = CM_DEFAULT_VALUE;

  #ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
  #error code not present
#endif

  #ifdef FEATURE_LTE
  gsm_wcdma_ps_info->esm_conn_id = 0;
  #endif

  gsm_wcdma_ps_info->backoff_timer.active = TRUE;
  gsm_wcdma_ps_info->backoff_timer.timer_count = 0;
  gsm_wcdma_ps_info->backoff_timer.valid= FALSE;

} /* cmwcall_reset_gw_wcdma_ps_info */
#endif

/*===========================================================================

FUNCTION cmwcall_reset

DESCRIPTION
  Reset GW-specific call fields to default values reflecting
  a GW call in idle state.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_reset(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_ptr->next_charged_call = NULL;

  call_ptr->gw_conn_id_src    = CONN_ID_UNKNOWN_SRC;
  call_ptr->gw_connection_id  = ( byte )CM_UNKNOWN_CONN_ID;

  #ifdef FEATURE_MMODE_GW
  /* Initialize gw_wcdma_ps_info */
  cmwcall_reset_gw_wcdma_ps_info(&call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info);
  #endif

  call_ptr->cmcall_mode_info.info.gsm_wcdma_info.rab_id_present = FALSE;

  #ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
  #error code not present
#endif

  call_ptr->cmcall_mode_info.info.gsm_wcdma_info.new_call_type_req
                          = CM_CALL_TYPE_NONE;

  call_ptr->cmcall_mode_info.info.gsm_wcdma_info.is_network_initiated
                          = TRUE;

  cmcall_set_gw_cc_result(call_ptr,CM_CC_RESULT_NOT_ALLOWED);

}


#if defined(FEATURE_MMODE_EMERG_SUPPORT)
/*===========================================================================

FUNCTION cmwcall_set_emergency_call_no_sim

DESCRIPTION
  Set the static boolean cmwcall_emergency_call_no_sim

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_set_emergency_call_no_sim(boolean set_value) {

  cmwcall_emergency_call_no_sim = set_value;
}
#endif

/*===========================================================================

FUNCTION cmwcall_end

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
void cmwcall_end(

    cmcall_s_type  *call_ptr
        /* pointer to a call object */
)
{
  cm_call_mode_info_e_type call_mode_being_ended = CM_CALL_MODE_INFO_NONE;
    /* Used to record the call mode being ended
    */

  cm_act_update_reas_e_type update_reason = CM_ACT_UPDATE_REAS_ACT_END;

  cm_ss_e_type    ss;
  sys_modem_as_id_e_type as_id = call_ptr->asubs_id;


  CM_MSG_HIGH_0( "cmwcall_end()");
  CM_ASSERT( call_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss = call_ptr->ss;

  #if defined(FEATURE_MMODE_3GPP) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE) && !defined(FEATURE_MMODE_GPRS_LTE)
  cmwaoc_end_call(call_ptr);
  #endif

  #if defined(FEATURE_MMODE_3GPP) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)

  #if defined(FEATURE_MMODE_EMERG_SUPPORT)

  /* check to see if we need to notify mmoc that we ended an
  ** emergency call without a SIM
  */

  if (cmwcall_emergency_call_no_sim)
  {
    cmwcall_emergency_call_no_sim = FALSE;
    cmph_notify_mmoc_no_sim(call_ptr->ss);
  }
  #endif
  #endif /* FEATURE_WCDMA || FEATURE_GSM */

  /* Change the call state to idle.
  */
  call_ptr->call_state = CM_CALL_STATE_IDLE;
  call_ptr->call_subst.idle = CM_CALL_IDLE_SUBST_NONE;

  cmcall_update_gsdi_and_log_call_state(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Notify clients of call end event.
  */
  /*
  ** Determine if the CM_CALL_EVENT_END should be sent to the client.
  ** The event is not sent if a setup indication was not accepted when
  ** there is a already a pending setup (i.e. can only process one
  ** incoming call at a time).
  */
  if ( call_ptr->send_call_event_end )
  {
    cmcall_update_call_end_info( call_ptr );

    /* Donot send end event, when call waiting for reorig phase2*/
    if(call_ptr->cc_object_ptr == NULL)
    {
      cmcall_event( call_ptr, CM_CALL_EVENT_END );
    }
  }


  if(call_ptr->end_status == CM_CALL_END_EMERGENCY_ONLY ||
    call_ptr->end_status == CM_CALL_END_THERMAL_EMERGENCY)
  {
    update_reason = CM_ACT_UPDATE_REAS_EMERG_ENTER;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the proprity queue's for this activity end
  */

  (void)cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                 CM_SS_MAIN,
                                 update_reason );

  (void)cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                 CM_SS_HYBR_2,
                                 update_reason );

  #ifdef FEATURE_MMODE_HYBR3_ENABLED
  (void)cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                 CM_SS_HYBR_3,
                                 update_reason );
  #endif

  (void)cmph_update_orig_param( (cm_act_id_type) call_ptr->call_id,
                                 CM_SS_HDR,
                                 update_reason );

  /* If there are no calls active, reset the phone variables.
  */
  /* Tell phone that call is over.
     Number of allocated calls equal to 1 means that this is the
     last call that is being ended, so we need to notify the phone
     now.
  */
  cmph_call_end_process(call_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* To decide on ciphering indication status
  */
  call_mode_being_ended = call_ptr->cmcall_mode_info.info_type;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* free the call ID */
  cm_call_id_deallocate ( cmcall_call_id_get( call_ptr ) );

  if (cmph_is_feature_mode_sglte() ||
      cmph_is_subs_feature_mode_sglte(as_id))
  {
    cmpmprx_notify_call_end(FALSE); //cmph_map_cm_ss_to_as_id(ss)
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update DDTM status to take care of emerg call end.
  */
  cmph_update_ddtm_status(cmph_ptr());

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Check if this is the last cs domain or ps domain call being ended
  ** and update ciphering indication accordingly.
  */
  #if defined(FEATURE_MMODE_GW) && !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
  if (call_mode_being_ended == CM_CALL_MODE_INFO_GW_CS &&
      cmcall_is_no_call_in_gw_cs(ss)
     )
  {
    cmss_rel_cipher_ind (SYS_SRV_DOMAIN_CS_ONLY);
  }
  else 
  #endif
  if (call_mode_being_ended == CM_CALL_MODE_INFO_GW_PS)
  {
    if (cmcall_is_no_call_in_gw_ps(ss)
        #ifdef FEATURE_CM_LTE
        && cmcall_is_no_call_in_lte_ps()
        #endif
       )
    {
      cmss_rel_cipher_ind (SYS_SRV_DOMAIN_PS_ONLY);
    }
  }

  /* When all the voice calls have ended, if there is an existing suspended data
  ** that is CM initiated, Update the suspend state
  */
  if( (cmcall_is_there_a_call_type(CM_SS_MAX,CM_CALL_TYPE_VOICE,CM_CALL_ID_INVALID) == CM_CALL_ID_INVALID )&&
	    (cmcall_is_there_a_call_type(CM_SS_MAX,CM_CALL_TYPE_SMS,CM_CALL_ID_INVALID) == CM_CALL_ID_INVALID )&&
      (cmcall_is_there_a_call_type(CM_SS_MAX,CM_CALL_TYPE_EMERGENCY,CM_CALL_ID_INVALID) == CM_CALL_ID_INVALID )&&
       cmss_ptr()->ue_init_ps_data_suspend != CM_SS_NONE
    )
  {

    if( cmss_ptr()->ue_init_ps_data_suspend == CM_SS_MAIN )
    {
      cmss_process_data_suspend_rpt(FALSE);
    }
    #ifdef FEATURE_MMODE_HYBR3_ENABLED
    else if( cmss_ptr()->ue_init_ps_data_suspend == CM_SS_HYBR_3)
    {
      cmss_process_hybr_3_data_suspend_rpt(FALSE);
    }
    #endif
    #ifdef FEATURE_MMODE_HYBR2_ENABLED
    else
    {
      cmss_process_hybr_2_data_suspend_rpt(FALSE);
    }
    #endif

  }

}


/*===========================================================================

FUNCTION cmwcall_end_call

DESCRIPTION
  End call active call.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_end_call (
  cmcall_s_type  *call_ptr /* pointer to a call object */
)
{

  #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
  mn_cnm_cmd_type           *mn_ptr;
  #endif
  cm_call_id_type           call_id = CM_CALL_ID_INVALID;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(call_ptr == NULL)
  {
    CM_ERR_0( "Invalid call_ptr");
    return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Tell the lower layer to tear down the call */
  switch( call_ptr->call_type )
  {
    #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
    case CM_CALL_TYPE_VOICE:
    case CM_CALL_TYPE_CS_DATA:
      mn_ptr = cm_mn_get_buf_else_err_fatal(CM_MO_END_X_CALL_REQ);
    /* Set the subscription id on which request is being sent */

      mn_ptr->cmd.cm_n_x_send_req.as_id = call_ptr->asubs_id;
      mn_ptr->cmd.cm_n_x_send_req.connection_id =call_ptr->gw_connection_id;
      mn_ptr->cmd.cm_n_x_send_req.cause_value = NORMAL_CALL_CLEARING;
      cm_mn_send_cmd( mn_ptr );
      break;
      #endif

      default:
        CM_MSG_HIGH_1("End CALL : Call type %d not supported in CS domain", call_ptr->call_type);
  }
  if(call_ptr->cc_object_ptr == NULL)
  {
    /* Tell the CM clients that the call will be ended */
    cmcall_update_call_end_info( call_ptr );
    cmcall_event( call_ptr, CM_CALL_EVENT_END_REQ );
    /*lint -restore */
  }

}

/*===========================================================================

FUNCTION cmwcall_cmd_copy_fields

DESCRIPTION
  Copy the active fields of a call command into a call object.
  Copy GW specific fields only

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_cmd_copy_fields(

    cmcall_s_type               *call_ptr,
        /* Pointer to a call object */

    const cm_call_cmd_s_type    *cmd_ptr
        /* Pointer to a call command */

)
{
  /* Point at call command information.
  */
  const cm_call_cmd_info_gw_cs_s_type     *cs_cmd_info_ptr    = WCALL_CMD_INFO_PTR( cmd_ptr );
  const cm_call_cmd_info_s_type           *cmd_info_ptr       = CMD_INFO_PTR( cmd_ptr );
  gsm_wcdma_cmcall_type                   *wcall_info_ptr     = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;
  const cm_call_cmd_info_gw_ps_s_type     *ps_cmd_info_ptr    = NULL;
  gsm_wcdma_cmcall_ps_type                *wcall_ps_info_ptr  = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_ptr != NULL )
  CM_ASSERT( cmd_ptr  != NULL )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  /* Copy GW specific fields
  */
  
  #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
  {
    cs_cmd_info_ptr      = WCALL_CMD_INFO_PTR( cmd_ptr );
    wcall_info_ptr    = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

    if ( cs_cmd_info_ptr->accept != CM_DEFAULT_VALUE )
    {
      wcall_info_ptr->accept = cs_cmd_info_ptr->accept;
    }

    if ( cs_cmd_info_ptr->bc_repeat_indicator.present)
    {
      wcall_info_ptr->bc_repeat_indicator = cs_cmd_info_ptr->bc_repeat_indicator;
    }

    if ( cs_cmd_info_ptr->bearer_capability_1.present )
    {
      wcall_info_ptr->bearer_capability_1 = cs_cmd_info_ptr->bearer_capability_1;
    }

    if ( cs_cmd_info_ptr->bearer_capability_2.present )
    {
      wcall_info_ptr->bearer_capability_2 = cs_cmd_info_ptr->bearer_capability_2;
    }

    if ( cs_cmd_info_ptr->llc_repeat_indicator.present )
    {
      wcall_info_ptr->llc_repeat_indicator = cs_cmd_info_ptr->llc_repeat_indicator;
    }

    if ( cs_cmd_info_ptr->low_layer_compatibility_1.present )
    {
      wcall_info_ptr->low_layer_compatibility_1 = cs_cmd_info_ptr->low_layer_compatibility_1;
    }

    if ( cs_cmd_info_ptr->low_layer_compatibility_2.present )
    {
      wcall_info_ptr->low_layer_compatibility_2 = cs_cmd_info_ptr->low_layer_compatibility_2;
    }

    if ( cs_cmd_info_ptr->hlc_repeat_indicator.present )
    {
      wcall_info_ptr->hlc_repeat_indicator = cs_cmd_info_ptr->hlc_repeat_indicator;
    }

    if ( cs_cmd_info_ptr->high_layer_compatibility_1.present )
    {
      wcall_info_ptr->high_layer_compatibility_1 = cs_cmd_info_ptr->high_layer_compatibility_1;
    }

    if ( cs_cmd_info_ptr->high_layer_compatibility_2.present )
    {
      wcall_info_ptr->high_layer_compatibility_2 = cs_cmd_info_ptr->high_layer_compatibility_2;
    }

    if ( cs_cmd_info_ptr->called_party_bcd_number.present )
    {
      wcall_info_ptr->called_party_bcd_number = cs_cmd_info_ptr->called_party_bcd_number;
    }

    if ( cs_cmd_info_ptr->called_party_subaddress.present )
    {
      wcall_info_ptr->called_party_subaddress = cs_cmd_info_ptr->called_party_subaddress;
    }

    if ( cs_cmd_info_ptr->calling_party_bcd_number.present )
    {
      wcall_info_ptr->calling_party_bcd_number = cs_cmd_info_ptr->calling_party_bcd_number;
    }

    if ( cs_cmd_info_ptr->calling_party_subaddress.present )
    {
      wcall_info_ptr->calling_party_subaddress = cs_cmd_info_ptr->calling_party_subaddress;
    }

    if ( cs_cmd_info_ptr->forward_cug_info.present )
    {
      wcall_info_ptr->forward_cug_info = cs_cmd_info_ptr->forward_cug_info;
    }

    if ( cs_cmd_info_ptr->cm_clir != CM_CLIR_DEFAULT )
    {
      wcall_info_ptr->cm_clir = cs_cmd_info_ptr->cm_clir;
    }

    if( cmd_info_ptr->sups_cmd_type != CM_CALL_SUPS_TYPE_NONE )
    {
      wcall_info_ptr->sups_type   = cmd_info_ptr->sups_cmd_type;
      wcall_info_ptr->sups_params = cs_cmd_info_ptr->sups_cmd_params;
    }

    #ifdef FEATURE_UUS
    if ( cs_cmd_info_ptr->mo_user_data.present )
    {
      wcall_info_ptr->mo_mt_user_data = cs_cmd_info_ptr->mo_user_data;
    }
    #endif

    wcall_info_ptr->ccbs_recall_rsp   = cs_cmd_info_ptr->ccbs_recall_rsp;

    wcall_info_ptr->cc_cause          = cs_cmd_info_ptr->cc_cause;
  }
  #endif
  
  #if defined (FEATURE_MULTIMEDIA_FALLBACK)

  /* Copy new call type requested into call object
  */
  if (cs_cmd_info_ptr->new_call_type_req != CM_CALL_TYPE_NONE)
  {
    wcall_info_ptr->new_call_type_req = cs_cmd_info_ptr->new_call_type_req;
  }

  /* Copy modify response to call object
  */
  wcall_info_ptr->gw_cs_modify_res  = cs_cmd_info_ptr->gw_cs_modify_res;

  #endif

  //wcall_info_ptr->cc_cause = cs_cmd_info_ptr->cc_cause;


}

/*===========================================================================

FUNCTION cmwcall_process_ss_notify_ccbs_recall

DESCRIPTION
  SS notification for CCBS recall is processed here

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  TRUE  - If SS Notify is for ss_code CCBS
  FALSE - If SS Notify is not for ss_code CCBS

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmwcall_process_ss_notify_ccbs_recall(


      cm_rpt_type     *cm_rpt_ptr
        /* Pointer to Lower Layer report */
)
/*lint -esym(818, cm_rpt_ptr) */
{
#if defined(FEATURE_CCBS) && defined(FEATURE_MMODE_SUPPORT_SUPS)

  ie_cm_notify_ss_T       *cm_notify_ss = NULL;
     /* Pointer to the Notify SS Indication */

  cmcall_s_type            *call_ptr = NULL;
      /* Point at call object */

  gsm_wcdma_cmcall_type    *wcall_info_ptr = NULL;
      /* Point at GW specific information */

  cm_call_id_type           call_id = CM_CALL_ID_INVALID;

  ie_ccbs_indicator_T       *ccbs_indicator;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cm_rpt_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* it is a Notify SS indication */
  cm_notify_ss = &cm_rpt_ptr->cmd.call_ss_ind.data.cm_notify_ss;

  ccbs_indicator = &cm_notify_ss->cm_ccbs_indicator;

  if ( ( cm_notify_ss->cm_ccbs_indicator.present )         &&
       ( cm_notify_ss->cm_ss_code.present == TRUE) &&
       ( cm_notify_ss->cm_ss_code.ss_code == (byte)ccbs )  )
  {

    if (ccbs_indicator->ccbsIndex.present)
    {

      /* CM needs to use the connection id given by MN to respond to the
      ** recall. cm_new_incom_call_id() can create a call object that
      ** is attached to the given connection_id. CCBS recall indication
      ** is not a MT call but a indication to user about remote party
      ** being available for a MO call.
      */
      if( !cm_new_incom_call_id ( CM_CALL_TYPE_VOICE, &call_id,
                                  cm_rpt_ptr->cmd.call_ss_ind.connection_id))
      {
        CM_ERR_0( " Recall Ind : Call id alloc Failed");
        return TRUE;
      }

      /* Get the call pointer from the call ID
      */
      call_ptr = cmcall_ptr ( call_id );

      if(call_ptr == NULL)
      {
        CM_ERR_1( " call ptr for Call id  %d is NULL", call_id);
        /*lint -save -e527 Unreachable */
        return TRUE;
        /*lint -restore */
      }

      /* Assign the SS of the call based on the subscription-id
      */
      call_ptr->ss = cmph_determine_ss_per_domain(cm_rpt_ptr->cmd.call_ss_ind.as_id,
                                                  SYS_SRV_DOMAIN_CS_ONLY);

      if (!cmph_is_valid_asubs_id_from_ll(cm_rpt_ptr->cmd.call_ss_ind.as_id))
      {
        CM_ERR_1(" Invalid subs id %d",cm_rpt_ptr->cmd.call_ss_ind.as_id);
        return TRUE;
      }
      call_ptr->asubs_id = cmph_get_as_id_from_ll_ind(cm_rpt_ptr->cmd.call_ss_ind.as_id);

      /* Fill relevent parameters of call object for CCBS recall indication
      */
      #if defined( FEATURE_CCBS)
      cmwcall_fill_call_obj_for_ccbs (call_ptr);
	  #endif

      /* Updates phone in use flag
      */
      cmph_call_start( call_ptr );

      wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

      /* Copy CCBS specific information
      */
      wcall_info_ptr->ccbs_ret_res.ccbs_cnf = CM_CCBS_CNF_RECALL_IND;

      /* ccbs indx present flag
      */
      wcall_info_ptr->ccbs_ret_res.ccbs_indx_present =
                                             ccbs_indicator->ccbsIndex.present;

      /* Copy ccbs present information
      */
      wcall_info_ptr->ccbs_ret_res.present           = ccbs_indicator->present;

      /* ccbs Index value
      */
      wcall_info_ptr->ccbs_ret_res.ccbs_indx         =
                                          ccbs_indicator->ccbsIndex.ccbs_Index;

      /* called party bcd number
      */
      wcall_info_ptr->called_party_bcd_number        =
                                            ccbs_indicator->subscriberNumber[0];

      /* called party subaddress
      */
      wcall_info_ptr->called_party_subaddress        =
                                             ccbs_indicator->subscriberAddress;

      /* Since recall has been received and called number details are
      ** stored in the call object, delete the entry in phone object
      */
      if (!cmph_remove_ccbs_entry (call_ptr->ss,
                                   ccbs_indicator->ccbsIndex.ccbs_Index))
      {
        CM_ERR_0 (" ccbs entry could not be removed ");
      }

      /* Need to copy more fields from MN and also add populate other
      ** call object fields to let silent redial succeed.
      */
    }

    return TRUE;
  }
  #else
  SYS_ARG_NOT_USED(cm_rpt_ptr);
  #endif /* FEATURE_CCBS */

  return FALSE;
} /* cmwcall_process_ss_notify_ccbs_recall () */
/*lint +esym(818, cm_rpt_ptr) */


/*===========================================================================

FUNCTION cmwcall_info_get

DESCRIPTION
  Copy the current call state information into a specified buffer.

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
boolean cmwcall_info_get(

    cm_mm_call_info_s_type        *call_info_ptr,
        /* Copy call state info into this buffer */

    const cmcall_s_type           *call_ptr
        /* Pointer to the call object  */
)
{
  const gsm_wcdma_cmcall_type    *wcall_mode_ptr    = NULL;
  cm_gw_cs_call_info_s_type      *info_mode_ptr     = NULL;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( call_info_ptr != NULL );
  CM_ASSERT( call_ptr != NULL );
  CM_ASSERT( CM_INIT_CHECK(call_ptr->init_mask) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  wcall_mode_ptr    =  &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;
  info_mode_ptr     = &call_info_ptr->mode_info.info.gw_cs_call;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( call_ptr->cmcall_mode_info.info_type )
    {
    #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
    case CM_CALL_MODE_INFO_GW_CS:
      info_mode_ptr->transaction_id            = wcall_mode_ptr->transaction_id;
      info_mode_ptr->accept                    = wcall_mode_ptr->accept;
      info_mode_ptr->bc_repeat_indicator       = wcall_mode_ptr->bc_repeat_indicator;
      info_mode_ptr->bearer_capability_1       = wcall_mode_ptr->bearer_capability_1;
      info_mode_ptr->bearer_capability_2       = wcall_mode_ptr->bearer_capability_2;
      info_mode_ptr->llc_repeat_indicator      = wcall_mode_ptr->llc_repeat_indicator;
      info_mode_ptr->low_layer_compatibility_1 = wcall_mode_ptr->low_layer_compatibility_1;
      info_mode_ptr->low_layer_compatibility_2 = wcall_mode_ptr->low_layer_compatibility_2;
      info_mode_ptr->hlc_repeat_indicator      = wcall_mode_ptr->hlc_repeat_indicator;
      info_mode_ptr->high_layer_compatibility_1 = wcall_mode_ptr->high_layer_compatibility_1;
      info_mode_ptr->high_layer_compatibility_2 = wcall_mode_ptr->high_layer_compatibility_2;
      info_mode_ptr->called_party_bcd_number   = wcall_mode_ptr->called_party_bcd_number;
      info_mode_ptr->called_party_subaddress   = wcall_mode_ptr->called_party_subaddress;
      info_mode_ptr->calling_party_bcd_number  = wcall_mode_ptr->calling_party_bcd_number;
      info_mode_ptr->calling_party_subaddress  = wcall_mode_ptr->calling_party_subaddress;
      info_mode_ptr->redirect_party_number     = wcall_mode_ptr->redirect_party_number;
      info_mode_ptr->forward_cug_info          = wcall_mode_ptr->forward_cug_info;
      info_mode_ptr->cm_clir                   = wcall_mode_ptr->cm_clir;
      info_mode_ptr->cc_cause                  = wcall_mode_ptr->cc_cause;
      info_mode_ptr->inband_tone_present       = wcall_mode_ptr->inband_tone_present;
      info_mode_ptr->cc_reject                 = wcall_mode_ptr->cc_reject;
      info_mode_ptr->call_details              = wcall_mode_ptr->call_details;
      info_mode_ptr->call_progress             = wcall_mode_ptr->call_progress;
      info_mode_ptr->connected_num             = wcall_mode_ptr->connected_num;
      info_mode_ptr->connected_subaddress      = wcall_mode_ptr->connected_subaddress;
      info_mode_ptr->cause_of_no_cli           = wcall_mode_ptr->cause_of_no_cli;
      info_mode_ptr->ect_info                  = wcall_mode_ptr->ect_info;
      info_mode_ptr->ss_error                  = wcall_mode_ptr->ss_error;
      info_mode_ptr->ss_info                   = wcall_mode_ptr->ss_info;
      info_mode_ptr->rab_id                    = wcall_mode_ptr->rab_id;
      info_mode_ptr->rab_id_present            = wcall_mode_ptr->rab_id_present;
      info_mode_ptr->ccbs_ret_res              = wcall_mode_ptr->ccbs_ret_res;
      info_mode_ptr->gw_handover_cause         = wcall_mode_ptr->handover_cause;
      info_mode_ptr->cm_alerting_pattern       = wcall_mode_ptr->alerting_pattern;

      #ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
      #error code not present
#endif
      info_mode_ptr->sups_type                 = wcall_mode_ptr->sups_type;
      info_mode_ptr->sups_params               = wcall_mode_ptr->sups_params;
      info_mode_ptr->ccbs_recall_rsp           = wcall_mode_ptr->ccbs_recall_rsp;

      #if defined (FEATURE_MULTIMEDIA_FALLBACK)
      info_mode_ptr->new_call_type_req         = wcall_mode_ptr->new_call_type_req;
      info_mode_ptr->incall_modif_err_status   = wcall_mode_ptr->incall_modif_err_status;
      info_mode_ptr->gw_cs_modify_res          = wcall_mode_ptr->gw_cs_modify_res;
      info_mode_ptr->is_call_mod_possible      = wcall_mode_ptr->is_call_mod_possible;
      info_mode_ptr->is_network_initiated      = wcall_mode_ptr->is_network_initiated;
      #endif

      /* for call events CM_CALL_EVENT_USER_DATA_IND and
      ** CM_CALL_EVENT_USER_DATA_CONG_IND
      */
      info_mode_ptr->end_user_data.cm_cc_cause      = wcall_mode_ptr->cm_cc_cause;
      #ifdef FEATURE_UUS
      info_mode_ptr->end_user_data.mt_user_data     = wcall_mode_ptr->mo_mt_user_data;
      #endif
      info_mode_ptr->end_user_data.is_receiver_busy = wcall_mode_ptr->is_receiver_busy;

      break;
	#endif

    default:
      CM_MSG_HIGH_1("Info type %d not supported", call_ptr->cmcall_mode_info.info_type);
    }


  /* Return boolean indicating whether call information is valid.
  */
  return CM_BOOLEAN( CM_INIT_CHECK(call_ptr->init_mask) );
}

#if defined(FEATURE_MMODE_GW)
/*===========================================================================

FUNCTION cmwcall_client_orig_proc

DESCRIPTION
  Process clients' call origination command.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_call_cmd_err_e_type cmwcall_client_orig_proc(

    cmcall_s_type  *call_ptr
        /* pointer to a CM command */
)
{
  cm_call_cmd_err_e_type           cmd_err         = CM_CALL_CMD_ERR_NOERR;

  cmph_s_type                     *ph_ptr          = cmph_ptr();
      /* Point at phone object */

  cmss_s_type                     *ss_ptr          = cmss_ptr();
    /* Pointer to the serving system object */

  cm_orig_q_s_type                *ph_orig_top_ptr = NULL;
  cm_ss_e_type                     temp_ss         = CM_SS_NONE;
    /* Local temp variable */

  cm_cc_subscription_source_e_type subscription_source;

  #if defined(FEATURE_MMODE_3GPP) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)  && !defined (FEATURE_MMODE_LTE_ONLY) && !defined(FEATURE_MMODE_GPRS_LTE)	
  cm_overall_call_state_e_type     overall_call_state;
  #endif
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr           != NULL);

  if (call_ptr == NULL)
  {
    CM_ERR_0 ("NULL call_ptr!");
    return CM_CALL_CMD_ERR_NULL_PARAM_P;
  }

  temp_ss = call_ptr->ss;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* check for call command parameter errors and whether a specified call
  ** command is allowed in the current state of the call/phone.
  ** Used to be cmwcall_client_orig_check ()
  */
  /* check cmd is already in error or not */
  #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
  /* Check if the SIM allows CS calls - emergency calls are allowed */
  {
    if ( ( ( call_ptr->call_type == CM_CALL_TYPE_VOICE )  ||
           ( call_ptr->call_type == CM_CALL_TYPE_CS_DATA)
         ) &&
         ( ( ss_ptr->info.sim_state == SYS_SIM_STATE_CS_INVALID && !(call_ptr->is_volte && call_ptr->call_type == CM_CALL_TYPE_VOICE)  ) ||
           ( ss_ptr->info.sim_state == SYS_SIM_STATE_CS_PS_INVALID )
         )
       )
    {
      return CM_CALL_CMD_ERR_INVALID_SIM_STATE_S;
    }
  }
  #endif
  
  subscription_source = cmph_get_gw_subscription_source (call_ptr->ss);

  if (( call_ptr->call_type != CM_CALL_TYPE_EMERGENCY )&& (!cmph_is_msim()) )
  {
    /* This check is not required if DUAL SIM is defined,
    ** taken care inside get_gw_subscription_source
    */
    if ( subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_1 &&
         !ph_ptr->valid_card_1)
    {
      CM_MSG_HIGH_0( "Orig: UIM 1 not present!");
      return CM_CALL_CMD_ERR_UIM_NOT_PRESENT_S;
    }
    #if defined(FEATURE_MMODE_DUAL_SIM)
    else if (subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_2 &&
             !ph_ptr->valid_card_2)
    {
      CM_MSG_HIGH_0( "Orig: UIM 2 not present!");
      return CM_CALL_CMD_ERR_UIM_NOT_PRESENT_S;
    }
    #endif
    #ifdef FEATURE_MMODE_HYBR3_ENABLED
    else if (subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_3 &&
             !ph_ptr->valid_card_3)
    {
      CM_MSG_HIGH_0( "Orig: UIM 3 not present!");
      return CM_CALL_CMD_ERR_UIM_NOT_PRESENT_S;
    }
    #endif
    else if (subscription_source == CM_CC_SUBSCRIPTION_SOURCE_NONE)
    {
      CM_MSG_HIGH_0( "Orig: No subscription information!");
      return CM_CALL_CMD_ERR_UIM_NOT_PRESENT_S;
    }

    CM_MSG_HIGH_1 ("Valid UIM %d", subscription_source);
  }
  else if(call_ptr->call_type != CM_CALL_TYPE_EMERGENCY)
  {
    if((call_ptr->asubs_id == SYS_MODEM_AS_ID_1 && !ph_ptr->valid_card_1) 
       || subscription_source == CM_CC_SUBSCRIPTION_SOURCE_NONE)
    {
      CM_MSG_HIGH_0( "Orig: No subscription information!");
      return CM_CALL_CMD_ERR_UIM_NOT_PRESENT_S;
    }
  }

  /*
  ** If this is a non-emergency call, then the subscription information
  ** must be available/verified.
  */
  {
    if( ( call_ptr->call_type != CM_CALL_TYPE_EMERGENCY ) &&
        ( ph_ptr->is_gwl_subscription_available == FALSE   )    )
    {
          CM_MSG_HIGH_0( "Orig: Subscription info not available!");
          return CM_CALL_CMD_ERR_UIM_NOT_PRESENT_S;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_MMODE_3GPP) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE) && !defined(FEATURE_MMODE_GPRS_LTE)
  if ( call_ptr->call_type != CM_CALL_TYPE_EMERGENCY &&
       cmph_aoc_no_funds_available(call_ptr->ss)
     )
  {
    return CM_CALL_CMD_ERR_NO_FUNDS_S;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Sanity check on emergency call type
  */
  if (call_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
  {
    CM_ASSERT (INRANGE(call_ptr->num.emerg_srv_categ,
                 CM_EMERG_NUM_TYPE_NONE, CM_EMERG_NUM_TYPE_MAX));

    CM_MSG_HIGH_1 ("Emerg serv categ in orig is %d",
                      call_ptr->num.emerg_srv_categ);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Kick the dog after making the above function call.
  */
  cm_kick_dog();

  #if !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE) 
  if( !cmwcall_check_if_mo_call_possible( call_ptr ) )
  {
    return CM_CALL_CMD_ERR_IN_USE_S;
  }
  #endif

  #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
  /* If we are originating a GW voice call, need to handle call collisions:
  ** 1. If there's already an incoming 1X voice call -> end this GW MO call.
  */
  if( call_ptr->call_type == CM_CALL_TYPE_VOICE
     && call_ptr->ss == CM_SS_MAIN)
  {
    /* Find the existing voice call beside the current voice call */
    cm_call_id_type voice_call_id;
    voice_call_id     = cmcall_is_there_a_call_type(CM_SS_MAIN, CM_CALL_TYPE_VOICE, call_ptr->call_id);

    if (voice_call_id != CM_CALL_ID_INVALID)
    {
      cmcall_s_type *voice_call_ptr = cmcall_ptr(voice_call_id);

      if(voice_call_ptr != NULL)
      {
        /* If there's already an 1X incoming call, we would terminate the
        ** current GW MO call, which by simply return error.
        */
        if ( (voice_call_ptr->ss      == CM_SS_MAIN)             &&
             (voice_call_ptr->direction == CM_CALL_DIRECTION_MT) &&
             (voice_call_ptr->sys_mode  == SYS_SYS_MODE_CDMA))
        {
          return CM_CALL_CMD_ERR_REJECT_DURING_INCOMING_CALL;
        }
      }
    }
  }
  #endif

  #if defined(FEATURE_MMODE_3GPP) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)  && !defined (FEATURE_MMODE_LTE_ONLY) && !defined(FEATURE_MMODE_GPRS_LTE)
  overall_call_state = cmcall_overall_call_state_per_subs(call_ptr->ss);

  if (overall_call_state  != CM_OVERALL_CALL_STATE_CONV)
  {
    #ifdef FEATURE_MMODE_HYBR3_ENABLED
    if( call_ptr->ss == CM_SS_HYBR_3 )
    {
      call_ptr->ph_ptr->hybr_3_stack_info.phone_charge.CCM = 0;
      call_ptr->ph_ptr->hybr_3_stack_info.phone_charge.last_CCM_to_update_ACM = 0;
    }
    else
    #endif
    #if defined(FEATURE_MMODE_DUAL_SIM)
    if(cmph_is_msim() &&
       call_ptr->ss == CM_SS_HYBR_2 )
    {
      if (cmph_is_sxlte())
      {
        call_ptr->ph_ptr->main_stack_info.phone_charge.CCM = 0;
        call_ptr->ph_ptr->main_stack_info.phone_charge.last_CCM_to_update_ACM = 0;
      }
      else
      {
        call_ptr->ph_ptr->hybr_2_stack_info.phone_charge.CCM = 0;
        call_ptr->ph_ptr->hybr_2_stack_info.phone_charge.last_CCM_to_update_ACM = 0;
      }
    }
    else
    #endif
    {
      call_ptr->ph_ptr->main_stack_info.phone_charge.CCM = 0;
      call_ptr->ph_ptr->main_stack_info.phone_charge.last_CCM_to_update_ACM = 0;
    }
  }
  #endif
  /* We need to kick sd to get ok to orig
  */
  if ( call_ptr->ss == CM_SS_MAX )
  {
    temp_ss = CM_SS_MAIN;
  }

  ph_orig_top_ptr = cmtask_orig_para_get_top( temp_ss );

  if (ph_orig_top_ptr == NULL)
  {
    CM_ERR_1("ph_orig_top_ptr NULL for ss %d", temp_ss);
    return CM_CALL_CMD_ERR_OTHER;
  }

  /* CM_ACT_TYPE_POLICYMAN_RESTRICT only applies to sglte */

  /* At this point we also check if there is another call VOICE call
  ** on the Priority Queue. If YES, we set the force_top_pref flag
  ** to FALSE. This is done because if the flag is TRUE, CM sends
  ** forces the preferences for the first call twice once with
  ** pref_reason REDIAL_NORM and again for the same call with
  ** pref_reason ORIG_CS. Check if the top object of priority queue
  ** is a call object with with the same priority as the current
  ** call obj  */

  #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
  if ( ph_orig_top_ptr->act_type == CM_ACT_TYPE_POLICYMAN_RESTRICT ||
       ( call_ptr->call_type == CM_CALL_TYPE_VOICE &&
         call_ptr->ss == CM_SS_MAIN &&
         ( ss_ptr->main_srv_status == SYS_SRV_STATUS_NO_SRV ||
           ss_ptr->main_srv_status == SYS_SRV_STATUS_NO_SRV_INTERNAL ) &&
         ph_orig_top_ptr->act_type == cmph_map_call_type_to_act_type( call_ptr->call_type ) &&
         ph_orig_top_ptr->orig->act_priority == CM_ACT_PRIORITY_30 )
     )
  {
    CM_MSG_HIGH_5( "cmwcall_orig(): ph_orig_top->act_type %d, call_type %d, ss %d, main_srv_stat %d, force_top_pref %d",
                 ph_orig_top_ptr->act_type, call_ptr->call_type,
                 call_ptr->ss, ss_ptr->main_srv_status, call_ptr->force_top_pref);
   
    call_ptr->force_top_pref = FALSE;

   
  }
  else
  #endif
  {
    call_ptr->force_top_pref = TRUE;
  }

  #if defined (FEATURE_MULTIMEDIA_FALLBACK)

  /* If BC2 is present, BC1 also needs to be present
  */
  if (!cmwcall_check_for_BC1_if_BC2_present (
        &call_ptr->cmcall_mode_info.info.gsm_wcdma_info.bearer_capability_1,
        &call_ptr->cmcall_mode_info.info.gsm_wcdma_info.bearer_capability_2 )
     )
  {
    return CM_CALL_CMD_ERR_INVALID_BC_P;
  }

  /* Set in-call modification field after all checks have passed on cmd_info
  */
  cmwcall_set_incall_modif_field (call_ptr);
  #endif

  return cmd_err;

}  /* cmwcall_client_orig_proc () */
#endif

/*===========================================================================

FUNCTION cmwcall_set_call_ptr_sys_mode

DESCRIPTION
  Set call_ptr->sys_mode to ss_ptr->last_system_reported
  Called before sending CALL events for PS calls.
  Helps call object to be insync with clients (call_ptr can go
  out of sync with clients because of cmss_event throttling)

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmwcall_set_call_ptr_sys_mode(

    cmcall_s_type    *call_ptr
        /* pointer to a CM command */
)
{
  CM_ASSERT(call_ptr != NULL);

  #ifdef FEATURE_MMODE_HYBR3_ENABLED
  if(call_ptr->ss == CM_SS_HYBR_3)
  {
    if (cmss_ptr()->hybr_3_gw_last_system_reported != SYS_SYS_MODE_NO_SRV)
    {
      call_ptr->sys_mode = cmss_ptr()->hybr_3_gw_last_system_reported;
    }
  }
  else
  #endif
  #ifdef FEATURE_MMODE_HYBR2_ENABLED
  if(call_ptr->ss == CM_SS_HYBR_2)
  {
    if (cmss_ptr()->hybr_gw_last_system_reported != SYS_SYS_MODE_NO_SRV)
    {
      call_ptr->sys_mode = cmss_ptr()->hybr_gw_last_system_reported;
    }
  }
  else 
  #endif
  if (cmss_ptr()->last_system_reported != SYS_SYS_MODE_NO_SRV)
  {
    call_ptr->sys_mode = cmss_ptr()->last_system_reported;
  }

  CM_MSG_HIGH_1("call_ptr->sys_mode %d",call_ptr->sys_mode);

  return;

}
/*===========================================================================

FUNCTION cmwcall_pack_subaddress

DESCRIPTION
  Utility function that packs the called party subaddress 
   in streaming formart.

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_pack_subaddress (
     cm_called_party_subaddress_T *mn_orig_subaddress_ptr,
  /* CM subaddress format to be sent to Network*/

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

    mn_orig_subaddress_ptr->data[0] = (uint8)octet;

    /* First byte of destination is used for octet, hence 1 is subtracted from destination size */
    mn_orig_subaddress_ptr->length = ( memscpy( &(mn_orig_subaddress_ptr->data[1]),
                                    (sizeof(mn_orig_subaddress_ptr->data) - 1),
                                    cm_subaddress_struct_ptr->chari,
                                    cm_subaddress_struct_ptr->num_char ) + 1 );
  }
  else
  {
    mn_orig_subaddress_ptr->length = 0;
  }

} /* cmwcall_pack_subaddress() */


/*===========================================================================

FUNCTION cmwcall_update_calls_on_srv_lost

DESCRIPTION
  Update the call object on service lost

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
#if (defined (FEATURE_LTE_TO_1X) && defined (FEATURE_LTE))
void     cmwcall_update_calls_on_srv_lost(

  sd_ss_e_type    ss,

  boolean is_in_rlf
)
#else
void     cmwcall_update_calls_on_srv_lost(

  sd_ss_e_type    ss
)
#endif
{
  cm_iterator_type    call_itr;
    /* Call Object Iterator */

  cmcall_s_type       *call_ptr;
      /* Point at call object */

  cm_ss_e_type       css = cmph_map_sd_ss_type_to_cm_ss_type(ss);

  cmcall_obj_iterator_init(&call_itr);

  call_ptr = cmcall_obj_get_next(&call_itr);

  while(call_ptr != NULL)
  {

    #if (defined (FEATURE_LTE_TO_1X) && defined (FEATURE_LTE))
    if( ( call_ptr->call_state == CM_CALL_STATE_ORIG ) &&
         ((call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS) ||
          (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS) ||
          ((call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_CDMA) &&
           (call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB)))
      )
    #else
    if( ( call_ptr->call_state == CM_CALL_STATE_ORIG ) &&
         ((call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_CS) ||
          (call_ptr->cmcall_mode_info.info_type == CM_CALL_MODE_INFO_GW_PS))
      )
    #endif
    {
      if( call_ptr->ss == css )
      {
        call_ptr->sr.is_ok_to_orig = FALSE;

        #if defined (FEATURE_LTE_TO_1X) && defined (FEATURE_LTE)
        if ((call_ptr->csfb_type == CM_CALL_CSFB_TYPE_1XCSFB) &&
            (is_in_rlf == TRUE))
        {
          call_ptr->is_rlf_in_1xcsfb_call = TRUE;
        }
        #endif

      }
      else
      {
        call_ptr->sr.hdr_is_ok_to_orig = FALSE;
      }
    }

    call_ptr = cmcall_obj_get_next(&call_itr);
  }

  return;
}
#endif

/*===========================================================================
 
 FUNCTION  cmwcall_client_cmd_check
 
 DESCRIPTION
   Check for call command parameter errors and whether a specified call command
   is allowed in the current state of the call/phone.
 
 DEPENDENCIES
   Call object must have already been initialized with
   cmcall_init().
 
 RETURN VALUE
   CM_CALL_CMD_ERR_NOERR if command is allowed in the current state
   of the call/phone, specific cm_call_cmd_err_e_type otherwise.
 
 SIDE EFFECTS
   If originating dial-string matches one of the special numbers that is
   stored in the phone-book, this function changes the call type to E911,
   OTASP, etc.
 
 ===========================================================================*/
 cm_call_cmd_err_e_type  cmwcall_client_cmd_check(
 
     cm_call_cmd_s_type          *call_cmd_ptr
         /* Pointer to a CM command */
 )
 {
 
   cm_call_cmd_info_gw_cs_s_type   *cs_cmd_info_ptr = WCALL_CMD_INFO_PTR( call_cmd_ptr );
       /* Point to GW cs call information */
 
 
   cmph_s_type                 *ph_ptr        = cmph_ptr();
       /* Point at phone object */
 
  #if (!(defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) || defined(FEATURE_UE_INITIATED_DORMANCY))
   cmss_s_type                 *ss_ptr        = cmss_ptr();
     /* Pointer to the serving system object */
 
  #endif /* #if (!defined(FEATURE_PS_DORMANT_PWR_SAVE) || defined(FEATURE_UE_INITIATED_DORMANCY))*/
 
   cmcall_s_type               *call_ptr = NULL;
     /* Pointer to the call object */
 
   cm_call_cmd_err_e_type       cmd_err       = CM_CALL_CMD_ERR_NOERR;
       /* Initialize command error to NO-ERROR */
 
   #if defined(FEATURE_MMODE_GW)
   int                          i;
   #endif
     /* For loop counter */
 
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  #if defined (FEATURE_MULTIMEDIA_FALLBACK)
 
   gsm_wcdma_cmcall_type       *wcall_info_ptr = NULL;
 
   cm_call_cmd_info_s_type     *cmd_info_ptr   = CMD_INFO_PTR( call_cmd_ptr );
 
  #endif
 
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
   CM_ASSERT( call_cmd_ptr != NULL );
   CM_ASSERT( call_cmd_ptr->cmd_type == CM_CMD_TYPE_CALL );
   CM_ASSERT( ph_ptr != NULL );
  #if (!(defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) || defined(FEATURE_UE_INITIATED_DORMANCY))
   CM_ASSERT( ss_ptr != NULL );
  #endif /* #if (!defined(FEATURE_PS_DORMANT_PWR_SAVE) || defined(FEATURE_UE_INITIATED_DORMANCY)) */
 
   CM_MSG_HIGH_1( "cmd check %d",call_cmd_ptr->cmd_type);
 
   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
   /* check cmd is already in error or not */
   if ( call_cmd_ptr->info.call_type != CM_CALL_TYPE_PS_DATA )
   {
     if (call_cmd_ptr->info.cmd_err != CM_CALL_CMD_ERR_NOERR)
     {
       return call_cmd_ptr->info.cmd_err;
     }
   }
 
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
   /* Check whether command is possible.
   */
   switch( call_cmd_ptr->cmd )
   {
                        /*---------------------------*/
                        /* Response to a setup message */
                        /*---------------------------*/
     #if defined(FEATURE_MMODE_GW) 
     #if !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
     case CM_CALL_CMD_SETUP_RES:
 
       /* Only able to end a call if phone is online.
       */
       if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE )
       {
         cmd_err = CM_CALL_CMD_ERR_OFFLINE_S;
         break;
       }
 
       call_ptr = cmcall_ptr ( call_cmd_ptr->info.call_id );
 
       /* Check for call id errors */
       if(call_ptr == NULL)
       {
         cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
         break;
       }
 
       if ( !( (call_ptr->call_state       == CM_CALL_STATE_INCOM ) &&
               (call_ptr->call_subst.incom == CM_CALL_INCOM_SUBST_SETUP )
             )
          )
       {
         /* Command received in incorrect state
         */
         cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
       }
 
      #if defined (FEATURE_MULTIMEDIA_FALLBACK)
 
       /* If BC2 is present in set up response, BC1 also needs to be present
       */
       if (!cmwcall_check_for_BC1_if_BC2_present
                         ( &cs_cmd_info_ptr->bearer_capability_1,
                           &cs_cmd_info_ptr->bearer_capability_2
                         )
          )
       {
         cmd_err = CM_CALL_CMD_ERR_INVALID_BC_P;
       }
 
      #endif
 
       /*lint -restore */
       break;
     
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
                        /*-------------------------*/
                        /* Answer an incoming call */
                        /*-------------------------*/
 
     case CM_CALL_CMD_ANSWER:
 
       /* check command parameter errors
       */
       if((cmd_err = cmcall_call_cmd_answer_para_check(call_cmd_ptr)) != CM_CALL_CMD_ERR_NOERR)
       {
         /* stop further processing if any errors found in the command */
         break;
       }
 
       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
       /* Only able to answer a call if phone is online.
       */
       if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE )
       {
         cmd_err = CM_CALL_CMD_ERR_OFFLINE_S;
         break;
       }
 
       call_ptr = cmcall_ptr ( call_cmd_ptr->info.call_id );
       if(call_ptr == NULL)
       {
         CM_MSG_HIGH_0( "call_ptr_NULL");
         break;
       }
 
       if ( !( (call_ptr->call_state       == CM_CALL_STATE_INCOM ) &&
               (call_ptr->call_subst.incom == CM_CALL_INCOM_SUBST_ALERTING )
             )
          )
       {
         /* Command received in incorrect state
         */
         cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
       }
 
       /*lint -restore */
       break; /* CM_CALL_CMD_ANSWER */
     #endif

       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
                        /*---------------------------*/
                        /* End call/call-origination */
                        /*---------------------------*/
 
     case CM_CALL_CMD_END:
 
       
 
       /* check command parameter errors */
       /* none */
       for ( i = 0; i < call_cmd_ptr->info.num_end_call_ids; i++ )
       {
         /* cmwcall should end calls of info_type GW_CS or GW_PS only
         */
         if (!( (cmcall_call_id_mode (call_cmd_ptr->info.end_params[i].call_id)
                == CM_CALL_MODE_INFO_GW_CS)  ||
                (cmcall_call_id_mode (call_cmd_ptr->info.end_params[i].call_id)
                == CM_CALL_MODE_INFO_GW_PS)
              )
            )
         {
           continue;
         }
 
         /* Only able to end a call if phone is online.
         */
         if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE )
         {
           cmd_err = CM_CALL_CMD_ERR_OFFLINE_S;
           break;
         }
 
         /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
         call_ptr = cmcall_ptr ( call_cmd_ptr->info.end_params[i].call_id );
 
         if (call_ptr == NULL )
         {
           cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
           break;
         }
 
         if ( call_ptr->call_state == CM_CALL_STATE_IDLE &&
              (call_ptr->end_status != CM_CALL_END_NETWORK_END &&\
               call_ptr->end_status != CM_CALL_END_CAUSE_PERM_REDIAL_NOT_NEEDED))
         {
           /* In idle this command makes no sense.
           */
           CM_MSG_HIGH_1( "Rejecting callend reason:%d",call_ptr->end_status );
           cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
           break;
         }
 
         if( ( ( call_ptr->call_type == CM_CALL_TYPE_CS_DATA ) ||
               ( call_ptr->call_type == CM_CALL_TYPE_VOICE ) ||
               ( call_ptr->call_type == CM_CALL_TYPE_EMERGENCY ) ) &&
             ( call_cmd_ptr->info.end_params[i].info_type != CM_CALL_MODE_INFO_GW_CS ) )
         {
           cmd_err = CM_CALL_CMD_ERR_MISMATCH_P;
           break;
         }
         else if( ( call_ptr->call_type == CM_CALL_TYPE_PS_DATA ) &&
                  ( call_cmd_ptr->info.end_params[i].info_type != CM_CALL_MODE_INFO_GW_PS ) )
         {
           cmd_err = CM_CALL_CMD_ERR_MISMATCH_P;
           break;
         }
       }
       break;
 
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
     case CM_CALL_CMD_PS_DATA_AVAILABLE:
 
      #if !(defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
 
       if( ( call_cmd_ptr->client_ptr == CM_CLIENT_PTR_NONE ) ||
           ((call_cmd_ptr->client_ptr)->init_mask != CM_INIT_MASK ) )
       {
         /*  Make sure the client has a valid ID.
         */
         cmd_err  = CM_CALL_CMD_ERR_CLIENT_ID_P;
         CM_MSG_HIGH_0( "CM_CALL_CMD_PS_DATA_AVAIL: ERR_CLIENT_ID_P");
       }
       else if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE )
       {
         /* Reject the command if phone is not online.
         */
         cmd_err = CM_CALL_CMD_ERR_OFFLINE_S;
         CM_MSG_HIGH_0( "CM_CALL_CMD_PS_DATA_AVAIL: ERR_OFFLINE_S");
       }
       else if( ss_ptr->info.srv_domain != SYS_SRV_DOMAIN_PS_ONLY &&
                ss_ptr->info.srv_domain != SYS_SRV_DOMAIN_CS_PS )
       {
         /* Reject the command if the current system supports PS.
         */
         cmd_err = CM_CALL_CMD_ERR_SRV_STATE_S;
         CM_MSG_HIGH_0( "CM_CALL_CMD_PS_DATA_AVAIL: ERR_SRV_STATE_S");
       }
       else if (ss_ptr->info.sys_mode == SYS_SYS_MODE_CDMA &&
                cmcall_is_there_a_call_type(CM_SS_MAIN, CM_CALL_TYPE_VOICE, CM_CALL_ID_INVALID ))
       {
         /* Reject the command if there's 1X call
         */
         cmd_err = CM_CALL_CMD_ERR_IN_USE_S;
         CM_MSG_HIGH_0("CM_CALL_CMD_PS_DATA_AVAIL: CM_CALL_CMD_ERR_IN_USE ");
       }
 
       if (cmph_is_msim())
       {
         sys_modem_as_id_e_type  asubs_id = SYS_MODEM_AS_ID_NONE;
 
         /* Check if there is an existing voice call
         */
         if((cmph_is_dsds())
        #ifdef FEATURE_MMODE_TRIPLE_SIM
         || ph_ptr->device_mode == SYS_MODEM_DEVICE_MODE_TRIPLE_SIM_TRIPLE_STANDBY
        #endif
         )
         {
           asubs_id = cmcall_get_subs_id_with_call_type(CM_CALL_TYPE_VOICE);
         }
 
         if( asubs_id == SYS_MODEM_AS_ID_NONE)
         {
           /* If no voice call, Check if there is an existing emerg call
           */
           asubs_id = cmcall_get_subs_id_with_call_type(CM_CALL_TYPE_EMERGENCY);
         }
 
         /* If a voice call (or) emerg call exist on other subscription, return PS_DATA_FAIL
         */
         if( asubs_id != SYS_MODEM_AS_ID_NONE &&
             asubs_id != call_cmd_ptr->cmd_subs)
         {
           CM_ERR_0("Phone in use, reject PS_DATA_AVAIL");
           cmd_err = CM_CALL_CMD_ERR_IN_USE_S;
         }
       }
      #endif /* !FEATURE_PS_DORMANT_PWR_SAVE */
 
       break;
 
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
     case CM_CALL_CMD_PS_SIG_REL:
     {
       sys_modem_as_id_e_type  asubs_id = cmph_get_as_id_from_ll_ind(call_cmd_ptr->cmd_subs);
 
      #ifdef FEATURE_UE_INITIATED_DORMANCY
       if( ( call_cmd_ptr->client_ptr == CM_CLIENT_PTR_NONE ) ||
           ((call_cmd_ptr->client_ptr)->init_mask != CM_INIT_MASK ) )
       {
         /*  Make sure the client has a valid ID.
         */
         cmd_err  = CM_CALL_CMD_ERR_CLIENT_ID_P;
         CM_MSG_HIGH_0( "CM_CALL_CMD_PS_SIG_REL: ERR_CLIENT_ID_P");
       }
       else if( ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE )
       {
         /* Reject the command if phone is not online.
         */
         cmd_err = CM_CALL_CMD_ERR_OFFLINE_S;
         CM_MSG_HIGH_0( "CM_CALL_CMD_PS_SIG_REL: ERR_OFFLINE_S");
       }
       else if(
        #ifdef FEATURE_MMODE_HYBR3_ENABLED
                (
                 call_cmd_ptr->cmd_subs == ph_ptr->hybr_3_stack_info.asubs_id &&
                  ss_ptr->info.gw3_srv_domain != SYS_SRV_DOMAIN_PS_ONLY &&
                  ss_ptr->info.gw3_srv_domain != SYS_SRV_DOMAIN_CS_PS
                ) ||
       #endif
       #ifdef FEATURE_MMODE_DUAL_SIM
                (!cmph_is_sxlte() && \
                  call_cmd_ptr->cmd_subs == SYS_MODEM_AS_ID_2 &&
                  ss_ptr->info.gw_srv_domain != SYS_SRV_DOMAIN_PS_ONLY &&
                  ss_ptr->info.gw_srv_domain != SYS_SRV_DOMAIN_CS_PS
                ) ||
       #endif
                (call_cmd_ptr->cmd_subs != SYS_MODEM_AS_ID_2 &&
                 call_cmd_ptr->cmd_subs != SYS_MODEM_AS_ID_3 &&
                 ss_ptr->info.srv_domain != SYS_SRV_DOMAIN_PS_ONLY &&
                  ss_ptr->info.srv_domain != SYS_SRV_DOMAIN_CS_PS
                )
              )
       {
         /* Reject the command if the current system does not support PS.
         */
         cmd_err = CM_CALL_CMD_ERR_SRV_STATE_S;
         CM_MSG_HIGH_0( "CM_CALL_CMD_PS_SIG_REL: ERR_SRV_STATE_S");
       }
       #ifdef FEATURE_MMODE_GW
       else if(!cmcall_check_if_ps_call_active_as_id(asubs_id))
       {
         cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
         CM_MSG_HIGH_0( "CM_CALL_CMD_PS_SIG_REL: ERR_CALL_STATE_S");
       }
	  #endif
      #endif
     }
     break;
 
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
     #if defined(FEATURE_MMODE_CALL_SUPS)
     case CM_CALL_CMD_SUPS:
 
       cmd_err = cmwcall_sups_cmd_check(call_cmd_ptr);
 
     break;
     #endif
 
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
 
 
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
    case CM_CALL_CMD_RECALL_RSP:
        
       #if defined(FEATURE_CCBS)
       call_ptr = cmcall_ptr ( cs_cmd_info_ptr->sups_cmd_params.call_id );
 
       if (call_ptr == NULL )
       {
         cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
         CM_ASSERT ( call_ptr != NULL );
         break;
       }
 
       /* Voice and CS_Data cannot both be present at same time
       ** TO-DO.
       */
       /* if( !cmwcall_check_if_mo_call_possible( call_cmd_ptr ) )
       ** {
       **   cmd_err =  CM_CALL_CMD_ERR_IN_USE_S;
       **
       **}
       */
 
       /* RECALL_RSP_PEND is the only state when recall response
       ** can be given to CM
       */
       if ( call_ptr->call_state != CM_CALL_STATE_RECALL_RSP_PEND )
       {
         /* Command should be received in right state
         */
         cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
       }
       #endif
 
     break;
 
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     #if !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
     case CM_CALL_CMD_MO_MODIFY_REQ:
 
      #if defined (FEATURE_MULTIMEDIA_FALLBACK)
 
       call_ptr = cmcall_ptr ( cmd_info_ptr->call_id );
 
 
       if (call_ptr == NULL )
       {
         cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
 
         CM_ASSERT ( call_ptr != NULL );
 
         break;
       }
 
       wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;
 
       /* Modification can be requested only for
       **
       ** 1> Call type voice and CS_DATA
       **
       ** 2> Call should be in conversation.
       **
       */
       if ( call_ptr->call_state != CM_CALL_STATE_CONV )
       {
         /* Command should be received in right state
         */
         cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
       }
 
       /* Call modification is possible on only Voice and CS_DATA calls
       */
       if ( !((call_ptr->call_type == CM_CALL_TYPE_VOICE) ||
              (call_ptr->call_type == CM_CALL_TYPE_CS_DATA)
             )
          )
       {
         cmd_err = CM_CALL_CMD_ERR_CALL_TYPE_S;
       }
 
       /* Call can be modified to only Voice or CS_DATA
       */
       if ( !((cs_cmd_info_ptr->new_call_type_req == CM_CALL_TYPE_VOICE) ||
              (cs_cmd_info_ptr->new_call_type_req == CM_CALL_TYPE_CS_DATA)
             )
          )
       {
         cmd_err = CM_CALL_CMD_ERR_CALL_TYPE_P;
       }
 
       /* For call to be modified to a particular type, bearer capability
       ** for that type should have been negotiated at call set up and
       ** this should be present as the 2nd BC.
       */
       if ((!wcall_info_ptr->bearer_capability_2.present)
            ||
           ( !cmwcall_is_call_type_partof_bearer
               (cs_cmd_info_ptr->new_call_type_req,
               &wcall_info_ptr->bearer_capability_2)
           )
          )
       {
         cmd_err = CM_CALL_CMD_ERR_CALL_TYPE_P;
       }
 
      #endif
 
       break;
 
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
     case CM_CALL_CMD_MODIFY_RES:
 
      #if defined (FEATURE_MULTIMEDIA_FALLBACK)
 
       call_ptr = cmcall_ptr ( cmd_info_ptr->call_id );
 
       if (call_ptr == NULL )
       {
         cmd_err = CM_CALL_CMD_ERR_CALL_ID_P;
 
         CM_ASSERT ( call_ptr != NULL );
 
         break;
       }
 
       /* Modification response can be sent only for call in conversation.
       */
       if ( call_ptr->call_state != CM_CALL_STATE_CONV )
       {
         /* Command should be received in right state
         */
         cmd_err = CM_CALL_CMD_ERR_CALL_STATE_S;
       }
 
      #endif
 
       break;
    #endif
    #endif
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
     default:
 
       cmd_err = CM_CALL_CMD_ERR_OTHER;
       CM_ERR_1( "Bad call command %d", (uint8) call_cmd_ptr->cmd);
 
   }
 
   return cmd_err;
 }

 
 /*===========================================================================
 
 FUNCTION cmwcall_client_cmd_forward_ll
 
 DESCRIPTION
   Forward a client call commands to Lower Layer.
 
 DEPENDENCIES
   Call object must have already been initialized with
   cmcall_init().
 
 RETURN VALUE
   TRUE if CM needs to wait for a reply from MN before continuing
   processing this command, FALSE otherwise.
 
 SIDE EFFECTS
   none
 
 ===========================================================================*/
 static boolean cmwcall_client_cmd_forward_ll(
 
     cm_call_cmd_s_type    *call_cmd_ptr
         /* Pointer to a CM command */
 )
 /*lint -esym(818, cmd_ptr) */
 {
 
   const cm_call_cmd_info_s_type  *cmd_info_ptr  = CMD_INFO_PTR( call_cmd_ptr );
       /* Pointer to call cmd information */
 
   const cm_call_cmd_info_gw_cs_s_type  *cs_cmd_info_ptr = WCALL_CMD_INFO_PTR( call_cmd_ptr );
   const cm_call_cmd_info_gw_ps_s_type  *ps_cmd_info_ptr = WCALL_PS_CMD_INFO_PTR( call_cmd_ptr );
 
  
   #if defined(FEATURE_MMODE_GW)
   int                                  i;
 
          /* Point at command information component */
 
   mn_cnm_cmd_type                      *mn_ptr;
       /* MN command pointer */

   byte                                 connection_id;

   #if !(defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
   mm_cmd_type                          *mm_ptr;
        /* MM command pointer */
   #endif /* !FEATURE_PS_DORMANT_PWR_SAVE */

   cmcall_s_type                        *call_ptr;
         /* Pointer to a call object */

   #endif


  #if defined (FEATURE_MULTIMEDIA_FALLBACK)
   gsm_wcdma_cmcall_type               *wcall_info_ptr = NULL;
     /* Pointer to GW related info */
  #endif
 
  #ifdef FEATURE_MMODE_DUAL_SIM
   cm_srv_type_e_type      srv_type;
   cm_mode_pref_e_type     fav_mode_pref;
  #endif //FEATURE_MMODE_DUAL_SIM
 
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
   CM_ASSERT( call_cmd_ptr != NULL );
   CM_ASSERT( call_cmd_ptr->cmd_type == CM_CMD_TYPE_CALL );
 
   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
   switch( call_cmd_ptr->cmd )
   {
                      /*---------------------------*/
                      /* Response to a setup message*/
                      /*---------------------------*/
     #if defined(FEATURE_MMODE_GW) 
     #if !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
     case CM_CALL_CMD_SETUP_RES:
 
       /*
       ** Send the CM_MT_SETUP_RES command now.
       */
       cmwcall_process_mt_setup_response
         (
           cmcall_ptr (call_cmd_ptr->info.call_id),
           cs_cmd_info_ptr->accept,
           &cs_cmd_info_ptr->cc_cause,
           &cs_cmd_info_ptr->bc_repeat_indicator,
           &cs_cmd_info_ptr->bearer_capability_1,
           &cs_cmd_info_ptr->bearer_capability_2
         );
 
       break;
       
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
                      /*-------------------------*/
                      /* Answer an incoming call */
                      /*-------------------------*/
 
     case CM_CALL_CMD_ANSWER:
 
       if (!cm_find_connection_id (call_cmd_ptr->info.call_id,
                      &connection_id))
       {
         CM_ERR_0 ("err in getting connection id");
         break;
       }
 
       if ( (call_ptr = cmcall_ptr ( call_cmd_ptr->info.call_id )) == NULL)
       {
         CM_ERR_0 ("err in getting call id");
         break;
       }
 
       #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
       {
         /* handling CALL_CMD_ANSWER for non PS calls */
         mn_ptr = cm_mn_get_buf_else_err_fatal(CM_MT_CALL_RES);
 
         mn_ptr->cmd.cm_mt_call_res.connection_id = connection_id;
         mn_ptr->cmd.cm_mt_call_res.cm_accept     = cs_cmd_info_ptr->accept;
         mn_ptr->cmd.cm_mt_call_res.cm_cc_cause.present     = cs_cmd_info_ptr->cc_cause.present;
         mn_ptr->cmd.cm_mt_call_res.cm_cc_cause.cause_value = cs_cmd_info_ptr->cc_cause.cause_value;
         mn_ptr->cmd.cm_mt_call_res.as_id         = call_ptr->asubs_id;
 
         cm_mn_send_cmd( mn_ptr );
       }
       #endif
	   
       break;
     #endif
 
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
                      /*---------------------------*/
                      /* End call/call-origination */
                      /*---------------------------*/
 
     case CM_CALL_CMD_END:
 
       if( call_cmd_ptr->info.num_end_call_ids == 1 )
       {
         /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
         /* cmwcall should end calls of info_type GW_CS or GW_PS only
         */
         if (!( (cmcall_call_id_mode (call_cmd_ptr->info.end_params[0].call_id)
                == CM_CALL_MODE_INFO_GW_CS)  ||
                (cmcall_call_id_mode (call_cmd_ptr->info.end_params[0].call_id)
                == CM_CALL_MODE_INFO_GW_PS)
              )
            )
         {
           break;
         }
 
         /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
 
         if (!cm_find_connection_id( call_cmd_ptr->info.end_params[0].call_id,
                                     &connection_id ))
         {
           CM_ERR_0( "Could not find connection id.");
         }
 
         /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
         call_ptr = cmcall_ptr( call_cmd_ptr->info.end_params[0].call_id);
 
         if (call_ptr == NULL)
         {
           sys_err_fatal_null_ptr_exception();
         }
 
         /* If the origination has not been sent to the lower layer,
         ** do not send a call end request to the lower layer.
         */
         if ((call_ptr->call_state == CM_CALL_STATE_ORIG) &&
             (call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE))
         {
           break;
         }
 
         switch ( call_ptr->call_type )
         {
         #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
         case CM_CALL_TYPE_VOICE:
         case CM_CALL_TYPE_CS_DATA:
         case CM_CALL_TYPE_EMERGENCY:
 
           mn_ptr = cm_mn_get_buf_else_err_fatal(CM_MO_END_X_CALL_REQ);
 
           mn_ptr->cmd.cm_n_x_send_req.connection_id   = connection_id;
           mn_ptr->cmd.cm_n_x_send_req.cause_value     = NORMAL_CALL_CLEARING;
           mn_ptr->cmd.cm_n_x_send_req.as_id           = call_ptr->asubs_id;
 
           cm_mn_send_cmd( mn_ptr );
           break;
         #endif

         default:
           CM_ERR_1("Bad call type! %d",call_ptr->call_type);
           break;
         }
       }
       else
       {
         for ( i = 0; i < call_cmd_ptr->info.num_end_call_ids; i++ )
         {
           /* cmwcall should end calls of info_type GW_CS or GW_PS only
           */
           if (!((cmcall_call_id_mode (call_cmd_ptr->info.end_params[i].call_id)
                   == CM_CALL_MODE_INFO_GW_CS)  ||
                 (cmcall_call_id_mode (call_cmd_ptr->info.end_params[i].call_id)
                   == CM_CALL_MODE_INFO_GW_PS)
                )
              )
           {
             continue;
           }
 
           /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
           if (!cm_find_connection_id( call_cmd_ptr->info.end_params[i].call_id,
                                      &connection_id ))
           {
             CM_ERR_0( "Could not find connection id.");
           }
 
           call_ptr = cmcall_ptr( call_cmd_ptr->info.end_params[i].call_id);
 
           if (call_ptr == NULL)
           {
             CM_ERR_1("Invalid call_ptr from call_id", call_cmd_ptr->info.end_params[i].call_id);
             continue;
           }
 
           /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
           switch ( call_ptr->call_type )
           {
             #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
             case CM_CALL_TYPE_VOICE:
             case CM_CALL_TYPE_CS_DATA:
             case CM_CALL_TYPE_EMERGENCY:
               cmwcall_send_cs_end_req (connection_id, call_ptr->ss);
               break;
             #endif

             default:
 
               CM_ERR_1("Bad call type! %d",call_ptr->call_type);
               break;
           } /* switch call_type ... */
 
         } /* for i < num_end_call_ids ... */
 
       } /* else num_end_call_ids > 1 ... */
 
       break;
 
 
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
 
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #if !(defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
                      /*------------------------------------------- */
                      /* PS Data Available Request to MM            */
                      /*--------------------------------------------*/
 
     case CM_CALL_CMD_PS_DATA_AVAILABLE:
 
       /* check whether the protocol stack is up
          or not before sending this command
       */
       mm_ptr = mm_cm_get_cmd_buf(MMCM_PS_DATA_AVAILABLE_REQ);
       if (mm_ptr == NULL)
       {
         CM_ERR_0("CM_CALL_CMD_PS_DATA_AVAIL: out of MM cmd buffer");
       }
       else
       {
         mm_put_cmd( mm_ptr );
       }
       break;
    #endif /* FEATURE_PS_DORMANT_PWR_SAVE */
#if defined(FEATURE_MMODE_CALL_SUPS)  
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
     case CM_CALL_CMD_SUPS:

       switch (cmd_info_ptr->sups_cmd_type)
       {
         case CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB:
           mn_ptr = cm_mn_get_buf_else_err_fatal(CM_0_SEND_REQ);
 
           /* Set the subscription id associated to this cmd
           */
           mn_ptr->cmd.cm_n_send_req.as_id = call_cmd_ptr->cmd_subs;
           cmcall_supscmd_update_end_status(CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB, CM_CALL_ID_NONE);
           cm_mn_send_cmd( mn_ptr );
           break;
 
         case CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL:
           mn_ptr = cm_mn_get_buf_else_err_fatal(CM_1_SEND_REQ);
 
           /* Set the subscription id associated to this cmd
           */
           mn_ptr->cmd.cm_n_send_req.as_id = call_cmd_ptr->cmd_subs;
           cmcall_supscmd_update_end_status(CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL, CM_CALL_ID_NONE);
           cm_mn_send_cmd( mn_ptr );
           break;
 
         case CM_CALL_SUPS_TYPE_HOLD_CALL:
           mn_ptr = cm_mn_get_buf_else_err_fatal(CM_2_SEND_REQ);
           mn_ptr->cmd.cm_n_send_req.as_id = call_cmd_ptr->cmd_subs;
           cm_mn_send_cmd( mn_ptr );
           break;
 
         case CM_CALL_SUPS_TYPE_MULTIPARTY_CALL:
           mn_ptr = cm_mn_get_buf_else_err_fatal(CM_3_SEND_REQ);
           mn_ptr->cmd.cm_n_send_req.as_id = call_cmd_ptr->cmd_subs;
           cm_mn_send_cmd( mn_ptr );
           break;
 
         case CM_CALL_SUPS_TYPE_EXPLICIT_CALL_TRANSFER:
           mn_ptr = cm_mn_get_buf_else_err_fatal(CM_4_SEND_REQ);
           mn_ptr->cmd.cm_n_send_req.as_id = call_cmd_ptr->cmd_subs;
           cm_mn_send_cmd( mn_ptr );
           break;
 
         case CM_CALL_SUPS_TYPE_ACT_CCBS_REQ:
 
          #if defined(FEATURE_CCBS)
           mn_ptr = cm_mn_get_buf_else_err_fatal(CM_5_SEND_REQ);
 
           if (!cm_find_connection_id(call_cmd_ptr->info.call_id,
                                    &mn_ptr->cmd.cm_5_send_req.connection_id))
           {
             CM_ERR_0( "Could not find connection id.");
           }
 
           mn_ptr->cmd.cm_5_send_req.as_id = call_cmd_ptr->cmd_subs;
           cm_mn_send_cmd( mn_ptr );
          #endif
           break;
 
         case CM_CALL_SUPS_TYPE_ACTIVE_CALL_X_REL:
           mn_ptr = cm_mn_get_buf_else_err_fatal(CM_1_X_SEND_REQ);
           mn_ptr->cmd.cm_n_x_send_req.cause_value = call_cmd_ptr->info.cm_gw_mode_info_u.gsm_wcdma_info.sups_cmd_params.cause;
 
           if (!cm_find_connection_id(call_cmd_ptr->info.cm_gw_mode_info_u.gsm_wcdma_info.sups_cmd_params.call_id,
                                      &mn_ptr->cmd.cm_n_x_send_req.connection_id))
           {
             CM_ERR_0( "Could not find connection id.");
           }
           mn_ptr->cmd.cm_n_x_send_req.as_id = call_cmd_ptr->cmd_subs;
           cmcall_supscmd_update_end_status(CM_CALL_SUPS_TYPE_ACTIVE_CALL_X_REL, call_cmd_ptr->info.cm_gw_mode_info_u.gsm_wcdma_info.sups_cmd_params.call_id);
           cm_mn_send_cmd( mn_ptr );
           break;
 
         case CM_CALL_SUPS_TYPE_SELECT_CALL_X:
 
           mn_ptr = cm_mn_get_buf_else_err_fatal(CM_2_X_SEND_REQ);
           mn_ptr->cmd.cm_n_x_send_req.cause_value = call_cmd_ptr->info.cm_gw_mode_info_u.gsm_wcdma_info.sups_cmd_params.cause;
 
           if (!cm_find_connection_id( call_cmd_ptr->info.cm_gw_mode_info_u.gsm_wcdma_info.sups_cmd_params.call_id,
                                       &mn_ptr->cmd.cm_n_x_send_req.connection_id))
           {
             CM_ERR_0( "Could not find connection id.");
           }
 
           mn_ptr->cmd.cm_n_x_send_req.as_id = call_cmd_ptr->cmd_subs;
           cm_mn_send_cmd( mn_ptr );
           break;
 
        case CM_CALL_SUPS_TYPE_CALL_DEFLECTION:
 
           mn_ptr = cm_mn_get_buf_else_err_fatal(CM_4_X_SEND_REQ);
 
           if (!cm_find_connection_id( call_cmd_ptr->info.cm_gw_mode_info_u.gsm_wcdma_info.sups_cmd_params.call_id,
                                       &mn_ptr->cmd.cm_4_x_send_req.connection_id))
           {
             CM_ERR_0( "Could not find connection id.");
           }
 
           mn_ptr->cmd.cm_4_x_send_req.deflectedToNumber =
                             call_cmd_ptr->info.cm_gw_mode_info_u.gsm_wcdma_info.called_party_bcd_number;
 
           mn_ptr->cmd.cm_4_x_send_req.deflectedToSubAddress =
                             call_cmd_ptr->info.cm_gw_mode_info_u.gsm_wcdma_info.called_party_subaddress;
 
           mn_ptr->cmd.cm_4_x_send_req.as_id = call_cmd_ptr->cmd_subs;
           cm_mn_send_cmd( mn_ptr );
           break;
 
         default:
           CM_MSG_HIGH_1( "Unknown command type received: %d",
                          cmd_info_ptr->sups_cmd_type );
           break;
       }
      #endif
       break;
 
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
     case CM_CALL_CMD_RECALL_RSP:
 
      #if defined(FEATURE_CCBS)
 
       /* Responses other than REJECT get sent to MN in cmwcall_send_orig()
       */
       if (cs_cmd_info_ptr->ccbs_recall_rsp != CM_RECALL_RSP_CCBS_REJECT)
       {
           break;
       }
 
       CM_ASSERT (cs_cmd_info_ptr->ccbs_recall_rsp == CM_RECALL_RSP_CCBS_REJECT);
 
       mn_ptr = cm_mn_get_buf_else_err_fatal(CM_RECALL_RSP);
 
       if (!cm_find_connection_id( cs_cmd_info_ptr->sups_cmd_params.call_id,
                                   &mn_ptr->cmd.cm_recall_rsp.connection_id))
       {
         CM_ERR_0( "Could not find connection id.");
       }
 
       mn_ptr->cmd.cm_recall_rsp.response =
               cmcall_map_recall_rsp_cm_to_mn (cs_cmd_info_ptr->ccbs_recall_rsp);
 
       call_ptr = cmcall_ptr(cs_cmd_info_ptr->sups_cmd_params.call_id);
 
       if (call_ptr == NULL)
       {
         sys_err_fatal_null_ptr_exception();
       }
 
       mn_ptr->cmd.cm_recall_rsp.as_id = call_ptr->asubs_id;
 
       CM_MSG_HIGH_1 ("CM -> MN Recall rsp = %d",
                                   mn_ptr->cmd.cm_recall_rsp.response);
 
       cm_mn_send_cmd( mn_ptr );
      #endif
 
       break;
 
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
    #ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
     #error code not present
#endif
 
 
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #if !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
     case CM_CALL_CMD_MO_MODIFY_REQ:
 
       call_ptr = cmcall_ptr(call_cmd_ptr->info.call_id);
 
       if (call_ptr == NULL)
       {
         sys_err_fatal_null_ptr_exception();
       }
 
      #if defined (FEATURE_MULTIMEDIA_FALLBACK)
 
       wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;
 
       mn_ptr = cm_mn_get_buf_else_err_fatal(CM_MO_MODIFY_REQ);
 
       if (!cm_find_connection_id( call_cmd_ptr->info.call_id,
                                   &mn_ptr->cmd.cm_mo_modify_req.connection_id))
       {
         CM_ERR_0( "Could not find connection id.");
       }
 
       /* Copy BC, LLC and HLC for modification
       **
       ** Current call type is based on values in BC1.
       ** New call type requested to modify the call should be based on
       ** BC2.
       */
       mn_ptr->cmd.cm_mo_modify_req.bearer_capability        =
                                   wcall_info_ptr->bearer_capability_2;
 
       mn_ptr->cmd.cm_mo_modify_req.high_layer_compatibility =
                                   wcall_info_ptr->high_layer_compatibility_2;
 
       mn_ptr->cmd.cm_mo_modify_req.low_layer_compatibility  =
                                   wcall_info_ptr->low_layer_compatibility_2;
 
       /* Reverse call setup direction is set to TRUE when there is modify
       ** request being made for MT CS_DATA call.
       */
       if ( (call_ptr->call_type == CM_CALL_TYPE_VOICE) &&
            (call_ptr->direction == CM_CALL_DIRECTION_MT) &&
            (cs_cmd_info_ptr->new_call_type_req == CM_CALL_TYPE_CS_DATA)
          )
       {
         mn_ptr->cmd.cm_mo_modify_req.reverse_call_setup_direction = TRUE;
       }
       else
       {
         mn_ptr->cmd.cm_mo_modify_req.reverse_call_setup_direction = FALSE;
       }
 
       mn_ptr->cmd.cm_mo_modify_req.as_id = call_ptr->asubs_id;
 
       CM_MSG_HIGH_1 ("Call Mod req sent to MN for call id %d",
                     call_cmd_ptr->info.call_id);
 
       cm_mn_send_cmd( mn_ptr );
 
      #endif
 
       break;
 
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
                        /*---------------------------*/
                        /* MT call modify response   */
                        /*---------------------------*/
 
     case CM_CALL_CMD_MODIFY_RES:
 
      #if defined (FEATURE_MULTIMEDIA_FALLBACK)
 
       mn_ptr = cm_mn_get_buf_else_err_fatal(CM_MT_MODIFY_RES);
 
       if (!cm_find_connection_id( call_cmd_ptr->info.call_id,
                                   &mn_ptr->cmd.cm_mt_modify_res.connection_id)
          )
       {
         CM_ERR_0( "Could not find connection id.");
       }
 
       call_ptr = cmcall_ptr(call_cmd_ptr->info.call_id);
 
       if (call_ptr == NULL)
       {
         sys_err_fatal_null_ptr_exception();
         break;
       }
 
       /* Response to network initiated in-call modification.
       */
       mn_ptr->cmd.cm_mt_modify_res.cm_accept   =
                                         cs_cmd_info_ptr->gw_cs_modify_res;
 
       mn_ptr->cmd.cm_mt_modify_res.cm_cc_cause =
                                         cs_cmd_info_ptr->cc_cause;
 
       if (cs_cmd_info_ptr->gw_cs_modify_res == FALSE)
       {
         /* in-call modification is rejected. reset the new_call_type */
         call_ptr->cmcall_mode_info.info.gsm_wcdma_info.new_call_type_req = CM_CALL_TYPE_NONE;
       }
 
       mn_ptr->cmd.cm_mt_modify_res.as_id = call_ptr->asubs_id;
 
       CM_MSG_HIGH_2 ("Modification resp for call %d is %d",
                     call_cmd_ptr->info.call_id,
                     cs_cmd_info_ptr->gw_cs_modify_res);
 
       cm_mn_send_cmd( mn_ptr );
 
      #endif
 
       break;
    #endif
    #endif 
     default:
 
       CM_ERR_1( "call cmd %d", (uint8) call_cmd_ptr->cmd);
 
 
   } /* switch ( call_cmd_ptr->cmd) */
 
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   return FALSE;
 }

 /*lint +esym(818, cmd_ptr) */
 
 /*===========================================================================
 
 FUNCTION cmwcall_client_cmd_ll_reply
 
 DESCRIPTION
   Process Lower Layer reply to clients call commands.
 
 DEPENDENCIES
   Call object must have already been initialized with
   cmcall_init().
 
 RETURN VALUE
   none
 
 SIDE EFFECTS
   none
 
 ===========================================================================*/
 static void cmwcall_client_cmd_ll_reply(
 
     const cm_call_cmd_s_type       *call_cmd_ptr,
         /* Pointer to a CM command */
 
     cm_call_cmd_err_e_type    cmd_err
         /* Indicate call command error */
 )
 {
   #if defined(FEATURE_MMODE_GW)
   cmcall_s_type               *call_ptr     = NULL;
         /* Pointer to a call object */
 
   /* Under feature to make lint happy */
  #if defined (FEATURE_MULTIMEDIA_FALLBACK)
   const cm_call_cmd_info_gw_cs_s_type  *cmd_info_ptr = WCALL_CMD_INFO_PTR( call_cmd_ptr );
       /* Point to GW cs call information */
 
   gsm_wcdma_cmcall_type                *wcall_info_ptr = NULL;
     /* Pointer to GW related info */
 
  #endif
 
   int i;
 
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
   CM_ASSERT( call_cmd_ptr != NULL );
   CM_ASSERT( BETWEEN( cmd_err, CM_CALL_CMD_ERR_NONE, CM_CALL_CMD_ERR_MAX));
   CM_ASSERT( call_cmd_ptr->cmd_type == CM_CMD_TYPE_CALL );
 
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
   CM_MSG_LOW_2("START cmwcall_client_cmd_ll_reply, cmd=%d, err=%d",
                   call_cmd_ptr->cmd, cmd_err);
 
   /*
   ** 1. Update call fields
   ** 2. Notify clients of call event
   */
 
   if( ( call_cmd_ptr->cmd != CM_CALL_CMD_SUPS) &&
       ( call_cmd_ptr->cmd != CM_CALL_CMD_END)
      #ifdef FEATURE_UE_INITIATED_DORMANCY
                                                &&
       ( call_cmd_ptr->cmd != CM_CALL_CMD_PS_SIG_REL)
      #endif
      #if !(defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
                                                &&
       (call_cmd_ptr->cmd != CM_CALL_CMD_PS_DATA_AVAILABLE)
      #endif /* ! FEATURE_PS_DORMANT_PWR_SAVE */
     )
   {
     if ((call_ptr = cmcall_ptr ( call_cmd_ptr->info.call_id)) == NULL && call_cmd_ptr->cmd !=CM_CALL_CMD_RESTORE_CTXT_RSP )
     {
       sys_err_fatal_null_ptr_exception();
 
        /*
        ** 527: Unreachable
        ** This is because for some targets _ERR_FATAL may evaluate with sth
        ** with exit(1) in it but on others error fatals may be turned off
        */
        /*lint -save -e527 */
        return;
        /*lint -restore */
     }
 
 
   }
 
   if( cmd_err == CM_CALL_CMD_ERR_NOERR )
   {
 
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
     /* If necessary, copy specific command fields into call struct,
     ** update call state, and notify clients of event.
     */
     switch( call_cmd_ptr->cmd )
     {
       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /*---------------------------*/
        /* Setup Response */
        /*---------------------------*/

       #if !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
       case CM_CALL_CMD_SETUP_RES:
 
         if (!call_ptr)
         {
           CM_ASSERT (call_ptr != NULL);
 
           break;
         }
 
         cmcall_cmd_copy_fields( call_ptr, call_cmd_ptr );
 
        #if defined (FEATURE_MULTIMEDIA_FALLBACK)
 
         /* Pointer to GW cs call info
         */
         wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;
 
         /* Setup response can result in the following
         **
         ** - Client can invalidate one set of BC, HLC and LLC values
         ** - Client can swap values between BC1, HLC1, LLC1 and
         **   BC2, HLC2, LLC2.
         **
         **  Need to copy both pairs from call cmd
         */
         cmwcall_copy_bc_hlc_llc_from_cmd (call_ptr, cmd_info_ptr);
 
         /* Call type is always determined by BC1
         ** Clients have the capability to respond to a voice call
         ** as VT and other way round. Call type can get changed on
         ** receiving set up response.
         */
         if ( !cmwcall_is_call_type_partof_bearer (call_ptr->call_type,
               &wcall_info_ptr->bearer_capability_1)
            )
         {
           call_ptr->call_type = cmwcall_determine_call_type
                             (call_ptr, &wcall_info_ptr->bearer_capability_1);
 
 
           CM_MSG_HIGH_1 ("Setup response changed call type to %d",
                         call_ptr->call_type);
 
         }
 
         /* Set in-call modification field. DS can invalidate Multimedia
         ** BC in which case a call would be no more modifiable.
         */
         cmwcall_set_incall_modif_field (call_ptr);
 
        #endif
 
         /* if setup was rejected, then send end to clients else no event is send*/
         if (call_cmd_ptr->info.end_status == CM_CALL_END_SETUP_REJ){
 
           /* send the end request event to the clients, wait for the
              final end indication from the lower layers
           */
           cmwcall_end_req( call_ptr );
         }
 
         cmcall_event (call_ptr, CM_CALL_EVENT_SETUP_RES);
 
         break;
       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
                        /*-------------------------*/
                        /* Answer an incoming call */
                        /*-------------------------*/
 
       case CM_CALL_CMD_ANSWER:
 
         if (call_ptr != NULL)
         {
 
           /* Update call fields by copying relevant
           ** command fields into call struct.
           */
           cmcall_cmd_copy_fields( call_ptr, call_cmd_ptr );
 
           /* Change incom substate to answer processed. Required for ps
           ** calls to identify the state before and after answer is sent
           */
           call_ptr->call_subst.incom =  CM_CALL_INCOM_SUBST_ANSWERED;
 
           call_ptr->call_client_ptr  = call_cmd_ptr->client_ptr;
 
           if (call_cmd_ptr->info.end_status == CM_CALL_END_INCOM_REJ)
           {
 
             if ( (call_ptr->call_type == CM_CALL_TYPE_PS_DATA) &&
                  (call_ptr->call_state != CM_CALL_STATE_INCOM)
                )
             {
               CM_ERR_0 (" Answering a non-MT PS Call");
             }
 
             /* send the end req to the clients, wait for the final end
             ** indication from the lower layers
             */
             cmwcall_end_req( call_ptr );
 
           }
           else
           {
             cmcall_event( call_ptr, CM_CALL_EVENT_ANSWER );
           }
         }
         else
         {
           CM_ERR_0 ("call_ptr is NULL \n");
         }
 
         break;
       #endif
 
 
       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #if !(defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
                      /*---------------------------*/
                      /* PS Data Available Request */
                      /*---------------------------*/
 
       case CM_CALL_CMD_PS_DATA_AVAILABLE:
       {
         cm_mm_call_info_s_type      *call_info_ptr;
          /* call information pointer */
 
         /* get a buffer for call info
         */
         call_info_ptr = cmcall_info_alloc();
 
         /* Copy call state information into
         ** allocated call info buffer.
         */
         call_info_ptr->call_client_id = cmclient_get_client_id( call_cmd_ptr->client_ptr);
 
         /* notify  the client list of the command received
         */
         cmclient_list_call_event_ntfy( CM_CALL_EVENT_PS_DATA_AVAILABLE, call_info_ptr );
 
         /* Deallocate the memory after sending the call event. */
         cm_mem_free(call_info_ptr);
       }
         break;
       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #endif /* ! FEATURE_PS_DORMANT_PWR_SAVE */
 
       case CM_CALL_CMD_PS_SIG_REL:
      #ifdef FEATURE_UE_INITIATED_DORMANCY
       {
         cm_mm_call_info_s_type      *call_info_ptr;
          /* call information pointer */
 
         /* get a buffer for call info
         */
         call_info_ptr = cmcall_info_alloc();
 
         /* Copy call state information into
         ** allocated call info buffer.
         */
         call_info_ptr->call_client_id = cmclient_get_client_id( call_cmd_ptr->client_ptr);
 
         /* notify  the client list of the command received
         */
         cmclient_list_call_event_ntfy( CM_CALL_EVENT_PS_SIG_REL_REQ, call_info_ptr );
 
         /* Deallocate the memory after sending the call event. */
         cm_mem_free(call_info_ptr);
 
       }
      #endif
         break;
 
 
                        /*---------------------------*/
                        /* End call/call-origination */
                        /*---------------------------*/
 
       case CM_CALL_CMD_END:
 
 
         for ( i=0; i < call_cmd_ptr->info.num_end_call_ids; i++ )
         {
 
           /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
           /* cmwcall should end calls of info_type GW_CS or GW_PS only
           */
           if (!( (cmcall_call_id_mode (call_cmd_ptr->info.end_params[i].call_id)
                == CM_CALL_MODE_INFO_GW_CS)  ||
 
                (cmcall_call_id_mode (call_cmd_ptr->info.end_params[i].call_id)
                == CM_CALL_MODE_INFO_GW_PS)
                )
              )
           {
             continue;
           }
 
           /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
           call_ptr = cmcall_ptr(call_cmd_ptr->info.end_params[i].call_id);
 
           if (call_ptr != NULL)
           {
             cmcall_cmd_copy_fields( call_ptr, call_cmd_ptr );
 
             call_ptr->end_status = CM_CALL_END_CLIENT_END;
 
             *(&(call_ptr->end_params)) = *(&(call_cmd_ptr->info.end_params[i]));
 
             /* If a MT PS Call in CM_CALL_STATE_INCOM is rejected, CM
             ** calls cmcall_end and sends a PDP_ACTIVATE_REJ_RSP to SM.
             ** On voice side CM needs to send a call end request to
             ** MN and get back a explicit CM_MT_CALL_END_IND before
             ** calling cmcall_end
             */
             if( (call_ptr->sr.orig_sent_on_mode == SYS_SYS_MODE_NONE) &&
                  (call_ptr->call_state == CM_CALL_STATE_ORIG))
             {
               cmwcall_end_req( call_ptr );   /* send END_REQ event */
              #if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
               cmcall_end( call_ptr );  /* No origination pending, just end */
              #endif
             }
             else
             {
               cmwcall_end_req( call_ptr );   /* send END_REQ event */
             }
           }
         }
         break;
 
       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
       #if defined(FEATURE_MMODE_CALL_SUPS)
       case CM_CALL_CMD_SUPS:
 
           call_ptr = cmcall_ptr( call_cmd_ptr->info.cm_gw_mode_info_u.gsm_wcdma_info.sups_cmd_params.call_id );
           if(call_ptr == NULL)
           {
             CM_MSG_HIGH_0( "call_ptr_NULL");
             break;
           }
 
           /* Update call fields by copying relevant
           ** command fields into call struct.
           */
           cmcall_cmd_copy_fields( call_ptr, call_cmd_ptr );
 
           /* Send CM_CALL_EVENT_SUPS to clients
           */
           cmcall_event( call_ptr, CM_CALL_EVENT_SUPS );
           break;
         #endif
 
           /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
 
 
      #ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
       #error code not present
#endif
 
       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
                        /*---------------------------*/
                        /* Recall response           */
                        /*---------------------------*/
 
       case CM_CALL_CMD_RECALL_RSP:
 
        #if defined(FEATURE_CCBS)
         if (!call_ptr)
         {
           break;
         }
 
         cmwcall_cmd_copy_fields( call_ptr, call_cmd_ptr );
 
         if (call_cmd_ptr->info.cm_gw_mode_info_u.gsm_wcdma_info.ccbs_recall_rsp
             != CM_RECALL_RSP_CCBS_REJECT)
         {
 
           /* Set a flag to indicate that this is a CCBS recall rsp
           */
           call_ptr->cmcall_mode_info.info.gsm_wcdma_info.is_ccbs_recall_resp
                         = TRUE;
 
           /* Calling generic origination function to handle the recall
           ** response
           */
          #if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
           cmcall_orig_start (call_ptr);
          #endif
         }
         else
         {
           cmcall_event (call_ptr, CM_CALL_EVENT_CCBS_RECALL_RSP);
          #if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
           cmcall_end (call_ptr);
          #endif
         }
        #endif
 
         break;
 
       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
                        /*---------------------------*/
                        /* MO call modify request    */
                        /*---------------------------*/
      #if !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
       case CM_CALL_CMD_MO_MODIFY_REQ:
 
        #if defined (FEATURE_MULTIMEDIA_FALLBACK)
 
         /* Update call fields by copying relevant
         ** command fields into call struct.
         */
         cmcall_cmd_copy_fields( call_ptr, call_cmd_ptr );
 
         cmcall_event (call_ptr, CM_CALL_EVENT_MODIFY_REQ);
 
        #endif
 
         break;
 
       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
                        /*---------------------------*/
                        /* MT call modify response   */
                        /*---------------------------*/
 
       case CM_CALL_CMD_MODIFY_RES:
 
        #if defined (FEATURE_MULTIMEDIA_FALLBACK)
 
         /* Update call fields by copying relevant
         ** command fields into call struct.
         */
         cmcall_cmd_copy_fields( call_ptr, call_cmd_ptr );
 
         cmcall_event (call_ptr, CM_CALL_EVENT_MODIFY_RES);
 
        #endif
 
         break;
      #endif
 
       default:
 
         /*lint -save -e571 Suspicious cast */
         CM_ERR_1( "Bad call command %d", (uint8) call_cmd_ptr->cmd);
         /*lint -restore */
 
     } /* switch */
 
   }
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
   /* Else - call command is NOT successful!
   **
   ** If this is a call-end command, we better end the call anyhow,
   ** so we are back in sync with MC.
   **
   ** This is currently only important for OTASP calls where we wait
   ** for the CM_OTASP_STATUS_F reply from MC.
   */
   else
   {
     if( call_cmd_ptr->cmd == CM_CALL_CMD_END )
     {
       /* Set the call end status to indicate
       ** on a communication problem with LL.
       */
       for ( i=0; i < call_cmd_ptr->info.num_end_call_ids; i++ )
       {
 
         /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
         /* cmwcall should end calls of info_type GW_CS or GW_PS only
         */
         if (!( (cmcall_call_id_mode (call_cmd_ptr->info.end_params[i].call_id)
                == CM_CALL_MODE_INFO_GW_CS)  ||
 
                (cmcall_call_id_mode (call_cmd_ptr->info.end_params[i].call_id)
                == CM_CALL_MODE_INFO_GW_PS)
              )
            )
         {
           continue;
         }
 
         /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
         call_ptr = cmcall_ptr(call_cmd_ptr->info.end_params[i].call_id);
 
         if (call_ptr != NULL)
         {
           call_ptr->end_status = CM_CALL_END_CLIENT_END;
           cmwcall_end_req( call_ptr );
           cmwcall_end( call_ptr );
         }
       }
     }
   }
   #endif
 }

 /*===========================================================================
 
 FUNCTION cmwcall_client_cmd_proc
 
 DESCRIPTION
   Process clients' call commands
 
 DEPENDENCIES
   Call object must have already been initialized with
   cmcall_init().
 
 RETURN VALUE
   none
 
 SIDE EFFECTS
   none
 
 ===========================================================================*/
 void cmwcall_client_cmd_proc(
 
     cm_call_cmd_s_type *call_cmd_ptr
         /* pointer to a CM command */
 )
 {
 
   cm_call_cmd_err_e_type    call_cmd_err;
 
   boolean                   wait_for_reply;
 
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
   CM_ASSERT( call_cmd_ptr != NULL );
   CM_ASSERT( call_cmd_ptr->cmd_type == CM_CMD_TYPE_CALL );
 
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
   /* Start by checking whether this is a valid command
   ** relative to the current state of the call/phone.
   */
  #ifdef FEATURE_GSTK
   if (call_cmd_ptr->info.call_control_processing_complete == TRUE)
     call_cmd_err = call_cmd_ptr->info.call_control_cmd_err;
   else
  #endif
   {
     call_cmd_err = cmwcall_client_cmd_check( call_cmd_ptr );
   }
 
   /*
   ** Notify clients of command processing status.
   ** If error found, return.
   */
   cmcall_client_cmd_err( call_cmd_ptr, call_cmd_err );
 
   if( call_cmd_err != CM_CALL_CMD_ERR_NOERR )
   {
     return;
   }
 
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   /* If we got here, no errors were found,
   ** so forward the client command to Lower Layer.
   */
   wait_for_reply = cmwcall_client_cmd_forward_ll( call_cmd_ptr );
 
   /* If NO need to wait for replay from MN,
   ** call the reply function right a way.
   */
   if( ! wait_for_reply )
   {
     cmwcall_client_cmd_ll_reply(call_cmd_ptr, call_cmd_err );
   }
 }

#ifdef FEATURE_MMODE_SRVCC
/*===========================================================================

FUNCTION cmwcall_is_int_ims

DESCRIPTION
  Return true if int IMS enabled for SRVCC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
boolean cmwcall_is_int_ims( void )
{

 return is_int_ims_enabled_for_srvcc;

} /* cmwcall_is_int_ims() */

/*===========================================================================
FUNCTION cmwcall_set_int_ims

DESCRIPTION
  Set IMSfor SRVCC during RRC HO START indication

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_set_int_ims( boolean is_ims_enabled )
{

  is_int_ims_enabled_for_srvcc = is_ims_enabled ;

} /* cmwcall_set_int_ims() */


#endif

#if !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
static void cmwcall_codec_ho_handler(
    sys_network_mode_e_type network_mode,
    cm_rpt_type        *cm_rpt_ptr
)
{
  #if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
  cm_call_id_type          call_id = CM_CALL_ID_INVALID;
  cmcall_s_type            *call_ptr;
  sys_codec_ho_info        *codec_info_ptr;
  cm_mm_call_info_s_type   *call_info_ptr = NULL;
  uint8 ho_mapping_index=0;
  sys_modem_as_id_e_type ll_as_id = cm_rpt_ptr->asubs_id;
  cmph_sub_config_s_type   *cmph_sub_info;


  ll_as_id = cmph_get_as_id_from_ll_ind(ll_as_id);
  cmph_sub_info = cmph_get_sub_config_ptr(ll_as_id);

  CM_MSG_HIGH_6("RX: CODEC_INFO_IND, network %d, as_id %d, lte as_id %d cmd %d,ho_type %d, oprt_mode %d",
                network_mode, 
                ll_as_id,
                cmph_get_sub_with_lte_cap(),
                cm_rpt_ptr->cmd.codec_info.cmd,
                cm_rpt_ptr->cmd.codec_info.ho_type,
                cmph_ptr()->oprt_mode);  

  if(cmph_ptr()->oprt_mode != SYS_OPRT_MODE_ONLINE)
  {
    return;
  }

   call_id = cmcall_find_active_voice_call();

  if(call_id == CM_CALL_ID_INVALID )
  {
    CM_MSG_HIGH_0("cmwcall_codec_ho_handler,no voice call found");

    /* Allocate a call info buffer */
    call_info_ptr = cmcall_info_alloc();
    if ( call_info_ptr == NULL )
    {
      CM_MSG_HIGH_0("cmwcall_codec_ho_handler, can not alloc memory for call info object ");
      return;
    }
    call_info_ptr->asubs_id = ll_as_id;
    codec_info_ptr = &cm_rpt_ptr->cmd.codec_info;

    for(ho_mapping_index=0; ho_mapping_index<ARR_SIZE(cmwcall_ho_mapping_table);
      ho_mapping_index++)
    {
      if(cmwcall_ho_mapping_table[ho_mapping_index].rrc_ho_type == codec_info_ptr->ho_type)
      {
        call_info_ptr->voice_ho_type = cmwcall_ho_mapping_table[ho_mapping_index].client_ho_type;
        CM_MSG_HIGH_2("Mapped ho type %d, RRC ho type %d",
        call_info_ptr->voice_ho_type,codec_info_ptr->ho_type);
        break;
  
      }
    }
  if(ho_mapping_index == ARR_SIZE(cmwcall_ho_mapping_table))
  {
    CM_MSG_HIGH_1("invalid rrc ho_type %d", codec_info_ptr->ho_type);
    call_info_ptr->voice_ho_type = SYS_VOICE_HO_NONE;
  }

    /* Notify clients list of call event. */

    switch(cm_rpt_ptr->cmd.codec_info.cmd)
    {
    case SYS_SPEECH_CODEC_INFO:
      call_info_ptr->speech_codec_info.network_mode = network_mode;
      call_info_ptr->speech_codec_info.speech_codec_type = cm_rpt_ptr->cmd.codec_info.speech_codec;
      call_info_ptr->speech_codec_info.speech_enc_samp_freq = cm_rpt_ptr->cmd.codec_info.speech_enc_samp_freq;
      CM_MSG_HIGH_2("GWT codec_type=%d, samp_freq=%d",cm_rpt_ptr->cmd.codec_info.speech_codec, cm_rpt_ptr->cmd.codec_info.speech_enc_samp_freq);
      cmclient_list_call_event_ntfy ( CM_CALL_EVENT_SPEECH_CODEC_INFO, call_info_ptr );
      break;
    
    case SYS_HO_START:
   
    #ifdef FEATURE_MMODE_SRVCC
    CM_MSG_HIGH_3("Sub %d mid call support %d, timer value %d",
                    ll_as_id,
                    cmph_sub_info->mid_call_support,
                    cmph_sub_info->ims_cache_expiry_duration);
    if(codec_info_ptr->ho_type == SYS_HO_TYPE_LTE_GSM_SRVCC || codec_info_ptr->ho_type == SYS_HO_TYPE_LTE_UMTS_SRVCC )
      {
        cm_ims_cache_init();
        cmwcall_set_int_ims(FALSE);
        ims_call_id = CM_CALL_ID_INVALID;
        if(cmph_sub_info->mid_call_support)
        {
          // Start timer
          call_info_ptr->mid_call_supported = TRUE;
          cm_ims_cache_update(codec_info_ptr->ho_type, ll_as_id);
        }
      }
     #endif
    cmclient_list_call_event_ntfy ( CM_CALL_EVENT_HO_START, call_info_ptr );
      break;

    case SYS_HO_FAIL:
      /* Notify clients list of call event. */
      cmclient_list_call_event_ntfy ( CM_CALL_EVENT_HO_FAIL, call_info_ptr );
      break;
    case SYS_HO_COMPLETE:
      #ifdef FEATURE_MMODE_SRVCC
      if(codec_info_ptr->ho_type == SYS_HO_TYPE_LTE_GSM_SRVCC || codec_info_ptr->ho_type == SYS_HO_TYPE_LTE_UMTS_SRVCC)
      {
        CM_MSG_HIGH_0("Drop dup HO complete for L->G/ L->W srvcc");
        break;
      }
      /* Notify clients list of call event. */
      #endif
      cmclient_list_call_event_ntfy ( CM_CALL_EVENT_HO_COMPLETE, call_info_ptr );
      break;
    default:
      CM_ERR_0("cmwcall_codec_ho_handler, cmd not mapped");
      break;
    }

    /* Deallocate the memory after sending the call event. */
    cm_mem_free(call_info_ptr);
  }
  else
  {

  CM_MSG_HIGH_1("cmwcall_codec_ho_handler, voice call found %d",call_id);
    call_ptr = cmcall_ptr ( call_id );
    if( call_ptr == NULL )
    {
      CM_MSG_HIGH_0("cmwcall_codec_ho_handler, call ptr NULL");
      return;
    }

    codec_info_ptr = &cm_rpt_ptr->cmd.codec_info;
	
    for(ho_mapping_index=0; ho_mapping_index<ARR_SIZE(cmwcall_ho_mapping_table);
      ho_mapping_index++)
  {
       if(cmwcall_ho_mapping_table[ho_mapping_index].rrc_ho_type == codec_info_ptr->ho_type)
     {
       call_ptr->voice_ho_type = cmwcall_ho_mapping_table[ho_mapping_index].client_ho_type;
       CM_MSG_HIGH_2("Mapped ho type %d , RRC ho_type %d",
       call_ptr->voice_ho_type,codec_info_ptr->ho_type);
     break;
     }
  }
  if(ho_mapping_index == ARR_SIZE(cmwcall_ho_mapping_table))
  {
    CM_MSG_HIGH_1("invalid rrc ho_type: %d",codec_info_ptr->ho_type);
    call_ptr->voice_ho_type  = SYS_VOICE_HO_NONE;
  }

  /* Notify clients list of call event. */
    switch(codec_info_ptr->cmd)
    {
    case SYS_SPEECH_CODEC_INFO:
      call_ptr->speech_codec_info.network_mode = network_mode;
      call_ptr->speech_codec_info.speech_codec_type = codec_info_ptr->speech_codec;
      call_ptr->speech_codec_info.speech_enc_samp_freq = codec_info_ptr->speech_enc_samp_freq;
      CM_MSG_HIGH_2("GWT codec_type=%d, samp_freq=%d",codec_info_ptr->speech_codec, codec_info_ptr->speech_enc_samp_freq);
      cmcall_event( call_ptr, CM_CALL_EVENT_SPEECH_CODEC_INFO );
      break;
    case SYS_HO_START:
      /* For SRVCC, Intrnal IMS, send ipapp notify to start in case of L->GW,
         ** For GW->GW, send client notifications.
      */
      /* identify L->G/W SRVCC */
    #ifdef FEATURE_MMODE_SRVCC
    if(codec_info_ptr->ho_type == SYS_HO_TYPE_LTE_GSM_SRVCC || codec_info_ptr->ho_type == SYS_HO_TYPE_LTE_UMTS_SRVCC )
    {
      cmwcall_set_int_ims(TRUE);

        ims_call_id = cmcall_get_active_srvcc_volte_call(CM_CALL_TYPE_VOICE_MASK |
                                                           CM_CALL_TYPE_EMERG_MASK   |
                                                           CM_CALL_TYPE_VT_MASK,
                                                           ll_as_id);

        cm_ims_cache_init();
        cmipapp_send_srvcc_notification(CMIPAPP_SRVCC_HO_START_NOTIFY, ll_as_id);

  CM_MSG_HIGH_3("Sub %d mid call support %d, timer value %d",
                  ll_as_id,
                  cmph_sub_info->mid_call_support,
                  cmph_sub_info->ims_cache_expiry_duration);

      if(cmph_sub_info->srvcc_sync_support)
      {
        /* srvcc_sync: set wait_for_audio_rel as TRUE, as CM needs to wait for Audion rel from IMS */
        cm_set_wait_for_ims_audio_rel(TRUE, ll_as_id);
      }
       
         if(cmph_sub_info->mid_call_support)
         {
           call_ptr->mid_call_supported = TRUE;        
           /* need to set ho_type even in case of non midcall support due to srvcc_sync design */
             cm_ims_cache_update(codec_info_ptr->ho_type, ll_as_id);
         }
         else
         {
           /* just set ho_type */
           cm_set_ims_cache_ho_type(codec_info_ptr->ho_type, ll_as_id);
         }
         
          /* decide on sending HO_START during SRVCC based on midcall value/ valid IMS call  */
          if(ims_call_id != CM_CALL_ID_INVALID)
          {
            call_ptr = cmcall_ptr ( ims_call_id );
            if( call_ptr == NULL )
            {
              CM_MSG_HIGH_0("cmwcall_codec_ho_handler, call ptr NULL");
              break;
            }
                /* set ho_type for active volte call for SRVCC HO START */
              call_ptr->voice_ho_type  = 
                (ho_mapping_index == ARR_SIZE(cmwcall_ho_mapping_table))?
                SYS_VOICE_HO_NONE : cmwcall_ho_mapping_table[ho_mapping_index].client_ho_type;
                  
              cmcall_event( call_ptr, CM_CALL_EVENT_HO_START );
          }
          else
          {
            CM_MSG_HIGH_0("NO VOLTE call active for HO START");
            break;
          }
        }
        /* If not SRVCC, send call event HO START for all GW transfers */
        else
        #endif
        {
          cmcall_event( call_ptr, CM_CALL_EVENT_HO_START );
        }
     break;
    case SYS_HO_FAIL:
      cmcall_event( call_ptr, CM_CALL_EVENT_HO_FAIL );
      break;
    case SYS_HO_COMPLETE:
      #ifdef FEATURE_MMODE_SRVCC
      if(codec_info_ptr->ho_type == SYS_HO_TYPE_LTE_GSM_SRVCC || codec_info_ptr->ho_type == SYS_HO_TYPE_LTE_UMTS_SRVCC )
      {
        if(cmph_sub_info->srvcc_sync_support)
        {
          CM_MSG_HIGH_0("srvcc_sync: sending HO_COMPLETE_NOTIFY to IMS");
          cmipapp_send_srvcc_notification(CMIPAPP_SRVCC_HO_COMPLETE_NOTIFY, ll_as_id);
          break;
        }
        else
        {
          CM_MSG_HIGH_0("Drop dup HO complete for L->G/ L->W srvcc");
          break;
        }
        CM_MSG_MED_0("HO complete for L->G/ L->W srvcc scenario");       
      }
      #endif
      cmcall_event( call_ptr, CM_CALL_EVENT_HO_COMPLETE );
      break;
    default:
      CM_ERR_0("cmwcall_codec_ho_handler, cmd not mapped");
      break;
    }
  }

  return;
  #endif
}
#endif

#ifdef FEATURE_MMODE_SRVCC
/*===========================================================================

FUNCTION cmwcall_timer_proc

DESCRIPTION
  Process timer event for SRVCC cache expiry

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_timer_proc(

  cm_timer_event_type    timer_event
    /* Indicate specific timer event.
    */
)
{
  dword                uptime = time_get_uptime_secs();

  /* Check if maximum time expired.
  */
  if ( uptime *1000 >  cm_get_cache_ind().cache_expiry_timer)
  {
    // If timer expires and we're in wait state. then clear all state,set cache validity
    if (cm_get_cache_ind().state == CMWCALL_CACHE_EXPECTED)
    {
      CM_MSG_HIGH_0("send FAIL rpt to NAS");
      cm_process_cmd_on_ims_cache(CMWCALL_CACHE_EXPIRED);
    }

  }

} /* cmwcall_timer_proc() */

/*===========================================================================

FUNCTION cmwcall_set_midcall_srvcc_info

DESCRIPTION
 Sets the srvcc info during GET REQ message from NAS

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmwcall_set_midcall_srvcc_info(cmcall_s_type *call_ptr, sys_radio_access_tech_e_type rat)
{

  #ifdef FEATURE_IP_CALL
  call_ptr->is_volte = FALSE;
  #endif

  call_ptr->is_mpty = FALSE;
  call_ptr->parent_call_id = CM_CALL_ID_INVALID; //set parent id as invalid for no mid call support
  call_ptr->is_srvcc_call = TRUE;
  call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_CS;

  if(call_ptr->call_type == CM_CALL_TYPE_VT)
  {
    CM_MSG_HIGH_0("VT change to voice");
    call_ptr->call_type = CM_CALL_TYPE_VOICE;
	call_ptr->ims_call_type  = CM_CALL_TYPE_VT;
  }

  switch(rat)
  {
    case SYS_RAT_GSM_RADIO_ACCESS:
      call_ptr->sys_mode = SYS_SYS_MODE_GSM;
      call_ptr->voice_ho_type = SYS_VOICE_HO_SRVCC_L_2_G;
    break;

    case SYS_RAT_UMTS_RADIO_ACCESS:
      call_ptr->sys_mode = SYS_SYS_MODE_WCDMA;
      call_ptr->voice_ho_type = SYS_VOICE_HO_SRVCC_L_2_W;
    break;

    default:
      CM_ERR_0("SRVCC:Unexpected RAT received");
    break;
  }
}


/*===========================================================================

FUNCTION cmwcall_send_mn_SRVCC_call_context

DESCRIPTION
Code to send srvcc call_context to NAS
  1. Get cmd_buffer from NAS
  2. Fill the parameters and post it to NAS

DEPENDENCIES
NAS should have already requested for cache using CM_GET_SRVCC_CONTEXT_REQ

RETURN VALUE
None

SIDE EFFECTS
none

===========================================================================*/
void cmwcall_send_mn_SRVCC_call_context(
  cm_srvcc_call_context_T   *cm_srvcc_call_context_p
)
{
  if (NULL != cm_srvcc_call_context_p)
  {
    mn_cnm_cmd_type           *mn_ptr = NULL;
    mn_ptr = cm_mn_get_buf_else_err_fatal( CM_TRANSFER_SRVCC_CALL_CONTEXT_RES);
    if (NULL != mn_ptr)
    {
      cm_srvcc_call_context_p->message_header = mn_ptr->cmd.cm_srvcc_call_context_res.message_header;
      mn_ptr->cmd.cm_srvcc_call_context_res = *cm_srvcc_call_context_p;
      CM_MSG_HIGH_2 ("CM -> MN SRVCC msg hdr %d, num_calls %d",
        mn_ptr->cmd.hdr.message_id, 
        mn_ptr->cmd.cm_srvcc_call_context_res.num_calls);
      cm_mn_send_cmd( mn_ptr );
    }
  }
  return;
}

/*===========================================================================

FUNCTION cmwcall_handle_get_srvcc_context_req

DESCRIPTION
Code to handle get srvcc context request from NAS, and send across the context transfer response back to NAS.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
none

===========================================================================*/
void cmwcall_handle_get_srvcc_context_req(cm_rpt_type *cm_rpt_ptr)
{

  cm_srvcc_call_context_T   *cm_srvcc_call_context_p = NULL;
  int nas_call_state_index =0;
  byte   connection_id;
  cmcall_s_type            *call_ptr;
  cm_iterator_type call_itr;
  int i=0;
  cm_call_id_type          call_id = CM_CALL_ID_INVALID;
  sys_modem_as_id_e_type   as_id = SYS_MODEM_AS_ID_1;
  cmph_sub_config_s_type   *cmph_sub_info;
  
  CM_MSG_HIGH_3("RXD: CM_GET_SRVCC_CONTEXT_REQ FROM MN, rat=%d, as_id=%d, lte as_id=%d",
                  cm_rpt_ptr->cmd.nas_srvcc_get_req_ind.active_rat,
                  as_id,
                  cmph_get_sub_with_lte_cap()); 
  

  if (cm_get_cache_ind().call_context_t == NULL)
  {
     CM_MSG_HIGH_0("Call_context cache is NULL, Ignore CM_GET_SRVCC_CONTEXT_REQ");
     return;
  }
  
  cm_srvcc_call_context_p = (cm_srvcc_call_context_T *) cm_mem_malloc (sizeof(cm_srvcc_call_context_T));
  /* cm_srvcc_call_context_p will be freed at all the returns from this function */
  if (cm_srvcc_call_context_p == NULL)
  {
    CM_ERR_0 ("Error getting MN buffer CM_TRANSFER_SRVCC_CALL_CONTEXT_RES");
    return;
  }

  as_id = cmph_get_as_id_from_ll_ind(cm_rpt_ptr->cmd.nas_srvcc_get_req_ind.as_id);
  cmph_sub_info = cmph_get_sub_config_ptr(as_id);

  cm_srvcc_call_context_p->message_header.message_id = CM_TRANSFER_SRVCC_CALL_CONTEXT_RES;
  cm_srvcc_call_context_p->message_header.message_set =MS_MN_CM;
  cm_srvcc_call_context_p->as_id = as_id;
  
  /*if no midcall support, fallback to connected SRVCC*/
  if (!cmph_sub_info->mid_call_support)
  {
    if(!cmwcall_is_int_ims())
    {
      CM_MSG_HIGH_0("CM_GET_SRVCC_CONTEXT_REQ:legacy ext IMS");
      call_id = cm_call_id_allocate();
      if(call_id == CM_CALL_ID_INVALID)
      {
        CM_MSG_HIGH_0("CM_GET_SRVCC_CONTEXT_REQ, call not found");
        cm_mem_free(cm_srvcc_call_context_p);
        return;
      }
      call_ptr = cmcall_ptr(call_id);

      call_ptr->call_type = CM_CALL_TYPE_VOICE;
      call_ptr->call_state = CM_CALL_STATE_CONV;
      call_ptr->call_subst.conv = CM_CALL_CONV_CALL_ACTIVE;
      call_ptr->direction = CM_CALL_DIRECTION_MT;
      call_ptr->is_int_ims = FALSE;

      cm_set_connection_id(call_ptr,CM_MIN_MT_TI);
      cm_set_connection_id_src(call_ptr,CONN_ID_MN);
      cmwcall_set_midcall_srvcc_info(call_ptr,cm_rpt_ptr->cmd.nas_srvcc_get_req_ind.active_rat);
      call_ptr->asubs_id = as_id;
      call_ptr->ss = cmph_determine_ss_per_domain(call_ptr->asubs_id,SYS_SRV_DOMAIN_CS_ONLY);
      cm_srvcc_call_context_p->num_calls =1;
      cm_srvcc_call_context_p->individual_call_context[0].srvcc_call_cc_state = CC_STATE_U10;
      cm_srvcc_call_context_p->individual_call_context[0].srvcc_call_connection_id = MT_TID_8;
      cm_srvcc_call_context_p->individual_call_context[0].srvcc_call_hold_aux_state =HOLD_IDLE;
      cm_srvcc_call_context_p->individual_call_context[0].srvcc_call_mpty_aux_state = MPTY_IDLE ;
    }/* ext IMS */
    else
    {
      #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
      call_id = cmcall_get_active_volte_call(CM_CALL_TYPE_VOICE_MASK | CM_CALL_TYPE_EMERG_MASK | CM_CALL_TYPE_VT_MASK);
      #endif
      CM_MSG_HIGH_0("CM_GET_SRVCC_CONTEXT_REQ:legacy IMS");
     
      if(call_id != CM_CALL_ID_INVALID)
      {
        if(cmph_sub_info->srvcc_sync_support && (cm_is_wait_for_ims_audio_rel(as_id) == TRUE) )
        {
          /* send ho complete to IMS and return, later when audio rel will be confirmed by IMS, 
                     cm will post get srvcc context command again to send response back to NAS */
          cmipapp_send_srvcc_notification(CMIPAPP_SRVCC_HO_COMPLETE_NOTIFY, as_id);
          /* store RAT and other info as need to use when audio rel cnf would be received and get_srvcc_context has to be re used */
          cm_set_ims_cache_state(CMWCALL_CACHE_RECIEVED);
          cm_set_nas_cache_req(CM_GET_SRVCC_CONTEXT_REQ,cm_rpt_ptr->cmd.nas_srvcc_get_req_ind.active_rat);
          CM_MSG_HIGH_0 ("srvcc_sync: no mid_call CM -> IMS IPAPP COMPLETE during GET_REQ as wait for ims aud rel is true");
          cm_mem_free(cm_srvcc_call_context_p);
          return;
        }
        
        call_ptr = cmcall_ptr(call_id);
        if( call_ptr == NULL )
        {
          sys_err_fatal_null_ptr_exception();
          CM_ERR_0( "Call_ptr is NULL so return from cmwcall_handle_get_srvcc_context_req()");
          cm_mem_free(cm_srvcc_call_context_p);
          return;
        }

        if (cm_find_connection_id(call_id,&connection_id))
        {
          call_ptr->ims_connection_id = connection_id;
        }
        call_ptr->is_int_ims = TRUE;
        cm_set_connection_id(call_ptr,MT_TID_8);
        cm_set_connection_id_src(call_ptr,CONN_ID_MN);
        if(cmph_sub_info->srvcc_sync_support && cm_get_cache_ind().rat != SYS_RAT_NONE)
        {
          CM_MSG_HIGH_1("srvcc_sync: Using rat from cmwcall_cache_ind %d",cm_get_cache_ind().rat);
          cmwcall_set_midcall_srvcc_info(call_ptr,cm_get_cache_ind().rat);
        }
        else
        {
          cmwcall_set_midcall_srvcc_info(call_ptr,cm_rpt_ptr->cmd.nas_srvcc_get_req_ind.active_rat);
        }
        cm_srvcc_call_context_p->num_calls =1;
        cm_srvcc_call_context_p->individual_call_context[0].srvcc_call_connection_id = MT_TID_8;
        for( nas_call_state_index =0;nas_call_state_index< ARR_SIZE( srvcc_ims_to_nas_call_mapping);
             nas_call_state_index++)
        {
          if(srvcc_ims_to_nas_call_mapping[nas_call_state_index].conv_subst == \
                call_ptr->call_subst.conv && \
                srvcc_ims_to_nas_call_mapping[nas_call_state_index].is_mpty == FALSE)
          {
            break;
          }
        }
        if(nas_call_state_index < ARR_SIZE( srvcc_ims_to_nas_call_mapping))
        {
          /* set call_context_T fields for NAS req */
          cm_srvcc_call_context_p->individual_call_context[0].srvcc_call_cc_state =
          srvcc_ims_to_nas_call_mapping[nas_call_state_index].cc_state;

          cm_srvcc_call_context_p->individual_call_context[0].srvcc_call_hold_aux_state =
          srvcc_ims_to_nas_call_mapping[nas_call_state_index].nas_hod_aux_state;

          cm_srvcc_call_context_p->individual_call_context[0].srvcc_call_mpty_aux_state =
          srvcc_ims_to_nas_call_mapping[nas_call_state_index].nas_mpty_aux_state;
        }

      }/* Active call Connected SRVCC */

      else
      {
         cm_srvcc_call_context_p->num_calls =0;
      }/* held/alerting SRVCC */
  
    }/* int IMS */
    cmwcall_send_mn_SRVCC_call_context(cm_srvcc_call_context_p);

  }/*no midcall support */
    
  /* mid call supported */
  else
  {
    CM_MSG_HIGH_0("CM_GET_SRVCC_CONTEXT_REQ: midcall supported");
     /* timer expired ,send HO FAIL to IMS*/
    if(cm_get_cache_ind().state ==  CMWCALL_CACHE_EXPIRED)
    {
      CM_MSG_HIGH_0("CM_GET_SRVCC_CONTEXT_REQ:Int/Ext, send HO FAIL context,timer expired ");

      cm_srvcc_call_context_p->num_calls =0;
      cmwcall_send_mn_SRVCC_call_context(cm_srvcc_call_context_p);
      /*reset values for ims cache */
      cm_free_ims_cache_call_context();
      /* srvcc_sync: set wait_for_ims_audio_rel false, as do not need to wait */
      cm_set_wait_for_ims_audio_rel(FALSE, as_id);
      cm_mem_free(cm_srvcc_call_context_p);
      return;
    }

    /*  wait untill IMS send cache*/
    else if( cm_get_cache_ind().state == CMWCALL_CACHE_EXPECTED)
    {
      cm_set_nas_cache_req(CM_GET_SRVCC_CONTEXT_REQ,cm_rpt_ptr->cmd.nas_srvcc_get_req_ind.active_rat);
      CM_MSG_HIGH_0("CM_GET_SRVCC_CONTEXT_REQ:ext IMS, NAS Request queued till cache expiry");
      cm_mem_free(cm_srvcc_call_context_p);
      return;
    }
    /* cahce set with valid call context  */
    else if( cm_get_cache_ind().state== CMWCALL_CACHE_RECIEVED)
    {
      /* srvcc-sync: check if audio release has been rcvd or not 
                  if not then do not send cache context to NAS */
      if(cmph_sub_info->srvcc_sync_support &&
           (cm_is_wait_for_ims_audio_rel(as_id) == TRUE) )
      {
        /* send ho complete to ISM and return, later when audio rel will be confjirmed by IMS, 
                   cm will post get srvcc context command again to send response back to NAS */
        cmipapp_send_srvcc_notification(CMIPAPP_SRVCC_HO_COMPLETE_NOTIFY, as_id);        
        cm_set_nas_cache_req(CM_GET_SRVCC_CONTEXT_REQ,cm_rpt_ptr->cmd.nas_srvcc_get_req_ind.active_rat);
        CM_MSG_HIGH_0 ("srvcc_sync: CM -> IMS IPAPP COMPLETE during GET_REQ as wait for ims aud rel is true");
        cm_mem_free(cm_srvcc_call_context_p);
        return;
      }          

      if((cmph_sub_info->srvcc_sync_support == FALSE) ||
            (cmph_sub_info->srvcc_sync_support == TRUE &&
            cm_is_wait_for_ims_audio_rel(as_id) == FALSE) )
      {
      
      CM_MSG_HIGH_0("CM_GET_SRVCC_CONTEXT_REQ: send NAS response with valid cache");
        /*cache filed by IMS after Expiry timer */

        cm_srvcc_call_context_p->num_calls = cm_get_cache_ind().call_context_t->num_calls;

        cmcall_obj_iterator_init(&call_itr);

        call_ptr = cmcall_obj_get_next(&call_itr);

        /*loop through each srvcc call and set rat */
        while(call_ptr != NULL)
        {
          if( call_ptr->is_srvcc_call)
          {
            call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_CS;
            /* Call obj transfer to W/G */
            switch(cm_rpt_ptr->cmd.nas_srvcc_get_req_ind.active_rat)
            {
              case SYS_RAT_GSM_RADIO_ACCESS:
                call_ptr->sys_mode = SYS_SYS_MODE_GSM;
                call_ptr->voice_ho_type = SYS_VOICE_HO_SRVCC_L_2_G;

                if(call_ptr->call_state == CM_CALL_STATE_ORIG)
                {
                  call_ptr->sr.orig_sent_on_mode = SYS_SYS_MODE_GSM;
                }
                break;

              case SYS_RAT_UMTS_RADIO_ACCESS:
                call_ptr->sys_mode = SYS_SYS_MODE_WCDMA;
                call_ptr->voice_ho_type = SYS_VOICE_HO_SRVCC_L_2_W;
                /* for IMS MO ORIG call, call end needs to be communicated to NAS */
                if(call_ptr->call_state == CM_CALL_STATE_ORIG )
                {
                  call_ptr->sr.orig_sent_on_mode = SYS_SYS_MODE_WCDMA;
                }

                break;
              default:
              CM_ERR_0("SRVCC:Unexpected RAT received");
              break;
            }
          }
          call_ptr = cmcall_obj_get_next(&call_itr);
        }

        for(i=0;i < cm_get_cache_ind().call_context_t->num_calls;i++)
        {
           cm_srvcc_call_context_p->individual_call_context[i] =
           cm_get_cache_ind().call_context_t->individual_call_context[i];
        }


        cmwcall_send_mn_SRVCC_call_context(cm_srvcc_call_context_p);
        /*reset values for ims cache */
        cm_free_ims_cache_call_context();
      }
    }  
  }  
  
  cm_mem_free(cm_srvcc_call_context_p);
  return;

}


/*===========================================================================

FUNCTION cmwcall_set_alert_media_type

DESCRIPTION
 Maps early media type from IMS to NAS during SRVCC

DEPENDENCIES
  none

RETURN VALUE
  cm_early_media_type_T

SIDE EFFECTS
  none

===========================================================================*/

cm_nas_srvcc_alert_media_type cmwcall_set_alert_media_type(cm_srvcc_alert_media_type ims_alert_media_type)
{
  cm_nas_srvcc_alert_media_type nas_alert_media_type = CM_NAS_SRVCC_ALERT_MEDIA_NONE;


  switch(ims_alert_media_type)
  {

    case CM_SRVCC_ALERTING_NW :
      nas_alert_media_type = CM_NAS_SRVCC_ALERT_MEDIA_NW;
	  break;

    case CM_SRVCC_ALERTING_UE:
      nas_alert_media_type = CM_NAS_SRVCC_ALERT_MEDIA_LOCAL;
      break;

    default:
      CM_ERR_0("SRVCC:Unexpected early media received, assingning  Alert media- NONE");
    break;
  }

  return nas_alert_media_type;
}


/*===========================================================================

FUNCTION cmwcall_handle_srvcc_ims_cache

DESCRIPTION
 Process IMS Cache and :
 a)Create new call objects for non-modem calls and  Conference
 b)Update existing IMS calls with srvcc related info(parent id,srvcc call in transition flag)
 c)Creates Call context buffer to send to NAS upon request.
 d)send ipapp notification upon HO_COMPLETE if IMS enabled.
 E)Send call event for one of srvcc call.

 Supported call types:
 i)  Single call in HOLD/ACTIVE/RETRIEVE REQUEST/HOLD REQUEST
 ii) Conference call in HOLD/ACTIVE/RETRIEVE REQUEST/HOLD REQUEST
 iii)Incoming Alerting
 iv)Outgoing Alerting


  cm_srvcc_call_s_type has all call combination expected.

  Connection ID Scenarios for Call Combinations:
             (MASK)

  CASE 1: Single Active Call

  CASE 2: Single Held Call

  CASE 3:  Single Incoming Call

  CASE 4:  Single Outgoing Call

  CASE 5: Active Conference Call

  CASE 6: Held Conference Call

  CASE 7: Held Request Call

  CASE 8: Retrieve Request Call

  CASE 9: Active and Held Call

  CASE 10: Active and Alerting incoming  Call

  CASE 11:Held and Alerting outgoing Call

  CASE 12: Active and Conference Held Call

  CASE 13: Held and Conference Active Call

  CASE 14: Held and Alerting Incoming Call

  CASE 15: Alerting Call Scenarios

  CASE 16: Incoming Call and Held Request

  CASE 17: Retrieve Request and Incoming Call

  CASE 18: Held Request  and Outgoing Call

  CASE 19: Retrieve Request and Outgoing Call

  CASE 20: Conference Active  and Incoming Call

  CASE 21: Conference Held  and Incoming Call

  CASE 22: Conference Active   and Outgoing Call

  CASE 23: Conference Held   and Outgoing Call

DEPENDENCIES
  Call object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_handle_srvcc_ims_cache(

  const cm_srvcc_call_context_rsp_s_type *cahce_rpt_ptr  /* Pointer to msgr report from IMS */
)

{

  int i=0;
    /* loop counters for calls */

  int j=0;
    /* loop counters for participant calls */

  int tid_table_index =0;
    /* tid assignment table index */

  int nas_call_state_index =0;
    /*  NAS call state mapping index */

  int tot_participants=0;
    /* Total participatns in a conference call */

  int call_context_ind=0;
    /* individual call contect buffer  */

  cm_call_id_type          call_id = CM_CALL_ID_INVALID;

  cmcall_s_type            *call_ptr = NULL;
    /* temp call ptr  */

  cm_srvcc_call_type_mask       srvcc_call_mask = CM_SRVCC_CALL_MASK_NONE;
    /* call type mask used for identifying SRVCC combo */

  cm_call_conv_subst_e_type  srvcc_call_substate = CM_CALL_CONV_SUBST_NONE;
    /* tmp call substate */

  cm_call_state_e_type srvcc_call_state;
    /* tmp call state */

  uint8                 connection_id;
    /*tmp conn id for new call */

  byte single_call_tid = TID_INVALID;
    /* TID assigned for single call scenario */

  cm_srvcc_call_context_T * call_context_t;
   /*Fill in NAS context info */

 /*=============================================================================*/


  if(cm_get_cache_ind().state == CMWCALL_CACHE_NOT_EXPECTED ||cm_get_cache_ind().state == CMWCALL_CACHE_EXPIRED )
  {
    CM_MSG_HIGH_0( "IMS cache not expected and is ignored");
    return;
  }

  CM_ASSERT( cahce_rpt_ptr != NULL );

  call_context_t = cm_get_cache_ind().call_context_t;

  if(call_context_t == NULL)
  {
    CM_ERR_0( "Not enough memory to allocate call_context ");
      return;
    }

  /* if  more than MAX_CALL_NUM_SRVCC call, simply set calls to 0 */
  if(cahce_rpt_ptr->num_of_calls > MAX_CALL_NUM_SRVCC)
  {
    CM_MSG_HIGH_1 ("Rcvd unmanegable Calls for SRVCC handover: %d",
                        cahce_rpt_ptr->num_of_calls);
    call_context_t->num_calls=0;

    cm_set_srvcc_ho_error_state  (CM_SRVCC_HO_ERR_UNMANAGED_CALLS);

    cm_process_cmd_on_ims_cache(CMWCALL_CACHE_RECIEVED);

    return;
  }
  
  else if(cahce_rpt_ptr->num_of_calls == 0)
  {
    CM_MSG_HIGH_1 ("Rcvd 0 Calls for SRVCC handover: %d ",cahce_rpt_ptr->num_of_calls);
    call_context_t->num_calls=0;

    cm_process_cmd_on_ims_cache(CMWCALL_CACHE_RECIEVED);

    return;

  }
  

  else
  {
     /* set flags according to call state of individual calls */
    for(i=0; i<cahce_rpt_ptr->num_of_calls; i++)
    {
      if(cahce_rpt_ptr->call_context[i].is_conf_call == TRUE)
      {
        srvcc_call_substate =cahce_rpt_ptr->call_context[i].call_info.
          ip_conf_call.conf_call_substate.conv;

        /* Overall Conf call state determines substate of each particpant */
        if( srvcc_call_substate== CM_CALL_CONV_CALL_ACTIVE || srvcc_call_substate == CM_CALL_CONV_HOLD_REQ_BY_UE)
        {
          srvcc_call_mask  |= CM_SRVCC_CALL_ACTIVE_CONFERENCE;
        }
        else  if(srvcc_call_substate== CM_CALL_CONV_CALL_ON_HOLD || srvcc_call_substate == CM_CALL_CONV_RETRIEVE_REQ_BY_UE)
        {
          srvcc_call_mask  |= CM_SRVCC_CALL_HELD_CONFERENCE;
        }

      }
      else
      {
        srvcc_call_state = cahce_rpt_ptr-> call_context[i].call_info.ip_call.call_state;

        if(srvcc_call_state == CM_CALL_STATE_CONV)
        {
          srvcc_call_substate = cahce_rpt_ptr-> call_context[i].call_info.ip_call.call_sub_state.conv;
        }

        if(srvcc_call_state == CM_CALL_STATE_CONV && \
          (srvcc_call_substate == CM_CALL_CONV_CALL_ACTIVE || srvcc_call_substate == CM_CALL_CONV_HOLD_REQ_BY_UE))
        {
          srvcc_call_mask |= CM_SRVCC_SINGLE_CALL_ACTIVE_MASK;
        }
        else if(srvcc_call_state == CM_CALL_STATE_CONV && \
         (srvcc_call_substate == CM_CALL_CONV_CALL_ON_HOLD || srvcc_call_substate == CM_CALL_CONV_RETRIEVE_REQ_BY_UE))
        {
          srvcc_call_mask |= CM_SRVCC_SINGLE_CALL_HELD_MASK;
        }

        else if(srvcc_call_state == CM_CALL_STATE_INCOM)
        {
          srvcc_call_mask |= CM_SRVCC_SINGLE_CALL_INCOMING_MASK;
        }
        else if(srvcc_call_state == CM_CALL_STATE_ORIG)
        {
          srvcc_call_mask |= CM_SRVCC_SINGLE_CALL_OUTGOING_MASK;
        }
      }

    }

    /* Get the Transaction id assignemnt index from the call mask */
    for(tid_table_index=0;j<ARR_SIZE(srvcc_tid_table); tid_table_index++)
    {
      if(srvcc_tid_table[tid_table_index].srvcc_call_combination ==  srvcc_call_mask)
      {
      break;
      }
    }

    if(tid_table_index >= ARR_SIZE(srvcc_tid_table))
    {
      CM_MSG_HIGH_1 ("Rcvd Unexptected Call cache combination[%d] SRVCC handover",
                        srvcc_call_mask);
      cm_set_srvcc_ho_error_state (CM_SRVCC_HO_UNMANAGED_CALL_COMBINATION);

      call_context_t->num_calls=0;

      cm_process_cmd_on_ims_cache(CMWCALL_CACHE_RECIEVED);
      return;
    }

    CM_MSG_HIGH_1 ("TID index %d", tid_table_index);

    /* loop through each call and add call_context */
    for(i=0; i<cahce_rpt_ptr->num_of_calls; i++)
    {
      /* Create new call objects for each participant in Conference
         At this point, both participant call and original ims call exist */

      if(cahce_rpt_ptr->call_context[i].is_conf_call == TRUE)
      {

        tot_participants = (cahce_rpt_ptr->call_context[i].
          call_info.ip_conf_call.num_of_participants> MAX_CONF_CALL_NUM_SRVCC)?
         (MAX_CONF_CALL_NUM_SRVCC):cahce_rpt_ptr->call_context[i].
         call_info.ip_conf_call.num_of_participants ;
		 
         CM_MSG_HIGH_2("total participants in conference : %d, for call %d", tot_participants, cahce_rpt_ptr->call_context[i].call_info.ip_conf_call.call_id);
		 
		 if (cmwcall_is_int_ims())
		 {
		   if ((call_ptr = cmcall_ptr(cahce_rpt_ptr->call_context[i].call_info.ip_conf_call.call_id)) == NULL)
           {
             sys_err_fatal_null_ptr_exception();
           }
           call_ptr->is_conf_call = TRUE;

		 }
        for(nas_call_state_index =0;nas_call_state_index< ARR_SIZE( srvcc_ims_to_nas_call_mapping);
         nas_call_state_index++)
        {
          if(srvcc_ims_to_nas_call_mapping[nas_call_state_index].conv_subst == \
            cahce_rpt_ptr->call_context[i].call_info.ip_conf_call.conf_call_substate.conv && \
            srvcc_ims_to_nas_call_mapping[nas_call_state_index].is_mpty == TRUE)
          {
            break;
          }

        }
        if(nas_call_state_index == ARR_SIZE( srvcc_ims_to_nas_call_mapping))
        {
          CM_MSG_HIGH_0 ("Dropped conf call");
          continue;
        }

        for(j = 0;j<tot_participants;j++)
        {
          /* Transfer calls only in CONV state */
          if(cahce_rpt_ptr->call_context[i].call_info.ip_conf_call.participant_list[j].call_state \
            == CM_CALL_STATE_CONV)
          {
            /*
            ** Allocate a new Call Id and a Call Object.
            */
            call_id = cm_call_id_allocate();

            if ((call_ptr = cmcall_ptr(call_id)) == NULL)
            {
              sys_err_fatal_null_ptr_exception();
            }

            srvcc_call_state = cahce_rpt_ptr->call_context[i].
            call_info.ip_conf_call.participant_list[j].call_state;

            call_ptr->call_type =
              cahce_rpt_ptr->call_context[i].call_info.ip_conf_call.participant_list[j].call_type;

            /* set call_ptr fields */
            call_ptr->is_srvcc_call= TRUE;

            /* assign parent  id as participant id for ext ims case */

            if (cmwcall_is_int_ims())
            {
              call_ptr->parent_call_id = cahce_rpt_ptr->call_context[i].call_info.ip_conf_call.call_id;
              call_ptr->is_int_ims = TRUE;
            }
            else
            {
              call_ptr->parent_call_id =  \
                cahce_rpt_ptr->call_context[i].call_info.ip_conf_call.participant_list[j].participant_id;
              call_ptr->is_int_ims = FALSE;
            }
            call_ptr->asubs_id = cahce_rpt_ptr->as_id;
            call_ptr->ss = cmph_map_as_id_to_cm_ss(call_ptr->asubs_id);
            call_ptr->is_mpty = TRUE;
       
            call_ptr->call_state = srvcc_call_state;

            call_ptr->call_subst = cahce_rpt_ptr->call_context[i].call_info.ip_conf_call.conf_call_substate;//set overall conf substate

            call_ptr->direction = cahce_rpt_ptr->call_context[i].call_info.ip_conf_call.\
              participant_list[j].call_direction ;

            call_ptr->num =
             cahce_rpt_ptr->call_context[i].call_info.ip_conf_call.participant_list[j].participant_num;

            /* set call_context_t for NAS request */
            call_context_t->individual_call_context[call_context_ind].srvcc_call_cc_state =
              srvcc_ims_to_nas_call_mapping[nas_call_state_index].cc_state;
            call_context_t->individual_call_context[call_context_ind].srvcc_call_hold_aux_state =
              srvcc_ims_to_nas_call_mapping[nas_call_state_index].nas_hod_aux_state;
            call_context_t->individual_call_context[call_context_ind].srvcc_call_mpty_aux_state =
              srvcc_ims_to_nas_call_mapping[nas_call_state_index].nas_mpty_aux_state;

            call_context_t->individual_call_context[call_context_ind].srvcc_call_connection_id =
              srvcc_tid_table[tid_table_index].conf_participant_tid_list[j];


            cm_set_connection_id(call_ptr,srvcc_tid_table[tid_table_index].conf_participant_tid_list[j]);

            cm_set_connection_id_src(call_ptr,0);//set 0 for ll as MN

            call_context_ind++;
          }

          /* non conv calls */
          else
          {
            CM_MSG_HIGH_2 ("SRVCC unsupported for conf call :rejected IMS call id:,participant call_state:%d",
             cahce_rpt_ptr->call_context[i].call_info.ip_conf_call.call_id,
             cahce_rpt_ptr->call_context[i].call_info.ip_conf_call.participant_list[j].call_state);
          }

        }//loop participants

      }//conf call

        /* non - conference calls */
      else
      {
        if(cahce_rpt_ptr->call_context[i].call_info.ip_call.call_state == CM_CALL_STATE_CONV ||
          cahce_rpt_ptr->call_context[i].call_info.ip_call.call_state == CM_CALL_STATE_INCOM ||
          cahce_rpt_ptr->call_context[i].call_info.ip_call.call_state == CM_CALL_STATE_ORIG)
        {
          call_id=cahce_rpt_ptr->call_context[i].call_info.ip_call.call_id;

          if (!cmwcall_is_int_ims())
          {
            /*
            ** Allocate a Call Id and a Call Object.
            */
            call_id = cm_call_id_allocate();

            if(call_id == CM_CALL_ID_INVALID)
            {
              CM_MSG_HIGH_0("cm_srvcc_handle_ims, call not found");
              break;
            }

            if ((call_ptr = cmcall_ptr(call_id)) == NULL)
            {
              sys_err_fatal_null_ptr_exception();
            }
            call_ptr->ims_connection_id = -1; //set to invalid conn id for ext IMS
            call_ptr->is_int_ims = FALSE;
	        call_ptr->asubs_id = cahce_rpt_ptr->as_id;
            call_ptr->ss = cmph_map_as_id_to_cm_ss(call_ptr->asubs_id);
			if (cahce_rpt_ptr->call_context[i].call_info.ip_call.call_state == CM_CALL_STATE_ORIG)
            {
			  call_ptr->favored_mode_pref = CM_MODE_PREF_GWL;
			  call_ptr->srv_type           = CM_SRV_TYPE_AUTOMATIC;
            }

          }
          else
          {
            /*Get call ptr and Save connection id of ims call */
            if (cm_find_connection_id(call_id,&connection_id))
            {
              if ((call_ptr = cmcall_ptr(call_id)) == NULL)
              {
                sys_err_fatal_null_ptr_exception();
              }
              call_ptr->ims_connection_id = connection_id;
              call_ptr->is_int_ims = TRUE;
	      
	            if(call_ptr->asubs_id == SYS_MODEM_AS_ID_NONE )
              {
                call_ptr->asubs_id = SYS_MODEM_AS_ID_1;
              }
            }
            else
            {
              CM_MSG_HIGH_0("cm_srvcc_handle_ims- IMS connection id not found, skip call");
              break;
            }

          }

          srvcc_call_state = cahce_rpt_ptr->call_context[i].call_info.ip_call.call_state;

          if(cahce_rpt_ptr->call_context[i].call_info.ip_call.call_type == CM_CALL_TYPE_VT)
          {
            CM_MSG_HIGH_0("VT change to voice");
            call_ptr->call_type = CM_CALL_TYPE_VOICE;
		        call_ptr->ims_call_type  = CM_CALL_TYPE_VT;
          }
          else
          {
            call_ptr->call_type = cahce_rpt_ptr->call_context[i].call_info.ip_call.call_type;
          }

          /* set call_ptr fields */
          call_ptr->parent_call_id = cahce_rpt_ptr->call_context[i].call_info.ip_call.call_id;

          call_ptr->is_mpty = FALSE;

          call_ptr->is_srvcc_call = TRUE;

          call_ptr->call_state = srvcc_call_state;

          call_ptr->call_subst = cahce_rpt_ptr->call_context[i].call_info.ip_call.call_sub_state;

          CM_MSG_HIGH_4 ("SRVCC call id: %d, substate: conv_subst %d, incom_subst %d, orig_subst %d",
            			call_ptr->call_id, call_ptr->call_subst.conv,
            			call_ptr->call_subst.incom, call_ptr->call_subst.orig);

          call_ptr->direction = cahce_rpt_ptr->call_context[i].call_info.ip_call.call_direction;

          call_ptr->num =
           cahce_rpt_ptr->call_context[i].call_info.ip_call.call_num;

		  call_ptr->alert_media_type = cahce_rpt_ptr->call_context[i].call_info.ip_call.call_alert_media_type;

          for(nas_call_state_index =0;nas_call_state_index< ARR_SIZE( srvcc_ims_to_nas_call_mapping);
            nas_call_state_index++)
          {
             if(srvcc_call_state ==CM_CALL_STATE_CONV && 
				cahce_rpt_ptr->call_context[i].call_info.ip_call.call_sub_state.conv != CM_CALL_CONV_SUBST_NONE)
            {
              if(srvcc_ims_to_nas_call_mapping[nas_call_state_index].conv_subst == \
                cahce_rpt_ptr->call_context[i].call_info.ip_call.call_sub_state.conv && \
                srvcc_ims_to_nas_call_mapping[nas_call_state_index].is_mpty == FALSE)
              {
                break;
              }
            }
            else if (srvcc_call_state == CM_CALL_STATE_INCOM &&
			  cahce_rpt_ptr->call_context[i].call_info.ip_call.call_sub_state.incom != CM_CALL_INCOM_SUBST_NONE )
            {
              if(srvcc_ims_to_nas_call_mapping[nas_call_state_index].incom_subst == \
                cahce_rpt_ptr->call_context[i].call_info.ip_call.call_sub_state.incom && \
                srvcc_ims_to_nas_call_mapping[nas_call_state_index].is_mpty == FALSE)
              {
                break;
              }

            }
             else if (srvcc_call_state == CM_CALL_STATE_ORIG &&
			  cahce_rpt_ptr->call_context[i].call_info.ip_call.call_sub_state.orig != CM_CALL_ORIG_SUBST_NONE )
            {
               if(srvcc_ims_to_nas_call_mapping[nas_call_state_index].orig_subst == 
                 cahce_rpt_ptr->call_context[i].call_info.ip_call.call_sub_state.orig && 
                srvcc_ims_to_nas_call_mapping[nas_call_state_index].is_mpty == FALSE)
              {
                break;
              }

            }
          }

          CM_MSG_HIGH_1 ("SRVCC NAS mapping index  %d",
            nas_call_state_index);

          if(nas_call_state_index == ARR_SIZE( srvcc_ims_to_nas_call_mapping))
          {
            CM_MSG_HIGH_1("call %d not SRVCCed due to unmanaged call state/substate ",call_ptr->call_id);
            if( !call_ptr->is_int_ims)
            {
              CM_MSG_HIGH_1("Ext IMS call %d dealloc due to unmanaged call state/substate ",call_ptr->call_id);
              cm_call_id_deallocate(call_ptr->call_id);
            }
            continue;
          }

          /* set call_context_T fields for NAS req */

          call_context_t->individual_call_context[call_context_ind].srvcc_call_cc_state =
            srvcc_ims_to_nas_call_mapping[nas_call_state_index].cc_state;

          call_context_t->individual_call_context[call_context_ind].srvcc_call_hold_aux_state =
            srvcc_ims_to_nas_call_mapping[nas_call_state_index].nas_hod_aux_state;

          call_context_t->individual_call_context[call_context_ind].srvcc_call_mpty_aux_state =
            srvcc_ims_to_nas_call_mapping[nas_call_state_index].nas_mpty_aux_state;

          /* TID Assignments */

          /* Incom Call Tid */
          if(srvcc_call_state == CM_CALL_STATE_INCOM)
          {
            single_call_tid = srvcc_tid_table[tid_table_index].incoming_call_tid;
          }
          /* Active call TID */
          else if (srvcc_call_state == CM_CALL_STATE_CONV &&
            cahce_rpt_ptr->call_context[i].call_info.ip_call.call_sub_state.conv ==
            CM_CALL_CONV_CALL_ACTIVE)
          {
            single_call_tid = srvcc_tid_table[tid_table_index].conv_call_tid;
          }
          /* Held call TID */
          else if (srvcc_call_state == CM_CALL_STATE_CONV &&
            cahce_rpt_ptr->call_context[i].call_info.ip_call.call_sub_state.conv ==
            CM_CALL_CONV_CALL_ON_HOLD )
          {
            single_call_tid = srvcc_tid_table[tid_table_index].held_call_tid;
          }
          /* transient call TID */
          else if (srvcc_call_state == CM_CALL_STATE_CONV &&
            cahce_rpt_ptr->call_context[i].call_info.ip_call.call_sub_state.conv ==
            CM_CALL_CONV_RETRIEVE_REQ_BY_UE )
          {
            single_call_tid = srvcc_tid_table[tid_table_index].held_call_tid;
          }
          else if (srvcc_call_state == CM_CALL_STATE_CONV &&
            cahce_rpt_ptr->call_context[i].call_info.ip_call.call_sub_state.conv ==
            CM_CALL_CONV_HOLD_REQ_BY_UE)
          {
            single_call_tid = srvcc_tid_table[tid_table_index].conv_call_tid;
          }
          /* MO Alerting TID */
          else if (srvcc_call_state == CM_CALL_STATE_ORIG)
          {
            single_call_tid = srvcc_tid_table[tid_table_index].outgoing_call_tid;
          }

          call_context_t->individual_call_context[call_context_ind].\
            srvcc_call_connection_id=single_call_tid;

		  call_context_t->individual_call_context[call_context_ind].srvcc_ps_early_media_type =
			  cmwcall_set_alert_media_type(cahce_rpt_ptr->call_context[i].call_info.ip_call.call_alert_media_type);

          cm_set_connection_id(call_ptr,single_call_tid);

          cm_set_connection_id_src(call_ptr,0);

          call_context_ind++;

        }
        else
        {
          CM_MSG_HIGH_2 ("SRVCC unsupported for single calls,rejected IMS call id:,call_state:%d",
            cahce_rpt_ptr->call_context[i].call_info.ip_call.call_id,
            cahce_rpt_ptr->call_context[i].call_info.ip_call.call_state);
        }

      }

    }//for

    call_context_t->num_calls = call_context_ind;
    call_context_t->as_id     = cahce_rpt_ptr->as_id;
    cm_process_cmd_on_ims_cache(CMWCALL_CACHE_RECIEVED);

  }// Managable call present

  CM_MSG_HIGH_1 ("Total calls in SRVCC HO:,num_calls:%d",
    call_context_t->num_calls );

}

/*================================================c===========================

FUNCTION cmwcall_send_srvcc_events_to_clients

DESCRIPTION
  Sends HO events to clients for SRVCC FAIL/COMPLETE/CACNEL

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_send_srvcc_events_to_clients
(
 cm_srvcc_ho_ind_type  cmd,

 cm_call_id_type complete_ho_call_id,
 /*none for 3rd FAILURE IND */

  cm_call_id_type          ims_call_id,

  sys_modem_as_id_e_type   asubs_id
)
{

  cm_mm_call_info_s_type  *call_info_ptr = NULL;

  cmcall_s_type            *call_ptr = NULL;

  cm_call_id_type          fail_ho_call_id = CM_CALL_ID_INVALID ;

  if(ims_call_id == CM_CALL_ID_INVALID)
  {
    /* Notify clients list of call event with default call ptr. */
    if(cmd == CM_SRVCC_HO_IND_FAILURE )
    {
      #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
      if(cmwcall_is_int_ims() && 
       (cmcall_get_active_srvcc_volte_call(CM_CALL_TYPE_VOICE_MASK |
                                           CM_CALL_TYPE_EMERG_MASK  |
                                           CM_CALL_TYPE_VT_MASK,
                                           asubs_id) == CM_CALL_ID_INVALID))
      {
        CM_MSG_HIGH_0("INT IMS:Drop HO_FAIL/CANCEL for non midcall without active call ");
        return;
      }
      else
      #endif
      {
        call_info_ptr = cmcall_info_alloc();

        if ( call_info_ptr == NULL )
        {
          CM_MSG_HIGH_0("Ext IMS:CM_MM_SRVCC_HANDOVER_FAILURE_IND, can not alloc memory for call info object ");
          return;
        }

        cmclient_list_call_event_ntfy ( CM_CALL_EVENT_HO_FAIL, call_info_ptr );

        CM_MSG_HIGH_0("Ext IMS:CM_MM_SRVCC_HANDOVER_FAILURE_IND, Notify clients list of call event");

        /* Deallocate the memory after sending the call event. */
        cm_mem_free(call_info_ptr);
      }
    }

    else if (cmd == CM_SRVCC_HO_IND_CANCEL)
    {
      call_info_ptr = cmcall_info_alloc();

      if ( call_info_ptr == NULL )
      {
        CM_MSG_HIGH_0("Ext IMS:CM_MM_SRVCC_HANDOVER_CANCEL_IND, can not alloc memory for call info object ");
        return;
      }

      cmclient_list_call_event_ntfy ( CM_CALL_EVENT_HO_CANCEL, call_info_ptr );

      CM_MSG_HIGH_0("Ext IMS:CM_MM_SRVCC_HANDOVER_CANCEL_IND, Notify clients list of call event");

      cm_mem_free(call_info_ptr);
    }
   
   
    else if (cmd == CM_SRVCC_HO_IND_COMPLETE)
    {
      call_ptr = cmcall_ptr(complete_ho_call_id);

      cmcall_event ( call_ptr,CM_CALL_EVENT_HO_COMPLETE );//check whether empty of newly created srvcc call

      CM_MSG_HIGH_0("Ext IMS:CM_NAS_SRVCC_HANDOVER_COMPLETE_IND,Notify clients list of call event");
    }

  }

  else
  {
    /* for int IMS, send HO CANCEL or FAIL with existing ims call */

    if(cmd == CM_SRVCC_HO_IND_FAILURE || cmd == CM_SRVCC_HO_IND_CANCEL )
    {
       /* check for active VOLTE call with/without midcall  NV setting , Since same API
              used at HO_START, clients will receive only if HO_START is sent */
       #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
      fail_ho_call_id = cmcall_get_active_srvcc_volte_call(CM_CALL_TYPE_VOICE_MASK |
                                                           CM_CALL_TYPE_EMERG_MASK |
                                                           CM_CALL_TYPE_VT_MASK,
                                                           asubs_id);
      #endif
      call_ptr = cmcall_ptr(fail_ho_call_id);

      if( call_ptr == NULL )
      {
        CM_MSG_HIGH_0("INT IMS: Dont send SRVCC fail/cancel as midcall not supported and no active call ");
        return;
      }

      if(cmd == CM_SRVCC_HO_IND_FAILURE)
      {
        cmcall_event( call_ptr, CM_CALL_EVENT_HO_FAIL );
        CM_MSG_HIGH_0("CM_MM_SRVCC_HANDOVER_FAILURE_IND, Notify clients list of call event");
      }

      if(cmd == CM_SRVCC_HO_IND_CANCEL)
      {
        cmcall_event( call_ptr, CM_CALL_EVENT_HO_CANCEL );
        CM_MSG_HIGH_0("CM_MM_SRVCC_HANDOVER_CANCEL_IND, Notify clients list of call event");
      }

    }

    /* for Int IMS, send HO COMPLETE , Send event with first existing ims call */
    else if (cmd == CM_SRVCC_HO_IND_COMPLETE)
    {
      call_ptr = cmcall_ptr(complete_ho_call_id);

      cmcall_event( call_ptr, CM_CALL_EVENT_HO_COMPLETE );

      CM_MSG_HIGH_0("CM_NAS_SRVCC_HANDOVER_COMPLETE_IND,Notify clients list of call event");

    }
  }

}


/*===========================================================================

FUNCTION cmwcall_process_ims_audio_rel_cnf

DESCRIPTION
IMS cache ind initialize

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
none

===========================================================================*/
void cmwcall_process_ims_audio_rel_cnf(void)
{
  cm_process_cmd_on_ims_cache(cm_get_cache_ind().state);
}

#endif

#if defined(FEATURE_MMODE_GW) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
#ifdef FEATURE_WCDMA
/*===========================================================================

FUNCTION map_rrc_paging_cause_to_cmcall_type

DESCRIPTION
  CM receives RRC paging causes for when a CM_MT_CALL_PAGE_FAIL_IND
  is received from lower layer. The function maps the received RRC paging
  cause to cm call type

DEPENDENCIES
  None

RETURN VALUE
  CM Call Type

SIDE EFFECTS
  none

===========================================================================*/
static cm_call_type_e_type cmwcall_map_rrc_paging_cause_to_cmcall_type( rrc_paging_cause_e_type paging_cause )
{
   cm_call_type_e_type call_type;

   switch ( paging_cause )
   {
     case RRC_PAGE_CAUSE_TERM_CONV_CALL:
       call_type = CM_CALL_TYPE_VOICE;
       break;

     case RRC_PAGE_CAUSE_SMS:
       call_type = CM_CALL_TYPE_SMS;
       break;

     case RRC_PAGE_CAUSE_HIGH_PRI_SIGNALLING:
     case RRC_PAGE_CAUSE_LOW_PRI_SIGNALLING:
       call_type = CM_CALL_TYPE_SUPS;
       break;

     case RRC_PAGE_CAUSE_UNKNOWN:
       call_type = CM_CALL_TYPE_UNKNOWN;
       break;

     default:
       call_type = CM_CALL_TYPE_UNKNOWN;
       break;
       /* Returning CALL_TYPE_UNKNOWN for PS call pages
          because no mappin gin call type enum and NAS
          assures that these values would not be sent */
   }

   return call_type;
}
#endif
/*===========================================================================

FUNCTION map_failure_cause_to_call_end_status

DESCRIPTION
  CM receives failure causes when a CM_MT_CALL_PAGE_FAIL_IND is
  received from NAS layer. The function maps the received failure
  cause to cm call end type

DEPENDENCIES
  None

RETURN VALUE
  CM Call End Type

SIDE EFFECTS
  none

===========================================================================*/
static cm_call_end_e_type cmwcall_map_failure_cause_to_call_end_status( sys_modem_mt_call_fail_cause_type failure_cause )
{
   if ( failure_cause == CM_MM_MT_CALL_FAIL_CAUSE_CONNECTION_EST_FAILURE )
   {
      return CM_CALL_END_FAIL_CAUSE_CONNECTION_EST_FAILURE;
   }
   else if ( failure_cause == CM_MM_MT_CALL_FAIL_CAUSE_CONNECTION_FAILURE )
   {
     return CM_CALL_END_FAIL_CAUSE_CONNECTION_FAILURE;
   }
   else if ( failure_cause == CM_MM_MT_CALL_FAIL_CAUSE_NO_RESPONSE_FROM_NW )
   {
     return CM_CALL_END_FAIL_CAUSE_NO_RESPONSE_FROM_NW;
   }
   else if ( failure_cause ==   CM_MM_MT_CALL_FAIL_CAUSE_MT_CSFB_NO_RESPONSE_FROM_NW )
   {
     return CM_CALL_END_FAIL_CAUSE_MT_CSFB_NO_RESPONSE_FROM_NW;
   }
   else
   {
     return CM_CALL_END_FAIL_CAUSE_ESR_FAILURE;
   }
}
#endif
void cmwcall_call_control_complete_cb(

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
{
  
  mm_cmd_type                   *mm_ptr;
      /* MM command pointer */

  mm_ptr = mm_cm_get_cmd_buf(MMCM_CALL_CTRL_RSP);

  mm_ptr->cmd.cm_mm_call_ctrl_rsp.ps_rat = cmregprx_map_sd_mode_to_sys_rat(gw_call_control_info_ptr->ps_cc_sys_mode);

  if ((gw_call_control_info_ptr->call_control_result >=
       CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS) &&
      (gw_call_control_info_ptr->call_control_result <=
       CM_CC_RESULT_ALLOWED_BUT_MODIFIED))
  {
    if((gw_call_control_info_ptr->call_control_result ==
        CM_CC_RESULT_NOT_ALLOWED) &&
       (gw_call_control_info_ptr->error_cause ==
        CM_CALL_CMD_ERR_CAUSE_ACL_NO_MATCH))
    {
      mm_ptr->cmd.cm_mm_call_ctrl_rsp.call_control_result = ESM_MSG_REJECTED_ACL_FAILURE;
    }
    else
    {
      mm_ptr->cmd.cm_mm_call_ctrl_rsp.call_control_result = 
                  (pdn_conn_req_status_T)gw_call_control_info_ptr->call_control_result;;
    }
  }
  else
  {
    mm_ptr->cmd.cm_mm_call_ctrl_rsp.call_control_result = ESM_MSG_REJECTED;
    CM_MSG_HIGH_1("Invalid call_control_result %d",
                gw_call_control_info_ptr->call_control_result);
  }
  
  if(status == CM_CC_STATUS_SUCCESS )
  {
    if(gw_call_control_info_ptr->ps_cc_sys_mode!= SYS_SYS_MODE_GSM)
    {
      
      mm_ptr->cmd.cm_mm_call_ctrl_rsp.ps_params.lte_ps_params 
                  = gw_call_control_info_ptr->lte_ps_cc_params;
    }
    else
    {
      mm_ptr->cmd.cm_mm_call_ctrl_rsp.ps_params.gw_ps_params 
                  = gw_call_control_info_ptr->gw_ps_cc_params;
    }
  }
  else
  {
    /* call control status */
    mm_ptr->cmd.cm_mm_call_ctrl_rsp.call_control_result =
    ESM_MSG_REJECTED;
  }
  
  mm_put_cmd( mm_ptr );
  
}
/*===========================================================================

FUNCTION cmwcall_rpt_proc

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
void cmwcall_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */
)
{
  cm_rpt_type              *cm_rpt_ptr = (cm_rpt_type *) rpt_ptr;

  cm_call_id_type          call_id = CM_CALL_ID_INVALID;
      /* Call ID */

  cm_restore_psm_ctxt_req_s_type  *cm_restore_psm_ctxt_req = NULL;

  #if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE))
  boolean                  is_find_lte = FALSE;
  #endif

  #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
    /* Pointer to Lower Layer report */
  cmcall_s_type            *call_ptr;
      /* Point at call object */
  #endif
  

  boolean                 valid_conn_id = TRUE;


  boolean                 presentation_ind_absent = TRUE;
     /* Extension bit information for PI */


  byte                    screening_indicator = 0;    /* user provided, not screened */

  cm_mm_call_info_s_type  *call_info_ptr = NULL;

  cmph_s_type             *ph_ptr = cmph_ptr();
    /* Pointer to the phone object */

  #if !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
  cm_iterator_type        call_itr;
    /* Call Object Iterator */

  int                       i;

  ie_cm_notify_ss_T       *cm_notify_ss;
   /* Pointer to the Notify SS Indication */

  byte                    calling_party_num[CM_CALLED_PARTY_BCD_WITH_BCD_LEN];

  byte                    ascii_subaddress[CM_MAX_SUBADDRESS_DIGITS];

  gsm_wcdma_cmcall_type    *wcall_info_ptr;
      /* Point at wcdma call info */

  byte                    ton_npi;
     /* Type of Number, Numbering Plan */


  byte                    connected_num[CM_CONNECTED_NO_LENGTH];

  byte                    presentation_indicator = 0; /* presentation allowed */
     /* Presentation Indicator */

  byte                    ascii_num [CM_ASCII_MAX_NUMBER_CHARS];

  byte                    active_connection_id;

  byte                    call_index;

  byte                    pres_scren_ind;

  #endif
  
  boolean                 reject_mt_call = FALSE;
    /* Boolean checked to send PDP_ACTIVATE_REJ_RSP */

  cmcall_s_type           *mo_ps_call_ptr = NULL;
    /* Used to retreive a mo ps call in orig state */

  cm_ss_e_type            ss              = CM_SS_MAIN;
    /* stack on which this report is being processed */

  #if defined(FEATURE_CCBS)
  cm_5_send_conf_T        *ccbs_conf_ptr = NULL;
    /* Points to 5_send_conf received from MN */

  cm_mo_normal_call_req_T *recall_info_ptr = NULL;
    /* Points to recall information received */

  #endif /* FEATURE_CCBS */

  #if defined(FEATURE_MULTIMEDIA_FALLBACK)
  cm_modify_complete_conf_T  *mod_compl_ptr = NULL;
    /* Pointer to modify complete conf */
  #endif

  #ifdef FEATURE_GSTK

  cm_subscription_status_e_type    gw_subs_status =
                                          CM_SUBSCRIPTION_STATUS_NO_CHANGE;
      /* GW subscription status */

  cm_subscription_status_e_type    gw2_subs_status =
                                          CM_SUBSCRIPTION_STATUS_NO_CHANGE;
      /* hybr GW subscription status */


  cm_subscription_status_e_type    gw3_subs_status =
                                          CM_SUBSCRIPTION_STATUS_NO_CHANGE;
      /* hybr GW subscription status */

  cm_subscription_status_e_type    cdma_subs_status =
                                          CM_SUBSCRIPTION_STATUS_NO_CHANGE;
      /* CDMA subscription status */

  #endif /* FEATURE_GSTK */

  uint8                   num_len = 0;

  dword                   uptime = time_get_uptime_secs();

  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)

  cm_mid_srvcc_ho_comp_list_s_type *ho_comp_list = NULL;

  cm_orig_q_s_type* p_orig_emg = NULL;

  cm_mm_srvcc_call_info_s_type *srvcc_call_info_ptr = NULL;

  #if 0
  cm_orig_q_s_type *ph_orig_top_ptr ;
  #endif
  
  int num_srvcc_calls = 0;

  cm_call_id_type          active_call_id = CM_CALL_ID_INVALID;
      /* Call ID */

  cm_orig_q_s_type* p_orig = NULL;

  cm_orig_q_s_type* ph_p_orig=NULL;	
  
  #endif
  sys_modem_as_id_e_type ll_as_id = SYS_MODEM_AS_ID_1;

  #if defined(FEATURE_GPSONE) && defined(FEATURE_MMODE_GW) && defined(FEATURE_CDMA)
  cmpd_session_s_type *pd_ptr      = cmpd_ptr(CMPD_SESSION_OBJ_MAIN);
  cmpd_session_s_type *pd_dmod_ptr = cmpd_ptr(CMPD_SESSION_OBJ_DMOD);

  CM_ASSERT( pd_ptr      != NULL );
  CM_ASSERT( pd_dmod_ptr != NULL );
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_rpt_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do command specific processing as follows:
  ** 1. Update appropriate call object fields.
  ** 2. Modify the call state if necessary.
  ** 3. Notifying clients of event if necessary.
  */
  switch( cm_rpt_ptr->hdr.cmd )
  {

   
#if !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
    case CM_MO_CALL_CONF:
       if (!cm_find_call_id_per_subs(CONN_ID_MN,
                  cm_rpt_ptr->cmd.call_conf.connection_id,
                 &call_id,cm_rpt_ptr->cmd.call_conf.as_id) )
       {
         CM_ERR_0(" ERROR: Unknown Connection ID");
         break;
       }

       /* get the call pointer from the call ID */
       call_ptr = cmcall_ptr ( call_id );

       CM_ASSERT( call_ptr != NULL );

       wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

       wcall_info_ptr->success = cm_rpt_ptr->cmd.call_conf.cnm_service_success;

       wcall_info_ptr->cc_cause = cm_rpt_ptr->cmd.call_conf.cm_cc_cause;

       wcall_info_ptr->call_details = cm_rpt_ptr->cmd.call_conf.cm_mo_call_details;

       wcall_info_ptr->cc_reject   = cm_rpt_ptr->cmd.call_conf.cm_cc_reject;

       if (wcall_info_ptr->success == TRUE)
       {
         /*--------------------------------------------------------------*/

     #ifdef FEATURE_MMODE_SRVCC
          if(!call_ptr->is_srvcc_call)
     #endif 
         {
            if(cmss_get_true_sys_mode(call_ptr->ss) != SYS_SYS_MODE_NO_SRV)
            {
              call_ptr->sys_mode = cmss_get_true_sys_mode(call_ptr->ss);
            }
         }

         wcall_info_ptr->transaction_id = cm_rpt_ptr->cmd.call_conf.connection_id;

         #if defined (FEATURE_MULTIMEDIA_FALLBACK)

         /*--------------------------------------------------------------*/

         /* If BCs gets flipped in the call conf, call object flips its
         ** BC1 and BC2. Call type is recalculated.
         */
         cmwcall_update_call_comp_cap (call_ptr,
                                       &cm_rpt_ptr->cmd.call_conf);

         /* Invalidate HLC and LLC. Since modified HLC and LLC are not
         ** returned by network, these fields are invalidated until available
         ** from MN again.
         */
         /* cmwcall_invalidate_llc_hlc (wcall_info_ptr);       */

         /*--------------------------------------------------------------*/

         /* Set in-call modification field
         ** For a dual mode call setup (valid BC1 and BC2) network
         ** can accept only one of them. incall_modif field needs
         ** to be recalculated.
         */
         cmwcall_set_incall_modif_field (call_ptr);

         /*--------------------------------------------------------------*/

         #endif

         cmcall_event( call_ptr, CM_CALL_EVENT_CALL_CONF );

       }
       else
       {
         /*--------------------------------------------------------------*/

            /**< If we are trying to originate, indicate a RELEASE
            ** end status and call the origination function to decide
            ** whether to continue or abort the origination process.
            */
            call_ptr->end_status = CM_CALL_END_LL_CAUSE;

         
         #if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
         cmcall_orig_proc( call_ptr );
         #endif
       }

       break;

    #if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
    case CM_CALL_PROGRESS_INFO_IND:

       if (!cm_find_call_id_per_subs(CONN_ID_MN,
                  cm_rpt_ptr->cmd.prog_info.connection_id,
                 &call_id,cm_rpt_ptr->cmd.prog_info.as_id) )
       {
         CM_ERR_0(" ERROR: Unknown Connection ID");
         break;
       }

       /* get the call pointer from the call ID */
       call_ptr = cmcall_ptr ( call_id );

       CM_ASSERT( call_ptr != NULL );

       wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

       wcall_info_ptr->call_progress = cm_rpt_ptr->cmd.prog_info.call_progress;

       #ifdef FEATURE_MULTIMEDIA_FALLBACK

       /* Copies present flag for rab_id */
       wcall_info_ptr->rab_id_present = cm_rpt_ptr->cmd.prog_info.rab_id.present;

       /* Copies actual rabid */
       wcall_info_ptr->rab_id         =
                      cm_rpt_ptr->cmd.prog_info.rab_id.radio_access_bearer_id;

       #endif /*  FEATURE_MULTIMEDIA_FALLBACK */

       #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
       if (cmcall_is_gw_cs_call_type (call_ptr->call_type))
       {
         if( wcall_info_ptr->call_progress == USER_RESUMED )
         {
           wcall_info_ptr->inband_rcvd = TRUE;
         }
         else if( wcall_info_ptr->call_progress == BEARER_RELEASED )
         {
           /* Network can end the traffic channel for a voice call but
           ** may keep the signaling up for a considerable time. NAS
           ** sends a call end only when MM connection for the cs call has
           ** been torn down. There is a new requirement to end the call
           ** as soon as traffic bearer gets released.
           */

           CM_MSG_HIGH_1 ("Sending end_req for call %d, bearer rel recvd",
                        call_id);

	   call_ptr->end_status = CM_CALL_END_NETWORK_END;

           /* Inform clients of call being ended */
           cmwcall_end_req (call_ptr);

           /* Send end request to MN */
           cmwcall_send_cs_end_req (cm_rpt_ptr->cmd.prog_info.connection_id,
                                    call_ptr->ss);
         }

          /* For MT incall modification
         ** CM <---- MN MT_MODIFY_IND (Call modif to other call type)
         ** CM ----> MN MT_MODIFY_RES (user accepts)
         ** CM <---  MN PROGRESS_INFO (BEARER_CHANGED) call type actually
         **          changes.
         ** For MO modification
         ** -------------------
         ** Order of BEARER_CHANGED or MODIFY_COMPLETE_CONF is not guaranteed.
         ** Whichever comes first, do the following...
         **
         **  1> Swap BC1, BC2.
         **  2> Changes call_type to reflect the new BC
         **  3> Resets new_call_type_req
         **  4> Copies BC1 only in Modify complete conf from RPT.
         **  5> BEARER_CHANGED should also provide final BC ???
         */
         if ( ( wcall_info_ptr->call_progress == BEARER_CHANGED )      &&
              (wcall_info_ptr->new_call_type_req != CM_CALL_TYPE_NONE) &&
              (wcall_info_ptr->new_call_type_req != call_ptr->call_type)
            )
         {
           #if defined(FEATURE_MULTIMEDIA_FALLBACK)
           cmwcall_swap_bc_llc_hlc (call_ptr);

           call_ptr->call_type = cmwcall_determine_call_type
                                  (call_ptr, &wcall_info_ptr->bearer_capability_1);
           #endif

           wcall_info_ptr->new_call_type_req = CM_CALL_TYPE_NONE;
         }

         cmcall_event( call_ptr, CM_CALL_EVENT_PROGRESS_INFO_IND );

       }
       #endif

    break;
    #endif

/**************************************************************/
    case CM_MO_CALL_ALERT_IND:

      if( !cm_find_call_id_per_subs (CONN_ID_MN,
      cm_rpt_ptr->cmd.call_alert_ind.connection_id,
      &call_id,cm_rpt_ptr->cmd.call_alert_ind.as_id) )

      {
        CM_ERR_0( " ERROR: Unknown Connection ID");
        break;
      }

      /* get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( call_id );

      CM_ASSERT( call_ptr != NULL );

      /* Re-using CALL EVENT ABRV ALERT for this GW case (earlier only applicable for CDMA)*/
      cmcall_event( call_ptr, CM_CALL_EVENT_ABRV_ALERT );

      break;



/**************************************************************/


    case CM_CALL_CONNECTED_IND:
       if (!cm_find_call_id_per_subs(CONN_ID_MN,
                      cm_rpt_ptr->cmd.call_connected.connection_id,
                     &call_id,cm_rpt_ptr->cmd.call_connected.as_id) )
       {
         CM_ERR_0(" ERROR: Unknown Connection ID");
         break;
       }

       /* get the call pointer from the call ID */
       call_ptr = cmcall_ptr ( call_id );

       CM_ASSERT( call_ptr != NULL );

       wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

       wcall_info_ptr->rab_id = cm_rpt_ptr->cmd.call_connected.radio_access_bearer_id;

       /* Sets rab_id present to always TRUE */
       wcall_info_ptr->rab_id_present = TRUE;

       call_ptr->call_state = CM_CALL_STATE_CONV;

       call_ptr->call_subst.conv = CM_CALL_CONV_CALL_ACTIVE;

       #ifdef FEATURE_MMODE_SRVCC
       if(!call_ptr->is_srvcc_call)
       #endif
       {
         if(cmss_get_true_sys_mode(call_ptr->ss) != SYS_SYS_MODE_NO_SRV)
         {
           call_ptr->sys_mode = cmss_get_true_sys_mode(call_ptr->ss);
         }
       }



       call_ptr->call_connect_time = time_get_uptime_secs();
       cmcall_event( call_ptr, CM_CALL_EVENT_CONNECT );

       #if defined(FEATURE_GPSONE) && defined(FEATURE_MMODE_CDMA)
       /* Check if we need to force the mode for the PD session
       */
       if( !pd_ptr->is_mode_forced                                 &&
           ( pd_ptr->session_state == CMPD_SESSION_STATE_INITIATED ||
             pd_ptr->session_state == CMPD_SESSION_STATE_GRANTED
           ))
       {
         cmpd_session_grant_proc( pd_ptr->session_type );
       }

       if( !pd_dmod_ptr->is_mode_forced                                 &&
           ( pd_dmod_ptr->session_state == CMPD_SESSION_STATE_INITIATED ||
             pd_dmod_ptr->session_state == CMPD_SESSION_STATE_GRANTED
           ))
       {
         cmpd_session_grant_proc( pd_dmod_ptr->session_type );
       }
       #endif

       cmcall_update_gsdi_and_log_call_state(call_ptr->ss);
       break;

    #if !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
    case CM_MT_SETUP_IND:

      if( !cm_new_incom_call_id ( CM_CALL_TYPE_VOICE, &call_id,
                                  cm_rpt_ptr->cmd.setup_ind.connection_id) )
      {
         CM_ERR_0( "ERROR: Unknown connection ID in setup ind");
         break;
      }

      /* get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( call_id );

      CM_ASSERT( call_ptr != NULL );

      /* Assign the SS of the call based on as_id
         */
      call_ptr->asubs_id = cmph_get_as_id_from_ll_ind(cm_rpt_ptr->cmd.setup_ind.as_id);
      call_ptr->ss = cmph_determine_ss_per_domain(call_ptr->asubs_id,SYS_SRV_DOMAIN_CS_ONLY);
        #ifdef FEATURE_MMODE_HYBR3_ENABLED
        CM_ASSERT( call_ptr->ss == CM_SS_MAIN || call_ptr->ss == CM_SS_HYBR_2 || call_ptr->ss == CM_SS_HYBR_3 )
        #else
        CM_ASSERT( call_ptr->ss == CM_SS_MAIN || call_ptr->ss == CM_SS_HYBR_2 )
        #endif
      CM_ASSERT_ENUM_IS_INRANGE(call_ptr->asubs_id, SYS_MODEM_AS_ID_MAX);

      call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_CS;
      wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

      wcall_info_ptr->bearer_capability_1 = cm_rpt_ptr->cmd.setup_ind.bearer_capability_1;
      wcall_info_ptr->bearer_capability_2 = cm_rpt_ptr->cmd.setup_ind.bearer_capability_2;

      /*----------------------------------------------------------*/
      /*  Determine Call Type here                                */
      /*----------------------------------------------------------*/

      /* For a MT setup indication
      **
      ** - If there is BC1=NONE and BC2=NONE, call type is voice
      ** - If there is BC1=Present and BC2=NONE, call type based on BC1.
      ** - If there is BC1=Present and BC2=Present, call type based on BC1.
      ** - If there is BC1=None and BC2=Present, this cannot happen, MN
      **   guarentees that this scenario will not happen.
      */
      #if defined (FEATURE_MULTIMEDIA_FALLBACK)

      call_ptr->call_type = cmwcall_determine_call_type (call_ptr,
                                                         &wcall_info_ptr->bearer_capability_1);

      /* Determine if in-call modification is possible
      */
      cmwcall_set_incall_modif_field (call_ptr);

      #else

      if (((wcall_info_ptr->bearer_capability_1.present) &&
          (wcall_info_ptr->bearer_capability_1.information_transfer_capability
              != ITC_SPEECH)
          #if (defined FEATURE_ALS) && (defined FEATURE_GSM)
          &&
          (wcall_info_ptr->bearer_capability_1.information_transfer_capability
              != cmph_ptr()->itc_aux_val_to_use
              )
          #endif
          )
                    ||
          ((wcall_info_ptr->bearer_capability_2.present) &&
          (wcall_info_ptr->bearer_capability_2.information_transfer_capability
              != ITC_SPEECH)
          #if (defined FEATURE_ALS) && (defined FEATURE_GSM)
          &&
          (wcall_info_ptr->bearer_capability_2.information_transfer_capability
              != cmph_ptr()->itc_aux_val_to_use
              )
          #endif
          ))
      {
        call_ptr->call_type = CM_CALL_TYPE_CS_DATA;
      }
      else
      {
        call_ptr->call_type = CM_CALL_TYPE_VOICE;
      }

      #endif

      /* If both BC1 and BC2 are found missing (single numbering scheme), MS is
      ** supposed to send OTA �call confirmed� message with the supported BC�s
      ** to the network. At CM level, just mark the call type as CM_CALL_TYPE_CS_DATA
      */
      if(( CMPH_SS_DISABLE_CALL_TYPE_MASK(call_ptr->asubs_id, ph_ptr) & CM_CALL_TYPE_VOICE_MASK ) &&
         (wcall_info_ptr->bearer_capability_1.present == FALSE) &&
         (wcall_info_ptr->bearer_capability_2.present == FALSE)
        )
      {
        call_ptr->call_type = CM_CALL_TYPE_CS_DATA;
        CM_MSG_HIGH_0("Both BC's missing-declaring call type as CS_DATA");
      }
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

      #if (defined FEATURE_ALS) && (defined FEATURE_GSM)
      /*
      ** For voice calls, look at fields in CM_MT_SETUP_IND,
      ** map them to line on which the call is
      ** to be received, set call_ptr->line to that line.
      ** For all other types of calls the default line will be used
      ** (call object is initialized with default line)
      */
      if(cmph_als_supported() && call_ptr->call_type == CM_CALL_TYPE_VOICE)
      {
        cm_als_line_e_type line;

        line = cmals_ptr_mmgsdi()->resolve_mt_call_ind_func( cm_rpt_ptr );

        call_ptr->line = line;
      }
      #endif

      /* Check if voice call is allowed, if not then block it. there are
      ** no emergency MT calls the check added is a safety check, in case
      ** the MT call comes with call type emergency
      */
      if( (( CMPH_SS_DISABLE_CALL_TYPE_MASK(call_ptr->asubs_id, ph_ptr) & CM_CALL_TYPE_VOICE_MASK ) &&
            ( call_ptr->call_type == CM_CALL_TYPE_VOICE ||
              call_ptr->call_type == CM_CALL_TYPE_EMERGENCY ))
                                              ||
           ( ( CMPH_SS_DISABLE_CALL_TYPE_MASK(call_ptr->asubs_id, ph_ptr) & CM_CALL_TYPE_CS_DATA_MASK )&&
            ( call_ptr->call_type == CM_CALL_TYPE_CS_DATA ))
        )
      {
        ie_cm_cc_cause_T             cause;

        cause.present                = TRUE;
        cause.coding_standard        = STD_CODESTANDARD;
        cause.location               = STD_LOCATION;
        cause.recommendation_present = FALSE;
        cause.diagnostic_length      = 0;
        cause.cause_value            = INCOMPATIBLE_DESTINATION;
        cmwcall_process_mt_setup_response
           ( call_ptr,
             FALSE,
             &cause,
             &cm_rpt_ptr->cmd.setup_ind.bc_repeat_indicator,
             &cm_rpt_ptr->cmd.setup_ind.bearer_capability_1,
             &cm_rpt_ptr->cmd.setup_ind.bearer_capability_2
           );

        /*
        ** Since the clients are not aware of this setup ind, do not send the
        ** CM_CALL_EVENT_END when the CM_MT_END_CALL_IND is received from the
        ** lower layers.
        **/
        call_ptr->send_call_event_end = FALSE;
        CM_MSG_HIGH_0( "Rejecting CM_MT_SETUP_IND");

        /* free the call ID */
        cm_call_id_deallocate ( cmcall_call_id_get( call_ptr ) );

        /* Break here since client should not get intimated about this
         ** MT call via CALL_EVENT
        */
        break;

      }

      /* look at the call type before forwarding the incoming
      ** call notification to the clients.
      */
      if ( !cmwcall_check_if_mt_call_possible( call_ptr ) )
      {
        /* send a setup_res command with accept = FALSE to reject
        ** the incoming CS data call
        */
        ie_cm_cc_cause_T             cause;

        cause.present                = TRUE;
        cause.coding_standard        = STD_CODESTANDARD;
        cause.location               = STD_LOCATION;
        cause.recommendation_present = FALSE;
        cause.diagnostic_length      = 0;
        cause.cause_value            = USER_BUSY;

        cmwcall_process_mt_setup_response
           ( call_ptr,
             FALSE,
             &cause,
             &cm_rpt_ptr->cmd.setup_ind.bc_repeat_indicator,
             &cm_rpt_ptr->cmd.setup_ind.bearer_capability_1,
             &cm_rpt_ptr->cmd.setup_ind.bearer_capability_2
           );

        /*
        ** Since the clients are not aware of this setup ind, do not send the
        ** CM_CALL_EVENT_END when the CM_MT_END_CALL_IND is received from the
        ** lower layers.
        **/
        call_ptr->send_call_event_end = FALSE;
        CM_MSG_HIGH_0( "Rejecting CM_MT_SETUP_IND");
        cm_call_id_deallocate ( cmcall_call_id_get( call_ptr ) );

        break;
      }

      call_ptr->call_state = CM_CALL_STATE_INCOM;
      call_ptr->call_subst.incom = CM_CALL_INCOM_SUBST_SETUP;
      CM_MSG_HIGH_1("incom subst: %d", call_ptr->call_subst.incom);

      cmcall_set_call_direction (call_ptr, CM_CALL_DIRECTION_MT);

      wcall_info_ptr->transaction_id = cm_rpt_ptr->cmd.setup_ind.connection_id;
      wcall_info_ptr->bc_repeat_indicator = cm_rpt_ptr->cmd.setup_ind.bc_repeat_indicator;
      wcall_info_ptr->llc_repeat_indicator = cm_rpt_ptr->cmd.setup_ind.llc_repeat_indicator;
      wcall_info_ptr->low_layer_compatibility_1 = cm_rpt_ptr->cmd.setup_ind.low_layer_compatibility_1;
      wcall_info_ptr->low_layer_compatibility_2 = cm_rpt_ptr->cmd.setup_ind.low_layer_compatibility_2;
      wcall_info_ptr->hlc_repeat_indicator = cm_rpt_ptr->cmd.setup_ind.hlc_repeat_indicator;
      wcall_info_ptr->high_layer_compatibility_1 = cm_rpt_ptr->cmd.setup_ind.high_layer_compatibility_1;
      wcall_info_ptr->high_layer_compatibility_2 = cm_rpt_ptr->cmd.setup_ind.high_layer_compatibility_2;
      wcall_info_ptr->called_party_bcd_number = cm_rpt_ptr->cmd.setup_ind.cm_called_party_bcd_number;
      wcall_info_ptr->called_party_subaddress = cm_rpt_ptr->cmd.setup_ind.cm_called_party_subaddress;
      wcall_info_ptr->calling_party_bcd_number = cm_rpt_ptr->cmd.setup_ind.cm_calling_party_bcd_number;
      wcall_info_ptr->calling_party_subaddress = cm_rpt_ptr->cmd.setup_ind.cm_calling_party_subaddress;
      wcall_info_ptr->cause_of_no_cli = cm_rpt_ptr->cmd.setup_ind.cm_cause_of_no_CLI;
      wcall_info_ptr->alerting_pattern = cm_rpt_ptr->cmd.setup_ind.cm_alerting_pattern;

      if( wcall_info_ptr->calling_party_bcd_number.present )
      {
        /* convert the bcd number to ascii
        */
        memset( calling_party_num, 0, CM_CALLING_PARTY_BCD_NO_LENGTH );

        ton_npi = wcall_info_ptr->calling_party_bcd_number.data[0];

        call_ptr->num.number_plan = (byte)(ton_npi & CM_NUM_PLAN_MASK);

        call_ptr->num.number_type = (byte)((ton_npi & CM_TON_MASK) >> 4);


        /* look for the presentation indicator according to 24.008 Calling party BCD number definition*/
        presentation_ind_absent = (boolean)((ton_npi & CM_EXTENSION_BIT) >> 7);

        if ( !presentation_ind_absent )
        {
          /* check extra byte octet 3a */
          pres_scren_ind = wcall_info_ptr->calling_party_bcd_number.data[1];

          presentation_indicator =
               (byte)((pres_scren_ind & CM_PRESENTATION_IND_MASK) >> 5);

          screening_indicator = (byte) (pres_scren_ind & CM_SCREENING_IND_MASK);
        }


        calling_party_num[0] = wcall_info_ptr->calling_party_bcd_number.length;

        if ( calling_party_num[0] > CM_CALLING_PARTY_BCD_NO_LENGTH )
        {
          calling_party_num[0] = CM_CALLING_PARTY_BCD_NO_LENGTH;
        }

        memscpy( &calling_party_num[1],
                 sizeof(calling_party_num),
                 wcall_info_ptr->calling_party_bcd_number.data,
                 calling_party_num[0] );

        cm_util_bcd_num_to_ascii_num( calling_party_num, ascii_num, sizeof(ascii_num) );

        num_len =  (byte )strlen((char *)ascii_num);

        if ( ascii_num[0] != 0 &&
       !(num_len == 1 && ascii_num[0] == '+'))//avoid sending just '+' when network sends zero length
                                                    // number buffer for international number type.
        {
          call_ptr->num.len = memscpy( call_ptr->num.buf, sizeof( call_ptr->num.buf ), ascii_num, num_len );
        }

        call_ptr->num.pi = presentation_indicator;
        call_ptr->num.si = screening_indicator;
      }

      if( cm_rpt_ptr->cmd.setup_ind.cm_redirect_party_bcd_number.present )
      {
        /* convert the bcd number to ascii */

        memset( calling_party_num, 0, CM_CALLING_PARTY_BCD_NO_LENGTH );

        ton_npi = cm_rpt_ptr->cmd.setup_ind.cm_redirect_party_bcd_number.data[0];

        wcall_info_ptr->redirect_party_number.number_plan = (byte)(ton_npi & CM_NUM_PLAN_MASK);

        wcall_info_ptr->redirect_party_number.number_type = (byte)((ton_npi & CM_TON_MASK) >> 4);


        /* look for the presentation indicator according to 24.008 Calling party BCD number definition*/
        presentation_ind_absent = (boolean)((ton_npi & CM_EXTENSION_BIT) >> 7);

        if ( !presentation_ind_absent )
        {
          /* check the extra byte octet 3a*/
          pres_scren_ind = cm_rpt_ptr->cmd.setup_ind.cm_redirect_party_bcd_number.data[1];

          presentation_indicator =
               (byte)((pres_scren_ind & CM_PRESENTATION_IND_MASK) >> 5);

          screening_indicator = (byte) (pres_scren_ind & CM_SCREENING_IND_MASK);

        }


        calling_party_num[0] = cm_rpt_ptr->cmd.setup_ind.cm_redirect_party_bcd_number.length;

        if ( calling_party_num[0] > CM_CALLING_PARTY_BCD_NO_LENGTH )
        {
          calling_party_num[0] = CM_CALLING_PARTY_BCD_NO_LENGTH;
        }

        memscpy( &calling_party_num[1],
                 sizeof(calling_party_num),
                 cm_rpt_ptr->cmd.setup_ind.cm_redirect_party_bcd_number.data,
                 calling_party_num[0] );

        cm_util_bcd_num_to_ascii_num( calling_party_num, ascii_num, sizeof(ascii_num) );

  num_len =  (byte )strlen((char *)ascii_num);

        if ( ascii_num[0] != 0 &&
             !(num_len == 1 && ascii_num[0] == '+'))//avoid sending just '+' when network sends zero length
                                                    // number buffer for international number type.
        {
            wcall_info_ptr->redirect_party_number.len = memscpy( wcall_info_ptr->redirect_party_number.buf,
                                                                 sizeof( wcall_info_ptr->redirect_party_number.buf ),
                                                                 ascii_num,
                                                                 num_len ) ;
        }

        wcall_info_ptr->redirect_party_number.pi = presentation_indicator;

        wcall_info_ptr->redirect_party_number.si = screening_indicator;

      }

      if( cm_rpt_ptr->cmd.setup_ind.cm_redirect_party_subaddress.present )
      {
        memset ( ascii_subaddress, 0, sizeof(ascii_subaddress));

        cm_util_subaddr_to_ascii_num (
                   cm_rpt_ptr->cmd.setup_ind.cm_redirect_party_subaddress.data,
                   cm_rpt_ptr->cmd.setup_ind.cm_redirect_party_subaddress.length,
                   ascii_subaddress,
                   sizeof(ascii_subaddress));

        if( ascii_subaddress[0] != '\0' )
        {
          byte subaddr_len = (byte) strlen((char *) ascii_subaddress );
          if (subaddr_len > sizeof(wcall_info_ptr->redirect_party_number.subaddr.chari))
          {
            subaddr_len = sizeof(wcall_info_ptr->redirect_party_number.subaddr.chari);
          }
          memscpy( wcall_info_ptr->redirect_party_number.subaddr.chari,
                   sizeof(wcall_info_ptr->redirect_party_number.subaddr.chari),
                   ascii_subaddress,
                   subaddr_len );

          wcall_info_ptr->redirect_party_number.subaddr.num_char = subaddr_len;
        }
      }

      {
        call_ptr->send_call_event_end = TRUE;
        if(cmss_get_true_sys_mode(call_ptr->ss))
        {
          call_ptr->sys_mode = cmss_get_true_sys_mode(call_ptr->ss);
        }
        cmcall_event( call_ptr, CM_CALL_EVENT_SETUP_IND );
      }

      if (cmcall_overall_call_state_per_subs(call_ptr->ss) == CM_OVERALL_CALL_STATE_INCOM )
      {
        #ifdef FEATURE_MMODE_HYBR3_ENABLED
        if( call_ptr->ss == CM_SS_HYBR_3 )
        {
          call_ptr->ph_ptr->hybr_3_stack_info.phone_charge.CCM = 0;
          call_ptr->ph_ptr->hybr_3_stack_info.phone_charge.last_CCM_to_update_ACM = 0;
        }
        else
        #endif
        #if defined(FEATURE_MMODE_DUAL_SIM)
        if(cmph_is_msim() &&
           call_ptr->ss == CM_SS_HYBR_2 )
        {
          call_ptr->ph_ptr->hybr_2_stack_info.phone_charge.CCM = 0;
          call_ptr->ph_ptr->hybr_2_stack_info.phone_charge.last_CCM_to_update_ACM = 0;
        }
        else
        #endif
		#if defined(FEATURE_MMODE_3GPP) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)  && !defined (FEATURE_MMODE_LTE_ONLY) && !defined(FEATURE_MMODE_GPRS_LTE)
        {
          call_ptr->ph_ptr->main_stack_info.phone_charge.CCM = 0;
          call_ptr->ph_ptr->main_stack_info.phone_charge.last_CCM_to_update_ACM = 0;
        }
		#endif

        cmcall_update_gsdi_and_log_call_state(call_ptr->ss);
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*------------------------------*/
                     /*      Incoming call           */
                     /*------------------------------*/

    case CM_MT_CALL_IND:

      if (!cm_find_call_id_per_subs(CONN_ID_MN,
                     cm_rpt_ptr->cmd.call_ind.connection_id,
                    &call_id,cm_rpt_ptr->cmd.call_ind.as_id) )
      {
        CM_ERR_0(" ERROR: Unknown Connection ID");
        break;
      }

      /* get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( call_id );

      CM_ASSERT( call_ptr != NULL );

      wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;
      wcall_info_ptr->transaction_id = cm_rpt_ptr->cmd.call_ind.connection_id;

      /* Call state should be incoming and subst should be setup_processed
      ** for alerting to be processed.
      */
      if( (call_ptr->call_state       == CM_CALL_STATE_INCOM) &&
          (call_ptr->call_subst.incom == CM_CALL_INCOM_SUBST_SETUP_PROCESSED)
        )
      {
        call_ptr->call_subst.incom = CM_CALL_INCOM_SUBST_ALERTING;

        if(cmss_get_true_sys_mode(call_ptr->ss))
        {
          call_ptr->sys_mode = cmss_get_true_sys_mode(call_ptr->ss);
        }

        cmph_call_start( call_ptr );

        /* signal/tone information is sent to UI in CM_CALL_EVENT_INCOM.
           If there is a valid signal information, UI plays the tone
           accordingly. Other UI plays the default tones.
           A separate CM_CALL_EVENT_SIGNAL is not sent to UI */

        if(cm_rpt_ptr->cmd.call_ind.signal.present)
        {
          call_ptr->signal.is_signal_info_avail = TRUE;
          call_ptr->signal.signal = cm_rpt_ptr->cmd.call_ind.signal.signal_value;
          call_ptr->signal.signal_type = (byte )CM_SIGNAL_NETWORK_ALERT;
        }
        else
        {
          call_ptr->signal.is_signal_info_avail = FALSE;
        }

        (void)cmph_update_orig_param_incoming_call(call_ptr);


        cmcall_event( call_ptr, CM_CALL_EVENT_INCOM );

      }
      else
      {

        CM_ERR_2( "Rcvd CALL_IND out of seq. state %d, substate %d",
                 call_ptr->call_state, call_ptr->call_subst.incom);

        call_ptr->end_status = CM_CALL_END_INCOM_CALL;
        #if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
        cmcall_end( call_ptr );
        #endif

      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_MT_DISC_IND:

       if (!cm_find_call_id_per_subs(CONN_ID_MN,
                      cm_rpt_ptr->cmd.disc_ind.connection_id,
                     &call_id,cm_rpt_ptr->cmd.disc_ind.as_id) )
       {
         CM_ERR_0(" ERROR: Unknown Connection ID");
         break;
       }

      /* get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( call_id );

      wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

      wcall_info_ptr->cc_cause = cm_rpt_ptr->cmd.disc_ind.cm_cc_cause;

      wcall_info_ptr->inband_tone_present = cm_rpt_ptr->cmd.disc_ind.inband_tone_present;

      CM_MSG_HIGH_1( "ICM_MT_DISC_IND inband_tone_present = %d", cm_rpt_ptr->cmd.disc_ind.inband_tone_present);

      CM_ASSERT( call_ptr != NULL );

      /**< If we are trying to originate, indicate a RELEASE
      ** end status and call the origination function to decide
      ** whether to continue or abort the origination process.
      */
      call_ptr->end_status = CM_CALL_END_NETWORK_END;

      if (call_ptr->direction == CM_CALL_DIRECTION_MT ||
         call_ptr->call_state != CM_CALL_STATE_ORIG)
      {
        cmwcall_end_req (call_ptr);
      }
      else
      {
        #if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
        cmcall_orig_proc(call_ptr);
        #endif
      }

      break;
    #endif

    case CM_MT_END_CALL_IND:

      /* Decide on the action according to the call state.
      */
      /* copy the information record */

       if (!cm_find_call_id_per_subs(CONN_ID_MN,
                      cm_rpt_ptr->cmd.end_call_ind.connection_id,
                     &call_id,cm_rpt_ptr->cmd.end_call_ind.as_id) )
       {
         CM_MSG_HIGH_0(" ERROR: Unknown Connection ID");
         break;
       }

      /* get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( call_id );

      CM_ASSERT( call_ptr != NULL );

      wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

      wcall_info_ptr->cc_cause = cm_rpt_ptr->cmd.end_call_ind.cm_cc_cause;

      wcall_info_ptr->cc_reject = cm_rpt_ptr->cmd.end_call_ind.cm_cc_reject;

      if ((call_ptr->call_state !=  CM_CALL_STATE_IDLE) &&
      (call_ptr->call_state !=  CM_CALL_STATE_CC_IN_PROGRESS))
      {
        if ((wcall_info_ptr->cc_reject.present== TRUE ) &&
              (wcall_info_ptr->cc_reject.rej_value== AS_REJ_DEEP_FADE ))
        {
          call_ptr->end_status = CM_CALL_END_FADE;
        }
        else
        {
          if((call_ptr->end_status!= CM_CALL_END_CLIENT_END) && (call_ptr->end_status!=CM_CALL_END_NETWORK_END)&& (call_ptr->end_status!=CM_CALL_END_NO_FUNDS))
          {
            call_ptr->end_status = CM_CALL_END_LL_CAUSE;
            CM_MSG_HIGH_1("Call ended because of lower layer cause  call_id = %d",call_ptr->call_id);
          }
        }
      }
      #if (!defined(FEATURE_MMODE_DISABLE_LTE_VOICE) || defined(FEATURE_MMODE_GPRS_LTE))
      switch( call_ptr->call_state )
      {

        case CM_CALL_STATE_IDLE:
          if (call_ptr->call_subst.idle == CM_CALL_IDLE_SUBST_ENDWAIT)
          {
            cmcall_end(call_ptr);
          }
          /* Client released already */
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_STATE_CC_IN_PROGRESS:
          /* Call request was not sent to nas yet
          ** so ignore the report
          */
          CM_ERR_1(" ERROR: CM_MT_END_CALL_IND received at wrong state, call_state=%d",
                 (uint8) call_ptr->call_state);

          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case CM_CALL_STATE_ORIG:

      /**< If we are trying to originate, indicate a RELEASE
      ** end status and call the origination function to decide
      ** whether to continue or abort the origination process.
      ** If  Ext IMS SRVCCed call in ORIG state is ended by network,
      ** no need to attempt origination
      */
      #ifdef FEATURE_MMODE_SRVCC
      if(call_ptr->is_srvcc_call)
      {
            cmcall_end( call_ptr );
      }
      else
      {
        cmcall_orig_proc(call_ptr);
      }
      #else
          cmcall_orig_proc(call_ptr);
      #endif

          break;
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_STATE_INCOM:
          /* Call was never established -
          ** simply end the call.
          */
          cmcall_end( call_ptr );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_STATE_CONV:
          cmcall_end(call_ptr);
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case CM_CALL_STATE_RECALL_RSP_PEND:
          cmcall_end(call_ptr);

          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        default:

          /*lint -save -e571 Suspicious cast */
          CM_ERR_1( "CM_CALL_RELEASE_F, call_state=%d",
                         (uint8) call_ptr->call_state);
          /*lint -restore */

      }
      #endif
      break;

    #if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
    case CM_MT_CALL_PAGE_FAIL_IND:

      /* Handle Call Setup failure case after Page is received but before
         call setup is completed */
      

      /* special processing required since this message does not
         have a specific connection ID */

      /* Allocate a call info buffer */
      call_info_ptr = cmcall_info_alloc();

      CM_MSG_HIGH_2( "RXD: CM_MT_CALL_PAGE_FAIL_IND rat %d, fail cause %d, rrc page cause %d", cm_rpt_ptr->cmd.call_fail_after_page_info.rat,
                   cm_rpt_ptr->cmd.call_fail_after_page_info.mt_call_fail_cause);



      /* Map Failure Cause to CM Call End Status */
      call_info_ptr->end_status = cmwcall_map_failure_cause_to_call_end_status( cm_rpt_ptr->cmd.call_fail_after_page_info.mt_call_fail_cause );

      #ifdef FEATURE_WCDMA
      switch ( cm_rpt_ptr->cmd.call_fail_after_page_info.rat )
      {

        case SYS_RAT_UMTS_RADIO_ACCESS:
        case SYS_RAT_TDS_RADIO_ACCESS:
          call_info_ptr->mode_info.info_type = CM_CALL_MODE_INFO_GW_CS;
          call_info_ptr->mode_info.info.gw_cs_call.rrc_paging_cause = cm_rpt_ptr->cmd.call_fail_after_page_info.paging_cause;
          break;

        default:
          CM_MSG_HIGH_0( "Page received on unsupported rat");
          break;
      }
	  
      /* Map rrc paging cause to CM Call Type */
      call_info_ptr->call_type = cmwcall_map_rrc_paging_cause_to_cmcall_type( cm_rpt_ptr->cmd.call_fail_after_page_info.paging_cause );
      #endif
	  
      cmclient_list_call_event_ntfy( CM_CALL_EVENT_MT_CALL_PAGE_FAIL, call_info_ptr );
      cm_mem_free( call_info_ptr );

      break;
    #endif

    case CM_CONNECTED_NUMBER_IND:

        /* Notify clients of event.
        */
        if (!cm_find_call_id_per_subs(CONN_ID_MN,
                      cm_rpt_ptr->cmd.connected_num.connection_id,
                      &call_id,cm_rpt_ptr->cmd.connected_num.as_id) )
        {
          CM_ERR_0(" ERROR: Unknown Connection ID");
          break;
        }

        /* get the call pointer from the call ID */
        call_ptr = cmcall_ptr ( call_id );

        CM_ASSERT( call_ptr != NULL );

        wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

        wcall_info_ptr->connected_num = cm_rpt_ptr->cmd.connected_num.cm_connected_number;
        wcall_info_ptr->transaction_id = cm_rpt_ptr->cmd.connected_num.connection_id;


        wcall_info_ptr->connected_subaddress = cm_rpt_ptr->cmd.connected_num.cm_connected_subaddress;

        if ( wcall_info_ptr->connected_num.present )
        /* format the number for display */
        {
          memset ( connected_num, 0, CM_CONNECTED_NO_LENGTH );

          ton_npi = wcall_info_ptr->connected_num.data[0];

          call_ptr->num.number_plan = (byte)(ton_npi & CM_NUM_PLAN_MASK);

          call_ptr->num.number_type = (byte)((ton_npi & CM_TON_MASK) >> 4 );

          /* look for the presentation indicator according to 24.008 Calling party BCD number definition*/
          presentation_ind_absent = (boolean)((ton_npi & CM_EXTENSION_BIT) >> 7);

          if (!presentation_ind_absent)
          {
            /* check extra byte octet 3a - presentation/screening indicator */
            pres_scren_ind =
              wcall_info_ptr->connected_num.data[1];

             presentation_indicator = (byte)((pres_scren_ind & CM_PRESENTATION_IND_MASK) >> 5);

             screening_indicator = (byte)( pres_scren_ind & CM_SCREENING_IND_MASK );

          }

          /* Copy connected number data to temp buffer */
          connected_num[0] = memscpy( &connected_num[1],
                                      sizeof(connected_num),
                                      wcall_info_ptr->connected_num.data,
                                      wcall_info_ptr->connected_num.length );

          /* convert to ascii */
          cm_util_bcd_num_to_ascii_num( connected_num, ascii_num, sizeof(ascii_num) );

          num_len =  (byte )strlen((char *)ascii_num);

          if ( ascii_num[0] != 0 &&
               !(num_len == 1 && ascii_num[0] == '+'))//avoid sending just '+' when network sends zero length
                                                      // number buffer for international number type.
          {
            /* copy the info to the number buffer */
            call_ptr->num.len = memscpy( call_ptr->num.buf,
                                         sizeof( call_ptr->num.buf ),
                                         ascii_num,
                                         num_len );
          }
          if (num_len == 0)
          {
	    call_ptr->num.len =0;
            memset(call_ptr->num.buf , 0, CM_MAX_NUMBER_CHARS);
          }
          call_ptr->num.pi = presentation_indicator;
          call_ptr->num.si = screening_indicator;
        }

        cmcall_event( call_ptr, CM_CALL_EVENT_CONNECTED_NUM );

        break;

    case CM_MANAGE_CALLS_CONF:

      /* Manage calls confirmation received */
      CM_MSG_HIGH_0("RXD: MANAGE_CALLS_CONF");

      /* special processing required since this message does not
                 have a specific connection ID */

      /* Allocate a call info buffer */
      call_info_ptr = cmcall_info_alloc();

      call_info_ptr->mode_info.info.gw_cs_call.call_ss_success =
                cm_rpt_ptr->cmd.mng_calls_conf.call_ss_success;

      call_info_ptr->mode_info.info_type = CM_CALL_MODE_INFO_GW_CS;

      if( call_info_ptr->mode_info.info.gw_cs_call.call_ss_success )
      {
        /* put the current active calls on hold */

        cmcall_obj_iterator_init(&call_itr);

        call_ptr = cmcall_obj_get_next(&call_itr);

        while(call_ptr != NULL)
        {
          if ( ( call_ptr->call_state == CM_CALL_STATE_CONV) &&
               ( call_ptr->call_subst.conv == CM_CALL_CONV_CALL_ACTIVE) &&
               ( call_ptr->call_type == CM_CALL_TYPE_VOICE ))
          {
              call_ptr->call_subst.conv = CM_CALL_CONV_CALL_ON_HOLD;
          }

          call_ptr = cmcall_obj_get_next(&call_itr);
        }

        /* update the call state information */
        for (call_index = 0;
             call_index < cm_rpt_ptr->cmd.mng_calls_conf.active_calls_list.size;
             call_index++
            )
        {
          active_connection_id = cm_rpt_ptr->cmd.mng_calls_conf.active_calls_list.table[call_index];

          if (!cm_find_call_id_per_subs(CONN_ID_MN,
                        active_connection_id,
                        &call_id,cm_rpt_ptr->cmd.mng_calls_conf.as_id) )

          {
            CM_ERR_0("ERROR: Unknown connection ID");
            valid_conn_id = FALSE;
            break;
          }

          call_ptr = cmcall_ptr( call_id );

          if(( call_ptr->call_state == CM_CALL_STATE_CONV ) &&
             ( call_ptr->call_subst.conv == CM_CALL_CONV_CALL_ON_HOLD)
            )
          {
            call_ptr->call_subst.conv = CM_CALL_CONV_CALL_ACTIVE;
          }

           /* copy the active calls list information into the
              call_info_ptr */
          call_info_ptr->mode_info.info.gw_cs_call.active_calls_list.table[call_index] = call_id;
        }

        if( valid_conn_id )
        {
          call_info_ptr->mode_info.info.gw_cs_call.active_calls_list.size =
                cm_rpt_ptr->cmd.mng_calls_conf.active_calls_list.size;
        }
        else
        {
          CM_ERR_0( "Invalid fields in MNG_CALLS_CONF");
        }
      }
      else
      {
        if( cm_rpt_ptr->cmd.mng_calls_conf.cm_ss_error.present)
        {
           call_info_ptr->mode_info.info.gw_cs_call.ss_error =
                                cm_rpt_ptr->cmd.mng_calls_conf.cm_ss_error;
        }
      }

      /* Copy subs id */
      if (cmph_is_feature_mask(FEATURE_MODE_MASK_MSTACK_MNAS))
      {
        call_info_ptr->asubs_id = cm_rpt_ptr->cmd.mng_calls_conf.as_id;
      }

      /* Notify clients list of call event. */
      cmclient_list_call_event_ntfy( CM_CALL_EVENT_MNG_CALLS_CONF, call_info_ptr );

      /* Deallocate the memory after sending the call event. */
      cm_mem_free(call_info_ptr);

    break;

    #if !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
    case CM_MT_CALL_SS_IND:

      /* Process ss notification for CCBS recall
      */
      if( cm_rpt_ptr->cmd.call_ss_ind.ind_type == CM_NOTIFY_SS_IND)
      {
        /*
        ** When a recall indication is received from network for
        ** a previosly activated CCBS two primitives are received
        ** back to back in CM
        **
        ** CM_MT_CALL_SS_IND (SS Notify with ss_code as ccbs)
        **   Gives ccbs indx and called number.
        **
        ** CM_RECALL_IND
        **   Call obj created by SS Notify is used to generate
        **   CM_CALL_EVENT_CCBS_RECALL_IND
        **
        */
        if (cmwcall_process_ss_notify_ccbs_recall (cm_rpt_ptr))
        {
          break;
        }
      }

      if (!cm_find_call_id_per_subs(CONN_ID_MN,
                    cm_rpt_ptr->cmd.call_ss_ind.connection_id,
                    &call_id,cm_rpt_ptr->cmd.call_ss_ind.as_id) )
      {
        CM_MSG_HIGH_0(" ERROR: Unknown Connection ID");
        break;
      }

      /* get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( call_id );

      CM_ASSERT( call_ptr != NULL );

      /* see whether it is a NotifySS or a Forward Charge Advice info */
      if( cm_rpt_ptr->cmd.call_ss_ind.ind_type == CM_NOTIFY_SS_IND)
      {
        /* it is a Notify SS indication */
        cm_notify_ss = &cm_rpt_ptr->cmd.call_ss_ind.data.cm_notify_ss;

        wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

        if ( cm_notify_ss->cm_ss_status.present )
        {
          /* copy the ss_status */
          wcall_info_ptr->ss_info.ss_status =  cm_notify_ss->cm_ss_status;

          if (cm_notify_ss->cm_ss_status.cm_ss_active == TRUE)
          {
            if(cm_notify_ss->cm_ss_code.present)
            {
              /* copy the ss_code */
              wcall_info_ptr->ss_info.ss_code = cm_notify_ss->cm_ss_code;

              /* Notification during an outgoing call about the status
                 of the call forwarding */
              if (cm_notify_ss->cm_ss_code.ss_code == (byte)cd)
              {
                cmcall_event ( call_ptr, CM_CALL_EVENT_CALL_DEFLECTION);
              }
              else if ( (cm_notify_ss->cm_ss_code.ss_code & 0xF0) ==
                                                    (byte)allCallRestrictionSS)
              {
                /* Call Barred Notification */
                cmcall_event (call_ptr, CM_CALL_EVENT_CALL_BARRED);
              }
              /* note: now it is assumed that only CF event need to be
              ** sent to clients. All ss_codes must be verified before sending the
              ** event
              */
              else
              {
                /* default indication about the status of CF */
                cmcall_event (call_ptr, CM_CALL_EVENT_ORIG_FWD_STATUS);
              }
            }
            else
            {
              /* default indication about the status of CF  */
              cmcall_event (call_ptr, CM_CALL_EVENT_ORIG_FWD_STATUS);
            }
          } /* ss_active .. */

        } /* ss_status.present ... */

        /* Call forwarding, call deflection */
        if ( cm_notify_ss->cm_ss_notification.present )
        {
          /* copy the ss notification */
          wcall_info_ptr->ss_info.ss_notification = cm_notify_ss->cm_ss_notification;

          if (((byte)(cm_notify_ss->cm_ss_notification.notification & 0x04) >> 2) == 1)
          {
            /* calling subsriber */
            if (cm_notify_ss->cm_ss_code.present )
            {
              wcall_info_ptr->ss_info.ss_code = cm_notify_ss->cm_ss_code;

              if (cm_notify_ss->cm_ss_code.ss_code == (byte)cd)
              {
                cmcall_event( call_ptr, CM_CALL_EVENT_CALL_DEFLECTION);
              }
              else
              {
                cmcall_event(call_ptr, CM_CALL_EVENT_CALL_BEING_FORWARDED);
              }
            }
            else
            {
              /* default indication that it is a forwarded call */
              cmcall_event(call_ptr, CM_CALL_EVENT_CALL_BEING_FORWARDED);
            }
          }

          else if (((byte)(cm_notify_ss->cm_ss_notification.notification & 0x02) >> 1) == 1)
          {
            /* forwarding subscriber */

            if (cm_notify_ss->cm_ss_code.present )
            {
              wcall_info_ptr->ss_info.ss_code = cm_notify_ss->cm_ss_code;

              if (cm_notify_ss->cm_ss_code.ss_code == (byte)cd)
              {
                cmcall_event( call_ptr, CM_CALL_EVENT_CALL_DEFLECTION);
              }
              else
              {
                cmcall_event(call_ptr,  CM_CALL_EVENT_CALL_FORWARDED );
              }
            }
            else
            {
              /* default indication */
              cmcall_event(call_ptr,  CM_CALL_EVENT_CALL_FORWARDED );
            }
          }
          else if ((cm_notify_ss->cm_ss_notification.notification & 0x01) == 1)
          {

            if(cmss_get_true_sys_mode(call_ptr->ss))
            {
              call_ptr->sys_mode = cmss_get_true_sys_mode(call_ptr->ss);
            }            /* forwarded to subscriber   */

            if (cm_notify_ss->cm_ss_code.present )
            {
              wcall_info_ptr->ss_info.ss_code = cm_notify_ss->cm_ss_code;

              if (cm_notify_ss->cm_ss_code.ss_code == (byte)cd)
              {
                cmcall_event( call_ptr, CM_CALL_EVENT_CALL_DEFLECTION);
              }
              else
              {
                cmcall_event (call_ptr, CM_CALL_EVENT_INCOM_FWD_CALL);
              }
            }
            else
            {
              cmcall_event (call_ptr, CM_CALL_EVENT_INCOM_FWD_CALL);
            }
          }
        } /* ss_notification.present ... */

        /* ECT */
        if ( ( cm_notify_ss->cm_ect_indicator.present ) &&
           ( cm_notify_ss->cm_ss_code.ss_code == (byte)ect ) )
        {
          wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

          /* ect state */
          wcall_info_ptr->ect_info.callState  = cm_notify_ss->cm_ect_indicator.callState;

          if( cm_notify_ss->cm_ect_indicator.routeDestNumber.present )
          {
            wcall_info_ptr->ect_info.routeDestNumber.present  = TRUE;

            /* look for the presentation indicator*/
            wcall_info_ptr->ect_info.routeDestNumber.type =
                  cm_notify_ss->cm_ect_indicator.routeDestNumber.type;

            /* save the redirected number information in ASCII*/
            if( ( cm_notify_ss->cm_ect_indicator.routeDestNumber.type == presentationAllowedAddress ) ||
                ( cm_notify_ss->cm_ect_indicator.routeDestNumber.type == presentationRestricteddAdress ))
            {
              for( i = 0; i < MAXNO_FORWARD_TO_NUMBER; i++ )
              {
                /* initialize the ect number info */
                memset( &wcall_info_ptr->ect_info.routeDestNumber.number[i],
                        0xFF,
                        sizeof( cm_called_party_bcd_no_s_type) );

                if ( cm_notify_ss->cm_ect_indicator.routeDestNumber.number[i].present)
                {
                  *(&(wcall_info_ptr->ect_info.routeDestNumber.number[i])) = *(&(cm_notify_ss->cm_ect_indicator.routeDestNumber.number[i]));
                } /* routeDestNumber.number[i].present... */

                wcall_info_ptr->ect_info.routeDestNumber.type = (routing_number_option_s_type) CM_PRESENTATION_ALLOWED;

              } /* for i=0; ...MAX_FWD_TO_NUM... */
            } /* routeDestNumber.type == ... */
          } /* routeDestNumber.present ... */

          if ( cm_notify_ss->cm_ect_indicator.routeDestNumber.address.present )
          {
            memset( &wcall_info_ptr->ect_info.routeDestNumber.address,
                    0xFF,
                    sizeof( cm_called_party_subaddress_s_type ));
            *(&(wcall_info_ptr->ect_info.routeDestNumber.address)) = *(&(cm_notify_ss->cm_ect_indicator.routeDestNumber.address));

          } /* address.present ... */
          cmcall_event( call_ptr, CM_CALL_EVENT_TRANSFERRED_CALL);

        }  /* ect indicator present ... */

        /* Call Waiting */
        if ( cm_notify_ss->cm_call_is_waiting_indicator.present )
        {
          cmcall_event(call_ptr, CM_CALL_EVENT_CALL_IS_WAITING);
        }

        #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
        /* Call Hold */
        if (cm_notify_ss->cm_call_on_hold_indicator.present )
        {
          /* check whether the call hold indicator is a HOLD
                              or a RETRIEVE
           */
          if (cm_notify_ss->cm_call_on_hold_indicator.call_on_hold ==
                              CALL_ON_HOLD)
          {
            /* update the call state information */
            if( (call_ptr->call_state == CM_CALL_STATE_CONV) &&
                (call_ptr->call_subst.conv == CM_CALL_CONV_CALL_ACTIVE) &&
                (call_ptr->call_type == CM_CALL_TYPE_VOICE))
            {
	      call_ptr->call_subst.conv = CM_CALL_CONV_CALL_ON_HOLD;
              cmcall_event( call_ptr, CM_CALL_EVENT_CALL_ON_HOLD);
            }
            else
            {
                CM_ERR_0( "Call on hold notification in idle call state");
            }

          }
          else if (cm_notify_ss->cm_call_on_hold_indicator.call_on_hold ==
                              CALL_RETRIEVED)
          {
            /* update the call state information */
            if( (call_ptr->call_state == CM_CALL_STATE_CONV) &&
                (call_ptr->call_type == CM_CALL_TYPE_VOICE))
            {
	      call_ptr->call_subst.conv = CM_CALL_CONV_CALL_ACTIVE;
              cmcall_event( call_ptr, CM_CALL_EVENT_CALL_RETRIEVED);
            }
            else
            {
              CM_ERR_0("Call retrieved notification in idle call state");
            }
          }
        }
        #endif

        /* Multiparty call indication */
        if (cm_notify_ss->cm_mpty_indicator.present)
        {
          cmcall_event(call_ptr, CM_CALL_EVENT_MPTY_CALL_IND);
        }

        /* CLIR */
        if (cm_notify_ss->cm_clir_suppression_rejected.clir_suppresion_rej )
        {
          cmcall_event( call_ptr, CM_CALL_EVENT_CALL_RESTRICTED);
        }

        if (cm_notify_ss->cm_name_indicator.present)
        {
          call_ptr->num.pi = (cm_npi_type)cm_notify_ss->cm_name_indicator.type;

          if (cm_notify_ss->cm_name_indicator.name.present)
          {
            /* Populates the fields in enc_alpha structure.
            */

            /* Copy the bytes into buffer */
            call_ptr->enc_alpha.num_bytes = memscpy( call_ptr->enc_alpha.buf,
                                                     sizeof(call_ptr->enc_alpha.buf),
                                                     cm_notify_ss->cm_name_indicator.name.nameString,
                                                     cm_notify_ss->cm_name_indicator.name.nameStringLength );

            /* Update the coding scheme */
            call_ptr->enc_alpha.coding_scheme = cm_notify_ss->cm_name_indicator.name.dataCodingScheme;

            /* Update the number of characters in the decoded string */
            call_ptr->enc_alpha.num_characters = cm_notify_ss->cm_name_indicator.name.lengthInCharacter;

            /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            /* Printing the data fields for cm_notify_ss->cm_name_indicator */
            CM_MSG_HIGH_3 ("DataCodingScheme:%d NumOfBytes:%d NumOfChars:%d",
                          cm_notify_ss->cm_name_indicator.name.dataCodingScheme, 
                          cm_notify_ss->cm_name_indicator.name.nameStringLength, 
                          cm_notify_ss->cm_name_indicator.name.lengthInCharacter);


            /* Populates the fields in alpha structure.
            */
            call_ptr->alpha.len = cm_notify_ss->cm_name_indicator.name.lengthInCharacter;

            /* Here we assume the DataCodingScheme used for encoding cm_notify_ss->cm_name_indicator.name 
            ** is UCS-2 . To support other coding schemes in future, this portion of code below would need
            ** to be enhanced  */
            if (call_ptr->alpha.len > (CM_MAX_ALPHA_TAG_BYTES / 2))             
            {
               call_ptr->alpha.len = (CM_MAX_ALPHA_TAG_BYTES / 2);
            }

            for (i = 0; i < call_ptr->alpha.len; i++)
            {
               call_ptr->alpha.buf[i] =
                  cm_notify_ss->cm_name_indicator.name.nameString[2*i+1];
            }
          }

          cmcall_event( call_ptr, CM_CALL_EVENT_CNAP_INFO_RECEIVED);
        }

        if (cm_notify_ss->cm_cug_index.present)
        {
          call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_CS;
          call_ptr->cmcall_mode_info.info.gsm_wcdma_info.forward_cug_info.present = TRUE;
          call_ptr->cmcall_mode_info.info.gsm_wcdma_info.forward_cug_info.cm_cug_index.present =
            TRUE;
          call_ptr->cmcall_mode_info.info.gsm_wcdma_info.forward_cug_info.cm_cug_index.msb =
            cm_notify_ss->cm_cug_index.msb;
          call_ptr->cmcall_mode_info.info.gsm_wcdma_info.forward_cug_info.cm_cug_index.lsb =
            cm_notify_ss->cm_cug_index.lsb;

          cmcall_event( call_ptr, CM_CALL_EVENT_CUG_INFO_RECEIVED);
        }
      }
      #if defined(FEATURE_MMODE_3GPP) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE) \
	  	&& !defined (FEATURE_MMODE_LTE_ONLY) && !defined(FEATURE_MMODE_GPRS_LTE)
      else if (cm_rpt_ptr->cmd.call_ss_ind.ind_type == CM_FWD_AOC_IND )
      {
        #if defined(FEATURE_MMODE_3GPP) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
        mn_cnm_cmd_type *aoc_response_ptr;
        #endif

        cm_e_values_type charge_info;

        if (cm_rpt_ptr->cmd.call_ss_ind.data.cm_forward_charge_advice.e1.present)
        {
          charge_info.e1 = cm_rpt_ptr->cmd.call_ss_ind.data.cm_forward_charge_advice.e1.msb << 8 |
                           cm_rpt_ptr->cmd.call_ss_ind.data.cm_forward_charge_advice.e1.lsb;
        }
        else
        {
          charge_info.e1 = 0;
        }

        if (cm_rpt_ptr->cmd.call_ss_ind.data.cm_forward_charge_advice.e2.present)
        {
          charge_info.e2 = cm_rpt_ptr->cmd.call_ss_ind.data.cm_forward_charge_advice.e2.msb << 8 |
                           cm_rpt_ptr->cmd.call_ss_ind.data.cm_forward_charge_advice.e2.lsb;
        }
        else
        {
          charge_info.e2 = 0;
        }

        if (cm_rpt_ptr->cmd.call_ss_ind.data.cm_forward_charge_advice.e3.present)
        {
          charge_info.e3 = cm_rpt_ptr->cmd.call_ss_ind.data.cm_forward_charge_advice.e3.msb << 8 |
                           cm_rpt_ptr->cmd.call_ss_ind.data.cm_forward_charge_advice.e3.lsb;
        }
        else
        {
          charge_info.e3 = 1;
        }

        if (cm_rpt_ptr->cmd.call_ss_ind.data.cm_forward_charge_advice.e4.present)
        {
          charge_info.e4 = cm_rpt_ptr->cmd.call_ss_ind.data.cm_forward_charge_advice.e4.msb << 8 |
                           cm_rpt_ptr->cmd.call_ss_ind.data.cm_forward_charge_advice.e4.lsb;
        }
        else
        {
          charge_info.e4 = 0;
        }

        if (cm_rpt_ptr->cmd.call_ss_ind.data.cm_forward_charge_advice.e5.present)
        {
          charge_info.e5 = cm_rpt_ptr->cmd.call_ss_ind.data.cm_forward_charge_advice.e5.msb << 8 |
                           cm_rpt_ptr->cmd.call_ss_ind.data.cm_forward_charge_advice.e5.lsb;
        }
        else
        {
          charge_info.e5 = 0;
        }

        if (cm_rpt_ptr->cmd.call_ss_ind.data.cm_forward_charge_advice.e6.present)
        {
          charge_info.e6 = cm_rpt_ptr->cmd.call_ss_ind.data.cm_forward_charge_advice.e6.msb << 8 |
                           cm_rpt_ptr->cmd.call_ss_ind.data.cm_forward_charge_advice.e6.lsb;
        }
        else
        {
          charge_info.e6 = 0;
        }

        if (cm_rpt_ptr->cmd.call_ss_ind.data.cm_forward_charge_advice.e7.present)
        {
          charge_info.e7 = cm_rpt_ptr->cmd.call_ss_ind.data.cm_forward_charge_advice.e7.msb << 8 |
                           cm_rpt_ptr->cmd.call_ss_ind.data.cm_forward_charge_advice.e7.lsb;
        }
        else
        {
          charge_info.e7 = 0;
        }

        aoc_response_ptr = cm_mn_get_buf_else_err_fatal(CM_MT_CALL_SS_RES);
        aoc_response_ptr->cmd.cm_mt_call_ss_res.aoc_supported =
            cmwaoc_setup_aoc_for_call(call_ptr, &charge_info);
        aoc_response_ptr->cmd.cm_mt_call_ss_res.as_id =  cm_rpt_ptr->cmd.call_ss_ind.as_id;
        cm_mn_send_cmd(aoc_response_ptr);
		
      }  /* it is AoC indication */
      #endif

      break;
    #endif

    #ifdef FEATURE_UUS
    case CM_MT_USER_DATA_IND:

      /* User data indication should be received only after CM_MT_SETUP_IND
      ** has been received for a connection id. It has to be received
      ** before CM_MT_CALL_IND so that user can be displayed the string
      ** at the time of alerting
      */
      if (!cm_find_call_id_per_subs(CONN_ID_MN,
                    cm_rpt_ptr->cmd.user_data.connection_id,
                    &call_id,cm_rpt_ptr->cmd.user_data.as_id) )
      {
        CM_ERR_0 ("Unknwn Conn ID : MT_USER_DATA_IND");
        break;
      }

      CM_MSG_HIGH_2 ("Receiving USER_DATA length = %d for call %d",
                    cm_rpt_ptr->cmd.user_data.user_user_data_length, call_id);

      /* get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( call_id );

      wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

      /* Setting present flag */
      wcall_info_ptr->mo_mt_user_data.present =  TRUE;

      /* Copying more_data_indicator from rpt */
      wcall_info_ptr->mo_mt_user_data.more_data_indicator =
                       cm_rpt_ptr->cmd.user_data.more_data_indicator;

      /* Copying user_data from rpt */
      wcall_info_ptr->mo_mt_user_data.user_user_data_length = memscpy( wcall_info_ptr->mo_mt_user_data.user_user_data,
                                                                       sizeof(wcall_info_ptr->mo_mt_user_data.user_user_data),
                                                                       cm_rpt_ptr->cmd.user_data.user_user_data,
                                                                       cm_rpt_ptr->cmd.user_data.user_user_data_length );

      cmcall_event (call_ptr, CM_CALL_EVENT_USER_DATA_IND);

      break;

    case CM_USER_CONGESTION_IND:

      /* Only applicable for MO user data to stop any more messages being
      ** sent down to NAS
      */
      if (!cm_find_call_id_per_subs(CONN_ID_MN,
                    cm_rpt_ptr->cmd.user_congestion.connection_id,
                    &call_id,cm_rpt_ptr->cmd.user_congestion.as_id) )
      {
        CM_ERR_0 ("Unknwn Conn ID : MT_USER_CONGESTION_IND");
        break;
      }

      CM_MSG_HIGH_1 ("Receiving congestion ind for call %d", call_id);

      /* get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( call_id );

      wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

      /* Clearing user data present just to make sure client does
      ** not pick up stale user data from the call event
      */
      wcall_info_ptr->mo_mt_user_data.present                 = FALSE;

      wcall_info_ptr->mo_mt_user_data.user_user_data_length   = 0;

      /* Copy receive ready field */
      wcall_info_ptr->is_receiver_busy  =
                      cm_rpt_ptr->cmd.user_congestion.receiver_busy;

      /* Copy cm_cc_cause field */
      wcall_info_ptr->cm_cc_cause     =
                      cm_rpt_ptr->cmd.user_congestion.cm_cc_cause;

      cmcall_event (call_ptr, CM_CALL_EVENT_USER_DATA_CONG_IND);

      break;
    #endif /* FEATURE_UUS */
    #if defined(FEATURE_CCBS) && defined(FEATURE_MMODE_SUPPORT_SUPS)
    case CM_5_SEND_CONF:
      #if defined(FEATURE_CCBS)
      /* Check conf_type, If CCBS_RET_RES copy ccbs index and pass on */
      if (!cm_find_call_id_per_subs(CONN_ID_MN,
                    cm_rpt_ptr->cmd.ccbs_send_conf.connection_id,
                    &call_id,cm_rpt_ptr->cmd.ccbs_send_conf.as_id) )
      {
        CM_ERR_1 ("Unknwn Conn ID : %d",
                 cm_rpt_ptr->cmd.ccbs_send_conf.connection_id);
        break;
      }

      /* get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( call_id );

      CM_ASSERT (call_ptr != NULL);

      wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

      ccbs_conf_ptr = &cm_rpt_ptr->cmd.ccbs_send_conf;

      wcall_info_ptr->ccbs_ret_res.present            = TRUE;

      wcall_info_ptr->ccbs_ret_res.ccbs_cnf           =
                 cmcall_map_5_send_conf_mn_to_cm (ccbs_conf_ptr->conf_type);

      /* Copy ccbs index received as part of activation confirmation
      */
      if (wcall_info_ptr->ccbs_ret_res.ccbs_cnf == CM_CCBS_CNF_RET_RES)
      {

        wcall_info_ptr->ccbs_ret_res.ccbs_indx_present =
                 ccbs_conf_ptr->rsp.ret_res.ccbsIndex.present;

        wcall_info_ptr->ccbs_ret_res.ccbs_indx         =
                 ccbs_conf_ptr->rsp.ret_res.ccbsIndex.ccbs_Index;

        /* Phone objects copy of ccbs indx and called number
        */
        if (!cmph_add_ccbs_entry (ss, ccbs_conf_ptr->rsp.ret_res.ccbsIndex.ccbs_Index,
                                  ccbs_conf_ptr->rsp.ret_res.subscriberNumber[0]))
        {
          CM_ERR_0 ("Adding ccbs index in Ph store failed");
        }

      }
      else if ( (wcall_info_ptr->ccbs_ret_res.ccbs_cnf == CM_CCBS_CNF_RET_ERR)
                ||
                (wcall_info_ptr->ccbs_ret_res.ccbs_cnf == CM_CCBS_CNF_REJ)
              )
      {
        wcall_info_ptr->ss_error  =  ccbs_conf_ptr->rsp.cm_ss_error;
      }

      cmcall_event (call_ptr, CM_CALL_EVENT_ACT_CCBS_CNF);

      #endif

      break;


    case CM_RECALL_IND:

      #if defined(FEATURE_CCBS)

      /* Recall indication for a call which was requested to be put under
      ** CCBS
      */
      if (!cm_find_call_id_per_subs(CONN_ID_MN,
                    cm_rpt_ptr->cmd.recall_ind.connection_id,
                    &call_id,cm_rpt_ptr->cmd.recall_ind.as_id) )
      {
        CM_ERR_0 ("Unknwn Conn ID : CM_RECALL_IND");
        break;
      }

      /* Get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( call_id );

      CM_ASSERT (call_ptr != NULL);

      wcall_info_ptr   = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

      recall_info_ptr  = &cm_rpt_ptr->cmd.recall_ind.call_info;


      /*-----------------------------------------------------------------*/

      /* Copy call details that might be later reused for silent redial
      */
      wcall_info_ptr->bc_repeat_indicator          =
          recall_info_ptr->bc_repeat_indicator;

      wcall_info_ptr->bearer_capability_1          =
          recall_info_ptr->bearer_capability_1;

      wcall_info_ptr->bearer_capability_2          =
          recall_info_ptr->bearer_capability_2;

      wcall_info_ptr->llc_repeat_indicator         =
          recall_info_ptr->llc_repeat_indicator;

      wcall_info_ptr->low_layer_compatibility_1    =
          recall_info_ptr->low_layer_compatibility_1;

      wcall_info_ptr->low_layer_compatibility_2    =
          recall_info_ptr->low_layer_compatibility_2;

      wcall_info_ptr->hlc_repeat_indicator         =
          recall_info_ptr->hlc_repeat_indicator;

      wcall_info_ptr->high_layer_compatibility_1   =
          recall_info_ptr->high_layer_compatibility_1;

      wcall_info_ptr->high_layer_compatibility_2   =
          recall_info_ptr->high_layer_compatibility_2;

      wcall_info_ptr->forward_cug_info             =
          recall_info_ptr->forward_cug_info;

      wcall_info_ptr->cm_clir                      =
          recall_info_ptr->cm_clir;

      wcall_info_ptr->calling_party_subaddress     =
          recall_info_ptr->cm_calling_party_subaddress;

      wcall_info_ptr->called_party_bcd_number      =
          recall_info_ptr->cm_called_party_bcd_number;

      wcall_info_ptr->called_party_subaddress      =
          recall_info_ptr->cm_called_party_subaddress;

      /* redirect_party_number  is not being sent during call set up.
      ** so we do not copy it here.
      */

      /*-----------------------------------------------------------------*/

      /* Send Recall indication for this call
      */
      cmcall_event (call_ptr, CM_CALL_EVENT_CCBS_RECALL_IND);

      #endif

      break;


    case CM_CCBS_INFO_IND:

      #if defined(FEATURE_CCBS)

      /* Received all the time when there is a disconnect from the
      ** network for an originating call
      */
      if (!cm_find_call_id_per_subs(CONN_ID_MN,
                    cm_rpt_ptr->cmd.ccbs_info.connection_id,
                    &call_id,cm_rpt_ptr->cmd.ccbs_info.as_id) )
      {
        CM_ERR_0 ("Unknwn Conn ID : CM_CCBS_INFO_IND");
        break;
      }

      /* Get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( call_id );

      CM_ASSERT (call_ptr != NULL);

      if (call_ptr->call_state != CM_CALL_STATE_ORIG)
      {
        CM_ERR_1 ("CM_CCBS_INFO_IND recvd in incorrect call state %d",
                  call_ptr->call_state);

        CM_ASSERT (call_ptr->call_state == CM_CALL_STATE_ORIG);
      }

      /* CCBS_ALLOWED event gets sent only if network reports ccbs being
      ** allowed. Otherwise no event gets sent
      */
      if (cm_rpt_ptr->cmd.ccbs_info.ccbs_allowed)
      {
        cmcall_event (call_ptr, CM_CALL_EVENT_CCBS_ALLOWED);
      }

      #endif

      break;
    #endif

    case CM_MODIFY_COMPLETE_CONF:

      #if defined (FEATURE_MULTIMEDIA_FALLBACK)

      /* Modify complete conf is received from network for modify request
      ** RAB change is received by NAS when RAB has to be changed from
      ** voice to VT or other way round. RAB change is received as progress
      ** info ind to CM. During call modification order of progress info
      ** and modify complete conf is not guaranteed, so CM changes the
      ** call_type on the first message received.
      */
      /* Connection id for the call */
      if (!cm_find_call_id_per_subs(CONN_ID_MN,
                    cm_rpt_ptr->cmd.modify_complete_conf.connection_id,
                    &call_id,cm_rpt_ptr->cmd.modify_complete_conf.as_id) )
      {
        CM_ERR_0 ("Unknwn Conn ID : CM_MODIFY_COMPLETE_CONF");
        break;
      }

      /* Get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( call_id );

      CM_ASSERT (call_ptr != NULL);

      wcall_info_ptr   = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

      CM_MSG_HIGH_1 ("CM_MODIFY_COMPLETE_CONF complete status = %d",
                   cm_rpt_ptr->cmd.modify_complete_conf.complete);


      /* pointer to modify compl */
      mod_compl_ptr = &cm_rpt_ptr->cmd.modify_complete_conf;

      /* Call type gets modified only if modify complete is TRUE.
      ** ALSO need to check for reverse_call_setup_direction ...????
      */
      if (cm_rpt_ptr->cmd.modify_complete_conf.complete)
      {
        /* Call modification succeeded
        */
        wcall_info_ptr->incall_modif_err_status = FALSE;

        /* Check made if BC returned by mod complete is of requested
        ** call type
        */
        if ((mod_compl_ptr->cm_bearer_capability.present)  &&
            (wcall_info_ptr->new_call_type_req != CM_CALL_TYPE_NONE) &&
            (!cmwcall_is_call_type_partof_bearer
                           (wcall_info_ptr->new_call_type_req,
                            &mod_compl_ptr->cm_bearer_capability)
            )
           )
        {
          CM_ERR_2 ("Modified BC = %d, not of req call type = %d",
                   mod_compl_ptr->cm_bearer_capability.information_transfer_capability,
                   wcall_info_ptr->new_call_type_req);
        }

        if (wcall_info_ptr->new_call_type_req != CM_CALL_TYPE_NONE)
        {

          /* Current BC1, LLC1  are copied into BC2, LLC2.
          ** BC and LLC from Modify complete conf are copied into BC1, LLC1
          */
          cmwcall_update_on_modify_complete (wcall_info_ptr,
                                     &cm_rpt_ptr->cmd.modify_complete_conf);

          /* Recompute call type
          */
          call_ptr->call_type = cmwcall_determine_call_type
                                  (call_ptr, &wcall_info_ptr->bearer_capability_1);
        }

      }
      else
      {
        /* Call modification failed
             */
        wcall_info_ptr->incall_modif_err_status = TRUE;

        /* Copy cause information for call failure
        */
        wcall_info_ptr->cc_cause      =
                              cm_rpt_ptr->cmd.modify_complete_conf.cm_cc_cause;

        /*Already received CM_CALL_PROGRESS_INFO_IND so call_type and
        ** BC should be reverted.
        */
        if (wcall_info_ptr->new_call_type_req == CM_CALL_TYPE_NONE)
        {
          /* Current BC1, LLC1  are copied into BC2, LLC2.
          ** BC and LLC from Modify complete conf are copied into BC1, LLC1
          */
          cmwcall_update_on_modify_complete (wcall_info_ptr,
                                     &cm_rpt_ptr->cmd.modify_complete_conf);

          /* Recompute call type */
          call_ptr->call_type = cmwcall_determine_call_type
                                  (call_ptr, &wcall_info_ptr->bearer_capability_1);
        }
        else
        {
          /* This is the first response message to modify_request */
          wcall_info_ptr->new_call_type_req = CM_CALL_TYPE_NONE;
        }

      }

      cmcall_event (call_ptr, CM_CALL_EVENT_MODIFY_COMPLETE_CONF);

      /* Reset new_call_type_req field
      */
      wcall_info_ptr->new_call_type_req = CM_CALL_TYPE_NONE;

      #endif

      break;


    case CM_MT_MODIFY_IND:

      #if defined (FEATURE_MULTIMEDIA_FALLBACK)

      /* Connection id for the call */
      if (!cm_find_call_id_per_subs(CONN_ID_MN,
                       cm_rpt_ptr->cmd.mt_modify_ind.connection_id,
                    &call_id,cm_rpt_ptr->cmd.mt_modify_ind.as_id) )
      {
        CM_ERR_0 ("Unknwn Conn ID : CM_MT_MODIFY_IND");
        break;
      }

      /* Get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( call_id );

      wcall_info_ptr   = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;

      CM_ASSERT (call_ptr != NULL);

      /* BC, HLC, LLC requested in a network initiated modif should match
      ** BC2, HLC2 and LLC2 of the call object.
      */
      if ( (!cmwcall_is_modify_ind_accep ( &cm_rpt_ptr->cmd.mt_modify_ind,
                                           call_ptr )
           )
         )
      {
        CM_ERR_0 ("Modify ind with non-negot/incompl params");
        break;
      }

      if (cm_rpt_ptr->cmd.mt_modify_ind.bearer_capability.present)
      {

        wcall_info_ptr->new_call_type_req =
        cmwcall_determine_call_type (call_ptr,
                           &cm_rpt_ptr->cmd.mt_modify_ind.bearer_capability);

      }

      CM_MSG_HIGH_1 ("CM_MT_MODIFY_IND nw_initiated_ind = %d",
                   cm_rpt_ptr->cmd.mt_modify_ind.nw_initiated_ind);

      /* Copy information of "nw_initiated_ind" coming from NAS rpt.
      */
      wcall_info_ptr->is_network_initiated =
                        cm_rpt_ptr->cmd.mt_modify_ind.nw_initiated_ind;

      cmcall_event (call_ptr, CM_CALL_EVENT_MODIFY_IND);

      #endif

      break;
    #endif
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    #ifdef FEATURE_GSTK
    case CM_MMGSDI_REFRESH_FCN_IND:
    {
      mmgsdi_refresh_evt_info_type *refresh_data = &(cm_rpt_ptr->cmd.card_status_mmgsdi.data.refresh);
      mmgsdi_slot_id_enum_type slot_id = refresh_data->slot;
      boolean                       is_only_data_call = FALSE;
      boolean                       enforcement_policy_vote = FALSE;
      boolean                       is_emergency_call_active = FALSE;

      sys_modem_as_id_e_type  asubs_id
        = cm_mmgsdi_ssn_tbl_get_subs_id_for_id(cm_rpt_ptr->cmd.card_status_mmgsdi.session_id );

      ss = cmph_map_subs_to_ss(asubs_id);

      is_only_data_call =
        ((int)cmcall_call_ids_allocated_per_as_id(asubs_id) ==
          cmcall_count_calls_with_call_type_per_as_id(CM_CALL_TYPE_PS_DATA,asubs_id));

      CM_MSG_HIGH_2("RXD: SIM_REF: refresh_vote_ok = %d, is_only_data_call = %d",
                                   ph_ptr->refresh_vote_ok,is_only_data_call);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

      is_emergency_call_active = cmcall_emergency_call_active();

      if(refresh_data->mode != MMGSDI_REFRESH_NAA_FCN &&
         refresh_data->stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT)
      {
        if(is_emergency_call_active)
        {
          cmmmgsdi_ok_to_refresh( slot_id,
          #ifdef FEATURE_MMGSDI_SESSION_LIB
                cm_rpt_ptr->cmd.card_status_mmgsdi.session_id,
          #endif
                FALSE);
          CM_MSG_HIGH_0("Voting FALSE as Emergency call is active");
          break;
        }
      }


      if((refresh_data->stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT ||
          refresh_data->stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN) &&
         refresh_data->mode == MMGSDI_REFRESH_RESET &&
         refresh_data->source == MMGSDI_REFRESH_SOURCE_ECALL &&
         (cmcall_is_there_a_call_type(CM_SS_MAX,CM_CALL_TYPE_EMERGENCY,
                                    CM_CALL_ID_INVALID) == CM_CALL_ID_INVALID))
      {
        // Voting true for ERA_GLONASS FR
        CM_MSG_HIGH_0("Voting ok_to_refresh");
        cmmmgsdi_ok_to_refresh( slot_id, 
          #ifdef FEATURE_MMGSDI_SESSION_LIB
          cm_rpt_ptr->cmd.card_status_mmgsdi.session_id,
          #endif
          TRUE );

        CM_MSG_HIGH_0("Voting TRUE for MMGSDI_REFRESH_SOURCE_ECALL");

        break;
      }

      CM_MSG_HIGH_3("refresh_data->mode %d, refresh_data->source %d, refresh_data->stage %d",
      refresh_data->mode,refresh_data->source, refresh_data->stage);

       if(refresh_data->enforcement_policy.is_enforcement_present == TRUE
        && (refresh_data->stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT ||
          refresh_data->stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN) )
       {
          enforcement_policy_vote = TRUE;

        if(refresh_data->enforcement_policy.data.force_refresh_voice_call == FALSE
          && (cmcall_is_there_a_call_type_per_sub(asubs_id, CM_CALL_TYPE_VOICE, NULL) != CM_CALL_ID_INVALID))
        {
          enforcement_policy_vote = FALSE;
        }

        CM_MSG_HIGH_1("is_enforcement_present: enforcement_policy_vote is %d",enforcement_policy_vote);

        cmmmgsdi_ok_to_refresh( slot_id,
        #ifdef FEATURE_MMGSDI_SESSION_LIB
          cm_rpt_ptr->cmd.card_status_mmgsdi.session_id,
        #endif
          enforcement_policy_vote );

        break;

      }

      if(refresh_data->mode != MMGSDI_REFRESH_NAA_FCN &&
         refresh_data->stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT)
      {
          cmmmgsdi_ok_to_refresh( slot_id,
          #ifdef FEATURE_MMGSDI_SESSION_LIB
          cm_rpt_ptr->cmd.card_status_mmgsdi.session_id,
          #endif
                                ph_ptr->refresh_vote_ok?TRUE:is_only_data_call );
        break;
       }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

       else if((refresh_data->mode == MMGSDI_REFRESH_NAA_FCN ||
               refresh_data->mode  == MMGSDI_REFRESH_NAA_INIT_FCN ||
               refresh_data->mode  == MMGSDI_REFRESH_3G_SESSION_RESET )
               &&
               refresh_data->stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN)
       {
        if( ph_ptr->refresh_vote_ok == TRUE || is_only_data_call == TRUE )
           {
             cmmmgsdi_ok_to_refresh( slot_id,
                                     #ifdef FEATURE_MMGSDI_SESSION_LIB
                                     cm_rpt_ptr->cmd.card_status_mmgsdi.session_id,
                                     #endif
                                     TRUE );
           }
           else
        {
          boolean is_ok_to_refresh = TRUE;

           /* If EF_RAT file has been refreshed, CM has to vote YES to Refresh */
          if ( refresh_data->refresh_files.file_list_ptr != NULL )
           {
            /* If the refreshed files are NOT used by CM during call, its ok
            ** to refresh. There may be a call in progress */
            is_ok_to_refresh = !( cmmmgsdi_is_refresh_list_used_by_cm (
                   refresh_data->refresh_files.file_list_ptr,
                                   (uint8)refresh_data->refresh_files.num_files));
             }

          CM_MSG_HIGH_1 ("cmmmgsdi_ok_to_refresh is %d",is_ok_to_refresh);
            cmmmgsdi_ok_to_refresh( slot_id,
               #ifdef FEATURE_MMGSDI_SESSION_LIB
               cm_rpt_ptr->cmd.card_status_mmgsdi.session_id,
               #endif
                                  is_ok_to_refresh );
         }

         /* Free the memory allocated to file list ptr */
         if (refresh_data->refresh_files.file_list_ptr != NULL)
         {
           cm_mem_free (refresh_data->refresh_files.file_list_ptr);
           refresh_data->refresh_files.file_list_ptr = NULL;
         }
         break;
       }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

      else if ( refresh_data->mode == MMGSDI_REFRESH_NAA_FCN
                && refresh_data->stage == MMGSDI_REFRESH_STAGE_START )
       {
         /* Now Refresh is to happen for the files given by SIM.
         ** Check that the request has come for only those files registerd
         ** by CM and nothing else.
         ** The case of CM receiving files for refresh other than registred
         ** with SIM should never happen. Error fatal in this case.
         */
         if(!cmmmgsdi_validate_received_file_list(
                refresh_data->refresh_files.file_list_ptr,
                refresh_data->refresh_files.num_files)
           )
         {
          CM_ERR_FATAL_0("SIM_REF: MMGSDI refresh files contains files other than CM registered for refresh");
         }
         else
         {
           cmmmgsdi_refresh_init( &refresh_data->refresh_files, ss);
           if (refresh_data->refresh_files.file_list_ptr != NULL)
           {
             cm_mem_free(refresh_data->refresh_files.file_list_ptr);
             refresh_data->refresh_files.file_list_ptr = NULL;
           }
         }
       }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

      else if ( refresh_data->stage == MMGSDI_REFRESH_STAGE_START
                &&
          (refresh_data->mode == MMGSDI_REFRESH_RESET||
           refresh_data->mode == MMGSDI_REFRESH_NAA_INIT ||
           refresh_data->mode == MMGSDI_REFRESH_NAA_INIT_FCN ||
           refresh_data->mode == MMGSDI_REFRESH_NAA_INIT_FULL_FCN ||
           refresh_data->mode == MMGSDI_REFRESH_NAA_APP_RESET ||
           refresh_data->mode == MMGSDI_REFRESH_3G_SESSION_RESET ||
           refresh_data->mode == MMGSDI_REFRESH_RESET_AUTO)
                &&
                (cmph_get_received_card_init_completed_evt() == TRUE)
          )
       {
           cmph_set_received_card_init_completed_evt(FALSE);

        /* If CDMA subscription from card is already sent, make CDMA
           ** subscription not available
           */
           #if defined(FEATURE_MMODE_CDMA) ||defined(FEATURE_JCDMA)
           if (cmph_get_cdma_subs_from_card_sent() == TRUE
              #ifdef FEATURE_MMGSDI_SESSION_LIB
              /* With session based APIs, only update CDMA sub status
              ** for CSIM/RUIM events
              */
              &&
              (refresh_data->app_info.app_data.app_type == MMGSDI_APP_CSIM    ||
               refresh_data->app_info.app_data.app_type == MMGSDI_APP_RUIM )
              #endif /* FEATURE_MMGSDI_SESSION_LIB */
              )
           {
             cdma_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;

          /* subscription status updated in global varible for later use */
             cmph_set_cdma_subs_from_card_sent( FALSE );
           }
           #endif
           
        /* If GW subscription from card is already sent, make GW
           ** subscription not available
           */
           if (cmph_get_gw_subs_from_card_sent() == TRUE
               #ifdef FEATURE_MMGSDI_SESSION_LIB
               /* With session based APIs, only update GW sub status
               ** for SIM/USIM events
               */
               &&
               (refresh_data->app_info.app_data.app_type == MMGSDI_APP_SIM    ||
                refresh_data->app_info.app_data.app_type == MMGSDI_APP_USIM )
               #endif /* FEATURE_MMGSDI_SESSION_LIB */
             &&
             asubs_id == cmph_ptr()->main_stack_info.asubs_id
              )
           {
             gw_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;

          /* subscription status updated in global varible for later use */
             cmph_set_gw_subs_from_card_sent( FALSE );
           }

            #if defined(FEATURE_MMODE_DUAL_SIM) || defined (FEATURE_MMODE_SC_SVLTE )\
||    defined(FEATURE_MMODE_SC_SGLTE)
           /* If hybr GW subscription from card is already sent, made GW
           ** subscription not available
           */
           if (cmph_get_hybr_gw_subs_from_card_sent() == TRUE
               && asubs_id == cmph_ptr()->hybr_2_stack_info.asubs_id
              )
           {
             gw2_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;

          /* subscription status updated in global varible for later use */
             cmph_set_hybr_gw_subs_from_card_sent( FALSE );
           }
           #endif

           #ifdef FEATURE_MMODE_HYBR3_ENABLED
           /* If hybr 3 GW subscription from card is already sent, made GW
           ** subscription not available
           */
           if (cmph_get_hybr_3_gw_subs_from_card_sent() == TRUE
               && asubs_id == cmph_ptr()->hybr_3_stack_info.asubs_id
              )
           {
             gw3_subs_status = CM_SUBSCRIPTION_STATUS_CHANGE;

             /* subscription status updated in global
             ** varible for later use
             */
             cmph_set_hybr_3_gw_subs_from_card_sent( FALSE );
           }
          #endif

           /* If subscription from card for any of the cellular technology
           ** (CDMA or GW) is to be made unavailable, then only queue the
           ** subscription not available command to CM
           */
           if (gw_subs_status == CM_SUBSCRIPTION_STATUS_CHANGE ||
               gw2_subs_status == CM_SUBSCRIPTION_STATUS_CHANGE ||
               #ifdef FEATURE_MMODE_HYBR3_ENABLED
               gw3_subs_status == CM_SUBSCRIPTION_STATUS_CHANGE ||
               #endif
             cdma_subs_status == CM_SUBSCRIPTION_STATUS_CHANGE
           )
           {
             if (!cm_ph_cmd_subscription_not_available3(
                      NULL,
                      NULL,
                      CM_CLIENT_ID_ANONYMOUS,
                      cdma_subs_status,
                      gw_subs_status,
                      gw2_subs_status,
                      gw3_subs_status)
                )
             {
            CM_ERR_0("Unable to queue subscription_not_avail cmd to CM");
             }
        } /* if (gw_subs_status == CM_SUBSCRIPTION_STATUS_CHANGE ... */
      } /* if (refresh_data->stage... */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

             else
             {
        CM_MSG_HIGH_2("SIM_REF: Mode %d and Stage %d combination unhandled",
                                     refresh_data->mode, refresh_data->stage);
             }
     }
     break;
     #endif /* FEATURE_GSTK */


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #if defined(FEATURE_MMODE_GW)
    #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
    case CM_HANDOVER_COMPLETE_IND:
      CM_MSG_HIGH_2("RXD: CM_HANDOVER_COMPLETE_IND, Cause = %d, as_id = %d",
                     cm_rpt_ptr->cmd.handover_complete_ind.handover_cause,
                     cm_rpt_ptr->cmd.handover_complete_ind.as_id);
      if( !cm_find_call_id_per_subs( CONN_ID_MN,
                         cm_rpt_ptr->cmd.handover_complete_ind.connection_id,
                         &call_id,
                         cm_rpt_ptr->cmd.handover_complete_ind.as_id ) )
      {
        CM_ERR_0(" ERROR: Unknown Connection ID");
        break;
      }

      /* get the call pointer from the call ID */
      call_ptr = cmcall_ptr ( call_id );

      CM_ASSERT( call_ptr != NULL );

      wcall_info_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info;
      wcall_info_ptr->handover_cause =
                                 cm_rpt_ptr->cmd.handover_complete_ind.handover_cause;

      cmcall_event( call_ptr, CM_CALL_EVENT_HANDOVER_COMPLETE_IND);

      break;

    #if !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE)
    case CM_RR_CODEC_INFO_IND:
      cmwcall_codec_ho_handler(SYS_NETWORK_TYPE_GSM, cm_rpt_ptr);
      break;

    #ifdef FEATURE_WCDMA
    case CM_RRC_CODEC_INFO_IND:
      cmwcall_codec_ho_handler(SYS_NETWORK_TYPE_WCDMA, cm_rpt_ptr);
      break;
    #endif
    
    #ifdef FEATURE_TDSCDMA
    case CM_TD_RRC_CODEC_INFO_IND:
      cmwcall_codec_ho_handler(SYS_NETWORK_TYPE_TD, cm_rpt_ptr);
      break;
    #endif
    #endif
#if 0
    case CM_NAS_SRVCC_HANDOVER_COMPLETE_IND:
      CM_MSG_HIGH_3("RXD: CM_NAS_SRVCC_HANDOVER_COMPLETE_IND, as_id=%d,succes=%d,lte as_id=%d",
                     cm_rpt_ptr->cmd.srvcc_handover_complete_ind.as_id,
                     cm_rpt_ptr->cmd.srvcc_handover_complete_ind.success,
                     cmph_get_sub_with_lte_cap());
      ll_as_id = cmph_get_as_id_from_ll_ind(cm_rpt_ptr->cmd.srvcc_handover_complete_ind.as_id);

      #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE) 

      /* check for NAS validation of call objects */
      if(cm_rpt_ptr->cmd.srvcc_handover_complete_ind.success)
      {

        ho_comp_list = (cm_mid_srvcc_ho_comp_list_s_type *)cm_mem_malloc
                                      (sizeof(cm_mid_srvcc_ho_comp_list_s_type));

        cmcall_obj_iterator_init(&call_itr);
        call_ptr = cmcall_obj_get_next(&call_itr);
        /* loop through all srvcc call in HO */
        while(call_ptr != NULL)
        {
          if (call_ptr->is_srvcc_call)
          {
            /* check whether the single call is IMS enabled*/
            if(call_ptr->is_int_ims)
            {
               if (!cm_unreserve_connection_id(call_ptr->ims_connection_id, call_ptr->asubs_id ))
               {
                  CM_ERR_2( "Could not unreserve MN connection id %d for %d.",
                             call_ptr->ims_connection_id, call_ptr->call_id);
               }
               
               ph_p_orig = cmtask_orig_para_search_act_type( call_ptr->ss,CM_ACT_TYPE_PH_OBJ ) ;
            
               p_orig =  cmtask_orig_para_search_orig_mode( call_ptr->ss, call_ptr->orig_mode ) ;
               if(p_orig != NULL)
               {
                 /* update SRVCCed call orig mode to NORMAL and restore 
                   call mode pref with phone pref ( Scenario where call is made with LTE_ONLY) 
                */
                 if(p_orig->orig->orig_mode == SD_SS_ORIG_MODE_VOLTE )
                 {
                   p_orig->orig->orig_mode = SD_SS_ORIG_MODE_NORMAL;
                   p_orig->orig->orig_mode_pref = ph_p_orig->orig->orig_mode_pref ;
                   p_orig->orig->orig_srv_domain_pref = CM_SRV_DOMAIN_PREF_CS_ONLY;
                 }                
               }
            }
            else
            {
             #if !defined(FEATURE_CM_DISABLE_IMS_EXT)
              /* Update priority queue
              ** - If we already have same call id in priority queue do nothing
              ** - search if there is existing ext ims emerg call. replace
              ** - emerg call act_id with new call id
              ** - If no emergency call in queue, add normal voice call to priQ
              */
              if(cmtask_orig_para_search_act_id( call_ptr->ss, call_ptr->call_id ) == NULL)
              {
                p_orig_emg = cmtask_orig_para_search_act_id( call_ptr->ss, cmemg_get_act_id(call_ptr->ss) );
                if(p_orig_emg == NULL)
                {
                  (void)cmph_update_orig_param_incoming_call(call_ptr);
                }
                else 
                {
                  p_orig_emg->orig->act_id = call_ptr->call_id;
                  /* update call type based on info we have */
                  call_ptr->call_type = CM_CALL_TYPE_EMERGENCY;
                }
              }
              
              /* Exit LTE e911 searching state */
              cmemg_set_reason_per_sub( CM_AC_REASON_NONE, ll_as_id);
              #endif
            }

            /* mark one call id for call event for 3rdparty/internal*/
            if(active_call_id ==CM_CALL_ID_INVALID )
            {
              active_call_id=call_ptr->call_id;
            }

            call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_GW_CS;
            #ifdef FEATURE_IP_CALL
            call_ptr->is_volte = FALSE;
            #endif

           /* After SRVCC exit ECBM since ECBM is required for LTE but not for GW */
           if(call_ptr->call_type == CM_CALL_TYPE_EMERGENCY )
           {
             cmph_sub_config_s_type *ph_sub_config_ptr = cmph_get_sub_config_ptr(call_ptr->asubs_id);
             if(ph_sub_config_ptr == NULL)
             {
               CM_MSG_HIGH_1("cmph sub config pointer[%d] is NULL",call_ptr->asubs_id);
               return;
             }
             if(ph_sub_config_ptr->is_ecbm_required)
             {
               cmph_exit_emergency_cb( ph_ptr,FALSE,call_ptr->ss,FALSE);
             }
           }

           call_info_ptr = cmcall_info_alloc();
           #if defined(FEATURE_MMODE_GW) && !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)      
           srvcc_call_info_ptr =cmcall_srvcc_info_alloc();
           #endif

           if ( call_info_ptr == NULL )
           {
              CM_MSG_HIGH_0("CM_NAS_SRVCC_HANDOVER_COMPLETE_IND, can not alloc memory for call info object");
              break;
           }

           if( srvcc_call_info_ptr == NULL )
           {
             CM_MSG_HIGH_0("CM_NAS_SRVCC_HANDOVER_COMPLETE_IND, can not alloc memory for srvcc call info object");
             break;
           }

           /* use call_info for sending Call event and srvcc_call_info for msgr */
           (void) cmcall_info_get( call_ptr, call_info_ptr );
           (void) cmcall_srvcc_info_get( call_ptr, srvcc_call_info_ptr );

            /*copy each srvcc call context to ho_comp_list buffer */
             * (&(ho_comp_list->info[num_srvcc_calls])) = (*srvcc_call_info_ptr);

            num_srvcc_calls++;
        
            cmlog_call_event_report_payload( CM_CALL_EVENT_HO_COMPLETE, call_info_ptr);

            cm_mem_free(call_info_ptr);
            cm_mem_free(srvcc_call_info_ptr);

            CM_MSG_HIGH_3("CM_NAS_SRVCC_HANDOVER_COMPLETE_IND, call id:%d,call_staet:%d,call_tid:%d",
              call_ptr->call_id,call_ptr->call_state,call_ptr->gw_connection_id);
          }
          
          call_ptr = cmcall_obj_get_next(&call_itr);

        } /* while */

      

        /*if IMS(single/conf) call present, send ipapp notify */
        if(cmwcall_is_int_ims())
        {
          /* Send ipapp notification for registered clients */
          cmipapp_send_srvcc_notification(CMIPAPP_SRVCC_HO_COMPLETE_NOTIFY, ll_as_id);
          ho_comp_list->is_int_ims = TRUE;

          CM_MSG_HIGH_0("IPAPP notification sent from HO_COMPLETE");
        }
        else
        {
          ho_comp_list->is_int_ims = FALSE;
        }

        /* Send HO complete event to clients if there was atleast one SRVCCed call
          with/without midcall. There cannot be a case with there was no SRVCCed
          call and CM received COMPLETE*/

        if(active_call_id!= CM_CALL_ID_INVALID)
        {
          cmwcall_send_srvcc_events_to_clients(CM_SRVCC_HO_IND_COMPLETE,active_call_id,ims_call_id, ll_as_id);
        }

    /* for int IMS, reset mmtel voice/vt status */
    if(cmcall_ptr(ims_call_id)!=NULL)
    {
      cmph_send_trm_priority_req(LTE_TRM_PRIORITY_LOW,
          cm_rpt_ptr->cmd.srvcc_handover_complete_ind.as_id);
      #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE
      cmcall_check_and_send_mmtel_status(cmcall_ptr(ims_call_id),FALSE);
      #endif
    }
        
        /* set total calls that got transferred to GW */
        ho_comp_list->number_calls= num_srvcc_calls;
        ho_comp_list->msg_hdr.inst_id= SYS_AS_ID_TO_INST_ID(ll_as_id);

        (void) cm_msgr_send( MM_CM_SRVCC_HO_COMPLETE_IND, MSGR_MM_CM,
            (msgr_hdr_s*)ho_comp_list, sizeof(cm_mid_srvcc_ho_comp_list_s_type) );

        ss = cmph_map_as_id_to_cm_ss(ll_as_id);

        {
          cmmsc_proc_cmd_pref_sys_chgd_s_type  cmd;
          ph_orig_top_ptr = cmtask_orig_para_get_top( ss == CM_SS_MAX ? CM_SS_MAIN : ss );
          if(ss == CM_SS_MAIN)
          {
            CM_MSG_HIGH_1("CM->MSGR: srvcc_complete: restore mode pref %d",cmph_ptr()->main_stack_info.pref_info.mode_pref);
          }
          else if(ss == CM_SS_HYBR_3)
          {
            #ifdef FEATURE_MMODE_HYBR3_ENABLED
              CM_MSG_HIGH_1("CM->MSGR: srvcc_complete: restore mode pref %d",cmph_ptr()->hybr_3_stack_info.pref_info.mode_pref);
            #endif
          }
          cmmsc_pack_cmd_for_pref_sys_chgd_per_top_queue(
              ss,
              SD_SS_PREF_REAS_RESTORE_USER_PREF,
              ph_orig_top_ptr->orig->orig_mode,
              FALSE,
              ph_orig_top_ptr,
              CM_ACT_UPDATE_REAS_NONE,
              &cmd
              );

          cmmsc_proc_cmd_pref_sys_chgd(&cmd);
        }
      /* HO processing complete, reset the IMS SRVCC cache */
      cm_ims_cache_reset();
      CM_MSG_HIGH_0("CM_NAS_SRVCC_HANDOVER_COMPLETE_IND, msgr sent HO calls");
      #ifndef FEATURE_MMODE_DISABLE_NON_LTE_VOICE
      cmcall_end_non_srvcc_call_ho_complete();
      #endif
      cm_mem_free(ho_comp_list);
  }
  /* NAS validation of srvcc call objects not successful, call SRVCC_HO_FAILURE*/
  else
  {
    cm_rpt_type   *cm_rpt_ptr        =  cm_get_cmd_buf();
    cm_rpt_ptr->hdr.cmd          = CM_MM_SRVCC_HANDOVER_FAILURE_IND;
    cm_rpt_ptr->cmd.srvcc_handover_failure_ind.as_id = ll_as_id;
    cm_set_ims_cache_error();
    CM_MSG_HIGH_0("CM_NAS_SRVCC_HANDOVER_COMPLETE_IND received with NAS FAIL");

    /*send fail cmd */
    cm_put_cmd (cm_rpt_ptr);
  }
  #endif
    break;

  case CM_MM_SRVCC_HANDOVER_FAILURE_IND:
    #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE) 

    CM_MSG_HIGH_2("RXD: CM_MM_SRVCC_HANDOVER_FAILURE_IND, as_id=%d, lte as_id=%d",
                  cm_rpt_ptr->cmd.srvcc_handover_failure_ind.as_id,
                  cmph_get_sub_with_lte_cap());
    ll_as_id = cmph_get_as_id_from_ll_ind(cm_rpt_ptr->cmd.srvcc_handover_failure_ind.as_id);
      
    /*i)Two phase - first remove new call objects created,
      ii) secondly send fail events */
 
    cmcall_obj_iterator_init(&call_itr);
 
    call_ptr = cmcall_obj_get_next(&call_itr);
 
    if(cm_get_srvcc_ho_error_state() != CM_SRVCC_HO_ERR_UNMANAGED_CALLS
     && cm_get_srvcc_ho_error_state() != CM_SRVCC_HO_UNMANAGED_CALL_COMBINATION  )
    {
      while(call_ptr != NULL)
      {
        /* Clear ext ims calls , both single/conf participants*/
        if ( call_ptr->is_srvcc_call && !cmwcall_is_int_ims())
        {
          #if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
          cmcall_end(call_ptr);
          #endif
          CM_MSG_HIGH_0("CM_MM_SRVCC_HANDOVER_FAILURE_IND, cleared ext IMS single calls");
        }
        /* Clear CS participant conf calls  */
        else if ( call_ptr->is_srvcc_call && cmwcall_is_int_ims())
        {
          if(call_ptr->is_mpty )
          {
            #if !defined(FEATURE_MMODE_DISABLE_LTE_VOICE)
            cmcall_end(call_ptr);
            #endif
            CM_MSG_HIGH_0("CM_MM_SRVCC_HANDOVER_FAILURE_IND, cleared IMS CS participant calls");
          }
        /* single ims calls , Retain IMS connection id  */
        else
        {
          CM_MSG_HIGH_0("CM_MM_SRVCC_HANDOVER_FAILURE_IND, retain IMS conn id and remove GW conn id");
          if (!cm_unreserve_connection_id(call_ptr->gw_connection_id, call_ptr->asubs_id ))
          {
            CM_ERR_2( "Could not unreserve W connection id %d for %d",
                       call_ptr->gw_connection_id, call_ptr->call_id);
          }

          cm_set_connection_id(call_ptr,call_ptr->ims_connection_id);
          cm_set_connection_id_src(call_ptr,0);
          call_ptr->parent_call_id = CM_CALL_ID_UNASSIGNED;
          call_ptr->sys_mode = SYS_SYS_MODE_LTE;
          call_ptr->is_srvcc_call = FALSE;
          call_ptr->cmcall_mode_info.info_type = CM_CALL_MODE_INFO_IP;
          if(call_ptr->call_state == CM_CALL_STATE_ORIG)
          {
            call_ptr->sr.orig_sent_on_mode = SYS_SYS_MODE_LTE;
          }
    
          /* reassign VT call type upon Failure */
          if(call_ptr->ims_call_type == CM_CALL_TYPE_VT)
          {
            call_ptr->call_type = call_ptr->ims_call_type;
            call_ptr->ims_call_type = CM_CALL_TYPE_NONE;
          }
        }
      }
      call_ptr = cmcall_obj_get_next(&call_itr);
     }//while

      /* if no int ims call present, we have to send fail with empty call */
      if(!cmwcall_is_int_ims())
      {
        cmwcall_send_srvcc_events_to_clients(CM_SRVCC_HO_IND_FAILURE,CM_CALL_ID_INVALID,ims_call_id, ll_as_id);
      }

      /* we have int ims single/conf call, send ipapp notify */
      else
      {
        cmwcall_send_srvcc_events_to_clients(CM_SRVCC_HO_IND_FAILURE,CM_CALL_ID_INVALID,ims_call_id, ll_as_id);
        if(cm_get_cache_ind().is_complete_with_fail)
        {
          cmipapp_send_srvcc_notification(CMIPAPP_SRVCC_CALL_CONTEXT_HO_FAIL_NOTIFY, ll_as_id);
          CM_MSG_HIGH_0("IPAPP HO CONTEXT FAIL sent");
        }
        else
        {
          cmipapp_send_srvcc_notification(CMIPAPP_SRVCC_HO_FAIL_NOTIFY, ll_as_id);
          CM_MSG_HIGH_0("IPAPP HO FAIL sent");
        }
      }
    }
    /* case where IMS fills in unmanageble calls in cache */
    else
    {
      if(cmwcall_is_int_ims())
      {
        cmipapp_send_srvcc_notification(CMIPAPP_SRVCC_HO_CANCEL_NOTIFY, ll_as_id);
      }

      cmwcall_send_srvcc_events_to_clients(CM_SRVCC_HO_IND_FAILURE,CM_CALL_ID_INVALID,ims_call_id, ll_as_id);
    }
    cm_set_srvcc_ho_error_state(CM_SRVCC_HO_ERR_NONE);
    /* HO processing complete, reset the IMS SRVCC cache */
    cm_ims_cache_reset();
    #endif
    break;

    case CM_GET_SRVCC_CONTEXT_REQ:
     #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && !defined(FEATURE_MMODE_DISABLE_NON_LTE_VOICE) 
      cmwcall_handle_get_srvcc_context_req(cm_rpt_ptr);
     #endif
     break;
     
    #endif
    #endif /* #ifndef FEATURE_MMODE_DISABLE_LTE_VOICE */
    #endif /* defined(FEATURE_MMODE_GW)*/

  case CM_CALL_CTRL_REQ:
    {
      if(!cmcc_call_control_processing(call_id,
        CM_CC_SUBSCRIPTION_SOURCE_NONE,
        cmph_get_gw_subscription_source(CM_SS_MAIN),
        CM_CALL_TYPE_PS_DATA,
        NULL,
        NULL,
        cmwcall_call_control_complete_cb,
        &(cm_rpt_ptr->cmd.call_ctrl_req)))
        {
          mm_cmd_type                   *mm_ptr;
          mm_ptr = mm_cm_get_cmd_buf(MMCM_CALL_CTRL_RSP);
          mm_ptr->cmd.cm_mm_call_ctrl_rsp.ps_rat = cm_rpt_ptr->cmd.call_ctrl_req.ps_rat;
          mm_ptr->cmd.cm_mm_call_ctrl_rsp.call_control_result = ESM_MSG_REJECTED;
          mm_put_cmd( mm_ptr );
        }
      break;
    }
    default:
      break;

  } /* switch */
}
#if defined(FEATURE_MMODE_GW)
/*===========================================================================

FUNCTION cmwcall_send_pdp_abort_req

DESCRIPTION
  Sends PDP ABORT request to SM.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwcall_send_pdp_abort_req(

  cm_ss_e_type    ss
    /* Stack on which the abort request has to be sent */

)
{
  sm_cmd_type                          *sm_ptr = NULL;
    /* SM command pointer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if there is a GW PS call.
  ** Note: cmcall_is_no_call_in_gw_ps returns TRUE if there is no GW PS call.
  */
  if(cmcall_is_no_call_in_gw_ps(ss) == FALSE )
  {
    /* Send abort request to SM so that it clears its objects.
    */
    sm_ptr = cm_sm_get_buf_else_err_fatal(SMREG_PDP_ABORT_REQ);

    CM_MSG_HIGH_0("TXD: SMREG_PDP_ABORT_REQ");

    cm_sm_send_cmd( sm_ptr );
  }

} /* cmwcall_send_pdp_abort_req() */
#endif
#endif
