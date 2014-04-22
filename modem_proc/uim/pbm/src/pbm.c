/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER FUNCTIONS

GENERAL DESCRIPTION
  This file contains the PBM write and init functions.

  Copyright (c) 2002 - 2016 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/pbm/src/pbm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/23/16   nr      ECC optimizations for IoE devices
05/19/16   sp      Logging improvements
02/06/16   nr      ECC only support in PBM
08/26/15   nr      pbm is not reading both NV67221 and NV0057 when NV70242 to 0
03/03/15   stv     Migrate some efs access to uim_common_efs API
08/05/15   nr      Operator specifc NV ECC numbers configuration
07/20/15   nr      Enter & leave critical section inside pbm_ecc_set_hardcoded_eccs()
01/19/15   kr      Fixed Compilation warnings
10/08/14   stv     Reduce redundant indications to AP for OTA ECC update through CM SS event
09/15/14   stv     Ignore the failure of reading /nv/item_files/pbm/pbm_nv_ecc_config
09/12/14   stv     Feature to maintain sim absent hardcoded ecc list till pin is verified
08/05/14   NR      Removal of unused feature flags
07/29/14   am      Correctly direct/redirect atomic_ops APIs across MOBs
07/25/14   am      Added stubs for RFM APIs to fix linker errors
07/07/14   NR      Use v2.0 diag macros
06/16/14   NR      AAS sim field info is not getting copied during boot up
                   for all sessions except card session.
06/06/14   stv     Checking emergency mode and service category as well while building
                   hardcoded ecc cache
05/29/14   stv     NV wrapping for 3gpp spec rel.11 changes
05/16/14   NR      Enabling or Disabling ADN based on NV configurations
05/01/14   NR      FR 19033: Reduce Code Duplication pbm
02/25/14   stv     CM SS event handling changes for SGLTE
02/10/14   stv     Potential de-referencing of NULL pointer without checking for it
01/09/14   kk      Fix compilation error for SBM MOB
01/09/14   NR      potential memory leak issues in PBM funcions
01/07/13   NR      Pointer Null check is missing for the pointer ecc_cache returned
                   from pbm_device_id_to_cache () in function pbm_ecc_clear_all_id ()
12/19/13   NR      PBM: LLVM compiler warnings
09/17/10   krishna Clear NW ECC no's when the corresponding emergency list len is 0
05/10/10   krishna provided the clients Session based field info
03/18/10   krishna Used pbmtask_i.h instead if pbmtask.h as part of CMI activity
03/18/10   krishna Resolved the compilation errors in WM7
07/10/09  ashwanik Support for CSIM and Dual Sim Features
05/04/09   krishna changes for CR 167785
12/29/08   pvphani Adding Null pointer check in pbm_list_recover function. Fix for CR: 164527
03/04/08   clm     Add logic to revert records containing USIM only fields upon
                   failure to add, delete, or update.
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
01/16/07   cvs     Add event for phonebooks becoming ready
12/06/06   cvs     Fix double read of files after full card refresh
12/06/06   cvs     Remove banned APIs
10/10/06   cvs     Move messages to PBM's diag msg ID
10/03/06   cvs     Add support for MBN
07/12/06   cvs     Fix compile warnings
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
04/10/06   cvs     add debug feature to ignore WMS init
03/24/06   cvs     %x -> 0x%x in log messages
02/28/06   cvs     Add support for MBDN
02/23/06   cvs     rework memory allocation macros
01/19/06   cvs     Emergency number cleanup, fix bug in network emerg nums
01/09/06   cvs     Lint fixes
01/04/06   cvs     Fix compile error without GSDI
12/14/05   cvs     Better log messages
12/06/05   cvs     Wait to init sim phonebooks until after WMS
11/15/05   cvs     Lint fixes
11/10/05   cvs     Add network ECC numbers
09/29/05   cvs     Fix usage of PBM_TEXT_SIZE and PBM_MAX_TEXT_LENGTH
08/22/05   AT      Moved contents inside of a FEATURE_PBM_TASK.
08/12/05   AT      Added UCS2 support for strings.
08/04/05   cvs     Check for committed list growth
08/01/05   cvs     Code review comments
07/29/05   AT      Added additional range checking.
07/06/05   cvs     Add 999 to emergency list if CPHS is enabled.
06/14/05   cvs     Fix GCF failures related to emergency numbers
05/26/05   cvs     support for API changes to make APIs more multiprocessor friendly
05/23/05   cvs     Fix off-by-one error in loading ECC list
04/29/05   cvs     Fix Error Handling for USIM failures
03/31/05   AT      Created feature FEATURE_CPHS_UK to hardcode the
                   emergency number 999 in all cases.  This feature
                   causes a GCF test case to fail, but meets the CPHS
                   requirement.  Note these requirements are in conflict.
                   Also added featurization for non_MMGSDI builds.
