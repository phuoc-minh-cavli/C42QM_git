/**
  @file policy_rat_capability.c

  @brief  Policy to modify the RAT capability of the device based on the IIN
          of the current SIM.
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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/src/policyman_rat_capability.c#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/

#include "mre_efs.h"

#include "policyman_svc_mode.h"
#include "policyman_rat_capability.h"
#include "policyman_cfgitem.h"
#include "policyman_dbg.h"
#include "policyman_device_config.h"
#include "policyman_diag.h"
#include "policyman_efs.h"
#include "policyman_rf.h"
#include "policyman_set.h"
#include "policyman_state.h"
#include "policyman_subs.h"
#include "policyman_task.h"
#include "policyman_uim.h"
#include "policyman_util.h"
#include "policyman_xml.h"

#include "event.h"
#include "sys.h"
#include <stringl/stringl.h>

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
#define RAT_CAPABILITY_TAG         "rat_capability"
#define RAT_CAPABILITY_IF_TAG      "rat_capability_if"

typedef struct
{
  sys_band_mask_type        gw_band_mask;
  sys_lte_band_mask_e_type  lte_band_mask;
  sys_band_mask_type        tds_band_mask;
  uint32                    rat_mask;
} rat_capability_efs_t;


typedef struct
{
  BASE_EXECUTE_CTX;

  boolean    includeHdrOnDds;
  boolean    wcdmaPresentOnDds;
} rat_execute_ctx_t;

#endif

/*=============================================================================
  Rat capability methods for cfgitem
=============================================================================*/

/*-------- policyman_rat_capability_item_new --------*/
policyman_item_t * policyman_rat_capability_item_new(
  policy_execute_ctx_t   *pCtx,
  uint32                  mask
)
{
  rat_capability_t *prc;
  uint32            subphoneMask;

  prc = (rat_capability_t *) policyman_cfgitem_new_item( POLICYMAN_ITEM_RAT_CAPABILITY,
                                                         sizeof(rat_capability_t),
                                                         policyman_simple_dtor );

  /*  Filter the RATs based on subphone capabilities.
   */
  subphoneMask = policyman_rat_capability_get_max_config(pCtx); 
  if (subphoneMask == 0)
  {
    subphoneMask = policyman_rf_get_overall_rat_mask();
  }

  prc->ratMask     = mask;
  prc->subphoneCap = subphoneMask;
  prc->asubs_id    = pCtx->asubs_id;  
  
  return (policyman_item_t *) prc;
}

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- policyman_rat_capability_get_default_from_efs --------*/
policyman_item_t * policyman_rat_capability_get_default_from_efs(
  sys_modem_as_id_e_type subs_id
)
{
  policyman_item_t       *prc      = NULL;
  uint32                  ratMask;
  sys_band_mask_type        gwBands;
  sys_lte_band_mask_e_type  lteBands;
  sys_band_mask_type        tdsBands;
  policy_execute_ctx_t    ctx;

  /* read EFS for RAT/BAND capability
   */
  if (!policyman_retrieve_rats_bands(subs_id, &ratMask, &gwBands, &lteBands, &tdsBands))
  {
    POLICYMAN_MSG_ERROR_1("subs %d: can't populate RAT item from EFS", subs_id);
    goto Done;
  }

  ctx.pItemSet = NULL;
  ctx.asubs_id = subs_id;
   
  prc = policyman_rat_capability_item_new(&ctx, ratMask);

Done:
  return (policyman_item_t *)prc;
}

