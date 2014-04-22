/**
  @file policyman_state.c

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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/src/policyman_state.c#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES

#include "mre_efs.h"
#include "policyman_cfgitem.h"
#include "policyman_device_config.h"
#include "policyman_efs.h"
#include "policyman_phone_events.h"
#include "policyman_plmn.h"
#include "policyman_policy.h"
#include "policyman_serving_system.h"
#include "policyman_set.h"
#include "policyman_state.h"
#include "policyman_subs.h"
#include "policyman_task.h"
#include "policyman_timer.h"
#include "policyman_util.h"
#include "policyman_uim.h"
#include "trm.h"


#include <stringl/stringl.h>



static policyman_device_info_t policyman_device_info;
static policyman_subs_state_t  policyman_subs_state_info[POLICYMAN_NUM_SUBS];
static policyman_global_info_t policyman_global_info;

static policyman_state_t  policyman_state;

static boolean device_calibrated;

void  policyman_state_get_imei(void);


/*=============================================================================
  General state APIs
=============================================================================*/

/*-------- policyman_state_retrieve_persisted_current_mcc --------*/
STATIC boolean policyman_state_retrieve_persisted_current_mcc(
  sys_mcc_type           *pCurrentMcc,
  sys_modem_as_id_e_type  subsId
)
{
  mre_efs_status_t      status;
  sys_mcc_type         *pMcc       = NULL;
  size_t                mccSize    = 0;

  /* Initialize Current MCC to 0
   */
  *pCurrentMcc = 0;

  /*  Try and read from the NV item for Current MCC.
   */
  status = mre_efs_get_file( POLICYMAN_CURRENT_MCC_NV_PATH,
                             (void **)&pMcc,
                             &mccSize,
                             subsId);

  /*  If we were able to read the file set the MCC to that value.
         Otherwise just return.
   */
  if (    status == MRE_EFS_STATUS_SUCCESS
      &&  mccSize == sizeof(sys_mcc_type)
      &&  pMcc != NULL
     )
  {
    *pCurrentMcc = *pMcc;
    POLICYMAN_MSG_HIGH_2("current_mcc %d for subs %d read from EFS", 
                         *pCurrentMcc,
                          subsId);
  }

  POLICYMAN_MEM_FREE_IF(pMcc);
  return *pCurrentMcc != 0;
}

/*-------- policyman_state_init --------*/
void policyman_state_init(
  void
)
{
  size_t subs;

  /* Init Policyman device state, subs state and global state
  */
  policyman_state.pDeviceInfo = &policyman_device_info;
  memset(policyman_state.pDeviceInfo, 0x00, sizeof(policyman_device_info_t));

  policyman_state.pGlobalInfo = &policyman_global_info;
  memset(policyman_state.pGlobalInfo, 0x00, sizeof(policyman_global_info_t));

  policyman_state_clear_hlos_mcc();

  for(subs = 0; subs < POLICYMAN_NUM_SUBS; subs++)
  {
    policyman_state.pSubsState[subs] = &policyman_subs_state_info[subs];
    memset(policyman_state.pSubsState[subs], 0x00, sizeof(policyman_subs_state_t));    
    policyman_state.pSubsState[subs]->simLockVerified = TRUE;
  }

  policyman_state_get_imei();

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
  (void) policyman_state_rf_init();
#endif
  (void) policyman_state_trm_cap_init();

  /* Check for rat capability NONE - indicates device is not calibrated, nothing should be persisted */
  device_calibrated = (policyman_rf_get_overall_rat_mask() != SYS_SYS_MODE_MASK_NONE);

  (void) policyman_state_ss_init();
  (void) policyman_state_uim_init();
  (void) policyman_state_network_init();

  /* Do SUBS init before Phone init since we need memory for subscription allocated before we
       evaluate multimode subscription
    */
  (void) policyman_state_subs_init();
  (void) policyman_state_cmph_init();
}


