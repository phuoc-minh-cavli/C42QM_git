/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            M M G S D I  P S M  D E F I N I T I O N S

                      A N D   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the power saving mode supporting functions.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2015 - 2020 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_psm.c#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/24/20   vgd     Delete PSM EFS files during initialization if it not PSM exit
12/26/19   tq      Map channel info for other clients during psm init
06/26/19   bcho    Added caching support for EF-ACL
02/28/19   tq      Vote (-)ve for PSM entry PSM exit procedure is in progress
12/18/18   tq      Save app capability in psm data before entering PSM mode
11/27/18   bcho    Differentiate between PSM exit and normal power up scenario
10/12/18   bcho    RCINIT Optimization
08/07/18   bcho    Store length in attr cache along with length in file cache
08/03/18   bcho    Save context on ENTER trigger instead of CONTEXT SAVE trigger
04/23/18   bcho    PSM initialization fails with error in caching proprietary EF
01/08/18   ra      Optimize write activity during PSM context save request processing
11/08/17   bcho    Store EF-OPL/EF-PNN record to PSM SFS only if it is valid
07/26/17   bcho    Store EF-OPL and EF-PNN records to PSM cache
04/06/17   nr      fix compilation error
03/15/17   nr      Add support to cache PIN1 of the SIM card when PSM mode enabled
03/16/17   bcho    Read NVs to determine PSM mode at task stop also
03/09/17   nr      Register for PSM_ENTER_REQ voting to save context
01/19/17   vdc     Few more enhancements for updating IMSI in cache
12/19/16   vdc     Update IMSI in cache without deleting it in PSM mode
09/26/16   dd      Fix compilation error
09/22/16   me      Added cache support for MMSS files   
09/22/16   tkl     Support PSM card detected mode
09/06/16   dd      Use efs_mkdir to create path for mmgsdi folder
07/07/16   kv      Change default behavior when uim psm efs is not configured
06/01/16   ks      Removing uim_set_proactive_uim slot specific functions
04/01/16   bcho    Invoke gsdi_fs_get_file_size with uint32 type length param
03/14/16   vdc     Fix compilation error
03/08/16   sp      Fix compilation warnings
02/01/16   bcho    Delete Slot, App and EF cache properly when PSM init fails
01/12/16   kv      ADF PIN Status & epsloci in case of ust disabled handling 
01/12/16   kv      UIM PSM handling updates
01/12/16   bcho    Create separate copy of LOCI, EPSLOCI and PSLOCI cache
01/12/16   bcho    Reset early init flag when card error occurs
01/12/16   bcho    EF-ECC cache creation from PSM SFS data
12/25/15   bcho    initial version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "uim_msg.h"

#include "queue.h"
#include "rex.h"

#include "mmgsdi_psm.h"
#include "mmgsdi_efs.h"
#include "mmgsdi_sfs.h"
#include "mmgsdiutil.h"
#include "mmgsdicache.h"
#include "mmgsdi_session.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_card_init.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_evt.h"
#include "mmgsdi_gen.h"
#include "mmgsdi.h"
#include "mmgsdi_uicc_rsp.h"
#include "mmgsdi_simlock.h"
#include "mmgsdi_file.h"
#include "mmgsdi_card_init.h"
#include "mmgsdi_uim_common.h"
#include "mmgsdi_common.h"
#include "mmgsdi_eons_opl_pnn.h"

#include "cm_msgr_msg.h"
#include "cmll_v.h"
#include "cm_v.h"
#include "time_svc.h"

#ifdef FEATURE_MMGSDI_3GPP2
#include "nvruim_p.h"
#endif /* FEATURE_MMGSDI_3GPP2 */

#include "uim_common_efs.h"
#include "gstk_p.h"

/*---------------------------------------------------------------------------
  MACRO:       MMGSDI_PSM_GET_FILE_ENUM_START_RANGE
  DESCRIPTION: To get file enum start range from file enum
---------------------------------------------------------------------------*/
#define MMGSDI_PSM_GET_FILE_ENUM_START_RANGE(mmgsdi_file)          \
          (((uint32)mmgsdi_file >> 8) & 0x00FF)


/* PSM UICC profile and EF data dir and file paths*/
static char *mmgsdi_safe_dir                            = "/safe";
static char *mmgsdi_sfs_dir                             = "/safe/sfs";
static char *mmgsdi_sfs_uim_dir                         = "/safe/sfs/uim";
static char *mmgsdi_sfs_uim_mmgsdi_dir                  = "/safe/sfs/uim/mmgsdi";
static char *mmgsdi_sfs_psm_uicc_profile                = "/safe/sfs/uim/mmgsdi/psm_uicc_profile_data";
static char *mmgsdi_sfs_psm_usim_sensitive_ef_data      = "/safe/sfs/uim/mmgsdi/psm_usim_sensitive_ef_data";
static char *mmgsdi_efs_dir                             = "/mmgsdi";
static char *mmgsdi_efs_uim_psm_dir                     = "/mmgsdi/psm";
static char *mmgsdi_efs_psm_usim_non_sensitive_ef_data  = "/mmgsdi/psm/usim_non_sensitive_ef_data";
#ifdef FEATURE_MMGSDI_3GPP2
static char *mmgsdi_sfs_psm_csim_sensitive_ef_data      = "/safe/sfs/uim/mmgsdi/psm_csim_sensitive_ef_data";
static char *mmgsdi_efs_psm_csim_non_sensitive_ef_data  = "/mmgsdi/psm/csim_non_sensitive_ef_data";
#endif /* FEATURE_MMGSDI_3GPP2 */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* maximum Record number for linear fixed file */
#define MMGSDI_MAX_RECS_IN_LINEAR_FIXED_EF   0xFE

/* Bitmasks for NV item 73957 */
#define MMGSDI_PSM_PIN1_CACHE_MASK                         0x01
#define MMGSDI_PSM_NO_CONTEXT_RESTORE_AFTER_PWR_CYCLE_MASK 0x02
#define MMGSDI_MAX_PROV_APP_IN_SINGLE_SLOT                 0x02

typedef PACKED struct PACKED_POST
{
  uint8                        num_of_valid_rec;
  uint8                        valid_rec_list[MMGSDI_MAX_RECS_IN_LINEAR_FIXED_EF];
}mmgsdi_valid_record_type;

/* PSM Data written into EFS */
typedef PACKED struct PACKED_POST
{
  mmgsdi_protocol_enum_type    card_type;
  mmgsdi_iccid_info_type       iccid_data;
  mmgsdi_aid_type              app_data[MMGSDI_MAX_APP_INFO];
  mmgsdi_pin_status_enum_type  pin_status;
  mmgsdi_static_data_type      pin_data;
  mmgsdi_valid_record_type     opl_record;
  uint64                       system_time;
  mmgsdi_app_capabilities_type app_cap[MMGSDI_MAX_PROV_APP_IN_SINGLE_SLOT];
}mmgsdi_psm_data_type;

/*=============================================================================
  EF Cache data format

  1. EF data will be copied from address of first_byte_data
  1. Only ALWAYS, NEVER and SINGLE protection method will be populdated.
  2. AND and OR protection would be mapped to SINGLE and first PIN value will be
     populated
=============================================================================*/
typedef PACKED struct PACKED_POST
{
  mmgsdi_file_enum_type          file_enum;
  uint8                          rec_num;
  uint8                          sfi;
  uint8                          num_of_rec;
  mmgsdi_file_security_enum_type read_protection;
  mmgsdi_pin_enum_type           read_pin;
  mmgsdi_file_security_enum_type write_protection;
  mmgsdi_pin_enum_type           write_pin;
  mmgsdi_file_security_enum_type increase_protection;
  mmgsdi_pin_enum_type           increase_pin;
  mmgsdi_len_type                attr_len; /*length in attribute */
  mmgsdi_len_type                data_len; /*length for data */
  uint8                          first_byte_data; /* Use this pointer to extract the rest of the Data of the EF */
}mmgsdi_psm_ef_data;

/* EPSLOCI, PSLOCI and LOCI data that is used for comparison with card EFs and pin_status */
typedef struct
{
  uint8                        loci_len;
  uint8                        loci_data[MMGSDI_LOCI_LEN];
  uint8                        psloci_len;
  uint8                        psloci_data[MMGSDI_PSLOCI_LEN];
  uint8                        epsloci_len;
  uint8                        epsloci_data[MMGSDI_EPSLOCI_LEN];
  mmgsdi_pin_status_enum_type  pin_status;
}mmgsdi_psm_local_cache_type;

/* To carry PSM data retrieved fom SFS */
static mmgsdi_psm_data_type        *mmgsdi_psm_data_ptr           = NULL;
static mmgsdi_psm_local_cache_type *mmgsdi_psm_local_cache_ptr    = NULL;

/* PSM mode is enabled or not */
static mmgsdi_psm_mode_enum_type    mmgsdi_psm_mode               = MMGSDI_PSM_MODE_DISABLED;

/* MMGSDI PSM PIN1 cache support and context restore after power cycle support
   enabled or not */
static uint8 mmgsdi_psm_nv73957_value                             = 0;

/* Indicates if PSM early init is done */
static boolean  mmgsdi_psm_early_init_done                        = FALSE;

/* This is to store the PSM READY IND status */
static boolean  mmgsdi_psm_ready_ind_pending                      = FALSE;
 
/* This variable tells if PSM init(from PSM cache init to receiving TP dl
    response) is in progress */
static boolean mmgsdi_psm_init_in_progress                    = FALSE;

/* USIM CACHE Elementary Files which are sensitive */
static mmgsdi_file_enum_type mmgsdi_psm_usim_sensitive_ef_cache_enums[]     =
{
  MMGSDI_USIM_IMSI,      MMGSDI_USIM_KEYS,   MMGSDI_USIM_KEYSPS,  MMGSDI_USIM_LOCI,     MMGSDI_USIM_PSLOCI,
  MMGSDI_USIM_KC,        MMGSDI_USIM_KCGPRS, MMGSDI_USIM_AD,      MMGSDI_USIM_EPSLOCI,  MMGSDI_USIM_EPSNSC,
  MMGSDI_USIM_HIDDENKEY, MMGSDI_USIM_UST,    MMGSDI_USIM_GID1,    MMGSDI_USIM_GID2,     MMGSDI_USIM_START_HFN
};

/* USIM CACHE Elementary files which are non sensitive */
static mmgsdi_file_enum_type mmgsdi_psm_usim_non_sensitive_ef_cache_enums[] =
{
  MMGSDI_USIM_HPLMN,                 MMGSDI_USIM_ACM_MAX,               MMGSDI_USIM_EST,
  MMGSDI_USIM_ACM,                   MMGSDI_USIM_SPN,                   MMGSDI_USIM_PUCT,
  MMGSDI_USIM_ACC,                   MMGSDI_USIM_EMLPP,                 MMGSDI_USIM_AAEM,
  MMGSDI_USIM_THRESHOLD,             MMGSDI_USIM_PLMNWACT,              MMGSDI_USIM_OPLMNWACT,
  MMGSDI_USIM_HPLMNWACT,             MMGSDI_USIM_RPLMNACT,              MMGSDI_USIM_FPLMN,
  MMGSDI_USIM_LI,                    MMGSDI_USIM_CBMID,                 MMGSDI_USIM_EHPLMN,
  MMGSDI_USIM_EHPLMNPI,              MMGSDI_USIM_LRPLMNSI,              MMGSDI_USIM_SPDI,
  MMGSDI_USIM_ECC,                   MMGSDI_USIM_NASCONFIG,             MMGSDI_USIM_7F66_PROP1_RAT,
  MMGSDI_USIM_7F66_PROP1_SPT_TABLE,  MMGSDI_USIM_7F66_PROP1_ACT_HPLMN,  MMGSDI_USIM_OPL,
  MMGSDI_USIM_PNN,                   MMGSDI_USIM_MSISDN,                MMGSDI_USIM_ACL
};

#ifdef FEATURE_MMGSDI_3GPP2
/* CSIM Cache Elementary files which are sensitive */
static mmgsdi_file_enum_type mmgsdi_psm_csim_sensitive_ef_cache_enums[]     =
{
  MMGSDI_CSIM_IMSI_M,        MMGSDI_CSIM_IMSI_T,      MMGSDI_CSIM_CST,   MMGSDI_CSIM_AD,
  MMGSDI_CSIM_RUIM_ID,       MMGSDI_CSIM_HIDDEN_KEY
};

/* CSIM Cache Elementary files which are non sensitive */
static mmgsdi_file_enum_type mmgsdi_psm_csim_non_sensitive_ef_cache_enums[] =
{
  MMGSDI_CSIM_LANG_IND,                  MMGSDI_CSIM_EST,                   MMGSDI_CSIM_SERVICE_PREF,
  MMGSDI_CSIM_ACCOLC,                    MMGSDI_CSIM_PRL,                   MMGSDI_CSIM_EPRL,   
  MMGSDI_CSIM_MAXIMUM_PRL,               MMGSDI_CSIM_3GPD_MIPUPP,           MMGSDI_CSIM_ECC,
  MMGSDI_CSIM_SPN,                       MMGSDI_CSIM_3GPD_SIPUPP,           MMGSDI_CSIM_HRPDUPP,
  MMGSDI_CSIM_SF_EUIM_ID,                MMGSDI_CSIM_NAM_LOCK,              MMGSDI_CSIM_CDMA_HOME_SID_NID,
  MMGSDI_CSIM_CDMA_ZONE_BASED_REGN_IND,  MMGSDI_CSIM_3GPD_SIPPAPSS,         MMGSDI_CSIM_CDMA_DIST_BASED_REGN_IND,
  MMGSDI_CSIM_CDMA_SYS_REGN_IND,         MMGSDI_CSIM_OTAPA_SPC_ENABLE,      MMGSDI_CSIM_3GPD_MIPSP,
  MMGSDI_CSIM_3GPD_MIPFLAGS,             MMGSDI_CSIM_CALL_TERM_MODE_PREF,   MMGSDI_CSIM_3GPD_3GPDOPM
};

/* Common Cache Elements */
static mmgsdi_file_enum_type mmgsdi_psm_common_cache_enums[] =
{
  MMGSDI_TELECOM_MLPL, MMGSDI_TELECOM_MSPL, MMGSDI_TELECOM_MMSSMODE
};
#endif /* FEATURE_MMGSDI_3GPP2 */


/*===========================================================================
FUNCTION MMGSDI_PSM_READ_PSM_CONFIGURATION

DESCRIPTION
  To read PSM configuration.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_psm_mode_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_psm_read_psm_configuration(void)
{
  uint8                     nv_data[8]     = {0};
  mmgsdi_psm_mode_enum_type psm_mode       = MMGSDI_PSM_MODE_DISABLED;

  /* Reset PSM mode to disabled */
  mmgsdi_psm_mode = MMGSDI_PSM_MODE_DISABLED;

  if(mmgsdi_generic_data_ptr == NULL ||
     mmgsdi_generic_data_ptr->num_slots != 1)
  {
    return;
  }

  /* First check MMODE PSM NV is enabled on not */
  if (uim_common_efs_read(UIM_COMMON_EFS_NON_UIM_PSM_MODE,
                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                          UIM_COMMON_EFS_DEVICE,
                          (uint8 *)nv_data,
                          sizeof(nv_data)) !=
      UIM_COMMON_EFS_SUCCESS)
  {
    UIM_MSG_LOW_0("MMODE PSM NV is not set");
    return;
  }

  /* Second byte of NV denotes feature status */
  if(nv_data[1] == 0x00)
  {
    UIM_MSG_LOW_0("MMODE PSM NV is not enabled");
    return;
  }

  /* Check UIM PSM NV is enabled on not
     If UIM PSM EFS is not configured - default the behavior to early events */
  if (uim_common_efs_read(UIM_COMMON_EFS_MMGSDI_PSM,
                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                          UIM_COMMON_EFS_DEVICE,
                          (uint8 *)&psm_mode,
                          sizeof(psm_mode)) ==
      UIM_COMMON_EFS_SUCCESS)
  {
    if(psm_mode == MMGSDI_PSM_MODE_DISABLED)
    {
      UIM_MSG_LOW_0("UIM PSM NV is not enabled");
      return;
    }
  }

  /* Set PSM mode, PSM early events mode is the default settings */
  mmgsdi_psm_mode = (psm_mode == MMGSDI_PSM_MODE_CARD_DETECTED) ?
                        MMGSDI_PSM_MODE_CARD_DETECTED : MMGSDI_PSM_MODE_EARLY_EVTS;
}/* mmgsdi_psm_read_psm_configuration */


/*===========================================================================
FUNCTION  MMGSDI_PSM_DELETE_EXISTING_SFS_OR_EFS_file

DESCRIPTION
  This function deletes the input file it it exists

PARAMETERS
  file_path  : Contains path of the input file
  use_sfs    : Indicates whether the input file is from sfs or efs

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_delete_existing_sfs_or_efs_file
(
  const char *file_path,
  boolean     use_sfs
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;

  /* Delete if any existing file is present */
#ifndef FEATURE_UIM_TEST_FRAMEWORK
  if(use_sfs)
  {
    mmgsdi_status = mmgsdi_sfs_rmfile(file_path);
  }
  else
  {
    mmgsdi_status = gsdi_efs_delete_file(file_path,
                                         UIM_COMMON_EFS_DEVICE);
  }
#else
  #error code not present
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */

  return mmgsdi_status; 
}/*mmgsdi_psm_delete_existing_sfs_or_efs_file*/


/*===========================================================================
FUNCTION MMGSDI_PSM_PURGE_DATA

DESCRIPTION
  To purge the data in EFS and to free heap memory occupied by PSM data

PARAMETERS
  is_complete_delete : It indicates whether to delete all PSM files from file system
                       or delete only the uicc profile file from file system

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_psm_purge_data
(
  boolean is_complete_delete
)
{
  UIM_MSG_HIGH_0("mmgsdi_psm_purge_data");

  /*Free global PSM data ptr if not already freed */
  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_psm_data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_psm_local_cache_ptr);

  /* Delete PSM UICC proflle file and sensitive EF data file from SFS along with
     non sensitive EF data file from EFS*/
  (void)mmgsdi_psm_delete_existing_sfs_or_efs_file(mmgsdi_sfs_psm_uicc_profile,
                                                   TRUE);
  if(!is_complete_delete)
  {
    /* As complete delete is not requested, we no need to go further*/
    return;
  }

  (void)mmgsdi_psm_delete_existing_sfs_or_efs_file(mmgsdi_sfs_psm_usim_sensitive_ef_data,
                                                   TRUE);
  (void)mmgsdi_psm_delete_existing_sfs_or_efs_file(mmgsdi_efs_psm_usim_non_sensitive_ef_data,
                                                   FALSE);

#ifdef FEATURE_MMGSDI_3GPP2
  (void)mmgsdi_psm_delete_existing_sfs_or_efs_file(mmgsdi_sfs_psm_csim_sensitive_ef_data,
                                                   TRUE);
  (void)mmgsdi_psm_delete_existing_sfs_or_efs_file(mmgsdi_efs_psm_csim_non_sensitive_ef_data,
                                                   FALSE);
#endif /* FEATURE_MMGSDI_3GPP2 */
}/* mmgsdi_psm_purge_data */


/*===========================================================================
FUNCTION MMGSDI_PSM_POPULATE_OPL_PNN_VALID_RECORD_INFO

DESCRIPTION
  To populate valid record info for EF-OPL and EF-PNN

PARAMETERS
  session_id : session id
  ef_data_ptr : address of buffer which contains EFs' data and attribute
  ef_data_len : length of buffer passed
  ef_cache_data_ptr : file psm cache

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_populate_opl_pnn_valid_record_info
(
  mmgsdi_session_id_type    session_id,
  uint8                    *ef_data_ptr,
  uint32                    ef_data_len,
  mmgsdi_psm_ef_data       *ef_cache_data_ptr,
  boolean                   is_all_valid_opl_rec_cached
)
{
  mmgsdi_return_enum_type           mmgsdi_status           = MMGSDI_SUCCESS;
  mmgsdi_psm_ef_data                ef_opl_pnn_data;

  memset(&ef_opl_pnn_data, 0x00, sizeof(mmgsdi_psm_ef_data));

  if(ef_cache_data_ptr == NULL ||
     ef_data_ptr == NULL ||
     ef_data_len == 0 ||
     (ef_cache_data_ptr->file_enum != MMGSDI_USIM_OPL &&
      ef_cache_data_ptr->file_enum != MMGSDI_USIM_PNN))
  {
    return MMGSDI_ERROR;
  }

  /* If the list of valid records is already populated, it means that it was
     already restored from the SFS, so nothing else to do at this point and we
     can directly return */
  if(mmgsdi_eons_opl_pnn_is_valid_record_list_updated(session_id, 
                                                      ef_cache_data_ptr->file_enum,
                                                      NULL,
                                                      NULL))
  {
    return MMGSDI_SUCCESS;
  }

  mmgsdi_status = mmgsdi_eons_opl_pnn_update_file_attr(session_id,
                                                       ef_cache_data_ptr->file_enum,
                                                       ef_cache_data_ptr->num_of_rec,
                                                       ef_cache_data_ptr->data_len,
                                                       MMGSDI_SUCCESS);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* Only valid record list for EF-OPL is stored in PSM EFS, we create valid
     record list for EF-PNN from cached EF-OPL records. */
  if(ef_cache_data_ptr->file_enum == MMGSDI_USIM_OPL)
  {
    /* Use valid record list present in PSM EFS data to create invalid record
       list. */
    uint8            index               = 0;
    uint8            valid_index         = 0;
    mmgsdi_data_type invalid_record_data = {0, NULL};

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(invalid_record_data.data_ptr,
                                       sizeof(uint8) * MMGSDI_MAX_RECS_IN_LINEAR_FIXED_EF);

    if(invalid_record_data.data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }

    for(index = 1; index <= ef_cache_data_ptr->num_of_rec &&
                   index <= MMGSDI_MAX_RECS_IN_LINEAR_FIXED_EF &&
                   invalid_record_data.data_len < MMGSDI_MAX_RECS_IN_LINEAR_FIXED_EF; index++)
    {
      if(index == mmgsdi_psm_data_ptr->opl_record.valid_rec_list[valid_index] &&
         valid_index < mmgsdi_psm_data_ptr->opl_record.num_of_valid_rec)
      {
        valid_index++;
      }
      else
      {
        invalid_record_data.data_ptr[invalid_record_data.data_len] = index;
        invalid_record_data.data_len++;
      }
    }

    mmgsdi_status = mmgsdi_eons_opl_update_valid_record_info(session_id,
                                                             MMGSDI_USIM_OPL,
                                                             invalid_record_data,
                                                             MMGSDI_SUCCESS);
    MMGSDIUTIL_TMC_MEM_FREE(invalid_record_data.data_ptr);
  }
  else if(is_all_valid_opl_rec_cached)
  {
    /* Invoke this function only if we have all valid EF-OPL record cached.
       If we don't have all valid record cached, we can't determine valid PNN
       records to be cached. */
    mmgsdi_status = mmgsdi_eons_pnn_update_valid_record_info(session_id);
  }

  return mmgsdi_status;
}/* mmgsdi_psm_populate_opl_pnn_valid_record_info */


