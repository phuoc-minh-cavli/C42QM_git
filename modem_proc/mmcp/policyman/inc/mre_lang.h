#ifndef _MRE_LANG_H_
#define _MRE_LANG_H_

/**
  @file mre_lang.h

  @brief  Code for the "language" used in the Modem Rules Engine XML.
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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/inc/mre_lang.h#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/

#include "mre_xml.h"


/*=============================================================================
  Conditions
=============================================================================*/

mre_status_t mre_condition_true_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_condition_t         **ppCondition
);


mre_status_t mre_boolean_test_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_condition_t         **ppCondition
);


mre_status_t mre_condition_not_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_condition_t         **ppCondition
);


mre_status_t mre_condition_or_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_condition_t         **ppCondition
);

mre_status_t mre_condition_and_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_condition_t         **ppCondition
);


/*=============================================================================
  Actions
=============================================================================*/

mre_status_t mre_continue_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_action_t            **ppAction
);


mre_status_t mre_boolean_define_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_action_t            **ppAction
);


mre_status_t mre_boolean_set_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_action_t            **ppAction
);

mre_status_t mre_if_action_new(
  mre_xml_element_t const *pElem,
  mre_policy_t            *pPolicy,
  mre_action_t            **ppAction
 );

#endif /* _MRE_LANG_H_ */
