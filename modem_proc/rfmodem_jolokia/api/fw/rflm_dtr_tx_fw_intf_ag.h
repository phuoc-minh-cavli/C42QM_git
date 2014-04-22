
#ifndef RFLM_DTR_TX_FW_INTF_AG_H
#define RFLM_DTR_TX_FW_INTF_AG_H


#ifdef __cplusplus
extern "C" {
#endif

/*
WARNING: This file is auto-generated.

Generated at:    Tue Jul 21 10:29:59 2020
Generated using: lm_autogen_txlm.pl v5.1.37
Generated from:  v7.0.XX of Jolokia_TXLM_register_settings.xlsx
*/

/*=============================================================================

           T X    A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the modem register settings 
  configured by FW, provided by the rflm_dtr_tx.

Copyright (c) 2009, 2010, 2011, 2012, 2013, 2014, 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

$DateTime: 2020/07/28 04:03:43 $
$Header: //components/rel/rfmodem_jolokia.mpss/1.10/api/fw/rflm_dtr_tx_fw_intf_ag.h#2 $

=============================================================================*/


/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "comdef.h" 



/*==============================================================================

Interface Revision Information

==============================================================================*/

#define RFLM_DTR_TX_MAJOR_REV 7
#define RFLM_DTR_TX_FW_INTF_REV 0

/*==============================================================================

Options for CONFIG_MASK

==============================================================================*/

#define RFLM_DTR_TX_STATIC_MASK 0x1 	/* Used to configure the set of registers that will be configured by SW just once; Rest may be updated dynamically from SW and needs to be written as independent blocks*/ 
#define RFLM_DTR_TX_IREF_UPDATE_MASK 0x2 /* This Control Word triggers programming of the DAC IREF registers to program both the IREF gain value for both chains*/ 
#define RFLM_DTR_TX_TXFE_DELAYMATCH_MASK 0x4 /* This Control Word triggers programming of the delay matching registers to program the Tx chain n*/ 
#define RFLM_DTR_TX_TXC_DP_IQ_GAIN_MASK 0x8 /* This Control Word triggers programming of the DP_IQ_GAIN registers to program the Tx chain 0*/ 
#define RFLM_DTR_TX_TXC_TIMESTAMP_MASK 0x10 /* This Control Word triggers programming of the TIMESTAMP registers to program the Tx chain n*/ 
#define RFLM_DTR_TX_ENABLE_DAC 0x20

#define RFLM_DTR_TX_CHAIN_0_FULL_CFG_MASK ( RFLM_DTR_TX_STATIC_MASK | RFLM_DTR_TX_IREF_UPDATE_MASK | RFLM_DTR_TX_TXFE_DELAYMATCH_MASK | RFLM_DTR_TX_TXC_DP_IQ_GAIN_MASK | RFLM_DTR_TX_TXC_TIMESTAMP_MASK | RFLM_DTR_TX_ENABLE_DAC )	/* This control word triggers programming all Tx Chain 0 static and dynamic regsiters in the buffer */
#define RFLM_DTR_TX_CHAIN_1_FULL_CFG_MASK ( RFLM_DTR_TX_STATIC_MASK | RFLM_DTR_TX_IREF_UPDATE_MASK | RFLM_DTR_TX_TXFE_DELAYMATCH_MASK | RFLM_DTR_TX_TXC_DP_IQ_GAIN_MASK | RFLM_DTR_TX_TXC_TIMESTAMP_MASK | RFLM_DTR_TX_ENABLE_DAC )	/* This control word triggers programming all  Tx Chain 1 static and dynamic regsiters in the buffer */




#define RFLM_DTR_TX_UNUSED_INDEX 0xF

/* This struct captures the autogenerated header, for Revision information, etc. */
typedef struct
{
  uint8 major_rev;  /* Indicates Product Line */ 
  uint8 fw_intf_rev;  /* Indicates FW TX Intf Rev */ 
  uint8 sw_settings_rev;  /* Indicates SW Settings Rev for the given FW Intf */ 
}rflm_dtr_tx_header_struct;



/* This struct captures the different indices used to index into the HWIO registers. */
typedef struct ALIGN(4)
{
  uint8 n_idx_0;
  uint8 c_idx_0;
  uint8 d_idx_0;
}rflm_dtr_tx_indices_struct;



 /* Group XO_VARS: */ 
/* This block consist of TXLM variables related to XO*/
typedef struct
{
  uint32 txrot_clk_hz;
  uint32 txrot_ideal_val;
  uint32 txrot_scale_q10;
  uint32 txr_pda_ideal_val;
  uint32 txr_pda_scale_q30;
}rflm_dtr_tx_xo_vars_group_struct;



 /* Group SW_XO_VARS: */ 
/* This block consist of TXLM variables related to XO*/
typedef struct
{
  uint32 inv_f_dl_lo_q19;
  uint32 tx_rx_ratio_q15;
}rflm_dtr_tx_sw_xo_vars_group_struct;



 /* Group DIG_DELAY_VARS: */ 
/* This block consist of TXLM variables related to XO*/
typedef struct
{
  uint32 tx_digital_delay_microsecs_q16;
}rflm_dtr_tx_dig_delay_vars_group_struct;



 /* Group ANALOG_DELAY_VARS: */ 
/* This block consist of TXLM variables related to XO*/
typedef struct
{
  uint32 analog_rf_delay_microsecs_q16;
}rflm_dtr_tx_analog_delay_vars_group_struct;



 /* Group TXAGC_DM_VARS: */ 
/* This block consist of TXLM variables related to XO*/
typedef struct
{
  uint32 dm1_rate_q31;
  uint32 dm1_rate_inv_q24;
  uint32 dm2_rate_q31;
  uint32 dm2_rate_inv_q26;
  uint32 dm3_rate_q26;
  uint32 dm3_rate_inv_q31;
}rflm_dtr_tx_txagc_dm_vars_group_struct;



 /* Group TXLM_VARS: */ 
/* This block consist of TXLM variables related to XO*/
typedef struct
{
  uint32 dac_freq_khz;
  uint32 txc_input_freq_hz;
  uint32 sysbdy_duration_nanosecs;
}rflm_dtr_tx_txlm_vars_group_struct;





typedef enum
{
  RFLM_DTR_TX_FW_GROUP_XO_VARS,	/* Use Struct: rflm_dtr_tx_xo_vars_group_struct */
  RFLM_DTR_TX_FW_GROUP_SW_XO_VARS,	/* Use Struct: rflm_dtr_tx_sw_xo_vars_group_struct */
  RFLM_DTR_TX_FW_GROUP_DIG_DELAY_VARS,	/* Use Struct: rflm_dtr_tx_dig_delay_vars_group_struct */
  RFLM_DTR_TX_FW_GROUP_ANALOG_DELAY_VARS,	/* Use Struct: rflm_dtr_tx_analog_delay_vars_group_struct */
  RFLM_DTR_TX_FW_GROUP_TXAGC_DM_VARS,	/* Use Struct: rflm_dtr_tx_txagc_dm_vars_group_struct */
  RFLM_DTR_TX_FW_GROUP_TXLM_VARS,	/* Use Struct: rflm_dtr_tx_txlm_vars_group_struct */
  RFLM_DTR_TX_INDICES_FW_GROUP,	/* Use Struct: rflm_dtr_tx_indices_struct */
  RFLM_DTR_TX_FW_GROUP_NUM,
  RFLM_DTR_TX_FW_GROUP_INVALID
}rflm_dtr_tx_fw_export_group_id;


#ifdef __cplusplus
}
#endif



#endif