/*===========================================================================
FUNCTION MMGSDI_PSM_BUILD_EF_DATA

DESCRIPTION
  To create EF cache from stored PSM data

PARAMETERS
  ef_data_ptr : address of buffer which contains EFs' data and attribute
  ef_data_len : length of buffer passed

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_build_ef_data
(
  uint8                    *ef_data_ptr,
  uint32                    ef_data_len
)
{
  mmgsdi_return_enum_type          mmgsdi_status                 = MMGSDI_SUCCESS;
  static uint8                     num_of_valid_opl_rec_in_cache = 0;

  if(ef_data_ptr == NULL ||
     ef_data_len == 0)
  {
    return MMGSDI_ERROR;
  }

  while(ef_data_len >= sizeof(mmgsdi_psm_ef_data))
  {
    mmgsdi_file_structure_enum_type  file_type        = MMGSDI_MAX_FILE_STRUCTURE_ENUM;
    mmgsdi_file_size_type            file_size        = 0;
    mmgsdi_session_type_enum_type    session_type     = MMGSDI_MAX_SESSION_TYPE_ENUM;
    mmgsdi_session_id_type           session_id       = MMGSDI_SESSION_ID_ZERO;
    mmgsdi_app_enum_type             app_type         = MMGSDI_APP_NONE;
    mmgsdi_psm_ef_data               ef_data;
    mmgsdi_access_type               file_access;
    mmgsdi_file_security_access_type security_access;
    mmgsdi_data_type                 file_data;

    memset(&ef_data, 0x00, sizeof(mmgsdi_psm_ef_data));
    memset(&file_access, 0x00, sizeof(mmgsdi_access_type));
    memset(&security_access, 0x00, sizeof(mmgsdi_file_security_access_type));
    memset(&file_data, 0x00, sizeof(mmgsdi_data_type));

    mmgsdi_memscpy(&ef_data, sizeof(mmgsdi_psm_ef_data),
                   ef_data_ptr, sizeof(mmgsdi_psm_ef_data));

    if(ef_data.data_len + sizeof(mmgsdi_psm_ef_data) - sizeof(uint8) > ef_data_len)
    {
      UIM_MSG_ERR_0("buffer data length is psm ef data + data len");
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* Get session type based on file enum type */
    if(MMGSDI_PSM_GET_FILE_ENUM_START_RANGE(ef_data.file_enum) == MMGSDI_USIM_EF_START ||
       MMGSDI_PSM_GET_FILE_ENUM_START_RANGE(ef_data.file_enum) == MMGSDI_USIM_GSM_ACCESS_DF_START ||
       MMGSDI_PSM_GET_FILE_ENUM_START_RANGE(ef_data.file_enum) == MMGSDI_CINGULAR_USIM_NETWORK_DF_START ||
       MMGSDI_PSM_GET_FILE_ENUM_START_RANGE(ef_data.file_enum) == MMGSDI_CINGULAR_USIM_EF_START)
    {
      session_type = MMGSDI_GW_PROV_PRI_SESSION;
      app_type     = MMGSDI_APP_USIM;
      session_id   = mmgsdi_generic_data_ptr->pri_gw_session_id;
    }
    else if(MMGSDI_PSM_GET_FILE_ENUM_START_RANGE(ef_data.file_enum) == MMGSDI_CSIM_EF_START)
    {
      session_type = MMGSDI_1X_PROV_PRI_SESSION;
      app_type     = MMGSDI_APP_CSIM;
      session_id   = mmgsdi_generic_data_ptr->pri_1x_session_id;
    }
    else if(ef_data.file_enum == MMGSDI_TELECOM_MLPL ||
            ef_data.file_enum == MMGSDI_TELECOM_MSPL ||
            ef_data.file_enum == MMGSDI_TELECOM_MMSSMODE)
    {
      session_type = MMGSDI_CARD_SESSION_SLOT_1;
      app_type     = MMGSDI_APP_USIM;
      session_id   = mmgsdi_generic_data_ptr->slot_session_id[0];
    }
    else
    {
      UIM_MSG_HIGH_1("Error in parsing for file enum 0x%x ", ef_data.file_enum);
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    if(ef_data.num_of_rec)
    {
      file_type = MMGSDI_LINEAR_FIXED_FILE;
      file_size = ef_data.num_of_rec * ef_data.data_len;
    }
    else
    {
      file_type = MMGSDI_TRANSPARENT_FILE;
      file_size = ef_data.data_len;
    }

    file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
    file_access.file.file_enum = ef_data.file_enum;

    /* If read_protetion is not initialized value means attributes are valid.
       In case of record based EF we will have attributes valid for first record only */
    if(ef_data.read_protection != MMGSDI_MAX_FILE_SECURITY_ENUM)
    {
      security_access.read.protection_method = ef_data.read_protection;

      /* If protection method is other that "Always" and "Allowed", it would be
         "Single Allowed" because we only store one PIN in PSM data for ease of
         caching */
      if(security_access.read.protection_method != MMGSDI_ALWAYS_ALLOWED &&
         security_access.read.protection_method != MMGSDI_NEVER_ALLOWED)
      {
        security_access.read.num_protection_pin = 1;
        security_access.read.protection_pin_ptr = &ef_data.read_pin;
      }

      security_access.write.protection_method = ef_data.write_protection;
      if(security_access.write.protection_method != MMGSDI_ALWAYS_ALLOWED &&
         security_access.write.protection_method != MMGSDI_NEVER_ALLOWED)
      {
        security_access.write.num_protection_pin = 1;
        security_access.write.protection_pin_ptr = &ef_data.write_pin;
      }

      security_access.increase.protection_method = ef_data.increase_protection;
      if(security_access.increase.protection_method != MMGSDI_ALWAYS_ALLOWED &&
         security_access.increase.protection_method != MMGSDI_NEVER_ALLOWED)
      {
        security_access.increase.num_protection_pin = 1;
       security_access.increase.protection_pin_ptr = &ef_data.increase_pin;
      }

      /* Update attribute cache */
      mmgsdi_status = mmgsdi_cache_attr_write(
                        session_id,
                        MMGSDI_SLOT_1,
                        &file_access,
                        file_type,
                        file_size,
                        ef_data.attr_len,
                        ef_data.num_of_rec,
                        ef_data.sfi,
                        &security_access,
                        NULL);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_2("Not able to write attr for EF 0x%x app 0x%x",
                      file_access.file.file_enum, app_type);
        break;
      }
    }

    if(ef_data.file_enum == MMGSDI_USIM_OPL ||
       ef_data.file_enum == MMGSDI_USIM_PNN)
    {
      boolean is_all_valid_opl_rec_cached = FALSE;

      /* Check if all valid OPL records are cached from PSM EFS data. If there
         is no valid record in EF-OPL, set the flag to TRUE so that we can
         continue with retrieval of EF-PNN valid record index. */
      if((num_of_valid_opl_rec_in_cache == mmgsdi_psm_data_ptr->opl_record.num_of_valid_rec &&
          mmgsdi_psm_data_ptr->opl_record.num_of_valid_rec > 0) ||
          mmgsdi_psm_data_ptr->opl_record.num_of_valid_rec == 0)
      {
        is_all_valid_opl_rec_cached = TRUE;
      }

      /* Along with record data, MMGSDI also maintains valid record index for
         EF-OPL and EF-PNN.
         This is technically required only for the first OPL or PNN record,
         but that check is done in that function itself.*/
      mmgsdi_status = mmgsdi_psm_populate_opl_pnn_valid_record_info(
                        session_id,
                        ef_data_ptr,
                        ef_data_len,
                        &ef_data,
                        is_all_valid_opl_rec_cached);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        break;
      }
    }


    /* Update EF cache. It is possible that record data is not present in SFS,
       only attributes are there; skip cache write in that case. */
    if(ef_data.file_enum == MMGSDI_USIM_OPL ||
       ef_data.file_enum == MMGSDI_USIM_PNN)
    {
      /* MMGSDI maintains separate caches for EF-OPL and EF-PNN because only
         valid records are read from card for EF-OPL; for EF-PNN, records
         which are referred in EF-OPL records are cached. */
      if(ef_data.rec_num != 0)
      {
        mmgsdi_data_type record_data = {0, NULL};

        record_data.data_len = ef_data.data_len;
        record_data.data_ptr = &((mmgsdi_psm_ef_data *)ef_data_ptr)->first_byte_data;

        mmgsdi_status = mmgsdi_eons_opl_pnn_update_record(session_id,
                                                          ef_data.file_enum,
                                                          record_data,
                                                          ef_data.rec_num,
                                                          MMGSDI_CACHE_INIT);
        if(ef_data.file_enum == MMGSDI_USIM_OPL &&
           ef_data.rec_num == mmgsdi_psm_data_ptr->opl_record.valid_rec_list[num_of_valid_opl_rec_in_cache])
        {
          num_of_valid_opl_rec_in_cache++;
        }

        /* Reset mmgsdi_status to SUCCESS as update record function will return
           ERROR if there is no valid record list*/
        mmgsdi_status = MMGSDI_SUCCESS;
      }
    }
    else
    {
      /* Update EF cache */
      mmgsdi_status = mmgsdi_cache_write_item(ef_data.file_enum,
                                              app_type,
                                              session_type,
                                              ef_data.data_len,
                                              0,
                                              &((mmgsdi_psm_ef_data *)ef_data_ptr)->first_byte_data,
                                              MMGSDI_DATA_FROM_CARD_COMPLETE);

      /* EF-ECC is a linear fixed file. mmgsdi_cache_write_item does not support
         caching of multiple records of record based EFs; it can only cache one record */
      if(ef_data.file_enum == MMGSDI_USIM_ECC)
      {
        file_data.data_len = ef_data.data_len / ef_data.num_of_rec;
        file_data.data_ptr = &((mmgsdi_psm_ef_data *)ef_data_ptr)->first_byte_data;

      /* Cache the first record read so that it can be used later for
         getting ECC record length when sending out the ECC event */
        mmgsdi_status = mmgsdi_cache_add_record(session_id,
                                   file_access,
                                   1,
                                   &file_data);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          UIM_MSG_ERR_0("Unable to cache USIM ECC record no 1");
          break;
        }

        file_data.data_len = ef_data.data_len;

        mmgsdi_status = mmgsdi_cache_create_usim_ecc_cache_psm_mode(
                          session_type,
                          ef_data.num_of_rec,
                          &file_data);
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          UIM_MSG_ERR_0("Unable to update num of records in USIM ECC cache");
          break;
        }
      }
    }

    /* Move to address where we store EF data by adding size of
       mmgsdi_psm_ef_data to ef_data_ptr. Note that
       sizeof(uint8) is substracted because first byte of EF data is part of
       structre mmgsdi_psm_ef_data*/
    ef_data_ptr += sizeof(mmgsdi_psm_ef_data) - sizeof(uint8); 
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_2("Not able to write cache for EF 0x%x app 0x%x",
                    file_access.file.file_enum, app_type);
      break;
    }

    /* To Move to address of next file data, add EF len to ef_data_ptr. Remaining
       length is decreased by size of structure and EF data len. Note that
       sizeof(uint8) is added back to remaining length because first byte of
       EF data is already part of structre mmgsdi_psm_ef_data */
    ef_data_len = ef_data_len - ef_data.data_len - sizeof(mmgsdi_psm_ef_data) + sizeof(uint8);
    ef_data_ptr += ef_data.data_len;    
  }

  return mmgsdi_status;
}/* mmgsdi_psm_build_ef_data */


/*===========================================================================
FUNCTION MMGSDI_PSM_BUILD_APP_DATA

DESCRIPTION
  To build application data from stored PSM data

PARAMETERS
  session_id   : session id to build app data
  app_data_ptr : Application data
  pin_status   : pin status
  pin_data     : pin data
  app_cap      : app capability

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_build_app_data
(
  mmgsdi_session_id_type       session_id, 
  mmgsdi_aid_type             *app_data_ptr,
  mmgsdi_pin_status_enum_type  pin_status,
  mmgsdi_static_data_type      pin_data,
  mmgsdi_app_capabilities_type app_cap
)
{
  mmgsdi_return_enum_type    mmgsdi_status       = MMGSDI_ERROR;
  mmgsdi_session_info_type  *session_info_ptr    = NULL;
  int32                      channel_index       = 0;
  mmgsdi_slot_data_type     *slot_data_ptr       = NULL;
  int32                      index_session       = 0;
  int32                      index_client        = 0;
  uint8                      app_index           = 0;
  mmgsdi_app_pin_info_type **app_pin1_tbl_pptr   = NULL;
  uint8                      app_pin1_tbl_size   = 0;
  uint32                     pin_index           = 0;

  if(app_data_ptr == NULL ||
     (app_data_ptr->app_type != MMGSDI_APP_CSIM &&
      app_data_ptr->app_type != MMGSDI_APP_USIM))
  {
    return MMGSDI_ERROR;
  }

  UIM_MSG_HIGH_0("mmgsdi_psm_build_app_data");

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(MMGSDI_SLOT_1);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if(MMGSDI_SUCCESS != mmgsdi_util_get_client_and_session_index(session_id,
                                                                &index_client,
                                                                &index_session))
  {
    return MMGSDI_ERROR;
  }

  if(mmgsdi_client_id_reg_table[index_client] == NULL)
  {
    return MMGSDI_ERROR;
  }

  session_info_ptr = mmgsdi_client_id_reg_table[index_client]->session_info_ptr[index_session];
  if(session_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* App specific EF cache and attribute cache gets populated here */
  do
  {
    uint8  channel_info_index = MMGSDI_MAX_CHANNEL_INFO;
    uint8  i                  = 0;
    uint8  j                  = 0;

    mmgsdi_status = mmgsdi_util_get_free_channel_index(&channel_index);
    if((mmgsdi_status != MMGSDI_SUCCESS) ||
       (channel_index >= MMGSDI_MAX_CHANNEL_INFO) ||
       (mmgsdi_channel_info_ptr_table[channel_index] != NULL))
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_channel_info_ptr_table[channel_index],
                                       sizeof(mmgsdi_channel_info_type));

    if (mmgsdi_channel_info_ptr_table[channel_index] == NULL)
    {
      mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      break;
    }

    /* Wipe off any data that exists at the index in the table */
    memset(mmgsdi_channel_info_ptr_table[channel_index], 0x00,
           sizeof(mmgsdi_channel_info_type));

    if(channel_index > 0)
    {
      mmgsdi_channel_info_ptr_table[channel_index]->channel_id = 1;
    }
    else
    {
      mmgsdi_channel_info_ptr_table[channel_index]->channel_id = MMGSDI_DEFAULT_CHANNEL;
    }

    mmgsdi_channel_info_ptr_table[channel_index]->slot_id = MMGSDI_SLOT_1;
    slot_data_ptr->available_logical_channels--;

    mmgsdi_channel_info_ptr_table[channel_index]->num_connections++;

    /* Map the new channel table entry with the session */
    session_info_ptr->channel_info_index = (uint8)channel_index;

    if (MMGSDI_SUCCESS != mmgsdi_session_activate_app_info(session_info_ptr->session_id,
                                                           app_data_ptr->aid))
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* Set application status */
    mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_selected =
      TRUE;
    mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_state =
      MMGSDI_APP_STATE_DETECTED;
    mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->perso_state =
      MMGSDI_SIMLOCK_VERIFIED;
    mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->pre_perso_files_cached = TRUE;

    (void)mmgsdi_cache_create_prov_file_cache_clear_caches(
            session_info_ptr->session_id,
            TRUE);

    /* As we don't have AID select response we can't update ADF attribute and PIN
       info in app_info_ptr */
    app_pin1_tbl_pptr = mmgsdi_util_get_app_pin1_info_tbl_ptr(&app_pin1_tbl_size,
                                                              MMGSDI_SLOT_1);

    if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr != NULL &&
       app_pin1_tbl_pptr != NULL)
    {
      /* Indicate that PIN data for the app was not extracted from FCP */
      mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->fcp_pin_data_absent = TRUE;
      
      /* No app has been selected on the default channel yet so setting key ref
         for PIN1 as MMGSDI_PIN_APPL_1 and PIN2 as MMGSDI_SEC_PIN_APPL_1
         respectively */

      /* Ignoring return type since app pin table has only 8 entries and
         one of them will match MMGSDI_PIN_APPL_1 key ref */
      (void)mmgsdi_util_find_app_pin_index((const mmgsdi_app_pin_info_type **)app_pin1_tbl_pptr,
                                           app_pin1_tbl_size,
                                           MMGSDI_PIN_APPL_1,
                                           &pin_index);
      mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->pin1_ptr = app_pin1_tbl_pptr[pin_index];
      if(mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->pin1_ptr != NULL)
      {
        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->pin1_ptr->status = pin_status;
        mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->pin1_ptr->pin_data = pin_data;
      }

      memset((void *)&mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->pin2,
             0x0, sizeof(mmgsdi_app_pin_info_type));
      mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->pin2.pin_id = MMGSDI_PIN2;
      mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->pin2.key_ref = MMGSDI_SEC_PIN_APPL_1;
      mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->pin2.status =
        MMGSDI_PIN_DISABLED;
      mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_capabilities = app_cap;
    }

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }
    /* Map channel index information to the session pointer
       of other clients */
    for(i = 0; i < MMGSDI_MAX_CLIENT_INFO; i++)
    {
      if (mmgsdi_client_id_reg_table[i] == NULL)
      {
        continue;
      }
      for(j = 0; j < MMGSDI_MAX_SESSION_INFO; j++)
      {
        if (mmgsdi_client_id_reg_table[i]->session_info_ptr[j] == NULL)
        {
          continue;
        }
  
        channel_info_index = mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->channel_info_index;
        if((session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
           (channel_info_index != session_info_ptr->channel_info_index))
        {
  
          if(/* Update channel info for sessions of provisioning type */
             (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
                session_info_ptr->session_type)
                                               ||
              /* Update channel info for sessions of card slot1 type */
             ((mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id == MMGSDI_SLOT_1) &&
              (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
                MMGSDI_CARD_SESSION_SLOT_1) &&
              (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_id ==
                MMGSDI_DEFAULT_CHANNEL))
                                               ||
              /* Update channel info for sessions of card slot2 type */
             ((mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id == MMGSDI_SLOT_2) &&
              (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
                MMGSDI_CARD_SESSION_SLOT_2) &&
              (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_id ==
                MMGSDI_DEFAULT_CHANNEL))
                                               ||
              /* Update channel info for sessions of card slot3 type */
             ((mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->slot_id == MMGSDI_SLOT_3) &&
              (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
                MMGSDI_CARD_SESSION_SLOT_3) &&
              (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->channel_id ==
                MMGSDI_DEFAULT_CHANNEL)))
          {
            UIM_MSG_HIGH_1("Channel mapping updated for session_id=0x%x",
                           mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_id);
            /* We are doing this to to handle remapping request. */
            if(( channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
               ( mmgsdi_channel_info_ptr_table[channel_info_index] != NULL) &&
               ( mmgsdi_channel_info_ptr_table[channel_info_index]->num_connections > 0))
            {
              mmgsdi_channel_info_ptr_table[channel_info_index]->num_connections--;
            }
            mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->num_connections++;
            mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->channel_info_index =
              session_info_ptr->channel_info_index;
          }
        }
      } /* for j<MAX_SESSION_INFO */
    }/* i<MMGSDI_MAX_CLIENT_INFO */
  }
  while(0);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  if(MMGSDI_SUCCESS != mmgsdi_util_get_app_index_for_app(app_data_ptr,
                                                         MMGSDI_SLOT_1,
                                                         &app_index))
  {
    return MMGSDI_ERROR;
  }

  if(session_info_ptr->session_type == MMGSDI_GW_PROV_PRI_SESSION)
  {
    slot_data_ptr->prov_app_indices[MMGSDI_PRI_GW_PROV_APP_SEL_INDEX] =
      MMGSDI_PROV_INDEX_VALIDITY_MASK | app_index;
  }
  else 
  {
    slot_data_ptr->prov_app_indices[MMGSDI_PRI_1X_PROV_APP_SEL_INDEX] = 
      MMGSDI_PROV_INDEX_VALIDITY_MASK | app_index;
  }

  return MMGSDI_SUCCESS;
}/* mmgsdi_psm_build_app_data */


