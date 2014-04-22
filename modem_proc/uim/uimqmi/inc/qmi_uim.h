#ifndef QMI_UIM_H
#define QMI_UIM_H
/*===========================================================================

                         Q M I _ U I M . H

DESCRIPTION

 The Data Services QMI User Identity Module Service header file.

                        COPYRIGHT INFORMATION

Copyright (c) 2010, 2012 - 2016, 2018 - 2019 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/uim.mpss/3.3.1/uimqmi/inc/qmi_uim.h#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/06/19    vgd    GET EID for M2M cards where LPA is not supported
02/21/19    bcho   De-register QMI UIM service on task stop
10/15/18    tq     Migration of QMI UIM service to QCCI-QCSI
10/12/18    bcho   RCINIT Optimization
10/13/17    rps    Remove old  perso support
09/29/16    sn     Added support to retrieve retry attempts left for locking
08/10/16    ar     Support for setting terminal capability from AP
07/21/16    sp     Seperate ERA-GLONASS and SAP related QMI UIM functionality
07/19/16    bcho   Move refresh_info to heap in qmi_uim_global_type
06/29/16    vdc    Remove callback user data from qmi uim global
06/22/16    sp     Increase max no of non prov session
05/19/16    vdc    Split QMI UIM files for supporting IOE
05/16/16    bcho   Removal of encrypt-decrypt pin functionality for IOE devices
11/25/15    sp     Updated copyright
09/07/15    bcho   Migrate to mcfg EFS wrappers
08/27/15    bfv    Do not retry service registration if it fails once
05/01/15    vdc    Added support for QMI UIM GBA
03/25/15    stv    API to block card status events
09/30/14    kk     Enhancements to NV handling
08/29/14    tl     Add support for supply voltage command and indication
05/22/14    tl     Added support for recovery complete indications
12/24/13    am     Add NV protection to Recovery API
02/19/13    av     Merge mmgsi,gstk,qmiuim/cat conf files to mmgsdi.conf
02/14/13    tl     Move QMI UIM to MMGSDI context from QMI modem context
02/23/12    nmb    QMI QMUX Changes
08/05/10    tkl    QMI UIM & QMI CAT split from data package
01/14/10    mib    Initial check-in of QMI_UIM
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_p.h"
#include "qmi_uim_cat_common.h"
#include "simlock_modem_p.h"
#include "gstk_exp_v.h"
#include "gba_lib.h"
#include "mmgsdilib_common.h"
#include "qmi_uim_parsing.h"
#include "user_identity_module_v01.h"
#include "user_identity_module_impl_v01.h"
#include "ds_qmi_fw_common.h"
#include "qmi_csi.h"
#include "qmi_sap.h"

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/
#define QMI_UIM_CONFIG_GBA_FQDN_WHITE_LIST  "/nv/item_files/modem/qmi/uim/gba_fqdn_white_list"
#define QMI_UIM_CONFIG_GBA_FQDN_BLACK_LIST  "/nv/item_files/modem/qmi/uim/gba_fqdn_black_list"

/*---------------------------------------------------------------------------
  Defines to be used for MMGSDI
---------------------------------------------------------------------------*/
#define UIM_INVALID_CLIENT_ID         (0)
#define UIM_INVALID_SESSION_ID        (0)
#define UIM_FAKE_SESSION_ID           (0xFFFFFFFF)
#define UIM_INVALID_APP_INDEX         (0xFFFF)

/*---------------------------------------------------------------------------
  Values used in APDU check
---------------------------------------------------------------------------*/
#define QMI_UIM_APDU_INS_BYTE_MANAGE_CHANNEL            0x70
#define QMI_UIM_APDU_INS_BYTE_SELECT_FILE               0xA4
#define QMI_UIM_APDU_P1_BYTE_SELECT_DF_NAME             0x04
#define QMI_UIM_APDU_INS_BYTE_AUTHENTICATE              0x88
#define QMI_UIM_APDU_P2_BYTE_USIM_AUTHENTICATE_GSM_ALGO 0x80
#define QMI_UIM_APDU_CLA_BYTE_ICC                       0xA0

