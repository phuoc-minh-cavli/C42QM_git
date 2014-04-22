/*===========================================================================

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2009 - 2014, 2017 - 2018, 2023 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/uim.mpss/3.3.1/uimdiag/src/uimdiag_mmgsdi_util.c#2 $$ $DateTime: 2023/03/08 09:28:28 $ $Author: pwbldsvc $

when       who   what, where, why
--------   ---   -----------------------------------------------------------
03/07/23   pvb   Fix overflow issue
07/25/18   bcho  Support UIMDIAG interface to only trigger NAA Refresh
09/10/17   rps   Build flag for Simlock in IOE devices
04/17/17   tq    Decrement uimdiag command length
05/25/16   vdc   Remove F3 messages for memory allocation failure
03/07/16   df    Featurise file with FEATURE_UIMDIAG
01/10/14   df    Use v2.0 diag macros
12/11/13   df    Remove unused memory paging feature
11/05/13   df    Remove unnecessary include tmc.h
11/05/13   df    Fix LLVM compiler issue in util functions
09/25/13   bcho  Query type parsed in UIMDIAG_MMGSDI_SESSION_GET_INFO_CMD
05/06/13   sw    Changes made for secure version of memcpy
04/24/13   rr    Featurized optional logs
11/23/12   bcho  Support for RAT type param in get_operator_name API
11/09/12   yt    Remove parsing of length field for PERSO_GET_DATA_CMD
09/25/12   tl    Adding nvruim_data_req_enum_type to sessionid_type
10/24/11   adp   Removing Num_of_files from request field for naa_refresh
10/24/11   adp   Removing support for UIMDIAG_MMGSDI_SAP_PIN_VERIFY_CMD,
                 UIMDIAG_MMGSDI_SAP_INITIALIZE_CMD and
                 UIMDIAG_MMGSDI_SAP_DEREGISTER_CMD
10/10/11   adp   Adding uimdiag enum mapping
10/18/11   bcho  Adding support for mmgsdi_session_open_ext API
10/14/11   bcho  Added support for Get Operator Name API
10/10/11   nb    Fixed Incorrect featurization due to merge error
09/23/11   nb    Support for SAP CONNECT/SAP DISCONNECT
09/15/11   adp   Adding support to trigger refresh in uimdiag
09/15/11   nb    Support for new refresh stage
08/22/11   av    Added support for pdown,pup,activate onchip,send apdu commands
08/05/11   av    UIMDiag now doesn't return error if search pattern is null;
                 It's handled in mmgsdi now.
06/09/11   adp   Fixing get file attr by path access, Fixing search and seek
04/06/11   adp   Swapping the fileds for set notify and exclusive channel
                 to match with the ICD and the test scripts.
02/02/10   ms    Fixed compiler warnings
12/29/10   ms    Adding support for BT SAP APIs
12/03/10   adp   Adding support for a number of commands
10/08/10   adp   Changing the offset to get the correct enum from
                 mmgsdilib_common.h
09/07/10   adp   Adding Support for dck_num_retries,lock_down_enable_ltd_access,
                 seek and search commands
08/09/10   adp   Adding support for service enable, disable, get perso key,
                 is service available, perso unblock.
04/07/10   adp   Removing check for cmd_ptr->path_len == 0 check.
                 Path len could be zero.
03/25/10   adp   Adding support for mmgsdi_session_deactivate_provisioning,
                 mmgsdi_session_get_app_capabilities,
                 mmgsdi_session_get_cphs_info,
                 isim_authenticate,send_card_status,
                 create_pkcs15_lookup_table
03/23/10   adp   Fixing check for ASSERT
03/19/10   rk    Added support for JCDMA get info
02/22/10   adp   Adding support for Perso Commands
12/27/09   ap    Adding support for Refresh commands
12/03/09   rk    Adding support for get file attributes, read and write commands
10/18/09   adp   Initial Revision -Added mmgsdi util functions


=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"
#include "diagcmd.h"
#include "uimdiag_mmgsdi.h"
#include "uimdiag_mmgsdi_util.h"
#include "uim_msg.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*=============================================================================
    Static table containing mapping between user specified elementary file
    to gsdi task internal values.
=============================================================================*/
static const struct
{
  uimdiag_file_enum_type     uimdiag_file;
  mmgsdi_file_enum_type      mmgsdi_file;
}  uimdiag_file_to_mmgsdi_table[] =
{
  /* This table is intentionally missing some of the entries to save memory.
     please refer to the end of the table for those entries which are removed */
  {UIMDIAG_ICCID,MMGSDI_ICCID},
  {UIMDIAG_ELP,MMGSDI_ELP},
  {UIMDIAG_DIR,MMGSDI_DIR},
  {UIMDIAG_ARR,  MMGSDI_ARR},
  {UIMDIAG_CDMA_CC,MMGSDI_CDMA_CC},
  {UIMDIAG_CDMA_IMSI_M,MMGSDI_CDMA_IMSI_M},
  {UIMDIAG_CDMA_IMSI_T,  MMGSDI_CDMA_IMSI_T},
  {UIMDIAG_CDMA_TMSI,MMGSDI_CDMA_TMSI},
  {UIMDIAG_CDMA_ANALOG_HOME_SID,MMGSDI_CDMA_ANALOG_HOME_SID},
  {UIMDIAG_CDMA_ANALOG_OP_PARAMS,MMGSDI_CDMA_ANALOG_OP_PARAMS},
  {UIMDIAG_CDMA_ANALOG_LOCN_AND_REGN_IND,MMGSDI_CDMA_ANALOG_LOCN_AND_REGN_IND},
  {UIMDIAG_CDMA_HOME_SID_NID,MMGSDI_CDMA_HOME_SID_NID},
  {UIMDIAG_CDMA_ZONE_BASED_REGN_IND,MMGSDI_CDMA_ZONE_BASED_REGN_IND},
  {UIMDIAG_CDMA_SYS_REGN_IND,MMGSDI_CDMA_SYS_REGN_IND},
  {UIMDIAG_CDMA_DIST_BASED_REGN_IND,MMGSDI_CDMA_DIST_BASED_REGN_IND},
  {UIMDIAG_CDMA_ACCOLC,MMGSDI_CDMA_ACCOLC},
  {UIMDIAG_CDMA_CALL_TERM_MODE_PREF,MMGSDI_CDMA_CALL_TERM_MODE_PREF},
  {UIMDIAG_CDMA_ANALOG_CHAN_PREF,MMGSDI_CDMA_ANALOG_CHAN_PREF},
  {UIMDIAG_CDMA_PRL,MMGSDI_CDMA_PRL},
  {UIMDIAG_CDMA_RUIM_ID,MMGSDI_CDMA_RUIM_ID},
  {UIMDIAG_CDMA_SVC_TABLE,MMGSDI_CDMA_SVC_TABLE},
  {UIMDIAG_CDMA_SPC,MMGSDI_CDMA_SPC},
  {UIMDIAG_CDMA_OTAPA_SPC_ENABLE, MMGSDI_CDMA_OTAPA_SPC_ENABLE},
  {UIMDIAG_CDMA_NAM_LOCK,MMGSDI_CDMA_NAM_LOCK},
  {UIMDIAG_CDMA_OTASP_OTAPA_FEATURES, MMGSDI_CDMA_OTASP_OTAPA_FEATURES},
  {UIMDIAG_CDMA_SERVICE_PREF,MMGSDI_CDMA_SERVICE_PREF},
  {UIMDIAG_CDMA_ESN_ME,MMGSDI_CDMA_ESN_ME},
  {UIMDIAG_CDMA_RUIM_PHASE,MMGSDI_CDMA_RUIM_PHASE},
  {UIMDIAG_CDMA_PREF_LANG,MMGSDI_CDMA_PREF_LANG},
  {UIMDIAG_CDMA_SMS,MMGSDI_CDMA_SMS},
  {UIMDIAG_CDMA_UIM_ID_USAGE_IND,MMGSDI_CDMA_UIM_ID_USAGE_IND},
  {UIMDIAG_CDMA_ADM_DATA,MMGSDI_CDMA_ADM_DATA},
  {UIMDIAG_CDMA_MAXIMUM_PRL,MMGSDI_CDMA_MAXIMUM_PRL},
  {UIMDIAG_CDMA_SPC_STATUS,MMGSDI_CDMA_SPC_STATUS},
  {UIMDIAG_CDMA_ECC,MMGSDI_CDMA_ECC},
  {UIMDIAG_CDMA_MECRP,MMGSDI_CDMA_MECRP},
  {UIMDIAG_CDMA_SF_EUIM_ID,  MMGSDI_CDMA_SF_EUIM_ID},
  {UIMDIAG_GSM_LP,MMGSDI_GSM_LP},
  {UIMDIAG_GSM_IMSI,MMGSDI_GSM_IMSI},
  {UIMDIAG_GSM_KC,MMGSDI_GSM_KC},
  {UIMDIAG_GSM_PLMN,MMGSDI_GSM_PLMN},
  {UIMDIAG_GSM_HPLMN,MMGSDI_GSM_HPLMN},
  {UIMDIAG_GSM_SST,MMGSDI_GSM_SST},
  {UIMDIAG_GSM_ACM,MMGSDI_GSM_ACM},
  {UIMDIAG_GSM_SPN,MMGSDI_GSM_SPN},
  {UIMDIAG_GSM_BCCH,MMGSDI_GSM_BCCH},
  {UIMDIAG_GSM_ACC,MMGSDI_GSM_ACC},
  {UIMDIAG_GSM_FPLMN,MMGSDI_GSM_FPLMN},
  {UIMDIAG_GSM_LOCI,MMGSDI_GSM_LOCI},
  {UIMDIAG_GSM_AD,MMGSDI_GSM_AD},
  {UIMDIAG_GSM_PHASE,MMGSDI_GSM_PHASE},
  {UIMDIAG_GSM_ECC,MMGSDI_GSM_ECC},
  {UIMDIAG_GSM_KCGPRS,MMGSDI_GSM_KCGPRS},
  {UIMDIAG_GSM_LOCIGPRS,MMGSDI_GSM_LOCIGPRS},
  {UIMDIAG_GSM_PNN,MMGSDI_GSM_PNN},
  {UIMDIAG_GSM_SPDI,MMGSDI_GSM_SPDI},
  {UIMDIAG_TELECOM_ADN,MMGSDI_TELECOM_ADN},
  {UIMDIAG_TELECOM_FDN,MMGSDI_TELECOM_FDN},
  {UIMDIAG_TELECOM_SMS,MMGSDI_TELECOM_SMS},
  {UIMDIAG_USIM_LI,MMGSDI_USIM_LI},
  {UIMDIAG_USIM_IMSI,MMGSDI_USIM_IMSI},
  {UIMDIAG_USIM_KEYS,MMGSDI_USIM_KEYS},
  {UIMDIAG_USIM_KEYSPS,MMGSDI_USIM_KEYSPS},
  {UIMDIAG_USIM_PLMNWACT,MMGSDI_USIM_PLMNWACT},
  {UIMDIAG_USIM_UPLMNSEL,MMGSDI_USIM_UPLMNSEL},
  {UIMDIAG_USIM_HPLMN,MMGSDI_USIM_HPLMN},
  {UIMDIAG_USIM_UST,MMGSDI_USIM_UST},
  {UIMDIAG_USIM_GID1,MMGSDI_USIM_GID1},
  {UIMDIAG_USIM_GID2,MMGSDI_USIM_GID2},
  {UIMDIAG_USIM_SPN,MMGSDI_USIM_SPN},
  {UIMDIAG_USIM_ACC,MMGSDI_USIM_ACC},
  {UIMDIAG_USIM_FPLMN,MMGSDI_USIM_FPLMN},
  {UIMDIAG_USIM_LOCI,MMGSDI_USIM_LOCI},
  {UIMDIAG_USIM_AD,MMGSDI_USIM_AD},
  {UIMDIAG_USIM_ECC,MMGSDI_USIM_ECC},
  {UIMDIAG_USIM_PSLOCI,MMGSDI_USIM_PSLOCI},
  {UIMDIAG_USIM_FDN,MMGSDI_USIM_FDN},
  {UIMDIAG_USIM_SMS,MMGSDI_USIM_SMS},
  {UIMDIAG_USIM_MSISDN,MMGSDI_USIM_MSISDN},
  {UIMDIAG_USIM_SMSP,MMGSDI_USIM_SMSP},
  {UIMDIAG_USIM_SMSS,MMGSDI_USIM_SMSS},
  {UIMDIAG_USIM_SDN,MMGSDI_USIM_SDN},
  {UIMDIAG_USIM_ICI,MMGSDI_USIM_ICI},
  {UIMDIAG_USIM_HIDDENKEY,MMGSDI_USIM_HIDDENKEY},
  {UIMDIAG_USIM_BDN,MMGSDI_USIM_BDN},
  {UIMDIAG_USIM_EST,MMGSDI_USIM_EST},
  {UIMDIAG_USIM_START_HFN,MMGSDI_USIM_START_HFN},
  {UIMDIAG_USIM_THRESHOLD,MMGSDI_USIM_THRESHOLD},
  {UIMDIAG_USIM_OPLMNWACT,MMGSDI_USIM_OPLMNWACT},
  {UIMDIAG_USIM_HPLMNWACT,MMGSDI_USIM_HPLMNWACT},
  {UIMDIAG_USIM_NETPAR,MMGSDI_USIM_NETPAR},
  {UIMDIAG_USIM_PNN,MMGSDI_USIM_PNN},
  {UIMDIAG_USIM_OPL,MMGSDI_USIM_OPL},
  {UIMDIAG_USIM_SPDI,MMGSDI_USIM_SPDI},
  {UIMDIAG_USIM_KC,MMGSDI_USIM_KC},
  {UIMDIAG_USIM_KCGPRS,MMGSDI_USIM_KCGPRS},
  {UIMDIAG_USIM_EPSLOCI,MMGSDI_USIM_EPSLOCI},
  {UIMDIAG_USIM_EPSNSC,MMGSDI_USIM_EPSNSC},
  {UIMDIAG_USIM_SPNI,MMGSDI_USIM_SPNI},
  {UIMDIAG_USIM_PNNI,MMGSDI_USIM_PNNI},
  {UIMDIAG_ISIM_IMPI,MMGSDI_ISIM_IMPI},
  {UIMDIAG_ISIM_DOMAIN,MMGSDI_ISIM_DOMAIN},
  {UIMDIAG_ISIM_IMPU,MMGSDI_ISIM_IMPU},
  {UIMDIAG_ISIM_ARR,MMGSDI_ISIM_ARR},
  {UIMDIAG_ISIM_KEYS,MMGSDI_ISIM_KEYS},
  {UIMDIAG_ISIM_AD,MMGSDI_ISIM_AD},
  {UIMDIAG_SIM_7F66_PROP1_ACT_HPLMN,MMGSDI_SIM_7F66_PROP1_ACT_HPLMN},
  {UIMDIAG_SIM_7F66_PROP1_SPT_TABLE,  MMGSDI_SIM_7F66_PROP1_SPT_TABLE},
  {UIMDIAG_USIM_7F66_PROP1_ACT_HPLMN,MMGSDI_USIM_7F66_PROP1_ACT_HPLMN},
  {UIMDIAG_USIM_7F66_PROP1_SPT_TABLE,MMGSDI_USIM_7F66_PROP1_SPT_TABLE},
  {UIMDIAG_USIM_7F66_PROP1_RAT,  MMGSDI_USIM_7F66_PROP1_RAT},
  {UIMDIAG_CSIM_ARR,MMGSDI_CSIM_ARR},
  {UIMDIAG_CSIM_CALL_COUNT,MMGSDI_CSIM_CALL_COUNT},
  {UIMDIAG_CSIM_IMSI_M,MMGSDI_CSIM_IMSI_M},
  {UIMDIAG_CSIM_IMSI_T,MMGSDI_CSIM_IMSI_T},
  {UIMDIAG_CSIM_TMSI,MMGSDI_CSIM_TMSI},
  {UIMDIAG_CSIM_ANALOG_HOME_SID,MMGSDI_CSIM_ANALOG_HOME_SID},
  {UIMDIAG_CSIM_ANALOG_OP_PARAMS,MMGSDI_CSIM_ANALOG_OP_PARAMS},
  {UIMDIAG_CSIM_ANALOG_LOCN_AND_REGN_IND,MMGSDI_CSIM_ANALOG_LOCN_AND_REGN_IND},
  {UIMDIAG_CSIM_CDMA_HOME_SID_NID,MMGSDI_CSIM_CDMA_HOME_SID_NID},
  {UIMDIAG_CSIM_CDMA_ZONE_BASED_REGN_IND,MMGSDI_CSIM_CDMA_ZONE_BASED_REGN_IND},
  {UIMDIAG_CSIM_CDMA_SYS_REGN_IND,MMGSDI_CSIM_CDMA_SYS_REGN_IND},
  {UIMDIAG_CSIM_CDMA_DIST_BASED_REGN_IND,MMGSDI_CSIM_CDMA_DIST_BASED_REGN_IND},
  {UIMDIAG_CSIM_ACCOLC,MMGSDI_CSIM_ACCOLC},
  {UIMDIAG_CSIM_CALL_TERM_MODE_PREF,MMGSDI_CSIM_CALL_TERM_MODE_PREF},
  {UIMDIAG_CSIM_ANALOG_CHAN_PREF,MMGSDI_CSIM_ANALOG_CHAN_PREF},
  {UIMDIAG_CSIM_PRL,MMGSDI_CSIM_PRL},
  {UIMDIAG_CSIM_RUIM_ID,MMGSDI_CSIM_RUIM_ID},
  {UIMDIAG_CSIM_CST,MMGSDI_CSIM_CST},
  {UIMDIAG_CSIM_SPC,MMGSDI_CSIM_SPC},
  {UIMDIAG_CSIM_OTAPA_SPC_ENABLE,MMGSDI_CSIM_OTAPA_SPC_ENABLE},
  {UIMDIAG_CSIM_NAM_LOCK,MMGSDI_CSIM_NAM_LOCK},
  {UIMDIAG_CSIM_OTASP_OTAPA_FEATURES,MMGSDI_CSIM_OTASP_OTAPA_FEATURES},
  {UIMDIAG_CSIM_SERVICE_PREF,MMGSDI_CSIM_SERVICE_PREF},
  {UIMDIAG_CSIM_ESN_ME,MMGSDI_CSIM_ESN_ME},
  {UIMDIAG_CSIM_LANG_IND,MMGSDI_CSIM_LANG_IND},
  {UIMDIAG_CSIM_FDN,MMGSDI_CSIM_FDN},
  {UIMDIAG_CSIM_SMS,MMGSDI_CSIM_SMS},
  {UIMDIAG_CSIM_SMS_PARAMS,MMGSDI_CSIM_SMS_PARAMS},
  {UIMDIAG_CSIM_SPN,MMGSDI_CSIM_SPN},
  {UIMDIAG_CSIM_USGIND,MMGSDI_CSIM_USGIND},
  {UIMDIAG_CSIM_AD,MMGSDI_CSIM_AD},
  {UIMDIAG_CSIM_MSISDN,MMGSDI_CSIM_MSISDN},
  {UIMDIAG_CSIM_MAXIMUM_PRL,MMGSDI_CSIM_MAXIMUM_PRL},
  {UIMDIAG_CSIM_SPC_STATUS,MMGSDI_CSIM_SPC_STATUS},
  {UIMDIAG_CSIM_ECC,MMGSDI_CSIM_ECC},
  {UIMDIAG_CSIM_MECRP,MMGSDI_CSIM_MECRP},
  {UIMDIAG_CSIM_EPRL,MMGSDI_CSIM_EPRL},
  {UIMDIAG_CSIM_HOME_TAG,MMGSDI_CSIM_HOME_TAG},
  {UIMDIAG_CSIM_GROUP_TAG,MMGSDI_CSIM_GROUP_TAG},
  {UIMDIAG_CSIM_SPECIFIC_TAG,MMGSDI_CSIM_SPECIFIC_TAG},
  {UIMDIAG_CSIM_CALL_PROMPT,MMGSDI_CSIM_CALL_PROMPT},
  {UIMDIAG_CSIM_SF_EUIM_ID,MMGSDI_CSIM_SF_EUIM_ID},
  {UIMDIAG_CSIM_ROOT_CERT,MMGSDI_CSIM_ROOT_CERT},
  {UIMDIAG_USIM_7F66_PROP1_RPM_EF,MMGSDI_USIM_7F66_PROP1_RPM_EF},
  {UIMDIAG_USIM_7F66_PROP1_RPM_PARAM,MMGSDI_USIM_7F66_PROP1_RPM_PARAM},
  {UIMDIAG_TELECOM_ICE_DN,MMGSDI_TELECOM_ICE_DN},
  {UIMDIAG_TELECOM_ICE_FF,MMGSDI_TELECOM_ICE_FF}

  /* Following Entries which are not used are removed to save image size
  {UIMDIAG_CDMA_SCI,MMGSDI_CDMA_SCI},
  {UIMDIAG_CDMA_UNASSIGNED_1,MMGSDI_CDMA_UNASSIGNED_1},
  {UIMDIAG_CDMA_SMS_PARAMS,MMGSDI_CDMA_SMS_PARAMS},
  {UIMDIAG_CDMA_SMS_STATUS,MMGSDI_CDMA_SMS_STATUS},
  {UIMDIAG_CDMA_SUP_SVCS_FEATURE_CODE_TABLE,MMGSDI_CDMA_SUP_SVCS_FEATURE_CODE_TABLE},
  {UIMDIAG_CDMA_UNASSIGNED_2,MMGSDI_CDMA_UNASSIGNED_2},
  {UIMDIAG_CDMA_HOME_SVC_PVDR_NAME,MMGSDI_CDMA_HOME_SVC_PVDR_NAME},
  {UIMDIAG_CDMA_MSISDN,MMGSDI_CDMA_MSISDN},
  {UIMDIAG_CDMA_3GPD_ME3GPDOPC,MMGSDI_CDMA_3GPD_ME3GPDOPC},
  {UIMDIAG_CDMA_3GPD_3GPDOPM,MMGSDI_CDMA_3GPD_3GPDOPM},
  {UIMDIAG_CDMA_3GPD_SIPCAP,MMGSDI_CDMA_3GPD_SIPCAP},
  {UIMDIAG_CDMA_3GPD_MIPCAP,MMGSDI_CDMA_3GPD_MIPCAP},
  {UIMDIAG_CDMA_3GPD_SIPUPP,MMGSDI_CDMA_3GPD_SIPUPP},
  {UIMDIAG_CDMA_3GPD_MIPUPP,MMGSDI_CDMA_3GPD_MIPUPP},
  {UIMDIAG_CDMA_3GPD_SIPSP,MMGSDI_CDMA_3GPD_SIPSP},
  {UIMDIAG_CDMA_3GPD_MIPSP,MMGSDI_CDMA_3GPD_MIPSP},
  {UIMDIAG_CDMA_3GPD_SIPPAPSS, MMGSDI_CDMA_3GPD_SIPPAPSS},
  {UIMDIAG_CDMA_UNASSIGNED_3,MMGSDI_CDMA_UNASSIGNED_3},
  {UIMDIAG_CDMA_UNASSIGNED_4,MMGSDI_CDMA_UNASSIGNED_4},
  {UIMDIAG_CDMA_PUZL,MMGSDI_CDMA_PUZL},
  {UIMDIAG_CDMA_MAXPUZL,MMGSDI_CDMA_MAXPUZL},
  {UIMDIAG_CDMA_HRPDCAP,MMGSDI_CDMA_HRPDCAP},
  {UIMDIAG_CDMA_HRPDUPP,MMGSDI_CDMA_HRPDUPP},
  {UIMDIAG_CDMA_CSSPR,MMGSDI_CDMA_CSSPR},
  {UIMDIAG_CDMA_ATC,MMGSDI_CDMA_ATC},
  {UIMDIAG_CDMA_EPRL,MMGSDI_CDMA_EPRL},
  {UIMDIAG_CDMA_BCSMS_CONFIG,MMGSDI_CDMA_BCSMS_CONFIG},
  {UIMDIAG_CDMA_BCSMS_PREF,MMGSDI_CDMA_BCSMS_PREF},
  {UIMDIAG_CDMA_BCSMS_TABLE,MMGSDI_CDMA_BCSMS_TABLE},
  {UIMDIAG_CDMA_BCSMS_PARAMS,MMGSDI_CDMA_BCSMS_PARAMS},
  {UIMDIAG_CDMA_MMS_NOTIF,MMGSDI_CDMA_MMS_NOTIF},
  {UIMDIAG_CDMA_MMS_EXT8,MMGSDI_CDMA_MMS_EXT8},
  {UIMDIAG_CDMA_MMS_ICP,MMGSDI_CDMA_MMS_ICP},
  {UIMDIAG_CDMA_MMS_UP,MMGSDI_CDMA_MMS_UP},
  {UIMDIAG_CDMA_SMS_CAP,MMGSDI_CDMA_SMS_CAP},
  {UIMDIAG_CDMA_3GPD_IPV6CAP,MMGSDI_CDMA_3GPD_IPV6CAP},
  {UIMDIAG_CDMA_3GPD_MIPFLAGS,MMGSDI_CDMA_3GPD_MIPFLAGS},
  {UIMDIAG_CDMA_3GPD_TCPCONFIG,MMGSDI_CDMA_3GPD_TCPCONFIG},
  {UIMDIAG_CDMA_3GPD_DGC,MMGSDI_CDMA_3GPD_DGC},
  {UIMDIAG_CDMA_BROWSER_CP,MMGSDI_CDMA_BROWSER_CP},
  {UIMDIAG_CDMA_BROWSER_BM,MMGSDI_CDMA_BROWSER_BM},
  {UIMDIAG_CDMA_3GPD_SIPUPPEXT,MMGSDI_CDMA_3GPD_SIPUPPEXT},
  {UIMDIAG_CDMA_MMS_CONFIG,MMGSDI_CDMA_MMS_CONFIG},
  {UIMDIAG_CDMA_JAVA_DURL,MMGSDI_CDMA_JAVA_DURL},
  {UIMDIAG_CDMA_3GPD_MIPUPPEXT,MMGSDI_CDMA_3GPD_MIPUPPEXT},
  {UIMDIAG_CDMA_BREW_DLOAD,MMGSDI_CDMA_BREW_DLOAD},
  {UIMDIAG_CDMA_BREW_TSID,MMGSDI_CDMA_BREW_TSID},
  {UIMDIAG_CDMA_BREW_SID,MMGSDI_CDMA_BREW_SID},
  {UIMDIAG_CDMA_LBS_XCONFIG,MMGSDI_CDMA_LBS_XCONFIG},
  {UIMDIAG_CDMA_LBS_XSURL,MMGSDI_CDMA_LBS_XSURL},
  {UIMDIAG_CDMA_LBS_V2CONFIG,MMGSDI_CDMA_LBS_V2CONFIG},
  {UIMDIAG_CDMA_LBS_V2PDEADDR, MMGSDI_CDMA_LBS_V2PDEADDR},
  {UIMDIAG_CDMA_LBS_V2MPCADDR,MMGSDI_CDMA_LBS_V2MPCADDR},
  {UIMDIAG_CDMA_BREW_AEP,MMGSDI_CDMA_BREW_AEP},
  {UIMDIAG_CDMA_MODEL,MMGSDI_CDMA_MODEL},
  {UIMDIAG_CDMA_RC,MMGSDI_CDMA_RC},
  {UIMDIAG_CDMA_APP_LABELS,MMGSDI_CDMA_APP_LABELS},
  {UIMDIAG_CDMA_USER_AGENT_STRING,MMGSDI_CDMA_USER_AGENT_STRING},
  {UIMDIAG_CDMA_GID_REFERENCES,MMGSDI_CDMA_GID_REFERENCES},
  {UIMDIAG_CDMA_ME_DOWNLOADABLE_DATA,MMGSDI_CDMA_ME_DOWNLOADABLE_DATA},
  {UIMDIAG_CDMA_ME_SETTING_DATA,MMGSDI_CDMA_ME_SETTING_DATA},
  {UIMDIAG_CDMA_ME_USER_DATA,MMGSDI_CDMA_ME_USER_DATA},
  {UIMDIAG_CDMA_RESERVED2,MMGSDI_CDMA_RESERVED2},
  {UIMDIAG_CDMA_RESERVED1,  MMGSDI_CDMA_RESERVED1},
  {UIMDIAG_CDMA_UIM_SVC_TABLE,MMGSDI_CDMA_UIM_SVC_TABLE},
  {UIMDIAG_CDMA_UIM_FEATURES,MMGSDI_CDMA_UIM_FEATURES},
  {UIMDIAG_CDMA_HRPD_HRPDUPP,MMGSDI_CDMA_HRPD_HRPDUPP},
  {UIMDIAG_CDMA_UIM_VERSION,MMGSDI_CDMA_UIM_VERSION},
  {UIMDIAG_CDMA_IMSI_STATUS,MMGSDI_CDMA_IMSI_STATUS},
  {UIMDIAG_CDMA_GID1,MMGSDI_CDMA_GID1},
  {UIMDIAG_CDMA_GID2,MMGSDI_CDMA_GID2},
  {UIMDIAG_CDMA_3GPD_OP_CAP,MMGSDI_CDMA_3GPD_OP_CAP},
  {UIMDIAG_CDMA_3GPD_UPPEXT,MMGSDI_CDMA_3GPD_UPPEXT},
  {UIMDIAG_GSM_ACM_MAX,MMGSDI_GSM_ACM_MAX},
  {UIMDIAG_GSM_GID1,MMGSDI_GSM_GID1},
  {UIMDIAG_GSM_GID2,MMGSDI_GSM_GID2},
  {UIMDIAG_GSM_PUCT,MMGSDI_GSM_PUCT},
  {UIMDIAG_GSM_CBMI,MMGSDI_GSM_CBMI},
  {UIMDIAG_GSM_VGCS,MMGSDI_GSM_VGCS},
  {UIMDIAG_GSM_VGCSS,MMGSDI_GSM_VGCSS},
  {UIMDIAG_GSM_VBS,MMGSDI_GSM_VBS},
  {UIMDIAG_GSM_VBSS,MMGSDI_GSM_VBSS},
  {UIMDIAG_GSM_EMLPP,MMGSDI_GSM_EMLPP},
  {UIMDIAG_GSM_AAEM,MMGSDI_GSM_AAEM},
  {UIMDIAG_GSM_CBMID,MMGSDI_GSM_CBMID},
  {UIMDIAG_GSM_CBMIR,MMGSDI_GSM_CBMIR},
  {UIMDIAG_GSM_CBMIR,MMGSDI_GSM_CBMIR},
  {UIMDIAG_GSM_CNL,MMGSDI_GSM_CNL},
  {UIMDIAG_GSM_NIA,MMGSDI_GSM_NIA},
  {UIMDIAG_GSM_SUME,MMGSDI_GSM_SUME},
  {UIMDIAG_GSM_PLMNWACT,MMGSDI_GSM_PLMNWACT},
  {UIMDIAG_GSM_OPLMNWACT,MMGSDI_GSM_OPLMNWACT},
  {UIMDIAG_GSM_HPLMNACT,MMGSDI_GSM_HPLMNACT},
  {UIMDIAG_GSM_CPBCCH,MMGSDI_GSM_CPBCCH},
  {UIMDIAG_GSM_INVSCAN,MMGSDI_GSM_INVSCAN},
  {UIMDIAG_GSM_RPLMNAT,MMGSDI_GSM_RPLMNAT},
  {UIMDIAG_GSM_OPL,MMGSDI_GSM_OPL},
  {UIMDIAG_GSM_MBDN,MMGSDI_GSM_MBDN},
  {UIMDIAG_GSM_EXT6,MMGSDI_GSM_EXT6},
  {UIMDIAG_GSM_MBI,MMGSDI_GSM_MBI},
  {UIMDIAG_GSM_MWIS,MMGSDI_GSM_MWIS},
  {UIMDIAG_GSM_EXT1,MMGSDI_GSM_EXT1},
  {UIMDIAG_GSM_CFIS,MMGSDI_GSM_CFIS},
  {UIMDIAG_GSM_VMWI,MMGSDI_GSM_VMWI},
  {UIMDIAG_GSM_SVC_STR_TBL,MMGSDI_GSM_SVC_STR_TBL},
  {UIMDIAG_GSM_CFF,MMGSDI_GSM_CFF},
  {UIMDIAG_GSM_ONS,MMGSDI_GSM_ONS},
  {UIMDIAG_GSM_CSP,MMGSDI_GSM_CSP},
  {UIMDIAG_GSM_CPHSI,MMGSDI_GSM_CPHSI},
  {UIMDIAG_GSM_MN,MMGSDI_GSM_MN},
  {UIMDIAG_GSM_SAI,MMGSDI_GSM_SAI},
  {UIMDIAG_GSM_SLL,MMGSDI_GSM_SLL},
  {UIMDIAG_GSM_MEXE_ST,MMGSDI_GSM_MEXE_ST},
  {UIMDIAG_GSM_ORPK,MMGSDI_GSM_ORPK},
  {UIMDIAG_GSM_ARPK,MMGSDI_GSM_ARPK},
  {UIMDIAG_GSM_TPRPK,MMGSDI_GSM_TPRPK},
  {UIMDIAG_GSM_IMG,MMGSDI_GSM_IMG},
  {UIMDIAG_GSM_IMG1INST1,MMGSDI_GSM_IMG1INST1},
  {UIMDIAG_GSM_IMG1INST2,MMGSDI_GSM_IMG1INST2},
  {UIMDIAG_GSM_IMG1INST3,MMGSDI_GSM_IMG1INST3},
  {UIMDIAG_GSM_IMG2INST1,MMGSDI_GSM_IMG2INST1},
  {UIMDIAG_GSM_IMG2INST2,MMGSDI_GSM_IMG2INST2},
  {UIMDIAG_GSM_IMG2INST3,MMGSDI_GSM_IMG2INST3},
  {UIMDIAG_GSM_IMG3INST1,MMGSDI_GSM_IMG3INST1},
  {UIMDIAG_GSM_IMG3INST2,MMGSDI_GSM_IMG3INST2},
  {UIMDIAG_GSM_IMG3INST3,MMGSDI_GSM_IMG3INST3},
  {UIMDIAG_IMAGE,MMGSDI_IMAGE},
  {UIMDIAG_TELECOM_CCP,MMGSDI_TELECOM_CCP},
  {UIMDIAG_TELECOM_ECCP,MMGSDI_TELECOM_ECCP},
  {UIMDIAG_TELECOM_MSISDN,MMGSDI_TELECOM_MSISDN},
  {UIMDIAG_TELECOM_SMSP,MMGSDI_TELECOM_SMSP},
  {UIMDIAG_TELECOM_SMSS,MMGSDI_TELECOM_SMSS},
  {UIMDIAG_TELECOM_LND,MMGSDI_TELECOM_LND},
  {UIMDIAG_TELECOM_SDN,MMGSDI_TELECOM_SDN},
  {UIMDIAG_TELECOM_EXT1,MMGSDI_TELECOM_EXT1},
  {UIMDIAG_TELECOM_EXT2,MMGSDI_TELECOM_EXT2},
  {UIMDIAG_TELECOM_EXT3,MMGSDI_TELECOM_EXT3},
  {UIMDIAG_TELECOM_BDN,MMGSDI_TELECOM_BDN},
  {UIMDIAG_TELECOM_EXT4,MMGSDI_TELECOM_EXT4},
  {UIMDIAG_TELECOM_SMSR,MMGSDI_TELECOM_SMSR},
  {UIMDIAG_TELECOM_CMI,MMGSDI_TELECOM_CMI},
  {UIMDIAG_TELECOM_SUME,MMGSDI_TELECOM_SUME},
  {UIMDIAG_TELECOM_ARR,MMGSDI_TELECOM_ARR},
  {UIMDIAG_IMAGE_FILE,MMGSDI_IMAGE_FILE},
  {UIMDIAG_TELECOM_PBR,MMGSDI_TELECOM_PBR},
  {UIMDIAG_TELECOM_PSC,MMGSDI_TELECOM_PSC},
  {UIMDIAG_TELECOM_CC,MMGSDI_TELECOM_CC},
  {UIMDIAG_TELECOM_PUID,MMGSDI_TELECOM_PUID},
  {UIMDIAG_TELECOM_MML,MMGSDI_TELECOM_MML},
  {UIMDIAG_TELECOM_MMDF,MMGSDI_TELECOM_MMDF},
  {UIMDIAG_TELECOM_MLPL,MMGSDI_TELECOM_MLPL},
  {UIMDIAG_TELECOM_MSPL,MMGSDI_TELECOM_MSPL},
  {UIMDIAG_TELECOM_MMSSMODE,MMGSDI_TELECOM_MMSSMODE},
  {UIMDIAG_USIM_ACM_MAX,MMGSDI_USIM_ACM_MAX},
  {UIMDIAG_USIM_ACM,MMGSDI_USIM_ACM},
  {UIMDIAG_USIM_PUCT,MMGSDI_USIM_PUCT},
  {UIMDIAG_USIM_CBMI,MMGSDI_USIM_CBMI},
  {UIMDIAG_USIM_CBMID,MMGSDI_USIM_CBMID},
  {UIMDIAG_USIM_CBMIR,MMGSDI_USIM_CBMIR},
  {UIMDIAG_USIM_EXT2,MMGSDI_USIM_EXT2},
  {UIMDIAG_USIM_EXT3,MMGSDI_USIM_EXT3},
  {UIMDIAG_USIM_SMSR,MMGSDI_USIM_SMSR},
  {UIMDIAG_USIM_OCI,MMGSDI_USIM_OCI},
  {UIMDIAG_USIM_ICT,MMGSDI_USIM_ICT},
  {UIMDIAG_USIM_OCT,MMGSDI_USIM_OCT},
  {UIMDIAG_USIM_EXT5,MMGSDI_USIM_EXT5},
  {UIMDIAG_USIM_CCP2,MMGSDI_USIM_CCP2},
  {UIMDIAG_USIM_EMLPP,MMGSDI_USIM_EMLPP},
  {UIMDIAG_USIM_AAEM,MMGSDI_USIM_AAEM},
  {UIMDIAG_USIM_GMSI,MMGSDI_USIM_GMSI},
  {UIMDIAG_USIM_EXT4,MMGSDI_USIM_EXT4},
  {UIMDIAG_USIM_CMI,MMGSDI_USIM_CMI},
  {UIMDIAG_USIM_ACL,MMGSDI_USIM_ACL},
  {UIMDIAG_USIM_DCK,MMGSDI_USIM_DCK},
  {UIMDIAG_USIM_CNL,MMGSDI_USIM_CNL},
  {UIMDIAG_USIM_OPLMNSEL,MMGSDI_USIM_OPLMNSEL},
  {UIMDIAG_USIM_ARR,MMGSDI_USIM_ARR},
  {UIMDIAG_USIM_RPLMNACT,MMGSDI_USIM_RPLMNACT},
  {UIMDIAG_USIM_MBDN,MMGSDI_USIM_MBDN},
  {UIMDIAG_USIM_EXT6,MMGSDI_USIM_EXT6},
  {UIMDIAG_USIM_MBI,MMGSDI_USIM_MBI},
  {UIMDIAG_USIM_MWIS,MMGSDI_USIM_MWIS},
  {UIMDIAG_USIM_EHPLMN,MMGSDI_USIM_EHPLMN},
  {UIMDIAG_USIM_CFIS,MMGSDI_USIM_CFIS},
  {UIMDIAG_USIM_VGCSCA,MMGSDI_USIM_VGCSCA},
  {UIMDIAG_USIM_VBSCA,  MMGSDI_USIM_VBSCA},
  {UIMDIAG_USIM_GBABP,MMGSDI_USIM_GBABP},
  {UIMDIAG_USIM_GBANL,MMGSDI_USIM_GBANL},
  {UIMDIAG_USIM_MSK,MMGSDI_USIM_MSK},
  {UIMDIAG_USIM_MUK,MMGSDI_USIM_MUK},
  {UIMDIAG_USIM_VMWI,MMGSDI_USIM_VMWI},
  {UIMDIAG_USIM_SVC_STR_TBL,MMGSDI_USIM_SVC_STR_TBL},
  {UIMDIAG_USIM_CFF,MMGSDI_USIM_CFF},
  {UIMDIAG_USIM_ONS,MMGSDI_USIM_ONS},
  {UIMDIAG_USIM_CSP,MMGSDI_USIM_CSP},
  {UIMDIAG_USIM_CPHSI,MMGSDI_USIM_CPHSI},
  {UIMDIAG_USIM_MN,MMGSDI_USIM_MN},
  {UIMDIAG_USIM_PBR,MMGSDI_USIM_PBR},
  {UIMDIAG_USIM_PSC,MMGSDI_USIM_PSC},
  {UIMDIAG_USIM_CC,MMGSDI_USIM_CC},
  {UIMDIAG_USIM_PUID,MMGSDI_USIM_PUID},
  {UIMDIAG_USIM_CPBCCH,MMGSDI_USIM_CPBCCH},
  {UIMDIAG_USIM_INVSCAN,MMGSDI_USIM_INVSCAN},
  {UIMDIAG_USIM_MEXE_ST,MMGSDI_USIM_MEXE_ST},
  {UIMDIAG_USIM_ORPK,MMGSDI_USIM_ORPK},
  {UIMDIAG_USIM_ARPK,MMGSDI_USIM_ARPK},
  {UIMDIAG_USIM_TPRPK,MMGSDI_USIM_TPRPK},
  {UIMDIAG_USIM_MMSN,MMGSDI_USIM_MMSN},
  {UIMDIAG_USIM_MMSICP,MMGSDI_USIM_MMSICP},
  {UIMDIAG_USIM_MMSUP,MMGSDI_USIM_MMSUP},
  {UIMDIAG_USIM_MMSUCP,MMGSDI_USIM_MMSUCP},
  {UIMDIAG_USIM_EXT8,MMGSDI_USIM_EXT8},
  {UIMDIAG_USIM_NCP_IP,MMGSDI_USIM_NCP_IP},
  {UIMDIAG_USIM_PSEUDO,MMGSDI_USIM_PSEUDO},
  {UIMDIAG_USIM_UPLMNWLAN,MMGSDI_USIM_UPLMNWLAN},
  {UIMDIAG_USIM_0PLMNWLAN,MMGSDI_USIM_0PLMNWLAN},
  {UIMDIAG_USIM_UWSIDL,MMGSDI_USIM_UWSIDL},
  {UIMDIAG_USIM_OWSIDL,MMGSDI_USIM_OWSIDL},
  {UIMDIAG_USIM_WRI,MMGSDI_USIM_WRI},
  {UIMDIAG_USIM_HWSIDL,MMGSDI_USIM_HWSIDL},
  {UIMDIAG_USIM_WEHPLMNPI,MMGSDI_USIM_WEHPLMNPI},
  {UIMDIAG_USIM_WHPI,MMGSDI_USIM_WHPI},
  {UIMDIAG_USIM_WLRPLMN,MMGSDI_USIM_WLRPLMN},
  {UIMDIAG_USIM_HPLMNDAI,MMGSDI_USIM_HPLMNDAI},
  {UIMDIAG_USIM_ACSGL,MMGSDI_USIM_ACSGL},
  {UIMDIAG_USIM_CSGT,MMGSDI_USIM_CSGT},
  {UIMDIAG_USIM_HNBN,MMGSDI_USIM_HNBN},
  {UIMDIAG_USIM_EHPLMNPI,MMGSDI_USIM_EHPLMNPI},
  {UIMDIAG_USIM_LRPLMNSI,  MMGSDI_USIM_LRPLMNSI},
  {UIMDIAG_PKCS15_ODF,MMGSDI_PKCS15_ODF},
  {UIMDIAG_PKCS15_PRKDF,MMGSDI_PKCS15_PRKDF},
  {UIMDIAG_PKCS15_PUKDF,MMGSDI_PKCS15_PUKDF},
  {UIMDIAG_PKCS15_SKDF,MMGSDI_PKCS15_SKDF},
  {UIMDIAG_PKCS15_CDF,MMGSDI_PKCS15_CDF},
  {UIMDIAG_PKCS15_DODF,MMGSDI_PKCS15_DODF},
  {UIMDIAG_PKCS15_AODF,MMGSDI_PKCS15_AODF},
  {UIMDIAG_PKCS15_TI,MMGSDI_PKCS15_TI},
  {UIMDIAG_PKCS15_US,  MMGSDI_PKCS15_US},
  {UIMDIAG_DCS1800_IMSI,MMGSDI_DCS1800_IMSI},
  {UIMDIAG_DCS1800_KC,MMGSDI_DCS1800_KC},
  {UIMDIAG_DCS1800_PLMN,MMGSDI_DCS1800_PLMN},
  {UIMDIAG_DCS1800_SST,MMGSDI_DCS1800_SST},
  {UIMDIAG_DCS1800_BCCH,MMGSDI_DCS1800_BCCH},
  {UIMDIAG_DCS1800_ACC,MMGSDI_DCS1800_ACC},
  {UIMDIAG_DCS1800_FPLMN,MMGSDI_DCS1800_FPLMN},
  {UIMDIAG_DCS1800_LOCI,MMGSDI_DCS1800_LOCI},
  {UIMDIAG_DCS1800_AD,  MMGSDI_DCS1800_AD},
  {UIMDIAG_SIM_7F40_PROP1_DFS,MMGSDI_SIM_7F40_PROP1_DFS},
  {UIMDIAG_SIM_7F40_PROP1_D2FS,MMGSDI_SIM_7F40_PROP1_D2FS},
  {UIMDIAG_SIM_7F40_PROP1_CSP2,MMGSDI_SIM_7F40_PROP1_CSP2},
  {UIMDIAG_SIM_7F40_PROP1_PARAMS,MMGSDI_SIM_7F40_PROP1_PARAMS},
  {UIMDIAG_USIM_7F40_PROP1_DFS,MMGSDI_USIM_7F40_PROP1_DFS},
  {UIMDIAG_USIM_7F40_PROP1_D2FS,MMGSDI_USIM_7F40_PROP1_D2FS},
  {UIMDIAG_USIM_7F40_PROP1_CSP2,MMGSDI_USIM_7F40_PROP1_CSP2},
  {UIMDIAG_USIM_7F40_PROP1_PARAMS, MMGSDI_USIM_7F40_PROP1_PARAMS},
  {UIMDIAG_SIM_7F43_PROP1_HZ,MMGSDI_SIM_7F43_PROP1_HZ},
  {UIMDIAG_SIM_7F43_PROP1_CACHE1,MMGSDI_SIM_7F43_PROP1_CACHE1},
  {UIMDIAG_SIM_7F43_PROP1_CACHE2,MMGSDI_SIM_7F43_PROP1_CACHE2},
  {UIMDIAG_SIM_7F43_PROP1_CACHE3,MMGSDI_SIM_7F43_PROP1_CACHE3},
  {UIMDIAG_SIM_7F43_PROP1_CACHE4,MMGSDI_SIM_7F43_PROP1_CACHE4},
  {UIMDIAG_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_1,MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_1},
  {UIMDIAG_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_2,MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_2},
  {UIMDIAG_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_3,MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_3},
  {UIMDIAG_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_4,MMGSDI_SIM_7F43_PROP1_SUBSCRIBED_LAC_CI_4},
  {UIMDIAG_SIM_7F43_PROP1_TAGS,MMGSDI_SIM_7F43_PROP1_TAGS},
  {UIMDIAG_SIM_7F43_PROP1_SETTINGS,  MMGSDI_SIM_7F43_PROP1_SETTINGS},
  {UIMDIAG_CSIM_SSCI,MMGSDI_CSIM_SSCI},
  {UIMDIAG_CSIM_UNASSIGNED_1,MMGSDI_CSIM_UNASSIGNED_1},
  {UIMDIAG_CSIM_SMS_STATUS,MMGSDI_CSIM_SMS_STATUS},
  {UIMDIAG_CSIM_SSFC,MMGSDI_CSIM_SSFC},
  {UIMDIAG_CSIM_3GPD_ME3GPDOPC,MMGSDI_CSIM_3GPD_ME3GPDOPC},
  {UIMDIAG_CSIM_3GPD_3GPDOPM,MMGSDI_CSIM_3GPD_3GPDOPM},
  {UIMDIAG_CSIM_3GPD_SIPCAP,MMGSDI_CSIM_3GPD_SIPCAP},
  {UIMDIAG_CSIM_3GPD_MIPCAP,MMGSDI_CSIM_3GPD_MIPCAP},
  {UIMDIAG_CSIM_3GPD_SIPUPP,MMGSDI_CSIM_3GPD_SIPUPP},
  {UIMDIAG_CSIM_3GPD_MIPUPP,MMGSDI_CSIM_3GPD_MIPUPP},
  {UIMDIAG_CSIM_3GPD_SIPSP,MMGSDI_CSIM_3GPD_SIPSP},
  {UIMDIAG_CSIM_3GPD_MIPSP,MMGSDI_CSIM_3GPD_MIPSP},
  {UIMDIAG_CSIM_3GPD_SIPPAPSS,MMGSDI_CSIM_3GPD_SIPPAPSS},
  {UIMDIAG_CSIM_UNASSIGNED_2,MMGSDI_CSIM_UNASSIGNED_2},
  {UIMDIAG_CSIM_UNASSIGNED_3,MMGSDI_CSIM_UNASSIGNED_3},
  {UIMDIAG_CSIM_PUZL,MMGSDI_CSIM_PUZL},
  {UIMDIAG_CSIM_MAXPUZL,MMGSDI_CSIM_MAXPUZL},
  {UIMDIAG_CSIM_HRPDCAP,MMGSDI_CSIM_HRPDCAP},
  {UIMDIAG_CSIM_HRPDUPP,MMGSDI_CSIM_HRPDUPP},
  {UIMDIAG_CSIM_CSSPR,MMGSDI_CSIM_CSSPR},
  {UIMDIAG_CSIM_ATC,MMGSDI_CSIM_ATC},
  {UIMDIAG_CSIM_BCSMS_CONFIG,MMGSDI_CSIM_BCSMS_CONFIG},
  {UIMDIAG_CSIM_BCSMS_PREF,MMGSDI_CSIM_BCSMS_PREF},
  {UIMDIAG_CSIM_BCSMS_TABLE,MMGSDI_CSIM_BCSMS_TABLE},
  {UIMDIAG_CSIM_BCSMS_PARAMS,MMGSDI_CSIM_BCSMS_PARAMS},
  {UIMDIAG_CSIM_BAKPARA,MMGSDI_CSIM_BAKPARA},
  {UIMDIAG_CSIM_UPBAKPARA,MMGSDI_CSIM_UPBAKPARA},
  {UIMDIAG_CSIM_MMSN,MMGSDI_CSIM_MMSN},
  {UIMDIAG_CSIM_MMS_EXT8,MMGSDI_CSIM_MMS_EXT8},
  {UIMDIAG_CSIM_MMS_ICP,MMGSDI_CSIM_MMS_ICP},
  {UIMDIAG_CSIM_MMS_UP,MMGSDI_CSIM_MMS_UP},
  {UIMDIAG_CSIM_MMS_UCP,MMGSDI_CSIM_MMS_UCP},
  {UIMDIAG_CSIM_AUTH_CAP,MMGSDI_CSIM_AUTH_CAP},
  {UIMDIAG_CSIM_3GCIK,MMGSDI_CSIM_3GCIK},
  {UIMDIAG_CSIM_DCK,MMGSDI_CSIM_DCK},
  {UIMDIAG_CSIM_GID1,MMGSDI_CSIM_GID1},
  {UIMDIAG_CSIM_GID2,MMGSDI_CSIM_GID2},
  {UIMDIAG_CSIM_CNL,MMGSDI_CSIM_CNL},
  {UIMDIAG_CSIM_EST,MMGSDI_CSIM_EST},
  {UIMDIAG_CSIM_HIDDEN_KEY,MMGSDI_CSIM_HIDDEN_KEY},
  {UIMDIAG_CSIM_LCS_VERSION,MMGSDI_CSIM_LCS_VERSION},
  {UIMDIAG_CSIM_LCS_CP,MMGSDI_CSIM_LCS_CP},
  {UIMDIAG_CSIM_SDN,MMGSDI_CSIM_SDN},
  {UIMDIAG_CSIM_EXT2,MMGSDI_CSIM_EXT2},
  {UIMDIAG_CSIM_EXT3,MMGSDI_CSIM_EXT3},
  {UIMDIAG_CSIM_ICI,MMGSDI_CSIM_ICI},
  {UIMDIAG_CSIM_OCI,MMGSDI_CSIM_OCI},
  {UIMDIAG_CSIM_EXT5,MMGSDI_CSIM_EXT5},
  {UIMDIAG_CSIM_CCP2,MMGSDI_CSIM_CCP2},
  {UIMDIAG_CSIM_APP_LABELS,MMGSDI_CSIM_APP_LABELS},
  {UIMDIAG_CSIM_DEV_MODEL,MMGSDI_CSIM_DEV_MODEL},
  {UIMDIAG_CSIM_SMS_CAP,MMGSDI_CSIM_SMS_CAP},
  {UIMDIAG_CSIM_3GPD_MIPFLAGS,MMGSDI_CSIM_3GPD_MIPFLAGS},
  {UIMDIAG_CSIM_3GPD_SIPUPPEXT,MMGSDI_CSIM_3GPD_SIPUPPEXT},
  {UIMDIAG_CSIM_3GPD_MIPUPPEXT,MMGSDI_CSIM_3GPD_MIPUPPEXT},
  {UIMDIAG_CSIM_3GPD_IPV6CAP,MMGSDI_CSIM_3GPD_IPV6CAP},
  {UIMDIAG_CSIM_3GPD_TCPCONFIG,MMGSDI_CSIM_3GPD_TCPCONFIG},
  {UIMDIAG_CSIM_3GPD_DGC,MMGSDI_CSIM_3GPD_DGC},
  {UIMDIAG_CSIM_BROWSER_CP,MMGSDI_CSIM_BROWSER_CP},
  {UIMDIAG_CSIM_BROWSER_BM,MMGSDI_CSIM_BROWSER_BM},
  {UIMDIAG_CSIM_MMS_CONFIG,MMGSDI_CSIM_MMS_CONFIG},
  {UIMDIAG_CSIM_JDL,MMGSDI_CSIM_JDL},
  {UIMDIAG_CSIM_PBR,MMGSDI_CSIM_PBR},
  {UIMDIAG_CSIM_PSC,MMGSDI_CSIM_PSC},
  {UIMDIAG_CSIM_CHANGE_COUNTER,MMGSDI_CSIM_CHANGE_COUNTER},
  {UIMDIAG_CSIM_PUID,MMGSDI_CSIM_PUID},
  {UIMDIAG_CSIM_LBS_XCONFIG,MMGSDI_CSIM_LBS_XCONFIG},
  {UIMDIAG_CSIM_LBS_XSURL,MMGSDI_CSIM_LBS_XSURL},
  {UIMDIAG_CSIM_LBS_V2CONFIG,MMGSDI_CSIM_LBS_V2CONFIG},
  {UIMDIAG_CSIM_LBS_V2PDEADDR,MMGSDI_CSIM_LBS_V2PDEADDR},
  {UIMDIAG_CSIM_LBS_V2MPCADDR,MMGSDI_CSIM_LBS_V2MPCADDR},
  {UIMDIAG_CSIM_BREW_DLOAD,MMGSDI_CSIM_BREW_DLOAD},
  {UIMDIAG_CSIM_BREW_TSID,MMGSDI_CSIM_BREW_TSID},
  {UIMDIAG_CSIM_BREW_SID,MMGSDI_CSIM_BREW_SID},
  {UIMDIAG_CSIM_BREW_AEP,MMGSDI_CSIM_BREW_AEP},
  {UIMDIAG_CSIM_EXT_ME_SETTING_DATA,MMGSDI_CSIM_EXT_ME_SETTING_DATA},
  {UIMDIAG_CSIM_USER_AGENT_STRING,MMGSDI_CSIM_USER_AGENT_STRING},
  {UIMDIAG_CSIM_IMSI_STATUS,MMGSDI_CSIM_IMSI_STATUS},
  {UIMDIAG_CSIM_ME_DOWNLOADABLE_DATA,MMGSDI_CSIM_ME_DOWNLOADABLE_DATA},
  {UIMDIAG_CSIM_ME_SETTING_DATA,MMGSDI_CSIM_ME_SETTING_DATA},
  {UIMDIAG_CSIM_ME_USER_DATA,MMGSDI_CSIM_ME_USER_DATA},
  {UIMDIAG_CSIM_UIM_SVC_TABLE,MMGSDI_CSIM_UIM_SVC_TABLE},
  {UIMDIAG_CSIM_UIM_FEATURES,MMGSDI_CSIM_UIM_FEATURES},
  {UIMDIAG_CSIM_UIM_VERSION,MMGSDI_CSIM_UIM_VERSION},
  {UIMDIAG_CSIM_3GPD_UPPEXT,MMGSDI_CSIM_3GPD_UPPEXT},
  {UIMDIAG_GSM_ONS_SHORT,MMGSDI_GSM_ONS_SHORT},
  {UIMDIAG_USIM_ONS_SHORT,MMGSDI_USIM_ONS_SHORT},
  {UIMDIAG_TELECOM_ICE_GRAPHICS,MMGSDI_TELECOM_ICE_GRAPHICS},
  {UIMDIAG_NO_FILE_ENUM,MMGSDI_NO_FILE_ENUM},
  {UIMDIAG_MAX_FILE_ENUM,MMGSDI_MAX_FILE_ENUM}
*/
};