void
policyman_state_deinit(
  void
  )
{
  size_t i;

  POLICYMAN_MEM_FREE_IF(policyman_device_info.pIMEI);

  policyman_subs_deinit(policyman_state.pDeviceInfo->pSubsInfo);
#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
  policyman_rf_deinit(policyman_state.pDeviceInfo->pRfInfo);
#endif
  for(i=0; i<POLICYMAN_NUM_SUBS; i++)
  {
    policyman_cmph_deinit(policyman_state.pSubsState[i]->pCmPhInfo);
    policyman_network_deinit(policyman_state.pSubsState[i]->pNetworkInfo);
    policyman_uim_deinit(policyman_state.pSubsState[i]->pUimInfo);
    policyman_ss_deinit(policyman_state.pSubsState[i]->pSsInfo);
  }

  memset(&policyman_state, 0, sizeof(policyman_state));
}


/*-------- policyman_state_get_state --------*/
policyman_state_t *
policyman_state_get_state(
  void
  )
{
  return &policyman_state;
}


/*-------- policyman_state_get_subs_state --------*/
policyman_subs_state_t *
policyman_state_get_subs_state(
  sys_modem_as_id_e_type  asubs_id
  )
{
  if (asubs_id >= ARR_SIZE(policyman_state.pSubsState))
  {
    POLICYMAN_ERR_FATAL("subs ID %d exceeds pSubsState array bounds", asubs_id, 0, 0);
  }
  return policyman_state.pSubsState[asubs_id];
}

/*-------- policyman_state_get_serving_system_info --------*/
policyman_ss_info_t *policyman_state_get_serving_system_info(
  policyman_state_t       *pState,
  sys_modem_as_id_e_type   subsId
)
{
  return pState->pSubsState[subsId]->pSsInfo;
}


/*-------- policyman_state_get_subs_precond --------*/
uint32
policyman_state_get_subs_precond(
  sys_modem_as_id_e_type  asubs_id
  )
{
  return policyman_state_get_subs_state(asubs_id)->preconditions_met;
}

/*-------- policyman_state_get_subs_info --------*/
policyman_subs_info_t *
policyman_state_get_subs_info(
  void
  )
{
  return policyman_state.pDeviceInfo->pSubsInfo;
}



/*-------- policyman_state_check_precondition_met --------*/
boolean
policyman_state_check_precondition_met(
  uint32                  precondition,
  sys_modem_as_id_e_type  asubs_id
  )
{
  uint32  curPrecond = policyman_state_get_subs_precond(asubs_id);

  POLICYMAN_MSG_MED_3("subs %d: preconditons met: 0x%02x, checking for 0x%02x",
                     asubs_id, curPrecond, precondition);
  return ((curPrecond & precondition) == precondition) ;
}


/*-------- policyman_state_set_precondition_met --------*/
void
policyman_state_set_precondition_met(
  uint32                  precondition,
  sys_modem_as_id_e_type  asubs_id
  )
{
  uint32  oldPrecond = policyman_state_get_subs_precond(asubs_id);
  uint32  newPrecond = oldPrecond | precondition;

  policyman_state_get_subs_state(asubs_id)->preconditions_met = newPrecond;

  if (oldPrecond != newPrecond)
  {
    POLICYMAN_MSG_HIGH_3( "Preconditions changed for subs %d from %d to %d",
                          asubs_id,
                          oldPrecond,
                          newPrecond );
  }
}

/*-------- policyman_state_reset_precondition_met --------*/
void
policyman_state_reset_precondition_met(
  uint32                  precondition,
  sys_modem_as_id_e_type  asubs_id
  )
{
  uint32  oldPrecond = policyman_state_get_subs_precond(asubs_id);
  uint32  newPrecond = oldPrecond & ~precondition;

  policyman_state_get_subs_state(asubs_id)->preconditions_met = newPrecond;

  if (oldPrecond != newPrecond)
  {
    POLICYMAN_MSG_HIGH_3( "Preconditions changed for subs %d from %d to %d",
                          asubs_id,
                          oldPrecond,
                          newPrecond );
  }
}


/*================================================================================
  Handle updates in PolicyMan state updates
==================================================================================*/