/*---------------------------------------------------------------------------
  Defines to be used for refresh
---------------------------------------------------------------------------*/
#define UIM_REFRESH_MAX_CLIENTS       (UIMQMI_SVC_MAX_CLIDS)
#define UIM_REFRESH_PROV_COUNT        (9)
#define UIM_REFRESH_NON_PROV_COUNT    (6)
#define UIM_REFRESH_SESSION_COUNT     (UIM_REFRESH_PROV_COUNT + UIM_REFRESH_NON_PROV_COUNT)

/*---------------------------------------------------------------------------
  Defines to be used for non provisioning applications
---------------------------------------------------------------------------*/
#define UIM_MAX_NON_PROV_SESSIONS     (QMI_UIM_MAX_CARD_COUNT * 3)
#define UIM_MAX_NON_PROV_CLIENTS      (UIMQMI_SVC_MAX_CLIDS)

/*---------------------------------------------------------------------------
  Defines to be used for channels to stream APDUs
---------------------------------------------------------------------------*/
#define UIM_MAX_APDU_CHANNEL_COUNT (QMI_UIM_MAX_CARD_COUNT * 20)

/*---------------------------------------------------------------------------
  Value used for EFS directory list
---------------------------------------------------------------------------*/
#define QMI_UIM_EFS_DIRECTORY_LIST_SIZE       (3)

/*---------------------------------------------------------------------------
  Length of IMSI digits data
---------------------------------------------------------------------------*/
#define QMI_UIM_IMSI_DIGIT_LEN                 (15)

#define QMI_UIM_IMPI_MAX                       (255)

/*===========================================================================

                                DATA TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  List of files a client has registered for refresh
---------------------------------------------------------------------------*/
typedef struct refresh_files_list
{
  mmgsdi_session_id_type             session_id;
  mmgsdi_path_type                   file;
  struct refresh_files_list        * next_ptr;
}qmi_uim_refresh_file_node_type;

/*---------------------------------------------------------------------------
  QMI UIM instance state definition & UIM client state definition
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_client_handle                  client_handle; /* client handle provided 
                                                       by QCSI - QCCI, unique for each client*/
  q_type                             cmd_q;         /* This will contain the commands in 
                                                        progress for each client*/
  struct
  {
    qmi_uim_registration_event_type  reg_event_type;
    boolean                          sap_intermediate_get_resp;
    uint32                           refresh_notify_mask;
    boolean                          refresh_vote;
    uint16                           refresh_num_files;
    qmi_uim_refresh_file_node_type * refresh_files_list;
    uint32                           refresh_all_notify_mask;
    boolean                          card_status_ind_postponed;
    boolean                          supply_voltage_pending_ack[QMI_UIM_MAX_CARD_COUNT];
  }uim_info;
} qmi_uimi_client_state_type;

/*---------------------------------------------------------------------------
  Watermark queue to hold deferred responses
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_csi_service_handle       service_handle;    /* provided by QCSI while registering QMI UIM 
                                                     with QCSI. Unique for each service */ 
  qmi_sap_client_handle        sap_handle;

  qmi_uimi_client_state_type * client[UIMQMI_SVC_MAX_CLIDS];
  uint32                       client_reg_count[UIMQMI_SVC_MAX_CLIDS];
} qmi_uimi_state_type;

/*===========================================================================

                               INTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  Possible refresh states
---------------------------------------------------------------------------*/
typedef enum
{
  UIM_REFRESH_STATE_IDLE               = 0,
  UIM_REFRESH_STATE_WAIT_FOR_OK        = 1,
  UIM_REFRESH_STATE_WAIT_FOR_COMPLETE  = 2
} qmi_uimi_refresh_state_type;

/*---------------------------------------------------------------------------
  AID list types
---------------------------------------------------------------------------*/
typedef enum
{
  UIM_AID_LIST_WHITE_LIST              = 0,
  UIM_AID_LIST_BLACK_LIST              = 1
} qmi_uimi_aid_list_type;

