/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC UPLINK TEST MODULE
***
***
*** DESCRIPTION
***
***  This module implements functions required for diagnostic purposes as well
***  as functions to aid Host test and general debugging
***
*** EXTERNALIZED FUNCTIONS
***
***  uint32  grlc_ul_pack_queue_info (void)
***  void grlc_ul_show_block_sent(uint16 bsn, tx_ack_state_t ack_state)
***  void grlc_ul_show_paq (void)
***  void grlc_ul_show_new_alloc(mr_ul_alloc_cnf_t *old_ptr, mr_ul_alloc_cnf_t *ptr)
***  void grlc_ul_show_alloc_cnf_await_ass (mr_ul_alloc_cnf_t *ptr)
***  void grlc_ul_show_llc_pdu_type (gprs_grr_ul_mes_t *ptr)
***  void grlc_ul_show_tx_vars(void)
***  void show_rbb (rm_acknack_desc_t *desc_ptr, uint8 fai)
***  void grlc_ul_show_vars (void)
***  uint16 rlc_ul_octets_rem (void)
***  uint16 rlc_ul_get_timer_cnt (uint8 bsn)
***  void rlc_ul_show_state_vars(rlc_ul_show_state_vars)
***  void rlc_ul_show_sv(void)
***  uint8 rlc_get_data_blk_size (uint8 cs)
***  uint16 rlc_ul_get_bs_cv_max_wait_ms (void)
***  uint16 rlc_ul_get_bs_cv_max (void)
***  uint16 rlc_ul_get_block_period_ms (void)
***  void rlc_ul_set_bs_cv_max (uint8 by)
***  void fill_ack_bit (const uint8 bsn, const uint8 ssn,uint8 *ptr)
***  void rlc_ul_scan_tx_array_for_non_null_dsm_ptrs (void)
***  static void block_timer_cb (int4 interval)
***  void xlate_u2d (grlc_ul_pl1_data_t  *pub, pl1_radio_block_t   *pdrb)
***  void grlc_diag_show_ul (void)
***  void show_group_status (void)
***  const grlc_diag_ul_statistics_t* grlc_diag_get_ul_statistics (void)
***  void grlc_diag_clr_ul_statistics (void)
***  const grlc_diag_ul_abnormal_release_counts_t*
***     grlc_diag_get_ul_abnormal_release_counts (void)
***  void grlc_diag_clr_ul_abnormal_release_counts (void)
***  const grlc_diag_ul_event_counts_t* grlc_diag_get_ul_event_counts (void)
***  void grlc_diag_clr_ul_event_counts (void)
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //components/rel/geran.mpss/5.2.0/grlc/src/grlcultst.c#5 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 11/24/03    hv     Created
*** 08/03/04    hv     Added EGPRS code
*** 04/07/13    hv     CR461047 - Make safe usage of memcpy() and memmove() in GRLC
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include <stringl.h>

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "comdef.h"
#include "queue.h"

#include "gmutex.h"

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST  */

#include "grlcultst.h"
#include "grlcllc.h"
#include "grlctsk.h"
#include "grlc_dyn_api.h"

#ifdef GSM_GPRS_GRLU_CONFIRM_TX
static post_tx_blk_fifo_t  grlc_ul_post_tx_blk_fifo;
#endif /* GSM_GPRS_GRLU_CONFIRM_TX */

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


/*===========================================================================
===
===  FUNCTION      GRLC_UL_PACK_QUEUE_INFO
===
===  DESCRIPTION
===
===    Return LLC PDU queue count, number of PDUs held in 'ul' struct and
===    number of octets as a packed 32 bit hex value
===
===  DEPENDENCIES
===
===  RETURN VALUE
===    uint32
===
===  SIDE EFFECTS
===
===========================================================================*/
uint32  grlc_ul_pack_queue_info (void)
{
  uint32  lu;

  /* Np Oc te ts    Np: number of PDUs, Octets: number of octets
  **  |  |  |  |
  **  |  |  |  |
  ** 24
  */
  lu  = 0L;
  lu  = (uint32)ul.llc_octets & 0xffffff;
  lu |= ((uint32)ul.llc_pdus & 0xff) << 24;

  return lu;
}

