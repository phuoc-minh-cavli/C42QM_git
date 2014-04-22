/**
  @file mre_rules.c

  @brief  Utility APIs to help in interpreting components of rules.
*/

/*
    Copyright (c) 2013,2014 QUALCOMM Technologies Incorporated.
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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/src/mre_rules.c#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/
#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
#include "mre.h"
#include "mre_dbg.h"
#include "mre_engine.h"
#include "mre_lang.h"
#include "mre_rules.h"
#include "mre_set.h"
#include "mre_util.h"
#include "mre_xml.h"

#include <stringl/stringl.h>


/*-----------------------------------------------------------------------------
  Definition of a rule
-----------------------------------------------------------------------------*/

/*  A rule is a set of conditions and sets of 'then' and 'else' actions.
 */
struct mre_rule_t
{
  _REF_CNT_OBJ;

  mre_policy_t  *pPolicy;
  mre_set_t     *pCondSet;
  mre_set_t     *pThenSet;
  mre_set_t     *pElseSet;
  uint32        preconds;
  uint32        id;
};



/*=============================================================================
  Conditions
=============================================================================*/


/*  Mappings between condition tags and the functions to create a new condition
 *  from the XML document.
 */
static mre_condition_map_element_t const  condition_map[] =
{
  /*---------------------------------------------------------------------------
    Language primitives
  ---------------------------------------------------------------------------*/
  {"true",                mre_condition_true_new,           MRE_PRECOND_NONE},
  {"not",                 mre_condition_not_new,            MRE_PRECOND_NONE},
  {"any_of",              mre_condition_or_new,             MRE_PRECOND_NONE},
  {"all_of",              mre_condition_and_new,            MRE_PRECOND_NONE},
  {"boolean_test",        mre_boolean_test_new,             MRE_PRECOND_NONE},

  /*---------------------------------------------------------------------------
    Terminator
  ---------------------------------------------------------------------------*/
  {NULL,                  NULL,                             MRE_PRECOND_NONE}
};


/*-------- mre_condition_dtor --------*/
void mre_condition_dtor(
  void  *pObj
)
{
  mre_condition_t *pCond = (mre_condition_t *) pObj;

  mre_mem_free(pCond);
}


/*-------- mre_find_condition --------*/
static mre_condition_map_element_t const *mre_find_condition(
  char                        const *pCondName,
  mre_condition_map_element_t const *pMap
)
{
  while (NULL != pMap->pTag)
  {
    if (strcmp(pCondName, pMap->pTag) == 0)
    {
      return pMap;
    }

    ++pMap;
  }

  return NULL;
}


/*-------- mre_find_condition_entry --------*/
static mre_condition_map_element_t const *mre_find_condition_entry(
  char const    *pCondName,
  mre_policy_t  *pPolicy
)
{
  mre_condition_map_element_t const *pEntry;

  /*  Search the MRE condition map first
   */
  pEntry = mre_find_condition(pCondName, condition_map);

  /*  If not found in the MRE condition map, search the client's map.
   */
  if (NULL == pEntry && NULL != MRE_POLICY_CONDTBL(pPolicy))
  {
    pEntry = mre_find_condition(pCondName, MRE_POLICY_CONDTBL(pPolicy));
  }

  if (pEntry == NULL)
  {
    MRE_UT_MSG_1("Unknown condition tag: %s", pCondName);
  }

  return pEntry;
}


