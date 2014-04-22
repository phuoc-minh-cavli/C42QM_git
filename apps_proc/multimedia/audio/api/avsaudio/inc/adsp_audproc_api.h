/*========================================================================*/
/**
@file adsp_audproc_api.h

This file contains the structure definitions for Audio Post Processing 
Topology IDs, Module IDs, Parameter IDs. 
*/

/*===========================================================================
NOTE: The description above does not appear in the PDF. 

      The audio_mainpage.dox file contains all file/group descriptions that are 
      in the output PDF generated using Doxygen and Latex. To edit or update 
      any of the file/group text in the PDF, edit the audio_mainpage.dox file 
      contact Tech Pubs.
===========================================================================*/

/*===========================================================================
Copyright (c) 2012 Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
======================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/audio.tx/2.0/audio/api/avsaudio/inc/adsp_audproc_api.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/08/12   sw      (Tech Pubs) Updated Doxygen comments based on 06.110 branch.
05/30/11   sw      (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
04/22/11    ss     Introducing Audproc API file that would hold all post processing 
                   Module IDs, Parameter IDs and Topology IDs

========================================================================== */

#ifndef _ADSP_AUDPROC_API_H_
#define _ADSP_AUDPROC_API_H_

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @addtogroup audio_pp_topos
@{ */

/** @xreflabel{hdr:AsmStrmPostProcTopoDefault}
    ID of the default audio postprocessing stream topology.
    
    @inputfigure{1,ASM_STREAM_POSTPROC_TOPO_ID_DEFAULT_topology.png}
*/
#define ASM_STREAM_POSTPROC_TOPO_ID_DEFAULT                         0x00010BE4

/** @xreflabel{hdr:AsmStrmPostProcTopoPeakmeter}
    ID of the default postprocessing topology with peak meter.

    @inputfigure{1,ASM_STREAM_POSTPROC_TOPO_ID_PEAKMETER_topology.png}
*/      
#define ASM_STREAM_POSTPROC_TOPO_ID_PEAKMETER                       0x00010D83

/** @xreflabel{hdr:AsmStrmPostProcTopoNone}
    ID of the no-default audio postprocessing topology. The resampler
    is the only module present in this topology.

    @inputfigure{1,ASM_STREAM_POSTPROC_TOPO_ID_NONE_topology.png}
    @newpage
*/
#define ASM_STREAM_POSTPROC_TOPO_ID_NONE                            0x00010C68

/** @xreflabel{hdr:AsmStrmPostProcTopoMchPeakVol}
    ID of the audo postprocessing topology with multichannel resampler, peak
    meter, and volume control.

    @inputfigure{1,ASM_STREAM_POSTPROC_TOPO_ID_MCH_PEAK_VOL_topology.png}

    Resampler:
    - Peak Meter                        (#AUDPROC_MODULE_ID_PEAK_METER)
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
*/
#define ASM_STREAM_POSTPROC_TOPO_ID_MCH_PEAK_VOL                        0x00010D8B

/** @xreflabel{hdr:AsmStrmPreProcTopoDef}
    ID of the default audio preprocessing stream topology.
    
    @inputfigure{1,ASM_STREAM_PREPROC_TOPO_ID_DEFAULT_and_NONE_topology.png}
*/
#define ASM_STREAM_PREPROC_TOPO_ID_DEFAULT   ASM_STREAM_POSTPROC_TOPO_ID_DEFAULT

/** @xreflabel{hdr:AsmStrmPreProcTopoNone}
    ID of the no-default audio preprocessing topology. The resampler is
    the only module present in this topology.

    @inputfigure{1,ASM_STREAM_PREPROC_TOPO_ID_DEFAULT_and_NONE_topology.png}
*/
#define ASM_STREAM_PREPROC_TOPO_ID_NONE      ASM_STREAM_POSTPROC_TOPO_ID_NONE


/** @xreflabel{hdr:AudprocPoppTopologyIdDtsAudio}
    ID of the DTS audio post processing topology.

    @inputfigure{1,AUDPROC_POPP_TOPOLOGY_ID_DTS_AUDIO.png}
*/
#define AUDPROC_POPP_TOPOLOGY_ID_DTS_AUDIO   0x0001071F

/** @xreflabel{hdr:AdmCmdCoppOpenTopoIdNoneAudioCopp}
    ID of the no-default COPP audio postprocessing device topology
    (indicates None). 

    @inputfigure{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_NONE_AUDIO_COPP_topology.png}
    @newpage
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_NONE_AUDIO_COPP              0x00010312

/** @xreflabel{hdr:AdmCmdCoppOpenTopoIdSpkrMonoAudioCopp}
    ID of SPEAKER_MONO in the COPP audio postprocessing device topology.

@inputfigure{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_MONO_AUDIO_COPP_topology.png}

    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Psychoacoustic Bass Enhancement   (#AUDPROC_MODULE_ID_PBE)
    - IIR Tuning Filter                 (#AUDPROC_MODULE_ID_IIR_TUNING_FILTER)
    - Multiband Dynamic Range Control   (#AUDPROC_MODULE_ID_MBDRC)
    - Spectrum Analyzer                 (#AUDPROC_MODULE_ID_SPA)
    - Rx Codec Gain                     (#AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_MONO_AUDIO_COPP           0x00010313

/** @xreflabel{hdr:AdmCmdCoppOpenTopoIdSpkrStereoAudioCopp}
    ID of SPEAKER_STEREO in the COPP audio postprocessing device topology.

@inputfigure{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_AUDIO_COPP_topology.png}

    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Equalizer                         (#AUDPROC_MODULE_ID_EQUALIZER)
    - QConcertPlus&tm;&nbsp;            (#AUDPROC_MODULE_ID_QCONCERT_PLUS_REVERB)
    - Psychoacoustic Bass Enhancement   (#AUDPROC_MODULE_ID_PBE)
    - IIR Tuning Filter                 (#AUDPROC_MODULE_ID_IIR_TUNING_FILTER)
    - Multiband Dynamic Range Control   (#AUDPROC_MODULE_ID_MBDRC)
    - QEnsemble&tm;&nbsp;               (#AUDPROC_MODULE_ID_QENSEMBLE)
    - Spectrum Analyzer                 (#AUDPROC_MODULE_ID_SPA)
    - Rx Codec Gain                     (#AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
    @newpage
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_AUDIO_COPP         0x00010314

/** @xreflabel{hdr:AdmCmdCoppOpenTopoIdSpkrStereoIirAudioCopp}
    ID of SPEAKER_STEREO_IIR ID in the COPP audio postprocessing device
    topology.

@inputfigure{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_IIR_AUDIO_COPP_topology.png}

    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Equalizer                         (#AUDPROC_MODULE_ID_EQUALIZER)
    - QconcertPlus                      (#AUDPROC_MODULE_ID_QCONCERT_PLUS_REVERB)
    - Psychoacoustic Bass Enhancement   (#AUDPROC_MODULE_ID_PBE)
    - IIR Tuning Filter Left            (#AUDPROC_MODULE_ID_LEFT_IIR_TUNING_FILTER)
    - IIR Tuning Filter Right           (#AUDPROC_MODULE_ID_RIGHT_IIR_TUNING_FILTER)
    - Multiband Dynamic Range Control   (#AUDPROC_MODULE_ID_MBDRC)
    - QEnsemble                         (#AUDPROC_MODULE_ID_QENSEMBLE)
    - Spectrum Analyzer                 (#AUDPROC_MODULE_ID_SPA)
    - Rx Codec Gain                     (#AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_IIR_AUDIO_COPP         0x00010704

/** @xreflabel{hdr:AdmCoppTopoIdSpkrMonoMbdrcV2}
    ID of the COPP topology of SPEAKER_MONO for MBDRC Version 2.

@inputfigure{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_MONO_AUDIO_COPP_MBDRCV2_topology.png}

    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Psychoacoustic Bass Enhancement   (#AUDPROC_MODULE_ID_PBE)
    - IIR Tuning filter                 (#AUDPROC_MODULE_ID_IIR_TUNING_FILTER)
    - Dynamic Range Control             (#AUDPROC_MODULE_ID_MBDRCV2)
    - Spectrum Analyzer                 (#AUDPROC_MODULE_ID_SPA)
    - RX Codec Gain                     (#AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
    @newpage
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_MONO_AUDIO_COPP_MBDRCV2 0x0001070D

/** @xreflabel{hdr:AdmCoppTopoIdSpkrStereoMbdrcV2}
    IID of the COPP topology of SPEAKER_STEREO for MBDRC Version 2.

@inputfigure{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_AUDIO_COPP_MBDRCV2_topology.png}

    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Equalizer                         (#AUDPROC_MODULE_ID_EQUALIZER)
    - QCconcertPlus                     (#AUDPROC_MODULE_ID_QCONCERT_PLUS_REVERB)
    - Psychoacoustic Bass Enhancement   (#AUDPROC_MODULE_ID_PBE)
    - IIR Tuning filter                 (#AUDPROC_MODULE_ID_IIR_TUNING_FILTER)
    - Multi-band Dynamic Range Control  (#AUDPROC_MODULE_ID_MBDRCV2)
    - QEnsemble                         (#AUDPROC_MODULE_ID_QENSEMBLE)
    - Spectrum Analyzer                 (#AUDPROC_MODULE_ID_SPA)
    - RX Codec Gain                     (#AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
    @newpage
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_AUDIO_COPP_MBDRCV2 0x0001070E

/** @xreflabel{hdr:AdmCoppTopoIdSpkrStereoIirAudioCoppMbdrcv2}
    ID of the COPP topology of STEREO_IIR for MBDRC Version 2.

@inputfigure{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_IIR_AUDIO_COPP_MBDRCV2_topology.png}
    
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Equalizer                         (#AUDPROC_MODULE_ID_EQUALIZER)
    - QCconcertPlus                     (#AUDPROC_MODULE_ID_QCONCERT_PLUS_REVERB)
    - Psychoacoustic Bass Enhancement   (#AUDPROC_MODULE_ID_PBE)
    - IIR Tuning filter left            (#AUDPROC_MODULE_ID_LEFT_IIR_TUNING_FILTER)
    - IIR Tuning filter right           (#AUDPROC_MODULE_ID_RIGHT_IIR_TUNING_FILTER)
    - Multi-band Dynamic Range Control  (#AUDPROC_MODULE_ID_MBDRCV2)
    - QEnsemble                         (#AUDPROC_MODULE_ID_QENSEMBLE)
    - Spectrum Analyzer                 (#AUDPROC_MODULE_ID_SPA)
    - Rx Codec Gain                     (#AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_IIR_AUDIO_COPP_MBDRCV2 0x0001070F

/** @xreflabel{hdr:AdmCmdCoppOpenTopoIdSpkrMchanPeakVol}
    ID of the COPP topology with multichannel peak meter and volume control.

    @inputfigure{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_MCH_PEAK_VOL_topology.png}

    - Peak Meter                        (#AUDPROC_MODULE_ID_PEAK_METER)
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
    @newpage
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_MCH_PEAK_VOL 0x0001031B

/** @xreflabel{hdr:AdmCmdCoppOpenTopoIdMicMonoAudioCopp}
    ID of Tx MIC_MONO in the COPP audio postprocessing device topology.

@inputfigure{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_MIC_MONO_AUDIO_COPP_topology.png}

    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Tx Mic Gain                       (#AUDPROC_MODULE_ID_TX_MIC_GAIN_CTRL)
    - High Pass Filter                  (#AUDPROC_MODULE_ID_HPF_IIR_TX_FILTER)
    - Enhanced Audio Noise Suppression  (#AUDPROC_MODULE_ID_EANS)
    - Tx IIR Tuning Filter              (#AUDPROC_MODULE_ID_TX_IIR_FILTER)
    - Dynamic Range Control             (#AUDPROC_MODULE_ID_MBDRC)
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_MIC_MONO_AUDIO_COPP           0x00010315

/** @xreflabel{hdr:AdmCmdCoppOpenTopoIdMicStereoAudioCopp}
    ID of Tx MIC_STEREO in the COPP audio postprocessing device topology.

@inputfigure{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_MIC_STEREO_AUDIO_COPP_topology.png}

    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Tx Mic Gain                       (#AUDPROC_MODULE_ID_TX_MIC_GAIN_CTRL)
    - High Pass Filter                  (#AUDPROC_MODULE_ID_HPF_IIR_TX_FILTER)
    - Enhanced Audio Noise Suppression  (#AUDPROC_MODULE_ID_EANS)
    - Tx IIR Tuning Filter              (#AUDPROC_MODULE_ID_TX_IIR_FILTER)
    - Dynamic Range Control             (#AUDPROC_MODULE_ID_MBDRC)
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_MIC_STEREO_AUDIO_COPP         0x00010316

/** @xreflabel{hdr:AudprocCoppTopoIdMchanIIRAudio}
    ID of Tx Multichannel IIR in the COPP audio postprocessing
    device topology.

    - Tx IIR Tuning Filter   (#AUDPROC_MODULE_ID_MCHAN_IIR_TUNING_FILTER)
*/
#define AUDPROC_COPP_TOPOLOGY_ID_MCHAN_IIR_AUDIO                    0x00010715

/** @xreflabel{hdr:AdmCmdCoppOpenTopoIdDefAudioCopp}
    ID of the default COPP audio postprocessing device topology.

    @inputfigure{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DEFAULT_AUDIO_COPP_topology.png}
    @newpage
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DEFAULT_AUDIO_COPP           0x00010BE3

/** @xreflabel{hdr:AdmCmdCoppOpenTopoIdPeakmeterAudioCopp}
    ID of the default COPP open topology with peak meter.

    @inputfigure{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_PEAKMETER_AUDIO_COPP_topology.png}
 */
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_PEAKMETER_AUDIO_COPP          0x00010317

/** @} */ /* end_addtogroup audio_pp_topos */

/** @ingroup audio_pp_module_ids
    AUDPROC_MODULE_ID_AIG .

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_AIG_ENABLE
    - #AUDPROC_PARAM_ID_AIG_CONFIG
*/

#define AUDPROC_MODULE_ID_AIG                         0x00010716

/** @addtogroup audio_pp_param_ids
@{ */
/** ID of the Audio AIG  enable parameter used by the #AUDPROC_MODULE_ID_AIG.

    @messagepayload
    @tablespace

*/                         
#define AUDPROC_PARAM_ID_AIG_ENABLE                   0x00010717 

/* Structure for enabling the configuration parameter for
    AUDIO AIG enable. */
typedef struct Audio_AigEnable_t Audio_AigEnable_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_AIG_ENABLE parameter in the AIG module.
*/
struct Audio_AigEnable_t
{

    uint32_t                  enable;
    /**< Specifies whether AIG is disabled (0) or enabled (nonzero).
         
        0 (default): disable
        1: enable         
         @newpage */
}
#include "adsp_end_pack.h"
;


/** @addtogroup audio_pp_param_ids
@{ */
/** ID of the Audio AIG  tunable parameters used by the #AUDPROC_MODULE_ID_AIG.
   @messagepayload
    @structure{Audio_AigParam_t}
    @tablespace

*/   
#define AUDPROC_PARAM_ID_AIG_CONFIG                   0x00010718  

/* Structure for Audio Aig configuration parameters
     */
typedef struct Audio_AigParam_t Audio_AigParam_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDIO_PARAM_AIG parameters in the AIG module.
*/
  struct Audio_AigParam_t
  {
  
    uint16_t 				  		mode;
    /**< Mode word for enabling AIG/SIG mode .
    
         Byte offset: 0 */
	
    int16_t 				  		staticGainL16Q12;
    /**< Static input gain when aigMode is set to 1. 
    
         Byte offset: 2 */	
    int16_t                   		initialGainDBL16Q7;
    /**<Initial value that the adaptive gain update starts from dB Q7
    
         Byte offset: 4 */		
	
	
    int16_t                   		idealRMSDBL16Q7;
	/**<Average RMS level that AIG attempts to achieve Q8.7
   
         Byte offset: 6 */			
    int32_t                   		noiseGateL32;
	/**Threshold below which signal is considered as noise and AIG
   
         Byte offset: 8 */		
    int32_t                   		minGainL32Q15;
	/**Minimum gain that can be provided by AIG Q16.15

   
         Byte offset: 12 */		
    int32_t 				  		maxGainL32Q15;
	/**Maximum gain that can be provided by AIG Q16.15
  
         Byte offset: 16 */			
    uint32_t                  		gainAtRtUL32Q31;
	/**Attack/release time for AIG update Q1.31
  
         Byte offset: 20 */				
	
    uint32_t                  		longGainAtRtUL32Q31;
	/**Long attack/release time while updating gain for noise/silence Q1.31
  
         Byte offset: 24 */	
	
    uint32_t                  		rmsTavUL32Q32;
	/**	RMS smoothing time constant used for long-term RMS estimate Q0.32

         Byte offset: 28 */	
	

    uint32_t                  		gainUpdateStartTimMsUL32Q0;
	/**		The waiting time before which AIG starts to apply adaptive gain update Q32.0

         Byte offset: 32 */		
     }

#include "adsp_end_pack.h"
;
  
/** @ingroup audio_pp_module_ids
    ID of the Enhanced Audio Noise Suppression (EANS) I module.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_EANS_ENABLE
    - #AUDPROC_PARAM_ID_EANS_PARAMS
*/
#define AUDPROC_MODULE_ID_EANS                            0x00010C4A

/** @addtogroup audio_pp_param_ids
@{ */
/** ID of the EANS enable parameter used by the #AUDPROC_MODULE_ID_EANS module.

    @messagepayload
    @structure{audproc_eans_enable_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_EANS_ENABLE.tex}
*/
#define AUDPROC_PARAM_ID_EANS_ENABLE                      0x00010C4B

/** ID of the EANS configuration parameters used by the
    #AUDPROC_MODULE_ID_EANS module.

    @messagepayload
    @structure{audproc_eans_params_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_EANS_PARAMS.tex}
    @newpage
*/
#define AUDPROC_PARAM_ID_EANS_PARAMS                      0x00010C4C

/* Structure for enabling the configuration parameter for
    spectrum analyzer enable. */
typedef struct audproc_eans_enable_t audproc_eans_enable_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_EANS_ENABLE parameter in the EANS module.
*/
struct audproc_eans_enable_t
{

    uint32_t                  enable_flag;
    /**< Specifies whether EANS is disabled (0) or enabled (nonzero).
           
         This is supported only for sampling rates of 8, 12, 16, 24, 32, and
         48 kHz. It is not supported for sampling rates of 11.025, 22.05, or
         44.1 kHz.
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for EANS configuration parameters. */
typedef struct audproc_eans_params_t audproc_eans_params_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_EANS_ENABLE parameters in the EANS module.
*/
struct audproc_eans_params_t
{
    int16_t                         eans_mode;
    /**< Mode word for enabling/disabling submodules.
    
         Byte offset: 0 */

    int16_t                         eans_input_gain;
    /**< Q2.13 input gain to the EANS module.
    
         Byte offset: 2 */

    int16_t                         eans_output_gain;
    /**< Q2.13 output gain to the EANS module.
    
         Byte offset: 4 */

    int16_t                         eans_target_ns;
    /**< Target noise suppression level in dB.
    
         Byte offset: 6 */

    int16_t                         eans_s_alpha;
    /**< Q3.12 over-subtraction factor for stationary noise suppression.
    
         Byte offset: 8 */

    int16_t                         eans_n_alpha;
    /**< Q3.12 over-subtraction factor for nonstationary noise suppression.
    
         Byte offset: 10
         @newpage */

    int16_t                         eans_n_alphamax;
    /**< Q3.12 maximum over-subtraction factor for nonstationary noise
         suppression.
    
         Byte offset: 12 */

    int16_t                         eans_e_alpha;
    /**< Q15 scaling factor for excess noise suppression.
    
         Byte offset: 14 */

    int16_t                         eans_ns_snrmax;
    /**< Upper boundary in dB for SNR estimation.
    
         Byte offset: 16 */

    int16_t                         eans_sns_block;
    /**< Quarter block size for stationary noise suppression.
    
         Byte offset: 18  */

    int16_t                         eans_ns_i;
    /**< Initialization block size for noise suppression.
    
         Byte offset: 20 */

    int16_t                         eans_np_scale;
    /**< Power scale factor for nonstationary noise update.

         Byte offset: 22 */

    int16_t                         eans_n_lambda;
    /**< Smoothing factor for higher level nonstationary noise update.

         Byte offset: 24 */

    int16_t                         eans_n_lambdaf;
    /**< Medium averaging factor for noise update.

         Byte offset: 26
         @newpage */

    int16_t                         eans_gs_bias;
    /**< Bias factor in dB for gain calculation.

         Byte offset: 28 */

    int16_t                         eans_gs_max;
    /**< SNR lower boundary in dB for aggressive gain calculation.

         Byte offset: 30 */

    int16_t                         eans_s_alpha_hb;
    /**< Q3.12 over-subtraction factor for high-band stationary noise
         suppression.

         Byte offset: 32 */

    int16_t                         eans_n_alphamax_hb;
    /**< Q3.12 maximum over-subtraction factor for high-band nonstationary noise
         suppression.

         Byte offset: 34 */

    int16_t                         eans_e_alpha_hb;
    /**< Q15 scaling factor for high-band excess noise suppression.

         Byte offset: 36 */

    int16_t                         eans_n_lambda0;
    /**< Smoothing factor for nonstationary noise update during speech activity.

         Byte offset: 38 */

    int16_t                         thresh;
    /**< Threshold for generating a binary VAD decision.

         Byte offset: 40 */

    int16_t                         pwr_scale;
    /**< Indirect lower boundary of the noise level estimate.

         Byte offset: 42
         @newpage */

    int16_t                         hangover_max;
    /**< Avoids mid-speech clipping and reliably detects weak speech bursts at
         the end of speech activity.

         Byte offset: 44 */

    int16_t                         alpha_snr;
    /**< Controls responsiveness of the VAD.

         Byte offset: 46 */

    int16_t                         snr_diff_max;
    /**< Maximum SNR difference. Decreasing this parameter value may help in
         making correct decisions during abrupt changes; however, decreasing
         too much may increase false alarms during long pauses/silences.

         Byte offset: 48 */

    int16_t                         snr_diff_min;
    /**< Minimum SNR difference. Decreasing this parameter value may help in
         making correct decisions during abrupt changes; however, decreasing
         too much may increase false alarms during long pauses/silences.

         Byte offset: 50 */

    int16_t                         init_length;
    /**< Defines the number of frames for which a noise level estimate is set
         to a fixed value.

         Byte offset: 52 */

    int16_t                         max_val;
    /**< Defines the upper limit of the noise level.

         Byte offset: 54 */

    int16_t                         init_bound;
    /**< Defines the initial bounding value for the noise level estimate. This
         is used during the initial segment defined by the init_length
         parameter.

         Byte offset: 56
         @newpage */

    int16_t                         reset_bound;
    /**< Reset boundary for noise tracking.

         Byte offset: 58  */

    int16_t                         avar_scale;
    /**< Defines the bias factor in noise estimation.

         Byte offset: 60 */

    int16_t                         sub_nc;
    /**< Defines the window length for noise estimation.

         Byte offset: 62 */

    int16_t                         spow_min;
    /**< Defines the minimum signal power required to update the boundaries
         for the noise floor estimate.

         Byte offset: 64 */

    int16_t                         eans_gs_fast;
    /**< Fast smoothing factor for postprocessor gain.

         Byte offset: 66 */

    int16_t                         eans_gs_med;
    /**< Medium smoothing factor for postprocessor gain.

         Byte offset: 68 */

    int16_t                         eans_gs_slow;
    /**< Slow smoothing factor for postprocessor gain.

         Byte offset: 70 */

    int16_t                         eans_swb_salpha;
    /**< Q3.12 super wideband aggressiveness factor for stationary noise
         suppression.

         Byte offset: 72 */

    int16_t                         eans_swb_nalpha;
    /**< Q3.12 super wideband aggressiveness factor for nonstationary noise
         suppression.

         Byte offset: 74
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup audio_pp_param_ids */

/** @ingroup audio_pp_module_ids
    ID of the Tx Mic Gain Control module.

    This module supports the following parameter ID:
    - #AUDPROC_PARAM_ID_TX_MIC_GAIN
*/
#define AUDPROC_MODULE_ID_TX_MIC_GAIN_CTRL                              0x00010C35

/** @addtogroup audio_pp_param_ids
@{ */
/** ID of the Tx mic gain control parameter used by the 
    #AUDPROC_MODULE_ID_TX_MIC_GAIN_CTRL module.

    @messagepayload
    @structure{audproc_tx_mic_gain_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_TX_MIC_GAIN.tex}
*/
#define AUDPROC_PARAM_ID_TX_MIC_GAIN                                    0x00010C36

/* Structure for a Tx mic gain parameter for the mic gain
    control module. */
typedef struct audproc_tx_mic_gain_t audproc_tx_mic_gain_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_TX_MIC_GAIN parameter in the
    Tx Mic Gain Control module.
*/
struct audproc_tx_mic_gain_t
{
    uint16_t                  tx_mic_gain;
    /**< Linear gain in Q13 format. */

    uint16_t                  reserved;
    /**< Clients must set this field to zero.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup audio_pp_param_ids */

/** @ingroup audio_pp_module_ids
    ID of the Rx Codec Gain Control module.

    This module supports the following parameter ID:
    - #AUDPROC_PARAM_ID_RX_CODEC_GAIN
*/
#define AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL                            0x00010C37

/** @addtogroup audio_pp_param_ids
@{ */
/** ID of the Rx codec gain control parameter used by the
    #AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL module.

    @messagepayload
    @structure{audproc_rx_codec_gain_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_RX_CODEC_GAIN.tex}
    @newpage
*/
#define AUDPROC_PARAM_ID_RX_CODEC_GAIN                                  0x00010C38

/* Structure for the Rx common codec gain control module. */
typedef struct audproc_rx_codec_gain_t audproc_rx_codec_gain_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_RX_CODEC_GAIN parameter in the
    Rx Codec Gain Control module.
*/
struct audproc_rx_codec_gain_t
{
    uint16_t                  rx_codec_gain;
    /**< Linear gain in Q13 format. */

    uint16_t                  reserved;
    /**< Clients must set this field to zero.
       @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup audio_pp_param_ids */

/** @ingroup audio_pp_module_ids
    ID of the HPF Tuning Filter module on the Tx path.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_ENABLE_CONFIG
    - #AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_PRE_GAIN
    - #AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_CONFIG_PARAMS
*/
#define AUDPROC_MODULE_ID_HPF_IIR_TX_FILTER                             0x00010C3D

/** @addtogroup audio_pp_param_ids
@{ */
/** ID of the Tx HPF IIR filter enable parameter used by the
    #AUDPROC_MODULE_ID_HPF_IIR_TX_FILTER module.
    
    @parspace Message payload
    @structure{audproc_hpf_tx_iir_filter_enable_cfg_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_ENABLE_CONFIG.tex}
*/
#define AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_ENABLE_CONFIG                0x00010C3E

/** ID of the Tx HPF IIR filter pregain parameter used by the
    #AUDPROC_MODULE_ID_HPF_IIR_TX_FILTER module.

    @parspace Message payload
    @structure{audproc_hpf_tx_iir_filter_pre_gain_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_PRE_GAIN.tex}
*/
#define AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_PRE_GAIN                     0x00010C3F

/** ID of the Tx HPF IIR filter configuration parameters used by the
    #AUDPROC_MODULE_ID_HPF_IIR_TX_FILTER module.

    @parspace Message payload
    @structure{audproc_hpf_tx_iir_filter_cfg_params_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_CONFIG_PARAMS.tex}
    @newpage
*/
#define AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_CONFIG_PARAMS                0x00010C40

/* Structure for enabling a configuration parameter for
    the HPF IIR tuning filter module on the Tx path. */
typedef struct audproc_hpf_tx_iir_filter_enable_cfg_t audproc_hpf_tx_iir_filter_enable_cfg_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_ENABLE_CONFIG
    parameter in the Tx path HPF Tuning Filter module.
*/
struct audproc_hpf_tx_iir_filter_enable_cfg_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether the HPF tuning filter is disabled (0) or
         enabled (nonzero).
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for the pregain parameter for the HPF
    IIR tuning filter module on the Tx path. */
typedef struct audproc_hpf_tx_iir_filter_pre_gain_t audproc_hpf_tx_iir_filter_pre_gain_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_PRE_GAIN parameter
    in the Tx path HPF Tuning Filter module.
*/
struct audproc_hpf_tx_iir_filter_pre_gain_t
{
    uint16_t                  pre_gain;
    /**< Linear gain in Q13 format. */

    uint16_t                  reserved;
    /**< Clients must set this field to zero.
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for the configuration parameter for the
    HPF IIR tuning filter module on the Tx path. */
typedef struct audproc_hpf_tx_iir_filter_cfg_params_t audproc_hpf_tx_iir_filter_cfg_params_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_CONFIG_PARAMS
    parameters in the Tx path HPF Tuning Filter module. \n
    \n
    This structure is followed by tuning filter coefficients as follows: \n
    - Sequence of int32 FilterCoeffs.
      Each band has five coefficients, each in int32 format in the order of
      b0, b1, b2, a1, a2.
    - Sequence of int16 NumShiftFactor.
      One int16 per band. The numerator shift factor is related to the Q
      factor of the filter coefficients. 
    - Sequence of uint16 PanSetting.
      One uint16 for each band to indicate application of the filter to
      left (0), right (1), or both (2) channels.
*/
struct audproc_hpf_tx_iir_filter_cfg_params_t
{
    uint16_t                  num_biquad_stages;
    /**< Number of bands.

         Supported values: 0 to 20 */

    uint16_t                  reserved;
    /**< Clients must set this field to zero.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup audio_pp_module_ids */

/** @addtogroup audio_pp_module_ids
@{ */
/** ID of the Tx path IIR Tuning Filter module.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_TX_IIR_FILTER_ENABLE_CONFIG
*/
#define AUDPROC_MODULE_ID_TX_IIR_FILTER                             0x00010C41

/** ID of the Rx path IIR Tuning Filter module for the left channel.

    The parameter IDs of the IIR tuning filter module
    (#AUDPROC_MODULE_ID_IIR_TUNING_FILTER) are used for the left IIR Rx tuning
    filter.
    
    Pan parameters are not required for this per-channel IIR filter; the pan
    parameters are ignored by this module.
*/
#define AUDPROC_MODULE_ID_LEFT_IIR_TUNING_FILTER                     0x00010705

/** ID of the the Rx path IIR Tuning Filter module for the right channel.

    The parameter IDs of the IIR tuning filter module
    (#AUDPROC_MODULE_ID_IIR_TUNING_FILTER) are used for the right IIR Rx tuning
    filter.  

    Pan parameters are not required for this per-channel IIR filter; the pan
    parameters are ignored by this module.
*/
#define AUDPROC_MODULE_ID_RIGHT_IIR_TUNING_FILTER                    0x00010706

/** @} */ /* end_addtogroup audio_pp_module_ids */

/** @addtogroup audio_pp_param_ids
@{ */

/** ID of the Tx IIR filter enable parameter used by the
    #AUDPROC_MODULE_ID_TX_IIR_FILTER module.

    @parspace Message payload
    @structure{audproc_tx_iir_filter_enable_cfg_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_TX_IIR_FILTER_ENABLE_CONFIG.tex}
*/
#define AUDPROC_PARAM_ID_TX_IIR_FILTER_ENABLE_CONFIG                0x00010C42

/** ID of the Tx IIR filter pregain parameter used by the
    #AUDPROC_MODULE_ID_TX_IIR_FILTER module.

    @parspace Message payload
    @structure{audproc_tx_iir_filter_pre_gain_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_TX_IIR_FILTER_PRE_GAIN.tex}
*/
#define AUDPROC_PARAM_ID_TX_IIR_FILTER_PRE_GAIN                     0x00010C43

/** ID of the Tx IIR filter configuration parameters used by the
    #AUDPROC_MODULE_ID_TX_IIR_FILTER module.

    @parspace Message payload
    @structure{audproc_tx_iir_filter_cfg_params_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_TX_IIR_FILTER_CONFIG_PARAMS.tex}
    @newpage
*/
#define AUDPROC_PARAM_ID_TX_IIR_FILTER_CONFIG_PARAMS                0x00010C44

/* Structure for enabling the configuration parameter for the
    IIR filter module on the Tx path. */
typedef struct audproc_tx_iir_filter_enable_cfg_t audproc_tx_iir_filter_enable_cfg_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_TX_IIR_FILTER_ENABLE_CONFIG parameter
    in the Tx Path IIR Tuning Filter module.
*/
struct audproc_tx_iir_filter_enable_cfg_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether the IIR tuning filter is disabled (0) or enabled
         (nonzero).
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for the pregain parameter for the
    IIR filter module on the Tx path. */
typedef struct audproc_tx_iir_filter_pre_gain_t audproc_tx_iir_filter_pre_gain_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_TX_IIR_FILTER_PRE_GAIN parameter
    in the Tx Path IIR Tuning Filter module.
*/
struct audproc_tx_iir_filter_pre_gain_t
{
    uint16_t                  pre_gain;
    /**< Linear gain in Q13 format. */

    uint16_t                  reserved;
    /**< Clients must set this field to zero.
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for the configuration parameter for the
    IIR filter module on the Tx path. */
typedef struct audproc_tx_iir_filter_cfg_params_t audproc_tx_iir_filter_cfg_params_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_TX_IIR_FILTER_CONFIG_PARAMS parameter
    in the Tx Path IIR Tuning Filter module. \n
    \n
    This structure is followed by the HPF IIR filter coefficients on the Tx
    path as follows: \n
    - Sequence of int32 ulFilterCoeffs.
      Each band has five coefficients, each in int32 format in the order of
      b0, b1, b2, a1, a2.
    - Sequence of int16 sNumShiftFactor.
      One int16 per band. The numerator shift factor is related to the Q
      factor of the filter coefficients. 
    - Sequence of uint16 usPanSetting.
      One uint16 for each band to indicate if the filter is applied to
      left (0), right (1), or both (2) channels.
*/
struct audproc_tx_iir_filter_cfg_params_t
{
    uint16_t                  num_biquad_stages;
    /**< Number of bands.

         Supported values: 0 to 20 */

    uint16_t                  reserved;
    /**< Clients must set this field to zero.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup audio_pp_module_ids */

/** @ingroup audio_pp_module_ids
    ID of the QEnsemble module.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_QENSEMBLE_ENABLE
    - #AUDPROC_PARAM_ID_QENSEMBLE_BACKGAIN
    - #AUDPROC_PARAM_ID_QENSEMBLE_SET_NEW_ANGLE
    @newpage
*/
#define AUDPROC_MODULE_ID_QENSEMBLE                                    0x00010C59

/** @addtogroup audio_pp_param_ids
@{ */
/** ID of the QEnsemble enable parameter used by the #AUDPROC_MODULE_ID_QENSEMBLE
    module.

    @messagepayload
    @structure{audproc_qensemble_enable_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_QENSEMBLE_ENABLE.tex}
*/
#define AUDPROC_PARAM_ID_QENSEMBLE_ENABLE                              0x00010C60

/** ID of the QEnsemble back gain parameter used by the
    #AUDPROC_MODULE_ID_QENSEMBLE module.

    @messagepayload
    @structure{audproc_qensemble_param_backgain_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_QENSEMBLE_BACKGAIN.tex}
*/
#define AUDPROC_PARAM_ID_QENSEMBLE_BACKGAIN                            0x00010C61

/** ID of the QEnsemble new angle parameter used by the
    #AUDPROC_MODULE_ID_QENSEMBLE module.

    @messagepayload
    @structure{audproc_qensemble_param_set_new_angle_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_QENSEMBLE_SET_NEW_ANGLE.tex}
    @newpage
*/
#define AUDPROC_PARAM_ID_QENSEMBLE_SET_NEW_ANGLE                       0x00010C62

/* Structure for enabling the configuration parameter for the
    QEnsemble module. */
typedef struct audproc_qensemble_enable_t audproc_qensemble_enable_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_QENSEMBLE_ENABLE parameter used by the
    QEnsemble module.
*/
struct audproc_qensemble_enable_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether the QEnsemble module is disabled (0) or enabled
         (nonzero).
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for the background gain for the QEnsemble module. */
typedef struct audproc_qensemble_param_backgain_t audproc_qensemble_param_backgain_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_QENSEMBLE_BACKGAIN parameter used by
    the QEnsemble module.
*/
struct audproc_qensemble_param_backgain_t
{
     int16_t                  back_gain;
     /**< Linear gain in Q15 format.

          Supported values: 0 to 32767 */

     uint16_t                 reserved;
     /**< Clients must set this field to zero.
          @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for setting a new angle for the QEnsemble module. */
typedef struct audproc_qensemble_param_set_new_angle_t audproc_qensemble_param_set_new_angle_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_QENSEMBLE_SET_NEW_ANGLE parameter used
    by the QEnsemble module.
*/
struct audproc_qensemble_param_set_new_angle_t
{
     int16_t                    new_angle;
     /**< New angle in degrees.

          Supported values: 0 to 359 */

     int16_t                    time_ms;
     /**< Transition time in milliseconds to set the new angle.

          Supported values: 0 to 32767
          @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup audio_pp_module_ids */

/** @ingroup audio_pp_module_ids
    ID of the Volume Control module pre/postprocessing block.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_VOL_CTRL_MASTER_GAIN
    - #AUDPROC_PARAM_ID_VOL_CTRL_LR_CHANNEL_GAIN
    - #AUDPROC_PARAM_ID_VOL_CTRL_MUTE_CONFIG
    - #AUDPROC_PARAM_ID_SOFT_VOL_STEPPING_PARAMETERS
    - #AUDPROC_PARAM_ID_SOFT_PAUSE_PARAMETERS
    - #AUDPROC_PARAM_ID_MULTICHANNEL_GAIN
    - #AUDPROC_PARAM_ID_MULTICHANNEL_MUTE
*/
#define AUDPROC_MODULE_ID_VOL_CTRL                                      0x00010BFE

/** @addtogroup audio_pp_param_ids
@{ */
/** ID of the master gain parameter used by the #AUDPROC_MODULE_ID_VOL_CTRL module. 

    @messagepayload
    @structure{audproc_volume_ctrl_master_gain_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_VOL_CTRL_MASTER_GAIN.tex}
*/
#define AUDPROC_PARAM_ID_VOL_CTRL_MASTER_GAIN                           0x00010BFF

/** ID of the left/right channel gain parameter used by the
    #AUDPROC_MODULE_ID_VOL_CTRL module. 

    @messagepayload
    @structure{audproc_volume_ctrl_lr_chan_gain_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_VOL_CTRL_LR_CHANNEL_GAIN.tex}
*/
#define AUDPROC_PARAM_ID_VOL_CTRL_LR_CHANNEL_GAIN                       0x00010C00

/** ID of the mute configuration parameter used by the
    #AUDPROC_MODULE_ID_VOL_CTRL module.
    
    @messagepayload
    @structure{audproc_volume_ctrl_mute_config_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_VOL_CTRL_MUTE_CONFIG.tex}
*/
#define AUDPROC_PARAM_ID_VOL_CTRL_MUTE_CONFIG                           0x00010C01

/** ID of the soft stepping volume parameters used by the
    #AUDPROC_MODULE_ID_VOL_CTRL module.

    @messagepayload
    @structure{audproc_soft_step_volume_params_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_SOFT_VOL_STEPPING_PARAMETERS.tex}
*/
#define AUDPROC_PARAM_ID_SOFT_VOL_STEPPING_PARAMETERS                   0x00010C29

/** ID of the soft pause parameters used by the #AUDPROC_MODULE_ID_VOL_CTRL module.

    @messagepayload
    @structure{audproc_soft_pause_params_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_SOFT_PAUSE_PARAMETERS.tex}
*/
#define AUDPROC_PARAM_ID_SOFT_PAUSE_PARAMETERS                          0x00010D6A

/** ID of the multiple-channel volume control parameters used by the
    #AUDPROC_MODULE_ID_VOL_CTRL module.

    @messagepayload
    @structure{audproc_volume_ctrl_multichannel_gain_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_MULTICHANNEL_GAIN.tex}

    @parspace Payload format of channel type/gain pairs
    @structure{audproc_volume_ctrl_channel_type_gain_pair_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_MULTICHANNEL_GAIN_pairs.tex}
*/
#define AUDPROC_PARAM_ID_MULTICHANNEL_GAIN                              0x00010713

/** ID of the multiple-channel mute configuration parameters used by the
    #AUDPROC_MODULE_ID_VOL_CTRL module.

    @messagepayload
    @structure{audproc_volume_ctrl_multichannel_mute_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_MULTICHANNEL_MUTE.tex}
    @newpage

    @parspace Payload format of channel type/mute setting pairs
    @structure{audproc_volume_ctrl_channel_type_mute_pair_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_MULTICHANNEL_MUTE_pairs.tex}
*/
#define AUDPROC_PARAM_ID_MULTICHANNEL_MUTE                              0x00010714

/* Structure for the master gain parameter for a volume control module. */
typedef struct audproc_volume_ctrl_master_gain_t audproc_volume_ctrl_master_gain_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_VOL_CTRL_MASTER_GAIN parameter used
    by the Volume Control module.
*/
struct audproc_volume_ctrl_master_gain_t
{
    uint16_t                  master_gain;
    /**< Linear gain in Q13 format. */

    uint16_t                  reserved;
    /**< Clients must set this field to zero.
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for the left/right channel gain parameter for a
    volume control module. */
typedef struct audproc_volume_ctrl_lr_chan_gain_t audproc_volume_ctrl_lr_chan_gain_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_VOL_CTRL_LR_CHANNEL_GAIN parameters
    used by the Volume Control module.
*/
struct audproc_volume_ctrl_lr_chan_gain_t
{
    uint16_t                  l_chan_gain;
    /**< Linear gain in Q13 format for the left channel. */

    uint16_t                  r_chan_gain;
    /**< Linear gain in Q13 format for the right channel.
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for the mute configuration parameter for a
    volume control module. */
typedef struct audproc_volume_ctrl_mute_config_t audproc_volume_ctrl_mute_config_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_VOL_CTRL_MUTE_CONFIG parameter used
    by the Volume Control module.
*/
struct audproc_volume_ctrl_mute_config_t
{
    uint32_t                  mute_flag;
    /**< Specifies whether mute is disabled (0) or enabled (nonzero).
         @newpage */
}
#include "adsp_end_pack.h"
;

/** @xreflabel{hdr:AsmParamSvcRampCurveLinear}
    Supported parameters for a soft stepping linear ramping curve.
*/
#define AUDPROC_PARAM_SVC_RAMPINGCURVE_LINEAR                               0

/** @xreflabel{hdr:AsmParamSvcRampCurveExp}
    Exponential ramping curve.
*/
#define AUDPROC_PARAM_SVC_RAMPINGCURVE_EXP                                  1

/** @xreflabel{hdr:AsmParamSvcRampCurveLog}
    Logarithmic ramping curve.
*/
#define AUDPROC_PARAM_SVC_RAMPINGCURVE_LOG                                  2

/* Structure for holding soft stepping volume parameters. */
typedef struct audproc_soft_step_volume_params_t audproc_soft_step_volume_params_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_SOFT_VOL_STEPPING_PARAMETERS
    parameters used by the Volume Control module.
*/
struct audproc_soft_step_volume_params_t
{
    uint32_t                  period;
    /**< Period in milliseconds.

         Supported values: 0 to 15000 */

    uint32_t                  step;
    /**< Step in microseconds.

         Supported values: 0 to 15000000 */

    uint32_t                  ramping_curve;
    /**< Ramping curve type.

         Supported values:
         - #AUDPROC_PARAM_SVC_RAMPINGCURVE_LINEAR
         - #AUDPROC_PARAM_SVC_RAMPINGCURVE_EXP
         - #AUDPROC_PARAM_SVC_RAMPINGCURVE_LOG
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for holding soft pause parameters. */
typedef struct audproc_soft_pause_params_t audproc_soft_pause_params_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_SOFT_PAUSE_PARAMETERS parameters used
    by the Volume Control module.
*/
struct audproc_soft_pause_params_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether soft pause is disabled (0) or enabled (nonzero). */

    uint32_t                  period;
    /**< Period in milliseconds.

         Supported values: 0 to 15000 */

    uint32_t                  step;
    /**< Step in microseconds.

         Supported values: 0 to 15000000 */

    uint32_t                  ramping_curve;
    /**< Ramping curve.

         Supported values:
         - #AUDPROC_PARAM_SVC_RAMPINGCURVE_LINEAR
         - #AUDPROC_PARAM_SVC_RAMPINGCURVE_EXP
         - #AUDPROC_PARAM_SVC_RAMPINGCURVE_LOG
         @newpage */
}
#include "adsp_end_pack.h"
;

/** Maximum number of channels.
    @newpage
*/
#define VOLUME_CONTROL_MAX_CHANNELS                       8

/* Structure for holding one channel type - gain pair. */
typedef struct audproc_volume_ctrl_channel_type_gain_pair_t audproc_volume_ctrl_channel_type_gain_pair_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_MULTICHANNEL_GAIN channel type/gain
    pairs used by the Volume Control module. \n
    \n
    This structure immediately follows the audproc_volume_ctrl_multichannel_gain_t
    structure.
*/
struct audproc_volume_ctrl_channel_type_gain_pair_t
{
    uint8_t                   channel_type;
    /**< Channel type for which the gain setting is to be applied.

         Supported values:
         - #PCM_CHANNEL_L
         - #PCM_CHANNEL_R
         - #PCM_CHANNEL_C
         - #PCM_CHANNEL_LS
         - #PCM_CHANNEL_RS
         - #PCM_CHANNEL_LFE
         - #PCM_CHANNEL_CS
         - #PCM_CHANNEL_LB
         - #PCM_CHANNEL_RB
         - #PCM_CHANNEL_TS
         - #PCM_CHANNEL_CVH
         - #PCM_CHANNEL_MS
         - #PCM_CHANNEL_FLC
         - #PCM_CHANNEL_FRC
         - #PCM_CHANNEL_RLC
         - #PCM_CHANNEL_RRC */

    uint8_t                   reserved1;
    /**< Clients must set this field to zero. */

    uint8_t                   reserved2;
    /**< Clients must set this field to zero. */

    uint8_t                   reserved3;
    /**< Clients must set this field to zero. */

    uint32_t                  gain;
    /**< Gain value for this channel in Q28 format.

         Supported values: Any
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for the multichannel gain command */
typedef struct audproc_volume_ctrl_multichannel_gain_t audproc_volume_ctrl_multichannel_gain_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_MULTICHANNEL_GAIN parameters used by
    the Volume Control module.
*/
struct audproc_volume_ctrl_multichannel_gain_t
{
    uint32_t                  num_channels;
    /**< Number of channels for which gain values are provided. Any channels
         present in the data for which gain is not provided are set to unity
         gain.

         Supported values: 1 to 8 */

    audproc_volume_ctrl_channel_type_gain_pair_t     gain_data[VOLUME_CONTROL_MAX_CHANNELS];
    /**< Array of channel type/gain pairs.

         Supported values: See audproc_volume_ctrl_channel_type_gain_pair_t
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for holding one channel type - mute pair. */
typedef struct audproc_volume_ctrl_channel_type_mute_pair_t audproc_volume_ctrl_channel_type_mute_pair_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_MULTICHANNEL_MUTE channel type/mute
    setting pairs used by the Volume Control module. \n
    \n
    This structure immediately follows the audproc_volume_ctrl_multichannel_mute_t
    structure.
*/
struct audproc_volume_ctrl_channel_type_mute_pair_t
{
    uint8_t                   channel_type;
    /**< Channel type for which the mute setting is to be applied.

         Supported values: 
         - #PCM_CHANNEL_L
         - #PCM_CHANNEL_R
         - #PCM_CHANNEL_C
         - #PCM_CHANNEL_LS
         - #PCM_CHANNEL_RS
         - #PCM_CHANNEL_LFE
         - #PCM_CHANNEL_CS
         - #PCM_CHANNEL_LB
         - #PCM_CHANNEL_RB
         - #PCM_CHANNEL_TS
         - #PCM_CHANNEL_CVH
         - #PCM_CHANNEL_MS
         - #PCM_CHANNEL_FLC
         - #PCM_CHANNEL_FRC
         - #PCM_CHANNEL_RLC
         - #PCM_CHANNEL_RRC*/

    uint8_t                   reserved1;
    /**< Clients must set this field to zero. */

    uint8_t                   reserved2;
    /**< Clients must set this field to zero. */

    uint8_t                   reserved3;
    /**< Clients must set this field to zero. */

    uint32_t                  mute;
    /**< Mute setting for this channel.

         Supported values:
         - 0 = Unmute
         - Nonzero = Mute
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for the multichannel mute command */
typedef struct audproc_volume_ctrl_multichannel_mute_t audproc_volume_ctrl_multichannel_mute_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_MULTICHANNEL_MUTE parameters used by
    the Volume Control module.
*/
struct audproc_volume_ctrl_multichannel_mute_t
{
    uint32_t                  num_channels;
    /**< Number of channels for which mute configuration is provided. Any
         channels present in the data for which mute configuration is not
         provided are set to unmute.

         Supported values: 1 to 8 */

    audproc_volume_ctrl_channel_type_mute_pair_t    mute_data[VOLUME_CONTROL_MAX_CHANNELS];
    /**< Array of channel type/mute setting pairs.

         Supported values: See audproc_volume_ctrl_channel_type_gain_pair_t
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup audio_pp_param_ids */

/** @ingroup audio_pp_module_ids
    ID of the IIR Tuning Filter module.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_IIR_TUNING_FILTER_ENABLE_CONFIG
    - #AUDPROC_PARAM_ID_IIR_TUNING_FILTER_PRE_GAIN
    - #AUDPROC_PARAM_ID_IIR_TUNING_FILTER_CONFIG_PARAMS
*/
#define AUDPROC_MODULE_ID_IIR_TUNING_FILTER                             0x00010C02

/** @addtogroup audio_pp_param_ids
@{ */
/** ID of the IIR tuning filter enable parameter used by the
    #AUDPROC_MODULE_ID_IIR_TUNING_FILTER module.

    @messagepayload
    @structure{audproc_iir_tuning_filter_enable_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_IIR_TUNING_FILTER_ENABLE_CONFIG.tex}
*/
#define AUDPROC_PARAM_ID_IIR_TUNING_FILTER_ENABLE_CONFIG                0x00010C03

/** ID of the IIR tuning filter pregain parameter used by the
    #AUDPROC_MODULE_ID_IIR_TUNING_FILTER module.

    @messagepayload
    @structure{audproc_iir_tuning_filter_pregain_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_IIR_TUNING_FILTER_PRE_GAIN.tex}
*/
#define AUDPROC_PARAM_ID_IIR_TUNING_FILTER_PRE_GAIN                     0x00010C04

/** ID of the IIR tuning filter configuration parameters used by the
    #AUDPROC_MODULE_ID_IIR_TUNING_FILTER module.

    @messagepayload
    @structure{audproc_iir_filter_config_params_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_IIR_TUNING_FILTER_CONFIG_PARAMS.tex}
    @newpage
*/
#define AUDPROC_PARAM_ID_IIR_TUNING_FILTER_CONFIG_PARAMS                0x00010C05

/* Structure for an enable configuration parameter for an
    IIR tuning filter module. */
typedef struct audproc_iir_tuning_filter_enable_t audproc_iir_tuning_filter_enable_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_IIR_TUNING_FILTER_ENABLE_CONFIG
    parameter used by the IIR Tuning Filter module.
*/
struct audproc_iir_tuning_filter_enable_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether the IIR tuning filter is disabled (0) or
         enabled (1).
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for the pregain parameter for an IIR tuning filter module. */
typedef struct audproc_iir_tuning_filter_pregain_t audproc_iir_tuning_filter_pregain_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_IIR_TUNING_FILTER_PRE_GAIN
    parameters used by the IIR Tuning Filter module.
*/
struct audproc_iir_tuning_filter_pregain_t
{
    uint16_t                  pregain;
    /**< Linear gain in Q13 format. */

    uint16_t                  reserved;
    /**< Clients must set this field to zero.
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for the configuration parameter for an IIR tuning filter module. */
typedef struct audproc_iir_filter_config_params_t audproc_iir_filter_config_params_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_IIR_TUNING_FILTER_CONFIG_PARAMS
    parameters used by the IIR Tuning Filter module. \n
    \n
    This structure is followed by the IIR filter coefficients: \n
    - Sequence of int32 FilterCoeffs \n
      Five coefficients for each band. Each coefficient is in int32 format, in
      the order of b0, b1, b2, a1, a2.
    - Sequence of int16 NumShiftFactor \n
      One int16 per band. The numerator shift factor is related to the Q
      factor of the filter coefficients. 
    - Sequence of uint16 PanSetting \n
      One uint16 per band, indicating if the filter is applied to left (0),
      right (1), or both (2) channels.
*/
struct audproc_iir_filter_config_params_t
{
    uint16_t                  num_biquad_stages;
    /**< Number of bands.

         Supported values: 0 to 20 */

    uint16_t                  reserved;
    /**< Clients must set this field to zero.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup audio_pp_param_ids */

/** @ingroup audio_pp_module_ids
    ID of the Multiband Dynamic Range Control (MBDRC) module on the Tx/Rx
    paths.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_MBDRC_ENABLE
    - #AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS
*/
#define AUDPROC_MODULE_ID_MBDRC                                        0x00010C06

/** @addtogroup audio_pp_param_ids
@{ */
/** ID of the MBDRC enable parameter used by the #AUDPROC_MODULE_ID_MBDRC module.

    @messagepayload
    @structure{audproc_mbdrc_enable_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_MBDRC_ENABLE.tex}
*/
#define AUDPROC_PARAM_ID_MBDRC_ENABLE                                  0x00010C07

/** ID of the MBDRC configuration parameters used by the
    #AUDPROC_MODULE_ID_MBDRC module.

    @messagepayload
    @structure{audproc_mbdrc_config_params_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS.tex}

    @parspace Sub-band DRC configuration parameters
    @structure{audproc_subband_drc_config_params_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS_subband_DRC.tex}

    @keep{6}
    To obtain legacy ADRC from MBDRC, use the calibration tool to:

    - Enable MBDRC (EnableFlag = TRUE)
    - Set number of bands to 1 (uiNumBands = 1)
    - Enable the first MBDRC band (DrcMode[0] = DRC_ENABLED = 1)
    - Clear the first band mute flag (MuteFlag[0] = 0)
    - Set the first band makeup gain to unity (compMakeUpGain[0] = 0x2000)
    - Use the legacy ADRC parameters to calibrate the rest of the MBDRC 
      parameters.
    @newpage 
*/
#define AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS                           0x00010C08

/** @} */ /* end_addtogroup audio_pp_param_ids */

/** @ingroup audio_pp_module_ids
    ID of the MMBDRC module version 2 pre/postprocessing block.

    This module differs from the original MBDRC (#AUDPROC_MODULE_ID_MBDRC) in
    the length of the filters used in each sub-band.

    This module supports the following parameter ID:
    - #AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS_IMPROVED_FILTBANK_V2
    @newpage
*/
#define AUDPROC_MODULE_ID_MBDRCV2                                0x0001070B

/** @addtogroup audio_pp_param_ids
@{ */
/** ID of the configuration parameters used by the #AUDPROC_MODULE_ID_MBDRCV2
    module for the improved filter structure of the MBDRC v2 pre/postprocessing
    block. 

    The update to this configuration structure from the original MBDRC is the
    number of filter coefficients in the filter structure. The sequence for
    is as follows:

    - 1 band = 0 FIR coefficient + 1 mute flag + uint16 padding
    - 2 bands = 141 FIR coefficients + 2 mute flags + uint16 padding    
    - 3 bands = 141+81 FIR coefficients + 3 mute flags + uint16 padding    
    - 4 bands = 141+81+61 FIR coefficients + 4 mute flags + uint16 padding     
    - 5 bands = 141+81+61+61 FIR coefficients + 5 mute flags + uint16 padding
      
    This block uses the same parameter structure as
    #AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS.
*/
#define AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS_IMPROVED_FILTBANK_V2 0x0001070C

/* Structure for the enable parameter for an MBDRC module. */
typedef struct audproc_mbdrc_enable_t audproc_mbdrc_enable_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_MBDRC_ENABLE parameter used by the
    MBDRC module.
*/
struct audproc_mbdrc_enable_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether MBDRC is disabled (0) or enabled (nonzero).
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for the configuration parameters for an MBDRC module. */
typedef struct audproc_mbdrc_config_params_t audproc_mbdrc_config_params_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS parameters used
    by the MBDRC module. \n
    \n
    Following this structure is the payload for sub-band DRC configuration
    parameters (audproc_subband_drc_config_params_t). This sub-band structure
    must be repeated for each band.
*/
struct audproc_mbdrc_config_params_t
{
    uint16_t                  num_bands;
    /**< Number of bands.

         Supported values: 1 to 5 */

    int16_t                   limiter_threshold;
    /**< Threshold in decibels for the limiter output.
    
         Supported values: -72 to 18 \n
         Recommended value: 3994 (-0.22 db in Q3.12 format) */

    int16_t                   limiter_makeup_gain;
    /**< Makeup gain in decibels for the limiter output.
         
         Supported values: -42 to 42 \n
         Recommended value: 256 (0 dB in Q7.8 format) */

    int16_t                   limiter_gc;
    /**< Limiter gain recovery coefficient.
    
         Supported values: 0.5 to 0.99 \n
         Recommended value: 32440 (0.99 in Q15 format) */

    int16_t                   limiter_delay;
    /**< Limiter delay in samples.
    
         Supported values: 0 to 10 \n
         Recommended value: 262 (0.008 samples in Q15 format) */

    int16_t                   limiter_max_wait;
    /**< Maximum limiter waiting time in samples.
    
         Supported values: 0 to 10 \n
         Recommended value: 262 (0.008 samples in Q15 format)
         @newpage */
}
#include "adsp_end_pack.h"
;

/* DRC configuration structure for each sub-band of an MBDRC module. */
typedef struct audproc_subband_drc_config_params_t audproc_subband_drc_config_params_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS DRC configuration
    parameters for each sub-band in the MBDRC module.
    (See also audproc_mbdrc_config_params_t.)

    After this DRC structure is configured for valid bands, the next MBDRC
    setparams expects the sequence of sub-band MBDRC filter coefficients (the
    length depends on the number of bands) plus the mute flag for that band
    plus uint16 padding.
    
    @keep{10}
    The filter coefficient and mute flag are of type int16:
    - FIR coefficient = int16 firFilter
    - Mute flag = int16 fMuteFlag

    The sequence is as follows:
    - 1 band = 0 FIR coefficient + 1 mute flag + uint16 padding
    - 2 bands = 97 FIR coefficients + 2 mute flags + uint16 padding    
    - 3 bands = 97+33 FIR coefficients + 3 mute flags + uint16 padding    
    - 4 bands = 97+33+33 FIR coefficients + 4 mute flags + uint16 padding     
    - 5 bands = 97+33+33+33 FIR coefficients + 5 mute flags + uint16 padding

    For improved filterbank, the sequence is as follows:
    - 1 band = 0 FIR coefficient + 1 mute flag + uint16 padding
    - 2 bands = 141 FIR coefficients + 2 mute flags + uint16 padding    
    - 3 bands = 141+81 FIR coefficients + 3 mute flags + uint16 padding    
    - 4 bands = 141+81+61 FIR coefficients + 4 mute flags + uint16 padding     
    - 5 bands = 141+81+61+61 FIR coefficients + 5 mute flags + uint16 padding
*/
struct audproc_subband_drc_config_params_t
{
    int16_t                   drc_stereo_linked_flag;
    /**< Specifies whether all stereo channels have the same applied dynamics
         (1) or if they process their dynamics independently (0).

         Supported values:
         - 0 -- Not linked
         - 1 -- Linked */

    int16_t                   drc_mode;
    /**< Specifies whether DRC mode is bypassed for sub-bands.

         Supported values:
         - 0 -- Disabled
         - 1 -- Enabled */

    int16_t                   drc_down_sample_level;
    /**< DRC down sample level.

         Supported values: @ge 1
         @newpage */

    int16_t                   drc_delay;
    /**< DRC delay in samples.

         Supported values: 0 to 1200 */

    uint16_t                  drc_rms_time_avg_const;
    /**< RMS signal energy time-averaging constant.

         Supported values: 0 to 2^16-1 */

    uint16_t                  drc_makeup_gain;
    /**< DRC makeup gain in decibels.

         Supported values: 258 to 64917 */

    /* Down expander settings */

    int16_t                   down_expdr_threshold;
    /**< Down expander threshold.

         Supported Q7 format values: 1320 to up_cmpsr_threshold */

    int16_t                   down_expdr_slope;
    /**< Down expander slope.

         Supported Q8 format values: -32768 to 0. */

    uint32_t                  down_expdr_attack;
    /**< Down expander attack constant.

         Supported Q31 format values: 196844 to 2^31. */

    uint32_t                  down_expdr_release;
    /**< Down expander release constant.

         Supported Q31 format values: 19685 to 2^31 */

    uint16_t                  down_expdr_hysteresis;
    /**< Down expander hysteresis constant.

         Supported Q14 format values: 1 to 32690 */

    uint16_t                  reserved;
    /**< Clients must set this field to zero. */

    int32_t                   down_expdr_min_gain_db;
    /**< Down expander minimum gain.

         Supported Q23 format values: -805306368 to 0. */ 

    /* Up compressor settings */

    int16_t                   up_cmpsr_threshold;
    /**< Up compressor threshold.

         Supported Q7 format values: down_expdr_threshold to
         down_cmpsr_threshold. */ 

    uint16_t                  up_cmpsr_slope;
    /**< Up compressor slope.

         Supported Q16 format values: 0 to 64881. */ 

    uint32_t                  up_cmpsr_attack;
    /**< Up compressor attack constant.

         Supported Q31 format values: 196844 to 2^31. */ 

    uint32_t                  up_cmpsr_release;
    /**< Up compressor release constant.

         Supported Q31 format values: 19685 to 2^31. */ 

    uint16_t                  up_cmpsr_hysteresis;
    /**< Up compressor hysteresis constant.

         Supported Q14 format values: 1 to 32690. */

    /* Down compressor settings */

    int16_t                   down_cmpsr_threshold;
    /**< Down compressor threshold.

         Supported Q7 format values: up_cmpsr_threshold to 11560. */

    uint16_t                  down_cmpsr_slope;
    /**< Down compressor slope.

         Supported Q16 format values: 0 to 64881. */

    uint16_t                  reserved1;
    /**< Clients must set this field to zero. */

    uint32_t                  down_cmpsr_attack;
    /**< Down compressor attack constant.

         Supported Q31 format values: 196844 to 2^31. */

    uint32_t                  down_cmpsr_release;
    /**< Down compressor release constant.

         Supported Q31 format values: 19685 to 2^31. */

    uint16_t                  down_cmpsr_hysteresis;
    /**< Down compressor hysteresis constant.

         Supported Q14 values: 1 to 32690. */

    uint16_t                  reserved2;
    /**< Clients must set this field to zero.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup audio_pp_param_ids */

/** @ingroup audio_pp_module_ids
    ID of the Equalizer module postprocessing block.

    This module supports the following parameter ID:
    - #AUDPROC_PARAM_ID_EQUALIZER_PARAMETERS
*/
#define AUDPROC_MODULE_ID_EQUALIZER                                     0x00010C27

/** @addtogroup audio_pp_param_ids
@{ */
/** ID of the equalizer configuration parameters used by the
    #AUDPROC_MODULE_ID_EQUALIZER module.

    @messagepayload
    @structure{audproc_eq_params_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_EQUALIZER_PARAMETERS.tex}

    @parspace Per-band equalizer parameters
    @structure{audproc_eq_per_band_params_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_EQUALIZER_PARAMETERS_per_band_equalizer.tex}
*/
#define AUDPROC_PARAM_ID_EQUALIZER_PARAMETERS                           0x00010C28

/* Structure for equalizer module parameters. */
typedef struct audproc_eq_params_t audproc_eq_params_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_EQUALIZER_PARAMETERS parameters used
    by the Equalizer module. \n
    \n
    Following this structure is a sequence of per-band equalizer parameters
    (audproc_eq_per_band_params_t). The length is dependent on the num_bands value.
*/
struct audproc_eq_params_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether the equalizer module is disabled (0) or enabled
         (nonzero). */
    
    uint32_t                  num_bands;
    /**< Number of bands.

         Supported values: 1 to 12
         @newpage */
}
#include "adsp_end_pack.h"
;

/** @xreflabel{hdr:AsmParamEqTypeNone}
    No equalizer effect.
*/ 
#define AUDPROC_PARAM_EQ_TYPE_NONE                                       0

/** @xreflabel{hdr:AsmParamEqBassBoost}
    Bass boost equalizer effect.
*/ 
#define AUDPROC_PARAM_EQ_BASS_BOOST                                      1

/** @xreflabel{hdr:AsmParamEqBassCut}
    Bass cut equalizer effect.
*/ 
#define AUDPROC_PARAM_EQ_BASS_CUT                                        2

/** @xreflabel{hdr:AsmParamEqTrebleBoost}
    Treble boost equalizer effect.
*/ 
#define AUDPROC_PARAM_EQ_TREBLE_BOOST                                    3

/** @xreflabel{hdr:AsmParamEqTrebleCut}
    Treble cut equalizer effect.
*/ 
#define AUDPROC_PARAM_EQ_TREBLE_CUT                                      4

/** @xreflabel{hdr:AsmParamEqBandBoost}
    Band boost equalizer effect.
*/ 
#define AUDPROC_PARAM_EQ_BAND_BOOST                                      5 

/** @xreflabel{hdr:AsmParamEqBandCut}
    Band cut equalizer effect.
    @newpage
*/ 
#define AUDPROC_PARAM_EQ_BAND_CUT                                        6

/* Structure for per-band parameters for the equalizer module.
*/     
typedef struct audproc_eq_per_band_params_t audproc_eq_per_band_params_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_EQUALIZER_PARAMETERS per-band
    parameters used by the Equalizer module. (See also audproc_eq_params_t.)
*/ 
struct audproc_eq_per_band_params_t
{
    uint32_t                  band_idx;
    /**< Band index.

         Supported values: 0 to 11 */

    uint32_t                  filter_type;
    /**< Type of filter.

         Supported values:
         - #AUDPROC_PARAM_EQ_TYPE_NONE
         - #AUDPROC_PARAM_EQ_BASS_BOOST
         - #AUDPROC_PARAM_EQ_BASS_CUT
         - #AUDPROC_PARAM_EQ_TREBLE_BOOST
         - #AUDPROC_PARAM_EQ_TREBLE_CUT
         - #AUDPROC_PARAM_EQ_BAND_BOOST
         - #AUDPROC_PARAM_EQ_BAND_CUT */

    uint32_t                  center_freq_hz;
    /**< Filter band center frequency in Hertz. */

    int32_t                   filter_gain;
    /**< Filter band initial gain.

         Supported values: +12 to -12 dB in 1 dB increments */

    int32_t                   q_factor;
    /**< Filter band quality factor expressed as a Q8 number, i.e., a
         fixed-point number with q factor of 8. For example, 3000/(2^8).
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup audio_pp_param_ids */

/** @ingroup audio_pp_module_ids
    ID of the QConcertPlus module reverb block.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_ENABLE
    - #AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_PRESET
    - #AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_STRENGTH
    - #AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_OUTTYPE
    - #AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_GEOMETRY
    - #AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_DELAY
*/
#define AUDPROC_MODULE_ID_QCONCERT_PLUS_REVERB                          0x00010C20

/** @addtogroup audio_pp_param_ids
@{ */
/** ID of the QConcertPlus reverb enable parameter used by the
    #AUDPROC_MODULE_ID_QCONCERT_PLUS_REVERB module.
  
    @messagepayload
    @structure{audproc_qcpr_enable_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_ENABLE.tex}
*/
#define AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_ENABLE                    0x00010C21

/** ID of the QConcertPlus reverb preset parameter used by the
    #AUDPROC_MODULE_ID_QCONCERT_PLUS_REVERB module.

    @messagepayload
    @structure{audproc_qcpr_preset_config_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_PRESET.tex}
    @newpage
*/
#define AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_PRESET                    0x00010C22

/** ID of the QConcertPlus reverb strength parameter used by the
    #AUDPROC_MODULE_ID_QCONCERT_PLUS_REVERB module.

    @messagepayload
    @structure{audproc_qcpr_strength_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_STRENGTH.tex}
*/
#define AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_STRENGTH                  0x00010C23

/** ID of the QConcertPlus reverb output type parameter used by the
    #AUDPROC_MODULE_ID_QCONCERT_PLUS_REVERB module.
  
    @messagepayload
    @structure{audproc_qcpr_output_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_OUTTYPE.tex}
*/
#define AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_OUTTYPE                   0x00010C24

/** ID of the QConcertPlus speaker geometry parameter used by the
    #AUDPROC_MODULE_ID_QCONCERT_PLUS_REVERB module.

    @messagepayload
    @structure{audproc_qcpr_geo_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_GEOMETRY.tex}
    @newpage
*/
#define AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_GEOMETRY                  0x00010C25

/** ID of the QConcertPlus reverb delay parameter used by the
    #AUDPROC_MODULE_ID_QCONCERT_PLUS_REVERB module.

    @messagepayload
    @structure{audproc_qcpr_delay_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_DELAY.tex}
*/
#define AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_DELAY                     0x00010C26

/* Structure for the enable configuration parameter for
    QConcertPlus reverb enable. */
typedef struct audproc_qcpr_enable_t audproc_qcpr_enable_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_ENABLE parameter
    used by the QConcertPlus Reverb module.
*/
struct audproc_qcpr_enable_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether QConcertPlus reverb is disabled (0) or enabled
         (nonzero).
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Supported params for QConcertPlus preset */

/** @xreflabel{hdr:AsmParamQcprPresetSmallRoom}
    QConcertPlus small room preset.
*/ 
#define AUDPROC_PARAM_QCPR_PRESET_SMALLROOM                               0

/** @xreflabel{hdr:AsmParamQcprPresetMedRoom}
    QConcertPlus medium room preset.
*/ 
#define AUDPROC_PARAM_QCPR_PRESET_MEDIUMROOM                              1

/** @xreflabel{hdr:AsmParamQcprPresetMedHall}
    QConcertPlus medium hall preset.
*/
#define AUDPROC_PARAM_QCPR_PRESET_MEDIUMHALL                              2

/** @xreflabel{hdr:AsmParamQcprPresetConcertHall}
    QConcertPlus concert hall preset.
*/
#define AUDPROC_PARAM_QCPR_PRESET_CONCERTHALL                             3 

/** @xreflabel{hdr:AsmParamQcprPresetSurround}
    QConcertPlus concert surround preset.
*/
#define AUDPROC_PARAM_QCPR_PRESET_SURROUND                                4

/** @xreflabel{hdr:AsmParamQcprPresetWarmStage}
    QConcertPlus concert warm stage preset.
*/
#define AUDPROC_PARAM_QCPR_PRESET_WARMSTAGE                               5

/** @xreflabel{hdr:AsmParamQcprPresetCrystal}
    QConcertPlus concert crystal preset.
*/
#define AUDPROC_PARAM_QCPR_PRESET_CRYSTAL                                 6

/** @xreflabel{hdr:AsmParamQcprPresetLivingRoom}
    QConcertPlus concert living room preset.
*/
#define AUDPROC_PARAM_QCPR_PRESET_LIVINGROOM                              7

/* Structure for the QConcertPlus reverb preset. */
typedef struct audproc_qcpr_preset_config_t audproc_qcpr_preset_config_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_PRESET parameter
    used by the QConcertPlus Reverb module.
*/
struct audproc_qcpr_preset_config_t
{
    uint32_t                  preset;
    /**< Type of filter.

         Supported values:
         - #AUDPROC_PARAM_QCPR_PRESET_SMALLROOM
         - #AUDPROC_PARAM_QCPR_PRESET_MEDIUMROOM
         - #AUDPROC_PARAM_QCPR_PRESET_MEDIUMHALL
         - #AUDPROC_PARAM_QCPR_PRESET_CONCERTHALL
         - #AUDPROC_PARAM_QCPR_PRESET_SURROUND
         - #AUDPROC_PARAM_QCPR_PRESET_WARMSTAGE
         - #AUDPROC_PARAM_QCPR_PRESET_CRYSTAL
         - #AUDPROC_PARAM_QCPR_PRESET_LIVINGROOM
         @newpage */ 
}
#include "adsp_end_pack.h"
;

/* Supported params for QConcertPlus Strength */

/** @xreflabel{hdr:AsmParamQcprStrength1}
    QConcertPlus strength one.
*/ 
#define AUDPROC_PARAM_QCPR_STRENGTH_ONE                                   0

/** @xreflabel{hdr:AsmParamQcprStrength2}
    QConcertPlus strength two.
*/ 
#define AUDPROC_PARAM_QCPR_STRENGTH_TWO                                   1

/** @xreflabel{hdr:AsmParamQcprStrength3}
    QConcertPlus strength three.
*/ 
#define AUDPROC_PARAM_QCPR_STRENGTH_THREE                                 2

/** @xreflabel{hdr:AsmParamQcprStrength4}
    QConcertPlus strength four.
*/ 
#define AUDPROC_PARAM_QCPR_STRENGTH_FOUR                                  3

/** @xreflabel{hdr:AsmParamQcprStrength5}
    QConcertPlus strength five.
*/ 
#define AUDPROC_PARAM_QCPR_STRENGTH_FIVE                                  4

/** @xreflabel{hdr:AsmParamQcprStrength6}
    QConcertPlus strength six.
*/ 
#define AUDPROC_PARAM_QCPR_STRENGTH_SIX                                   5

/** @xreflabel{hdr:AsmParamQcprStrength7}
    QConcertPlus strength seven.
*/ 
#define AUDPROC_PARAM_QCPR_STRENGTH_SEVEN                                 6

/** @xreflabel{hdr:AsmParamQcprStrength8}
    QConcertPlus strength eight.
*/ 
#define AUDPROC_PARAM_QCPR_STRENGTH_EIGHT                                 7

/** @xreflabel{hdr:AsmParamQcprStrength9}
    QConcertPlus strength nine.
*/ 
#define AUDPROC_PARAM_QCPR_STRENGTH_NINE                                  8

/** @xreflabel{hdr:AsmParamQcprStrength10}
    QConcertPlus strength ten.
*/ 
#define AUDPROC_PARAM_QCPR_STRENGTH_TEN                                   9

/* Structure for the QConcertPlus reverb strength. */    
typedef struct audproc_qcpr_strength_t audproc_qcpr_strength_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_STRENGTH parameter
    used by the QConcertPlus Reverb module.
*/
struct audproc_qcpr_strength_t
{
    uint32_t                  strength;
    /**< QCPR strength.

         Supported values:
         - #AUDPROC_PARAM_QCPR_STRENGTH_ONE
         - #AUDPROC_PARAM_QCPR_STRENGTH_TWO
         - #AUDPROC_PARAM_QCPR_STRENGTH_THREE
         - #AUDPROC_PARAM_QCPR_STRENGTH_FOUR
         - #AUDPROC_PARAM_QCPR_STRENGTH_FIVE
         - #AUDPROC_PARAM_QCPR_STRENGTH_SIX
         - #AUDPROC_PARAM_QCPR_STRENGTH_SEVEN
         - #AUDPROC_PARAM_QCPR_STRENGTH_EIGHT
         - #AUDPROC_PARAM_QCPR_STRENGTH_NINE
         - #AUDPROC_PARAM_QCPR_STRENGTH_TEN         
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Supported params for QConcertPlus Output Type */

/** @xreflabel{hdr:AsmParamAudioOutputSpeakers}
    QConcertPlus output speakers.
*/ 
#define AUDPROC_PARAM_AUDIO_OUTPUT_SPEAKERS                               0

/** @xreflabel{hdr:AsmParamAudioOutputHeadphone}
    QConcertPlus output headphone.
*/ 
#define AUDPROC_PARAM_AUDIO_OUTPUT_HEADPHONE                              1
 
/* Structure for the QConcertPlus reverb output type. */
typedef struct audproc_qcpr_output_t audproc_qcpr_output_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_OUTTYPE parameter
    used by the QConcertPlus Reverb module.
*/
struct audproc_qcpr_output_t
{
    uint32_t                  output_type;
    /**< QCPR output type.

         Supported values:
         - #AUDPROC_PARAM_AUDIO_OUTPUT_SPEAKERS
         - #AUDPROC_PARAM_AUDIO_OUTPUT_HEADPHONE
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Supported params for QConcertPlus Geometry */

/** @xreflabel{hdr:AsmParamSpkoutGeomDesktop}
    QConcertPlus speaker geometry desktop.
*/ 
#define  AUDPROC_PARAM_SPKOUT_GEOMETRY_DESKTOP                            0

/** @xreflabel{hdr:AsmParamSpkoutGeomFront}
    QConcertPlus speaker geometry front.
*/ 
#define  AUDPROC_PARAM_SPKOUT_GEOMETRY_FRONT                              1

/** @xreflabel{hdr:AsmParamSpkoutGeomSide}
    QConcertPlus speaker geometry side.
    @newpage
*/ 
#define  AUDPROC_PARAM_SPKOUT_GEOMETRY_SIDE                               2

/* Structure for the QConcertPlus geometry. */
typedef struct audproc_qcpr_geo_t audproc_qcpr_geo_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_GEOMETRY parameter
    used by the QConcertPlus Reverb module.
*/
struct audproc_qcpr_geo_t
{
    uint32_t                  gemoetry;
    /**< QCPR speaker geometry type.

         Supported values:
         - #AUDPROC_PARAM_SPKOUT_GEOMETRY_DESKTOP
         - #AUDPROC_PARAM_SPKOUT_GEOMETRY_FRONT
         - #AUDPROC_PARAM_SPKOUT_GEOMETRY_SIDE
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for the QConcertPlus delay. */
typedef struct audproc_qcpr_delay_t audproc_qcpr_delay_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_DELAY parameter
    used by the QConcertPlus Reverb module.
*/
struct audproc_qcpr_delay_t
{
    int16_t                  delay;
    /**< Delay mode. */

    uint16_t                  reserved;
    /**< Clients must set this field to zero.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup audio_pp_param_ids */

/** @ingroup audio_pp_module_ids
    ID of the Psychoacoustic Bass Enhancement module.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_PBE_ENABLE
    - #AUDPROC_PARAM_ID_PBE_PARAMS_CONFIG
*/
#define AUDPROC_MODULE_ID_PBE                              0x00010C2A

/** @addtogroup audio_pp_param_ids
@{ */
/** ID of the PBE enable configuration parameter used by the
    #AUDPROC_MODULE_ID_PBE module. test

    @messagepayload
    @structure{audproc_pbe_enable_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_PBE_ENABLE.tex}
*/
#define AUDPROC_PARAM_ID_PBE_ENABLE                       0x00010C2B

/** ID of the PBE configuration parameters used by the
    #AUDPROC_MODULE_ID_PBE module.

    @messagepayload
    @structure{audproc_pbe_params_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_PBE_PARAMS_CONFIG.tex}
    @newpage
*/
#define AUDPROC_PARAM_ID_PBE_PARAMS_CONFIG                0x00010C49

/* Structure for the enable configuration parameter for PBE enable. */
typedef struct audproc_pbe_enable_t audproc_pbe_enable_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_PBE_ENABLE parameter used by the
    PBE module.
*/
struct audproc_pbe_enable_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether the PBE module is disabled (0) or enabled (nonzero).
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for PBE configuration parameters. */
typedef struct audproc_pbe_params_t audproc_pbe_params_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_PBE_PARAMS_CONFIG parameters used by
    the PBE module. \n
    \n
    This structure is followed by a structure that contains all the filter
    coefficients. The length is dependent on the number of bands: \n
    - Sequence of int32 Low Pass Filter (LPF) coefficients if uiXoverFltOrder
      equals 3 {length = 5 * 2}. Else {length = 5}.
    - Sequence of int32 High Pass Filter (HPF) coefficients if uiXoverFltOrder
      equals 3 {length = 5 * 2}. Else {length = 5}.
    - Sequence of int32 band pass filter coefficients if length is 
      5 * uiBandpassFltOrder. Each band has five coefficients, and each
      coefficient is in int32 format in the order of b0, b1, b2, a1, a2.
    - Sequence of int32 bass shelf filter coefficients of length 5. Each 
      coefficient is in int32 format in the order of b0, b1, b2, a1, a2. 
    - Sequence of int32 treble shelf filter coefficients of length 5. Each
      coefficient is in int32 format in the order of b0, b1, b2, a1, a2.
*/
struct audproc_pbe_params_t
{
    int16_t                   real_bass_mix_q15;
    /**< Mix amount of real bass harmonics and virtual bass harmonics.

         Supported values: 0.0 (all virtual) to 1.0 (all real) \n
         Default: 0.4 */

    int16_t                   bass_color_ctrl_q15;
    /**< Color control of virtual bass.
    
         Supported values: 0.0 (more even harmonics) to 1.0 (more odd
                           harmonics) \n
         Default: 0.3 */

    uint16_t                  main_chain_delay;
    /**< Sample delay in the non-bass path to compensate an algorithm delay in
         the bass path.
    
         Supported values: 0 to 32 \n
         Default: 15 */

    uint16_t                  xover_filter_order;
    /**< Low-pass and HPF order.

         Supported values: 1 to 3
         - 1 -- 6 dB/oct
         - 2 -- 12 dB/oct (Default)
         - 3 -- 18 dB/oct
         @newpage */

    uint16_t                  bpass_filter_order;
    /**< Band pass filter order.
    
         Supported values: 1 to 3
         - 1 -- 6 dB/oct
         - 2 -- 12 dB/oct (Default)
         - 3 -- 18 dB/oct */

    int16_t                   drc_delay;
    /**< Delay on the signal path to which to apply DRC gain.
    
         Supported values: 0.0 to 3.0 \n
         Delay: 2.0 */

    uint16_t                  rms_time_avg_ul16q16;
    /**< Short-term root mean square energy estimation time.
    
         Supported values: 0.0 to 3.0 \n
         Delay: 2.0 */

    int16_t                   expdr_threshold_l16q8;
    /**< Downward expansion threshold.
    
         Supported values: -80.0 to cmpsr_threashold_l16q7 \n
         Default: -35.0 */

    uint16_t                  expdr_slope_l16q8;
    /**< Downward expansion ratio.
    
         Supported values: 0.0 to 1.0 \n
         Default: 0.6 */

    int16_t                   cmpsr_threashold_l16q7;
    /**< Downward compression threshold.
    
         Supported values: expdr_threshold_l16q8 to 0.0 \n
         Default: -20.0
         @newpage */

    uint16_t                  cmpsr_slope_ul16q16;
    /**< Downward compression ratio.
    
         Supported values: 1.0 to 30.0 \n
         Default: 10.0 */

    uint16_t                  makeup_gain_ul16q12;
    /**< Makeup gain.
    
         Supported values: -18.0 to 18.0 \n
         Default: 6.0 */

    uint32_t                  cmpsr_attack_ul32q31;
    /**< Downward compression gain smooth attack time.
    
         Supported values: 0.0 to 500.0 \n
         Default: 10.0 */

    uint32_t                  cmpsr_release_ul32q31;
    /**< Downward compression gain smooth release time.
    
         Supported values: 0.0 to 5000.0 \n
         Default: 100.0 */

    uint32_t                  expdr_attack_ul32q31; 
    /**< Downward expansion gain smooth attack time.
    
         Supported values: 0.0 to 500.0 \n
         Default: 50.0 */

    uint32_t                  expdr_release_ul32q31;     
    /**< Downward expansion gain smooth release time.
    
         Supported values: 0.0 to 5000.0 \n
         Default: 50.0
         @newpage */

    int16_t                   limiter_bass_threshold_q12; 
    /**< Output level of bass content.
    
         Supported values: 0.0 to 48.0 \n
         Default: 27.0 */

    int16_t                   limiter_high_threshold_q12;
    /**< Output level of non-bass content.
    
         Supported values: 0.0 to 48.0 \n
         Default: 6.0 */

    int16_t                   limiter_bass_makeup_gain_q8;
    /**< Extra boost gain for bass content.
    
         Supported values: 0.0 to 1.0 \n
         Default: 0.99 */

    int16_t                   limiter_high_makeup_gain_q8;
    /**< Extra boost gain for non-bass content.
    
         Supported values: 0.0 to 1.0 \n
         Default: 0.99 */

    int16_t                   limiter_bass_grc_q15;
    /**< Limiter gain recovery constant for bass content.
    
         Supported values: 0.0 to 1.0 \n
         Default: 0.99 */

    int16_t                   limiter_high_grc_q15;
    /**< Limiter gain recovery constant for non-bass content.
    
         Supported values: 0.0 to 1.0 \n
         Default: 0.99
         @newpage */

    int16_t                   limiter_delay_q15; 
    /**< Limiter delay and maximum waiting time.
    
         Supported values: 0.0 to 10.0 \n
         Default: 10.0 */

    uint16_t                  reserved;
    /**< This field must be set to zero.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup audio_pp_param_ids */

/** @ingroup audio_pp_module_ids
    ID of the Spectrum Analyzer module.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_SPA_ENABLE
    - #AUDPROC_PARAM_ID_SPA_PARAMS_CONFIG
    @newpage
*/
#define AUDPROC_MODULE_ID_SPA                             0x00010C39

/** @addtogroup audio_pp_param_ids
@{ */
/** ID of the SPA enable parameter used by the #AUDPROC_MODULE_ID_SPA module.

    @parspace Message
    @structure{audproc_spa_enable_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_SPA_ENABLE.tex}
*/
#define AUDPROC_PARAM_ID_SPA_ENABLE                       0x00010C3A

/** ID of the SPA configuration parameters used by the
    #AUDPROC_MODULE_ID_SPA module.

    @messagepayload 
    @structure{audproc_spa_param_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_SPA_PARAMS_CONFIG.tex}
*/
#define AUDPROC_PARAM_ID_SPA_PARAMS_CONFIG                0x00010C3B

/* Structure for the enable configuration parameter for spectrum analyzer enable. */
typedef struct audproc_spa_enable_t audproc_spa_enable_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_SPA_ENABLE parameter used by the
    Spectrum Analyzer module.
*/
struct audproc_spa_enable_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether the spectrum analyzer is disabled (0) or enabled
         (nonzero).
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for the configuration parameters for the spectrum analyzer
    module. */
typedef struct audproc_spa_param_t audproc_spa_param_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_SPA_PARAMS_CONFIG parameters used by the
    Spectrum Analyzer module.
*/
struct audproc_spa_param_t
{
    uint32_t                  sample_interval;
    /**< Sample interval.

         Supported values: @ge 512 */

    uint16_t                  sample_points;
    /**< Supported FFT size.

         Supported values: 32, 64, 128, 256 */

    uint16_t                  reserved;
    /**< This field must be set to zero.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup audio_pp_param_ids */

/** @ingroup audio_pp_module_ids
    ID of the Peak Meter module.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_PEAK_INTERVAL
    - #AUDPROC_PARAM_ID_PEAK_VALUES
*/
#define AUDPROC_MODULE_ID_PEAK_METER            0x00010712 

/** @addtogroup audio_pp_param_ids
@{ */
/** ID of the peak meter configuration parameter used by the
    #AUDPROC_MODULE_ID_PEAK_METER module. This parameter indicates the update
    interval of the peak meter.

    @messagepayload 
    @structure{audproc_peakmeter_interval_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_PEAK_INTERVAL.tex}
*/
#define AUDPROC_PARAM_ID_PEAK_INTERVAL                  0x00010711

/** ID of the peak meter values parameters used by the
    #AUDPROC_MODULE_ID_PEAK_METER module.

    @messagepayload 
    @structure{audproc_peakmeter_values_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_PEAK_VALUES.tex}
*/
#define AUDPROC_PARAM_ID_PEAK_VALUES                    0x00010710

/** @xreflabel{hdr:PeakmeterMaxNumChs}
    Maximum number of channels that supports peak meter measurement.
    @newpage
*/
#define PEAKMETER_MAX_NUM_CHANNELS                  8

typedef struct audproc_peakmeter_interval_t audproc_peakmeter_interval_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_PEAK_INTERVAL parameter used by
    the Peak Meter module.
*/
struct audproc_peakmeter_interval_t
{
    uint32_t            sample_interval;
    /**<  Update time interval in milliseconds of the peak meter.

          Supported values: 0 to 0xFFFFFFFF
         
          A value of zero disables the peak meter. In this case, the aDSP will
          not report valid peak meter values. 

          A nonzero value indicates the peak meter log/reset frequency. For
          example, a value of 20 indicates an interval of 20 ms. This means
          a peak value is updated every 20 ms, and the newly calculated peak
          value replaces the existing value once every 20 ms.

          @note1hang The recommended sample interval is a multiple of 10 ms.
          @newpage */

} 
#include "adsp_end_pack.h"
;

typedef struct audproc_peakmeter_values_t audproc_peakmeter_values_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_PEAK_VALUES parameters used by the
    Peak Meter module. The peak meter values are returned by the aDSP.
*/
struct audproc_peakmeter_values_t
{
    uint32_t            num_supported_channels;
    /**<  Number of channels for which the peak value was calculated. A value
          of zero indicates that the peak meter is disabled.

          Supported values: 0 to #PEAKMETER_MAX_NUM_CHANNELS */

    int16_t             peak_values[PEAKMETER_MAX_NUM_CHANNELS];
    /**<  Array of peak values for each channel in the most recent window.

          Supported values: 0 to 32767
          @newpage */
} 
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup audio_pp_param_ids */

/** @ingroup audio_pp_module_ids
    ID of the Multiple-channel IIR Tuning Filter module.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_ENABLE
    - #AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_PREGAIN
    - #AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_CONFIG_PARAMS
*/
#define AUDPROC_MODULE_ID_MCHAN_IIR_TUNING_FILTER              0x0001031F

/** @addtogroup audio_pp_param_ids
@{ */
/** ID of the multiple-channel IIR tuning filter enable parameters used by the
    #AUDPROC_MODULE_ID_MCHAN_IIR_TUNING_FILTER module.

    @messagepayload 
    @structure{audproc_mchan_iir_enable_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_ENABLE.tex}

    @parspace Payload format of channel type/IIR enable flag pairs
    @structure{audproc_channel_type_iir_enable_pair_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_ENABLE_pairs.tex}
    @newpage
*/
#define AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_ENABLE        0x0001031C

/** @xreflabel{hdr:IirTuningFilterMaxCh}
    Maximum number of channels for the multiple-channel IIR tuning filter.
*/
#define IIR_TUNING_FILTER_MAX_CHANNELS                 8

/* Structure for holding one channel type - IIR enable pair. */
typedef struct audproc_channel_type_iir_enable_pair_t audproc_channel_type_iir_enable_pair_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_ENABLE
    channel type/IIR enable pairs used by the Multiple-channel IIR Tuning
    Filter module. \n
    \n
    This structure immediately follows the audproc_mchan_iir_enable_t
    structure.
*/
struct audproc_channel_type_iir_enable_pair_t
{
    uint8_t                   channel_type;
    /**< Channel type for which the IIR enable is to be applied.

         Supported values:
         - #PCM_CHANNEL_L
         - #PCM_CHANNEL_R
         - #PCM_CHANNEL_C
         - #PCM_CHANNEL_LS
         - #PCM_CHANNEL_RS
         - #PCM_CHANNEL_LFE
         - #PCM_CHANNEL_CS
         - #PCM_CHANNEL_LB
         - #PCM_CHANNEL_RB
         - #PCM_CHANNEL_TS
         - #PCM_CHANNEL_CVH
         - #PCM_CHANNEL_MS
         - #PCM_CHANNEL_FLC
         - #PCM_CHANNEL_FRC
         - #PCM_CHANNEL_RLC
         - #PCM_CHANNEL_RRC */

    uint8_t                   reserved1;
    /**< Clients must set this field to zero. */

    uint8_t                   reserved2;
    /**< Clients must set this field to zero. */

    uint8_t                   reserved3;
    /**< Clients must set this field to zero. */

    uint32_t                  enable_flag;
    /**< Specifies whether the channel IIR is disabled (0) or enabled (1).
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for the multichannel IIR enable command */
typedef struct audproc_mchan_iir_enable_t audproc_mchan_iir_enable_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_ENABLE
    parameters used by the Multiple-channel IIR Tuning Filter module.
*/
struct audproc_mchan_iir_enable_t
{
    uint32_t                  num_channels;
    /**< Number of channels for which enable flags are provided.

         Supported values: 0 to #IIR_TUNING_FILTER_MAX_CHANNELS */

    audproc_channel_type_iir_enable_pair_t     enable_flag_settings[IIR_TUNING_FILTER_MAX_CHANNELS];
    /**< Channel type/enable_flag value pairs.

         Supported values: See audproc_channel_type_iir_enable_pair_t
         @newpage */
}
#include "adsp_end_pack.h"
;

/** ID of the multiple-channel IIR tuning filter pregain parameters used by the
    #AUDPROC_MODULE_ID_MCHAN_IIR_TUNING_FILTER module.

    @messagepayload 
    @structure{audproc_mchan_iir_pregain_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_PREGAIN.tex}

    @parspace Payload format of channel type/IIR pregain pairs
    @structure{audproc_channel_type_iir_pregain_pair_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_PREGAIN_pairs.tex}
    @newpage
*/
#define AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_PREGAIN        0x0001031D

/* Structure for holding one channel type - IIR pregain pair. */
typedef struct audproc_channel_type_iir_pregain_pair_t audproc_channel_type_iir_pregain_pair_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_PREGAIN
    channel type/IIR pregain pairs used by the Multiple-channel IIR Tuning
    Filter module. \n
    \n
    This structure immediately follows the audproc_mchan_iir_pregain_
    t structure.
*/
struct audproc_channel_type_iir_pregain_pair_t
{
    uint8_t                   channel_type;
    /**< Channel type for which the IIR pregain is to be applied.

         Supported values:
         - #PCM_CHANNEL_L
         - #PCM_CHANNEL_R
         - #PCM_CHANNEL_C
         - #PCM_CHANNEL_LS
         - #PCM_CHANNEL_RS
         - #PCM_CHANNEL_LFE
         - #PCM_CHANNEL_CS
         - #PCM_CHANNEL_LB
         - #PCM_CHANNEL_RB
         - #PCM_CHANNEL_TS
         - #PCM_CHANNEL_CVH
         - #PCM_CHANNEL_MS
         - #PCM_CHANNEL_FLC
         - #PCM_CHANNEL_FRC
         - #PCM_CHANNEL_RLC
         - #PCM_CHANNEL_RRC */

    uint8_t                   reserved1;
    /**< Clients must set this field to zero. */

    uint8_t                   reserved2;
    /**< Clients must set this field to zero. */

    uint8_t                   reserved3;
    /**< Clients must set this field to zero. */

    int32_t                   preGain;
    /**< Pregain of IIR for this channel.

         Supported values: Any in Q27 format
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for the multichannel IIR preGain command */
typedef struct audproc_mchan_iir_pregain_t audproc_mchan_iir_pregain_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_PREGAIN
    parameters used by the Multiple-channel IIR Tuning Filter module.
*/
struct audproc_mchan_iir_pregain_t
{
    uint32_t                  num_channels;
    /**< Number of channels for which pregains are provided.

         Supported values: 0 to #IIR_TUNING_FILTER_MAX_CHANNELS */

    audproc_channel_type_iir_pregain_pair_t     preGain_settings[IIR_TUNING_FILTER_MAX_CHANNELS];
    /**< Channel type/pregain value pairs.

         Supported values: See audproc_channel_type_iir_pregain_pair_t
         @newpage */
}
#include "adsp_end_pack.h"
;

/** ID of the multiple-channel IIR tuning filter configuration parameters used
    by the #AUDPROC_MODULE_ID_MCHAN_IIR_TUNING_FILTER module.

    @messagepayload 
    @structure{audproc_mchan_iir_config_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_CONFIG_PARAMS.tex}
    @newpage

    @parspace Payload format of channel type/configuration pairs
    @structure{audproc_channel_type_iir_config_pair_t}
    @tablespace
    @inputtable{Audio_Postproc_AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_CONFIG_PARAMS_pairs.tex}
    @newpage
*/
#define AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_CONFIG_PARAMS             0x0001031E

/* Structure for the configuration parameter for multichannel IIR tuning filter module. */
typedef struct audproc_channel_type_iir_config_pair_t audproc_channel_type_iir_config_pair_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the
    #AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_CONFIG_PARAMS channel
    type/configuration pairs used by the Multi-Channel IIR Tuning Filter
    module. \n
    \n
    This structure is followed by the IIR filter coefficients: \n
    - Sequence of int32 FilterCoeffs\n
      Five coefficients for each band. Each coefficient is in int32 format,
      in the order of b0, b1, b2, a1, a2.
    - Sequence of int16 NumShiftFactor\n
      One int16 per band. The numerator shift factor is related to the Q
      factor of the filter coefficients b0, b1, b2. If the number of bands
      is odd, there must be an extra 16-bit padding by the end of the
      numerator shift factors. This extra 16-bit padding makes the entire
      structure 32-bit aligned. The padding bits must be all zeros.
*/
struct audproc_channel_type_iir_config_pair_t
{
    uint8_t                   channel_type;
    /**< Channel type for which the IIR configuration is to be applied.

         Supported values:
         - #PCM_CHANNEL_L
         - #PCM_CHANNEL_R
         - #PCM_CHANNEL_C
         - #PCM_CHANNEL_LS
         - #PCM_CHANNEL_RS
         - #PCM_CHANNEL_LFE
         - #PCM_CHANNEL_CS
         - #PCM_CHANNEL_LB
         - #PCM_CHANNEL_RB
         - #PCM_CHANNEL_TS
         - #PCM_CHANNEL_CVH
         - #PCM_CHANNEL_MS
         - #PCM_CHANNEL_FLC
         - #PCM_CHANNEL_FRC
         - #PCM_CHANNEL_RLC
         - #PCM_CHANNEL_RRC */

    uint8_t                   reserved;
    /**< Clients must set this field to zero. */

    uint16_t                  num_biquad_stages;
    /**< Number of biquad-IIR bands.

         Supported values: 0 to 20
         @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for the multichannel IIR config params */
typedef struct audproc_mchan_iir_config_t audproc_mchan_iir_config_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the 
    #AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_CONFIG_PARAMS parameters used by
    the Multi-Channel IIR Tuning Filter module. 
*/
struct audproc_mchan_iir_config_t
{
    uint32_t                  num_channels;
    /**< Number of channels for which enable flags are provided.

         Supported values: 0 to #IIR_TUNING_FILTER_MAX_CHANNELS

         This field is followed by sequences of 
         audproc_channel_type_iir_config_pair_t, int32 FilterCoeffs, and
         int16 NumShiftFactor. These data sequences must be one per channel.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup audio_pp_param_ids */

/** @ingroup audio_pp_topos
    @xreflabel{hdr:AdmCmdCoppOpenTopoIdSrsTrumedia}
    ID of the COPP topology for the SRS TruMedia module.
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SRS_TRUMEDIA_TOPOLOGY                 0x00010D90

/** @ingroup audio_pp_module_ids
    ID of the SRS TruMedia&tm;&nbsp;module.

    This module supports the following parameter IDs:
    - #APPI_SRS_TRUMEDIA_PARAMS
    - #APPI_SRS_TRUMEDIA_WOWHD_PARAMS
    - #APPI_SRS_TRUMEDIA_CSHP_PARAMS
    - #APPI_SRS_TRUMEDIA_HPF_PARAMS
    - #APPI_SRS_TRUMEDIA_PEQ_PARAMS
    - #APPI_SRS_TRUMEDIA_HL_PARAMS
    @newpage
*/
#define APPI_SRS_TRUMEDIA_MODULE_ID                   0x10005010

/** @addtogroup audio_pp_param_ids
@{ */

/** ID of the SRS Trumedia enable parameters used by the
    #APPI_SRS_TRUMEDIA_MODULE_ID module.

    @messagepayload 
    @structure{srs_TruMedia_params_t}
    @tablespace
    @inputtable{Audio_Postproc_APPI_SRS_TRUMEDIA_PARAMS.tex}
*/
#define APPI_SRS_TRUMEDIA_PARAMS                      0x10005011

/* The following are bit fields for the  Enable_Flags
 * field in srs_TruMedia_params_t structure defined below*/

/** @xreflabel{hdr:SrsEnableGlobal}
    Any SRS processing may occur. This bit must be set if any of the
    other enable macros are to be used. */
#define SRS_ENABLE_GLOBAL    0x00000001    

/** @xreflabel{hdr:SrsEnableWowhd}
    SRS WOW HD&tm;&nbsp;processing stage is active.
*/
#define SRS_ENABLE_WOWHD     0x00000002

/** @xreflabel{hdr:SrsEnableCshp}
    CS Headphone&tm;&nbsp;(CSHP) processing stage is active.
*/
#define SRS_ENABLE_CSHP      0x00000004

/** @xreflabel{hdr:SrsEnableHpf}  High Pass Filter stage is active. */
#define SRS_ENABLE_HPF       0x00000008

/** @xreflabel{hdr:SrsEnablePeq}  TruEQ&tm;&nbsp; stage is active. */
#define SRS_ENABLE_PEQ       0x00000010

/** @xreflabel{hdr:SrsEnableHl}  Hard Limiter stage is active. */
#define SRS_ENABLE_HL        0x00000020

typedef struct srs_TruMedia_params_t srs_TruMedia_params_t;

/** @brief Payload of the #APPI_SRS_TRUMEDIA_PARAMS parameters used by the SRS
    TruMedia module. \n
    \n
    This structure also contains a payload used to verify the SRS Licensing
    scheme with the aDSP module.
*/
#include "adsp_begin_pack.h"

struct srs_TruMedia_params_t {
   uint8_t                  Enable_Flags;    
   /**< Bit field of macros used to enable SRS Trumedia. The values are a
        bitwise OR of any of these macros. When a bit corresponding to a
        particular macro is set, the processing block associated with that
        macro is enabled.

        The bit corresponding to SRS_ENABLE_GLOBAL must be set if any of the
        other bits in the bit field are to be set. 

        Supported values:
        - #SRS_ENABLE_GLOBAL
        - #SRS_ENABLE_WOWHD
        - #SRS_ENABLE_CSHP
        - #SRS_ENABLE_HPF
        - #SRS_ENABLE_PEQ
        - #SRS_ENABLE_HL */

   uint8_t                  Reserved;
   /**< This field must be set to zero. */

   uint8_t                  I_0;
   /**< SRS licensing payload. */

   uint8_t                  V_0;
   /**< SRS licensing payload. */

   uint8_t                  I_1;
   /**< SRS licensing payload. */

   uint8_t                  V_1;
   /**< SRS licensing payload. */

   uint8_t                  I_2;
   /**< SRS licensing payload. */

    uint8_t                  V_2;
   /**< SRS licensing payload.
        @newpage */
}
#include "adsp_end_pack.h"
;

/** ID of the WOW HD configuration parameters used by the
    #APPI_SRS_TRUMEDIA_MODULE_ID module.

    @messagepayload 
    @structure{srs_TruMedia_wowhd_params_t}
    @tablespace
    @inputtable{Audio_Postproc_APPI_SRS_TRUMEDIA_WOWHD_PARAMS.tex}
*/
#define APPI_SRS_TRUMEDIA_WOWHD_PARAMS                0x10005012

/* The following are bit fields for the EnableFlags field in
 * srs_TruMedia_wowhd_params_t defined below*/

/** @xreflabel{hdr:SrsTrumediaWowhiDolimit}  Limiter is enabled. */
#define SRS_TRUMEDIA_WOWHD_DOLIMIT          0x0001

/** @xreflabel{hdr:SrsTrumediaWowhiDosrs3d}  SRS 3D is enabled. */
#define SRS_TRUMEDIA_WOWHD_DOSRS3D          0x0002

/** @xreflabel{hdr:SrsTrumediaWowhiDotb}  TruBass is enabled. */
#define SRS_TRUMEDIA_WOWHD_DOTB             0x0004

/** @xreflabel{hdr:SrsTrumediaWowhiDosatb}  Split-analysis TruBass is active. */
#define SRS_TRUMEDIA_WOWHD_DOSATB           0x0008

/** @xreflabel{hdr:SrsTrumediaWowhiDofocus}  Focus is enabled. */
#define SRS_TRUMEDIA_WOWHD_DOFOCUS          0x0010

/** @xreflabel{hdr:SrsTrumediaWowhiDodef}  Definition is enabled. */
#define SRS_TRUMEDIA_WOWHD_DODEF            0x0020

/** @xreflabel{hdr:SrsTrumediaWowhiTbmode}
    TruBass is stereo when set, and mono when not set.
*/
#define SRS_TRUMEDIA_WOWHD_TBMODE           0x0040

/** @xreflabel{hdr:SrsTrumediaWowhiSrstype}
    Headphone mode for SRS 3D is active when set.
*/
#define SRS_TRUMEDIA_WOWHD_SRSTYPE          0x0080

/** @xreflabel{hdr:SrsTrumediaWowhiSrsmodeMask}
    Defines the SRSMODE values.
*/
#define SRS_TRUMEDIA_WOWHD_SRSMODE_MASK     0x0F00

/** @xreflabel{hdr:SrsTrumediaWowhiSrsmodeMono}
    SRS 3D mode is SRSSrs3dMono.
*/
#define SRS_TRUMEDIA_WOWHD_SRSMODE_MONO     0x0100

/** @xreflabel{hdr:SrsTrumediaWowhiSrsmodeSingle}
    SRS 3D mode is SRSSrs3dSingleSpeaker.
*/
#define SRS_TRUMEDIA_WOWHD_SRSMODE_SINGLE   0x0200

/** @xreflabel{hdr:SrsTrumediaWowhiSrsmodeStereo}
    SRS 3D mode is SRSSrs3dStereo.
*/
#define SRS_TRUMEDIA_WOWHD_SRSMODE_STEREO   0x0300

/** @xreflabel{hdr:SrsTrumediaWowhiSrsmodeXtreme}
    SRS 3D mode is SRSSrs3dExtreme.
    @newpage
*/
#define SRS_TRUMEDIA_WOWHD_SRSMODE_XTREME   0x0400

typedef struct srs_TruMedia_wowhd_params_t srs_TruMedia_wowhd_params_t;


#include "adsp_begin_pack.h"
/** @brief Payload of #APPI_SRS_TRUMEDIA_WOWHD_PARAMS used by the SRS TruMedia
    module.
*/
struct srs_TruMedia_wowhd_params_t {
    uint32_t                EnableFlags;
   /**< Bit field of macros used to enable the SRS TruMedia WOW HD
        parameters. The values are a bitwise OR of any of these macros.
        When a bit corresponding to a particular macro is set, the
        processing block associated with that macro is enabled.

        Supported values:
        - #SRS_TRUMEDIA_WOWHD_DOLIMIT
        - #SRS_TRUMEDIA_WOWHD_DOSRS3D
        - #SRS_TRUMEDIA_WOWHD_DOTB
        - #SRS_TRUMEDIA_WOWHD_DOSATB
        - #SRS_TRUMEDIA_WOWHD_DOFOCUS
        - #SRS_TRUMEDIA_WOWHD_DODEF
        - #SRS_TRUMEDIA_WOWHD_TBMODE
        - #SRS_TRUMEDIA_WOWHD_SRSMODE_MASK
        - #SRS_TRUMEDIA_WOWHD_SRSMODE_MONO
        - #SRS_TRUMEDIA_WOWHD_SRSMODE_SINGLE
        - #SRS_TRUMEDIA_WOWHD_SRSMODE_STEREO
        - #SRS_TRUMEDIA_WOWHD_SRSMODE_XTREME  */

   uint16_t             Gain_Input;
   /**< Specifies the gain applied to the signal before processing.

        Supported values: Q15 format */

   uint16_t             Gain_Output;            
   /**< Specifies the gain applied to the signal after processing.

        Supported values: Q15 format */

   uint16_t             Gain_Bypass;
   /**< Inactive in this revision. */

   uint16_t             Focus_Level;
   /**< Specifies the level of focus effect.

        Supported values: Q15 format */

   uint16_t             Def_Level;
   /**< Specifies the level of definition effect.

        Supported values: Q15 format */

   uint16_t             Center_Level;
   /**< Specifies the SRS 3D Center control.

        Supported values: Q15 format */

   uint16_t             Space_Level;
   /**< Specifies the SRS 3D Space control.

        Supported values: Q15 format */

   uint16_t             Reserved;
   /**< This field must be set to zero. */

   uint32_t             Limit_Level;
   /**< Specifies the limiter's limit.

        Supported values: Q30 format */

   uint16_t             TruBass_Level;
   /**< Specifies the level of TruBass effect.

        Supported values: Q15 format */

   uint16_t             TruBass_Compress;
   /**< Specifies the TruBass compressor's level.

        Supported values: Q15 format */
    
   uint32_t             TruBass_Coefs[16];
   /**< TruBass coefficients. */
}
#include "adsp_end_pack.h"
;

/** ID of the CS Headphone configuration parameters used by the
    #APPI_SRS_TRUMEDIA_MODULE_ID module.

    @messagepayload 
    @structure{srs_TruMedia_cshp_params_t}
    @tablespace
    @inputtable{Audio_Postproc_APPI_SRS_TRUMEDIA_CSHP_PARAMS.tex}
    @newpage
*/
#define APPI_SRS_TRUMEDIA_CSHP_PARAMS                 0x10005013        

/* The following are bit fields for the EnableFlags field in
 * srs_TruMedia_cshp_params_t defined below*/

/** @xreflabel{hdr:SrsTrumediaCshpDolimit}  Limiter is enabled. */
#define SRS_TRUMEDIA_CSHP_DOLIMIT       0x0001

/** @xreflabel{hdr:SrsTrumediaCshpDodialog}  Dialog enhancement is enabled. */
#define SRS_TRUMEDIA_CSHP_DODIALOG      0x0002

/** @xreflabel{hdr:SrsTrumediaCshpDodotb}  TruBass is enabled. */
#define SRS_TRUMEDIA_CSHP_DOTB          0x0004

/** @xreflabel{hdr:SrsTrumediaCshpDosatb}  Split-Analysis TruBass is active. */
#define SRS_TRUMEDIA_CSHP_DOSATB        0x0008

/** @xreflabel{hdr:SrsTrumediaCshpDodecode}  Circle Surround<sup>&reg;</sup>
    (CS) decoder is enabled. */
#define SRS_TRUMEDIA_CSHP_DODECODE      0x0010

/** @xreflabel{hdr:SrsTrumediaCshpDecodemode}
    CS decoding is active when set; passive matrix decoding when not set. */
#define SRS_TRUMEDIA_CSHP_DECODEMODE    0x0020

/** @xreflabel{hdr:SrsTrumediaCshpDodef}  Definition is enabled. */
#define SRS_TRUMEDIA_CSHP_DODEF         0x0040

/** @xreflabel{hdr:SrsTrumediaCshpTbmode}
    TruBass is stereo when set, and mono when not set.
    @newpage
*/
#define SRS_TRUMEDIA_CSHP_TBMODE        0x0080

typedef struct srs_TruMedia_cshp_params_t srs_TruMedia_cshp_params_t;

#include "adsp_begin_pack.h"
/** @brief Payload of the #APPI_SRS_TRUMEDIA_CSHP_PARAMS parameters used by
    the SRS TruMedia module.
*/
struct srs_TruMedia_cshp_params_t {    
    uint32_t                EnableFlags;
   /**< Bit field of macros used to specify how SRS TruMedia CSHP is
        enabled. The values are a bitwise OR of any of these macros. When a
        bit corresponding to a particular macro is set, the processing block
        associated with that macro is enabled.

        Supported values:
        - #SRS_TRUMEDIA_CSHP_DOLIMIT
        - #SRS_TRUMEDIA_CSHP_DODIALOG
        - #SRS_TRUMEDIA_CSHP_DOTB
        - #SRS_TRUMEDIA_CSHP_DOSATB
        - #SRS_TRUMEDIA_CSHP_DODECODE
        - #SRS_TRUMEDIA_CSHP_DECODEMODE
        - #SRS_TRUMEDIA_CSHP_DODEF
        - #SRS_TRUMEDIA_CSHP_TBMODE */

   uint16_t             Gain_Input;
   /**< Specifies the gain applied to the signal before processing.

        Supported values: Q15 format */

   uint16_t             Gain_Output;
   /**< Specifies the gain applied to the signal after processing.

        Supported values: Q15 format*/

   uint16_t             Gain_Bypass;
   /**< Currently not used. */

   uint16_t             Dialog_Level;
   /**< Specifies the level of dialog enhancement.

        Supported values: Q15 format */

   uint16_t             Def_Level;
   /**< Specifies the level of definition effect.

        Supported values: Q15 format */

   uint16_t             Reserved;
   /**< This field must be set to zero. */

   uint32_t             Limit_Level;
   /**< Specifies the limiter's limit.

        Supported values: Q30 format */

   uint16_t             TruBass_Level;
   /**< Specifies the level of TruBass effect.

        Supported values: Q15 format */

   uint16_t             TruBass_Compress;
   /**< Specifies the TruBass compressor's level.

        Supported values: Q15 format */

   uint32_t             TruBass_Coefs[16];
   /**< TruBass coefficients.
        @newpage */
}
#include "adsp_end_pack.h"
;

/** ID of the high pass filter configuration parameters used by the
    #APPI_SRS_TRUMEDIA_MODULE_ID module.

    @messagepayload 
    @structure{srs_TruMedia_hpf_params_t}
    @tablespace
    @inputtable{Audio_Postproc_APPI_SRS_TRUMEDIA_HPF_PARAMS.tex}
*/
#define APPI_SRS_TRUMEDIA_HPF_PARAMS                  0x10005014

typedef struct srs_TruMedia_hpf_params_t srs_TruMedia_hpf_params_t;

#include "adsp_begin_pack.h"
/** @brief Payload of the #APPI_SRS_TRUMEDIA_HPF_PARAMS parameters used by the
    SRS TruMedia module.
*/
struct srs_TruMedia_hpf_params_t {
    uint32_t                Order;                      
   /**< Order of the filter. */

   uint32_t             Coefs[26];
   /**< High pass filter coefficients.
        @newpage  */
}
#include "adsp_end_pack.h"
;

/** ID of the Parametric Equalizer (PEQ) configuration parameters used by the
    #APPI_SRS_TRUMEDIA_MODULE_ID module.

    @messagepayload 
    @structure{srs_TruMedia_peq_params_t}
    @tablespace
    @inputtable{Audio_Postproc_APPI_SRS_TRUMEDIA_PEQ_PARAMS.tex}
    @newpage
*/
#define APPI_SRS_TRUMEDIA_PEQ_PARAMS                  0x10005015

/* The following are bit fields for the EnableFlags field in
 * srs_TruMedia_peq_params_t defined below*/

/** @xreflabel{hdr:SrsTrumediaTrueqDol}  Left channel is enabled. */
#define SRS_TRUMEDIA_TRUEQ_DOL        0x0001

/** @xreflabel{hdr:SrsTrumediaTrueqDor}  Right channel is enabled. */
#define SRS_TRUMEDIA_TRUEQ_DOR        0x0002

/** @xreflabel{hdr:SrsTrumediaTrueqLb0}  Left channel (band 0) is enabled. */
#define SRS_TRUMEDIA_TRUEQ_LB0        0x0010

/** @xreflabel{hdr:SrsTrumediaTrueqLb1}  Left channel (band 1) is enabled. */
#define SRS_TRUMEDIA_TRUEQ_LB1        0x0020

/** @xreflabel{hdr:SrsTrumediaTrueqLb2}  Left channel (band 2) is enabled. */
#define SRS_TRUMEDIA_TRUEQ_LB2        0x0040

/** @xreflabel{hdr:SrsTrumediaTrueqLb3}  Left channel (band 3) is enabled. */
#define SRS_TRUMEDIA_TRUEQ_LB3        0x0080

/** @xreflabel{hdr:SrsTrumediaTrueqRb0}  Right channel (band 0) is enabled. */
#define SRS_TRUMEDIA_TRUEQ_RB0        0x0100 

/** @xreflabel{hdr:SrsTrumediaTrueqRb1}  Right channel (band 1) is enabled. */
#define SRS_TRUMEDIA_TRUEQ_RB1        0x0200

/** @xreflabel{hdr:SrsTrumediaTrueqRb2}  Right channel (band 2) is enabled. */
#define SRS_TRUMEDIA_TRUEQ_RB2        0x0400

/** @xreflabel{hdr:SrsTrumediaTrueqRb3}  Right Channel (band 3) is enabled.
    @newpage
*/
#define SRS_TRUMEDIA_TRUEQ_RB3        0x0800

typedef struct srs_TruMedia_peq_params_t srs_TruMedia_peq_params_t;

#include "adsp_begin_pack.h"
/** @brief Payload of the #APPI_SRS_TRUMEDIA_PEQ_PARAMS parameters used by the
    SRS TruMedia module.
*/
struct srs_TruMedia_peq_params_t {    
   uint32_t             EnableFlags;
   /**< Bit field of macros used to enable the SRS TruMedia PEQ parameters.
        The values are a bitwise OR of any of these macros. When a bit
        corresponding to a particular macro is set, the processing block
        associated with that macro is enabled.

        Supported values:
        - #SRS_TRUMEDIA_TRUEQ_DOL
        - #SRS_TRUMEDIA_TRUEQ_DOR
        - #SRS_TRUMEDIA_TRUEQ_LB0
        - #SRS_TRUMEDIA_TRUEQ_LB1
        - #SRS_TRUMEDIA_TRUEQ_LB2
        - #SRS_TRUMEDIA_TRUEQ_LB3
        - #SRS_TRUMEDIA_TRUEQ_RB0
        - #SRS_TRUMEDIA_TRUEQ_RB1
        - #SRS_TRUMEDIA_TRUEQ_RB2
        - #SRS_TRUMEDIA_TRUEQ_RB3 */

   uint16_t             Gain_Input;
   /**< Specifies the gain applied to the signal before processing.

        Supported values: Q12 format */

   uint16_t             Gain_Output;
   /**< Specifies the gain applied to the signal after processing.

        Supported values: Q12 format */

   uint16_t             Gain_Bypass;
   /**< Currently not used. */

   uint16_t             Reserved;
   /**< This field must be set to zero. */

   uint32_t             L_Coefs[26];
   /**< Left-channel PEQ coefficients. */

   uint32_t             R_Coefs[26];
   /**< Left-channel PEQ coefficients.
        @newpage */
}
#include "adsp_end_pack.h"
;

/** ID of the hard limiter configuration parameters used by the
    #APPI_SRS_TRUMEDIA_MODULE_ID module.

    @messagepayload 
    @structure{srs_TruMedia_hl_params_t}
    @tablespace
    @inputtable{Audio_Postproc_APPI_SRS_TRUMEDIA_HL_PARAMS.tex}
    @newpage
*/
#define APPI_SRS_TRUMEDIA_HL_PARAMS                   0x10005016 

typedef struct srs_TruMedia_hl_params_t srs_TruMedia_hl_params_t;

#include "adsp_begin_pack.h"
/** @brief Payload of the #APPI_SRS_TRUMEDIA_HL_PARAMS parameters used by the
    SRS TruMedia module.
*/
struct srs_TruMedia_hl_params_t {
    uint16_t                Gain_Input;
   /**< Specifies the gain applied to the signal before processing.

        Supported values: Q13 format */

   uint16_t             Gain_Output;
   /**< Specifies the gain applied to the signal after processing.

        Supported values: Q13 format */

   uint16_t             Gain_Bypass;   
   /**< Currently not used. */

   uint16_t             Reserved_1;
   /**< This field must be set to zero. */

   int32_t              Delay_Len;
   /**< Number of samples in the delay buffer. */

   uint32_t             Boost;   
   /**< Specifies the gain applied to the signal before limiting.

        Supported values: Q23 format */

   uint16_t             Limit;
   /**< Specifies the limiter's limit.

        Supported values: Q15 format */

   uint16_t             Reserved_2;
   /**< This field must be set to zero. */

   uint32_t             DecaySmooth;
   /**< When nonzero, decay smoothing is enabled.

        Supported values: Any
        @newpage */
}
#include "adsp_end_pack.h"
;

/** @} */ /* end_addtogroup audio_pp_param_ids */
/** @ingroup audio_pp_module_ids
    ID of the DTS DialNorm module.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_DTS_DIALNORM_ENABLE
*/

#define AUDPROC_MODULE_ID_DTS_DIALNORM    0x0001071B

/** @addtogroup audio_pp_param_ids
@{ */
/** ID of the DTS DialNorm enable parameter used by the #AUDPROC_MODULE_ID_DTS_DIALNORM.

    @messagepayload
    @structure{audproc_dts_dialnorm_enable_t}
    @tablespace

*/
#define AUDPROC_PARAM_ID_DTS_DIALNORM_ENABLE    0x0001071C

typedef struct audproc_dts_dialnorm_enable_t audproc_dts_dialnorm_enable_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_DTS_DIALNORM_ENABLE
           enable_flag parameter used by the dialnorm module.
*/

struct audproc_dts_dialnorm_enable_t
{

    uint32_t                  enable_flag;
    /**< Specifies whether DTS DialNorm is disabled (0) or enabled (1).
         0 (default): disable
         1: enable

         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup audio_pp_param_ids */

/** @ingroup audio_pp_module_ids
    ID of the DTS DRC module.

    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_DTS_DRC_RATIO
*/

#define AUDPROC_MODULE_ID_DTS_DRC    0x0001071D

/** @addtogroup audio_pp_param_ids
@{ */
/** ID of the DTS DRC DRC_Ratio parameter used by the #AUDPROC_MODULE_ID_DTS_DRC.

    @messagepayload
    @structure{audproc_dts_drc_param_t}
    @tablespace

*/

#define AUDPROC_PARAM_ID_DTS_DRC_RATIO    0x0001071E

/* Structure for DTS DRC parameter DRC_Ratio. */

typedef struct audproc_dts_drc_param_t audproc_dts_drc_param_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #AUDPROC_PARAM_ID_DTS_DRC_RATIO
           drc_ratio parameter used by the DRC module.
*/

struct audproc_dts_drc_param_t
{

    uint32_t                  drc_ratio;
    /**< Specifies the DTS DRC_ratio used by the DRC.
        Supported values: 0  to 100  \n
        value greater than 4, then DRC is enabled else bypassed 

         @newpage */
}
#include "adsp_end_pack.h"
;

/* Module ID for the Resampler Module. This module does not
   have any parameters.
 */
#define AUDPROC_MODULE_ID_RESAMPLER   0x00010719

/* Module ID for the Downmixer Module. This module does not
   have any parameters.
 */
#define AUDPROC_MODULE_ID_DOWNMIX     0x00010721

/* Module ID for the sample slipping/stuffing Module. This
   module does not have any parameters.
 */
#define AUDPROC_MODULE_ID_SAMPLESLIP  0x00010722

/** @} */ /* end_addtogroup audio_pp_param_ids */

typedef struct audproc_module_info_t audproc_module_info_t;

/** @brief The per module structure used by the structure
     audproc_topology_definition_t. */
struct audproc_module_info_t
{
   uint32_t module_id;
   /**< Specifies the module id.

        Supported values: Any valid module id. */
   uint32_t use_lpm;
   /**< Specifies whether to put this module in low power memory.
        If enough low power memory is not available, this module
        will be placed in the regular memory pool.

        Supported values: 0 - Do not put this module in LPM.
                          1 - Put this module in LPM. */
   uint8_t  init_params[16];
   /**< The initialization parameters passed to the module
        at creation time. The format is module dependent.

        Supported values: Module specific. */
};

typedef struct audproc_topology_definition_t audproc_topology_definition_t;

/** The maximum number of modules supported in a topology.
*/
#define AUDPROC_MAX_MODULES_PER_TOPOLOGY 16

/** @brief The per topology structure used to describe a
     custom topology.*/
struct audproc_topology_definition_t
{
   uint32_t topology_id;
   /**< Specifies the topology id of this custom topology.

        Supported values: 0x10000000-0x1FFFFFFF. */
   uint32_t num_modules;
   /**< Specifies the number of modules in this topology.

        Supported values: 0 to AUDPROC_MAX_MODULES_PER_TOPOLOGY. */
   audproc_module_info_t module_info[AUDPROC_MAX_MODULES_PER_TOPOLOGY];
   /**< Gives the information about the modules in this topology. */
};

typedef struct audproc_custom_topologies_t audproc_custom_topologies_t;

/** @brief The header structure for the custom topologies
     command buffer. The buffer contains this structure followed
     by num_topologies instances of audproc_topology_definition_t. */
struct audproc_custom_topologies_t
{
   uint32_t num_topologies;
   /**< Specifies the number of topologies in this command.

        Supported values: Any nonzero number. */
};

// The following names have been deprecated. Please use the new names.
#define ADM_MODULE_ID_EANS AUDPROC_MODULE_ID_EANS
#define ADM_PARAM_ID_EANS_ENABLE AUDPROC_PARAM_ID_EANS_ENABLE
#define ADM_PARAM_ID_EANS_PARAMS AUDPROC_PARAM_ID_EANS_PARAMS
#define ADM_MODULE_ID_TX_MIC_GAIN_CTRL AUDPROC_MODULE_ID_TX_MIC_GAIN_CTRL
#define ADM_PARAM_ID_TX_MIC_GAIN AUDPROC_PARAM_ID_TX_MIC_GAIN
#define ADM_MODULE_ID_RX_CODEC_GAIN_CTRL AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL
#define ADM_PARAM_ID_RX_CODEC_GAIN AUDPROC_PARAM_ID_RX_CODEC_GAIN
#define ADM_MODULE_ID_HPF_IIR_TX_FILTER AUDPROC_MODULE_ID_HPF_IIR_TX_FILTER
#define ADM_PARAM_ID_HPF_IIR_TX_FILTER_ENABLE_CONFIG AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_ENABLE_CONFIG
#define ADM_PARAM_ID_HPF_IIR_TX_FILTER_PRE_GAIN AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_PRE_GAIN
#define ADM_PARAM_ID_HPF_IIR_TX_FILTER_CONFIG_PARAMS AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_CONFIG_PARAMS
#define ADM_MODULE_ID_TX_IIR_FILTER AUDPROC_MODULE_ID_TX_IIR_FILTER
#define ADM_MODULE_ID_LEFT_IIR_TUNING_FILTER AUDPROC_MODULE_ID_LEFT_IIR_TUNING_FILTER
#define ADM_MODULE_ID_RIGHT_IIR_TUNING_FILTER AUDPROC_MODULE_ID_RIGHT_IIR_TUNING_FILTER
#define ADM_PARAM_ID_TX_IIR_FILTER_ENABLE_CONFIG AUDPROC_PARAM_ID_TX_IIR_FILTER_ENABLE_CONFIG
#define ADM_PARAM_ID_TX_IIR_FILTER_PRE_GAIN AUDPROC_PARAM_ID_TX_IIR_FILTER_PRE_GAIN
#define ADM_PARAM_ID_TX_IIR_FILTER_CONFIG_PARAMS AUDPROC_PARAM_ID_TX_IIR_FILTER_CONFIG_PARAMS
#define ADM_MODULE_ID_QENSEMBLE AUDPROC_MODULE_ID_QENSEMBLE
#define ADM_PARAM_ID_QENSEMBLE_ENABLE AUDPROC_PARAM_ID_QENSEMBLE_ENABLE
#define ADM_PARAM_ID_QENSEMBLE_BACKGAIN AUDPROC_PARAM_ID_QENSEMBLE_BACKGAIN
#define ADM_PARAM_ID_QENSEMBLE_SET_NEW_ANGLE AUDPROC_PARAM_ID_QENSEMBLE_SET_NEW_ANGLE
#define ASM_MODULE_ID_VOL_CTRL AUDPROC_MODULE_ID_VOL_CTRL
#define ASM_PARAM_ID_VOL_CTRL_MASTER_GAIN AUDPROC_PARAM_ID_VOL_CTRL_MASTER_GAIN
#define ASM_PARAM_ID_VOL_CTRL_LR_CHANNEL_GAIN AUDPROC_PARAM_ID_VOL_CTRL_LR_CHANNEL_GAIN
#define ASM_PARAM_ID_VOL_CTRL_MUTE_CONFIG AUDPROC_PARAM_ID_VOL_CTRL_MUTE_CONFIG
#define ASM_PARAM_ID_SOFT_VOL_STEPPING_PARAMETERS AUDPROC_PARAM_ID_SOFT_VOL_STEPPING_PARAMETERS
#define ASM_PARAM_ID_SOFT_PAUSE_PARAMETERS AUDPROC_PARAM_ID_SOFT_PAUSE_PARAMETERS
#define ASM_PARAM_ID_MULTICHANNEL_GAIN AUDPROC_PARAM_ID_MULTICHANNEL_GAIN
#define ASM_PARAM_ID_MULTICHANNEL_MUTE AUDPROC_PARAM_ID_MULTICHANNEL_MUTE
#define ASM_PARAM_SVC_RAMPINGCURVE_LINEAR AUDPROC_PARAM_SVC_RAMPINGCURVE_LINEAR
#define ASM_PARAM_SVC_RAMPINGCURVE_EXP AUDPROC_PARAM_SVC_RAMPINGCURVE_EXP
#define ASM_PARAM_SVC_RAMPINGCURVE_LOG AUDPROC_PARAM_SVC_RAMPINGCURVE_LOG
#define ASM_MODULE_ID_IIR_TUNING_FILTER AUDPROC_MODULE_ID_IIR_TUNING_FILTER
#define ASM_PARAM_ID_IIR_TUNING_FILTER_ENABLE_CONFIG AUDPROC_PARAM_ID_IIR_TUNING_FILTER_ENABLE_CONFIG
#define ASM_PARAM_ID_IIR_TUNING_FILTER_PRE_GAIN AUDPROC_PARAM_ID_IIR_TUNING_FILTER_PRE_GAIN
#define ASM_PARAM_ID_IIR_TUNING_FILTER_CONFIG_PARAMS AUDPROC_PARAM_ID_IIR_TUNING_FILTER_CONFIG_PARAMS
#define ASM_MODULE_ID_MBDRC AUDPROC_MODULE_ID_MBDRC
#define ASM_PARAM_ID_MBDRC_ENABLE AUDPROC_PARAM_ID_MBDRC_ENABLE
#define ASM_PARAM_ID_MBDRC_CONFIG_PARAMS AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS
#define ASM_MODULE_ID_MBDRCV2 AUDPROC_MODULE_ID_MBDRCV2
#define ASM_PARAM_ID_MBDRC_CONFIG_PARAMS_IMPROVED_FILTBANK_V2 AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS_IMPROVED_FILTBANK_V2
#define ASM_MODULE_ID_EQUALIZER AUDPROC_MODULE_ID_EQUALIZER
#define ASM_PARAM_ID_EQUALIZER_PARAMETERS AUDPROC_PARAM_ID_EQUALIZER_PARAMETERS
#define ASM_PARAM_EQ_TYPE_NONE AUDPROC_PARAM_EQ_TYPE_NONE
#define ASM_PARAM_EQ_BASS_BOOST AUDPROC_PARAM_EQ_BASS_BOOST
#define ASM_PARAM_EQ_BASS_CUT AUDPROC_PARAM_EQ_BASS_CUT
#define ASM_PARAM_EQ_TREBLE_BOOST AUDPROC_PARAM_EQ_TREBLE_BOOST
#define ASM_PARAM_EQ_TREBLE_CUT AUDPROC_PARAM_EQ_TREBLE_CUT
#define ASM_PARAM_EQ_BAND_BOOST AUDPROC_PARAM_EQ_BAND_BOOST
#define ASM_PARAM_EQ_BAND_CUT AUDPROC_PARAM_EQ_BAND_CUT
#define ASM_MODULE_ID_QCONCERT_PLUS_REVERB AUDPROC_MODULE_ID_QCONCERT_PLUS_REVERB
#define ASM_PARAM_ID_QCONCERT_PLUS_REVERB_ENABLE AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_ENABLE
#define ASM_PARAM_ID_QCONCERT_PLUS_REVERB_PRESET AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_PRESET
#define ASM_PARAM_ID_QCONCERT_PLUS_REVERB_STRENGTH AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_STRENGTH
#define ASM_PARAM_ID_QCONCERT_PLUS_REVERB_OUTTYPE AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_OUTTYPE
#define ASM_PARAM_ID_QCONCERT_PLUS_REVERB_GEOMETRY AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_GEOMETRY
#define ASM_PARAM_ID_QCONCERT_PLUS_REVERB_DELAY AUDPROC_PARAM_ID_QCONCERT_PLUS_REVERB_DELAY
#define ASM_PARAM_QCPR_PRESET_SMALLROOM AUDPROC_PARAM_QCPR_PRESET_SMALLROOM
#define ASM_PARAM_QCPR_PRESET_MEDIUMROOM AUDPROC_PARAM_QCPR_PRESET_MEDIUMROOM
#define ASM_PARAM_QCPR_PRESET_MEDIUMHALL AUDPROC_PARAM_QCPR_PRESET_MEDIUMHALL
#define ASM_PARAM_QCPR_PRESET_CONCERTHALL AUDPROC_PARAM_QCPR_PRESET_CONCERTHALL
#define ASM_PARAM_QCPR_PRESET_SURROUND AUDPROC_PARAM_QCPR_PRESET_SURROUND
#define ASM_PARAM_QCPR_PRESET_WARMSTAGE AUDPROC_PARAM_QCPR_PRESET_WARMSTAGE
#define ASM_PARAM_QCPR_PRESET_CRYSTAL AUDPROC_PARAM_QCPR_PRESET_CRYSTAL
#define ASM_PARAM_QCPR_PRESET_LIVINGROOM AUDPROC_PARAM_QCPR_PRESET_LIVINGROOM
#define ASM_PARAM_QCPR_STRENGTH_ONE AUDPROC_PARAM_QCPR_STRENGTH_ONE
#define ASM_PARAM_QCPR_STRENGTH_TWO AUDPROC_PARAM_QCPR_STRENGTH_TWO
#define ASM_PARAM_QCPR_STRENGTH_THREE AUDPROC_PARAM_QCPR_STRENGTH_THREE
#define ASM_PARAM_QCPR_STRENGTH_FOUR AUDPROC_PARAM_QCPR_STRENGTH_FOUR
#define ASM_PARAM_QCPR_STRENGTH_FIVE AUDPROC_PARAM_QCPR_STRENGTH_FIVE
#define ASM_PARAM_QCPR_STRENGTH_SIX AUDPROC_PARAM_QCPR_STRENGTH_SIX
#define ASM_PARAM_QCPR_STRENGTH_SEVEN AUDPROC_PARAM_QCPR_STRENGTH_SEVEN
#define ASM_PARAM_QCPR_STRENGTH_EIGHT AUDPROC_PARAM_QCPR_STRENGTH_EIGHT
#define ASM_PARAM_QCPR_STRENGTH_NINE AUDPROC_PARAM_QCPR_STRENGTH_NINE
#define ASM_PARAM_QCPR_STRENGTH_TEN AUDPROC_PARAM_QCPR_STRENGTH_TEN
#define ASM_PARAM_AUDIO_OUTPUT_SPEAKERS AUDPROC_PARAM_AUDIO_OUTPUT_SPEAKERS
#define ASM_PARAM_AUDIO_OUTPUT_HEADPHONE AUDPROC_PARAM_AUDIO_OUTPUT_HEADPHONE
#define ASM_PARAM_SPKOUT_GEOMETRY_DESKTOP AUDPROC_PARAM_SPKOUT_GEOMETRY_DESKTOP
#define ASM_PARAM_SPKOUT_GEOMETRY_FRONT AUDPROC_PARAM_SPKOUT_GEOMETRY_FRONT
#define ASM_PARAM_SPKOUT_GEOMETRY_SIDE AUDPROC_PARAM_SPKOUT_GEOMETRY_SIDE
#define ASM_PARAM_ID_PBE AUDPROC_MODULE_ID_PBE
#define ASM_PARAM_ID_PBE_ENABLE AUDPROC_PARAM_ID_PBE_ENABLE
#define ASM_PARAM_ID_PBE_PARAMS_CONFIG AUDPROC_PARAM_ID_PBE_PARAMS_CONFIG
#define ASM_MODULE_ID_SPA AUDPROC_MODULE_ID_SPA
#define ASM_PARAM_ID_SPA_ENABLE AUDPROC_PARAM_ID_SPA_ENABLE
#define ASM_PARAM_ID_SPA_PARAMS_CONFIG AUDPROC_PARAM_ID_SPA_PARAMS_CONFIG

typedef audproc_eans_enable_t adm_eans_enable_t;
typedef audproc_eans_params_t adm_eans_params_t;
typedef audproc_tx_mic_gain_t adm_tx_mic_gain_t;
typedef audproc_rx_codec_gain_t adm_rx_codec_gain_t;
typedef audproc_hpf_tx_iir_filter_enable_cfg_t adm_hpf_tx_iir_filter_enable_cfg_t;
typedef audproc_hpf_tx_iir_filter_pre_gain_t adm_hpf_tx_iir_filter_pre_gain_t;
typedef audproc_hpf_tx_iir_filter_cfg_params_t adm_hpf_tx_iir_filter_cfg_params_t;
typedef audproc_tx_iir_filter_enable_cfg_t adm_tx_iir_filter_enable_cfg_t;
typedef audproc_tx_iir_filter_pre_gain_t adm_tx_iir_filter_pre_gain_t;
typedef audproc_tx_iir_filter_cfg_params_t adm_tx_iir_filter_cfg_params_t;
typedef audproc_qensemble_enable_t adm_qensemble_enable_t;
typedef audproc_qensemble_param_backgain_t adm_qensemble_param_backgain_t;
typedef audproc_qensemble_param_set_new_angle_t adm_qensemble_param_set_new_angle_t;
typedef audproc_volume_ctrl_master_gain_t asm_volume_ctrl_master_gain_t;
typedef audproc_volume_ctrl_lr_chan_gain_t asm_volume_ctrl_lr_chan_gain_t;
typedef audproc_volume_ctrl_mute_config_t asm_volume_ctrl_mute_config_t;
typedef audproc_soft_step_volume_params_t asm_soft_step_volume_params_t;
typedef audproc_soft_pause_params_t asm_soft_pause_params_t;
typedef audproc_volume_ctrl_channel_type_gain_pair_t asm_volume_ctrl_channel_type_gain_pair_t;
typedef audproc_volume_ctrl_multichannel_gain_t asm_volume_ctrl_multichannel_gain_t;
typedef audproc_volume_ctrl_channel_type_mute_pair_t asm_volume_ctrl_channel_type_mute_pair_t;
typedef audproc_volume_ctrl_multichannel_mute_t asm_volume_ctrl_multichannel_mute_t;
typedef audproc_iir_tuning_filter_enable_t asm_iir_tuning_filter_enable_t;
typedef audproc_iir_tuning_filter_pregain_t asm_iir_tuning_filter_pregain_t;
typedef audproc_iir_filter_config_params_t asm_iir_filter_config_params_t;
typedef audproc_mbdrc_enable_t asm_mbdrc_enable_t;
typedef audproc_mbdrc_config_params_t asm_mbdrc_config_params_t;
typedef audproc_subband_drc_config_params_t asm_subband_drc_config_params_t;
typedef audproc_eq_params_t asm_eq_params_t;
typedef audproc_eq_per_band_params_t asm_eq_per_band_params_t;
typedef audproc_qcpr_enable_t asm_qcpr_enable_t;
typedef audproc_qcpr_preset_config_t asm_qcpr_preset_config_t;
typedef audproc_qcpr_strength_t asm_qcpr_strength_t;
typedef audproc_qcpr_output_t asm_qcpr_output_t;
typedef audproc_qcpr_geo_t asm_qcpr_geo_t;
typedef audproc_qcpr_delay_t asm_qcpr_delay_t;
typedef audproc_pbe_enable_t asm_pbe_enable_t;
typedef audproc_pbe_params_t asm_pbe_params_t;
typedef audproc_spa_enable_t asm_spa_enable_t;
typedef audproc_spa_param_t asm_spa_param_t;

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_AUDPROC_API_H_ */
