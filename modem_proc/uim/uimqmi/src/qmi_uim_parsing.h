/*================================================================================

DESCRIPTION


Copyright (c) 2009 - 2019 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_parsing.h#1 $$ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
02/06/19   tq      Corrected addtional record length
01/23/19   tq      Make return type TRUE for terminal capability response
10/15/18   tq      Migration of QMI UIM service to QCCI-QCSI
10/13/17   rps     Remove old  perso support
12/05/16   bcho    Remove compilation error
09/28/16   sn      Added support to retrieve retry attempts left for locking
08/10/16   ar      Support for setting terminal capability from AP
09/28/16   sn      Populate verify retries correctly
08/10/16   ar      Support for setting terminal capability from AP
07/15/16   sp      Seperate SAP and ERA-GLONASS related functionality
06/16/16   vdc     Split QMI UIM parsing files for supporting IOE
05/19/16   vdc     Split QMI UIM files for supporting IOE
05/16/16   ar      Featurizing code for IoE
09/25/15   vr      Added support for ERA-GLONASS
08/20/15   at      Replacement of Secapis to use a derived key & HMAC
08/10/15   stv     To send temporary unlock status indication
07/01/15   bcho    Fix enabling a lock with more than 20 codes of lock data
07/08/15   bcho    Redundant checks for SIMLOCK
05/15/15   vv      Support for RSU
05/14/15   ar      Support for terminate_app TLV in qmi_uim_close_session
05/01/15   vdc     Added support for APN for bootstrapping
05/01/15   vdc     Added support for QMI UIM GBA
03/19/15   at      Introducing new relock feature in SIMLock
02/03/15   xj      Add USB UICC and SAP CLIENT supported interface
12/16/14   av      Apply auth restr via NV for IIC only if USIM is selected
10/10/14   vv      Add support for remote simlock configuration
10/08/14   vdc     Added support for UICC activation status indication
09/30/14   vv      Add support for emergency only command
09/25/14   tl      Add support for secure read
09/17/14   tl      Add support for perso and deperso secure
08/29/14   tl      Add support for supply voltage command and indication
07/25/14   vv      Added support for perso command
07/22/14   at      Fix error in ISIM Auth response for different contexts
06/30/14   vv      Remove perso code
06/12/14   av      Enhance GET_SERVICE_STATUS to accomodate all service numbers
05/22/14   tl      Added support for recovery complete indications
05/13/14   vdc     Support for reduced card status indications
05/12/14   df      Add QMI support for status cmd
03/26/14   tl      SIM Lock secondary revisions
03/18/14   tl      Introduce new SIM Lock feature
02/25/14   yt      Support for QMI_UIM_RESELCT_REQ
02/24/14   at      Support for read transparent long indication
12/09/13   spo     Fix for handling auth request for USIM and ISIM
11/20/13   tkl     Added API for Increase
10/27/13   df      Add QMI_UIM_CARD_ERROR_SAP_CONNECTED to error type
10/04/13   vv      Added API to return se13 table release date and source
09/23/13   df      Support for SIM busy
09/09/13   yt      Remove redundant code
08/08/13   ks      Support for subscription manager
07/15/13   av      Introduction of new NULL_BYTES card error type
07/02/13   yt      Dynamically allocate slot and app data
06/21/13   kk      Updated support for triple sim
05/21/13   vv      Extend session closed indication for all sessions
02/22/13   yt      Support for slot 3
02/12/13   at      Added support for file status TLV
02/07/13   yt      Support for activation/deactivation of EFs
11/02/12   yt      Add support for ACL in SET and GET SERVICE STATUS commands
09/07/12   tl      Added support for SGLTE dual slots
08/29/12   at      Optional TlV in SAP connection req to check for active calls
08/28/12   spo     Added support for Session Closed Indication
08/16/12   at      New command handling for QMI_UIM_REFRESH_REGISTER_ALL
08/16/12   tl      Add session close without app termination functionality
05/14/12   at      Add new TLV to QMI_UIM_GET_CARD_STATUS command
04/11/12   at      Indication support for sending long APDUs
03/02/12   vv      Added optional TLV in send APDU request
03/02/12   vv      Added support for querying ESN status
02/27/12   yt      Add function to construct file_length TLV
01/03/12   nb      Added optional TLV for QMI configuration
12/21/11   nmb     Secure channel perso for apps
12/15/11   nb      Added new card error type
10/12/11   at      Handling intermediate get response in SAP commands
10/05/11   yt      Added support for refresh of non provisioning sessions
                   Removed flag to indicate if TLV is mandatory or optional
09/16/11   yt      Added optional TLV for status of hot swap switch
08/16/11   at      Indication support for various commands
08/01/11   at      Support for keyref_id TLV in PIN commands
08/01/11   at      Support for using session id on an open channel id
08/01/11   vs      Support app selection with FCI via logical channel operation
06/27/11   at      Renamed qmi_uim_response_sap_generic()
04/28/11   mib     Fixed crash due to invalid free reading multiple records
04/26/11   vs      Support for subscription ok request
04/07/11   mib     Added support to read multiple records at the same time
04/01/11   mib     Changed TLV for the encrypted PIN passed for verify
03/09/11   at      Added support for PIN1 encryption/decryption
03/03/11   mib     Added support to manage logical channels to send APDUs
03/01/11   at      Added support for SAP request & indication
01/12/11   mib     Added support for CSIM file paths
01/04/11   mib     Added support for Compute IP Authentication
12/21/10   at      Added support for SAP connection request
12/17/10   mib     Split APDU request into two TLVs
12/09/10   at      Added support for send APDU request
10/22/10   mib     Added support for get configuration request
08/19/10   mib     Added support for POSSIBLY_REMOVED error code
08/05/10   tkl     QMI UIM & QMI CAT split from data package
06/16/10   mib     Added support for hidden key
06/07/10   mib     Added support for GET LABEL and SESSION CHANGE requests
05/31/10   mib     Added storage of application label
05/05/10   mib     Improved mechanism to pass TLV id for responses
04/06/10   mib     Added support for authenticate and close session.
                   Changed TLV ids for consistency with other QMI services
01/13/10   mib     Initial version
=============================================================================*/

