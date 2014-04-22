/*========================================================================*/
/**
@file adsp_adm_api.h

This file contains the structure definitions for Audio Device Manager (ADM) 
commands used in client-ADM communications.
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

$Header: //components/rel/audio.tx/2.0/audio/api/avsaudio/inc/adsp_adm_api.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/15/12   KR      Non-backward compatible API changes for Badger.
02/07/12   KK      Memory map changes for Badger.
02/06/12   KK      Branched to dev/avs2.0 for API development.

========================================================================== */

#ifndef _ADSP_ADM_API_H_
#define _ADSP_ADM_API_H_

#include "mmdefs.h"
#include "adsp_audio_memmap_api.h"
#include "adsp_audproc_api.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


/** @ingroup versions
    Hexagon ADM version ID.
*/
#define ADSP_ADM_VERSION    0x00070000

/** @ingroup adm_cmd_shared_mem_map_regions_t
    Commands the ADM to map multiple shared memory regions.

    @relevant_apr_hdr_fields
      Opcode -- ADM_CMD_SHARED_MEM_MAP_REGIONS \n
      Dst_port -- 0000 (Session ID/Stream ID = 0/0)

    @apr_msg_payload
      @structure{avs_cmd_shared_mem_map_regions_t}
      @tablespace
      @inputtable{ADM_CMD_SHARED_MEM_MAP_REGIONS_APR_msg_payload.tex}

    @mappayload
      @structure{avs_shared_map_region_payload_t}
      @tablespace
      @inputtable{ADM_CMD_SHARED_MEM_MAP_REGIONS_MapPayload.tex}

    @prerequisites
      All regions to be mapped must be from the same memory pool.

    @return
      ADM_CMDRSP_SHARED_MEM_MAP_REGIONS (refer to @xhyperref{Q3,[Q3]}).
*/
#define ADM_CMD_SHARED_MEM_MAP_REGIONS               0x00010322

/* ADM memory map regions command structure is the same as the
    avs memory map regions command structure. */
typedef struct  avs_cmd_shared_mem_map_regions_t adm_cmd_shared_mem_map_regions_t;

/** @ingroup adm_cmdrsp_shared_mem_map_regions_t
    response sent by ADM to the client in response to the ADM_CMD_SHARED_MEM_MAP_REGIONS command.

    @relevant_apr_hdr_fields
      Opcode -- ADM_CMDRSP_SHARED_MEM_MAP_REGIONS \n

    @apr_msg_payload
      @structure{avs_cmdrsp_shared_mem_map_regions_t}
      @tablespace
      @inputtable{ADM_CMDRSP_SHARED_MEM_MAP_REGIONS_APR_msg_payload.tex}

    @prerequisites
      ADM_CMD_SHARED_MEM_MAP_REGIONS cmd must be sent to receive this response from ADM
*/
#define ADM_CMDRSP_SHARED_MEM_MAP_REGIONS               0x00010323

/* ADM memory map regions response structure is the same as the
    avs memory map regions response structure. */
typedef struct  avs_cmdrsp_shared_mem_map_regions_t adm_cmdrsp_shared_mem_map_regions_t;

/** @ingroup adm_cmd_shared_mem_unmap_regions_t
    Commands the ADM to unmap multiple shared memory regions that were
    previously mapped.

    @relevant_apr_hdr_fields
      Opcode -- ADM_CMD_SHARED_MEM_UNMAP_REGIONS \n
      Dst_port -- 0000 (Session ID/Stream ID = 0/0)

    @apr_msg_payload
      @structure{avs_cmd_shared_mem_unmap_regions_t}
      @tablespace
      @inputtable{ADM_CMD_SHARED_MEM_UNMAP_REGIONS_APR_msg_payload.tex}

    @prerequisites
      None.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
*/
#define ADM_CMD_SHARED_MEM_UNMAP_REGIONS              0x00010324

/* ADM memory unmap regions command structure is the same as the
    avs memory unmap regions command structure. */
typedef struct avs_cmd_shared_mem_unmap_regions_t adm_cmd_shared_mem_unmap_regions_t;

/** @addtogroup adm_cmd_update_matrix_map_v5
@{ */
/** This command allows clients to provide updates to the matrix mapping between session IDs and COPP IDs.

    @relevant_apr_hdr_fields
      Opcode -- ADM_CMD_MATRIX_MAP_ROUTINGS_V5 \n
      Dst_port -- Ignored

    @apr_msg_payload
      @structure{adm_cmd_matrix_map_routings_v5_t}
      @tablespace
      @inputtable{ADM_CMD_MATRIX_MAP_ROUTINGS_V5_APR_msg_payload.tex}

    @prerequisites
      All specified session and COPP IDs must be valid and connected to the
      targeted matrix.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

    @parspace Session map node message payload
      @structure{adm_session_map_node_v5_t}
      @tablespace
      @inputtable{ADM_CMD_MATRIX_MAP_ROUTINGS_V5_SessionMapNode_msg_payload.tex}
*/
#define ADM_CMD_MATRIX_MAP_ROUTINGS_V5                                 0x00010325

/** Enumeration for an audio Rx matrix ID.*/
#define ADM_MATRIX_ID_AUDIO_RX              0