/*-------- mre_rules_add_condition --------*/
boolean mre_rule_add_condition(
  mre_policy_t            *pPolicy,
  mre_set_t               *pCondSet,
  mre_xml_element_t const *pElem
)
{
  char const                        *pCondName;
  mre_condition_map_element_t const *pEntry;
  mre_condition_t                   *pCondition = NULL;

  pCondName = mre_xml_get_tag(pElem);
  pEntry = mre_find_condition_entry(pCondName, pPolicy);

  /*  If we found an entry, call its create function to create a condition.
   */
  if (NULL != pEntry)
  {
    mre_status_t      status;
    
    status = pEntry->pfnNew(pElem, pPolicy, &pCondition);
    if (MRE_FAILED(status))
    {
      MRE_UT_MSG_2("Error %08x while creating condition %s", status, pCondName);
    }
  }

  /*  If condition creation was successful, fill out the condition.
   */
  if (NULL != pCondition)
  {
    pCondition->pPolicy = pPolicy;
    mre_set_add(pCondSet, &pCondition);
    ref_cnt_obj_release(pCondition);

    MRE_POLICY_PRECONDITIONS(pPolicy) |= pEntry->precondition;
  }

  return pCondition != NULL;
}



/*-------- mre_rule_get_conditions --------*/
static boolean mre_rule_get_conditions(
  mre_policy_t            *pPolicy,
  mre_set_t               **ppCondSet,
  mre_xml_element_t const *pElem
)
{
  boolean succeeded = TRUE;
  size_t  nConds;
  size_t  condIndex;

  nConds = mre_xml_num_children(pElem);

  *ppCondSet = mre_set_refcnt_new(nConds, nConds, NULL);

  for (condIndex = 0 ; succeeded && condIndex < nConds ; ++condIndex)
  {
    mre_xml_element_t const *pChild;

    pChild = mre_xml_get_child(pElem, condIndex);
    
    MRE_XML_MSG("processing %s", mre_xml_get_tag(pChild), 0, 0);
    succeeded = mre_rule_add_condition(pPolicy, *ppCondSet, pChild);
  }

  return succeeded;
}


/*-------- condition_is_true --------*/
static boolean condition_is_true(
  void  *pElem,
  void  *pData1,
  void  *pData2
)
{
  mre_condition_t const *pCond = *((mre_condition_t const **) pElem);
  void                  *pCtx = pData1;

  /*  If the condition is true, we continue execution; otherwise we stop
   *  evaluating conditions in the set.
   */
  return pCond->isTrue(pCond, pCtx);
}


/*-------- mre_condset_evaluate --------*/
boolean mre_condset_evaluate(
  mre_set_t const   *pCondSet,
  void              *pCtx
)
{
  return mre_set_iterate((mre_set_t *) pCondSet, condition_is_true, pCtx, NULL);
}



/*=============================================================================
  Actions
=============================================================================*/

/*  Mappings between action tags and the functions to create a new action
 *  from the XML document.
 */
static mre_action_map_element_t const action_map[] =
{
  /*---------------------------------------------------------------------------
    Language actions
  ---------------------------------------------------------------------------*/
  {"boolean_define",          mre_boolean_define_new},
  {"boolean_set",             mre_boolean_set_new},
  {"continue",                mre_continue_new},
  {"if",                      mre_if_action_new},

  /*---------------------------------------------------------------------------
    Terminator
  ---------------------------------------------------------------------------*/
  {NULL,                      NULL}
};


/*=============================================================================
  General internal functions to handle rules
=============================================================================*/


/*-------- mre_action_dtor --------*/
void mre_action_dtor(
  void  *pObj
)
{
  mre_action_t  *pAction = (mre_action_t *) pObj;

  mre_mem_free(pAction);
}


/*-------- mre_find_action --------*/
static mre_action_map_element_t const *mre_find_action(
  char const                      *pActName,
  mre_action_map_element_t const  *pMap
)
{
  while (NULL != pMap->pTag)
  {
    if (strcmp(pActName, pMap->pTag) == 0)
    {
      return pMap;
    }

    ++pMap;
  }

  return NULL;
}


