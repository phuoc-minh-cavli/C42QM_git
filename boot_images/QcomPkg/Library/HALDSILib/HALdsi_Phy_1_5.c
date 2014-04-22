/*=============================================================================

  File: HALdsi_Phy.c
  

     Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
     Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif 

#include "HALdsi.h"
#include "HALdsi_Phy.h"
#include "HALdsi_Pll.h"
#include "dsiHostSystem.h"

/* -----------------------------------------------------------------------
** Local defines
** ----------------------------------------------------------------------- */

/* timeout counter values used in iterations of polling PLL ready status; 
 * each polling iteration contains a delay of 100us;                       */
#define HAL_DSI_PHY_PLL_READY_TIMEOUT           2550           /* ~255 ms */

/* Generic parameters that applies to all parameter calculations */
#define HAL_DSI_PHY_GENERIC_SHIFT_OFFSET        0x02
#define HAL_DSI_PHY_GENERIC_TIMING_MAX          0xFF
#define HAL_DSI_PHY_TIPX_NUMERATOR              1000000000.0f                    /* numerator for the TIPX formula */
#define HAL_DSI_PHY_PERCENT_DENOMENATOR         100.0f
#define HAL_DSI_PHY_TREOT                       20                               /* t_reot */
 
/* Range factor applied */
#define HAL_DSI_PHY_RANGEFACTOR_90              90.0f
#define HAL_DSI_PHY_RANGEFACTOR_50              50.0f
#define HAL_DSI_PHY_RANGEFACTOR_5               5.0f
#define HAL_DSI_PHY_BITCLK_RANGE_FREQ_1         180000000                        /* bit clk frequency in the unit of Hz */
#define HAL_DSI_PHY_BITCLK_RANGE_FREQ_2         1200000000                       /* bit clk frequency in the unit of Hz */


/* Recommended values for TCLK_PREPARE formula */
#define HAL_DSI_PHY_TCLK_PREPARE_PHY_MIN        38.0f
#define HAL_DSI_PHY_TCLK_PREPARE_PHY_MAX        95.0f

/* Recommended Values for TCLK_ZERO formula */
#define HAL_DSI_PHY_TCLK_ZERO_PARAM1            300.0f

/* Recommended Values for TCLK_TRAIL formula */
#define HAL_DSI_PHY_TCLK_TRAIL_MIN              60.0f

/* Recommended Values for T_HS_EXIT formula */
#define HAL_DSI_PHY_T_HS_EXIT_MIN               100.0f

/* Recommended Values for T_HS_ZERO formula */
#define HAL_DSI_PHY_T_HS_ZERO_PARAM1            0x91
#define HAL_DSI_PHY_T_HS_ZERO_PARAM2            0x0A
#define HAL_DSI_PHY_T_HS_ZERO_PARAM3            0x18

/* Recommended Values for T_HS_PREPARE formula */
#define HAL_DSI_PHY_T_HS_PREPARE_PARAM1         0x28
#define HAL_DSI_PHY_T_HS_PREPARE_PARAM2         0x04
#define HAL_DSI_PHY_T_HS_PREPARE_PARAM3         0x55
#define HAL_DSI_PHY_T_HS_PREPARE_PARAM4         0x06

/* Recommended Values for T_HS_TRAIL formula */
#define HAL_DSI_PHY_T_HS_TRAIL_PARAM1           0x3C
#define HAL_DSI_PHY_T_HS_TRAIL_PARAM2           0x04

/* Recommended Values for T_HS_RQST formula */
#define HAL_DSI_PHY_T_HS_RQST_PARAM1            0x19

/* Recommended Values for T_TA_GO formula */
#define HAL_DSI_PHY_T_TA_GO_PARAM1              0x03

/* Recommended Values for T_TA_SURE formula */
#define HAL_DSI_PHY_T_TA_SURE_PARAM1            0x00

/* Recommended Values for T_TA_GET formula */
#define HAL_DSI_PHY_T_TA_GET_PARAM1             0x04

/* Recommended Values for TEOT formula */
#define HAL_DSI_PHY_TEOT_PARAM1                 0x69
#define HAL_DSI_PHY_TEOT_PARAM2                 0x0C

/* Recommended Values for T_CLK_PRE formula */
#define HAL_DSI_PHY_T_CLK_PRE_PARAM1            0x08
#define HAL_DSI_PHY_T_CLK_PRE_PARAM2            0x08
#define HAL_DSI_PHY_T_CLK_PRE_PARAM3            0x01
#define HAL_DSI_PHY_T_CLK_PRE_MAX               0x3F

/* Recommended Values for T_CLK_POST formula */
#define HAL_DSI_PHY_T_CLK_POST_PARAM1           0x3C
#define HAL_DSI_PHY_T_CLK_POST_PARAM2           0x34
#define HAL_DSI_PHY_T_CLK_POST_PARAM3           0x18
#define HAL_DSI_PHY_T_CLK_POST_PARAM4           0x08
#define HAL_DSI_PHY_T_CLK_POST_PARAM5           0x01
#define HAL_DSI_PHY_T_CLK_POST_MAX              0x3F


#define HAL_DSI_PHY_TIMING_CTRL_11              0xA0

#define HAL_DSI_PHY_SETPHYCTRL0_STEP1           1
#define HAL_DSI_PHY_SETPHYCTRL0_STEP2           2

#define HAL_DSI_PHY_PLLSEQUENCE_MAXTRY          3


/* -----------------------------------------------------------------------
** Local Data Types
** ----------------------------------------------------------------------- */

/*!
 * \struct _HAL_DSI_PllSettingType
 *
 * PLL parameters
 */
typedef struct _HAL_DSI_PllSettingType
{
   uint32   uActBitClkFreq;          /* actual bit clock frequency generated in Hz */
   uint32   uVcoClkFreq;             /* VCO output frequency in Hz */
   
   uint8    uPllRefClkDiv;           /* DSI_x_PHY_PLL_UNIPHY_PLL_REFCLK_CFG.PLL_REFCLK_DIV */
   uint8    uPllRefClkDblr;          /* DSI_x_PHY_PLL_UNIPHY_PLL_REFCLK_CFG.PLL_REFCLK_DBLR */
   uint8    uPllSdmByp;              /* DSI_x_PHY_PLL_UNIPHY_PLL_SDM_CFG0.PLL_SDM_BYP */
   uint8    uPllSdmBypDiv;           /* DSI_x_PHY_PLL_UNIPHY_PLL_SDM_CFG0.PLL_SDM_BYP_DIV */
   uint8    uPllSdmDcOffset;         /* DSI_x_PHY_PLL_UNIPHY_PLL_SDM_CFG1.PLL_SDM_DC_OFFSET */
   uint8    uPllSdmFreqSeed_7_0;     /* DSI_x_PHY_PLL_UNIPHY_PLL_SDM_CFG2.PLL_SDM_FREQ_SEED_7_0 */
   uint8    uPllSdmFreqSeed_15_8;    /* DSI_x_PHY_PLL_UNIPHY_PLL_SDM_CFG3.PLL_SDM_FREQ_SEED_15_8 */
   
   uint8    uPllPostDiv1;            /* DSI_x_PHY_PLL_UNIPHY_PLL_POSTDIV1_CFG.PLL_POSTDIV1 */
   uint8    uPllPostDiv1BypassB;     /* DSI_x_PHY_PLL_UNIPHY_PLL__VREG_CFG.PLL_POSTDIV1_BYPASS_B */
   uint8    uPllPostDiv2;            /* DSI_x_PHY_PLL_UNIPHY_PLL_POSTDIV2_CFG.PLL_POSTDIV2 */
   uint8    uPllPostDiv3;            /* DSI_x_PHY_PLL_UNIPHY_PLL_POSTDIV3_CFG.PLL_POSTDIV3 */
   
   uint8    uPllCpIdac;              /* DSI_x_PHY_PLL_UNIPHY_PLL_CHGPUMP_CFG.PLL_CP_IDAC */
   uint8    uPllLpfR;                /* DSI_x_PHY_PLL_UNIPHY_PLL_LPFR_CFG.PLL_LPF_R */   
   uint8    uPllLpfC1;               /* DSI_x_PHY_PLL_UNIPHY_PLL_LPFC1_CFG.PLL_LPF_C1 */
   uint8    uPllLpfC2;               /* DSI_x_PHY_PLL_UNIPHY_PLL_LPFC2_CFG.PLL_LPF_C2 */
   uint8    uPllC3Sel;               /* DSI_x_PHY_PLL_UNIPHY_PLL_VCOLPF_CFG.PLL_C3_SEL */
   uint8    uPllBypassP3;            /* DSI_x_PHY_PLL_UNIPHY_PLL_VCOLPF_CFG.PLL_BYPASS_P3 */
} HAL_DSI_PllSettingType;




/* -----------------------------------------------------------------------
** Local functions
** ----------------------------------------------------------------------- */

