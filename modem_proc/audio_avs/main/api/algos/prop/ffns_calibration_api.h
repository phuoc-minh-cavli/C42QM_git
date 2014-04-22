/*! 
*  @brief     Calibration file of unified application programming interface (API) 2.0. 
*  @details   This file contains definitions/declarations for this 
*              framework's unified API 2.0. This file informs the firmware team 
*             about the enumerations and the structures of the exposed (advanced) 
*             parameters that can be accessed/set by the customer in order to tune them. 
*  @file      ffns_calibration_api.h 
*  @author    Qualcomm 
*  @author    hpessent  
*  @version   1.0
*  @date      2018/01/09
*  @pre       This framework is based on `ffns_api.h`; the modules history is available at 
*             `//depot/AUDIO_SYSTEM/MEC/main/latest/MecLib/inc/mec_api.h#11`. 
*/
#ifndef __FFNS_CALIBRATION_API_H__
#define __FFNS_CALIBRATION_API_H__



/**********************************************
* Libraries
***********************************************/
#include "ffv_common_calibration.h"
#include "voice_param_mod_enable_calib.h"
#include "audio_common_param_calib.h"
#include "audio_rtm_logging_param_calib.h"
#include "avs_imc_calib.h"

/**********************************************
* Constants, Types, Variables, and Structures
***********************************************/
// Define certain variables, constants, types, and structures 
// in order to avoied loading any (external) libraries. 
#define API_MAX_NUM_TX_CHAN         (FFV_MAX_TX_CHAN)          // maximum input TX channels) // Maximum number of microphone/beamformer channels. 
#define API_MAX_NUM_BEAMFORMER_CHAN (FFV_MAX_NUM_BEAMFORMER_CHAN) // Number of fixed beamformers or beamformer output channels. 
#define API_BF_COEFF_SIZE_MAX       (API_MAX_NUM_BEAMFORMER_CHAN * API_MAX_NUM_TX_CHAN * FFV_NFREQS_WB) // Maximum of beamformer coefficients. (Note: This will be removed in the future; right now, I use it to verify the correct loading of the coefficients.)
#define API_MAX_NUM_OUT_CHAN        (FFV_MAX_OUT_CHAN) // Maximum number of output channels. 
#define API_MAX_NUM_CHAN            (11) // Maximum number of the aforementioned numbers of channels. 

// This enumeration refers to structures that can be used to communicate with 
// the library's functions. Note: `enum` numbers doesn't have to be sequential. 
// Moreover, each structure should have a specific ID in `ffns_struct_id_t`. 
typedef enum FFNS_STRUCT_IDS
{
   FFNS_GLOBAL_PARAM = 0x00010963,         // Refers to `ffns_global_param_t`.
   FFNS_BF_PARAM = FFV_BF_PARAM,             // refers to ffv_bf_param_t in ffv_common_calibration.h
   FFNS_OUT_CONFIG_PARAM = FFV_OUT_CONFIG_PARAM,            // refers to ffv_out_config_param_t in ffv_common_calibration.h
   FFNS_BF_COEF = FFV_BF_COEF,             // refers to ffv_bf_coef_t in ffv_common_calibration.h
   FFNS_HPF_PARAM = FFV_HPF_PARAM,         // refers to ffv_hpf_param_t in ffv_common_calibration.h
   FFNS_DOA_PARAM = FFV_DOA_PARAM,         // refers to ffv_doa_param_t in ffv_common_calibration.h
   FFNS_SPF_FREEZE = FFV_SPF_FREEZE,        // Refers to freezing the beamformer output channels; this parameter is not exposed.
   FFNS_SPF_MONITOR = FFV_SPF_MONITOR,        // Refers to ffv_spf_monitor_param_t in ffv_common_calibration.h
   FFNS_VERSION_PARAM = FFV_VERSION_PARAM,    // Refers to `ffns_lib_version_param_t`.
   FFNS_MEMORY_CONFIG = 0,                    // Refers to `ffns_memory_config_t`.
   FFNS_INPUT_DATA_CONFIG,         // Refers to `ffns_input_data_config_t`.
   FFNS_OUTPUT_DATA_CONFIG,        // Refers to `ffns_output_data_config_t`.
   FFNS_GLOBAL_DEFAULT_PARAM,         // Refers to `ffns_global_param_t`.
   FFNS_BF_DEFAULT_PARAM,            // Refers to `ffns_bf_param_t`.
   FFNS_BF_DEFAULT_COEF,            // Refers to `ffns_bf_coef_t`.
   FFNS_HPF_DEFAULT_PARAM,            // Refers to `ffns_hpf_param_t`.
   FFNS_DOA_DEFAULT_PARAM,            // Refers to `ffns_doa_param_t`.
   FFNS_IO_PARAM,                    // Refers to `ffns_io_param_t`.
   FFNS_IO_DEFAULT_PARAM,            // Refers to `ffns_io_param_t`.
   FFNS_SCRATCH_MEM_REQ_PARAM,        // Refers to `ffns_scratch_mem_param_t`. (Only `get`: provides the required size based on the latest calibration.)
   FFNS_SCRATCH_MEM_PARAM,         // Refers to `ffns_scratch_mem_req_param_t`. (`set` and `get`: set will initialize the scratch memory, get will give you the pointer and the size of the current data memory.)
   FFNS_DATA_MEM_REQ_PARAM,         // Refers to `ffns_data_mem_req_param_t`. (Only `get`: provides the required size based on the latest calibration.)
   FFNS_DATA_MEM_PARAM,             // Refers to `ffns_data_mem_req_param_t`. (`set` and `get`: set will initialize the memory, get will give you the pointer and the size of the current data memory.)
   FFNS_STRUCT_IDS_END             // End flag.
} FFNS_STRUCT_IDS;

