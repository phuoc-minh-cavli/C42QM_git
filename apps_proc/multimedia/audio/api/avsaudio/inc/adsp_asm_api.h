/*========================================================================*/
/**
@file adsp_asm_api.h

@brief This file contains all other ASM header files covering service, stream,
session, data commands and events. Users need only include this file
for ASM API.
*/

/*===========================================================================
Copyright (c) 2012 Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
======================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/audio.tx/2.0/audio/api/avsaudio/inc/adsp_asm_api.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
05/30/11   sw      (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
04/15/10   rkc      Created file.
========================================================================== */

#ifndef _ADSP_ASM_API_H_
#define _ADSP_ASM_API_H_

/**
 * API_developer_note:
 *
 *  A-family APIs deleted in B-family (left) & corresponding B family APIs (right):
 *    ASM_SVC_CMD_GET_WALLCLOCK_TIME
 *    ASM_SVC_CMDRSP_GET_WALLCLOCK_TIME
 *    ASM_STREAM_CMD_OPEN_WRITE
 *    ASM_STREAM_CMD_OPEN_READ
 *    ASM_STREAM_CMD_OPEN_READWRITE
 *    ASM_STREAM_CMD_OPEN_LOOPBACK
 *    ASM_SESSION_CMD_GET_SESSION_TIME                  -> ASM_SESSION_CMD_GET_SESSION_TIME_V3
 *    ASM_SESSION_CMD_GET_SESSION_TIME_V2				-> ASM_SESSION_CMD_GET_SESSION_TIME_V3
 *    ASM_SESSION_CMDRSP_GET_SESSION_TIME				-> ASM_SESSION_CMDRSP_GET_SESSION_TIME_V3
 *    ASM_SESSION_CMDRSP_GET_SESSION_TIME_V2			-> ASM_SESSION_CMDRSP_GET_SESSION_TIME_V3
 *    ASM_SESSION_CMD_ADJUST_SESSION_CLOCK              -> ASM_SESSION_CMD_ADJUST_SESSION_CLOCK_V2
 *    ASM_SESSION_CMDRSP_ADJUST_SESSION_CLOCK           -> ASM_SESSION_CMDRSP_ADJUST_SESSION_CLOCK_V2
 *    ASM_MEDIA_FMT_MULTI_CHANNEL_PCM                   -> ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2
 *    ASM_MEDIA_FMT_LINEAR_PCM
 *    ASM_SESSION_CMD_RUN                               -> ASM_SESSION_CMD_RUN_V2
 *    ASM_CMD_MEMORY_MAP
 *    ASM_CMD_MEMORY_UNMAP
 *    ASM_CMD_MEMORY_MAP_REGIONS                        -> ASM_CMD_SHARED_MEM_MAP_REGIONS
 *    ASM_CMD_MEMORY_UNMAP_REGIONS                      -> ASM_CMD_SHARED_MEM_UNMAP_REGIONS, ASM_CMDRSP_SHARED_MEM_MAP_REGIONS
 *    ASM_STREAM_CMD_GET_PP_PARAMS                      -> ASM_STREAM_CMD_GET_PP_PARAMS_V2
 *    ASM_STREAM_CMDRSP_GET_PP_PARAMS					-> ASM_STREAM_CMDRSP_GET_PP_PARAMS_V2
 *    ASM_STREAM_CMD_SET_PP_PARAMS                      -> ASM_STREAM_CMD_SET_PP_PARAMS_V2
 *    ASM_DATA_CMD_READ                                 -> ASM_DATA_CMD_READ_V2
 *    ASM_DATA_CMD_WRITE                                -> ASM_DATA_CMD_WRITE_V2
 *    ASM_DATA_EVENT_READ_DONE                          -> ASM_DATA_EVENT_READ_DONE_V2
 *    ASM_DATA_EVENT_WRITE_DONE							-> ASM_DATA_EVENT_WRITE_DONE_V2
 *    ASM_DATA_CMD_SPA                                  -> ASM_DATA_CMD_SPA_V2
 *    ASM_DATA_EVENT_SPA_DONE                           -> ASM_DATA_EVENT_SPA_DONE_V2
 *    ASM_DATA_CMD_MEDIA_FMT_UPDATE                     -> ASM_DATA_CMD_MEDIA_FMT_UPDATE_V2
 *    ASM_SESSION_CMD_CONNECT_AFE_PORT
 *
 *    ASM_PARAM_ID_ENCDEC_IMM_DECODE
 *    ASM_PARAM_ID_ENCDEC_ENC_CFG_BLK                   -> ASM_PARAM_ID_ENCDEC_ENC_CFG_BLK_V2
 *
 *    ASM_PARAM_ID_AC3_NUM_OUTPUT_CHANNELS
 *    ASM_PARAM_ID_AC3_BITS_PER_SAMPLE
 *    ASM_PARAM_ID_AC3_LFE_MODE
 *    ASM_PARAM_ID_AC3_CHANNEL_ROUTING
 *    ASM_PARAM_ID_AC3_CHANNEL_CONFIG
 *
 *    ASM_PARAM_ID_WMAPRO_BITS_PER_SAMPLE
 *    ASM_PARAM_ID_WMAPRO_CHANNEL_MASK
 *
 *    ASM_MEDIA_FMT_AMR_WB_PLUS                         -> ASM_MEDIA_FMT_AMR_WB_PLUS_V2
 *    ASM_MEDIA_FMT_WMA_V10PRO                          -> ASM_MEDIA_FMT_WMA_V10PRO_V2
 *    ASM_MEDIA_FMT_WMA_V9                              -> ASM_MEDIA_FMT_WMA_V9_V2
 *    ASM_MEDIA_FMT_AAC                                 -> ASM_MEDIA_FMT_AAC_V2
 *
 *    PCM_CHANNEL_NULL
 *
 */


/** @ingroup versions
    Hexagon ASM version ID.

    API_developer_note: in place of 7 in 0x00070001, 1,2,...,6 are for A family. 7 onwards are for B family.
    @newpage */
#define ADSP_ASM_VERSION    0x00070001

#include "adsp_asm_service_commands.h"
#include "adsp_asm_stream_commands.h"
#include "adsp_asm_session_commands.h"
#include "adsp_asm_data_commands.h"

#endif /*_ADSP_ASM_API_H_*/
