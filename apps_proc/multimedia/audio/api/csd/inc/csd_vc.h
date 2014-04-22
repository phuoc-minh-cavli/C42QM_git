#ifndef __CSD_VC_H__
#define __CSD_VC_H__
/**
  @file  csd_vc.h
  @brief Contains voice context API definitions.
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The CSD_mainpage.dox file contains all file/group descriptions that are 
      in the output PDF generated using Doxygen and LaTeX. To edit or update 
      any of the file/group text in the PDF, edit the CSD_mainpage.dox file or 
      contact Tech Pubs.
===========================================================================*/
/*-----------------------------------------------------------------------------
     Copyright (C) 2011-12 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary.
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
    This section contains comments describing changes made to this module.
    Notice that changes are listed in reverse chronological order.

    $Header: //components/rel/audio.tx/2.0/audio/api/csd/inc/csd_vc.h#1 $
    $DateTime: 2018/12/25 22:39:07 $
    $Author: pwbldsvc $
    $Change: 17929171 $
    $Revision: #1 $
when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/26/12   jk       added CSD_VC_COMMAND_RSP_MAP type mem map response 
06/04/12   sd      (Tech Pubs) Edited/added Doxygen markup and comments.
03/15/11   sd      (Tech Pubs) Edited/added Doxygen markup and comments.
=============================================================================*/

#include "mmdefs.h"
#include "csd_vcommon.h"

/** @addtogroup csd_vc
@{ */

/****************************************************************************
 * CSD VC EVENT DEFINITIONS                                                 *
 ****************************************************************************/

/** 
  @name Voice Context Event Definitions
  @{
*/

/**
  Event callback function type for the voice context.
  
  @param[in] cmd_token  Transaction identifier provided by the 
                        client that allows the client to
                        identify which command completed.\n
                        @note1hang The upper 8 bits are reserved
                        for internal processing.
  @param[in] event_id   ID of the callback event.
  @param[in] param      Payload data of the callback event.
  @param[in] param_size Size of the payload data for the callback event.
  @param[in] cb_data    Callback data.

  @return
  CSD_EOK -- Success.\n
  CSD_EFAILED -- Otherwise.

  @dependencies
  None.
*/
typedef uint32_t ( *csd_vc_event_callback_fn_t ) (
  uint32_t cmd_token,
  uint32_t event_id,
  void* param,
  uint32_t param_size,
  void* cb_data
);

/**
  Event issued by the voice context to its client in response to an IOCTL that 
  indicates the success/failure status of the command. 

  @payload
  #csd_vc_event_command_rsp_t

  @latexonly \input{tables/CSD_VC_EVENTID_COMMAND_RSP.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/
#define CSD_VC_EVENTID_COMMAND_RSP ( 0x00011141 )

/**
  Event issued by the voice context to its client that indicates the transition 
  to/from the Run state.  

  @payload
  #csd_vc_event_state_notify_t

  @latexonly \input{tables/CSD_VC_EVENTID_STATE_NOTIFY.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/
#define CSD_VC_EVENTID_STATE_NOTIFY ( 0x00011182 )

/**
  Event sent by the voice context to the client that enabled Tx DTMF detection 
  when DTMF is detected in the Tx path. 

  @payload
  #csd_vc_event_tx_dtmf_detected_t
   
  @description
  The DTMF detection feature can only be used to detect DTMF frequencies
  as listed in ITU-T Recommendation Q.23 
  @latexonly \hyperref[S5]{[S5]}@endlatexonly, Section 6. The supported values 
  for the low_freq and high_freq parameters are as listed in 
  #csd_vc_event_tx_dtmf_detected_t.

  @return
  None.

  @dependencies
  None.
*/
#define CSD_VC_EVENTID_TX_DTMF_DETECTED ( 0x00011034 )

/** @} */ /* end_name Voice Context Event Definitions */

/****************************************************************************
 * CSD VC EVENT PAYLOAD STRUCTURES                                          *
 ****************************************************************************/

/** @brief Payload for #CSD_VC_EVENTID_COMMAND_RSP. */
struct csd_vc_event_command_rsp_t
{
  uint32_t status_id; 
    /**< Status code of the response. */
};

/* Type definition for #csd_vc_event_command_rsp_t. */
typedef struct csd_vc_event_command_rsp_t csd_vc_event_command_rsp_t;