/*-------- policyman_state_handle_update --------*/
void policyman_state_handle_update(
  sys_modem_as_id_e_type asubs_id
)
{
  policyman_set_t *pItemSet = NULL;

  /* Don't run rules in test mode or FTM
   */
  if(  policyman_is_test_mode()
    || policyman_is_FTM()
    )
  {
    POLICYMAN_MSG_HIGH_0("test mode is enabled, no rule execution done");
    return;
  }

  pItemSet = policyman_itemset_new();
  policyman_policy_run_policy_check(&policyman_state, asubs_id, pItemSet);
  policyman_cfgitem_update_items(pItemSet);
  ref_cnt_obj_release(pItemSet);
}


/*=============================================================================
  RF-related state APIs
=============================================================================*/
#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
boolean
policyman_state_rf_init(
  void
  )
{
  return policyman_rf_init(&policyman_state.pDeviceInfo->pRfInfo);
}
#endif
void policyman_state_trm_cap_init(
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

  policyman_state.pDeviceInfo->trm_cap = (output.info_type == input.info_type) ? output.info.simul_cap_info : 0;

  POLICYMAN_MSG_HIGH_2("trm_get_info() for TRM_SIMUL_CAPABILITY returned %d with info_type %d", output.info.simul_cap_info, output.info_type);

}



boolean
policyman_state_get_rf_info(
  policyman_state_t const     *pState,
  size_t                      device,
  policyman_rf_device_info_t  *pInfo
  )
{
#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
  return ( pState != NULL
           && pState->pDeviceInfo != NULL)?
              policyman_rf_get_device_info(pState->pDeviceInfo->pRfInfo, device, pInfo)
           :  FALSE;
#else
  return policyman_rf_get_device_info(NULL, device, pInfo);
#endif
}

boolean
policyman_state_device_calibrated(
  void
  )
{
  return device_calibrated;
}

/*=============================================================================
  Serving-system related APIs
=============================================================================*/
void policyman_state_ss_init(
  void
)
{
  sys_modem_as_id_e_type  subs;

  for (subs = 0; subs < POLICYMAN_NUM_SUBS; subs++)
  {
    sys_mcc_type mcc = NO_MCC;
  
    policyman_ss_init(subs, &policyman_state_get_subs_state(subs)->pSsInfo);
    policyman_state_retrieve_persisted_current_mcc(&mcc, subs);
    policyman_state_set_subs_mcc(subs, mcc);
  }
}

uint32 policyman_state_get_serving_mcc(
  policyman_state_t const   *pState,
  sys_modem_stack_id_e_type num_stack,
  sys_modem_as_id_e_type    asubs_id
  )
{
  return policyman_ss_get_mcc(pState->pSubsState[asubs_id]->pSsInfo, num_stack);
}

void
policyman_state_get_serving_plmn(
  policyman_state_t const   *pState,
  sys_modem_stack_id_e_type num_stack,
  sys_modem_as_id_e_type    asubs_id,
  sys_plmn_id_s_type        *pPlmn
  )
{
  policyman_ss_get_plmn(pState->pSubsState[asubs_id]->pSsInfo, num_stack, pPlmn);
}


/*===========================================================================
  FUNCTION POLICYMAN_STATE_GET_SRV_STATUS()

  DESCRIPTION
    Get srv_status for stack from PolicyMan state
 ===========================================================================*/
sys_srv_status_e_type
policyman_state_get_srv_status
(
  policyman_state_t const   *pState,
  sys_modem_stack_id_e_type num_stack,
  sys_modem_as_id_e_type    asubs_id
)
{
  return policyman_ss_get_srv_status(pState->pSubsState[asubs_id]->pSsInfo, num_stack);
}

/*-----------------------------------------------------------------------------
  Update the serving system's PLMN in policyman's state.
-----------------------------------------------------------------------------*/

/*  Command to update SS PLMN.
 */
typedef struct
{
  POLICYMAN_CMD_HDR;

  sys_plmn_id_s_type      plmn;
  sys_modem_as_id_e_type  asubs_id;
  sys_srv_status_e_type   srv_status;
} policyman_cmd_set_serving_plmn_t;