static const void* uimdiag_req_ptr;
static uint32 uimdiag_req_len;
static uint32 uimdiag_offset;
static uimdiag_return_enum_type uimdiag_decode_status;

/*=============================================================================

                              FUNCTIONS

=============================================================================*/

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_init_decode_params

DESCRIPTION
  This function should be called before any structure is decoded.

  This function copies diag_req_ptr,diag_req_len to static variables
  and initializes the offset and uimdiag_decode_status.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static void uimdiag_mmgsdi_util_init_decode_params(
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{
  uimdiag_req_ptr = diag_req_ptr;
  uimdiag_req_len = diag_req_len;
  uimdiag_decode_status = UIMDIAG_SUCCESS;
  uimdiag_offset = 0;
} /* uimdiag_mmgsdi_util_decode_uint8 */


/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_decode_uint8

DESCRIPTION
  This function copies uint8 from diag_req_ptr to cmd_ptr,
  from offset value.
  Before copy, it checks diag_req_len, to validate the data in diag_req_ptr.
  The offset is updated after the copy.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

static void uimdiag_mmgsdi_util_decode_uint8(void *cmd_ptr)
{
  if (uimdiag_decode_status != UIMDIAG_SUCCESS)
  {
    return;
  }
  if(uimdiag_offset + sizeof(uint8) > uimdiag_req_len)
  {
    UIMDIAG_MSG_ERROR_0("uimdiag_mmgsdi_util_decode_uint8- Cmd error");
    uimdiag_decode_status = UIMDIAG_ERROR;
    return;
  }
  memscpy(cmd_ptr,
          sizeof(uint8),
          (void *)((uint8*)uimdiag_req_ptr + uimdiag_offset),
          sizeof(uint8));

  uimdiag_offset  += sizeof(uint8);
} /* uimdiag_mmgsdi_util_decode_uint8 */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_decode_uint32

DESCRIPTION
  This function copies uint32 from diag_req_ptr to cmd_ptr,
  from offset value.
  Before copy, it checks diag_req_len, to validate the data in diag_req_ptr.
  The offset is updated after the copy.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static void uimdiag_mmgsdi_util_decode_uint32(void *cmd_ptr)
{
    if (uimdiag_decode_status != UIMDIAG_SUCCESS)
    {
      return;
    }
    if(uimdiag_offset + sizeof(uint32) > uimdiag_req_len)
    {
      UIMDIAG_MSG_ERROR_0("uimdiag_mmgsdi_util_decode_uint32- Cmd error");
      uimdiag_decode_status = UIMDIAG_ERROR;
      return;
    }
    memscpy(cmd_ptr,
          sizeof(uint32),
          (void *)((uint8*)uimdiag_req_ptr + uimdiag_offset),
          sizeof(uint32));

    uimdiag_offset  += sizeof(uint32);
}/* uimdiag_mmgsdi_util_decode_uint32 */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_decode_len_val_uint8

DESCRIPTION
  This function dynamically allocates 'length' number of bytes in the cmd_ptr.
  It copes the 'length' bytes from diag_req_ptr to the allocated cmd_ptr,
  from offset value.
  Before copy, it checks diag_req_len, to validate the data in diag_req_ptr.
  The offset is updated after the copy.

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type, which reflects status of allocation.

SIDE EFFECTS
  None.
===========================================================================*/

static void
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    uint8                                             ** cmd_val_ptr,
    uint32                                               length)
{
  if (uimdiag_decode_status != UIMDIAG_SUCCESS)
  {
    return;
  }
  if(uimdiag_offset + length > uimdiag_req_len)
  {
    UIMDIAG_MSG_ERROR_0("uimdiag_mmgsdi_util_decode_len_val_uint8- Cmd error");
    uimdiag_decode_status = UIMDIAG_ERROR;
    return;
  }
  if(length > 0)
  {
    UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
        *cmd_val_ptr,
        length);
    if(*cmd_val_ptr == NULL)
    {
      uimdiag_decode_status = UIMDIAG_MEMORY_ERROR_HEAP_EXHAUSTED;
      return;
    }
    memscpy(*cmd_val_ptr,
      (length* sizeof(uint8)),
      (void *)((uint8*)uimdiag_req_ptr + uimdiag_offset),
      (length* sizeof(uint8)));
  }

  if(length == 0)
  {
    *cmd_val_ptr = NULL;
  }

  uimdiag_offset  += (length)*sizeof(uint8);
}

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_decode_len_val_uint16

