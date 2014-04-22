/**
  @file policyman_cfgitem.c

  @brief Functions to manage PM configuration items.
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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/src/policyman_cfgitem.c#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/

#include "mre_efs.h"

#include "policyman_cfgitem.h"
#include "policyman_cm.h"
#include "policyman_dbg.h"
#include "policyman_device_config.h"
#include "policyman_diag.h"
#include "policyman_efs.h"
#include "policyman_msg.h"
#include "policyman_rat_capability.h"
#include "policyman_rf.h"
#include "policyman_set.h"
#include "policyman_svc_mode.h"
#include "policyman_task.h"
#include "policyman_uim.h"
#include "policyman_util.h"

#include "sys.h"
#include "log.h"
#include "stringl/stringl.h"

extern policyman_crit_sect_t   policyman_item_crit_sect;

typedef struct 
{
  policyman_item_id_t    id;
  sys_modem_as_id_e_type asubs_id;
} policyman_item_cmp_t;

/* Database of policyman config info
*/
STATIC policyman_config_info_t  policyman_cfg_info[] =
{
  {
    POLICYMAN_ITEM_DEVICE_CONFIGURATION,
    "Modem configuration",
    policyman_device_config_get_default,
    policyman_device_config_display,
    DYNAMIC_COMPONENTS(
      policyman_device_config_compare,
    NULL,
      policyman_device_config_get_default_from_efs,
      NULL
      )
  },
  {
    POLICYMAN_ITEM_RAT_CAPABILITY,
    "RAT capability",
    policyman_rat_capability_get_default,
    policyman_rat_capability_display,
    DYNAMIC_COMPONENTS(
      policyman_rat_capability_compare,
      policyman_rat_capability_on_update,
      policyman_rat_capability_get_default_from_efs,
      policyman_rat_capability_fill_subs_diag_log
      )
  },
  {
    POLICYMAN_ITEM_RF_BANDS,
    "RF bands",
    policyman_rf_bands_get_default,
    policyman_rf_bands_display,
    DYNAMIC_COMPONENTS(
      policyman_rf_bands_compare,
    NULL,
      policyman_rf_bands_get_default_from_efs,
      policyman_rf_bands_fill_subs_diag_log
      )
  },
  {
    POLICYMAN_ITEM_SVC_MODE,
    "SVC mode",
    policyman_svc_mode_get_default,
    policyman_svc_mode_display,
    DYNAMIC_COMPONENTS(
      policyman_svc_mode_compare,
    NULL,
      policyman_svc_mode_get_default_from_efs,
      policyman_svc_mode_fill_subs_diag_log
      )
  },
};

typedef struct
{
  policyman_config_info_t *pCfgInfo;
  sys_modem_as_id_e_type   subsId;
  boolean                  is_changed;
  
  policyman_item_t        *pItem;  
} policyman_config_item_t;

#define NUM_PM_SUBS_ITEMS (ARR_SIZE(policyman_cfg_info) - 1)
#define NUM_PM_CFG_ITEMS  (1 + (NUM_PM_SUBS_ITEMS * POLICYMAN_NUM_SUBS))

/* Database of actual policyman items
    Array size is the number of per-subs items plus one for the device config
*/
STATIC policyman_config_item_t policyman_cfgitems[NUM_PM_CFG_ITEMS];

/*  Sequence number of the database.
 */
STATIC size_t g_seqno;

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
static void
policyman_cfgitem_notify_clients(
  policyman_set_t *pItemSet
  );
#endif
/*-------- sequence_number_init --------*/
STATIC void sequence_number_init(
  void
)
{
  g_seqno = 0;
}


/*-------- sequence_number_increment --------*/
STATIC void sequence_number_increment(
  void
)
{
  ++g_seqno;
}


/*-------- sequence_number_get --------*/
STATIC size_t sequence_number_get(
  void
)
{
  return g_seqno;
}


/*-------- policyman_config_item_is_device_level --------*/
STATIC boolean policyman_config_item_is_device_level(
  policyman_config_item_t *pCfgItem
)
{
  return POLICYMAN_ITEM_DEVICE_CONFIGURATION == pCfgItem->pCfgInfo->id;
}


/*-------- policyman_subs_id_is_configured --------*/
boolean policyman_subs_id_is_configured(
  sys_modem_as_id_e_type subs_id 
)
{
  
  size_t  nSim = 0;

  (void) policyman_get_current_num_sim(&nSim) ;

  return ((size_t) subs_id) < nSim;
}


/*-------- policyman_cfgitems_iterate --------*/
STATIC void policyman_cfgitems_iterate(
  elem_iter_fn_t   pfnIter,
  void            *pData1,
  void            *pData2
)
{
  size_t  i;
  boolean keepGoing = TRUE;

  policyman_enter_crit_sect(&policyman_item_crit_sect);

  for (i = 0 ; keepGoing && i < ARR_SIZE(policyman_cfgitems) ; ++i)
  {
    policyman_config_item_t *pCfgItem = &policyman_cfgitems[i];
    
    if (    policyman_config_item_is_device_level(pCfgItem)
        ||  policyman_subs_id_is_configured(pCfgItem->subsId)
       )
    {
      keepGoing = pfnIter(pCfgItem, pData1, pData2);
    }
  }

  policyman_leave_crit_sect(&policyman_item_crit_sect);
}


/*-------- policyman_cfgitem_init_item --------*/
void policyman_cfgitem_init_item(
  size_t    subsIndex,
  size_t    cfgIndex,
  size_t    itemIndex
)
{
  policyman_config_item_t   *pCfgItem;

  pCfgItem             = &policyman_cfgitems[itemIndex];
  pCfgItem->pCfgInfo   = &policyman_cfg_info[cfgIndex];
  pCfgItem->subsId     = subsIndex;
  pCfgItem->pItem      = NULL;
  pCfgItem->is_changed = 0;
}


/*-------- cfgitems_init --------*/
STATIC void cfgitems_init(
  void
)
{
  // Initialize the special item 0 (device config)
  policyman_cfgitem_init_item(0, 0, 0);

  // Initialize remaining per-subs items
  {
    size_t                   subsIndex; 
    size_t                   cfgIndex; 
    size_t                   itemIndex;

    itemIndex = 1;

    for (subsIndex = 0; subsIndex < POLICYMAN_NUM_SUBS; ++subsIndex)
    {
      // Skip device config item 0 for per-subs items
      for (cfgIndex = 1; cfgIndex < ARR_SIZE(policyman_cfg_info); ++cfgIndex)
      {
        policyman_cfgitem_init_item(subsIndex, cfgIndex, itemIndex);
        ++itemIndex;
      }
    }
  }
}

 #ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- cfgitems_get_persisted_values --------*/