/*---------------------------------------------------------------------------
  Type of QMI UIM messages posted to DCC task
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_UIM_MESSAGE_EVENT,
  QMI_UIM_MESSAGE_CNF,
  QMI_UIM_MESSAGE_PHYSICAL_SLOTS_STATUS,
  QMI_UIM_MESSAGE_RECOVERY_CB,
  QMI_UIM_MESSAGE_SIMLOCK_CB,
  QMI_UIM_MESSAGE_RECOVERY_COMPLETE_IND,
  QMI_UIM_MESSAGE_SUPPLY_VOLTAGE_IND,
  QMI_UIM_MESSAGE_SIMLOCK_REMOTE_SFS_CB,
  QMI_UIM_MESSAGE_OTASP_STATUS_IND,
  QMI_UIM_MESSAGE_SIMLOCK_SET_REMOTE_SFS_CB,
  QMI_UIM_MESSAGE_GBA_CB,
  QMI_UIM_MESSAGE_REMOTE_UNLOCK_CB,
  QMI_UIM_MESSAGE_SIMLOCK_TEMPORARY_UNLOCK_CB
} qmi_uim_message_enum_type;

/*---------------------------------------------------------------------------
  Values used in APDU security restrictions EFS item
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_UIM_APDU_SEC_RESTRICTIONS_NONE   = 0,
  QMI_UIM_APDU_SEC_RESTRICTIONS_ALL    = 1,
  QMI_UIM_APDU_SEC_RESTRICTIONS_II_CLA = 2
} qmi_uim_apdu_sec_restr_type;

/*---------------------------------------------------------------------------
  Values used in AUTH security restrictions EFS item
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_UIM_AUTH_SEC_RESTRICTIONS_NONE   = 0,
  QMI_UIM_AUTH_SEC_RESTRICTIONS_ALL    = 1,
  QMI_UIM_AUTH_SEC_RESTRICTIONS_II_CLA = 2
} qmi_uim_auth_sec_restr_type;

/*---------------------------------------------------------------------------
  Values describe the level of encryption
---------------------------------------------------------------------------*/
typedef enum
{
  QMI_UIM_NO_ENCRYPTION         = 0,
  QMI_UIM_DATA_ONLY_ENCRYPTION  = 1,
  QMI_UIM_COMPLETE_ENCRYPTION   = 2
} qmi_uim_requested_encrytion_type;

/*---------------------------------------------------------------------------
  Type of QMI UIM PUP request state
     QMI_UIM_PUP_NOT_IN_PROGRESS:    No power_up is in progress
     QMI_UIM_PUP_MMGSDI_RSP_PENDING: Power_up req sent to MMGSDI and waiting
                                     for first rsp from MMGSDI
     QMI_UIM_PUP_RSP_SENT_TO_CLIENT: Rsp sent to QMI UIM client as part of first
                                     rsp from MMGSDI. Now, we need to drop second
                                     rsp from MMGSDI.
     QMI_UIM_PUP_CLIENT_RSP_PENDING: No rsp sent to client as part of first rsp
                                     from MMGSDI. Rsp will be send to client as
                                     part of second rsp from MMGSDI.
---------------------------------------------------------------------------*/
typedef enum {
  QMI_UIM_PUP_NOT_IN_PROGRESS                    = 0,
  QMI_UIM_PUP_MMGSDI_RSP_PENDING                 = 1,
  QMI_UIM_PUP_RSP_SENT_TO_CLIENT                 = 2,
  QMI_UIM_PUP_CLIENT_RSP_PENDING                 = 3
} qmi_uim_pup_state_enum_type;

/*===========================================================================
   ENUM:      QMI_UIM_NV_STAUTS_ENUM_TYPE
=============================================================================*/
typedef enum
{
  QMI_UIM_NV_STATUS_NOT_SET = 0x0,
  QMI_UIM_NV_STATUS_TRUE,
  QMI_UIM_NV_STATUS_FALSE,
} qmi_uim_nv_status_enum_type;

/* To initialize the command type*/
#define   UIMI_CMD_VAL_WIDTH                   0xFFFF

