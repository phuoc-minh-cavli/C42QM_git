/*
  Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved. 
  
  $DateTime: 2020/01/30 22:49:35 $
  $Header: //components/rel/core.tx/6.0/buses/spmi/src/platform/os/tn/SpmiFetcherCfg.c#1 $
  $Change: 22400605 $ 
  
                               Edit History
  Date      Description
  --------  ------------------------------------------------------------------- 
  06/27/16   rk      MSM9206: API porting from DALsys to Threadx (CR - 1034553) 
  11/10/15  Initial Version
*/

#include "SpmiDal.h"
#include "SpmiBusInternal.h"
#include "SpmiOs.h"
#include "SpmiOsTarget.h"
#include "SpmiLogs.h"
#include "PmicArb.h"
#include "SpmiInfo.h"


/*------------------------------------------------------------------------------
Following functions are extended in DalSpmi Interface.
------------------------------------------------------------------------------*/

DALResult Spmi_ConfigureFetcherChannel(
    DalDeviceHandle *h,
    uint16 periph,
    DALBOOL enableInt,
    uint32 *pChannel)
{
    SpmiClientCtxt *clientCtx;
    SpmiDevCtxt *pSpmiDevCtxt;
    uint8 busId;
    uint8 slaveId;
    SpmiDevInfo *pDev;
    uint16 channel;
    DALResult result = DAL_ERROR;
    
    SPMI_LOG_TRACE("DalSpmi: Spmi_ConfigureFetcherChannel func Entry, handle: 0x%x", h);

    if(NULL == h)
    {
        SPMI_LOG_ERROR(            "DalSpmi: Spmi_ConfigureFetcherChannel: DAL handle passed is NULL");
        goto ERROR;
    }

    clientCtx = (SpmiClientCtxt *)(((DalSpmiHandle *)h)->clientCtx);
    pSpmiDevCtxt = clientCtx->pSpmiDevCtxt;

    if(!clientCtx->uOpened)
    {
        SPMI_LOG_ERROR(            "DalSpmi: Spmi_ConfigureFetcherChannel: Client with device handle 0x%x has not called device open call",
            h);
        goto ERROR;
    }

    spmiDecodeVSid( PPID_TO_VSLAVE_ID(periph), &busId, &slaveId );
    periph = PPID_TO_PERIPH_ID(periph);
    
    if (NULL == (pDev = _SpmiBus_GetDevInfo(busId)))
    {
        goto ERROR;
    }
    
    /* Enter the Critical Section now */
    qurt_mutex_lock(pSpmiDevCtxt->hSpmiBusDeviceSynchronization);

    SPMI_LOG_TRACE(        "DalSpmi: Spmi_ConfigureFetcherChannel: Entered critical section for client with device handle 0x%x",
        h);

    if (SPMI_SUCCESS == PmicArb_FindChannel( pDev, slaveId, periph, &channel ))
    {
        uint32 uOwner;

        if( NULL != *pChannel)
        {
            *pChannel = (uint32)channel;
        }
        uOwner = SPMI_HWIO_IN_FIELD(HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR(pDev->baseAddrs, channel),
                                    HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_PERIPH2OWNER);        

        if(enableInt)
        {
            SPMI_HWIO_OUT_FIELD(HWIO_PMIC_ARBq_CHNLn_CONFIG_ADDR(pDev->baseAddrs, uOwner, channel), HWIO_PMIC_ARBq_CHNLn_CONFIG_IRQ_EN, 1);
        }
        else
        {
            SPMI_HWIO_OUT_FIELD(HWIO_PMIC_ARBq_CHNLn_CONFIG_ADDR(pDev->baseAddrs, uOwner, channel), HWIO_PMIC_ARBq_CHNLn_CONFIG_IRQ_EN, 0);
        }
    }
    
    qurt_mutex_unlock(pSpmiDevCtxt->hSpmiBusDeviceSynchronization);
    SPMI_LOG_TRACE(        "DalSpmi: Spmi_ConfigureFetcherChannel: Exited critical section for client with device handle 0x%x",
        h);

ERROR:
    SPMI_LOG_TRACE("DalSpmi: Spmi_ConfigureFetcherChannel func Exit, Status: %d", result);
    return result;
}