#ifdef  GSM_GPRS_GRLU_SHOW_BLOCK_SENT_TO_FIFO
/*===========================================================================
===
===  FUNCTION      GRLC_UL_SHOW_BLOCK_SENT
===
===  DESCRIPTION
===
===     Show the block type, BSN, CV, payload length of the data block sent
===     to the UL FIFO
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_block_sent(uint16 bsn, tx_ack_state_t ack_state)
{
  switch(ack_state)
  {
    case TXA_PTX:
#ifdef  GSM_GPRS_GRLU_SHOW_PTX
      if (ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
      {
        MSG_GH3 ("ERLU Sent PTX %d CV %d l %d",
                  bsn,
                  TX_ARR( bsn).blk.hdr.cv,
                  TX_ARR( bsn).blk.data_len);
      }
      else
      {
        MSG_GH3 ("GRLU Sent PTX %d CV %d l %d",
                  bsn,
                  TX_ARR( bsn).blk.mac_hdr.type.ul_data.cv,
                  TX_ARR( bsn).blk.data_len);
      }
#endif
      break;

    case TXA_PACK:
#ifdef  GSM_GPRS_GRLU_SHOW_PACK
      if (ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
      {
        MSG_GH3 ("ERLU Sent PAK %d CV %d l %d",
                  bsn,
                  TX_ARR( bsn).blk.hdr.cv,
                  TX_ARR( bsn).blk.data_len);
      }
      else
      {
        MSG_GH3 ("GRLU Sent PAK %d CV %d l %d",
                  bsn,
                  TX_ARR( bsn).blk.mac_hdr.type.ul_data.cv,
                  TX_ARR( bsn).blk.data_len);
      }
#endif
      break;

    case TXA_ACK:
      if (ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
      {
        MSG_GH3 ("ERLU Sent ACK %d CV %d l %d",
                  bsn,
                  TX_ARR( bsn).blk.hdr.cv,
                  TX_ARR( bsn).blk.data_len);
      }
      else
      {
      MSG_GH3 ("GRLU Sent ACK %d CV %d l %d",
                bsn,
                TX_ARR( bsn).blk.mac_hdr.type.ul_data.cv,
                TX_ARR( bsn).blk.data_len);
      }
      break;

    case TXA_UNACK:
#ifdef  GSM_GPRS_GRLU_SHOW_UNACK
      if (ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
      {
        MSG_GH3 ("ERLU Sent UNA %d CV %d l %d",
                  bsn,
                  TX_ARR( bsn).blk.hdr.cv,
                  TX_ARR( bsn).blk.data_len);
      }
      else
      {
        MSG_GH3 ("GRLU Sent UNA %d CV %d l %d",
                  bsn,
                  TX_ARR( bsn).blk.mac_hdr.type.ul_data.cv,
                  TX_ARR( bsn).blk.data_len);
      }
#endif
      break;

    case TXA_PENDING_PACK:
      if (ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
      {
        MSG_GH3 ("ERLU Sent PPA %d CV %d l %d",
                  bsn,
                  TX_ARR( bsn).blk.hdr.cv,
                  TX_ARR( bsn).blk.data_len);
      }
      else
      {
        MSG_GH3 ("GRLU Sent PPA %d CV %d l %d",
                bsn,
                TX_ARR( bsn).blk.mac_hdr.type.ul_data.cv,
                TX_ARR( bsn).blk.data_len);
      }
      break;

    case TXA_NACK:
#ifdef  GSM_GPRS_GRLU_SHOW_NACK
      if (ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
      {
        MSG_GH3 ("ERLU Sent NAK %d CV %d l %d",
                  bsn,
                  TX_ARR( bsn).blk.hdr.cv,
                  TX_ARR( bsn).blk.data_len);
      }
      else
      {
        MSG_GH3 ("GRLU Sent NAK %d CV %d l %d",
                  bsn,
                  TX_ARR( bsn).blk.mac_hdr.type.ul_data.cv,
                  TX_ARR( bsn).blk.data_len);
      }
#endif
      break;

    case TXA_INVALID:
    case TXA_INACTIVE:
    default:
      break;
  }
} /* grlc_ul_show_block_sent */

#endif /* GSM_GPRS_GRLU_SHOW_BLOCK_SENT_TO_FIFO */

#ifdef GSM_GPRS_GRLU_CONFIRM_TX

/*===========================================================================
===
===  FUNCTION        GRLC_UL_INIT_POST_TX_SHOW_FIFO
===
===  DESCRIPTION
===
===     Clear the post-tx-block-fifo
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_init_post_tx_show_fifo (void)
{
  GRLC_UL_TX_VARS_LOCK();

  grlc_ul_post_tx_blk_fifo.in_ind = 0;
  grlc_ul_post_tx_blk_fifo.out_ind= 0;
  grlc_ul_post_tx_blk_fifo.ctr= 0;

  GRLC_UL_TX_VARS_UNLOCK();
}

/*===========================================================================
===
===  FUNCTION        GRLC_UL_QUEUE_BLK_FOR_SHOW
===
===  DESCRIPTION
===
===     This function inserts the BSN and ack-status of a data block into the
===     post-tx-blk-fifo. This function is called from post_tx_actions(), ie.
===     from PL1's ISR such that the action of formatting and calling MSG_X()
===     are deferred from the ISR and executed in RLC's task context
===
===     Pushes BSN and ack_state into the global variable
===     grlc_ul_post_tx_blk_fifo
===
===     No INT-LOCK used because it is designed to be called from the
===     post_tx_actions() which is called by the ISR callback
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_queue_blk_for_show
(
  uint16          bsn,
  tx_ack_state_t  ack_state
)
{

  GRLC_UL_TX_VARS_LOCK();

  if (grlc_ul_post_tx_blk_fifo.ctr >= POST_TX_SHOW_ITEM_MAX)
  {
    /* Counter overflow: show error and clear buffer
    */
    MSG_GERAN_LOW_1_G("GRLU post msg buf(%u) OVF", POST_TX_SHOW_ITEM_MAX);
    grlc_ul_init_post_tx_show_fifo();
  }
  else
  {
    grlc_ul_post_tx_blk_fifo.show_fifo[grlc_ul_post_tx_blk_fifo.in_ind].bsn = bsn;
    grlc_ul_post_tx_blk_fifo.show_fifo[grlc_ul_post_tx_blk_fifo.in_ind].ack_state =
      ack_state;

    INC_MOD(grlc_ul_post_tx_blk_fifo.in_ind, POST_TX_SHOW_ITEM_MAX);

    /* Already checked for OVF.
    */
    ++grlc_ul_post_tx_blk_fifo.ctr;
  }

  GRLC_UL_TX_VARS_UNLOCK();

} /* grlc_ul_queue_blk_for_show */

