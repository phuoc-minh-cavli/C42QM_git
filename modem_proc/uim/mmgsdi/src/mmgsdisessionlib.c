/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        M M G S D I  S E S S I O N  L I B R A R Y


GENERAL DESCRIPTION

  This file contains Library function that MMGSDI client can call to access
  various SIM/USIM/RUIM functions

  mmgsdi_read_transparent
    Read data from a Transparent file

  mmgsdi_read_record
    Read data from a record in a Linear Fixed or Cyclic file

  mmgsdi_write_transparent
    Write data into a Transparent file

  mmgsdi_write_record
    Write data into a record Linear Fixed or Cyclic file

  mmgsdi_get_file_attr
    Get File Attribute information for the files

  mmgsdi_send_card_status
    Send a Status Command to the card for Detection Purposes

  mmgsdi_verify_pin
    Verify the PIN with the card

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2009 - 2019 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdisessionlib.c#1 $$ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
08/22/19   sch     Send SELECT ISDR to determine if card is eUICC or not
12/24/18   vgd     Don't wait for ok init from a cient deregistered for refresh
12/17/18   tq      Replace deprecated APIs with its sync API in UIM code
12/11/18   tq      F3 reduction
10/29/18   tq      Acquire app critical section before run eons algo
10/31/18   tq      Removal of MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_REQ
10/30/18   rps     Change steering of roaming refresh handling
10/15/18   tq      Fix mem leak in get operator sync call
07/09/18   tq      UIM interface optimization
09/10/17   rps     Build flag for Simlock in IOE devices
09/28/16   sn      Add upper bound length check in read/write APIs 
06/22/16   tkl     Stub out additional APIs
06/21/16   ar      Moving compute_ip_auth handler to 3GPP2 specific file
06/15/16   sp      Remove command type parameter from validate_client_id function
06/03/16   ks      Remove the feature flag UIM_TEMP_DISABLED_POLL_FR
05/26/16   bcho    EONS F3 reduction
05/24/16   ar      Review of macros used by MMGSDI
05/12/16   vdc     Added stub file for hidden key functionality
05/02/16   ar      Snip out 3GPP2 code in new file
04/25/16   dd      Removed mmgsdi_session_get_file_enum_from_path_sync
04/25/16   bcho    Code optimization for READ, WRITE and SERVICE enable/disable APIs
04/19/16   ssr     Fix compilation error
04/14/16   vdc     Added external personlization files for IOE flavor
04/13/16   ar      Remove duplicates from mmgsdi_util_print_file_information()
03/09/16   vdc     Removed JCDMA related code
03/09/16   vdc     Remove bcast related code
03/08/16   sp      Fix compilation warnings
03/08/16   sp      Remove length information from requests and confirmations
03/07/16   ar      Remove ICC seek support
11/05/15   ks      Rel12 polling enhancement
09/22/15   lm      Fix KW errors
02/23/15   vdc     Allow reading of record based EFs from cache in caller context
12/04/14   ar      Allow cache access using card_session in client context
09/09/14   kk      Enhancements to NV handling
08/16/14   lj      Optimize app capabilities with cached data in client context
08/11/14   bcho    Optimize srv req by using cached srv table in client context
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/15/14   yt      Support for NV refresh
07/11/14   vv      Fix for compiler warnings
07/10/14   tl      Remove ALWAYS OFF feature FEATURE_MMGSDI_PKCS15
06/30/14   vv      Remove Perso code
04/01/14   av      NVRUIM headers cleanup
03/27/14   yt      Ensure usage of correct slot id
02/25/14   yt      API to support SELECT AID request with select mode
02/25/14   hh      Purge temporary mobile identities during UICC Reset Refresh
02/10/14   ar      Replace session checks with utility functions
01/28/14   vdc     Optimize read trans req to read from cache before SIM init
12/05/13   tkl     SFI Support and decouple uim file enums
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
12/18/13   tl      Prevent initalizing the cache with partial files
12/16/13   yt      Support for ISIM AUTH in HTTP digest security context
12/01/13   df      Convert to use prov session type to session ID util
11/08/13   yt      Reduce size of MMGSDI tables
10/17/13   at      Support for MMGSDI Cancel APIs
10/04/13   rp      F3 message reduction
10/02/13   yt      Remove duplicate code
08/30/13   tl      Convert channel info table from static to dynamic allocation
08/14/13   av      Optimize read transparent req by reading from cache directly
08/07/13   tl      Add CDMA AKA Authenticate functionality
06/25/13   yt      New AUTH APIs with option to ignore app state
06/24/13   vdc     Added support for triple SIM
05/16/13   vdc     Replace memcpy with safer version memscpy
04/16/13   tl      Clean up of radio access features from MMGSDI code
04/15/13   tl      Remove obsoleted featurization
04/04/13   bcho    Set internal_deact_req to FALSE for external cmd queuing
03/04/13   av      Updates for T3245 timer support to mark APP as legal/valid
02/07/13   yt      Support for activation/deactivation of EFs
01/21/13   vv      Added support to update EF-ACSGL record
12/26/12   av      Do not allow writing to file that is being refreshed
10/13/12   abg     Added check in mmgsdi_session_usim_authenticate() to allow
                   only USIM Auth Context requests
10/03/12   abg     Removed Queuing related F3 message
09/28/12   abg     Updated ERR to MSG_ERROR
09/25/12   av      New MMGSDI API mmgsdi_session_get_file_attr_ext()
09/25/12   av      Skip reading ARR for internal selects
09/11/12   tl      Fix KW errors
09/11/12   bcho    Added definition for Session Get Operator Name Extentsion API
08/16/12   at      Support for new API mmgsdi_session_register_all_for_refresh
08/16/12   tl      Add mmgsdi_session_close_terminate() to support
08/10/12   av      Removed RPC support along with RPC style comments
07/25/12   tl      Fixed otasp ms key for a_key_p_rev greater than 2
05/23/12   vv      Added support for file update notification registration
05/10/12   tl      Change first parameter in alignment check for
                   mmgsdi_session_otasp_ms_key_req()
04/18/12   bcho    mmgsdi_util_is_session_id_valid function call is replaced by
                   mmgsdi_util_is_session_id_valid_in_cmd in
                   mmgsdi_session_get_operator_name()
04/18/12   bcho    Crit Sect leave function called before returning from
                   mmgsdi_session_get_operator_name()
04/12/12   bcho    Crit Sect Enter/Leave macro used in Get Operator Name API
04/02/12   bcho    Check added for incorrect param in
                   mmgsdi_session_get_operator_name API
03/29/12   shr     Added support for Session Open with MF,
                   support for non-prov. app selection using zero length and
                   partial AID
03/20/12   yt      Add request type for session_open_with_select_rsp
03/20/12   shr     Request FCP when Sesion Open is requested
                   through mmgsdi_session_open
03/01/12   kk      Added support for app state extraction in get_session_info
03/01/12   bcho    slot_id reference is removed from function
                   mmgsdi_session_get_app_capabilities_sync
02/23/12   shr     Added support for Session Open with MF
02/23/12   bcho    Critical Section Enter/Leave function calls replaced by
                   respective macro to add logging
01/30/12   bcho    Featurization for get operator name fusion API added
12/23/11   shr     Removed featurization for ALWAYS ON features
12/21/11   bcho    Legacy GSDI removal updates
12/21/11   bcho    Changes for new API to open multiple sessions
                   in a single request
12/21/11   bcho    Added support for Get Operator Name API
11/30/11   at      Added check for file path len in refresh deregister request
11/23/11   at      Added checks for input pointers in compute ip auth request
10/28/11   yt      Added new files for cache-related utility functions
10/28/11   shr     Added additional logging for Session Run Cave Algo
10/13/11   nb      Support for new Refresh stage
09/28/11   tkl     Fixed write cache for TMSI PLMN check
08/05/11   av      UIMDiag now doesn't return error if search pattern is null;
                   It's handled in mmgsdi now.
08/02/11   av      Fixed incorrect slot_id being sent out in the session_open
                   and session_close cnfs
08/01/11   vs      Updates for key ref based PIN operations
08/01/11   vs      Added support for session open with select response
06/16/11   yt      Thread safety updates for INCREASE API
05/23/11   yt      Added support for INCREASE request
05/23/11   shr     Clean up Get File Attr handling
05/17/11   shr     Updates for Thread safety
05/13/11   nmb     Support for ISIM GBA authentication
04/22/11   vs      Support for subscription ok request
04/18/11   nmb     Fix for fusion rpc compilation error
04/08/11   js      Add debug messages for refresh on fusion
04/01/11   yt      Verify validity of path for file access
03/31/11   yt      Fixed compilation warning
03/30/11   shr     Fixed client ID retrieval logic in Compute IP Auth request
03/11/11   js      Support for cache read/write functions for type II fusion
02/21/11   kk/ms   Added sync call for get app capabilities
02/08/11   ssr     Fixed PRL read for JCDMA case
02/03/11   ms      Updated F3 messages for Client ID
02/02/11   ssr     Fixed compilation warning
01/31/11   ssr     Decoupled FEATURE_UIM_JCDMA_RUIM_SUPPORT feature
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
10/27/10   shr     Fixed input data length check when handling an
                   OTASP block request
10/18/10   shr     Fixed input data length check when handling a
                   Compute IP Auth request
10/18/10   shr     Fixed input data length check when handling a
                   Run GSM Algorithm request
10/07/10   shr     Fixed input data length check when handling an OTA Deperso
10/05/10   nmb     Removed extra bytes from task_cmd buffer
09/10/10   ps      Merge UIM Driver DSDS changes
09/08/10   nmb     mmgsdi_task_cmd_type memory reduction
09/07/10   yt      Check MF identifier before accessing file by path
08/20/10   yt      Added featurization for nvruim_3gpd_mip_svc and nvruim_3gpd_mip_svc
08/13/10   js      Updated mmgsdi_session_get_info_sync() for Fusion
08/10/10   js      Updated nvruim get data sync to support 3gpd control data
08/04/10   shr     Added support for session based UIM power control API
07/14/10   js      Fixed compiler warnings
07/01/10   nmb     Updates for JCDMA CSIM support
06/17/10   js      Added support for mmgsdi_session_get_nvruim_data_sync
05/21/10   tkl     Fixed perso key with wrong length not decrement counter of attempts
05/17/10   nmb     Added pin length to mmgsdi_util_is_pin_valid calls
05/18/10   shr     File access operations need to be allowed on Card Sessions
                   irrespective of the provisioning status
05/14/10   yt      Replaced feature_activate_req with feature_deactivate_req
                   in mmgsdi_session_perso_deactivate()
04/28/10   nmb     Check record file length in cache write function
04/27/10   shr     Added Hiddenkey support
03/30/10   yt      Added functionality for FEATURE_MMGSDI_OP_MODE_SLAVE
03/29/10   adp     Disabling the featurization for function mmgsdi_session_perso_get_key
03/29/10   shr     Switch/deactivate provisioning clean up and redesign
03/26/10   kk      Fixed RUN GSM Algo handling
03/15/10   mib     Added linear fixed EFs to cache write function
03/09/10   shr     Clean up REFRESH FCN, INIT, INIT_FCN and APP_RESET
02/19/10   shr     Session get info API implementation
02/15/10   kk      Added get app info sync api
02/08/10   ssr     Fixed MS Key Request for Parameter P or G
02/01/10   jk      EPRL Support
01/25/10   shr     Fixed compute IP authentication handling
01/13/10   nb      Exposed mmgsdi_session_perso_operation
01/12/10   nb      Lint Fixes
01/04/10   nb      Removed inclusion of uimgen.h
12/23/09   shr     Fixed Perso Lock Down handling
12/16/09   rn      DF enum access support for mmgsdi_session_get_file_attr
12/22/09   nb      Updated mmgsdi_session_otasp_otapa_req: Possible value for
                   start_stop are 0x00 or 0x80
12/18/09   nb      Initial Version


=============================================================================*/

/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "intconv.h"
#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"
#include "mmgsdisessionlib_v.h"
#include "mmgsdilib_p.h"
#include "mmgsdi.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdicache.h"
#include "mmgsdi_session.h"
#include "nvruimi.h"
#include "nvruim_p.h"
#include "mmgsdi_icc.h"
#include "mmgsdi_uicc.h"
#include "mmgsdi_refresh.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_file.h"
#include "mmgsdi_caller_context.h"
#include "mmgsdi_external_simlock.h"
#include "mmgsdi_hidden_key.h"
#include "mmgsdi_cphs.h"
#include "mmgsdi_csg.h"
#include "mmgsdi_evt.h"

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/
/*=============================================================================
FUNCTION:       MMGSDI_SESSION_OPEN_EXT_SYNC

  Opens multiple sessions to an application on the card.
  The session can point to the GSM/WCDMA (GW) or 1X provisioning application.
  NON_PROV sessions are not supported.
  Card sessions are also supported as part of this API.

  Event callback is common to all the sessions opened. A client must provide an
  event callback if it is to receive session-related event notifications. If the
  pointer is NULL, the client receives no event.

  Caller should allocate the memory for obtaining session info based on the number
  of requested sessions. After returning from the function, it is the responsibility 
  of the caller to free the memory.

  @param[in] client_id:           Client ID of the caller.
  @param[in] session_type_mask:   Holds a mask of all sessions requested.
                                  (GW prov. and/or 1X prov. and/or Card session)
                                  Session_type will be determined by the bits set.

  @param[in] evt_cb_ptr:          Application event callback. A client must provide
                                  an event callback if it is to receive session-
                                  related event notifications. If the pointer is NULL,
                                  the client receives no event notifications.
                          
  @param[out] mmgsdi_session_info_ptr: Contains session info. It is valid only when
                                       return value of this function is MMGSDI_SUCCESS.

  @return
  MMGSDI_SUCCESS:            Request is processed successfully
  MMGSDI_INCORRECT_PARAMS:   The parameters supplied to the API are not
                             within appropriate ranges.

..@dependencies
  None

  @limitations
  If any one of the requested session fails, all other 
  opened sessions will be cleaned up and will return generic error.

  @side effects
  None
===================================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_open_ext_sync(
  mmgsdi_client_id_type            client_id,
  uint32                           session_type_mask,
  mmgsdi_evt_callback_type        *evt_cb_ptr,
  mmgsdi_session_open_info_type   *mmgsdi_session_info_ptr
)
{
  mmgsdi_session_open_ext_req_type *msg_ptr             = NULL;
  mmgsdi_return_enum_type           mmgsdi_status       = MMGSDI_ERROR;
  uint8                             num_sessions_opened = 0;
  int32                             client_index        = 0;
  int32                             session_index       = 0;
  uint8                             i                   = 0;

  UIM_MSG_HIGH_2("mmgsdi_session_open_ext_sync - Client Id 0x%x, Session Mask 0x%x",
                 client_id, session_type_mask);

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client ID
    2) session info ptr Check
    3) Session Type Mask
    4) Memory allocation for msg_ptr and check for allocation
    5) APP ID Length and pointer
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(mmgsdi_session_info_ptr);

  if((session_type_mask & MMGSDI_SESSION_VALID_MASK) == 0 ||
     (session_type_mask & (~MMGSDI_SESSION_VALID_MASK)))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(msg_ptr, sizeof(mmgsdi_session_open_ext_req_type));
  
  if (msg_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_open_ext_req_type header
      1) Client ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_OPEN_EXT_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_OPEN_EXT_REQ;
  msg_ptr->request_header.client_data       = 0;
  msg_ptr->request_header.response_cb       = NULL;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_open_ext_req_type content
    - session_type_mask
    - evt_cb_ptr
    - set_notify_mask
  ---------------------------------------------------------------------------*/
  msg_ptr->session_type_mask       = session_type_mask;
  msg_ptr->evt_cb_ptr              = evt_cb_ptr;
  
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  mmgsdi_status= mmgsdi_session_proc_open_ext(msg_ptr,
                               mmgsdi_session_info_ptr,
                               &num_sessions_opened);

  MMGSDIUTIL_TMC_MEM_FREE(msg_ptr);

  /* Check if we need to send session event */
  if(mmgsdi_status == MMGSDI_SUCCESS &&
     mmgsdi_session_info_ptr != NULL)
  {
    for(i = 0; i < num_sessions_opened; i++)
    {
      /* Loop through all the sessions and
         set the variable to allow sending of session events */
      mmgsdi_status = mmgsdi_util_get_client_and_session_index(
                        mmgsdi_session_info_ptr[i].session_id,
                        &client_index,
                        &session_index);

      if(mmgsdi_status == MMGSDI_SUCCESS )
      {
        mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index]->is_session_open_cnf_sent = TRUE;
      }
      mmgsdi_evt_build_and_notify_session_evt_from_state(mmgsdi_session_info_ptr[i].session_id);
    }
  }
  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT

  return mmgsdi_status;
} /* mmgsdi_session_open_ext_sync */


/*=============================================================================
FUNCTION        MMGSDI_SESSION_OPEN

DESCRIPTION
  This function allows the client to open a Session to an Application on the
  card. The session will either point to the GW or 1X PROVISIONING Application
  or to a particular Application if an AID is specified instead. If a
  provisioning application is indicated, and the provisioning application is
  not yet established, it will automatically be mapped to the provisioning
  application when it is established. Using this application also provides
  the client with all application-related events.

  If this is an ICC card, a session must still be opened, but all sessions
  will internally be mapped to the default channel.

  If a 1X Provisioning app is specified, and the card does not contain an
  CSIM App or CDMA DF, or if a GW Provisioning App is specified, but the card
  does not contain a USIM Application or GSM DF, the session will still
  succeed, but no init completed event will ever be sent for this
  application. Subsequent request with this Session ID may yield failures
  depending on the commands being sent.

  Upon successful completion of this command, the session will have an
  associated logical channel and an associated UICC Application Identifier.
  This session may be MUXED on the same channel as other sessions wishing to
  communicated with the same Application.

  If the session is of type MMGSDI_NON_PROV_SESSION_SLOT_1 or
  MMGSDI_NON_PROV_SESSION_SLOT_2, then the pointer non_prov_app_data_ptr
  should point to a valid Application ID; otherwise, it should be NULL.


PARAMETERS
  client_id:              Client ID.
  session_type:           GW, 1X or non-prov App.
  non_prov_app_data_ptr:  For non-prov, App. ID.
  evt_cb_ptr:             App Event Callback. A client should provide
                          an event callback if it is interested in receiving
                          session-related event notifications. If NULL,
                          client will receive no event notifications.
  set_notify:             Boolean indicating whether clients should call
                          mmgsdi_session_prov_app_init_complete() when they
                          finish their initialization procedure for the
                          applications. This is only applicable to the
                          provisioning-session types.
                          No meaning if evt_cb_ptr is a NULL pointer.
  response_cb_ptr         Command CB.
  client_ref:             User Data.

DEPENDENCIES
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and a failure will be returned in the response.
  Achieved by calling mmgsdi_client_id_and_evt_reg().

LIMITATIONS
  The parameter set_notify should be FALSE for non-provisioning sessions,
  otherwise MMGSDI returns an error.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
  Will result in the event callback for the session of interest getting called
  everytime MMGSDI needs to notify the client on this session about all
  session-related events.
===============================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_open (
  mmgsdi_client_id_type                 client_id,
  mmgsdi_session_type_enum_type         session_type,
  const mmgsdi_non_prov_app_info_type * non_prov_app_data_ptr,
  mmgsdi_evt_callback_type              evt_cb_ptr,
  boolean                               set_notify,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
)
{
  mmgsdi_task_cmd_type            *task_cmd_ptr  = NULL;
  mmgsdi_session_open_req_type    *msg_ptr       = NULL;
  mmgsdi_return_enum_type         mmgsdi_status  = MMGSDI_ERROR;
  int32                           total_mem_len  = 0;
  int32                           temp_mem       = 0;
  int32                           var_len        = 0;

  if (set_notify)
  {
    return MMGSDI_INCORRECT_PARAMS;	
  }

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client ID
    2) Response CB function Check
    3) Session Type and non_prov_app_data_ptr
    4) Memory allocation for msg_ptr and check for allocation
    5) APP ID Length and pointer
  ---------------------------------------------------------------------------*/

  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* If session_type indicates non_prov app then non_prov_app_data_ptr should
     be valid ptr */

  if(mmgsdi_util_is_non_prov_session(session_type) &&
     (non_prov_app_data_ptr == NULL))
  {
    UIM_MSG_ERR_0("non_prov_app_data_ptr can not be null for non_prov session type");
    return MMGSDI_INCORRECT_PARAMS;
  }

  if(!mmgsdi_util_is_non_prov_session(session_type) &&
     (non_prov_app_data_ptr != NULL))
  {
    UIM_MSG_ERR_0("Cannot provide app data for prov or card session types");
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* If client provided a non-prov app data then verify
     the aid len and aid data ptr */
  if(non_prov_app_data_ptr != NULL)
  {
    if((non_prov_app_data_ptr->app_id_data.data_len <= 0) ||
       (non_prov_app_data_ptr->app_id_data.data_len > MMGSDI_MAX_APP_ID_LEN) ||
       (non_prov_app_data_ptr->app_id_data.data_ptr == NULL))
    {
      UIM_MSG_ERR_2("Incorrect input app_data, data_len=0x%x, data_ptr=0x%x",
                    non_prov_app_data_ptr->app_id_data.data_len,
                    non_prov_app_data_ptr->app_id_data.data_ptr);
      return MMGSDI_ERROR;
    }

    var_len = non_prov_app_data_ptr->app_id_data.data_len;
  }

  if(evt_cb_ptr == NULL)
  {
    UIM_MSG_HIGH_0("mmgsdi_session_open null evt_cb_ptr");
  }

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem + var_len;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, int32touint32(total_mem_len));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  if (var_len > 0)
  {
    mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.session_open_req,
                           MMGSDI_SESSION_OPEN_REQ);
  }

  msg_ptr = &task_cmd_ptr->cmd.cmd.session_open_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_open_req_type header
      1) Client ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SESSION_OPEN_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_OPEN_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_OPEN_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;
  msg_ptr->request_header.slot_id           = MMGSDI_MAX_SLOT_ID_ENUM;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_open_req_type content
    - session_type
    - evt_cb_ptr
    - set_notify
    - non_prov_app_data
  ---------------------------------------------------------------------------*/
  msg_ptr->session_type         = session_type;
  msg_ptr->evt_cb_ptr           = evt_cb_ptr;
  msg_ptr->select_rsp_requested = FALSE;
  msg_ptr->select_rsp_type      = MMGSDI_SELECT_RSP_FCP;

  if(non_prov_app_data_ptr != NULL)
  {
    msg_ptr->non_prov_app_data.exclusive_channel  =
      non_prov_app_data_ptr->exclusive_channel;
    msg_ptr->non_prov_app_data.app_id_data.data_len =
      non_prov_app_data_ptr->app_id_data.data_len;
    mmgsdi_memscpy(msg_ptr->non_prov_app_data.app_id_data.data_ptr,
                   int32touint32(non_prov_app_data_ptr->app_id_data.data_len),
                   non_prov_app_data_ptr->app_id_data.data_ptr,
                   int32touint32(non_prov_app_data_ptr->app_id_data.data_len));
  }

  if(session_type == MMGSDI_NON_PROV_SESSION_SLOT_1)
  {
    msg_ptr->request_header.slot_id = MMGSDI_SLOT_1;
  }
  else if(session_type == MMGSDI_NON_PROV_SESSION_SLOT_2)
  {
    msg_ptr->request_header.slot_id = MMGSDI_SLOT_2;
  }
  else if(session_type == MMGSDI_NON_PROV_SESSION_SLOT_3)
  {
    msg_ptr->request_header.slot_id = MMGSDI_SLOT_3;
  }

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_OPEN_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_open */


/* ============================================================================
FUNCTION        MMGSDI_SESSION_CLOSE

DESCRIPTION
  This function is used to close the currently active session associated with
  the Session ID specified.

  Upon successful completion of this function, MMGSDI deallocates the
  Session ID and breaks the link in its internal sesssion table.

  If this close session applies to the last session ID for the active UICC
  application, MMGSDI will clean up as above and also perform the
  application deactivation procedure.  Also, note that, the logical channel
  associated with the Session ID will be released with the exception of
  logical channel 0.

PARAMETERS
  mmgsdi_session_id_type   session_id:      Session ID
  mmgsdi_callback_type     response_cb_ptr: Response callback
  mmgsdi_client_data_type  client_ref:      User Data returned upon
                                            completion of this cmd.

DEPENDENCIES
  The client must have a valid client ID, and the Session ID specified must
  be valid for deallocation.

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

=============================================================================== */
mmgsdi_return_enum_type  mmgsdi_session_close (
  mmgsdi_session_id_type       session_id,
  mmgsdi_callback_type         response_cb_ptr,
  mmgsdi_client_data_type      client_ref
)
{
  /* TRUE parameter indicates termination of non-provisioning
     applications as interpreted in legacy code */
  return mmgsdi_session_close_terminate (session_id,
                                         TRUE,
                                         response_cb_ptr,
                                         client_ref);
} /* mmgsdi_session_close */


