/**
  @file policyman_serving_system.c

  @brief  
*/

/*
    Copyright (c) 2013-2018 QUALCOMM Technologies Incorporated.
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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/src/policyman_serving_system.c#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/
#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
#include "policyman_cm.h"
#include "policyman_dbg.h"
#include "policyman_device_config.h"
#include "policyman_nas.h"
#include "policyman_plmn.h"
#include "policyman_policy.h"
#include "policyman_rat_capability.h"
#include "policyman_rules.h"
#include "policyman_serving_system.h"
#include "policyman_set.h"
#include "policyman_state.h"
#include "policyman_subs.h"
#include "policyman_task.h"
#include "policyman_timer.h"
#include "policyman_uim.h"
#include "policyman_util.h"
#include "policyman_xml.h"

#include <stringl/stringl.h>

#define INVALID_RAT_MASK  0xffffffff


/*=============================================================================
  Data Structure definitions.
=============================================================================*/
typedef struct 
{  
  /*  stack variables
   */
  boolean                 is_operational;
  byte                    pending_reject; /* pending reject cause*/
  byte                    reject_cause; /* registration reject cause*/
  sys_sys_id_type_e_type  id_type;      /* PLMN id type */
  boolean                 serving_info_valid;  /* Whether PLMN should be used */
  sys_plmn_id_s_type      plmn;         /* PLMN of the serving system */
  sys_plmn_mcc_list       mcc_list_3gpp2;      /* List of 3GPP2 MCCs  */
  sys_mnc_type            mnc_3gpp2;           /* 3GPP2 MNC */
  sys_sys_mode_e_type     sys_mode;     /* System's mode */
  sys_srv_status_e_type   srv_status;   /* Service Status */
  sys_band_class_e_type   active_band;  /* serving RF band */
  sys_srv_domain_e_type   srv_domain ;  /* Serving domain */
  int                     acq_fail_cnt; /* tracks number of acq fails*/
} policyman_stack_info_t;


struct policyman_ss_info_t
{
  sys_modem_as_id_e_type  asubs_id;     /* Subscription for this info */
  policyman_stack_info_t  stack[SYS_MODEM_STACK_ID_MAX];
};

/*=============================================================================
  Serving system Event callback definition.
=============================================================================*/

struct policyman_cm_serving_info_cb_s
{
  POLICYMAN_CMD_HDR;
  policyman_cm_serving_info_t info;
};

struct policyman_hlos_mcc_cmd_s
{
  POLICYMAN_CMD_HDR;

  sys_mcc_type  mcc;
  size_t        confidenceLevel;
  size_t        status;
};

struct policyman_cm_report_acq_fail_s
{
  POLICYMAN_CMD_HDR;
  sys_modem_as_id_e_type     subsId;
  sys_modem_stack_id_e_type  stackId;
};


/*=============================================================================
  Serving system Event callback definition for Multisim
=============================================================================*/
struct policyman_cmss_evt_cb_s
{
  POLICYMAN_CMD_HDR;
  cm_ss_event_e_type        evt;
  cm_mm_ss_info_s_type      info;
};



/*=============================================================================
  APIs for serving system information
=============================================================================*/
void policyman_ss_set_default_value(
   policyman_ss_info_t         *pSsInfo,
   sys_modem_stack_id_e_type   num_stack
)
{
  policyman_stack_info_t *pStack;

  if (pSsInfo != NULL)
  {
    pStack = &pSsInfo->stack[num_stack];

    pStack->sys_mode = SYS_SYS_MODE_NONE;
    pStack->srv_status = SYS_SRV_STATUS_NO_SRV;     
    pStack->active_band = SYS_BAND_CLASS_NONE;

    // make PLMN digits undefined upon power up
    pStack->id_type = SYS_SYS_ID_TYPE_UNDEFINED;

    // PLMN is 3 bytes, set to undefined
    pStack->serving_info_valid = FALSE;
    pStack->acq_fail_cnt = 0;
    policyman_plmn_clear(&pStack->plmn);
    POLICYMAN_MSG_MED_2("Subs %d stack %d does not have valid data", pSsInfo->asubs_id, num_stack);
  }
}

/*-------- policyman_ss_init --------*/
boolean policyman_ss_init(
  sys_modem_as_id_e_type  subs,
  policyman_ss_info_t     **ppInfo
)
{
  size_t              i;
  policyman_ss_info_t *pInfo;
  
  pInfo = (policyman_ss_info_t *) policyman_mem_alloc(sizeof(policyman_ss_info_t));

  for (i = SYS_MODEM_STACK_ID_1 ; i < SYS_MODEM_STACK_ID_MAX ; i++)
  {
    pInfo->asubs_id = subs;
    policyman_ss_set_default_value(pInfo, i);
  }

  *ppInfo = pInfo;
  return TRUE;
}


/*-------- policyman_ss_deinit --------*/
void policyman_ss_deinit(
  policyman_ss_info_t *pInfo
)
{
  POLICYMAN_MEM_FREE_IF(pInfo);
}

/*-------- policyman_ss_get_ss_info --------*/
policyman_ss_info_t* policyman_ss_get_ss_info(
  sys_modem_as_id_e_type subsId
)
{
  if(subsId < 0 || subsId >= POLICYMAN_NUM_SUBS)
  {
    POLICYMAN_ERR_FATAL("subsId out of range", 0, 0, 0);
  }

  return policyman_state_get_state()->pSubsState[subsId]->pSsInfo;
}

/*-------- policyman_ss_get_stack_info --------*/
STATIC policyman_stack_info_t* policyman_ss_get_stack_info(
  sys_modem_as_id_e_type subsId,
  sys_modem_stack_id_e_type stackId
)
{
  if(stackId < SYS_MODEM_STACK_ID_1 || stackId > SYS_MODEM_STACK_ID_2)
  {
    POLICYMAN_ERR_FATAL("stack id out of range", 0, 0, 0);
  }

  return  &policyman_ss_get_ss_info(subsId)->stack[stackId];
}

/*-------- policyman_ss_get_stack --------*/
STATIC policyman_stack_info_t *policyman_ss_get_stack(
  policyman_state_t        *pState,
  sys_modem_as_id_e_type    subsId,
  sys_modem_stack_id_e_type stackId
)
{
  policyman_ss_info_t *pSsInfo;
  
  pSsInfo = policyman_state_get_serving_system_info(pState, subsId);
  return &pSsInfo->stack[stackId];
}
 

/*-------- policyman_ss_set_stack_opr --------*/
void policyman_ss_set_stack_opr(
  sys_modem_as_id_e_type subsId,
  sys_modem_stack_id_e_type stackId,
  boolean isOperational
)
{
    policyman_stack_info_t* pStackInfo = policyman_ss_get_stack_info(subsId,stackId);
    pStackInfo->is_operational = isOperational;
  }

/*-------- policyman_ss_subs_sys_id_is_valid --------*/
STATIC boolean policyman_ss_subs_sys_id_is_valid(
 sys_modem_as_id_e_type  subsId
)
{
  size_t               stackId;
  policyman_ss_info_t *pInfo;
  boolean              idValid = FALSE;

  pInfo = policyman_ss_get_ss_info(subsId);

  for (stackId = SYS_MODEM_STACK_ID_1; !idValid && stackId < SYS_MODEM_STACK_ID_MAX; stackId++)
  {
    policyman_stack_info_t *pStackInfo;

    pStackInfo = policyman_ss_get_stack_info(subsId, stackId);
    idValid    = pStackInfo->is_operational 
              && pStackInfo->serving_info_valid;

  }

  return idValid;
}

/*-------- policyman_ss_subs_sys_id_is_defined --------*/
STATIC boolean policyman_ss_subs_sys_id_is_defined(
 sys_modem_as_id_e_type  subsId
)
{
  size_t               stackId;
  policyman_ss_info_t *pInfo;
  boolean              idValid = FALSE;

  pInfo = policyman_ss_get_ss_info(subsId);

  for (stackId = SYS_MODEM_STACK_ID_1; !idValid && stackId < SYS_MODEM_STACK_ID_MAX; stackId++)
  {
    policyman_stack_info_t *pStackInfo;

    pStackInfo = policyman_ss_get_stack_info(subsId, stackId);
    idValid    = pStackInfo->is_operational 
              && pStackInfo->id_type != SYS_SYS_ID_TYPE_UNDEFINED;

  }

  return idValid;
}

/*-------- policyman_ss_status_is_service --------*/
STATIC boolean policyman_ss_status_is_service(
  sys_srv_status_e_type srv_status
)
{
  boolean retval = FALSE;

  switch(srv_status)
  {
    case SYS_SRV_STATUS_SRV:
    case SYS_SRV_STATUS_LIMITED:
    case SYS_SRV_STATUS_LIMITED_REGIONAL:
      retval = TRUE;
      break;

    case SYS_SRV_STATUS_NONE:
    case SYS_SRV_STATUS_NO_SRV:
    case SYS_SRV_STATUS_PWR_SAVE:
    default:
      break;
  }

  return retval;
}

/*-------- policyman_ss_reset_acq_fail_cnt --------*/
STATIC void policyman_ss_reset_acq_fail_cnt(
  policyman_stack_info_t *pStack
)
{
  POLICYMAN_MSG_HIGH_0("Acq fail cnt reset");
  pStack->acq_fail_cnt = 0;
}