STATIC void cfgitems_get_persisted_values(
  void
)
{
  size_t  itemIndex;

  for (itemIndex = 0 ; itemIndex < ARR_SIZE(policyman_cfgitems) ; ++itemIndex)
  {
    policyman_config_item_t *pCfgItem;
    sys_modem_as_id_e_type   subsId;

    pCfgItem = &policyman_cfgitems[itemIndex];
    subsId   = pCfgItem->subsId;

    if (  !policyman_is_test_mode()
        && pCfgItem->pCfgInfo->pfnGetEFS != NULL )
    {
      pCfgItem->pItem = pCfgItem->pCfgInfo->pfnGetEFS(subsId);
    }

    POLICYMAN_MSG_HIGH_4( "INIT_ITEM: item at index %d, id %d, subs %d, pItem 0x%08x",
                          itemIndex,
                          pCfgItem->pCfgInfo->id,
                          subsId,
                          pCfgItem->pItem );
  }
}

#endif
/*-------- policyman_cfgitem_init --------*/
void policyman_cfgitem_init(
  void
)
{
  sequence_number_init();
  cfgitems_init();
  #ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
  cfgitems_get_persisted_values();
  #endif
}


 #ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- policyman_cfgitem_persistence_allowed --------*/

STATIC boolean policyman_cfgitem_persistence_allowed(
  void
)
{
  return policyman_state_thermal_allow_persistence();
}

/*-------- policyman_persist_cfgitems --------*/
void
policyman_persist_cfgitems(
  policyman_set_t *pNotifySet
)
{
  size_t                    i;
  size_t                    nElems;
  policyman_item_id_t       id;
  policyman_item_t const   *pItem;
  policyman_item_t const  **ppItem;
  boolean                   doUpdate = FALSE;
  uint8                     subsMask = 0;

  if (!policyman_cfgitem_persistence_allowed())
  {
    POLICYMAN_MSG_HIGH_0("Persistence of Config items not allowed...");
    return;
  }


  /*  Do not persist if we're in test mode, or if device has not been calibrated yet.
   */
  if (policyman_is_test_mode() || !policyman_state_device_calibrated() )
  {
    return;
  }

  nElems = policyman_set_num_elems(pNotifySet);
  for (i = 0; i < nElems ; ++i)
  {
    ppItem = (policyman_item_t const **) policyman_set_elem_ptr(pNotifySet, i);
    if (ppItem == NULL)
    {
      break;
    }

    pItem = *ppItem;   
    id = policyman_item_get_id(pItem);

    switch(id)
    {
      case POLICYMAN_ITEM_SVC_MODE:
        policyman_svc_mode_update_to_efs(pItem);
        break;

      case POLICYMAN_ITEM_RAT_CAPABILITY:
      case POLICYMAN_ITEM_RF_BANDS:
        doUpdate = TRUE;
        subsMask |= 1<<(uint8)pItem->asubs_id;
        break;

      case POLICYMAN_ITEM_DEVICE_CONFIGURATION:
        policyman_device_config_update_to_efs(pItem);
        break;

      default:
        POLICYMAN_MSG_MED_2("subs %d: items id %d doesn't need update", pItem->asubs_id, id);
    }
  }

  if(doUpdate)
  {
    policyman_update_rats_bands_to_efs(subsMask);
  }
}

#endif
/*-------- policyman_cfgitem_reset_changed --------*/
STATIC void policyman_cfgitem_reset_changed(
  void
)
{
  size_t                   itemIndex;

  for (itemIndex = 0; itemIndex < NUM_PM_CFG_ITEMS; ++itemIndex)
  {
    policyman_cfgitems[itemIndex].is_changed = FALSE;
  }
}

 #ifndef FEATURE_DISABLE_DYNAMIC_POLICIES

/*-------- policyman_cfgitem_devcfg_has_changes --------*/
STATIC boolean policyman_cfgitem_devcfg_has_changes(
  void
)
{
  return policyman_cfgitems[0].is_changed;
}

/*-------- policyman_cfgitem_subs_has_changes --------*/
STATIC boolean policyman_cfgitem_subs_has_changes(
  size_t    subsIndex
)
{
  boolean  has_changes = FALSE;
  size_t   itemIndex;
  size_t   startIndex;
  size_t   endIndex;

  startIndex = 1 + (subsIndex * NUM_PM_SUBS_ITEMS);
  endIndex   = startIndex + NUM_PM_SUBS_ITEMS;

  // Skip device config item 0 for per-subs items
  for (itemIndex = startIndex; itemIndex < endIndex; ++itemIndex)
  {
    if (   (policyman_cfgitems[itemIndex].subsId == subsIndex)
        &&  policyman_cfgitems[itemIndex].is_changed )
    {
      has_changes = TRUE;
      break;
    }
  }

  return has_changes;
}

/*-------- policyman_cfgitem_send_diag_cfg_log --------*/
void policyman_cfgitem_send_diag_cfg_log(
  void
)
{
  policyman_item_t         *pCfgItem;

  if (!policyman_cfgitem_devcfg_has_changes())
  {
    POLICYMAN_UT_MSG_0("send_diag_cfg_log: no changes to config");
    return;
  }

  pCfgItem = policyman_cfgitems[0].pItem;

  policyman_device_config_create_and_send_diag_cfg_log(pCfgItem);
}

STATIC void policyman_cfgitem_set_item_changed_mask(
  LOG_PM_SUBS_INFO_type   *log_ptr,
  policyman_item_t        *pItem,
  size_t                   itemIndex
)
{
  policyman_item_id_t  id;

  id = policyman_item_get_id(pItem);

  POLICYMAN_UT_MSG_4( "SUBS_LOG: item at index %d, id %d, subs %d: changed %d",
                      itemIndex,
                      id,
                      pItem->asubs_id,
                      policyman_cfgitems[itemIndex].is_changed );

  if (policyman_cfgitems[itemIndex].is_changed)
  {
    log_ptr->changed_mask |= BM(id);
  }
}