/** @brief Payload for #CSD_VC_EVENTID_STATE_NOTIFY. */
struct csd_vc_event_state_notify_t
{
  uint32_t state_id; 
    /**< State ID of the state notification. */
};

/* Type definition for #csd_vc_event_state_notify_t. */
typedef struct csd_vc_event_state_notify_t csd_vc_event_state_notify_t;

/** @brief Payload for #CSD_VC_EVENTID_TX_DTMF_DETECTED. */
struct csd_vc_event_tx_dtmf_detected_t
{
  uint16_t low_freq;
    /**< Detected low frequency. Support values:\n
         - 697 Hz. 
         - 770 Hz. 
         - 852 Hz. 
         - 941 Hz. */
  uint16_t high_freq;
    /**< Detected high frequency. Support values:\n
         - 1209 Hz. 
         - 1336 Hz. 
         - 1477 Hz. 
         - 1633 Hz. */
};

/* Type definition for #csd_vc_event_tx_dtmf_detected_t. */
typedef struct csd_vc_event_tx_dtmf_detected_t csd_vc_event_tx_dtmf_detected_t;

/****************************************************************************
 * CSD VC OPEN PAYLOAD STRUCTURES to be used with #csd_open()                *
 ****************************************************************************/

/** @brief Payload for #CSD_OPEN_VOICE_CONTEXT. */
struct csd_vc_open_full_control_t
{
  csd_vc_event_callback_fn_t callback_fn;
    /**< Event callback function. */
  void* cb_data;
    /**< Callback data. */
  const char_t* name;
    /**< Session name. */
  uint32_t name_size;
    /**< Session name size. */

  uint16_t direction;
    /**< Direction in which the stream is flowing. Supported values:\n
         - 0 -- Tx only. 
         - 1 -- Rx only. 
         - 2 -- Tx and Rx.      */
  uint32_t network_id;
    /**< Network ID. Default: 0. */

  uint32_t* ret_status;
    /**< Returns the Open status. */
};

/* Type definition for #csd_vc_open_full_control_t. */
typedef struct csd_vc_open_full_control_t csd_vc_open_full_control_t;

/** @name Notify event masks
  @{
 */
/** Output buffer is filled. */
#define CSD_VC_IVPCM_NOTIFY_MASK_OUTPUT_BUFFER ( 1 ) /* output buffer filled. */

/** Input buffer is consumed. */
#define CSD_VC_IVPCM_NOTIFY_MASK_INPUT_BUFFER ( 2 ) /* input buffer consumed. */

/** Event is a time tick. */
#define CSD_VC_IVPCM_NOTIFY_MASK_TIMETICK ( 4 ) /* event is a timetick. */

/** Error occurred in the output buffer operation. */
#define CSD_VC_IVPCM_NOTIFY_MASK_OUTPUT_ERROR ( 8 ) /* error in output buffer operation. */

/** Error occurred in the input buffer operation. */
#define CSD_VC_IVPCM_NOTIFY_MASK_INPUT_ERROR ( 16 ) /* error in input buffer operation. */

/** @} */ /* end_namegroup Notify event masks */