/*---------------------------------------------------------------------------
  Structure for QMI UIM messages
---------------------------------------------------------------------------*/
typedef struct
{
  /* This needs to be in first position */
  q_link_type                                     link_next;

  qmi_uim_message_enum_type                       message_type;

  union
  {
    struct
    {
      mmgsdi_return_enum_type                     status;
      mmgsdi_cnf_enum_type                        cnf_type;
      mmgsdi_cnf_type                             cnf_value;
    }                                             cnf;
    struct
    {
      mmgsdi_event_data_type                      evt;
    }                                             event;
    struct
    {
      qmi_framework_common_msg_hdr_type           cmd_hdr;
    }                                             clid_cb;
    struct
    {
      uimqmi_framework_cmd_type                   cmd;
    }                                             framework_cmd;
    struct
    {
      uint8                                       num_slots;
      uim_phy_slot_status_type                  * slot_status_ptr;
    }                                             physical_slots_info;
    struct
    {
      uim_slot_type                               slot;
      const void                                * user_data;
    }                                             recovery_data;
    struct
    {
      simlock_result_enum_type                    status;
      simlock_message_response_data_type          data;
      const void                                * user_data_ptr;
    }                                             simlock;
    struct
    {
      uim_slot_type                               slot;
    }                                             recovery_ind;
    struct
    {
      uim_slot_type                               slot;
      uimdrv_qmi_indications_type                 ldo_state;
      uimdrv_qmi_power_management_callback_type   uim_callback;
    }                                             supply_voltage;
    struct
    {
      simlock_file_operation_enum_type            operation;
      simlock_token_id_type                       request_token_id;
      simlock_file_type                           file;
      simlock_data_type                           simlock_data;
      simlock_remote_sfs_response_cb_type         simlock_resp_cb;
    }                                             simlock_remote_sfs_req;
    struct
    {
      gstk_slot_id_enum_type                      slot;
      gstk_otasp_act_status_enum_type             status;
    }                                             otasp_status_ind;
    struct
    {
      simlock_result_enum_type                    status;
      const void                                * user_data_ptr;
    }                                             simlock_set_remote_sfs_resp;
    struct
    {
      gba_result_enum_type                        status;
      gba_response_data_type                      cnf_value;
      const void                                * user_data_ptr;
    }                                             gba_cnf;
    struct
    {
      simlock_result_enum_type                    status;
      simlock_data_type                           key_data;
      const void                                * user_data_ptr;
    }                                             remote_unlock_resp;
  }                                               data;
} qmi_uim_message_type;

/*---------------------------------------------------------------------------
  Pending request info for non provisioning sessions
---------------------------------------------------------------------------*/
typedef struct qmi_uimi_pending_request_type
{
  uint16                                     command_id;
  void *                                     tlv_data_ptr;
  uint16                                     tlv_data_size;
  qmi_req_handle                             req_handle;
  uint8_t                                    indication_token_valid; 
  uint32_t                                   indication_token;
  qmi_client_handle                          cli_handle;
  uint32                                     client_reg_count;
  struct qmi_uimi_pending_request_type      *next_ptr;
} qmi_uimi_pending_request_type;

/*---------------------------------------------------------------------------
  Token information extracted from a command
---------------------------------------------------------------------------*/
typedef struct
{
  boolean                    is_valid;
  uint32                     token;
} qmi_uimi_ind_token_info_type;

