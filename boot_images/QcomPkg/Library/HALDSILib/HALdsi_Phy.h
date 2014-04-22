#ifndef HALDSI_PHY_H
#define HALDSI_PHY_H
/*=============================================================================

  File: HALdsi_Phy.h
  

  Copyright (c) 2010-2016, 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#include "HALdsi.h"
#include "MDPLib_i.h"

/*------------------------------------------------------------------------
   Data Type Defines
-------------------------------------------------------------------------- */
#define  DSIV2_MAJOR_VERSION  0x02
#define  DSIV2_MINOR_VERSION  0x00

/* ------------------------------------------------------------------------
Enumerated types
---------------------------------------------------------------------------*/

typedef enum _HAL_DSI_BitDepthType
{
  HAL_DSI_BitDepth_RGB565 = 0x0,
  HAL_DSI_BitDepth_RGB666P = 0x1,
  HAL_DSI_BitDepth_RGB666 = 0x2,
  HAL_DSI_BitDepth_RGB888 = 0x3,
} HAL_DSI_BitDepthType;

typedef enum _HAL_DSI_PLLConfigSourceType
{
  HAL_DSI_PLL_CONFIG_INDEPENDENT_SOURCE,          /**< PLL0 and PLL1 run independently         */
  HAL_DSI_PLL_CONFIG_SPLIT_SOURCE,                /**< PLL0 is used to drive DSI0 and DSI1     */
  HAL_DSI_PLL_CONFIG_EXTERNAL_SOURCE,             /**< PLLs are used to drive external device  */
} HAL_DSI_PLLConfigSourceType;

/*------------------------------------------------------------------------
Data structure definitions
-------------------------------------------------------------------------- */


/*!
 * \struct _HAL_DSI_PhyPllConfigType
 *
 * Config different features of Phy and PLL.
 *
 */
typedef struct _HAL_DSI_PhyConfigType
{
   DSI_Device_IDType            eDeviceId;              /* DSI Core ID */
   uint32                       uDesiredBitClkFreq;     /* desired bit clock frequency in Hz */
   uint32                       uBitsPerPixel;          /* number of bits per pixel */
   uint32                       uNumOfDataLanes;        /* number of DSI data lanes */
   uint32                       uConfigFlags;           /* PLL config options, use flags defined in the above enum _HAL_DSI_PhyConfigFlagType.     */
   bool32                       bDCDCMode;              /* Regulator mode, TRUE=DCDC ,FALSE=LDO */
   uint32                       uClockStrengthHS;       /* Clock lane strength in HS */
   uint32                       uDataStrengthHS;        /* Data lane strength in HS */
   uint32                       uDataStrengthLP;        /* Data lane strength in LP */
   HAL_DSI_PLLConfigSourceType  ePLLConfigSource;       /* PLL Source */
} HAL_DSI_PhyConfigType;


