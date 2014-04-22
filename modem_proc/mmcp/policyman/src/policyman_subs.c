/**
  @file policyman_subs.c

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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/src/policyman_subs.c#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/
#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES

#include "policyman_cfgitem.h"

#include "mre_efs.h"
#include "mre_set.h"
#include "policyman_cm.h"
#include "policyman_dbg.h"
#include "policyman_device_config.h"
#include "policyman_policy.h"
#include "policyman_rat_capability.h"
#include "policyman_rules.h"
#include "policyman_state.h"
#include "policyman_subs.h"
#include "policyman_task.h"
#include "policyman_uim.h"
#include "policyman_util.h"
#include "policyman_xml.h"

#include "sys.h"
#include <stringl/stringl.h>

/*=============================================================================
 Data Structure for Subs info
==============================================================================*/
struct policyman_subs_info_s
{
  size_t                  num_subs;               // Count of active subscriptions.
  uint8                   active_subs_mask;       // Mask of currently active subscriptions.
  sys_modem_as_id_e_type  ddsSubs;                // Current subscriptions which is DDS
  boolean                 ddsChangeisPermanent;   // DDS change is permanent
};

/*=============================================================================
  APIs for Subs information
=============================================================================*/
boolean policyman_subs_init(
  policyman_subs_info_t **ppInfo
)
{
  nv_stat_enum_type      nv_status;
  nv_item_type           nv_item;
  sys_modem_as_id_e_type ddsSubs = SYS_MODEM_AS_ID_1;

  *ppInfo = (policyman_subs_info_t *) policyman_mem_alloc(sizeof(policyman_subs_info_t));

  /* update subs state
   */
 (*ppInfo)->num_subs         = 0;
 (*ppInfo)->active_subs_mask = SYS_MODEM_AS_ID_NO_ACTIVE_MASK;

  /* Read DDS SUBS from context 0 and initialize */
  nv_status = mre_efs_get_nv_item( NV_DUAL_STANDBY_CFG_ITEMS_I, 
                                   &nv_item, 
                                   SYS_MODEM_AS_ID_1);

  if (nv_status == NV_DONE_S)
  {
    ddsSubs = (sys_modem_as_id_e_type)nv_item.dual_standby_cfg_items.dedicated_data_subs;
    POLICYMAN_MSG_HIGH_2("Read DDS NV: DDS subs %d", nv_status, ddsSubs);
  }
  else
  {
    POLICYMAN_MSG_ERROR_1("DDS NV read failed status %d, default DDS subs is AS_ID_1", nv_status);
  }

  (*ppInfo)->ddsSubs = ddsSubs;
#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
  (*ppInfo)->ddsChangeisPermanent = TRUE;
#endif

  return TRUE;
}

/*-------- policyman_subs_deinit --------*/
void policyman_subs_deinit(
  policyman_subs_info_t *pInfo
)
{
  POLICYMAN_MEM_FREE_IF(pInfo);
}

/*-------- policyman_get_num_subs --------*/
size_t
policyman_get_num_subs(
  void
  )
{
  policyman_subs_info_t *pSubsInfo = policyman_state_get_subs_info();

  return (pSubsInfo != NULL)? pSubsInfo->num_subs : 0;
}


/*-------- policyman_get_active_subs_mask --------*/
static uint8
policyman_get_active_subs_mask(
 void
 )
{
  policyman_subs_info_t *pSubsInfo = policyman_state_get_subs_info();

  return (pSubsInfo != NULL)? pSubsInfo->active_subs_mask : 0;
}

/*-------- policyman_subs_get_dds_subs --------*/
sys_modem_as_id_e_type policyman_subs_get_dds_subs(
  void
)
{
  policyman_subs_info_t *pSubsInfo = policyman_state_get_subs_info();

  return (pSubsInfo != NULL)? pSubsInfo->ddsSubs: SYS_MODEM_AS_ID_1;
}


/*=============================================================================
  Subs-related APIs
=============================================================================*/

/*-------- policyman_str_to_subs --------*/
policyman_subs_t
policyman_str_to_subs(
  char const  *pStr
  )
{
  typedef struct
  {
    const char        *pStr;
    policyman_subs_t  subs;
  } subs_map;

  static subs_map   map[] =
  {
    {"mmode",         SUBS_MMODE},
    {"non-mmode",     SUBS_NON_MMODE},
    {"all",           SUBS_ALL},
    {"this",          SUBS_THIS},
    {"other",         SUBS_OTHER},
  };

  policyman_subs_t  subs = SUBS_NONE;

  size_t  i;
  char    token[32];

  policyman_get_token(&pStr, token, sizeof(token));

  for (i = 0 ; i < ARR_SIZE(map) ; ++i)
  {
    if (strcasecmp(token, map[i].pStr) == 0)
    {
      subs = map[i].subs;
      break;
    }
  }

  if (subs == SUBS_NONE)
  {
    POLICYMAN_UT_MSG_1("Invalid value for 'subs' attribute of <service_status_in>: %s", pStr);
  }

  return subs;
}




