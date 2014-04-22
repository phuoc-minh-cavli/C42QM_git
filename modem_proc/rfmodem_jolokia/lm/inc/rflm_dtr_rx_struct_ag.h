
#ifndef RFLM_DTR_RX_STRUCT_AG_H
#define RFLM_DTR_RX_STRUCT_AG_H


#ifdef __cplusplus
extern "C" {
#endif

/*
WARNING: This file is auto-generated.

Generated at:    Thu Dec 26 15:11:44 2019
Generated using: lm_autogen.pl v5.1.37
Generated from:  v7.3.3 of Jolokia_RxFE_Register_Settings.xlsx
*/

/*=============================================================================

           R X    A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the modem register settings 
  configured by FW, provided by the rflm_dtr_rx.

Copyright (c) 2009, 2010, 2011, 2012, 2013, 2014, 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

$DateTime: 2020/04/16 12:27:55 $
$Header: //components/rel/rfmodem_jolokia.mpss/1.10/lm/inc/rflm_dtr_rx_struct_ag.h#1 $

=============================================================================*/

/*=============================================================================
                           REVISION HISTORY
Version    Author   Date   
         Comments   IR Number   Other POC (Comma Delimited)   Release Validation Notes   Dependencies   AG File Location   
Jolokia 6.4.13; Tabasco 7.3.3   xiangzou   7/31/2017   
         Changed the truncation bits in NB_IoT_HighSpeed Columnfrom 4 to 3 since the icifir adds up to 256 and 256= 2^(5+3) is needed to normalize the dc gain. By the SWI definition,  truncation bit = ICIFIR_TRUNCBITS +5                  
Jolokia 6.4.13; Tabasco 7.3.3   xiangzou   3/23/2017   
         NB_IoT_HighSpeed Column in JOLOKIA_bbrx_config added                  
Jolokia 6.4.13; Tabasco 7.3.3   xiangzou   3/16/2017   
         Fixed the WB not enabled in SS                  
Jolokia 6.4.13; Tabasco 7.3.3   xiangzou   3/7/2017   
         Correct the flag name in  NB_IoT_HighSpeed Column                  
Jolokia 6.4.13; Tabasco 7.3.3   xiangzou   3/1/2017   
         Set DEC_FACTOR to 1 in common_state_reg_settings for NB_IoT_HighSpeed Column                  
Jolokia 6.4.13; Tabasco 7.3.3   xiangzou   2/26/2017   
         NB-IoT_HS column is added                  
Jolokia 6.4.13; Tabasco 7.3.3   xiangzou   9/26/2016   
         fixed a few errors in CSR related parameters in common_state_reg_settings                   
Jolokia 6.4.13; Tabasco 7.3.3   xiangzou   9/23/2016   
         Set SWAP_IQ to 1 in common_state_reg_settings                   
Jolokia 6.4.13; Tabasco 7.3.3   xiangzou   9/21/2016   
         NB-IoT column is added                  
Jolokia 6.4.12; Tabasco 7.3.3   gatesj   6/16/2015   
         1. Included type definition for modem specific tabs - corrects RxLM vars types 2. Renamed Jolokia BBRx settings to align with Tabasco naming      lmiao@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.12/ https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.3.3/   
Jolokia 6.4.11; Tabasco 7.3.2   gatesj   6/5/2015   
         1. Added support for Eldarion/Jacala ADC registers using overrides 2. Added modem specific support to WB2/Writer4 debug writes for Lykan support      djoffe@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.11/ https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.3.2/   
Jolokia 6.4.10; Tabasco 7.3.1   gatesj   5/11/2015   
         1.TDS WBDC K=4 for TRK and IDLE modes      huanx@qti.qualcomm.com, ccmercad@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.10/ https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.3.1/   
Jolokia 6.4.9; Tabasco 7.3.0   gatesj   4/28/2015   
         1. FTM mode variables LTE_K_NORM and LTE_NBEE renamed for tabasco, to include single Rx support.  2. Single Rx SW dynamic tab updated with FTM mode variables 3. Single Rx CA information added to the NBPWR SW dynamic tab 4. Dec8 blocks now runtime dynamic 5. NB2 added to NBPWR SW dynamic tab 6. NB2 NBPWR registers now SW dynamic      vboda@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.9/ https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.3.0/   
Jolokia 6.4.9; Tabasco 7.2.1   gatesj   4/21/2015   
         1. Updated common Dec8 for Tabasco. Block groups split into DEC8_FILT0 and DEC8_FILT1. Block valid flags adjusted.      djoffe@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.9/ https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.2.1/   
Jolokia 6.4.9; Tabasco 7.2.0   gatesj   4/17/2015   
         1. NB3 buffer included for Tabasco pairings only. Single-Rx CA tab updated. 2. Always Armed bit set to 0 for WBPWR, NBPWR, DVGA before WB and NB clears 3. Tabasco HPM (Eldarion) BBRx settings populated      ccmercad@qti.qualcomm.com, yusong@qti.qualcomm.com, vboda@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.9/ https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.2.0/   
Jolokia 6.4.8; Tabasco 7.1.8   gatesj   4/6/2015   
         1. New LTE_ADC_SW_Dynamic tab added to handle dynamic ADC rates, previous states removed. 2. ICIFIR_SW_Dynamic tab added for WTR based ICIFIR selection, currently only for TDS, LTE15, and LTE20 3. VSRC and ADC CFG registers are now SW dynamic      mlane@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.8/ https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.1.8/   
Jolokia 6.4.7; Tabasco 7.1.7   gatesj   4/1/2015   
         1. BBRx change in release 6.4.6 for LTE20 removed.  2. New Tabasco RxLM state for B7/B41/B40 to adjust ADC rate.       wangyan@qti.qualcomm.com, mlane@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.7/ https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.1.7/   
Jolokia 6.4.6; Tabasco 7.1.6   gatesj   3/30/2015   
         1. BBRx settings updated for GSM, WCDMA, TDS, and C2K/DO.      wangyan@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.6/ https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.1.6/   
Jolokia 6.4.5; Tabasco 7.1.5   gatesj   3/4/2015   
         1. Corrected ICIFIR length for GSM IP2 state      tslocomb@qti.qualcomm.com, ppachett@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.5/ https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.1.5/   
Jolokia 6.4.4; Tabasco 7.1.4   gatesj   3/3/2015   
         1. Updated ICIFIR coefficients for GSM IP2 cal state to match those in DPM      tslocomb@qti.qualcomm.com, ppachett@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.4/ https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.1.4/   
Jolokia 6.4.3; Tabasco 7.1.3   gatesj   2/27/2015   
         1. Single Rx LTE CA hooks added in CA mode tab.  2. Single Rx LTE CA dynamic ICIFIR tab included. 3. ADC Vcm cal state included 4. NB_DEC groups are now SW dynamic      kbaskara@qti.qualcomm.com, djoffe@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.3/ https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.1.3/   
Jolokia 6.4.2; Tabasco 7.1.2   gatesj   12/11/2014   
         1. Update WCDMA fast raw scan NB index      harishv@qti.qualcomm.com, vishnun@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.2/ https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.1.2/   
Jolokia 6.4.1; Tabasco 7.1.1   gatesj   12/4/2014   
         1. New state for WCDMA fast raw scan 2. ICIFIR updated for 1x/DO, provided by 1x team      harishv@qti.qualcomm.com, vishnun@qti.qualcomm.com, ndhar@qti.qualcomm.com, ruhuah@qti.qualcomm.com, sekin@qti.qualcomm.com   1. Verified in CSIM 2. Verified by 1x team on target and in CSIM      https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.1/ https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.1.1/   
Jolokia 6.4.0; Tabasco 7.1.0   gatesj   11/10/2014   
         1. Included the NBPWR configuration for LTE states in the nbpwr_SW_Dynamic tab 2. Included two new RxLM Variables for LTE_K_NORM_FACTOR and LTE_K_NBEE_TO_DBV. All scale factors now int32. 3. Renamed COMMON_NOTCH_02 block name to COMMON_NOTCH_23      bhiremat@qti.qualcomm.com, itkachov@qti.qualcomm.com   1 & 2. Verified on DPM      https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.4.0/ https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.1.0/   
Jolokia 6.3.3; Tabasco 7.0.2   gatesj   10/28/2014   
         1. Updated ideal CSR values for Tabasco states: UMTS3C, LTECA               https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.3.3/ https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.0.1/   
Jolokia 6.3.3; Tabasco 7.0.1   gatesj   10/28/2014   
         1. Correction to the Tabasco notch CMD registers.                https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.3.3/ https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.0.1/   
Jolokia 6.3.3; Tabasco 7.0.0   gatesj   10/27/2014   
         1. Initial updates for Tabasco checkin. Includes all new states and registers. Placeholders for HPm BBRx settings.                https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.3.3/ https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/7.0.0/   
6.3.3   gatesj   10/21/2014   
         1. UMTS DSR FET NB has imm trigger removed      aojha@qti.qualcomm.com, carstenp@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.3.3/   
6.3.2   gatesj   10/20/2014   
         1. UMTS DSR state now has an extra NB for FET      aojha@qti.qualcomm.com, carstenp@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.3.2/   
6.3.1   gatesj   10/20/2014   
         1. WTR2955 added to the TRX RxLM parameter for NBR state      vsundar@qti.qualcomm.com, aojha@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.3.1/   
6.3.0   gatesj   9/17/2014   
         1. Updated WBDC scale factors for LTE states 2. Updated UMTS NBR state to include expected CSR values 3. ADC_CFG block now runtime dynamic - requires interface change 4. New RxLM Variable NOTCH_SAMPLING_RATE - requires interface change 5. IQMC fields now set to DIRECT_WRITE 6. LTE20 and LTE15 states modified to increase VSRC output rate to 122.88MHz to avoid DC images falling in band     a) Dec0 disabled     b) Dec2 enabled     c) Scaling factors updated     d) Delays updated     e) ICIFIR updated     f) VSRC regulator bypassed 7. 1x NBPWR mask set to 4095, 128 cx1 samples, to match the Bolt code used in Jolokia   IR-028568   mhoffman@qti.qualcomm.com, sekin@qti.qualcomm.com, jpaul@qti.qualcomm.com   1. Verified on DPM  2. Verified on DPM 3. Verified on DPM 4. Verified on DPM 6. Verified on DPM 7. Verified on RUMI by C2K team   7. CR726173   https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.3.0/   
6.2.4   gatesj   8/11/2014   
         1. BBRx register settings updated. 1605/2605 references removed from bbrx_config tab. LTE40 mode added.  2. WBDC LSHIFT_UPDT_CMD set for all LTE modes.  3. GSM state now uses Dec7 in mode 1. ICIFIR updated.    IR-021714   wangyan@qti.qualcomm.com   2. Verified on DPM      https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.2.4/   
6.2.3   gatesj   7/28/2014   
         1. CSR_IDEAL_VAL_C0 and CSR_IDEAL_VAL_C1 updated for UMTS2C state to now contain the 2.5MHz and -2.5MHz rotator values.    IR-019381   rllynch@qti.qualcomm.com, carstenp@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.2.3/   
6.2.2   gatesj   7/27/2014   
         1. GSM IP2 cal state changes (rotator location/frequency, ICIFIR) 2. STMR_MHZ_FS modifed for 1x/HDR states correcting the STMR_FCW RFLM values.  3. Modified the triggering of the FET NB, followup to change 2 in 5.3.5. Now uses imm_trig instead of enable/start_stop   IR-019089   vijayr@qti.qualcomm.com   1. Verified by the G team on DimePM      https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.2.2/   
6.2.1   gatesj   7/10/2014   
         1. GSM VSRC SCTR wrap around doubled 2. TDS TRK IRAT WBDC parameter K2 set to 7 (previously 6). 3. IQ Swap set to 1 for TDS states.  4. UMTS 2C ICIFIR updated. 5. UMTS SC state has the FET NB disabled by default.   IR-016445   bcanpola@qti.qualcomm.com, yclin@qti.qualcomm.com, yingxuel@qti.qualcomm.com, carstenp@qti.qualcomm.com   1. Validated by the G team on Bolt 2. Validated by the T team on DimePM 4. Validated by the W team on DimePM 5. Validated by the W team on DimePM      https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.2.1/   
6.2.0   gatesj   6/27/2014   
         1. IQ_SWAP values set to 1 for LTE demod chain output, 1x/DO outputs, UMTS outputs. 2. Updated BBRx configuration registers to new recommended values.  3. bbrx_fuse_SW_Dynamic tab included for BBRx RSB compensation. 4. NB blocks are now runtime dynamic for CA - requires interface change.    IR-014276   mhoffman@qti.qualcomm.com, sekin@qti.qualcomm.com, carstenp@qti.qualcomm.com, wangyan@qti.qualcomm.com, cdeshmuk@qti.qualcomm.com, bow@qti.qualcomm.com, djoffe@qti.qualcomm.com         https://sharepoint.qualcomm.com/qct/Modem-Tech/Projects/Jolokia/Systems/Common/RF%20Interfaces/RxFE_AG_Archive/6.2.0/   
6.1.1   gatesj   6/5/2014   
         1. Separate states on 4905 for LTE1.4,3,5 to account for WTR pole differences and ICIFIR changes. No SW changes required.  2. VSRC_WA updated to FW Export dynamicity. VSRC subgroup reordering to allow for this.  3. UMTS_1C state now has 2 NB, to enable FET NB mirroring.  4. New UMTS_ACQ state - copy of the 1C state for acquisition with only 1 NB.                  
6.1.0   gatesj   5/7/2014   
         1. TDS ACQ output format changed to 16SC15 through SROUND2, PBS_CFG updated 2. WBDC/WBPWR/NBPWR scale factors now have csim tags.  3. Included WBDC_OFFSET variable, requires interface change.  4. All WBPWR/WBDC/NBPWR offset/scale factors are now Q24 format. 5. GSM output format now defaults to 16SC15 through SSAT2 instead of SROUND2.  6. The TDS ACQ  state now has PBS_SAMPX_SEL updated to 5 to account for the decimation in the writer. 7. BBRx configuration registers updated. 8. ADC insertion loss deltas updated.      yingxuel@qti.qualcomm.com            
6.0.1   gatesj   4/16/2014   
         1. Corrected UMTS2C ICIFIR truncbits calculation  2. Corrected TDS ACQ output format.  3. Addition of the UMTS NBR state. 4. Addition of the nbpwr_SW_Dynamic tab containing UMTS NBPWR Cal information.  5. Modified the ADC_INSERTION_LOSS_DELTA values set to zero, but will use Q24 format in the future if needed.  6. All LTE searcher NB modified p2=4, to reduce the wraparound to 12288 samples, 6.4ms. 7. ROUND_ENABLE register renamed to ROUND_SAT_ENABLE. Bit widths corrected.       lopezm@qti.qualcomm.com            
6.0.0   gatesj   3/27/2014   
         1. Branched from DimePM RxLM 5.2.5 2. Removed 1605/2605 specific states. Added 2955 TRX strings to 4905 states for compatibility.  3. Added PBS_WIDE32_WORD_EN register. Set to 1 for states with 16SC15 output.  4. Added RXFE_NB_FINAL_SWAP_NBn to each NB. None currently set.  5. Replaced DimePM modem strings with Jolokia.                  

=============================================================================*/
/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "rflm_dtr_rx_fw_intf_ag.h" 
#include "rflm_dtr_rx_typedef_ag.h" 



/* This struct captures flags that control whether certain groups of registers are programmed for a given state. */
typedef struct
{
  boolean rxfe_enable_flag;/* This flag enables programming of the RXFE ENABLE*/
  boolean mtc_trig_flag;/* This flag enables programming of the MTC registers to program the STMR trigger for URxFE WB chain 0*/
  boolean adc_cfg_flag;/* This flag enables programming of the Bolt registers to program ADC sampling clock*/
  boolean rxfe_top_flag;/* This flag enables programming of the ADC RXFE DBG mode registers and ACI filters*/
  boolean urxfe_wb_flag;/* This flag enables programming of the URxFE registers to program a WB chain.*/
  boolean urxfe_nb0_flag;/* This flag enables programming of the URxFE registers to program the first NB chain in a NB group.*/
  boolean urxfe_nb1_flag;/* This flag enables programming of the URxFE registers to program the second NB chain*/
}rflm_dtr_rx_block_valid_flags_struct;



 /* Group RXFE_ENABLE: */ 
/* This block consist of the RXFE ENABLE*/
typedef struct
{
  uint32 rxfe_enable;
}rflm_dtr_rx_rxfe_enable_group_struct;



 /* Group MTC_TRIG: */ 
/* This block consist of the MTC registers to program the STMR trigger for URxFE WB chain 0*/
typedef struct
{
  uint32 rxfe_cn_trig_cmd;
  uint32 rxfe_cn_trig_val;
}rflm_dtr_rx_mtc_trig_group_struct;



 /* Group ADC_CLK_CFG: */ 
/* This block consist of the Bolt registers to program ADC sampling clock*/
typedef struct
{
  uint32 mss_adc_cfg_rcgr;
  uint32 mss_adc_cmd_rcgr;
  uint32 mss_adc_cbcr;
}rflm_dtr_rx_adc_clk_cfg_group_struct;



 /* Group ADC_CONFIG: */ 
/* This block consist of the Bolt registers to program ADC sampling clock*/
typedef struct
{
  uint32 mss_atop_reg1;
  uint32 mss_atop_reg2;
  uint32 mss_atop_reg3;
  uint32 mss_dtop_reg1;
  uint32 mss_dtop_reg2;
  uint32 rxfe_adca_logic_cfg;
}rflm_dtr_rx_adc_config_group_struct;



 /* Group TOP_DBG: */ 
/* This block consist of the ADC RXFE DBG mode registers and ACI filters*/
typedef struct
{
  uint32 rxfe_dbg_enables;
  uint32 rxfe_dbg_probe_ctl;
  uint32 rxfe_dbg_cmd;
}rflm_dtr_rx_top_dbg_group_struct;



 /* Group PBS_CFG: */ 
/* This block consist of the ADC RXFE DBG mode registers and ACI filters*/
typedef struct
{
  uint32 rxfe_pbs_cfg;
}rflm_dtr_rx_pbs_cfg_group_struct;



 /* Group RFIF_UPDATED: */ 
/* This block consist of the ADC RXFE DBG mode registers and ACI filters*/
typedef struct
{
  uint32 rxfe_rfif_update_cmd;
}rflm_dtr_rx_rfif_updated_group_struct;



 /* Group AA_WB: */ 
/* This block consist of the URxFE registers to program a WB chain.*/
typedef struct
{
  uint32 rxfe_wb_wbpwr_cfg_wbw;
}rflm_dtr_rx_aa_wb_group_struct;



 /* Group TOP_WB_v0: */ 
/* This block consist of the URxFE registers to program a WB chain.*/
typedef struct
{
  uint32 rxfe_wb_top_ctl_wbw;
}rflm_dtr_rx_top_wb_v0_group_struct;



 /* Group WB_ADCMUX: */ 
/* This block consist of the URxFE registers to program a WB chain.*/
typedef struct
{
  uint32 rxfe_wbw_adcmux_cfg;
}rflm_dtr_rx_wb_adcmux_group_struct;



 /* Group TOP_WB_CMD: */ 
/* This block consist of the URxFE registers to program a WB chain.*/
typedef struct
{
  uint32 rxfe_wb_stopgate_action_sample_wbw;
  uint32 rxfe_wb_top_cmd_wbw;
}rflm_dtr_rx_top_wb_cmd_group_struct;



 /* Group WB_PWR: */ 
/* This block consist of the URxFE registers to program a WB chain.*/
typedef struct
{
  uint32 rxfe_wb_wbpwr_start_mask_wbw;
  uint32 rxfe_wb_wbpwr_cfg_wbw;
  uint32 rxfe_wb_wbpwr_start_action_sample_wbw;
  uint32 rxfe_wb_wbpwr_stop_action_sample_wbw;
  uint32 rxfe_wb_wbpwr_cmd_wbw;
}rflm_dtr_rx_wb_pwr_group_struct;



 /* Group WBDC: */ 
/* This block consist of the URxFE registers to program a WB chain.*/
typedef struct
{
  uint32 rxfe_wb_wbdc_cfg_wbw;
  uint32 rxfe_wb_wbdc_update_action_sample_wbw;
  uint32 rxfe_wb_wbdc_start_action_sample_wbw;
  uint32 rxfe_wb_wbdc_stop_action_sample_wbw;
  uint32 rxfe_wb_wbdc_cmd_wbw;
  uint32 rxfe_wb_wbdc_update_delay_wbw;
  uint32 rxfe_wb_wbdc_ovrd_i_wbw;
  uint32 rxfe_wb_wbdc_ovrd_q_wbw;
  uint32 rxfe_wb_wbdc_est1_load_i_wbw;
  uint32 rxfe_wb_wbdc_est1_load_q_wbw;
  uint32 rxfe_wb_wbdc_est2_load_i_wbw;
  uint32 rxfe_wb_wbdc_est2_load_q_wbw;
}rflm_dtr_rx_wbdc_group_struct;



 /* Group VSRC: */ 
/* This block consist of the URxFE registers to program a WB chain.*/
typedef struct
{
  uint32 rxfe_wb_vsrc_cfg_wbw;
  uint32 rxfe_wb_vsrc_t2byt1m1_wbw;
  uint32 rxfe_wb_vsrc_updt_action_sample_wbw;
  uint32 rxfe_wb_vsrc_sctr_load_value_wbw;
  uint32 rxfe_wb_vsrc_cmd_wbw;
}rflm_dtr_rx_vsrc_group_struct;



 /* Group WB_DEC: */ 
/* This block consist of the URxFE registers to program a WB chain.*/
typedef struct
{
  uint32 rxfe_wb_dec_filt_cfg_wbw;
}rflm_dtr_rx_wb_dec_group_struct;



 /* Group IQMC_A_B_COEFFS: */ 
/* This block consists of all notch related registers in a WB chain*/
typedef struct
{
  uint32 rxfe_wb_iqmc_cfg1_wbw;
}rflm_dtr_rx_iqmc_a_b_coeffs_group_struct;



 /* Group IQMC: */ 
/* This block consists of all notch related registers in a WB chain*/
typedef struct
{
  uint32 rxfe_wb_iqmc_cfg0_wbw;
  uint32 rxfe_wb_iqmc_updt_action_sample_wbw;
  uint32 rxfe_wb_iqmc_updt_cmd_wbw;
}rflm_dtr_rx_iqmc_group_struct;



 /* Group NOTCH: */ 
/* This block consists of all notch related registers in a WB chain*/
typedef struct
{
  uint32 rxfe_wb_notch_filtn_cfg0_wbw;
  uint32 rxfe_wb_notch_filtn_cfg1_wbw;
  uint32 rxfe_wb_notch_filt012_cfg_wbw;
}rflm_dtr_rx_notch_group_struct;



 /* Group NB_WBMUX_NB: */ 
/* This block consist of the URxFE registers to program the first NB chain in a NB group.*/
typedef struct
{
  uint32 rxfe_nbn_wbmux_cfg;
}rflm_dtr_rx_nb_wbmux_nb_group_struct;



 /* Group AA_NB: */ 
/* This block consist of the URxFE registers to program the first NB chain in a NB group.*/
typedef struct
{
  uint32 rxfe_nb_nbpwr_cfg_nbn;
}rflm_dtr_rx_aa_nb_group_struct;



 /* Group TOP_NB_v0: */ 
/* This block consist of the URxFE registers to program the first NB chain in a NB group.*/
typedef struct
{
  uint32 rxfe_nb_top_ctl_nbn;
}rflm_dtr_rx_top_nb_v0_group_struct;



 /* Group NB_GATE_NB: */ 
/* This block consist of the URxFE registers to program the first NB chain in a NB group.*/
typedef struct
{
  uint32 rxfe_nb_gate_action_sample_nbn;
  uint32 rxfe_nb_gate_cmd_nbn;
}rflm_dtr_rx_nb_gate_nb_group_struct;



 /* Group NB_DEC_NB: */ 
/* This block consist of the URxFE registers to program the first NB chain in a NB group.*/
typedef struct
{
  uint32 rxfe_nb_filters_cfg_nbn;
}rflm_dtr_rx_nb_dec_nb_group_struct;



 /* Group ICIFIR_NB: */ 
/* This block consist of the URxFE registers to program the first NB chain in a NB group.*/
typedef struct
{
  uint32 rxfe_nb_filters_cfg_nbn;
  uint32 rxfe_nb_ici_coefc_nbn[11];
}rflm_dtr_rx_icifir_nb_group_struct;



 /* Group NB_DC_NB: */ 
/* This block consist of the URxFE registers to program the first NB chain in a NB group.*/
typedef struct
{
  uint32 rxfe_nb_filters_cfg_nbn;
  uint32 rxfe_nb_nbdc_compval_i_nbn;
  uint32 rxfe_nb_nbdc_compval_q_nbn;
}rflm_dtr_rx_nb_dc_nb_group_struct;



 /* Group NB_PWR_NB: */ 
/* This block consist of the URxFE registers to program the first NB chain in a NB group.*/
typedef struct
{
  uint32 rxfe_nb_nbpwr_cfg_nbn;
  uint32 rxfe_nb_nbpwr_start_action_sample_nbn;
  uint32 rxfe_nb_nbpwr_start_mask_nbn;
  uint32 rxfe_nb_nbpwr_stop_action_sample_nbn;
  uint32 rxfe_nb_nbpwr_cmd_nbn;
}rflm_dtr_rx_nb_pwr_nb_group_struct;



 /* Group FINAL_BITWIDTHS_NB: */ 
/* This block consist of the URxFE registers to program the first NB chain in a NB group.*/
typedef struct
{
  uint32 rxfe_nb_final_bitwidths_cfg_nbn;
  uint32 rxfe_nb_final_swap_nbn;
}rflm_dtr_rx_final_bitwidths_nb_group_struct;



 /* Group WRITER_NB: */ 
/* This block consist of the URxFE registers to program the first NB chain in a NB group.*/
typedef struct
{
  uint32 rxfe_nb_wtr_cfg_nbn;
  uint32 rxfe_nb_wtr_cmd_nbn;
}rflm_dtr_rx_writer_nb_group_struct;



 /* Group DBG_NB: */ 
/* This block consist of the URxFE registers to program the first NB chain in a NB group.*/
typedef struct
{
  uint32 rxfe_nb_wtr_start_action_sample_nbn;
  uint32 rxfe_nb_wtr_stop_action_sample_nbn;
  uint32 rxfe_nb_wtr_dbg_cfg0_nbn;
  uint32 rxfe_nb_wtr_dbg_cfg1_nbn;
}rflm_dtr_rx_dbg_nb_group_struct;



 /* Group CSR_NB: */ 
/* This block consist of the URxFE registers to program the first NB chain in a NB group.*/
typedef struct
{
  uint32 rxfe_nb_csr_cfg_nbn;
  uint32 rxfe_nb_csr_phaseupdt_action_sample_nbn;
  uint32 rxfe_nb_csr_cmd_nbn;
}rflm_dtr_rx_csr_nb_group_struct;



 /* Group CSR_PH_OFFSET_NB: */ 
/* This block consist of the URxFE registers to program the first NB chain in a NB group.*/
typedef struct
{
  uint32 rxfe_nb_csr_phase_offset_nbn;
}rflm_dtr_rx_csr_ph_offset_nb_group_struct;



 /* Group WTR_FUNC_CFG_NB: */ 
/* This block consist of the URxFE registers to program the first NB chain in a NB group.*/
typedef struct
{
  uint32 rxfe_nb_wtr_func_cfg_nbn;
}rflm_dtr_rx_wtr_func_cfg_nb_group_struct;



 /* Group CSR_FREQ_OFFSET_NB: */ 
/* This block consist of the URxFE registers to program the second NB chain's cordic rotator frequency offset in a NB group.*/
typedef struct
{
  uint32 rxfe_nb_csr_freq_offset_nbn;
  uint32 rxfe_nb_csr_frequpdt_action_sample_nbn;
  uint32 rxfe_nb_csr_cmd_nbn;
}rflm_dtr_rx_csr_freq_offset_nb_group_struct;



// SW FW Interface Buffer

typedef struct ALIGN(32)
{
  rflm_dtr_rx_csr_xo_vars_group_struct csr_xo_vars_params;
  rflm_dtr_rx_xo_vars_group_struct xo_vars_params;
  rflm_dtr_rx_vsrc_xo_vars_group_struct vsrc_xo_vars_params;
  rflm_dtr_rx_stmr_xo_vars_group_struct stmr_xo_vars_params;
  rflm_dtr_rx_delay_vars_group_struct delay_vars_params;
  rflm_dtr_rx_vsrc_vars_group_struct vsrc_vars_params;
  rflm_dtr_rx_adc_vars_group_struct adc_vars_params;
  rflm_dtr_rx_csr_phase_comp_vars_group_struct csr_phase_comp_vars_params;
  rflm_dtr_rx_notch_thresh_group_struct notch_thresh_params;
  rflm_dtr_rx_scale_vars_group_struct scale_vars_params;
  rflm_dtr_rx_rxfe_enable_group_struct rxfe_enable_params;
  rflm_dtr_rx_mtc_trig_group_struct mtc_trig_params;
  rflm_dtr_rx_adc_clk_cfg_group_struct adc_clk_cfg_params;
  rflm_dtr_rx_adc_config_group_struct adc_config_params;
  rflm_dtr_rx_top_dbg_group_struct top_dbg_params;
  rflm_dtr_rx_pbs_cfg_group_struct pbs_cfg_params;
  rflm_dtr_rx_rfif_updated_group_struct rfif_updated_params;
  rflm_dtr_rx_aa_wb_group_struct aa_wb0_params;
  rflm_dtr_rx_top_wb_v0_group_struct top_wb_v0_params;
  rflm_dtr_rx_top_wb_v0_group_struct top_wb_v1_params;
  rflm_dtr_rx_top_wb_v0_group_struct top_wb_v2_params;
  rflm_dtr_rx_wb_adcmux_group_struct wb_adcmux_params;
  rflm_dtr_rx_top_wb_v0_group_struct top_wb_v3_params;
  rflm_dtr_rx_top_wb_cmd_group_struct top_wb_cmd_params;
  rflm_dtr_rx_wb_pwr_group_struct wb_pwr_params;
  rflm_dtr_rx_wbdc_group_struct wbdc_params;
  rflm_dtr_rx_vsrc_group_struct vsrc_params;
  rflm_dtr_rx_vsrc_wa_count_group_struct vsrc_wa_count_params;
  rflm_dtr_rx_wb_dec_group_struct wb_dec_params;
  rflm_dtr_rx_iqmc_a_b_coeffs_group_struct iqmc_a_b_coeffs_params;
  rflm_dtr_rx_iqmc_group_struct iqmc_params;
  rflm_dtr_rx_notch_group_struct notch_params;
  rflm_dtr_rx_nb_wbmux_nb_group_struct nb_wbmux_nb0_params;
  rflm_dtr_rx_aa_nb_group_struct aa_nb0_params;
  rflm_dtr_rx_top_nb_v0_group_struct top_nb0_v0_params;
  rflm_dtr_rx_top_nb_v0_group_struct top_nb0_v1_params;
  rflm_dtr_rx_top_nb_v0_group_struct top_nb0_v2_params;
  rflm_dtr_rx_top_nb_v0_group_struct top_nb0_v3_params;
  rflm_dtr_rx_nb_gate_nb_group_struct nb_gate_nb0_params;
  rflm_dtr_rx_nb_dec_nb_group_struct nb_dec_nb0_params;
  rflm_dtr_rx_icifir_nb_group_struct icifir_nb0_params;
  rflm_dtr_rx_nb_dc_nb_group_struct nb_dc_nb0_params;
  rflm_dtr_rx_nb_pwr_nb_group_struct nb_pwr_nb0_params;
  rflm_dtr_rx_final_bitwidths_nb_group_struct final_bitwidths_nb0_params;
  rflm_dtr_rx_writer_nb_group_struct writer_nb0_params;
  rflm_dtr_rx_dbg_nb_group_struct dbg_nb0_params;
  rflm_dtr_rx_csr_nb_group_struct csr_nb0_params;
  rflm_dtr_rx_csr_ph_offset_nb_group_struct csr_ph_offset_nb0_params;
  rflm_dtr_rx_wtr_func_cfg_nb_group_struct wtr_func_cfg_nb0_params;
  rflm_dtr_rx_csr_freq_offset_nb_group_struct csr_freq_offset_nb0_params;
  rflm_dtr_rx_nb_wbmux_nb_group_struct nb_wbmux_nb1_params;
  rflm_dtr_rx_aa_nb_group_struct aa_nb1_params;
  rflm_dtr_rx_top_nb_v0_group_struct top_nb1_v0_params;
  rflm_dtr_rx_top_nb_v0_group_struct top_nb1_v1_params;
  rflm_dtr_rx_top_nb_v0_group_struct top_nb1_v2_params;
  rflm_dtr_rx_top_nb_v0_group_struct top_nb1_v3_params;
  rflm_dtr_rx_nb_gate_nb_group_struct nb_gate_nb1_params;
  rflm_dtr_rx_nb_dec_nb_group_struct nb_dec_nb1_params;
  rflm_dtr_rx_icifir_nb_group_struct icifir_nb1_params;
  rflm_dtr_rx_nb_dc_nb_group_struct nb_dc_nb1_params;
  rflm_dtr_rx_nb_pwr_nb_group_struct nb_pwr_nb1_params;
  rflm_dtr_rx_final_bitwidths_nb_group_struct final_bitwidths_nb1_params;
  rflm_dtr_rx_writer_nb_group_struct writer_nb1_params;
  rflm_dtr_rx_dbg_nb_group_struct dbg_nb1_params;
  rflm_dtr_rx_csr_nb_group_struct csr_nb1_params;
  rflm_dtr_rx_csr_ph_offset_nb_group_struct csr_ph_offset_nb1_params;
  rflm_dtr_rx_wtr_func_cfg_nb_group_struct wtr_func_cfg_nb1_params;
  rflm_dtr_rx_csr_freq_offset_nb_group_struct csr_freq_offset_nb1_params;
  rflm_dtr_rx_indices_struct rflm_dtr_rx_reg_indices;
  rflm_dtr_rx_block_valid_flags_struct rflm_dtr_rx_block_valid;
  rflm_dtr_rx_header_struct rx_header;
}rflm_dtr_rx_settings_type_ag;



/* This struct captures the different indices used to index into the HWIO registers of the IFreq Delta Buffer. */
typedef struct
{
  uint8 wbw_idx_0;
  uint8 nbn_idx_0;
  uint8 nbn_idx_1;
}rflm_dtr_rx_ifreq_indices_struct;



/* This is the struct type for the IFreq Delta buffer, that will be used incremental to the full DTR Neighbor buffer. */
typedef struct
{
  rflm_dtr_rx_iqmc_a_b_coeffs_group_struct iqmc_a_b_coeffs_params;
  rflm_dtr_rx_csr_ph_offset_nb_group_struct csr_ph_offset_nb0_params;
  rflm_dtr_rx_csr_ph_offset_nb_group_struct csr_ph_offset_nb1_params;
  rflm_dtr_rx_ifreq_indices_struct rflm_dtr_rx_reg_indices;
}rflm_dtr_rx_ifreq_delta_struct;


typedef struct
{
/* Structure type definition containing notch settings and index values for secondary notch settings */
  rflm_dtr_rx_notch_group_struct notch_params;
  rflm_dtr_rx_notch_thresh_group_struct notch_thresh_params;
  rflm_dtr_rx_indices_struct rflm_dtr_rx_reg_indices;


}rflm_dtr_rx_secondary_notch_settings_type;

#ifdef __cplusplus
}
#endif



#endif