/*===========================================================================
  FUNCTION POLICYMAN_STATE_UPDATE_SS_INFO()

  DESCRIPTION
    Update Policyman state variables with new CM SS event
===========================================================================*/
void policyman_state_update_ss_info(
  policyman_ss_info_t *pSsInfo,
  sys_modem_as_id_e_type  asubs_id
  )
{
  policyman_ss_update_state(policyman_state_get_subs_state(asubs_id)->pSsInfo, pSsInfo, asubs_id);
}

/*===========================================================================
  FUNCTION POLICYMAN_STATE_MSIM_UPDATE_SS_INFO()

  DESCRIPTION
    Update Policyman state variables with new Mutisim CM SS event
===========================================================================*/
void policyman_state_msim_update_ss_info(
  policyman_ss_info_t    *pSsInfo,
  sys_modem_as_id_e_type  asubs_id
  )
{
  policyman_ss_update_state(policyman_state_get_subs_state(asubs_id)->pSsInfo, pSsInfo, asubs_id);
}


/*=============================================================================
  Phone-Event related APIs
=============================================================================*/
boolean policyman_state_cmph_init(
   void
)
{
   size_t i;
   boolean retval;

   for (i=0; i<POLICYMAN_NUM_SUBS; i++)
   {
     retval = policyman_cmph_init(&policyman_state_get_subs_state(i)->pCmPhInfo, i);
   }

   return retval;
 }

/*-------- policyman_state_cmph_nv_init --------*/
void policyman_state_cmph_nv_init(
  sys_modem_as_id_e_type asubs_id
)
{
  policyman_cmph_nv_init(&policyman_state_get_subs_state(asubs_id)->pCmPhInfo, asubs_id);
}

/*-------- policyman_state_set_simlock_verified --------*/
STATIC void policyman_state_set_simlock_verified(
  sys_modem_as_id_e_type  subsId,
  boolean                 verified
)
{
  boolean old = policyman_state.pSubsState[subsId]->simLockVerified;
  if (verified != old)
  {
    policyman_state.pSubsState[subsId]->simLockVerified = verified;
    POLICYMAN_MSG_HIGH_2("state simLockVerified for subs %d set to %d", subsId, verified);
  }
}
/*===========================================================================
  FUNCTION POLICYMAN_UIM_DEL_SUB()

  DESCRIPTION

  RETURN VALUE

  DEPENDENCIES

  SIDE EFFECTS
===========================================================================*/
boolean policyman_uim_del_sub(
  policyman_uim_info_t *pUimInfo,
  sys_modem_as_id_e_type asubs_id,
  mmgsdi_session_type_enum_type session_type
)
{
  boolean changed = FALSE;

  if (  pUimInfo != NULL
     && policyman_state.pDeviceInfo->uimNSubs
     )
  {
    policyman_state.pDeviceInfo->uimNSubs--;

    if (session_type == MMGSDI_GW_PROV_PRI_SESSION || session_type == MMGSDI_GW_PROV_SEC_SESSION)
    {
      memset(&pUimInfo->plmn, 0, sizeof(sys_plmn_id_s_type));
      pUimInfo->sim_app_type = MMGSDI_APP_NONE;
      changed = TRUE;
      policyman_state_reset_precondition_met(POLICYMAN_PRECOND_SIMTYPE, asubs_id);
    }
    else if (session_type == MMGSDI_1X_PROV_PRI_SESSION)
    {
      pUimInfo->csim_mcc      = 0;
      pUimInfo->cdma_app_type = MMGSDI_APP_NONE;
      changed = TRUE;
      policyman_state_reset_precondition_met(POLICYMAN_PRECOND_CDMASIMTYPE, asubs_id);
    }

    /* Precond IMSI is reset unconditionally on premise that we'll either have GW or 1x session removed */
    policyman_state_reset_precondition_met(POLICYMAN_PRECOND_IMSI, asubs_id);
    policyman_state_set_simlock_verified(asubs_id, TRUE);
  }

  POLICYMAN_MSG_HIGH_2("deleted sub status %d for sub %d",changed,asubs_id);
  return changed;
}

/*-------- policyman_state_delete_uim_info --------*/
void policyman_state_delete_uim_info(
  sys_modem_as_id_e_type asubs_id,
  mmgsdi_session_type_enum_type session_type
)
{
  if (policyman_uim_del_sub(policyman_state_get_subs_state(asubs_id)->pUimInfo, asubs_id, session_type) )
  {
    policyman_state_handle_update(asubs_id);
  }
}


