/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             P M    NPA   S E R V I C E S

GENERAL DESCRIPTION
  This file contains initialization functions for NPA

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2010-2013           by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/npa/src/pm_npa.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/16/13   umr     Add Suppressible request support.
03/01/2011 umr     Use Scalar request for VS and NCP.
12/14/2010 umr     NPA Transaction support.
09/26/10   umr     Add CXO scalar resource support.
06/03/10   umr     Created.
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "npa.h"
#include "npa_resource.h"
#include "pm_npa_device_clk_buff.h"
#include "pm_npa_device_ldo.h"
#include "pm_npa_device_smps.h"
#include "pm_npa_device_vs.h"
#include "pm_npa.h"
#include "pm_target_information.h"
#include "pm_ulog.h"

/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/
/*===========================================================================

                LOCAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/

/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION pm_npa_init                                EXTERNAL FUNCTION

DESCRIPTION
    This function initializes the NPA for PMIC.

    It does the following:
    * Initializes the PMIC NPA Node and resources.

INPUT PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  NPA Framework should be available in the build being compiled for

SIDE EFFECTS
  NONE.

===========================================================================*/
void
pm_npa_init (void)
{
} /* end of pm_apps_proc_npa_init() */

/*===========================================================================

FUNCTION pm_npa_resource_set

DESCRIPTION
    This function handles all the client requests.

    It does the following:
     Gets the user data from resource handle and passes requests
     the Device NPA Resources.

INPUT PARAMETERS
  Resource- The resource that is being requested
  Client-   Client requesting the resource
  mode_id-  Mode to which the Resource needs to be set.

RETURN VALUE
  State.

DEPENDENCIES
  NPA Framework should be available in the build being compiled for

SIDE EFFECTS
  NONE.

===========================================================================*/
npa_resource_state
pm_npa_process_rsrc (npa_client_handle client,
                     npa_resource *resource,
                     npa_resource_state mode_id,
                     uint32 index, uint32 dep_idx)
{
  pm_npa_pam_client_cfg_type *pam_data;
  pam_data = (pm_npa_pam_client_cfg_type *) resource->definition->data;

  npa_pass_request_attributes( client, NPA_DEPENDENCY( resource, dep_idx ) );

  switch (pam_data[index].rail_type)
  {
    case PM_NPA_VREG_SMPS:
      {
        pm_npa_smps_kvps *vreg_data;
        vreg_data = (pm_npa_smps_kvps *)pam_data[index].rail_data;
        vreg_data = &vreg_data[mode_id];   
        
        npa_issue_vector_request( NPA_DEPENDENCY( resource, dep_idx ), 
               sizeof (pm_npa_smps_kvps) / sizeof( npa_resource_state ),
                (npa_resource_state *)vreg_data );
      }
      break;

    case PM_NPA_VREG_LDO:
      {
        pm_npa_ldo_kvps *vreg_data;
        vreg_data = (pm_npa_ldo_kvps *)pam_data[index].rail_data;
        vreg_data = &vreg_data[mode_id];   
        npa_issue_vector_request( NPA_DEPENDENCY( resource, dep_idx ), 
               sizeof (pm_npa_ldo_kvps) / sizeof( npa_resource_state ),
                (npa_resource_state *)vreg_data );
      }
      break;

    case PM_NPA_VREG_VS:
      {
        pm_npa_vs_kvps *vreg_data;
        vreg_data = (pm_npa_vs_kvps *)pam_data[index].rail_data;
        vreg_data = &vreg_data[mode_id];   

        npa_issue_vector_request( NPA_DEPENDENCY( resource, dep_idx ), 
            sizeof (pm_npa_vs_kvps) / sizeof( npa_resource_state ),
            (npa_resource_state *)vreg_data );
      }
      break;

    case PM_NPA_CLK_BUFF:
      {
        pm_npa_clk_buff_kvps *cxo_data;
        cxo_data = (pm_npa_clk_buff_kvps *)pam_data[index].rail_data;
        cxo_data = &cxo_data[mode_id];   
        
        npa_issue_vector_request( NPA_DEPENDENCY( resource, dep_idx ), 
            sizeof (pm_npa_clk_buff_kvps) / sizeof( npa_resource_state ),
            (npa_resource_state *)cxo_data );
      }
      break;

    default:
      return 0;
  }
  return mode_id;
}

npa_resource_state
pm_npa_resource_set (npa_resource      *resource,
                     npa_client_handle  client,
                     npa_resource_state mode_id)
{
  uint32 i, dep;
  npa_resource_state mode_id_req;

  if (client->type == NPA_CLIENT_INITIALIZE)
  {
    /*
     * Initial state of resources to be set if any, Apart from device initial
     * states
     */
  } else
  {
    PM_LOG_MSG_INFO("ResName=%s, ClientName=%s, ModeId=%d, Type=0x%x", 
    client->resource_name, client->name, mode_id, client->type);
    
    /* Use the aggregation corresponding to the current client->type.
     * The overall agg isn't important here as much as the individual
     * REQUIRED and SUPPRESSIBLE aggregations, because we have 2 client
     * handles to the dependency and it will aggregate across these anyway.
     */
    if (client->type == NPA_CLIENT_SUPPRESSIBLE)
    {
      dep = 1;
      mode_id_req = NPA_RESOURCE_SUPPRESSIBLE_REQUEST(resource);
    }
    else /* required */
    { 
      dep = 0;
      mode_id_req = NPA_RESOURCE_REQUIRED_REQUEST(resource);
    }

    for (i = 0; dep < resource->node->dependency_count; i++, dep+=2)
      {
      pm_npa_process_rsrc(client, resource, mode_id_req, i, dep);
      }
    }

  return mode_id;
}
