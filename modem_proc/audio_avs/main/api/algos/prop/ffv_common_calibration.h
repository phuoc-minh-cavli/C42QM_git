/*============================================================================
* Copyright (c) 2018 Qualcomm Technologies, Inc.                             *
* All Rights Reserved.                                                       *
* Confidential and Proprietary - Qualcomm Technologies, Inc.                 *
* ===========================================================================*/
/* ===========================================================================
* Defines common interface between ffencs and ffns modules                   *
* ===========================================================================*/
#ifndef __H2XML__
#include <stdint.h>
#else
#include "mmdefs.h"
#endif

#ifndef __FFV_COMMON_CALIBRATION_H__
#define __FFV_COMMON_CALIBRATION_H__

#define FFV_RELEASE_VERSION_MSB    (0x03010101)
#define FFV_RELEASE_VERSION_LSB    (0x00000000)

#define FFV_BIQUAD_MAX_NUM_SECTIONS (4)
#define FFV_BIQUAD_NCOEFF_DEN   (2)
#define FFV_BIQUAD_NCOEFF_NUM   (3)

#define FFV_NFFT_8KHZ      (256)  /* FFT/IFFT size for 8kHZ processing */   
#define FFV_NFFT_8KHZ_LLC  (64)  /* LLC FFT/IFFT size for 8kHZ processing */   
#define FFV_NFREQS_WB      (FFV_NFFT_8KHZ+1) /* # of frequency bins for normal filterbank 16kHz*/
#define FFV_NFREQS_WB_LLC  (FFV_NFFT_8KHZ_LLC+1) /* # of frequency bins for LLC filterbank 16kHz*/ 
#define FFV_MAX_TX_CHAN  (8)                                         // maximum input TX channels
#define FFV_MAX_NUM_BEAMFORMER_CHAN (9) // Number of fixed beamformers or beamformer output channels. 
#define FFV_MAX_RX_CHAN  (6)                                         // maximum input RX channels
#define FFV_MIN_TX_CHAN  (1)                                         // minimum input TX channels
#define FFV_MIN_RX_CHAN  (0)                                         // minimum input RX channels
#define FFV_MIN_OUT_CHAN (1)                                         // minimum output channels
#define FFV_LEC_OUT_CHAN (1)                                         // # of linear echo canceller output channel
#define FFV_RSV_OUT_CHAN (1)                                         // # of reserved output channel 
#define FFV_MAX_OUT_CHAN (FFV_MAX_NUM_BEAMFORMER_CHAN+FFV_LEC_OUT_CHAN + FFV_RSV_OUT_CHAN)   // maximum output channels      

typedef enum MODULE_IDS
{
	FFECNS_MODULE		= 0x00010952,
	FFNS_MODULE			= 0x00010962
}MODULE_IDS;

/*Macro definitions for Common interface*/
typedef enum FFV_STRUCT_IDS
{
   FFV_HPF_PARAM     = (0x00010957),
   FFV_BF_PARAM      = (0x00010958),
   FFV_BF_COEF       = (0x00010959),
   FFV_DOA_PARAM     = (0x0001095A),
   FFV_SPF_MONITOR   = (0x0001095E),             // Refers to `ffv_spf_monitor_param_t`. (Only `get`: provides the corresponding parameter for monitoring.)
   FFV_VERSION_PARAM = (0x00010937),
   FFV_SPF_FREEZE    = (0x00010960),             // Refers to freezing the beamformer output channels; this parameter is not exposed.
   FFV_IVA_PARAM     = (0x0001096E),
   FFV_SCS_PARAM     = (0x0001096F),
   FFV_IVA_MONITOR   = (0x00010970),
   FFV_OUT_CONFIG_PARAM  = (0x00010964),
}FFV_STRUCT_IDS;

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

/* ffv_out_config_param_t - Structure for the output number of channels and channel map configuration
   This structure defines the parameters associated with FFECNS_OUT_CONFIG_PARAM in enum FFECNS_STRUCT_IDS, which is in ffecns_api.h
   struct_size:      [IN/OUT]: structure size
                     [IN]: caller fills size for the size of the payload structure in apis set(...) & get(...). expects size >= sizeof(ffv_out_config_param_t)
                     [OUT]: callee fills the appropriate size of the buffer filled in api get(...),sizeof(ffv_out_config_param_t)
   */