/*!
 * \struct HAL_DSI_PhyConfigInfoType 
 * 
 * Defines the clock divider ratio for DSI clocks. DSI clock tree follows this path
 *
 * Direct path mode (when the required bit clock rate > 680 MHz)      TODO: clarify the different edge frequency mentioned in 2 HW documents 
 *
 *                 +-------------------------------------------------------------+
 *                 |                                                             |-->[  Esc Clk PreDivider & MND ]---EscClkFreq--->[  DSI Controller & Phy ]
 *                 |                                                        +----+
 *                 |                                                        |
 * [TCXO(19.2Mhz)]-+->[DSI PLL (VCO)]-+->[  Constant div 4 ]---ByteClkFreq--+------->[  Pass-through without MND ]---ByteClkFreq-->[  DSI Controller & Phy ]
 *                                    |
 *                                    +->[ Digital Postdiv ]---DsiClkFreq----------->[ DSI PCLK PreDivider & MND ]---PClkFreq----->[ DSI Controller & MDP5 ]
 *                                                                              
 *                                                                              
 *
 * |---Reference---|--------------------------DSI PLL-----------------------|-------------------MMSS CC Dividers-----------------|
 *
 *
 *
 * Non-direct path mode  (when the required bit clock rate < 680 MHz) 
 *
 *                 +-------------------------------------------------------------+
 *                 |                                                             |-->[  Esc Clk PreDivider & MND ]---EscClkFreq--->[  DSI Controller & Phy ]
 *                 |                                                        +----+
 *                 |                                                        |
 * [TCXO(19.2Mhz)]-+->[DSI PLL (VCO)]-+->[ 8*Analog Postdiv]---ByteClkFreq--+------->[  Pass-through without MND ]---ByteClkFreq-->[  DSI Controller & Phy ]
 *                                    |
 *                                    +->[ Digital Postdiv ]---DsiClkFreq----------->[ DSI PCLK PreDivider & MND ]---PClkFreq----->[ DSI Controller & MDP5 ]
 *                                                                              
 *                                                                              
 *
 * |---Reference---|---------------------------DSI PLL-----------------------|-------------------MMSS CC Dividers-----------------|
 *
 *
 * Based on the diagram above:
 *
 * direct path:
 *                  ByteClk = VcoCLk/4 (Analog Postdiv bypassed)
 *                  PClk    = VcoClk/Digital Postdiv / MND Divider = DsiClk/MND Divider
 * non-direct path: 
 *                  ByteClk = VcoCLk/(8*Analog Postdiv)
 *                  PClk    = VcoClk/Digital Postdiv / MND Divider = DsiClk/MND Divider
 *
 */
typedef struct _HAL_DSI_PhyConfigInfoType
{
    uint32  uPllVcoOutputFreq;     /* PLL VCO output frequency in Hz */
    uint32  uBitClkFreq;           /* bit clock frequency in Hz */
    uint32  uByteClkFreq;          /* byte clock frequency in Hz */    
    uint32  uDsiClkFreq;           /* DSI clock frequency in Hz; this is the source of PClk (see the diagram above); */
    uint32  uPclkFreq;             /* PClk frequency in Hz */
    uint32  uPClkDivNumerator;     /* numerator of PClk divider ratio in CC */
    uint32  uPClkDivDenominator;   /* denominator of PClk divider ratio in CC */
                                   /*TODO: add output info on Esc clock once getting clarification from HW team */
} HAL_DSI_PhyConfigInfoType;


/*!
 * \struct _HAL_DSI_PllSettingType
 *
 * DSI Timing parameters
 */
typedef struct _HAL_DSI_TimingSettingType
{
  DSI_Device_IDType         eDeviceId;             /* DSI Core ID */
  uint32                    uBitclock;
  uint32                    uEscapeFreq;
  DSI_TimingOverrideType   *pPreDefinedTimings;
} HAL_DSI_TimingSettingType;


/*!
* \struct HAL_DSI_PhyFunctionTable
*
* DSI PHY/PLL function table
*/
typedef struct
{
  /* Initialize DSI PHY/PLL */
  void (*HAL_DSI_PhyPllInitialize) (void);

  /* Disable DSI PHY */
  void (*HAL_DSI_PhyDisable) (DSI_Device_IDType eDeviceId);

  /* Power up/down PLL, LDO and powergen */
  HAL_MDSS_ErrorType (*HAL_DSI_PhyPllPowerCtrl) (DSI_Device_IDType eDeviceId, bool32 bPllPowerUp);

  /* Setup DSI PHY */
  HAL_MDSS_ErrorType (*HAL_DSI_PhySetup) (DSI_Device_IDType eDeviceId, HAL_DSI_PhyConfigType *psDsiPhyConfig);

  /* Setup DSI PLL */
  HAL_MDSS_ErrorType (*HAL_DSI_PhyPllSetup) (HAL_DSI_PhyConfigType *psDsiPhyConfig, HAL_DSI_PhyConfigInfoType *psDsiPhyConfigInfo);

  /* Calculate PHY timing parameters */
  HAL_MDSS_ErrorType (*HAL_DSI_PhySetupTimingParams) (HAL_DSI_TimingSettingType *pTimingParameters);

  /* Reconfigures PHY PLL to a different refresh rate */
  HAL_MDSS_ErrorType (*HAL_DSI_PhyPllReConfigure) (HAL_DSI_PhyConfigType *psDsiPhyConfig, HAL_DSI_PhyConfigInfoType *psDsiPhyPLLConfigInfo);

} HAL_DSI_PhyFunctionTable;

