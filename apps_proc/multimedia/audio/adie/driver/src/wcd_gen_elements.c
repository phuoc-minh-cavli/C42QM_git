/**
@file wcd_gen_elements.c
@brief 
Generated elements and events for WCD codec.
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
   All rights reserved.
   Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/wcd_gen_elements.c#4 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/13/13   AG     Added new speaker PA element type
07/23/13   KNM    Added ANC FB Tune1 & Tune2 routing in Tx path.
05/31/13   KNM    Corrected connectable element arrary of RX3_BIAS.
03/21/13   KNM    Resolved SAL warnings.
03/01/13   KNM    Initial revision.
===============================================================================*/

#include "wcd_gen_elements.h"
#include "wcd_gen_events.h"
#include "wcd_gen_id_defines.h"
#include "target_defs.h"

/*
// Enable this if WCD_DEBUG macro is added in this file
#ifdef SPL_LOG_SUPPORT
	#include "wcd_gen_elements.tmh"
#endif
*/

/**********************************************************************************
 *******************                  ELEMENTS                  *******************
 *********************************************************************************/
wcd_element wcd_SBRX_1 = { 
	 WCD_SBRX_1_ID, 
	 WCD_BUS_PORT_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_SBRX_2 = { 
	 WCD_SBRX_2_ID, 
	 WCD_BUS_PORT_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_SBRX_3 = { 
	 WCD_SBRX_3_ID, 
	 WCD_BUS_PORT_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_SBRX_4 = { 
	 WCD_SBRX_4_ID, 
	 WCD_BUS_PORT_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_SBRX_5 = { 
	 WCD_SBRX_5_ID, 
	 WCD_BUS_PORT_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RDAC_1_MUX = { 
	 WCD_RDAC_1_MUX_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_EAROUT = { 
	 WCD_EAROUT_ID, 
	 WCD_EAR_PA_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_1_MAIN_MUX_1 = { 
	 WCD_RX_1_MAIN_MUX_1_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_1_MAIN_MUX_2 = { 
	 WCD_RX_1_MAIN_MUX_2_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_1_MAIN_MUX_3 = { 
	 WCD_RX_1_MAIN_MUX_3_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_1_MAIN_MIXER = { 
	 WCD_RX_1_MAIN_MIXER_ID, 
	 WCD_MIXER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_1_ST_MIXER = { 
	 WCD_RX_1_ST_MIXER_ID, 
	 WCD_MIXER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_1_ST_MUX_1 = { 
	 WCD_RX_1_ST_MUX_1_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_1_ST_MUX_2 = { 
	 WCD_RX_1_ST_MUX_2_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_1_HPF = { 
	 WCD_RX_1_HPF_ID, 
	 WCD_HPF_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_1_PATH = { 
	 WCD_RX_1_PATH_ID, 
	 WCD_RX_PATH_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_1_COMPANDER = { 
	 WCD_RX_1_COMPANDER_ID, 
	 WCD_COMPANDER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_CLASS_CONTROLLER_1 = { 
	 WCD_CLASS_CONTROLLER_1_ID, 
	 WCD_CLASS_CONTROLLER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_BIAS_1 = { 
	 WCD_RX_BIAS_1_ID, 
	 WCD_RX_BIAS_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_HPHL = { 
	 WCD_HPHL_ID, 
	 WCD_DAC_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_2_MAIN_MUX_1 = { 
	 WCD_RX_2_MAIN_MUX_1_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_2_MAIN_MUX_2 = { 
	 WCD_RX_2_MAIN_MUX_2_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_2_MAIN_MUX_3 = { 
	 WCD_RX_2_MAIN_MUX_3_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_2_MAIN_MIXER = { 
	 WCD_RX_2_MAIN_MIXER_ID, 
	 WCD_MIXER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_2_ST_MUX_1 = { 
	 WCD_RX_2_ST_MUX_1_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_2_ST_MUX_2 = { 
	 WCD_RX_2_ST_MUX_2_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_2_ST_MIXER = { 
	 WCD_RX_2_ST_MIXER_ID, 
	 WCD_MIXER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_2_HPF = { 
	 WCD_RX_2_HPF_ID, 
	 WCD_HPF_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_2_PATH = { 
	 WCD_RX_2_PATH_ID, 
	 WCD_RX_PATH_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_2_COMPANDER = { 
	 WCD_RX_2_COMPANDER_ID, 
	 WCD_COMPANDER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_CLASS_CONTROLLER_2 = { 
	 WCD_CLASS_CONTROLLER_2_ID, 
	 WCD_CLASS_CONTROLLER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_BIAS_2 = { 
	 WCD_RX_BIAS_2_ID, 
	 WCD_RX_BIAS_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RDAC_3_MUX = { 
	 WCD_RDAC_3_MUX_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_HPHR = { 
	 WCD_HPHR_ID, 
	 WCD_DAC_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_3_MAIN_MUX_1 = { 
	 WCD_RX_3_MAIN_MUX_1_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_3_MAIN_MUX_2 = { 
	 WCD_RX_3_MAIN_MUX_2_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_3_MAIN_MUX_3 = { 
	 WCD_RX_3_MAIN_MUX_3_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_3_MAIN_MIXER = { 
	 WCD_RX_3_MAIN_MIXER_ID, 
	 WCD_MIXER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_3_HPF = { 
	 WCD_RX_3_HPF_ID, 
	 WCD_HPF_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_3_PATH = { 
	 WCD_RX_3_PATH_ID, 
	 WCD_RX_PATH_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_4_MAIN_MUX_1 = { 
	 WCD_RX_4_MAIN_MUX_1_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_4_MAIN_MUX_2 = { 
	 WCD_RX_4_MAIN_MUX_2_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_4_MAIN_MUX_3 = { 
	 WCD_RX_4_MAIN_MUX_3_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_4_MAIN_MIXER = { 
	 WCD_RX_4_MAIN_MIXER_ID, 
	 WCD_MIXER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_4_ST_MUX_1 = { 
	 WCD_RX_4_ST_MUX_1_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_4_ST_MUX_2 = { 
	 WCD_RX_4_ST_MUX_2_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_4_ST_MIXER = { 
	 WCD_RX_4_ST_MIXER_ID, 
	 WCD_MIXER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_4_HPF = { 
	 WCD_RX_4_HPF_ID, 
	 WCD_HPF_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_4_PATH = { 
	 WCD_RX_4_PATH_ID, 
	 WCD_RX_PATH_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_MBIAS_MGR_1 = { 
	 WCD_MBIAS_MGR_1_ID, 
	 WCD_MICBIAS_MANAGER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_DMIC_1 = { 
	 WCD_DMIC_1_ID, 
	 WCD_DMIC_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_MBIAS_MGR_2 = { 
	 WCD_MBIAS_MGR_2_ID, 
	 WCD_MICBIAS_MANAGER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_DMIC_2 = { 
	 WCD_DMIC_2_ID, 
	 WCD_DMIC_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_MBIAS_MGR_3 = { 
	 WCD_MBIAS_MGR_3_ID, 
	 WCD_MICBIAS_MANAGER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_DMIC_3 = { 
	 WCD_DMIC_3_ID, 
	 WCD_DMIC_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_MBIAS_MGR_4 = { 
	 WCD_MBIAS_MGR_4_ID, 
	 WCD_MICBIAS_MANAGER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_DMIC_4 = { 
	 WCD_DMIC_4_ID, 
	 WCD_DMIC_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_MBIAS_MGR_5 = { 
	 WCD_MBIAS_MGR_5_ID, 
	 WCD_MICBIAS_MANAGER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_ADC_1 = { 
	 WCD_ADC_1_ID, 
	 WCD_ADC_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_MBIAS_MGR_6 = { 
	 WCD_MBIAS_MGR_6_ID, 
	 WCD_MICBIAS_MANAGER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_MBIAS_MGR_7 = { 
	 WCD_MBIAS_MGR_7_ID, 
	 WCD_MICBIAS_MANAGER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_MBIAS_MGR_8 = { 
	 WCD_MBIAS_MGR_8_ID, 
	 WCD_MICBIAS_MANAGER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_ADC_2 = { 
	 WCD_ADC_2_ID, 
	 WCD_ADC_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_TX_PATH_MUX_1 = { 
	 WCD_TX_PATH_MUX_1_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_TX_PATH_HPF_1 = { 
	 WCD_TX_PATH_HPF_1_ID, 
	 WCD_HPF_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_TX_PATH_1 = { 
	 WCD_TX_PATH_1_ID, 
	 WCD_TX_PATH_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_TX_PATH_MUX_2 = { 
	 WCD_TX_PATH_MUX_2_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_TX_PATH_MUX_3 = { 
	 WCD_TX_PATH_MUX_3_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_TX_PATH_MUX_4 = { 
	 WCD_TX_PATH_MUX_4_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_TX_PATH_HPF_2 = { 
	 WCD_TX_PATH_HPF_2_ID, 
	 WCD_HPF_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_TX_PATH_2 = { 
	 WCD_TX_PATH_2_ID, 
	 WCD_TX_PATH_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_TX_PATH_HPF_3 = { 
	 WCD_TX_PATH_HPF_3_ID, 
	 WCD_HPF_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_TX_PATH_HPF_4 = { 
	 WCD_TX_PATH_HPF_4_ID, 
	 WCD_HPF_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_TX_PATH_3 = { 
	 WCD_TX_PATH_3_ID, 
	 WCD_TX_PATH_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_TX_PATH_4 = { 
	 WCD_TX_PATH_4_ID, 
	 WCD_TX_PATH_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_SBTX_MUX_1 = { 
	 WCD_SBTX_MUX_1_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_SBTX_1 = { 
	 WCD_SBTX_1_ID, 
	 WCD_BUS_PORT_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_SBTX_MUX_2 = { 
	 WCD_SBTX_MUX_2_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_SBTX_2 = { 
	 WCD_SBTX_2_ID, 
	 WCD_BUS_PORT_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_SBTX_MUX_3 = { 
	 WCD_SBTX_MUX_3_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_SBTX_MUX_4 = { 
	 WCD_SBTX_MUX_4_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_SBTX_MUX_5 = { 
	 WCD_SBTX_MUX_5_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_SBTX_3 = { 
	 WCD_SBTX_3_ID, 
	 WCD_BUS_PORT_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_SBTX_4 = { 
	 WCD_SBTX_4_ID, 
	 WCD_BUS_PORT_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_SBTX_5 = { 
	 WCD_SBTX_5_ID, 
	 WCD_BUS_PORT_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_4_INTERPOLATOR_MIXER = { 
	 WCD_RX_4_INTERPOLATOR_MIXER_ID, 
	 WCD_MIXER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_3_COMPANDER = { 
	 WCD_RX_3_COMPANDER_ID, 
	 WCD_COMPANDER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_BIAS_3 = { 
	 WCD_RX_BIAS_3_ID, 
	 WCD_RX_BIAS_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RDAC_4_MUX = { 
	 WCD_RDAC_4_MUX_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_LO1_L_P = { 
	 WCD_LO1_L_P_ID, 
	 WCD_DAC_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_4_COMPANDER = { 
	 WCD_RX_4_COMPANDER_ID, 
	 WCD_COMPANDER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_CLASS_CONTROLLER_4 = { 
	 WCD_CLASS_CONTROLLER_4_ID, 
	 WCD_CLASS_CONTROLLER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RX_BIAS_4 = { 
	 WCD_RX_BIAS_4_ID, 
	 WCD_RX_BIAS_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};

wcd_element wcd_SPEAKER = { 
	 WCD_SPEAKER_ID, 
	 WCD_DAC_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_RDAC_5_DIFF_MUX = { 
	 WCD_RDAC_5_DIFF_MUX_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_LO2_R_M = { 
	 WCD_LO2_R_M_ID, 
	 WCD_DAC_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_USONIC_MUX = { 
	 WCD_USONIC_MUX_ID, 
	 WCD_MUX_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_ADC_3 = { 
	 WCD_ADC_3_ID, 
	 WCD_ADC_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_ADC_4 = { 
	 WCD_ADC_4_ID, 
	 WCD_ADC_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_MBIAS_MGR_9 = { 
	 WCD_MBIAS_MGR_9_ID, 
	 WCD_MICBIAS_MANAGER_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_ADC_5 = { 
	 WCD_ADC_5_ID, 
	 WCD_ADC_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_ANC_CHANNEL_1 = { 
	 WCD_ANC_CHANNEL_1_ID, 
	 WCD_ANC_CHANNEL_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};
wcd_element wcd_ANC_CHANNEL_2 = { 
	 WCD_ANC_CHANNEL_2_ID, 
	 WCD_ANC_CHANNEL_TYPE_ID, 
	 0, 
	 NULL, 
	 NULL, 
	 0, 
	 NULL, 
	 0, 
	 0 
};

/**********************************************************************************
 *******************        CONNECTABLE ELEMENTS ARRAYS         *******************
 *********************************************************************************/

wcd_element *wcd_SBRX_1_cea[] = {  NULL };
uint32 wcd_SBRX_1_cea_count = (sizeof(wcd_SBRX_1_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_SBRX_2_cea[] = {  NULL };
uint32 wcd_SBRX_2_cea_count = (sizeof(wcd_SBRX_2_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_SBRX_3_cea[] = {  NULL };
uint32 wcd_SBRX_3_cea_count = (sizeof(wcd_SBRX_3_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_SBRX_4_cea[] = {  NULL };
uint32 wcd_SBRX_4_cea_count = (sizeof(wcd_SBRX_4_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_SBRX_5_cea[] = {  NULL };
uint32 wcd_SBRX_5_cea_count = (sizeof(wcd_SBRX_5_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RDAC_1_MUX_cea[] = { &wcd_RX_BIAS_1, &wcd_RX_BIAS_4,  NULL };
uint32 wcd_RDAC_1_MUX_cea_count = (sizeof(wcd_RDAC_1_MUX_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_EAROUT_cea[] = { &wcd_RDAC_1_MUX,  NULL };
uint32 wcd_EAROUT_cea_count = (sizeof(wcd_EAROUT_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_1_MAIN_MUX_1_cea[] = { &wcd_SBRX_1, &wcd_SBRX_2, &wcd_SBRX_3, &wcd_SBRX_4, &wcd_SBRX_5,  NULL };
uint32 wcd_RX_1_MAIN_MUX_1_cea_count = (sizeof(wcd_RX_1_MAIN_MUX_1_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_1_MAIN_MUX_2_cea[] = { &wcd_SBRX_1, &wcd_SBRX_2, &wcd_SBRX_3, &wcd_SBRX_4, &wcd_SBRX_5,  NULL };
uint32 wcd_RX_1_MAIN_MUX_2_cea_count = (sizeof(wcd_RX_1_MAIN_MUX_2_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_1_MAIN_MUX_3_cea[] = { &wcd_SBRX_1, &wcd_SBRX_2, &wcd_SBRX_3, &wcd_SBRX_4, &wcd_SBRX_5,  NULL };
uint32 wcd_RX_1_MAIN_MUX_3_cea_count = (sizeof(wcd_RX_1_MAIN_MUX_3_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_1_MAIN_MIXER_cea[] = { &wcd_RX_1_MAIN_MUX_1, &wcd_RX_1_MAIN_MUX_2, &wcd_RX_1_MAIN_MUX_3,  NULL };
uint32 wcd_RX_1_MAIN_MIXER_cea_count = (sizeof(wcd_RX_1_MAIN_MIXER_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_1_ST_MIXER_cea[] = { &wcd_RX_1_MAIN_MIXER, &wcd_RX_1_ST_MUX_1, &wcd_RX_1_ST_MUX_2,  NULL };
uint32 wcd_RX_1_ST_MIXER_cea_count = (sizeof(wcd_RX_1_ST_MIXER_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_1_ST_MUX_1_cea[] = {  NULL };
uint32 wcd_RX_1_ST_MUX_1_cea_count = (sizeof(wcd_RX_1_ST_MUX_1_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_1_ST_MUX_2_cea[] = {  NULL };
uint32 wcd_RX_1_ST_MUX_2_cea_count = (sizeof(wcd_RX_1_ST_MUX_2_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_1_HPF_cea[] = { &wcd_RX_1_ST_MIXER,  NULL };
uint32 wcd_RX_1_HPF_cea_count = (sizeof(wcd_RX_1_HPF_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_1_PATH_cea[] = { &wcd_RX_1_HPF,  NULL };
uint32 wcd_RX_1_PATH_cea_count = (sizeof(wcd_RX_1_PATH_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_1_COMPANDER_cea[] = { &wcd_RX_1_PATH,  NULL };
uint32 wcd_RX_1_COMPANDER_cea_count = (sizeof(wcd_RX_1_COMPANDER_cea)/sizeof(wcd_element*)) - 1;

wcd_element **wcd_CLASS_CONTROLLER_1_cea = NULL;
uint32 wcd_CLASS_CONTROLLER_1_cea_count;

wcd_element *wcd9306_CLASS_CONTROLLER_1_cea[] = { &wcd_RX_1_COMPANDER,  NULL };
uint32 wcd9306_CLASS_CONTROLLER_1_cea_count = (sizeof(wcd9306_CLASS_CONTROLLER_1_cea)/sizeof(wcd_element*)) - 1;

wcd_element *wcd9302_CLASS_CONTROLLER_1_cea[] = { &wcd_RX_1_PATH,  NULL };
uint32 wcd9302_CLASS_CONTROLLER_1_cea_count = (sizeof(wcd9302_CLASS_CONTROLLER_1_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_BIAS_1_cea[] = { &wcd_CLASS_CONTROLLER_1,  NULL };
uint32 wcd_RX_BIAS_1_cea_count = (sizeof(wcd_RX_BIAS_1_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_HPHL_cea[] = { &wcd_RX_BIAS_1,  NULL };
uint32 wcd_HPHL_cea_count = (sizeof(wcd_HPHL_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_2_MAIN_MUX_1_cea[] = { &wcd_SBRX_1, &wcd_SBRX_2, &wcd_SBRX_3, &wcd_SBRX_4, &wcd_SBRX_5,  NULL };
uint32 wcd_RX_2_MAIN_MUX_1_cea_count = (sizeof(wcd_RX_2_MAIN_MUX_1_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_2_MAIN_MUX_2_cea[] = { &wcd_SBRX_1, &wcd_SBRX_2, &wcd_SBRX_3, &wcd_SBRX_4, &wcd_SBRX_5,  NULL };
uint32 wcd_RX_2_MAIN_MUX_2_cea_count = (sizeof(wcd_RX_2_MAIN_MUX_2_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_2_MAIN_MUX_3_cea[] = { &wcd_SBRX_1, &wcd_SBRX_2, &wcd_SBRX_3, &wcd_SBRX_4, &wcd_SBRX_5,  NULL };
uint32 wcd_RX_2_MAIN_MUX_3_cea_count = (sizeof(wcd_RX_2_MAIN_MUX_3_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_2_MAIN_MIXER_cea[] = { &wcd_RX_2_MAIN_MUX_1, &wcd_RX_2_MAIN_MUX_2, &wcd_RX_2_MAIN_MUX_3,  NULL };
uint32 wcd_RX_2_MAIN_MIXER_cea_count = (sizeof(wcd_RX_2_MAIN_MIXER_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_2_ST_MUX_1_cea[] = {  NULL };
uint32 wcd_RX_2_ST_MUX_1_cea_count = (sizeof(wcd_RX_2_ST_MUX_1_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_2_ST_MUX_2_cea[] = {  NULL };
uint32 wcd_RX_2_ST_MUX_2_cea_count = (sizeof(wcd_RX_2_ST_MUX_2_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_2_ST_MIXER_cea[] = { &wcd_RX_2_MAIN_MIXER, &wcd_RX_2_ST_MUX_1, &wcd_RX_2_ST_MUX_2,  NULL };
uint32 wcd_RX_2_ST_MIXER_cea_count = (sizeof(wcd_RX_2_ST_MIXER_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_2_HPF_cea[] = { &wcd_RX_2_ST_MIXER,  NULL };
uint32 wcd_RX_2_HPF_cea_count = (sizeof(wcd_RX_2_HPF_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_2_PATH_cea[] = { &wcd_RX_2_HPF,  NULL };
uint32 wcd_RX_2_PATH_cea_count = (sizeof(wcd_RX_2_PATH_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_2_COMPANDER_cea[] = { &wcd_RX_2_PATH,  NULL };
uint32 wcd_RX_2_COMPANDER_cea_count = (sizeof(wcd_RX_2_COMPANDER_cea)/sizeof(wcd_element*)) - 1;

wcd_element **wcd_CLASS_CONTROLLER_2_cea = NULL;
uint32 wcd_CLASS_CONTROLLER_2_cea_count;

wcd_element *wcd9306_CLASS_CONTROLLER_2_cea[] = { &wcd_RX_2_COMPANDER,  NULL };
uint32 wcd9306_CLASS_CONTROLLER_2_cea_count = (sizeof(wcd9306_CLASS_CONTROLLER_2_cea)/sizeof(wcd_element*)) - 1;

wcd_element *wcd9302_CLASS_CONTROLLER_2_cea[] = { &wcd_RX_2_PATH,  NULL };
uint32 wcd9302_CLASS_CONTROLLER_2_cea_count = (sizeof(wcd9302_CLASS_CONTROLLER_2_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_BIAS_2_cea[] = { &wcd_CLASS_CONTROLLER_2,  NULL };
uint32 wcd_RX_BIAS_2_cea_count = (sizeof(wcd_RX_BIAS_2_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RDAC_3_MUX_cea[] = { &wcd_RX_BIAS_2, &wcd_RX_BIAS_1,  NULL };
uint32 wcd_RDAC_3_MUX_cea_count = (sizeof(wcd_RDAC_3_MUX_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_HPHR_cea[] = { &wcd_RDAC_3_MUX,  NULL };
uint32 wcd_HPHR_cea_count = (sizeof(wcd_HPHR_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_3_MAIN_MUX_1_cea[] = { &wcd_SBRX_1, &wcd_SBRX_2, &wcd_SBRX_3, &wcd_SBRX_4, &wcd_SBRX_5,  NULL };
uint32 wcd_RX_3_MAIN_MUX_1_cea_count = (sizeof(wcd_RX_3_MAIN_MUX_1_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_3_MAIN_MUX_2_cea[] = { &wcd_SBRX_1, &wcd_SBRX_2, &wcd_SBRX_3, &wcd_SBRX_4, &wcd_SBRX_5,  NULL };
uint32 wcd_RX_3_MAIN_MUX_2_cea_count = (sizeof(wcd_RX_3_MAIN_MUX_2_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_3_MAIN_MUX_3_cea[] = { &wcd_SBRX_1, &wcd_SBRX_2, &wcd_SBRX_3, &wcd_SBRX_4, &wcd_SBRX_5,  NULL };
uint32 wcd_RX_3_MAIN_MUX_3_cea_count = (sizeof(wcd_RX_3_MAIN_MUX_3_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_3_MAIN_MIXER_cea[] = { &wcd_RX_3_MAIN_MUX_1, &wcd_RX_3_MAIN_MUX_2, &wcd_RX_3_MAIN_MUX_3,  NULL };
uint32 wcd_RX_3_MAIN_MIXER_cea_count = (sizeof(wcd_RX_3_MAIN_MIXER_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_3_HPF_cea[] = { &wcd_RX_3_MAIN_MIXER,  NULL };
uint32 wcd_RX_3_HPF_cea_count = (sizeof(wcd_RX_3_HPF_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_3_PATH_cea[] = { &wcd_RX_3_HPF,  NULL };
uint32 wcd_RX_3_PATH_cea_count = (sizeof(wcd_RX_3_PATH_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_4_MAIN_MUX_1_cea[] = { &wcd_SBRX_1, &wcd_SBRX_2, &wcd_SBRX_3, &wcd_SBRX_4, &wcd_SBRX_5,  NULL };
uint32 wcd_RX_4_MAIN_MUX_1_cea_count = (sizeof(wcd_RX_4_MAIN_MUX_1_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_4_MAIN_MUX_2_cea[] = { &wcd_SBRX_1, &wcd_SBRX_2, &wcd_SBRX_3, &wcd_SBRX_4, &wcd_SBRX_5,  NULL };
uint32 wcd_RX_4_MAIN_MUX_2_cea_count = (sizeof(wcd_RX_4_MAIN_MUX_2_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_4_MAIN_MUX_3_cea[] = { &wcd_SBRX_1, &wcd_SBRX_2, &wcd_SBRX_3, &wcd_SBRX_4, &wcd_SBRX_5,  NULL };
uint32 wcd_RX_4_MAIN_MUX_3_cea_count = (sizeof(wcd_RX_4_MAIN_MUX_3_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_4_MAIN_MIXER_cea[] = { &wcd_RX_4_MAIN_MUX_1, &wcd_RX_4_MAIN_MUX_2, &wcd_RX_4_MAIN_MUX_3,  NULL };
uint32 wcd_RX_4_MAIN_MIXER_cea_count = (sizeof(wcd_RX_4_MAIN_MIXER_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_4_ST_MUX_1_cea[] = {  NULL };
uint32 wcd_RX_4_ST_MUX_1_cea_count = (sizeof(wcd_RX_4_ST_MUX_1_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_4_ST_MUX_2_cea[] = {  NULL };
uint32 wcd_RX_4_ST_MUX_2_cea_count = (sizeof(wcd_RX_4_ST_MUX_2_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_4_ST_MIXER_cea[] = { &wcd_RX_4_MAIN_MIXER, &wcd_RX_4_ST_MUX_1, &wcd_RX_4_ST_MUX_2,  NULL };
uint32 wcd_RX_4_ST_MIXER_cea_count = (sizeof(wcd_RX_4_ST_MIXER_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_4_HPF_cea[] = { &wcd_RX_4_ST_MIXER,  NULL };
uint32 wcd_RX_4_HPF_cea_count = (sizeof(wcd_RX_4_HPF_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_4_PATH_cea[] = { &wcd_RX_4_HPF,  NULL };
uint32 wcd_RX_4_PATH_cea_count = (sizeof(wcd_RX_4_PATH_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_MBIAS_MGR_1_cea[] = {  NULL };
uint32 wcd_MBIAS_MGR_1_cea_count = (sizeof(wcd_MBIAS_MGR_1_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_DMIC_1_cea[] = { &wcd_MBIAS_MGR_1,  NULL };
uint32 wcd_DMIC_1_cea_count = (sizeof(wcd_DMIC_1_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_MBIAS_MGR_2_cea[] = {  NULL };
uint32 wcd_MBIAS_MGR_2_cea_count = (sizeof(wcd_MBIAS_MGR_2_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_DMIC_2_cea[] = { &wcd_MBIAS_MGR_2,  NULL };
uint32 wcd_DMIC_2_cea_count = (sizeof(wcd_DMIC_2_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_MBIAS_MGR_3_cea[] = {  NULL };
uint32 wcd_MBIAS_MGR_3_cea_count = (sizeof(wcd_MBIAS_MGR_3_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_DMIC_3_cea[] = { &wcd_MBIAS_MGR_3,  NULL };
uint32 wcd_DMIC_3_cea_count = (sizeof(wcd_DMIC_3_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_MBIAS_MGR_4_cea[] = {  NULL };
uint32 wcd_MBIAS_MGR_4_cea_count = (sizeof(wcd_MBIAS_MGR_4_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_DMIC_4_cea[] = { &wcd_MBIAS_MGR_4,  NULL };
uint32 wcd_DMIC_4_cea_count = (sizeof(wcd_DMIC_4_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_MBIAS_MGR_5_cea[] = {  NULL };
uint32 wcd_MBIAS_MGR_5_cea_count = (sizeof(wcd_MBIAS_MGR_5_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_ADC_1_cea[] = { &wcd_MBIAS_MGR_5,  NULL };
uint32 wcd_ADC_1_cea_count = (sizeof(wcd_ADC_1_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_MBIAS_MGR_6_cea[] = {  NULL };
uint32 wcd_MBIAS_MGR_6_cea_count = (sizeof(wcd_MBIAS_MGR_6_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_MBIAS_MGR_7_cea[] = {  NULL };
uint32 wcd_MBIAS_MGR_7_cea_count = (sizeof(wcd_MBIAS_MGR_7_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_MBIAS_MGR_8_cea[] = {  NULL };
uint32 wcd_MBIAS_MGR_8_cea_count = (sizeof(wcd_MBIAS_MGR_8_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_ADC_2_cea[] = { &wcd_MBIAS_MGR_6,  NULL };
uint32 wcd_ADC_2_cea_count = (sizeof(wcd_ADC_2_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_TX_PATH_MUX_1_cea[] = { &wcd_ADC_1, &wcd_ADC_2, &wcd_ADC_3, &wcd_ADC_4, &wcd_DMIC_1, &wcd_DMIC_2, &wcd_DMIC_3, &wcd_DMIC_4,  NULL };
uint32 wcd_TX_PATH_MUX_1_cea_count = (sizeof(wcd_TX_PATH_MUX_1_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_TX_PATH_HPF_1_cea[] = { &wcd_TX_PATH_MUX_1,  NULL };
uint32 wcd_TX_PATH_HPF_1_cea_count = (sizeof(wcd_TX_PATH_HPF_1_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_TX_PATH_1_cea[] = { &wcd_TX_PATH_HPF_1,  NULL };
uint32 wcd_TX_PATH_1_cea_count = (sizeof(wcd_TX_PATH_1_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_TX_PATH_MUX_2_cea[] = { &wcd_ADC_1, &wcd_ADC_2, &wcd_ADC_3, &wcd_ADC_4, &wcd_DMIC_1, &wcd_DMIC_2, &wcd_DMIC_3, &wcd_DMIC_4,  NULL };
uint32 wcd_TX_PATH_MUX_2_cea_count = (sizeof(wcd_TX_PATH_MUX_2_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_TX_PATH_MUX_3_cea[] = { &wcd_ADC_1, &wcd_ADC_2, &wcd_ADC_3, &wcd_ADC_4, &wcd_ADC_5, &wcd_DMIC_1, &wcd_DMIC_2, &wcd_DMIC_3, &wcd_DMIC_4,  NULL };
uint32 wcd_TX_PATH_MUX_3_cea_count = (sizeof(wcd_TX_PATH_MUX_3_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_TX_PATH_MUX_4_cea[] = { &wcd_ADC_1, &wcd_ADC_2, &wcd_ADC_3, &wcd_ADC_4, &wcd_ADC_5, &wcd_DMIC_1, &wcd_DMIC_2, &wcd_DMIC_3, &wcd_DMIC_4,  NULL };
uint32 wcd_TX_PATH_MUX_4_cea_count = (sizeof(wcd_TX_PATH_MUX_4_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_TX_PATH_HPF_2_cea[] = { &wcd_TX_PATH_MUX_2,  NULL };
uint32 wcd_TX_PATH_HPF_2_cea_count = (sizeof(wcd_TX_PATH_HPF_2_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_TX_PATH_2_cea[] = { &wcd_TX_PATH_HPF_2,  NULL };
uint32 wcd_TX_PATH_2_cea_count = (sizeof(wcd_TX_PATH_2_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_TX_PATH_HPF_3_cea[] = { &wcd_TX_PATH_MUX_3,  NULL };
uint32 wcd_TX_PATH_HPF_3_cea_count = (sizeof(wcd_TX_PATH_HPF_3_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_TX_PATH_HPF_4_cea[] = { &wcd_TX_PATH_MUX_4,  NULL };
uint32 wcd_TX_PATH_HPF_4_cea_count = (sizeof(wcd_TX_PATH_HPF_4_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_TX_PATH_3_cea[] = { &wcd_TX_PATH_HPF_3,  NULL };
uint32 wcd_TX_PATH_3_cea_count = (sizeof(wcd_TX_PATH_3_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_TX_PATH_4_cea[] = { &wcd_TX_PATH_HPF_4,  NULL };
uint32 wcd_TX_PATH_4_cea_count = (sizeof(wcd_TX_PATH_4_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_SBTX_MUX_1_cea[] = { &wcd_RX_1_MAIN_MIXER, &wcd_RX_2_MAIN_MIXER, &wcd_RX_3_MAIN_MIXER, &wcd_RX_4_MAIN_MIXER, &wcd_TX_PATH_1, &wcd_TX_PATH_2, &wcd_TX_PATH_3, &wcd_TX_PATH_4,  NULL };
uint32 wcd_SBTX_MUX_1_cea_count = (sizeof(wcd_SBTX_MUX_1_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_SBTX_1_cea[] = { &wcd_SBTX_MUX_1,  NULL };
uint32 wcd_SBTX_1_cea_count = (sizeof(wcd_SBTX_1_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_SBTX_MUX_2_cea[] = { &wcd_RX_1_MAIN_MIXER, &wcd_RX_2_MAIN_MIXER, &wcd_RX_3_MAIN_MIXER, &wcd_RX_4_MAIN_MIXER, &wcd_TX_PATH_1, &wcd_TX_PATH_2, &wcd_TX_PATH_3, &wcd_TX_PATH_4,  NULL };
uint32 wcd_SBTX_MUX_2_cea_count = (sizeof(wcd_SBTX_MUX_2_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_SBTX_2_cea[] = { &wcd_SBTX_MUX_2,  NULL };
uint32 wcd_SBTX_2_cea_count = (sizeof(wcd_SBTX_2_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_SBTX_MUX_3_cea[] = { &wcd_RX_1_MAIN_MIXER, &wcd_RX_2_MAIN_MIXER, &wcd_RX_3_MAIN_MIXER, &wcd_RX_4_MAIN_MIXER, &wcd_TX_PATH_3,  NULL };
uint32 wcd_SBTX_MUX_3_cea_count = (sizeof(wcd_SBTX_MUX_3_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_SBTX_MUX_4_cea[] = { &wcd_RX_1_MAIN_MIXER, &wcd_RX_2_MAIN_MIXER, &wcd_RX_3_MAIN_MIXER, &wcd_RX_4_MAIN_MIXER, &wcd_TX_PATH_4,  NULL };
uint32 wcd_SBTX_MUX_4_cea_count = (sizeof(wcd_SBTX_MUX_4_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_SBTX_MUX_5_cea[] = { &wcd_RX_1_MAIN_MIXER, &wcd_RX_2_MAIN_MIXER, &wcd_RX_3_MAIN_MIXER, &wcd_RX_4_MAIN_MIXER, &wcd_TX_PATH_1,  NULL };
uint32 wcd_SBTX_MUX_5_cea_count = (sizeof(wcd_SBTX_MUX_5_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_SBTX_3_cea[] = { &wcd_SBTX_MUX_3,  NULL };
uint32 wcd_SBTX_3_cea_count = (sizeof(wcd_SBTX_3_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_SBTX_4_cea[] = { &wcd_SBTX_MUX_4,  NULL };
uint32 wcd_SBTX_4_cea_count = (sizeof(wcd_SBTX_4_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_SBTX_5_cea[] = { &wcd_SBTX_MUX_5,  NULL };
uint32 wcd_SBTX_5_cea_count = (sizeof(wcd_SBTX_5_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_4_INTERPOLATOR_MIXER_cea[] = { &wcd_RX_4_PATH, &wcd_USONIC_MUX,  NULL };
uint32 wcd_RX_4_INTERPOLATOR_MIXER_cea_count = (sizeof(wcd_RX_4_INTERPOLATOR_MIXER_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_3_COMPANDER_cea[] = { &wcd_RX_3_PATH,  NULL };
uint32 wcd_RX_3_COMPANDER_cea_count = (sizeof(wcd_RX_3_COMPANDER_cea)/sizeof(wcd_element*)) - 1;

wcd_element **wcd_RX_BIAS_3_cea = NULL;
uint32 wcd_RX_BIAS_3_cea_count;

wcd_element *wcd9306_RX_BIAS_3_cea[] = { &wcd_RX_3_COMPANDER,  NULL };
uint32 wcd9306_RX_BIAS_3_cea_count = (sizeof(wcd9306_RX_BIAS_3_cea)/sizeof(wcd_element*)) - 1;

wcd_element *wcd9302_RX_BIAS_3_cea[] = { &wcd_RX_3_PATH,  NULL };
uint32 wcd9302_RX_BIAS_3_cea_count = (sizeof(wcd9302_RX_BIAS_3_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RDAC_4_MUX_cea[] = { &wcd_RX_BIAS_3, &wcd_RX_BIAS_2,  NULL };
uint32 wcd_RDAC_4_MUX_cea_count = (sizeof(wcd_RDAC_4_MUX_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_LO1_L_P_cea[] = { &wcd_RDAC_4_MUX,  NULL };
uint32 wcd_LO1_L_P_cea_count = (sizeof(wcd_LO1_L_P_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_4_COMPANDER_cea[] = { &wcd_RX_4_INTERPOLATOR_MIXER,  NULL };
uint32 wcd_RX_4_COMPANDER_cea_count = (sizeof(wcd_RX_4_COMPANDER_cea)/sizeof(wcd_element*)) - 1;

wcd_element **wcd_CLASS_CONTROLLER_4_cea = NULL;
uint32 wcd_CLASS_CONTROLLER_4_cea_count;

wcd_element *wcd9306_CLASS_CONTROLLER_4_cea[] = { &wcd_RX_4_COMPANDER,  NULL };
uint32 wcd9306_CLASS_CONTROLLER_4_cea_count = (sizeof(wcd9306_CLASS_CONTROLLER_4_cea)/sizeof(wcd_element*)) - 1;

wcd_element *wcd9302_CLASS_CONTROLLER_4_cea[] = { &wcd_RX_4_PATH,  NULL };
uint32 wcd9302_CLASS_CONTROLLER_4_cea_count = (sizeof(wcd9302_CLASS_CONTROLLER_4_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_RX_BIAS_4_cea[] = { &wcd_CLASS_CONTROLLER_4,  NULL };
uint32 wcd_RX_BIAS_4_cea_count = (sizeof(wcd_RX_BIAS_4_cea)/sizeof(wcd_element*)) - 1;


wcd_element **wcd_SPEAKER_cea = NULL;
uint32 wcd_SPEAKER_cea_count;

wcd_element *wcd9306_SPEAKER_cea[] = { &wcd_RX_BIAS_4,  NULL };
uint32 wcd9306_SPEAKER_cea_count = (sizeof(wcd9306_SPEAKER_cea)/sizeof(wcd_element*)) - 1;

wcd_element *wcd9302_SPEAKER_cea[] = { &wcd_RX_BIAS_3,  NULL };
uint32 wcd9302_SPEAKER_cea_count = (sizeof(wcd9302_SPEAKER_cea)/sizeof(wcd_element*)) - 1;

wcd_element **wcd_RDAC_5_DIFF_MUX_cea = NULL;
uint32 wcd_RDAC_5_DIFF_MUX_cea_count;

wcd_element *wcd9306_RDAC_5_DIFF_MUX_cea[] = { &wcd_RX_BIAS_4, &wcd_RDAC_4_MUX,  NULL };
uint32 wcd9306_RDAC_5_DIFF_MUX_cea_count = (sizeof(wcd9306_RDAC_5_DIFF_MUX_cea)/sizeof(wcd_element*)) - 1;

wcd_element *wcd9302_RDAC_5_DIFF_MUX_cea[] = { &wcd_RX_BIAS_3, &wcd_RDAC_4_MUX,  NULL };
uint32 wcd9302_RDAC_5_DIFF_MUX_cea_count = (sizeof(wcd9302_RDAC_5_DIFF_MUX_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_LO2_R_M_cea[] = { &wcd_RDAC_5_DIFF_MUX,  NULL };
uint32 wcd_LO2_R_M_cea_count = (sizeof(wcd_LO2_R_M_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_USONIC_MUX_cea[] = { &wcd_RX_3_PATH,  NULL };
uint32 wcd_USONIC_MUX_cea_count = (sizeof(wcd_USONIC_MUX_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_ADC_3_cea[] = { &wcd_MBIAS_MGR_7,  NULL };
uint32 wcd_ADC_3_cea_count = (sizeof(wcd_ADC_3_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_ADC_4_cea[] = { &wcd_MBIAS_MGR_8,  NULL };
uint32 wcd_ADC_4_cea_count = (sizeof(wcd_ADC_4_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_MBIAS_MGR_9_cea[] = {  NULL };
uint32 wcd_MBIAS_MGR_9_cea_count = (sizeof(wcd_MBIAS_MGR_9_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_ADC_5_cea[] = { &wcd_MBIAS_MGR_9,  NULL };
uint32 wcd_ADC_5_cea_count = (sizeof(wcd_ADC_5_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_ANC_CHANNEL_1_cea[] = {  NULL };
uint32 wcd_ANC_CHANNEL_1_cea_count = (sizeof(wcd_ANC_CHANNEL_1_cea)/sizeof(wcd_element*)) - 1;


wcd_element *wcd_ANC_CHANNEL_2_cea[] = {  NULL };
uint32 wcd_ANC_CHANNEL_2_cea_count = (sizeof(wcd_ANC_CHANNEL_2_cea)/sizeof(wcd_element*)) - 1;


/**********************************************************************************
 *******************                 EVENT MAPS                 *******************
 *********************************************************************************/
wcd_element_event wcd_SBRX_1_em[] = { 
	 { 1, &wcd_SBRX_1_SET_BIT_WIDTH_12BIT }, 
	 { 2, &wcd_SBRX_1_SET_BIT_WIDTH_16BIT }, 
	 { 3, &wcd_SBRX_1_SET_BIT_WIDTH_20BIT }, 
	 { 4, &wcd_SBRX_1_SET_BIT_WIDTH_24BIT }, 
	 {0, NULL} 
};
uint32 wcd_SBRX_1_em_count = sizeof(wcd_SBRX_1_em)/sizeof(wcd_element_event);

wcd_element_event wcd_SBRX_2_em[] = { 
	 { 1, &wcd_SBRX_2_SET_BIT_WIDTH_12BIT }, 
	 { 2, &wcd_SBRX_2_SET_BIT_WIDTH_16BIT }, 
	 { 3, &wcd_SBRX_2_SET_BIT_WIDTH_20BIT }, 
	 { 4, &wcd_SBRX_2_SET_BIT_WIDTH_24BIT }, 
	 {0, NULL} 
};
uint32 wcd_SBRX_2_em_count = sizeof(wcd_SBRX_2_em)/sizeof(wcd_element_event);

wcd_element_event wcd_SBRX_3_em[] = { 
	 { 1, &wcd_SBRX_3_SET_BIT_WIDTH_12BIT }, 
	 { 2, &wcd_SBRX_3_SET_BIT_WIDTH_16BIT }, 
	 { 3, &wcd_SBRX_3_SET_BIT_WIDTH_20BIT }, 
	 { 4, &wcd_SBRX_3_SET_BIT_WIDTH_24BIT }, 
	 {0, NULL} 
};
uint32 wcd_SBRX_3_em_count = sizeof(wcd_SBRX_3_em)/sizeof(wcd_element_event);

wcd_element_event wcd_SBRX_4_em[] = { 
	 { 1, &wcd_SBRX_4_SET_BIT_WIDTH_12BIT }, 
	 { 2, &wcd_SBRX_4_SET_BIT_WIDTH_16BIT }, 
	 { 3, &wcd_SBRX_4_SET_BIT_WIDTH_20BIT }, 
	 { 4, &wcd_SBRX_4_SET_BIT_WIDTH_24BIT }, 
	 {0, NULL} 
};
uint32 wcd_SBRX_4_em_count = sizeof(wcd_SBRX_4_em)/sizeof(wcd_element_event);

wcd_element_event wcd_SBRX_5_em[] = { 
	 { 1, &wcd_SBRX_5_SET_BIT_WIDTH_12BIT }, 
	 { 2, &wcd_SBRX_5_SET_BIT_WIDTH_16BIT }, 
	 { 3, &wcd_SBRX_5_SET_BIT_WIDTH_20BIT }, 
	 { 4, &wcd_SBRX_5_SET_BIT_WIDTH_24BIT }, 
	 {0, NULL} 
};
uint32 wcd_SBRX_5_em_count = sizeof(wcd_SBRX_5_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RDAC_1_MUX_em[] = { 
	 { 1, &wcd_RDAC_1_MUX_CONNECT }, 
	 { 2, &wcd_RDAC_1_MUX_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RDAC_1_MUX_em_count = sizeof(wcd_RDAC_1_MUX_em)/sizeof(wcd_element_event);

wcd_element_event wcd_EAROUT_em[] = { 
	 { 1, &wcd_EAROUT_ENABLE }, 
	 { 2, &wcd_EAROUT_DISABLE }, 
	 { 3, &wcd_EAROUT_SET_GAIN_0DB }, 
	 { 4, &wcd_EAROUT_SET_GAIN_1P5DB }, 
	 { 5, &wcd_EAROUT_SET_GAIN_3DB }, 
	 { 6, &wcd_EAROUT_SET_GAIN_4P5DB }, 
	 { 7, &wcd_EAROUT_SET_GAIN_6DB }, 
	 {0, NULL} 
};
uint32 wcd_EAROUT_em_count = sizeof(wcd_EAROUT_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_1_MAIN_MUX_1_em[] = { 
	 { 1, &wcd_RX_1_MAIN_MUX_1_CONNECT }, 
	 { 2, &wcd_RX_1_MAIN_MUX_1_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_1_MAIN_MUX_1_em_count = sizeof(wcd_RX_1_MAIN_MUX_1_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_1_MAIN_MUX_2_em[] = { 
	 { 1, &wcd_RX_1_MAIN_MUX_2_CONNECT }, 
	 { 2, &wcd_RX_1_MAIN_MUX_2_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_1_MAIN_MUX_2_em_count = sizeof(wcd_RX_1_MAIN_MUX_2_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_1_MAIN_MUX_3_em[] = { 
	 { 1, &wcd_RX_1_MAIN_MUX_3_CONNECT }, 
	 { 2, &wcd_RX_1_MAIN_MUX_3_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_1_MAIN_MUX_3_em_count = sizeof(wcd_RX_1_MAIN_MUX_3_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_1_MAIN_MIXER_em[] = { 
	 { 1, &wcd_RX_1_MAIN_MIXER_CONNECT }, 
	 { 2, &wcd_RX_1_MAIN_MIXER_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_1_MAIN_MIXER_em_count = sizeof(wcd_RX_1_MAIN_MIXER_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_1_ST_MIXER_em[] = { 
	 { 1, &wcd_RX_1_ST_MIXER_CONNECT }, 
	 { 2, &wcd_RX_1_ST_MIXER_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_1_ST_MIXER_em_count = sizeof(wcd_RX_1_ST_MIXER_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_1_ST_MUX_1_em[] = { 
	 { 1, &wcd_RX_1_ST_MUX_1_CONNECT }, 
	 { 2, &wcd_RX_1_ST_MUX_1_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_1_ST_MUX_1_em_count = sizeof(wcd_RX_1_ST_MUX_1_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_1_ST_MUX_2_em[] = { 
	 { 1, &wcd_RX_1_ST_MUX_2_CONNECT }, 
	 { 2, &wcd_RX_1_ST_MUX_2_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_1_ST_MUX_2_em_count = sizeof(wcd_RX_1_ST_MUX_2_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_1_HPF_em[] = { 
	 { 1, &wcd_RX_1_HPF_BYPASS }, 
	 { 2, &wcd_RX_1_HPF_SET_CUTOFF_FREQ_4HZ }, 
	 { 3, &wcd_RX_1_HPF_SET_CUTOFF_FREQ_75HZ }, 
	 { 4, &wcd_RX_1_HPF_SET_CUTOFF_FREQ_150HZ }, 
	 {0, NULL} 
};
uint32 wcd_RX_1_HPF_em_count = sizeof(wcd_RX_1_HPF_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_1_PATH_em[] = { 
	 { 1, &wcd_RX_1_PATH_ENABLE }, 
	 { 2, &wcd_RX_1_PATH_DISABLE }, 
	 { 3, &wcd_RX_1_PATH_MUTE }, 
	 { 4, &wcd_RX_1_PATH_UNMUTE }, 
	 { 5, &wcd_RX_1_PATH_RESET }, 
	 { 6, &wcd_RX_1_PATH_SET_GAIN }, 
	 {0, NULL} 
};
uint32 wcd_RX_1_PATH_em_count = sizeof(wcd_RX_1_PATH_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_1_COMPANDER_em[] = { 
	 { 1, &wcd_RX_1_COMPANDER_ENABLE }, 
	 { 2, &wcd_RX_1_COMPANDER_DISABLE }, 
	 {0, NULL} 
};
uint32 wcd_RX_1_COMPANDER_em_count = sizeof(wcd_RX_1_COMPANDER_em)/sizeof(wcd_element_event);

wcd_element_event wcd_CLASS_CONTROLLER_1_em[] = { 
	 { 1, &wcd_CLASS_CONTROLLER_1_ENABLE }, 
	 { 2, &wcd_CLASS_CONTROLLER_1_DISABLE }, 
	 {0, NULL} 
};
uint32 wcd_CLASS_CONTROLLER_1_em_count = sizeof(wcd_CLASS_CONTROLLER_1_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_BIAS_1_em[] = { 
	 { 1, &wcd_RX_BIAS_1_ENABLE }, 
	 { 2, &wcd_RX_BIAS_1_DISABLE }, 
	 {0, NULL} 
};
uint32 wcd_RX_BIAS_1_em_count = sizeof(wcd_RX_BIAS_1_em)/sizeof(wcd_element_event);

wcd_element_event wcd_HPHL_em[] = { 
	 { 1, &wcd_HPHL_SET_GAIN }, 
	 { 2, &wcd_HPHL_PRECHARGE }, 
	 { 3, &wcd_HPHL_ENABLE }, 
	 { 4, &wcd_HPHL_DISABLE }, 
	 {0, NULL} 
};
uint32 wcd_HPHL_em_count = sizeof(wcd_HPHL_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_2_MAIN_MUX_1_em[] = { 
	 { 1, &wcd_RX_2_MAIN_MUX_1_CONNECT }, 
	 { 2, &wcd_RX_2_MAIN_MUX_1_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_2_MAIN_MUX_1_em_count = sizeof(wcd_RX_2_MAIN_MUX_1_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_2_MAIN_MUX_2_em[] = { 
	 { 1, &wcd_RX_2_MAIN_MUX_2_CONNECT }, 
	 { 2, &wcd_RX_2_MAIN_MUX_2_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_2_MAIN_MUX_2_em_count = sizeof(wcd_RX_2_MAIN_MUX_2_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_2_MAIN_MUX_3_em[] = { 
	 { 1, &wcd_RX_2_MAIN_MUX_3_CONNECT }, 
	 { 2, &wcd_RX_2_MAIN_MUX_3_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_2_MAIN_MUX_3_em_count = sizeof(wcd_RX_2_MAIN_MUX_3_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_2_MAIN_MIXER_em[] = { 
	 { 1, &wcd_RX_2_MAIN_MIXER_CONNECT }, 
	 { 2, &wcd_RX_2_MAIN_MIXER_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_2_MAIN_MIXER_em_count = sizeof(wcd_RX_2_MAIN_MIXER_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_2_ST_MUX_1_em[] = { 
	 { 1, &wcd_RX_2_ST_MUX_1_CONNECT }, 
	 { 2, &wcd_RX_2_ST_MUX_1_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_2_ST_MUX_1_em_count = sizeof(wcd_RX_2_ST_MUX_1_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_2_ST_MUX_2_em[] = { 
	 { 1, &wcd_RX_2_ST_MUX_2_CONNECT }, 
	 { 2, &wcd_RX_2_ST_MUX_2_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_2_ST_MUX_2_em_count = sizeof(wcd_RX_2_ST_MUX_2_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_2_ST_MIXER_em[] = { 
	 { 1, &wcd_RX_2_ST_MIXER_CONNECT }, 
	 { 2, &wcd_RX_2_ST_MIXER_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_2_ST_MIXER_em_count = sizeof(wcd_RX_2_ST_MIXER_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_2_HPF_em[] = { 
	 { 1, &wcd_RX_2_HPF_BYPASS }, 
	 { 2, &wcd_RX_2_HPF_SET_CUTOFF_FREQ_4HZ }, 
	 { 3, &wcd_RX_2_HPF_SET_CUTOFF_FREQ_75HZ }, 
	 { 4, &wcd_RX_2_HPF_SET_CUTOFF_FREQ_150HZ }, 
	 {0, NULL} 
};
uint32 wcd_RX_2_HPF_em_count = sizeof(wcd_RX_2_HPF_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_2_PATH_em[] = { 
	 { 1, &wcd_RX_2_PATH_ENABLE }, 
	 { 2, &wcd_RX_2_PATH_DISABLE }, 
	 { 3, &wcd_RX_2_PATH_MUTE }, 
	 { 4, &wcd_RX_2_PATH_UNMUTE }, 
	 { 5, &wcd_RX_2_PATH_RESET }, 
	 { 6, &wcd_RX_2_PATH_SET_GAIN }, 
	 {0, NULL} 
};
uint32 wcd_RX_2_PATH_em_count = sizeof(wcd_RX_2_PATH_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_2_COMPANDER_em[] = { 
	 { 1, &wcd_RX_2_COMPANDER_ENABLE }, 
	 { 2, &wcd_RX_2_COMPANDER_DISABLE }, 
	 {0, NULL} 
};
uint32 wcd_RX_2_COMPANDER_em_count = sizeof(wcd_RX_2_COMPANDER_em)/sizeof(wcd_element_event);

wcd_element_event wcd_CLASS_CONTROLLER_2_em[] = { 
	 { 1, &wcd_CLASS_CONTROLLER_2_ENABLE }, 
	 { 2, &wcd_CLASS_CONTROLLER_2_DISABLE }, 
	 {0, NULL} 
};
uint32 wcd_CLASS_CONTROLLER_2_em_count = sizeof(wcd_CLASS_CONTROLLER_2_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_BIAS_2_em[] = { 
	 { 1, &wcd_RX_BIAS_2_ENABLE }, 
	 { 2, &wcd_RX_BIAS_2_DISABLE }, 
	 {0, NULL} 
};
uint32 wcd_RX_BIAS_2_em_count = sizeof(wcd_RX_BIAS_2_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RDAC_3_MUX_em[] = { 
	 { 1, &wcd_RDAC_3_MUX_CONNECT }, 
	 { 2, &wcd_RDAC_3_MUX_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RDAC_3_MUX_em_count = sizeof(wcd_RDAC_3_MUX_em)/sizeof(wcd_element_event);

wcd_element_event wcd_HPHR_em[] = { 
	 { 1, &wcd_HPHR_SET_GAIN }, 
	 { 2, &wcd_HPHR_PRECHARGE }, 
	 { 3, &wcd_HPHR_ENABLE }, 
	 { 4, &wcd_HPHR_DISABLE }, 
	 {0, NULL} 
};
uint32 wcd_HPHR_em_count = sizeof(wcd_HPHR_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_3_MAIN_MUX_1_em[] = { 
	 { 1, &wcd_RX_3_MAIN_MUX_1_CONNECT }, 
	 { 2, &wcd_RX_3_MAIN_MUX_1_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_3_MAIN_MUX_1_em_count = sizeof(wcd_RX_3_MAIN_MUX_1_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_3_MAIN_MUX_2_em[] = { 
	 { 1, &wcd_RX_3_MAIN_MUX_2_CONNECT }, 
	 { 2, &wcd_RX_3_MAIN_MUX_2_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_3_MAIN_MUX_2_em_count = sizeof(wcd_RX_3_MAIN_MUX_2_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_3_MAIN_MUX_3_em[] = { 
	 { 1, &wcd_RX_3_MAIN_MUX_3_CONNECT }, 
	 { 2, &wcd_RX_3_MAIN_MUX_3_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_3_MAIN_MUX_3_em_count = sizeof(wcd_RX_3_MAIN_MUX_3_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_3_MAIN_MIXER_em[] = { 
	 { 1, &wcd_RX_3_MAIN_MIXER_CONNECT }, 
	 { 2, &wcd_RX_3_MAIN_MIXER_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_3_MAIN_MIXER_em_count = sizeof(wcd_RX_3_MAIN_MIXER_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_3_HPF_em[] = { 
	 { 1, &wcd_RX_3_HPF_BYPASS }, 
	 { 2, &wcd_RX_3_HPF_SET_CUTOFF_FREQ_4HZ }, 
	 { 3, &wcd_RX_3_HPF_SET_CUTOFF_FREQ_75HZ }, 
	 { 4, &wcd_RX_3_HPF_SET_CUTOFF_FREQ_150HZ }, 
	 {0, NULL} 
};
uint32 wcd_RX_3_HPF_em_count = sizeof(wcd_RX_3_HPF_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_3_PATH_em[] = { 
	 { 1, &wcd_RX_3_PATH_ENABLE }, 
	 { 2, &wcd_RX_3_PATH_DISABLE }, 
	 { 3, &wcd_RX_3_PATH_MUTE }, 
	 { 4, &wcd_RX_3_PATH_UNMUTE }, 
	 { 5, &wcd_RX_3_PATH_RESET }, 
	 { 6, &wcd_RX_3_PATH_SET_GAIN }, 
	 {0, NULL} 
};
uint32 wcd_RX_3_PATH_em_count = sizeof(wcd_RX_3_PATH_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_4_MAIN_MUX_1_em[] = { 
	 { 1, &wcd_RX_4_MAIN_MUX_1_CONNECT }, 
	 { 2, &wcd_RX_4_MAIN_MUX_1_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_4_MAIN_MUX_1_em_count = sizeof(wcd_RX_4_MAIN_MUX_1_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_4_MAIN_MUX_2_em[] = { 
	 { 1, &wcd_RX_4_MAIN_MUX_2_CONNECT }, 
	 { 2, &wcd_RX_4_MAIN_MUX_2_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_4_MAIN_MUX_2_em_count = sizeof(wcd_RX_4_MAIN_MUX_2_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_4_MAIN_MUX_3_em[] = { 
	 { 1, &wcd_RX_4_MAIN_MUX_3_CONNECT }, 
	 { 2, &wcd_RX_4_MAIN_MUX_3_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_4_MAIN_MUX_3_em_count = sizeof(wcd_RX_4_MAIN_MUX_3_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_4_MAIN_MIXER_em[] = { 
	 { 1, &wcd_RX_4_MAIN_MIXER_CONNECT }, 
	 { 2, &wcd_RX_4_MAIN_MIXER_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_4_MAIN_MIXER_em_count = sizeof(wcd_RX_4_MAIN_MIXER_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_4_ST_MUX_1_em[] = { 
	 { 1, &wcd_RX_4_ST_MUX_1_CONNECT }, 
	 { 2, &wcd_RX_4_ST_MUX_1_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_4_ST_MUX_1_em_count = sizeof(wcd_RX_4_ST_MUX_1_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_4_ST_MUX_2_em[] = { 
	 { 1, &wcd_RX_4_ST_MUX_2_CONNECT }, 
	 { 2, &wcd_RX_4_ST_MUX_2_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_4_ST_MUX_2_em_count = sizeof(wcd_RX_4_ST_MUX_2_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_4_ST_MIXER_em[] = { 
	 { 1, &wcd_RX_4_ST_MIXER_CONNECT }, 
	 { 2, &wcd_RX_4_ST_MIXER_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_4_ST_MIXER_em_count = sizeof(wcd_RX_4_ST_MIXER_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_4_HPF_em[] = { 
	 { 1, &wcd_RX_4_HPF_BYPASS }, 
	 { 2, &wcd_RX_4_HPF_SET_CUTOFF_FREQ_4HZ }, 
	 { 3, &wcd_RX_4_HPF_SET_CUTOFF_FREQ_75HZ }, 
	 { 4, &wcd_RX_4_HPF_SET_CUTOFF_FREQ_150HZ }, 
	 {0, NULL} 
};
uint32 wcd_RX_4_HPF_em_count = sizeof(wcd_RX_4_HPF_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_4_PATH_em[] = { 
	 { 1, &wcd_RX_4_PATH_ENABLE }, 
	 { 2, &wcd_RX_4_PATH_DISABLE }, 
	 { 3, &wcd_RX_4_PATH_MUTE }, 
	 { 4, &wcd_RX_4_PATH_UNMUTE }, 
	 { 5, &wcd_RX_4_PATH_RESET }, 
	 { 6, &wcd_RX_4_PATH_SET_GAIN }, 
	 {0, NULL} 
};
uint32 wcd_RX_4_PATH_em_count = sizeof(wcd_RX_4_PATH_em)/sizeof(wcd_element_event);

wcd_element_event wcd_MBIAS_MGR_1_em[] = { 
	 { 1, &wcd_MBIAS_MGR_1_SELECT_MICBIAS }, 
	 { 2, &wcd_MBIAS_MGR_1_SELECT_CFILTER }, 
	 { 3, &wcd_MBIAS_MGR_1_ENABLE }, 
	 { 4, &wcd_MBIAS_MGR_1_DISABLE }, 
	 { 5, &wcd_MBIAS_MGR_1_APPLY_DESIRED_VOLTAGE }, 
	 { 6, &wcd_MBIAS_MGR_1_APPLY_EXTERNAL_BYPASS_CAPACITOR }, 
	 { 7, &wcd_MBIAS_MGR_1_SET_INTERNAL_BIAS_RESISTOR }, 
	 { 8, &wcd_MBIAS_MGR_1_ENABLE_INTERNAL_BIAS }, 
	 { 9, &wcd_MBIAS_MGR_1_DISABLE_INTERNAL_BIAS }, 
	 {0, NULL} 
};
uint32 wcd_MBIAS_MGR_1_em_count = sizeof(wcd_MBIAS_MGR_1_em)/sizeof(wcd_element_event);

wcd_element_event wcd_DMIC_1_em[] = { 
	 { 1, &wcd_DMIC_1_ENABLE }, 
	 { 2, &wcd_DMIC_1_DISABLE }, 
	 { 3, &wcd_DMIC_1_SET_CLOCK_MODE }, 
	 {0, NULL} 
};
uint32 wcd_DMIC_1_em_count = sizeof(wcd_DMIC_1_em)/sizeof(wcd_element_event);

wcd_element_event wcd_MBIAS_MGR_2_em[] = { 
	 { 1, &wcd_MBIAS_MGR_2_SELECT_MICBIAS }, 
	 { 2, &wcd_MBIAS_MGR_2_SELECT_CFILTER }, 
	 { 3, &wcd_MBIAS_MGR_2_ENABLE }, 
	 { 4, &wcd_MBIAS_MGR_2_DISABLE }, 
	 { 5, &wcd_MBIAS_MGR_2_APPLY_DESIRED_VOLTAGE }, 
	 { 6, &wcd_MBIAS_MGR_2_APPLY_EXTERNAL_BYPASS_CAPACITOR }, 
	 { 7, &wcd_MBIAS_MGR_2_SET_INTERNAL_BIAS_RESISTOR }, 
	 { 8, &wcd_MBIAS_MGR_2_ENABLE_INTERNAL_BIAS }, 
	 { 9, &wcd_MBIAS_MGR_2_DISABLE_INTERNAL_BIAS }, 
	 {0, NULL} 
};
uint32 wcd_MBIAS_MGR_2_em_count = sizeof(wcd_MBIAS_MGR_2_em)/sizeof(wcd_element_event);

wcd_element_event wcd_DMIC_2_em[] = { 
	 { 1, &wcd_DMIC_2_ENABLE }, 
	 { 2, &wcd_DMIC_2_DISABLE }, 
	 { 3, &wcd_DMIC_2_SET_CLOCK_MODE }, 
	 {0, NULL} 
};
uint32 wcd_DMIC_2_em_count = sizeof(wcd_DMIC_2_em)/sizeof(wcd_element_event);

wcd_element_event wcd_MBIAS_MGR_3_em[] = { 
	 { 1, &wcd_MBIAS_MGR_3_SELECT_MICBIAS }, 
	 { 2, &wcd_MBIAS_MGR_3_SELECT_CFILTER }, 
	 { 3, &wcd_MBIAS_MGR_3_ENABLE }, 
	 { 4, &wcd_MBIAS_MGR_3_DISABLE }, 
	 { 5, &wcd_MBIAS_MGR_3_APPLY_DESIRED_VOLTAGE }, 
	 { 6, &wcd_MBIAS_MGR_3_APPLY_EXTERNAL_BYPASS_CAPACITOR }, 
	 { 7, &wcd_MBIAS_MGR_3_SET_INTERNAL_BIAS_RESISTOR }, 
	 { 8, &wcd_MBIAS_MGR_3_ENABLE_INTERNAL_BIAS }, 
	 { 9, &wcd_MBIAS_MGR_3_DISABLE_INTERNAL_BIAS }, 
	 {0, NULL} 
};
uint32 wcd_MBIAS_MGR_3_em_count = sizeof(wcd_MBIAS_MGR_3_em)/sizeof(wcd_element_event);

wcd_element_event wcd_DMIC_3_em[] = { 
	 { 1, &wcd_DMIC_3_ENABLE }, 
	 { 2, &wcd_DMIC_3_DISABLE }, 
	 { 3, &wcd_DMIC_3_SET_CLOCK_MODE }, 
	 {0, NULL} 
};
uint32 wcd_DMIC_3_em_count = sizeof(wcd_DMIC_3_em)/sizeof(wcd_element_event);

wcd_element_event wcd_MBIAS_MGR_4_em[] = { 
	 { 1, &wcd_MBIAS_MGR_4_SELECT_MICBIAS }, 
	 { 2, &wcd_MBIAS_MGR_4_SELECT_CFILTER }, 
	 { 3, &wcd_MBIAS_MGR_4_ENABLE }, 
	 { 4, &wcd_MBIAS_MGR_4_DISABLE }, 
	 { 5, &wcd_MBIAS_MGR_4_APPLY_DESIRED_VOLTAGE }, 
	 { 6, &wcd_MBIAS_MGR_4_APPLY_EXTERNAL_BYPASS_CAPACITOR }, 
	 { 7, &wcd_MBIAS_MGR_4_SET_INTERNAL_BIAS_RESISTOR }, 
	 { 8, &wcd_MBIAS_MGR_4_ENABLE_INTERNAL_BIAS }, 
	 { 9, &wcd_MBIAS_MGR_4_DISABLE_INTERNAL_BIAS }, 
	 {0, NULL} 
};
uint32 wcd_MBIAS_MGR_4_em_count = sizeof(wcd_MBIAS_MGR_4_em)/sizeof(wcd_element_event);

wcd_element_event wcd_DMIC_4_em[] = { 
	 { 1, &wcd_DMIC_4_ENABLE }, 
	 { 2, &wcd_DMIC_4_DISABLE }, 
	 { 3, &wcd_DMIC_4_SET_CLOCK_MODE }, 
	 {0, NULL} 
};
uint32 wcd_DMIC_4_em_count = sizeof(wcd_DMIC_4_em)/sizeof(wcd_element_event);

wcd_element_event wcd_MBIAS_MGR_5_em[] = { 
	 { 1, &wcd_MBIAS_MGR_5_SELECT_MICBIAS }, 
	 { 2, &wcd_MBIAS_MGR_5_SELECT_CFILTER }, 
	 { 3, &wcd_MBIAS_MGR_5_ENABLE }, 
	 { 4, &wcd_MBIAS_MGR_5_DISABLE }, 
	 { 5, &wcd_MBIAS_MGR_5_APPLY_DESIRED_VOLTAGE }, 
	 { 6, &wcd_MBIAS_MGR_5_APPLY_EXTERNAL_BYPASS_CAPACITOR }, 
	 { 7, &wcd_MBIAS_MGR_5_SET_INTERNAL_BIAS_RESISTOR }, 
	 { 8, &wcd_MBIAS_MGR_5_ENABLE_INTERNAL_BIAS }, 
	 { 9, &wcd_MBIAS_MGR_5_DISABLE_INTERNAL_BIAS }, 
	 {0, NULL} 
};
uint32 wcd_MBIAS_MGR_5_em_count = sizeof(wcd_MBIAS_MGR_5_em)/sizeof(wcd_element_event);

wcd_element_event wcd_ADC_1_em[] = { 
	 { 1, &wcd_ADC_1_ENABLE }, 
	 { 2, &wcd_ADC_1_DISABLE }, 
	 { 3, &wcd_ADC_1_SET_ANALOG_GAIN }, 
	 {0, NULL} 
};
uint32 wcd_ADC_1_em_count = sizeof(wcd_ADC_1_em)/sizeof(wcd_element_event);

wcd_element_event wcd_MBIAS_MGR_6_em[] = { 
	 { 1, &wcd_MBIAS_MGR_6_SELECT_MICBIAS }, 
	 { 2, &wcd_MBIAS_MGR_6_SELECT_CFILTER }, 
	 { 3, &wcd_MBIAS_MGR_6_ENABLE }, 
	 { 4, &wcd_MBIAS_MGR_6_DISABLE }, 
	 { 5, &wcd_MBIAS_MGR_6_APPLY_DESIRED_VOLTAGE }, 
	 { 6, &wcd_MBIAS_MGR_6_APPLY_EXTERNAL_BYPASS_CAPACITOR }, 
	 { 7, &wcd_MBIAS_MGR_6_SET_INTERNAL_BIAS_RESISTOR }, 
	 { 8, &wcd_MBIAS_MGR_6_ENABLE_INTERNAL_BIAS }, 
	 { 9, &wcd_MBIAS_MGR_6_DISABLE_INTERNAL_BIAS }, 
	 {0, NULL} 
};
uint32 wcd_MBIAS_MGR_6_em_count = sizeof(wcd_MBIAS_MGR_6_em)/sizeof(wcd_element_event);

wcd_element_event wcd_MBIAS_MGR_7_em[] = { 
	 { 1, &wcd_MBIAS_MGR_7_SELECT_MICBIAS }, 
	 { 2, &wcd_MBIAS_MGR_7_SELECT_CFILTER }, 
	 { 3, &wcd_MBIAS_MGR_7_ENABLE }, 
	 { 4, &wcd_MBIAS_MGR_7_DISABLE }, 
	 { 5, &wcd_MBIAS_MGR_7_APPLY_DESIRED_VOLTAGE }, 
	 { 6, &wcd_MBIAS_MGR_7_APPLY_EXTERNAL_BYPASS_CAPACITOR }, 
	 { 7, &wcd_MBIAS_MGR_7_SET_INTERNAL_BIAS_RESISTOR }, 
	 { 8, &wcd_MBIAS_MGR_7_ENABLE_INTERNAL_BIAS }, 
	 { 9, &wcd_MBIAS_MGR_7_DISABLE_INTERNAL_BIAS }, 
	 {0, NULL} 
};
uint32 wcd_MBIAS_MGR_7_em_count = sizeof(wcd_MBIAS_MGR_7_em)/sizeof(wcd_element_event);

wcd_element_event wcd_MBIAS_MGR_8_em[] = { 
	 { 1, &wcd_MBIAS_MGR_8_SELECT_MICBIAS }, 
	 { 2, &wcd_MBIAS_MGR_8_SELECT_CFILTER }, 
	 { 3, &wcd_MBIAS_MGR_8_ENABLE }, 
	 { 4, &wcd_MBIAS_MGR_8_DISABLE }, 
	 { 5, &wcd_MBIAS_MGR_8_APPLY_DESIRED_VOLTAGE }, 
	 { 6, &wcd_MBIAS_MGR_8_APPLY_EXTERNAL_BYPASS_CAPACITOR }, 
	 { 7, &wcd_MBIAS_MGR_8_SET_INTERNAL_BIAS_RESISTOR }, 
	 { 8, &wcd_MBIAS_MGR_8_ENABLE_INTERNAL_BIAS }, 
	 { 9, &wcd_MBIAS_MGR_8_DISABLE_INTERNAL_BIAS }, 
	 {0, NULL} 
};
uint32 wcd_MBIAS_MGR_8_em_count = sizeof(wcd_MBIAS_MGR_8_em)/sizeof(wcd_element_event);

wcd_element_event wcd_ADC_2_em[] = { 
	 { 1, &wcd_ADC_2_ENABLE }, 
	 { 2, &wcd_ADC_2_DISABLE }, 
	 { 3, &wcd_ADC_2_SET_ANALOG_GAIN }, 
	 {0, NULL} 
};
uint32 wcd_ADC_2_em_count = sizeof(wcd_ADC_2_em)/sizeof(wcd_element_event);

wcd_element_event wcd_TX_PATH_MUX_1_em[] = { 
	 { 1, &wcd_TX_PATH_MUX_1_CONNECT }, 
	 { 2, &wcd_TX_PATH_MUX_1_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_TX_PATH_MUX_1_em_count = sizeof(wcd_TX_PATH_MUX_1_em)/sizeof(wcd_element_event);

wcd_element_event wcd_TX_PATH_HPF_1_em[] = { 
	 { 1, &wcd_TX_PATH_HPF_1_BYPASS }, 
	 { 2, &wcd_TX_PATH_HPF_1_SET_CUTOFF_FREQ_4HZ }, 
	 { 3, &wcd_TX_PATH_HPF_1_SET_CUTOFF_FREQ_75HZ }, 
	 { 4, &wcd_TX_PATH_HPF_1_SET_CUTOFF_FREQ_150HZ }, 
	 {0, NULL} 
};
uint32 wcd_TX_PATH_HPF_1_em_count = sizeof(wcd_TX_PATH_HPF_1_em)/sizeof(wcd_element_event);

wcd_element_event wcd_TX_PATH_1_em[] = { 
	 { 1, &wcd_TX_PATH_1_ENABLE }, 
	 { 2, &wcd_TX_PATH_1_DISABLE }, 
	 { 3, &wcd_TX_PATH_1_RESET }, 
	 { 4, &wcd_TX_PATH_1_MUTE }, 
	 { 5, &wcd_TX_PATH_1_UNMUTE }, 
	 { 6, &wcd_TX_PATH_1_SET_GAIN }, 
	 {0, NULL} 
};
uint32 wcd_TX_PATH_1_em_count = sizeof(wcd_TX_PATH_1_em)/sizeof(wcd_element_event);

wcd_element_event wcd_TX_PATH_MUX_2_em[] = { 
	 { 1, &wcd_TX_PATH_MUX_2_CONNECT }, 
	 { 2, &wcd_TX_PATH_MUX_2_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_TX_PATH_MUX_2_em_count = sizeof(wcd_TX_PATH_MUX_2_em)/sizeof(wcd_element_event);

wcd_element_event wcd_TX_PATH_MUX_3_em[] = { 
	 { 1, &wcd_TX_PATH_MUX_3_CONNECT }, 
	 { 2, &wcd_TX_PATH_MUX_3_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_TX_PATH_MUX_3_em_count = sizeof(wcd_TX_PATH_MUX_3_em)/sizeof(wcd_element_event);

wcd_element_event wcd_TX_PATH_MUX_4_em[] = { 
	 { 1, &wcd_TX_PATH_MUX_4_CONNECT }, 
	 { 2, &wcd_TX_PATH_MUX_4_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_TX_PATH_MUX_4_em_count = sizeof(wcd_TX_PATH_MUX_4_em)/sizeof(wcd_element_event);

wcd_element_event wcd_TX_PATH_HPF_2_em[] = { 
	 { 1, &wcd_TX_PATH_HPF_2_BYPASS }, 
	 { 2, &wcd_TX_PATH_HPF_2_SET_CUTOFF_FREQ_4HZ }, 
	 { 3, &wcd_TX_PATH_HPF_2_SET_CUTOFF_FREQ_75HZ }, 
	 { 4, &wcd_TX_PATH_HPF_2_SET_CUTOFF_FREQ_150HZ }, 
	 {0, NULL} 
};
uint32 wcd_TX_PATH_HPF_2_em_count = sizeof(wcd_TX_PATH_HPF_2_em)/sizeof(wcd_element_event);

wcd_element_event wcd_TX_PATH_2_em[] = { 
	 { 1, &wcd_TX_PATH_2_ENABLE }, 
	 { 2, &wcd_TX_PATH_2_DISABLE }, 
	 { 3, &wcd_TX_PATH_2_RESET }, 
	 { 4, &wcd_TX_PATH_2_MUTE }, 
	 { 5, &wcd_TX_PATH_2_UNMUTE }, 
	 { 6, &wcd_TX_PATH_2_SET_GAIN }, 
	 {0, NULL} 
};
uint32 wcd_TX_PATH_2_em_count = sizeof(wcd_TX_PATH_2_em)/sizeof(wcd_element_event);

wcd_element_event wcd_TX_PATH_HPF_3_em[] = { 
	 { 1, &wcd_TX_PATH_HPF_3_BYPASS }, 
	 { 2, &wcd_TX_PATH_HPF_3_SET_CUTOFF_FREQ_4HZ }, 
	 { 3, &wcd_TX_PATH_HPF_3_SET_CUTOFF_FREQ_75HZ }, 
	 { 4, &wcd_TX_PATH_HPF_3_SET_CUTOFF_FREQ_150HZ }, 
	 {0, NULL} 
};
uint32 wcd_TX_PATH_HPF_3_em_count = sizeof(wcd_TX_PATH_HPF_3_em)/sizeof(wcd_element_event);

wcd_element_event wcd_TX_PATH_HPF_4_em[] = { 
	 { 1, &wcd_TX_PATH_HPF_4_BYPASS }, 
	 { 2, &wcd_TX_PATH_HPF_4_SET_CUTOFF_FREQ_4HZ }, 
	 { 3, &wcd_TX_PATH_HPF_4_SET_CUTOFF_FREQ_75HZ }, 
	 { 4, &wcd_TX_PATH_HPF_4_SET_CUTOFF_FREQ_150HZ }, 
	 {0, NULL} 
};
uint32 wcd_TX_PATH_HPF_4_em_count = sizeof(wcd_TX_PATH_HPF_4_em)/sizeof(wcd_element_event);

wcd_element_event wcd_TX_PATH_3_em[] = { 
	 { 1, &wcd_TX_PATH_3_ENABLE }, 
	 { 2, &wcd_TX_PATH_3_DISABLE }, 
	 { 3, &wcd_TX_PATH_3_RESET }, 
	 { 4, &wcd_TX_PATH_3_MUTE }, 
	 { 5, &wcd_TX_PATH_3_UNMUTE }, 
	 { 6, &wcd_TX_PATH_3_SET_GAIN }, 
	 {0, NULL} 
};
uint32 wcd_TX_PATH_3_em_count = sizeof(wcd_TX_PATH_3_em)/sizeof(wcd_element_event);

wcd_element_event wcd_TX_PATH_4_em[] = { 
	 { 1, &wcd_TX_PATH_4_ENABLE }, 
	 { 2, &wcd_TX_PATH_4_DISABLE }, 
	 { 3, &wcd_TX_PATH_4_RESET }, 
	 { 4, &wcd_TX_PATH_4_MUTE }, 
	 { 5, &wcd_TX_PATH_4_UNMUTE }, 
	 { 6, &wcd_TX_PATH_4_SET_GAIN }, 
	 {0, NULL} 
};
uint32 wcd_TX_PATH_4_em_count = sizeof(wcd_TX_PATH_4_em)/sizeof(wcd_element_event);

wcd_element_event wcd_SBTX_MUX_1_em[] = { 
	 { 1, &wcd_SBTX_MUX_1_CONNECT }, 
	 { 2, &wcd_SBTX_MUX_1_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_SBTX_MUX_1_em_count = sizeof(wcd_SBTX_MUX_1_em)/sizeof(wcd_element_event);

wcd_element_event wcd_SBTX_1_em[] = { 
	 { 1, &wcd_SBTX_1_SET_BIT_WIDTH_12BIT }, 
	 { 2, &wcd_SBTX_1_SET_BIT_WIDTH_16BIT }, 
	 { 3, &wcd_SBTX_1_SET_BIT_WIDTH_20BIT }, 
	 { 4, &wcd_SBTX_1_SET_BIT_WIDTH_24BIT }, 
	 {0, NULL} 
};
uint32 wcd_SBTX_1_em_count = sizeof(wcd_SBTX_1_em)/sizeof(wcd_element_event);

wcd_element_event wcd_SBTX_MUX_2_em[] = { 
	 { 1, &wcd_SBTX_MUX_2_CONNECT }, 
	 { 2, &wcd_SBTX_MUX_2_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_SBTX_MUX_2_em_count = sizeof(wcd_SBTX_MUX_2_em)/sizeof(wcd_element_event);

wcd_element_event wcd_SBTX_2_em[] = { 
	 { 1, &wcd_SBTX_2_SET_BIT_WIDTH_12BIT }, 
	 { 2, &wcd_SBTX_2_SET_BIT_WIDTH_16BIT }, 
	 { 3, &wcd_SBTX_2_SET_BIT_WIDTH_20BIT }, 
	 { 4, &wcd_SBTX_2_SET_BIT_WIDTH_24BIT }, 
	 {0, NULL} 
};
uint32 wcd_SBTX_2_em_count = sizeof(wcd_SBTX_2_em)/sizeof(wcd_element_event);

wcd_element_event wcd_SBTX_MUX_3_em[] = { 
	 { 1, &wcd_SBTX_MUX_3_CONNECT }, 
	 { 2, &wcd_SBTX_MUX_3_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_SBTX_MUX_3_em_count = sizeof(wcd_SBTX_MUX_3_em)/sizeof(wcd_element_event);

wcd_element_event wcd_SBTX_MUX_4_em[] = { 
	 { 1, &wcd_SBTX_MUX_4_CONNECT }, 
	 { 2, &wcd_SBTX_MUX_4_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_SBTX_MUX_4_em_count = sizeof(wcd_SBTX_MUX_4_em)/sizeof(wcd_element_event);

wcd_element_event wcd_SBTX_MUX_5_em[] = { 
	 { 1, &wcd_SBTX_MUX_5_CONNECT }, 
	 { 2, &wcd_SBTX_MUX_5_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_SBTX_MUX_5_em_count = sizeof(wcd_SBTX_MUX_5_em)/sizeof(wcd_element_event);

wcd_element_event wcd_SBTX_3_em[] = { 
	 { 1, &wcd_SBTX_3_SET_BIT_WIDTH_12BIT }, 
	 { 2, &wcd_SBTX_3_SET_BIT_WIDTH_16BIT }, 
	 { 3, &wcd_SBTX_3_SET_BIT_WIDTH_20BIT }, 
	 { 4, &wcd_SBTX_3_SET_BIT_WIDTH_24BIT }, 
	 {0, NULL} 
};
uint32 wcd_SBTX_3_em_count = sizeof(wcd_SBTX_3_em)/sizeof(wcd_element_event);

wcd_element_event wcd_SBTX_4_em[] = { 
	 { 1, &wcd_SBTX_4_SET_BIT_WIDTH_12BIT }, 
	 { 2, &wcd_SBTX_4_SET_BIT_WIDTH_16BIT }, 
	 { 3, &wcd_SBTX_4_SET_BIT_WIDTH_20BIT }, 
	 { 4, &wcd_SBTX_4_SET_BIT_WIDTH_24BIT }, 
	 {0, NULL} 
};
uint32 wcd_SBTX_4_em_count = sizeof(wcd_SBTX_4_em)/sizeof(wcd_element_event);

wcd_element_event wcd_SBTX_5_em[] = { 
	 { 1, &wcd_SBTX_5_SET_BIT_WIDTH_12BIT }, 
	 { 2, &wcd_SBTX_5_SET_BIT_WIDTH_16BIT }, 
	 { 3, &wcd_SBTX_5_SET_BIT_WIDTH_20BIT }, 
	 { 4, &wcd_SBTX_5_SET_BIT_WIDTH_24BIT }, 
	 {0, NULL} 
};
uint32 wcd_SBTX_5_em_count = sizeof(wcd_SBTX_5_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_4_INTERPOLATOR_MIXER_em[] = { 
	 { 1, &wcd_RX_4_INTERPOLATOR_MIXER_CONNECT }, 
	 { 2, &wcd_RX_4_INTERPOLATOR_MIXER_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RX_4_INTERPOLATOR_MIXER_em_count = sizeof(wcd_RX_4_INTERPOLATOR_MIXER_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_3_COMPANDER_em[] = { 
	 { 1, &wcd_RX_3_COMPANDER_ENABLE }, 
	 { 2, &wcd_RX_3_COMPANDER_DISABLE }, 
	 {0, NULL} 
};
uint32 wcd_RX_3_COMPANDER_em_count = sizeof(wcd_RX_3_COMPANDER_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_BIAS_3_em[] = { 
	 { 1, &wcd_RX_BIAS_3_ENABLE }, 
	 { 2, &wcd_RX_BIAS_3_DISABLE }, 
	 {0, NULL} 
};
uint32 wcd_RX_BIAS_3_em_count = sizeof(wcd_RX_BIAS_3_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RDAC_4_MUX_em[] = { 
	 { 1, &wcd_RDAC_4_MUX_CONNECT }, 
	 { 2, &wcd_RDAC_4_MUX_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_RDAC_4_MUX_em_count = sizeof(wcd_RDAC_4_MUX_em)/sizeof(wcd_element_event);

wcd_element_event wcd_LO1_L_P_em[] = { 
	 { 1, &wcd_LO1_L_P_SET_GAIN }, 
	 { 2, &wcd_LO1_L_P_PRECHARGE }, 
	 { 3, &wcd_LO1_L_P_ENABLE }, 
	 { 4, &wcd_LO1_L_P_DISABLE }, 
	 {0, NULL} 
};
uint32 wcd_LO1_L_P_em_count = sizeof(wcd_LO1_L_P_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_4_COMPANDER_em[] = { 
	 { 1, &wcd_RX_4_COMPANDER_ENABLE }, 
	 { 2, &wcd_RX_4_COMPANDER_DISABLE }, 
	 {0, NULL} 
};
uint32 wcd_RX_4_COMPANDER_em_count = sizeof(wcd_RX_4_COMPANDER_em)/sizeof(wcd_element_event);

wcd_element_event wcd_CLASS_CONTROLLER_4_em[] = { 
	 { 1, &wcd_CLASS_CONTROLLER_4_ENABLE }, 
	 { 2, &wcd_CLASS_CONTROLLER_4_DISABLE }, 
	 {0, NULL} 
};
uint32 wcd_CLASS_CONTROLLER_4_em_count = sizeof(wcd_CLASS_CONTROLLER_4_em)/sizeof(wcd_element_event);

wcd_element_event wcd_RX_BIAS_4_em[] = { 
	 { 1, &wcd_RX_BIAS_4_ENABLE }, 
	 { 2, &wcd_RX_BIAS_4_DISABLE }, 
	 {0, NULL} 
};
uint32 wcd_RX_BIAS_4_em_count = sizeof(wcd_RX_BIAS_4_em)/sizeof(wcd_element_event);

wcd_element_event wcd_SPEAKER_em[] = { 
	 { 1, &wcd_SPEAKER_SET_GAIN }, 
	 { 2, &wcd_SPEAKER_PRECHARGE }, 
	 { 3, &wcd_SPEAKER_ENABLE }, 
	 { 4, &wcd_SPEAKER_DISABLE }, 
	 {0, NULL} 
};
uint32 wcd_SPEAKER_em_count = sizeof(wcd_SPEAKER_em)/sizeof(wcd_element_event);

wcd_element_event *wcd_RDAC_5_DIFF_MUX_em = NULL;
uint32 wcd_RDAC_5_DIFF_MUX_em_count;

wcd_element_event wcd9306_RDAC_5_DIFF_MUX_em[] = { 
	 { 1, &wcd9306_RDAC_5_DIFF_MUX_CONNECT }, 
	 { 2, &wcd9306_RDAC_5_DIFF_MUX_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd9306_RDAC_5_DIFF_MUX_em_count = sizeof(wcd9306_RDAC_5_DIFF_MUX_em)/sizeof(wcd_element_event);

wcd_element_event wcd9302_RDAC_5_DIFF_MUX_em[] = { 
	 { 1, &wcd9302_RDAC_5_DIFF_MUX_CONNECT }, 
	 { 2, &wcd9302_RDAC_5_DIFF_MUX_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd9302_RDAC_5_DIFF_MUX_em_count = sizeof(wcd9302_RDAC_5_DIFF_MUX_em)/sizeof(wcd_element_event);

wcd_element_event wcd_LO2_R_M_em[] = { 
	 { 1, &wcd_LO2_R_M_SET_GAIN }, 
	 { 2, &wcd_LO2_R_M_PRECHARGE }, 
	 { 3, &wcd_LO2_R_M_ENABLE }, 
	 { 4, &wcd_LO2_R_M_DISABLE }, 
	 {0, NULL} 
};
uint32 wcd_LO2_R_M_em_count = sizeof(wcd_LO2_R_M_em)/sizeof(wcd_element_event);

wcd_element_event wcd_USONIC_MUX_em[] = { 
	 { 1, &wcd_USONIC_MUX_CONNECT }, 
	 { 2, &wcd_USONIC_MUX_DISCONNECT }, 
	 {0, NULL} 
};
uint32 wcd_USONIC_MUX_em_count = sizeof(wcd_USONIC_MUX_em)/sizeof(wcd_element_event);

wcd_element_event wcd_ADC_3_em[] = { 
	 { 1, &wcd_ADC_3_ENABLE }, 
	 { 2, &wcd_ADC_3_DISABLE }, 
	 { 3, &wcd_ADC_3_SET_ANALOG_GAIN }, 
	 {0, NULL} 
};
uint32 wcd_ADC_3_em_count = sizeof(wcd_ADC_3_em)/sizeof(wcd_element_event);

wcd_element_event wcd_ADC_4_em[] = { 
	 { 1, &wcd_ADC_4_ENABLE }, 
	 { 2, &wcd_ADC_4_DISABLE }, 
	 { 3, &wcd_ADC_4_SET_ANALOG_GAIN }, 
	 {0, NULL} 
};
uint32 wcd_ADC_4_em_count = sizeof(wcd_ADC_4_em)/sizeof(wcd_element_event);

wcd_element_event wcd_MBIAS_MGR_9_em[] = { 
	 { 1, &wcd_MBIAS_MGR_9_SELECT_MICBIAS }, 
	 { 2, &wcd_MBIAS_MGR_9_SELECT_CFILTER }, 
	 { 3, &wcd_MBIAS_MGR_9_ENABLE }, 
	 { 4, &wcd_MBIAS_MGR_9_DISABLE }, 
	 { 5, &wcd_MBIAS_MGR_9_APPLY_DESIRED_VOLTAGE }, 
	 { 6, &wcd_MBIAS_MGR_9_APPLY_EXTERNAL_BYPASS_CAPACITOR }, 
	 { 7, &wcd_MBIAS_MGR_9_SET_INTERNAL_BIAS_RESISTOR }, 
	 { 8, &wcd_MBIAS_MGR_9_ENABLE_INTERNAL_BIAS }, 
	 { 9, &wcd_MBIAS_MGR_9_DISABLE_INTERNAL_BIAS }, 
	 {0, NULL} 
};
uint32 wcd_MBIAS_MGR_9_em_count = sizeof(wcd_MBIAS_MGR_9_em)/sizeof(wcd_element_event);

wcd_element_event wcd_ADC_5_em[] = { 
	 { 1, &wcd_ADC_5_ENABLE }, 
	 { 2, &wcd_ADC_5_DISABLE }, 
	 { 3, &wcd_ADC_5_SET_ANALOG_GAIN }, 
	 {0, NULL} 
};
uint32 wcd_ADC_5_em_count = sizeof(wcd_ADC_5_em)/sizeof(wcd_element_event);

wcd_element_event wcd_ANC_CHANNEL_1_em[] = { 
	 {0, NULL} 
};
uint32 wcd_ANC_CHANNEL_1_em_count = sizeof(wcd_ANC_CHANNEL_1_em)/sizeof(wcd_element_event);

wcd_element_event wcd_ANC_CHANNEL_2_em[] = { 
	 {0, NULL} 
};
uint32 wcd_ANC_CHANNEL_2_em_count = sizeof(wcd_ANC_CHANNEL_2_em)/sizeof(wcd_element_event);


/**********************************************************************************
 *******************                 INITIALIZE                 *******************
 *********************************************************************************/
wcd_result wcd_initialize_elements(wcd_codec_info codec_info)
{
	 wcd_result return_code = E_WCD_SUCCESS;

	 if(codec_info.codec == E_WCD_WCD9306)
     {
	   wcd_SPEAKER_cea = wcd9306_SPEAKER_cea;
	   wcd_SPEAKER_cea_count = wcd9306_SPEAKER_cea_count;

	   wcd_RDAC_5_DIFF_MUX_cea = wcd9306_RDAC_5_DIFF_MUX_cea;
	   wcd_RDAC_5_DIFF_MUX_cea_count = wcd9306_RDAC_5_DIFF_MUX_cea_count;
	   wcd_RDAC_5_DIFF_MUX_em = wcd9306_RDAC_5_DIFF_MUX_em;
	   wcd_RDAC_5_DIFF_MUX_em_count = wcd9306_RDAC_5_DIFF_MUX_em_count;

	   wcd_RX_BIAS_3_cea = wcd9306_RX_BIAS_3_cea;
       wcd_RX_BIAS_3_cea_count = wcd9306_RX_BIAS_3_cea_count;

	   wcd_CLASS_CONTROLLER_1_cea = wcd9306_CLASS_CONTROLLER_1_cea;
	   wcd_CLASS_CONTROLLER_1_cea_count = wcd9306_CLASS_CONTROLLER_1_cea_count;

	   wcd_CLASS_CONTROLLER_2_cea = wcd9306_CLASS_CONTROLLER_2_cea;
	   wcd_CLASS_CONTROLLER_2_cea_count = wcd9306_CLASS_CONTROLLER_2_cea_count;

	   wcd_CLASS_CONTROLLER_4_cea = wcd9306_CLASS_CONTROLLER_4_cea;
	   wcd_CLASS_CONTROLLER_4_cea_count = wcd9306_CLASS_CONTROLLER_4_cea_count;
	 }
	 else
	 {
       wcd_SPEAKER_cea = wcd9302_SPEAKER_cea;
	   wcd_SPEAKER_cea_count = wcd9302_SPEAKER_cea_count;

	   wcd_RDAC_5_DIFF_MUX_cea = wcd9302_RDAC_5_DIFF_MUX_cea;
	   wcd_RDAC_5_DIFF_MUX_cea_count = wcd9302_RDAC_5_DIFF_MUX_cea_count;
	   wcd_RDAC_5_DIFF_MUX_em = wcd9302_RDAC_5_DIFF_MUX_em;
	   wcd_RDAC_5_DIFF_MUX_em_count = wcd9302_RDAC_5_DIFF_MUX_em_count;

	   wcd_RX_BIAS_3_cea = wcd9302_RX_BIAS_3_cea;
       wcd_RX_BIAS_3_cea_count = wcd9302_RX_BIAS_3_cea_count;

	   wcd_CLASS_CONTROLLER_1_cea = wcd9302_CLASS_CONTROLLER_1_cea;
	   wcd_CLASS_CONTROLLER_1_cea_count = wcd9302_CLASS_CONTROLLER_1_cea_count;

	   wcd_CLASS_CONTROLLER_2_cea = wcd9302_CLASS_CONTROLLER_2_cea;
	   wcd_CLASS_CONTROLLER_2_cea_count = wcd9302_CLASS_CONTROLLER_2_cea_count;

	   wcd_CLASS_CONTROLLER_4_cea = wcd9302_CLASS_CONTROLLER_4_cea;
	   wcd_CLASS_CONTROLLER_4_cea_count = wcd9302_CLASS_CONTROLLER_4_cea_count;
	 }

	 wcd_SBRX_1.connectable_elements = wcd_SBRX_1_cea; 
	 wcd_SBRX_1.connectable_elements_count = wcd_SBRX_1_cea_count; 
	 wcd_SBRX_1.events_map = wcd_SBRX_1_em; 
	 wcd_SBRX_1.events_count = wcd_SBRX_1_em_count; 

	 wcd_SBRX_2.connectable_elements = wcd_SBRX_2_cea; 
	 wcd_SBRX_2.connectable_elements_count = wcd_SBRX_2_cea_count; 
	 wcd_SBRX_2.events_map = wcd_SBRX_2_em; 
	 wcd_SBRX_2.events_count = wcd_SBRX_2_em_count; 

	 wcd_SBRX_3.connectable_elements = wcd_SBRX_3_cea; 
	 wcd_SBRX_3.connectable_elements_count = wcd_SBRX_3_cea_count; 
	 wcd_SBRX_3.events_map = wcd_SBRX_3_em; 
	 wcd_SBRX_3.events_count = wcd_SBRX_3_em_count; 

	 wcd_SBRX_4.connectable_elements = wcd_SBRX_4_cea; 
	 wcd_SBRX_4.connectable_elements_count = wcd_SBRX_4_cea_count; 
	 wcd_SBRX_4.events_map = wcd_SBRX_4_em; 
	 wcd_SBRX_4.events_count = wcd_SBRX_4_em_count; 

	 wcd_SBRX_5.connectable_elements = wcd_SBRX_5_cea; 
	 wcd_SBRX_5.connectable_elements_count = wcd_SBRX_5_cea_count; 
	 wcd_SBRX_5.events_map = wcd_SBRX_5_em; 
	 wcd_SBRX_5.events_count = wcd_SBRX_5_em_count; 

	 wcd_RDAC_1_MUX.connectable_elements = wcd_RDAC_1_MUX_cea; 
	 wcd_RDAC_1_MUX.connectable_elements_count = wcd_RDAC_1_MUX_cea_count; 
	 wcd_RDAC_1_MUX.events_map = wcd_RDAC_1_MUX_em; 
	 wcd_RDAC_1_MUX.events_count = wcd_RDAC_1_MUX_em_count; 

	 wcd_EAROUT.connectable_elements = wcd_EAROUT_cea; 
	 wcd_EAROUT.connectable_elements_count = wcd_EAROUT_cea_count; 
	 wcd_EAROUT.events_map = wcd_EAROUT_em; 
	 wcd_EAROUT.events_count = wcd_EAROUT_em_count; 

	 wcd_RX_1_MAIN_MUX_1.connectable_elements = wcd_RX_1_MAIN_MUX_1_cea; 
	 wcd_RX_1_MAIN_MUX_1.connectable_elements_count = wcd_RX_1_MAIN_MUX_1_cea_count; 
	 wcd_RX_1_MAIN_MUX_1.events_map = wcd_RX_1_MAIN_MUX_1_em; 
	 wcd_RX_1_MAIN_MUX_1.events_count = wcd_RX_1_MAIN_MUX_1_em_count; 

	 wcd_RX_1_MAIN_MUX_2.connectable_elements = wcd_RX_1_MAIN_MUX_2_cea; 
	 wcd_RX_1_MAIN_MUX_2.connectable_elements_count = wcd_RX_1_MAIN_MUX_2_cea_count; 
	 wcd_RX_1_MAIN_MUX_2.events_map = wcd_RX_1_MAIN_MUX_2_em; 
	 wcd_RX_1_MAIN_MUX_2.events_count = wcd_RX_1_MAIN_MUX_2_em_count; 

	 wcd_RX_1_MAIN_MUX_3.connectable_elements = wcd_RX_1_MAIN_MUX_3_cea; 
	 wcd_RX_1_MAIN_MUX_3.connectable_elements_count = wcd_RX_1_MAIN_MUX_3_cea_count; 
	 wcd_RX_1_MAIN_MUX_3.events_map = wcd_RX_1_MAIN_MUX_3_em; 
	 wcd_RX_1_MAIN_MUX_3.events_count = wcd_RX_1_MAIN_MUX_3_em_count; 

	 wcd_RX_1_MAIN_MIXER.connectable_elements = wcd_RX_1_MAIN_MIXER_cea; 
	 wcd_RX_1_MAIN_MIXER.connectable_elements_count = wcd_RX_1_MAIN_MIXER_cea_count; 
	 wcd_RX_1_MAIN_MIXER.events_map = wcd_RX_1_MAIN_MIXER_em; 
	 wcd_RX_1_MAIN_MIXER.events_count = wcd_RX_1_MAIN_MIXER_em_count; 

	 wcd_RX_1_ST_MIXER.connectable_elements = wcd_RX_1_ST_MIXER_cea; 
	 wcd_RX_1_ST_MIXER.connectable_elements_count = wcd_RX_1_ST_MIXER_cea_count; 
	 wcd_RX_1_ST_MIXER.events_map = wcd_RX_1_ST_MIXER_em; 
	 wcd_RX_1_ST_MIXER.events_count = wcd_RX_1_ST_MIXER_em_count; 

	 wcd_RX_1_ST_MUX_1.connectable_elements = wcd_RX_1_ST_MUX_1_cea; 
	 wcd_RX_1_ST_MUX_1.connectable_elements_count = wcd_RX_1_ST_MUX_1_cea_count; 
	 wcd_RX_1_ST_MUX_1.events_map = wcd_RX_1_ST_MUX_1_em; 
	 wcd_RX_1_ST_MUX_1.events_count = wcd_RX_1_ST_MUX_1_em_count; 

	 wcd_RX_1_ST_MUX_2.connectable_elements = wcd_RX_1_ST_MUX_2_cea; 
	 wcd_RX_1_ST_MUX_2.connectable_elements_count = wcd_RX_1_ST_MUX_2_cea_count; 
	 wcd_RX_1_ST_MUX_2.events_map = wcd_RX_1_ST_MUX_2_em; 
	 wcd_RX_1_ST_MUX_2.events_count = wcd_RX_1_ST_MUX_2_em_count; 

	 wcd_RX_1_HPF.connectable_elements = wcd_RX_1_HPF_cea; 
	 wcd_RX_1_HPF.connectable_elements_count = wcd_RX_1_HPF_cea_count; 
	 wcd_RX_1_HPF.events_map = wcd_RX_1_HPF_em; 
	 wcd_RX_1_HPF.events_count = wcd_RX_1_HPF_em_count; 

	 wcd_RX_1_PATH.connectable_elements = wcd_RX_1_PATH_cea; 
	 wcd_RX_1_PATH.connectable_elements_count = wcd_RX_1_PATH_cea_count; 
	 wcd_RX_1_PATH.events_map = wcd_RX_1_PATH_em; 
	 wcd_RX_1_PATH.events_count = wcd_RX_1_PATH_em_count; 

	 wcd_RX_1_COMPANDER.connectable_elements = wcd_RX_1_COMPANDER_cea; 
	 wcd_RX_1_COMPANDER.connectable_elements_count = wcd_RX_1_COMPANDER_cea_count; 
	 wcd_RX_1_COMPANDER.events_map = wcd_RX_1_COMPANDER_em; 
	 wcd_RX_1_COMPANDER.events_count = wcd_RX_1_COMPANDER_em_count; 

	 wcd_CLASS_CONTROLLER_1.connectable_elements = wcd_CLASS_CONTROLLER_1_cea; 
	 wcd_CLASS_CONTROLLER_1.connectable_elements_count = wcd_CLASS_CONTROLLER_1_cea_count; 
	 wcd_CLASS_CONTROLLER_1.events_map = wcd_CLASS_CONTROLLER_1_em; 
	 wcd_CLASS_CONTROLLER_1.events_count = wcd_CLASS_CONTROLLER_1_em_count; 

	 wcd_RX_BIAS_1.connectable_elements = wcd_RX_BIAS_1_cea; 
	 wcd_RX_BIAS_1.connectable_elements_count = wcd_RX_BIAS_1_cea_count; 
	 wcd_RX_BIAS_1.events_map = wcd_RX_BIAS_1_em; 
	 wcd_RX_BIAS_1.events_count = wcd_RX_BIAS_1_em_count; 

	 wcd_HPHL.connectable_elements = wcd_HPHL_cea; 
	 wcd_HPHL.connectable_elements_count = wcd_HPHL_cea_count; 
	 wcd_HPHL.events_map = wcd_HPHL_em; 
	 wcd_HPHL.events_count = wcd_HPHL_em_count; 

	 wcd_RX_2_MAIN_MUX_1.connectable_elements = wcd_RX_2_MAIN_MUX_1_cea; 
	 wcd_RX_2_MAIN_MUX_1.connectable_elements_count = wcd_RX_2_MAIN_MUX_1_cea_count; 
	 wcd_RX_2_MAIN_MUX_1.events_map = wcd_RX_2_MAIN_MUX_1_em; 
	 wcd_RX_2_MAIN_MUX_1.events_count = wcd_RX_2_MAIN_MUX_1_em_count; 

	 wcd_RX_2_MAIN_MUX_2.connectable_elements = wcd_RX_2_MAIN_MUX_2_cea; 
	 wcd_RX_2_MAIN_MUX_2.connectable_elements_count = wcd_RX_2_MAIN_MUX_2_cea_count; 
	 wcd_RX_2_MAIN_MUX_2.events_map = wcd_RX_2_MAIN_MUX_2_em; 
	 wcd_RX_2_MAIN_MUX_2.events_count = wcd_RX_2_MAIN_MUX_2_em_count; 

	 wcd_RX_2_MAIN_MUX_3.connectable_elements = wcd_RX_2_MAIN_MUX_3_cea; 
	 wcd_RX_2_MAIN_MUX_3.connectable_elements_count = wcd_RX_2_MAIN_MUX_3_cea_count; 
	 wcd_RX_2_MAIN_MUX_3.events_map = wcd_RX_2_MAIN_MUX_3_em; 
	 wcd_RX_2_MAIN_MUX_3.events_count = wcd_RX_2_MAIN_MUX_3_em_count; 

	 wcd_RX_2_MAIN_MIXER.connectable_elements = wcd_RX_2_MAIN_MIXER_cea; 
	 wcd_RX_2_MAIN_MIXER.connectable_elements_count = wcd_RX_2_MAIN_MIXER_cea_count; 
	 wcd_RX_2_MAIN_MIXER.events_map = wcd_RX_2_MAIN_MIXER_em; 
	 wcd_RX_2_MAIN_MIXER.events_count = wcd_RX_2_MAIN_MIXER_em_count; 

	 wcd_RX_2_ST_MUX_1.connectable_elements = wcd_RX_2_ST_MUX_1_cea; 
	 wcd_RX_2_ST_MUX_1.connectable_elements_count = wcd_RX_2_ST_MUX_1_cea_count; 
	 wcd_RX_2_ST_MUX_1.events_map = wcd_RX_2_ST_MUX_1_em; 
	 wcd_RX_2_ST_MUX_1.events_count = wcd_RX_2_ST_MUX_1_em_count; 

	 wcd_RX_2_ST_MUX_2.connectable_elements = wcd_RX_2_ST_MUX_2_cea; 
	 wcd_RX_2_ST_MUX_2.connectable_elements_count = wcd_RX_2_ST_MUX_2_cea_count; 
	 wcd_RX_2_ST_MUX_2.events_map = wcd_RX_2_ST_MUX_2_em; 
	 wcd_RX_2_ST_MUX_2.events_count = wcd_RX_2_ST_MUX_2_em_count; 

	 wcd_RX_2_ST_MIXER.connectable_elements = wcd_RX_2_ST_MIXER_cea; 
	 wcd_RX_2_ST_MIXER.connectable_elements_count = wcd_RX_2_ST_MIXER_cea_count; 
	 wcd_RX_2_ST_MIXER.events_map = wcd_RX_2_ST_MIXER_em; 
	 wcd_RX_2_ST_MIXER.events_count = wcd_RX_2_ST_MIXER_em_count; 

	 wcd_RX_2_HPF.connectable_elements = wcd_RX_2_HPF_cea; 
	 wcd_RX_2_HPF.connectable_elements_count = wcd_RX_2_HPF_cea_count; 
	 wcd_RX_2_HPF.events_map = wcd_RX_2_HPF_em; 
	 wcd_RX_2_HPF.events_count = wcd_RX_2_HPF_em_count; 

	 wcd_RX_2_PATH.connectable_elements = wcd_RX_2_PATH_cea; 
	 wcd_RX_2_PATH.connectable_elements_count = wcd_RX_2_PATH_cea_count; 
	 wcd_RX_2_PATH.events_map = wcd_RX_2_PATH_em; 
	 wcd_RX_2_PATH.events_count = wcd_RX_2_PATH_em_count; 

	 wcd_RX_2_COMPANDER.connectable_elements = wcd_RX_2_COMPANDER_cea; 
	 wcd_RX_2_COMPANDER.connectable_elements_count = wcd_RX_2_COMPANDER_cea_count; 
	 wcd_RX_2_COMPANDER.events_map = wcd_RX_2_COMPANDER_em; 
	 wcd_RX_2_COMPANDER.events_count = wcd_RX_2_COMPANDER_em_count; 

	 wcd_CLASS_CONTROLLER_2.connectable_elements = wcd_CLASS_CONTROLLER_2_cea; 
	 wcd_CLASS_CONTROLLER_2.connectable_elements_count = wcd_CLASS_CONTROLLER_2_cea_count; 
	 wcd_CLASS_CONTROLLER_2.events_map = wcd_CLASS_CONTROLLER_2_em; 
	 wcd_CLASS_CONTROLLER_2.events_count = wcd_CLASS_CONTROLLER_2_em_count; 

	 wcd_RX_BIAS_2.connectable_elements = wcd_RX_BIAS_2_cea; 
	 wcd_RX_BIAS_2.connectable_elements_count = wcd_RX_BIAS_2_cea_count; 
	 wcd_RX_BIAS_2.events_map = wcd_RX_BIAS_2_em; 
	 wcd_RX_BIAS_2.events_count = wcd_RX_BIAS_2_em_count; 

	 wcd_RDAC_3_MUX.connectable_elements = wcd_RDAC_3_MUX_cea; 
	 wcd_RDAC_3_MUX.connectable_elements_count = wcd_RDAC_3_MUX_cea_count; 
	 wcd_RDAC_3_MUX.events_map = wcd_RDAC_3_MUX_em; 
	 wcd_RDAC_3_MUX.events_count = wcd_RDAC_3_MUX_em_count; 

	 wcd_HPHR.connectable_elements = wcd_HPHR_cea; 
	 wcd_HPHR.connectable_elements_count = wcd_HPHR_cea_count; 
	 wcd_HPHR.events_map = wcd_HPHR_em; 
	 wcd_HPHR.events_count = wcd_HPHR_em_count; 

	 wcd_RX_3_MAIN_MUX_1.connectable_elements = wcd_RX_3_MAIN_MUX_1_cea; 
	 wcd_RX_3_MAIN_MUX_1.connectable_elements_count = wcd_RX_3_MAIN_MUX_1_cea_count; 
	 wcd_RX_3_MAIN_MUX_1.events_map = wcd_RX_3_MAIN_MUX_1_em; 
	 wcd_RX_3_MAIN_MUX_1.events_count = wcd_RX_3_MAIN_MUX_1_em_count; 

	 wcd_RX_3_MAIN_MUX_2.connectable_elements = wcd_RX_3_MAIN_MUX_2_cea; 
	 wcd_RX_3_MAIN_MUX_2.connectable_elements_count = wcd_RX_3_MAIN_MUX_2_cea_count; 
	 wcd_RX_3_MAIN_MUX_2.events_map = wcd_RX_3_MAIN_MUX_2_em; 
	 wcd_RX_3_MAIN_MUX_2.events_count = wcd_RX_3_MAIN_MUX_2_em_count; 

	 wcd_RX_3_MAIN_MUX_3.connectable_elements = wcd_RX_3_MAIN_MUX_3_cea; 
	 wcd_RX_3_MAIN_MUX_3.connectable_elements_count = wcd_RX_3_MAIN_MUX_3_cea_count; 
	 wcd_RX_3_MAIN_MUX_3.events_map = wcd_RX_3_MAIN_MUX_3_em; 
	 wcd_RX_3_MAIN_MUX_3.events_count = wcd_RX_3_MAIN_MUX_3_em_count; 

	 wcd_RX_3_MAIN_MIXER.connectable_elements = wcd_RX_3_MAIN_MIXER_cea; 
	 wcd_RX_3_MAIN_MIXER.connectable_elements_count = wcd_RX_3_MAIN_MIXER_cea_count; 
	 wcd_RX_3_MAIN_MIXER.events_map = wcd_RX_3_MAIN_MIXER_em; 
	 wcd_RX_3_MAIN_MIXER.events_count = wcd_RX_3_MAIN_MIXER_em_count; 

	 wcd_RX_3_HPF.connectable_elements = wcd_RX_3_HPF_cea; 
	 wcd_RX_3_HPF.connectable_elements_count = wcd_RX_3_HPF_cea_count; 
	 wcd_RX_3_HPF.events_map = wcd_RX_3_HPF_em; 
	 wcd_RX_3_HPF.events_count = wcd_RX_3_HPF_em_count; 

	 wcd_RX_3_PATH.connectable_elements = wcd_RX_3_PATH_cea; 
	 wcd_RX_3_PATH.connectable_elements_count = wcd_RX_3_PATH_cea_count; 
	 wcd_RX_3_PATH.events_map = wcd_RX_3_PATH_em; 
	 wcd_RX_3_PATH.events_count = wcd_RX_3_PATH_em_count; 

	 wcd_RX_4_MAIN_MUX_1.connectable_elements = wcd_RX_4_MAIN_MUX_1_cea; 
	 wcd_RX_4_MAIN_MUX_1.connectable_elements_count = wcd_RX_4_MAIN_MUX_1_cea_count; 
	 wcd_RX_4_MAIN_MUX_1.events_map = wcd_RX_4_MAIN_MUX_1_em; 
	 wcd_RX_4_MAIN_MUX_1.events_count = wcd_RX_4_MAIN_MUX_1_em_count; 

	 wcd_RX_4_MAIN_MUX_2.connectable_elements = wcd_RX_4_MAIN_MUX_2_cea; 
	 wcd_RX_4_MAIN_MUX_2.connectable_elements_count = wcd_RX_4_MAIN_MUX_2_cea_count; 
	 wcd_RX_4_MAIN_MUX_2.events_map = wcd_RX_4_MAIN_MUX_2_em; 
	 wcd_RX_4_MAIN_MUX_2.events_count = wcd_RX_4_MAIN_MUX_2_em_count; 

	 wcd_RX_4_MAIN_MUX_3.connectable_elements = wcd_RX_4_MAIN_MUX_3_cea; 
	 wcd_RX_4_MAIN_MUX_3.connectable_elements_count = wcd_RX_4_MAIN_MUX_3_cea_count; 
	 wcd_RX_4_MAIN_MUX_3.events_map = wcd_RX_4_MAIN_MUX_3_em; 
	 wcd_RX_4_MAIN_MUX_3.events_count = wcd_RX_4_MAIN_MUX_3_em_count; 

	 wcd_RX_4_MAIN_MIXER.connectable_elements = wcd_RX_4_MAIN_MIXER_cea; 
	 wcd_RX_4_MAIN_MIXER.connectable_elements_count = wcd_RX_4_MAIN_MIXER_cea_count; 
	 wcd_RX_4_MAIN_MIXER.events_map = wcd_RX_4_MAIN_MIXER_em; 
	 wcd_RX_4_MAIN_MIXER.events_count = wcd_RX_4_MAIN_MIXER_em_count; 

	 wcd_RX_4_ST_MUX_1.connectable_elements = wcd_RX_4_ST_MUX_1_cea; 
	 wcd_RX_4_ST_MUX_1.connectable_elements_count = wcd_RX_4_ST_MUX_1_cea_count; 
	 wcd_RX_4_ST_MUX_1.events_map = wcd_RX_4_ST_MUX_1_em; 
	 wcd_RX_4_ST_MUX_1.events_count = wcd_RX_4_ST_MUX_1_em_count; 

	 wcd_RX_4_ST_MUX_2.connectable_elements = wcd_RX_4_ST_MUX_2_cea; 
	 wcd_RX_4_ST_MUX_2.connectable_elements_count = wcd_RX_4_ST_MUX_2_cea_count; 
	 wcd_RX_4_ST_MUX_2.events_map = wcd_RX_4_ST_MUX_2_em; 
	 wcd_RX_4_ST_MUX_2.events_count = wcd_RX_4_ST_MUX_2_em_count; 

	 wcd_RX_4_ST_MIXER.connectable_elements = wcd_RX_4_ST_MIXER_cea; 
	 wcd_RX_4_ST_MIXER.connectable_elements_count = wcd_RX_4_ST_MIXER_cea_count; 
	 wcd_RX_4_ST_MIXER.events_map = wcd_RX_4_ST_MIXER_em; 
	 wcd_RX_4_ST_MIXER.events_count = wcd_RX_4_ST_MIXER_em_count; 

	 wcd_RX_4_HPF.connectable_elements = wcd_RX_4_HPF_cea; 
	 wcd_RX_4_HPF.connectable_elements_count = wcd_RX_4_HPF_cea_count; 
	 wcd_RX_4_HPF.events_map = wcd_RX_4_HPF_em; 
	 wcd_RX_4_HPF.events_count = wcd_RX_4_HPF_em_count; 

	 wcd_RX_4_PATH.connectable_elements = wcd_RX_4_PATH_cea; 
	 wcd_RX_4_PATH.connectable_elements_count = wcd_RX_4_PATH_cea_count; 
	 wcd_RX_4_PATH.events_map = wcd_RX_4_PATH_em; 
	 wcd_RX_4_PATH.events_count = wcd_RX_4_PATH_em_count; 

	 wcd_MBIAS_MGR_1.connectable_elements = wcd_MBIAS_MGR_1_cea; 
	 wcd_MBIAS_MGR_1.connectable_elements_count = wcd_MBIAS_MGR_1_cea_count; 
	 wcd_MBIAS_MGR_1.events_map = wcd_MBIAS_MGR_1_em; 
	 wcd_MBIAS_MGR_1.events_count = wcd_MBIAS_MGR_1_em_count; 

	 wcd_DMIC_1.connectable_elements = wcd_DMIC_1_cea; 
	 wcd_DMIC_1.connectable_elements_count = wcd_DMIC_1_cea_count; 
	 wcd_DMIC_1.events_map = wcd_DMIC_1_em; 
	 wcd_DMIC_1.events_count = wcd_DMIC_1_em_count; 

	 wcd_MBIAS_MGR_2.connectable_elements = wcd_MBIAS_MGR_2_cea; 
	 wcd_MBIAS_MGR_2.connectable_elements_count = wcd_MBIAS_MGR_2_cea_count; 
	 wcd_MBIAS_MGR_2.events_map = wcd_MBIAS_MGR_2_em; 
	 wcd_MBIAS_MGR_2.events_count = wcd_MBIAS_MGR_2_em_count; 

	 wcd_DMIC_2.connectable_elements = wcd_DMIC_2_cea; 
	 wcd_DMIC_2.connectable_elements_count = wcd_DMIC_2_cea_count; 
	 wcd_DMIC_2.events_map = wcd_DMIC_2_em; 
	 wcd_DMIC_2.events_count = wcd_DMIC_2_em_count; 

	 wcd_MBIAS_MGR_3.connectable_elements = wcd_MBIAS_MGR_3_cea; 
	 wcd_MBIAS_MGR_3.connectable_elements_count = wcd_MBIAS_MGR_3_cea_count; 
	 wcd_MBIAS_MGR_3.events_map = wcd_MBIAS_MGR_3_em; 
	 wcd_MBIAS_MGR_3.events_count = wcd_MBIAS_MGR_3_em_count; 

	 wcd_DMIC_3.connectable_elements = wcd_DMIC_3_cea; 
	 wcd_DMIC_3.connectable_elements_count = wcd_DMIC_3_cea_count; 
	 wcd_DMIC_3.events_map = wcd_DMIC_3_em; 
	 wcd_DMIC_3.events_count = wcd_DMIC_3_em_count; 

	 wcd_MBIAS_MGR_4.connectable_elements = wcd_MBIAS_MGR_4_cea; 
	 wcd_MBIAS_MGR_4.connectable_elements_count = wcd_MBIAS_MGR_4_cea_count; 
	 wcd_MBIAS_MGR_4.events_map = wcd_MBIAS_MGR_4_em; 
	 wcd_MBIAS_MGR_4.events_count = wcd_MBIAS_MGR_4_em_count; 

	 wcd_DMIC_4.connectable_elements = wcd_DMIC_4_cea; 
	 wcd_DMIC_4.connectable_elements_count = wcd_DMIC_4_cea_count; 
	 wcd_DMIC_4.events_map = wcd_DMIC_4_em; 
	 wcd_DMIC_4.events_count = wcd_DMIC_4_em_count; 

	 wcd_MBIAS_MGR_5.connectable_elements = wcd_MBIAS_MGR_5_cea; 
	 wcd_MBIAS_MGR_5.connectable_elements_count = wcd_MBIAS_MGR_5_cea_count; 
	 wcd_MBIAS_MGR_5.events_map = wcd_MBIAS_MGR_5_em; 
	 wcd_MBIAS_MGR_5.events_count = wcd_MBIAS_MGR_5_em_count; 

	 wcd_ADC_1.connectable_elements = wcd_ADC_1_cea; 
	 wcd_ADC_1.connectable_elements_count = wcd_ADC_1_cea_count; 
	 wcd_ADC_1.events_map = wcd_ADC_1_em; 
	 wcd_ADC_1.events_count = wcd_ADC_1_em_count; 

	 wcd_MBIAS_MGR_6.connectable_elements = wcd_MBIAS_MGR_6_cea; 
	 wcd_MBIAS_MGR_6.connectable_elements_count = wcd_MBIAS_MGR_6_cea_count; 
	 wcd_MBIAS_MGR_6.events_map = wcd_MBIAS_MGR_6_em; 
	 wcd_MBIAS_MGR_6.events_count = wcd_MBIAS_MGR_6_em_count; 

	 wcd_MBIAS_MGR_7.connectable_elements = wcd_MBIAS_MGR_7_cea; 
	 wcd_MBIAS_MGR_7.connectable_elements_count = wcd_MBIAS_MGR_7_cea_count; 
	 wcd_MBIAS_MGR_7.events_map = wcd_MBIAS_MGR_7_em; 
	 wcd_MBIAS_MGR_7.events_count = wcd_MBIAS_MGR_7_em_count; 

	 wcd_MBIAS_MGR_8.connectable_elements = wcd_MBIAS_MGR_8_cea; 
	 wcd_MBIAS_MGR_8.connectable_elements_count = wcd_MBIAS_MGR_8_cea_count; 
	 wcd_MBIAS_MGR_8.events_map = wcd_MBIAS_MGR_8_em; 
	 wcd_MBIAS_MGR_8.events_count = wcd_MBIAS_MGR_8_em_count; 

	 wcd_ADC_2.connectable_elements = wcd_ADC_2_cea; 
	 wcd_ADC_2.connectable_elements_count = wcd_ADC_2_cea_count; 
	 wcd_ADC_2.events_map = wcd_ADC_2_em; 
	 wcd_ADC_2.events_count = wcd_ADC_2_em_count; 

	 wcd_TX_PATH_MUX_1.connectable_elements = wcd_TX_PATH_MUX_1_cea; 
	 wcd_TX_PATH_MUX_1.connectable_elements_count = wcd_TX_PATH_MUX_1_cea_count; 
	 wcd_TX_PATH_MUX_1.events_map = wcd_TX_PATH_MUX_1_em; 
	 wcd_TX_PATH_MUX_1.events_count = wcd_TX_PATH_MUX_1_em_count; 

	 wcd_TX_PATH_HPF_1.connectable_elements = wcd_TX_PATH_HPF_1_cea; 
	 wcd_TX_PATH_HPF_1.connectable_elements_count = wcd_TX_PATH_HPF_1_cea_count; 
	 wcd_TX_PATH_HPF_1.events_map = wcd_TX_PATH_HPF_1_em; 
	 wcd_TX_PATH_HPF_1.events_count = wcd_TX_PATH_HPF_1_em_count; 

	 wcd_TX_PATH_1.connectable_elements = wcd_TX_PATH_1_cea; 
	 wcd_TX_PATH_1.connectable_elements_count = wcd_TX_PATH_1_cea_count; 
	 wcd_TX_PATH_1.events_map = wcd_TX_PATH_1_em; 
	 wcd_TX_PATH_1.events_count = wcd_TX_PATH_1_em_count; 

	 wcd_TX_PATH_MUX_2.connectable_elements = wcd_TX_PATH_MUX_2_cea; 
	 wcd_TX_PATH_MUX_2.connectable_elements_count = wcd_TX_PATH_MUX_2_cea_count; 
	 wcd_TX_PATH_MUX_2.events_map = wcd_TX_PATH_MUX_2_em; 
	 wcd_TX_PATH_MUX_2.events_count = wcd_TX_PATH_MUX_2_em_count; 

	 wcd_TX_PATH_MUX_3.connectable_elements = wcd_TX_PATH_MUX_3_cea; 
	 wcd_TX_PATH_MUX_3.connectable_elements_count = wcd_TX_PATH_MUX_3_cea_count; 
	 wcd_TX_PATH_MUX_3.events_map = wcd_TX_PATH_MUX_3_em; 
	 wcd_TX_PATH_MUX_3.events_count = wcd_TX_PATH_MUX_3_em_count; 

	 wcd_TX_PATH_MUX_4.connectable_elements = wcd_TX_PATH_MUX_4_cea; 
	 wcd_TX_PATH_MUX_4.connectable_elements_count = wcd_TX_PATH_MUX_4_cea_count; 
	 wcd_TX_PATH_MUX_4.events_map = wcd_TX_PATH_MUX_4_em; 
	 wcd_TX_PATH_MUX_4.events_count = wcd_TX_PATH_MUX_4_em_count; 

	 wcd_TX_PATH_HPF_2.connectable_elements = wcd_TX_PATH_HPF_2_cea; 
	 wcd_TX_PATH_HPF_2.connectable_elements_count = wcd_TX_PATH_HPF_2_cea_count; 
	 wcd_TX_PATH_HPF_2.events_map = wcd_TX_PATH_HPF_2_em; 
	 wcd_TX_PATH_HPF_2.events_count = wcd_TX_PATH_HPF_2_em_count; 

	 wcd_TX_PATH_2.connectable_elements = wcd_TX_PATH_2_cea; 
	 wcd_TX_PATH_2.connectable_elements_count = wcd_TX_PATH_2_cea_count; 
	 wcd_TX_PATH_2.events_map = wcd_TX_PATH_2_em; 
	 wcd_TX_PATH_2.events_count = wcd_TX_PATH_2_em_count; 

	 wcd_TX_PATH_HPF_3.connectable_elements = wcd_TX_PATH_HPF_3_cea; 
	 wcd_TX_PATH_HPF_3.connectable_elements_count = wcd_TX_PATH_HPF_3_cea_count; 
	 wcd_TX_PATH_HPF_3.events_map = wcd_TX_PATH_HPF_3_em; 
	 wcd_TX_PATH_HPF_3.events_count = wcd_TX_PATH_HPF_3_em_count; 

	 wcd_TX_PATH_HPF_4.connectable_elements = wcd_TX_PATH_HPF_4_cea; 
	 wcd_TX_PATH_HPF_4.connectable_elements_count = wcd_TX_PATH_HPF_4_cea_count; 
	 wcd_TX_PATH_HPF_4.events_map = wcd_TX_PATH_HPF_4_em; 
	 wcd_TX_PATH_HPF_4.events_count = wcd_TX_PATH_HPF_4_em_count; 

	 wcd_TX_PATH_3.connectable_elements = wcd_TX_PATH_3_cea; 
	 wcd_TX_PATH_3.connectable_elements_count = wcd_TX_PATH_3_cea_count; 
	 wcd_TX_PATH_3.events_map = wcd_TX_PATH_3_em; 
	 wcd_TX_PATH_3.events_count = wcd_TX_PATH_3_em_count; 

	 wcd_TX_PATH_4.connectable_elements = wcd_TX_PATH_4_cea; 
	 wcd_TX_PATH_4.connectable_elements_count = wcd_TX_PATH_4_cea_count; 
	 wcd_TX_PATH_4.events_map = wcd_TX_PATH_4_em; 
	 wcd_TX_PATH_4.events_count = wcd_TX_PATH_4_em_count; 

	 wcd_SBTX_MUX_1.connectable_elements = wcd_SBTX_MUX_1_cea; 
	 wcd_SBTX_MUX_1.connectable_elements_count = wcd_SBTX_MUX_1_cea_count; 
	 wcd_SBTX_MUX_1.events_map = wcd_SBTX_MUX_1_em; 
	 wcd_SBTX_MUX_1.events_count = wcd_SBTX_MUX_1_em_count; 

	 wcd_SBTX_1.connectable_elements = wcd_SBTX_1_cea; 
	 wcd_SBTX_1.connectable_elements_count = wcd_SBTX_1_cea_count; 
	 wcd_SBTX_1.events_map = wcd_SBTX_1_em; 
	 wcd_SBTX_1.events_count = wcd_SBTX_1_em_count; 

	 wcd_SBTX_MUX_2.connectable_elements = wcd_SBTX_MUX_2_cea; 
	 wcd_SBTX_MUX_2.connectable_elements_count = wcd_SBTX_MUX_2_cea_count; 
	 wcd_SBTX_MUX_2.events_map = wcd_SBTX_MUX_2_em; 
	 wcd_SBTX_MUX_2.events_count = wcd_SBTX_MUX_2_em_count; 

	 wcd_SBTX_2.connectable_elements = wcd_SBTX_2_cea; 
	 wcd_SBTX_2.connectable_elements_count = wcd_SBTX_2_cea_count; 
	 wcd_SBTX_2.events_map = wcd_SBTX_2_em; 
	 wcd_SBTX_2.events_count = wcd_SBTX_2_em_count; 

	 wcd_SBTX_MUX_3.connectable_elements = wcd_SBTX_MUX_3_cea; 
	 wcd_SBTX_MUX_3.connectable_elements_count = wcd_SBTX_MUX_3_cea_count; 
	 wcd_SBTX_MUX_3.events_map = wcd_SBTX_MUX_3_em; 
	 wcd_SBTX_MUX_3.events_count = wcd_SBTX_MUX_3_em_count; 

	 wcd_SBTX_MUX_4.connectable_elements = wcd_SBTX_MUX_4_cea; 
	 wcd_SBTX_MUX_4.connectable_elements_count = wcd_SBTX_MUX_4_cea_count; 
	 wcd_SBTX_MUX_4.events_map = wcd_SBTX_MUX_4_em; 
	 wcd_SBTX_MUX_4.events_count = wcd_SBTX_MUX_4_em_count; 

	 wcd_SBTX_MUX_5.connectable_elements = wcd_SBTX_MUX_5_cea; 
	 wcd_SBTX_MUX_5.connectable_elements_count = wcd_SBTX_MUX_5_cea_count; 
	 wcd_SBTX_MUX_5.events_map = wcd_SBTX_MUX_5_em; 
	 wcd_SBTX_MUX_5.events_count = wcd_SBTX_MUX_5_em_count; 

	 wcd_SBTX_3.connectable_elements = wcd_SBTX_3_cea; 
	 wcd_SBTX_3.connectable_elements_count = wcd_SBTX_3_cea_count; 
	 wcd_SBTX_3.events_map = wcd_SBTX_3_em; 
	 wcd_SBTX_3.events_count = wcd_SBTX_3_em_count; 

	 wcd_SBTX_4.connectable_elements = wcd_SBTX_4_cea; 
	 wcd_SBTX_4.connectable_elements_count = wcd_SBTX_4_cea_count; 
	 wcd_SBTX_4.events_map = wcd_SBTX_4_em; 
	 wcd_SBTX_4.events_count = wcd_SBTX_4_em_count; 

	 wcd_SBTX_5.connectable_elements = wcd_SBTX_5_cea; 
	 wcd_SBTX_5.connectable_elements_count = wcd_SBTX_5_cea_count; 
	 wcd_SBTX_5.events_map = wcd_SBTX_5_em; 
	 wcd_SBTX_5.events_count = wcd_SBTX_5_em_count; 

	 wcd_RX_4_INTERPOLATOR_MIXER.connectable_elements = wcd_RX_4_INTERPOLATOR_MIXER_cea; 
	 wcd_RX_4_INTERPOLATOR_MIXER.connectable_elements_count = wcd_RX_4_INTERPOLATOR_MIXER_cea_count; 
	 wcd_RX_4_INTERPOLATOR_MIXER.events_map = wcd_RX_4_INTERPOLATOR_MIXER_em; 
	 wcd_RX_4_INTERPOLATOR_MIXER.events_count = wcd_RX_4_INTERPOLATOR_MIXER_em_count; 

	 wcd_RX_3_COMPANDER.connectable_elements = wcd_RX_3_COMPANDER_cea; 
	 wcd_RX_3_COMPANDER.connectable_elements_count = wcd_RX_3_COMPANDER_cea_count; 
	 wcd_RX_3_COMPANDER.events_map = wcd_RX_3_COMPANDER_em; 
	 wcd_RX_3_COMPANDER.events_count = wcd_RX_3_COMPANDER_em_count; 

	 wcd_RX_BIAS_3.connectable_elements = wcd_RX_BIAS_3_cea; 
	 wcd_RX_BIAS_3.connectable_elements_count = wcd_RX_BIAS_3_cea_count; 
	 wcd_RX_BIAS_3.events_map = wcd_RX_BIAS_3_em; 
	 wcd_RX_BIAS_3.events_count = wcd_RX_BIAS_3_em_count; 

	 wcd_RDAC_4_MUX.connectable_elements = wcd_RDAC_4_MUX_cea; 
	 wcd_RDAC_4_MUX.connectable_elements_count = wcd_RDAC_4_MUX_cea_count; 
	 wcd_RDAC_4_MUX.events_map = wcd_RDAC_4_MUX_em; 
	 wcd_RDAC_4_MUX.events_count = wcd_RDAC_4_MUX_em_count; 

	 wcd_LO1_L_P.connectable_elements = wcd_LO1_L_P_cea; 
	 wcd_LO1_L_P.connectable_elements_count = wcd_LO1_L_P_cea_count; 
	 wcd_LO1_L_P.events_map = wcd_LO1_L_P_em; 
	 wcd_LO1_L_P.events_count = wcd_LO1_L_P_em_count; 

	 wcd_RX_4_COMPANDER.connectable_elements = wcd_RX_4_COMPANDER_cea; 
	 wcd_RX_4_COMPANDER.connectable_elements_count = wcd_RX_4_COMPANDER_cea_count; 
	 wcd_RX_4_COMPANDER.events_map = wcd_RX_4_COMPANDER_em; 
	 wcd_RX_4_COMPANDER.events_count = wcd_RX_4_COMPANDER_em_count; 

	 wcd_CLASS_CONTROLLER_4.connectable_elements = wcd_CLASS_CONTROLLER_4_cea; 
	 wcd_CLASS_CONTROLLER_4.connectable_elements_count = wcd_CLASS_CONTROLLER_4_cea_count; 
	 wcd_CLASS_CONTROLLER_4.events_map = wcd_CLASS_CONTROLLER_4_em; 
	 wcd_CLASS_CONTROLLER_4.events_count = wcd_CLASS_CONTROLLER_4_em_count; 

	 wcd_RX_BIAS_4.connectable_elements = wcd_RX_BIAS_4_cea; 
	 wcd_RX_BIAS_4.connectable_elements_count = wcd_RX_BIAS_4_cea_count; 
	 wcd_RX_BIAS_4.events_map = wcd_RX_BIAS_4_em; 
	 wcd_RX_BIAS_4.events_count = wcd_RX_BIAS_4_em_count; 

	 wcd_SPEAKER.connectable_elements = wcd_SPEAKER_cea; 
	 wcd_SPEAKER.connectable_elements_count = wcd_SPEAKER_cea_count; 
	 wcd_SPEAKER.events_map = wcd_SPEAKER_em; 
	 wcd_SPEAKER.events_count = wcd_SPEAKER_em_count; 

	 wcd_RDAC_5_DIFF_MUX.connectable_elements = wcd_RDAC_5_DIFF_MUX_cea; 
	 wcd_RDAC_5_DIFF_MUX.connectable_elements_count = wcd_RDAC_5_DIFF_MUX_cea_count; 
	 wcd_RDAC_5_DIFF_MUX.events_map = wcd_RDAC_5_DIFF_MUX_em; 
	 wcd_RDAC_5_DIFF_MUX.events_count = wcd_RDAC_5_DIFF_MUX_em_count; 

	 wcd_LO2_R_M.connectable_elements = wcd_LO2_R_M_cea; 
	 wcd_LO2_R_M.connectable_elements_count = wcd_LO2_R_M_cea_count; 
	 wcd_LO2_R_M.events_map = wcd_LO2_R_M_em; 
	 wcd_LO2_R_M.events_count = wcd_LO2_R_M_em_count; 

	 wcd_USONIC_MUX.connectable_elements = wcd_USONIC_MUX_cea; 
	 wcd_USONIC_MUX.connectable_elements_count = wcd_USONIC_MUX_cea_count; 
	 wcd_USONIC_MUX.events_map = wcd_USONIC_MUX_em; 
	 wcd_USONIC_MUX.events_count = wcd_USONIC_MUX_em_count; 

	 wcd_ADC_3.connectable_elements = wcd_ADC_3_cea; 
	 wcd_ADC_3.connectable_elements_count = wcd_ADC_3_cea_count; 
	 wcd_ADC_3.events_map = wcd_ADC_3_em; 
	 wcd_ADC_3.events_count = wcd_ADC_3_em_count; 

	 wcd_ADC_4.connectable_elements = wcd_ADC_4_cea; 
	 wcd_ADC_4.connectable_elements_count = wcd_ADC_4_cea_count; 
	 wcd_ADC_4.events_map = wcd_ADC_4_em; 
	 wcd_ADC_4.events_count = wcd_ADC_4_em_count; 

	 wcd_MBIAS_MGR_9.connectable_elements = wcd_MBIAS_MGR_9_cea; 
	 wcd_MBIAS_MGR_9.connectable_elements_count = wcd_MBIAS_MGR_9_cea_count; 
	 wcd_MBIAS_MGR_9.events_map = wcd_MBIAS_MGR_9_em; 
	 wcd_MBIAS_MGR_9.events_count = wcd_MBIAS_MGR_9_em_count; 

	 wcd_ADC_5.connectable_elements = wcd_ADC_5_cea; 
	 wcd_ADC_5.connectable_elements_count = wcd_ADC_5_cea_count; 
	 wcd_ADC_5.events_map = wcd_ADC_5_em; 
	 wcd_ADC_5.events_count = wcd_ADC_5_em_count;
	 
	 wcd_ANC_CHANNEL_1.connectable_elements = wcd_ANC_CHANNEL_1_cea; 
	 wcd_ANC_CHANNEL_1.connectable_elements_count = wcd_ANC_CHANNEL_1_cea_count; 
	 wcd_ANC_CHANNEL_1.events_map = wcd_ANC_CHANNEL_1_em; 
	 wcd_ANC_CHANNEL_1.events_count = wcd_ANC_CHANNEL_1_em_count; 

	 wcd_ANC_CHANNEL_2.connectable_elements = wcd_ANC_CHANNEL_2_cea; 
	 wcd_ANC_CHANNEL_2.connectable_elements_count = wcd_ANC_CHANNEL_2_cea_count; 
	 wcd_ANC_CHANNEL_2.events_map = wcd_ANC_CHANNEL_2_em; 
	 wcd_ANC_CHANNEL_2.events_count = wcd_ANC_CHANNEL_2_em_count;

	 return return_code;
}