/** @h2xmlp_parameter    {"PARAM_ID_FFV_OUT_CONFIG", FFV_OUT_CONFIG_PARAM}
    @h2xmlp_description  { Structure for the configuration FFV output(s)\n} */

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffv_out_config_param_t
{
   int16_t num_out_chan;
   /**< @h2xmle_description  { Number of output channels }
         @h2xmle_default     {9}
         @h2xmle_range       {1..9}
         @h2xmle_policy      {Basic} */

   int16_t out_ch_map[FFV_MAX_NUM_BEAMFORMER_CHAN];
   /**< @h2xmle_description  {output channel mapping}
         @h2xmle_defaultList {1,2,3,4,5,6,7,8,9}
         @h2xmle_range       {1..9}
         @h2xmle_policy      {Advanced} */

} ffv_out_config_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif


/* ffv_bf_param_t - Structure for the configuration of the spatial filter(s)
	This structure defines the parameters associated with FFECNS_BF_PARAM in enum FFECNS_STRUCT_IDS, which is in ffecns_api.h
struct_size:			[IN/OUT]: structure size
							[IN]: caller fills size for the size of the payload structure in apis set(...) & get(...). expects size >= sizeof(ffv_bf_param_t)
						   [OUT]: callee fills the appropriate size of the buffer filled in api get(...),sizeof(ffv_bf_param_t)
   */
/** @h2xmlp_parameter    {"PARAM_ID_FFV_BF", FFV_BF_PARAM}
    @h2xmlp_description  { Structure for the configuration of the BEAM FORMER(s)\n} */

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffv_bf_param_t 
{
	int16_t doa_buffer;
    /**< @h2xmle_description {Size of the DOA estimator's history buffer} 
         @h2xmle_default     {100} 
		 @h2xmle_subgroup    {Beamforming}
		 @h2xmle_group       {SPF}
         @h2xmle_range       {0..32767} 
         @h2xmle_policy      {Advanced} */

	int16_t filter_delay;
    /**< @h2xmle_description {Upper limit for all values in maxDoahist2. It is the max number of times a certain channel featured the highest energy in a given time interval } 
         @h2xmle_default     {100} 
		 @h2xmle_subgroup    {Beamforming}
		 @h2xmle_group       {SPF}
         @h2xmle_range       {0..32767} 
         @h2xmle_policy      {Advanced} */

    int16_t num_bf_out_chan;
    /**< @h2xmle_description { Number of BEAM FORMERS} 
         @h2xmle_default     {9} 
		 @h2xmle_subgroup    {Beamforming}
		 @h2xmle_group       {SPF}
         @h2xmle_range       {1..9} 
         @h2xmle_policy      {Basic} */

    int16_t reserved;
    /**< @h2xmle_description {Added for alignment. Client must set this field to zero.}
         @h2xmle_default     {0x00}
         @h2xmle_range       {0x0000..0x7FFF}
		 @h2xmle_visibility  {hide}
         @h2xmle_readOnly    {true} */

    int32_t direct_threshold;
    /**< @h2xmle_description {Threshold (energy) that needs to be exceeded in order to update the DOA scores (i.e., the DOA's circular buffer)} 
         @h2xmle_default     {69412} 
		 @h2xmle_subgroup    {Beamforming}
		 @h2xmle_group       {SPF}
         @h2xmle_range       {0..2147483647} 
         @h2xmle_policy      {Advanced} */

} ffv_bf_param_t; 

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif

/* ffv_bf_coef_t - Structure for the coefficients of the spatial filter(s). 
	This structure defines the parameters associated with FFECNS_BF_COEF in enum FFECNS_STRUCT_IDS, which is in ffecns_api.h
   */