03/28/05   AT      Updated ECC requirements.
03/08/05   PA      Support for PBM notifications
01/20/05   PA      Added support for read lock, non-blocking write lock and
                   support for MSISDN.
01/24/05   PA      Changed NV_EMERGENCY_NUMBER_START to
                   PBM_NV_EMERGENCY_NUMBER_START
01/20/05   PA      Add only emergency numbers to ecc_cache.
01/12/05   PA      PBM USIM Support
12/09/04   PA      Set cache status to ready after reading NV items
12/03/04   PA      Fix category used in mirroring
12/02/04   PA      Add reference when new record in added to committed list.
11/19/04   PA      Support non-GSDI builds
11/18/04   PA      Support for write lock
09/29/04   PA      Initial Revision
===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "pbm.h"
#include "uim_msg.h"
#include "pbm_common_types.h"
#include "pbmefs.h"
#include "pbm_conversion.h"
#include "pbmtask_i.h"
#include "pbmlib.h"
#include "pbmutils.h"
#include "pbmuim.h"
#include "pbmgsdi.h"
#include "pbm_nv_refresh.h"
#include "cm.h"
#ifdef FEATURE_DUAL_SIM
#include "cm_dualsim.h"
#endif /* FEATURE_DUAL_SIM */
#include "atomic_ops.h"
#include "stringl/stringl.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================
                              MACROS
===========================================================================*/

/*--------------------------------------------------------
  To store list of status of pbm features
----------------------------------------------------------*/
pbm_features_status_list_type pbm_features_status_list;

/*===========================================================================
FUNCTION PBM_NV_FEATURES_STATUS_LIST_UPDATE_VERSION

DESCRIPTION
  Function used to validate the PBM Features Status NVITEM's version.
  If what's in the NV is less than the one currently defined in the build,
  updates it to the current version defined and writes it back to the NV.
  It also writes back to the NV the default values of the delta items in
  in the list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Version gets updated in the NV if it doesn't match the one in the build. If
  the version is updated, next time when the user reads the NVITEM, a
  dropdown of items corresponding to the updated version is shown instead of
  what the user saw the first time the NV read was done (corresponding to
  the version present in the NV at that point).
===========================================================================*/
static void pbm_nv_features_status_list_update_version(void);

/*===========================================================================
FUNCTION PBM_NV_INIT_FEATURES_STATUS_NV_LOOKUP

DESCRIPTION
  Function used to get the status of PBM features as defined by the NVITEM
  "PBM_FEATURES_STATUS_LIST" (71535).
  The global, "features_status_list", used to hold the NVITEM, is expandable,
  yet the size is fixed. See the struct definition for more detailed doc on
  that. The global struct has a "version" item that is updated everytime we
  add new items to this global struct (starting with version 0 in the first
  implementation)
  The pbm code has full control over version of NVITEM. Taking an example,
  if in future, in version 2 of this struct(that has 10 items), the delta is 3
  items (from version 0 that had 7 items), and this build is loaded onto the
  device that already had the items set using version 0 of this NVITEM, then,
  on boot-up after loading the build, pbm will validate the version it read
  from NV. If it is not 2, it will update it to 2 and write it back to NV. At
  this point, however, the value of features/items 8 through 10 will be set to
  their default values by pbm code.

DEPENDENCIES
  Minimum QXDM version required to be able to set this NVITEM (71535) is
  QXDM 3.14.757

RETURN VALUE
  None

SIDE EFFECTS
  "version" item in this NVITEM is writable through QXDM (no QXDM support to
  hide it or grey it out) or QCN. However if what is written explicitly doesn't
  match what's in the build, pbm will change it to what's in the build and
  it could be annoying for the QXDM user to see it change to something else
  (when the user reads the NVITEM next time).
===========================================================================*/
void pbm_nv_init_features_status_nv_lookup(void);

