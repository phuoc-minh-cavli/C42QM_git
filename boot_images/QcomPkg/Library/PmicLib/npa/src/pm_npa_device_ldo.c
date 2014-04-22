/*! \file
*  
*  \brief  pm_npa_device_ldo.c ----This file contains init functions for the RPM PMIC NPA
*          driver
*  \details 
*  
*  
*  &copy; Copyright 2012-2014 QUALCOMM Technologies Inc, All Rights Reserved
*/
/*===========================================================================

            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/25/17   ks      removed the redundant code
11/17/14   al      Replacing UEFI banned function
06/13/14   va      Updating HDMI EXT NPA node and PCIE 
06/20/14   al      Arch updated 
04/04/14   al      Warning cleanup 
01/16/13   umr     Add Suppressible request support.
03/07/12   hs      Use DAl APIs to replace memory allocation. 
02/12/12   wra     KVP based device setting initial file 
===========================================================================*/
/*===========================================================================

             INCLUDE FILES FOR MODULE

===========================================================================*/
#include <Library/QcomBaseLib.h>

#include "pm_npa_device_ldo.h"
#include "pm_utils.h" 
#include "pm_target_information.h"
#include "pm_sw_event.h"
#include "pm_npa.h"

// LDO Create and Destroy Client Callbacks
void pm_npa_ldo_kvps_create_client_fcn(npa_client *client)
{
  // Allocate storage space because NPA discards the data otherwise.
  pm_malloc(sizeof(pm_npa_ldo_kvps), (void **)&client->resource_data);

  DALSYS_memset(client->resource_data, 0, sizeof(pm_npa_ldo_kvps));
}

void pm_npa_ldo_kvps_destroy_client_fcn(npa_client *client)
{
  // Clean up our allocated storage space.
  DALSYS_Free(client->resource_data);
}

static void
pm_npa_ldo_kvps_aggregate(pm_npa_ldo_kvps *kvps, pm_npa_ldo_kvps *request)
{
  // Aggregate Software Enable
  kvps->sw_enable_value = MAX(kvps->sw_enable_value, request->sw_enable_value);

  // Aggregate Software Mode Enable
  kvps->ldo_sw_mode_value = MAX(kvps->ldo_sw_mode_value, request->ldo_sw_mode_value);

  // Aggregate ORed values of Pin Control Enable
  kvps->pin_ctrl_enable_value = kvps->pin_ctrl_enable_value | request->pin_ctrl_enable_value;

  // Aggregate ORed values of Pin Control Power Mode
  kvps->pin_ctrl_power_mode_value = kvps->pin_ctrl_power_mode_value | request->pin_ctrl_power_mode_value;

  // Aggregate Maximum Voltage
  kvps->uv_value = MAX(kvps->uv_value, request->uv_value);

  // Aggregate Summation of Current
  kvps->current_value = kvps->current_value + request->current_value;

  // Aggregate Headroom Voltage
  kvps->head_room_voltage_value = MAX(kvps->head_room_voltage_value, request->head_room_voltage_value);

  // Aggregate corners
  kvps->corner_value = MAX(kvps->corner_value, request->corner_value);

  // Aggregate floor
  kvps->floor_value = MAX(kvps->floor_value, request->floor_value);

  return;
}

