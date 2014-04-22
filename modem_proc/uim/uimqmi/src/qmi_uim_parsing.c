/*================================================================================

DESCRIPTION


Copyright (c) 2009 - 2019 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_parsing.c#1 $$ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/30/19   tq      Fix Compilation warning 
02/22/19   tq      Fix the QCSI-QCCI migration FR releted errors
02/06/19   tq      Corrected addtional record length
01/14/19   tq      Wrongly return in case of multiple read record
01/23/19   tq      Make return type TRUE for terminal capability response
10/15/18   tq      Migration of QMI UIM service to QCCI-QCSI
08/14/18   tq      Featurize 3GPP2 code under FEATURE_MMGSDI_3GPP2
10/13/17   rps     Remove old  perso support 
12/05/16   bcho    Remove compilation error
09/29/16   sn      Added support to retrieve retry attempts left for locking
09/28/16   sn      Populate verify retries correctly
08/10/16   ar      Ensure better error handling for terminal cap API
08/10/16   ar      Support for setting terminal capability from AP
07/22/16   sp      Seperate SAP and ERA-GLONASS related functionality
06/16/16   vdc     Split QMI UIM parsing files for supporting IOE
05/26/16   sp      F3 logging improvements
05/20/16   ar      Reducing F3s for the parsing error
05/19/16   vdc     Split QMI UIM files for supporting IOE
05/16/16   ar      Featurizing code for IoE
03/09/16   vdc     Removed JCDMA related code
03/09/16   vdc     Featurize simlock/perso code under FEATURE_SIMLOCK
03/07/16   lm      Remove unneeded F3's
11/18/15   vdc     Fix parsing logic for CP & SP personalization
11/16/15   vdc     Decrease the check for minimum perso code len to 15
12/10/14   vdc     Added support for ERA-GLONASS
08/20/15   at      Replacement of Secapis to use a derived key & HMAC
08/10/15   stv     To send temporary unlock status indication
07/01/15   bcho    Fix enabling a lock with more than 20 codes of lock data
07/08/15   bcho    Redundant checks for SIMLOCK
05/15/15   vv      Support for RSU
05/14/15   ar      Support for terminate_app TLV in qmi_uim_close_session
05/04/15   vv      Support for more than 1 bytes of GID1/GID2
05/01/15   vdc     Added support for APN for bootstrapping
05/01/15   vdc     Added support for QMI UIM GBA
03/30/15   ar      Support for cross-mapping in multisim_auto_provisioning NV
03/19/15   at      Introducing new relock feature in SIMLock
02/24/15   vdc     Add USIM MBI EF to USIM path/enum conversion table
02/16/15   ar      Pass large buffer to SECAPI for decryption
02/03/15   xj      Add USB UICC and SAP CLIENT supported interface
12/16/14   av      Apply auth restr via NV for IIC only if USIM is selected
11/14/14   hh      Fix compilation warnings
10/14/14   tl      Add featurization to strip encryption and decryption
10/14/14   vv      Fix decrypting the simlock config operation
10/13/14   tl      Change error returned when CK length is zero
10/10/14   vv      Fix a merge error
10/10/14   vv      Add support for remote simlock configuration
10/08/14   vdc     Added support for UICC activation status indication
09/30/14   vv      Add support for emergency only command
09/25/14   tl      Add support for secure read
09/22/14   tl      Assign the decrypted PIN limit before decryption
09/17/14   tl      Add support for perso and deperso secure
08/29/14   tl      Add support for supply voltage command and indication
08/11/14   vv      Fixed compiler warnings
07/25/14   vv      Added support for perso command
07/22/14   at      Fix error in ISIM Auth response for different contexts
06/30/14   vv      Remove perso code
06/12/14   av      Enhance GET_SERVICE_STATUS to accomodate all service numbers
06/12/14   av      Cache mandatory EFs used by NVRUIM for critical 1x NV reads
05/22/14   tl      Added support for recovery complete indications
05/13/14   yt      Add CDMA PRL and EPRL files to RUIM enum-to-path table
05/13/14   vdc     Support for reduced card status indications
05/12/14   df      Add QMI support for status cmd
04/03/14   yt      Calculate card status TLV length based on number of apps
03/26/14   tl      SIM Lock secondary revisions
03/18/14   tl      Introduce new SIM Lock feature
02/25/14   yt      Support for QMI_UIM_RESELCT_REQ
02/25/14   bcho    Added support to store OPLMN List in EFS and cache it
02/24/14   at      Support for read transparent long indication
01/10/14   df      Use v2.0 diag macros
12/16/13   yt      Support for ISIM AUTH in HTTP digest security context
12/09/13   spo     Fix for handling auth request for USIM and ISIM
11/25/13   ar      Added Conversion from Path to Enum in CSIM table
11/20/13   tkl     Added API for Increase
10/09/13   spo     Fixed conversion of file access method from enum to path
10/09/13   spo     Fixed conversion from path to enum for third slot
10/04/13   vv      Added API to return se13 table release date and source
09/23/13   df      Support for SIM busy
09/16/13   yt      Remove redundant code
09/11/13   av      Handle new hotswap status CARD_UNKNOWN
09/02/13   df      Correct comments
08/08/13   ks      Support for subscription manager
07/26/13   av      Fix qmi_uim_tlv_aid to validate against the proper max AID
07/10/13   vdc     Added support for missing triple sim instances
07/02/13   yt      Dynamically allocate slot and app data
07/02/13   kb      Fix compilation error
06/28/13   kb      Added QMI malloc and free wrapper functions
06/27/13   spo     Added support for files under DF TELECOM
06/25/13   av      Add CSIM/CDMA SF_EUIMID to path conversion tables for caching
06/25/13   av      Support path to enum conversion for 7F66_PROP1_ACT_HPLMN EFs
06/21/13   kk      Updated support for triple sim
06/19/13   vv      Do not udpate PIN2 state based on app state
05/21/13   vv      Extend session closed indication for all sessions
04/30/13   spo     Replaced instances of memcpy with memscpy
02/22/13   yt      Support for slot 3
02/12/13   at      Added support for file status TLV
02/07/13   yt      Support for activation/deactivation of EFs
12/07/12   tl      Add SST/UST/CST EFs to path/enum conversion tables
11/09/12   yt      Move QMI UIM to UIM heap
11/02/12   yt      Add support for ACL in SET and GET SERVICE STATUS commands
09/10/12   at      Fixed compiler warnings
09/07/12   at      Removal of private DS functions and headers
09/07/12   at      Fixed parsing of SAP Connection Condition TLV
09/07/12   tl      Added support for SGLTE dual slots
08/29/12   at      Optional TlV in SAP connection req to check for active calls
08/28/12   spo     Added support for Session Closed Indication
08/16/12   at      New command handling for QMI_UIM_REFRESH_REGISTER_ALL
08/16/12   tl      Add session close without app termination functionality
05/23/12   hn      Enable 1-byte alignment packing on Windows environment
05/18/12   tl      Fixed Run Cave Algo response package
05/14/12   at      Add new TLV to QMI_UIM_GET_CARD_STATUS command
04/27/12   av      Removed FEATURE_UIM_SUPPORT_HOTSWAP feature flag
04/11/12   at      Indication support for sending long APDUs
03/02/12   vv      Added optional TLV in send APDU request
03/02/12   vv      Added support for querying ESN status
02/27/12   yt      Add function to construct file_length TLV
02/23/12   nmb     QMI QMUX Transition
01/03/12   nb      Added optional TLV for QMI configuration
12/21/11   nmb     Secure channel perso for apps
12/21/11   shr     Fixed compilation errors - GSDI Removal
12/20/11   hn      Add support for off-target testing
10/12/11   at      Handling intermediate get response in SAP commands
10/05/11   yt      Added support for refresh of non provisioning sessions
                   Removed flag to indicate if TLV is mandatory or optional
09/19/11   yt      Fixed compilation error
09/16/11   yt      Added optional TLV for status of hot swap switch
08/16/11   at      Indication support for various commands
08/01/11   at      Support for keyref_id TLV in PIN commands
08/01/11   at      Support for using session id on an open channel id
08/01/11   vs      Updating generic response copy to avoid write overflow
06/27/11   at      Renamed qmi_uim_response_sap_generic()
05/13/11   nmb     Added support for ISIM GBA Authentication
04/28/11   mib     Fixed crash due to invalid free reading multiple records
04/26/11   vs      Support for subscription ok request
04/25/11   mib     Removed mapping of some TELECOM files into MMGSDI enum
04/07/11   mib     Added support to read multiple records at the same time
04/01/11   mib     Changed TLV for the encrypted PIN passed for verify
03/29/11   mib     Fixed possible memory leak in refresh registration
03/15/11   mib     Added support for ISIM app type
03/09/11   at      Added support for PIN1 encryption/decryption
03/03/11   mib     Added support to manage logical channels to send APDUs
03/01/11   at      Added support for SAP request & indication
01/12/11   mib     Added support for CSIM file paths
01/04/11   mib     Added support for Compute IP Authentication
12/21/10   at      Added support for SAP connection request
12/17/10   mib     Split APDU request into two TLVs
12/09/10   at      Added support for send APDU request
11/08/10   mib     Fixed Klokwork warning in refresh indication
11/02/10   mib     Added additional check on path length
10/22/10   mib     Added support for get configuration request
10/12/10   mib     Support for refresh by path
09/20/10   mib     Fixed authenticate response
09/09/10   mib     Changed PIN status in TLV when PIN is not initialized
08/25/10   yt      Updated file_type for MF and DF in get file attr response
08/05/10   at      Updated GSM & USIM EF mapping tables with new EFs
08/05/10   tkl     QMI UIM & QMI CAT split from data package
08/04/10   at      Fixed current pointer updation for register refresh
07/26/10   mib     Return QMI_ERR_INVALID_ARG for wrong path length
07/08/10   mib     Added check on slot 2 for single slot devices
07/08/10   mib     Added support for QMI_ERR_INVALID_ARG
07/07/10   yt      Updated TLV parsing for session change
07/07/10   mib     Better error handling for QMI_ERR_ARG_TOO_LONG
06/18/10   tkl     Added enums support for refresh and handle telecom files
                   conversion
06/18/10   mib     Fixed Klokwork warnings: uninitialized security mask
                   Fixed AID value in case of multiple applications
06/16/10   mib     Added support for hidden key
06/10/10   mib     Fixed number of retries for PIN operations
06/07/10   mib     Added support for GET LABEL and SESSION CHANGE requests
06/03/10   mib     Fixed compilation warnings
05/15/10   mib     Improved mechanism to pass TLV id for responses
05/13/10   mib     Fixed SW1/SW2 in the response
04/15/10   mib     Fixed session type for 1X provisioning.
04/06/10   mib     Added support for authenticate and close session.
                   Changed TLV ids for consistency with other QMI services
04/05/10   at      Fixed handling for TLV length in refresh indications
01/13/10   mib     Initial version
=============================================================================*/


#include "uim_variation.h"
#include "customer.h"
#include "uim_msg.h"

#include "comdef.h"
#include "dsm.h"
#include "amssassert.h"
#include "modem_mem.h"
#include <stringl/stringl.h>

#include "qmi_svc_defs.h"
#include "qmi_svc_utils.h"
#include "qmi_uim_parsing.h"
#include "qmi_uim_util.h"
#include "qmi_uim_cat_common.h"
#include "qmi_uim_encryption.h"
#include "qmi_uim_simlock_parsing.h"
#include "qmi_uim_sub_mgr_parsing.h"
#include "qmi_uim_sub_mgr.h"
#include "uimi.h"
#include "user_identity_module_v01.h"

/* These includes need to always be at bottom - offtarget use only */
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*---------------------------------------------------------------------------
  Minimum length of Request TLVs with variable size
---------------------------------------------------------------------------*/
#define QMI_UIM_TLV_MIN_LEN_SESSION_INFORMATION       2
#define QMI_UIM_TLV_MIN_LEN_FILE_ID                   3
#define QMI_UIM_TLV_MIN_LEN_SET_PIN_PROTECTION        3
#define QMI_UIM_TLV_MIN_LEN_VERIFY_PIN                2
#define QMI_UIM_TLV_MIN_LEN_UNBLOCK_PIN               3
#define QMI_UIM_TLV_MIN_LEN_CHANGE_PIN                3
#define QMI_UIM_TLV_MIN_LEN_APPLICATION_INFORMATION   2
#define QMI_UIM_TLV_MIN_LEN_AID                       1
#define QMI_UIM_TLV_MIN_LEN_ENCRYPTED_PIN             1
#define QMI_UIM_TLV_MAX_ENCRYPTED_IMSI_LEN            255

/*---------------------------------------------------------------------------
  Get Config Mask Values
---------------------------------------------------------------------------*/
#define UIM_GET_CONFIGURATION_AUTOMATIC_SELECTION    (0x01)
#define UIM_GET_CONFIGURATION_PERSONALIZATION_STATUS (0x02)
#define UIM_GET_CONFIGURATION_HALT_SUBSCRIPTION      (0x04)
#define UIM_GET_CONFIGURATION_USB_UICC_SUPPORTED     (0x08)
#define UIM_GET_CONFIGURATION_SAP_CLIENT_SUPPORTED   (0x10)

/*---------------------------------------------------------------------------
  Length of ICCID
---------------------------------------------------------------------------*/
#define QMI_UIM_MAX_ICCID_SIZE                        10

/*---------------------------------------------------------------------------
  Maximum size of lock codes
---------------------------------------------------------------------------*/
#define QMI_UIM_TLV_PERSO_NUM_NW_MAX             (85)
#define QMI_UIM_TLV_PERSO_NUM_NS_MAX             (64)
#define QMI_UIM_TLV_PERSO_NUM_SP_MAX             (64)
#define QMI_UIM_TLV_PERSO_NUM_CP_MAX             (51)
#define QMI_UIM_TLV_PERSO_NUM_SIM_MAX            (32)
#define QMI_UIM_TLV_PERSO_NUM_NW_TYPE2_MAX       (128)

/*---------------------------------------------------------------------------
  Structure with map between file path and file enum
---------------------------------------------------------------------------*/
typedef struct
{
  uint16                 file_id;
  mmgsdi_file_enum_type  mmgsdi_enum;
} qmi_uim_ef_map_type;