#endif
/*-------- policyman_rat_capability_get_default --------*/
policyman_item_t * policyman_rat_capability_get_default(
  sys_modem_as_id_e_type subsId
  )
{
  uint32                mask;
  policy_execute_ctx_t  ctx;
  rat_capability_t     *prc;
  uint32                rat_disable_mask;

  rat_disable_mask = policyman_state_get_thermal_rat_disable_mask();
  mask             = policyman_rf_get_overall_rat_mask();

  /* Apply rat disable mask becuase of thermal runaway
  */
  mask &= ~rat_disable_mask;
  ctx.asubs_id = subsId;
  ctx.pItemSet = NULL;

  prc = (rat_capability_t *)policyman_rat_capability_item_new(&ctx, mask);
  POLICYMAN_MSG_HIGH_2("Default RAT capability for subs %d: mask = 0x%04x", subsId, prc->ratMask);

  return (policyman_item_t *)prc;
}

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- policyman_rat_capability_get_base_rats --------*/
STATIC uint32 policyman_rat_capability_get_base_rats(
  policyman_item_t const *pItem
)
{
  rat_capability_t  *prc  = (rat_capability_t *)pItem;  

  return prc->ratMask;
}
#endif
/*-------- policyman_rat_capability_get_effective_rats --------*/
STATIC uint32 policyman_rat_capability_get_effective_rats(
  policyman_item_t *pItem
)
{
  uint32             ratCap;  
  rat_capability_t  *prc     = (rat_capability_t *)pItem;  

  ratCap  = prc->ratMask | prc->ratIncludeMask;
  ratCap &= ~ prc->ratExcludeMask;
  ratCap &= prc->subphoneCap;

  return ratCap;
}

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- policyman_rat_capability_compare --------*/
boolean policyman_rat_capability_compare(
  policyman_item_t  *pData1,
  policyman_item_t  *pData2
  )
{
  rat_capability_t  *prc1 = (rat_capability_t *) pData1;
  rat_capability_t  *prc2 = (rat_capability_t *) pData2;

  return    prc1 != NULL
         && prc2 != NULL
         && (policyman_rat_capability_get_effective_rats(pData1) == policyman_rat_capability_get_effective_rats(pData2));
}
#endif
/*-------- policyman_rat_capability_display --------*/
void policyman_rat_capability_display
(
  policyman_item_t  *pItem
)
{
  rat_capability_t  *prc = (rat_capability_t *)pItem;

  POLICYMAN_MSG_HIGH_5("Rat capability for subs %d: Rat cap 0x%04x, Base 0x%04x, subphone cap 0x%04x, exclude mask 0x%04x",
                        prc->asubs_id, 
                        policyman_rat_capability_get_effective_rats(pItem),
                        prc->ratMask,
                        prc->subphoneCap,
                        prc->ratExcludeMask );
}

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- policyman_rat_capability_on_update --------*/
void policyman_rat_capability_on_update(
  policyman_item_t  *pItem
)
{
  rat_capability_t                *prc           = (rat_capability_t *)pItem;
  policyman_diag_event_rat_cap_t   event_rat_cap;

  event_rat_cap.ratMask  = policyman_rat_capability_get_effective_rats(pItem);
  event_rat_cap.subs     = (int8)prc->asubs_id;

  POLICYMAN_MSG_HIGH_0("Sending EVENT_PM_RAT_CHANGE");

  event_report_payload( EVENT_PM_RAT_CHANGE,
                        sizeof(event_rat_cap),
                        &event_rat_cap );

  return;
}

/*-------- policyman_get_rat_capability_base_for_subs --------*/
STATIC policyman_status_t policyman_get_rat_capability_base_for_subs(
  sys_modem_as_id_e_type  subsId,
  uint32                 *pRatMask
)
{
  size_t                nItems;
  policyman_status_t    status  = POLICYMAN_STATUS_ERR;
  policyman_item_t     *pItem   = NULL;
  policyman_item_id_t   id[]    = { POLICYMAN_ITEM_RAT_CAPABILITY };

  nItems = policyman_cfgitem_get_items_per_subs( (policyman_item_id_t const *)&id, 
                                                 ARR_SIZE(id), 
                                                 (policyman_item_t const **)&pItem, 
                                                 subsId);
  if (nItems == ARR_SIZE(id))
  {
    *pRatMask = policyman_rat_capability_get_base_rats(pItem);
     status  = POLICYMAN_STATUS_SUCCESS;
  }

  REF_CNT_OBJ_RELEASE_IF(pItem);
  return status;
}

/*-------- policyman_rat_capability_clone --------*/
STATIC policyman_item_t *policyman_rat_capability_clone(
 policy_execute_ctx_t   *pCtx
)
{  
  uint32                currentMask = 0; 
  policyman_status_t    status;
  policyman_item_t     *prc         = NULL;  

  // get current RAT capabilities for subs and create a new object
  status = policyman_get_rat_capability_base_for_subs(pCtx->asubs_id, &currentMask);
  if (POLICYMAN_SUCCEEDED(status))
  {
    prc = policyman_rat_capability_item_new(pCtx, currentMask);
  }

  return prc;
}

/*-------- policyman_ensure_rat_capability_for_each_subs --------*/
STATIC void policyman_ensure_rat_capability_for_each_subs(
  rat_execute_ctx_t *pCtx
)
{
  size_t             nSim;
  size_t             subsId;
  policyman_status_t status;

  status = policyman_get_current_num_sim(&nSim);

  // For each subs add RAT capability to itemset if not present
  for (subsId = SYS_MODEM_AS_ID_1; POLICYMAN_SUCCEEDED(status) && subsId < nSim; subsId++)
  {
    policyman_item_t      **ppItem;

    ppItem = policyman_cfgitem_find_item_in_itemset( pCtx->pItemSet, 
                                                     POLICYMAN_ITEM_RAT_CAPABILITY, 
                                                     subsId );
    if (ppItem == NULL)
    {      
      policyman_item_t     *prc;
      policy_execute_ctx_t  ctx;

      // create a new context for given subsId to clone the RAT capability
      ctx.pItemSet = pCtx->pItemSet;
      ctx.pState   = pCtx->pState;
      ctx.asubs_id = subsId;

      // get current RAT capabilities and add to set
      prc = policyman_rat_capability_clone(&ctx);
      
      if (NULL != prc)
      {
        policyman_set_add(pCtx->pItemSet, &prc);
        ref_cnt_obj_release(prc);
      }
    }
  }
}

