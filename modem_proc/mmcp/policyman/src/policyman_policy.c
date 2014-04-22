/**
  @file policyman_policy.c

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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/src/policyman_policy.c#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/
#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
#include "mre.h"
#include "mre_efs.h"
#include "mre_set.h"
#include "mre_util.h"
#include "mre_engine.h"


#include "policyman_cfgitem.h"
#include "policyman_dbg.h"
#include "policyman_device_config.h"
#include "policyman_efs.h"
#include "policyman_network_events.h"
#include "policyman_plmn.h"
#include "policyman_policy.h"
#include "policyman_rat_capability.h"
#include "policyman_state.h"
#include "policyman_subs.h"
#include "policyman_svc_mode.h"
#include "policyman_task.h"
#include "policyman_tech.h"
#include "policyman_timer.h"
#include "policyman_uim.h"
#include "policyman_util.h"
#include "policyman_xml.h"

#include "policyman_generic.h"

#include <stringl/stringl.h>


/*=============================================================================
  Mappings between condition tags and the functions to create a new condition
  from the XML document.
=============================================================================*/


static mre_condition_map_element_t const  policyman_condition_map[] =
{
  /*---------------------------------------------------------------------------
    Serving system conditions
  ---------------------------------------------------------------------------*/
  {"serving_plmn_in",     policyman_condition_serving_plmn_new,       POLICYMAN_PRECOND_SS},
  {"oos_scan_cnt",        policyman_condition_oos_scan_cnt_new,       POLICYMAN_PRECOND_NONE},
  {"location_mcc_in",     policyman_condition_location_mcc_new,       POLICYMAN_PRECOND_NONE},
  {"have_service",        policyman_condition_have_service_new,       POLICYMAN_PRECOND_NONE},

  /*---------------------------------------------------------------------------
    UIM conditions
  ---------------------------------------------------------------------------*/
  {"imsi_plmn_in",        policyman_condition_imsi_plmn_new,      POLICYMAN_PRECOND_IMSI},
  {"imsi_mcc_in",         policyman_condition_imsi_mcc_new,       POLICYMAN_PRECOND_IMSI},

  /*---------------------------------------------------------------------------
    Current state conditions
  ---------------------------------------------------------------------------*/
  {"num_subs",            policyman_condition_num_subs_new,         POLICYMAN_PRECOND_NONE},
  {"enforce_full_rat",    policyman_condition_enforce_full_rat_new, POLICYMAN_PRECOND_NONE},

  /*---------------------------------------------------------------------------
    Timer conditions
  ---------------------------------------------------------------------------*/
  {"timer_expired",       policyman_condition_timer_expired_new,  POLICYMAN_PRECOND_NONE},

  /*---------------------------------------------------------------------------
    Phone Operating Mode conditions
  ---------------------------------------------------------------------------*/
  {"phone_operating_mode",policyman_condition_ph_operating_mode_new,POLICYMAN_PRECOND_NONE},

  /*---------------------------------------------------------------------------
    Terminator
  ---------------------------------------------------------------------------*/
  {NULL, NULL, POLICYMAN_PRECOND_NONE}
};



/*=============================================================================
  Mappings between action tags and the functions to create a new action
  from the XML document.
=============================================================================*/

static mre_action_map_element_t const  policyman_action_map[] =
{
  /*  Configuration item actions.
   */
  {"device_configuration",    policyman_device_config_action_new},
  {"define_config",           policyman_device_config_define_new},
  {"rat_capability",          policyman_rat_capability_action_new},
  {"rat_capability_if",       policyman_rat_capability_if_action_new},
  {"rf_bands",                policyman_rf_bands_action_new},
  {"rf_bands_if",             policyman_rf_bands_if_action_new},
  {"svc_mode",                policyman_svc_mode_action_new},

  /*  Actions to define named objects
   */
  {"plmn_list",               policyman_plmn_list_new},
  {"rf_band_list",            policyman_rf_band_list_action_new},
  {"mcc_list",                policyman_mcc_list_new},

  /*---------------------------------------------------------------------------
    Timer actions
  ---------------------------------------------------------------------------*/
  {"define_timer",            policyman_named_timer_action_new},
  {"expired_timer_handled",   policyman_expired_timer_handled_new},
  {"timer_start",             policyman_timer_start_new},
  {"timer_stop",              policyman_timer_stop_new},

  /* Network Scan actions
   */
  {"network_scan",           policyman_network_scan_action_new},

  /*---------------------------------------------------------------------------
    Terminator
  ---------------------------------------------------------------------------*/
  {NULL, NULL}
};