/*=============================================================================
  UIM related APIs
=============================================================================*/
boolean policyman_state_uim_init(
  void
)
{
  size_t i;
  boolean retval;

  for(i=0; i<POLICYMAN_NUM_SUBS; i++)
  {
    retval = policyman_uim_init(&policyman_state_get_subs_state(i)->pUimInfo);
  }

  return retval;
}


void policyman_state_set_uim_num_subs(
  size_t  nSubs
  )
{
  policyman_uim_set_num_subs(&policyman_state, nSubs);
}

uint32 policyman_state_get_uim_num_subs(
  void
)
{
  return policyman_uim_get_num_subs();
}

boolean policyman_state_get_uim_plmn(
  policyman_state_t const *pState,
  size_t                  subsId,
  sys_plmn_id_s_type      *pPlmn
)
{
  return policyman_uim_get_imsi_plmn((policyman_state_t *)pState, subsId, pPlmn);
}

uint8 policyman_state_get_trm_cap(
  void
)
{
  policyman_state_t *pState = policyman_state_get_state();
  return pState->pDeviceInfo->trm_cap;
}
/*===========================================================================
  FUNCTION POLICYMAN_STATE_SET_SIM_TYPE()

  DESCRIPTION
   Set SIM App type in UIM state
===========================================================================*/
void
policyman_state_set_sim_type(
  size_t              subsId,
  mmgsdi_app_enum_type sim_app_type)
{

  /*  Set the IMSI PLMN; if it changed, handle a state update.
   */
  if (policyman_uim_set_sim_type(&policyman_state, subsId, sim_app_type))
  {
    /* run this method unconditionally, if all preconditions are met
          then rules will evaluate later and take appropriate action
      */
    policyman_policy_handle_uim_update(&policyman_state, subsId);

    policyman_state_handle_update(subsId);
  }
}

/*===========================================================================
  FUNCTION POLICYMAN_STATE_GET_SIM_TYPE()

  DESCRIPTION
   Get SIM App type from UIM state
===========================================================================*/
void
policyman_state_get_sim_type(
  size_t              subsId,
  mmgsdi_app_enum_type *sim_app_type)
{

  /*  Get the SIM APP type
   */
  policyman_uim_get_sim_type(&policyman_state, subsId, sim_app_type);
}

/*===========================================================================
  FUNCTION POLICYMAN_STATE_SET_CDMA_SIM_TYPE()

  DESCRIPTION
   Set SIM App type in UIM state
===========================================================================*/
void
policyman_state_set_cdma_sim_type(
  size_t               subsId,
  mmgsdi_app_enum_type sim_app_type)
{
  /*  Set the IMSI PLMN; if it changed, handle a state update.
   */
  if (policyman_uim_set_cdma_sim_type(&policyman_state, subsId, sim_app_type))
  {
    /* run this method unconditionally, if all preconditions are met
          then rules will evaluate later and take appropriate action
      */
    policyman_policy_handle_uim_update(&policyman_state, subsId);

    policyman_state_handle_update(subsId);
  }
}

/*===========================================================================
  FUNCTION policyman_state_get_cdma_sim_type

  DESCRIPTION
   Get CDMA SIM App type from UIM state
===========================================================================*/
mmgsdi_app_enum_type policyman_state_get_cdma_sim_type(
  size_t                subsId
)
{
  /*  Get the CDMA SIM APP type
   */
  return policyman_uim_get_cdma_sim_type(&policyman_state, subsId);
}

/*===========================================================================
  FUNCTION POLICYMAN_STATE_HANDLE_FTM_MODE()

  DESCRIPTION
   Handle FTM mode specific procsessing
===========================================================================*/
static void
policyman_state_handle_ftm_mode(
  void
  )
{
  /* Set FTM NV so that no rule execution can happen
   */
  policyman_set_FTM(TRUE);
}