/*-------- ue_has_3gpp2_subscription --------*/
STATIC boolean ue_has_3gpp2_subscription(
  policy_execute_ctx_t *pCtx
)
{
  cm_rtre_control_e_type  rtre_control;
  boolean                 sim_has_3gpp2_subs;
  boolean                 nv_has_3gpp2_subs;

  rtre_control = policyman_phone_get_rtre_control(SYS_MODEM_AS_ID_1);
 
  sim_has_3gpp2_subs = policyman_uim_sim_is_3gpp2((policy_execute_ctx_t *)pCtx);
  nv_has_3gpp2_subs = (rtre_control == CM_RTRE_CONTROL_NV);

  POLICYMAN_MSG_HIGH_2( "sim_has_3gpp2_subs %d, nv_has_3gpp2_subs %d", 
                        sim_has_3gpp2_subs,
                        nv_has_3gpp2_subs);

  return (sim_has_3gpp2_subs || nv_has_3gpp2_subs);
}

/*-------- determine_concurrency_parameters --------*/
STATIC boolean determine_concurrency_parameters(
  void  *pElem,
  void  *pData1,
  void  *pData2
)
{
  policyman_item_t  *pItem         = *((policyman_item_t **)pElem);
  rat_execute_ctx_t *pCtx          = (rat_execute_ctx_t *)pData1;

  if (policyman_cfgitem_id(pItem) == POLICYMAN_ITEM_RAT_CAPABILITY)
  {
    policy_execute_ctx_t  context;
    uint32                effectiveMask;

    context.asubs_id = pItem->asubs_id;
    context.pItemSet = pCtx->pItemSet;
    context.pState   = pCtx->pState;
        
    effectiveMask = policyman_rat_capability_get_effective_rats(pItem);

    // SubsId in context here is DDS subs
    if (pItem->asubs_id == pCtx->asubs_id)
    {
      // HDR present on DDS SUBS only if it is a 3GPP2 card
      pCtx->includeHdrOnDds =   ue_has_3gpp2_subscription((policy_execute_ctx_t *)pCtx) 
                           && ((effectiveMask & SYS_SYS_MODE_MASK_HDR) != 0);

      // WCDMA present on DDS SUBS
      pCtx->wcdmaPresentOnDds = ((effectiveMask & SYS_SYS_MODE_MASK_WCDMA) != 0);
    }   

    POLICYMAN_MSG_HIGH_2( "includeHdrOnDds %d, wcdmaPresentOnDds %d",
                          pCtx->includeHdrOnDds,
                          pCtx->wcdmaPresentOnDds);
  }

  return TRUE;
}


/*-------- update_thermal_masks --------*/
STATIC boolean update_thermal_masks(
  void  *pElem,
  void  *pData1,
  void  *pData2
)
{
  policyman_item_t	*pItem	 = *((policyman_item_t **)pElem);  
  
  if (policyman_cfgitem_id(pItem) == POLICYMAN_ITEM_RAT_CAPABILITY)
  {
    rat_capability_t	   *prc	= (rat_capability_t *)pItem;

    prc->ratExcludeMask |= policyman_state_get_thermal_rat_disable_mask();
  }

  return TRUE;
}

/*-------- update_concurrency_masks --------*/
STATIC boolean update_concurrency_masks(
  void  *pElem,
  void  *pData1,
  void  *pData2
)
{
  policyman_item_t    *pItem   = *((policyman_item_t **)pElem);
  rat_execute_ctx_t   *pCtx    = (rat_execute_ctx_t *)pData1; 

  // work only on RAT capability item
  if (policyman_cfgitem_id(pItem) == POLICYMAN_ITEM_RAT_CAPABILITY)
  {
    sys_modem_as_id_e_type  ddsSubs = pCtx->asubs_id;
    rat_capability_t       *prc     = (rat_capability_t *)pItem;

    prc->ratIncludeMask = 0;
    prc->ratExcludeMask = 0;

    // non-DDS subs
    if (pItem->asubs_id != ddsSubs)
    {

      /* Exclude data Rats from non-DDS SUBS
             Remove T/W if HDR present on DDS SUBS
             Remove TDS if WCDMA present on DDS SUBS
         */
      prc->ratExcludeMask |= (SYS_SYS_MODE_MASK_HDR | SYS_SYS_MODE_MASK_LTE);

      if (pCtx->includeHdrOnDds)
      {
        prc->ratExcludeMask |= (SYS_SYS_MODE_MASK_TDS | SYS_SYS_MODE_MASK_WCDMA);      
      }

      if (pCtx->wcdmaPresentOnDds)
      {
        prc->ratExcludeMask |= SYS_SYS_MODE_MASK_TDS;
      }
    }
    // Disable HDR on DDS subs if not allowed
    else if (pItem->asubs_id == ddsSubs && !pCtx->includeHdrOnDds)
    {
      prc->ratExcludeMask |= SYS_SYS_MODE_MASK_HDR;
    }
  }

  return TRUE;
}