/*---------------------------------------------------------------------------
  Structure used for TLV composition of card status
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  uint8  card_state;
  uint8  upin_state;
  uint8  upin_retries;
  uint8  upuk_retries;
  uint8  error_code;
  uint8  num_app;
} tlv_data_card_state_type;

typedef PACKED struct PACKED_POST
{
  uint8  app_type;
  uint8  app_state;
  uint8  perso_state;
  uint8  perso_feature;
  uint8  perso_retries;
  uint8  perso_unblock_retries;
  uint8  aid_len;
} tlv_data_app_state_top_type;

typedef PACKED struct PACKED_POST
{
  uint8  univ_pin;
  uint8  pin1_state;
  uint8  pin1_retries;
  uint8  puk1_retries;
  uint8  pin2_state;
  uint8  pin2_retries;
  uint8  puk2_retries;
} tlv_data_app_state_bottom_type;

/*---------------------------------------------------------------------------
  Tables with map between file path and file enum
---------------------------------------------------------------------------*/
#define QMI_UIM_GSM_TABLE(table, path)          \
         const uint16 path[] =                  \
         { 0x3F00, 0x7F20 };                    \
         const qmi_uim_ef_map_type table[] =    \
         {                                      \
           { 0x6F05 , MMGSDI_GSM_LP          }, \
           { 0x6F07 , MMGSDI_GSM_IMSI        }, \
           { 0x6F13 , MMGSDI_GSM_CFF         }, \
           { 0x6F14 , MMGSDI_GSM_ONS         }, \
           { 0x6F15 , MMGSDI_GSM_CSP         }, \
           { 0x6F20 , MMGSDI_GSM_KC          }, \
           { 0x6F30 , MMGSDI_GSM_PLMN        }, \
           { 0x6F31 , MMGSDI_GSM_HPLMN       }, \
           { 0x6F37 , MMGSDI_GSM_ACM_MAX     }, \
           { 0x6F38 , MMGSDI_GSM_SST         }, \
           { 0x6F39 , MMGSDI_GSM_ACM         }, \
           { 0x6F3E , MMGSDI_GSM_GID1        }, \
           { 0x6F3F , MMGSDI_GSM_GID2        }, \
           { 0x6F46 , MMGSDI_GSM_SPN         }, \
           { 0x6F41 , MMGSDI_GSM_PUCT        }, \
           { 0x6F45 , MMGSDI_GSM_CBMI        }, \
           { 0x6F74 , MMGSDI_GSM_BCCH        }, \
           { 0x6F78 , MMGSDI_GSM_ACC         }, \
           { 0x6F7B , MMGSDI_GSM_FPLMN       }, \
           { 0x6F7E , MMGSDI_GSM_LOCI        }, \
           { 0x6FAD , MMGSDI_GSM_AD          }, \
           { 0x6FAE , MMGSDI_GSM_PHASE       }, \
           { 0x6FB1 , MMGSDI_GSM_VGCS        }, \
           { 0x6FC5 , MMGSDI_GSM_PNN         }, \
           { 0x6FC6 , MMGSDI_GSM_OPL         }, \
           { 0x6FC7 , MMGSDI_GSM_MBDN        }, \
           { 0x6FB2 , MMGSDI_GSM_VGCSS       }, \
           { 0x6FB3 , MMGSDI_GSM_VBS         }, \
           { 0x6FB4 , MMGSDI_GSM_VBSS        }, \
           { 0x6FB5 , MMGSDI_GSM_EMLPP       }, \
           { 0x6FB6 , MMGSDI_GSM_AAEM        }, \
           { 0x6F50 , MMGSDI_GSM_CBMIR       }, \
           { 0x6F32 , MMGSDI_GSM_CNL         }, \
           { 0x6F51 , MMGSDI_GSM_NIA         }, \
           { 0x6F63 , MMGSDI_GSM_CPBCCH      }, \
           { 0x6FCA , MMGSDI_GSM_MWIS        }, \
           { 0x6FCD , MMGSDI_GSM_SPDI        }, \
           { 0x6FCB , MMGSDI_GSM_CFIS        }, \
           { 0x6F61 , MMGSDI_GSM_OPLMNWACT   }  \
         };

#define QMI_UIM_RUIM_TABLE(table, path)                     \
         const uint16 path[] =                              \
         { 0x3F00, 0x7F25 };                                \
         const qmi_uim_ef_map_type table[] =                \
         {                                                  \
           { 0x6F22 , MMGSDI_CDMA_IMSI_M                 }, \
           { 0x6F23 , MMGSDI_CDMA_IMSI_T                 }, \
           { 0x6F41 , MMGSDI_CDMA_HOME_SVC_PVDR_NAME     }, \
           { 0x6F74 , MMGSDI_CDMA_SF_EUIM_ID             }, \
           { 0x6F30 , MMGSDI_CDMA_PRL                    }, \
           { 0x6F5A , MMGSDI_CDMA_EPRL                   }  \
         };

#define QMI_UIM_USIM_TABLE(table, path)         \
         const uint16 path[] =                  \
         { 0x3F00, 0x7FFF };                    \
         const qmi_uim_ef_map_type table[] =    \
         {                                      \
           { 0x6F05 , MMGSDI_USIM_LI         }, \
           { 0x6F07 , MMGSDI_USIM_IMSI       }, \
           { 0x6F13 , MMGSDI_USIM_CFF        }, \
           { 0x6F14 , MMGSDI_USIM_ONS        }, \
           { 0x6F15 , MMGSDI_USIM_CSP        }, \
           { 0x6F20 , MMGSDI_USIM_KEYS       }, \
           { 0x6F08 , MMGSDI_USIM_KEYS       }, \
           { 0x6F09 , MMGSDI_USIM_KEYSPS     }, \
           { 0x6F60 , MMGSDI_USIM_PLMNWACT   }, \
           { 0x6F30 , MMGSDI_USIM_PLMNWACT   }, \
           { 0x6F31 , MMGSDI_USIM_HPLMN      }, \
           { 0x6F37 , MMGSDI_USIM_ACM_MAX    }, \
           { 0x6F38 , MMGSDI_USIM_UST        }, \
           { 0x6F39 , MMGSDI_USIM_ACM        }, \
           { 0x6F3E , MMGSDI_USIM_GID1       }, \
           { 0x6F3F , MMGSDI_USIM_GID2       }, \
           { 0x6F46 , MMGSDI_USIM_SPN        }, \
           { 0x6F41 , MMGSDI_USIM_PUCT       }, \
           { 0x6F45 , MMGSDI_USIM_CBMI       }, \
           { 0x6F78 , MMGSDI_USIM_ACC        }, \
           { 0x6F7B , MMGSDI_USIM_FPLMN      }, \
           { 0x6F7E , MMGSDI_USIM_LOCI       }, \
           { 0x6FAD , MMGSDI_USIM_AD         }, \
           { 0x6F48 , MMGSDI_USIM_CBMID      }, \
           { 0x6FB7 , MMGSDI_USIM_ECC        }, \
           { 0x6F50 , MMGSDI_USIM_CBMIR      }, \
           { 0x6F73 , MMGSDI_USIM_PSLOCI     }, \
           { 0x6F3B , MMGSDI_USIM_FDN        }, \
           { 0x6F3C , MMGSDI_USIM_SMS        }, \
           { 0x6F40 , MMGSDI_USIM_MSISDN     }, \
           { 0x6F42 , MMGSDI_USIM_SMSP       }, \
           { 0x6FC5 , MMGSDI_USIM_PNN        }, \
           { 0x6FC6 , MMGSDI_USIM_OPL        }, \
           { 0x6FC7 , MMGSDI_USIM_MBDN       }, \
           { 0x6FCB , MMGSDI_USIM_CFIS       }, \
           { 0x6FCA , MMGSDI_USIM_MWIS       }, \
           { 0x6F4E , MMGSDI_USIM_EXT5       }, \
           { 0x6F49 , MMGSDI_USIM_SDN        }, \
           { 0x6F4B , MMGSDI_USIM_EXT2       }, \
           { 0x6F4C , MMGSDI_USIM_EXT3       }, \
           { 0x6F4D , MMGSDI_USIM_BDN        }, \
           { 0x6F55 , MMGSDI_USIM_EXT4       }, \
           { 0x6F81 , MMGSDI_USIM_OCI        }, \
           { 0x6F80 , MMGSDI_USIM_ICI        }, \
           { 0x6F47 , MMGSDI_USIM_SMSR       }, \
           { 0x6FB5 , MMGSDI_USIM_EMLPP      }, \
           { 0x6FB6 , MMGSDI_USIM_AAEM       }, \
           { 0x6F56 , MMGSDI_USIM_EST        }, \
           { 0x6F57 , MMGSDI_USIM_ACL        }, \
           { 0x6F32 , MMGSDI_USIM_CNL        }, \
           { 0x6FCD , MMGSDI_USIM_SPDI       }, \
           { 0x6FCF , MMGSDI_USIM_EXT8       }, \
           { 0x6FD6 , MMGSDI_USIM_GBABP      }, \
           { 0x6FDB , MMGSDI_USIM_EHPLMNPI   }, \
           { 0x6FDC , MMGSDI_USIM_LRPLMNSI   }, \
           { 0x6FDE , MMGSDI_USIM_SPNI       }, \
           { 0x6FDF , MMGSDI_USIM_PNNI       }, \
           { 0x6FE2 , MMGSDI_USIM_NCP_IP     }, \
           { 0x6F62 , MMGSDI_USIM_HPLMNWACT  }, \
           { 0x6F4F , MMGSDI_USIM_CCP2       }, \
           { 0x6FC3 , MMGSDI_USIM_HIDDENKEY  }, \
           { 0x6F61 , MMGSDI_USIM_OPLMNWACT  }, \
           { 0x6FC9 , MMGSDI_USIM_MBI        }  \
         };

#define QMI_UIM_CSIM_TABLE(table, path)          \
         const uint16 path[] =                   \
         { 0x3F00, 0x7FFF };                     \
         const qmi_uim_ef_map_type table[] =     \
         {                                       \
           { 0x6F22 , MMGSDI_CSIM_IMSI_M      }, \
           { 0x6F23 , MMGSDI_CSIM_IMSI_T      }, \
           { 0x6F31 , MMGSDI_CSIM_RUIM_ID     }, \
           { 0x6F32 , MMGSDI_CSIM_CST         }, \
           { 0x6F75 , MMGSDI_CSIM_EST         }, \
           { 0x6F3B , MMGSDI_CSIM_FDN         }, \
           { 0x6F44 , MMGSDI_CSIM_MSISDN      }, \
           { 0x6F30 , MMGSDI_CSIM_PRL         }, \
           { 0x6F5A , MMGSDI_CSIM_EPRL        }, \
           { 0x6F45 , MMGSDI_CSIM_MAXIMUM_PRL }, \
           { 0x6F7A , MMGSDI_CSIM_EXT2        }, \
           { 0x6F7B , MMGSDI_CSIM_EXT3        }, \
           { 0x6F69 , MMGSDI_CSIM_MMS_UCP     }, \
           { 0x6F6D , MMGSDI_CSIM_GID1        }, \
           { 0x6F6E , MMGSDI_CSIM_GID2        }, \
           { 0x6F6F , MMGSDI_CSIM_CNL         }, \
           { 0x6F7D , MMGSDI_CSIM_OCI         }, \
           { 0x6F7C , MMGSDI_CSIM_ICI         }, \
           { 0x6F7E , MMGSDI_CSIM_EXT5        }, \
           { 0x6F8D , MMGSDI_CSIM_JDL         }, \
           { 0x6F41 , MMGSDI_CSIM_SPN         }, \
           { 0x6F74 , MMGSDI_CSIM_SF_EUIM_ID  }, \
           { 0x6F47 , MMGSDI_CSIM_ECC         }, \
           { 0x6F3A , MMGSDI_CSIM_LANG_IND    }, \
           { 0x6F76 , MMGSDI_CSIM_HIDDEN_KEY  }  \
         };

#define QMI_UIM_MF_TABLE(table, path)          \
         const uint16 path[]  =                \
         { 0x3F00 };                           \
         const qmi_uim_ef_map_type table[] =   \
         {                                     \
           { 0x2FE2, MMGSDI_ICCID     },       \
           { 0x2F05, MMGSDI_ELP       },       \
           { 0x2F00, MMGSDI_DIR       },       \
           { 0x2F06, MMGSDI_ARR       }        \
         };

#define QMI_UIM_TELECOM_TABLE(table, path)      \
         const uint16 path[] =                  \
         { 0x3F00, 0x7F10 };                    \
         const qmi_uim_ef_map_type table[] =    \
         {                                      \
           { 0x6F3B , MMGSDI_TELECOM_FDN     }, \
           { 0x6F3C , MMGSDI_TELECOM_SMS     }, \
           { 0x6F4F , MMGSDI_TELECOM_ECCP    }, \
           { 0x6F40 , MMGSDI_TELECOM_MSISDN  }, \
           { 0x6F42 , MMGSDI_TELECOM_SMSP    }, \
           { 0x6F43 , MMGSDI_TELECOM_SMSS    }, \
           { 0x6F44 , MMGSDI_TELECOM_LND     }, \
           { 0x6F49 , MMGSDI_TELECOM_SDN     }, \
           { 0x6F4C , MMGSDI_TELECOM_EXT3    }, \
           { 0x6F4D , MMGSDI_TELECOM_BDN     }, \
           { 0x6F4E , MMGSDI_TELECOM_EXT4    }, \
           { 0x6F47 , MMGSDI_TELECOM_SMSR    }, \
           { 0x6FE0 , MMGSDI_TELECOM_ICE_DN  }, \
           { 0x6FE1 , MMGSDI_TELECOM_ICE_FF  }  \
         };

#define QMI_UIM_TELECOM_GRAPHICS_TABLE(table, path)     \
         const uint16 path[] =                          \
         { 0x3F00, 0x7F10, 0x5F50 };                    \
         const qmi_uim_ef_map_type table[] =            \
         {                                              \
           { 0x4F21, MMGSDI_TELECOM_ICE_GRAPHICS }      \
         };

#define QMI_UIM_PROP_7F66_2G_TABLE(table, path)          \
         const uint16 path[] =                           \
         { 0x3F00, 0x7F66, 0x5F30 };                     \
         const qmi_uim_ef_map_type table[] =             \
         {                                               \
           { 0x4F34, MMGSDI_SIM_7F66_PROP1_ACT_HPLMN  }  \
         };

#define QMI_UIM_PROP_7F66_3G_TABLE(table, path)         \
         const uint16 path[] =                          \
         { 0x3F00, 0x7FFF, 0x7F66, 0x5F30 };            \
         const qmi_uim_ef_map_type table[] =            \
         {                                              \
           { 0x4F34, MMGSDI_USIM_7F66_PROP1_ACT_HPLMN } \
         };

/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD_TABLE()

  DESCRIPTION
    Converts file access method between path and MMGSDI enum
    using a specified input table

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method
    table_ptr       : table to be used for the conversion
    table_size      : table size

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_convert_file_access_method_table
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method,
  const qmi_uim_ef_map_type      * table_ptr,
  uint16                           table_size,
  const uint16                   * table_path_ptr,
  uint8                            table_path_len
)
{
  uint32 index    = 0;

  ASSERT(file_access_ptr);
  ASSERT(table_ptr);
  ASSERT(table_size > 0);
  ASSERT(table_path_ptr);
  ASSERT(table_path_len < MMGSDI_MAX_PATH_LEN);


  /* Check file access method */
  if (file_access_ptr->access_method == access_method)
  {
    /* Input format matches output format, so we can
       immediately return success */
    UIM_MSG_HIGH_0("No file access conversion required");
    return TRUE;
  }

  if (file_access_ptr->access_method == MMGSDI_BY_PATH_ACCESS &&
      access_method == MMGSDI_EF_ENUM_ACCESS)
  {
    /* Conversion from PATH to ENUM */

    /* Check length */
    if ((file_access_ptr->file.path_type.path_len != table_path_len + 1) ||
        memcmp(file_access_ptr->file.path_type.path_buf, table_path_ptr, table_path_len * sizeof(uint16)) != 0)
    {
      UIM_MSG_HIGH_1("QMI_UIM invalid path or path len: 0x%x",
                     file_access_ptr->file.path_type.path_len);
      return FALSE;
    }

    /* loop on all items in the table */
    for (index = 0; index < table_size; index++)
    {
      if (table_ptr[index].file_id == file_access_ptr->file.path_type.path_buf[table_path_len])
      {
        file_access_ptr->access_method = MMGSDI_EF_ENUM_ACCESS;
        file_access_ptr->file.file_enum = table_ptr[index].mmgsdi_enum;
        return TRUE;
      }
    }
    UIM_MSG_HIGH_0("QMI_UIM conversion from path to enum failed");
  }
  else if (file_access_ptr->access_method == MMGSDI_EF_ENUM_ACCESS &&
           access_method == MMGSDI_BY_PATH_ACCESS)
  {
    /* Conversion from ENUM to PATH */

    /* loop on all items in the table */
    for (index = 0; index < table_size; index++)
    {
      if (table_ptr[index].mmgsdi_enum == file_access_ptr->file.file_enum)
      {
        file_access_ptr->access_method = MMGSDI_BY_PATH_ACCESS;
        file_access_ptr->file.path_type.path_len = table_path_len + 1;
        (void)memscpy((void*)file_access_ptr->file.path_type.path_buf,
                      sizeof(file_access_ptr->file.path_type.path_buf),
                      (void*)table_path_ptr,
                      table_path_len * sizeof(uint16));
        file_access_ptr->file.path_type.path_buf[table_path_len] = table_ptr[index].file_id;
        return TRUE;
      }
    }
    UIM_MSG_HIGH_0("QMI_UIM conversion from enum to path failed");
  }
  else
  {
    /* Invalid conversion */
    UIM_MSG_HIGH_2("QMI_UIM conversion from 0x%x to 0x%x is impossible",
                   file_access_ptr->access_method, access_method);
  }

  return FALSE;
} /* qmi_uim_convert_file_access_method_table */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD_TELECOM()

  DESCRIPTION
    Creates the table with TELECOM files and invokes the
    function to execute the conversion based on that table

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_convert_file_access_method_telecom
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method
)
{
  boolean result = FALSE;
  QMI_UIM_TELECOM_TABLE(telecom_table, telecom_path);

  ASSERT(file_access_ptr);

  result = qmi_uim_convert_file_access_method_table(
                file_access_ptr,
                access_method,
                telecom_table,
                sizeof(telecom_table) / sizeof(qmi_uim_ef_map_type),
                telecom_path,
                sizeof(telecom_path) / sizeof(uint16));

  return result;
} /* qmi_uim_convert_file_access_method_telecom */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD_GSM()

  DESCRIPTION
    Creates the table with GSM files and invokes the
    function to execute the conversion based on that table

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_convert_file_access_method_gsm
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method
)
{
  boolean result = FALSE;
  QMI_UIM_GSM_TABLE(gsm_table, gsm_path);

  ASSERT(file_access_ptr);

  result = qmi_uim_convert_file_access_method_table(
                file_access_ptr,
                access_method,
                gsm_table,
                sizeof(gsm_table) / sizeof(qmi_uim_ef_map_type),
                gsm_path,
                sizeof(gsm_path) / sizeof(uint16));

  return result;
} /* qmi_uim_convert_file_access_method_gsm */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD_RUIM()

  DESCRIPTION
    Creates the table with RUIM files and invokes the
    function to execute the conversion based on that table

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_convert_file_access_method_ruim
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  boolean result = FALSE;
  QMI_UIM_RUIM_TABLE(ruim_table, ruim_path);

  ASSERT(file_access_ptr);

  result = qmi_uim_convert_file_access_method_table(
                file_access_ptr,
                access_method,
                ruim_table,
                sizeof(ruim_table) / sizeof(qmi_uim_ef_map_type),
                ruim_path,
                sizeof(ruim_path) / sizeof(uint16));

  return result;