/*===========================================================================
  FUNCTION POLICYMAN_STATE_HANDLE_OPRT_MODE()

  DESCRIPTION
   Handle FTM mode specific procsessing
===========================================================================*/
static void
policyman_state_handle_oprt_mode(
  sys_oprt_mode_e_type oprt_mode
  )
{
  /* Print out all configuration database when oprt_mode changes to ONLINE
        Policy Init will display the database if we are coming from test mode.
   */
  if(oprt_mode == SYS_OPRT_MODE_ONLINE)
  {
    if ( !policyman_ue_is_online() )
    {
      POLICYMAN_MSG_MED_0("oprt_mode changes to SYS_OPRT_MODE_ONLINE, print all configuration database");
      policyman_cfgitem_display_all();
    }
  }

  /* Initialize Policy when already in FTM and disable FTM
        For test_mode, we should contiue to be running without policy
    */
  if(   policyman_is_FTM()
     && !policyman_is_test_mode()
    )
  {
    /* Set in_FTM to be FALSE
          Do an uncoditional policy init
    */
    policyman_set_FTM(FALSE);
    policyman_policy_init(&policyman_state);
  }
}

/*===========================================================================
  FUNCTION POLICYMAN_STATE_UPDATE_PH_OPRT_MODE()

  DESCRIPTION
    Update operating mode into PM Phone state and calls handler function for its update

  PARAMETERS
    oprt_mode : operating mode to be set

  RETURN VALUE
    None
===========================================================================*/
void policyman_state_update_ph_oprt_mode(
  sys_oprt_mode_e_type oprt_mode
  )
{
  size_t i;

  switch(oprt_mode)
  {
    case SYS_OPRT_MODE_FTM:
      policyman_state_handle_ftm_mode();
      break;

    default:
      policyman_state_handle_oprt_mode(oprt_mode);
      break;
  }

  for (i = 0; i < POLICYMAN_NUM_SUBS; i++)
  {
    sys_modem_as_id_e_type  asubs_id = (sys_modem_as_id_e_type) i;
    policyman_subs_state_t  *pSubsState = policyman_state_get_subs_state(asubs_id);

    if (policyman_ph_set_oprt_mode(pSubsState->pCmPhInfo, oprt_mode))
    {
      policyman_policy_handle_user_pref_update(asubs_id);
      policyman_state_handle_update(asubs_id);
    }
  }
}

/*=============================================================================
  Num Subs related APIs
=============================================================================*/
 boolean
 policyman_state_subs_init(
   void
   )
 {
   return policyman_subs_init(&policyman_state.pDeviceInfo->pSubsInfo);
 }

/*-------- policyman_state_get_imei --------*/
void
policyman_state_get_imei(
  void
  )
{
  byte              *pImei = NULL;
  char              imei[16];
  char              *pDst;
  size_t            i;

  /*  Get the IMEI
 */
  nv_stat_enum_type result;
  nv_item_type      data;

  result = mre_efs_get_nv_item(NV_UE_IMEI_I, &data, SYS_MODEM_AS_ID_1);
  if (result == NV_DONE_S)
  {
    pImei = data.ue_imei.ue_imei;
  }

  /*  The IMEI buffer must start with a length byte of 8 and have 0x0a in
   *  the low nibble of the next byte.  If it doesn't, don't set IMEI.
   */
  if (pImei == NULL)
  {
    POLICYMAN_MSG_HIGH_0("Unable to read IMEI from NV");
    return;
  }

  if (*pImei != 0x08 || (pImei[1] & 0x0f) != 0x0a)
  {
    POLICYMAN_MSG_HIGH_0("IMEI read from NV has invalid format");
    policyman_device_imei_print(pImei);
    return;
  }

  /*  Copy the raw IMEI into the device information.
   */
  memscpy(
      policyman_device_info.rawIMEI,
      sizeof(policyman_device_info.rawIMEI),
      pImei,
      sizeof(policyman_device_info.rawIMEI)
      );

  ++pImei;

  /*  Get a pointer to the storage for the string we'll produce.
   */
  pDst = imei;

  /*  Get the first character of the IMEI.
   */
  *pDst++ = ((*pImei++) >> 4) + '0';

  /*  Get the rest of the characters.
   */
  for (i = 0 ; i < 6 ; ++i)
  {
    *pDst++ = (*pImei & 0x0f) + '0';
    *pDst++ = ((*pImei++) >> 4) + '0';
  }
  *pDst++ = (*pImei & 0x0f) + '0';
  *pDst = '\0';

  POLICYMAN_UT_MSG_1("IMEI is set to %s", imei);

  /*  Store a copy of the string in the device information.
   */
  policyman_device_info.pIMEI = policyman_str_dup(imei);
}

