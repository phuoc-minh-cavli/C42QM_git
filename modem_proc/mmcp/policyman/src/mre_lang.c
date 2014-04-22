/**
  @file mre_lang.c

  @brief  Code for the "language" used in policyman XML.
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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/src/mre_lang.c#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/
#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
#include "mre_engine.h"
#include "mre_lang.h"
#include "mre_rules.h"
#include "mre_set.h"
#include "mre_util.h"
#include "mre_dbg.h"

#include <stringl/stringl.h>



typedef struct
{
  MRE_NAMED_OBJECT_BASE;

  uint32  value;
} mre_boolean_t;




/*=============================================================================
  Public utility functions
=============================================================================*/

/*-------- mre_boolean_get_value_from_set --------*/
mre_status_t mre_boolean_get_value_from_set(
  char const          *pName,
  boolean             *pValue,
  mre_set_t           *pSet
)
{
  mre_status_t  status = MRE_STATUS_ERR_NOT_PRESENT;
  mre_boolean_t *pBoolean;
  boolean       value = FALSE;

  pBoolean = (mre_boolean_t *) mre_named_object_find_in_set(
                                        pName,
                                        MRE_NAMED_BOOLEAN,
                                        pSet
                                        );
  if (pBoolean != NULL)
  {
    value = (boolean) pBoolean->value;
    status = MRE_STATUS_SUCCESS;
  }
  
  *pValue = value;
  return status;
}


/*-------- mre_boolean_get_value --------*/
mre_status_t mre_boolean_get_value(
  char const          *pName,
  boolean             *pValue,
  mre_policy_t        *pPolicy
)
{
  mre_status_t   status = MRE_STATUS_ERR_NOT_PRESENT;
  mre_boolean_t *pBoolean;

  pBoolean = (mre_boolean_t *) mre_named_object_find(pName, MRE_NAMED_BOOLEAN, pPolicy);
  if (NULL != pBoolean)
  {
    status = MRE_STATUS_SUCCESS;
    *pValue = (boolean) pBoolean->value;
  }
  
  return status;
}


/*=============================================================================
  Private utility functions
=============================================================================*/

/*-------- mre_str_to_boolean --------*/
static mre_status_t mre_str_to_boolean(
  char const  *pStr,
  uint32      *pBoolean
)
{
  mre_status_t  status = MRE_STATUS_ERR_MALFORMED_XML;

  if (pStr == NULL)
  {
    goto Done;
  }

  if      (strcasecmp(pStr, "true") == 0)
  {
    *pBoolean = TRUE;
  }
  else if (strcasecmp(pStr, "false") == 0)
  {
    *pBoolean = FALSE;
  }
  else
  {
    goto Done;
  }

  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}




/*=============================================================================
  Conditions
=============================================================================*/


/*-----------------------------------------------------------------------------
  <true>
-----------------------------------------------------------------------------*/

typedef struct
{
  MRE_CONDITION_BASE;
} mre_true_condition_t;



static boolean mre_condition_true_evaluate(
  mre_condition_t const *pCondition,
  void                  *pCtx
)
{
  MRE_MSG_HIGH("condition <true> returns 1", 0, 0, 0);
  return TRUE;
}


mre_status_t mre_condition_true_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_condition_t         **ppCondition
)
{
  mre_true_condition_t  *pCondition;

  pCondition = (mre_true_condition_t *)
                  mre_mem_alloc(sizeof(mre_true_condition_t));

  ref_cnt_obj_init(pCondition, mre_condition_dtor);
  pCondition->isTrue = mre_condition_true_evaluate;

  *ppCondition = (mre_condition_t *) pCondition;
  MRE_MSG_HIGH("mre_condition_true_new returned SUCCESS", 0, 0, 0);

  return MRE_STATUS_SUCCESS;
}


/*-----------------------------------------------------------------------------
  <boolean_is_true>
-----------------------------------------------------------------------------*/


typedef struct
{
  MRE_CONDITION_BASE;

  char const  *pBooleanName;
} mre_boolean_test_t;


