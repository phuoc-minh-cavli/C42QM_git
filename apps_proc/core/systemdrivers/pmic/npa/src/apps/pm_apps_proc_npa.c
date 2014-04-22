/*! \file
 *  
 *  \brief  pm_modem_proc_npa.c ----This file contains initialization functions for NPA
 *  \details This file contains initialization functions for NPA
 *          and node definitions.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation NPA Client Version: MSM8x26_PM8026_NPA_2013_02_12 - Approved
 *    PMIC code generation NPA Device Setting Value Version: MSM8x26_PM8026_NPA_Current - Not Approved
 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2013 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/npa/src/apps/pm_apps_proc_npa.c#1 $ 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "comdef.h"
#include <err.h>
#include "npa.h"
#include "npa_resource.h"

#include "pm_qc_pmic.h"
#include "pmapp_npa.h"
#include "pm_npa.h"
#include "pm_apps_proc_npa_device.h"
#include "pm_apps_proc_npa.h"
#include "pm_npa_device.h"
#include "pm_target_information.h"
#include "pm_malloc.h"
#include "DDIPlatformInfo.h"

/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/

/*===========================================================================

                LOCAL FUNCTION PROTOTYPES

===========================================================================*/
uint32                             *num_of_nodes = 0;
npa_node_definition                *pmic_npa_apps_nodes = NULL;
npa_resource_definition            *pmic_npa_apps_resources = NULL; 
extern boolean                      bPMICStub;

void pm_apps_proc_npa_device_init (void);

/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/

/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/
/* Surf, FFA, FLUID platform info*/
DalPlatformInfoPlatformInfoType pm_npa_platform_info;


/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION pm_apps_proc_npa_platform_compensate

DESCRIPTION
    This function performes the changes needed for surf ffa on PAM data.

INPUT PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  Platform detection should be avaliable.

SIDE EFFECTS
  NONE.

===========================================================================*/
static void
pm_apps_proc_npa_platform_compensate (void)
{
   DalDeviceHandle      *phPFormInfo = NULL;
   DALResult             eResult;

   eResult = DAL_DeviceAttach(DALDEVICEID_PLATFORMINFO, &phPFormInfo);

   if (DAL_SUCCESS == eResult)
{
      DalDevice_Open (phPFormInfo, DAL_OPEN_SHARED);
      DalPlatformInfo_GetPlatformInfo (phPFormInfo, &pm_npa_platform_info );
      DalDevice_Close (phPFormInfo);
   }
}


/*===========================================================================

FUNCTION pm_apps_proc_pam_init                                EXTERNAL
FUNCTION

DESCRIPTION
This function initializes the PAM for PMIC.

It does the following:
* Initializes the NPA PMIC Nodes and resources.

INPUT PARAMETERS
None.

RETURN VALUE
None.

DEPENDENCIES
NPA Framework should be available in the build being compiled for

SIDE EFFECTS
NONE.

===========================================================================*/
static void
pm_apps_proc_pam_init (void)
{
   pm_apps_proc_npa_platform_compensate ();
} /* end of pm_apps_proc_pam_init() */

/*===========================================================================

FUNCTION pm_apps_proc_npa_init                                EXTERNAL FUNCTION

DESCRIPTION
This function initializes the NPA for PMIC.

It does the following:
* It initializes the PMIC NPA Node and resources.

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
pm_apps_proc_npa_init (void)
{
    uint32 i;
    pm_npa_node_resource_info          *nodeRscArray = NULL;
    pm_npa_node_resource_info          *nodeRsc = NULL;
    
   /* Init PAM */
   pm_apps_proc_pam_init ();
   
   /* Init RPM device trans-apply */
   pm_apps_proc_npa_device_init ();
   
	// Get the PAM info
	nodeRscArray = (pm_npa_node_resource_info*)pm_target_information_get_specific_info(PM_PROP_PAM_NODE_RSRCS);
	num_of_nodes = (uint32*)pm_target_information_get_specific_info(PM_PROP_PAM_NODE_NUM);

	if(num_of_nodes && nodeRscArray)
	{
        pm_malloc(sizeof(npa_node_definition)*(*num_of_nodes), (void**)&pmic_npa_apps_nodes);
        pm_malloc(sizeof(npa_resource_definition)*(*num_of_nodes), (void**)&pmic_npa_apps_resources);

        // construct the PMIC RPM resources
        for(i = 0; i< (*num_of_nodes); i++)
        {
            nodeRsc = &nodeRscArray[i];

            // construct the PMIC RPM resources
            pmic_npa_apps_resources[i].name = nodeRsc->group_name;
            pmic_npa_apps_resources[i].units = "ModeID";
            pmic_npa_apps_resources[i].max = nodeRsc->max_mode;
            pmic_npa_apps_resources[i].attributes = nodeRsc->resource_attributes;
            if ((pmic_npa_apps_resources[i].attributes & NPA_RESOURCE_SINGLE_CLIENT) == 
            NPA_RESOURCE_SINGLE_CLIENT)
            {
                pmic_npa_apps_resources[i].plugin = &npa_identity_plugin;
            }
            else
            {
                pmic_npa_apps_resources[i].plugin = &npa_max_plugin;
            }
            pmic_npa_apps_resources[i].data = nodeRsc->resource_data;

            // construct the PMIC RPM Nodes
            pmic_npa_apps_nodes[i].name = nodeRsc->node_name;

            pmic_npa_apps_nodes[i].driver_fcn = pm_npa_resource_set;

            pmic_npa_apps_nodes[i].attributes = nodeRsc->node_attributes;
            pmic_npa_apps_nodes[i].data = nodeRsc->node_data;
            pmic_npa_apps_nodes[i].dependencies = nodeRsc->node_dependencies;
            pmic_npa_apps_nodes[i].dependency_count = nodeRsc->dependency_count;
            pmic_npa_apps_nodes[i].resource_count = 1;
            pmic_npa_apps_nodes[i].resources = NULL;
        }

        for (i = 0; i < (*num_of_nodes); i++)
        {
            pmic_npa_apps_nodes[i].resources = &pmic_npa_apps_resources[i];
            if (bPMICStub == TRUE)
            {
                npa_stub_resource(pmic_npa_apps_resources[i].name);
            }
            else
            {
                npa_define_node (pmic_npa_apps_nodes + i, NULL, NULL);
            }
        }
	}
	else
	{
		ERR_FATAL( "PMIC-apps PAM initialization failed! ", 0, 0, 0 ); // we should never get here
	}
   
} /* end of pm_apps_proc_npa_init() */