#else 
  (void)file_access_ptr;
  (void)access_method;
  return FALSE;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* qmi_uim_convert_file_access_method_ruim */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD_USIM()

  DESCRIPTION
    Creates the table with USIM files and invokes the
    function to execute the conversion based on that table

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_convert_file_access_method_usim
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method
)
{
  boolean result = FALSE;
  QMI_UIM_USIM_TABLE(usim_table, usim_path);

  ASSERT(file_access_ptr);

  result = qmi_uim_convert_file_access_method_table(
                file_access_ptr,
                access_method,
                usim_table,
                sizeof(usim_table) / sizeof(qmi_uim_ef_map_type),
                usim_path,
                sizeof(usim_path) / sizeof(uint16));

  return result;
} /* qmi_uim_convert_file_access_method_usim */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD_CSIM()

  DESCRIPTION
    Creates the table with CSIM files and invokes the
    function to execute the conversion based on that table

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_convert_file_access_method_csim
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  boolean result = FALSE;
  QMI_UIM_CSIM_TABLE(csim_table, csim_path);

  ASSERT(file_access_ptr);

  result = qmi_uim_convert_file_access_method_table(
                file_access_ptr,
                access_method,
                csim_table,
                sizeof(csim_table) / sizeof(qmi_uim_ef_map_type),
                csim_path,
                sizeof(csim_path) / sizeof(uint16));

  return result;
#else
  (void)file_access_ptr;
  (void)access_method;
  return FALSE;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* qmi_uim_convert_file_access_method_csim */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD_MF()

  DESCRIPTION
    Creates the table with miscellaneous files (files not under
    GSM, USIM or CDMA) and invokes the function to execute the
    conversion based on that table

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_convert_file_access_method_mf
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method
)
{
  boolean result = FALSE;
  QMI_UIM_MF_TABLE(mf_table, mf_path);

  ASSERT(file_access_ptr);

  result = qmi_uim_convert_file_access_method_table(
                file_access_ptr,
                access_method,
                mf_table,
                sizeof(mf_table) / sizeof(qmi_uim_ef_map_type),
                mf_path,
                sizeof(mf_path) / sizeof(uint16));

  return result;
} /* qmi_uim_convert_file_access_method_mf */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD_PROP_7F66_2G()

  DESCRIPTION
    Creates the table with propprietary 2G files and invokes the
    function to execute the conversion based on that table

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_convert_file_access_method_prop_7f66_2g
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method
)
{
  boolean result = FALSE;
  QMI_UIM_PROP_7F66_2G_TABLE(prop_7f66_2g_table, prop_7f66_2g_path);

  ASSERT(file_access_ptr);

  result = qmi_uim_convert_file_access_method_table(
                file_access_ptr,
                access_method,
                prop_7f66_2g_table,
                sizeof(prop_7f66_2g_table) / sizeof(qmi_uim_ef_map_type),
                prop_7f66_2g_path,
                sizeof(prop_7f66_2g_path) / sizeof(uint16));

  return result;
} /* qmi_uim_convert_file_access_method_prop_7f66_2g */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD_PROP_7F66_3G()

  DESCRIPTION
    Creates the table with proprietary 3G files and invokes the
    function to execute the conversion based on that table

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_convert_file_access_method_prop_7f66_3g
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method
)
{
  boolean result = FALSE;
  QMI_UIM_PROP_7F66_3G_TABLE(prop_7f66_3g_table, prop_7f66_3g_path);

  ASSERT(file_access_ptr);

  result = qmi_uim_convert_file_access_method_table(
                file_access_ptr,
                access_method,
                prop_7f66_3g_table,
                sizeof(prop_7f66_3g_table) / sizeof(qmi_uim_ef_map_type),
                prop_7f66_3g_path,
                sizeof(prop_7f66_3g_path) / sizeof(uint16));

  return result;
} /* qmi_uim_convert_file_access_method_prop_7f66_3g */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_FILE_ACCESS_METHOD_TELECOM_GRAPHICS()

  DESCRIPTION
    Creates the table with Telecom Graphics files and invokes the
    function to execute the conversion based on that table

  PARAMETERS
    file_access_ptr : input/output with file value
    access_method   : desired output access method

  RETURN VALUE
    TRUE on success. FALSE on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_convert_file_access_method_telecom_graphics
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method
)
{
  boolean result = FALSE;
  QMI_UIM_TELECOM_GRAPHICS_TABLE(telecom_graphics_table, telecom_graphics_path);

  ASSERT(file_access_ptr);

  result = qmi_uim_convert_file_access_method_table(
                file_access_ptr,
                access_method,
                telecom_graphics_table,
                sizeof(telecom_graphics_table) / sizeof(qmi_uim_ef_map_type),
                telecom_graphics_path,
                sizeof(telecom_graphics_path) / sizeof(uint16));

  return result;
} /* qmi_uim_convert_file_access_method_telecom_graphics */


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
boolean qmi_uim_convert_file_access_method
(
  mmgsdi_access_type             * file_access_ptr,
  mmgsdi_access_method_enum_type   access_method,
  mmgsdi_session_type_enum_type    session_type
)
{
  ASSERT(file_access_ptr);

  /* Check file access method */
  if (file_access_ptr->access_method == access_method)
  {
    /* Input format matches output format, so we can
       immediately return success */
    UIM_MSG_HIGH_0("No file access conversion required");
    return TRUE;
  }

  switch(file_access_ptr->access_method)
  {
    case MMGSDI_BY_PATH_ACCESS:
      if (file_access_ptr->file.path_type.path_len == 2 &&
          file_access_ptr->file.path_type.path_buf[0] == 0x3F00)
      {
        return qmi_uim_convert_file_access_method_mf(file_access_ptr,
                                                     access_method);
      }
      if (file_access_ptr->file.path_type.path_len == 3 &&
          file_access_ptr->file.path_type.path_buf[0] == 0x3F00)
      {
        /* Look at the second item in the path to understand which
           function is required */
        if (file_access_ptr->file.path_type.path_buf[1] == 0x7F10)
        {
          return qmi_uim_convert_file_access_method_telecom(file_access_ptr,
                                                            access_method);
        }
        if (file_access_ptr->file.path_type.path_buf[1] == 0x7F20)
        {
          return qmi_uim_convert_file_access_method_gsm(file_access_ptr,
                                                        access_method);
        }
        if (file_access_ptr->file.path_type.path_buf[1] == 0x7F25)
        {
          return qmi_uim_convert_file_access_method_ruim(file_access_ptr,
                                                         access_method);
        }
        if (file_access_ptr->file.path_type.path_buf[1] == 0x7FFF)
        {
          if (session_type == MMGSDI_GW_PROV_PRI_SESSION ||
              session_type == MMGSDI_GW_PROV_SEC_SESSION ||
              session_type == MMGSDI_GW_PROV_TER_SESSION)
          {
            return qmi_uim_convert_file_access_method_usim(file_access_ptr,
                                                           access_method);
          }
          else if (session_type == MMGSDI_1X_PROV_PRI_SESSION ||
                   session_type == MMGSDI_1X_PROV_SEC_SESSION ||
                   session_type == MMGSDI_1X_PROV_TER_SESSION)
          {
            return qmi_uim_convert_file_access_method_csim(file_access_ptr,
                                                           access_method);
          }
          else
          {
            /* For non-provisioning sessions, don't convert the path
               into enum: nothing to do! */
          }
        }
      }
      if (file_access_ptr->file.path_type.path_len == 4 &&
          file_access_ptr->file.path_type.path_buf[0] == 0x3F00 &&
          file_access_ptr->file.path_type.path_buf[1] == 0x7F66 &&
          file_access_ptr->file.path_type.path_buf[2] == 0x5F30)
      {
        return qmi_uim_convert_file_access_method_prop_7f66_2g(file_access_ptr,
                                                               access_method);
      }
      if (file_access_ptr->file.path_type.path_len == 5 &&
          file_access_ptr->file.path_type.path_buf[0] == 0x3F00 &&
          file_access_ptr->file.path_type.path_buf[1] == 0x7FFF &&
          file_access_ptr->file.path_type.path_buf[2] == 0x7F66 &&
          file_access_ptr->file.path_type.path_buf[3] == 0x5F30)
      {
        if (session_type == MMGSDI_GW_PROV_PRI_SESSION ||
            session_type == MMGSDI_GW_PROV_SEC_SESSION ||
            session_type == MMGSDI_GW_PROV_TER_SESSION)
        {
          return qmi_uim_convert_file_access_method_prop_7f66_3g(file_access_ptr,
                                                                 access_method);
        }
      }
      if (file_access_ptr->file.path_type.path_len == 4 &&
          file_access_ptr->file.path_type.path_buf[0] == 0x3F00 &&
          file_access_ptr->file.path_type.path_buf[1] == 0x7F10 &&
          file_access_ptr->file.path_type.path_buf[2] == 0x5F50)
      {
          return qmi_uim_convert_file_access_method_telecom_graphics(file_access_ptr,
                                                                     access_method);
      }
      break;

    case MMGSDI_EF_ENUM_ACCESS:
      /* Look at the range of the enum to understand which
         function is required */
      if (file_access_ptr->file.file_enum >= MMGSDI_ICCID &&
          file_access_ptr->file.file_enum <= MMGSDI_ARR)
      {
        return qmi_uim_convert_file_access_method_mf(file_access_ptr,
                                                     access_method);
      }
      if (file_access_ptr->file.file_enum >= MMGSDI_IMAGE_FILE &&
          file_access_ptr->file.file_enum <= MMGSDI_TELECOM_ICE_GRAPHICS)
      {
        return qmi_uim_convert_file_access_method_telecom_graphics(file_access_ptr,
                                                                   access_method);
      }
      if (file_access_ptr->file.file_enum >= MMGSDI_TELECOM_ADN &&
          file_access_ptr->file.file_enum <= MMGSDI_TELECOM_MMSSMODE)
      {
        return qmi_uim_convert_file_access_method_telecom(file_access_ptr,
                                                          access_method);
      }
      if (file_access_ptr->file.file_enum >= MMGSDI_GSM_LP &&
          file_access_ptr->file.file_enum <= MMGSDI_GSM_IMG3INST3)
      {
        return qmi_uim_convert_file_access_method_gsm(file_access_ptr,
                                                      access_method);
      }
      if (file_access_ptr->file.file_enum >= MMGSDI_CDMA_CC &&
          file_access_ptr->file.file_enum <= MMGSDI_CDMA_3GPD_OP_CAP)
      {
        return qmi_uim_convert_file_access_method_ruim(file_access_ptr,
                                                       access_method);
      }
      if (file_access_ptr->file.file_enum >= MMGSDI_USIM_LI &&
          file_access_ptr->file.file_enum <= MMGSDI_USIM_LRPLMNSI)
      {
        return qmi_uim_convert_file_access_method_usim(file_access_ptr,
                                                       access_method);
      }
      if (file_access_ptr->file.file_enum >= MMGSDI_CSIM_ARR &&
          file_access_ptr->file.file_enum <= MMGSDI_CSIM_PUID)
      {
        return qmi_uim_convert_file_access_method_csim(file_access_ptr,
                                                       access_method);
      }
      if (file_access_ptr->file.file_enum >= MMGSDI_SIM_7F66_PROP1_ACT_HPLMN &&
          file_access_ptr->file.file_enum <= MMGSDI_SIM_7F66_PROP1_SPT_TABLE)
      {
        return qmi_uim_convert_file_access_method_prop_7f66_2g(file_access_ptr,
                                                               access_method);
      }
      if (file_access_ptr->file.file_enum >= MMGSDI_USIM_7F66_PROP1_ACT_HPLMN &&
          file_access_ptr->file.file_enum <= MMGSDI_USIM_7F66_PROP1_RAT)
      {
        return qmi_uim_convert_file_access_method_prop_7f66_3g(file_access_ptr,
                                                               access_method);
      }
      break;

    default:
      /* Not supported */
      break;
  }

  return FALSE;
} /* qmi_uim_convert_file_access_method */


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
boolean qmi_uim_check_pin_digits
(
  mmgsdi_data_type  pin_value
)
{
  mmgsdi_len_type i = 0;

  if (pin_value.data_ptr == NULL)
  {
    return FALSE;
  }

  for (i = 0; i < pin_value.data_len; i++)
  {
    if (pin_value.data_ptr[i] < '0' ||
        pin_value.data_ptr[i] > '9')
    {
      UIM_MSG_ERR_1("QMI_UIM invalid PIN digit: 0x%x", pin_value.data_ptr[i]);
      return FALSE;
    }
  }

  return TRUE;
} /* qmi_uim_check_pin_digits */