/*-------- policyman_cfgitem_fill_diag_subs_log --------*/
STATIC void policyman_cfgitem_fill_diag_subs_log(
  size_t                      subsIndex,
  LOG_PM_SUBS_INFO_type      *log_ptr
)
{
  size_t   itemIndex;
  size_t   startIndex;
  size_t   endIndex;

  startIndex = 1 + (subsIndex * NUM_PM_SUBS_ITEMS);
  endIndex   = startIndex + NUM_PM_SUBS_ITEMS;

  // Iterate through subs items.
  // Skip device config item 0 for per-subs items
  for (itemIndex = startIndex; itemIndex < endIndex; ++itemIndex)
  {
    policyman_item_t    *pItem;

    pItem = policyman_cfgitems[itemIndex].pItem;

    if (NULL == pItem)
    {
      continue;
    }

    policyman_cfgitem_set_item_changed_mask(log_ptr, pItem, itemIndex);

    POLICYMAN_UT_MSG_1("CONFIG_LOG: changed_mask 0x%08x", log_ptr->changed_mask);

    if (NULL != pItem->pCfgInfo->pfnFillDiagSubsLog)
    {
      pItem->pCfgInfo->pfnFillDiagSubsLog(pItem, log_ptr);
    }
  }
}

/*-------- policyman_cfgitem_allocate_and_init_diag_subs_log --------*/
STATIC LOG_PM_SUBS_INFO_type * policyman_cfgitem_allocate_and_init_diag_subs_log(
  void
)
{
  LOG_PM_SUBS_INFO_type      *log_ptr;

  log_ptr = (LOG_PM_SUBS_INFO_type *)log_alloc_ex( LOG_PM_SUBS_INFO_C,
                                                   sizeof(LOG_PM_SUBS_INFO_type) );

  if (NULL != log_ptr)
  {
    log_ptr->version      = LOG_PM_SUBS_INFO_LOG_PACKET_VERSION;
    log_ptr->subs         = SYS_MODEM_AS_ID_1;
    log_ptr->flags        = 0;
    log_ptr->feature0     = 0;
    log_ptr->changed_mask = 0x00000000;
    log_ptr->rat_order_table_len    = 0;
    log_ptr->num_rat_order_entries  = 0;
    log_ptr->voice_dom_pref = 0;
  }

  return log_ptr;
}

/*-------- policyman_cfgitem_send_diag_subs_log --------*/
void policyman_cfgitem_send_diag_subs_log(
  size_t  subsIndex
)
{
  LOG_PM_SUBS_INFO_type      *log_ptr;

  if (!policyman_cfgitem_subs_has_changes(subsIndex))
  {
    POLICYMAN_UT_MSG_1("send_diag_subs_log: no changes to subs %d", subsIndex);
    return;
  }

  log_ptr = policyman_cfgitem_allocate_and_init_diag_subs_log();

  if (NULL != log_ptr)
  {
    policyman_cfgitem_fill_diag_subs_log(subsIndex, log_ptr);

    /* Submit the log to DIAG.
    */
    log_commit( (void *)log_ptr );
  }
}

/*-------- policyman_cfgitem_send_diag_logs --------*/
void policyman_cfgitem_send_diag_logs(
  mre_set_t    *pNotifySet
)
{
  size_t  subsIndex;

  if (mre_set_is_empty(pNotifySet))
  {
    return;
  }

  policyman_cfgitem_send_diag_cfg_log();

  for (subsIndex = 0; subsIndex < POLICYMAN_NUM_SUBS; ++subsIndex)
  {
    policyman_cfgitem_send_diag_subs_log(subsIndex);
  }
}

#endif

/*-------- add_cfgitem_to_set --------*/
STATIC boolean add_cfgitem_to_set(
  void  *pElem,
  void  *pData1,
  void  *pData2
)
{
  policyman_config_item_t *pCfgItem = (policyman_config_item_t *) pElem;
  mre_set_t               *pItemSet = (mre_set_t *) pData1;

  mre_set_add(pItemSet, &pCfgItem->pItem);

  return TRUE;
}



/*-------- ensure_item --------*/
STATIC boolean ensure_item(
  void  *pElem,
  void  *pData1,
  void  *pData2
)
{
  policyman_config_item_t *pCfgItem = (policyman_config_item_t *) pElem;

  if (NULL == pCfgItem->pItem && NULL != pCfgItem->pCfgInfo->pfnGetDefault)
  {
    pCfgItem->pItem = pCfgItem->pCfgInfo->pfnGetDefault(pCfgItem->subsId);
  }

  return TRUE;
}


/*-------- policyman_cfgitem_ensure_items --------*/
void policyman_cfgitem_ensure_items(
  void
)
{
  POLICYMAN_MSG_HIGH_0("========== Ensure all database items");

  policyman_cfgitems_iterate(ensure_item, NULL, NULL);

  POLICYMAN_MSG_SEPARATOR(POLICYMAN_SEPARATOR_EQUAL);
}


/*-------- get_device_config_info --------*/
STATIC boolean get_device_config_info(
  void  *pElem,
  void  *pData1,
  void  *pData2
)
{
  policyman_config_item_t  *pThisItem   = (policyman_config_item_t *) pElem;
  policyman_config_item_t **ppFoundItem = (policyman_config_item_t **) pData1;

  if (pThisItem->pCfgInfo->id == POLICYMAN_ITEM_DEVICE_CONFIGURATION)
  {
    *ppFoundItem = pThisItem;
    return FALSE;
  }

  return TRUE;
}

/*-------- policyman_cfgitem_get_device_config --------*/
policyman_item_t *policyman_cfgitem_get_device_config(
  void
)
{
  policyman_config_item_t *pCfg = NULL;

  policyman_cfgitems_iterate(get_device_config_info, &pCfg, NULL);

  return (pCfg != NULL) ? pCfg->pItem : NULL;
}

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES

/*-------- policyman_cfgitem_ensure_device_config --------*/
void policyman_cfgitem_ensure_device_config(
  void
)
{
  policyman_config_item_t *pCfgItem;

  POLICYMAN_MSG_HIGH_0("========== Ensure device config");

  policyman_cfgitems_iterate(get_device_config_info, &pCfgItem, NULL);
  if (NULL != pCfgItem)
  {
    ensure_item(pCfgItem, NULL, NULL);
  }

  POLICYMAN_MSG_SEPARATOR(POLICYMAN_SEPARATOR_EQUAL);
}
#endif
/*-------- config_item_deinit --------*/
STATIC boolean config_item_deinit(
  void  *pElem,
  void  *pData1,
  void  *pData2
)
{
  policyman_config_item_t *pCfgItem = (policyman_config_item_t *) pElem;

  if (NULL != pCfgItem->pItem)
  {
    ref_cnt_obj_release(pCfgItem->pItem);
    pCfgItem->pItem = NULL;
  }

  return TRUE;
}