/*-------- mre_boolean_test_dtor --------*/
static void mre_boolean_test_dtor(
  void  *pObj
)
{
  mre_boolean_test_t  *pMe = (mre_boolean_test_t *) pObj;

  MRE_MEM_FREE_IF(pMe->pBooleanName);
  mre_action_dtor(pMe);
}


/*-------- mre_boolean_test_evaluate --------*/
static boolean mre_boolean_test_evaluate(
  mre_condition_t const *pCondition,
  void                  *pCtx
)
{
  mre_boolean_test_t  *pCond = (mre_boolean_test_t *) pCondition;
  mre_boolean_t       *pBoolean;
  boolean                   result;

  pBoolean = (mre_boolean_t *) mre_named_object_find(
                                          pCond->pBooleanName,
                                          MRE_NAMED_BOOLEAN,
                                          pCondition->pPolicy
                                          );
  result =    pBoolean != NULL 
          &&  pBoolean->value != 0;

  MRE_MSG_HIGH("<boolean_test> returns %d", result, 0, 0);
  MRE_UT_MSG_S2("<boolean_test name='%s'> returns %d", pCond->pBooleanName, result);

  return result;
}


/*-------- mre_boolean_test_new --------*/
mre_status_t mre_boolean_test_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_condition_t         **ppCondition
)
{
  mre_status_t         status = MRE_STATUS_ERR_INVALID_CONDITION;
  mre_boolean_test_t  *pCondition = NULL;
  char const          *pStr;
  char                *pBooleanName = NULL;

  /*  Get the name of the boolean to test.
   */
  pStr = mre_xml_get_attribute(pElem, "name");
  if (pStr == NULL)
  {
    MRE_UT_MSG("<boolean_test>: requires a 'name' attribute", 0, 0, 0);
    goto Done;
  }
  pBooleanName = mre_str_dup(pStr);

  /*  Create the condition.
   */
  pCondition = (mre_boolean_test_t *)
                  mre_mem_alloc(sizeof(mre_boolean_test_t));
  ref_cnt_obj_init(pCondition, mre_boolean_test_dtor);

  pCondition->isTrue = mre_boolean_test_evaluate;
  pCondition->pBooleanName = pBooleanName;
  
  status = MRE_STATUS_SUCCESS;

Done:
  *ppCondition = (mre_condition_t *) pCondition;

  return status;
}



/*=============================================================================
  Compound conditions (those that modify other conditions)
=============================================================================*/

/*-----------------------------------------------------------------------------
  Utility functions for compound conditions
-----------------------------------------------------------------------------*/

typedef struct
{
  MRE_CONDITION_BASE;

  mre_set_t *pCondSet;
} mre_compound_condition_t;


/*-------- mre_compound_condition_dtor --------*/
static void mre_compound_condition_dtor(
  void  *pObj
)
{
  mre_compound_condition_t *pMe = (mre_compound_condition_t *) pObj;
  REF_CNT_OBJ_RELEASE_IF(pMe->pCondSet);
  mre_condition_dtor(pObj);
}


/*-------- mre_collect_conditions --------*/
static mre_set_t *mre_collect_conditions(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy
)
{
  size_t    nConds;
  size_t    i;
  mre_set_t *pCondSet = NULL;

  nConds = mre_xml_num_children(pElem);
  if (nConds == 0)
  {
    MRE_UT_MSG_S1("<%s>: must have subelements", mre_xml_get_tag(pElem));
    goto Done;
  }

  pCondSet = mre_set_refcnt_new(nConds, nConds, NULL);

  for (i = 0 ; i < nConds ; ++i)
  {
    mre_xml_element_t const *pChild = mre_xml_get_child(pElem, i);

    if (pChild == NULL)
    {
      MRE_MSG_ERROR(
          "Internal error: unable to get child %d of node when number of nodes is %d",
          i, nConds, 0);
      REF_CNT_OBJ_RELEASE_IF(pCondSet);
      goto Done;
    }

    mre_rule_add_condition(pPolicy, pCondSet, pChild);
  }

Done:
  return pCondSet;
}


