/**
  @file policyman_staticcfg_stubs.c

  @brief stubs for FEATURE_DISABLE_DYNAMIC_POLICIES enabled build
*/

/*
    Copyright (c) 2016 QUALCOMM Technologies Incorporated.
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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/src/policyman_staticcfg_stubs.c#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/
#ifdef FEATURE_DISABLE_DYNAMIC_POLICIES

#include "comdef.h"
#include "policyman_cfgitem.h"
#include "policyman_device_config.h"
#include "policyman_cm.h"
#include "policyman_dbg.h"
#include "policyman_i.h"
#include "policyman_task.h"
#include "trm.h"
#include "cfcm.h"
#include "policyman_rf.h"
#include "mre_xml.h"


/*-----------------------------------------------------------------------------
  Policyman suspend/resume framework
-----------------------------------------------------------------------------*/

/*-------- policyman_suspend --------*/
void policyman_suspend(
  void
)
{
  return;
}

/*-------- policyman_resume--------*/
void policyman_resume(
  void
)
{
  return;
}

/*-------- policyman_is_suspended --------*/
boolean policyman_policy_is_suspended(
  void
)
{
  return FALSE;
}

/*-------- policyman_get_subs_boolean_value --------*/
boolean policyman_get_subs_boolean_value(
  char const  *pBooleanName,
  sys_modem_as_id_e_type  subsId,
  boolean     *pValue
)
{
  return FALSE;
}


/*-------- policyman_get_boolean_value --------*/
boolean policyman_get_boolean_value(
  char const  *pBooleanName,
  boolean     *pValue
)
{
  return policyman_get_subs_boolean_value(pBooleanName, SYS_MODEM_AS_ID_1, pValue);
}

/*-------- policyman_svd_possible_on_plmn --------*/
/**
@brief  Function to determine if SVD is possible on PLMN
*/
policyman_status_t policyman_svd_possible_on_plmn(
  sys_plmn_id_s_type  *pPlmnId,
  boolean              mccOnly,
  boolean             *pSvdPossible
)
{
  *pSvdPossible = FALSE;
  return POLICYMAN_STATUS_SUCCESS;
   
}

