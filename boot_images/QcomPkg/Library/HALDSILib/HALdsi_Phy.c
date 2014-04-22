/*=============================================================================

  File: HALdsi_Phy.c
  

  Copyright (c) 2010-2016, 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif 

#include "HALdsi.h"
#include "HALdsi_Phy.h"
#include "dsiHostSystem.h"

/* -----------------------------------------------------------------------
** Local Data Types
** ----------------------------------------------------------------------- */
HAL_DSI_PhyFunctionTable    gDsiPhyFxnTbl;
/* -----------------------------------------------------------------------
** Local functions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Public functions
** ----------------------------------------------------------------------- */
/****************************************************************************
*
** FUNCTION: HAL_DSI_ReadPHYVersionInfo()
*/
/*!
* \brief
*     Returns the DSI PHY version(Major,Minor,Step) information
*
* \param [in/out] psDSICoreVersionInfo            - Version information
*
* \retval None
*
****************************************************************************/
void HAL_DSI_ReadPHYVersionInfo(HAL_DSI_VersionType *psDSICoreVersionInfo)
{
  uint32      uVersionInfo = 0;

  if (NULL != psDSICoreVersionInfo)
  {
    uVersionInfo = in_dword(HWIO_MMSS_DSI_0_PHY_DSIPHY_REVISION_ID3_ADDR);

    psDSICoreVersionInfo->uMajorVersion = HWIO_GETVAL(DSI_0_PHY_DSIPHY_REVISION_ID3, uVersionInfo,   MAJOR);
    psDSICoreVersionInfo->uMinorVersion = HWIO_GETVAL(DSI_0_PHY_DSIPHY_REVISION_ID3, uVersionInfo,   MINOR_11_8) << 8;

    uVersionInfo = in_dword(HWIO_MMSS_DSI_0_PHY_DSIPHY_REVISION_ID2_ADDR);
    psDSICoreVersionInfo->uMinorVersion |= HWIO_GETVAL(DSI_0_PHY_DSIPHY_REVISION_ID2, uVersionInfo,  MINOR_7_0);

    uVersionInfo = in_dword(HWIO_MMSS_DSI_0_PHY_DSIPHY_REVISION_ID1_ADDR);
    psDSICoreVersionInfo->uReleaseVersion = HWIO_GETVAL(DSI_0_PHY_DSIPHY_REVISION_ID1, uVersionInfo, STEP_15_8)  << 8;

    uVersionInfo = in_dword(HWIO_MMSS_DSI_0_PHY_DSIPHY_REVISION_ID0_ADDR);
    psDSICoreVersionInfo->uReleaseVersion |= HWIO_GETVAL(DSI_0_PHY_DSIPHY_REVISION_ID0, uVersionInfo, STEP_7_0);
  }
}

/****************************************************************************
*
** FUNCTION: HAL_DSI_PhyDisable()
*/
/*!
* \brief
*     Disables DSI Phy.
*
* \param [in]   eDeviceId   - DSI core ID
*
* \retval None
*
****************************************************************************/
void HAL_DSI_PhyDisable( DSI_Device_IDType   eDeviceId )
{
  if (NULL != gDsiPhyFxnTbl.HAL_DSI_PhyDisable)
  {
    gDsiPhyFxnTbl.HAL_DSI_PhyDisable(eDeviceId);
  }

  return;
}