DESCRIPTION
  This function dynamically allocates 'length' number of bytes in the cmd_ptr.
  It copes the 'length' bytes from diag_req_ptr to the allocated cmd_ptr,
  from offset value.
  Before copy, it checks diag_req_len, to validate the data in diag_req_ptr.
  The offset is updated after the copy.

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type, which reflects status of allocation.

SIDE EFFECTS
  None.
===========================================================================*/

static void
  uimdiag_mmgsdi_util_decode_len_val_uint16(
    uint8                                             ** cmd_val_ptr,
    uint32                                               length)
{
  if (uimdiag_decode_status != UIMDIAG_SUCCESS)
  {
    return;
  }
  if(uimdiag_offset + length > uimdiag_req_len)
  {
    UIMDIAG_MSG_ERROR_0("uimdiag_mmgsdi_util_decode_len_val_uint8- Cmd error");
    uimdiag_decode_status = UIMDIAG_ERROR;
    return;
  }
  if(length > 0)
  {
    UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
        *cmd_val_ptr,
        (length*sizeof(uint16)));
    if(*cmd_val_ptr == NULL)
    {
      uimdiag_decode_status = UIMDIAG_MEMORY_ERROR_HEAP_EXHAUSTED;
      return;
    }
    memscpy(*cmd_val_ptr,
      (length* sizeof(uint16)),
      (void *)((uint8*)uimdiag_req_ptr + uimdiag_offset),
      (length* sizeof(uint16)));
  }

  if(length == 0)
  {
    *cmd_val_ptr = NULL;
  }

  uimdiag_offset  += (length)*sizeof(uint16);
}

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_OPEN_CMD

