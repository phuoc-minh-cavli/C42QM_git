/* Dynamic Configuration/Calibration API Structures */

#include "ffv_common_calibration.h"

#include "voice_param_mod_enable_calib.h"
#include "audio_common_param_calib.h"
#include "audio_rtm_logging_param_calib.h"
#include "avs_imc_calib.h"

#ifndef __FFECNS_CALIBRATION_API_H__
#define __FFECNS_CALIBRATION_API_H__

#define FFECNS_MAX_TX_CHAN  (FFV_MAX_TX_CHAN)                           // maximum input TX channels
#define FFECNS_MAX_RX_CHAN  (FFV_MAX_RX_CHAN)                           // maximum input RX channels
#define FFECNS_MAX_OUT_CHAN (FFV_MAX_OUT_CHAN)                          // maximum output channels      
#define FFECNS_MIN_TX_CHAN  (FFV_MIN_TX_CHAN)                                         // minimum input TX channels
#define FFECNS_MIN_RX_CHAN  (FFV_MIN_RX_CHAN)                                         // minimum input RX channels
#define FFECNS_MIN_OUT_CHAN (FFV_MIN_OUT_CHAN)                                         // minimum output channels

#define FFECNS_ROOMRESPONSE_SFLEN_MAX (40)
#define FFECNS_ROOMRESPONSE_SFLEN_MAX_LLC (80)
#define FFECNS_ROOMRESPONSE_PER_MIC_SIZE_MAX (FFV_NFREQS_WB*FFECNS_ROOMRESPONSE_SFLEN_MAX*FFECNS_MAX_RX_CHAN)

/*
FFECNS_STRUCT_ID

FFECNS_STRUCT_ID enum typedef refers to structures that can be used to communicate with library functions.
Along with refering to structures in ffecns_api.h. This enum also refers to
the calibration structures in ffecns_calibration_api.h

Note: enum numbers doesn't have to be sequential.

   */
typedef enum FFECNS_STRUCT_IDS
{
    FFECNS_GLOBAL_PARAM = 0x00010953,                // refers to ffecns_global_param_t in ffecns_calibration_api.h
    FFECNS_AEC_PARAM = 0x00010954,                   // refers to ffecns_aec_param_t in ffecns_calibration_api.h
    FFECNS_ECPP_CLIP_PARAM = 0x00010955,             // refers to ffecns_ecpp_clip_param_t in ffecns_calibration_api.h
    FFECNS_DELAY_PARAM = 0x00010956,                 // refers to ffecns_delay_param_t in ffecns_calibration_api.h

    FFECNS_BF_PARAM = FFV_BF_PARAM,                 // refers to ffv_bf_param_t in ffv_common_calibration.h    
    FFECNS_OUT_CONFIG_PARAM = FFV_OUT_CONFIG_PARAM,            // refers to ffv_out_config_param_t in ffv_common_calibration.h
    FFECNS_BF_COEF = FFV_BF_COEF,                     // refers to ffv_bf_coef_t in ffv_common_calibration.h
	FFECNS_IVA_PARAM = FFV_IVA_PARAM, 				// refers to ffv_iva_param_t in ffv_common_calibration.h
	FFECNS_SCS_PARAM = FFV_SCS_PARAM, 				// refers to ffv_scs_param_t in ffv_common_calibration.h
	FFECNS_IVA_MONITOR_PARAM = FFV_IVA_MONITOR, 				// refers to ffv_iva_monitor_param_t in ffv_common_calibration.h
    FFECNS_HPF_PARAM = FFV_HPF_PARAM,                 // refers to ffv_hpf_param_t in ffv_common_calibration.h
    FFECNS_DOA_PARAM = FFV_DOA_PARAM,                 // refers to ffv_doa_param_t in ffv_common_calibration.h
    FFECNS_VERSION_PARAM = FFV_VERSION_PARAM,        // refers to ffecns_lib_version_param_t in ffecns_calibration_api.h   
    FFECNS_SPF_FREEZE_PARAM = FFV_SPF_FREEZE,        // Refers to ffv_spf_freeze_param_t in ffv_common_calibration.h
    FFECNS_HW_AF_PARAM = 0x0001095B,                 // refers to ffecns_hw_af_param_t in ffecns_calibration_api.h
    FFECNS_SURROUND_EC_CAL_PARAM = 0x0001095C,        // refers to ffecns_surround_ec_calibration_param_t in ffecns_calibration_api.h
    
    FFECNS_EC_CALIBRATION_MODE_PARAM =  0x00010961,  // refers to ffecns_ec_calibration_mode_param_t in ffecns_calibration_api.h

	// for monitoring echo cancellation  status
	 FFECNS_EC_MONITOR_PARAM = 0x0001095D,			// only get
    FFECNS_SPF_MONITOR_PARAM = FFV_SPF_MONITOR,		// Refers to ffv_spf_monitor_param_t in ffv_common_calibration.h

    FFECNS_EC_AF_COEFFS_MIC0_PARAM = 0x00010966,       // only get,
    FFECNS_EC_AF_COEFFS_MIC1_PARAM = 0x00010967,         // only get,
    FFECNS_EC_AF_COEFFS_MIC2_PARAM = 0x00010968,         // only get,
    FFECNS_EC_AF_COEFFS_MIC3_PARAM = 0x00010969,         // only get,
    FFECNS_EC_AF_COEFFS_MIC4_PARAM = 0x0001096A,         // only get,
    FFECNS_EC_AF_COEFFS_MIC5_PARAM = 0x0001096B,         // only get,
    FFECNS_EC_AF_COEFFS_MIC6_PARAM = 0x0001096C,         // only get,
    FFECNS_EC_AF_COEFFS_MIC7_PARAM = 0x0001096D,         // only get,

    FFECNS_IO_PARAM = 0,                            // refers to ffecns_io_param_t

    FFECNS_MEMORY_CONFIG,            // refers to ffecns_memory_config_t
    FFECNS_INPUT_DATA_CONFIG,         // refers to ffecns_input_data_config_t
    FFECNS_OUTPUT_DATA_CONFIG,        // refers to ffecns_output_data_config_t
    FFECNS_SCRATCH_MEM_REQ_PARAM,     // refers to ffecns_scratch_mem_param_t in ffecns_calibration_api.h
                                    // only get, gives you the size needed based on the latest calibration
    FFECNS_SCRATCH_MEM_PARAM,         // refers to ffecns_scratch_mem_req_param_t in ffecns_calibration_api.h
                                    // Set and get. Set will initialize the scratch memory, get will give you the pointer and the size of the current data memory
    FFECNS_DATA_MEM_REQ_PARAM,         // refers to ffecns_data_mem_req_param_t in ffecns_calibration_api.h
                                    // only get, gives you the size needed based on the latest calibration
    FFECNS_DATA_MEM_PARAM,             // refers to ffecns_data_mem_req_param_t in ffecns_calibration_api.h
                                    // Set and get. Set will initialize the memory, get will give you the pointer and the size of the current data memory

    /* for save and retrieve the unsorted beamformer top DOA channel index */
    //FFECNS_DOA_CHANNEL_INDEX_PARAM,       // Set and get
    /* for save and retrieve the unsorted beamformer top DOA channel index */
    FFECNS_EC_RESET_PARAM,       // Set 
    /* for monitoring beam forming  status*/
    FFECNS_BF_MONITOR_PARAM,        // only get
    FFECNS_EC_HW_AF_CB_PARAM,
    FFECNS_TH_POOL_CB_PARAM,
    FFECNS_STRUCT_IDS_END           // end flag
}FFECNS_STRUCT_IDS;

/*
   Enum encapsulating the different feature feature codes - linked to feature in the ffecns_global_param_t structure
   */
typedef enum FFECNS_FEATURE
{
    FFECNS_FEATURE_HP                     = (1<<2),   // Apply a high-pass filter to the signal 
    FFECNS_FEATURE_AEC                    = (1<<3),   // Perform linear acoustic echo cancellation 
    FFECNS_FEATURE_BF                     = (1<<4),   // Enable Spatial Filtering
    FFECNS_FEATURE_AEC_MAS                = (1<<5),   // Enable mic-array switching during EC
	FFECNS_FEATURE_16_BIT_FIXED_FILTERING = (1<<6),   // Enable 16 bit room response filtering
	FFECNS_FEATURE_IVA                    = (1<<7),   // Enable independent vector analysis. 
    FFECNS_FEATURE_AEC_PF                 = (1<<8),   // Perform AEC post-filtering 
    FFECNS_FEATURE_STEREORX_USELEFT_ONLY  = (1<<10),
	FFECNS_FEATURE_STEREORX_USERIGHT_ONLY = (1<<11),
    FFECNS_FEATURE_LLC                    = (1<<12),  // low-latency-complexity filterbank N = 64, R = 16 for MEC, N = 32, R = 8 for ICC 
    FFECNS_FEATURE_ENABLE_LOW_POWER_MODE  = (1<<13),  // enable low-power mode to bypass filterbank processing when RX is NULL 	
	
}FFECNS_FEATURE;