/*-------- mre_find_action_entry --------*/
static mre_action_map_element_t const *mre_find_action_entry(
  char const    *pActName,
  mre_policy_t  *pPolicy
)
{
  mre_action_map_element_t const *pEntry;

  /*  Search the MRE action map first
   */
  pEntry = mre_find_action(pActName, action_map);

  /*  If not found in the MRE action map, search the client's map.
   */
  if (NULL == pEntry && NULL != MRE_POLICY_ACTTBL(pPolicy))
  {
    pEntry = mre_find_action(pActName, MRE_POLICY_ACTTBL(pPolicy));
  }

  if (NULL == pEntry)
  {
    MRE_UT_MSG_1("Unknown action tag: %s", pActName);
  }

  return pEntry;
}


/*-------- mre_rule_add_action --------*/
static boolean mre_rule_add_action(
  mre_policy_t            *pPolicy,
  mre_set_t               *pActionSet,
  mre_xml_element_t const *pElem
)
{
  char const                      *pActName;
  mre_action_map_element_t const  *pEntry;
  mre_action_t                    *pAction = NULL;

  pActName = mre_xml_get_tag(pElem);
  pEntry = mre_find_action_entry(pActName, pPolicy);

  /*  If we found an entry, call its create function to create an action.
   */
  if (NULL != pEntry)
  {
    mre_status_t  status;
    
    status = pEntry->pfnNew(pElem, pPolicy, &pAction);
    if (MRE_FAILED(status))
    {
      MRE_UT_MSG_2("Error %08x while creating action %s", status, pActName);
    }
  }

  /*  If action creation was successful, fill it out.
   */
  if (NULL != pAction)
  {
    pAction->pPolicy = pPolicy;
    mre_set_add(pActionSet, &pAction);
    ref_cnt_obj_release(pAction);
  }

  return pAction != NULL;
}


/*-------- mre_rules_get_actions --------*/
static boolean mre_rule_get_actions(
  mre_policy_t                    *pPolicy,
  mre_set_t                       **ppActionSet,
  mre_xml_element_t         const *pElem
)
{
  boolean succeeded = TRUE;
  size_t  nActions;
  size_t  actIndex;

  nActions = mre_xml_num_children(pElem);
  *ppActionSet = mre_set_refcnt_new(
                        (nActions == 0)? 1 : nActions,  /* zero actions is OK, but we need a set */
                        MRE_SET_NO_MAX_SIZE,
                        NULL
                        );

  for (actIndex = 0 ; succeeded && actIndex < nActions ; ++actIndex)
  {
    mre_xml_element_t const *pChild;

    pChild = mre_xml_get_child(pElem, actIndex);

    MRE_XML_MSG("processing %s", mre_xml_get_tag(pChild), 0, 0);
    succeeded = mre_rule_add_action(pPolicy, *ppActionSet, pChild);
  }

  return succeeded;
}


/*-------- execute_action --------*/
static boolean execute_action(
  void  *pElem,
  void  *pData1,
  void  *pData2
)
{
  mre_action_t const  *pAction = *((mre_action_t const **) pElem);
  void                *pCtx = pData1;

  pAction->execute(pAction, pCtx);

  /*  We always continue executing actions...
   */
  return TRUE;
}


/*-------- mre_actionset_execute --------*/
void mre_actionset_execute(
  mre_set_t const *pActionSet,
  void            *pCtx
)
{
  mre_set_iterate((mre_set_t *) pActionSet, execute_action, pCtx, NULL);
}


/*-------- mre_rules_continue_evaluation --------*/
void mre_rules_continue_evaluation(
  mre_policy_t  *pPolicy
)
{
  MRE_POLICY_INFO(pPolicy)->contEval = TRUE;
}