DESCRIPTION
  This function parses cmd_ptr and fills the session open command params

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_open_cmd(
  uimdiag_mmgsdi_session_open_cmd_type         * cmd_ptr,
  const void                                   * diag_req_ptr,
  uint32                                       diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1. Client Id Lower/Higher 32 bits
  2. Session Type
  3. Set Notify
  4. Exclusive Channel
  5. AID Data Len
  6. AID Data
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* offset updated to access the session type */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_type);

  /* offset updated to access exclusive channel information */

  uimdiag_mmgsdi_util_decode_uint32((void*)
    &cmd_ptr->exclusive_channel);

  /* offset updated to access the set notify field */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->set_notify);

  /* offset updated to get AID length */

  uimdiag_mmgsdi_util_decode_uint32((void*)
    &cmd_ptr->aid_data_len);

  /* offset updated to access the AID Data Pointer */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
     (uint8 **)&(cmd_ptr->aid_data_ptr),
           cmd_ptr->aid_data_len);

  /* prov session will have len as zero */
  if(cmd_ptr->aid_data_len == 0)
  {
     UIMDIAG_MSG_HIGH_0("Session open for Prov App, 0, 0");
  }

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_open_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_ID

DESCRIPTION
  This function parses the cmd_ptr and fills the session id params
  Functions which are using this are :
  1. session_close
  2. session_act_or_sw_prov
  3. session_get_all_pin_status
  4. uimdiag_mmgsdi_util_populate_session_deact_prov_cmd,
  5. uimdiag_mmgsdi_util_populate_session_get_app_cap_cmd,
  6. uimdiag_mmgsdi_util_populate_session_get_cphs_info_cmd,
  7. uimdiag_mmgsdi_util_populate_session_create_pkcs15_lookup_table_cmd

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_id(
  uimdiag_mmgsdi_sessionid_type           * cmd_ptr,
  const void                              * diag_req_ptr,
  uint32                                    diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to end of cmd_type */
  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_id */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_NVRUIM_DATA_SYNC

DESCRIPTION
  This function parses the cmd_ptr and fills the session id
  and NVRUIM data request params

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_nvruim_data_sync(
  uimdiag_mmgsdi_nvruim_data_req_type     * cmd_ptr,
  const void                              * diag_req_ptr,
  uint32                                    diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* NVRUIM data request type */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->nvruim_data_req_enum);

  /* offset updated to end of cmd_type */
  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_nvruim_data_sync */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_CLIENT_ID_AND_EVT_DEREG_CMD

DESCRIPTION
  This function parses the cmd_ptr and fills the client id and evt dereg command params

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_client_id_and_evt_dereg_cmd(
  uimdiag_mmgsdi_client_id_and_evt_dereg_cmd_type   * cmd_ptr,
  const void                              * diag_req_ptr,
  uint32                                    diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Client Id Lower/Higher 32 bits
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID lower 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* offset updated to end of client_id_and_evt_dereg_cmd_type */

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_client_id_and_evt_dereg_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_ACT_OR_SW_PROV_CMD

DESCRIPTION
  This function parses the cmd_ptr and fills the session activate or switch
  command params

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_act_or_sw_prov_cmd(
  uimdiag_mmgsdi_session_act_or_sw_prov_cmd_type              * cmd_ptr,
  const void                                                  * diag_req_ptr,
  uint32                                                        diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2. slot Id
  3. AID Data Length
  4. AID Data Pointer
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access the slot id */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->slot_id);

  /* offset updated to access AID Data length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->data_len);

  /* offset updated to point to AID Data Ptr */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
     (uint8 **)&(cmd_ptr->data_ptr),
         cmd_ptr->data_len);
  if(cmd_ptr->data_len == 0)
  {
    cmd_ptr->data_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("Session Activate and Switch Prov, Data Len is 0");
    return UIMDIAG_ERROR;
  }

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_act_or_sw_prov_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_GET_INFO_CMD

DESCRIPTION
  This function parses the cmd_ptr and allows a client to query MMGSDI for
  session information by Session ID or Session Type. A client can get
  Session Type, Slot Number, and AID, if querying by Session ID;
  or Session ID, Slot Number and AID, if querying by Session Type.

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_get_info_cmd(
  uimdiag_mmgsdi_session_get_info_cmd_type   * cmd_ptr,
  const void                                 * diag_req_ptr,
  uint32                                       diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1. Session Id Lower/Higher 32 bits
  2. Session Type
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* Query type */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_info);

  /* Session type */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_type);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_get_info_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_VERIFY_PIN_CMD

