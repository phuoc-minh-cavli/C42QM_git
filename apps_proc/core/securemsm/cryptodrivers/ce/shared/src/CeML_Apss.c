/**
@file CeCL_Apss.c
@brief Crypto Engine Clock Enable Source file
*/

/*===========================================================================

                     Crypto Engine Clock Enable File

DESCRIPTION

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) {2018} Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

 $Header: //components/rel/core.tx/6.0/securemsm/cryptodrivers/ce/shared/src/CeML_Apss.c#1 $
 $DateTime: 2020/01/30 22:49:35 $
 $Author: pwbldsvc $

when         who     what, where, why
----------   ---     ----------------------------------------------------------
01/19/18     yash    Initial version
============================================================================*/

#include "CeEL_Env.h"
#include "CeML.h"
#include "CeCL_Clk_Target.h"
#include "DALDeviceId.h"
#include "DDIClock.h"
/* For ICB related API's */
#include "icbarb.h"
#include "icbid.h"

#define NPA_NODE_ICB_ARBITER    "/icb/arbiter"

typedef struct
{
  uint64 ib;
  uint64 ab;
}CeClClkBwVal;

typedef struct
{
  npa_client_handle icbArbClientHandle;
  ICBArb_RequestType vecReq;
}CeClkInternalDataType;

static CeClkInternalDataType CeClkData;
static uint32 ce_clk_cb_init = 0;
static uint32 ce_clk_init = 0;

static DalDeviceHandle *hclock_crypto = NULL;
static ClockIdType      nClockID_crypto;
static ClockIdType      nClockID_axi;
static ClockIdType      nClockID_ahb;

CeClClkBwVal clk_bw_val_s[CE_MAX_BW] = {
 {CE_CLK_BW_IB_HIGH, CE_CLK_BW_AB_HIGH}, /* CE_HIGH_BW */
 {CE_CLK_BW_IB_MEDIUM, CE_CLK_BW_AB_MEDIUM}, /* CE_MEDIUM_BW */
 {CE_CLK_BW_IB_LOW, CE_CLK_BW_AB_LOW}, /* CE_LOW_BW */ 
};

CEEL_MUTEX_TYPE ceel_mutex;
uint32 ceel_mutex_init = 0;

/**
 * @brief   This is a callback function, called when ICB arbiter resource is ready.
 *
 * @param None
 **///Init_icb_arbiter_CE_callback
static void CeCLNPACallbackICBarbiter(void)
{
  /* We create the initial states for the bus arbiter */
  /* There are 2 master/slave pairs which are affected by MCA */
  ICBArb_MasterSlaveType msArray[] =
  {
    { ICBID_MASTER_CRYPTO, ICBID_SLAVE_EBI1 }
  };

  CEEL_MUTEX_ENTER();
  /* Create a client for the bus arbiter */
  CeClkData.icbArbClientHandle = icbarb_create_suppressible_client_ex( "/node/apss/crypto", msArray, 1, NULL );

  /* Indicate we are using request type 3 */
  CeClkData.vecReq.arbType = ICBARB_REQUEST_TYPE_3_LAT;
  CeClkData.vecReq.arbData.type3.uLatencyNs = 0;
  CEEL_MUTEX_EXIT();
}

/**
 * @brief
 *
 *
 * @return None
 *
 * @see
 *
 */