/*---------------------------------------------------------------------------
  Userdata used for MMGSDI functions
---------------------------------------------------------------------------*/
typedef struct
{
  uint16                                request_id;
  qmi_uimi_ind_token_info_type          ind_token;
  qmi_client_handle                     client_handle;
  uint32                                client_reg_count;
  qmi_req_handle                        req_handle;
  /* this structure can be expanded with input parameters
     required at the end. Right now, this is not necessary */
  union
  {
    struct
    {
      qmi_uim_requested_encrytion_type  encryption_status;
      mmgsdi_session_type_enum_type     session_type;
      mmgsdi_access_type                file_access;
    }                                   read_transparent;
    struct
    {
      mmgsdi_access_type                file_access;
      mmgsdi_rec_num_type               first_record;
      uint16                            total_records;
      uint16                            read_records;
      qmi_uim_additional_records_type * records_cnf;
    }                                   read_record;
    struct
    {
      mmgsdi_access_type                file_access;
      mmgsdi_rec_num_type               record;
      mmgsdi_write_data_type            data;
    }                                   write_record;
    struct
    {
      mmgsdi_access_type                file_access;
    }                                   get_file_attributes;
    struct
    {
      uint8                             pin1_length;
      uint8                             pin1_value[QMI_UIM_TLV_MAX_PIN_LEN];
    }                                   pin_operation;
    struct
    {
      boolean                           hidden_key;
      boolean                           index_in_dir;
      mmgsdi_service_enum_type          service_id;
    }                                   get_service_status;
    struct
    {
      boolean                           automatic_selection_status;
      boolean                           personalization_status;
      boolean                           halt_subscription_status;
      boolean                           usb_uicc_supported_status;
      boolean                           sap_client_supported_status;
      boolean                           automatic_selection;
      boolean                           halt_subscription;
      boolean                           usb_uicc_supported;
      boolean                           sap_client_supported;
      boolean                           perso_reactivation_status;
    }                                   get_configuration;
    struct
    {
      uint8                             ins_value;
    }                                   sap_request;
    struct
    {
      uint8                             channel_index;
      uint8                             session_open_type;
      mmgsdi_session_type_enum_type     session_type;
    }                                   session_open_request;
    struct
    {
      uint8                             channel_index;
    }                                   session_close_request;
    struct
    {
      simlock_category_enum_type        category;
      simlock_slot_enum_type            slot;
      simlock_result_enum_type          result;
      uint32                            num_retries;
      boolean                           ck_present;
      uint16                            request_id;
    }                                   simlock_unlock;
    struct
    {
      boolean                           ck_present;
      uint16                            request_id;
      simlock_slot_enum_type            slot;
      simlock_ck_data_type              ck;
      uint8                             total_list_cnt;
      uint8                             next_list_to_lock;
      simlock_category_data_type      * code_ptr;
    }                                   simlock_lock;
    struct
    {
      uim_auth_context_enum_v01         auth_context;
    }                                   authenticate;
    struct
    {
      uint16                            request_id;
    }                                   simlock_set_device_mode;
    struct
    {
      uint16                            request_id;
    }                                   simlock_set_remote_sfs;
    struct
    {
      uint8                             channel_index;
    }                                   reselect_request;
    struct
    {
      gba_naf_fqdn_type                 fqdn_data;
    }                                   gba;
  }                                     data;
} qmi_uimi_userdata_type;

/*===========================================================================
  TYPEDEF QMI_UIM_REQ_HANDLER_TYPE

  DESCRIPTION
    cl_sp             : Service handle as given by the service in qmi_csi_connect
    req_handle Handle : Provided by the infrastructure to identify this particular transaction and message
    req_c_struct      : C structure with the decoded message
    req_c_struct_len: : Size of the C data structure
---------------------------------------------------------------------------*/
typedef qmi_csi_error (* const qmi_uim_req_handler_type)
(
  qmi_uimi_client_state_type * cl_sp,
  qmi_req_handle               req_handle,
  void                       * req_c_struct,
  int                          req_c_struct_len
);
 
/*===========================================================================
  TYPEDEF QMI_UIM_CMD_HDLR_TYPE
 
  DESCRIPTION
    This type can be used for each qmi msg command req
===========================================================================*/
typedef struct
{
  uint16                    cmd_type;
  qmi_uim_req_handler_type  request_hdlr;
} qmi_uim_cmd_hdlr_type;

/*===========================================================================
  TYPEDEF QMI_UIM_CMD_BUF_S
  
  DESCRIPTION
    The QMI command buf type.
    Each command buf a link to Transaction and client handle.
    It has a command type and response status.
  ===========================================================================*/
typedef struct
{
  q_link_type                   link_next;    /* must be first */
  uint16                        cmd_type;     /* Command Type  */
  qmi_req_handle                req_handle;   /* req handle */
  boolean                       resp_pending; /* response pending */
  qmi_client_handle             client_handle; /* client handle */
} qmi_uim_cmd_buf_s;

/*===========================================================================
  TYPEDEF QMI_UIM_COMPARE_S
  
  DESCRIPTION
    It contains req_handle and client handle
  ===========================================================================*/
typedef struct
{
  qmi_req_handle                req_handle;    /* req handle */
  qmi_client_handle             client_handle; /* client handle */
} qmi_uim_compare_s;

/*---------------------------------------------------------------------------
  Cache with record type used for write record
---------------------------------------------------------------------------*/
typedef struct qmi_uimi_file_record_type
{
  mmgsdi_session_id_type             session_id;
  mmgsdi_access_type                 file_access;
  mmgsdi_file_structure_enum_type    record_type;
  struct qmi_uimi_file_record_type * next_ptr;
} qmi_uimi_file_record_type;

