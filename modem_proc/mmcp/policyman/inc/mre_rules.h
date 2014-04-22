#ifndef _MRE_RULES_H_
#define _MRE_RULES_H_

/**
  @file mre_rules.h

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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/inc/mre_rules.h#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/


#include "mre.h"


typedef struct mre_rule_t mre_rule_t;


/*=============================================================================
  APIs to operate on rulesets
=============================================================================*/


/*-------- mre_rule_add_condition --------*/
/**
@brief  

@param

@return
  
*/
boolean mre_rule_add_condition(
  mre_policy_t            *pPolicy,
  mre_set_t               *pCondSet,
  mre_xml_element_t const *pElem
);


/*-------- mre_condset_add --------*/
/**
@brief  

@param

@return
  
*/
boolean mre_condset_add(
  mre_set_t               **ppCondSet,
  mre_xml_element_t const *pElem
);


/*-------- mre_condset_evaluate --------*/
/**
@brief  

@param

@return
  
*/
boolean mre_condset_evaluate(
  mre_set_t const *pCondSet,
  void            *pCtx
);


/*-------- mre_initset_add --------*/
/**
@brief  

@param

@return
  
*/
boolean mre_initset_add(
  mre_policy_t            *pPolicy,
  mre_xml_element_t const *pElem
);



/*-------- mre_actionset_execute --------*/
/**
@brief  

@param

@return
  
*/
void mre_actionset_execute(
  mre_set_t const *pActionSet,
  void            *pCtx
);



/*-------- mre_ruleset_add_rule --------*/
/**
@brief  

@param

@return
  
*/
boolean mre_ruleset_add_rule(
  mre_policy_t            *pPolicy,
  mre_xml_element_t const *pElem
);



/*-------- mre_ruleset_execute --------*/
/**
@brief  

@param

@return
  
*/
void mre_ruleset_execute(
  mre_set_t *pRuleSet,
  void      *pCtx,
  uint32    precondsMet
);


/*-------- mre_rules_continue_evaluation --------*/
/**
@brief  Continue rule evaluation despite having all rule conditions met.

This is specifically for the implementation of the <continue> action, so that
rule evaluation can continue despite having executed actions.

@param

@return
  
*/
void mre_rules_continue_evaluation(
  mre_policy_t  *pPolicy
);


/*-------- mre_rule_create --------*/
/**
@brief  

@param

@return
  
*/
boolean mre_rule_create(
  mre_policy_t            *pPolicy,
  mre_xml_element_t const *pElem,
  mre_rule_t              **ppRule
);


/*-------- mre_rule_evaluate --------*/
/**
@brief  

@param

@return
  
*/
void mre_rule_evaluate(
  mre_rule_t const  *pRule,
  mre_policy_t      *pPolicy,
  void              *pCtx
);


#endif /* _MRE_RULES_H_ */
