#ifndef _POLICYMAN_PLMN_H_
#define _POLICYMAN_PLMN_H_

/**
  @file policyman_plmn.h

  @brief  
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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/inc/policyman_plmn.h#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/

#include "policyman_i.h"
#include "sys.h"



/*-------- policyman_parse_ascii_plmn --------*/
/**
@brief  

@param

@return
  
*/
boolean
policyman_parse_ascii_plmn(
  char const          *plmnStr,
  sys_plmn_id_s_type  *pPlmn
  );


/*-------- policyman_plmn_print --------*/
/**
@brief  

@param

@return
  
*/
void
policyman_plmn_print(
  sys_plmn_id_s_type  *pPlmn
  );


/*-------- policyman_plmns_are_equal --------*/
/**
@brief  

@param

@return
  
*/
boolean
policyman_plmns_are_equal(
  sys_plmn_id_s_type  *pPlmn1,
  sys_plmn_id_s_type  *pPlmn2
  );



/*-------- policyman_plmn_get_mcc --------*/
/**
@brief  

@param

@return
  
*/
uint32
policyman_plmn_get_mcc(
  sys_plmn_id_s_type  *pPlmn
  );


/*-------- policyman_plmn_list_read --------*/
/**
@brief  

@param

@return
  
*/
policyman_set_t *
policyman_plmn_list_read(
  char const  *pStr
  );


/*-------- policyman_plmn_list_new --------*/
/**
@brief  

@param

@return
  
*/
mre_status_t
policyman_plmn_list_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  );


/*-------- policyman_mcc_list_read --------*/
/**
@brief  

@param

@return
  
*/
policyman_set_t *
policyman_mcc_list_read(
  char const  *pStr
  );


/*-------- policyman_mcc_list_new --------*/
/**
@brief  

@param

@return
  
*/
mre_status_t
policyman_mcc_list_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  );


/*-------- policyman_plmn_from_mcc_mnc --------*/
/**
@brief  

@param

@return
  
*/
void
policyman_plmn_from_mcc_mnc(
  sys_plmn_id_s_type  *pPlmn,
  uint32              mcc,
  uint32              mnc
  );

/*-------- policyman_plmn_print --------*/
/**
@brief  

@param

@return
  
*/
void policyman_plmn_print(
  sys_plmn_id_s_type *pPlmn
);


/*-------- policyman_plmn_clear --------*/
/**
@brief  

@param

@return
  
*/
void
policyman_plmn_clear(
  sys_plmn_id_s_type  *pPlmn
  );

/*-------- policyman_plmn_mcc_new --------*/
/**
@brief  

@param

@return
  
*/
mre_status_t
policyman_plmn_mcc_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition,
  pfn_evaluate_t                evaluate
  );

/*-------- policyman_plmn_mcc_is_volte_only --------*/
boolean policyman_plmn_mcc_is_volte_only(
  sys_mcc_type            mcc,
  sys_modem_as_id_e_type  subs
);

#endif /* _POLICYMAN_PLMN_H_ */
