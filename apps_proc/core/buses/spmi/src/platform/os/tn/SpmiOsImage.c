/**
 * @file:  SpmiOsImage.c
 * 
 * Copyright (c) 2013-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2020/01/30 22:49:35 $
 * $Header: //components/rel/core.tx/6.0/buses/spmi/src/platform/os/tn/SpmiOsImage.c#1 $
 * $Change: 22400605 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 06/27/16   rk      MSM9206: API porting from DALsys to Threadx (CR - 1034553) 
 * 1/20/15  Multiple bus support
 * 11/3/14  Automatic channel assignment
 * 10/1/13  Initial Version
 */

#include "SpmiOs.h"
#include "SpmiOsNhlos.h"
#include "SpmiLogs.h"
#include "SpmiCfgInternal.h"
#include "PmicArb.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "DDIClock.h"
#include "SpmiOsTarget.h"
#include "DDITimetick.h"
#include "DDIInterruptController.h"
#include "SpmiOsTarget.h"
#include <stdlib.h>


//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************



//******************************************************************************
// Global Data
//******************************************************************************

static boolean clkHdAllowedToInit = FALSE;

//******************************************************************************
// Local Helper Functions
//******************************************************************************

void SpmiOs_ClkStatusInit(void)
{
    // Allow attachment to DAL Clock Driver now that it has initialized.
    clkHdAllowedToInit = TRUE;
}

static DalDeviceHandle* SpmiOs_GetClkHandle(void)
{
    static DalDeviceHandle* hClk = NULL;
    DALResult rslt;

    // Do not init if Clock Driver is not initialized yet.
    if (NULL == hClk && clkHdAllowedToInit) {
       rslt = DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &hClk);
       if ( DAL_SUCCESS != rslt ) {
          hClk = NULL;     
       }
    }

    return hClk;
}


//******************************************************************************
// Public API Functions
//******************************************************************************

boolean SpmiOs_HandleTransactionError(SpmiDevInfo *pDev, Spmi_Result* rslt, PmicArbCmd cmd, 
                                      uint8 slaveId, uint16 address, uint8 tries)
{
    return FALSE;
}

Spmi_Result SpmiOs_Malloc(uint32 size, void** buf)
{
    *buf = malloc(size);
    if(*buf) {
        memset( (void *)(*buf), 0, size );
        return SPMI_SUCCESS;
    }    
    
    return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
}

Spmi_Result SpmiOs_Free(void* buf)
{
    free( buf );
        return SPMI_SUCCESS;
}

Spmi_Result SpmiOs_RegisterISR(SpmiDevInfo *pDev, SpmiOs_IsrPtr isr)
{
    static DalDeviceHandle* interruptDev = NULL;
    SpmiTargetConfig *pSpmiTgt = (SpmiTargetConfig*)pDev->target;
    DALInterruptID interruptNum = pSpmiTgt->interruptVec;
    DALResult rslt;
    
    if(interruptDev == NULL)
    {
        if((rslt = DAL_DeviceAttach( DALDEVICEID_INTERRUPTCONTROLLER, &interruptDev )) != DAL_SUCCESS)
        {
            SPMI_LOG_ERROR( "Could not get handle to interrupt controller.  Result: %d", rslt );
            return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
        }
    }
    
    if(interruptDev != NULL)
    {
        if((rslt = DalInterruptController_RegisterISR( interruptDev,
                                                       interruptNum,
                                                       isr,
                                                       (void *)pDev,
                                                       DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER )) == DAL_SUCCESS) 
        {
            return SPMI_SUCCESS;            
        }
        else 
        {
            SPMI_LOG_ERROR( "Could not register for interrupt number %d.  Result: %d",
                            interruptNum,
                            rslt );
        }
    }
        
    return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
}

SpmiOs_ClkStatus SpmiOs_GetSerialClkState(SpmiDevInfo *pDev)
{
    SpmiTargetConfig *cfg = (SpmiTargetConfig*)pDev->target;
    DalDeviceHandle  *hClk;

    if(cfg == NULL) {
        return SPMI_CLK_STATUS_NOT_SUPPORTED;
    }
    hClk = SpmiOs_GetClkHandle();
    if (NULL != hClk) {
       if( !cfg->serClkIdInited )
       {
          if( DAL_SUCCESS == 
              DalClock_GetClockId(hClk, cfg->serClkStr, &cfg->serClkId) )
          {   
             cfg->serClkIdInited = TRUE;
          }
       }

       if( cfg->serClkIdInited )
       {
           return DalClock_IsClockOn(hClk, cfg->serClkId)? SPMI_CLK_ON : SPMI_CLK_OFF;
       }
    }
    
    return SPMI_CLK_STATUS_NOT_SUPPORTED;
}

SpmiOs_ClkStatus SpmiOs_GetAhbClkState(SpmiDevInfo *pDev)
{
    SpmiTargetConfig *cfg = (SpmiTargetConfig*)pDev->target;
    DalDeviceHandle  *hClk;
    
    if(cfg == NULL) {
        return SPMI_CLK_STATUS_NOT_SUPPORTED;
    }
    hClk = SpmiOs_GetClkHandle();
    if (NULL != hClk) {
       if(!cfg->ahbClkIdInited)
       {
          if( DAL_SUCCESS == 
             DalClock_GetClockId(hClk, cfg->ahbClkStr, &cfg->ahbClkId) )
          {
             cfg->ahbClkIdInited = TRUE;
          }
       }

       if(cfg->ahbClkIdInited)
       {
          return DalClock_IsClockOn(hClk, cfg->ahbClkId)? SPMI_CLK_ON : SPMI_CLK_OFF;     
       }
    }
    
    return SPMI_CLK_STATUS_NOT_SUPPORTED;
}

uint64 SpmiOs_GetTimeTick(void) 
{
    static boolean timeHdlInited = FALSE;
    static DalDeviceHandle* timeDev = NULL;
    uint64 tick = 0;
    
    if(!timeHdlInited) 
    {
        if(DalTimetick_Attach( "SystemTimer", &timeDev ) != DAL_SUCCESS) {
            timeDev = NULL;
        }
        else if(DalTimetick_InitTimetick64( timeDev ) != DAL_SUCCESS)
        {
            DAL_DeviceDetach( timeDev );
            timeDev = NULL;
        }
        timeHdlInited = TRUE;
    }
    
    if(timeDev != NULL) {
        DalTimetick_GetTimetick64( timeDev, &tick );
    }
    
    return tick;
}
