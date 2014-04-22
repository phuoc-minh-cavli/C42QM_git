/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   C N F   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains functions to build confirmation message for
  all the commands.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2020, 2023 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_cnf.c#2 $$ $DateTime: 2023/06/21 11:32:16 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/23/23   tq	   Modem base IMSI switch
06/24/20   cj      Sync QMI UIM with proper slot status upon task start up
03/06/19   vgd     GET EID for M2M cards where LPA is not supported
11/19/18   tq      Acquire app critical section before run eons algo
10/31/18   tq      Removal of MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_REQ
10/24/18   rps     Refragment alloc extra param code
07/27/18   tq      Moving function to stub file
07/09/18   tq      UIM interface optimization
10/13/17   rps     Remove old  perso support
09/27/16   sn      Send only one response for SELECT_AID request
06/22/16   tkl     Stub out additional APIs
06/07/16   ar      Removing string prints from APDU and AUTH
05/25/16   vdc     Remove F3 messages for memory allocation failure
05/24/16   ar      Review of macros used by MMGSDI
05/17/16   tkl     F3 log prints cleanup
05/11/16   vdc     Guard with FEATURE_MMGSDI_3GPP2 for 3GPP2 related functions
05/04/16   sp      Don't reject activation req during ecc call for AP centric
05/02/16   ar      Removing the mmgsdi_cnf_map_req_to_cnf_enum_type()
05/02/16   ar      Snip out 3GPP2 code in new file
05/02/16   sp      Reduce F3 log prints
04/18/16   vdc     Send act resp and session changed evt at same time
03/09/16   vdc     Removed JCDMA related code
03/09/16   vdc     Remove bcast related code
03/08/16   sp      Fix compilation warnings
03/08/16   sp      Remove length information from requests and confirmations
11/23/15   vdc     Send profile switching evt when profile has switched in card
10/12/15   vdc     Add support for potential silent file change notification
09/25/15   vr      Added support for ERA-GLONASS
08/10/15   vdc     Fix potential buffer overflow in cdma AKA cnf
05/29/15   hh      Convert MANAGE CHANNEL APDU to MMGSDI requests
12/15/14   kk      Fix MCFG UIM data sent for nv refresh
11/14/14   hh      Fix compilation warnings
11/05/14   ar      Remove mmgsdi_get_card_reader_status () API
10/23/14   yt      Updates to session and app management
09/27/14   ar      Add support for multiple slots in BT-SAP
09/03/14   yt      Return complete response data in INCREASE confirmation
08/29/14   hh      Fix compliation warnings
08/27/14   tl      Introduce network related SIM Lock slot policies
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/15/14   kk      NV Refresh: MCFG - MMGSDI interactions
07/15/14   kk      NV Refresh: MCFG - MMGSDI interactions
07/15/14   yt      Support for NV refresh
07/10/14   tl      Remove ALWAYS OFF feature FEATURE_MMGSDI_PKCS15
06/30/14   vv      Remove Perso code
05/12/14   vv      Fix compiler errors
05/12/14   bcho    NULL check added before dereferencing extra_param_ptr
04/17/14   av      Enhance MMGSDI attributes caching
04/08/14   ar      Skip sending event to clients if service status is same
03/25/14   yt      Handle refresh of deactivated app
03/24/14   ar      Remove MMGSDI_SESSION_ENABLE/DISABLE_FDN_REQ
03/21/14   tl      Introduce new SIM Lock feature
03/13/14   ar      Update correct PRL_VERSION and PRL_REVISION
02/25/14   yt      Support for selection mode in SELECT AID request
02/25/14   bcho    Added support to store OPLMN List in EFS and cache it
02/24/14   ar      Removed wrapper function mmgsdi_util_alloc()
02/24/14   ar      Replace session checks in switch with utility functions
02/13/14   ar      Replace session checks with utility functions
01/22/13   tl      Fix KW errors
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
01/09/14   ar      Replace old Macros with MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE
12/11/13   df      Remove unused memory paging feature
12/05/13   ar      Replaced all mmgsdi_malloc direct calls with macro
11/22/13   vv      Added input parameter check for the CSG ID
11/12/13   yt      Fixes for multi-SIM OnChip activation
11/06/13   tl      Remove TO-DO
10/04/13   rp      F3 message reduction
09/23/13   df      Support for SIM busy
08/30/13   tl      Convert channel info table from static to dynamic allocation
08/07/13   tl      Add CDMA AKA Authenticate functionality
07/15/13   vs      EONS updates to support single baseband SGLTE
07/09/13   vdc     Fix and code optimization for get available apps
06/28/13   bcho    APIs added to return plmn info from SE13 table.
06/24/13   vdc     Added support for triple SIM
06/18/13   vdc     Fix to send session deactivate failure CNF to QMI UIM
05/31/13   spo     Fixed logic for get_all_available_apps()
05/28/13   tl      Appropriately check response status for Otasp Commit
05/16/13   vdc     Replace memcpy with safer version memscpy
05/09/13   vdc     Memory optimizations for slot, app, pin, upin tables
05/06/13   spo     Modified CPHS variables to array of pointers
04/16/13   tl      Clean up of radio access features from MMGSDI code
04/15/13   tl      Remove obsoleted featurization
04/12/13   spo     Do not send events to clients before Evt Reg CNF is sent
04/11/13   vv      Re-design handling of VPE response from the card
04/01/13   spo     Do not send session events before Session Open CNF is sent
03/04/13   av      Updates for T3245 timer support to mark APP as legal/valid
01/20/13   vv      Added support for updating EF-ACSGL record
12/12/12   av      Fixed compiler critical warnings
12/5/12    vv      Added support for parallel processing in MMGSDI
10/15/12   bcho    NULL chcek added for cphs_int_info_ptr
10/12/12   abg     Do not send provisioning events to non-prov sessions
10/11/12   spo     Removed unnecessary F3 messages
10/09/12   tl      Added support for compute IP Auth data-less response
10/01/12   abg     Removed Problem determining protocol type F3 message
09/28/12   abg     Updated ERR to MSG_ERROR
09/20/12   bcho    Added support for mmgsdi_get_SE13_plmn_names API
09/11/12   av      Handling of VPE response from the card
09/11/12   bcho    Support for ignore display condition param in
                   MMGSDI_SESSION_GET_OPERATOR_NAME_REQ handling
09/07/12   abg     Updated RUIM HRPD indicators status
09/03/12   bcho    Include mmgsdi_eons.h
07/19/12   tl      Support more application and logical channels
07/13/12   vv      Added support to identify apps not in EF-DIR
05/23/12   vv      Added support for the file update notification registration
03/29/12   shr     Added support for Session open with MF, support for Send
                   APDU extension API
03/20/12   yt      Add request type for session_open_with_select_rsp
03/16/12   yt      Fix compiler warnings
03/14/12   nmb     Ensure QMI response for logical channel failures
03/07/12   bcho    Return specific error if perso feature is not activated/
                   deactivated because of incorrect parameters
03/01/12   kk      Added support for app state extraction in get_session_info
02/23/12   bcho    Critical Section Enter/Leave function calls replaced by
                   respective macro to add logging
02/22/12   vv      Send PIN events in case of a decrease in PIN retries count
01/30/12   shr     Maintain flag to indicate last successful OTASP Commit
01/30/12   bcho    Added support for MMGSDI_EONS_SS_EVENT_REQ
01/14/12   bcho    Changes in mmgsdi_cnf_build_available_apps_data to provide
                   pin info only when it is cached
12/23/11   shr     Removed featurization for ALWAYS ON features
12/21/11   shr     Legacy GSDI removal updates
12/21/11   bcho    Changes for new API to open multiple sessions
                   in a single request
12/21/11   bcho    Added support for Get Operator Name API
08/22/11   bcho    Return specific error when perso feature is not activated/
                   deactivated/unblocked because mmgsdi is not ready.
08/01/11   av      Slot-specific handling of JCDMA apis
08/01/11   vs      Updates for key ref based PIN operations
08/01/11   vs      Added support for session open with select response
05/24/11   nmb     Added support for ISIM GBA Authentication
05/20/11   yt      Added support for INCREASE request
05/17/11   kk      Updates for Thread safety
05/17/11   shr     Updates for Thread safety
04/26/11   vs      Support for subscription ok request
04/01/11   vs      Return status for failed manage channel and select AID
03/31/11   yt      Fixed compilation warning
02/21/11   ms      Implementation of Session based caching on App capabilities
02/21/11   ms      Fix for Get file attribute failure with ISIM
02/11/11   ssr     Clear RUIM and GSDI cache after successful OTASP commit
01/31/11   ssr     Decoupled FEATURE_UIM_JCDMA_RUIM_SUPPORT feature
01/07/11   nmb     Added accessed offset to read_data cnf
01/07/11   yt      Fixed union usage for enable service confirmation
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
12/17/10   shr     Send FDN event to the session on which the FDN has been
                   updated on
11/30/10   nmb     Fixed compilation error from SQA merge
11/23/10   nmb     Updated ISIM Auth to correct possible out of bounds read
10/19/10   kk      Added support for sending FDN event when EF FDN is updated
10/05/10   nmb     Removed extra bytes from task_cmd buffer
09/29/10   yt      Enabling support for INCREASE
09/15/10   nmb     Allocate mmgsdi_client_id_reg_table dynamically
09/08/10   nmb     mmgsdi_task_cmd_type memory reduction
08/30/10   shr     Perso clean up for Sessions
08/18/10   mib     Fixed Lint warnings
08/12/10   nmb     Added MMGSDI_PERSO_CK_BLOCKED for blocked indicators
08/10/10   nmb     Meaningful error results for perso_cnf activate/block
08/06/10   vs      Populating power up option in CNF when card is reset
08/02/10   adp     valid_num_retries and valid_num_unblock_retries set is
                   not updated in the confirmation
08/02/10   adp     Fixing compiler warning in SAP.
07/28/10   yt      Updated read_record to return data for requested length
07/21/10   nb      Fix for SAP Send APDU
07/14/10   js      Fixed compilation warnings
06/30/10   adp     Correct offset for Perso SIM features
06/30/10   adp     Fixing Lint errors
06/28/10   shr     Added support for Hidden key event
06/24/10   yt      Klocwork fixes
06/18/10   shr     Send PIN2 event to Sessions corresponding to all
                   apps sharing the same key reference only on ICC cards
04/27/10   shr     Added Hiddenkey support
04/16/10   yt      Fixed compilation warnings
04/07/10   yt      Fixed compilation warnings
04/07/10   nb      SAP Update
03/09/10   shr     Clean up REFRESH FCN, INIT, INIT_FCN and APP_RESET
03/02/10   kk      Fixed get all available apps command handling
02/19/10   shr     Session get info API implementation
02/17/10   shr     Fixed Session activate/deactivate handling
02/09/10   vs      Fixed available apps logic
02/01/10   jk      EPRL Support
01/06/10   shr     Fixed Session Close handling
01/06/10   kk      Fixed Get all available cnf building
12/16/09   shr     Fixed Perso Lock Down handling
12/18/09   nb      Moved Dual Slot Changes
12/16/09   ssr     Fixed OTAPA response data length
12/15/09   ssr     Sending PIN event to all applications who have same pin
                   reference
12/14/09   ssr     Removed MMGSDI_SELECT_AID_REQ
12/11/09   kk      Removed featurization FEATURE_MMGSDI_MBMS for USIM AUTH req
12/07/09   js      Fixed Lint Error
12/04/09   kp      Replacing mmgsdi_data_slot2 with mmgsdi_data_slot2_ptr
12/03/09   rn      Added support for accessing cached field
11/27/09   rn      Fixed setting of prov_app field in slot_data_ptr
10/12/09   kp      Moving Read-Write varibles to Heap
09/26/09   kp      ZI memory reduction changes
09/08/09   rn      Fixed klocwork errors
08/24/09   shr     Fixed compliation errors (for CDMA only)
07/28/09   mib     Added mmgsdi_perso_lock_down_and_enable_limited_access
07/14/09   shr     Handle STATUS command confirmation case where no
                   status response data is expected
07/10/09   kk      Added support for returning path info get_file_attr response
07/10/09   rn      Fixed Lint errors
07/01/09   yk      Flip the MMGSDI_STATE and the card protocol to allow the
                   MMGSDI to process commands for MFLO
06/01/09   ssr     Merging of SIM and RUIM PERSO
05/15/09   nb      Added support for perso_get_unblock_dck_retries command
04/01/09   ssr     Adding index for mmgsdi perso cnf
03/23/09   nb      Updated perso response length in response header to take
                   into account dynamically allocated data
03/12/09   js      Support for reporting invalid number of PIN retries
03/02/09   sun     Redid perso cnf names
02/24/09   nb      Perso Command support in MMGSDI
01/21/09   kk      Added channel id to session_open cnf data
12/17/08   nb      Common Modem Interface
08/20/08   jk      Fixes for CAVE API & Generate Key API
08/08/08   tml     Fixed header rev update issue
07/17/08   kk      Added support for PKCS 15
06/12/08   ssr     Support of MMGSDI Search functionality
06/23/08   tml     Added featurization for BT in WM
06/04/08   tml     Added sw support to the cnf response header
03/24/08   sun     Added support for OMA Bcast
03/14/08   nk      Updated mmgsdi_cnf_build_rehabilitate_data
01/24/08   nk      Updated JCDMA card color cnf with change me esn flag
11/15/07   sun     Added support for NAA refresh
11/15/07   tml     Use regular rex function for critical sections for
                   mmgsdi_task_rsp_q to prevent gsdi task stuck when synchronous
                   commands were issued
09/27/07   tml     Lint fixes
09/23/07   sk      Put a lock around mmgsdi_task_rsp
09/10/07   tml     Added MFLO Support
08/15/07   jk      Changes to Support DATA Authentication MMGSDI API
05/17/07   wli     Added support for JCDMA RUIM
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
05/02/07   sun     Added support for Service Available and USIM Auth
03/19/07   sun     Fixed Lint Error
03/07/07   sun     Added support for Disable with Replacement
01/08/07   sun     Get the AppId from the Client ID for All Pin Operations
12/28/06   tml     Populated offset and data len for write confirmation
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/07/06   tml     Added ACL support
08/07/06   sun     Send PIN ID regardless of status
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/12/06   sun     Check for the right mmgsdi_state while setting Card Reader
                   status
06/08/06   tml     fixed featurized
05/23/06   tml     Lint fixes and BDN, rehabilitate and invalidate supports
04/15/06   tml     moved onchip member to global data structure
04/11/06   jar     Added ONCHIP SIM support
02/22/06   tml     Returned client id as part of the open and close cnf
01/04/06   tml     Updated enum name for SAP and Card powerup and down
02/24/05   tml     Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "intconv.h"
#include "mmgsdi.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_cnf.h"
#include "uim.h"
#include "mmgsdi_uicc_rsp.h"
#include "mmgsdi_evt.h"
#include "mmgsdi_icc_rsp.h"
#include "mmgsdi_sap_rsp.h"
#include "bit.h"
#include "mmgsdi_session.h"
#include "mmgsdi_file_update.h"
#include "task.h"
#include "mmgsdi_eons.h"
#include "mmgsdi_builtin_plmn.h"
#include "mmgsdicache.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_multi_profile.h"
#include "mmgsdi_cphs.h"
#include "mmgsdi_csg.h"
#include "mmgsdi_onchip.h"
#include "mmgsdi_sap.h"
#include "mmgsdi_icc.h"
#include "mmgsdi_uicc.h"
#include "mmgsdi_euicc_m2m.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/
/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_CLOSE_DATA

   DESCRIPTION:
     This function populates the Session Close data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_cnf_build_close_data(
  mmgsdi_session_close_cnf_type             *close_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  mmgsdi_session_id_type                    session_id,
  const mmgsdi_session_cnf_add_info_type   *session_data_ptr);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_OPEN_DATA

   DESCRIPTION:
     This function populates the Session Open data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_cnf_build_open_data(
  mmgsdi_session_open_cnf_type             *open_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  mmgsdi_session_id_type                    session_id,
  const mmgsdi_session_cnf_add_info_type   *session_data_ptr);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_OPEN_EXT_DATA

   DESCRIPTION:
     This function populates the Session Open ext data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_cnf_build_open_ext_data(
  mmgsdi_session_open_ext_cnf_type             * open_ext_cnf_ptr,
  mmgsdi_return_enum_type                        mmgsdi_status,
  const mmgsdi_session_open_ext_cnf_info_type  * session_data_ptr);


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SESSION_GET_INFO_DATA

   DESCRIPTION:
     This function builds the Cnf data for session get info command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_cnf_build_session_get_info_data(
    mmgsdi_session_get_info_cnf_type              *session_get_info_cnf_ptr,
    mmgsdi_return_enum_type                       *mmgsdi_status,
    const mmgsdi_session_get_info_extra_info_type *session_get_info_data_ptr
);