/*-------- policyman_ss_determine_plmn_validity --------*/
STATIC void policyman_ss_determine_plmn_validity(
 sys_modem_as_id_e_type  subsId
)
{
  size_t               stackId;
  policyman_ss_info_t *pInfo;

  pInfo = policyman_ss_get_ss_info(subsId);
  for (stackId = SYS_MODEM_STACK_ID_1; stackId < SYS_MODEM_STACK_ID_MAX; stackId++)
  {
    policyman_stack_info_t *pStackInfo;

    pStackInfo = policyman_ss_get_stack_info(subsId, stackId);

    if (pStackInfo->is_operational)
    {
      // Reset acq_fail_cnt if service is available.
      if (policyman_ss_status_is_service(pStackInfo->srv_status))
      {
        policyman_ss_reset_acq_fail_cnt(pStackInfo);
      }
      // Increment acq_fail_cnt if we are in power save
      else if (pStackInfo->srv_status == SYS_SRV_STATUS_PWR_SAVE)
      {
        pStackInfo->acq_fail_cnt++;
      }
    }
    /* A status of NO_SRV does not itself affect the PLMN validity. In this
          case the only changes will be the PLMN becoming invalid if the stack 
          is not operational or the id_type is undefined.
      */
    if (pStackInfo->srv_status == SYS_SRV_STATUS_NO_SRV)
    {
      pStackInfo->serving_info_valid =  pStackInfo->serving_info_valid
                                     && pStackInfo->is_operational
                                     && pStackInfo->id_type != SYS_SYS_ID_TYPE_UNDEFINED
                                     && (pStackInfo->acq_fail_cnt < 1);
    }
    else
    {
      pStackInfo->serving_info_valid =   pStackInfo->is_operational
                                      && pStackInfo->id_type != SYS_SYS_ID_TYPE_UNDEFINED
                                      && pStackInfo->srv_status != SYS_SRV_STATUS_PWR_SAVE
                                      && (pStackInfo->acq_fail_cnt < 2);
    }
  }
}

/*-------- policyman_ss_stack_plmn_is_valid --------*/
STATIC boolean policyman_ss_stack_plmn_is_valid(
  sys_modem_stack_id_e_type stackId,
  policyman_ss_info_t       *pInfo
)
{
  policyman_stack_info_t *pStackInfo;

  pStackInfo = &pInfo->stack[stackId];
  return pStackInfo->serving_info_valid;
}

/*-------- policyman_ss_find_timer --------*/
STATIC policyman_timer_t *policyman_ss_find_timer(
  char const                  *pName,
  policyman_timer_expirefn_t   pfnExpire,
  sys_modem_as_id_e_type       subsId
)
{
  policyman_timer_t *pTimer = NULL;

  pTimer = policyman_timer_get_timer(pName);
  policyman_timer_set_subs(pTimer, subsId);
  policyman_timer_set_expirefn(pTimer, pfnExpire);

  return pTimer;
}

/*-------- policyman_subs_has_location --------*/
STATIC boolean policyman_subs_has_location(
  sys_modem_as_id_e_type  subsId
)
{
  return policyman_ss_subs_sys_id_is_valid(subsId);
}

/*-------- policyman_find_subs_with_location_info --------*/
STATIC boolean policyman_find_subs_with_location_info(
  sys_modem_as_id_e_type  *pLocationSubs
)
{
  size_t  index;
  size_t  nSim;
  boolean result = FALSE;

  (void)policyman_get_current_num_sim(&nSim);

  /* loop through all SUBS and determine which one has location info */
  for (index = SYS_MODEM_AS_ID_1; index < nSim; index++)
  {
    result = policyman_subs_has_location(index);
    if (result)
    {
      *pLocationSubs = (sys_modem_as_id_e_type)index;
      break;
    }
  }

  return result;
}

/*-------- policyman_ss_update_location_precond --------*/
STATIC void policyman_ss_update_location_precond(
  void
)
{
  size_t                   nSim;
  size_t                   subsId;
  boolean                  haveLocation;

  haveLocation = policyman_find_subs_with_location_info((sys_modem_as_id_e_type*) &subsId);
  (void)policyman_get_current_num_sim(&nSim);

  // loop through all SUBS and update location precond if we have location
  for (subsId = SYS_MODEM_AS_ID_1; subsId < nSim; subsId++)
  {
    if (haveLocation)
    {
      policyman_state_set_precondition_met(POLICYMAN_PRECOND_LOCATION, subsId);
    }
    else
    {
      policyman_state_reset_precondition_met(POLICYMAN_PRECOND_LOCATION, subsId);
    }
  }    
}

/*-------- policyman_ss_update_precond_for_subs --------*/
STATIC void policyman_ss_update_precond_for_subs(
  sys_modem_as_id_e_type    subsId,
  policyman_ss_info_t      *pSsInfo
)
{
  boolean                    plmnValid = FALSE;
  sys_modem_stack_id_e_type  stack;

  for (stack = SYS_MODEM_STACK_ID_1; !plmnValid && (stack < SYS_MODEM_STACK_ID_MAX); stack++)
  {
    plmnValid = policyman_ss_stack_plmn_is_valid(stack, pSsInfo);
  }

  if (plmnValid)
  {
    policyman_state_set_precondition_met(POLICYMAN_PRECOND_SS, subsId);
 
    // reset HLOS MCC
    policyman_state_clear_hlos_mcc();
  }
  else
  {
    POLICYMAN_MSG_ERROR_1("subs %d: PLMN not valid on any stack, reset SS precondition", subsId);
    policyman_state_reset_precondition_met(POLICYMAN_PRECOND_SS, subsId);
  }

  policyman_ss_update_location_precond();
}

/*-------- policyman_ss_handle_timer_expired --------*/
STATIC void policyman_ss_handle_timer_expired(
  policyman_timer_t        *pTimer,
  sys_modem_stack_id_e_type stackId
)
{
  sys_modem_as_id_e_type    subsId  = policyman_timer_get_subs(pTimer);
  policyman_stack_info_t   *pStack = policyman_ss_get_stack_info(subsId, stackId);


  /* Timer has expired so mark stack PLMN as invalid for decision making
   */
  if (pStack != NULL)
  {
    policyman_ss_info_t  *pSsinfo;
    
    pStack->serving_info_valid = FALSE;
    pSsinfo = policyman_ss_get_ss_info(subsId);

    policyman_ss_update_precond_for_subs(subsId, pSsinfo);
    policyman_state_handle_update(SYS_MODEM_AS_ID_1);
  }
}

/*-------- policyman_ss_handle_1x_oos_expired --------*/
STATIC void policyman_ss_handle_1x_oos_expired(
  policyman_timer_t *pTimer
)
{
  policyman_ss_handle_timer_expired(pTimer, SYS_MODEM_STACK_ID_1);
}

/*-------- policyman_ss_handle_hybrid_oos_expired --------*/
STATIC void policyman_ss_handle_hybrid_oos_expired(
  policyman_timer_t *pTimer
)
{
  policyman_ss_handle_timer_expired(pTimer, SYS_MODEM_STACK_ID_2);
}

/*-------- policyman_ss_get_timer_for_stack --------*/
STATIC policyman_timer_t *policyman_ss_get_timer_for_stack(
  policyman_stack_info_t    *pStack,
  sys_modem_stack_id_e_type stackId,
  sys_modem_as_id_e_type    subsId
)
{  
  policyman_timer_t       *pTimer = NULL;

  if (   stackId == SYS_MODEM_STACK_ID_1
      && subsId == SYS_MODEM_AS_ID_1
     )
  {
    pTimer = policyman_ss_find_timer("1x_oos", policyman_ss_handle_1x_oos_expired, subsId);
  }
  /* For secondary stack look up for hybr_oos timer
     */
  else if (   stackId == SYS_MODEM_STACK_ID_2
           && subsId == SYS_MODEM_AS_ID_1
          )
  {
    pTimer = policyman_ss_find_timer("hybr_oos", policyman_ss_handle_hybrid_oos_expired, subsId);
  }

  return pTimer;  
}

/*-------- policyman_ss_check_stack_oos --------*/
STATIC void policyman_ss_check_stack_oos(
  policyman_stack_info_t    *pStack,
  sys_modem_stack_id_e_type stackId,
  sys_modem_as_id_e_type    subsId
)
{  
  policyman_timer_t *pTimer = NULL;

  pTimer = policyman_ss_get_timer_for_stack(pStack, stackId, subsId);

  if (NULL != pTimer)
  {
    if (policyman_ss_status_is_service(pStack->srv_status))
    {
      policyman_timer_stop(pTimer);
    }
  }
}


/*===========================================================================
  FUNCTION POLICYMAN_SS_GET_MCC()

  DESCRIPTION
    GET MCC for stack from PM STATE
 ===========================================================================*/
uint32 policyman_ss_get_mcc(
  policyman_ss_info_t         *pInfo,
  sys_modem_stack_id_e_type   num_stack
)
{
  if(pInfo) {return policyman_plmn_get_mcc(&pInfo->stack[num_stack].plmn);}
  else      {return 0;}
}

/*===========================================================================
  FUNCTION POLICYMAN_SS_GET_PLMN()

  DESCRIPTION
    GET PLMN for stack from PM STATE
 ===========================================================================*/
void policyman_ss_get_plmn(
  policyman_ss_info_t         *pInfo,
  sys_modem_stack_id_e_type   num_stack,
  sys_plmn_id_s_type          *pPlmn
)
{
  policyman_stack_info_t *pStack;
  
  if (pInfo != NULL && pPlmn != NULL)
  {
    pStack = &pInfo->stack[num_stack];
    
    if (policyman_ss_stack_plmn_is_valid(num_stack, pInfo))
    {
      *pPlmn = pStack->plmn;
    }
    else
  {
      POLICYMAN_MSG_ERROR_3("Subs %d stack %d not operational or PLMN invalid %d, returning PLMN 0",
                             pInfo->asubs_id, 
                             num_stack, 
                             pStack->serving_info_valid );

      sys_plmn_set_mcc_mnc(FALSE, 0, 0, pPlmn);
    }
  }
}

/*===========================================================================
  FUNCTION POLICYMAN_SS_GET_PLMN_ID_TYPE()

  DESCRIPTION
    GET PLMN ID type for MAIN stack
 ===========================================================================*/
sys_sys_id_type_e_type policyman_ss_get_plmn_id_type(
  policyman_ss_info_t         *pSsInfo,
  sys_modem_stack_id_e_type   num_stack
)
{
  if(pSsInfo){ return pSsInfo->stack[num_stack].id_type; }
  else return SYS_SYS_ID_TYPE_UNDEFINED;
}