static int CeClClockEnable(CeMLClkBwType clk_bw)
{
  DALResult           eResult;
  uint64 ib_val = 0;
  uint64 ab_val = 0;

  CEEL_MUTEX_ENTER();

  /* Below piece of scope will execute only once during the boot. */
  if(ce_clk_cb_init == 0)
  {
    ce_clk_cb_init = 1;

    /* Register callback to create sync client to /icb/arbiter */
    if(!CeClkData.icbArbClientHandle)
    {
      npa_resource_available_cb(NPA_NODE_ICB_ARBITER, (npa_callback)CeCLNPACallbackICBarbiter, NULL );
    }
  }

  if(clk_bw >= CE_MAX_BW)
  {
    CEEL_MUTEX_EXIT();
    /* Invalid clock bandwidth request */	  
    return -1;
  }

  if (ce_clk_init == 0)
  {
    // DALSYS_InitMod(NULL);

    /*Request to enable clocks for the path from CPU to Peripheral*/
    if(CeClkData.icbArbClientHandle)
    {
      /* Set up the vector of request states for the bus arbiter . */
      CeClkData.vecReq.arbData.type3.uIb = clk_bw_val_s[clk_bw].ib;
      CeClkData.vecReq.arbData.type3.uAb = clk_bw_val_s[clk_bw].ab;

      // Issue request to /icb/arbiter
      if(ICBARB_ERROR_SUCCESS != icbarb_issue_request( CeClkData.icbArbClientHandle,
                                                       &CeClkData.vecReq,
                                                       1 ))
      {
        CEEL_MUTEX_EXIT();
        return -1;
      }
    }

    /* Get the handle for clock device */
    if(hclock_crypto == NULL)
    {
      eResult = DAL_DeviceAttach(DALDEVICEID_CLOCK, &hclock_crypto);
      if((eResult != DAL_SUCCESS) || (hclock_crypto == NULL))
      {
        CEEL_MUTEX_EXIT();
        return -1;
      }


      /* Get the clock ID */
      eResult = DalClock_GetClockId(hclock_crypto, "gcc_crypto_ahb_clk", &nClockID_ahb);
      if(eResult != DAL_SUCCESS)
      {
        CEEL_MUTEX_EXIT();
        return -1;
      }

      /* Get the clock ID */
      eResult = DalClock_GetClockId(hclock_crypto, "gcc_crypto_axi_clk", &nClockID_axi);
      if(eResult != DAL_SUCCESS)
      {
        CEEL_MUTEX_EXIT();
        return -1;
      }

      /* Get the clock ID */
      eResult = DalClock_GetClockId(hclock_crypto, "gcc_crypto_clk", &nClockID_crypto);
      if(eResult != DAL_SUCCESS)
      {
        CEEL_MUTEX_EXIT();
        return -1;
      }
    }

    //Deassert CE core and bring it out of reset
    eResult = DalClock_ResetClock(hclock_crypto, nClockID_crypto, CLOCK_RESET_DEASSERT);
    if(eResult != DAL_SUCCESS)
    {
      CEEL_MUTEX_EXIT();
      return -1;
    }

    eResult = DalClock_EnableClock(hclock_crypto,nClockID_axi);
    if(eResult != DAL_SUCCESS)
    {
      CEEL_MUTEX_EXIT();
      return -1;
    }

    eResult = DalClock_EnableClock(hclock_crypto,nClockID_ahb);
    if(eResult != DAL_SUCCESS)
    {
      CEEL_MUTEX_EXIT();
      return -1;
    }

    eResult = DalClock_EnableClock(hclock_crypto,nClockID_crypto);
    if(eResult != DAL_SUCCESS)
    {
      CEEL_MUTEX_EXIT();
      return -1;
    }
  }

  ce_clk_init++;
  CEEL_MUTEX_EXIT();
  return 0;
}

/**
 * @brief
 *
 *
 * @return None
 *
 * @see
 *
 */
static int CeClClockDisable(void)
{
  DALResult           eResult;

  CEEL_MUTEX_ENTER();

  if(ce_clk_init!=0)
    ce_clk_init--;

  if (ce_clk_init == 0)
  {
    eResult = DalClock_DisableClock(hclock_crypto,nClockID_axi);
    if(eResult != DAL_SUCCESS)
    {
      CEEL_MUTEX_EXIT();
      return -1;
    }

    eResult = DalClock_DisableClock(hclock_crypto,nClockID_ahb);
    if(eResult != DAL_SUCCESS)
    {
      CEEL_MUTEX_EXIT();
      return -1;
    }

    eResult = DalClock_DisableClock(hclock_crypto,nClockID_crypto);
    if(eResult != DAL_SUCCESS)
    {
      CEEL_MUTEX_EXIT();
      return -1;
    }

    if(CeClkData.icbArbClientHandle)
    {
      /* Set up the vector of request states for the bus arbiter . */
      CeClkData.vecReq.arbData.type3.uIb = 0;
      CeClkData.vecReq.arbData.type3.uAb = 0;
      // Issue request to /icb/arbiter
      icbarb_issue_request(CeClkData.icbArbClientHandle,
                           &CeClkData.vecReq,
                           1 );
    }
  }
  CEEL_MUTEX_EXIT();
  return 0;
}

int CeMLSetBandwidth(CeMLClkBwType freq, boolean req)
{
  if(req)
  {
    if(0 != CeClClockEnable(freq)) 
      return -1;	    
  }
  else
  {
    if(0 != CeClClockDisable())
      return -1;	    
  }  
  return 0;
}