/*-------- mre_rule_preconditions_met --------*/
static boolean mre_rule_preconditions_met(
  mre_rule_t  const *pRule,
  uint32            precondsMet
)
{
  boolean       met;
  mre_policy_t  *pPolicy = pRule->pPolicy;
  uint32        rulePreconds = pRule->preconds;

  switch (rulePreconds)
  {
    case MRE_PRECOND_NONE:
      met = TRUE;
      MRE_MSG_HIGH_1("Rule #%d: evaluated; no preconditions",
                        MRE_POLICY_RULENUM(pPolicy)
                    );
      break;

    case MRE_PRECOND_POLICY:
      met = MRE_POLICY_PRECONDSAREMET(pPolicy);
      if (!met)
      {
        MRE_MSG_HIGH_1("Rule #%d: skipped; policy preconditions not met",
                        MRE_POLICY_RULENUM(pPolicy)
                      );
      }
      break;

    default:
      met = ((rulePreconds & precondsMet) == rulePreconds);
      if (!met)
      {
        MRE_MSG_HIGH_3("Rule #%d: skipped due to preconditions; rule %d, met %d",
                        MRE_POLICY_RULENUM(pPolicy), rulePreconds, precondsMet
                      );
      }
      break;
  }      

  return met;
}


/*-------- mre_rule_evaluate --------*/
void mre_rule_evaluate(
  mre_rule_t const  *pRule,
  mre_policy_t      *pPolicy,
  void              *pCtx
)
{
  boolean condsTrue;
  size_t  ruleNum = MRE_POLICY_RULENUM(pPolicy);

  condsTrue = mre_condset_evaluate(pRule->pCondSet, pCtx);

  if (condsTrue)
  {
    MRE_MSG_HIGH_1("Rule #%d: executing actions", ruleNum);
    MRE_POLICY_CONTEVAL(pPolicy) = FALSE;
    mre_actionset_execute(pRule->pThenSet, pCtx);
  }
  else if (NULL != pRule->pElseSet)
  {
    MRE_MSG_HIGH_1("Rule #%d: executing 'else' actions", ruleNum);
    MRE_POLICY_CONTEVAL(pPolicy) = FALSE;
    mre_actionset_execute(pRule->pElseSet, pCtx);
  }
  else
  {
    MRE_POLICY_CONTEVAL(pPolicy) = TRUE;
  }
}


/*-------- process_rule --------*/
static boolean process_rule(
  void  *pElem,
  void  *pData1,
  void  *pData2
)
{
  mre_rule_t const  *pRule      = *((mre_rule_t const **) pElem);
  void              *pCtx       = pData1;
  uint32            precondsMet = (uint32) pData2;
  mre_policy_t      *pPolicy    = pRule->pPolicy;

  ++MRE_POLICY_RULENUM(pPolicy);

  /*  If preconditions for this rule aren't met, keep processing rules.
   */
  if (!mre_rule_preconditions_met(pRule, precondsMet))
  {
    return TRUE;
  }

  MRE_MSG_HIGH_1("Rule #%d: evaluating conditions", MRE_POLICY_RULENUM(pPolicy));
  mre_rule_evaluate(pRule, pPolicy, pCtx);

  /*  Return whether we should continue rule evaluation.
   */
  return MRE_POLICY_CONTEVAL(pPolicy);
}  


/*-------- mre_move_to_actions --------*/
/**
@brief  The <initial> block was originally defined to require an <actions> node.
        This is completely unnecessary - we should be able to embed actions
        directly in <initial>.  This function returns the <actions> subnode if
        present, otherwise it returns the <initial> node.
*/
static mre_xml_element_t const  *mre_move_to_actions(
    mre_xml_element_t const *pElem
)
{
  mre_xml_element_t const *pChild;

  /*  See if this has an <actions> node - if so move the element to
   *  point to that rather than the <initial> node.
   */
  pChild = mre_xml_get_child(pElem, 0);
  if (NULL != pChild)
  {
    char const  *pStr;

    pStr = mre_xml_get_tag(pChild);
    if (strcasecmp(pStr, "actions") == 0)
    {
      pElem = pChild;
    }
  }

  return pElem;
}