/** Enumeration for an audio Tx matrix ID.*/
#define ADM_MATRIX_ID_AUDIO_TX              1

/* Payload of an ADM_CMD_MATRIX_MAP_ROUTINGS_V5 command. */
typedef struct adm_cmd_matrix_map_routings_v5_t adm_cmd_matrix_map_routings_v5_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ADM_CMD_MATRIX_MAP_ROUTINGS_V5 command.\n
    \n
    Immediately following this structure are num_sessions of the session map
    node payload (adm_session_map_node_v5_t).
*/
struct adm_cmd_matrix_map_routings_v5_t
{
    uint32_t                  matrix_id;
    /**< Specifies whether the matrix ID is Audio Rx (0) or Audio Tx (1). 
         Use the ADM_MATRIX_ID_AUDIO_RX or  ADM_MATRIX_ID_AUDIO_TX
         macros to set this field. */

    uint32_t                  num_sessions;
    /**< Number of sessions being updated by this command (optional).*/
}
#include "adsp_end_pack.h"
;

/* Session map node structure. */
typedef struct adm_session_map_node_v5_t adm_session_map_node_v5_t;

#include "adsp_begin_pack.h"

/** @brief Session map node structure.\n
    \n
    Immediately following this structure are num_copps entries of COPP IDs. The
    COPP IDs are 16 bits, so there might be a padding 16-bit field if num_copps
    is odd.
*/
struct adm_session_map_node_v5_t
{
    uint16_t                  session_id;
    /**< Handle of the ASM session to be routed.
             Supported values: 1 to 8. */

    uint16_t                  num_copps;
    /**< Number of COPPs to which this session is to be routed. 
             Supported values: 0 < num_copps <= ADM_MAX_COPPS.
      */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup adm_cmd_update_matrix_map_v5 */

/** @addtogroup adm_cmd_device_open_v5
@{ */
/** This command allows a client to open a COPP/Voice Proc. TX module and sets up the device  
      session: Matrix -> COPP -> AFE on the RX and AFE -> COPP -> Matrix on the TX. 
      This enables PCM data to be transferred to/from the endpoint (AFEPortID).

    @relevant_apr_hdr_fields
      Opcode -- ADM_CMD_DEVICE_OPEN_V5 \n
      Dst_port -- Ignored

    @apr_msg_payload
      @structure{adm_cmd_device_open_v5_t}
      @tablespace
      @inputtable{ADM_CMD_DEVICE_OPEN_V5_APR_msg_payload.tex}

    @prerequisites
      None.

    @return
      #ADM_CMDRSP_DEVICE_OPEN_V5 with the resulting status and
      COPP ID.
*/
#define ADM_CMD_DEVICE_OPEN_V5                          0x00010326

/** Indicates that endpoint_id_2 is to be ignored.*/
#define ADM_CMD_COPP_OPEN_END_POINT_ID_2_IGNORE                     0xFFFF

/** @xreflabel{hdr:RxPathCopp} Indicates an Rx path COPP.*/
#define ADM_CMD_COPP_OPEN_MODE_OF_OPERATION_RX_PATH_COPP             1

/** @xreflabel{hdr:LiveCopp} Indicates a Tx path live COPreP.*/
#define ADM_CMD_COPP_OPEN_MODE_OF_OPERATION_TX_PATH_LIVE_COPP        2

/** @xreflabel{hdr:NonLiveCopp} Indicates a Tx path nonlive COPP.*/
#define ADM_CMD_COPP_OPEN_MODE_OF_OPERATION_TX_PATH_NON_LIVE_COPP    3

/** Indicates that an audio COPP is to send/receive a mono PCM stream to/from
    END_POINT_ID_1.
*/
#define ADM_CMD_COPP_OPEN_CHANNEL_CONFIG_MONO                        1

/** Indicates that an audio COPP is to send/receive a
    stereo PCM stream to/from END_POINT_ID_1.
*/
#define ADM_CMD_COPP_OPEN_CHANNEL_CONFIG_STEREO                      2

/** Sample rate is 8000 Hz.*/
#define ADM_CMD_COPP_OPEN_SAMPLE_RATE_8K                             8000

/** Sample rate is 16000 Hz.*/
#define ADM_CMD_COPP_OPEN_SAMPLE_RATE_16K                            16000

/** Sample rate is 48000 Hz.*/
#define ADM_CMD_COPP_OPEN_SAMPLE_RATE_48K                            48000

/** Definition for a COPP live input flag bitmask.*/
#define ADM_BIT_MASK_COPP_LIVE_INPUT_FLAG                            (0x0001U)

/** Definition for a COPP live shift value bitmask.*/
#define ADM_SHIFT_COPP_LIVE_INPUT_FLAG                               0

/** Definition for the COPP ID bitmask.*/
#define ADM_BIT_MASK_COPP_ID                                        (0x0000FFFFUL)

/** Definition for the COPP ID shift value.*/
#define ADM_SHIFT_COPP_ID                                           0

/** Definition for the service ID bitmask.*/
#define ADM_BIT_MASK_SERVICE_ID                                     (0x00FF0000UL)

/** Definition for the service ID shift value.*/
#define ADM_SHIFT_SERVICE_ID                                        16

/** Definition for the domain ID bitmask.*/
#define ADM_BIT_MASK_DOMAIN_ID                                      (0xFF000000UL)

/** Definition for the domain ID shift value.*/
#define ADM_SHIFT_DOMAIN_ID                                          24

/** Bitmask for device_perf mode.
*/
#define ADM_BIT_MASK_DEVICE_PERF_MODE_FLAG                          (0x0000e000UL)

/** Bit shift for device_perf mode.
*/
#define ADM_BIT_SHIFT_DEVICE_PERF_MODE_FLAG                           13


/** A Legacy device session is identified by this value.
*/
#define ADM_LEGACY_DEVICE_SESSION                                      0

/** A Low latency stream session is identified by this value.
*/
#define ADM_LOW_LATENCY_DEVICE_SESSION                                 1



/* ADM device open command payload. */
typedef struct adm_cmd_device_open_v5_t adm_cmd_device_open_v5_t;

#include "adsp_begin_pack.h"

/** @brief ADM device open command payload of the
    #ADM_CMD_DEVICE_OPEN_V5 command.
*/
struct adm_cmd_device_open_v5_t
{
    uint16_t                  flags;
    /**< flags to configure the device session based on the bitmasks */

