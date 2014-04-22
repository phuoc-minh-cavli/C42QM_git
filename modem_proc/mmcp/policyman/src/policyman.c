/**
  @file policyman.c

  @brief Public interface to Policy Manager
*/

/*
    Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/src/policyman.c#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/

#include "comdef.h"
#include "mre_efs.h"
#include "policyman_dbg.h"
#include "policyman_device_config.h"
#include "policyman_i.h"
#include "policyman_policy.h"
#include "policyman_cfgitem.h"
#include "policyman_rf.h"
#include "policyman_timer.h"
#include "policyman_phone_events.h"
#include "policyman_serving_system.h"
#include "policyman_efs.h"
#include "policyman_task.h"

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*------------------------------------------------------------------------
  FUNCTION Prototypes
--------------------------------------------------------------------------*/
void policyman_register_for_cm_events(void);

#endif
/*  Boolean used by policyman_is_test_mode() to know if it has determined
 *  whether policyman is in test mode.  This is a static outside the function
 *  because it must be reset each time policyman starts in unit tests.
 */
static boolean  test_mode_read;
static boolean  in_test_mode;
static boolean  in_FTM;


/*-------- ftm_mode_enabled --------*/
static boolean ftm_mode_enabled(
  void
)
{
  nv_stat_enum_type result;
  nv_item_type      data;

  result = mre_efs_get_nv_item(NV_FTM_MODE_I, &data, 0);

  return  ((result == NV_DONE_S) && (data.ftm_mode != 0));
}


/*-------- policyman_is_test_mode --------*/
/**
@brief  Is policyman supposed to run in test-only mode?
*/
boolean policyman_is_test_mode(
  void
)
{
  if (!test_mode_read)
  {
    test_mode_read = TRUE;
    in_test_mode   = mre_efs_file_exists( POLICYMAN_TEST_MODE_EFS_PATH,
                                          SYS_MODEM_AS_ID_NONE );

    /* Read FTM NV here. 
         This step will happen at power up so we are fine reading FTM NV
     */
    in_FTM         = ftm_mode_enabled();
  }

  return in_test_mode;
}

/*-------- policyman_is_FTM --------*/
/**
@brief  Is policyman in FTM
*/
boolean
policyman_is_FTM(
  void
  )
{
  return in_FTM;
}


/*-------- policyman_set_test_mode --------*/
/**
@brief  Set test mode to given value
*/
void
policyman_set_test_mode(
  boolean value
  )
{
  in_test_mode = value;
  test_mode_read = value;
}

/*-------- policyman_set_FTM --------*/
/**
@brief  Set static in_FTM to given value
*/
void
policyman_set_FTM(
  boolean value
  )
{
  in_FTM = value;
}


/*-------- policyman_init --------*/
/**
@brief  Initialize Policy Manager.  Private function to be used by PM task code.

@return
  None
*/
void
policyman_init(
  void
)
{
  policyman_state_init();
  #if defined(FEATURE_DISABLE_DYNAMIC_POLICIES)
  policyman_state_rf_init();
  #endif
  policyman_cfgitem_init();

  if (!policyman_is_test_mode())
  {
    policyman_policy_init(policyman_state_get_state());
  }

  policyman_cfgitem_ensure_device_config();
  policyman_ensure_configurations();
  policyman_cfgitem_ensure_items();
  policyman_efs_initialize();
  policyman_send_all_config_items();
}

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- policyman_per_subs_init --------*/
/**
@brief  Initialize subscription specific state information, and overall policy and configuration
           for Policy Manager.  Private function to be used by PM task code.

@return
  None
*/
void
policyman_per_subs_init(
  size_t subs
  )
{
  policyman_state_cmph_nv_init(subs);
  
  // re-read policy for subs
  if (!policyman_is_test_mode())
  {
    policyman_policy_per_subs_init(policyman_state_get_state(), subs);
  }
}
#endif
#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- policyman_per_subs_deinit --------*/
/**
@brief  Deinitialize specific subscription state, as well as overall configuration for
           Policy Manager.  Private function to be used by PM task code.

@return
  None
*/
void policyman_per_subs_deinit(
  size_t subs
)
{
  if (!policyman_is_test_mode())
  {
    policyman_device_config_reset_subs_feature((sys_modem_as_id_e_type)subs);
    policyman_policy_per_subs_deinit(subs);
  }
}
#endif

