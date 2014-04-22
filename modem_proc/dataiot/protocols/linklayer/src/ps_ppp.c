
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               P S _ P P P . C

GENERAL DESCRIPTION
  All of the external PPP protocol suite interface functions.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2019 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp.c_v   1.16   31 Jan 2003 18:46:32   ubabbar  $
  $Header: //components/rel/dataiot.mpss/2.0/protocols/linklayer/src/ps_ppp.c#2 $
  $Author: pwbldsvc $ $DateTime: 2023/04/06 06:44:01 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/19/19    ds     Code changes to post PPP_STOP cmd when called by mode handler
                   in case of PDN DOWN from N/w.
04/08/19    ds     Memory optimization related changes.
04/02/19    ds     Fixed Compiler warnings.
01/22/19    ds     Code changes to internally call ppp_abort() whenever 
                   ppp_stop() is called by client.
08/23/14    skc    Removing FEATURE_DATA_PS_HDLC_PP
06/06/14    vl     BSS optimization changes
11/03/13    cx     Set bridge device in ppp_start() in order to support 2nd 
                   RM call in PDP-PPP call
06/27/13    cx     Replace memcpy and memmove with memscpy and memsmove
10/25/12    fn     Remove usage of fatal error log messages
02/08/12    ash    Added NCPI_CB_IS_INITED Macro to check initilization status 
                   of VSNCP control block.
12/22/11    scb    Added support for IPv6 over PPP-DUN calls
11/28/11    jz     ppp control packet on dos(FEATURE_DATA_SEND_PPP_CTL_ON_DOS) 
11/02/11    jee    Fix graceful termination when phys link flow not enabled
10/17/11    ssh    eHRPD Always On support
09/20/11    jee    Fix to handle new call when device manager is cleaning up
09/14/11    ash    Cancel PPP setup times after AUTH success
06/06/11    ash    Added support for PPP Partial Context
05/16/11    ssh    Protect writes to device cache and inited flag
03/23/11    op     Data SU API cleanup
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
04/19/10    ash    Added changes to support Dual IP Feature.
03/09/10    mga    Added ppp_is_device_configured 
11/18/09    jee    Changes for the MMGSDI re-arch
10/01/09    mga    Added ppp_device_reset
08/31/09    mga    Merged from eHRPD branch
07/14/09    pp     ppp_cb_lock changes.
06/09/09    mga    Merged Compiler warnings fixes
05/07/09    pp     CMI Phase-4: SU Level API Effort.
12/12/08    scb    Adding PPP_STARTING_EV_MASK check to deregistration API.
11/19/08    scb    Fixed P2 framer and deframer issues.
11/12/08    scb    Added protocol processor specific functionality.
11/10/08    scb    Fixed QTF compilation warnings.
09/12/08    pp     Metainfo optimizations.
09/02/08    rnp    Added STA related changes.
05/21/08    scb    High LINT error fixes
05/04/08    scb    Bug fixes caught during PPP STA development
04/11/08    scb    Klocwork fixes
01/22/08    scb    Mainlined features under FEATURE_JCDMA_1X
12/19/07    scb    Added the framer_complete_cb function for the protocol
                   processor framer functionality.
11/26/07    scb    Added the ppp_get_phase() API to obtain the current phase
                   PPP is in. Added for FEATURE_CHUM featurization.
08/27/07    scb    Added the PPP_STARTING_EV_MASK for the PPP_PROTOCOL_AUTH
                   protocol in ppp_event_register as a part of the UNDP 
                   requirements.
09/13/07    scb    Replaced the ppp_update_ipcp_config() function with a
                   more generic ppp_update_proto_config() function.
07/24/07    scb    Modified critical section around session handle check in 
                   ppp_abort() to avert race condition.
07/14/07    scb    Added critical section around session handle check in 
                   ppp_abort() to avert race condition.
07/06/07    scb    Added the auth_proto_mask variable to 
                   ppp_set_auth_server_opts() and set it to perform both PAP
                   and CHAP.
05/14/07    scb    Changed variable names to avoid compilation error for
                   JCDMA_1X builds
04/27/07    scb    Added ppp_get_device_constant() to return default values 
                   for the specified PPP constant,originally populated during
                   startup.
04/27/07    scb    Removed LINT suppressions
04/19/07    scb    Added support for the KDDI PPP compliance requirement
                   to configure Application specific PPP constants
04/12/07    ssh    Disable DNS negotiation for resyncs (JCDMA)
02/28/07    ssh    Removed VJ from IPv6CP code
02/08/07    scb    Fixed Klocwork High errors
01/08/07    ssh    Added PPP_STARTING_EV
11/01/06    ssh    Updated ppp_update_auth_setting to support auth_proto_mask
10/12/06    ssh    JCDMA: Setting abort_flag to FALSE for AN device
08/04/06    ssh    Now using featurized critical sections for event cbacks
06/05/06    mp     Moved DPL link logging control block from ps_iface to
                   ps_phys_link and added DPL PPP Partial Logging
06/07/06    ssh    Added ppp_get_hdlc_handle()
05/04/06    ssh    Added ppp_update_password() and ppp_update_user_id()
04/28/06    ifk    Added additional critical sections.
04/20/06    ssh    Changed ppp_input() to handle all PPP modes (including
                   pseudonet) by invoking rx_pkt_hdlr_f_ptr
04/05/06    ifk    Added support for initializing IPv6 compression option.
03/06/06    ssh    Fixed HDLC_BAD_HANDLE processing in ppp_input()
02/21/06    ssh    Fixed pointer handling in ppp_input(), change to
                   pppi_config_check().
02/06/06    ssh    Changed TASKLOCKs to critical sections (for L4 builds).
01/18/06    ssh    pppi_config_check() should check for invalid LCP/HDLC mode
01/12/06    ssh    ppp_input() and ppp_output() should take PPP device as arg
12/09/05    sv     Added support for new data path framework.
11/29/05    jd     Call PPP logging macro wrapper to reduce CPU loading
11/29/05    ssh    Moved protocol mode setting from DS context to PS context,
                   fixed session_handle setting in ppp_start()
11/29/05    ssh    Logging incoming packet before discarding in ppp_input()
11/28/05  ifk/ssh  Added support for IPHC and ROHC over PPP under
                   FEATURE_DATA_PS_PPP_IPHC and FEATURE_DATA_PS_PPP_ROHC.
09/01/05    ssh    Changed hdlc_unframe() invokation in ppp_input() to
                   match new semantic
08/16/05    msr    Fixed PS_BRANCH_TASKFREE()
08/11/05    ssh    Changed ppp_set_mode(), now setting PPP mode only in
                   internal function pppi_set_mode()
07/21/05    ssh    Changes pursuant to new ppp_fail_reason_e_type
06/29/05    ssh    Fixed logging issue in ppp_input()
06/01/05    rt     Added ps_ppp_logging.h inclusion.
05/31/05    jd     Added legacy PPP logging in ppp_input()
05/19/05    ssh    Error handling for hdlc_unframe() in ppp_input()
04/27/05    mct    Fixed session_handle to assign value to configbuf_ptr, in
                   cases where ppp cleanup doesn not occur.
04/11/05    ssh    Added handling for session_handle wraparound and passing
                   session_handle in config_buf_ptr in ppp_start()
01/19/05    jd     Added ppp_update_auth_setting to update auth requirement
                   at runtime.
11/01/04    rr/ks  Added primary_nbns and secondary_nbns addresses as input
                   parameters to ppp_set_v4_server_opts() for WINS. Used by
                   UMTS. For 1x these are passed in as zeros by the mode
                   handler as 1x currently doesn't support WINS.
10/18/04    ifk    In ppp_set_mode() check for bridge device if in psuedo
                   net or full net mode.
09/30/04    ifk    Changed check for all NCPs being disabled to print a LOG_MSG_INFO2
                   instead of returning an error from pppi_config_check()
09/25/04    vas    Added check for device in ppp_config_check()
07/15/04    ifk    Changed enum values of cp_mode_type, seperated PPP_RESYNC
                   command from PPP_START.  Added protocol to ppp_resync()
06/15/04    ifk    Took out function ppp_reg_event() added ppp_event_alloc(),
                   ppp_event_register(), ppp_event_deregister() and
                   ppp_event_dealloc()
06/11/04    vp     Changes for representation of IP addresses as struct ps_in_addr.
05/03/04    sy     Added rx signal handler to PPP interface. Also added
                   ppp_inout() which uses the frames received on the
                   forward link pass it to the HDLC deframer.
04/30/04    usb    Removed func callback to get peer info,
                   can be retrieved directly from iface_cb.
04/21/04    ifk    Clear an NCP from ncp_mask in ppp_stop() to avoid race
                   condition.
04/09/04    ifk    Check bridge device in ppp_set_mode() to PS task context.
03/15/04    ifk    Added PPP configuration buffer moving code changing ppp_cb
                   to PS context function pppi_start() from ppp_start() and
                   ppp_resync()
02/26/04    usb    Reset iface_ptr only in PPP task context not in ppp_abort.
02/03/04    sv     Changed IP6CP to IPV6CP.
01/31/04    usb    Support for new config item passive_wakeup_on_data.
01/08/04    aku    Changed check in ppp_stop() to include IP6CP.
11/21/03    jd     In ppp_resync(), set all passive protocols to active
                   before restarting PPP.
10/30/03    mvl    changed ppp_get_auth_info() to return auth_id.
10/09/03    ifk    Removed setting of magic number in ppp_get_default_opts()
09/25/03    ifk    Added IPv6CP specific code
09/18/03    jd     In ppp_get_dev_opts(), return auth info if device
                   authenticates, and clear auth info if not.
                   In set_auth_server_opts, if no challenge name provided,
                   set challenge name length to 0.
09/16/03    usb    Passing user data in call to resync event callback.
08/15/03    aku    Use ps_iface macro to access graceful_dormant_close
                   variable
08/13/03    ifk    Event handling update and seperation of LCP and IPCP as
                   precursor for IPv6 support.
05/28/03    usb    Fixed ppp_resync to prevent potential race conditions
05/22/03    usb    Make sure PPP device is configured before changing mode
                   in ppp_set_mode(), also warpped under TASKLOCK.
05/14/03    usb    Corrected auth parameter validation in ppp_i_config.
04/23/03    usb    Fixed ppp_resync() to pass in the correct device and
                   session handle wih the cmd to PS. Fixed
                   ppp_set_v4_server_opts() to not assign DNS addresses if
                   not asked by peer.
04/18/03    vsk    Return NULL if Auth was not performed in ppp_get_auth_info()
04/18/03    vsk    initialize serves_address in ppp_update_ipcp_config()
04/11/03    mvl    Added support for SNOOP_INTERNAL mode to set_mode(), Removed
                   INLINE to fix link issue and added the
                   set_auth_server_opts().
04/08/03    mvl    Cleanup up file
04/07/03    usb    Renamed ppp_set_resync_opts() to
                   ppp_update_ipcp_config(), Added ppp_inject_packet()
04/06/03    mvl    Addition of capabilities to support server style
                   authentication for both PAP and CHAP.
03/10/03    usb    Fixed ppp_set_resnc_options() to complete set of options,
                   Fixed ppp_set_v4_server_opts() to appropriately turn the
                   bits off for LOCAL side and made local_addr optional.
01/27/03    usb    PPP mode is always set in PS context, added
                   PPP_SET_MODE_CMD
12/10/02    usb    Moved setting PPP mode to NONE from ppp_abort to
                   pppi_abort (PS context).
11/14/02    jd/usb Split get_dev_opts into get_dev_setup & get_protocol_opts
                   removed 'swap' param from get dev opts, added
                   get_bridge_dev_opts for this purpose.
                   Added set_resync_opts for getting iface ready for snoop
                   resync (sets will/want with given config)
09/25/02    mvl    Modified signal usage.
09/24/02    usb    Added user data to ppp config options so that they can be
                   passed on to ppp tx and rx functions
09/19/02    mvl    Added ppp_set_v4_server_opts() to make server config.
                   Making sure that the bridge device is set when switching
                   to psuedo or full net mode.  Fixed some issues with new
                   serves_addr field in configuration.
09/18/02    mvl    Added user_data to ppp event callbacks.  Setting mode in
                   start() and abort() calls, not internally.  Added explicit
                   configuration for client vs. server behavior.  Removed
                   do_mip.
09/12/02    mvl    Added support for a setup timeout, TASKLOCK()ed
                   ppp_set_mode().
09/06/02    mvl    fixed assert()s.
08/13/02    mvl    Session handle support and moved set_mode function from
                   netmdl.
08/06/02    mvl    Reworked authentication.
07/31/02    usb    Renamed psmisc.h to ps_utils.h
07/30/02    mvl    Put TASKLOCK() around invokation of event callbacks.
07/29/02    mvl    Setting do_auth variable when configuring PPP, fixed
                   TASKLOCK()s.
07/25/02    mvl    Fixed bug in get_default_opts() where values were not
                   being set correctly.  Updates for PPP renaming.
07/24/02    mvl    Fixed TASKLOCK() bug, and can now handle stopping/aborting
                   a device that is not running.
07/16/02    mvl    Configuration related fixes.
07/11/02    mvl    Updates for new PPP architecture.
07/02/02    mvl    Changes to decouple from PS.  Added ppp_reg_event().
05/22/02    mvl    created file with all of the external information for
                   users of ppp.