   /**< device_perf_mode (bit 15:13): Indicates which performance mode this device session
    * needs to   be opened.
        Supported values for bit 15:13:
        -#ADM_LEGACY_DEVICE_SESSION -- Opens a legacy device session using default buffer sizes.

        -#ADM_LOW_LATENCY_DEVICE_SESSION -- Opens a low latency device session by using shortened buffers
              in matrix & in COPP. It is not recommended to enable High latency algorithms as they
              might  negate the benefits of opening a low latency device and might also suffer quality
              degradation from unexpected jitter. adsp data path input latency
              (device side only) can vary from 1 to 2msec and adsp data path output latency(device side only)
              can vary from 2 to 4msec.
        -Other values -- reserved for future use.
        - Use #ADM_BIT_MASK_DEVICE_PERF_MODE_FLAG and
          #ADM_BIT_SHIFT_DEVICE_PERF_MODE_FLAG to configure this sub field.
    */


    uint16_t                  mode_of_operation;
    /**< Specifies whether the COPP must be opened on the Tx or Rx path. Use
         the ADM_CMD_COPP_OPEN_MODE_OF_OPERATION_* macros for supported
         values and interpretation.

         Supported values:
         - 0x1 -- Rx path COPP
         - 0x2 -- Tx path live COPP
         - 0x3 -- Tx path nonlive COPP

         Live connections cause sample discarding in the Tx device matrix if
         the destination output ports do not pull them fast enough. Nonlive
         connections queue the samples indefinitely. 
    */

    uint16_t                  endpoint_id_1;
    /**< Logical and physical endpoint ID of the audio path. If the ID is a
         voice processor Tx block, it receives near samples.
         
         Supported values: Any pseudoport, AFE Rx port, or AFE Tx port
         For a list of valid IDs, refer to @xhyperref{Q4,[Q4]}. 
         Q4 = Hexagon Multimedia: AFE Interface Specification 
    */

    uint16_t                  endpoint_id_2;
    /**< Logical and physical endpoint ID 2 for a voice processor Tx block.
         This is not applicable to audio COPP.
         
         Supported values:
         - AFE Rx port
         - 0xFFFF -- Endpoint 2 is unavailable and the voice processor Tx
           block ignores this endpoint
           
         When the voice processor Tx block is created on the audio record path,
         it can receive far-end samples from an AFE Rx port if the voice call
         is active. The ID of the AFE port is provided in this field.

         For a list of valid IDs, refer @xhyperref{Q4,[Q4]}. 
    */

    uint32_t                  topology_id;
    /**< Audio COPP topology ID; 32-bit GUID.

         For a complete list of audio topology IDs, see Section
         @xref{hdr:AudioPostProcTopos}.
    */

    uint16_t                  dev_num_channel;
    /**< Number of channels the audio COPP sends to/receives from the endpoint.
         Supported values: 1 to 8.    

         The value is ignored for the voice processor Tx block, where channel
         configuration is derived from the topology ID. 
    */

     uint16_t                  bit_width;
     /**< Bit width (in bits) that the audio COPP sends to/receives from the
          endpoint. The value is ignored for the voice processing Tx block,
          where the PCM width is 16 bits. 
    */

    uint32_t                  sample_rate;
    /**< Sampling rate at which the audio COPP/voice processor Tx block
         interfaces with the endpoint.
         
         Supported values for voice processor Tx: 8000, 16000, 48000 Hz
         Supported values for audio COPP: >0 and <=192 kHz
    */

