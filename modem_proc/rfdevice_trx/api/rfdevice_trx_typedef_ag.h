#ifndef RFDEVICE_TRX_TYPEDEF_AG_H
#define RFDEVICE_TRX_TYPEDEF_AG_H
/*
  WARNING: This file is auto-generated.

  Generated at:    0
  Generated using: AutoGenerate.pl
  Generated from:  v60 of the hubble spreadsheet
*/

/*!
  @file
  rfdevice_trx_typedef_ag.h

  @brief
  WTR common ag definitions

  @details
  This file is auto-generated and it contains data types to support the interaction with the QUALCOMM transceiver chips

  @addtogroup rfdevice common AG
  @{
*/

/*=============================================================================
  Copyright (c) 2015-2016 by QUALCOMM Technologies, Inc.  All Rights Reserved.

  Qualcomm Technologies Proprietary and Confidential

  Export of this technology or software is regulated by the U.S. 
  Government. Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //components/rel/rfdevice_trx.mpss/1.20/api/rfdevice_trx_typedef_ag.h#9 $
  $DateTime: 2023/10/17 23:12:43 $
  $Author: mplcsds1 $
=============================================================================*/

/*========================= INCLUDE FILES FOR MODULE ========================*/
#include "comdef.h" 
#include "rfdevice_type_defs.h" 
#include "rfdevice_cmn_type_defs.h" 

/*===========================================================================*/
  
#define RFDEVICE_TRX_PATH_ASSN_ALL 0
#define RFDEVICE_TRX_CHAR_NOT_AVAILABLE -32768
#define RFDEVICE_TRX_LNA_PHASE_COMP_NOT_AVAILABLE 0