/*-------- policyman_cfgitem_deinit --------*/
void policyman_cfgitem_deinit(
  void
)
{
  policyman_cfgitems_iterate(config_item_deinit, NULL, NULL);
}


/*-------- policyman_cfgitem_new_item --------*/
policyman_item_t*
policyman_cfgitem_new_item(
  policyman_item_id_t    id,
  size_t                 itemSize,
  destructor_ptr         dtor
  )
{
  policyman_item_t  *pItem;

  if (id == POLICYMAN_ITEM_INVALID)
  {
    POLICYMAN_ERR_FATAL("Attempting to create policyman item with invalid ID", 0, 0, 0);
  }

  pItem = policyman_mem_alloc(itemSize);
  ref_cnt_obj_init(pItem, dtor);
  
  pItem->pCfgInfo = &policyman_cfg_info[id - 1];

  return pItem;
}



/*-------- policyman_cfgitem_find_item --------*/
/**
@brief  Find an item in the configuration item database.  Private function.

@param[in]  id               ID of the configuration item.

@return
  - Pointer to the configuration item if it is in the database.
  - NULL otherwise.
*/
STATIC policyman_config_item_t* policyman_cfgitem_find_item(
  policyman_item_id_t    id,
  sys_modem_as_id_e_type asubs_id
  )
{
  size_t  itr;
  size_t  start_index;
  size_t  end_index;

  if (id == POLICYMAN_ITEM_DEVICE_CONFIGURATION)
  {
    return &policyman_cfgitems[0];
  }

  if(  asubs_id == SYS_MODEM_AS_ID_NONE
     || asubs_id >= (sys_modem_as_id_e_type) POLICYMAN_NUM_SUBS
    )
  {
    POLICYMAN_MSG_ERROR_1("Unsupported subsId %d", asubs_id);
    return NULL;
  }
  /*
      policyman_cfgitems[] = {device_config_item , 
                                           NUM_PM_SUBS_ITEMS of AS_ID_1 cfg items,
                                           NUM_PM_SUBS_ITEMS of AS_ID_2 cfg items... }
    
       set start index to the start of config items corresponding to asubs_id
   */
  start_index = (NUM_PM_SUBS_ITEMS * asubs_id) + 1;
  end_index = start_index + NUM_PM_SUBS_ITEMS;

  for (itr = start_index ; itr < end_index ; ++itr)
  {
    if (policyman_cfgitems[itr].pCfgInfo->id == id)
    {
      if (policyman_cfgitems[itr].pItem == NULL ||
          policyman_cfgitems[itr].pItem->asubs_id == asubs_id
         )
      {
        return &policyman_cfgitems[itr];
      }
    }
  }

  return NULL;
}

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- policyman_items_equal --------*/
static boolean 
policyman_items_equal(
  policyman_item_t  *pItem1,
  policyman_item_t  *pItem2
)
{
  sys_modem_as_id_e_type asubs_id;
  policyman_item_id_t 	    id    = policyman_cfgitem_id_msim(pItem1,&asubs_id);
  policyman_config_item_t  *pInfo = policyman_cfgitem_find_item(id, asubs_id);

  if (NULL == pInfo)
  {
    POLICYMAN_MSG_ERROR_1("Unable to retrieve config item id %d for compare", id);
    return FALSE;
  }
  
  return pInfo->pCfgInfo->pfnItemsEqual(pItem1,pItem2);
}


/*-------- policyman_cfgitem_update_item --------*/
/**
@brief  Update (or add) an item in the configuration database.  Private function.

@param[in]  pNewItem    Pointer to the item with which to replace the old
                        configuration item.
@param[in]  asubs_id   Subscription Id for which old configuration item to be replaced.

@return
  TRUE if item was updated (i.e. the new item differs from the cached item).
*/
static boolean policyman_cfgitem_update_item(
  policyman_item_t        *pNewItem
)
{
  policyman_item_id_t       id;
  policyman_config_item_t  *pInfo;
  boolean                   itemUpdated = FALSE;

  id    = policyman_cfgitem_id(pNewItem);
  pInfo = policyman_cfgitem_find_item(id, pNewItem->asubs_id);

  if (pInfo != NULL)
  {
    policyman_item_t         *pOldItem    = NULL;

    pOldItem = pInfo->pItem;
    itemUpdated = !policyman_items_equal(pNewItem, pOldItem);

    if (itemUpdated)
    {
      /* Add a reference on the new item for the array.
      */
      ref_cnt_obj_add_ref(pNewItem);
      pInfo->pItem = pNewItem;
      
      // Mark the item as changed since last notification
      pInfo->is_changed = TRUE;

      if (pInfo->pCfgInfo->pfnOnUpdate != NULL)
      {
        if (!policyman_is_initializing())
        {
          pInfo->pCfgInfo->pfnOnUpdate(pNewItem);
        }
      }

      if (pInfo->pCfgInfo->pfnDisplay != NULL)
      {
        pInfo->pCfgInfo->pfnDisplay(pNewItem);
      }

      REF_CNT_OBJ_RELEASE_IF(pOldItem);
    }
  }

  return itemUpdated;
}

#endif
/*-------- policyman_cfgitem_release_msg_attach --------*/
void
policyman_cfgitem_release_msg_attach(
  msgr_hdr_s  *pMsg
  )
{
  msgr_attach_s *pAttach;
  size_t        i;

  /*  Release all reference-counted attachments
  */
  for (i = 0 ; i < msgr_get_num_attach(pMsg) ; ++i)
  {
    pAttach = msgr_get_attach(pMsg, i);
    if (pAttach->type == MSGR_ATTACH_REFCNT)
    {
      REF_CNT_OBJ_RELEASE_IF(pAttach->data.refcnt.refcnt_obj_ptr);
    }
  }
}


/*-------- policyman_item_collection_dtor --------*/
static void
policyman_item_collection_dtor(
  void  *pObj
  )
{
  policyman_item_collection_t *pCollection;
  size_t                      i;

  pCollection = (policyman_item_collection_t *) pObj;
  for (i = 0 ; i < pCollection->numItems ; ++i)
  {
    ref_cnt_obj_release((void *) pCollection->pItems[i]);
  }

  policyman_mem_free(pObj);
}


/*-------- policyman_cfgitem_notify_msg_dtor --------*/
void
policyman_cfgitem_notify_msg_dtor(
  void  *pObject
  )
{
  msgr_hdr_s    *pMsg = (msgr_hdr_s *) pObject;

  policyman_cfgitem_release_msg_attach(pMsg);

  /*  Free the message memory.
   */
  policyman_mem_free(pMsg);
}


