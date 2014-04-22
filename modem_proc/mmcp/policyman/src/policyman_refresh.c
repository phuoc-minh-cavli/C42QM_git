/**
  @file policyman_refresh.c

  @brief  Handling for PM refresh on SIM swap
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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/src/policyman_refresh.c#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/
#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
#include "policyman_i.h"
#include "policyman_refresh.h"
#include "policyman_task.h"

#ifdef FEATURE_MODEM_CONFIG_REFRESH

/*=============================================================================
  MCFG Refresh Event callback definition.
=============================================================================*/
struct policyman_refresh_evt_cb_s
{
  POLICYMAN_CMD_HDR;
  
  mcfg_refresh_info_s_type info;
};

/*===========================================================================
  FUNCTION POLICYMAN_EXECUTE_PROCESS_MCFG_REFRESH_EVT()
 
  DESCRIPTION
    Execute mcfg sim refresh event 
 
  RETURN VALUE
    None
 
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
===========================================================================*/
void policyman_execute_process_mcfg_refresh_evt(
  policyman_cmd_t *pCmd
)
{
  struct policyman_refresh_evt_cb_s  *refresh_evt  = (struct policyman_refresh_evt_cb_s*) pCmd;
  mcfg_refresh_info_s_type            refresh_info = refresh_evt->info;

  policyman_suspend();
  
  // Clear out cfgitem and per-subs state information
  policyman_per_subs_deinit(refresh_info.sub_index);

  // Restart policyman info -- re-read everything due to a potential new xmls
  policyman_per_subs_init(refresh_info.sub_index);

  // let mcfg know were done
  if ( mcfg_refresh_done(refresh_info.reference_id) )
  {
    POLICYMAN_MSG_HIGH_0("Refresh done notification sent to MCFG");
  }
  else
  {
    POLICYMAN_MSG_ERROR_0("Refresh done notification to MCFG failed");
  }

  policyman_resume();
}

/*===========================================================================
  FUNCTION POLICYMAN_MCFG_REFRESH_CB()

  DESCRIPTION
    Handle MCFG SIM refresh callback

  PARAMETERS
    evt     : CM Phone EVENT type
    p_info : actual payload of CM Phone EVENT

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean policyman_mcfg_refresh_cb( 
  mcfg_refresh_info_s_type *p_info
)
{
  struct policyman_refresh_evt_cb_s *pCmd = NULL;

  POLICYMAN_MSG_HIGH_1("received mcfg sim refresh for sim %d", p_info->sub_index);

  pCmd = (struct policyman_refresh_evt_cb_s *) policyman_cmd_new( sizeof(struct policyman_refresh_evt_cb_s), 
                                                                  policyman_execute_process_mcfg_refresh_evt, 
                                                                  NULL,
                                                                  NULL );  // No policy associated with CM Phone Event

  pCmd->info = *p_info;

  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
  ref_cnt_obj_release(pCmd);

  return TRUE;
}


/*------------------------------------------------------------------------
  FUNCTION POLICYMAN_REGISTER_FOR_MCFG_REFRESH()

  DESCRIPTION
    With this function, Policy Manager registers with MCFG refresh events as a client
--------------------------------------------------------------------------*/
void policyman_register_for_mcfg_refresh(
  void
)
{
  mcfg_refresh_registration_s_type  reg;
  boolean                           success = FALSE;
  
  reg.order     = MCFG_REFRESH_ORDER_30;
  reg.slot_mask = MCFG_REFRESH_INDEX_MASK_NONE;
  reg.sub_mask  = MCFG_REFRESH_INDEX_MASK_ALL;
  reg.cb        = policyman_mcfg_refresh_cb;

  success = mcfg_refresh_register( &reg );

  if ( !success )
  {
    POLICYMAN_MSG_ERROR_0("mcfg_refresh_register failed");
  }
}

#endif

#endif