/****************************************************************************
*
** FUNCTION: HAL_DSI_PhyPllPowerCtrl()
*/
/*!
* \brief
*     Power up/down PLL, LDO and powergen.
*
*
* \param [in]  eDeviceId    - DSI core ID
* \param [in]  bPllPowerUp  - TRUE: power up, FALSE: power down;
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_PhyPllPowerCtrl( DSI_Device_IDType   eDeviceId,
                                            bool32              bPllPowerUp )
{
  HAL_MDSS_ErrorType   eStatus = HAL_MDSS_STATUS_SUCCESS;

  if (NULL != gDsiPhyFxnTbl.HAL_DSI_PhyPllPowerCtrl)
  {
    eStatus = gDsiPhyFxnTbl.HAL_DSI_PhyPllPowerCtrl(eDeviceId, bPllPowerUp);
  }

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_DSI_PhySetup()
*/
/*!
* \brief
*     Set up DSI Phy alone.
*
* \param [in]  eDeviceId       - DSI core ID
* \param [IN]  psDsiPhyConfig     - Phy configuration
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_PhySetup(DSI_Device_IDType          eDeviceId, 
                                    HAL_DSI_PhyConfigType     *psDsiPhyConfig)
{
  HAL_MDSS_ErrorType        eStatus = HAL_MDSS_STATUS_SUCCESS;

  if (NULL != gDsiPhyFxnTbl.HAL_DSI_PhySetup)
  {
    eStatus = gDsiPhyFxnTbl.HAL_DSI_PhySetup(eDeviceId, psDsiPhyConfig);
  }

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_DSI_PhyPllSetup()
*/
/*!
* \brief
*     Set up DSI PLL, pass back some config parameters, such as VCO output frequency, 
*     PCLK divider ratio for CC in the form of numerator and denominator, etc.
*
* \param [in]   psDsiPhyConfig     - Phy config info
* \param [out]  psDsiPhyConfigInfo - Phy & PLL config pass back info
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_PhyPllSetup(HAL_DSI_PhyConfigType       *psDsiPhyConfig,
                                       HAL_DSI_PhyConfigInfoType   *psDsiPhyConfigInfo)
{
  HAL_MDSS_ErrorType        eStatus = HAL_MDSS_STATUS_SUCCESS;

  if (NULL != gDsiPhyFxnTbl.HAL_DSI_PhyPllSetup)
  {
    eStatus = gDsiPhyFxnTbl.HAL_DSI_PhyPllSetup(psDsiPhyConfig, psDsiPhyConfigInfo);
  }

  return eStatus;
}



/****************************************************************************
*
** FUNCTION: HAL_DSI_PhySetupTimingParams()
*/
/*!
* \brief
*     Calculate PHY timing parameters.
*
* \param [in]  pTimingParameters - DSI core ID
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_PhySetupTimingParams(HAL_DSI_TimingSettingType        *pTimingParameters)
{
  HAL_MDSS_ErrorType eStatus = HAL_MDSS_STATUS_SUCCESS;

  if (NULL != gDsiPhyFxnTbl.HAL_DSI_PhySetupTimingParams)
  {
    eStatus = gDsiPhyFxnTbl.HAL_DSI_PhySetupTimingParams(pTimingParameters);
  }

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_DSI_PhyPllInitialize()
*/
/*!
* \brief
*    This function initializes the entire DSI core (Phy/PLL) to a default state.
*    This function should be applied only once after the core has come out of reset.
*
* \retval void
*
****************************************************************************/
void HAL_DSI_PhyPllInitialize(void)
{
  if (NULL != gDsiPhyFxnTbl.HAL_DSI_PhyPllInitialize)
  {
    gDsiPhyFxnTbl.HAL_DSI_PhyPllInitialize();
  }
}