/*-------- policyman_item_collection_new --------*/
static policyman_item_collection_t *
policyman_item_collection_new(
  size_t            nItems,
  policyman_item_t  **ppItems,
  boolean           addRef
  )
{
  policyman_item_collection_t *pCollection;
  size_t                      bufSize = nItems * sizeof(policyman_item_t *);
  size_t                      i;

  /*  Allocate a collection of the proper size and initialize it.
   */
  pCollection = (policyman_item_collection_t *)
                    policyman_mem_alloc(sizeof(policyman_item_collection_t) + bufSize);
  ref_cnt_obj_init(pCollection, policyman_item_collection_dtor);

  /*  Set the sequence number of the collection to the current database sequence number.
   */
  pCollection->seqno = sequence_number_get();

  /*  Add a reference to all items going into the collection.
   */
  if (addRef)
  {
    for (i = 0 ; i < nItems ; ++i)
    {
      ref_cnt_obj_add_ref(ppItems[i]);
    }
  }

  /*  Add the items to the collection.
   */
  pCollection->numItems = nItems;
  memscpy((policyman_item_t *) pCollection->pItems, bufSize, ppItems, bufSize);

  return pCollection;
}


/*-------- policyman_cfgitem_notify_msg_msim_new --------*/
msgr_hdr_s *policyman_cfgitem_notify_msg_msim_new(
  policyman_item_t  **ppItems,
  size_t            numItems,
  size_t            *pMsgSize
)
{
  size_t                      msgSize = sizeof(msgr_hdr_s) + sizeof(msgr_attach_s);
  msgr_hdr_s                  *pMsg;
  msgr_attach_s               *pAttach;
  policyman_item_collection_t *pCollection;

  /*  Allocate a message with a single attachment (the collection).
   */
  pMsg = policyman_mem_alloc(msgSize);

  /*  Set up the message.
   */
  msgr_init_hdr_attach( pMsg,
                        MSGR_POLICYMAN_CFG,
                        POLICYMAN_CFG_UPDATE_MSIM_IND,
                        MSGR_PRIORITY_HIGH,
                        1 );

  /*  Get the items into a collection and attach the collection to the message.
   *  Have the collection add references for each item.
   */
  pCollection = policyman_item_collection_new(numItems, ppItems, TRUE);
  pAttach = msgr_get_attach(pMsg, 0);
  if (pAttach != NULL)
  {
    pAttach->type = MSGR_ATTACH_REFCNT;
    pAttach->data.refcnt.refcnt_obj_ptr = pCollection;
  }

  if (pMsgSize != NULL)
  {
    *pMsgSize = msgSize;
  }

  return pMsg;
}



static  notification_hook   g_notificationHook;
static  void                *g_pNotificationContext;


/*-------- policyman_cfgitem_hook_notification --------*/
void
policyman_cfgitem_hook_notification(
  notification_hook hook,
  void              *pContext
  )
{
  g_notificationHook = hook;
  g_pNotificationContext = pContext;
}


/*-------- policyman_cfgitem_display --------*/
boolean policyman_cfgitem_display(
  void  *pElem,
  void  *pData1,
  void  *pData2
  )
{
  policyman_item_t  *pItem = *((policyman_item_t **) pElem);

  if (NULL != pItem && NULL != pItem->pCfgInfo)
  {
    pItem->pCfgInfo->pfnDisplay(pItem);
  }

  return TRUE;
}


/*-------- config_item_display --------*/
STATIC boolean config_item_display(
  void  *pElem,
  void  *pData1,
  void  *pData2
)
{
  policyman_config_item_t  *pCfgItem = (policyman_config_item_t *) pElem;

  return  policyman_cfgitem_display(&pCfgItem->pItem, pData1, pData2);
}


/*-------- policyman_cfgitem_display_all --------*/
void policyman_cfgitem_display_all(
  void
)
{
  POLICYMAN_MSG_SEPARATOR(POLICYMAN_SEPARATOR_HYPHEN);
  POLICYMAN_MSG_HIGH_1("Item database sequence#: %d", sequence_number_get());
  policyman_cfgitems_iterate(config_item_display, NULL, NULL);
}


STATIC boolean set_item_seqno(
  void  *pElem,
  void  *pData1,
  void  *pData2
)
{
  policyman_item_t  *pItem = *((policyman_item_t **) pElem);
  size_t             seqno = (size_t) pData1;

  pItem->seqno = seqno;

  return TRUE;
}


/*-------- policyman_cfgitem_notify_clients --------*/
void policyman_cfgitem_notify_clients(
  policyman_set_t *pItemSet
)
{
  size_t              nItems;
  policyman_item_t  **ppItems;
  size_t              msgSize;
  msgr_hdr_s         *pMsg;

  /*  Get the number of items to be udpated and a pointer to the buffer
   *  in the itemSet that holds those pointers.
   */
  nItems = policyman_set_num_elems(pItemSet);
  ppItems = (policyman_item_t **) policyman_set_get_buf_ptr(pItemSet);

  /*  Increment database sequence number and set it as the sequence number
   *  on all the items.
   */
  sequence_number_increment();
  policyman_set_iterate(pItemSet, set_item_seqno, (void *) sequence_number_get(), NULL);

  POLICYMAN_MSG_HIGH_2( "********** Notifying clients of changes to %d items, sequence #%d:",
                        nItems,
                        sequence_number_get() );

  policyman_set_iterate(pItemSet, policyman_cfgitem_display, NULL, NULL);

  POLICYMAN_MSG_SEPARATOR(POLICYMAN_SEPARATOR_STAR);

  if (g_notificationHook != NULL)
  {
    g_notificationHook(g_pNotificationContext, nItems, ppItems);
  }

  /*  Send the MSIM message
   */
  pMsg = policyman_cfgitem_notify_msg_msim_new(ppItems, nItems, &msgSize);
  msgr_send(pMsg, msgSize);
  policyman_cfgitem_notify_msg_dtor(pMsg);

  policyman_cfgitem_send_diag_logs(pItemSet);
  policyman_cfgitem_reset_changed();
}