/*-------- policyman_update_rat_for_thermal_level --------*/
void policyman_update_rat_for_thermal_level(
  policyman_set_t *pItemSet
)
{  
  policyman_set_iterate(pItemSet, update_thermal_masks, NULL, NULL);
}


/*-------- policyman_set_rat_capability_concurrency_masks --------*/
void policyman_set_rat_capability_concurrency_masks(
  policyman_set_t *pItemSet
)
{
  rat_execute_ctx_t ctx;

  ctx.pItemSet = pItemSet;
  ctx.asubs_id = policyman_subs_get_dds_subs();
  ctx.pState   = policyman_state_get_state();
  
  // ensure itemset has RAT capability for all SUBS
  policyman_ensure_rat_capability_for_each_subs(&ctx);

  // get a context with info on HDR/WCDMA info in DDS SUBS
  policyman_set_iterate(pItemSet, determine_concurrency_parameters, &ctx, NULL);

  // update concurrency masks for all RAT capability items
  policyman_set_iterate(pItemSet, update_concurrency_masks, &ctx, NULL);

}


/*-----------------------------------------------------------------------------
  rat_capability action
-----------------------------------------------------------------------------*/


typedef struct
{
  POLICYMAN_ACTION_BASE;

  rat_capability_info_t rat_info;
} rat_capability_action_t;

#endif
#define INVALID_RAT_MASK  0xffffffff