/*=============================================================================
  Network Scan related APIs
=============================================================================*/
boolean
policyman_state_network_init(
  void
  )
{
   size_t i;
   boolean retval;

   for (i=0; i<POLICYMAN_NUM_SUBS; i++)
   {
     retval = policyman_network_init(&policyman_state_get_subs_state(i)->pNetworkInfo, i);
   }

  return retval;
}

/*-------- policyman_state_update_network_scan_result --------*/
void
policyman_state_update_network_scan_result(
  cm_ph_info_s_type *pInfo
  )
{
  policyman_network_info_t *pNetworkInfo;
  sys_modem_as_id_e_type    subsId = SYS_MODEM_AS_ID_1;

  if(pInfo != NULL)
  {
    pNetworkInfo = policyman_state_get_network_info(subsId);

    /* Update the result and run rules
      */
    if(policyman_network_scan_update_result( pNetworkInfo, 
                                             subsId, 
                                             pInfo->available_networks_list_cnf, 
                                             &pInfo->available_networks
                                           )
      )
    {
      policyman_state_handle_update(subsId);
    }
  }

}

/*-------- policyman_state_get_network_info --------*/
policyman_network_info_t *
policyman_state_get_network_info(
  sys_modem_as_id_e_type subsId
  )
{
  return policyman_state.pSubsState[subsId]->pNetworkInfo;
}

/*=============================================================================
  HLOS MCC related APIs
=============================================================================*/
 
/*-------- policyman_state_current_mcc_update_to_efs --------*/
STATIC void policyman_state_current_mcc_update_to_efs(
  sys_mcc_type            mcc,
  sys_modem_as_id_e_type  subsId
)
{
  boolean  writeStatus;

  writeStatus = policyman_cfgitem_persist_item( POLICYMAN_CURRENT_MCC_NV_PATH,
                                                &mcc,
                                                sizeof(mcc),
                                                subsId);

  POLICYMAN_MSG_HIGH_3( "current_mcc %d for subs %d written to efs status %d",
                        mcc,
                        subsId,
                        writeStatus );
}

/*-------- policyman_state_update_hlos_mcc --------*/
void policyman_state_update_hlos_mcc(
  sys_mcc_type hlos_mcc,
  size_t       confidenceLevel,
  size_t       status
)
{
  policyman_state.pGlobalInfo->mcc             = hlos_mcc;
  policyman_state.pGlobalInfo->confidenceLevel = confidenceLevel;
  policyman_state.pGlobalInfo->status          = status;
}


/*-------- policyman_state_clear_hlos_mcc --------*/
void policyman_state_clear_hlos_mcc(
  void
)
{
  policyman_state_update_hlos_mcc(NO_MCC, 0, 0);
}


/*-------- policyman_state_get_hlos_mcc --------*/
sys_mcc_type policyman_state_get_hlos_mcc(
  void
)
{
  return policyman_state.pGlobalInfo->mcc;
}

/*-------- policyman_state_set_subs_mcc --------*/
void policyman_state_set_subs_mcc(
  sys_modem_as_id_e_type subsId,
  sys_mcc_type mcc
)
{
  sys_mcc_type current_mcc = policyman_state_get_subs_mcc(subsId);
  
  if (mcc != current_mcc)
  {
    policyman_state.pSubsState[subsId]->current_mcc = mcc;
    POLICYMAN_MSG_HIGH_3( "subs %d: updating current_mcc from %d to %d", subsId, current_mcc, mcc);
  }
}