/*===========================================================================
===
===  FUNCTION        GRLC_UL_DEQUEUE_BLK_FOR_SHOW
===
===  DESCRIPTION
===
===     This function takes an item out of the post-tx-fifo and adjust the
===     FIFO accordingly
===
===     Lock interrupt briefly to decrement a single counter
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
static void grlc_ul_dequeue_blk_for_show
(
  uint16          *bsn_ptr,
  tx_ack_state_t  *ack_state_ptr
)
{
  GRLC_UL_TX_VARS_LOCK();

  if (grlc_ul_post_tx_blk_fifo.ctr != 0)
  {
    *bsn_ptr = grlc_ul_post_tx_blk_fifo.show_fifo[grlc_ul_post_tx_blk_fifo.out_ind].bsn;
    *ack_state_ptr =
      grlc_ul_post_tx_blk_fifo.show_fifo[grlc_ul_post_tx_blk_fifo.out_ind].ack_state;

    --grlc_ul_post_tx_blk_fifo.ctr;
    INC_MOD(grlc_ul_post_tx_blk_fifo.out_ind, POST_TX_SHOW_ITEM_MAX);
  }
  else
  {
    /* Check underflow: show error message and clear buffer
    */
    MSG_GERAN_LOW_1_G("GRLU post msg buf(%u) UDF", POST_TX_SHOW_ITEM_MAX);
    grlc_ul_init_post_tx_show_fifo();
  }

  GRLC_UL_TX_VARS_UNLOCK();

} /* grlc_ul_dequeue_blk_for_show */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_SHOW_CONFIRM_TX
===
===  DESCRIPTION
===
===     Show the BSN and the type of data block that has been confirmed as
===     transmitted by PL1 (following NPL1 callback)
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
static void grlc_ul_show_confirm_tx
(
  uint16          bsn,
  tx_ack_state_t  ack_state
)
{
  uint32  u = 0L, u2=0L, u3=0L;

  /* check for valid RLC UL state and valid tx_arr pointer
  ** This is required as the memory for tx_arr is freed during release ,
  ** but a previous post_ptx_actions callback which is deferred to print
  ** block tx info may end up with Null pointer.
  */
  if (p_tx_arr  == NULL   ||
      ul.state  == US_NULL )
  {
    MSG_GERAN_LOW_1_G("GRLU RLC UL state already NULL %d",ul.state);
    return;
  }

  if (ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
  {
    /* ERLU post yyy BSN L=zz CvMcCpSi
    **                         | | | |
    **                        24 | | |
    **                          16 8 0
    **                             8 0
    */
    u = (((uint32)TX_ARR( bsn).blk.hdr.cv  & 0x0f) << 24 |
         ((uint32)TX_ARR( bsn).blk.cs      & 0x0f) << 16 |
         ((uint32)TX_ARR( bsn).blk.hdr.cps & 0x1f) <<  8 |
         ((uint32)TX_ARR( bsn).blk.hdr.si  & 0x01)
        );

    /* ERLU post 0BsnBsLn.     0Bsn: BSN in hex, Bs=BS_CV_MAX Ln=block length
    **            | | | |
    **           24 | | |
    **             16 8 0
    */
    u2 = ((((uint32)bsn                      & 0xffff) << 16) |
          (((uint32)BS_CV_MAX()        &    0xf) <<  8) |
          ((uint32)TX_ARR( bsn).blk.data_len &   0xff)
         );

    /* ERLU   0TbcABsn. 0Tbc: current ul.tbc, bp->abs_bsn
    **         | | | |
    **        24 | | |
    **          16 8 0
    */
    u3 = ((((uint32)ul.tbc              & 0xffff) << 16) |
          ((uint32)TX_ARR( bsn).blk.abs_bsn  & 0xffff)
         );

    switch(ack_state)
    {
      /* 'post' refers to post-transmission ack
      */
      case TXA_PTX:
      case TXA_PACK:
      case TXA_ACK:
      case TXA_UNACK:
      case TXA_PENDING_PACK:
      case TXA_NACK:
        MSG_GERAN_HIGH_4_G("ERLU post %d, 0BsnBsLn=%08x 0TbcABsn=%08x CvMcCpSi=%08x",ack_state,u2,u3,u);
        break;

      case TXA_INVALID:
      case TXA_INACTIVE:
        MSG_GERAN_LOW_1_G("ERLU post invalid ack_state %d", ack_state);
        break;

      default:
        break;
    }
  }
  else
  {
    /* GRLU post yyy BSN L=zz CvCsTsSi
    **                         | | | |
    **                        24 | | |
    **                          16   0
    */
    u = ((uint32)TX_ARR( bsn).blk.mac_hdr.type.ul_data.cv << 24) |
        ((uint32)ul.cur_res.cs                           << 16) |
        (((uint32)ul.cur_res.num_timeslots & 0xf)        <<  8) |
        (uint32)TX_ARR( bsn).blk.mac_hdr.type.ul_data.si;

    /* GRLU post 0BsnBsLn.     0Bsn: BSN in hex, Bs=BS_CV_MAX Ln=block length
    **            | | | |
    **           24 | | |
    **             16 8 0
    */
    u2 = ((((uint32)bsn                  & 0x00ff) << 16) |
          (((uint32)BS_CV_MAX()       &    0xf) <<  8) |
          ((uint32)TX_ARR( bsn).blk.data_len & 0xff)
         );

    /* GRLU   0TbcABsn. 0Tbc: current ul.tbc, bp->abs_bsn
    **         | | | |
    **        24 | | |
    **          16 8 0
    */
    u3 = ((((uint32)ul.tbc                  & 0xffff) << 16) |
          ((uint32)TX_ARR( bsn).blk.abs_bsn  & 0xffff)
         );

    switch(ack_state)
    {
      /* 'post' refers to post-transmission ack
      */
      case TXA_PTX:
      case TXA_PACK:
      case TXA_ACK:
      case TXA_UNACK:
      case TXA_PENDING_PACK:
      case TXA_NACK:
        MSG_GERAN_HIGH_4_G("GRLU post %d 0BsnBsLn=%08x 0TbcABsn=%08x CvCsTsSi=%08x",ack_state,u2,u3,u);
        break;

      case TXA_INVALID:
      case TXA_INACTIVE:
        MSG_GERAN_LOW_1_G ("GRLU post invalid ack_state %d",ack_state);
        break;

      default:
        break;
    }
  }
} /* grlc_ul_show_confirm_tx */

/*===========================================================================
===
===  FUNCTION        GRLC_UL_SHOW_POST_TX_BLKS
===
===  DESCRIPTION
===
===     This function shows all the data blocks queued up in the post-tx-blk-fifo
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_post_tx_blks (void)
{
  uint16         bsn = 0;
  tx_ack_state_t ack_state = TXA_INACTIVE;
  uint16         ctr = 0;   /* don't loop for ever if good people
                            ** violate our space
                            */

  while (grlc_ul_post_tx_blk_fifo.ctr != 0 && ctr < POST_TX_SHOW_ITEM_MAX)
  {
    grlc_ul_dequeue_blk_for_show ( &bsn, &ack_state);
    GRLC_UL_SHOW_CONFIRM_TX( bsn, ack_state);

    ctr++;
  }
}

