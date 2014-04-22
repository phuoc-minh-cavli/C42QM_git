/**
 * @file:  SpmiOs.c
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2020/01/30 22:49:35 $
 * $Header: //components/rel/core.tx/6.0/buses/spmi/src/platform/os/tn/SpmiOs.c#1 $
 * $Change: 22400605 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 06/27/16   rk      MSM9206: API porting from DALsys to Threadx (CR - 1034553) 
 * 10/1/13  Initial Version
 */

#include "SpmiOs.h"
#include "SpmiOsNhlos.h"
#include "SpmiLogs.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "DDISpmi.h"
#include "DDIHWIO.h"
#include "DDIInterruptController.h"
#include "SpmiInfo.h"
#include "SpmiOsTarget.h"
#include "busywait.h"


//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

// Hard coded pmic arb base address (property)
#define SPMI_HW_DEV_PROP "pmic_arb_base_addr"

// This owner number for the EE this driver is running on (property)
#define OWNER_PROP "owner"

// Name expected by the HWIO driver for the pmic arb base address
#define PMIC_ARB_BASE_ID "PMIC_ARB"
#define SPMI_DEVICES_PROP "spmi_devices"

//******************************************************************************
// Global Data
//******************************************************************************

static boolean propHdlInited = FALSE;
static DALSYS_PROPERTY_HANDLE_DECLARE(dalPropDev);


//******************************************************************************
// Private API Helper Functions
//******************************************************************************

Spmi_Result SpmiOsNhlos_GetDalProp(char* propId, DALSYSPropertyVar* val)
{
    if(!propHdlInited)
    {
        if(DALSYS_GetDALPropertyHandleStr( DALSPMI_DEVICE_ID, dalPropDev ) != DAL_SUCCESS ) {
            SPMI_LOG_ERROR( "Unable to get SPMI property handle" );
            return SPMI_FAILURE_INIT_FAILED;
        }
        
        propHdlInited = TRUE;
    }
    
    return DALSYS_GetPropertyValue( dalPropDev, propId, 0, val ) == DAL_SUCCESS ? 
           SPMI_SUCCESS : SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
}


//******************************************************************************
// Public API Functions
//******************************************************************************
Spmi_Result SpmiOs_Init(SpmiInfo **ppSpmiInfo)
{
    Spmi_Result rslt = SPMI_SUCCESS;
    DALSYSPropertyVar dalProp;
    DalDeviceHandle* handle;
    uint8** addr;
    SpmiInfo *pSpmiInfo;
    SpmiTargetConfig *pTgt;
    uint32 i;

    *ppSpmiInfo = NULL;
    if((rslt = SpmiOsNhlos_GetDalProp( SPMI_DEVICES_PROP, &dalProp )) == SPMI_SUCCESS) {
       pSpmiInfo = (SpmiInfo *)dalProp.Val.pStruct;
    }
    else {
       return SPMI_FAILURE_INIT_FAILED;     
    }

    rslt = SpmiOsNhlos_GetDalProp( OWNER_PROP, &dalProp );
    if(rslt != SPMI_SUCCESS) {
       return SPMI_FAILURE_INIT_FAILED;     
    }
    pSpmiInfo->owner = dalProp.Val.dwVal;

    
    // Try to use the HWIO driver to get the base address
    if(DAL_HWIODeviceAttach( DALDEVICEID_HWIO, &handle ) == DAL_SUCCESS && handle != NULL)
    {
        if(DalDevice_Open( handle, DAL_OPEN_SHARED ) == DAL_SUCCESS) {
           for (i=0; i < pSpmiInfo->uNumDevices; i++)
           {
              addr = &pSpmiInfo->devices[i].baseAddrs ; // just one bus for now.
              pTgt = (SpmiTargetConfig *)pSpmiInfo->devices[i].target;    
              if ( (DalHWIO_MapRegion( handle, pTgt->hwioAddrStr, (uint8**) addr )  != DAL_SUCCESS) ||
                   (NULL == (*addr))) {
                 return SPMI_FAILURE_INIT_FAILED;
              }
           }
           DalDevice_Close( handle );
        } 
    }
    
    *ppSpmiInfo = pSpmiInfo;
    
    return rslt;
}

void SpmiOs_Wait(uint32 us)
{
    busywait( us );
}