/*-------- policyman_is_svd_allowed --------*/
policyman_status_t policyman_is_svd_allowed(
  boolean *pIsSvd
)
{

  *pIsSvd = FALSE;
  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_is_svd_operation_allowed --------*/
boolean policyman_is_svd_operation_allowed(
  sys_modem_as_id_e_type subsId
)
{
  return FALSE;
}

void policyman_cm_serving_info_cb(
  policyman_cm_serving_info_t *pCmServingInfo
)
{ 
  return;
}

/*-------- policyman_report_preference_change --------*/

msgr_hdr_s *policyman_report_preference_change(
  sys_modem_as_id_e_type               asubs_id,
  cm_sys_sel_pref_params_s_type const *pPref
)
{
  return NULL;
}

/*-------- policyman_compute_policy_with_newpref --------*/

msgr_hdr_s *policyman_compute_policy_with_newpref(
  sys_modem_as_id_e_type               asubs_id,
  cm_sys_sel_pref_params_s_type const *pPref
)
{
  return NULL;
}

/*-------- policyman_report_preference_msg_free --------*/
void policyman_report_preference_msg_free(
  msgr_hdr_s  *pMsg
)
{
  return;
}

/*-------- policyman_report_multimode_subs --------*/
msgr_hdr_s *policyman_report_multimode_subs(
  uint8 active_subs_mask
)
{
  return NULL;
}

/*-------- policyman_get_persisted_user_plmn --------*/
boolean policyman_get_persisted_user_plmn(
  sys_modem_as_id_e_type   subs,
  boolean                  clear,
  sys_plmn_id_s_type      *pPlmn
)
{
  return FALSE;
}

/*-------- policyman_msim_report_subs_info --------*/
msgr_hdr_s * policyman_msim_report_subs_info(
  dds_info_t const *pSubsInfo,
  uint8             active_subs_mask
)
{
  return NULL;
}


/*-------- policyman_report_acq_fail --------*/
void policyman_report_acq_fail(
  sys_modem_as_id_e_type         subs_id,
  sys_modem_stack_id_e_type      stack_id
)
{
  return;
}

/*-------- policyman_report_operational_radio_stacks --------*/
void policyman_report_operational_radio_stacks(
  sys_modem_as_id_e_type subs_id,
  uint32                 stackMask
)
{
  return;
}

/*-------- policyman_efs_obliterate_subs --------*/
int policyman_efs_obliterate_subs(
  sys_modem_as_id_e_type  subsId
)
{
  return 0;
}

/*-------- policyman_report_plmn_candidate --------*/
msgr_hdr_s * policyman_report_plmn_candidate(
  sys_modem_as_id_e_type  subsId,
  sys_plmn_id_s_type     *pPlmn
)
{
  return NULL;
}

/*-------- policyman_device_config_set_explicit_index --------*/
policyman_status_t policyman_device_config_set_explicit_index(
  int  explicitIndex
)
{
    return POLICYMAN_STATUS_ERR_INVALID_ARGS;
}

/*-------- policyman_device_config_get_explicit_index --------*/
policyman_status_t policyman_device_config_get_explicit_index(
  int *pExplicitIndex
)
{
  *pExplicitIndex = 0;
  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_state_get_imsi_verified --------*/
boolean policyman_state_get_imsi_verified(
  sys_modem_as_id_e_type  subsId
)
{
  return TRUE;
}

void policyman_state_init(
  void
)
{
  return;
}

void policyman_state_deinit(
  void
)
{
  return;
}

policyman_state_t* policyman_state_get_state(
  void
)
{
  return NULL;
}

uint8 policyman_state_get_trm_cap(
  void
)
{
  trm_get_info_input_type     input;
  trm_get_info_return_type    output;

  input.client_id = TRM_CM;
  input.info_type = TRM_SIMUL_CAPABILITY;

  /**
   * For input we are giving client id and info type.
   * TRM will provide simulataneos capability in output.info.simul_cap_info
  **/
  trm_get_info(&input, &output);
  
  return output.info.simul_cap_info;
}


void policyman_policy_init(
  policyman_state_t* pState
)
{
  return;
}

void policyman_policy_deinit(
  void
)
{
  return;
}

void policyman_cfgitem_ensure_device_config(
  void
)
{
  return;
}

void policyman_ensure_configurations(
  void
)
{
  return;
}

void policyman_efs_initialize(
  void
)
{
  return;
}

void policyman_cfgitem_p_init(
  void
)
{
  return;
}

void policyman_persist_cfgitems(
  policyman_set_t *pNotifySet
)
{
  return;
}

void policyman_cfgitem_p_deinit(
  void
)
{
  return;
}

void policyman_cfgitem_send_diag_logs(  
  mre_set_t    *pNotifySet
)
{
  return;
}
void policyman_register_for_mcfg_refresh(
  void
)
{
  return;
}
void policyman_register_for_cm_events(
  void
)
{
  return;
}
void policyman_register_for_uim_events(
  void
)
{
  return;
}

boolean policyman_retrieve_call_mode(
  sys_call_mode_t         *callMode,
  sys_modem_as_id_e_type   asubs_id
)
{

  *callMode = SYS_CALL_MODE_NORMAL;

  return TRUE;
}
/*-------- policyman_state_get_thermal_rat_disable_mask --------*/
uint32 policyman_state_get_thermal_rat_disable_mask(
  void
)
{

  return SYS_SYS_MODE_MASK_NONE;
}
/*-------- policyman_state_get_simlock_verified --------*/
boolean policyman_state_get_simlock_verified(
  sys_modem_as_id_e_type  subsId
)
{
  return TRUE;
}

/*-------- policyman_get_call_mode --------*/
sys_call_mode_t policyman_get_call_mode(
  sys_modem_as_id_e_type   subsId,
  sys_plmn_id_s_type      *pCampedPLMN
)
{
  return SYS_CALL_MODE_NORMAL;
}

/*-------- policyman_get_current_mcc --------*/
policyman_status_t policyman_get_current_mcc(
  sys_modem_as_id_e_type subsId,
  sys_mcc_type           *pMcc
)
{

  return POLICYMAN_STATUS_ERR_CONFIG_FAILURE;
}
/*-------- policyman_report_thermal_monitor_status --------*/
msgr_hdr_s *policyman_report_thermal_state(
  cfcm_cmd_type_s const *pCfcm_cmd
  )
{
  return NULL;
}

/*-------- policyman_set_periodic_plmn_search_allowed--------*/
void policyman_set_periodic_plmn_search_allowed(
   sys_modem_as_id_e_type  subsId,
   boolean                 isAllowed
)
{
  return;
}

/*-------- policyman_set_hlos_mcc --------*/
policyman_status_t policyman_set_hlos_mcc(
  sys_mcc_type  mcc,
  size_t        confidenceLevel,
  size_t        status
)
{
  return POLICYMAN_STATUS_SUCCESS;
}

boolean
policyman_state_get_rf_info(
  policyman_state_t const     *pState,
  size_t                      device,
  policyman_rf_device_info_t  *pInfo
  )
{
  return policyman_rf_get_device_info(NULL, device, pInfo);
}



/*-------- device_config_get_subphone_index --------*/
STATIC size_t device_config_get_subphone_index(
 size_t  num_sims,
 size_t  nCfg,
 size_t  nSim
)
{
  return  nSim;
}

/*-------- device_config_get_base_config --------*/
uint32 device_config_get_base_config(
  device_configuration_t const  *pdc,
  size_t                        nCfg,
  size_t                        nSim
)
{
   return policyman_rf_get_overall_rat_mask();
}

/*-------- device_config_get_config --------*/
uint32 device_config_get_config(
  device_configuration_t const  *pdc,
  size_t                        nCfg,
  size_t                        nSim
  )
{
  return policyman_rf_get_overall_rat_mask();
}

/*-------- device_config_get_current_config --------*/
uint32 device_config_get_current_config(
  device_configuration_t const  *pdc,
  size_t                         nSim
)
{
  return policyman_rf_get_overall_rat_mask();
}

/*-------- device_config_get_subs_feature --------*/
sys_subs_feature_t device_config_get_subs_feature(
  device_configuration_t const  *pdc,
  size_t                        nCfg,
  size_t                        nSim
)
{
  return SYS_SUBS_FEATURE_MODE_NORMAL;
}

/*-------- policyman_device_config_num_sims --------*/
policyman_status_t policyman_device_config_num_sims(
  policyman_item_t const  *pItem,
  size_t                  *pnSims
)
{
  *pnSims = 1;
  return POLICYMAN_STATUS_SUCCESS;
}


/*-------- policyman_device_config_max_active_voice --------*/
policyman_status_t policyman_device_config_max_active_voice(
  policyman_item_t const  *pItem,
  size_t                  *pnMaxActive
)
{
  *pnMaxActive = 1 ;
  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_device_config_max_active_data --------*/
policyman_status_t policyman_device_config_max_active_data(
  policyman_item_t const  *pItem,
  size_t                  *pnMaxActiveData
)
{
  *pnMaxActiveData = 1;
  return POLICYMAN_STATUS_SUCCESS;
}


/*-------- policyman_device_overall_config_feature --------*/
policyman_status_t policyman_device_config_overall_feature(
  policyman_item_t const  *pItem,
  sys_overall_feature_t   *pFeature
)
{
  *pFeature = SYS_OVERALL_FEATURE_MODE_NORMAL;
  return POLICYMAN_STATUS_SUCCESS;
}


/*-------- policyman_device_config_num_cfgs --------*/
policyman_status_t policyman_device_config_num_cfgs(
  policyman_item_t const  *pItem,
  size_t                  *pnCfgs
)
{

 *pnCfgs = 1;
  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_device_config_get_config --------*/
policyman_status_t policyman_device_config_get_config(
  policyman_item_t const  *pItem,
  size_t                  nCfg,
  size_t                  nSim,
  uint32                  *pRatCfg
)
{
  *pRatCfg = policyman_rf_get_overall_rat_mask();
  return POLICYMAN_STATUS_SUCCESS;

}


/*-------- policyman_device_config_get_subs_feature --------*/
policyman_status_t policyman_device_config_get_subs_feature(
  policyman_item_t const  *pItem,
  size_t                  nCfg,
  size_t                  nSim,
  sys_subs_feature_t      *pFeature
)
{

  *pFeature = SYS_SUBS_FEATURE_MODE_NORMAL;
  return POLICYMAN_STATUS_SUCCESS;
}


/*-------- policyman_device_config_get_specialization --------*/
policyman_status_t policyman_device_config_get_specialization(
  policyman_item_t const  *pItem,
  sys_specialization_t    *pSpecial
)
{
  *pSpecial = SYS_SPECIAL_NONE;
  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_get_current_num_sim --------*/
policyman_status_t policyman_get_current_num_sim(
  size_t *pnSim
)
{
 *pnSim = 1;
  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_device_config_get_max_capability --------*/
policyman_status_t policyman_device_config_get_max_capability(
  policyman_item_t const  *pItem,
  size_t                   nSim,
  uint32                  *pRatCfg
)
{

  *pRatCfg = policyman_rf_get_overall_rat_mask();
  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_device_config_get_current_subphone_capability --------*/
policyman_status_t policyman_device_config_get_current_subphone_capability(
  policyman_item_t const  *pItem,
  size_t                   nSim,
  uint32                  *pRatCfg
)
{
  *pRatCfg = policyman_rf_get_overall_rat_mask();
  return POLICYMAN_STATUS_SUCCESS;

}

/*-------- policyman_get_e911_subphone_capability --------*/
policyman_status_t policyman_get_e911_subphone_capability(
  policyman_item_t const  *pItem,
  size_t                   numItems,
  uint32                  *pRatArray
)
{
  pRatArray[0] = policyman_rf_get_overall_rat_mask();
  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_device_config_get_current_config_index --------*/
policyman_status_t policyman_device_config_get_current_config_index(
  policyman_item_t const  *pItem,
  size_t                  *pIndex
)
{
 *pIndex = 0;
  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_device_config_max_active_subs --------*/
policyman_status_t policyman_device_config_max_active_subs(
  policyman_item_t const  *pItem,
  size_t                  *pnMaxActive
)
{
 *pnMaxActive = 1;

  return POLICYMAN_STATUS_SUCCESS;
}
/*-------- policyman_get_items_since_sequence_number --------*/
policyman_item_collection_t *policyman_get_items_since_sequence_number(
  size_t  seqno
)
{
  return NULL;
}


/*-------- mre_xml_parse_from_text --------*/
mre_status_t mre_xml_parse_from_text(
  const char            *pDocument,
  const mre_xml_root_t **ppRoot
)
{
  return MRE_STATUS_ERR_NOT_PRESENT;
}

/*-------- mre_xml_get_root_element --------*/
const mre_xml_element_t *
mre_xml_get_root_element(
  mre_xml_root_t const  *pRoot
  )
{
  return NULL;
}

/*-------- mre_xml_get_tag --------*/
char const *
mre_xml_get_tag(
  mre_xml_element_t const *pElement
  )
{
 
  return NULL;
}

/*-------- mre_xml_get_text_from_child --------*/
char const *mre_xml_get_text_from_child(
  mre_xml_element_t const *pElem,
  char const              *pTag
)
{
  return NULL;
}


#endif