/*===========================================================================
  FUNCTION QMI_UIM_MAP_AND_VALIDATE_SLOT()

  DESCRIPTION
    Map uim slot id to mmgsi slot id and validate the slot.

  PARAMETERS
    uim_slot        : input with slot value
    mmgsdi_slot_ptr : output with slot value

  RETURN VALUE
    QMI_ERR_NONE_V01 on success. Other error code on failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_map_and_validate_slot 
(
  mmgsdi_slot_id_enum_type *mmgsdi_slot_ptr,
  uim_slot_enum_v01         uim_slot
)
{
  qmi_error_type_v01 qmi_err = QMI_ERR_NONE_V01;

  ASSERT(mmgsdi_slot_ptr);

  switch (uim_slot)
  {
    case UIM_SLOT_1_V01:
      *mmgsdi_slot_ptr = MMGSDI_SLOT_1;
      break;
    case UIM_SLOT_2_V01:
      *mmgsdi_slot_ptr = MMGSDI_SLOT_2;
      break;
    case UIM_SLOT_3_V01:
      *mmgsdi_slot_ptr = MMGSDI_SLOT_3;
      break;
    case UIM_SLOT_4_V01:
    case UIM_SLOT_5_V01:
      return QMI_ERR_NOT_SUPPORTED_V01;
    default:
      return QMI_ERR_INVALID_ARG_V01;
  }

  if(*mmgsdi_slot_ptr > qmi_uim_global_ptr->card_state.num_slots)
  {
    qmi_err = QMI_ERR_NOT_SUPPORTED_V01;
  }

  return qmi_err;
} /* qmi_uim_map_and_validate_slot */


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
)
{
  ASSERT(decrypt_data_ptr && encrypt_pin_ptr);

  memset(decrypt_data_ptr, 0x00, sizeof(mmgsdi_static_data_type));

  if (encrypt_pin_ptr->enc_pin1_value_len == 0)
  {
    return QMI_ERR_ARG_TOO_LONG_V01;
  }

  decrypt_data_ptr->data_len = sizeof(decrypt_data_ptr->data_ptr);

  /* Decrypt the PIN and update the TLV */
  if (!qmi_uim_decrypt_pin(encrypt_pin_ptr->enc_pin1_value_len,
                           encrypt_pin_ptr->enc_pin1_value,
                           &decrypt_data_ptr->data_len,
                           decrypt_data_ptr->data_ptr))
  {
    return QMI_ERR_INCORRECT_PIN_V01;
  }

  /* Check decrypted pin length */
  if (decrypt_data_ptr->data_len == 0 ||
      decrypt_data_ptr->data_len > QMI_UIM_PIN_MAX_V01)
  {
    return QMI_ERR_INCORRECT_PIN_V01;
  }

  return QMI_ERR_NONE_V01;
} /* qmi_uim_tlv_decrypted_pin */


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
  qmi_uim_tlv_file_id_type  *file_id_ptr
)
{
  int32                          temp;

  if(!file_id_tlv || !file_id_ptr)
  {
    return QMI_ERR_INVALID_ARG_V01;
  }

  /* Set access by path */
  file_id_ptr->file_access.access_method = MMGSDI_BY_PATH_ACCESS;

  /* Verify path length value */
  if ((file_id_tlv->path_len == 0) || (file_id_tlv->path_len & 0x01) != 0) 
  {
    return QMI_ERR_INVALID_ARG_V01;
  }
  if ( (file_id_tlv->path_len > (MMGSDI_MAX_PATH_LEN - 1) * sizeof(uint16)) )
  {
    return QMI_ERR_ARG_TOO_LONG_V01;
  }

  /* Path length (in numbers of uint16) and value */
  file_id_ptr->file_access.file.path_type.path_len =
    ((mmgsdi_len_type)file_id_tlv->path_len) / sizeof(uint16);
  (void)memscpy((void*)file_id_ptr->file_access.file.path_type.path_buf,
                sizeof(file_id_ptr->file_access.file.path_type.path_buf),
                (void*)file_id_tlv->path,
                file_id_tlv->path_len );

  /* Add file id to the path */
  file_id_ptr->file_access.file.path_type.path_buf[file_id_ptr->file_access.file.path_type.path_len]
       = file_id_tlv->file_id;
  file_id_ptr->file_access.file.path_type.path_len += 1;

  return QMI_ERR_NONE_V01;
} /* qmi_uim_populate_file_id */


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
)
{
  uint16                    i                  = 0;
  uint16                    file_count         = 0;
  qmi_error_type_v01        ret_value          = QMI_ERR_NONE_V01;
  qmi_uim_files_list_type * last_file_node_ptr = NULL;
  qmi_uim_files_list_type * new_file_node_ptr  = NULL;
  
  /* Initialize the refresh list. */

  ASSERT(file_list_in_ptr && file_list_out_pptr);

  *file_list_out_pptr = NULL;

  /* The case with zero files in the refresh list is
     perfectly valid: we can immediately return */
  if (num_file == 0)
  {
    return QMI_ERR_NONE_V01;
  }

  for (i = 0; i < num_file; i++)
  {
    mmgsdi_access_type  file_access;
    
    memset(&file_access, 0, sizeof(mmgsdi_access_type));
    file_access.access_method = MMGSDI_BY_PATH_ACCESS;

    if (file_list_in_ptr[i].path_len > (MMGSDI_MAX_PATH_LEN - 1) * sizeof(uint16) )
    {
      ret_value = QMI_ERR_ARG_TOO_LONG_V01;
      break;
    }
    if ( file_list_in_ptr[i].path_len == 0 )
    {
      ret_value = QMI_ERR_INVALID_ARG_V01;
      break;
    }

    /* Path length (in numbers of uint16) and value */
    file_access.file.path_type.path_len = file_list_in_ptr[i].path_len / sizeof(uint16);
    (void)memscpy(file_access.file.path_type.path_buf, sizeof(file_access.file.path_type.path_buf),
                  file_list_in_ptr[i].path, file_list_in_ptr[i].path_len);

    /* Add file id to the path */
    file_access.file.path_type.path_buf[file_access.file.path_type.path_len] =
      file_list_in_ptr[i].file_id;
    file_access.file.path_type.path_len += 1;

    /* Allocate node in the linked list */
    new_file_node_ptr =
      (qmi_uim_files_list_type*)uimqmi_malloc(sizeof(qmi_uim_files_list_type));
    if (new_file_node_ptr == NULL)
    {
      continue;
    }

    /* Copy file details into the new note */
    new_file_node_ptr->next_ptr = NULL;
    (void)memscpy((void*)&new_file_node_ptr->file,
                  sizeof(mmgsdi_access_type),
                  (void*)&file_access,
                  sizeof(mmgsdi_access_type) );

    /* Append new node to the end of the list */
    if (last_file_node_ptr == NULL)
    {
      *file_list_out_pptr = new_file_node_ptr;
    }
    else
    {
      last_file_node_ptr->next_ptr = new_file_node_ptr;
    }
    last_file_node_ptr = new_file_node_ptr;

    file_count++;
  }

  UIM_MSG_HIGH_1("QMI_UIM files in the refresh list: 0x%x", file_count);

  return ret_value;
} /* qmi_uim_parse_register_refresh */


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
)
{
  ASSERT(mmgsdi_file_ctrl_info);

  /* map file control information requested */
  switch (uim_file_ctrl_info)
  {
    case UIM_FILE_CONTROL_INFORMATION_NO_DATA_V01:
      *mmgsdi_file_ctrl_info = MMGSDI_SELECT_RSP_NO_DATA;
      break;
    case UIM_FILE_CONTROL_INFORMATION_FCP_V01:
      *mmgsdi_file_ctrl_info = MMGSDI_SELECT_RSP_FCP;
      break;
    case UIM_FILE_CONTROL_INFORMATION_FCI_V01:
      *mmgsdi_file_ctrl_info = MMGSDI_SELECT_RSP_FCI;
      break;
    case UIM_FILE_CONTROL_INFORMATION_FCI_WITH_INTERFACES_V01:
      *mmgsdi_file_ctrl_info = MMGSDI_SELECT_RSP_FCI_WITH_INTERFACES;
      break;
    case UIM_FILE_CONTROL_INFORMATION_FMD_V01:
      *mmgsdi_file_ctrl_info = MMGSDI_SELECT_RSP_FMD;
      break;
    default:
      return QMI_ERR_INVALID_ARG_V01;
  }
  return QMI_ERR_NONE_V01;
}/* qmi_uim_map_file_control_info_to_mmgsdi */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_READ_ADDITIONAL_RECORDS_RESULT()

  DESCRIPTION
    Populate additional records result

  PARAMETERS
  uim_read_additional_record_result_type_v01 *add_read_result_ptr
  uint16                                      num_records
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
)
{
  uint16  i            = 0;
  uint16  expected_len = 0;
  boolean ret_value    = TRUE;
  uint16  tlv_len      = 0;
  uint8*  tlv_data_ptr = NULL;
  uint8*  curr_ptr     = NULL;

  ASSERT(add_read_result_ptr);

  /* At least 2 records should be present. We can still return TRUE
     as this is an optional TLV */
  if (num_records <= 1 ||
      records_ptr == NULL)
  {
    return FALSE;
  }

  /* All records should have the size of the first record. We can still
     return TRUE as this is an optional TLV */
  expected_len = (uint16)records_ptr[0].read_data.data_len;
  if (expected_len == 0)
  {
    return FALSE;
  }

  /* Assign memory */
  tlv_data_ptr = add_read_result_ptr->additional_record;
  tlv_len      = sizeof(add_read_result_ptr->additional_record);

  curr_ptr = tlv_data_ptr;

  /* Copy data (except first record) */
  for (i = 1; i < num_records; i++)
  {
    if (records_ptr[i].mmgsdi_status == MMGSDI_SUCCESS &&
        records_ptr[i].read_data.data_len == expected_len &&
        records_ptr[i].read_data.data_ptr != NULL)
    {
      add_read_result_ptr->additional_record_len += records_ptr[i].read_data.data_len;
      if(curr_ptr + expected_len > tlv_data_ptr + tlv_len)
      {
        ret_value = FALSE;
        break;
      }
      (void)memscpy(curr_ptr,
                    tlv_len - (curr_ptr - tlv_data_ptr),
                    records_ptr[i].read_data.data_ptr,
                    records_ptr[i].read_data.data_len);
      curr_ptr += records_ptr[i].read_data.data_len;
    }
    else
    {
      ret_value = FALSE;
      break;
    }
  }

  return ret_value;
} /* qmi_uim_response_read_additional_records_result */


/*===========================================================================
  FUNCTION QMI_UIM_CONVERT_SECURITY_ATTRIBUTES()

  DESCRIPTION
    security values to be used in the response
  for get file attributes

  PARAMETERS
    file_security       : MMGSDI structure with file security info
    security_method_ptr : output parameter with security method
    security_mask_ptr   : output parameter with security mask

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void qmi_uim_convert_security_attributes
(
  mmgsdi_file_security_type     file_security,
  uim_sec_condition_enum_v01  * security_method_ptr,
  uint16 *                      security_mask_ptr
)
{
  ASSERT(security_method_ptr && security_mask_ptr);

  /* Get security method */
  *security_method_ptr = (uim_sec_condition_enum_v01) file_security.protection_method;
  if(file_security.protection_method != MMGSDI_ALWAYS_ALLOWED &&
     file_security.protection_method != MMGSDI_NEVER_ALLOWED  &&
     file_security.protection_method != MMGSDI_AND_ALLOWED    &&
     file_security.protection_method != MMGSDI_AND_ALLOWED    &&
     file_security.protection_method != MMGSDI_SINGLE_ALLOWED)
  {
    *security_method_ptr = (uim_sec_condition_enum_v01)UIM_SEC_CONDITION_ALWAYS_V01;
  }

  /* Compose the security mask */
  *security_mask_ptr = 0;
  if (file_security.protection_method == MMGSDI_AND_ALLOWED ||
      file_security.protection_method == MMGSDI_OR_ALLOWED ||
      file_security.protection_method == MMGSDI_SINGLE_ALLOWED)
  {
    if (file_security.num_protection_pin != 0)
    {
      uint16 i = 0;
      for (i = 0; i < file_security.num_protection_pin; i++)
      {
        switch(file_security.protection_pin_ptr[i])
        {
          case MMGSDI_PIN1:
            *security_mask_ptr |= 0x1;
            break;

          case MMGSDI_PIN2:
            *security_mask_ptr |= 0x2;
            break;

          case MMGSDI_UNIVERSAL_PIN:
            *security_mask_ptr |= 0x4;
            break;

          default:
            /* Handle all ADM values in the same way */
            *security_mask_ptr |= 0x8;
            break;
        }
      }
    }
  }
} /* qmi_uim_convert_security_attributes */


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
)
{
  mmgsdi_file_security_access_type * security_access   = NULL;
        
  ASSERT(file_attr_out);
  memset(file_attr_out, 0x00, sizeof(uim_file_attributes_type_v01));

  file_attr_out->file_size = (uint16)attribute_data.file_size;
  file_attr_out->file_id   = (attribute_data.file_id[0] << 8) + attribute_data.file_id[1];
  switch (attribute_data.file_type)
  {
    case MMGSDI_LINEAR_FIXED_FILE:
      file_attr_out->file_type = UIM_FILE_TYPE_LINEAR_FIXED_V01;
      file_attr_out->rec_size  = (uint16)attribute_data.file_info.linear_fixed_file.rec_len;
      file_attr_out->rec_count = (uint16)attribute_data.file_info.linear_fixed_file.num_of_rec;
      security_access          = &attribute_data.file_info.linear_fixed_file.file_security;
      break;

    case MMGSDI_CYCLIC_FILE:
      file_attr_out->file_type = UIM_FILE_TYPE_CYCLIC_V01;
      file_attr_out->rec_size  = (uint16)attribute_data.file_info.cyclic_file.rec_len;
      file_attr_out->rec_count = (uint16)attribute_data.file_info.cyclic_file.num_of_rec;
      security_access          = &attribute_data.file_info.cyclic_file.file_security;
      break;

    case MMGSDI_TRANSPARENT_FILE:
      file_attr_out->file_type = UIM_FILE_TYPE_TRANSPARENT_V01;
      security_access          = &attribute_data.file_info.transparent_file.file_security;
      break;

    case MMGSDI_MASTER_FILE:
      file_attr_out->file_type = UIM_FILE_TYPE_MASTER_FILE_V01;
      break;

    case MMGSDI_DEDICATED_FILE:
      file_attr_out->file_type = UIM_FILE_TYPE_DEDICATED_FILE_V01;
      break;

    default:
      return FALSE;
  }

  if (security_access != NULL)
  {
    /* Read */
    qmi_uim_convert_security_attributes(security_access->read,
                                        &file_attr_out->sec_read,
                                        &file_attr_out->sec_read_mask);

    /* Write */
    qmi_uim_convert_security_attributes(security_access->write,
                                        &file_attr_out->sec_write,
                                        &file_attr_out->sec_write_mask);

    /* Increase */
    qmi_uim_convert_security_attributes(security_access->increase,
                                        &file_attr_out->sec_increase,
                                        &file_attr_out->sec_increase_mask);

    /* Deactivate */
    qmi_uim_convert_security_attributes(security_access->invalidate_deactivate,
                                         &file_attr_out->sec_deactivate,
                                         &file_attr_out->sec_deactivate_mask);

    /* Activate */
    qmi_uim_convert_security_attributes(security_access->rehabilitate_activate,
                                        &file_attr_out->sec_activate,
                                        &file_attr_out->sec_activate_mask);
  }

  (void)qmi_uim_util_populate_opaque(NULL, 
                                     file_attr_out->raw_value,
                                     &file_attr_out->raw_value_len,
                                     sizeof(file_attr_out->raw_value),
                                     raw_value.data_ptr,
                                     raw_value.data_len,
                                     FALSE);

  return TRUE;
} /* qmi_uim_response_get_file_attributes */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_RETRIES_LEFT()

  DESCRIPTION
    populate of PIN retries left

  PARAMETERS
    uint8               : *num_unblock_retries_ptr,
    uint8               : *num_retrie_ptr,
    num_retries         : number of PIN retries
    num_unblock_retries : number of PUK retries

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
)
{
  ASSERT(num_unblock_retries_ptr && num_retrie_ptr);

  if(num_retries >= QMI_UIM_MAX_RETRIES)
  {
    *num_retrie_ptr = QMI_UIM_MAX_RETRIES;
  }
  else
  {
    *num_retrie_ptr = (uint8)num_retries;
  }

  if(num_unblock_retries >= QMI_UIM_MAX_RETRIES)
  {
    *num_unblock_retries_ptr = QMI_UIM_MAX_RETRIES;
  }
  else
  {
    *num_unblock_retries_ptr = (uint8)num_unblock_retries;
  }

} /* qmi_uim_response_retries_left */