#endif /* GSM_GPRS_GRLU_CONFIRM_TX */

#ifdef  GSM_GPRS_GRLU_SHOW_PAQ
/*===========================================================================
===
===  FUNCTION        GRLC_UL_SHOW_PAQ
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
/* Show contents of the Restore-FIFO
*/
void grlc_ul_show_paq (void)
{
  MSG_GERAN_MED_3_G ("GRLU show_paq NpOctets=%08x dup free=%u small free=%u",grlc_ul_pack_queue_info(),
            DSM_POOL_FREE_CNT(DSM_DUP_ITEM_POOL),
            DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL));
}
#endif

/*===========================================================================
===
===  FUNCTION      GRLC_UL_SHOW_NEW_ALLOC
===
===  DESCRIPTION
===     Show the attributes of the new uplink assignment
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_new_alloc
(
  const mr_ul_alloc_cnf_t *old_ptr,
  const mr_ul_alloc_cnf_t *ptr
)
{
  uint32  lu0=0L, lu1=0L, lu2=0L;

  /* Pack num_timeslots, coding scheme, TFI, TBF-type (open/close), access-phase (1/2),
  ** access-mode (dyn/fixed), tlli-block coding scheme and usf-granularity into two
  ** 32-bit hex
  **
  ** Ts Cs Tf Tp    Ph Mo Tc Gr
  ** ff ff ff ff    ff ff ff ff
  **  24             24
  **     16             16
  **        8              8
  */
  lu0  = ((uint32)old_ptr->num_timeslots & 0xf) << 24;
  lu0 |= ((uint32)old_ptr->cs & 0xf) << 16;
  lu0 |= ((uint32)old_ptr->ul_tfi & 0x1f) << 8;
  lu0 |= ((uint32)old_ptr->tbf_type & 0xf);

  lu1  = ((uint32)ptr->num_timeslots & 0xf) << 24;
  lu1 |= ((uint32)ptr->cs & 0xf) << 16;
  lu1 |= ((uint32)ptr->ul_tfi & 0x1f) << 8;
  lu1 |= ((uint32)ptr->tbf_type & 0xf);

  lu2  = ((uint32)ptr->access_phase & 0xff) << 24;
  lu2 |= ((uint32)ptr->access_mode & 0xf) << 16;
  lu2 |= ((uint32)ptr->tlli_block_cs & 0xf) << 8;
  lu2 |= ((uint32)ptr->usf_granularity & 0xf);

  if (ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
  {
    MSG_GERAN_MED_5_G("ERLU NewMCS=%u WS=%u, TsMcTfTp=%08x NewTsMcTfTp=%08x PhMoTcGr=%08x",
      ptr->mcs, ptr->ws, lu0,lu1,lu2);
  }
  else
  {
    MSG_GERAN_MED_4_G("GRLU NewCS=%u, TsCsTfTp=%08x NewTsCsTfTp=%08x PhMoTcGr=%08x",ptr->cs, lu0,lu1,lu2);
  }
  /* avoid compiler and lint warnings about unused variables when MSG
     is defined away to nothing */
  (void)lu0; (void)lu1; (void)lu2;

} /* grlc_ul_show_new_alloc */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_SHOW_ALLOC_CNF_AWAIT_ASS
===
===  DESCRIPTION
===     Show the attributes of the uplink assignment during AWAIT_ASS state
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_alloc_cnf_await_ass (  const mr_ul_alloc_cnf_t *ptr)
{
  uint32  lu1=0L, lu2=0L;

  /* Pack num_timeslots, coding scheme, TFI, TBF-type (open/close), access-phase (1/2),
  ** access-mode (dyn/fixed), tlli-block coding scheme and usf-granularity into two
  ** 32-bit hex
  **
  ** Ts Cs Tf Tp    Ph Mo Tc Gr
  ** ff ff ff ff    ff ff ff ff
  **  24             24
  **     16             16
  **        8              8
  */
  lu1  = ((uint32)ptr->num_timeslots & 0xf) << 24;
  if (ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
    lu1 |= ((uint32)ptr->mcs & 0xf) << 16;
  else
    lu1 |= ((uint32)ptr->cs & 0xf) << 16;
  lu1 |= ((uint32)ptr->ul_tfi & 0x1f) << 8;
  lu1 |= ((uint32)ptr->tbf_type & 0xf);

  lu2  = ((uint32)ptr->access_phase & 0xff) << 24;
  lu2 |= ((uint32)ptr->access_mode & 0xf) << 16;
  lu2 |= ((uint32)ptr->tlli_block_cs & 0xf) << 8;
  lu2 |= ((uint32)ptr->usf_granularity & 0xf);

  MSG_GERAN_MED_4_G ("ERLU AWAIT_ASS ALLOC_CNF TsMcTfTp=%08x PhMoTcGr=%08x tbf_mode %d, WS=%u",
    lu1, lu2, ul.cur_res.tbf_mode, ptr->ws);
  /* avoid compiler and lint warnings about unused variables when MSG
     is defined away to nothing */
  (void)lu1; (void)lu2;
} /* grlc_ul_show_alloc_cnf_await_ass */

#ifdef  GSM_GPRS_GRLU_LLC_PDU_TYPE
/*===========================================================================
===
===  FUNCTION      GRLC_UL_SHOW_LLC_PDU
===
===  DESCRIPTION
===
===     Show the characteristics of LLC PDU received
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_llc_pdu_type (  gprs_grr_ul_mes_t *ptr)
{
  uint32  lu = 0L;
  uint32  l2 = 0L;

  /* Mo (rlc mode), Pt (peak-throughput class), Rp (radio priority), GCa (cause)
  ** GCa is Gas_id plus Cause, b3b2 =  b1b0 = Cause
  ** Mo Pt Rp GCa    tlli     type len_PcPi
  **  |  |  | ||cause         |  | | |
  **  |  |  | 4gas_id         |  | | |
  **  |  |  8                 |  | | PFI value
  **  |  16                   |  | PFC active or not
  **  24                      |  length of LLC PDU in hex 0fff 0000
  **                          llc_pdu_type MSByte f000 0000
  */
  lu  = ((uint32)ptr->rlc_mode & 0xf) << 24;
  lu |= ((uint32)ptr->peak_throughput & 0xf) << 16;
  lu |= ((uint32)ptr->radio_priority & 0xf) << 8;
  lu |= (uint32)ptr->cause & 0xf;

  l2  = ((uint32)ptr->llc_pdu_type & 0xf) << 28;
  l2 |= ((uint32)ptr->len << 16) & (0x0fff0000);
  l2 |= ((uint32)(((IS_PFC_NW_SUPPORTED() ? 1 : 0) << 8) & (0x0000ff00)));
  l2 |= ((uint32)ptr->pfi & 0xff);

  MSG_GERAN_MED_3_G ("GRLU MoPtRpGC=%08x tlli=%08x Tylen_PcPi=%08x ",lu, ptr->tlli, l2);
  /* avoid compiler and lint warnings about unused variables when MSG
     is defined away to nothing */
  (void)lu;  (void)l2;

} /* grlc_ul_show_llc_pdu_type */