/*===========================================================================
FUNCTION PBM_NV_WRITE_DEFAULT_VALUE_OF_DELTA_FEATURES

DESCRIPTION
  Function used to write the default values of PBM Features (controlled by
  PBM_Feature_Status_list NVITEM) that are delta between the two
  versions of Feature_Status_List to global features_status_list.

DEPENDENCIES

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void pbm_nv_write_default_value_of_delta_features(
  uint8 version_in_nv,
  uint8 version_in_build
);

/*===========================================================================
FUNCTION PBM_NV_WRITE_DEFAULT_VALUE_OF_ALL_FEATURES

DESCRIPTION
  Function used to write the default values of all the PBM Features
  (controlled by PBM_Feature_Status_list NVITEM), defined currently, to
  global pbm_features_status_list.

DEPENDENCIES

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void pbm_nv_write_default_value_of_all_features(void);

/*===========================================================================
FUNCTION PBM_NV_FEATURE_WRITE_DEFAULT_VALUE

DESCRIPTION
  Function used to write the default value of MMGSDI Feature (controlled by
  PBM_Feature_Status_list NVITEM) to global pbm_features_status_list.

DEPENDENCIES

RETURN VALUE
  pbm_return_type

SIDE EFFECTS
  None
===========================================================================*/
static pbm_return_type pbm_nv_feature_write_default_value(
  pbm_nv_features_enum_type nv_feature);

/*-------------------------------------------------------------------
  List of total number of PBM features present in NVITEM in each version
  of feature_status_list. Whenever a newer version of the NVITEM is exposed,
  size of the array grows by 1...So, if 8 features/items were added in VERSION0
  and 2 in VERSION1, the array should have 2 items listing the number of total
  features at each version.. {8,10} in this case...
---------------------------------------------------------------------*/
uint8  pbm_features_total_till_version[] =
  {PBM_FEATURES_STATUS_LIST_VERSION0_COUNT,
   PBM_FEATURES_STATUS_LIST_VERSION1_COUNT};


/*===========================================================================
FUNCTION PBM_NV_GET_FEATURE_STATUS

DESCRIPTION
  Function used to get the status of PBM features listed in enum
  pbm_nv_features_enum_type as defined by the NVITEM 71535 -
  "PBM_FEATURES_STATUS_LIST".

DEPENDENCIES
  Minimum QXDM version required to be able to read/write this NVITEM is
  latest QXDM(or any version later than 3.14.757)

RETURN VALUE
  pbm_feature_status_enum

SIDE EFFECTS
  None
===========================================================================*/
pbm_feature_status_enum_type pbm_nv_get_feature_status(
  pbm_nv_features_enum_type  nv_feature)
{
  pbm_feature_status_enum_type ret_value = PBM_FEATURE_DISABLED;

  switch(nv_feature)
  {
    case PBM_FEATURE_FDN_SPECIAL_HANDLING:
      ret_value = pbm_features_status_list.fdn_special_handling;
      break;
    case PBM_FEATURE_ECC_HARDCODE_SIM_ABSENT_LIST_TILL_PIN_LOCK :
      ret_value = pbm_features_status_list.pbm_ecc_hardcode_sim_absent_list_till_pin_lock;
      break;
    default:
      return ret_value;
  }

  UIM_MSG_HIGH_2("Status of feature 0x%x is 0x%x", nv_feature, ret_value);
  return ret_value;
} /* pbm_nv_get_feature_status */