/**
  Provides the following notifications on a per-tap-point basis to the client: 
  - Buffer processing notifications
  - Timetick information
  - Sampling rate information
  - Number of output channels 

  The interpretation of this notification is indicated by the notify_mask in 
  the payload.

  The #CSD_VC_IVPCM_EVT_NOTIFY event is provided by CSD_VC to the client for 
  Vocoder PCM notifications once this feature is enabled using 
  #CSD_VC_IOCTL_IVPCM_CMD_START. 

  If the timetick bit of notify_mask is set, this notification is used as a 
  timetick that is issued to the client every x milliseconds, where x is the 
  size of the PCM frame on which the vDSP operates, and denotes the boundary 
  upon which the PCM buffer transfers occur between the client and CSD_VC. If 
  the timetick bit is not set, the notify event is used for buffer 
  notifications only. If an output or input error occurs in the buffer 
  operation, the correponding error bit is set in the notify_mask.

  Each notify event can accommodate any combination of timetick, output buffer 
  filled, or input buffer consumed masks.
  
  @vertspace 
  @par Payload
  #csd_vc_ivpcm_evt_notify_t

  @return
  None.

  @dependencies
  None.

  @description 
  After a CSD_VC_IVPCM_CMD_START command is issued, the buffer transfer between 
  the client and CSD starts when the first CSD_VC_IVPCM_EVT_NOTIFY event is 
  received by the client. The buffer transfer cannot start prior to receiving 
  the first event.\n 
  @vertspace
  The first notification is for buffer setup information only. The 
  interpretation of the #CSD_VC_IVPCM_EVT_NOTIFY event and the information 
  conveyed by it to the client depends on the use case and the time when the 
  notification is issued.  
  @par
  USE CASE 1 -- Output only tap point:\n
    - First #CSD_VC_IVPCM_EVT_NOTIFY event after the Start command:
        - notify_mask = 0
        - Indicates the requested size of the output buffer to be provided by 
          the client.
    - Subsequent #CSD_VC_IVPCM_EVT_NOTIFY events:
        - notify_mask bit 0 is set.
        - Indicate the previously provided output buffer has been filled, and 
          its sampling rate and number of channels are as specified by the 
          sampling_rate and num_out_channels fields. This event also indicates 
          the size of the next output buffer to be provided by the client.
  @par
  USE CASE 2 -- Input only tap point:\n
    - First #CSD_VC_IVPCM_EVT_NOTIFY event after the Start command:
        - notify_mask = 0
        - Indicates the requested size and sampling rate of the input buffer to
          be provided by the client.
    - Subsequent #CSD_VC_IVPCM_EVT_NOTIFY events:
        - notify_mask bit 1 is set.
        - Indicate the previously provided input buffer has been consumed. 
        - Indicate the size and sampling rate of the next input buffer to be 
          provided by the client.
  @par
  USE CASE 3 -- Output-input tap point:\n
    @note1hang The sampling rate of the input and output must always be the 
               same in this case.
    - First #CSD_VC_IVPCM_EVT_NOTIFY event after the Start command:
        - notify_mask = 0
        - Indicates the requested size of the output buffer to be provided by 
          the client.
    - Second #CSD_VC_IVPCM_EVT_NOTIFY event after the START command:
        - notify_mask bit 1 is set.
        - Indicates that the first provided output buffer has been filled, and 
          its sampling rate and number of channels are as specified by the 
          sampling_rate and num_out_channles fields.
        - Indicates the size and sampling rate of the first input buffer to
          be provided by the client.
    - Subsequent #CSD_VC_IVPCM_EVT_NOTIFY events:
        - notify_mask bits 0 and 1 both set
        - Indicate the previously provided output buffer has been filled, and 
          its sampling rate and number of channels are as specified by the 
          sampling_rate and num_out_channels fields.
        - Indicate the previously provided input buffer has been consumed.
        - Indicate the requested size of the next output buffer and the
          requested size and sampling rate of the next input buffer.
*/
#define CSD_VC_IVPCM_EVT_NOTIFY ( 0x000112AF )

/* Type definition for #csd_vc_ivpcm_evt_notify_t.
*/
typedef struct csd_vc_ivpcm_evt_notify_t csd_vc_ivpcm_evt_notify_t;

/** @brief Payload for #CSD_VC_IVPCM_EVT_NOTIFY.
*/
struct csd_vc_ivpcm_evt_notify_t
{
 uint32_t tap_point;
   /**< GUID indicating tap point location for which this notification is
        being issued. */
 uint32_t notify_mask;
   /**< Bitmask indicating the notification mode.
        Bitmask description:
        - Bit 0 -- Output buffer filled, use 
          #CSD_VC_IVPCM_NOTIFY_MASK_OUTPUT_BUFFER
        - Bit 1 -- Input buffer consumed, use 
          #CSD_VC_IVPCM_NOTIFY_MASK_INPUT_BUFFER
        - Bit 2 -- Notify event is a timetick, use 
          #CSD_VC_IVPCM_NOTIFY_MASK_TIMETICK
        - Bit 3 -- Error in output buffer operation, use 
          #CSD_VC_IVPCM_NOTIFY_MASK_OUTPUT_ERROR\n
          This bit will be set if there is an error in output buffer operation. 
          This bit will be set along with the "output buffer filled" bit to 
          return the output buffer with an error indication.
        - Bit 4 -- Error in input buffer operation, use 
          #CSD_VC_IVPCM_NOTIFY_MASK_INPUT_ERROR.\n
          This bit will be set if there is an error in input buffer operation. This bit
          will be set along with the "input buffer consumed" bit to return the input buffer
          with an error indication. */
 uint32_t out_buff_addr;
   /**< If bit 0 of the notify_mask is set, this field indicates the physical
        address of the output buffer. Otherwise ignore. */
 uint32_t in_buff_addr;
   /**< If bit 1 of the notify_mask is set, this field indicates the physical
        address of the input buffer. Otherwise ignore. */
 uint16_t filled_out_size;
   /**< If bit 0 of the notify_mask is set, this field indicates the filled size
        of the output buffer located at the out_buff_addr. Otherwise ignore. */
 uint16_t request_buff_size;
   /**< Request the size of the input buffer. */
 uint16_t sampling_rate;
   /**< Sampling rate of the input/output buffer. Supported values:
        - CSD_VC_IVPCM_SAMPLING_RATE_8K
        - CSD_VC_IVPCM_SAMPLING_RATE_16K */
 uint16_t num_out_channels;
   /**< Number of output channels contained in the filled output buffer. */
};