#ifndef QMI_UIM_PARSING_H
#define QMI_UIM_PARSING_H

/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "comdef.h"
#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"
#include "mmgsdilib_common.h"
#include "uim_v.h"
#include "uim_p.h"
#include "simlock_common.h"
#include "gstk_exp_v.h"
#include "simlock_modem_p.h"
#include "gba_lib.h"
#include "user_identity_module_v01.h"
#include "ds_qmi_fw_common.h"

/*---------------------------------------------------------------------------
  Indication TLVs tags
---------------------------------------------------------------------------*/
#define UIMI_TLV_IND_TAG_MANDATORY_1             (0x01)

/*---------------------------------------------------------------------------
  Maximum size of fields in TLV structures
---------------------------------------------------------------------------*/
#define QMI_UIM_TLV_MAX_PIN_LEN                  ( 8)
#define QMI_UIM_TLV_MAX_CK_LEN                   (16)
#define QMI_UIM_TLV_MAX_ENCRYPTED_PIN1_LEN       (255)

/*---------------------------------------------------------------------------
  Size of fields in TLV structures
---------------------------------------------------------------------------*/
#define QMI_UIM_TLV_CHANNEL_ID_LEN               ( 1)

/*---------------------------------------------------------------------------
  Number of provisioning sessions of each type (GW/1x)
---------------------------------------------------------------------------*/
#define QMI_UIM_MAX_PROV_SESSIONS                ( 3)

/*---------------------------------------------------------------------------
  Number of card sessions
---------------------------------------------------------------------------*/
#define QMI_UIM_MAX_CARD_SESSIONS                ( 3)

/*---------------------------------------------------------------------------
  Number of cards and applications
---------------------------------------------------------------------------*/
#define QMI_UIM_MAX_CARD_COUNT                 ( 3)
#define QMI_UIM_MAX_APP_COUNT                  (60)
#define QMI_UIM_MAX_APPS_PER_CARD              (20)
#define QMI_UIM_LEGACY_MAX_APP_COUNT           (24)
#define QMI_UIM_LEGACY_MAX_CARD_COUNT          ( 2)
#define QMI_UIM_LEGACY_MAX_APPS_PER_CARD       ( 8)

/*---------------------------------------------------------------------------
  Maximum number of retries
---------------------------------------------------------------------------*/
#define QMI_UIM_MAX_RETRIES                    (0xFF)

/*---------------------------------------------------------------------------
  Generic data magnitudes
---------------------------------------------------------------------------*/
#define QMI_UIM_MAX_IMSI_LEN                   (10)

/*---------------------------------------------------------------------------
  Maximum number of perso features
---------------------------------------------------------------------------*/
#define QMI_UIM_MAX_NUMBER_PERSO_FEATURES      11

/*---------------------------------------------------------------------------
  RFU size of encrypted message
---------------------------------------------------------------------------*/
#define QMI_UIM_ENCRYPTED_MSG_RFU_SIZE                         (4)

/*---------------------------------------------------------------------------
  Maximum length of encrypted response
---------------------------------------------------------------------------*/
#define QMI_UIM_ENCRYPTED_RESP_MAX                             (1024)

/*=============================================================================

                           STRUCTURE DECLARATIONS

=============================================================================*/

/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_FILES_LIST_TYPE

   DESCRIPTION:        Linked list with list of files
     file:             File
     next_ptr:         Pointer to next node
-------------------------------------------------------------------------------*/
typedef struct files_list
{
  mmgsdi_access_type                   file;
  struct files_list *                  next_ptr;
} qmi_uim_files_list_type;

/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_SESSION_INFORMATION_TYPE

   DESCRIPTION:        Data extracted from session information TLV
     session_type:     Session type
     aid:              AID value
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_session_type_enum_type          session_type;
  mmgsdi_static_data_type                aid;
} qmi_uim_session_information_type    ;

/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_TLV_FILE_ID_TYPE

   DESCRIPTION:        Data extracted from file id TLV
     file_access:      Path and file id
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_access_type                     file_access;
} qmi_uim_tlv_file_id_type;

