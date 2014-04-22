#ifndef WCD_GEN_ELEMENTS_INTF_
#define WCD_GEN_ELEMENTS_INTF_

/**
@file wcd_gen_elements.h
@brief 
Header file for generated elements and events of WCD codec.
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/wcd_gen_elements.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/23/13   KNM    Added ANC FB Tune1 & Tune2 routing in Tx path
03/01/13   KNM    Initial revision.
===============================================================================*/

#include "wcd_result.h"
#include "wcd_types.h"

extern wcd_element wcd_SBRX_1;
extern wcd_element wcd_SBRX_2;
extern wcd_element wcd_SBRX_3;
extern wcd_element wcd_SBRX_4;
extern wcd_element wcd_SBRX_5;
extern wcd_element wcd_RDAC_1_MUX;
extern wcd_element wcd_EAROUT;
extern wcd_element wcd_RX_1_MAIN_MUX_1;
extern wcd_element wcd_RX_1_MAIN_MUX_2;
extern wcd_element wcd_RX_1_MAIN_MUX_3;
extern wcd_element wcd_RX_1_MAIN_MIXER;
extern wcd_element wcd_RX_1_ST_MIXER;
extern wcd_element wcd_RX_1_ST_MUX_1;
extern wcd_element wcd_RX_1_ST_MUX_2;
extern wcd_element wcd_RX_1_HPF;
extern wcd_element wcd_RX_1_PATH;
extern wcd_element wcd_RX_1_COMPANDER;
extern wcd_element wcd_CLASS_CONTROLLER_1;
extern wcd_element wcd_RX_BIAS_1;
extern wcd_element wcd_HPHL;
extern wcd_element wcd_RX_2_MAIN_MUX_1;
extern wcd_element wcd_RX_2_MAIN_MUX_2;
extern wcd_element wcd_RX_2_MAIN_MUX_3;
extern wcd_element wcd_RX_2_MAIN_MIXER;
extern wcd_element wcd_RX_2_ST_MUX_1;
extern wcd_element wcd_RX_2_ST_MUX_2;
extern wcd_element wcd_RX_2_ST_MIXER;
extern wcd_element wcd_RX_2_HPF;
extern wcd_element wcd_RX_2_PATH;
extern wcd_element wcd_RX_2_COMPANDER;
extern wcd_element wcd_CLASS_CONTROLLER_2;
extern wcd_element wcd_RX_BIAS_2;
extern wcd_element wcd_RDAC_3_MUX;
extern wcd_element wcd_HPHR;
extern wcd_element wcd_RX_3_MAIN_MUX_1;
extern wcd_element wcd_RX_3_MAIN_MUX_2;
extern wcd_element wcd_RX_3_MAIN_MUX_3;
extern wcd_element wcd_RX_3_MAIN_MIXER;
extern wcd_element wcd_RX_3_HPF;
extern wcd_element wcd_RX_3_PATH;
extern wcd_element wcd_RX_4_MAIN_MUX_1;
extern wcd_element wcd_RX_4_MAIN_MUX_2;
extern wcd_element wcd_RX_4_MAIN_MUX_3;
extern wcd_element wcd_RX_4_MAIN_MIXER;
extern wcd_element wcd_RX_4_ST_MUX_1;
extern wcd_element wcd_RX_4_ST_MUX_2;
extern wcd_element wcd_RX_4_ST_MIXER;
extern wcd_element wcd_RX_4_HPF;
extern wcd_element wcd_RX_4_PATH;
extern wcd_element wcd_MBIAS_MGR_1;
extern wcd_element wcd_DMIC_1;
extern wcd_element wcd_MBIAS_MGR_2;
extern wcd_element wcd_DMIC_2;
extern wcd_element wcd_MBIAS_MGR_3;
extern wcd_element wcd_DMIC_3;
extern wcd_element wcd_MBIAS_MGR_4;
extern wcd_element wcd_DMIC_4;
extern wcd_element wcd_MBIAS_MGR_5;
extern wcd_element wcd_ADC_1;
extern wcd_element wcd_MBIAS_MGR_6;
extern wcd_element wcd_MBIAS_MGR_7;
extern wcd_element wcd_MBIAS_MGR_8;
extern wcd_element wcd_ADC_2;
extern wcd_element wcd_TX_PATH_MUX_1;
extern wcd_element wcd_TX_PATH_HPF_1;
extern wcd_element wcd_TX_PATH_1;
extern wcd_element wcd_TX_PATH_MUX_2;
extern wcd_element wcd_TX_PATH_MUX_3;
extern wcd_element wcd_TX_PATH_MUX_4;
extern wcd_element wcd_TX_PATH_HPF_2;
extern wcd_element wcd_TX_PATH_2;
extern wcd_element wcd_TX_PATH_HPF_3;
extern wcd_element wcd_TX_PATH_HPF_4;
extern wcd_element wcd_TX_PATH_3;
extern wcd_element wcd_TX_PATH_4;
extern wcd_element wcd_SBTX_MUX_1;
extern wcd_element wcd_SBTX_1;
extern wcd_element wcd_SBTX_MUX_2;
extern wcd_element wcd_SBTX_2;
extern wcd_element wcd_SBTX_MUX_3;
extern wcd_element wcd_SBTX_MUX_4;
extern wcd_element wcd_SBTX_MUX_5;
extern wcd_element wcd_SBTX_3;
extern wcd_element wcd_SBTX_4;
extern wcd_element wcd_SBTX_5;
extern wcd_element wcd_RX_4_INTERPOLATOR_MIXER;
extern wcd_element wcd_RX_3_COMPANDER;
extern wcd_element wcd_RX_BIAS_3;
extern wcd_element wcd_RDAC_4_MUX;
extern wcd_element wcd_LO1_L_P;
extern wcd_element wcd_RX_4_COMPANDER;
extern wcd_element wcd_CLASS_CONTROLLER_4;
extern wcd_element wcd_RX_BIAS_4;
extern wcd_element wcd_SPEAKER;
extern wcd_element wcd_RDAC_5_DIFF_MUX;
extern wcd_element wcd_LO2_R_M;
extern wcd_element wcd_USONIC_MUX;
extern wcd_element wcd_ADC_3;
extern wcd_element wcd_ADC_4;
extern wcd_element wcd_MBIAS_MGR_9;
extern wcd_element wcd_ADC_5;
extern wcd_element wcd_ANC_CHANNEL_1;
extern wcd_element wcd_ANC_CHANNEL_2;

wcd_result wcd_initialize_elements(wcd_codec_info codec_info);

#endif