/*===========================================================================
FUNCTION MMGSDI_PSM_BUILD_SLOT_DATA

DESCRIPTION
  To build slot data from stored PSM data

PARAMETERS
  psm_data_ptr : stored PSM data pointer
  slot_id      : slot id

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_build_slot_data
(
  mmgsdi_psm_data_type     *psm_data_ptr
)
{
  mmgsdi_return_enum_type           mmgsdi_status     = MMGSDI_SUCCESS;
  mmgsdi_session_id_type            card_session_id   = MMGSDI_SESSION_ID_ZERO;
  mmgsdi_int_app_info_type        **app_info_pptr     = NULL;
  mmgsdi_key_ref_values_enum_type   slot_pin1_key_ref = MMGSDI_PIN_APPL_1;
  uint8                             pin1_index        = 0;  
  uint32                            num_apps          = 0;
  uint8                             index             = 0;
  mmgsdi_data_type                  file_data;
  mmgsdi_access_type                access_info;

  /* --------------------------------------------------------------------------
     Validate the Input Parameters.
     ----------------------------------------------------------------------- */
  if(psm_data_ptr == NULL )
  {
    return MMGSDI_ERROR;
  }

  UIM_MSG_HIGH_0("mmgsdi_psm_build_slot_data");

  memset(&file_data, 0x00, sizeof(mmgsdi_data_type));
  memset(&access_info, 0x00, sizeof(mmgsdi_access_type));

  /* Allocating memory  for Slot data*/
  if(mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX] == NULL)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX],
                                       sizeof(mmgsdi_slot_data_type));

    if(mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX] == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
  }

  /* Clear the Slot specific MMGSDI data */
  mmgsdi_util_preinit_mmgsdi(MMGSDI_SESSION_ID_ZERO,
                             MMGSDI_APP_NONE,
                             MMGSDI_REFRESH_RESET,
                             MMGSDI_SLOT_1,
                             FALSE);

  /* --------------------------------------------------------------------------
  Populate card info to MMGSDI slot data
  ----------------------------------------------------------------------- */

  /* Allocating memory for all index in app pin1 table ptr for slot 1 */
  for(pin1_index = 0; pin1_index < MMGSDI_MAX_PIN1_INFO; pin1_index++)
  {
    if(mmgsdi_app_pin1_table_ptr[MMGSDI_SLOT_1_INDEX][pin1_index] != NULL)
    {
      MMGSDI_MSG_ERROR_2("PIN1 table is not NULL, slot index 0x%x, pin1 index 0x%x",
                         MMGSDI_SLOT_1_INDEX, pin1_index);
      return MMGSDI_ERROR;
    }

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      mmgsdi_app_pin1_table_ptr[MMGSDI_SLOT_1_INDEX][pin1_index],
      sizeof(mmgsdi_app_pin_info_type));

    if(mmgsdi_app_pin1_table_ptr[MMGSDI_SLOT_1_INDEX][pin1_index] == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
    mmgsdi_app_pin1_table_ptr[MMGSDI_SLOT_1_INDEX][pin1_index]->pin_id  = MMGSDI_PIN1;
    mmgsdi_app_pin1_table_ptr[MMGSDI_SLOT_1_INDEX][pin1_index]->key_ref = slot_pin1_key_ref++;
  }

  /* Allocating memory for Univ PIN*/
  if(mmgsdi_app_univ_pin_table_ptr[MMGSDI_SLOT_1_INDEX] != NULL)
  {
    MMGSDI_MSG_ERROR_1("Univ PIN table is not NULL, slot index 0x%x",
                       MMGSDI_SLOT_1_INDEX);
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
    mmgsdi_app_univ_pin_table_ptr[MMGSDI_SLOT_1_INDEX],
    sizeof(mmgsdi_app_pin_info_type));

  if (mmgsdi_app_univ_pin_table_ptr[MMGSDI_SLOT_1_INDEX] == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  mmgsdi_app_univ_pin_table_ptr[MMGSDI_SLOT_1_INDEX]->pin_id  = MMGSDI_UNIVERSAL_PIN;
  mmgsdi_app_univ_pin_table_ptr[MMGSDI_SLOT_1_INDEX]->key_ref = MMGSDI_UNIVERSAL_PIN_KEY;

  mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->mmgsdi_state = MMGSDI_STATE_NOT_INIT;

  /* Copy the card protocol */
  mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->protocol = psm_data_ptr->card_type;

  app_info_pptr = mmgsdi_util_get_app_pin_info_tbl_ptr(MMGSDI_SLOT_1);

  /* Assigning app_info_ptr to App_PIN table */
  mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->app_info_pptr = app_info_pptr;

  /* Get the Card Slot Session ID corresponding to the slot */
  card_session_id = mmgsdi_generic_data_ptr->slot_session_id[MMGSDI_SLOT_1_INDEX];

  /* Retrieve the ICCD */
  memset(&access_info, 0x00, sizeof(mmgsdi_access_type));
  access_info.access_method = MMGSDI_EF_ENUM_ACCESS;
  access_info.file.file_enum = MMGSDI_ICCID;

  file_data.data_len = psm_data_ptr->iccid_data.length;
  file_data.data_ptr = psm_data_ptr->iccid_data.data;

  /* Copy the ICCID to the common cache */
  mmgsdi_status = mmgsdi_cache_write(card_session_id,
                                     &access_info,
                                     file_data,
                                     0,
                                     MMGSDI_DATA_FROM_CARD_COMPLETE);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->mmgsdi_rpm_iccid_has_changed = FALSE;

  /* Set UIM proactive to FALSE here, it will be set to TRUE once UICC mode of
     operation is identified */
  (void)uim_set_proactive_uim_slot(FALSE, UIM_SLOT_1);

  for(index = 0; index < sizeof(psm_data_ptr->app_data) / sizeof(psm_data_ptr->app_data[0]); index++)
  {
    mmgsdi_key_ref_values_enum_type slot_pin2_key_ref = MMGSDI_SEC_PIN_APPL_1;
    if(psm_data_ptr->app_data[index].app_type == MMGSDI_APP_NONE)
    {
      continue;
    }

    mmgsdi_status = mmgsdi_card_init_populate_app_pin_table(
                      MMGSDI_SLOT_1,
                      num_apps,
                      slot_pin2_key_ref);

    /* PIN2 key reference is incremented to point to next PIN2 for
       assigning this to next app */
    slot_pin2_key_ref++;

    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      return MMGSDI_ERROR;
    }

    mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->app_info_pptr[num_apps]->app_data.aid.data_len =
      psm_data_ptr->app_data[index].aid.data_len;

    /* Copy the AID into the global data structure which holds the AID data */
    mmgsdi_memscpy(mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->app_info_pptr[num_apps]->app_data.aid.data_ptr,
      sizeof(mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->app_info_pptr[num_apps]->app_data.aid.data_ptr),
      psm_data_ptr->app_data[index].aid.data_ptr,
      psm_data_ptr->app_data[index].aid.data_len);

    mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->app_info_pptr[num_apps]->app_selected = FALSE;
    mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->app_info_pptr[num_apps]->app_data.app_type =
      psm_data_ptr->app_data[index].app_type;
    mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->app_info_pptr[num_apps]->proprietary_app = FALSE;
    num_apps++;
  }  

  /* If there are valid UICC apps found... */
  if(num_apps > 0)
  {
    UIM_MSG_HIGH_1("No. of Apps present in UICC is 0x%x", num_apps);

    mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->card_apps_available |= MMGSDI_USIM_APP_MASK;

#ifdef FEATURE_MMGSDI_3GPP2
    if(mmgsdi_util_get_csim_apps(MMGSDI_SLOT_1))
    {
      UIM_MSG_HIGH_0("CSIM app. present");
      mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->card_apps_available |= MMGSDI_CSIM_APP_MASK;
      nvruim_set_uim_dir_present(NVRUIM_CDMA_SUBSCRIP_PRESENT);
    }
#endif /* FEATURE_MMGSDI_3GPP2 */
  }

  /* Reset the card error condition to UNKNOWN as we got link established
    notification */
  mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]->card_error_condition =
    MMGSDI_CARD_ERR_UNKNOWN_ERROR;

 return MMGSDI_SUCCESS;
}/* mmgsdi_psm_build_slot_data */


/*===========================================================================
FUNCTION MMGSDI_PSM_SLOT_APP_EF_DATA_CLEANUP

DESCRIPTION
  To free and reset slot, app and EF cache if Early Init in PSM mode fails 

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_psm_slot_app_ef_data_cleanup(void)
{
  uint8 channel_index = 0;

  /* Clearing prov and common caches */
  mmgsdi_cache_clear_prov_type_cache_alloc(mmgsdi_generic_data_ptr->pri_1x_session_id);
  mmgsdi_cache_clear_prov_type_cache_alloc(mmgsdi_generic_data_ptr->pri_gw_session_id);
  mmgsdi_cache_delete_common_file_cache(MMGSDI_SLOT_1);

  /* Clearing the Common Cache Table Pointers */
  if(mmgsdi_slot1_common_cache_tbl_ptr != NULL)
  {
    mmgsdi_attr_cache_element_type * tmp_attr_element_ptr   = NULL;

    /* Delete all the File attribute cache elements */
    while(mmgsdi_slot1_common_cache_tbl_ptr->item_ptr != NULL)
    {
      tmp_attr_element_ptr = mmgsdi_slot1_common_cache_tbl_ptr->item_ptr->next_ptr;
      mmgsdi_cache_delete_file_attr_cache_item(&mmgsdi_slot1_common_cache_tbl_ptr->item_ptr);
      mmgsdi_slot1_common_cache_tbl_ptr->item_ptr = tmp_attr_element_ptr;
    }
  }

  /* This frees the Channel Info Pointer Table pointers */
  for (channel_index = 0;
       channel_index < sizeof(mmgsdi_channel_info_ptr_table)/
                       sizeof(mmgsdi_channel_info_ptr_table[0]);
       channel_index++)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_channel_info_ptr_table[channel_index]);
  }

  /* Reset all the app related data */
  mmgsdi_util_reset_app_data(FALSE, MMGSDI_SLOT_1);
  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_data_slot_ptr[MMGSDI_SLOT_1_INDEX]);
}/* mmgsdi_psm_slot_app_ef_data_cleanup */


/*===========================================================================
FUNCTION MMGSDI_PSM_CREATE_LOCAL_CACHE

DESCRIPTION
  To create local cache for EPSLOCI, PSLOCI, LOCI and PIN1 status

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_psm_create_local_cache
(
  mmgsdi_pin_status_enum_type  pin_status
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_len_type         file_len      = 0;

  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_psm_local_cache_ptr);
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_psm_local_cache_ptr,
                                     sizeof(mmgsdi_psm_local_cache_type));
  if(mmgsdi_psm_local_cache_ptr == NULL)
  {
    return;
  }

  mmgsdi_status = mmgsdi_cache_read_item(MMGSDI_USIM_LOCI,
                                         MMGSDI_APP_USIM,
                                         MMGSDI_GW_PROV_PRI_SESSION,
                                         sizeof(mmgsdi_psm_local_cache_ptr->loci_data),
                                         0,
                                         &file_len,
                                         mmgsdi_psm_local_cache_ptr->loci_data,
                                         NULL);

  mmgsdi_psm_local_cache_ptr->loci_len = (uint8)file_len;
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_psm_local_cache_ptr->loci_len = 0;
  }

  mmgsdi_status = mmgsdi_cache_read_item(MMGSDI_USIM_PSLOCI,
                                         MMGSDI_APP_USIM,
                                         MMGSDI_GW_PROV_PRI_SESSION,
                                         sizeof(mmgsdi_psm_local_cache_ptr->psloci_data),
                                         0,
                                         &file_len,
                                         mmgsdi_psm_local_cache_ptr->psloci_data,
                                         NULL);

  mmgsdi_psm_local_cache_ptr->psloci_len = (uint8)file_len;
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_psm_local_cache_ptr->psloci_len = 0;
  }

  mmgsdi_status = mmgsdi_cache_read_item(MMGSDI_USIM_EPSLOCI,
                                         MMGSDI_APP_USIM,
                                         MMGSDI_GW_PROV_PRI_SESSION,
                                         sizeof(mmgsdi_psm_local_cache_ptr->epsloci_data),
                                         0,
                                         &file_len,
                                         mmgsdi_psm_local_cache_ptr->epsloci_data,
                                         NULL);

  mmgsdi_psm_local_cache_ptr->epsloci_len = (uint8)file_len;
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_psm_local_cache_ptr->epsloci_len = 0;
  }

  mmgsdi_psm_local_cache_ptr->pin_status = pin_status;
}/* mmgsdi_psm_create_local_cache */


/*===========================================================================
FUNCTION MMGSDI_PSM_READ_FILE

DESCRIPTION
  This function reads the data from the given input file location and returns it to the caller.
  Here we allocate memory to store the data freom the input file. So,the caller should take care
  of freeing the memory after the usage
  
PARAMETERS
  file_name        : path of the input file
  use_sfs          : Tells if EFS or SFS is used
  len_ptr          : Length of the input file is returned by the function
  data_pptr        : Data which is read from the given input file, is returned by the function.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type
  
  MMGSDI_SUCCESS
  MMGSDI_ERROR : Could not get input file size or Could not read input file data or dest_file_ptr is NULL
  MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED : Memory allocation failed

SIDE EFFECTS
  In this function, we are allocating memory for storing the data from input
  file and giving it to the caller. So, caller of this function should free the memory after its usage.
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_read_file
(
  const char  *file_name,
  boolean     use_sfs,
  uint32      *len_ptr,
  uint8       **data_pptr
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;

  if(data_pptr == NULL || len_ptr == NULL || file_name == NULL)
  {
    return MMGSDI_ERROR;
  }

#ifndef FEATURE_UIM_TEST_FRAMEWORK
  /* Retrieve file size to check the existence */
  if (use_sfs)
  {
    mmgsdi_status  = mmgsdi_sfs_get_size(len_ptr,
                                         file_name);
  }
  else
  {
    mmgsdi_status = gsdi_fs_get_file_size(file_name,
                                          len_ptr,
                                          UIM_COMMON_EFS_DEVICE);
  }
#else
  #error code not present
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
  if (mmgsdi_status != MMGSDI_SUCCESS ||
      *len_ptr == 0)
  {
    UIM_MSG_ERR_0("Failed to get file size");
    return mmgsdi_status;
  }

  /* Allocate memory to store the data from the input file */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*data_pptr,
                                     *len_ptr);

  if(*data_pptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

#ifndef FEATURE_UIM_TEST_FRAMEWORK
  if(use_sfs)
  {
    mmgsdi_status = mmgsdi_sfs_read(*data_pptr,
                                    *len_ptr,
                                    file_name);
  }
  else
  {
    mmgsdi_status = gsdi_efs_read_file(file_name,
                                       len_ptr,
                                       *data_pptr,
                                       UIM_COMMON_EFS_DEVICE);
  }
#else
  #error code not present
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Failed to read data from the input file");
    MMGSDIUTIL_TMC_MEM_FREE(*data_pptr);
    return mmgsdi_status;
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_psm_read_file */


/*===========================================================================
FUNCTION MMGSDI_PSM_SFS_AND_EFS_CACHE_INIT

DESCRIPTION
  To initialize slot and app data from stored PSM data

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_sfs_and_efs_cache_init(void)
{
  uint32                       file_len                           = 0;
  uint8                       *file_data_ptr                      = NULL;
  mmgsdi_return_enum_type      mmgsdi_status                      = MMGSDI_ERROR;
  mmgsdi_pin_status_enum_type  pin_status                         = MMGSDI_PIN_STATUS_NOT_INITIALIZED;
  mmgsdi_static_data_type      pin_data                           = {0,};
  mmgsdi_aid_type              aid_data;
  mmgsdi_app_capabilities_type app_cap;
  char                       * file_list[]                        = {mmgsdi_sfs_psm_usim_sensitive_ef_data,
                                                                     mmgsdi_efs_psm_usim_non_sensitive_ef_data
#ifdef FEATURE_MMGSDI_3GPP2
                                                                    ,mmgsdi_sfs_psm_csim_sensitive_ef_data,
                                                                     mmgsdi_efs_psm_csim_non_sensitive_ef_data
#endif /* FEATURE_MMGSDI_3GPP2 */
                                                                    };
  uint8 list_size                                                 = sizeof(file_list) /
                                                                      sizeof(file_list[0]);
  uint8 index                                                     = 0;

  memset(&aid_data, 0x00, sizeof(mmgsdi_aid_type));
  memset(&app_cap, 0x00, sizeof(app_cap));
  do
  {
    mmgsdi_status = mmgsdi_psm_read_file(mmgsdi_sfs_psm_uicc_profile,
                                         TRUE,
                                         &file_len,
                                         &file_data_ptr);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_1("Failed to read the file from file system 0x%x", mmgsdi_status);
      break;
    }
    if (file_len != sizeof(mmgsdi_psm_data_type))
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* Allocate global variable to store UICC profile */
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_psm_data_ptr);
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_psm_data_ptr,
                                       sizeof(mmgsdi_psm_data_type));
    if(mmgsdi_psm_data_ptr == NULL)
    {
      mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      break;
    }

    mmgsdi_memscpy(mmgsdi_psm_data_ptr, sizeof(mmgsdi_psm_data_type),
                   file_data_ptr, sizeof(mmgsdi_psm_data_type));

    /* Usage of file_data_ptr is done. Freeing it so that we will use the ponter again for reading other files */
    MMGSDIUTIL_TMC_MEM_FREE(file_data_ptr);

    /* End PSM Init if PSM context is stored to be used only in PSM exit scenario
       and it is normal power up. */
    if((mmgsdi_psm_nv73957_value & MMGSDI_PSM_NO_CONTEXT_RESTORE_AFTER_PWR_CYCLE_MASK) &&
       time_get_ms_native() < mmgsdi_psm_data_ptr->system_time)
    {
      UIM_MSG_ERR_0("PSM context is stored to be used only in PSM exit but this is normal pup - end PSM init");
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* Check slot id and card type in SFS data */
    if(mmgsdi_psm_data_ptr->card_type != MMGSDI_UICC)
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* Build slot data */
    mmgsdi_status = mmgsdi_psm_build_slot_data(mmgsdi_psm_data_ptr);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }

    /* App data present in mmgsdi_psm_data_ptr is packed. so can't copy it as a
       whole */
    aid_data.app_type = mmgsdi_psm_data_ptr->app_data[0].app_type;

    aid_data.aid.data_len = mmgsdi_psm_data_ptr->app_data[0].aid.data_len;
    mmgsdi_memscpy(aid_data.aid.data_ptr, sizeof(aid_data.aid.data_ptr), mmgsdi_psm_data_ptr->app_data[0].aid.data_ptr,
                   mmgsdi_psm_data_ptr->app_data[0].aid.data_len);

    aid_data.label.data_len = mmgsdi_psm_data_ptr->app_data[0].label.data_len;
    mmgsdi_memscpy(aid_data.label.data_ptr, sizeof(aid_data.label.data_ptr), mmgsdi_psm_data_ptr->app_data[0].label.data_ptr,
                   mmgsdi_psm_data_ptr->app_data[0].label.data_len);

    /* USIM app capability present in mmgsdi_psm_data_ptr is packed. 
       so can't copy it as a whole */
    app_cap.acl_state            = mmgsdi_psm_data_ptr->app_cap[0].acl_state;
    app_cap.fdn_state            = mmgsdi_psm_data_ptr->app_cap[0].fdn_state;
    app_cap.cc_state             = mmgsdi_psm_data_ptr->app_cap[0].cc_state;
    app_cap.bdn_state            = mmgsdi_psm_data_ptr->app_cap[0].bdn_state;
    app_cap.imsi_invalidated     = mmgsdi_psm_data_ptr->app_cap[0].imsi_invalidated;
    app_cap.all_srv_init         = mmgsdi_psm_data_ptr->app_cap[0].all_srv_init;
    app_cap.unknown_capabilities = mmgsdi_psm_data_ptr->app_cap[0].unknown_capabilities;

    pin_status = mmgsdi_psm_data_ptr->pin_status;

    pin_data.data_len = mmgsdi_psm_data_ptr->pin_data.data_len;
    mmgsdi_memscpy(pin_data.data_ptr, sizeof(pin_data.data_ptr), mmgsdi_psm_data_ptr->pin_data.data_ptr,
                   mmgsdi_psm_data_ptr->pin_data.data_len);

    /* Build app data present at index 0*/
    mmgsdi_status = mmgsdi_psm_build_app_data(mmgsdi_generic_data_ptr->pri_gw_session_id,
                                              &aid_data,
                                              pin_status,
                                              pin_data,
                                              app_cap);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      break;
    }

#ifdef FEATURE_MMGSDI_3GPP2
    /* Build app data present at index 1*/
    if(mmgsdi_psm_data_ptr->app_data[1].app_type == MMGSDI_APP_CSIM)
    {
      /* App data present in mmgsdi_psn_data_ptr is packed. so can't copy it as a
         whole */
      aid_data.app_type = mmgsdi_psm_data_ptr->app_data[1].app_type;

      aid_data.aid.data_len = mmgsdi_psm_data_ptr->app_data[1].aid.data_len;
      mmgsdi_memscpy(aid_data.aid.data_ptr, sizeof(aid_data.aid.data_ptr), mmgsdi_psm_data_ptr->app_data[1].aid.data_ptr,
                     mmgsdi_psm_data_ptr->app_data[1].aid.data_len);

      aid_data.label.data_len = mmgsdi_psm_data_ptr->app_data[1].label.data_len;
      mmgsdi_memscpy(aid_data.label.data_ptr, sizeof(aid_data.label.data_ptr), mmgsdi_psm_data_ptr->app_data[1].label.data_ptr,
                     mmgsdi_psm_data_ptr->app_data[1].label.data_len);

      /* building CSIM app capability present in mmgsdi_psm_data_ptr is packed. 
      so can't copy it as a whole */
      app_cap.acl_state            = mmgsdi_psm_data_ptr->app_cap[1].acl_state;
      app_cap.fdn_state            = mmgsdi_psm_data_ptr->app_cap[1].fdn_state;
      app_cap.cc_state             = mmgsdi_psm_data_ptr->app_cap[1].cc_state;
      app_cap.bdn_state            = mmgsdi_psm_data_ptr->app_cap[1].bdn_state;
      app_cap.imsi_invalidated     = mmgsdi_psm_data_ptr->app_cap[1].imsi_invalidated;
      app_cap.all_srv_init         = mmgsdi_psm_data_ptr->app_cap[1].all_srv_init;
      app_cap.unknown_capabilities = mmgsdi_psm_data_ptr->app_cap[1].unknown_capabilities;

      mmgsdi_status = mmgsdi_psm_build_app_data(mmgsdi_generic_data_ptr->pri_1x_session_id,
                                                &aid_data,
                                                pin_status,
                                                pin_data,
                                                app_cap);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        break;
      }
    }
    else
    {
      /* If CSIM is not present, remove second half of the list with files dedicated to CSIM */
      list_size = list_size / 2;
    }
