#ifndef _POLICYMAN_SERVING_SYSTEM_H_
#define _POLICYMAN_SERVING_SYSTEM_H_

/**
  @file policyman_serving_system.h

  @brief  
*/

/*
    Copyright (c) 2013-2016 QUALCOMM Technologies Incorporated.
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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/inc/policyman_serving_system.h#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/

#include "policyman_i.h"
#include "sys.h"
#include "cm.h"

typedef struct policyman_ss_info_t  policyman_ss_info_t;


boolean
policyman_ss_init(
  sys_modem_as_id_e_type  subs,
  policyman_ss_info_t     **ppInfo
  );

void
policyman_ss_deinit(
  policyman_ss_info_t *pInfo
  );
  

uint32
policyman_ss_get_mcc(
  policyman_ss_info_t       *pInfo,
  sys_modem_stack_id_e_type num_stack
  );

void
policyman_ss_get_plmn(
  policyman_ss_info_t       *pInfo,
  sys_modem_stack_id_e_type num_stack,
  sys_plmn_id_s_type        *pPlmn
  );

boolean
policyman_ss_set_plmn(
  policyman_ss_info_t       *pInfo,
  sys_modem_stack_id_e_type num_stack,
  sys_plmn_id_s_type        *pPlmn
  );
  
void policyman_ss_set_stack_opr(
  sys_modem_as_id_e_type subsId,
  sys_modem_stack_id_e_type stackId,
  boolean isOperational
  );

/*===========================================================================
  FUNCTION POLICYMAN_SS_GET_SRV_STATUS()
 
  DESCRIPTION
    GET srv_status for stack
 ===========================================================================*/
sys_srv_status_e_type
policyman_ss_get_srv_status
(  
  policyman_ss_info_t       *pInfo, 
  sys_modem_stack_id_e_type num_stack
);

/*===========================================================================
  FUNCTION POLICYMAN_SS_SET_SRV_STATUS()
 
  DESCRIPTION
    SET srv_status for  stack into PolicyMan state

 RETURN VALUE
   TRUE is policyman state is updated correctly, FALSE otherwise
 ===========================================================================*/
boolean policyman_ss_set_srv_status
(  
  policyman_ss_info_t       *pInfo, 
  sys_modem_stack_id_e_type num_stack,
  sys_srv_status_e_type     srv_status
);

/*===========================================================================
  FUNCTION POLICYMAN_SS_UPDATE_GW_HYBR_OPERATIONAL()
 
  DESCRIPTION
    update for gw_hybr operational flag into PolicyMan state
 ===========================================================================*/
void policyman_ss_update_gw_hybr_operational
(  
  policyman_ss_info_t *pInfo, 
  boolean state
);


/*-------- policyman_ss_get_ss_info --------*/
policyman_ss_info_t *policyman_ss_get_ss_info(
  sys_modem_as_id_e_type subsId
);

/*-------- policyman_condition_serving_plmn_new --------*/
mre_status_t
policyman_condition_serving_plmn_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  );

/*-------- policyman_condition_enforce_full_rat_new --------*/
mre_status_t policyman_condition_enforce_full_rat_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  );

/*-------- policyman_condition_oos_scan_cnt_new --------*/
mre_status_t policyman_condition_oos_scan_cnt_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
 );

/*-------- policyman_condition_location_mcc_new --------*/
mre_status_t policyman_condition_location_mcc_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t              **ppCondition
);

/*-------- policyman_condition_have_service_new --------*/
mre_status_t policyman_condition_have_service_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t              **ppCondition
);

/*===========================================================================
  FUNCTION POLICYMAN_MSIM_CMSS_EVENT_CB()

  DESCRIPTION
    Handle CM SS event callback function for Multisim

  PARAMETERS
    evt     : CM SS EVENT type
    p_info : actual payload of CM SS EVENT
===========================================================================*/
void policyman_msim_cmss_event_cb
( 
  cm_ss_event_e_type               evt, 
  const cm_mm_ss_info_s_type      *p_info
);


/*-------- policyman_ss_update_state --------*/
/**
@brief  Handle a serving system state update.

@param

@return
  
*/
void
policyman_ss_update_state(
    policyman_ss_info_t *pCurrentInfo,
    policyman_ss_info_t *pNewInfo,
    sys_modem_as_id_e_type  asubs_id
    );

/*===========================================================================
  FUNCTION POLICYMAN_SS_GET_PLMN_ID_TYPE()

  DESCRIPTION
    GET PLMN ID type for MAIN stack
 ===========================================================================*/
sys_sys_id_type_e_type policyman_ss_get_plmn_id_type(
  policyman_ss_info_t       *pSsInfo,
  sys_modem_stack_id_e_type num_stack
);

/*===========================================================================
  FUNCTION POLICYMAN_SS_SET_DEFAULT_VALUE()

  DESCRIPTION
    Set Default value on both stacks
 ===========================================================================*/
void policyman_ss_set_default_value(
  policyman_ss_info_t       *pInfo,
  sys_modem_stack_id_e_type  num_stack
);


/*===========================================================================
  FUNCTION POLICYMAN_MSIM_NON_MULTIMODE_SUBS_IN_SERVICE()

  DESCRIPTION
    Check if non-multimode subs has any service on its stacks
 ===========================================================================*/
boolean policyman_msim_non_multimode_subs_in_service(
  void
);

/*===========================================================================
  FUNCTION POLICYMAN_MSIM_NON_MULTIMODE_SUBS_HAS_SGLTE_MCC()

  DESCRIPTION
    Check if non-multimode subs has SGLTE MCC
 ===========================================================================*/
boolean policyman_msim_non_multimode_subs_has_sglte_mcc(
  void
);

/*-------- policyman_ss_subs_is_in_pwr_save --------*/
boolean policyman_ss_subs_is_in_pwr_save(
  sys_modem_as_id_e_type subsId
);

/*-------- policyman_get_current_mcc --------*/
policyman_status_t policyman_get_current_mcc(
  sys_modem_as_id_e_type subsId,
  sys_mcc_type           *pMcc
);

#endif /* _POLICYMAN_SERVING_SYSTEM_H_ */