/*-------- mre_compound_condition_new --------*/
static mre_status_t mre_compound_condition_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  pfn_evaluate_t          pfnEvaluate,
  mre_condition_t         **ppCondition
)
{
  mre_status_t              status = MRE_STATUS_ERR_INVALID_CONDITION;
  mre_set_t                 *pCondSet = NULL;
  mre_compound_condition_t  *pCondition = NULL;

  /*  Get the subelements of this condition.
   */
  pCondSet = mre_collect_conditions(pElem, pPolicy);
  if (pCondSet == NULL)
  {
    MRE_UT_MSG_S1("<%s>: unable to collect conditions", mre_xml_get_tag(pElem));
    goto Done;
  }

  /*  Create the condition
   */
  pCondition = (mre_compound_condition_t *)
                  mre_mem_alloc(sizeof(mre_compound_condition_t));
  ref_cnt_obj_init(pCondition, mre_compound_condition_dtor);

  pCondition->isTrue = pfnEvaluate;
  pCondition->pCondSet = pCondSet;
  
  status = MRE_STATUS_SUCCESS;

Done:
  *ppCondition = (mre_condition_t *) pCondition;
  MRE_UT_MSG_S2("mre_condition_%s_new status %d", mre_xml_get_tag(pElem), status);

  return status;
}


/*-----------------------------------------------------------------------------
  <not>
-----------------------------------------------------------------------------*/

/*-------- mre_condition_not_evaluate --------*/
static boolean mre_condition_not_evaluate(
  mre_condition_t const *pCondition,
  void                  *pCtx
)
{
  mre_compound_condition_t  *pCond = (mre_compound_condition_t *) pCondition;
  boolean                   result;

  result = !mre_condset_evaluate(pCond->pCondSet, pCtx);
  
  MRE_MSG_HIGH("condition <not> returns %d", result, 0, 0);
  return result;
}


/*-------- mre_condition_not_new --------*/
mre_status_t mre_condition_not_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_condition_t         **ppCondition
)
{
  return mre_compound_condition_new(
                pElem,
                pPolicy,
                mre_condition_not_evaluate,
                ppCondition
                );
}


/*-----------------------------------------------------------------------------
  <or>
-----------------------------------------------------------------------------*/

/*-------- or_condition_test --------*/
static boolean or_condition_test(
  void  *pElem,
  void  *pData1,
  void  *pData2
)
{
  mre_condition_t const *pCond = *((mre_condition_t const **) pElem);
  boolean               *pIsTrue = (boolean *) pData1;
  void                  *pCtx = pData2;

  /*  If the condition is true, we set the result of the test to TRUE, but
   *  return FALSE to stop execution.
   */
  if (pCond->isTrue(pCond, pCtx))
  {
    *pIsTrue = TRUE;
    return FALSE;
  }

  /*  Otherwise, return TRUE to continue the iteration.
   */
  return TRUE;
}


/*-------- mre_condset_any_true --------*/
boolean mre_condset_any_true(
  mre_set_t const *pCondSet,
  void            *pCtx
)
{
  boolean condsetIsTrue = FALSE;
  
  mre_set_iterate(
            (mre_set_t *) pCondSet, 
            or_condition_test,
            (void *) &condsetIsTrue,
            (void *) pCtx
            );

  return condsetIsTrue;
}



/*-------- mre_condition_or_evaluate --------*/
static boolean mre_condition_or_evaluate(
  mre_condition_t const *pCondition,
  void                  *pCtx
)
{
  mre_compound_condition_t  *pCond = (mre_compound_condition_t *) pCondition;
  boolean                         result;

  result = mre_condset_any_true(pCond->pCondSet, pCtx);
  
  MRE_MSG_HIGH("condition <any_of> returns %d", result, 0, 0);
  return result;
}


/*-------- mre_condition_or_new --------*/
mre_status_t mre_condition_or_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_condition_t         **ppCondition
)
{
  return mre_compound_condition_new(
                pElem,
                pPolicy,
                mre_condition_or_evaluate,
                ppCondition
                );
}


/*-----------------------------------------------------------------------------
  <and>
-----------------------------------------------------------------------------*/

