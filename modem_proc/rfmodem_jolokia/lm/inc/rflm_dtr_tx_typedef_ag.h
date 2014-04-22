
#ifndef RFLM_DTR_TX_TYPEDEF_AG_H
#define RFLM_DTR_TX_TYPEDEF_AG_H


#ifdef __cplusplus
extern "C" {
#endif

/*
WARNING: This file is auto-generated.

Generated at:    Tue Jul 21 10:29:59 2020
Generated using: lm_autogen_txlm.pl v5.1.37
Generated from:  v7.0.3 of Jolokia_TXLM_register_settings.xlsx
*/

/*=============================================================================

           T X    A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the modem register settings 
  configured by FW, provided by the rflm_dtr_tx.

Copyright (c) 2009, 2010, 2011, 2012, 2013, 2014, 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

$DateTime: 2020/07/28 04:03:43 $
$Header: //components/rel/rfmodem_jolokia.mpss/1.10/lm/inc/rflm_dtr_tx_typedef_ag.h#2 $

=============================================================================*/

/*=============================================================================
                           REVISION HISTORY
Version    Author   Date   
         Comments   IR Number   Other POC (Comma Delimited)   Release Validation Notes   Dependencies   AG File Location   Changes for   
7.0.3   speshwe   7/20/2020   
         Add 100 kHz offset for NBIoT LTE 0.2 MHz case      yitankar@qti.qualcomm.com            Chiltepin   
7.0.2   yitankar   2/5/2019   
         1. IREF table added for Chiltepin DAC      yitankar@qti.qualcomm.com            Chiltepin   
7.0.2   yitankar   2/4/2019   
         1. Modified msm_reg_settings labels (row 3) for Chiltepin DAC rates       yitankar@qti.qualcomm.com            Chiltepin   
7.0.2   yitankar   2/3/2019   
         1. Modified row 56 under msm_reg_settings (DAC_CLK per tech) 2. TX_CONFIG3 changed for mission mode (as per DAC datasheet)  3. Zero indexed (_c) for DAC_CAL_control registers  4. TX_DAC0_IREF_GAIN : This register is not applicable for Chiltepin DAC. Removed it. 5. Removed hidden column under msm_reg_settings : LTE_DAC_230p4MHz_chain0_BW10MHz_1kFFT      yitankar@qti.qualcomm.com            Chiltepin   
7.0.2   yitankar   1/14/2019   
         1. Updated DAC companion registers. Jacala DAC companion is used for Chiltepin.  2. Labels added to msm_reg_settings as per DTR team's recommendation.  3. Removed GSM_IPCAL2 column in msm_reg_settings.  4. Modified DM1/2/3 for GSM config. Envelope path is delayed by 520.83 ns (from RF bench characterization).      yitankar@qti.qualcomm.com            chiltepin   
7.0.2   Xiaofeng Wang   10/3/2016   
         Changed TX_DIGITAL_DELAY_MICROSECS_Q16 for NB1 to be 3.301(us)*2^16=216334      junsheng@qti.qualcomm.com,wangxiao@qti.qualcomm.com               
7.0.1   junsheng   9/29/2016   
         Configure DP UP2 to use regular (non-MC) setting      junsheng@qti.qualcomm.com,wangxiao@qti.qualcomm.com            LTE NB1   
7.0.0   junsheng   9/1/2016   
         Initial version for NB1 based on LTE 1.4MHz mode. DAC frequency at 76.8MHz.       junsheng@qti.qualcomm.com,wangxiao@qti.qualcomm.com            LTE NB1   
6.3.2   nichunw   8/28/2015   
         1. Enabled TWO_SAMPLES_CYCLE for DO, to accommodate the lower offline clock rate (72MHz).      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            DO   
6.3.1   nichunw   5/21/2015   
         1. Added modem spec column to handle the SWI difference between Cheel and Lykan. 2. Remove GSM chain 1 state.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            All techs   
6.3.0   nichunw   5/18/2015   
         FW interface change for LTE power optimization. (No spreadsheet change).      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            LTE   
6.2.8   sunilk   2/11/2014   
         updates to cal/mission-mode DAC_CFG0/1/2 values from Filip; new DAC_IREF LUT for GF in the IREF_Dynamic tab      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            DAC   
6.2.7   sunilk   2/11/2014   
         1. Change REF_DWN_FACTOR to 1 for TDS_sAPT states in DAC rate 230.4MHz to lower down the DPD capture rate by half.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            TDS   
6.2.6   nichunw   11/18/2014   
         1. Added foundry dependent DAC configuration for TSMC and GF.       nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            DAC   
6.2.5   nichunw   10/29/2014   
         1. Enable PEQ2/SORU blocks for LTE and WCDMA (with coefficients for WTR4905).      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            LTE, UMTS   
6.2.4   nichunw   10/21/2014   
         1. Added UMTS_SC_DAC_230p4MHz_chain0_CW state.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            UMTS   
6.2.3   nichunw   10/21/2014   
         1. Updated COMBODAC_CFG1_0 to enable reading back the latched IREF gain.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            DAC   
6.2.2   nichunw   10/2/2014   
         1. Added COMBODAC_COMP_CTR::IQ_EN_IMMED=0 for all states. 2. Switch off PEQ/SORU before getting updated coefficients for WTR2955. 3. Updated DP_PEQ_SW_Dynamic tab to add UMTS with WTR4905. 4. Updated COMBODAC_CFGx in DAC_cal_LM_settings tab.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            All techs   
6.2.1   nichunw   8/15/2014   
         1. Change modem version for TXFE_DELAYMATCH block from DimePM to Jolokia. 2. Added the fix of DAC avg_error and overflow bits (ported from DimePM)  3. Added COMBODAC_CLK_SRC_CFG::DAC_FCAL_CLK_SEL and COMBODAC_COMP_CTRL::DAC_COMP_MODE, so that after MCPM sleep the register values are reprogrammed.       nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            All techs   
6.2.0   nichunw   7/29/2014   
         1. . Updated PEQ1/PEQ2/SORU coefficients for WTR4905, both on main tab and DP_PEQ_SW_dynamic tab. 2. Added two new txlm variable TXC_input_freq_hz and DAC_freq_khz. 3. Changed state specific mask write to normal mask write for COMBODAC_CLK_SRC_CFG. 4. Added COMBODAC_CAL_CTRL_0::FSM_CAL_MODE=2 for all states.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            All techs   
6.1.3   nichunw   6/5/2014   
         1. Change d index to 0 index for register COMBODAC_CAL_STATUS.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            DAC   
6.1.2   nichunw   6/2/2014   
         1. Added GSM_DAC_76p8MHz_Anritsu_chain0 state.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            GSM   
6.1.1   nichunw   5/28/2014   
         1. Changed An indices under TXC0 and TXC_DP_IQ_GAIN blocks to A0. 2. Changed d indices for COMBODAC_CFG to 0.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            All techs   
6.1.0   nichunw   5/22/2014   
         1. Updated DAC config settings for mission mode and factory cal mode. 2. Change to P3 SWI names (DAC registers use d_index) 3. Added TXFE_DELAYMATCH block for GSM to dynamically change the DM values. 4. Added Foundry column for foundry-dependent DAC_cfg. 5. Change TWO_SAMPLES_CYCLE to 1 for 1x and TDS 230.4MHz. 6. Disable EP_PEQ1/PEQ2_ENABLE for LTE, UMTS, 1x, DO. EP is not used by those techs. 7. Add Bolt indices to IREF LUT. SW should use Bolt indices to match indices used on WTR.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            All techs   
6.0.4   nichunw   5/15/2014   
         1. Change PP_GAIN for GSM 76.8MHz to 44373.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            GSM   
6.0.3   nichunw   5/6/2014   
         1. Change two_sample_cycle to 1 for TDS and 1x.  2. Added mask write to COMBODAC_CLK_SRC_CFG.  3. Set TX_MEM_EN=1 for LTE.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            All techs   
6.0.2   nichunw   4/21/2014   
         1. Revert the DAC-related register names to match HW P2 release to unblock FW.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            All techs   
6.0.1   nichunw   4/16/2014   
         1. Removed COMBODAC_DIGVAL_EN_d from the main tab. This is to optimize the single-Tx DSDA timeline, so txlm config can be done outside the transition time gap.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com            DSDA   
6.0.0   nichunw   4/3/2014   
         Iinitial version. Derived from DimePM txlm 5.3.7 with the following changes: 1. DAC rate=230.4MHz for all non-GSM states and 76.8MHz for GSM states. 2. Only GSM on chain 1. 3. New TXC0 block for chain 0 only registers. 4. Updated TX_COMMON block to Triton-based TX registers. 5. Updated field names in TXC/TXR.      nichunw@qti.qualcomm.com,sunilk@qti.qualcomm.com               
                              
                              
                              
                              
                              
                              
                              
                              
                              
                              
                              
                              
                              
                              
                              
                              

=============================================================================*/
/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "comdef.h" 
#include "txlm_intf.h" 





typedef enum
{
  RFLM_DTR_TX_STATE_GSM_DAC_153P6MHZ_CHAIN0,
  RFLM_DTR_TX_STATE_GSM_DAC_153P6MHZ_ANRITSU_CHAIN0,
  RFLM_DTR_TX_STATE_LTE_DAC_115P2MHZ_CHAIN0_BW1P4MHZ,
  RFLM_DTR_TX_STATE_LTE_NB1_DAC_115P2MHZ_CHAIN0,
  RFLM_DTR_TX_STATE_NUM,
  RFLM_DTR_TX_STATE_INVALID
}rflm_dtr_tx_state_enum_type;



typedef enum
{
  RFLM_DTR_TX_BW_1P4MHZ, 
  RFLM_DTR_TX_BW_200KHZ, 
  RFLM_DTR_TX_BW_NUM,
  RFLM_DTR_TX_BW_INVALID
}rflm_dtr_tx_enum_bw;



typedef enum
{
  RFLM_DTR_TX_FFT_SIZE_1K, 
  RFLM_DTR_TX_FFT_SIZE_32, 
  RFLM_DTR_TX_FFT_SIZE_NUM,
  RFLM_DTR_TX_FFT_SIZE_INVALID
}rflm_dtr_tx_enum_fft_size;



typedef struct
{
  txlm_chain_type chain;
  rflm_dtr_tx_enum_bw bw;
  boolean sapt_on;
  rflm_dtr_tx_enum_fft_size fft_size;
  boolean ip2cal;
  boolean anritsu;
}txlm_state_cfg_params;



typedef enum
{
  TXLM_GROUP_XO_VARS,	/* Use Struct: rflm_dtr_tx_xo_vars_group_struct */
  TXLM_GROUP_SW_XO_VARS,	/* Use Struct: rflm_dtr_tx_sw_xo_vars_group_struct */
  TXLM_GROUP_DIG_DELAY_VARS,	/* Use Struct: rflm_dtr_tx_dig_delay_vars_group_struct */
  TXLM_GROUP_ANALOG_DELAY_VARS,	/* Use Struct: rflm_dtr_tx_analog_delay_vars_group_struct */
  TXLM_GROUP_TXAGC_DM_VARS,	/* Use Struct: rflm_dtr_tx_txagc_dm_vars_group_struct */
  TXLM_GROUP_TXLM_VARS,	/* Use Struct: rflm_dtr_tx_txlm_vars_group_struct */
  TXLM_GROUP_TX_TOP,	/* Use Struct: rflm_dtr_tx_tx_top_group_struct */
  TXLM_GROUP_DAC_COMP,	/* Use Struct: rflm_dtr_tx_dac_comp_group_struct */
  TXLM_GROUP_TX_DAC_CFG,	/* Use Struct: rflm_dtr_tx_tx_dac_cfg_group_struct */
  TXLM_GROUP_IREF,	/* Use Struct: rflm_dtr_tx_iref_group_struct */
  TXLM_GROUP_TXC_DP_CFG,	/* Use Struct: rflm_dtr_tx_txc_dp_cfg_group_struct */
  TXLM_GROUP_DC,	/* Use Struct: rflm_dtr_tx_dc_group_struct */
  TXLM_GROUP_DM,	/* Use Struct: rflm_dtr_tx_dm_group_struct */
  TXLM_GROUP_REF,	/* Use Struct: rflm_dtr_tx_ref_group_struct */
  TXLM_GROUP_DP_CFG,	/* Use Struct: rflm_dtr_tx_dp_cfg_group_struct */
  TXLM_GROUP_IQMC,	/* Use Struct: rflm_dtr_tx_iqmc_group_struct */
  TXLM_GROUP_PEQ,	/* Use Struct: rflm_dtr_tx_peq_group_struct */
  TXLM_GROUP_EP_PEQ,	/* Use Struct: rflm_dtr_tx_ep_peq_group_struct */
  TXLM_GROUP_TXFE_DELAYMATCH,	/* Use Struct: rflm_dtr_tx_txfe_delaymatch_group_struct */
  TXLM_GROUP_DPIQGAIN_TXC,	/* Use Struct: rflm_dtr_tx_dpiqgain_txc_group_struct */
  TXLM_GROUP_TIMESTAMP,	/* Use Struct: rflm_dtr_tx_timestamp_txc_group_struct */
  TXLM_GROUP_TXR_IQ_DAC,	/* Use Struct: rflm_dtr_tx_txr_iq_dac_group_struct */
  RFLM_DTR_TX_BLOCK_ENABLE_FLAGS,	/* Use Struct: rflm_dtr_tx_block_valid_flags_struct */
  RFLM_DTR_TX_INDICES,	/* Use Struct: rflm_dtr_tx_indices_struct */
  TXLM_GROUP_NUM,
  TXLM_GROUP_INVALID
}txlm_dyn_group_type;



typedef enum
{
  RFLM_DTR_TX_CHIPSET_ID_CHIPSET_CHILTEPIN,
  RFLM_DTR_TX_CHIPSET_ID_NUM,
  RFLM_DTR_TX_CHIPSET_ID_INVALID
}rflm_dtr_tx_enum_chipset_id;

typedef enum
{
  RFLM_DTR_TX_LTE_LTE_BW5MHZ_WTR_SAW_WTR4905,
  RFLM_DTR_TX_LTE_LTE_BW3MHZ_WTR_SAW_WTR4905,
  RFLM_DTR_TX_LTE_LTE_BW1p4MHZ_WTR_SAW_WTR4905,
  RFLM_DTR_TX_LTE_LTE_BW5MHZ_SAWLESS_WTR4905,
  RFLM_DTR_TX_LTE_LTE_BW3MHZ_SAWLESS_WTR4905,
  RFLM_DTR_TX_LTE_LTE_BW1p4MHZ_SAWLESS_WTR4905,
  RFLM_DTR_TX_DP_PEQ_SW_DYNAMIC_SETTINGS_NUM,
  RFLM_DTR_TX_DP_PEQ_SW_DYNAMIC_SETTINGS_INVALID
}rflm_dtr_tx_dp_peq_sw_dynamic_settings_enum_type;

typedef enum
{
  RFLM_DTR_TX_DP_PEQ,
  RFLM_DTR_TX_DYN_FUNCTIONALITY_NUM,
  RFLM_DTR_TX_DYN_FUNCTIONALITY_INVALID
}rflm_dtr_tx_dyn_functionality_type;

typedef struct
{
	txlm_chain_type chain;
	boolean et_slave;
	rflm_dtr_tx_enum_chipset_id chipset_id;
}txlm_dac_cal_config_type;

typedef enum
{
   /* Write enums */
   TXLM_DAC_CAL_CMD_WRITE_DAC_CALIB_CMDS_C_0_0_MSBCAL_MEM_READ, /* Trigger to read DACc internal memory. */
   TXLM_DAC_CAL_CMD_WRITE_DAC_CALIB_CMDS_C_0_0_MSBCAL_MEM_WRITE, /* Trigger to write DACc internal memory. */
   TXLM_DAC_CAL_CMD_WRITE_DAC_CALIB_CMDS_C_0_0_TX_CFG_START, /* Trigger MSB Calibration on DACc */

   /* Read enums */
   TXLM_DAC_CAL_CMD_READ_DAC_CALIB_STATUS_C_0_0_MSBCAL_MEM_RD_DONE, /* Indicate DACc internal memory read done */
   TXLM_DAC_CAL_CMD_READ_DAC_CALIB_STATUS_C_0_0_MSBCAL_MEM_WR_DONE, /* Indicate DACc internal memory write done */
   TXLM_DAC_CAL_CMD_READ_DAC_CALIB_STATUS_C_0_0_TX_ALL_DONE, /* Indicates all calibrations is done. */

   TXLM_DAC_CAL_CMD_NUM,
   TXLM_DAC_CAL_CMD_INVALID
}txlm_dac_cal_cmd_enum_type;


#ifdef __cplusplus
}
#endif



#endif