/* ============================================================================
FUNCTION        MMGSDI_SESSION_CLOSE_TERMINATE

DESCRIPTION
  This function is used to close the currently active session associated with
  the Session ID specified.

  Upon successful completion of this function, MMGSDI deallocates the
  Session ID and breaks the link in its internal sesssion table.

  If this close session applies to the last session ID for the active UICC
  application and the client allows for applications to terminate, MMGSDI
  will clean up as above and, if requested by the client for non-provisioning
  applications, perform the application deactivation procedure. Also, note
  that, the logical channel associated with the Session ID will be released
  with the exception of logical channel 0.

  When the terminate parameter is TRUE, this function will perform a normal
  session close where the application is terminated before the channel is
  closed. When this parameter is FALSE, this function will skip the
  application termination and perform only the channel close.

PARAMETERS
  mmgsdi_session_id_type   session_id:      Session ID
  boolean                  terminate:       TRUE: perform terminate SELECT
                                            FALSE: skip terminate SELECT
                                            Does not affect the following
                                            manage channel
  mmgsdi_callback_type     response_cb_ptr: Response callback
  mmgsdi_client_data_type  client_ref:      User Data returned upon
                                            completion of this cmd.

DEPENDENCIES
  The client must have a valid client ID, and the Session ID specified must
  be valid for deallocation.

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

=============================================================================== */
mmgsdi_return_enum_type  mmgsdi_session_close_terminate (
  mmgsdi_session_id_type       session_id,
  boolean                      terminate,
  mmgsdi_callback_type         response_cb_ptr,
  mmgsdi_client_data_type      client_ref
)
{
  mmgsdi_task_cmd_type              *task_cmd_ptr   = NULL;
  mmgsdi_session_close_req_type     *msg_ptr        = NULL;
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_ERROR;
  uint32                             task_cmd_len   = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Response CB function Check
    3) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_CLOSE_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  task_cmd_len += sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  msg_ptr = &task_cmd_ptr->cmd.cmd.session_close_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_close_req_type header
      1) Session ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SESSION_CLOSE_REQ;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_CLOSE_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_CLOSE_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;
  msg_ptr->request_header.slot_id           = MMGSDI_MAX_SLOT_ID_ENUM;

  /*---------------------------------------------------------------------------
   Determine if non-provisioning application will be terminated
  ---------------------------------------------------------------------------*/
  msg_ptr->non_prov_terminate                  = terminate;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_CLOSE_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_close_terminate */


/*=============================================================================
FUNCTION        MMGSDI_SESSION_ACTIVATE_OR_SWITCH_PROVISIONING

DESCRIPTION
  This function allows the client to establish an application:
  SIM/USIM/RUIM/CSIM as the provisioning application on a session which
  it has opened. It may also call this API to change the current
  provisioning application.

  If the card is a UICC, the opened session will be associated with the
  USIM or CSIM AID specified by the slot number upon successful completion
  of this command, and the associated application will be selected on a
  particular logical channel mapped to the session ID.

  If the card is an ICC, the opened session will be associated with the
  GSM DF or CDMA DF specified by the slot number upon successful
  completion of this request. The associated application will be
  selected on the default logical channel mapped to the session ID.

  Any other clients which have opened the 1X or GW Provisioning will have
  their sessions automatically mapped to the same logical channel.

  If this API is used to change the current 1X or GW provisioning
  application, the new application will be opened on the same or a
  different logical channel and mapped to this session.

  All clients registered to receive session events for this provisioning
  application (1X or GW) will receive MMGSDI_SESSION_CHANGED_EVT
  (activated = FALSE) as the first notification and then a
  MMGSDI_SESSION_CHANGED_EVT (activated = TRUE) notification

PARAMETERS
  mmgsdi_session_id_type     session_id:       Session ID
  mmgsdi_slot_id_enum_type   card_slot:        Physical slot for
                                               multiple slot targets.
  mmgsdi_data_type           app_id_data:      Application Identifier.
  mmgsdi_callback_type       response_cb_ptr:  Response CB.
  mmgsdi_client_data_type    client_ref:       User Data.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

===============================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_activate_or_switch_provisioning (
  mmgsdi_session_id_type                session_id,
  mmgsdi_slot_id_enum_type              card_slot,
  mmgsdi_data_type                      app_id_data,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
)
{
  mmgsdi_task_cmd_type                       *task_cmd_ptr    = NULL;
  mmgsdi_session_act_or_switch_prov_req_type *msg_ptr         = NULL;
  mmgsdi_return_enum_type                     mmgsdi_status   = MMGSDI_ERROR;
  uint32                                      task_cmd_len    = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Response CB function Check
    3) Slot
    4) AID
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  if((!mmgsdi_util_is_slot_valid(card_slot) &&
      card_slot != MMGSDI_SLOT_AUTOMATIC) ||
     app_id_data.data_len > MMGSDI_MAX_AID_LEN ||
     app_id_data.data_len < 0 )
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  task_cmd_len += sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  msg_ptr = &task_cmd_ptr->cmd.cmd.session_act_or_switch_prov_req;

    /*---------------------------------------------------------------------------
    Populating session_act_or_switch_prov_req header
      1) Session ID
      2) Request TYPE
      3) Slot ID
      4) Client Data Pointer
      5) Response CallBack
      6) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Populating session_act_or_switch_prov_req content
    - app_id_data
  ---------------------------------------------------------------------------*/
  msg_ptr->app_id_data.data_len = app_id_data.data_len;
  mmgsdi_memscpy(msg_ptr->app_id_data.data_ptr,
                 sizeof(msg_ptr->app_id_data.data_ptr),
                 app_id_data.data_ptr,
                 int32touint32(app_id_data.data_len));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_session_activate_or_switch_provisioning */


/*=============================================================================
FUNCTION        MMGSDI_SESSION_DEACTIVATE_PROVISIONING

DESCRIPTION
  This function allows the client to deactivate an application:
  SIM/USIM/RUIM/CSIM from being the provisioning application.

  If the card is a UICC, the existing session ids for this provisioning
  application will no longer be bound to any application on the card.
  MMGSDI will perform the deactivation procedure for the provsioning
  application and the logical channel will be closed with the exception of
  channel 0.

  If the card is an ICC, the existing session ids for this provisioning
  application will no longer be bound to any application on the card.

  All clients registered to receive session events for this provisioning
  application (1X or GW) will receive MMGSDI_SESSION_CHANGED_EVT
  (activated = FALSE) as the notification.

PARAMETERS

  mmgsdi_session_id_type     session_id:       Session ID
  mmgsdi_callback_type       response_cb_ptr:  Response CB.
  mmgsdi_client_data_type    client_ref:       User Data.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  None.

RETURN VALUE

  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly
                             generated and queued onto the MMGSDI
                             Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI
                             Task because the max number of commands
                             are already queued.

SIDE EFFECTS
  None.
===============================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_deactivate_provisioning (
  mmgsdi_session_id_type                session_id,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
)
{
  mmgsdi_task_cmd_type                       *task_cmd_ptr    = NULL;
  mmgsdi_session_deact_req_type              *msg_ptr         = NULL;
  mmgsdi_return_enum_type                     mmgsdi_status   = MMGSDI_ERROR;
  uint32                                      task_cmd_len    = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Response CB function Check
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_DEACTIVATE_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  task_cmd_len += sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  msg_ptr = &task_cmd_ptr->cmd.cmd.session_deact_req;

    /*---------------------------------------------------------------------------
    Populating mmgsdi_session_deact_req_type header
      1) Session ID
      2) Request TYPE
      3) Response CallBack
  ---------------------------------------------------------------------------*/

  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SESSION_DEACTIVATE_REQ;
  msg_ptr->internal_deact_req               = FALSE;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_DEACTIVATE_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_DEACTIVATE_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_DEACTIVATE_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_session_deactivate_provisioning */


/*=========================================================================
FUNCTION        MMGSDI_SESSION_GET_INFO

DESCRIPTION
  This function allows a client to query MMGSDI for session information by
  Session ID or Session Type. A client can get Session Type, Slot Number,
  Channel ID and AID using this request.

PARAMETERS
  session_info_query:   query type and key.
  response_cb_ptr:      Response CB.
  client_ref:           User Data.

DEPENDENCIES
  If a client queries by Session ID, a valid Session ID is required, which can
  be achieved by calling mmgsdi_session_open().

LIMITATIONS

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

=======================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_get_info (
  mmgsdi_session_info_query_type    session_info_query,
  mmgsdi_callback_type              response_cb_ptr,
  mmgsdi_client_data_type           client_ref
)
{
  mmgsdi_task_cmd_type                       *task_cmd_ptr    = NULL;
  mmgsdi_session_get_info_req_type           *msg_ptr         = NULL;
  mmgsdi_return_enum_type                     mmgsdi_status   = MMGSDI_ERROR;
  uint32                                      task_cmd_len    = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Response CB function Check
  ---------------------------------------------------------------------------*/

  if(session_info_query.query_type == MMGSDI_SESSION_INFO_QUERY_BY_ID)
  {
    mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                      session_info_query.query_key.session_id,
                      MMGSDI_SESSION_GET_INFO_REQ);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  task_cmd_len += sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  msg_ptr = &task_cmd_ptr->cmd.cmd.session_get_info_req;

    /*---------------------------------------------------------------------------
    Populating mmgsdi_session_get_info_req_type header
      1) Session ID
      2) Request TYPE
      3) Response CallBack
  ---------------------------------------------------------------------------*/

  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SESSION_GET_INFO_REQ;
  if(session_info_query.query_type == MMGSDI_SESSION_INFO_QUERY_BY_ID)
  {
    msg_ptr->request_header.session_id =
      session_info_query.query_key.session_id;
  }
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_GET_INFO_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_GET_INFO_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Populating session_get_info_req content
  ---------------------------------------------------------------------------*/
  mmgsdi_memscpy(&msg_ptr->session_info_query,
                 sizeof(msg_ptr->session_info_query),
                 &session_info_query,
                 sizeof(mmgsdi_session_info_query_type));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_GET_INFO_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_get_info */


/*=========================================================================
FUNCTION        MMGSDI_SESSION_GET_INFO_SYNC

DESCRIPTION
  This function allows a client to query MMGSDI for session information by
  Session ID or Session Type. A client can get Session Type, Slot Number,
  Channel ID and AID using this request.
  The client calling this function expects an instant response.

  This API uses critical sections to protect accesses to
  MMGSDI global data. Hence, it is advisable to pet the dog
  before invoking the API.

PARAMETERS
  session_info_query:   query type and key.
  session_get_info_ptr: Buffer to store the Session related information.

DEPENDENCIES
  If a client queries by Session ID, a valid Session ID is required, which can
  be achieved by calling mmgsdi_session_open().

LIMITATIONS

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          Success.
    MMGSDI_ERROR:            There is no application linked to the
                             Session ID/Session type.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

SIDE EFFECTS

=======================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_get_info_sync (
  mmgsdi_session_info_query_type    session_info_query,
  mmgsdi_session_get_info_type    * session_get_info_ptr
)
{
  mmgsdi_return_enum_type              mmgsdi_status     = MMGSDI_SUCCESS;
  mmgsdi_session_info_type           * session_info_ptr  = NULL;
  mmgsdi_channel_info_type           * channel_info_ptr  = NULL;
  mmgsdi_int_app_info_type           * app_info_ptr      = NULL;
  mmgsdi_session_id_type               mmgsdi_session_id = 0;
  mmgsdi_session_id_status_enum_type   session_status    = MMGSDI_SESSION_ID_UNASSIGNED;

  UIM_MSG_HIGH_0("mmgsdi_session_get_info_sync");

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response CB function Check
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_RETURN_IF_NULL(session_get_info_ptr);
  memset(session_get_info_ptr, 0x00, sizeof(mmgsdi_session_get_info_type));

  /* Protect access to Client ID Reg. Table/App. table */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    /* Get the session info */
    if(session_info_query.query_type == MMGSDI_SESSION_INFO_QUERY_BY_ID)
    {
      session_status = mmgsdi_util_is_session_id_valid(
                         session_info_query.query_key.session_id);

      if((session_status !=   MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE) &&
         (session_status !=  MMGSDI_SESSION_ID_ASSIGNED_NOT_ACTIVE))
      {
        UIM_MSG_ERR_1("Input session Id is not in use id=0x%x ",
                      session_info_query.query_key.session_id);
        mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
        break;
      }
      mmgsdi_session_id = session_info_query.query_key.session_id;
    }
    else if(session_info_query.query_type ==
                                          MMGSDI_SESSION_INFO_QUERY_BY_TYPE)
    {
      if (MMGSDI_SUCCESS != mmgsdi_util_get_mmgsdi_session_id_from_provisioning_type(
                              session_info_query.query_key.session_type,
                              &mmgsdi_session_id))
      {
        UIM_MSG_ERR_1("Invalid Session type requested=0x%x ",
                      session_info_query.query_key.session_type);
        mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      }
    }
    else
    {
       UIM_MSG_ERR_1("Invalid Session Query type=0x%x ",
                     session_info_query.query_type);
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      break;
    }

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }

    /* Get app info corresponding to the Session ID */
    mmgsdi_status = mmgsdi_util_get_session_app_info(
                      mmgsdi_session_id, NULL, NULL, &app_info_ptr,
                      &channel_info_ptr, &session_info_ptr, NULL);

    if(mmgsdi_status != MMGSDI_SUCCESS ||
       session_info_ptr == NULL ||
       channel_info_ptr == NULL ||
       app_info_ptr == NULL)
    {
      UIM_MSG_ERR_0("Session Info unavailable");

      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      break;
    }

    session_get_info_ptr->slot_id = channel_info_ptr->slot_id;
    session_get_info_ptr->channel_id = channel_info_ptr->channel_id;
    session_get_info_ptr->session_type = session_info_ptr->session_type;
    mmgsdi_memscpy(&session_get_info_ptr->app_data,
                   sizeof(session_get_info_ptr->app_data),
                   &app_info_ptr->app_data,
                   sizeof(mmgsdi_aid_type));
    session_get_info_ptr->app_state = app_info_ptr->app_state;
  }while(0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  return mmgsdi_status;
}/* mmgsdi_session_get_info_sync */


/*===========================================================================
FUNCTION        MMGSDI_SESSION_GET_ALL_PIN_STATUS

DESCRIPTION
  This function will build a request to the MMGSDI Task to request the PIN
  status for all relevant pins applicable to the session ID provided.

PARAMETERS
  session_id:        Session ID of the caller
  response_cb_ptr:   Response callback.
  client_ref:        User data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  MMGSDI is only responsible for provisioning applications' PIN status.
  Non-provisioning applications' PIN status needs to be performed by
  clients independently.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_get_all_pin_status (
  mmgsdi_session_id_type            session_id,
  mmgsdi_callback_type              response_cb_ptr,
  mmgsdi_client_data_type           client_ref
)
{
  mmgsdi_task_cmd_type               * task_cmd_ptr   = NULL;
  mmgsdi_get_all_pin_status_req_type * msg_ptr        = NULL;
  mmgsdi_return_enum_type              mmgsdi_status  = MMGSDI_ERROR;
  uint32                               task_cmd_len   = 0;

  /* --------------------------------------------------------------------------
     Validate the input parameters
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_GET_ALL_PIN_STATUS_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* --------------------------------------------------------------------------
     Allocate 1 large block of memory for the task_cmd_ptr and for the
     onchip config data.
     NOTE:  Side effect of the following total len is that it may not
            match exactly as a result of mmgsdi_task_cmd_data_type being
            a union. Will be addressed by using the correct type size
            rather than sizeof mmgsdi_task_cmd_data_type
     ------------------------------------------------------------------------*/
  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*-----------------------------------------------------------------------------
    No need to align pointers
    ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.pin_status_req;

  /* ----------------------------------------------------------------------------
     Populate the command Request Header with the information provided by
     the client.
     ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_GET_ALL_PIN_STATUS_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_GET_ALL_PIN_STATUS_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_GET_ALL_PIN_STATUS_REQ;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.slot_id           = MMGSDI_SLOT_1;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* --------------------------------------------------------------------------
     Queue the command, free the command, and return the status
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr
    ** has not been put onto the command queue
    */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_GET_ALL_PIN_STATUS_REQ status 0x%x ",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_get_all_pin_status */


/*===========================================================================
FUNCTION      MMGSDI_SESSION_PIN_OPERATION

DESCRIPTION
  Populate the core PIN operation request

PARAMS:
  session_id:        Session Id of the caller
  card_slot:         Physical slot for multiple slot targets.
  pin_id:            The PIN Identification to be verified.
  pin_data:          Will contain the len and code of the PIN.
  client_ref:        User Data returned upon completion of this cmd.

===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_session_pin_operation(
  mmgsdi_session_id_type             session_id,
  mmgsdi_pin_enum_type               pin_id,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref,
  mmgsdi_pin_operation_req_type      **msg_ptr,
  mmgsdi_task_cmd_type               **task_cmd_ptr,
  mmgsdi_pin_operation_enum_type     pin_op
)
{
  uint32                             task_cmd_len   = 0;
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_ERROR;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Response CB function Check
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_PIN_OPERATION_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*task_cmd_ptr, task_cmd_len);
  if ((*task_cmd_ptr) == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
    /*---------------------------------------------------------------------------
    - No need to align pointers
    ---------------------------------------------------------------------------*/
  *msg_ptr = &((*task_cmd_ptr)->cmd.cmd.pin_operation_req);

  /*---------------------------------------------------------------------------
    Populating mmgsdi_pin_operation_req_type header
      1) Session ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  (*task_cmd_ptr)->cmd.cmd_enum                = MMGSDI_PIN_OPERATION_REQ;
  (*msg_ptr)->request_header.session_id        = session_id;
  (*msg_ptr)->request_header.request_type      = MMGSDI_PIN_OPERATION_REQ;
  (*msg_ptr)->request_header.orig_request_type = MMGSDI_PIN_OPERATION_REQ;
  (*msg_ptr)->request_header.client_data       = client_ref;
  (*msg_ptr)->request_header.response_cb       = response_cb_ptr;

  (*msg_ptr)->pin_op                           = pin_op;

  if (pin_id == MMGSDI_KEY_REF_PIN_APPL_1 ||
      pin_id == MMGSDI_KEY_REF_PIN_APPL_2 ||
      pin_id == MMGSDI_KEY_REF_PIN_APPL_3 ||
      pin_id == MMGSDI_KEY_REF_PIN_APPL_4 ||
      pin_id == MMGSDI_KEY_REF_PIN_APPL_5 ||
      pin_id == MMGSDI_KEY_REF_PIN_APPL_6 ||
      pin_id == MMGSDI_KEY_REF_PIN_APPL_7 ||
      pin_id == MMGSDI_KEY_REF_PIN_APPL_8)
  {
    (*msg_ptr)->pin_id       = MMGSDI_PIN1;
    (*msg_ptr)->orig_pin_id  = pin_id;
  }
  else if (pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_1 ||
           pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_2 ||
           pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_3 ||
           pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_4 ||
           pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_5 ||
           pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_6 ||
           pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_7 ||
           pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_8)
  {
    (*msg_ptr)->pin_id       = MMGSDI_PIN2;
    (*msg_ptr)->orig_pin_id  = pin_id;
  }
  else
  {
    (*msg_ptr)->pin_id       = pin_id;
    (*msg_ptr)->orig_pin_id  = pin_id;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_session_pin_operation */