/*------------------------------------------------------------------------
Function declarations
-------------------------------------------------------------------------- */
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
void HAL_DSI_PhyFxnsInit(void);

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
void HAL_DSI_PhyDisable( DSI_Device_IDType   eDeviceId );



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
                                            bool32              bPllPowerUp );


/****************************************************************************
*
** FUNCTION: HAL_DSI_PhySetup()
*/
/*!
* \brief
*     Set up DSI Phy alone.
*
* \param [in]  eDeviceId       - DSI core ID
* \param [IN]   psDsiPhyConfig     - Phy configuration
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_PhySetup(DSI_Device_IDType          eDeviceId, 
                                    HAL_DSI_PhyConfigType     *psDsiPhyConfig);


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
HAL_MDSS_ErrorType HAL_DSI_PhyPllSetup( HAL_DSI_PhyConfigType       *psDsiPhyConfig,
                                        HAL_DSI_PhyConfigInfoType   *psDsiPhyConfigInfo);



/****************************************************************************
*
** FUNCTION: HAL_DSI_PhySetupTimingParams()
*/
/*!
* \brief
*     Calculate PHY timing parameters.
*
* \param [in]  pTimingParameters - Phy timing parameters
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_PhySetupTimingParams(HAL_DSI_TimingSettingType        *pTimingParameters);

/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_Disable()
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
void HAL_DSI_Phy_1_5_Disable( DSI_Device_IDType   eDeviceId );

/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_Pll_1_6_2_PowerCtrl()
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
HAL_MDSS_ErrorType HAL_DSI_Phy_1_5_Pll_1_6_2_PowerCtrl( DSI_Device_IDType   eDeviceId,
                                            bool32              bPllPowerUp );


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_Setup()
*/
/*!
* \brief
*     Set up DSI Phy alone.
*
* \param [in]  eDeviceId       - DSI core ID
* \param [in]  psDsiPhyConfig  - Phy configuration
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_Phy_1_5_Setup(DSI_Device_IDType         eDeviceId, 
                                                HAL_DSI_PhyConfigType     *psDsiPhyConfig);


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_PllSetup()
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
HAL_MDSS_ErrorType HAL_DSI_Phy_1_5_PllSetup(HAL_DSI_PhyConfigType       *psDsiPhyConfig,
                                            HAL_DSI_PhyConfigInfoType   *psDsiPhyConfigInfo);



/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_SetupTimingParams()
*/
/*!
* \brief
*     Calculate PHY timing parameters.
*
* \param [in]  pTimingParameters - Phy timing parameters
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_Phy_1_5_SetupTimingParams(HAL_DSI_TimingSettingType        *pTimingParameters);

/*********************************************************************************************************************/
#if 0
/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_20nm_1_0_Disable()
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
void HAL_DSI_Phy_20nm_1_0_Disable( DSI_Device_IDType   eDeviceId );

/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_20nm_1_0_PowerCtrl()
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
HAL_MDSS_ErrorType HAL_DSI_Phy_20nm_1_0_PowerCtrl( DSI_Device_IDType   eDeviceId,
                                                   bool32              bPllPowerUp );

/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_20nm_1_0_Setup()
*/
/*!
* \brief
*     Set up DSI Phy alone.
*
* \param [in]  eDeviceId       - DSI core ID
* \param [in]  psDsiPhyConfig  - Phy configuration
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_Phy_20nm_1_0_Setup(DSI_Device_IDType         eDeviceId, 
                                              HAL_DSI_PhyConfigType     *psDsiPhyConfig);


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_20nm_1_0_PowerCtrl()
*/
/*!
* \brief
*      DSI Phy PLL power up/down sequence for DSI 20nm phy 1.0.0 versions,
*
*
* \param [in]  eDeviceId    - DSI core ID
* \param [in]  bPllPowerUp  - TRUE: power up, FALSE: power down
*
* \retval none
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_Phy_20nm_1_0_PowerCtrl(DSI_Device_IDType   eDeviceId,
                                                  bool32              bPllPowerUp);

/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_20nm_1_0_PllSetup()
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
HAL_MDSS_ErrorType HAL_DSI_Phy_20nm_1_0_PllSetup( HAL_DSI_PhyConfigType       *psDsiPhyConfig,
                                                  HAL_DSI_PhyConfigInfoType   *psDsiPhyConfigInfo);



/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_20nm_1_0_SetupTimingParams()
*/
/*!
* \brief
*     Calculate PHY timing parameters.
*
* \param [in]  pTimingParameters - Phy timing parameters
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_Phy_20nm_1_0_SetupTimingParams(HAL_DSI_TimingSettingType        *pTimingParameters);


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_20nm_1_0_PhyPllInitialize()
*/
/*!
* \brief
*     Initialize DSI PHY/PLL.
*
* \retval None
*
****************************************************************************/
void HAL_DSI_Phy_20nm_1_0_PhyPllInitialize(void);