/*=============================================================================
  Mappings between precondition tags and their corresponding bitmasks
=============================================================================*/

static mre_precond_map_element_t policyman_precond_map[] =
{
  {"IMSI",        POLICYMAN_PRECOND_IMSI},
  {"SS",          POLICYMAN_PRECOND_SS},
  {"SIM_TYPE",    POLICYMAN_PRECOND_SIMTYPE},
  {"CDMASIMTYPE", POLICYMAN_PRECOND_CDMASIMTYPE},
  {"LOCATION",    POLICYMAN_PRECOND_LOCATION},
  {NULL,          0}
};



/*-------- g_policyVtbls --------*/
/**
@brief  Pointers to the vtables of the defined internal policies.
*/
static  policyman_policy_vtbl_t *g_policyVtbls[] =
{
  &policyman_policy_generic_vtbl,
};


STATIC  policyman_set_t      *g_pPolicySet;
STATIC  policyman_set_t      *g_pGlobalPolicySet;
STATIC  policyman_set_t      *g_pSubsPolicySet;
STATIC  atomic_word_t         g_suspendCount;
STATIC  boolean               is_policy_init;
STATIC  mre_namespace_info_t  g_namespaceInfo;


/*-------- policyman_get_subs_for_policy --------*/
STATIC sys_modem_as_id_e_type  policyman_get_subs_for_policy(
  mre_policy_t *pPolicy
)
{
  policyman_policy_t *pSubsPolicy;

  pSubsPolicy = (policyman_policy_t *)pPolicy;
  return pSubsPolicy->subs;
}


/*-------- policyman_get_namespace_info --------*/
mre_namespace_info_t  *policyman_get_namespace_info(
  void
)
{
  return &g_namespaceInfo;
}


/*-------- policyman_namespace_info_init --------*/
STATIC void policyman_namespace_info_init(
  void
)
{
  memset(&g_namespaceInfo, 0, sizeof(g_namespaceInfo));
  g_namespaceInfo.defaultNS         = MRE_NAMESPACE_SUBS;
  g_namespaceInfo.pfnGetPolicySubs  = policyman_get_subs_for_policy;
}


/*-------- policyman_namespace_info_per_subs_deinit --------*/
STATIC void policyman_namespace_info_per_subs_deinit(
  sys_modem_as_id_e_type subsId
)
{
  REF_CNT_OBJ_RELEASE_IF(g_namespaceInfo.perSubsNamedObjs[subsId]);
}


/*-------- policyman_namespace_info_deinit --------*/
STATIC void policyman_namespace_info_deinit(
  void
)
{
  size_t  subs;

  for (subs = 0 ; subs < ARR_SIZE(g_namespaceInfo.perSubsNamedObjs) ; ++subs)
  {
    REF_CNT_OBJ_RELEASE_IF(g_namespaceInfo.perSubsNamedObjs[subs]);
  }
  REF_CNT_OBJ_RELEASE_IF(g_namespaceInfo.pGlobalNamedObjs);
}


#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */


/*-------- policyman_policy_dtor --------*/
void
policyman_policy_dtor(
  void  *pObj
)
{
  mre_policy_dtor(pObj);
}


/*-------- policyman_policy_disable--------*/
void policyman_policy_disable(
  policyman_policy_t              *pPolicy,
  policyman_policy_error_cause     reason
)
{
  MRE_POLICY_ENABLED((mre_policy_t *) pPolicy) = FALSE;
  POLICYMAN_POLICY_ERROR_CAUSE(pPolicy) = reason;
}



