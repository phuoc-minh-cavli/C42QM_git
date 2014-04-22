/*
   DESCRIPTION
   This file contains definitions for processing packets in voice adapter
   DSM pools.

   Copyright (C) 2015, 2017-2018 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.mpss/8.0.c12/vsd/utils/src/voice_dsm.c#1 $
   $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "comdef.h"
#include "msg.h"
#include "err.h"

/* Voice utils APIs. */
#include "amrsup.h"
#include "voice_dsm_if.h"


/****************************************************************************
 * VOICE DSM UTILITY ROUTINES                                               *
 ****************************************************************************/


void voice_dsm_amr_q_empty (
   voice_amr_dsm_queue_t *amr_queues
)
{
  if ( amr_queues == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "voice_dsm_amr_q_empty(): Provided AMR queue are NULL.");
    return;
  }

#ifndef WINSIM
  dsm_empty_queue(&(amr_queues->wm_a));
  dsm_empty_queue(&(amr_queues->wm_b));
  dsm_empty_queue(&(amr_queues->wm_c));
#endif /* WINSIM */

  return;
}


void voice_dsm_amr_q_init(
  voice_amr_dsm_queue_t* amr_dsm_wms,
  uint32_t queue_len
)
{
  if (amr_dsm_wms==NULL)
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"AMR DSM queue init failed.");
    return;
  }

#ifndef WINSIM

  /* Initialize class A watermark */
  dsm_queue_init( &amr_dsm_wms->wm_a, ( AMRSUP_CLASS_A_BYTES * queue_len + 1 ),
                  &amr_dsm_wms->queue_a );
  amr_dsm_wms->wm_a.hi_watermark = AMRSUP_CLASS_A_BYTES;
  amr_dsm_wms->wm_a.lo_watermark = 0;

  /* Initialize class B watermark */
  dsm_queue_init( &amr_dsm_wms->wm_b, ( AMRSUP_CLASS_B_BYTES * queue_len + 1 ),
                  &amr_dsm_wms->queue_b );
  amr_dsm_wms->wm_b.hi_watermark = AMRSUP_CLASS_B_BYTES;
  amr_dsm_wms->wm_b.lo_watermark = 0;

  /* Initialize class C watermark */
  dsm_queue_init( &amr_dsm_wms->wm_c, ( AMRSUP_CLASS_C_BYTES * queue_len + 1 ),
                  &amr_dsm_wms->queue_c );
  amr_dsm_wms->wm_c.hi_watermark = AMRSUP_CLASS_C_BYTES;
  amr_dsm_wms->wm_c.lo_watermark = 0;

#endif /* WINSIM */

  return;
}


#ifndef WINSIM

#if 0
/**
 * DSM Watermark calback function.
 *
 * This is invoked whenever queue goes non empty.
 */
void voice_dsm_watermark_cb (
  dsm_watermark_type *dsm_watermark,
  void *param
)
{
  return;
  //dsm_empty_queue( dsm_watermark );
}
#endif


