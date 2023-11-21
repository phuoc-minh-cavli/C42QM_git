/*! \file */

/*
===========================================================================

FILE:        hal_mdp_traffic_ctrl.c

DESCRIPTION: Manages traffic control of memory transactions between MDP 
             and VBIF that interfaces to system memory.
  
===========================================================================
  Copyright (c) 2012-2014, 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/

/* -----------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------- */

#include "hal_mdp_i.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------------------
 * Local defines 
 * ------------------------------------------------------------------------------------ */



#define HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MIN            0
#define HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MAX            HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_REQPRIORITY_FIFO_WATERMARK_0_WATERMARK_BMSK
                                                                    /* width of the field is 16-bit */

#define HAL_MDP_TRAFFIC_SHAPER_BYTES_PER_CLOCK_MIN                  1
#define HAL_MDP_TRAFFIC_SHAPER_BYTES_PER_CLOCK_MAX                  HWIO_MMSS_MDP_TRAFFIC_SHAPER_RD_CLIENT_0_BYTES_PER_CLOCK_BMSK
                                                                    /* width of the field is 8-bit */

/* QoS watermark level refers to the number of 128-bit (16 bytes) entries in SMP,
   hence its step size is 16 bytes.                                              */
#define HAL_MDP_WATERMARK_STEP_SIZE                                 16

/* -----------------------------------------------------------------------
 * Static data declarations and functions
 * ----------------------------------------------------------------------- */