/*===========================================================================
  FUNCTION QMI_UIM_AID_MATCH()

  DESCRIPTION
    Matches the two AIDs and return result

  PARAMETERS
    app_aid_ptr         : Value of USIM/CSIM/ISIM AID
    app_aid_len         : Length of USIM/CSIM/ISIM AID
    aid_ptr             : Value of the request AID
    partial_aid_match_ok: Dictates whether caller is ok with going ahead
                          with checking for a partial AID match incase
                          complete AID did not match with USIM/CSIM/ISIM

  RETURN VALUE
    TRUE if matches. FALSE if not

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_aid_match
(
  const uint8*                    app_aid_ptr,
  uint8                           app_aid_len,
  const mmgsdi_static_data_type * aid_ptr,
  boolean                         partial_aid_match_ok
)
{
  boolean     match_found      = FALSE;

  ASSERT(aid_ptr != NULL);
  ASSERT(app_aid_ptr != NULL);

  if (aid_ptr->data_len < 0 ||
      aid_ptr->data_len > MMGSDI_MAX_AID_LEN)
  {
    return FALSE;
  }

  if (aid_ptr->data_len >= app_aid_len)
  {
    if (memcmp(aid_ptr->data_ptr, app_aid_ptr, app_aid_len) == 0)
    {
      /* AID matched completely with a USIM AID (first 7 bytes)*/
      match_found = TRUE;
    }
    return match_found;
  }

  /* Input AID seems to be a partial AID (shorter than 7 bytes). If the
     caller is ok with checking for a partial-AID match, lets see if the
     input AID matches partially with the USIM/CSIM/ISIM aid and return
     TRUE in that case. */
  if (partial_aid_match_ok)
  {
    /* If zero-length AID is passed in as input, lets mark it as a (partial)
       match */
    if (aid_ptr->data_len == 0 ||
        memcmp(aid_ptr->data_ptr, app_aid_ptr, aid_ptr->data_len) == 0)
    {
      match_found = TRUE;
    }
  }
  return match_found;
} /* qmi_uim_aid_match */


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
)
{
  const uint8 isim_aid_value[] = { 0xA0, 0x00, 0x00, 0x00, 0x87,
                                   0x10, 0x04 };

  return qmi_uim_aid_match(isim_aid_value,
                           sizeof(isim_aid_value),
                           aid_ptr,
                           FALSE);
} /* qmi_uim_is_isim_aid */


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
  const   mmgsdi_static_data_type * aid_ptr,
  boolean                           partial_aid_match_ok
)
{
  const uint8 usim_aid_value[] = { 0xA0, 0x00, 0x00, 0x00, 0x87,
                                   0x10, 0x02 };

  return qmi_uim_aid_match(usim_aid_value,
                           sizeof(usim_aid_value),
                           aid_ptr,
                           partial_aid_match_ok);
} /* qmi_uim_is_usim_aid */


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
)
{
  const uint8 csim_aid_value[] = { 0xA0, 0x00, 0x00, 0x03, 0x43,
                                   0x10, 0x02 };

  return qmi_uim_aid_match(csim_aid_value,
                           sizeof(csim_aid_value),
                           aid_ptr,
                           partial_aid_match_ok);
} /* qmi_uim_is_csim_aid */


/*===========================================================================
  FUNCTION QMI_UIM_POPULATE_CARD_AND_APP_STATE()

  DESCRIPTION
    Composes the card and app state info for card status indication or
    get card status request

  PARAMETERS
    card_state_ptr          : card status
    app_count_per_slot_len  : length of apps present per slot array
    app_count_per_slot_ptr  : apps present per slot
    extended_card_status    : if the TLV is for extended card status
    validit_len             : length of card status validity array
    validity_ptr            : card status validity array
    num_of_card_ptr         : output number of cards 
    card_info_ptr           : output card info

  RETURN VALUE
    boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_populate_card_and_app_state
(
  const qmi_uim_card_state * card_state_ptr,
  uint8                      app_count_per_slot_len,
  uint8                    * app_count_per_slot_ptr,
  boolean                    extended_card_status,
  uint8                      validity_len,
  const boolean            * validity_ptr,
  card_info_type_v01       * card_info_ptr,
  uint32                   * num_of_card_ptr
)
{
  uint8                           i                          = 0;
  uint8                           j                          = 0;
  uint8                           slot_index                 = 0;
  uint8                           max_card_count             = 0;
  uint8                           max_app_count              = 0;
  uint32                          app_count                  = 0;

  ASSERT(card_state_ptr && app_count_per_slot_ptr && card_info_ptr && num_of_card_ptr);

  max_card_count = (card_state_ptr->num_slots < QMI_UIM_LEGACY_MAX_CARD_COUNT) ?
                    card_state_ptr->num_slots : QMI_UIM_LEGACY_MAX_CARD_COUNT;
  max_app_count = QMI_UIM_LEGACY_MAX_APPS_PER_CARD;

  if(app_count_per_slot_len < max_card_count)
  {
    return FALSE;
  }

  * num_of_card_ptr = 0;

  /* Loop thru all the cards */
  for (i = 0; i < max_card_count; i++)
  {
    if (card_state_ptr->card[i] == NULL)
    {
      return FALSE;
    }

    /* Card state */
    if (extended_card_status && (validity_ptr != NULL) && (i < validity_len))
    {
      if (validity_ptr[i])
      {
        card_info_ptr[i].card_state = (uint8)card_state_ptr->card[i]->card_state;
      }
      else
      {
        card_info_ptr[i].card_state = UIM_CARD_STATE_ENUM_MAX_ENUM_VAL_V01;
      }
    }
    else
    {
      card_info_ptr[i].card_state = (uint8)card_state_ptr->card[i]->card_state;
    }

    if(card_state_ptr->card[i]->upin_state >= MMGSDI_PIN_ENABLED_NOT_VERIFIED &&
       card_state_ptr->card[i]->upin_state <= MMGSDI_PIN_PERM_BLOCKED)
    {
      card_info_ptr[i].upin.pin_state = (uim_pin_state_enum_v01) card_state_ptr->card[i]->upin_state;
    }
    else
    {
      card_info_ptr[i].upin.pin_state = UIM_PIN_STATE_UNKNOWN_V01;
    }
    card_info_ptr[i].upin.pin_retries = card_state_ptr->card[i]->upin_num_retries;
    card_info_ptr[i].upin.puk_retries = card_state_ptr->card[i]->upuk_num_retries;
    card_info_ptr[i].error_code       = (uint8)card_state_ptr->card[i]->card_error;
    card_info_ptr[i].app_info_len     = app_count_per_slot_ptr[i];

    /* Loop thru all the applications */
    app_count = 0;
    for (j = 0; j < card_state_ptr->num_apps && j < QMI_UIM_MAX_APP_COUNT; j++)
    {
      if (app_count >= max_app_count)
      {
        break;
      }

      if (card_state_ptr->application[j] == NULL)
      {
        return FALSE;
      }

      if (extended_card_status == FALSE &&
          (card_state_ptr->application[j]->slot != MMGSDI_SLOT_1 &&
           card_state_ptr->application[j]->slot != MMGSDI_SLOT_2))
      {
        continue;
      }

      /* Pick only applications for this card */
      if (!qmi_uim_convert_slot_to_index(card_state_ptr->application[j]->slot,
                                         &slot_index))
      {
        continue;
      }

      if (slot_index != i)
      {
        continue;
      }

      /* App type */
      switch(card_state_ptr->application[j]->app_type)
      {
        case MMGSDI_APP_SIM:
          card_info_ptr[i].app_info[app_count].app_type = UIM_APP_TYPE_SIM_V01;
          break;
        case MMGSDI_APP_RUIM:
          card_info_ptr[i].app_info[app_count].app_type = UIM_APP_TYPE_RUIM_V01;
          break;
        case MMGSDI_APP_USIM:
          card_info_ptr[i].app_info[app_count].app_type = UIM_APP_TYPE_USIM_V01;
          break;
        case MMGSDI_APP_CSIM:
          card_info_ptr[i].app_info[app_count].app_type = UIM_APP_TYPE_CSIM_V01;
          break;
        case MMGSDI_APP_UNKNOWN:
          if (qmi_uim_is_isim_aid(&card_state_ptr->application[j]->aid))
          {
            card_info_ptr[i].app_info[app_count].app_type = UIM_APP_TYPE_ISIM_V01;
          }
          else
          {
            card_info_ptr[i].app_info[app_count].app_type = UIM_APP_TYPE_UNKNOWN_V01;
          }
          break;
        default:
          card_info_ptr[i].app_info[app_count].app_type =  UIM_APP_TYPE_UNKNOWN_V01;
          break;
      }

      /* App state */
      card_info_ptr[i].app_info[app_count].app_state =
        (uint8)card_state_ptr->application[j]->app_state;

      /* Perso state */
      card_info_ptr[i].app_info[app_count].perso_state =
        (uint8)card_state_ptr->application[j]->perso_state;

      /* Perso feature */
      switch(card_state_ptr->application[j]->perso_feature)
      {
        case MMGSDI_PERSO_NW:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_GW_NETWORK_V01;
          break;

        case MMGSDI_PERSO_NS:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_GW_NETWORK_SUBSET_V01;
          break;

        case MMGSDI_PERSO_SP:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_GW_SERVICE_PROVIDER_V01;
          break;

        case MMGSDI_PERSO_CP:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_GW_CORPORATE_V01;
          break;

        case MMGSDI_PERSO_SIM:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_GW_UIM_V01;
          break;

        case MMGSDI_PERSO_RUIM_NW1:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_1X_NETWORK_TYPE_1_V01;
          break;

        case MMGSDI_PERSO_RUIM_NW2:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_1X_NETWORK_TYPE_2_V01;
          break;

        case MMGSDI_PERSO_RUIM_HRPD:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_1X_HRPD_V01;
          break;

        case MMGSDI_PERSO_RUIM_SP:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_1X_SERVICE_PROVIDER_V01;
          break;

        case MMGSDI_PERSO_RUIM_CP:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_1X_CORPORATE_V01;
          break;

        case MMGSDI_PERSO_RUIM_RUIM:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_1X_RUIM_V01;
          break;

        default:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_UNKNOWN_V01;
          break;
      }

      /* Perso retries */
      if(card_state_ptr->application[j]->perso_retries >= QMI_UIM_MAX_RETRIES)
      {
        card_info_ptr[i].app_info[app_count].perso_retries = QMI_UIM_MAX_RETRIES;
      }
      else
      {
        card_info_ptr[i].app_info[app_count].perso_retries =
           (uint8)card_state_ptr->application[j]->perso_retries;
      }

      /* Perso unblock retries is no longer supported with current SimLock engine.
         So setting always to 0 to indicate that unblock functionality cannot
         be used */
      card_info_ptr[i].app_info[app_count].perso_unblock_retries = 0;

      /* AID length */
          card_info_ptr[i].app_info[app_count].aid_value_len =
        (uint8)card_state_ptr->application[j]->aid.data_len;

      /* UPIN */
      card_info_ptr[i].app_info[app_count].univ_pin =
        (card_state_ptr->application[j]->upin ?
           UIM_UNIV_PIN_UPIN_REPLACES_PIN1_V01 : UIM_UNIV_PIN_PIN1_USED_V01);

      /* PIN 1 state */
      if(card_state_ptr->application[j]->pin1_state >= MMGSDI_PIN_ENABLED_NOT_VERIFIED &&
         card_state_ptr->application[j]->pin1_state <= MMGSDI_PIN_PERM_BLOCKED)
      {
        card_info_ptr[i].app_info[app_count].pin1.pin_state = (uim_pin_state_enum_v01)card_state_ptr->application[j]->pin1_state;
      }
      else if(card_state_ptr->application[j]->pin1_state == MMGSDI_PIN_STATUS_NOT_INITIALIZED &&
              (card_state_ptr->application[j]->app_state == QMI_UIM_APP_STATE_READY ||
               card_state_ptr->application[j]->app_state == QMI_UIM_APP_STATE_PERSO ||
               card_state_ptr->application[j]->app_state == QMI_UIM_APP_STATE_ILLEGAL))
      {
        card_info_ptr[i].app_info[app_count].pin1.pin_state = UIM_PIN_STATE_DISABLED_V01;
      }
      else
      {
        card_info_ptr[i].app_info[app_count].pin1.pin_state = UIM_PIN_STATE_UNKNOWN_V01;
      }

      /* PIN1 retries */
      card_info_ptr[i].app_info[app_count].pin1.pin_retries =
        card_state_ptr->application[j]->pin1_num_retries;
      card_info_ptr[i].app_info[app_count].pin1.puk_retries =
        card_state_ptr->application[j]->puk1_num_retries;

      /* PIN 2 state */
      if(card_state_ptr->application[j]->pin2_state >= MMGSDI_PIN_ENABLED_NOT_VERIFIED &&
         card_state_ptr->application[j]->pin2_state <= MMGSDI_PIN_PERM_BLOCKED)
      {
        card_info_ptr[i].app_info[app_count].pin2.pin_state = (uim_pin_state_enum_v01) card_state_ptr->application[j]->pin2_state;
      }
      else
      {
        card_info_ptr[i].app_info[app_count].pin2.pin_state = UIM_PIN_STATE_UNKNOWN_V01;
      }

      /* PIN2 retries */
      card_info_ptr[i].app_info[app_count].pin2.pin_retries =
        card_state_ptr->application[j]->pin2_num_retries;
      card_info_ptr[i].app_info[app_count].pin2.puk_retries = 
        card_state_ptr->application[j]->puk2_num_retries;

      /* AID value */
      if (card_state_ptr->application[j]->aid.data_len > 0)
      {
        (void)memscpy(card_info_ptr[i].app_info[app_count].aid_value,
                      sizeof(card_info_ptr[i].app_info[app_count].aid_value),
                      (void*)card_state_ptr->application[j]->aid.data_ptr,
                      (uint32)card_state_ptr->application[j]->aid.data_len);
      }
      app_count++;
    } /* for (j = 0; j < card_state_ptr->num_apps && j < QMI_UIM_MAX_APP_COUNT; j++) */

    /* populating total number of app in one card */
    card_info_ptr[i].app_info_len = app_count; 
    /* counting number of card*/
    (*num_of_card_ptr)++;
  } /* for (i = 0; i < num_slots; i++) */

  return TRUE;
} /* qmi_uim_populate_card_and_app_state */


