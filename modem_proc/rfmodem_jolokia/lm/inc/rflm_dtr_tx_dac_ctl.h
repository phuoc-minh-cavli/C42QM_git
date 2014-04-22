#ifndef RFLM_DTR_TX_DAC_CTL_H
#define RFLM_DTR_TX_DAC_CTL_H

/*!
   @file
   txlm_hal.h

   @brief
   This file implements the TXLM MDM9K HAL functionality. These are platform
   dependent.

*/

/*===========================================================================

Copyright (c) 2010 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2020/04/29 00:10:25 $ $Author: mplcsds1 $
$Header: //components/rel/rfmodem_jolokia.mpss/1.10/lm/inc/rflm_dtr_tx_dac_ctl.h#2 $

when       who     what, where, why
-------------------------------------------------------------------------------
02/12/19   sk      DAC/ADC changes per VI script
02/05/15   dr      Adding MSB Cal Reprogramming.
01/08/14   kai     Update ET DAC config settings
12/20/13   cri     Changes for LTE digital modem settings
12/09/13   st      DAC Mission Mode: Restore TXDAC Mode for Companion
11/06/13   vrb     Fixes for compiler warnings
10/29/13   st      DAC Sleep Wakeup
10/29/13   st      Add IREF Update Latch for Bootup
10/29/13   st      Remove JTAG DBG DAC Disable
10/29/13   st      DAC Bootup Sequence
10/25/13   st      Fix Configuration codes
10/25/13   cvd     DAC IREF caliberation support 
10/16/13   st      Initial version.
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#ifdef _cplusplus
extern "C" {
#endif

#include "txlm_intf.h"
#include "rflm_dtr_tx_typedef_ag.h"

#define DAC_DEBUG_FOUNDRY_ID 0
#define FOUNDRY_GF 1
#define FOUNDRY_SMIC 5
#define FOUNDRY_ID_MASK 0x780
#define FOUNDRY_ID_SHFT 7
#define QFUSE_READY_ON 0x1
#define QFUSE_READY_OFF 0x0
#define DEBUG_Q_START_WR_ADDR 0x00200003
#define DEBUG_I_START_WR_ADDR 0x00280003
#define DEBUG_Q_START_RD_ADDR 0x00200007
#define DEBUG_I_START_RD_ADDR 0x00280007
#define DEBUG_BUS_INC 0x00001000
#define DEBUG_Q_SKIP_WR_ADDR 0x00209003
#define DEBUG_I_SKIP_WR_ADDR 0x00289003
#define DEBUG_WR_TOGGLE_ON 0xFFFFFFFF
#define DEBUG_WR_TOGGLE_OFF 0xFFFFFFFD
#define CAL_IQ_RANGE 45
#define CAL_IQ_TOTAL 88
#define DEBUG_MSBCAL_DISABLE 0x00200002
#define DEBUG_MSBCAL_ENABLE 0x00200003
#define DEBUG_MSBCAL_RD_ENABLE 0x00200007
#define DEBUG_DISABLE 0x00200000
#define DAC_PRECAL_TXDAC0 0x40850000
#define DAC_PRECAL_TXDAC1 0xC0018F0A
#define DAC_PRECAL_TXDAC2 0xF00082F9
#define DAC_PRECAL_TXDAC3 0x00102886
#define ALL_DONE_MASK 0x00000001
#define ALL_DONE_POLL 0x00000001
#define DAC_DEBUG_MSB_CAL 0
#define DAC_CAL_TXDAC_CONFIG2_DNL_c 0xF0008278
#define DAC_PWR_ON_CLK_DIV_PGM		0
#define DAC_PWR_ON_CLK_PLL_SEL		0
#define DAC_PWR_ON_CLK_DAC1_CLK_SRC	0
#define DAC_TX_XO_GATE_ENABLE 1
#define DAC_TX_XO_GATE_DISABLE 0
#define DAC_TX_FCAL_CLOCK_ENABLE 1
#define DAC_TX_FCAL_CLOCK_DISABLE 0
#define DAC_TX_XO_FCAL_ENABLE 1
#define DAC_TX_XO_FCAL_DISABLE 0
#define DAC_TX_XO_CLK_SEL_ENABLE 1
#define DAC_TX_XO_CLK_SEL_DISABLE 0
#define DAC_TX_RTCAL_CLOCK_ENABLE 1
#define DAC_TX_RTCAL_CLOCK_DISABLE 0
#define DAC_TX_CAL_MODE_ENABLE 1
#define DAC_TX_MISSION_MODE_ENABLE 2
#define DAC_TX_FORCE_TXDAC_ON 1
#define DAC_TX_FORCE_TXDAC_OFF 0
#define IREF_UPDATE_ON 0x1
#define IREF_UPDATE_OFF 0x0
#define DAC_HI_Z_TXDAC0 0x00850000
#define DAC_HI_Z_TXDAC1 0xC0018F0B
#define DAC_HI_Z_TXDAC2 0xF0008000
#define DAC_HI_Z_TXDAC3 0x00102882
#define DAC_MISSION_GSM_TXDAC2 0xF0008100
#define DAC_MISSION_MODE_TXDAC0 0x00850000
#define DAC_MISSION_MODE_TXDAC1 0xC0018F0B
#define DAC_MISSION_MODE_TXDAC2 0xF0008180 
#define DAC_MISSION_MODE_TXDAC3 0x00102882
#define DAC_SETTLING_TIME_10 10
#define DAC_SETTLING_TIME_50 50
#define DAC_LOWZ_MASK 0x180
#define DAC_LOWZ_MASK_GSM 0x100
#define DAC_HIZ_MASK 0xFFFFFE7F
#define DAC_MSBCAL_REGARRAY_MASK_0 	0x3F
#define DAC_MSBCAL_REGARRAY_MASK_1 	0xFC0
#define DAC_MSBCAL_REGARRAY_MASK_2 	0x3F000
#define DAC_MSBCAL_REGARRAY_MASK_3 	0xFC0000
#define DAC_MSBCAL_REGARRAY_MASK 0x3F

#define DAC_MSBCAL_REGARRAY_SHFT_0 0
#define DAC_MSBCAL_SHFT_0 3
#define DAC_MSBCAL_REGARRAY_SHFT_1 6
#define DAC_MSBCAL_REGARRAY_SHFT_2 12
#define DAC_MSBCAL_REGARRAY_SHFT_3 18
#define DAC_MSBCAL_10_MASK 0x3FFFF
#define DAC_MSBCAL_28_MASK 0xFC0FFF
#define DAC_MSBCAL_REG_MASK 0xFFFFFFFF

#define DAC_DCCAL_REGARRAY_MASK 0x7F
#define DAC_DCCAL_REGARRAY_MASK_0 	0x7F
#define DAC_DCCAL_REGARRAY_MASK_1 	0x3F80
#define DAC_DCCAL_REGARRAY_SHFT_0 0
#define DAC_DCCAL_REGARRAY_SHFT_1 7

#define TXDAC_MSBCAL_SIZE 148
#define TXDAC_DC_CAL_SIZE 2
#define TXDAC_CAL_DATA_SIZE (TXDAC_MSBCAL_SIZE+TXDAC_DC_CAL_SIZE)

#define DAC_FCAL_CLK_10_CYCLES							1
#define DAC_PWR_ON_TX_ALL_DONE_WAIT_US					20  /* Actual 20 usec - Safe Margin added */
#define TXDAC_PWR_ON_MSBCAL_MEM_WR_DONE_WAIT_US			2000	/* actual 568 XO Cycles - Safe Margin added */
#define ETDAC_PWR_ON_MSBCAL_MEM_WR_DONE_WAIT_US			100  /* Actual 284 XO Cycles - Safe Margin added */
#define TXDAC_PWR_ON_DCCAL_MEM_WR_DONE_WAIT_US			100	/* Actual 8 XO Cycles - Safe Margin added */
#define TXDAC_PWR_ON_MSBCAL_MEM_WR_WAIT_US              10
extern void rflm_dtr_tx_shared_reg_lock_init(void);
extern void rflm_dtr_tx_shared_reg_lock();
extern void rflm_dtr_tx_shared_reg_unlock();
void rflm_dtr_remove_hwdbg_flags(void);
void rflm_dtr_tx_dac_comp_mux_ctl(txlm_chain_type chain,uint32 cmd_ctl);
void rflm_dtr_tx_cal_protect(void);
void rflm_dtr_tx_xogate(uint32 tx_xo);
rflm_err_t rflm_dtr_tx_dac_power_up_clk_enable(rflm_handle_rx_t handle);
void rflm_dtr_tx_txdac_factory_cal_read_rpoly(txlm_chain_type chain, uint32* data);
void rflm_dtr_tx_txdac_reactivate(txlm_chain_type chain , uint32 activate_ustmr,boolean imm);
void rflm_dtr_tx_txdac_reactivate_gsm(txlm_chain_type chain, uint32 activate_ustmr,boolean imm);
void rflm_tx_dac_debug_bus_read();
void rflm_dtr_tx_txdac_power_down(txlm_chain_type chain ,uint32 deactivate_ustmr,boolean imm);
void rflm_tx_dac_debug_bus_wr(uint32* data);
void rflm_dtr_tx_fcal_clk(uint32 fcal, uint32 xo_fcal);
void rflm_dtr_tx_dac_clk_src_xo(uint32 src_xo);
void rflm_dtr_tx_txdac_rt_cal_clk(txlm_chain_type chain, uint32 rt_cal);
void rflm_dtr_tx_txdac_msbcal_set_cfg(txlm_chain_type chain);
void rflm_dtr_tx_txdac_pwr_on_restore_fusecode(txlm_chain_type chain, uint32 data);
void rflm_dtr_tx_txdac_set_mode(txlm_chain_type chain, uint32 mode, uint32 force_txdac);
void rflm_dtr_tx_txdac_pwr_on_pwr_on_reset(txlm_chain_type chain);
void rflm_dtr_tx_txdac_load_regarray(txlm_chain_type chain, uint8* data);
void rflm_dtr_tx_txdac_read_msbcal_from_regarray(txlm_chain_type chain,uint32 mask,uint8 size,uint32 * data);
void rflm_dtr_tx_txdac_read_dccal_from_regarray(txlm_chain_type chain,uint32 mask,uint32 * data);
void rflm_dtr_tx_etdac_read_msbcal_from_regarray(txlm_chain_type chain,uint32 mask,uint8 size,uint32 * data);
void rflm_dtr_tx_txdac_pwr_on_read_all_done(txlm_chain_type chain, uint32* data);
void rflm_dtr_tx_dac_factory_cal_msbcal_mem_rd(txlm_chain_type chain);
void rflm_dtr_tx_dac_factory_cal_msbcal_mem_rd_done(txlm_chain_type chain, uint32* data);
void rflm_dtr_tx_dac_pwr_on_msbcal_mem_wr(txlm_chain_type chain);
void rflm_dtr_tx_dac_pwr_on_msbcal_mem_wr_done(txlm_chain_type chain, uint32* data);
void rflm_dtr_tx_txdac_pwrdn(txlm_chain_type chain);
void rflm_dtr_tx_read_qfuse(txlm_chain_type chain, uint32* qfuse_rd);
void rflm_dtr_tx_read_overflow_bit(txlm_chain_type chain, uint32* overflow_bit);
void rflm_dtr_tx_read_0_1_fuseflag_bit(uint32* fuseflag_bit);
void rflm_dtr_tx_read_avg_error(txlm_chain_type chain, uint32* avg_error);
void rflm_dtr_tx_read_dac_status_tx_all_done(txlm_chain_type chain,uint32* data);
void rflm_dtr_tx_read_dac_status_msbcal_wr_done(txlm_chain_type chain,uint32* data);
void rflm_dtr_tx_read_dac_status_dccal_wr_done(txlm_chain_type chain,uint32* data);
void rflm_dtr_tx_read_dac_status_i_q_en(txlm_chain_type chain,uint32* data);
void rflm_dtr_tx_set_manual_iref_update(txlm_chain_type chain);
void rflm_dtr_tx_clear_manual_iref_update(txlm_chain_type chain);
void rflm_dtr_tx_txdac_power_down_mission(txlm_chain_type chain);
void rflm_dtr_tx_txdac_reactivate_mission(txlm_chain_type chain, boolean blocking);
void rflm_dtr_tx_foundry_init_ag ( uint16 foundry_id );
void rflm_tx_dac_fast_charge(uint32 dac_clk_cycles,  uint32 iref_update_stop);