/****************************************************************************
*
** FUNCTION: HAL_MDP_TrafficCtrl_SetClientWatermarkLvl()
*/
/*!
* \brief
*     Sets a read client's watermark levels based on client's latency need.
*
* \param [in] eLatency       - latency setting for the client; 
* \param [in] uTotalSMPBytes - total number of bytes in SMP allocated for the client;
* \param [in] uRegAddr       - address of watermark level-0 register;
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_MDP_TrafficCtrl_SetClientWatermarkLvl (
                              HAL_MDP_TrafficCtrl_LatencyType  eLatency,
                              uint32                           uTotalSMPBytes,
                              uintPtr                          uRegAddr )
{
   HAL_MDSS_ErrorType    eStatus          = HAL_MDSS_STATUS_SUCCESS;
   uint32                uIndex           = 0;
   uint32                uRegValue        = 0;
   uint32                uWatermarkLvl[]  = {0, 0, 0};

   /* in the current HW, 3 watermark registers are used by HW to check the status
    * of buffer level in SMP and then generate 1 of 4 QoS priority levels.  
    *
    * if the number of watermark registers in HW is changed and not 3, the code
    * must be revisited in order to accommodate the new configurations in HW.
    */
   if (3 == pgsMdpHwInfo->uQosReqPriWatermarkRegsCount)
   {
      if (0 != uTotalSMPBytes)
      {       
         /* set watermark levels to 1/4, 1/2 and 3/4 of the total SMP space allocated */
         uWatermarkLvl[0] = (uint32)(uTotalSMPBytes/(HAL_MDP_WATERMARK_STEP_SIZE*4));
         uWatermarkLvl[1] = (uint32)((uTotalSMPBytes*2)/(HAL_MDP_WATERMARK_STEP_SIZE*4));
         uWatermarkLvl[2] = (uint32)((uTotalSMPBytes*3)/(HAL_MDP_WATERMARK_STEP_SIZE*4));        
      }
      else
      {
        switch (eLatency)
        {
        case HAL_MDP_TRFCTRL_LATENCY_BEST_EFFORT:
           uWatermarkLvl[0] = HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MAX;
           uWatermarkLvl[1] = HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MAX;
           uWatermarkLvl[2] = HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MAX;
           break;

        case HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME:
           uWatermarkLvl[0] = HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MIN;
           uWatermarkLvl[1] = HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MAX;
           uWatermarkLvl[2] = HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MAX;
           break;

        case HAL_MDP_TRFCTRL_LATENCY_REALTIME:
           uWatermarkLvl[0] = HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MIN;
           uWatermarkLvl[1] = HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MIN;
           uWatermarkLvl[2] = HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MAX;
           break;

        case HAL_MDP_TRFCTRL_LATENCY_ULTRA_LOW_LATENCY:
           uWatermarkLvl[0] = HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MIN;
           uWatermarkLvl[1] = HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MIN;
           uWatermarkLvl[2] = HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MIN;
           break;

        case HAL_MDP_TRFCTRL_LATENCY_MAX:
        case HAL_MDP_TRFCTRL_CLIENT_FORCE_32BIT:
        default:
           eStatus = HAL_MDSS_STATUS_FAILED_INVALID_INPUT_PARAMETER;
           break;
        }
      }

      if (HAL_MDSS_STATUS_SUCCESS == eStatus)
      {
         for (uIndex = 0; uIndex < pgsMdpHwInfo->uQosReqPriWatermarkRegsCount; uIndex++)
         {
            uRegValue = 0;
            uRegValue = HWIO_OUT_FLD(uRegValue,
                                     MDP_VP_0_VIG_0_SSPP_REQPRIORITY_FIFO_WATERMARK_0,
                                     WATERMARK,
                                     uWatermarkLvl[uIndex]);
            out_dword(uRegAddr, uRegValue);
            uRegAddr += 4;      /* move to next register address */
         }
      }
   }
   else
   {
      eStatus = HAL_MDSS_STATUS_FAILED_NOT_SUPPORTED;
   }

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_TrafficCtrl_SetTrfShaper()
*/
/*!
* \brief
*     Configures traffice shaper.
*
* \param [in] bEnTrfShaper - enable/disable the traffic shaper;
* \param [in] uBwLimit     - bandwidth limit in number of MB/sec; 
* \param [in] uMdpClkFreq  - MDP core clock working frequency in Hz;
* \param [in] uRegAddr     - address of the traffic shaper register;
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_MDP_TrafficCtrl_SetTrfShaper ( 
                              bool32  bEnTrfShaper,
                              uint32  uBwLimit,
                              uint32  uMdpClkFreq,
                              uintPtr uRegAddr )
{
   HAL_MDSS_ErrorType    eStatus      = HAL_MDSS_STATUS_SUCCESS;
   uint32                uBytesPerClk = 0;
   uint32                uRegValue    = 0;

   if (TRUE == bEnTrfShaper)
   {
      uMdpClkFreq = (uMdpClkFreq == 0) ? pgsMdpHwInfo->uMaxMdpCoreClkFreq : uMdpClkFreq;
      
      /* constant numbers used below are:
       *  * 1000000, to convert uBwLmt from MB/sec to B/sec;
       *  * 16, as the register field is in 4.4 format;
       *  the part ( * 2 + 1) >> 1 is for rounding up the calculation;
       */
      uBytesPerClk = (((uBwLimit * 1000000) / uMdpClkFreq) * 16 * 2 + 1) >> 1;
      uBytesPerClk = (uBytesPerClk > HAL_MDP_TRAFFIC_SHAPER_BYTES_PER_CLOCK_MAX) ?
                      HAL_MDP_TRAFFIC_SHAPER_BYTES_PER_CLOCK_MAX : uBytesPerClk;

      if ((0 != uBwLimit) && (0 == uBytesPerClk))
      {
          uBytesPerClk = HAL_MDP_TRAFFIC_SHAPER_BYTES_PER_CLOCK_MIN;  /* assign the min if calculated result is between 0 and 1 */
      }
   }
   else
   {
      uBytesPerClk = 0;
   }

   uRegValue = 0;
   uRegValue = HWIO_OUT_FLD(uRegValue,
                            MDP_TRAFFIC_SHAPER_RD_CLIENT_0,
                            EN,
                            bEnTrfShaper);
   uRegValue = HWIO_OUT_FLD(uRegValue,
                            MDP_TRAFFIC_SHAPER_RD_CLIENT_0,
                            BYTES_PER_CLOCK,
                            uBytesPerClk);
   out_dword(uRegAddr, uRegValue);

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_TrafficCtrl_InitClients()
*/
/*!
* \brief
*     Initializes settings of traffic control clients with consecutive IDs.
*
* \param [in] eClientId     - ID of the first client; IDs of the following  
*                             clients must be consecutive to this;
* \param [in] uNumOfClients - number of clients with consecutive IDs;
*
* \retval None
*
****************************************************************************/
static void HAL_MDP_TrafficCtrl_InitClients (
                HAL_MDP_TrafficCtrl_ClientIDType   eClientId,
                uint32                             uNumOfClients )
{
   HAL_MDP_TrfCtrlParamsType   sSetting[HAL_MDP_TRFCTRL_CLIENT_MAX];
   HAL_MDP_TrfCtrlParamsType  *psSetting                            = &sSetting[0];
   uint32                      uIndex                               = 0;

   if (0 != uNumOfClients)   /* check this condition to avoid unnecessary function calls */
   {
      for (uIndex = 0; uIndex < uNumOfClients; uIndex++)
      {
         psSetting->eClientId          = eClientId;
         psSetting->eReadClientLatency = (eClientId <= HAL_MDP_TRFCTRL_CLIENT_LAYER4_CURSOR) ?
                                          HAL_MDP_TRFCTRL_LATENCY_REALTIME : HAL_MDP_TRFCTRL_LATENCY_BEST_EFFORT;
         psSetting->uTotalSMPBytes     = 0;
         psSetting->bEnableTrfShaper   = FALSE;
         psSetting->uBwLimitInMBs      = 0;

         eClientId++;
         psSetting++;
      }

      if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_TrafficCtrl_Setup(&sSetting[0], uNumOfClients, 0, 0))
      {
         HAL_MDP_ASSERT();
      }
   }

   return;
}