/** @h2xmlp_parameter    {"PARAM_ID_FFV_BF_COEF", FFV_BF_COEF}
    @h2xmlp_description  { This structure is used for the setting up the beam former coeff\n} */

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffv_bf_coef_t 
{
	int32_t bf_coeff_size;
    /**< @h2xmle_description {Total number of beamformer coefficients for all beamformers, microphones, and frequency indices. (Note: 514 = 21257; 18504 = 89257; [#mics * #bfs * #bins])} 
         @h2xmle_default     {0} 
		 @h2xmle_subgroup    {Beamforming}
		 @h2xmle_group       {SPF}
         @h2xmle_range       {0..2147483647} 
         @h2xmle_policy      {Advanced} */

   int16_t  reserved[2];   
    /**< @h2xmle_description {Added for alignment. Client must set this field to zero.} 
         @h2xmle_default     {0} 
         @h2xmle_range       {0x0000..0x7FFF}
		 @h2xmle_visibility  {hide}
         @h2xmle_readOnly    {true} */

	int64_t bf_coeff[0];
    /**< @h2xmle_description            {Array of beamformer coefficients.} 
         @h2xmle_variableArraySize      {bf_coeff_size}
         @h2xmle_default                {0} 
		 @h2xmle_subgroup    {Beamforming}
		 @h2xmle_group       {SPF}
         @h2xmle_policy                 {Advanced} */

} ffv_bf_coef_t; 

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif

/* ffv_hpf_param_t - Structure for the configuration of the high-pass filter
	This structure defines the parameters associated with FFECNS_HPF_PARAM in enum FFECNS_STRUCT_IDS, which is in ffecns_api.h
struct_size:			[IN/OUT]: structure size
							[IN]: caller fills size for the size of the payload structure in apis set(...) & get(...). expects size >= sizeof(ffv_hpf_param_t)
						   [OUT]: callee fills the appropriate size of the buffer filled in api get(...),sizeof(ffv_hpf_param_t)
   */

/** @h2xmlp_parameter    {"PARAM_ID_FFV_HPF", FFV_HPF_PARAM}
    @h2xmlp_description  { Structure for the configuration of the high-pass filter\n} */

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffv_hpf_param_t 
{
	int16_t num_sections; // Number of biquad sections
    /**< @h2xmle_description { Number of biquad filters/sections} 
         @h2xmle_default     {2} 
		 @h2xmle_group       {HPF}
         @h2xmle_range       {1..4} 
         @h2xmle_policy      {Advanced} */

    int16_t reserved;
    /**< @h2xmle_description {Added for alignment. Client must set this field to zero.}
         @h2xmle_default     {0x00}
         @h2xmle_range       {0x0000..0x7FFF}
		 @h2xmle_visibility  {hide}
         @h2xmle_readOnly    {true} */

	int16_t num_q[FFV_BIQUAD_MAX_NUM_SECTIONS]; // Q factor (IWL) for numerator coeffs
    /**< @h2xmle_description { Q factor (IWL) for the numerator coefficients for biquad filters/sections 1 to 4} 
         @h2xmle_defaultList {1, 1, 0, 0} 
		 @h2xmle_group       {HPF}
         @h2xmle_range       {0x8000..0x7FFF} 
         @h2xmle_policy      {Advanced} */

	int16_t den_q[FFV_BIQUAD_MAX_NUM_SECTIONS]; // Q factor (IWL) for denominator coeffs
    /**< @h2xmle_description { q factor for the denominator coefficients, section 1 to section 4} 
         @h2xmle_defaultList {1, 1, 0, 0}  
		 @h2xmle_group       {HPF}
         @h2xmle_range       {0x8000..0x7FFF}
         @h2xmle_policy      {Advanced} */

	int32_t num_coeff[FFV_BIQUAD_MAX_NUM_SECTIONS][FFV_BIQUAD_NCOEFF_NUM]; // Numerator coefficients, three for each section. 
    /**< @h2xmle_description { numerator coefficient b0, b1, b2 for section 1, section 2, section 3 and section 4} 
         @h2xmle_defaultList {0x40000000, 0x80000000, 0x40000000, 0x3D491AC8, 0x856DCA6F, 0x3D491AC8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0} 
         @h2xmle_range       {0x80000000..0x7FFFFFFF} 
		 @h2xmle_group       {HPF}
         @h2xmle_policy      {Advanced} */

	int32_t den_coeff[FFV_BIQUAD_MAX_NUM_SECTIONS][FFV_BIQUAD_NCOEFF_DEN]; // Denominator coefficients, two for each section.
    /**< @h2xmle_description {denominator coefficient a0, a1, a2 for section 1, section 2 , section 3 and section 4} 
         @h2xmle_defaultList {0x83DF9596, 0x3C31E721, 0x81AC6E99, 0x3E655D75, 0x0, 0x0, 0x0, 0x0} 
         @h2xmle_range       {0x80000000..0x7FFFFFFF} 
		 @h2xmle_group       {HPF}
         @h2xmle_policy      {Advanced} */

}ffv_hpf_param_t; 

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif
/* ffv_doa_param_t - Structure for the configuration of the high-pass filter.
	This structure defines the parameters associated with FFECNS_BF_PARAM in enum FFECNS_STRUCT_IDS, which is in ffecns_api.h
struct_size:			[IN/OUT]: structure size
							[IN]: caller fills size for the size of the payload structure in apis set(...) & get(...). expects size >= sizeof(ffv_doa_param_t)
						   [OUT]: callee fills the appropriate size of the buffer filled in api get(...),sizeof(ffv_doa_param_t)
   */

