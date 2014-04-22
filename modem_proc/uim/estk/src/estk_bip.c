/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                      ESTK  BEARER INDEPENDENT PROTOCOL


GENERAL DESCRIPTION : ENHANCED STK layer BIP support

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2010 - 2022 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/estk/src/estk_bip.c#9 $

when       who     what, where, why
--------   ---     --------------------------------------------------------------------
11/18/22   ac      Fixed wrong slot id issue in open channel when reuse is bit on in close channel
11/11/21   dt      Clear dss_nethandle_v6 while cleaning up BIP variables
01/13/21   dt      Reset close_ch ewould block flag for close channel command
11/25/20   dt      Do not clear bip globals when network cb is pending
03/03/21   dt      Send terminal response when DS returns DSS_SUCCESS for OPEN CHANNEL command
09/17/20   dt      Fix the double free issue
08/26/20   dt      Fix DNS issue when IPV6 is success
08/06/20   dt      Fixing Offtarget TC failures
07/13/20   ykr     Fix compilation warnings.
03/19/20   dt      User data fix in bip bringup call
03/12/20   ks      Store IPv6 DSS nethandle in bip table.
12/26/19   dt      Adding DNS support for BIP
12/24/19   dt      Wait for net cb after dss_bringdown_call
12/24/19   dt      Add null at the end of APN in BIP channel table
07/19/19   ks      Store dss_nethandle in bip channel table 
05/31/19   dt      Fix compilation warnings
02/22/19   kb      Fix compilation issues on linux mob
01/17/19   ks      Depricate DSS_SO_SYS_SOCK option.
11/30/18   ks      Update DS profile ident info
11/21/18   gs      Set the default value of cellular_allowed to FALSE
10/12/18   ks      Split ME and slot specific NV
10/08/18   gs      Crash during Open Channel Timer Expiry Cleanup
10/08/18   gs      Clean up the BIP Channel Table during Close Channel
08/31/18   dt      9x05 interface changes for memory reduction
04/23/18   dt      Deprecate GSTK_CFG_FEATURE_SLOT_ESTK_BIP_IP_DEFAULT
11/06/17   dt      Do not send channel status indication for closed channel
09/05/17   dt      Do not process BIP over cellular when cellular bearer is null on index 0
09/27/16   sn      Fix to use first index of DS rat array as preferred
06/03/16   shr     GSTK/ESTK globals memory optimization
05/30/16   gs      F3 logging improvements
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
05/16/16   gs      F3 logging improvements
04/14/16   gm      Removal of SCWS code for IOE devices
04/13/16   gm      3gpp2 feature controlled
11/26/15   lm      Fix KW errors
10/01/15   lm      Fix compilation issues on linux mob
09/07/15   gs      Read Default APN NV using GSTK NV API
06/26/15   gs      Update the Channel index while populating SCWS Channel Table
06/15/15   vr      Remove TR delay logic for CLOSE CHANNEL
03/17/15   shr     While processing SEND DATA, along with Link Drop envelope
                   (due to OOS/DS subsystem error), send error TR
03/12/15   gm      Added Channel Index validity check
03/10/15   gm      Fixed incorrect usage of snprintf
02/26/15   gm      Fix channel reuse not working for DNS query open channel
01/22/15   gs      Send the right TR cause when DS Service is not available
01/19/15   kr      Fixed Compilation warnings
01/19/15   vr      estk_cm_cs_call_connected changed as counter
01/08/15   gs      Send respective TR instead of Link Drop event
12/29/14   gs      Fix for rejecting the Open Channel with with Busy on Call
12/17/14   gs      Dereg the IP Filter after properly closing the socket
12/09/14   lj/bd   Deliver profile 254 to DS if null APN provisioned in EHRPD
12/08/14   dy      Re checking in backed out CR 743973
11/25/14   dy      Fix BIP err TR not sent in open ch DNS query fail case
11/24/14   shr     Use gcf_gstk EFS item instead of NV_GPRS_ANITE_GCF_I
11/11/14   shr     Fixed compilation warnings
11/07/14   dy      Backing out fix for CR 743973 temporarily
11/07/14   bd      Ignore keep_ch_open flag in UICC server mode case
11/03/14   dy      Fix channel reuse variable incorrectly reset
                   Fix not attempting to get IPv6 handle for IPv4v6 profile
10/29/14   dy      Fix open ch DNS query followed by legacy close ch clean up error
10/29/14   dy      Fix keep_ch_open timer expiry not stopping PDN connection
10/29/14   dy      Fix possible crash when prev_open_ch cleaned up
10/29/14   sw      Check availability of 3GPP2 system in all indexes of DS avail_sys
10/21/14   shr     Close channel kept open if next proactive cmd is unexpected
10/16/14   dy      Fix incorrect usage of GSTK_IO_GET_DS_SYSTEM_RAT_INFO
10/14/14   bd      Support for configuring NV73530 with lesser than 100 bytes
10/12/14   sw      Check DS service availability for OPEN CH
10/07/14   shr     Memory optmization to the GSTK NV logic
10/01/14   bd      Removed feature GSTK_CFG_FEATURE_ME_ESTK_ICON
09/29/14   dy      Fix dsnet_stop() being called twice for same nethandle
09/25/14   dy      Fix memory leak
09/24/14   dy      Use ephermeral ports for BIP sessions
09/15/14   dy      Do not delete BIP profile if it is being used by other channels
09/15/14   sw      rex_timer_undef() keep_ch_open_timer_p to deallocate timer memory
09/09/14   vr      Reduce stack usage
09/09/14   dy      Fix logic involving IPV6 iface bring up in DNS query
09/09/14   bd      Features clean up
08/22/14   shr     Disable continous read into BIP Rx buffer by default
08/13/14   dy      Fix querying default profile not checking NV item
08/11/14   bd      Support for using default APN from NV 73530 for OPEN CH (default bearer)
08/01/14   gs      Send No specific cause can be given when slot mismatches
07/21/14   sw      Display alpha related changes
07/11/14   shr     Add Support for NV Refresh
07/11/14   gs      Reject the Open Channel incase of GSM, when the CS Call is connected
06/17/14   dy      Fix ch id not being copied to TR for on demand open ch
06/13/14   shr     Enable continous read into BIP Rx buffer based on NV config
06/10/14   bd      Fix 3GPP2 APN, user name and pwd local buffer size
06/03/14   hn      Fix prev_open_ch memory leak
06/03/14   hn      Pack Channel ID in TR for OPEN CH DNS query
05/29/14   shr     Cycle through source ports for consecutive BIP sessions
05/27/14   bd      Changes to hold null alpha and no alpha indications
05/12/14   shr     Do not send Alpha to clients if SEND DATA processing fails
05/12/14   shr     Do not send Link Drop envelope after Close Channel
05/07/14   shr     Fix 3GPP APN local buffer size
05/06/14   gs      Removed the ESTK featurization
05/05/14   gm      Update protocol identifier for IPv6
04/30/14   hn      Close existing socket for legacy OPEN CH reuse
04/03/14   dy      Replace gstk_malloc() with macro
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
03/17/14   hn      Fix crash in channel reuse logic
03/11/14   dy      Fixed wrong TR sent when no data available for recv data req
03/04/14   shr     Made BIP Session in progress API slot specific
03/04/13   shr     Fixed on-demand link establishment implementation
03/03/14   shr     ESTK BIP clean-up and enhancements
02/27/14   dy      Add default_apn, bip_apn_mapping_tbl, rat_priority NV item
02/21/14   dy      Fixed GSTK EFS stub fct bug and added default_apn nv
02/21/14   vr      Check default profile can be used for default bearer
02/21/14   vr      Remove hack in estk_set_net_policy_profile()
02/21/14   gs      Dereg the filter only after socket close is successful
02/11/14   gm      Fix KW critical warning
02/11/14   gs      Establish BIP channel even if we are not able to set a DS profile
                   for a non-preferred network
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
01/09/14   kb      Cleanup global variables while handling STOP signal
01/09/14   dy      Fix array reading error for user reject open ch req case
01/03/14   shr     BIP throughput optimizations
12/05/13   hn      Support DNS query and Channel reuse
11/13/13   shr     Added support for BIP over WLAN
10/10/13   gm      Marked cmd_ref_id to get command table index
10/04/13   sw      Reduced F3 messages
09/13/13   gm      Support for Recovery and hot-swap
09/02/13   sw      Removed returning error for 3GPP2 profile creation in 3GPP case
07/29/13   gm      Sending response to client when data available envelope fail
07/25/13   hn      Enable FEATURE_ESTK_BIP_SERVER_MODE, add control bitmask
07/24/13   hn      Fix merging error leading to BIP username not set (3GPP)
07/23/13   sw      Support for DS callback to obtain RAT information.
07/15/13   gs      Remove NV 5770 dependency for STK related tests except for GSM
06/28/13   sw      Call to estk_bip_set_default_apn_from_efs is removed for
                   GSTK_BEARER_DEFAULT bearer type
06/11/13   hh      Fix Toolkit Compiling Errors
05/24/13   hn      Does APN mapping based on NV item
05/23/13   hn      Reset auth params after 3GPP create profile
                   Reset profile buf len for 3GPP
                   Revert previous change
05/23/13   gm      compilation error in MSG_ERROR macro call
05/23/13   gm      handling duplicate user confirmation
05/13/13   vr      FIX for BIP error returned after pressing no during
                   the user confirmation phase
05/07/13   vr      Send data buffer clean-up, if timer expires for send data
04/11/13   hh      Log "BIP session in progress" only when BIP is active
04/09/13   vr      Use safer versions of memcpy() and memmove()
03/19/13   bd      Migration to transient DS profiles
02/19/13   bd      Do not copy user ID and User pwd while checking Auth params
02/11/13   bd      Support for (e)HRPD access tech
02/07/13   hn      Update apn_class_info table
12/10/12   js      Fixed compiler warnings
10/19/12   hk      Memory Optimization in ESTK through F3 removal
10/08/12   sg      Support class 2/3 APN modification
09/19/12   sg      Store and reuse iface_id for phys link down
09/10/12   sg      Handle SCWS alpha
08/31/12   sg      Read auth parameter to set it in profile for 3gpp
06/25/12   dd      Read CS_PS NV item at init
06/08/12   dd      Fix SCWS Open Ch TR if call is rejected
05/28/12   sg      DS Sys API changes
05/14/12   sg      APN comparison for 3gpp2 shall not be case sensitive
04/10/12   sg      Create-reuse profile for 3gpp2
04/03/12   av      Replace feature flags with NVITEM
04/03/12   xz      Don't free APN, username and passwd until CH ID is freed
02/27/12   sg      When no alpha but QMI is present send to QMI to handle
02/18/12   av      Cleaned up remaining ALWAYS ON/OFF features
01/16/12   sg      Clear User CNF timer
01/03/12   sg      Avoid race condition by not using dropped channel
11/02/11   sg      Remove always on FEATURE_ESTK_BIP_USE_DATA_FILTER
10/20/11   dd      Fix BIP Client Data Available
10/06/11   sg      Remove AEESTD APIs
10/05/11   dd      Fix stale data in SCWS overflow buffer
10/03/11   sg      Send correct error code when send/rcv on closed channel
09/28/11   dd      Clear data buffer in send data or listen state
09/27/11   sg      Set QOS Parameters for TR based on what DS returns
09/23/11   sg      LTE QOS parameters for PDN connection
09/20/11   bd      Fixes for compilation errors
08/24/11   xz      Fix compiler warnings and LINT/KW errors
08/23/11   sg      Clean up after no_sim state
08/22/11   sg      Use ESTK Client id in open channel alpha handling
08/18/11   sg      Remove bearer type versus RAT checks
07/25/11   dd      Remove the SCWS Send Data log to improve performance
07/21/11   bd      Fixed copying issue when dealing with overlapping blocks
07/13/11   dd      SCWS Close Ch Fix
07/13/11   dd      Clear the SCWS Tx/Rx buffers
07/13/11   dd      Fix modified reason code for Open Ch TR
07/13/11   dd      Fix the scws overflow buffer copy
06/24/11   js      Update DS iface policy for dss_open_netlib2()
06/13/11   sg      Move FEATURE_ESTK_IGNORE_ALPHA to NV
05/20/11   sg      Fix crash when clearing null timer for phys link down
05/05/11   dd      Cache overflow SCWS data in ESTK
05/03/11   xz      Fix CMI compile issue
04/29/11   sg      Fix BIP alpha
04/28/11   nk      Fixed warning to include gstk.h need for gstk_get_nv() and
                   fixed warning for return value for non-void function
04/24/11   dd      SCWS support
04/22/11   yk      Fix for a card issue leading to card error during STATUS
                   command
04/20/11   sg      Remove reference to depricated fs.h
04/19/11   nb      Support to check call in progress for OPEN CHANNEL
04/12/11   sg      Fix estk_bip_set_default_apn_from_efs
03/30/11   sg      Use correct global for features_enabled
03/21/11   sg      Change PDN type if NV is set to default
03/09/11   sg      Clean up network handle when socket descriptor is invalid
02/28/11   sg      Add new state for CLOSE IN PROGRESS
02/14/11   dd      Add user confirmation with icon for BIP
02/14/11   sg      Fix close channel for on-demand link establishment
01/26/11   sg      Remove terminating null from APN, username and password
01/26/11   sg      Move registration for DS events outside DS context
12/09/10   js      Support PDN type IP_V4_V6 for BIP
01/03/11   sg      1) Call dsnet_start everytime for BIP call
                   2) Add API for open channel cleanup on gstk timer expiry
                   3) Organize cleanup
12/28/10   sg      Cleanup Open channel proactive command
12/21/10   sg      When setting profiles for Inter-RAT fails ignore error
12/08/10   sg      Port BIP fixes
11/22/10   js      1) Fix to not send duplicate channel status evt for link
                   dropped.
                   2) Try reconnection when you get DS_EINPROGRESS for socket.
11/09/10   bd      Fixed dual standby issues for network related commands
11/10/10   js      Set correct APN profile when EHRPD is defined
10/20/10   xz      Use default profile when APN is null
10/14/10   dd      Set routeable to 1 if filtering APIs are used
10/07/10   dd      Change opt_len size to 4
10/05/10   xz      1) KW Fixes
                   2) Use DS naming convention
10/04/10   js      Use profile 107 only when EHRPD feature is available
10/01/10   dd      Add filtering APIs
10/01/10   dd      Fix for Open Channel User Confirmation
09/23/10   dd      Add support of raw commands to BIP Alpha
08/30/10   xz      1) Fix issue of send error TR
                   2) Don't send CHANNEL STATUS if OPEN CH is pending
08/19/10   xz      Support default APN config
08/18/10   dd      Fix KW error
08/07/10   js      Fixed compilation error
08/06/10   sg      1)Fix crash when received data is equal to max buffer size
                   2)Release handle when establish link fails
                   3)Use Profile 107 always for 3GPP2
08/05/10   sg      Fix compilation error due to merge
07/30/10   dd      Send the Open Ch TR in case of on demand data
07/27/10   sg      Register for Physical Iface Down event
07/23/10   sg      Fix memory overwrite in receive data
07/22/10   sg      Fix compilation warnings on 6610
07/20/10   xz      Initialize extran param for TR
07/06/10   sg      1) Fix Lint error
                   2) Release network handle when no network for open channel
                   3) Reuse profiles on EFS when possible
07/01/10   xz      Fix the issue of send error TR
06/18/10   sg      Fix compilation error on 6610 target
06/10/10   sg      Save profile number and clean it up on close channel
06/09/10   sg      Correct profile creation
04/26/10   sg      Fixes memory leak
04/23/10   yt      Fixed KW errors
04/23/10   sg      Temporary hack for close channel race condition
04/09/10   sg      When no network for open channel send error TR
03/31/10   sg      Fix TCP handling and close channel PDN teardown
03/30/10   sg      Fix memory allocation for APN, User Login and User Password
03/23/10   xz      1) Fix the issue of second OPEN CH not working
                   2) copy alpha only when length is not zero
03/19/10   xz      Ignore access tech check as we don't have info from E-MM
03/15/10   xz      Add support of LTE and setting bearer parameters in profile
03/17/10   sg      UDP multihoming support and TCP bug fixes
03/12/10   sg      BIP bug fixes
02/21/10   xz      Fix link error
02/15/10   nb      Update for Dual Slot Support
02/05/10   sg      Fix malloc issue for APN
01/14/10   sg      Fixed multiple bugs for several commands
01/12/10   sg      Added featurization FEATURE_ESTK_BIP_INTERFACE
11/03/09   sg      Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include <stringl/stringl.h>
#include "uim_msg.h"
#ifdef FEATURE_ESTK
#include "gstk_nv_p.h"
#include "gstk_exp.h"
#include "fs_sys_types.h"
#include "fs_lib.h"
#include "fs_public.h"
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
#include "estk_bip.h"
#include "nv.h"
#include "dss.h"

#include "ds_sys.h"
#include "ds_sys_event.h"

static gstk_access_technology_type  estk_curr_access_tech = GSTK_ACCESS_NONE;

static estk_result_enum_type estk_bip_process_send_data_error(
  uint32 ch_idx);

static estk_result_enum_type estk_bip_process_sock_close_evt(
  sint15 sock_fd,
  uint32 cmd_ref_id);

static estk_result_enum_type estk_bip_process_sock_close_error(
  int                                 index,
  uint32                              cmd_ref_id,
  gstk_general_result_enum_type       general_result,
  gstk_additional_info_ptr_type      *additional_result_ptr,
  gstk_terminal_rsp_extra_param_type *other_info_ptr
);

static estk_result_enum_type estk_bip_get_prio_network(
  estk_network_type               *nw_type_ptr,
  uint8                           *addi_res_ptr,
  gstk_slot_id_enum_type           slot,
  gstk_bearer_code_enum_type       bearer_type,
  gstk_pkt_data_address_enum_type  address_type
);

static estk_result_enum_type estk_populate_open_ch_table(
  int32             index,
  estk_network_type nw_type
);

static estk_result_enum_type estk_bip_send_ch_status_event(
  int32                                    channel_id,
  gstk_ch_status_additional_info_enum_type ch_status_add_info
);

static boolean estk_bip_check_existing_gsm_cs_call(void);

/*===========================================================================

FUNCTION: estk_bip_channel_table_init

DESCRIPTION :
  This function sets bip_channel_table fields with non-zero initial values

PARAMETERS:
  instance: the estk_instances_ptr instance to be initialized

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_channel_table_init(uint32 instance)
{
  uint32 i = 0;

  ESTK_RETURN_IF_NULL_PTR(estk_instances_ptr[instance]);

  for (i = 0; i < ESTK_MAX_BIP_CH; i++)
  {
    estk_instances_ptr[instance]->bip_channel_table[i].sock_fd           = DSS_ERROR;
    estk_instances_ptr[instance]->bip_channel_table[i].channel_id        = -1;
    estk_instances_ptr[instance]->bip_channel_table[i].dss_nethandle_v6        = -1;
  }
}/* estk_bip_channel_table_init() */

/*===========================================================================

FUNCTION: estk_bip_cellular_oos_timer_cb

DESCRIPTION :
  The callback funciton supplied to Cellular OOS timer

PARAMETERS:
  slot_index: Index of the slot corresponding to the timer

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_cellular_oos_timer_cb(uint32 slot_index)
{
  estk_cmd_type            *cmd_ptr = NULL;
  gstk_slot_id_enum_type  slot_id = GSTK_SLOT_ID_MAX;

  UIM_MSG_HIGH_1("OOS timer expiry on slot: 0x%x", slot_index);

  slot_id = (gstk_slot_id_enum_type)slot_index;

  if (!ESTK_IS_VALID_SLOT_ID(slot_id))
  {
    return;
  }

  cmd_ptr = estk_get_cmd_buf();
  if (cmd_ptr != NULL)
  {
    cmd_ptr->hdr.cmd = ESTK_BIP_CELLULAR_OOS_TIMER_EXP_CMD;
    cmd_ptr->hdr.sim_slot_id = slot_id;
    estk_put_cmd(cmd_ptr);
  }
} /* estk_bip_cellular_oos_timer_cb */

/*===========================================================================

FUNCTION: estk_bip_keep_ch_open_timer_cb

DESCRIPTION :
  The callback funciton supplied to keep ch open timer

PARAMETERS:
  index: the index if the channel where the link is dropped

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_keep_ch_open_timer_cb(uint32 mask)
{
  estk_cmd_type          *cmd_ptr = NULL;
  uint32                  index   = ESTK_MAX_BIP_CH;
  gstk_slot_id_enum_type  slot_id = GSTK_SLOT_ID_MAX;
   
  UIM_MSG_HIGH_1("estk_bip_keep_ch_open_timer_cb, mask: 0x%x",mask);
    
  ESTK_BIP_GET_INDEX_FROM_MASK(mask, index);
    
  if (index >= ESTK_MAX_BIP_CH)
  {
    return;
  }
    
  ESTK_BIP_GET_SLOT_FROM_MASK(mask, slot_id);
  
  if (!ESTK_IS_VALID_SLOT_ID(slot_id))
  {
    return;
  }
    
  cmd_ptr = estk_get_cmd_buf();
  if (cmd_ptr != NULL)
  {
    cmd_ptr->hdr.cmd           = ESTK_BIP_KEEP_CH_OPEN_TIMER_EXP_CMD;
    cmd_ptr->hdr.cli_user_data = index;
    cmd_ptr->hdr.sim_slot_id   = slot_id;
    estk_put_cmd(cmd_ptr);
  }

} /* estk_bip_keep_ch_open_timer_cb */

/*===========================================================================

FUNCTION: estk_bip_clean_up_reuse_ch

DESCRIPTION :
  Free memory allocated for the BIP channel that was meant to be reused

PARAMETERS:
  index : [Input] Index of the channel to be cleaned

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_clean_up_reuse_ch(uint32 index)
{
  ESTK_RETURN_IF_NULL_PTR(estk_curr_inst_ptr);

  if (index >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_1("Invalid Channel Table Index: 0x%x",
                  index);
    return;
  }

  if (estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet)
  {
    gstk_free(estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet);
    estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet = NULL;
    estk_curr_inst_ptr->prev_open_ch.access_point_name.length = 0;
  }
  if (estk_curr_inst_ptr->prev_open_ch.data_dest_address.pkt_address.address)
  {
    gstk_free(estk_curr_inst_ptr->prev_open_ch.data_dest_address.pkt_address.address);
    estk_curr_inst_ptr->prev_open_ch.data_dest_address.pkt_address.address = NULL;
  }

  if (estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p != NULL)
  {
    UIM_MSG_HIGH_0("Clean up keep_ch_open_timer_p");
    (void)rex_clr_timer(estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p);
    rex_undef_timer(estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p);
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p);
    estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p = NULL;
  }

  estk_curr_inst_ptr->keep_ch_open = FALSE;
  estk_curr_inst_ptr->ch_open_id   = ESTK_MAX_BIP_CH;
  
  /*estk_curr_inst_ptr->bip_channel_table[index].ds_netcb_status_v4 = DS_ENETNONET;*/

  estk_curr_inst_ptr->bip_channel_table[index].ds_netcb_status_v4 = DS_ENETNONET;
  estk_curr_inst_ptr->bip_channel_table[index].ds_netcb_status_v6 = DS_ENETNONET;
} /*estk_bip_clean_up_reuse_ch*/

/*===========================================================================

FUNCTION: estk_bip_set_default_apn_from_efs

DESCRIPTION :
  This function sets the default APN from EFS

PARAMETERS:
  index: the index in bip_channel_table

DEPENDENCIES:
  None

RETURN VALUE:
  The length of the apn (apn > 0), -1 if error

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
int32 estk_bip_set_default_apn_from_efs(uint32 index)
{
  uint32                   i           = 0;
  size_t                   apn_len     = 0;
  int32                    ret         = -1;
  gstk_status_enum_type    gstk_status = GSTK_SUCCESS;
  gstk_nv_items_data_type  nv_data;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ret);

  if(index >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_1("Invalid ch idx: 0x%x", index);
    return ret;
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].apn_len == 0)
  {
    memset(&nv_data, 0x00, sizeof(nv_data));

    /* Allocate memory in order to read the defualt APN from NV */
    nv_data.default_apn_ptr =
      (gstk_default_apn_data_type*)GSTK_CALLOC(sizeof(gstk_default_apn_data_type));

    if (nv_data.default_apn_ptr == NULL)
    {
      return ESTK_ERROR;
    }

    /* Read the Default APN */
    gstk_status = gstk_nv_access_read(GSTK_NV_SLOT_DEFAULT_APN,
                                      estk_curr_inst_ptr->slot_id,
                                      &nv_data);

    if (gstk_status != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_0("Unable to get the default APN from efs");
      gstk_free(nv_data.default_apn_ptr);
      return ret;
    }

    apn_len = strnlen((char *)nv_data.default_apn_ptr->apn, (ESTK_APN_MAX_LENGTH + 1));
    if ((apn_len > 0) && (apn_len <= ESTK_APN_MAX_LENGTH))
    {
      if (estk_curr_inst_ptr->bip_channel_table[index].apn)
      {
        UIM_MSG_HIGH_0("Previous APN not deallocated, freeing memory now");
        gstk_free(estk_curr_inst_ptr->bip_channel_table[index].apn);
		estk_curr_inst_ptr->bip_channel_table[index].apn = NULL;
      }

      /*
        For 3GPP, DS uses strlen() to get length of APN (not apn_len)
        Hence, memory allocation size is apn_length + 1 in order to add
        a NULL character at the end of the string
      */
      estk_curr_inst_ptr->bip_channel_table[index].apn = (uint8 *)GSTK_CALLOC(apn_len + 1);

      if(estk_curr_inst_ptr->bip_channel_table[index].apn != NULL)
      {
        if(gstk_memcpy(estk_curr_inst_ptr->bip_channel_table[index].apn,
                       nv_data.default_apn_ptr->apn,
                       apn_len,
                       apn_len,
                       sizeof(nv_data.default_apn_ptr->apn)) < apn_len)
        {
          gstk_free(estk_curr_inst_ptr->bip_channel_table[index].apn);
          estk_curr_inst_ptr->bip_channel_table[index].apn = NULL;
          gstk_free(nv_data.default_apn_ptr);
          return -1;
        }
        estk_curr_inst_ptr->bip_channel_table[index].apn_len = (uint32)apn_len;
        /* Add NULL character at end of APN */
        estk_curr_inst_ptr->bip_channel_table[index].apn[apn_len] = (uint8)0;

        for (i = 0; i < estk_curr_inst_ptr->bip_channel_table[index].apn_len; ++i)
        {
          UIM_MSG_HIGH_3("DEFAULT APN[%d] = 0x%x %c", i,
                          estk_curr_inst_ptr->bip_channel_table[index].apn[i],
                          estk_curr_inst_ptr->bip_channel_table[index].apn[i]);
        }
        ret = (int32)apn_len;
      }
    } /* if ((apn_len > 0) && (apn_len <= ESTK_APN_MAX_LENGTH)) */
    else
    {
      UIM_MSG_ERR_1("Incorrect APN configuration, apn_len: 0x%x", apn_len);
    }
    gstk_free(nv_data.default_apn_ptr);
  } /* if(bip_channel_table[index].apn_len == 0) */
  return ret;
} /*estk_bip_set_default_apn_from_efs*/

/*===========================================================================

FUNCTION: estk_find_ch_socket_index

DESCRIPTION :
  This function is used to get the index of the first available channel
  corresponding to the channel status specified as argument

PARAMETERS:
  index : [Input] Value result argument to return available channel
  ch_status : [Input] The status of channel that is of interest

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_find_ch_socket_index(
  int32                     *index,
  estk_channel_status_type   ch_status
)
{
  uint8 i = 0;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if (index == NULL)
  {
    UIM_MSG_ERR_0("index ERR:NULL");
    return ESTK_ERROR;
  }

  if (estk_curr_inst_ptr->keep_ch_open)
  {
    *index = estk_curr_inst_ptr->ch_open_id;
    return ESTK_SUCCESS;
  }

  for (i=0; i< ESTK_MAX_BIP_CH; i++)
  {
    if (estk_curr_inst_ptr->bip_channel_table[i].status == ch_status)
    {
      *index = i;
      estk_curr_inst_ptr->bip_channel_table[i].index = i;
      return ESTK_SUCCESS;
    }
  }
  return ESTK_ERROR;
} /* estk_find_ch_socket_index */

/*===========================================================================
FUNCTION: estk_process_keep_ch_open_expired_evt

DESCRIPTION:
  Process the keep ch open expired event

PARAMETERS:
  ch_idx : [Input] The channel index of which the link is to be torn down

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_keep_ch_open_expired_evt(
  uint32 ch_idx
)
{
  if (ch_idx >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_1("invalid ch idx 0x%x!", ch_idx);
    return ESTK_BAD_INPUT_PARAM;
  }

  (void)estk_bip_clean_up_ch_table(ch_idx);

  return ESTK_SUCCESS;
}

/*===========================================================================

FUNCTION: estk_bip_clean_up_ch_table

DESCRIPTION :
  Free memory allocated for the BIP channel

PARAMETERS:
  index : [Input] Index of the channel to be cleaned

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_clean_up_ch_table(int index)
{
  estk_result_enum_type     estk_result        = ESTK_SUCCESS;

  ESTK_RETURN_IF_NULL_PTR(estk_curr_inst_ptr);

  if(index < 0 || index >= ESTK_MAX_BIP_CH)
  {
    return;
  }

  (void)estk_bip_clean_up_reuse_ch(index);

  if(estk_curr_inst_ptr->bip_channel_table[index].send_buffer != NULL)
  {
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].send_buffer);
    estk_curr_inst_ptr->bip_channel_table[index].send_buffer = NULL;
    estk_curr_inst_ptr->bip_channel_table[index].send_buf_len = 0;
    estk_curr_inst_ptr->bip_channel_table[index].send_data_timer_expired = FALSE;
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer != NULL)
  {
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer);
    estk_curr_inst_ptr->bip_channel_table[index].recv_buffer = NULL;
    estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len = 0;
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].src_addr != NULL)
  {
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].src_addr);
    estk_curr_inst_ptr->bip_channel_table[index].src_addr = NULL;
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].dest_addr != NULL)
  {
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].dest_addr);
    estk_curr_inst_ptr->bip_channel_table[index].dest_addr = NULL;
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].apn != NULL)
  {
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].apn);
    estk_curr_inst_ptr->bip_channel_table[index].apn = NULL;
    estk_curr_inst_ptr->bip_channel_table[index].apn_len = 0;
  }

  if (estk_curr_inst_ptr->bip_channel_table[index].username_len > 0)
  {
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].username);
    estk_curr_inst_ptr->bip_channel_table[index].username = NULL;
    estk_curr_inst_ptr->bip_channel_table[index].username_len = 0;
  }

  if (estk_curr_inst_ptr->bip_channel_table[index].passwd_len > 0)
  {
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].passwd);
    estk_curr_inst_ptr->bip_channel_table[index].passwd = NULL;
    estk_curr_inst_ptr->bip_channel_table[index].passwd_len = 0;
  }

#ifdef FEATURE_CDMA
  memset(&(estk_curr_inst_ptr->bip_channel_table[index].policy_info), 0,
         sizeof(estk_curr_inst_ptr->bip_channel_table[index].policy_info));
#endif
  if(estk_curr_inst_ptr->bip_channel_table[index].sock_fd != DSS_ERROR)
  {
    if(estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_CLOSED)
    {
      estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_CLOSE_IN_PROGRESS;

      if(estk_curr_inst_ptr == NULL)
      {
        UIM_MSG_ERR_0("estk_curr_inst_ptr is NULL");
        estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_CLOSED;
        return;
      }
      if(estk_curr_inst_ptr->bip_channel_table[index].is_scws_ch == FALSE)
      {
        estk_result = estk_bip_process_sock_close_evt(
                        estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                        estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference);
        UIM_MSG_HIGH_1("BIP sock close returns %d", estk_result);
      }
    }
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].close_ch_is_ewould_block != TRUE)
  {
    estk_curr_inst_ptr->bip_channel_table[index].channel_id            = -1;
    estk_curr_inst_ptr->bip_channel_table[index].sock_fd               = DSS_ERROR;
    estk_curr_inst_ptr->bip_channel_table[index].card_has_auth_params  = FALSE;
    estk_curr_inst_ptr->bip_channel_table[index].first_dss_sendto_done = FALSE;
    estk_curr_inst_ptr->bip_channel_table[index].receive_data          = FALSE;
    estk_scws_clear_send_receive_buf(index+1); /* ch_id = index+1 */
    estk_curr_inst_ptr->bip_channel_table[index].receive_data_allowed  = TRUE;
    estk_curr_inst_ptr->bip_channel_table[index].is_scws_ch            = FALSE;
    estk_curr_inst_ptr->bip_channel_table[index].me_port_num           = 0;
    estk_curr_inst_ptr->bip_channel_table[index].close_ch_is_ewould_block = 0;

    estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle         = DSS_ERROR;
    estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle_v6      = DSS_ERROR;
  
    /* If Channel Table clean-up is not due to a fallback, then clean-up the
     cached Open Channel command, Channel status and Network Type */
    if(estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_FALLBACK_IN_PROGRESS)
    {
      estk_curr_inst_ptr->bip_channel_table[index].nw_type = ESTK_NETWORK_NONE;
      estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_CLOSED;
	  estk_curr_inst_ptr->bip_channel_table[index].current_command_id = GSTK_CMD_ENUM_NOT_USE;
      estk_open_ch_cleanup(&estk_curr_inst_ptr->bip_channel_table[index].open_ch_cmd);
    }
  }
} /*estk_bip_clean_up_ch_table*/