/*-------- policyman_get_rat_capability --------*/
policyman_status_t policyman_get_rat_capability(
  policyman_item_t const  *pItem,
  uint32                  *pRatCfg
  )
{
  if (pItem == NULL || pRatCfg == NULL)
  {
    POLICYMAN_MSG_ERROR_0("get_rat_capability returned err POLICYMAN_STATUS_ERR_INVALID_ARGS");
    return POLICYMAN_STATUS_ERR_INVALID_ARGS;
  }

  if (policyman_cfgitem_id(pItem) != POLICYMAN_ITEM_RAT_CAPABILITY)
  {
    POLICYMAN_MSG_ERROR_0("get_rat_capability returned err POLICYMAN_STATUS_ERR_INVALID_ITEM_ID");
    return POLICYMAN_STATUS_ERR_INVALID_ITEM_ID;
  }

  *pRatCfg = policyman_rat_capability_get_effective_rats((policyman_item_t *)pItem);
  
  POLICYMAN_MSG_HIGH_2( "get_rat_capability subs %d returned 0x%04x",
                        pItem->asubs_id,
                        *pRatCfg );

  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_get_rat_capability_for_subs --------*/
policyman_status_t policyman_get_rat_capability_for_subs(
  sys_modem_as_id_e_type  subsId,
  uint32                 *pRatMask
)
{
  size_t                nItems;
  policyman_status_t    status  = POLICYMAN_STATUS_ERR;
  policyman_item_t     *pItem   = NULL;
  policyman_item_id_t   id[]    = { POLICYMAN_ITEM_RAT_CAPABILITY };

#ifdef FEATURE_DISABLE_DYNAMIC_POLICIES
  nItems = policyman_cfgitem_get_items( (policyman_item_id_t const *)&id, 
                                        ARR_SIZE(id), 
                                       (policyman_item_t const **)&pItem);
#else
  nItems = policyman_cfgitem_get_items_per_subs( (policyman_item_id_t const *)&id, 
                                                 ARR_SIZE(id), 
                                                 (policyman_item_t const **)&pItem, 
                                                 subsId);

#endif

  if (nItems == ARR_SIZE(id))
  {
    status = policyman_get_rat_capability(pItem, pRatMask);
  }

  REF_CNT_OBJ_RELEASE_IF(pItem);
  return status;
}

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- policyman_rat_capability_str_to_rat --------*/
uint32 policyman_rat_capability_str_to_rat(
  const char  *pRatName
  )
{
  rat_map *rat_item;
  uint32  mask = INVALID_RAT_MASK;

  rat_item = policyman_util_str_to_rat(pRatName);

  if (NULL != rat_item)
  {
    mask = rat_item->mask;
  }

  return mask;
}



/*-------- policyman_rat_config_parse_rats --------*/
boolean policyman_rat_config_parse_rats(
  char const  *pStr,
  uint32      *pMask
  )
{
  char    token[32];
  boolean succeeded = TRUE;
  uint32  mask;

  while (policyman_get_token(&pStr, token, sizeof(token)))
  {
    mask = policyman_rat_capability_str_to_rat(token);
    if (mask == INVALID_RAT_MASK)
    {
      POLICYMAN_UT_MSG("invalid RAT specified: %s", token, 0, 0);
      succeeded = FALSE;
      break;
    }
    *pMask |= mask;
  }

  return succeeded;
}


/*-------- policyman_rat_capability_read_info --------*/
policyman_status_t policyman_rat_capability_read_info(
  policyman_xml_element_t const *pElem,
  rat_capability_info_t         *pInfo
  )
{
  policyman_status_t status = POLICYMAN_STATUS_ERR_INVALID_ACTION;
  size_t            nChildren;
  size_t            i;

  pInfo->base = POLICYMAN_BASE_NONE;
  pInfo->includeMask = 0;
  pInfo->excludeMask = 0;

  if (!policyman_util_get_base(pElem, &pInfo->base))        { goto Done; }

  nChildren = policyman_xml_num_children(pElem);
  if (nChildren == 0)
  {
    status = POLICYMAN_STATUS_SUCCESS;
    goto Done;
  }

  for (i = 0 ; i < nChildren ; ++i)
  {
    policyman_xml_element_t const *pChild;
    char const                  *pStr;

    pChild = policyman_xml_get_child(pElem, i);
    if (pChild == NULL)
    {
      POLICYMAN_MSG_ERROR_1("Internal error - no element #%d", i);
      goto Done;
    }

    pStr = policyman_xml_get_text(pChild);
    if      (policyman_xml_tag_is(pChild, "include"))
    {
      if (!policyman_rat_config_parse_rats(pStr, &pInfo->includeMask)) { goto Done; }
    }
    else if (policyman_xml_tag_is(pChild, "exclude"))
    {
      if (!policyman_rat_config_parse_rats(pStr, &pInfo->excludeMask)) { goto Done; }
    }
    else
    {
      goto Done;
    }
  }

  status = POLICYMAN_STATUS_SUCCESS;

Done:
  return status;
}
#endif

/*-------- policyman_get_configured_rats --------*/
static uint32 policyman_get_configured_rats(
  sys_modem_as_id_e_type asubs_id
)
{
  policyman_item_t const  *pItem;
  uint32                  mask;

  pItem = policyman_cfgitem_get_item(
              POLICYMAN_ITEM_DEVICE_CONFIGURATION,
              SYS_MODEM_AS_ID_1
              );

  if (pItem == NULL)
  {
    POLICYMAN_ERR_FATAL("No device configuration present!!  Make sure that either a HW MBN or a /policyman/device_config.xml file is loaded on the device", 0, 0, 0);
  }

  mask = device_config_get_current_config((device_configuration_t const *)pItem, asubs_id);
  policyman_item_release(pItem);

  return mask;
}


/*-------- policyman_rat_capability_get_max_config --------*/
uint32 policyman_rat_capability_get_max_config(
  policy_execute_ctx_t *prcCtx
  )
{
  device_configuration_t **ppItem = NULL;
  uint32                 config_mask = 0;

  if (NULL != prcCtx->pItemSet)
  {
    ppItem = policyman_cfgitem_find_item_in_itemset( prcCtx->pItemSet,
                                                     POLICYMAN_ITEM_DEVICE_CONFIGURATION,
                                                     SYS_MODEM_AS_ID_1 );
  }

  if (NULL != ppItem)
  {
    /* XML specifies for which subs we need RAT info */
    config_mask = device_config_get_current_config(*ppItem, (size_t)prcCtx->asubs_id);
  }
  else
  {
    config_mask = policyman_get_configured_rats(prcCtx->asubs_id);
  }

  return config_mask;
}

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- execute_rat_capability_update --------*/
STATIC boolean execute_rat_capability_update(
  policyman_action_t const *pAction,
  void                     *pCtx,
  sys_modem_as_id_e_type    subsId
)
{
  policy_execute_ctx_t   *prcCtx    = (policy_execute_ctx_t *) pCtx;
  policyman_item_t const *pOldRc;
  policyman_item_t       *pNewRc    = NULL;  
  uint32                  mask;

  pOldRc = policyman_cfgitem_get_item(POLICYMAN_ITEM_RAT_CAPABILITY, subsId);

  mask = ((rat_capability_t *)pOldRc)->ratMask;
  prcCtx->asubs_id = subsId;

  pNewRc  = policyman_rat_capability_item_new(prcCtx, mask);
  policyman_set_replace(prcCtx->pItemSet, &pNewRc);

  ref_cnt_obj_release(pNewRc);
  policyman_item_release(pOldRc);

  return TRUE;
}


/*-------- policyman_rat_capability_if_process --------*/
STATIC void policyman_rat_capability_if_process(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  policyman_action_t const *pAct = (policyman_action_t const *)pAction;

  policyman_execute_action(pAct, pCtx, execute_rat_capability_update);
}


/*-------- policyman_rat_capability_evaluate --------*/
uint32 policyman_rat_capability_evaluate(
  rat_capability_info_t *pInfo,
  policy_execute_ctx_t  *prcCtx
)
{
  uint32                        mask = 0;
  policyman_rf_device_info_t    deviceInfo;

  switch (pInfo->base)
  {
    case POLICYMAN_BASE_NONE:
      mask = 0;
      break;

    case POLICYMAN_BASE_CONFIG:
      mask = policyman_rat_capability_get_max_config(prcCtx);
      break;

    case POLICYMAN_BASE_HARDWARE:
      policyman_state_get_rf_info(prcCtx->pState, 0, &deviceInfo);
      mask = deviceInfo.ratMask;
      break;

    default:
      POLICYMAN_MSG_ERROR_1("invalid base type: %d", pInfo->base);
      break;
  }

  mask |= pInfo->includeMask;
  mask &= ~(pInfo->excludeMask);

  return mask;
}


/*-------- execute_rat_capability --------*/
static boolean execute_rat_capability(
  policyman_action_t const *pAction,
  void                     *pCtx,
  sys_modem_as_id_e_type    subsId
)
{
  rat_capability_action_t *prcAction = (rat_capability_action_t *) pAction;
  policy_execute_ctx_t    *prcCtx    = (policy_execute_ctx_t *) pCtx;
  rat_capability_t        *prc;
  uint32                   mask;

  mask = policyman_rat_capability_evaluate(&prcAction->rat_info, prcCtx);
  prcCtx->asubs_id = subsId;

  /* if none rat capability is attempted, enable GSM */
  if (mask == SYS_SYS_MODE_MASK_NONE)
  {
    POLICYMAN_MSG_ERROR_0("<rat_capability> none attempted, action not taken");
    return TRUE;
  }

  prc = (rat_capability_t *)policyman_rat_capability_item_new(prcCtx, mask);
  policyman_set_replace(prcCtx->pItemSet, &prc);

  POLICYMAN_MSG_HIGH_2("action <rat_capability> set Base to 0x%04x for subs %d", prc->ratMask, subsId);

  ref_cnt_obj_release(prc);
  return TRUE;
}


/*-------- policyman_rat_capability_execute --------*/
static boolean policyman_rat_capability_execute(
  mre_action_t const  *pAction,
  void                *pCtx
  )
{
  policyman_action_t const *pAct = (policyman_action_t const *)pAction;

  return policyman_execute_action(pAct, pCtx, execute_rat_capability);
}


/*-------- policyman_rat_capability_if_execute --------*/
static boolean policyman_rat_capability_if_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  rat_capability_action_t *pratAction      = (rat_capability_action_t *)pAction;
  boolean                  succeeded       = TRUE;
  boolean                  item_is_present;


  item_is_present = policyman_cfgitem_already_present( POLICYMAN_ITEM_RAT_CAPABILITY,
                                                       ACTION_SUBS(pratAction),
                                                       pratAction->pmSubsId );
  if (item_is_present)
  {
    POLICYMAN_UT_MSG_0("<rat_capability_if> item present, masking with device config");
    policyman_rat_capability_if_process(pAction, pCtx);
  }
  else
  {
    succeeded = policyman_rat_capability_execute(pAction, pCtx);
  }

  return succeeded;
}


