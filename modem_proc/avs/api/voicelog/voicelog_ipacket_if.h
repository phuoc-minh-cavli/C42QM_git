#ifndef __VOICELOG_IPACKET_IF_H__
#define __VOICELOG_IPACKET_IF_H__

/**
  @file  voicelog_ipacket_if.h
  @brief This file contains interface definitions of voice packet logging. 
*/

/*
  ============================================================================

   Copyright (C) 2015 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //components/rel/avs.mpss/8.0.c12/api/voicelog/voicelog_ipacket_if.h#1 $
  $Author: pwbldsvc $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------


  ============================================================================
*/

/***************************************************************************
  Include files for Module                                                 *
****************************************************************************/

/* SYSTEM UTILITY APIs. */
#include "mmdefs.h"

/****************************************************************************
  VOICE LOG INTERFACE DEFINES                                               *
*****************************************************************************/

/** Log Codes Definitions supported by voice.
 *
 *  Log codes are allocated and maintained by DIAG team. Please refer to 
 *  <root>/modem_proc/core/api/services/log_codes.h for more details.
 */
#define VOICELOG_IPACKET_CODE_UL_VOCODER_PACKET ( 0x1914 )
#define VOICELOG_IPACKET_CODE_DL_VOCODER_PACKET ( 0x1915 )

/** Tap Point Ids: It identifies the module that is logging the vocoder packet. 
 */
#define VOICELOG_IPACKET_TAP_POINT_ID_IMS ( 0x000131E6 )
#define VOICELOG_IPACKET_TAP_POINT_ID_VOICE_ADAPTER ( 0x000131E7 )
#define VOICELOG_IPACKET_TAP_POINT_ID_VOICE_SERVICE ( 0x000131E8 )


/** Version: This identifies the Vocoder packet data structure.
 */
#define VOICELOG_IPACKET_VOCODER_DATA_VERSION_V ( 0x0001 )

/****************************************************************************
 * VOICE LOG INTERFACE DEFINITION                                           *
 ****************************************************************************/

/**
  Logs the Uplink/Downlink vocoder packets data corresponding to a voice
  sub-system. This is synchronous blocking API.
 
  @par Payload
  #voicelog_ipacket_cmd_commit_data_t.

  @return
  #APR_EOK in case of success.

  @dependencies
  None.

  @comments
  None.
*/

#define VOICELOG_IPACKET_CMD_COMMIT_DATA ( 0x000131EB )

/** @brief Type definition for #voicelog_ipacket_cmd_commit_data_t.
*/
typedef struct voicelog_ipacket_cmd_commit_data_t voicelog_ipacket_cmd_commit_data_t;

/** @brief Payload structure for the #VOICELOG_IPACKET_CMD_COMMIT_DATA command.
*/
 struct voicelog_ipacket_cmd_commit_data_t {
 
  uint16_t version;
    /**<
     * Vocoder data format version. The currently supported version is 
     * VOICELOG_IPACKET_VOCODER_DATA_VERSION_V.
     * Any changes are backward compatible.
     */
  uint32_t log_code;
    /**< Identifies the Uplink/Downlink Log packet. */
  uint32_t vsid;
    /**< Identifies the voice-subsystem involved. */
  uint32_t network_id;
    /**< Identifies the network involved. See VOICELOG_ICOMMON_NETWORK_ID_XXX. */
  uint64_t timestamp;
    /**< Time stamp whose definition is client specific. */
  uint32_t tap_point_id;
    /**< 
     * Identifies the Client logging the vocoder packet data. see
     * VOICELOG_IPACKET_TAP_POINT_XXX.
     */
  uint32_t media_id;
    /**< 
     * The media ID determines the data type of the frame data.
     * See VOICELOG_ICOMMON_MEDIA_ID_XXX. 
     */
  uint8_t  frame_header;
    /**< Contains vocoder frame type and rate information. */
  uint8_t* frame;
    /**< 
     * Reference to vocoder frame data.
     * The client shall be responsible for allocating memory and sharing
     * valid reference to frame.
     */
  uint32_t frame_size;
    /**< 
     * The size of the frame data excluding the frame header 
     * (frame header size is 1byte). 
     */
  void* data_extension;
    /**< 
     * This field is for future expansions. Version field determines the
     * data type associated with this field.
     * For VOICELOG_IPACKET_VOCODER_DATA_VERSION_V, this should be NULL.
     */
 };

#endif /* __VOICELOG_IPACKET_IF_H__ */