/*===========================================================================

FUNCTION: estk_convert_to_ip_format

DESCRIPTION :
  Converts IP address into string format

PARAMETERS:
  index : [Input] Index of the channel to be populated

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_convert_to_ip_format(
  gstk_pkt_data_address_type         *p_addr,
  char                               **p_s_addr)
{
  char    buff[64];
  uint16  bytes_copied = 0;
  uint16  buff_size    = sizeof(buff);
  uint16  i            = 0;
  int16   ret          = 0;

  UIM_MSG_HIGH_2("estk_convert_to_ip_format: p_addr=0x%x, p_s_addr=0x%x",
                 p_addr, p_s_addr);

  if (p_addr == NULL || p_s_addr == NULL)
  {
    return ESTK_ERROR;
  }

  memset(buff, 0x00, buff_size);
  *p_s_addr = NULL;

  UIM_MSG_HIGH_1("estk_convert_to_ip_format: Address type:",
                 p_addr->address_type);
  switch(p_addr->address_type)
  {
  case GSTK_PKT_DATA_IPV4_ADDRESS:
    if (p_addr->pkt_address.length != 4)
    {
      UIM_MSG_ERR_1("invalid addr len 0x%x!", p_addr->pkt_address.length);
      return ESTK_ERROR;
    }
    ret = snprintf(
            buff,
            buff_size,
            "%d.%d.%d.%d",
            p_addr->pkt_address.address[0],
            p_addr->pkt_address.address[1],
            p_addr->pkt_address.address[2],
            p_addr->pkt_address.address[3]);

    if (ret >= buff_size || ret < 0)
    {
      return ESTK_ERROR;      
    }
    break;
  case GSTK_PKT_DATA_IPV6_ADDRESS:
    if (p_addr->pkt_address.length != 16)
    {
      UIM_MSG_ERR_1("invalid addr len 0x%x!", p_addr->pkt_address.length);
      return ESTK_ERROR;
    }

    for (i = 1; i < p_addr->pkt_address.length; i = i + 2)
    {
      if (i != (p_addr->pkt_address.length - 1))
      {
        ret = snprintf(
                (buff + bytes_copied),
                buff_size,
                "%02x%02x:",
                p_addr->pkt_address.address[i - 1],
                p_addr->pkt_address.address[i]);
      }
      else
      {
        ret = snprintf(
                (buff + bytes_copied),
                buff_size,
                "%02x%02x",
                p_addr->pkt_address.address[i - 1],
                p_addr->pkt_address.address[i]);
      }
      if (ret >= buff_size || ret < 0)
      {
        return ESTK_ERROR; 
      }
      bytes_copied = bytes_copied + ret;
      buff_size = buff_size - ret;
    }
    break;
  default:
    UIM_MSG_ERR_1("invalid address_type 0x%x!", p_addr->address_type);
    return ESTK_ERROR;
  }
  for (i = 0; i < strlen(buff); ++i)
  {
    UIM_MSG_HIGH_3("IP Addr[%d] = 0x%x(%c)", i, buff[i], buff[i]);
  }
  i = strlen(buff) + 1;
  *p_s_addr = (char *)GSTK_CALLOC(i);
  if (*p_s_addr == NULL)
  {
    return ESTK_ERROR;
  }
  (void)strlcpy(*p_s_addr, buff, i);
  return ESTK_SUCCESS;
} /* estk_convert_to_ip_format */

/*===========================================================================

FUNCTION: estk_bip_send_ch_status_event

DESCRIPTION :
  This function sends the Channel Status Event Download envelope to GSTK

PARAMETERS:
  channel_id : [Input] Channel ID

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_bip_send_ch_status_event(
  int32                                    channel_id,
  gstk_ch_status_additional_info_enum_type ch_status_add_info
)
{
  gstk_evt_dl_ind_type ch_status;

  UIM_MSG_HIGH_2("In estk_bip_send_ch_status_event, channel id: 0x%x, add info: 0x%x",
                 channel_id, ch_status_add_info);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  memset(&ch_status, 0x00, sizeof(ch_status));

  ch_status.evt_type = GSTK_CH_STATUS_EVT_IND;
  ch_status.evt_ind.ch_status_evt_ind.ch_id = channel_id;
  ch_status.evt_ind.ch_status_evt_ind.ch_status_additional_info =
    ch_status_add_info;
  ch_status.evt_ind.ch_status_evt_ind.is_link_est = FALSE;

  if(gstk_send_envelope_evt_download_command(
       estk_curr_inst_ptr->gstk_client_id,
       0, &ch_status) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_0("Sending Channel Status envelope failed");
    return ESTK_ERROR;
  }
  return ESTK_SUCCESS;
} /* estk_bip_send_ch_status_event */

/*===========================================================================

FUNCTION: estk_bip_process_sock_read_evt

DESCRIPTION :
  This function processes a read socket event from DS

PARAMETERS:
  sock_fd : [Input] Socket descriptor

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_bip_process_sock_read_evt(
  sint15 sock_fd)
{
  int8                   index              = 0;
  sint15                 num_bytes_read     = 0;
  int32                  prev_recv_buf_len  = 0;
  int32                  recv_buf_len_avail = 0;
  sint15                 error_num          = DSS_ERROR;
  sint31                 dss_status         = DSS_ERROR;
  estk_result_enum_type  estk_status        = ESTK_ERROR;
  char                  *read_buffer        = NULL;
  gstk_evt_dl_ind_type   data_avail;
  gstk_evt_dl_ind_type   ch_status;

  UIM_MSG_HIGH_1("estk_bip_process_sock_read_evt: sock_fd=0x%x", sock_fd);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);
  ESTK_BIP_CHECK_SOCK_FD(sock_fd);

  /* Find the Channel index corresponding to the Socket */
  for(index = 0; index < ESTK_MAX_BIP_CH; index++)
  {
    if(estk_curr_inst_ptr->bip_channel_table[index].sock_fd == sock_fd)
    {
      break;
    }
  }

  if(index == ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_0("Invalid sock_fd");
    return ESTK_ERROR;
  }

  memset(&data_avail, 0x00, sizeof(gstk_evt_dl_ind_type));
  memset(&ch_status, 0x00, sizeof(gstk_evt_dl_ind_type));

  /* Only process Read Event when Channel is active */
  if(estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_ACTIVE)
  {
    UIM_MSG_HIGH_1("Cannot process DS_READ_EVENT in Channel State 0x%x",
                   estk_curr_inst_ptr->bip_channel_table[index].status);
    return ESTK_ERROR;
  }

  switch(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol)
  {
    case GSTK_PKT_DATA_TCP:
      /* In case of TCP, when continous read feature is disabled,
         do not read new data if existing data in the receive buffer
         has not yet been sent to the card */
      if((gstk_nv_get_feature_status(
            GSTK_CFG_FEATURE_SLOT_ESTK_ENABLE_BIP_TCP_CONT_READ,
            estk_curr_inst_ptr->slot_id) == FALSE) &&
         (estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len != 0))
      {
        UIM_MSG_HIGH_1("TCP: Cannot append to Receive Buffer when its not empty: %d and continous read feature is disabled",
                       estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len);
        return ESTK_ERROR;
      }

      /* If current length of buffer is non-zero but less than 0xFF, that
         implies that the previous DATA AVAILABLE or RECEIVE DATA TR
         indicated a Channel length of < 0xFF to the card.
         Appending new data to the receive buffer means that there would
         be a mismatch in the Data Length card assumes available and the
         actual data available. Hence, do not read new Data till receive
         buffer is empty */
      if((estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len > 0) &&
         (estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len < 0xFF))
      {
        UIM_MSG_HIGH_1("TCP: Cannot append to Receive Buffer when its not empty and buffer len: %d is < 255",
                       estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len);
        return ESTK_ERROR;
      }
      break;
    /* In case of UDP, do not read new data if existing data in the
     receive buffer has not yet been sent to the card */
    case GSTK_PKT_DATA_UDP:
      if (estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len != 0)
      {
        /* ETSI 102 223: 6.4.29
           In the case of packet/datagram transmission, the terminal shall put
           in the Rx buffer a complete packet SDU and only one at one time. For
           example, if UDP datagrams are received by the terminal, the latter shall
           insert only the SDU of each UDP packet received in the Rx buffer. After
           one SDU has been downloaded by the UICC (using one or several RECEIVE
           DATA commands), the terminal shall insert the next SDU
           of UDP datagram, and so on */
        UIM_MSG_HIGH_1("UDP: Cannot append to Receive Buffer when its not empty, buffer length: %d",
                       estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len);
        return ESTK_ERROR;
      }
      break;
    default:
      UIM_MSG_ERR_1("Invalid protocol: 0x%x, Never reach here",
                    estk_curr_inst_ptr->bip_channel_table[index].transport_protocol);
      return ESTK_ERROR;
  }

  recv_buf_len_avail = estk_curr_inst_ptr->bip_channel_table[index].buffer_size -
                         estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len;

  if (recv_buf_len_avail <= 0)
  {
    UIM_MSG_HIGH_0("No space in receive buffer to Read new data from DS");
    return ESTK_ERROR;
  }

  /* Allocate memory to read from DATA */
  read_buffer = (char *)GSTK_CALLOC(recv_buf_len_avail * sizeof(char));
  if(read_buffer == NULL)
  {
    return ESTK_ERROR;
  }

  UIM_MSG_HIGH_3("Current Recv buffer len: %d, Len available: %d, Max. buffer size: %d",
                 estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len,
                 recv_buf_len_avail,
                 estk_curr_inst_ptr->bip_channel_table[index].buffer_size);

  if(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol ==
     GSTK_PKT_DATA_TCP)
  {
    num_bytes_read = dss_read(sock_fd,
                              read_buffer,
                              int32touint16(recv_buf_len_avail),
                              &error_num);

    UIM_MSG_HIGH_2("dss_read returned %d bytes with error_num %d",
                   num_bytes_read, error_num);
  }
  else if(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol ==
          GSTK_PKT_DATA_UDP)
  {
    num_bytes_read = dss_recvfrom(sock_fd,
                                  read_buffer,
                                  int32touint16(recv_buf_len_avail),
                                  0,
                                  NULL,
                                  NULL,
                                  &error_num);

    UIM_MSG_HIGH_2("dss_recvfrom returned %d bytes with error_num %d",
                   num_bytes_read, error_num);
  }

  do
  {
    /* Code must be added to check boundary conditions */
    if(num_bytes_read > 0 && num_bytes_read <= recv_buf_len_avail)
    {
      (void)memscpy((void *)(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer +
              estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len),
              recv_buf_len_avail,
              (void *)read_buffer,
              num_bytes_read);

      /* Cache the Receive buffer length for later use */
      prev_recv_buf_len = estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len;

      estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len += (int32)num_bytes_read;

      /* If protocol is TCP, attempt continuous read from DATA (ETSI 102 223:
         6.4.29 is not applicable to TCP since its a streaming protocol) if there
         is space available in the receive buffer and continuous read feature
         is enabled */
      if((gstk_nv_get_feature_status(
            GSTK_CFG_FEATURE_SLOT_ESTK_ENABLE_BIP_TCP_CONT_READ,
            estk_curr_inst_ptr->slot_id) == TRUE) &&
         (estk_curr_inst_ptr->bip_channel_table[index].transport_protocol == GSTK_PKT_DATA_TCP) &&
         ((estk_curr_inst_ptr->bip_channel_table[index].buffer_size -
           estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len) > 0))
      {
        /* Request for more Data only if card has been indicated in previous DATA
           AVAILABLE/RECEIVE DATA TR that 0xFF data is available OR
           if receive buffer is empty and more than 0xFE data has been read.
           This is to ensure that there would not be a mismatch in the Data
           Length card assumes available and the actual data available */
        if((prev_recv_buf_len >= 0xFF) ||
           ((prev_recv_buf_len == 0) && (num_bytes_read >= 0xFF)))
        {
          dss_status = dss_async_select(sock_fd, DS_READ_EVENT, &error_num);

          if(dss_status != DSS_ERROR)
          {
            UIM_MSG_HIGH_0("dss_async_select succeeded");
          }
          else
          {
            UIM_MSG_ERR_0("dss_async_select failed");
          }
        }
      }

      /* Send DATA available envelope to card only if Receive Buffer was empty
         prior to read from DS */
      if(prev_recv_buf_len == 0)
      {
         /* Send event download envelope with data available event */
        data_avail.evt_ind.data_avail_evt_ind.ch_status.ch_id
          = estk_curr_inst_ptr->bip_channel_table[index].channel_id;
        data_avail.evt_ind.data_avail_evt_ind.ch_status.ch_status_additional_info
          = GSTK_CH_STATUS_NO_FURTHER_INFO;
        data_avail.evt_ind.data_avail_evt_ind.ch_status.is_link_est = TRUE;
        data_avail.evt_type = GSTK_DATA_AVAIL_EVT_IND;
        data_avail.evt_ind.data_avail_evt_ind.ch_len
          = estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len;

        UIM_MSG_HIGH_1("ESTK sending data available event, Channel Len: %d",
                       estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len);

        if(gstk_send_envelope_evt_download_command(estk_curr_inst_ptr->gstk_client_id,
                                                   0, &data_avail) != GSTK_SUCCESS)
        {
          UIM_MSG_ERR_0("Sending data available envelope failed");
          estk_status = ESTK_ERROR;
          break;
        }
      }
      estk_status = ESTK_SUCCESS;
    }
    else if(num_bytes_read == 0 && error_num == DS_EEOF)
    {
      UIM_MSG_HIGH_0("EOF!");
      estk_status = ESTK_SUCCESS;
    }
    else if(num_bytes_read == DSS_ERROR)
    {
      if(error_num == DS_EWOULDBLOCK)
      {
        UIM_MSG_HIGH_0("dss_read returned EWOULDBLOCK");

        dss_status = dss_async_select(sock_fd, DS_READ_EVENT, &error_num);

        if(dss_status != DSS_ERROR)
        {
          UIM_MSG_HIGH_0("dss_async_select succeeded");
          estk_status = ESTK_SUCCESS;
        }
        else
        {
          UIM_MSG_ERR_0("dss_async_select failed");
          estk_status = ESTK_ERROR;
        }
      }
      else /*if error_num != DS_EWOULDBLOCK*/
      {
        UIM_MSG_HIGH_0("Sending channel dropped event");
        if(estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_DROPPED)
        {
          estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_DROPPED;

          /* Send Channel status Event Download envelope command */
          estk_status = estk_bip_send_ch_status_event(
                          estk_curr_inst_ptr->bip_channel_table[index].channel_id,
                          GSTK_CH_STATUS_LINK_DROPPED);

          if(estk_status != ESTK_SUCCESS)
          {
            UIM_MSG_ERR_0("Sending Channel Status envelope failed");
            estk_status = ESTK_ERROR;
            break;
          }
        }
        else
        {
          UIM_MSG_HIGH_0("Link Drop already sent");
        }
        estk_status = ESTK_SUCCESS;
      }
    }
  }while(0);

  gstk_free(read_buffer);
  read_buffer = NULL;
  return estk_status;
} /* estk_bip_process_sock_read_evt */

/*===========================================================================
FUNCTION: estk_bip_write_sock

DESCRIPTION:


PARAMETERS:

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_bip_write_sock (
  int32               index,
  uint32              cmd_ref_id
)
{
  sint31                              num_bytes       = 0;
  sint15                              error_num       = DSS_ERROR;
  sint31                              dss_status      = DSS_ERROR;
  estk_result_enum_type               estk_status     = ESTK_ERROR;
  uint8                               addi_res        = 0;
  gstk_additional_info_ptr_type       additional_info;
  gstk_terminal_rsp_extra_param_type  tr_extra_param;
  uint16                              i;
  struct ps_sockaddr_in              *p_dest_addr_v4  = NULL;
  uint16                              addr_size;

  if ((index < 0) || (index >= ESTK_MAX_BIP_CH))
  {
    UIM_MSG_ERR_1("estk_bip_write_sock: invalid index=0x%x",
                  index);
    return ESTK_ERROR;
  }

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  p_dest_addr_v4 =
    (struct ps_sockaddr_in*)estk_curr_inst_ptr->bip_channel_table[index].dest_addr;

  UIM_MSG_HIGH_3("estk_bip_write_sock, index=0x%x, cmd_ref_id=0x%x, p_dest_addr_v4=0x%x",
                 index, cmd_ref_id, p_dest_addr_v4);

  if (p_dest_addr_v4 == NULL)
  {
    return ESTK_ERROR;
  }

  ESTK_BIP_CHECK_SOCK_FD(estk_curr_inst_ptr->bip_channel_table[index].sock_fd);

  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));

  additional_info.length = 1;
  additional_info.additional_info_ptr = &addi_res;

  for(i = 0; i<estk_curr_inst_ptr->bip_channel_table[index].send_buf_len; i++)
  {
    UIM_MSG_HIGH_2("Send Data buf %d is 0x%x",
                   i, estk_curr_inst_ptr->bip_channel_table[index].send_buffer[i]);
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol ==
     GSTK_PKT_DATA_TCP)
  {
    num_bytes = dss_write(
                  estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                  (void *)estk_curr_inst_ptr->bip_channel_table[index].send_buffer,
                  estk_curr_inst_ptr->bip_channel_table[index].send_buf_len,
                  &error_num);
  }
  else if(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol ==
          GSTK_PKT_DATA_UDP)
  {
    p_dest_addr_v4->ps_sin_port =
      dss_htons(estk_curr_inst_ptr->bip_channel_table[index].port_number);
    addr_size =
      (p_dest_addr_v4->ps_sin_family == DSS_AF_INET) ?
        sizeof(struct ps_sockaddr_in) : sizeof(struct ps_sockaddr_in6);

    num_bytes = dss_sendto(
                  estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                  estk_curr_inst_ptr->bip_channel_table[index].send_buffer,
                  estk_curr_inst_ptr->bip_channel_table[index].send_buf_len,
                  0, /*flags not supported*/
                  (struct ps_sockaddr*)estk_curr_inst_ptr->bip_channel_table[index].dest_addr,
                  addr_size,
                  &error_num);
  }

  UIM_MSG_HIGH_3("Wrote %d bytes with error_num %d, Buffer Len: %d",
                 num_bytes,
                 error_num,
                 estk_curr_inst_ptr->bip_channel_table[index].send_buf_len);

  tr_extra_param.rsp_type = GSTK_SEND_DATA_CNF;
  tr_extra_param.present = TRUE;

  if(num_bytes == (sint15)estk_curr_inst_ptr->bip_channel_table[index].send_buf_len)
  {
    /* dss_sendto is successful. Cache this info */
    if(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol ==
       GSTK_PKT_DATA_UDP)
    {
      estk_curr_inst_ptr->bip_channel_table[index].first_dss_sendto_done = TRUE;
    }
    estk_curr_inst_ptr->bip_channel_table[index].send_buf_len = 0;
    tr_extra_param.extra_param.send_data_extra_param.ch_data_remain_buf_len =
     estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
    /* Send Success terminal response */
    if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
    {
      UIM_MSG_HIGH_0("Saving SEND DATA TR");
      estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
      estk_curr_inst_ptr->result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
      estk_curr_inst_ptr->extra_param = tr_extra_param;
#ifdef FEATURE_UIM_TEST_FRAMEWORK
      #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
      return ESTK_SUCCESS;
    }
    else
    {
      estk_status = estk_bip_send_terminal_response(
                      cmd_ref_id,
                      GSTK_SEND_DATA_CNF,
                      GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
                      NULL,
                      &tr_extra_param,
                      index);

      if(estk_status == ESTK_ERROR)
      {
       UIM_MSG_ERR_0("Error in sending terminal response");
       return estk_status;
      }
      else
      {
       /* Reset the TX buffer */
       estk_curr_inst_ptr->bip_channel_table[index].send_buf_len = 0;
       return ESTK_SUCCESS;
      }
    }
  }
  else if(num_bytes == DSS_ERROR)
  {
    if(error_num == DS_EWOULDBLOCK)
    {
      dss_status = dss_async_select(estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                                    DS_WRITE_EVENT,
                                    &error_num);
      if(dss_status == DSS_SUCCESS)
      {
        return ESTK_SUCCESS;
      }
      else
      {
        UIM_MSG_ERR_1("dss_async_select failed with error_num %d" , error_num);
        return ESTK_ERROR;
      }
    }
    else
    {
      UIM_MSG_ERR_1("dss_write returns unhandled error_num %d", error_num);

      tr_extra_param.extra_param.send_data_extra_param.ch_data_remain_buf_len =
       estk_curr_inst_ptr->bip_channel_table[index].buffer_size -
       estk_curr_inst_ptr->bip_channel_table[index].send_buf_len;
      addi_res = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;

      if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
      {
        UIM_MSG_HIGH_0("Saving SEND DATA TR");
        estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
        estk_curr_inst_ptr->result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
        estk_cache_additional_info(&additional_info);
        estk_curr_inst_ptr->extra_param = tr_extra_param;
        estk_status = ESTK_SUCCESS;
      }
      else
      {
        /* Send error terminal response */
        estk_status = estk_bip_send_terminal_response(
                        cmd_ref_id,
                        GSTK_SEND_DATA_CNF,
                        GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                        &additional_info,
                        &tr_extra_param,
                        index);
      }
      if(error_num != DS_EMSGSIZE)
      {
       UIM_MSG_ERR_0("DS_EMSGSIZE error -- update channel status ");
       if(estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_DROPPED)
       {
         /* Send Event Download envelope command */
         estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_DROPPED;

         (void)estk_bip_send_ch_status_event(
                 estk_curr_inst_ptr->bip_channel_table[index].channel_id,
                 GSTK_CH_STATUS_LINK_DROPPED);
       }
       else
       {
         UIM_MSG_HIGH_0("Link Drop already sent");
       }
       return ESTK_ERROR;
      }
      else
      {
       return estk_status;
      }
    }
  }
  else /* num of bytes is a positive number */
  {
    UIM_MSG_HIGH_0("dss_write did not write all the bytes of send data");
    estk_curr_inst_ptr->bip_channel_table[index].send_buf_len -= (uint16)num_bytes;
    memsmove(estk_curr_inst_ptr->bip_channel_table[index].send_buffer,
             (size_t)estk_curr_inst_ptr->bip_channel_table[index].send_buf_len,
             estk_curr_inst_ptr->bip_channel_table[index].send_buffer+num_bytes,
             (size_t)estk_curr_inst_ptr->bip_channel_table[index].send_buf_len);
    estk_status = estk_bip_write_sock(index, cmd_ref_id);
    return estk_status;
  }
} /* estk_bip_write_sock */

/*===========================================================================

FUNCTION: estk_bip_process_sock_write_evt

DESCRIPTION :
  This function processes a write socket event from DS

PARAMETERS:
  sock_fd : [Input] Socket descriptor

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_bip_process_sock_write_evt(
  sint15 sock_fd,
  uint32 cmd_ref_id)
{
  int8                  index          = 0;
  estk_result_enum_type estk_status    = ESTK_SUCCESS;

  UIM_MSG_HIGH_2("estk_bip_process_sock_write_evt: sock_fd=0x%x, cmd_ref_id=0x%x",
                 sock_fd, cmd_ref_id);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  for(index = 0; index < ESTK_MAX_BIP_CH; index++)
  {
    if(estk_curr_inst_ptr->bip_channel_table[index].sock_fd == sock_fd)
    {
      break;
    }
  }

  if(index == ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_0("Invalid sock_fd");
    return ESTK_ERROR;
  }

  UIM_MSG_HIGH_2("estk_bip_process_sock_write_evt: Index: 0x%x, Current command is 0x%x",
                 index,
                estk_curr_inst_ptr->bip_channel_table[index].current_command_id);

  if(estk_curr_inst_ptr->bip_channel_table[index].current_command_id == GSTK_OPEN_CH_REQ)
  {
    return estk_perform_net_connect(estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle);
  }
  else if(estk_curr_inst_ptr->bip_channel_table[index].current_command_id == GSTK_SEND_DATA_REQ)
  {
    if((estk_curr_inst_ptr->bip_channel_table[index].on_demand_link == TRUE) &&
       (estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_ACTIVE))
    {
      estk_status = estk_perform_net_connect(estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle);
      UIM_MSG_HIGH_1("Returning from estk_perform_net_connect with status 0x%x",
                     estk_status);
    }

    if((estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_ACTIVE) &&
       (estk_status == ESTK_SUCCESS))
    {
      return estk_bip_write_sock(index, cmd_ref_id);
    }
    else
    {
      UIM_MSG_ERR_0("Channel not active for writing data on socket");
      return ESTK_ERROR;
    }
  }
  return ESTK_ERROR;
} /* estk_bip_process_sock_write_evt */

/*===========================================================================
FUNCTION: estk_bip_process_send_data_error

DESCRIPTION:
  The function:
  - Sends error TR for the SEND DATA being processed if
    not waiting for alpha CNF
  - Sets up error TR info in ESTK globals if SEND DATA processing
    is waiting for alpha CNF
  - Sends link Drop Channel Status Envelope to the card

PARAMETERS:
  ch_idx : [Input] Index of the channel on which SEND DATA is being processed

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_bip_process_send_data_error(
  uint32 ch_idx)
{
  gstk_additional_info_ptr_type       additional_info;
  uint8                               addi_res    = 0;
  estk_result_enum_type               estk_status = ESTK_ERROR;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if(ch_idx >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_1("Invalid ch idx: 0x%x", ch_idx);
    return ESTK_ERROR;
  }

  memset((void *)&additional_info, 0, sizeof(gstk_additional_info_ptr_type));

  addi_res = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
  additional_info.additional_info_ptr = &addi_res;
  additional_info.length = 1;

  if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
  {
    UIM_MSG_HIGH_0("Saving SEND DATA TR");
    estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
    estk_curr_inst_ptr->result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
    estk_cache_additional_info(&additional_info);
    estk_status = ESTK_SUCCESS;
  }
  else
  {
    estk_status = estk_bip_send_terminal_response(
                    estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference,
                    GSTK_SEND_DATA_CNF,
                    GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                    &additional_info,
                    NULL,
                    ch_idx);

    if(estk_status != ESTK_SUCCESS)
    {
      UIM_MSG_ERR_0("Sending TR for send data failed");
    }
  }
  if(estk_curr_inst_ptr->bip_channel_table[ch_idx].status != ESTK_CH_DROPPED)
  {
    estk_curr_inst_ptr->bip_channel_table[ch_idx].status = ESTK_CH_DROPPED;

    (void)estk_bip_send_ch_status_event(
            estk_curr_inst_ptr->bip_channel_table[ch_idx].channel_id,
            GSTK_CH_STATUS_LINK_DROPPED);
  }
  return estk_status;
} /* estk_bip_process_send_data_error */

