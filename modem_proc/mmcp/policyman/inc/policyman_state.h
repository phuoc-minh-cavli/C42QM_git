#ifndef _POLICYMAN_STATE_H_
#define _POLICYMAN_STATE_H_

/**
  @file policyman_state.h

  @brief
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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/inc/policyman_state.h#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/

#include "policyman_i.h"
#include "policyman_network_events.h"
#include "policyman_phone_events.h"
#include "policyman_rf.h"
#include "policyman_serving_system.h"
#include "policyman_subs.h"
#include "policyman_thermal_state.h"
#include "policyman_uim.h"



#define NO_MCC 1665              // 1665 is 0xFFF in BCD encoding and considered invalid MCC

typedef struct
{
  policyman_rf_info_t       *pRfInfo;
  policyman_subs_info_t     *pSubsInfo;
  size_t                     uimNSubs;
  char const                *pIMEI;
  byte                       rawIMEI[9];
  uint8                      trm_cap;  
  policyman_thermal_level_t  thermalLevel;
} policyman_device_info_t;

typedef struct
{
  sys_modem_as_id_e_type    asubs_id;
  sys_mcc_type              current_mcc;
  uint32                    preconditions_met;
  boolean                   simLockVerified;
  policyman_ss_info_t      *pSsInfo;
  policyman_cmph_info_t    *pCmPhInfo;
  policyman_uim_info_t     *pUimInfo;
  policyman_network_info_t *pNetworkInfo;
} policyman_subs_state_t;

typedef struct
{
  sys_mcc_type         mcc;
  size_t               confidenceLevel;
  size_t               status;
  boolean              immediate_fullrat;
} policyman_global_info_t;

struct policyman_state_t
{
  policyman_device_info_t     *pDeviceInfo;
  policyman_subs_state_t      *pSubsState[POLICYMAN_NUM_SUBS];
  policyman_global_info_t     *pGlobalInfo;
};


/*=============================================================================
  General APIs
=============================================================================*/


/*-------- policyman_state_init --------*/
/**
@brief

@param

@return

*/
void
policyman_state_init(
  void
  );


/*-------- policyman_state_deinit --------*/
/**
@brief

@param

@return

*/
void
policyman_state_deinit(
  void
  );


/*-------- policyman_state_get_state --------*/
/**
@brief  Get a pointer to the policyman state.  This will in general only be
        called by unit tests - most use of policyman_state will be through
        parameters passed to functions.

@param

@return

*/
policyman_state_t *
policyman_state_get_state(
  void
  );


/*-------- policyman_state_get_subs_state --------*/
/**
@brief

@param

@return

*/
policyman_subs_state_t *
policyman_state_get_subs_state(
  sys_modem_as_id_e_type  asubs_id
  );


/*-------- policyman_state_get_serving_system_info --------*/
policyman_ss_info_t *policyman_state_get_serving_system_info(
  policyman_state_t       *pState,
  sys_modem_as_id_e_type   subsId
);


/*-------- policyman_state_get_subs_precond --------*/
/**
@brief

@param

@return

*/
uint32
policyman_state_get_subs_precond(
  sys_modem_as_id_e_type  asubs_id
  );


/*-------- policyman_state_get_subs_state --------*/
/**
@brief  Get a pointer to the policyman subs state.

@param

@return

*/
policyman_subs_info_t *
policyman_state_get_subs_info(
  void
  );



/*-------- policyman_state_handle_update --------*/
/**
@brief  The policyman state has been udpated; policies should be processed to
        determine whether there is a policy update.

@param[in]  asubs_id subscription id for which policies should be processed

@return
  Nothing.
*/
void
policyman_state_handle_update(
  sys_modem_as_id_e_type  asubs_id
  );


/*-------- policyman_state_set_precondition_met --------*/
/**
@brief  Indicate that a precondition has been met.

This should only be called on the main policyman thread - events that involve
preconditions should be queued to that thread.

@param[in]  precondition  Bitmask of the POLICYMAN_PRECOND_XXXX defines in
                          policyman_policy.h indicating which precondition has
                          now been met.
@param[in]  asubs_id subscription id for which precondiiton is required to be set

@return
  Nothing
*/
void
policyman_state_set_precondition_met(
  uint32  precondition,
  sys_modem_as_id_e_type asubs_id
  );