DESCRIPTION
  This command is used to perform a PIN verification procedure against a card
  for PIN1, PIN2 or UPIN.

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_verify_pin_cmd(
  uimdiag_mmgsdi_session_verify_pin_cmd_type           * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2. Pin ID
  3. Pin Value
  4. Pin Value Ptr
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access Pin ID */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pin_id);

  /* offset updated to access length of PIN_VALUE */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pin_len);

  /* offset updated to point to PIN VALUE Ptr */
  if(cmd_ptr->pin_len == 0)
  {
    cmd_ptr->pin_value_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("Session verify pin, Pin Len is 0");
    return UIMDIAG_ERROR;
  }
  uimdiag_mmgsdi_util_decode_len_val_uint8(
     (uint8 **)&(cmd_ptr->pin_value_ptr),
      cmd_ptr->pin_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_verify_pin_status_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_DISABLE_PIN_CMD

DESCRIPTION
  This command is used to build a request to MMGSDI to disable the PIN provided

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_disable_pin_cmd(
  uimdiag_mmgsdi_session_disable_pin_cmd_type           * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
  {

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2. Pin ID
  3. Replace PIN
  4. Pin Value
  5. Pin Value Ptr
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access Pin ID */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pin_id);

  /* offset updated to access REPLACE_PIN */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pin_replace);

  /* offset updated to access length of PIN_VALUE */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pin_len);

  /* offset updated to point to PIN VALUE Ptr */

  if(cmd_ptr->pin_len == 0)
  {
    cmd_ptr->pin_value_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("Session disable pin, Pin length is 0");
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
     (uint8 **)&(cmd_ptr->pin_value_ptr),
      cmd_ptr->pin_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_disable_pin_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_ENABLE_PIN_CMD

DESCRIPTION
  This command is used to build a request to MMGSDI to enable the PIN provided

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_enable_pin_cmd(
  uimdiag_mmgsdi_session_enable_pin_cmd_type           * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
  {

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2. Pin Id
  3. Pin Value
  4. Pin Value Ptr
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access Pin ID */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pin_id);

  /* offset updated to access length of PIN_VALUE */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pin_len);

  /* offset updated to point to PIN VALUE Ptr */
  if(cmd_ptr->pin_len == 0)
  {
    cmd_ptr->pin_value_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("Session Enable pin, Pin length is 0");
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->pin_value_ptr),
     cmd_ptr->pin_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_enable_pin_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_CHANGE_PIN_CMD

DESCRIPTION
  This command is used to build a request to MMGSDI to change the PIN provided

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_change_pin_cmd(
  uimdiag_mmgsdi_session_change_pin_cmd_type           * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
  {

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1. Session Id Lower/Higher 32 bits
  2. Pin ID
  3. Pin Value
  4. Pin Value Ptr
  5. New Pin Value
  6. New Pin Value Ptr
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access Pin ID */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pin_id);

  /* offset updated to access length of PIN_VALUE */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pin_len);

  /* offset updated to point to PIN VALUE Ptr */
  if(cmd_ptr->pin_len == 0)
  {
    cmd_ptr->pin_value_ptr = NULL;
    return UIMDIAG_ERROR;
  }
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->pin_value_ptr),
    cmd_ptr->pin_len);

  /* offset updated to access length of NEW_PIN_VALUE */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->new_pin_len);

  /* offset updated to point to NEW_PIN_VALUE Ptr */
  if(cmd_ptr->new_pin_len == 0)
  {
    cmd_ptr->new_pin_value_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("Session Change pin, Pin length is 0");
    return UIMDIAG_ERROR;
  }
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->new_pin_value_ptr),
     cmd_ptr->new_pin_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_change_pin_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_UNBLOCK_PIN_CMD

DESCRIPTION
  This command is used to build a request to MMGSDI to unblock the PIN provided

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_unblock_pin_cmd(
  uimdiag_mmgsdi_session_unblock_pin_cmd_type           * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
  {

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1. Session Id Lower/Higher 32 bits
  2. Pin Id
  3. Puk Value
  4. Puk Value Ptr
  5. New Pin Value
  6. New Pin Value Ptr
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access Pin ID */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pin_id);

  /* offset updated to access length of PUK_VALUE */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->puk_len);

  /* offset updated to point to PUK VALUE Ptr */
  if(cmd_ptr->puk_len == 0)
  {
    cmd_ptr->puk_value_ptr = NULL;
    return UIMDIAG_ERROR;
  }
  uimdiag_mmgsdi_util_decode_len_val_uint8(
       (uint8 **)&(cmd_ptr->puk_value_ptr),
        cmd_ptr->puk_len);

  /* offset updated to access length of NEW_PIN_VALUE */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->new_pin_len);

  /* offset updated to point to NEW_PIN_VALUE Ptr */
  if(cmd_ptr->new_pin_len == 0)
  {
    cmd_ptr->new_pin_value_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("Session Unblock pin, Pin length is 0");
    return UIMDIAG_ERROR;
  }
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->new_pin_value_ptr),
    cmd_ptr->new_pin_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_unblock_pin_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_GET_FILE_ATTR_CMD

DESCRIPTION
  This command is used to build a request to MMGSDI to get file attr

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_get_file_attr_cmd(
  uimdiag_mmgsdi_session_get_file_attr_cmd_type          * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
  {

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.Access method
  3.ef enum
  4.df enum
  5.path len and path
  6.aid len and aid
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access "access_method" */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->access_method);

  /* offset updated to access ef enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->ef_enum);

  /* offset updated to access df enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->df_enum);

  /* offset updated to get path length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->path_len);

  /* offset updated to access the path */
  uimdiag_mmgsdi_util_decode_len_val_uint16(
    (uint8 **)&(cmd_ptr->path_ptr),
           cmd_ptr->path_len);

  /* aid_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->aid_len);

  /* offset updated to access the AID Data Pointer */
  if(cmd_ptr->aid_len == 0)
  {
    cmd_ptr->aid_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("Session Get File Attr, AID length is 0");
    return UIMDIAG_ERROR;
  }
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->aid_ptr),
    cmd_ptr->aid_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_get_file_attr_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_READ_RECORD_CMD

DESCRIPTION
  This command is used to build a request to MMGSDI to read record

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_read_record_cmd(
  uimdiag_mmgsdi_session_read_record_cmd_type          * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
  {

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.Access method
  3.ef enum
  4.df enum
  5.path len and path
  6.aid len and aid
  7.record number
  8.record length
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access "access_method" */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->access_method);

  /* offset updated to access ef enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->ef_enum);

  /* offset updated to access df enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->df_enum);

  /* offset updated to get path length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->path_len);

  /* offset updated to access the path */
  uimdiag_mmgsdi_util_decode_len_val_uint16(
    (uint8 **)&(cmd_ptr->path_ptr),
      cmd_ptr->path_len);

  /* aid_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->aid_len);

  if(cmd_ptr->aid_len == 0)
  {
    cmd_ptr->aid_ptr = NULL;
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->aid_ptr),
    cmd_ptr->aid_len);

  /* rec_num */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->rec_num);

  /* offset updated to get record length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->rec_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_read_record_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_READ_TRANSPARENT_CMD

DESCRIPTION
  This command is used to build a request to MMGSDI to read transparent file

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_read_transparent_cmd(
  uimdiag_mmgsdi_session_read_transparent_cmd_type          * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
  {

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.Access method
  3.ef enum
  4.df enum
  5.path len and path
  6.aid len and aid
  7.offset
  8.record length
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access "access_method" */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->access_method);

  /* offset updated to access ef enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->ef_enum);

  /* offset updated to access df enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->df_enum);

  /* offset updated to get path length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->path_len);

  /* offset updated to access the path */
  uimdiag_mmgsdi_util_decode_len_val_uint16(
    (uint8 **)&(cmd_ptr->path_ptr),
     cmd_ptr->path_len);

  /* aid_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->aid_len);

  /* offset updated to access the AID Data Pointer */
  if(cmd_ptr->aid_len == 0)
  {
    cmd_ptr->aid_ptr = NULL;
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->aid_ptr),
    cmd_ptr->aid_len);

  /* offset */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->offset);

  /* offset updated to get record length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->req_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_read_transparent_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_WRITE_RECORD_CMD

DESCRIPTION
  This command is used to build a request to MMGSDI to write record

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_write_record_cmd(
  uimdiag_mmgsdi_session_write_record_cmd_type          * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
  {

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.Access method
  3.ef enum
  4.df enum
  5.path len and path
  6.aid len and aid
  7.record type
  8.record number
  9.data length and data
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access "access_method" */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->access_method);

  /* offset updated to access ef enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->ef_enum);

  /* offset updated to access df enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->df_enum);

  /* offset updated to get path length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->path_len);

  /* offset updated to access the path */
  uimdiag_mmgsdi_util_decode_len_val_uint16(
    (uint8 **)&(cmd_ptr->path_ptr),
           cmd_ptr->path_len);

  /* aid_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->aid_len);

  /* offset updated to access the AID Data Pointer */
  if(cmd_ptr->aid_len == 0)
  {
    cmd_ptr->aid_ptr = NULL;
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->aid_ptr),
    cmd_ptr->aid_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->rec_type);

  /* offset updated to get record number */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->rec_num);

   /* offset updated to get data length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->data_len);

  /* offset updated to access the data Pointer */

  if(cmd_ptr->data_len == 0)
  {
    cmd_ptr->data_ptr = NULL;
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->data_ptr),
    cmd_ptr->data_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_write_record_cmd */


/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_WRITE_TRANSPARENT_CMD

