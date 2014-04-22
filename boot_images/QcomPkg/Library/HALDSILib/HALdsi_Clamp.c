/*=============================================================================

  File: HALdsi_Clamp.c
  

     Copyright (c) 2010-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
     Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#include "HALdsi.h"
#include "mmss_misc_dsi.h"

/* --- DO NOT ADD HEADER FILES HERE! --- */

#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************
*
** FUNCTION: HAL_DSI_ULPSClampConfig()
*/
/*!
* \brief
*   This API configures the DSI ULPS clamp control so that DSI lanes can be driven using different source
*
* \param [in] eDeviceID        - DSI Core Device ID 
* \param [in] bEnable          - DSI ULPS clamp enable/disable
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_ULPSClampConfig(DSI_Device_IDType  eDeviceID,
                                      bool32             bEnable)
{
    uint32    uRegValue = in_dword(HWIO_MMSS_DSI_ULP_CLAMP_CTRL_ADDR);    

    /* DSI-0 */
  if(DSI_DeviceID_0 == eDeviceID)
  {
      /* DSI-0 ULPS clamp config enabled */
    if(TRUE == bEnable)
    {
        /* DSI-0 ULPS clamp config */
        uRegValue   |=    (HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_CLAMP_EN_BMSK           |
                   HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_CLKLN_EN_BMSK           |
                   HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_CLKLN_ULPS_REQUEST_BMSK |
                   HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN0_EN_BMSK            |
                   HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN0_ULPS_REQUEST_BMSK  |
                   HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN1_EN_BMSK            |
                   HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN1_ULPS_REQUEST_BMSK  |
                   HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN2_EN_BMSK            |
                   HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN2_ULPS_REQUEST_BMSK  |
                   HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN3_EN_BMSK            |
                   HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN3_ULPS_REQUEST_BMSK);
    }
      else /* DSI-0 ULPS clamp config disabled */
      {
        /* DSI-0 ULPS clamp config */
        uRegValue   &=   ~(HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_CLAMP_EN_BMSK           |
                           HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_CLKLN_EN_BMSK           |
                           HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_CLKLN_ULPS_REQUEST_BMSK |
                           HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN0_EN_BMSK            |
                           HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN0_ULPS_REQUEST_BMSK  |
                           HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN1_EN_BMSK            |
                           HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN1_ULPS_REQUEST_BMSK  |
                           HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN2_EN_BMSK            |
                           HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN2_ULPS_REQUEST_BMSK  |
                           HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN3_EN_BMSK            |
                           HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN3_ULPS_REQUEST_BMSK);

      }
  }
    /* DSI-1 */
  else if(DSI_DeviceID_1 == eDeviceID)
  {
      /* DSI-1 ULPS clamp config enabled */
    if(TRUE == bEnable)
    {
        /* DSI-1 ULPS clamp config */
        uRegValue   |=    (HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_CLAMP_EN_BMSK           |
                           HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_CLKLN_EN_BMSK           |
                           HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_CLKLN_ULPS_REQUEST_BMSK |
                           HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN0_EN_BMSK            |
                           HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN0_ULPS_REQUEST_BMSK  |
                           HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN1_EN_BMSK            |
                           HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN1_ULPS_REQUEST_BMSK  |
                           HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN2_EN_BMSK            |
                           HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN2_ULPS_REQUEST_BMSK  |
                           HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN3_EN_BMSK            |
                           HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN3_ULPS_REQUEST_BMSK);
      }
      else /* DSI-1 ULPS clamp config disabled */
      {
        /* DSI-1 ULPS clamp config */
        uRegValue   &=   ~(HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_CLAMP_EN_BMSK           |
                   HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_CLKLN_EN_BMSK           |
                   HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_CLKLN_ULPS_REQUEST_BMSK |
                   HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN0_EN_BMSK            |
                   HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN0_ULPS_REQUEST_BMSK  |
                   HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN1_EN_BMSK            |
                   HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN1_ULPS_REQUEST_BMSK  |
                   HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN2_EN_BMSK            |
                   HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN2_ULPS_REQUEST_BMSK  |
                   HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN3_EN_BMSK            |
                   HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN3_ULPS_REQUEST_BMSK);
    }
  }

  out_dword(HWIO_MMSS_DSI_ULP_CLAMP_CTRL_ADDR, uRegValue);
}


#ifdef __cplusplus
}
#endif