/*===========================================================================
FUNCTION        MMGSDI_SESSION_VERIFY_PIN

DESCRIPTION
  This function will build a request to the MMGSDI Task to perform a PIN
  verification procedure against the Card for UPIN, PIN1 or PIN2. The PIN
  provided will have to be applicable to the Session ID Provided.

  A failure to verify could result in a blocked PIN ID if the maximum
  number of attempts to retry are exhausted.

PARAMETERS
  session_id:        Session ID of the caller
  pin_id:            The PIN Identification to be verified.
  pin_data:          Will contain the len and code of the PIN.
  response_cb_ptr:   Response callback
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_verify_pin (
  mmgsdi_session_id_type            session_id,
  mmgsdi_pin_enum_type              pin_id,
  mmgsdi_data_type                  pin_data,
  mmgsdi_callback_type              response_cb_ptr,
  mmgsdi_client_data_type           client_ref
)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr   = NULL;
  mmgsdi_pin_operation_req_type * msg_ptr        = NULL;
  mmgsdi_return_enum_type         mmgsdi_status  = MMGSDI_ERROR;

  if(pin_data.data_len <= 0 || pin_data.data_len > MMGSDI_PIN_MAX_LEN)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }
  MMGSDIUTIL_RETURN_IF_NULL(pin_data.data_ptr);

  UIM_MSG_HIGH_1("mmgsdi_session_verify_pin: pin id: 0x%x", pin_id);

  /* If request is for Hiddenkey verification */
  if(pin_id == MMGSDI_HIDDENKEY)
  {
    /* Check if the Hidden key is in the correct length and data range */
    if (!mmgsdi_hidden_key_is_valid(&pin_data))
    {
      return MMGSDI_INCORRECT_PARAMS;
    }
  }
  /* check if pin is in the correct length and data range */
  else if (!mmgsdi_util_is_pin_valid(pin_data.data_ptr, pin_data.data_len))
  {
     return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_session_pin_operation(session_id,
                                       pin_id,
                                       response_cb_ptr,
                                       client_ref,
                                       &msg_ptr,
                                       &task_cmd_ptr,
                                       MMGSDI_PIN_OP_VERIFY);

   if(mmgsdi_status != MMGSDI_SUCCESS)
   {
      UIM_MSG_ERR_1("mmgsdi_session_pin_operation returned error 0x%x",
                    mmgsdi_status);
      return mmgsdi_status;
   }
  /*---------------------------------------------------------------------------
    Populating mmgsdi_pin_operation_req_type content
    - pin_op - MMGSDI_PIN_OP_VERIFY
    - pin_id - pin_id
    - pin_puk - pin_data
    ---------------------------------------------------------------------------*/
  msg_ptr->pin_puk.data_len = pin_data.data_len;

  mmgsdi_memscpy(msg_ptr->pin_puk.data_ptr,
                 sizeof(msg_ptr->pin_puk.data_ptr),
                 pin_data.data_ptr,
                 int32touint32(pin_data.data_len));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_PIN_OPERATION_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_session_verify_pin */


/*===========================================================================
FUNCTION        MMGSDI_SESSION_DISABLE_PIN

DESCRIPTION
  This function will build a request to the MMGSDI Task to disable the PIN
  provided and indicate whether it should be replaced by the Universal PIN.
  The PIN provided will have to be applicable to the Session ID Provided.

  A failure to disable could result in a blocked PIN ID if the maximum
  number of attempts to retry are exhausted.

PARAMETERS:
  session_id:        Session ID of the caller
  pin_id:            The PIN Identification to be verified.
  replace_pin:       The replacement option.
  pin_data:          Will contain the len and code of the PIN.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_disable_pin (
  mmgsdi_session_id_type            session_id,
  mmgsdi_pin_enum_type              pin_id,
  mmgsdi_pin_replace_enum_type      replace_pin,
  mmgsdi_data_type                  pin_data,
  mmgsdi_callback_type              response_cb_ptr,
  mmgsdi_client_data_type           client_ref
)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr   = NULL;
  mmgsdi_pin_operation_req_type * msg_ptr        = NULL;
  mmgsdi_return_enum_type         mmgsdi_status  = MMGSDI_ERROR;
  mmgsdi_pin_operation_enum_type  pin_op         = MMGSDI_PIN_OP_DISABLE;


  if(pin_data.data_len <= 0 || pin_data.data_len > MMGSDI_PIN_MAX_LEN)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }
  MMGSDIUTIL_RETURN_IF_NULL(pin_data.data_ptr);

  UIM_MSG_HIGH_1("mmgsdi_session_disable_pin: pin id: 0x%x", pin_id);

  /* If request is to disable Hiddenkey */
  if(pin_id == MMGSDI_HIDDENKEY)
  {
    /* Check if the Hidden key is in the correct length and data range */
    if (!mmgsdi_hidden_key_is_valid(&pin_data))
    {
      return MMGSDI_INCORRECT_PARAMS;
    }
  }
  /* check if pin is in the correct length and data range */
  else if (!mmgsdi_util_is_pin_valid(pin_data.data_ptr, pin_data.data_len))
  {
     return MMGSDI_INCORRECT_PARAMS;
  }

  if(replace_pin == MMGSDI_PIN_REPLACED_BY_UNIVERSAL)
  {
    pin_op = MMGSDI_PIN_OP_DISABLE_AND_REPLACE;
    if(pin_id != MMGSDI_PIN1)
    {
      UIM_MSG_ERR_0("PIN 0x%x cannot be replaced");
      return MMGSDI_INCORRECT_PARAMS;
    }
  }

  mmgsdi_status = mmgsdi_session_pin_operation(session_id,
                                       pin_id,
                                       response_cb_ptr,
                                       client_ref,
                                       &msg_ptr,
                                       &task_cmd_ptr,
                                       pin_op);

   if(mmgsdi_status != MMGSDI_SUCCESS)
   {
      UIM_MSG_ERR_1("mmgsdi_session_pin_operation returned error 0x%x",
                    mmgsdi_status);
      return mmgsdi_status;
   }

  /*---------------------------------------------------------------------------
    Populating mmgsdi_pin_operation_req_type content
    - pin_op - MMGSDI_PIN_OP_DISABLE
    - pin_id - pin_id
    - pin_puk - pin_data
    ---------------------------------------------------------------------------*/
  msg_ptr->pin_puk.data_len = pin_data.data_len;

  mmgsdi_memscpy(msg_ptr->pin_puk.data_ptr,
                 sizeof(msg_ptr->pin_puk.data_ptr),
                 pin_data.data_ptr,
                 int32touint32(pin_data.data_len));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto
       the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_PIN_OPERATION_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_disable_pin */


/*===========================================================================
FUNCTION        MMGSDI_SESSION_ENABLE_PIN

DESCRIPTION
  This function will build a request to the MMGSDI Task to enable the PIN
  provided.  The PIN provided will have to be applicable to the Session
  ID Provided.

  A failure to enable could result in a blocked PIN ID if the maximum
  number of attempts to retry are exhausted.

PARAMETERS
  session_id:        Session ID of the caller
  pin_id:            The PIN Identification to be verified.
  pin_data:          Will contain the len and code of the PIN.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_enable_pin (
  mmgsdi_session_id_type             session_id,
  mmgsdi_pin_enum_type               pin_id,
  mmgsdi_data_type                   pin_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr   = NULL;
  mmgsdi_pin_operation_req_type * msg_ptr        = NULL;
  mmgsdi_return_enum_type         mmgsdi_status   = MMGSDI_ERROR;

  if(pin_data.data_len <= 0 || pin_data.data_len > MMGSDI_PIN_MAX_LEN)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }
  MMGSDIUTIL_RETURN_IF_NULL(pin_data.data_ptr);

  UIM_MSG_HIGH_1("mmgsdi_session_enable_pin: pin id: 0x%x", pin_id);

  /* If request is for enabling Hiddenkey */
  if(pin_id == MMGSDI_HIDDENKEY)
  {
    /* Check if the Hidden key is in the correct length and data range */
    if (!mmgsdi_hidden_key_is_valid(&pin_data))
    {
      return MMGSDI_INCORRECT_PARAMS;
    }
  }
  /* check if pin is in the correct length and data range */
  else if (!mmgsdi_util_is_pin_valid(pin_data.data_ptr, pin_data.data_len))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_session_pin_operation(session_id,
                                       pin_id,
                                       response_cb_ptr,
                                       client_ref,
                                       &msg_ptr,
                                       &task_cmd_ptr,
                                       MMGSDI_PIN_OP_ENABLE);

   if(mmgsdi_status != MMGSDI_SUCCESS)
   {
      UIM_MSG_ERR_1("mmgsdi_session_pin_operation returned error 0x%x",
                    mmgsdi_status);
      return mmgsdi_status;
   }
  /*---------------------------------------------------------------------------
    Populating mmgsdi_pin_operation_req_type content
    - pin_op - MMGSDI_PIN_OP_ENABLE
    - pin_id - pin_id
    - pin_puk - pin_data
    ---------------------------------------------------------------------------*/
  msg_ptr->pin_puk.data_len = pin_data.data_len;

  mmgsdi_memscpy(msg_ptr->pin_puk.data_ptr,
                 sizeof(msg_ptr->pin_puk.data_ptr),
                 pin_data.data_ptr,
                 int32touint32(pin_data.data_len));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_PIN_OPERATION_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_enable_pin */


/*===========================================================================
FUNCTION        MMGSDI_SESSION_CHANGE_PIN

DESCRIPTION
  This function will build a request to the MMGSDI Task to change the PIN
  provided (UPIN, PIN1 or PIN2). The PIN provided will have to be applicable
  to the Session ID Provided.

  A failure to change could result in a blocked PIN ID if the maximum
  number of attempts to retry are exhausted.

PARAMETERS:
  session_id:        Session ID of the caller
  pin_id:            The PIN Identification to be verified.
  pin_data:          Will contain the len and code of the current PIN.
  new_pin_data:      Will contain the len and code of the new PIN.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
  None.
===================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_change_pin(
  mmgsdi_session_id_type        session_id,
  mmgsdi_pin_enum_type          pin_id,
  mmgsdi_data_type              pin_data,
  mmgsdi_data_type              new_pin_data,
  mmgsdi_callback_type          response_cb_ptr,
  mmgsdi_client_data_type       client_ref
)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr   = NULL;
  mmgsdi_pin_operation_req_type * msg_ptr        = NULL;
  mmgsdi_return_enum_type         mmgsdi_status   = MMGSDI_ERROR;

  if(pin_data.data_len <= 0 || pin_data.data_len > MMGSDI_PIN_MAX_LEN ||
     new_pin_data.data_len <= 0 || new_pin_data.data_len > MMGSDI_PIN_MAX_LEN)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }
  MMGSDIUTIL_RETURN_IF_NULL_2(pin_data.data_ptr, new_pin_data.data_ptr);

  UIM_MSG_HIGH_1("mmgsdi_session_change_pin: pin id: 0x%x", pin_id);

  /* If request is for Hiddenkey Change */
  if(pin_id == MMGSDI_HIDDENKEY)
  {
    /* Check if the Hidden key is in the correct length and data range */
    if (!mmgsdi_hidden_key_is_valid(&pin_data) ||
        !mmgsdi_hidden_key_is_valid(&new_pin_data))
    {
      return MMGSDI_INCORRECT_PARAMS;
    }
  }
  /* check if pin is in the correct length and data range */
  else if ( (!mmgsdi_util_is_pin_valid(pin_data.data_ptr, pin_data.data_len)) ||
            (!mmgsdi_util_is_pin_valid(new_pin_data.data_ptr, new_pin_data.data_len)) )
  {
     return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_session_pin_operation(session_id,
                                       pin_id,
                                       response_cb_ptr,
                                       client_ref,
                                       &msg_ptr,
                                       &task_cmd_ptr,
                                       MMGSDI_PIN_OP_CHANGE);

   if(mmgsdi_status != MMGSDI_SUCCESS)
   {
      UIM_MSG_ERR_1("mmgsdi_session_pin_operation returned error 0x%x",
                    mmgsdi_status);
      return mmgsdi_status;
   }
  /*---------------------------------------------------------------------------
    Populating mmgsdi_pin_operation_req_type content
    - pin_op - MMGSDI_PIN_OP_CHANGE
    - pin_id - pin_id
    - pin_puk - pin_data
    -------------------------------------------------------------------------*/
  msg_ptr->pin_puk.data_len = pin_data.data_len;

  mmgsdi_memscpy(msg_ptr->pin_puk.data_ptr,
                 sizeof(msg_ptr->pin_puk.data_ptr),
                 pin_data.data_ptr,
                 int32touint32(pin_data.data_len));

  msg_ptr->new_pin.data_len = new_pin_data.data_len;

  mmgsdi_memscpy(msg_ptr->new_pin.data_ptr,
                 sizeof(msg_ptr->new_pin.data_ptr),
                 new_pin_data.data_ptr,
                 int32touint32(new_pin_data.data_len));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_PIN_OPERATION_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /*mmgsdi_session_change_pin*/


/*===========================================================================
FUNCTION        MMGSDI_SESSION_UNBLOCK_PIN

DESCRIPTION
  This function will build a request to the MMGSDI Task to unblock the PIN
  provided.  The PIN provided will have to be applicable to the Session
  ID Provided.

  A failure to unblock could result in a permanently blocked PIN ID if
  the maximum number of attempts to unblock are exhausted.

PARAMETERS
  session_id:        Session ID of the caller
  pin_id:            The PIN Identification to be verified.
  puk_data:          Will contain the len and code of the PUK.
  new_pin_data:      Will contain the len and code of the new PIN.
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_unblock_pin(
  mmgsdi_session_id_type             session_id,
  mmgsdi_pin_enum_type               pin_id,
  mmgsdi_data_type                   puk_data,
  mmgsdi_data_type                   new_pin_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type          * task_cmd_ptr   = NULL;
  mmgsdi_pin_operation_req_type * msg_ptr        = NULL;
  mmgsdi_return_enum_type         mmgsdi_status   = MMGSDI_ERROR;

  if(puk_data.data_len <= 0 || puk_data.data_len > MMGSDI_PIN_MAX_LEN ||
     new_pin_data.data_len <= 0 || new_pin_data.data_len > MMGSDI_PIN_MAX_LEN)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }
  MMGSDIUTIL_RETURN_IF_NULL_2(puk_data.data_ptr, new_pin_data.data_ptr);

  /* If request is for Hiddenkey unblock */
  if(pin_id == MMGSDI_HIDDENKEY)
  {
    UIM_MSG_ERR_0("Hiddenkey unblock operation not supported");
    return MMGSDI_NOT_SUPPORTED;
  }
  /* check if pin is in the correct length and data range */
  else if (!mmgsdi_util_is_pin_valid(new_pin_data.data_ptr, new_pin_data.data_len))
  {
     return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_session_pin_operation(session_id,
                                       pin_id,
                                       response_cb_ptr,
                                       client_ref,
                                       &msg_ptr,
                                       &task_cmd_ptr,
                                       MMGSDI_PIN_OP_UNBLOCK);

   if(mmgsdi_status != MMGSDI_SUCCESS)
   {
      UIM_MSG_ERR_1("mmgsdi_session_pin_operation returned error 0x%x",
                    mmgsdi_status);
      return mmgsdi_status;
   }
  /*---------------------------------------------------------------------------
    Populating mmgsdi_pin_operation_req_type content
    - pin_op - MMGSDI_PIN_OP_UNBLOCK
    - pin_id - pin_id
    - pin_puk - pin_data
    ---------------------------------------------------------------------------*/
  msg_ptr->pin_puk.data_len = puk_data.data_len;

  mmgsdi_memscpy(msg_ptr->pin_puk.data_ptr,
                 sizeof(msg_ptr->pin_puk.data_ptr),
                 puk_data.data_ptr,
                 int32touint32(puk_data.data_len));

  msg_ptr->new_pin.data_len = new_pin_data.data_len;

  mmgsdi_memscpy(msg_ptr->new_pin.data_ptr,
                 sizeof(msg_ptr->new_pin.data_ptr),
                 new_pin_data.data_ptr,
                 int32touint32(new_pin_data.data_len));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
     onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_PIN_OPERATION_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_unblock_pin */


/* ========================================================================
FUNCTION        MMGSDI_SESSION_READ

DESCRIPTION
  This function provides access to any Record File in the SIM/USIM/RUIM/CSIM
  or to read transparent file.
  It allows file access by file enum or path.
  This library function is intended to encapsulate the details required to
  read a file from the SIM/USIM/RUIM/CSIM.

  Behavior for the function for the following file types:

    TRANSPARENT FILE:   Will return the contents as required in the Library.
    CYCLIC FILE:        Will access the record of the file provided in the
                        library call.
    LINEAR FIXED FILE:  Will access the record of the file provided in the
                        library call.

  Behavior for the function for the following acess types:
    MMGSDI_EF_ENUM_ACCESS: Will return the contents as required in the
                           Library Call.
    MMGSDI_BY_PATH_ACCESS: Will return the contents as required in the
                           Library Call. Note, there is no check whether the
                           path passed in is record file or not. The
                           caller is expected to have the knowledge of
                           whether the file to access is record or not.
    others:                Returns an Error.

PARAMETERS
  session_id:           Session ID of the caller.
  file_access:          File to read.
  file_type:            file type.
  offset:               Offset from the start of a file. Valid only for transparent files
  record_number:        Record number, should be equal to or greater than 1.
                        Valid only for record-based files
  request_length:       Request length.
  response_cb_ptr:      Response callback.
  client_ref:           User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

=========================================================================== */
static mmgsdi_return_enum_type  mmgsdi_session_read (
  mmgsdi_session_id_type            session_id,
  mmgsdi_access_type                file_access,
  mmgsdi_file_structure_enum_type   file_type,
  mmgsdi_offset_type                offset,            /* Valid only for transparent files */
  mmgsdi_rec_num_type               record_number,     /* Valid only for record-based files */
  mmgsdi_len_type                   request_length,
  mmgsdi_callback_type              response_cb_ptr,
  mmgsdi_client_data_type           client_ref
)
{
  mmgsdi_task_cmd_type             * task_cmd_ptr   = NULL;
  mmgsdi_read_req_type             * msg_ptr        = NULL;
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_ERROR;
  uint32                             task_cmd_len   = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Response CB function Check
    3) offset >= 0 Check : for transparent files
    4) Record Number > 0 chek : for record based files
    4) Data len > 0 Check
    5) Check for EF and Path access
    6) Check for path length and MF/DF/EF identifiers
    7) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_READ_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);
  MMGSDIUTIL_RETURN_IF_NEGATIVE(request_length);

  if ((file_access.access_method != MMGSDI_BY_PATH_ACCESS) &&
      (file_access.access_method != MMGSDI_EF_ENUM_ACCESS))
  {
    UIM_MSG_ERR_1("Invalid Access Method Type 0x%x", file_access.access_method);
    return MMGSDI_INCORRECT_PARAMS;
  }

   if(file_type == MMGSDI_LINEAR_FIXED_FILE &&
     (record_number <= 0 || record_number > MMGSDI_MAX_REC_NUM ||
      request_length > MMGSDI_MAX_DATA_SIZE || request_length <0 ))
  {
    return MMGSDI_ERROR;
  }

  /* enforce an upper limit of file/record size in API */
  if (file_type == MMGSDI_TRANSPARENT_FILE &&
      (offset < 0 || offset > MMGSDI_MAX_DATA_SIZE ||
       request_length > MMGSDI_MAX_DATA_SIZE - offset || request_length < 0))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  if((file_access.access_method == MMGSDI_BY_PATH_ACCESS) &&
     !mmgsdi_util_is_path_valid(&file_access.file.path_type))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  /* Preparing the command first to check whether client is allowed to read the
     EF before SIM initialization*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
    - No need to align pointers
    ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.read_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_read_req_type header
      1) Session ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_READ_REQ;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.request_type      = MMGSDI_READ_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_READ_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_read_req_type content
    - access
      1) Access method in this function is limited to ENUM or PATH access only
      2) File name in the form of file enum type
    - file type
      TRANSPARENT
      Either CYCLIC or LINEAR FIXED is OK, defaulted to LINEAR FIXED
      (they are equilvalent in this context because they shared the same
      APDU parameters)
    - record number
    - offset : for transparent file
    - data len
      Mem malloc and validate, if fail, free memory space and return
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_copy_access_type(&msg_ptr->access, &file_access);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the copying of access type fail*/
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    return mmgsdi_status;
  }

  msg_ptr->file_type             = file_type;
  msg_ptr->rec_num               = record_number;
  msg_ptr->data_len              = request_length;
  msg_ptr->offset                = offset;

  /* Try and read the item from cache first. If the cache read
     fails, we will queue the request to MMGSDI */
  mmgsdi_status = mmgsdi_caller_context_read_cache_and_notify_client(task_cmd_ptr);

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* Item was either read successfully from the cache or the cache revealed
       that the item is not found on the card. Free the command and return the status */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    return mmgsdi_status;
  }

  /* Continue with normal processing, that is, continue with queuing the
     request to MMGSDI */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_READ_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_read */


/* ========================================================================
FUNCTION        MMGSDI_SESSION_READ_RECORD

DESCRIPTION
  This function provides access to any Record File in the SIM/USIM/RUIM/CSIM.
  It allows file access by file enum or path.
  This library function is intended to encapsulate the details required to
  read a file from the SIM/USIM/RUIM/CSIM. The client using this library
  function will only be able to access files that require a record number.

  Behavior for the function for the following file types:

    TRANSPARENT FILE:   Will return an error.
    CYCLIC FILE:        Will access the record of the file provided in the
                        library call.
    LINEAR FIXED FILE:  Will access the record of the file provided in the
                        library call.

  Behavior for the function for the following acess types:
    MMGSDI_EF_ENUM_ACCESS: Will return the contents as required in the
                           Library Call.
    MMGSDI_BY_PATH_ACCESS: Will return the contents as required in the
                           Library Call. Note, there is no check whether the
                           path passed in is record file or not. The
                           caller is expected to have the knowledge of
                           whether the file to access is record or not.
    others:                Returns an Error.

PARAMETERS
  session_id:           Session ID of the caller.
  file_access:          File to read.
  record_number:        Record number, should be equal to or greater than 1.
  request_length:       Request length.
  response_cb_ptr:      Response callback.
  client_ref:           User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  TRANSPARENT FILE:     Will return an error when used to access a
                        transparent file.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

=========================================================================== */
mmgsdi_return_enum_type  mmgsdi_session_read_record (
  mmgsdi_session_id_type             session_id,
  mmgsdi_access_type                 file_access,
  mmgsdi_rec_num_type                record_number,
  mmgsdi_len_type                    request_length,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  return mmgsdi_session_read (session_id,
                              file_access,
                              MMGSDI_LINEAR_FIXED_FILE,
                              0,
                              record_number,
                              request_length,
                              response_cb_ptr,
                              client_ref);
} /* mmgsdi_session_read_record */


/* ===========================================================================
FUNCTION        MMGSDI_SESSION_READ_TRANSPARENT

DESCRIPTION
  This function provides read access to any Transparent File in the
  SIM/USIM/RUIM/CSIM. It allows file access by file enum or path.
  This library function is intended to encapsulate the details required to
  read a file from the SIM/USIM/RUIM/CSIM. The client using this library
  function will only be able to access files that has a flat data structure.
  If the data the already cached by MMGSDI, the request is processed in
  client's context (and data returned from cache) and is not queued to MMGSDI.

  Behavior for the function for the following file types:

    TRANSPARENT FILE:   Will return the contents as required in the Library
                        call.
    CYCLIC FILE:        Returns an Error.
    LINEAR FIXED FILE:  Returns an Error.

  Behavior for the function for the following access type:

    MMGSDI_EF_ENUM_ACCESS: Will return the contents as required in the
                           Library Call.
    MMGSDI_BY_PATH_ACCESS: Will return the contents as required in the
                           Library Call. Note, there is no check whether the
                           path passed in is transparent file or not.  The
                           caller is expected to have the knowledge of
                           whether the file to access is transparent or not.
    others:                Returns an Error.

PARAMETERS
  session_id:           Session ID of the caller.
  file_access:          File to read.
  offset:               Offset from the start of a file.
  req_len:              Request length.
  response_cb_ptr:      Response callback.
  client_ref:           User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  CYCLIC or LINEAR FIXED FILE: Will return an Error when used to access a
                               CYCLIC FILE or CYCLIC FILE.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
  None.
========================================================================== */
mmgsdi_return_enum_type  mmgsdi_session_read_transparent (
  mmgsdi_session_id_type   session_id,
  mmgsdi_access_type       file_access,
  mmgsdi_offset_type       offset,
  mmgsdi_len_type          req_len,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
  return mmgsdi_session_read (session_id,
                              file_access,
                              MMGSDI_TRANSPARENT_FILE,
                              offset,
                              0,
                              req_len,
                              response_cb_ptr,
                              client_ref);
} /* mmgsdi_session_read_transparent */


/* ==========================================================================
FUNCTION        MMGSDI_SESSION_WRITE

DESCRIPTION
  This function will provide write access to any record in a Linear Fixed
  or Cyclic File or to write a transparent file. It allows file access by file enum or path.
  This library function is intended to encapsulate the details required to
  write a file to the SIM/USIM/RUIM/CSIM.

  Behavior for the function for the following file types:

    TRANSPARENT FILE:   write the data provided in the Library call.
    CYCLIC FILE:        Will write the record of the file provided in the
                        library call.
    LINEAR FIXED FILE:  Will write the record of the file provided in the
                        library call.

  Behavior for the function for the following access type:

    MMGSDI_EF_ENUM_ACCESS: Will write the record of the file provided in the
                           library call.
    MMGSDI_BY_PATH_ACCESS: Will write the record of the file provided in the
                           library call. Note, there is no check whether the
                           path passed in is record file or not. The
                           caller is expected to have the knowledge of
                           whether the file to access is record or not.
    others:                Returns an Error.

  Behavior for the function for the following record type:

    MMGSDI_LINEAR_FIXED_FILE:       Will use ABSOULTE access to card
    MMGSDI_CYCLIC_FILE:             Will use PREVIOUS access to card
    MMGSDI_TRANSPARENT_FILE:        Return an Error
    MMGSDI_MASTER_FILE:             Return an Error
    MMGSDI_DEDICATED_FILE:          Return an Error
    MMGSDI_MAX_FILE_STRUCTURE_ENUM: Return an Error

PARAMETERS
  session_id:           Session ID of the caller.
  file_access:          File to write.
  file_type:            file type.
  offset:               Offset from the start of a file. Valid only for transparent files
  record_number:        Record number, should be equal to or greater than 1.
                        Valid only for record-based files
  write_data:           Data to be written.
  response_cb_ptr:      Response callback.
  client_ref:           User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

========================================================================== */
static mmgsdi_return_enum_type  mmgsdi_session_write (
  mmgsdi_session_id_type             session_id,
  mmgsdi_access_type                 file_access,
  mmgsdi_file_structure_enum_type    file_type,
  mmgsdi_offset_type                 offset,  /* Valid only for transparent files */
  mmgsdi_rec_num_type                record_number,  /* Valid only for record-based files */
  mmgsdi_write_data_type             write_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type             * task_cmd_ptr   = NULL;
  mmgsdi_write_req_type            * msg_ptr        = NULL;
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_SUCCESS;
  int32                              total_mem_len  = 0;
  int32                              temp_mem       = 0;
  mmgsdi_slot_id_enum_type           slot_id        = MMGSDI_MAX_SLOT_ID_ENUM;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Response CB function Check
    3) offset >= 0 Check for transparent file
    4) Record number > 0 Check if Linear Fixed
    4) Data len > 0 Check
    5) Check for EF and Path access
    6) Check for path length and MF/DF/EF identifiers
    7) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_WRITE_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL_2(response_cb_ptr, write_data.data_ptr);
  MMGSDIUTIL_RETURN_IF_NEGATIVE(record_number);

  if ((file_access.access_method != MMGSDI_BY_PATH_ACCESS) &&
      (file_access.access_method != MMGSDI_EF_ENUM_ACCESS))
  {
    UIM_MSG_ERR_1("Invalid Access Method Type 0x%x", file_access.access_method);
    return MMGSDI_INCORRECT_PARAMS;
  }

  if(file_type == MMGSDI_LINEAR_FIXED_FILE &&
     (record_number <= 0 || record_number > MMGSDI_MAX_REC_NUM ||
      write_data.data_len <= 0 || write_data.data_len > MMGSDI_MAX_DATA_SIZE))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  if (file_type == MMGSDI_TRANSPARENT_FILE &&
      (write_data.data_len <= 0 ||
       offset < 0 || offset > MMGSDI_MAX_DATA_SIZE ||
       write_data.data_len > MMGSDI_MAX_DATA_SIZE - offset))
  {
    MMGSDIUTIL_DEBUG_MSG_ERROR_1("Invalid Write Data Len: 0x%x ",
                                 write_data.data_len);
    return MMGSDI_INCORRECT_PARAMS;
  }

  if((file_access.access_method == MMGSDI_BY_PATH_ACCESS) &&
     !mmgsdi_util_is_path_valid(&file_access.file.path_type))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* Check if this file is currently being refreshed */
  if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_BLOCK_WRITES_TO_REFRESH_FILES, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC)
       == MMGSDI_FEATURE_ENABLED &&
     mmgsdi_is_file_in_refresh_data_info_list(session_id,
                                              file_access))
  {
    UIM_MSG_ERR_0("Request temporarily not allowed as requested file is being refreshed");
    return MMGSDI_ERROR;
  }

  /* Check if requested file is being purged */
  if ((file_type == MMGSDI_TRANSPARENT_FILE) &&
      (MMGSDI_EF_ENUM_ACCESS  == file_access.access_method) &&
      ((MMGSDI_GSM_LOCI       == file_access.file.file_enum) ||
       (MMGSDI_USIM_LOCI      == file_access.file.file_enum) ||
       (MMGSDI_USIM_PSLOCI    == file_access.file.file_enum) ||
       (MMGSDI_USIM_EPSLOCI   == file_access.file.file_enum)))
  {
    MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
    if (MMGSDI_SUCCESS == mmgsdi_util_get_session_app_info(session_id, &slot_id, NULL,
                                                           NULL, NULL, NULL, NULL))
    {
      if (mmgsdi_util_is_purge_temp_identities_in_progress(slot_id))
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
    }
    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("Request not allowed, file_enum=0x%x is being purged",
                    file_access.file.file_enum);
      return MMGSDI_ERROR;
    }
  }/* XXLOCI file */

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem + write_data.data_len;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, int32touint32(total_mem_len));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.write_req,
                         MMGSDI_WRITE_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.write_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_write_req_type header
      1) Session ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_WRITE_REQ;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.request_type      = MMGSDI_WRITE_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_WRITE_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_write_req_type content
    - access
      1) Access method in this function is limited to ENUM or PATH access only
      2) File name in the form of file enum type
    - file type
      TRANSPARENT
      Either CYCLIC or LINEAR FIXED is OK, defaulted to LINEAR FIXED
      (they are equilvalent in this context because they shared the same
      APDU parameters)
    - offset : for transparent file
    - record index mode
    - record number
    - data len
    - data
      Mem malloc and validate, if fail, free memory space and return
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_copy_access_type(&msg_ptr->access, &file_access);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the copying of access type fail*/
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    return mmgsdi_status;
  }

  msg_ptr->file_type             = file_type;
  msg_ptr->offset                = offset;
  msg_ptr->rec_num               = record_number;
  msg_ptr->data.data_len         = write_data.data_len;
  msg_ptr->int_client_data       = 0;

  mmgsdi_memscpy(msg_ptr->data.data_ptr,
                 int32touint32(msg_ptr->data.data_len),
                 write_data.data_ptr,
                 int32touint32(msg_ptr->data.data_len));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_WRITE_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_session_write */


/* ==========================================================================
FUNCTION        MMGSDI_SESSION_WRITE_RECORD

DESCRIPTION
  This function will provide write access to any record in a Linear Fixed
  or Cyclic File. It allows file access by file enum or path.
  This library function is intended to encapsulate the details required to
  write a file to the SIM/USIM/RUIM/CSIM. The client using this library
  function will only be able to access files that require a record number.

  Behavior for the function for the following file types:

    TRANSPARENT FILE:   Will return an Error.
    CYCLIC FILE:        Will write the record of the file provided in the
                        library call.
    LINEAR FIXED FILE:  Will write the record of the file provided in the
                        library call.

  Behavior for the function for the following access type:

    MMGSDI_EF_ENUM_ACCESS: Will write the record of the file provided in the
                           library call.
    MMGSDI_BY_PATH_ACCESS: Will write the record of the file provided in the
                           library call. Note, there is no check whether the
                           path passed in is record file or not. The
                           caller is expected to have the knowledge of
                           whether the file to access is record or not.
    others:                Returns an Error.

  Behavior for the function for the following record type:

    MMGSDI_LINEAR_FIXED_FILE:       Will use ABSOULTE access to card
    MMGSDI_CYCLIC_FILE:             Will use PREVIOUS access to card
    MMGSDI_TRANSPARENT_FILE:        Return an Error
    MMGSDI_MASTER_FILE:             Return an Error
    MMGSDI_DEDICATED_FILE:          Return an Error
    MMGSDI_MAX_FILE_STRUCTURE_ENUM: Return an Error

PARAMETERS
  session_id:           Session ID of the caller.
  file_access:          File to write.
  record_type:          Record type.
  record_number:        Record number, should be equal to or greater than 1.
  write_data:           Data to be written.
  response_cb_ptr:      Response callback.
  client_ref:           User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  TRANSPARENT FILE:     Will return an Error when used to access a
                        transparent file.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

========================================================================== */
mmgsdi_return_enum_type  mmgsdi_session_write_record (
  mmgsdi_session_id_type             session_id,
  mmgsdi_access_type                 file_access,
  mmgsdi_file_structure_enum_type    record_type,
  mmgsdi_rec_num_type                record_number,
  mmgsdi_write_data_type             write_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  return mmgsdi_session_write (session_id,
                               file_access,
                               record_type,
                               0,
                               record_number,
                               write_data,
                               response_cb_ptr,
                               client_ref);
}/* mmgsdi_session_write_record */