    uint8_t                   dev_channel_mapping[8];
    /**< Array of channel mapping of buffers that the audio COPP sends to the
         endpoint.
         
         Channel[i] mapping describes channel I inside the buffer, where
         0 < i < dev_num_channel.
           
         This value is relevent only for an audio Rx COPP. For the voice
         processor block and Tx audio block, this field is set to zero and is
         ignored.
    */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup adm_cmd_device_open_v5 */

/** @ingroup adm_cmd_device_close_v5
    This command allows the client to close a COPP and disconnect the device session.

    @relevant_apr_hdr_fields
      Opcode -- ADM_CMD_DEVICE_CLOSE_V5 \n
      Dst_port -- COPP ID

    @apr_msg_payload
      None.

    @prerequisites
      None.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
*/
#define ADM_CMD_DEVICE_CLOSE_V5                                          0x00010327

/** @addtogroup adm_cmd_set_pp_params_v5
@{ */
/** Sets one or more parameters to a COPP.

    @relevant_apr_hdr_fields
      Opcode -- ADM_CMD_SET_PP_PARAMS_V5 \n
      Dst_port -- COPP ID

    @apr_msg_payload
      See Section @xref{hdr:ADMCmdSetPPParams_v5}.
      @negparspace
      @structure{adm_cmd_set_pp_params_v5_t}

    @prerequisites
      This command must be applied to a valid COPP ID.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
*/
#define ADM_CMD_SET_PP_PARAMS_V5                                       0x00010328

/* Payload of an ADM_CMD_SET_PP_PARAMS_V5 command. */
typedef struct adm_cmd_set_pp_params_v5_t adm_cmd_set_pp_params_v5_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ADM_CMD_SET_PP_PARAMS_V5 command.\n
    \n
    If the data_payload_addr_lsw and data_payload_addr_msw element are NULL, 
    a series of adm_param_data_t structures immediately follows, whose total size 
    is data_payload_size bytes.
*/
struct adm_cmd_set_pp_params_v5_t
{

    uint32_t                  data_payload_addr_lsw;
    /**< LSW of parameter data payload address.*/
    uint32_t                  data_payload_addr_msw;
    /**< MSW of parameter data payload address.*/

    // If mem_map_handle is non zero, the ParamData payloads begin at the address specified (out-of-band).

    uint32_t                  mem_map_handle;
    /**< Memory map handle returned by ADM_CMD_SHARED_MEM_MAP_REGIONS command*/
    // If mem_map_handle is zero implies the message is in the payload (in-band).

    uint32_t                  data_payload_size;
    /**< Size in bytes of the variable payload accompanying this message or
         in shared memory. This is used for parsing the parameter payload.
    */
}
#include "adsp_end_pack.h"
;

/* Payload of COPP parameter data. */
typedef struct adm_param_data_v5_t adm_param_data_v5_t;

#include "adsp_begin_pack.h"

/** @brief Payload format for COPP parameter data.\n
    \n
    Immediately following this structure are param_size bytes of parameter
    data.
*/
struct adm_param_data_v5_t
{
    uint32_t                  module_id;
    /**< Unique ID of the module. */

    uint32_t                  param_id;
    /**< Unique ID of the parameter. */

    uint16_t                  param_size;
    /**< Data size of the param_id/module_id combination. This value is a
         multiple of 4 bytes. */

    uint16_t                  reserved;
    /**< Reserved for future enhancements. This field must be set to zero.
     */ 
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup adm_cmd_set_pp_params_v5 */

/** @addtogroup adm_resp_device_open_v5
@{ */
/** Returns the status and COPP ID to an #ADM_CMD_DEVICE_OPEN_V5 command.

    @relevant_apr_hdr_fields
      Opcode -- ADM_CMDRSP_DEVICE_OPEN_V5 \n
      Dst_port -- Ignored

    @apr_msg_payload
      @structure{adm_cmd_rsp_device_open_v5_t}
      @tablespace
      @inputtable{ADM_CMDRSP_DEVICE_OPEN_V5_APR_msg_payload.tex}

    @prerequisites
      None.

    @return
      None.
*/
#define ADM_CMDRSP_DEVICE_OPEN_V5                                     0x00010329

/* ADM command response to a Device open command. */
typedef struct adm_cmd_rsp_device_open_v5_t adm_cmd_rsp_device_open_v5_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ADM_CMDRSP_DEVICE_OPEN_V5 message, which returns the
    status and COPP ID to an #ADM_CMD_DEVICE_OPEN_V5 command.
*/
struct adm_cmd_rsp_device_open_v5_t
{
    uint32_t                  status;
    /**< Status message (error code).
         Supported values: Refer to @xhyperref{Q3,[Q3]} */

    uint16_t                  copp_id;
    /**< COPP ID:  Supported values: 0 <= copp_id < ADM_MAX_COPPS*/

    uint16_t                  reserved;
    /**< Reserved. This field must be set to zero.*/
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup adm_resp_device_open_v5 */

/** @addtogroup adm_cmd_get_pp_params_v5
@{ */
/** This command allows a query of one COPP parameter.

    @relevant_apr_hdr_fields
      Opcode -- ADM_CMD_GET_PP_PARAMS_V5 \n
      Dst_port -- COPP ID

    @apr_msg_payload
      See Section @xref{hdr:ADMCmdGetPPParams_v5}.

    @prerequisites
      This command must be applied to a valid COPP ID.

    @return
      #ADM_CMDRSP_GET_PP_PARAMS_V5.
*/
#define ADM_CMD_GET_PP_PARAMS_V5                                0x0001032A

/* Payload format for the Get Parameters command. */
typedef struct adm_cmd_get_pp_params_v5_t adm_cmd_get_pp_params_v5_t;

#include "adsp_begin_pack.h"

/** @brief Payload an #ADM_CMD_GET_PP_PARAMS_V5 command.
*/
struct adm_cmd_get_pp_params_v5_t
{
    uint32_t                  data_payload_addr_lsw;
    /**< LSW of parameter data payload address.*/
    