/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_FindPllSettings()
*/
/*!
* \brief
*     Find the PLL settings for the desired DSI bit clock frequency.
*
* \param [in]  uDesiredBitClk - desired DSI bit clock frequency in Hz
* \param [out] psPllSettings  - calculation output of PLL settings
*
* \retval - HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_DSI_Phy_1_5_FindPllSettings ( uint32                    uDesiredBitClk,
                                                       HAL_DSI_PllSettingType   *psPllSettings )
{
   HAL_MDSS_ErrorType   eStatus = HAL_MDSS_STATUS_SUCCESS;
   uint32               uAcceptableBitClkMin = 0;
   uint32               uAcceptableBitClkMax = 0;
   uint32               uHalfRateBitClk = 0;
   uint32               uGeneratedVcoClk = 0;
   uint32               uGeneratedBitClk = 0;
   uint32               uRefClkToPll = 0;
   uint32               uVcoClk = 0;
   uint32               uIndex = 0;
   uint32               uTableIndex = 0;
   uint8                uPostDiv1BypassB = 0;
   uint32               uIntFracMode = 0;
   uint8                uPostDiv = 0;
   double               fDivFb = 0.0;

   /* accuracy range for generated bit-clock */ 
   uAcceptableBitClkMin = uDesiredBitClk - DSIPLL_BIT_CLK_ACCURACY;
   uAcceptableBitClkMax = uDesiredBitClk + DSIPLL_BIT_CLK_ACCURACY;

   /* choose the analog post divider value to have the lower VCO frequency */
   uHalfRateBitClk = uDesiredBitClk>>1;
   if (uHalfRateBitClk >= DSIPLL_MIN_VCO_CLOCK)
   {
      /* direct path, analog post divider is bypassed, postdiv is always 1 */
      uPostDiv1BypassB = 0;
      uPostDiv = 1;
   }
   else
   {
      double   fPostDiv;
      uint8    uPostDivInt;
      uint32   uPostDivFrac;

      /* non-direct path, analog post divider is in, and also a fixed divider of 2 */
      uPostDiv1BypassB = 1;

      /* figure out postdiv */
      fPostDiv = (double)DSIPLL_MIN_VCO_CLOCK/(double)uHalfRateBitClk;
      uPostDivInt = (uint8)fPostDiv;
      uPostDivFrac = ((uint32)(fPostDiv * 1000000.0)) % 1000000;
      if ((uPostDivFrac == 0) && ((uPostDivInt & 0x1) == 0))
      {
          /* fPostDiv is an even number of integer */
          uPostDiv = uPostDivInt>>1;
          }
          else
          {
          /* fPostDiv has fraction part */
          uPostDiv = (uint8)(fPostDiv/2.0 + 1.0);
      }
   }

   /* make sure uPostDiv is within the reange */
   if ((uPostDiv == 0) || (uPostDiv > DSIPLL_MAX_POST_DIV))
   {
      /* the requested analog post divider value is out of range */
      eStatus = HAL_MDSS_DSI_FAILED_PARAMETER_OUT_OF_RANGE;
   }
   else
   {
      /* VCO must be within the limited range */
      uVcoClk = (uPostDiv1BypassB == 1) ?  (uHalfRateBitClk * 2 * uPostDiv) : uHalfRateBitClk;
      if ((uVcoClk < DSIPLL_MIN_VCO_CLOCK) || (uVcoClk > DSIPLL_MAX_VCO_CLOCK))
      {
         /* the requested DSI PLL VCO frequency is out of range */
         eStatus = HAL_MDSS_DSI_FAILED_PARAMETER_OUT_OF_RANGE;
      }
      else
      {
         /* determine Integer-N mode or Frac-N mode */
         uIntFracMode = (uint32)(((double)uVcoClk / (double)DSIPLL_REF_CLOCK) * 1000000.0) % 1000000;
         if (uIntFracMode)
         {
            /* fraction-N mode, double reference clock */
            uRefClkToPll = DSIPLL_REF_CLOCK * 2;
         }
         else
         {
            /* integer-N mode, direct reference clock */
            uRefClkToPll = DSIPLL_REF_CLOCK;
         }
   
         /* calculate feedback divider */
         fDivFb = (double)uVcoClk / (double)uRefClkToPll;

         /* check if the generated bit clcok frequency is within the acceptable range */
         uGeneratedVcoClk = (uint32)(fDivFb * uRefClkToPll);
         uGeneratedBitClk = ((uPostDiv1BypassB == 1) ? uGeneratedVcoClk/(2 * uPostDiv) : uGeneratedVcoClk) * 2;
         if ((uGeneratedBitClk < uAcceptableBitClkMin) || (uGeneratedBitClk > uAcceptableBitClkMax))
         {
            /* the generated bit clock frequency does not fall into the acceptable range */
            eStatus = HAL_MDSS_DSI_STATUS_FAILED;
         }
      }
   }

   if (eStatus == HAL_MDSS_STATUS_SUCCESS)
   {
      /* look up the pre-calculated table to get the corresponding LPF settings */
      /* VCO frequency increases at the step of 500KHz in the pre-calculated LPF setting table */
      uIndex = (uGeneratedVcoClk - DSIPLL_MIN_VCO_CLOCK)/(500*1000);
      if (uIndex < (DSI_PLL_LPF_SETTING_TABLE_SIZE - 1))
      {
          uint32    uDist1;
          uint32    uDist2;

          /* VCO frequency stored in the table is in KHz */
          uDist1 = uGeneratedVcoClk/1000 - gDsiPllLpfSettingTable[uIndex].uVcoClkFreq;
          uDist2 = gDsiPllLpfSettingTable[uIndex+1].uVcoClkFreq - uGeneratedVcoClk/1000;               
          uTableIndex = (uDist1 > uDist2) ? (uIndex + 1) : uIndex;
      }
      else
      {
         uTableIndex = uIndex;
      }

      /* get the LPF settings from the pre-calculated table */
      psPllSettings->uPllLpfR = (uint8)(gDsiPllLpfSettingTable[uTableIndex].uPllLpfR);
          
      /* populate the calculated results into the data structure for configuring PLL register fields */
      psPllSettings->uActBitClkFreq = uGeneratedBitClk;
      psPllSettings->uVcoClkFreq = uGeneratedVcoClk;

      /* settings of ref divider, feedback divider and SDM */ 
      psPllSettings->uPllRefClkDiv = 0;      /* HW requires refdiv be always 0 */
      if (uIntFracMode)
      {
         uint16    uDivFbFrac;

         /* fraction-N mode */
         psPllSettings->uPllRefClkDblr = 1;
         psPllSettings->uPllSdmByp = 0;
         psPllSettings->uPllSdmBypDiv = 0;
         psPllSettings->uPllSdmDcOffset = (uint8)(fDivFb - 1);
         uDivFbFrac = (uint16)((fDivFb - (uint32)fDivFb) * 65536.0 + 0.5);
         psPllSettings->uPllSdmFreqSeed_7_0  = (uint8)(uDivFbFrac & 0xFF);
         psPllSettings->uPllSdmFreqSeed_15_8 = (uint8)((uDivFbFrac >> 8) & 0xFF);
      }
      else
      {
         /* integer-N mode */
         psPllSettings->uPllRefClkDblr = 0;
         psPllSettings->uPllSdmByp = 1;
         psPllSettings->uPllSdmBypDiv = (uint8)(fDivFb - 1);
         psPllSettings->uPllSdmDcOffset = 0;
         psPllSettings->uPllSdmFreqSeed_7_0 = 0;
         psPllSettings->uPllSdmFreqSeed_15_8 = 0;
      }

      /* post dividers */
      psPllSettings->uPllPostDiv1 = uPostDiv - 1;    /* minus 1 for the field in HW register */
      psPllSettings->uPllPostDiv1BypassB = uPostDiv1BypassB;
      psPllSettings->uPllPostDiv2 = 3;       /* HW requires postdiv2 be always 3 */
          
      /* settings of charge pump and LPF */
      psPllSettings->uPllCpIdac   = 2;     /* always set to 2 */
      psPllSettings->uPllLpfC1    = 112;   /* always use 125pF */
      psPllSettings->uPllLpfC2    = 21;    /* always use 3.757pF */
      psPllSettings->uPllC3Sel    = 0;     /* always use 3.34pF */
      psPllSettings->uPllBypassP3 = 1;     /* always use set to 1 */
   }

   return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_GetPClkPostDivider()