/*===========================================================================
  FUNCTION QMI_UIM_POPULATE_CARD_AND_APP_STATE()

  DESCRIPTION
    Composes the card and app state info for card status indication or
    get card status request

  PARAMETERS
    card_state_ptr          : card status
    app_count_per_slot_len  : length of apps present per slot array
    app_count_per_slot_ptr  : apps present per slot
    extended_card_status    : if the TLV is for extended card status
    validit_len             : length of card status validity array
    validity_ptr            : card status validity array
    num_of_card_ptr         : output number of cards 
    card_info_ptr           : output card info

  RETURN VALUE
    boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean qmi_uim_populate_card_and_app_state_extended
(
  const qmi_uim_card_state * card_state_ptr,
  uint8                      app_count_per_slot_len,
  uint8                    * app_count_per_slot_ptr,
  boolean                    extended_card_status,
  uint8                      validity_len,
  const boolean            * validity_ptr,
  extended_card_info_type_v01       * card_info_ptr,
  uint32                            * num_of_card_ptr
)
{
  uint8                           i                          = 0;
  uint8                           j                          = 0;
  uint8                           slot_index                 = 0;
  uint8                           max_card_count             = 0;
  uint8                           max_app_count              = 0;
  uint32                          app_count                  = 0;

  ASSERT(card_state_ptr && app_count_per_slot_ptr && card_info_ptr && num_of_card_ptr);

  max_card_count = (card_state_ptr->num_slots < QMI_UIM_MAX_CARD_COUNT) ?
                     card_state_ptr->num_slots : QMI_UIM_MAX_CARD_COUNT;
  max_app_count = QMI_UIM_MAX_APPS_PER_CARD;

  if(app_count_per_slot_len < max_card_count)
  {
    return FALSE;
  }

  * num_of_card_ptr = 0;

  /* Loop thru all the cards */
  for (i = 0; i < max_card_count; i++)
  {
    if (card_state_ptr->card[i] == NULL)
    {
      return FALSE;
    }

    /* Card state */
    if (extended_card_status && (validity_ptr != NULL) && (i < validity_len))
    {
      if (validity_ptr[i])
      {
        card_info_ptr[i].card_state = (uint8)card_state_ptr->card[i]->card_state;
      }
      else
      {
        card_info_ptr[i].card_state = UIM_EXTENDED_CARD_STATE_UNKNOWN_V01;
      }
    }
    else
    {
      card_info_ptr[i].card_state = (uint8)card_state_ptr->card[i]->card_state;
    }

    if(card_state_ptr->card[i]->upin_state >= MMGSDI_PIN_ENABLED_NOT_VERIFIED &&
       card_state_ptr->card[i]->upin_state <= MMGSDI_PIN_PERM_BLOCKED)
    {
      card_info_ptr[i].upin.pin_state = (uim_pin_state_enum_v01) card_state_ptr->card[i]->upin_state;
    }
    else
    {
      card_info_ptr[i].upin.pin_state = UIM_PIN_STATE_UNKNOWN_V01;
    }
    card_info_ptr[i].upin.pin_retries = card_state_ptr->card[i]->upin_num_retries;
    card_info_ptr[i].upin.puk_retries = card_state_ptr->card[i]->upuk_num_retries;
    card_info_ptr[i].error_code       = (uint8)card_state_ptr->card[i]->card_error;
    card_info_ptr[i].app_info_len     = app_count_per_slot_ptr[i];

    /* Loop thru all the applications */
    app_count = 0;
    for (j = 0; j < card_state_ptr->num_apps && j < QMI_UIM_MAX_APP_COUNT; j++)
    {
      if (app_count >= max_app_count)
      {
        break;
      }

      if (card_state_ptr->application[j] == NULL)
      {
        return FALSE;
      }

      if (extended_card_status == FALSE &&
          (card_state_ptr->application[j]->slot != MMGSDI_SLOT_1 &&
           card_state_ptr->application[j]->slot != MMGSDI_SLOT_2))
      {
        continue;
      }

      /* Pick only applications for this card */
      if (!qmi_uim_convert_slot_to_index(card_state_ptr->application[j]->slot,
                                         &slot_index))
      {
        continue;
      }

      if (slot_index != i)
      {
        continue;
      }

      /* App type */
      switch(card_state_ptr->application[j]->app_type)
      {
        case MMGSDI_APP_SIM:
          card_info_ptr[i].app_info[app_count].app_type = UIM_APP_TYPE_SIM_V01;
          break;
        case MMGSDI_APP_RUIM:
          card_info_ptr[i].app_info[app_count].app_type = UIM_APP_TYPE_RUIM_V01;
          break;
        case MMGSDI_APP_USIM:
          card_info_ptr[i].app_info[app_count].app_type = UIM_APP_TYPE_USIM_V01;
          break;
        case MMGSDI_APP_CSIM:
          card_info_ptr[i].app_info[app_count].app_type = UIM_APP_TYPE_CSIM_V01;
          break;
        case MMGSDI_APP_UNKNOWN:
          if (qmi_uim_is_isim_aid(&card_state_ptr->application[j]->aid))
          {
            card_info_ptr[i].app_info[app_count].app_type = UIM_APP_TYPE_ISIM_V01;
          }
          else
          {
            card_info_ptr[i].app_info[app_count].app_type = UIM_APP_TYPE_UNKNOWN_V01;
          }
          break;
        default:
          card_info_ptr[i].app_info[app_count].app_type =  UIM_APP_TYPE_UNKNOWN_V01;
          break;
      }

      /* App state */
      card_info_ptr[i].app_info[app_count].app_state =
        (uint8)card_state_ptr->application[j]->app_state;

      /* Perso state */
      card_info_ptr[i].app_info[app_count].perso_state =
        (uint8)card_state_ptr->application[j]->perso_state;

      /* Perso feature */
      switch(card_state_ptr->application[j]->perso_feature)
      {
        case MMGSDI_PERSO_NW:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_GW_NETWORK_V01;
          break;

        case MMGSDI_PERSO_NS:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_GW_NETWORK_SUBSET_V01;
          break;

        case MMGSDI_PERSO_SP:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_GW_SERVICE_PROVIDER_V01;
          break;

        case MMGSDI_PERSO_CP:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_GW_CORPORATE_V01;
          break;

        case MMGSDI_PERSO_SIM:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_GW_UIM_V01;
          break;

        case MMGSDI_PERSO_RUIM_NW1:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_1X_NETWORK_TYPE_1_V01;
          break;

        case MMGSDI_PERSO_RUIM_NW2:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_1X_NETWORK_TYPE_2_V01;
          break;

        case MMGSDI_PERSO_RUIM_HRPD:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_1X_HRPD_V01;
          break;

        case MMGSDI_PERSO_RUIM_SP:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_1X_SERVICE_PROVIDER_V01;
          break;

        case MMGSDI_PERSO_RUIM_CP:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_1X_CORPORATE_V01;
          break;

        case MMGSDI_PERSO_RUIM_RUIM:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_1X_RUIM_V01;
          break;

        default:
          card_info_ptr[i].app_info[app_count].perso_feature = UIM_PERSO_FEATURE_STATUS_UNKNOWN_V01;
          break;
      }

      /* Perso retries */
      if(card_state_ptr->application[j]->perso_retries >= QMI_UIM_MAX_RETRIES)
      {
        card_info_ptr[i].app_info[app_count].perso_retries = QMI_UIM_MAX_RETRIES;
      }
      else
      {
        card_info_ptr[i].app_info[app_count].perso_retries =
           (uint8)card_state_ptr->application[j]->perso_retries;
      }

      /* Perso unblock retries is no longer supported with current SimLock engine.
         So setting always to 0 to indicate that unblock functionality cannot
         be used */
      card_info_ptr[i].app_info[app_count].perso_unblock_retries = 0;

      /* AID length */
          card_info_ptr[i].app_info[app_count].aid_value_len =
        (uint8)card_state_ptr->application[j]->aid.data_len;

      /* UPIN */
      card_info_ptr[i].app_info[app_count].univ_pin =
        (card_state_ptr->application[j]->upin ?
           UIM_UNIV_PIN_UPIN_REPLACES_PIN1_V01 : UIM_UNIV_PIN_PIN1_USED_V01);

      /* PIN 1 state */
      if(card_state_ptr->application[j]->pin1_state >= MMGSDI_PIN_ENABLED_NOT_VERIFIED &&
         card_state_ptr->application[j]->pin1_state <= MMGSDI_PIN_PERM_BLOCKED)
      {
        card_info_ptr[i].app_info[app_count].pin1.pin_state = (uim_pin_state_enum_v01)card_state_ptr->application[j]->pin1_state;
      }
      else if(card_state_ptr->application[j]->pin1_state == MMGSDI_PIN_STATUS_NOT_INITIALIZED &&
              (card_state_ptr->application[j]->app_state == QMI_UIM_APP_STATE_READY ||
               card_state_ptr->application[j]->app_state == QMI_UIM_APP_STATE_PERSO ||
               card_state_ptr->application[j]->app_state == QMI_UIM_APP_STATE_ILLEGAL))
      {
        card_info_ptr[i].app_info[app_count].pin1.pin_state = UIM_PIN_STATE_DISABLED_V01;
      }
      else
      {
        card_info_ptr[i].app_info[app_count].pin1.pin_state = UIM_PIN_STATE_UNKNOWN_V01;
      }

      /* PIN1 retries */
      card_info_ptr[i].app_info[app_count].pin1.pin_retries =
        card_state_ptr->application[j]->pin1_num_retries;
      card_info_ptr[i].app_info[app_count].pin1.puk_retries =
        card_state_ptr->application[j]->puk1_num_retries;

      /* PIN 2 state */
      if(card_state_ptr->application[j]->pin2_state >= MMGSDI_PIN_ENABLED_NOT_VERIFIED &&
         card_state_ptr->application[j]->pin2_state <= MMGSDI_PIN_PERM_BLOCKED)
      {
        card_info_ptr[i].app_info[app_count].pin2.pin_state = (uim_pin_state_enum_v01) card_state_ptr->application[j]->pin2_state;
      }
      else
      {
        card_info_ptr[i].app_info[app_count].pin2.pin_state = UIM_PIN_STATE_UNKNOWN_V01;
      }

      /* PIN2 retries */
      card_info_ptr[i].app_info[app_count].pin2.pin_retries =
        card_state_ptr->application[j]->pin2_num_retries;
      card_info_ptr[i].app_info[app_count].pin2.puk_retries = 
        card_state_ptr->application[j]->puk2_num_retries;

      /* AID value */
      if (card_state_ptr->application[j]->aid.data_len > 0)
      {
        (void)memscpy(card_info_ptr[i].app_info[app_count].aid_value,
                      sizeof(card_info_ptr[i].app_info[app_count].aid_value),
                      (void*)card_state_ptr->application[j]->aid.data_ptr,
                      (uint32)card_state_ptr->application[j]->aid.data_len);
      }
      app_count++;
    } /* for (j = 0; j < card_state_ptr->num_apps && j < QMI_UIM_MAX_APP_COUNT; j++) */

    /* populating total number of app in one card */
    card_info_ptr[i].app_info_len = app_count; 
    /* counting number of card*/
    (*num_of_card_ptr)++;
  } /* for (i = 0; i < num_slots; i++) */

  return TRUE;
} /* qmi_uim_populate_card_and_app_state */


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
)
{
  uint8   i            = 0;
  uint8   slot_index   = 0;
  uint8   num_slots    = 0;
  uint8   app_count_per_slot[QMI_UIM_LEGACY_MAX_CARD_COUNT];

  ASSERT(card_status_out_ptr && card_state_ptr &&
         card_state_ptr->num_apps <= QMI_UIM_MAX_APP_COUNT &&
         card_state_ptr->num_slots > 0);

  memset(card_status_out_ptr, 0x00, sizeof(uim_card_status_type_v01));
  memset(app_count_per_slot, 0, sizeof(app_count_per_slot));

  num_slots = (QMI_UIM_LEGACY_MAX_CARD_COUNT < card_state_ptr->num_slots) ?
                QMI_UIM_LEGACY_MAX_CARD_COUNT : card_state_ptr->num_slots;

  /* Intro: set default values for indexes and number of slots */
  card_status_out_ptr->index_gw_pri = 0xFFFF;
  card_status_out_ptr->index_1x_pri = 0xFFFF;
  card_status_out_ptr->index_gw_sec = 0xFFFF;
  card_status_out_ptr->index_1x_sec = 0xFFFF;

  /* Count the number of applications for each slot and
     populate the indexes to be used in the TLV */
  for (i = 0; i < card_state_ptr->num_apps && i < QMI_UIM_MAX_APP_COUNT; i++)
  {
    if (card_state_ptr->application[i] == NULL)
    {
      continue;
    }

    if (card_state_ptr->application[i]->slot != MMGSDI_SLOT_1 &&
        card_state_ptr->application[i]->slot != MMGSDI_SLOT_2)
    {
      continue;
    }

    if (!qmi_uim_convert_slot_to_index(card_state_ptr->application[i]->slot,
                                         &slot_index))
    {
      continue;
    }

    if (app_count_per_slot[slot_index] >= QMI_UIM_LEGACY_MAX_APPS_PER_CARD)
    {
      continue;
    }

    if (i == card_state_ptr->index_gw[0])
    {
      card_status_out_ptr->index_gw_pri = (uint16)((uint16)slot_index << 8) +
                                          (uint16)app_count_per_slot[slot_index];
    }
    if (i == card_state_ptr->index_1x[0])
    {
      card_status_out_ptr->index_1x_pri = (uint16)((uint16)slot_index << 8) +
                                          (uint16)app_count_per_slot[slot_index];
    }
    if (i == card_state_ptr->index_gw[1])
    {
      card_status_out_ptr->index_gw_sec = (uint16)((uint16)slot_index << 8) +
                                          (uint16)app_count_per_slot[slot_index];
    }
    if (i == card_state_ptr->index_1x[1])
    {
      card_status_out_ptr->index_1x_sec = (uint16)((uint16)slot_index << 8) +
                                          (uint16)app_count_per_slot[slot_index];
    }
    app_count_per_slot[slot_index]++;

  }

  return qmi_uim_populate_card_and_app_state(card_state_ptr,
                                             QMI_UIM_LEGACY_MAX_CARD_COUNT,
                                             app_count_per_slot,
                                             FALSE,
                                             0,
                                             NULL,
                                             card_status_out_ptr->card_info,
                                             &card_status_out_ptr->card_info_len);
} /* qmi_uim_response_card_status */


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
)
{
  uint8   i                     = 0;
  uint8   j                     = 0;
  uint8*  curr_ptr              = NULL;
  uint8   slot_index            = 0;
  uint8   num_slots             = 0;
  uint8   num_prov_sessions     = 0;
  uint8   app_count_per_slot[QMI_UIM_MAX_CARD_COUNT];

  ASSERT(extnd_card_status_ptr && card_state_ptr &&
         card_state_ptr->num_apps <= QMI_UIM_MAX_APP_COUNT &&
         card_state_ptr->num_slots > 0);

  memset(app_count_per_slot, 0, sizeof(app_count_per_slot));
  num_slots = (QMI_UIM_MAX_CARD_COUNT < card_state_ptr->num_slots) ?
                QMI_UIM_MAX_CARD_COUNT : card_state_ptr->num_slots;
  num_prov_sessions = (QMI_UIM_MAX_PROV_SESSIONS < card_state_ptr->num_slots) ?
                         QMI_UIM_MAX_PROV_SESSIONS : card_state_ptr->num_slots;

  /* Intro: set default values for indexes and number of slots */
  extnd_card_status_ptr->index_gw_len = num_prov_sessions;
  memset(extnd_card_status_ptr->index_gw, 0xFF, sizeof(extnd_card_status_ptr->index_gw));
  extnd_card_status_ptr->index_1x_len = num_prov_sessions;
  memset(extnd_card_status_ptr->index_1x, 0xFF, sizeof(extnd_card_status_ptr->index_1x));
  extnd_card_status_ptr->card_info_len = num_slots;

  /* Count the number of applications for each slot and
     populate the indexes to be used in the TLV */
  for (i = 0; i < card_state_ptr->num_apps && i < QMI_UIM_MAX_APP_COUNT; i++)
  {
    if (card_state_ptr->application[i] == NULL)
    {
      continue;
    }

    if (!qmi_uim_convert_slot_to_index(card_state_ptr->application[i]->slot,
                                       &slot_index))
    {
      continue;
    }

    for (j = 0; j < QMI_UIM_MAX_PROV_SESSIONS && j < card_state_ptr->num_slots; j++)
    {
      if (i == card_state_ptr->index_gw[j])
      {
        extnd_card_status_ptr->index_gw[j] = (uint16)((uint16)slot_index << 8) +
                                         (uint16)app_count_per_slot[slot_index];
      }
      if (i == card_state_ptr->index_1x[j])
      {
        extnd_card_status_ptr->index_1x[j] = (uint16)((uint16)slot_index << 8) +
                                         (uint16)app_count_per_slot[slot_index];
      }
    }
    app_count_per_slot[slot_index]++;
  }

  return qmi_uim_populate_card_and_app_state_extended(card_state_ptr,
                                             QMI_UIM_MAX_CARD_COUNT,
                                             app_count_per_slot,
                                             TRUE,
                                             validity_len,
                                             validity_ptr,
                                             extnd_card_status_ptr->card_info,
                                             &extnd_card_status_ptr->card_info_len);
} /* qmi_uim_response_extended_card_status */


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
)
{
  ASSERT(is_valid && mask);

  *mask = 0x00;
  *is_valid = TRUE;

  if (reg_event_type.card_status)
  {
    *mask |= 0x01;
  }
  if (reg_event_type.sap_events)
  {
    *mask |= 0x02;
  }
  if (reg_event_type.extended_card_status)
  {
    *mask |= 0x04;
  }
  if(reg_event_type.prov_session_close_event)
  {
    *mask |= 0x08;
  }
  if (reg_event_type.physical_slots_status)
  {
    *mask |= 0x10;
  }
  if(reg_event_type.sim_busy_events)
  {
    *mask |= 0x20;
  }
  if(reg_event_type.reduced_card_status_events)
  {
    *mask |= 0x40;
  }
  if(reg_event_type.recovery_complete_event)
  {
    *mask |= 0x80;
  }
  if(reg_event_type.supply_voltage_events)
  {
    *mask |= 0x100;
  }
  if(reg_event_type.reg_for_activation_status_event)
  {
    *mask |= 0x200;
  }
  if(reg_event_type.simlock_remote_sfs_events)
  {
    *mask |= 0x400;
  }
  if(reg_event_type.simlock_temp_unlock_status)
  {
    *mask |= 0x800;
  }
} /* qmi_uim_response_event_reg */


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
)
{
  uint16   i               = 0;
  uint16   file_count      = 0;

  ASSERT(ref_event_ptr && aid_ptr && refresh_evt_ptr);

  /* Stage */
  switch(refresh_evt_ptr->stage)
  {
    case MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT:
    case MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN:
       ref_event_ptr->stage = UIM_REFRESH_STAGE_WAIT_FOR_OK_V01;
      break;

    case MMGSDI_REFRESH_STAGE_START:
      ref_event_ptr->stage = UIM_REFRESH_STAGE_START_V01;
      break;

    case MMGSDI_REFRESH_STAGE_END_SUCCESS:
      ref_event_ptr->stage = UIM_REFRESH_STAGE_END_WITH_SUCCESS_V01;
      break;

    case MMGSDI_REFRESH_STAGE_END_FAILED:
      ref_event_ptr->stage = UIM_REFRESH_STAGE_END_WITH_FAILURE_V01;
      break;

    default:
      /* Some stages are not sent to QMI clients */
      return FALSE;
  }

  /* Mode */
  switch(refresh_evt_ptr->mode)
  {
    case MMGSDI_REFRESH_RESET:
    case MMGSDI_REFRESH_RESET_AUTO:
      ref_event_ptr->mode = UIM_REFRESH_MODE_RESET_V01;
      break;

    case MMGSDI_REFRESH_NAA_INIT:
      ref_event_ptr->mode = UIM_REFRESH_MODE_INIT_V01;
      break;

    case MMGSDI_REFRESH_NAA_INIT_FCN:
      ref_event_ptr->mode = UIM_REFRESH_MODE_INIT_FCN_V01;
      break;

    case MMGSDI_REFRESH_NAA_FCN:
      ref_event_ptr->mode = UIM_REFRESH_MODE_FCN_V01;
      break;

    case MMGSDI_REFRESH_NAA_INIT_FULL_FCN:
      ref_event_ptr->mode = UIM_REFRESH_MODE_INIT_FULL_FCN_V01;
      break;

    case MMGSDI_REFRESH_NAA_APP_RESET:
      ref_event_ptr->mode = UIM_REFRESH_MODE_APP_RESET_V01;
      break;

    case MMGSDI_REFRESH_3G_SESSION_RESET:
      ref_event_ptr->mode = UIM_REFRESH_MODE_3G_RESET_V01;
      break;

    default:
      /* Some modes are not sent to QMI clients */
      return FALSE;
  }

  /* Session type */
  switch(session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      ref_event_ptr->session_type = UIM_SESSION_TYPE_PRIMARY_GW_V01;
      break;

    case MMGSDI_1X_PROV_PRI_SESSION:
      ref_event_ptr->session_type = UIM_SESSION_TYPE_PRIMARY_1X_V01;
      break;

    case MMGSDI_GW_PROV_SEC_SESSION:
      ref_event_ptr->session_type = UIM_SESSION_TYPE_SECONDARY_GW_V01;
      break;

    case MMGSDI_1X_PROV_SEC_SESSION:
      ref_event_ptr->session_type = UIM_SESSION_TYPE_SECONDARY_1X_V01;
      break;

    case MMGSDI_NON_PROV_SESSION_SLOT_1:
      ref_event_ptr->session_type = UIM_SESSION_TYPE_NONPROVISIONING_SLOT_1_V01;
      break;

    case MMGSDI_NON_PROV_SESSION_SLOT_2:
      ref_event_ptr->session_type = UIM_SESSION_TYPE_NONPROVISIONING_SLOT_2_V01;
      break;

    case MMGSDI_CARD_SESSION_SLOT_1:
      ref_event_ptr->session_type = UIM_SESSION_TYPE_CARD_ON_SLOT_1_V01;
      break;

    case MMGSDI_CARD_SESSION_SLOT_2:
      ref_event_ptr->session_type = UIM_SESSION_TYPE_CARD_ON_SLOT_2_V01;
      break;

    case MMGSDI_GW_PROV_TER_SESSION:
      ref_event_ptr->session_type = UIM_SESSION_TYPE_TERTIARY_GW_V01;
      break;

    case MMGSDI_1X_PROV_TER_SESSION:
      ref_event_ptr->session_type = UIM_SESSION_TYPE_TERTIARY_1X_V01;
      break;

    case MMGSDI_NON_PROV_SESSION_SLOT_3:
      ref_event_ptr->session_type = UIM_SESSION_TYPE_NONPROVISIONING_SLOT_3_V01;
      break;

    case MMGSDI_CARD_SESSION_SLOT_3:
      ref_event_ptr->session_type = UIM_SESSION_TYPE_CARD_ON_SLOT_3_V01;
      break;

    default:
      /* Some session types are not sent to QMI clients */
      return FALSE;
  }

  /* Validating AID length */
  if(aid_ptr->data_len > QMI_UIM_AID_MAX_V01)
  {
    return FALSE;
  }

  /* AID length */
  ref_event_ptr->aid_len  = aid_ptr->data_len;

  /* Set number of files to zero initially */
  ref_event_ptr->files_len = 0;

  /* Copy AID in the TLV */
  (void)memscpy(ref_event_ptr->aid, sizeof(ref_event_ptr->aid),
                aid_ptr->data_ptr, aid_ptr->data_len);

  /* Loop thru all the files */
  if (refresh_evt_ptr->refresh_files.file_path_ptr != NULL)
  {
    boolean            error = FALSE;
    mmgsdi_path_type   file_path;

    memset(&file_path, 0, sizeof(file_path));

    for (i = 0; i < refresh_evt_ptr->refresh_files.file_path_len - 1; i += 2)
    {
      /* Check if there is the start of a new file */
      if (refresh_evt_ptr->refresh_files.file_path_ptr[i] == 0x3F &&
          refresh_evt_ptr->refresh_files.file_path_ptr[i + 1] == 0x00)
      {
        /* Store the previous file in the TLV */
        if (file_path.path_len > 0 && file_path.path_len <= MMGSDI_MAX_PATH_LEN && !error)
        {
          /* Increase total number of files */
          ref_event_ptr->files_len++;

          /* Calculate file id and path len for TLV */
          ref_event_ptr->files[file_count].file_id = file_path.path_buf[file_path.path_len - 1];
          ref_event_ptr->files[file_count].path_len = ((file_path.path_len - 1) * sizeof(uint16));

          /* Copy path in the TLV */
          if (file_path.path_len > 1)
          {

           (void)memscpy(ref_event_ptr->files[file_count].path, sizeof(ref_event_ptr->files[file_count].path),
                         &file_path.path_buf[0], ref_event_ptr->files[file_count].path_len);
          }
          file_count++;
        }

        /* Reset file path to start a new file */
        memset(&file_path, 0, sizeof(file_path));
        error = FALSE;
      }

      /* Insert data in the current file */
      if (file_path.path_len < MMGSDI_MAX_PATH_LEN)
      {
        file_path.path_buf[file_path.path_len]  = (uint16)(refresh_evt_ptr->refresh_files.file_path_ptr[i] & 0xFF);
        file_path.path_buf[file_path.path_len]  = file_path.path_buf[file_path.path_len] << 8;
        file_path.path_buf[file_path.path_len] += (uint16)(refresh_evt_ptr->refresh_files.file_path_ptr[i + 1] & 0xFF);

        file_path.path_len += 1;
      }
      else
      {
        UIM_MSG_ERR_0("Path too long... will be discarded");
        error = TRUE;
      }
    }

    /* Insert the last file in the TLV */
    if (file_path.path_len > 0 && file_path.path_len <= MMGSDI_MAX_PATH_LEN && !error)
    {
      /* Increase total number of files */
      ref_event_ptr->files_len++;

      /* Calculate file id and path len for TLV */
      ref_event_ptr->files[file_count].file_id   = file_path.path_buf[file_path.path_len - 1];
      ref_event_ptr->files[file_count].path_len  = ((file_path.path_len - 1) * sizeof(uint16));

       /* Copy path in the TLV */
      if (file_path.path_len > 1)
      {
        (void)memscpy(ref_event_ptr->files[file_count].path, sizeof(ref_event_ptr->files[file_count].path),
                      &file_path.path_buf[0], ref_event_ptr->files[file_count].path_len);
        file_count++;
      }
    }
  }

  return TRUE;
} /* qmi_uim_response_refresh_event */


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
)
{
  boolean  ret_value    = FALSE;
  uint16   index        = 0;
  uint16   tlv_len      = 0;
  uint8 *  tlv_data_ptr = NULL;
  uint16   len_value    = 0;

  ASSERT(content_valid_ptr && content_ptr && content_len_ptr);

  /* Compose payload */
  if (status == MMGSDI_SUCCESS)
  {
    ASSERT(res.data_ptr);

    /* Update response length based on the contexts requested:
       - ISIM AKA Authentication returns ck and ik in addition to res
       - HTTP Digest context returns res and ck
       - GBA Auth contexts ck & ik are not needed, so skip it altogether */
    switch (auth_context)
    {
      case UIM_AUTH_CONTEXT_GBA_SEC_BOOTSTRAPPING_V01:
      case UIM_AUTH_CONTEXT_GBA_SEC_NAF_DERIVATION_V01:
        len_value = (uint16)(res.data_len + 2);
        break;
      case UIM_AUTH_CONTEXT_HTTP_DIGEST_SEC_V01:
        len_value = (uint16)(res.data_len + ck.data_len + 3);
        break;
      case UIM_AUTH_CONTEXT_IMS_AKA_SEC_V01:
        len_value = (uint16)(res.data_len + ck.data_len + ik.data_len + 4);
        break;
      default:
        return FALSE;
    }

    tlv_len = len_value;
    tlv_data_ptr = uimqmi_malloc(tlv_len);

    if (tlv_data_ptr == NULL)
    {
      return FALSE;
    }

    /* Reconstruct the response from card. Success
       for all ISIM AUTH operations is 0xDB */
    tlv_data_ptr[index++] = 0xDB;

    tlv_data_ptr[index++] = (uint8)res.data_len;
    (void)memscpy((void*)(tlv_data_ptr + index), tlv_len - index,
                  (void*)res.data_ptr, (uint32)res.data_len);
    index += (uint16)res.data_len;

    /* Only ISIM AKA Authentication returns ck and ik in addition to res */
    if ((auth_context == UIM_AUTH_CONTEXT_IMS_AKA_SEC_V01) ||
        (auth_context == UIM_AUTH_CONTEXT_HTTP_DIGEST_SEC_V01))
    {
      tlv_data_ptr[index++] = (uint8)ck.data_len;
      if((ck.data_len > 0) && (ck.data_ptr != NULL))
      {
        (void)memscpy((void*)(tlv_data_ptr + index),
                      tlv_len - index,
                      (void*)ck.data_ptr,
                      (uint32)ck.data_len);
        index += (uint16)ck.data_len;
      }
    }
    if (auth_context == UIM_AUTH_CONTEXT_IMS_AKA_SEC_V01)
    {
      tlv_data_ptr[index++] = (uint8)ik.data_len;
      if ((ik.data_len > 0) && (ik.data_ptr != NULL))
      {
        (void)memscpy((void*)(tlv_data_ptr + index), tlv_len - index,
                      (void*)ik.data_ptr, (uint32)ik.data_len);
        index += (uint16)ik.data_len;
      }
    }
  }
  else if (status == MMGSDI_AUTS_FAIL)
  {
    ASSERT(auts.data_ptr);

    len_value = (uint16)(auts.data_len + 2);

    tlv_len = len_value;
    tlv_data_ptr = uimqmi_malloc(tlv_len);

    if (tlv_data_ptr == NULL)
    {
      return FALSE;
    }

    /* Reconstruct the response from card */
    tlv_data_ptr[index++] = 0xDC;

    tlv_data_ptr[index++] = (uint8)auts.data_len;
    (void)memscpy((void*)(tlv_data_ptr + index), tlv_len - index,
                  (void*)auts.data_ptr, (uint32)auts.data_len);
    index += (uint16)auts.data_len;
  }
  else
  {
    return FALSE;
  }
  
  ASSERT(content_buf_size > tlv_len);

  /* copy tlv in output ptr after removing len*/
  (void)memscpy((void*)content_ptr, content_buf_size,
                (void*)tlv_data_ptr, tlv_len);
  *content_valid_ptr = 1;
  *content_len_ptr = tlv_len;

  uimqmi_free(tlv_data_ptr);

  return ret_value;
} /* qmi_uim_response_authenticate_isim */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_AUTH_COMMON()

  DESCRIPTION
    Populate the response for authentication request. we can use common 
    function for all type of authentications response because response 
    structure is common for all.

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
)
{
  qmi_error_type_v01      errval = QMI_ERR_NONE_V01;
 
  ASSERT(content_valid_ptr && content_ptr && content_len_ptr && cnf_ptr);

  if(status == MMGSDI_SUCCESS && cnf_type != MMGSDI_ISIM_AUTH_CNF)
  {
    if(cnf_type == MMGSDI_RUN_CAVE_CNF)
    {
      ASSERT(content_buf_size >= 3);
      *content_len_ptr = 3;
      content_ptr[0]   = (uint8)((cnf_ptr->run_cave_cnf.run_cave_response_data >> 16) & 0xFF);
      content_ptr[1]   = (uint8)((cnf_ptr->run_cave_cnf.run_cave_response_data >> 8) & 0xFF);
      content_ptr[2]   = (uint8)(cnf_ptr->run_cave_cnf.run_cave_response_data & 0xFF);
      *content_valid_ptr = 1;
    }
    else if(cnf_type == MMGSDI_SESSION_RUN_GSM_ALGO_CNF)
    {
      ASSERT(content_buf_size >  MMGSDI_GSM_ALGO_SRES_LEN + MMGSDI_GSM_ALGO_KC_LEN);

      *content_len_ptr = MMGSDI_GSM_ALGO_SRES_LEN + MMGSDI_GSM_ALGO_KC_LEN;

     /* Reconstruct the response from card */
     (void)memscpy((void*)content_ptr,
                   content_buf_size,
                   (void*)&cnf_ptr->session_run_gsm_algo_cnf.sres_resp,
                   MMGSDI_GSM_ALGO_SRES_LEN);
     (void)memscpy((void*)(content_ptr + MMGSDI_GSM_ALGO_SRES_LEN),
                   content_buf_size - MMGSDI_GSM_ALGO_SRES_LEN,
                   (void*)&cnf_ptr->session_run_gsm_algo_cnf.kc_resp,
                   MMGSDI_GSM_ALGO_KC_LEN);
    }
    else if(cnf_type == MMGSDI_USIM_AUTH_CNF)
    {
      if (!qmi_uim_util_populate_opaque(content_valid_ptr,
                                        content_ptr,
                                        content_len_ptr,
                                        content_buf_size,
                                        cnf_ptr->usim_auth_cnf.auth_response.data_ptr,
                                        cnf_ptr->usim_auth_cnf.auth_response.data_len,
                                        TRUE))
      {
        errval = QMI_ERR_INTERNAL_V01;
      }
    }
    else if(cnf_type == MMGSDI_COMPUTE_IP_AUTH_CNF)
    {
      if (!qmi_uim_util_populate_opaque(content_valid_ptr,
                                        content_ptr,
                                        content_len_ptr,
                                        content_buf_size,
                                        cnf_ptr->compute_ip_cnf.cmpt_ip_response_data.data_ptr,
                                        cnf_ptr->compute_ip_cnf.cmpt_ip_response_data.data_len,
                                        TRUE))
      {
        errval = QMI_ERR_INTERNAL_V01;
      }
    }
  }
  else if(cnf_type == MMGSDI_ISIM_AUTH_CNF &&
          (status == MMGSDI_SUCCESS || status == MMGSDI_AUTS_FAIL))
  {
    if (FALSE == qmi_uim_response_authenticate_isim(
                              content_valid_ptr,
                              content_len_ptr,
                              content_ptr,
                              content_buf_size,
                              status,
                              auth_context,
                              cnf_ptr->isim_auth_cnf.res,
                              cnf_ptr->isim_auth_cnf.ck,
                              cnf_ptr->isim_auth_cnf.ik,
                              cnf_ptr->isim_auth_cnf.auts))
      {
        errval = QMI_ERR_INTERNAL_V01;
      }
  }
  else
  {
    errval = qmi_uim_util_convert_mmgsdi_status_to_errval(status);
  }
  
  return errval;
} /* qmi_uim_response_auth_common */


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
)
{
  ASSERT(response_ptr && valid_ptr);

  if (is_true)
  {
    *response_ptr = 1;
  }
  else
  {
    *response_ptr = 0;
  }
  *valid_ptr = 1;
} /* qmi_uim_response_boolean */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_HOT_SWAP_STATUS()

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
  uim_rt_status_enum *            hot_swap_status_ptr
)
{
  uint8              slot         = 0;

  ASSERT(hotswap_status_ptr && hot_swap_status_ptr && hotswap_len_ptr && is_valid);

  if (extended_status)
  {
    *hotswap_len_ptr = (uint32)(QMI_UIM_MAX_CARD_COUNT < hot_swap_status_len) ?
                     QMI_UIM_MAX_CARD_COUNT : hot_swap_status_len;
  }
  else
  {
    *hotswap_len_ptr = (uint32)(QMI_UIM_LEGACY_MAX_CARD_COUNT < hot_swap_status_len) ?
                     QMI_UIM_LEGACY_MAX_CARD_COUNT : hot_swap_status_len;
  }

  for(slot = 0;
      slot < QMI_UIM_MAX_CARD_COUNT && slot < *hotswap_len_ptr;
      slot++)
  {
    switch(hot_swap_status_ptr[slot])
    {
      case UIM_RT_STATUS_CARD_PRESENT:
        hotswap_status_ptr[slot] = UIM_HOT_SWAP_STATUS_PRESENT_V01;
        break;
      case UIM_RT_STATUS_CARD_ABSENT:
        hotswap_status_ptr[slot] = UIM_HOT_SWAP_STATUS_NOT_PRESENT_V01;
        break;
      case UIM_RT_STATUS_NOT_SUPPORTED:
      default:
        hotswap_status_ptr[slot] = UIM_HOT_SWAP_NOT_SUPPORTED_V01;
        break;
    }
  }
  *is_valid  = 1;
}/* qmi_uim_response_hot_swap_status */