/*-------- mre_condition_and_evaluate --------*/
static boolean mre_condition_and_evaluate(
  mre_condition_t const *pCondition,
  void                  *pCtx
)
{
  mre_compound_condition_t  *pCond = (mre_compound_condition_t *) pCondition;
  boolean                   result;

  result = mre_condset_evaluate(pCond->pCondSet, pCtx);
  
  MRE_MSG_HIGH("condition <all_of> returns %d", result, 0, 0);
  return result;
}


/*-------- mre_condition_and_new --------*/
mre_status_t mre_condition_and_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_condition_t         **ppCondition
)
{
  return mre_compound_condition_new(
                pElem,
                pPolicy,
                mre_condition_and_evaluate,
                ppCondition
                );
}


/*=============================================================================
  Actions
=============================================================================*/


/*-----------------------------------------------------------------------------
  <continue>
-----------------------------------------------------------------------------*/

/*-------- mre_continue_execute --------*/
static boolean mre_continue_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  mre_rules_continue_evaluation(pAction->pPolicy);
  MRE_MSG_HIGH("<continue> encountered - continuing rule evaluation", 0, 0, 0);
  return TRUE;
}


/*-------- mre_continue_new --------*/
mre_status_t mre_continue_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_action_t            **ppAction
)
{
  mre_action_t  *pAction = NULL;

  pAction = (mre_action_t *)
                mre_mem_alloc(sizeof(mre_action_t));
  
  ref_cnt_obj_init(pAction, mre_action_dtor);
  pAction->execute = mre_continue_execute;

  *ppAction = (mre_action_t *) pAction;
  MRE_MSG_HIGH("mre_continue_new returned status %d", TRUE, 0, 0);

  return MRE_STATUS_SUCCESS;
}


/*-----------------------------------------------------------------------------
  <boolean_define>
-----------------------------------------------------------------------------*/


typedef struct
{
  MRE_ACTION_BASE;

  char const    *pBooleanName;  /* Name of the boolean to create */
  mre_named_obj_namespace_t ns;
  uint32        initialValue;
} boolean_define_action_t;


/*-------- mre_define_boolean_execute --------*/
static boolean mre_define_boolean_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  boolean_define_action_t *pBooleanAction = (boolean_define_action_t *) pAction;
  mre_policy_t      *pPolicy = pAction->pPolicy;
  mre_boolean_t     *pBoolean;
  boolean                 succeeded;

  if (pPolicy == NULL)
  {
    MRE_MSG_ERROR("Internal error - executing <define_boolean> with no policy", 0, 0, 0);
    return FALSE;
  }

  pBoolean = (mre_boolean_t *) mre_mem_alloc(sizeof(mre_boolean_t));
  ref_cnt_obj_init(pBoolean, mre_named_object_dtor);
  mre_named_object_init(
            (mre_named_object_t *) pBoolean,
            pBooleanAction->pBooleanName,
            MRE_NAMED_BOOLEAN
            );
  pBoolean->value = pBooleanAction->initialValue;

  succeeded = mre_named_object_add(
                  (mre_named_object_t *) pBoolean,
                  pBooleanAction->ns,
                  pPolicy
                  );
  ref_cnt_obj_release(pBoolean);

  return succeeded;
}


/*-------- mre_define_boolean_dtor --------*/
static void mre_define_boolean_dtor(
  void  *pObj
)
{
  boolean_define_action_t  *pMe = (boolean_define_action_t *) pObj;

  MRE_MEM_FREE_IF(pMe->pBooleanName);
  mre_action_dtor(pMe);
}