#endif /* FEATURE_MMGSDI_3GPP2 */

    for (index = 0; index < list_size; index++)
    {
      /*Reading EF data file from SFS or EFS based on the index . For even index, read from SFS.
        For odd index read from EFS */
      mmgsdi_status = mmgsdi_psm_read_file(file_list[index],
                                           ((index % 2) ? FALSE : TRUE),
                                           &file_len,
                                           &file_data_ptr);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        break;
      }
      /* Build USIM sensitive EF cache */
      mmgsdi_status = mmgsdi_psm_build_ef_data(file_data_ptr,
                                               file_len);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        break;
      }
      MMGSDIUTIL_TMC_MEM_FREE(file_data_ptr);
    }
  }while(0);

  /* Create local copy of LOCI, EPSLOCI and PSLOCI cache; it will be used for
  comparison with card's EFs. Note that global cache of these files can be
  updated by MMGSDI's clients in their context so we need a local copy */
  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    mmgsdi_psm_create_local_cache(pin_status);
  }
  else
  {
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_psm_data_ptr);
    mmgsdi_psm_slot_app_ef_data_cleanup();
  }
  MMGSDIUTIL_TMC_MEM_FREE(file_data_ptr);

  return mmgsdi_status;
}/* mmgsdi_psm_sfs_and_efs_cache_init */


/*===========================================================================
FUNCTION MMGSDI_PSM_TRIGGER_SESSION_EVENTS

DESCRIPTION
  To trigger card/session events.

PARAMETERS
  session_id :  session id

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_trigger_session_events(
  mmgsdi_session_id_type session_id)
{
  mmgsdi_return_enum_type             mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_int_app_info_type           *sel_app_ptr   = NULL;
  mmgsdi_evt_session_notify_type      notify_type;

  /* Send session changed events for USIM and CSIM if app data is valid */
  mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                 NULL,
                                                 NULL,
                                                 &sel_app_ptr,
                                                 NULL,
                                                 NULL,
                                                 NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS ||
      sel_app_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  UIM_MSG_HIGH_1("Session Events for app: 0x%x",sel_app_ptr->app_data.app_type);

  /* Assign session notify type info */
  notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
  notify_type.slot_id     = MMGSDI_SLOT_1;
  notify_type.session_id  = session_id;

  mmgsdi_evt_build_and_send_session_changed(
    notify_type, TRUE, (const mmgsdi_int_app_info_type*)sel_app_ptr);

  mmgsdi_evt_build_and_send_ecc(notify_type);

  /* Send all the PINs event */
  if(sel_app_ptr->pin1_ptr)
  {
    mmgsdi_evt_build_and_send_pin(
      notify_type,
      MMGSDI_PIN1,
      sel_app_ptr->pin1_ptr->status,
      (const mmgsdi_int_app_info_type*)sel_app_ptr);
  }

  mmgsdi_evt_build_and_send_pin(
    notify_type,
    MMGSDI_PIN2,
    sel_app_ptr->pin2.status,
    (const mmgsdi_int_app_info_type*)sel_app_ptr);

  if(sel_app_ptr->universal_pin_ptr)
  {
    mmgsdi_evt_build_and_send_pin(
      notify_type,
      MMGSDI_UNIVERSAL_PIN,
      sel_app_ptr->universal_pin_ptr->status,
      (const mmgsdi_int_app_info_type*)sel_app_ptr);
  }

#ifdef FEATURE_SIMLOCK
  mmgsdi_simlock_build_and_run_algo(TRUE);
#endif /* FEATURE_SIMLOCK */

  mmgsdi_evt_build_and_send_perso(
    notify_type,
    (const mmgsdi_int_app_info_type*)sel_app_ptr,
    MMGSDI_PERSO_STATUS_DONE,
    MMGSDI_MAX_PERSO_FEATURE_ENUM,
    0,
    0);

  UIM_MSG_HIGH_0("MMGSDI_SUBSCRIPTION_READY_EVT");

  mmgsdi_evt_build_and_send_subscription_ready(
    notify_type, (const mmgsdi_int_app_info_type*)sel_app_ptr);

  sel_app_ptr->app_state = MMGSDI_APP_STATE_READY;

  return MMGSDI_SUCCESS;
}/* mmgsdi_psm_trigger_session_events */


/*===========================================================================
FUNCTION MMGSDI_PSM_SEND_CARD_INSERTED_EVENT

DESCRIPTION
  To trigger the send of card inserted event.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_psm_send_card_inserted_event(
  void
)
{
  mmgsdi_slot_data_type  *slot_data_ptr = NULL;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(MMGSDI_SLOT_1);
  if (slot_data_ptr == NULL)
  {
    return;
  }

  slot_data_ptr->mmgsdi_state = MMGSDI_STATE_CARD_INSERTED;

  UIM_MSG_HIGH_0("MMGSDI_CARD_INSERTED_EVT");
  mmgsdi_evt_build_and_send_card_inserted(TRUE, 0, MMGSDI_SLOT_1);
}/* mmgsdi_psm_send_card_inserted_event */


/*===========================================================================
FUNCTION MMGSDI_PSM_TRIGGER_EARLY_EVENTS

DESCRIPTION
  To trigger card/session events for PSM mode (early events)

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_psm_trigger_early_events(
  void
)
{
  /* Send event for card inserted */
  mmgsdi_psm_send_card_inserted_event();

  /* Send events related to GW session */
  (void)mmgsdi_psm_trigger_session_events(mmgsdi_generic_data_ptr->pri_gw_session_id);

#ifdef FEATURE_MMGSDI_3GPP2
  /* Send events related to 1X session */
  (void)mmgsdi_psm_trigger_session_events(mmgsdi_generic_data_ptr->pri_1x_session_id);
#endif /* FEATURE_MMGSDI_3GPP2 */
}/* mmgsdi_psm_trigger_early_events */


/*===========================================================================
FUNCTION MMGSDI_PSM_INITIALIZE

DESCRIPTION
  To trigger reading of EFS data, caching it and seding card/session events.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_psm_initialize
(
  void
)
{
  mmgsdi_return_enum_type   mmgsdi_status  = MMGSDI_SUCCESS;
  mmgsdi_psm_mode_enum_type psm_mode       = MMGSDI_PSM_MODE_DISABLED;

  mmgsdi_psm_read_psm_configuration();

  psm_mode = mmgsdi_psm_get_enabled_status();

  if(psm_mode == MMGSDI_PSM_MODE_DISABLED)
  {
    /* Purging PSM data completely because it is possible that PSM mode is disabled across
       power cycle but data has been stored during previous power down when PSM mode
       was enabled. */
    mmgsdi_psm_purge_data(TRUE);
    return;
  }

  /* Check UIM PSM PIN1 cache support NV is enabled on not */
  (void)uim_common_efs_read(UIM_COMMON_EFS_MMGSDI_PSM_PIN1_CACHE_SUPPORT,
                            UIM_COMMON_EFS_ITEM_FILE_TYPE,
                            UIM_COMMON_EFS_DEVICE,
                            &mmgsdi_psm_nv73957_value,
                            sizeof(mmgsdi_psm_nv73957_value));

  /* Since we are about to initilaize PSM cache content, set
     mmgsdi_psm_init_in_progress to TRUE so that sensitive SFS or 
     non-sensitive EFS file will not be deleted */
  mmgsdi_psm_init_in_progress = TRUE;

  /* continue with PSM if NV is set to PSM enable at task startup */
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  /* initialize slot, app and EF cache from stored PSM data */
  mmgsdi_status = mmgsdi_psm_sfs_and_efs_cache_init();

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  /* send early events */
  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* Set the early init flag to true */
    mmgsdi_psm_early_init_done = TRUE;

    if(psm_mode == MMGSDI_PSM_MODE_EARLY_EVTS)
    {
      /* trigger card/session events based on cached data. */
      mmgsdi_psm_trigger_early_events();
    }
  }
  else
  {
    /* PSM cache init is not success, so normal initialization would take place.
       Set mmgsdi_psm_init_in_progress to FALSE, so that psm related SFS and EFS
       files will be deleted during normal initialization */
    mmgsdi_psm_init_in_progress = FALSE;
  }
}/* mmgsdi_psm_initialize */


/*===========================================================================
FUNCTION   MMGSDI_PSM_UPDATE_PSM_READY_RESP_STATUS

DESCRIPTION
  This function checks PIN1 status and update PSM Ready status to CM.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void mmgsdi_psm_update_psm_ready_resp_status(
  sys_psm_status_e_type  *psm_read_status_ptr
)
{
  mmgsdi_slot_data_type       *mmgsdi_slot_data_ptr = NULL;
  mmgsdi_int_app_info_type    *app_info_ptr         = NULL;
  mmgsdi_return_enum_type      mmgsdi_status        = MMGSDI_ERROR;
  mmgsdi_pin_status_enum_type  pin1_status_gw       = MMGSDI_PIN_DISABLED;
  mmgsdi_pin_status_enum_type  pin1_status_1x       = MMGSDI_PIN_DISABLED;

  if(psm_read_status_ptr == NULL)
  {
    return;
  }

  *psm_read_status_ptr = SYS_PSM_STATUS_READY;

  mmgsdi_slot_data_ptr = mmgsdi_util_get_slot_data_ptr(MMGSDI_SLOT_1);
  if(mmgsdi_slot_data_ptr == NULL)
  {
    return;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(mmgsdi_generic_data_ptr->pri_gw_session_id,
                                                   NULL,
                                                   NULL,
                                                   &app_info_ptr,
                                                   NULL,
                                                   NULL,
                                                   NULL);
  if (mmgsdi_status == MMGSDI_SUCCESS &&
      app_info_ptr != NULL &&
      app_info_ptr->pin1_ptr!= NULL)
  {
    pin1_status_gw = app_info_ptr->pin1_ptr->status;
  }

#ifdef FEATURE_MMGSDI_3GPP2
  mmgsdi_status = mmgsdi_util_get_session_app_info(mmgsdi_generic_data_ptr->pri_1x_session_id,
                                                   NULL,
                                                   NULL,
                                                   &app_info_ptr,
                                                   NULL,
                                                   NULL,
                                                   NULL);
  if (mmgsdi_status == MMGSDI_SUCCESS &&
      app_info_ptr != NULL &&
      app_info_ptr->pin1_ptr!= NULL)
  {
    pin1_status_1x = app_info_ptr->pin1_ptr->status;
  }
#endif /* FEATURE_MMGSDI_3GPP2 */

  /* Vote negatively if the following are met else vote +ve
     1.PIN1 caching NV support is enabled and
     2.PIN1 status is not disabled */
  if(!(pin1_status_gw == MMGSDI_PIN_DISABLED &&
       pin1_status_1x == MMGSDI_PIN_DISABLED) &&
     !(mmgsdi_psm_nv73957_value & MMGSDI_PSM_PIN1_CACHE_MASK))
  {
    *psm_read_status_ptr = SYS_PSM_STATUS_ENTER_DEEP_SLEEP;

    /* Set mmgsdi global to remember CM_CLIENT_PSM_READY_IND is pending 
       This is mainly to take care of sending CM_CLIENT_PSM_READY_IND
       when there is any run time PIN1 disable */
    mmgsdi_psm_ready_ind_pending = TRUE;
  }
  else if(mmgsdi_psm_get_status_early_init())
  {
    *psm_read_status_ptr = SYS_PSM_STATUS_NOT_READY_NOT_IDLE;
      
    /* Set mmgsdi global to remember CM_CLIENT_PSM_READY_IND is pending 
       This is mainly to take care of sending CM_CLIENT_PSM_READY_IND
       when PSM exit procedure is going on.
       NV value 2:
       CM sent context save request only after reaching sub redy.
       In case of UIM PSM NV value 2, MMGSDI send events only after actual 
       card got detected. So back to back PSM entry issue will not come here */
    mmgsdi_psm_ready_ind_pending = TRUE;
  }

}/* mmgsdi_psm_update_psm_ready_resp_status */


/*===========================================================================
FUNCTION   MMGSDI_PSM_CM_SEND_PSM_READY_REQ_CB

DESCRIPTION
  This function will be called by CM to if check MMGSDI is ready to enter PSM
  mode or not.

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/
static boolean mmgsdi_psm_cm_send_psm_ready_req_cb(
  void
)
{
  cm_generic_rpt_u_type     gen_rpt;

  memset(&gen_rpt, 0x00, sizeof(cm_generic_rpt_u_type));

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  mmgsdi_psm_update_psm_ready_resp_status(&gen_rpt.psm_ready_rsp.status);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  /* No need to update timer as UIM don't have any timer value */
  gen_rpt.psm_ready_rsp.activate_timer = CM_PSM_PERIODIC_UPDATE_TIMER_INVALID;
  gen_rpt.psm_ready_rsp.module = CM_PSM_MODULE_UIM;

  UIM_MSG_HIGH_2("MMGSDI PSM Ready Rsp Status: 0x%x IND pending: 0x%x",
                 gen_rpt.psm_ready_rsp.status, mmgsdi_psm_ready_ind_pending);

  cm_client_send_psm_rpt(CM_CLIENT_PSM_READY_RSP, &gen_rpt); 

  return TRUE;
} /* mmgsdi_psm_cm_send_psm_ready_req_cb */


/*===========================================================================
FUNCTION   MMGSDI_CM_SEND_PSM_CONTEXT_SAVE_REQ_CB

DESCRIPTION
  This function will queue PSM context save request to MMGSDI Queue.

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/
static boolean mmgsdi_cm_send_psm_context_save_req_cb(
  void
)
{
  mmgsdi_task_cmd_type                      *task_cmd_ptr   = NULL;
  mmgsdi_psm_context_save_req_type          *msg_ptr        = NULL;
  mmgsdi_return_enum_type                    mmgsdi_status  = MMGSDI_ERROR;

  /* Memory allocation for msg_ptr and check for allocation */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr,
                                     sizeof(mmgsdi_task_cmd_type));
  if(task_cmd_ptr == NULL)
  {
    return FALSE;
  }

  msg_ptr = &task_cmd_ptr->cmd.cmd.psm_context_save_req;

  /* Populating mmgsdi_map_sessions_to_slots header */
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_PSM_CONTEXT_SAVE_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_PSM_CONTEXT_SAVE_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_PSM_CONTEXT_SAVE_REQ;
  msg_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  msg_ptr->request_header.session_id        = 0;
  msg_ptr->request_header.slot_id           = MMGSDI_SLOT_1;

  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_PSM_CONTEXT_SAVE_REQ status 0x%x",
                 mmgsdi_status);

  return TRUE;
} /* mmgsdi_cm_send_psm_context_save_req_cb */


/*===========================================================================
FUNCTION MMGSDI_PSM_CM_REGISTER

DESCRIPTION
  This function register with CM as a client to partcipate in PSM_ENTER_REQ
  voting for saving the context gracefully into SFS.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_psm_cm_register(
  void
)
{
  boolean psm_reg       = FALSE;
  boolean psm_ready_reg = FALSE;

  /* Register with CM to vote to enter PSM */
  psm_ready_reg = cm_client_psm_ready_reg(mmgsdi_psm_cm_send_psm_ready_req_cb, CM_PSM_MODULE_UIM);

  /* Register with CM to be notified before we enter PSM */
  psm_reg = cm_client_psm_enter_reg(mmgsdi_cm_send_psm_context_save_req_cb, CM_PSM_MODULE_UIM);

  if (psm_reg == FALSE || psm_ready_reg == FALSE)
  {
    UIM_MSG_ERR_2("Registration with CM for PSM: reg = 0x%x, voting = 0x%x", psm_reg, psm_ready_reg);
  }
} /* mmgsdi_psm_cm_register */


/*===========================================================================
FUNCTION MMGSDI_PSM_GET_EF_DATA_LEN

DESCRIPTION
  To calculate EF's data and attribute total length associated with session type and sensitiveness of the EF content.

PARAMETERS
  app_type            : app type
  session_type        : session type
  variable_length_ptr : address of length data

DEPENDENCIES
  None

RETURN VALUE
  uint32 length of data and attribute

SIDE EFFECTS
  None
===========================================================================*/
static uint32 mmgsdi_psm_get_ef_data_len
(
  mmgsdi_session_type_enum_type session_type,
  boolean                       ef_sensitive
)
{
  uint32                          variable_length   = 0;
  mmgsdi_return_enum_type         mmgsdi_status     = MMGSDI_ERROR;
  uint8                           index             = 0;
  mmgsdi_file_enum_type          *file_array_ptr    = NULL;
  uint8                           num_of_files      = 0;
  mmgsdi_len_type                 file_len          = 0;
  mmgsdi_cache_init_enum_type     cache_state       = MMGSDI_CACHE_MAX_ENUM;
  mmgsdi_app_enum_type            app_type          = MMGSDI_APP_USIM;
  mmgsdi_attr_cache_element_type *cache_element_ptr = NULL;
  mmgsdi_session_id_type          session_id        = MMGSDI_SESSION_ID_ZERO;
  mmgsdi_access_type              access;

  memset(&access, 0x00, sizeof(mmgsdi_access_type));
  access.access_method = MMGSDI_EF_ENUM_ACCESS;

  if (session_type == MMGSDI_GW_PROV_PRI_SESSION)
  {
    app_type = MMGSDI_APP_USIM;
    if (ef_sensitive)
    {
      file_array_ptr = mmgsdi_psm_usim_sensitive_ef_cache_enums;
      num_of_files   = sizeof(mmgsdi_psm_usim_sensitive_ef_cache_enums) /
                        sizeof(mmgsdi_psm_usim_sensitive_ef_cache_enums[0]);
    }
    else
    {
      file_array_ptr = mmgsdi_psm_usim_non_sensitive_ef_cache_enums;
      num_of_files = sizeof(mmgsdi_psm_usim_non_sensitive_ef_cache_enums) /
                      sizeof(mmgsdi_psm_usim_non_sensitive_ef_cache_enums[0]);
    }
  }
#ifdef FEATURE_MMGSDI_3GPP2
  else if (session_type == MMGSDI_1X_PROV_PRI_SESSION)
  {
    app_type = MMGSDI_APP_CSIM;
    if (ef_sensitive)
    {
      file_array_ptr  = mmgsdi_psm_csim_sensitive_ef_cache_enums;
      num_of_files    = sizeof(mmgsdi_psm_csim_sensitive_ef_cache_enums) /
                         sizeof(mmgsdi_psm_csim_sensitive_ef_cache_enums[0]);
    }
    else
    {
      file_array_ptr  = mmgsdi_psm_csim_non_sensitive_ef_cache_enums;
      num_of_files  = sizeof(mmgsdi_psm_csim_non_sensitive_ef_cache_enums) /
                       sizeof(mmgsdi_psm_csim_non_sensitive_ef_cache_enums[0]);
    }
  }
  else if(session_type == MMGSDI_CARD_SESSION_SLOT_1)
  {
    app_type = MMGSDI_APP_USIM;
    file_array_ptr = mmgsdi_psm_common_cache_enums;
    num_of_files   = sizeof(mmgsdi_psm_common_cache_enums) /
                      sizeof(mmgsdi_psm_common_cache_enums[0]);
  }
#endif /* FEATURE_MMGSDI_3GPP2 */
  else
  {
    return 0;
  }

  if(MMGSDI_SUCCESS != mmgsdi_util_get_mmgsdi_session_id_from_type(
                         session_type, &session_id))
  {
    return 0;
  }

  for(index = 0; index < num_of_files; index++)
  {
    access.file.file_enum = file_array_ptr[index];
    mmgsdi_status = mmgsdi_cache_attr_read(session_id,
                                           MMGSDI_SLOT_1,
                                           &access,
                                           &cache_element_ptr);

    if((mmgsdi_status != MMGSDI_SUCCESS) || cache_element_ptr == NULL)
    {
      mmgsdi_cache_delete_file_attr_cache_item(&cache_element_ptr);
      continue;
    }

    if(cache_element_ptr->file_type == MMGSDI_LINEAR_FIXED_FILE &&
       (file_array_ptr[index] == MMGSDI_USIM_OPL ||
        file_array_ptr[index] == MMGSDI_USIM_PNN))
    {
      variable_length = variable_length +
                        (sizeof(mmgsdi_psm_ef_data) +
                         cache_element_ptr->rec_len - sizeof(uint8))*cache_element_ptr->num_of_rec;

    }
    else
    {
      /* For transparent files, first check whether we have file cache or not.
         If file cache is not present, we cannot only store attribute as both
         attribute and file data are required for PSM cache; skip it. */
      mmgsdi_status = mmgsdi_cache_read_item_len(file_array_ptr[index],
                                                 app_type,
                                                 session_type,
                                                 0,
                                                 &file_len,
                                                 &cache_state);
      if(mmgsdi_status != MMGSDI_SUCCESS ||
         file_len == 0 ||
         (file_len > cache_element_ptr->file_size &&
          cache_element_ptr->file_type == MMGSDI_TRANSPARENT_FILE))
      {
        mmgsdi_cache_delete_file_attr_cache_item(&cache_element_ptr);
        continue;
      }
      variable_length = variable_length + sizeof(mmgsdi_psm_ef_data) + file_len - sizeof(uint8);
    }
    mmgsdi_cache_delete_file_attr_cache_item(&cache_element_ptr);
  }
  UIM_MSG_HIGH_2("length of EFs data and atttibute cache 0x%x for session type 0x%x",
                 variable_length, session_type);

  return variable_length;
}/* mmgsdi_psm_get_ef_data_len */