/*-------- policyman_rat_capability_create_new_action --------*/
mre_status_t policyman_rat_capability_create_new_action(
  policyman_xml_element_t const  *pElem,
  policyman_policy_t             *pPolicy,
  mre_action_t                  **ppAction,
  pfn_execute_t                   executefn
)
{
  mre_status_t             status  = MRE_STATUS_ERR_INVALID_ACTION;
  rat_capability_action_t *pAction = NULL;
  policyman_subs_t         pm_subs;

  pAction = (rat_capability_action_t *) policyman_mem_alloc(sizeof(rat_capability_action_t));

  ref_cnt_obj_init(pAction, policyman_action_dtor);
  pAction->execute = executefn;

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);
  if (MRE_FAILED(status))
  {
    goto Done;
  }
  pAction->pmSubsId = pm_subs;

  status = policyman_rat_capability_read_info(pElem, &pAction->rat_info);

Done:
  if (MRE_SUCCEEDED(status))
  {
    *ppAction = (mre_action_t *) pAction;
  }
  else
  {
    POLICYMAN_UT_MSG("<rat_capability>: invalid XML", 0, 0, 0);
    REF_CNT_OBJ_RELEASE_IF(pAction);
  }

  return status;
}


/*-------- policyman_rat_capability_action_new --------*/
mre_status_t policyman_rat_capability_action_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  )
{
  return policyman_rat_capability_create_new_action(
                pElem, (policyman_policy_t *) pPolicy, ppAction,
                policyman_rat_capability_execute);
}

/*-------- policyman_rat_capability_if_action_new --------*/
mre_status_t policyman_rat_capability_if_action_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  )
{
 return policyman_rat_capability_create_new_action(
          pElem, (policyman_policy_t *) pPolicy, ppAction,
          policyman_rat_capability_if_execute);
}