/*-------- mre_ensure_initset --------*/
/**
@brief  Return a pointer to the initset, creating it if necessary.
*/
static mre_set_t *mre_ensure_initset(
  mre_policy_t  *pPolicy,
  size_t        nChildren
)
{
  mre_set_t *pInitSet;

  pInitSet = MRE_POLICY_INITSET(pPolicy);
  if (NULL == pInitSet)
  {
    pInitSet = mre_set_refcnt_new(nChildren, MRE_SET_NO_MAX_SIZE, NULL);
    MRE_POLICY_INITSET(pPolicy) = pInitSet;
  }

  return pInitSet;
}
  

/*-------- mre_initset_add --------*/
boolean mre_initset_add(
  mre_policy_t            *pPolicy,
  mre_xml_element_t const *pElem
)
{
  boolean   succeeded = FALSE;
  size_t    nChildren;
  size_t    childIndex;
  mre_set_t *pInitSet;

  /*  Ensure we're at the node with the actions.
   */
  pElem = mre_move_to_actions(pElem);

  /*  Get the number of actions in this node
   */
  nChildren = mre_xml_num_children(pElem);

  /*  Get the initset, creating it if necessary.
   */
  pInitSet = mre_ensure_initset(pPolicy, nChildren);

  /*  Add each action to the initset.
   */
  succeeded = TRUE;
  for (childIndex = 0 ; succeeded && childIndex < nChildren ; ++childIndex)
  {
    mre_xml_element_t const *pChild;

    pChild = mre_xml_get_child(pElem, childIndex);

    MRE_XML_MSG("processing %s", mre_xml_get_tag(pChild), 0, 0);
    succeeded = mre_rule_add_action(pPolicy, pInitSet, pChild);
  }
  
  return succeeded;
}



/*-------- mre_rule_dtor --------*/
void mre_rule_dtor(
  void  *pObj
)
{
  mre_rule_t  *pRule = (mre_rule_t *) pObj;

  REF_CNT_OBJ_RELEASE_IF(pRule->pCondSet);
  REF_CNT_OBJ_RELEASE_IF(pRule->pThenSet);
  REF_CNT_OBJ_RELEASE_IF(pRule->pElseSet);
  mre_mem_free(pRule);
}


/*-------- mre_lookup_precond --------*/
static boolean mre_lookup_precond(
  mre_precond_map_element_t const *pPrecondTbl,
  char                      const *pStr,
  uint32                          *pMask
)
{
  while (NULL != pPrecondTbl->pTag)
  {
    if (strcasecmp(pStr, pPrecondTbl->pTag) == 0)
    {
      *pMask |= pPrecondTbl->mask;
      return TRUE;
    }
    ++pPrecondTbl;
  }

  return FALSE;
}


/*-------- mre_rule_parse_rule_preconds --------*/
static boolean mre_rule_parse_rule_preconds(
  mre_precond_map_element_t const *pPrecondTbl,
  uint32                          *pPreconds,
  char                      const *pStr
)
{
  char    token[100];
  uint32  mask = MRE_PRECOND_NONE;
  boolean noneSpecified = FALSE;

  while (mre_get_token(&pStr, token, sizeof(token)))
  {
    if (strcasecmp(token, "none") == 0)
    {
      noneSpecified = TRUE;
      if (MRE_PRECOND_NONE != mask)
      {
        MRE_MSG_ERROR_0("'none' precondition combined with other preconditions");
        return FALSE;
      }
    }
    else if (noneSpecified)
    {
      MRE_MSG_ERROR_0("'none' precondition combined with other preconditions");
      return FALSE;
    }
    else if (strcasecmp(token, "policy") == 0)
    {
      mask = MRE_PRECOND_POLICY;
    }
    else if (NULL == pPrecondTbl)
    {
      MRE_MSG_ERROR_0("'precond' attribute specified but no precondition table passed");
      return FALSE;
    }
    else if (!mre_lookup_precond(pPrecondTbl, token, &mask))
    {
      MRE_MSG_ERROR_0("Unsupported precondition in rule");
      return FALSE;
    }
  }

  *pPreconds = mask;
  return TRUE;
}