/*-------- policyman_ss_some_stack_has_service --------*/
STATIC boolean policyman_ss_some_stack_has_service(
  policyman_ss_info_t *pCurrentInfo
)
{
  size_t stack;

  for (stack = SYS_MODEM_STACK_ID_1 ; stack < SYS_MODEM_STACK_ID_MAX ; stack++)
  {
    if (  pCurrentInfo->stack[stack].is_operational
       && policyman_ss_status_is_service(pCurrentInfo->stack[stack].srv_status)
       )
    {
      return TRUE;
    }
  }

  return FALSE;
}
  
/*-------- policyman_is_new_srv_status_changed --------*/
STATIC boolean policyman_is_new_srv_status_changed(
  sys_srv_status_e_type current_srv_status, 
  sys_srv_status_e_type new_srv_status
)
{  
  /* The valid changes are:
    * In Service -> OOS, PWR_SAVE, LIMITED REGIONAL
    * OOS -> PWR-SAVE, SRV, LIMITED SRV, LIMITED REGIONAL
    * LIMITED <-> FULL SERVICE
    */

  return new_srv_status != current_srv_status;

}

/*-------- policyman_3gpp2_mccs_are_equal --------*/
STATIC boolean policyman_3gpp2_mccs_are_equal(
  sys_plmn_mcc_list *pCurrentList,
  sys_plmn_mcc_list *pNewList
)
{
  return memcmp(pCurrentList, pNewList, sizeof(sys_plmn_mcc_list)) == 0;
}

/*-------- policyman_ss_sys_mode_is_3gpp --------*/
STATIC boolean policyman_ss_sys_mode_is_3gpp(
  sys_sys_mode_e_type sys_mode
)
{
  return    sys_mode == SYS_SYS_MODE_GSM
         || sys_mode == SYS_SYS_MODE_WCDMA
         || sys_mode == SYS_SYS_MODE_LTE
         || sys_mode == SYS_SYS_MODE_TDS;
}

/*-------- policyman_ss_get_plmn_from_stack --------*/
STATIC void policyman_ss_get_plmn_from_stack(
  policyman_stack_info_t  *pStack,
  sys_plmn_id_s_type      *pPlmn
)
{
  /* For 3GPP2 system use MCC from MCC list, otherwise use 3GPP PLMN */
  if (pStack->sys_mode != SYS_SYS_MODE_CDMA)
  {
    *pPlmn = pStack->plmn;
  }
  else if (pStack->mcc_list_3gpp2.count > 0)
  {
    policyman_plmn_from_mcc_mnc(pPlmn, pStack->mcc_list_3gpp2.mcc_list[0], pStack->mnc_3gpp2);
  }
}

/*-------- policyman_ss_get_plmn_from_subs --------*/
STATIC boolean policyman_ss_get_plmn_from_subs(
  sys_modem_as_id_e_type   subsId,
  sys_plmn_id_s_type      *pPlmn
)
{
  size_t                  stackId;
  boolean                 plmnValid = FALSE;
  policyman_ss_info_t    *pSsInfo;
  boolean                 stackIs3gpp = FALSE;
  policyman_stack_info_t *pStack      = NULL;

  // Loop through all stacks and get the PLMN if valid. Prefer 3GPP system over 3GPP2.
  pSsInfo = policyman_ss_get_ss_info(subsId);
  for (stackId = SYS_MODEM_STACK_ID_1; !stackIs3gpp && stackId < SYS_MODEM_STACK_ID_MAX; stackId++)
  {
    plmnValid = policyman_ss_stack_plmn_is_valid(stackId, pSsInfo);
    if (plmnValid)
    {
      pStack = policyman_ss_get_stack_info(subsId, stackId);
      stackIs3gpp = policyman_ss_sys_mode_is_3gpp(pStack->sys_mode);
    }
  }

  if (NULL != pStack)
  {
    policyman_ss_get_plmn_from_stack(pStack, pPlmn);
  }

  return (NULL != pStack);
}

/*-------- policyman_ss_get_mcc_from_subs --------*/
sys_mcc_type policyman_ss_get_mcc_from_subs(
  sys_modem_as_id_e_type              subsId
)
{
  sys_plmn_id_s_type plmn;
  boolean             haveMcc;  
  sys_mcc_type        mcc     = NO_MCC;

  haveMcc = policyman_ss_get_plmn_from_subs(subsId, &plmn);

  if (haveMcc)
  {
    mcc = policyman_plmn_get_mcc(&plmn);
    
    POLICYMAN_MSG_HIGH_2("Determined MCC %d for SUBS %d based on network service", mcc, subsId);
  }

  return mcc;
}


/*===========================================================================
  FUNCTION POLICYMAN_SS_SET_PLMN()

  DESCRIPTION
    SET PLMN for stack into PM STATE
 ===========================================================================*/
boolean policyman_ss_set_plmn(
  policyman_ss_info_t          *pInfo,
  sys_modem_stack_id_e_type    num_stack,
  sys_plmn_id_s_type           *pPlmn
)
{
  boolean changed = FALSE;
  policyman_stack_info_t *pStack;
  
  if (
        pInfo != NULL
     && pPlmn != NULL
     )
  {
    pStack = &pInfo->stack[num_stack];

    if( !policyman_plmns_are_equal(&pStack->plmn, pPlmn) )
    {
      pStack->plmn = *pPlmn;
      pStack->is_operational = TRUE;
      policyman_ss_determine_plmn_validity(pInfo->asubs_id);
      changed = TRUE;
    }
  }

  return changed;
}

/*===========================================================================
  FUNCTION POLICYMAN_SS_GET_SRV_STATUS()
 
  DESCRIPTION
    GET srv_status for stack
 ===========================================================================*/
sys_srv_status_e_type policyman_ss_get_srv_status(  
  policyman_ss_info_t        *pInfo, 
  sys_modem_stack_id_e_type  num_stack
)
{
  return (pInfo != NULL)? pInfo->stack[num_stack].srv_status : SYS_SRV_STATUS_NO_SRV;
}

/*===========================================================================
  FUNCTION POLICYMAN_SS_SET_SRV_STATUS()
 
  DESCRIPTION
    SET srv_status for stack into PolicyMan state

 RETURN VALUE
   TRUE is policyman state is updated correctly, FALSE otherwise
 ===========================================================================*/
boolean policyman_ss_set_srv_status(  
  policyman_ss_info_t        *pInfo, 
  sys_modem_stack_id_e_type  num_stack,
  sys_srv_status_e_type      srv_status
)
{
  boolean changed = FALSE;

  if ( pInfo != NULL && 
       (pInfo->stack[num_stack].srv_status != srv_status)
     )
  {
    pInfo->stack[num_stack].srv_status = srv_status;
    changed =  pInfo->stack[num_stack].is_operational = TRUE;
  }

  return changed;
}

/*-------- policyman_ss_check_subs_in_service --------*/
STATIC boolean policyman_ss_check_subs_in_service(
  sys_modem_as_id_e_type subsId
)
{
  policyman_stack_info_t  *pStack1 = policyman_ss_get_stack_info(subsId, SYS_MODEM_STACK_ID_1);
  policyman_stack_info_t  *pStack2 = policyman_ss_get_stack_info(subsId, SYS_MODEM_STACK_ID_2);

  /* don't check stack operational as it won't have service if inactive */
  return    policyman_ss_status_is_service(pStack1->srv_status)
         || policyman_ss_status_is_service(pStack2->srv_status);
}

/*-------- policyman_ss_subs_is_in_pwr_save --------*/
boolean policyman_ss_subs_is_in_pwr_save(
  sys_modem_as_id_e_type subsId
)
{
  boolean                  isPwrSave = TRUE;
  boolean                  subsHasOprStack  = FALSE;
  size_t                   stackID;
  policyman_stack_info_t  *pStack;

  for(stackID = SYS_MODEM_STACK_ID_1; isPwrSave && stackID < SYS_MODEM_STACK_ID_MAX; stackID++)
  {
    pStack = policyman_ss_get_stack_info(subsId, stackID);
    subsHasOprStack |=  pStack->is_operational;

    if (pStack->is_operational)
    {
      isPwrSave = (pStack->srv_status == SYS_SRV_STATUS_PWR_SAVE);
    }
  }

  return isPwrSave && subsHasOprStack;
}

/*===========================================================================
  FUNCTION POLICYMAN_SERVING_DOMAIN_CAMPED()
 
  DESCRIPTION
    Check CM serving domain is CAMPED ONLY 
===========================================================================*/
boolean policyman_serving_domain_camped(
  sys_srv_domain_e_type srv_domain
)
{
  return srv_domain == SYS_SRV_DOMAIN_CAMPED;
}

/*===========================================================================
  FUNCTION POLICYMAN_SRV_STATUS_CHECK_NO_SRV()
 
  DESCRIPTION
    Check if Service Status is No service
===========================================================================*/
boolean policyman_srv_status_check_no_srv(
  sys_srv_status_e_type srv_status
)
{
  return srv_status == SYS_SRV_STATUS_NO_SRV
         || srv_status == SYS_SRV_STATUS_NONE;
}

/*===========================================================================
  FUNCTION POLICYMAN_EXTRACT_SYS_ID()

  DESCRIPTION
    Get PLMN for 3gpp and 3gpp2
===========================================================================*/
void policyman_extract_sys_id(
  struct policyman_cmss_evt_cb_s      *cm_ss_evt,
  policyman_stack_info_t              *pStack,
  cm_mm_ss_info_s_type                *pcmStack,
  size_t                               stackNum
)
{
  POLICYMAN_MSG_HIGH_2("Received system id type %d and service domain %d", pcmStack->sys_id.id_type, pcmStack->srv_domain);
  
  switch (pcmStack->sys_id.id_type)
  {
    case SYS_SYS_ID_TYPE_UMTS:
      
      /* Ignore Main stack 3GPP PLMN if srv_domain is CAMPED_ONLY
               */
      if(!policyman_serving_domain_camped(pcmStack->srv_domain))
      {
        pStack->id_type  = pcmStack->sys_id.id_type;
        pStack->plmn = pcmStack->sys_id.id.plmn;
        policyman_plmn_print(&pStack->plmn);
      }
      break;
  
    default:
      POLICYMAN_MSG_ERROR_0("PLMN ignored");
      break;
  }
}