/*-------- policyman_rat_capability_full_rat_mode --------*/
void policyman_rat_capability_full_rat_mode(
  policyman_set_t       *pItemSet,
  policyman_svc_mode_t   svc_mode,
  sys_modem_as_id_e_type subsId
  )
{
  policyman_svc_mode_item_t *pSvcItem;
  policyman_item_t          *pRatItem;
  policyman_item_t          *pRfBandsItem;
  policyman_state_t         *pState = policyman_state_get_state();
  policy_execute_ctx_t       ctx;

  policyman_rf_device_info_t    rfInfo;

  ctx.pState   = pState;
  ctx.asubs_id = subsId;
  ctx.pItemSet = pItemSet;

  POLICYMAN_MSG_HIGH_0("Moving to full RAT mode to find service");

  // Add SVC mode to set - FULL RAT mode
  pSvcItem = policyman_svc_mode_item_new(svc_mode, subsId);
  policyman_set_replace(pItemSet, &pSvcItem);
  ref_cnt_obj_release(pSvcItem);

  // Get RF capabilities
  (void) policyman_state_get_rf_info(pState, 0, &rfInfo);

  // Add full rat capability to set
  pRatItem = policyman_rat_capability_item_new(&ctx, rfInfo.ratMask);
  policyman_set_replace(pItemSet, &pRatItem);
  ref_cnt_obj_release(pRatItem);

  // Add all supported bands to set
  pRfBandsItem = policyman_rf_bands_item_new(rfInfo.gwBands, rfInfo.lteBands, rfInfo.tdsBands);
  policyman_set_replace(pItemSet, &pRfBandsItem);
  ref_cnt_obj_release(pRfBandsItem);
}


/*-------- policyman_retrieve_rats_bands --------*/
boolean policyman_retrieve_rats_bands(
  sys_modem_as_id_e_type   subsId,
  uint32                  *pRatMask,
  sys_band_mask_type      *pGWBand,
  sys_lte_band_mask_e_type  *pLTEBand,
  sys_band_mask_type      *pTDSBand
)
{
  mre_efs_status_t          efsStatus      = MRE_EFS_STATUS_ERROR;
  rat_capability_efs_t     *rat_cap_buffer = NULL;
  size_t                    rat_file_size  = 0;

  efsStatus = mre_efs_get_file( POLICYMAN_RAT_MASK_NV_PATH,
                                (void **)&rat_cap_buffer,
                                &rat_file_size,
                                subsId );

  if (MRE_EFS_STATUS_SUCCESS == efsStatus)
  {
    *pRatMask = rat_cap_buffer->rat_mask;
    *pGWBand  = rat_cap_buffer->gw_band_mask;
    *pLTEBand = rat_cap_buffer->lte_band_mask;
    *pTDSBand = rat_cap_buffer->tds_band_mask;

    POLICYMAN_MSG_HIGH_0("---------- RATs and bands from EFS");
    POLICYMAN_MSG_HIGH_1("  RAT mask: 0x%04x", *pRatMask);
    policyman_rf_print_bands(*pGWBand, *pLTEBand, *pTDSBand);
    POLICYMAN_MSG_SEPARATOR(POLICYMAN_SEPARATOR_HYPHEN);
  }


  POLICYMAN_MEM_FREE_IF(rat_cap_buffer);

  POLICYMAN_MSG_HIGH_3( "policyman_retrieve_rats_bands(subs %d): status %d, filesize %d",
                        subsId,
                        efsStatus,
                        rat_file_size );

  return (efsStatus == MRE_EFS_STATUS_SUCCESS);
}

/*-------- policyman_rats_bands_persist --------*/
STATIC boolean policyman_rats_bands_persist(
  sys_modem_as_id_e_type     subsId,
  uint32                    *pRatMask,
  sys_band_mask_type        *pGWBand,
  sys_lte_band_mask_e_type  *pLTEBand,
  sys_band_mask_type        *pTDSBand
)
{
  boolean                   write_status;
  rat_capability_efs_t      rat_cap_buffer;

  rat_cap_buffer.rat_mask      = *pRatMask;
  rat_cap_buffer.gw_band_mask  = *pGWBand;
  rat_cap_buffer.lte_band_mask = *pLTEBand;
  rat_cap_buffer.tds_band_mask = *pTDSBand;

  write_status = policyman_cfgitem_persist_item( POLICYMAN_RAT_MASK_NV_PATH,
                                                 (void *)&rat_cap_buffer,
                                                 sizeof(rat_cap_buffer),
                                                 subsId );

  POLICYMAN_MSG_HIGH_1("Wrote RAT mask to efs:  base 0x%04x", *pRatMask);
  POLICYMAN_MSG_HIGH_3( "policyman_rats_bands_persist(sub %d): status %d, filesize %d",
                        subsId,
                        write_status,
                        sizeof(rat_cap_buffer) );

  return write_status;
}