/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_Mdp565_Disable()
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
void HAL_DSI_Phy_Mdp565_Disable(DSI_Device_IDType   eDeviceId);

/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_Mdp565_PowerCtrl()
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
HAL_MDSS_ErrorType HAL_DSI_Phy_Mdp565_PowerCtrl(DSI_Device_IDType   eDeviceId,
                                                bool32              bPllPowerUp);

/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_Mdp565_Setup()
*/
/*!
* \brief
*     Set up DSI Phy alone.
*
* \param [in]  eDeviceId       - DSI core ID
* \param [in]  psDsiPhyConfig  - Phy configuration
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_Phy_Mdp565_Setup(DSI_Device_IDType         eDeviceId,
                                            HAL_DSI_PhyConfigType     *psDsiPhyConfig);

/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_Mdp565_PllSetup()
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
HAL_MDSS_ErrorType HAL_DSI_Phy_Mdp565_PllSetup(HAL_DSI_PhyConfigType       *psDsiPhyConfig,
                                               HAL_DSI_PhyConfigInfoType   *psDsiPhyConfigInfo);

/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_Mdp565_SetupTimingParams()
*/
/*!
* \brief
*     Calculate PHY timing parameters.
*
* \param [in]  pTimingParameters - Phy timing parameters
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_Phy_Mdp565_SetupTimingParams(HAL_DSI_TimingSettingType        *pTimingParameters);

#endif
/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_FindVco()
*/
/*!
* \brief
*     Find Desired VCO clock based on Bit clock.
*
* \param [in]  uDesiredBitClk - desired Bit clock in Hz
*
* \retval uint32
*
****************************************************************************/
uint32 HAL_DSI_Phy_FindVco(uint32 uDesiredBitClk);

/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_FindPostDividerControl()
*/
/*!
* \brief
*     Find Post divider value
*
* \param [in]  uDesiredBitClk -Half of desired Bit clock in Hz
*
* \retval uint32
*
****************************************************************************/
uint32 HAL_DSI_Phy_FindPostDividerControl(uint32 uHalfBitClkInHz);

/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_FindNdiv()
*/
/*!
* \brief
*     Find N Divider value
*
* \param [in]  uDesiredBitClk -Half of desired Bit clock in Hz
*
* \retval uint32
*
****************************************************************************/
uint32 HAL_DSI_Phy_FindNdiv(uint32 uHalfBitClkInHz);

/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_CalcPixelClockDivider()
*/
/*!
* \brief
*     Calculate Pixel Clock divider.
*
* \param [in]  uHalfBitClkInHz      - Half of desired Bit clock
* \param [in]  uNumOfDataLanes      - number of data lanes
* \param [in]  uBitsPerPixel        - BPP of the panel
* \param [out] psDsiPhyConfigInfo   - PHY Config Info
*
* \retval uint32
*
****************************************************************************/
uint32 HAL_DSI_Phy_CalcPixelClockDivider(uint32                     uHalfBitClkInHz, 
                                         uint32                     uNumOfDataLanes, 
                                         uint32                     uBitsPerPixel, 
                                         HAL_DSI_PhyConfigInfoType *psDsiPhyConfigInfo);

#ifdef __cplusplus
}
#endif

#endif  /* #define HALDSI_PHY_H */
