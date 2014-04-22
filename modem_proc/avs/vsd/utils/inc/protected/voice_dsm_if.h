#ifndef __VOICE_DSM_IF_H__
#define __VOICE_DSM_IF_H__

/**
  @file  voice_dsm_if.h
  @brief This is the public header file that clients of GVA should include.
         This file includes all other GVA public header files and contains
         single entry point into the GVA.
*/

/*
  ============================================================================
  Copyright (C) 2015, 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/vsd/utils/inc/protected/voice_dsm_if.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
  Include files for Module
****************************************************************************/

/* SYSTEM UTILS APIs. */
#include "dsmutil.h"
#include "mmdefs.h"

/* SELF APIs. */
#include "voice_amr_if.h"


/****************************************************************************
 * VOICE DSM UTILITY ROUTINES                                               *
 ****************************************************************************/


/**
 * Initialize AMR DSM buffers and should be called only once.
 */
void voice_dsm_amr_q_init ( 
  voice_amr_dsm_queue_t* amr_queue,
  uint32_t queue_len
);


/**
 * Put AMR packets into uplink DSM queues.
 */
void voice_dsm_queue_ul_amr_speech (
  amrsup_core_speech_type *ul_speech,
  voice_amr_chan_state_t* chan_state,
  voice_amr_dsm_queue_t* amr_dsm_q
);


/**
 * Extract and decode AMR packets from downlink DSM queues in normal mode.
 */
void voice_dsm_dequeue_dl_amr_apeech (
 amrsup_core_speech_type* dl_speech,
 voice_amr_dsm_queue_t* amr_dsm_q
);


/**
 * It clears Uplink DSM queues.
 */
void voice_dsm_amr_q_empty (
  voice_amr_dsm_queue_t *amr_queues
);


#endif  /* __VOICE_DSM_IF_H__ */