/*-------- policyman_policy_initialize_policy --------*/
/**
@brief  Read the policy_info file for this policy and use it to initialize
        the policy.

@param[in]  pPolicy   Pointer to the policy.

@return
  TRUE if policy is initialized
*/
static boolean policyman_policy_initialize_policy(
  policyman_policy_t  *pPolicy,
  policyman_state_t   *pState
)
{
  policyman_set_t       *pItemSet = NULL;
  policy_execute_ctx_t  ctx;

  /*  If we're passed a NULL pointer for the state, we're just running
   *  an XML validation test.  Finish here.
   */
  if (pState == NULL)               { goto Done; }

  /*  Configure the policy, setting any initial configuration items.
   */
  pItemSet     = policyman_itemset_new();
  ctx.pItemSet = pItemSet;
  ctx.pState   = pState;
  ctx.asubs_id = pPolicy->subs;

  mre_policy_init(
        (mre_policy_t *) pPolicy,
        policyman_get_namespace_info(),
        &ctx
        );

  POLICY_CONFIGURE(pPolicy, &ctx);

  if (!policyman_feature_is_supported_by_hardware(pItemSet))
  {
    POLICYMAN_MSG_ERROR_1("========== Mismatch in XML and device capabilities, trm cap = 0x%x ========", policyman_state_get_trm_cap());
    policyman_policy_disable(pPolicy, POLICYMAN_POLICY_ERROR_FEATURE_MISMATCH);
    goto Done;
  }

  /* Update the database and send client notification */
  policyman_cfgitem_update_items(pItemSet);

  POLICYMAN_MSG_HIGH_0("---------- Initial database after initialization");
  policyman_cfgitem_display_all();
  POLICYMAN_MSG_SEPARATOR(POLICYMAN_SEPARATOR_HYPHEN);


Done:

  REF_CNT_OBJ_RELEASE_IF(pItemSet);
  return TRUE;
}


static boolean  policyman_init_succeeded;


/*-------- policyman_policy_init_succeeded --------*/
boolean
policyman_policy_init_succeeded(
  void
)
{
  return policyman_init_succeeded;
}


/*-------- policyman_global_policy_files --------*/
static char const  *policyman_global_policy_files[] =
{
  POLICYMAN_DEVICE_CONFIG_XML_NAME,
};


/*-------- policyman_subs_policy_files --------*/
static char const  *policyman_subs_policy_files[] =
{
  POLICYMAN_RAT_ORDER_XML_NAME,
  POLICYMAN_RESTRICTIONS_XML_NAME,
  POLICYMAN_CARRIER_POLICY_XML_NAME,
  POLICYMAN_SEGMENT_LOADING_XML_NAME,
};


/*-------- policyman_policy_file_enum_init --------*/
void policyman_policy_file_enum_init(
  policyman_policy_enum_ctx *pCtx
)
{
  *pCtx = 0;
}

/*-------- policyman_policy_global_file_enum_next --------*/
boolean policyman_policy_global_file_enum_next(
  policyman_policy_enum_ctx *pCtx,
  char                      *pFileName,
  size_t                     nameLength
)
{
  policyman_policy_enum_ctx index     = *pCtx;
  size_t                    arraySize = ARR_SIZE(policyman_global_policy_files);
  boolean                   havePath  = FALSE;

  POLICYMAN_MSG_HIGH_1("policyman_policy_file_enum_next: processing file index %d", index);

  /* We have more XMLs to look into under different folders
        Ensure folder name string fits well into the buffer space
   */
  if (index < arraySize)
  {
    snprintf(pFileName, nameLength, POLICYMAN_EFS_ROOT_DIR"/%s", policyman_global_policy_files[index]);
    havePath = TRUE;
  }

  *pCtx = index + 1;
  return havePath;
}

/*-------- policyman_policy_file_enum_next --------*/
boolean policyman_policy_file_enum_next(
  policyman_policy_enum_ctx *pCtx,
  char                      *pFileName,
  size_t                     nameLength
)
{
  policyman_policy_enum_ctx index     = *pCtx;
  size_t                    arraySize = ARR_SIZE(policyman_subs_policy_files);
  boolean                   havePath  = FALSE;

  POLICYMAN_MSG_HIGH_1("policyman_policy_file_enum_next: processing file index %d", index);

  /* We have more XMLs to look into under different folders
        Ensure folder name string fits well into the buffer space
   */
  if (index < arraySize)
  {
    snprintf(pFileName, nameLength, POLICYMAN_EFS_ROOT_DIR"/%s", policyman_subs_policy_files[index]);
    havePath = TRUE;
  }

  *pCtx = index + 1;
  return havePath;
}