/*===========================================================================
FUNCTION MMGSDI_PSM_STORE_EF_DATA

DESCRIPTION
  To copy EFs' data and attribute total length associated with session type and sensitiveness of EF

PARAMETERS
  uicc_profile_pptr    : address of UICC profile PSM data ptr
  app_type             : app type
  session_type         : session type
  slot_id              : slot id
  remaining_length_ptr : inout-output paramter; provide reamining length left
                         after copied PSM data here for associated app type

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_store_ef_data
(
  uint8                            **uicc_profile_pptr,
  mmgsdi_session_type_enum_type      session_type,
  mmgsdi_slot_id_enum_type           slot_id,
  uint32                             remaining_length,
  boolean                            ef_sensitive
)
{
  mmgsdi_return_enum_type          mmgsdi_status     = MMGSDI_SUCCESS;
  uint8                            index             = 0;
  mmgsdi_attr_cache_element_type  *cache_element_ptr = NULL;
  mmgsdi_session_id_type           session_id        = MMGSDI_SESSION_ID_ZERO; 
  mmgsdi_file_enum_type           *file_array_ptr    = NULL;
  mmgsdi_cache_init_enum_type      cache_state       = MMGSDI_CACHE_MAX_ENUM;
  uint8                            num_of_files      = 0;
  mmgsdi_len_type                  file_len          = 0;
  uint8                           *data_ptr          = NULL;
  mmgsdi_access_type               access;
  mmgsdi_app_enum_type             app_type          = MMGSDI_APP_USIM;
  uint8                            rec_index         = 0;

  if(uicc_profile_pptr == NULL || remaining_length == 0)
  {
    return MMGSDI_ERROR;
  }
  UIM_MSG_HIGH_2("Remaining length to copy EFs data and atttibute cache 0x%x for session type 0x%x",
                 remaining_length, session_type);

  memset(&access, 0x00, sizeof(mmgsdi_access_type));

  if(session_type == MMGSDI_GW_PROV_PRI_SESSION)
  {
    app_type = MMGSDI_APP_USIM;
      if (ef_sensitive)
      {
        file_array_ptr = mmgsdi_psm_usim_sensitive_ef_cache_enums;
        num_of_files   = sizeof(mmgsdi_psm_usim_sensitive_ef_cache_enums) /
                          sizeof(mmgsdi_psm_usim_sensitive_ef_cache_enums[0]);
      }
      else
      {
        file_array_ptr = mmgsdi_psm_usim_non_sensitive_ef_cache_enums;
        num_of_files = sizeof(mmgsdi_psm_usim_non_sensitive_ef_cache_enums) /
                      sizeof(mmgsdi_psm_usim_non_sensitive_ef_cache_enums[0]);
      }
  }
#ifdef FEATURE_MMGSDI_3GPP2
  else if(session_type == MMGSDI_1X_PROV_PRI_SESSION)
  {
    app_type = MMGSDI_APP_CSIM;
    if (ef_sensitive)
    {
      file_array_ptr  = mmgsdi_psm_csim_sensitive_ef_cache_enums;
      num_of_files    = sizeof(mmgsdi_psm_csim_sensitive_ef_cache_enums) /
                         sizeof(mmgsdi_psm_csim_sensitive_ef_cache_enums[0]);
    }
    else
    {
      file_array_ptr   = mmgsdi_psm_csim_non_sensitive_ef_cache_enums;
      num_of_files   = sizeof(mmgsdi_psm_csim_non_sensitive_ef_cache_enums) /
                      sizeof(mmgsdi_psm_csim_non_sensitive_ef_cache_enums[0]);
    }
  }
  else if(session_type == MMGSDI_CARD_SESSION_SLOT_1)
  {
    app_type = MMGSDI_APP_USIM;
    file_array_ptr = mmgsdi_psm_common_cache_enums;
    num_of_files   = sizeof(mmgsdi_psm_common_cache_enums) /
                      sizeof(mmgsdi_psm_common_cache_enums[0]);
  }
#endif /* FEATURE_MMGSDI_3GPP2 */
  else
  {
    return MMGSDI_ERROR;
  }

  if(MMGSDI_SUCCESS != mmgsdi_util_get_mmgsdi_session_id_from_type(
                         session_type, &session_id))
  {
    return MMGSDI_ERROR;
  }

  for(index = 0; index < num_of_files; index++)
  {
    access.access_method = MMGSDI_EF_ENUM_ACCESS;
    access.file.file_enum = file_array_ptr[index];

    mmgsdi_status = mmgsdi_cache_attr_read(session_id,
                                           slot_id,
                                           &access,
                                           &cache_element_ptr);

    if((mmgsdi_status != MMGSDI_SUCCESS) || cache_element_ptr == NULL)
    {
      mmgsdi_cache_delete_file_attr_cache_item(&cache_element_ptr);
      continue;
    }

    if(file_array_ptr[index] != MMGSDI_USIM_OPL &&
       file_array_ptr[index] != MMGSDI_USIM_PNN)
    {
      /* For transparent files, first check whether we have file cache or not.
         If file cache is not present, skip it because both attribute and file
         data are required for PSM cache. */
      mmgsdi_status = mmgsdi_cache_read_item_len(file_array_ptr[index],
                                                 app_type,
                                                 session_type,
                                                 0,
                                                 &file_len,
                                                 &cache_state);
      if(mmgsdi_status != MMGSDI_SUCCESS ||
         file_len == 0 ||
         (file_len > cache_element_ptr->file_size &&
          cache_element_ptr->file_type == MMGSDI_TRANSPARENT_FILE))
      {
        mmgsdi_cache_delete_file_attr_cache_item(&cache_element_ptr);
        continue;
      }
    }

    ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->sfi =
      cache_element_ptr->sfi;
    ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->num_of_rec =
      (uint8)cache_element_ptr->num_of_rec;

    /* If protection method is other that "Always" and "Allowed", we map it to
       "Single Allowed" because we only store one PIN in PSM data for ease of
       caching */
    if(cache_element_ptr->read.protection_method != MMGSDI_ALWAYS_ALLOWED &&
       cache_element_ptr->read.protection_method != MMGSDI_NEVER_ALLOWED)
    {
      ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->read_protection = MMGSDI_SINGLE_ALLOWED;
      if(cache_element_ptr->read.protection_pin_ptr)
      {
        ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->read_pin =
          cache_element_ptr->read.protection_pin_ptr[0];
      }
    }
    else
    {
      ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->read_protection =
        cache_element_ptr->read.protection_method;
    }

    if(cache_element_ptr->write.protection_method != MMGSDI_ALWAYS_ALLOWED &&
       cache_element_ptr->write.protection_method != MMGSDI_NEVER_ALLOWED)
    {
      ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->write_protection = MMGSDI_SINGLE_ALLOWED;
      if(cache_element_ptr->write.protection_pin_ptr)
      {
        ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->write_pin =
          cache_element_ptr->write.protection_pin_ptr[0];
      }
    }
    else
    {
      ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->write_protection =
        cache_element_ptr->write.protection_method;
    }

    if(cache_element_ptr->increase.protection_method != MMGSDI_ALWAYS_ALLOWED &&
       cache_element_ptr->increase.protection_method != MMGSDI_NEVER_ALLOWED)
    {
      ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->increase_protection = MMGSDI_SINGLE_ALLOWED;
      if(cache_element_ptr->increase.protection_pin_ptr)
      {
        ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->increase_pin =
          cache_element_ptr->increase.protection_pin_ptr[0];
      }
    }
    else
    {
      ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->increase_protection =
        cache_element_ptr->increase.protection_method;
    }

    if(cache_element_ptr->file_type == MMGSDI_LINEAR_FIXED_FILE &&
       (file_array_ptr[index] == MMGSDI_USIM_OPL ||
        file_array_ptr[index] == MMGSDI_USIM_PNN))
    {
      for(rec_index = 1; rec_index <= cache_element_ptr->num_of_rec; rec_index++)
      {
        mmgsdi_data_type     record_data = {0 , NULL};
        mmgsdi_read_req_type read_request;

        ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->file_enum = file_array_ptr[index];
        ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->num_of_rec = (uint8)cache_element_ptr->num_of_rec;
        ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->attr_len = cache_element_ptr->rec_len;
        ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->data_len = cache_element_ptr->rec_len;

        memset(&read_request, 0x00, sizeof(mmgsdi_read_req_type));

        read_request.request_header.session_id = session_id;
        read_request.access = access;
        read_request.data_len = cache_element_ptr->rec_len;
        read_request.rec_num = rec_index;

        mmgsdi_status = mmgsdi_eons_opl_pnn_read_record(&read_request,
                                                        &record_data,
                                                        NULL,
                                                        TRUE);
        file_len = record_data.data_len;

        /* Cache read function allocates memory for EF-OPL and EF-PNN */
        data_ptr = record_data.data_ptr;
 
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          UIM_MSG_HIGH_2("mmgsdi_cache_read_item failed for EF 0x%x rec num 0x%x",
                         file_array_ptr[index], rec_index);

          /* Set rec num to 0 at record is not present in cache */
          ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->rec_num = 0;

          if(remaining_length < (sizeof(mmgsdi_psm_ef_data) +
                                 cache_element_ptr->rec_len -
                                 sizeof(uint8)))
          {
            MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
            mmgsdi_cache_delete_file_attr_cache_item(&cache_element_ptr);
            return MMGSDI_ERROR;
          }

          remaining_length = remaining_length - sizeof(mmgsdi_psm_ef_data) + sizeof(uint8);
          remaining_length = remaining_length - cache_element_ptr->rec_len;
          *uicc_profile_pptr += (sizeof(mmgsdi_psm_ef_data) +
                                 cache_element_ptr->rec_len -
                                 sizeof(uint8));

          /* Only first record will have valid attributes so set read_pin with
             initialized data for next record */
          if(rec_index != cache_element_ptr->num_of_rec)
          {
           ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->read_protection = MMGSDI_MAX_FILE_SECURITY_ENUM;
          }
          MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
          continue;
        }

        /* rec num is used to check whether PSM cache has valid record or not.
           file enum, data len and num of rec is stored with all records. */
        ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->rec_num = rec_index;

        if(remaining_length < (sizeof(mmgsdi_psm_ef_data) +
                               cache_element_ptr->rec_len -
                               sizeof(uint8)))
        {
          MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
          mmgsdi_cache_delete_file_attr_cache_item(&cache_element_ptr);
          return MMGSDI_ERROR;
        }

        remaining_length = remaining_length - sizeof(mmgsdi_psm_ef_data) + sizeof(uint8);
        mmgsdi_memscpy(&((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->first_byte_data,
                       remaining_length,
                       data_ptr,
                       ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->data_len);

        remaining_length = remaining_length - ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->data_len;
        *uicc_profile_pptr += (sizeof(mmgsdi_psm_ef_data) +
                               ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->data_len -
                               sizeof(uint8));
        /* Only first record will have valid attributes so set read_pin with
           initialized data for next record */
        if(rec_index != cache_element_ptr->num_of_rec)
        {
          ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->read_protection = MMGSDI_MAX_FILE_SECURITY_ENUM;
        }
        MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
      }
    }
    else
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(data_ptr, file_len);
      if(data_ptr == NULL)
      {
        mmgsdi_cache_delete_file_attr_cache_item(&cache_element_ptr);
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }

      mmgsdi_status = mmgsdi_cache_read_item(file_array_ptr[index],
                                             app_type,
                                             session_type,
                                             file_len,
                                             0,
                                             &file_len,
                                             data_ptr,
                                             &cache_state);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
        mmgsdi_cache_delete_file_attr_cache_item(&cache_element_ptr);
        return MMGSDI_ERROR;
      }
      ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->file_enum = file_array_ptr[index];
      ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->rec_num = 0;
      ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->attr_len = cache_element_ptr->file_size;
      ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->data_len = file_len;

      if(remaining_length < (sizeof(mmgsdi_psm_ef_data) +
                             ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->data_len -
                             sizeof(uint8)))
      {
        MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
        mmgsdi_cache_delete_file_attr_cache_item(&cache_element_ptr);
        return MMGSDI_ERROR;
      }
      remaining_length = remaining_length - sizeof(mmgsdi_psm_ef_data) + sizeof(uint8);
      mmgsdi_memscpy(&((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->first_byte_data,
                     remaining_length,
                     data_ptr,
                     ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->data_len);

      remaining_length = remaining_length - ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->data_len;
      *uicc_profile_pptr += (sizeof(mmgsdi_psm_ef_data) +
                             ((mmgsdi_psm_ef_data *)*uicc_profile_pptr)->data_len -
                             sizeof(uint8));
      MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
    }
    mmgsdi_cache_delete_file_attr_cache_item(&cache_element_ptr);
  }
  return MMGSDI_SUCCESS;
}/* mmgsdi_psm_store_ef_data */


/*===========================================================================
FUNCTION MMGSDI_PSM_CHECK_UICC_PROFILE_PATH

DESCRIPTION
  Check whether path exists to store the PSM data in EFS and SFS.
  If path is not available, we try to create the path

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  Boolean
    TRUE  : If path available or created successfully
    FALSE : Otherwise

SIDE EFFECTS
  None
===========================================================================*/
static boolean mmgsdi_psm_check_uicc_path
(
  void
)
{
  boolean uicc_profile_path_is_available = FALSE;

  if (MMGSDI_SUCCESS != gsdi_efs_name_test(mmgsdi_safe_dir,
                                           UIM_COMMON_EFS_DEVICE))
  {
    return FALSE;
  }

  /* safe directory is available, check availability of sfs dir*/
  if (MMGSDI_SUCCESS == gsdi_efs_name_test(mmgsdi_sfs_dir,
                                           UIM_COMMON_EFS_DEVICE))
  {
    uicc_profile_path_is_available = TRUE;
  }
  else
  {
    if (MMGSDI_SUCCESS == gsdi_efs_mkdir(mmgsdi_sfs_dir))
    {
      uicc_profile_path_is_available = TRUE;
    }
  }

  if (uicc_profile_path_is_available)
  {
    /*sfs directory is available, check availability of uim dir*/
    if(MMGSDI_SUCCESS != gsdi_efs_name_test(mmgsdi_sfs_uim_dir,
                                            UIM_COMMON_EFS_DEVICE))
    {
      if (MMGSDI_SUCCESS != gsdi_efs_mkdir(mmgsdi_sfs_uim_dir))
      {
        uicc_profile_path_is_available = FALSE;
      }
    }
  }

  if (uicc_profile_path_is_available)
  {
#ifndef FEATURE_UIM_TEST_FRAMEWORK
    /* Need to create folder using SFS API to write/read file in it using
       SFS APIs */
    if (MMGSDI_SUCCESS != mmgsdi_sfs_mkdir(mmgsdi_sfs_uim_mmgsdi_dir))
    {
      uicc_profile_path_is_available = FALSE;
    }
#else
    #error code not present
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */
  }

  /* Check whether PSM EFS file paths are existing. If not, create them */
  if (uicc_profile_path_is_available)
  {
    if (MMGSDI_SUCCESS != gsdi_efs_name_test(mmgsdi_efs_dir,
                                            UIM_COMMON_EFS_DEVICE))
    {
      if(MMGSDI_SUCCESS != gsdi_efs_mkdir(mmgsdi_efs_dir))
      {
        uicc_profile_path_is_available = FALSE;
      }
    }
  }

  if(uicc_profile_path_is_available)
  {
    if (MMGSDI_SUCCESS != gsdi_efs_name_test(mmgsdi_efs_uim_psm_dir,
                                             UIM_COMMON_EFS_DEVICE))
    {
      if(MMGSDI_SUCCESS != gsdi_efs_mkdir(mmgsdi_efs_uim_psm_dir))
      {
        uicc_profile_path_is_available = FALSE;
      }
    }
  }

  return uicc_profile_path_is_available;
}/* mmgsdi_psm_check_uicc_path */


/*===========================================================================
FUNCTION MMGSDI_PSM_WRITE_FILE

DESCRIPTION
  This function takes input file path and fills EF data in it based on session_type and sensitivity
  and writes it to the File System accordingly (may be SFS or EFS)

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_write_file
(
  const char                     *file_name,
  boolean                        use_sfs,
  mmgsdi_session_type_enum_type  session_type
)
{
  uint32  file_len                      = 0;
  uint32  common_cache_ef_len           = 0;
  uint8  *data_ptr                      = NULL;
  uint8  *curr_ptr                      = NULL;
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;

  /* Check whether the file already exists.*/
#ifndef FEATURE_UIM_TEST_FRAMEWORK
  if (!use_sfs)
  {
    mmgsdi_status = gsdi_efs_name_test(file_name,
                                       UIM_COMMON_EFS_DEVICE);
  }
  else
  {
    mmgsdi_status = mmgsdi_sfs_get_size(&file_len,
                                         file_name);
  }
#else
  #error code not present
#endif /*FEATURE_UIM_TEST_FRAMEWORK */

  /* If file already exists, no need to write again */
  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  file_len = mmgsdi_psm_get_ef_data_len(session_type, use_sfs);

#ifdef FEATURE_MMGSDI_3GPP2
  if(session_type == MMGSDI_GW_PROV_PRI_SESSION &&
     !use_sfs)
  {
    /* Common cache enums are stored alng with USIM non sensitive EF data*/
    common_cache_ef_len = mmgsdi_psm_get_ef_data_len(MMGSDI_CARD_SESSION_SLOT_1, 0);
    file_len += common_cache_ef_len;
  }
#endif /* FEATURE_MMGSDI_3GPP2*/
  if(file_len == 0)
  {
    return MMGSDI_ERROR;
  }

  /* Allocate memory for storing EF DATA */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(data_ptr,
                                     file_len);
  if(data_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  curr_ptr = data_ptr;
  mmgsdi_status = mmgsdi_psm_store_ef_data(&curr_ptr,
                                           session_type,
                                           MMGSDI_SLOT_1,
                                           file_len,
                                           use_sfs);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
    return mmgsdi_status;
  }

#ifdef FEATURE_MMGSDI_3GPP2
  if(session_type == MMGSDI_GW_PROV_PRI_SESSION &&
     !use_sfs)
  {
    if(common_cache_ef_len > 0)
    {
      mmgsdi_status = mmgsdi_psm_store_ef_data(&curr_ptr,
                                               MMGSDI_CARD_SESSION_SLOT_1,
                                               MMGSDI_SLOT_1,
                                               file_len - (curr_ptr - data_ptr),
                                               use_sfs);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
        return mmgsdi_status;
      }
    }
  }
#endif /* FEATURE_MMGSDI_3GPP2*/

#ifndef FEATURE_UIM_TEST_FRAMEWORK
  if(use_sfs)
  {
    mmgsdi_status = mmgsdi_sfs_write(data_ptr,
                                     file_len,
                                     file_name);
  }
  else
  {
    mmgsdi_status = gsdi_efs_write_file(file_name,
                                        file_len,
                                        data_ptr,
                                        UIM_COMMON_EFS_DEVICE);
  }
#else
  #error code not present
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */

  MMGSDIUTIL_TMC_MEM_FREE(data_ptr);

  return mmgsdi_status;
}/* mmgsdi_psm_write_file */


/*===========================================================================
FUNCTION MMGSDI_PSM_STORE_DATA

DESCRIPTION
  To populate and store PSM UICC profile data on reception of task stop signal

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_psm_store_data
(
  void
)
{
  mmgsdi_return_enum_type      mmgsdi_status                         = MMGSDI_ERROR;
  boolean                      psm_uicc_path_is_available            = FALSE;
  uint8*                       psm_data_ptr                          = NULL;
  mmgsdi_slot_data_type       *slot_data_ptr                         = NULL;
  uint8                        index                                 = 0;
  uint8                        app_index                             = 0;
  uint8                        usim_app_index                        = MMGSDI_MAX_EXT_APP_INFO;
  uint8                        csim_app_index                        = MMGSDI_MAX_EXT_APP_INFO;
  uint8                        prov_index                            = 0;
  mmgsdi_cache_init_enum_type  cache_state                           = MMGSDI_CACHE_MAX_ENUM;
  mmgsdi_nv_item_cache_type    mmgsdi_nv_data                        = {0};
  nv_stat_enum_type            nv_status                             = NV_FAIL_S;
  mmgsdi_access_type           access;
  mmgsdi_data_type             file_data;
#ifdef FEATURE_MMGSDI_3GPP2
  boolean   rtre_nv_only                                             = FALSE;
#endif /* FEATURE_MMGSDI_3GPP2 */
  mmgsdi_psm_mode_enum_type    psm_mode                              = MMGSDI_PSM_MODE_DISABLED;

  memset(&access, 0x00, sizeof(mmgsdi_access_type));
  memset(&file_data, 0x00, sizeof(mmgsdi_data_type));

  psm_uicc_path_is_available = mmgsdi_psm_check_uicc_path();
  if(!psm_uicc_path_is_available)
  {
    UIM_MSG_ERR_0("MMGSDI PSM EFS or SFS path is not present");
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_psm_data_ptr);
    return MMGSDI_ERROR;
  }

  /* Instead of relying on existing PSM mode global, we need to read PSM
     configuration again because NV values may change on the fly. */
  nv_status = mmgsdi_nv_get_item(NV_UIM_SELECT_DEFAULT_USIM_APP_I,
                                 &mmgsdi_nv_data, MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);

  UIM_MSG_LOW_2("NV_UIM_SELECT_DEFAULT_USIM_APP_I Read: Status: 0x%x, Select Default App.: 0x%x",
                nv_status, mmgsdi_nv_data.item_value.uim_select_default_usim_app);

  if ((nv_status == NV_DONE_S) &&
      (mmgsdi_nv_data.item_value.uim_select_default_usim_app == FALSE))
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_psm_read_psm_configuration();

  psm_mode = mmgsdi_psm_get_enabled_status();

  /* First check PSM NVs are enabled on not */
  if (psm_mode == MMGSDI_PSM_MODE_DISABLED)
  {
    UIM_MSG_MED_0("PSM mode is not enabled");
    return MMGSDI_ERROR;
  }

  memset(&mmgsdi_nv_data, 0x00, sizeof(mmgsdi_nv_item_cache_type));

  /* Add check for Test SIM */
  nv_status = mmgsdi_nv_get_item(NV_GPRS_ANITE_GCF_I, &mmgsdi_nv_data,
                                 MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);
  if (nv_status == NV_DONE_S &&
      mmgsdi_nv_data.item_value.gprs_anite_gcf)
  {
    UIM_MSG_MED_0("GPRS ANITE GCF NV is set to 1");
    return MMGSDI_ERROR;
  }

  /* Check if SLOT data ptr is available */
  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(MMGSDI_SLOT_1);

  if(slot_data_ptr == NULL || slot_data_ptr->mmgsdi_state != MMGSDI_STATE_CARD_INSERTED ||
     slot_data_ptr->protocol != MMGSDI_UICC)
  {
    return MMGSDI_ERROR;
  }