/*---------------------------------------------------------------------------
  Whitelist/blacklist details used for APDU security
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_uimi_aid_list_type           aid_list_type;
  uint16                           aid_count;
  mmgsdi_static_data_type        * aid_list;
} qmi_uimi_apdu_sec_aid_info_type;

/*---------------------------------------------------------------------------
  Channel details for Send APDU
---------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_session_id_type           mmgsdi_session_id;
  mmgsdi_slot_id_enum_type         slot;
  uint8                            logical_channel;
  mmgsdi_static_data_type          aid;
  qmi_uimi_client_state_type     * opening_cl_sp;
}qmi_uim_apdu_channel_info_type;

/*---------------------------------------------------------------------------
 Non provisioning session info 
---------------------------------------------------------------------------*/
typedef struct
{
  mmgsdi_session_id_type             mmgsdi_session_id;
  mmgsdi_session_type_enum_type      session_type;
  mmgsdi_static_data_type            aid;
  qmi_uimi_pending_request_type    * pending_req_ptr;
  /* Client Info */
  struct
  {
    /* Number of clients linked to the non-prov. session */
    uint16                           client_count;
    /* Client state pointers of all clients linked to the session */
    qmi_uimi_client_state_type *     opening_cl_sp[UIM_MAX_NON_PROV_CLIENTS];
  }                                  client_info;
} qmi_uim_non_prov_session_info_type;

/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_IMSI_DIGITS_DATA_TYPE

   DESCRIPTION:        Structure containing IMSI digits data
     imsi_digits_len:  Data length
     imsi_digits:      IMSI data in BCD format
-------------------------------------------------------------------------------*/
typedef struct{
  uint8   imsi_digits_len;
  uint8   imsi_digits[QMI_UIM_IMSI_DIGIT_LEN];
} qmi_uim_imsi_digits_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      QMI_UIM_IMPI_TYPE

   DESCRIPTION:    Structure which stores the IMPI data.
     data_len:     Data length of IMPI data
     data:         IMPI data
-------------------------------------------------------------------------------*/
typedef struct {
  uint8  data_len;
  uint8  data[QMI_UIM_IMPI_MAX];
} qmi_uim_impi_type;

/* QMI UIM NAA Refresh Info */
typedef struct
{
  qmi_uimi_refresh_state_type        state;
  boolean                            ok_to_init;
  boolean                            complete;
  uint16                             client_count;
  qmi_uimi_client_state_type *       client_list[UIM_REFRESH_MAX_CLIENTS];
  struct
  {
    mmgsdi_session_id_type           session_id;
    mmgsdi_refresh_evt_info_type    *event_ptr;
  }                                  cache;
}qmi_uim_refresh_info_type;