===========================================================================*/


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "amssassert.h"
#include "rex.h"
#include "task.h"
#include "dsm.h"
#include "ps_ppp.h"
#include "ps_pppi.h"
#include "ps_ppp_task.h"
#include "ps_ppp_defs.h"
#include "ps_ppp_lcp.h"
#include "ps_pppi_lcp.h"
#include "ps_ppp_auth.h"
#include "ps_pppi_auth.h"
#include "ps_ppp_ipcp.h"
#include "ps_pppi_ipcp.h"
#include "ps_pppi_fsm.h"
#include "ps_mem.h"
#include "ds_system_heap.h"
#ifdef FEATURE_DATA_PS_IPV6
#include "ps_ppp_ipv6cp.h"
#endif /* FEATURE_DATA_PS_IPV6 */
#include "ps_utils.h"
#include "ps_hdlc_lib.h"

#ifdef FEATURE_DATA_PS_PPP_LOGGING
#include "ps_ppp_logging.h"
#endif /* FEATURE_DATA_PS_PPP_LOGGING */

#include "ds_Utils_DebugMsg.h"

#include "ds_bearer_manager.h"
#include "ds_pdn_manager.h"
#include "ds_fwki.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            FORWARD DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#define PS_PPP_ASSERT(expression)  \
          ps_ppp_assert_wrapper(__LINE__, expression)

#define PPP_UPDATE_PROTO_CONFIG(proto_info, proto_value_type)      \
 {\
  fsm_ptr->local.will  = proto_info.will_mask[FSM_LOCAL]; \
  fsm_ptr->local.want  = proto_info.want_mask[FSM_LOCAL]; \
  fsm_ptr->remote.will = proto_info.will_mask[FSM_REMOTE];\
  fsm_ptr->remote.want = proto_info.want_mask[FSM_REMOTE];\
  fsm_ptr->local.work  = fsm_ptr->local.want;                         \
  fsm_ptr->remote.work = fsm_ptr->remote.want;                        \
  memscpy(fsm_ptr->remote.want_pdv,                                    \
         sizeof(proto_value_type),                                    \
         &(proto_info.want_vals[FSM_REMOTE]),              \
         sizeof(proto_value_type));                                    \
  memscpy(fsm_ptr->local.want_pdv,                                     \
         sizeof(proto_value_type),                                    \
         &(proto_info.want_vals[FSM_LOCAL]),               \
         sizeof(proto_value_type));                                    \
  memscpy(fsm_ptr->remote.work_pdv,                                    \
         sizeof(proto_value_type),                                    \
         &(proto_info.work_vals[FSM_REMOTE]),              \
         sizeof(proto_value_type));                                    \
  memscpy(fsm_ptr->local.work_pdv,                                     \
         sizeof(proto_value_type),                        \
         &(proto_info.work_vals[FSM_LOCAL]),               \
         sizeof(proto_value_type));                        \
 }


/*---------------------------------------------------------------------------
  struct ppp_dev_const_s_type
  Structure for setting/getting the constant parameters on a particular
  interface. 
---------------------------------------------------------------------------*/
typedef struct ppp_dev_const_s
{
  uint8  lcp_try_configure;
  uint16 lcp_ack_timeout;
  uint8  ipcp_try_configure;
  uint16 ipcp_ack_timeout;
  uint8  auth_retry_limit;
  uint16 auth_timeout;
#ifdef FEATURE_DATA_PS_IPV6
  uint8  ipv6cp_try_configure;
  uint16 ipv6cp_ack_timeout;
#endif /* FEATURE_DATA_PS_IPV6 */
} ppp_dev_const_s_type;

static ppp_dev_const_s_type ppp_def_dev_const[PPP_MAX_DEV];


static int pppi_config_check
(
  ppp_dev_opts_type *opts_ptr
);

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================

FUNCTION ps_ppp_assert_wrapper

DESCRIPTION
  Wrapper function for PS_PPP_ASSERT
 
DEPENDENCIES 
  None

RETURN VALUE
  None
 
SIDE EFFECTS

===========================================================================*/
static void ps_ppp_assert_wrapper
(
  unsigned int     line_num,
  int              expression
)
{
  if ( !expression )
  {
    ERR_FATAL("PS_PPP_ERR_FATAL at line:%d ",  
                                   line_num,0,0);  
  }
}/* ps_ppp_assert_wrapper */

/*===========================================================================
FUNCTION PPP_GET_DEFAULT_OPTS()

DESCRIPTION
  This function will populate the options data structure with the default
  values for a PPP session.  The intention is to allow users to get a default
  configuration and modify only the fields that they wish and the fields that
  are set to invalid values (the owner of the configuration structure must
  set these values by hand).

  The following fields are set to invalid values:
    lcp_info.mode   -> PPP_CP_MODE_INVALID
    ipcp_info.mode  -> PPP_CP_MODE_INVALID
    rx_f_ptr        -> NULL
    rx_iface_ptr    -> NULL
    hdlc_mode       -> HDLC_INVALID_MODE
    bridge_dev      -> PPP_INVALID_DEV
    serves_addrs    -> FALSE
    auths_peer      -> FALSE
    rx_signal       -> PPP_INVALID_SIGNAL
    rx_sig_f_ptr    -> pppi_get_data_cback;

  NOTE: authentication is not requested in LCP, the desired local IPCP values
  for compression are used.

PARAMETERS
  config_ptr: configuration structure to fill in with the default information

RETURN VALUE
  The passed structure of type ppp_dev_opts_type is initialized with default
  options.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_get_default_opts
(
  ppp_dev_opts_type *config_ptr
)
{
  lcp_value_type    *lcp_default_opts_vals_ptr;
  ipcp_value_type   *ipcp_default_opts_vals_ptr;
  ipv6cp_value_type *ipv6cp_default_opts_vals_ptr;
  negotiate_t        lcp_accept;
  negotiate_t        ipcp_accept;
  negotiate_t        ipv6cp_accept;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == config_ptr )
  {
    PS_PPP_ASSERT(0);
    return;
  }

  /*------------------------------------------------------------------------- 
    Get LCP default values & accept bitmask.
  -------------------------------------------------------------------------*/
  lcp_default_opts_vals_ptr = lcp_get_default_opts_vals_ptr();
  lcp_accept                = lcp_get_accept_mask();

  /*------------------------------------------------------------------------- 
    Get IPCP default values & accept bitmask.
  -------------------------------------------------------------------------*/
  ipcp_default_opts_vals_ptr = ipcp_get_default_opts_vals_ptr();
  ipcp_accept                = ipcp_get_accept_mask();

  /*------------------------------------------------------------------------- 
    Get IPv6CP default values & accept bitmask.
  -------------------------------------------------------------------------*/
  ipv6cp_default_opts_vals_ptr = ipv6cp_get_default_opts_vals_ptr();
  ipv6cp_accept                = ipv6cp_get_accept_mask();


  memset(config_ptr, 0, sizeof(ppp_dev_opts_type));

  /*-------------------------------------------------------------------------
    The other default values have been set to zero by the memset above, but
    the HDLC mode, the bridge dev and the rx_signal need to be set explicitly
    to their non zero invalid settings.
  -------------------------------------------------------------------------*/
  config_ptr->hdlc_mode  = HDLC_INVALID_MODE;
  config_ptr->bridge_dev = PPP_INVALID_DEV;
  config_ptr->rx_signal  = PPP_INVALID_SIGNAL;
  config_ptr->rx_sig_f_ptr = pppi_get_data_cback;

  /*-------------------------------------------------------------------------
    Set up LCP options to first request defaults - set the mode to invalid,
    then setup the masks: we want to negotiate ACCM, ACFC. PFC and the magic
    number (but NOT auth).  For the peer we will not force them to do
    anything, but will accept anything.  We will accept whatever is defined
    in lcp_accept.
  -------------------------------------------------------------------------*/
  config_ptr->lcp_info.mode = PPP_CP_MODE_INVALID;
  config_ptr->lcp_info.will_mask[FSM_REMOTE] = lcp_accept;
  config_ptr->lcp_info.will_mask[FSM_LOCAL]  = lcp_accept;
  config_ptr->lcp_info.want_mask[FSM_REMOTE] = 0;
  config_ptr->lcp_info.want_mask[FSM_LOCAL]  = LCP_N_ACCM | LCP_N_ACFC |
                                               LCP_N_PFC  | LCP_N_MAGIC;

  /*-------------------------------------------------------------------------
    setup the want and work protocol dependent variables using the default
    values - the only exception being the local want, where we prefer to have
    more optimized values for ACCM
  -------------------------------------------------------------------------*/
  memscpy((config_ptr->lcp_info.want_vals) + (uint8)FSM_LOCAL,
           sizeof(lcp_value_type),
           lcp_default_opts_vals_ptr,
           sizeof(lcp_value_type));
  config_ptr->lcp_info.want_vals[FSM_LOCAL].accm = 0;
  memscpy((config_ptr->lcp_info.work_vals) + (uint8)FSM_LOCAL,
           sizeof(lcp_value_type),
           (config_ptr->lcp_info.want_vals) + (uint8)FSM_LOCAL,
           sizeof(lcp_value_type));
  memscpy((config_ptr->lcp_info.want_vals) + (uint8)FSM_REMOTE,
           sizeof(lcp_value_type),
           lcp_default_opts_vals_ptr,
           sizeof(lcp_value_type));
  memscpy((config_ptr->lcp_info.work_vals) + (uint8)FSM_REMOTE,
           sizeof(lcp_value_type),
           (config_ptr->lcp_info.want_vals) + (uint8)FSM_REMOTE,
           sizeof(lcp_value_type));

  /*-------------------------------------------------------------------------
    Set up IPCP options to first request defaults - set the mode to active,
    then setupthe masks: we want to negotiate compression, an address and the
    DNS server addresses.  We will accept whatever is defined in ipcp_accept.
  -------------------------------------------------------------------------*/
  config_ptr->ipcp_info.mode = PPP_CP_MODE_INVALID;
  config_ptr->ipcp_info.will_mask[FSM_REMOTE] = ipcp_accept;
  config_ptr->ipcp_info.will_mask[FSM_LOCAL]  = ipcp_accept;
  config_ptr->ipcp_info.want_mask[FSM_REMOTE] = IPCP_N_1ADDRESS;
  config_ptr->ipcp_info.want_mask[FSM_LOCAL]  = ipcp_accept;

  /*-------------------------------------------------------------------------
    Turn off WINS bits from will and want masks to ensure that 1x is not
    affected by WINS addition
  -------------------------------------------------------------------------*/

  config_ptr->ipcp_info.will_mask[FSM_REMOTE] &= ~(IPCP_N_PRIMARY_NBNS |
                                                   IPCP_N_SECONDARY_NBNS);
  config_ptr->ipcp_info.will_mask[FSM_LOCAL]  &= ~(IPCP_N_PRIMARY_NBNS |
                                                   IPCP_N_SECONDARY_NBNS);
  config_ptr->ipcp_info.want_mask[FSM_LOCAL]  &= ~(IPCP_N_PRIMARY_NBNS |
                                                   IPCP_N_SECONDARY_NBNS);

  /*-------------------------------------------------------------------------
    setup the want protocol dependent variables using the default values
  -------------------------------------------------------------------------*/
  memscpy((config_ptr->ipcp_info.want_vals) + (uint8)FSM_LOCAL,
           sizeof(ipcp_value_type),
           ipcp_default_opts_vals_ptr,
           sizeof(ipcp_value_type));
  memscpy((config_ptr->ipcp_info.want_vals) + (uint8)FSM_REMOTE,
           sizeof(ipcp_value_type),
           ipcp_default_opts_vals_ptr,
           sizeof(ipcp_value_type));

  /*-------------------------------------------------------------------------
    setup the compression protocol information
  -------------------------------------------------------------------------*/
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].compression = PPP_VJCH_PROTOCOL;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].vj_config.num_slots =
    IPCP_SLOT_DEFAULT;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].vj_config.slot_comp =
    IPCP_SLOT_COMPRESS;
#ifdef FEATURE_DATA_PS_PPP_ROHC
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].rohc_config.max_cid =
    IPCP_ROHC_MAX_CID_DEFAULT;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].rohc_config.mrru =
    IPCP_ROHC_MRRU_DEFAULT;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].rohc_config.max_header =
    IPCP_ROHC_MAX_HEADER_DEFAULT;
  /*-------------------------------------------------------------------------
   RFC 3242 2.2.  PROFILES Suboption
   The set of profiles to be enabled is subject to negotiation.  Most
   initial implementations of ROHC implement profiles 0x0000 to 0x0003.
   This option MUST be supplied.
   Including profiles 0x0000 through 0x0003 for now as RFC suggests.
  -------------------------------------------------------------------------*/
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].rohc_config.no_of_profiles = 3;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].rohc_config.profiles[0] =
    ROHC_PROFILE_UNCOMPRESSED;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].rohc_config.profiles[1] =
    ROHC_PROFILE_RTP;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].rohc_config.profiles[2] =
    ROHC_PROFILE_UDP;
#endif /* FEATURE_DATA_PS_PPP_ROHC */

#ifdef FEATURE_DATA_PS_PPP_IPHC
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].iphc_config.tcp_space =
    IPCP_IPHC_TCP_SPACE_DEFAULT;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].iphc_config.non_tcp_space =
    IPCP_IPHC_NON_TCP_SPACE_DEFAULT;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].iphc_config.f_max_period =
    IPCP_IPHC_F_MAX_PERIOD_DEFAULT;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].iphc_config.f_max_time =
    IPCP_IPHC_F_MAX_TIME_DEFAULT;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].iphc_config.max_header =
    IPCP_IPHC_MAX_HEADER_DEFAULT;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].iphc_config.rtp_enabled = FALSE;