/**
  Per tap-point vocoder PCM notification issued by CSD_VC, 
  providing a buffer transfer notification to the client. 
  Additionally, this notification is overloaded to provide 
  timetick and various information to the client, depending on 
  the use case, such as: requested output buffer size, requested 
  input buffer size and sampling rate, input buffer consumed 
  notification, output buffer filled notification. The 
  interpretation of this notification is indicated by the 
  notify_mask in the payload. 

  If the timetick bit of notify_mask is set, this notification is used as a 
  timetick which is issued to the client every 'x' milliseconds, where 'x' is
  the PCM frame size which the VDSP operates on, and denotes the boundary upon
  which PCM buffer transfers between the client and CSD_VC 
  occur. If the timetick bit is not set, notify event is used 
  for buffer notifications only. In case of output or input 
  error in buffer operation, the correponding error bit will be 
  set in the notify_mask. 

  Each notify event can accommodate any combination of timetick, output buffer 
  filled or input buffer consumed masks.

  @par Payload
  #csd_vc_ivpcm_evt_notify_v2_t

  @return
  None.

  @dependencies
  None.

  @comments
  After a CSD_VC_IOCTL_IVPCM_CMD_START_V2 command is issued, 
  buffer transfer between the client and CSD_VC commences when 
  the first #CSD_VC_IVPCM_EVT_NOTIFY_V2 event is received by the
  client (it cannot start prior to receiving the first event). 
  First notification is for buffer setup information only. The 
  interpretation of the #CSD_VC_IVPCM_EVT_NOTIFY_V2 event and 
  the information conveyed by it to the client depends on the 
  use case and the time when it is issued: 

  USE CASE 1 -- Output only tap point:

    - First #CSD_VC_IVPCM_EVT_NOTIFY_V2 event after
      #CSD_VC_IOCTL_IVPCM_CMD_START_V2
        - notify_mask = 0
        - Indicates the requested size of the output buffer to be provided by 
          the client.
    - Subsequent #CSD_VC_IVPCM_EVT_NOTIFY_V2 events
        - notify_mask bit 0 set
        - Indicates the previously provided output buffer has been filled and 
          its sampling rate and number of channels are as speciffied by the 
          sampling_rate and num_out_channels fields. Also indicates the size of
          the next output buffer to be provided by the client.

  USE CASE 2 -- Input only tap point:

    - First #CSD_VC_IVPCM_EVT_NOTIFY_V2 event after
      #CSD_VC_IOCTL_IVPCM_CMD_START_V2
        - notify_mask = 0
        - Indicates the requested size and sampling rate of the input buffer to
          be provided by the client.
    - Subsequent #CSD_VC_IVPCM_EVT_NOTIFY_V2 events
        - notify_mask bit 1 set
        - Indicates the previously provided input buffer has been consumed.
          Also indicates the size and sampling rate of the next input buffer to
          be provided by the client.

  USE CASE 3 -- Output-input tap point:

    Sampling rate of the input and output must always be the same in this case.

    - First #CSD_VC_IVPCM_EVT_NOTIFY_V2 event after
      #CSD_VC_IOCTL_IVPCM_CMD_START_V2
        - notify_mask = 0
        - Indicates the requested size of the output buffer to be provided by 
          the client.
    - Second #CSD_VC_IVPCM_EVT_NOTIFY_V2 event after
      #CSD_VC_IOCTL_IVPCM_CMD_START_V2
        - notify_mask bit 0 set
        - Indicates that the first provided output buffer has been filled and 
          its sampling rate and number of channels are as speciffied by the 
          sampling_rate and num_out_channels fields. Also indicates the size
          and sampling rate of the first input buffer to be provided by the 
          client.
    - Subsequent #CSD_VC_IVPCM_EVT_NOTIFY_V2 events
        - notify_mask bits 0 and 1 both set
        - Indicates the previously provided output buffer has been filled and 
          its sampling rate and number of channels are as specified by the 
          sampling_rate and num_out_channels fields.
          Also indicates the previously provided input buffer has been 
          consumed. Also indicates the requested size of the next output buffer
          and the requested size and sampling rate of the next input buffer.
*/
#define CSD_VC_IVPCM_EVT_NOTIFY_V2 ( 0x00012A14 )