/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_TLV_DEPERSONALIZATION_TYPE

   DESCRIPTION:        Data extracted from deactivate perso TLV
     category:         Category to deactivate
     ck:               Control key value
-------------------------------------------------------------------------------*/
typedef struct {
  simlock_category_enum_type             category;
  simlock_ck_data_type                   ck;
} qmi_uim_tlv_depersonalization_type;

/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_TLV_SLOT_TYPE

   DESCRIPTION:        Data extracted from slot TLV
     slot:             Slot id
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_slot_id_enum_type               slot;
} qmi_uim_tlv_slot_type;


/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_IMSI_TYPE

   DESCRIPTION:        Structure containing IMSI data
-------------------------------------------------------------------------------*/
typedef struct
{
  uint8                                  len;
  uint8                                  imsi[QMI_UIM_MAX_IMSI_LEN];
} qmi_uim_imsi_type;

/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_TLV_DECRYPTED_IMSI_TYPE

   DESCRIPTION:        Data decoded from the encrypted IMSI TLV
-------------------------------------------------------------------------------*/
typedef struct {
  qmi_uim_imsi_type                     imsi_data;
} qmi_uim_tlv_decrypted_imsi_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      QMI_UIM_SECURE_HEADER_TYPE

   DESCRIPTION:
     Contains request information corresponding to the command.

-----------------------------------------------------------------------------*/
typedef struct
{
  uint16 cmd_id;
  uint16 request_id;
} qmi_uim_secure_header_type;

/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_TLV_DECRYPTED_PERSO_SECURE_TYPE

   DESCRIPTION:        Data decoded from the encrypted perso TLV
-------------------------------------------------------------------------------*/
typedef struct {
  qmi_uim_secure_header_type            header;
  simlock_slot_enum_type                slot;
  uint8                                 num_of_lists;
  simlock_category_data_type          * code_ptr;
} qmi_uim_tlv_decrypted_perso_secure_type;

/* -----------------------------------------------------------------------------
   ENUM:      QMI_UIM_DEPERSO_SECURE_OPERATION_TYPE

   DESCRIPTION:
     Indicates the operation type of the deperso secure command
-------------------------------------------------------------------------------*/
typedef enum
{
  QMI_UIM_DEPERSO_UNLOCK_WITHOUT_CK     = 0,
  QMI_UIM_DEPERSO_TEMP_UNLOCK_ENABLE    = 1,
  QMI_UIM_DEPERSO_TEMP_UNLOCK_DISABLE   = 2
} qmi_uim_deperso_secure_operation_type;

/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_TLV_DECRYPTED_DEPERSO_SECURE_TYPE

   DESCRIPTION:        Data decoded from the encrypted deperso TLV
-------------------------------------------------------------------------------*/
typedef struct {
  qmi_uim_secure_header_type             header;
  simlock_slot_enum_type                 slot;
  simlock_category_enum_type             category_type;
  qmi_uim_deperso_secure_operation_type  operation;
  uint32                                 duration;
} qmi_uim_tlv_decrypted_deperso_secure_type;

/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_TLV_PERSONALIZATION_CODE_TYPE

   DESCRIPTION:            Lock code data extracted from activate perso TLV
     num_of_list:          Each list contains SIMLOCK_NUM_CODES_MAX (20) codes.
                           The num_of_lists indicate count of such lists.
     code_ptr:             Pointer to lock codes
-------------------------------------------------------------------------------*/
typedef struct {
  uint8                          num_of_lists;
  simlock_category_data_type   * code_ptr;
} qmi_uim_tlv_personalization_code_type;

/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_TLV_DECRYPTED_EMERGENCY_ONLY_TYPE

   DESCRIPTION:        Data decoded from the encrypted emergency only TLV
-------------------------------------------------------------------------------*/
typedef struct {
  qmi_uim_secure_header_type             header;
  simlock_device_mode_enum_type          device_mode;
} qmi_uim_tlv_decrypted_emergency_only_type;

/* -----------------------------------------------------------------------------
   ENUM:      QMI_UIM_SIMLOCK_CONFIG_OPERATION_TYPE

   DESCRIPTION:
     Type of simlock configuration operation
-------------------------------------------------------------------------------*/
typedef enum
{
  QMI_UIM_SIMLOCK_CONFIG_OPERATION_READ                = 0,
  QMI_UIM_SIMLOCK_CONFIG_OPERATION_WRITE               = 1,
  QMI_UIM_SIMLOCK_CONFIG_OPERATION_ENABLE_REMOTE_SFS   = 2,
  QMI_UIM_SIMLOCK_CONFIG_OPERATION_DISABLE_REMOTE_SFS  = 3
} qmi_uim_simlock_config_operation_type;

/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_TLV_DECRYPTED_SIMLOCK_CONFIG_TYPE

   DESCRIPTION:        Data decoded from the encrypted simlock configuration TLV