/* ==========================================================================
FUNCTION        MMGSDI_SESSION_WRITE_TRANSPARENT

DESCRIPTION
  This function provides write access to any Transparent File. It allows file
  access by file enum or path.
  This library function is intended to encapsulate the details required to
  write a file to the SIM/USIM/RUIM/CSIM. The client using this library
  function will only be able to access files that has a flat data structure.

  Behavior for the function for the following file types:

    TRANSPARENT FILE:  write the data provided in the Library call.
    CYCLIC FILE:       Returns an Error.
    LINEAR FIXED FILE: Returns an Error.

  Behavior for the function for the following access type:

    MMGSDI_EF_ENUM_ACCESS: Will write the data provided in the Library call.
    MMGSDI_BY_PATH_ACCESS: Will write the data provided in the Library call.
                           Note, there is no check whether the path passed in is
                           transparent file or not. The caller is expected to
                           have the knowledge of whether the file to access is
                           transparent or not.
    others:                Returns an Error.

PARAMETERS
  session_id:           Session ID of the caller.
  file_access:          File to write.
  offset:               Offset from the start of a file.
  write_data:           Data to be written.
  response_cb_ptr:      Response callback.
  client_ref:           User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  CYCLIC or LINEAR FIXED FILE: Will return an Error when used to access a
                               Record based File.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
  MMGSDI cache will be updated if the requested data is located in the cache
========================================================================== */
mmgsdi_return_enum_type  mmgsdi_session_write_transparent (
  mmgsdi_session_id_type           session_id,
  mmgsdi_access_type               file_access,
  mmgsdi_offset_type               offset,
  mmgsdi_write_data_type           write_data,
  mmgsdi_callback_type             response_cb_ptr,
  mmgsdi_client_data_type          client_ref
)
{
  return mmgsdi_session_write (session_id,
                               file_access,
                               MMGSDI_TRANSPARENT_FILE,
                               offset,
                               0,
                               write_data,
                               response_cb_ptr,
                               client_ref);
}/* mmgsdi_session_write_transparent */


/* ==========================================================================
FUNCTION        MMGSDI_SESSION_GET_FILE_ATTR

DESCRIPTION
  This function will provide get file attributes to any file in the
  SIM/USIM/RUIM/CSIM.

  Behavior for the function for the following file types:

    TRANSPARENT FILE:  Will get information on the characteristics of
                       transparent file
    CYCLIC FILE:       Will get information on the characteristics of
                       cyclic file.
    LINEAR FIXED FILE: Will get information on the characteristics of
                       linear Fixed file.
    MASTER FILE:       Will get information on the Master File
    DEDICATED FILE:    Will get information on first level DFs

PARAMETERS
  session_id:           Session ID of the caller
  file_name:            File name
  response_cb_ptr:      Response callback.
  client_ref:           User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

==========================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_get_file_attr (
  mmgsdi_session_id_type            session_id,
  mmgsdi_access_type                file_name,
  mmgsdi_callback_type              response_cb_ptr,
  mmgsdi_client_data_type           client_ref
)
{
  return mmgsdi_session_get_file_attr_ext(session_id,
                                          file_name,
                                          response_cb_ptr,
                                          client_ref,
                                          FALSE);

} /* mmgsdi_session_get_file_attr */


/* ==========================================================================
FUNCTION        MMGSDI_SESSION_GET_FILE_ATTR_EXT

DESCRIPTION
  Gets file attributes from any file in the SIM, USIM, RUIM, or CSIM card.
  The way it is different from mmgsdi_session_get_file_attr() is that the
  mmgsdi_session_get_file_attr_ext() lets the caller specify whether they
  want to skip the EF-ARR check for the file. If client chooses to do so,
  it will receive ALWAYS as the security attribute for the file in the cnf.
  This API does not achieve anything different from
  mmgsdi_session_get_file_attr() for the UICC cards for which all the
  accessibility information is in the FCP (instead of a reference to EF-ARR).

  The skip_uicc_arr parameter is applicable only to UICC. For ICC, it is
  ignored by the implementation.

  The function behaves as follows for these file types:

    Transparent file:    Gets information on the characteristics of a
                          transparent file.
    Cyclic file:         Gets information on the characteristics of a
                          cyclic file.
    Linear fixed file:   Gets information on the characteristics of a linear
                          fixed file.
    Master file:         Gets information on the master file.
    Dedicated file:      Gets information on the dedicated file.

PARAMETERS
  session_id       Session ID of the caller.
  file_name        File name.
  response_cb_ptr  Pointer to the response callback.
  client_ref       User data returned upon completion of this
                   command.
  skip_uicc_arr    Skip reading EF-ARR for security attributes
                   of the file.

DEPENDENCIES
  A valid session ID is required.

LIMITATIONS

RETURN VALUE
  MMGSDI_SUCCESS:            Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS:   Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL:     Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

SIDE EFFECTS

==========================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_get_file_attr_ext (
  mmgsdi_session_id_type            session_id,
  mmgsdi_access_type                file_name,
  mmgsdi_callback_type              response_cb_ptr,
  mmgsdi_client_data_type           client_ref,
  boolean                           skip_uicc_arr
)
{
  mmgsdi_task_cmd_type             * task_cmd_ptr      = NULL;
  mmgsdi_return_enum_type            mmgsdi_status     = MMGSDI_SUCCESS;
  mmgsdi_get_file_attr_req_type    * get_file_attr_msg = NULL;
  uint32                             task_cmd_len      = 0;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. session_id
    2. file_name.access - MMGSDI_BY_PATH_ACCESS or MMGSDI_EF_ENUM_ACCESS
    3. response_cb_ptr  - Cannot be NULL
    4. client_ref
  ---------------------------------------------------------------------------*/
  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Response CB function Check
    3) file_name.access -
        MMGSDI_BY_PATH_ACCESS/MMGSDI_EF_ENUM_ACCESS/MMGSDI_DF_ENUM_ACCESS
    4) If access method is by Path, path validation
    5) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_GET_FILE_ATTR_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  if ((file_name.access_method != MMGSDI_BY_PATH_ACCESS) &&
      (file_name.access_method != MMGSDI_EF_ENUM_ACCESS) &&
      (file_name.access_method != MMGSDI_DF_ENUM_ACCESS))
  {
    UIM_MSG_ERR_1("Invalid Access Method Type 0x%x", file_name.access_method);
    return MMGSDI_INCORRECT_PARAMS;
  }

  if((file_name.access_method == MMGSDI_BY_PATH_ACCESS) &&
     !mmgsdi_util_is_path_valid(&file_name.file.path_type))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No need to align pointers
    ---------------------------------------------------------------------------*/
  get_file_attr_msg = &task_cmd_ptr->cmd.cmd.get_file_attr_req;

  /*---------------------------------------------------------------------------
    POPULATE THE REQUEST MESSAGE PAYLOAD
    1. session_id      - Session ID Obtained
    2. request_type    - Request Command Type
    3. client_data     - Pointer to Client Data
    4. response_cb     - Response Callback Pointer
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                          = MMGSDI_GET_FILE_ATTR_REQ;
  get_file_attr_msg->request_header.session_id        = session_id;
  get_file_attr_msg->request_header.request_type      = MMGSDI_GET_FILE_ATTR_REQ;
  get_file_attr_msg->request_header.orig_request_type = MMGSDI_GET_FILE_ATTR_REQ;
  get_file_attr_msg->request_header.client_data       = client_ref;
  get_file_attr_msg->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    POPULATE THE REQUEST MESSAGE PAYLOAD
    1.  access_method       -  Access type specified in the function paramters
    2.  file_enum           -  File name specified in the function paramters
                           OR
    2a. path                -  Path specified in the function paramters
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_copy_access_type(&get_file_attr_msg->access,
                                               &file_name);
  if (mmgsdi_status != MMGSDI_SUCCESS) {
    MMGSDIUTIL_DEBUG_MSG_ERROR_1("Get File Attr Lib Fail: Access Copy Fail 0x%x",
                                 mmgsdi_status);
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    return mmgsdi_status;
  }

  get_file_attr_msg->activate_aid    = TRUE;
  get_file_attr_msg->int_client_data = 0;
  get_file_attr_msg->skip_uicc_arr   = skip_uicc_arr;

  /* PLACE ON GSDI QUEUE */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
    onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_GET_FILE_ATTR_REQ status 0x%x",
                 mmgsdi_status);

  return MMGSDI_SUCCESS;
} /* mmgsdi_session_get_file_attr_ext */


/* ============================================================================
FUNCTION        MMGSDI_SESSION_ISIM_AUTHENTICATE

DESCRIPTION
  This function is used to perform a "Network" Initiated Authentication with
  the ISIM Application.  This is only valid and applicable when an ISIM
  Application is available as specified in 31.103.  This function will
  perform a check for the ISIM Application before sending the command.

PARAMETERS
  mmgsdi_session_id_type   session_id:     Session ID of the caller
  mmgsdi_data_type         auth_req:       Length and Data of auth
                                           challenge.
  mmgsdi_callback_type     response_cb_ptr Client callback function
  mmgsdi_client_data_type  client_ref:     User Data returned upon
                                           completion of this cmd.

DEPENDENCIES
  The client must also have a valid session open.
  Achieved by calling mmgsdi_session_open().

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

========================================================================== */
mmgsdi_return_enum_type  mmgsdi_session_isim_authenticate (
  mmgsdi_session_id_type   session_id,
  mmgsdi_data_type         auth_req,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
  return mmgsdi_session_uicc_authenticate(session_id,
                                          MMGSDI_AUTN_ISIM_CONTEXT,
                                          auth_req,
                                          response_cb_ptr,
                                          client_ref);
} /* mmgsdi_session_isim_authenticate */


/*===========================================================================
FUNCTION        MMGSDI_SESSION_USIM_AUTHENTICATE

DESCRIPTION
  This function is used to perform a "Network" Initiated Authentication with
  the USIM Application.  This is only valid and applicable when an USIM
  Application is available . This function will perform a check for the USIM
  Application before sending the command.

PARAMETERS
  mmgsdi_session_id_type   session_id:     Session ID of the caller
  mmgsdi_data_type         auth_req:       Length and Data of auth
                                           challenge.
  mmgsdi_client_data_type  client_ref:     User Data returned upon
                                           completion of this cmd.

DEPENDENCIES
  The client must also have a valid session open if the USIM Application
  is not the default application
  Achieved by calling mmgsdi_session_open().

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_usim_authenticate (
  mmgsdi_session_id_type              session_id,
  mmgsdi_autn_context_enum_type       auth_context,
  mmgsdi_data_type                    auth_req,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
)
{
  return mmgsdi_session_usim_authenticate_ext(session_id,
                                              auth_context,
                                              auth_req,
                                              FALSE,
                                              response_cb_ptr,
                                              client_ref);
}/*mmgsdi_session_usim_authenticate*/


/*===========================================================================
FUNCTION        MMGSDI_SESSION_USIM_AUTHENTICATE_EXT

DESCRIPTION
  This function is used to perform a "Network" Initiated Authentication with
  the USIM Application.  This is only valid and applicable when an USIM
  Application is available. The caller of this function can specify whether
  to perform a check on application state before sending the command to card.

PARAMETERS
  mmgsdi_session_id_type   session_id:          Session ID of the caller
  mmgsdi_data_type         auth_req:            Length and Data of auth
                                                challenge.
  boolean                  illegal_or_perso_ok: Flag to indicate if the cmd
                                                can be executed if app state
                                                is ILLEGAL or PERSO failed.
  mmgsdi_client_data_type  client_ref:          User Data returned upon
                                                completion of this cmd.

DEPENDENCIES
  The client must also have a valid session open if the USIM Application
  is not the default application
  Achieved by calling mmgsdi_session_open().

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_usim_authenticate_ext (
  mmgsdi_session_id_type              session_id,
  mmgsdi_autn_context_enum_type       auth_context,
  mmgsdi_data_type                    auth_req,
  boolean                             illegal_or_perso_ok,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
)
{
  mmgsdi_task_cmd_type              *task_cmd_ptr   = NULL;
  mmgsdi_auth_req_type              *msg_ptr        = NULL;
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_ERROR;
  int32                              total_mem_len  = 0;
  int32                              temp_mem       = 0;
  mmgsdi_cmd_enum_type               cmd_enum       = MMGSDI_MAX_CMD_ENUM;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response CB function Check
    2) Memory allocation for msg_ptr and check for allocation
    3) Session ID
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  if ((auth_req.data_len <= 0) || (auth_req.data_ptr == NULL))
  {
    UIM_MSG_ERR_0("Authenticate data length or data invalid");
    return MMGSDI_ERROR;
  }

  switch (auth_context)
  {
    case MMGSDI_AUTN_USIM_GSM_CONTEXT:
    case MMGSDI_AUTN_USIM_3G_CONTEXT:
    case MMGSDI_AUTN_USIM_VGCS_VBS_CONTEXT:
    case MMGSDI_AUTN_USIM_GBA_BOOTSTRAPPING_CONTEXT:
    case MMGSDI_AUTN_USIM_GBA_NAF_DERIVATION_CONTEXT:
    case MMGSDI_AUTN_USIM_MBMS_MSK_UPDATE_CONTEXT:
    case MMGSDI_AUTN_USIM_MBMS_SPE_DEL_CONTEXT:
    case MMGSDI_AUTN_USIM_MBMS_REC_DEL_CONTEXT:
    case MMGSDI_AUTN_USIM_MBMS_MTK_GEN_CONTEXT:
    case MMGSDI_AUTN_USIM_MBMS_MSK_DEL_CONTEXT:
    case MMGSDI_AUTN_USIM_MBMS_MUK_DEL_CONTEXT:
      cmd_enum = MMGSDI_USIM_AUTH_REQ;
      break;
    default:
      UIM_MSG_ERR_0("Unsupported Auth Context");
      return MMGSDI_NOT_SUPPORTED;
  }

  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    cmd_enum);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem + auth_req.data_len;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, int32touint32(total_mem_len));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
    ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.auth_req,
                         cmd_enum);

  msg_ptr = &task_cmd_ptr->cmd.cmd.auth_req;

  /*---------------------------------------------------------------------------
      Populating mmgsdi_auth_req_type header
      1) Session ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/

  task_cmd_ptr->cmd.cmd_enum                = cmd_enum;
  msg_ptr->request_header.request_type      = cmd_enum;
  msg_ptr->request_header.orig_request_type = cmd_enum;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;
  msg_ptr->auth_context                     = auth_context;
  msg_ptr->illegal_or_perso_ok              = illegal_or_perso_ok;

/*---------------------------------------------------------------------------
    Populating auth req content
  ---------------------------------------------------------------------------*/
  msg_ptr->auth_req.data_len = auth_req.data_len;
  mmgsdi_memscpy(msg_ptr->auth_req.data_ptr,
                 int32touint32(msg_ptr->auth_req.data_len),
                 auth_req.data_ptr,
                 int32touint32(auth_req.data_len));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_2("Queue of MMGSDI command: 0x%x status 0x%x",
                 cmd_enum, mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_session_usim_authenticate_ext */


/*===========================================================================
FUNCTION        MMGSDI_SESSION_UICC_AUTHENTICATE

DESCRIPTION
  This function is used to perform a "Network" Initiated Authentication with
  a UICC Application.  This function will perform a check for the
  application before sending the command.

PARAMETERS
  mmgsdi_session_id_type        session_id:      Session ID of the caller
  mmgsdi_autn_context_enum_type auth_context:    Context of authentication
  mmgsdi_data_type              auth_req:        Length and data of auth
                                                 challenge.
  mmgsdi_callback_type          response_cb_ptr: Client call back
  mmgsdi_client_data_type       client_ref:      User data returned upon
                                                 completion of this cmd.

DEPENDENCIES
  The client must have a valid session open this can be achieved
  by calling mmgsdi_session_open().

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_uicc_authenticate (
  mmgsdi_session_id_type              session_id,
  mmgsdi_autn_context_enum_type       auth_context,
  mmgsdi_data_type                    auth_req,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
)
{
  mmgsdi_task_cmd_type              *task_cmd_ptr   = NULL;
  mmgsdi_auth_req_type              *msg_ptr        = NULL;
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_ERROR;
  int32                              total_mem_len  = 0;
  int32                              temp_mem       = 0;
  mmgsdi_cmd_enum_type               cmd_enum       = MMGSDI_MAX_CMD_ENUM;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response CB function Check
    2) Memory allocation for msg_ptr and check for allocation
    3) Session ID
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  if ((auth_req.data_len <= 0) || (auth_req.data_ptr == NULL))
  {
    UIM_MSG_ERR_0("Authenticate data length or data invalid");
    return MMGSDI_ERROR;
  }

  switch (auth_context)
  {
    case MMGSDI_AUTN_USIM_GSM_CONTEXT:
    case MMGSDI_AUTN_USIM_3G_CONTEXT:
    case MMGSDI_AUTN_USIM_VGCS_VBS_CONTEXT:
    case MMGSDI_AUTN_USIM_GBA_BOOTSTRAPPING_CONTEXT:
    case MMGSDI_AUTN_USIM_GBA_NAF_DERIVATION_CONTEXT:
    case MMGSDI_AUTN_USIM_MBMS_MSK_UPDATE_CONTEXT:
    case MMGSDI_AUTN_USIM_MBMS_SPE_DEL_CONTEXT:
    case MMGSDI_AUTN_USIM_MBMS_REC_DEL_CONTEXT:
    case MMGSDI_AUTN_USIM_MBMS_MTK_GEN_CONTEXT:
    case MMGSDI_AUTN_USIM_MBMS_MSK_DEL_CONTEXT:
    case MMGSDI_AUTN_USIM_MBMS_MUK_DEL_CONTEXT:
      cmd_enum = MMGSDI_USIM_AUTH_REQ;
      break;
    case MMGSDI_AUTN_ISIM_CONTEXT:
    case MMGSDI_AUTN_ISIM_GBA_BOOTSTRAPPING_CONTEXT:
    case MMGSDI_AUTN_ISIM_GBA_NAF_DERIVATION_CONTEXT:
    case MMGSDI_AUTN_ISIM_HTTP_DIGEST_CONTEXT:
      cmd_enum = MMGSDI_ISIM_AUTH_REQ;
      break;
    default:
      UIM_MSG_ERR_0("Unsupported Auth Context");
      return MMGSDI_NOT_SUPPORTED;
  }

  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    cmd_enum);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem + auth_req.data_len;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, int32touint32(total_mem_len));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
    ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.auth_req,
                         cmd_enum);

  msg_ptr = &task_cmd_ptr->cmd.cmd.auth_req;

  /*---------------------------------------------------------------------------
      Populating mmgsdi_(u/i)sim_auth_req_type header
      1) Session ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/

  task_cmd_ptr->cmd.cmd_enum                = cmd_enum;
  msg_ptr->request_header.request_type      = cmd_enum;
  msg_ptr->request_header.orig_request_type = cmd_enum;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;
  msg_ptr->auth_context                     = auth_context;
  msg_ptr->illegal_or_perso_ok              = FALSE;

/*---------------------------------------------------------------------------
    Populating auth req content
  ---------------------------------------------------------------------------*/
  msg_ptr->auth_req.data_len = auth_req.data_len;
  mmgsdi_memscpy(msg_ptr->auth_req.data_ptr,
                 int32touint32(msg_ptr->auth_req.data_len),
                 auth_req.data_ptr,
                 int32touint32(msg_ptr->auth_req.data_len));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_2("Queue of MMGSDI command: 0x%x status 0x%x",
                 cmd_enum, mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_uicc_authenticate */


/*===========================================================================
FUNCTION:       MMGSDI_SESSION_RUN_GSM_ALGO

DESCRIPTION:
  This function is used to perform a "Network" Initiated Authentication with
  the SIM card. This is only valid and applicable when an SIM card is
  available.

  Description of the parameters:
  session_id:                Session Id of the caller
  rand:                      Length and Data of RAND.
  response_cb_ptr:           Call back function when the command execution is
                             done.
  client_ref:                User Data returned upon completion of this cmd.

DEPENDENCIES:
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_run_gsm_algo (
  mmgsdi_session_id_type              session_id,
  mmgsdi_data_type                    rand_data,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
)
{
  return mmgsdi_session_run_gsm_algo_ext(session_id,
                                         rand_data,
                                         FALSE,
                                         response_cb_ptr,
                                         client_ref);
}/* mmgsdi_session_run_gsm_algo */


/*===========================================================================
FUNCTION:       MMGSDI_SESSION_RUN_GSM_ALGO_EXT

DESCRIPTION:
  This function is used to perform a "Network" Initiated Authentication with
  the SIM card. This is only valid and applicable when an SIM card is
  available. The caller of this function can specify whether to perform a
  check on application state before sending the command to card.

  Description of the parameters:
  session_id:                Session Id of the caller
  rand:                      Length and Data of RAND.
  illegal_or_perso_ok:       Flag to indicate if this command can be executed
                             if app state is ILLEGAL or PERSO failed.
  response_cb_ptr:           Call back function when the command execution is
                             done.
  client_ref:                User Data returned upon completion of this cmd.

DEPENDENCIES:
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_run_gsm_algo_ext (
  mmgsdi_session_id_type              session_id,
  mmgsdi_data_type                    rand_data,
  boolean                             illegal_or_perso_ok,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
)
{
  mmgsdi_task_cmd_type                   *task_cmd_ptr   = NULL;
  mmgsdi_session_run_gsm_algo_req_type   *msg_ptr        = NULL;
  mmgsdi_return_enum_type                 mmgsdi_status  = MMGSDI_ERROR;
  int32                                   total_mem_len  = 0;
  int32                                   temp_mem       = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Rand
    3) Response CB function Check
    4) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_RUN_GSM_ALGO_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL_2(rand_data.data_ptr,
                              response_cb_ptr);

  if (rand_data.data_len <= 0)
  {
     return MMGSDI_INCORRECT_PARAMS;
  }

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem + rand_data.data_len;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, int32touint32(total_mem_len));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.session_run_gsm_algo_req,
                         MMGSDI_SESSION_RUN_GSM_ALGO_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.session_run_gsm_algo_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_generate_key_req_type header
      1) Session ID
      2 Request TYPE
      3 Client Data Pointer
      4 Response CallBack
      5 Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SESSION_RUN_GSM_ALGO_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_RUN_GSM_ALGO_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_RUN_GSM_ALGO_REQ;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;
  msg_ptr->illegal_or_perso_ok              = illegal_or_perso_ok;
   /*---------------------------------------------------------------------------
    Populating RUN GSM Algo DATA
  ---------------------------------------------------------------------------*/
  msg_ptr->rand_val.data_len = rand_data.data_len;
  mmgsdi_memscpy(msg_ptr->rand_val.data_ptr,
                 int32touint32(msg_ptr->rand_val.data_len),
                 rand_data.data_ptr,
                 int32touint32(msg_ptr->rand_val.data_len));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_RUN_GSM_ALGO_REQ Algo status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_run_gsm_algo_ext */


/* ============================================================================
FUNCTION        MMGSDI_SESSION_SEND_CARD_STATUS

DESCRIPTION
  GSM 11.11 functionality:
    This function returns information concerning the current directory.
    A current EF is not affected by the STATUS function. It is also used to
    give an opportunity for a pro active SIM to indicate that the SIM
    wants to issue a SIM Application Toolkit command to the ME.

    Status and return type is not required when using a GSM SIM Card.

  UMTS 31.102 Functionality:
    This function returns information concerning the current directory or current
    application.  In addition, according to the application specification,
    it may be used to indicate to the application in the UICC that its
    session activation procedure has been successfully executed or that
    its termination procedure will be executed.
    NOTE: These indications may be used to synchronize the applications in the
    terminal and in the UICC.

    A status and return type is required when using a UICC Card with a USIM
    application.

PARAMETERS
  session_id:      Session ID of the caller
  me_app_status:   The status of the application in the handset
  ret_data_struct: Structure in which the data should be returned in
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
  None.
========================================================================== */
mmgsdi_return_enum_type mmgsdi_session_send_card_status(
  mmgsdi_session_id_type           session_id,
  mmgsdi_status_me_app_enum_type   me_app_status,
  mmgsdi_status_ret_data_enum_type ret_data_struct,
  mmgsdi_callback_type             response_cb_ptr,
  mmgsdi_client_data_type          client_ref
)
{
  mmgsdi_task_cmd_type             * task_cmd_ptr   = NULL;
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_ERROR;
  mmgsdi_status_req_type           * msg_ptr        = NULL;
  uint32                             task_cmd_len   = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Response Callback Pointer Check
    3) Verify the Params provided are within the valid range
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_CARD_STATUS_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(me_app_status,
                                    MMGSDI_STATUS_APP_NONE,
                                    MMGSDI_STATUS_APP_TERMINATED);
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(ret_data_struct,
                                    MMGSDI_STATUS_NONE,
                                    MMGSDI_STATUS_NO_DATA);

  /* --------------------------------------------------------------------------
     Allocate the memory for this request
     ------------------------------------------------------------------------*/
  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No need to align pointers
    ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.status_req;

  /* ----------------------------------------------------------------------------
     Populate the command Request Header with the information provided by
     the client.
     ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_CARD_STATUS_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_CARD_STATUS_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_CARD_STATUS_REQ;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* --------------------------------------------------------------------------
     Populate the command details with the information provided by the client
     ------------------------------------------------------------------------*/
  msg_ptr->me_app_status   = me_app_status;
  msg_ptr->ret_data_struct = ret_data_struct;

  /* --------------------------------------------------------------------------
     Queue the command, free the command, and return the status
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
    onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_CARD_STATUS_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_session_send_card_status */


/* ============================================================================
FUNCTION        MMGSDI_SESSION_CHANGE_SERVICE_STATUS

DESCRIPTION
  This function enables/disables BDN, ACL, and FDN service.

PARAMETERS
  session_id:      Session ID of the caller.
  service:         Service type (BDN, ACL, or FDN).
  enable_service:  to enable or disable the service
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.
    MMGSDI_NOT_SUPPORTED:    When used to enable services other than BDN or
                             ACL.

SIDE EFFECTS

========================================================================== */
static mmgsdi_return_enum_type mmgsdi_session_change_service_status (
  mmgsdi_session_id_type             session_id,
  mmgsdi_service_enum_type           service,
  boolean                            enable_service,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type                    * task_cmd_ptr    = NULL;
  mmgsdi_return_enum_type                   mmgsdi_status   = MMGSDI_ERROR;
  mmgsdi_session_enable_service_req_type  * enable_msg_ptr  = NULL;
  mmgsdi_session_disable_service_req_type * disable_msg_ptr = NULL;
  uint32                                    task_cmd_len   = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Response Callback Pointer Check
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    enable_service? MMGSDI_SESSION_ENABLE_SERVICE_REQ :
                                    MMGSDI_SESSION_DISABLE_SERVICE_REQ);


  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* --------------------------------------------------------------------------
     Allocate the memory for this request
     ------------------------------------------------------------------------*/
  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No need to align pointers
  ---------------------------------------------------------------------------*/

  /* ----------------------------------------------------------------------------
     Populate the command Request Header with the information provided by
     the client.
     ---------------------------------------------------------------------------*/

  if(enable_service)
  {
    enable_msg_ptr = &task_cmd_ptr->cmd.cmd.session_enable_service_req;
    task_cmd_ptr->cmd.cmd_enum                       = MMGSDI_SESSION_ENABLE_SERVICE_REQ;
    enable_msg_ptr->request_header.request_type      = MMGSDI_SESSION_ENABLE_SERVICE_REQ;
    enable_msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_ENABLE_SERVICE_REQ;
    enable_msg_ptr->request_header.session_id        = session_id;
    enable_msg_ptr->request_header.client_data       = client_ref;
    enable_msg_ptr->request_header.response_cb       = response_cb_ptr;
    enable_msg_ptr->service                          = service;
  }
  else
  {
    disable_msg_ptr = &task_cmd_ptr->cmd.cmd.session_disable_service_req;
    task_cmd_ptr->cmd.cmd_enum                        = MMGSDI_SESSION_DISABLE_SERVICE_REQ;
    disable_msg_ptr->request_header.request_type      = MMGSDI_SESSION_DISABLE_SERVICE_REQ;
    disable_msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_DISABLE_SERVICE_REQ;
    disable_msg_ptr->request_header.session_id        = session_id;
    disable_msg_ptr->request_header.client_data       = client_ref;
    disable_msg_ptr->request_header.response_cb       = response_cb_ptr;
    disable_msg_ptr->service                          = service;
  }

  /* --------------------------------------------------------------------------
     Queue the command, free the command, and return the status
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  if(enable_service)
  {
    UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_ENABLE_SERVICE_REQ status 0x%x",
                 mmgsdi_status);
  }
  else
  {
    UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_DISABLE_SERVICE_REQ status 0x%x",
                   mmgsdi_status);
  }

  return mmgsdi_status;
}/* mmgsdi_session_change_service_status */


/* ============================================================================
FUNCTION        MMGSDI_SESSION_ENABLE_SERVICE

DESCRIPTION
  This function enables BDN, ACL, and FDN service.

PARAMETERS
  session_id:      Session ID of the caller.
  service:         Service type (BDN, ACL, or FDN).
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.
    MMGSDI_NOT_SUPPORTED:    When used to enable services other than BDN or
                             ACL.

SIDE EFFECTS

========================================================================== */
mmgsdi_return_enum_type mmgsdi_session_enable_service (
  mmgsdi_session_id_type             session_id,
  mmgsdi_service_enum_type           service,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  return mmgsdi_session_change_service_status (session_id,
                                               service,
                                               TRUE,
                                               response_cb_ptr,
                                               client_ref);
}/*mmgsdi_session_enable_service*/


/* ============================================================================
FUNCTION        MMGSDI_SESSION_DISABLE_SERVICE

DESCRIPTION
  This function disables ACL, BDN, or FDN service.

PARAMETERS
  session_id:      Session ID of the caller.
  service:         Service type.
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.
    MMGSDI_NOT_SUPPORTED:    When used to disable services other than BDN or
                             ACL.

SIDE EFFECTS

========================================================================== */
mmgsdi_return_enum_type mmgsdi_session_disable_service (
  mmgsdi_session_id_type             session_id,
  mmgsdi_service_enum_type           service,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  return mmgsdi_session_change_service_status (session_id,
                                               service,
                                               FALSE,
                                               response_cb_ptr,
                                               client_ref);
}/*mmgsdi_session_disable_service*/


/*===========================================================================
FUNCTION        MMGSDI_SESSION_IS_SERVICE_AVAILABLE

DESCRIPTION
  This function finds out if the service is available or not given the
  service_type

PARAMETERS
  session_id:      Session ID of the caller
  srvc_type:       Service that the client is interested in
  response_cb_ptr: Response callback.
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
    None.
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_is_service_available (
  mmgsdi_session_id_type              session_id,
  mmgsdi_service_enum_type            srvc_type,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
)
{
  mmgsdi_task_cmd_type             * task_cmd_ptr   = NULL;
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_ERROR;
  mmgsdi_srv_available_req_type    * msg_ptr        = NULL;
  uint32                             task_cmd_len   = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Response Callback Pointer Check
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SRV_AVAILABLE_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /* --------------------------------------------------------------------------
     Allocate the memory for this request
     ------------------------------------------------------------------------*/
  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No need to align pointers
    ---------------------------------------------------------------------------*/
  msg_ptr = &task_cmd_ptr->cmd.cmd.srv_available_req;

  /* ----------------------------------------------------------------------------
     Populate the command Request Header with the information provided by
     the client.
     ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SRV_AVAILABLE_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_SRV_AVAILABLE_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SRV_AVAILABLE_REQ;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;
  msg_ptr->service                          = srvc_type;

  /* check the service status from cached service table first. If the service
     is not cached, we will queue the request to MMGSDI */
  mmgsdi_status = mmgsdi_caller_context_check_service_and_notify_client(task_cmd_ptr);

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* Item was either read successfully from the cache or the cache revealed
       that the item is not found on the card. Free the command and return the status */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    return mmgsdi_status;
  }

  /* Continue with normal processing, that is, continue with queuing the
     request to MMGSDI */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SRV_AVAILABLE_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/*mmgsdi_session_is_service_available*/ /*lint !e715*/


/*===========================================================================
FUNCTION        MMGSDI_SESSION_JCDMA_GET_CARD_INFO

DESCRIPTION
  This function will build a request to the MMGSDI Task for the JCDMA
  card information. It should only be called for JCDMA phones from the OEM
  layer at powerup stage after the Subscription Ready event.

PARAMETERS
  session_id:        Session ID of the caller
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  PIN1 verification should be done and Subscription Ready event should be
  received before calling this function.

LIMITATIONS

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_NOT_SUPPORTED: The command is no longer supported as part of
                          JCDMA code removal

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_jcdma_get_card_info (
  mmgsdi_session_id_type             session_id,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  UIM_MSG_LOW_0("JCDMA APIs are no longer supported");

  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_session_jcdma_get_card_info */ /*lint !e715*/


/*=============================================================================
FUNCTION:       MMGSDI_SESSION_REGISTER_FOR_REFRESH_SYNC
=============================================================================*/
/**
  This function allows the client to register for file change notifications for
  specific files and/or vote for refresh (i.e. vote for either taking the call stack down
  or terminating application that the client is interested in) and/or sending approval for
  proceeding with refresh. When the FCN request is received, then MMGSDI will send an 
  event MMGSDI_REFRESH_EVT with mode = MMGSDI_REFRESH_FCN with the file list that has changed.
  MMGSDI will filter out the file list based on what the client registered for.
  Some FCNs on default could potentially result in bringing the protocol stack down.
  If the client is interested in these files, then they should also register for voting 
  explicitly on default app.
  Some refresh scenario may result in a card powerdown. If a client is interested in providing
  approval before MMGSDI can proceed with refresh and do a card power down later, they do it
  using proceed_with_refresh flag.
  A Refresh vote means to either take the call stack down or terminate an application.

  @parameter
  session_id:           Session ID of the caller
  refresh_files:        Files that the Client is interested in getting notification for
  vote_for_init:        Vote to ok bringing the call stack down/terminating
                        the application
  proceed_with_refresh: Receive notification before starting refresh.

  @return
  MMGSDI_SUCCESS:           Registration for refresh is processed successfully
  MMGSDI_INCORRECT_PARAMS:  The parameters supplied to the API are not
                            within appropriate ranges.

  @dependencies
  A valid Session ID is required.

  @limitations
  This API will be limited to the session on which the client is.
  If the client is interested in default app, then the appropriate client id should be used.

  @side effects
  None
*/
mmgsdi_return_enum_type   mmgsdi_session_register_for_refresh_sync
(
  mmgsdi_session_id_type             session_id,
  mmgsdi_refresh_file_list_type      refresh_files,
  boolean                            vote_for_init,
  boolean                            proceed_with_refresh
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_refresh_req_type req           = {{0}};
  mmgsdi_client_id_type   client_id     = MMGSDI_CLIENT_ID_ZERO; 

  req.refresh_req_type = MMGSDI_REFRESH_ORIG_REG_REQ;

  MMGSDIUTIL_CHK_REFRESH_FILE_PTRS(refresh_files.file_list_ptr,
    refresh_files.file_path_ptr, mmgsdi_status);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
  /* Validate the Session ID */
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_REFRESH_REQ);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }
  
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    if(mmgsdi_util_get_client_id(session_id, &client_id) != MMGSDI_SUCCESS)
    { 
      mmgsdi_status =  MMGSDI_ERROR;
      break;
    }

    mmgsdi_status = mmgsdi_parse_refresh_file_list(refresh_files, &req);
 
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }
 
    req.request_header.session_id        = session_id;
    req.request_header.client_id         = client_id;
    req.refresh_req_type                 = MMGSDI_REFRESH_ORIG_REG_REQ;
    req.refresh.reg_req.vote_for_init    = vote_for_init;
    req.refresh.reg_req.ok_to_proceed    = proceed_with_refresh;
    req.request_header.request_type      = MMGSDI_REFRESH_REQ;
    req.request_header.orig_request_type = MMGSDI_REFRESH_REQ;
    req.request_header.slot_id           = MMGSDI_SLOT_AUTOMATIC;
    req.request_header.client_data       = 0;
    req.request_header.response_cb       = NULL;

    mmgsdi_status = mmgsdi_refresh_req_process_reg(&req);
  }while(0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  /* free the file_path if required */
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
    req.refresh.reg_req.refresh_files.file_path_ptr);

  return mmgsdi_status;
}/* mmgsdi_session_register_for_refresh_sync */ 