/* ffecns_global_param_t
	This structure defines the parameters associated with FFECNS_GLOBAL_PARAM in enum FFECNS_STRUCT_IDS, which is in ffecns_api.h
struct_size:			[IN/OUT]: structure size
							[IN]: caller fills size for the size of the payload structure in apis set(...) & get(...). expects size >= sizeof(ffecns_global_param_t)
						   [OUT]: callee fills the appropriate size of the buffer filled in api get(...),sizeof(ffecns_global_param_t)
   */

/** @h2xml_title1           {MODULE_ID_FFECNS}
    @h2xml_title_agile_rev  {MODULE_ID_FFECNS}
    @h2xml_title_date       {April 2, 2018} */

/** @h2xmlp_parameter    {"PARAM_ID_FFECNS_GLOBAL", FFECNS_GLOBAL_PARAM}
    @h2xmlp_description  { Structure for the configuration of the global parameters\n} */

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffecns_global_param_t
{
    int32_t features;             			   /* features bitmask for Mec algorithm */
    /**< @h2xmle_description {features bitmask for FFECNS algorithm} 
         @h2xmle_range       {0x80000000..0x7FFFFFFF} 
         @h2xmle_policy      {Basic}

		@h2xmle_bitfield			{0x00000003}
			@h2xmle_bitName			{Bit_1_0}
			@h2xmle_description		{Reserved Bit[1:0]}
			@h2xmle_visibility		{hide}
		@h2xmle_bitfieldEnd

		@h2xmle_bitfield			{0x00000004}
			@h2xmle_bitName			{Bit_2}
			@h2xmle_description		{Apply a high-pass filter to the signal}
		@h2xmle_bitfieldEnd

		@h2xmle_bitfield			{0x00000008}
			@h2xmle_bitName			{Bit_3}
			@h2xmle_default         {1} 
			@h2xmle_description		{Perform linear acoustic echo cancellation}
		@h2xmle_bitfieldEnd

		@h2xmle_bitfield			{0x00000010}
			@h2xmle_bitName			{Bit_4}
			@h2xmle_description		{Enable Spatial Filtering}
		@h2xmle_bitfieldEnd

		@h2xmle_bitfield			{0x00000020}
			@h2xmle_bitName			{Bit_5}
			@h2xmle_description		{Enable mic-array switching during EC}
		@h2xmle_bitfieldEnd

		@h2xmle_bitfield			{0x00000040}
		@h2xmle_bitName			{Bit_6}
		@h2xmle_default         {1}
		@h2xmle_description		{enable 16 bit room response filtering}
		@h2xmle_bitfieldEnd

		@h2xmle_bitfield			{0x00000080}
			@h2xmle_bitName			{Bit_7}
			@h2xmle_description		{Enable IVA based noise suppression, valid for 2 mic's only}
		@h2xmle_bitfieldEnd

		@h2xmle_bitfield			{0x00000100}
			@h2xmle_bitName			{Bit_8}
			@h2xmle_description		{Perform AEC post-filtering}
		@h2xmle_bitfieldEnd

		@h2xmle_bitfield			{0x00000200}
			@h2xmle_bitName			{Bit_9}
			@h2xmle_description		{Reserved Bit[9]}
			@h2xmle_visibility		{hide}
		@h2xmle_bitfieldEnd

		@h2xmle_bitfield			{0x00000400}
			@h2xmle_bitName			{Bit_10}
			@h2xmle_description		{Stereo RX Channel use left only}
		@h2xmle_bitfieldEnd

		@h2xmle_bitfield			{0x00000800}
			@h2xmle_bitName			{Bit_11}
			@h2xmle_description		{Stereo RX Channel use right only}
		@h2xmle_bitfieldEnd

		@h2xmle_bitfield			{0x00001000}
			@h2xmle_bitName			{Bit_12}
			@h2xmle_description		{low-latency-complexity filterbank N = 64, R = 16}
		@h2xmle_bitfieldEnd

		@h2xmle_bitfield			{0x00002000}
			@h2xmle_bitName			{Bit_13}
			@h2xmle_description		{enable low-power mode to bypass filterbank processing when RX is NULL}
		@h2xmle_bitfieldEnd

		@h2xmle_bitfield			{0xFFFFC000}
			@h2xmle_bitName			{Bit_31_14}
			@h2xmle_description		{Reserved Bit[31:14]}
			@h2xmle_visibility		{hide}
		@h2xmle_bitfieldEnd         */

    int16_t mic_ch_map_l16[FFECNS_MAX_TX_CHAN];   /* Microphone input mapping to channels.
                                                This array is indexed by microphone index (PCM slot).
                                                Each element value defines the channel in the
                                                FrameBufferType where the microphone data is placed.
                                                This mapping is to be applied outside of
                                                MecProcess, during population of FrameBufferType. */

    /**< @h2xmle_description {Channel Map for path of Mic1 to Mic8} 
         @h2xmle_defaultList {0,1,2,3,4,5,6,7}
         @h2xmle_group	     {GLOBAL}
         @h2xmle_range       {0..7} 
         @h2xmle_policy      {Advanced} */

    int16_t aec_mas_chan_map[FFECNS_MAX_TX_CHAN];      /* channel atteuation during EC,
                                                       */
    /**< @h2xmle_description {Channel gain mask for Mic1 to Mic8 during EC} 
         @h2xmle_default     {0x7FFF} 
         @h2xmle_group	     {GLOBAL}
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} */

    int16_t input_gain_l16q13[FFECNS_MAX_TX_CHAN];   /* Fixed scaling factor to be applied to each input channel.
                                                Negative scaling factor indicates a 180 degree phase shift.
                                                Zero indicates no scaling applied. */
    /**< @h2xmle_description {Input Gain for Mic 1 to Mic 8} 
         @h2xmle_default     {0x0} 
         @h2xmle_group	     {Gain}
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} */

    int16_t prim_in_chan_l16;                  /* primary input channel (0-3) */
    /**< @h2xmle_description {Channel Map for the primary mic path (hardcoded)} 
         @h2xmle_default     {0x0} 
	 @h2xmle_group	     {GLOBAL}
         @h2xmle_range       {0..0} 
         @h2xmle_policy      {Advanced} */


    int16_t aec_out_chan_l16;                   /* output channel to contain echo-cleaned primary
                                                channel signal, or -1 if this output is not desired.
                                                Overrides any other channel mapping*/
    /**< @h2xmle_description {output channel to contain echo canceled primary  channel signal, or -1 if this output is not desired.} 
         @h2xmle_default     {-1} 
	 @h2xmle_group	     {GLOBAL}
         @h2xmle_range       {-1..7} 
         @h2xmle_policy      {Advanced} */


    int16_t delay_out_chan_l16;                 /* output channel to contain delayed primary 
                                                channel signal, or -1 if this output is not desired.
                                                Overrides any other channel mapping    from masking, or chanMapL16. */
    /**< @h2xmle_description {output channel to contain delayed primary  channel signal, or -1 if this output is not desired.} 
         @h2xmle_default     {-1} 
         @h2xmle_group	     {GLOBAL}		 
         @h2xmle_range       {-1..7} 
         @h2xmle_policy      {Advanced} */


    int16_t aec_est_chan_l16;                   /* output channel to contain echo estimate of primary 
                                                channel signal, or -1 if this output is not desired.
                                                Overrides any other channel mapping    from masking, or chanMapL16. */
    /**< @h2xmle_description {output channel to contain echo estimate of primary channel signal, or -1 if this output is not desired. (hard-coded)} 
         @h2xmle_default     {-1} 
         @h2xmle_group	     {GLOBAL}
         @h2xmle_range       {-1..7} 
         @h2xmle_policy      {Advanced} */

    int16_t out_gain_l16q11;                   /* Output gain for primary channel (Q4.11).  Zero indicates gain bypass. */
    /**< @h2xmle_description {output gain for all output channels} 
         @h2xmle_default     {0x0} 
         @h2xmle_group	     {Gain}
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Basic} */

    int16_t reserved;
    /**< @h2xmle_description {Added for alignment. Client must set this field to zero.} 
         @h2xmle_default     {0} 
         @h2xmle_range       {0x0000..0x7FFF}
		 @h2xmle_visibility  {hide}
         @h2xmle_readOnly    {true} */

}ffecns_global_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif

/* ffecns_aec_channel_param_t
	This structure defines the parameters for one AEC channel, which is related to ffecns_aec_channel_param_t
    The size of aec_mem_ptr in ffecns_aec_channel_param_t depends on num_ch and aec_chan_size
   */
