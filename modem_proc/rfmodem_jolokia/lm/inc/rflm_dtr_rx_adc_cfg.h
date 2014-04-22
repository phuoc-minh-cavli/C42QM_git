#ifndef RFLM_DTR_RX_ADC_CFG_H
#define RFLM_DTR_RX_ADC_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================

  Copyright (c) 2012 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "rflm_features.h"
#if (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
#include "rflm_api_cmn.h"
#include "rflm_dtr.h"

#define MSS_TOP_REG_ATOP_REG1 0x537300B7
#define MSS_TOP_REG_ATOP_REG2 0x4FA1B07E
#define MSS_TOP_REG_ATOP_REG3 0x20059600
#define MSS_TOP_REG_DTOP_REG1 0x004EEC3B
#define MSS_TOP_REG_DTOP_REG2 0x00000014
#define DTOP_RESET 0x00000000
#define LDO_ENABLE 0x107000B7
#define CAL_REG_ATOP_REG1 0x10B0DDDF
#define CAL_REG_ATOP_REG2 0x4F800000
#define CAL_REG_ATOP_REG3 0x00019600
#define CAL_REG_DTOP_REG1 0x004E603B
#define CAL_REG_DTOP_REG2 0x0000001F
#define ENABLE_CAL_MODE_SHFT 0x10
#define START_CALIBRATION 0x004F603B
#define CAL_SUCCESSFUL 0x004E603B
#define CAL_FAILURE 0x004E607B
#define SHIFT_BIT_31 31
#define MASK_BIT_24 0x00FFFFFF
#define CAL_DONE_I_BIT 0x100
#define CAL_DONE_Q_BIT 0x1
#define CAL_ERROR_I_BIT 0x200
#define CAL_ERROR_Q_BIT 0x2
#define CAL_DONE_I 0x8
#define CAL_DONE_Q 0x0
#define CAL_ERROR_I 0x9
#define CAL_ERROR_Q 0x1
#define WAIT_TIME_1 1
#define WAIT_TIME_2 2
#define WAIT_TIME_10 10
#define WAIT_TIME_100 100
#define WAIT_TIME_300 300
#define MISSION_REG_ATOP_REG1_GNSS 0x1070FF97
#define MISSION_REG_ATOP_REG2_GNSS 0x4FA1B06C
#define MISSION_REG_ATOP_REG3_GNSS 0x00051600
#define MISSION_REG_DTOP_REG1_GNSS 0x004EEC3B
#define MISSION_REG_DTOP_REG2_GNSS 0x00000011
#define RCGR_UPDATE 0x0
#define SRC_DIVIDE 0x7
#define SRC_SELECT 0x1
#define TRIG_GEN_ROOT_EN 0X1
#define TRIG_GEN_UPDATE 0x1
#define SET_CLOCK_GENERATOR 0x0
#define RESET_CLOCK_GENERATOR 0x1
#define ENABLE_ADC_CLOCK 0x1
#define DISABLE_ADC_CLOCK 0x0
#define ENABLE_ADC 0x1
#define DISABLE_ADC 0x0
#define CLAMP_ATOP 0x1
#define DECLAMP_ATOP 0x0
#define CONNECT_RF 0x1
#define DISCONNECT_RF 0x0
#define CALIBRATION_SUCCESS_POLL 0x0101
#define CAL_DONE_SUCCESS 1
#define CAL_ERROR_NONE 0
#define MSB_CHECK_MASK 80000000
#define ADC_SETTLING_TIME_20 20
#define ADC_SETTLING_TIME_50 50
#define ADC_SETTLING_TIME_100 100
#define MSB_SHIFT 31
#define OFFSET_SHIFT 24
#define OFFSET_MASK 0XFFFFFFFF
#define ADC_TIMED_MASK 0x80FFFFFF 
#define REF_FACT_DOUBLER_ON_Q9 1350
#define REF_FACT_DOUBLER_OFF_Q9 574 
#define REF_FACT_DOUBLER_ON 200
#define REF_FACT_DOUBLER_OFF 470 

typedef struct{
	uint32 debug_clk_cfg;
	uint32 debug_dbg_ctl;
	uint32 debug_dac_clk;
	uint32 debug_atop1;
	uint32 debug_atop2;
	uint32 debug_atop3;
	uint32 debug_dtop1;
	uint32 debug_dtop2;	
    uint32 debug_dtop3;
	uint32 debug_dtop4;	
    uint32 debug_dtop5;
	uint32 debug_dtop6;	
    uint32 debug_dtop7;
	uint32 debug_dtop8;	
    uint32 debug_dtop9;
	uint32 debug_dtop10;	
    uint32 debug_dtop13;
	uint32 debug_iref;
	uint32 debug_cfg0;
	uint32 debug_cfg1;
	uint32 debug_cfg2;
	uint32 debug_cfg3;
	uint32 debug_calib;
	uint32 debug_rt;
	uint32 debug_calib_cmds;
	uint32 debug_start_trig;
	uint32 debug_start_val;
	uint32 debug_stop_trig;
	uint32 debug_stop_val;
	uint32 debug_adc_cfg_rcgr;
	uint32 debug_adc_cmd_rcgr;
	uint32 debug_adc_cbcr;
	uint32 debug_adc_bcr;
	uint32 debug_protect;
	uint32 debug_clamp;
	uint32 debug_adc_en;
	uint32 debug_digital_clamp;
	uint32 debug_en_clk;
    uint32 trig_act;
	uint32 reset_cfg;
    uint32 vpe_vu;
    uint32 mem_slp;
    uint32 ext_clk;
    boolean cal_status;
}rflm_adc_dac_whole_dump;




void  rflm_dtr_rx_adc_calib_setup(void);
rflm_err_t  rflm_dtr_rx_adc_calib_poll_check(void);
void rflm_dtr_rx_adc_config_clk_gen(void);
rflm_err_t rflm_dtr_rx_adc_mission_reg_set(rflm_handle_rx_t handle);
rflm_err_t  rflm_dtr_rx_adc_calib_mode(void);
rflm_err_t  rflm_dtr_rx_adc_activate(rflm_handle_rx_t handle , boolean imm ,uint32 time_ustmr);    
void rflm_dtr_rx_rcg_clk_gen(void);
rflm_err_t  rflm_dtr_rx_adc_deactivate(rflm_handle_rx_t handle , boolean imm ,uint32 time_ustmr);
rflm_err_t  rflm_dtr_rx_adc_hi_z(rflm_handle_rx_t handle , boolean imm ,uint32 time_ustmr);
rflm_err_t  rflm_dtr_rx_doubler(rflm_handle_rx_t handle , boolean imm ,uint32 time_ustmr,uint32 doubler_on);
void rflm_dtr_adc_dac_reg_dump(void);
extern void rflm_dtr_write_wbdc_val(int32 dc_mv_val_i ,int32 dc_mv_val_q ,uint32 ustmr_time , uint32 doubler_on);
extern void rflm_dtr_read_wbdc_val(int32* dc_val_i ,int32* dc_val_q , uint32 doubler_on);
extern void rflm_dtr_read_wbdc_val_gsm(int32 wbdc_val_i,int32 wbdc_val_q,int32* dc_val_i ,int32* dc_val_q , boolean doubler_on);
#ifdef FEATURE_MODEM_CRS_CAPTURE
rflm_err_t rflm_dtr_rx_notch_enable_imm(rflm_handle_rx_t handle,uint8 wb_idx,boolean is_enable);
#endif
#endif
#ifdef __cplusplus
}
#endif


#endif