#define RFDEVICE_TRX_RX_IIP2_INVALID_INDEX 0xFFFF

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_instr_enum_type enum.
*/
typedef enum 
{
  RFDEVICE_TRX_DELAY, 
  RFDEVICE_TRX_EXTERNAL_DCCAL_RXGAIN_DRX_BBF_REF, 
  RFDEVICE_TRX_EXTERNAL_DCCAL_RXGAIN_DRX_ILNA_REF, 
  RFDEVICE_TRX_EXTERNAL_DCCAL_RXGAIN_PRX_BBF_REF, 
  RFDEVICE_TRX_EXTERNAL_DCCAL_RXGAIN_PRX_ILNA_REF, 
  RFDEVICE_TRX_EXTERNAL_DRX_SM_OFF_REF, 
  RFDEVICE_TRX_EXTERNAL_DRX_SM_ON_REF, 
  RFDEVICE_TRX_EXTERNAL_FBADC_CTRL_DISABLE_REF, 
  RFDEVICE_TRX_EXTERNAL_FBADC_CTRL_ENABLE_REF, 
  RFDEVICE_TRX_EXTERNAL_FBADC_CTRL_RESET_REF, 
  RFDEVICE_TRX_EXTERNAL_GNSS_PGA_REF, 
  RFDEVICE_TRX_EXTERNAL_HDET_LO_CTRL_RESET_REF, 
  RFDEVICE_TRX_EXTERNAL_HDET_LO_CTRL_SET_REF, 
  RFDEVICE_TRX_EXTERNAL_HDET_RXGAIN_G9_REF, 
  RFDEVICE_TRX_EXTERNAL_HDET_RXGAIN_PRX_BBF_G9_REF, 
  RFDEVICE_TRX_EXTERNAL_HDET_RXGAIN_PRX_ILNA_G9_REF, 
  RFDEVICE_TRX_EXTERNAL_IIP2_DRX_REF, 
  RFDEVICE_TRX_EXTERNAL_IIP2_ICODE_REF, 
  RFDEVICE_TRX_EXTERNAL_IIP2_LOOPBACK_DATA_REF, 
  RFDEVICE_TRX_EXTERNAL_IIP2_LOOPBACK_DA_SWITCH_VAL_REF, 
  RFDEVICE_TRX_EXTERNAL_IIP2_PRX_REF, 
  RFDEVICE_TRX_EXTERNAL_IIP2_QCODE_REF, 
  RFDEVICE_TRX_EXTERNAL_IIP2_REF, 
  RFDEVICE_TRX_EXTERNAL_LO_DIV_REF, 
  RFDEVICE_TRX_EXTERNAL_LUT_FBRXGAIN_REF, 
  RFDEVICE_TRX_EXTERNAL_LUT_RXBW_REF, 
  RFDEVICE_TRX_EXTERNAL_LUT_RXGAIN_DRX_BBF_REF, 
  RFDEVICE_TRX_EXTERNAL_LUT_RXGAIN_DRX_ILNA_REF, 
  RFDEVICE_TRX_EXTERNAL_LUT_RXGAIN_PRX_BBF_REF, 
  RFDEVICE_TRX_EXTERNAL_LUT_RXGAIN_PRX_ILNA_REF, 
  RFDEVICE_TRX_EXTERNAL_LUT_TXBBF_0_REF, 
  RFDEVICE_TRX_EXTERNAL_LUT_TXBBF_REF, 
  RFDEVICE_TRX_EXTERNAL_LUT_TXRGI_0_REF, 
  RFDEVICE_TRX_EXTERNAL_LUT_TXRGI_REF, 
  RFDEVICE_TRX_EXTERNAL_PLL_AFC_REF, 
  RFDEVICE_TRX_EXTERNAL_PLL_BANK_SEL_REF, 
  RFDEVICE_TRX_EXTERNAL_PLL_CALC1_REF, 
  RFDEVICE_TRX_EXTERNAL_PLL_CALC2_REF, 
  RFDEVICE_TRX_EXTERNAL_PLL_CALC3_REF, 
  RFDEVICE_TRX_EXTERNAL_PLL_DFLO_REF, 
  RFDEVICE_TRX_EXTERNAL_PLL_REF, 
  RFDEVICE_TRX_EXTERNAL_PRX_SM_OFF_REF, 
  RFDEVICE_TRX_EXTERNAL_PRX_SM_ON_REF, 
  RFDEVICE_TRX_EXTERNAL_REF, 
  RFDEVICE_TRX_EXTERNAL_RX_ENABLE_REF, 
  RFDEVICE_TRX_EXTERNAL_RX_ON_REF, 
  RFDEVICE_TRX_EXTERNAL_SPLL_TDD_TX_REF, 
  RFDEVICE_TRX_READ_WXE, 
  RFDEVICE_TRX_WXE_DELAY_INSTR_ABSOLUTE, 
  RFDEVICE_TRX_WXE_DELAY_INSTR_RELATIVE, 
  RFDEVICE_TRX_WXE_INSTR_FIXED, 
  RFDEVICE_TRX_WXE_MOVE16_DATA_FIXED, 
  RFDEVICE_TRX_WXE_MOVE16_EXTERNAL_DST_DRX_SM, 
  RFDEVICE_TRX_WXE_MOVE16_EXTERNAL_DST_PRX_SM, 
  RFDEVICE_TRX_WXE_MOVE16_INSTR_DYNAMIC, 
  RFDEVICE_TRX_WXE_MOVE16_INSTR_FIXED, 
  RFDEVICE_TRX_WXE_MOVE16_INSTR_STATIC, 
  RFDEVICE_TRX_WXE_MOVE_DATA_FIXED, 
  RFDEVICE_TRX_WXE_MOVE_INSTR_DYNAMIC, 
  RFDEVICE_TRX_WXE_MOVE_INSTR_FIXED, 
  RFDEVICE_TRX_WXE_MOVE_INSTR_STATIC, 
  RFDEVICE_TRX_WXE_TRIGGER, 
} rfdevice_trx_instr_enum_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_move_instr_type enum.
*/
typedef enum 
{
  RFDEVICE_TRX_MOVE16_TYPE,  /* Instruction type MOVE16 */
  RFDEVICE_TRX_MOVE_TYPE,  /* Instruction type MOVE(32) */
  RFDEVICE_TRX_INVALID_MOVE_TYPE,  /* Instruction type INVALID MOVE */
} rfdevice_trx_move_instr_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_fixup_token_type enum.
*/
typedef enum 
{
  RFDEVICE_TRX_TIMER_FIXUP_TOKEN_DEFAULT_TIMER, 
  RFDEVICE_TRX_TIMER_FIXUP_TOKEN_HPM_TO_LPM, 
  RFDEVICE_TRX_TIMER_FIXUP_TOKEN_LPM_TO_HPM, 
  RFDEVICE_TRX_TIMER_FIXUP_TOKEN_RX_PLL_OFF, 
  RFDEVICE_TRX_TIMER_FIXUP_TOKEN_RX_PLL_ON, 
  RFDEVICE_TRX_TIMER_FIXUP_TOKEN_RX_SIGPATH_OFF, 
  RFDEVICE_TRX_TIMER_FIXUP_TOKEN_RX_SIGPATH_ON, 
  RFDEVICE_TRX_TIMER_FIXUP_TOKEN_RX_SIGPATH_ON_SPLL, 
  RFDEVICE_TRX_TIMER_FIXUP_TOKEN_TXRGI_0, 
  RFDEVICE_TRX_TIMER_FIXUP_TOKEN_TX_PLL_OFF, 
  RFDEVICE_TRX_TIMER_FIXUP_TOKEN_TX_PLL_ON, 
  RFDEVICE_TRX_TIMER_FIXUP_TOKEN_TX_SIGPATH_OFF, 
  RFDEVICE_TRX_TIMER_FIXUP_TOKEN_TX_SIGPATH_ON, 
  RFDEVICE_TRX_DATA_FIXUP_TOKEN_PRX_AGC, 
  RFDEVICE_TRX_DATA_FIXUP_TOKEN_PLL, 
  RFDEVICE_TRX_DATA_FIXUP_TOKEN_PRX_EOC, 
  RFDEVICE_TRX_DATA_FIXUP_TOKEN_TXRGI, 
  RFDEVICE_TRX_DATA_FIXUP_TOKEN_CT_REG_BANK_SEL, 
  RFDEVICE_TRX_DATA_FIXUP_TOKEN_TX_FFH, 
  RFDEVICE_TRX_DATA_FIXUP_TOKEN_RX_FFH, 
  RFDEVICE_TRX_DATA_FIXUP_TOKEN_FBADC_RESET, 
  RFDEVICE_TRX_DATA_FIXUP_TOKEN_FBADC_ENABLE, 
  RFDEVICE_TRX_FIXUP_TOKEN_MAX, 
  RFDEVICE_TRX_FIXUP_TOKEN_INVALID, 
} rfdevice_trx_fixup_token_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_instr_type struct.
*/
typedef struct 
{
  uint8 instr_type;  /* Instruction type (rfdevice_trx_instr_enum_type) */
  uint8 reg_tbl_entries_to_scan;  /* reg tbl entries to scan */
  uint8 move_type;  /* Move type (rfdevice_trx_move_instr_type) */
  uint32 coded_instr;  /* coded wxe instruction */
} rfdevice_trx_instr_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_instr_tbl_type struct.
*/
typedef struct 
{
  uint16 instruction_tbl_size;  /* Size of instruction table */
  const rfdevice_trx_instr_type* instruction;  /* Array of WXE instructions */
} rfdevice_trx_instr_tbl_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_reg_type struct.
*/
typedef struct 
{
  uint8 revision;  /* Chip revision */
  uint8 process;  /* Chip process */
  uint8 path_assn;  /* path association */
} rfdevice_trx_reg_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_reg_tbl_type struct.
*/
typedef struct 
{
  uint16 reg_tbl_size;  /* Size of register table table */
  const rfdevice_trx_reg_type* reg_tbl;  /* Array of Registers */
} rfdevice_trx_reg_tbl_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_wxe_mem_tbl_type struct.
*/
typedef struct 
{
  uint8 wxe_protection_id; 
  uint8 wxe_engine; 
  uint16 wxe_address_cmd_start; 
  uint16 wxe_address_error; 
  uint16 wxe_address_status; 
  uint16 wxe_mem_instr_address_start; 
  uint16 wxe_mem_instr_address_stop; 
  uint16 wxe_mem_data_address_start; 
  uint16 wxe_mem_data_address_stop; 
} rfdevice_trx_wxe_mem_tbl_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_lodiv_data_type struct.
*/
typedef struct 
{
  uint8 lodiv_ratio;  /* LO div ratio integer value */
  const uint16* lodiv_data;  /* Pointer to settings for this lo div ratio */
} rfdevice_trx_lodiv_data_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_lodiv_lut_type struct.
*/
typedef struct 
{
  uint8 num_lodiv_ratios;  /* Number of LO div ratios for this band */
  const rfdevice_trx_lodiv_data_type* lodiv_lut;  /* Pointer to array with data for all LO div ratios */
} rfdevice_trx_lodiv_lut_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_cal_needed_lut_type struct.
*/
typedef struct 
{
  uint8 revision_id;  /* Number of chip revisions */
  uint16 tbl_size;  /* total entries in array table */
  const boolean* cal_needed_lut;  /* Pointer to array with data for Cal needed flag */
} rfdevice_trx_cal_needed_lut_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_wxe_template_type struct.
*/
typedef struct 
{
  uint16 wxe_instr_start_addr;  /* Start address in WXE where template instructions can be loaded */
  uint16 wxe_instr_stop_addr;  /* WXE stop address beyond which template instructions should not write */
  uint16 wxe_data_start_addr;  /* Start address in WXE where template data can be loaded */
  uint16 wxe_data_stop_addr;  /* WXE stop address beyond which template data should not write */
  rfdevice_trx_instr_tbl_type wxe_templ_instr_tbl;  /* WXE instruction list */
} rfdevice_trx_wxe_template_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_edge_env_delay_ag_type struct.
*/
typedef struct 
{
  uint32 dp_rfi_et_coarse_tau; 
  uint32 dtr_txfe_dp_dm2_i_fine_tau; 
  uint32 dtr_txfe_dp_dm2_q_fine_tau; 
  uint32 dtr_txfe_dp_dm3_i_frac_tau; 
  uint32 dtr_txfe_dp_dm3_q_frac_tau; 
  uint32 dtr_txfe_et_dm2_fine_tau; 
  uint32 dtr_txfe_et_dm3_frac_tau; 
  uint32 dtr_dac_rate_khz; 
} rfdevice_trx_edge_env_delay_ag_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_test_dyn_port_lut_type struct.
*/
typedef struct 
{
  rfdevice_trx_tech_type tech; 
  uint16 rf_band; 
  uint32 freq_khz; 
  uint8 rf_bw; 
  uint16 rfc_port; 
  uint8 vco_id; 
  boolean is_mimo; 
  rfdevice_trx_iq_path_type trx_iq_path; 
  uint16 dynamic_wtr_band_port; 
} rfdevice_trx_test_dyn_port_lut_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_test_dyn_port_lut_wrapper_type struct.
*/
typedef struct 
{
  uint16 num_rows; 
  uint16 num_clns; 
  uint16 num_tx; 
  const rfdevice_trx_test_dyn_port_lut_type* test_dyn_port_lut; 
  const uint16* test_dyn_port_chip_rev_lut; 
  const char** test_key_lut; 
} rfdevice_trx_test_dyn_port_lut_wrapper_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_process_data_type enum.
*/
typedef enum 
{
  RFDEVICE_TRX_PROCESS_NN, 
  RFDEVICE_TRX_PROCESS_SS, 
  RFDEVICE_TRX_PROCESS_FF, 
  RFDEVICE_TRX_PROCESS_NUM, 
  RFDEVICE_TRX_PROCESS_INVALID, 
} rfdevice_process_data_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_rsb_data_type struct.
*/
typedef struct 
{
  int16 gain_watts; 
  int16 phase_sine; 
  int16 phase_cosine; 
} rfdevice_rsb_data_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_test_standalone_mode_pll_option_lut_type struct.
*/
typedef struct 
{
  rfdevice_trx_tech_type tech; 
  uint16 rf_band; 
  uint32 freq_khz; 
  uint8 rf_bw; 
  uint16 rfc_port; 
  uint8 vco_id; 
  rfdevice_trx_iq_path_type trx_iq_path; 
  uint8 expected_pll_option; 
} rfdevice_trx_test_standalone_mode_pll_option_lut_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_test_standalone_mode_pll_option_lut_wrapper_type struct.
*/
typedef struct 
{
  uint8 num_rows; 
  uint8 num_clns; 
  uint8 num_tx; 
  const rfdevice_trx_test_standalone_mode_pll_option_lut_type* test_vec; 
  const uint16* test_dyn_port_chip_rev_lut; 
  const char** test_key_lut; 
} rfdevice_trx_test_standalone_mode_pll_option_lut_wrapper_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_qfprom_sym_info_type struct.
*/
typedef struct 
{
  uint16 block_indx;  /* block/row index in SS */
  uint8 bit_start;  /* Start bit in the block */
  uint8 bit_stop;  /* Stop bit in the block */
  uint16 default_val;  /* default value for the QFPROM sym */
} rfdevice_trx_qfprom_sym_info_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_qfprom_sym_tbl_type struct.
*/
typedef struct 
{
  uint16 start_addr;  /* qfprom start address */
  uint16 stop_addr;  /* qfprom stop address */
  uint16 num_entries;  /* number of entries */
  const rfdevice_trx_qfprom_sym_info_type* syms_info; 
} rfdevice_trx_qfprom_sym_tbl_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_fdrsb_cal_params_ag_type struct.
*/
typedef struct 
{
  uint8 bw;  /* LTE Bandwidth for this row */
  uint8 num_tone_offsets;  /* Number of tone offsets for FDRSB cal - indicates size of tone_offsets_arr and inv_rb matrix */
  uint32 adc_fs_khz;  /* ADC sampling rate in khz */
  const int16* tone_offset_arr_khz;  /* Pointer to tone offset array of size num_tone_offsets */
  const int32* inv_rb_real_matrix;  /* Pointer to INV RB Real matrix of size num_tone_offsets x num_tone_offsets */
  const int32* inv_rb_imag_matrix;  /* Pointer to INV RB Imag matrix of size num_tone_offsets x num_tone_offsets */
  uint8 scaling_factor;  /* Scaling factor to be applied in FDRSB calculations */
} rfdevice_trx_fdrsb_cal_params_ag_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_sub_band_tbl_type struct.
*/
typedef struct 
{
  uint16 lo_freq_start_mhz_x10; 
  uint16 lo_freq_stop_mhz_x10; 
  uint16 band_port; 
} rfdevice_trx_sub_band_tbl_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_sub_band_wrapper_tbl_type struct.
*/
typedef struct 
{
  uint8 num_sub_bands; 
  const rfdevice_trx_sub_band_tbl_type* sub_band_tbl; 
} rfdevice_trx_sub_band_wrapper_tbl_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_band_port_rev_type struct.
*/
typedef struct 
{
  uint8 revision_mask;  /* Revisions for which band_port is applicable */
  uint16 band_port;  /* WTR specific band port value */
} rfdevice_trx_band_port_rev_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_band_mode_rev_type struct.
*/
typedef struct 
{
  uint8 revision_mask;  /* Revisions for which band_mode is applicable */
  uint16 band_mode;  /* WTR specific band mode value */
} rfdevice_trx_band_mode_rev_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_rx_iip2_info_type struct.
*/
typedef struct 
{
  uint8 qfprom_rev_mask;  /* QFPROM revision bit mask */
  uint16 qfprom_rx_iip2_sym;  /* QFPROM RX IIP2 Symbol */
  uint16 path_assn;  /* Path Assn */
} rfdevice_trx_rx_iip2_info_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_rx_iip2_index_type struct.
*/
typedef struct 
{
  uint16 start_index;  /* Start index */
  uint16 stop_index;  /* Stop index */
} rfdevice_trx_rx_iip2_index_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_rxsm_data_type struct.
*/
typedef struct 
{
  uint16 address;  /* Address for RxSM control for given Rx Path Association */
  uint16 rxsm_on_data;  /* Data to be written to enable RxSM for given path association */
  uint16 rxsm_off_data;  /* Data to be written to reset RxSM */
} rfdevice_trx_rxsm_data_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_rxsm_lut_type struct.
*/
typedef struct 
{
  uint8 rev_mask;  /* Revision mask for Rx Switch table */
  uint16 rxsm_size;  /* Size of Rx Switch table array = number of Rx Path Association. Printed here so code to parse this lut can be in common trx area */
  const rfdevice_trx_rxsm_data_type* rxsm_tbl;  /* Pointer to Rx Switch module data table */
} rfdevice_trx_rxsm_lut_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_input_port_restriction_type struct.
*/
typedef struct 
{
  uint8 revision;  /* chip revision bitmask */
  rfdevice_trx_tech_type tech;  /* tech type */
  sys_band_class_e_type band;  /* System band embeds info about tech and band */
  uint16 rx_port_1;  /* Restricted WTR port 1 */
  uint16 rx_port_2;  /* Restricted WTR port 2 */
} rfdevice_trx_input_port_restriction_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_hdet_reg_type struct.
*/
typedef struct 
{
  uint16 vco_syn_lo_set[2]; 
  uint16 fbadc_ctr1_reset; 
  uint16 fbadc_ctr1_enable; 
  uint16 vco_syn_lo_reset[2]; 
  uint16 fbadc_ctr1_disable; 
} rfdevice_trx_hdet_reg_type;