-------------------------------------------------------------------------------*/
typedef struct {
  qmi_uim_secure_header_type                   header;
  qmi_uim_simlock_config_operation_type        operation;
  union {
    simlock_remote_sfs_request_enum_type       remote_sfs_operation;
    struct {
      simlock_result_enum_type                 result;
      uint8                                    file_id;
      simlock_data_type                        data;
    }                                          sfs_read_operation;
    struct {
      simlock_result_enum_type                 result;
      uint8                                    file_id;
    }                                          sfs_write_operation;
  }operation_type;
} qmi_uim_tlv_decrypted_simlock_config_type;

/* -----------------------------------------------------------------------------
   STRUCTURE:   QMI_UIM_TLV_GBA_SESSION_INFO_TYPE

   DESCRIPTION: Data decoded from session information TLV
-------------------------------------------------------------------------------*/
typedef struct {
  gba_session_type    session_type;
  gba_aid_type        aid;
} qmi_uim_tlv_gba_session_info_type;

/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_TLV_NAF_ID_TYPE

   DESCRIPTION:  Data decoded from the NAF ID TLV
-------------------------------------------------------------------------------*/
typedef struct {
  gba_naf_id_type *naf_id_ptr;
} qmi_uim_tlv_naf_id_type;

/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_TLV_APN_TYPE

   DESCRIPTION:  Data decoded from the APN TLV
-------------------------------------------------------------------------------*/
typedef struct {
  char      *apn_data_ptr;
} qmi_uim_tlv_apn_type;

/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_TLV_REMOTE_UNLOCK_CONFIG_TYPE

   DESCRIPTION:  Data from the remote unlock request
-------------------------------------------------------------------------------*/
typedef struct {
  simlock_data_type       config_data;
} qmi_uim_tlv_remote_unlock_config_type;

/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_TLV_IMSI_TYPE

   DESCRIPTION:  IMSI Data from the verify IMSI request
-------------------------------------------------------------------------------*/
typedef struct {
  simlock_imsi_type       imsi_data;
} qmi_uim_tlv_imsi_type;

/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_TLV_SET_SIM_PROFILE_TYPE

   DESCRIPTION:         Data extracted from Set SIM Profile TLV
   profile_id           Indicates the profile id to activate
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_profile_id_enum_type            profile_id;
} qmi_uim_tlv_set_sim_profile_type;

/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_TERMINAL_CAPABILITY_TLV_TYPE

   DESCRIPTION:        Terminal capability tlv
-------------------------------------------------------------------------------*/
typedef struct {
  uint8                  tag;
  uint8                  value_len;
  uint8                  value[QMI_UIM_TERMINAL_CAPABILITY_VALUE_MAX_V01];
  boolean                remove_tlv;
} qmi_uim_terminal_capability_tlv_type;

/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_TLV_TERMINAL_CAPABILITY_TLV_TYPE

   DESCRIPTION:        Terminal capability tlv contents
-------------------------------------------------------------------------------*/
typedef struct {
  uint8                                num_tlvs;
  qmi_uim_terminal_capability_tlv_type *tlv_ptr;
} qmi_uim_tlv_terminal_capability_tlv_type;

/* -----------------------------------------------------------------------------
   ENUM:      QMI_UIM_CARD_STATE_TYPE

   DESCRIPTION:
     Indication of card state
-------------------------------------------------------------------------------*/
typedef enum {
  QMI_UIM_CARD_STATE_ABSENT         = 0,
  QMI_UIM_CARD_STATE_PRESENT        = 1,
  QMI_UIM_CARD_STATE_ERROR          = 2
} qmi_uim_card_state_type;

/* -----------------------------------------------------------------------------
   ENUM:      QMI_UIM_APP_STATE_TYPE

   DESCRIPTION:
     Indication of application state
-------------------------------------------------------------------------------*/
typedef enum {
  QMI_UIM_APP_STATE_UNKNOWN         = 0,
  QMI_UIM_APP_STATE_DETECTED        = 1,
  QMI_UIM_APP_STATE_PIN_REQUIRED    = 2,
  QMI_UIM_APP_STATE_PUK1_REQUIRED   = 3,
  QMI_UIM_APP_STATE_PERSO           = 4,
  QMI_UIM_APP_STATE_BLOCKED         = 5,
  QMI_UIM_APP_STATE_ILLEGAL         = 6,
  QMI_UIM_APP_STATE_READY           = 7
} qmi_uim_app_state_type;

/* -----------------------------------------------------------------------------
   ENUM:      QMI_UIM_PERSO_STATE_TYPE

   DESCRIPTION:
     Indication of perso state
-------------------------------------------------------------------------------*/
typedef enum {
  QMI_UIM_PERSO_STATE_UNKNOWN       = 0,
  QMI_UIM_PERSO_STATE_IN_PROGRESS   = 1,
  QMI_UIM_PERSO_STATE_READY         = 2,
  QMI_UIM_PERSO_STATE_CODE_REQUIRED = 3,
  QMI_UIM_PERSO_STATE_PUK_REQUIRED  = 4,
  QMI_UIM_PERSO_STATE_PERM_BLOCKED  = 5
} qmi_uim_perso_state_type;