#endif /* GSM_GPRS_GRLU_LLC_PDU_TYPE */

#ifdef GSM_GPRS_GRLU_SHOW_TX_VARS
/*===========================================================================
===
===  FUNCTION      GRLC_UL_SHOW_TX_VARS
===
===  DESCRIPTION
===
===     Show the 6 Transmit Variables packed into three 32-bit hex numbers
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_tx_vars(void)
{
  /* Show tx vars in hex in 1 line or in decimal in 2 lines
  */
  uint32  u321=0L, u322=0L, u323=0L;

  /*    u321                 u322               u323
  **    ff  ff  ff  ff       ff  ff  ff  ff     ff  ff  ff  ff
  **    00  va  00  vs       00  vsf 00  seg    00  pvsf 0  nvsf
  */
  u321 = (uint32)((uint32)ul.va << 16)       | (uint32)ul.vs;
  u322 = (uint32)((uint32)ul.vsf << 16)      | (uint32)ul.seg_ind;
  u323 = (uint32)((uint32)ul.pack_vsf << 16) | (uint32)ul.nack_vsf;

  MSG_GERAN_MED_3_G ("VaVs %08x VfSg%08x PfNf %08x",u321,u322,u323);
  /* avoid compiler and lint warnings about unused variables when MSG
     is defined away to nothing */
  (void)u321; (void)u322; (void)u323;

} /* grlc_ul_show_tx_vars */

#endif /* GSM_GPRS_GRLU_SHOW_TX_VARS */


