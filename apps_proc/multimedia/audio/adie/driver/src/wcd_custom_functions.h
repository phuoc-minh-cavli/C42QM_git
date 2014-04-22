#ifndef WCD_CUSTOM_FUNCTIONS_INTF_
#define WCD_CUSTOM_FUNCTIONS_INTF_

/**
@file wcd_custom_functions.h
@brief 
Custom function declaration for WCD codec driver.
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/wcd_custom_functions.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/18/13   SP		Support HPH PA enable/disable command concurrecy between 
					MBHC and Core WCD modules.
05/31/13   KNM		Added micbias op-amp control from mbhc.
03/01/13   KNM		Initial revision.
===============================================================================*/

#include "wcd_result.h"
#include "wcd_types.h"

#define ENABLE_HPHL 0x1
#define ENABLE_HPHR 0x2

wcd_result WCDH_EAROUT_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_EAROUT_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_RX_SET_SAMPLE_RATE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_RX_PATH_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_RX_PATH_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_RX_PATH_RESET(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_RX_PATH_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_COMPANDER_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_COMPANDER_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_CLASS_CONTROLLER_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_CLASS_CONTROLLER_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_RX_BIAS_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_RX_BIAS_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_HPH_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_HPH_PRECHARGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_HPH_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_HPH_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_MBIAS_MGR_SELECT_MICBIAS(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_MBIAS_MGR_SELECT_CFILTER(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_MBIAS_MGR_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_MBIAS_MGR_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_MBIAS_MGR_APPLY_DESIRED_VOLTAGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_MBIAS_MGR_APPLY_EXTERNAL_BYPASS_CAPACITOR(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_MBIAS_MGR_SET_IB_RES(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_MBIAS_MGR_ENABLE_IB(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_MBIAS_MGR_DISABLE_IB(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_HS_MBIAS_OPA_ENABLE(uint16 reg, uint8 mask, uint8 shift, uint8 val, wcd_bool enable, wcd_owner_module owner);
wcd_result WCDH_HS_HPH_ENABLE(uint16 reg, uint8 mask, uint8 shift, uint8 val, uint8 hph_pa_en, wcd_bool enable, wcd_owner_module owner);
wcd_result WCDH_DMIC_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_DMIC_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_DMIC_SET_CLOCK_MODE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_ADC_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_ADC_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_TX_SET_SAMPLE_RATE(wcd_element* p_element, uint32 sample_rate, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_TX_PATH_RESET(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_TX_PATH_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_LO_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_LO_PRECHARGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_LO_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_LO_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_SPEAKER_SET_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_SPEAKER_PRECHARGE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_SPEAKER_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_SPEAKER_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr);
wcd_result WCDH_ADC_SET_ANALOG_GAIN(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_TX_PATH_ENABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 
wcd_result WCDH_TX_PATH_DISABLE(wcd_element* p_element, uint32 parameter, wcd_list* p_element_list, wcd_event_context_pointer* context_ptr); 

wcd_result WCDH_RESET(void);
wcd_result WCDH_SET_MICBIAS_COUNT(wcd_codec codec);
wcd_result WCDH_INT_RBIAS_ENABLE(wcd_ctrl_bus_interface bus_intf, uint16 reg, uint8 mask, uint8 shift, uint8 val, uint8 in_path, wcd_bool enable);

#endif