/*-------- policyman_send_all_config_items --------*/
void policyman_send_all_config_items(
  void
)
{
  policyman_set_t   *pItemSet;

  pItemSet = policyman_itemset_new();

  policyman_cfgitems_iterate(add_cfgitem_to_set, pItemSet, NULL);
  policyman_cfgitem_notify_clients(pItemSet);
  
  #ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
  policyman_persist_cfgitems(pItemSet);
  #endif

  ref_cnt_obj_release(pItemSet);
}

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- update_item --------*/
static boolean update_item(
  void  *pElem,
  void  *pData1,
  void  *pData2
)
{
  policyman_item_t       *pItem       = *((policyman_item_t **) pElem);
  policyman_set_t        *pNotifySet  = (policyman_set_t *) pData1;
  boolean                 itemUpdated;

  itemUpdated = policyman_cfgitem_update_item(pItem);

  if (itemUpdated && pNotifySet != NULL)
  {
    policyman_set_add(pNotifySet, &pItem);
  }

  return TRUE;
}

/*-------- find_and_collect_changed_items --------*/
static boolean find_and_collect_changed_items(
  void  *pElem,
  void  *pData1,
  void  *pData2
)
{
  policyman_item_t        *pNewItem    = *((policyman_item_t **) pElem);
  sys_modem_as_id_e_type   asubs_id;
  policyman_item_id_t      item_id     = policyman_cfgitem_id_msim(pNewItem,&asubs_id);
  policyman_config_item_t *pCfgItem    = policyman_cfgitem_find_item(item_id,asubs_id);
  policyman_item_t        *pActualItem = NULL;
  policyman_set_t         *pNotifySet  = (policyman_set_t *) pData1;
  boolean                  itemSame;

  if (NULL != pCfgItem)
  {
    pActualItem = pCfgItem->pItem;
  }
  else
  {
    POLICYMAN_MSG_ERROR_1("Unable to retrieve changed config item id %d", item_id);
    return FALSE;
  }

  itemSame = policyman_items_equal(pNewItem, pActualItem);
  if (!itemSame )
  {
    policyman_set_add(pNotifySet, &pNewItem);
  }

  return TRUE;
}

/*-------- policyman_cfgitem_persist_and_notify --------*/
STATIC void policyman_cfgitem_persist_and_notify(
  policyman_set_t *pNotifySet
)
{
  /* Notify all clients of the changes. Don't bother during initialization. */
  if (pNotifySet != NULL && !policyman_set_is_empty(pNotifySet))
  {
    /* Update EFS writing of updating policy items    
      */
    policyman_persist_cfgitems(pNotifySet);
    policyman_cfgitem_notify_clients(pNotifySet);
  }
  else
  {
    POLICYMAN_MSG_HIGH_0("No configuration items need updating");
  }
}

/*-------- policyman_cfgitem_update_items --------*/
/*
This queues a command to update the items to the Policy Manager task thread for
later processing in order to decouple updating items and client notification
from the event that caused the policy change and update.
*/
boolean policyman_cfgitem_update_items(
  policyman_set_t *pItemSet
)
{
  policyman_set_t *pNotifySet = NULL;

  if (policyman_policy_is_suspended())
  {
    return TRUE;
  }

  if (!policyman_is_initializing())
  {
    pNotifySet = policyman_itemset_new();
  }

  policyman_set_rat_capability_concurrency_masks(pItemSet);
  policyman_update_rat_for_thermal_level(pItemSet);

  policyman_enter_crit_sect(&policyman_item_crit_sect);
  policyman_set_iterate(pItemSet, update_item, pNotifySet, NULL);  
  policyman_leave_crit_sect(&policyman_item_crit_sect);

  /* Notify all clients of the changes. Don't bother during initialization. */
  policyman_cfgitem_persist_and_notify(pNotifySet);

  REF_CNT_OBJ_RELEASE_IF(pNotifySet);
  return TRUE;
}

#endif
/*-------- policyman_cfgitem_id --------*/
policyman_item_id_t
policyman_cfgitem_id(
  policyman_item_t const  *pItem
  )
{
  return pItem->pCfgInfo->id;
}

/*-------- policyman_cfgitem_id_msim --------*/
policyman_item_id_t
policyman_cfgitem_id_msim(
  policyman_item_t const  *pItem,
  sys_modem_as_id_e_type  *pSubsId
  )
{
  *pSubsId = pItem->asubs_id;
  return policyman_cfgitem_id(pItem);
}

/*-------- policyman_cfgitem_get_items --------*/
size_t
policyman_cfgitem_get_items(
  policyman_item_id_t const *pItemIds,
  size_t                  numIds,
  policyman_item_t const  **ppItems
  )
{
  size_t                    i;
  size_t                    numItems = 0;
  policyman_config_item_t  *pItemInfo;
  policyman_item_t          *pItem;

  policyman_enter_crit_sect(&policyman_item_crit_sect);

  for (i = 0 ; i < numIds ; ++i)
  {
    POLICYMAN_MSG_HIGH_3("policyman_cfgitem_get_items requested for item id %d (%d of %d)", pItemIds[i], i, numIds);

    /* Default search for subscription 1
       Use MSIM API if items needed for other subscriptions
      */    
    pItemInfo = policyman_cfgitem_find_item(pItemIds[i], SYS_MODEM_AS_ID_1);
    if (pItemInfo != NULL)
    {
      pItem = pItemInfo->pItem;
      if (pItem != NULL)
      {
        ref_cnt_obj_add_ref(pItem);
        ppItems[numItems++] = pItem;
      }
    }
  }

  policyman_leave_crit_sect(&policyman_item_crit_sect);

  return numItems;
}


typedef struct
{
  policyman_item_id_t     id;
  sys_modem_as_id_e_type  subs;
} get_cfgitem_ctx_t;


/*-------- get_cfgitem --------*/
STATIC boolean get_cfgitem(
  void  *pElem,
  void  *pData1,
  void  *pData2
)
{
  policyman_config_item_t  *pCfgItem  = (policyman_config_item_t *) pElem;
  policyman_item_t        **ppItem    = (policyman_item_t **) pData1;
  get_cfgitem_ctx_t        *pCtx      = (get_cfgitem_ctx_t *) pData2;
  policyman_item_t         *pItem;

  pItem = pCfgItem->pItem;
  if (    pItem != NULL
       && pCfgItem->subsId == pCtx->subs
       && pCfgItem->pCfgInfo != NULL
       && pCfgItem->pCfgInfo->id == pCtx->id
     )
  {
    ref_cnt_obj_add_ref(pItem);
    *ppItem = pItem;
    return FALSE;
  }

  return TRUE;
}