#endif /* FEATURE_DATA_PS_PPP_IPHC */

  /*-------------------------------------------------------------------------
    Copy the work protocol dependent values from the want values.
  -------------------------------------------------------------------------*/
  memscpy((config_ptr->ipcp_info.work_vals) + (uint8)FSM_LOCAL,
           sizeof(ipcp_value_type),
           (config_ptr->ipcp_info.want_vals) + (uint8)FSM_LOCAL,
           sizeof(ipcp_value_type));
  memscpy((config_ptr->ipcp_info.work_vals) + (uint8)FSM_REMOTE,
           sizeof(ipcp_value_type),
           (config_ptr->ipcp_info.want_vals) + (uint8)FSM_REMOTE,
           sizeof(ipcp_value_type));

#ifdef FEATURE_DATA_PS_IPV6
  /*-------------------------------------------------------------------------
    Set up IPV6CP options to first request defaults - set the mode to
    invalid, then set up the masks: we want to negotiate compression and
    an iface.    We will accept whatever is defined in ipv6cp_accept.
  -------------------------------------------------------------------------*/
  config_ptr->ipv6cp_info.mode = PPP_CP_MODE_INVALID;
  config_ptr->ipv6cp_info.will_mask[FSM_REMOTE] = ipv6cp_accept;
  config_ptr->ipv6cp_info.will_mask[FSM_LOCAL]  = ipv6cp_accept;
  config_ptr->ipv6cp_info.want_mask[FSM_REMOTE] = IPV6CP_N_IFACE_ID;
  config_ptr->ipv6cp_info.want_mask[FSM_LOCAL]  = ipv6cp_accept;

  /*-------------------------------------------------------------------------
    setup the want protocol dependent variables using the default values
  -------------------------------------------------------------------------*/
  memscpy((config_ptr->ipv6cp_info.want_vals) + (uint8)FSM_LOCAL,
           sizeof(ipv6cp_value_type),
           ipv6cp_default_opts_vals_ptr,
           sizeof(ipv6cp_value_type));
  memscpy((config_ptr->ipv6cp_info.want_vals) + (uint8)FSM_REMOTE,
           sizeof(ipv6cp_value_type),
           ipv6cp_default_opts_vals_ptr,
           sizeof(ipv6cp_value_type));

  /*-------------------------------------------------------------------------
    setup the compression protocol information
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_PPP_ROHC
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].rohc_config.max_cid =
    IPV6CP_ROHC_MAX_CID_DEFAULT;
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].rohc_config.mrru =
    IPV6CP_ROHC_MRRU_DEFAULT;
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].rohc_config.max_header =
    IPV6CP_ROHC_MAX_HEADER_DEFAULT;
  /*-------------------------------------------------------------------------
   RFC 3242 2.2.  PROFILES Suboption
   The set of profiles to be enabled is subject to negotiation.  Most
   initial implementations of ROHC implement profiles 0x0000 to 0x0003.
   This option MUST be supplied.
   Including profiles 0x0000 through 0x0003 for now as RFC suggests.
  -------------------------------------------------------------------------*/
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].rohc_config.no_of_profiles = 3;
  config_ptr->ipcp_info.want_vals[FSM_LOCAL].rohc_config.profiles[0] =
    ROHC_PROFILE_UNCOMPRESSED;
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].rohc_config.profiles[1] =
    ROHC_PROFILE_RTP;
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].rohc_config.profiles[2] =
    ROHC_PROFILE_UDP;
#endif /* FEATURE_DATA_PS_PPP_ROHC */

#ifdef FEATURE_DATA_PS_PPP_IPHC
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].iphc_config.tcp_space =
    IPV6CP_IPHC_TCP_SPACE_DEFAULT;
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].iphc_config.non_tcp_space =
    IPV6CP_IPHC_NON_TCP_SPACE_DEFAULT;
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].iphc_config.f_max_period =
    IPV6CP_IPHC_F_MAX_PERIOD_DEFAULT;
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].iphc_config.f_max_time =
    IPV6CP_IPHC_F_MAX_TIME_DEFAULT;
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].iphc_config.max_header =
    IPV6CP_IPHC_MAX_HEADER_DEFAULT;
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].iphc_config.rtp_enabled = FALSE;
#endif /* FEATURE_DATA_PS_PPP_IPHC */

  /*-------------------------------------------------------------------------
    Copy the work protocol dependent values from the want values.
  -------------------------------------------------------------------------*/
  memscpy((config_ptr->ipv6cp_info.work_vals) + (uint8)FSM_LOCAL,
          sizeof(ipv6cp_value_type),
          (config_ptr->ipv6cp_info.want_vals) + (uint8)FSM_LOCAL,
          sizeof(ipv6cp_value_type));
  memscpy((config_ptr->ipv6cp_info.work_vals) + (uint8)FSM_REMOTE,
           sizeof(ipv6cp_value_type),
           (config_ptr->ipv6cp_info.want_vals) + (uint8)FSM_REMOTE,
           sizeof(ipv6cp_value_type));
#endif /* FEATURE_DATA_PS_IPV6 */

  /*-------------------------------------------------------------------------
    Set up the default values for the world phone related PPP options.
  -------------------------------------------------------------------------*/
  config_ptr->set_stabilize_accm_delay = TRUE;
  config_ptr->resync_dns_opts = TRUE;

#ifdef FEATURE_DATA_PS_ENFORCE_AUTH
  /*-------------------------------------------------------------------------
    Set the allow_no_auth to FALSE. This is done to mandate auth for eHRPD.
  -------------------------------------------------------------------------*/
  config_ptr->allow_no_auth = FALSE;
#else
  config_ptr->allow_no_auth = TRUE;
#endif /* FEATURE_DATA_PS_ENFORCE_AUTH */

#ifdef FEATURE_DATA_SEND_PPP_CTL_ON_DOS
  config_ptr->dos_is_enabled = FALSE;
#endif /*FEATURE_DATA_SEND_PPP_CTL_ON_DOS*/
} /* ppp_get_default_opts() */

/*===========================================================================
FUNCTION PPP_GET_AUTH_INFO()

DESCRIPTION
  This function will return a pointer to a data structure containing all of
  th authentication information.  The macro PPP_AUTH_WAS_CHAP() can be used
  to determine if CHAP or PAP was done.  The return value will be NULL if the
  device is out of bounds OR no authentication was performed.

  NOTE: this informaion should be treated as READ-ONLY changing values of the
        pointer information has unspecified effects.

PARAMETERS
  device: the PPP device from which to return the auth information.
  auth_info_ptr: pointer to auth_info_type where the auth_info will be copied.
  auth_id_ptr:   pointer to uint8 where the auth ID will be copied - if
                 passed in as NULL, will be ingored.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_get_auth_info
(
  ppp_dev_enum_type   device,
  ppp_auth_info_type *auth_info_ptr,
  uint8              *auth_id_ptr
)
{
  ppp_type *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(device  >= PPP_MAX_DEV ||
     auth_info_ptr == NULL)
  {
    ASSERT(0);
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  if( ppp_cb_array[device].auth.mode != PPP_NO_AUTH )
  {
    /*-----------------------------------------------------------------------
      copy the auth info into the passed in data structure.  If the auth_id
      pointere is valid then copy that auth ID in there as well.
    -----------------------------------------------------------------------*/
    DS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
    memscpy(auth_info_ptr, 
            sizeof(ppp_auth_info_type),
            &(ppp_cb_array[device].auth.info),
            sizeof(ppp_auth_info_type));

    if(auth_id_ptr != NULL)
    {
      *auth_id_ptr = ppp_cb_array[device].auth.active_id;
    }
    DS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

    return 0;
  }
  else
  {
    return -1;
  }

} /* ppp_get_auth_info() */



