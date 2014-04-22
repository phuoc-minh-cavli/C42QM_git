#ifndef WCD_TARGET_DEFS_INTF_
#define WCD_TARGET_DEFS_INTF_

/**
@file target_defs.h
@brief 
External target functions declaration for WCD.
*/
/*-------------------------------------------------------------------------------
     Copyright (c) 2011 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                             Edit History
$Id: //components/rel/audio.tx/2.0/audio/adie/driver/src/target_defs.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/25/13   KNM     Added a generic function to determine connection direction.
07/23/13   KNM     Added V5V6 specific changes for dynamic buck voltage switching.
05/31/13   KNM     Multichannel sidetone usecase support and signature update
                   for wcd_reset_codec.
03/07/13   SSP     Driver Unification Update
03/01/13   KNM     Initial revision.
===============================================================================*/

#include "wcd_intf.h"

/** 
 * \brief Validates the source and maps the source to the appropriate element.
 */
wcd_result wcd_map_source_to_element(wcd_usecase* usecase, wcd_source* source, wcd_element** p_element_handle);

/** 
 * \brief Validates the sink and maps the sink to the appropriate element.
 */
wcd_result wcd_map_sink_to_element(wcd_source source, wcd_sink* sink, 
                                   wcd_connection_type connection_type, 
                                   wcd_element** p_element_handle);

/** 
 * \brief Validates the sink and maps the sink to the differential element.
 */
wcd_result wcd_map_sink_to_differential_element(wcd_sink sink, 
                                   wcd_connection_type connection_type, 
                                   wcd_element** p_element_handle);

/**
 * \brief This bit will tell us if there needs to be something special done when routing
 * that element type. This is mainly for Earout Ultrasound mode and ADC/DMIC micbias addition
 */
#define WCD_ROUTE_STACK_SPECIAL_TYPE  0x80000000

/**
 * \brief This indicates end of a route stack
 */
#define WCD_ROUTE_STACK_END           0

/**
 * \brief Determines and populates the route stack from the source to the sink
 */ 
wcd_result wcd_determine_route_stack(wcd_source source, wcd_sink sink, uint32** p_route_stack);

/**
 * \brief Determines and populates the route stack from the source to the sink for the differential connection
 */ 
wcd_result wcd_determine_differential_route_stack(wcd_sink sink, uint32** p_route_stack);

/**
 * \brief Determines the TX Path to use based on the source (adc/dmic) and sink (sbtx port)
 */ 
wcd_result wcd_determine_txpath(wcd_element* source, wcd_element *sink, wcd_element** txpath);

/**
 * \brief Marks a TX Path as freed and allows other connections
 */ 
wcd_result wcd_free_txpath(wcd_element* tx_path);

/**
 * \brief Determines the slimbus port number based on the element passed in
 * Also determines if it is either a RX or TX port
 */
wcd_result wcd_determine_slimbusport_number(wcd_element* bus_port_element, 
                                            uint32* bus_port_id,
                                            wcd_slimbus_direction* direction);

/**
 * \brief Marks a TX/RX slimbus port as freed and allows other connections
 */
wcd_result wcd_free_slimbusport(wcd_element* bus_port_element);

/**
 * \brief Handles any special routing not posible by the common code. Mainly for 
 * ultrasound earout routing.
 */
wcd_result wcd_handle_special_routing(wcd_route* route, uint32 special_route_stack_type);

/**
 * \brief Marks that a reset will be required before the codec is initialized.
 */
wcd_result wcd_reset_required(void);

/**
 * \brief Initializes the codec before enabling any usecases.
 */
wcd_result wcd_initialize_codec(void);

/**
 * \brief Deinitializes the codec once all the usecases have been disabled
 */
wcd_result wcd_deinitialize_codec(void);

/**
 * \brief Any target specific register writes that need to be done before enabling a particular (or any) usecase
 */
wcd_result wcd_pre_enable(wcd_usecase *p_usecase);

/**
 * \brief Any target specific register writes that need to be done after disabling a particular (or any) usecase
 */
wcd_result wcd_post_disable(wcd_usecase *p_usecase);

/**
 * \brief Soft resets the codec.
 */
wcd_result wcd_reset_codec(void* reset_payload);

/**
 * \brief Returns the codec info.
 */
wcd_result wcd_get_codec_info(wcd_codec_info* p_codec_info);

/**
 * \brief Returns the maximum number of RX ports based on bus type.
 */
wcd_result wcd_get_max_rx_ports(uint32* port_count);