/*=============================================================================
FUNCTION:       MMGSDI_SESSION_DEREGISTER_FOR_REFRESH_SYNC

  This function allows the client deregister all the files

  session_id:       Session ID of the caller
  vote_for_init:    Deregister from Voting,
                    FALSE   No changes to Current Registration,
                    TRUE    DeRegister from Voting


  @dependencies
  A valid Session ID is required.

  @return
  MMGSDI_SUCCESS:            Request is processed successfully
  MMGSDI_INCORRECT_PARAMS:   The parameters supplied to the API are not
                             within appropriate ranges.

  @limitations
  None

  @side effects
  None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_deregister_for_refresh_sync
(
  mmgsdi_session_id_type             session_id,
  boolean                            vote_for_init
)
{
  mmgsdi_return_enum_type      mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_session_info_type    *session_ptr   = NULL;

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  /*Get the Session Pointer*/
  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                    session_id,
                    &session_ptr,
                    NULL);
  if(mmgsdi_status == MMGSDI_SUCCESS && session_ptr != NULL)
  {
    /*All files dergistered*/
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(session_ptr->refresh_data.refresh_files.file_path_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(session_ptr->refresh_data.refresh_files.file_list_ptr);
    session_ptr->refresh_data.refresh_files.num_files = 0;
    session_ptr->steering_of_roaming    = FALSE;
    session_ptr->steering_complete_rcvd = FALSE;
    session_ptr->refresh_data.refresh_files.num_file_paths = 0;
	
    /*Change Voting only if client is specifically deregistering*/
    if(vote_for_init)
    {
      session_ptr->refresh_data.vote_for_init = FALSE;
    }
  }

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  return mmgsdi_status;
}/* mmgsdi_session_deregister_for_refresh_sync */


/*=============================================================================
FUNCTION        MMGSDI_SESSION_REFRESH_COMPLETE

DESCRIPTION
      This function allows the client to inform MMGSDI when it has finished all
      the files that were refreshed.

PARAMETERS
  session_id:       Session ID of the caller
  pass_fail:        Boolean indicating whether client was able to
                    successfully read all the files that were refreshed
                    This could also be applicable to init cases, where the
                    client would inform MMGSDI that it was able to perform
                    its init properly
  response_cb_ptr:  Callback to this command
  client_ref:       User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The client should have also registered for FCNs and should have also received
  a notification for FCN before they call this function.

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_session_refresh_complete (
   mmgsdi_session_id_type             session_id,
   boolean                            pass_fail,
   mmgsdi_callback_type               response_cb_ptr,
   mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_refresh_req_type req           = {{0}};

  req.refresh_req_type  = MMGSDI_REFRESH_ORIG_COMPLETE_REQ;
  req.refresh.pass_fail = pass_fail;

  mmgsdi_status = mmgsdi_session_build_refresh_req(session_id,
                                                   &req,
                                                   response_cb_ptr,
                                                   client_ref);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Command Queuing Failed 0x%x, mmgsdi_session_refresh_complete",
                  mmgsdi_status);
  }
  return mmgsdi_status;
}/*mmgsdi_session_refresh_complete*/ /*lint !e715*/


/*=============================================================================
FUNCTION        MMGSDI_SESSION_OK_TO_REFRESH

DESCRIPTION
  This function will have to be called by clients if they get a Refresh
  event (REFRESH_STAGE_WAIT_FOR_OK_TO_INIT,
  REFRESH_STAGE_WAIT_FOR_OK_TO_FCN). Unless this API is called, the refresh
  procedures will not start. With this API call, the client will be able to
  inform MMGSDI if it is ok to terminate the application that they are
  communicating with OR it is ok to continue with File Change.

PARAMETERS
  session_id:      Session Id of the caller
  ok_to_refresh:   Client informs MMGSDI if it is ok to continue with
                   refresh or not. For clients on default application this
                   would imply bringing the stack down and for others it
                   would apply to app termination
  Response_cb_ptr: Callback to this command
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The client should have also registered for FCNs and should have also
  received a notification for FCN before they call this function.

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_session_ok_to_refresh (
   mmgsdi_session_id_type         session_id,
   boolean                        ok_to_refresh,
   mmgsdi_callback_type           response_cb_ptr,
   mmgsdi_client_data_type        client_ref
)
{
  mmgsdi_refresh_req_type req  = {{0}};

  req.refresh_req_type = MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ;
  req.refresh.ok_req   = ok_to_refresh;

  return mmgsdi_session_build_refresh_req(session_id,
                                          &req,
                                          response_cb_ptr,
                                          client_ref);
}/*mmgsdi_session_ok_to_refresh*/


/*=============================================================================
FUNCTION        MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH

DESCRIPTION
  Approves to proceed with refresh.

  The client must call this function if it gets a refresh event
  (REFRESH_STAGE_WAIT_FOR_OK_TO_PROCEED), or else MMGSDI will not proceed with refresh.

PARAMETERS
  session_id:      Session Id of the caller
  ok_to_proceed:   Client notifies the MMGSDI that it is OK to
                   continue with refresh (TRUE) or not (FALSE).
  Response_cb_ptr: Callback to this command
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The client should have also registered for FCNs and should have also
  received a notification for FCN before they call this function.

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_session_ok_to_proceed_with_refresh (
   mmgsdi_session_id_type                 session_id,
   boolean                                ok_to_proceed,
   mmgsdi_callback_type                   response_cb_ptr,
   mmgsdi_client_data_type                client_ref
)
{
  mmgsdi_refresh_req_type req  = {{0}};

  req.refresh_req_type     = MMGSDI_REFRESH_ORIG_PROCEED_WITH_REFRESH_REQ;
  req.refresh.notify_pdown = ok_to_proceed;

  return mmgsdi_session_build_refresh_req(session_id,
                                          &req,
                                          response_cb_ptr,
                                          client_ref);
}/*mmgsdi_session_ok_to_proceed_with_refresh*/


/*===========================================================================
FUNCTION        MMGSDI_SESSION_SEEK

DESCRIPTION
  This function performs GSM Seek command on card.

PARAMETRES
  session_id:        Session ID of the caller.
  file_access:       File Information.
  seek_direction:    Search Direction.
  seek_pattern:      Search Data.
  response_cb_ptr:   Callback for MMGSDI to call upon completion of the
                     request.
  client_ref:        User Data returned to the user upon completion of
                     the command.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type

    MMGSDI_SUCCESS:           The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS:  The parameters supplied to the API are
                              not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:    The command was not queued to the MMGSDI Task
                              because the max numbers of commands are already
                              queued.
    MMGSDI_NOT_SUPPORTED:     when card is not ICC card.

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_seek(
  mmgsdi_session_id_type          session_id,
  mmgsdi_access_type              file_access,
  mmgsdi_seek_direction_enum_type seek_direction,
  mmgsdi_seek_data_type           seek_pattern,
  mmgsdi_callback_type            response_cb_ptr,
  mmgsdi_client_data_type         client_ref
)
{
  (void)session_id;
  (void)file_access;
  (void)seek_direction;
  (void)seek_pattern;
  (void)response_cb_ptr;
  (void)client_ref;

  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_session_seek function */


/*===========================================================================
FUNCTION        MMGSDI_SESSION_SEARCH

DESCRIPTION
  This function performs Search command on UICC card.
  The input parameters contain following information.

PARAMETERS
  session_id:                   Session ID of the caller
  file_access:                  File Information
  search_type:                  UICC Search Type
                                Valid Range:
                                  MMGSDI_UICC_ENHANCE_SEARCH
                                  MMGSDI_UICC_SIMPLE_SEARCH
  search_record_num:            Record Number, from where search will
                                start in file.
                                Valid Range:
                                  0x00: Current Record
                                  0x01-0xFE: Physical record Number
  search_direction:             Search Direction
  enhanced_search_offset_data:  Offset, from where search starts in a record.
                                Valid Range for offset data
                                  0x00-0xFF for MMGSDI_SEARCH_BY_CHAR type.
                                  0x00-0xFE for MMGSDI_SEARCH_BY_OFFSET type.
  search_pattern:               Data to be searched. Cannot be NULL.
  response_cb_ptr:              Callback for MMGSDI to call upon completion
                                of the request.
  client_ref:                   User Data returned to the user upon completion
                                of the command.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:            The command structure was properly generated
                               and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS:   The parameters supplied to the API are
                               not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:     The command was not queued to the MMGSDI Task
                               because the max numbers of commands are already
                               queued.
    MMGSDI_NOT_SUPPORTED:      when card is not UICC card.

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_search(
  mmgsdi_session_id_type            session_id,
  mmgsdi_access_type                file_access,
  mmgsdi_search_enum_type           search_type,
  mmgsdi_rec_num_type               search_record_num,
  mmgsdi_search_direction_enum_type search_direction,
  mmgsdi_search_offset_data_type    enhanced_search_offset_data,
  mmgsdi_search_data_type           search_pattern,
  mmgsdi_callback_type              response_cb_ptr,
  mmgsdi_client_data_type           client_ref
)
{
  mmgsdi_task_cmd_type             * task_cmd_ptr   = NULL;
  mmgsdi_search_req_type           * msg_ptr        = NULL;
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_ERROR;
  int32                              total_mem_len  = 0;
  int32                              temp_mem       = 0;
  mmgsdi_offset_type                 offset         = 0;

  /*---------------------------------------------
    Input Parameters Checking
   -----------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SEARCH_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  offset = (MMGSDI_SEARCH_BY_OFFSET == enhanced_search_offset_data.offset_type) ?
            enhanced_search_offset_data.offset_data : 0;

  /* search_pattern checking */
  if(search_pattern.data_len <= 0 || search_pattern.data_ptr == NULL ||
     offset < 0 || offset > MMGSDI_MAX_DATA_SIZE ||
     search_pattern.data_len > MMGSDI_MAX_DATA_SIZE - offset)
  {
    UIM_MSG_ERR_0("Invalid search pattern");
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* file_access checking */
  if ((file_access.access_method != MMGSDI_BY_PATH_ACCESS) &&
    (file_access.access_method != MMGSDI_EF_ENUM_ACCESS))
  {
    UIM_MSG_ERR_1("Invalid Access Method Type 0x%x", file_access.access_method);
    return MMGSDI_INCORRECT_PARAMS;
  }
  /* search_type checking */
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(search_type,
                                    MMGSDI_UICC_SIMPLE_SEARCH,
                                    MMGSDI_UICC_ENHANCED_SEARCH);

  /* search_record_num checking */
  MMGSDIUTIL_RETURN_IF_NEGATIVE(search_record_num);
  MMGSDIUTIL_RETURN_IF_EXCEEDS(search_record_num, MMGSDI_MAX_SEARCH_RECORD_NUMBER);
  if (0x00 == search_record_num)
  {
    UIM_MSG_ERR_0("Invalid record number 0x00");
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* search_direction checking */
  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(search_direction,
                                    MMGSDI_SEARCH_FORWARD_FROM_REC_NUM,
                                    MMGSDI_SEARCH_BACKWARD_FROM_REC_NUM);

  /* enhance_search_offset_data checking */
  if (MMGSDI_UICC_ENHANCED_SEARCH == search_type )
  {
    switch(enhanced_search_offset_data.offset_type )
    {
      case MMGSDI_SEARCH_BY_OFFSET:
        MMGSDIUTIL_RETURN_IF_EXCEEDS(enhanced_search_offset_data.offset_data,
                                     MMGSDI_MAX_ENHANCED_SEARCH_RECORD_OFFSET);
        break;

      case MMGSDI_SEARCH_BY_CHAR:
        MMGSDIUTIL_RETURN_IF_EXCEEDS(enhanced_search_offset_data.offset_data,
                                     MMGSDI_MAX_ENHANCED_SEARCH_CHARACTER_VALUE);
        break;

      default:
        UIM_MSG_ERR_1("Invalid Offset Type for enchance search 0x%x",
                      enhanced_search_offset_data.offset_type);
        return MMGSDI_INCORRECT_PARAMS;
    }/*End of switch(enhanced_search_offset_data.offset_type ) */
  }/* End of  if (MMGSDI_UICC_ENHANCE_SEARCH == search_type ) */
  else
  {
    /* offset will be ignore for Simple Search and
       we are setting a default value */
    enhanced_search_offset_data.offset_type = MMGSDI_SEARCH_OPTION_NONE;
    enhanced_search_offset_data.offset_data = 0x00; /*Starting Offset */
  }

  /* response_cb_ptr checking */
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  /*------------------------------------------------------------------------
  - Allocating memory to Task Command Pointer
  -------------------------------------------------------------------------*/
  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem + search_pattern.data_len;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, int32touint32(total_mem_len));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*------------------------------------------------------------------------
    - Align pointers of the various structures members
  -------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.search_req, MMGSDI_SEARCH_REQ);

  /* -----------------------------------------------------------------------
  Populate Task data , Take the pointer of search request
  -------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum = MMGSDI_SEARCH_REQ;
  msg_ptr = &task_cmd_ptr->cmd.cmd.search_req;

    /*---------------------------------------------------------------------------
    Populating mmgsdi_usim_auth_req_type header
      1) Session ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len
      6) request len
  ---------------------------------------------------------------------------*/
  /* Updating Request Header */
  msg_ptr->request_header.session_id           = session_id;
  msg_ptr->request_header.orig_request_type    = MMGSDI_SEARCH_REQ;
  msg_ptr->request_header.request_type         = MMGSDI_SEARCH_REQ;
  msg_ptr->request_header.client_data          = client_ref;
  msg_ptr->request_header.response_cb          = response_cb_ptr;

  /* Updating access  */
  mmgsdi_status = mmgsdi_util_copy_access_type(&msg_ptr->access, &file_access);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the copying of access type fail*/
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    UIM_MSG_ERR_0("Copying of Access Type is Fail in MMGSDI_SEARCH");
    mmgsdi_util_print_file_information(file_access);
    return mmgsdi_status;
  }

  /* Updating search_type */
  msg_ptr->search_type                         = search_type;
  /* Updating rec_num */
  msg_ptr->rec_num                             = search_record_num;
  /* Updating search_direction */
  msg_ptr->search_direction                    = search_direction;
  /* Updating offset */
  msg_ptr->enhanced_search_offset.offset_type  =
                                      enhanced_search_offset_data.offset_type;
  msg_ptr->enhanced_search_offset.offset_data  =
                                      enhanced_search_offset_data.offset_data;
  /* Updating data */
  msg_ptr->data.data_len                       = search_pattern.data_len;
  mmgsdi_memscpy(msg_ptr->data.data_ptr,
                 int32touint32(msg_ptr->data.data_len),
                 search_pattern.data_ptr,
                 int32touint32(msg_ptr->data.data_len));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SEARCH_REQ(search) status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/*  mmgsdi_session_search*/