*/
/*!
* \brief
*     Get post divider value for PClk.
*
* direct path:       ByteClk = VcoCLk/4    (Postdiv1 bypassed)
*                    PClk = VcoClk/Postdiv
* non-direct path:   ByteClk = VcoCLk/(8*postdiv1)
*                    PClk = VcoClk/Postdiv
*   and
*                    PClk =  (ByteClk * Lanes)/BytesPerPixel
*                    BytesPerPixel = BitPerPixel/8
*
* so, Postdiv for PClk is:
*
* direct path:        Postdiv = BitPerPiexl/(2*lanes);
* non-direct path:    Postdiv = (Postdiv1*BitPerPiexl)/lanes;
*
* ByteClk path passes through MND in CC without further dividing.
* PCLK path has 1 post divider (Postdiv3) in PLL, and a MND divider in CC, 
* in most cases its MND can be set to 1, there are a few exceptions need to
* have a M and N setting.
* so, Postdiv for PClk is:
*
* direct path:      Postdiv3 = Postdiv*M/N = (BitPerPiexl*M)/(2*lanes*N)
* non-direct path:  Postdiv3 = Postdiv*M/N = (Postdiv1*BitPerPiexl*M)/(lanes*N)
*
*
* \param [in]   psPllSettings - PLL setting parameters
* \param [in]   uBitsPerPixel - bits per pixel
* \param [in]   uNumOfLanes   - number of DSI data lanes
*
* \param [out]  psConfigInfo  - Phy config passback info
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_DSI_Phy_1_5_GetPClkPostDivider( HAL_DSI_PllSettingType      *psPllSettings,
                                                         HAL_DSI_PhyConfigInfoType   *psConfigInfo,
                                                         uint32                       uBitsPerPixel,
                                                         uint32                       uNumOfLanes )
{
   HAL_MDSS_ErrorType   eStatus = HAL_MDSS_STATUS_SUCCESS;

   /* init numerator and denominator of divider ratio in CC to 1 */
   psConfigInfo->uPClkDivNumerator   = 1;
   psConfigInfo->uPClkDivDenominator = 1;

   /* determine dividernumerator and denominato for exception cases */
   if (uBitsPerPixel == 18)
   {
      switch (uNumOfLanes)
      {
      case 1:
         psConfigInfo->uPClkDivNumerator   = 2;
         psConfigInfo->uPClkDivDenominator = 9;
         break;
          
      case 2:
         psConfigInfo->uPClkDivNumerator   = 2;
         psConfigInfo->uPClkDivDenominator = 9;
         break;
          
      case 4:
         psConfigInfo->uPClkDivNumerator   = 4;
         psConfigInfo->uPClkDivDenominator = 9;
         break;
          
      default:
         break;
      }            
   }
   else if ((uBitsPerPixel == 16) && (uNumOfLanes == 3))
   {
      psConfigInfo->uPClkDivNumerator   = 3;
      psConfigInfo->uPClkDivDenominator = 8;        
   }
   
   psConfigInfo->uPllVcoOutputFreq = psPllSettings->uVcoClkFreq;
   psConfigInfo->uBitClkFreq = psPllSettings->uActBitClkFreq;

   /* determine PLL postdiv3 */
   if (psPllSettings->uPllPostDiv1BypassB == 0)
   {
      /* direct path: ByteClk = VcoCLk/4 */
      psConfigInfo->uByteClkFreq = psPllSettings->uVcoClkFreq >> 2;
     
      /* Postdiv3 = (BitPerPiexl*M)/(2*lanes*N) */
      psPllSettings->uPllPostDiv3 = (uint8)((uBitsPerPixel * psConfigInfo->uPClkDivNumerator) /
                                    (2 * uNumOfLanes * psConfigInfo->uPClkDivDenominator));
   }
   else
   {
      /* non-direct path: ByteClk = VcoCLk/(8*postdiv1); add 1 to postdiv1 as postdiv1 holds the value for the HW field; */
      psConfigInfo->uByteClkFreq = psPllSettings->uVcoClkFreq / (8*(psPllSettings->uPllPostDiv1 + 1));
       
      /* Postdiv3 = (Postdiv1*BitPerPiexl*M)/(lanes*N) */
      psPllSettings->uPllPostDiv3 = (uint8)(((psPllSettings->uPllPostDiv1 + 1) * uBitsPerPixel * psConfigInfo->uPClkDivNumerator) / 
                                    (uNumOfLanes * psConfigInfo->uPClkDivDenominator));
   }

   /* get output PClk frequency, DsiClk = VcoCLk/postdiv3, PClk = DsiClk*M/N */
   if (psPllSettings->uPllPostDiv3 == 0)
   {
      eStatus = HAL_MDSS_DSI_STATUS_FAILED;
   }
   else
   {
      psConfigInfo->uDsiClkFreq = psPllSettings->uVcoClkFreq / psPllSettings->uPllPostDiv3;
      psConfigInfo->uPclkFreq   = (psConfigInfo->uDsiClkFreq * psConfigInfo->uPClkDivNumerator) / psConfigInfo->uPClkDivDenominator;
      psPllSettings->uPllPostDiv3--;  /* the field in HW register is defined as the divider value minus 1 */
   }
   
   return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_SetupPll()
*/
/*!
* \brief
*     Set up DSI Phy PLL.
*
*
* \param [in]  eDeviceId     - DSI core ID
* \param [in]  psPllSettings - PLL setting parameters
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_DSI_Phy_1_5_SetupPll(DSI_Device_IDType         eDeviceId, 
                                              HAL_DSI_PllSettingType   *psPllSettings)
{
   HAL_MDSS_ErrorType   eStatus = HAL_MDSS_STATUS_SUCCESS;
   uint32               uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId);

   /* set charge pump, LPF etc.  */   
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_LPFR_CFG_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_LPFR_CFG, PLL_LPF_R, psPllSettings->uPllLpfR) );

   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_LPFC1_CFG_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_LPFC1_CFG, PLL_LPF_C1, psPllSettings->uPllLpfC1) );
   
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_LPFC2_CFG_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_LPFC2_CFG, PLL_LPF_C2, psPllSettings->uPllLpfC2) );

   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_CHGPUMP_CFG_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_CHGPUMP_CFG, PLL_CP_IDAC, psPllSettings->uPllCpIdac) );
   

   /* set post dividers */
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_POSTDIV1_CFG_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_POSTDIV1_CFG, PLL_POSTDIV1, psPllSettings->uPllPostDiv1) );

   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_POSTDIV2_CFG_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_POSTDIV2_CFG, PLL_POSTDIV2, psPllSettings->uPllPostDiv2) );

   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_POSTDIV3_CFG_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_POSTDIV3_CFG, PLL_POSTDIV3, psPllSettings->uPllPostDiv3) );


   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_CAL_CFG3_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_CAL_CFG3, PLL_VCO_MAN_OFFSET, 0x2B) );

   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_CAL_CFG4_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_CAL_CFG4, PLL_VCO_MAN_SLOPE, 0x6) );

   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_LKDET_CFG2_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_LKDET_CFG2, PLL_LOCKDET_PPM, 1) |  
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_LKDET_CFG2, PLL_LOCKDET_EN, 1) );
   

   /* set sigma-delta modulation */    
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_SDM_CFG1_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_SDM_CFG1, PLL_SDM_DC_OFFSET, psPllSettings->uPllSdmDcOffset) );
   
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_SDM_CFG2_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_SDM_CFG2, PLL_SDM_FREQ_SEED_7_0, psPllSettings->uPllSdmFreqSeed_7_0) );
   
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_SDM_CFG3_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_SDM_CFG3, PLL_SDM_FREQ_SEED_15_8, psPllSettings->uPllSdmFreqSeed_15_8) );
   
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_SDM_CFG4_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_SDM_CFG4, PLL_SDM_FREQ_SEED_17_16, 0) );

   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_REFCLK_CFG_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_REFCLK_CFG, PLL_REFCLK_DIV, psPllSettings->uPllRefClkDiv) |
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_REFCLK_CFG, PLL_REFCLK_DBLR, psPllSettings->uPllRefClkDblr) );
   
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_PWRGEN_CFG_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_PWRGEN_CFG, PLL_PWRGEN_CTRL, 0) );

   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_VCOLPF_CFG_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_VCOLPF_CFG, PLL_C3_SEL, psPllSettings->uPllC3Sel) |
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_VCOLPF_CFG, PLL_BYPASS_P3, psPllSettings->uPllBypassP3) |
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_VCOLPF_CFG, PD_LP_VCO, 1) |
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_VCOLPF_CFG, PD_HP_VCO, 1) |
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_VCOLPF_CFG, RSVD_7_6,  1)); 
  
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_VREG_CFG_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_VREG_CFG, PLL_POSTDIV1_BYPASS_B, psPllSettings->uPllPostDiv1BypassB) );
       
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_SDM_CFG0_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_SDM_CFG0, PLL_SDM_BYP, psPllSettings->uPllSdmByp) |
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_SDM_CFG0, PLL_SDM_BYP_DIV, psPllSettings->uPllSdmBypDiv) );

   /* set calibrations */
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_CAL_CFG0_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_CAL_CFG0, PLL_CAL_WAITTIME, 0x1) |
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_CAL_CFG0, PLL_CAL_MODE, 0x2) );

   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_CAL_CFG6_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_CAL_CFG6, PLL_CAL_KVCO_COUNT_7_0, 0x30) );
   
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_CAL_CFG7_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_CAL_CFG7, PLL_CAL_KVCO_COUNT_9_8, 0x0) );


   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_CAL_CFG8_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_CAL_CFG8, PLL_CAL_REFCLK_DIV_7_0, 0x5F) );  
   
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_CAL_CFG9_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_CAL_CFG9, PLL_CAL_REFCLK_DIV_9_8, 0x0) );

   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_CAL_CFG10_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_CAL_CFG10, PLL_VCO_FREQ_7_0, (psPllSettings->uVcoClkFreq/1000000) & 0xFF) );

   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_CAL_CFG11_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_CAL_CFG11, PLL_VCO_FREQ_10_8, ((psPllSettings->uVcoClkFreq/1000000)>>8) & 0x7) );

   /* power up PLL, and poll if PLL locked */
   eStatus = HAL_DSI_PhyPllPowerCtrl(eDeviceId, TRUE);

   /* pll efuse setting */
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_EFUSE_CFG_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_EFUSE_CFG, PLL_EFUSE_OVRRIDE_SEL, 0x1) );
   
   return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_Clear_Pll_Ready()
*/
/*!
* \brief
*     Toggle the LKDET_CFG2_ADDR to make sure that UNIPHY_PLL_STATUS_ADDR is in good state.
*
* \param [in]  eDeviceId - DSI core ID
*
* \retval None
*
****************************************************************************/
static void HAL_DSI_Phy_1_5_Clear_Pll_Ready(DSI_Device_IDType   eDeviceId)
{
  uint32               uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId);
   
  out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_LKDET_CFG2_ADDR,0x4); 
  DSI_OSAL_SleepUs(1);  /*delay ~1us*/
  out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_LKDET_CFG2_ADDR,0x5);

}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_PllLockDetect()
*/
/*!
* \brief
*     Wait and check if PLL locked.
*
* \param [in]  eDeviceId - DSI core ID
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_DSI_Phy_1_5_PllLockDetect(DSI_Device_IDType    eDeviceId)
{
   HAL_MDSS_ErrorType   eStatus = HAL_MDSS_STATUS_SUCCESS;
   uint32               uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId);
   uint32               uTimeout = HAL_DSI_PHY_PLL_READY_TIMEOUT;
   uint32               uPllStatus;

   /*Before checking the PLL lock, clear the PLL Ready to ensure that PLL shows correct status */
   HAL_DSI_Phy_1_5_Clear_Pll_Ready(eDeviceId);

   uPllStatus = in_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_STATUS_ADDR) & HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_STATUS_PLL_RDY_BMSK;
   while ((!uPllStatus) && (uTimeout))
   {
      DSI_OSAL_SleepUs(100);  /*delay ~100us*/
      uPllStatus = in_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_STATUS_ADDR) & HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_STATUS_PLL_RDY_BMSK;
      uTimeout--;
   }

   if (uPllStatus != HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_STATUS_PLL_RDY_BMSK)
   {
      /* timeout while polling the lock status */
      eStatus = HAL_MDSS_DSI_FAILED_UNABLE_TO_INIT_HW;
   }

   return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_SetAnaLaneConfig()
