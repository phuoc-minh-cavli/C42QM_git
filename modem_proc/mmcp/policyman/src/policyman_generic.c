/**
  @file policy_generic.c

  @brief  Generic policy.  This policy is entirely rules-based, with the
          addition of OOS handling.
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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/src/policyman_generic.c#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/
#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
#include "policyman_generic.h"
#include "policyman_device_config.h"
#include "policyman_dbg.h"
#include "policyman_plmn.h"
#include "policyman_rat_capability.h"
#include "policyman_serving_system.h"
#include "policyman_set.h"
#include "policyman_state.h"
#include "policyman_timer.h"
#include "policyman_util.h"

#include "sys.h"
#include <stringl/stringl.h>



typedef struct
{
  POLICYMAN_POLICY_BASE;

  /*  Timers used by Generic policy
   */
  policyman_timer_t   *pOosTimer;
} policyman_policy_generic_t;


#define POLICY_TO_GENERIC_POLICY(p) ((policyman_policy_generic_t *) (p))

#define GENERIC_OOS_TIMER    1



/*=============================================================================
  Generic policy APIs
=============================================================================*/


/*-------- policy_generic_dtor --------*/
static void
policy_generic_dtor(
  void  *pPolicy
  )
{
  policyman_policy_generic_t *pMe = POLICY_TO_GENERIC_POLICY(pPolicy);

  REF_CNT_OBJ_RELEASE_IF(pMe->pOosTimer);
  policyman_policy_dtor(pPolicy);
}


/*-------- policy_generic_new_instance --------*/
policyman_policy_t  *
policy_generic_new_instance(
  char const  *pName
  )
{
  policyman_policy_generic_t  *pPolicy = NULL;
  
  if (
          strcmp(pName, "generic") == 0
      ||  strcmp(pName, "rat capability") == 0
      ||  strcmp(pName, "device configuration") == 0
     )
  {
    pPolicy = policyman_mem_alloc(sizeof(policyman_policy_generic_t));
    ref_cnt_obj_init(pPolicy, policy_generic_dtor);

    pPolicy->pName = "generic";
    pPolicy->schema_ver = 2;
    pPolicy->id = POLICYMAN_POLICY_ID_GENERIC;
  }

  return (policyman_policy_t *) pPolicy;
}


/*-------- policy_generic_init_status --------*/
static void
policy_generic_init_status(
  policyman_policy_t  *pPolicy,
  policyman_status_t  status
  )
{
  switch (status)
  {
    case POLICYMAN_STATUS_SUCCESS:
      POLICYMAN_MSG_SEPARATOR(POLICYMAN_SEPARATOR_STAR);
      POLICYMAN_MSG_HIGH_0("Generic policy: successfully initialized");
      POLICYMAN_MSG_SEPARATOR(POLICYMAN_SEPARATOR_STAR);
      break;

    case POLICYMAN_STATUS_ERR_CONFIG_FAILURE:
      POLICYMAN_MSG_HIGH_0("Generic policy: configuration failed");
      break;

    case POLICYMAN_STATUS_ERR_POLICY_NOT_ENABLED:
      POLICYMAN_MSG_HIGH_0("Generic policy: policy disabled in XML");
      break;

    case POLICYMAN_STATUS_ERR_INVALID_VERSION:
      POLICYMAN_MSG_HIGH_0("Generic policy: version mismatch between XML and policy");
      break;

    case POLICYMAN_STATUS_ERR_MALFORMED_XML:
      POLICYMAN_MSG_HIGH_0("Generic policy: cannot parse XML file");
      break;

    default:
      POLICYMAN_MSG_HIGH_1("Generic policy: unknown status %d", status);
      break;
  }
}


/*-------- policy_generic_init_policy --------*/
static void
policy_generic_init_policy(
  policyman_policy_generic_t  *pMe
  )
{
  pMe->pOosTimer      = NULL;
}

/*-------- policy_generic_configure --------*/
static policyman_status_t
policy_generic_configure(
  policyman_policy_t   *pPolicy,
  policy_execute_ctx_t *pCtx
  )
{
  policyman_policy_generic_t  *pMe = POLICY_TO_GENERIC_POLICY(pPolicy);
  policyman_timer_t           *pTimer;

  POLICYMAN_MSG_HIGH_0("Configuring Generic policy");

  policy_generic_init_policy(pMe);

  /*---------------------------------------------------------------------------
    Find and initialize the OOS timer.  If not present that's fine - we just
    won't do anything on OOS.
  ---------------------------------------------------------------------------*/
  pTimer = (policyman_timer_t *) mre_named_object_find( "oos",
                                                         POLICYMAN_NAMED_TIMER,
                                                         (mre_policy_t *) pPolicy );
  if (pTimer == NULL)
  {
    POLICYMAN_MSG_HIGH_0("OOS timer not defined in Generic policy");
  }
  else
  {
    pMe->pOosTimer = pTimer;
    policyman_timer_set_subs(pTimer, SYS_MODEM_AS_ID_1);
    ref_cnt_obj_add_ref(pTimer);
    POLICYMAN_MSG_HIGH_2("OOS timer (id %d) defined with interval %d seconds in Generic policy",
                          GENERIC_OOS_TIMER, 
                          policyman_timer_get_interval(pTimer) );
  }

  /* detect if we need optimization to enter Full RAT before OOS timer expiry
   */
  policyman_policy_check_for_immediate_full_rat((policyman_policy_t *) pMe);

  POLICYMAN_MSG_HIGH_0("GENERIC: policy configured");
  return POLICYMAN_STATUS_SUCCESS;
}


