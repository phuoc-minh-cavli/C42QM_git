/**
  @file policyman_svc_mode.c

  @brief
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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/src/policyman_svc_mode.c#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/

#include "mre_efs.h"

#include "policyman_svc_mode.h"
#include "policyman_device_config.h"
#include "policyman_diag.h"
#include "policyman_efs.h"
#include "policyman_set.h"
#include "policyman_subs.h"
#include "policyman_util.h"
#include "policyman_xml.h"

#include <stringl/stringl.h>

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
#define SVC_MODE_TAG  "svc_mode"

/*=============================================================================
  SVC mode APIs
=============================================================================*/

 typedef struct
 {
   POLICYMAN_ACTION_BASE;

   sys_modem_as_id_e_type subsId;
   policyman_svc_mode_t svc_mode;
 } svc_mode_action_t;
#endif

/*-------- policyman_svc_mode_item_new --------*/
policyman_svc_mode_item_t *
policyman_svc_mode_item_new(
  policyman_svc_mode_t    svcMode,
  sys_modem_as_id_e_type  subsId
  )
{
  policyman_svc_mode_item_t *pItem;

  if (!policyman_state_get_simlock_verified(subsId))
  {
    POLICYMAN_MSG_HIGH_2("simlock is not verified. actual requested Svc Mode %d for subs %d", svcMode, subsId);
    svcMode = POLICYMAN_SVC_MODE_LIMITED;
  }

  pItem = (policyman_svc_mode_item_t *) policyman_cfgitem_new_item(
                                            POLICYMAN_ITEM_SVC_MODE,
                                            sizeof(policyman_svc_mode_item_t),
                                            policyman_simple_dtor
                                            );
  pItem->mode = svcMode;
  pItem->asubs_id = subsId;

  return pItem;
}

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- policyman_retrieve_svc_mode --------*/
STATIC boolean policyman_retrieve_svc_mode(
  sys_modem_as_id_e_type   subsId,
  policyman_svc_mode_t    *pSvcMode
)
{
  mre_efs_status_t        status;
  policyman_svc_mode_t   *pMode    = NULL;
  size_t                  modeSize;
  boolean                 retval   = FALSE;

  /*  Try and read from the NV item for UE mode.
   */
  status = mre_efs_get_file( POLICYMAN_SVC_MODE_NV_PATH,
                             (void **)&pMode,
                             &modeSize,
                             subsId );

  /*  If we were able to read the NV and if the value is in the proper range,
   *  set the mode to that value.  Otherwise just return NORMAL mode.
   */
  if (   (status == MRE_EFS_STATUS_SUCCESS)
      && (modeSize == sizeof(policyman_svc_mode_t))
      && (pMode != NULL)
      && (*pMode >= POLICYMAN_SVC_MODE_LIMITED)
      && (*pMode <= POLICYMAN_SVC_MODE_ALL) )
  {
    *pSvcMode = *pMode;
    retval = TRUE;
  }

  POLICYMAN_MSG_HIGH_2( "retrieve_svc_mode: status %d, svc_mode %d",
                        status,
                        *pSvcMode );

  POLICYMAN_MEM_FREE_IF(pMode);

  return retval;
}

/*-------- policyman_svc_mode_get_default_from_efs --------*/
policyman_item_t *policyman_svc_mode_get_default_from_efs(
  sys_modem_as_id_e_type subs_id
)
{
  policyman_svc_mode_item_t *pSvcMode = NULL;
  policyman_svc_mode_t       svcMode;

  if (!policyman_retrieve_svc_mode(subs_id, &svcMode))
  {
    POLICYMAN_MSG_ERROR_1("subs %d: can't populate SVC mode Item from EFS", subs_id);
    goto Done;
  }

  pSvcMode = policyman_svc_mode_item_new(svcMode, subs_id);

Done:

  return (policyman_item_t *)pSvcMode;
}
#endif
/*-------- policyman_svc_mode_get_default --------*/
policyman_item_t *
policyman_svc_mode_get_default(
  sys_modem_as_id_e_type subsId
  )
{
  policyman_svc_mode_item_t  *pItem;

  pItem = policyman_svc_mode_item_new(POLICYMAN_SVC_MODE_FULL, subsId);

  POLICYMAN_MSG_HIGH_2("Default SVC mode for subs %d: %d", subsId, pItem->mode);

  return (policyman_item_t *) pItem;
}
#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- policyman_svc_mode_compare --------*/
boolean
policyman_svc_mode_compare(
  policyman_item_t  *pItem1,
  policyman_item_t  *pItem2
  )
{
  policyman_svc_mode_item_t  *pMode1 = (policyman_svc_mode_item_t *) pItem1;
  policyman_svc_mode_item_t  *pMode2 = (policyman_svc_mode_item_t *) pItem2;

  return pMode1 != NULL && pMode2 != NULL && pMode1->mode == pMode2->mode;
}
#endif