/*=============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SESSION_GET_APP_CAPABILITIES_DATA

   DESCRIPTION:
     This function builds the Cnf data for session get app capabilities command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_cnf_build_session_get_app_capabilities_data(
  mmgsdi_slot_id_enum_type                  slot_id,
  mmgsdi_session_app_capabilities_cnf_type *session_get_app_capabilities_cnf_ptr,
  mmgsdi_session_id_type                    session_id
);


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SESSION_ENABLE_SERVICE_DATA

   DESCRIPTION:
     This function builds the CNF data for Enable service request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:    The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
 mmgsdi_return_enum_type mmgsdi_cnf_build_session_enable_service_data(
   mmgsdi_return_enum_type                  mmgsdi_status,
   mmgsdi_session_enable_service_cnf_type  *session_enable_service_cnf_ptr,
   const mmgsdi_session_enable_service_extra_info_type
                                           *session_enable_service_data_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SESSION_DISABLE_SERVICE_DATA

   DESCRIPTION:
     This function builds the CNF data for Disable service request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:    The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
 mmgsdi_return_enum_type mmgsdi_cnf_build_session_disable_service_data(
   mmgsdi_return_enum_type                  mmgsdi_status,
   mmgsdi_session_disable_service_cnf_type *session_disable_service_cnf_ptr,
   const mmgsdi_session_enable_service_extra_info_type
                                           *session_disable_service_data_ptr
);


/*=============================================================================
  FUNCTION: MMGSDI_CNF_GET_ATR_LEN

  DESCRIPTION:
    This is the function called from the main procedure handling function.
    This funciton will validate the important parameters.  Once the
    parameters pass the checking, it will call the GSDI - UIM Function
    to send it to the UIM.

  PARAMETERS:
    slot: The slot to the attribute len of.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    length of the ATR

  SIDE EFFECTS:
    None
=============================================================================*/
static uint8 mmgsdi_cnf_get_atr_len(
    mmgsdi_slot_id_enum_type slot_id)
{
  uint8                  len           = 0;
  mmgsdi_slot_data_type *slot_data_ptr = NULL;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if(slot_data_ptr != NULL)
  {
    len = slot_data_ptr->card_atr.atr_length;
  }

  return len;
}/* mmgsdi_cnf_get_atr_len */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_POPULATE_CNF_RSP_HEADER

   DESCRIPTION:
     This function populate the confirmation header information

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
=============================================================================*/
void mmgsdi_cnf_populate_cnf_rsp_header(
  mmgsdi_callback_cnf_type          * cnf_ptr,
  const mmgsdi_request_header_type  * req_hdr_ptr,
  mmgsdi_return_enum_type             mmgsdi_status,
  boolean                             sw_present,
  mmgsdi_sw_status_type               status_word
)
{
  if ((cnf_ptr == NULL) || (req_hdr_ptr == NULL))
  {
    return;
  }

  cnf_ptr->client_req_cb                                  = req_hdr_ptr->response_cb;
  cnf_ptr->mmgsdi_cnf.response_header.client_data         = req_hdr_ptr->client_data;
  cnf_ptr->mmgsdi_cnf.response_header.client_id           = req_hdr_ptr->client_id;
  cnf_ptr->mmgsdi_cnf.response_header.session_id          = req_hdr_ptr->session_id;
  cnf_ptr->mmgsdi_cnf.response_header.slot_id             = req_hdr_ptr->slot_id;
  cnf_ptr->mmgsdi_cnf.response_header.mmgsdi_status       = mmgsdi_status;
  cnf_ptr->mmgsdi_cnf.response_header.status_word.present = sw_present;
  cnf_ptr->mmgsdi_cnf.response_header.status_word.sw1     = status_word.sw1;
  cnf_ptr->mmgsdi_cnf.response_header.status_word.sw2     = status_word.sw2;

  /* response_type is being populated in the build_and_queue function
     before the populate header is being called.
     This is done only for SESSION OPEN requests to correctly populate
     the session id. */
  switch(cnf_ptr->mmgsdi_cnf.response_header.response_type)
  {
    case MMGSDI_SESSION_OPEN_REQ:
      cnf_ptr->mmgsdi_cnf.session_open_cnf.response_header.session_id =
        cnf_ptr->mmgsdi_cnf.session_open_cnf.session_id;
      break;

    case MMGSDI_SESSION_OPEN_EXT_REQ:
      cnf_ptr->mmgsdi_cnf.session_open_ext_cnf.response_header.session_id =
        0;
      cnf_ptr->mmgsdi_cnf.response_header.slot_id =
        MMGSDI_MAX_SLOT_ID_ENUM;
      break;

    case MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ:
      cnf_ptr->mmgsdi_cnf.session_open_with_sel_rsp_cnf.response_header.session_id =
        cnf_ptr->mmgsdi_cnf.session_open_with_sel_rsp_cnf.session_id;
      break;

    case MMGSDI_SESSION_OPEN_WITH_MF_REQ:
      cnf_ptr->mmgsdi_cnf.session_open_with_mf_cnf.response_header.session_id =
        cnf_ptr->mmgsdi_cnf.session_open_with_mf_cnf.session_id;
      break;

    case MMGSDI_SAP_CARD_READER_STATUS_REQ:
    case MMGSDI_RESTRICTED_SIM_ACCESS_REQ:
    case MMGSDI_MAX_CNF_ENUM:
      UIM_MSG_ERR_1("Incorrect cnf enum type 0x%x",
                  cnf_ptr->mmgsdi_cnf.response_header.response_type);
      return;
    default:
      break;
  }
} /* mmgsdi_cnf_populate_cnf_rsp_header */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SESSION_DEACTIVATE_DATA

   DESCRIPTION:
     This function builds the deactivate confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_cnf_build_session_deactivate_data(
  mmgsdi_session_deactivate_cnf_type             *deactivate_cnf_ptr,
  mmgsdi_session_id_type                          session_id,
  const mmgsdi_session_cnf_add_info_type         *add_info_ptr)
{
  MMGSDIUTIL_RETURN_IF_NULL(deactivate_cnf_ptr);

  deactivate_cnf_ptr->session_id = session_id;
  if(add_info_ptr != NULL)
  {
    mmgsdi_memscpy((void *)&deactivate_cnf_ptr->app_id,
                   sizeof(deactivate_cnf_ptr->app_id),
                   (void *)&add_info_ptr->app_info,
                   sizeof(mmgsdi_aid_type));
  }

  return MMGSDI_SUCCESS;
}/* mmgsdi_cnf_build_session_deactivate_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SESSION_APP_RESET_DEACTIVATE_DATA

   DESCRIPTION:
     This function builds the deactivate confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_cnf_build_session_app_reset_deactivate_data(
  mmgsdi_session_app_reset_deactivate_cnf_type   *deactivate_cnf_ptr,
  mmgsdi_session_id_type                          session_id,
  const mmgsdi_session_cnf_add_info_type         *add_info_ptr)
{
  MMGSDIUTIL_RETURN_IF_NULL(deactivate_cnf_ptr);

  deactivate_cnf_ptr->session_id = session_id;
  if(add_info_ptr != NULL)
  {
    mmgsdi_memscpy((void *)&deactivate_cnf_ptr->app_id,
                   sizeof(deactivate_cnf_ptr->app_id),
                   (void *)&add_info_ptr->app_info,
                   sizeof(mmgsdi_aid_type));
  }

  return MMGSDI_SUCCESS;
}/* mmgsdi_cnf_build_session_app_reset_deactivate_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SESSION_SELECT_AID_DATA

   DESCRIPTION:
     This function builds the session select aid confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_cnf_build_session_select_aid_data(
  mmgsdi_session_select_aid_cnf_type             *select_aid_cnf_ptr,
  mmgsdi_session_id_type                          session_id,
  mmgsdi_return_enum_type                         mmgsdi_status,
  const mmgsdi_session_cnf_add_info_type         *add_info_ptr)
{
  MMGSDIUTIL_RETURN_IF_NULL_2(select_aid_cnf_ptr, add_info_ptr);

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    select_aid_cnf_ptr->session_id = session_id;
    mmgsdi_memscpy((void *)&select_aid_cnf_ptr->app_id,
                   sizeof(select_aid_cnf_ptr->app_id),
                   (void *)&add_info_ptr->app_info,
                   sizeof(mmgsdi_aid_type));

    if (add_info_ptr->select_rsp_requested)
    {
      select_aid_cnf_ptr->select_rsp_type = add_info_ptr->select_rsp_type;

      if (add_info_ptr->rsp_data.data_len > 0)
      {
        select_aid_cnf_ptr->select_rsp.data_len = add_info_ptr->rsp_data.data_len;
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(select_aid_cnf_ptr->select_rsp.data_ptr,
                                           select_aid_cnf_ptr->select_rsp.data_len);
        if (select_aid_cnf_ptr->select_rsp.data_ptr == NULL)
        {
          return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        }
        mmgsdi_memscpy(select_aid_cnf_ptr->select_rsp.data_ptr,
                       int32touint32(select_aid_cnf_ptr->select_rsp.data_len),
                       add_info_ptr->rsp_data.data_ptr,
                       int32touint32(add_info_ptr->rsp_data.data_len));
      }
    }
  }

  return MMGSDI_SUCCESS;
}/* mmgsdi_cnf_build_session_select_aid_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_REHABILITATE_DATA

   DESCRIPTION:
     This function build the rehabilitate confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_rehabilitate_data(
  mmgsdi_rehabilitate_cnf_type                     * rehabilitate_cnf_ptr,
  const mmgsdi_client_req_extra_info_type          * extra_param_ptr
)
{
  mmgsdi_return_enum_type build_cnf_status = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL_2(extra_param_ptr, rehabilitate_cnf_ptr);

    /*---------------------------------------------------------------------------
    Copy the access data info from request pointer to confirmation pointer
  ---------------------------------------------------------------------------*/
  build_cnf_status = mmgsdi_util_copy_access_type(&rehabilitate_cnf_ptr->access,
    (mmgsdi_access_type*)&extra_param_ptr->rehab_data.access);

  return build_cnf_status;
}/* mmgsdi_cnf_build_rehabilitate_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_INVALIDATE_DATA

   DESCRIPTION:
     This function build the invalidate confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_cnf_build_invalidate_data(
  mmgsdi_invalidate_cnf_type                     * invalidate_cnf_ptr,
  const mmgsdi_client_req_extra_info_type        * extra_param_ptr
)
{
  mmgsdi_return_enum_type build_cnf_status = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL_2(extra_param_ptr, invalidate_cnf_ptr);

    /*---------------------------------------------------------------------------
    Copy the access data info from request pointer to confirmation pointer
  ---------------------------------------------------------------------------*/
  build_cnf_status = mmgsdi_util_copy_access_type(&invalidate_cnf_ptr->access,
    (mmgsdi_access_type*)&extra_param_ptr->invalidate_data.access);

  return build_cnf_status;
}/* mmgsdi_cnf_build_invalidate_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_AVAILABLE_APPS_DATA

   DESCRIPTION:
     This function builds the available apps data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_cnf_build_available_apps_data(
  mmgsdi_get_all_available_apps_cnf_type   *available_apps_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status
)
{
  mmgsdi_available_apps_type    *available_apps_ptr = NULL;

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Do nothing if status is not success */
    return MMGSDI_SUCCESS;
  }

  MMGSDIUTIL_RETURN_IF_NULL(available_apps_cnf_ptr);

  /* Allocate memory for available apps pointer */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(available_apps_ptr,
    sizeof(mmgsdi_available_apps_type));

  if(available_apps_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_all_available_apps(available_apps_ptr);
  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* Copy data from available apps cnf to available apps */
    available_apps_cnf_ptr->card1_num_app = available_apps_ptr->card1_num_app;
    available_apps_cnf_ptr->card2_num_app = available_apps_ptr->card2_num_app;
    available_apps_cnf_ptr->card3_num_app = available_apps_ptr->card3_num_app;

    mmgsdi_memscpy(available_apps_cnf_ptr->card1_app_info,
                   sizeof(available_apps_cnf_ptr->card1_app_info),
                   available_apps_ptr->card1_app_info,
                   sizeof(available_apps_ptr->card1_app_info));

    mmgsdi_memscpy(available_apps_cnf_ptr->card2_app_info,
                   sizeof(available_apps_cnf_ptr->card2_app_info),
                   available_apps_ptr->card2_app_info,
                   sizeof(available_apps_ptr->card2_app_info));

    mmgsdi_memscpy(available_apps_cnf_ptr->card3_app_info,
                   sizeof(available_apps_cnf_ptr->card3_app_info),
                   available_apps_ptr->card3_app_info,
                   sizeof(available_apps_ptr->card3_app_info));
  }
  /* Free available apps info */
  MMGSDIUTIL_TMC_MEM_FREE(available_apps_ptr);

  return mmgsdi_status;
} /* mmgsdi_cnf_build_available_apps_data */