/** @h2xmlp_subStruct */

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffecns_aec_channel_param_t
{
    int16_t aec_cfg_clip_flag_frac_l16q15;             // meccfg.fbcfg.aeccfg.aecsfcfg[ch].clipflagcfg.sclipflagfrac_q15
    /**< @h2xmle_description {path change detector fg output to mic energy ratio parameter} 
         @h2xmle_default     {164} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
	 @h2xmle_groupIndex  {0}
	 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{EC_Clipper}
	 @h2xmle_groupIndexEnd

	 @h2xmle_groupIndex  {1}
	 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{EC_Clipper}
	 @h2xmle_groupIndexEnd

	 @h2xmle_groupIndex  {2}
	 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{EC_Clipper}
	 @h2xmle_groupIndexEnd

	 @h2xmle_groupIndex  {3}
	 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{EC_Clipper}
	 @h2xmle_groupIndexEnd
		 
	 @h2xmle_groupIndex  {4}
	 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{EC_Clipper}
	 @h2xmle_groupIndexEnd
		 
	 @h2xmle_groupIndex  {5}
	 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{EC_Clipper}
	 @h2xmle_groupIndexEnd
		 
	 @h2xmle_groupIndex  {6}
	 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{EC_Clipper}
	 @h2xmle_groupIndexEnd
		 
	 @h2xmle_groupIndex  {7}
	 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{EC_Clipper}
	 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_pathchange_flag_frac_l16q15;       // fbcfg.aeccfg.aecsfcfg[ch].pathchangeflagcfg.spathchangeflagfrac_q15
    /**< @h2xmle_description {path change detector filter coefficients ratio parameter} 
         @h2xmle_default     {27305} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
	 @h2xmle_groupIndex  {0}
	 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{PCD}
	 @h2xmle_groupIndexEnd

	 @h2xmle_groupIndex  {1}
	 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{PCD}
	 @h2xmle_groupIndexEnd

	 @h2xmle_groupIndex  {2}
	 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{PCD}
	 @h2xmle_groupIndexEnd

	 @h2xmle_groupIndex  {3}
	 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{PCD}
	 @h2xmle_groupIndexEnd
		 
	 @h2xmle_groupIndex  {4}
	 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{PCD}
	 @h2xmle_groupIndexEnd
		 
	 @h2xmle_groupIndex  {5}
	 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{PCD}
	 @h2xmle_groupIndexEnd
		 
	 @h2xmle_groupIndex  {6}
	 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{PCD}
	 @h2xmle_groupIndexEnd
		 
	 @h2xmle_groupIndex  {7}
	 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{PCD}
	 @h2xmle_groupIndexEnd */
		 
    int16_t aec_cfg_pathchange_wgt_enfact_d_l16q15;    // meccfg.fbcfg.aeccfg.aecsfcfg[ch].pathchangeflagcfg.pcwgtenfactdlq15
    /**< @h2xmle_description {path change detector condition for background filter having better convergence than foreground filter} 
         @h2xmle_default     {13107} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
	 @h2xmle_groupIndex  {0}
	 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{PCD}
	 @h2xmle_groupIndexEnd

	 @h2xmle_groupIndex  {1}
	 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{PCD}
	 @h2xmle_groupIndexEnd

	 @h2xmle_groupIndex  {2}
	 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{PCD}
	 @h2xmle_groupIndexEnd

	 @h2xmle_groupIndex  {3}
	 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{PCD}
	 @h2xmle_groupIndexEnd
		 
	 @h2xmle_groupIndex  {4}
	 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{PCD}
	 @h2xmle_groupIndexEnd
		 
	 @h2xmle_groupIndex  {5}
	 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{PCD}
	 @h2xmle_groupIndexEnd
		 
	 @h2xmle_groupIndex  {6}
	 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{PCD}
	 @h2xmle_groupIndexEnd
		 
         @h2xmle_groupIndex  {7}
	 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{PCD}
	 @h2xmle_groupIndexEnd */
	
    int16_t aec_cfg_sflen_l16;                         // fbcfg.aeccfg.aecsfcfg[ch].filtadpcfg.sflen
    /**< @h2xmle_description {adaptive filter length for lowband} 
         @h2xmle_default     {8} 
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Basic} 
		 
	 @h2xmle_groupIndex  {0}
	 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
	 @h2xmle_groupIndexEnd

	 @h2xmle_groupIndex  {1}
	 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
	 @h2xmle_groupIndexEnd

	 @h2xmle_groupIndex  {2}
	 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
	 @h2xmle_groupIndexEnd

	 @h2xmle_groupIndex  {3}
	 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
	 @h2xmle_groupIndexEnd
		 
	 @h2xmle_groupIndex  {4}
	 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
	 @h2xmle_groupIndexEnd
		 
	 @h2xmle_groupIndex  {5}
	 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
	 @h2xmle_groupIndexEnd
		 
	 @h2xmle_groupIndex  {6}
	 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
	 @h2xmle_groupIndexEnd
		 
	 @h2xmle_groupIndex  {7}
	 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
	 @h2xmle_groupIndexEnd */
		 
    int16_t aec_cfg_sflen_l16_hb;
    /**< @h2xmle_description {adaptive filter length for high band} 
         @h2xmle_default     {10} 
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Basic} 
	 @h2xmle_groupIndex  {0}
	 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
	 @h2xmle_groupIndexEnd

	 @h2xmle_groupIndex  {1}
	 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
	 @h2xmle_groupIndexEnd

	 @h2xmle_groupIndex  {2}
	 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
	 @h2xmle_groupIndexEnd

	 @h2xmle_groupIndex  {3}
	 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
	 @h2xmle_groupIndexEnd
		 
	 @h2xmle_groupIndex  {4}
	 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
	 @h2xmle_groupIndexEnd
		 
	 @h2xmle_groupIndex  {5}
	 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
	 @h2xmle_groupIndexEnd
		 
	 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd */
		 
    int16_t aec_cfg_wgtq_l16;                          // fbcfg.aeccfg.aecsfcfg[ch].filtadpcfg.swgtq
    /**< @h2xmle_description {q-factor for filter coefficients} 
         @h2xmle_default     {3} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd */
		 
    int16_t aec_cfg_r_update_l16;                      // fbcfg.aeccfg.aecsfcfg[ch].filtadpcfg.fgupdate
    /**< @h2xmle_description {period (#of far-end active frames) of fg filter update} 
         @h2xmle_default     {0} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd */
		 
    int16_t aec_cfg_bg_update_dt;                      // flag to enable bg filter update during dt
    /**< @h2xmle_description {flag to enable bg filter update during dt} 
         @h2xmle_default     {1} 
         @h2xmle_range       {0..1} 
         @h2xmle_policy      {Basic} 
		 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_r_lrate_l16;                       // fbcfg.aeccfg.aecsfcfg[ch].filtadpcfg.rlrate
    /**< @h2xmle_description {right-shift on sLrate for fg filter update} 
         @h2xmle_default     {4} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_sqshift_max_l16;                   // fbcfg.aeccfg.aecsfcfg[ch].filtadpcfg.sqshift_ma
    /**< @h2xmle_description {sQshift upperbound for fg filter update} 
         @h2xmle_default     {-10} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_sbg_mic_frac_l16q15;               // fbcfg.aeccfg.aecsfcfg[ch].filtadpcfg.sbgmicfrac_q15
    /**< @h2xmle_description {convergence download test ERLE threshold} 
         @h2xmle_default     {16384} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Basic} 
		 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_saec_full_learn_frames_l16;        // fbcfg.aeccfg.aecsfcfg[ch].filtadpcfg.saec_full_learn_frames
    /**< @h2xmle_description {# of intial far-end active (no dt) frames enforcing down-load irrespective of tests} 
         @h2xmle_default     {40} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_saec_num_frames_l16;               // fbcfg.aeccfg.aecsfcfg[ch].filtadpcfg.snumframes
    /**< @h2xmle_description {convergence download test pass frame counter} 
         @h2xmle_default     {4} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_dtd_threshold_lec_l16q15;          // fbcfg.aeccfg.aecsfcfg[ch].dtdcfg.sdtd_threshold_lec_q15
    /**< @h2xmle_description {double-talk detector threshold} 
         @h2xmle_default     {27852} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Basic} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup    {Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup    {Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup    {Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup    {Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup    {Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup    {Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup    {Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup    {Double_Talk_Detection}
		 @h2xmle_groupIndexEnd*/

    int16_t aec_cfg_dtd_threshold_postp_l16q15;        // fbcfg.aeccfg.aecsfcfg[ch].dtdcfg.sdtd_threshold_postp_q15
    /**< @h2xmle_description {double-talk detector threshold for EC PP} 
         @h2xmle_default     {27852} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd*/

    int16_t aec_cfg_l_rate_l16;                        // fbcfg.aeccfg.aecsfcfg[ch].filtadpcfg.slrate
    /**< @h2xmle_description {left-shift approximating AEC filter learning rate} 
         @h2xmle_default     {-1} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Basic} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_l_rate_l16_dt;                     // fbcfg.aeccfg.aecsfcfg[ch].filtadpcfg.slrate_dt
    /**< @h2xmle_description {sLrate offset (right shift) during double-talk detected frame} 
         @h2xmle_default     {0} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Basic} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_bg_fg_frac_l16q15;                 // fbcfg.aeccfg.aecsfcfg[ch].filtadpcfg.sbgfgfrac_q15
    /**< @h2xmle_description {convergence download test bg/fg threshold} 
         @h2xmle_default     {32767} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Basic} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_divg_mic_fg_frac_l16q15;           // fbcfg.aeccfg.aecsfcfg[ch].filtadpcfg.sdivgdlmicfgfrac_q15
    /**< @h2xmle_description {divvergence download test bg/fg threshold} 
         @h2xmle_default     {16384} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_divg_bg_mic_frac_l16q15;           // fbcfg.aeccfg.aecsfcfg[ch].filtadpcfg.sdivgdlbgmicfrac_q15
    /**< @h2xmle_description {divergence download test bg/mic threshold} 
         @h2xmle_default     {16384} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_divg_frame_trig_l16;               // fbcfg.aeccfg.aecsfcfg[ch].filtadpcfg.divdlframetrig
    /**< @h2xmle_description {divergence download test frame trigger threshold} 
         @h2xmle_default     {5} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_dtd_hangover_max_l16;              // fbcfg.aeccfg.aecsfcfg[ch].dtdcfg.sdtd_hangover_max
    /**< @h2xmle_description {double-talk detector hang-over duration} 
         @h2xmle_default     {20} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_dtd_ni_scalefactor_l16;            // fbcfg.aeccfg.aecsfcfg[ch].dtdcfg.snoise_correction_scalefactor
    /**< @h2xmle_description {double-talk detector compensate for the noise level in the numerator by using the scaled far-end excitation energy} 
         @h2xmle_default     {41} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_dtd_bg_mic_frac_l16q15;            // fbcfg.aeccfg.aecsfcfg[ch].dtdcfg.sbgmicfrac_q15
    /**< @h2xmle_description {double-talk detector bg/mic threshold} 
         @h2xmle_default     {656} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_dtd_good_frames_l16;               // fbcfg.aeccfg.aecsfcfg[ch].dtdcfg.dtd_good_frames
    /**< @h2xmle_description {double-talk detected frame counter} 
         @h2xmle_default     {200} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_sen_sm_bit;                        // fbcfg.aeccfg.aecsfcfg[ch].filtadpcfg.sen_sm_bit         //sachin: changing position it to make 4 byte aligned
    /**< @h2xmle_description {Right shift for energy smoothing} 
         @h2xmle_default     {2} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd*/
		 
    int32_t aec_cfg_laec_sc2_q31;                      // fbcfg.aeccfg.aecsfcfg[ch].filtadpcfg.laec_sc2_q31
    /**< @h2xmle_description {regularization parameter to stablize NLMS update} 
         @h2xmle_default     {3280} 
         @h2xmle_range       {0x00000000..0x7FFFFFFF} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_stestsuccess;                      // fbcfg.aeccfg.aecsfcfg[ch].filtadpcfg.stestsuccess
    /**< @h2xmle_description {convergence download test minimum passes} 
         @h2xmle_default     {3} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Basic} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_sdtd_al_bitshift;                  // fbcfg.aeccfg.aecsfcfg[ch].dtdcfg.sdtd_al_bitshift,
    /**< @h2xmle_description {double-talk detector 2nd order statistics smoothing approximation right-shift} 
         @h2xmle_default     {3} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_sdtd_trigger;                      // fbcfg.aeccfg.aecsfcfg[ch].dtdcfg.sdtd_trigger
    /**< @h2xmle_description {frames of double-talk that triggers hangover period} 
         @h2xmle_default     {1} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_saec_full_learn_frames_dt_l16;     // fbcfg.aeccfg.aecsfcfg[ch].filtadpcfg.saec_full_learn_frames_dt
    /**< @h2xmle_description {# of intial far-end active (ignoring dt) frames enforcing down-load} 
         @h2xmle_default     {0} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_pathchange_al_bitshift;            // fbcfg.aeccfg.aecsfcfg[ch].pathchangeflagcfg.spathchangeflag_al_bitshift
    /**< @h2xmle_description {Smoothing factor used in computing second order statistics for the EC path change flag} 
         @h2xmle_default     {4} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{PCD}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{PCD}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{PCD}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{PCD}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{PCD}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{PCD}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{PCD}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{PCD}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_pathchange_shangovermax;           // fbcfg.aeccfg.aecsfcfg[ch].pathchangeflagcfg.shangovermax
    /**< @h2xmle_description {hangover frames for path change flag} 
         @h2xmle_default     {15} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{PCD}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{PCD}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{PCD}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{PCD}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{PCD}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{PCD}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{PCD}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{PCD}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_clip_flag_al_bitshift;             // fbcfg.aeccfg.aecsfcfg[ch].clipflagcfg.sclipflag_al_bitshift
    /**< @h2xmle_description {Smoothing factor used in computing second order statistics for the EC Clip flag} 
         @h2xmle_default     {3} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{EC_Clipper}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{EC_Clipper}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{EC_Clipper}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{EC_Clipper}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{EC_Clipper}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{EC_Clipper}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{EC_Clipper}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{EC_Clipper}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_vsscfg_svss_al_bitshift;           // fbcfg.aeccfg.aecsfcfg[ch].vsscfg.svss_al_bitshift
    /**< @h2xmle_description {Smoothing factor used in computing second order statistics for the EC vss} 
         @h2xmle_default     {9} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
    int32_t aec_cfg_vsscfg_lvss_eps_q31;               // fbcfg.aeccfg.aecsfcfg[ch].vsscfg.lvss_eps_q31
    /**< @h2xmle_description {vss Regularization factor} 
         @h2xmle_default     {2148} 
         @h2xmle_range       {0x00000000..0x7FFFFFFF} 
         @h2xmle_policy      {Advanced} */
		 
    int16_t aec_cfg_vsscfg_svss_min_mu_q15;            // fbcfg.aeccfg.aecsfcfg[ch].vsscfg.svss_min_mu_q15
    /**< @h2xmle_description {vss (variable-step size) mu lowerbound} 
         @h2xmle_default     {32767} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
    int16_t aec_cfg_vsscfg_svss_min_statistic_blk_size;// fbcfg.aeccfg.aecsfcfg[ch].vsscfg.svss_min_statistic_blk_size
    /**< @h2xmle_description {num of frames in a minStat block} 
         @h2xmle_default     {20} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
    int16_t aec_cfg_vsscfg_svss_min_statistic_num_blk; // fbcfg.aeccfg.aecsfcfg[ch].vsscfg.svss_min_statistic_num_blk
    /**< @h2xmle_description {num of MnStat blocks} 
         @h2xmle_default     {10} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
    int16_t aec_cfg_vsscfg_svss_min_statistic_num_frames; //fbcfg.aeccfg.aecsfcfg[ch].vsscfg.svss_min_statistic_num_frames
    /**< @h2xmle_description {number of frames for minimum statistics} 
         @h2xmle_default     {200} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
    int16_t aec_cfg_sdtd_onlyflag_al_bitshift;            // fbCfg.aecCfg.aecSfCfg[ch].dtdOnlyFlagCfg.sDTDonlyFlag_Al_Bitshift
    /**< @h2xmle_description {Smoothing factor used in computing second order statistics for the dtD only flag} 
         @h2xmle_default     {3} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_dleps_q15;                          // fbCfg.aecCfg.aecSfCfg[ch].filtAdpCfg.dlEps_Q15
    /**< @h2xmle_description {small signal amplitude constant in BgMic, per sample} 
         @h2xmle_default     {10} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_saec_down_shift;                    // fbCfg.aecCfg.aecSfCfg[ch].filtAdpCfg.sAec_Down_Shift 
    /**< @h2xmle_description {Head room during filtering} 
         @h2xmle_default     {0} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd*/
		 
    int16_t aec_cfg_div_dleps_q15;                      // fbCfg.aecCfg.aecSfCfg[ch].filtAdpCfg.divDlEps_Q15
    /**< @h2xmle_description {small signal amplitude constant in DivgDlMic, per sample} 
         @h2xmle_default     {200} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Download_Control}
		 @h2xmle_groupIndexEnd*/
		 
    int32_t aec_cfg_ldtd_eps_q31;                       // fbCfg.aecCfg.aecSfCfg[ch].dtdCfg.lDtd_Eps_Q31
    /**< @h2xmle_description {regularization parameter to stabilize dtD statistics computation} 
         @h2xmle_default     {1073760} 
         @h2xmle_range       {0x80000000..0x7FFFFFFF} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_groupIndex  {0}
		 @h2xmle_group	    {LEC_MIC1}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	    {LEC_MIC2}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	    {LEC_MIC3}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	    {LEC_MIC4}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	    {LEC_MIC5}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	    {LEC_MIC6}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	    {LEC_MIC7}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	    {LEC_MIC8}
         @h2xmle_subgroup 	{Double_Talk_Detection}
		 @h2xmle_groupIndexEnd*/
}ffecns_aec_channel_param_t;   

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif

/* ffecns_aec_param_t
	This structure defines the parameters associated with FFECNS_AEC_PARAM in enum FFECNS_STRUCT_IDS, which is in ffecns_api.h
struct_size:			[IN/OUT]: structure size
							[IN]: caller fills size for the size of the payload structure in apis set(...) & get(...). expects size >= sizeof(ffecns_aec_param_t)
						   [OUT]: callee fills the appropriate size of the buffer filled in api get(...),sizeof(ffecns_aec_param_t)
   */

/** @h2xmlp_parameter    {"PARAM_ID_FFECNS_AEC", FFECNS_AEC_PARAM}
    @h2xmlp_description  { Structure for AEC params\n} */

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffecns_aec_param_t
{
   int16_t aec_f_vad_shangover_max;             // fbCfg.aecCfg.farEndVadCfg.sHangoverMax
    /**< @h2xmle_description {Far-end VAD hangover duration (frames)} 
         @h2xmle_default     {1} 
		 @h2xmle_group	    {LEC_Common}
		 @h2xmle_subgroup   {Far_end_VAD}
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
   int16_t aec_f_vad_shangover_max_pf;           // fbCfg.aecCfg.farEndVadCfg.sHangoverMaxPF
    /**< @h2xmle_description {Far-end VAD for EC post-filtering hangover duration (frames)} 
         @h2xmle_default     {10} 
		 @h2xmle_group	    {LEC_Common}
		 @h2xmle_subgroup   {Far_end_VAD}
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
   int16_t aec_f_vad_strigger;                 // fbCfg.aecCfg.farEndVadCfg.sTrigger  
    /**< @h2xmle_description {Far-end Vad trigger} 
         @h2xmle_default     {1} 
		 @h2xmle_group	    {LEC_Common}
		 @h2xmle_subgroup   {Far_end_VAD}
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
   int16_t aec_glb_flag_fvad_nb_cnt;
    /**< @h2xmle_description {minimum number of active subbands to determine far-end activity (0-4kHz)} 
         @h2xmle_default     {5} 
		 @h2xmle_group	    {LEC_Common}
		 @h2xmle_subgroup   {Far_end_VAD}
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
   int16_t aec_glb_flag_fvad_hb_cnt;
    /**< @h2xmle_description {minimum number of active subbands to determine far-end activity (>4kHz)} 
         @h2xmle_default     {2} 
		 @h2xmle_group	    {LEC_Common}
		 @h2xmle_subgroup   {Far_end_VAD}
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
   int16_t aec_glb_flag_fvad_pf_cnt;
    /**< @h2xmle_description {minimum number of active subbands to determine far-end activity for EC post-filtering} 
         @h2xmle_default     {2} 
		 @h2xmle_group	    {LEC_Common}
		 @h2xmle_subgroup   {Far_end_VAD}
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Advanced} */   
   
   int16_t aec_glb_flag_dtd_nb_cnt;
    /**< @h2xmle_description {minimum number of active subbands to determine double-talk activity (0-4kHz)} 
         @h2xmle_default     {5} 
		 @h2xmle_group	    {LEC_Common}
		 @h2xmle_subgroup   {Double_Talk_Detection}
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
   int16_t aec_glb_flag_dtd_hb_cnt;
    /**< @h2xmle_description {minimum number of active subbands to determine double-talk activity (>4kHz)} 
         @h2xmle_default     {5} 
		 @h2xmle_group	    {LEC_Common}
		 @h2xmle_subgroup   {Double_Talk_Detection}
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
   int16_t aec_glb_flag_clip_nb_cnt;
    /**< @h2xmle_description {minimum number of active subbands to determine clipping (0-4kHz)} 
         @h2xmle_default     {20} 
		 @h2xmle_group	    {LEC_Common}
		 @h2xmle_subgroup   {EC_Clipper}
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
   int16_t aec_glb_flag_clip_hb_cnt;
    /**< @h2xmle_description {minimum number of active subbands to determine clipping (>4kHz)} 
         @h2xmle_default     {20} 
		 @h2xmle_group	    {LEC_Common}
		 @h2xmle_subgroup   {EC_Clipper}
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
   int16_t aec_glb_flag_pcd_nb_cnt;
    /**< @h2xmle_description {minimum number of active subbands to determine path change activity (0-4kHz)} 
         @h2xmle_default     {16} 
		 @h2xmle_group	    {LEC_Common}
		 @h2xmle_subgroup   {PCD}
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
   int16_t aec_glb_flag_pcd_hb_cnt;
    /**< @h2xmle_description {minimum number of active subbands to determine path change activity (>4kHz)} 
         @h2xmle_default     {16} 
		 @h2xmle_group	    {LEC_Common}
		 @h2xmle_subgroup   {PCD}
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
   int16_t fb_cfg_force_max_bw_flag;                    // fbCfg.filterBankForceMaxBwFlag  
    /**< @h2xmle_description {if set, force processing up to Fs/2 in AEC path, overriding bw_limit params below} 
         @h2xmle_default     {0} 
		 @h2xmle_group	    {GLOBAL}
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
   int16_t fb_cfg_primary_mic_aec_bw_limit;             // fbCfg.primaryMicAecBwLimit
    /**< @h2xmle_description {max frequency to process for primary mic 4000, 8000, 16000, 24000Hz} 
         @h2xmle_default     {8000} 
		 @h2xmle_group	    {GLOBAL}
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
   int16_t fb_cfg_nonPrimary_mic_aec_bw_limit;          // fbCfg.nonPrimaryMicAecBwLimit
    /**< @h2xmle_description {max frequency to process for non-primary mic 4000, 8000, 16000, 24000Hz} 
         @h2xmle_default     {8000} 
		 @h2xmle_group	    {GLOBAL}
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
   int16_t fb_cfg_multi_chan_rx_bw_limit;               // fbCfg.multiChanRxCfg.bwLimit
    /**< @h2xmle_description {max frequency to perform surround sound filtering 4000, 8000, 16000, 24000Hz} 
         @h2xmle_default     {8000} 
		 @h2xmle_group	    {LEC_Common}
		 @h2xmle_subgroup   {Room_Response}
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
   int16_t fb_cfg_multi_chan_rx_delay_tap_idx;          // fbCfg.multiChanRxCfg.delayTapIdx
    /**< @h2xmle_description {above fb_cfg_multi_chan_rx_bw_limit, we delay the selected input channel by this amount} 
         @h2xmle_default     {2} 
		 @h2xmle_group	    {LEC_Common}
		 @h2xmle_subgroup   {Room_Response}
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
   int16_t fb_cfg_multi_chan_rx_delay_chan_idx;         // fbCfg.multiChanRxCfg.delayChanIdx
    /**< @h2xmle_description {selects which channel to use for the delay} 
         @h2xmle_default     {0} 
		 @h2xmle_group	    {LEC_Common}
		 @h2xmle_subgroup   {Room_Response}
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
   int16_t fb_cfg_multi_chan_rx_response_len;           // fbCfg.multiChanRxCfg.responseLen
    /**< @h2xmle_description {filter lengths for surround sound room responses} 
         @h2xmle_default     {16} 
		 @h2xmle_group	    {LEC_Common}
		 @h2xmle_subgroup   {Room_Response}
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Basic} */
		 
   int16_t aec_num_subframes_per_call;         // fbCfg.numSubFramesPerCall
    /**< @h2xmle_description {number of subframes processed by per AEC call }
         @h2xmle_default     {2}
		 @h2xmle_group	    {GLOBAL}
         @h2xmle_range       {0x0000..0x0002}
         @h2xmle_policy      {Advanced} */
		 
   int32_t aec_f_vad_lth_en_l32q20;                     // fbCfg.aecCfg.farEndVadCfg.lThEn
    /**< @h2xmle_description {Far-end VAD subband energy threshold for  LEC control} 
         @h2xmle_default     {0x3} 
		 @h2xmle_group	    {LEC_Common}
		 @h2xmle_subgroup   {Far_end_VAD}
         @h2xmle_range       {0x80000000..0x7FFFFFFF} 
         @h2xmle_policy      {Basic} */
		 
   int32_t num_chan;                                    // dynamic aec channel number
    /**< @h2xmle_description {same as the number of tx channels} 
         @h2xmle_default     {8} 
		 @h2xmle_group	    {GLOBAL}
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Advanced} */
		 
   ffecns_aec_channel_param_t aec_chan_param[0];        // aec channel parameters
    /**< @h2xmle_description		{same as the number of tx channels}
	     @h2xmle_variableArraySize  {num_chan}
         @h2xmle_policy				{Advanced} */
}ffecns_aec_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif

/* ffecns_ecpp_clip_param_t
	This structure defines the parameters associated with FFECNS_ECPP_CLIP_PARAM in enum FFECNS_STRUCT_IDS, which is in ffecns_api.h
struct_size:			[IN/OUT]: structure size
							[IN]: caller fills size for the size of the payload structure in apis set(...) & get(...). expects size >= sizeof(ffecns_ecpp_param_t)
						   [OUT]: callee fills the appropriate size of the buffer filled in api get(...),sizeof(ffecns_ecpp_param_t)
   */

/** @h2xmlp_parameter    {"PARAM_ID_FFECNS_ECPP_CLIP", FFECNS_ECPP_CLIP_PARAM}
    @h2xmlp_description  { Structure for the EC CLIP params \n} */

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffecns_ecpp_clip_param_t
{	
	int16_t aec_pf_clip_saec_clip_factor2_l16q15; //aecPfClipCfg.sAecClipFactor2Q15 initial clipping  factor (attenuation)
	/**< @h2xmle_description {Input Gain for Mic-I input}
         @h2xmle_default     {0x00}
		 @h2xmle_group	    {ECPP}
		 @h2xmle_subgroup   {EC_Clipper}
         @h2xmle_range       {0x8000..0x7FFF}
         @h2xmle_policy      {Advanced} */

    int16_t aec_pf_clip_sinit_frames_l16;       // aecPfClipCfg.sInitFrames
	/**< @h2xmle_description {Input Gain for Mic-I input}
         @h2xmle_default     {0x00}
		 @h2xmle_group	    {ECPP}
		 @h2xmle_subgroup   {EC_Clipper}
         @h2xmle_range       {0x8000..0x7FFF}
         @h2xmle_policy      {Advanced} */	

    int16_t aec_pf_clip_saec_clip_factor_l16q15; // aecPfClipCfg.sAecClipFactorQ15
	/**< @h2xmle_description {Input Gain for Mic-I input}
         @h2xmle_default     {0x00}
		 @h2xmle_group	    {ECPP}
		 @h2xmle_subgroup   {EC_Clipper}
         @h2xmle_range       {0x8000..0x7FFF}
         @h2xmle_policy      {Advanced} */	

   int16_t reserved;                                    // !! used for alignment
    /**< @h2xmle_description {Added for alignment. Client must set this field to zero.} 
         @h2xmle_default     {0} 
         @h2xmle_range       {0x0000..0x7FFF}
		 @h2xmle_visibility  {hide}
         @h2xmle_readOnly    {true} */

}ffecns_ecpp_clip_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif

/* ffecns_delay_param_t
	This structure defines the parameters associated with FFECNS_DELAY_PARAM in enum FFECNS_STRUCT_IDS, which is in ffecns_api.h
struct_size:			[IN/OUT]: structure size
							[IN]: caller fills size for the size of the payload structure in apis set(...) & get(...). expects size >= sizeof(ffecns_delay_param_t)
						   [OUT]: callee fills the appropriate size of the buffer filled in api get(...),sizeof(ffecns_delay_param_t)
   */

/** @h2xmlp_parameter    {"PARAM_ID_FFECNS_DELAY", FFECNS_DELAY_PARAM}
    @h2xmlp_description  { Structure for the DELAY params\n} */

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffecns_delay_param_t
{
    int16_t ec_delay_l16;
    /**< @h2xmle_description {the echo path delay (equivalent samples in 8kHz)} 
         @h2xmle_default     {0} 
		 @h2xmle_group	    {Echo_Path_Delay}
         @h2xmle_range       {-2400..2400} 
         @h2xmle_policy      {Basic} */

    int16_t surround_tx_delay;
    /**< @h2xmle_description {additional tx delay in surround EC case (equivalent samples in 8kHz)} 
         @h2xmle_default     {0} 
		 @h2xmle_group	    {Echo_Path_Delay}
         @h2xmle_range       {0..2400} 
         @h2xmle_policy      {Advanced} */

}ffecns_delay_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif

/* ffecns_surround_ec_calibration_param_t
	This structure defines the parameters associated with FFECNS_DELAY_PARAM in enum FFECNS_STRUCT_IDS, which is in ffecns_api.h
struct_size:			[IN/OUT]: structure size
							[IN]: caller fills size for the size of the payload structure in apis set(...) & get(...). expects size >= sizeof(ffecns_surround_ec_calibration_param_t)
						   [OUT]: callee fills the appropriate size of the buffer filled in api get(...),sizeof(ffecns_surround_ec_calibration_param_t)
   */

/** @h2xmlp_parameter    {"PARAM_ID_FFECNS_SURROUND_EC_CAL", FFECNS_SURROUND_EC_CAL_PARAM}
    @h2xmlp_description  { Structure for the Surround EC calibration params\n}
    @h2xmlp_toolPolicy   {CALIBRATION}*/

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"

#endif
typedef struct ffecns_surround_ec_calibration_param_t
{
   int16_t num_tx_chan;
   /**< @h2xmle_description {number of the speaker references during calibration}
        @h2xmle_default     {4}
        @h2xmle_range       {1..8}
        @h2xmle_policy      {Advanced} */

    int16_t num_rx_chan;
    /**< @h2xmle_description {number of the speaker references during calibration} 
         @h2xmle_default     {4} 
         @h2xmle_range       {2..6} 
         @h2xmle_policy      {Advanced} */

    int16_t room_response_sflen;
    /**< @h2xmle_description {number of the subband filter taps used for calibration} 
         @h2xmle_default     {16} 
         @h2xmle_range       {0..40} 
         @h2xmle_policy      {Advanced} */	

    int16_t num_subband;
    /**< @h2xmle_description {number of subbands used for calibration} 
         @h2xmle_default     {257} 
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Advanced} */

    int16_t wgt_q;                      // wgtQ of room response coefficients       
    /**< @h2xmle_description {q-factor for filter coefficients used for calibration} 
         @h2xmle_default     {3} 
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} */	

    int16_t reserved;
    /**< @h2xmle_description {Added for alignment. Client must set this field to zero.}
         @h2xmle_default     {0}
         @h2xmle_range       {0x0000..0x7FFF}
         @h2xmle_visibility  {hide}
         @h2xmle_readOnly    {true} */

    int32_t room_response[0];
    /**< @h2xmle_description {pointer to the room response coefficients} 
         @h2xmle_default     {0x00000000} 
		 @h2xmle_variableArraySize   {"num_tx_chan * num_rx_chan * room_response_sflen * num_subband"}
         @h2xmle_policy      {Advanced}
		 @h2xmle_rawData
         @h2xmle_displayType {file} */    

}ffecns_surround_ec_calibration_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif
/* ffecns_hw_af_param_t

This structure defines the parameters associated with FFECNS_HW_AF_PARAM in enum FFECNS_STRUCT_IDS, which is in ffecns_api.h

Supported only for set(...) api

hw_flag: [IN/OUT]: flag to decide if hw adaptive filter should be active or not

*/

/** @h2xmlp_parameter    {"PARAM_ID_FFECNS_HW_AF", FFECNS_HW_AF_PARAM}
    @h2xmlp_description {Param for storing state of hardware flag}
    @h2xmlp_toolPolicy  {CALIBRATION}
    @h2xmlp_readOnly    {true} */

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffecns_hw_af_param_t
{
    int32_t hw_flag;
     /**< @h2xmle_description {flag to decide if hw adaptive filter should be active or not}
          @h2xmle_default     {0x0000}
          @h2xmle_range       {0x00000000..0x00000001}
          @h2xmle_policy      {Basic} */

} ffecns_hw_af_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif


/** @h2xmlp_parameter    {"PARAM_ID_FFECNS_EC_CALIBRATION_MODE", FFECNS_EC_CALIBRATION_MODE_PARAM}
    @h2xmlp_description {Param to set calibration mode} */

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffecns_ec_calibration_mode_param_t
{
    int16_t enable_calibration_mode;
     /**< @h2xmle_description {flag to enable/disable ec calibration mode}
          @h2xmle_default     {0x0000}
          @h2xmle_range       {0x0000..0x0001}
          @h2xmle_policy      {Basic} */
	int16_t ec_ref_chan_idx;
     /**< @h2xmle_description {rx ec reference channel index which is active during calibration mode }
          @h2xmle_default     {0x0000}
          @h2xmle_range       {0x0000..0x7FFF}
          @h2xmle_policy      {Basic} */

} ffecns_ec_calibration_mode_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif


/* ffecns_ec_af_coeffs_param_t
for multi-channel EC room response calibration. This structure captures af coeffs for a given mic
*/
/** @h2xmlp_subStruct */
#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffecns_ec_af_coeffs_param_t
{   
   int16_t  sf_len;        // number of subband filter taps
    /**< @h2xmle_description {number of subbands filter taps} 
         @h2xmle_default     {16} 
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Basic} */

   int16_t  num_subbands;  // number of subbands, below 8kHz
    /**< @h2xmle_description {number of subbands used for calibration} 
         @h2xmle_default     {257} 
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Basic} */

   int16_t  wgtQ;          // must be the same for all mics
    /**< @h2xmle_description {Q factor fot the filter coefficients} 
         @h2xmle_default     {3} 
         @h2xmle_range       {0..0x7FFF} 
         @h2xmle_policy      {Advanced} */

	int16_t reserved;
	/**< @h2xmle_description {Added for alignment. Client must set this field to zero.}
	@h2xmle_default     {0}
	@h2xmle_range       {0x0000..0x7FFF}
	@h2xmle_visibility  {hide}
	@h2xmle_readOnly    {true} */

   int32_t  filt_coeffs[0]; // assumes the normal filterbank and up to 8kHz
    /**< @h2xmle_description            {16-bit complex filter coefficients}
         @h2xmle_default			{0} 
         @h2xmle_variableArraySize              {"num_subbands * sf_len"}
         @h2xmle_range                  {0x80000000..0x7FFFFFFF} 
         @h2xmle_policy				{Advanced} */

   // complex subband adaptive filter coefficients for each mic
}ffecns_ec_af_coeffs_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif


/*RTC get param*/
/* for multi-channel EC room response calibration */

/** @h2xmlp_parameter    {"PARAM_ID_FFECNS_EC_AF_COEFFS_MIC0", FFECNS_EC_AF_COEFFS_MIC0_PARAM}
    @h2xmlp_description  { Structure for multi-channel EC room response calibration params\n} 
    @h2xmlp_toolPolicy  {RTC_READONLY}
    @h2xmlp_readOnly    {true}*/
#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffecns_ec_af_coeffs_mic0_param_t
{
	ffecns_ec_af_coeffs_param_t ec_af_coeff;        // aec channel parameters
	/**< @h2xmle_description		{sub structure for af coeff per mic}
	@h2xmle_policy				{Advanced} */
}ffecns_ec_af_coeffs_mic0_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif


/** @h2xmlp_parameter    {"PARAM_ID_FFECNS_EC_AF_COEFFS_MIC1", FFECNS_EC_AF_COEFFS_MIC1_PARAM}
@h2xmlp_description  { Structure for multi-channel EC room response calibration params\n}
@h2xmlp_toolPolicy  {RTC_READONLY}
@h2xmlp_readOnly    {true}*/
#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffecns_ec_af_coeffs_mic1_param_t
{
	ffecns_ec_af_coeffs_param_t ec_af_coeff;        // aec channel parameters
	/**< @h2xmle_description		{sub structure for af coeff per mic}
	@h2xmle_policy				{Advanced} */
}ffecns_ec_af_coeffs_mic1_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif


/** @h2xmlp_parameter    {"PARAM_ID_FFECNS_EC_AF_COEFFS_MIC2", FFECNS_EC_AF_COEFFS_MIC2_PARAM}
@h2xmlp_description  { Structure for multi-channel EC room response calibration params\n}
@h2xmlp_toolPolicy  {RTC_READONLY}
@h2xmlp_readOnly    {true}*/
#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffecns_ec_af_coeffs_mic2_param_t
{
	ffecns_ec_af_coeffs_param_t ec_af_coeff;        // aec channel parameters
	/**< @h2xmle_description		{sub structure for af coeff per mic}
	@h2xmle_policy				{Advanced} */
}ffecns_ec_af_coeffs_mic2_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif

/** @h2xmlp_parameter    {"PARAM_ID_FFECNS_EC_AF_COEFFS_MIC3", FFECNS_EC_AF_COEFFS_MIC3_PARAM}
@h2xmlp_description  { Structure for multi-channel EC room response calibration params\n}
@h2xmlp_toolPolicy  {RTC_READONLY}
@h2xmlp_readOnly    {true}*/
#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffecns_ec_af_coeffs_mic3_param_t
{
	ffecns_ec_af_coeffs_param_t ec_af_coeff;        // aec channel parameters
	/**< @h2xmle_description		{sub structure for af coeff per mic}
	@h2xmle_policy				{Advanced} */
}ffecns_ec_af_coeffs_mic3_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif

/** @h2xmlp_parameter    {"PARAM_ID_FFECNS_EC_AF_COEFFS_MIC4", FFECNS_EC_AF_COEFFS_MIC4_PARAM}
@h2xmlp_description  { Structure for multi-channel EC room response calibration params\n}
@h2xmlp_toolPolicy  {RTC_READONLY}
@h2xmlp_readOnly    {true}*/
#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffecns_ec_af_coeffs_mic4_param_t
{
	ffecns_ec_af_coeffs_param_t ec_af_coeff;        // aec channel parameters
	/**< @h2xmle_description		{sub structure for af coeff per mic}
	@h2xmle_policy				{Advanced} */
}ffecns_ec_af_coeffs_mic4_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif


/** @h2xmlp_parameter    {"PARAM_ID_FFECNS_EC_AF_COEFFS_MIC5", FFECNS_EC_AF_COEFFS_MIC5_PARAM}
@h2xmlp_description  { Structure for multi-channel EC room response calibration params\n}
@h2xmlp_toolPolicy  {RTC_READONLY}
@h2xmlp_readOnly    {true}*/
#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffecns_ec_af_coeffs_mic5_param_t
{
	ffecns_ec_af_coeffs_param_t ec_af_coeff;        // aec channel parameters
	/**< @h2xmle_description		{sub structure for af coeff per mic}
	@h2xmle_policy				{Advanced} */
}ffecns_ec_af_coeffs_mic5_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif


/** @h2xmlp_parameter    {"PARAM_ID_FFECNS_EC_AF_COEFFS_MIC6", FFECNS_EC_AF_COEFFS_MIC6_PARAM}
@h2xmlp_description  { Structure for multi-channel EC room response calibration params\n}
@h2xmlp_toolPolicy  {RTC_READONLY}
@h2xmlp_readOnly    {true}*/
#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffecns_ec_af_coeffs_mic6_param_t
{
	ffecns_ec_af_coeffs_param_t ec_af_coeff;        // aec channel parameters
	/**< @h2xmle_description		{sub structure for af coeff per mic}
	@h2xmle_policy				{Advanced} */
}ffecns_ec_af_coeffs_mic6_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif

/** @h2xmlp_parameter    {"PARAM_ID_FFECNS_EC_AF_COEFFS_MIC7", FFECNS_EC_AF_COEFFS_MIC7_PARAM}
@h2xmlp_description  { Structure for multi-channel EC room response calibration params\n}
@h2xmlp_toolPolicy  {RTC_READONLY}
@h2xmlp_readOnly    {true}*/
#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffecns_ec_af_coeffs_mic7_param_t
{
	ffecns_ec_af_coeffs_param_t ec_af_coeff;        // aec channel parameters
	/**< @h2xmle_description		{sub structure for af coeff per mic}
	@h2xmle_policy				{Advanced} */
}ffecns_ec_af_coeffs_mic7_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif


/* RTM */

