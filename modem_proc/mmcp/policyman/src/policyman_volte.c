/**
  @file policyman_volte.c

  @brief
*/

/*
    Copyright (c) 2014 QUALCOMM Technologies Incorporated.
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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/src/policyman_volte.c#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/
#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES

#include "ref_cnt_obj.h"
#include "sys.h"
#include "cm.h"
#include "mre.h"
#include "policyman_device_config.h"
#include "policyman_policy.h"
#include "policyman_rules.h"
#include "policyman_subs.h"
#include "policyman_util.h"
#include "policyman_volte.h"
#include "policyman_xml.h"

#include <stringl/stringl.h>

/*=============================================================================
  VoLTE enable APIs
=============================================================================*/

typedef struct
{
  POLICYMAN_ACTION_BASE;

  cm_volte_enable_e_type volte_state;
} volte_action_t;


/*-------- execute_volte --------*/
static boolean execute_volte(
  policyman_action_t const *pAction,
  void                     *pCtx,
  sys_modem_as_id_e_type    subsId 
)
{
  volte_action_t         *pVolteAction  = (volte_action_t *)pAction;
  cm_volte_enable_e_type  volte_enabled_state;

  volte_enabled_state = policyman_ph_get_volte_enabled_state(subsId);

  POLICYMAN_MSG_HIGH_3( "action <volte> (subs %d) state %d -> %d",
                        subsId,
                        volte_enabled_state,
                        pVolteAction->volte_state );

  if (volte_enabled_state != pVolteAction->volte_state)
  {
    policyman_ph_set_volte_enabled_state( subsId, pVolteAction->volte_state );
  }
  return TRUE;
}


/*-------- policyman_volte_action_execute --------*/
static boolean policyman_volte_action_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  policyman_action_t const *pAct = (policyman_action_t const *)pAction;
  return policyman_execute_action(pAct, pCtx, execute_volte);
}

/*-------- policyman_volte_create_new_action --------*/
mre_status_t policyman_volte_create_new_action(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction,
  pfn_execute_t             actionfn
)
{
  mre_status_t             status      = MRE_STATUS_ERR_INVALID_ACTION;
  volte_action_t          *pAction     = NULL;
  cm_volte_enable_e_type   volte_state = CM_VOLTE_DISABLED;
  policyman_subs_t         pm_subs;

  /* Handle the 'enabled' attribute
   * No enabled attribute is an error.
   * Invalid attribute value (not "true" or "false") is an error.
   */
  {
    char const               *pState;

    pState = policyman_xml_get_attribute(pElem, "enabled");

    if (pState == NULL)
    {
      POLICYMAN_MSG_ERROR_0("<volte>: action requires an 'enabled' attribute");
      goto Done;
    }

    if (strcmp(pState, "true") == 0)
    {
      volte_state = CM_VOLTE_ENABLED;
    }
    else if (strcmp(pState, "false") != 0)
    {
      POLICYMAN_MSG_ERROR_0("<volte>: invalid 'enabled' value for action");
      goto Done;
    }
  }

  /* Handle the 'subs' attribute
   * No subs attribute is OK -- use subscription from policy
   * Invalid attribute value (not "1" or "2") is OK -- use subscription from policy
   */
  {
    mre_status_t    status;

    status = policyman_util_get_subs(pElem, POLICY_SUBS(pPolicy), &pm_subs);

    if (MRE_STATUS_SUCCESS != status)
    {
      POLICYMAN_MSG_HIGH_0("<volte>: invalid 'subs' value for action");
      goto Done;
    }
  }

  /* Create the action
   */
  pAction = (volte_action_t *)policyman_mem_alloc(sizeof(*pAction));

  ref_cnt_obj_init(pAction, policyman_action_dtor);

  pAction->execute     = actionfn;
  pAction->volte_state = volte_state;
  pAction->pmSubsId    = pm_subs;

  *ppAction = (mre_action_t *)pAction;
  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}

/*-------- policyman_volte_action_new --------*/
mre_status_t policyman_volte_action_new(
  mre_xml_element_t const  *pElem,
  mre_policy_t             *pPolicy,
  mre_action_t            **ppAction
)
{
  return policyman_volte_create_new_action( pElem,
                                            (mre_policy_t *)pPolicy,
                                            ppAction,
                                            policyman_volte_action_execute );
}

#endif