boolean rflm_dtr_tx_txdac_reprogram_msbcal_code(txlm_chain_type chain);
void rflm_dtr_tx_dac_extract_msb_calcodes(txlm_chain_type chain,uint8 size,uint32* cal_data);

void rflm_dtr_tx_msb_cal_code_dump();
void rflm_dtr_tx_txdac_msb_reprogram_set_calib(txlm_chain_type chain);
void rflm_dtr_tx_start_dac_clk_cal_mode(void);
void rflm_dtr_tx_read_fuseflag_bit(uint32* fuseflag_bit);
void rflm_dtr_tx_txdac_clk_enable_cal_mode(void);
void rflm_tx_txdac_mission_mode(rflm_handle_tx_t handle,uint32* nv_dac_cal_results);
void rflm_dtr_tx_fcal_clk_disable();
void rflm_dtr_tx_debug_mode();
void rfl_dtr_check_dac_mode(uint32* dac_status);
void rflm_dtr_tx_atop_reg_update(void);







boolean rflm_dtr_tx_and_et_dac_reprogram_msbcal_code(txlm_chain_type chain, boolean etdac_setup);
uint32 rfmodem_target_get_chipset_id(void);
void rflm_dtr_tx_dac_iref
(
  txlm_chain_type chain,
  boolean update_en,
  uint32 iref_idx,
  uint32 iref_update_start,
  uint32 iref_update_stop
);



boolean rfmodem_target_fuse_bbrx_compensation_needed(void);


#ifdef _cplusplus
}
#endif

#endif /* RFLM_DTR_TX_DAC_CTL_H */