/*===========================================================================

FUNCTION: estk_bip_process_sock_close_evt

DESCRIPTION :
  This function processes a close socket event from DS

PARAMETERS:
  sock_fd    : [Input] Socket descriptor
  cmd_ref_id : [Input] command reference ID

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_bip_process_sock_close_evt(
  sint15 sock_fd,
  uint32 cmd_ref_id)
{
  sint15                              error_num      = DSS_ERROR;
  sint31                              dss_status     = DSS_ERROR;
  estk_result_enum_type               estk_status    = ESTK_ERROR;
  int32                               index          = 0;
  uint8                               addi_res       = 0;
  uint32                              opt_len        = 0;
  gstk_terminal_rsp_extra_param_type  tr_extra_param;
  gstk_additional_info_ptr_type       additional_info;
  dss_so_linger_type_s                sock_linger;
  gstk_general_result_enum_type       general_result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;

  UIM_MSG_HIGH_2("estk_bip_process_sock_close_evt: sock_fd=0x%x, cmd_ref_id=0x%x",
                 sock_fd, cmd_ref_id);

  for(index = 0; index< ESTK_MAX_BIP_CH; index++)
  {
    if(estk_curr_inst_ptr->bip_channel_table[index].sock_fd == sock_fd)
    {
      break;
    }
  }
  if(index == ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_0("Invalid sock_fd");
    return ESTK_ERROR;
  }

  memset((void *)&sock_linger, 0x00, sizeof(dss_so_linger_type_s));
  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));
  memset(&additional_info, 0x00, sizeof(additional_info));

  /* Check the pending command */
  if (estk_curr_inst_ptr->bip_channel_table[index].current_command_id == GSTK_CLOSE_CH_REQ ||
      estk_curr_inst_ptr->bip_channel_table[index].current_command_id == GSTK_OPEN_CH_REQ ||
      estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_CLOSE_IN_PROGRESS ||
      estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_FALLBACK_IN_PROGRESS)
  {
    addi_res = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    additional_info.additional_info_ptr = &addi_res;
    additional_info.length = 1;

    /* only OPEN CH TR will use those */
    tr_extra_param.present = TRUE;
    tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;
    tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_id = 0;
    tr_extra_param.extra_param.open_ch_extra_param.ch_status.is_link_est =
      FALSE;
    tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_status_additional_info =
      GSTK_CH_STATUS_LINK_DROPPED;
    tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
      estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
    (void)memscpy(&tr_extra_param.extra_param.open_ch_extra_param.bearer_description,
           sizeof(gstk_bearer_description_type),
           &estk_curr_inst_ptr->bip_channel_table[index].bearer_desc,
           sizeof(gstk_bearer_description_type));
    tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
      estk_curr_inst_ptr->bip_channel_table[index].buffer_size;

    sock_linger.l_onoff = 1;
    sock_linger.l_linger = 10000; /* Linger time of 10 seconds */
    opt_len = sizeof(sock_linger);

    /* Set socket option Linger time */
    dss_status = dss_setsockopt(sock_fd,
                                DSS_SOCK,
                                DSS_SO_LINGER,
                                (void *)&sock_linger,
                                &opt_len,
                                &error_num);

    UIM_MSG_HIGH_2("Setting Linger time returned %d and error_num %d",
                   dss_status, error_num);

    dss_status = dss_close(sock_fd, &error_num);

    if(dss_status == DSS_SUCCESS)
    {
      /* IP filter is not register for WLAN. Hence, no need to de-register */
      if(estk_curr_inst_ptr->bip_channel_table[index].nw_type != ESTK_NETWORK_WLAN)
      {
        dss_status = dss_dereg_ip_filter(
                       sock_fd,
                       estk_curr_inst_ptr->bip_channel_table[index].filter_handle,
                       &error_num);
        if(dss_status != 0)
        {
          UIM_MSG_MED_2("dss_dereg_ip_filter failed with status 0x%x error number 0x%x",
                      dss_status, error_num);
        }
      }

      estk_curr_inst_ptr->bip_channel_table[index].sock_fd = DSS_ERROR;      

      if (estk_curr_inst_ptr->keep_ch_open &&
          estk_curr_inst_ptr->bip_channel_table[index].current_command_id ==
          GSTK_CLOSE_CH_REQ)
      {
        UIM_MSG_HIGH_0("Successfully closed existing socket. PDN connection kept open.");

        /* Reset ch status to ESTK_CH_PENDING so new socket can be created for next OPEN CH*/
        estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_PENDING;

        return estk_bip_send_terminal_response(
                 cmd_ref_id,
                 GSTK_CLOSE_CH_CNF,
                 GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
                 NULL,
                 NULL,
                 index);
      }

      dss_status = dss_bringdown_call(estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle, &error_num);
      UIM_MSG_HIGH_2("dss_bringdown_call returns 0x%x with error_num 0x%x",
                       dss_status, error_num);
      if(dss_status == DSS_ERROR)
      {
        UIM_MSG_HIGH_1("dss_bringdown_call returns DSS_ERROR with error_num %d",
                       error_num);
        switch(error_num)
        {
        case DS_EWOULDBLOCK:
        case DS_ENETCLOSEINPROGRESS:
          return ESTK_SUCCESS;
        default:
          if(error_num == DS_EBADAPP)
          {
            UIM_MSG_ERR_0("Invalid network handle");
          }

          /* Handle the Socket Close error */
          return estk_bip_process_sock_close_error(index,
                                                   cmd_ref_id,
                                                   general_result,
                                                   &additional_info,
                                                   &tr_extra_param);
        }
      }
      else if(dss_status == DSS_SUCCESS)
      {
        UIM_MSG_HIGH_1("dss_bringdown_call returns DSS_SUCCESS with error_num %d",
                       error_num);
        switch(error_num)
        {
          case DS_EWOULDBLOCK:
            /* if bringdown_call returns DSS_SUCCESS and error_num is DS_EWOULDBLOCK, \
             wait for network CB with DS_ENETNONET */
            estk_curr_inst_ptr->bip_channel_table[index].close_ch_is_ewould_block = TRUE;
            return ESTK_SUCCESS;
          default:
            break;
        }
      }
      else
      {
        UIM_MSG_ERR_0("Unhandled return status for close channel");

        /* Handle the Socket Close error */
        return estk_bip_process_sock_close_error(index,
                                                 cmd_ref_id,
                                                 general_result,
                                                 &additional_info,
                                                 &tr_extra_param);
      }
	  
      UIM_MSG_HIGH_2("dss_bringdown_call returns 0x%x with error_num 0x%x",
                     dss_status, error_num);
      
        estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_CLOSE_IN_PROGRESS;
        estk_status = ESTK_ERROR;

      if(estk_status == ESTK_SUCCESS)
      {
        return estk_status;
      }

      /* If the proactive command is Close Channel, then success TR to be sent,
         as Socket is successfully closed */
      if(estk_curr_inst_ptr->bip_channel_table[index].current_command_id ==
         GSTK_CLOSE_CH_REQ)
      {
        general_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;

        /* No additional info to be sent in success case */
        additional_info.length = 0;
        additional_info.additional_info_ptr = NULL;
      }

      return estk_bip_process_sock_close_error(index,
                                               cmd_ref_id,
                                               general_result,
                                               &additional_info,
                                               &tr_extra_param);
    }
    else if(dss_status == DSS_ERROR)
    {
      UIM_MSG_HIGH_1("dss_close returns DSS_ERROR with error_num %d",
                     error_num);

      if(error_num == DS_EWOULDBLOCK)
      {
        dss_status = dss_async_select(sock_fd, DS_CLOSE_EVENT, &error_num);
        if(dss_status != DSS_ERROR)
        {
          return ESTK_SUCCESS;
        }
        /* Corner case scenario - not expected to happen */
        else
        {
          UIM_MSG_ERR_0("dss_async_select failed");
        }
      }
      else
      {
        /* IP filter is not register for WLAN. Hence, no need to de-register */
        if(estk_curr_inst_ptr->bip_channel_table[index].nw_type != ESTK_NETWORK_WLAN)
        {
          dss_status = dss_dereg_ip_filter(
                       sock_fd,
                       estk_curr_inst_ptr->bip_channel_table[index].filter_handle,
                       &error_num);
          if(dss_status != 0)
          {
            UIM_MSG_MED_2("dss_dereg_ip_filter failed with status 0x%x error number 0x%x",
                      dss_status, error_num);
          }
        }
      }

      /* Force sock_fd to DSS_ERROR, so that Channel table index can be freed
         up and error TR sent to card */
      estk_curr_inst_ptr->bip_channel_table[index].sock_fd = DSS_ERROR;

      if (estk_curr_inst_ptr->keep_ch_open &&
          estk_curr_inst_ptr->bip_channel_table[index].current_command_id ==
          GSTK_CLOSE_CH_REQ)
      {
        UIM_MSG_HIGH_0("Failed to close existing socket. PDN connection kept open.");

        /* Reset ch status to ESTK_CH_PENDING so new socket can be created for next OPEN CH*/
        estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_PENDING;

        if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
        {
          UIM_MSG_HIGH_0("Saving CLOSE CHANNEL TR");
          estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
          estk_curr_inst_ptr->result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
          estk_cache_additional_info(&additional_info);
          return ESTK_SUCCESS;
        }
        else
        {
          return estk_bip_send_terminal_response(
                   cmd_ref_id,
                   GSTK_CLOSE_CH_CNF,
                   GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                   &additional_info,
                   NULL,
                   index);
        }
      }

      /* Handle the Socket Close error */
      return estk_bip_process_sock_close_error(index,
                                               cmd_ref_id,
                                               general_result,
                                               &additional_info,
                                               &tr_extra_param);
    }
  }
  else
  {
    /* If Open Channel has already been processed */
    if ((estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_ACTIVE) ||
        ((estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_PENDING) &&
         (estk_curr_inst_ptr->bip_channel_table[index].on_demand_link == TRUE)))
    {
      estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_DROPPED;
      return estk_bip_send_ch_status_event(
               estk_curr_inst_ptr->bip_channel_table[index].channel_id,
               GSTK_CH_STATUS_LINK_DROPPED);
    }
    else
    {
      UIM_MSG_HIGH_0("Link Drop already sent");
      return ESTK_SUCCESS;
    }
  }
  return ESTK_ERROR;
} /* estk_bip_process_sock_close_evt */

/*===========================================================================

FUNCTION: estk_bip_process_sock_close_error

DESCRIPTION :
  This function processes a close socket error and sends down
  TR to GSTK based on the proactive command in progress

PARAMETERS:
  index :                 [Input] Index of the channel on which error occured
  cmd_detail_ref_id:      [Input] Specifies the reference ID for command details of
                                  the proactive command that the client is sending
                                  a terminal response to. The reference id value
                                  is sent to the client earlier in the structure
                                  of the proactive command request, inside the
                                  gstk_exp_hdr_type
  general_result:         [Input] Specifies the result of the proactive command
                                  operation
  additional_result_ptr:  [Input] Pointer specifies the optional additional result
                                  for the proactive command, the length field in
                                  the gstk_additonal_info_type specifies number of
                                  additional_info in the structure
  other_info_ptr:         [Input] A union pointer of various extra parameters for the
                                  various proactive command response, please refer to
                                  gstk_terminal_rsp_param_type for details

DEPENDENCIES:
  None

RETURN VALUE:
  estk_status_enum_type:
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_bip_process_sock_close_error(
  int                                 index,
  uint32                              cmd_ref_id,
  gstk_general_result_enum_type       general_result,
  gstk_additional_info_ptr_type      *additional_result_ptr,
  gstk_terminal_rsp_extra_param_type *other_info_ptr
)
{
  estk_result_enum_type estk_status = ESTK_ERROR;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if((index < 0) || (index >= ESTK_MAX_BIP_CH))
  {
    UIM_MSG_ERR_1("Invalid ch idx: 0x%x", index);
    return ESTK_ERROR;
  }

  /* Fallback has to be stopped due to Socket Close Error */
  if(estk_curr_inst_ptr->bip_channel_table[index].status ==
     ESTK_CH_FALLBACK_IN_PROGRESS)
  {
    estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_CLOSE_IN_PROGRESS;
  }

  if (estk_curr_inst_ptr->bip_channel_table[index].current_command_id == GSTK_CLOSE_CH_REQ)
  {
    if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
    {
      UIM_MSG_HIGH_0("Saving CLOSE CHANNEL TR");
      estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
      estk_curr_inst_ptr->result = general_result;
      estk_cache_additional_info(additional_result_ptr);
#ifdef FEATURE_UIM_TEST_FRAMEWORK
      #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
      return ESTK_SUCCESS;
    }
    else
    {
      (void)estk_bip_clean_up_ch_table(index);

      return estk_bip_send_terminal_response(
               cmd_ref_id,
               GSTK_CLOSE_CH_CNF,
               general_result,
               additional_result_ptr,
               NULL,
               index);
    }
  }
  else if (estk_curr_inst_ptr->bip_channel_table[index].current_command_id == GSTK_OPEN_CH_REQ)
  {
    (void)estk_bip_clean_up_ch_table(index);

    return estk_bip_send_terminal_response(
             cmd_ref_id,
             GSTK_OPEN_CH_CNF,
             general_result,
             additional_result_ptr,
             other_info_ptr,
             index);
  }
  else if (estk_curr_inst_ptr->bip_channel_table[index].current_command_id == GSTK_SEND_DATA_REQ)
  {
    if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
    {
      UIM_MSG_HIGH_0("Saving SEND DATA TR");
      estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
      estk_curr_inst_ptr->result = general_result;
      estk_cache_additional_info(additional_result_ptr);
      estk_status = ESTK_SUCCESS;
    }
    else
    {
      estk_status = estk_bip_send_terminal_response(
                      cmd_ref_id,
                      GSTK_SEND_DATA_CNF,
                      general_result,
                      additional_result_ptr,
                      NULL,
                      index);
    }

    (void)estk_bip_send_ch_status_event(
            estk_curr_inst_ptr->bip_channel_table[index].channel_id,
            GSTK_CH_STATUS_LINK_DROPPED);

    return estk_status;
  }
  else
  {
    return ESTK_SUCCESS;
  }
} /* estk_bip_process_sock_close_error */

/*===========================================================================

FUNCTION: estk_bip_process_sock_evt

DESCRIPTION :
  This function processes a socket event from DS

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_bip_process_sock_evt(
  const estk_cmd_type *cmd_ptr
)
{
  estk_result_enum_type       estk_status  = ESTK_ERROR;

  if(cmd_ptr == NULL)
  {
    return ESTK_ERROR;
  }

  UIM_MSG_HIGH_3("estk_bip_process_sock_evt: evt_type=0x%x, sock_fd=0x%x, "
                 "cmd_ref_id=0x%x",
                 cmd_ptr->cmd_data.sock_evt.evt_type,
                 cmd_ptr->cmd_data.sock_evt.sock_fd,
                 cmd_ptr->hdr.cmd_ref_id);

  if(cmd_ptr->cmd_data.sock_evt.evt_type & ESTK_DS_CLOSE_EVENT)
  {
    estk_status = estk_bip_process_sock_close_evt(
                    cmd_ptr->cmd_data.sock_evt.sock_fd,
                    cmd_ptr->hdr.cmd_ref_id);
  }
  /* Read and Write events can come in together */
  else
  {
    if(cmd_ptr->cmd_data.sock_evt.evt_type & (uint32)ESTK_DS_READ_EVENT)
    {
      estk_status = estk_bip_process_sock_read_evt(
                    cmd_ptr->cmd_data.sock_evt.sock_fd);
      UIM_MSG_HIGH_1("estk_bip_process_sock_read_evt() rc=0x%x", estk_status);
    }
    if(cmd_ptr->cmd_data.sock_evt.evt_type & (uint32)ESTK_DS_WRITE_EVENT)
    {
    estk_status = estk_bip_process_sock_write_evt(
                    cmd_ptr->cmd_data.sock_evt.sock_fd,
                    cmd_ptr->hdr.cmd_ref_id);
    }
  }
  UIM_MSG_HIGH_1("estk_bip_process_sock_evt: rc=0x%x", estk_status);
  return estk_status;
} /* estk_bip_process_sock_evt */

/*===========================================================================

FUNCTION: estk_bip_network_cb

DESCRIPTION :
  This function serves as cb for the network related events

PARAMETERS:
  net_callback_info_ptr : [Input] DSS Net Callback parameters

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_network_cb(
  dss_net_callback_info_s *net_callback_info_ptr)
{
  estk_cmd_type             *task_cmd_ptr = NULL;
  gstk_slot_id_enum_type     slot_id      = GSTK_SLOT_ID_MAX;
  estk_bip_net_cb_user_data *user_data_ptr    = NULL;

  UIM_MSG_HIGH_0("estk_bip_network_cb");

  if((net_callback_info_ptr == NULL)||
     (net_callback_info_ptr->net_cb_user_data == NULL))
  {
    return;
  }

  user_data_ptr = ((estk_bip_net_cb_user_data *)net_callback_info_ptr->net_cb_user_data);
  slot_id       = user_data_ptr->slot_id;
  UIM_MSG_HIGH_1("estk_bip_network_cb: slot_id=0x%x", slot_id);

  if(!ESTK_IS_VALID_SLOT_ID(slot_id))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", slot_id);
    return;
  }

  ESTK_RETURN_IF_NULL_PTR(estk_instances_ptr[(uint32)slot_id - 1]);
 
  /* get the command from buffer */
  task_cmd_ptr = estk_get_cmd_buf();
  if(task_cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("task_cmd_ptr is NULL");
    return;
  }
  
  task_cmd_ptr->hdr.cmd_ref_id =
    estk_instances_ptr[(uint32)slot_id - 1]->curr_cmd.hdr_cmd.cmd_detail_reference;
  task_cmd_ptr->hdr.cmd                            = ESTK_BIP_PROCESS_NET_CB_CMD;
  task_cmd_ptr->hdr.sim_slot_id                    = slot_id;
  task_cmd_ptr->cmd_data.net_cb_info.dss_nethandle = net_callback_info_ptr->dss_nethandle;
  task_cmd_ptr->cmd_data.net_cb_info.error_num     = net_callback_info_ptr->dss_errno;
  task_cmd_ptr->cmd_data.net_cb_info.user_data.slot_id        = user_data_ptr->slot_id;
  task_cmd_ptr->cmd_data.net_cb_info.user_data.ch_index       = user_data_ptr->ch_index;
  task_cmd_ptr->cmd_data.net_cb_info.user_data.address_family = user_data_ptr->address_family;
  
  estk_put_cmd(task_cmd_ptr);
  
  if((estk_instances_ptr[(uint32)slot_id - 1]->bip_channel_table[user_data_ptr->ch_index].current_command_id == GSTK_CLOSE_CH_REQ)&&
     (task_cmd_ptr->cmd_data.net_cb_info.error_num == DS_ENETNONET) &&
     (estk_instances_ptr[(uint32)slot_id - 1]->keep_ch_open == FALSE))
  {
    gstk_free(net_callback_info_ptr->net_cb_user_data);
    net_callback_info_ptr->net_cb_user_data = NULL;
  }  
}/* estk_bip_network_cb */


/*===========================================================================

FUNCTION: estk_bip_socket_cb

DESCRIPTION :
  This function serves as cb for the socket related events

PARAMETERS:
  sock_callback_info_ptr : [Input] DSS SOCKET Callback parameters

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_socket_cb (
  dss_sock_callback_info_s *sock_callback_info_ptr)
{
  estk_cmd_type              *task_cmd_ptr = NULL;
  gstk_slot_id_enum_type      slot_id      = GSTK_SLOT_ID_MAX;
  estk_bip_sock_cb_user_data *user_data_ptr = NULL;

  if((sock_callback_info_ptr == NULL)||(sock_callback_info_ptr->sock_cb_user_data == NULL))
  {
    UIM_MSG_ERR_0("sock_cb_user_data is NULL");
    return;
  }

  user_data_ptr = ((estk_bip_sock_cb_user_data *)sock_callback_info_ptr->sock_cb_user_data);
  slot_id       = user_data_ptr->slot_id;

  UIM_MSG_HIGH_2("estk_bip_socket_cb: slot_id=0x%x, dss_nethandle=0x%x",
                 slot_id, sock_callback_info_ptr->dss_nethandle);

  if(!ESTK_IS_VALID_SLOT_ID(slot_id))
  {
    UIM_MSG_ERR_1("Invalid Slot ID: 0x%x",slot_id);
    return;
  }

  ESTK_RETURN_IF_NULL_PTR(estk_instances_ptr[(uint32)slot_id - 1]);

  /* get the command from buffer */
  task_cmd_ptr = estk_get_cmd_buf();
  if(task_cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("task_cmd_ptr is NULL");
    return;
  }

  task_cmd_ptr->hdr.cmd_ref_id =
    estk_instances_ptr[(uint32)slot_id - 1]->curr_cmd.hdr_cmd.cmd_detail_reference;
  task_cmd_ptr->hdr.sim_slot_id            = slot_id;
  task_cmd_ptr->hdr.cmd                    = ESTK_BIP_SOCK_EVT_CMD;
  task_cmd_ptr->cmd_data.sock_evt.evt_type = sock_callback_info_ptr->event_mask;
  task_cmd_ptr->cmd_data.sock_evt.sock_fd  = sock_callback_info_ptr->sockfd;
  task_cmd_ptr->result                     = ESTK_SUCCESS;
  
  estk_put_cmd(task_cmd_ptr);

  if((estk_instances_ptr[(uint32)slot_id - 1]->bip_channel_table[user_data_ptr->ch_index].current_command_id == GSTK_CLOSE_CH_REQ)&&
     (task_cmd_ptr->cmd_data.sock_evt.evt_type == ESTK_DS_CLOSE_EVENT) && 
     (estk_instances_ptr[(uint32)slot_id - 1]->keep_ch_open == FALSE))
  {
    gstk_free(sock_callback_info_ptr->sock_cb_user_data);
    sock_callback_info_ptr->sock_cb_user_data = NULL;
  }
}/* estk_bip_socket_cb */

/*===========================================================================

FUNCTION: estk_send_dns_tr

DESCRIPTION :
  This function sends the DNS addresses to card in a Terminal Response

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_send_dns_tr(
  uint32 index)
{
  estk_result_enum_type               estk_status        = ESTK_ERROR;
  sint15                              dsErrno            = 0;
  int16                               result             = 0;
  ds_pdn_ioctl_get_all_dns_addrs_type argVal;
  uint32                              i                  = 0;
  gstk_terminal_rsp_extra_param_type  tr_extra_param;
  gstk_additional_info_ptr_type       additional_info;
  uint8                               addi_res           = 0;
  gstk_general_result_enum_type       gen_result         = GSTK_GENERAL_RESULT_MAX;
  uint32                              cmd_ref_id         = 0;
  sint15                              client_handle      = 0;
  uint8                               client_type        = DS_FWK_CLIENT_SOCKETS;
  uint8                               valid_dns_addr     = 0;
  uint8                               valid_dns_addr_idx = 0;

  UIM_MSG_HIGH_1("estk_send_dns_tr, index %d", index);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));
  memset(&additional_info, 0x00, sizeof(additional_info));
  cmd_ref_id = estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference;

  if (index >= ESTK_MAX_BIP_CH || 
      ((estk_curr_inst_ptr->bip_channel_table[index].ds_netcb_status_v4 != DS_ENETISCONN) &&
      (estk_curr_inst_ptr->bip_channel_table[index].ds_netcb_status_v6 != DS_ENETISCONN)))
  {
    /* Send error TR */
    UIM_MSG_ERR_1("estk_send_dns_tr, no valid nethandle or index %d out of bound",
                  index);
    gen_result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
    addi_res   = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    additional_info.length = 1;
    additional_info.additional_info_ptr = &addi_res;

    tr_extra_param.present = TRUE;
    tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;
    tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_id = 0;
    tr_extra_param.extra_param.open_ch_extra_param.ch_status.is_link_est =
      FALSE;
    tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_status_additional_info =
      GSTK_CH_STATUS_NO_FURTHER_INFO;
    tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
      estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
    (void)memscpy(&tr_extra_param.extra_param.open_ch_extra_param.bearer_description,
           sizeof(gstk_bearer_description_type),
           &estk_curr_inst_ptr->bip_channel_table[index].bearer_desc,
           sizeof(gstk_bearer_description_type));

    goto ESTK_DNS_ADDRS_SEND_TR;
  }

  /* Reset number of DNS addresses saved */
  estk_curr_inst_ptr->dns_addrs_len = 0;
  
  argVal.dns_addrs_ptr = estk_curr_inst_ptr->dns_addrs;
  argVal.num_dns_addrs = DS_NUM_DNS_ADDRS;
  client_handle = estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle;

    /*Get updated DNS parameters from DS for PDN%*/
  if(ds_pdn_ioctl(client_type,
                  client_handle,
                  DS_PDN_IOCTL_GET_ALL_DNS_ADDR,
                  &argVal,
                  &dsErrno))
  {
    UIM_MSG_HIGH_1("IOCTL to get DNS addresses failed with error %d", dsErrno);
  }

  if (DSS_SUCCESS == result)
  {
    UIM_MSG_HIGH_0("Retrieved DNS addresses");

    /* Update number of DNS addresses saved so far */
    estk_curr_inst_ptr->dns_addrs_len += argVal.num_dns_addrs;
    for (i = 0; i < estk_curr_inst_ptr->dns_addrs_len &&
         i < (sizeof(estk_curr_inst_ptr->dns_addrs) / sizeof(ip_addr_type)); ++ i)
    {
      if (estk_curr_inst_ptr->dns_addrs[i].type == IPV4_ADDR)
      {
        UIM_MSG_HIGH_2("DNS[%d]: 0x%x",
                       i, estk_curr_inst_ptr->dns_addrs[i].addr.v4);
      }
    }
  }

  
  argVal.dns_addrs_ptr = &estk_curr_inst_ptr->dns_addrs[estk_curr_inst_ptr->dns_addrs_len];
  argVal.num_dns_addrs = DS_NUM_DNS_ADDRS;
  client_handle = estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle_v6;

    /*Get updated DNS parameters from DS for PDN%*/
  if(ds_pdn_ioctl(client_type,
                  client_handle,
                  DS_PDN_IOCTL_GET_ALL_DNS_ADDR,
                  &argVal,
                  &dsErrno))
  {
    UIM_MSG_HIGH_1("IOCTL to get DNS addresses failed with error %d", dsErrno);
  }

  if (DSS_SUCCESS == result)
  {
    UIM_MSG_HIGH_0("Retrieved DNS addresses");

    /* Update number of DNS addresses saved so far */
    estk_curr_inst_ptr->dns_addrs_len += argVal.num_dns_addrs;
    for (i = 0; i < estk_curr_inst_ptr->dns_addrs_len &&
         i < (sizeof(estk_curr_inst_ptr->dns_addrs) / sizeof(ip_addr_type)); ++ i)
    {
      if (estk_curr_inst_ptr->dns_addrs[i].type == IPV4_ADDR)
      {
        UIM_MSG_HIGH_2("DNS[%d]: 0x%x",
                       i, estk_curr_inst_ptr->dns_addrs[i].addr.v4);
      }
      else if(estk_curr_inst_ptr->dns_addrs[i].type == IPV6_ADDR)
      {
        UIM_MSG_HIGH_3("DNS[0x%x]: 0x%lx 0x%lx",
                        i,
                        estk_curr_inst_ptr->dns_addrs[i].addr.v6[0],
                        estk_curr_inst_ptr->dns_addrs[i].addr.v6[1]);
      }
    }
  }

  UIM_MSG_HIGH_1("Number of DNS addresses from DS: %d",
                 estk_curr_inst_ptr->dns_addrs_len);

  if (estk_curr_inst_ptr->dns_addrs_len > 0)
  {
    for (i = 0; i < estk_curr_inst_ptr->dns_addrs_len; ++ i)
    {
      /* Check for valid DNS addresses from DS */
      if ((estk_curr_inst_ptr->dns_addrs[i].type == IPV4_ADDR) ||
          (estk_curr_inst_ptr->dns_addrs[i].type == IPV6_ADDR))
      {
        valid_dns_addr++;
      }
    }
    UIM_MSG_HIGH_1("Number of valid DNS addresses : %d",
                   valid_dns_addr);
    tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.num_addresses = valid_dns_addr;
  }


  if (tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.num_addresses > 0)
  {
    tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses =
      GSTK_CALLOC(tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.num_addresses * sizeof(gstk_data_address_type));
    if (!tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses)
    {
      /* Send error TR */
      gen_result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
      addi_res   = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
      additional_info.length = 1;
      additional_info.additional_info_ptr = &addi_res;
      goto ESTK_DNS_ADDRS_SEND_TR;
    }
    memset(tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses,
           0x00,
           tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.num_addresses * sizeof(gstk_data_address_type));
  }

  gen_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
  additional_info.length = 0;
  additional_info.additional_info_ptr = NULL;

  tr_extra_param.present = TRUE;
  tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;
  (void)memscpy(&tr_extra_param.extra_param.open_ch_extra_param.bearer_description,
                sizeof(gstk_bearer_description_type),
                &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.bearer,
                sizeof(gstk_bearer_description_type));
  tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
    estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
  tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_id =
    estk_curr_inst_ptr->bip_channel_table[index].channel_id;
  tr_extra_param.extra_param.open_ch_extra_param.ch_status.is_link_est =
    TRUE;

  UIM_MSG_HIGH_0("Convert ip_addr_type to gstk_data_address_type");
  for (i = 0; i < estk_curr_inst_ptr->dns_addrs_len; ++ i)
  {
    if (estk_curr_inst_ptr->dns_addrs[i].type == IPV4_ADDR)
    {
      tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[valid_dns_addr_idx].address =
        GSTK_CALLOC(GSTK_IPV4_ADDRESS_LEN);
      if (tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[valid_dns_addr_idx].address)
      {
        tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[valid_dns_addr_idx].length = 4;
        (void)memscpy(tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[valid_dns_addr_idx].address,
                      tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[valid_dns_addr_idx].length,
                      &estk_curr_inst_ptr->dns_addrs[i].addr.v4,
                      tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[valid_dns_addr_idx].length);
      }
      valid_dns_addr_idx++;
    }
    if (estk_curr_inst_ptr->dns_addrs[i].type == IPV6_ADDR)
    {
      tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[valid_dns_addr_idx].address =
        GSTK_CALLOC(GSTK_IPV6_ADDRESS_LEN);
      if (tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[valid_dns_addr_idx].address)
      {
        tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[valid_dns_addr_idx].length = 16;
        (void)memscpy(tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[valid_dns_addr_idx].address,
                      tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[valid_dns_addr_idx].length,
                      estk_curr_inst_ptr->dns_addrs[i].addr.v6,
                      tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[valid_dns_addr_idx].length);
      }
      valid_dns_addr_idx++;
    }
  }

ESTK_DNS_ADDRS_SEND_TR:
  if (gen_result != GSTK_COMMAND_PERFORMED_SUCCESSFULLY && index < ESTK_MAX_BIP_CH)
  {
    estk_bip_clean_up_ch_table(index);
  }

  estk_status = estk_bip_send_terminal_response(
                  cmd_ref_id,
                  GSTK_OPEN_CH_CNF,
                  gen_result,
                  &additional_info,
                  &tr_extra_param,
                  index);

  if (tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses)
  {
    for (i = 0; i < (uint32)tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.num_addresses; ++i)
    {
      if (tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[i].address)
      {
        gstk_free(tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[i].address);
        tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses[i].address = NULL;
      }
    }
    gstk_free(tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses);
    tr_extra_param.extra_param.open_ch_extra_param.dns_addrs.addresses = NULL;
  }

  return estk_status;
} /* estk_send_dns_tr */

/*===========================================================================

FUNCTION: estk_perform_net_disconnect

DESCRIPTION :
  This function tears down the link and takes care of releasing resources

PARAMETERS:
  dss_nethandle : [Input] DS Nethandle

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_perform_net_disconnect(
  sint15 dss_nethandle)
{
  uint8                              i           = 0;
  gstk_terminal_rsp_extra_param_type  tr_extra_param;
  gstk_additional_info_ptr_type       additional_info;
  uint8                               addi_res   = 0;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));
  memset((void *)&additional_info, 0, sizeof(gstk_additional_info_ptr_type));

  for(i = 0; i < ESTK_MAX_BIP_CH; i++)
  {
    if(estk_curr_inst_ptr->bip_channel_table[i].dss_nethandle== dss_nethandle)
	{
      /* Matching network handle */
      switch(estk_curr_inst_ptr->bip_channel_table[i].current_command_id)
      {
        case GSTK_OPEN_CH_REQ:
	      estk_curr_inst_ptr->bip_channel_table[i].close_ch_is_ewould_block = FALSE;
          (void)estk_bip_clean_up_ch_table(i);
          /* Send error TR for open channel */
          tr_extra_param.present = TRUE;
          tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;
          tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_id = 0;
          tr_extra_param.extra_param.open_ch_extra_param.ch_status.is_link_est =
            FALSE;
  
          (void)memscpy(&tr_extra_param.extra_param.open_ch_extra_param.bearer_description,
                 sizeof(gstk_bearer_description_type),
                 &estk_curr_inst_ptr->bip_channel_table[i].bearer_desc,
                 sizeof(gstk_bearer_description_type));
  
          tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
            estk_curr_inst_ptr->bip_channel_table[i].buffer_size;
  
          addi_res = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
          additional_info.additional_info_ptr = &addi_res;
          additional_info.length = 1;

          if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
          {
            UIM_MSG_HIGH_0("Saving Open Channel Error TR");
            estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
            estk_curr_inst_ptr->result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
            estk_cache_additional_info(&additional_info);
            estk_curr_inst_ptr->extra_param = tr_extra_param;
          }
          else
          {
            if(estk_bip_send_terminal_response(
                estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference,
                GSTK_OPEN_CH_CNF,
                GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                &additional_info,
                &tr_extra_param,
                i) != ESTK_SUCCESS)
            {
              UIM_MSG_ERR_0("Sending TR for open channel failed");
              return ESTK_ERROR;
            }
          }
          return ESTK_SUCCESS;
        case GSTK_SEND_DATA_REQ:
          return estk_bip_process_send_data_error(i);
        case GSTK_CLOSE_CH_REQ:
          if(estk_curr_inst_ptr->bip_channel_table[i].close_ch_is_ewould_block)
          {
            /* No additional info to be sent in success case */
            additional_info.length = 0;
            additional_info.additional_info_ptr = NULL;
			estk_curr_inst_ptr->bip_channel_table[i].close_ch_is_ewould_block = FALSE;
            estk_bip_process_sock_close_error(i,estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference,
                                            GSTK_COMMAND_PERFORMED_SUCCESSFULLY,&additional_info,
                                            &tr_extra_param);
          }
          else
          {
        (void)estk_bip_process_sock_close_evt(
                estk_curr_inst_ptr->bip_channel_table[i].sock_fd,
                estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference);
          }
          return ESTK_SUCCESS;
        default:
          UIM_MSG_HIGH_2("Current command is 0x%x and ch status 0x%x",
                         estk_curr_inst_ptr->bip_channel_table[i].current_command_id,
                         estk_curr_inst_ptr->bip_channel_table[i].status);
          
          /* If Open Channel has already been processed */
          if ((estk_curr_inst_ptr->bip_channel_table[i].status == ESTK_CH_ACTIVE) ||
            ((estk_curr_inst_ptr->bip_channel_table[i].status == ESTK_CH_PENDING) &&
             (estk_curr_inst_ptr->bip_channel_table[i].on_demand_link == TRUE)))
          {
            estk_curr_inst_ptr->bip_channel_table[i].status = ESTK_CH_DROPPED;
            /* Send Channel status Event Download envelope command */
            (void)estk_bip_send_ch_status_event(
                  estk_curr_inst_ptr->bip_channel_table[i].channel_id,
                  GSTK_CH_STATUS_LINK_DROPPED);
          }
          return ESTK_SUCCESS;        
      } /* switch */
    }
  }
  return ESTK_ERROR;
} /*estk_perform_net_disconnect*/