/*-------- policyman_state_check_precondition_met --------*/
/**
@brief  Check that a precondition has been met.

@param[in]  precondition  Bitmask of the POLICYMAN_PRECOND_XXXX defines in
                          policyman_policy.h indicating which precondition has
                          to be checked.
@param[in] asubs_id Subscription id to check for precondition

@return
  TRUE if precondition is met, FALSE otherwise
*/
boolean policyman_state_check_precondition_met(
  uint32                  precondition,
  sys_modem_as_id_e_type  asubs_id
);

/*-------- policyman_state_reset_precondition_met --------*/
/**
@brief  Reset a precondition that has been met.

@param[in]  precondition  Bitmask of the POLICYMAN_PRECOND_XXXX defines in
                          policyman_policy.h indicating which precondition has
                          to be reset.
@param[in]  asubs_id  Subscription for which precondition has to be reset.

@return
  None
*/
void
policyman_state_reset_precondition_met(
  uint32                  precondition,
  sys_modem_as_id_e_type  asubs_id
  );




/*=============================================================================
  RF related APIs
=============================================================================*/


/*-------- policyman_state_get_rf_info --------*/
/**
@brief  Get the RF information (RATs supported + band masks) for a specific
        device.

@param

@return

*/
boolean
policyman_state_get_rf_info(
  policyman_state_t const     *pState,
  size_t                      device,
  policyman_rf_device_info_t  *pInfo
  );


/*-------- policyman_state_device_calibrated --------*/
/**
@brief  Get whether device is calibrated, and it is valid to persist EFS items

@param

@return

*/
boolean
policyman_state_device_calibrated(
  void
  );


/*=============================================================================
  Serving System related APIs
=============================================================================*/

uint32
policyman_state_get_serving_mcc(
  policyman_state_t const   *pState,
  sys_modem_stack_id_e_type num_stack,
  sys_modem_as_id_e_type    asubs_id
  );


void
policyman_state_get_serving_plmn(
  policyman_state_t const   *pState,
  sys_modem_stack_id_e_type num_stack,
  sys_modem_as_id_e_type    asubs_id,
  sys_plmn_id_s_type        *pPlmn
  );

sys_plmn_id_s_type *policyman_state_get_device_plmn(
  policyman_state_t const   *pState
);

void policyman_state_update_device_plmn(
  sys_plmn_id_s_type *pPlmn
);


/*===========================================================================
  FUNCTION POLICYMAN_STATE_UPDATE_SS_INFO()

  DESCRIPTION
    Update Policyman state variables with new CM SS event
===========================================================================*/
void policyman_state_update_ss_info
(
  policyman_ss_info_t *pSsInfo,
  sys_modem_as_id_e_type  asubs_id
);

/*===========================================================================
  FUNCTION POLICYMAN_STATE_MSIM_UPDATE_SS_INFO()

  DESCRIPTION
    Update Policyman state variables with new Mutisim CM SS event
===========================================================================*/
void
policyman_state_msim_update_ss_info(
  policyman_ss_info_t *pSsInfo,
  sys_modem_as_id_e_type  asubs_id
  );

/*===========================================================================
  FUNCTION POLICYMAN_STATE_GET_SRV_STATUS()

  DESCRIPTION
    Get srv_status for stack from PolicyMan state
 ===========================================================================*/
sys_srv_status_e_type
policyman_state_get_srv_status(
  policyman_state_t const   *pState,
  sys_modem_stack_id_e_type num_stack,
  sys_modem_as_id_e_type    asubs_id
  );

/*===========================================================================
  FUNCTION POLICYMAN_STATE_GET_REJECT_CAUSE()

  DESCRIPTION
    Get Registration Reject cause for stack from PolicyMan state
 ===========================================================================*/
byte policyman_state_get_reject_cause(
  policyman_state_t const *pState,
  sys_modem_stack_id_e_type num_stack,
  sys_modem_as_id_e_type  asubs_id
  );


/*=============================================================================
  CM Phone Event related APIs
=============================================================================*/

/*===========================================================================
  FUNCTION POLICYMAN_STATE_UPDATE_PH_OPRT_MODE()

  DESCRIPTION
    Update operating mode into PM Phone state

  PARAMETERS
    oprt_mode : operating mode to be set
===========================================================================*/
void policyman_state_update_ph_oprt_mode
(
  sys_oprt_mode_e_type oprt_mode
);


/*===========================================================================
  FUNCTION POLICYMAN_STATE_CMPH_NV_INIT()

  DESCRIPTION
    Read NVs related to CM PH state for a subscription

  PARAMETERS    
    asubs_id : subscription to read NVs for
===========================================================================*/
void policyman_state_cmph_nv_init(
  sys_modem_as_id_e_type asubs_id
);