    uint32_t                  data_payload_addr_msw;
    /**< MSW of parameter data payload address.*/

    //If the mem_map_handle is non zero, on ACK, the ParamData payloads begin at the address specified (out-of-band).

    uint32_t                  mem_map_handle;
    /**< Memory map handle returned by ADM_CMD_SHARED_MEM_MAP_REGIONS command.
            If the mem_map_handle is 0, it implies that the ACK's payload will contain the ParamData (in-band).
    */

    uint32_t                  module_id;
    /**< Unique ID of the module. */

    uint32_t                  param_id;
    /**< Unique ID of the parameter. */

    uint16_t                  param_max_size;
    /**< Maximum data size of the parameter ID/module ID combination. This
         field is a multiple of 4 bytes. 
    */

    uint16_t                  reserved;
    /**< Reserved for future enhancements. This field must be set to zero.*/
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup adm_cmd_get_pp_params_v5 */

/** @addtogroup adm_resp_get_pp_param_values_v5
@{ */
/** Returns parameter values in response to an #ADM_CMD_GET_PP_PARAMS_V5 command.

    @relevant_apr_hdr_fields
      Opcode -- ADM_CMDRSP_GET_PP_PARAMS_V5 \n
      Dst_port -- COPP ID

    @apr_msg_payload
      @structure{adm_cmd_rsp_get_pp_params_v5_t}
      @tablespace
      @inputtable{ADM_CMDRSP_GET_PP_PARAMS_V5_APR_msg_payload.tex}

    @parspace Acknowledgment parameter payload
      @structure{adm_param_data_v5_t}
      @tablespace
      @inputtable{Shared_param_data_variable_payload.tex}

    @keep{4}
    @prerequisites
      #ADM_CMD_GET_PP_PARAMS_V5 must have been applied to a valid COPP.

    @return
      None.
*/
#define ADM_CMDRSP_GET_PP_PARAMS_V5                             0x0001032B

/* Payload for an ADM get pre/postprocessing parameters ACK event. */
typedef struct adm_cmd_rsp_get_pp_params_v5_t adm_cmd_rsp_get_pp_params_v5_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ADM_CMDRSP_GET_PP_PARAMS_V5 message, which returns
    parameter values in response to an #ADM_CMD_GET_PP_PARAMS_V5 command.\n
    \n
    Immediately following this structure is the adm_param_data_v5_t
    structure containing the pre/postprocessing parameter data. For an in-band
    scenario, the variable payload depends on the size of the parameter.
*/
struct adm_cmd_rsp_get_pp_params_v5_t
{
    uint32_t                  status;
    /**< Status message (error code).

         Supported values: Refer to @xhyperref{Q3,[Q3]}
      */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup adm_resp_get_pp_param_values_v5 */

/** @addtogroup adm_cmd_gain_control_v5
@{ */
/** Allows a client to control the gains on various session-to-COPP paths.

    @relevant_apr_hdr_fields
      Opcode -- ADM_CMD_MATRIX_RAMP_GAINS_V5 \n
      Dst_port -- Ignored

    @apr_msg_payload
      @structure{adm_cmd_matrix_ramp_gains_v5_t}
      @tablespace
      @inputtable{ADM_CMD_MATRIX_RAMP_GAINS_V5_APR_msg_payload.tex}

    @description
     For every session-to-COPP path, the gains specified are channel-specific and 
     can be applied immediately, or achieved over a period of time in ramping fashion. 
     The latter helps avoid a pop when the gain is changed. The client can specify the ramp curve, 
     ramp duration, and step duration to indicate the kind of ramping required to achieve the 
     target gain. This command allows a client to specify gain ramps for multiple session-to-COPP 
     paths at one time, allowing better synchronization. It can also be used to apply the gain ramp 
     globally to all session-to-COPP paths that exist for the given session. This command takes effect 
     immediately upon receipt.
     
     This command:
        Can be used for controlling per-stream-per-device volume
        Supports multichannel (up to 7.1) gain settings
        Can be used for soft-stepping during a stream’s volume changes
    
     It is the client’s responsibility to maintain a history of the gain values for every channel in each 
     of the session-to-COPP paths. This command does not allow a client to specify new gain values 
     for only some channels in a stream. It has to specify new gain values for all the channels. 
     If the client wishes to change the gains selectively of only some of the channels in an audio stream, 
     it should still send this command with all the required target gains. For the channels it does 
     not wish to change the gains, it should specify its current gain as the target gain.

     The aDSP shall apply gains on a stream after reformatting, if required, to match the COPP’s 
     channel mapping. Therefore, it is the client’s responsibility to send this command with gain 
     values set for exactly those channels that the device (COPP) is operating on. 

    @prerequisites
      All specified session IDs and COPP IDs must be valid and connected to the targeted matrix.
    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

    @parspace Session COPP gain type
      @structure{adm_session_copp_gain_v5_t}
      @tablespace
      @inputtable{ADM_CMD_MATRIX_RAMP_GAINS_V5_Session_COPP_gain_type.tex}
*/
#define ADM_CMD_MATRIX_RAMP_GAINS_V5                                 0x0001032C

/** Indicates that the target gain in the current adm_session_copp_gain_v5_t
    structure is to be applied to all the session-to-COPP paths that exist for
    the specified session.
*/
#define ADM_CMD_MATRIX_RAMP_GAINS_COPP_ID_ALL_CONNECTED_COPPS     0xFFFF

/** Indicates that the target gain is to be immediately applied to the
    specified session-to-COPP path, without a ramping fashion.
*/
#define ADM_CMD_MATRIX_RAMP_GAINS_RAMP_DURATION_IMMEDIATE         0x0000

/** Enumeration for a linear ramping curve.*/
#define ADM_CMD_MATRIX_RAMP_GAINS_RAMP_CURVE_LINEAR               0x0000

/* Payload format for the ADM Matrix Ramp Gains command. */
typedef struct adm_cmd_matrix_ramp_gains_v5_t adm_cmd_matrix_ramp_gains_v5_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ADM_CMD_MATRIX_RAMP_GAINS_V5 command.\n
    \n    
    Immediately following this structure are num_gains of the
    adm_session_copp_gain_v5_t structure.
*/
struct adm_cmd_matrix_ramp_gains_v5_t
{
    uint32_t                  matrix_id;
    /**< Specifies whether the matrix ID is Audio Rx (0) or Audio Tx (1). 
         Use the ADM_MATRIX_ID_AUDIO_RX or  ADM_MATRIX_ID_AUDIO_TX
         macros to set this field. 
    */