/*-------- policyman_get_svc_mode --------*/
policyman_status_t
policyman_get_svc_mode(
  policyman_item_t const  *pItem,
  policyman_svc_mode_t    *pSvcMode
  )
{
  policyman_svc_mode_item_t  *pMode = (policyman_svc_mode_item_t *) pItem;

  if (pMode == NULL || pSvcMode == NULL)
  {
    POLICYMAN_MSG_ERROR_0("get_svc_mode returned err POLICYMAN_STATUS_ERR_INVALID_ARGS");
    return POLICYMAN_STATUS_ERR_INVALID_ARGS;
  }
  if (policyman_cfgitem_id(pItem) != POLICYMAN_ITEM_SVC_MODE)
  {
    POLICYMAN_MSG_ERROR_0("get_svc_mode returned err POLICYMAN_STATUS_ERR_INVALID_ITEM_ID");
    return POLICYMAN_STATUS_ERR_INVALID_ITEM_ID;
  }

  *pSvcMode = pMode->mode;

  POLICYMAN_MSG_HIGH_1("get_svc_mode returned svc_mode %d", pMode->mode);

  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_ue_mode_display --------*/
void policyman_svc_mode_display
(
  policyman_item_t  *pItem
)
{
  policyman_svc_mode_item_t  *pMode = (policyman_svc_mode_item_t *) pItem;

  POLICYMAN_MSG_HIGH_2("SVC mode for subs %d: %d", pMode->asubs_id, pMode->mode);
}

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- policyman_svc_mode_update_to_efs --------*/
void policyman_svc_mode_update_to_efs(
  policyman_item_t const *pItem
)
{
  policyman_svc_mode_item_t *pSvcMode    = (policyman_svc_mode_item_t *)pItem;
  boolean                    writeStatus;
  size_t                     nSim        = 0;

  /* Don't update UE mode into EFS for the SUBS which is not configured
   */
  (void)policyman_get_current_num_sim(&nSim);

  if ((size_t)pItem->asubs_id >= nSim)
  {
    POLICYMAN_MSG_MED_1("subs %d not configured: EFS not updated for SVC mode", pItem->asubs_id);
    return;
  }

  writeStatus = policyman_cfgitem_persist_item( POLICYMAN_SVC_MODE_NV_PATH,
                                                &pSvcMode->mode,
                                                sizeof(pSvcMode->mode),
                                                pItem->asubs_id );

  POLICYMAN_MSG_HIGH_3( "subs %d: svc_mode %d write to efs status %d",
                        pItem->asubs_id,
                        pSvcMode->mode,
                        writeStatus );
}

/*-------- policyman_str_to_svc_mode --------*/
static boolean
policyman_str_to_svc_mode(
  const char            *pStr,
  policyman_svc_mode_t  *pSvcMode
  )
{
  typedef struct
  {
    const char            *pStr;
    policyman_svc_mode_t  svcMode;
  } mask_map;

  static mask_map map[] =
  {
    {"FULL",      POLICYMAN_SVC_MODE_FULL},
    {"CAMP_ONLY", POLICYMAN_SVC_MODE_CAMP_ONLY},
    {"LIMITED",   POLICYMAN_SVC_MODE_LIMITED},
    {"ALL",       POLICYMAN_SVC_MODE_ALL},
  };

  size_t  i;
  char    token[32];
  boolean succeeded = FALSE;


  *pSvcMode = POLICYMAN_SVC_MODE_FULL;

  policyman_get_token(&pStr, token, sizeof(token));

  for (i = 0 ; i < ARR_SIZE(map) ; ++i)
  {
    if (strcasecmp(token, map[i].pStr) == 0)
    {
      *pSvcMode = map[i].svcMode;
      succeeded = TRUE;
      break;
    }
  }

  return succeeded;
}


/*-------- policyman_svc_mode_read_info --------*/
policyman_status_t policyman_svc_mode_read_info(
  policyman_xml_element_t const *pElem,
  policyman_svc_mode_t         *pSvcmode
  )
{
  policyman_status_t status = POLICYMAN_STATUS_ERR_INVALID_ACTION;
  char const         *pStr;

  *pSvcmode = POLICYMAN_SVC_MODE_FULL;

  if (!policyman_xml_tag_is(pElem, SVC_MODE_TAG))     { goto Done; }
  pStr = policyman_xml_get_text(pElem);
  policyman_str_to_svc_mode(pStr, pSvcmode);
  status = POLICYMAN_STATUS_SUCCESS;

  Done:
    return status;
}


/*-------- execute_svc_mode --------*/
static boolean execute_svc_mode(
  policyman_action_t const *pAction,
  void                     *pCtx,
  sys_modem_as_id_e_type    subsId
)
{
  svc_mode_action_t         *psvcAction = (svc_mode_action_t *) pAction;
  policy_execute_ctx_t      *psvcCtx = (policy_execute_ctx_t *)pCtx;
  policyman_svc_mode_item_t *pSm = policyman_svc_mode_item_new(psvcAction->svc_mode, subsId);

  policyman_set_replace(psvcCtx->pItemSet, &pSm);
  ref_cnt_obj_release(pSm);

  POLICYMAN_MSG_HIGH_2("action <svc_mode> set mode to %d for subs %d", pSm->mode, subsId);
  return TRUE;
}


/*-------- policyman_svc_mode_execute --------*/
static boolean policyman_svc_mode_execute(
  mre_action_t const  *pAction,
  void                      *pCtx
  )
{
  policyman_action_t const *pAct = (policyman_action_t const *)pAction;
  return policyman_execute_action(pAct, pCtx, execute_svc_mode);
}


/*-------- policyman_svc_mode_action_new --------*/
mre_status_t policyman_svc_mode_action_new
(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
)
{
  mre_status_t            status = MRE_STATUS_ERR_INVALID_ACTION;
  svc_mode_action_t       *pAction = NULL;
  policyman_svc_mode_t    mode;
  policyman_subs_t        pm_subs;

  status = policyman_svc_mode_read_info(pElem, &mode);
  if (MRE_FAILED(status))
  {
    POLICYMAN_UT_MSG("<svc_mode>: invalid service mode", 0, 0, 0);
    goto Done;
  }

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);
  if (MRE_FAILED(status))
  {
    goto Done;
  }

  pAction = (svc_mode_action_t *)policyman_mem_alloc(sizeof(svc_mode_action_t));
  ref_cnt_obj_init(pAction, policyman_action_dtor);
  pAction->execute = policyman_svc_mode_execute;
  pAction->svc_mode = mode;
  pAction->pmSubsId = pm_subs;

  *ppAction = (mre_action_t *) pAction;

Done:
  return status;
}


typedef struct
{
  POLICYMAN_CONDITION_BASE;

  policyman_svc_mode_t svc_mode;
} policyman_svc_mode_condition_t;

/*-------- policyman_svc_mode_fill_subs_diag_log --------*/
void policyman_svc_mode_fill_subs_diag_log(
  policyman_item_t const  *pItem,
  LOG_PM_SUBS_INFO_type   *log_ptr
)
{
  policyman_svc_mode_item_t     *pSvcMode = (policyman_svc_mode_item_t *)pItem;

  log_ptr->svc_mode = (uint8)pSvcMode->mode;
}
#endif