#ifdef FEATURE_MMGSDI_3GPP2
  /* Get RTRE configuration and if RTRE is NV_ONLY then don't check
     CSIM app activation status */
  nv_status = mmgsdi_nv_get_item(NV_RTRE_CONFIG_I, &mmgsdi_nv_data,
                                 MMGSDI_NV_CONTEXT_DEVICE_SPECIFIC);
  if((nv_status == NV_DONE_S) &&
     (mmgsdi_nv_data.item_value.rtre_config == NV_RTRE_CONFIG_NV_ONLY))
  {
    rtre_nv_only = TRUE;
  }
#endif /* FEATURE_MMGSDI_3GPP2 */

  /* Check if USIM/CSIM app is a provisioning app, in READY state and PIN is
     disabled */
  for(index = 0; index < MMGSDI_MAX_EXT_APP_INFO; index++)
  {
    if(slot_data_ptr->app_info_pptr[index] == NULL)
    {
      continue;
    }

    if(slot_data_ptr->app_info_pptr[index]->app_data.app_type == MMGSDI_APP_USIM ||
       slot_data_ptr->app_info_pptr[index]->app_data.app_type == MMGSDI_APP_CSIM)
    {
      if(slot_data_ptr->app_info_pptr[index]->app_data.aid.data_len > 0 &&
         slot_data_ptr->app_info_pptr[index]->app_state == MMGSDI_APP_STATE_READY)
      {
        for(prov_index = 0; prov_index < MMGSDI_MAX_PROV_APPS; prov_index++)
        {
          if(slot_data_ptr->prov_app_indices[prov_index] ==
             (uint32)(index | MMGSDI_PROV_INDEX_VALIDITY_MASK))
          {
            if(slot_data_ptr->app_info_pptr[index]->app_data.app_type == MMGSDI_APP_USIM)
            {
              usim_app_index = index;
            }
            else
            {
              csim_app_index = index;
            }
          }
        }
      }
      else
      {
        /* if USIM/CSIM App index is already found no need to return; we have
           more than one USIM/CSIM app */
#ifdef FEATURE_MMGSDI_3GPP2
        if((slot_data_ptr->app_info_pptr[index]->app_data.app_type == MMGSDI_APP_USIM &&
            usim_app_index == MMGSDI_MAX_EXT_APP_INFO) ||
           (slot_data_ptr->app_info_pptr[index]->app_data.app_type == MMGSDI_APP_CSIM &&
            csim_app_index == MMGSDI_MAX_EXT_APP_INFO &&
            rtre_nv_only == FALSE))
#else
        if(slot_data_ptr->app_info_pptr[index]->app_data.app_type == MMGSDI_APP_USIM &&
           usim_app_index == MMGSDI_MAX_EXT_APP_INFO)
#endif /* FEATURE_MMGSDI_3GPP2 */
        {
          return MMGSDI_ERROR;
        }
      }
    }
  }

  /* Do not store PSM data into SFS if USIM PIN1 status is not disabled and 
     PSM PIN1 caching support NV is not enabled */
  if(usim_app_index >= MMGSDI_MAX_EXT_APP_INFO ||
     slot_data_ptr->app_info_pptr[usim_app_index]->pin1_ptr == NULL ||
     (slot_data_ptr->app_info_pptr[usim_app_index]->pin1_ptr->status != MMGSDI_PIN_DISABLED &&
      !(mmgsdi_psm_nv73957_value & MMGSDI_PSM_PIN1_CACHE_MASK)))
  {
    UIM_MSG_MED_1("USIM PIN is enabled: mmgsdi_psm_nv73957_value 0x%x",
                  mmgsdi_psm_nv73957_value);
    return MMGSDI_ERROR;
  }

  /* Do not store PSM data into SFS if CSIM PIN1 status is not disabled and 
     PSM PIN1 caching support NV is not enabled and
     key ref for both USIM and CSIM is not same */
#ifdef FEATURE_MMGSDI_3GPP2
  if(csim_app_index < MMGSDI_MAX_EXT_APP_INFO &&
     slot_data_ptr->app_info_pptr[csim_app_index]->pin1_ptr != NULL &&
     slot_data_ptr->app_info_pptr[csim_app_index]->pin1_ptr->status != MMGSDI_PIN_DISABLED &&
     !(slot_data_ptr->app_info_pptr[usim_app_index]->pin1_ptr->key_ref ==
       slot_data_ptr->app_info_pptr[csim_app_index]->pin1_ptr->key_ref) &&
     !(mmgsdi_psm_nv73957_value & MMGSDI_PSM_PIN1_CACHE_MASK))
  {
    UIM_MSG_MED_1("CSIM PIN is enabled or key ref is not same for both apps: mmgsdi_psm_nv73957_value 0x%x",
                  mmgsdi_psm_nv73957_value);
    return MMGSDI_ERROR;
  }
#endif /* FEATURE_MMGSDI_3GPP2 */

  if(mmgsdi_psm_data_ptr != NULL)
  {
    memset(mmgsdi_psm_data_ptr, 0x00, sizeof(mmgsdi_psm_data_type));
  }
  else
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_psm_data_ptr,
                                       sizeof(mmgsdi_psm_data_type));
    if(mmgsdi_psm_data_ptr == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
  }

  /* Populate slot id and card type */
  mmgsdi_psm_data_ptr->card_type = slot_data_ptr->protocol;

  /* Populate ICCID */
  access.access_method  = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = MMGSDI_ICCID;

  file_data.data_len = MMGSDI_ICCID_LEN; 
  file_data.data_ptr = mmgsdi_psm_data_ptr->iccid_data.data;

  mmgsdi_status = mmgsdi_cache_read(mmgsdi_generic_data_ptr->slot_session_id[MMGSDI_SLOT_1_INDEX],
                                    &access,
                                    &file_data,
                                    0,
                                    &cache_state);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_2("failed to read cache for ICCID: mmgsdi_status 0x%x, cache_state 0x%x",
                  mmgsdi_status, cache_state);
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_psm_data_ptr);
    return mmgsdi_status;
  }

  mmgsdi_psm_data_ptr->iccid_data.length = (uint8)file_data.data_len;

  /* Populate USIM app info. USIM is always on first position of array stored
     in the SFS, regardless of its position in EF-DIR */
  mmgsdi_memscpy(&mmgsdi_psm_data_ptr->app_data[0],
                 sizeof(mmgsdi_psm_data_ptr->app_data[0]),
                 &slot_data_ptr->app_info_pptr[usim_app_index]->app_data,
                 sizeof(slot_data_ptr->app_info_pptr[usim_app_index]->app_data)); 
  
  /* Store USIM app capability before entering PSM.
     mmgsdi_psm_data_ptr is packed. so can't copy it as a whole */
  mmgsdi_psm_data_ptr->app_cap[0].acl_state = slot_data_ptr->app_info_pptr[usim_app_index]->app_capabilities.acl_state;
  mmgsdi_psm_data_ptr->app_cap[0].fdn_state = slot_data_ptr->app_info_pptr[usim_app_index]->app_capabilities.fdn_state;
  mmgsdi_psm_data_ptr->app_cap[0].cc_state = slot_data_ptr->app_info_pptr[usim_app_index]->app_capabilities.cc_state;
  mmgsdi_psm_data_ptr->app_cap[0].bdn_state = slot_data_ptr->app_info_pptr[usim_app_index]->app_capabilities.bdn_state;
  mmgsdi_psm_data_ptr->app_cap[0].imsi_invalidated = slot_data_ptr->app_info_pptr[usim_app_index]->app_capabilities.imsi_invalidated;
  mmgsdi_psm_data_ptr->app_cap[0].all_srv_init = slot_data_ptr->app_info_pptr[usim_app_index]->app_capabilities.all_srv_init;
  mmgsdi_psm_data_ptr->app_cap[0].unknown_capabilities = slot_data_ptr->app_info_pptr[usim_app_index]->app_capabilities.unknown_capabilities;

  app_index++;

  /* Copying PIN data to SFS */
  mmgsdi_psm_data_ptr->pin_status = slot_data_ptr->app_info_pptr[usim_app_index]->pin1_ptr->status;
  mmgsdi_psm_data_ptr->pin_data   = slot_data_ptr->app_info_pptr[usim_app_index]->pin1_ptr->pin_data;

#ifdef FEATURE_MMGSDI_3GPP2
  /* Populate CSIM app info. CSIM is always on second position of array stored
     in the SFS if present, regardless of its position in EF-DIR */
  if(csim_app_index != MMGSDI_MAX_EXT_APP_INFO)
  {
    mmgsdi_memscpy(&mmgsdi_psm_data_ptr->app_data[1],
                   sizeof(mmgsdi_psm_data_ptr->app_data[1]),
                   &slot_data_ptr->app_info_pptr[csim_app_index]->app_data,
                   sizeof(slot_data_ptr->app_info_pptr[csim_app_index]->app_data)); 

    /* Store CSIM app capability before entering PSM.
       mmgsdi_psm_data_ptr is packed. so can't copy it as a whole */
    mmgsdi_psm_data_ptr->app_cap[1].acl_state = slot_data_ptr->app_info_pptr[csim_app_index]->app_capabilities.acl_state;
    mmgsdi_psm_data_ptr->app_cap[1].fdn_state = slot_data_ptr->app_info_pptr[csim_app_index]->app_capabilities.fdn_state;
    mmgsdi_psm_data_ptr->app_cap[1].cc_state = slot_data_ptr->app_info_pptr[csim_app_index]->app_capabilities.cc_state;
    mmgsdi_psm_data_ptr->app_cap[1].bdn_state = slot_data_ptr->app_info_pptr[csim_app_index]->app_capabilities.bdn_state;
    mmgsdi_psm_data_ptr->app_cap[1].imsi_invalidated = slot_data_ptr->app_info_pptr[csim_app_index]->app_capabilities.imsi_invalidated;
    mmgsdi_psm_data_ptr->app_cap[1].all_srv_init = slot_data_ptr->app_info_pptr[csim_app_index]->app_capabilities.all_srv_init;
    mmgsdi_psm_data_ptr->app_cap[1].unknown_capabilities = slot_data_ptr->app_info_pptr[csim_app_index]->app_capabilities.unknown_capabilities;
    app_index++;
  }
#endif /* FEATURE_MMGSDI_3GPP2 */

  /* Populate all other apps listed in EF-DIR */
  for(index = 0; index < MMGSDI_MAX_EXT_APP_INFO && app_index < MMGSDI_MAX_APP_INFO; index++)
  {
    if(slot_data_ptr->app_info_pptr[index] == NULL ||
       slot_data_ptr->app_info_pptr[index]->app_not_in_efdir != FALSE ||
       index == usim_app_index ||
       index == csim_app_index)
    {
      continue;
    }
    mmgsdi_memscpy(&mmgsdi_psm_data_ptr->app_data[app_index],
                   sizeof(mmgsdi_psm_data_ptr->app_data[app_index]),
                   &slot_data_ptr->app_info_pptr[index]->app_data,
                   sizeof(slot_data_ptr->app_info_pptr[index]->app_data)); 
    app_index++;
  }

  /* Store OPL valid record list in EFS.
     Use num_of_valid_rec as input/output parameter, Input to provide max
     length of buffer and output to get copied data length. */
  mmgsdi_psm_data_ptr->opl_record.num_of_valid_rec =
    MMGSDI_MAX_RECS_IN_LINEAR_FIXED_EF;

  /* Possible that valid record list is not present; all records may be invalid */
  if(FALSE == mmgsdi_eons_opl_pnn_is_valid_record_list_updated(
                mmgsdi_generic_data_ptr->pri_gw_session_id, 
                MMGSDI_USIM_OPL,
                &mmgsdi_psm_data_ptr->opl_record.num_of_valid_rec,
                mmgsdi_psm_data_ptr->opl_record.valid_rec_list))
  {
    mmgsdi_psm_data_ptr->opl_record.num_of_valid_rec = 0;
  }

  /* Store the system time in the EFS. This will be used upon restart to see if
     the device was power cycled or not. */
  mmgsdi_psm_data_ptr->system_time = time_get_ms_native();

  /* Allocate memory to copy data from global variable so that */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(psm_data_ptr, 
                                     sizeof(mmgsdi_psm_data_type));
  if(psm_data_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_psm_data_ptr);
    return  MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  mmgsdi_memscpy(psm_data_ptr,
                 sizeof(mmgsdi_psm_data_type),
                 mmgsdi_psm_data_ptr,
                 sizeof(mmgsdi_psm_data_type));

  /*We can free the global variable now as it is no longer used */
  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_psm_data_ptr);

#ifndef FEATURE_UIM_TEST_FRAMEWORK
  mmgsdi_status = mmgsdi_sfs_write(psm_data_ptr,
                                   sizeof(mmgsdi_psm_data_type),
                                   mmgsdi_sfs_psm_uicc_profile);
#else
  #error code not present
#endif /* !FEATURE_UIM_TEST_FRAMEWORK */

  MMGSDIUTIL_TMC_MEM_FREE(psm_data_ptr);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*Write USIM Sensitive files data to the SFS*/
  mmgsdi_status = mmgsdi_psm_write_file(mmgsdi_sfs_psm_usim_sensitive_ef_data,
                                        TRUE,
                                        MMGSDI_GW_PROV_PRI_SESSION);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*Write USIM non-sensitive files data to the EFS*/
  mmgsdi_status = mmgsdi_psm_write_file(mmgsdi_efs_psm_usim_non_sensitive_ef_data,
                                        FALSE,
                                        MMGSDI_GW_PROV_PRI_SESSION);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

#ifdef FEATURE_MMGSDI_3GPP2
  if(csim_app_index != MMGSDI_MAX_EXT_APP_INFO)
  {
    /*Write CSIM Sensitive files data to the SFS*/
    mmgsdi_status = mmgsdi_psm_write_file(mmgsdi_sfs_psm_csim_sensitive_ef_data,
                                          TRUE,
                                          MMGSDI_1X_PROV_PRI_SESSION);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }

    /*Write CSIM non-sensitive files data to the EFS*/
    mmgsdi_status = mmgsdi_psm_write_file(mmgsdi_efs_psm_csim_non_sensitive_ef_data,
                                          FALSE,
                                          MMGSDI_1X_PROV_PRI_SESSION);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }
  }
#endif /* FEATURE_MMGSDI_3GPP2 */

  return mmgsdi_status;
}/* mmgsdi_psm_store_data */


/*===========================================================================
FUNCTION MMGSDI_PSM_GET_ENABLED_STATUS

DESCRIPTION
   Return the status whether UIM PSM NV, MMODE PSM NV and NV 4398
   auto provisioning are enabled or not.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_psm_mode_enum_type mmgsdi_psm_get_enabled_status(
  void
)
{
  /* return the status whether PSM is enabled or not */
  return mmgsdi_psm_mode;
}/* mmgsdi_psm_get_enabled_status */


/* ==========================================================================
FUNCTION MMGSDI_PSM_UTIL_INTERNAL_CB

DESCRIPTION
  This function, defined as mmgsdi_callback_type, will handle the internal
  commands  generated by MMGSDI PSM

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
----------------------------------------------------------------------------*/
static void mmgsdi_psm_util_internal_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr)
{
  (void)status;
  (void)cnf;
  (void)cnf_ptr;
}/*mmgsdi_psm_util_internal_cb*/


/* ===========================================================================
FUNCTION:      MMGSDI_PSM_UTIL_SELECT_ADF

DESCRIPTION:
  This function sends SELECT for correct ADF 

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type

  MMGSDI_SUCCESS:          SELECT operation was successful
  MMGSDI_ERROR:            SELECT operation was successful

SIDE EFFECTS:
  None.
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_util_select_adf(
  mmgsdi_session_id_type   session_id
)
{
  mmgsdi_return_enum_type              mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_get_file_attr_req_type       *select_ptr    = NULL;
  mmgsdi_get_file_attr_cnf_type       *cnf_ptr       = NULL;
  mmgsdi_int_app_info_type            *sel_app_ptr   = NULL;
  mmgsdi_slot_id_enum_type             slot          = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_get_file_info_extra_info_type extra_info;

  memset(&extra_info, 0x00, sizeof(mmgsdi_get_file_info_extra_info_type));

  mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                   &slot,
                                                   NULL,
                                                   &sel_app_ptr,
                                                   NULL,
                                                   NULL,
                                                   NULL);
  if (sel_app_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if((sel_app_ptr->app_data.app_type != MMGSDI_APP_USIM) &&
     (sel_app_ptr->app_data.app_type != MMGSDI_APP_CSIM))
  {
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(select_ptr,
    sizeof(mmgsdi_get_file_attr_req_type));
  if (select_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  select_ptr->request_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  select_ptr->request_header.session_id        = session_id;
  select_ptr->request_header.request_type      = MMGSDI_GET_FILE_ATTR_REQ;
  select_ptr->request_header.orig_request_type = MMGSDI_GET_FILE_ATTR_REQ;
  select_ptr->request_header.slot_id           = slot;
  select_ptr->request_header.client_data       = 0;
  select_ptr->request_header.response_cb       = NULL;
  select_ptr->access.access_method             = MMGSDI_BY_APP_ID_ACCESS;
  /* Since we do not care about the sec attributes, skip the check */
  select_ptr->skip_uicc_arr                    = TRUE;

  mmgsdi_memscpy(&select_ptr->access.file.app_id,
                 sizeof(select_ptr->access.file.app_id),
                 &sel_app_ptr->app_data.aid,
                 sizeof(mmgsdi_static_data_type));

  select_ptr->activate_aid                     = TRUE;
  select_ptr->select_rsp_type                  = MMGSDI_SELECT_RSP_FCP;
  select_ptr->select_rsp_requested             = TRUE;

  mmgsdi_status = mmgsdi_uim_uicc_select (select_ptr,
                                          TRUE,
                                          &cnf_ptr,
                                          TRUE);
  if (cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("Cnf Ptr Null after SELECT ADF");
    MMGSDIUTIL_TMC_MEM_FREE(select_ptr);
    return MMGSDI_ERROR;
  }

  if( (mmgsdi_status != MMGSDI_SUCCESS) ||
      (cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS))
  {
    UIM_MSG_ERR_0("Select ADF failed");
    MMGSDIUTIL_TMC_MEM_FREE(select_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);
    return MMGSDI_ERROR;
  }

  if(mmgsdi_uicc_rsp_parse_select_response_adf(
       sel_app_ptr,
       cnf_ptr->raw_file_attr_data.data_ptr,
       (uint16)cnf_ptr->raw_file_attr_data.data_len,
       MMGSDI_SELECT_RSP_FCP) !=
     MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("SELECT response could not be parsed");
    MMGSDIUTIL_TMC_MEM_FREE(select_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(cnf_ptr);
    return MMGSDI_ERROR;
  }

  extra_info.select_rsp_type = MMGSDI_SELECT_RSP_FCP;
  extra_info.access.access_method = MMGSDI_BY_APP_ID_ACCESS;
  mmgsdi_memscpy(&extra_info.access.file.app_id,
                 sizeof(extra_info.access.file.app_id),
                 &sel_app_ptr->app_data.aid,
                 sizeof(mmgsdi_static_data_type));

  /* Reset the PIN1 status before decoding */
  sel_app_ptr->pin1_ptr->status = MMGSDI_PIN_STATUS_NOT_INITIALIZED;

  mmgsdi_status =  mmgsdi_uicc_rsp_decode_pin_header_data (
      &(select_ptr->request_header),
      &extra_info,
      cnf_ptr->raw_file_attr_data.data_ptr,
      cnf_ptr->raw_file_attr_data.data_len);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Select ADF failed");
    mmgsdi_status = MMGSDI_ERROR;
  }

  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(select_ptr);
  return mmgsdi_status;
} /* mmgsdi_psm_util_select_adf */


/* ===========================================================================
FUNCTION      MMGSDI_PSM_UTIL_SEND_STATUS

DESCRIPTION
  This function will send the STATUS command 

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  mmgsdi_return_enum_type

  MMGSDI_SUCCESS:          The command processing was successful.
  MMGSDI_ERROR:            The command processing was not successful.
  MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                           within appropriate ranges.
  MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                           but the service table indicates the card does not
                           have the support

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_util_send_status(
  mmgsdi_session_id_type    session_id,
  mmgsdi_slot_id_enum_type  slot_id
)
{
  mmgsdi_status_req_type       status_req;
  mmgsdi_status_cnf_type     * cnf_ptr       = NULL;
  mmgsdi_return_enum_type      mmgsdi_status = MMGSDI_ERROR;

  memset(&status_req, 0, sizeof(status_req));

  status_req.request_header.request_type      = MMGSDI_CARD_STATUS_REQ;
  status_req.request_header.orig_request_type = MMGSDI_CARD_STATUS_REQ;
  status_req.request_header.session_id        = session_id;
  status_req.request_header.slot_id           = slot_id;
  status_req.me_app_status                    = MMGSDI_STATUS_APP_TERMINATED;
  status_req.ret_data_struct                  = MMGSDI_STATUS_NO_DATA;

  mmgsdi_status = mmgsdi_uim_uicc_status(&status_req, TRUE, &cnf_ptr, NULL);

  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type *) cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_MED_0("Unable to send STATUS command ");
  }

  return mmgsdi_status;
} /* mmgsdi_psm_util_send_status */