    uint16_t                  num_gains;
    /**< Number of gains being applied. */

    uint16_t                  reserved_for_align;
    /**< Reserved. This field must be set to zero.*/
}
#include "adsp_end_pack.h"
;

/* Session-to-COPP path gain structure. */
typedef struct adm_session_copp_gain_v5_t adm_session_copp_gain_v5_t;

#include "adsp_begin_pack.h"

/** @brief Session-to-COPP path gain structure, used by the
    #ADM_CMD_MATRIX_RAMP_GAINS_V5 command.\n
    \n
    This structure specifies the target gain (per channel) that must be applied
    to a particular session-to-COPP path in the audio matrix. The structure can
    also be used to apply the gain globally to all session-to-COPP paths that
    exist for the given session.\n
    \n    
    The aDSP uses device channel mapping to determine which channel gains to
    use from this command. For example, if the device is configured as stereo,
    the aDSP uses only target_gain_ch_1 and target_gain_ch_2, and it ignores
    the others.
*/
struct adm_session_copp_gain_v5_t
{
    uint16_t                  session_id;
    /**< Handle of the ASM session.
         Supported values: 1 to 8. 
    */

    uint16_t                  copp_id;
    /**< Handle of the COPP. Gain will be applied on the Session ID – COPP ID path.*/

    uint16_t                  ramp_duration;
    /**< Duration (in milliseconds) of the ramp over which target gains are
         to be applied. Use #ADM_CMD_MATRIX_RAMP_GAINS_RAMP_DURATION_IMMEDIATE
         to indicate that gain must be applied immediately. 
    */

    uint16_t                  step_duration;
    /**< Duration (in milliseconds) of each step in the ramp. This parameter
         is ignored if ramp_duration is equal to
         #ADM_CMD_MATRIX_RAMP_GAINS_RAMP_DURATION_IMMEDIATE.

         Supported value: 1
    */

    uint16_t                  ramp_curve;
    /**< Type of ramping curve.  

          Supported value: #ADM_CMD_MATRIX_RAMP_GAINS_RAMP_CURVE_LINEAR
    */

    uint16_t                  reserved_for_align;
    /**< Reserved. This field must be set to zero. */

    uint16_t                  target_gain_ch_1;
    /**< Target linear gain for channel 1 in Q13 format; */

    uint16_t                  target_gain_ch_2;
    /**< Target linear gain for channel 2 in Q13 format; */

    uint16_t                  target_gain_ch_3;
    /**< Target linear gain for channel 3 in Q13 format; */

    uint16_t                  target_gain_ch_4;
    /**< Target linear gain for channel 4 in Q13 format; */

    uint16_t                  target_gain_ch_5;
    /**< Target linear gain for channel 5 in Q13 format; */

    uint16_t                  target_gain_ch_6;
    /**< Target linear gain for channel 6 in Q13 format; */

    uint16_t                  target_gain_ch_7;
    /**< Target linear gain for channel 7 in Q13 format; */