/*=============================================================================
  Num Subs conditions
=============================================================================*/


/*-----------------------------------------------------------------------------
  Num Subs
-----------------------------------------------------------------------------*/

typedef struct
{
  POLICYMAN_CONDITION_BASE;

  size_t num_subs;
} policyman_num_subs_condition_t;

/*-------- policyman_condition_num_subs_evaluate --------*/
static boolean
policyman_condition_num_subs_evaluate(
  mre_condition_t const *pCondition,
  void                        *pCtx
  )
{
  policyman_subs_info_t          const  *pSubsInfo = policyman_state_get_subs_info();
  policyman_num_subs_condition_t const  *pCond = (policyman_num_subs_condition_t *) pCondition;
  boolean                               result;

  result = pCond->num_subs == pSubsInfo->num_subs;

  POLICYMAN_MSG_HIGH_2("condition <num_subs> with num_subs %d returns %d", pSubsInfo->num_subs, result);
  return result;
}

/*-------- policyman_condition_num_subs_new --------*/
mre_status_t
policyman_condition_num_subs_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  )
{
  mre_status_t                    status = MRE_STATUS_ERR_MALFORMED_XML;
  policyman_num_subs_condition_t  *pCondition;
  char const                      *pStr;  
  char                            numSubsStr[4];
  int                             numSubs;

  pStr = policyman_xml_get_text(pElem);
  if (pStr == NULL ||
      !policyman_get_token(&pStr, numSubsStr, sizeof(numSubsStr)))
  {
    POLICYMAN_UT_MSG("<num_subs>: must specify a number of subscriptions", 0, 0, 0);
    goto Done;
  }

  numSubs = atoi(numSubsStr);
  if (numSubs < 0)
  {
    POLICYMAN_UT_MSG("<num_subs>: invalid num_subs %s", numSubsStr, 0, 0);
    goto Done;
  }

  pCondition = (policyman_num_subs_condition_t *)
                  policyman_mem_alloc(sizeof(policyman_num_subs_condition_t));

  ref_cnt_obj_init(pCondition, policyman_condition_dtor);
  pCondition->isTrue = policyman_condition_num_subs_evaluate;
  pCondition->num_subs = numSubs;

  *ppCondition = (mre_condition_t *) pCondition;
  status = MRE_STATUS_SUCCESS;

Done:
  POLICYMAN_MSG_HIGH_1("policyman_condition_num_subs_new status %d", status);
  return status;
}


/*=============================================================================
===============================================================================

  Preference handling

===============================================================================
=============================================================================*/

typedef struct
{
  POLICYMAN_CMD_HDR;

  size_t     num_subs;
  uint8      active_subs_mask;
} policyman_cmd_report_subs_chg_t;


/*-------- policyman_subs_update_notification_hook --------*/
/**
@brief  Hook function for notification of the result of subs updates.

This creates another MSGR message (not sent by the MSGR) that is stored in the
command that initiated the notification.  The signal to unblock the thread
that sent the update is then set, which releases it and returns the message
to the caller of policyman_report_preference_change().
*/
STATIC void policyman_subs_update_notification_hook(
  void              *pContext,
  size_t            nItems,
  policyman_item_t  **ppItems
)
{
  policyman_cmd_t *pCmd;

  /*  Create a new MSGR message and store it in the command.
   */
  pCmd = (policyman_cmd_t *) pContext;
  pCmd->pMsg = policyman_cfgitem_notify_msg_msim_new(ppItems, nItems, NULL);

  /*  Unblock the thread that issued the command.
   */
  policyman_client_block_signal_set(pCmd->signal);
}


/*-------- policyman_subs_update_multimode_subs --------*/
void policyman_subs_update_multimode_subs(
  sys_modem_as_id_e_type  currentSubs,
  policyman_cmd_t         *pCmd
)
{
  policyman_set_t   *pItemSet = policyman_itemset_new();
  policyman_state_t *pState = policyman_state_get_state();
  
  policyman_cfgitem_hook_notification( policyman_subs_update_notification_hook,
                                       pCmd );

  /* Run a policy check on the multimode subs.
   */
  policyman_policy_run_policy_check(pState, currentSubs, pItemSet);

  policyman_cfgitem_update_items(pItemSet);
  policyman_cfgitem_hook_notification(NULL, NULL);
  REF_CNT_OBJ_RELEASE_IF(pItemSet);
}