/*-------- policyman_determine_current_mcc_for_subs --------*/
STATIC sys_mcc_type policyman_determine_current_mcc_for_subs(
  sys_modem_as_id_e_type   subsId
)
{
  boolean       haveLocationFromService;
  sys_mcc_type  mcc  = NO_MCC;


  /*  See if we have a location based on service.  Give preference to the
   *  current subscription's MCC if it has one; otherwise find a subscription
   *  that *does* have an MCC.
   */
  haveLocationFromService =     policyman_subs_has_location(subsId)
                             || policyman_find_subs_with_location_info(&subsId);

  if (haveLocationFromService)
  {
    mcc = policyman_ss_get_mcc_from_subs(subsId);

    // If we have a location based on service, the HLOS MCC should be cleared.
    policyman_state_clear_hlos_mcc();
  }
  else 
  {
    // We don't have service, so use the HLOS MCC if there is one.
    mcc = policyman_state_get_hlos_mcc();
    POLICYMAN_MSG_HIGH_2("Determined MCC %d for SUBS %d based on HLOS value", mcc, subsId);
  }

  return mcc;
}


/*-------- policyman_ss_update_current_mcc --------*/
STATIC void policyman_ss_update_current_mcc(
  sys_modem_as_id_e_type  subsId
)
{
  sys_mcc_type  mcc;

  mcc = policyman_determine_current_mcc_for_subs(subsId);
  policyman_state_update_subs_mcc(subsId, mcc);
}

/*-------- policyman_ss_update_current_mcc_on_all_subs --------*/
STATIC void policyman_ss_update_current_mcc_on_all_subs(
  void
)
{
  sys_modem_as_id_e_type subsId;
  size_t                 nSim;

  (void)policyman_get_current_num_sim(&nSim);

  for (subsId = SYS_MODEM_AS_ID_1; subsId < nSim; ++subsId)
  {
    policyman_ss_update_current_mcc(subsId);
  }
}

/*-------- policyman_ss_3gpp_plmn_print --------*/
STATIC void policyman_ss_3gpp2_plmn_print(
  policyman_stack_info_t const *pStack
)
{
  size_t                 index;
  sys_plmn_id_s_type     plmn;
  
  for (index = 0; index < pStack->mcc_list_3gpp2.count; index++)
  {
    policyman_plmn_from_mcc_mnc(&plmn, pStack->mcc_list_3gpp2.mcc_list[index], pStack->mnc_3gpp2);
    policyman_plmn_print(&plmn);
  }
}

/*-------- policyman_ss_print_stack_info --------*/
STATIC void policyman_ss_print_stack_info(
  policyman_state_t const     *pState,
  sys_modem_as_id_e_type       subsId
)
{
  policyman_stack_info_t const  *pStack;
  size_t                         stackId;

  POLICYMAN_MSG_HIGH_0("=========== Stack info BEGIN ===========");

  /*  Print out stack information
   */
  for (stackId = 0 ; stackId < SYS_MODEM_STACK_ID_MAX; ++stackId)
  {
    pStack = policyman_ss_get_stack_info( subsId, stackId);
    POLICYMAN_MSG_HIGH_2("Subs %d stack %d:", subsId, stackId);
    POLICYMAN_MSG_HIGH_3("   srv_status = %d sys_mode = %d plmn_valid = %d",
                              pStack->srv_status,
                              pStack->sys_mode,
                              pStack->serving_info_valid
                         );

    /* For Non-CDMA, PLMN print is fine
           For CDMA it should come from MCC list
      */
    if (pStack->is_operational)
    {
      if (pStack->sys_mode == SYS_SYS_MODE_CDMA)
      {
        policyman_ss_3gpp2_plmn_print(pStack);
      }
      else
      {
        policyman_plmn_print((sys_plmn_id_s_type *)&pStack->plmn);
      }
    }
  }

  POLICYMAN_MSG_HIGH_0("=========== Stack info END ===========");
}

/*-------- policyman_ss_update_state --------*/
void policyman_ss_update_state(
  policyman_ss_info_t     *pCurrentInfo,
  policyman_ss_info_t     *pNewInfo,
  sys_modem_as_id_e_type   asubs_id
)
{
  size_t  stackId;
  boolean stateChanged[SYS_MODEM_STACK_ID_MAX];
  boolean haveService;

  if (pCurrentInfo == NULL || pNewInfo == NULL)   
  { 
    return; 
  }

  POLICYMAN_MSG_HIGH_1("Updating SS state for subs %d",  asubs_id);

  for (stackId = SYS_MODEM_STACK_ID_1 ; stackId < SYS_MODEM_STACK_ID_MAX ; stackId++)
  {
    policyman_stack_info_t *pCurStack = &pCurrentInfo->stack[stackId];
    policyman_stack_info_t *pNewStack = &pNewInfo->stack[stackId];

    stateChanged[stackId] =
          pCurStack->is_operational != pNewStack->is_operational
       || !policyman_plmns_are_equal(&pCurStack->plmn, &pNewStack->plmn)
       || !policyman_3gpp2_mccs_are_equal(&pCurStack->mcc_list_3gpp2, &pNewStack->mcc_list_3gpp2)
       || policyman_is_new_srv_status_changed(pCurStack->srv_status, pNewStack->srv_status)
       || pNewStack->reject_cause > 0;
  }

  /*  Update the state
   */
  *pCurrentInfo = *pNewInfo;
  haveService = policyman_ss_some_stack_has_service(pCurrentInfo);

  /* Evaluate precondition on SUBS after validating PLMN */
  policyman_ss_determine_plmn_validity(asubs_id);
  policyman_ss_update_precond_for_subs(asubs_id, pCurrentInfo);

  /*  If there was no state change or no SIM refresh pending, just return.
   */
  if (  !stateChanged[SYS_MODEM_STACK_ID_1]
     && !stateChanged[SYS_MODEM_STACK_ID_2]
     && !policyman_uim_get_sim_refresh(asubs_id)
     )
  {
    POLICYMAN_MSG_HIGH_1("Subs %d: information not changed on any stack", asubs_id);
    goto Done;
  }

  /* Update current_mcc as we have got our state updated now with the latest info 
     */
  policyman_ss_update_current_mcc_on_all_subs();
  policyman_ss_print_stack_info(policyman_state_get_state(), asubs_id);

  /*  If the service status is valid for any stack, handle updates
   */
  if (haveService)
  {
    POLICYMAN_MSG_HIGH_3("Information changed on subs %d stacks: main %d, gw_hybr %d, handle update",
                        asubs_id,
                        stateChanged[SYS_MODEM_STACK_ID_1], 
                        stateChanged[SYS_MODEM_STACK_ID_2]
                        );

    policyman_state_handle_update(asubs_id);
  }

Done: 
  policyman_policy_notify_service(haveService, asubs_id);
}


/*===========================================================================
  FUNCTION POLICYMAN_MSIM_GET_STACK_INFO()
 
  DESCRIPTION
    Get Main stack info
  ===========================================================================*/
void policyman_msim_get_stack_info(
  policyman_ss_info_t                 *cm_ss_info, 
  struct policyman_cmss_evt_cb_s      *cm_ss_evt
)
{

  sys_modem_as_id_e_type    subs     = cm_ss_info->asubs_id;
  sys_modem_stack_id_e_type stack_id = SYS_MODEM_STACK_ID_1;

  /* get current state info locally
    */
  policyman_stack_info_t          *pStack   = &cm_ss_info->stack[stack_id];
  cm_mm_ss_info_s_type            *pcmStack = &cm_ss_evt->info;

  pStack->is_operational = TRUE;

  /* get stack info based on corresponding mask
    */
  if (pcmStack->changed_fields & (CM_SS_EVT_SYS_ID_MASK))
  {
    policyman_extract_sys_id(cm_ss_evt, pStack, pcmStack,stack_id);
  }

  if(pcmStack->changed_fields & CM_SS_EVT_SRV_STATUS_MASK)
  {
    pStack->srv_status = pcmStack->srv_status;
    POLICYMAN_MSG_HIGH_3("subs %d stack %d - srv_status %d", subs, stack_id, pcmStack->srv_status);
  }

  /* Ignore sys mode change if associated service status is NO_SRV/PWR_SAVE
    */
  if( (pcmStack->changed_fields & CM_SS_EVT_SYS_MODE_MASK)
    && policyman_ss_status_is_service(pcmStack->srv_status)
    )
  {
    pStack->sys_mode = pcmStack->sys_mode;
    POLICYMAN_MSG_HIGH_3("subs %d stack %d - sys_mode %d", subs, stack_id, pStack->sys_mode);
  }

  if(pcmStack->changed_fields & CM_SS_EVT_SRV_DOMAIN_MASK)
  {
    pStack->srv_domain = pcmStack->srv_domain;
    POLICYMAN_MSG_HIGH_3("subs %d stack %d - srv_domain %d", subs, stack_id, pcmStack->srv_domain);
  }

  if(pcmStack->changed_fields & CM_SS_EVT_ACTIVE_BAND_MASK)
  {
    pStack->active_band = pcmStack->active_band;
    POLICYMAN_MSG_HIGH_3("subs %d stack %d - active_band %d", subs, stack_id, pcmStack->active_band);
  }

  /* update reject cause if we have pending reject, reset pending reject
    */
  pStack->reject_cause = pStack->pending_reject;
  pStack->pending_reject = 0;    

  /* Decide if 1x_oos/hybr_oos timer needs to be started or stopped
    */
  policyman_ss_check_stack_oos(pStack, stack_id, subs);
}

/*===========================================================================
  FUNCTION POLICYMAN_MSIM_GET_SS_INFO()
 
  DESCRIPTION
    GET changed CM SS Info from event into local policyman_ss_info_t structure
 
  PARAMETERS
    ss_info : Policyman local structure to retain changed SS info
    ss_evt : Incoming CM SS EVENT for Multisim
  ===========================================================================*/