/*-------- policyman_policy_set_execute_subs --------*/
static void policyman_policy_set_execute_subs(
  policyman_policy_t      *pPolicy,
  sys_modem_as_id_e_type   subsId
)
{
  pPolicy->subs = subsId;
}


/*-------- policyman_policy_new_policy_by_name --------*/
static policyman_policy_t *
policyman_new_policy_by_name(
  const char  *pName
)
{
  size_t              i;
  policyman_policy_t  *pPolicy = NULL;

  for (i = 0 ; i < ARR_SIZE(g_policyVtbls) ; ++i)
  {
    pPolicy = g_policyVtbls[i]->newInstance(pName);
    if (pPolicy != NULL)
    {
      pPolicy->pVtbl = g_policyVtbls[i];
      break;
    }
  }

  return pPolicy;
}


/*-------- policyman_new_policy --------*/
static mre_policy_t *
policyman_new_policy(
  char              const *pPolicyName,
  mre_xml_element_t const *pPolicyElem
)
{
  policyman_policy_t  *pPolicy = policyman_new_policy_by_name(pPolicyName);

  if (NULL != pPolicy)
  {
    POLICYMAN_MSG_HIGH_1("Policyman: created policy %d", pPolicy->id);
  }

  return (mre_policy_t *) pPolicy;
}

/*-------- policyman_policy_load_global --------*/
size_t policyman_policy_load_global(
  policyman_state_t        *pState
)
{
  policyman_policy_enum_ctx  fileCtx;
  char                       fileName[128] = {0};
  size_t                     numFiles      = 0;

  policyman_policy_file_enum_init(&fileCtx);

  while (policyman_policy_global_file_enum_next(&fileCtx, fileName, sizeof(fileName)))
  {
    mre_status_t               status;
    policyman_policy_t        *pPolicy           = NULL;

    POLICYMAN_UT_MSG_1( "Loading policy file %s",
                        fileName );

    status = mre_policy_create( fileName,
                                SYS_MODEM_AS_ID_NONE,
                                policyman_condition_map,
                                policyman_action_map,
                                policyman_precond_map,
                                policyman_new_policy,
                                (mre_policy_t **) &pPolicy );

    if (MRE_STATUS_SUCCESS == status)
    {
      policyman_policy_set_execute_subs(pPolicy, SYS_MODEM_AS_ID_1);

      ++numFiles;
      policyman_set_add(g_pGlobalPolicySet, &pPolicy);
      ref_cnt_obj_release(pPolicy);

      policyman_policy_initialize_policy(pPolicy, pState);
    }
  }

  return numFiles;
}


/*-------- policyman_policy_load_subs --------*/
size_t policyman_policy_load_subs(
  policyman_state_t        *pState,
  sys_modem_as_id_e_type    policySubsId
)
{
  policyman_policy_enum_ctx  fileCtx;
  char                       fileName[128] = {0};
  size_t                     numFiles      = 0;

  policyman_policy_file_enum_init(&fileCtx);

  while (policyman_policy_file_enum_next(&fileCtx, fileName, sizeof(fileName)))
  {
    mre_status_t               status;
    policyman_policy_t        *pPolicy           = NULL;

    POLICYMAN_UT_MSG_2( "Loading policy file %s from subs %d",
                        fileName,
                        policySubsId );

    status = mre_policy_create( fileName,
                                policySubsId,
                                policyman_condition_map,
                                policyman_action_map,
                                policyman_precond_map,
                                policyman_new_policy,
                                (mre_policy_t **) &pPolicy );

    if (MRE_STATUS_SUCCESS == status)
    {
      policyman_policy_set_execute_subs(pPolicy, policySubsId);

      ++numFiles;
      policyman_set_add(g_pSubsPolicySet, &pPolicy);
      ref_cnt_obj_release(pPolicy);

      policyman_policy_initialize_policy(pPolicy, pState);
    }
  }

  return numFiles;
}

