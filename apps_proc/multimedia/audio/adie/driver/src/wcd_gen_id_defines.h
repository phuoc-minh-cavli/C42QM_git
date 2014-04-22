#ifndef WCD_GEN_ID_DEFINES_INTF_
#define WCD_GEN_ID_DEFINES_INTF_

/**
@file wcd_gen_id_defines.h
@brief 
Header file for generated element type ids and element ids of wcd.
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/wcd_gen_id_defines.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/13/13   AG      Added new speaker PA element type
03/01/13   KNM     Initial revision.
===============================================================================*/

/**********************************************************************************
 *******************                  TYPE IDS                  *******************
 *********************************************************************************/
#define WCD_MUX_TYPE_ID                        1 
#define WCD_MIXER_TYPE_ID                      2 
#define WCD_HPF_TYPE_ID                        3 
#define WCD_RX_PATH_TYPE_ID                    4 
#define WCD_TX_PATH_TYPE_ID                    5 
#define WCD_BUS_PORT_TYPE_ID                   6 
#define WCD_ADC_TYPE_ID                        7 
#define WCD_DMIC_TYPE_ID                       8 
#define WCD_EAR_PA_TYPE_ID                     9 
#define WCD_RX_BIAS_TYPE_ID                   10 
#define WCD_DAC_TYPE_ID                       11 
#define WCD_CLASS_CONTROLLER_TYPE_ID          12 
#define WCD_MICBIAS_MANAGER_TYPE_ID           13 
#define WCD_IIR_TYPE_ID                       14 
#define WCD_SAMPLE_RATE_CONVERTER_TYPE_ID     15 
#define WCD_COMPANDER_TYPE_ID                 16 
#define WCD_ANC_CHANNEL_TYPE_ID               17 
#define WCD_SPKR_PA_TYPE_ID                   18 

/**********************************************************************************
 *******************                 ELEMENT IDS                *******************
 *********************************************************************************/