/*===========================================================================
FUNCTION        MMGSDI_SESSION_READ_CACHE_FILE_SIZE

DESCRIPTION
  Clients invoke this API to get the size of the file to be cached for read.
  (This API will be moved to mmgsdisessionlib_v.h)

  This API uses critical sections to protect accesses to
  MMGSDI global data. Hence, it is advisable to pet the dog
  before invoking the API.

PARAMETERS
  session_id:                  Session ID of the caller
  file_name:                   The enum used to access the SIM/USIM/RUIM/CSIM
                               Elementary Files.
  file_size_ptr:               The pointer to the buffer to store the file
                               size. The memory should be allocated and
                               provided by the caller.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:            The command structure was properly generated
                               and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS:   The parameters supplied to the API are
                               not within appropriate ranges.
    MMGSDI_NOT_FOUND:          The requested file not found
    MMGSDI_NOT_INIT:           Cache not yet initialized

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_read_cache_file_size (
  mmgsdi_session_id_type       session_id,
  mmgsdi_file_enum_type        file_name,
  mmgsdi_len_type            * file_size_ptr
)
{
  mmgsdi_access_type                 access;
  mmgsdi_offset_type                 offset         = 0;
  mmgsdi_session_id_status_enum_type session_status = MMGSDI_SESSION_ID_UNASSIGNED;
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_ERROR;

  UIM_MSG_HIGH_0("mmgsdi_session_read_cache_file_size");

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Memory allocated for file_size_ptr check
    2) Session ID
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_RETURN_IF_NULL(file_size_ptr);

  /* Protect access to client ID reg. table */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    session_status = mmgsdi_util_is_session_id_valid(session_id);

    if(session_status !=  MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE)
    {
      UIM_MSG_ERR_2("Input session ID is not active session_id=0x%x status=0x%x ",
                    session_id, session_status);
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      break;
    }

    memset(&access, 0x00, sizeof(mmgsdi_access_type));

    access.access_method  = MMGSDI_EF_ENUM_ACCESS;
    access.file.file_enum = file_name;

    /* Read cache length */
    mmgsdi_status = mmgsdi_cache_read_len(session_id,
                                               &access,
                                               file_size_ptr,
                                               offset,
                                               NULL);
  }while(0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  return mmgsdi_status;
} /* mmgsdi_session_read_cache_file_size */


/*===========================================================================
FUNCTION        MMGSDI_SESSION_READ_CACHE_EXT

DESCRIPTION
  Clients invoke this API to read data from a cached file.

  This API uses critical sections to protect accesses to
  MMGSDI global data. Hence, it is advisable to pet the dog
  before invoking the API.

PARAMETERS
  session_id:                  Session ID of the caller
  file_name:                   The enum used to access the SIM/USIM/RUIM/CSIM
                               Elementary Files.
  read_data_ptr:               Buffer length and pointer. Used to store the
                               data to be read. The memory should be allocated
                               and provided by the caller.
  offset:                      Offset to start the read from.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  Either a transparent file or 1 record from a linear fixed file can be cached.
  Always read the entire file/record.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:            The command structure was properly generated
                               and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS:   The parameters supplied to the API are
                               not within appropriate ranges.
    MMGSDI_NOT_FOUND:          The requested file not found
    MMGSDI_NOT_INIT:           Cache not yet initialized

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_read_cache_ext (
  mmgsdi_session_id_type       session_id,
  mmgsdi_file_enum_type        file_name,
  mmgsdi_data_type           * read_data_ptr,
  mmgsdi_offset_type           offset
)
{
  mmgsdi_access_type                 access;
  mmgsdi_session_id_status_enum_type session_status = MMGSDI_SESSION_ID_UNASSIGNED;
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_ERROR;
  mmgsdi_client_id_type              client_id         = 0;
  mmgsdi_slot_id_enum_type           slot              = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_int_app_info_type          *prov_app_info_ptr = NULL;
  mmgsdi_session_type_enum_type      session_type      = MMGSDI_MAX_SESSION_TYPE_ENUM;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Memory allocated for data_ptr check
    2) Data len > 0 Check
    3) Session ID
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_RETURN_IF_NULL_2(read_data_ptr,
                              read_data_ptr->data_ptr);

  memset(&access, 0x00, sizeof(mmgsdi_access_type));

  if(read_data_ptr->data_len <= 0 ||
     offset < 0 ||  offset > MMGSDI_MAX_DATA_SIZE || 
     read_data_ptr->data_len > MMGSDI_MAX_DATA_SIZE - offset)
  {
    UIM_MSG_ERR_1("Read Data Len: 0x%x <= 0", read_data_ptr->data_len);
    return MMGSDI_INCORRECT_PARAMS;
  }

  access.access_method  = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = file_name;

  /* Protect access to client ID reg. table */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    session_status = mmgsdi_util_is_session_id_valid(session_id);

    if(session_status !=  MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE)
    {
      UIM_MSG_ERR_2("Input session ID is not active session_id=0x%x status=0x%x ",
                    session_id, session_status);
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      break;
    }

    if (mmgsdi_util_get_session_type(session_id,
                                     &session_type,
                                     NULL) != MMGSDI_SUCCESS)
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* MMGSDI don't cache files for non-prov session. */
    if (mmgsdi_util_is_non_prov_session(session_type))
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    if (mmgsdi_util_is_prov_session(session_type))
    {
      /* Get info about the Session */
      mmgsdi_status = mmgsdi_util_get_prov_session_info(session_id,
                                                        NULL,
                                                        &slot,
                                                        &prov_app_info_ptr);

      if((mmgsdi_status != MMGSDI_SUCCESS) || (slot == MMGSDI_MAX_SLOT_ID_ENUM) ||
         (prov_app_info_ptr == NULL))
      {
        mmgsdi_status = MMGSDI_ERROR;
        break;
      }

      mmgsdi_status = mmgsdi_util_get_client_id(session_id,&client_id);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        break;
      }

      /* Check to see if the security attributes allow this file to be read */
      mmgsdi_status = mmgsdi_util_check_file_attr(client_id,
                                                  session_id,
                                                  slot,
                                                  prov_app_info_ptr,
                                                  &access,
                                                  MMGSDI_READ_REQ,
                                                  FALSE);

      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("File Read not allowed, status = 0x%x", mmgsdi_status);
        break;
      }
    }

    /* Read data from cache */
    mmgsdi_status = mmgsdi_cache_read(session_id,
                                      &access,
                                      read_data_ptr,
                                      offset,
                                      NULL);
  }while(0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  return mmgsdi_status;
} /* mmgsdi_session_read_cache_ext */


/*===========================================================================
FUNCTION        MMGSDI_SESSION_READ_CACHE

DESCRIPTION
  Clients invoke this API to read data from a cached file.
  (This API will be moved to mmgsdisessionlib_v.h)

  This API uses critical sections to protect accesses to
  MMGSDI global data. Hence, it is advisable to pet the dog
  before invoking the API.

PARAMETERS
  session_id:                  Session ID of the caller
  file_name:                   The enum used to access the SIM/USIM/RUIM/CSIM
                               Elementary Files.
  read_data:                   Buffer length and pointer. Used to store the
                               data to be read. The memory should be allocated
                               and provided by the caller.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  Either a transparent file or 1 record from a linear fixed file can be cached.
  Always read the entire file/record.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:            The command structure was properly generated
                               and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS:   The parameters supplied to the API are
                               not within appropriate ranges.
    MMGSDI_NOT_FOUND:          The requested file not found
    MMGSDI_NOT_INIT:           Cache not yet initialized

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_read_cache (
  mmgsdi_session_id_type       session_id,
  mmgsdi_file_enum_type        file_name,
  mmgsdi_data_type             read_data
)
{
  return mmgsdi_session_read_cache_ext(session_id,
                                       file_name,
                                       &read_data,
                                       (mmgsdi_offset_type) 0);
} /* mmgsdi_session_read_cache */


/*===========================================================================
FUNCTION        MMGSDI_SESSION_WRITE_CACHE

DESCRIPTION
  Clients invoke this API to write data to a cached file.
  (This API will be moved to mmgsdisessionlib_v.h)

  This API uses critical sections to protect accesses to
  MMGSDI global data. Hence, it is advisable to pet the dog
  before invoking the API.

PARAMETERS
  session_id:                 Session ID of the caller
  file_name:                  The enum used to access the SIM/USIM/RUIM/CSIM
                              Elementary Files.
  write_data:                 Buffer length and pointer. Used to store the
                              data to be written.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  Can be used to write only into a transparent file in the Cache.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:            The command structure was properly generated
                               and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS:   The parameters supplied to the API are
                               not within appropriate ranges.
    MMGSDI_NOT_FOUND:          The requested file not found
    MMGSDI_NOT_INIT:           Cache not yet initialized

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_write_cache (
  mmgsdi_session_id_type       session_id,
  mmgsdi_file_enum_type        file_name,
  mmgsdi_data_type             write_data
)
{
  mmgsdi_access_type                 access;
  mmgsdi_offset_type                 offset            = 0;
  mmgsdi_session_id_status_enum_type session_status    = MMGSDI_SESSION_ID_UNASSIGNED;
  mmgsdi_return_enum_type            mmgsdi_status     = MMGSDI_ERROR;
  mmgsdi_client_id_type              client_id         = 0;
  mmgsdi_slot_id_enum_type           slot              = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_int_app_info_type          *prov_app_info_ptr = NULL;
  mmgsdi_len_type                    rec_len           = 0;
  mmgsdi_data_type                   write_data_buf;

  UIM_MSG_HIGH_0("mmgsdi_session_write_cache");
  memset(&write_data_buf, 0x00, sizeof(mmgsdi_data_type));

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Memory allocated for data_ptr check
    2) Data len > 0 Check
    3) Session ID
  ---------------------------------------------------------------------------*/

  MMGSDIUTIL_RETURN_IF_NULL(write_data.data_ptr);

  if(write_data.data_len <= 0 ||
     write_data.data_len > MMGSDI_MAX_DATA_SIZE)
  {
    UIM_MSG_ERR_1("Write Data Len: 0x%x <= 0", write_data.data_len);
    return MMGSDI_INCORRECT_PARAMS;
  }

  memset(&access, 0x00, sizeof(mmgsdi_access_type));

  access.access_method = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = file_name;

  /* Check if this file is currently being refreshed */
  if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_BLOCK_WRITES_TO_REFRESH_FILES, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC)
       == MMGSDI_FEATURE_ENABLED &&
     mmgsdi_is_file_in_refresh_data_info_list(session_id,
                                              access))
  {
    UIM_MSG_ERR_0("Request temporarily not allowed as requested file is being refreshed");
    return MMGSDI_ERROR;
  }

  /* Protect access to client ID reg. table
     Also, ensure that the updation of the cache and the async. write request
     to MMGSDI are sequential */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    session_status = mmgsdi_util_is_session_id_valid(session_id);

    if(session_status !=  MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE)
    {
      UIM_MSG_ERR_2("Input session ID is not active session_id=0x%x status=0x%x ",
                    session_id, session_status);
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      break;
    }

    /* Get info about the Session */
    mmgsdi_status = mmgsdi_util_get_prov_session_info(session_id,
                                                      NULL,
                                                      &slot,
                                                      &prov_app_info_ptr);

    if((mmgsdi_status != MMGSDI_SUCCESS) || (slot == MMGSDI_MAX_SLOT_ID_ENUM) ||
       (prov_app_info_ptr == NULL))
    {
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      break;
    }

    /* Check if requested file is being purged */
    if (((MMGSDI_GSM_LOCI     == file_name) ||
         (MMGSDI_USIM_LOCI    == file_name) ||
         (MMGSDI_USIM_PSLOCI  == file_name) ||
         (MMGSDI_USIM_EPSLOCI == file_name))    &&
        (mmgsdi_util_is_purge_temp_identities_in_progress(slot)))
    {
      UIM_MSG_ERR_1("Request not allowed, file_name=0x%x is being purged",
                    file_name);
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* Get the client ID */
    mmgsdi_status = mmgsdi_util_get_client_id(session_id,&client_id);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }

    /* Check to see if the security attributes allow this file to be updated */
    mmgsdi_status = mmgsdi_util_check_file_attr(client_id,
                                                session_id,
                                                slot,
                                                prov_app_info_ptr,
                                                &access,
                                                MMGSDI_WRITE_REQ,
                                                FALSE);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("File Update not allowed, status = 0x%x", mmgsdi_status);
      break;
    }

    /* For record writes ensure the client input length equals the record size */
    if (file_name == MMGSDI_USIM_EPSNSC ||
        file_name == MMGSDI_USIM_ACM ||
        file_name == MMGSDI_GSM_ACM)
    {
      mmgsdi_status = mmgsdi_cache_read_len(session_id,
                                            &access,
                                            &rec_len,
                                            0,
                                            NULL);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        break;
      }

      if (write_data.data_len != rec_len)
      {
        UIM_MSG_ERR_2("Write Data Len: 0x%x != Record Length: 0x%x",
                      write_data.data_len, rec_len);
        mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
        break;
      }
    }

   /* copy write_data_buf from write_data so it can be modified by
      TMSI PLMN check during util write cache operation*/
    write_data_buf.data_len = write_data.data_len;

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(write_data_buf.data_ptr,
                                       write_data_buf.data_len);

    if (write_data_buf.data_ptr != NULL)
    {
      mmgsdi_memscpy(write_data_buf.data_ptr,
                     int32touint32(write_data_buf.data_len),
                     write_data.data_ptr,
                     int32touint32(write_data_buf.data_len));

      mmgsdi_status = mmgsdi_cache_write(session_id,
                                         &access,
                                         write_data_buf,
                                         offset,
                                         MMGSDI_DATA_FROM_CLIENT);

      MMGSDIUTIL_TMC_MEM_FREE(write_data_buf.data_ptr);
    }
    else
    {
      mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    /* If cache write is scuccessful, write data to card */
    if(mmgsdi_status == MMGSDI_SUCCESS)
    {
      /*-------------------------------------------------------------------------
        Note
        1) Make an exception for EPSNSC and ACM... these are the only two
           linear fixed EFs being cached. All other EFs are transparent.
        2) For linear fixed EFs, the function can access only the first
           record
        3) Dummy CB is provided to the API since the write cache API
           is synchronous
      -------------------------------------------------------------------------*/
      if (file_name == MMGSDI_USIM_EPSNSC ||
          file_name == MMGSDI_USIM_ACM ||
          file_name == MMGSDI_GSM_ACM)
      {
        mmgsdi_status = mmgsdi_session_write_record(session_id,
                                                    access,
                                                    MMGSDI_LINEAR_FIXED_FILE,
                                                    1,
                                                    write_data,
                                                    mmgsdi_util_discard_cb,
                                                    0);
      }
      else
      {
        mmgsdi_status = mmgsdi_session_write_transparent(session_id,
                                                         access,
                                                         offset,
                                                         write_data,
                                                         mmgsdi_util_discard_cb,
                                                         0);
      }
    }
  }while(0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  return mmgsdi_status;
} /* mmgsdi_session_write_cache */

/*================================================================
 FUNCTION:      MMGSDI_SESSION_MANAGE_ILLEGAL_SUBSCRIPTION

DESCRIPTION:
  Indicates whether the subscription on the card is illegal or legal.
  This is a synchronous function from a client perspective.

PARAMETERS:
  session_id:   Session ID of illegal/legal subs (should be PROV SESSION)
  legal_status: Whether the request is to mark the app ILLEGAL or LEGAL

DEPENDENCIES:
  A valid PROV Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The session ID should only be that of a provisioning application

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
  MMGSDI_SUCCESS:          The command structure was properly
                           Generated and queued onto the MMGSDI
                           Command Queue.
  MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                           are not within appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the
                           MMGSDI Task because the max number of
                           commands are already queued.

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_manage_illegal_subscription(
  mmgsdi_session_id_type                session_id,
  mmgsdi_session_app_legal_status_type  legal_status
)
{
  mmgsdi_task_cmd_type                                 *task_cmd_ptr   = NULL;
  mmgsdi_session_manage_illegal_subscription_req_type  *msg_ptr        = NULL;
  mmgsdi_return_enum_type                               mmgsdi_status  = MMGSDI_ERROR;
  uint32                                                task_cmd_len   = 0;

  UIM_MSG_HIGH_0("mmgsdi_session_manage_illegal_subscription");

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Legal status
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_MANAGE_ILLEGAL_SUBSCRIPTION_REQ);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (legal_status != MMGSDI_SESSION_APP_IS_ILLEGAL &&
      legal_status != MMGSDI_SESSION_APP_IS_LEGAL)
  {
    UIM_MSG_HIGH_1("Invalid parameters: legal status out of range: 0x%x",
               legal_status);
    return MMGSDI_INCORRECT_PARAMS;
  }

  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  msg_ptr = &task_cmd_ptr->cmd.cmd.session_manage_illegal_subscription_req;
  /*---------------------------------------------------------------------------
    Populating session_manage_illegal_subscription_req header
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SESSION_MANAGE_ILLEGAL_SUBSCRIPTION_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_MANAGE_ILLEGAL_SUBSCRIPTION_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_MANAGE_ILLEGAL_SUBSCRIPTION_REQ;
  msg_ptr->request_header.session_id        = session_id;

  /* Whether this is a request to mark the app as ILLEGAL or LEGAL */
  msg_ptr->legal_status = legal_status;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }
  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_MANAGE_ILLEGAL_SUBSCRIPTION_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_manage_illegal_subscription */


/*================================================================
 FUNCTION:      MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_SYNC

DESCRIPTION:
  This synchronous function is indicate that a subscription on the
  card is illegal and should be blocked.

PARAMETERS:
  session_id:        Session ID of illegal subs (should be PROV SESSION)

DEPENDENCIES:
  A valid PROV Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The session ID should only be that of a provisioning application

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
  MMGSDI_SUCCESS:          The command structure was properly
                           Generated and queued onto the MMGSDI
                           Command Queue.
  MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                           are not within appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the
                           MMGSDI Task because the max number of
                           commands are already queued.

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_illegal_subscription_sync(
  mmgsdi_session_id_type              session_id
)
{
  return mmgsdi_session_manage_illegal_subscription(session_id,
                                                    MMGSDI_SESSION_APP_IS_ILLEGAL);
}/* mmgsdi_session_illegal_subscription_sync */


/*================================================================
FUNCTION:      MMGSDI_SESSION_GET_APP_CAPABILITIES

DESCRIPTION:
  This API is used to extract all the provisioning application
  capabilities like fdn, bdn, acl, imsi status and phone book related.

PARAMETERS:
  session_id     : Session Id of the caller

  response_cb_ptr: Call back function when the command execution is done.
  client_ref     : User Data returned upon completion of this cmd.

DEPENDENCIES:
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The session ID should only be that of a provisioning application

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS:          The command structure was properly
                             Generated and queued onto the MMGSDI
                             Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                             are not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the
                             MMGSDI Task because the max number of
                             commands are already queued.

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_get_app_capabilities(
  mmgsdi_session_id_type                session_id,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
)
{
  mmgsdi_task_cmd_type                         *task_cmd_ptr     = NULL;
  mmgsdi_session_get_app_capabilities_req_type *msg_ptr          = NULL;
  mmgsdi_return_enum_type                       mmgsdi_status    = MMGSDI_ERROR;
  uint32                                        task_cmd_len     = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Response CB function Check
    3) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_GET_APP_CAPABILITIES_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.session_get_app_cap_req,
                         MMGSDI_SESSION_GET_APP_CAPABILITIES_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.session_get_app_cap_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_get_app_capabilities_req_type header
      1) Session ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SESSION_GET_APP_CAPABILITIES_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_GET_APP_CAPABILITIES_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_GET_APP_CAPABILITIES_REQ;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* check the app capabilities from cached data first. If app capabilities
     is not cached, we will queue the request to MMGSDI */
  mmgsdi_status = mmgsdi_caller_context_check_app_capabilities_and_notify_client(task_cmd_ptr);

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* Item was either read successfully from the cache or the cache revealed
       that the item is not found on the card. Free the command and return the status */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    return mmgsdi_status;
  }

  /* Continue with normal processing, that is, continue with queuing the
     request to MMGSDI */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
     onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_GET_APP_CAPABILITIES_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_get_app_capabilities */


/*===========================================================================
FUNCTION        MMGSDI_SESSION_GET_CPHS_INFO

DESCRIPTION
  This function is used to extract the CPHS related information which is
  already populated in MMGSDI. This is only valid and applicable when an USIM
  or GSM Application is available . This function will perform a check for
  the USIM or GSM Application before sending the command.

PARAMETERS
  mmgsdi_session_id_type  session_id:      Session ID of the caller
  mmgsdi_callback_type    response_cb_ptr: Call back function when the
                                           command execution is done.
  mmgsdi_client_data_type client_ref:      User Data returned upon
                                           completion of this cmd.

DEPENDENCIES
  The client must also have a valid session open if the USIM Application
  is not the default application
  Achieved by calling mmgsdi_session_open().

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_get_cphs_info (
  mmgsdi_session_id_type              session_id,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
)
{
  return mmgsdi_session_queue_get_cphs_info_req(session_id, response_cb_ptr, client_ref);
}/*mmgsdi_session_get_cphs_info*/


/*================================================================
FUNCTION:      MMGSDI_SESSION_GET_APP_INFO_SYNC

DESCRIPTION:
  This API is used by modem layers to extract the app info associated to a
  particular session id, if that session type is active

  This API uses critical sections to protect accesses to
  MMGSDI global data. Hence, it is advisable to pet the dog
  before invoking the API.

PARAMETERS:
  session_id     : Session Id of the caller
  app_type_ptr   : App type stored here for calling function to use
                          MMGSDI_APP_NONE: No app selected
                          MMGSDI_APP_SIM:  2G SIM mode selected
                          MMGSDI_APP_RUIM: 2G RUIM mode selected
                          MMGSDI_APP_USIM: 3G USIM mode app selected
                          MMGSDI_APP_CSIM: 3G CSIM mode app selected
                          MMGSDI_APP_UNKNOWN: Unknown app selected

DEPENDENCIES:
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The session ID should only be that of a GW provisioning application

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS: Request handled successfully, app_type_ptr has the app info
    MMGSDI_INCORRECT_PARAMS: Session id is invalid
    MMGSDI_ERROR: Request handling failed

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_get_app_info_sync (
  mmgsdi_session_id_type               session_id,
  mmgsdi_app_enum_type                *app_type_ptr
)
{
  mmgsdi_return_enum_type             mmgsdi_status    = MMGSDI_ERROR;
  mmgsdi_session_id_status_enum_type  session_status   = MMGSDI_SESSION_ID_UNASSIGNED;
  mmgsdi_int_app_info_type           *app_info_ptr     = NULL;

  UIM_MSG_HIGH_0("mmgsdi_session_get_app_info_sync");

  /*---------------------------------------------------------------------------
     Input Parameters Check
     1) app_type_ptr check
     2) Session ID
   ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(app_type_ptr);

  /* Protect access to Client ID Reg.Table */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    session_status = mmgsdi_util_is_session_id_valid(session_id);

    if((session_status !=  MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE) &&
       (session_status !=  MMGSDI_SESSION_ID_ASSIGNED_NOT_ACTIVE))
    {
      UIM_MSG_ERR_2("Input session ID is not in use session_id=0x%x status=0x%x ",
                    session_id, session_status);
      *app_type_ptr = MMGSDI_APP_NONE;
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      break;
    }
    mmgsdi_status = mmgsdi_util_get_session_app_info(
                            session_id,NULL,NULL,&app_info_ptr,
                            NULL,NULL,NULL);
    if(mmgsdi_status != MMGSDI_SUCCESS || app_info_ptr == NULL)
    {
      UIM_MSG_ERR_1("Invalid session ID = 0x%x", session_id);
      *app_type_ptr = MMGSDI_APP_NONE;
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    *app_type_ptr = app_info_ptr->app_data.app_type;
  }while(0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  return mmgsdi_status;
}/* mmgsdi_session_get_app_info_sync */