/*===========================================================================
FUNCTION PPP_UPDATE_PROTO_CONFIG()

DESCRIPTION
  This function reconfigures protocol options for the specified PPP device.
  This prepares PPP device for subsequent resyncs after it has already come
  up or allows reconfiguration while PPP is in the middle of coming up.

PARAMETERS
  device:        PPP device to configure
  config_ptr:    configuration to use

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Modifies protocol options for the given PPP device
===========================================================================*/
void ppp_update_proto_config
(
  ppp_dev_enum_type       device,
  ppp_protocol_e_type     protocol,
  ppp_dev_opts_type       *config_ptr
)
{
  ppp_fsm_type * fsm_ptr = NULL;
  ppp_type     * ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Make sure the specified device is valid, the config_ptr is non-NULL
    and the protocol is valid.
  -------------------------------------------------------------------------*/
  if( ( PPP_MAX_DEV <= device ) || ( NULL == config_ptr ) || 
      ( PPP_PROTOCOL_MAX <= protocol ) )
  {
    /*Invalid params : device, protocol or NULL config_ptr*/
    PS_PPP_ASSERT(0);
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  DS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  switch(protocol)
  {
    case PPP_PROTOCOL_LCP:
      fsm_ptr = &(ppp_cb_array[device].fsm[protocol]);
      PPP_UPDATE_PROTO_CONFIG(config_ptr->lcp_info, lcp_value_type);
      break;

    case PPP_PROTOCOL_IPCP:
      fsm_ptr = &(ppp_cb_array[device].fsm[protocol]);
      PPP_UPDATE_PROTO_CONFIG(config_ptr->ipcp_info, ipcp_value_type);
      /*Update the fw_inst_ptr*/
      fsm_ptr->fwk_inst_ptr = config_ptr->ipcp_info.fwk_inst_ptr;
      /*---------------------------------------------------------------------
         Copy the flag which indicates if we are IPCP client or server
      ---------------------------------------------------------------------*/
      ppp_cb_array[device].serves_addrs = config_ptr->serves_addrs;
      break;

#ifdef FEATURE_DATA_PS_IPV6
    case PPP_PROTOCOL_IPV6CP:
      fsm_ptr = &(ppp_cb_array[device].fsm[protocol]);
      PPP_UPDATE_PROTO_CONFIG(config_ptr->ipv6cp_info, ipv6cp_value_type);
      /*---------------------------------------------------------------------
         Copy the flag which indicates if we are IPv6CP client or server
      ---------------------------------------------------------------------*/
      /*Update the fw_inst_ptr*/
      fsm_ptr->fwk_inst_ptr = config_ptr->ipv6cp_info.fwk_inst_ptr;
      ppp_cb_array[device].serves_addrs = config_ptr->serves_addrs;
      break;
#endif /* FEATURE_DATA_PS_IPV6 */

    case PPP_PROTOCOL_AUTH:
      if( ( 0 < config_ptr->auth_info.passwd_len) &&
          ( PPP_MAX_PASSWD_LEN >= config_ptr->auth_info.passwd_len ) )
      {
        memset( ppp_cb_array[device].auth.info.passwd_info,
                0,
                sizeof( ppp_cb_array[device].auth.info.passwd_info ) );
        memscpy( ppp_cb_array[device].auth.info.passwd_info,
                 PPP_MAX_PASSWD_LEN,
                 config_ptr->auth_info.passwd_info,
                 config_ptr->auth_info.passwd_len );
        ppp_cb_array[device].auth.info.passwd_len = 
                config_ptr->auth_info.passwd_len;
      }
      else
      {
       /* Invalid password len passed to update_proto_config */
       DATA_PROTO_ERROR_LOG_1(PS_BAD_ARG_OR_NULL_PTR,
                              config_ptr->auth_info.passwd_len);
      }
      
      if( ( 0 < config_ptr->auth_info.user_id_len ) &&
          ( PPP_MAX_USER_ID_LEN >= config_ptr->auth_info.user_id_len ) )
      {
        memset( ppp_cb_array[device].auth.info.user_id_info,
                0,
                sizeof( ppp_cb_array[device].auth.info.user_id_info ) );
        memscpy( ppp_cb_array[device].auth.info.user_id_info,
                 PPP_MAX_USER_ID_LEN,
                 config_ptr->auth_info.user_id_info,
                 config_ptr->auth_info.user_id_len );
        ppp_cb_array[device].auth.info.user_id_len = 
                config_ptr->auth_info.user_id_len;
      }
      else
      {
        /* Invalid userid len passed to update_proto_config */
        DATA_PROTO_ERROR_LOG_1(PS_BAD_ARG_OR_NULL_PTR,config_ptr->auth_info.user_id_len);
      }
      if( ( 0 < config_ptr->auth_info.challenge_name_len ) &&
          ( PPP_CHAP_CHAL_NAME_LEN >=
            config_ptr->auth_info.challenge_name_len) )
      {
        memset( ppp_cb_array[device].auth.info.challenge_name,
                0,
                sizeof( ppp_cb_array[device].auth.info.challenge_name ) );
        memscpy( ppp_cb_array[device].auth.info.challenge_name,
                 PPP_CHAP_CHAL_NAME_LEN, 
                 config_ptr->auth_info.challenge_name,
                 config_ptr->auth_info.challenge_name_len );
        ppp_cb_array[device].auth.info.challenge_name_len = 
                config_ptr->auth_info.challenge_name_len;
      }
      else
      {
        /* Invalid challenge name len passed to update_proto_config */
        DATA_PROTO_ERROR_LOG_1(PS_BAD_ARG_OR_NULL_PTR,
                              config_ptr->auth_info.challenge_name_len);  
      }
      break;
    default:
       /* Invalid protocol passed to update_proto_config */
       DATA_PROTO_ERROR_LOG_0(PS_BAD_ARG_OR_NULL_PTR);
      break;
  }

  DS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

} /* ppp_update_proto_config() */


/*===========================================================================
FUNCTION PPP_SET_V4_SERVER_OPTS()

DESCRIPTION
  This function will change the configuration passed in to serve IP
  addresses.  This includes setting the serves_addrs field, and setting the
  appropriate IPCP bits. An address must be passed in to be assigned to the
  peer. Other fields could be NULL and if so the corresponding bits are
  turned off.

PARAMETERS
  config_ptr:    configuration to convert
  local_addr:    the local IP address to advertize
  remote_addr:   the IP address to assign to peer
  primary_dns:   the primary DNS address to assign to peer
  secondary_dns: the secondary DNS address to assign to peer

RETURN VALUE
  The passed information is used to setup the passed config_ptr.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_set_v4_server_opts
(
  ppp_dev_opts_type *config_ptr,
  uint32             local_addr,
  uint32             remote_addr,
  uint32             primary_dns,
  uint32             secondary_dns,
  uint32             primary_nbns,
  uint32             secondary_nbns
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(config_ptr  == NULL || remote_addr == 0)
  {
    DATA_PROTO_ERROR_LOG_2(PS_BAD_ARG_OR_NULL_PTR,config_ptr,remote_addr);
    return;
  }

  config_ptr->serves_addrs = TRUE;

  /*-------------------------------------------------------------------------
    Set the remote address,  and associated bits in the masks. Since we
    are acting as server we must have an address to assign.
  -------------------------------------------------------------------------*/
  config_ptr->ipcp_info.want_mask[FSM_REMOTE] |= IPCP_N_1ADDRESS;
  config_ptr->ipcp_info.will_mask[FSM_REMOTE] |= IPCP_N_1ADDRESS;
  config_ptr->ipcp_info.work_vals[FSM_REMOTE].address.ps_s_addr = remote_addr;
  config_ptr->ipcp_info.want_vals[FSM_REMOTE].address.ps_s_addr = remote_addr;

  /*-------------------------------------------------------------------------
    if the local address is available, assign the values and set the
    LOCAL bits. Else turn them off since we don't want to ask for our
    address while acting as server.
  -------------------------------------------------------------------------*/
  if(local_addr != 0)
  {
    config_ptr->ipcp_info.want_mask[FSM_LOCAL] |= IPCP_N_1ADDRESS;
    config_ptr->ipcp_info.will_mask[FSM_LOCAL] |= IPCP_N_1ADDRESS;
    config_ptr->ipcp_info.work_vals[FSM_LOCAL].address.ps_s_addr = local_addr;
    config_ptr->ipcp_info.want_vals[FSM_LOCAL].address.ps_s_addr = local_addr;
  }
  else
  {
    config_ptr->ipcp_info.want_mask[FSM_LOCAL] &= ~IPCP_N_1ADDRESS;
    config_ptr->ipcp_info.will_mask[FSM_LOCAL] &= ~IPCP_N_1ADDRESS;
  }

  /*-------------------------------------------------------------------------
    If we have dns address to assign to peer then assign the values and set
    the REMOTE bits. LOCAL bits should always be turned off since we don't
    want to ask for DNS addresses while acting as server.
  -------------------------------------------------------------------------*/
  if(primary_dns != 0)
  {
    config_ptr->ipcp_info.will_mask[FSM_REMOTE] |= IPCP_N_PRIMARY_DNS;
    config_ptr->ipcp_info.work_vals[FSM_REMOTE].primary_dns.ps_s_addr =
      primary_dns;
    config_ptr->ipcp_info.want_vals[FSM_REMOTE].primary_dns.ps_s_addr =
      primary_dns;
  }
  else
  {
    config_ptr->ipcp_info.will_mask[FSM_REMOTE] &= ~IPCP_N_PRIMARY_DNS;
  }

  config_ptr->ipcp_info.want_mask[FSM_REMOTE] &= ~IPCP_N_PRIMARY_DNS;
  config_ptr->ipcp_info.want_mask[FSM_LOCAL] &= ~IPCP_N_PRIMARY_DNS;
  config_ptr->ipcp_info.will_mask[FSM_LOCAL] &= ~IPCP_N_PRIMARY_DNS;

  if(secondary_dns != 0)
  {
    config_ptr->ipcp_info.will_mask[FSM_REMOTE] |= IPCP_N_SECONDARY_DNS;
    config_ptr->ipcp_info.work_vals[FSM_REMOTE].secondary_dns.ps_s_addr =
      secondary_dns;
    config_ptr->ipcp_info.want_vals[FSM_REMOTE].secondary_dns.ps_s_addr =
      secondary_dns;
  }
  else
  {
    config_ptr->ipcp_info.will_mask[FSM_REMOTE] &= ~IPCP_N_SECONDARY_DNS;
  }

  config_ptr->ipcp_info.want_mask[FSM_REMOTE] &= ~IPCP_N_SECONDARY_DNS;
  config_ptr->ipcp_info.want_mask[FSM_LOCAL] &= ~IPCP_N_SECONDARY_DNS;
  config_ptr->ipcp_info.will_mask[FSM_LOCAL] &= ~IPCP_N_SECONDARY_DNS;

  /*-------------------------------------------------------------------------
    If we have nbns address to assign to peer then assign the values and set
    the REMOTE bits. LOCAL bits should always be turned off since we don't
    want to ask for NBNS addresses while acting as server.
  -------------------------------------------------------------------------*/
  if(primary_nbns != 0)
  {
    LOG_MSG_INFO1_1("PRIMARY NBNS ADDRESS ASSIGNED %lx", primary_nbns);
    config_ptr->ipcp_info.will_mask[FSM_REMOTE] |= IPCP_N_PRIMARY_NBNS;
    config_ptr->ipcp_info.work_vals[FSM_REMOTE].primary_nbns.ps_s_addr =
      primary_nbns;
    config_ptr->ipcp_info.want_vals[FSM_REMOTE].primary_nbns.ps_s_addr =
      primary_nbns;
  }
  else
  {
    config_ptr->ipcp_info.will_mask[FSM_REMOTE] &= ~IPCP_N_PRIMARY_NBNS;
  }

  config_ptr->ipcp_info.want_mask[FSM_REMOTE] &= ~IPCP_N_PRIMARY_NBNS;
  config_ptr->ipcp_info.want_mask[FSM_LOCAL] &= ~IPCP_N_PRIMARY_NBNS;
  config_ptr->ipcp_info.will_mask[FSM_LOCAL] &= ~IPCP_N_PRIMARY_NBNS;

  if(secondary_nbns != 0)
  {
    config_ptr->ipcp_info.will_mask[FSM_REMOTE] |= IPCP_N_SECONDARY_NBNS;
    config_ptr->ipcp_info.work_vals[FSM_REMOTE].secondary_nbns.ps_s_addr =
      secondary_nbns;
    config_ptr->ipcp_info.want_vals[FSM_REMOTE].secondary_nbns.ps_s_addr =
      secondary_nbns;
  }
  else
  {
    config_ptr->ipcp_info.will_mask[FSM_REMOTE] &= ~IPCP_N_SECONDARY_NBNS;
  }

  config_ptr->ipcp_info.want_mask[FSM_REMOTE] &= ~IPCP_N_SECONDARY_NBNS;
  config_ptr->ipcp_info.want_mask[FSM_LOCAL] &= ~IPCP_N_SECONDARY_NBNS;
  config_ptr->ipcp_info.will_mask[FSM_LOCAL] &= ~IPCP_N_SECONDARY_NBNS;

} /* ppp_set_v4_server_opts() */



#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION PPP_SET_V6_SERVER_OPTS()

DESCRIPTION
  This function will change the configuration passed in to serve IP
  addresses.  This includes setting the addresses, serves_addrs, and the
  appropriate IPV6CP bits. A local IID (serves as gateway IID) and a 
  remote IID must be passed in to be assigned to the peer.

  Compression is currently not negotiated and is turned off.

PARAMETERS
  config_ptr:   configuration to convert
  local_iid:    gateway IID
  remote_iid:   IID to be assigned to the peer

RETURN VALUE
  The passed information is used to setup the passed config_ptr.

DEPENDENCIES
  NOTE: The local_iid and remote_iid must be in network byte order.

SIDE EFFECTS
  None
===========================================================================*/
void ppp_set_v6_server_opts
(
  ppp_dev_opts_type *config_ptr,
  uint64            local_iid ,
  uint64            remote_iid 
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == config_ptr || 0 == local_iid || 0 == remote_iid )
  {
    /* ppp_set_v6_server_opts: Invalid config ptr  */
    DATA_PROTO_ERROR_LOG_3(PS_BAD_ARG_OR_NULL_PTR, config_ptr, config_ptr, 
                           remote_iid);
    return;
  }

  config_ptr->serves_addrs = TRUE;

  /*-------------------------------------------------------------------------
    Set the remote address,  and associated bits in the masks. Since we
    are acting as server we must have an address to assign.
  -------------------------------------------------------------------------*/
  config_ptr->ipv6cp_info.want_mask[FSM_REMOTE] |= IPV6CP_N_IFACE_ID;
  config_ptr->ipv6cp_info.will_mask[FSM_REMOTE] |= IPV6CP_N_IFACE_ID;

  config_ptr->ipv6cp_info.work_vals[FSM_REMOTE].iid = remote_iid;
  config_ptr->ipv6cp_info.want_vals[FSM_REMOTE].iid = remote_iid;

  /*-------------------------------------------------------------------------
    Set the local address (G/W address),and the associated bits in the masks. 
  -------------------------------------------------------------------------*/
  config_ptr->ipv6cp_info.want_mask[FSM_LOCAL] |= IPV6CP_N_IFACE_ID;
  config_ptr->ipv6cp_info.will_mask[FSM_LOCAL] |= IPV6CP_N_IFACE_ID;

  config_ptr->ipv6cp_info.work_vals[FSM_LOCAL].iid = local_iid;
  config_ptr->ipv6cp_info.want_vals[FSM_LOCAL].iid = local_iid;

#ifdef FEATURE_DATA_PS_PPP_ROHC
  config_ptr->ipv6cp_info.want_mask[FSM_REMOTE] &= ~IPV6CP_N_COMPRESS_ROHC;
  config_ptr->ipv6cp_info.will_mask[FSM_REMOTE] &= ~IPV6CP_N_COMPRESS_ROHC;
  config_ptr->ipv6cp_info.want_mask[FSM_LOCAL] &= ~IPV6CP_N_COMPRESS_ROHC;
  config_ptr->ipv6cp_info.will_mask[FSM_LOCAL] &= ~IPV6CP_N_COMPRESS_ROHC;
#endif /* FEATURE_DATA_PS_PPP_ROHC */

#ifdef FEATURE_DATA_PS_PPP_IPHC
  config_ptr->ipv6cp_info.want_mask[FSM_REMOTE] &= ~IPV6CP_N_COMPRESS_IPHC;
  config_ptr->ipv6cp_info.will_mask[FSM_REMOTE] &= ~IPV6CP_N_COMPRESS_IPHC;
  config_ptr->ipv6cp_info.want_mask[FSM_LOCAL] &= ~IPV6CP_N_COMPRESS_IPHC;
  config_ptr->ipv6cp_info.will_mask[FSM_LOCAL] &= ~IPV6CP_N_COMPRESS_IPHC;
#endif /* FEATURE_DATA_PS_PPP_IPHC */

} /* ppp_set_v6_server_opts() */
#endif /* FEATURE_DATA_PS_IPV6 */

/*===========================================================================
FUNCTION PPP_SET_AUTH_SERVER_OPTS()

DESCRIPTION
  This function will setup the configuration to enable the receiving PPP
  instance to authenticate the peer.

PARAMETERS
  config_ptr: pointer to the configuration being modified
  challenge name: string indicating the name that should be sent as part of
      the chap challenge.  This can be NULL or the empty string if no name is
      needed (PAP), or wants to be sent.

RETURN VALUE
  -1 on error
  the number of bytes of challenge_name copied
      NOTE: this may be less than strlen(challenge_name) if that is greater
            than PPP_CHAP_CHAL_NAME_LEN.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_set_auth_server_opts
(
  ppp_dev_opts_type *config_ptr,
  char              *challenge_name
)
{
  unsigned int copy_len = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(config_ptr == NULL)
  {
    PS_PPP_ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Set all of the authentication options up to do server style CHAP.
  -------------------------------------------------------------------------*/
  config_ptr->auths_peer = TRUE;
  config_ptr->lcp_info.work_vals[FSM_LOCAL].authentication =
    PPP_CHAP_PROTOCOL;
  config_ptr->lcp_info.work_vals[FSM_LOCAL].chap_digest = PPP_CHAP_MD5;
  config_ptr->lcp_info.work_vals[FSM_LOCAL].auth_proto_mask = 
    (ppp_auth_proto_e_type) ( (uint32)PPP_AUTH_PAP | (uint32)PPP_AUTH_CHAP );
  config_ptr->lcp_info.want_vals[FSM_LOCAL].authentication =
    PPP_CHAP_PROTOCOL;
  config_ptr->lcp_info.want_vals[FSM_LOCAL].chap_digest = PPP_CHAP_MD5;
  config_ptr->lcp_info.want_vals[FSM_LOCAL].auth_proto_mask = 
    (ppp_auth_proto_e_type) ( (uint32)PPP_AUTH_PAP | (uint32)PPP_AUTH_CHAP );
  config_ptr->lcp_info.want_mask[FSM_LOCAL] |= LCP_N_AP;
  config_ptr->lcp_info.will_mask[FSM_LOCAL] |= LCP_N_AP;

  /*-------------------------------------------------------------------------
    If the challenge name is non-NULL and the length is non-zero copy the
    challenge name.  Otherwise, set the challenge name length to 0.
  -------------------------------------------------------------------------*/
  if(challenge_name != NULL)
  {
    copy_len = strlen(challenge_name);
    copy_len = MIN(copy_len, PPP_CHAP_CHAL_NAME_LEN);
    config_ptr->auth_info.challenge_name_len = (uint8)copy_len;
    memscpy(config_ptr->auth_info.challenge_name, 
             PPP_CHAP_CHAL_NAME_LEN, 
             challenge_name, 
             copy_len);
  }
  else
  {
    config_ptr->auth_info.challenge_name_len = 0;
  }

  return (int)copy_len;
} /* ppp_set_auth_server_opts() */