void policyman_msim_get_ss_info(
  policyman_ss_info_t                 *pSsInfo, 
  struct policyman_cmss_evt_cb_s      *pSsEvt
)
{
  if(pSsInfo != NULL && pSsEvt != NULL)
  {
    sys_modem_as_id_e_type subsId = SYS_MODEM_AS_ID_1;

    /* restore the old PM state info */
    *pSsInfo = *policyman_ss_get_ss_info(subsId);    

    /* get stack info */
    policyman_msim_get_stack_info(pSsInfo, pSsEvt);
  }
  else
  {
    POLICYMAN_MSG_ERROR_2("msim_get_ss_info NULL pointer, data not copied - cm ss info %x, cm ss evt %x", pSsInfo, pSsEvt);
  }
}

/*===========================================================================
  FUNCTION POLICYMAN_MSIM_SS_UPDATE_REJECT_INFO()
 
  DESCRIPTION
    Update for reject info into PM serving system state for Multisim
===========================================================================*/
void policyman_msim_ss_update_reject_info(
  struct policyman_cmss_evt_cb_s *ss_evt
)
{
  size_t                      i;
  sys_modem_as_id_e_type      subs_id    = SYS_MODEM_AS_ID_1;
  policyman_ss_info_t        *cm_ss_info = policyman_ss_get_ss_info(subs_id);
  sys_modem_stack_id_e_type   stack_id   = SYS_MODEM_STACK_ID_1;

  if(cm_ss_info)
  {    
    /* update reject cause in state
      */
    cm_ss_info->stack[stack_id].pending_reject = ss_evt->info.reg_reject_info.reject_cause;
    POLICYMAN_MSG_HIGH_4("subs %d stack %d - reject_cause %d, reject_domain %d", 
                          cm_ss_info->asubs_id, 
                          stack_id,
                          ss_evt->info.reg_reject_info.reject_cause, 
                          ss_evt->info.reg_reject_info.reject_srv_domain
                        );
  }
}


/*===========================================================================
  FUNCTION POLICYMAN_EXECUTE_MSIM_PROCESS_CM_REJECT_EVT()
 
  DESCRIPTION
    Execute CM SS Set update for reject info for Multisim
 ===========================================================================*/
void policyman_execute_msim_process_cm_reject_evt(
  policyman_cmd_t *pCmd
)
  {
  struct policyman_cmss_evt_cb_s *ss_evt = (struct policyman_cmss_evt_cb_s*) pCmd;

  if(ss_evt)
  {
    /* only update the state, don't run the rules now
       next SS event will be LIMITED service so run rules there
    */
    policyman_msim_ss_update_reject_info(ss_evt);
  }
}

/*===========================================================================
  FUNCTION POLICYMAN_EXECUTE_MSIM_PROCESS_CMSS_EVT()
 
  DESCRIPTION
    Execute CM SS Set update and policy check for Multisim
 ===========================================================================*/
void policyman_execute_msim_process_cmss_evt(
  policyman_cmd_t *pCmd
)
{
  struct policyman_cmss_evt_cb_s *ss_evt   = (struct policyman_cmss_evt_cb_s*) pCmd;
  struct policyman_ss_info_t      ss_info;
  sys_modem_as_id_e_type          asubs_Id = SYS_MODEM_AS_ID_1;

  if(ss_evt)
  {
    policyman_msim_get_ss_info(&ss_info, ss_evt);
    policyman_state_msim_update_ss_info(&ss_info, asubs_Id);
  }
}

/*===========================================================================
  FUNCTION POLICYMAN_MSIM_CMSS_EVENT_CB()

  DESCRIPTION
    Handle CM SS event callback function for Multisim

  PARAMETERS
    evt     : CM SS EVENT type
    p_info : actual payload of CM SS EVENT
===========================================================================*/
void policyman_msim_cmss_event_cb(
  cm_ss_event_e_type               evt, 
  const cm_mm_ss_info_s_type      *p_info
)
{
  struct policyman_cmss_evt_cb_s *pCmd    = NULL;
  execute_fn_t                    policyman_execute_msim_cm_event = NULL;
  sys_modem_as_id_e_type          subs_id = SYS_MODEM_AS_ID_1;

  switch(evt)
  {
    case CM_SS_EVENT_REG_REJECT:
      POLICYMAN_MSG_HIGH_1("subs %d received msim CM_SS_EVENT_REG_REJECT", subs_id);
      policyman_execute_msim_cm_event = policyman_execute_msim_process_cm_reject_evt;
      break;

    case CM_SS_EVENT_SRV_CHANGED:
      POLICYMAN_MSG_HIGH_1("subs %d received msim CM_SS_EVENT_SRV_CHANGED", subs_id);
      policyman_execute_msim_cm_event = policyman_execute_msim_process_cmss_evt;
      break;

    default:
      break;     
  }

  if(policyman_execute_msim_cm_event)
  {
    pCmd = (struct policyman_cmss_evt_cb_s *) policyman_cmd_new( sizeof(struct policyman_cmss_evt_cb_s), 
                                                                  policyman_execute_msim_cm_event, 
                                                                  NULL,
                                                                  NULL                               // No policy associated with CM SS Event
                                                               );

    pCmd->evt    = evt;
    pCmd->info = *p_info;

    policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
    ref_cnt_obj_release(pCmd);
  }
}

/*=============================================================================
  Conditions based on serving system.
=============================================================================*/

typedef struct
{
  POLICYMAN_CONDITION_BASE;

  char const      *pSetName;
  policyman_set_t *pPlmnSet;
} policyman_serving_plmn_condition_t;

typedef struct
{
  POLICYMAN_CONDITION_BASE;

  sys_sys_mode_mask_e_type rat_mask;
} policyman_serving_rat_condition_t;

typedef struct
{
  POLICYMAN_CONDITION_BASE;

  policyman_set_t *pRfBandSet;   
} policyman_serving_rf_band_condition_t;

typedef struct
{
  POLICYMAN_CONDITION_BASE;

  policyman_set_t *pRejectCauseSet;   
} policyman_reg_reject_condition_t;


/*-------- policyman_check_serving_plmn_in_stack --------*/
STATIC boolean policyman_check_serving_plmn_in_stack
(
  policyman_stack_info_t const *pStack,
  policyman_set_t              *pPlmnSet
)
{
  boolean                 retval = FALSE;
  size_t                  index;
  sys_plmn_id_s_type      plmn;
  sys_plmn_mcc_list const *pPlmnList;

  if (pStack->serving_info_valid)
  {
    switch (pStack->sys_mode)
    {
      case SYS_SYS_MODE_CDMA:
        /* get PLMN for each MCC in list and check in set
            */
        pPlmnList = &pStack->mcc_list_3gpp2;
        POLICYMAN_MSG_HIGH_1("checking %d CDMA PLMNs in set", pPlmnList->count);

        for (index = 0; index < pPlmnList->count; index++)
        {
          policyman_plmn_from_mcc_mnc(&plmn, pPlmnList->mcc_list[index] , pStack->mnc_3gpp2);
          policyman_plmn_print(&plmn);
          retval = policyman_set_contains(pPlmnSet, (void *)&plmn);
          if (retval)
          {
            break;
          }
        }
        break;

      default:
        retval = policyman_set_contains(pPlmnSet, (void *)&pStack->plmn);
        break;
    }
  }

  return retval;
}


/*-------- evaluate_serving_plmn_in --------*/
STATIC boolean evaluate_serving_plmn_in(
  policyman_condition_t const *pCondition,
  void                        *pCtx,
  sys_modem_as_id_e_type       subsId
)
{
  policyman_serving_plmn_condition_t *pCond  = (policyman_serving_plmn_condition_t *) pCondition;
  policyman_state_t                  *pState = ((policy_execute_ctx_t *) pCtx)->pState;
  policyman_stack_info_t const       *pStacks;
  policyman_stack_info_t const       *pStack;
  size_t                              nStacks;
  size_t                              stack;
  boolean                             result = FALSE;

  POLICYMAN_MSG_HIGH_0("evaluating <serving_plmn_in>");

  /*  Return FALSE if the SS precondition is not met.
   */
  if (!policyman_state_check_precondition_met(POLICYMAN_PRECOND_SS, subsId))
  {
    POLICYMAN_MSG_ERROR_0("SS Precondition is not met, returning FALSE");
    goto Done;
  }

  /*  Find the PLMN set to use; if we don't have one we'll return FALSE.
   */
  if (pCond->pPlmnSet == NULL && pCond->pSetName != NULL)
  {    
    pCond->pPlmnSet = (policyman_set_t *) mre_named_object_find(
                            pCond->pSetName,
                                                                 POLICYMAN_NAMED_PLMN_SET,
                            pCond->pPolicy
                          );

    POLICYMAN_MEM_FREE_IF(pCond->pSetName);

    if (NULL != pCond->pPlmnSet)
    {
      ref_cnt_obj_add_ref(pCond->pPlmnSet);
    }

  }
  if (pCond->pPlmnSet == NULL)             
  {
    POLICYMAN_MSG_ERROR_0("PLMN set not found");
    goto Done;
  }

  /*  Print stack information.  This call probably wants to go into
   *  policyman_state_handle_update(), so we print it at the beginning
   *  of each evaluation of the rules.  Perhaps include more state information
   *  in this as well - call it policyman_display_state() or something.
   */
  policyman_ss_print_stack_info(pState, subsId);

  /*  Get stack information in easy-to-use variables
   */
  pStacks = pState->pSubsState[subsId]->pSsInfo->stack;
  nStacks = ARR_SIZE(pState->pSubsState[subsId]->pSsInfo->stack);

  /*  Check to see if some stack's PLMN is in the list.
   */
  for (stack = 0 ; stack < nStacks ; ++stack)
  {
    pStack = pStacks + stack;

    if (policyman_check_serving_plmn_in_stack(pStack,  pCond->pPlmnSet))
    {
      POLICYMAN_MSG_HIGH_1("PLMN of stack %d is in the PLMN set", stack);
      result = TRUE;
         goto Done;
    }
    else
    {
      POLICYMAN_MSG_HIGH_4("PLMN not found in stack %d, is_operational %d, srv_status %d, plmn valid %d", 
                            stack, 
                            pStack->is_operational, 
                            pStack->srv_status,
                            pStack->serving_info_valid );
    }
  }

Done:
  POLICYMAN_MSG_HIGH_1("condition <serving_plmn_in> returns %d", result);
  return result;
}