/*===========================================================================

FUNCTION: estk_process_net_cb_cmd

DESCRIPTION :
  This function processes the net callback result in ESTK context

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_net_cb_cmd(
  const estk_cmd_type *cmd_ptr)
{
  gstk_open_ch_req_type              *open_ch_cmd  = NULL;
  sint15							 dss_nethandle = cmd_ptr->cmd_data.net_cb_info.dss_nethandle;
  sint15							 error_num = cmd_ptr->cmd_data.net_cb_info.error_num;
  estk_result_enum_type              result = ESTK_SUCCESS;
  uint8                              index         = cmd_ptr->cmd_data.net_cb_info.user_data.ch_index;
  boolean                is_v6_cb   = FALSE;
  boolean                send_tr    = FALSE;

  UIM_MSG_HIGH_0("estk_process_net_cb_cmd");
  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);
  open_ch_cmd = &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req;

  if(cmd_ptr->cmd_data.net_cb_info.user_data.address_family == DSS_AF_INET)
  {
    estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle   = dss_nethandle;
    estk_curr_inst_ptr->bip_channel_table[index].ds_netcb_status_v4 = error_num;
  }
  else if(cmd_ptr->cmd_data.net_cb_info.user_data.address_family == DSS_AF_INET6)
  {
    is_v6_cb = TRUE;
    estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle_v6 = dss_nethandle;
    estk_curr_inst_ptr->bip_channel_table[index].ds_netcb_status_v6 = error_num;
  }
  UIM_MSG_HIGH_2("estk_process_net_cb_cmd: is_v6_cb = %d, error_num = %d",
                   is_v6_cb, error_num);
  
  if (open_ch_cmd != NULL && open_ch_cmd->dns_requested)
  {
    /* v4 and v6 */
    if (is_v6_cb)
    {
      /* got v6 callback, ready to send TR */
      send_tr = TRUE;
    }
    else if (estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle_v6 == DSS_ERROR)
    {
      /* got v4 callback, bring up v6 iface */
      UIM_MSG_HIGH_0("estk_process_net_cb_cmd: Bring up IPv6 iface");
      result = estk_bip_bringup_call(index, DSS_AF_INET6);
      /*
      If ESTK cannot bring up IPV6 iface, then it needs to always send DNS TR because:
      1) If IPV4 bring up succeeded - send the IPV4 DNS addresses to card
      2) If IPV4 bring up failed - send an error TR. This is guaranteed
         because both v4 and v6 nethandle is DSS_ERROR
      */
      if (result == ESTK_ERROR)
      {
        send_tr = TRUE;
      }
    }
      
    if (send_tr)
    {
      UIM_MSG_HIGH_0("estk_process_net_cb_cmd: Send OPEN CH TR for DNS addresses");
      result = estk_send_dns_tr(index);
    }
  }
  else
  {
    UIM_MSG_HIGH_0("estk_process_net_cb_cmd: No DNS requested by card");
    switch(error_num)
    {
    case DS_ENETISCONN:
      result = estk_perform_net_connect(dss_nethandle);
      break;
    case DS_ENETNONET:
      result = estk_perform_net_disconnect(dss_nethandle);
      break;
    default:
      UIM_MSG_HIGH_1("Network CB returns Network not connected %d", error_num);
      result = ESTK_SUCCESS;
      break;
  }
  }
  return result;
} /* estk_process_net_cb_cmd */

/*===========================================================================

FUNCTION: estk_bip_setup_data_filter

DESCRIPTION :
  This function sets up a data filter

PARAMETERS:
  index : [input] Index of BIP channel table

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_bip_setup_data_filter(int32 index)
{
  struct ps_sockaddr_storage  local_addr;
  struct ps_sockaddr_in      *p_inaddr_v4    = NULL;
  struct ps_sockaddr_in6     *p_inaddr_v6    = NULL;
  struct ps_sockaddr_in      *p_dest_addr_v4 = NULL;
  struct ps_sockaddr_in6     *p_dest_addr_v6 = NULL;
  sint31                      dss_status     = DSS_ERROR;
  sint15                      error_num      = DSS_ERROR;
  uint16                      sock_addr_len  = 0;
  int                         ret            = DSS_ERROR;
  uint32                      opt_len        = 4;         /* DSS requires 4 */
  ip_filter_type              filters[1];
  uint32                      filter_handle  = 0;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if((index < 0) || (index >= ESTK_MAX_BIP_CH))
  {
    UIM_MSG_ERR_1("Invalid ch idx: 0x%x", index);
    return ESTK_ERROR;
  }

  UIM_MSG_HIGH_2("estk_bip_setup_data_filter: index=0x%x, dest_addr=0x%x",
                 index, estk_curr_inst_ptr->bip_channel_table[index].dest_addr);

  if (estk_curr_inst_ptr->bip_channel_table[index].dest_addr == NULL)
  {
    return ESTK_ERROR;
  }

  p_dest_addr_v4 =
    (struct ps_sockaddr_in*)estk_curr_inst_ptr->bip_channel_table[index].dest_addr;
  p_dest_addr_v6 =
    (struct ps_sockaddr_in6*)estk_curr_inst_ptr->bip_channel_table[index].dest_addr;

  (void)gstk_memcpy(&local_addr,
                 estk_curr_inst_ptr->bip_channel_table[index].dest_addr,
                 sizeof(struct ps_sockaddr_storage),
                 sizeof(struct ps_sockaddr_storage),
                 sizeof(struct ps_sockaddr_storage));

  p_inaddr_v4 = (struct ps_sockaddr_in*)&local_addr;
  p_inaddr_v6 = (struct ps_sockaddr_in6*)&local_addr;

  switch(p_inaddr_v4->ps_sin_family)
  {
  case DSS_AF_INET:
    p_inaddr_v4->ps_sin_addr.ps_s_addr = dss_htonl(PS_INADDR_ANY);
    sock_addr_len = sizeof(struct ps_sockaddr_in);
    break;
  case DSS_AF_INET6:
    p_inaddr_v6->ps_sin6_addr = ps_in6addr_any;
    sock_addr_len = sizeof(struct ps_sockaddr_in6);
    break;
  default:
    UIM_MSG_ERR_1("invalid ps_sin_family 0x%x!", p_inaddr_v4->ps_sin_family);
    return ESTK_ERROR;
  }

  /*
    Use ephemeral port - when port number 0 is passed,
    DS assigns a random port number from the port range
    reserved for modem
  */
  p_inaddr_v4->ps_sin_port = 0;

  /* Bind port to the dss_socket */
  dss_status = dss_bind(estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                        (struct ps_sockaddr *)&local_addr,
                        sock_addr_len,
                        &error_num);

  if(dss_status != DSS_SUCCESS)
  {
    UIM_MSG_ERR_1("dss_bind failed with error_num %d", error_num);
    return ESTK_ERROR;
  }

  /* Get dss_socket2's port. Used to populate IP filter */
  ret = dss_getsockname(estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                        (struct ps_sockaddr *)&local_addr,
                        &sock_addr_len,
                        &error_num);

  if(dss_status != DSS_SUCCESS)
  {
    UIM_MSG_ERR_1("dss_getsockname failed with error_num %d", error_num);
    return ESTK_ERROR;
  }
  else
  {
    UIM_MSG_HIGH_3("Local addr family=%d, port=%d, addr=0x%x",
                   p_inaddr_v4->ps_sin_family,
                   p_inaddr_v4->ps_sin_port,
                   p_inaddr_v4->ps_sin_addr.ps_s_addr);
  }

  /* Cache ephemeral port number from DS in host format */
  estk_curr_inst_ptr->bip_channel_table[index].me_port_num =
    dss_ntohs(p_inaddr_v4->ps_sin_port);

  /* IP Filter setup is not required for WLAN, hence return */
  if(estk_curr_inst_ptr->bip_channel_table[index].nw_type == ESTK_NETWORK_WLAN)
  {
   return ESTK_SUCCESS;
  }

  /*-------------------------------------------------------------------------
    Populate filter using destination machine's IP address as source address,
    destination socket's port as source port, and dss_socket2's port as destination
    port
  -------------------------------------------------------------------------*/
  switch(p_inaddr_v4->ps_sin_family)
  {
    case DSS_AF_INET:
      filters[0].ip_vsn = IPV4_ADDR;
      filters[0].ip_hdr.v4.field_mask = (IPFLTR_MASK_IP4_SRC_ADDR |
                                          IPFLTR_MASK_IP4_NEXT_HDR_PROT);
      filters[0].ip_hdr.v4.src.addr.ps_s_addr =
        p_dest_addr_v4->ps_sin_addr.ps_s_addr;
      filters[0].ip_hdr.v4.src.subnet_mask.ps_s_addr = dss_htonl(0xFFFFFFFF);
      switch(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol)
      {
        case GSTK_PKT_DATA_TCP:
          filters[0].ip_hdr.v4.next_hdr_prot = PS_IPPROTO_TCP;
          break;
        case GSTK_PKT_DATA_UDP:
          filters[0].ip_hdr.v4.next_hdr_prot = PS_IPPROTO_UDP;
          break;
        default:
          filters[0].ip_hdr.v4.next_hdr_prot = PS_IPPROTO_TCP_UDP;
          break;
      }
      break;
    case DSS_AF_INET6:
      filters[0].ip_vsn = IPV6_ADDR;
      filters[0].ip_hdr.v6.field_mask =
        IPFLTR_MASK_IP6_SRC_ADDR | IPFLTR_MASK_IP6_NEXT_HDR_PROT;
      filters[0].ip_hdr.v6.src.addr = p_dest_addr_v6->ps_sin6_addr;
      filters[0].ip_hdr.v6.src.prefix_len = 128;
      switch(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol)
      {
        case GSTK_PKT_DATA_TCP:
          filters[0].ip_hdr.v6.next_hdr_prot = PS_IPPROTO_TCP;
          break;
        case GSTK_PKT_DATA_UDP:
          filters[0].ip_hdr.v6.next_hdr_prot = PS_IPPROTO_UDP;
          break;
        default:
          filters[0].ip_hdr.v6.next_hdr_prot = PS_IPPROTO_TCP_UDP;
          break;
      }
      break;
    default:
      UIM_MSG_ERR_1("invalid ps_sin_family 0x%x!", p_inaddr_v4->ps_sin_family);
      return ESTK_ERROR;
  }
  if(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol == GSTK_PKT_DATA_TCP)
  {
    filters[0].next_prot_hdr.tcp.field_mask = IPFLTR_MASK_TCP_DST_PORT;
    filters[0].next_prot_hdr.tcp.src.port =
      dss_htons(estk_curr_inst_ptr->bip_channel_table[index].port_number);
    filters[0].next_prot_hdr.tcp.src.range = 0;
    filters[0].next_prot_hdr.tcp.dst.port = p_inaddr_v4->ps_sin_port;
    filters[0].next_prot_hdr.tcp.dst.range = 0;
  }
  else if(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol == GSTK_PKT_DATA_UDP)
  {
    filters[0].next_prot_hdr.udp.field_mask = IPFLTR_MASK_UDP_DST_PORT;
    filters[0].next_prot_hdr.udp.src.port =
      dss_htons(estk_curr_inst_ptr->bip_channel_table[index].port_number);
    filters[0].next_prot_hdr.udp.src.range = 0;
    filters[0].next_prot_hdr.udp.dst.port = p_inaddr_v4->ps_sin_port;
    filters[0].next_prot_hdr.udp.dst.range = 0;
  }

  /*Register socket's based filter*/
  ret = dss_reg_ip_filter(estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                          1,
                          filters,
                          &filter_handle,
                          &error_num);

  if(ret != DSS_SUCCESS)
  {
    UIM_MSG_ERR_1("dss_reg_ip_filter failed with error_num %d", error_num);
    return ESTK_ERROR;
  }
  
  estk_curr_inst_ptr->bip_channel_table[index].filter_handle = filter_handle;

  return ESTK_SUCCESS;
} /*estk_bip_setup_data_filter*/

/*===========================================================================

FUNCTION: estk_bip_copy_pdn_qos_parameters

DESCRIPTION :
  This function copies the updated PDN QOS parameters from Data Services

PARAMETERS:
  ds_pdn_qos_ptr : [Input] QoS parameters received from DS
  index          : [Input] Index of BIP channel table

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_copy_pdn_qos_parameters(
  ds_pdn_ioctl_qos_get_granted_flow_spec2_type            *ds_pdn_qos_ptr,
  uint32                                                  index)
{
  UIM_MSG_HIGH_2("estk_bip_copy_pdn_qos_parameters: ds_pdn_qos=0x%x, index=0x%x",
                 ds_pdn_qos_ptr, index);

  ESTK_RETURN_IF_NULL_PTR(estk_curr_inst_ptr);

  if(ds_pdn_qos_ptr == NULL)
  {
    return;
  }

  if (index >= ESTK_MAX_BIP_CH)
  {
    /*invalid ch idx 0x%x!*/
    return;
  }

  switch(estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_type)
  {
  case GSTK_EUTRAN_EXT_PARAM:
    estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_params.eutran_ext_bearer.traffic_class
      =  (uint8)ds_pdn_qos_ptr->rx_flow.trf_class;
    estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_params.eutran_ext_bearer.max_bitrate_ul
      =  (uint32)ds_pdn_qos_ptr->tx_flow.data_rate.format.min_max.max_rate;
    estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_params.eutran_ext_bearer.max_bitrate_dl
      =  (uint32)ds_pdn_qos_ptr->rx_flow.data_rate.format.min_max.max_rate;
    estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_params.eutran_ext_bearer.guaranteed_bitrate_ul
      =  (uint32)ds_pdn_qos_ptr->tx_flow.data_rate.format.min_max.guaranteed_rate;
    estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_params.eutran_ext_bearer.guaranteed_bitrate_dl
      =  (uint32)ds_pdn_qos_ptr->rx_flow.data_rate.format.min_max.guaranteed_rate;
    break;
  case GSTK_EUTRAN_MAPPED_UTRAN_PS:
    estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_params.eutran_mapped_utran_ps_bearer.qci
      = (uint8)ds_pdn_qos_ptr->rx_flow.lte_params.lte_qci;
    estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_params.eutran_mapped_utran_ps_bearer.max_bitrate_ul
      = (uint8)ds_pdn_qos_ptr->tx_flow.data_rate.format.min_max.max_rate;
    estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_params.eutran_mapped_utran_ps_bearer.max_bitrate_dl
      = (uint8)ds_pdn_qos_ptr->rx_flow.data_rate.format.min_max.max_rate;
    estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_params.eutran_mapped_utran_ps_bearer.guaranteed_bitrate_ul
      =  (uint8)ds_pdn_qos_ptr->tx_flow.data_rate.format.min_max.guaranteed_rate;
    estk_curr_inst_ptr->bip_channel_table[index].bearer_desc.bearer_params.eutran_mapped_utran_ps_bearer.guaranteed_bitrate_dl
      =  (uint8)ds_pdn_qos_ptr->rx_flow.data_rate.format.min_max.guaranteed_rate;
    break;
  default:
    break;
  }
} /* estk_bip_copy_pdn_qos_parameters */

/*===========================================================================

FUNCTION: estk_perform_net_connect

DESCRIPTION :
  This function established the link

PARAMETERS:
  dss_nethandle : [Input] Nethandle from DS

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_perform_net_connect(sint15 dss_nethandle)
{
  struct ps_sockaddr_in                                  *p_dest_addr_v4 = NULL;
  struct ps_sockaddr_in6                                 *p_dest_addr_v6 = NULL;
  uint16                                                 addr_size;
  sint31                                                 dss_status      = DSS_ERROR;
  sint15                                                 error_num       = DSS_ERROR;
  int32                                                  index           = -1;
  sint15                                                 sock_fd         = DSS_ERROR;
  gstk_general_result_enum_type                          gen_result      = GSTK_GENERAL_RESULT_MAX;
  gstk_terminal_rsp_extra_param_type                     tr_extra_param;
  gstk_additional_info_ptr_type                          additional_info;
  uint8                                                  addi_res;
  ds_pdn_ioctl_qos_get_granted_flow_spec2_type           ds_pdn_qos;
  uint32                                                 cmd_ref_id      = 0;
  estk_result_enum_type                                  estk_status     = ESTK_ERROR;
  uint8				                         client_type     = DS_FWK_CLIENT_SOCKETS;
  sint15                                                 client_handle   = 0;

/* Call dss_netstatus to find the current network status */

  UIM_MSG_HIGH_0("estk_perform_net_connect");

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));
  memset(&additional_info, 0x00, sizeof(additional_info));
  memset(&ds_pdn_qos, 0x00, sizeof(ds_pdn_qos));

  for(index = 0; index < ESTK_MAX_BIP_CH; index++)
  {
    p_dest_addr_v4 =
      (struct ps_sockaddr_in*)estk_curr_inst_ptr->bip_channel_table[index].dest_addr;
    p_dest_addr_v6 =
      (struct ps_sockaddr_in6*)estk_curr_inst_ptr->bip_channel_table[index].dest_addr;

    if(estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_PENDING)
    {
      UIM_MSG_HIGH_1("Found channel %d in ESTK_CH_PENDING", index);

      /* save iface_id and set scope_id for IPv6 */
      if (p_dest_addr_v6 == NULL)
      {
        UIM_MSG_ERR_0("invalid dest addr!");
        /* Continue looping through the channel table, since OPEN CH DNS query
           could have empty destination address. Theoretically this scenario
           should not occur because card is expected to send CLOSE CH for OPEN
           CH DNS query before sending new OPEN CHs. */
        continue;
      }
	  
      client_handle = dss_nethandle;
	  estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle = dss_nethandle;
	  
        /*Get updated QOS parameters from DS for PDN%*/
      if(ds_pdn_ioctl(client_type,
                      client_handle,
                      DS_PDN_FLOW_IOCTL_QOS_GET_GRANTED_FLOW_SPEC,
                      &ds_pdn_qos,
                      &error_num))
        {
          UIM_MSG_HIGH_1("IOCTL to get QOS failed with error %d", error_num);
        }
        else
        {
          estk_bip_copy_pdn_qos_parameters(&ds_pdn_qos, index);
        }

      UIM_MSG_HIGH_1("net_handle: 0x%x",
                     estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle);

      if(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol == GSTK_PKT_DATA_TCP)
      {
        sock_fd = dss_socket(
                    estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle,
                    uint16touint8(p_dest_addr_v6->ps_sin6_family),
                    DSS_SOCK_STREAM,
                    (byte)PS_IPPROTO_TCP,
                    &error_num);
      }
      else
      {
        sock_fd = dss_socket(
                    estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle,
                    uint16touint8(p_dest_addr_v6->ps_sin6_family),
                    DSS_SOCK_DGRAM,
                    (byte)PS_IPPROTO_UDP,
                    &error_num);
      }

      /* If socket cannot be opened, send error TR to card */
      if(sock_fd == DSS_ERROR)
      {
        UIM_MSG_ERR_0("dss_socket, Invalid sock_fd");

        addi_res = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
        additional_info.additional_info_ptr = &addi_res;
        additional_info.length = 1;
        tr_extra_param.present = TRUE;
        tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;
        tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_id = 0;
        tr_extra_param.extra_param.open_ch_extra_param.ch_status.is_link_est =
          FALSE;
        tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_status_additional_info =
          GSTK_CH_STATUS_LINK_DROPPED;
        (void)memscpy(&tr_extra_param.extra_param.open_ch_extra_param.bearer_description,
               sizeof(gstk_bearer_description_type),
               &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.bearer,
               sizeof(gstk_bearer_description_type));

        tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
          estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
        cmd_ref_id = estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference;

        if (estk_curr_inst_ptr->bip_channel_table[index].current_command_id ==
            GSTK_OPEN_CH_REQ)
        {
          (void)estk_bip_clean_up_ch_table(index);

          return estk_bip_send_terminal_response(
                   cmd_ref_id,
                   GSTK_OPEN_CH_CNF,
                   GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                   &additional_info,
                   &tr_extra_param,
                   index);
        }
        else if (estk_curr_inst_ptr->bip_channel_table[index].current_command_id ==
                 GSTK_SEND_DATA_REQ)
        {
          if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
          {
            UIM_MSG_HIGH_0("Saving SEND DATA TR");
            estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
            estk_curr_inst_ptr->result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
            estk_cache_additional_info(&additional_info);
            estk_status = ESTK_SUCCESS;
          }
          else
          {
            estk_status = estk_bip_send_terminal_response(
                            cmd_ref_id,
                            GSTK_SEND_DATA_CNF,
                            GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                            &additional_info,
                            NULL,
                            index);
           }
          estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_DROPPED;

          (void)estk_bip_send_ch_status_event(
                  estk_curr_inst_ptr->bip_channel_table[index].channel_id,
                  GSTK_CH_STATUS_LINK_DROPPED);

          return estk_status;
        }
      }

      UIM_MSG_HIGH_1("dss_socket, sock_fd = %d", sock_fd);
      estk_curr_inst_ptr->bip_channel_table[index].sock_fd = sock_fd;

      if(estk_bip_setup_data_filter(index) == ESTK_ERROR)
      {
        UIM_MSG_ERR_0("estk_bip_setup_data_filter failed");
      }

      estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_OPEN_WAIT;
    }
    if(estk_curr_inst_ptr->bip_channel_table[index].status ==
       ESTK_CH_OPEN_WAIT)
    {
      UIM_MSG_HIGH_1("Found channel %d in ESTK_CH_OPEN_WAIT", index);

      if(estk_curr_inst_ptr->bip_channel_table[index].transport_protocol ==
         GSTK_PKT_DATA_TCP)
      {
        if (p_dest_addr_v4 == NULL)
        {
          UIM_MSG_ERR_0("invalid dest addr!");
          return ESTK_ERROR;
        }

        p_dest_addr_v4->ps_sin_port =
          dss_htons(estk_curr_inst_ptr->bip_channel_table[index].port_number);
        addr_size =
          (p_dest_addr_v4->ps_sin_family == DSS_AF_INET) ?
            sizeof(struct ps_sockaddr_in) : sizeof(struct ps_sockaddr_in6);

        ESTK_BIP_CHECK_SOCK_FD(estk_curr_inst_ptr->bip_channel_table[index].sock_fd);
        dss_status = dss_connect(
                       estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                       (struct ps_sockaddr *)estk_curr_inst_ptr->bip_channel_table[index].dest_addr,
                       addr_size,
                       &error_num);

        UIM_MSG_HIGH_2("dss_connect returns dss_status %d with error_num %d",
                       dss_status, error_num);

        if(dss_status == DSS_ERROR && error_num == DS_EISCONN)
        {
          UIM_MSG_HIGH_0("dss_connect succeeded for TCP");
          dss_status = DSS_SUCCESS;
        }
      }
      else
      {
        UIM_MSG_HIGH_0("UDP transport layer, omit dss_connect" );
        dss_status = DSS_SUCCESS;
      }

      switch(dss_status)
      {
      case DSS_ERROR:
        if((error_num == DS_EWOULDBLOCK) || (error_num == DS_EINPROGRESS))
        {
          ESTK_BIP_CHECK_SOCK_FD(estk_curr_inst_ptr->bip_channel_table[index].sock_fd);
          dss_status = dss_async_select(
                         estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                         DS_WRITE_EVENT | DS_CLOSE_EVENT,
                         &error_num);
          if(dss_status != DSS_ERROR)
          {
            return ESTK_SUCCESS;
          }
          else
          {
            UIM_MSG_ERR_1("dss_async_select call failed - error_num %d",
                          error_num);
            return ESTK_ERROR;
          }
        }
        else
        {
          UIM_MSG_ERR_0("dss_connect failed");
          (void)estk_bip_clean_up_ch_table(index);
          return ESTK_SUCCESS;
        }
      case DSS_SUCCESS:
        estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_ACTIVE;
        ESTK_BIP_CHECK_SOCK_FD(estk_curr_inst_ptr->bip_channel_table[index].sock_fd);
        dss_status = dss_async_select(estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                                      DS_READ_EVENT | DS_WRITE_EVENT | DS_CLOSE_EVENT,
                                      &error_num);

        if(dss_status == DSS_ERROR)
        {
          UIM_MSG_HIGH_0("dss_async_select failed");
          return ESTK_ERROR;
        }

        if(estk_curr_inst_ptr->bip_channel_table[index].current_command_id ==
           GSTK_OPEN_CH_REQ)
        {
          /*Send success terminal response*/
          gen_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
          if(estk_curr_inst_ptr->bip_channel_table[index].open_ch_param_modified)
          {
            gen_result = GSTK_COMMAND_PERFORMED_WITH_MODIFICATION;
          }

          tr_extra_param.present = TRUE;
          tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;
          tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_id =
            estk_curr_inst_ptr->bip_channel_table[index].channel_id;
          tr_extra_param.extra_param.open_ch_extra_param.ch_status.is_link_est =
            TRUE;
          tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
            estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
          (void)memscpy(&tr_extra_param.extra_param.open_ch_extra_param.bearer_description,
                 sizeof(gstk_bearer_description_type),
                 &estk_curr_inst_ptr->bip_channel_table[index].bearer_desc,
                 sizeof(gstk_bearer_description_type));

          tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
            estk_curr_inst_ptr->bip_channel_table[index].buffer_size;

          return estk_bip_send_terminal_response(
                   estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference,
                   GSTK_OPEN_CH_CNF,
                   gen_result,
                   NULL,
                   &tr_extra_param,
                   index);

        }
        else if(estk_curr_inst_ptr->bip_channel_table[index].current_command_id ==
                GSTK_SEND_DATA_REQ)
        {
          UIM_MSG_HIGH_0("Returning success because in Send Data command");
          return ESTK_SUCCESS;
        }
        break;
      default:
        UIM_MSG_ERR_1("Unhandled return value for dss_connect %d", dss_status);
        break;
      } /* end of switch*/
    }
  }
  UIM_MSG_HIGH_0("estk_perform_net_connect as no channels in OPEN_WAIT");
  return ESTK_ERROR;
} /* estk_perform_net_connect */

/*===========================================================================

FUNCTION: estk_bip_process_apn

DESCRIPTION :
  This function checks if profile exists with APN provided by card

PARAMETERS:
  index : Channel index

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_bip_process_apn(
  uint32 index)
{
  ds_profile_status_etype     ds_status    = DS_PROFILE_RESULT_FAIL;  
  uint8                       i            = 0;
  estk_result_enum_type       estk_status  = ESTK_ERROR;
  ds_profile_ident_info       profile_ident;
  ds_profile_type_s           payload;
   
  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if (index >= ESTK_MAX_BIP_CH || 
      estk_curr_inst_ptr->bip_channel_table[index].apn_len == 0)
  {
    return ESTK_ERROR;
  }

  memset(&profile_ident, 0x00, sizeof(profile_ident));
  
  if(estk_curr_inst_ptr->bip_channel_table[index].apn_len > DS_PROFILE_APN_STRING_LEN)
  {
    return ESTK_ERROR;
  }
  if(gstk_memcpy(
       profile_ident.apn, 
       estk_curr_inst_ptr->bip_channel_table[index].apn,
       estk_curr_inst_ptr->bip_channel_table[index].apn_len,
       DS_PROFILE_APN_STRING_LEN,
       DS_PROFILE_APN_STRING_LEN) <
       estk_curr_inst_ptr->bip_channel_table[index].apn_len)
  {
    return ESTK_ERROR;
  }
    	   
  profile_ident.apn[estk_curr_inst_ptr->bip_channel_table[index].apn_len] = '\0';
  
  /* Allow PDP session either on  IPV4 or IPV6 */
  profile_ident.pdp_type = DS_PROFILE_PDP_IPV4V6;
  
  for (i = 0; i < estk_curr_inst_ptr->bip_channel_table[index].apn_len; ++i)
  {
    UIM_MSG_HIGH_3("APN[%d] = 0x%x %c", i,
                  profile_ident.apn[i],
                  profile_ident.apn[i]);
  }
  
  /*BIP will query if DS profile corresponding to this APN and IP type is present. 
     ds_profile_get_param - API provided to find whether DS has existing profile matching with the APN.
  "   Both APN and IP Type Matches :  DS_PROFILE_RESULT_SUCCESS is returned
  "   APN Name matches and IP Type fail:   DS_PROFILE_RESULT_ERR_INVAL_IDENT 
  "   APN Name mismatch and IP Type Mismatch : DS_PROFILE_RESULT_ERR_INVAL_IDENT */
  memset(&payload, 0x00, sizeof(payload));
  ds_status = ds_profile_get_param(profile_ident, &payload);
  
  UIM_MSG_HIGH_1("ds_profile_get_param: ds_status=0%d",ds_status);
  /* When DS_PROFILE_RESULT_SUCCESS is returned, it means DS has existing profile with 
   required APN and IP type. Hence, we do not create any profile - use existig one */
  if(ds_status == DS_PROFILE_RESULT_SUCCESS)
  {
    estk_status = ESTK_SUCCESS;
  }
  else if(ds_status == DS_PROFILE_RESULT_ERR_INVAL_IDENT)
  {
    gstk_memcpy(payload.context.apn, 
               estk_curr_inst_ptr->bip_channel_table[index].apn,
               estk_curr_inst_ptr->bip_channel_table[index].apn_len,
               estk_curr_inst_ptr->bip_channel_table[index].apn_len,
               estk_curr_inst_ptr->bip_channel_table[index].apn_len);
    
    payload.is_non_persistent = TRUE;

    ds_status = ds_profile_create(profile_ident, DS_PROFILE_APN_NAME, &payload);
    UIM_MSG_HIGH_1("ds_profile_create: ds_status=0%d",ds_status);

    if(ds_status != DS_PROFILE_RESULT_SUCCESS)
    {
      estk_status = ESTK_ERROR;
    }
    else
    {
      estk_status = ESTK_SUCCESS;
    }
  }
  return estk_status;
} /* estk_bip_process_apn */