DESCRIPTION
  This command is used to build a request to MMGSDI to write to transparent file.

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_write_transparent_cmd(
  uimdiag_mmgsdi_session_write_transparent_cmd_type    * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
  {

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.Access method
  3.ef enum
  4.df enum
  5.path len and path
  6.aid len and aid
  7.offset
  8.data length and data
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access "access_method" */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->access_method);

  /* offset updated to access ef enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->ef_enum);

  /* offset updated to access df enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->df_enum);

  /* offset updated to get path length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->path_len);

  /* offset updated to access the path */
  uimdiag_mmgsdi_util_decode_len_val_uint16(
    (uint8 **)&(cmd_ptr->path_ptr),
           cmd_ptr->path_len);

  /* aid_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->aid_len);

  /* offset updated to access the AID Data Pointer */
  if(cmd_ptr->aid_len == 0)
  {
    cmd_ptr->aid_ptr = NULL;
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->aid_ptr),
    cmd_ptr->aid_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->offset);

  /* offset updated to get data length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->data_len);

  /* offset updated to access the data Pointer */
  if(cmd_ptr->data_len == 0)
  {
    cmd_ptr->data_ptr = NULL;
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->data_ptr),
    cmd_ptr->data_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_write_transparent_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_REFRESH_CMD

DESCRIPTION
  This command is used to build a request to register / deregister
  for refresh

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_refresh_cmd(
    uimdiag_mmgsdi_session_refresh_cmd_type              * cmd_ptr,
    const void                                           * diag_req_ptr,
    uint32                                                 diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.File List length
  3.File List
  4.File Path length
  5.File path
  6.Vote for Init

  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access file list length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->num_of_files);

  /* offset updated to access file_list */
  /* Note file_list_ptr is uint32*        */
  UIMDIAG_ASSERT((uimdiag_offset <= diag_req_len), UIMDIAG_ERROR);

  if(cmd_ptr->num_of_files > 0)
  {
    UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      cmd_ptr->file_list_ptr,
      (cmd_ptr->num_of_files * sizeof(uint32)));

    if(cmd_ptr->file_list_ptr == NULL)
    {
      uimdiag_decode_status = UIMDIAG_MEMORY_ERROR_HEAP_EXHAUSTED;
      return uimdiag_decode_status;
    }

    UIMDIAG_ASSERT(((uimdiag_offset + cmd_ptr->num_of_files) <= diag_req_len), UIMDIAG_ERROR);
    memscpy((void *)cmd_ptr->file_list_ptr,
      (cmd_ptr->num_of_files * sizeof(uint32)),
      (void *)((uint8*)diag_req_ptr + uimdiag_offset),
      (cmd_ptr->num_of_files* sizeof(uint32)));
  }

  if(cmd_ptr->num_of_files == 0)
  {
    cmd_ptr->file_list_ptr = NULL;
  }

  uimdiag_offset  += (cmd_ptr->num_of_files*sizeof(uint32));

  /* offset updated to access file path length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->file_path_len);

  /* offset updated to get file path */
  uimdiag_mmgsdi_util_decode_len_val_uint16(
    (uint8 **)&(cmd_ptr->file_path_ptr),
      cmd_ptr->file_path_len);

  /* offset being updated to point to vote for init */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->vote_for_init);

  /* offset updated to the end of the data structure */
  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_refresh_cmd */


/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_REFRESH_PROCEED_VOTING_CMD

DESCRIPTION
  This command is used to build a request to register / deregister
  for refresh and also to receive a notification from MMGSDI before proceeding with Refresh

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_refresh_proceed_voting_cmd(
    uimdiag_mmgsdi_session_refresh_proceed_voting_cmd_type   * cmd_ptr,
    const void                                               * diag_req_ptr,
    uint32                                                     diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.File List length
  3.File List
  4.File Path length
  5.File path
  6.Vote for Init

  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access file list length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->num_of_files);

  /* offset updated to access file_list */
  /* Note file_list_ptr is uint32*        */
  UIMDIAG_ASSERT((uimdiag_offset <= diag_req_len), UIMDIAG_ERROR);

  if(cmd_ptr->num_of_files > 0)
  {
    UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      cmd_ptr->file_list_ptr,
      (cmd_ptr->num_of_files * sizeof(uint32)));

    if(cmd_ptr->file_list_ptr == NULL)
    {
      uimdiag_decode_status = UIMDIAG_MEMORY_ERROR_HEAP_EXHAUSTED;
      return uimdiag_decode_status;
    }

    UIMDIAG_ASSERT(((uimdiag_offset + cmd_ptr->num_of_files) <= diag_req_len), UIMDIAG_ERROR);
    memscpy((void *)cmd_ptr->file_list_ptr,
      (cmd_ptr->num_of_files * sizeof(uint32)),
      (void *)((uint8*)diag_req_ptr + uimdiag_offset),
      (cmd_ptr->num_of_files* sizeof(uint32)));
  }

  if(cmd_ptr->num_of_files == 0)
  {
    cmd_ptr->file_list_ptr = NULL;
  }

  uimdiag_offset  += (cmd_ptr->num_of_files*sizeof(uint32));

  /* offset updated to access file path length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->file_path_len);

  /* offset updated to get file path */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->file_path_ptr),
      cmd_ptr->file_path_len);

  /* offset being updated to point to vote for init */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->vote_for_init);

  /* offset being updated to point to ok to proceed */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->proceed_refresh);

  /* offset updated to the end of the data structure */
  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_refresh_proceed_voting_cmd */


/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_NAA_REFRESH_CMD

DESCRIPTION
  This command is used to build a request to trigger for refresh

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_naa_refresh_cmd(
    uimdiag_mmgsdi_naa_refresh_cmd_type   * cmd_ptr,
    const void                            * diag_req_ptr,
    uint32                                  diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);


  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1. Client ID Id Lower/Higher 32 bits
  2. Slot ID.
  3. App ID Length
  4. App ID
  5. Refresh Mode
  6. Path Length
  7. Path

  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

   /* offset updated to access slot ID  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->slot_id);


  /* offset updated to access App Id Length  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->sim_app_id_len);

  /* offset updated to access aid*/
  /* Note sim_app_id is uint32*        */
  UIMDIAG_ASSERT((uimdiag_offset <= diag_req_len), UIMDIAG_ERROR);

  if(cmd_ptr->sim_app_id_len > 0)
  {
    UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      cmd_ptr->sim_app_id,
      (cmd_ptr->sim_app_id_len * sizeof(uint8)));
    if(cmd_ptr->sim_app_id == NULL)
    {
      uimdiag_decode_status = UIMDIAG_ERROR;
      return uimdiag_decode_status;
    }

    UIMDIAG_ASSERT(((uimdiag_offset + cmd_ptr->sim_app_id_len) <= diag_req_len), UIMDIAG_ERROR);
    memscpy((void *)cmd_ptr->sim_app_id,
      (cmd_ptr->sim_app_id_len * sizeof(uint8)),
      (void *)((uint8*)diag_req_ptr + uimdiag_offset),
      (cmd_ptr->sim_app_id_len* sizeof(uint8)));
  }

  if(cmd_ptr->sim_app_id_len == 0)
  {
    cmd_ptr->sim_app_id = NULL;
  }

  uimdiag_offset  += (cmd_ptr->sim_app_id_len*sizeof(uint8));

  /* offset being updated to point to refresh mode */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->refresh_mode);

  /* offset being updated to point to path Length  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->path_len);

  /* offset updated to path */
  /* Note path length is uint8*        */
  UIMDIAG_ASSERT((uimdiag_offset <= diag_req_len), UIMDIAG_ERROR);

  if(cmd_ptr->path_len > 0)
  {
    UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      cmd_ptr->path,
      (cmd_ptr->path_len * sizeof(uint8)));

    if(cmd_ptr->path == NULL)
    {
      uimdiag_decode_status = UIMDIAG_ERROR;
      return uimdiag_decode_status;
    }

    UIMDIAG_ASSERT(((uimdiag_offset + cmd_ptr->path_len) <= diag_req_len), UIMDIAG_ERROR);
    memscpy((void *)cmd_ptr->path,
      (cmd_ptr->path_len * sizeof(uint8)),
      (void *)((uint8*)diag_req_ptr + uimdiag_offset),
      (cmd_ptr->path_len* sizeof(uint8)));
  }

  if(cmd_ptr->path_len == 0)
  {
    cmd_ptr->path = NULL;
  }

  uimdiag_offset  += (cmd_ptr->path_len*sizeof(uint8));

  /* offset updated to the end of the data structure */
  return uimdiag_decode_status;

}/* uimdiag_mmgsdi_util_populate_session_naa_refresh_cmd */


/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_OK_TO_REFRESH_CMD

DESCRIPTION
  This command is used to build a request to simulate an OK to Refresh to MMGSDI.

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_ok_to_refresh_cmd (
    uimdiag_mmgsdi_session_ok_to_refresh_cmd_type        * cmd_ptr,
    const void                                           * diag_req_ptr,
    uint32                                                 diag_req_len )
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.Ok_to_refresh

  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access ok_to_refresh field */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->ok_to_refresh);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_ok_to_refresh_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_PROCEED_WITH_REFRESH_CMD

DESCRIPTION
  This command is used to build a request to simulate an proceed with refresh to MMGSDI.

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_proceed_with_refresh_cmd (
    uimdiag_mmgsdi_session_proceed_refresh_cmd_type      * cmd_ptr,
    const void                                           * diag_req_ptr,
    uint32                                                 diag_req_len )
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.Ok_to_refresh

  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access ok_to_refresh field */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->ok_to_proceed);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_proceed_with_refresh_cmd */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_REFRESH_COMPLETE_CMD

DESCRIPTION
  This command is used to build a request to to indicate to MMGSDI that the
  client has finished its refresh process

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_refresh_complete_cmd(
    uimdiag_mmgsdi_session_refresh_complete_cmd_type     * cmd_ptr,
    const void                                           * diag_req_ptr,
    uint32                                                 diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.Pass_fail
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access pass or fail refresh field */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->pass_fail);

  return UIMDIAG_SUCCESS;
}/* uimdiag_mmgsdi_util_populate_session_refresh_complete_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_ISIM_AUTH_CMD

DESCRIPTION
  This function parses cmd_ptr and is used to erform a "Network" Initiated
  Authentication with the ISIM Application.

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_isim_auth_cmd(
  uimdiag_mmgsdi_session_isim_auth_cmd_type    * cmd_ptr,
  const void                                   * diag_req_ptr,
  uint32                                         diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1. Session Id Lower/Higher 32 bits
  2. Auth Challenge Len
  3. Auth challenge Data
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to get Auth Challenge length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->auth_data_len);

  /* offset updated to access the Auth Challenge Data Pointer */

  if(cmd_ptr->auth_data_len == 0)
  {
     cmd_ptr->auth_data_ptr = NULL;
     UIMDIAG_MSG_ERROR_0("Session ISIM Authenticate  Data length is zero, 0, 0");
     return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->auth_data_ptr),
    cmd_ptr->auth_data_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_isim_auth_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_SEND_CARD_STATUS_CMD

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  concerning the current directory

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_send_card_status_cmd(
    uimdiag_mmgsdi_session_send_crd_status_cmd_type    * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  1. Session Id:      Lower/Higher 32 bits
  2. me_app_status:   The status of the application in the handset
  3. ret_data_struct: Structure in which the data should be returned in
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to status of the application in the handset  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->me_app_status);

  /* offset updated to Structure in which the data should be returned in  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->ret_data_struct);

    /* offset being updated to point to the end of session send card status command */

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_send_card_status_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_SERVICE

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related enable/ disable service, is service available

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_service(
    uimdiag_mmgsdi_session_service_cmd_type            * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  1. Session Id:      Lower/Higher 32 bits
  2. service:         The service that needs disabled/ enabled
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to service type  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->service);

  /* offset being updated to point to the end of session send card status command */

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_service */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_SEEK_CMD

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_SEEK_CMD

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_seek_cmd(
  uimdiag_mmgsdi_session_seek_cmd_type          * cmd_ptr,
  const void                                    * diag_req_ptr,
  uint32                                          diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2.Access method
  3.ef enum
  4.df enum
  5.path len and path
  6.aid len and aid
  7.seek direction
  8.seek pattern
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* offset updated to access Session ID higher 32 bits */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* offset updated to access Session ID lower 32 bits */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access "access_method" */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->access_method);

  /* offset updated to access ef enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->ef_enum);

  /* offset updated to access df enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->df_enum);

  /* offset updated to get path length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->path_len);

  /* offset updated to access the path */
  uimdiag_mmgsdi_util_decode_len_val_uint16(
    (uint8 **)&(cmd_ptr->path_ptr),
           cmd_ptr->path_len);

  /* aid_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->aid_len);

  /* offset updated to access the AID Data Pointer */
  if(cmd_ptr->aid_len == 0)
  {
    cmd_ptr->aid_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("POPULATE SESSION SEEK CMD, AID length is 0");
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->aid_ptr),
    cmd_ptr->aid_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->seek_direction);

  /* offset updated to access "seek pattern" */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->seek_pattern_len);

  /* offset updated to access the Seek Pattern Pointer */
  if(cmd_ptr->seek_pattern_len== 0)
  {
    cmd_ptr->seek_pattern_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("POPULATE SESSION SEEK CMD, Seek Pattern length is 0");
    return UIMDIAG_ERROR;
  }
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->seek_pattern_ptr),
    cmd_ptr->seek_pattern_len);

  return UIMDIAG_SUCCESS;
}/* uimdiag_mmgsdi_util_populate_session_seek_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_SEARCH_CMD

DESCRIPTION
  This command performs Search command on UICC card.

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_search_cmd(
  uimdiag_mmgsdi_session_search_cmd_type        * cmd_ptr,
  const void                                    * diag_req_ptr,
  uint32                                          diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.  Session Id Lower/Higher 32 bits
  2.  Access method
  3.  ef enum
  4.  df enum
  5.  path len and path
  6.  aid len and aid
  7.  search_type
  8.  search_record_num
  9.  search_direction
  10. offset
  11. search_pattern
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* offset updated to access Session ID higher 32 bits */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* offset updated to access Session ID lower 32 bits */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access "access_method" */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->access_method);

  /* offset updated to access ef enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->ef_enum);

  /* offset updated to access df enum */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->df_enum);

  /* offset updated to get path length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->path_len);

  /* offset updated to access the path */
  uimdiag_mmgsdi_util_decode_len_val_uint16(
    (uint8 **)&(cmd_ptr->path_ptr),
           cmd_ptr->path_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->aid_len);

  /* offset updated to access the AID Data Pointer */
  if(cmd_ptr->aid_len == 0)
  {
    cmd_ptr->aid_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("POPULATE SESSION SEARCH CMD, AID length is 0");
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->aid_ptr),
    cmd_ptr->aid_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->search_type);

  /* offset updated to access search record number */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->search_record_num);

  /* offset updated to access search direction */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->search_direction);

  /* offset updated to access the offset_type */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->offset_type);

  /* offset updated to access the offset data */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->offset_data);

  /* offset updated to access the Search Pattern Length  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->search_pattern_len);

  /* offset updated to access the Search Pattern Data Pointer  */
  if(cmd_ptr->search_pattern_len == 0)
  {
    cmd_ptr->search_pattern_ptr = NULL;
    UIMDIAG_MSG_ERROR_0("POPULATE SESSION SEARCH CMD, Search Pattern length is 0");
  }
  else
  {
    uimdiag_mmgsdi_util_decode_len_val_uint8(
      (uint8 **)&(cmd_ptr->search_pattern_ptr),
      cmd_ptr->search_pattern_len);
  }

  return UIMDIAG_SUCCESS;
}/* uimdiag_mmgsdi_util_populate_session_search_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_PROV_APP_INIT_COMPLETE

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_CMD

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_app_init_complete(
    uimdiag_mmgsdi_session_prov_app_init_compl_cmd_type * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  1. Session Id:      Lower/Higher 32 bits
  2. init_status:       Initialization status of provisional App
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to init_status  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->init_status);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_app_init_complete */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_COMPUTE_IP_AUTH

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SESSION_COMPUTE_IP_AUTH

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_compute_ip_auth(
    uimdiag_mmgsdi_session_compute_ip_auth_cmd_type    * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  1. Session Id                :      Lower/Higher 32 bits
  2. operation_type         :   Type of authentication operation
  3. chap_id, chap_nai_entry_index, chap_chal_len, chap_challenge
                                     : CHAP operation for the SIP.
  4. mn_ha_nai_entry_index,mn_ha_reg_data_len,mn_ha_reg_data
                                     :  MN-HA authenticator.
  5. rrq_data_len,rrq_data : MIP-RRQ hash
  6. mn_aaa_nai_entry_index, mn_aaa_chal_len, mn_aaa_challenge
                                     : MN-AAA authenticator.
  7. hrpd_chap_id, hrpd_chap_chal_len, hrpd_chap_challenge
                                     : CHAP operation for High Rate Packet Data
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* operation_type */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->operation_type);

  /* chap_id  */
  uimdiag_mmgsdi_util_decode_uint8((void*)&cmd_ptr->chap_id);

  /* chap_nai_entry_index */
  uimdiag_mmgsdi_util_decode_uint8((void*)&cmd_ptr->chap_nai_entry_index);

  /* chap_chal_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->chap_chal_len);

  if(cmd_ptr->chap_chal_len > MMGSDI_MAX_3GPD_CHAP_CHALLENGE_LENGTH)
  {
     return UIMDIAG_ERROR;
  }

  /* chap_challenge */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
   (uint8 **)&(cmd_ptr->chap_challenge),
    cmd_ptr->chap_chal_len);

  /* mn_ha_nai_entry_index */
  uimdiag_mmgsdi_util_decode_uint8((void*)&cmd_ptr->mn_ha_nai_entry_index);

  /* mn_ha_reg_data_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->mn_ha_reg_data_len);

  /* mn_ha_reg_data */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->mn_ha_reg_data),
    cmd_ptr->mn_ha_reg_data_len);

  /* rrq_data_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->rrq_data_len);

  /* rrq_data */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->rrq_data),
    cmd_ptr->rrq_data_len);

  /* mn_aaa_nai_entry_index */
  uimdiag_mmgsdi_util_decode_uint8((void*)&cmd_ptr->mn_aaa_nai_entry_index);

  /* mn_aaa_chal_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->mn_aaa_chal_len);

  /* mn_aaa_challenge */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
      (uint8 **)&(cmd_ptr->mn_aaa_challenge),
       cmd_ptr->mn_aaa_chal_len);

  /* hrpd_chap_id */
  uimdiag_mmgsdi_util_decode_uint8((void*)&cmd_ptr->hrpd_chap_id);

  /* hrpd_chap_chal_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->hrpd_chap_chal_len);

  /* hrpd_chap_challenge */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->hrpd_chap_challenge),
     cmd_ptr->hrpd_chap_chal_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_app_init_complete */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_SSD_UPDATE

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SESSION_SSD_UPDATE

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_ssd_update(
    uimdiag_mmgsdi_session_ssd_update_cmd_type         * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer
  session_id_high:  Higher 32 bits of session ID
  session_id_low:   Lower  32 bits of session ID
  randssd:              56-bit random number from the base station for SSD
                                generation.
  process_control:  Process control bits
  esn:                     Electronic Serial Number of the ME

  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* randssd_high */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->randssd_high);

  /* randssd_low  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->randssd_low);

  /* process_control */
  uimdiag_mmgsdi_util_decode_uint8((void*)&cmd_ptr->process_control);

  /* esn */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->esn);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_ssd_update */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_SSD_CONFIRM

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SESSION_SSD_CONFIRM

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_ssd_confirm(
    uimdiag_mmgsdi_session_confirm_ssd_cmd_type        * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  session_id_high:   Higher 32 bits of session ID
  session_id_low:    Lower  32 bits of session ID
  authbs:                 Authentication signature from the base station.
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* authbs */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->authbs);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_ssd_confirm */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_BS_CHAL

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SESSION_BS_CHAL

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_bs_chal(
    uimdiag_mmgsdi_session_bs_chal_cmd_type            * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

 /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  session_id_high:   Higher 32 bits of session ID
  session_id_low:    Lower  32 bits of session ID
  randseed:             Random number generator seed.
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* randseed */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->randseed);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_bs_chal */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_INFO_SYNC

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SESSION_GET_INFO_SYNC

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_get_info_sync(
    uimdiag_mmgsdi_session_get_info_sync_cmd_type      * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* ------------------------------------------------------------------
   query_type:          0 - By Session_id, 1 - By session type
   session_id_high:   Higher 32 bits of session ID
   session_id_low:    Lower  32 bits of session ID
   session_type:       Session Type
   --------------------------------------------------------------------*/
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);
   /* query_type */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->query_type);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* session_type */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_type);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_get_info_sync */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_read_cache_file_size_cmd

