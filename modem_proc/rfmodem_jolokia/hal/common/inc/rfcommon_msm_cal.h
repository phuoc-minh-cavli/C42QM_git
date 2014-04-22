#ifndef RFCOMMON_MSM_CAL_H
#define RFCOMMON_MSM_CAL_H

/*!
  @file rfcommon_msm_cal.h

  @brief
  This file contains APIs to support any common calibration routines specific
  to the current MSM.
*/

/*============================================================================== 
   
  Copyright (c) 2011 - 2019 Qualcomm Technologies Incorporated. All Rights Reserved 
   
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
   
                        EDIT HISTORY FOR MODULE 
   
This section contains comments describing changes made to the module. 
Notice that changes are listed in reverse chronological order. 
 
$Header: //components/rel/rfmodem_jolokia.mpss/1.10/hal/common/inc/rfcommon_msm_cal.h#1 $ 
   
when       who     what, where, why 
--------   ---     ------------------------------------------------------------- 
03/05/19   sk      RF PAGE ONLY changes for NB/CATM
02/05/15   dr      MSB Cal Reprogramming.
04/21/14   dej     Fix Compiler Errors
02/26/14   dej     Fix linker error for DPM
11/27/13   ljl     DAC Cal result structure change for Bolt
08/22/13   dej     Remove references to FW TxLM files\structures 
01/15/13   aca     DIME DAC cal NV update
01/14/13   aca     DIME DAC cal mission mode config
01/10/12   aca     DIME Dac Nikel read method support
12/03/12   aca     DIME Dac cal
11/21/11   vb      Changes for DAC config based on the recommended Iref (which in 
                   turn is based on current device tx band port in use) 
11/02/11   vb      Support for performing DAC cal on dynamic number of Iref values
09/25/11   vb      Support for DAC cal using different Irefs
09/08/11   vb      Initial version
==============================================================================*/ 

#include "comdef.h"
#ifndef FEATURE_RF_LTE_PAGE_ONLY
#include "rfcom.h"
#include "txlm_intf.h"
#include "rfdevice_cmn_intf.h"
#include "fw_rf_common_intf.h" 
#include "rfcommon_msm.h"
#include "rflm_api_cmn.h"
/*Number of DACs supported for calibration*/
#define RFCOMMON_MAX_DAC_NUM             FW_RF_COMMON_NUM_TX_CHAINS

/*Results for each DAC*/
#define RFCOMMON_DAC_LUT_ENTRIES         FW_RF_COMMON_DAC_CAL_BUF_SIZE  

/* DAC cal results */
#define RFCOMMON_NUM_TXDAC_NUM_MSB_RESULTS	37 
/*----------------------------------------------------------------------------*/
typedef enum
{
	RFCOMMON_DAC_CAL_SAVE_NV,
		
	RFCOMMON_DAC_CAL_READ_NV,
	
	RFCOMMON_DAC_CAL_INVALID_CMD
	
}rfcommon_msm_dac_cal_data_cmd_enum_type;

/*----------------------------------------------------------------------------*/

typedef enum
{
	RFCOMMON_DAC_CAL_DAC0_IQ_MSB,
		
	RFCOMMON_DAC_CAL_DAC0_IQ_DC,
		
	RFCOMMON_DAC_CAL_DAC0_ET,

	RFCOMMON_DAC_CAL_DAC1_IQ_MSB,
		
	RFCOMMON_DAC_CAL_DAC1_IQ_DC,	
	
	RFCOMMON_DAC_CAL_DATA_ALL,
		
	RFCOMMON_DAC_CAL_INVALID
	
}rfcommon_msm_dac_cal_data_type;

typedef enum
{
  RFCOMMON_DAC_CAL_RD_DAC0_IQ_MSB,

  RFCOMMON_DAC_CAL_RD_DAC1_IQ_MSB,  

  RFCOMMON_DAC_CAL_RD_DATA_ALL,

  RFCOMMON_DAC_CAL_RD_INVALID

}rfcommon_msm_dac_cal_read_data_type;




typedef struct
{
  /* DAC cal results per chain 
     IQ DAC MSB cal - 37x24 reg array
     Each of the first 32 registers store 4 6-bit nibbles of the MSB CAL DATA in the 24 SBï¿½s.
     The LSBï¿½s of the COMBODAC_REGARRAY0: 0-5 have the MSB CAL data from internal memory addr 0 of the DAC and the bits 6-11 have data from internal memory addr 1 in the DAC
  */
   uint8  txdac_msb_result_valid;
   uint32 txdac_msb_results[RFCOMMON_NUM_TXDAC_NUM_MSB_RESULTS];
}rfcommon_msm_dac_cal_result_type;

typedef struct
{
  /* Flag to mention if the results from NV are valid  */
  boolean validity_flag;

  /*Holds the DAC cal results for both DACs- ET and Non ET results - hence 2*/
  rfcommon_msm_dac_cal_result_type dac_cal_results[TXLM_CHAIN_MAX];

} rfcommon_msm_cal_result_type;

/* DAC Cal data*/
extern rfcommon_msm_cal_result_type *rfcommon_msm_dac_cal_result;

/*----------------------------------------------------------------------------*/

boolean rfcommon_msm_dac_cal_nv_cfg
(
  rfcommon_msm_dac_cal_data_cmd_enum_type cmd,
  rfcommon_msm_dac_cal_read_data_type data_type,
  uint32* cal_results
);


/*--------------------------------------------------------------------------*/
boolean rfcommon_msm_dac_cfg_dynamic_update( txlm_chain_type txlm_chain,
                                             uint32 txlm_buf_idx,
                                             rfdevice_dac_iref_val iref_val );

/*--------------------------------------------------------------------------*/

boolean rfcommon_msm_cal_execute_dac_cal(rfcommon_msm_dac_cal_data_type dac_cal_type);

/*----------------------------------------------------------------------------*/
boolean rfcommon_msm_dac_cal_lut_dynamic_update( txlm_chain_type txlm_chain,
                                                 uint32 txlm_buf_idx,
                                                 boolean et_enabled );

/*----------------------------------------------------------------------------*/
#ifdef ENABLE_REF_DAC_CAL_MISSION_MODE_INIT
void rfcommon_msm_cal_dac_init(void);
#endif /* ENABLE_REF_DAC_CAL_MISSION_MODE_INIT */
boolean rfcommon_msm_cal_dac_chk_execute(rfcommon_msm_cal_result_type *nv_dac_cal,
                                                   rfcommon_msm_dac_cal_read_data_type 
                                                   dac_cal_read_type);

boolean rfcommon_msm_dac_cal_results_read(rfcommon_msm_cal_result_type *nv_dac_cal_results);
void rfcommon_msm_dac_bringup(rflm_handle_tx_t handle);
void rflm_tx_txdac_mission_mode(rflm_handle_tx_t handle,uint32* nv_dac_cal_results);
uint8 rfcommon_msm_get_dac_cal_command(void);
boolean rfcommon_msm_cal_extract_results_v2(txlm_chain_type chain,uint8 size,uint32* cal_data);
boolean rfcommon_msm_cal_reprogram_msbcal_codes(txlm_chain_type chain);
#endif /*FEATURE_RF_LTE_PAGE_ONLY*/

#endif/*RFCOMMON_MSM_CAL_H*/