void voice_dsm_queue_ul_amr_speech (
  amrsup_core_speech_type *ul_speech,
  voice_amr_chan_state_t* chan_state,
  voice_amr_dsm_queue_t* amr_dsm_q
)
{
  /* to check changes of channels setup */
  voice_amr_dsm_queue_t *amr_q = amr_dsm_q;
  dsm_item_type* dsm_item_a = NULL;
  dsm_item_type* dsm_item_b = NULL;
  dsm_item_type* dsm_item_c = NULL;
  uint32 checkpoint = 0;

  if ( ul_speech == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "voice_dsm_queue_ul_amr_speech(): ul_speech NULL" );
    return;
  }

  /* check if class A data would be lost due to channel A setup */
  if ((chan_state->has_chan_a != TRUE) && (ul_speech->num_bits_a > 0))
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "Warning loss of uplink class A data" );
  }

  /* check if class B data would be lost due to channel B setup */
  if ((chan_state->has_chan_b != TRUE) && (ul_speech->num_bits_b > 0))
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "Warning loss of uplink class B data" );
  }

  /* check if class C data would be lost due to channel C setup */
  if ((chan_state->has_chan_c != TRUE) && (ul_speech->num_bits_c > 0))
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
         "Warning loss of uplink class C data" );
  }

  if ( q_cnt( amr_q->wm_a.q_ptr ) == 1 )
  {
    voice_dsm_amr_q_empty( amr_q );
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "voice_dsm_queue_ul_amr_speech(): UL PACKET discarded, Previous packet not "
         "taken consumed at RLC" );
  }

  for ( ;; )
  {
    /* ----- Preparing class C DSM items if channel C is active ----- */
    if ( chan_state->has_chan_c == TRUE )
    {
      if ( ul_speech->num_bits_c > 0 )
      {
        dsm_item_c = dsm_sdu_to_pkt_chain_bit( ul_speech->data_c,
                                               ( uint16 ) ul_speech->num_bits_c );
      }
      else
      {
        dsm_item_c = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );
      }
  
      if ( dsm_item_c == NULL )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "Can't get DSM item for class C AMR data." );
        checkpoint = 1;
      }
    }
  
    /* ----- Preparing class B DSM items if channel B is active ----- */
    if ( chan_state->has_chan_b == TRUE )
    {
      if ( ul_speech->num_bits_b > 0 )
      {
        dsm_item_b = dsm_sdu_to_pkt_chain_bit( ul_speech->data_b,
                                               ( uint16 ) ul_speech->num_bits_b );
      }
      else
      {
        dsm_item_b = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );
      }
  
      if ( dsm_item_b == NULL )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "Can't get DSM item for class B AMR data." );
        checkpoint = 2;
      }
    }
  
    /* ----- Preparing class A DSM items if channel A is active ----- */
    if ( chan_state->has_chan_a == TRUE )
    {
      if ( ul_speech->num_bits_a > 0 )
      {
        dsm_item_a = dsm_sdu_to_pkt_chain_bit( ul_speech->data_a,
                                               ( uint16 ) ul_speech->num_bits_a );
      }
      else
      {
        dsm_item_a = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );
      }

      if (dsm_item_a == NULL)
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "Can't get DSM item for class A AMR data." );
      }
    }

    /* ---------- Queuing DSM items ---------- */
    if (dsm_item_c != NULL)
    {
      dsm_simple_enqueue_isr( &(amr_q->wm_c), &dsm_item_c );
    }
    if (dsm_item_b != NULL)
    {
      dsm_simple_enqueue_isr( &(amr_q->wm_b), &dsm_item_b );
    }
    if (dsm_item_a != NULL)
    {
      dsm_simple_enqueue_isr( &(amr_q->wm_a), &dsm_item_a );
    }

    break;
  }

  switch ( checkpoint )
  {
   case 2:
     {
       dsm_free_packet( &dsm_item_b );
       dsm_item_b = NULL;
     } //pass through
   case 1:
     {
       dsm_free_packet( &dsm_item_c );
       dsm_item_c = NULL;
     } //pass through

  }

  return;
}


void voice_dsm_dequeue_dl_amr_apeech (
  amrsup_core_speech_type* dl_speech,
  voice_amr_dsm_queue_t* amr_dsm_q
)
{
  voice_amr_dsm_queue_t* amr_q = amr_dsm_q;
  dsm_item_type *dsm_item = NULL;

  if ( dl_speech == NULL )
  {
    /* Clear the downlink queue instantly to avoid overflow and RTD increase
       over DL. */
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "voice_dsm_dequeue_dl_amr_apeech(): DSM WM qcnt q_a=%d, q_b=%d, "
           "q_c= %d", q_cnt( amr_q->wm_a.q_ptr ), q_cnt( amr_q->wm_b.q_ptr ),
           q_cnt( amr_q->wm_c.q_ptr ) );

    voice_dsm_amr_q_empty( amr_dsm_q );
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "voice_dsm_dequeue_dl_amr_apeech(): dl_speech NULL, DL PACKET lost" );
    return;
  }

  /* return empty speech if no downlink packet is available */
  dl_speech->num_bits_a = 0;
  dl_speech->num_bits_b = 0;
  dl_speech->num_bits_c = 0;

  /* ========== Extract downlink DSM packets ========== */

  /* Revisit loop. */
  while ( ( q_cnt( amr_q->wm_a.q_ptr ) > 0 ) || 
          ( q_cnt( amr_q->wm_b.q_ptr ) > 0 ) ||
          ( q_cnt( amr_q->wm_c.q_ptr ) > 0 ) )
  {

    /* extract class A data */
    dsm_item = dsm_dequeue( &(amr_q->wm_a) );
    if ( dsm_item == NULL )
    {
      dl_speech->num_bits_a = 0;
    }
    else
    {
      dl_speech->num_bits_a  = (int) dsm_pkt_to_sdu_chain_bit( &dsm_item,
                                                               dl_speech->data_a );
    }

    /* extract class B data */
    dsm_item = NULL;
    dsm_item = dsm_dequeue( &(amr_q->wm_b) );
    if (dsm_item==NULL)
    {
      dl_speech->num_bits_b = 0;
    }
    else
    {
      dl_speech->num_bits_b  = (int) dsm_pkt_to_sdu_chain_bit( &dsm_item,
                                                               dl_speech->data_b );
    }

    /* extract class C data */
    dsm_item = NULL;
    dsm_item = dsm_dequeue( &(amr_q->wm_c) );
    if (dsm_item==NULL)
    {
       dl_speech->num_bits_c = 0;
    }
    else
    {
      dl_speech->num_bits_c  = (int) dsm_pkt_to_sdu_chain_bit( &dsm_item,
                                                               dl_speech->data_c );
    }
  } /* end while loop */

  return;
}

#endif /* !WINSIM */