/*-------- policyman_condition_serving_plmn_evaluate --------*/
STATIC boolean policyman_condition_serving_plmn_evaluate(
  mre_condition_t const *pCondition,
  void                        *pCtx
)
{
  policyman_condition_t const * pCond = (policyman_condition_t const *)pCondition;
  return policyman_evaluate_cond(pCond, pCtx, evaluate_serving_plmn_in);
}

/*-------- policyman_condition_serving_plmn_dtor --------*/
STATIC void policyman_condition_serving_plmn_dtor(
  void  *pObj
)
{
  policyman_serving_plmn_condition_t *pCondition = 
                                    (policyman_serving_plmn_condition_t *) pObj;

  REF_CNT_OBJ_RELEASE_IF(pCondition->pPlmnSet);
  POLICYMAN_MEM_FREE_IF(pCondition->pSetName);
  policyman_condition_dtor(pCondition);
}


/*-------- policyman_condition_serving_plmn_new --------*/
mre_status_t policyman_condition_serving_plmn_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
)
{
  mre_status_t                        status = MRE_STATUS_ERR_INVALID_CONDITION;
  policyman_serving_plmn_condition_t  *pCondition = NULL;
  const char                          *pStr;
  policyman_set_t                     *pSet = NULL;
  char                                *pSetName = NULL;
  policyman_subs_t                     pm_subs;

  /*  See if there is a named PLMN list to use for this condition.
   */
  pStr = policyman_xml_get_attribute(pElem, "list");
  if (pStr != NULL)
  {
    pSetName = policyman_str_dup(pStr);
  }
  else
  {
    /*  No name - read an inline list.
     */
    pStr = policyman_xml_get_text(pElem);
    if (pStr != NULL)
    {
      pSet = policyman_plmn_list_read(pStr);
    }  
  }
  
  /*  If there is neither an inline or named list, bail.
   */
  if (pSet == NULL && pSetName == NULL)
  {
    goto Done;
  }

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);
  if (MRE_FAILED(status))
  {
    goto Done;
  }

  /*  Create the condition
   */
  pCondition = (policyman_serving_plmn_condition_t *)
                  policyman_mem_alloc(sizeof(policyman_serving_plmn_condition_t));
  ref_cnt_obj_init(pCondition, policyman_condition_serving_plmn_dtor);

  pCondition->isTrue = policyman_condition_serving_plmn_evaluate;
  pCondition->pPlmnSet = pSet;
  pCondition->pSetName = pSetName;
  pCondition->pmSubsId = pm_subs;

  status = MRE_STATUS_SUCCESS;

Done:
  *ppCondition = (mre_condition_t *) pCondition;

  return status;
}

/*-------- policyman_check_3gpp_mcc_in_stack --------*/
STATIC boolean policyman_check_3gpp_mcc_in_stack(
  policyman_stack_info_t const    *pStack,
  policyman_set_t                 *pMccSet
)
{
  uint32    servingMcc;
  
  servingMcc = policyman_plmn_get_mcc((sys_plmn_id_s_type *)&pStack->plmn);
  
  return    pStack->serving_info_valid 
         && policyman_set_contains(pMccSet, &servingMcc);
}


/*-------- policyman_check_3gpp2_mcc_in_stack --------*/
STATIC boolean policyman_check_3gpp2_mcc_in_stack(
  policyman_stack_info_t const   *pStack,
  policyman_set_t                *pMccSet
)
{  
  size_t   index;
  boolean  result = FALSE;
  
   /* Check if CDMA stack can be used
           if yes then iteratively check all MCCs in list to determine we are HOME
      */
  if (pStack->serving_info_valid)
  {
    for (index = 0; !result && index < pStack->mcc_list_3gpp2.count; index++)
    {
      result = policyman_set_contains(pMccSet, (void *)&pStack->mcc_list_3gpp2.mcc_list[index]);
    }
  }

  return result;
}

/*-------- policyman_ss_is_subs_mcc_in_set --------*/
STATIC boolean policyman_ss_is_subs_mcc_in_set(
  policyman_state_t       *pState,
  policyman_set_t         *pMccSet,
  sys_modem_as_id_e_type   subsId
)
{
  policyman_stack_info_t const *pStack;
  size_t                        stackId;
  boolean                       mccIsOnSubs = FALSE;  
  
  /*  Check to see if some stack's MCC is in the list.
   */
  for (stackId = 0 ; !mccIsOnSubs && stackId < SYS_MODEM_STACK_ID_MAX ; ++stackId)
  {
    pStack = policyman_ss_get_stack(pState, subsId, stackId);
  
    if (pStack->sys_mode == SYS_SYS_MODE_CDMA)
    {
      mccIsOnSubs = policyman_check_3gpp2_mcc_in_stack(pStack, pMccSet);
    }
    else
    {
      mccIsOnSubs = policyman_check_3gpp_mcc_in_stack(pStack, pMccSet);
    } 
  }

  return mccIsOnSubs;
}


/*-------- policyman_ss_ensure_mcc_set --------*/
STATIC mre_set_t * policyman_ss_ensure_mcc_set(
  policyman_mcc_condition_t *pCond
)
{
  // If we don't have a set but *do* have a set name, try to find the named set.
  if (pCond->pMccSet == NULL && pCond->pSetName != NULL)
  {    
    pCond->pMccSet = (policyman_set_t *) mre_named_object_find( pCond->pSetName,
                                                                POLICYMAN_NAMED_MCC_SET,
                                                                pCond->pPolicy );

    POLICYMAN_MEM_FREE_IF(pCond->pSetName);

    if (NULL != pCond->pMccSet)
    {
      ref_cnt_obj_add_ref(pCond->pMccSet);
    }
  }

  return pCond->pMccSet;
}


/*-------- policyman_condition_mcc_dtor --------*/
void policyman_condition_mcc_dtor(
  void  *pObj
)
{
  policyman_mcc_condition_t *pCondition;
  pCondition = (policyman_mcc_condition_t *) pObj;

  REF_CNT_OBJ_RELEASE_IF(pCondition->pMccSet);
  POLICYMAN_MEM_FREE_IF(pCondition->pSetName);
  policyman_condition_dtor(pCondition);
}


/*-------- policyman_map_condition_subs_to_subs_id --------*/
STATIC sys_modem_as_id_e_type policyman_map_condition_subs_to_subs_id(
  mre_condition_t const *pCondition,
  policyman_subs_t       condSubs
)
{
  sys_modem_as_id_e_type subsId   = SYS_MODEM_AS_ID_1;
  sys_modem_as_id_e_type thisSubs = CONDITION_SUBS(pCondition);
  
  switch (condSubs)
  {
    case SUBS_THIS:
    case SUBS_MMODE:
      subsId = thisSubs;
      break;

    case SUBS_OTHER:
    default:
      if (thisSubs == SYS_MODEM_AS_ID_1)
      {
        subsId = SYS_MODEM_AS_ID_2;
      }
      break;
  }

  return subsId;
}

/*-------- policyman_condition_location_mcc_evaluate --------*/
STATIC boolean policyman_condition_location_mcc_evaluate(
  mre_condition_t const *pCondition,
  void                        *pCtx
)
{
  policyman_set_t           *pMccSet;
  sys_modem_as_id_e_type     subsId;
  policyman_mcc_condition_t *pCond        = (policyman_mcc_condition_t *) pCondition;
  policyman_state_t         *pState       = ((policy_execute_ctx_t *)pCtx)->pState;
  boolean                    result       = FALSE;
  boolean                    haveLocationFromService;

  subsId = policyman_map_condition_subs_to_subs_id(pCondition, pCond->subs);

  pMccSet = policyman_ss_ensure_mcc_set(pCond);
    
  /*  Without a set, we can't work.
   */
  if (pMccSet == NULL)             
  { 
    POLICYMAN_MSG_ERROR_0("named MCC set not found");
    goto Done;
  }

  haveLocationFromService =    policyman_subs_has_location(subsId)
                            || policyman_find_subs_with_location_info(&subsId);


  // Try evaluation by looking into Modem and HLOS MCC   
  if (haveLocationFromService)
  {
    result = policyman_ss_is_subs_mcc_in_set(pState, pMccSet, subsId);
  }
  else if (policyman_state_check_precondition_met(POLICYMAN_PRECOND_LOCATION, subsId))
  {
    sys_mcc_type hlos_mcc = policyman_state_get_hlos_mcc();

    result = policyman_set_contains(pMccSet, &hlos_mcc);
    POLICYMAN_MSG_HIGH_2("looking into HLOS MCC %d, match found = %d", hlos_mcc, result);
  }

Done:

 POLICYMAN_MSG_HIGH_2("condition <location_mcc_in> for subs %d returns %d", subsId, result);
 return result;
}

/*-------- policyman_condition_location_mcc_new --------*/
mre_status_t policyman_condition_location_mcc_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t              **ppCondition
)
{
  return  policyman_plmn_mcc_new( pElem, 
                                  pPolicy, 
                                  ppCondition, 
                                  policyman_condition_location_mcc_evaluate
                                );
}

/*-----------------------------------------------------------------------------
  OOS SCAN CNT
-----------------------------------------------------------------------------*/

typedef struct
{
  POLICYMAN_CONDITION_BASE;

  policyman_subs_t        subsId;
  policyman_comparator_t  test;
  int                     limit;
} policyman_oos_scan_cnt_condition_t;