/*===========================================================================
===
===  FUNCTION      GRLC_UL_SHOW_FIFO_VARS
===
===  DESCRIPTION
===
===     Show Uplink FIFO variables
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_fifo_vars (void)
{
  MSG_GERAN_MED_9_G ("GRLU Ptx ctr=%d, in=%d, out=%d, read_ctr=%d, read_out=%d, validity=%d; Pak ctr=%d, in=%d, out=%d",
          grlc_ul_fifo.ptx_ctr,
          grlc_ul_fifo.ptx_in_ind,
          grlc_ul_fifo.ptx_out_ind,
          grlc_ul_fifo.ptx_read_ctr,
          grlc_ul_fifo.ptx_read_out_ind,
          grlc_ul_fifo.b_validity,
          grlc_ul_fifo.pack_ctr,
          grlc_ul_fifo.pack_in_ind,
          grlc_ul_fifo.pack_out_ind);

  MSG_GERAN_MED_8_G ("GRLU Pak read_ctr=%d, read_out=%d, dup free=%u; Nak ctr=%d, in=%d, out=%d, read_ctr=%d, read_out=%d",
          grlc_ul_fifo.pack_read_ctr,
          grlc_ul_fifo.pack_read_out_ind,
          DSM_POOL_FREE_CNT(DSM_DUP_ITEM_POOL),
          grlc_ul_fifo.nack_ctr,
          grlc_ul_fifo.nack_in_ind,
          grlc_ul_fifo.nack_out_ind,
          grlc_ul_fifo.nack_read_ctr,
          grlc_ul_fifo.nack_read_out_ind);
}

/*===========================================================================
===
===  FUNCTION      SHOW_RBB
===
===  DESCRIPTION
===
===     Format the RBB array into uint32 such that the bitmap can be displayed
===     as two 32-bit hex numbers
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void show_rbb
(
  const rm_acknack_desc_t *desc_ptr,
  uint8                   fai
)
{
  uint16 u;
  uint32 hu32=0L, lu32=0L, lu1=0L;

  for (u=0; u<4; u++)
  {
    /* RBB[4..7] into the lower 32 bits
    */
    lu32 |= (desc_ptr->rbb[u+4] << (8*((4-u)-1)) );

    /* RBB[0..3] into the higher 32 bits
    */
    hu32 |= (desc_ptr->rbb[u] << (8*((4-u)-1)) );
  }

  /* Pack SS(SSN), 00OOOO(ul.llc_octets) into a 32 bit hex
  ** ff ff ff ff
  ** SS Oc te ts
  **  | ||
  **  | |16
  **  | 20
  **  24
  */
  lu1 = (((uint32)desc_ptr->ssn & 0xff)   << 24 |
         ((uint32)ul.llc_octets & 0xffffff)
        );

  MSG_GERAN_MED_4_G ("GRLU FAI %d SnOctets=%08x 0x%08x %08x",fai, lu1,hu32,lu32);

  if (fai != RM_ACKNACK_FAI_TBF_COMPLETE)
  {
    GRLC_UL_SHOW_TX_VARS();
  }
  /* avoid compiler and lint warnings about unused variables when MSG
     is defined away to nothing */
  (void)lu1;
} /* show_rbb */


/*===========================================================================
===
===  FUNCTION      E_SHOW_RBB
===
===  DESCRIPTION
===
===     Format and display the EGPRS acknack desc in the most compact form
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void e_show_rbb (  const egprs_ack_nack_des_t *desc_ptr, uint8 fai)
{
  /* Three 32-bit words representing the fields:
  **
  ** FBEC SSSN                           CRBB_len(decimal)  URBB_len(decimal)
  ** |||| ||_|_Starting-sequence-number
  ** |||| |_Starting colour code
  ** ||||_Compressed_present
  ** |||_End-of-window
  ** ||_Beginning-of-window
  ** |_Final-ack-ind
  **
  ** If compressed bitmap is present
  **
  ** crbb
  */

  uint32 l1=0L;
  NOTUSED(fai);

  /* Format Fai, bow, eow, comp_present, scc and ssn fields
  */
  l1  = ((uint32)desc_ptr->fai << 28);
  l1 |= ((uint32)desc_ptr->bow << 24);
  l1 |= ((uint32)desc_ptr->eow << 20);
  l1 |= ((uint32)desc_ptr->comp_present << 16);

  l1 |= ((uint32)desc_ptr->scc << 12);
  l1 |= ((uint32)desc_ptr->ssn & 0x0fff);

  MSG_GERAN_MED_3_G ("ERLU FBECSSsn=%08x crbb_len=%u urbb_len=%u",
          l1,
          (desc_ptr->comp_present?desc_ptr->crbb_len:0),
          desc_ptr->urbb_len);

  if (desc_ptr->comp_present)
  {
    /* show crbb[4] of when compressed bitmap is present
    */
    MSG_GERAN_MED_4_G ("ERLU crbb[3]=%08x crbb[2]=%08x [1]%08x [0]%08x",
            desc_ptr->crbb[3],
            desc_ptr->crbb[2],
            desc_ptr->crbb[1],
            desc_ptr->crbb[0]);
  }

  if (desc_ptr->urbb_len)
  {
    /* Show the first 3 words (96 bits)
    */
    MSG_GERAN_MED_3_G ("ERLU urbb[2]=%08x [1]%08x [0]%08x",
            desc_ptr->urbb[2],
            desc_ptr->urbb[1],
            desc_ptr->urbb[0]);

    /* Show the next 3 words only if urbb_len allows it
    */
    if (desc_ptr->urbb_len > 32*3)
    {
      MSG_GERAN_MED_2_G ("ERLU urbb[4]=%08x [3]%08x",desc_ptr->urbb[4], desc_ptr->urbb[3]);
    }
  }
} /* e_show_rbb */

/*===========================================================================
===
===  FUNCTION      grlc_ul_show_pdus
===
===  DESCRIPTION
===
===     List each of the uplink PDUs in the pending-ack queue, the temporary
===     queue and the main queue. The PDUs are NOT removed from the queues
===
===  DEPENDENCIES
===     When this function is called a soft recovery is about to be executed so
===     no state preservation is required
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_pdus (void)
{
  gprs_grr_ul_mes_t*  pdu_ptr;

  MSG_GERAN_HIGH_1_G("Temp Queue = %u items", q_cnt(&ULTQ()));

  /* Read the top-most PDU - it is not removed from the queue
  */
  pdu_ptr = q_check( &ULTQ() );
  while ( pdu_ptr != NULL )
  {
    MSG_GERAN_LOW_3_G("psn=%u orig_len=%u ptr=0x%x",
        pdu_ptr->psn,
        pdu_ptr->orig_len,
        pdu_ptr->pdu_ptr);
    grlc_ul_show_llc_pdu_type( pdu_ptr);

    /* read the next PDU - again it is not removed from the queue
    */
    pdu_ptr = q_next( &ULTQ(), &pdu_ptr->link );
  }

  MSG_GERAN_HIGH_1_G("Main Queue = %u items", q_cnt(&ULQ()));

  /* Read the top-most PDU - it is not removed from the queue
  */
  pdu_ptr = q_check( &ULQ() );
  while ( pdu_ptr != NULL )
  {
    MSG_GERAN_LOW_3_G("psn=%u orig_len=%u ptr=0x%x",
        pdu_ptr->psn,
        pdu_ptr->orig_len,
        pdu_ptr->pdu_ptr);
    grlc_ul_show_llc_pdu_type( pdu_ptr);

    /* read the next PDU - again it is not removed from the queue
    */
    pdu_ptr = q_next( &ULQ(), &pdu_ptr->link );
  }

} /* grlc_ul_show_pdus */