/*-------- policyman_state_update_subs_mcc --------*/
void policyman_state_update_subs_mcc(
  sys_modem_as_id_e_type  subsId,
  sys_mcc_type mcc
)
{
  sys_mcc_type current_mcc = policyman_state_get_subs_mcc(subsId);
  
  /* update state and write to EFS if MCC has changed
       update location precond only if MCC is valid
  */
  if (mcc != current_mcc)
  {
    policyman_state_set_subs_mcc(subsId, mcc);
    policyman_state_current_mcc_update_to_efs(mcc, subsId);
    if (mcc != NO_MCC)
    {
      policyman_state_set_precondition_met(POLICYMAN_PRECOND_LOCATION, subsId);
    }
  }
}

/*-------- policyman_state_get_subs_mcc --------*/
sys_mcc_type policyman_state_get_subs_mcc(
  sys_modem_as_id_e_type  subsId
)
{
  return policyman_state.pSubsState[subsId]->current_mcc;  
}

/*=============================================================================
  FULL RAT related APIs
=============================================================================*/
void policyman_state_update_full_rat(
  boolean enterFullRat
  )
{
  if (enterFullRat != policyman_state.pGlobalInfo->immediate_fullrat)
  {
    policyman_state.pGlobalInfo->immediate_fullrat = enterFullRat;
    POLICYMAN_MSG_HIGH_1("policyman state updated with full rat status = %d", enterFullRat);
  }
}

/*-------- policyman_state_get_full_rat --------*/
boolean policyman_state_get_full_rat(
  void
  )
{
  return policyman_state.pGlobalInfo->immediate_fullrat;
}

/*-------- policyman_state_handle_full_rat_update --------*/
void policyman_state_handle_full_rat_update(
  policyman_policy_t   *pPolicy,
  policy_execute_ctx_t *pCtx
)
{
  /* For MSIM handle Full RAT enforcement in XML condition.
  */
  policyman_policy_execute(pPolicy, pCtx);
}
/*-------- policyman_state_get_simlock_verified --------*/
boolean policyman_state_get_simlock_verified(
  sys_modem_as_id_e_type  subsId
)
{
  return policyman_state.pSubsState[subsId]->simLockVerified;
}

/*-------- policyman_state_update_simlock_verified --------*/
void policyman_state_update_simlock_verified(
  sys_modem_as_id_e_type  subsId,
  boolean                 verified
)
{
  boolean old = policyman_state.pSubsState[subsId]->simLockVerified;
  if (verified != old)
  {
    policyman_state.pSubsState[subsId]->simLockVerified &= verified;
    POLICYMAN_MSG_HIGH_3("state simLockVerified for subs %d updated from %d to %d", 
                          subsId, old, policyman_state.pSubsState[subsId]->simLockVerified);
  }
}

/*-------- policyman_state_get_thermal_rat_disable_mask --------*/
uint32 policyman_state_get_thermal_rat_disable_mask(
  void
)
{
  uint32                    mask       = SYS_SYS_MODE_MASK_NONE;
  policyman_state_t        *pStateInfo = NULL;
  policyman_thermal_level_t lvl ;

  pStateInfo = policyman_state_get_state();
  lvl        = pStateInfo->pDeviceInfo->thermalLevel;
  
  if (lvl == POLICYMAN_THERM_LEVEL_2)
  {
    mask |= SYS_SYS_MODE_MASK_LTE; 
  }
  POLICYMAN_MSG_HIGH_1("get_rat_disable_mask 0x%04x", mask);

  return mask;
}


void policyman_state_update_thermal_level(
  policyman_thermal_level_t  thermalLevel
)
{
  policyman_state_t  *pStateInfo = NULL;

  pStateInfo = policyman_state_get_state();
  
  POLICYMAN_MSG_HIGH_2 ( "Thermal level updated from 0x%04x to 0x%04x", 
						  pStateInfo->pDeviceInfo->thermalLevel, 
						  thermalLevel);
  
  pStateInfo->pDeviceInfo->thermalLevel = thermalLevel;
}

/*-------- policyman_state_thermal_allow_persistence --------*/
boolean policyman_state_thermal_allow_persistence(
  void
)
{
  return (policyman_state_get_thermal_rat_disable_mask() == SYS_SYS_MODE_MASK_NONE) ? TRUE : FALSE;
}

#endif