/** @brief Type definition for #csd_vc_ivpcm_evt_notify_v2_t.
*/
typedef struct csd_vc_ivpcm_evt_notify_v2_t csd_vc_ivpcm_evt_notify_v2_t;

/** @brief Payload structure for the #CSD_VC_IVPCM_EVT_NOTIFY_V2
 *         command.
*/
struct csd_vc_ivpcm_evt_notify_v2_t
{
  uint32_t tap_point;
    /**< GUID indicating tap point location for which this notification is
         being issued. */
  uint32_t notify_mask;
   /**< Bitmask indicating the notification mode.
        Bitmask description:
        - bit 0 -- output buffer filled, use CSD_VC_IVPCM_NOTIFY_MASK_OUTPUT_BUFFER
        - bit 1 -- input buffer consumed, use CSD_VC_IVPCM_NOTIFY_MASK_INPUT_BUFFER
        - bit 2 -- notify event is a timetick, use CSD_VC_IVPCM_NOTIFY_MASK_TIMETICK
        - bit 3 -- error in output buffer operation, use CSD_VC_IVPCM_NOTIFY_MASK_OUTPUT_ERROR
          This bit will be set if there is an error in output buffer operation. This bit
          will be set along with "output buffer filled" bit to return the output buffer
          with error indication
        - bit 4 -- error in input buffer operation, use CSD_VC_IVPCM_NOTIFY_MASK_INPUT_ERROR
          This bit will be set if there is an error in input buffer operation. This bit
          will be set along with "input buffer consumed" bit to return the input buffer
          with error indication. */
  uint64_t out_buf_mem_address;
    /**< If bit 0 of the notify_mask is set, this field indicates the location
         of the output buffer. Otherwise ignore. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how
         the address is intrepreted. */
  uint64_t in_buf_mem_address;
    /**< If bit 1 of the notify_mask is set, this field indicates the location
         of the input buffer. Otherwise ignore. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how
         the address is intrepreted. */
  uint16_t filled_out_size;
    /**< If bit 0 of the notify_mask is set, this field indicates the filled 
         size of the output buffer located at the out_buf_mem_address.
         Otherwise ignore. */
  uint16_t request_buf_size;
    /**< Request size of the input buffer. */
  uint16_t sampling_rate;
     /**< Sampling rate of the input/output buffer. Supported values:
        - CSD_VC_IVPCM_SAMPLING_RATE_8K
        - CSD_VC_IVPCM_SAMPLING_RATE_16K */
  uint16_t num_out_channels;
    /**< Number of output channels contained in the filled output buffer. */
};

/**
  Response to the following memory mapping command:\n
  #CSD_VC_IOCTL_CMD_MAP_MEMORY_V2.\n


  @par Payload
  #csd_vc_rsp_map_t

  @return
  None.

  @dependencies
  None.
 */
#define CSD_VC_COMMAND_RSP_MAP ( 0x00012A0A )

/** @brief Type definition for #csd_vc_rsp_map_t.
*/
typedef struct csd_vc_rsp_map_t csd_vc_rsp_map_t;

/** @brief Payload structure for the #CSD_VC_COMMAND_RSP_MAP command.
*/
struct csd_vc_rsp_map_t
{
  uint32_t mem_handle;
    /**< Memory handle encapsulating shared memory attributes. The client
         provides this handle to CSD whenever a memory address or offset is 
         passed to CSD for sharing data. */
};


/** @} */ /* end_addtogroup csd_vc */

#endif /* __CSD_VC_H__ */