/** @h2xmlp_parameter    {"PARAM_ID_FFV_DOA", FFV_DOA_PARAM}
    @h2xmlp_description  { Structure for the configuration of the DOA.\n} */

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffv_doa_param_t 
{
	int16_t sumbf_mag_freq_low;
    /**< @h2xmle_description {Lower bound of Frequency} 
         @h2xmle_default     {10} 
		 @h2xmle_group       {SPF}
		 @h2xmle_subgroup    {DOA}
         @h2xmle_range       {0..256} 
         @h2xmle_policy      {Basic} */

	int16_t sumbf_mag_freq_high;
    /**< @h2xmle_description {Lower bound of frequency indices whose corresponding magnitudes are summed in order to determine the acoustic source's angle} 
         @h2xmle_default     {138} 
		 @h2xmle_group       {SPF}
		 @h2xmle_subgroup    {DOA}
         @h2xmle_range       {0..256} 
         @h2xmle_policy      {Basic} */

    int16_t is_doa_score_based_sorting;
    /**< @h2xmle_description {Sort output channels according to the top-N DOA scores if set to one.} 
         @h2xmle_default     {1} 
		 @h2xmle_group       {SPF}
		 @h2xmle_subgroup    {DOA}
         @h2xmle_range       {0..1} 
         @h2xmle_policy      {Basic} */

    int16_t reserved;
    /**< @h2xmle_description {Added for alignment. Client must set this field to zero.}
         @h2xmle_default     {0x00}
         @h2xmle_range       {0x0000..0x7FFF}
		 @h2xmle_visibility  {hide}
         @h2xmle_readOnly    {true} */

} ffv_doa_param_t; 

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif

// Structure encapsulating the parameters that need to be monitored.

/** @h2xmlp_parameter    {"PARAM_ID_FFV_SPF_MONITOR", FFV_SPF_MONITOR}
    @h2xmlp_description  { Structure encapsulating the parameters that need to be monitored.\n} 
	@h2xmlp_toolPolicy  {RTM}
	@h2xmlp_readOnly    {true} */

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffv_spf_monitor_param_t 
{
	int16_t is_frozen_output_channels; 
    /**< @h2xmle_description {Freeze top-N output channels if set to one; update them according to the top-N DOA score-based channel selector if zero.} 
         @h2xmle_default     {0} 
         @h2xmle_range       {0..1} 
         @h2xmle_policy      {Basic}
		 @h2xmle_rtmPlotType {RTM_PLOT}
		 @h2xmle_group        {SPF}
	 @h2xmle_subgroup     {DOA} */

	int16_t doa_indices[FFV_MAX_NUM_BEAMFORMER_CHAN];
    /**< @h2xmle_description {1st to 9th element of the sorted list of channel indices that feature the top-N DOA scores.} 
         @h2xmle_default     {0} 
         @h2xmle_range       {0..8} 
         @h2xmle_policy      {Advanced} 
		 @h2xmle_rtmPlotType {RTM_PLOT}
		 @h2xmle_group        {SPF}
	 @h2xmle_subgroup     {DOA}*/

	int32_t doa_scores[FFV_MAX_NUM_BEAMFORMER_CHAN];
	/**< @h2xmle_description {1stto 9th element of the sorted list of DOA scores that correspond to the sorted list of channel indices.} 
         @h2xmle_default     {0} 
         @h2xmle_range       {0..2147483647} 
         @h2xmle_policy      {Advanced} 
	 @h2xmle_rtmPlotType {RTM_PLOT}	
		 @h2xmle_group        {SPF}
	 @h2xmle_subgroup     {DOA}*/

} ffv_spf_monitor_param_t; 

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif

/* ffv_iva_param_t - Structure for the configuration of the independent vector analysis FFNS algorithm
This structure defines the parameters associated with FFNS_IVA_PARAM in enum FFV_STRUCT_IDS, which is in ffns_calibration_api.h
struct_size:			[IN/OUT]: structure size
[IN]: caller fills size for the size of the payload structure in apis set(...) & get(...). expects size >= sizeof(ffv_iva_param_t)
[OUT]: callee fills the appropriate size of the buffer filled in api get(...),sizeof(ffv_iva_param_t)
*/
/** @h2xmlp_parameter    {"PARAM_ID_FFV_IVA", FFV_IVA_PARAM}
@h2xmlp_description  { Structure for the configuration of the independent vector analysis FFNS algorithm\n} */

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffv_iva_param_t
{
	int16_t number_sources;
	/**< @h2xmle_description {number of acoustic sources to be separated (limited by # of mics)}
	@h2xmle_default     {2}
	@h2xmle_range       {0x1..0x2}
	@h2xmle_policy      {Advanced} */

	int16_t number_iterations;
	/**< @h2xmle_description {number of iterations of IVA learning per frame}
	@h2xmle_default     {1}
	@h2xmle_range       {0x1..0xA}
	@h2xmle_policy      {Advanced} */

	int16_t number_frames_buffered;
	/**< @h2xmle_description {number of frame input history saved for IVA learning}
	@h2xmle_default     {3}
	@h2xmle_range       {0x1..0x32}
	@h2xmle_policy      {Advanced} */

	int16_t update_interval;
	/**< @h2xmle_description {number of frames to employ one IVA learning}
	@h2xmle_default     {1}
	@h2xmle_range       {0x1..0x32}
	@h2xmle_policy      {Advanced} */

	int16_t cov_mat_16bit;
	/**< @h2xmle_description {flag to enable 16 bit covariance matrix storing}
	@h2xmle_default     {0}
	@h2xmle_range       {0x0..0x1}
	@h2xmle_policy      {Advanced} */

	int16_t cov_mat_norm;
	/**< @h2xmle_description {flag to enable normalization of covariance matrix}
	@h2xmle_default     {0}
	@h2xmle_range       {0x0..0x1}
	@h2xmle_policy      {Advanced} */

	int16_t sb_division_factor;
	/**< @h2xmle_description {number of partitions to total number of subbands, each partition will be processed per dsfact frame}
	@h2xmle_default     {1}
	@h2xmle_range       {0x1..0x8}
	@h2xmle_policy      {Advanced} */

	int16_t reserved;
	/**< @h2xmle_description {Added for alignment. Client must set this field to zero.}
	@h2xmle_default     {0x00}
	@h2xmle_range       {0x0000..0x7FFF}
	@h2xmle_visibility  {hide}
	@h2xmle_readOnly    {true} */

	int32_t forgetting_factor_L32Q31;
	/**< @h2xmle_description {smoothing factor for covariance matrix update}
	@h2xmle_default     {0x770a3d71}
	@h2xmle_range       {0x0..0x7FFFFFFF}
	@h2xmle_policy      {Advanced} */

	int32_t shape_factor_L32Q30;
	/**< @h2xmle_description {shape parameter for the generalized Gaussian source model}
	@h2xmle_default     {0x20000000}
	@h2xmle_range       {0x0..0x7FFFFFFF}
	@h2xmle_policy      {Advanced} */

} ffv_iva_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif

/* ffv_scs_param_t - Structure for the configuration of the output channel switching logic
This structure defines the parameters associated with FFNS_SCS_PARAM in enum FFV_STRUCT_IDS, which is in ffns_calibration_api.h
struct_size:			[IN/OUT]: structure size
[IN]: caller fills size for the size of the payload structure in apis set(...) & get(...). expects size >= sizeof(ffv_scs_param_t)
[OUT]: callee fills the appropriate size of the buffer filled in api get(...),sizeof(ffv_scs_param_t)
*/
/** @h2xmlp_parameter    {"PARAM_ID_SCS_IVA", FFV_SCS_PARAM}
@h2xmlp_description  { Structure for the configuration of the independent vector analysis FFNS algorithm\n} */

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffv_scs_param_t
{
	int16_t enable;
	/**< @h2xmle_description {source channel selection enable/disable }
	@h2xmle_default     {0}
	@h2xmle_range       {0x0..0x1}
	@h2xmle_policy      {basic} */

	int16_t hangover_fr;
	/**< @h2xmle_description {number of frames before the first channel selection is made}
	@h2xmle_default     {0x32}
	@h2xmle_range       {0x0..0x7FFF}
	@h2xmle_policy      {Advanced} */

	int16_t hangover_fr_switch;
	/**< @h2xmle_description {number of frames before following channel switch}
	@h2xmle_default     {0x64}
	@h2xmle_range       {0x0..0x7FFF}
	@h2xmle_policy      {Advanced} */

	int16_t noise_est_init_fr;
	/**< @h2xmle_description {number of frame for noise estimate initialization}
	@h2xmle_default     {0x64}
	@h2xmle_range       {0x8000..0x7FFF}
	@h2xmle_policy      {Advanced} */

	int16_t alpha_st_L16Q15;
	/**< @h2xmle_description {smoothing factor for short-term estimate}
	@h2xmle_default     {0x6666}
	@h2xmle_range       {0x8000..0x7FFF}
	@h2xmle_policy      {Advanced} */

	int16_t alpha_lt_L16Q15;
	/**< @h2xmle_description {smoothing factor for long-term estimate}
	@h2xmle_default     {0x7EB8}
	@h2xmle_range       {0x8000..0x7FFF}
	@h2xmle_policy      {Advanced} */

	int16_t noise_est_snr_max_lshift;
	/**< @h2xmle_description {max snr (in terms of left shift) for noise estimate }
	@h2xmle_default     {4}
	@h2xmle_range       {0x8000..0x7FFF}
	@h2xmle_policy      {Advanced} */

	int16_t channel_switch_snr_min_lshift;
	/**< @h2xmle_description {min source channel snr (in terms of left shift) for channel switch}
	@h2xmle_default     {0x03}
	@h2xmle_range       {0x8000..0x7FFF}
	@h2xmle_policy      {Advanced} */

	int16_t channel_switch_snrDff_min_lshift;
	/**< @h2xmle_description {min snr difference (in terms of left shift) for channel switch}
	@h2xmle_default     {1}
	@h2xmle_range       {0x8000..0x7FFF}
	@h2xmle_policy      {Advanced} */

	int16_t reserved;
	/**< @h2xmle_description {Added for alignment. Client must set this field to zero.}
	@h2xmle_default     {0x00}
	@h2xmle_range       {0x0000..0x7FFF}
	@h2xmle_visibility  {hide}
	@h2xmle_readOnly    {true} */

	int32_t channel_switch_energy_min;
	/**< @h2xmle_description {min source channel energy for channel switch}
	@h2xmle_default     {0x68E}
	@h2xmle_range       {0x0..0x7FFFFFFF}
	@h2xmle_policy      {Advanced} */

} ffv_scs_param_t;


#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif


// Structure encapsulating the parameters that need to be monitored.

