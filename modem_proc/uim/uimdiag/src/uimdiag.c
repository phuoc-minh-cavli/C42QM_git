/*===========================================================================

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2009 - 2018 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/uim.mpss/3.3.1/uimdiag/src/uimdiag.c#1 $$ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who   what, where, why
--------   ---   -----------------------------------------------------------
10/31/18   tq    Removal of MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_REQ
09/10/17   rps   Build flag for Simlock in IOE devices
05/22/16   vdc   Remove F3 messages for memory allocation failure
07/01/15   bcho  Support for whitelist and blacklist codes in same category
07/10/14   tl    Remove ALWAYS OFF feature FEATURE_MMGSDI_PKCS15
06/12/14   vv    Remove FEATURE_SIMLOCK
03/18/14   tl    Introduce new SIM Lock feature
01/27/14   df    Featurise file with FEATURE_UIMDIAG
01/10/14   df    Use v2.0 diag macros
12/11/13   df    Remove unused memory paging feature
11/05/13   df    Remove unnecessary include tmc.h
05/06/13   sw    Changes made for secure version of memcpy
04/24/13   rr    Featurized optional logs
11/09/12   yt    Remove parsing of length field for PERSO_GET_DATA_CMD
09/25/12   tl    Adding nvruim_data_req_enum_type to sessionid_type
11/17/11   kk    Added get all available apps async support
10/24/11   adp   Removing support for UIMDIAG_MMGSDI_SAP_PIN_VERIFY_CMD,
                 UIMDIAG_MMGSDI_SAP_INITIALIZE_CMD and
                 UIMDIAG_MMGSDI_SAP_DEREGISTER_CMD
10/18/11   bcho  Adding support for mmgsdi_session_open_ext API
10/04/11   adp   Adding support for is service available sync cmd
09/29/11   kk    Added support for get all available apps sync cmd
10/14/11   bcho  Added support for Get Operator Name API
09/23/11   nb    Support for SAP CONNECT/SAP DISCONNECT
08/05/11   adp   Adding support to trigger refresh in uimdiag
09/15/11   nb    Support for new refresh stage
08/22/11   av    Added support for pdown,pup,activate onchip,send apdu commands
04/26/11   yt    Replaced references to TMC heap with Modem heap
12/29/10   ms    Adding support for BT SAP APIs
12/03/10   adp   Adding support for a number of commands
12/22/10   yt    Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
11/30/10   yt    Including private UIMDRV header
09/07/10   adp   Adding Support for dck_num_retries,lock_down_enable_ltd_access,
                 seek and search commands
08/09/10   adp   Adding support for service enable, disable, get perso key,
                 is service available, perso unblock.
06/09/10   adp   Fixing Compiler Warnings
04/07/10   yt    Fixed compilation warnings
03/25/10   adp   Adding support for mmgsdi_session_deactivate_provisioning,
                 mmgsdi_session_get_app_capabilities,get_cphs_info
                 isim_authenticate,send_card_status,
                 create_pkcs15_lookup_table
03/24/10   adp   Fixing Compiler Warnings, the function call to check whether
                 the pointer is in the queue or not is not needed for uimdiag
03/19/10   rk    Added support for JCDMA get info
02/22/10   adp   Adding support for Perso Commands
02/04/10   yt    Fixed compilation warnings
12/27/09   ap    Adding support for Refresh Commands
12/17/09   rk    Fix for uimdiag small heap memory alloction issue
12/03/09   rk    Adding support for get file attributes, read and write commands
10/19/09   adp   Adding support for pin commands
10/18/09   adp   Moving MMGSDI specific util functions to uimdiag_mmgsdi_util.c
                 Adding support for Activate_or_switch_prov, session_get_info
                 Adding functionality for uimdiag_free
09/02/09   adp   Initial Revision -Added initial mmgsdi session based commands


=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"
#include "diagcmd.h"
#include "uimdiag.h"
#include "uimdiag_mmgsdi.h"
#include "uimdiag_simlock.h"
#include "uimdiag_mmgsdi_util.h"
#include "uim_msg.h"
#include "uimdiag_util.h"
#include "uim_p.h"
#include "modem_mem.h"

/*=============================================================================

                       GLOBAL DECLARATIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   UIMDIAG_SUBSYS_DELAYED_TBL

   DESCRITPION:
     Table contains uimdiag subsystem command ids and corresponding
     call back functions. This table is used during uimdiag initialization
     for registering

   ------------------------------------------------------------------------- */