/*================================================================
FUNCTION:      MMGSDI_SESSION_TOGGLE_UIM_POWER_CONTROL_SYNC

DESCRIPTION:
  This API is used by modem layers to inform UIM when the modem is in and out
  of traffic channel. when Modem is in traffic channel, power control is turned
  ON else turned OFF.

  This API uses critical sections to protect accesses to
  MMGSDI global data. Hence, it is advisable to pet the dog
  before invoking the API.

PARAMETERS:
  session_id : Session on which traffic channel is enabled
  in_TC      : Boolean - in traffic channel or out of Traffic channel
  mode       : Mode on which traffic channel established GW or 1X
  uim_call_mask : Mask indicating if PS or non PS call is ongoing

DEPENDENCIES:
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The session ID should only be that of a provisioning application

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS:          The command structure was properly
                             Generated and queued onto the MMGSDI
                             Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                             are not within appropriate ranges.

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_toggle_uim_power_control_sync (
  mmgsdi_session_id_type      session_id,
  boolean                     in_TC,
  mmgsdi_in_tc_mode_enum_type mode,
  uint16                      uim_call_mask
)
{
  mmgsdi_session_id_status_enum_type session_status = MMGSDI_SESSION_ID_UNASSIGNED;
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_ERROR;
  uim_voter_type                     mask           = UIM_NONE;
  mmgsdi_slot_id_enum_type           slot           = MMGSDI_MAX_SLOT_ID_ENUM;
  uim_slot_type                      uim_slot       = UIM_SLOT_NONE;

  UIM_MSG_HIGH_0("mmgsdi_session_toggle_uim_power_control_sync");

  /* Protect access to Client ID Reg.Table/App. table */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    session_status = mmgsdi_util_is_session_id_valid(session_id);

    if(session_status !=  MMGSDI_SESSION_ID_ASSIGNED_AND_ACTIVE)
    {
      UIM_MSG_ERR_2("Input session ID is not active session_id: 0x%x status: 0x%x",
                    session_id, session_status);
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      break;
    }

    /* Verify that the session is a valid provisioning session */
    mmgsdi_status = mmgsdi_util_get_prov_session_info(
                                               session_id,NULL,&slot,NULL);

    if((mmgsdi_status != MMGSDI_SUCCESS) || (slot == MMGSDI_MAX_SLOT_ID_ENUM))
    {
      UIM_MSG_ERR_1("Toggle uim power control failed, session_id: 0x%x",
                    session_id);
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = MMGSDI_ERROR;
      }
      break;
    }
  }while(0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  switch (mode)
  {
    case MMGSDI_IN_TC_GW:
      mask = UIM_TC;
      break;

    case MMGSDI_IN_TC_1X:
      mask = UIM_CDMA_TC;
      break;

    case MMGSDI_NOT_IN_TC:
    default:
      UIM_MSG_ERR_2("Toggle uim power control failed for slot: 0x%x due to bad Mode: 0%x",
                    slot, mode);
      return MMGSDI_ERROR;
  }

  UIM_MSG_HIGH_2("Toggle UIM Power Control on slot: 0x%x, traffic channel: 0x%x",
                 slot, in_TC);

  mmgsdi_status = mmgsdi_util_convert_uim_slot(slot, &uim_slot);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Invalid slot id 0x%x", slot);
    return MMGSDI_ERROR;
  }

  (void)uim_power_control_ds(uim_slot, mask, in_TC, uim_call_mask);

  return MMGSDI_SUCCESS;
}/* mmgsdi_session_toggle_uim_power_control_sync */

/*================================================================
FUNCTION:      MMGSDI_SESSION_GET_APP_CAPABILITIES_SYNC

DESCRIPTION:
  This API is used to extract all the provisioning application
  capabilities like fdn, bdn, acl, imsi status and phone book related.

  This API uses critical sections to protect accesses to
  MMGSDI global data. Hence, it is advisable to pet the dog
  before invoking the API.

PARAMETERS:
  session_id           : Session Id of the caller
  app_capabilities_ptr : app capabilities

DEPENDENCIES:
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().
  The session ID should only be that of a provisioning application

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
  MMGSDI_SUCCESS
  MMGSDI_ERROR

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_get_app_capabilities_sync(
  mmgsdi_session_id_type                    session_id,
  mmgsdi_session_app_capabilities_cnf_type *app_capabilities_ptr
)
{
  mmgsdi_int_app_info_type *app_info_ptr         = NULL;
  mmgsdi_return_enum_type   mmgsdi_status        = MMGSDI_SUCCESS;

  UIM_MSG_HIGH_0("mmgsdi_session_get_app_capabilities_sync");

  /* Protect access to Client ID Reg.Table/App. table */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                      NULL, NULL, &app_info_ptr, NULL, NULL, NULL);

    if(mmgsdi_status != MMGSDI_SUCCESS || app_info_ptr == NULL)
    {
      UIM_MSG_ERR_0("Invalid session info");
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }
    if(app_info_ptr->app_capabilities.all_srv_init == FALSE)
    {
      UIM_MSG_ERR_0("All services are not in initialized state");
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }
    if(app_info_ptr->app_capabilities.fdn_state == MMGSDI_SRV_ENABLE)
    {
      app_capabilities_ptr->fdn_enabled = TRUE;
    }
    else
    {
      app_capabilities_ptr->fdn_enabled = FALSE;
    }

    if(app_info_ptr->app_capabilities.bdn_state == MMGSDI_SRV_ENABLE)
    {
      app_capabilities_ptr->bdn_enabled = TRUE;
    }
    else
    {
      app_capabilities_ptr->bdn_enabled = FALSE;
    }

    if(app_info_ptr->app_capabilities.acl_state == MMGSDI_SRV_ENABLE)
    {
      app_capabilities_ptr->acl_enabled = TRUE;
    }
    else
    {
      app_capabilities_ptr->acl_enabled = FALSE;
    }

    app_capabilities_ptr->imsi_invalidated =
                          app_info_ptr->app_capabilities.imsi_invalidated;
  }while(0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  return mmgsdi_status;
}/* mmgsdi_session_get_app_capabilities_sync */


/*=============================================================================
FUNCTION        MMGSDI_SESSION_SUBSCRIPTION_OK

DESCRIPTION
  This function can be called by a client to inform MMGSDI that the subscription
  is OK and it can proceed with publishing the subscription to all clients thus
  allowing the modem to move to full service

PARAMETERS
  session_id:          Session Id of the caller
  ok_for_subscription: Client informs MMGSDI if it is ok to publish the
                       subcriptioin for the provisioning application
  Response_cb_ptr:     Callback to this command
  client_ref:          User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  Only valid for provisioing sessions

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_session_subscription_ok (
   mmgsdi_session_id_type         session_id,
   boolean                        ok_for_subscription,
   mmgsdi_callback_type           response_cb_ptr,
   mmgsdi_client_data_type        client_ref
)
{
  return mmgsdi_external_simlock_sub_ok(session_id,
                                        ok_for_subscription,
                                        response_cb_ptr,
                                        client_ref);
}/* mmgsdi_session_subscription_ok */


/* ==========================================================================
FUNCTION        MMGSDI_SESSION_INCREASE

DESCRIPTION
  This function will provide increase access to any record in a Cyclic File.
  It allows file access by file enum or path. This library function is intended
  to encapsulate the details required to increment a file in the
  SIM/USIM/RUIM/CSIM. The client using this library function will only be able
  to access files that are Cyclic.

  Behavior for the function for the following file types:

    TRANSPARENT FILE:   Will return an Error.
    CYCLIC FILE:        Will increment the last selected record of the file
                        provided in the library call.
    LINEAR FIXED FILE:  Will return an Error.

  Behavior for the function for the following access type:

    MMGSDI_EF_ENUM_ACCESS: Will increment the record of the file provided in the
                           library call.
    MMGSDI_BY_PATH_ACCESS: Will increment the record of the file provided in the
                           library call. Note, there is no check whether the
                           path passed in is Cyclic file or not. The
                           caller is expected to have the knowledge of
                           whether the file to access is Cyclic or not.
    others:                Returns an Error.

  Behavior for the function for the following record type:

    MMGSDI_LINEAR_FIXED_FILE:       Return an Error
    MMGSDI_CYCLIC_FILE:             Will use PREVIOUS access to card
    MMGSDI_TRANSPARENT_FILE:        Return an Error
    MMGSDI_MASTER_FILE:             Return an Error
    MMGSDI_DEDICATED_FILE:          Return an Error
    MMGSDI_MAX_FILE_STRUCTURE_ENUM: Return an Error

PARAMETERS
  session_id:           Session ID of the caller.
  file_access:          File to increment.
  increase_data:        Value of increment.
  response_cb_ptr:      Response callback.
  client_ref:           User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  TRANSPARENT FILE:     Will return an Error when used to access a
                        transparent file.
  LINEAR FIXED FILE:    Will return an Error when used to access a
                        linear fixed file.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

========================================================================== */
mmgsdi_return_enum_type  mmgsdi_session_increase (
  mmgsdi_session_id_type             session_id,
  mmgsdi_access_type                 file_access,
  mmgsdi_increase_data_type          increase_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type             * task_cmd_ptr   = NULL;
  mmgsdi_increase_req_type         * msg_ptr        = NULL;
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_SUCCESS;
  int32                              total_mem_len  = 0;
  int32                              temp_mem       = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Response CB function Check
    3) Record number > 0 Check if Linear Fixed
    4) Data len > 0 Check
    5) Check for EF or PATH access
    6) Check for path length and MF/DF/EF identifiers
    7) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_INCREASE_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);
  if ((file_access.access_method != MMGSDI_BY_PATH_ACCESS) &&
      (file_access.access_method != MMGSDI_EF_ENUM_ACCESS))
  {
    UIM_MSG_ERR_1("Invalid Access Method Type 0x%x", file_access.access_method);
    return MMGSDI_INCORRECT_PARAMS;
  }

  if((file_access.access_method == MMGSDI_BY_PATH_ACCESS) &&
     !mmgsdi_util_is_path_valid(&file_access.file.path_type))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* Check if this file is currently being refreshed */
  if(mmgsdi_nv_get_feature_status(MMGSDI_FEATURE_BLOCK_WRITES_TO_REFRESH_FILES, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC)
       == MMGSDI_FEATURE_ENABLED &&
     mmgsdi_is_file_in_refresh_data_info_list(session_id,
                                              file_access))
  {
    UIM_MSG_ERR_0("Request temporarily not allowed as requested file is being refreshed");
    return MMGSDI_ERROR;
  }

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem + increase_data.data_len;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, int32touint32(total_mem_len));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.increase_req,
                         MMGSDI_INCREASE_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.increase_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_increase_req_type header
      1) Session ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_INCREASE_REQ;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.request_type      = MMGSDI_INCREASE_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_INCREASE_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_increase_req_type content
    - access
      1) Access method in this function is limited to ENUM or PATH access only
      2) File name in the form of file enum type
    - file type
      CYCLIC is the only valid file type for INCREASE
    - increment value
      Mem malloc and validate, if fail, free memory space and return
    [offset is not being populated because this is a write record function]
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_copy_access_type(&msg_ptr->access, &file_access);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the copying of access type fail*/
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    return mmgsdi_status;
  }
  msg_ptr->data.data_len   = increase_data.data_len;
  msg_ptr->int_client_data = 0;

  mmgsdi_memscpy(msg_ptr->data.data_ptr,
                 int32touint32(msg_ptr->data.data_len),
                 increase_data.data_ptr,
                 int32touint32(msg_ptr->data.data_len));

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_INCREASE_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_session_increase */


/*================================================================
FUNCTION:      MMGSDI_SESSION_OPEN_WITH_SELECT_RSP

DESCRIPTION:
  Opens a non-provisioning session to an application on the card and returns
  the type of select response requested.

  Application selection using full length AID, partial AID and empty AID (i.e.
  AID length of zero) are supported.
  When partial/empty AID is provided, first/only occurence of the AID on the
  card is selected.

PARAMETERS:
  client_id:        Client ID of the caller
  slot:             Slot in which card is inserted
  app_info:         Application ID, channel exclusivity and
                    select response type
  evt_cb_ptr:       Application event callback. A client must provide
                    an event callback if it is to receive
                    session-related event notifications. If the
                    pointer is NULL, the client receives no event
                    notifications.
  response_cb_ptr:  Pointer to the response callback.
  client_ref:       User data returned upon completion of this
                    command.

DEPENDENCIES:
  The client must have a valid client ID.

LIMITATIONS:
  Function cannot be used with ICC cards.

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS:          The command structure was properly
                             Generated and queued onto the MMGSDI
                             Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                             are not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the
                             MMGSDI Task because the max number of
                             commands are already queued.

SIDE EFFECTS:
  The event callback for the session of interest is called every time the MMGSDI
  notifies the client on this session about all session-related events.
================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_open_with_select_rsp (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           slot,
  mmgsdi_app_select_with_rsp_type    app_info,
  mmgsdi_evt_callback_type           evt_cb_ptr,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type           *task_cmd_ptr   = NULL;
  mmgsdi_session_open_req_type   *msg_ptr        = NULL;
  mmgsdi_return_enum_type         mmgsdi_status  = MMGSDI_ERROR;
  int32                           total_mem_len  = 0;
  int32                           temp_mem       = 0;
  int32                           var_len        = 0;
  mmgsdi_session_type_enum_type   session_type   = MMGSDI_NON_PROV_SESSION_SLOT_1;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client ID
    2) Slot
    3) Response CB function Check
    4) App info
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(!mmgsdi_util_is_slot_valid(slot))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_util_get_non_prov_session_type_from_slot(
                    slot,
                    &session_type);
  if(MMGSDI_SUCCESS != mmgsdi_status)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  if((app_info.select_rsp_type < MMGSDI_SELECT_RSP_NO_DATA) ||
     (app_info.select_rsp_type > MMGSDI_SELECT_RSP_FCI_WITH_INTERFACES))
  {
    UIM_MSG_ERR_1("Invalid Select Rsp type: 0x%x", app_info.select_rsp_type);
    return MMGSDI_INCORRECT_PARAMS;
  }

  if(app_info.app_id_data.data_len == 0)
  {
    UIM_MSG_HIGH_0("Empty AID Select request");
  }
  else if((app_info.app_id_data.data_len < 0) ||
          (app_info.app_id_data.data_len > MMGSDI_MAX_APP_ID_LEN) ||
          (app_info.app_id_data.data_ptr == NULL))
  {
    UIM_MSG_ERR_2("Incorrect input, data_len=0x%x, data_ptr=0x%x",
                  app_info.app_id_data.data_len, app_info.app_id_data.data_ptr);
    return MMGSDI_INCORRECT_PARAMS;
  }

  var_len = app_info.app_id_data.data_len;

  if(evt_cb_ptr == NULL)
  {
    UIM_MSG_HIGH_0("mmgsdi_session_open_with_select_rsp: NULL evt_cb_ptr");
  }

  /* Memory allocation for msg_ptr and check for allocation */
  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem + var_len;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, int32touint32(total_mem_len));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  if (var_len > 0)
  {
    mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.session_open_req,
                           MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ);
  }

  msg_ptr = &task_cmd_ptr->cmd.cmd.session_open_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_open_req_type header
      - Command enumeration
      - Client ID
      - Request type
      - Original request type
      - Client Data
      - Response CallBack
 ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_open_req_type content
    - evt_cb_ptr
    - set_notify
    - select rsp requested
    - select rsp type
    - non-prov app info
    - session_type
  ---------------------------------------------------------------------------*/
  msg_ptr->evt_cb_ptr                             = evt_cb_ptr;
  msg_ptr->select_rsp_requested                   = TRUE;
  msg_ptr->select_rsp_type                        = app_info.select_rsp_type;
  msg_ptr->non_prov_app_data.exclusive_channel    = app_info.exclusive_channel;
  msg_ptr->non_prov_app_data.app_id_data.data_len = app_info.app_id_data.data_len;

  /* Copy AID only when it has non-zero length */
  if(app_info.app_id_data.data_len > 0)
  {
    mmgsdi_memscpy(msg_ptr->non_prov_app_data.app_id_data.data_ptr,
                   int32touint32(app_info.app_id_data.data_len),
                   app_info.app_id_data.data_ptr,
                   int32touint32(app_info.app_id_data.data_len));
  }

  msg_ptr->request_header.slot_id                = slot;
  msg_ptr->session_type                          = session_type;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_session_open_with_select_rsp */


/* ==========================================================================
FUNCTION:       MMGSDI_SESSION_GET_OPERATOR_NAME_EXT_SYNC

  This API is used to get operator name according to PLMN Id.
  Client can set param ignore_display_condition
  to bypass display condition provided in EF-SPN for  RPLMN name and SPN.

  If any PLMN id of PLMN Id list matches with registered PLMN Id, MMGSDI sends 
  registered PLMN name in confirmation. Caller should allocate the memory for obtaining
  operator name info based on the number of plmn ids passed.
  After returning from the function, it is the responsibility of the caller to free the memory.

  @param[in]    session_id:                Session ID
  @param[in]    plmn_id_list:              List of PLMN ids
  @param[in]    ignore_display_condition:  Ignore display condition enum type
  @param[out]   operator_name_info_ptr :   Contains operator name information

  @return
  MMGSDI_SUCCESS:           Request is processed successfully
  MMGSDI_INCORRECT_PARAMS:  The parameters supplied to the API are not
                             within appropriate ranges.

  @dependencies
  The client must have a valid client ID, and the Session ID specified must be valid.

  @limitations
  None

  @side effects
  None
==============================================================================*/
mmgsdi_return_enum_type   mmgsdi_session_get_operator_name_ext_sync
(
  mmgsdi_session_id_type                        session_id,
  mmgsdi_plmn_id_list_type                      plmn_id_list,
  mmgsdi_eons_ignore_disp_cond_enum_type        ignore_display_condition,
  mmgsdi_session_get_operator_name_info_type   *operator_name_info_ptr
)
{
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_eons_display_info_type display_info;
  mmgsdi_eons_name_type       * spn_ptr       = NULL;
  mmgsdi_list_plmn_name_type    plmn_list;
  mmgsdi_eons_name_type         spn;
  mmgsdi_session_type_enum_type session_type  = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_nv_context_type        nv_context    = MMGSDI_NV_CONTEXT_INVALID;
  uint32                        plmn_ids_cnt  = 0;
  
  memset(&plmn_list, 0x0, sizeof(mmgsdi_list_plmn_name_type));
  memset(&spn, 0x0, sizeof(mmgsdi_eons_name_type));
  memset(&display_info, 0x00, sizeof(mmgsdi_eons_display_info_type));

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) NULL check operator_name_info_ptr 
    2) valid ignore display condition check
    3) Session ID
    4) plmn list ptr and num of plmn ids check
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(operator_name_info_ptr);

  if(ignore_display_condition > MMGSDI_EONS_IGNORE_SPN_RPLMN_DISPLAY_COND)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_GET_OPERATOR_NAME_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*return INCORRECT PARAM error when API is called with
    1.Number of plmn ids 0 and a valid plmn list ptr
    2.Or number of plmn ids > 0 and plmn list ptr is NULL*/
  if((plmn_id_list.plmn_list_ptr == NULL && plmn_id_list.num_of_plmn_ids > 0)||
     (plmn_id_list.plmn_list_ptr != NULL && plmn_id_list.num_of_plmn_ids == 0))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    mmgsdi_status =  mmgsdi_util_get_session_type(
                       session_id,
                       &session_type,
                       NULL);
    /* Process get operator name command only when session type is
       GW PRI, GW SEC or 1X PRI*/
    if(MMGSDI_SUCCESS != mmgsdi_status ||
       (session_type != MMGSDI_GW_PROV_PRI_SESSION &&
        session_type != MMGSDI_GW_PROV_SEC_SESSION &&
        session_type != MMGSDI_GW_PROV_TER_SESSION &&
        session_type != MMGSDI_1X_PROV_PRI_SESSION ))
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* When CSG feature is disabled, check if CSG ID is appropriately set */
    if((mmgsdi_util_get_efs_item_index(session_id, &nv_context) ==
          MMGSDI_SUCCESS) &&
       (mmgsdi_nv_get_external_feature_status(MMGSDI_EXT_FEAT_NAS_CFG_FEATURE_CSG, nv_context) ==
         MMGSDI_FEATURE_DISABLED))
    {  
      for (plmn_ids_cnt = 0; plmn_ids_cnt < plmn_id_list.num_of_plmn_ids; plmn_ids_cnt++)
      {
        if ((plmn_id_list.plmn_list_ptr != NULL) &&
            (plmn_id_list.plmn_list_ptr[plmn_ids_cnt].csg_id != MMGSDI_EONS_INVALID_CSG_ID))
        {  
          mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
          break;
        }
      }
    }

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }

    MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

    mmgsdi_status = mmgsdi_eons_run_algo(session_id,
                            MMGSDI_EONS_TRIGGER_API_CALL,
                            plmn_id_list,
                            ignore_display_condition,
                            &(spn),
                            &(operator_name_info_ptr->spn_lang_ind),
                            &plmn_list,
                            &display_info);

    MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }

	if(plmn_list.num_of_plmn_ids == 1)
    {
      operator_name_info_ptr->spn_display_bit = display_info.spn_display_bit;
      operator_name_info_ptr->rplmn_display_bit = display_info.rplmn_display_bit;
      operator_name_info_ptr->roaming_status = display_info.roaming_status;
    } 

    if((plmn_list.num_of_plmn_ids == 1 || session_type == MMGSDI_1X_PROV_PRI_SESSION) &&
       spn.eons_data.data_len > 0 && 
       spn.eons_data.data_ptr != NULL)
    {
      operator_name_info_ptr->spn.eons_encoding = spn.eons_encoding;
      operator_name_info_ptr->spn.spn_data_len = spn.eons_data.data_len;
  
      mmgsdi_memscpy(operator_name_info_ptr->spn.spn_data, 
                     sizeof(operator_name_info_ptr->spn.spn_data), 
                     spn.eons_data.data_ptr,
                     spn.eons_data.data_len);
    }  

    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(spn.eons_data.data_ptr);

    if(plmn_list.plmn_info_ptr != NULL)
    {
      uint32 loop_count = 0;

      for(loop_count = 0;
          loop_count < plmn_list.num_of_plmn_ids;
          loop_count++)
      {  
        operator_name_info_ptr->plmn_info_ptr[loop_count].plmn_id = plmn_list.plmn_info_ptr[loop_count].plmn_id;
        operator_name_info_ptr->plmn_info_ptr[loop_count].lac = plmn_list.plmn_info_ptr[loop_count].lac;
        operator_name_info_ptr->plmn_info_ptr[loop_count].plmn_name_source = plmn_list.plmn_info_ptr[loop_count].plmn_name_source;
  
        operator_name_info_ptr->plmn_info_ptr[loop_count].plmn_long_name.plmn_encoding = plmn_list.plmn_info_ptr[loop_count].plmn_long_name.plmn_name.eons_encoding;
        operator_name_info_ptr->plmn_info_ptr[loop_count].plmn_long_name.plmn_name_ci = plmn_list.plmn_info_ptr[loop_count].plmn_long_name.plmn_name_ci;
        operator_name_info_ptr->plmn_info_ptr[loop_count].plmn_long_name.plmn_name_spare_bits = plmn_list.plmn_info_ptr[loop_count].plmn_long_name.plmn_name_spare_bits;
        operator_name_info_ptr->plmn_info_ptr[loop_count].plmn_long_name.plmn_data_len = plmn_list.plmn_info_ptr[loop_count].plmn_long_name.plmn_name.eons_data.data_len;
        mmgsdi_memscpy(operator_name_info_ptr->plmn_info_ptr[loop_count].plmn_long_name.plmn_data , 
                       sizeof(operator_name_info_ptr->plmn_info_ptr[loop_count].plmn_long_name.plmn_data),
                       plmn_list.plmn_info_ptr[loop_count].plmn_long_name.plmn_name.eons_data.data_ptr,
                       plmn_list.plmn_info_ptr[loop_count].plmn_long_name.plmn_name.eons_data.data_len);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          plmn_list.plmn_info_ptr[loop_count].plmn_long_name.plmn_name.eons_data.data_ptr);
  
        operator_name_info_ptr->plmn_info_ptr[loop_count].plmn_short_name.plmn_encoding = plmn_list.plmn_info_ptr[loop_count].plmn_short_name.plmn_name.eons_encoding;
        operator_name_info_ptr->plmn_info_ptr[loop_count].plmn_short_name.plmn_name_ci = plmn_list.plmn_info_ptr[loop_count].plmn_short_name.plmn_name_ci;
        operator_name_info_ptr->plmn_info_ptr[loop_count].plmn_short_name.plmn_name_spare_bits = plmn_list.plmn_info_ptr[loop_count].plmn_short_name.plmn_name_spare_bits;
        operator_name_info_ptr->plmn_info_ptr[loop_count].plmn_short_name.plmn_data_len = plmn_list.plmn_info_ptr[loop_count].plmn_short_name.plmn_name.eons_data.data_len;
  
        mmgsdi_memscpy(operator_name_info_ptr->plmn_info_ptr[loop_count].plmn_short_name.plmn_data , 
                       sizeof(operator_name_info_ptr->plmn_info_ptr[loop_count].plmn_short_name.plmn_data),
                       plmn_list.plmn_info_ptr[loop_count].plmn_short_name.plmn_name.eons_data.data_ptr,
                       plmn_list.plmn_info_ptr[loop_count].plmn_short_name.plmn_name.eons_data.data_len);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          plmn_list.plmn_info_ptr[loop_count].plmn_short_name.plmn_name.eons_data.data_ptr);
  
        operator_name_info_ptr->plmn_info_ptr[loop_count].plmn_additional_info.info_encoding = plmn_list.plmn_info_ptr[loop_count].plmn_additional_info.info_encoding;
        operator_name_info_ptr->plmn_info_ptr[loop_count].plmn_additional_info.info_data_len = plmn_list.plmn_info_ptr[loop_count].plmn_additional_info.info_data.data_len;

        mmgsdi_memscpy(operator_name_info_ptr->plmn_info_ptr[loop_count].plmn_additional_info.info_data ,
                       sizeof(operator_name_info_ptr->plmn_info_ptr[loop_count].plmn_additional_info.info_data),
                       plmn_list.plmn_info_ptr[loop_count].plmn_additional_info.info_data.data_ptr,
                       plmn_list.plmn_info_ptr[loop_count].plmn_additional_info.info_data.data_len);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          plmn_list.plmn_info_ptr[loop_count].plmn_additional_info.info_data.data_ptr);
      }
      MMGSDIUTIL_TMC_MEM_FREE(plmn_list.plmn_info_ptr);
    }    
  }while(0);
  
  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT

  return mmgsdi_status;
}/* mmgsdi_session_get_operator_name_ext_sync */