/*===========================================================================

FUNCTION: estk_bip_process_null_apn

DESCRIPTION :
  When OPEN CHANNEL does not provide an APN, this function gets the APN to
  use and sets in the BIP channel table, and the DS profile to use
  (in some usecases like use of default DS profile or DS profile 0xFE)

PARAMETERS:
  index:                [Input]  Index of the active channel

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR
    ESTK_BAD_INPUT_PARAM

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_bip_process_null_apn(
  uint32              index
)
{
  gstk_nv_items_data_type  nv_data                     = {0};
  boolean                  disable_default_profile_use = FALSE;
  uint8                    i                           = 0;
  ds_profile_type_s        *profile_ptr                = NULL;
  ds_profile_status_etype  ds_status                   = DS_PROFILE_RESULT_FAIL;
  char                     always_share_apn[]          = "always_share";
  ds_profile_type_s        payload;
  ds_profile_ident_info    profile_ident;
  estk_result_enum_type    estk_status                 = ESTK_ERROR;

  UIM_MSG_HIGH_1("estk_bip_process_null_apn: index=0%d",index);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  memset(&profile_ident, 0x00, sizeof(profile_ident));
  
  if (index >= ESTK_MAX_BIP_CH ||
      estk_curr_inst_ptr->bip_channel_table[index].apn_len != 0)
  {
    UIM_MSG_ERR_2("Index: 0x%x APN length non-zero: 0x%x", index,
                  estk_curr_inst_ptr->bip_channel_table[index].apn_len);
    return ESTK_ERROR;
  }  
  
  (void)gstk_nv_access_read(GSTK_NV_EXT_ME_GCF_GSTK,
                            estk_curr_inst_ptr->slot_id,
                            &nv_data);

  /* Check if use of default DS 3GPP profile is disabled */
  disable_default_profile_use = gstk_nv_get_feature_2_status(
                                  GSTK_CFG_FEATURE_2_SLOT_DISABLE_BIP_DEFAULT_DS_PROFILE_USE,
                                  estk_curr_inst_ptr->slot_id);

  /* Try and retrieve the APN/profile to use in the following order:
     1. Use NV 73530 if it has valid APN, else
     2. Use always ON APN if its valid and allowed to be used, else - not present on this PL
     3. Use APN from default DS profile if its valid and allowed to be used, else
     4. Use DS sharing profile: always_on*/
  for(i = (uint8)ESTK_BIP_APN_NV_73530;
      i <= (uint8)ESTK_BIP_APN_DS_SHARING;
      i++)
  {
    switch(i)
    {
      case ESTK_BIP_APN_NV_73530:
        /* NV 73530 cannot be used for GCF usecase */
        if(nv_data.gcf_gstk)
        {
          continue;
        }
        /* If valid APN cannot be retireved from NV 73530, try other methods */
        if(estk_bip_set_default_apn_from_efs(index) < 0)
        {
          continue;
        }
        else
        {
          /* Create a Profile with Valid APN found */
		  /*BIP will query if DS profile corresponding to this APN and IP type is present. 
             ds_profile_get_param - API provided to find whether DS has existing profile matching with the APN.
          "   Both APN and IP Type Matches :  DS_PROFILE_RESULT_SUCCESS is returned
          "   APN Name matches and IP Type fail:   DS_PROFILE_RESULT_ERR_INVAL_IDENT 
          "   APN Name mismatch and IP Type Mismatch : DS_PROFILE_RESULT_ERR_INVAL_IDENT */
          memset(&payload, 0x00, sizeof(payload));
          
          if(estk_curr_inst_ptr->bip_channel_table[index].apn_len > DS_PROFILE_APN_STRING_LEN)
          {
            return ESTK_ERROR;
          }
          if(gstk_memcpy(
               profile_ident.apn, 
               estk_curr_inst_ptr->bip_channel_table[index].apn,
               estk_curr_inst_ptr->bip_channel_table[index].apn_len,
               DS_PROFILE_APN_STRING_LEN,
               DS_PROFILE_APN_STRING_LEN) <
               estk_curr_inst_ptr->bip_channel_table[index].apn_len)
          {
            return ESTK_ERROR;
          }

          profile_ident.apn[estk_curr_inst_ptr->bip_channel_table[index].apn_len] = '\0';
          ds_status = ds_profile_get_param(profile_ident, &payload);
          
          UIM_MSG_HIGH_1("ds_profile_get_param: ds_status=0%d",ds_status);
          /* When DS_PROFILE_RESULT_SUCCESS is returned, it means DS has existing profile with 
           required APN and IP type. Hence, we do not create any profile - use existig one */
          if(ds_status == DS_PROFILE_RESULT_SUCCESS)
          {
            estk_status = ESTK_SUCCESS;
          }
          else if(ds_status == DS_PROFILE_RESULT_ERR_INVAL_IDENT)
          {
            gstk_memcpy(payload.context.apn, 
                       estk_curr_inst_ptr->bip_channel_table[index].apn,
                       estk_curr_inst_ptr->bip_channel_table[index].apn_len,
                       estk_curr_inst_ptr->bip_channel_table[index].apn_len,
                       estk_curr_inst_ptr->bip_channel_table[index].apn_len);
            
            payload.is_non_persistent = TRUE;
        
            ds_status = ds_profile_create(profile_ident, DS_PROFILE_APN_NAME, &payload);
            UIM_MSG_HIGH_1("ds_profile_create: ds_status=0%d",ds_status);
        
            if(ds_status != DS_PROFILE_RESULT_SUCCESS)
            {
              estk_status = ESTK_ERROR;
            }
            else
            {
              estk_status = ESTK_SUCCESS;
            }
          }
          return estk_status;
        }
        break;
      case ESTK_BIP_APN_DEFAULT_PROFILE:
         /* Default DS profile cannot be used if its not allowed per NV config */
        if(disable_default_profile_use == TRUE)
        {
          continue;
        }
        /* If valid APN cannot be retrieved from default
         profile, try other methods */
        profile_ptr = (ds_profile_type_s*)GSTK_CALLOC(sizeof(ds_profile_type_s));
        if(profile_ptr == NULL)
        {
          return ESTK_ERROR;
        }

        ds_status = ds_profile_get_default_profile_setting(profile_ptr);
        if(ds_status != DSS_SUCCESS)
        {
          UIM_MSG_ERR_0("ds_profile_get_default_profile_setting FAILED");
          gstk_free(profile_ptr);
          continue;
        }
        else
        {
          UIM_MSG_HIGH_1("APN Length is 0x%x", strlen(profile_ptr->context.apn));
          if(strlen(profile_ptr->context.apn) > 0)
          {          
            estk_curr_inst_ptr->bip_channel_table[index].apn = 
                       (uint8 *)GSTK_CALLOC(strlen(profile_ptr->context.apn) * sizeof(uint8));
            if(estk_curr_inst_ptr->bip_channel_table[index].apn == NULL)
            {
              return ESTK_ERROR;
            }
            estk_curr_inst_ptr->bip_channel_table[index].apn_len = strlen(profile_ptr->context.apn);
            gstk_memcpy(estk_curr_inst_ptr->bip_channel_table[index].apn, 
                      profile_ptr->context.apn,
                      estk_curr_inst_ptr->bip_channel_table[index].apn_len,
                      estk_curr_inst_ptr->bip_channel_table[index].apn_len,
                      strlen(profile_ptr->context.apn));
          }
          else
          {
            gstk_free(profile_ptr);
            profile_ptr = NULL;
            continue;
          }
          gstk_free(profile_ptr);
          profile_ptr = NULL;
          return ESTK_SUCCESS;
        }
        break;
      case ESTK_BIP_APN_DS_SHARING:
        if(estk_curr_inst_ptr->bip_channel_table[index].apn != NULL)
        {
          gstk_free(estk_curr_inst_ptr->bip_channel_table[index].apn);
          estk_curr_inst_ptr->bip_channel_table[index].apn = NULL;
          estk_curr_inst_ptr->bip_channel_table[index].apn_len = 0;
        }
        estk_curr_inst_ptr->bip_channel_table[index].apn = (uint8 *)GSTK_CALLOC(strlen(always_share_apn));
        if(estk_curr_inst_ptr->bip_channel_table[index].apn != NULL)
        {
          estk_curr_inst_ptr->bip_channel_table[index].apn_len = strlen(always_share_apn);
          if(gstk_memcpy(estk_curr_inst_ptr->bip_channel_table[index].apn,
                          always_share_apn,
                          strlen(always_share_apn),
                          estk_curr_inst_ptr->bip_channel_table[index].apn_len,
                          strlen(always_share_apn)) < strlen(always_share_apn))
          {
            gstk_free(estk_curr_inst_ptr->bip_channel_table[index].apn);
            estk_curr_inst_ptr->bip_channel_table[index].apn = NULL;
            estk_curr_inst_ptr->bip_channel_table[index].apn_len = 0;
            return ESTK_ERROR;
          }
        }
        else
        {
          return ESTK_ERROR;
        }
        return ESTK_SUCCESS;
        break;
      default:
        break;
    }
  }
  return ESTK_SUCCESS;
} /* estk_bip_process_null_apn */


/*===========================================================================

FUNCTION: estk_bip_bringup_call

DESCRIPTION :
  This function brings up call

PARAMETERS:
  index : Channel index
  address_family: DSS_AF_INET or DSS_AF_INET6

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_bip_bringup_call(
  uint32 index,
  uint16 address_family)
{
  sint15                      error_num             = DSS_ERROR;
  dss_sock_cb_fcn             sock_cb               = estk_bip_socket_cb;
  dss_net_cb_fcn              net_cb                = estk_bip_network_cb;
  gstk_open_ch_req_type      *open_ch_cmd           = NULL;
  gstk_general_result_enum_type      
                              gen_result            = GSTK_GENERAL_RESULT_MAX;
  dss_net_lib_info_type_s     net_params;
  estk_result_enum_type       estk_status           = ESTK_ERROR;
  sint15                      app_id                = 0;                
  estk_bip_net_cb_user_data  *net_cb_user_data_ptr  = NULL;
  estk_bip_sock_cb_user_data *sock_cb_user_data_ptr = NULL;
  boolean                     perform_net_connect   = 0;
  gstk_terminal_rsp_extra_param_type 
                              tr_extra_param;
  
  UIM_MSG_HIGH_2("estk_bip_bringup_call: index=0%d, address_family=%d",
                 index, address_family);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if (index >= ESTK_MAX_BIP_CH)
  {
    return ESTK_ERROR;
  }

  if (estk_curr_inst_ptr->keep_ch_open &&
        estk_curr_inst_ptr->ch_open_id < ESTK_MAX_BIP_CH &&
        index == estk_curr_inst_ptr->ch_open_id)
    {
      UIM_MSG_HIGH_0("estk_bip_establish_link: CHANNEL reuse");
      /* Reusing previous CHANNEL */
      if (address_family == DSS_AF_INET)
      {
        if (estk_curr_inst_ptr->bip_channel_table[index].ds_netcb_status_v4 != DS_ENETISCONN)
        {
          UIM_MSG_ERR_0("estk_bip_establish_link: IPv4 interface status != DS_ENETISCONN");
          return ESTK_ERROR;
        }
  
        perform_net_connect = TRUE;
      }
      else if (address_family == DSS_AF_INET6 )
      {
        if (estk_curr_inst_ptr->bip_channel_table[index].ds_netcb_status_v6 != DS_ENETISCONN)
        {
          UIM_MSG_ERR_0("estk_bip_establish_link: IPv6 interface status != DS_ENETISCONN");
          return ESTK_ERROR;
        }
 
        perform_net_connect = TRUE;
      }
      else
      {
        /*
          If profile was set to IPV4 only, and we don't have a valid v4 nethandle, then
          we must return error
  
          If profile was NOT set to IPV4 only, and we don't have a valid v4 nethandle, then
          we must attempt to bring up v4 nethandle
  
          If profile was NOT set as IPV4 only, and we don't have a valid v6 nethandle, then
          we must attempt to bring up v6 nethandle*/
        
        
          if (address_family == DSS_AF_INET &&
              estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle_v6 != DSS_ERROR)
          {
            UIM_MSG_HIGH_0("Missing IPV4 net handle");
          }
          else if (address_family == DSS_AF_INET6 &&
              estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle != DSS_ERROR)
          {
            UIM_MSG_HIGH_0("Missing IPV6 net handle");
          }
          else
          {
            UIM_MSG_ERR_2("invalid nethandles %d, %d",
                          estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle,
                          estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle_v6);
            return ESTK_ERROR;
          }
          UIM_MSG_HIGH_1("Attempting to bring up address_family = 0x%x nethandle", address_family);
          return estk_bip_bringup_call(index, address_family);
      }
    }
  
    if (perform_net_connect)
    {
      if(estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_ACTIVE)    
      {      
        return estk_perform_net_connect(estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle);    
      }    
      else    
      {
        if(estk_curr_inst_ptr->bip_channel_table[index].current_command_id == GSTK_OPEN_CH_REQ)
        {
          /*Send success terminal response*/
          gen_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
          if(estk_curr_inst_ptr->bip_channel_table[index].open_ch_param_modified)
          {
            gen_result = GSTK_COMMAND_PERFORMED_WITH_MODIFICATION;
          }
  
          tr_extra_param.present = TRUE;
          tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;
          tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_id =
            estk_curr_inst_ptr->bip_channel_table[index].channel_id;
          tr_extra_param.extra_param.open_ch_extra_param.ch_status.is_link_est =
            TRUE;
          tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
            estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
          UIM_MSG_HIGH_0("Copying bearer param for terminal response");
          (void)memscpy(&tr_extra_param.extra_param.open_ch_extra_param.bearer_description,
                 sizeof(gstk_bearer_description_type),
                 &estk_curr_inst_ptr->bip_channel_table[index].bearer_desc,
                 sizeof(gstk_bearer_description_type));
  
          tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
            estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
  
          return estk_bip_send_terminal_response(
                   estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference,
                   GSTK_OPEN_CH_CNF,
                   gen_result,
                   NULL,
                   &tr_extra_param,
                   index);
        }
        else if(estk_curr_inst_ptr->bip_channel_table[index].current_command_id == GSTK_SEND_DATA_REQ)
        {
          UIM_MSG_HIGH_0("Returning success because in Send Data command");
          return ESTK_SUCCESS;
        }
      }
    }
    /* Not reusing previous CHANNEL */

  if(estk_curr_inst_ptr->bip_channel_table[index].apn_len != 0)
  {
    estk_status = estk_bip_process_apn(index);   
  }
  else
  {
    estk_status = estk_bip_process_null_apn(index);
  }
  
  if(estk_status == ESTK_SUCCESS)
  {
      net_cb_user_data_ptr = (estk_bip_net_cb_user_data *)GSTK_CALLOC(sizeof(estk_bip_net_cb_user_data));
      if(net_cb_user_data_ptr == NULL)
      {
        UIM_MSG_ERR_0("net_cb_user_data memory allocation failed");
        return ESTK_ERROR;
      }
      sock_cb_user_data_ptr = (estk_bip_sock_cb_user_data *)GSTK_CALLOC(sizeof(estk_bip_sock_cb_user_data));
      if(sock_cb_user_data_ptr == NULL)
      {
        UIM_MSG_ERR_0("sock_cb_user_data memory allocation failed");
        return ESTK_ERROR;
      }

    net_cb_user_data_ptr->ch_index       = index;
    net_cb_user_data_ptr->slot_id        = estk_curr_inst_ptr->slot_id;
    net_cb_user_data_ptr->address_family = address_family;
    sock_cb_user_data_ptr->slot_id       = estk_curr_inst_ptr->slot_id; 
    sock_cb_user_data_ptr->ch_index      = index;
  
    net_params.apn_type          = DS_APN_TYPE_UNSPECIFIED;
    net_params.family            = address_family;
    net_params.name              = estk_curr_inst_ptr->bip_channel_table[index].apn;
    net_params.net_cb            = net_cb;
    net_params.sock_cb           = sock_cb;
    net_params.net_cb_user_data  = net_cb_user_data_ptr;
    net_params.sock_cb_user_data = sock_cb_user_data_ptr;

    if(estk_curr_inst_ptr->bip_channel_table[index].card_has_auth_params)
    {
      if(estk_curr_inst_ptr->bip_channel_table[index].username_len != 0)
      {
        (void)gstk_memcpy(net_params.authentication_params.auth_params.username,
                       estk_curr_inst_ptr->bip_channel_table[index].username,
                       estk_curr_inst_ptr->bip_channel_table[index].username_len,
                       estk_curr_inst_ptr->bip_channel_table[index].username_len,
                       estk_curr_inst_ptr->bip_channel_table[index].username_len);
      }
    
      if(estk_curr_inst_ptr->bip_channel_table[index].passwd_len != 0)
      {  
        (void)gstk_memcpy(net_params.authentication_params.auth_params.password,
                       estk_curr_inst_ptr->bip_channel_table[index].passwd,
                       estk_curr_inst_ptr->bip_channel_table[index].passwd_len,
                       estk_curr_inst_ptr->bip_channel_table[index].passwd_len,
                       estk_curr_inst_ptr->bip_channel_table[index].passwd_len);
      }
    }

    app_id = dss_bringup_call(&net_params,&error_num);

    if (address_family == DSS_AF_INET)
    {
      estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle   = app_id;
    }
    else if (address_family == DSS_AF_INET6)
    {
      estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle_v6 = app_id;
    }

    UIM_MSG_HIGH_3("estk_bip_bringup_call: error_num=0x%x , dss_nethandle in bip channel table=0x%x, dss_nethandle_v6 in bip channel table=0x%x",
                   error_num, estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle, estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle_v6);

    open_ch_cmd = &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req;
    switch(error_num)
    {
      case DS_EWOULDBLOCK:
        return ESTK_SUCCESS;
      case DSS_SUCCESS:
        gstk_free(net_cb_user_data_ptr);
        net_cb_user_data_ptr = NULL;
        /* For CT PL, DS will never call nw call back function if dss_bringup_call returns error number as DSS_SUCCESS */
        if(estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_ACTIVE)    
        {      
          return estk_perform_net_connect(app_id);    
        }    
        else    
        {
          if(estk_curr_inst_ptr->bip_channel_table[index].current_command_id == GSTK_OPEN_CH_REQ)
          {
            /*Send success terminal response*/
            gen_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
            if(estk_curr_inst_ptr->bip_channel_table[index].open_ch_param_modified)
            {
              gen_result = GSTK_COMMAND_PERFORMED_WITH_MODIFICATION;
            }
  
            tr_extra_param.present = TRUE;
            tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;
            tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_id =
            estk_curr_inst_ptr->bip_channel_table[index].channel_id;
            tr_extra_param.extra_param.open_ch_extra_param.ch_status.is_link_est =
            TRUE;
            tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
            estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
            UIM_MSG_HIGH_0("Copying bearer param for terminal response");
            (void)memscpy(&tr_extra_param.extra_param.open_ch_extra_param.bearer_description,
                 sizeof(gstk_bearer_description_type),
                 &estk_curr_inst_ptr->bip_channel_table[index].bearer_desc,
                 sizeof(gstk_bearer_description_type));
  
            tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
            estk_curr_inst_ptr->bip_channel_table[index].buffer_size;
  
            return estk_bip_send_terminal_response(
                   estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference,
                   GSTK_OPEN_CH_CNF,
                   gen_result,
                   NULL,
                   &tr_extra_param,
                   index);
          }
          else
          {
            UIM_MSG_HIGH_0("Returning success because in Send Data command");
            return ESTK_SUCCESS;
          }
        }
        break;
      default:
        UIM_MSG_HIGH_1("estk_bip_bringup_call return unhandled return status %d",
				       error_num);
        gstk_free(net_cb_user_data_ptr);
        net_cb_user_data_ptr = NULL;
        if (open_ch_cmd != NULL && open_ch_cmd->dns_requested && address_family == DSS_AF_INET)
        {
          UIM_MSG_ERR_0("DNS query: iface v4 bringup fails, attempt iface v6...");
          return estk_bip_bringup_call(index, DSS_AF_INET6);
        }
        return ESTK_ERROR;
    }
  }
  return ESTK_ERROR;
} /* estk_bip_bringup_call */

/*===========================================================================

FUNCTION: estk_populate_address_in_open_ch_table

DESCRIPTION :
  This function populates local or destination address into the Open channel
  Table.

PARAMETERS:
  index : [Input] Index of the channel to be populated
  address_type : [Input] Local or destination address

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_populate_address_in_open_ch_table(
  int32                       index,
  estk_address_type           address_type,
  gstk_pkt_data_address_type  *pkt_data_addr_ptr)
{
  char                       *IP_address   = NULL;
  sint31                     dss_status    = DSS_ERROR;
  int16                      dss_errno     = 0;
  estk_result_enum_type      estk_status   = ESTK_ERROR;
  struct ps_sockaddr_storage **p_addr_ptr  = NULL;
  struct ps_sockaddr_in      *p_addr_v4    = NULL;
  struct ps_sockaddr_in6     *p_addr_v6    = NULL;

  UIM_MSG_HIGH_3("estk_populate_address_in_open_ch_table: index=0x%x "
                 "address_type=0x%x, "
                 "pkt_data_addr_ptr=0x%x",
                 index, address_type, pkt_data_addr_ptr);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if(pkt_data_addr_ptr == NULL)
  {
    return ESTK_SUCCESS;
  }

  if(index < 0 || index >= ESTK_MAX_BIP_CH)
  {
    return ESTK_ERROR;
  }

  if(address_type > ESTK_DESTINATION_ADDRESS)
  {
    UIM_MSG_ERR_0("Invalid address type to populate open channel table");
    return ESTK_ERROR;
  }

  if(pkt_data_addr_ptr->pkt_address.length > 0)
  {
    switch(address_type)
    {
    case ESTK_LOCAL_ADDRESS:
      p_addr_ptr = &estk_curr_inst_ptr->bip_channel_table[index].src_addr;
      break;
    case ESTK_DESTINATION_ADDRESS:
      p_addr_ptr = &estk_curr_inst_ptr->bip_channel_table[index].dest_addr;
      break;
    default:
      UIM_MSG_ERR_1("Invalid addr type 0x%x!", address_type);
      return ESTK_ERROR;
    }

    /* Free existing address as the address will be re-alloc-ed for channel reuse */
    if (p_addr_ptr != NULL && *p_addr_ptr != NULL)
    {
      gstk_free(*p_addr_ptr);
      *p_addr_ptr = NULL;
    }

    *p_addr_ptr =
        (struct ps_sockaddr_storage*)GSTK_CALLOC(sizeof(struct ps_sockaddr_storage));
    if (*p_addr_ptr == NULL)
    {
      return ESTK_ERROR;
    }
    memset(*p_addr_ptr, 0x00, sizeof(struct ps_sockaddr_storage));
    p_addr_v4 = (struct ps_sockaddr_in*)(*p_addr_ptr);
    p_addr_v6 = (struct ps_sockaddr_in6*)(*p_addr_ptr);

    if(estk_convert_to_ip_format(pkt_data_addr_ptr,
                                 &IP_address) != ESTK_SUCCESS)
    {
      UIM_MSG_ERR_0("Local IP address conversion to IPV4 format failed");
    }
    else
    {
      switch(pkt_data_addr_ptr->address_type)
      {
      case GSTK_PKT_DATA_IPV4_ADDRESS:
        p_addr_v4->ps_sin_family = DSS_AF_INET;
        dss_status = dss_inet_pton(
                       IP_address,
                       DSS_AF_INET,
                       &p_addr_v4->ps_sin_addr,
                       sizeof(struct ps_in_addr),
                       &dss_errno);
        if (dss_status != DSS_SUCCESS)
        {
          UIM_MSG_ERR_2("failed inet_pton()! 0x%x, 0x%x",
                        dss_status, dss_errno);
          estk_status = ESTK_ERROR;
        }
        else
        {
          estk_status = ESTK_SUCCESS;
        }
        break;
      case GSTK_PKT_DATA_IPV6_ADDRESS:
        p_addr_v6->ps_sin6_family = DSS_AF_INET6;
        dss_status = dss_inet_pton(
                       IP_address,
                       DSS_AF_INET6,
                       &p_addr_v6->ps_sin6_addr,
                       sizeof(struct ps_in6_addr),
                       &dss_errno);
        if (dss_status != DSS_SUCCESS)
        {
          UIM_MSG_ERR_2("failed inet_pton()! 0x%x, 0x%x",
                        dss_status, dss_errno);
          estk_status = ESTK_ERROR;
        }
        else
        {
          estk_status = ESTK_SUCCESS;
        }
        break;
      default:
        UIM_MSG_ERR_1("invalid address_type 0x%x!",
                      pkt_data_addr_ptr->address_type);
        estk_status = ESTK_ERROR;
      }

      gstk_free(IP_address);
      IP_address = NULL;
    }
  }
  else
  {
    estk_status = ESTK_SUCCESS;
  }
  return estk_status;
} /* estk_populate_address_in_open_ch_table */

/*===========================================================================

FUNCTION: estk_modify_apn_based_on_mapping_table

DESCRIPTION :
  This function modifies APN based on pre-configured APN mapping table

PARAMETERS:
  index : [Input] Index of the channel to be populated

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_modify_apn_based_on_mapping_table(int32 index)
{
  int32                    idx         = 0;
  uint32                   i           = 0;
  uint8                   *new_apn     = NULL;
  uint32                   new_apn_len = 0;
  gstk_status_enum_type    gstk_status = GSTK_ERROR;
  gstk_nv_items_data_type  nv_data;

  UIM_MSG_HIGH_1("estk_modify_apn_based_on_mapping_table: index=0x%x", index);

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if (index < 0 || index >= ESTK_MAX_BIP_CH)
  {
    UIM_MSG_ERR_1("Invalid ch idx: 0x%x", index);
    return ESTK_ERROR;
  }

  memset(&nv_data, 0x00, sizeof(nv_data));

  nv_data.apn_mapping_table_ptr =
    (gstk_apn_mapping_table_data_type*)GSTK_CALLOC(sizeof(gstk_apn_mapping_table_data_type));

  if (nv_data.apn_mapping_table_ptr == NULL)
  {
    return ESTK_ERROR;
  }

  /* Read the BIP APN mapping table */
  gstk_status = gstk_nv_access_read(GSTK_NV_SLOT_BIP_APN_MAPPING_TABLE,
                                    estk_curr_inst_ptr->slot_id,
                                    &nv_data);

  if (gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_0("estk_read_apn_mapping_table() failed, skip mapping");
    gstk_free(nv_data.apn_mapping_table_ptr);
    return ESTK_ERROR;
  }

  /*Start comparison and do APN mapping if necessary...*/
  UIM_MSG_HIGH_2("estk_curr_inst_ptr->bip_channel_table[%x].apn_len = %x ",
                 index, estk_curr_inst_ptr->bip_channel_table[index].apn_len);

  for (idx = 0; idx < ESTK_APN_MAPPING_MAX; ++ idx)
  {
    UIM_MSG_HIGH_1("APN table idx: %x ", idx);

    /* exact mapping */
    if (nv_data.apn_mapping_table_ptr->value[idx].input_apn[0] != '\0' &&
        estk_curr_inst_ptr->bip_channel_table[index].apn_len ==
        strlen(nv_data.apn_mapping_table_ptr->value[idx].input_apn) &&
        strncasecmp((char *)estk_curr_inst_ptr->bip_channel_table[index].apn,
                    nv_data.apn_mapping_table_ptr->value[idx].input_apn,
                    estk_curr_inst_ptr->bip_channel_table[index].apn_len) == 0)
    {
      new_apn_len = strlen(nv_data.apn_mapping_table_ptr->value[idx].output_apn);
      new_apn = (uint8 *)GSTK_CALLOC(new_apn_len + 1);
      if (new_apn == NULL)
      {
        gstk_free(nv_data.apn_mapping_table_ptr);
        return ESTK_ERROR;
      }
      memset(new_apn,0,new_apn_len + 1);

      if (estk_curr_inst_ptr->bip_channel_table[index].apn != NULL)
      {
        gstk_free(estk_curr_inst_ptr->bip_channel_table[index].apn);
        estk_curr_inst_ptr->bip_channel_table[index].apn_len = 0;
		estk_curr_inst_ptr->bip_channel_table[index].apn = NULL;
      }

      (void)gstk_memcpy(new_apn,
                     nv_data.apn_mapping_table_ptr->value[idx].output_apn,
                     new_apn_len,
                     new_apn_len,
                     new_apn_len);

      estk_curr_inst_ptr->bip_channel_table[index].apn = new_apn;
      estk_curr_inst_ptr->bip_channel_table[index].apn_len = new_apn_len;
      UIM_MSG_HIGH_0("*** New APN ***");
      for (i = 0; i < estk_curr_inst_ptr->bip_channel_table[index].apn_len; ++ i)
      {
        UIM_MSG_HIGH_2("APN[%d]: %c",
                       i, estk_curr_inst_ptr->bip_channel_table[index].apn[i]);
      }
      gstk_free(nv_data.apn_mapping_table_ptr);
      return ESTK_SUCCESS;
    }
  }

  gstk_free(nv_data.apn_mapping_table_ptr);
  return ESTK_SUCCESS;
} /* estk_modify_apn_based_on_mapping_table */

/*===========================================================================

FUNCTION: estk_bip_get_prio_network

DESCRIPTION :
  This function returns the network on which the BIP session should be
  attempted: Cellular or WLAN. The decision is made based on:
  - Bearer type (Default or Packet Data)
  - Destination IP address (IPv4 or IPv6)
  - If BIP over WLAN is supported (GSTK_CFG_FEATURE_ESTK_BIP_OVER_WLAN)
  - If BIP over WLAN has higher priority than Cellular
    (GSTK_CFG_FEATURE_ESTK_BIP_WLAN_PRIO)
  - Which networks are available (WLAN/Cellular)

PARAMETERS:
  nw_type_ptr : [Output] Contains the Priority network
  addi_res_ptr : [Output] Contains Additional Result for an Error TR
  slot         : [Input] Slot value
  bearer_type  : [Input] Bearer type of BIP
  address_type : [Input] Data IP address format of BIP

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_bip_get_prio_network(
  estk_network_type               *nw_type_ptr,
  uint8                           *addi_res_ptr,
  gstk_slot_id_enum_type           slot,
  gstk_bearer_code_enum_type       bearer_type,
  gstk_pkt_data_address_enum_type  address_type
)
{
  boolean                               cellular_allowed     = FALSE;
  boolean                               cellular_data_avail  = FALSE;
  gstk_status_enum_type                 gstk_status          = GSTK_SUCCESS;
  gstk_slot_id_enum_type                active_slot          = GSTK_SLOT_ID_MAX;
  const gstk_ds_system_status_type     *sys_status           = NULL;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  UIM_MSG_HIGH_3("In estk_bip_get_prio_network, slot: 0x%x, bearer_type: 0x%x, address type: 0x%x",
                 slot, bearer_type, address_type);

  if(nw_type_ptr == NULL || addi_res_ptr == NULL)
  {
    UIM_MSG_ERR_2("Null pointer for Network type 0x%x, 0x%x", nw_type_ptr, addi_res_ptr);
    return ESTK_ERROR;
  }

  if(!ESTK_IS_VALID_SLOT_ID(slot))
  {
    return ESTK_ERROR;
  }

  *nw_type_ptr = ESTK_NETWORK_NONE;
  *addi_res_ptr = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;

  if(gstk_io_ctrl(GSTK_IO_GET_DS_SYSTEM_RAT_INFO,
                  (void *)&estk_curr_inst_ptr->slot_id,
                  (void *)&sys_status) == GSTK_SUCCESS)
  {
    /* Check if any Cellular RATs are available. Preferred Celluar RAT is always available on index 0.
       If index 0 technology is cellular, then check only index 0 i.e preferred rat. If it is WLAN in index 0 then 
       check for other indices also for cellular availability. */
 
    if(sys_status->avail_sys[0].technology ==
       GSTK_DS_SYS_NETWORK_3GPP)
    {
      if(sys_status->avail_sys[0].rat_value !=
         GSTK_DS_SYS_RAT_EX_NULL_BEARER)
      {
        cellular_data_avail = TRUE;
      }
    }
  }  

  *addi_res_ptr = (uint8)GSTK_NO_SERVICE_AVAILABLE;
  gstk_status = gstk_io_ctrl(GSTK_IO_GET_ACTIVE_DATA_SUB_SLOT_ID,
                             (void *)&slot,
                             (void *)&active_slot);

  if(gstk_status == GSTK_SUCCESS)
  {
    /* If the BIP transaction is requested on the slot 
       with the active DATA subscription, BIP over
       Cellular can be allowed */
    if(slot == active_slot)
    {
       if(estk_bip_check_existing_gsm_cs_call())
       {
          *addi_res_ptr = (uint8)GSTK_ME_CURRENTLY_BUSY_ON_CALL;
       }
       else
       {
          if(cellular_data_avail == TRUE)
          {
            cellular_allowed = TRUE;
          }
       }
    }
    else
    {
       *addi_res_ptr = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    }
  }

  UIM_MSG_HIGH_1("cellular_allowed: 0x%x",
                 cellular_allowed);

  if(cellular_allowed == TRUE)
  {
    *nw_type_ptr = ESTK_NETWORK_CELLULAR;
  }
  else
  {
    return ESTK_ERROR;
  }

  UIM_MSG_HIGH_1("Chosen network for BIP: 0x%x", *nw_type_ptr);

  return ESTK_SUCCESS;
} /* estk_bip_get_prio_network */

