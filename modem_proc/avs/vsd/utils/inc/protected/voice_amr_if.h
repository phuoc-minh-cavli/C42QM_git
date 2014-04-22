#ifndef __VOICE_AMR_IF_H__
#define __VOICE_AMR_IF_H__

/**
  @file  voice_amr_if.h
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
  $Header: //components/rel/avs.mpss/8.0.c12/vsd/utils/inc/protected/voice_amr_if.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
  Include files for Module
****************************************************************************/

/* SYSTEM UTILS APIs. */
#include "dsmutil.h"
#include "mmdefs.h"

/* VSD APIs*/
#include "amrsup.h"
#include "vs.h"

/****************************************************************************
  VOICE AMR STRUCTURE DEFINATION
****************************************************************************/

/**
 * DSM Watermark vased AMR queue.
 */
typedef struct voice_amr_dsm_queue_t voice_amr_dsm_queue_t;

struct voice_amr_dsm_queue_t {

  dsm_watermark_type wm_a;
  dsm_watermark_type wm_b;
  dsm_watermark_type wm_c;
  q_type queue_a;
  q_type queue_b;
  q_type queue_c;
};


/**
 * AMR Logical channels state type.
 */
typedef struct voice_amr_chan_state_t voice_amr_chan_state_t;

struct voice_amr_chan_state_t {

  boolean has_chan_a; 
    /**< channel A exists */
  uint8_t lca;
    /**< class A logical channel ID. */

  boolean has_chan_b;
    /**< channel B exists */
  uint8_t lcb;                    
    /**< class B logical channel ID. */

  boolean has_chan_c;  
    /**< channel C exists */
  uint8_t lcc;                  
    /**< class C logical channel ID. */

};


/****************************************************************************
 * VOICE AMR UTILITY ROUTINES                                               *
 ****************************************************************************/

/**
 * This function processes uplink data and transports it over the DSM queue
 * initialized in the mvssup_wcdma_set_ul_channel or 
 * mvssup_tdscdma_set_ul_channel.
 */
uint32_t voice_amr_ul_processing (
  uint32_t vs_media_id,
  uint8_t* frame,
  voice_amr_chan_state_t* ul_chan_state,
  voice_amr_dsm_queue_t* amr_dsm_q
);



/**
 * This function retrieves downlink data from the DSM queue initialized in the
 * mvssup_wcdma_set_dl_channel or mvssup_tdscdma_set_dl_channel and processes 
 * it for vocoder use.
 */
uint32_t voice_amr_dl_processing (
  vs_pktexg_buffer_t* vs_buffer,
  voice_amr_dsm_queue_t* amr_dsm_q
);

#endif /* __VOICE_AMR_IF_H__ */