/**
 * \brief Returns the maximum number of TX ports based on bus type.
 */
wcd_result wcd_get_max_tx_ports(uint32* port_count);

/**
 * \brief Initialize the decimator mapping for ADCS, DMICs & Slimbus Tx port based on codec.
 */
wcd_result wcd_initialize_codec_decimator_mapping(wcd_codec_info codec_info);

/**
 * \brief Initialize route stack based on codec.
 */
wcd_result wcd_initialize_route_stack(wcd_codec_info codec_info);

/**
 * \brief Initialize max value for ADCs, DMICs, TX & RX path count based on codec.
 */
wcd_result wcd_initialize_codec_max_count(wcd_codec_info codec_info);

/**
 * \brief Returns the Sidetone RX usecase associated with the usecase if it is part of a sidetone
 * usecase. Returns NULL if its not part of a sidetone usecase
 */
wcd_usecase* wcd_check_for_sidetone_usecase(wcd_usecase *usecase);

/**
 * \brief Sets the first or second IIR in codec to be used as the sidetone path for this particular usecase.
 */
wcd_result wcd_set_sidetone_iir(wcd_usecase *rx_usecase, wcd_usecase* tx_usecase);

/**
 * \brief Enables sidetone and connects the sidetone mux to the decimator passed in the parameter
 */
wcd_result wcd_enable_sidetone(wcd_usecase*, wcd_element*);

/**
 * \brief Disables sidetone if possible. Also clears the sidetone from the IIR being used.
 */
wcd_result wcd_disable_sidetone(wcd_usecase*);

/**
 * \brief Returns the saved sidetone IIR configuration for hte particular usecase.
 */
wcd_result wcd_get_sidetone_iir_configuration(wcd_usecase *usecase, wcd_adie_codec_db_sidetone_iir_config* config);

/**
 * \brief Configures the IIR filter for sidetone on the codec
 */
wcd_result wcd_configure_sidetone_iir(wcd_usecase* usecase, wcd_adie_codec_db_sidetone_iir_config* config);

/**
 * \brief Connects sidetone to the RX Path.
 */
wcd_result wcd_connect_sidetone(wcd_element*);

/**
 * \brief Disconnects sidetone to the RX Path.
 */
wcd_result wcd_disconnect_sidetone(wcd_element*);

/**
 * \brief Sets the port number of the voltage in feedback speaker protection
 */
wcd_result wcd_get_spkr_prot_voltage_port(uint32* port_num);

/**
 * \brief Sets the port number of the current in feedback speaker protection
 */
wcd_result wcd_get_spkr_prot_current_port(uint32* port_num);

/**
 * \brief Enables speaker protection on the codec, if supported
 */
wcd_result wcd_enable_speaker_protection(void);

/**
 * \brief Disables speaker protection on the codec, if supported
 */
wcd_result wcd_disable_speaker_protection(void);

/**
 * \brief configures common parameters for the different MAD modes.
 */
wcd_result wcd_configure_mad_common( wcd_configure_mad* p_command );

/**
 * \brief Disables common parameters for the different MAD modes.
 */
wcd_result wcd_disable_mad_common(void);

/**
 * \brief function to determine if the codec is being used for audio
 */
wcd_bool wcd_audio_configured( void );

/**
 * \brief function to determine codec tier
 */
wcd_result wcd_get_codec_tier_info(wcd_codec_info* p_codec_info);

/**
 * \brief function required to identify ADC for Helicon
 */
wcd_result wcd_find_ADC_Path (wcd_element** pCurrElement, wcd_element* pSrcElement, uint32 elem_type);

/**
 * \brief Save vdd buck voltage locally, passed by client during wcd_init
 */
wcd_result wcd_store_codec_buck_voltage(wcd_vdd_buck_voltage buck_voltage);

/**
 * \brief return vdd buck voltage which is set
 */
wcd_vdd_buck_voltage wcd_get_codec_buck_voltage(void);

/**
 * \brief Get connection direction from element type
 */
wcd_result wcd_get_connection_direction(wcd_element* source_element, wcd_element* sink_element, adie_codec_direction *connection_direction);

#ifdef WCD_MBHC_ENABLE
/**
 * \brief Function to call the client callback to toggle the 
 *        external codec reset GPIO
 */
wcd_result wcd_reset_codec_external_pin(void);

/**
 * \brief Function to write value to an O/P GPIO
 */
wcd_result WCDSwitchGpio(void *param, wcd_module_gpio_id id, int *value);
#endif

#endif