/*-------- mre_boolean_define_new --------*/
mre_status_t mre_boolean_define_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_action_t            **ppAction
)
{
  mre_status_t            status = MRE_STATUS_ERR_MALFORMED_XML;
  char const              *pName;
  mre_named_obj_namespace_t ns;
  char const              *pInitial;
  uint32                  initial = FALSE;
  boolean_define_action_t *pAction;

  pName = mre_xml_get_attribute(pElem, "name");
  if (pName == NULL)
  {
    MRE_UT_MSG("<boolean_define>: 'name' attribute required", 0, 0, 0);
    goto Done;
  }

  ns = mre_named_object_get_namespace(pElem);  

  pInitial = mre_xml_get_attribute(pElem, "initial");
  status = mre_str_to_boolean(pInitial, &initial);
  if (MRE_FAILED(status))
  {
    MRE_UT_MSG("<boolean_define>: valid values for 'initial' attribute are 'true' and 'false'", 0, 0, 0);
    goto Done;
  }

  /*  Allocate and initialize the timer create action.
   */
  pAction = (boolean_define_action_t *)
                mre_mem_alloc(sizeof(boolean_define_action_t));
  ref_cnt_obj_init(pAction, mre_define_boolean_dtor);
  pAction->execute = mre_define_boolean_execute;

  pAction->pBooleanName = mre_str_dup(pName);
  pAction->ns = ns;
  pAction->initialValue = initial;

  *ppAction = (mre_action_t*) pAction;
  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}



/*-----------------------------------------------------------------------------
  <boolean_set>
-----------------------------------------------------------------------------*/

typedef struct
{
  MRE_ACTION_BASE;

  char const  *pBooleanName;  /* Name of the boolean to set */
  uint32      value;
  mre_set_t   *pConditions;
} boolean_set_action_t;


/*-------- mre_boolean_set_action_dtor --------*/
void mre_boolean_set_action_dtor(
  void  *pObj
)
{
  boolean_set_action_t  *pAction = (boolean_set_action_t *) pObj;

  MRE_MEM_FREE_IF(pAction->pBooleanName);
  REF_CNT_OBJ_RELEASE_IF(pAction->pConditions);
  mre_action_dtor(pAction);
}


/*-------- mre_boolean_set_execute --------*/
static boolean mre_boolean_set_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  boolean_set_action_t  *pAct = (boolean_set_action_t *) pAction;
  mre_boolean_t   *pBoolean;

  pBoolean = (mre_boolean_t *) mre_named_object_find(
                                          pAct->pBooleanName,
                                          MRE_NAMED_BOOLEAN,
                                          pAction->pPolicy
                                          );
  if (pBoolean == NULL)
  {
    MRE_UT_MSG_S1("<boolean_set>: no boolean named %s", pAct->pBooleanName);
    MRE_MSG_HIGH("<boolean_set>: unable to find boolean", 0, 0, 0);
    return FALSE;
  }

  if (pAct->pConditions != NULL)
  {
    MRE_MSG_HIGH("<boolean_set>: evaluating conditions for boolean", 0, 0, 0);
    pBoolean->value = mre_condset_evaluate(pAct->pConditions, pCtx);
  }
  else
  {
    pBoolean->value = pAct->value;
  }

  MRE_UT_MSG_S2("<boolean_set>: boolean %s set to %d", pBoolean->pName, pBoolean->value);
  MRE_MSG_HIGH("<boolean_set>: boolean set to %d", pBoolean->value, 0, 0);

  return TRUE;
}