/*-------- policyman_str_to_service_status --------*/
policyman_comparator_t policyman_str_to_comparator(
  char const  *pStr
)
{
  typedef struct
  {
    const char                 *pStr;
    policyman_comparator_t      test;
  } comparator_map;

  policyman_comparator_t     test = POLICYMAN_CMP_INVALID;

  static comparator_map map[] =
  {
    {"=",       POLICYMAN_CMP_EQUALS},
    {">",       POLICYMAN_CMP_EXCEEDS},
    {">=",      POLICYMAN_CMP_EQUALS_OR_EXCEEDS},
  };

  size_t  i;
  char    token[32];

  policyman_get_token(&pStr, token, sizeof(token));

  for (i = 0 ; i < ARR_SIZE(map) ; ++i)
  {
    if (strcasecmp(token, map[i].pStr) == 0)
    {
      test = map[i].test;
      break;
    }
  }

#if defined(TEST_FRAMEWORK)
  #error code not present
#endif /* +TEST_FRAMEWORK. */

  return test;
}

/*-------- policyman_ss_get_min_acq_fail_count --------*/
STATIC int policyman_ss_get_min_acq_fail_count(
  sys_modem_as_id_e_type subsId
)
{
  policyman_stack_info_t   *pStk1Info;
  policyman_stack_info_t   *pStk2Info;
  
  pStk1Info = policyman_ss_get_stack_info(subsId, SYS_MODEM_STACK_ID_1);
  pStk2Info = policyman_ss_get_stack_info(subsId, SYS_MODEM_STACK_ID_2); 

  POLICYMAN_MSG_MED_4("get acq_fail_count: stack1 opr %d acq_fail_cnt %d, stack2 opr %d acq_fail_cnt %d",
                      pStk1Info->is_operational,
                      pStk1Info->acq_fail_cnt,
                      pStk2Info->is_operational,
                      pStk2Info->acq_fail_cnt);
  
  if (pStk1Info->is_operational && pStk2Info->is_operational)
  {
    return MIN(pStk1Info->acq_fail_cnt, pStk2Info->acq_fail_cnt);
  }
  else
  {
    return pStk1Info->acq_fail_cnt;
  }

}

/*-------- policyman_condition_oos_scan_cnt_evaluate --------*/
STATIC boolean policyman_condition_oos_scan_cnt_evaluate(
  mre_condition_t const *pCondition,
  void                  *pCtx
)
{
  policyman_oos_scan_cnt_condition_t  *pCond = (policyman_oos_scan_cnt_condition_t *) pCondition;
  int                                  scanCnt;
  boolean                              ret = FALSE;
  sys_modem_as_id_e_type               policySubs = CONDITION_SUBS(pCond);
  
  scanCnt = policyman_ss_get_min_acq_fail_count(policySubs);
  
  switch(pCond->test)
  {
    case POLICYMAN_CMP_EQUALS:
      ret = (scanCnt == pCond->limit);
      break;

    case POLICYMAN_CMP_EXCEEDS:
      ret = (scanCnt > pCond->limit);
      break;

    case POLICYMAN_CMP_EQUALS_OR_EXCEEDS:
      ret = (scanCnt >= pCond->limit);
      break;

    default:
      break;
  }
  
  POLICYMAN_MSG_HIGH_2("condition <oos_scan_cnt> test %d returns %d", pCond->test, ret);
  return ret;
}

/*-------- policyman_condition_oos_scan_cnt_dtor --------*/
STATIC void policyman_condition_oos_scan_cnt_dtor(
  void  *pObj
)
{
  policyman_oos_scan_cnt_condition_t *pCondition;
  
  pCondition = (policyman_oos_scan_cnt_condition_t *) pObj;
  policyman_condition_dtor(pCondition);
}

/*-------- policyman_condition_oos_scan_cnt_new --------*/
mre_status_t policyman_condition_oos_scan_cnt_new(
  policyman_xml_element_t const  *pElem,
  mre_policy_t                   *pPolicy,
  mre_condition_t               **ppCondition
)
{
  mre_status_t                        status = MRE_STATUS_ERR_INVALID_CONDITION;
  policyman_oos_scan_cnt_condition_t *pCondition = NULL;
  char const                         *pStr;
  policyman_comparator_t              test;
  int                                 limit;
  policyman_subs_t                    subs;
  
  pStr = mre_xml_get_attribute(pElem, "test");
  if (pStr == NULL)
  {
    POLICYMAN_UT_MSG_0("<oos_scan_cnt>: 'test' attribute required");
    goto Done;
  }

  test = policyman_str_to_comparator(pStr);
  if (test == POLICYMAN_CMP_INVALID)
  {
    POLICYMAN_UT_MSG_0("<oos_scan_cnt>: invalid 'test' attribute");
    goto Done;

  }

  pStr = NULL;
  pStr = mre_xml_get_attribute(pElem, "value");
  if (pStr == NULL)
  {
    POLICYMAN_UT_MSG_0("<oos_scan_cnt>: 'value' attribute required");
    goto Done;
  }

  limit = atoi(pStr);

  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &subs);
  if (MRE_FAILED(status))
  {
    goto Done;
  }
  
  pCondition = (policyman_oos_scan_cnt_condition_t *)policyman_mem_alloc(sizeof(*pCondition));
  ref_cnt_obj_init(pCondition, policyman_condition_oos_scan_cnt_dtor);
  
  pCondition->isTrue = policyman_condition_oos_scan_cnt_evaluate;
  pCondition->subsId = subs;
  pCondition->test   = test;
  pCondition->limit  = limit;

  status = MRE_STATUS_SUCCESS;

  Done:
  if (MRE_FAILED(status))
  {
    REF_CNT_OBJ_RELEASE_IF(pCondition);
  }
   
  *ppCondition = (mre_condition_t *) pCondition;
  return status;

}

/*-----------------------------------------------------------------------------
  Enforce Full RAT mode
-----------------------------------------------------------------------------*/

typedef struct
{
  POLICYMAN_CONDITION_BASE;
} policyman_enforce_fullrat_condition_t;

/*-------- policyman_condition_enforce_full_rat_evaluate --------*/
STATIC boolean policyman_condition_enforce_full_rat_evaluate(
  mre_condition_t const *pCondition,
  void                        *pCtx
  )
{
  boolean                result;
  policy_execute_ctx_t  *pContext = (policy_execute_ctx_t *)pCtx;

  /* Full RAT is enforced only when XML defines the boolean.
        PLMN id type stays undefined at "first" PWR_SAVE event after coming 
        ONLINE, but not on a regular PWR_SAVE event sent after service loss.
   */
  result =    policyman_state_get_full_rat()
           && policyman_ss_subs_is_in_pwr_save(pContext->asubs_id)
           && !policyman_ss_subs_sys_id_is_defined(pContext->asubs_id);

  POLICYMAN_MSG_HIGH_2("condition <enforce_full_rat> on subs %d returns %d", pContext->asubs_id, result);
  return result;
}

/*-------- policyman_condition_enforce_full_rat_new --------*/
mre_status_t policyman_condition_enforce_full_rat_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  )
{
  mre_status_t                           status     = MRE_STATUS_ERR_MALFORMED_XML;
  policyman_enforce_fullrat_condition_t *pCondition = NULL;

  pCondition = (policyman_enforce_fullrat_condition_t *)
                  policyman_mem_alloc(sizeof(policyman_enforce_fullrat_condition_t));

  ref_cnt_obj_init(pCondition, policyman_condition_dtor);
  
  pCondition->isTrue = policyman_condition_enforce_full_rat_evaluate;

  *ppCondition = (mre_condition_t *) pCondition;
  status = MRE_STATUS_SUCCESS;

  POLICYMAN_MSG_HIGH_1("policyman_condition_enforce_full_rat_new status %d", status);
  return status;
}

/*-----------------------------------------------------------------------------
  Have Service
-----------------------------------------------------------------------------*/

/*-------- policyman_condition_have_service_evaluate --------*/
STATIC boolean policyman_condition_have_service_evaluate(
  mre_condition_t const *pCondition,
  void                  *pCtx
)
{
  boolean   haveService = FALSE;
  size_t    subs;

  for (subs = SYS_MODEM_AS_ID_1; !haveService && subs < POLICYMAN_NUM_SUBS; subs++)
  {
    haveService = policyman_ss_check_subs_in_service((sys_modem_as_id_e_type) subs);
  }

  POLICYMAN_MSG_HIGH_1("condition <have_service> returns %d", haveService);
  return haveService;
}

/*-------- policyman_condition_have_service_new --------*/
mre_status_t policyman_condition_have_service_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t              **ppCondition
)
{
  mre_status_t     status;
  mre_condition_t *pCondition = NULL;

  pCondition = (mre_condition_t *)policyman_mem_alloc(sizeof(mre_condition_t));
  ref_cnt_obj_init(pCondition, mre_condition_dtor);
  
  pCondition->isTrue = policyman_condition_have_service_evaluate;

  *ppCondition = pCondition;
  status = MRE_STATUS_SUCCESS;

  POLICYMAN_MSG_HIGH_1("policyman_condition_have_service_new returned status %d", status);
  return status;
}

/*===========================================================================
  FUNCTION POLICYMAN_GET_CM_SERVING_INFO()
 
  DESCRIPTION
    GET changed CM SS Info from event into local policyman_ss_info_t structure
 
  PARAMETERS
    ss_info : Policyman local structure to retain changed SS info
    ss_evt : Incoming CM Serving Info
  ===========================================================================*/
void policyman_get_cm_serving_info
(
  policyman_ss_info_t *pm_ss_info, 
  policyman_cm_serving_info_t *cm_serving_info
)
{
  sys_modem_stack_id_e_type pm_stack;
  policyman_stack_info_t *pStack;

  if(pm_ss_info != NULL && cm_serving_info != NULL)
  {    
    // get stack info from serving system info
    if(policyman_check_stack_is_valid(cm_serving_info->stack))
    {
      // Mark stack operational since CM is sending data for that stack        
      pm_stack = cm_serving_info->stack;
      pStack = &pm_ss_info->stack[pm_stack];
      
      pStack->is_operational = TRUE;
      pStack->srv_status = cm_serving_info->srv_status;

      // get sys_mode 
      pStack->sys_mode = cm_serving_info->sys_mode;

      POLICYMAN_MSG_HIGH_4("cm_serving_info: subs=%d, stack %d, received sys_mode %d, srv_status %d",
                          cm_serving_info->asubs_id,
                          pm_stack, 
                          cm_serving_info->sys_mode, 
                          cm_serving_info->srv_status );

      // update the PLMN id - CM guarantees to give only 3GPP PLMN id
      pStack->id_type = cm_serving_info->id_type;
      if(cm_serving_info->id_type == SYS_SYS_ID_TYPE_UMTS)
      {
        pStack->plmn = cm_serving_info->plmn;
        POLICYMAN_MSG_HIGH_1("stack %d PLMN:", pm_stack);
        policyman_plmn_print(&pStack->plmn);
      }
      else
      {
        POLICYMAN_MSG_ERROR_0("PLMN id is not 3GPP, PLMN not updated");
      }
    }
    else
    {
      POLICYMAN_MSG_ERROR_1("cm_serving_info ignored, cm stack %d not handled", cm_serving_info->stack);
    }
  }
  else
  {
    POLICYMAN_MSG_ERROR_2("get_cm_serving_info NULL pointer, data not copied - pm_ss_info %x, cm_serving_info %x", pm_ss_info, cm_serving_info);
  }
}