/* =============================================================================
   FUNCTION:      MMGSDI_BUILD_WRITE_FDN_EVENT

   DESCRIPTION:
     This function builds FDN event for every successful handling of FDN
     write record request

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
=============================================================================*/
void mmgsdi_build_write_fdn_event(
  mmgsdi_return_enum_type             mmgsdi_status,
  const mmgsdi_write_extra_info_type *write_extra_param_ptr,
  mmgsdi_session_id_type              session_id)
{
  mmgsdi_int_app_info_type        *sel_app_ptr      = NULL;
  mmgsdi_evt_session_notify_type   notify_type;
  mmgsdi_slot_id_enum_type         slot_id          = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_session_info_type        *session_info_ptr = NULL;
  boolean                          fdn_enabled      = FALSE;

  if (write_extra_param_ptr == NULL)
  {
    return;
  }

  memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    if((write_extra_param_ptr->access.access_method == MMGSDI_EF_ENUM_ACCESS) &&
       (write_extra_param_ptr->access.file.file_enum == MMGSDI_USIM_FDN ||
        write_extra_param_ptr->access.file.file_enum == MMGSDI_CSIM_FDN ||
        write_extra_param_ptr->access.file.file_enum == MMGSDI_TELECOM_FDN))
    {
      /* If this is a write record request for FDN EFs, trigger a FDN event
             with record num and record data details in the event */
      mmgsdi_status = mmgsdi_util_get_session_app_info(
                        session_id,
                        &slot_id, NULL,
                        &sel_app_ptr,NULL,&session_info_ptr,NULL);
      if((mmgsdi_status != MMGSDI_SUCCESS) ||
         (session_info_ptr == NULL) ||
         (sel_app_ptr == NULL))
      {
        UIM_MSG_ERR_1("Invalid session id 0x%x", session_id);
        return;
      }
      if(sel_app_ptr->app_capabilities.fdn_state == MMGSDI_SRV_ENABLE)
      {
        fdn_enabled = TRUE;
      }
      else
      {
        fdn_enabled = FALSE;
      }

      if(mmgsdi_util_is_card_session(session_info_ptr->session_type))
      {
        notify_type.notify_type = MMGSDI_EVT_NOTIFY_CARD_SESSION;
      }
      else
      {
        notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
      }
      notify_type.slot_id     = slot_id;
      notify_type.session_id  = session_id;
      mmgsdi_evt_build_and_send_fdn(
        notify_type, fdn_enabled,
        (const mmgsdi_int_app_info_type*)sel_app_ptr,
        write_extra_param_ptr->rec_num,
        &(write_extra_param_ptr->data));
    }
  }
} /* mmgsdi_build_write_fdn_event */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_OPEN_WITH_SELECT_RSP_DATA

   DESCRIPTION:
     This function builds the Session open with Select Rsp confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_cnf_build_open_with_select_rsp_data(
  mmgsdi_session_open_with_sel_rsp_cnf_type  * open_cnf_ptr,
  mmgsdi_return_enum_type                      mmgsdi_status,
  mmgsdi_session_id_type                       session_id,
  const mmgsdi_session_cnf_add_info_type     * session_data_ptr)
{
  mmgsdi_return_enum_type       build_cnf_status = MMGSDI_SUCCESS;
  mmgsdi_session_info_type     *session_info_ptr = NULL;
  int32                         session_index    = 0;
  int32                         client_index     = 0;

  MMGSDIUTIL_RETURN_IF_NULL(open_cnf_ptr);

  /* Set default values in CNF if Session Open is not successful */
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    open_cnf_ptr->session_id = MMGSDI_SESSION_ID_ZERO;
    open_cnf_ptr->channel_id = 0x0;
    open_cnf_ptr->slot_id = MMGSDI_MAX_SLOT_ID_ENUM;
    return MMGSDI_SUCCESS;
  }

  open_cnf_ptr->session_id  = session_id;
  open_cnf_ptr->slot_id     = MMGSDI_MAX_SLOT_ID_ENUM;

  if(session_data_ptr != NULL)
  {
    mmgsdi_memscpy((void*)&open_cnf_ptr->app_info,
                   sizeof(open_cnf_ptr->app_info),
                   (void*)&session_data_ptr->app_info,
                   sizeof(mmgsdi_aid_type));

    open_cnf_ptr->select_rsp_type = session_data_ptr->select_rsp_type;

    if (session_data_ptr->rsp_data.data_ptr != NULL)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(open_cnf_ptr->select_rsp.data_ptr,
        (uint32)session_data_ptr->rsp_data.data_len);
      if (open_cnf_ptr->select_rsp.data_ptr == NULL)
      {
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
      mmgsdi_memscpy((void*)open_cnf_ptr->select_rsp.data_ptr,
                     (uint32)session_data_ptr->rsp_data.data_len,
                     (void*)session_data_ptr->rsp_data.data_ptr,
                     (uint32)session_data_ptr->rsp_data.data_len);
      open_cnf_ptr->select_rsp.data_len = session_data_ptr->rsp_data.data_len;
    }
  }

  build_cnf_status = mmgsdi_util_get_client_and_session_index(session_id,
                                                              &client_index,
                                                              &session_index);

  if(build_cnf_status != MMGSDI_SUCCESS)
  {
    return build_cnf_status;
  }
  /* mmgsdi_util_get_client_and_session_index success guarantees client_index and
     session_index are valid indexes for dereferencing */
  session_info_ptr =
    mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];

  MMGSDIUTIL_RETURN_STATUS_IF_NULL(session_info_ptr,MMGSDI_ERROR);

  switch(session_info_ptr->session_type)
  {
    case MMGSDI_NON_PROV_SESSION_SLOT_1:
    case MMGSDI_NON_PROV_SESSION_SLOT_2:
    case MMGSDI_NON_PROV_SESSION_SLOT_3:
      if((session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
         (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL))
      {
        open_cnf_ptr->channel_id =
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_id;
        open_cnf_ptr->slot_id =
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id;
      }
      break;
    default:
      build_cnf_status = MMGSDI_ERROR;
      break;
  }

  return build_cnf_status;
} /* mmgsdi_cnf_build_open_with_select_rsp_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_OPEN_WITH_MF_DATA

   DESCRIPTION:
     This function builds the the Session open with Select Rsp confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_cnf_build_open_with_mf_data(
  mmgsdi_session_open_with_mf_cnf_type * open_with_mf_cnf_ptr,
  mmgsdi_return_enum_type                mmgsdi_status,
  mmgsdi_session_id_type                 session_id)
{
  mmgsdi_return_enum_type       build_cnf_status = MMGSDI_SUCCESS;
  mmgsdi_session_info_type     *session_info_ptr = NULL;
  int32                         session_index    = 0;
  int32                         client_index     = 0;

  MMGSDIUTIL_RETURN_IF_NULL(open_with_mf_cnf_ptr);

  /* Set default values in CNF if Session Open is not successful */
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    open_with_mf_cnf_ptr->session_id = MMGSDI_SESSION_ID_ZERO;
    open_with_mf_cnf_ptr->channel_id = 0x0;
    open_with_mf_cnf_ptr->slot_id = MMGSDI_MAX_SLOT_ID_ENUM;
    return MMGSDI_SUCCESS;
  }

  open_with_mf_cnf_ptr->session_id = session_id;
  open_with_mf_cnf_ptr->slot_id = MMGSDI_MAX_SLOT_ID_ENUM;

  build_cnf_status = mmgsdi_util_get_client_and_session_index(session_id,
                                                              &client_index,
                                                              &session_index);

  if(build_cnf_status != MMGSDI_SUCCESS)
  {
    return build_cnf_status;
  }
  /* mmgsdi_util_get_client_and_session_index success guarantees client_index and
     session_index are valid indexes for dereferencing */
  session_info_ptr =
    mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];

  MMGSDIUTIL_RETURN_STATUS_IF_NULL(session_info_ptr,MMGSDI_ERROR);

  switch(session_info_ptr->session_type)
  {
    case MMGSDI_NON_PROV_SESSION_SLOT_1:
    case MMGSDI_NON_PROV_SESSION_SLOT_2:
    case MMGSDI_NON_PROV_SESSION_SLOT_3:
      if((session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
         (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL))
      {
        open_with_mf_cnf_ptr->channel_id =
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_id;
        open_with_mf_cnf_ptr->slot_id =
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id;
      }
      break;
    default:
      build_cnf_status = MMGSDI_ERROR;
      break;
  }

  return build_cnf_status;
} /* mmgsdi_cnf_build_open_with_mf_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SEND_APDU_EXT_DATA

   DESCRIPTION:
     This function builds the send APDU extension confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_cnf_build_send_apdu_ext_data(
  mmgsdi_send_apdu_ext_cnf_type           * apdu_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const mmgsdi_data_type                  * uim_rsp_data_ptr)
{
  uint32                   apdu_len         = 0;

  MMGSDIUTIL_RETURN_IF_NULL_3(apdu_cnf_ptr, extra_param_ptr, uim_rsp_data_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    apdu_len =  int32touint32(uim_rsp_data_ptr->data_len);

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(apdu_cnf_ptr->apdu_data.data_ptr,
      apdu_len);

    if (apdu_cnf_ptr->apdu_data.data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    mmgsdi_memscpy(apdu_cnf_ptr->apdu_data.data_ptr,
                   apdu_len,
                   uim_rsp_data_ptr->data_ptr,
                   apdu_len);

    apdu_cnf_ptr->apdu_data.data_len = uim_rsp_data_ptr->data_len - 2;
  }
  apdu_cnf_ptr->apdu_option =
    extra_param_ptr->send_apdu_data.apdu_option;

  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_send_apdu_ext_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_AND_QUEUE

   DESCRIPTION:
     This function build the task command for confirmation

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_and_queue(
  mmgsdi_return_enum_type             mmgsdi_status,
  const mmgsdi_request_header_type  * req_header_ptr,
  mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const void                        * add_data_ptr,
  boolean                             sw_present,
  mmgsdi_sw_status_type               status_word
)
{
  mmgsdi_task_cmd_type    *task_cmd_ptr        = NULL;
  int32                    mem_size            = 0;
  mmgsdi_return_enum_type  build_status        = MMGSDI_SUCCESS;
  uint32                   task_cmd_len        = 0;
  mmgsdi_task_enum_type    mmgsdi_task         = MMGSDI_TASK_MAX;
  mmgsdi_slot_data_type   *slot_data_ptr       = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(req_header_ptr);

  UIM_MSG_HIGH_4("In mmgsdi_cnf_build_and_queue, original request type = 0x%x,request type = 0x%x,"
                 "slot id = 0x%x, status = 0x%x",
                 req_header_ptr->orig_request_type,
                 req_header_ptr->request_type,
                 req_header_ptr->slot_id,
                 mmgsdi_status);

  /* It is possible that extra param ptr or add_data_ptr are NULL for various command */

  /*---------------------------------------------------------------------------
    - Calculate the total memory size
    - malloc the total memory size
  ---------------------------------------------------------------------------*/
  mmgsdi_util_cal_align_size(sizeof(mmgsdi_task_cmd_type),
                               &mem_size);

  task_cmd_len = sizeof(mmgsdi_callback_cnf_type) + int32touint32(mem_size);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  mmgsdi_util_align_mem(&task_cmd_ptr->cmd,
                          MMGSDI_CNF);

  task_cmd_ptr->cmd.cmd_enum = MMGSDI_CNF;

  if(task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
    return MMGSDI_ERROR;
  }

  task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.response_header.response_type =
                     req_header_ptr->orig_request_type;

  switch(task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.response_header.response_type)
  {
    case MMGSDI_WRITE_REQ:
    if (extra_param_ptr == NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
      return MMGSDI_ERROR;
    }
    build_status = mmgsdi_cnf_build_write_data(
      &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.write_cnf,
      mmgsdi_status, extra_param_ptr);

    mmgsdi_build_write_fdn_event(mmgsdi_status,&(extra_param_ptr->write_data),
                                 req_header_ptr->session_id);
    mmgsdi_build_file_update_event(mmgsdi_status,&(extra_param_ptr->write_data),
                                   req_header_ptr->session_id,
                                   req_header_ptr->slot_id);
    break;

    case MMGSDI_READ_REQ:
      build_status = mmgsdi_cnf_build_read_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.read_cnf,
        mmgsdi_status, extra_param_ptr, (mmgsdi_data_type*)add_data_ptr);
      break;

    case MMGSDI_INCREASE_REQ:
      build_status = mmgsdi_cnf_build_increase_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.increase_cnf,
        mmgsdi_status, extra_param_ptr, (mmgsdi_data_type*)add_data_ptr);
      break;

    case MMGSDI_GET_FILE_ATTR_REQ:
      build_status = mmgsdi_cnf_build_get_file_attr_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.get_file_attr_cnf,
        mmgsdi_status, extra_param_ptr, req_header_ptr, (mmgsdi_data_type*)add_data_ptr);
      break;

    case MMGSDI_SESSION_CLOSE_REQ:
      build_status = mmgsdi_cnf_build_close_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_close_cnf,
        mmgsdi_status, req_header_ptr->session_id,
        (mmgsdi_session_cnf_add_info_type*)add_data_ptr);
      break;

    case MMGSDI_SESSION_OPEN_REQ:
      build_status = mmgsdi_cnf_build_open_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_open_cnf,
        mmgsdi_status, req_header_ptr->session_id,
        (mmgsdi_session_cnf_add_info_type*)add_data_ptr);
      break;

    case MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ:
      build_status = mmgsdi_cnf_build_open_with_select_rsp_data(
          &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_open_with_sel_rsp_cnf,
          mmgsdi_status, req_header_ptr->session_id,
          (mmgsdi_session_cnf_add_info_type *)add_data_ptr);
      break;
    case MMGSDI_SESSION_OPEN_EXT_REQ:
      build_status = mmgsdi_cnf_build_open_ext_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_open_ext_cnf,
        mmgsdi_status,
        (mmgsdi_session_open_ext_cnf_info_type*)add_data_ptr);
      break;

    case MMGSDI_SESSION_OPEN_WITH_MF_REQ:
      build_status = mmgsdi_cnf_build_open_with_mf_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_open_with_mf_cnf,
        mmgsdi_status, req_header_ptr->session_id);
      break;

    case MMGSDI_SESSION_DEACTIVATE_REQ:
      build_status = mmgsdi_cnf_build_session_deactivate_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_deactivate_cnf,
        req_header_ptr->session_id,
        (mmgsdi_session_cnf_add_info_type*)add_data_ptr);
      break;

    case MMGSDI_SESSION_APP_RESET_DEACTIVATE_REQ:
      build_status = mmgsdi_cnf_build_session_app_reset_deactivate_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_app_reset_deactivate_cnf,
        req_header_ptr->session_id,
        (mmgsdi_session_cnf_add_info_type*)add_data_ptr);
      break;

    case MMGSDI_SESSION_APP_RESET_ACTIVATE_REQ:
      break;

    case MMGSDI_SESSION_SELECT_AID_REQ:
      build_status = mmgsdi_cnf_build_session_select_aid_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_select_aid_cnf,
        req_header_ptr->session_id,
        mmgsdi_status,
        (mmgsdi_session_cnf_add_info_type*)add_data_ptr);
      break;

    case MMGSDI_ISIM_AUTH_REQ:
      build_status = mmgsdi_cnf_build_isim_auth_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.isim_auth_cnf,
        mmgsdi_status, (mmgsdi_isim_auth_data_type*)add_data_ptr);
      break;

    case MMGSDI_SEND_APDU_REQ:
      build_status = mmgsdi_cnf_build_send_apdu_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.send_apdu_cnf,
        mmgsdi_status, extra_param_ptr, (mmgsdi_data_type*)add_data_ptr,
        req_header_ptr->slot_id);
      break;

    case MMGSDI_SEND_APDU_EXT_REQ:
      build_status = mmgsdi_cnf_build_send_apdu_ext_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.send_apdu_ext_cnf,
        mmgsdi_status, extra_param_ptr, (mmgsdi_data_type*)add_data_ptr);
      break;

    case MMGSDI_SAP_SEND_APDU_REQ:
      if (extra_param_ptr == NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
        return MMGSDI_ERROR;
      }
      build_status = mmgsdi_cnf_build_sap_send_apdu_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.sap_send_apdu_cnf,
        mmgsdi_status, extra_param_ptr, (mmgsdi_data_type*)add_data_ptr);
      break;

    case MMGSDI_SAP_POWER_ON_REQ:
    case MMGSDI_SAP_POWER_OFF_REQ:
    case MMGSDI_SAP_CONNECT_REQ:
      break;

    case MMGSDI_SAP_DISCONNECT_REQ:
      if (extra_param_ptr == NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
        return MMGSDI_ERROR;
      }
      build_status = mmgsdi_cnf_build_sap_disconnect_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.sap_disconnect_cnf,
        mmgsdi_status, extra_param_ptr->sap_disconnect_data.disconnect_mode);
      break;

    case MMGSDI_SAP_GET_ATR_REQ:
      build_status = mmgsdi_cnf_build_sap_get_atr_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.sap_get_atr_cnf,
        mmgsdi_status, req_header_ptr->slot_id);
      break;

    case MMGSDI_SAP_RESET_REQ:
      break;

    case MMGSDI_CARD_PUP_REQ:
      if (extra_param_ptr == NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
        return MMGSDI_ERROR;
      }
      if ((req_header_ptr->orig_request_type == MMGSDI_CARD_PUP_REQ) &&
          (req_header_ptr->request_type == MMGSDI_REFRESH_REQ))
      {
        build_status = mmgsdi_cnf_build_card_pup_data(
          &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.card_pup_cnf,
          mmgsdi_status, MMGSDI_CARD_POWER_UP_INITIAL_PUP);
      }
      else
      {
        build_status = mmgsdi_cnf_build_card_pup_data(
          &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.card_pup_cnf,
          mmgsdi_status, extra_param_ptr->pup_data.option);
      }
      break;

    case MMGSDI_CARD_PDOWN_REQ:
      if (extra_param_ptr == NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
        return MMGSDI_ERROR;
      }
      build_status = mmgsdi_cnf_build_card_pdown_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.card_pdown_cnf,
        mmgsdi_status, extra_param_ptr->pdown_data.option);
      break;

    case MMGSDI_CARD_STATUS_REQ:
      build_status = mmgsdi_cnf_build_status_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.status_cnf,
        mmgsdi_status, (mmgsdi_data_type*)add_data_ptr);
      break;

    case MMGSDI_REFRESH_REQ:
      build_status = mmgsdi_cnf_build_refresh_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.refresh_cnf,
        extra_param_ptr,
        mmgsdi_status);
      break;

    case MMGSDI_ACTIVATE_ONCHIP_SIM_REQ:
      build_status = mmgsdi_cnf_build_onchip_sim_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.act_onchip_cnf,
        mmgsdi_status,
        req_header_ptr->slot_id);
      break;

    case MMGSDI_CLIENT_ID_AND_EVT_REG_REQ:
    case MMGSDI_CLIENT_ID_AND_EVT_DEREG_REQ:
    case MMGSDI_CONTINUE_AFTER_NV_REFRESH_REQ:
    case MMGSDI_HANDLE_NV_REFRESH_REQ:
    case MMGSDI_SET_SIM_PROFILE_REQ:
    case MMGSDI_IMSI_SWITCH_REQ:
      break;

    case MMGSDI_GET_ALL_AVAILABLE_APPS_REQ:
      build_status = mmgsdi_cnf_build_available_apps_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.get_all_available_apps_cnf,
        mmgsdi_status);
      break;

    case MMGSDI_REHABILITATE_REQ:
      build_status = mmgsdi_cnf_build_rehabilitate_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.rehab_cnf,
        extra_param_ptr);
      break;

    case MMGSDI_INVALIDATE_REQ:
      build_status = mmgsdi_cnf_build_invalidate_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.invalidate_cnf,
        extra_param_ptr);
      break;

    case MMGSDI_PIN_OPERATION_REQ:
      build_status = mmgsdi_cnf_build_pin_operation_data(
        req_header_ptr,
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.pin_operation_cnf,
        mmgsdi_status,
        extra_param_ptr);
      break;

    case MMGSDI_GET_ALL_PIN_STATUS_REQ:
      build_status = mmgsdi_cnf_build_get_all_pin_status_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.get_all_pin_status_cnf,
        req_header_ptr,
        mmgsdi_status);
      break;

    case MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ:
     break;

    case MMGSDI_SRV_AVAILABLE_REQ:
      build_status = mmgsdi_cnf_build_srv_available_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.srv_available_cnf,
        &mmgsdi_status,
        extra_param_ptr,
        req_header_ptr);
      break;

    case MMGSDI_USIM_AUTH_REQ:
      build_status = mmgsdi_cnf_build_usim_auth_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.usim_auth_cnf,
        mmgsdi_status,
        (mmgsdi_data_type*)add_data_ptr);
      break;

    case MMGSDI_COMPUTE_IP_AUTH_REQ:
      build_status = mmgsdi_cnf_build_compute_ip_auth_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.compute_ip_cnf,
        mmgsdi_status,
        (mmgsdi_data_type*)add_data_ptr);
      break;

    case MMGSDI_RUN_CAVE_REQ:
#ifdef FEATURE_MMGSDI_3GPP2
      build_status = mmgsdi_cnf_build_run_cave_data(
         &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.run_cave_cnf,
         mmgsdi_status,
         (mmgsdi_data_type*)add_data_ptr);
#endif /* FEATURE_MMGSDI_3GPP2 */
      break;

    case MMGSDI_SESSION_CDMA_AKA_REQ:
#ifdef FEATURE_MMGSDI_3GPP2
      build_status = mmgsdi_cnf_build_cdma_aka_data(
         &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.cdma_aka_cnf,
         mmgsdi_status,
         (mmgsdi_data_type*)add_data_ptr);
#endif /* FEATURE_MMGSDI_3GPP2 */
      break;

    case MMGSDI_GENERATE_KEY_VPM_REQ:
#ifdef FEATURE_MMGSDI_3GPP2
      build_status = mmgsdi_cnf_build_generate_key_vpm_data(
         &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.generate_key_vpm_cnf,
         mmgsdi_status,
         (mmgsdi_data_type*)add_data_ptr);
#endif /* FEATURE_MMGSDI_3GPP2 */
      break;

    case MMGSDI_GET_ATR_REQ:
      build_status = mmgsdi_cnf_build_get_atr_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.get_atr_cnf,
        mmgsdi_status, req_header_ptr->slot_id);
      break;

    case MMGSDI_CARD_RESET_REQ:
      break;

    case MMGSDI_SEARCH_REQ:
      build_status = mmgsdi_cnf_build_search_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.search_cnf,
        mmgsdi_status, extra_param_ptr, (mmgsdi_data_type*)add_data_ptr);
      break;

    case MMGSDI_SESSION_RUN_GSM_ALGO_REQ:
#ifdef FEATURE_MMGSDI_3GPP
      build_status = mmgsdi_cnf_build_run_gsm_algo_data(
                     &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_run_gsm_algo_cnf,
                     mmgsdi_status,
                     (mmgsdi_data_type*)add_data_ptr);
#endif /* FEATURE_MMGSDI_3GPP */
      break;

    case MMGSDI_SESSION_GET_INFO_REQ:
      if(extra_param_ptr != NULL)
      {
        build_status = mmgsdi_cnf_build_session_get_info_data(
                       &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_get_info_cnf,
                       &mmgsdi_status,
                       &(extra_param_ptr->session_get_info_data));
      }
      else
      {
        /* Session Info is not available, so send error status to client */
        mmgsdi_status = MMGSDI_ERROR;
      }
      break;

    case MMGSDI_SESSION_READ_PRL_REQ:
      build_status = mmgsdi_cnf_build_session_read_prl_data(
                     &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_read_prl_cnf,
                     &mmgsdi_status,
                     req_header_ptr->session_id);

      break;

    case MMGSDI_SESSION_ENABLE_SERVICE_REQ:
      if (extra_param_ptr == NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
        return MMGSDI_ERROR;
      }
      build_status = mmgsdi_cnf_build_session_enable_service_data(mmgsdi_status,
                       &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_enable_service_cnf,
                       &(extra_param_ptr->enable_service_data));
      break;

    case MMGSDI_SESSION_DISABLE_SERVICE_REQ:
      if (extra_param_ptr == NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
        return MMGSDI_ERROR;
      }
      build_status = mmgsdi_cnf_build_session_disable_service_data(mmgsdi_status,
                       &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_disable_service_cnf,
                       &(extra_param_ptr->enable_service_data));
      break;

    case MMGSDI_SESSION_GET_CPHS_INFO_REQ:
      build_status = mmgsdi_cnf_build_session_get_cphs_info(
                      req_header_ptr->session_id,
                     &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_get_cphs_info_cnf,
                      mmgsdi_status);
      break;

    case MMGSDI_SESSION_GET_APP_CAPABILITIES_REQ:
      build_status = mmgsdi_cnf_build_session_get_app_capabilities_data(
                      req_header_ptr->slot_id,
                     &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_app_capabilities_cnf,
                      req_header_ptr->session_id);
      break;

    case MMGSDI_SESSION_SSD_CONFIRM_REQ:
      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_ssd_confirm_cnf.ssd_update_ok
          = TRUE;
      }
      else
      {
        task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_ssd_confirm_cnf.ssd_update_ok
          = FALSE;
      }
      break;

    case MMGSDI_SESSION_BS_CHAL_REQ:
      build_status =
      mmgsdi_cnf_build_bs_chal_req_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_bs_chal_cnf,
        mmgsdi_status,
        add_data_ptr);
      break;

    case MMGSDI_SESSION_OTASP_MS_KEY_REQ:
      build_status =
      mmgsdi_cnf_build_otasp_ms_key_req_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_otasp_ms_key_cnf,
        mmgsdi_status,
        add_data_ptr);
      break;

    case MMGSDI_SESSION_OTASP_KEY_GEN_REQ:
      build_status =
      mmgsdi_cnf_build_otasp_key_gen_req_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_otasp_key_gen_cnf,
        mmgsdi_status,
        add_data_ptr);
      break;

    case MMGSDI_SESSION_OTASP_SSPR_CONFIG_REQ:
      build_status =
      mmgsdi_cnf_build_otasp_sspr_config_req_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_otasp_sspr_config_cnf,
        mmgsdi_status,
        add_data_ptr);
      break;

    case MMGSDI_SESSION_OTASP_COMMIT_REQ:
      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        if ((add_data_ptr != NULL) &&
            (((mmgsdi_data_type *)add_data_ptr)->data_ptr != NULL) &&
            (((mmgsdi_data_type *)add_data_ptr)->data_ptr[0] ==
              MMGSDI_OTASP_COMMIT_RSP_SUCCESS))
        {
          slot_data_ptr = mmgsdi_util_get_slot_data_ptr(req_header_ptr->slot_id);
          if(slot_data_ptr == NULL)
          {
            return MMGSDI_ERROR;
          }

          slot_data_ptr->otasp_commit_done = TRUE;
          build_status = MMGSDI_SUCCESS;
        }
        else
        {
          build_status = MMGSDI_ERROR;
        }
      }
      break;

    case MMGSDI_SESSION_OTASP_OTAPA_REQ:
      build_status = mmgsdi_cnf_build_otasp_otapa_req_data(
                       &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_otasp_otapa_cnf,
                       mmgsdi_status,
                       add_data_ptr);
    break;

    case MMGSDI_SESSION_OTASP_BLOCK_REQ:
      build_status =
      mmgsdi_cnf_build_otasp_block_req_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_otasp_block_cnf,
        mmgsdi_status,
        extra_param_ptr,
        add_data_ptr);
      break;

    case MMGSDI_SESSION_SUB_OK_REQ:
      UIM_MSG_HIGH_1 ("MMGSDI_CNF_BUILD_SESSION_SUBSCRIPTION_OK, status = 0x%x",
                      mmgsdi_status);
      break;

    case MMGSDI_SESSION_GET_OPERATOR_NAME_REQ:
      build_status =
        mmgsdi_cnf_build_get_eons_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_get_operator_name_cnf,
        mmgsdi_status,
        extra_param_ptr,
        req_header_ptr->session_id);
      break;

    case MMGSDI_GSTK_LOC_ENV_RSP_RECEIVED_REQ:
      UIM_MSG_HIGH_1("MMGSDI_CNF_BUILD_GSTK_LOC_ENV_RSP_RECEIVED, status = 0x%x",
                     mmgsdi_status);
      build_status = mmgsdi_cnf_build_gstk_loc_env_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.gstk_loc_env_rsp_received_cnf,
        extra_param_ptr);
      break;

    case MMGSDI_GET_SE13_PLMN_NAMES_REQ:
      build_status =
        mmgsdi_cnf_build_get_se13_plmn_names_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.get_se13_plmn_names_cnf,
        mmgsdi_status,
        extra_param_ptr);
        break;

    case MMGSDI_SESSION_UPDATE_ACSGL_RECORD_REQ:
      if (extra_param_ptr == NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
        return MMGSDI_ERROR;
      }
      build_status = mmgsdi_cnf_build_session_update_acsgl_data(
                     &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.session_update_acsgl_record_cnf,
                     &(extra_param_ptr->update_acsgl_data));
      break;

    case MMGSDI_GET_SE13_PLMN_INFO_BY_NAME_REQ:
      build_status =
        mmgsdi_cnf_build_get_se13_plmn_info_by_name_data(
        &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.get_se13_plmn_info_by_name_cnf,
        (mmgsdi_get_se13_plmn_info_by_name_req_type *)add_data_ptr);
      break;

    case MMGSDI_SET_BUILTIN_PLMN_LIST_REQ:
      build_status =
        mmgsdi_cnf_build_set_builtin_plmn_list_data(
        (mmgsdi_set_builtin_plmn_list_req_type *)add_data_ptr);
      break;

    case MMGSDI_MANAGE_CHANNEL_REQ:
      UIM_MSG_MED_1("MMGSDI_MANAGE_CHANNEL_CNF, status = 0x%x", mmgsdi_status);
      if (MMGSDI_SUCCESS == mmgsdi_status)
      {
        if (add_data_ptr                                 &&
            ((mmgsdi_data_type *)add_data_ptr)->data_ptr &&
            ((mmgsdi_data_type *)add_data_ptr)->data_len > 0)
        {
          task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.manage_channel_cnf.channel_id
            = ((mmgsdi_data_type *)add_data_ptr)->data_ptr[0];
        }
        else
        {
          build_status = MMGSDI_ERROR;
        }
      }
      break;

    case MMGSDI_GET_SIM_PROFILE_REQ:
#ifdef FEATURE_UIM_MULTI_PROFILE
      build_status = mmgsdi_cnf_build_get_sim_profile_data(
                       &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.get_sim_profile_cnf,
                       mmgsdi_status);
#endif /* FEATURE_UIM_MULTI_PROFILE */
      break;

    case MMGSDI_GET_M2M_EID_REQ:
      build_status = mmgsdi_cnf_build_get_m2m_eid_data(
                       &task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr->mmgsdi_cnf.get_m2m_eid_cnf,
                       mmgsdi_status,
		       (mmgsdi_send_apdu_data_type *)add_data_ptr);
    break;

    case MMGSDI_SESSION_STORE_ESN_REQ:
    case MMGSDI_SESSION_SSD_UPDATE_REQ:
    case MMGSDI_SESSION_REGISTER_FILE_UPDATE_REQ:
      break;

    case MMGSDI_SAP_CARD_READER_STATUS_REQ:
    case MMGSDI_RESTRICTED_SIM_ACCESS_REQ:
    default:
      MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
      return MMGSDI_ERROR;
  }

  if (build_status == MMGSDI_SUCCESS)
  {
    /* Populate Header */
    /* Header population should be done AFTER data populate
       because any pointer length will be assigned in the
       individual confirmation population and used in the
       populate_cnf_rsp_header for assigning the response
       length */

    mmgsdi_cnf_populate_cnf_rsp_header(
      task_cmd_ptr->cmd.cmd.mmgsdi_cnf_ptr,
      req_header_ptr,
      mmgsdi_status,
      sw_present,
      status_word);

    MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
    if(mmgsdi_task == MMGSDI_TASK_MAX)
    {
      /* In case the current task is undetermined,
         queue to main task */
      mmgsdi_task = MMGSDI_TASK_MAIN;
    }

    (void)q_link(task_cmd_ptr, &task_cmd_ptr->link);
    /* Put the message on the queue */
    q_put(&mmgsdi_task_rsp_q[mmgsdi_task], &task_cmd_ptr->link);

    /* Set the command queue signal for */
    /* receiving task                     */
    /* In case current task is non-MMGSDI,
       the response is queued to main task. */
    if(mmgsdi_task == MMGSDI_TASK_MAIN)
    {
      (void) rex_set_sigs(UIM_GSDI_TCB, MMGSDI_TASK_RSP_Q_SIG);
    }
    else
    {
      (void) rex_set_sigs(rex_self(), MMGSDI_TASK_RSP_Q_SIG);
    }
  }
  else
  {
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  if (build_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_2("mmgsdi_cnf_build_and queue, CNF Build Status = 0x%x, Task 0x%x",
                   build_status, mmgsdi_task);
  }

  return build_status;
} /* mmgsdi_cnf_build_and_queue */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_WRITE_DATA

   DESCRIPTION:
     This function build the write confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_write_data(
  mmgsdi_write_cnf_type                   * write_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr
)
{
  mmgsdi_return_enum_type  build_cnf_status    = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL_2(extra_param_ptr, write_cnf_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    write_cnf_ptr->accessed_rec_num = extra_param_ptr->write_data.rec_num;
    write_cnf_ptr->accessed_offset  = extra_param_ptr->write_data.offset;
    write_cnf_ptr->written_data_len = extra_param_ptr->write_data.data.data_len;
  }

  /*---------------------------------------------------------------------------
    Copy the access data info from request pointer to confirmation pointer
  ---------------------------------------------------------------------------*/
  build_cnf_status = mmgsdi_util_copy_access_type(
    &write_cnf_ptr->access,
    &extra_param_ptr->write_data.access);

  return build_cnf_status;
} /* mmgsdi_cnf_build_write_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_READ_DATA

   DESCRIPTION:
     This function build the read confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_read_data(
  mmgsdi_read_cnf_type                    * read_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const mmgsdi_data_type                  * data_read_ptr
)
{
  mmgsdi_return_enum_type  build_cnf_status    = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL_3(extra_param_ptr, read_cnf_ptr, data_read_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    read_cnf_ptr->accessed_rec_num   = extra_param_ptr->read_data.rec_num;
    read_cnf_ptr->accessed_offset    = extra_param_ptr->read_data.offset;
    read_cnf_ptr->read_data.data_len = extra_param_ptr->read_data.data_len;

    if(read_cnf_ptr->read_data.data_len <= 0 ||
       read_cnf_ptr->read_data.data_len > data_read_ptr->data_len)
    {
      read_cnf_ptr->read_data.data_len = data_read_ptr->data_len;
    }

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(read_cnf_ptr->read_data.data_ptr,
                                       read_cnf_ptr->read_data.data_len);

    /* even if can't malloc for read data, still want to populate
       the cnf header to notify client */
    if (read_cnf_ptr->read_data.data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    mmgsdi_memscpy(read_cnf_ptr->read_data.data_ptr,
                   int32touint32(read_cnf_ptr->read_data.data_len),
                   data_read_ptr->data_ptr,
                   int32touint32(read_cnf_ptr->read_data.data_len));

  }

  /*---------------------------------------------------------------------------
    Copy the access data info from request pointer to confirmation pointer
  ---------------------------------------------------------------------------*/
  build_cnf_status = mmgsdi_util_copy_access_type(
    &read_cnf_ptr->access,
    &extra_param_ptr->read_data.access);

  if (build_cnf_status != MMGSDI_SUCCESS)
  {
    /* Free the read data */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr->read_data.data_ptr);
  }

  return build_cnf_status;
} /* mmgsdi_cnf_build_read_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_INCREASE_DATA

   DESCRIPTION:
     This function build the increase confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_increase_data(
  mmgsdi_increase_cnf_type                * increase_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const mmgsdi_data_type                  * inc_data_ptr
)
{
  mmgsdi_return_enum_type  build_cnf_status    = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL_3(extra_param_ptr, increase_cnf_ptr, inc_data_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    increase_cnf_ptr->increased_data.data_len = inc_data_ptr->data_len;

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      increase_cnf_ptr->increased_data.data_ptr,
      increase_cnf_ptr->increased_data.data_len);

    if (increase_cnf_ptr->increased_data.data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    /* Populate confirmation with new value of the incremented record*/
    mmgsdi_memscpy(increase_cnf_ptr->increased_data.data_ptr,
                   int32touint32(increase_cnf_ptr->increased_data.data_len),
                   inc_data_ptr->data_ptr,
                   int32touint32(increase_cnf_ptr->increased_data.data_len));
  }

  /*---------------------------------------------------------------------------
    Copy the access data info from request pointer to confirmation pointer
  ---------------------------------------------------------------------------*/
  build_cnf_status = mmgsdi_util_copy_access_type(
    &increase_cnf_ptr->access,
    &extra_param_ptr->increase_data.access);

  if (build_cnf_status != MMGSDI_SUCCESS)
  {
    /* Free the increased data */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(increase_cnf_ptr->increased_data.data_ptr);
  }

  return build_cnf_status;
} /* mmgsdi_cnf_build_increase_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_FILE_ATTR_DATA

   DESCRIPTION:
     This function build the get file attribute confirmation's specific data. If
     the cnf status is SUCCESS, it also writes that attributes to the attributes
     cache.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_get_file_attr_data(
  mmgsdi_get_file_attr_cnf_type           * get_file_attr_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const mmgsdi_request_header_type        * req_header_ptr,
  const mmgsdi_data_type                  * data_read_ptr
)
{
  mmgsdi_return_enum_type            build_cnf_status    = MMGSDI_SUCCESS;
  mmgsdi_protocol_enum_type          protocol            = MMGSDI_MAX_PROTOCOL_ENUM;
  mmgsdi_file_security_access_type  *security_access_ptr = NULL;
  boolean                           *increase_ptr        = NULL;
  mmgsdi_file_size_type              file_size           = 0;
  mmgsdi_len_type                    rec_len             = 0;
  mmgsdi_rec_num_type                num_of_rec          = 0;
  mmgsdi_file_structure_enum_type    file_type           = MMGSDI_MAX_FILE_STRUCTURE_ENUM;
  uint8                              sfi                 = MMGSDI_INVALID_SFI;

  MMGSDIUTIL_RETURN_IF_NULL_4(extra_param_ptr,
                              data_read_ptr,
                              req_header_ptr,
                              get_file_attr_cnf_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* Check if UICC or ICC */
    protocol = mmgsdi_util_determine_protocol(req_header_ptr->slot_id);

    if(protocol == MMGSDI_NO_PROTOCOL)
    {
      return MMGSDI_ERROR;
    }

    if(protocol == MMGSDI_UICC)
    {
      /* Extract the important file attributes from the file header */
      mmgsdi_uicc_rsp_decode_file_header(req_header_ptr,
                                         &extra_param_ptr->get_file_attr_data,
                                          data_read_ptr->data_ptr,
                                          data_read_ptr->data_len,
                                          &get_file_attr_cnf_ptr->file_attrib);
    }
    else
    {
      /* Extract the important file attributes from the file header */
      mmgsdi_icc_rsp_extract_file_attr(req_header_ptr->slot_id,
                                      data_read_ptr->data_ptr,
                                      data_read_ptr->data_len,
                                      &get_file_attr_cnf_ptr->file_attrib );
    }
    /*---------------------------------------------------------------------------
      Copy raw file attribute data to confirmation pointer
    ---------------------------------------------------------------------------*/
    if((data_read_ptr->data_ptr != NULL) && (data_read_ptr->data_len > 0))
    {
      get_file_attr_cnf_ptr->raw_file_attr_data.data_len =
                                                data_read_ptr->data_len;

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        get_file_attr_cnf_ptr->raw_file_attr_data.data_ptr,
        get_file_attr_cnf_ptr->raw_file_attr_data.data_len);

      if(get_file_attr_cnf_ptr->raw_file_attr_data.data_ptr == NULL)
      {
        /* No response will be sent hence free all malloc'ed data */
        mmgsdi_uicc_rsp_free_pin_protection_info(
          &(get_file_attr_cnf_ptr->file_attrib));
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }

      mmgsdi_memscpy((void *)get_file_attr_cnf_ptr->raw_file_attr_data.data_ptr,
                     get_file_attr_cnf_ptr->raw_file_attr_data.data_len,
                     (void *)data_read_ptr->data_ptr,
                     int32touint32(get_file_attr_cnf_ptr->raw_file_attr_data.data_len));
    }

    /* Copy the retrieved attributes to the file attributes cache, if not already
       present in file attributes cache */
    file_type = get_file_attr_cnf_ptr->file_attrib.file_type;
    sfi = get_file_attr_cnf_ptr->file_attrib.sfi;
    switch (file_type)
    {
      case MMGSDI_LINEAR_FIXED_FILE:
        security_access_ptr =
          &get_file_attr_cnf_ptr->file_attrib.file_info.linear_fixed_file.file_security;
        num_of_rec =
          get_file_attr_cnf_ptr->file_attrib.file_info.linear_fixed_file.num_of_rec;
        rec_len =
          get_file_attr_cnf_ptr->file_attrib.file_info.linear_fixed_file.rec_len;
        file_size = num_of_rec * rec_len;
        break;
      case MMGSDI_TRANSPARENT_FILE:
        security_access_ptr =
          &get_file_attr_cnf_ptr->file_attrib.file_info.transparent_file.file_security;
        file_size = get_file_attr_cnf_ptr->file_attrib.file_size;
        break;
      case MMGSDI_CYCLIC_FILE:
        security_access_ptr =
          &get_file_attr_cnf_ptr->file_attrib.file_info.cyclic_file.file_security;
        increase_ptr =
          &get_file_attr_cnf_ptr->file_attrib.file_info.cyclic_file.increase_allowed;
        num_of_rec =
          get_file_attr_cnf_ptr->file_attrib.file_info.cyclic_file.num_of_rec;
        rec_len =
          get_file_attr_cnf_ptr->file_attrib.file_info.cyclic_file.rec_len;
        file_size = num_of_rec * rec_len;
        break;
      default:
        build_cnf_status = MMGSDI_ERROR;
    }

    if (build_cnf_status == MMGSDI_SUCCESS)
    {
      (void) mmgsdi_cache_attr_write(req_header_ptr->session_id,
                                     req_header_ptr->slot_id,
                                     &extra_param_ptr->get_file_attr_data.access,
                                     file_type,
                                     file_size,
                                     rec_len,
                                     num_of_rec,
                                     sfi,
                                     security_access_ptr,
                                     increase_ptr);
    }
  }

  /*---------------------------------------------------------------------------
    Copy the access data info from request pointer to confirmation pointer
  ---------------------------------------------------------------------------*/
  build_cnf_status = mmgsdi_util_copy_access_type(&get_file_attr_cnf_ptr->access,
    &extra_param_ptr->get_file_attr_data.access);

  if (build_cnf_status != MMGSDI_SUCCESS)
  {
    /* Free any data */
    MMGSDIUTIL_TMC_MEM_FREE(get_file_attr_cnf_ptr->raw_file_attr_data.data_ptr);
    mmgsdi_uicc_rsp_free_pin_protection_info(&(get_file_attr_cnf_ptr->file_attrib));
    MMGSDIUTIL_DEBUG_MSG_ERROR_1("Copy Access type failed in get file attr 0x%x",
                                 build_cnf_status);
  }
  return build_cnf_status;
} /* mmgsdi_cnf_build_get_file_attr_data */