/*===========================================================================
FUNCTION PBM_NV_FEATURE_WRITE_DEFAULT_VALUE

DESCRIPTION
  Function used to write the default value of MMGSDI Feature (controlled by
  PBM_Feature_Status_list NVITEM) to global pbm_features_status_list.

DEPENDENCIES

RETURN VALUE
  pbm_return_type

SIDE EFFECTS
  None
===========================================================================*/
static pbm_return_type pbm_nv_feature_write_default_value(
  pbm_nv_features_enum_type nv_feature)
{
  switch(nv_feature)
  {
    case PBM_FEATURE_FDN_SPECIAL_HANDLING:
      pbm_features_status_list.fdn_special_handling = PBM_FEATURE_DISABLED;
      break;
    case PBM_FEATURE_ADN_CACHING:
    {
      uint8 slot_id;
      for(slot_id = 0;slot_id < PBM_MAX_SLOTS;slot_id++)
      {
        pbm_features_status_list.pbm_adn_phonebook_caching_status[slot_id].
                                   gpb_caching_status    = PBM_FEATURE_ENABLED;
        pbm_features_status_list.pbm_adn_phonebook_caching_status[slot_id].
                                   lpb_1x_caching_status = PBM_FEATURE_ENABLED;
        pbm_features_status_list.pbm_adn_phonebook_caching_status[slot_id].
                                   lpb_gw_caching_status = PBM_FEATURE_ENABLED;
      }
    }
      break;
    case PBM_FEATURE_ECC_HARDCODE_SIM_ABSENT_LIST_TILL_PIN_LOCK :
      pbm_features_status_list.pbm_ecc_hardcode_sim_absent_list_till_pin_lock = PBM_FEATURE_DISABLED;
    break;

    default:
      return PBM_ERROR;
  }
  return PBM_SUCCESS;
} /* pbm_nv_feature_write_default_value */


/*===========================================================================
FUNCTION PBM_NV_WRITE_DEFAULT_VALUE_OF_ALL_FEATURES

DESCRIPTION
  Function used to write the default values of all the PBM Features
  (controlled by PBM_Feature_Status_list NVITEM), defined currently, to
  global pbm_features_status_list.

DEPENDENCIES

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void pbm_nv_write_default_value_of_all_features(void)
{
  uint8 pbm_feature;
  uint8 num_features_in_build;

  num_features_in_build = pbm_features_total_till_version[PBM_FEATURES_STATUS_LIST_CURRENT_VERSION];

  memset(&pbm_features_status_list, 0x00, sizeof(pbm_features_status_list));

  /* Update the Version in features_status_list to the current build version */
  pbm_features_status_list.version = PBM_FEATURES_STATUS_LIST_CURRENT_VERSION;

  /* Update features_status_list with default values */
  for(pbm_feature = 0; pbm_feature < num_features_in_build; pbm_feature++)
  {
    (void)pbm_nv_feature_write_default_value((pbm_nv_features_enum_type )pbm_feature);
  }
} /* pbm_nv_write_default_value_of_all_features */


/*===========================================================================
FUNCTION PBM_NV_WRITE_DEFAULT_VALUE_OF_DELTA_FEATURES

DESCRIPTION
  Function used to write the default values of PBM Features (controlled by
  PBM_Feature_Status_list NVITEM) that are delta between the two
  versions of Feature_Status_List to global features_status_list.

DEPENDENCIES

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
static void pbm_nv_write_default_value_of_delta_features(
  uint8 version_in_nv,
  uint8 version_in_build
)
{
  uint8 pbm_feature;
  uint8 num_features_in_nv;
  uint8 num_features_in_build;

  if((version_in_nv >= sizeof(pbm_features_total_till_version)/sizeof(uint8)) ||
     (version_in_build >= sizeof(pbm_features_total_till_version)/sizeof(uint8)))
  {
    UIM_MSG_ERR_0("Array out-of-bounds while writing default values for PBM features");
    return;
  }

  num_features_in_nv    = pbm_features_total_till_version[version_in_nv];
  num_features_in_build = pbm_features_total_till_version[version_in_build];

  /* Since pbm_features_total_till_version array is indexed by the
     feature_status_list *version*, it's value at version version_in_nv is
     essentially the first feature of the delta list...no need to bail out in
     case writing fails... */
  for(pbm_feature = num_features_in_nv; pbm_feature < num_features_in_build; pbm_feature++)
  {
    (void)pbm_nv_feature_write_default_value((pbm_nv_features_enum_type )pbm_feature);
  }
} /* pbm_nv_write_default_value_of_all_features */