/*===========================================================================
===
===  FUNCTION      grlc_ul_show_vars
===
===  DESCRIPTION
===
===     Show the main variables in RLC UL in verbose format
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_vars (void)
{
  uint16  al;       /* actual length */
  uint16  qcnt;

  if (cur_pdu_ptr != NULL)
  {
    al = (uint16)GPRS_PDU_LENGTH(cur_pdu_ptr->pdu_ptr);
  }
  else
  {
    al = 9999;
  }
  qcnt = (uint16)q_cnt(&ULQ());

#ifndef FEATURE_GERAN_REDUCED_DEBUG
  MSG_GERAN_MED_3_G ("GRLU Diag CntA=%d CntB=%d CntC=%d",
   ul_diag_stats_st.ul_gen_purpose_count_a,
   ul_diag_stats_st.ul_gen_purpose_count_b,
   ul_diag_stats_st.ul_gen_purpose_count_c);
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

  MSG_GERAN_MED_6_G ("GRLU Dump Mode seg=%d va=%d  vs=%d vsf=%d p_f=%d n_f=%d",
   ul.seg_ind,
   ul.va,
   ul.vs,
   ul.vsf,
   ul.pack_vsf,
   ul.nack_vsf);

  if (ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
    e_show_rbb( &ul.egprs_acknack_desc,
                ul.egprs_acknack_desc.fai);
  else
    show_rbb( &ul.ul_acknack,ul.ul_acknack.fai);

  MSG_GERAN_MED_9_G ("GRLU tbc=%u abs=%u si=%d grtd=%u; cur_blk #p=%u #octs=%lu bsn=%d ofs=%d a_bsn=%d",
    ul.tbc,
    ul.abs_bsn,
    ul.si,
    ul.blocks_granted,
    ul.llc_pdus,
    ul.llc_octets,
    cur_blk.bsn,
    cur_blk.offset,
    cur_blk.abs_bsn);

  MSG_GERAN_MED_6_G ("GRLU cur_blk data_len=%d rem_cnt=%d cv=%d b_access = %u 1st_psn_to_free %u fctrl=%u",
    cur_blk.data_len,
    cur_blk.rem_cnt,
    cur_blk.mac_hdr.type.ul_data.cv,
    ul.b_access,
    ul.first_psn_to_free,
    ul.flow_control_suspend);

  if (cur_pdu_ptr  != NULL &&
      next_pdu_ptr != NULL
     )
  {
    MSG_GERAN_MED_9_G ("GRLU curp=0x%x nxt=0x%x #q=%d p=%u last_p=%u pgstate=%d cur#blks=%u next#blks=%u next_s=%d",
      cur_pdu_ptr,
      next_pdu_ptr,
      qcnt,
      cur_pdu_ptr->psn,
      cur_pdu_ptr->pg_last_psn,
      cur_pdu_ptr->pg_state,
      cur_pdu_ptr->pg_num_blks,
      next_pdu_ptr->pg_num_blks,
      next_pdu_ptr->pg_state);

    MSG_GERAN_MED_9_G ("GRLU len=%u act_len=%u rem=%u ori_len=%u ptc=%d rprio=%d #blks=%u restore=%u #pdus=%u",
      cur_pdu_ptr->len,
      al,
      cur_pdu_ptr->rem_ind,
      cur_pdu_ptr->orig_len,
      cur_pdu_ptr->peak_throughput,
      cur_pdu_ptr->radio_priority,
      cur_pdu_ptr->pg_num_blks_cur_pdu,
      cur_pdu_ptr->restore_cnt,
      cur_pdu_ptr->pg_pdus);
  }

  grlc_ul_show_pdus();

  MSG_GERAN_MED_8_G ("GRLU PtxCtr=%d PtxRdCtr=%d; NakCtr=%d NakRdCtr=%d; PakCtr=%d PakRdCtr=%d; validity=%d dup free = %u",
    grlc_ul_fifo.ptx_ctr,
    grlc_ul_fifo.ptx_read_ctr,
    grlc_ul_fifo.nack_ctr,
    grlc_ul_fifo.nack_read_ctr,
    grlc_ul_fifo.pack_ctr,
    grlc_ul_fifo.pack_read_ctr,
    grlc_ul_fifo.b_validity,
    DSM_POOL_FREE_CNT(DSM_DUP_ITEM_POOL));

  if (grlc_ul_fifo.ptx_ctr != 0       ||
      grlc_ul_fifo.ptx_read_ctr != 0  ||
      grlc_ul_fifo.nack_ctr != 0      ||
      grlc_ul_fifo.nack_read_ctr != 0 ||
      grlc_ul_fifo.pack_ctr != 0      ||
      grlc_ul_fifo.pack_read_ctr
     )
  {
    MSG_GERAN_MED_0_G ("GRLU PL1_NO_GET");
  }

  MSG_GERAN_MED_6_G ("GRLU dup free = %u tot = %u, large free = %u tot = %u small free = %u tot = %u",
    DSM_POOL_FREE_CNT(DSM_DUP_ITEM_POOL),
    DSM_POOL_ITEM_CNT(DSM_DUP_ITEM_POOL),
    DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL),
    DSM_POOL_ITEM_CNT(DSM_DS_LARGE_ITEM_POOL),
    DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),
    DSM_POOL_ITEM_CNT(DSM_DS_SMALL_ITEM_POOL));
} /* grlc_ul_show_vars */