#define RFDEVICE_TRX_TXBBF_NUM_REGS 8

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_txbbf_data_type struct.
*/
typedef struct 
{
  uint8 chip_rev;  /* chip revision */
  uint8 bbf_idx;  /* bbf index */
  uint16 bbf_data[RFDEVICE_TRX_TXBBF_NUM_REGS];  /* bbf data */
} rfdevice_trx_txbbf_data_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_txbbf_lut_type struct.
*/
typedef struct 
{
  uint16 num_bbf_index;  /* Size of BBF LUT */
  const rfdevice_trx_txbbf_data_type* bbf_data_tbl;  /* Pointer to bbf lut */
} rfdevice_trx_txbbf_lut_type;

#define RFDEVICE_TRX_TXAGC_LUT_NUM_REGS 6

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_txagc_pwr_data_type enum.
*/
typedef enum 
{
  RFDEVICE_TRX_HP_LP_LUT_TYPE,  /* Common for High and Low power */
  RFDEVICE_TRX_HP_LUT_TYPE,  /* High Power */
  RFDEVICE_TRX_LP_LUT_TYPE,  /* Low Power */
  RFDEVICE_TRX_LUT_PWR_NUM,  /* Max */
} rfdevice_trx_txagc_pwr_data_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_chip_process_type enum.
*/
typedef enum 
{
  RFDEVICE_TRX_CHIP_PROCESS_NOMINAL,  /* NN */
  RFDEVICE_TRX_CHIP_PROCESS_SLOW,  /* SS */
  RFDEVICE_TRX_CHIP_PROCESS_FAST,  /* FF */
  RFDEVICE_TRX_CHIP_PROCESS_MAX,  /* MAX */
} rfdevice_trx_chip_process_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_txrsb_index_type enum.
*/
typedef enum 
{
  RFDEVICE_TRX_TXRSB_TX0_INDEX,  /* TX0_INDEX */
  RFDEVICE_TRX_TXRSB_TX1_INDEX,  /* TX1_INDEX */
  RFDEVICE_TRX_TXRSB_MAX_TX_INDEX,  /* MAX_INDEX */
} rfdevice_trx_txrsb_index_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_txagc_data_type struct.
*/
typedef struct 
{
  rfdevice_trx_txagc_pwr_data_type tx_pwr_type;  /* HP/LP Pwr */
  uint8 rgi;  /* rgi */
  uint8 enc_rev;  /* Encoded revision */
  uint8 bbf_indx;  /* bbf index in bbf table */
  uint8 phase;  /* phase data */
  uint16 iref;  /* iref data */
  int16 da_pout_db100;  /* Expected DA output power in dBx100 */
  uint16 lut_data[RFDEVICE_TRX_TXAGC_LUT_NUM_REGS];  /* Tx RGI data */
  rfdevice_rsb_data_type rsb_data[RFDEVICE_TRX_PROCESS_NUM];  /* Tx RSB data */
} rfdevice_trx_txagc_data_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_static_tx_rsb_data_type struct.
*/
typedef struct 
{
  uint32 dcoc;  /* dcoc */
  int16 tau_scale;  /* tau_scale */
  int16 gain_offs;  /* gain_offs */
  int16 gain_scale;  /* gain_scale */
  int16 gain_const;  /* gain_const */
  int16 phase_offs;  /* phase_offs */
  int16 phase_scale;  /* phase_scale */
  int16 phase_const;  /* phase_const */
} rfdevice_trx_static_tx_rsb_data_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_qfprom_tx_rsb_data_type struct.
*/
typedef struct 
{
  rfdevice_trx_txagc_pwr_data_type tx_pwr_type;  /* HP/LP Pwr */
  rfdevice_trx_txrsb_index_type txrsb_tx0_tx1_idx;  /* TX0/TX1 Index */
  uint8  rf_chip_bmap;  /* chip_rev_bmap */
  uint8  qfprom_rev_bmap;  /* qfprom rev bmap */
  uint16 qfprom_txrsb_gain;  /* qfprom_sym_enum_type */
  uint16 qfprom_txrsb_phase;  /* qfprom_sym_enum_type */
  uint16 qfprom_txrsb_tau;  /* qfprom_sym_enum_type */
} rfdevice_trx_qfprom_tx_rsb_data_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_qfprom_tx_rsb_type struct.
*/
typedef struct 
{
  const rfdevice_trx_qfprom_tx_rsb_data_type *tx_qfprom_rsb_tbl;  /* Pointer to QFPROM TXRSB LUT */
  uint16 num_qfprom_txrsb_elements;  /* Num Rows in LUT */
} rfdevice_trx_qfprom_tx_rsb_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_txagc_indx_tbl_type struct.
*/
typedef struct 
{
  uint8 rgi_start;  /* Start rgi */
  uint8 rgi_end;  /* End rgi */
  uint8 arr_indx_start;  /* Array index of lut tbl corresponding to firt entry of rgi_start */
  uint8 arr_indx_end;  /* Array index of lut tbl corresponding to last entry rgi_end */
} rfdevice_trx_txagc_indx_tbl_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_txagc_lut_type struct.
*/
typedef struct 
{
  rfdevice_trx_txagc_indx_tbl_type rgi_indx_tbl[RFDEVICE_TRX_LUT_PWR_NUM];  /* Array of RGI index info per LUT */
  rfdevice_trx_qfprom_tx_rsb_type qfprom_rsb_lut;  /* Array of Static TX RSB LUTs */
  const rfdevice_trx_txagc_data_type* tx_rgi_tbl;  /* Pointer to TX QPROM RSB LUT */
} rfdevice_trx_txagc_lut_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_txagc_lodiv_entry_type struct.
*/
typedef struct 
{
  uint8 lo_div_ratio;  /* LO Div ratio corresponding to Tx AGC LUT  */
  const rfdevice_trx_txagc_lut_type* txagc_lut;  /* Pointer to TX AGC lut */
} rfdevice_trx_txagc_lodiv_entry_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_txagc_lodiv_lut_type struct.
*/
typedef struct 
{
  uint8 num_lodiv;  /* Number of LO dividers for given band/port */
  const rfdevice_trx_txagc_lodiv_entry_type* txagc_lodiv_lut;  /* Pointer to TX AGC lut for all LO Dividers */
} rfdevice_trx_txagc_lodiv_lut_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_rsb_sincos_type struct.
*/
typedef struct 
{
  int16 sine_val;  /* sin value */
  int16 cosine_val;  /* cos value */
} rfdevice_trx_rsb_sincos_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_rsb_gain_watt_type struct.
*/
typedef struct 
{
  uint16 gain_watt_pos_q14;  /* positive Gain */
  uint16 gain_watt_neg_q14;  /* negative Gain */
} rfdevice_trx_rsb_gain_watt_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_rsb_type struct.
*/
typedef struct 
{
  uint16 gain_watt;  /* Gain */
  int16 sine_val;  /* sin value */
  int16 cosine_val;  /* cos value */
} rfdevice_trx_rsb_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the rfdevice_trx_rx_rsb_type struct.
*/
typedef struct 
{
  boolean rsb_calibration_required;  /* is cal required */
  int16 error_gain_room_x1000;  /* error gain x1000 */
    int16 error_phase_room_x1000;  /* error phase x1000 */
  int16 sin_phase_room_x2pow14;  /* sine phase x2pow14 */
  int16 cos_phase_room_x2pow14;  /* cos phase x2pow14 */
  int16 error_gain_room_x2pow14;  /* error gain x2pow14 */
} rfdevice_trx_rx_rsb_type;


typedef enum
{
  RFDEVICE_TRX_LIN_HLIN,
  RFDEVICE_TRX_LIN_LLIN,
  RFDEVICE_TRX_LIN_MODE_NUM,
  RFDEVICE_TRX_LIN_MODE_INVALID
} rfdevice_trx_lin_mode_enum_type;

typedef enum
{
  RFDEVICE_TRX_LNA_PATH_RX,
  RFDEVICE_TRX_LNA_PATH_NUM,
  RFDEVICE_TRX_LNA_PATH_INVALID
} rfdevice_trx_lna_path_enum_type;

typedef enum
{
  RFDEVICE_TRX_MODULATION_EDGE,
  RFDEVICE_TRX_MODULATION_GSM,
  RFDEVICE_TRX_MODULATION_NUM,
  RFDEVICE_TRX_MODULATION_INVALID
} rfdevice_trx_modulation_enum_type;

#endif /* RFDEVICE_TRX_TYPEDEF_AG_H */

/*! @} */ 