/*-------- policyman_get_items --------*/
policyman_status_t
policyman_get_items(
  policyman_item_id_t const *pIds,
  size_t                    nIds,
  policyman_item_t const    **ppItem
  )
{
  size_t  numItems;

  if (policyman_is_initializing())
  {
    return POLICYMAN_STATUS_ERR_NOT_READY;
  }

  numItems = policyman_cfgitem_get_items(pIds, nIds, ppItem);

  if(policyman_policy_is_suspended())
  {
    return POLICYMAN_STATUS_WRN_SUSPENDED;
  }
  else
  {
    return (numItems == nIds)? POLICYMAN_STATUS_SUCCESS : POLICYMAN_STATUS_ERR_INVALID_ITEM_ID;
  }
}


/*-------- policyman_get_items_msim --------*/
policyman_status_t
policyman_get_items_msim(
  policyman_item_id_t const         *pIds,
  size_t                            nIds,
  policyman_item_collection_t const **ppItems
  )
{
  policyman_status_t  status;

  if (policyman_is_initializing())
  {
    return POLICYMAN_STATUS_ERR_NOT_READY;
  }

  POLICYMAN_MSG_HIGH("policyman_get_items_msim requested for %d items ", nIds, 0, 0);
  status = policyman_cfgitem_get_items_msim(pIds, nIds, (policyman_item_collection_t **) ppItems);

  return policyman_policy_is_suspended()? POLICYMAN_STATUS_WRN_SUSPENDED : status;
}


/*-------- policyman_get_items_block --------*/
policyman_status_t
policyman_get_items_block(
  policyman_item_id_t const *pIds,
  size_t                    nIds,
  policyman_item_t const    **ppItem
  )
{
  if (policyman_is_initializing())
  {
    POLICYMAN_MSG_HIGH("Client blocked in policyman_get_items_block", 0, 0, 0);
    policyman_blocked_clients_add();
  }

  return policyman_get_items(pIds, nIds, ppItem);
}


/*-------- policyman_item_get_id --------*/
policyman_item_id_t
policyman_item_get_id(
  policyman_item_t const  *pItem
  )
{
  return (pItem == NULL)?   POLICYMAN_ITEM_INVALID
                          : policyman_cfgitem_id(pItem);
}

/*-------- policyman_get_items_block_msim --------*/
policyman_status_t
policyman_get_items_block_msim(
  policyman_item_id_t const         *pIds,
  size_t                            nIds,
  policyman_item_collection_t const **ppItems
  )
{
  if (policyman_is_initializing())
  {
    POLICYMAN_MSG_HIGH("Client blocked in policyman_get_items_block_msim", 0, 0, 0);
    policyman_blocked_clients_add();
  }

  return policyman_get_items_msim(pIds, nIds, ppItems);
}

/*-------- policyman_item_get_id_msim --------*/
policyman_item_id_t
policyman_item_get_id_msim(
  policyman_item_t const  *pItem,
  sys_modem_as_id_e_type  *pSubsId
  )
{
  return (pItem == NULL)?   POLICYMAN_ITEM_INVALID
                          : policyman_cfgitem_id_msim(pItem, pSubsId);
}


#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */

/*-------- policyman_item_addref --------*/
void
policyman_item_addref(
  policyman_item_t const  *pItem
  )
{
  if (pItem != NULL)
  {
    policyman_cfgitem_addref(pItem);
  }
}


/*-------- policyman_item_collection_release --------*/
void
policyman_item_collection_release(
  policyman_item_collection_t const *pCollection
  )
{
  ref_cnt_obj_release((void *) pCollection);
}


/*-------- policyman_item_release --------*/
void
policyman_item_release(
  policyman_item_t const  *pItem
  )
{
  if (pItem != NULL)
  {
    policyman_cfgitem_release(pItem);
  }
}


/*-------- policyman_msgr_attachments_release --------*/
void
policyman_msgr_attachments_release(
  msgr_hdr_s  *pMsg
  )
{
  if (pMsg != NULL)
  {
    POLICYMAN_MSG_HIGH("policyman_msgr_attachments_release called", 0, 0, 0);
    policyman_cfgitem_release_msg_attach(pMsg);
  }
}

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- policyman_get_boolean_value --------*/

boolean policyman_get_boolean_value(
  char const  *pBooleanName,
  boolean     *pValue
)
{
  mre_namespace_info_t  *pInfo;
  mre_status_t           status;
  boolean                namedBoolean = FALSE;
  
  pInfo = policyman_get_namespace_info();
  
  status =  mre_boolean_get_value_from_set(
              pBooleanName,
              &namedBoolean,
              pInfo->perSubsNamedObjs[0]
              );

  if (status == MRE_STATUS_SUCCESS)
  {
    *pValue = namedBoolean;  
    POLICYMAN_MSG_HIGH_1("policyman_get_boolean_value returns true. pValue=%d", *pValue);
    return TRUE;
  }
  else
  {
    POLICYMAN_MSG_HIGH_0("policyman_get_boolean_value returns false");
    return FALSE;
  }
}
#endif