/* =============================================================================
   FUNCTION:      MMGSDI_SESSION_BUILD_CLOSE_CNF

   DESCRIPTION:
     This function builds the close confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_cnf_build_close_data(
  mmgsdi_session_close_cnf_type           * close_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  mmgsdi_session_id_type                    session_id,
  const mmgsdi_session_cnf_add_info_type  * session_data_ptr)
{
  MMGSDIUTIL_RETURN_IF_NULL(close_cnf_ptr);

  close_cnf_ptr->session_id = session_id;

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    if(session_data_ptr != NULL)
    {
      mmgsdi_memscpy((void *)&close_cnf_ptr->app_info,
                     sizeof(close_cnf_ptr->app_info),
                     (void *)&session_data_ptr->app_info,
                     sizeof(mmgsdi_aid_type));
    }
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_cnf_build_close_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_OPEN_EXT_DATA

   DESCRIPTION:
     This function builds the open ext confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_cnf_build_open_ext_data(
  mmgsdi_session_open_ext_cnf_type              * open_ext_cnf_ptr,
  mmgsdi_return_enum_type                         mmgsdi_status,
  const mmgsdi_session_open_ext_cnf_info_type   * session_data_ptr)
{
  uint8                         num_sessions     = 0;

  MMGSDIUTIL_RETURN_IF_NULL_2(open_ext_cnf_ptr, session_data_ptr);

  num_sessions = session_data_ptr->num_sessions;

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(open_ext_cnf_ptr->session_info_ptr,
      (sizeof(mmgsdi_session_open_info_type)* num_sessions));

    if(open_ext_cnf_ptr->session_info_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    open_ext_cnf_ptr->num_sessions = num_sessions;
    mmgsdi_memscpy((void*)open_ext_cnf_ptr->session_info_ptr,
                   sizeof(mmgsdi_session_open_info_type)* num_sessions,
                   (void*)session_data_ptr->session_info_ptr,
                   sizeof(mmgsdi_session_open_info_type)* num_sessions);
  }

  UIM_MSG_LOW_1("MMGSDI_CNF_BUILD_OPEN_EXT_DATA, number of sessions = 0x%x",
                 num_sessions);

  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_open_ext_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_OPEN_DATA

   DESCRIPTION:
     This function builds the open confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_cnf_build_open_data(
  mmgsdi_session_open_cnf_type             *open_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  mmgsdi_session_id_type                    session_id,
  const mmgsdi_session_cnf_add_info_type   *session_data_ptr)
{
  mmgsdi_return_enum_type       build_cnf_status = MMGSDI_SUCCESS;
  mmgsdi_session_info_type     *session_info_ptr = NULL;
  int32                         session_index    = 0;
  int32                         client_index     = 0;

  MMGSDIUTIL_RETURN_IF_NULL(open_cnf_ptr);

  /* Set default values in CNF if Session Open is not successful */
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    open_cnf_ptr->session_id = MMGSDI_SESSION_ID_ZERO;
    open_cnf_ptr->channel_id = 0x0;
    open_cnf_ptr->slot_id = MMGSDI_MAX_SLOT_ID_ENUM;
    return MMGSDI_SUCCESS;
  }

  open_cnf_ptr->session_id = session_id;
  open_cnf_ptr->slot_id = MMGSDI_MAX_SLOT_ID_ENUM;

  if(session_data_ptr != NULL)
  {
    mmgsdi_memscpy((void*)&open_cnf_ptr->app_info,
                   sizeof(open_cnf_ptr->app_info),
                   (void*)&session_data_ptr->app_info,
                   sizeof(mmgsdi_aid_type));
  }

  build_cnf_status = mmgsdi_util_get_client_and_session_index(session_id,
                                                              &client_index,
                                                              &session_index);

  if(build_cnf_status != MMGSDI_SUCCESS)
  {
    return build_cnf_status;
  }
  /* mmgsdi_util_get_client_and_session_index success guarantees client_index and
     session_index are valid indexes for dereferencing */
  session_info_ptr =
    mmgsdi_client_id_reg_table[client_index]->session_info_ptr[session_index];

  MMGSDIUTIL_RETURN_STATUS_IF_NULL(session_info_ptr,MMGSDI_ERROR);

  if (mmgsdi_util_is_prov_session(session_info_ptr->session_type) ||
      mmgsdi_util_is_non_prov_session(session_info_ptr->session_type))
  {
    if((session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
       (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL))
    {
      open_cnf_ptr->channel_id =
        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_id;
      open_cnf_ptr->slot_id =
        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id;
    }
  }
  else if (mmgsdi_util_is_card_session(session_info_ptr->session_type))
  {
    open_cnf_ptr->channel_id = MMGSDI_DEFAULT_CHANNEL;
  }
  else
  {
    build_cnf_status = MMGSDI_ERROR;
  }
  return build_cnf_status;
} /* mmgsdi_cnf_build_open_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_ISIM_AUTH_DATA

   DESCRIPTION:
     This function builds the ISIM authentication confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_isim_auth_data(
  mmgsdi_isim_auth_cnf_type   *isim_auth_cnf_ptr,
  mmgsdi_return_enum_type      mmgsdi_status,
  const mmgsdi_isim_auth_data_type  *auth_rsp_data_ptr)
{
  uint32                   auth_len            = 0;

  MMGSDIUTIL_RETURN_IF_NULL_2(isim_auth_cnf_ptr, auth_rsp_data_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    /*-------------------------------------------------------------------------
      Authentication response is successful
      - Mem malloc corresponding data pointers for RES, CK and IK
      - Populate the data pointers
      - Error and free if memory allocation failed
    -------------------------------------------------------------------------*/
    auth_len = int32touint32(auth_rsp_data_ptr->res.data_len);
    if (auth_len > 0)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(isim_auth_cnf_ptr->res.data_ptr,
                                         auth_len);
      if (isim_auth_cnf_ptr->res.data_ptr == NULL)
      {
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
      isim_auth_cnf_ptr->res.data_len = auth_len;
      mmgsdi_memscpy(isim_auth_cnf_ptr->res.data_ptr,
                     auth_len,
                     auth_rsp_data_ptr->res.data_ptr,
                     auth_len);
    }

    auth_len = int32touint32(auth_rsp_data_ptr->ck.data_len);
    if (auth_len > 0)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(isim_auth_cnf_ptr->ck.data_ptr,
                                         auth_len);
      if (isim_auth_cnf_ptr->ck.data_ptr == NULL)
      {
        /* Free the auth data */
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(isim_auth_cnf_ptr->res.data_ptr);
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
      isim_auth_cnf_ptr->ck.data_len = auth_len;
      mmgsdi_memscpy(isim_auth_cnf_ptr->ck.data_ptr,
                     auth_len,
                     auth_rsp_data_ptr->ck.data_ptr,
                     auth_len);
    }

    auth_len = int32touint32(auth_rsp_data_ptr->ik.data_len);
    if (auth_len > 0)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(isim_auth_cnf_ptr->ik.data_ptr,
                                         auth_len);
      if (isim_auth_cnf_ptr->ik.data_ptr == NULL)
      {
        /* Free the auth data */
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(isim_auth_cnf_ptr->res.data_ptr);
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(isim_auth_cnf_ptr->ck.data_ptr);
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
      isim_auth_cnf_ptr->ik.data_len = auth_len;
      mmgsdi_memscpy(isim_auth_cnf_ptr->ik.data_ptr,
                     auth_len,
                     auth_rsp_data_ptr->ik.data_ptr,
                     auth_len);
    }
  }
  if (mmgsdi_status == MMGSDI_AUTS_FAIL)
  {
    /*-------------------------------------------------------------------------
      Authentication response is AUTS failure
      - Mem malloc corresponding data pointer for AUTS
      - Populate the data pointer
      - Error and free if memory allocation failed
    -------------------------------------------------------------------------*/
    auth_len = int32touint32(auth_rsp_data_ptr->res.data_len);
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(isim_auth_cnf_ptr->auts.data_ptr,
                                       auth_len);
    if (isim_auth_cnf_ptr->auts.data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    isim_auth_cnf_ptr->auts.data_len =
      auth_rsp_data_ptr->res.data_len;
    mmgsdi_status = MMGSDI_AUTS_FAIL;
    mmgsdi_memscpy(isim_auth_cnf_ptr->auts.data_ptr,
                   auth_len,
                   auth_rsp_data_ptr->res.data_ptr,
                   int32touint32(auth_rsp_data_ptr->res.data_len));
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_isim_auth_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SEND_APDU_DATA

   DESCRIPTION:
     This function builds the send APDU confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_send_apdu_data(
  mmgsdi_send_apdu_cnf_type               * apdu_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const mmgsdi_data_type                  * uim_rsp_data_ptr,
  mmgsdi_slot_id_enum_type                  slot_id)
{
  uint32                   apdu_len            = 0;
  uint8                    slot_index          = 0;

  MMGSDIUTIL_RETURN_IF_NULL_3(apdu_cnf_ptr, extra_param_ptr, uim_rsp_data_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    if (MMGSDI_SUCCESS != mmgsdi_util_get_slot_index(slot_id, &slot_index))
    {
      return MMGSDI_ERROR;
    }

    if ((mmgsdibt_pipe_apdu_ptr[slot_index] != NULL) &&
        (((mmgsdibt_pipe_apdu_ptr[slot_index]->sap_send_apdu_cnf.apdu_data.data_len >= 2) &&
         ((mmgsdibt_pipe_apdu_ptr[slot_index]->sap_send_apdu_cnf.implicit_get_rsp_sw1 ==
             GSDI_SIM_SW1_NORM_END_W_STK_EXTRA_INFO)                           ||
         (mmgsdibt_pipe_apdu_ptr[slot_index]->sap_send_apdu_cnf.implicit_get_rsp_sw1 ==
            GSDI_SIM_SW1_NORM_END_W_SW2_LENGTH)))                             ||
         (mmgsdibt_pipe_apdu_ptr[slot_index]->sap_send_apdu_cnf.implicit_get_rsp_sw1 ==
            GSDI_SW1_WARNINGS_PART_1)))
    {
      apdu_len =  0x02;

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(apdu_cnf_ptr->apdu_data.data_ptr,
                                         apdu_len);

      if (apdu_cnf_ptr->apdu_data.data_ptr == NULL)
      {
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }

      apdu_cnf_ptr->apdu_data.data_len = 0x02;
      apdu_cnf_ptr->apdu_data.data_ptr[0] = uim_rsp_data_ptr->data_ptr[uim_rsp_data_ptr->data_len-2];
      apdu_cnf_ptr->apdu_data.data_ptr[1] = uim_rsp_data_ptr->data_ptr[uim_rsp_data_ptr->data_len-1];
    }
    else
    {
      apdu_len =  int32touint32(uim_rsp_data_ptr->data_len);
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(apdu_cnf_ptr->apdu_data.data_ptr,
                                         apdu_len);

      if (apdu_cnf_ptr->apdu_data.data_ptr == NULL)
      {
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }

      mmgsdi_memscpy(apdu_cnf_ptr->apdu_data.data_ptr,
                     apdu_len,
                     uim_rsp_data_ptr->data_ptr,
                     apdu_len);

      apdu_cnf_ptr->apdu_data.data_len = uim_rsp_data_ptr->data_len - 2;
    }
    apdu_cnf_ptr->implicit_get_rsp_sw1 =
      uim_rsp_data_ptr->data_ptr[uim_rsp_data_ptr->data_len-2];
    apdu_cnf_ptr->implicit_get_rsp_sw2 =
      uim_rsp_data_ptr->data_ptr[uim_rsp_data_ptr->data_len-1];
    apdu_cnf_ptr->channel_id =
      extra_param_ptr->send_apdu_data.channel_id;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_send_apdu_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_CARD_PUP_DATA

   DESCRIPTION:
     This function builds the Card Power Up confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_card_pup_data(
  mmgsdi_card_pup_cnf_type               * card_pup_cnf_ptr,
  mmgsdi_return_enum_type                  mmgsdi_status,
  mmgsdi_card_pup_options_enum_type        pup_options)
{
  MMGSDIUTIL_RETURN_IF_NULL(card_pup_cnf_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    card_pup_cnf_ptr->option = pup_options;
  }

  return MMGSDI_SUCCESS;
}/* mmgsdi_cnf_build_card_pup_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_CARD_PDOWN_DATA

   DESCRIPTION:
     This function builds the Card Power Down confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_card_pdown_data(
  mmgsdi_card_pdown_cnf_type             * card_pdown_cnf_ptr,
  mmgsdi_return_enum_type                  mmgsdi_status,
  mmgsdi_card_pdown_options_enum_type      pdown_options)
{
  MMGSDIUTIL_RETURN_IF_NULL(card_pdown_cnf_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    card_pdown_cnf_ptr->option = pdown_options;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_card_pdown_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_REFRESH_DATA

   DESCRIPTION:
     This function builds cnf for the REFRESH

   DEPENDENCIES:


   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_refresh_data(
  mmgsdi_refresh_cnf_type                 * refresh_cnf_ptr,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status)
{
  MMGSDIUTIL_RETURN_IF_NULL_2(refresh_cnf_ptr, extra_param_ptr);

  refresh_cnf_ptr->orig_refresh_req = extra_param_ptr->refresh_data.refresh_req_type;
  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_refresh_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_STATUS_DATA

   DESCRIPTION:
     This function builds the Status confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_status_data(
  mmgsdi_status_cnf_type            * status_cnf_ptr,
  mmgsdi_return_enum_type             mmgsdi_status,
  const mmgsdi_data_type            * status_data_ptr)
{
  MMGSDIUTIL_RETURN_IF_NULL(status_cnf_ptr);
  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_RETURN_IF_NULL(status_data_ptr);
    /* Handle case where no Status Response data is expected */
    if (status_data_ptr->data_len == 0)
    {
      status_cnf_ptr->status_data.data_len = 0;
    }
    /* -----------------------------------------------------------------------
       Allocate memory for the Status Response data.
       If the malloc fails, continue with building the confirmation so that
       the client isn't hung.

       Copy the response data if mem is available or indicate in the
       confirmation that the memory was exhausted.

       mmgsdi_status will contain the status we want to return to the client
       for confirmation purposes.
       ---------------------------------------------------------------------*/
    else
    {
      if ((status_data_ptr->data_len) < 0)
      {
        MMGSDIUTIL_DEBUG_MSG_ERROR_1("Unexpected data_len : 0x%x",
                                     status_data_ptr->data_len);
        status_cnf_ptr->status_data.data_len = 0;
        return MMGSDI_INCORRECT_PARAMS;
      }
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(status_cnf_ptr->status_data.data_ptr,
                                         status_data_ptr->data_len)

      if (status_cnf_ptr->status_data.data_ptr == NULL )
      {
        status_cnf_ptr->status_data.data_len = 0;
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }

      mmgsdi_memscpy(status_cnf_ptr->status_data.data_ptr,
                     int32touint32(status_data_ptr->data_len),
                     status_data_ptr->data_ptr,
                     int32touint32(status_data_ptr->data_len));

      status_cnf_ptr->status_data.data_len = status_data_ptr->data_len;
    }
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_status_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_STATUS_PIN_OPERATION_DATA

   DESCRIPTION:
     This function builds the PIN Operation confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_pin_operation_data(
  const mmgsdi_request_header_type  * req_header_ptr,
  mmgsdi_pin_operation_cnf_type     * pin_operation_cnf_ptr,
  mmgsdi_return_enum_type             mmgsdi_status,
  mmgsdi_client_req_extra_info_type * extra_info_ptr)
{
  mmgsdi_return_enum_type             build_cnf_status  = MMGSDI_SUCCESS;
  mmgsdi_pin_extra_info_type          *pin_info_ptr     = NULL;
  mmgsdi_int_app_info_type            *sel_app_ptr      = NULL;
  mmgsdi_channel_info_type            *channel_info_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL_3(req_header_ptr, pin_operation_cnf_ptr,
                              extra_info_ptr);

  if(req_header_ptr->request_type == MMGSDI_GET_FILE_ATTR_REQ)
  {
    pin_info_ptr =  (mmgsdi_pin_extra_info_type*)
                    extra_info_ptr->get_file_attr_data.int_client_data;
  }
  else if(req_header_ptr->request_type == MMGSDI_PIN_OPERATION_REQ)
  {
    pin_info_ptr = &extra_info_ptr->pin_data;
  }

  MMGSDIUTIL_RETURN_IF_NULL(pin_info_ptr);
  pin_operation_cnf_ptr->pin_info.pin_id = pin_info_ptr->orig_pin_id;

  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                    req_header_ptr->session_id,
                    NULL,
                    &channel_info_ptr);
  if((mmgsdi_status != MMGSDI_SUCCESS)
       ||(channel_info_ptr == NULL)
       ||(channel_info_ptr->app_info_ptr == NULL))
  {
    return mmgsdi_status;
  }
  sel_app_ptr = channel_info_ptr->app_info_ptr;

  if(pin_info_ptr->pin_info.pin_id == MMGSDI_PIN1)
  {
    if(sel_app_ptr->pin1_ptr)
    {
      pin_operation_cnf_ptr->pin_info.num_retries               = sel_app_ptr->pin1_ptr->num_retries;
      pin_operation_cnf_ptr->pin_info.num_unblock_retries       = sel_app_ptr->pin1_ptr->num_unblock_retries;
      pin_operation_cnf_ptr->pin_info.status                    = sel_app_ptr->pin1_ptr->status;
      pin_operation_cnf_ptr->pin_info.pin_replacement           = sel_app_ptr->pin1_ptr->pin_replacement;
      pin_operation_cnf_ptr->pin_info.valid_num_retries         = sel_app_ptr->pin1_ptr->valid_num_retries;
      pin_operation_cnf_ptr->pin_info.valid_num_unblock_retries = sel_app_ptr->pin1_ptr->valid_num_unblock_retries;

    }
  }
  else if(pin_info_ptr->pin_info.pin_id == MMGSDI_PIN2)
  {
    pin_operation_cnf_ptr->pin_info.num_retries               = sel_app_ptr->pin2.num_retries;
    pin_operation_cnf_ptr->pin_info.num_unblock_retries       = sel_app_ptr->pin2.num_unblock_retries;
    pin_operation_cnf_ptr->pin_info.status                    = sel_app_ptr->pin2.status;
    pin_operation_cnf_ptr->pin_info.pin_replacement           = sel_app_ptr->pin2.pin_replacement;
    pin_operation_cnf_ptr->pin_info.valid_num_retries         = sel_app_ptr->pin2.valid_num_retries;
    pin_operation_cnf_ptr->pin_info.valid_num_unblock_retries = sel_app_ptr->pin2.valid_num_unblock_retries;
  }
  else if(pin_info_ptr->pin_info.pin_id == MMGSDI_UNIVERSAL_PIN)
  {
    if(sel_app_ptr->universal_pin_ptr)
    {
      pin_operation_cnf_ptr->pin_info.num_retries               = sel_app_ptr->universal_pin_ptr->num_retries;
      pin_operation_cnf_ptr->pin_info.num_unblock_retries       = sel_app_ptr->universal_pin_ptr->num_unblock_retries;
      pin_operation_cnf_ptr->pin_info.status                    = sel_app_ptr->universal_pin_ptr->status;
      pin_operation_cnf_ptr->pin_info.pin_replacement           = sel_app_ptr->universal_pin_ptr->pin_replacement;
      pin_operation_cnf_ptr->pin_info.valid_num_retries         = sel_app_ptr->universal_pin_ptr->valid_num_retries;
      pin_operation_cnf_ptr->pin_info.valid_num_unblock_retries = sel_app_ptr->universal_pin_ptr->valid_num_unblock_retries;
    }
  }
  else if(pin_info_ptr->pin_info.pin_id == MMGSDI_HIDDENKEY)
  {
    pin_operation_cnf_ptr->pin_info.num_retries               = 0;
    pin_operation_cnf_ptr->pin_info.num_unblock_retries       = 0;
    pin_operation_cnf_ptr->pin_info.status                    = sel_app_ptr->hiddenkey.status;
    pin_operation_cnf_ptr->pin_info.pin_replacement           = sel_app_ptr->hiddenkey.pin_replacement;
    pin_operation_cnf_ptr->pin_info.valid_num_retries         = sel_app_ptr->hiddenkey.valid_num_retries;
    pin_operation_cnf_ptr->pin_info.valid_num_unblock_retries = sel_app_ptr->hiddenkey.valid_num_unblock_retries;
  }

  pin_operation_cnf_ptr->pin_op = pin_info_ptr->orig_pin_op;

  return build_cnf_status;
} /* mmgsdi_cnf_build_pin_operation_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_ALL_PIN_STATUS_DATA

   DESCRIPTION:
     This function builds the PIN Status confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_get_all_pin_status_data(
  mmgsdi_get_all_pin_status_cnf_type     * get_all_pin_status_cnf_ptr,
  const mmgsdi_request_header_type       * req_header_ptr,
  mmgsdi_return_enum_type                  mmgsdi_status)
{
  mmgsdi_int_app_info_type          *sel_app_ptr      = NULL;
  mmgsdi_channel_info_type          *channel_info_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(get_all_pin_status_cnf_ptr);

  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                             req_header_ptr->session_id,
                             NULL,
                             &channel_info_ptr);
  if((mmgsdi_status != MMGSDI_SUCCESS) ||
     (channel_info_ptr == NULL)        ||
     (channel_info_ptr->app_info_ptr == NULL))
  {
    return mmgsdi_status;
  }

  sel_app_ptr = channel_info_ptr->app_info_ptr;

  MMGSDIUTIL_RETURN_STATUS_IF_NULL(sel_app_ptr, MMGSDI_ERROR);

  /*Copy Over PIN1*/
  mmgsdi_util_set_pin_info(
    &get_all_pin_status_cnf_ptr->pin1,sel_app_ptr->pin1_ptr);

  /*Copy Over PIN2*/
  mmgsdi_util_set_pin_info(
    &get_all_pin_status_cnf_ptr->pin2, &sel_app_ptr->pin2);

  /*Copy Over Universal Pin*/
  mmgsdi_util_set_pin_info(
    &get_all_pin_status_cnf_ptr->universal_pin, sel_app_ptr->universal_pin_ptr);

  /*Copy Over Hiddenkey*/
  mmgsdi_util_set_pin_info(
    &get_all_pin_status_cnf_ptr->hiddenkey, &sel_app_ptr->hiddenkey);

  /* Copy the EF-DIR index corresponding to the app */
  get_all_pin_status_cnf_ptr->dir_index = sel_app_ptr->dir_index;

  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_get_all_pin_status_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SRV_AVAILABLE_DATA

   DESCRIPTION:
     This function builds the Service Available confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_srv_available_data(
  mmgsdi_srv_available_cnf_type           * srv_available_cnf_ptr,
  mmgsdi_return_enum_type                 * mmgsdi_status_ptr,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const mmgsdi_request_header_type        * req_header_ptr
)
{
  MMGSDIUTIL_RETURN_IF_NULL_4(srv_available_cnf_ptr, extra_param_ptr, req_header_ptr, mmgsdi_status_ptr);

  srv_available_cnf_ptr->srv_available = FALSE;

  if (*mmgsdi_status_ptr == MMGSDI_SUCCESS)
  {
    mmgsdi_protocol_enum_type protocol = mmgsdi_util_determine_protocol(req_header_ptr->slot_id);

    *mmgsdi_status_ptr = MMGSDI_ERROR;
    if(protocol == MMGSDI_UICC)
    {
      *mmgsdi_status_ptr = mmgsdi_uicc_chk_srv_available(req_header_ptr->client_id,
                                                         req_header_ptr->session_id,
                                                         MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                         extra_param_ptr->srv_available_data.service,
                                                         &srv_available_cnf_ptr->srv_available);
    }
    else if(protocol == MMGSDI_ICC)
    {
      *mmgsdi_status_ptr = mmgsdi_icc_chk_srv_available(req_header_ptr->session_id,
                                                        MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                        extra_param_ptr->srv_available_data.service,
                                                        &srv_available_cnf_ptr->srv_available);
    }
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_srv_available_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_USIM_AUTH_DATA

   DESCRIPTION:
     This function builds the USIM authentication confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_usim_auth_data(
  mmgsdi_usim_auth_cnf_type   *usim_auth_cnf_ptr,
  mmgsdi_return_enum_type      mmgsdi_status,
  const mmgsdi_data_type      *auth_rsp_data_ptr)
{
  uint32                   auth_len            = 0;

  MMGSDIUTIL_RETURN_IF_NULL_2(usim_auth_cnf_ptr, auth_rsp_data_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    /*-------------------------------------------------------------------------
      Authentication response is successful
      - Mem malloc corresponding data pointers for RES, CK and IK
      - Populate the data pointers
      - Error and free if memory allocation failed
    -------------------------------------------------------------------------*/
   auth_len = int32touint32(auth_rsp_data_ptr->data_len);

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      usim_auth_cnf_ptr->auth_response.data_ptr,
      auth_len);
    if (usim_auth_cnf_ptr->auth_response.data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    usim_auth_cnf_ptr->auth_response.data_len =
      auth_rsp_data_ptr->data_len;
    mmgsdi_memscpy(usim_auth_cnf_ptr->auth_response.data_ptr,
                   auth_len,
                   auth_rsp_data_ptr->data_ptr,
                   auth_len);
  }
  else
  {
    UIM_MSG_HIGH_1("USIM Auth Fail, status %x", mmgsdi_status);
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_usim_auth_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_ATR_DATA

   DESCRIPTION:
     This function builds the get ATR confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_get_atr_data(
  mmgsdi_get_atr_cnf_type       * get_atr_cnf_ptr,
  mmgsdi_return_enum_type         mmgsdi_status,
  mmgsdi_slot_id_enum_type        slot_id)
{
  uint32                   atr_len             = 0;
  mmgsdi_slot_data_type   *slot_data_ptr       = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(get_atr_cnf_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    atr_len = mmgsdi_cnf_get_atr_len(slot_id);
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(get_atr_cnf_ptr->atr_data.data_ptr,
                                       atr_len);

    if (get_atr_cnf_ptr->atr_data.data_ptr == NULL)
    {
      return MMGSDI_ERROR;
    }

    slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

    if(slot_data_ptr == NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(get_atr_cnf_ptr->atr_data.data_ptr);
      return MMGSDI_ERROR;
    }

    get_atr_cnf_ptr->atr_data.data_len =
      (int32)slot_data_ptr->card_atr.atr_length;

    mmgsdi_memscpy(get_atr_cnf_ptr->atr_data.data_ptr,
                   atr_len,
                   slot_data_ptr->card_atr.atr_data,
                   slot_data_ptr->card_atr.atr_length);

  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_get_atr_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_GENERATE_GET_ALL_PIN_STATUS_EVT

   DESCRIPTION:
     This function generates all the pin events as a result of the
     get_all_pin_status confirmation

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     void

   SIDE EFFECTS:
     None
=============================================================================*/
void mmgsdi_cnf_generate_get_all_pin_status_evt(
  mmgsdi_session_id_type session_id)
{
  mmgsdi_int_app_info_type *sel_app_ptr    = NULL;
  mmgsdi_event_data_type   *mmgsdi_evt_ptr = NULL;
  mmgsdi_return_enum_type   event_status   = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type  slot           = MMGSDI_MAX_SLOT_ID_ENUM;

  (void)mmgsdi_util_get_session_app_info(session_id,
                                         &slot,
                                         NULL,
                                         &sel_app_ptr,
                                         NULL,
                                         NULL,
                                         NULL);
  if(sel_app_ptr == NULL)
     return;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_evt_ptr,
                                     sizeof(mmgsdi_event_data_type));

  if (mmgsdi_evt_ptr == NULL)
  {
    return;
  }

  if (sel_app_ptr->pin1_ptr != NULL)
  {
    event_status =  mmgsdi_evt_build_pin(session_id,
                                         MMGSDI_PIN1,
                                         sel_app_ptr->pin1_ptr->status,
                                         mmgsdi_evt_ptr);
    if(event_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("Could not Send PIN 1Event to Clients");
    }
    else
    {
      mmgsdi_evt_notify_session(mmgsdi_evt_ptr,
                                slot,
                                (const mmgsdi_int_app_info_type*)sel_app_ptr,
                                TRUE);
    }
  }

  event_status =  mmgsdi_evt_build_pin(session_id,
                                       MMGSDI_PIN2,
                                       sel_app_ptr->pin2.status,
                                       mmgsdi_evt_ptr);
  if(event_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Could not Send PIN 2 Event to Clients");
  }
  else
  {
    mmgsdi_evt_notify_session(mmgsdi_evt_ptr,
                              slot,
                              (const mmgsdi_int_app_info_type*)sel_app_ptr,
                              TRUE);
  }

  if (sel_app_ptr->universal_pin_ptr != NULL)
  {
    event_status =  mmgsdi_evt_build_pin(session_id,
                                         MMGSDI_UNIVERSAL_PIN,
                                         sel_app_ptr->universal_pin_ptr->status,
                                         mmgsdi_evt_ptr);
    if(event_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("Could not Send UNIV PIN Event to Clients");
    }
    else
    {
      mmgsdi_evt_notify_session(mmgsdi_evt_ptr,
                                slot,
                                (const mmgsdi_int_app_info_type*)sel_app_ptr,
                                TRUE);
    }
  }
  mmgsdi_evt_free_data(mmgsdi_evt_ptr);
  MMGSDIUTIL_TMC_MEM_FREE( mmgsdi_evt_ptr);
} /* mmgsdi_cnf_generate_get_all_pin_status_evt */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_GENERATE_PIN_OP_EVT

   DESCRIPTION:
     This function generates pin events as a result of the
     pin_op confirmation

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     void

   SIDE EFFECTS:
     None
=============================================================================*/
static void mmgsdi_cnf_generate_pin_op_evt(
  mmgsdi_return_enum_type              mmgsdi_status,
  const mmgsdi_pin_operation_cnf_type *pin_cnf_ptr)
{
  mmgsdi_pin_status_enum_type     pin_status     = MMGSDI_PIN_STATUS_NOT_INITIALIZED;
  mmgsdi_int_app_info_type       *app_info_ptr   = NULL;
  mmgsdi_slot_id_enum_type        slot_id        = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_app_pin_info_type       *pin_info_ptr   = NULL;
  mmgsdi_slot_data_type          *slot_data_ptr  = NULL;
  mmgsdi_session_info_type       *session_ptr    = NULL;
  uint32                          i              = 0;
  mmgsdi_protocol_enum_type       protocol       = MMGSDI_MAX_PROTOCOL_ENUM;
  mmgsdi_evt_session_notify_type  notify_type;
  mmgsdi_pin_enum_type            pin_id         = MMGSDI_PIN1;

  memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));

  if (pin_cnf_ptr == NULL)
    return;

  pin_status = pin_cnf_ptr->pin_info.status;

  if (pin_cnf_ptr->pin_info.pin_id == MMGSDI_KEY_REF_PIN_APPL_1 ||
      pin_cnf_ptr->pin_info.pin_id == MMGSDI_KEY_REF_PIN_APPL_2 ||
      pin_cnf_ptr->pin_info.pin_id == MMGSDI_KEY_REF_PIN_APPL_3 ||
      pin_cnf_ptr->pin_info.pin_id == MMGSDI_KEY_REF_PIN_APPL_4 ||
      pin_cnf_ptr->pin_info.pin_id == MMGSDI_KEY_REF_PIN_APPL_5 ||
      pin_cnf_ptr->pin_info.pin_id == MMGSDI_KEY_REF_PIN_APPL_6 ||
      pin_cnf_ptr->pin_info.pin_id == MMGSDI_KEY_REF_PIN_APPL_7 ||
      pin_cnf_ptr->pin_info.pin_id == MMGSDI_KEY_REF_PIN_APPL_8)
  {
    pin_id = MMGSDI_PIN1;
  }
  else if (pin_cnf_ptr->pin_info.pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_1 ||
           pin_cnf_ptr->pin_info.pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_2 ||
           pin_cnf_ptr->pin_info.pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_3 ||
           pin_cnf_ptr->pin_info.pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_4 ||
           pin_cnf_ptr->pin_info.pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_5 ||
           pin_cnf_ptr->pin_info.pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_6 ||
           pin_cnf_ptr->pin_info.pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_7 ||
           pin_cnf_ptr->pin_info.pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_8)
  {
    pin_id = MMGSDI_PIN2;
  }
  else
  {
    pin_id = pin_cnf_ptr->pin_info.pin_id;
  }

  if ((pin_status != MMGSDI_PIN_BLOCKED) &&
      (pin_status != MMGSDI_PIN_PERM_BLOCKED))
  {
    /* for non blocked or perm blocked status, a success operation or an
       operation that causes the retry count to decrease should generate
       an event */
    if ((mmgsdi_status != MMGSDI_SUCCESS) &&
        (mmgsdi_status != MMGSDI_INCORRECT_CODE))
      return;

    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      switch (pin_cnf_ptr->pin_op)
      {
        case MMGSDI_PIN_OP_CHANGE:
          pin_status = MMGSDI_PIN_CHANGED;
          break;
        case MMGSDI_PIN_OP_UNBLOCK:
          pin_status = MMGSDI_PIN_UNBLOCKED;
          break;
        case MMGSDI_PIN_OP_VERIFY:
        case MMGSDI_PIN_OP_ENABLE:
          break;
        case MMGSDI_PIN_OP_DISABLE:
        case MMGSDI_PIN_OP_DISABLE_AND_REPLACE:
          pin_status = MMGSDI_PIN_DISABLED;
          break;
        case MMGSDI_PIN_OP_NONE:
          return;
      }
    }
    else if (mmgsdi_status == MMGSDI_INCORRECT_CODE)
    {
      /* This error code indicates that the retry count decreased.
         So set the pin status accordingly */
      pin_status = MMGSDI_PIN_RETRY_COUNT_DECREASED;
    }
  }

  (void)mmgsdi_util_get_session_app_info(
          pin_cnf_ptr->response_header.session_id,
          &slot_id,
          NULL,
          &app_info_ptr,
          NULL,
          NULL,
          NULL);
  if(app_info_ptr == NULL)
  {
    UIM_MSG_ERR_1("Invalid apps for pin id 0x%x", pin_id);
    return;
  }

  switch (pin_id)
  {
    case MMGSDI_PIN1:
      pin_info_ptr = app_info_ptr->pin1_ptr;
      break;
    case MMGSDI_PIN2:
      pin_info_ptr = &app_info_ptr->pin2;
      break;
    case MMGSDI_UNIVERSAL_PIN:
      pin_info_ptr = app_info_ptr->universal_pin_ptr;
      break;
    case MMGSDI_HIDDENKEY:
      pin_info_ptr = &app_info_ptr->hiddenkey;
      break;
    default: /* Rest of the PIN to be handle in later stage */
      break;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);
  protocol = mmgsdi_util_determine_protocol(slot_id);

  if(pin_info_ptr == NULL  ||
     slot_data_ptr == NULL || slot_data_ptr->app_info_pptr == NULL ||
     protocol == MMGSDI_NO_PROTOCOL)
  {
    UIM_MSG_ERR_2("Invalid pin id 0x%x or Invalid slot for pin id or invalid protocol 0x%x",
                  pin_id,
                  protocol);
    return;
  }

  /* Send out PIN2/Hiddenkey event to all Sessions on the app. PIN2 event is
     sent out here only for UICC apps */
  if(((pin_id == MMGSDI_PIN2) && (protocol == MMGSDI_UICC)) ||
     (pin_id == MMGSDI_HIDDENKEY))
  {
    notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
    notify_type.slot_id     = slot_id;
    notify_type.session_id  = pin_cnf_ptr->response_header.session_id;

    mmgsdi_evt_build_and_send_pin(notify_type,
                                  pin_id,
                                  pin_status,
                                  (const mmgsdi_int_app_info_type*)app_info_ptr);

    return;
  }

  /* PIN2 is local to the application for UICC apps. But for ICC cards, CHV2
     can be shared between DF-GSM and DF-CDMA. Hence when CHV2 status gets
     updated, the event is sent out to all provisioning sessions on the card */

  /* Posting PIN event to all applications who have same PIN reference */
  for (i = 0; i < MMGSDI_MAX_EXT_APP_INFO; i++)
  {
    if(slot_data_ptr->app_info_pptr[i] == NULL)
    {
      continue;
    }
    if ((pin_info_ptr == slot_data_ptr->app_info_pptr[i]->pin1_ptr) ||
        (pin_info_ptr == slot_data_ptr->app_info_pptr[i]->universal_pin_ptr) ||
        (pin_info_ptr->key_ref == slot_data_ptr->app_info_pptr[i]->pin2.key_ref))
    {
      app_info_ptr = slot_data_ptr->app_info_pptr[i];

      /* Update PIN2 Status for Apps */
      if (MMGSDI_PIN2 == pin_id)
      {
        app_info_ptr->pin2.cached              =  pin_info_ptr->cached;
        app_info_ptr->pin2.pin_replacement     =  pin_info_ptr->pin_replacement;
        app_info_ptr->pin2.status              =  pin_info_ptr->status;
        app_info_ptr->pin2.valid_pin           =  pin_info_ptr->valid_pin;
        app_info_ptr->pin2.num_retries         =  pin_info_ptr->num_retries;
        app_info_ptr->pin2.num_unblock_retries =
                                        pin_info_ptr->num_unblock_retries;
        app_info_ptr->pin2.pin_data.data_len   =
                                        pin_info_ptr->pin_data.data_len;
        mmgsdi_memscpy(app_info_ptr->pin2.pin_data.data_ptr,
                       sizeof(app_info_ptr->pin2.pin_data.data_ptr),
                       pin_info_ptr->pin_data.data_ptr,
                       int32touint32(pin_info_ptr->pin_data.data_len));
      }

      /* Check App have any session or not */
      mmgsdi_status = mmgsdi_util_get_session_info_from_app(
                                                     &app_info_ptr->app_data,
                                                     slot_id,
                                                     &session_ptr);
      if ((mmgsdi_status != MMGSDI_SUCCESS) || (session_ptr == NULL))
      {
        /* No need to send the evt as it does not have any session */
        continue;
      }
      notify_type.notify_type                  = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
      notify_type.slot_id                      = slot_id;
      notify_type.session_id                   = session_ptr->session_id;
      mmgsdi_evt_build_and_send_pin(
                              notify_type,
                              pin_id,
                              pin_status,
                              (const mmgsdi_int_app_info_type*)app_info_ptr);
    }
  }
} /* mmgsdi_cnf_generate_pin_op_evt */


#ifdef FEATURE_MODEM_CONFIG_REFRESH
/* =============================================================================
   FUNCTION:      MMGSDI_TRIGGER_MCFG_NV_REFRESH_FOR_SESSION

   DESCRIPTION:
     This function checks if the MCFG NV Refresh is required for session and then triggers it

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
    None
=============================================================================*/
static void mmgsdi_trigger_mcfg_refresh_session(
  mmgsdi_slot_id_enum_type          slot,
  mmgsdi_int_app_info_type         **sel_app_pptr,
  const mmgsdi_session_info_type   *session_info_ptr,
  const mmgsdi_channel_info_type   *channel_info_ptr,
  mmgsdi_session_id_type            session_to_check,
  mcfg_refresh_index_type           mcfg_refresh_index
)
{
  boolean                          mcfg_refresh_status      = FALSE;
  mcfg_refresh_uimdata_s_type      mcfg_uim_data;
  mmgsdi_cache_init_enum_type      cache_state              = MMGSDI_CACHE_MAX_ENUM;
  mmgsdi_data_type                 iccid;
  mmgsdi_access_type               access;
  uint8                            iccid_data[MMGSDI_ICCID_LEN];
  mmgsdi_session_id_type           card_session_id          = 0;
  mmgsdi_int_app_info_type       * session_to_check_app_ptr = NULL;
  boolean                          nv_refresh_enabled       = FALSE;
  mmgsdi_return_enum_type          mmgsdi_status            = MMGSDI_ERROR;
  mmgsdi_slot_data_type           *slot_data_ptr            = NULL;

  memset(&mcfg_uim_data, 0x00, sizeof(mcfg_refresh_uimdata_s_type));

  /* Do not trigger sending out session changed evt if NV Refresh is required */
  switch(slot)
  {
    case MMGSDI_SLOT_1:
      card_session_id = mmgsdi_generic_data_ptr->slot_session_id[MMGSDI_SLOT_1_INDEX];
      break;
    case MMGSDI_SLOT_2:
      card_session_id = mmgsdi_generic_data_ptr->slot_session_id[MMGSDI_SLOT_2_INDEX];
      break;
    case MMGSDI_SLOT_3:
      card_session_id = mmgsdi_generic_data_ptr->slot_session_id[MMGSDI_SLOT_3_INDEX];
      break;
    default:
      UIM_MSG_ERR_1("Invalid slot - 0x%x",
                    slot);
      return;
  }

  if(mmgsdi_util_get_mcfg_refresh_index(slot, &mcfg_uim_data.slot_index) != MMGSDI_SUCCESS)
  {
     return;
  }

  nv_refresh_enabled = mcfg_refresh_autoselect_enabled(mcfg_uim_data.slot_index);
  if(!nv_refresh_enabled)
  {
    UIM_MSG_HIGH_0("MCFG REFRESH not set in nv - proceed with session changed EVT");
    return;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);
  if(slot_data_ptr == NULL)
  {
    return;
  }

  /* Check if an existing session had already sent a refresh trigger
        for same context, if so avoid sending again, when the done
        is received for already sent app, this session will continue */
  mmgsdi_status = mmgsdi_util_get_session_app_info(
             session_to_check,
             NULL,
             NULL,
             &session_to_check_app_ptr,
             NULL,NULL,NULL);

  if((mmgsdi_status != MMGSDI_SUCCESS) || (session_to_check_app_ptr == NULL) ||
     ((session_to_check_app_ptr != NULL) &&
      (session_to_check_app_ptr->nv_refresh_in_progress == FALSE)))
  {
    UIM_MSG_ERR_1("session to check is invalid or nv refresh is not in progress, so proceed with MCFG Refresh for session type: 0x%x",
                  session_info_ptr->session_type);

    /* Trigger MCFG NV Refresh */
    memset(&access, 0x00, sizeof(mmgsdi_access_type));
    memset(&iccid, 0x00, sizeof(mmgsdi_data_type));

    access.access_method = MMGSDI_EF_ENUM_ACCESS;
    access.file.file_enum = MMGSDI_ICCID;
    /* Get the Card Slot Session ID corresponding to the slot */

    mmgsdi_status = mmgsdi_cache_read_len(card_session_id,
                                          &access,
                                          &iccid.data_len,
                                          0,
                                          &cache_state);

    if (mmgsdi_status == MMGSDI_SUCCESS
        && cache_state != MMGSDI_CACHE_NOT_FOUND
        && iccid.data_len <= MMGSDI_ICCID_LEN)
    {
      iccid.data_ptr = &iccid_data[0];

      mmgsdi_status = mmgsdi_cache_read(card_session_id,
                                        &access,
                                        &iccid,
                                        0,
                                        &cache_state);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        iccid.data_len = 0;
        UIM_MSG_ERR_2("failed to read cache for ICCID: mmgsdi_status 0x%x, cache_state 0x%x",
                      mmgsdi_status, cache_state);
      }
    }
    else
    {
      iccid.data_len = 0;
      UIM_MSG_ERR_2("failed to read cache len for ICCID: mmgsdi_status 0x%x, cache_state 0x%x",
                    mmgsdi_status, cache_state);
    }

    mcfg_uim_data.iccid.data_len = iccid.data_len;
    mcfg_uim_data.iccid.data_ptr = iccid.data_ptr;
    mcfg_uim_data.type = MCFG_REFRESH_UIM_SESSION;
    mcfg_uim_data.sub_index = mcfg_refresh_index;
    mcfg_uim_data.reference_id = ++slot_data_ptr->nv_refresh_reference_count;

    mcfg_refresh_status = mcfg_refresh_update_from_uim (&mcfg_uim_data);
    if(mcfg_refresh_status)
    {
      (*sel_app_pptr)->nv_refresh_in_progress = TRUE;
    }
    UIM_MSG_HIGH_2("MCFG triggered for refresh, for session type 0x%x, status 0x%x",
                    session_info_ptr->session_type,mcfg_refresh_status);

  }
  else
  {
    UIM_MSG_ERR_1("nv refresh for 'session to check' is in progress, update the current session and proceed - session type: 0x%x",
                  session_info_ptr->session_type);
    (*sel_app_pptr)->nv_refresh_in_progress = TRUE;

  }
  return;
}/*mmgsdi_trigger_mcfg_refresh_session*/
#endif /* FEATURE_MODEM_CONFIG_REFRESH */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_GENERATE_RSP_AND_EVT

   DESCRIPTION:
     This function based on the confirmation, generate the reponse and
     corresponding event as needed

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None

   CRITICAL SECTIONS:
    The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
    to prov app indices.
=============================================================================*/
void mmgsdi_cnf_generate_rsp_and_evt(
  const mmgsdi_callback_cnf_type *callback_cnf_ptr)
{
  mmgsdi_int_app_info_type       * sel_app_ptr          = NULL;
  mmgsdi_return_enum_type          mmgsdi_status        = MMGSDI_ERROR;
  mmgsdi_session_info_type       * session_info_ptr     = NULL;
  mmgsdi_channel_info_type       * channel_info_ptr     = NULL;
  uint32                           app_index            = 0;
  mmgsdi_slot_data_type          * slot_data_ptr        = NULL;
  mmgsdi_slot_id_enum_type         slot                 = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_session_id_type           session_id           = 0;
  int32                            client_index         = MMGSDI_MAX_CLIENT_INFO;
  mmgsdi_evt_session_notify_type   notify_type;
  uint32                         * prov_app_indices_ptr = NULL;
  boolean                          activate             = FALSE;
  int32                            index                = 0;
  int32                            session_index        = 0;
  mmgsdi_slot_id_enum_type         slot_id              = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_session_id_type           session_to_check     = 0;
  mcfg_refresh_index_type          mcfg_refresh_index   = MCFG_REFRESH_INDEX_INVALID;
  mmgsdi_return_enum_type          cnf_status           = MMGSDI_SUCCESS;
  mmgsdi_cnf_enum_type             cnf                  = MMGSDI_MAX_CNF_ENUM;
  const mmgsdi_cnf_type          * cnf_ptr              = NULL;

  if(callback_cnf_ptr == NULL)
  {
    return;
  }

  cnf_status = callback_cnf_ptr->mmgsdi_cnf.response_header.mmgsdi_status;
  cnf        = callback_cnf_ptr->mmgsdi_cnf.response_header.response_type;
  cnf_ptr    = &callback_cnf_ptr->mmgsdi_cnf;

  /* Send confirmation for all the responses except switch/activate and select AID request.
     The confirmation of switch/activate request is sent along with session changed event
     In case of no nv refresh the cnf is sent in this same function in the switch
     case and for nv refresh it is done when continue_after_refresh API is invoked 
     No NV refresh is done for select AID request and confirmation is sent when the sesion
     is non prov sessoin.*/
  if(callback_cnf_ptr->client_req_cb != NULL &&
     cnf != MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ &&
     cnf != MMGSDI_SESSION_SELECT_AID_REQ)
  {
    (callback_cnf_ptr->client_req_cb)(
      cnf_status,
      cnf,
      cnf_ptr);
  }

  switch(cnf)
  {
    case MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ:
      session_id =
        cnf_ptr->session_activate_or_switch_provisioning_cnf.response_header.session_id;
      break;
    case MMGSDI_SESSION_SELECT_AID_REQ:
      session_id = cnf_ptr->session_select_aid_cnf.response_header.session_id;
      break;
    case MMGSDI_SESSION_ENABLE_SERVICE_REQ:
      session_id = cnf_ptr->session_enable_service_cnf.response_header.session_id;
      break;
    case MMGSDI_SESSION_DISABLE_SERVICE_REQ:
      session_id = cnf_ptr->session_disable_service_cnf.response_header.session_id;
      break;
    case MMGSDI_SESSION_APP_RESET_DEACTIVATE_REQ:
      session_id =
        cnf_ptr->session_app_reset_deactivate_cnf.response_header.session_id;
        break;
    case MMGSDI_SESSION_APP_RESET_ACTIVATE_REQ:
      session_id =
        cnf_ptr->session_app_reset_activate_cnf.response_header.session_id;
      break;
    default:
      break;
  }

  memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));

  switch(cnf)
  {
    case MMGSDI_PIN_OPERATION_REQ:
      switch (cnf_ptr->pin_operation_cnf.pin_info.status)
      {
        case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
        case MMGSDI_PIN_NOT_FOUND:
        case MMGSDI_PIN_RETRY_COUNT_DECREASED:
          return;
        case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
        case MMGSDI_PIN_ENABLED_VERIFIED:
        case MMGSDI_PIN_DISABLED:
        case MMGSDI_PIN_BLOCKED:
        case MMGSDI_PIN_PERM_BLOCKED:
        case MMGSDI_PIN_UNBLOCKED:
        case MMGSDI_PIN_CHANGED:
          mmgsdi_cnf_generate_pin_op_evt(cnf_status, &cnf_ptr->pin_operation_cnf);
          break;
      }
      break;

    case MMGSDI_SESSION_OPEN_REQ:
    case MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_REQ:
    case MMGSDI_SESSION_OPEN_WITH_MF_REQ:
      /* Check if we need to send session event */
      if(cnf_ptr->response_header.mmgsdi_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = mmgsdi_util_get_client_and_session_index(
                            cnf_ptr->session_open_cnf.response_header.session_id,
                            &client_index,
                            &session_index);

        if(mmgsdi_status == MMGSDI_SUCCESS)
        {
          /* Session events to clients can be sent only after session open cnf is sent.
             Hence set the variable to true to allow sending of session events */
          mmgsdi_client_id_reg_table[client_index]->
                           session_info_ptr[session_index]->
                           is_session_open_cnf_sent = TRUE;
        }
        mmgsdi_evt_build_and_notify_session_evt_from_state(
          cnf_ptr->session_open_cnf.response_header.session_id);
      }
      break;

    case MMGSDI_SESSION_OPEN_EXT_REQ:
      /* Check if we need to send session event */
      if(cnf_ptr->session_open_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS &&
         cnf_ptr->session_open_ext_cnf.session_info_ptr != NULL)
      {
        for(index = 0; index < cnf_ptr->session_open_ext_cnf.num_sessions; index++)
        {
          /* Loop through all the sessions and
             set the variable to allow sending of session events */
          mmgsdi_status = mmgsdi_util_get_client_and_session_index(
                cnf_ptr->session_open_ext_cnf.session_info_ptr[index].session_id,
                &client_index,
                &session_index);

          if(mmgsdi_status == MMGSDI_SUCCESS )
          {
             mmgsdi_client_id_reg_table[client_index]->
                              session_info_ptr[session_index]->
                              is_session_open_cnf_sent = TRUE;
          }
          mmgsdi_evt_build_and_notify_session_evt_from_state(
            cnf_ptr->session_open_ext_cnf.session_info_ptr[index].session_id);
        }
      }
      break;

    case MMGSDI_SESSION_SELECT_AID_REQ:
    case MMGSDI_SESSION_ACT_OR_SWITCH_PROV_REQ:
      if(cnf_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = mmgsdi_util_get_session_app_info(
                          session_id,
                          &slot,
                          NULL,
                          &sel_app_ptr,
                          &channel_info_ptr,
                          &session_info_ptr,
                          &app_index);

        if((mmgsdi_status != MMGSDI_SUCCESS) || (sel_app_ptr == NULL) ||
           (session_info_ptr == NULL) || (channel_info_ptr == NULL))
        {
          UIM_MSG_ERR_3("Invalid session - session_info_p=0x%x,chnnl_info_p=0x%x,app_info_p=0x%x",
                        session_info_ptr, channel_info_ptr, sel_app_ptr);
          return;
        }

        /* Get the slot data ptr */
        slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot);
        if(slot_data_ptr == NULL)
        {
          return;
        }
        switch(session_info_ptr->session_type)
        {
          case MMGSDI_GW_PROV_PRI_SESSION:
            prov_app_indices_ptr =
              &slot_data_ptr->prov_app_indices[MMGSDI_PRI_GW_PROV_APP_SEL_INDEX];
            session_to_check = mmgsdi_generic_data_ptr->pri_1x_session_id;
            mcfg_refresh_index = MCFG_REFRESH_INDEX_0;
            break;
          case MMGSDI_GW_PROV_SEC_SESSION:
            prov_app_indices_ptr =
              &slot_data_ptr->prov_app_indices[MMGSDI_SEC_GW_PROV_APP_SEL_INDEX];
            session_to_check = mmgsdi_generic_data_ptr->sec_1x_session_id;
            mcfg_refresh_index = MCFG_REFRESH_INDEX_1;
            break;
          case MMGSDI_GW_PROV_TER_SESSION:
            prov_app_indices_ptr =
              &slot_data_ptr->prov_app_indices[MMGSDI_TER_GW_PROV_APP_SEL_INDEX];
            session_to_check = mmgsdi_generic_data_ptr->ter_1x_session_id;
            mcfg_refresh_index = MCFG_REFRESH_INDEX_2;
            break;
          case MMGSDI_1X_PROV_PRI_SESSION:
            prov_app_indices_ptr =
              &slot_data_ptr->prov_app_indices[MMGSDI_PRI_1X_PROV_APP_SEL_INDEX];
            session_to_check = mmgsdi_generic_data_ptr->pri_gw_session_id;
            mcfg_refresh_index = MCFG_REFRESH_INDEX_0;
            break;
          case MMGSDI_1X_PROV_SEC_SESSION:
            prov_app_indices_ptr =
              &slot_data_ptr->prov_app_indices[MMGSDI_SEC_1X_PROV_APP_SEL_INDEX];
            session_to_check = mmgsdi_generic_data_ptr->sec_gw_session_id;
            mcfg_refresh_index = MCFG_REFRESH_INDEX_1;
            break;
          case MMGSDI_1X_PROV_TER_SESSION:
            prov_app_indices_ptr =
              &slot_data_ptr->prov_app_indices[MMGSDI_TER_1X_PROV_APP_SEL_INDEX];
            session_to_check = mmgsdi_generic_data_ptr->ter_gw_session_id;
            mcfg_refresh_index = MCFG_REFRESH_INDEX_2;
            break;
          default:
            UIM_MSG_ERR_1("Invalid session type - 0x%x",
                          session_info_ptr->session_type);
            if(callback_cnf_ptr->client_req_cb != NULL)
            {
              (callback_cnf_ptr->client_req_cb)(cnf_status,
                                                cnf,
                                                cnf_ptr);
            }

            return;
        }

        /* If the app is already activated, send the confirmation and do not send out any events */
        if((app_index | MMGSDI_PROV_INDEX_VALIDITY_MASK) == *prov_app_indices_ptr)
        {
          if(callback_cnf_ptr->client_req_cb != NULL)
          {
            (callback_cnf_ptr->client_req_cb)(cnf_status,
                                              cnf,
                                              cnf_ptr);
          }

          return;
        }

        MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

        /* Update the new app index */
        *prov_app_indices_ptr = app_index | MMGSDI_PROV_INDEX_VALIDITY_MASK;

        MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

#ifdef FEATURE_MODEM_CONFIG_REFRESH
        /* Do not trigger sending out session changed evt if NV Refresh is required */
        (void)mmgsdi_trigger_mcfg_refresh_session(
                slot,&sel_app_ptr,session_info_ptr,channel_info_ptr,
                session_to_check,mcfg_refresh_index);
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

        if(!sel_app_ptr->nv_refresh_in_progress)
        {
          /* Send the confirmation for activation before sending session changed event */
          if(callback_cnf_ptr->client_req_cb != NULL)
          {
            (callback_cnf_ptr->client_req_cb)(cnf_status,
                                              cnf,
                                              cnf_ptr);
          }

          notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
          notify_type.slot_id     = channel_info_ptr->slot_id;
          mmgsdi_evt_build_and_send_session_changed(
            notify_type, TRUE, (const mmgsdi_int_app_info_type*)sel_app_ptr);
        }
        else
        {
          uint8 prov_index = MMGSDI_MAX_PROV_APPS;

          prov_index = mmgsdi_util_get_prov_index_from_session(session_info_ptr->session_type);
          if(prov_index == MMGSDI_MAX_PROV_APPS ||
             mmgsdi_generic_data_ptr->mmgsdi_switch_act_cnf_ptr[prov_index] != NULL)
          {
            /* Send the confirmation at least in error case */
            if(callback_cnf_ptr->client_req_cb != NULL)
            {
              (callback_cnf_ptr->client_req_cb)(cnf_status,
                                                cnf,
                                                cnf_ptr);
            }
            return;
          }

          /* Cache the confirmation data to send the confirmation after MCFG NV refresh is completed */
          MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_generic_data_ptr->mmgsdi_switch_act_cnf_ptr[prov_index],
                                             sizeof(mmgsdi_callback_cnf_type));
          if(mmgsdi_generic_data_ptr->mmgsdi_switch_act_cnf_ptr[prov_index] == NULL)
          {
            /* Send the confirmation at least in error case */
            if(callback_cnf_ptr->client_req_cb != NULL)
            {
              (callback_cnf_ptr->client_req_cb)(cnf_status,
                                                cnf,
                                                cnf_ptr);
            }
            return;
          }

          mmgsdi_memscpy(mmgsdi_generic_data_ptr->mmgsdi_switch_act_cnf_ptr[prov_index],
                         sizeof(mmgsdi_callback_cnf_type),
                         callback_cnf_ptr,
                         sizeof(mmgsdi_callback_cnf_type));
        }
      }
      else
      {
        /* Send the confirmation in error case */
        if(callback_cnf_ptr->client_req_cb != NULL)
        {
          (callback_cnf_ptr->client_req_cb)(cnf_status,
                                            cnf,
                                            cnf_ptr);
        }
      }
      break;

      case MMGSDI_SESSION_APP_RESET_DEACTIVATE_REQ:
      case MMGSDI_SESSION_APP_RESET_ACTIVATE_REQ:
      if(cnf_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = mmgsdi_util_get_prov_session_info(
                          session_id,
                          NULL,
                          &slot,
                          &sel_app_ptr);

        if((mmgsdi_status != MMGSDI_SUCCESS) || (sel_app_ptr == NULL))
        {
          UIM_MSG_ERR_1("Invalid Prov. session - Session ID=0x%x", session_id);
          return;
        }

        notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
        notify_type.slot_id     = slot;

        if(cnf == MMGSDI_SESSION_APP_RESET_DEACTIVATE_REQ)
        {
          activate = FALSE;
        }
        else
        {
          activate = TRUE;
        }
        mmgsdi_evt_build_and_send_session_changed(
          notify_type, activate, (const mmgsdi_int_app_info_type*)sel_app_ptr);
      }
      break;

    case MMGSDI_CLIENT_ID_AND_EVT_REG_REQ:
      if(cnf_status == MMGSDI_SUCCESS)
      {
        mmgsdi_status = mmgsdi_util_get_client_id_index(
                                         cnf_ptr->response_header.client_id,
                                         &client_index);
        if (mmgsdi_status == MMGSDI_SUCCESS)
        {
          /* Set the event reg cnf sent as true for clients
             so that events can be sent */
          mmgsdi_client_id_reg_table[client_index]->
                                    is_client_evt_reg_cnf_sent = TRUE;
          for(slot_id = MMGSDI_SLOT_1; slot_id <= MMGSDI_SLOT_3; slot_id++)
          {
            slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);
          /* If client has not been notified yet, send out the event */
            if (slot_data_ptr != NULL)
            {
              mmgsdi_evt_build_and_notify_card_evt_from_state(
                 slot_id, client_index);
            }
          }
        }
      }
      break;

    case MMGSDI_SESSION_ENABLE_SERVICE_REQ:
      if((cnf_ptr->session_enable_service_cnf.service_type == MMGSDI_CDMA_SRV_FDN) ||
         (cnf_ptr->session_enable_service_cnf.service_type == MMGSDI_GSM_SRV_FDN)  ||
         (cnf_ptr->session_enable_service_cnf.service_type == MMGSDI_USIM_SRV_FDN) ||
         (cnf_ptr->session_enable_service_cnf.service_type == MMGSDI_CSIM_SRV_FDN))
      {
        if((cnf_status == MMGSDI_SUCCESS) &&
           (cnf_ptr->session_enable_service_cnf.is_status_same == FALSE))
        {
          mmgsdi_status = mmgsdi_util_get_session_app_info(
                            session_id,
                            &slot,
                            NULL,
                            &sel_app_ptr,
                            &channel_info_ptr,
                            &session_info_ptr,
                            &app_index);

          if((mmgsdi_status != MMGSDI_SUCCESS) || (sel_app_ptr == NULL) ||
             (session_info_ptr == NULL) || (channel_info_ptr == NULL))
          {
            UIM_MSG_ERR_3("Invalid session - session_info_p=0x%x,chnnl_info_p=0x%x,app_info_p=0x%x",
                          session_info_ptr, channel_info_ptr, sel_app_ptr);
            return;
          }
          notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
          notify_type.slot_id     = slot;
          mmgsdi_evt_build_and_send_fdn(
            notify_type, TRUE, (const mmgsdi_int_app_info_type*)sel_app_ptr,
            0,NULL);
        }
      }
      break;

    case MMGSDI_SESSION_DISABLE_SERVICE_REQ:
      if((cnf_ptr->session_disable_service_cnf.service_type == MMGSDI_CDMA_SRV_FDN) ||
         (cnf_ptr->session_disable_service_cnf.service_type == MMGSDI_GSM_SRV_FDN)  ||
         (cnf_ptr->session_disable_service_cnf.service_type == MMGSDI_USIM_SRV_FDN) ||
         (cnf_ptr->session_disable_service_cnf.service_type == MMGSDI_CSIM_SRV_FDN))
      {
        if((cnf_status == MMGSDI_SUCCESS) &&
          (cnf_ptr->session_disable_service_cnf.is_status_same == FALSE))
        {
          mmgsdi_status = mmgsdi_util_get_session_app_info(
                            session_id,
                            &slot,
                            NULL,
                            &sel_app_ptr,
                            &channel_info_ptr,
                            &session_info_ptr,
                            &app_index);

          if((mmgsdi_status != MMGSDI_SUCCESS) || (sel_app_ptr == NULL) ||
             (session_info_ptr == NULL) || (channel_info_ptr == NULL))
          {
            UIM_MSG_ERR_3("Invalid session - session_info_p=0x%x,chnnl_info_p=0x%x,app_info_p=0x%x",
                          session_info_ptr, channel_info_ptr, sel_app_ptr);
            return;
          }
          notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
          notify_type.slot_id     = slot;
          mmgsdi_evt_build_and_send_fdn(
            notify_type, FALSE, (const mmgsdi_int_app_info_type*)sel_app_ptr,
            0,NULL);
        }
      }
      break;

    case MMGSDI_SET_SIM_PROFILE_REQ:
#ifdef FEATURE_UIM_MULTI_PROFILE
      /* Profile switching has started. Send profile switching event to clients */
      if(cnf_status == MMGSDI_SUCCESS && mmgsdi_profile_info_ptr != NULL)
      {
        mmgsdi_evt_build_and_send_profile_switching_evt(
          TRUE,
          mmgsdi_profile_info_ptr->active_profile,
          cnf_ptr->set_sim_profile_cnf.response_header.client_id,
          cnf_ptr->set_sim_profile_cnf.response_header.slot_id);
      }
#endif /* FEATURE_UIM_MULTI_PROFILE */
      break;

    default:
      return;
  }
}/* mmgsdi_cnf_generate_rsp_and_evt */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SEARCH_DATA

   DESCRIPTION:
     This function build the search confirmation's specific data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS : The command processing was successful.
     MMGSDI_ERROR   : The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_search_data(
  mmgsdi_search_cnf_type                   * search_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const mmgsdi_data_type                  * data_search_ptr
)
{
  mmgsdi_return_enum_type  build_cnf_status    = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL_3(extra_param_ptr, search_cnf_ptr, data_search_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* Copy the response data */
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      search_cnf_ptr->searched_record_nums.data_ptr,
      data_search_ptr->data_len);

    /* even if can't malloc for read data, still want to populate
       the cnf header to notify client */
    if (search_cnf_ptr->searched_record_nums.data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    mmgsdi_memscpy(search_cnf_ptr->searched_record_nums.data_ptr,
                   int32touint32(data_search_ptr->data_len),
                   data_search_ptr->data_ptr,
                   int32touint32(data_search_ptr->data_len));
    search_cnf_ptr->searched_record_nums.data_len = data_search_ptr->data_len;

  } /* End of if (mmgsdi_status == MMGSDI_SUCCESS) */

  /* Copy the access data info from request pointer to confirmation pointer */
  build_cnf_status = mmgsdi_util_copy_access_type(&search_cnf_ptr->access,
                                                  &extra_param_ptr->search_data.access);

  if (build_cnf_status != MMGSDI_SUCCESS)
  {
    /* Free the search data */
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(search_cnf_ptr->searched_record_nums.data_ptr);
    MMGSDIUTIL_DEBUG_MSG_ERROR_1("Copy Access type failed in read 0x%x",
                                 build_cnf_status);
  }

  return build_cnf_status;

}/* mmgsdi_cnf_build_search_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SESSION_GET_INFO_DATA

   DESCRIPTION:
     This function builds the Cnf data for session get info command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_cnf_build_session_get_info_data(
  mmgsdi_session_get_info_cnf_type              *session_get_info_cnf_ptr,
  mmgsdi_return_enum_type                       *mmgsdi_status_ptr,
  const mmgsdi_session_get_info_extra_info_type *session_get_info_data_ptr)
{
  MMGSDIUTIL_RETURN_IF_NULL_3(session_get_info_cnf_ptr,
                              mmgsdi_status_ptr,
                              session_get_info_data_ptr);

  if (*mmgsdi_status_ptr != MMGSDI_SUCCESS)
  {
    return MMGSDI_SUCCESS;
  }

  do
  {
    mmgsdi_session_info_type   *session_info_ptr  = NULL;
    mmgsdi_channel_info_type   *channel_info_ptr  = NULL;
    mmgsdi_int_app_info_type   *app_info_ptr      = NULL;
    mmgsdi_session_id_type      mmgsdi_session_id = MMGSDI_SESSION_ID_ZERO;

    if(session_get_info_data_ptr->query_type == MMGSDI_SESSION_INFO_QUERY_BY_ID)
    {
      mmgsdi_session_id = session_get_info_data_ptr->query_session_id;
    }
    else if(session_get_info_data_ptr->query_type ==
                                          MMGSDI_SESSION_INFO_QUERY_BY_TYPE)
    {
      if (MMGSDI_SUCCESS != mmgsdi_util_get_mmgsdi_session_id_from_provisioning_type(
                              session_get_info_data_ptr->query_session_type,
                              &mmgsdi_session_id))
      {
         UIM_MSG_ERR_1("Invalid Session type requested=0x%x ",
                       session_get_info_data_ptr->query_session_type);
         *mmgsdi_status_ptr = MMGSDI_INCORRECT_PARAMS;
         break;
      }
    }
    else
    {
      UIM_MSG_ERR_1("Invalid Session Query type=0x%x ",
                    session_get_info_data_ptr->query_type);
      *mmgsdi_status_ptr = MMGSDI_INCORRECT_PARAMS;
      break;
    }
      /* Get app info corresponding to the Session ID */
    *mmgsdi_status_ptr = mmgsdi_util_get_session_app_info(mmgsdi_session_id,
                                                          NULL,
                                                          NULL,
                                                          &app_info_ptr,
                                                          &channel_info_ptr,
                                                          &session_info_ptr,
                                                          NULL);
  
    if(*mmgsdi_status_ptr != MMGSDI_SUCCESS ||
       session_info_ptr == NULL ||
       channel_info_ptr == NULL ||
       app_info_ptr == NULL)
    {
      UIM_MSG_ERR_0("Session Info unavailable");
      *mmgsdi_status_ptr = MMGSDI_ERROR;
      break;
    }
    session_get_info_cnf_ptr->slot_id = channel_info_ptr->slot_id;
    session_get_info_cnf_ptr->channel_id = channel_info_ptr->channel_id;
    session_get_info_cnf_ptr->session_type = session_info_ptr->session_type;
    mmgsdi_memscpy(&(session_get_info_cnf_ptr->app_data),
                   sizeof(session_get_info_cnf_ptr->app_data),
                   &app_info_ptr->app_data,
                   sizeof(mmgsdi_aid_type));
    session_get_info_cnf_ptr->app_state = app_info_ptr->app_state;
  }while(0);

  return MMGSDI_SUCCESS;
}/* mmgsdi_cnf_build_session_get_info_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SESSION_GET_APP_CAPABILITIES_DATA

   DESCRIPTION:
     This function builds the Cnf data for session get app capabilities command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
static mmgsdi_return_enum_type mmgsdi_cnf_build_session_get_app_capabilities_data(
  mmgsdi_slot_id_enum_type                  slot_id,
  mmgsdi_session_app_capabilities_cnf_type *session_get_app_capabilities_cnf_ptr,
  mmgsdi_session_id_type                    session_id)
{

  mmgsdi_return_enum_type      mmgsdi_status        = MMGSDI_ERROR;
  mmgsdi_int_app_info_type    *app_info_ptr         = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(session_get_app_capabilities_cnf_ptr);

  mmgsdi_status =  mmgsdi_util_get_prov_session_info(session_id,
                                                     NULL,
                                                     NULL,
                                                     &app_info_ptr);
  if(mmgsdi_status != MMGSDI_SUCCESS || app_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if(app_info_ptr->app_capabilities.fdn_state == MMGSDI_SRV_ENABLE)
  {
    session_get_app_capabilities_cnf_ptr->fdn_enabled = TRUE;
  }
  else
  {
    session_get_app_capabilities_cnf_ptr->fdn_enabled = FALSE;
  }

  if(app_info_ptr->app_capabilities.bdn_state == MMGSDI_SRV_ENABLE)
  {
    session_get_app_capabilities_cnf_ptr->bdn_enabled = TRUE;
  }
  else
  {
    session_get_app_capabilities_cnf_ptr->bdn_enabled = FALSE;
  }

  if(app_info_ptr->app_capabilities.acl_state == MMGSDI_SRV_ENABLE)
  {
    session_get_app_capabilities_cnf_ptr->acl_enabled = TRUE;
  }
  else
  {
    session_get_app_capabilities_cnf_ptr->acl_enabled = FALSE;
  }

  session_get_app_capabilities_cnf_ptr->imsi_invalidated =
    app_info_ptr->app_capabilities.imsi_invalidated;
  return MMGSDI_SUCCESS;
}/*mmgsdi_cnf_build_session_get_app_capabilities_data*/


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SESSION_ENABLE_SERVICE_DATA

   DESCRIPTION:
     This function builds the CNF data for Enable service request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:    The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
 mmgsdi_return_enum_type mmgsdi_cnf_build_session_enable_service_data(
   mmgsdi_return_enum_type                  mmgsdi_status,
   mmgsdi_session_enable_service_cnf_type  *session_enable_service_cnf_ptr,
   const mmgsdi_session_enable_service_extra_info_type
                                           *session_enable_service_data_ptr)
{
  mmgsdi_return_enum_type  build_cnf_status    = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(session_enable_service_data_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    session_enable_service_cnf_ptr->response_header.client_data =
       session_enable_service_data_ptr->client_data;
    session_enable_service_cnf_ptr->service_type =
       session_enable_service_data_ptr->service_type;
    session_enable_service_cnf_ptr->is_status_same =
       session_enable_service_data_ptr->is_status_same;
  }

  return build_cnf_status;
}/* mmgsdi_cnf_build_session_enable_service_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SESSION_DISABLE_SERVICE_DATA

   DESCRIPTION:
     This function builds the CNF data for Disable service request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:    The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
 mmgsdi_return_enum_type mmgsdi_cnf_build_session_disable_service_data(
   mmgsdi_return_enum_type                  mmgsdi_status,
   mmgsdi_session_disable_service_cnf_type *session_disable_service_cnf_ptr,
   const mmgsdi_session_enable_service_extra_info_type
                                           *session_disable_service_data_ptr)
{
  mmgsdi_return_enum_type  build_cnf_status    = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(session_disable_service_data_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    session_disable_service_cnf_ptr->response_header.client_data =
       session_disable_service_data_ptr->client_data;
    session_disable_service_cnf_ptr->service_type =
       session_disable_service_data_ptr->service_type;
    session_disable_service_cnf_ptr->is_status_same =
       session_disable_service_data_ptr->is_status_same;
  }

  return build_cnf_status;
}/* mmgsdi_cnf_build_session_disable_service_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_EONS_DATA

   DESCRIPTION:
     This function builds the CNF data for get operator name request

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:    The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
 mmgsdi_return_enum_type mmgsdi_cnf_build_get_eons_data(
   mmgsdi_session_get_operator_name_cnf_type *session_get_eons_cnf_ptr,
   mmgsdi_return_enum_type                    mmgsdi_status,
   const mmgsdi_client_req_extra_info_type   *extra_get_eons_data_ptr,
   mmgsdi_session_id_type                     session_id)
{
  mmgsdi_return_enum_type       build_mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_eons_display_info_type display_info;

  memset(&display_info, 0x00, sizeof(mmgsdi_eons_display_info_type));
  MMGSDIUTIL_RETURN_IF_NULL_2(session_get_eons_cnf_ptr,
                              extra_get_eons_data_ptr);

  /* In case of a failure, do not run the algo. This can happen, if any
     of the input parameters (e.g. CSG ID) are not set correctly. */
  /* The extra_get_eons_data_ptr is released in mmgsdi_process_response() */
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return build_mmgsdi_status;
  }

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  build_mmgsdi_status = mmgsdi_eons_run_algo(session_id,
                          MMGSDI_EONS_TRIGGER_API_CALL,
                          extra_get_eons_data_ptr->session_get_eons_data.plmn_id_list,
                          extra_get_eons_data_ptr->session_get_eons_data.ignore_display_condition,
                          &(session_get_eons_cnf_ptr->spn),
                          &(session_get_eons_cnf_ptr->spn_lang_ind),
                          &(session_get_eons_cnf_ptr->plmn_list),
                          &display_info);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  if(build_mmgsdi_status == MMGSDI_SUCCESS)
  {
    session_get_eons_cnf_ptr->spn_display_bit = display_info.spn_display_bit;
    session_get_eons_cnf_ptr->rplmn_display_bit =
      display_info.rplmn_display_bit;
    session_get_eons_cnf_ptr->roaming_status = display_info.roaming_status;
  }

  return build_mmgsdi_status;
} /* mmgsdi_cnf_build_get_eons_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_SE13_PLMN_NAMES_DATA

   DESCRIPTION:
     This function builds the CNF data for get SE13 plmn names request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:    The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_get_se13_plmn_names_data(
  mmgsdi_get_se13_plmn_names_cnf_type       *get_se13_plmn_names_cnf_ptr,
  mmgsdi_return_enum_type                    mmgsdi_status,
  const mmgsdi_client_req_extra_info_type   *extra_get_se13_plmn_names_data_ptr
)
{
  mmgsdi_return_enum_type build_mmgsdi_status = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL_2(get_se13_plmn_names_cnf_ptr,
                              extra_get_se13_plmn_names_data_ptr);

  build_mmgsdi_status = mmgsdi_eons_get_se13_plmn_names(
                          extra_get_se13_plmn_names_data_ptr->get_se13_plmn_names_data.plmn_id_list,
                          &(get_se13_plmn_names_cnf_ptr->plmn_list));

  return build_mmgsdi_status;
}/* mmgsdi_cnf_build_get_se13_plmn_names_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GSTK_LOC_ENV_DATA

   DESCRIPTION:
     This function builds the location env response confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_gstk_loc_env_data(
  mmgsdi_gstk_loc_env_rsp_received_cnf_type * gstk_loc_env_cnf_ptr,
  const mmgsdi_client_req_extra_info_type   * extra_param_ptr)
{
  MMGSDIUTIL_RETURN_IF_NULL_2(gstk_loc_env_cnf_ptr, extra_param_ptr);

  gstk_loc_env_cnf_ptr->imsi_switched = extra_param_ptr->gstk_loc_env_data.imsi_switched;

  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_gstk_loc_env_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_SE13_PLMN_INFO_BY_NAME_DATA

   DESCRIPTION:
     This function builds the CNF data for get SE13 plmn info by name request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:    The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_get_se13_plmn_info_by_name_data(
  mmgsdi_get_se13_plmn_info_by_name_cnf_type       *get_se13_plmn_info_by_name_cnf_ptr,
  const mmgsdi_get_se13_plmn_info_by_name_req_type *get_se13_plmn_info_by_name_req_ptr
)
{
  mmgsdi_return_enum_type build_mmgsdi_status = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL_2(get_se13_plmn_info_by_name_cnf_ptr,
                              get_se13_plmn_info_by_name_req_ptr);

  build_mmgsdi_status = mmgsdi_eons_get_se13_plmn_info_by_name(
                          get_se13_plmn_info_by_name_req_ptr,
                          get_se13_plmn_info_by_name_cnf_ptr);

  return build_mmgsdi_status;
}/* mmgsdi_cnf_build_get_se13_plmn_info_by_name_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SET_BUILTIN_PLMN_LIST_DATA

   DESCRIPTION:
     This function builds the CNF data for set builtin plmn list request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:    The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_set_builtin_plmn_list_data(
  const mmgsdi_set_builtin_plmn_list_req_type      *set_builtin_plmn_list_req_ptr
)
{
  mmgsdi_return_enum_type build_mmgsdi_status = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(set_builtin_plmn_list_req_ptr);

  build_mmgsdi_status = mmgsdi_store_builtin_plmn_list(
                          &set_builtin_plmn_list_req_ptr->builtin_plmn_list);

  return build_mmgsdi_status;
}/* mmgsdi_cnf_build_set_builtin_plmn_list_data */