/*-------- policyman_cfgitem_get_item --------*/
policyman_item_t  const *policyman_cfgitem_get_item(
  policyman_item_id_t     id,
  sys_modem_as_id_e_type  subs
  )
{
  get_cfgitem_ctx_t  ctx;
  policyman_item_t  *pItem = NULL;

  ctx.id = id;
  ctx.subs = subs;

  policyman_cfgitems_iterate(get_cfgitem, &pItem, &ctx);

  return pItem;
}

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- policyman_cfgitem_is_present --------*/
boolean policyman_cfgitem_is_present(
  policyman_item_id_t    id,
  sys_modem_as_id_e_type subsId
)
{
  boolean                  isPresent = FALSE;
  policyman_item_t const  *pItem;
 
  pItem = policyman_cfgitem_get_item(id, subsId);

  if (pItem != NULL)
  {
    ref_cnt_obj_release((void *) pItem);
    isPresent = TRUE;
  }

  return isPresent;
}

/*-------- policyman_cfgitem_already_present --------*/
boolean policyman_cfgitem_already_present(
  policyman_item_id_t      itemId,
  sys_modem_as_id_e_type   policySubs,
  policyman_subs_t         targetSubs
)
{
  boolean                item_is_present = FALSE;
  size_t                 numSim          = 0;

  (void)policyman_get_current_num_sim(&numSim);

  switch (targetSubs)
  {
    case SUBS_THIS:
      item_is_present = policyman_cfgitem_is_present(itemId, policySubs);
      break;

    default:
      item_is_present = FALSE;
      break;
  }

  return item_is_present;
}
#endif
/*-------- policyman_cfgitem_id_in_request --------*/
/**
@brief  Return TRUE if the given ID was requested in a call to
        policyman_cfgitem_get_items_msim.
*/
static boolean
policyman_cfgitem_id_in_request(
  policyman_item_id_t const id,
  policyman_item_id_t const *pIds,
  size_t                    nIds
  )
{
  while (nIds-- != 0)
  {
    if (*pIds++ == id)
    {
      return TRUE;
    }
  }

  return FALSE;
}


/*-------- policyman_cfgitem_get_items_msim --------*/
policyman_status_t
policyman_cfgitem_get_items_msim(
  policyman_item_id_t const   *pIds,
  size_t                      nIds,
  policyman_item_collection_t **ppItems
  )
{
  size_t                    i;
  size_t                    numItems = 0;
  policyman_item_t          *items[ARR_SIZE(policyman_cfg_info) * POLICYMAN_NUM_SUBS];

  /*  Enter the critical section and collect ref-counted copies of any items
   *  requested into our local buffer.
   */
  policyman_enter_crit_sect(&policyman_item_crit_sect);
  for (i = 0 ; i < ARR_SIZE(policyman_cfgitems) ; ++i)
  {
    if ((policyman_cfgitem_id_in_request(policyman_cfgitems[i].pCfgInfo->id, pIds, nIds))
        && (policyman_subs_id_is_configured(policyman_cfgitems[i].subsId)))
    {
      items[numItems] = policyman_cfgitems[i].pItem;

      if (items[numItems] == NULL) { continue; }

      ref_cnt_obj_add_ref(items[numItems]);
      ++numItems;
    }
  }
  policyman_leave_crit_sect(&policyman_item_crit_sect);

  /*  Now, outside the critical section, allocate memory for the item collection
   *  and copy the items to it.  We've already added a reference to the items
   *  while in the critical section, so no need for the collection to do so.
   */
  *ppItems = policyman_item_collection_new(numItems, items, FALSE);

  return POLICYMAN_STATUS_SUCCESS;
}


/*-------- policyman_cfgitem_get_items_per_subs --------*/
size_t
policyman_cfgitem_get_items_per_subs(
  policyman_item_id_t const *pItemIds,
  size_t                  numIds,
  policyman_item_t const  **ppItems,
  sys_modem_as_id_e_type   subs_id
  )
{
  size_t                  i;
  size_t                  numItems = 0;
  policyman_config_item_t  *pItemInfo;
  policyman_item_t        *pItem;

  policyman_enter_crit_sect(&policyman_item_crit_sect);

  for (i = 0 ; i < numIds ; ++i)
  {
    POLICYMAN_MSG_HIGH_1("policyman_cfgitem_get_items_per_subs requested for item id %d", pItemIds[i]);

    /* Default search for subscription 1
       Use MSIM API if items needed for other subscriptions
      */    
    pItemInfo = policyman_cfgitem_find_item(pItemIds[i], subs_id);
    if (pItemInfo != NULL)
    {
      pItem = pItemInfo->pItem;
      if (pItem != NULL)
      {
        ref_cnt_obj_add_ref(pItem);
        ppItems[numItems++] = pItem;
      }
    }
  }

  policyman_leave_crit_sect(&policyman_item_crit_sect);

  return numItems;
}


/*-------- ids_are_equal --------*/
static boolean
ids_are_equal(
  void  *pElem1,
  void  *pElem2
  )
{
  policyman_item_t  *pItem1 = *((policyman_item_t **) pElem1);
  policyman_item_t  *pItem2 = *((policyman_item_t **) pElem2);
  
  return    policyman_cfgitem_id(pItem1) == policyman_cfgitem_id(pItem2)
         && pItem1->asubs_id == pItem2->asubs_id
         ;
}


/*-------- policyman_itemset_new --------*/
policyman_set_t *
policyman_itemset_new(
  void
  )
{
  return  policyman_set_refcnt_new(
              ARR_SIZE(policyman_cfg_info),
              ARR_SIZE(policyman_cfgitems),
              ids_are_equal
            );
}


/*-------- policyman_report_msg_free --------*/
void
policyman_report_msg_free(
  msgr_hdr_s  *pMsg
  )
{
  if (pMsg != NULL)
  {
    policyman_cfgitem_notify_msg_dtor(pMsg);
  }
}

/*-------- policyman_ensure_default_item --------*/
void policyman_ensure_default_item(
  sys_modem_as_id_e_type  subsId,
  policyman_item_id_t     item_id,
  policyman_set_t        *pItemSet
)
{
  size_t index;

  POLICYMAN_MSG_HIGH_2("++++++++++ Getting default item %d for subs %d", item_id, subsId);

  /* Update items for this subs
    */
  for (index = 0 ; index < ARR_SIZE(policyman_cfgitems) ; index++)
  {
    policyman_config_item_t *pCfgItem;


    pCfgItem = &policyman_cfgitems[index];
    if ((pCfgItem->pCfgInfo->id == item_id)
       && (pCfgItem->subsId == subsId)
      )
    {
      policyman_item_t        *pDefaultItem;
	  
      pDefaultItem = pCfgItem->pCfgInfo->pfnGetDefault(subsId);
      if (pDefaultItem != NULL)
      {
        policyman_set_add(pItemSet, &pDefaultItem);
        REF_CNT_OBJ_RELEASE_IF(pDefaultItem);
      }
    }
  }

  POLICYMAN_MSG_SEPARATOR(POLICYMAN_SEPARATOR_PLUS);
}