/*-------- mre_boolean_set_new --------*/
mre_status_t mre_boolean_set_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_action_t            **ppAction
)
{
  mre_status_t          status = MRE_STATUS_ERR_INVALID_ACTION;
  char const            *pName;
  char const            *pValue;
  uint32                value = FALSE;
  size_t                nConds;
  mre_set_t             *pCondSet = NULL;
  boolean_set_action_t  *pAction = NULL;

  pName = mre_xml_get_attribute(pElem, "name");
  if (pName == NULL)
  {
    MRE_UT_MSG("<boolean_set>: 'name' attribute required", 0, 0, 0);
    goto Done;
  }

  /*  See if there are children of this element (conditions).
   */
  nConds = mre_xml_num_children(pElem);

  /*  see if there is a "value" attribute on this element.
   */
  pValue = mre_xml_get_attribute(pElem, "value");

  /*  You can't have both a value and conditions (children).
   */
  if (nConds != 0 && pValue != NULL)
  {
    MRE_MSG_HIGH("<boolean_set>: can't have both a 'value' attribute and conditions", 0, 0, 0);
    goto Done;
  }

  /*  You must have either a value or conditions (children).
   */
  if (nConds == 0 && pValue == NULL)
  {
    MRE_MSG_HIGH("<boolean_set>: must have either a 'value' attribute or conditions", 0, 0, 0);
    goto Done;
  }

  /*  If we have conditions, collect them; otherwise, get the value.
   */
  if (nConds != 0)
  {
    MRE_UT_MSG_S1("<boolean_set>: collecting conditions for boolean %s", pName);
    pCondSet = mre_collect_conditions(pElem, pPolicy);
    if (pCondSet == NULL)
    {
      MRE_UT_MSG("<boolean_set>: unable to collect conditions", 0, 0, 0);
      goto Done;
    }
  }
  else
  {
    status = mre_str_to_boolean(pValue, &value);
    if (MRE_FAILED(status))
    {
      MRE_UT_MSG("<boolean_set>: 'value' attribute of 'true' or 'false' required for boolean", pName, 0, 0);
      goto Done;
    }
  }

  pAction = (boolean_set_action_t *)
                mre_mem_alloc(sizeof(boolean_set_action_t));
  ref_cnt_obj_init(pAction, mre_boolean_set_action_dtor);

  pAction->execute = mre_boolean_set_execute;
  pAction->pBooleanName = mre_str_dup(pName);
  pAction->value = value;
  pAction->pConditions = pCondSet;

  *ppAction = (mre_action_t *) pAction;
  status = MRE_STATUS_SUCCESS;

Done:
  MRE_MSG_HIGH("mre_boolean_set_new returned status %d", status, 0, 0);
  return status;
}


/*=============================================================================
  Action to run an embedded rule.
=============================================================================*/


typedef struct
{
  MRE_ACTION_BASE;

  mre_rule_t  *pRule;
} if_action_t;


/*-------- mre_if_action_dtor --------*/
static void mre_if_action_dtor(
  void  *pObj
)
{
  if_action_t *pMe = (if_action_t *) pObj;

  REF_CNT_OBJ_RELEASE_IF(pMe->pRule);
  mre_action_dtor(pMe);
}


/*-------- mre_if_action_execute --------*/
static boolean mre_if_action_execute(
  mre_action_t const  *pAction,
  void                *pCtx
)
{
  if_action_t   *pAct = (if_action_t *) pAction;
  mre_policy_t  *pPolicy = pAct->pPolicy;
  boolean       contEval;

  /*  Embedded 'if' actions are just like rules, except for one thing: the fact
   *  that the actions are executed does *not* mean that rule evaluation should
   *  stop.  If the enclosing top-level rule has a <continue />, an 'if' action
   *  should not override this.  On the other hand, if the 'if' does a <continue />
   *  *that* should be honored.
   *
   *  This is enforced by checking the 'continue' result after evaluating the
   *  'if'.  If it is TRUE, we leave it as-is.  If it is FALSE, we restore the
   *  state prior to evaluating the 'if'.
   */
  
  contEval = MRE_POLICY_CONTEVAL(pPolicy);

  MRE_MSG_HIGH_1("Executing 'if' in rule #%d", MRE_POLICY_RULENUM(pPolicy));
  mre_rule_evaluate(pAct->pRule, pPolicy, pCtx);

  if (!MRE_POLICY_CONTEVAL(pPolicy))
  {
    MRE_POLICY_CONTEVAL(pPolicy) = contEval;
  }

  return TRUE;
}


/*-------- mre_if_action_new --------*/
mre_status_t mre_if_action_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_action_t            **ppAction
)
{
  boolean       succeeded;
  mre_rule_t    *pRule = NULL;
  if_action_t   *pAct;

  /*  Create the rule from the node.
   */
  succeeded = mre_rule_create(pPolicy, pElem, &pRule);
  if (succeeded)
  {
    /*  Create the action.
     */
    pAct = (if_action_t *) mre_mem_alloc(sizeof(*pAct));
    ref_cnt_obj_init(pAct, mre_if_action_dtor);
    pAct->pRule = pRule;
    pAct->execute = mre_if_action_execute;
    *ppAction = (mre_action_t *) pAct;
  }

  return succeeded ? MRE_STATUS_SUCCESS : MRE_STATUS_ERR_MALFORMED_XML;
}

#endif