/*-------- mre_rule_get_rule_preconditions --------*/
static boolean mre_rule_get_rule_preconditions(
  mre_policy_t            *pPolicy,
  mre_xml_element_t const *pElem,
  uint32                  *pPreconds
)
{
  boolean     succeeded = TRUE;
  boolean     precondFound = FALSE;
  char const  *pStr;
  uint32      preconds;

  /*  Default preconditions (in absence of override) is to use overall
   *  policy preconditions.
   */
  preconds = MRE_PRECOND_POLICY;

  /*  If there us a "precond" attribute, we'll use that.
   */
  pStr = mre_xml_get_attribute(pElem, "precond");
  if (NULL != pStr)
  {
    precondFound = TRUE;
    succeeded = mre_rule_parse_rule_preconds(MRE_POLICY_PRECONDTBL(pPolicy), &preconds, pStr);
    if (!succeeded)
    {
      goto Done;
    }
  }

  /*  If the deprecated "always_run" attribute is present *and* if there
   *  are no preconditions set, set preconditions to none.
   */
  pStr = mre_xml_get_attribute(pElem, "always_run");
  if (NULL != pStr)
  {
    if (precondFound)
    {
      MRE_MSG_ERROR_0("Rule has both 'always_run' and 'precond' attributes");
      succeeded = FALSE;
    }
    else if (strcasecmp(pStr, "true") == 0)
    {
      preconds = MRE_PRECOND_NONE;
    }
  }

Done:
  *pPreconds = preconds;
  return succeeded;
}


/*-------- mre_tag_is_cond --------*/
static boolean  mre_tag_is_cond(
  char const  *pTag
)
{
  return    strcasecmp(pTag, "conditions") == 0
         || strcasecmp(pTag, "cond") == 0
         ;
}


/*-------- mre_tag_is_then --------*/
static boolean  mre_tag_is_then(
  char const  *pTag
)
{
  return    strcasecmp(pTag, "actions") == 0
         || strcasecmp(pTag, "then") == 0
         ;
}


/*-------- mre_tag_is_else --------*/
static boolean  mre_tag_is_else(
  char const  *pTag
)
{
  return strcasecmp(pTag, "else") == 0;
}


/*-------- mre_rule_validate_rule_node --------*/
static boolean mre_rule_validate_rule_node(
  mre_xml_element_t const *pElem,
  size_t                  nChildren
)
{
  size_t  childIndex;
  boolean haveCond = FALSE;
  boolean haveThen = FALSE;
  boolean haveBadTag = FALSE;

  for (childIndex = 0 ; childIndex < nChildren ; ++childIndex)
  {
    mre_xml_element_t const *pChild;
    char const              *pStr;

    pChild = mre_xml_get_child(pElem, childIndex);
    pStr = mre_xml_get_tag(pChild);

    if (mre_tag_is_cond(pStr))
    {
      haveCond = TRUE;
    }
    else if (mre_tag_is_then(pStr))
    {
      haveThen = TRUE;
    }
    else if (!mre_tag_is_else(pStr))
    {
      MRE_UT_MSG_1("Illegal node in rule/if with tag: %s", pStr);
      haveBadTag = TRUE;
    }
  }

  if (!haveCond)
  {
    MRE_MSG_ERROR_0("A rule/if must have either a <conditions> or <cond> node");
  }

  if (!haveThen)
  {
    MRE_MSG_ERROR_0("A rule/if must have either aa <actions> or <then> node");
  }
    
  if (haveBadTag)
  {
    MRE_MSG_ERROR_0("Invalid node in rule/if. Allowed/required tags are:");
    MRE_MSG_ERROR_0("  <conditions> or <cond>");
    MRE_MSG_ERROR_0("  <actions> or <then>");
    MRE_MSG_ERROR_0("  (optionally) <else>");
  }
  
  return haveCond && haveThen && !haveBadTag;
}