/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_CARD_STATUS_VALIDITY()

  DESCRIPTION
    Composes the card status validity TLV content for get card status
    request

  PARAMETERS
    card_status_validity_ptr     : output pointer to card status validity array
    card_status_validity_len_ptr : len of output pointer to card status validity array
    is_field_valid               : output data is valid or not
    validity_len    : length of validity status array
    validity_ptr    : pointer to validity status array

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
)
{
  uint8              slot     = 0;
  boolean            skip_tlv = TRUE;

  ASSERT(card_status_validity_ptr && card_status_validity_len_ptr);

  /* Skip TLV on error input */
  if ((validity_ptr == NULL) ||
      (validity_len == 0) ||
      (validity_len > QMI_UIM_MAX_CARD_COUNT))
  {
    return FALSE;
  }

  /* Skip this TLV in case status in all slots are determined to be valid */
  for(slot = 0; slot < validity_len; slot++)
  {
    if (!validity_ptr[slot])
    {
      skip_tlv = FALSE;
      break;
    }
  }

  if (skip_tlv)
  {
    return TRUE;
  }

  * card_status_validity_len_ptr = validity_len;

  for(slot = 0; slot < QMI_UIM_MAX_CARD_COUNT && slot < validity_len; slot++)
  {
    card_status_validity_ptr[slot] = (validity_ptr[slot]) ? 1 : 0;
  }
  if(is_field_valid)
  {
    *is_field_valid = TRUE;
  }
  return TRUE;
} /* qmi_uim_response_card_status_validity */


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
)
{
  boolean file_invalidated = FALSE;

  ASSERT(valid_ptr && file_status_ptr && file_attrib_ptr);

  switch (file_attrib_ptr->file_type)
  {
     case MMGSDI_LINEAR_FIXED_FILE:
       file_invalidated =
         file_attrib_ptr->file_info.linear_fixed_file.file_status.file_invalidated;
       break;
     case MMGSDI_CYCLIC_FILE:
       file_invalidated =
         file_attrib_ptr->file_info.cyclic_file.file_status.file_invalidated;
       break;
     case MMGSDI_TRANSPARENT_FILE:
       file_invalidated =
         file_attrib_ptr->file_info.transparent_file.file_status.file_invalidated;
       break;
     default:
       return;
  }

  /* Update the value */
  *file_status_ptr = file_invalidated ? UIM_FILE_STATUS_DEACTIVATED_V01: UIM_FILE_STATUS_ACTIVATED_V01;
  *valid_ptr = TRUE;
} /* qmi_uim_response_file_status */