/*===========================================================================
FUNCTION MMGSDI_PSM_UTIL_READ_AND_COMPARE_EF

DESCRIPTION
  Function to read and compare EF data with the early init cache and real card

PARAMETERS
  session id :  session id
  file       : file type
  data_ptr   : pointer to data type
  app_type   : app type

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_util_read_and_compare_ef(
  mmgsdi_session_id_type        session_id,
  mmgsdi_file_enum_type         file,
  mmgsdi_app_enum_type          app_type,
  mmgsdi_session_type_enum_type session_type
)
{
  mmgsdi_len_type                     data_len      = 0;
  uint8                              *data_ptr      = NULL;
  mmgsdi_return_enum_type             mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_read_req_type               *read_req_ptr  = NULL;
  mmgsdi_read_cnf_type               *read_cnf_ptr  = NULL;
  mmgsdi_access_type                  access;

  if(mmgsdi_psm_local_cache_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  switch(file)
  {
    case MMGSDI_USIM_LOCI:
      data_len = mmgsdi_psm_local_cache_ptr->loci_len;
      data_ptr = mmgsdi_psm_local_cache_ptr->loci_data;
      break;

    case MMGSDI_USIM_PSLOCI:
      data_len = mmgsdi_psm_local_cache_ptr->psloci_len;
      data_ptr = mmgsdi_psm_local_cache_ptr->psloci_data;
      break;

    case MMGSDI_USIM_EPSLOCI:
      data_len = mmgsdi_psm_local_cache_ptr->epsloci_len;
      data_ptr = mmgsdi_psm_local_cache_ptr->epsloci_data;
      break;

    default:
      /* get the existing cache data into temp location */
      mmgsdi_status = mmgsdi_cache_read_item_len(
                        file,
                        app_type,
                        session_type,
                        0,
                        &data_len,
                        NULL);
      if(mmgsdi_status != MMGSDI_SUCCESS ||
         data_len <= 0)
      {
        return MMGSDI_ERROR;
      }

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(data_ptr, data_len);
      if(data_ptr == NULL)
      {
        return MMGSDI_ERROR;
      }

      mmgsdi_status = mmgsdi_cache_read_item(file,
                                             app_type,
                                             session_type,
                                             data_len,
                                             0,
                                             &data_len,
                                             data_ptr,
                                             NULL);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
        return MMGSDI_ERROR;
      }
      break;
  }

  memset(&access, 0x00, sizeof(access));

  access.access_method  = MMGSDI_EF_ENUM_ACCESS;
  access.file.file_enum = file;

  do
  {
    /* Check status of EF in respective service table */
    mmgsdi_status = mmgsdi_util_is_file_ok_in_svc_table(mmgsdi_generic_data_ptr->client_id,
                                                        session_id,
                                                        app_type,
                                                        &access);
    if (MMGSDI_SUCCESS != mmgsdi_status)
    {
      /* In case PSM cache is empty and the access from card also returned
         NOT_SUPPORTED due to UST setting, consider it as sucessful match */
      if(file == MMGSDI_USIM_EPSLOCI &&
         mmgsdi_status == MMGSDI_NOT_SUPPORTED &&
         mmgsdi_psm_local_cache_ptr->epsloci_len == 0)
      {
        mmgsdi_status = MMGSDI_SUCCESS;
      }

      break;
    }

    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(read_req_ptr,
      sizeof(mmgsdi_read_req_type));
    if (read_req_ptr == NULL)
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* Prepare read request to read the file from card */
    read_req_ptr->request_header.session_id        = session_id;
    read_req_ptr->request_header.request_type      = MMGSDI_READ_REQ;
    read_req_ptr->request_header.orig_request_type = MMGSDI_READ_REQ;
    read_req_ptr->request_header.slot_id           = MMGSDI_SLOT_1;
    read_req_ptr->request_header.client_data       = 0;
    read_req_ptr->request_header.response_cb       = NULL;
    read_req_ptr->access                           = access;
    read_req_ptr->rec_num                          = 0;
    read_req_ptr->offset                           = 0;
    read_req_ptr->file_type                        = MMGSDI_TRANSPARENT_FILE;

    mmgsdi_status = mmgsdi_uim_uicc_read_transparent(read_req_ptr,
                                                     TRUE,
                                                     &read_cnf_ptr);

    MMGSDIUTIL_TMC_MEM_FREE(read_req_ptr);

    if (mmgsdi_status != MMGSDI_SUCCESS ||
        read_cnf_ptr == NULL ||
        read_cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS ||
        read_cnf_ptr->read_data.data_ptr == NULL ||
        read_cnf_ptr->read_data.data_len == 0)
    {
      mmgsdi_status = MMGSDI_ERROR;
      break;
    }

    /* mem cmp EF Data */
    if (data_len != read_cnf_ptr->read_data.data_len ||
        (memcmp(data_ptr, read_cnf_ptr->read_data.data_ptr, data_len) != 0))
    {
      mmgsdi_status = MMGSDI_ERROR;
    }
  }while(0);

  if(file != MMGSDI_USIM_LOCI &&
     file != MMGSDI_USIM_PSLOCI &&
     file != MMGSDI_USIM_EPSLOCI)
  {
    MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
  }

  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)read_cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(read_cnf_ptr);

  return mmgsdi_status;
}/*mmgsdi_psm_util_read_and_compare_ef*/


/*===========================================================================
FUNCTION MMGSDI_PSM_UTIL_OPEN_CHANNEL

DESCRIPTION
  Perform open channel request

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_util_open_channel(
  mmgsdi_session_id_type   session_id,
  mmgsdi_channel_id_type   channel_id
)
{
  mmgsdi_request_header_type          req_header     = {0};
  mmgsdi_return_enum_type             mmgsdi_status  = MMGSDI_ERROR;
  mmgsdi_session_deactivate_cnf_type *cnf_ptr        = NULL;
  mmgsdi_channel_id_type              out_channel_id = 0xff;

  req_header.client_id         = mmgsdi_generic_data_ptr->client_id;
  req_header.session_id        = session_id;
  req_header.channel_id        = 0;
  req_header.request_type      = MMGSDI_MANAGE_CHANNEL_REQ;
  req_header.slot_id           = MMGSDI_SLOT_1;
  req_header.orig_request_type = MMGSDI_MANAGE_CHANNEL_REQ;

  /* open channel */
  mmgsdi_status = mmgsdi_uim_uicc_manage_channel_op(&req_header,
                                                    UIM_CHANNEL0,
                                                    UIM_MC_OPEN,
                                                    NULL,
                                                    TRUE,
                                                    &cnf_ptr,
                                                    &out_channel_id);
  if (mmgsdi_status != MMGSDI_SUCCESS ||
      cnf_ptr == NULL ||
      (cnf_ptr != NULL && 
       cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS))
  {
    UIM_MSG_ERR_1("Channel id open failed for channel:0x%x:", channel_id);
    mmgsdi_status = MMGSDI_ERROR;
  }

  /* If channel returned from Card is different from previosuly initialized at psm init
     - update channel in channel info table */
  if (mmgsdi_status == MMGSDI_SUCCESS &&
      out_channel_id != channel_id)
  {
    mmgsdi_channel_info_type *channel_info_ptr = NULL;

    mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                     NULL,
                                                     NULL,
                                                     NULL,
                                                     &channel_info_ptr,
                                                     NULL,
                                                     NULL);
    if (mmgsdi_status != MMGSDI_SUCCESS ||
        channel_info_ptr == NULL)
    {
      mmgsdi_status = MMGSDI_ERROR;
    }
    else
    {
      channel_info_ptr->channel_id = out_channel_id;
    }
  }

  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cnf_ptr);

  return mmgsdi_status;
}/*mmgsdi_psm_util_open_channel*/


/*===========================================================================
FUNCTION MMGSDI_PSM_UTIL_UPDATE_TST

DESCRIPTION
  Perform EF TST WRITE if applicable

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_psm_util_update_tst(
  void
)
{
  mmgsdi_len_type          data_len      = 0;
  uint8                   *data_ptr      = NULL;
  mmgsdi_write_req_type   *req_ptr       = NULL;
  mmgsdi_write_cnf_type   *read_cnf_ptr  = NULL;
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;

  /* get the existing cache data into temp location */
  mmgsdi_status = mmgsdi_cache_read_item_len(
                    MMGSDI_USIM_7F66_PROP1_SPT_TABLE,
                    MMGSDI_APP_USIM,
                    MMGSDI_GW_PROV_PRI_SESSION,
                    0,
                    &data_len,
                    NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS || data_len <= 0)
  {
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(data_ptr, data_len);
  if (data_ptr == NULL)
  {
    return;
  }
  
  mmgsdi_status = mmgsdi_cache_read_item(MMGSDI_USIM_7F66_PROP1_SPT_TABLE,
                                         MMGSDI_APP_USIM,
                                         MMGSDI_GW_PROV_PRI_SESSION,
                                         data_len,
                                         0,
                                         &data_len,
                                         data_ptr,
                                         NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(req_ptr,
                                     sizeof(mmgsdi_write_req_type));
  if (req_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
    return;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(req_ptr->data.data_ptr, data_len);
  if (req_ptr->data.data_ptr == NULL)
  {
    MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
    return;
  }
  
  req_ptr->request_header.session_id         = mmgsdi_generic_data_ptr->pri_gw_session_id;
  req_ptr->request_header.slot_id            = MMGSDI_SLOT_1;
  req_ptr->request_header.request_type       = MMGSDI_WRITE_REQ;
  req_ptr->request_header.orig_request_type  = MMGSDI_WRITE_REQ;
  req_ptr->access.access_method              = MMGSDI_EF_ENUM_ACCESS;
  req_ptr->access.file.file_enum             = MMGSDI_USIM_7F66_PROP1_SPT_TABLE;
  req_ptr->rec_num                           = 0;
  req_ptr->offset                            = 0;
  req_ptr->file_type                         = MMGSDI_TRANSPARENT_FILE;
  req_ptr->data.data_len                     = data_len;

  mmgsdi_memscpy(req_ptr->data.data_ptr,
                 data_len,
                 data_ptr,
                 data_len);

  mmgsdi_status = mmgsdi_uim_uicc_write_transparent(req_ptr, TRUE, &read_cnf_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_1("Failed to write to SPT Table: Status = 0x%x", mmgsdi_status);
  }
  
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(req_ptr->data.data_ptr);
  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)read_cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(read_cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(data_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
}/*mmgsdi_psm_util_update_tst*/


/*===========================================================================
FUNCTION MMGSDI_PSM_SILENT_PIN_VERIFY_PIN

DESCRIPTION
  This function will send the PIN verification request with the cached PIN
  data.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_silent_pin_verify_pin(
  mmgsdi_session_id_type            session_id,
  mmgsdi_static_data_type           pin_data
)
{
  mmgsdi_protocol_enum_type         protocol_type = MMGSDI_MAX_PROTOCOL_ENUM;
  mmgsdi_return_enum_type           mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_pin_operation_req_type   * req_ptr       = NULL;

  /* End PSM init if PSM PIN1 cahing NV is disabled */
  if(!(mmgsdi_psm_nv73957_value & MMGSDI_PSM_PIN1_CACHE_MASK))
  {
    UIM_MSG_ERR_0("MMGSDI PSM PIN caching NV support is disabled and PIN status is not DISABLED - end PSM init");
    return MMGSDI_ERROR;
  }

  UIM_MSG_HIGH_0("mmgsdi_psm_silent_pin_verify_pin()");

  /* Get the MMGSDI protocol */
  protocol_type = mmgsdi_util_determine_protocol(MMGSDI_SLOT_1);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(req_ptr, sizeof(mmgsdi_pin_operation_req_type));
  if (req_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /*---------------------------------------------------------------------------
    Populating mmgsdi_pin_operation_req_type header
      1) Session ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  req_ptr->request_header.slot_id           = MMGSDI_SLOT_1;
  req_ptr->request_header.session_id        = session_id;
  req_ptr->request_header.request_type      = MMGSDI_PIN_OPERATION_REQ;
  req_ptr->request_header.orig_request_type = MMGSDI_PIN_OPERATION_REQ;
  req_ptr->request_header.client_data       = 0;
  req_ptr->request_header.response_cb       = mmgsdi_psm_util_internal_cb;
  req_ptr->pin_op                           = MMGSDI_PIN_OP_VERIFY;
  req_ptr->pin_id                           = MMGSDI_PIN1;
  req_ptr->orig_pin_id                      = MMGSDI_PIN1;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_pin_operation_req_type content
    - pin_op - MMGSDI_PIN_OP_VERIFY
    - pin_id - pin_id
    - pin_puk - pin_data
    ---------------------------------------------------------------------------*/
  req_ptr->pin_puk.data_len = pin_data.data_len;

  mmgsdi_memscpy(req_ptr->pin_puk.data_ptr,
                 sizeof(req_ptr->pin_puk.data_ptr),
                 pin_data.data_ptr,
                 pin_data.data_len);

  mmgsdi_status = mmgsdi_uim_common_unblock_verify_pin(req_ptr,
                                                       MMGSDI_RETRY_NONE,
                                                       protocol_type,
                                                       TRUE);

  MMGSDIUTIL_TMC_MEM_FREE(req_ptr);

  /* PIN1 RSP failure */
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("Silent PIN verfication failed - end PSM init");
  }

  return mmgsdi_status;
} /* mmgsdi_psm_silent_pin_verify_pin */


/*===========================================================================
FUNCTION MMGSDI_PSM_HANDLE_USIM_INIT

DESCRIPTION
  Hanldes USIM app initialization in PSM mode - 
  Peforms AID selection, read of IMSI, LOCI, PS LOCI and EPS LOCI from
  card and compare them with PSM data.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_handle_usim_init(
  void
)
{
  mmgsdi_int_app_info_type           *sel_app_ptr   = NULL;
  mmgsdi_channel_id_type              channel_id    = 0;
  mmgsdi_return_enum_type             mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type            slot_id       = MMGSDI_MAX_SLOT_ID_ENUM;

  mmgsdi_status = mmgsdi_util_get_session_app_info(mmgsdi_generic_data_ptr->pri_gw_session_id,
                                                   &slot_id,
                                                   &channel_id,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS ||
      slot_id != MMGSDI_SLOT_1)
  {
    return MMGSDI_ERROR;
  }

  /* if the channel is non-default, open channel before select AID */
  if (channel_id != MMGSDI_DEFAULT_CHANNEL)
  {
    mmgsdi_status = mmgsdi_psm_util_open_channel(
      mmgsdi_generic_data_ptr->pri_gw_session_id,
      channel_id);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return MMGSDI_ERROR;
    }
  }

  mmgsdi_status = mmgsdi_psm_util_select_adf(mmgsdi_generic_data_ptr->pri_gw_session_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Check pin status */
  mmgsdi_status = mmgsdi_util_get_session_app_info(mmgsdi_generic_data_ptr->pri_gw_session_id,
                                                   NULL,
                                                   NULL,
                                                   &sel_app_ptr,
                                                   NULL,
                                                   NULL,
                                                   NULL);
  if (sel_app_ptr == NULL ||
      sel_app_ptr->pin1_ptr == NULL ||
      mmgsdi_psm_local_cache_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* End PSM init if the cache pin status and card pin status is not matching */
  if(!((mmgsdi_psm_local_cache_ptr->pin_status == MMGSDI_PIN_ENABLED_VERIFIED ||
        mmgsdi_psm_local_cache_ptr->pin_status == MMGSDI_PIN_UNBLOCKED ||
        mmgsdi_psm_local_cache_ptr->pin_status == MMGSDI_PIN_CHANGED) &&
       sel_app_ptr->pin1_ptr->status == MMGSDI_PIN_ENABLED_NOT_VERIFIED) &&
     mmgsdi_psm_local_cache_ptr->pin_status != sel_app_ptr->pin1_ptr->status)
  {
    UIM_MSG_ERR_2("Cached pin status 0x%x and card pin status 0x%X are not matching - end PSM init",
                  mmgsdi_psm_local_cache_ptr->pin_status, sel_app_ptr->pin1_ptr->status);
    return MMGSDI_ERROR;
  }

  switch(sel_app_ptr->pin1_ptr->status)
  {
    case MMGSDI_PIN_DISABLED:
      /* PIN is disabled: nothing to do */
      mmgsdi_status = MMGSDI_SUCCESS;
      break;
    case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
      mmgsdi_status = mmgsdi_psm_silent_pin_verify_pin(mmgsdi_generic_data_ptr->pri_gw_session_id,
                                                       sel_app_ptr->pin1_ptr->pin_data);
      break;
    default:
      /* Invalid PIN status */
      mmgsdi_status = MMGSDI_ERROR;
      break;
  }
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Write EF TST if available in cache */
  mmgsdi_psm_util_update_tst();

  /* Read and compare IMSI Data */
  mmgsdi_status = mmgsdi_psm_util_read_and_compare_ef(mmgsdi_generic_data_ptr->pri_gw_session_id,
                    MMGSDI_USIM_IMSI, MMGSDI_APP_USIM, MMGSDI_GW_PROV_PRI_SESSION);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Read and compare LOCI Data */
  mmgsdi_status = mmgsdi_psm_util_read_and_compare_ef(mmgsdi_generic_data_ptr->pri_gw_session_id,
                    MMGSDI_USIM_LOCI, MMGSDI_APP_USIM, MMGSDI_GW_PROV_PRI_SESSION);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Read and compare PSLOCI Data */
  mmgsdi_status = mmgsdi_psm_util_read_and_compare_ef(mmgsdi_generic_data_ptr->pri_gw_session_id,
                    MMGSDI_USIM_PSLOCI, MMGSDI_APP_USIM, MMGSDI_GW_PROV_PRI_SESSION);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Read and compare EPSLOCI Data */
  mmgsdi_status = mmgsdi_psm_util_read_and_compare_ef(mmgsdi_generic_data_ptr->pri_gw_session_id,
                    MMGSDI_USIM_EPSLOCI, MMGSDI_APP_USIM, MMGSDI_GW_PROV_PRI_SESSION);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Init is done for USIM, send STATUS command */
  mmgsdi_status = mmgsdi_psm_util_send_status(mmgsdi_generic_data_ptr->pri_gw_session_id, slot_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
}/* mmgsdi_psm_handle_usim_init */


#ifdef FEATURE_MMGSDI_3GPP2
/*===========================================================================
FUNCTION MMGSDI_PSM_HANDLE_CSIM_INIT

DESCRIPTION
  Hanldes CSIM app initialization in PSM mode - 
  Peforms AID selection, read of IMSI_M from card and compare them with PSM data.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_psm_handle_csim_init(
  void
)
{
  mmgsdi_int_app_info_type           *sel_app_ptr   = NULL;
  mmgsdi_channel_id_type              channel_id    = 0;
  mmgsdi_return_enum_type             mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_slot_id_enum_type            slot_id       = MMGSDI_MAX_SLOT_ID_ENUM;

  mmgsdi_status = mmgsdi_util_get_session_app_info(mmgsdi_generic_data_ptr->pri_1x_session_id,
                                                   &slot_id,
                                                   &channel_id,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS ||
      slot_id != MMGSDI_SLOT_1)
  {
    return MMGSDI_ERROR;
  }

  /* if the channel is non-default, open channel before select AID */
  if (channel_id != MMGSDI_DEFAULT_CHANNEL)
  {
    mmgsdi_status = mmgsdi_psm_util_open_channel(
      mmgsdi_generic_data_ptr->pri_1x_session_id,
      channel_id);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return MMGSDI_ERROR;
    }
  }

  mmgsdi_status = mmgsdi_psm_util_select_adf(mmgsdi_generic_data_ptr->pri_1x_session_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Check pin status */
  mmgsdi_status = mmgsdi_util_get_session_app_info(mmgsdi_generic_data_ptr->pri_1x_session_id,
                                                   NULL,
                                                   NULL,
                                                   &sel_app_ptr,
                                                   NULL,
                                                   NULL,
                                                   NULL);
  if (sel_app_ptr == NULL ||
      sel_app_ptr->pin1_ptr == NULL ||
      mmgsdi_psm_local_cache_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* End PSM init if the cache pin status and card pin status is not matching */
  if(!((mmgsdi_psm_local_cache_ptr->pin_status == MMGSDI_PIN_ENABLED_VERIFIED ||
        mmgsdi_psm_local_cache_ptr->pin_status == MMGSDI_PIN_UNBLOCKED ||
        mmgsdi_psm_local_cache_ptr->pin_status == MMGSDI_PIN_CHANGED) &&
       sel_app_ptr->pin1_ptr->status == MMGSDI_PIN_ENABLED_NOT_VERIFIED) &&
     mmgsdi_psm_local_cache_ptr->pin_status != sel_app_ptr->pin1_ptr->status)
  {
    UIM_MSG_ERR_2("Cached pin status 0x%x and card pin status 0x%X are not matching - end PSM init",
                  mmgsdi_psm_local_cache_ptr->pin_status, sel_app_ptr->pin1_ptr->status);
    return MMGSDI_ERROR;
  }

  switch(sel_app_ptr->pin1_ptr->status)
  {
    case MMGSDI_PIN_DISABLED:
      /* PIN is disabled: nothing to do */
      mmgsdi_status = MMGSDI_SUCCESS;
      break;
    case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
      mmgsdi_status = mmgsdi_psm_silent_pin_verify_pin(mmgsdi_generic_data_ptr->pri_1x_session_id,
                                                       sel_app_ptr->pin1_ptr->pin_data);
      break;
    default:
      /* Invalid PIN status */
      mmgsdi_status = MMGSDI_ERROR;
      break;
  }
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  /* Read and compare IMSI_M Data */
  mmgsdi_status = mmgsdi_psm_util_read_and_compare_ef(mmgsdi_generic_data_ptr->pri_1x_session_id,
                    MMGSDI_CSIM_IMSI_M, MMGSDI_APP_CSIM, MMGSDI_1X_PROV_PRI_SESSION);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_card_init_store_esn(
                    mmgsdi_generic_data_ptr->pri_1x_session_id,
                    slot_id,
                    MMGSDI_APP_CSIM);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("CSIM Store ESN failed - end PSM init");
    return MMGSDI_ERROR;
  }

  /* Init is done for CSIM, send STATUS command */
  mmgsdi_status = mmgsdi_psm_util_send_status(mmgsdi_generic_data_ptr->pri_1x_session_id, slot_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
}/* mmgsdi_psm_handle_csim_init */
#endif /* FEATURE_MMGSDI_3GPP2 */


/*===========================================================================
FUNCTION MMGSDI_PSM_SESSION_CLEANUP_SEND_CARD_ERROR

DESCRIPTION
  This function is invoked when card mismatch is detected, CARD 
  ERR EVENT is triggered and CARD Is reset.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
static void mmgsdi_psm_session_cleanup_send_card_error(
  void
)
{
  mmgsdi_card_err_info_enum_type   prev_err_cause = MMGSDI_CARD_ERR_UNKNOWN_ERROR;
  mmgsdi_slot_data_type           *slot_data_ptr = NULL;

  /* send out card error and session clean up and trigger card reset */


  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(MMGSDI_SLOT_1);

  if (slot_data_ptr == NULL)
  {
    return;
  }

  /* reset the early init flag */
  mmgsdi_psm_early_init_done = FALSE;

  slot_data_ptr->sim_busy = FALSE;

  prev_err_cause = slot_data_ptr->card_error_condition;
  slot_data_ptr->card_error_condition = MMGSDI_CARD_ERR_PWR_DN_CMD_NOTIFY;

  /* Send out the Card Error event and perform Session and app clean-up */
  mmgsdi_evt_build_and_send_card_error(TRUE,
                                       0,
                                       MMGSDI_SLOT_1,
                                       prev_err_cause);

  /* Update the session closed cause. The session changed event
     broadcasts the cause */
  mmgsdi_util_update_session_closed_cause_for_slot(MMGSDI_SLOT_1, 
    MMGSDI_SESSION_CLOSED_CAUSE_CARD_POWER_DOWN);

  mmgsdi_util_close_all_sessions_cleanup_and_notify(MMGSDI_SLOT_1);
  mmgsdi_util_reset_app_data(FALSE, MMGSDI_SLOT_1);

  /* Reset Sanity Timer to default value */
  slot_data_ptr->mmgsdi_uim_sanity_timer_value
    = MMGSDI_UIM_SANITY_TIMER_VALUE;
} /* mmgsdi_psm_session_cleanup_send_card_error */


/*===========================================================================
FUNCTION MMGSDI_PSM_HANDLE_CARD_INIT_POST_TP_DL

DESCRIPTION
  This function handles select of AID and other init operations and
  make sure the early init is done on the same card that is inserted.
  if the card is found to be different, CARD ERROR is triggered and 
  card is reset to continue as a new card

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_psm_handle_card_init_post_tp_dl(
  void
)
{
  mmgsdi_return_enum_type    mmgsdi_status = MMGSDI_ERROR;

  if (mmgsdi_psm_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* TP DL is done
        - send card inserted event for PSM mode (card detected)
        - perform select on USIM
        - check pin
        - read EFs
        - Compare EFs
        - send session events for PSM mode (card detected)
        - send Status
   */
  do
  {
    /* sends card inserted event for PSM mode (card detected) */
    if(mmgsdi_psm_get_enabled_status() == MMGSDI_PSM_MODE_CARD_DETECTED)
    {
      mmgsdi_psm_send_card_inserted_event();
    }

    if (mmgsdi_psm_data_ptr->app_data[0].app_type == MMGSDI_APP_USIM &&
        mmgsdi_psm_data_ptr->app_data[0].aid.data_len != 0)
    {
      mmgsdi_status = mmgsdi_psm_handle_usim_init();
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        break;
      }
    }

#ifdef FEATURE_MMGSDI_3GPP2
    if (mmgsdi_psm_data_ptr->app_data[1].app_type == MMGSDI_APP_CSIM &&
        mmgsdi_psm_data_ptr->app_data[1].aid.data_len != 0)
    {
      mmgsdi_status = mmgsdi_psm_handle_csim_init();
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        break;
      }
    }
#endif /* FEATURE_MMGSDI_3GPP2 */

    /* send session events after all mandatory EFs are verified in
       PSM card detected mode */
    if(mmgsdi_psm_get_enabled_status() == MMGSDI_PSM_MODE_CARD_DETECTED)
    {
      mmgsdi_status = mmgsdi_psm_trigger_session_events(mmgsdi_generic_data_ptr->pri_gw_session_id);
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        break;
      }
#ifdef FEATURE_MMGSDI_3GPP2
      mmgsdi_status = mmgsdi_psm_trigger_session_events(mmgsdi_generic_data_ptr->pri_1x_session_id);
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        break;
      }