/*=====================================================================
FUNCTION        MMGSDI_SESSION_IS_SERVICE_AVAILABLE_SYNC

DESCRIPTION
  This function determines if a service is available or not for a given service type

  This function uses critical sections to protect accesses to
  MMGSDI global data. Hence, it is advisable to pet the dog
  before invoking the function.

PARAMETERS
  session_id:             Session ID of the caller.
  srvc_type:              Service that the client is interested in.
  srv_available_ptr:      Pointer to a boolean type, this pointer holds the information if service is available or not.

DEPENDENCIES
  None.

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The request was successfully handled.
    MMGSDI_ERROR:            The parameters supplied to the API are not
                             within appropriate ranges.


SIDE EFFECTS
  None.
=====================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_is_service_available_sync (
  mmgsdi_session_id_type     session_id,
  mmgsdi_service_enum_type   srvc_type,
  boolean                   *srvc_available_ptr
)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_int_app_info_type  *app_info_ptr    = NULL;
  mmgsdi_client_id_type         client_id           = 0;

  MMGSDIUTIL_RETURN_IF_NULL(srvc_available_ptr);

  /* Protect access to Client ID Reg. Table/App. table */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    mmgsdi_status = mmgsdi_util_get_client_id(session_id, &client_id);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("Invalid session id 0x%x", session_id);
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                       NULL,NULL,
                                                       &app_info_ptr,
                                                       NULL,NULL,NULL);
    if(mmgsdi_status != MMGSDI_SUCCESS || app_info_ptr == NULL)
    {
      UIM_MSG_ERR_0("No app available on this session");
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    if(app_info_ptr->app_data.app_type == MMGSDI_APP_USIM ||
       app_info_ptr->app_data.app_type == MMGSDI_APP_CSIM)
    {
      mmgsdi_status = mmgsdi_uicc_chk_srv_available(client_id,
                                                  session_id,
                                                  MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                  srvc_type,
                                                  srvc_available_ptr);
    }
    else if(app_info_ptr->app_data.app_type == MMGSDI_APP_SIM ||
               app_info_ptr->app_data.app_type == MMGSDI_APP_RUIM)
    {
      mmgsdi_status = mmgsdi_icc_chk_srv_available(session_id,
                                                   MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                   srvc_type,
                                                   srvc_available_ptr);
    }
    else
    {
      mmgsdi_status = MMGSDI_ERROR;
    }
  }while(0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  return mmgsdi_status;
}/* mmgsdi_session_is_service_available_sync */


/*================================================================
FUNCTION:      MMGSDI_SESSION_OPEN_WITH_MF

DESCRIPTION:
  Opens a non-provisioning session and a corresponding logical channel
  on the card.

PARAMETERS:
  client_id:        Client ID of the caller
  slot:             Slot in which card is inserted
  evt_cb_ptr:       Application event callback. A client must provide
                    an event callback if it is to receive
                    session-related event notifications. If the
                    pointer is NULL, the client receives no event
                    notifications.
  response_cb_ptr:  Pointer to the response callback.
  client_ref:       User data returned upon completion of this
                    command.

DEPENDENCIES:
  The client must have a valid client ID.

LIMITATIONS:
  Function cannot be used with ICC cards.
  PIN operations are not supported on the opened Session.

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS:          The command structure was properly
                             Generated and queued onto the MMGSDI
                             Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                             are not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the
                             MMGSDI Task because the max number of
                             commands are already queued.

SIDE EFFECTS:
  The event callback for the session of interest is called every time the MMGSDI
  notifies the client on this session about all session-related events.
================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_open_with_mf (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           slot,
  mmgsdi_evt_callback_type           evt_cb_ptr,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type                 *task_cmd_ptr  = NULL;
  mmgsdi_session_open_with_mf_req_type *msg_ptr       = NULL;
  mmgsdi_return_enum_type               mmgsdi_status = MMGSDI_ERROR;
  int32                                 total_mem_len = 0;
  uint32                                task_cmd_len  = 0;
  mmgsdi_session_type_enum_type         session_type  = MMGSDI_NON_PROV_SESSION_SLOT_1;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Client ID
    2) Slot
    3) Response CB function Check
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if(!mmgsdi_util_is_slot_valid(slot))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_util_get_non_prov_session_type_from_slot(
                    slot,
                    &session_type);
  if(MMGSDI_SUCCESS != mmgsdi_status)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  if(evt_cb_ptr == NULL)
  {
    UIM_MSG_HIGH_0("mmgsdi_session_open_with_mf: NULL evt_cb_ptr");
  }

  /* Memory allocation for msg_ptr and check for allocation */
  total_mem_len = sizeof(mmgsdi_task_cmd_type);

  task_cmd_len = int32touint32(total_mem_len);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  msg_ptr = &task_cmd_ptr->cmd.cmd.session_open_with_mf_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_open_req_type header
      - Command enumeration
      - Client ID
      - Request type
      - Original request type
      - Client Data
      - Response CallBack
 ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SESSION_OPEN_WITH_MF_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_OPEN_WITH_MF_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_OPEN_WITH_MF_REQ;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;
  msg_ptr->request_header.slot_id           = slot;
  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_open_req_type content
    - evt_cb_ptr
    - session_type
  ---------------------------------------------------------------------------*/
  msg_ptr->evt_cb_ptr                       = evt_cb_ptr;
  msg_ptr->session_type                     = session_type;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_OPEN_WITH_MF_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_session_open_with_mf */


/*=============================================================================
FUNCTION        MMGSDI_SESSION_REGISTER_FOR_FILE_UPDATE

DESCRIPTION
  This function allows the client to register or deregister for file update
  notifications for the specific files.
  If the API is called a second time by a client, the file list is updated,
  meaning the previous list is overwritten.
  If the API is called by a client with an empty file list, the client
  is deregistered.

PARAMETERS
  session_id:      Session ID of the caller
  file_watch_list: Files that the client is interested in getting
                   notification for
  response_cb_ptr: Callback to this command
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  This API will be limited to the session on which the client has a valid
  open session

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
  None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_session_register_for_file_update (
  mmgsdi_session_id_type               session_id,
  const mmgsdi_file_update_list_type  *file_watch_list_ptr,
  mmgsdi_callback_type                 response_cb_ptr,
  mmgsdi_client_data_type              client_ref
)
{
  mmgsdi_task_cmd_type                         * task_cmd_ptr     = NULL;
  mmgsdi_session_register_file_update_req_type * msg_ptr          = NULL;
  mmgsdi_return_enum_type                        mmgsdi_status    = MMGSDI_SUCCESS;
  uint32                                         var_len          = 0;
  int32                                          temp_mem         = 0;
  int32                                          total_mem_len    = 0;

  UIM_MSG_HIGH_0("mmgsdi_session_register_for_file_update");

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response Callback Pointer Check
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(file_watch_list_ptr, response_cb_ptr);

  /* File list cannot be empty if num_files > 0 */
  if ((file_watch_list_ptr->num_files > 0) &&
      (file_watch_list_ptr->file_list_ptr == NULL))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* Validate the Session ID */
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  var_len = file_watch_list_ptr->num_files * sizeof(mmgsdi_access_type);

  /* --------------------------------------------------------------------------
   Allocate the memory for this request
   ------------------------------------------------------------------------*/
  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem + var_len;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, int32touint32(total_mem_len));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  if (var_len > 0)
  {
    mmgsdi_util_align_mem(&task_cmd_ptr->cmd.cmd.file_update_req, MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ);
  }

  msg_ptr = &task_cmd_ptr->cmd.cmd.file_update_req;

  /* ----------------------------------------------------------------------------
     Populate the command Request Header with the information provided by
     the client.
     ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.slot_id           = MMGSDI_SLOT_AUTOMATIC;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* --------------------------------------------------------------------------
     Populate the command details with the information provided by the client
    ------------------------------------------------------------------------*/
  if (var_len > 0)
  {
    mmgsdi_memscpy(msg_ptr->file_update_list.file_list_ptr,
                   var_len,
                   file_watch_list_ptr->file_list_ptr,
                   var_len);
  }

  msg_ptr->file_update_list.num_files = file_watch_list_ptr->num_files;

  /* --------------------------------------------------------------------------
     Queue the command, free the command, and return the status
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_session_register_for_file_update */


/*=============================================================================
FUNCTION         MMGSDI_SESSION_REGISTER_ALL_FOR_REFRESH

DESCRIPTION
  This function allows the client to register or unregister for file update
  notifications for any file in a particular DF depending upon the session
  id.
  If the API is called a second time by a client, previous parameters are
  updated only if there is any change in the current request.

PARAMETERS
  session_id             Session ID of the caller.
  is_registration        Indicates if it is registration or de-registration
  vote_for_init          Vote to bring down the call stack or terminate
                         the application.
  response_cb_ptr        Pointer to the response callback.
  client_ref             User data returned upon completion of this command

DEPENDENCIES
  A valid session ID is required.

LIMITATIONS
  This function is limited to the client's session.\n
  If the client has registered for the default application, the appropriate
  client ID must be used.

RETURN VALUE
  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.\n
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because
                             the maximum number of commands are already queued.

========================================================================== */
mmgsdi_return_enum_type mmgsdi_session_register_all_for_refresh (
  mmgsdi_session_id_type                session_id,
  boolean                               is_registration,
  boolean                               vote_for_init,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_refresh_req_type req           = {{0}};

  /* Update request parameters */
  if (is_registration)
  {
    req.refresh_req_type = MMGSDI_REFRESH_ORIG_REG_ALL_REQ;
    req.refresh.reg_req.reg_all_files = TRUE;
  }
  else
  {
    req.refresh_req_type = MMGSDI_REFRESH_ORIG_DEREG_ALL_REQ;
  }

  req.refresh.reg_req.vote_for_init = vote_for_init;

  mmgsdi_status = mmgsdi_session_build_refresh_req(session_id,
                                                   &req,
                                                   response_cb_ptr,
                                                   client_ref);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Command Queuing Failed 0x%x, mmgsdi_session_register_all_for_refresh",
                  mmgsdi_status);
  }
  return mmgsdi_status;
} /* mmgsdi_session_register_all_for_refresh */


/*=============================================================================
FUNCTION        MMGSDI_SESSION_UPDATE_ACSGL_RECORD

DESCRIPTION
  This function allows the client to update (add or delete) a CSG ID associated
  with a PLMN ID in the EF-ACSGL record.

PARAMETERS
  session_id:      Session ID of the caller
  update_type:     Type of update of CSG ID requested
  plmn_csg_id:     CSG ID to be updated and associated PLMN ID
  hbn_name:        Home Node(b) name corresponding to the CSG ID
  response_cb_ptr: Callback to this command
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  This API will be limited to the session on which the client has a valid
  open session

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
  None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_session_update_acsgl_record(
  mmgsdi_session_id_type               session_id,
  mmgsdi_update_acsgl_enum_type        update_type,
  mmgsdi_plmn_csg_id_type              plmn_csg_id,
  mmgsdi_data_type                     hnb_name,
  mmgsdi_callback_type                 response_cb_ptr,
  mmgsdi_client_data_type              client_ref
)
{
  return mmgsdi_session_queue_update_acsgl_record_req(session_id,
                                                      update_type,
                                                      plmn_csg_id,
                                                      hnb_name,
                                                      response_cb_ptr,
                                                      client_ref);
} /* mmgsdi_session_update_acsgl_record */


/* ===========================================================================
FUNCTION        MMGSDI_SESSION_SET_FILE_STATUS

DESCRIPTION
  This function provides a way to activate (rehabilitate) or deactivate
  (invalidate) a file in the SIM/USIM/RUIM/CSIM.

PARAMETERS
  session_id:           Session ID of the caller.
  file_access:          File to activate.
  req_type:             Activate or deactivate request.
  response_cb_ptr:      Response callback.
  client_ref:           User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
  None.
========================================================================== */
static mmgsdi_return_enum_type  mmgsdi_session_set_file_status (
  mmgsdi_session_id_type   session_id,
  mmgsdi_access_type       file_access,
  mmgsdi_cmd_enum_type     req_type,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
  mmgsdi_task_cmd_type             * task_cmd_ptr   = NULL;
  mmgsdi_set_file_status_req_type  * msg_ptr        = NULL;
  mmgsdi_return_enum_type            mmgsdi_status  = MMGSDI_ERROR;
  uint32                             task_cmd_len   = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Response CB function Check
    3) Check for EF and Path access
    4) Check for path length and MF/DF/EF identifiers
    5) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  if (req_type != MMGSDI_INVALIDATE_REQ && req_type != MMGSDI_REHABILITATE_REQ)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    req_type);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);
  if ((file_access.access_method != MMGSDI_BY_PATH_ACCESS) &&
      (file_access.access_method != MMGSDI_EF_ENUM_ACCESS))
  {
    UIM_MSG_ERR_1("Invalid Access Method Type 0x%x", file_access.access_method);
    return MMGSDI_INCORRECT_PARAMS;
  }

  if((file_access.access_method == MMGSDI_BY_PATH_ACCESS) &&
     !mmgsdi_util_is_path_valid(&file_access.file.path_type))
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - No need to align pointers
    ---------------------------------------------------------------------------*/
  if (req_type == MMGSDI_REHABILITATE_REQ)
  {
    msg_ptr = &task_cmd_ptr->cmd.cmd.rehab_req;
  }
  else
  {
    msg_ptr = &task_cmd_ptr->cmd.cmd.invalidate_req;
  }

  /*---------------------------------------------------------------------------
    Populating mmgsdi_read_req_type header
      1) Session ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = req_type;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.request_type      = req_type;
  msg_ptr->request_header.orig_request_type = req_type;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_invalidate_req_type/mmgsdi_rehabilitate_req_type content
    - access
      1) Access method in this function is limited to ENUM or PATH access only
      2) File name in the form of file enum type
      Mem malloc and validate, if fail, free memory space and return
    [record number is not being populated because this is a read transparent
     function]
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_copy_access_type(&msg_ptr->access, &file_access);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the copying of access type fail*/
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    return mmgsdi_status;
  }

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
    onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_2("Queue of MMGSDI command: 0x%x status 0x%x",
                 req_type, mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_session_set_file_status */


/* ===========================================================================
FUNCTION        MMGSDI_SESSION_ACTIVATE_FILE

DESCRIPTION
  This function provides a way to activate (rehabilitate) a file in the
  SIM/USIM/RUIM/CSIM.

PARAMETERS
  session_id:           Session ID of the caller.
  file_access:          File to activate.
  response_cb_ptr:      Response callback.
  client_ref:           User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
  None.
========================================================================== */
mmgsdi_return_enum_type  mmgsdi_session_activate_file (
  mmgsdi_session_id_type   session_id,
  mmgsdi_access_type       file_access,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
  return mmgsdi_session_set_file_status(session_id,
                                        file_access,
                                        MMGSDI_REHABILITATE_REQ,
                                        response_cb_ptr,
                                        client_ref);
}/* mmgsdi_session_activate_file */


/* ===========================================================================
FUNCTION        MMGSDI_SESSION_DEACTIVATE_FILE

DESCRIPTION
  This function provides a way to deactivate (invalidate) a file in the
  SIM/USIM/RUIM/CSIM.

PARAMETERS
  session_id:           Session ID of the caller.
  file_access:          File to deactivate.
  response_cb_ptr:      Response callback.
  client_ref:           User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
  None.
========================================================================== */
mmgsdi_return_enum_type  mmgsdi_session_deactivate_file (
  mmgsdi_session_id_type   session_id,
  mmgsdi_access_type       file_access,
  mmgsdi_callback_type     response_cb_ptr,
  mmgsdi_client_data_type  client_ref
)
{
  return mmgsdi_session_set_file_status(session_id,
                                        file_access,
                                        MMGSDI_INVALIDATE_REQ,
                                        response_cb_ptr,
                                        client_ref);
}/* mmgsdi_session_deactivate_file */


/*=============================================================================
 FUNCTION:      MMGSDI_SESSION_CANCEL_REQUESTS
=============================================================================*/
/**
  This API is used to cancel all the pending requests related to the particular
  session ID.

  @param[in] session_id  Session ID of the requests the need to be cancelled.

  @return
  MMGSDI_SUCCESS          -- Command was properly processed.\n
  MMGSDI_INCORRECT_PARAMS -- The parameters supplied to the API are not within
                             appropriate ranges.\n

  @dependencies
  A valid session ID for a provisioning application is required.
*/
mmgsdi_return_enum_type mmgsdi_session_cancel_requests(
  mmgsdi_session_id_type              session_id
)
{
  mmgsdi_cancel_info_type  cancel_info;
  mmgsdi_return_enum_type  mmgsdi_status  = MMGSDI_SUCCESS;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
  ---------------------------------------------------------------------------*/
  if((session_id == MMGSDI_SESSION_ID_ZERO) ||
     (session_id == MMGSDI_INVALID_SESSION_ID))
  {
    UIM_MSG_ERR_1("Session id is Invalid: 0x%x", session_id);
    return MMGSDI_INCORRECT_PARAMS;
  }

  /* Update cancel info */
  memset(&cancel_info, 0x00, sizeof(mmgsdi_cancel_info_type));
  cancel_info.cancel_type = MMGSDI_CANCEL_USING_SESSION_ID;
  cancel_info.id.session_id = session_id;

  /* We need to enter this critical section because we want the MMGSDI task
     command queue manipulation to be done in the client API context */
  MMGSDIUTIL_ENTER_TASK_SYNC_CRIT_SECT;
  mmgsdi_status = mmgsdi_process_cancel_req(cancel_info);
  MMGSDIUTIL_LEAVE_TASK_SYNC_CRIT_SECT;

  return mmgsdi_status;
} /* mmgsdi_session_cancel_requests */


/* ===========================================================================
FUNCTION        MMGSDI_SESSION_RESELECT_AID

DESCRIPTION
  This function provides a way to SELECT an AID on an already-opened channel
  using the given selection mode.

PARAMETERS
  session_id:           Session ID of the caller.
  select_mode:          Mode for SELECT command.
  response_cb_ptr:      Response callback.
  client_ref:           User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
  None.
========================================================================== */
mmgsdi_return_enum_type  mmgsdi_session_reselect_aid (
  mmgsdi_session_id_type         session_id,
  mmgsdi_select_mode_enum_type   select_mode,
  mmgsdi_callback_type           response_cb_ptr,
  mmgsdi_client_data_type        client_ref
)
{
  mmgsdi_task_cmd_type           *task_cmd_ptr  = NULL;
  mmgsdi_get_file_attr_req_type  *msg_ptr       = NULL;
  mmgsdi_return_enum_type         mmgsdi_status  = MMGSDI_ERROR;
  int32                           total_mem_len  = 0;
  int32                           temp_mem       = 0;
  int32                           var_len        = 0;

  if (select_mode != MMGSDI_SELECT_MODE_NEXT)
  {
    UIM_MSG_ERR_1("AID select mode 0x%x not supported", select_mode);
    return MMGSDI_NOT_SUPPORTED;
  }

  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_SELECT_AID_REQ);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem + var_len;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, int32touint32(total_mem_len));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  if (var_len > 0)
  {
    mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.get_file_attr_req,
                           MMGSDI_SESSION_SELECT_AID_REQ);
  }

  msg_ptr = &task_cmd_ptr->cmd.cmd.get_file_attr_req;

  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SESSION_SELECT_AID_REQ;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_SELECT_AID_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_SELECT_AID_REQ;
  msg_ptr->request_header.response_cb       = response_cb_ptr;
  msg_ptr->request_header.client_data       = client_ref;

  msg_ptr->activate_aid                     = TRUE;
  msg_ptr->aid_select_mode                  = select_mode;
  msg_ptr->access.access_method             = MMGSDI_BY_APP_ID_ACCESS;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_2("Queue of MMGSDI_SESSION_SELECT_AID_REQ status: 0x%x, select mode: 0x%x",
                 mmgsdi_status, select_mode);

  return mmgsdi_status;
} /* mmgsdi_session_reselect_aid */


/* ===========================================================================
FUNCTION        MMGSDI_SESSION_GET_MCC_MNC_VALUES_FROM_IMSI

  This function provides MCC, MNC values from IMSI

  @param[in]  session_id:      Session ID of the caller.
  @param[out] mcc_value_ptr:   Buffer to store MCC data.
  @param[out] mnc_value_ptr:   Buffer to store MNC data.

  @dependencies
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open_ext().

  @return
  MMGSDI_SUCCESS:                     Successful construction of MCC & MNC values
  MMGSDI_INCORRECT_PARAMS:            The parameters supplied to the API are not
                                      within appropriate ranges.

  @limitations
  None

  @side effects
  None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_session_get_mcc_mnc_values_from_imsi(
  mmgsdi_session_id_type         session_id,
  uint16                       * mcc_value_ptr,
  uint16                       * mnc_value_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status                = MMGSDI_SUCCESS;
  mmgsdi_data_type                   imsi_data                    = {0, NULL};
  uint8                              imsi_buffer[MMGSDI_IMSI_LEN] = {0};
  uint8                              ad_buffer[MMGSDI_EF_AD_LEN]  = {0};
  mmgsdi_data_type                   ad_data                      = {0, NULL};
  mmgsdi_int_app_info_type          *app_info_ptr                 = NULL;
  mmgsdi_file_enum_type              imsi_file                    = MMGSDI_GSM_IMSI;
  mmgsdi_file_enum_type              ad_file                      = MMGSDI_GSM_AD;
  uint8                              num_of_mnc_digit             = 0;
  mmgsdi_access_type                 access;

  MMGSDIUTIL_RETURN_IF_NULL_2(mcc_value_ptr, mnc_value_ptr);

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  do
  {
    mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                     NULL,
                                                     NULL,
                                                     &app_info_ptr,
                                                     NULL,
                                                     NULL,
                                                     NULL);

    if(mmgsdi_status != MMGSDI_SUCCESS || app_info_ptr == NULL)
    {
      UIM_MSG_ERR_1("No app available on this session: 0x%x", session_id);
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    if(app_info_ptr->app_data.app_type == MMGSDI_APP_SIM)
    {
      imsi_file = MMGSDI_GSM_IMSI;
      ad_file   = MMGSDI_GSM_AD;
    }
    else if(app_info_ptr->app_data.app_type == MMGSDI_APP_USIM)
    {
      imsi_file = MMGSDI_USIM_IMSI;
      ad_file   = MMGSDI_USIM_AD;
    }
    else
    {
      UIM_MSG_ERR_1("App type not supported: 0x%x", app_info_ptr->app_data.app_type);
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    memset(&access, 0x00, sizeof(mmgsdi_access_type));

    /* Read IMSI data from cache */
    access.access_method = MMGSDI_EF_ENUM_ACCESS;
    access.file.file_enum = imsi_file;

    mmgsdi_status = mmgsdi_cache_read_len(session_id,
                                          &access,
                                          &imsi_data.data_len,
                                          0,
                                          NULL);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }

    if(imsi_data.data_len > MMGSDI_IMSI_LEN)
    {
      imsi_data.data_len = MMGSDI_IMSI_LEN;
    }

    imsi_data.data_ptr = imsi_buffer;

    mmgsdi_status = mmgsdi_cache_read(session_id,
                                      &access,
                                      &imsi_data,
                                      0,
                                      NULL);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }

    /* Read AD data from cache */
    access.access_method = MMGSDI_EF_ENUM_ACCESS;
    access.file.file_enum = ad_file;

    mmgsdi_status = mmgsdi_cache_read_len(session_id,
                                          &access,
                                          &ad_data.data_len,
                                          0,
                                          NULL);

    /* Proceed even if EF AD is missing */
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_status = MMGSDI_SUCCESS;
      break;
    }

    if(ad_data.data_len > MMGSDI_EF_AD_LEN)
    {
      ad_data.data_len = MMGSDI_EF_AD_LEN;
    }

    ad_data.data_ptr = ad_buffer;

    mmgsdi_status = mmgsdi_cache_read(session_id,
                                      &access,
                                      &ad_data,
                                      0,
                                      NULL);

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      ad_data.data_ptr = NULL;
      mmgsdi_status = MMGSDI_SUCCESS;
      break;
    }
  } while(0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_num_of_mnc_digits(imsi_data, ad_data, &num_of_mnc_digit);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Calculate MCC value */
  *mcc_value_ptr  = (uint16)((imsi_data.data_ptr[MMGSDIUTIL_IMSI_DIGIT_1_OFFSET] >> 4) &
                    MMGSDIUTIL_IMSI_LOW_DIGIT_MASK) * 100;
  *mcc_value_ptr += (uint16)((imsi_data.data_ptr[MMGSDIUTIL_IMSI_DIGIT_2_OFFSET]) &
                    MMGSDIUTIL_IMSI_LOW_DIGIT_MASK) * 10;
  *mcc_value_ptr += (uint16)((imsi_data.data_ptr[MMGSDIUTIL_IMSI_DIGIT_2_OFFSET] >> 4) &
                    MMGSDIUTIL_IMSI_LOW_DIGIT_MASK);

  /* Calculate MNC value */
  if(num_of_mnc_digit == MMGSDIUTIL_MNC_3_DIGITS)
  {
    *mnc_value_ptr  = (uint16)((imsi_data.data_ptr[MMGSDIUTIL_IMSI_DIGIT_3_OFFSET]) &
                      MMGSDIUTIL_IMSI_LOW_DIGIT_MASK) * 100;
    *mnc_value_ptr += (uint16)((imsi_data.data_ptr[MMGSDIUTIL_IMSI_DIGIT_3_OFFSET] >> 4) &
                      MMGSDIUTIL_IMSI_LOW_DIGIT_MASK) * 10;
    *mnc_value_ptr += (uint16)((imsi_data.data_ptr[MMGSDIUTIL_IMSI_DIGIT_4_OFFSET]) &
                      MMGSDIUTIL_IMSI_LOW_DIGIT_MASK);
  }
  else
  {
    *mnc_value_ptr  = (uint16)((imsi_data.data_ptr[MMGSDIUTIL_IMSI_DIGIT_3_OFFSET]) &
                      MMGSDIUTIL_IMSI_LOW_DIGIT_MASK) * 10;
    *mnc_value_ptr += (uint16)((imsi_data.data_ptr[MMGSDIUTIL_IMSI_DIGIT_3_OFFSET] >> 4) &
                      MMGSDIUTIL_IMSI_LOW_DIGIT_MASK);
  }

  UIM_MSG_MED_2("Get MCC-MNC values: MCC: 0x%x, MNC: 0x%x", *mcc_value_ptr, *mnc_value_ptr);

  return MMGSDI_SUCCESS;
} /* mmgsdi_session_get_mcc_mnc_values_from_imsi */