/*-------- policyman_cfigitem_cmp_ids --------*/
static boolean
policyman_cfigitem_cmp_ids(
  void  *pData1,
  void  *pData2
  )
{
  policyman_item_t     *pItem1 = *((policyman_item_t **) pData1);
  policyman_item_cmp_t *pItem2 = (policyman_item_cmp_t *) pData2;

  return    policyman_cfgitem_id(pItem1) == pItem2->id
         && pItem1->asubs_id == pItem2->asubs_id
         ;
}

/*-------- policyman_cfgitem_find_item_in_itemset --------*/
void *
policyman_cfgitem_find_item_in_itemset(
  policyman_set_t       *pItemSet,
  policyman_item_id_t    id,
  sys_modem_as_id_e_type asubs_id
)
{
  policyman_item_cmp_t item;

  item.id = id;
  item.asubs_id = asubs_id;
    
  return policyman_set_find( pItemSet, policyman_cfigitem_cmp_ids, (void *)&item );
}

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- policyman_compute_changed_set --------*/
static void
policyman_compute_changed_set(
  policyman_set_t       *pItemSet,
  policyman_set_t       *pChangeSet
)
{
  policyman_set_iterate(pItemSet, find_and_collect_changed_items, pChangeSet, NULL);
}

/*-------- policyman_cfgitem_get_change_msg_hdr --------
   checks if pItemSet has config items that differ from database
   if found any such items, generates a ref cnted attach object
   and returns hdr to the clients
*/
msgr_hdr_s *policyman_cfgitem_get_change_msg_hdr(
  policyman_set_t  *pItemSet
)
{  
  policyman_set_t    *pChangeSet = NULL;
  msgr_hdr_s         *pMsg       = NULL;
  policyman_item_t  **ppItems;

  pChangeSet = policyman_itemset_new();
  
  /* If itemset is not empty, generate the changed set */
  if (!policyman_set_is_empty(pItemSet))
  {
    policyman_compute_changed_set(pItemSet,pChangeSet);
  }

  if (!policyman_set_is_empty(pChangeSet)) 
  {
    /* Create the MSGR message from changed set */
   
    ppItems = (policyman_item_t **) policyman_set_get_buf_ptr(pChangeSet);
    pMsg = policyman_cfgitem_notify_msg_msim_new(ppItems, policyman_set_num_elems(pChangeSet), NULL);
  }
  
  REF_CNT_OBJ_RELEASE_IF(pChangeSet);
  return pMsg;
}

/*-------- policyman_cfgitem_persist_item --------*/
boolean policyman_cfgitem_persist_item(
  char const              *pPath,
  void                    *pData,
  size_t                   dataSize,
  sys_modem_as_id_e_type   asubs_id
)
{
  mre_efs_status_t status;
  /* the last argument is false as Policyman doesn't
   * want to persist the item if /policyman folder
   * is not already present 
   */
  status = mre_efs_put_file( pPath,
                             pData,
                             dataSize,
               /*O_AUTODIR*/ FALSE,
                             asubs_id );

  return (MRE_EFS_STATUS_SUCCESS == status);
}


/*-------- policyman_cfgitem_find_item_in_context --------*/
void *policyman_cfgitem_find_item_in_context(
  void                  *pCtx,
  policyman_item_id_t    id,
  sys_modem_as_id_e_type asubs_id
)
{
  policy_execute_ctx_t *pContext = (policy_execute_ctx_t *) pCtx;  
  policyman_set_t      *pItemSet = pContext->pItemSet;  
  policyman_item_t    **ppItem   = NULL;  
  policyman_item_t     *pItem    = NULL;

  /* check if item with 'id' present in pItemSet */
  if ( NULL != pItemSet )  
  {    
    ppItem = policyman_cfgitem_find_item_in_itemset( pItemSet, id, asubs_id);  
  }

  /* if item with 'id' found in pItemSet, add ref
     as policyman_cfgitem_find_item_in_itemset doesn't do that*/
  if ( NULL != ppItem )  
  {    
    pItem = *ppItem;
    ref_cnt_obj_add_ref(pItem);
    POLICYMAN_MSG_HIGH_1("Found item %d in context itemset", id);
  }  
  else  /* get from Policyman config DB */
  {    
    policyman_cfgitem_get_items( &id,                                 
                                 1,                                 
                                (policyman_item_t const **)&pItem );
  }  
  
  return pItem;
}


/*-------- add_to_itemset_if_sequence_number_greater --------*/
boolean add_to_itemset_if_sequence_number_greater(
  void  *pElem,
  void  *pData1,
  void  *pData2
)
{
  policyman_config_item_t *pCfgItem = (policyman_config_item_t *) pElem;
  size_t                   seqno    = (size_t) pData1;
  mre_set_t               *pItemSet = (mre_set_t *) pData2;
  policyman_item_t        *pItem;

  pItem = pCfgItem->pItem;
  if (seqno < pItem->seqno)
  {
    mre_set_add(pItemSet, &pItem);
  }

  return TRUE;
}

/*-------- policyman_get_items_since_sequence_number --------*/
policyman_item_collection_t *policyman_get_items_since_sequence_number(
  size_t  seqno
)
{
  policyman_item_collection_t *pCollection = NULL;

  if (sequence_number_get() <= seqno)
  {
    POLICYMAN_MSG_HIGH_1("No items changed since sequence number %d", seqno);
  }
  else
  {
    mre_set_t         *pItemSet;
    size_t             numItems;
    policyman_item_t **ppItems;

    pItemSet = policyman_itemset_new();

    policyman_cfgitems_iterate(add_to_itemset_if_sequence_number_greater, (void *) seqno, pItemSet);
    
    numItems = mre_set_num_elems(pItemSet);
    ppItems = mre_set_get_buf_ptr(pItemSet);
    pCollection = policyman_item_collection_new(numItems, ppItems, TRUE);

    POLICYMAN_MSG_SEPARATOR(POLICYMAN_SEPARATOR_HYPHEN);
    POLICYMAN_MSG_HIGH_2("Returning %d items changed since sequence number %d:", numItems, seqno);
    mre_set_iterate(pItemSet, policyman_cfgitem_display, NULL, NULL);

    ref_cnt_obj_release(pItemSet);
  }

  return pCollection;
}
#endif