#endif /* FEATURE_MMGSDI_3GPP2 */
    }
  }
  while (0);

  /* Since USIM/CSIM init is complete, set mmgsdi_psm_init_in_progress to
     FALSE so that if there is any change in the sensitive/non-sensitive EF's
     data, sensitive SFS/non-sensitive EFS file will be deleted */
  mmgsdi_psm_init_in_progress = FALSE;

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Usage of SFS and EFS is done by now.
    We are here because card init is not successful. So, we will reset completely which means that
    we will not retain the current PSM files in our file system */
    mmgsdi_psm_reset_early_init(TRUE);

    /* Send out card error and session cleanup and start new card */
    UIM_MSG_HIGH_0("Verification of data on the card failed with PSM data");
    mmgsdi_psm_session_cleanup_send_card_error();

    /* Reset the card */
    mmgsdi_status = mmgsdi_gen_build_card_reset(mmgsdi_generic_data_ptr->client_id,
                                                MMGSDI_SLOT_1,
                                                MMGSDI_CARD_RESET_REQ,
                                                UIM_UICC,
                                                MMGSDI_RESET_CAUSE_GENERAL,
                                                mmgsdi_psm_util_internal_cb,
                                                0);
  }
  else
  {
    /* Usage of SFS and EFS is done by now.
       We are here because card init is successful and events are sent successfully.
       So we will reset partially which means that we will retain current PSM files in our file system */
    mmgsdi_psm_reset_early_init(FALSE);
 
    /* Update GSTK to enable FETCH if init is successful */
    gstk_gsdi_set_fetch_status(GSTK_SLOT_1, TRUE);
    mmgsdi_eons_trigger_opl_pnn_cache(mmgsdi_generic_data_ptr->pri_gw_session_id);
  }

  /* Send PSM ready ind to CM as we voted (-)ve because PSM exit process was going on */
  if(mmgsdi_psm_ready_ind_pending)
  {
    cm_generic_rpt_u_type     gen_rpt;

    memset(&gen_rpt, 0x00, sizeof(cm_generic_rpt_u_type));

    /* No need to update timer as UIM doesn't have any timer value */
    gen_rpt.psm_ready_ind.activate_timer = CM_PSM_PERIODIC_UPDATE_TIMER_INVALID;
    gen_rpt.psm_ready_ind.module = CM_PSM_MODULE_UIM;

    mmgsdi_psm_ready_ind_pending = FALSE;
    UIM_MSG_HIGH_0("Sending PSM ready indication to CM" );

    cm_client_send_psm_rpt(CM_CLIENT_PSM_READY_IND, &gen_rpt);
  }

  return mmgsdi_status;
}/* mmgsdi_psm_handle_card_init_post_tp_dl */


/*===========================================================================
FUNCTION MMGSDI_PSM_GET_STATUS_EARLY_INIT

DESCRIPTION
  This function returns the status of PSM early init during the time 
  between early card and session events are sent and the real card is 
  detected. once the real card is detected, this function returns FALSE,
  indicating no impact to command processing and other functionalities

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_psm_get_status_early_init(
  void
)
{
  /* return the status if the early init was done and sub ready was sent */
  return mmgsdi_psm_early_init_done;
}/* mmgsdi_psm_get_status_early_init */


/*===========================================================================
FUNCTION MMGSDI_PSM_ICCID_AND_PROTOCOL_MATCH_DURING_LINK_EST

DESCRIPTION
  This function is invoked on link est of real card to match ICCID
  with psm iccid data  to ensure the early CARD and session events
  are valid

PARAMETERS:
  ICCID of the real card

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS:
  None
===========================================================================*/
boolean mmgsdi_psm_iccid_and_protocol_match_during_link_est(
  mmgsdi_iccid_info_type    iccid,
  mmgsdi_protocol_enum_type protocol
)
{
  mmgsdi_slot_data_type *slot_data_ptr = NULL;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(MMGSDI_SLOT_1);

  if (slot_data_ptr != NULL &&
      protocol == MMGSDI_UICC &&
      slot_data_ptr->protocol == protocol &&
      iccid.length > 0 &&
      iccid.length <= MMGSDI_ICCID_LEN &&
      mmgsdi_psm_data_ptr != NULL &&
      iccid.length == mmgsdi_psm_data_ptr->iccid_data.length &&
      (memcmp(iccid.data, mmgsdi_psm_data_ptr->iccid_data.data, iccid.length) == 0))
  {
    return TRUE;
  }

  /* Mismatch in ICCID -Purge PSM data */
  UIM_MSG_HIGH_0("ICCID data of the card does not match data stored for PSM");
  mmgsdi_psm_reset_early_init(TRUE);

  /* trigger card error and session cleanup if early events are sent */
  if(mmgsdi_psm_get_enabled_status() == MMGSDI_PSM_MODE_EARLY_EVTS)
  {
    mmgsdi_psm_session_cleanup_send_card_error();
  }

  return FALSE;
}/*mmgsdi_psm_iccid_and_protocol_match_during_link_est*/


/*===========================================================================
FUNCTION MMGSDI_PSM_RESET_EARLY_INIT

DESCRIPTION
  This function reset the PSM early init flag and set MMGSDI_TASK_CMD_Q_SIG 
  when card error occurs. MMGSDI Slot1 task does not process this signal until
  early init flag is set to FALSE.

PARAMETERS
  is_complete_reset : Indicates whether to delete all files from PSM's EFS and SFS path or
                      delete only uicc profile file from PSM's SFS path so that remaining files
                      are retained.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_psm_reset_early_init(
  boolean is_complete_reset
)
{
  if(mmgsdi_psm_early_init_done == FALSE)
  {
    return;
  }

  /* Free global PSM data ptr and delete SFS file */
  mmgsdi_psm_purge_data(is_complete_reset);

  /* reset the flag */
  mmgsdi_psm_early_init_done = FALSE;

  MMGSDIUTIL_ENTER_TASK_STATE_CRIT_SECT;

  /* Check the count on the command queue and set signal */
  if (mmgsdi_task_state[MMGSDI_TASK_SLOT1] == MMGSDI_TASK_ACTIVE &&
      (q_cnt(&mmgsdi_high_pri_cmd_q[MMGSDI_TASK_SLOT1]) > 0 ||
       q_cnt(&mmgsdi_task_cmd_q[MMGSDI_TASK_SLOT1]) > 0))
  {
    /* Set the command queue signal */
    (void) rex_set_sigs(UIM_GSDI_SLOT1_TCB, MMGSDI_TASK_CMD_Q_SIG);
  }

  MMGSDIUTIL_LEAVE_TASK_STATE_CRIT_SECT;
}/* mmgsdi_psm_reset_early_init */


/*===========================================================================
FUNCTION   MMGSDI_PROCESS_PSM_CONTEXT_SAVE_REQ

DESCRIPTION
  This function will process PSM enter request and call
  mmgsdi_psm_store_data() to store context to SFS.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void mmgsdi_process_psm_context_save_req(
  void
)
{
  cm_generic_rpt_u_type     gen_rpt;
  mmgsdi_return_enum_type   mmgsdi_status = MMGSDI_ERROR;

  memset(&gen_rpt, 0x00, sizeof(cm_generic_rpt_u_type));

  /* To store UICC profile data in SFS for PSM mode */
  mmgsdi_status = mmgsdi_psm_store_data();
  gen_rpt.psm_enter_rsp.module = CM_PSM_MODULE_UIM;

  /* Populating PSM ENTER RESP and call CM function to post the response */
  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    gen_rpt.psm_enter_rsp.status = TRUE;
  }
  else
  {
    gen_rpt.psm_enter_rsp.status = FALSE;
  }

  UIM_MSG_HIGH_2("MMGSDI PSM ENTER RESP: status 0x%x, mmgsdi_status 0x%x",
                 gen_rpt.psm_enter_rsp.status,
                 mmgsdi_status);

  cm_client_send_psm_rpt(CM_CLIENT_PSM_ENTER_RSP, &gen_rpt); 
} /* mmgsdi_process_psm_context_save_req */


/*===========================================================================
FUNCTION   MMGSDI_PSM_PIN_EVT

DESCRIPTION
  This function will process PIN1 event and send PSM_READY_IND if it is
  pending due to PIN status.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void mmgsdi_psm_pin_evt(
  const mmgsdi_pin_evt_info_type    *pin_ptr
)
{
  cm_generic_rpt_u_type     gen_rpt;

  if(pin_ptr == NULL)
  {
    return;
  }

  memset(&gen_rpt, 0x00, sizeof(cm_generic_rpt_u_type));

  UIM_MSG_HIGH_2("mmgsdi_psm_pin_evt: pin1_status 0x%x IND pending: 0x%x",
                 pin_ptr->pin_info.status, mmgsdi_psm_ready_ind_pending);

  /* Send PSM ready indication in case MMGSDI vote negatively earlier */
  if(mmgsdi_psm_ready_ind_pending &&
     pin_ptr->pin_info.status == MMGSDI_PIN_DISABLED)
  {
    /* No need to update timer as UIM doesn't have any timer value */
    gen_rpt.psm_ready_ind.activate_timer = CM_PSM_PERIODIC_UPDATE_TIMER_INVALID;
    gen_rpt.psm_ready_ind.module = CM_PSM_MODULE_UIM;

    mmgsdi_psm_ready_ind_pending = FALSE;

    cm_client_send_psm_rpt(CM_CLIENT_PSM_READY_IND, &gen_rpt);
  }
} /* mmgsdi_psm_pin_evt */


/*===========================================================================
FUNCTION   MMGSDI_PSM_HANDLE_CHANGE_NOTIFICATION

DESCRIPTION
  This function searches for input EF in the input table and if found, it deletes the file
  associated with that EF from PSM's EFS or SFS path accordingly.

PARAMETERS
  mmgsdi_file        : Input EF
  file_table_ptr     : file table in which we search for the EF
  file_table_length  : Length of the file table
  file_name_ptr      : Holds the name of the associated file
  use_sfs            : Indicates whether the associated file is from SFS or EFS
  changed_flag_ptr   : Tells the status of any change in any of the EFs associated with the file

DEPENDENCIES
  none

RETURN VALUE
  TRUE  : Indicates that EF is found in the given table and associated file is deleted.
  FALSE : Indicates that EF is not found in the file.

SIDE EFFECTS
  none

===========================================================================*/
static boolean mmgsdi_psm_handle_change_notification
(
  mmgsdi_file_enum_type  mmgsdi_file,
  mmgsdi_file_enum_type  *file_table_ptr,
  uint32                 file_table_length,
  const char             *file_name_ptr,
  boolean                use_sfs
)
{
  uint8 index = 0;

  if (file_table_ptr == NULL || file_name_ptr == NULL)
  {
    return FALSE;
  }

  for(index = 0; index < file_table_length; index++)
  {
    if(mmgsdi_file == file_table_ptr[index])
    {
      /* Delete existing file associated with the EF*/
      (void)mmgsdi_psm_delete_existing_sfs_or_efs_file(file_name_ptr,
                                                       use_sfs);
      return TRUE;
    }
  }

  return FALSE;
}/* mmgsdi_psm_handle_change_notification */


/*===========================================================================
FUNCTION  MMGSDI_PSM_HANDLE_CHANGE_NOTIFICATION_OF_EF

DESCRIPTION
  This function handles the notification of MMGSDI cache changes

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
void mmgsdi_psm_handle_change_notification_of_ef
(
  mmgsdi_session_type_enum_type session_type,
  mmgsdi_file_enum_type         mmgsdi_file
)
{
  mmgsdi_file_enum_type * sensitive_ef_table_ptr                    = NULL;
  mmgsdi_file_enum_type * non_sensitive_ef_table_ptr                = NULL;
  uint32                  sensitive_ef_table_size                   = 0;
  uint32                  non_sensitive_ef_table_size               = 0;
  boolean               * sensitive_ef_data_changed_ptr             = NULL;
  boolean               * non_sensitive_ef_data_changed_ptr         = NULL;
  char                  * sensitive_file_ptr                        = NULL;
  char                  * non_sensitive_file_ptr                    = NULL;

  /* These tell whether any EF data is changed in cache after retention of PSM EFS and SFS files */
  static boolean usim_sensitive_ef_data_changed                     = FALSE;
  static boolean usim_non_sensitive_ef_data_changed                 = FALSE;
#ifdef FEATURE_MMGSDI_3GPP2
  static boolean csim_sensitive_ef_data_changed                     = FALSE;
  static boolean csim_non_sensitive_ef_data_changed                 = FALSE;
#endif /* FEATURE_MMGSDI_3GPP2 */

  if(mmgsdi_psm_init_in_progress)
  {
    /* Since data is written into cache as a part of PSM initialization, don't
       delete SFS/EFS PSM files*/
    return;
  }

  /*We need not handle changes to USIM ECC as it is cached during initialization only and
    We are not handling changes to common cache enums as they are never updated */
  if (session_type == MMGSDI_GW_PROV_PRI_SESSION &&
      mmgsdi_file != MMGSDI_USIM_ECC)
  {
    sensitive_ef_data_changed_ptr     = &usim_sensitive_ef_data_changed;
    sensitive_ef_table_ptr            = &mmgsdi_psm_usim_sensitive_ef_cache_enums[0];
    sensitive_ef_table_size           = sizeof(mmgsdi_psm_usim_sensitive_ef_cache_enums) / 
                                          sizeof(mmgsdi_psm_usim_sensitive_ef_cache_enums[0]);
    sensitive_file_ptr                = mmgsdi_sfs_psm_usim_sensitive_ef_data;
    non_sensitive_ef_data_changed_ptr = &usim_non_sensitive_ef_data_changed;
    non_sensitive_ef_table_ptr        = &mmgsdi_psm_usim_non_sensitive_ef_cache_enums[0];
    non_sensitive_ef_table_size       = sizeof(mmgsdi_psm_usim_non_sensitive_ef_cache_enums) / 
                                          sizeof(mmgsdi_psm_usim_non_sensitive_ef_cache_enums[0]);
    non_sensitive_file_ptr            = mmgsdi_efs_psm_usim_non_sensitive_ef_data;
  }
#ifdef FEATURE_MMGSDI_3GPP2
  else if (session_type == MMGSDI_1X_PROV_PRI_SESSION)
  {
    sensitive_ef_data_changed_ptr     = &csim_sensitive_ef_data_changed;
    sensitive_ef_table_ptr            = &mmgsdi_psm_csim_sensitive_ef_cache_enums[0];
    sensitive_ef_table_size           = sizeof(mmgsdi_psm_csim_sensitive_ef_cache_enums) / 
                                          sizeof(mmgsdi_psm_csim_sensitive_ef_cache_enums[0]);
    sensitive_file_ptr                = mmgsdi_sfs_psm_csim_sensitive_ef_data;
    non_sensitive_ef_data_changed_ptr = &csim_non_sensitive_ef_data_changed;
    non_sensitive_ef_table_ptr        = &mmgsdi_psm_csim_non_sensitive_ef_cache_enums[0];
    non_sensitive_ef_table_size       = sizeof(mmgsdi_psm_csim_non_sensitive_ef_cache_enums) / 
                                          sizeof(mmgsdi_psm_csim_non_sensitive_ef_cache_enums[0]);
    non_sensitive_file_ptr            = mmgsdi_efs_psm_csim_non_sensitive_ef_data;
  }
#endif /* FEATURE_MMGSDI_3GPP2 */
  else
  {
    /* Nothing to do */
    return ;
  }

  /* We need to know whether there is already any change in any of the EFs in non sensitive EFS file.
     If there is a change, it indicates that  the corresponding file is already deleted. So, we no longer need to
     search our file in that list. We can skip and start searching in other lists */
  if (!(*non_sensitive_ef_data_changed_ptr))
  {
    /* To check whether the input EF is from non-sensitive EF list */
    *non_sensitive_ef_data_changed_ptr = mmgsdi_psm_handle_change_notification(
                                           mmgsdi_file,
                                           non_sensitive_ef_table_ptr,
                                           non_sensitive_ef_table_size,
                                           non_sensitive_file_ptr,
                                           FALSE);

    if(*non_sensitive_ef_data_changed_ptr)
    {
      /* This indicates we already found the EF in the list. No need to handle further*/
      return ;
    }
  }

  /* We need to know whether there is already any change in any of the EFs in sensitive SFS file.
     If there is a change, it indicates that  the corresponding file is already deleted. So, we no longer need to
     search our file in that list. We can skip */
  if (!(*sensitive_ef_data_changed_ptr))
  {
    /* To check whether the input EF is from sensitive EF list */
    *sensitive_ef_data_changed_ptr = mmgsdi_psm_handle_change_notification(
                                       mmgsdi_file,
                                       sensitive_ef_table_ptr,
                                       sensitive_ef_table_size,
                                       sensitive_file_ptr,
                                       TRUE);
  }
}/* mmgsdi_psm_handle_change_notification_of_ef */

