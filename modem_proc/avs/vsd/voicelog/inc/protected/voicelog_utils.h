#ifndef __VOICELOG_UTILS_H__
#define __VOICELOG_UTILS_H__

/**
  @file  voicelog_utils.h
  @brief This is the protected header file that clients should include 
         to aid vocoder packet logging.
*/

/*
  ============================================================================
  Copyright (C) 2015, 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/vsd/voicelog/inc/protected/voicelog_utils.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
  Include files for Module
****************************************************************************/

/* SYSTEM UTILITY APIs. */
#include "mmdefs.h"

/* APR APIs. */
#include "apr_comdef.h"
#include "apr_errcodes.h"
#if ( defined WINSIM || defined AVS_MPSS_TEST )
#include "qurt_elite.h"
#endif

/****************************************************************************
  VOICELOG DEFINITION
****************************************************************************/

typedef enum voicelog_info_enum_t {

  VOICELOG_INFO_ENUM_NONE,
  VOICELOG_INFO_ENUM_CALL_NUMBER,
  VOICELOG_INFO_ENUM_VOCODER_SESSION_NUMBER,
  VOICELOG_INFO_ENUM_INVALID
}
  voicelog_info_enum_t;

/****************************************************************************
 * VOICELOG UTILITY FUNCTIONS                                                    *
 ****************************************************************************/

/**
 * Initialize the vocoder packet logging session instance corresponding
 * to a voice sub-system (VSID).
 *
 * @param[in] vsid.
 *
 * @return APR_EOK when successful.
 */
uint32_t voicelog_session_init ( 
  uint32_t vsid
);


/**
 * Updates the log session parameters identified by the corresponding info_id.
 * 
 * @param[in] vsid.
 * @param[in] info_id. Identifies the field that needs to be updated.
 *
 * @return APR_EOK when successful.
 */
uint32_t voicelog_session_update ( 
  uint32_t vsid,
  voicelog_info_enum_t info_id
);


/**
 * Updates the frame header corresponding to media id and frame info.
 * 
 * @param[in]  vs_media_id.
 * @param[in]  frame_info. Identifies the field that needs to be updated.
 * @param[out] frame_header.
 * @return APR_EOK when successful.
 */
uint32_t voicelog_event_report ( 
  sys_modem_as_id_e_type  asid,
  uint32_t vsid,
  uint8_t  vocoder_state,
  uint32_t vs_network_id,
  uint32_t vs_media_id
);

#endif /* __VOICELOG_UTILS_H__ */