/* -----------------------------------------------------------------------------
   ENUM:      QMI_UIM_CARD_ERROR_TYPE

   DESCRIPTION:
     Indication of card error reason
-------------------------------------------------------------------------------*/
typedef enum {
  QMI_UIM_CARD_ERROR_UNKNOWN                     = 0,
  QMI_UIM_CARD_ERROR_POWER_DOWN                  = 1,
  QMI_UIM_CARD_ERROR_POLL_ERROR                  = 2,
  QMI_UIM_CARD_ERROR_NO_ATR_RECEIVED             = 3,
  QMI_UIM_CARD_ERROR_VOLT_MISMATCH               = 4,
  QMI_UIM_CARD_ERROR_PARITY_ERROR                = 5,
  QMI_UIM_CARD_ERROR_UNKNOWN_POSSIBLY_REMOVED    = 6,
  QMI_UIM_CARD_ERROR_CODE_SIM_TECHNICAL_PROBLEMS = 7,
  QMI_UIM_CARD_ERROR_NULL_BYTES                  = 8,
  QMI_UIM_CARD_ERROR_SAP_CONNECTED               = 9
} qmi_uim_card_error_type;

/* -----------------------------------------------------------------------------
   ENUM:      QMI_UIM_CARD_INFO_TYPE

   DESCRIPTION:
     Description of card info
-------------------------------------------------------------------------------*/
typedef struct {
  qmi_uim_card_state_type         card_state;
  mmgsdi_pin_status_enum_type     upin_state;
  uint8                           upin_num_retries;
  uint8                           upuk_num_retries;
  qmi_uim_card_error_type         card_error;
  boolean                         sim_busy;
} qmi_uim_card_info_type;

/* -----------------------------------------------------------------------------
   ENUM:      QMI_UIM_APP_INFO_TYPE

   DESCRIPTION:
     Description of application info
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_app_enum_type            app_type;
  qmi_uim_app_state_type          app_state;
  qmi_uim_perso_state_type        perso_state;
  mmgsdi_perso_feature_enum_type  perso_feature;
  uint32                          perso_retries;
  mmgsdi_static_data_type         aid;
  mmgsdi_static_data_type         label;
  boolean                         upin;
  mmgsdi_pin_status_enum_type     pin1_state;
  uint8                           pin1_num_retries;
  uint8                           puk1_num_retries;
  mmgsdi_pin_status_enum_type     pin2_state;
  uint8                           pin2_num_retries;
  uint8                           puk2_num_retries;
  mmgsdi_slot_id_enum_type        slot;
} qmi_uim_app_info_type;

/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_CARD_STATE

   DESCRIPTION:        Item containing description of card state
-------------------------------------------------------------------------------*/
typedef struct {
  uint8                             num_slots;
  qmi_uim_card_info_type          * card[QMI_UIM_MAX_CARD_COUNT];
  uint16                            index_gw[QMI_UIM_MAX_PROV_SESSIONS];
  uint16                            index_1x[QMI_UIM_MAX_PROV_SESSIONS];
  uint16                            num_apps;
  qmi_uim_app_info_type           * application[QMI_UIM_MAX_APP_COUNT];
} qmi_uim_card_state;


/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_ADDITIONAL_RECORDS_TYPE

   DESCRIPTION:        Item containing response for additional records read
-------------------------------------------------------------------------------*/
typedef struct {
  mmgsdi_return_enum_type   mmgsdi_status;
  mmgsdi_sw_type            status_word;
  mmgsdi_data_type          read_data;
} qmi_uim_additional_records_type;


/* -----------------------------------------------------------------------------
   STRUCTURE:    QMI_UIM_REGISTRATION_EVENT_TYPE

   DESCRIPTION:        Structure used to hold the registration status
     card_status:                      Registration to legacy card status events
     sap_events:                       Registration to SAP events
     extended_card_status:             Registration to extended card status events
     provisioning_session_close_event: Registration to prov session closed event
     physical_slots_status:            Registration to physical slots status events
     sim_busy_events:                  Registration to sim busy events
     reduced_card_status_events:       Registration to reduced card status events
     recovery_complete_event:          Registration to recovery complete event
     supply_voltage_events:            Registration to supply voltage events
     reduced_card_status_events        Registration to reduced card status events
     simlock_remote_sfs_events:        Registration to simlock remote SFS events
     simlock_temp_unlock_status:       Registration to simlock temp unlock status
-------------------------------------------------------------------------------*/
typedef struct {
  boolean          card_status;
  boolean          sap_events;
  boolean          extended_card_status;
  boolean          prov_session_close_event;
  boolean          physical_slots_status;
  boolean          sim_busy_events;
  boolean          reduced_card_status_events;
  boolean          recovery_complete_event;
  boolean          supply_voltage_events;
  boolean          reg_for_activation_status_event;
  boolean          simlock_remote_sfs_events;
  boolean          simlock_temp_unlock_status;
} qmi_uim_registration_event_type;


/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_READ_ADDITIONAL_RECORDS_RESULT()

  DESCRIPTION
    Populate additional records result

  PARAMETERS
  uim_read_additional_record_result_type_v01 *add_read_result_ptr,
  uint16                                      num_records,
  qmi_uim_additional_records_type            *records_ptr

  RETURN VALUE
    TRUE on Valid. FALSE otherwise


  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_read_additional_records_result