/*===========================================================================
===
===  FUNCTION      grlc_ul_show_dsm_info
===
===  DESCRIPTION
===
===     Show DSM Memory info
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_show_dsm_info (void)
{
  uint32  u1 = 0L, u2=0L, u3=0L;

  u1 = (DSM_POOL_ITEM_CNT(DSM_DS_SMALL_ITEM_POOL) << 16) | DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL);
  u2 = (DSM_POOL_ITEM_CNT(DSM_DUP_ITEM_POOL)      << 16) | DSM_POOL_FREE_CNT(DSM_DUP_ITEM_POOL);

  #if (DSM_MAJOR_VER_NO == 2)
    u3 = (dsmi_pool_mgmt_table[DSM_DS_SMALL_ITEM_POOL].stats_min_free_count <<16 ) |
        dsmi_pool_mgmt_table[DSM_DUP_ITEM_POOL].stats_min_free_count;
  #else
    u3 = (dsm_ds_small_item_pool.stats_min_free_count) << 16 | dsm_dup_item_pool.stats_min_free_count;
  #endif

  MSG_GERAN_LOW_3_G("SmCntFree=%08x ,DupCntFree=%08x , SmMinDupMin=%08x",u1,u2,u3);
  /* avoid compiler and lint warnings about unused variables when MSG
     is defined away to nothing */
  (void)u1; (void)u2; (void)u3;

}/* grlc_ul_show_dsm_info*/


#if  defined (DEBUG_GSM_GPRS_DS_UNIT_TEST) || defined (PERLUTF)
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST/PERLUTF */

#if  defined (DEBUG_GSM_GPRS_RLC_TEST) || defined (PERLUTF)
#error code not present
#endif /* DEBUG_GSM_GPRS_RLC_TEST/PERLUTF */

/*===========================================================================
===
===  FUNCTION      SHOW_GROUP_STATUS()
===
===  DESCRIPTION
===
===    Show PDU group status info
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void show_group_status (void)
{
#ifdef  GSM_GPRS_GRLU_SHOW_PDU_GROUPING_STATUS
  if (cur_pdu_ptr != NULL)
  {
    MSG_GM3 ("GRLU pg psn=%u state=%d last_p%u",
            cur_pdu_ptr->psn,
            cur_pdu_ptr->pg_state,
            cur_pdu_ptr->pg_last_psn);
  }
#endif
}

#ifndef FEATURE_GERAN_REDUCED_DEBUG
/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_GET_UL_STATISTICS()
===
===  DESCRIPTION
===
===    RLC function to fill and return the Uplink Statistics Info
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    pointer to data structure
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
const grlc_diag_ul_statistics_t* grlc_diag_get_ul_statistics (void)
{
  static grlc_diag_ul_statistics_t  temp_st; /* static_dual_spaced_ignore */

  temp_st = ul_diag_stats_st;

  return &temp_st;
}

/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_CLR_UL_STATISTICS()
===
===  DESCRIPTION
===
===    RLC function to clear Uplink Statistics
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void grlc_diag_clr_ul_statistics (void)
{
  (void)memset(&ul_diag_stats_st, 0, sizeof(ul_diag_stats_st));
}


/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_GET_UL_ABNORMAL_RELEASE_COUNTS()
===
===  DESCRIPTION
===
===    RLC function to fill and return Abnornal Release Counts
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    pointer to data structure
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
const grlc_diag_ul_abnormal_release_counts_t* grlc_diag_get_ul_abnormal_release_counts
(
)
{
  static grlc_diag_ul_abnormal_release_counts_t   temp_st; /* static_dual_spaced_ignore */

  temp_st = ul_diag_abn_rel_cnts_st;

  return &temp_st;
}

/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_CLR_UL_ABNORMAL_RELEASE_COUNTS()
===
===  DESCRIPTION
===
===    RLC function to clear Uplink Abnormal Release Counts
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void grlc_diag_clr_ul_abnormal_release_counts (void)
{
  (void)memset(&ul_diag_abn_rel_cnts_st, 0, sizeof(ul_diag_abn_rel_cnts_st));
}

/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_GET_UL_EVENT_COUNTS()
===
===  DESCRIPTION
===
===    RLC function to fill and return Uplink Event Counts
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    pointer to data structure
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
const grlc_diag_ul_event_counts_t* grlc_diag_get_ul_event_counts (void)
{
  static grlc_diag_ul_event_counts_t  temp_st; /* static_dual_spaced_ignore */

  temp_st = ul_diag_evt_cnts_st;

  return &temp_st;
}

/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_CLR_UL_EVENT_COUNTS()
===
===  DESCRIPTION
===
===    RLC function to clear Uplink Event Counts
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void grlc_diag_clr_ul_event_counts (void)
{
  (void)memset(&ul_diag_evt_cnts_st, 0, sizeof(ul_diag_evt_cnts_st));
}
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */


/*** EOF: don't remove! ***/