/*===========================================================================
FUNCTION PPP_SET_CP_MODE()

DESCRIPTION
  This function is used to set the CP mode associated with a specific PPP 
  protocol and device.
 
DEPENDENCIES
  None

RETURN VALUE
  0 on success
 -1 on failure

SIDE EFFECTS
  Sets the CP mode in the FSM associated with a specific PPP protocol.
===========================================================================*/
int ppp_set_cp_mode
(
  ppp_dev_enum_type   device,
  ppp_protocol_e_type protocol,
  cp_mode_type        cp_mode
)
{
  ppp_type *ppp_cb_array = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( PPP_MAX_DEV <= device || 
      ( PPP_CP_MODE_INVALID > cp_mode  || PPP_CP_MODE_MAX <= cp_mode ) || 
      ( PPP_PROTOCOL_MIN > protocol || PPP_PROTOCOL_MAX <= protocol ) ) 
  {
    /*"Invalid args: device, cp_mode value protocol value*/
    PS_PPP_ASSERT(0);
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();

  LOG_MSG_INFO2("ppp_set_cp_mode: Device %d's FSM[%d] has old cp mode %d",
	   device, protocol, ppp_cb_array[device].fsm[protocol].mode);

  /*--------------------------------------------------------------------------
    Allow transition from DISCARD to PASSIVE for IPxCP if initialized
  --------------------------------------------------------------------------*/
  if( 0 != ppp_cb_array[device].session_handle &&
      PPP_PROTOCOL_IPV6CP == protocol &&
      PPP_CP_MODE_DISCARD == ppp_cb_array[device].fsm[protocol].mode &&
      fsmINITIAL == ppp_cb_array[device].fsm[protocol].state &&
      PPP_CP_MODE_PASSIVE == cp_mode )
  {
    DS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
    ppp_cb_array[device].fsm[protocol].mode = PPP_CP_MODE_PASSIVE;
    ppp_cb_array[device].ncp_mask |= PPP_PROTOCOL_IPV6CP_MASK;
    DS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
    fsm_open(&ppp_cb_array[device].fsm[protocol]);
  }
  else if( 0 != ppp_cb_array[device].session_handle &&
           PPP_PROTOCOL_IPCP == protocol &&
           PPP_CP_MODE_DISCARD == ppp_cb_array[device].fsm[protocol].mode &&
           fsmINITIAL == ppp_cb_array[device].fsm[protocol].state &&
           PPP_CP_MODE_PASSIVE == cp_mode )
  {
    DS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
    ppp_cb_array[device].fsm[protocol].mode = PPP_CP_MODE_PASSIVE;
    ppp_cb_array[device].ncp_mask |= PPP_PROTOCOL_IPCP_MASK;
    DS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
    fsm_open(&ppp_cb_array[device].fsm[protocol]);
  }
  else
  {
    DS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
    ppp_cb_array[device].fsm[protocol].mode         =
      pppi_ncp_mode_change( ppp_cb_array[device].fsm[protocol].mode,
                            cp_mode );
    DS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  }
  LOG_MSG_INFO2("ppp_set_cp_mode: Device %d's FSM[%d] has new cp mode %d",
	   device, protocol, ppp_cb_array[device].fsm[protocol].mode);

  return 0;

} /* ppp_set_cp_mode() */

/*===========================================================================
FUNCTION PPP_EVENT_ALLOC()

DESCRIPTION
  This function will allocate a PPP event information structure and
  register callback functions for supported PPP protocols.

PARAMETERS
  device:           PPP instance to register for
  lcp_cb_f:         LCP callback function to call
  lcp_user_data:    user data to be returned with the LCP callback function
  auth_cb_f:        auth callback function to call
  auth_user_data:   user data to be returned with the auth callback function
  ipcp_cb_f:        IPCP callback function to call
  ipcp_user_data:   user data to be returned with the IPCP callback
  ipv6cp_cb_f:      IPV6CP callback function to call
  ipv6cp_user_data: user data to be returned with the IPV6CP callback

RETURN VALUE
  NULL on error
  handle to the PPP event information structure on success

DEPENDENCIES
  None

SIDE EFFECTS
  Allocates a PPP event information structure and places it on the event
  queue.  Initializes event masks for all protocols to 0 and registers
  callbacks for the PPP protocols
===========================================================================*/
void *ppp_event_alloc
(
  ppp_dev_enum_type           device,
  ppp_event_cback_f_ptr_type  lcp_cb_f,
  void                       *lcp_user_data,
  ppp_event_cback_f_ptr_type  auth_cb_f,
  void                       *auth_user_data,
  ppp_event_cback_f_ptr_type  ipcp_cb_f,
  void                       *ipcp_user_data,
  ppp_event_cback_f_ptr_type  ipv6cp_cb_f,
  void                       *ipv6cp_user_data
)
{
  ppp_event_info_s_type *event_info_ptr = NULL;
  ppp_type              *ppp_cb_ptr;
  ppp_type              *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check if a valid PPP device passed
  -------------------------------------------------------------------------*/
  if( PPP_MAX_DEV <= device )
  {
    /*"Invalid device passed"*/
    PS_PPP_ASSERT(0);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Check that at least one callback is being registered
  -------------------------------------------------------------------------*/
  if( NULL == lcp_cb_f && NULL == auth_cb_f && NULL == ipcp_cb_f
#ifdef FEATURE_DATA_PS_IPV6
                       &&
      NULL == ipv6cp_cb_f
#endif /* FEATURE_DATA_PS_IPV6 */
    )
  {
    /*"NULL functions passed"*/
    PS_PPP_ASSERT(0);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Get the PPP control block for this device
  -------------------------------------------------------------------------*/
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &ppp_cb_array[device];

  /*-------------------------------------------------------------------------
    Get a new event information structure
  -------------------------------------------------------------------------*/
  event_info_ptr = ps_mem_get_buf( PS_MEM_PPP_EVENT_INFO_ITEM_TYPE );

  if( NULL == event_info_ptr )
  {
    /* Couldn't allocate event info structure */
    DATA_PROTO_ERROR_LOG_0(PS_MEM_ALLOC_ERR);
  }
  else
  {
    /*-----------------------------------------------------------------------
      Initialize the event information structure
    -----------------------------------------------------------------------*/
    memset( event_info_ptr, 0, sizeof(ppp_event_info_s_type) );
    DS_ENTER_CRIT_SECTION_NO_TASKLOCK( pppi_get_crit_sect_lock(device) );
    (void)q_link( &ppp_cb_ptr->event_queue, &event_info_ptr->link );

    event_info_ptr->device            = device;

    /*-----------------------------------------------------------------------
      Fill the structure with the passed callbacks and user_data cookies
    -----------------------------------------------------------------------*/
    event_info_ptr->cback[LCP]        = lcp_cb_f;
    event_info_ptr->user_data[LCP]    = lcp_user_data;
    event_info_ptr->cback[AUTH]       = auth_cb_f;
    event_info_ptr->user_data[AUTH]   = auth_user_data;
    event_info_ptr->cback[IPCP]       = ipcp_cb_f;
    event_info_ptr->user_data[IPCP]   = ipcp_user_data;
#ifdef FEATURE_DATA_PS_IPV6
    event_info_ptr->cback[IPV6CP]     = ipv6cp_cb_f;
    event_info_ptr->user_data[IPV6CP] = ipv6cp_user_data;
#endif /* FEATURE_DATA_PS_IPV6 */

    /*-----------------------------------------------------------------------
      Place the event information structure on the event queue for device
    -----------------------------------------------------------------------*/
    q_put( &ppp_cb_ptr->event_queue, &event_info_ptr->link );
    DS_LEAVE_CRIT_SECTION_NO_TASKLOCK( pppi_get_crit_sect_lock(device) );
  }

  return event_info_ptr;
} /* ppp_event_alloc() */



/*===========================================================================
FUNCTION PPP_EVENT_REGISTER()

DESCRIPTION
  This function will add to an event mask for a PPP protocol.  It will
  generate an UP event if the event mask is registering for an UP event
  and the UP event was previously not registerd for this protocol and the
  associated protocol is already up.

PARAMETERS
  handle      PPP event structure to add this event mask to
  protocol    Protocol to add the event mask to
  event_mask  event mask to add

RETURN VALUE
  -1 on error
   0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  Adds the passed event mask to the already registered event mask
===========================================================================*/
int32 ppp_event_register
(
  void                  *handle,
  ppp_protocol_e_type    protocol,
  ppp_event_mask_e_type  event_mask
)
{
  ppp_event_info_s_type *event_info_ptr = handle;
  uint16 supported_events = ( (uint16)PPP_DOWN_EV_MASK     |
                              (uint16)PPP_UP_EV_MASK       |
                              (uint16)PPP_RESYNC_EV_MASK   |
                              (uint16)PPP_STARTING_EV_MASK |
#ifdef FEATURE_DATA_EHRPD_DUAL_IP 
                              (uint16)PPP_SUBS_LIMITED_TO_V4_EV_MASK |
                              (uint16)PPP_SUBS_LIMITED_TO_V6_EV_MASK |
#endif
                              (uint16)PPP_SUCCESS_EV_MASK  |
                              (uint16)PPP_FAILURE_EV_MASK );

  ppp_type              *ppp_cb_ptr;
  ppp_type              *ppp_cb_array = NULL;
  ppp_fail_reason_e_type fail_reason = PPP_FAIL_REASON_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify that the passed arguments are valid
  -------------------------------------------------------------------------*/
  if( NULL == handle ||
      FALSE == ps_mem_is_valid(handle, PS_MEM_PPP_EVENT_INFO_ITEM_TYPE) ||
      PPP_PROTOCOL_MAX <= protocol ||
      0 == (supported_events & (uint16)event_mask) )
  {
    /*"invalid args: handle, protocol, event_mask"*/
    PS_PPP_ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    If registering a mask for a protocol for which no callback is registerd
    return error.
  -------------------------------------------------------------------------*/
  if( NULL == event_info_ptr->cback[protocol] )
  {
    /*"Registering event for a protocol with NULL callback"*/
    PS_PPP_ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    If protocol is AUTH then we support only PPP_STARTING_EV,
    PPP_FAILURE_EV and PPP_SUCCESS_EV
  -------------------------------------------------------------------------*/
  if( PPP_PROTOCOL_AUTH == protocol &&
      0 == ( (uint16)PPP_STARTING_EV_MASK & (uint16)event_mask ) &&
      0 == ( (uint16)PPP_FAILURE_EV_MASK & (uint16)event_mask) &&
      0 == ( (uint16)PPP_SUCCESS_EV_MASK & (uint16)event_mask ) )
  {
    /* "invalid mask for AUTH" */
    PS_PPP_ASSERT(0);
    return -1;
  }

  if(event_info_ptr->device >= PPP_MAX_DEV )
  {
    /* Invalid dev <event_info_ptr->device> in event reg   */
    DATA_PROTO_ERROR_LOG_1(PS_BAD_ARG_OR_NULL_PTR,event_info_ptr->device);
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &ppp_cb_array[event_info_ptr->device];

  DS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(event_info_ptr->device) );
  /*-------------------------------------------------------------------------
    If the UP event was not previously registered and we are registering it
    now then generate the UP callback if the protocol is UP.
    TODO: For Up event call callback for active NCPs(VSNCPs).
  -------------------------------------------------------------------------*/
  if( 
     !( (uint16)PPP_UP_EV_MASK & (uint16)event_info_ptr->event_mask[protocol] )
     && ( (uint16)PPP_UP_EV_MASK & (uint16)event_mask )  
     && ( ( ( protocol < (ppp_protocol_e_type)fsmi_Size )&& 
            ( fsmOPENED == ppp_cb_ptr->fsm[protocol].state ) ) 
          || ( PPP_PROTOCOL_AUTH == protocol && 
               pppNETWORK == ppp_cb_ptr->phase ) ) )
  {
    event_info_ptr->cback[protocol]( event_info_ptr->device,
                                     protocol,
                                     PPP_UP_EV,
                                     event_info_ptr->user_data[protocol],
                                     ppp_cb_ptr->session_handle,
                                     fail_reason
                                   );
  }

  /*-------------------------------------------------------------------------
    Set the event_mask for the event information structure
  -------------------------------------------------------------------------*/
  event_info_ptr->event_mask[protocol]  =
    (ppp_event_mask_e_type)( (uint16)event_info_ptr->event_mask[protocol] 
                             | (uint16) event_mask ) ;
  DS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(event_info_ptr->device) );

  return 0;
} /* ppp_event_register() */



/*===========================================================================
FUNCTION PPP_EVENT_DEREGISTER()

DESCRIPTION
  This function will remove an event mask for a PPP protocol.

PARAMETERS
  handle      PPP event structure to add this event mask to
  protocol    Protocol to add the event mask to
  event_mask  event mask to add

RETURN VALUE
  -1 on error
   0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  Removes the passed event mask from the already registered event mask
===========================================================================*/
int32 ppp_event_deregister
(
  void                  *handle,
  ppp_protocol_e_type    protocol,
  ppp_event_mask_e_type  event_mask
)
{
  ppp_event_info_s_type *event_info_ptr = handle;
  uint16 supported_events = ( (uint16)PPP_DOWN_EV_MASK     |
                              (uint16)PPP_UP_EV_MASK       |
                              (uint16)PPP_RESYNC_EV_MASK   |
                              (uint16)PPP_STARTING_EV_MASK |
#ifdef FEATURE_DATA_EHRPD_DUAL_IP 
                              (uint16)PPP_SUBS_LIMITED_TO_V4_EV_MASK |
                              (uint16)PPP_SUBS_LIMITED_TO_V6_EV_MASK |
#endif
                              (uint16)PPP_SUCCESS_EV_MASK  |
                              (uint16)PPP_FAILURE_EV_MASK );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify that the passed arguments are valid
  -------------------------------------------------------------------------*/
  if( NULL == handle ||
      FALSE == ps_mem_is_valid(handle, PS_MEM_PPP_EVENT_INFO_ITEM_TYPE) ||
      PPP_PROTOCOL_MAX <= protocol ||
      0 == ( supported_events & (uint16)event_mask ) )
  {
    /*"invalid args: handle, protocol, event_mask"*/
    PS_PPP_ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    If protocol is AUTH then we support only PPP_STARTING_EV,
    PPP_FAILURE_EV and PPP_SUCCESS_EV
  -------------------------------------------------------------------------*/
  if( PPP_PROTOCOL_AUTH == protocol &&
      0 == ( (uint16)PPP_STARTING_EV_MASK & (uint16)event_mask ) &&
      0 == ( (uint16)PPP_FAILURE_EV_MASK & (uint16)event_mask ) &&
      0 == ( (uint16)PPP_SUCCESS_EV_MASK & (uint16)event_mask ) )
  {
    /*( "invalid mask for AUTH:")*/
    PS_PPP_ASSERT(0);
    return -1;
  }

  if(event_info_ptr->device >= PPP_MAX_DEV )
  {
    /* Invalid dev <event_info_ptr->device> in event dereg */
    DATA_PROTO_ERROR_LOG_1(PS_BAD_ARG_OR_NULL_PTR,event_info_ptr->device);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Remove the event_mask for the event information structure
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(event_info_ptr->device) );
  event_info_ptr->event_mask[protocol] =
    (ppp_event_mask_e_type)( (uint16)event_info_ptr->event_mask[protocol] 
                             & ~(uint16)event_mask );
  DS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(event_info_ptr->device) );

  return 0;
} /* ppp_event_deregister() */

/*===========================================================================
FUNCTION PPP_EVENT_DEALLOC()

DESCRIPTION
  This function will deallocate a PPP event information structure.

NOTE: This function should not be called in the callback function registered
      for PPP events.

PARAMETERS
  device      PPP device to which this handle belongs
  handle      PPP event structure to deallocate

RETURN VALUE
  -1 on error
   0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  Deallocates this PPP event structure.  Sets handle to NULL
===========================================================================*/
int32 ppp_event_dealloc
(
  ppp_dev_enum_type   device,
  void              **handle
)
{
  ppp_event_info_s_type *event_info_ptr;
  ppp_type              *ppp_cb_ptr;
  ppp_type              *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Verify that the passed arguments are valid
  -------------------------------------------------------------------------*/
  if( NULL == handle ||
      FALSE == ps_mem_is_valid(*handle, PS_MEM_PPP_EVENT_INFO_ITEM_TYPE) ||
      PPP_MAX_DEV <= device )
  {
    /*( "invalid args: handle, device")*/
    PS_PPP_ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Get the PPP control block for this device
  -------------------------------------------------------------------------*/
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &ppp_cb_array[device];

  /*-------------------------------------------------------------------------
    Delete the event info structure from the PPP device's event queue
  -------------------------------------------------------------------------*/
  event_info_ptr = *(ppp_event_info_s_type**)handle;
  DS_ENTER_CRIT_SECTION_NO_TASKLOCK( pppi_get_crit_sect_lock(device)  );
#ifdef FEATURE_Q_NO_SELF_QPTR
  q_delete( &ppp_cb_ptr->event_queue, &event_info_ptr->link );
#else
  q_delete( &event_info_ptr->link );
#endif
  DS_LEAVE_CRIT_SECTION_NO_TASKLOCK( pppi_get_crit_sect_lock(device) );

  /*-------------------------------------------------------------------------
    Free the event information structure's memory
  -------------------------------------------------------------------------*/
  PS_MEM_FREE( *handle );

  return 0;
} /* ppp_event_dealloc() */

/*===========================================================================
FUNCTION PPP_START()

DESCRIPTION
  This function is used by PPP clients to start a particular PPP client with
  a specific configuration.  The caller is ALWAYS required to pass in a
  configuration which should be initialized to some set of values - the
  ppp_get*_opts() function can be used to configure the configuration
  structure.

  Fields that MUST be set in the config structure:
    ipcp_info.rx_iface_ptr || ipv6cp_info.rx_iface_ptr

PARAMETERS
  device:     the PPP device to be started
  config_ptr: pointer to the configuration with which to start PPP device
              'device'

RETURN VALUE
 -1: if the command fails for any reason: such as the inability to get a
     command buffer, or some configuration is bad.
  session handle greater then 0 on success.  This can be used to associate
  PPP events with this instance of PPP.

DEPENDENCIES
  None

SIDE EFFECTS
  The configuration that is passed in will be copied to the PPP device
  IMMEDIATELY (if the device is free).  This means that the PPP context is
  modified in the callers task context - it also means that the memory for
  configuration passed in is free as soon as this function returns.
===========================================================================*/
int ppp_start
(
  ppp_dev_enum_type  device,
  ppp_dev_opts_type *config_ptr
)
{
  static uint16             session_handle = 1;
  ppp_type                 *ppp_cb_ptr;
  ppp_type                 *ppp_cb_array = NULL;
  ppp_config_buffer_s_type *configbuf_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("ppp_start(dev %d)", device);
  if(device >= PPP_MAX_DEV || NULL == config_ptr)
  {
    PS_PPP_ASSERT(0);
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &ppp_cb_array[device];

  /*-------------------------------------------------------------------------
    If a valid configuration is passed for IPCP or IPV6CP then either the PS
    iface pointer currently registered for that NCP should be NULL or the
    same as the passed PS iface pointer
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
#ifdef FEATURE_DATA_PS_IPV6
  if((PPP_CP_MODE_INVALID != config_ptr->ipcp_info.mode &&
      NULL != ppp_cb_ptr->fsm[IPCP].fwk_inst_ptr &&
      ppp_cb_ptr->fsm[IPCP].fwk_inst_ptr!=
        config_ptr->ipcp_info.fwk_inst_ptr) ||
     (PPP_CP_MODE_INVALID != config_ptr->ipv6cp_info.mode &&
      NULL != ppp_cb_ptr->fsm[IPV6CP].fwk_inst_ptr&&
      ppp_cb_ptr->fsm[IPV6CP].fwk_inst_ptr !=
        config_ptr->ipv6cp_info.fwk_inst_ptr))
#else
  if( (PPP_CP_MODE_INVALID != config_ptr->ipcp_info.mode &&
       NULL != ppp_cb_ptr->fsm[IPCP].fwk_inst_ptr &&
       ppp_cb_ptr->fsm[IPCP].fwk_inst_ptr !=
         config_ptr->ipcp_info.fwk_inst_ptr) )
#endif /* FEATURE_DATA_PS_IPV6 */
  {
    LOG_MSG_INFO1_0( "ppp_start() called on protocol already started" );
    DS_BRANCH_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
    return -1;
  }
  DS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device)  );

  /*-------------------------------------------------------------------------
    Sanity check the passed configuration
  -------------------------------------------------------------------------*/
  if( pppi_config_check(config_ptr) < 0 )
  {
    /*TODO: Add config check for PP as well*/
    LOG_MSG_INFO1_0( "Invalid configuration passed" );
    return -1;
  }

  /*-------------------------------------------------------------------------
    Get a configuration buffer and send a PPP_START_CMD
  -------------------------------------------------------------------------*/
  configbuf_ptr = (ppp_config_buffer_s_type *)
                    ps_mem_get_buf(PS_MEM_PPP_CMD_BUF_TYPE);

  if( NULL == configbuf_ptr )
  {
    /*("Memory allocation for PPP config buffer failed")*/
    PS_PPP_ASSERT(0);
    return -1;
  }
  else
  {
    ULOG_RT_PRINTF_1(ds_fwk_get_ulog_handle(),"ppp_start(): "
                   "Configbuf_ptr 0x%x allocated from PS MEM POOL", 
                   configbuf_ptr);
    /*-----------------------------------------------------------------------
      Is session_handle about to wrap-around ?
    -----------------------------------------------------------------------*/
    if( 0xFFFF == session_handle )
    {
      LOG_MSG_INFO1_0("PPP Session handle wrapped around");
      session_handle = 1;
    }

    /*-----------------------------------------------------------------------
      Set the session_handle field.
    -----------------------------------------------------------------------*/
    DS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device)  );
    if( 0 == ppp_cb_ptr->session_handle )
    {
      ppp_cb_ptr->session_handle    = session_handle++;
    }
    configbuf_ptr->session_handle = ppp_cb_ptr->session_handle;

    /*-----------------------------------------------------------------------
      Set the relevant flags in NCP mask
    -----------------------------------------------------------------------*/
    if( ( PPP_CP_ACTIVE  == config_ptr->ipcp_info.mode ) ||
        ( PPP_CP_PASSIVE == config_ptr->ipcp_info.mode ) )
    {
      ppp_cb_ptr->ncp_mask =
        ( ppp_cb_ptr->ncp_mask | ( (uint16)PPP_PROTOCOL_IPCP_MASK ) );
    }
#ifdef FEATURE_DATA_PS_IPV6
    if( PPP_CP_ACTIVE  == config_ptr->ipv6cp_info.mode ||
        PPP_CP_PASSIVE == config_ptr->ipv6cp_info.mode )
    {
      ppp_cb_ptr->ncp_mask =
        ( ppp_cb_ptr->ncp_mask | (uint16)PPP_PROTOCOL_IPV6CP_MASK );
    }
#endif /* FEATURE_DATA_PS_IPV6 */

    ppp_cb_ptr->bridge_dev = config_ptr->bridge_dev;

    DS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

    configbuf_ptr->device        = device;
    configbuf_ptr->dev_opts_null = FALSE;
    memscpy( &configbuf_ptr->dev_opts, 
             sizeof(ppp_dev_opts_type),
             config_ptr, 
             sizeof(ppp_dev_opts_type) );
    PPP_SEND_CMD(PPP_START_CMD, (void*)configbuf_ptr);
  }

  return ppp_cb_ptr->session_handle;
} /* ppp_start() */

/*===========================================================================
FUNCTION PPP_STOP()

DESCRIPTION
  This function is used by PPP clients to stop a particular PPP device.
  NOTE: if the graceful_stop flag for this PPP device is false, this will
  cause an abort() rather than doing stop() processing.

PARAMETERS
  device: the PPP device to be closeed

RETURN VALUE
  0: on success
 -1: if the command fails for any reason: such as the inability to get a
     command buffer, or a bqad device was passed in.

DEPENDENCIES
  None

SIDE EFFECTS
  the 'device' field in the configuration structure will be set to the value
  of the 'device' parameter passed in.
===========================================================================*/
int ppp_stop
(
  ppp_dev_enum_type   device,
  ppp_protocol_e_type protocol
)
{
  ppp_type            *ppp_cb_ptr;
  ppp_type            *ppp_cb_array = NULL;
  uint32               stop_info = 0x0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DATA_PS_IPV6
  if( ( device >= PPP_MAX_DEV ) ||
      ( ( PPP_PROTOCOL_LCP != protocol    ) &&
        ( PPP_PROTOCOL_IPCP != protocol   ) &&
        ( PPP_PROTOCOL_IPV6CP != protocol ) ) )
#else
  if( ( device >= PPP_MAX_DEV ) ||
      ( ( PPP_PROTOCOL_LCP != protocol  ) &&
        ( PPP_PROTOCOL_IPCP != protocol ) ) )
#endif /* FEATURE_DATA_PS_IPV6 */
  {
    PS_PPP_ASSERT(0);
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)device;

  /*-------------------------------------------------------------------------
    Determine if LCP needs to be brought down.
    The AND operation on the ncp_mask checks if the ncp_mask is reduced to
    zero once the protocol being stopped here is removed from it.  If the
    ncp_mask goes to zero then it means that this is the only NCP protocol
    being run.
  -------------------------------------------------------------------------*/
  if( PPP_PROTOCOL_LCP != protocol )
  {
    DS_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
    ppp_cb_ptr->ncp_mask &= ~(uint16)( 1 << (uint16)protocol );
    DS_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

    if( 0 == ppp_cb_ptr->ncp_mask )
    {
      /*----------------------------------------------------------------------- 
        Propagate the fail reason to the LCP FSM since this is the last NCP.
      -----------------------------------------------------------------------*/
      ppp_cb_ptr->fsm[LCP].fail_reason = ppp_cb_ptr->fsm[protocol].fail_reason;
      protocol = PPP_PROTOCOL_LCP;
    }
  }

 /*---------------------------------------------------------------------
    Fill the information to be passed to pppi_stop()
    stop_info:
    fill high 16-bits with device      
    fill low 16-bits with protocol
   ---------------------------------------------------------------------*/
   stop_info |= (((uint16)device) << 16);
   stop_info |= ((uint16)protocol); 
   LOG_MSG_INFO2_2("ppp_stop(): protocol=0x%x, device=%d", protocol, device);
   PPP_SEND_CMD(PPP_STOP_CMD, (void*)stop_info);

  return 0;
} /* ppp_stop() */



/*===========================================================================
FUNCTION PPP_ABORT()

DESCRIPTION
  This function is used by PPP clients to abort a particular PPP device.

PARAMETERS
  device: the PPP device to be aborted

RETURN VALUE
  0: on success
 -1: if the command fails for any reason: such as the inability to get a
     command buffer, or a bqad device was passed in.

DEPENDENCIES
  None

SIDE EFFECTS
  the 'device' field in the configuration structure will be set to the value
  of the 'device' parameter passed in.
===========================================================================*/
int ppp_abort
(
  ppp_dev_enum_type  device
)
{
  ppp_type             *ppp_cb_ptr;  
  ppp_type             *ppp_cb_array = NULL;
  uint32 abort_info = 0x0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("ppp_abort(dev %d)", device);
  if(device >= PPP_MAX_DEV)
  {
    PS_PPP_ASSERT(0);
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)device;

  DS_BRANCH_ENTER_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  if(ppp_cb_ptr->session_handle == 0)
  {
    LOG_MSG_INFO2_1("ppp_abort(%d) on free device!", device);
    DS_BRANCH_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );
  }
  else
  {
    PPP_DISABLE_SIG(ppp_cb_ptr->rx_signal);
    /*-----------------------------------------------------------------------
      As the device is being released, reset the session handle to indicate
      this, and set the rx and tx function pointers to the default values.
    -----------------------------------------------------------------------*/
    ppp_cb_ptr->rx_f_ptr           = pppi_default_rx;
    ppp_cb_ptr->tx_f_ptr           = pppi_default_tx;
    ppp_cb_ptr->rx_user_data       = NULL;
    ppp_cb_ptr->tx_user_data       = NULL;
    /*-----------------------------------------------------------------------
      abort_info:
        fill high 16-bits with device      
        fill low 16-bits with session_handle    
    -----------------------------------------------------------------------*/
    abort_info |= (((uint16)device) << 16);
    abort_info |= ((uint16)(ppp_cb_ptr->session_handle));
    ppp_cb_ptr->session_handle     = 0;

    DS_BRANCH_LEAVE_CRIT_SECTION( pppi_get_crit_sect_lock(device) );

    PPP_SEND_CMD(PPP_ABORT_CMD, (void*)abort_info);
  }

  return 0;
} /* ppp_abort() */



/*===========================================================================
FUNCTION PPP_SET_MODE()

DESCRIPTION
  This function takes a new mode and interface as arguments and changes the
  mode of the interface appropriately.
  Will throw an error if:
  (1) Mode switch to PPP_PSEUDO_NET_MODE or PPP_FULL_NET_MODE is requested
      without a bridge device set.
  (2) Mode switch to PPP_RELAY_MODE is requested from a mode other than
      itself or PPP_NONE_MODE.
  (3) Mode switch to PPP_PSEUDO_NET_MODE is requested while running in HDLC
      hardware mode.
  (4) An invalid PPP mode switch is requested.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_set_mode
(
  ppp_dev_enum_type  device,             /* device to change              */
  ppp_mode_enum_type new_mode            /* new mode for that device      */
)
{
  ppp_type            *ppp_cb_ptr;
  ppp_type            *ppp_cb_array = NULL;
  uint32 mode_info = 0x0;  
  pppi_mode_info_type  ppp_mode_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(device >= PPP_MAX_DEV)
  {
    PS_PPP_ASSERT(0);
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = ppp_cb_array + (uint8)device;

  if(ppp_cb_ptr->session_handle == 0)
  {
    LOG_MSG_INFO2_2("ppp_set_mode(%d) on free device(%d)!", new_mode, device);
  }
  else
  {
    LOG_MSG_INFO2_3("dev %d mode from %d to %d",
                    device, ppp_cb_ptr->mode, new_mode);

    switch(new_mode)
    {
      case PPP_PSEUDO_NET_MODE:
      case PPP_FULL_NET_MODE:
      case PPP_RELAY_MODE:
      case PPP_INTERNAL_MODE:
      case PPP_INTERNAL_SNOOP_MODE:
      case PPP_NONE_MODE:
        /*-------------------------------------------------------------------
          Since changing a mode can impact how PPP processes data, it should
          only be changed in the same context in which PPP is running to
          avoid any race conditions. If the caller is calling from the same
          context, set the new mode here else send a cmd to the PPP task.
        -------------------------------------------------------------------*/
        if(PPP_IS_CURR_TASK_OWNER())
        {
          ppp_mode_info.device = (uint16)device;
          ppp_mode_info.mode   = (uint16)new_mode;
          pppi_set_mode(ppp_mode_info);
        }
        else
        {
          /*------------------------------------------------------------------
            mode_info:
              fill high 16-bits with device      
              fill low 16-bits with new_mode    
          ------------------------------------------------------------------*/
          mode_info |= (((uint16)device) << 16);
          mode_info |= ((uint16)new_mode);
          PPP_SEND_CMD(PPP_SET_MODE_CMD, (void*)mode_info);
        }

        break;

      default:
        /*-------------------------------------------------------------------
          Invalid mode!
        -------------------------------------------------------------------*/
        /*("PPP, invalid mode")*/
        PS_PPP_ASSERT(0);
    break;
    } /* switch(device mode) */
  }
} /* ppp_set_mode() */

/*===========================================================================
FUNCTION PPP_GET_MODE()

DESCRIPTION
  This function returns the current mode of the interface in question.

DEPENDENCIES
  None

RETURN VALUE
  The mode of the requested interface.

SIDE EFFECTS
  None
===========================================================================*/
ppp_mode_enum_type ppp_get_mode
(
  ppp_dev_enum_type device              /* device to check              */
)
{
  ppp_type      *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(device >= PPP_MAX_DEV )
  {
    /*("Invalid dev in get mode")*/
    PS_PPP_ASSERT(0);
    return PPP_INVALID_MODE;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  return ppp_cb_array[device].mode;

} /* ppp_get_mode() */



/*===========================================================================
FUNCTION PPP_INJECT_PACKET()

DESCRIPTION
  This function processes a PPP packet as if the packet has been
  received from the underlying device. The processing is performed as
  soon as this function is called before other packets that are already
  queued in the rx queue of the device. Only exception is the packet that is
  currrently being processed in PS context. This function posts a cmd to PS
  so that packet processing always takes place in PS. This function allows
  replaying previously stored PPP packets.

PARAMETERS
  device:        PPP device to configure
  ppp_pkt_ptr:   Pointer to PPP packet to be injected

RETURN VALUE
  0: on success
 -1: if an invalid or unconfigured PPP device specified

DEPENDENCIES
  This function should not be called while already processing a PPP
  packet. That means this function should not be called from pppi_incoming(),
  pppi_receive(), pppi_hdlc_sw/hw_frame/unframe() functions.
  It expects an unframed PPP packet, without FCS and ASYNC FLAG (7E)

SIDE EFFECTS
  Processes the specified PPP packet out of order
===========================================================================*/
int ppp_inject_packet
(
  ppp_dev_enum_type device,               /* PPP device to pass pkt to     */
  uint16            protocol,             /* Protocol of packet            */
  dsm_item_type     *ppp_pkt_ptr          /* Pointer to PPP pkt            */
)
{
  ppp_type *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((device >= PPP_MAX_DEV) || (ppp_pkt_ptr == NULL))
  {
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  if(ppp_cb_array[device].session_handle == 0)
  {
    LOG_MSG_INFO2_1("ppp_inject_packet(%d) on free device!", device);
    return -1;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Put device in lower 16 bit and protocol in upper 16 bit of the
      app_field of the pkt ptr. Post cmd to PS to start processing the
      specified pkt and pass the pkt ptr as the cmd param.
    -----------------------------------------------------------------------*/
    ppp_pkt_ptr->app_field = (protocol << 16) | (uint8)device;
    PPP_SEND_CMD(PPP_INJECT_PKT_CMD, (void*)ppp_pkt_ptr);
  }

  return 0;
} /* ppp_inject_packet() */

/*===========================================================================
FUNCTION PPPI_CONFIG_CHECK()

DESCRIPTION
  This function is used by PPP clients to check configuration for a
  particular PPP device.

PARAMETERS
  config_ptr: pointer to the configuration options to check

RETURN VALUE
  0 on success
 -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static int pppi_config_check
(
  ppp_dev_opts_type *config_ptr
)
{
  negotiate_t lcp_accept;
  negotiate_t ipcp_accept;
  negotiate_t ipv6cp_accept;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check TX function pointer
  -------------------------------------------------------------------------*/
  if( config_ptr->tx_f_ptr == NULL )
  {
    LOG_MSG_ERROR_0("NULL TX function passed");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Check protocol modes and PS iface pointers.  It is an error if:
      *  LCP mode is DISABLED, DISCARD or INVALID
      *  HDLC is in HDLC_INVALID_MODE
      *  No PS iface is being registered
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_IPV6
  if( PPP_CP_MODE_DISABLED   == config_ptr->lcp_info.mode          ||
      PPP_CP_MODE_DISCARD    == config_ptr->lcp_info.mode          ||
      PPP_CP_MODE_INVALID    == config_ptr->lcp_info.mode          ||
      PPP_CP_MODE_MAX        <= config_ptr->lcp_info.mode          ||
      HDLC_INVALID_MODE      <= config_ptr->hdlc_mode   )  
#else
  if( PPP_CP_MODE_DISABLED == config_ptr->lcp_info.mode    ||
      PPP_CP_MODE_DISCARD  == config_ptr->lcp_info.mode    ||
      PPP_CP_MODE_INVALID  == config_ptr->lcp_info.mode    ||
      PPP_CP_MODE_MAX      <= config_ptr->lcp_info.mode    ||
      HDLC_INVALID_MODE    <= config_ptr->hdlc_mode        ||
      NULL                 == config_ptr->ipcp_info.fwk_inst_ptr )
#endif /* FEATURE_DATA_PS_IPV6 */
  {
    LOG_MSG_ERROR_0("Invalid protocol mode or PS iface passed");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Check NCP modes.  Print a message if:
      *  BOTH NCPs are in a combination of INVALID, DISCARD or DISABLED i.e.
         neither NCP is being configured
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_IPV6
  if( (PPP_CP_MODE_DISCARD  == config_ptr->ipcp_info.mode ||
       PPP_CP_MODE_DISABLED == config_ptr->ipcp_info.mode ||
       PPP_CP_MODE_INVALID  == config_ptr->ipcp_info.mode)     &&
      (PPP_CP_MODE_DISCARD  == config_ptr->ipv6cp_info.mode ||
       PPP_CP_MODE_DISABLED == config_ptr->ipv6cp_info.mode ||
       PPP_CP_MODE_INVALID  == config_ptr->ipv6cp_info.mode) )
#else
  if( PPP_CP_MODE_DISCARD  == config_ptr->ipcp_info.mode   ||
      PPP_CP_MODE_DISABLED == config_ptr->ipcp_info.mode   ||
      PPP_CP_MODE_INVALID  == config_ptr->ipcp_info.mode )
#endif /* FEATURE_DATA_PS_IPV6 */
  {
    LOG_MSG_INFO2_0("No NCP is in Active or Passive mode");
  }

  /*------------------------------------------------------------------------- 
    Get LCP accept bitmask.
  -------------------------------------------------------------------------*/
  lcp_accept  = lcp_get_accept_mask();

  /*-------------------------------------------------------------------------
    Check LCP parameters
  -------------------------------------------------------------------------*/
  if( (config_ptr->lcp_info.want_mask[FSM_LOCAL]  & ~lcp_accept)  != 0 ||
      (config_ptr->lcp_info.will_mask[FSM_LOCAL]  & ~lcp_accept)  != 0 ||
      (config_ptr->lcp_info.want_mask[FSM_REMOTE] & ~lcp_accept)  != 0 ||
      (config_ptr->lcp_info.will_mask[FSM_REMOTE] & ~lcp_accept)  != 0 )
  {
    LOG_MSG_ERROR_0("Invalid LCP configuration param(s)!");
    return -1;
  }

  if( (config_ptr->lcp_info.want_mask[FSM_LOCAL] & LCP_N_MAGIC) != 0  &&
      (config_ptr->lcp_info.will_mask[FSM_REMOTE] & LCP_N_MAGIC)  == 0 )
  {
    LOG_MSG_ERROR_0("Invalid LCP configuration param(s)!");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Check AUTH parameters
  -------------------------------------------------------------------------*/
  if( config_ptr->auth_info.user_id_len        > PPP_MAX_USER_ID_LEN ||
      config_ptr->auth_info.passwd_len         > PPP_MAX_PASSWD_LEN  ||
      config_ptr->auth_info.challenge_len      > PPP_CHAP_CHAL_LEN   ||
      config_ptr->auth_info.challenge_name_len > PPP_CHAP_CHAL_NAME_LEN )
  {
    LOG_MSG_ERROR_0("Invalid Auth configuration param(s)!");
    return -1;
  }

  /*------------------------------------------------------------------------- 
    Get IPCP accept bitmask.
  -------------------------------------------------------------------------*/
  ipcp_accept  = ipcp_get_accept_mask();

  /*-------------------------------------------------------------------------
    Check IPCP parameters
  -------------------------------------------------------------------------*/
  if( config_ptr->ipcp_info.mode != PPP_CP_MODE_INVALID &&
       ((config_ptr->ipcp_info.want_mask[FSM_LOCAL]  & ~ipcp_accept) != 0 ||
       (config_ptr->ipcp_info.will_mask[FSM_LOCAL]  & ~ipcp_accept) != 0 ||
       (config_ptr->ipcp_info.want_mask[FSM_REMOTE] & ~ipcp_accept) != 0 ||
       (config_ptr->ipcp_info.will_mask[FSM_REMOTE] & ~ipcp_accept) != 0) )
  {
    LOG_MSG_ERROR_0("Invalid IPCP configuration param(s)!");
    return -1;
  }

#ifdef FEATURE_DATA_PS_IPV6
  /*------------------------------------------------------------------------- 
    Get IPv6CP accept bitmask.
  -------------------------------------------------------------------------*/
  ipv6cp_accept  = ipv6cp_get_accept_mask();
  /*-------------------------------------------------------------------------
    Check IPv6CP parameters
  -------------------------------------------------------------------------*/
  if( config_ptr->ipv6cp_info.mode != PPP_CP_MODE_INVALID &&
       ((config_ptr->ipv6cp_info.want_mask[FSM_LOCAL]  & ~ipv6cp_accept)
                                                                    != 0 ||
       (config_ptr->ipv6cp_info.will_mask[FSM_LOCAL]  & ~ipv6cp_accept)
                                                                    != 0 ||
       (config_ptr->ipv6cp_info.want_mask[FSM_REMOTE] & ~ipv6cp_accept)
                                                                    != 0 ||
       (config_ptr->ipv6cp_info.will_mask[FSM_REMOTE] & ~ipv6cp_accept)
                                                                    != 0) )
  {
    LOG_MSG_ERROR_0("Invalid IPv6CP configuration param(s)!");
    return -1;
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  /*-------------------------------------------------------------------------
     Checking EAP parameters when configured in authenticatee mode.
     Therefore, check if FSM_REMOTE configured correctly
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_EAP
  if((config_ptr->lcp_info.will_mask[FSM_REMOTE] & LCP_N_AP) &&
     ( (uint32)config_ptr->lcp_info.want_vals[FSM_REMOTE].auth_proto_mask & \
        (uint32)PPP_AUTH_EAP) )
  {
    
    /*-----------------------------------------------------------------------
     Verify if the AKA_ALGO type is set appropriately.
     If the AKAALGO type variable is milenage, then ensure that the
     aka_algo_milenage_op_data_len variable is checked to be not
     equal to EAP_MILENAGE_OP_LEN [16 bytes]
    -----------------------------------------------------------------------*/
    if( config_ptr->auth_info.aka_algo_type >= EAP_SIM_AKA_ALGO_MAX )
    {
      LOG_MSG_ERROR_1("Invalid EAP configuration parameter aka_algo_type %d",
                      config_ptr->auth_info.aka_algo_type);
      return -1;
    }

    /*-----------------------------------------------------------------------
      If the USIM card is present, No need to validate the EAP parameters 
      which come from the card
    -----------------------------------------------------------------------*/
    if (TRUE == config_ptr->auth_info.ehrpd_usim_present)
    {
      LOG_MSG_INFO2_0("No need to validate the EAP parameters from the card since"
                      " USIM is present");
      return 0;
    }

    if( config_ptr->auth_info.aka_algo_type == EAP_AKA_ALGO_MILENAGE &&
        config_ptr->auth_info.aka_algo_milenage_op_data_len != 
        EAP_AKA_ALGO_MILENAGE_OP_LEN )
    {
      LOG_MSG_ERROR_2( "Invalid EAP configuration parameter aka_algo_type %d"
           " aka_algo_milenage_op_data_len %d", 
           config_ptr->auth_info.aka_algo_type, 
                       config_ptr->auth_info.aka_algo_milenage_op_data_len );
      return -1;
    }

    if(config_ptr->auth_info.aka_algo_seqnum_arr_num_elements != 
       EAP_AKA_SEQ_NUM_ARRAY_ELEMENTS)
    {
      LOG_MSG_ERROR_0("Invalid EAP configuration parameter");
      return -1;
    }
  }
#endif /* FEATURE_DATA_PS_EAP */

  return 0;
} /* pppi_config_check() */

/*===========================================================================
FUNCTION PPP_INPUT()

DESCRIPTION
  This function processes the packet received from the lower layer,
  according to the mode of the PPP device for which the packet is received.
  Here is a description of the processing for various modes.

  +--------------------------+--------------------------------------------+
  |        PPP Mode          |           Processing routine               |
  +--------------------------+--------------------------------------------+
  | PPP_NONE_MODE            | pppi_none_mode_hdlr(), frees incoming pkt  |
  +--------------------------+--------------------------------------------+
  | PPP_RELAY_MODE           | pppi_relay_mode_hdlr(), INVALID condition  |
  +--------------------------+--------------------------------------------+
  | PPP_PSEUDO_NET_MODE      | pppi_pseudo_net_mode_hdlr(), snoops and    |
  |                          | unframes (via HDLC library) if needed      |
  +--------------------------+--------------------------------------------+
  | PPP_INTERNAL_MODE        |                                            |
  +--------------------------+  pppi_internal_mode_hdlr(), unframes (if   |
  | PPP_FULL_NET_MODE        |  HDLC is enabled) and dispatches packet up |
  +--------------------------+  the stack                                 |
  | PPP_INTERNAL_SNOOP_MODE  |                                            |
  +--------------------------+--------------------------------------------+

   PPP should be the current task when this function is executed.

PARAMETERS
  device    :  PPP device to pass the packet to
  hdlc inst :  instance of HDLC for which the received packet belongs to
  item_ref_ptr : pointer to the incoming PPP packet

RETURN VALUE
   0 : Successfully unframed the packet.
  -1 : Failed to unframe.

DEPENDENCIES
  This function gets executed in PPP task.

SIDE EFFECTS
  None

===========================================================================*/
int ppp_input
(
  ppp_dev_enum_type   device,               /* PPP device to pass pkt to   */
  void              * hdlc_inst,            /* Handle to HDLC instance     */
  dsm_item_type    ** item_ref_ptr          /* Pointer to PPP pkt          */
)
{
  ppp_type             * ppp_cb_ptr;
  ppp_type             * ppp_cb_array = NULL;

  //ps_phys_link_type  * phys_link_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(device >= PPP_MAX_DEV)
  {
    PS_PPP_ASSERT(0);
    return -1;
  }

  if( item_ref_ptr == NULL || *item_ref_ptr == NULL )
  {
    /*"Bad input parameters to ppp_input(dev %d)" */
    DATA_PROTO_ERROR_LOG_2(PS_BAD_ARG_OR_NULL_PTR,item_ref_ptr,device);
    return -1;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = (ppp_type*) (&(ppp_cb_array[device]));

  /*-----------------------------------------------------------------------
    Log incoming frames if HDLC mode is not HDLC_OFF
  -----------------------------------------------------------------------*/
  if(ppp_cb_ptr->hdlc_mode != HDLC_OFF)
  {


#ifdef FEATURE_DATA_PS_PPP_LOGGING
    PPP_LOG_FRAMES_EXT (*item_ref_ptr, PPP_LOG_FRAMED_RX, device);
#endif /* FEATURE_DATA_PS_PPP_LOGGING */

#if 0  
    phys_link_ptr =
      PS_IFACE_GET_PHYS_LINK(ppp_cb_ptr->fsm[IPCP].fwk_inst_ptr);
#ifdef PS_LOGGING_IOE_DPL
    if(phys_link_ptr != NULL)
    {
    DPL_LOG_PPP_PARTIAL_RX_PACKET(phys_link_ptr, *item_ref_ptr);
    }
#endif //PS_LOGGING_IOE_DPL
#endif
  }


  if(!PPP_IS_CURR_TASK_OWNER())
  {
    ASSERT(0);
    LOG_MSG_ERROR_2("ppp_input(%d) must be in owning task, curr_task:0x%x",
                    device, PPP_CURR_TASK());
    dsm_free_packet(item_ref_ptr);
    /*("ppp_input must be in owning task")*/
    PS_PPP_ASSERT(0);
    return -1;
  }
  
  /*-------------------------------------------------------------------------
    If session_handle is zero, then discard the packet right here
  -------------------------------------------------------------------------*/
  if( ppp_cb_ptr->session_handle == 0 )
  {
    LOG_MSG_INFO2_1("ppp_input(%d) on free device!", device);
    dsm_free_packet(item_ref_ptr);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Invoke the PPP-mode specific handler to process this packet. See function
    description above for a description of the various handler routines.
  -------------------------------------------------------------------------*/
  ppp_cb_ptr->rx_pkt_hdlr_f_ptr( device, hdlc_inst, item_ref_ptr );

  return 0;
} /* ppp_input() */



/*===========================================================================
FUNCTION PPP_OUTPUT()

DESCRIPTION
  PPP link transmit function

PARAMETERS
  device: PPP device.
  item_ref_ptr: reference to a dsm item pointer: this is payload for PPP
  meta_info_ptr: pointer to a meta info item.
  higher_layer_protocol: Higher layer protocol id.

RETURN VALUE
  Same as pppi_outgoing
   0 on success
  -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ppp_output
(
  ppp_dev_enum_type       device,
  dsm_item_type        ** item_ref_ptr,
  uint16                  higher_layer_protocol,
  ps_tx_meta_info_type  * meta_info_ptr
)
{
  uint16                   ppp_protocol = PPP_NO_PROTOCOL;
  ppp_type                *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(device >= PPP_MAX_DEV)
  {
    PS_PPP_ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Determine protocol type
  -------------------------------------------------------------------------*/
  if(meta_info_ptr->rt_meta_info_ptr->pkt_info.ip_vsn == IP_V4)
  {
    ppp_protocol = PPP_IP_PROTOCOL;
  }
#ifdef FEATURE_DATA_PS_IPV6
  else if(meta_info_ptr->rt_meta_info_ptr->pkt_info.ip_vsn == IP_V6) 
  {
    ppp_protocol = PPP_IPV6_PROTOCOL;
  }
#endif

#if 0
  ppp_protocol = 
    (higher_layer_protocol == (uint16)PS_PHYS_LINK_HIGHER_LAYER_PROTOCOL_IPV4) ?
               PPP_IP_PROTOCOL :
#ifdef FEATURE_DATA_PS_IPV6
               PPP_IPV6_PROTOCOL;
#else
               0;
#endif /* FEATURE_DATA_PS_IPV6 */
#endif

  ppp_cb_array = fsm_get_ppp_cb_array();
  if(ppp_cb_array[device].mode == PPP_NONE_MODE)
  {
    LOG_MSG_INFO1_1("PPP %d not configured discading pkt", device);
    dsm_free_packet(item_ref_ptr);
    PS_TX_META_INFO_FREE(&meta_info_ptr);
    return -1;
  }

  return pppi_outgoing(device, ppp_protocol, item_ref_ptr, meta_info_ptr);
} /* ppp_output() */

/*===========================================================================
FUNCTION PPP_SET_DEVICE_CONSTANT()

DESCRIPTION
  This function is called to set the configurable protocol constants for
  PPP on the interface specified.

PARAMETERS
  device       : the device for which the user id needs to be updated
  constant     : the PPP constant enum value that needs to be set
  value        : the value that the constant needs to be set to

RETURN VALUE
   0 on success
  -1 on failure

DEPENDENCIES

SIDE EFFECTS/LIMITATION
  None
===========================================================================*/

int ppp_set_device_constant
(
  ppp_dev_enum_type device,
  ppp_init_consts_enum_type constant,
  uint32 value 
)   
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( device >= PPP_MAX_DEV || constant >= PPP_MAX_INIT_CONSTS )
  {
    LOG_MSG_ERROR_2( "ppp_set_device_constant: Invalid args: device %d,"
                     " constant %d", device, constant );
    ASSERT(0);
    return -1;
  }

  switch(constant)
  {
    case LCP_ACK_TIMEOUT: 
    case LCP_REQUEST_TRY: 
       ppp_lcp_init_device_constant(device, constant, value);
       break;

    case IPCP_ACK_TIMEOUT: 
    case IPCP_REQUEST_TRY: 
      ppp_ipcp_init_device_constant(device, constant, value);
      break;
#ifdef FEATURE_DATA_PS_IPV6
    case IPV6CP_ACK_TIMEOUT: 
    case IPV6CP_REQUEST_TRY: 
      ppp_ipv6cp_init_device_constant(device, constant, value);
      break;
#endif /* FEATURE_DATA_PS_IPV6 */
    case DEFAULT_LCP_ACK_TIMEOUT: 
       ppp_def_dev_const[device].lcp_ack_timeout = (uint16)value;
       break;
    case DEFAULT_LCP_REQUEST_TRY: 
       ppp_def_dev_const[device].lcp_try_configure = (uint8)value;
       break;
    case DEFAULT_IPCP_ACK_TIMEOUT: 
       ppp_def_dev_const[device].ipcp_ack_timeout = (uint16)value;
       break;
    case DEFAULT_IPCP_REQUEST_TRY: 
       ppp_def_dev_const[device].ipcp_try_configure = (uint8)value;
       break;
    case DEFAULT_AUTH_TIMEOUT:
       ppp_def_dev_const[device].auth_timeout = (uint16)value;
       break;
    case DEFAULT_AUTH_RETRY: 
       ppp_def_dev_const[device].auth_retry_limit = (uint8)value;
       break;
#ifdef FEATURE_DATA_PS_IPV6
    case DEFAULT_IPV6CP_ACK_TIMEOUT: 
       ppp_def_dev_const[device].ipv6cp_ack_timeout = (uint16)value;
       break;
    case DEFAULT_IPV6CP_REQUEST_TRY: 
       ppp_def_dev_const[device].ipv6cp_try_configure = (uint8)value;
       break;
#endif /* FEATURE_DATA_PS_IPV6 */
    default:
      LOG_MSG_ERROR_1("ppp_set_device_constant: Unknown constant %d",
                      constant);
      return -1;
  }
  return 0;
} /* ppp_set_device_constant */



/*===========================================================================
FUNCTION PPP_GET_MAX_DEVICE()

DESCRIPTION
  This function will return the PPP_MAX_DEV enum.

PARAMETERS
  None

RETURN VALUE
  ppp_dev_enum_type
 
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ppp_dev_enum_type ppp_get_max_device()
{
  return   PPP_MAX_DEV;

}/*ppp_get_max_device*/


