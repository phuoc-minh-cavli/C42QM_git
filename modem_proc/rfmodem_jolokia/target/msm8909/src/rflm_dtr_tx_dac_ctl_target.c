/*! 
  @file
  rflm_dtr_tx_dac_ctl.c
 
  @brief
  Implements the RFLM DTR tx Interface Functions to FW
 
*/

/*==============================================================================

  Copyright (c) 2012 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rfmodem_jolokia.mpss/1.10/target/msm8909/src/rflm_dtr_tx_dac_ctl_target.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
03/28/15   jhap      Disabling BBRX fuse solution
03/16/15   Jhap      Added proper mask for Jacala Hibernation mode
02/16/16   jhap      Added support for BBRx RSB fuse compensation
02/09/15   jhap   Updated Default fuse value for jacala
09/22/15   jhap     Adding Jacala support.
04/28/15   lm       Adding support for BBRX ADC hibernation and restore.
05/29/14   cvd      Initial version.
==============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "rflm.h"
#include "rflm_dtr.h"
#include "txlm_intf.h"
#include "rxlm_intf.h"
#include "rflm_dtr_tx_dac_ctl.h"
#include "rflm_dtr_tx_struct_ag.h"
#include "rflm_dtr_tx_typedef_ag.h"
#include "msm.h"
#include "rflm_dtr_rx_struct_ag.h"
#include "msmhwio.h"
#include "rflm_api_cmn.h"
#include "rflm_dm_api.h"

#include "rflm_hwintf.h"
#include "rflm_features.h"
#include "rfmodem_target_common.h"

#include "rflm_time.h"
#if (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
#include "DALSys.h"

/*For ADC DC cal*/
#include "rfcmd_msg.h"
#include "rflm_adc_dc_cal.h"
#include "rfcommon_efs.h"
#include "rfcommon_mdsp_cal.h"
#include "modem_mem.h"
#include "rflm_api_dtr_rx_fw.h"
#include "rfcommon_msg.h"
#include "rflm_dtr_rx_typedef_ag.h"

/*!For ADC VCM calibration*/
extern rfcommon_vcm_adc_cal_data_type   rfcommon_vcm_adc_cal_data;                                          /*used to store the firmware response data for ADC VCM calibration*/
#endif
/* 
const rflm_dtr_rx_adc_config_group_struct rflm_dtr_rx_adc_low_pwr_settings[RFLM_DTR_RX_LPS_LTE_MAX] =
{
  {
    0x00003800, 0x00000000, 0x00000000, 0x28300000, 0x00008000
  }, // RFLM_DTR_RX_LPS_LTE_5_10
  {
    0x00003800, 0x00000000, 0x00000000, 0x28300000, 0x00008000
  },
}; */



/*----------------------------------------------------------------------------*/
/*
  @brief
  New API for reading the foundary id.

  @details

 */

/* uint32 txlm_read_foundry_register()	
{
    uint32 foundry_id = 0;
 #ifdef HWIO_QFPROM_CORR_PTE2_IN
    foundry_id = (HWIO_IN(QFPROM_CORR_PTE2) & 0x700) >> 8;
 #endif
    return foundry_id;
} */

/*----------------------------------------------------------------------------*/
/*
  @brief
  New API Added for reading the qfuse bit status

  @details

 */
 
void rflm_dtr_tx_read_0_1_fuseflag_bit(uint32* fuseflag_bit)
{
#ifdef HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TXDAC_0_1_FLAG_BMSK
  *fuseflag_bit =  HWIO_INF(QFPROM_CORR_CALIB_ROW3_MSB, TXDAC_0_1_FLAG);
#endif
   //RF_MSG_1(RF_HIGH, "rflm_dtr_tx_read_0_1_fuseflag_bit(): Fuse Flag [%d] ",*fuseflag_bit);
}


/*----------------------------------------------------------------------------*/
/*
  @brief
  New API Added for reading the qfuse bit.

  @details

 */




void rflm_dtr_tx_read_qfuse(txlm_chain_type chain, uint32* qfuse_rd)
{
  if (TXLM_CHAIN_0 == chain)
   {
    #ifdef HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TXDAC0_CAL_7_4_BMSK
    *qfuse_rd = (((HWIO_INF(QFPROM_CORR_CALIB_ROW3_MSB, TXDAC0_CAL_7_4)<< 0x4)&0xF0)|( (HWIO_INF(QFPROM_CORR_CALIB_ROW3_LSB, TXDAC0_CAL_3_0) &0xF)));
     #endif
  }
  else
   {
    /* Invalid Chain Information: Returning default value */
    *qfuse_rd = 150;
   }
//  RF_MSG_1(RF_HIGH, "rflm_dtr_tx_read_qfuse(): Qfuse Value [%d] ",*qfuse_rd);
}




/*----------------------------------------------------------------------------*/
/*
  @brief
  New API Added for reading the average bit error.

  @details

 */
 
void rflm_dtr_tx_read_avg_error(txlm_chain_type chain, uint32* avg_error)
{
  if (TXLM_CHAIN_0 == chain)
  {
    #ifdef HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TXDAC0_CAL_AVG_ERR_BMSK
    *avg_error = HWIO_INF(QFPROM_CORR_CALIB_ROW3_MSB, TXDAC0_CAL_AVG_ERR);
    #endif
  }
/*
  else if (TXLM_CHAIN_1 == chain)  
  {
    #ifdef HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TXDAC1_CAL_AVG_ERR_BMSK
    *avg_error = HWIO_INF(QFPROM_CORR_CALIB_ROW3_MSB, TXDAC1_CAL_AVG_ERR);
    #endif
  }
*/
   //  RF_MSG_1(RF_HIGH, "rflm_dtr_tx_read_avg_error(): Average error [%d] ",*avg_error );
}
/*----------------------------------------------------------------------------*/
/*
  @brief
  New API Added for reading the overflow Bit.

  @details

 */

void rflm_dtr_tx_read_overflow_bit(txlm_chain_type chain, uint32* overflow_bit)
{
 
 if (TXLM_CHAIN_0 == chain)
  {
   #ifdef HWIO_QFPROM_CORR_CALIB_ROW3_MSB_TXDAC0_CAL_OVFLOW_BMSK
    *overflow_bit = HWIO_INF(QFPROM_CORR_CALIB_ROW3_MSB, TXDAC0_CAL_OVFLOW);
	   #endif
  }

  else
  {
    /* Invalid Chain Information: Returning default value */
    *overflow_bit = 0;
  }
// RF_MSG_1(RF_HIGH, "rflm_dtr_tx_read_overflow_bit(): Overflow bit [%d] ",*overflow_bit );
 

}



uint32 rfmodem_target_get_chipset_id(void)
{
   return (uint32)RFMODEM_TARGET_MSM8909;
}

rflm_dtr_tx_enum_chipset_id rfmodem_target_get_chipset_id_dac_cal(void) 
{ 
  return RFLM_DTR_TX_CHIPSET_ID_CHIPSET_CHILTEPIN; 
} 

boolean rfmodem_target_fuse_bbrx_compensation_needed(void)
{
  //BBRx fuse-based compensation is not required for Jacala
  return FALSE;
}