/*===========================================================================

FUNCTION: estk_save_prev_open_ch

DESCRIPTION :
  This function saves the current OPEN CH cmd so the channel might be reused
  by the card if needed.

PARAMETERS:
  gstk_req_ptr :   [Input] Original OPEN CH cmd

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static void estk_save_prev_open_ch(const gstk_cmd_from_card_type *gstk_req_ptr)
{
  if (gstk_req_ptr == NULL)
  {
    return;
  }

  /* Save current OPEN CH for future comparison */
  /*--------------------------------------------------------------------------*/

  /* Bearer TLV */
 (void)memscpy(&(estk_curr_inst_ptr->prev_open_ch.bearer),
               sizeof(gstk_bearer_description_type),
               &(gstk_req_ptr->cmd.open_ch_pro_cmd_req.bearer),
               sizeof(gstk_bearer_description_type));

  /* APN TLV */

  if (estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet)
  {
    gstk_free(estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet);
    estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet = NULL;
    estk_curr_inst_ptr->prev_open_ch.access_point_name.length = 0;
  }

  estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet_given
    = gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.alphabet_given;

  estk_curr_inst_ptr->prev_open_ch.access_point_name.length
    = gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.length;

  if(gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.alphabet_given &&
     gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.length != 0)
  {
    estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet
      = (uint16 *)GSTK_CALLOC(((dword)gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.length) * sizeof(uint16));
    if(estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet == NULL)
    {
      return;
    }
    else
    {
      (void)memscpy(estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet,
                    (uint32)gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.length * sizeof(uint16),
                    gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.alphabet,
                    (uint32)gstk_req_ptr->cmd.open_ch_pro_cmd_req.access_point_name.length * sizeof(uint16));
    }
  }

  /* Transport Level TLV */
  (void)memscpy(&(estk_curr_inst_ptr->prev_open_ch.transport_level),
                sizeof(gstk_pkt_data_transport_level_type),
                &(gstk_req_ptr->cmd.open_ch_pro_cmd_req.transport_level),
                sizeof(gstk_pkt_data_transport_level_type));

  /* Data Destination Address TLV */
  estk_curr_inst_ptr->prev_open_ch.data_dest_address.address_type =
    gstk_req_ptr->cmd.open_ch_pro_cmd_req.data_dest_address.address_type;

  estk_curr_inst_ptr->prev_open_ch.data_dest_address.pkt_address.length =
    gstk_req_ptr->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.length;

  if(gstk_req_ptr->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.length > 0)
  {
    if (estk_curr_inst_ptr->prev_open_ch.data_dest_address.pkt_address.address)
    {
      gstk_free(estk_curr_inst_ptr->prev_open_ch.data_dest_address.pkt_address.address);
      estk_curr_inst_ptr->prev_open_ch.data_dest_address.pkt_address.address = NULL;
    }
    estk_curr_inst_ptr->prev_open_ch.data_dest_address.pkt_address.address
      = (uint8 *)GSTK_CALLOC((dword)gstk_req_ptr->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.length);
    if(estk_curr_inst_ptr->prev_open_ch.data_dest_address.pkt_address.address == NULL)
    {
      if(estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet)
      {
        gstk_free(estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet);
        estk_curr_inst_ptr->prev_open_ch.access_point_name.alphabet = NULL;
      }
      return;
    }
    else
    {
      (void)memscpy(estk_curr_inst_ptr->prev_open_ch.data_dest_address.pkt_address.address,
                    (uint32)gstk_req_ptr->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.length,
                    gstk_req_ptr->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.address,
                    (uint32)gstk_req_ptr->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.length * sizeof(uint8));
    }
  }
} /* estk_save_prev_open_ch */


/*===========================================================================

FUNCTION: estk_populate_open_ch_table

DESCRIPTION :
  This function populates information into the Open channel Table.

PARAMETERS:
  index :   [Input] Index of the channel to be populated
  nw_type : [Input] Contains the network on which Channel is being opened

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static estk_result_enum_type estk_populate_open_ch_table(
  int32             index,
  estk_network_type nw_type)
{
  gstk_open_ch_req_type   *open_ch_cmd        = NULL;
  uint32                   i                  = 0;
  boolean                  in_fallback        = FALSE;
  uint16                   address_family     = DSS_AF_UNSPEC;
  struct ps_sockaddr_in    *p_dest_addr_v4    = NULL;
  boolean                  ch_reuse           = FALSE;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  UIM_MSG_HIGH_2("estk_populate_open_ch_table: index=0x%x, slot=0x%x",
                 index, estk_curr_inst_ptr->slot_id);

  if (index < 0 || index >= ESTK_MAX_BIP_CH)
  {
    return ESTK_ERROR;
  } 

  if(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_OPEN_CH_REQ &&
     estk_curr_inst_ptr->keep_ch_open &&
     estk_curr_inst_ptr->ch_open_id < ESTK_MAX_BIP_CH &&
     index == estk_curr_inst_ptr->ch_open_id)
  {
    UIM_MSG_HIGH_0("estk_populate_open_ch_table: Attempting CHANNEL reuse");

    if (estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle == DSS_ERROR &&
        estk_curr_inst_ptr->bip_channel_table[index].dss_nethandle_v6 == DSS_ERROR)
    {
      UIM_MSG_HIGH_0("No valid net_handles - do not reuse channel");
      ch_reuse = FALSE;
    }
    else if (memcmp(&estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.bearer,
                    &estk_curr_inst_ptr->prev_open_ch.bearer,
                    sizeof(gstk_bearer_description_type)) != 0)
    {
      UIM_MSG_HIGH_0("Bearer descriptions do not match - do not reuse channel");
      ch_reuse = FALSE;
    }
    else if (estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.access_point_name.length == 0)
    {
      UIM_MSG_HIGH_0("Current Open Channel has NULL APN - reuse previous channel");
      ch_reuse = TRUE;
    }
    else if (estk_curr_inst_ptr->prev_open_ch.access_point_name.length == 0)
    {
      UIM_MSG_HIGH_0("Prev Open Channel has NULL APN,"
                     "current Open Channel has an APN - do not re use channel");
      ch_reuse = FALSE;
    }
    else if (memcmp((uint8*)estk_curr_inst_ptr->
                      curr_cmd.cmd.open_ch_pro_cmd_req.access_point_name.alphabet,
                    (uint8*)estk_curr_inst_ptr->
                      prev_open_ch.access_point_name.alphabet,
                    estk_curr_inst_ptr->
                      curr_cmd.cmd.open_ch_pro_cmd_req.access_point_name.length * sizeof(uint16)) != 0)
    {
      UIM_MSG_HIGH_0("APN mismatch - do not reuse previous channel");
      ch_reuse = FALSE;
    }
    else
    {
      UIM_MSG_HIGH_0("APN matches & Bearer matches - reuse previous channel");
      ch_reuse = TRUE;
    }

    if (ch_reuse)
    {
      UIM_MSG_HIGH_0("estk_populate_open_ch_table: CHANNEL reuse - clear timer");
      (void)rex_clr_timer(estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p);
    }
  }

  /* Clean-up channel table if table is not being populated as part of
     Channel Reuse */
  if(!ch_reuse &&
     estk_curr_inst_ptr->keep_ch_open &&
     estk_curr_inst_ptr->ch_open_id == index)
  {
    estk_curr_inst_ptr->keep_pdn_conn = TRUE;
    estk_bip_clean_up_ch_table(index);
  }

  /* If fallback is in progress, retrieve the cached Open Channel command */
  if(estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_FALLBACK_IN_PROGRESS)
  {
    UIM_MSG_HIGH_0("estk_populate_open_ch_table: ESTK_CH_FALLBACK_IN_PROGRESS");
    open_ch_cmd = &estk_curr_inst_ptr->bip_channel_table[index].open_ch_cmd;
    in_fallback = TRUE;
  }
  else
  {
    open_ch_cmd = &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req;

    estk_open_ch_cleanup(&estk_curr_inst_ptr->bip_channel_table[index].open_ch_cmd);
    /* Save the Open Channel command for later use (in fallback) */
    (void)estk_save_open_channel_cmd_details(
      &estk_curr_inst_ptr->curr_cmd,
      &estk_curr_inst_ptr->bip_channel_table[index].open_ch_cmd);

    /* Save the Open Channel command for possible channel reuse */
    (void)estk_save_prev_open_ch(&estk_curr_inst_ptr->curr_cmd);
  }

  if(!ch_reuse)
  {
    UIM_MSG_HIGH_0("estk_populate_open_ch_table: Initialize CHANNEL data...");

    estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_PENDING;
    estk_curr_inst_ptr->bip_channel_table[index].nw_type = nw_type;
    estk_curr_inst_ptr->curr_bip_cmd_index = index;
    estk_curr_inst_ptr->bip_channel_table[index].channel_id = (index + 1);
  }
  estk_curr_inst_ptr->bip_channel_table[index].sock_fd = DSS_ERROR;

  open_ch_cmd = &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req;

  estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_PENDING;
  estk_curr_inst_ptr->bip_channel_table[index].nw_type = nw_type;
  estk_curr_inst_ptr->curr_bip_cmd_index = (int8)index;
  estk_curr_inst_ptr->bip_channel_table[index].channel_id = (index + 1);

  /* Cache the currently ongoing proactive command - only done for OPEN CHANNEL,
     as for rest of BIP Proactive commands, the chaching happens elsewhere */
  if(estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_id == GSTK_OPEN_CH_REQ)
  {
    estk_curr_inst_ptr->bip_channel_table[index].current_command_id = GSTK_OPEN_CH_REQ;
  }

  /* Initialize or update Channel parameters */
  estk_curr_inst_ptr->bip_channel_table[index].transport_protocol =
    open_ch_cmd->transport_level.transport_protocol;
  estk_curr_inst_ptr->bip_channel_table[index].port_number =
    uint32touint16(open_ch_cmd->transport_level.port_number);
  estk_curr_inst_ptr->bip_channel_table[index].card_has_auth_params = FALSE;
#ifdef FEATURE_CDMA
  memset(&(estk_curr_inst_ptr->bip_channel_table[index].policy_info), 0,
         sizeof(estk_curr_inst_ptr->bip_channel_table[index].policy_info));
#endif
  /* Handle buffer size and if the command parameters are modified */
  if(open_ch_cmd->buffer_size <= (int32)ESTK_BIP_MAX_BUFFER_SIZE)
  {
    estk_curr_inst_ptr->bip_channel_table[index].buffer_size = open_ch_cmd->buffer_size;
    estk_curr_inst_ptr->bip_channel_table[index].open_ch_param_modified = FALSE;
  }
  else
  {
    estk_curr_inst_ptr->bip_channel_table[index].buffer_size = ESTK_BIP_MAX_BUFFER_SIZE;
    estk_curr_inst_ptr->bip_channel_table[index].open_ch_param_modified = TRUE;
  }

  /* Handle On demand Link Establishment*/
  estk_curr_inst_ptr->bip_channel_table[index].on_demand_link = open_ch_cmd->on_demand_link_est;

  /* Handle Bearer type and copy parameters */
  (void)gstk_memcpy(&estk_curr_inst_ptr->bip_channel_table[index].bearer_desc,
                 &open_ch_cmd->bearer,
                 sizeof(estk_curr_inst_ptr->bip_channel_table[index].bearer_desc),
                 sizeof(estk_curr_inst_ptr->bip_channel_table[index].bearer_desc),
                 sizeof(estk_curr_inst_ptr->bip_channel_table[index].bearer_desc));

  /* Handle memory allocation for the receive buffer */
  if(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer != NULL)
  {
    /* Free dangling pointer if any */
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer);
  }

  estk_curr_inst_ptr->bip_channel_table[index].recv_buffer = (uint8 *)GSTK_CALLOC(
    int32touint32(estk_curr_inst_ptr->bip_channel_table[index].buffer_size));
  if(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer == NULL)
  {
    return ESTK_ERROR;
  }

  if(estk_curr_inst_ptr->bip_channel_table[index].send_buffer != NULL)
  {
    /* Free dangling pointer if any */
    gstk_free(estk_curr_inst_ptr->bip_channel_table[index].send_buffer);
  }

  estk_curr_inst_ptr->bip_channel_table[index].send_buffer = (uint8 *)GSTK_CALLOC(
    int32touint32(estk_curr_inst_ptr->bip_channel_table[index].buffer_size));
  if(estk_curr_inst_ptr->bip_channel_table[index].send_buffer == NULL)
  {
    return ESTK_ERROR;
  }

  estk_curr_inst_ptr->bip_channel_table[index].send_buf_len = 0;
  estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len = 0;
  estk_curr_inst_ptr->bip_channel_table[index].send_data_timer_expired = FALSE;

  /* Local Address */
  if(estk_populate_address_in_open_ch_table(index, ESTK_LOCAL_ADDRESS,
       &(open_ch_cmd->local_address)) != ESTK_SUCCESS)
  {
    UIM_MSG_ERR_0("Saving Local IP address info failed");
    return ESTK_ERROR;
  }

    /* APN setting */
    if(open_ch_cmd->access_point_name.alphabet_given == TRUE &&
       open_ch_cmd->access_point_name.length != 0)
    {
      UIM_MSG_HIGH_0("APN present in open channel command");
      if(open_ch_cmd->access_point_name.length > 0)
      {
        estk_curr_inst_ptr->bip_channel_table[index].apn_len =
          open_ch_cmd->access_point_name.length;
        /* If NULL terminated remove */
        if(open_ch_cmd->access_point_name.alphabet[open_ch_cmd->access_point_name.length - 1] == 0)
        {
          estk_curr_inst_ptr->bip_channel_table[index].apn_len--;
        }
        if (estk_curr_inst_ptr->bip_channel_table[index].apn)
        {
          gstk_free(estk_curr_inst_ptr->bip_channel_table[index].apn);
          estk_curr_inst_ptr->bip_channel_table[index].apn = NULL;
        }
        estk_curr_inst_ptr->bip_channel_table[index].apn = (uint8 *)GSTK_CALLOC(
          (estk_curr_inst_ptr->bip_channel_table[index].apn_len + 1) * sizeof(uint8));
        if(estk_curr_inst_ptr->bip_channel_table[index].apn == NULL)
        {
          estk_curr_inst_ptr->bip_channel_table[index].apn_len = 0;
          return ESTK_ERROR;
        }
        for (i = 0; i < estk_curr_inst_ptr->bip_channel_table[index].apn_len; ++i)
        {
          estk_curr_inst_ptr->bip_channel_table[index].apn[i] =
            uint16touint8(open_ch_cmd->access_point_name.alphabet[i]);
          UIM_MSG_HIGH_2("APN[%d] = 0x%x",
                         i, estk_curr_inst_ptr->bip_channel_table[index].apn[i]);
        }
        estk_curr_inst_ptr->bip_channel_table[index].apn[estk_curr_inst_ptr->bip_channel_table[index].apn_len] = '\0';
        if (estk_modify_apn_based_on_mapping_table(index) != ESTK_SUCCESS)
        {
          UIM_MSG_ERR_0("Failed to modify APN based on mapping table!");
        }
      }
    }

    /* login name */
    if(open_ch_cmd->text_string_user_login.alphabet_given &&
       open_ch_cmd->text_string_user_login.length != 0)
    {
      UIM_MSG_HIGH_0("Save login name");
      estk_curr_inst_ptr->bip_channel_table[index].username_len =
          int32touint32(open_ch_cmd->text_string_user_login.length);
      estk_curr_inst_ptr->bip_channel_table[index].card_has_auth_params = TRUE;

      /* If NULL terminated remove */
      if(open_ch_cmd->text_string_user_login.alphabet[open_ch_cmd->text_string_user_login.length - 1] == 0)
      {
        estk_curr_inst_ptr->bip_channel_table[index].username_len--;
      }
      estk_curr_inst_ptr->bip_channel_table[index].username =
        (char *)GSTK_CALLOC(estk_curr_inst_ptr->bip_channel_table[index].username_len);
      if (estk_curr_inst_ptr->bip_channel_table[index].username)
      {
        for (i = 0; i < estk_curr_inst_ptr->bip_channel_table[index].username_len; ++i)
        {
          estk_curr_inst_ptr->bip_channel_table[index].username[i] =
            uint16touint8(open_ch_cmd->text_string_user_login.alphabet[i]);
          UIM_MSG_HIGH_2("UserLogin[%d] = 0x%x",
                         i, estk_curr_inst_ptr->bip_channel_table[index].username[i]);
        }
      }
      else
      {
        return ESTK_ERROR;
      }
    }

    /* password */
    if(open_ch_cmd->text_string_user_passwd.alphabet_given &&
       open_ch_cmd->text_string_user_passwd.length != 0)
    {
      UIM_MSG_HIGH_0("Save password");
      estk_curr_inst_ptr->bip_channel_table[index].passwd_len =
          int32touint32(open_ch_cmd->text_string_user_passwd.length);
      estk_curr_inst_ptr->bip_channel_table[index].card_has_auth_params = TRUE;

      if(open_ch_cmd->text_string_user_passwd.alphabet[open_ch_cmd->text_string_user_passwd.length - 1] == 0)
      {
        estk_curr_inst_ptr->bip_channel_table[index].passwd_len--;
      }
      estk_curr_inst_ptr->bip_channel_table[index].passwd =
        (char *)GSTK_CALLOC(estk_curr_inst_ptr->bip_channel_table[index].passwd_len);
      if (estk_curr_inst_ptr->bip_channel_table[index].passwd)
      {
        for (i = 0; i < estk_curr_inst_ptr->bip_channel_table[index].passwd_len; ++ i)
        {
          estk_curr_inst_ptr->bip_channel_table[index].passwd[i] =
            uint16touint8(open_ch_cmd->text_string_user_passwd.alphabet[i]);
        }
      }
      else
      {
        return ESTK_ERROR;
      }
    }

  /* Data Destination address */
  if(estk_populate_address_in_open_ch_table(index, ESTK_DESTINATION_ADDRESS,
       &(open_ch_cmd->data_dest_address)) != ESTK_SUCCESS)
  {
    UIM_MSG_ERR_0("Saving Destination IP address info failed");
    return ESTK_ERROR;
  }

  /* Attempt immediate link establishment if card has requested it or fallback
     is being attempted */
  if((open_ch_cmd->on_demand_link_est == TRUE) &&
     (in_fallback == FALSE))
  {
    UIM_MSG_HIGH_0("On demand link establishment");
    return ESTK_SUCCESS;
  }
  else
  {
    p_dest_addr_v4 =
      (struct ps_sockaddr_in*)estk_curr_inst_ptr->bip_channel_table[index].dest_addr;
    
    if (estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.dns_requested)
    {
      address_family = DSS_AF_INET;
    }
    else if (p_dest_addr_v4 != NULL)
    {
      address_family = p_dest_addr_v4->ps_sin_family;
    }

    if(estk_bip_bringup_call(index, address_family) == ESTK_ERROR)
    {    
      if (open_ch_cmd != NULL && open_ch_cmd->dns_requested && address_family == DSS_AF_INET)
      {
        UIM_MSG_ERR_0("DNS query: iface v4 bringup fails, attempt iface v6...");
        return estk_bip_bringup_call(index, DSS_AF_INET6);
      }
      else
    {
      UIM_MSG_ERR_0("Bring up call failed");
      estk_bip_clean_up_ch_table(index);
      return ESTK_ERROR;      }
    }
    else
    {
      UIM_MSG_HIGH_0("BIP link successfully established");
      return ESTK_SUCCESS;
    }
  }
} /* estk_populate_open_ch_table */

/*===========================================================================

FUNCTION: estk_bip_check_existing_gsm_cs_call

DESCRIPTION :
  This function checks for the CS call existence in case of GSM RAT.

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE: Voice call is already established connected
  FALSE: No CS Call.

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
static boolean estk_bip_check_existing_gsm_cs_call(void)
{
  gstk_slot_id_enum_type       slot             = GSTK_SLOT_ID_MAX;
  gstk_status_enum_type        gstk_status      = GSTK_SUCCESS;
  boolean                      cs_ps_parallel   = FALSE;
  boolean                      dtm_feat_support = FALSE;
  gstk_nv_items_data_type      nv_data;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, FALSE); 
  slot = estk_curr_inst_ptr->slot_id;

  if(!ESTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("Invalid slot %d", slot);
    return FALSE;
  }
  (void)gstk_nv_access_read(GSTK_NV_SLOT_TOOLKIT_CS_PS_PARALLEL_I,
                            slot,
                            &nv_data);

  cs_ps_parallel = nv_data.cs_ps_parallel;

  (void)gstk_nv_access_read(GSTK_NV_EXT_SUB_DTM_FEATURE_SUPPORT_I,
                            slot,
                            &nv_data);

  dtm_feat_support = nv_data.dtm_feat_support;

  gstk_status = gstk_io_ctrl(GSTK_IO_GET_CURR_ACCESS_TECH,
                             (void *)&slot,
                             (void *)&estk_curr_access_tech);
  if(gstk_status == GSTK_SUCCESS)
  {
    /* Only in case of GSM Access Technology estk checks for the CS and PS Parllel Call Support
       Where as in case of UMTS and LTE by default the Parllel Calls are possible,
       therefore !cs_ps_parallel_call should be checked only in case of GSM Technology */
    if((estk_curr_access_tech == GSTK_ACCESS_TECH_GSM) &&
       (!cs_ps_parallel || !dtm_feat_support)
        /*If there is any existing CS Call, then send TR with unable to Process the Command */
       && (estk_instances_ptr[(uint8)slot - 1]->estk_cm_cs_call_connected != 0))
    {
      UIM_MSG_HIGH_0("GSM CS Call is in progress");
      return TRUE;
    }
  }
  return FALSE;
}

/*===========================================================================

FUNCTION: estk_process_bip_user_cnf_rsp

DESCRIPTION :
  This function will be called by ESTK to process the user confirmation response
  received from the UI client.If the user confirmed for an affirmative action,
  the call will be set up, otherwise an appropriate Terminal Response will be
  sent to the card saying "The user rejected the call setup".
  If the user fails to respond within a certain time, a Terminal response of
  "No response from the user will be sent to the card.

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_bip_user_cnf_rsp(
  const estk_cmd_type *cmd_ptr
)
{
  uint32                              cmd_ref_id       = 0;
  uint32                              type_of_command  = 0;
  boolean                             send_tr          = FALSE;
  gstk_general_result_enum_type       gen_result       = GSTK_GENERAL_RESULT_MAX;
  int32                               index            = -1;
  gstk_terminal_rsp_extra_param_type  tr_extra_param;
  gstk_additional_info_ptr_type       additional_info;
  uint8                               addi_res         = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
  gstk_open_ch_req_type              *open_ch_cmd      = NULL;
  gstk_slot_id_enum_type              slot             = GSTK_SLOT_ID_MAX;
  estk_network_type                   nw_type          = ESTK_NETWORK_MAX;
  estk_result_enum_type               estk_status      = ESTK_SUCCESS;

  if(cmd_ptr == NULL)
  {
    /*Either cmd_ptr is NULL or Ignoring Second User confirmation*/
    return ESTK_ERROR;
  }

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;

  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));

  if(rex_get_timer(&(estk_curr_inst_ptr->user_or_disp_conf_timer)) > 0)
  {
    UIM_MSG_HIGH_0("Clear user_or_disp_conf_timer on alpha response");
    (void)rex_clr_timer(&(estk_curr_inst_ptr->user_or_disp_conf_timer));
  }

  open_ch_cmd = &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req;

   /* Step 1: Check input parameters */
  cmd_ref_id = cmd_ptr->hdr.cmd_ref_id;
  if(GSTK_CMD_REF_TABLE_INDEX(cmd_ref_id) >= GSTK_MAX_PROACTIVE_COMMAND)
  {
    UIM_MSG_ERR_1("cmd_ref_id 0x%x >= GSTK_MAX_PROACTIVE_COMMAND", cmd_ref_id);
    return ESTK_ERROR;
  }

  type_of_command =
    command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_ref_id)].command_details.type_of_command;
  if(type_of_command != GSTK_CMD_STK_OPEN_CHANNEL)
  {
    UIM_MSG_ERR_1("Incorrect command type 0x%x", type_of_command);
    return ESTK_ERROR;
  }

  if(cmd_ptr->hdr.cmd != ESTK_GENERAL_USER_CNF_CMD)
  {
    UIM_MSG_ERR_1("Incorrect command id 0x%x", cmd_ptr->hdr.cmd);
    return ESTK_ERROR;
  }

  additional_info.length = 0;

  UIM_MSG_HIGH_1("Icon disp is 0x%x", cmd_ptr->cmd_data.user_rsp.icon_disp);
  estk_curr_inst_ptr->icon_disp = cmd_ptr->cmd_data.user_rsp.icon_disp;

  /* Check user response. If User said "No", send TR and do not originate call */
  if(cmd_ptr->cmd_data.user_rsp.yes_no_rsp == 0)
  {
    UIM_MSG_HIGH_0("User did not accept open channel request");
    gen_result = GSTK_USER_DID_NOT_ACCEPT_CALL_SET_UP_REQUEST;
    send_tr    = TRUE;
  }
  else /* If user responded with a yes */
  {
    additional_info.length = 1;
    if(estk_find_ch_socket_index(&index, ESTK_CH_CLOSED) != ESTK_SUCCESS)
    {
      /* no more room */
      UIM_MSG_ERR_0("No more Room for Open Ch");
      gen_result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
      send_tr    = TRUE;
      addi_res   = (uint8)GSTK_CH_UNAVAILABLE;
      additional_info.additional_info_ptr = &addi_res;
    }
    else
    {
      UIM_MSG_HIGH_1("Index chosen for open channel %d", index );
      if(open_ch_cmd->transport_level.transport_protocol
        == GSTK_PKT_DATA_TCP_UICC_SERVER_MODE)
      {
        if (estk_scws_populate_open_ch_table(index) != ESTK_SUCCESS)
        {
          UIM_MSG_ERR_0("estk_scws_populate_open_ch_table failed");
          gen_result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
          send_tr    = TRUE;
          addi_res   = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
          additional_info.additional_info_ptr = &addi_res;
        }
        else
        {
          send_tr = FALSE;
        }
      }
      else
      {
        slot = estk_curr_inst_ptr->curr_cmd.hdr_cmd.sim_slot_id;

        if(!ESTK_IS_VALID_SLOT_ID(slot))
        {
          UIM_MSG_HIGH_1("Invalid slot %d", slot );
          return ESTK_ERROR;
        }

        estk_status = estk_bip_get_prio_network(      
                        &nw_type,
                        &addi_res,
                        slot,
                        estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.bearer.bearer_type,
                        estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.data_dest_address.address_type);

        if(estk_status != ESTK_SUCCESS)
        {
          gen_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
          send_tr    = TRUE;
          additional_info.additional_info_ptr = &addi_res;
        }

        /* Continue processing the Open Channel command if no error has yet
           been flagged */
        if(send_tr == FALSE)
        {
          if(estk_populate_open_ch_table(index, nw_type) != ESTK_SUCCESS)
          {
            UIM_MSG_ERR_0("estk_populate_open_ch_table failed");
            gen_result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
            send_tr    = TRUE;
            addi_res   = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
            additional_info.additional_info_ptr = &addi_res;
          }
          else
          {
            if(open_ch_cmd->on_demand_link_est)
            {
              UIM_MSG_HIGH_1("Sending TR because on demand link is 0x%x",
                             open_ch_cmd->on_demand_link_est);
              send_tr = TRUE;
              gen_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
              additional_info.length = 0;
              additional_info.additional_info_ptr = NULL;
              tr_extra_param.extra_param.open_ch_extra_param.ch_status.ch_id =
                estk_curr_inst_ptr->bip_channel_table[index].channel_id;
            }
            else
            {
              UIM_MSG_HIGH_0("Populating open channel table succeeded");
              return ESTK_SUCCESS;
            }
          }
        }
      }
    }
  }

  if(send_tr == TRUE)
  {
    tr_extra_param.present = TRUE;
    tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;
    (void)memscpy(&tr_extra_param.extra_param.open_ch_extra_param.bearer_description,
           sizeof(gstk_bearer_description_type),
           &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.bearer,
           sizeof(gstk_bearer_description_type));

    tr_extra_param.extra_param.open_ch_extra_param.buffer_size =
      open_ch_cmd->buffer_size;

    /* If the Open Ch is SCWS type, set the is_scws_ch to TRUE so that the correct TR is sent */
    if(open_ch_cmd->transport_level.transport_protocol
       == GSTK_PKT_DATA_TCP_UICC_SERVER_MODE)
    {
      tr_extra_param.extra_param.open_ch_extra_param.ch_status.is_scws_ch = TRUE;
    }

    if (gen_result != GSTK_COMMAND_PERFORMED_SUCCESSFULLY && index != -1)
    {
      estk_bip_clean_up_ch_table(index);
    }

    return estk_bip_send_terminal_response(
             cmd_ref_id,
             GSTK_OPEN_CH_CNF,
             gen_result,
             &additional_info,
             &tr_extra_param,
             index);
  }

  return ESTK_ERROR;
} /* estk_process_bip_user_cnf_rsp */

/*===========================================================================
FUNCTION: estk_process_open_ch_req

DESCRIPTION:
  Process the OPEN CHANNEL Toolkit Request from the card

PARAMETERS:
  gstk_req_ptr : [Input] gstk OPEN CHANNEL cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_open_ch_req (
  gstk_cmd_from_card_type *gstk_req_ptr
)
{
  gstk_client_id_type                 client_id            = 0;
  gstk_status_enum_type               gstk_status          = GSTK_SUCCESS;
  uint32                              cmd_ref_id           = 0;
  gstk_additional_info_ptr_type       additional_info      = {0};
  gstk_terminal_rsp_extra_param_type  tr_extra_param;
  gstk_slot_id_enum_type              slot                 = GSTK_SLOT_ID_MAX;
  estk_network_type                   nw_type              = ESTK_NETWORK_MAX;
  uint8                               addi_res             = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
  estk_result_enum_type               estk_status          = ESTK_SUCCESS;
  gstk_general_result_enum_type       gen_result           = GSTK_GENERAL_RESULT_MAX;

  if(gstk_req_ptr == NULL)
  {
    UIM_MSG_ERR_0("GSTK request ptr is NULL");
    return ESTK_ERROR;
  }

  UIM_MSG_HIGH_2("estk_process_open_ch_req, slot: 0x%x, Transport Protocol: 0x%x",
                 gstk_req_ptr->hdr_cmd.sim_slot_id,
                 gstk_req_ptr->cmd.open_ch_pro_cmd_req.transport_level.transport_protocol);

  slot = gstk_req_ptr->hdr_cmd.sim_slot_id;
  if (!ESTK_IS_VALID_SLOT_ID(slot))
  {
    return ESTK_ERROR;
  }

  memset((void*)&tr_extra_param, 0x00, sizeof(tr_extra_param));
  tr_extra_param.present = TRUE;
  tr_extra_param.rsp_type = GSTK_OPEN_CH_CNF;
  cmd_ref_id = gstk_req_ptr->hdr_cmd.cmd_detail_reference;
  additional_info.length = 0;
  additional_info.additional_info_ptr = NULL;

  if(gstk_req_ptr->cmd.open_ch_pro_cmd_req.transport_level.transport_protocol
      != GSTK_PKT_DATA_TCP_UICC_SERVER_MODE)
  {
    (void)memscpy(&tr_extra_param.extra_param.open_ch_extra_param.bearer_description,
                  sizeof(gstk_bearer_description_type),
                  &estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.bearer,
                  sizeof(gstk_bearer_description_type));

    estk_status = estk_bip_get_prio_network(
                        &nw_type,
                        &addi_res,
                        slot,
                        estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.bearer.bearer_type,
                        estk_curr_inst_ptr->curr_cmd.cmd.open_ch_pro_cmd_req.data_dest_address.address_type);

    if(estk_status != ESTK_SUCCESS)
    {
      gen_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
      additional_info.length = 1;
      additional_info.additional_info_ptr = &addi_res;
      return estk_send_terminal_response(cmd_ref_id,
                                  GSTK_OPEN_CH_CNF,
                                  GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
                                  &additional_info,
                                  &tr_extra_param);
    }
  }

  if(((gstk_req_ptr->cmd.open_ch_pro_cmd_req.transport_level.transport_protocol
      != GSTK_PKT_DATA_UDP) &&
      (gstk_req_ptr->cmd.open_ch_pro_cmd_req.transport_level.transport_protocol
      != GSTK_PKT_DATA_TCP)
      && (gstk_req_ptr->cmd.open_ch_pro_cmd_req.transport_level.transport_protocol
      != GSTK_PKT_DATA_TCP_UICC_SERVER_MODE)
      ) && !gstk_req_ptr->cmd.open_ch_pro_cmd_req.dns_requested)
  {
    UIM_MSG_ERR_0("Transport protocol Not Supported");

    return estk_bip_send_terminal_response(
             cmd_ref_id,
             GSTK_OPEN_CH_CNF,
             GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
             &additional_info,
             &tr_extra_param,
             ESTK_MAX_BIP_CH);
  }
  else
  {
    estk_curr_inst_ptr->state = ESTK_WAIT_FOR_USER_CNF_ST;
    client_id = estk_curr_inst_ptr->gstk_client_id;
    if ((gstk_nv_get_me_feature_status(GSTK_CFG_FEATURE_ME_ESTK_IGNORE_ALPHA) == TRUE)
       || (gstk_req_ptr->cmd.open_ch_pro_cmd_req.transport_level.transport_protocol
       == GSTK_PKT_DATA_TCP_UICC_SERVER_MODE)
       )
    {
      gstk_status = gstk_send_user_cnf_alpha_and_icon_disp_rsp(client_id, cmd_ref_id, TRUE, 0);
      if(gstk_status == GSTK_SUCCESS)
      {
        return ESTK_SUCCESS;
      }
      else
      {
        UIM_MSG_ERR_0("gstk_send_user_cnf_alpha_and_icon_disp_rsp failed");
        return ESTK_ERROR;
      }
    }
    /* Check if confirmation alpha is present*/
    else if(((gstk_req_ptr->cmd.open_ch_pro_cmd_req.alpha.length) > 0 &&
            (gstk_req_ptr->cmd.open_ch_pro_cmd_req.alpha.alphabet)) ||
            (gstk_nv_get_me_feature_status(GSTK_CFG_FEATURE_ME_ESTK_QMI_SEND_NO_AND_NULL_ALPHA_IND) == TRUE))
    {
      UIM_MSG_HIGH_1("ALPHA TLV length: 0x%x",
                      gstk_req_ptr->cmd.open_ch_pro_cmd_req.alpha.length);
      return estk_send_alpha_to_display_client(gstk_req_ptr, FALSE);
    }
    else /* no alpha or null alpha */
    {
      gstk_status = gstk_send_user_cnf_alpha_and_icon_disp_rsp(client_id, cmd_ref_id, TRUE, 0);
      if(gstk_status == GSTK_SUCCESS)
      {
        return ESTK_SUCCESS;
      }
      else
      {
        UIM_MSG_ERR_0("gstk_send_user_cnf_alpha_and_icon_disp_rsp failed");
        return ESTK_ERROR;
      }
    }
  }
} /* estk_process_open_ch_req */