/*=============================================================================
  UIM related APIs
=============================================================================*/

uint32
policyman_state_get_uim_num_subs(
  void
);

boolean
policyman_state_get_uim_plmn(
  policyman_state_t const *pState,
  size_t                  subsId,
  sys_plmn_id_s_type      *pPlmn
  );

void
policyman_state_set_uim_num_subs(
  size_t  nSubs
  );


uint8 policyman_state_get_trm_cap(
  void
);


void
policyman_state_set_sim_type(
  size_t              subsId,
  mmgsdi_app_enum_type sim_app_type);

void
policyman_state_set_cdma_sim_type(
  size_t               subsId,
  mmgsdi_app_enum_type sim_app_type);

void policyman_state_delete_uim_info
(
  sys_modem_as_id_e_type asubs_id,
  mmgsdi_session_type_enum_type session_type
);

/*===========================================================================
  FUNCTION POLICYMAN_STATE_GET_SIM_TYPE()

  DESCRIPTION
   Get SIM App type from UIM state
===========================================================================*/
void
policyman_state_get_sim_type(
  size_t              subsId,
  mmgsdi_app_enum_type *sim_app_type);

/*-------- policyman_state_get_cdma_sim_type --------*/
mmgsdi_app_enum_type policyman_state_get_cdma_sim_type(
  size_t                 subsId
);


/*=============================================================================
  Network Scan related APIs
=============================================================================*/
void
policyman_state_update_network_scan_result(
  cm_ph_info_s_type *pInfo
  );

/*-------- policyman_state_get_network_info --------*/
policyman_network_info_t *
policyman_state_get_network_info(
  sys_modem_as_id_e_type subsId
  );

/*=============================================================================
  HLOS MCC related APIs
=============================================================================*/
void policyman_state_update_hlos_mcc(
  sys_mcc_type hlos_mcc,
  size_t       confidenceLevel,
  size_t       status
);

/*-------- policyman_state_clear_hlos_mcc --------*/
void policyman_state_clear_hlos_mcc(
  void
);

/*-------- policyman_state_get_hlos_mcc --------*/
sys_mcc_type policyman_state_get_hlos_mcc(
  void
);

/*-------- policyman_state_set_subs_mcc --------*/
void policyman_state_set_subs_mcc(
  sys_modem_as_id_e_type subsId,
  sys_mcc_type mcc
);

/*-------- policyman_state_update_subs_mcc --------*/
void policyman_state_update_subs_mcc(
  sys_modem_as_id_e_type  subsId,
  sys_mcc_type mcc
);

/*-------- policyman_state_get_subs_mcc --------*/
sys_mcc_type policyman_state_get_subs_mcc(
  sys_modem_as_id_e_type  subsId
);

/*-------- policyman_state_get_simlock_verified --------*/
boolean policyman_state_get_simlock_verified(
  sys_modem_as_id_e_type  subsId
);

/*-------- policyman_state_update_simlock_verified --------*/
void policyman_state_update_simlock_verified(
  sys_modem_as_id_e_type  subsId,
  boolean                 verified
);

/*=============================================================================
  FULL RAT related APIs
=============================================================================*/
void policyman_state_update_full_rat(
  boolean enterFullRat
);

/*-------- policyman_state_get_full_rat --------*/
boolean policyman_state_get_full_rat(
  void
);

/*-------- policyman_state_handle_full_rat_update --------*/
void policyman_state_handle_full_rat_update(
  policyman_policy_t   *pPolicy,
  policy_execute_ctx_t *pCtx
);

/*=============================================================================
  Internal functions exported to unit test code.
=============================================================================*/

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
boolean
policyman_state_rf_init(
  void
  );
#endif

void policyman_state_ss_init(
  void
);

boolean
policyman_state_uim_init(
  void
  );

boolean
policyman_state_cmph_init(
  void
  );

boolean
policyman_state_subs_init(
  void
  );

boolean
policyman_state_cmcall_init(
  void
  );

boolean
policyman_state_network_init(
  void
  );

void policyman_state_trm_cap_init(
  void
);

void policyman_state_update_thermal_level(
  policyman_thermal_level_t  thermallvl
);


/*-------- policyman_state_get_thermal_rat_disable_mask --------*/
uint32 policyman_state_get_thermal_rat_disable_mask(
  void
);

boolean policyman_state_thermal_allow_persistence(
  void
);	

#if defined FEATURE_DISABLE_DYNAMIC_POLICIES
uint8 policyman_state_get_trm_cap(
  void
);
#endif
#endif /* _POLICYMAN_STATE_H_ */