*/
/*!
* \brief
*     Set analog lane config.
*
* \param [in]  eDeviceId     - DSI core ID
*             [in]  uClockStrengthHS - Clock strength in high speed
*             [in]  uDataStrengthHS  - Data strength in high speed
*
* \retval None
*
****************************************************************************/
static void HAL_DSI_Phy_1_5_SetAnaLaneConfig(DSI_Device_IDType    eDeviceId,
                                        uint32               uClockStrengthHS,
                                        uint32               uDataStrengthHS)
 {
    uint32   uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId);
    uint32   n;
        
    for (n=0; n<4; n++)
    {
       out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_LNn_CFG0_ADDR(n), 0x01);
       out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_LNn_CFG1_ADDR(n), 0xC0);
       out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_LNn_CFG2_ADDR(n), 0x00);
       out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_LNn_CFG3_ADDR(n), 0x00);
       out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_LNn_CFG4_ADDR(n), 0x00);        
    }
    
    out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_LNCK_CFG0_ADDR, 0x00);
    out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_LNCK_CFG1_ADDR, 0x40);
    out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_LNCK_CFG2_ADDR, 0x00);
    out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_LNCK_CFG3_ADDR, 0x00);
    out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_LNCK_CFG4_ADDR, 0x00);
 
    for (n=0; n<4; n++)
    {
       out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_LNn_TEST_DATAPATH_ADDR(n), 0x00);
    }
    out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_LNCK_TEST_DATAPATH_ADDR, 0x00);
    
    for (n=0; n<4; n++)
    {
       out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_LNn_DEBUG_SEL_ADDR(n), 0x00);
    }
    out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_LNCK_DEBUG_SEL_ADDR, 0x00);
 
    // Setup strengths
    for (n=0; n<4; n++)
    {
       // Data lanes
       out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_LNn_TEST_STR0_ADDR(n), 0x01);
    }
    // Clock lane
    out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_LNCK_TEST_STR0_ADDR, 0x01);
    
    for (n=0; n<4; n++)
    {
       // Data lanes
       if (0 == uDataStrengthHS)
       {
         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_LNn_TEST_STR1_ADDR(n), 0x97);
       }
       else
       {
         uint32 uDataStrengthVal = uDataStrengthHS-1;  // Subtract one to be 0 based
         
         // Override pull up and pull down strength
         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_LNn_TEST_STR1_ADDR(n), uDataStrengthVal & 
           (HWIO_MMSS_DSI_0_PHY_DSIPHY_LNn_TEST_STR1_DSIPHY_HSTX_STR_HSTOP_BMSK | HWIO_MMSS_DSI_0_PHY_DSIPHY_LNn_TEST_STR1_DSIPHY_HSTX_STR_HSBOT_BMSK));
       }
    }
 
    // Clock lane
    if (0 == uClockStrengthHS)
    {
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_LNCK_TEST_STR1_ADDR, 0xFF);
    }
    else
    {
      uint32 uClockStrengthVal = uClockStrengthHS-1;  // Subtract one to be 0 based
      
      // Override pull up and pull down strength
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_LNCK_TEST_STR1_ADDR, uClockStrengthVal &
       (HWIO_MMSS_DSI_0_PHY_DSIPHY_LNCK_TEST_STR1_DSIPHY_HSTX_STR_HSTOP_BMSK | HWIO_MMSS_DSI_0_PHY_DSIPHY_LNCK_TEST_STR1_DSIPHY_HSTX_STR_HSBOT_BMSK));
    }

    out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_CTRL_4_ADDR, 0x0A);
    
    return;
 }



/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_Roundup()
*/
/*!
* \brief
*     Rounds up to the nearest integer.
*
*
* \param [in]  fFloatVal - float number to be processed;
*
* \retval  rounded up integer;
*
****************************************************************************/
static int32 HAL_DSI_Phy_1_5_Roundup(float fFloatVal)
{
  int32  iRoundupVal = (int32)fFloatVal;

  if(fFloatVal - iRoundupVal > 0)
    iRoundupVal ++;
  else if(fFloatVal - iRoundupVal < 0)
    iRoundupVal --;

  return iRoundupVal;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_SetGlobalTestCtrl()
*/
/*!
* \brief
*     Set  global test control.
*
*
* \param [in]  eDeviceId - DSI core ID
*
* \retval None
*
****************************************************************************/
static void HAL_DSI_Phy_1_5_SetGlobalTestCtrl(DSI_Device_IDType    eDeviceId)
{
   uint32   uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId);
          
   /* global test control */ 
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_GLBL_TEST_CTRL_ADDR, 
            HWIO_SET_FLD(DSI_0_PHY_DSIPHY_GLBL_TEST_CTRL, TEST_ESC_CLK_SEL, 0) |
            HWIO_SET_FLD(DSI_0_PHY_DSIPHY_GLBL_TEST_CTRL, DSIPHY_BITCLK_HS_SEL, 1) );  /* to be tested: 0 or 1 for DSI_1 */
   
   return;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_SetRegBistOff()
*/
/*!
* \brief
*     Set BIST off
*
*
* \param [in]  eDeviceId - DSI core ID
*
* \retval None
*
****************************************************************************/
static void HAL_DSI_Phy_1_5_SetRegBistOff(DSI_Device_IDType    eDeviceId)
{
   uint32   uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId);

   /* BIST off */
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_BIST_CTRL0_ADDR, 0x00);
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_BIST_CTRL1_ADDR, 0x00);
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_BIST_CTRL2_ADDR, 0x01);
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_BIST_CTRL3_ADDR, 0xFF);
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_BIST_CTRL4_ADDR, 0x00);
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_BIST_CTRL5_ADDR, 0x00);
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_BIST_CTRL4_ADDR, 0x00);

   return;
}

    
/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_EnableLdoRegulator()
*/
/*!
* \brief
*     
*
* \param [in]  eDeviceId - DSI core ID
*
* \retval None
*
****************************************************************************/
static void HAL_DSI_Phy_1_5_EnableLdoRegulator(DSI_Device_IDType    eDeviceId)
{
   uint32   uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId);

   /* enable LDO regulator */
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_LDO_CNTRL_ADDR, 0x05);

   return;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_EnableDcdcRegulator()
*/
/*!
* \brief
*           Enables DCDC Regulator, disables resistor divider   
*
*
* \param [in]  eDeviceId - DSI core ID
*
* \retval None
*
****************************************************************************/
static void HAL_DSI_Phy_1_5_EnableDcdcRegulator(DSI_Device_IDType    eDeviceId)
{
   uint32   uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId);

   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_REG_DSIPHY_REGULATOR_CAL_PWR_CFG_ADDR, 0x00000001);
   
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_REG_DSIPHY_REGULATOR_CTRL_3_ADDR, 0x00000000);
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_REG_DSIPHY_REGULATOR_CTRL_2_ADDR, 0x00000001);
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_REG_DSIPHY_REGULATOR_CTRL_1_ADDR, 0x00000001);
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_REG_DSIPHY_REGULATOR_CTRL_0_ADDR, 0x00000003);
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_REG_DSIPHY_REGULATOR_CTRL_4_ADDR, 0x00000020);

   return;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_ResetPhy()
*/
/*!
* \brief   
*
*
* \param [in]  eDeviceId - DSI core ID
*
* \retval None
*
****************************************************************************/
static void HAL_DSI_Phy_1_5_ResetPhy(DSI_Device_IDType    eDeviceId)
{
   uint32   uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId);

   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_SW_RESET_ADDR, 0x1);
   DSI_OSAL_SleepUs(1000);          /* delay ~1ms */
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_SW_RESET_ADDR, 0x0);
    
   return;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_SetStrengthCtrl0()
*/
/*!
* \brief   
*
*
* \param [in]  eDeviceId       - DSI core ID
*        [in]  uDataStrengthLP - Data strength in LP
*
* \retval None
*
****************************************************************************/
static void HAL_DSI_Phy_1_5_SetStrengthCtrl0(DSI_Device_IDType    eDeviceId,
                                       uint32               uDataStrengthLP)
 {
    uint32   uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId);
 
    if (0 == uDataStrengthLP)
    {
      // Default LP strength
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_STRENGTH_CTRL_0_ADDR, 0x0F);
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_STRENGTH_CTRL_0_ADDR, 0xFF);
    }
    else
    {
      uint32 uStrengthVal = uDataStrengthLP-1; // Subtract one to be one based.
      uint32 uStrength    = ((uStrengthVal<<HWIO_MMSS_DSI_0_PHY_DSIPHY_STRENGTH_CTRL_0_DSIPHY_STR_LP_P_SHFT) & HWIO_MMSS_DSI_0_PHY_DSIPHY_STRENGTH_CTRL_0_DSIPHY_STR_LP_P_BMSK) |
                            ((uStrengthVal<<HWIO_MMSS_DSI_0_PHY_DSIPHY_STRENGTH_CTRL_0_DSIPHY_STR_LP_N_SHFT) & HWIO_MMSS_DSI_0_PHY_DSIPHY_STRENGTH_CTRL_0_DSIPHY_STR_LP_N_BMSK);
 
      // Positive first
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_STRENGTH_CTRL_0_ADDR, uStrength & HWIO_MMSS_DSI_0_PHY_DSIPHY_STRENGTH_CTRL_0_DSIPHY_STR_LP_P_BMSK);
 
      // Positive & Negative     
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_STRENGTH_CTRL_0_ADDR, uStrength);
    }
 
    return;
 }