/*-------- policyman_policy_init --------*/
void policyman_policy_init(
  policyman_state_t *pState
)
{
  size_t                  numFiles = 0;
  sys_modem_as_id_e_type  subsId;

  /* Return FALSE if policy already initialized
   */
  if (is_policy_init)
  {
    return;
  }

  /*  Initialize the namespace info.
   */
  policyman_namespace_info_init();

  /*  Initialize g_suspendCount to 0.
   */
  atomic_init(&g_suspendCount, 0);

  /*  Create the set to hold the policies.
   */
  g_pPolicySet = policyman_set_refcnt_new(3, POLICYMAN_SET_NO_MAX_SIZE, NULL);
  g_pSubsPolicySet = policyman_set_refcnt_new(3, POLICYMAN_SET_NO_MAX_SIZE, NULL);
  g_pGlobalPolicySet = policyman_set_refcnt_new(3, POLICYMAN_SET_NO_MAX_SIZE, NULL);

  /*  Load all XML files that are in the policyman directory.
   */
  numFiles += policyman_policy_load_global(pState);
  
  for (subsId = SYS_MODEM_AS_ID_1; subsId < POLICYMAN_NUM_SUBS; ++subsId)
  {
    numFiles += policyman_policy_load_subs(pState, subsId);
  }

  policyman_set_add(g_pPolicySet, &g_pGlobalPolicySet);
  policyman_set_add(g_pPolicySet, &g_pSubsPolicySet);

  is_policy_init = (numFiles == (policyman_set_num_elems(g_pGlobalPolicySet) + policyman_set_num_elems(g_pSubsPolicySet)));
}


/*-------- policyman_policy_per_subs_init --------*/
void  policyman_policy_per_subs_init(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId
)
{
  size_t                  numFiles = 0;

  /* Return FALSE if policy already initialized
   */
  if (is_policy_init)
  {
    return;
  }
  
  numFiles = policyman_set_num_elems(g_pSubsPolicySet);

  /*  Load all XML files that are in the policyman directory for the given subs.
   */
  numFiles += policyman_policy_load_subs(pState, subsId);

  is_policy_init = (numFiles == policyman_set_num_elems(g_pSubsPolicySet));
}


/*-------- policyman_policy_deinit --------*/
void
policyman_policy_deinit(
  void
)
{
  is_policy_init = FALSE;

  /*  Release any named object namespaces that were created.
   */
  policyman_namespace_info_deinit();

  /*  Release the policy sets.
   */
  REF_CNT_OBJ_RELEASE_IF(g_pGlobalPolicySet);
  REF_CNT_OBJ_RELEASE_IF(g_pSubsPolicySet);
  REF_CNT_OBJ_RELEASE_IF(g_pPolicySet);
}


/*-------- policyman_policy_to_remove --------*/
STATIC boolean policyman_policy_to_remove(
  void  *pObj,
  void  *pData1,
  void  *pData2
)
{
  policyman_policy_t    *pPolicy    = *((policyman_policy_t **) pObj);
  policy_execute_ctx_t  *pCtx       = (policy_execute_ctx_t *) pData1;
  policyman_set_t       *pRemoveSet = (policyman_set_t *)pData2;

  if (pCtx->asubs_id == pPolicy->subs)
  {
    policyman_set_add(pRemoveSet, &pPolicy);
  }

  return TRUE;
}


/*-------- policyman_policy_remove --------*/
STATIC boolean policyman_policy_remove(
  void  *pObj,
  void  *pData1,
  void  *pData2
)
{
  policyman_policy_t *pPolicy    = *((policyman_policy_t **) pObj);
  policyman_set_t    *pPolicySet = (policyman_set_t *)pData1;

  policyman_set_remove(pPolicySet, &pPolicy);

  REF_CNT_OBJ_RELEASE_IF(pPolicy);

  return TRUE;
}