    uint16_t                  target_gain_ch_8;
    /**< Target linear gain for channel 8 in Q13 format; */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup adm_cmd_gain_control_v5 */

/** @addtogroup adm_cmd_matrix_mute_v5
@{ */
/** Allows a client to set mute/unmute on various session-to-COPP paths.

    @relevant_apr_hdr_fields
      Opcode -- ADM_CMD_MATRIX_MUTE_V5 \n
      Dst_port -- Ignored

    @apr_msg_payload
      @structure{adm_cmd_matrix_mute_v5_t}
      @tablespace
      @inputtable{ADM_CMD_MATRIX_MUTE_V5_APR_msg_payload.tex}

    @description
      For every session-to-COPP path (stream-device interconnection), 
      mute/unmute can be set individually on the output channels.
        @negparspace
      The ADM_CMD_MATRIX_MUTE_V5 command:
      - Can be used for controlling per-stream-per-device mute/unmute
      - Supports multichannel mute settings (up to 8 channels)
      - Overrides any ADM_CMD_MATRIX_RAMP_GAINS that may be called
        @negparspace
      Example 1: Ramp_gain > Mute > Unmute sequence
      -# Mute is applied immediately.
      -# All ramps are canceled.
      -# On unmute, the target setting provided in the ramp_gain field is
        applied immediately.
        @negparspace
      Example 2: Mute > Ramp_gain > Unmute sequence
      -# Mute is applied immediately.
      -# Ramps are not serviced.
      -# On unmute, the target setting provided in the ramp_gain field is
        applied immediately.
        @negparspace
      On the playback path, if the client wants to use this as a stream mute
      command, it can set the ADM_CMD_MATRIX_MUTE_COPP_ID_ALL_CONNECTED_COPPS
      for the copp_id. This will mute all interconnections emanating from this
      stream. 
        @negparspace
      On the playback path, if the client wants to use this as a device (#n)
      mute command, it must provide the mute command to individual (stream 1,
      device n), (stream 2, device n) combinations. In this example, stream 1
      and stream 2 mix into device n.
        @negparspace
      The aDSP applies mute/unmute on a stream after reformatting, if required,
      to match the COPP's channel mapping. Therefore, it is
      the client’s responsibility to send this command to mute/unmute exactly
      those channels on which the device (COPP) is operating. 

    @prerequisites
      All specified session IDs and COPP IDs must be valid and connected to the targeted matrix.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
*/
#define ADM_CMD_MATRIX_MUTE_V5                                0x0001032D

/** Indicates that mute/unmute in the current adm_session_copp_mute_v5_t structure
    is to be applied to all the session-to-COPP paths that exist for the
    specified session.
*/
#define ADM_CMD_MATRIX_MUTE_COPP_ID_ALL_CONNECTED_COPPS     0xFFFF

/* Payload format for the ADM Matrix mute command. */
typedef struct adm_cmd_matrix_mute_v5_t adm_cmd_matrix_mute_v5_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ADM_CMD_MATRIX_MUTE_V5 command*/
struct adm_cmd_matrix_mute_v5_t
{
    uint32_t                  matrix_id;
    /**< Specifies whether the matrix ID is Audio Rx (0) or Audio Tx (1). 
         Use the ADM_MATRIX_ID_AUDIO_RX or  ADM_MATRIX_ID_AUDIO_TX
         macros to set this field. 
    */

    uint16_t                  session_id;
    /**< Handle of the ASM session.
         Supported values: 1 to 8. 
    */

    uint16_t                  copp_id;
    /**< Handle of the COPP.
         Use ADM_CMD_MATRIX_MUTE_COPP_ID_ALL_CONNECTED_COPPS to indicate that
         mute/unmute must be applied to all the COPPs connected to session_id. 
         
         Supported values:
         - 0xFFFF -- Apply mute/unmute to all connected COPPs
         - Other values -- Valid COPP ID 
    */

    uint8_t                  mute_flag_ch_1;
    /**< Mute flag for channel 1 is set to unmute (0) or mute (1). */

    uint8_t                  mute_flag_ch_2;
    /**< Mute flag for channel 2 is set to unmute (0) or mute (1). */

    uint8_t                  mute_flag_ch_3;
    /**< Mute flag for channel 3 is set to unmute (0) or mute (1). */

    uint8_t                  mute_flag_ch_4;
    /**< Mute flag for channel 4 is set to unmute (0) or mute (1). */

    uint8_t                  mute_flag_ch_5;
    /**< Mute flag for channel 5 is set to unmute (0) or mute (1). */

    uint8_t                  mute_flag_ch_6;
    /**< Mute flag for channel 6 is set to unmute (0) or mute (1). */

    uint8_t                  mute_flag_ch_7;
    /**< Mute flag for channel 7 is set to unmute (0) or mute (1). */

    uint8_t                  mute_flag_ch_8;
    /**< Mute flag for channel 8 is set to unmute (0) or mute (1). */

    uint16_t                 ramp_duration;
    /**< Period (in milliseconds) over which the soft mute/unmute will be
         applied.
             
         Supported values: 0 (Default) to 0xFFFF 
         
         The default of 0 means mute/unmute will be applied immediately. 
    */