/*===========================================================================
FUNCTION PBM_NV_FEATURES_STATUS_LIST_UPDATE_VERSION

DESCRIPTION
  Function used to validate the PBM Features Status NVITEM's version.
  If what's in the NV is less than the one currently defined in the build,
  updates it to the current version defined and writes it back to the NV.
  It also writes back to the NV the default values of the delta items in
  in the list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Version gets updated in the NV if it doesn't match the one in the build. If
  the version is updated, next time when the user reads the NVITEM, a
  dropdown of items corresponding to the updated version is shown instead of
  what the user saw the first time the NV read was done (corresponding to
  the version present in the NV at that point).
===========================================================================*/
static void pbm_nv_features_status_list_update_version(void)
{
  uim_common_efs_status_enum_type   efs_status  = UIM_COMMON_EFS_ERROR;

  UIM_MSG_HIGH_2("PBM Features Status NV ITEM version - in NV: 0x%x, in build: 0x%x",
                 pbm_features_status_list.version,
                 PBM_FEATURES_STATUS_LIST_CURRENT_VERSION);

  if (pbm_features_status_list.version < PBM_FEATURES_STATUS_LIST_CURRENT_VERSION)
  {
    /* If the features_status_list version in NV is less than current
       pbm_features_status_list version in our code, meaning new features have been
       added since the version in NV till the current version, we write back
       the default value of all those delta features to the NV. */
    pbm_nv_write_default_value_of_delta_features(
                                                 pbm_features_status_list.version,
                                                 PBM_FEATURES_STATUS_LIST_CURRENT_VERSION
                                                );

    pbm_features_status_list.version = PBM_FEATURES_STATUS_LIST_CURRENT_VERSION;

    efs_status = uim_common_efs_write(UIM_COMMON_EFS_PBM_FEATURES_LIST,
                                      UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                      UIM_COMMON_EFS_DEVICE,
                                     (const char *)&pbm_features_status_list,
                                     (uint32)sizeof(pbm_features_status_list));

    if (efs_status != UIM_COMMON_EFS_SUCCESS)
    {
      UIM_MSG_ERR_1("PBM Features Status NV ITEM version update failed!, error 0x%x",
                    efs_status);
    }
  }
} /* pbm_nv_features_status_list_update_version */


/*===========================================================================
FUNCTION PBM_NV_INIT_FEATURES_STATUS_NV_LOOKUP

DESCRIPTION
  Function used to get the status of PBM features as defined by the NVITEM
  "PBM_FEATURES_STATUS_LIST" (71535).
  The global, "features_status_list", used to hold the NVITEM, is expandable,
  yet the size is fixed. See the struct definition for more detailed doc on
  that. The global struct has a "version" item that is updated everytime we
  add new items to this global struct (starting with version 0 in the first
  implementation)
  The pbm code has full control over version of NVITEM. Taking an example,
  if in future, in version 2 of this struct(that has 10 items), the delta is 3
  items (from version 0 that had 7 items), and this build is loaded onto the
  device that already had the items set using version 0 of this NVITEM, then,
  on boot-up after loading the build, pbm will validate the version it read
  from NV. If it is not 2, it will update it to 2 and write it back to NV. At
  this point, however, the value of features/items 8 through 10 will be set to
  their default values by pbm code.

DEPENDENCIES
  Minimum QXDM version required to be able to set this NVITEM (71535) is
  QXDM 3.12.997.

RETURN VALUE
  None

SIDE EFFECTS
  "version" item in this NVITEM is writable through QXDM (no QXDM support to
  hide it or grey it out) or QCN. However if what is written explicitly doesn't
  match what's in the build, pbm will change it to what's in the build and
  it could be annoying for the QXDM user to see it change to something else
  (when the user reads the NVITEM next time).
===========================================================================*/
void pbm_nv_init_features_status_nv_lookup(void)
{
  uim_common_efs_status_enum_type efs_status = UIM_COMMON_EFS_ERROR;

  /* Start off with the global struct initialised to 0 */
  memset(&pbm_features_status_list, 0x00, sizeof(pbm_features_status_list));

  efs_status = uim_common_efs_read(UIM_COMMON_EFS_PBM_FEATURES_LIST,
                                   UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                   UIM_COMMON_EFS_DEVICE,
                                   (uint8 *)&pbm_features_status_list,
                                   (uint32)sizeof(pbm_features_status_list));

  UIM_MSG_HIGH_1("pbm_nv_init_features_status_nv_lookup Features NV read status 0x%x", efs_status);

  if (efs_status == UIM_COMMON_EFS_SUCCESS)
  {
    pbm_nv_features_status_list_update_version();
  }
  else
  {
    /* Updating global pbm_features_status_list with default value of all the
       defined features */
    pbm_nv_write_default_value_of_all_features();

    /* Write features' status to the EFS, erasing previous contents,
    create if not present */
    efs_status = uim_common_efs_write(UIM_COMMON_EFS_PBM_FEATURES_LIST,
                                      UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                      UIM_COMMON_EFS_DEVICE,
                                      (const char *)&pbm_features_status_list,
                                      (uint32)sizeof(pbm_features_status_list));

    if(efs_status != UIM_COMMON_EFS_SUCCESS)
    {
       UIM_MSG_LOW_1("Write to pbm_features_list default failed 0x%x", efs_status);
    }
  }
} /* pbm_nv_init_features_status_nv_lookup */