/*-------- policyman_policy_per_subs_deinit --------*/
void policyman_policy_per_subs_deinit(
  sys_modem_as_id_e_type subsId
)
{
  policy_execute_ctx_t    ctx;
  policyman_set_t        *pRemoveSet; 
  
  is_policy_init = FALSE;
  ctx.asubs_id   = subsId;

  pRemoveSet = policyman_set_refcnt_new(3, POLICYMAN_SET_NO_MAX_SIZE, NULL);

  /*  Release any subs specific named object namespaces that were created.
   */
  policyman_namespace_info_per_subs_deinit(subsId);

  /* Remove subs specific policies from the set
   */  
  policyman_set_iterate(g_pSubsPolicySet, policyman_policy_to_remove, &ctx, pRemoveSet);
  policyman_set_iterate(pRemoveSet, policyman_policy_remove, g_pSubsPolicySet, NULL);

  REF_CNT_OBJ_RELEASE_IF(pRemoveSet);
}


/*-------- policyman_policy_are_preconditions_met --------*/
boolean
policyman_policy_are_preconditions_met(
  policyman_policy_t  *pPolicy,
  policyman_state_t   *pState
)
{
  return mre_policy_are_preconds_met(
                (mre_policy_t *) pPolicy,
                policyman_state_get_subs_precond(pPolicy->subs)
                );
}


/*-------- policyman_policy_subs_check --------*/
static boolean
policyman_policy_subs_check(
  sys_modem_as_id_e_type subsId
)
{
  if (   subsId == SYS_MODEM_AS_ID_NONE
      || subsId >= (sys_modem_as_id_e_type) POLICYMAN_NUM_SUBS
     )
  {
    POLICYMAN_MSG_ERROR_1("Unsupported subsId %d", subsId);
    return FALSE;
  }

  return TRUE;
}

/*-------- policyman_policy_execute --------*/
void policyman_policy_execute(
  policyman_policy_t    *pPolicy,
  policy_execute_ctx_t  *pCtx
)
{
  uint32                precondsMet;

  /* Don't allow rules to execute if current subs is out of range
   */
  if (!policyman_policy_subs_check(POLICY_SUBS(pPolicy)))
  {
    return;
  }

  precondsMet = policyman_state_get_subs_precond(POLICY_SUBS(pPolicy));
  pCtx->asubs_id = POLICY_SUBS(pPolicy);

  if (POLICYMAN_POLICY_ERROR_CAUSE(pPolicy) == POLICYMAN_POLICY_ERROR_FEATURE_MISMATCH)
  {
    POLICYMAN_MSG_ERROR_1("Policyman feature does not match TRM capabilities of %x", policyman_state_get_trm_cap());
    POLICYMAN_MSG_HIGH_3("----------- Error in policy version %d.%d.%d -----------",
                         MRE_POLICY_NUMBER((mre_policy_t *)pPolicy),
                         MRE_POLICY_TYPE((mre_policy_t *)pPolicy),
                         MRE_POLICY_VERSION((mre_policy_t *)pPolicy));
  }

  mre_policy_execute((mre_policy_t *) pPolicy, precondsMet, pCtx);
}

/*-------- policyman_policy_run --------*/
static boolean policyman_policy_run(
  void  *pObj,
  void  *pData1,
  void  *pData2
)
{
  policyman_policy_t    *pPolicy = *((policyman_policy_t **) pObj);
  policy_execute_ctx_t  *pCtx    = (policy_execute_ctx_t *) pData1;

  policyman_policy_execute(pPolicy, pCtx);

  if (POLICY_CAN_EXECUTE(pPolicy))
  {
    POLICY_EXECUTE(pPolicy, pCtx);
  }

  return TRUE;
}

/*-------- policyman_policy_can_policy_execute --------*/
STATIC boolean policyman_policy_can_policy_execute(
  void
)
{
  // return FALSE if suspended or in test mode
  if (policyman_is_test_mode() || policyman_policy_is_suspended())
  {
    POLICYMAN_MSG_ERROR_1("test mode is enabled or policy suspended - g_suspendCount %d", g_suspendCount.value);
    return FALSE;
  }

  return TRUE;
}

/*-------- policyman_policy_set_iterate --------*/
STATIC boolean policyman_policy_set_iterate(
  void  *pObj,
  void  *pData1,
  void  *pData2
)
{
  policyman_set_t      *pPolicySet = *((policyman_set_t **)pObj);
  policy_execute_ctx_t *pCtx       = (policy_execute_ctx_t *)pData1;
  elem_iter_fn_t        pfnIter    = (elem_iter_fn_t)pData2;

  policyman_set_iterate(pPolicySet, pfnIter, pCtx, NULL);

  return TRUE;
}