    uint16_t                 reserved_for_align;
    /**< Clients must set this field to zero.*/
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup adm_cmd_matrix_mute_v5 */

/** @addtogroup adm_cmd_connect_afe_port_v5
@{ */
/** Allows a client to connect the desired stream to
   the desired AFE port through the stream router

@relevant_apr_hdr_fields
      Opcode -- ADM_CMD_CONNECT_AFE_PORT_V5 \n
      Dst_port -- Ignored

    @apr_msg_payload
      @structure{adm_cmd_connect_afe_port_t}
      @tablespace
      @inputtable{ADM_CMD_CONNECT_AFE_PORT_V5_APR_msg_payload.tex}

    @description
      This command allows the client to connect specified session to specified AFE port.
      This is used for compressed streams only opened using the #ASM_STREAM_CMD_OPEN_WRITE_COMPRESSED
      or #ASM_STREAM_CMD_OPEN_READ_COMPRESSED command.

    @prerequisites
      Session ID and AFE Port ID must be valid.
      #ASM_STREAM_CMD_OPEN_WRITE_COMPRESSED or #ASM_STREAM_CMD_OPEN_READ_COMPRESSED
      must have been called on this session.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
*/

#define ADM_CMD_CONNECT_AFE_PORT_V5                           0x0001032E

/** Enumeration for the Rx stream router ID.*/
#define ADM_STRTR_ID_RX                    0

/** Enumeration for the Tx stream router ID.*/
#define ADM_STRTR_ID_TX                    1

/* Payload format for the ADM Connect AFE port command. */
typedef struct adm_cmd_connect_afe_port_v5_t adm_cmd_connect_afe_port_v5_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ADM_CMD_CONNECT_AFE_PORT_V5 command.*/
struct adm_cmd_connect_afe_port_v5_t
{
    uint8_t                  mode;
    /**< ID of the stream router (RX/TX). Use the
         ADM_STRTR_ID_RX or ADM_STRTR_ID_TX macros to set this field. */

    uint8_t                  session_id;
    /**< Session ID of the stream to connect */

    uint16_t                 afe_port_id;
    /**< Port ID of the AFE port to connect to.*/
    uint32_t                 num_channels;
    /**< Number of device channels
             Supported values: 2(Audio Sample Packet),
             8 (HBR Audio Stream Sample Packet) */

    uint32_t                 sampling_rate;
    /**< Device sampling rate
                 Supported values: Any */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup adm_cmd_connect_afe_port_v5 */

/** @addtogroup adm_cmd_disconnect_afe_port_v5
@{ */
/** Allows a client to disconnect stream router from desired AFE port

@relevant_apr_hdr_fields
      Opcode -- ADM_CMD_DISCONNECT_AFE_PORT_V5 \n
      Dst_port -- AFE Port ID

    @apr_msg_payload
      None

    @description
      This command allows the client to disconnect stream router from desired AFE port

    @prerequisites
      The #ADM_CMD_CONNECT_AFE_PORT_V5 command should have been used to connect this AFE port
      to the stream router.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
*/

#define ADM_CMD_DISCONNECT_AFE_PORT_V5                           0x0001032F

/** @} */ /* end_addtogroup adm_cmd_disconnect_afe_port_v5 */

/** @addtogroup adm_cmd_get_spa_data_v5
    Indicates a data buffer to be filled with spectrum analyzer data.

    @relevant_apr_hdr_fields
      Opcode -- ADM_DATA_CMD_SPA_V5 \n
      Dst_port -- COPP ID

    @apr_msg_payload
      See Section @xref{hdr:ASMDataCmdSPA}.

    @prerequisites
      This command must be applied to a valid COPP ID.
        @negparspace
      The spectrum analyzer must have been enabled using #ADM_CMD_SET_PP_PARAMS_V5.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
    @newpage
*/
#define ADM_DATA_CMD_SPA_V5                                          0x00010330

/** @} */ /* end_addtogroup adm_cmd_get_spa_data_v5 */

/** @addtogroup adm_event_spa_v5_done_resp
    Indicates that the referenced buffer has been filled with spectrum analyzer
    data and is available to the client.

    @relevant_apr_hdr_fields
      Opcode -- ADM_DATA_EVENT_SPA_V5_DONE \n
      Dst_port -- COPP ID

    @apr_msg_payload
      See Section @xref{hdr:ASMDataEventSpaDone}.

    @prerequisites
      A corresponding #ADM_DATA_CMD_SPA_V5 on a valid COPP ID must have been received and processed.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
*/
#define ADM_DATA_EVENT_SPA_V5_DONE                                    0x00010331

/** @} */ /* end_addtogroup adm_event_spa_v5_done_resp*/

/**
  This command adds custom topologies to the DSP for use in COPP. These
  topologies can be created by specifying their topology IDs in the COPP open command.
  */
#define ADM_CMD_ADD_TOPOLOGIES_V5                                0x00010335

/* The payload for the ADM_CMD_ADD_TOPOLOGIES_V5 command */
typedef struct adm_cmd_add_topologies_v5_t adm_cmd_add_topologies_v5_t;

/** @brief Payload of the ADM_CMD_ADD_TOPOLOGIES_V5 message.
*/
#include "adsp_begin_pack.h"
struct adm_cmd_add_topologies_v5_t
{
    uint32_t                  data_payload_addr_lsw;
    /**< LSW of the topology buffer address. */
    uint32_t                  data_payload_addr_msw;
    /**< MSW of the topology buffer address. */

    uint32_t                  mem_map_handle;
    /**< Supported Values: Any.
    Memory map handle returned by ADM_CMD_SHARED_MEM_MAP_REGIONS command. */

    uint32_t                  buffer_size;
    /**< Size of the valid data in the topology buffer in bytes */
}
#include "adsp_end_pack.h"
;

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_ADM_API_H_ */
