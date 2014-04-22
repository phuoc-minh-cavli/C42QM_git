/******************************************************************************
  @file: loc_for_constrained_tunc.h
  @brief:   This module provides an interface for the constrained tunc client 
            that runs in location middle ware
  

  DESCRIPTION

  INITIALIZATION AND SEQUENCING REQUIREMENTS
   N/A

  -----------------------------------------------------------------------------
Copyright (c) 2018 Qualcomm Technologies, Inc. 
Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/loc_mw/src/loc_constrained_tunc_client.c#1 $
$DateTime: 2020/07/21 21:23:44 $
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
09/11/14   sj      Initial version
======================================================================*/

#include "location_service_iot_v02.h"
#include "loc_api_internal.h"
#include "loc_constrained_tunc_client.h"
#include "loc_qmi_shim.h"
#include "loc_client.h"
#include "aries_os_api.h"
#include "mgp_api.h"
#include "sm_api.h"
#include "gnss_consts.h"

/*===========================================================================
FUNCTION loc_mw_query_gnss_energy_consumed

DESCRIPTION
  This function requests the energy consumed so far by the GNSS engine.
 
PARAMETERS 
 
DEPENDENCIES

RETURN VALUE 

SIDE EFFECTS
===========================================================================*/
boolean loc_mw_query_gnss_energy_consumed
(
  locQmiShimCltInfoStructT *pz_loc_client_ptr,
  const qmiLocQueryGNSSEnergyConsumedReqMsgT_v02 *pz_msg,
  qmiLocGenRespMsgT_v02  *pz_qmi_resp
)
{
  boolean v_RetVal = FALSE;
  os_IpcMsgType      *p_ipc_msg_ptr = NULL;
  
  LOC_MSG_HIGH("TUNC CONSTRAINT: Query GNSS Energy Consumed!!", 0, 0, 0);
  
  p_ipc_msg_ptr = os_IpcCreate(sizeof(int32), IPC_ALLOCATION_DYNAMIC,
                               (uint32)THREAD_ID_LOC_MIDDLEWARE);

  if (NULL == p_ipc_msg_ptr)
  {
     LOC_MSG_ERROR ("TUNC CONSTRAINT: IPC allocation fail!",
                     0, 0, 0);
     return v_RetVal;
  }
  
 if (NULL != pz_loc_client_ptr)
  { 
	p_ipc_msg_ptr->q_MsgId            = LM_MIDDLEWARE_MSG_ID_QUERY_GNSS_ENERGY_CONSUMED;
	p_ipc_msg_ptr->q_DestThreadId     = (uint32)THREAD_ID_LOC_MIDDLEWARE;
	p_ipc_msg_ptr->q_Size             = sizeof(int32);
	*(p_ipc_msg_ptr->p_Data) = pz_loc_client_ptr->l_LocClientHandle;
	*((loc_client_handle_type *) p_ipc_msg_ptr->p_Data) = pz_loc_client_ptr->l_LocClientHandle;
	
	if(os_IpcSend(p_ipc_msg_ptr, (uint32)THREAD_ID_LOC_MIDDLEWARE) == FALSE)
		{
			LOC_MSG_ERROR ("TUNC CONSTRAINT:IPC send fail", 0, 0, 0);
			/* no matter whether os_IpcDelete succeeds or not, there is nothing we can do */
			(void) os_IpcDelete(p_ipc_msg_ptr);
			return v_RetVal;
		}
	else
		{
			v_RetVal = TRUE;
		}
  }
  else 
  {
    LOC_MSG_ERROR ("TUNC CONSTRAINT:NULL client ptr received", 0, 0, 0);
  } 
 

  pz_qmi_resp->resp.error  = QMI_ERR_NONE_V01;
  pz_qmi_resp->resp.result = QMI_RESULT_SUCCESS_V01;
  return v_RetVal;
}

/*===========================================================================
FUNCTION loc_mw_handle_query_gnss_energy_consumed

DESCRIPTION
  This function handles the request to query energy consumed so far by the 
  GNSS engine in loc mw task context.
 
PARAMETERS 
 
DEPENDENCIES

RETURN VALUE 

SIDE EFFECTS
===========================================================================*/
void loc_mw_handle_query_gnss_energy_consumed(loc_client_handle_type l_LocClientHandle)
{
  mgp_GetEnergyConsumptionType              z_energy_consumption     = { 0 };
  qmiLocQueryGNSSEnergyConsumedIndMsgT_v02  z_loc_ind_msg;


  /* Calculate energy burnt! */
  mgp_GetEnergyConsumption (&z_energy_consumption,
                             MGP_ENERGYMODE_SINCE_REBOOT);

  if(z_energy_consumption.b_IsValid)
  {
    z_loc_ind_msg.energyConsumedSinceLastBoot = z_energy_consumption.u_Energy;
  }
  else
  {
    z_loc_ind_msg.energyConsumedSinceLastBoot = 0xffffffffffffffff;
  }
  
  LOC_MSG_HIGH("Loc_mw_handle_query_gnss_energy_consumed z_energy_consumption.b_IsValid: %d, energy: %d",
  z_energy_consumption.b_IsValid, z_loc_ind_msg.energyConsumedSinceLastBoot , 0);

  /* Calculate energy burnt! */
  mgp_GetEnergyConsumption (&z_energy_consumption,
                             MGP_ENERGYMODE_ACROSS_REBOOTS);

  if(z_energy_consumption.b_IsValid)
  {
    z_loc_ind_msg.energyConsumedSinceFirstBoot = z_energy_consumption.u_Energy;
  }
  else
  {
    z_loc_ind_msg.energyConsumedSinceFirstBoot = 0xffffffffffffffff;
  }
  
   locQmi_ProcessInd(l_LocClientHandle,
                     QMI_LOC_QUERY_GNSS_ENERGY_CONSUMED_IND_V02,
                    &z_loc_ind_msg, 
                     eQMI_LOC_SUCCESS_V02);
   

}