/*-------- policyman_policyset_execute --------*/
STATIC void policyman_policyset_execute(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId,
  boolean                 haveService,
  elem_iter_fn_t          pfnIter
)
{
  policy_execute_ctx_t    ctx;
  policyman_set_t        *pItemSet;

  pItemSet        = policyman_itemset_new();
  ctx.pState      = pState;
  ctx.pItemSet    = pItemSet;
  ctx.asubs_id    = subsId;
  ctx.haveService = haveService;

  policyman_set_iterate(g_pPolicySet, policyman_policy_set_iterate, &ctx, pfnIter);
  policyman_cfgitem_update_items(pItemSet);
  ref_cnt_obj_release(pItemSet);
}

/*-------- policyman_policy_run_policy_check --------*/
/**
@brief  Run a policy check and collect updates into the itemset passed.

@param

@return

*/
void policyman_policy_run_policy_check(
  policyman_state_t       *pState,
  sys_modem_as_id_e_type  asubs_id,
  policyman_set_t         *pItemSet
)
{
  if (policyman_policy_can_policy_execute())
  {
    policy_execute_ctx_t    ctx;

    ctx.pState    = pState;
    ctx.pItemSet  = pItemSet;
    ctx.asubs_id  = asubs_id;
    policyman_set_iterate(g_pPolicySet, policyman_policy_set_iterate, &ctx, policyman_policy_run);
  }
}


/*-------- policy_notify_service --------*/
static boolean policy_notify_service(
  void  *pObj,
  void  *pData1,
  void  *pData2
)
{
  policyman_policy_t    *pPolicy = *((policyman_policy_t **) pObj);
  policy_execute_ctx_t  *pCtx = (policy_execute_ctx_t *) pData1;

  if (    POLICY_IS_ENABLED(pPolicy)
      &&  POLICY_CAN_NOTIFY_SERVICE(pPolicy)
     )
  {
    POLICY_NOTIFY_SERVICE(pPolicy, pCtx);
  }

  return TRUE;
}


/*-------- policyman_policy_notify_service --------*/
void policyman_policy_notify_service(
  boolean                haveService,
  sys_modem_as_id_e_type asubs_id
)
{
  if (policyman_policy_can_policy_execute())
  {
    policyman_state_t     *pState;

    pState = policyman_state_get_state();

    policyman_policyset_execute(pState, asubs_id, haveService, policy_notify_service);
  }
}


/*-------- policy_handle_user_pref_update --------*/
static boolean policy_handle_user_pref_update(
  void  *pObj,
  void  *pData1,
  void  *pData2
)
{
  policyman_policy_t    *pPolicy = *((policyman_policy_t **) pObj);
  policy_execute_ctx_t  *pCtx    = (policy_execute_ctx_t *) pData1;

  if (  POLICY_IS_ENABLED(pPolicy)
     && POLICY_CAN_HANDLE_USERPREF_UPDATE(pPolicy)
     )
  {
    POLICY_HANDLE_USERPREF_UPDATE(pPolicy, pCtx );
  }

  return TRUE;
}


/*-------- policyman_policy_handle_user_pref_update --------*/
void policyman_policy_handle_user_pref_update(
  sys_modem_as_id_e_type asubs_id
)
{
  if (policyman_policy_can_policy_execute())
  {
    policyman_state_t  *pState;
    boolean             haveService = TRUE;

    pState = policyman_state_get_state();
    policyman_policyset_execute(pState, asubs_id, haveService, policy_handle_user_pref_update);
  }
}


/*-------- policy_handle_uim_update --------*/
static boolean policy_handle_uim_update(
  void  *pObj,
  void  *pData1,
  void  *pData2
)
{
  policyman_policy_t    *pPolicy = *((policyman_policy_t **) pObj);
  policy_execute_ctx_t  *pCtx    = (policy_execute_ctx_t *) pData1;

  if (  POLICY_IS_ENABLED(pPolicy)
     && POLICY_CAN_HANDLE_UIM_UPDATE(pPolicy)
     )
  {
    POLICY_HANDLE_UIM_UPDATE(pPolicy, pCtx);
  }

  return TRUE;
}