/*---------------------------------------------------------------------------
  Global data for QMI UIM module
---------------------------------------------------------------------------*/
typedef struct
{
  /* Indicates if the global variable has been initialized or not */
  boolean                              init;
  /* MMGSDI client and session ids */
  mmgsdi_client_id_type                mmgsdi_client_id;
  mmgsdi_session_id_type               mmgsdi_gw_session_id[QMI_UIM_MAX_PROV_SESSIONS];
  mmgsdi_session_id_type               mmgsdi_1x_session_id[QMI_UIM_MAX_PROV_SESSIONS];
  mmgsdi_session_id_type               mmgsdi_card_slot_session_id[QMI_UIM_MAX_CARD_SESSIONS];
  /* Non provisioning session ids */
  qmi_uim_non_prov_session_info_type  *non_prov_session[UIM_MAX_NON_PROV_SESSIONS];
  /* Refresh details */
  qmi_uim_refresh_info_type           *refresh_info_ptr[UIM_REFRESH_SESSION_COUNT];
  /* Channel details for Send APDU */
  qmi_uim_apdu_channel_info_type      *apdu_channel[UIM_MAX_APDU_CHANNEL_COUNT];
  /* Session ids for non prov sessions */
  mmgsdi_session_id_type               non_prov_refresh_session_id[UIM_REFRESH_NON_PROV_COUNT];
  /* SAP details */
  struct
  {
    boolean                            is_enabled;
    mmgsdi_sap_state_enum_type         sap_state;
  }                                    sap_info[QMI_UIM_MAX_CARD_COUNT];
  /* Card state */
  qmi_uim_card_state                   card_state;
  /* Card state validity status */
  boolean                              card_state_valid[QMI_UIM_MAX_CARD_COUNT];
  /* Card is eUICC or not */
  boolean                              is_euicc_card[QMI_UIM_MAX_CARD_COUNT];
  /* Cache with record type */
  qmi_uimi_file_record_type *          record_cache_ptr;
  /* Power up commands */
  qmi_uim_pup_state_enum_type          power_up_state[QMI_UIM_MAX_CARD_COUNT];
  /* Token value for long APDU Response/Indication */
  uint32                               apdu_response_token;
  /* Token value for long read data Response/Indication */
  uint32                               long_read_response_token;

  /* APDU security AID list details for APDU security */
  qmi_uimi_apdu_sec_aid_info_type      apdu_sec_aid_info;
  boolean                              apdu_sec_aid_info_read;

  /* Indicates if APDU security restriction is supported */
  qmi_uim_apdu_sec_restr_type          apdu_sec_restr_value;
  boolean                              apdu_sec_restr_value_read;

  /* Indicates if AUTHENTICATE security restriction is supported */
  qmi_uim_auth_sec_restr_type          auth_sec_restr_value;
  boolean                              auth_sec_restr_value_read;

  /* Indicates close logical channel indication preference */
  qmi_uim_nv_status_enum_type          close_channel_sync;
  /* Indicates if silent PIN1 verification is supported */
  qmi_uim_nv_status_enum_type          silent_pin1_supported;
  /* Indicates if SAP security restriction is supported */
  qmi_uim_nv_status_enum_type          sap_sec_restr_supported;
  /* Indicates if the provisioning is auto or manual*/
  boolean                              automatic_provisioning;
  /* Stores QMI_UIM's EFS directory name */
  char                               * efs_directory_list[QMI_UIM_EFS_DIRECTORY_LIST_SIZE];
  /* Indicates if the card status indications are to be block */
  boolean                            block_card_status_ind;
  /* Stores status of all physical slots */
  struct
  {
    uint8                              num_slots;
    uim_phy_slot_status_type         * slot_status_ptr;
  }                                    physical_slots_info;
  /* Message queue */
  q_type                               qmi_uim_q;
#ifdef FEATURE_UIM_SSM
  /* SSM info indicates SSM client id and status */
  struct
  {
    uint32                             qmi_uim_ssm_id;
    ssm_err_t                          qmi_uim_ssm_init_err;
  }                                    ssm_info;
#endif /* FEATURE_UIM_SSM */
  /* Stores the supply voltage callback for each slot to notify drivers
     it can proceed with power down */
  uimdrv_qmi_power_management_callback_type
                                       supply_voltage_cb[QMI_UIM_MAX_CARD_COUNT];

  /* Stores the remote SFS response callback to notify
     simlock task about the pending read/write request */
  simlock_remote_sfs_response_cb_type  simlock_remote_sfs_operation_resp_cb;

  /* Stores the status if a temporary unlock is runnning for
     a given personalization feature for all slots.*/
  boolean                              temp_unlock_info[QMI_UIM_MAX_CARD_COUNT][QMI_UIM_MAX_NUMBER_PERSO_FEATURES];
} qmi_uim_global_type;

extern qmi_uim_global_type  * const qmi_uim_global_ptr;

extern qmi_uimi_state_type           *qmi_uim_state_ptr;

/*===========================================================================
  FUNCTION QMI_UIM_INIT_PRE_STARTUP()

  DESCRIPTION
    Initializes the QMI UIM global variables and register with MMGSDI

  PARAMETERS
    void

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_init_pre_startup
(
  void
);

/*===========================================================================
  FUNCTION QMI_UIM_INIT_POST_STARTUP()

  DESCRIPTION
    Registers the QMI UIM service to the QMI framework to initalize
    the service.

  PARAMETERS
    tcb_ptr :   MMGSDI tcb pointer
    sig     :   QMI Signal that has to be register with
                QCSI.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_init_post_startup
(
  rex_tcb_type  * tcb_ptr,
  rex_sigs_type   sig
);

/*===========================================================================
QMI_UIM_SIG_HANDLER

DESCRIPTION
  QMI UIM signal handler for MMGSDI task signals

PARAMETERS
  None.

  Returns TRUE if signal should be cleared from the set signal mask,
  FALSE if further signal processing is needed and hence signal
  should not be cleared.
===========================================================================*/
extern boolean qmi_uim_sig_handler
(
  void
);