/*===========================================================================
FUNCTION: estk_convert_ch_id_to_index

DESCRIPTION:
  Find out index in bip_channel_table based on channel id

PARAMETERS:
  ch_id :   [Input] Channel id whose index has to found
  index :   [Input] Result that contains the mapped index

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_convert_ch_id_to_index (
  int32                   ch_id,
  int32                  *index
)
{
  uint32                i       = 0;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  if(index == NULL)
  {
    return ESTK_ERROR;
  }
  for(i=0; i<ESTK_MAX_BIP_CH; i++)
  {
    if(estk_curr_inst_ptr->bip_channel_table[i].channel_id == ch_id)
    {
      *index = estk_curr_inst_ptr->bip_channel_table[i].index;
      return ESTK_SUCCESS;
    }
  }
  return ESTK_ERROR;
} /* estk_convert_ch_id_to_index */

/*===========================================================================
FUNCTION: estk_process_send_data_req

DESCRIPTION:
  Process the Send Data Toolkit Request from the card

PARAMETERS:
  gstk_req_ptr : [Input] gstk SEND DATA cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_send_data_req (
  gstk_cmd_from_card_type *gstk_req_ptr
)
{
  int32                               index;
  int32                               ch_id;
  estk_result_enum_type               estk_status     = ESTK_SUCCESS;
  uint32                              cmd_ref_id      = 0;
  gstk_terminal_rsp_extra_param_type  tr_extra_param;
  gstk_additional_info_ptr_type       additional_info;
  uint8                               addi_res;
  gstk_raw_cmd_from_card_type         raw_cmd;
  struct ps_sockaddr_in               *p_dest_addr_v4 = NULL;

  if(gstk_req_ptr == NULL)
  {
    UIM_MSG_ERR_0("GSTK request ptr is NULL");
    return ESTK_ERROR;
  }

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;

  memset(&raw_cmd, 0x00, sizeof(gstk_raw_cmd_from_card_type));
  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));

  additional_info.length = 1;
  additional_info.additional_info_ptr = &addi_res;
  cmd_ref_id = gstk_req_ptr->hdr_cmd.cmd_detail_reference;
  ch_id = gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_id;

  UIM_MSG_HIGH_2("estk_process_send_data_req: cmd_detail_reference: 0x%x, ch_id: 0x%x",
                 cmd_ref_id, ch_id);

  estk_status = estk_convert_ch_id_to_index(ch_id, &index);

  if(estk_status != ESTK_SUCCESS)
  {
    addi_res = (uint8)GSTK_CH_ID_INVALID;
    UIM_MSG_ERR_0("estk_convert_ch_id_to_index failed");
    return estk_bip_send_terminal_response(
             cmd_ref_id,
             GSTK_SEND_DATA_CNF,
             GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
             &additional_info,
             NULL,
             ESTK_MAX_BIP_CH);
  }

  /* Cache the currently ongoing proactive command */
  estk_curr_inst_ptr->bip_channel_table[index].current_command_id = GSTK_SEND_DATA_REQ;

  /* Check channel status if PENDING or ACTIVE */
  if(estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_PENDING
     && estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_ACTIVE
     && estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_OPEN_WAIT
     && estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_SCWS_LISTEN
     && estk_curr_inst_ptr->bip_channel_table[index].status != ESTK_CH_SCWS_ESTABLISHED
  )
  {
    UIM_MSG_ERR_1("Channel status for send data is wrong for send data %d",
                  estk_curr_inst_ptr->bip_channel_table[index].status);

    addi_res = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    return estk_bip_send_terminal_response(
             cmd_ref_id,
             GSTK_SEND_DATA_CNF,
             GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
             &additional_info,
             NULL,
             index);
  }

  p_dest_addr_v4 =
    (struct ps_sockaddr_in*)estk_curr_inst_ptr->bip_channel_table[index].dest_addr;
  if (p_dest_addr_v4 == NULL)
  {
    UIM_MSG_ERR_0("estk_process_send_data_req: invalid dest addr!");
    addi_res = (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    return estk_bip_send_terminal_response(
             cmd_ref_id,
             GSTK_SEND_DATA_CNF,
             GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
             &additional_info,
             NULL,
             index);
  }

  /* Send data timer will expire when socket write fails or Link
     Establishment (on demand link) fails */
  if(estk_curr_inst_ptr->bip_channel_table[index].send_data_timer_expired)
  {
    UIM_MSG_HIGH_2("Send Data timer has expired previously, Send Buffer len: %d, Send Data Command data len: %d",
                   estk_curr_inst_ptr->bip_channel_table[index].send_buf_len,
                   gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_data.data_len);

    /* Card is resending only the failed SEND DATA request */
    if((gstk_req_ptr->cmd.send_data_pro_cmd_req.send_data_immediately) &&
       (estk_curr_inst_ptr->bip_channel_table[index].send_buf_len >=
        uint32touint16(gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_data.data_len)))
    {
      estk_curr_inst_ptr->bip_channel_table[index].send_buf_len
        -= uint32touint16(gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_data.data_len);
    }
    /* Card is resending SEND DATA from first */
    else
    {
      estk_curr_inst_ptr->bip_channel_table[index].send_buf_len = 0;
    }
    estk_curr_inst_ptr->bip_channel_table[index].send_data_timer_expired = FALSE;
  }

  /* Check if Send Buffer is valid and there is space in buffer */
  if((estk_curr_inst_ptr->bip_channel_table[index].send_buffer == NULL) ||
     ((estk_curr_inst_ptr->bip_channel_table[index].send_buf_len
       + gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_data.data_len) >
      estk_curr_inst_ptr->bip_channel_table[index].buffer_size))
  {
    UIM_MSG_ERR_3("Send Buffer 0x%x, Send buffer len %d, Send Data buffer len %d",
                  estk_curr_inst_ptr->bip_channel_table[index].send_buffer,
                  estk_curr_inst_ptr->bip_channel_table[index].send_buf_len,
                  gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_data.data_len);

    addi_res = (uint8)GSTK_BUFFER_SIZE_UNAVAILABLE;
    additional_info.additional_info_ptr = &addi_res;

    return estk_bip_send_terminal_response(
             cmd_ref_id,
             GSTK_SEND_DATA_CNF,
             GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
             &additional_info,
             NULL,
             index);
  }

  if(gstk_memcpy((void *)(estk_curr_inst_ptr->bip_channel_table[index].send_buffer +
                          estk_curr_inst_ptr->bip_channel_table[index].send_buf_len),
                 (void *)gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_data.data,
                 gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_data.data_len,
                 estk_curr_inst_ptr->bip_channel_table[index].buffer_size -
                 estk_curr_inst_ptr->bip_channel_table[index].send_buf_len,
                 gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_data.data_len) <
     (size_t)gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_data.data_len)
  {
    UIM_MSG_ERR_0("estk_process_send_data_req(): gstk_memcpy failed");
    addi_res = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    return estk_bip_send_terminal_response(cmd_ref_id,
                                           GSTK_SEND_DATA_CNF,
                                           GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                           &additional_info,
                                           NULL,
                                           index);
  }

  /* Check if display alpha is present*/
  if(((gstk_req_ptr->cmd.send_data_pro_cmd_req.alpha.length) > 0 &&
      (gstk_req_ptr->cmd.send_data_pro_cmd_req.alpha.alphabet)) ||
     (gstk_nv_get_me_feature_status(GSTK_CFG_FEATURE_ME_ESTK_QMI_SEND_NO_AND_NULL_ALPHA_IND) == TRUE))
  {
    UIM_MSG_HIGH_0("ALPHA TLV present in Send Data command"  );
    if(gstk_nv_get_me_feature_status(GSTK_CFG_FEATURE_ME_ESTK_IGNORE_ALPHA) == FALSE)
    {
      estk_status = estk_send_alpha_to_display_client(gstk_req_ptr, TRUE);
      if(estk_status != ESTK_SUCCESS)
      {
        return ESTK_ERROR;
      }
    }
  }

  estk_curr_inst_ptr->bip_channel_table[index].send_buf_len
    += uint32touint16(gstk_req_ptr->cmd.send_data_pro_cmd_req.ch_data.data_len);

  if(estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_PENDING)
  {
    UIM_MSG_HIGH_0("Send Data Channel status is pending");
    estk_status = estk_bip_bringup_call(index, p_dest_addr_v4->ps_sin_family);
    if(estk_status != ESTK_SUCCESS)
    {
      addi_res = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
      if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
      {
        UIM_MSG_HIGH_0("Saving SEND DATA TR");
        estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
        estk_curr_inst_ptr->result = GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR;
        estk_cache_additional_info(&additional_info);
        estk_status = ESTK_SUCCESS;
      }
      else
      {
      estk_status = estk_bip_send_terminal_response(
                      cmd_ref_id,
                      GSTK_SEND_DATA_CNF,
                      GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                      &additional_info,
                      NULL,
                      index);
      }
      if(estk_status == ESTK_SUCCESS)
      {
        estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_CLOSED;
        /* Send Event Download envelope command */
        return estk_bip_send_ch_status_event(
                 estk_curr_inst_ptr->bip_channel_table[index].channel_id,
                 GSTK_CH_STATUS_LINK_DROPPED);
      }
      else
      {
        UIM_MSG_HIGH_0("Failed on sending terminal response for send data");
        return ESTK_ERROR;
      }
    }
  }
  else if(estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_ACTIVE
         || (estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_SCWS_ESTABLISHED)
       )
  {
    UIM_MSG_HIGH_1("Send Data channel status active: send_data_immediately=%x",
                   gstk_req_ptr->cmd.send_data_pro_cmd_req.send_data_immediately);

    if(gstk_req_ptr->cmd.send_data_pro_cmd_req.send_data_immediately)
    {
      if (estk_curr_inst_ptr->bip_channel_table[index].is_scws_ch)
      {
        return estk_scws_send_data(index, cmd_ref_id);
      }
      else
      {
         return estk_bip_write_sock(index, cmd_ref_id);
      }
    }
    else
    {
      /* Success terminal response with num bytes left */
      tr_extra_param.rsp_type = GSTK_SEND_DATA_CNF;
      tr_extra_param.present = TRUE;
      tr_extra_param.extra_param.send_data_extra_param.ch_data_remain_buf_len =
        estk_curr_inst_ptr->bip_channel_table[index].buffer_size -
        estk_curr_inst_ptr->bip_channel_table[index].send_buf_len;
      if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
      {
          UIM_MSG_HIGH_0("Saving SEND DATA TR");
          estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
          estk_curr_inst_ptr->result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
          estk_curr_inst_ptr->extra_param = tr_extra_param;
          estk_status = ESTK_SUCCESS;
      }
      else
      {
        estk_status = estk_bip_send_terminal_response(
                        cmd_ref_id,
                        GSTK_SEND_DATA_CNF,
                        GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
                        NULL,
                        &tr_extra_param,
                        index);
      }
    }
  }
  else
  {
    UIM_MSG_ERR_0("Send Data Channel status neither Pending nor Active");
    if(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf != NULL)
    {
      UIM_MSG_HIGH_0("Freeing the SCWS overflow buffer");
      gstk_free(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf);
      estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf = NULL;
      estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len = 0;
    }
    if(estk_curr_inst_ptr->bip_channel_table[index].send_buffer != NULL)
    {
      UIM_MSG_HIGH_0("Setting the send buffer to 0");
      memset(estk_curr_inst_ptr->bip_channel_table[index].send_buffer,
             0x00,
             estk_curr_inst_ptr->bip_channel_table[index].send_buf_len);
      estk_curr_inst_ptr->bip_channel_table[index].send_buf_len = 0;
    }
    return ESTK_ERROR;
  }
  return estk_status;
} /* estk_process_send_data_req */

/*===========================================================================
FUNCTION: estk_util_min_3num

DESCRIPTION:
  Find the minimum of 3 numbers

PARAMETERS:
  a : [Input] First number
  b : [Input] Second number
  c : [Input] Third number

DEPENDENCIES:
  None

RETURN VALUE:
  int32 - minimum of 3 numbers

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
int32 estk_util_min_3num(int32 a, int32 b, int32 c)
{
  if (a<b)
  {
    if (a<c)
    {
      return a;
    }
    else
    {
      return c;
    }
  }
  else
  {
    if (b<c)
    {
      return b;
    }
    else
    {
      return c;
    }
  }
} /* estk_util_min_3num */

/*===========================================================================
FUNCTION: estk_process_receive_data_req

DESCRIPTION:
  Process the Receive Data Toolkit Request from the card

PARAMETERS:
  gstk_req_ptr : [Input] gstk RECEIVE DATA cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_receive_data_req (
  gstk_cmd_from_card_type *gstk_req_ptr
)
{
  int32                                 index;
  int32                                 ch_id;
  int32                                 copy_len;
  uint32                                cmd_ref_id;
  estk_result_enum_type                 estk_status = ESTK_SUCCESS;
  gstk_receive_data_req_type            *recv_data_req;
  gstk_additional_info_ptr_type         additional_info;
  uint8                                 addi_res;
  gstk_raw_cmd_from_card_type           raw_cmd;

  if(gstk_req_ptr == NULL)
  {
    UIM_MSG_ERR_0("GSTK request ptr is NULL");
    return ESTK_ERROR;
  }

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;

  memset(&raw_cmd, 0x00, sizeof(gstk_raw_cmd_from_card_type));
  memset(&additional_info, 0x00, sizeof(additional_info));

  additional_info.length = 1;
  additional_info.additional_info_ptr = &addi_res;
  cmd_ref_id = gstk_req_ptr->hdr_cmd.cmd_detail_reference;
  recv_data_req = &(gstk_req_ptr->cmd.receive_data_pro_cmd_req);
  ch_id = recv_data_req->ch_id;

  estk_status = estk_convert_ch_id_to_index(ch_id, &index);

  if(estk_status != ESTK_SUCCESS)
  {
    UIM_MSG_ERR_0("estk_convert_ch_id_to_index failed");
    return ESTK_ERROR;
  }

  /* Cache the currently ongoing proactive command */
  estk_curr_inst_ptr->bip_channel_table[index].current_command_id = GSTK_RECEIVE_DATA_REQ;

  /* If channel is already closed and no data is available send error TR
     if channel is closed, but data is available to send, send the available
     data to the card */
  if((estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len == 0) &&
     (estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_CLOSED ||
      estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_CLOSE_IN_PROGRESS ||
      estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_DROPPED))
  {
    UIM_MSG_ERR_1("Channel with status 0x%x not active to receive data",
                  estk_curr_inst_ptr->bip_channel_table[index].status);
    addi_res = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    return estk_bip_send_terminal_response(
             cmd_ref_id,
             GSTK_RECEIVE_DATA_CNF,
             GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
             &additional_info,
             NULL,
             index);
  }

  /* Check if there is available data */
  if (estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len == 0)
  {
    UIM_MSG_ERR_1("Channel with status 0x%x has no available data",
                  estk_curr_inst_ptr->bip_channel_table[index].status);
    addi_res = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    return estk_bip_send_terminal_response(
             cmd_ref_id,
             GSTK_RECEIVE_DATA_CNF,
             GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
             &additional_info,
             NULL,
             index);
  }

  /* Cache the info that RECEIVE DATA has occurred. This shall be used later
     when processing Channel Close to see if the BIP Session was successful or
     not */
  estk_curr_inst_ptr->bip_channel_table[index].receive_data = TRUE;

  if((estk_curr_inst_ptr->bip_channel_table[index].is_scws_ch == TRUE) &&
     (estk_curr_inst_ptr->bip_channel_table[index].receive_data_allowed == FALSE))
  {
    UIM_MSG_ERR_0("receive_data_allowed is FALSE, returning error");
    return ESTK_ERROR;
  }

  /* Populate terminal response with data from the receive data */
  estk_curr_inst_ptr->extra_param.present = TRUE;
  estk_curr_inst_ptr->extra_param.rsp_type = GSTK_RECEIVE_DATA_CNF;

  copy_len = estk_util_min_3num((int32)ESTK_BIP_MAX_CH_DATA_SIZE,
                                recv_data_req->ch_data_length,
                                estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len);

  UIM_MSG_HIGH_3("estk_util_min_3num returns %d, request length %d , buf len %d",
                 copy_len,
                 recv_data_req->ch_data_length,
                 estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len);

  estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data_len = copy_len;

  estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data
    = (uint8 *)GSTK_CALLOC(copy_len);

  if(estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data == NULL)
  {
    return ESTK_ERROR;
  }

  (void)gstk_memcpy((void *)estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data,
                 (void *)estk_curr_inst_ptr->bip_channel_table[index].recv_buffer,
                 copy_len,
                 copy_len,
                 copy_len);

  estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data_remaining_len =
     estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len - copy_len;

  UIM_MSG_HIGH_1("Receive Data sends terminal response with remaining len: %d",
                 estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data_remaining_len);

  /* Check if display alpha is present*/
  if(((recv_data_req->alpha.length) > 0 && (recv_data_req->alpha.alphabet)) ||
     (gstk_nv_get_me_feature_status(GSTK_CFG_FEATURE_ME_ESTK_QMI_SEND_NO_AND_NULL_ALPHA_IND) == TRUE))

  {
    UIM_MSG_HIGH_0("ALPHA TLV present in Receive Data command"  );
    if(gstk_nv_get_me_feature_status(GSTK_CFG_FEATURE_ME_ESTK_IGNORE_ALPHA) == FALSE)
    {
      estk_status = estk_send_alpha_to_display_client(gstk_req_ptr, TRUE);
      if(estk_status != ESTK_SUCCESS)
      {
        return ESTK_ERROR;
      }
    }
  }

  if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
  {
    UIM_MSG_HIGH_0("Saving RECEIVE DATA TR"  );
    estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
#ifdef FEATURE_UIM_TEST_FRAMEWORK
    #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
  }
  else
  {
    /* Send terminal response with channel data copied from Rx buffer */
    estk_status = estk_send_receive_data_terminal_response(cmd_ref_id);
  }

  return estk_status;
} /* estk_process_receive_data_req */

/*===========================================================================
FUNCTION: estk_send_receive_data_terminal_response

DESCRIPTION:
  Send TR for RECEIVE DATA Toolkit Request from the card

PARAMETERS:
  cmd_ref_id : [Input] ref_id of the Receive Data request

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_send_receive_data_terminal_response(uint32 cmd_ref_id)
{
  int32                                 ch_id;
  int32                                 index;
  int32                                 copy_len;
  gstk_additional_info_ptr_type         additional_info;
  uint8                                 addi_res;
  sint31                                dss_status  = DSS_ERROR;
  estk_result_enum_type                 estk_status = ESTK_SUCCESS;
  sint15                                error_num   = DSS_ERROR;
  gstk_evt_dl_ind_type                  data_avail;

  memset(&additional_info, 0x00, sizeof(additional_info));
  memset(&data_avail, 0x00, sizeof(gstk_evt_dl_ind_type));

  additional_info.length = 1;
  additional_info.additional_info_ptr = &addi_res;

  ch_id = estk_curr_inst_ptr->curr_cmd.cmd.receive_data_pro_cmd_req.ch_id;
  UIM_MSG_HIGH_1("estk_send_receive_data_terminal_response ch_id = 0x%x", ch_id);

  estk_status = estk_convert_ch_id_to_index(ch_id, &index);
  if(estk_status != ESTK_SUCCESS)
  {
    UIM_MSG_ERR_0("estk_convert_ch_id_to_index failed");
    return ESTK_ERROR;
  }

  copy_len = estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data_len;

  /* Send terminal response with channel data copied from Rx buffer */
  if(estk_bip_send_terminal_response(
       cmd_ref_id,
       GSTK_RECEIVE_DATA_CNF,
       GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
       NULL,
       &estk_curr_inst_ptr->extra_param,
       index) == ESTK_SUCCESS)
  {
    if(estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data != NULL)
    {
      gstk_free(estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data);
      estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data = NULL;
    }
    /* Adjust Receive buffer length*/
    estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len -= copy_len;

    if(estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len == 0)
    {
      UIM_MSG_HIGH_0("Receive buffer is empty after receive data command" );
      if(estk_curr_inst_ptr->bip_channel_table[index].is_scws_ch == TRUE)
      {
        UIM_MSG_HIGH_3("scws_overflow_buf_len is %d, buffer size is %d scws_overflow_buf is %d",
                       estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len,
                       estk_curr_inst_ptr->bip_channel_table[index].buffer_size,
                       estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf);
        if((estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len <=
            (uint32)estk_curr_inst_ptr->bip_channel_table[index].buffer_size) &&
           (estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf != NULL))
        {
          /* Copy the data from the overflow buffer if present */
          if(gstk_memcpy((void *)estk_curr_inst_ptr->bip_channel_table[index].recv_buffer,
                         (void *)(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf),
                         estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len,
                         estk_curr_inst_ptr->bip_channel_table[index].buffer_size,
                         estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len) <
             estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len)
          {
            addi_res = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
            return estk_bip_send_terminal_response(cmd_ref_id,
                                                   GSTK_SEND_DATA_CNF,
                                                   GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                                   &additional_info,
                                                   NULL,
                                                   index);
          }

          estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len =
            estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len;

          gstk_free(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf);
          estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf = NULL;
          estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len = 0;
          UIM_MSG_HIGH_1("recv_buf_len is %d, scws_overflow_buf is deleted",
                         estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len);
        }
        else if((estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len >
                 (uint32)estk_curr_inst_ptr->bip_channel_table[index].buffer_size) &&
                (estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf != NULL))
        {
          if(gstk_memcpy((void *)estk_curr_inst_ptr->bip_channel_table[index].recv_buffer,
                         (void *)(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf),
                         estk_curr_inst_ptr->bip_channel_table[index].buffer_size,
                         estk_curr_inst_ptr->bip_channel_table[index].buffer_size,
                         estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len) <
             (size_t)estk_curr_inst_ptr->bip_channel_table[index].buffer_size)
          {
            addi_res = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
            return estk_bip_send_terminal_response(cmd_ref_id,
                                                   GSTK_SEND_DATA_CNF,
                                                   GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                                   &additional_info,
                                                   NULL,
                                                   index);
          }

          estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len =
            estk_curr_inst_ptr->bip_channel_table[index].buffer_size;


          UIM_MSG_HIGH_0("Copy overflow scws to receive buffer");
          if(gstk_memcpy(
               (void *)estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf,
               (void *)(estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf + estk_curr_inst_ptr->bip_channel_table[index].buffer_size),
               estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len - estk_curr_inst_ptr->bip_channel_table[index].buffer_size,
               estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len,
               estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len) <
             estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len - estk_curr_inst_ptr->bip_channel_table[index].buffer_size)
          {
            addi_res = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
            return estk_bip_send_terminal_response(cmd_ref_id,
                                                   GSTK_SEND_DATA_CNF,
                                                   GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
                                                   &additional_info,
                                                   NULL,
                                                   index);
          }

          estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len -=
            (uint16)estk_curr_inst_ptr->bip_channel_table[index].buffer_size;

          UIM_MSG_HIGH_2("scws_overflow_buf_len= %d, recv_buf_len = %d",
                         estk_curr_inst_ptr->bip_channel_table[index].scws_overflow_buf_len,
                         estk_curr_inst_ptr->bip_channel_table[index].buffer_size);
        }
        data_avail.evt_ind.data_avail_evt_ind.ch_status.ch_id
          = estk_curr_inst_ptr->bip_channel_table[index].channel_id;
        data_avail.evt_ind.data_avail_evt_ind.ch_status.is_scws_ch = TRUE;
        data_avail.evt_ind.data_avail_evt_ind.ch_status.scws_ch_status =
        (gstk_scws_ch_state_enum_type)((uint32)estk_curr_inst_ptr->bip_channel_table[index].status - (uint32)ESTK_CH_SCWS_OFFSET);
        data_avail.evt_ind.data_avail_evt_ind.ch_status.ch_status_additional_info
          = GSTK_CH_STATUS_NO_FURTHER_INFO;
        data_avail.evt_ind.data_avail_evt_ind.ch_status.is_link_est = TRUE;
        data_avail.evt_type = GSTK_DATA_AVAIL_EVT_IND;
        data_avail.evt_ind.data_avail_evt_ind.ch_len
          = estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len;

        UIM_MSG_HIGH_1("ESTK sending data available event with size %d",
                       data_avail.evt_ind.data_avail_evt_ind.ch_len);
        if(gstk_send_envelope_evt_download_command(estk_curr_inst_ptr->gstk_client_id,
                                                   0, &data_avail) != GSTK_SUCCESS)
        {
          UIM_MSG_ERR_0("Sending data available envelope failed");
          return ESTK_ERROR;
        }
      }
      else
      {
        dss_status = dss_async_select(estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                                      DS_READ_EVENT,
                                      &error_num);
        if(dss_status == DSS_ERROR)
        {
          UIM_MSG_ERR_0("dss_async_select failed");
          return ESTK_ERROR;
        }
        else
        {
          UIM_MSG_HIGH_0("dss_async_select succeeded");
          return ESTK_SUCCESS;
        }
      }
    }
    else
    {
      /* If protocol is TCP, attempt continuous read from DATA (ETSI 102 223:
         6.4.29 is not applicable to TCP since its a streaming protocol) if there
         is space available in the receive buffer and buffer size is greater
         than 0xFE and continuous read feature is enabled */
      if((gstk_nv_get_feature_status(
            GSTK_CFG_FEATURE_SLOT_ESTK_ENABLE_BIP_TCP_CONT_READ,
            estk_curr_inst_ptr->slot_id) == TRUE) &&
         (estk_curr_inst_ptr->bip_channel_table[index].transport_protocol == GSTK_PKT_DATA_TCP) &&
         ((estk_curr_inst_ptr->bip_channel_table[index].buffer_size -
           estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len) > 0) &&
         (estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len >= 0xFF))
      {
        dss_status = dss_async_select(
                       estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
                       DS_READ_EVENT,
                       &error_num);

        if(dss_status != DSS_ERROR)
        {
          UIM_MSG_HIGH_0("dss_async_select succeeded");
        }
        else
        {
          UIM_MSG_ERR_0("dss_async_select failed");
        }
      }

      UIM_MSG_HIGH_0("Adjust receive buffer for leftover data");
      memsmove((void *)estk_curr_inst_ptr->bip_channel_table[index].recv_buffer,
               (size_t)estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len,
               (void *)(estk_curr_inst_ptr->bip_channel_table[index].recv_buffer+copy_len),
               (size_t)estk_curr_inst_ptr->bip_channel_table[index].recv_buf_len);
    }
  }
  else
  {
    if(estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data != NULL)
    {
      UIM_MSG_HIGH_0("Receive data mem leak");
      gstk_free(estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data);
      estk_curr_inst_ptr->extra_param.extra_param.receive_data_extra_param.ch_data.data = NULL;
    }
    UIM_MSG_ERR_0("estk_send_terminal_response failed" );
    return ESTK_ERROR;
  }

  return ESTK_SUCCESS;

}

/*===========================================================================
FUNCTION: estk_process_get_ch_status_req

DESCRIPTION:
  Process the Get Channel Status Toolkit Request from the card

PARAMETERS:
  gstk_req_ptr : [Input] gstk GET CHANNEL STATUS cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_get_ch_status_req (
  gstk_cmd_from_card_type *gstk_req_ptr
)
{
  uint8                                 i           = 0;
  gstk_terminal_rsp_extra_param_type    tr_extra_param;

  if(gstk_req_ptr == NULL)
  {
    return ESTK_ERROR;
  }

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  memset(&tr_extra_param, 0x00, sizeof(tr_extra_param));

  tr_extra_param.present = TRUE;
  tr_extra_param.rsp_type = GSTK_GET_CH_STATUS_CNF;

  for(i = 0; i < ESTK_MAX_BIP_CH; i++)
  {
    tr_extra_param.extra_param.get_ch_status_extram_param.ch_status[i].ch_id
      = i + 1;
    if(estk_curr_inst_ptr->bip_channel_table[i].is_scws_ch == TRUE)
    {
      tr_extra_param.extra_param.get_ch_status_extram_param.ch_status[i].is_scws_ch = TRUE;

      tr_extra_param.extra_param.get_ch_status_extram_param.ch_status[i].ch_status_additional_info
        = GSTK_CH_STATUS_NO_FURTHER_INFO;
      tr_extra_param.extra_param.get_ch_status_extram_param.ch_status[i].scws_ch_status =
        (gstk_scws_ch_state_enum_type)((uint32)estk_curr_inst_ptr->bip_channel_table[i].status - (uint32)ESTK_CH_SCWS_OFFSET);
    }
    if(estk_curr_inst_ptr->bip_channel_table[i].status == ESTK_CH_ACTIVE)
    {
      tr_extra_param.extra_param.get_ch_status_extram_param.ch_status[i].is_link_est
        = TRUE;
    }
    else
    {
      tr_extra_param.extra_param.get_ch_status_extram_param.ch_status[i].is_link_est
        = FALSE;
    }

    if(estk_curr_inst_ptr->bip_channel_table[i].status == ESTK_CH_DROPPED)
    {
      tr_extra_param.extra_param.get_ch_status_extram_param.ch_status[i].ch_status_additional_info
        = GSTK_CH_STATUS_LINK_DROPPED;
    }
    else
    {
      tr_extra_param.extra_param.get_ch_status_extram_param.ch_status[i].ch_status_additional_info
        = GSTK_CH_STATUS_NO_FURTHER_INFO;
    }

    tr_extra_param.extra_param.get_ch_status_extram_param.cnt = (i+1);
  }
  return estk_bip_send_terminal_response(
           gstk_req_ptr->hdr_cmd.cmd_detail_reference,
           GSTK_GET_CH_STATUS_CNF,
           GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
           NULL,
           &tr_extra_param,
           ESTK_MAX_BIP_CH);
} /* estk_process_get_ch_status_req */