/** @h2xmlp_parameter    {"PARAM_ID_FFV_IVA_MONITOR", FFV_IVA_MONITOR}
@h2xmlp_description  { Structure encapsulating the parameters that need to be monitored.\n}
@h2xmlp_toolPolicy  {RTM}
@h2xmlp_readOnly    {true} */

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffv_iva_monitor_param_t
{
	int16_t source_channel;
	/**< @h2xmle_description {channel selected by channel switching logic}
	@h2xmle_default     {0}
	@h2xmle_range       {0x0..0x1}
	@h2xmle_policy      {Advanced} */

	int16_t reserved;
	/**< @h2xmle_description {Added for alignment. Client must set this field to zero.}
	@h2xmle_default     {0x00}
	@h2xmle_range       {0x0000..0x7FFF}
	@h2xmle_visibility  {hide}
	@h2xmle_readOnly    {true} */

	int32_t energy_lt[2];
	/**< @h2xmle_description {long term smoothed energy for all sources}
	@h2xmle_default     {0}
	@h2xmle_range       {0x0..0x7FFFFFFF}
	@h2xmle_policy      {Advanced} */

	int32_t energy_st[2];
	/**< @h2xmle_description {to enable output channel switching logic}
	@h2xmle_default     {0}
	@h2xmle_range       {0x0..0x7FFFFFFF}
	@h2xmle_policy      {Advanced} */

} ffv_iva_monitor_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif


/* ffv_lib_version_param_t
	This structure defines the parameters associated with FFECNS_VERSION_PARAM in enum FFECNS_STRUCT_IDS, which is in ffecns_api.h
struct_size:			[IN/OUT]: structure size
							[IN]: caller fills size for the size of the payload structure in apis set(...) & get(...). expects size >= sizeof(ffecns_lib_version_param_t)
						   [OUT]: callee fills the appropriate size of the buffer filled in api get(...),sizeof(ffecns_lib_version_param_t)
   */

/** @h2xmlp_parameter    {"PARAM_ID_FFV_VERSION", FFV_VERSION_PARAM}
    @h2xmlp_description  { Common structure across modules to get the current library version.\n} 
	@h2xmlp_toolPolicy   {RTC_READONLY} */

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffv_lib_version_param_t
{
    uint32_t lib_version_low;
	/**< @h2xmle_description {Lower 32 bits of the 64-bit library version number} */

    uint32_t lib_version_high;
	/**< @h2xmle_description {Higher 32 bits of the 64-bit library version number} */
	
} ffv_lib_version_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif


/* ffv_common_calibration
   This structure defines the parameters associated with FFV_PARAM_FREEZE in enum FFNS/FFECNS_STRUCT_IDS, which is in ffns/ffecns_api.h
*/

/** @h2xmlp_parameter    {"PARAM_ID_FFV_SPF_FREEZE", FFV_SPF_FREEZE}
    @h2xmlp_description  { Indicate if DOA re-ordering should be freezed or unfreezed.\n}
    @h2xmlp_toolPolicy   {NO_SUPPORT} */

#ifdef Q6_PADDING_CHECK
#include "q6_begin_pad_check.h"
#endif

typedef struct ffv_spf_freeze_param_t
{
    uint16_t freeze;
    /**< @h2xmle_description {Indicate if DOA re-ordering should be freezed or unfreezed}
         @h2xmle_default     {0}
         @h2xmle_range       {0..1}
         @h2xmle_policy      {Advanced} */

   int16_t  reserved;   
    /**< @h2xmle_description {Added for alignment. Client must set this field to zero.} 
         @h2xmle_default     {0} 
         @h2xmle_range       {0x0000..0x7FFF}
         @h2xmle_visibility  {hide}
         @h2xmle_readOnly    {true} */

    uint32_t source_id;
    /**< @h2xmle_description {Identifies the SVA source informed the freeze event}
         @h2xmle_default     {0}
         @h2xmle_range       {0x0..0xFFFFFFFF}
         @h2xmle_policy      {Advanced} */

} ffv_spf_freeze_param_t;

#ifdef Q6_PADDING_CHECK
#include "q6_end_pad_check.h"
#endif

#endif //#ifndef __FFV_COMMON_CALIBRATION_H__