/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_SetStrengthCtrl1()
*/
/*!
* \brief   
*
*
* \param [in]  eDeviceId - DSI core ID
*
* \retval None
*
****************************************************************************/
static void HAL_DSI_Phy_1_5_SetStrengthCtrl1(DSI_Device_IDType    eDeviceId)
{
   uint32   uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId);

   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_STRENGTH_CTRL_1_ADDR, 0x06);

   return;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_SetPhyCtrl0()
*/
/*!
* \brief   
*
*
* \param [in]  eDeviceId - DSI core ID
*
* \retval None
*
****************************************************************************/
static void HAL_DSI_Phy_1_5_SetPhyCtrl0(DSI_Device_IDType   eDeviceId,
                                   uint32              uFlags )
{
   uint32   uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId);

   if (uFlags == HAL_DSI_PHY_SETPHYCTRL0_STEP1)
   {
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_CTRL_0_ADDR, 0x00);
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_CTRL_0_ADDR, 
                         (HWIO_SET_FLD(DSI_0_PHY_DSIPHY_CTRL_0, DSIPHY_DIGTOP_PWRDN_B, 1) |
                          HWIO_SET_FLD(DSI_0_PHY_DSIPHY_CTRL_0, DSIPHY_DLN3_SHUTDOWNB, 1) |
                          HWIO_SET_FLD(DSI_0_PHY_DSIPHY_CTRL_0, DSIPHY_DLN2_SHUTDOWNB, 1) |
                          HWIO_SET_FLD(DSI_0_PHY_DSIPHY_CTRL_0, DSIPHY_DLN1_SHUTDOWNB, 1) |
                          HWIO_SET_FLD(DSI_0_PHY_DSIPHY_CTRL_0, DSIPHY_DLN0_SHUTDOWNB, 1) ));
   }
   else if (uFlags == HAL_DSI_PHY_SETPHYCTRL0_STEP2)
   {
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_CTRL_0_ADDR, 
                         (HWIO_SET_FLD(DSI_0_PHY_DSIPHY_CTRL_0, DSIPHY_DIGTOP_PWRDN_B, 1) |
                          HWIO_SET_FLD(DSI_0_PHY_DSIPHY_CTRL_0, DSIPHY_DLN3_SHUTDOWNB, 1) |
                          HWIO_SET_FLD(DSI_0_PHY_DSIPHY_CTRL_0, DSIPHY_DLN2_SHUTDOWNB, 1) |
                          HWIO_SET_FLD(DSI_0_PHY_DSIPHY_CTRL_0, DSIPHY_CLK_SHUTDOWNB,  1) |
                          HWIO_SET_FLD(DSI_0_PHY_DSIPHY_CTRL_0, DSIPHY_DLN1_SHUTDOWNB, 1) |
                          HWIO_SET_FLD(DSI_0_PHY_DSIPHY_CTRL_0, DSIPHY_DLN0_SHUTDOWNB, 1) ));
   }

   return;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_PllSwReset()
*/
/*!
* \brief
*     SW reset the DSI PLL.
*
*
* \param [in]   eDeviceId   - DSI core ID
*
* \retval none
*
****************************************************************************/
static void HAL_DSI_Phy_1_5_PllSwReset(DSI_Device_IDType   eDeviceId)
{
   uint32   uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId);

   /* assert PLL SW Reset */ 
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_TEST_CFG_ADDR, 
             HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_TEST_CFG, PLL_SW_RESET, 1) );         
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_TEST_CFG_ADDR, 
             HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_TEST_CFG, PLL_SW_RESET, 1) );

   /* delay ~1ms */
   DSI_OSAL_SleepUs(1000);
   
   /* release PLL SW Reset */
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_TEST_CFG_ADDR, 
             HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_TEST_CFG, PLL_SW_RESET, 0) );         
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_TEST_CFG_ADDR, 
             HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_TEST_CFG, PLL_SW_RESET, 0) );
   
   /* delay ~1ms */
   DSI_OSAL_SleepUs(1000); 
}

/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_Pll_1_6_2_PowerCtrl()
*/
/*!
* \brief
*      DSI Phy PLL power up/down sequence for DSI phy 1.5.x versions, PLL version 1.6.2.3 
*     PLL version 1.6.2.3 is used in 8916,8x26,
*
*
* \param [in]  eDeviceId - DSI core ID
* \param [in]  bPllPowerUp  - TRUE: power up, FALSE: power down
*
* \retval none
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_Phy_1_5_Pll_1_6_2_PowerCtrl(DSI_Device_IDType   eDeviceId,
                                                       bool32              bPllPowerUp)
{
   HAL_MDSS_ErrorType   eStatus         = HAL_MDSS_STATUS_SUCCESS;
   uint32               uOffset         = HAL_DSI_GetRegBaseOffset(eDeviceId);
   bool32               uIsPLLLocked    = FALSE;
   uint32               uRetrySequenceM = 2;
   uint32               uRetrySequenceD = 2;
   
   if (TRUE == bPllPowerUp)
   {
      /* Clearing the PLL */
      HAL_DSI_Phy_1_5_Clear_Pll_Ready(eDeviceId);
      
      /* assert/deassert DSI PLL SW Reset */
      HAL_DSI_Phy_1_5_PllSwReset(eDeviceId);

      /* For DSI phy 1.5.x , program the PLL using MMDDF1CE sequence. */
      while ((FALSE == uIsPLLLocked) && uRetrySequenceM--)
      {
        out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_CAL_CFG1_ADDR,0x34);
  
        /* Power-up PLL */
        out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                  HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 0 ) |
                  HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 0) |
                  HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 0) |
                  HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 0) |
                  HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1) );
  
        /* delay ~200us */
        DSI_OSAL_SleepUs(200);
  
        /* Power-up powergen */
        out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                  HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 0 ) |
                  HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 0) |
                  HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 1) |
                  HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 0) |
                  HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1) );
  
        /* delay ~200us */
        DSI_OSAL_SleepUs(200);
  
        /* Enable PLL and power-up LDO*/
        out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                  HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 1 ) |
                  HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 1) |
                  HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 1) |
                  HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 1) |
                  HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1) );
        
        /* delay ~600us */
        DSI_OSAL_SleepUs(600);

        if (HAL_MDSS_STATUS_SUCCESS != HAL_DSI_Phy_1_5_PllLockDetect(eDeviceId))
        {
          uint32 i;

          for (i = 0; (i < 7) && (FALSE == uIsPLLLocked); i++)
          {
            out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_PWRGEN_CFG_ADDR, 0x0);

            /* delay ~50us */
            DSI_OSAL_SleepUs(50);

            /* Power-up PLL and powergen */
            out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                      HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 0 ) |
                      HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 0) |
                      HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 1) |
                      HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 0) |
                      HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1) );

            /* delay ~100us */
            DSI_OSAL_SleepUs(100);

            /* Enable PLL and power-up LDO*/
            out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                      HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 1 ) |
                      HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 1) |
                      HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 1) |
                      HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 1) |
                      HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1) );
            
            /* delay ~600us */
            DSI_OSAL_SleepUs(600);

            if(HAL_MDSS_STATUS_SUCCESS == HAL_DSI_Phy_1_5_PllLockDetect(eDeviceId))
            {
              uIsPLLLocked = TRUE;
            }
          }
        }
        else
        {
          uIsPLLLocked = TRUE;
        }
      }

      /* If PLL not locked, use sequence D */
      while ((FALSE == uIsPLLLocked) && uRetrySequenceD--)
      {
         /* assert/deassert DSI PLL SW Reset */
         HAL_DSI_Phy_1_5_PllSwReset(eDeviceId);

         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_PWRGEN_CFG_ADDR, 0x0);

         /* delay ~50us */
         DSI_OSAL_SleepUs(50);

         /* power-up PLL */
         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 0 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1) );

         /* delay ~200us */
         DSI_OSAL_SleepUs(200);

         /*  power-up powergen */
         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 0 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 1) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1) );
         
         /* delay ~200us */
         DSI_OSAL_SleepUs(200);
         
         /*   power-up LDO */
         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 0 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 1) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 1) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1) );
         
         /* delay 200us */
         DSI_OSAL_SleepUs(200);
         
         /*  power-down LDO */
         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 0 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 1) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1) );
         
         /* delay ~200us */
         DSI_OSAL_SleepUs(200);
         
         /*   power-up LDO */
         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 0 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 1) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 1) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1) );
         
         /* delay ~200us */
         DSI_OSAL_SleepUs(200);
         
         /* Enable PLL */
         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 1 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 1) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 1) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 1) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1) );
         
         /* delay ~600us */
         DSI_OSAL_SleepUs(600);
         
         /* Poll if PLL locked */          
         if (HAL_MDSS_STATUS_SUCCESS == HAL_DSI_Phy_1_5_PllLockDetect(eDeviceId))
         {
            uIsPLLLocked = TRUE;
         }
      }

      /* If PLL is not locked, use sequence F1*/
      if (FALSE == uIsPLLLocked)
      {
         /* assert/deassert DSI PLL SW Reset */
         HAL_DSI_Phy_1_5_PllSwReset(eDeviceId);

         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_PWRGEN_CFG_ADDR, 0x0);

         /* delay ~50us */
         DSI_OSAL_SleepUs(50);

         /* power-up PLL */
         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 0 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1) );
         
         /* delay ~200us */
         DSI_OSAL_SleepUs(200);
         
         /* Power-up powergen */
         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 0 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 1) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1) );
         
         /* delay ~200us */
         DSI_OSAL_SleepUs(200);
         
         /* Power-up LDO and enable PLL */ 
         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 1 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 1 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 1 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 1 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1 ) );
         
         /* delay ~200us */
         DSI_OSAL_SleepUs(200);
         
         /* Power down LDO */          
         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 1 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 1 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 1 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 0 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1 ) );
         
         /* delay ~200us */
         DSI_OSAL_SleepUs(200);
         
         /* Power-up LDO */   
         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 1 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 1 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 1 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 1 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1 ) );
         
         /* delay ~600us */
         DSI_OSAL_SleepUs(600);
         
         /* Poll if PLL locked */ 
         if (HAL_MDSS_STATUS_SUCCESS == HAL_DSI_Phy_1_5_PllLockDetect(eDeviceId))
         {
            uIsPLLLocked = TRUE;
         }
      }
      
      /* If PLL is not locked, use sequence C*/
      if (FALSE == uIsPLLLocked)
      {
         /* assert/deassert DSI PLL SW Reset */
         HAL_DSI_Phy_1_5_PllSwReset(eDeviceId);

         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_PWRGEN_CFG_ADDR, 0x0);

         /* delay ~50us */
         DSI_OSAL_SleepUs(50);

         /* power-up PLL */
         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 0 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1) );

         /* delay ~200us */
         DSI_OSAL_SleepUs(200);

         /*  power-up powergen */  
         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 0 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 1) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1) );

         /* delay ~200us */
         DSI_OSAL_SleepUs(200);

         /* power-up LDO and enable PLL*/
         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 1 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 1) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 1) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 1) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1) );
         
         /* delay ~600ms */
         DSI_OSAL_SleepUs(600);
         
         /* Poll if PLL locked */
         if (HAL_MDSS_STATUS_SUCCESS == HAL_DSI_Phy_1_5_PllLockDetect(eDeviceId))
         {
            uIsPLLLocked = TRUE;
         }     
      }
      
      /* If PLL is not locked, use sequence E*/
      if (FALSE == uIsPLLLocked)
      {
         /* assert/deassert DSI PLL SW Reset */
         HAL_DSI_Phy_1_5_PllSwReset(eDeviceId);

         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_PWRGEN_CFG_ADDR, 0x0);

         /* delay ~50us */
         DSI_OSAL_SleepUs(50);

         /* power-up PLL */
         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 0 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1) );
         
         /* delay ~200us */
         DSI_OSAL_SleepUs(200);
         
         /*  power-up powergen */          
         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 0 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 1) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1) );
         
         /* delay ~200us */
         DSI_OSAL_SleepUs(200);
         
         /*  Enable PLL */         
         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 0 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 1) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 1) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 0) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1) );
         
         /* delay ~1us */
         DSI_OSAL_SleepUs(1);
         
         /*  Power-up LDO */           
         out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_GLB_CFG, 1 ) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_ENABLE, 1) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRGEN_PWRDN_B, 1) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_LDO_PWRDN_B, 1) |
                   HWIO_SET_FLD(DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG, PLL_PWRDN_B, 1) );
         
         /* delay ~600us */
         DSI_OSAL_SleepUs(600);
         
         /* again, poll if PLL locked */
         eStatus = HAL_DSI_Phy_1_5_PllLockDetect(eDeviceId);
      }
   }
   else
   {
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_PLL_UNIPHY_PLL_GLB_CFG_ADDR, 0);
   }
   
   return eStatus;
}