/****************************************************************************
*
** FUNCTION: HAL_DSI_PhyFxnsInit()
*/
/*!
* \brief
*     Initialize DSI function table.
*
* \retval None
*
****************************************************************************/
void HAL_DSI_PhyFxnsInit(void)
{
  /* Clear DSI PHY Function table */
  gDsiPhyFxnTbl.HAL_DSI_PhyPllInitialize     = NULL;
  gDsiPhyFxnTbl.HAL_DSI_PhyDisable           = NULL;
  gDsiPhyFxnTbl.HAL_DSI_PhyPllPowerCtrl      = NULL;
  gDsiPhyFxnTbl.HAL_DSI_PhyPllSetup          = NULL;
  gDsiPhyFxnTbl.HAL_DSI_PhyPllReConfigure    = NULL;
  gDsiPhyFxnTbl.HAL_DSI_PhySetup             = NULL;
  gDsiPhyFxnTbl.HAL_DSI_PhySetupTimingParams = NULL;



  HAL_DSI_VersionType          sDSICoreVersionInfo;
  HAL_DSI_VersionType          sDSIPHYVersionInfo;
  
  HAL_DSI_ReadCoreVersionInfo(&sDSICoreVersionInfo);
  HAL_DSI_ReadPHYVersionInfo( &sDSIPHYVersionInfo );

#if 0
/* Cheel, Bagheera, Kichi, Shere, Sahi */
if ((0x01 == sDSICoreVersionInfo.uMajorVersion) &&
    (0x03 == sDSICoreVersionInfo.uMinorVersion) &&
    (0x01 == sDSICoreVersionInfo.uReleaseVersion))
{
#endif
  gDsiPhyFxnTbl.HAL_DSI_PhyPllInitialize     = NULL;
  gDsiPhyFxnTbl.HAL_DSI_PhyDisable           = HAL_DSI_Phy_1_5_Disable;
  gDsiPhyFxnTbl.HAL_DSI_PhyPllPowerCtrl      = HAL_DSI_Phy_1_5_Pll_1_6_2_PowerCtrl;
  gDsiPhyFxnTbl.HAL_DSI_PhyPllSetup          = HAL_DSI_Phy_1_5_PllSetup;
  gDsiPhyFxnTbl.HAL_DSI_PhySetup             = HAL_DSI_Phy_1_5_Setup;
  gDsiPhyFxnTbl.HAL_DSI_PhySetupTimingParams = HAL_DSI_Phy_1_5_SetupTimingParams;
#if 0
}
/* Eldarion, Estel, Elessar */
else if ( (0x01 == sDSICoreVersionInfo.uMajorVersion)   &&
          (0x03 == sDSICoreVersionInfo.uMinorVersion)   &&
          (0x00 == sDSICoreVersionInfo.uReleaseVersion) &&
          (0x01 == sDSIPHYVersionInfo.uMajorVersion)    &&
          (0x00 == sDSIPHYVersionInfo.uMinorVersion)    &&
          (0x00 == sDSIPHYVersionInfo.uReleaseVersion))
{
  HAL_HW_VersionType  sVersionType;
  HAL_MDP_HwInfo      sMdpHwInfo;
  HAL_MDP_ReadVersionInfo(&sVersionType, &sMdpHwInfo);

  if ((0x01 == sVersionType.uMajorVersion) &&
      (0x0a == sVersionType.uMinorVersion))
  {
    /*
      8992 has DSI6G controller version 1.3.0, which uses DSI 20nm Phy version 1.0.0, but has its own sequence
    */
    gDsiPhyFxnTbl.HAL_DSI_PhyPllInitialize     = HAL_DSI_Phy_20nm_1_0_PhyPllInitialize;
    gDsiPhyFxnTbl.HAL_DSI_PhyDisable           = HAL_DSI_Phy_Mdp565_Disable;
    gDsiPhyFxnTbl.HAL_DSI_PhyPllPowerCtrl      = HAL_DSI_Phy_20nm_1_0_PowerCtrl;
    gDsiPhyFxnTbl.HAL_DSI_PhyPllSetup          = HAL_DSI_Phy_Mdp565_PllSetup;
    gDsiPhyFxnTbl.HAL_DSI_PhySetup             = HAL_DSI_Phy_Mdp565_Setup;
    gDsiPhyFxnTbl.HAL_DSI_PhySetupTimingParams = HAL_DSI_Phy_Mdp565_SetupTimingParams;
  }
  else
  {
    /*
      8994 has DSI6G controller version 1.3.0, which uses DSI 20nm Phy version 1.0.0
    */
    gDsiPhyFxnTbl.HAL_DSI_PhyPllInitialize     = HAL_DSI_Phy_20nm_1_0_PhyPllInitialize;
    gDsiPhyFxnTbl.HAL_DSI_PhyDisable           = HAL_DSI_Phy_20nm_1_0_Disable;
    gDsiPhyFxnTbl.HAL_DSI_PhyPllPowerCtrl      = HAL_DSI_Phy_20nm_1_0_PowerCtrl;
    gDsiPhyFxnTbl.HAL_DSI_PhyPllSetup          = HAL_DSI_Phy_20nm_1_0_PllSetup;
    gDsiPhyFxnTbl.HAL_DSI_PhySetup             = HAL_DSI_Phy_20nm_1_0_Setup;
    gDsiPhyFxnTbl.HAL_DSI_PhySetupTimingParams = HAL_DSI_Phy_20nm_1_0_SetupTimingParams;
  }
}
else
{
  /* Other Platform */
}
#endif

  

}
#ifdef __cplusplus
}
#endif