/*===========================================================================
FUNCTION: estk_process_setup_evt_list_req

DESCRIPTION:
  Process the Set Up Event List Toolkit Request from the card

PARAMETERS:
  gstk_req_ptr : [Input] gstk OPEN CHANNEL cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_setup_evt_list_req (
  gstk_cmd_from_card_type *gstk_req_ptr
)
{
  if(gstk_req_ptr == NULL)
  {
    UIM_MSG_ERR_0("GSTK request ptr is NULL");
    return ESTK_ERROR;
  }
  UIM_MSG_HIGH_1("estk_process_setup_evt_list_req: command_id=0x%x",
                 gstk_req_ptr->hdr_cmd.command_id);

  if(gstk_req_ptr->hdr_cmd.command_id == GSTK_SETUP_EVT_DATA_AVAIL_NOTIFY)
  {
    /*Setting Data Available Event Flag*/
 //   estk_data_avail_evt_flag = TRUE;
  }
  else if(gstk_req_ptr->hdr_cmd.command_id == GSTK_SETUP_EVT_CH_STATUS_NOTIFY)
  {
    /*Setting Channel Status Notify Flag*/
 //   estk_ch_status_evt_flag = TRUE;
  }
  else
  {
    //Should not get here
    UIM_MSG_ERR_1("Unhandled command id %d", gstk_req_ptr->hdr_cmd.command_id);
    return ESTK_ERROR;
  }

  return ESTK_SUCCESS;
} /* estk_process_setup_evt_list_req */

/*===========================================================================
FUNCTION: estk_process_close_ch_req

DESCRIPTION:
  Process the Close Channel Request from the card

PARAMETERS:
  gstk_req_ptr : [Input] gstk CLOSE CHANNEL cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_close_ch_req (
  gstk_cmd_from_card_type *gstk_req_ptr
)
{
  int32                               close_ch_id;
  int32                               i;
  uint32                              mask  = 0;
  int32                               index = -1;
  gstk_status_enum_type               gstk_status;
  gstk_client_id_type                 client_id;
  gstk_additional_info_ptr_type       additional_info;
  uint8                               addi_res;
  gstk_raw_cmd_from_card_type         raw_cmd;
  uint32                              user_data   = 0;
  gstk_io_find_client_with_registered_event_in_param_type find_cli_reg;
  gstk_io_call_client_evt_cb_param_type call_cli_evt_cb;
  gstk_cmd_from_card_type               gstk_scws_cmd;
  boolean                               keep_ch_open = FALSE;
  
  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;

  memset(&raw_cmd, 0x00, sizeof(gstk_raw_cmd_from_card_type));
  memset(&find_cli_reg, 0x00, sizeof(gstk_io_find_client_with_registered_event_in_param_type));
  memset(&call_cli_evt_cb, 0x00, sizeof(call_cli_evt_cb));
  memset(&gstk_scws_cmd, 0x00, sizeof(gstk_cmd_from_card_type));

  if(gstk_req_ptr == NULL)
  {
    UIM_MSG_ERR_0("GSTK request ptr is NULL");
    return ESTK_ERROR;
  }

  UIM_MSG_HIGH_2("estk_process_close_ch_req: cmd_detail_reference: 0x%x, ch_id: 0x%x",
                 gstk_req_ptr->hdr_cmd.cmd_detail_reference,
                 gstk_req_ptr->cmd.close_ch_pro_cmd_req.ch_id);

  close_ch_id = gstk_req_ptr->cmd.close_ch_pro_cmd_req.ch_id;

  for(i = 0; i < ESTK_MAX_BIP_CH; i++)
  {
    if(estk_curr_inst_ptr->bip_channel_table[i].channel_id == close_ch_id)
    {
      if(estk_curr_inst_ptr->bip_channel_table[i].status != ESTK_CH_ACTIVE
         && estk_curr_inst_ptr->bip_channel_table[i].status != ESTK_CH_PENDING
         && estk_curr_inst_ptr->bip_channel_table[i].status != ESTK_CH_OPEN_WAIT
         && estk_curr_inst_ptr->bip_channel_table[i].status != ESTK_CH_DROPPED
         && estk_curr_inst_ptr->bip_channel_table[i].status != ESTK_CH_CLOSE_IN_PROGRESS
         && estk_curr_inst_ptr->bip_channel_table[i].status != ESTK_CH_SCWS_LISTEN
         && estk_curr_inst_ptr->bip_channel_table[i].status != ESTK_CH_SCWS_ESTABLISHED
      )
      {
        UIM_MSG_ERR_1("Invalid channel id 0x%x to close", close_ch_id);
        break;
      }
      else
      {
        index = i;
        UIM_MSG_HIGH_2("Found channel 0x%x to close, index=0x%x",
                       close_ch_id, index);
        break;
      }
    }
  }

  if((index < 0) || (index >= ESTK_MAX_BIP_CH))
  {
    UIM_MSG_HIGH_0("Send error terminal response for close channel");
    additional_info.length = 1;
    addi_res = (uint8)GSTK_CH_ID_INVALID;
    additional_info.additional_info_ptr = &addi_res;
    return estk_bip_send_terminal_response(
              gstk_req_ptr->hdr_cmd.cmd_detail_reference,
              GSTK_CLOSE_CH_CNF,
              GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR,
              &additional_info,
              NULL,
              ESTK_MAX_BIP_CH);
  }

  /* Cache the currently ongoing proactive command */
  estk_curr_inst_ptr->bip_channel_table[index].current_command_id = GSTK_CLOSE_CH_REQ;

  /* Have to handle if display alpha is present */
  if(((gstk_req_ptr->cmd.close_ch_pro_cmd_req.alpha.length) > 0 &&
      (gstk_req_ptr->cmd.close_ch_pro_cmd_req.alpha.alphabet)) ||
     (gstk_nv_get_me_feature_status(GSTK_CFG_FEATURE_ME_ESTK_QMI_SEND_NO_AND_NULL_ALPHA_IND) == TRUE))
  {
    UIM_MSG_HIGH_0("ALPHA TLV present in CLOSE Channel command"  );
    if(gstk_nv_get_me_feature_status(GSTK_CFG_FEATURE_ME_ESTK_IGNORE_ALPHA) == FALSE)
    {
      if(estk_send_alpha_to_display_client(gstk_req_ptr, TRUE) != ESTK_SUCCESS)
      {
        return ESTK_ERROR;
      }
    }
  }
  /*
      If bit1 of qualifier in CLOSE CHANNEL for packet data service is 1, next CAT
      command will be OPEN CHANNEL using same Network Access Name as channel to be closed
  
      This is assuming the close channel is NOT for a UICC server mode channel
    */
    if ((estk_curr_inst_ptr->bip_channel_table[index].transport_protocol != GSTK_PKT_DATA_TCP_UICC_SERVER_MODE) &&
        (gstk_req_ptr->cmd.close_ch_pro_cmd_req.qualifier & 0x01))
    {
      keep_ch_open = TRUE;
    }
  
    if (keep_ch_open)
    {
      /* keep_ch_open_timer */
      if (estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p == NULL)
      {
        boolean mask_set_status = FALSE;
  
        ESTK_BIP_SET_SLOT_INDEX_MASK(
          estk_curr_inst_ptr->slot_id, index, mask, mask_set_status);
  
        if(mask_set_status)
        {
          estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p =
            (rex_timer_type *)GSTK_CALLOC(sizeof(rex_timer_type));
  
          UIM_MSG_HIGH_1("keep_ch_open_timer_p malloc-ed, ptr = 0x%x",
                         estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p);
          if (estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p != NULL)
          {
            rex_def_timer_ex(estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p,
                             estk_bip_keep_ch_open_timer_cb,
                             mask);
          }
          else
          {
            return ESTK_ERROR;
          }
        }
      }
      if (estk_curr_inst_ptr->bip_channel_table[index].keep_ch_open_timer_p != NULL)
      {
        UIM_MSG_HIGH_1("estk_process_close_ch_req: Keep CH %d open",
                       index);
  
        estk_curr_inst_ptr->keep_ch_open = TRUE;
        estk_curr_inst_ptr->ch_open_id = index;
  
        /* If sock_fd is valid, close the socket because it might not be reused */
        if (estk_curr_inst_ptr->bip_channel_table[index].sock_fd != DSS_ERROR)
        {
          UIM_MSG_HIGH_0("estk_process_close_ch_req: close existing socket");
  
          estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_CLOSE_IN_PROGRESS;
  
          if(estk_bip_process_sock_close_evt(estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
             gstk_req_ptr->hdr_cmd.cmd_detail_reference) == ESTK_SUCCESS)
          {
            return ESTK_SUCCESS;
          }
          else
          {
            UIM_MSG_ERR_0("estk_bip_process_sock_close_evt failed");
            return ESTK_ERROR;
          }
        }
  
        if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
        {
          UIM_MSG_HIGH_0("Saving CLOSE CHANNEL TR.");
          estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
          estk_curr_inst_ptr->result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
          return ESTK_SUCCESS;
        }
        else
        {
          return estk_bip_send_terminal_response(
                   gstk_req_ptr->hdr_cmd.cmd_detail_reference,
                   GSTK_CLOSE_CH_CNF,
                   GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
                   NULL,
                   NULL,
                   index);
        }
      }
    }

  if(estk_curr_inst_ptr->bip_channel_table[index].is_scws_ch == FALSE)
  {
    if((estk_curr_inst_ptr->bip_channel_table[index].on_demand_link == TRUE)
       && (estk_curr_inst_ptr->bip_channel_table[index].status == ESTK_CH_PENDING))
    {
      UIM_MSG_HIGH_0("Close channel for on-demand in pending state");

      if(estk_curr_inst_ptr->state == ESTK_WAIT_FOR_DISPLAY_ALPHA_CNF_ST)
      {
        UIM_MSG_HIGH_0("Saving CLOSE CHANNEL  TR");
        estk_curr_inst_ptr->state = ESTK_PENDING_TR_ST;
        estk_curr_inst_ptr->result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
        return ESTK_SUCCESS;
      }
      else
      {
        estk_bip_clean_up_ch_table(index);
        return estk_bip_send_terminal_response(
                 gstk_req_ptr->hdr_cmd.cmd_detail_reference,
                 GSTK_CLOSE_CH_CNF,
                 GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
                 NULL,
                 NULL,
                 index);
      }
    }

    estk_curr_inst_ptr->bip_channel_table[index].status = ESTK_CH_CLOSE_IN_PROGRESS;

    /*
      sock_fd is only valid for legacy open channel. For DNS query open channel
      we do not perform net_connect() so we do not need to close any sockets
    */
    if(estk_curr_inst_ptr->bip_channel_table[index].sock_fd == DSS_ERROR)
    {
      /* No additional info to be sent in success case */
      additional_info.length = 0;
      additional_info.additional_info_ptr = NULL;

      return estk_bip_process_sock_close_error(index,
                                               gstk_req_ptr->hdr_cmd.cmd_detail_reference,
                                               GSTK_COMMAND_PERFORMED_SUCCESSFULLY,
                                               &additional_info,
                                               NULL);
    }
    else if(estk_bip_process_sock_close_evt(estk_curr_inst_ptr->bip_channel_table[index].sock_fd,
         gstk_req_ptr->hdr_cmd.cmd_detail_reference) == ESTK_SUCCESS)
    {
      return ESTK_SUCCESS;
    }
    else
    {
      UIM_MSG_ERR_0("estk_bip_process_sock_close_evt failed");
      return ESTK_ERROR;
    }
  }
  else
  {
    find_cli_reg.cli_func_type = GSTK_HANDLE_DISPLAY_FUNC_ONLY;
    find_cli_reg.cmd_type = GSTK_SCWS_CLOSE_CH_REQ;
    find_cli_reg.format_type = GSTK_GSTK_FORMAT;
    gstk_status = gstk_io_ctrl(
                  GSTK_IO_FIND_CLIENT_WITH_REGISTERED_EVENT,
                  (void *)&find_cli_reg,
                  (void *)&client_id);
    if(gstk_status != GSTK_SUCCESS)
    {
      UIM_MSG_HIGH_0("Did not find any clients registered for GSTK_SCWS_CLOSE_CH_REQ");
      return ESTK_ERROR;
    }
    else
    {
      call_cli_evt_cb.client_id = client_id;
      call_cli_evt_cb.gstk_req_ptr = (gstk_cmd_from_card_type*)&gstk_scws_cmd;
      gstk_scws_cmd.hdr_cmd.command_id = GSTK_SCWS_CLOSE_CH_REQ;
      gstk_scws_cmd.hdr_cmd.cmd_detail_reference = estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference;
      gstk_scws_cmd.hdr_cmd.sim_slot_id =  estk_curr_inst_ptr->curr_cmd.hdr_cmd.sim_slot_id;
      gstk_scws_cmd.hdr_cmd.command_number = estk_curr_inst_ptr->curr_cmd.hdr_cmd.command_number;
      gstk_scws_cmd.hdr_cmd.user_data = estk_curr_inst_ptr->curr_cmd.hdr_cmd.user_data;

      gstk_scws_cmd.cmd.scws_close_ch_cmd_req.ch_id         = close_ch_id;
      gstk_scws_cmd.cmd.scws_close_ch_cmd_req.channel_state =
        (gstk_scws_ch_state_enum_type)(estk_curr_inst_ptr->curr_cmd.hdr_cmd.cmd_detail_reference & 0x1);

      if (gstk_io_ctrl(
            GSTK_IO_CALL_CLIENT_EVT_CB,
            (void *)&call_cli_evt_cb,
            (void *)&user_data /* dummy data*/) != GSTK_SUCCESS)
      {
        UIM_MSG_ERR_0("can't call cli evt cb!");
        return ESTK_ERROR;
      }
    }
  }
  return ESTK_SUCCESS;
} /* estk_process_close_ch_req */

/*===========================================================================
FUNCTION: estk_process_cellular_oos_evt

DESCRIPTION:
  Processes the Cellular OOS event received from Data Services (via GSTK).
  If Location Status (from MM IDLE/CM SS event) is Normal Service,
  function starts a 10 sec timer to wait for Location status to change,
  else sends Link Dropped Envelope as required.

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_cellular_oos_evt(void)
{
  uint32                         ch_idx      = 0;
  gstk_slot_id_enum_type         slot       = GSTK_SLOT_ID_MAX;
  gstk_location_status_enum_type loc_status = GSTK_NO_SERVICE;

  UIM_MSG_HIGH_0("In estk_process_cellular_oos_evt");

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  slot = estk_curr_inst_ptr->slot_id;

  if (gstk_io_ctrl(GSTK_IO_GET_CURR_LOC_STATUS,
                   (void *)&slot,
                   (void *)&loc_status) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_0("Failed to get loc status");
    return ESTK_ERROR;
  }

  /* If Locaton Status is still Normal Service, start 10 sec timer to wait for
     location status to change */
  if (loc_status == GSTK_NORMAL_SERVICE)
  {
    UIM_MSG_HIGH_1("Still in service, start link drop timer, duration: 0x%x",
                   ESTK_OOS_TIMER_DURATION);

    (void)rex_clr_timer(&(estk_curr_inst_ptr->cellular_oos_timer));
    (void)rex_set_timer(&(estk_curr_inst_ptr->cellular_oos_timer),
                        ESTK_OOS_TIMER_DURATION);
  }
  else
  {
    for(ch_idx = 0; ch_idx < ESTK_MAX_BIP_CH; ch_idx++)
    {
      UIM_MSG_HIGH_3("Channel Table Index: 0x%x, Status: 0x%x, NW type: 0x%x",
                      ch_idx,
                      estk_curr_inst_ptr->bip_channel_table[ch_idx].status,
                      estk_curr_inst_ptr->bip_channel_table[ch_idx].nw_type);

      UIM_MSG_HIGH_2("NW type: 0x%x, Slot 0x%x",
                     estk_curr_inst_ptr->bip_channel_table[ch_idx].nw_type,
                     estk_curr_inst_ptr->slot_id);

      /* Process Cellular OOS event only for Channels open over Cellular */
      if (estk_curr_inst_ptr->bip_channel_table[ch_idx].nw_type != ESTK_NETWORK_CELLULAR)
      {
        continue;
      }

      UIM_MSG_HIGH_1("Current Command: 0x%x",
                     estk_curr_inst_ptr->bip_channel_table[ch_idx].current_command_id);

      switch (estk_curr_inst_ptr->bip_channel_table[ch_idx].current_command_id)
      {
        case GSTK_OPEN_CH_REQ:
        case GSTK_CLOSE_CH_REQ:
          /* OPEN/CLOSE CHANNEL is in progress */
          if (estk_curr_inst_ptr->bip_channel_table[ch_idx].status == ESTK_CH_ACTIVE)
          {
            estk_curr_inst_ptr->bip_channel_table[ch_idx].status = ESTK_CH_DROPPED;
          }
          break;
        case GSTK_SEND_DATA_REQ:
          return estk_bip_process_send_data_error(ch_idx);
        default:
          if ((estk_curr_inst_ptr->bip_channel_table[ch_idx].status == ESTK_CH_ACTIVE) ||
              ((estk_curr_inst_ptr->bip_channel_table[ch_idx].status == ESTK_CH_PENDING) &&
               (estk_curr_inst_ptr->bip_channel_table[ch_idx].on_demand_link == TRUE)))
          {
            UIM_MSG_HIGH_0("send link drop envelope cmd");
            estk_curr_inst_ptr->bip_channel_table[ch_idx].status = ESTK_CH_DROPPED;

            return estk_bip_send_ch_status_event(
                     estk_curr_inst_ptr->bip_channel_table[ch_idx].channel_id,
                     GSTK_CH_STATUS_LINK_DROPPED);
          }
      }
    }
  }

  return ESTK_SUCCESS;
} /* estk_process_cellular_oos_evt */

/*===========================================================================
FUNCTION: estk_process_cellular_status_change_evt

DESCRIPTION :
  This function processes a Cellular Status Change notification from GSTK

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_cellular_status_change_evt(
  const estk_cmd_type *cmd_ptr
)
{
  if(cmd_ptr == NULL)
  {
    return ESTK_ERROR;
  }

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  UIM_MSG_HIGH_2("In estk_process_cellular_status_change_evt, Cellular status: 0x%x, Slot: 0x%x",
                 cmd_ptr->cmd_data.cellular_status_evt.cellular_service_available,
                 estk_curr_inst_ptr->slot_id);

  /* If DS indicates that Cellular Service has become available and Location
     status has always been Normal Service, stop the OOS timer */
  if(cmd_ptr->cmd_data.cellular_status_evt.cellular_service_available)
  {
    if(rex_get_timer(&(estk_curr_inst_ptr->cellular_oos_timer)) > 0)
    {
      UIM_MSG_HIGH_0("Clear running OOS timer");
      (void) rex_clr_timer(&(estk_curr_inst_ptr->cellular_oos_timer));
    }
  }
  else
  {
    estk_process_cellular_oos_evt();
  }

  return ESTK_SUCCESS;
} /* estk_process_cellular_status_change_evt */

/*===========================================================================
FUNCTION:   estk_open_ch_cleanup

DESCRIPTION:
  This ESTK function clears the open channel related private data.

PARAMETERS:
  None

DEPENDENCIES:
  open_ch_ptr : [Input] Open Channel pointer to clean-up

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_open_ch_cleanup(
  gstk_open_ch_req_type *open_ch_ptr
)
{
  if(open_ch_ptr == NULL)
  {
    UIM_MSG_ERR_0("estk_open_ch_ptr is NULL");
    return;
  }

  if(open_ch_ptr->alpha.alphabet != NULL)
  {
    gstk_free(open_ch_ptr->alpha.alphabet);
    open_ch_ptr->alpha.alphabet = NULL;
  }

  if(open_ch_ptr->access_point_name.alphabet != NULL)
  {
    gstk_free(open_ch_ptr->access_point_name.alphabet);
    open_ch_ptr->access_point_name.alphabet = NULL;
  }

  if(open_ch_ptr->local_address.pkt_address.address != NULL)
  {
    gstk_free(open_ch_ptr->local_address.pkt_address.address);
    open_ch_ptr->local_address.pkt_address.address = NULL;
  }

  if(open_ch_ptr->data_dest_address.pkt_address.address != NULL)
  {
    gstk_free(open_ch_ptr->data_dest_address.pkt_address.address);
    open_ch_ptr->data_dest_address.pkt_address.address = NULL;
  }

  if(open_ch_ptr->text_string_user_login.alphabet != NULL)
  {
    gstk_free(open_ch_ptr->text_string_user_login.alphabet);
    open_ch_ptr->text_string_user_login.alphabet = NULL;
  }

  if(open_ch_ptr->text_string_user_passwd.alphabet != NULL)
  {
    gstk_free(open_ch_ptr->text_string_user_passwd.alphabet);
    open_ch_ptr->text_string_user_passwd.alphabet = NULL;
  }
} /*estk_open_ch_cleanup*/

/*===========================================================================
FUNCTION:   estk_clean_up_bip_open_ch_timer_exp

DESCRIPTION:
  This ESTK function cleans up ESTK globals on open channel timer expiry

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_clean_up_bip_open_ch_timer_exp(void)
{
  if(estk_curr_inst_ptr == NULL)
  {
    return ESTK_ERROR;
  }

  if((estk_curr_inst_ptr->curr_bip_cmd_index < 0)
     || (estk_curr_inst_ptr->curr_bip_cmd_index >= ESTK_MAX_BIP_CH))
  {
    UIM_MSG_ERR_0("Invalid index to clean up");
    return ESTK_ERROR;
  }
  estk_bip_clean_up_ch_table(estk_curr_inst_ptr->curr_bip_cmd_index);
  return ESTK_SUCCESS;
}
/*estk_clean_up_bip_open_ch_timer_exp*/

/*===========================================================================
FUNCTION:   estk_clean_up_bip_send_data_timer_exp

DESCRIPTION:
  This ESTK function cleans up send data buffer on send data timer expiry

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_clean_up_bip_send_data_timer_exp(void)
{
  int8 index = 0;

  ESTK_RETURN_ERROR_IF_NULL_PTR(estk_curr_inst_ptr, ESTK_ERROR);

  index = estk_curr_inst_ptr->curr_bip_cmd_index;

  if((index < 0) || (index >= ESTK_MAX_BIP_CH))
  {
    return ESTK_ERROR;
  }

  /* Reset current command as SEND DATA is no longer being processed */
  estk_curr_inst_ptr->bip_channel_table[index].current_command_id = GSTK_CMD_ENUM_NOT_USE;

  if(estk_curr_inst_ptr->curr_cmd.cmd.send_data_pro_cmd_req.send_data_immediately)
  {
    estk_curr_inst_ptr->bip_channel_table[index].send_data_timer_expired = TRUE;
  }
  return ESTK_SUCCESS;
}
/*estk_clean_up_bip_send_data_timer_exp*/

/*===========================================================================
FUNCTION:   estk_bip_full_clean_up

DESCRIPTION:
  This ESTK function cleans up entire BIP table

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_full_clean_up(void)
{
  uint8                                     i = 0;

  for (i=0; i< ESTK_MAX_BIP_CH; i++)
  {
    estk_bip_clean_up_ch_table(i);
  }
}
/*estk_bip_full_clean_up*/

/*===========================================================================
FUNCTION:   estk_bip_full_clean_up_slot

DESCRIPTION :
  Free memory allocated for the BIP channel

PARAMETERS:
  slot_id : [Input] slot on which the BIP channel to be cleaned

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_full_clean_up_slot(gstk_slot_id_enum_type slot_id)
{
  uint8 i = 0;

  UIM_MSG_HIGH_1("estk_bip_full_clean_up_slot: slot_id=0x%x", slot_id);

  if (!ESTK_IS_VALID_SLOT_ID(slot_id))
  {
    return;
  }

  ESTK_RETURN_IF_NULL_PTR(estk_instances_ptr[(uint32)slot_id - 1]);

  for (i = 0; i < ESTK_MAX_BIP_CH; i++)
  {
    estk_bip_clean_up_ch_table(i);
  }
  estk_instances_ptr[(uint32)slot_id - 1]->curr_bip_cmd_index = -1;
}/* estk_bip_full_clean_up_slot */

/*===========================================================================
FUNCTION: estk_uim_is_bip_session_in_progress

DESCRIPTION:
  Return status indicating any BIP session is in progress on the slot requested

PARAMETERS:
  slot_id : [Input] slot on which the BIP session status is requested

DEPENDENCIES:
  None

RETURN VALUE:
  Return
  TRUE:  BIP session in progress
  FALSE: No BIP session in progress

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
boolean estk_uim_is_bip_session_in_progress(
  gstk_slot_id_enum_type slot_id
)
{
  boolean estk_bip_session_in_progress = FALSE;
  uint16 i = 0;

  UIM_MSG_HIGH_1("estk_uim_is_bip_session_in_progress: slot_id: 0x%x",
                 slot_id);

  if (!ESTK_IS_VALID_SLOT_ID(slot_id))
  {
    return FALSE;
  }

  if(estk_instances_ptr[(uint32)slot_id - 1] == NULL)
  {
    UIM_MSG_ERR_1("ESTK instance: 0x%x is NULL",
                  ((uint32)slot_id - 1));
    return FALSE;
  }

  for (i = 0; i < ESTK_MAX_BIP_CH; i++)
  {
    if ((estk_instances_ptr[(uint32)slot_id - 1]->bip_channel_table[i].status !=
         ESTK_CH_CLOSED) &&
        (estk_instances_ptr[(uint32)slot_id - 1]->bip_channel_table[i].status !=
         ESTK_CH_DROPPED))
    {
      /* Even if one channel is not CLOSED or DROPPED
         implies a BIP session is active. So, STATUS command
         cannot return with FCP or DF */
      estk_bip_session_in_progress = TRUE;
      UIM_MSG_HIGH_1("BIP session in progress is %d",
                     estk_bip_session_in_progress);
      break;
    }
  }
  return estk_bip_session_in_progress;
}

/*===========================================================================
FUNCTION: estk_bip_reset_curr_cmd_id_for_channel

DESCRIPTION:
  Clears the current command ID in the BIP channel table

PARAMETERS:
  index:      [Input] Index of BIP channel table for which the Current
                      command ID needs to be cleared. Clear only if index
                      is in valid range

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_reset_curr_cmd_id_for_channel(uint32 index)
{
  ESTK_RETURN_IF_NULL_PTR(estk_curr_inst_ptr);

  if(index < ESTK_MAX_BIP_CH)
  {
    estk_curr_inst_ptr->bip_channel_table[index].current_command_id =
      GSTK_CMD_ENUM_NOT_USE;
  }
} /* estk_bip_reset_curr_cmd_id_for_channel */

/*===========================================================================

FUNCTION estk_bip_send_terminal_response

DESCRIPTION:
  This function is served as a utility function to the estk client, so that
  the client can call this function in sending a terminal response to
  a BIP proactive command.

  The function resets the current command ID stored in the BIP channel table
  index (only if a valid index is passed in).

PARAMETERS:
  cmd_detail_ref_id:      [Input] Specifies the reference ID for command details of
                                  the proactive command that the client is sending
                                  a terminal response to.  The reference id value
                                  is sent to the client earlier in the structure
                                  of the proactive command request, inside the
                                  gstk_exp_hdr_type
  terminal_response_type: [Input] Specifies the _CNF for the response to
                                  the proactive cmd that the client wants to
                                  notify SIM/USIM
  general_result:         [Input] Specifies the result of the proactive command
                                  operation
  additional_result_ptr:  [Input] Pointer specifies the optional additional result
                                  for the proactive command, the length field in
                                  the gstk_additonal_info_type specifies number of
                                  additional_info in the structure
  other_info_ptr:         [Input] A union pointer of various extra parameters for the
                                  various proactive command response, please refer to
                                  gstk_terminal_rsp_param_type for details
  bip_channel_index:      [Input] Index of BIP channel table for which the Current
                                  command ID needs to be cleared. Clear only if index
                                  is in valid range

DEPENDENCIES:
  None

RETURN VALUE:
  estk_status_enum_type:
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  This function should only be called after estk_client_init has been called

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_bip_send_terminal_response (
  uint32                                     cmd_detail_ref_id,
  gstk_cmd_enum_type                         terminal_response_type,
  gstk_general_result_enum_type              general_result,
  gstk_additional_info_ptr_type              *additional_result_ptr,
  gstk_terminal_rsp_extra_param_type         *other_info_ptr,
  uint32                                     bip_channel_index)
{
  UIM_MSG_HIGH_1("estk_bip_send_terminal_response, index: 0x%x",
                 bip_channel_index);

  /* Clear current command ID in the BIP channel table if a valid index is
     provided - since TR is being sent, command is no longer is progress */
  estk_bip_reset_curr_cmd_id_for_channel(bip_channel_index);

  /* Send the Terminal Response */
  return estk_send_terminal_response(
           cmd_detail_ref_id,
           terminal_response_type,
           general_result,
           additional_result_ptr,
           other_info_ptr);
} /* estk_bip_send_terminal_response */

/*===========================================================================

FUNCTION: estk_notify_proactive_command

DESCRIPTION :
  This function allows GSTK to notify ESTK whenever a proactive command is
  received from the card

PARAMETERS:
  type_of_command : [Input] Type of proactive command
  slot_id         : [Input] Slot ID on which the proactive command is received

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void estk_notify_proactive_command(
  uint8                  type_of_command,
  gstk_slot_id_enum_type slot_id
)
{
  int32   index           = -1;

  UIM_MSG_HIGH_2("estk_notify_proactive_command, command=0x%x, slot_id=0x%x",
                 type_of_command, slot_id);

  if (!ESTK_IS_VALID_SLOT_ID(slot_id))
  {
    UIM_MSG_ERR_1("Invalid Slot ID: 0x%x",slot_id);
    return;
  }

  if(estk_instances_ptr[(uint32)slot_id - 1] == NULL)
  {
    UIM_MSG_ERR_1("ESTK global instance for slot: 0x%x is invalid",slot_id);
    return;
  }

  /* If currently a BIP channel is kept open, but the subsequent CAT command
     is not an OPEN CHANNEL, the channel needs to be closed */
  if((type_of_command != GSTK_CMD_STK_END_OF_PROACTIVE_SES) &&
     (type_of_command != GSTK_CMD_STK_OPEN_CHANNEL))
  {
    index = estk_instances_ptr[(uint32)slot_id - 1]->ch_open_id;
  }
} /* estk_notify_proactive_command */


/*===========================================================================
FUNCTION: estk_bip_is_channel_closed

DESCRIPTION:
  Return status indicating if the channel is closed with the channel ID 
  passed and on the slot requested

PARAMETERS:
  ch_id   : [Input] channel ID of the BIP channel
  slot_id : [Input] slot on which the BIP session status is requested

DEPENDENCIES:
  None

RETURN VALUE:
  Return
  TRUE:  Channel with provided channel ID is closed
  FALSE: Channel with provided channel ID is open

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
boolean estk_bip_is_channel_closed(
  int32                  ch_id,
  gstk_slot_id_enum_type slot_id
)
{
  uint16 i = 0;

  if ((!ESTK_IS_VALID_SLOT_ID(slot_id))||
      (estk_instances_ptr[(uint32)slot_id - 1] == NULL))
  {
    return TRUE;
  }

  for (i = 0; i < ESTK_MAX_BIP_CH; i++)
  {
    if(estk_instances_ptr[(uint32)slot_id - 1]->bip_channel_table[i].channel_id == ch_id)
    {
      if (estk_instances_ptr[(uint32)slot_id - 1]->bip_channel_table[i].status !=
         ESTK_CH_CLOSED)
      {
        return FALSE;
      }
      else
      {
        break;
      }
    }
  }    
  return TRUE;
}

#endif /* FEATURE_ESTK */