static const diagpkt_user_table_entry_type uimdiag_subsys_delayed_tbl[] =
{
  {(word)UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_REG_CMD,
   (word)UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_REG_CMD,
   uimdiag_mmgsdi_client_id_and_evt_reg_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_OPEN_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_OPEN_CMD,
   uimdiag_mmgsdi_session_open_cmd },
  {(word)UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_DEREG_CMD,
   (word)UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_DEREG_CMD,
   uimdiag_mmgsdi_client_id_and_evt_dereg_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_CLOSE_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_CLOSE_CMD,
   uimdiag_mmgsdi_session_close_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_ACT_OR_SW_PROV_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_ACT_OR_SW_PROV_CMD,
   uimdiag_mmgsdi_session_act_or_sw_prov_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_GET_INFO_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_GET_INFO_CMD,
   uimdiag_mmgsdi_session_get_info_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_GET_ALL_PIN_STATUS_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_GET_ALL_PIN_STATUS_CMD,
   uimdiag_mmgsdi_session_get_all_pin_status_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_VERIFY_PIN_CMD,
   (word) UIMDIAG_MMGSDI_SESSION_VERIFY_PIN_CMD,
   uimdiag_mmgsdi_session_verify_pin_cmd },
  {(word) UIMDIAG_MMGSDI_SESSION_DISABLE_PIN_CMD,
   (word) UIMDIAG_MMGSDI_SESSION_DISABLE_PIN_CMD,
   uimdiag_mmgsdi_session_disable_pin_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_ENABLE_PIN_CMD,
   (word) UIMDIAG_MMGSDI_SESSION_ENABLE_PIN_CMD,
   uimdiag_mmgsdi_session_enable_pin_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_CHANGE_PIN_CMD,
   (word) UIMDIAG_MMGSDI_SESSION_CHANGE_PIN_CMD,
   uimdiag_mmgsdi_session_change_pin_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_UNBLOCK_PIN_CMD,
   (word) UIMDIAG_MMGSDI_SESSION_UNBLOCK_PIN_CMD,
   uimdiag_mmgsdi_session_unblock_pin_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_GET_FILE_ATTR_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_GET_FILE_ATTR_CMD,
   uimdiag_mmgsdi_session_get_file_attr_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_READ_RECORD_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_READ_RECORD_CMD,
   uimdiag_mmgsdi_session_read_record_cmd },
  {(word)UIMDIAG_MMGSDI_SESSION_READ_TRANSPARENT_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_READ_TRANSPARENT_CMD,
   uimdiag_mmgsdi_session_read_transparent_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_WRITE_RECORD_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_WRITE_RECORD_CMD,
   uimdiag_mmgsdi_session_write_record_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_WRITE_TRANSPARENT_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_WRITE_TRANSPARENT_CMD,
   uimdiag_mmgsdi_session_write_transparent_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_REG_FOR_REFRESH_CMD ,
    (word)UIMDIAG_MMGSDI_SESSION_REG_FOR_REFRESH_CMD,
     uimdiag_mmgsdi_session_reg_for_refresh_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_DEREG_FOR_REFRESH_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_DEREG_FOR_REFRESH_CMD,
   uimdiag_mmgsdi_session_dereg_for_refresh_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_OK_TO_REFRESH_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_OK_TO_REFRESH_CMD,
   uimdiag_mmgsdi_session_ok_to_refresh_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_REFRESH_COMPLETE_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_REFRESH_COMPLETE_CMD,
   uimdiag_mmgsdi_session_refresh_complete_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_DEACT_PROV_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_DEACT_PROV_CMD,
   uimdiag_mmgsdi_session_deact_prov_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_GET_APP_CAP_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_GET_APP_CAP_CMD,
   uimdiag_mmgsdi_session_get_app_cap_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_GET_CPHS_INFO_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_GET_CPHS_INFO_CMD,
   uimdiag_mmgsdi_session_get_cphs_info_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_ISIM_AUTH_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_ISIM_AUTH_CMD,
   uimdiag_mmgsdi_session_isim_auth_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_SEND_CARD_STATUS_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_SEND_CARD_STATUS_CMD,
   uimdiag_mmgsdi_session_send_card_status_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_DISABLE_SERVICE_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_DISABLE_SERVICE_CMD,
   uimdiag_mmgsdi_session_disable_service_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_ENABLE_SERVICE_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_ENABLE_SERVICE_CMD,
   uimdiag_mmgsdi_session_enable_service_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_IS_SERVICE_AVAIL_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_IS_SERVICE_AVAIL_CMD,
   uimdiag_mmgsdi_session_is_service_avail_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_SEEK_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_SEEK_CMD,
   uimdiag_mmgsdi_session_seek_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_SEARCH_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_SEARCH_CMD,
   uimdiag_mmgsdi_session_search_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_COMPUTE_IP_AUTH_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_COMPUTE_IP_AUTH_CMD,
   uimdiag_mmgsdi_session_compute_ip_auth_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_SSD_UPDATE_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_SSD_UPDATE_CMD,
   uimdiag_mmgsdi_session_ssd_update_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_SSD_CONFIRM_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_SSD_CONFIRM_CMD,
   uimdiag_mmgsdi_session_confirm_ssd_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_BS_CHAL_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_BS_CHAL_CMD,
   uimdiag_mmgsdi_session_bs_chal_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_READ_PRL_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_READ_PRL_CMD,
   uimdiag_mmgsdi_session_read_prl_cmd},
   {(word)UIMDIAG_MMGSDI_CHECK_DIAG_SUPPORT_CMD ,
   (word)UIMDIAG_MMGSDI_CHECK_DIAG_SUPPORT_CMD,
   uimdiag_check_diag_support_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_GET_INFO_SYNC_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_GET_INFO_SYNC_CMD,
   uimdiag_mmgsdi_session_get_info_sync_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_READ_CACHE_FILE_SIZE_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_READ_CACHE_FILE_SIZE_CMD,
   uimdiag_mmgsdi_session_read_cache_file_size_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_READ_CACHE_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_READ_CACHE_CMD,
   uimdiag_mmgsdi_session_read_cache_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_WRITE_CACHE_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_WRITE_CACHE_CMD,
   uimdiag_mmgsdi_session_write_cache_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_SYNC_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_SYNC_CMD,
   uimdiag_mmgsdi_session_illegal_subscription_sync_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_TOGGLE_UIM_PWR_CTRL_SYNC_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_TOGGLE_UIM_PWR_CTRL_SYNC_CMD,
   uimdiag_mmgsdi_session_toggle_uim_pwr_ctrl_sync_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_GET_APP_CAPABILITIES_SYNC_CMD ,
   (word)UIMDIAG_MMGSDI_SESSION_GET_APP_CAPABILITIES_SYNC_CMD,
   uimdiag_mmgsdi_session_get_app_capabilities_sync_cmd},
   {(word)UIMDIAG_MMGSDI_SAP_GET_ATR_CMD,
   (word)UIMDIAG_MMGSDI_SAP_GET_ATR_CMD,
   uimdiag_mmgsdi_sap_get_atr_cmd},
   {(word)UIMDIAG_MMGSDI_SAP_SEND_APDU_CMD,
   (word)UIMDIAG_MMGSDI_SAP_SEND_APDU_CMD,
   uimdiag_mmgsdi_sap_send_apdu_cmd},
   {(word)UIMDIAG_MMGSDI_SAP_POWER_ON_CMD,
   (word)UIMDIAG_MMGSDI_SAP_POWER_ON_CMD,
   uimdiag_mmgsdi_sap_power_on_cmd},
   {(word)UIMDIAG_MMGSDI_SAP_POWER_OFF_CMD,
   (word)UIMDIAG_MMGSDI_SAP_POWER_OFF_CMD,
   uimdiag_mmgsdi_sap_power_off_cmd},
   {(word)UIMDIAG_MMGSDI_SAP_CARD_READER_STATUS_CMD,
   (word)UIMDIAG_MMGSDI_SAP_CARD_READER_STATUS_CMD,
   uimdiag_mmgsdi_sap_card_reader_status_cmd},
   {(word)UIMDIAG_MMGSDI_SAP_RESET_CMD,
   (word)UIMDIAG_MMGSDI_SAP_RESET_CMD,
   uimdiag_mmgsdi_sap_reset_cmd},
   {(word)UIMDIAG_MMGSDI_SAP_CONNECT_CMD,
   (word)UIMDIAG_MMGSDI_SAP_CONNECT_CMD,
   uimdiag_mmgsdi_sap_connect_cmd},
   {(word)UIMDIAG_MMGSDI_SAP_DISCONNECT_CMD,
   (word)UIMDIAG_MMGSDI_SAP_DISCONNECT_CMD,
   uimdiag_mmgsdi_sap_disconnect_cmd},
   {(word)UIMDIAG_MMGDDI_SESSION_REGISTER_FOR_REFRESH_PROCEED_VOTING_CMD,
   (word)UIMDIAG_MMGDDI_SESSION_REGISTER_FOR_REFRESH_PROCEED_VOTING_CMD,
   uimdiag_mmgsdi_session_reg_for_refresh_proceed_voting_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_CMD,
   uimdiag_mmgsdi_session_ok_to_proceed_with_refresh_cmd},
   {(word)UIMDIAG_MMGSDI_NAA_REFRESH_CMD,
   (word)UIMDIAG_MMGSDI_NAA_REFRESH_CMD,
   uimdiag_mmgsdi_naa_refresh_cmd},
   {(word)UIMDIAG_MMGSDI_CARD_PDOWN_CMD,
   (word)UIMDIAG_MMGSDI_CARD_PDOWN_CMD,
   uimdiag_mmgsdi_card_pdown_cmd},
   {(word)UIMDIAG_MMGSDI_CARD_PUP_CMD,
   (word)UIMDIAG_MMGSDI_CARD_PUP_CMD,
   uimdiag_mmgsdi_card_pup_cmd},
   {(word)UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_SIM_CMD,
   (word)UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_SIM_CMD,
   uimdiag_mmgsdi_activate_onchip_sim_cmd},
   {(word)UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_USIM_CMD,
   (word)UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_USIM_CMD,
   uimdiag_mmgsdi_activate_onchip_usim_cmd},
   {(word)UIMDIAG_MMGSDI_SEND_APDU_CMD,
   (word)UIMDIAG_MMGSDI_SEND_APDU_CMD,
   uimdiag_mmgsdi_send_apdu_cmd},
   {(word)UIMDIAG_MMGSDI_GET_ALL_AVAILABLE_APPS_CMD,
   (word)UIMDIAG_MMGSDI_GET_ALL_AVAILABLE_APPS_CMD,
   uimdiag_mmgsdi_get_all_available_apps_async_cmd},
   {(word)UIMDIAG_MMGSDI_GET_ALL_AVAIL_APP_SYNC_CMD ,
   (word)UIMDIAG_MMGSDI_GET_ALL_AVAIL_APP_SYNC_CMD,
   uimdiag_mmgsdi_get_all_available_apps_sync_cmd},
   {(word)UIMDIAG_MMGSDI_IS_SERVICE_AVAILABLE_SYNC_CMD,
   (word)UIMDIAG_MMGSDI_IS_SERVICE_AVAILABLE_SYNC_CMD,
   uimdiag_mmgsdi_is_service_available_sync_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_GET_OPERATOR_NAME_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_GET_OPERATOR_NAME_CMD,
   uimdiag_mmgsdi_session_get_operator_name_table_cmd},
   {(word)UIMDIAG_MMGSDI_SESSION_OPEN_EXT_CMD,
    (word)UIMDIAG_MMGSDI_SESSION_OPEN_EXT_CMD,
    uimdiag_mmgsdi_session_open_ext_cmd },
   {(word)UIMDIAG_MMGSDI_SESSION_GET_NVRUIM_DATA_SYNC_CMD,
   (word)UIMDIAG_MMGSDI_SESSION_GET_NVRUIM_DATA_SYNC_CMD,
   uimdiag_mmgsdi_session_get_nvruim_data_sync_cmd},
   {(word)UIMDIAG_SIMLOCK_SET_POLICY_CMD,
   (word)UIMDIAG_SIMLOCK_SET_POLICY_CMD,
   uimdiag_simlock_set_policy_cmd },
   {(word)UIMDIAG_SIMLOCK_CK_LOCK_CMD,
   (word)UIMDIAG_SIMLOCK_CK_LOCK_CMD,
   uimdiag_simlock_ck_lock_cmd },
   {(word)UIMDIAG_SIMLOCK_HCK_LOCK_CMD,
   (word)UIMDIAG_SIMLOCK_HCK_LOCK_CMD,
   uimdiag_simlock_hck_lock_cmd },
   {(word)UIMDIAG_SIMLOCK_UNLOCK_CMD,
   (word)UIMDIAG_SIMLOCK_UNLOCK_CMD,
   uimdiag_simlock_unlock_cmd },
   {(word)UIMDIAG_SIMLOCK_GET_STATUS_CMD,
   (word)UIMDIAG_SIMLOCK_GET_STATUS_CMD,
   uimdiag_simlock_get_status_cmd },
   {(word)UIMDIAG_SIMLOCK_GET_CATEGORY_DATA_CMD,
   (word)UIMDIAG_SIMLOCK_GET_CATEGORY_DATA_CMD,
   uimdiag_simlock_get_category_data_cmd },
   {(word)UIMDIAG_SIMLOCK_ADD_LOCK_CODES_CK_CMD,
   (word)UIMDIAG_SIMLOCK_ADD_LOCK_CODES_CK_CMD,
   uimdiag_simlock_add_lock_codes_ck_cmd },
   {(word)UIMDIAG_SIMLOCK_ADD_LOCK_CODES_HCK_CMD,
   (word)UIMDIAG_SIMLOCK_ADD_LOCK_CODES_HCK_CMD,
   uimdiag_simlock_add_lock_codes_hck_cmd },
   {(word)UIMDIAG_SIMLOCK_GET_CATEGORY_DATA_EXT_CMD,
   (word)UIMDIAG_SIMLOCK_GET_CATEGORY_DATA_EXT_CMD,
   uimdiag_simlock_get_category_data_ext_cmd }
};

uimdiag_req_table_elem_type *uimdiag_req_table_ptr = NULL;

/*=============================================================================

                              FUNCTIONS

=============================================================================*/

/*===========================================================================
FUNCTION UIMDIAG_REQ_TABLE_INIT

DESCRIPTION
    Initializes the req_table to NULL.

ARGUMENTS
    None

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static void uimdiag_req_table_init(void)
{
  /* Initialize table. Set it to NULL */
  uint32 max_client_req_tbl_size          = UIMDIAG_MAX_CLIENT_REQ_TBL_SIZE;

  UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    uimdiag_req_table_ptr,
    (sizeof(uimdiag_req_table_elem_type) * max_client_req_tbl_size));
}/* uimdiag_req_table_init */


/*===========================================================================

FUNCTION UIMDIAG_INIT

DESCRIPTION
  The uimdiag init function initializes sub system commands to DIAG framework.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void uimdiag_init(void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY(
    DIAG_SUBSYS_CMD_VER_2_F,
    (uint8)DIAG_SUBSYS_UIM,
    uimdiag_subsys_delayed_tbl);
    uimdiag_req_table_init();
}/* uimdiag_init */