DESCRIPTION
  Gets the size of the file to be cached for a read operation/Read the cached file.
DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_read_cache_file_size_cmd(
    uimdiag_mmgsdi_session_read_cache_file_size_cmd_type  * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  session_id_high:   Higher 32 bits of session ID
  session_id_low:    Lower  32 bits of session ID
  file_name:            EF
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* file_name */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->file_name);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_read_cache_file_size_cmd */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_read_cache_cmd

DESCRIPTION
  Gets the size of the file to be cached for a read operation/Read the cached file.
DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_read_cache_cmd(
    uimdiag_mmgsdi_session_read_cache_cmd_type         * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  session_id_high:   Higher 32 bits of session ID
  session_id_low:    Lower  32 bits of session ID
  file_name:            EF
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* file_name */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->file_name);

  /* offset updated to get data length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->data_len);

  /* offset updated to access the data Pointer */

  if(cmd_ptr->data_len == 0 || cmd_ptr->data_len > UIMDIAG_MAX_READ_DATA_LEN)
  {
    return UIMDIAG_ERROR;
  }
  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_read_cache_cmd */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_write_cache

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SESSION_WRITE_CACHE

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_write_cache(
    uimdiag_mmgsdi_session_write_cache_cmd_type            * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  session_id_high:   Higher 32 bits of session ID
  session_id_low:    Lower  32 bits of session ID
  file_name:             Random number generator seed.
  data_len:              Length of data to write
  data:                    Data to write
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* file_name */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->file_name);

  /* data_len */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->data_len);

  /* data */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->data),
     cmd_ptr->data_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_write_cache */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_pwr_ctrl_sync

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SESSION_TOGGLE_UIM_PWR_CTRL_SYNC

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_pwr_ctrl_sync(
    uimdiag_mmgsdi_session_toggle_uim_pwr_ctrl_cmd_type * cmd_ptr,
    const void                                          * diag_req_ptr,
    uint32                                                diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  session_id_high:   Higher 32 bits of session ID
  session_id_low:    Lower  32 bits of session ID
  in_TC:             Indicates whether the modem is in or out of the traffic channel.
  mode:              Mode on which traffic channel established GW or 1X
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* in_TC */
  uimdiag_mmgsdi_util_decode_uint8((void*)&cmd_ptr->in_TC);

  /* mode */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->mode);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_pwr_ctrl_sync */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_sap_get_atr

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SAP_GET_ATR

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_sap_get_atr(
    uimdiag_mmgsdi_sap_get_atr_cmd_type * cmd_ptr,
    const void                          * diag_req_ptr,
    uint32                                diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  client_id_high:   Higher 32 bits of client  ID
  client_id_low:    Lower  32 bits of client  ID
  card_slot:        Indicates the SLOT

  ------------------------------------------------------------------- */

  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* Card slot  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->card_slot);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_sap_get_atr */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_sap_send_apdu

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SAP_SEND_APDU

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_sap_send_apdu(
    uimdiag_mmgsdi_sap_send_apdu_cmd_type * cmd_ptr,
    const void                            * diag_req_ptr,
    uint32                                  diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  client_id_high:   Higher 32 bits of client  ID
  client_id_low:    Lower  32 bits of client  ID
  card_slot:        Indicates the SLOT
  channel_id:       The logical channel id
  apdu_data_len:    Length of the APDU
  apdu_data_ptr:    Holds Apdu data
  ------------------------------------------------------------------- */

  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* Card slot  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->card_slot);

  /* Channel Id */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->channel_id);

  /* APDU Data length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->apdu_data_len);

  /* APDU data */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->apdu_data_ptr),
     cmd_ptr->apdu_data_len);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_sap_send_apdu */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_sap_generic

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SAP_POWER_ON
             UIMDIAG_MMGSDI_SAP_POWER_OFF
             UIMDIAG_MMGSDI_UTIL_POPULATE_SAP_CARD_READER_STATUS
             UIMDIAG_MMGSDI_SAP_RESET


DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_sap_generic(
    uimdiag_mmgsdi_sap_generic_cmd_type * cmd_ptr,
    const void                           * diag_req_ptr,
    uint32                                 diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  client_id_high:   Higher 32 bits of client  ID
  client_id_low:    Lower  32 bits of client  ID
  card_slot:        Indicates the SLOT

  ------------------------------------------------------------------- */

  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* Card slot  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->card_slot);

  /* SAP mode */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->sap_mode);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_sap_generic */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_card_pdown

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_CARD_PDOWN

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_card_pdown(
    uimdiag_mmgsdi_card_pdown_cmd_type * cmd_ptr,
    const void                         * diag_req_ptr,
    uint32                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  client_id_high:   Higher 32 bits of client  ID
  client_id_low:    Lower  32 bits of client  ID
  slot_id:          Indicates the SLOT
  power_down_option:Indicates whether to just power down the CARD or
                    to do a power down and notify GSDI about card
                    removal

  ------------------------------------------------------------------- */

  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* Card slot  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->slot_id);

  /* power_down_option */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->power_down_option);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_card_pdown */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_card_pup

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_CARD_PUP

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_card_pup(
    uimdiag_mmgsdi_card_pup_cmd_type * cmd_ptr,
    const void                       * diag_req_ptr,
    uint32                             diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  client_id_high:   Higher 32 bits of client  ID
  client_id_low:    Lower  32 bits of client  ID
  slot_id:          Indicates the SLOT
  power_up_option:  Card power up option

  ------------------------------------------------------------------- */

  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* Card slot  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->slot_id);

  /* power_up_option */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->power_up_option);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_card_pup */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_activate_onchip_sim

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_SIM

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_activate_onchip_sim(
    uimdiag_mmgsdi_activate_onchip_sim_cmd_type * cmd_ptr,
    const void                                  * diag_req_ptr,
    uint32                                        diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  client_id_high:                      Higher 32 bits of client  ID
  client_id_low:                       Lower  32 bits of client  ID
  slot_id:                             Indicates the SLOT
  onchip_data_length, onchip_data_ptr: TLV data length and data

  ------------------------------------------------------------------- */

  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* Card slot  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->slot_id);

  /* offset updated to get TLV data length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->onchip_data_length);

  /* offset updated to access the TLV data pointer */
  if(cmd_ptr->onchip_data_length == 0)
  {
    cmd_ptr->onchip_data_ptr = NULL;
    return UIMDIAG_ERROR;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->onchip_data_ptr),
    cmd_ptr->onchip_data_length);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_activate_onchip_sim */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_send_apdu

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SEND_APDU

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_send_apdu(
    uimdiag_mmgsdi_send_apdu_cmd_type * cmd_ptr,
    const void                        * diag_req_ptr,
    uint32                              diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  channel_id:       Default channel ID
  slot_id:          Indicates the SLOT
  apdu_data_length: APDU buffer length
  apdu_data_ptr:    APDU buffer

  ------------------------------------------------------------------- */

  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* Card slot  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->slot_id);

   /* APDU Data length */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->apdu_data_length);

  /* APDU data */
  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->apdu_data_ptr),
     cmd_ptr->apdu_data_length);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_send_apdu */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_sap_disconnect

DESCRIPTION
  This function parses cmd_ptr and is used to get all information
  related to UIMDIAG_MMGSDI_SAP_DISCONNECT

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_sap_disconnect(
    uimdiag_mmgsdi_sap_disconnect_cmd_type * cmd_ptr,
    const void                             * diag_req_ptr,
    uint32                                   diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  client_id_high:   Higher 32 bits of client  ID
  client_id_low:    Lower  32 bits of client  ID
  card_slot:        Indicates the SLOT

  ------------------------------------------------------------------- */

  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* Card slot  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->card_slot);

  /* SAP mode */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->sap_mode);

  /* SAP mode */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->sap_disconnect_mode);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_sap_generic */

/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_GET_OPERATOR_NAME_CMD

DESCRIPTION
  This command is used to build a request to MMGSDI for get operator name.

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_get_operator_name_cmd(
  uimdiag_mmgsdi_session_get_operator_name_cmd_type    * cmd_ptr,
  const void                                           * diag_req_ptr,
  uint32                                                 diag_req_len)
  {

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1.Session Id Lower/Higher 32 bits
  2. No of PLMNs
  3. PLMN ID(s)
  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* Session ID higher 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_high);

  /* Session ID lower 32 bits  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_id_low);

  /* offset updated to access "no_of_plmns" */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->no_of_plmns);

  if(cmd_ptr->no_of_plmns > UIMDIAG_MAX_PLMN_IN_LIST)
  {
    UIMDIAG_MSG_ERROR_0("No of PLMNS is unexpected");
    uimdiag_decode_status = UIMDIAG_INCORRECT_PARAM;
  }

  uimdiag_mmgsdi_util_decode_len_val_uint8(
    (uint8 **)&(cmd_ptr->plmn_list_ptr),
    cmd_ptr->no_of_plmns * 4); /*4 is UIMDIAG input array length for PLMN-RAT*/

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_get_operator_name_cmd */

/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_populate_get_all_available_app_cmd

DESCRIPTION
  Gets the client id for get all available apps sync cmd.
DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_get_all_available_app_cmd(
    uimdiag_mmgsdi_get_all_available_apps_cmd_type     * cmd_ptr,
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* ------------------------------------------------------------------
  Parse following fields from diag buffer

  client_id_high:   Higher 32 bits of client ID
  client_id_low:    Lower  32 bits of client ID
  ------------------------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);


  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_get_all_available_app_cmd */

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_OPEN_EXT_CMD

DESCRIPTION
  This function parses cmd_ptr and fills the session open ext command params

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_util_populate_session_open_ext_cmd(
  uimdiag_mmgsdi_session_open_ext_cmd_type         * cmd_ptr,
  const void                                       * diag_req_ptr,
  uint32                                             diag_req_len)
{
  UIMDIAG_UTIL_RETURN_IF_NULL_2(cmd_ptr, diag_req_ptr);

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1. Client Id Lower/Higher 32 bits
  2. Session Type_mask
  3. Set Notify_mask

  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  /* offset updated to access Client ID Higher 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);

  /* offset updated to access Client ID Lower 32 bits */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

  /* offset updated to access the session type mask field*/

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->session_type_mask);

  /* offset updated to access the set notify mask field */

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->set_notify_mask);

  return uimdiag_decode_status;
}/* uimdiag_mmgsdi_util_populate_session_open_ext_cmd */
/* ==========================================================================
   FUNCTION:      UIMDIAG_UTIL_CONVERT_TO_MMGSDI_ITEMS_ENUM

   DESCRIPTION:
     This function checks for valid range of the mmgsdi_file_enum_type and
     returns the corresponding mmgsdi_enum_type

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_NO_FILE_ENUM     : uim item not found in the conversion table


   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_file_enum_type uimdiag_util_convert_to_mmgsdi_items_enum (
  uimdiag_file_enum_type uimdiag_mmgsdi_file
)
{
  uint32 i     = 0;
  uint32 count = (uint32)(sizeof(uimdiag_file_to_mmgsdi_table) /
                          sizeof(uimdiag_file_to_mmgsdi_table[0]));

  for (i = 0; i< count; i++)
  {
    if (uimdiag_file_to_mmgsdi_table[i].uimdiag_file == uimdiag_mmgsdi_file)
    {
      return uimdiag_file_to_mmgsdi_table[i].mmgsdi_file;
    }
  }
  return MMGSDI_NO_FILE_ENUM;
} /* uimdiag_util_convert_to_mmgsdi_items_enum */