// Enumeration encapsulating the different feature codes. 
typedef enum FFNS_FEATURE
{
    FFNS_FEATURE_HP                     = (1<<2),   // Enable high-pass filter. 
    FFNS_FEATURE_BF                     = (1<<4),   // Enable spatial filter. 
    FFNS_FEATURE_LLC                    = (1<<12),  // Low-latency-complexity filterbank: N = 64, R = 16 for MEC, N = 32, R = 8 for ICC. 
    FFNS_FEATURE_ENABLE_LOW_POWER_MODE  = (1<<13),  // Enable low-power mode to bypass filterbank processing (when RX equals `null`). 
} FFNS_FEATURE;

// Structure encapsulating the global parameters. 

/** @h2xmlp_parameter    {"PARAM_ID_FFNS_GLOBAL", FFNS_GLOBAL_PARAM}
    @h2xmlp_description  { Structure for encapsulating the global parameters\n} */

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffns_global_param_t
{
    uint32_t mode; // Mode word required to enable/disable certain (exposed) features. 
    /**< @h2xmle_description        {Mode word required to enable/disable certain (exposed) features} 
         @h2xmle_default            {0x0} 
         @h2xmle_range                {0x00000000..0xFFFFFFFF} 
         @h2xmle_policy                {Basic} 

        @h2xmle_bitfield            {0x00000003}
            @h2xmle_bitName            {Bit_1_0}
            @h2xmle_description        {Reserved Bit[1:0]}
            @h2xmle_visibility        {hide}
        @h2xmle_bitfieldEnd

        @h2xmle_bitfield            {0x00000004}
            @h2xmle_bitName            {Bit_2}
            @h2xmle_description        {Enable high-pass filter.}
        @h2xmle_bitfieldEnd

        @h2xmle_bitfield            {0x00000008}
            @h2xmle_bitName            {Bit_3}
            @h2xmle_description        {Reserved Bit[3]}
        @h2xmle_bitfieldEnd

        @h2xmle_bitfield            {0x00000010}
            @h2xmle_bitName            {Bit_4}
            @h2xmle_description        {Enable Spatial Filtering}
        @h2xmle_bitfieldEnd

        @h2xmle_bitfield            {0x00000FE0}
            @h2xmle_bitName            {Bit_11_5}
            @h2xmle_description        {Reserved Bit[11:5]}
            @h2xmle_visibility        {hide}
        @h2xmle_bitfieldEnd

        @h2xmle_bitfield            {0x00001000}
            @h2xmle_bitName            {Bit_12}
            @h2xmle_description        {low-latency-complexity filterbank N = 64, R = 16 for MEC, N = 32, R = 8 for ICC}
        @h2xmle_bitfieldEnd

        @h2xmle_bitfield            {0x00002000}
            @h2xmle_bitName            {Bit_13}
            @h2xmle_description        {Enable low-power mode to bypass filterbank processing when RX is NULL}
        @h2xmle_bitfieldEnd

        @h2xmle_bitfield            {0xFFFFC000}
            @h2xmle_bitName            {Bit_31_14}
            @h2xmle_description        {Reserved Bit[31:14]}
            @h2xmle_visibility        {hide} 
        @h2xmle_bitfieldEnd  */


    int16_t primInChanL16; // Index of primary input channel. 
    /**< @h2xmle_description {Index of primary input channel} 
         @h2xmle_default     {0x0} 
         @h2xmle_range       {0..7} 
         @h2xmle_policy      {Advanced} */

    int16_t micChMapL16[API_MAX_NUM_TX_CHAN]; // Microphone/input channel mapping.
    /**< @h2xmle_description {Channel mapping of microphone 1 to microphone 8} 
         @h2xmle_defaultList {0,1,2,3,4,5,6,7} 
         @h2xmle_range       {0..7} 
         @h2xmle_policy      {Advanced} */

    int16_t input_gain_L16q13[API_MAX_NUM_TX_CHAN]; // Gains of the input channels; a gain of 8913 is equivalent to a gain of approx. 12 dB. 
    /**< @h2xmle_description {Gains of input channel 1 to 8; gain of 8912 (Int16) -> gain of 1 (0 dB) (Float) (20*log10(8192/(2^13)) = 0dB)} 
         @h2xmle_default     {0} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} */

    int16_t out_gain_L16Q11; // Gain of the output channels; a gain of 8913 is equivalent to a gain of approx. 12 dB. 
    /**< @h2xmle_description {Gains of the output channels; gain of 8912 (Int16) -> gain of 1 (0 dB) (Float) (20*log10(8192/(2^13)) = 0dB)} 
         @h2xmle_default     {0} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} */
} ffns_global_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif
/**
    @h2xmlm_module       {"MODULE_ID_FFNS", FFNS_MODULE}
    @h2xmlm_displayName  {"FFNS_MODULE"}
    @h2xmlm_description {FFNS 
                         This module supports the following parameter IDs 
                        -    VOICE_PARAM_MOD_ENABLE 
                        -    AUDPROC_PARAM_ID_LIB_VERSION 
                        -    PARAM_ID_FFNS_GLOBAL
                        -    PARAM_ID_FFV_HPF 
                        -    PARAM_ID_FFV_BF
                        -    PARAM_ID_FFV_OUT_CONFIG
                        -    PARAM_ID_FFV_BF_COEF 
                        -    PARAM_ID_FFV_DOA 
                        -    PARAM_ID_FFV_BF PARAM_ID_FFV_DOA
                        -    PARAM_ID_FFV_SPF_MONITOR 
                        -    PARAM_ID_FFV_SPF_FREEZE
                         }
    @h2xmlm_ToolPolicy                 {Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select                    {voice_param_mod_enable_t}
    @h2xmlm_InsertParameter
    @h2xml_Select       {voice_param_mod_enable_t::Enable}
    @h2xmle_default     {1}

    @h2xml_Select                {audproc_lib_version_t}
    @h2xmlm_InsertParameter

    @h2xml_Select                {ffns_global_param_t}
    @h2xmlm_InsertParameter

    @h2xml_Select                {ffv_hpf_param_t}
    @h2xmlm_InsertParameter

    @h2xml_Select                {ffv_bf_param_t}
    @h2xmlm_InsertParameter

    @h2xml_Select                {ffv_out_config_param_t}
    @h2xmlm_InsertParameter

    @h2xml_Select                {ffv_bf_coef_t}
    @h2xmlm_InsertParameter

    @h2xml_Select                {ffv_doa_param_t}
    @h2xmlm_InsertParameter

    @h2xml_Select                {ffv_spf_monitor_param_t}
    @h2xmlm_InsertParameter

    @h2xml_select                {ffv_spf_freeze_param_t}
    @h2xmlm_InsertParameter                            

    @h2xml_Select                {audproc_rtm_logging_enable_payload_t}
    @h2xmlm_InsertParameter

    @h2xml_Select                {audproc_rtm_pcm_logging_payload_t}
    @h2xmlm_InsertParameter

    @h2xml_Select                {avs_imc_payload_t}
    @h2xmlm_InsertParameter
    @h2xml_Select       {avs_imc_payload_t::purpose}
    @h2xmle_default     {AVS_PURPOSE_ID_SVA_FFV_FREEZE_BEAMFORMER}


   @}                   <-- End of the Module -->*/

#endif 