/*===========================================================================
  FUNCTION POLICYMAN_EXECUTE_PROCESS_CM_SERVING_INFO()
 
  DESCRIPTION
    Process serving system information received from Call Manager 
 ===========================================================================*/
void policyman_execute_process_cm_serving_info
(
  policyman_cmd_t *pCmd
)
{
  struct policyman_cm_serving_info_cb_s *serving_info = (struct policyman_cm_serving_info_cb_s*) pCmd;
  struct policyman_ss_info_t             ss_info;
  policyman_cm_serving_info_t           *pInfo = NULL;

  if(serving_info != NULL)
  {
    pInfo = &serving_info->info;

    /* Allow API to update for all service domains but for subs 0, subs 1 & subs2
      */
    if(  pInfo
      && pInfo->asubs_id > SYS_MODEM_AS_ID_NONE 
      && pInfo->asubs_id < SYS_MODEM_AS_ID_NO_CHANGE
      )
    {
      /* restore the old state first
          */
      ss_info = *policyman_ss_get_ss_info(pInfo->asubs_id);
      policyman_get_cm_serving_info(&ss_info, pInfo);

      policyman_state_update_ss_info(&ss_info, pInfo->asubs_id);

      policyman_ss_update_current_mcc_on_all_subs();
    }
  }
}

/*===========================================================================
  FUNCTION POLICYMAN_CM_SERVING_INFO_CB()

  DESCRIPTION
    Get Serving system information from CM

  PARAMETERS
    pCmServingInfo    Pointer to CM serving system information.
===========================================================================*/
void policyman_cm_serving_info_cb(
  policyman_cm_serving_info_t *pCmServingInfo
)
{
  struct policyman_cm_serving_info_cb_s *pCmd;

  if (pCmServingInfo != NULL)
  {
    POLICYMAN_MSG_HIGH_1("received cm serving info cb for subs %d", pCmServingInfo->asubs_id);

    pCmd = (struct policyman_cm_serving_info_cb_s *) policyman_cmd_new( sizeof(struct policyman_cm_serving_info_cb_s), 
                                                                 policyman_execute_process_cm_serving_info, 
                                                                 NULL,
                                                                 NULL                               // No policy associated with CM SS Event
                                                               );

    pCmd->info = *pCmServingInfo;

    policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
    ref_cnt_obj_release(pCmd);
  }
}

/*-------- policyman_msim_non_multimode_subs_in_service --------*/
boolean policyman_msim_non_multimode_subs_in_service(
  void
)
{
  size_t nSim = 1;

  /* if it is single SIM then return FALSE 
  */
 (void)policyman_get_current_num_sim(&nSim);
  if (nSim == 1)
  {
    POLICYMAN_MSG_MED_0("device is single SIM, no service on non-mmode SUBS");
    return FALSE;
  }

  return policyman_ss_some_stack_has_service(policyman_ss_get_ss_info(SYS_MODEM_AS_ID_2));
}

/*-------- policyman_msim_non_multimode_subs_has_sglte_mcc --------*/
boolean policyman_msim_non_multimode_subs_has_sglte_mcc(
  void
)
{  
  policyman_ss_info_t    *pSsInfo;
  boolean                 retval       = FALSE;
  policyman_stack_info_t *pStack;
  size_t                  i;
  policyman_set_t        *pSet;
  sys_mcc_type            mcc;

  /* get sglte_mccs set
    */
  pSet = (policyman_set_t*) mre_named_object_find_with_subs(
                                 "sglte_mccs",
                                 POLICYMAN_NAMED_MCC_SET,
                                 SYS_MODEM_AS_ID_1,
                                 policyman_get_namespace_info() );
  
  /*  Without a set, we can't work.
   */
  if (pSet == NULL)             
  { 
    POLICYMAN_MSG_ERROR_0("named PLMN set not found");
    goto Done;
  }   

  pSsInfo = policyman_ss_get_ss_info(SYS_MODEM_AS_ID_2);

  /* Print stack info
   */
  policyman_ss_print_stack_info(policyman_state_get_state(), SYS_MODEM_AS_ID_2);

  /* check if MCC is in set
   */
   for(i=SYS_MODEM_STACK_ID_1; i<SYS_MODEM_STACK_ID_MAX; i++)
   {
    pStack = &pSsInfo->stack[i];
     mcc = policyman_ss_get_mcc(pSsInfo, i);     

     if ( policyman_ss_stack_plmn_is_valid(i, pSsInfo) 
       && policyman_set_contains(pSet, &mcc)
       )
     {
       POLICYMAN_MSG_HIGH_1("MCC of stack %d is in the MCC set", i);
       retval = TRUE;
     }
     else
     {
       POLICYMAN_MSG_HIGH_3("MCC not found in stack %d, is_operational %d, serving_info_valid %d", 
                             i, 
                             pStack->is_operational, 
                             pStack->serving_info_valid );
     }
   }

Done:
  return retval;
}

/*-------- policyman_execute_process_hlos_mcc --------*/
void policyman_execute_process_hlos_mcc(
  policyman_cmd_t *pCmd
)
{  
  struct policyman_hlos_mcc_cmd_s *pHlosInfo = (struct policyman_hlos_mcc_cmd_s *) pCmd;

  /*  Sanitize the input parameter. MCC is 3 digits
   */
  if (  pHlosInfo->status != 0
     || pHlosInfo->mcc < 1
     || pHlosInfo->mcc > 999
     )
  {
    POLICYMAN_MSG_ERROR_0("ignoring HLOS MCC, status is not SUCCESS or MCC out of range !!");
    return;
  }
 
  policyman_state_update_hlos_mcc( pHlosInfo->mcc, 
                                   pHlosInfo->confidenceLevel, 
                                   pHlosInfo->status );

  policyman_ss_update_current_mcc_on_all_subs();

  policyman_state_handle_update(SYS_MODEM_AS_ID_1);
}

/*-------- policyman_set_hlos_mcc --------*/
policyman_status_t policyman_set_hlos_mcc(
  sys_mcc_type  mcc,
  size_t        confidenceLevel,
  size_t        status
)
{
  struct policyman_hlos_mcc_cmd_s *pCmd;
 
  POLICYMAN_MSG_HIGH_3( "received HLOS MCC %d, confidence level %d, HLOS status %d", 
                        mcc, 
                        confidenceLevel, 
                        status );
 
  pCmd = (struct policyman_hlos_mcc_cmd_s *) policyman_cmd_new( sizeof(struct policyman_hlos_mcc_cmd_s), 
                                                                policyman_execute_process_hlos_mcc, 
                                                                NULL,
                                                                NULL );
 
  pCmd->mcc             = mcc;
  pCmd->confidenceLevel = confidenceLevel;
  pCmd->status          = status;

  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
  ref_cnt_obj_release(pCmd);

  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_get_current_mcc --------*/
policyman_status_t policyman_get_current_mcc(
  sys_modem_as_id_e_type subsId,
  sys_mcc_type           *pMcc
)
{
  if ( subsId < SYS_MODEM_AS_ID_1
     || subsId >= POLICYMAN_NUM_SUBS
     || pMcc == NULL
     )
  {
    POLICYMAN_MSG_ERROR_1("Invalid argument: subsId %d", subsId);
    return POLICYMAN_STATUS_ERR_INVALID_ARGS;
  }
  
  /* fetch MCC and return
   */
  *pMcc = policyman_state_get_subs_mcc(subsId);
  return POLICYMAN_STATUS_SUCCESS;
}

/*-------- policyman_execute_process_acq_fail_ind --------*/
void policyman_execute_process_acq_fail_ind(
  policyman_cmd_t *pCmd
)
{
  struct policyman_cm_report_acq_fail_s *pAcqFailCmd = (struct policyman_cm_report_acq_fail_s*) pCmd;
  policyman_stack_info_t                *pStackInfo;

  pStackInfo = policyman_ss_get_stack_info(pAcqFailCmd->subsId,
                                           pAcqFailCmd->stackId);
  pStackInfo->acq_fail_cnt += 1;

  POLICYMAN_MSG_HIGH_1("policyman_execute_process_acq_fail_ind: acq_fail_cnt %d", pStackInfo->acq_fail_cnt);

  policyman_ss_determine_plmn_validity(pAcqFailCmd->subsId);
  policyman_state_handle_update(pAcqFailCmd->subsId);
}

/*-------- policyman_report_acq_fail --------*/

void policyman_report_acq_fail(
  sys_modem_as_id_e_type         subs_id,
  sys_modem_stack_id_e_type      stack_id 
)
{
  struct policyman_cm_report_acq_fail_s *pCmd;

  POLICYMAN_MSG_HIGH_2("policyman_report_acq_fail: subs %d stack %d", subs_id, stack_id);

  pCmd = (struct policyman_cm_report_acq_fail_s *) policyman_cmd_new( sizeof(struct policyman_cm_report_acq_fail_s), 
                                                               policyman_execute_process_acq_fail_ind, 
                                                               NULL,
                                                               NULL      
                                                               );

  pCmd->subsId = subs_id;
  pCmd->stackId = stack_id;

  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
  ref_cnt_obj_release(pCmd);
}
#endif