/* ==========================================================================
   FUNCTION:      UIMDIAG_UTIL_CONVERT_FROM_MMGSDI_ITEMS_ENUM

   DESCRIPTION:
     This function checks for valid range of the mmgsdi_file_enum_type and
     returns the corresponding uimdiag_file_enum_type

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     uimdiag_file_enum_type


   SIDE EFFECTS:
     None
==========================================================================*/
uimdiag_file_enum_type uimdiag_util_convert_from_mmgsdi_items_enum (
  mmgsdi_file_enum_type mmgsdi_file
)
{
  uint32 i     = 0;
  uint32 count = (uint32)(sizeof(uimdiag_file_to_mmgsdi_table) /
                          sizeof(uimdiag_file_to_mmgsdi_table[0]));

  for (i = 0; i< count; i++)
  {
    if (uimdiag_file_to_mmgsdi_table[i].mmgsdi_file == mmgsdi_file)
    {
      return uimdiag_file_to_mmgsdi_table[i].uimdiag_file;
    }
  }
  return UIMDIAG_NO_FILE_ENUM;
} /* uimdiag_util_convert_from_mmgsdi_items_enum */


/*===========================================================================
FUNCTION: UIMDIAG_PARSE_CMD_DATA

DESCRIPTION:
  Populates the request structure with data from the Diag packet.

INPUTS:
  *req_ptr:       Generic_command struct request pointer
                  which will be populated with data from the diag_req_ptr
  *diag_req_ptr:  Pointer to raw Diag request packet
  diag_req_len:   Length of the diag request packet

DEPENDENCIES:
  None

RETURN VALUE:
  uimdiag_return_enum_type

OUTPUT:
  None
===========================================================================*/
uimdiag_return_enum_type uimdiag_parse_cmd_data (
  uimdiag_generic_req_type  *req_ptr,
  PACKED void               *diag_req_ptr,
  uint32                     diag_req_len
)
{
  uimdiag_subsystem_cmd_enum_type uimdiag_subsys_cmd = UIMDIAG_MAX_CMD;

  UIMDIAG_MSG_HIGH_0("uimdiag_parse_cmd_data");

  if ((req_ptr == NULL) ||
      (diag_req_ptr == NULL) ||
      (diag_req_len < sizeof(uimdiag_cmd_header_type)))
  {
    UIMDIAG_MSG_ERROR_0("req_ptr or diag_req_ptr is NULL or diag_req_len to small:");
    return UIMDIAG_ERROR;
  }

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1. COMMAND CODE
  2. SUBSYSTEM ID
  3. SUBSYSTEM CMD CODE
  4. COMMAND Specific params
  ----------------------------------------------------- */

  /* command code */
  (void)memscpy( ((uint8*)&(req_ptr->cmd_header.command_code)),
                 sizeof(req_ptr->cmd_header.command_code),
                 (uint8*)diag_req_ptr, sizeof(uint8));

  diag_req_ptr  = (char*)diag_req_ptr + sizeof(uint8);
  diag_req_len -=  sizeof(uint8);
  
  /* subsystem id */
  (void)memscpy(((uint8*)&(req_ptr->cmd_header.subsys_id)),
                sizeof(req_ptr->cmd_header.subsys_id),
                (uint8*)diag_req_ptr, sizeof(diagpkt_subsys_id_type));

  diag_req_ptr  = (char*)diag_req_ptr + sizeof(diagpkt_subsys_id_type);
  diag_req_len -=  sizeof(diagpkt_subsys_id_type);
  
  /* subsystem command code */
  (void) memscpy( ((void*)&(req_ptr->cmd_header.subsys_cmd_code)),
                  sizeof(req_ptr->cmd_header.subsys_cmd_code),
                  (void*)diag_req_ptr,sizeof(diagpkt_subsys_cmd_code_type));

  diag_req_ptr  = (char*)diag_req_ptr + sizeof(diagpkt_subsys_cmd_code_type);
  diag_req_len -=  sizeof(diagpkt_subsys_cmd_code_type);

  uimdiag_subsys_cmd = (uimdiag_subsystem_cmd_enum_type)req_ptr->cmd_header.subsys_cmd_code;
  switch(uimdiag_subsys_cmd)
  {
      case UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_REG_CMD:
        return UIMDIAG_SUCCESS;

      case UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_DEREG_CMD:
        return uimdiag_mmgsdi_util_populate_client_id_and_evt_dereg_cmd(
                       &req_ptr->request.client_id_and_evt_dereg,
                       (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_OPEN_CMD:
        return uimdiag_mmgsdi_util_populate_session_open_cmd(
                       &req_ptr->request.session_open,
                       (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_CLOSE_CMD:
        return uimdiag_mmgsdi_util_populate_session_id(
                       &req_ptr->request.session_close,
                       (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_ACT_OR_SW_PROV_CMD:
        return uimdiag_mmgsdi_util_populate_session_act_or_sw_prov_cmd(
                     &req_ptr->request.session_act_or_sw_prov,
                     (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_GET_INFO_CMD:
        return uimdiag_mmgsdi_util_populate_session_get_info_cmd(
                     &req_ptr->request.session_get_info,
                     (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_GET_ALL_PIN_STATUS_CMD:
        return uimdiag_mmgsdi_util_populate_session_id(
                 &req_ptr->request.session_get_all_pin_stat,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_VERIFY_PIN_CMD:
        return uimdiag_mmgsdi_util_populate_session_verify_pin_cmd(
                 &req_ptr->request.session_verify_pin,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_DISABLE_PIN_CMD:
        return uimdiag_mmgsdi_util_populate_session_disable_pin_cmd(
                 &req_ptr->request.session_disable_pin,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_ENABLE_PIN_CMD:
        return uimdiag_mmgsdi_util_populate_session_enable_pin_cmd(
                &req_ptr->request.session_enable_pin,
                (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_CHANGE_PIN_CMD:
        return uimdiag_mmgsdi_util_populate_session_change_pin_cmd(
                &req_ptr->request.session_change_pin,
                (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_UNBLOCK_PIN_CMD:
        return uimdiag_mmgsdi_util_populate_session_unblock_pin_cmd(
                &req_ptr->request.session_unblock_pin,
                (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_GET_FILE_ATTR_CMD:
        return uimdiag_mmgsdi_util_populate_session_get_file_attr_cmd(
                &req_ptr->request.session_get_file_attr,
                (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_READ_RECORD_CMD:
        return uimdiag_mmgsdi_util_populate_session_read_record_cmd(
                &req_ptr->request.session_read_record,
                (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_READ_TRANSPARENT_CMD:
        return uimdiag_mmgsdi_util_populate_session_read_transparent_cmd(
                &req_ptr->request.session_read_transparent,
                (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_WRITE_RECORD_CMD:
        return uimdiag_mmgsdi_util_populate_session_write_record_cmd(
                &req_ptr->request.session_write_record,
                (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_WRITE_TRANSPARENT_CMD:
        return uimdiag_mmgsdi_util_populate_session_write_transparent_cmd(
                &req_ptr->request.session_write_transparent,
                (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_REG_FOR_REFRESH_CMD:
      case UIMDIAG_MMGSDI_SESSION_DEREG_FOR_REFRESH_CMD:
        return uimdiag_mmgsdi_util_populate_session_refresh_cmd(
                 &req_ptr->request.session_refresh,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGDDI_SESSION_REGISTER_FOR_REFRESH_PROCEED_VOTING_CMD:
        return uimdiag_mmgsdi_util_populate_session_refresh_proceed_voting_cmd(
                 &req_ptr->request.session_refresh_proceed_voting,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_OK_TO_REFRESH_CMD:
        return uimdiag_mmgsdi_util_populate_session_ok_to_refresh_cmd(
                 &req_ptr->request.session_ok_to_refresh,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_CMD:
        return uimdiag_mmgsdi_util_populate_session_proceed_with_refresh_cmd(
                 &req_ptr->request.session_refresh_ok_to_proceed,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_NAA_REFRESH_CMD:
        return uimdiag_mmgsdi_util_populate_session_naa_refresh_cmd(
                 &req_ptr->request.session_trigger_refresh,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_REFRESH_COMPLETE_CMD:
        return uimdiag_mmgsdi_util_populate_session_refresh_complete_cmd(
                 &req_ptr->request.session_refresh_complete,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_DEACT_PROV_CMD:
        return uimdiag_mmgsdi_util_populate_session_id(
                 &req_ptr->request.session_deact_prov,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_GET_APP_CAP_CMD:
        return uimdiag_mmgsdi_util_populate_session_id(
                 &req_ptr->request.session_get_app_cap,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_GET_CPHS_INFO_CMD:
        return uimdiag_mmgsdi_util_populate_session_id(
                 &req_ptr->request.session_get_cphs_info,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_ISIM_AUTH_CMD:
        return uimdiag_mmgsdi_util_populate_session_isim_auth_cmd(
                 &req_ptr->request.session_isim_auth,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_SEND_CARD_STATUS_CMD:
        return uimdiag_mmgsdi_util_populate_session_send_card_status_cmd(
                 &req_ptr->request.session_send_crd_status,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_DISABLE_SERVICE_CMD:
        return uimdiag_mmgsdi_util_populate_session_service(
                 &req_ptr->request.session_disable_srv,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_ENABLE_SERVICE_CMD:
        return uimdiag_mmgsdi_util_populate_session_service(
                 &req_ptr->request.session_enable_srv,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_IS_SERVICE_AVAIL_CMD:
        return uimdiag_mmgsdi_util_populate_session_service(
                 &req_ptr->request.session_is_srv_avail,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_SEEK_CMD:
        return uimdiag_mmgsdi_util_populate_session_seek_cmd(
                 &req_ptr->request.session_seek_req,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_SEARCH_CMD:
        return uimdiag_mmgsdi_util_populate_session_search_cmd(
                 &req_ptr->request.session_search_req,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_CMD:
        return uimdiag_mmgsdi_util_populate_app_init_complete(
          &req_ptr->request.session_prov_app_init_complete,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_COMPUTE_IP_AUTH_CMD:
        return uimdiag_mmgsdi_util_populate_compute_ip_auth(
          &req_ptr->request.session_compute_ip_auth,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_SSD_UPDATE_CMD:
        return uimdiag_mmgsdi_util_populate_ssd_update(
          &req_ptr->request.session_ssd_update,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_SSD_CONFIRM_CMD:
        return uimdiag_mmgsdi_util_populate_ssd_confirm(
          &req_ptr->request.session_cnf_ssd,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_BS_CHAL_CMD:
        return uimdiag_mmgsdi_util_populate_bs_chal(
          &req_ptr->request.session_bs_chal,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_READ_PRL_CMD:
        return uimdiag_mmgsdi_util_populate_session_id(
          &req_ptr->request.session_read_prl,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_GET_INFO_SYNC_CMD:
        return uimdiag_mmgsdi_util_populate_get_info_sync(
          &req_ptr->request.session_get_info_sync,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_READ_CACHE_FILE_SIZE_CMD:
        return uimdiag_mmgsdi_util_populate_read_cache_file_size_cmd(
          &req_ptr->request.session_cache_file_size_type,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_READ_CACHE_CMD:
        return uimdiag_mmgsdi_util_populate_read_cache_cmd(
          &req_ptr->request.session_read_cache,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_WRITE_CACHE_CMD:
        return uimdiag_mmgsdi_util_populate_write_cache(
          &req_ptr->request.session_write_cache,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_SYNC_CMD:
        return uimdiag_mmgsdi_util_populate_session_id(
          &req_ptr->request.session_illegal_suscr_sync,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_TOGGLE_UIM_PWR_CTRL_SYNC_CMD:
        return uimdiag_mmgsdi_util_populate_pwr_ctrl_sync(
          &req_ptr->request.session_toggle_uim_pwr_ctrl,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_GET_APP_CAPABILITIES_SYNC_CMD:
        return uimdiag_mmgsdi_util_populate_session_id(
          &req_ptr->request.session_get_app_cap_sync,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SAP_GET_ATR_CMD:
        return uimdiag_mmgsdi_util_populate_sap_get_atr(
          &req_ptr->request.sap_get_atr,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SAP_SEND_APDU_CMD:
        return uimdiag_mmgsdi_util_populate_sap_send_apdu(
          &req_ptr->request.sap_send_apdu,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SAP_POWER_ON_CMD:
        return uimdiag_mmgsdi_util_populate_sap_generic(
          &req_ptr->request.sap_power_on,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SAP_POWER_OFF_CMD:
        return uimdiag_mmgsdi_util_populate_sap_generic(
          &req_ptr->request.sap_power_off,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SAP_CARD_READER_STATUS_CMD:
        return uimdiag_mmgsdi_util_populate_sap_generic(
          &req_ptr->request.sap_card_reader_status,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SAP_RESET_CMD:
        return uimdiag_mmgsdi_util_populate_sap_generic(
          &req_ptr->request.sap_reset,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_CARD_PDOWN_CMD:
        return uimdiag_mmgsdi_util_populate_card_pdown(
          &req_ptr->request.card_pdown,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_CARD_PUP_CMD:
        return uimdiag_mmgsdi_util_populate_card_pup(
          &req_ptr->request.card_pup,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_SIM_CMD:
      case UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_USIM_CMD:
        return uimdiag_mmgsdi_util_populate_activate_onchip_sim(
          &req_ptr->request.activate_onchip_sim,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SEND_APDU_CMD:
        return uimdiag_mmgsdi_util_populate_send_apdu(
          &req_ptr->request.send_apdu,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_GET_ALL_AVAILABLE_APPS_CMD:
      case UIMDIAG_MMGSDI_GET_ALL_AVAIL_APP_SYNC_CMD:
        return uimdiag_mmgsdi_util_populate_get_all_available_app_cmd(
          &req_ptr->request.get_all_available_apps,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_IS_SERVICE_AVAILABLE_SYNC_CMD:
        return uimdiag_mmgsdi_util_populate_session_service(
                 &req_ptr->request.is_service_available_sync,
                 (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SAP_CONNECT_CMD:
        return uimdiag_mmgsdi_util_populate_sap_generic(
          &req_ptr->request.sap_connect,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SAP_DISCONNECT_CMD:
        return uimdiag_mmgsdi_util_populate_sap_disconnect(
          &req_ptr->request.sap_disconnect,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_GET_OPERATOR_NAME_CMD:
        return uimdiag_mmgsdi_util_populate_session_get_operator_name_cmd(
          &req_ptr->request.session_get_operator_name,
          (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_OPEN_EXT_CMD:
         return uimdiag_mmgsdi_util_populate_session_open_ext_cmd(
                       &req_ptr->request.session_open_ext,
                       (void*)diag_req_ptr, diag_req_len);

      case UIMDIAG_MMGSDI_SESSION_GET_NVRUIM_DATA_SYNC_CMD:
        return uimdiag_mmgsdi_util_populate_nvruim_data_sync(
          &req_ptr->request.session_get_nvruim_data_sync,
          (void*)diag_req_ptr, diag_req_len);


      default:
        UIMDIAG_MSG_ERROR_1("Invalid subcmd 0x%x", uimdiag_subsys_cmd);
        return UIMDIAG_ERROR;
  }
} /* uimdiag_parse_cmd_data */