/*-------- policyman_report_multimode_subs_execute --------*/
static void
policyman_report_multimode_subs_execute(
  policyman_cmd_t *pCmd
  )
{
  uint8                            active_subs_mask;
  size_t                           num_subs;
  policyman_cmd_report_subs_chg_t  *pSubsCmd = NULL;
  policyman_subs_info_t            *pInfo = policyman_state_get_subs_info();


  pSubsCmd = (policyman_cmd_report_subs_chg_t *) pCmd;
  active_subs_mask = pSubsCmd->active_subs_mask;
  num_subs = pSubsCmd->num_subs;
  
  if (pInfo->active_subs_mask == active_subs_mask &&
      pInfo->num_subs == num_subs)
  {
    return;
  }
  
  POLICYMAN_MSG_HIGH_2("executing subs change command with active_subs_mask = %d, num_subs %d",
                        active_subs_mask,
                        num_subs );
  POLICYMAN_MSG_HIGH_2("  previous values: active_subs_mask %d, num_subs %d",
                        pInfo->active_subs_mask,
                        pInfo->num_subs );
  
  pInfo->active_subs_mask = active_subs_mask;
  pInfo->num_subs = num_subs;

  policyman_subs_update_multimode_subs(SYS_MODEM_AS_ID_1, pCmd);
}

/*-------- policyman_report_subs_have_changed --------*/
static boolean
policyman_report_subs_have_changed(
  uint8  active_subs_mask,
  size_t num_subs
  )
{
  policyman_subs_info_t *pInfo = policyman_state_get_subs_info();

  return    pInfo->num_subs != num_subs
         || pInfo->active_subs_mask != active_subs_mask;
}  

/*-------- policyman_block_for_multimode_subs --------*/
STATIC boolean policyman_block_for_multimode_subs(
  uint8  active_subs_mask,
  size_t num_subs
  )
{

  /*  Block the caller of policyman_report_preference_change() if:
   *    - We're not initializing
   *    - and there are changes in preferences that we care about
   */
  return    !policyman_is_initializing()
         && policyman_report_subs_have_changed(active_subs_mask, num_subs);
}

/*-------- policyman_count_num_subs --------*/
size_t
policyman_count_num_subs(
  uint8 active_subs_mask
  )
{
  size_t num_subs = 0;

  if (active_subs_mask != SYS_MODEM_AS_ID_NO_CHANGE_MASK)
  {
    while (active_subs_mask>0)
    {
      num_subs += active_subs_mask & 1;
      active_subs_mask >>= 1; 
    }
  }

  return num_subs;
}

/*-------- policyman_report_preference_change --------*/
msgr_hdr_s *
policyman_report_multimode_subs(
  uint8 active_subs_mask
  )
{
  policyman_cmd_report_subs_chg_t *pCmd;
  msgr_hdr_s                      *pMsg = NULL;
  size_t                          num_subs;

  num_subs = policyman_count_num_subs(active_subs_mask);
  
  /*  If we aren't going to block to process this request, just return.
   */
  if (!policyman_block_for_multimode_subs(active_subs_mask, num_subs))
  {
    goto Done;
  }

  /*  If we should block the calling thread to do this update, get a signal on
   *  which to block for command completion, and put it in the command.
   *  Queue the command and wait for completion.
   */
  pCmd = (policyman_cmd_report_subs_chg_t *) policyman_cmd_new( sizeof(policyman_cmd_report_subs_chg_t),
                                                policyman_report_multimode_subs_execute,
                                                NULL,
                                                                NULL );
  pCmd->active_subs_mask = active_subs_mask;
  pCmd->num_subs         = num_subs;
  pCmd->pMsg = NULL;

  POLICYMAN_MSG_HIGH_0("blocking caller of policyman_report_multimode_subs()");
  pCmd->signal = policyman_client_block_sig_get();
  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
  policyman_client_block_signal_wait(pCmd->signal);

  /*  Get any message to be returned and then release the command.
   */
  pMsg = pCmd->pMsg;
  ref_cnt_obj_release(pCmd);

Done:
  POLICYMAN_MSG_HIGH_0("returning to caller of policyman_report_multimode_subss()");
  return pMsg;
}

/*-------- policyman_is_cm_subs_active --------*/
boolean policyman_is_cm_subs_active(
 sys_modem_as_id_e_type subsId
)
{
  uint8 active_subs_mask = SYS_MODEM_AS_ID_NO_CHANGE_MASK;

  switch(subsId)
  {
    case SYS_MODEM_AS_ID_1:
      active_subs_mask = SYS_MODEM_AS_ID_1_MASK;
      break;

    case SYS_MODEM_AS_ID_2:
      active_subs_mask = SYS_MODEM_AS_ID_2_MASK;
      break;    

    case SYS_MODEM_AS_ID_3:
      active_subs_mask = SYS_MODEM_AS_ID_3_MASK;
      break;

    default:
     POLICYMAN_MSG_ERROR_1("subsId %d is not handled for checking active subs", subsId);
     break;
  }

  return ((policyman_get_active_subs_mask() & active_subs_mask) == active_subs_mask);
}


/*=============================================================================
===============================================================================

  DDS handling

===============================================================================
=============================================================================*/

/*-------- policyman_has_dds_changed --------*/
STATIC boolean policyman_has_dds_changed(
  sys_modem_as_id_e_type  ddsSubs
)
{
  policyman_subs_info_t *pInfo = policyman_state_get_subs_info();

  return pInfo->ddsSubs != ddsSubs;
}
#endif