#ifdef FEATURE_UIM_MULTI_PROFILE
/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_SIM_PROFILE_DATA

   DESCRIPTION:
     This function builds the CNF data for get sim profile command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_get_sim_profile_data(
  mmgsdi_get_sim_profile_cnf_type  *get_sim_profile_cnf_ptr,
  mmgsdi_return_enum_type           mmgsdi_status)
{
  mmgsdi_return_enum_type build_cnf_status = MMGSDI_SUCCESS;

  MMGSDIUTIL_RETURN_IF_NULL(get_sim_profile_cnf_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS && mmgsdi_profile_info_ptr != NULL)
  {
    get_sim_profile_cnf_ptr->num_profile = mmgsdi_profile_info_ptr->num_profile;

    /* Copy the active profile information */
    get_sim_profile_cnf_ptr->active_profile = mmgsdi_profile_info_ptr->active_profile;

    /* Copy the profile information */
    mmgsdi_memscpy(get_sim_profile_cnf_ptr->profile_list,
                   sizeof(get_sim_profile_cnf_ptr->profile_list),
                   mmgsdi_profile_info_ptr->profile_data,
                   mmgsdi_profile_info_ptr->num_profile * sizeof(mmgsdi_profile_info_type));
  }
  return build_cnf_status;
}/*mmgsdi_cnf_build_get_sim_profile_data*/
#endif /* FEATURE_UIM_MULTI_PROFILE */