/* -------------------------------------------------------------------------------------
 * Public functions
 * ------------------------------------------------------------------------------------ */


/****************************************************************************
*
** FUNCTION: HAL_MDP_TrafficCtrl_Setup()
*/
/*!
* \brief
*     Configures the traffic control of memory transactions between MDP and VBIF
*     that interfaces to system memory.
*
* \param [in] psTrfCtrlSetting - traffic control settings;
* \param [in] uNumOfSettings   - number of settings;
* \param [in] uMdpCoreClkFreq  - MDP core clock working frequency in Hz;
* \param [in] uFlags           - reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_TrafficCtrl_Setup (
                       HAL_MDP_TrfCtrlParamsType  *psTrfCtrlSetting,
                       uint32                      uNumOfSettings,
                       uint32                      uMdpCoreClkFreq,
                       uint32                      uFlags )
{
   HAL_MDSS_ErrorType          eStatus     = HAL_MDSS_STATUS_SUCCESS;
   HAL_MDP_TrfCtrlParamsType  *psSetting   = psTrfCtrlSetting; 
   uint32                      uIndex      = 0;
   uintPtr                     uRegAddr    = 0;
   uintPtr                     uRegAddrInc = 0;

   for(uIndex = 0; uIndex < uNumOfSettings; uIndex++)
   {
      switch (psSetting->eClientId)
      {
      case HAL_MDP_TRFCTRL_CLIENT_VIG0:
      case HAL_MDP_TRFCTRL_CLIENT_VIG1:
      case HAL_MDP_TRFCTRL_CLIENT_VIG2:
      case HAL_MDP_TRFCTRL_CLIENT_VIG3:
         /* sets watermark levels */
         uRegAddrInc = HWIO_MMSS_MDP_VP_0_VIG_1_SSPP_REQPRIORITY_FIFO_WATERMARK_0_ADDR - HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_REQPRIORITY_FIFO_WATERMARK_0_ADDR;
         uRegAddr    = HWIO_MMSS_MDP_VP_0_VIG_0_SSPP_REQPRIORITY_FIFO_WATERMARK_0_ADDR +
                       (psSetting->eClientId - HAL_MDP_TRFCTRL_CLIENT_VIG0) * uRegAddrInc;
         eStatus = HAL_MDP_TrafficCtrl_SetClientWatermarkLvl(psSetting->eReadClientLatency, psSetting->uTotalSMPBytes, uRegAddr);
         if (HAL_MDSS_STATUS_SUCCESS == eStatus)
         {
           if(HAL_MDP_TRFCTRL_CLIENT_VIG3 == psSetting->eClientId)
           {
              uRegAddr = HWIO_MMSS_MDP_TRAFFIC_SHAPER_RD_CLIENT_9_ADDR;
           }
           else
           {
             /* configures the traffice shaper for the client */
             uRegAddrInc = HWIO_MMSS_MDP_TRAFFIC_SHAPER_RD_CLIENT_2_ADDR - HWIO_MMSS_MDP_TRAFFIC_SHAPER_RD_CLIENT_0_ADDR;
             uRegAddr    = HWIO_MMSS_MDP_TRAFFIC_SHAPER_RD_CLIENT_0_ADDR + 
                           (psSetting->eClientId - HAL_MDP_TRFCTRL_CLIENT_VIG0) * uRegAddrInc;
           }
           HAL_MDP_TrafficCtrl_SetTrfShaper(psSetting->bEnableTrfShaper, psSetting->uBwLimitInMBs, uMdpCoreClkFreq, uRegAddr);
         }

         break;

      case HAL_MDP_TRFCTRL_CLIENT_RGB0:
      case HAL_MDP_TRFCTRL_CLIENT_RGB1:
      case HAL_MDP_TRFCTRL_CLIENT_RGB2:
      case HAL_MDP_TRFCTRL_CLIENT_RGB3:
         /* sets watermark levels */
         uRegAddrInc = HWIO_MMSS_MDP_VP_0_RGB_1_SSPP_REQPRIORITY_FIFO_WATERMARK_0_ADDR - HWIO_MMSS_MDP_VP_0_RGB_0_SSPP_REQPRIORITY_FIFO_WATERMARK_0_ADDR;
         uRegAddr    = HWIO_MMSS_MDP_VP_0_RGB_0_SSPP_REQPRIORITY_FIFO_WATERMARK_0_ADDR + 
                       (psSetting->eClientId - HAL_MDP_TRFCTRL_CLIENT_RGB0) * uRegAddrInc;
         eStatus = HAL_MDP_TrafficCtrl_SetClientWatermarkLvl(psSetting->eReadClientLatency, psSetting->uTotalSMPBytes, uRegAddr);
         if (HAL_MDSS_STATUS_SUCCESS == eStatus)
         {
           if(HAL_MDP_TRFCTRL_CLIENT_RGB3 == psSetting->eClientId)
           {
             uRegAddr = HWIO_MMSS_MDP_TRAFFIC_SHAPER_RD_CLIENT_10_ADDR;
           }
           else
           {
             /* configures the traffice shaper for the client */
             uRegAddrInc = HWIO_MMSS_MDP_TRAFFIC_SHAPER_RD_CLIENT_3_ADDR - HWIO_MMSS_MDP_TRAFFIC_SHAPER_RD_CLIENT_1_ADDR;
             uRegAddr    = HWIO_MMSS_MDP_TRAFFIC_SHAPER_RD_CLIENT_1_ADDR + 
                          (psSetting->eClientId - HAL_MDP_TRFCTRL_CLIENT_RGB0) * uRegAddrInc;
           }
           HAL_MDP_TrafficCtrl_SetTrfShaper(psSetting->bEnableTrfShaper, psSetting->uBwLimitInMBs, uMdpCoreClkFreq, uRegAddr);
         }

         break;

      case HAL_MDP_TRFCTRL_CLIENT_DMA0:
      case HAL_MDP_TRFCTRL_CLIENT_DMA1:
         /* sets watermark levels */
         uRegAddrInc = HWIO_MMSS_MDP_VP_0_DMA_1_SSPP_REQPRIORITY_FIFO_WATERMARK_0_ADDR - HWIO_MMSS_MDP_VP_0_DMA_0_SSPP_REQPRIORITY_FIFO_WATERMARK_0_ADDR;
         uRegAddr    = HWIO_MMSS_MDP_VP_0_DMA_0_SSPP_REQPRIORITY_FIFO_WATERMARK_0_ADDR +
                       (psSetting->eClientId - HAL_MDP_TRFCTRL_CLIENT_DMA0) * uRegAddrInc;
         eStatus = HAL_MDP_TrafficCtrl_SetClientWatermarkLvl(psSetting->eReadClientLatency, psSetting->uTotalSMPBytes, uRegAddr);
         if (HAL_MDSS_STATUS_SUCCESS == eStatus)
         {
            /* configures the traffice shaper for the client */
            uRegAddrInc = HWIO_MMSS_MDP_TRAFFIC_SHAPER_RD_CLIENT_7_ADDR - HWIO_MMSS_MDP_TRAFFIC_SHAPER_RD_CLIENT_6_ADDR;
            uRegAddr    = HWIO_MMSS_MDP_TRAFFIC_SHAPER_RD_CLIENT_6_ADDR + 
                          (psSetting->eClientId - HAL_MDP_TRFCTRL_CLIENT_DMA0) * uRegAddrInc;
            HAL_MDP_TrafficCtrl_SetTrfShaper(psSetting->bEnableTrfShaper, psSetting->uBwLimitInMBs, uMdpCoreClkFreq, uRegAddr);
         }

         break;

      case HAL_MDP_TRFCTRL_CLIENT_LAYER0_CURSOR:
      case HAL_MDP_TRFCTRL_CLIENT_LAYER1_CURSOR:
      case HAL_MDP_TRFCTRL_CLIENT_LAYER2_CURSOR:
      case HAL_MDP_TRFCTRL_CLIENT_LAYER3_CURSOR:
      case HAL_MDP_TRFCTRL_CLIENT_LAYER4_CURSOR:
         /* sets watermark levels */
         uRegAddrInc = HWIO_MMSS_MDP_VP_0_LAYER_1_CURSOR_REQPRIORITY_FIFO_WATERMARK_0_ADDR - HWIO_MMSS_MDP_VP_0_LAYER_0_CURSOR_REQPRIORITY_FIFO_WATERMARK_0_ADDR;
         uRegAddr    = HWIO_MMSS_MDP_VP_0_LAYER_0_CURSOR_REQPRIORITY_FIFO_WATERMARK_0_ADDR +
                       (psSetting->eClientId - HAL_MDP_TRFCTRL_CLIENT_LAYER0_CURSOR) * uRegAddrInc;
         eStatus = HAL_MDP_TrafficCtrl_SetClientWatermarkLvl(psSetting->eReadClientLatency, 0, uRegAddr);

         break;

      case HAL_MDP_TRFCTRL_CLIENT_WB0:
      case HAL_MDP_TRFCTRL_CLIENT_WB1:
      case HAL_MDP_TRFCTRL_CLIENT_WB2:
         /* configures the traffice shaper for the client */
         uRegAddrInc = HWIO_MMSS_MDP_TRAFFIC_SHAPER_WR_CLIENT_1_ADDR - HWIO_MMSS_MDP_TRAFFIC_SHAPER_WR_CLIENT_0_ADDR;
         uRegAddr    = HWIO_MMSS_MDP_TRAFFIC_SHAPER_WR_CLIENT_0_ADDR + 
                       (psSetting->eClientId - HAL_MDP_TRFCTRL_CLIENT_WB0) * uRegAddrInc;
         HAL_MDP_TrafficCtrl_SetTrfShaper(psSetting->bEnableTrfShaper, psSetting->uBwLimitInMBs, uMdpCoreClkFreq, uRegAddr);

         break;

      case HAL_MDP_TRFCTRL_CLIENT_NONE:
      case HAL_MDP_TRFCTRL_CLIENT_MAX:
      case HAL_MDP_TRFCTRL_CLIENT_FORCE_32BIT:
      default:
         eStatus = HAL_MDSS_STATUS_FAILED_INVALID_INPUT_PARAMETER;
         break;         
      }

      if (HAL_MDSS_STATUS_SUCCESS == eStatus)
      {
         psSetting++;
      }
      else
      {
         break;          /* Exit on ERROR! */
      }
   }

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_TrafficCtrl_Init()
*/
/*!
* \brief
*     Initializes QoS priority re-mapping and watermark level settings for  
*     read clients, the output of QoS priority level for write paths, and 
*     the traffic shaper. 
*
* \param [in] uFlags - reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_TrafficCtrl_Init (
                       uint32  uFlags )
{
   HAL_MDSS_ErrorType          eStatus     = HAL_MDSS_STATUS_SUCCESS;
   uint32                      uIndex      = 0;
   uintPtr                     uRegAddr    = 0;
   uintPtr                     uRegAddrInc = 0;
   uint32                      uRegValue   = 0;

   /* init QoS priority re-mapping for all read clients */
   /* For each read client, 3 watermark settings are used by HW to check the status of buffer level 
      in SMP and then generate QoS priority signals. By using the above API, HAL_MDP_TrafficCtrl_Setup,
      to program watermark registers, 1 out of 4 QoS priority levels can be dynamically assigned for 
      each read client. Because of this, the single buffered registers of QoS re-mapper are set once 
      here during initialization with a fixed mapping. */
   out_dword(HWIO_MMSS_MDP_QOS_REMAPPER_CLASS_0_ADDR, pgsMdpHwInfo->uQosRemapperRealTimeClients);
   out_dword(HWIO_MMSS_MDP_QOS_REMAPPER_CLASS_1_ADDR, pgsMdpHwInfo->uQosRemapperNonRealTimeClients);
   
   /* init QoS output level for all write paths */
   /* For each write back path, QoS priority is purely controlled by a field of a single buffered
      register, dynamic altering is not allowed. Hence, they are all set here once at the 
      initialization stage. */
   uRegAddr    = HWIO_MMSS_MDP_WB_0_DST_WRITE_CONFIG_ADDR;
   uRegAddrInc = HWIO_MMSS_MDP_WB_1_DST_WRITE_CONFIG_ADDR - HWIO_MMSS_MDP_WB_0_DST_WRITE_CONFIG_ADDR;
   for (uIndex = 0; uIndex < pgsMdpHwInfo->sHwInfo.uNumOfDataPaths; uIndex++)
   {
      uRegValue = in_dword(uRegAddr);
      uRegValue = HWIO_OUT_FLD(uRegValue, MDP_WB_0_DST_WRITE_CONFIG, PRIORITY_LEVEL, *(pgsMdpHwInfo->pQosWBPathsPriority + uIndex));
      uRegValue = HWIO_OUT_FLD(uRegValue, MDP_WB_0_DST_WRITE_CONFIG, REQ_PRIORITY,   *(pgsMdpHwInfo->pQosWBPathsPriority + uIndex));
      out_dword(uRegAddr, uRegValue);

      uRegAddr += uRegAddrInc;
   }

   /* init all watermark registers and traffic shapers */
   HAL_MDP_TrafficCtrl_InitClients(HAL_MDP_TRFCTRL_CLIENT_VIG0, pgsMdpHwInfo->sHwInfo.uNumOfVIGLayers);
   HAL_MDP_TrafficCtrl_InitClients(HAL_MDP_TRFCTRL_CLIENT_RGB0, pgsMdpHwInfo->sHwInfo.uNumOfRGBLayers);
   HAL_MDP_TrafficCtrl_InitClients(HAL_MDP_TRFCTRL_CLIENT_DMA0, pgsMdpHwInfo->sHwInfo.uNumOfDMALayers);
   HAL_MDP_TrafficCtrl_InitClients(HAL_MDP_TRFCTRL_CLIENT_LAYER0_CURSOR, pgsMdpHwInfo->sHwInfo.uNumOfCursorLayers);
   HAL_MDP_TrafficCtrl_InitClients(HAL_MDP_TRFCTRL_CLIENT_WB0, pgsMdpHwInfo->sHwInfo.uNumOfBltEngines);

   return eStatus;
}


#ifdef __cplusplus
}
#endif