npa_resource_state pm_npa_ldo_kvps_update_fcn(npa_resource *resource, npa_client_handle client)
{
  npa_client_handle active_client = resource->clients;
  pm_npa_ldo_kvps  *kvps, *reqd_kvps, *client_req;
  npa_client_type req_type = client->type;
  
  // Firstly, save the client's request so that we don't lose it later.
  client_req = (pm_npa_ldo_kvps *)client->resource_data;
  DALSYS_memscpy(client_req, sizeof(pm_npa_ldo_kvps), NPA_PENDING_REQUEST(client).pointer.vector, sizeof(pm_npa_ldo_kvps));
  NPA_PENDING_REQUEST(client).pointer.vector = (npa_resource_state *)client_req;

  /* Ovearall agg, both Reqd and supp */
  kvps = (pm_npa_ldo_kvps *)resource->state_vector;
  DALSYS_memscpy(kvps, sizeof(pm_npa_ldo_kvps), client_req, sizeof(pm_npa_ldo_kvps));

  /* Reqd request Only*/
  reqd_kvps = (pm_npa_ldo_kvps *)resource->required_state_vector;
  if (req_type == NPA_CLIENT_VECTOR)
  {
    DALSYS_memscpy(reqd_kvps, sizeof(pm_npa_ldo_kvps), client_req, sizeof(pm_npa_ldo_kvps));
  }

  while (active_client)
  {
    // Find the saved request for this client.
    pm_npa_ldo_kvps *request = active_client->resource_data;

    // If this is the current client, we have already started with his values.
    if (client == active_client)
    {
      active_client = active_client->next;
      continue;
     }
    /* Aggregate for both Suppressable and Required */
    pm_npa_ldo_kvps_aggregate (kvps, request);
	pm_sw_event(PMIC_LDO_AGG1, 3, 0, kvps->sw_enable_value, kvps->ldo_sw_mode_value); 
	pm_sw_event(PMIC_LDO_AGG2, 3, 0, kvps->pin_ctrl_enable_value, kvps->pin_ctrl_power_mode_value); 
	pm_sw_event(PMIC_LDO_AGG3, 3, 0, kvps->uv_value, kvps->current_value); 
	pm_sw_event(PMIC_LDO_AGG4, 3, 0, kvps->head_room_voltage_value, kvps->corner_value); 

    if (req_type == NPA_CLIENT_VECTOR && active_client->type == NPA_CLIENT_VECTOR)
    {
      /* Aggregate only Required */
      pm_npa_ldo_kvps_aggregate (reqd_kvps, request);
	  pm_sw_event(PMIC_CLK_BUF_AGG2, 3, 0, kvps->sw_enable_value, kvps->pin_ctrl_enable_value);
    }
    active_client = active_client->next;
  }
        
   return kvps->uv_value; // Return the voltage for quick perusal in logs; "real state" is in resource->state_vector.
}

npa_resource_plugin pm_npa_ldo_kvps_plugin =
{
  pm_npa_ldo_kvps_update_fcn,
  NPA_CLIENT_VECTOR | NPA_CLIENT_SUPPRESSIBLE_VECTOR,
  pm_npa_ldo_kvps_create_client_fcn,
  pm_npa_ldo_kvps_destroy_client_fcn,
  NULL,
  NULL
};

/**
 * pm_npa_ldo_kvps_init
 *
 * See pm_npa_vreg.h
 */
pm_err_flag_type
pm_npa_ldo_kvps_init (pm_npa_ldo_kvps *kvps)
{
  if (kvps == NULL)
  {
    return PM_ERR_FLAG__INVALID_POINTER;
  }

  kvps->sw_enable_key = PM_NPA_KEY_SOFTWARE_ENABLE;
  kvps->sw_enable_size = 4;
  kvps->sw_enable_value = 0;

  kvps->ldo_sw_mode_key = PM_NPA_KEY_LDO_SOFTWARE_MODE;
  kvps->ldo_sw_mode_size = 4;
  kvps->ldo_sw_mode_value = 0;

  kvps->pin_ctrl_enable_key = PM_NPA_KEY_PIN_CTRL_ENABLE;
  kvps->pin_ctrl_enable_size = 4;
  kvps->pin_ctrl_enable_value = 0;

  kvps->pin_ctrl_power_mode_key = PM_NPA_KEY_PIN_CTRL_POWER_MODE;
  kvps->pin_ctrl_power_mode_size = 4;
  kvps->pin_ctrl_power_mode_value = 0;

  kvps->uv_key = PM_NPA_KEY_MICRO_VOLT;
  kvps->uv_size = 4;
  kvps->uv_value = 0;

  kvps->current_key = PM_NPA_KEY_CURRENT;
  kvps->current_size = 4;
  kvps->current_value = 0;

  kvps->head_room_voltage_key = PM_NPA_KEY_HEAD_ROOM;
  kvps->head_room_voltage_size = 4;
  kvps->head_room_voltage_value = 0;

  kvps->bypass_allowed_key = PM_NPA_KEY_BYPASS_ALLOWED_KEY;
  kvps->bypass_allowed_size = 4;
  kvps->bypass_allowed_value = 0;

  kvps->corner_key = PM_NPA_KEY_CORNER_LEVEL_KEY;
  kvps->corner_size = 4;
  kvps->corner_value = 0;

  kvps->floor_key = PM_NPA_KEY_ACTIVE_FLOOR;
  kvps->floor_size = 4;
  kvps->floor_value = 0;

  return PM_ERR_FLAG__SUCCESS;
}

npa_resource_plugin *pm_npa_get_ldo_kvps_plugin(void)
{
    return(&pm_npa_ldo_kvps_plugin);
}