(
  uim_read_additional_record_result_type_v01 *add_read_result_ptr,
  uint16                                      num_records,
  qmi_uim_additional_records_type            *records_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_GET_FILE_ATTRIBUTES()

  DESCRIPTION
    Composes the TLV content of read result

  PARAMETERS
    file_attr_out  : output file attr
    attribute_data : file attributes from the card
    raw_value      : attributes raw value

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_get_file_attributes
(
  uim_file_attributes_type_v01 * file_attr_out,
  mmgsdi_file_attributes_type    attribute_data,
  mmgsdi_get_file_attr_data_type raw_value
);

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_RETRIES_LEFT()

  DESCRIPTION
    populate of PIN retries left

  PARAMETERS
    uint8               : *num_unblock_retries_ptr,
    uint8               : *num_retrie_ptr,
    num_retries         :  number of PIN retries
    num_unblock_retries :  number of PUK retries

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_response_retries_left
(
  uint8           *num_unblock_retries_ptr,
  uint8           *num_retrie_ptr,
  uint32           num_retries,
  uint32           num_unblock_retries
);

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_CARD_STATUS()

  DESCRIPTION
    Composes the TLV content for card status indication or
    get card status request

  PARAMETERS
    card_state_ptr         : input card status ptr
    card_status_out_ptr    : output card status

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_card_status
(
  const qmi_uim_card_state * card_state_ptr,
  uim_card_status_type_v01 * card_status_out_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_EXTENDED_CARD_STATUS()

  DESCRIPTION
    Composes the extended card status TLV content for card status indication
    or get card status request

  PARAMETERS
    extnd_card_status_ptr          : Output extended card status
    card_state_ptr                 : card status ptr
    validity_len                   : len of card states valid array
    validity_ptr                   : Card states valid or not

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_extended_card_status
(
  uim_extended_card_status_type_v01  * extnd_card_status_ptr,
  const qmi_uim_card_state           * card_state_ptr,
  uint8                                validity_len,
  const boolean                      * validity_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_EVENT_REG()

  DESCRIPTION
    Composes the event mask

  PARAMETERS
    is_valid                 : valid flag
    mask                     : output mask
    reg_event_type           : event registered type

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_response_event_reg
(
  byte                              *is_valid,
  uint32_t                          *mask,
  qmi_uim_registration_event_type    reg_event_type
);

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_REFRESH_EVENT()

  DESCRIPTION
    Populate refresh event

  PARAMETERS
    ref_event_ptr       : output ptr
    session_type        : session type
    aid_ptr             : AID value
    refresh_evt_ptr     : refresh event

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_refresh_event
(
  uim_refresh_event_type_v01         * ref_event_ptr,
  mmgsdi_session_type_enum_type        session_type,
  const mmgsdi_static_data_type      * aid_ptr,
  const mmgsdi_refresh_evt_info_type * refresh_evt_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_AUTHENTICATE_ISIM()

  DESCRIPTION
    Composes the TLV content of ISIM authentication

  PARAMETERS  
    content_valid_ptr       : output valid flag
    content_len_ptr         : output len
    content_ptr             : output result
    content_buf_size        : szie of content buf[In]
    status                  : status
    auth_context            : auth context
    res                     : RES value
    ck                      : CK value
    ik                      : IK value
    auts                    : AUTS value

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_authenticate_isim
( 
  uint8_t                    *content_valid_ptr,
  uint32_t                   *content_len_ptr,
  uint8_t                    *content_ptr,
  const uint32_t              content_buf_size,
  mmgsdi_return_enum_type     status,
  uim_auth_context_enum_v01   auth_context,
  mmgsdi_data_type            res,
  mmgsdi_data_type            ck,
  mmgsdi_data_type            ik,
  mmgsdi_data_type            auts
);

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_HOT_SWAP_STATUS_NEW()

  DESCRIPTION
    Composes the hot swap switch status TLV content for get card status
    request

  PARAMETERS
    hotswap_status_ptr    : output hotswap status ptr
    hotswap_len_ptr       : output len of output hotswap status ptr
    is_valid              : output data is valid or not
    extended_status       : if it is extended hot swap status
    hot_swap_status_len   : length of hot_swap_status_ptr array
    hot_swap_status_ptr   : array with status of hot swap switch for each slot

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_response_hot_swap_status
(
  uim_hot_swap_status_enum_v01   *hotswap_status_ptr,
  uint32                         *hotswap_len_ptr,
  uint8                          *is_valid,
  boolean                         extended_status,
  uint8                           hot_swap_status_len,
  uim_rt_status_enum             *hot_swap_status_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD()

  DESCRIPTION
    Converts file access method between path and MMGSDI enum

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method
    session_type    : session type

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_convert_file_access_method(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method,
  mmgsdi_session_type_enum_type    session_type
);

/*===========================================================================
  FUNCTION QMI_UIM_CHECK_PIN_DIGITS()

  DESCRIPTION
    Checks the digits of PIN value. Allowed digits are only from
    '0' to '9'. This function does not check the length of the PIN.

  PARAMETERS
    pin_value : value of the pin

  RETURN VALUE
    boolean to indicate success or failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_check_pin_digits (
  mmgsdi_data_type  pin_value
);

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_CARD_STATUS_VALIDITY()

  DESCRIPTION
    Composes the card status validity TLV content for get card status
    request

  PARAMETERS
    card_status_validity_ptr     : output pointer to card status validity array
    card_status_validity_len_ptr : len of output pointer to card status validity array
    is_field_valid               : output data is valid or not
    validity_len                 : length of validity status array
    validity_ptr                 : pointer to validity status array

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_card_status_validity
(
  uint8                         * card_status_validity_ptr,
  uint32                        * card_status_validity_len_ptr,
  uint8                         * is_field_valid,
  uint8                           validity_len,
  boolean                       * validity_ptr
);


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_FILE_STATUS()

  DESCRIPTION
    Populate the TLV content for file status

  PARAMETERS 
  boolean                            : valid_ptr,
  uim_file_status_type_v01           : file_status_ptr,
  const mmgsdi_file_attributes_type  : file_attrib_ptr

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_response_file_status
(
  uint8                              * valid_ptr,
  uim_file_status_type_v01           * file_status_ptr,
  const mmgsdi_file_attributes_type  * file_attrib_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_SESSION_TYPE()

  DESCRIPTION
    Composes the TLV content for Session type

  PARAMETERS
    uim_session_type_ptr    : Pointer to output session pointer
    session_type            : MMGSDI session type

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_session_type
(
  uim_session_type_enum_v01      *uim_session_type_ptr,
  mmgsdi_session_type_enum_type   session_type
);

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_SESSION_CLOSED_CAUSE()

  DESCRIPTION
    Convert session close cause from  mmgsdi_session_closed_cause_enum_type
    to mmgsdi_session_closed_cause_enum_type and Populate it

  PARAMETERS
    uim_session_close_cause_enum_v01      : uim_cause,
    mmgsdi_session_closed_cause_enum_type : mmgsdi_cause

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_response_session_closed_cause
(
  uim_session_close_cause_enum_v01       *uim_cause,
  mmgsdi_session_closed_cause_enum_type   mmgsdi_cause
);

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_SESSION_CLOSED_FILE_ID()

  DESCRIPTION
    Composes the TLV content for the File ID

  PARAMETERS
    file_id_ptr      : Pointer to file id
    cause            : Session close cause

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_session_closed_file_id
(
  uint16                               * file_id_ptr,
  mmgsdi_session_closed_cause_enum_type  cause
);

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_PHYSICAL_SLOTS_STATUS()

  DESCRIPTION
    Composes the TLV content for physical slots status indication

  PARAMETERS
    tlv_id             : id to be used for the TLV
    response_pptr      : output sdu
    slot_status_ptr    : Status of physical slots
    num_slots          : number of physical slots

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_physical_slots_status
(
  byte                               tlv_id,
  dsm_item_type                   ** response_pptr,
  const uim_phy_slot_status_type   * slot_status_ptr,
  uint8                              num_slots
);

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_BOOLEAN()

  DESCRIPTION
    Composes the TLV content from the value of boolean that is passed in

  PARAMETERS
    response_ptr  : output value
    valid_ptr     : output valid ptr
    is_true       : boolean

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_response_boolean
(
  uint8                      *response_ptr,
  uint8                      *valid_ptr,
  boolean                     is_true
);

/*===========================================================================
  FUNCTION QMI_UIM_IS_ISIM_AID()

  DESCRIPTION
    Checks if the current AID belongs to a ISIM or not

  PARAMETERS
    aid_ptr       : value of the AID

  RETURN VALUE
    TRUE for ISIM. FALSE in all other cases

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_is_isim_aid
(
  const mmgsdi_static_data_type * aid_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_IS_USIM_AID()

  DESCRIPTION
    Checks if the current AID belongs to a USIM or not.

  PARAMETERS
    aid_ptr             : value of the AID
    partial_aid_match_ok: dictates whether caller is ok with going ahead
                          with checking for a partial AID match incase
                          complete AID did not match with USIM

  RETURN VALUE
    boolean
      TRUE        : if the AID matched (completely or partially depending on
                    input parameter).
      FALSE       : in all other cases

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_is_usim_aid
(
  const mmgsdi_static_data_type * aid_ptr,
  boolean                         partial_aid_match_ok
);

/*===========================================================================
  FUNCTION QMI_UIM_IS_CSIM_AID()

  DESCRIPTION
    Checks if the current AID belongs to a CSIM or not.

  PARAMETERS
    aid_ptr             : value of the AID
    partial_aid_match_ok: dictates whether caller is ok with going ahead
                          with checking for a partial AID match incase
                          complete AID did not match with USIM

  RETURN VALUE
    boolean
      TRUE        : if the AID matched (completely or partially depending on
                    input parameter).
      FALSE       : in all other cases

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_is_csim_aid
(
  const   mmgsdi_static_data_type * aid_ptr,
  boolean                           partial_aid_match_ok
);

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_TERMINAL_CAPABILITY()

  DESCRIPTION
    Composes the TLV content of GET terminal capability

  PARAMETERS
    num_ter_capb_ptr      : output number of capability
    ter_capb_valid_ptr    : output cpability is valid or not
    response_pptr         : output capability ptr
    num_tlvs              : Number of TLVs
    tlvs_data_len         : Total length of the ptr containing the TLVs
    tlvs_data_ptr         : Ptr containing the TLVs

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_response_terminal_capability
(
  uint32                                    *num_ter_capb_ptr,
  uint8                                     *ter_capb_valid_ptr,
  uim_terminal_capability_tlv_resp_type_v01 *ter_cabp_resp_ptr,
  uint8                                      num_tlvs,
  uint8                                      tlvs_data_len,
  uint8*                                     tlvs_data_ptr
);

/*===========================================================================
  FUNCTION  QMI_UIM_RESPONSE_ENCRYPTED_STATUS()

  DESCRIPTION
    Encrypts and builds the response returned to security sensitive clients

  PARAMETERS
    cmd_id             : Command id
    request_id         : Request id
    result             : SimLock result status
    response_ptr       : Output sdu
    len_ptr            : IN/OUT len

  RETURN VALUE
    errval             : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_response_encrypted_status
(
  uint16                       cmd_id,
  uint16                       request_id,
  qmi_error_type_v01           error,
  uint8                       *response_ptr,
  uint32                      *len_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_POPULATE_FILE_ID()

  DESCRIPTION
    populate file id

  PARAMETERS
    file_id_tlv:  input file id
    file_id_ptr : output with file id value

  RETURN VALUE
    QMI_ERR_NONE_V01 on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_populate_file_id
(
  uim_file_id_type_v01      *file_id_tlv,
  qmi_uim_tlv_file_id_type * file_id_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_MAP_FILE_CONTROL_INFO_TO_MMGSDI()

  DESCRIPTION
    map file control information request uim type to mmgsdi type

  PARAMETERS
    uim_file_ctrl_info     : input file control info
    mmgsdi_file_ctrl_info  : output with ok file control information request type value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
 qmi_error_type_v01 qmi_uim_map_file_control_info_to_mmgsdi
(
  uim_file_control_information_enum_v01 uim_file_ctrl_info,
  mmgsdi_select_rsp_enum_type         * mmgsdi_file_ctrl_info
);

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_AUTH_COMMON()

  DESCRIPTION
    Composes the TLV content of cave authentication

  PARAMETERS
    content_valid_ptr       : output valid flag
    content_len_ptr         : output len
    content_ptr             : output result
    content_buf_size        : szie of content buf[In]
    cnf_ptr                 : cnf pointer
    cnf_type                : cnf type
    status                  : status
    auth_context            : auth context 
    
  RETURN VALUE
    qmi_error_type_v01

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_response_auth_common
(
  uint8_t                    *content_valid_ptr,
  uint32_t                   *content_len_ptr,
  uint8_t                    *content_ptr,
  const uint32_t              content_buf_size,
  mmgsdi_cnf_type            *cnf_ptr,
  mmgsdi_cnf_enum_type        cnf_type,
  mmgsdi_return_enum_type     status,
  uim_auth_context_enum_v01   auth_context
);

/*===========================================================================
  FUNCTION QMI_UIM_PARSE_REGISTER_REFRESH()

  DESCRIPTION
    Parses the file register for refresh

  PARAMETERS
    file_list_in_ptr               : input file list
    num_file                       : number of files in file list
    file_list_out_ptr              : output file list

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_parse_register_refresh_new
(
  uim_register_refresh_file_id_type_v01                  * file_list_in_ptr,
  uint32_t                                                 num_file,
  qmi_uim_files_list_type                                * file_list_out_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_TLV_SLOT()

  DESCRIPTION
    Map uim slot id to mmgsi slot id and validate the slot.

  PARAMETERS
    uim_slot        : input with slot value
    mmgsdi_slot_ptr : output with slot value

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
 qmi_error_type_v01 qmi_uim_map_and_validate_slot
(
  mmgsdi_slot_id_enum_type *mmgsdi_slot_ptr,
  uim_slot_enum_v01         uim_slot
);

/*===========================================================================
  FUNCTION QMI_UIM_TLV_DECRYPTED_PIN()

  DESCRIPTION
    Parses the TLV item of encrypted PIN and decodes it.

  PARAMETERS
    decrypt_data_ptr            : output deccryptrd data
    encrypt_pin_ptr             : encrypted input data
    
  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
 qmi_error_type_v01 qmi_uim_tlv_decrypted_pin
(
  mmgsdi_static_data_type               * decrypt_data_ptr,
  uim_encrypted_pin_type_v01            * encrypt_pin_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_PARSE_REGISTER_REFRESH()

  DESCRIPTION
    Parses the files register for refresh

  PARAMETERS
    file_list_in_ptr               : input file list
    num_file                       : number of files in file list
    file_list_out_ptr              : output file list

  RETURN VALUE
    QMI_ERR_NONE on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_parse_register_refresh
(
  uim_register_refresh_file_id_type_v01                  * file_list_in_ptr,
  uint32_t                                                 num_file,
  qmi_uim_files_list_type                                **file_list_out_pptr
);

#endif /* QMI_UIM_PARSING_H */