/*-------- mre_rule_create --------*/
boolean mre_rule_create(
  mre_policy_t            *pPolicy,
  mre_xml_element_t const *pElem,
  mre_rule_t              **ppRule
)
{
  boolean     succeeded = FALSE;
  mre_rule_t  *pRule = NULL;
  size_t      nChildren;
  size_t      childIndex;

  nChildren = mre_xml_num_children(pElem);

  /*  Validate this node and bail if invalid.
   */
  if (!mre_rule_validate_rule_node(pElem, nChildren))
  {
    goto Done;
  }

  /*  Create the rule object for this node.
   */
  pRule = (mre_rule_t *) mre_mem_alloc(sizeof(mre_rule_t));
  ref_cnt_obj_init(pRule, mre_rule_dtor);
  pRule->pPolicy = pPolicy;

  /*  Process the subnodes of the rule.
   */
  succeeded = TRUE;
  for (childIndex = 0 ; succeeded && childIndex < nChildren ; ++childIndex)
  {
    mre_xml_element_t const *pChild;
    char const              *pStr;

    pChild = mre_xml_get_child(pElem, childIndex);
    pStr = mre_xml_get_tag(pChild);
    MRE_XML_MSG("processing %s", pStr, 0, 0);

    MRE_UT_MSG_1("Processing tag %s", pStr);
    if (mre_tag_is_cond(pStr))
    {
      succeeded = mre_rule_get_conditions(pPolicy, &pRule->pCondSet, pChild);
    }
    else if (mre_tag_is_then(pStr))
    {
      succeeded = mre_rule_get_actions(pPolicy, &pRule->pThenSet, pChild);
    }
    else if (mre_tag_is_else(pStr))
    {
      succeeded = mre_rule_get_actions(pPolicy, &pRule->pElseSet, pChild);
    }
  }

Done:
  if (!succeeded)
  {
    REF_CNT_OBJ_RELEASE_IF(pRule);
  }
  *ppRule = pRule;

  return succeeded;
}


/*-------- mre_ensure_ruleset --------*/
static mre_set_t  *mre_ensure_ruleset(
  mre_policy_t  *pPolicy
)
{
  mre_set_t   *pRuleSet;

  pRuleSet = MRE_POLICY_RULESET(pPolicy);
  if (pRuleSet == NULL)
  {
    pRuleSet = mre_set_refcnt_new(5, MRE_SET_NO_MAX_SIZE, NULL);
    MRE_POLICY_RULESET(pPolicy) = pRuleSet;
  }

  return pRuleSet;
}

/*-------- mre_ruleset_add --------*/
boolean mre_ruleset_add_rule(
  mre_policy_t            *pPolicy,
  mre_xml_element_t const *pElem
)
{
  boolean     succeeded = FALSE;
  mre_rule_t  *pRule = NULL;
  uint32      preconds;
  mre_set_t   *pRuleSet;

  /*  Get preconditions, bail if invalid.
   */
  succeeded = mre_rule_get_rule_preconditions(pPolicy, pElem, &preconds);
  if (!succeeded)
  {
    goto Done;
  }

  /*  Create a rule from the node.
   */
  succeeded = mre_rule_create(pPolicy, pElem, &pRule);
  if (!succeeded)
  {
    goto Done;
  }

  pRule->preconds = preconds;

  pRuleSet = mre_ensure_ruleset(pPolicy);
  mre_set_add(pRuleSet, &pRule);
  ref_cnt_obj_release(pRule);
  
Done:
  return succeeded;
}


/*-------- mre_ruleset_execute --------*/
void mre_ruleset_execute(
  mre_set_t *pRuleset,
  void      *pCtx,
  uint32    precondsMet
)
{
  (void) mre_set_iterate(pRuleset, process_rule, pCtx, (void *) precondsMet);
}


#endif