/*===========================================================================
  FUNCTION QMI_UIM_RESPONSE_SESSION_TYPE()

  DESCRIPTION
    populate the content for Session type

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
)
{

  ASSERT(uim_session_type_ptr);

  switch(session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      *uim_session_type_ptr = UIM_SESSION_TYPE_PRIMARY_GW_V01;
      break;
    case MMGSDI_1X_PROV_PRI_SESSION:
      *uim_session_type_ptr = UIM_SESSION_TYPE_PRIMARY_1X_V01;
      break;
    case MMGSDI_GW_PROV_SEC_SESSION:
      *uim_session_type_ptr = UIM_SESSION_TYPE_SECONDARY_GW_V01;
      break;
    case MMGSDI_1X_PROV_SEC_SESSION:
      *uim_session_type_ptr = UIM_SESSION_TYPE_SECONDARY_1X_V01;
      break;
    case MMGSDI_GW_PROV_TER_SESSION:
      *uim_session_type_ptr = UIM_SESSION_TYPE_TERTIARY_GW_V01;
      break;
    case MMGSDI_1X_PROV_TER_SESSION:
      *uim_session_type_ptr = UIM_SESSION_TYPE_TERTIARY_1X_V01;
      break;
    default:
      return FALSE;
  }
  return TRUE;
} /* qmi_uim_response_session_type */


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
)
{
  ASSERT(uim_cause);

  switch(mmgsdi_cause)
  {
    case MMGSDI_SESSION_CLOSED_CAUSE_CLIENT_REQUEST:
    case MMGSDI_SESSION_CLOSED_CAUSE_MISMATCH_FOUND:
      *uim_cause = UIM_SESSION_CLOSED_CAUSE_CLIENT_REQUEST_V01;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_CARD_ERROR:
      *uim_cause = UIM_SESSION_CLOSED_CAUSE_CARD_ERROR_V01;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_CARD_POWER_DOWN:
      *uim_cause = UIM_SESSION_CLOSED_CAUSE_CARD_POWER_DOWN_V01;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_CARD_REMOVED:
      *uim_cause = UIM_SESSION_CLOSED_CAUSE_CARD_REMOVED_V01;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_REFRESH:
      *uim_cause = UIM_SESSION_CLOSED_CAUSE_REFRESH_V01;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_PIN_STATUS_FAIL:
      *uim_cause = UIM_SESSION_CLOSED_CAUSE_PIN_STATUS_FAIL_V01;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_RECOVERY:
      *uim_cause = UIM_SESSION_CLOSED_CAUSE_RECOVERY_V01;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_FDN_SRV_ENABLED_WITH_NO_FDN_SUPPORT:
      *uim_cause = UIM_SESSION_CLOSED_CAUSE_FDN_SRV_ENABLED_WITH_NO_FDN_SUPPORT_V01;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_PERSO_OPERATION_FAIL:
      *uim_cause = UIM_SESSION_CLOSED_CAUSE_PERSO_OPERATION_FAIL_V01;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_FILE_CONTENT_INVALID_IMSI:
    case MMGSDI_SESSION_CLOSED_CAUSE_FILE_CONTENT_INVALID_ACC:
    case MMGSDI_SESSION_CLOSED_CAUSE_FILE_CONTENT_INVALID_PRL:
      *uim_cause = UIM_SESSION_CLOSED_CAUSE_FILE_CONTENT_INVALID_V01;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_AD:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_IMSI:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_ACC:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_HPPLMN:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_LOCI:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_PSLOCI:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_KEYS:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_FPLMN:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_START_HFN:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_THRESHOLD:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_KC:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_PHASE:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_CST:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_RUIM_ID:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_SERVICE_PREF:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_IMSI_M:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_IMSI_T:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_ACCOLC:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_DIST_BASED_REGN_IND:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_SYS_REGN_IND:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_OTAPA_SPC_ENABLE:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_NAM_LOCK:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_HOME_SID_NID:
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_ZONE_BASED_REGN_IND:
      *uim_cause = UIM_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_V01;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_UNKNOWN:
    default:
      *uim_cause = UIM_SESSION_CLOSED_CAUSE_UNKNOWN_V01;
      break;
  }
} /* qmi_uim_response_session_closed_cause */


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
  uint16_t                             * file_id_ptr,
  mmgsdi_session_closed_cause_enum_type  cause
)
{
  ASSERT(file_id_ptr);

  switch (cause)
  {
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_AD:
      *file_id_ptr = 0x6FAD;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_IMSI:
    case MMGSDI_SESSION_CLOSED_CAUSE_FILE_CONTENT_INVALID_IMSI:
      *file_id_ptr = 0x6F07;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_ACC:
    case MMGSDI_SESSION_CLOSED_CAUSE_FILE_CONTENT_INVALID_ACC:
      *file_id_ptr = 0x6F78;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_HPPLMN:
      *file_id_ptr = 0x6F31;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_LOCI:
      *file_id_ptr = 0x6F7E;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_PSLOCI:
      *file_id_ptr = 0x6F73;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_KEYS:
      *file_id_ptr = 0x6F08;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_FPLMN:
      *file_id_ptr = 0x6F7B;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_START_HFN:
      *file_id_ptr = 0x6F5B;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_THRESHOLD:
      *file_id_ptr = 0x6F5C;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_PHASE:
      *file_id_ptr = 0x6FAE;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_CST:
      *file_id_ptr = 0x6F32;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_RUIM_ID:
      *file_id_ptr = 0x6F31;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_SERVICE_PREF:
      *file_id_ptr = 0x6F37;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_IMSI_M:
      *file_id_ptr = 0x6F22;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_IMSI_T:
      *file_id_ptr = 0x6F23;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_ACCOLC:
      *file_id_ptr = 0x6F2C;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_DIST_BASED_REGN_IND:
      *file_id_ptr = 0x6F2B;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_SYS_REGN_IND:
      *file_id_ptr = 0x6F2A;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_OTAPA_SPC_ENABLE:
      *file_id_ptr = 0x6F34;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_NAM_LOCK:
      *file_id_ptr = 0x6F35;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_HOME_SID_NID:
      *file_id_ptr = 0x6F28;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_MANDATORY_FILE_MISSING_ZONE_BASED_REGN_IND:
      *file_id_ptr = 0x6F29;
      break;
    case MMGSDI_SESSION_CLOSED_CAUSE_FILE_CONTENT_INVALID_PRL:
      *file_id_ptr = 0x6F30;
      break;
    default:
      return FALSE;
  }

  return TRUE;
} /* qmi_uim_response_session_closed_file_id */


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
)
{
  uint16    in_index             = 0;
  uint8     out_index            = 0;
  uint8     num_tlvs_counter     = 0;
  uint8    *tlv_data_out_ptr     = NULL;
  uint32    tlv_data_out_len     = 0;
  uint8     current_tlv_len      = 0;

  ASSERT(ter_cabp_resp_ptr);

  if(num_tlvs > QMI_UIM_TERMINAL_CAPABILITY_TLVS_MAX_V01)
  {
    return FALSE;
  }
  
  if(num_tlvs > 0 &&
     tlvs_data_len > 0 &&
     tlvs_data_ptr)
  {
    /* Start from out_index 1 as zeroth index contains already the number
       of terminal capability TLVs */
    for(out_index = 0;
        out_index < num_tlvs && in_index < (tlvs_data_len - 1);
        out_index++)
    {
      ter_cabp_resp_ptr[out_index].value_len      = 0;

      /* Byte corresponding to the Tag of the TLV */
      ter_cabp_resp_ptr[out_index].tag = tlvs_data_ptr[in_index++];
      /* Byte corresponding to the Length of the TLV */
      ter_cabp_resp_ptr[out_index].value_len = tlvs_data_ptr[in_index++];

      if (ter_cabp_resp_ptr[out_index].value_len > 0)
      {
        if (ter_cabp_resp_ptr[out_index].value_len
              < QMI_UIM_TERMINAL_CAPABILITY_VALUE_MAX_V01 &&
            ter_cabp_resp_ptr[out_index].value_len <=
              (tlvs_data_len - in_index))
        {
          /* Bytes corresponding to the Value of the TLV */
          memscpy(&ter_cabp_resp_ptr[out_index].value,
                  QMI_UIM_TERMINAL_CAPABILITY_VALUE_MAX_V01,
                  &tlvs_data_ptr[in_index],
                  ter_cabp_resp_ptr[out_index].value_len);
          in_index += ter_cabp_resp_ptr[out_index].value_len;
        }
        else
        {
          return FALSE;
        }
      }
      num_tlvs_counter++;
    }
  }

  if(in_index == tlvs_data_len  &&
     num_tlvs_counter == num_tlvs)
  {
    /* Populate the first byte containing the number of TLVs */
    *num_ter_capb_ptr = num_tlvs;
    *ter_capb_valid_ptr = 1;
  }
  else
  {
     return FALSE;
  }

  return TRUE;
} /* qmi_uim_response_terminal_capability */


/*===========================================================================
  FUNCTION  QMI_UIM_RESPONSE_ENCRYPTED_STATUS()

  DESCRIPTION
    Encrypts and builds the response returned to security sensitive clients

  PARAMETERS
    cmd_id             : Command id
    request_id         : Request id
    result             : SimLock result status
    response_ptr       : Output data
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
)
{
  qmi_error_type_v01      errval              = QMI_ERR_NONE_V01;
  mmgsdi_data_type        rsp_data            = { 0, NULL };
  mmgsdi_data_type        encrypted_rsp_data  = { 0, NULL };
  boolean                 encrypted           = FALSE;
  uint32                  buflen              = 0;

  PACKED struct PACKED_POST
  {
    uint16   cmd_id;
    uint16   request_id;
    uint8    rfu[QMI_UIM_ENCRYPTED_MSG_RFU_SIZE];
    uint16   result;
    uint16   error;
  } rsp_data_buffer;

  if(response_ptr == NULL || len_ptr == NULL || *len_ptr == 0)
  {
    return QMI_ERR_INTERNAL_V01;
  }

  memset(&rsp_data_buffer, 0x00, sizeof(rsp_data_buffer));

  rsp_data_buffer.cmd_id = cmd_id;
  rsp_data_buffer.request_id = request_id;
  buflen = *len_ptr;

  if (error != QMI_ERR_NONE_V01)
  {
    rsp_data_buffer.result = QMI_RESULT_FAILURE_V01;
  }
  else
  {
    rsp_data_buffer.result = QMI_RESULT_SUCCESS_V01;
  }

  rsp_data_buffer.error = error;

  rsp_data.data_len = sizeof(rsp_data_buffer);
  rsp_data.data_ptr = (uint8*)&rsp_data_buffer;

  encrypted = qmi_uim_encrypt_payload(rsp_data,
                                      &encrypted_rsp_data);

  if (encrypted &&
      encrypted_rsp_data.data_len <= QMI_UIM_ENCRYPTED_RESP_MAX  &&
      buflen                      <= QMI_UIM_ENCRYPTED_RESP_MAX  &&  
      encrypted_rsp_data.data_ptr != NULL)
  {
    (void)qmi_uim_util_populate_opaque(NULL,
                                       response_ptr,
                                       len_ptr,
                                       buflen,
                                       encrypted_rsp_data.data_ptr,
                                       encrypted_rsp_data.data_len,
                                       FALSE);  
  }
  else
  {
    UIM_MSG_ERR_2("Encrypted: 0x%x, Encrypted data length: 0x%x",
                  encrypted,
                  encrypted_rsp_data.data_len);
    errval = QMI_ERR_INTERNAL_V01;
  }

  if(encrypted_rsp_data.data_ptr != NULL)
  {
    uimqmi_free(encrypted_rsp_data.data_ptr);
    encrypted_rsp_data.data_ptr = NULL;
  }

  return errval;
} /* qmi_uim_response_encrypted_status */

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