/*===========================================================================
  FUNCTION QMI_UIM_RECOVERY_COMPLETE_IND()

  DESCRIPTION
    This function is invoked by the UIM drivers when a recovery indication
    has completed successfully to inform clients of QMI UIM.

  PARAMETERS
    slot: Indicates the slot in which the recovery occured

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_recovery_complete_ind
(
  const uim_slot_type slot
);

/*===========================================================================
  FUNCTION QMI_UIM_SUPPLY_VOLTAGE_IND()

  DESCRIPTION
    This function is invoked by the UIM drivers when the Vcc needs to
    be deactivated or when Vcc has been activated.

  PARAMETERS
    slot                                      : UIM slot id
    uimdrv_qmi_indications_type               : LDO state
    uimdrv_qmi_power_management_callback_type : callback to ack drivers

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_supply_voltage_ind
(
  uim_slot_type                                     slot,
  uimdrv_qmi_indications_type                       ldo_state,
  uimdrv_qmi_power_management_callback_type         uim_callback_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_BLOCK_CARD_STATUS_INDICATIONS

  DESCRIPTION
    API to block card status indications from being sent.

  PARAMETERS
    None.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_block_card_status_indications
(
  void
);

/*===========================================================================
  FUNCTION QMI_UIM_OPEN_SESSION()

  DESCRIPTION
    Opens a new non provisioning session on the fly and enqueues
    the command in the list of the session, so it can be executed
    later on, when the session is successfully opened.

  PARAMETERS
    qmi_uimi_cmd_val_e_type                command_id
    qmi_uim_session_information_type     * session_ptr
    void                                 * tlv_data_ptr
    uint16                                 tlv_data_size
    qmi_uim_cmd_buf_s                    * cmd_buf_ptr
    uint32                               * token_value
    qmi_uimi_client_state_type           * cl_sp

  RETURN VALUE
    errval         : QMI client-specific error message
                     of qmi_error_type_v01.
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_open_session
(
  uint16                                 command_id,
  qmi_uim_session_information_type     * session_ptr,
  void                                 * tlv_data_ptr,
  uint16                                 tlv_data_size,
  qmi_uim_cmd_buf_s                    * cmd_buf_ptr,
  uint32                               * token_value,
  qmi_uimi_client_state_type           * cl_sp
);

/*===========================================================================
  FUNCTION QMI_UIM_EXECUTE_READ_TRANSPARENT()

  DESCRIPTION
    Reads the content of a transparent file on the card

  PARAMETERS
    command_id        : command id
    session_id        : session id
    file_access       : file to be read
    offset            : offset in the file
    length            : length to be read
    encryption_status : requested encrption type
    token_ptr         : token pointer
    cmd_buf_ptr       : userdata
    clid              : client id of the control point

  RETURN VALUE
    errval      : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_execute_read_transparent
(
  uint16                              command_id,
  mmgsdi_session_id_type              session_id,
  mmgsdi_access_type                  file_access,
  mmgsdi_offset_type                  offset,
  mmgsdi_len_type                     length,
  qmi_uim_requested_encrytion_type    encryption_status,
  const uint32                      * token_ptr,
  qmi_uim_cmd_buf_s                 * cmd_buf_ptr,
  qmi_client_handle                   client_handle
);

/*===========================================================================
  FUNCTION QMI_UIMI_MMGSDI_CALLBACK()

  DESCRIPTION
    This function is executed as callback of MMGSDI API. This is
    executed in the context of MMGSDI task and needs to make a deep
    copy of the results and post a command to the QMI task to be
    processed.

  PARAMETERS

  RETURN VALUE
    None.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uimi_mmgsdi_callback
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type  *cnf_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_HANDLE_QCSI_EVT

  DESCRIPTION
    Function is called when a QCSI signal is received by the QMI UIM REMOTE
    service. It will result in the calling of one of the QMI UIM REMOTE
    request handlers.

  PARAMETERS
    rex_tcb_type  task_tcb
    rex_sigs_type sigs

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_handle_qcsi_evt
(
  rex_tcb_type * task_tcb, 
  rex_sigs_type  sigs
);

/*===========================================================================
  FUNCTION QMI_UIM_DEREG

  DESCRIPTION
    Called to deregister the QMI UIM as QMI service and as SAP client on
    reception of task stop signal

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_dereg(
  void
);

#endif /* QMI_UIM_H */