/* -----------------------------------------------------------------------
** Public functions
** ----------------------------------------------------------------------- */

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
void HAL_DSI_Phy_1_5_Disable( DSI_Device_IDType   eDeviceId )
{
   uint32                  uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId);
   uint32                  uRegVal;
     
   /* Steps of enabling process are distributed in function HAL_DSI_PhySetup();
   *enabling is never called alone.
   */
   /* turn off PLL before other DSI registers to ensure the controller is powered off. */
   HAL_DSI_Phy_1_5_Pll_1_6_2_PowerCtrl(eDeviceId, FALSE);
   
   /* power down DSI calibration */
   uRegVal = in_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_CTRL_0_ADDR);
   uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_CTRL_0, DSIPHY_CAL_PWRDN_B, 0);
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_CTRL_0_ADDR, uRegVal);
   
   /* power down all DSI blocks */
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_CTRL_0_ADDR, 0);
   
   /* power down regularor */
   uRegVal = in_dword(uOffset + HWIO_MMSS_DSI_0_PHY_REG_DSIPHY_REGULATOR_CAL_PWR_CFG_ADDR);
   uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_REG_DSIPHY_REGULATOR_CAL_PWR_CFG, DSIPHY_REG_PWRDN_B, 0);
   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_REG_DSIPHY_REGULATOR_CAL_PWR_CFG_ADDR, uRegVal);

   return;
}

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
                                                HAL_DSI_PhyConfigType     *psDsiPhyConfig)
{
  HAL_MDSS_ErrorType        eStatus = HAL_MDSS_STATUS_SUCCESS;

  /* validate input parameters */
  if ((eDeviceId != DSI_DeviceID_0) && 
      (eDeviceId != DSI_DeviceID_1))
  {
    //DEBUG((EFI_D_ERROR, "Invalid DSI device ID: %d !\n", eDeviceId));
    eStatus = HAL_MDSS_DSI_FAILED_INVALID_INPUT_PARAMETER;
  }
  else
  {
    HAL_DSI_Phy_1_5_ResetPhy(eDeviceId);
    
    HAL_DSI_Phy_1_5_SetPhyCtrl0(eDeviceId, HAL_DSI_PHY_SETPHYCTRL0_STEP1);
    
    HAL_DSI_Phy_1_5_SetStrengthCtrl0(eDeviceId, psDsiPhyConfig->uDataStrengthLP);
    
    if (psDsiPhyConfig->bDCDCMode)
    {
      /* enable DCDC regulator */
      HAL_DSI_Phy_1_5_EnableDcdcRegulator(eDeviceId);
    }
    else
    {
      /* enable LDO regulator */
      HAL_DSI_Phy_1_5_EnableLdoRegulator(eDeviceId);
    }
    
    MDP_OSAL_DELAYUS(1000);   /* delay ~1ms */    

    HAL_DSI_Phy_1_5_SetStrengthCtrl1(eDeviceId);

    /* set analog lane config register */
    HAL_DSI_Phy_1_5_SetAnaLaneConfig(eDeviceId, psDsiPhyConfig->uClockStrengthHS, psDsiPhyConfig->uDataStrengthHS);

    HAL_DSI_Phy_1_5_SetGlobalTestCtrl(eDeviceId);
    
    HAL_DSI_Phy_1_5_SetPhyCtrl0(eDeviceId, HAL_DSI_PHY_SETPHYCTRL0_STEP2);

    HAL_DSI_Phy_1_5_SetRegBistOff(eDeviceId);
  }
  
  return eStatus;
}

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
HAL_MDSS_ErrorType HAL_DSI_Phy_1_5_PllSetup( HAL_DSI_PhyConfigType       *psDsiPhyConfig,
                                     HAL_DSI_PhyConfigInfoType   *psDsiPhyConfigInfo )
{
   HAL_MDSS_ErrorType        eStatus = HAL_MDSS_STATUS_SUCCESS;
   HAL_DSI_PllSettingType    sPllSettings;
   HAL_DSI_PllSettingType   *psPllSettings = &sPllSettings;

   /* validate input parameters */
   if ((psDsiPhyConfig->eDeviceId != DSI_DeviceID_0) && 
       (psDsiPhyConfig->eDeviceId != DSI_DeviceID_1))
   {
      eStatus = HAL_MDSS_DSI_FAILED_INVALID_INPUT_PARAMETER;
   }
   else if ((psDsiPhyConfig->uBitsPerPixel != 16) && 
            (psDsiPhyConfig->uBitsPerPixel != 18) && 
            (psDsiPhyConfig->uBitsPerPixel != 24))
   {
      /* unsupported pixel bit depth */
      eStatus = HAL_MDSS_DSI_FAILED_INVALID_INPUT_PARAMETER;
   }
   else if ((psDsiPhyConfig->uNumOfDataLanes == 0) ||
            (psDsiPhyConfig->uNumOfDataLanes > 4))
   {
      /* illegal number of DSI data lanes */
      eStatus = HAL_MDSS_DSI_FAILED_INVALID_INPUT_PARAMETER;
   }

   if (eStatus == HAL_MDSS_STATUS_SUCCESS)
   {
      /* find a PLL setting for the desired clock frequency */
      eStatus = HAL_DSI_Phy_1_5_FindPllSettings(psDsiPhyConfig->uDesiredBitClkFreq, psPllSettings);      
      if (eStatus == HAL_MDSS_STATUS_SUCCESS)
      {
         /* get PClk post divider value */
         eStatus = HAL_DSI_Phy_1_5_GetPClkPostDivider(psPllSettings, psDsiPhyConfigInfo, psDsiPhyConfig->uBitsPerPixel, psDsiPhyConfig->uNumOfDataLanes);
         if (eStatus == HAL_MDSS_STATUS_SUCCESS)
         {
            /* set up PLL */
            eStatus = HAL_DSI_Phy_1_5_SetupPll(psDsiPhyConfig->eDeviceId, psPllSettings);
         }
      }
   }
   
    return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_1_5_SetupTimingParams()
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
HAL_MDSS_ErrorType HAL_DSI_Phy_1_5_SetupTimingParams(HAL_DSI_TimingSettingType *pTimingParameters)
{
   HAL_MDSS_ErrorType eStatus   = HAL_MDSS_STATUS_SUCCESS;
   uint32 uOffset               = 0;
        
   int32  iMinVal               = 0;
   int32  iMaxVal               = 0;
   float  fMidVal               = 0.0f;
   
   uint32 uRegVal               = 0;
   float  fRangeFactor          = 0.0;
   float  fEscclk               = (pTimingParameters->uEscapeFreq<=1000000)?(HAL_DSI_ESCCLK_SRC*1000000):(pTimingParameters->uEscapeFreq);  /* in Hz */
   float  fTlpx                 = HAL_DSI_PHY_TIPX_NUMERATOR / fEscclk;
   float  fUiBitclk             = HAL_DSI_PHY_TIPX_NUMERATOR / (float)pTimingParameters->uBitclock;
 
   float  fT_clk_prepare_actual = 0.0f;
   float  fT_clk_zero_actual    = 0.0f;
   float  fT_hs_prepare_actual  = 0.0f;
   float  fT_hs_exit_actual     = 0.0f;
 
   float  fT_clk_zero_min       = 0.0f;
   float  fT_hs_prepare_min     = 0.0f;
   float  fT_hs_prepare_max     = 0.0f;
   float  fT_hs_zero_min        = 0.0f;
   float  fT_clk_post_min       = 0.0f;
   float  fT_clk_pre_min        = 0.0f;
 
   uint32 uT_clk_prepare        = 0;
   uint32 uT_clk_zero           = 0;
   uint32 uT_clk_zero_msb       = 0;
   uint32 uT_hs_prepare         = 0;
   uint32 uT_hs_zero            = 0;
   uint32 uT_hs_trail           = 0;
   uint32 uT_hs_request         = 0;
   uint32 uT_hs_exit            = 0;
   uint32 uT_clk_post           = 0;
   uint32 uT_clk_pre            = 0;

   DSI_TimingOverrideType *pPreDefinedTimings = pTimingParameters->pPreDefinedTimings;
   uOffset = HAL_DSI_GetRegBaseOffset(pTimingParameters->eDeviceId);
     
   //uT_clk_prepare calculation
   iMinVal = HAL_DSI_Phy_1_5_Roundup(HAL_DSI_PHY_TCLK_PREPARE_PHY_MIN / fUiBitclk) - HAL_DSI_PHY_GENERIC_SHIFT_OFFSET;
   iMaxVal = HAL_DSI_Phy_1_5_Roundup(HAL_DSI_PHY_TCLK_PREPARE_PHY_MAX / fUiBitclk) - HAL_DSI_PHY_GENERIC_SHIFT_OFFSET;
   fRangeFactor =  (pTimingParameters->uBitclock > HAL_DSI_PHY_BITCLK_RANGE_FREQ_2) ? HAL_DSI_PHY_RANGEFACTOR_90 : 
                  ((pTimingParameters->uBitclock > HAL_DSI_PHY_BITCLK_RANGE_FREQ_1) ? HAL_DSI_PHY_RANGEFACTOR_50 : HAL_DSI_PHY_RANGEFACTOR_5);
   fMidVal = (iMaxVal - iMinVal) * (fRangeFactor / HAL_DSI_PHY_PERCENT_DENOMENATOR) + iMinVal;
   uT_clk_prepare = HAL_DSI_Phy_1_5_Roundup(fMidVal);
   if (uT_clk_prepare % 2)
   {
      uT_clk_prepare -= HAL_DSI_PHY_GENERIC_SHIFT_OFFSET;
   }   
   fT_clk_prepare_actual = ((uT_clk_prepare>>1) + 1) * 2 * fUiBitclk;

   if (pPreDefinedTimings->bTimingCLKPrepareOverride)
   {
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_2_ADDR, pPreDefinedTimings->uTimingCLKPrepareValue);
   }
   else
   {
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_2_ADDR, uT_clk_prepare);
   }
   
   //T_HS_RQST is user entry based on UI/ESCCLK
   uT_hs_request = (uint32)(fTlpx / fUiBitclk);
   if (!(uT_hs_request % 2))
   {
      uT_hs_request -= HAL_DSI_PHY_GENERIC_SHIFT_OFFSET;
   }
   if (pPreDefinedTimings->bTimingHSRequestOverride)
   {
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_8_ADDR, pPreDefinedTimings->uTimingHSRequestValue);
   }
   else
   {
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_8_ADDR, uT_hs_request);
   }
   
   //T_CLK_ZERO calculation
   fT_clk_zero_min = HAL_DSI_PHY_TCLK_ZERO_PARAM1 - fT_clk_prepare_actual;   
   iMinVal = HAL_DSI_Phy_1_5_Roundup(fT_clk_zero_min / fUiBitclk) - HAL_DSI_PHY_GENERIC_SHIFT_OFFSET;
   if (HAL_DSI_PHY_GENERIC_TIMING_MAX < iMinVal)
   {
      iMaxVal = HAL_DSI_PHY_GENERIC_TIMING_MAX * 2 + 1;
      uT_clk_zero_msb = 1;
   }
   else
   {  
      iMaxVal = HAL_DSI_PHY_GENERIC_TIMING_MAX;
      uT_clk_zero_msb = 0;
   }
   fRangeFactor = HAL_DSI_PHY_RANGEFACTOR_5;
   fMidVal = (iMaxVal == HAL_DSI_PHY_GENERIC_TIMING_MAX) ? ((iMaxVal - iMinVal)*(fRangeFactor/HAL_DSI_PHY_PERCENT_DENOMENATOR) + iMinVal) :
                                                           (iMinVal * (fRangeFactor/HAL_DSI_PHY_PERCENT_DENOMENATOR) + iMinVal);
   uT_clk_zero = HAL_DSI_Phy_1_5_Roundup(fMidVal);
   if (uT_clk_zero % 2)
   {
      uT_clk_zero -= HAL_DSI_PHY_GENERIC_SHIFT_OFFSET;
   }
   
   /*** This is SW workaround needed for 8974V1 ***/
   while ((uT_clk_prepare + uT_hs_request + uT_clk_zero) % 8)
   {
      uT_clk_zero--;
   }
   /*** End of the SW workaround ***/

   fT_clk_zero_actual = ((uT_clk_zero>>1) + 1) * 2 * fUiBitclk;
   if (HAL_DSI_PHY_GENERIC_TIMING_MAX != iMaxVal)
   {
      uT_clk_zero -= HAL_DSI_PHY_GENERIC_TIMING_MAX;
   }
    
   if (pPreDefinedTimings->bTimingCLKZeroOverride)
   {
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_0_ADDR, (pPreDefinedTimings->uTimingCLKZeroValue & 0xFF));      /* bit 0-7 */
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_3_ADDR, (pPreDefinedTimings->uTimingCLKZeroValue & 0x100)>>8);  /* bit 8   */
   }
   else
   {
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_0_ADDR, uT_clk_zero);
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_3_ADDR, uT_clk_zero_msb);
   }
   
   //T_CLK_TRAIL calculation
   if (pPreDefinedTimings->bTimingCLKTrailOverride)
   {
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_1_ADDR, pPreDefinedTimings->uTimingCLKTrailValue);
   }
   else
   {
      uint32 uT_clk_trail = 0; 
      iMinVal = HAL_DSI_Phy_1_5_Roundup(HAL_DSI_PHY_TCLK_TRAIL_MIN / fUiBitclk) - HAL_DSI_PHY_GENERIC_SHIFT_OFFSET;
      iMaxVal = HAL_DSI_Phy_1_5_Roundup((HAL_DSI_PHY_TEOT_PARAM1 + HAL_DSI_PHY_TEOT_PARAM2*fUiBitclk - HAL_DSI_PHY_TREOT) / fUiBitclk) - HAL_DSI_PHY_GENERIC_SHIFT_OFFSET;
      fRangeFactor = HAL_DSI_PHY_RANGEFACTOR_50;
      fMidVal = (iMaxVal - iMinVal) * (fRangeFactor / HAL_DSI_PHY_PERCENT_DENOMENATOR) + iMinVal;
      uT_clk_trail = HAL_DSI_Phy_1_5_Roundup(fMidVal);
      if (uT_clk_trail % 2)
      {
         uT_clk_trail -= HAL_DSI_PHY_GENERIC_SHIFT_OFFSET;
      }
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_1_ADDR, uT_clk_trail);
   }
   
   //T_HS_PREPARE calculation
   fT_hs_prepare_min = HAL_DSI_PHY_T_HS_PREPARE_PARAM1 + (HAL_DSI_PHY_T_HS_PREPARE_PARAM2 * fUiBitclk);
   fT_hs_prepare_max = HAL_DSI_PHY_T_HS_PREPARE_PARAM3 + (HAL_DSI_PHY_T_HS_PREPARE_PARAM4 * fUiBitclk);
   iMinVal = HAL_DSI_Phy_1_5_Roundup(fT_hs_prepare_min / fUiBitclk) - 2;
   iMaxVal = HAL_DSI_Phy_1_5_Roundup(fT_hs_prepare_max / fUiBitclk) - 2;
   fRangeFactor = (pTimingParameters->uBitclock > HAL_DSI_PHY_BITCLK_RANGE_FREQ_2) ? HAL_DSI_PHY_RANGEFACTOR_90 : HAL_DSI_PHY_RANGEFACTOR_50;
   fMidVal = (iMaxVal - iMinVal) * (fRangeFactor / HAL_DSI_PHY_PERCENT_DENOMENATOR) + iMinVal;
   uT_hs_prepare = HAL_DSI_Phy_1_5_Roundup(fMidVal);
   if (uT_hs_prepare % 2)
   {
      uT_hs_prepare -= HAL_DSI_PHY_GENERIC_SHIFT_OFFSET;
   }   
   fT_hs_prepare_actual = ((uT_hs_prepare>>1) + 1) * 2 * fUiBitclk;

   if (pPreDefinedTimings->bTimingHSPrepareOverride)
   {
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_6_ADDR, pPreDefinedTimings->uTimingHSPrepareValue);
   }
   else
   {
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_6_ADDR, uT_hs_prepare);
   }
      
   //T_HS_ZERO calculation
   if (pPreDefinedTimings->bTimingHSZeroOverride)
   {
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_5_ADDR, pPreDefinedTimings->uTimingHSZeroValue);
   }
   else
   {
      fT_hs_zero_min = HAL_DSI_PHY_T_HS_ZERO_PARAM1 + (HAL_DSI_PHY_T_HS_ZERO_PARAM2 * fUiBitclk) - fT_hs_prepare_actual;
      iMinVal = HAL_DSI_Phy_1_5_Roundup(fT_hs_zero_min / fUiBitclk) - HAL_DSI_PHY_GENERIC_SHIFT_OFFSET;
      iMaxVal = HAL_DSI_PHY_GENERIC_TIMING_MAX;
      fRangeFactor = HAL_DSI_PHY_RANGEFACTOR_5;
      fMidVal = (iMaxVal - iMinVal) * (fRangeFactor / HAL_DSI_PHY_PERCENT_DENOMENATOR) + iMinVal;
      uT_hs_zero = HAL_DSI_Phy_1_5_Roundup(fMidVal);
      if (uT_hs_zero % 2)
      {
         uT_hs_zero -= HAL_DSI_PHY_GENERIC_SHIFT_OFFSET;
      }
      if (HAL_DSI_PHY_T_HS_ZERO_PARAM3 > uT_hs_zero)
      {
         uT_hs_zero = HAL_DSI_PHY_T_HS_ZERO_PARAM3;
      }
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_5_ADDR, uT_hs_zero);
   }
   
   //T_HS_TRAIL calculation
   if (pPreDefinedTimings->bTimingHSTrailOverride)
   {
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_7_ADDR, (uint32)pPreDefinedTimings->uTimingHSTrailValue);
   }
   else
   {
      float  fT_hs_trail_min       = 0;

      fT_hs_trail_min = HAL_DSI_PHY_T_HS_TRAIL_PARAM1 + (HAL_DSI_PHY_T_HS_TRAIL_PARAM2 * fUiBitclk);   
      iMinVal = HAL_DSI_Phy_1_5_Roundup(fT_hs_trail_min / fUiBitclk) - HAL_DSI_PHY_GENERIC_SHIFT_OFFSET;
      iMaxVal = HAL_DSI_Phy_1_5_Roundup((HAL_DSI_PHY_TEOT_PARAM1 + HAL_DSI_PHY_TEOT_PARAM2*fUiBitclk - HAL_DSI_PHY_TREOT) / fUiBitclk) - HAL_DSI_PHY_GENERIC_SHIFT_OFFSET;
      fRangeFactor = HAL_DSI_PHY_RANGEFACTOR_50;
      fMidVal = (iMaxVal - iMinVal) * (fRangeFactor / HAL_DSI_PHY_PERCENT_DENOMENATOR) + iMinVal;
      uT_hs_trail = HAL_DSI_Phy_1_5_Roundup(fMidVal);
      if (uT_hs_trail % 2)
      {
         uT_hs_trail -= HAL_DSI_PHY_GENERIC_SHIFT_OFFSET;
      }

      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_7_ADDR, uT_hs_trail);
   }
 
   //T_HS_EXIT calculation
   if (pPreDefinedTimings->bTimingHSExitOverride)
   {
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_4_ADDR, pPreDefinedTimings->uTimingHSExitValue);
   }
   else
   {
      iMinVal = HAL_DSI_Phy_1_5_Roundup(HAL_DSI_PHY_T_HS_EXIT_MIN / fUiBitclk) - HAL_DSI_PHY_GENERIC_SHIFT_OFFSET;
      iMaxVal = HAL_DSI_PHY_GENERIC_TIMING_MAX;
      fRangeFactor = HAL_DSI_PHY_RANGEFACTOR_5;
      fMidVal = (iMaxVal - iMinVal) * (fRangeFactor / HAL_DSI_PHY_PERCENT_DENOMENATOR) + iMinVal;
      uT_hs_exit        = HAL_DSI_Phy_1_5_Roundup(fMidVal);
      if (uT_hs_exit % 2)
      {
         uT_hs_exit -= HAL_DSI_PHY_GENERIC_SHIFT_OFFSET;
      }
      fT_hs_exit_actual = ((uT_hs_exit>>1) + 1) * 2 * fUiBitclk;

      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_4_ADDR, uT_hs_exit);
   }
 
   //T_TA_GO and T_TA_SURE value from VI
   {
      uint32 uTAGo = HAL_DSI_PHY_T_TA_GO_PARAM1;
      uint32 uTASure = HAL_DSI_PHY_T_TA_SURE_PARAM1;
      
      if (pPreDefinedTimings->bTimingTAGoOverride)
      {
         uTAGo = pPreDefinedTimings->uTimingTAGoValue;
      }
   
      if (pPreDefinedTimings->bTimingTASureOverride)
      {
         uTASure = pPreDefinedTimings->uTimingTASureValue;
      }

      uRegVal = HWIO_SET_FLD(DSI_0_PHY_DSIPHY_TIMING_CTRL_9, DSIPHY_T_TA_GO, uTAGo) | 
                HWIO_SET_FLD(DSI_0_PHY_DSIPHY_TIMING_CTRL_9, DSIPHY_T_TA_SURE, uTASure);

      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_9_ADDR, uRegVal);
   }
   
   //T_TA_GET calculation
   if (pPreDefinedTimings->bTimingTAGetOverride)
   {
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_10_ADDR, pPreDefinedTimings->uTimingTAGetValue);
   }
   else
   {
      uRegVal = HWIO_SET_FLD(DSI_0_PHY_DSIPHY_TIMING_CTRL_10, DSIPHY_T_TA_GET, HAL_DSI_PHY_T_TA_GET_PARAM1); 
      out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_10_ADDR, uRegVal);
   }
   
   //T_CLK_POST calculation
   if (pPreDefinedTimings->bTimingCLKPostOverride)
   {
      uT_clk_post = pPreDefinedTimings->uTimingCLKPostValue;
   }
   else
   {
      fT_clk_post_min = HAL_DSI_PHY_T_CLK_POST_PARAM1 + (HAL_DSI_PHY_T_CLK_POST_PARAM2 * fUiBitclk);
      iMinVal = HAL_DSI_Phy_1_5_Roundup((fT_clk_post_min - (HAL_DSI_PHY_T_CLK_POST_PARAM3 * fUiBitclk) - fT_hs_exit_actual) / 
                (HAL_DSI_PHY_T_CLK_POST_PARAM4 * fUiBitclk)) - HAL_DSI_PHY_T_CLK_POST_PARAM5;
      iMaxVal = HAL_DSI_PHY_T_CLK_POST_MAX;
      fRangeFactor = HAL_DSI_PHY_RANGEFACTOR_50;
      fMidVal = (iMaxVal - iMinVal) * (fRangeFactor / HAL_DSI_PHY_PERCENT_DENOMENATOR) + iMinVal;
      uT_clk_post = (fMidVal > 0) ? HAL_DSI_Phy_1_5_Roundup(fMidVal) : 0;
   }
   
   //T_CLK_PRE calculation
   if (pPreDefinedTimings->bTimingCLKPreOverride)
   {
      uT_clk_pre = pPreDefinedTimings->uTimingCLKPreValue;
   }
   else
   {
      fT_clk_pre_min = HAL_DSI_PHY_T_CLK_PRE_PARAM1 * fUiBitclk;
      iMinVal = HAL_DSI_Phy_1_5_Roundup((fT_clk_pre_min + fT_clk_prepare_actual + fT_clk_zero_actual + fTlpx) / (HAL_DSI_PHY_T_CLK_PRE_PARAM2 * fUiBitclk)) - HAL_DSI_PHY_T_CLK_PRE_PARAM3;
      iMaxVal = HAL_DSI_PHY_T_CLK_PRE_MAX;
      fRangeFactor = HAL_DSI_PHY_RANGEFACTOR_50;
      fMidVal = (HAL_DSI_PHY_T_CLK_PRE_MAX > iMinVal) ? 
                (iMaxVal - iMinVal) * (fRangeFactor / HAL_DSI_PHY_PERCENT_DENOMENATOR) + iMinVal : HAL_DSI_PHY_T_CLK_PRE_MAX;
      uT_clk_pre = HAL_DSI_Phy_1_5_Roundup(fMidVal);
   }
   
   uRegVal = HWIO_SET_FLD(DSI_0_CLKOUT_TIMING_CTRL, T_CLK_PRE, uT_clk_pre) |
             HWIO_SET_FLD(DSI_0_CLKOUT_TIMING_CTRL, T_CLK_POST, uT_clk_post) ; 
   out_dword(uOffset + HWIO_MMSS_DSI_0_CLKOUT_TIMING_CTRL_ADDR, uRegVal);

   out_dword(uOffset + HWIO_MMSS_DSI_0_PHY_DSIPHY_TIMING_CTRL_11_ADDR, HAL_DSI_PHY_TIMING_CTRL_11);

   return eStatus;
}

#ifdef __cplusplus
}
#endif