#define WCD_SBRX_1_ID 		                  1 
#define WCD_SBRX_2_ID 		                  2 
#define WCD_SBRX_3_ID 		                  3 
#define WCD_SBRX_4_ID 		                  4 
#define WCD_SBRX_5_ID 		                  5 
#define WCD_RDAC_1_MUX_ID 		              6 
#define WCD_EAROUT_ID 		                  7 
#define WCD_RX_1_MAIN_MUX_1_ID 		          8 
#define WCD_RX_1_MAIN_MUX_2_ID 		          9 
#define WCD_RX_1_MAIN_MUX_3_ID 		         10 
#define WCD_RX_1_MAIN_MIXER_ID 		         11 
#define WCD_RX_1_ST_MIXER_ID 		         12 
#define WCD_RX_1_ST_MUX_1_ID 		         13 
#define WCD_RX_1_ST_MUX_2_ID 		         14 
#define WCD_RX_1_HPF_ID 		             15 
#define WCD_RX_1_PATH_ID 		             16 
#define WCD_RX_1_COMPANDER_ID 		         17 
#define WCD_CLASS_CONTROLLER_1_ID 		     18 
#define WCD_RX_BIAS_1_ID 		             19 
#define WCD_HPHL_ID 		                 20 
#define WCD_RX_2_MAIN_MUX_1_ID 		         21 
#define WCD_RX_2_MAIN_MUX_2_ID 		         22 
#define WCD_RX_2_MAIN_MUX_3_ID 		         23 
#define WCD_RX_2_MAIN_MIXER_ID 		         24 
#define WCD_RX_2_ST_MUX_1_ID 		         25 
#define WCD_RX_2_ST_MUX_2_ID 		         26 
#define WCD_RX_2_ST_MIXER_ID 		         27 
#define WCD_RX_2_HPF_ID 		             28 
#define WCD_RX_2_PATH_ID 		             29 
#define WCD_RX_2_COMPANDER_ID 		         30 
#define WCD_CLASS_CONTROLLER_2_ID 		     31 
#define WCD_RX_BIAS_2_ID 		             32 
#define WCD_RDAC_3_MUX_ID 		             33 
#define WCD_HPHR_ID 		                 34 
#define WCD_RX_3_MAIN_MUX_1_ID 		         35 
#define WCD_RX_3_MAIN_MUX_2_ID 		         36 
#define WCD_RX_3_MAIN_MUX_3_ID 		         37 
#define WCD_RX_3_MAIN_MIXER_ID 		         38 
#define WCD_RX_3_HPF_ID 		             39 
#define WCD_RX_3_PATH_ID 		             40 
#define WCD_RX_4_MAIN_MUX_1_ID 		         41 
#define WCD_RX_4_MAIN_MUX_2_ID 		         42 
#define WCD_RX_4_MAIN_MUX_3_ID 		         43 
#define WCD_RX_4_MAIN_MIXER_ID 		         44 
#define WCD_RX_4_ST_MUX_1_ID 		         45 
#define WCD_RX_4_ST_MUX_2_ID 		         46 
#define WCD_RX_4_ST_MIXER_ID 		         47 
#define WCD_RX_4_HPF_ID 		             48 
#define WCD_RX_4_PATH_ID 		             49 
#define WCD_MBIAS_MGR_1_ID 		             50 
#define WCD_DMIC_1_ID 		                 51 
#define WCD_MBIAS_MGR_2_ID 		             52 
#define WCD_DMIC_2_ID 		                 53 
#define WCD_MBIAS_MGR_3_ID 		             54 
#define WCD_DMIC_3_ID 		                 55 
#define WCD_MBIAS_MGR_4_ID 		             56 
#define WCD_DMIC_4_ID 		                 57 
#define WCD_MBIAS_MGR_5_ID 		             58 
#define WCD_ADC_1_ID 		                 59 
#define WCD_MBIAS_MGR_6_ID 		             60 
#define WCD_MBIAS_MGR_7_ID 		             61 
#define WCD_MBIAS_MGR_8_ID 		             62 
#define WCD_ADC_2_ID 		                 63 
#define WCD_TX_PATH_MUX_1_ID 		         64 
#define WCD_TX_PATH_HPF_1_ID 		         65 
#define WCD_TX_PATH_1_ID 		             66 
#define WCD_TX_PATH_MUX_2_ID 		         67 
#define WCD_TX_PATH_MUX_3_ID 		         68 
#define WCD_TX_PATH_MUX_4_ID 		         69 
#define WCD_TX_PATH_HPF_2_ID 		         70 
#define WCD_TX_PATH_2_ID 		             71 
#define WCD_TX_PATH_HPF_3_ID 		         72 
#define WCD_TX_PATH_HPF_4_ID 		         73 
#define WCD_TX_PATH_3_ID 		             74 
#define WCD_TX_PATH_4_ID 		             75 
#define WCD_SBTX_MUX_1_ID 		             76 
#define WCD_SBTX_1_ID 		                 77 
#define WCD_SBTX_MUX_2_ID 		             78 
#define WCD_SBTX_2_ID 		                 79 
#define WCD_SBTX_MUX_3_ID 		             80 
#define WCD_SBTX_MUX_4_ID 		             81 
#define WCD_SBTX_MUX_5_ID 		             82 
#define WCD_SBTX_3_ID 		                 83 
#define WCD_SBTX_4_ID 		                 84 
#define WCD_SBTX_5_ID 		                 85 
#define WCD_RX_4_INTERPOLATOR_MIXER_ID 		 86 
#define WCD_RX_3_COMPANDER_ID 		         87 
#define WCD_RX_BIAS_3_ID 		             88 
#define WCD_RDAC_4_MUX_ID 		             89 
#define WCD_LO1_L_P_ID 		                 90 
#define WCD_RX_4_COMPANDER_ID 		         91 
#define WCD_CLASS_CONTROLLER_4_ID 		     92 
#define WCD_RX_BIAS_4_ID 		             93 
#define WCD_SPEAKER_ID 		                 94 
#define WCD_RDAC_5_DIFF_MUX_ID 		         95
#define WCD_LO2_R_M_ID 		                 96
#define WCD_USONIC_MUX_ID 		             97 
#define WCD_ADC_3_ID 		                 98 
#define WCD_ADC_4_ID 		                 99 
#define WCD_MBIAS_MGR_9_ID 		            100 
#define WCD_ADC_5_ID 		                101 
#define WCD_ANC_CHANNEL_1_ID 		        102 
#define WCD_ANC_CHANNEL_2_ID 		        103 

#endif