/*-------- policyman_retrieve_rats_bands_per_subs --------*/
policyman_status_t policyman_retrieve_rats_bands_per_subs(
  size_t subs,
  uint32 *pRatMask,
  sys_band_mask_type *pGwBands,
  sys_lte_band_mask_e_type *pLteBands,
  sys_band_mask_type *pTdsBands
)
{
  size_t                nItems;
  policyman_status_t    status   = POLICYMAN_STATUS_ERR;
  policyman_item_t     *pRatcfg  = NULL;
  policyman_item_id_t   ratId    = POLICYMAN_ITEM_RAT_CAPABILITY;
  policyman_item_t     *pBandcfg = NULL;
  policyman_item_id_t   rfId     = POLICYMAN_ITEM_RF_BANDS;

  nItems = policyman_cfgitem_get_items_per_subs( &ratId,
                                                 1 ,
                                                 (policyman_item_t const **)&pRatcfg,
                                                 subs
                                               );
  if (nItems != 1)
  {
    POLICYMAN_MSG_ERROR_1("subs %d: policyman_get_items failed for RAT capability", subs);
    goto Done;
  }

  *pRatMask = policyman_rat_capability_get_base_rats((policyman_item_t const *)pRatcfg);

  nItems = policyman_cfgitem_get_items_per_subs( &rfId,
                                                 1 ,
                                                 (policyman_item_t const **)&pBandcfg,
                                                 subs
                                               );
  if (nItems != 1)
  {
    POLICYMAN_MSG_ERROR_1("subs %d: policyman_get_items failed for Band capability", subs);
    goto Done;
  }
  status = policyman_get_rf_bands((policyman_item_t const *)pBandcfg, pGwBands, pLteBands, pTdsBands);

Done:

  REF_CNT_OBJ_RELEASE_IF(pRatcfg);
  REF_CNT_OBJ_RELEASE_IF(pBandcfg);
  return status;
}

/*-------- policyman_update_rats_bands_to_efs --------*/
void policyman_update_rats_bands_to_efs(
  uint8 subsMask
)
{
  policyman_status_t     status;
  boolean                write_status;
  uint32                 ratMask;
  sys_band_mask_type     gwBand;
  sys_lte_band_mask_e_type  lteBand;
  sys_band_mask_type     tdsBand;
  size_t                 subs;
  size_t                 nSim      = 0;

  (void)policyman_get_current_num_sim(&nSim);

  for (subs = 0; subs < nSim; subs++)
  {
    status = policyman_retrieve_rats_bands_per_subs( subs,
                                                     &ratMask,
                                                     &gwBand,
                                                     &lteBand,
                                                     &tdsBand );
    if (!POLICYMAN_SUCCEEDED(status))
    {
      POLICYMAN_MSG_ERROR_0("Failed to retrieve rat/band information, skipping EFS write");
      continue;
    }

    /* Write data to EFS file
    */
    write_status = policyman_rats_bands_persist( (sys_modem_as_id_e_type)subs,
                                                 &ratMask,
                                                 &gwBand,
                                                 &lteBand,
                                                 &tdsBand );


    POLICYMAN_MSG_HIGH_2( "update_rats_bands_to_efs(subs %d): write_status %d",
                          subs,
                          write_status );
  }
}

/*-------- policyman_update_rats_bands_to_efs_per_tech --------*/
void policyman_update_rats_bands_to_efs_per_tech(
  uint32  rat_include
)
{
  policyman_status_t     status;
  boolean                write_status;
  uint32                 ratMask;
  sys_band_mask_type     gwBand;
  sys_lte_band_mask_e_type  lteBand;
  sys_band_mask_type     tdsBand;
  sys_band_mask_type     hwGwBand;
  sys_band_mask_type     hwTdsBand;
  sys_modem_as_id_e_type subs;

  subs = SYS_MODEM_AS_ID_1;

  policyman_get_hardware_bands_all_techs( &hwGwBand, NULL, &hwTdsBand );

  status = policyman_retrieve_rats_bands_per_subs( subs,
                                                   &ratMask,
                                                   &gwBand,
                                                   &lteBand,
                                                   &tdsBand );
  if (!POLICYMAN_SUCCEEDED(status))
  {
    POLICYMAN_MSG_ERROR_0("Failed to retrieve rat/band information, skipping EFS write");
    return;
  }

  if ( rat_include & SYS_SYS_MODE_MASK_WCDMA )
  {
    ratMask |= SYS_SYS_MODE_MASK_WCDMA;
    gwBand  |= hwGwBand & ALL_WCDMA_BANDS;
  }
  else
  {
    ratMask = ratMask & ~SYS_SYS_MODE_MASK_WCDMA;
    gwBand  = gwBand & ~ALL_WCDMA_BANDS;
  }

  if ( rat_include & SYS_SYS_MODE_MASK_TDS )
  {
    ratMask |= SYS_SYS_MODE_MASK_TDS;
    tdsBand = hwTdsBand;
  }
  else
  {
    ratMask = ratMask & ~SYS_SYS_MODE_MASK_TDS;
    tdsBand = 0;
  }

  /* Write data to EFS file
   */
  write_status = policyman_rats_bands_persist(  subs,
                                                &ratMask,
                                                &gwBand,
                                                &lteBand,
                                                &tdsBand );

  POLICYMAN_MSG_HIGH_2( "update_rats_bands_to_efs_per_tech(subs %d): write_status %d",
                        subs,
                        write_status );
}


/*-------- policyman_rat_capability_fill_subs_diag_log --------*/
void policyman_rat_capability_fill_subs_diag_log(
  policyman_item_t const  *pItem,
  LOG_PM_SUBS_INFO_type   *log_ptr
)
{
  rat_capability_t  *pRatCap = (rat_capability_t *)pItem;

  log_ptr->rat_mask = pRatCap->ratMask;
}
#endif