/*-------- policyman_policy_handle_uim_update --------*/
void policyman_policy_handle_uim_update(
  policyman_state_t       *pState,
  sys_modem_as_id_e_type   subsId
)
{
  if (policyman_policy_can_policy_execute())
  {
    boolean haveService = TRUE;

    policyman_policyset_execute(pState, subsId, haveService, policy_handle_uim_update);
  }
}

/*-------- policyman_policy_check_for_immediate_full_rat --------*/
void  policyman_policy_check_for_immediate_full_rat(
  policyman_policy_t  *pPolicy
)
{
  mre_status_t  status;
  boolean       immediate_fullrat;

  status = mre_boolean_get_value(
              "pm:xml_full_rat",
              &immediate_fullrat,
              (mre_policy_t *) pPolicy );
  policyman_state_update_full_rat(immediate_fullrat);
  pPolicy->immediate_full_rat = (MRE_STATUS_SUCCESS == status);
}



/*-------- policyman_policy_is_suspended --------*/
boolean
policyman_policy_is_suspended(
  void
 )
{
  return g_suspendCount.value > 0;
}

/*-------- policyman_suspend --------*/
void
policyman_suspend(
  void
  )
{
  atomic_inc(&g_suspendCount);
  POLICYMAN_MSG_HIGH_1("policy manager is suspended, g_suspendCount %d", g_suspendCount.value);
}


/*------- policyman_execute_process_resume --------*/
void policyman_execute_process_resume
(
  policyman_cmd_t *pCmd
)
{
  /* handle update here - force run rules on current multimode subs
    */
   policyman_state_handle_update(SYS_MODEM_AS_ID_1);
}

/*-------- policyman_resume--------*/
void
policyman_resume(
  void
  )
{
  policyman_cmd_t *pCmd = NULL;

  POLICYMAN_MSG_HIGH_1("policy manager resume called, g_suspendCount %d", g_suspendCount.value);

  if(policyman_policy_is_suspended())
  {
    /* if PM is suspended, then only resume - this is to avoid the conditions where resume is called without suspend
          run rules when count becomes 0
      */
    if(atomic_dec_return(&g_suspendCount)==0)
    {
      pCmd = (policyman_cmd_t *) policyman_cmd_new( sizeof(policyman_cmd_t),
                                                     policyman_execute_process_resume,
                                                     NULL,
                                                     NULL
                                                   );

      policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
      ref_cnt_obj_release(pCmd);
    }
  }
  else
  {
    POLICYMAN_MSG_ERROR_0("policy is not suspended so can't be resumed");
  }
}


/*-------- policyman_evaluate_cond_for_this_subs --------*/
boolean policyman_evaluate_cond_for_this_subs(
  policyman_condition_t const *pCond,
  void                        *pCtx,
  pfnEvaluateCondForSubs      pfnEvaluate
)
{
  return pfnEvaluate(pCond, pCtx, CONDITION_SUBS(pCond));
}


/*-------- policyman_evaluate_cond --------*/
boolean policyman_evaluate_cond(
  policyman_condition_t const *pCond,
  void                        *pCtx,
  pfnEvaluateCondForSubs      pfnEvaluate
)
{
  switch (pCond->pmSubsId)
  {
    case SUBS_THIS:
      return policyman_evaluate_cond_for_this_subs(pCond, pCtx, pfnEvaluate);
      break;

    default:
      return FALSE;
      break;
  }
}


/*-------- policyman_execute_action_for_this_subs --------*/
boolean policyman_execute_action_for_this_subs(
  policyman_action_t const *pAction,
  void                     *pCtx,
  pfnExecuteActionForSubs  pfnExecute
)
{
  return pfnExecute(pAction, pCtx, ACTION_SUBS(pAction));
}


/*-------- policyman_execute_action --------*/
boolean policyman_execute_action(
  policyman_action_t const *pAction,
  void                     *pCtx,
  pfnExecuteActionForSubs  pfnExecute
)
{
  switch (pAction->pmSubsId)
  {
    case SUBS_THIS:
      return policyman_execute_action_for_this_subs(pAction, pCtx, pfnExecute);
      break;

    default:
      return FALSE;
      break;
  }

}

#endif