static boolean
policy_generic_execute(
  policyman_policy_t   *pPolicy,
  policy_execute_ctx_t *pCtx
  )
{
  policyman_policy_generic_t *pMe      = POLICY_TO_GENERIC_POLICY(pPolicy);
  policyman_timer_t          *pTimer   = pMe->pOosTimer;
  policyman_svc_mode_t        svcMode  = POLICYMAN_SVC_MODE_LIMITED;

  if (policyman_policy_is_suspended())
  {
    POLICYMAN_MSG_ERROR_0("policy is suspended, not running execute method");
    return TRUE;
  }

  if (policyman_timer_is_expired(pTimer))
  {
    policyman_timer_set_handled(pTimer);
 
    /* If non-mmode SUBS has service then don't enforce Full RAT, 
         call for rule execution.
       */
    if (policyman_msim_non_multimode_subs_in_service())
    {
      POLICYMAN_MSG_HIGH_0("non multimode subs has service, executing policy instead of entering full RAT mode");
      policyman_policy_execute(pPolicy, pCtx);
      policyman_timer_start(pTimer);
      goto Done;
    }

    policyman_rat_capability_full_rat_mode(pCtx->pItemSet, svcMode, POLICY_SUBS(pPolicy));
    policyman_timer_start(pTimer);
  }

Done:
  return TRUE;
}


/*-------- policy_generic_notify_service --------*/
static void
policy_generic_notify_service(
  policyman_policy_t   *pPolicy,
  policy_execute_ctx_t *pCtx
  )
{
  policyman_policy_generic_t  *pMe       = POLICY_TO_GENERIC_POLICY(pPolicy);
  sys_oprt_mode_e_type         oprt_mode = policyman_ph_get_oprt_mode(POLICY_SUBS(pPolicy));

    /*  Start or stop the OOS timer depending on whether we have service.
         optimization to enter Full RAT upon PWR_SAVE is handled in rule evaluation.
   */
  policyman_timer_set_subs(pMe->pOosTimer, pPolicy->subs);
	
  if (  !pCtx->haveService
     && oprt_mode == SYS_OPRT_MODE_ONLINE 
     )
  {
    policyman_timer_start(pMe->pOosTimer);
    POLICYMAN_MSG_HIGH_0("calling for rule execution from generic_notify_service");
    policyman_policy_execute(pPolicy, pCtx);
  }
  else
  {
    policyman_timer_stop(pMe->pOosTimer);
  }
}


/*-------- policy_generic_handle_user_pref_update --------*/
static void policy_generic_handle_user_pref_update(
  policyman_policy_t       *pPolicy,
  policy_execute_ctx_t     *pCtx
)
{  
  policyman_policy_generic_t  *pMe        = POLICY_TO_GENERIC_POLICY(pPolicy);
  sys_oprt_mode_e_type         oprt_mode  = policyman_ph_get_oprt_mode(POLICY_SUBS(pPolicy));
  policyman_subs_state_t      *pSubsState = policyman_state_get_subs_state(pCtx->asubs_id);
  size_t                       i;

  /* Stop any OOS timer, and reset the SS state info and preconditions if UE entered LPM mode
  */
  if (oprt_mode ==SYS_OPRT_MODE_LPM)
  {
    policyman_timer_stop(pMe->pOosTimer);

    for(i=SYS_MODEM_STACK_ID_1; i< SYS_MODEM_STACK_ID_MAX; i++)
    {
      policyman_ss_set_default_value(pSubsState->pSsInfo, i);
    }
    policyman_state_reset_precondition_met(POLICYMAN_PRECOND_SS | POLICYMAN_PRECOND_LOCATION, pSubsState->asubs_id);
  }
}

/*-----------------------------------------------------------------------------
  VTable for policy_generic
-----------------------------------------------------------------------------*/
policyman_policy_vtbl_t  policyman_policy_generic_vtbl =
{
  policy_generic_new_instance,
  policy_generic_init_status,
  policy_generic_configure,
  policy_generic_execute,
  policy_generic_notify_service,
  policy_generic_handle_user_pref_update,
  NULL
};


#endif