/*===========================================================================
FUNCTION PBM_GET_NV

DESCRIPTION
  Read an item from the NV memory.

  Note that NV read is done in a synchronous fashion; that is, this
  function waits till the read is done.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
nv_stat_enum_type pbm_get_nv(nv_items_enum_type nv_item, nv_item_type *data_ptr)
{
   static nv_cmd_type pbm_nv_cmd;

   pbm_nv_cmd.item       = nv_item;
   pbm_nv_cmd.cmd        = NV_READ_F;
   pbm_nv_cmd.data_ptr   = data_ptr;
   pbm_nv_cmd.tcb_ptr    = rex_self();
   pbm_nv_cmd.sigs       = PBM_NV_SIG;
   pbm_nv_cmd.done_q_ptr = NULL;

   pbm_kick_dog();

   (void) rex_clr_sigs(rex_self(), PBM_NV_SIG);
   nv_cmd(&pbm_nv_cmd);
   (void) pbm_wait(PBM_NV_SIG);
   (void) rex_clr_sigs(rex_self(), PBM_NV_SIG);

   if (pbm_nv_cmd.status != NV_DONE_S)
   {
      UIM_MSG_HIGH_2("Unable to read nv item %d, error %d", nv_item,
                   pbm_nv_cmd.status);
   }

   return pbm_nv_cmd.status;
}


/*===========================================================================
FUNCTION PBM_PUT_NV

DESCRIPTION
  Write an item to NV memory.

  Note that NV write is done in a synchronous fashion; that is, this
  function waits till the write is done.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
nv_stat_enum_type pbm_put_nv(nv_items_enum_type nv_item, nv_item_type *data_ptr)
{
   static nv_cmd_type pbm_nv_cmd;

   pbm_nv_cmd.item       = nv_item;
   pbm_nv_cmd.cmd        = NV_WRITE_F;
   pbm_nv_cmd.data_ptr   = data_ptr;
   pbm_nv_cmd.tcb_ptr    = rex_self();
   pbm_nv_cmd.sigs       = PBM_NV_SIG;
   pbm_nv_cmd.done_q_ptr = NULL;

   pbm_kick_dog();

   (void) rex_clr_sigs(rex_self(), PBM_NV_SIG);
   nv_cmd(&pbm_nv_cmd);
   (void) pbm_wait(PBM_NV_SIG);
   (void) rex_clr_sigs(rex_self(), PBM_NV_SIG);

   if (pbm_nv_cmd.status != NV_DONE_S)
   {
      UIM_MSG_ERR_2("Unable to write nv item %d, error %d", nv_item,
                  pbm_nv_cmd.status);
   }

   return pbm_nv_cmd.status;
}