/** @h2xmlp_parameter    {"PARAM_ID_FFECNS_EC_MONITOR", FFECNS_EC_MONITOR_PARAM}
    @h2xmlp_description  { Structure encapsulating the EC parameters that need to be monitored.\n} 
	@h2xmlp_toolPolicy  {RTM}
	@h2xmlp_readOnly    {true} */

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffecns_ec_monitor_param_t
{
   /* LEC */
   int16_t aec_num_subframes_per_call;
    /**< @h2xmle_description {The number of subframes per HW EC adaptive filtering call} 
         @h2xmle_group	    {LEC_Common}
		 @h2xmle_rtmPlotType {RTM_PLOT} */

   int16_t aec_f_vad_flag;
    /**< @h2xmle_description {far end vad flag for 0-4KHz} 
	 @h2xmle_group       {LEC_Common}
	 @h2xmle_subgroup   {Far_end_VAD} 
         @h2xmle_rtmPlotType {FAR_END}
	 @h2xmle_channel     {1}*/

   int16_t aec_f_vad_hb_flag;
    /**< @h2xmle_description {far end vad flag for > 4KHz} 
	 @h2xmle_group       {LEC_Common}
 	 @h2xmle_subgroup   {Far_end_VAD} 
         @h2xmle_rtmPlotType {FAR_END}
	 @h2xmle_channel     {1}*/

   int16_t aec_f_vad_pf_flag;
    /**< @h2xmle_description {far end vad flag for ec pp} 
	 @h2xmle_group       {LEC_Common}
	 @h2xmle_subgroup   {Far_end_VAD}
         @h2xmle_rtmPlotType {FAR_END}
	 @h2xmle_channel     {1}*/

   int16_t aec_pcd_flag;
    /**< @h2xmle_description {aec path change flag for 0-4KHz } 
	 @h2xmle_group       {LEC_Common}
	 @h2xmle_subgroup   {PCD} 
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {1}*/

   int16_t aec_pcd_hb_flag;
    /**< @h2xmle_description {aec path change flag for > 4KHz } 
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {1}
	 @h2xmle_group       {LEC_Common}
	 @h2xmle_subgroup   {PCD}*/

   int16_t aec_dtd_flag;
    /**< @h2xmle_description {aec double talk flag for 0-4KHz} 
         @h2xmle_group       {LEC_Common}
	 @h2xmle_subgroup   {Double_Talk_Detection}
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {1} */

   int16_t aec_dtd_hb_flag;
    /**< @h2xmle_description {aec double talk flag for > 4KHz} 
         @h2xmle_group       {LEC_Common}
	 @h2xmle_subgroup   {Double_Talk_Detection}
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {1}  */

   int16_t aec_clip_flag;
    /**< @h2xmle_description {aec clipping flag for 0-4KHz} 
         @h2xmle_group       {LEC_Common}
	 @h2xmle_subgroup   {EC_Clipper}
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {1}*/

   int16_t aec_clip_hb_flag;
    /**< @h2xmle_description {aec clipping flag for > 4KHz} 
         @h2xmle_group       {LEC_Common}
	 @h2xmle_subgroup   {EC_Clipper}
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {1} */

   int16_t aec_download_flag[FFECNS_MAX_TX_CHAN];   
    /**< @h2xmle_description {aec download flag for mic1 to mic8} 
         @h2xmle_groupIndex  {0}
		 @h2xmle_group	     {LEC_MIC1}
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {1}
         @h2xmle_subgroup    {Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
		 @h2xmle_group	     {LEC_MIC2}
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {2}
         @h2xmle_subgroup    {Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	     {LEC_MIC3}
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {3}
         @h2xmle_subgroup    {Download_Control}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	     {LEC_MIC4}
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {4}
         @h2xmle_subgroup    {Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	     {LEC_MIC5}
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {5}
         @h2xmle_subgroup    {Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	     {LEC_MIC6}
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {6}
         @h2xmle_subgroup    {Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	     {LEC_MIC7}
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {7}
         @h2xmle_subgroup    {Download_Control}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	     {LEC_MIC8}
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {8}
         @h2xmle_subgroup    {Download_Control}
		 @h2xmle_groupIndexEnd  */

   int32_t aec_erle_dB[FFECNS_MAX_TX_CHAN];    
    /**< @h2xmle_description {linear ec ERLE for mic1 to mic8} 
         @h2xmle_groupIndex  {0}
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {1}
         @h2xmle_subgroup    {Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {1}
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {2}
         @h2xmle_subgroup    {Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {2}
		 @h2xmle_group	     {LEC_MIC3}
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {3}
         @h2xmle_subgroup    {Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd

		 @h2xmle_groupIndex  {3}
		 @h2xmle_group	     {LEC_MIC4}
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {4}
         @h2xmle_subgroup    {Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {4}
		 @h2xmle_group	     {LEC_MIC5}
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {5}
         @h2xmle_subgroup    {Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {5}
		 @h2xmle_group	     {LEC_MIC6}
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {6}
         @h2xmle_subgroup    {Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {6}
		 @h2xmle_group	     {LEC_MIC7}
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {7}
         @h2xmle_subgroup    {Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd
		 
		 @h2xmle_groupIndex  {7}
		 @h2xmle_group	     {LEC_MIC8}
         @h2xmle_rtmPlotType {NEAR_END}
	 @h2xmle_channel     {8}
         @h2xmle_subgroup    {Adaptive_Filter_Paramaters}
		 @h2xmle_groupIndexEnd  */
}ffecns_ec_monitor_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif

/**
    @h2xmlm_module       {"MODULE_ID_FFENCS", FFECNS_MODULE}
    @h2xmlm_displayName  {"FFECNS_MODULE"}
    @h2xmlm_description {FFECNS (Far Field Voice ECNS) module performs optimal echo cancellation 
                         and noise suppression for far feild use cases. 
                         This module supports the following parameter IDs 
                        -    VOICE_PARAM_MOD_ENABLE
                        -    AUDPROC_PARAM_ID_LIB_VERSION
                        -    PARAM_ID_FFECNS_GLOBAL
                        -    PARAM_ID_FFECNS_AEC
                        -    PARAM_ID_FFECNS_ECPP_CLIP
                        -    PARAM_ID_FFECNS_DELAY
                        -    PARAM_ID_FFECNS_SURROUND_EC_CAL
                        -    PARAM_ID_FFECNS_HW_AF
                        -    PARAM_ID_FFECNS_EC_MONITOR
						-    PARAM_ID_FFV_VERSION
                        -    PARAM_ID_FFV_HPF
						-    PARAM_ID_FFV_IVA
						-    PARAM_ID_FFV_SCS
						-    PARAM_ID_FFV_IVA_MONITOR
                        -    PARAM_ID_FFV_BF_COEF					
                        -    PARAM_ID_FFV_BF
                        -    PARAM_ID_FFV_OUT_CONFIG
                        -    PARAM_ID_FFV_DOA          
                        -    PARAM_ID_FFV_SPF_MONITOR
                        -    PARAM_ID_FFV_SPF_FREEZE
                        -    PARAM_ID_FFECNS_EC_CALIBRATION_MODE
						-    PARAM_ID_FFECNS_EC_AF_COEFFS_MIC0
						-    PARAM_ID_FFECNS_EC_AF_COEFFS_MIC1
						-    PARAM_ID_FFECNS_EC_AF_COEFFS_MIC2
						-    PARAM_ID_FFECNS_EC_AF_COEFFS_MIC3
						-    PARAM_ID_FFECNS_EC_AF_COEFFS_MIC4
						-    PARAM_ID_FFECNS_EC_AF_COEFFS_MIC5
						-    PARAM_ID_FFECNS_EC_AF_COEFFS_MIC6
						-    PARAM_ID_FFECNS_EC_AF_COEFFS_MIC7
                         }
    @h2xmlm_ToolPolicy                 {Calibration; RTC}
    @{                   <-- Start of the Module -->

    @h2xml_Select                {voice_param_mod_enable_t}
    @h2xmlm_InsertParameter
    @h2xml_Select       {voice_param_mod_enable_t::Enable}
    @h2xmle_default     {1}

    @h2xml_Select                {audproc_lib_version_t}
    @h2xmlm_InsertParameter

    @h2xml_Select                {ffecns_global_param_t}
    @h2xmlm_InsertParameter
	
	@h2xml_Select                {ffecns_aec_channel_param_t}
	@h2xmlm_InsertParameter

    @h2xml_Select                {ffecns_aec_param_t}
    @h2xmlm_InsertParameter

    @h2xml_Select                {ffecns_ecpp_clip_param_t}
    @h2xmlm_InsertParameter

    @h2xml_Select                {ffecns_delay_param_t}
    @h2xmlm_InsertParameter

    @h2xml_Select                {ffecns_surround_ec_calibration_param_t}
    @h2xmlm_InsertParameter

    @h2xml_Select                {ffecns_hw_af_param_t}
    @h2xmlm_InsertParameter
	
    @h2xml_Select                {ffecns_ec_monitor_param_t}
    @h2xmlm_InsertParameter
	
    @h2xml_Select				 {ffv_lib_version_param_t}
    @h2xmlm_InsertParameter
	
	@h2xml_Select                {ffv_hpf_param_t}
    @h2xmlm_InsertParameter

    @h2xml_Select				 {ffv_iva_param_t}
    @h2xmlm_InsertParameter

    @h2xml_Select				 {ffv_scs_param_t}
    @h2xmlm_InsertParameter

    @h2xml_Select				 {ffv_iva_monitor_param_t}
    @h2xmlm_InsertParameter
	
    @h2xml_Select                {ffv_bf_coef_t}
    @h2xmlm_InsertParameter
		
    @h2xml_Select                {ffv_bf_param_t}
    @h2xmlm_InsertParameter

    @h2xml_Select                {ffv_out_config_param_t}
    @h2xmlm_InsertParameter

    @h2xml_Select                {ffv_doa_param_t}
    @h2xmlm_InsertParameter

    @h2xml_Select                {ffv_spf_monitor_param_t}
    @h2xmlm_InsertParameter

    @h2xml_select                {ffv_spf_freeze_param_t}
    @h2xmlm_InsertParameter 

    @h2xml_Select                {ffecns_ec_calibration_mode_param_t}
    @h2xmlm_InsertParameter
	
    @h2xml_Select                {ffecns_ec_af_coeffs_param_t}
    @h2xmlm_InsertParameter

    @h2xml_Select				 {ffecns_ec_af_coeffs_mic0_param_t}
    @h2xmlm_InsertParameter

	@h2xml_Select				 {ffecns_ec_af_coeffs_mic1_param_t}
	@h2xmlm_InsertParameter

	@h2xml_Select				 {ffecns_ec_af_coeffs_mic2_param_t}
	@h2xmlm_InsertParameter

	@h2xml_Select				 {ffecns_ec_af_coeffs_mic3_param_t}
	@h2xmlm_InsertParameter

	@h2xml_Select				 {ffecns_ec_af_coeffs_mic4_param_t}
	@h2xmlm_InsertParameter

	@h2xml_Select				 {ffecns_ec_af_coeffs_mic5_param_t}
	@h2xmlm_InsertParameter

	@h2xml_Select				 {ffecns_ec_af_coeffs_mic6_param_t}
	@h2xmlm_InsertParameter

	@h2xml_Select				 {ffecns_ec_af_coeffs_mic7_param_t}
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


#endif // #ifndef __FFECNS_CALIBRATION_API_H__
