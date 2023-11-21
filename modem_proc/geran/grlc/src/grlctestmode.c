/*****************************************************************************
***
*** TITLE
***
***  GPRS Radio Link Control Receive Module
***
*** DESCRIPTION
***
***  This module contains all the receive processing functions
***
***
*** EXTERNALIZED FUNCTIONS
***
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
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
*** $Header: //components/rel/geran.mpss/5.2.0/grlc/src/grlctestmode.c#3 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 20/08/13    hv     CR531564
*** 11/09/06    hv     Created
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "comdef.h"
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "geran_msgs.h"
#include "gl1rlccmn.h"

#ifdef DEBUG_GSM_EGPRS_DS_UNIT_TEST
#error code not present
#endif /* DEBUG_GSM_EGPRS_DS_UNIT_TEST */

#include "grlcdl.h"
#include "rex.h"
#include "gprs_mem.h"
#include "gmacrlc.h"
#include "grlcmac.h"
#include "grlcx.h"
#include "grlcutil.h"
#include "grlctimer.h"
#include "grlcdlfifo.h"
#include "gbitutil.h"
#include "grlcdiag.h"
#include "grlclog.h"
#include "grlcdli.h"
#include "grlcl1.h"
#include "grlctsk.h"
#include "grlcsgm.h" /* definition of ul cur_dlk needed for TEST_MODE */
#include "grlcdltst.h"
#include "grlcasm.h"
#include "gmutex.h"
#include "geran_eng_mode_info.h"
#include "geran_tasks.h"
#include "geran_multi_sim.h"

/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION     PROCESS_TEST_MODE_B_DL_ACK_BLOCK()
===
===  DESCRIPTION
===
===    If BSN is within the valid rx window then attach the block to the
===    rx array and update V(R), V(N) and SSN. If the block is outside the
===    rx window then mark it invalid and free the DSM memory.
===
===    In the second stage the BSN is checked and if it co-incides with
===    V(Q) then V(Q) is updated . Note that in Test Mode B, no blocks
===    are assembled to the current LLC PDU.
===
===    input: global dl (dl_ass, vr, vq, bsn), dl.bsn
===    output: dl (vr, vq)
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    All vars involved are module variables
===
===========================================================================*/
static void process_test_mode_b_dl_ack_block (void)
{
  uint8     b, ind;     /* loop count */
  uint8     temp_vq;     /* temporary value of V(Q) */

  /*----------------------------------------------------------*/

  /* Data block in cur_dl_blk has just taken out of the FIFO. Check the BSN
  ** of the data block and if it is within the rx window then attach the
  ** block to the rx array and update VN.
  **
  ** Also update the V(R) in the process. BSN is valid if:
  ** [V(R) <= BSN' < V(Q)+WS] mod SNS is TRUE. However, due to the way we
  ** implement our modulo comparison function, the expression is modifed to:
  ** [V(R) <= BSN' <= (V(Q)+WS-1)] mod SNS.
  */
  if (IS_VQ_LE_BSN_LT_VQ_PLUS_WS(dl.vq, dl.bsn, WS))
  {
    /* put block on dl queue
    */
    RX_ARR( dl.bsn) = cur_dl_blk;

    /* In Test Mode B there is no need to keep a reference coppy of the
    ** payload pointer in the RX array
    */
    RX_ARR( dl.bsn).dsm_payload_ptr = NULL;

    /* Update the VN array
    */
    RX_ARR( dl.bsn).vn = RXS_RECEIVED;

    /* Update V(R)
    */
    if (IS_VR_LE_BSN_LT_VQ_PLUS_WS(dl.vr, dl.bsn, dl.vq, RM_RLC_WINDOW_SIZE))
    {
      /* Clear the blocks between the old VR (inc VR) and BSN-1 but only if VR != BSN
      */
      if (dl.vr != dl.bsn)
      {
        clear_receive_status( dl.vr, dl.bsn);
      }

      dl.vr = (uint8)MOD_SNS(dl.bsn + 1);
      UPD_DL_VR();

      /* Update SSN as well
      */
      dl.ssn = dl.vr;
      UPD_DL_SSN();
    }
  }
  else
  /* Data block is not in the receive window. Now check to see the BSN of
  ** is outside the VN window range which is relative to V(R). If, so
  ** mark that VN element RXS_INVALID
  **
  ** This operation is somewhat redundant but is implemented in case the
  ** meaning is overlooked.
  */
  {
    if (IS_VR_LE_BSN_LE_VR_MINUS_K(dl.vr, dl.bsn, RM_RLC_WINDOW_SIZE))
    {
      RX_ARR( dl.bsn).vn = RXS_INVALID;
    }
    MSG_GERAN_LOW_3_G("GRLD TEST_MODE_B Rcved out of Win bsn=%d, vq=%d, vr=%d", dl.bsn, dl.vq, dl.vr);
  }

  /*----------------------------------------------------------------------*/

  /* Update V(Q). Only if BSN hits the start of the queue (vq).
  ** If V(Q) moves then assemble the block into the current PDU
  */
  if (dl.vq == dl.bsn)
  {
    /* Set V(Q) to the value of the BSN within the window that
    ** has not yet been properly received and which minimises
    ** the expression [BSN - V(R)] mod SNS.
    **
    ** The expression above means the NEW value of V(Q) will be
    ** nearest to the old value of V(Q). When searching from
    ** V(Q)+1 towards V(R) the first BSN with a status not being
    ** RXS_RECEIVED item shall be assigned to V(Q).
    ** If there is no item whose V(N) value is RXS_RECEIVED then
    ** assign V(R) to V(Q).
    **
    ** The case where V(Q) = V(R) before the search starts will
    ** not happen because when V(R) = V(Q) = 0 and BSN = 0,
    ** V(R) gets incremented and is 1 higher than V(Q).
    */

    /* Advance V(Q). Now loop from the new value of V(Q) to the new value
    ** of V(R) to seek a contiguous block with a RXS_RECEIVED state.
    ** If found the
    */
    RX_ARR( dl.vq).vn = RXS_RECEIVED_ASSEMBLED;
    dl.vq = (uint8)MOD_SNS(dl.vq+1);
    UPD_DL_VQ();

    /* Use a copy of V(Q) since V(Q) gets altered in the loop
    */
    temp_vq = (uint8)dl.vq;

    for (b=0; b<MOD_DIF(dl.vr, temp_vq, SNS); b++)
    {
      ind = (uint8)MOD_SNS(b + temp_vq);
      if (RX_ARR( ind).vn == RXS_RECEIVED)
      {
        /* found another contiguous block with RXS_RECEIVED status.
        ** So, assemble it and advance V(Q)
        */
        dl.vq = (uint8)MOD_SNS(ind + 1);
        UPD_DL_VQ();

        RX_ARR( ind).vn = RXS_RECEIVED_ASSEMBLED;

      }
      else
      {
        /* break out of loop and do not alter V(Q) when the first RXS_INVALID
        ** block is met
        */
        break;
      }
    } /* end search V(Q) loop */
  }
} /* process_test_mode_b_dl_ack_block */

/*===========================================================================
===
===  FUNCTION     E_PROCESS_TEST_MODE_DL_ACK_BLOCK()
===
===  DESCRIPTION
===
===    If BSN is within the valid rx window then attach the block to the
===    rx array and update V(R), V(N) and SSN. If the block is outside the
===    rx window then mark it invalid and free the DSM memory.
===
===    In the second stage the BSN is checked and if it co-incides with
===    V(Q) then V(Q) is updated . Note that in Test Mode B, no blocks
===    are assembled to the current LLC PDU.
===
===    input: global dl (dl_ass, vr, vq, bsn), dl.bsn
===    output: dl (vr, vq)
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    All vars involved are module variables
===
===========================================================================*/
static void e_process_test_mode_dl_ack_block (void)
{

  uint16     b, ind;     /* loop count */
  uint16     temp_vq;     /* temporary value of V(Q) */

  /*----------------------------------------------------------*/

  /* Data block in cur_dl_blk has just taken out of the FIFO. Check the BSN
  ** of the data block and if it is within the rx window then attach the
  ** block to the rx array and update VN.
  **
  ** Also update the V(R) in the process. BSN is valid if:
  ** [V(R) <= BSN' < V(Q)+WS] mod SNS is TRUE. However, due to the way we
  ** implement our modulo comparison function, the expression is modifed to:
  ** [V(R) <= BSN' <= (V(Q)+WS-1)] mod SNS.
  */
  if (EGPRS_IS_VQ_LE_BSN_LT_VQ_PLUS_WS(dl.vq, dl.bsn, dl.ws) )
  {
    /* Is it a split data block */
    if( IS_BLOCK_SPLIT() )
    {
      /* Check if both splits have been rcved */
      if( ( (cur_dl_blk.spb        == SPB_RETX_PART_1) &&
            (E_RX_ARR( dl.bsn).spb  == SPB_RETX_PART_2)
          ) ||
          ( (cur_dl_blk.spb        == SPB_RETX_PART_2) &&
            (E_RX_ARR( dl.bsn).spb  == SPB_RETX_PART_1)
          )
        )
      {
        /* Both parts of the split block have been rcved.
        */
        MSG_GERAN_LOW_1_G("ERLD TEST_MODE Rcved both splits for bsn=%d",dl.bsn);
      }
      else if( E_RX_ARR( dl.bsn).dsm_payload_ptr == NULL )
      {
        /* One part of the split block is rcved. Put it on dl queue
        */
        E_RX_ARR( dl.bsn) = cur_dl_blk;

        E_RX_ARR( dl.bsn).vn = RXS_INVALID;

        /* In Test Mode there is no need to keep a reference coppy of the
        ** payload pointer in the RX array
        */
        E_RX_ARR( dl.bsn).dsm_payload_ptr = NULL;

        /* Do not process the dl block. Only one part has been recved
        */
        return;
      }
      else
      {
        MSG_GERAN_LOW_3_G("ERLD TEST_MODE Rcved duplicate split bsn=%d, rcved_spb=%d, spb=%d",
          dl.bsn, cur_dl_blk.spb, E_RX_ARR( dl.bsn).spb );

        /* Do not process the dl block. Only one part has been recved
        */
        return;
      }
    }

    /* put block on dl queue
    */
    E_RX_ARR( dl.bsn) = cur_dl_blk;

    /* In Test Mode there is no need to keep a reference coppy of the
    ** payload pointer in the RX array
    */
    E_RX_ARR( dl.bsn).dsm_payload_ptr = NULL;

    /* Update the VN array
    */
    E_RX_ARR( dl.bsn).vn = RXS_RECEIVED;

    /* Update V(R)
    */
    if (EGPRS_IS_VR_LE_BSN_LT_VQ_PLUS_WS(dl.vr, dl.bsn, dl.vq, dl.ws))
    {
      /* Clear the blocks between the old VR (inc VR) and BSN-1 but only if VR != BSN
      */
      if (dl.vr != dl.bsn)
      {
        clear_receive_status( dl.vr, dl.bsn);
      }

      dl.vr = MOD_ESNS(dl.bsn + 1);

#ifdef FEATURE_GSM_EGPRS_IR_WS_REDUCTION
      /* as we want ms_out mem to be zero always */
      dl.vr_max = dl.vr;
#endif /* FEATURE_GSM_EGPRS_IR_WS_REDUCTION */

      UPD_DL_VR();

      /* Update SSN as well
      */
      dl.ssn = dl.vr;
      UPD_DL_SSN();
    }
    else
    {
      MSG_GERAN_LOW_3_G("ERLD TEST_MODE did not update V(R)=%d, dl.bsn=%d, dl.vq=%d",
        dl.vr, dl.bsn, dl.vq );
    }
  }
  else
  /* Data block is not in the receive window, mark VN element RXS_INVALID
  */
  {
    E_RX_ARR( dl.bsn).vn = RXS_INVALID;

    MSG_GERAN_LOW_3_G("ERLD TEST_MODE Rcved out of win bsn=%d vq=%d vr=%d", dl.bsn, dl.vq, dl.vr);
  }

  /*----------------------------------------------------------------------*/

  /* Update V(Q). Only if BSN hits the start of the queue (vq).
  ** If V(Q) moves then assemble the block into the current PDU
  */
  if (dl.vq == dl.bsn)
  {
    /* Set V(Q) to the value of the BSN within the window that
    ** has not yet been properly received and which minimises
    ** the expression [BSN - V(R)] mod SNS.
    **
    ** The expression above means the NEW value of V(Q) will be
    ** nearest to the old value of V(Q). When searching from
    ** V(Q)+1 towards V(R) the first BSN with a status not being
    ** RXS_RECEIVED item shall be assigned to V(Q).
    ** If there is no item whose V(N) value is RXS_RECEIVED then
    ** assign V(R) to V(Q).
    **
    ** Note in Test Mode no blocks are assembled into PDU(s)
    **
    ** The case where V(Q) = V(R) before the search starts will
    ** not happen because when V(R) = V(Q) = 0 and BSN = 0,
    ** V(R) gets incremented and is 1 higher than V(Q).
    */

    /* Advance V(Q). Now loop from the new value of V(Q) to the new value
    ** of V(R) to seek a contiguous block with a RXS_RECEIVED state.
    ** If found the
    */
    E_RX_ARR( dl.vq).vn = RXS_INVALID;
    dl.vq = MOD_ESNS(dl.vq+1);
    UPD_DL_VQ();

    /* Use a copy of V(Q) since V(Q) gets altered in the loop
    */
    temp_vq = dl.vq;

    for (b=0; b < MOD_DIF_ESNS(dl.vr, temp_vq); b++)
    {
      ind = MOD_ESNS(b + temp_vq);
      if (E_RX_ARR( ind).vn == RXS_RECEIVED)
      {
        /* found another contiguous block with RXS_RECEIVED status.
        ** So, clear block state and advance V(Q)
        */
        E_RX_ARR( dl.vq).vn = RXS_INVALID;
        dl.vq = MOD_ESNS(ind + 1);
        UPD_DL_VQ();
      }
      else
      {
        /* break out of loop and do not alter V(Q) when the first RXS_INVALID
        ** block is met
        */
        break;
      }
    } /* end search V(Q) loop */

#ifdef FEATURE_GSM_EGPRS_IR_WS_REDUCTION
    /* Update V(R)_MAX only when Vr_max falls out of window
    ** We want to retain vr_max only for crc failure blocks
    ** so as to report ms_out_of_mem only when for fail crc
    ** blocks and good crc blocks.
    */
    if (! EGPRS_IS_VQ_LE_BSN_LT_VQ_PLUS_WS(dl.vq, dl.vr_max, dl.ws) )
    {
      dl.vr_max = dl.vq;
      MSG_GERAN_LOW_2_G("ERLD vr_max has gone out of window,update vr_max %d vq %d",
               dl.vr_max,dl.vq);
    }
#endif /* FEATURE_GSM_EGPRS_IR_WS_REDUCTION */
  }
} /* e_process_test_mode_dl_ack_block */

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      DS_RX_ACK_TEST_MODE_B_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in GPRS Test Mode state in ACK mode.
===   loop_blk_cnt now is count given by L1 for any DL / UL config, requesting RLC
===   to generate more than one block on same TS.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
void ds_rx_ack_test_mode_b_state_pl1_ph_data_ind_event_handler(void)
{
  /* pointer to buffer for holding messages to send to MAC
  */
  rlc_mac_msg_t         *rlc_msg_ptr;
  uint8                 loop_blk_cnt=0;

  /* if there is data block available
  */
  while (grlc_dl_fifo_get_cnt() > 0)
  {
    /* if there is a message there must be data blocks in
    ** the fifo. Point b_ptr to data block.
    */
    if (grlc_dl_fifo_get_block( &pl1_blk_hdr_ptr, &pl1_blk_dsm_ptr) == DL_FIFO_EMPTY)
    {
      MSG_GERAN_LOW_0_G("GRLD un-exp DL empty");
    }
    else
    {
      /* translate the over-the-air raw data into cur_dl_blk
      */
      test_mode_xlate_ext_dl_data( pl1_blk_hdr_ptr, pl1_blk_dsm_ptr, &cur_dl_blk);
      loop_blk_cnt = pl1_blk_hdr_ptr->loop_dl_blk;

      /* Save the coding scheme, frame number, time_slot, S/P, RRBP and
      ** final block indicator from the data block just translated to the
      ** dl struct
      */
      dl.bsn = cur_dl_blk.bsn;

      MSG_GERAN_LOW_1_G("GRLD Ack DS_TEST_MODE B Rcvd PH_DATA_IND bsn=%d ",dl.bsn);

      /* Update coding scheme for engg_mode
      */
      if (dl.cs != cur_dl_blk.cs)
      {
        geran_eng_mode_data_write(ENG_MODE_GPRS_DL_CS, &cur_dl_blk.cs, TRUE);
      }

      /* copy PL1 header info
      */
      dl.cs = cur_dl_blk.cs;
      dl.sp = cur_mac_hdr.type.dl_data.sp;
      dl.rrbp = cur_mac_hdr.type.dl_data.rrbp;
      dl.fbi  = cur_dl_blk.fbi;

      /* latch the FBI value if the current FBI (belonging to a block)
      ** is set and the latch value if it is NOT set
      */
      if (dl.fbi == RM_FBI_FINAL)
      {
        dl.fbi_latch = RM_FBI_FINAL;
      }

      /* Put data block on dl queue and update state vars if the block
      ** has not been received before otherwise ignore the data carried
      ** by the block
      */
      if ( (RX_ARR( dl.bsn).vn == RXS_INVALID           ) ||
           (RX_ARR( dl.bsn).vn == RXS_RECEIVED_ASSEMBLED)
         )
      {

        /* In Test Mode B the first DL BSN can be of any value not necessarily 0.
        ** Set the RX elements from "0" to "first received DL BSN" to RXS_RECEIVED
        ** to avoid NACKing any blocks at the start of the test mode.
        */
        if( dl.b_first_block_ind_sent == FALSE )
        {
          MSG_GERAN_LOW_3_G( "GRLD ACK TEST_MODE B 1st DL blk bsn=%d vq=%d vr=%d",dl.bsn,dl.vq,dl.vr);
          if( (dl.bsn != 0 ) && (dl.bsn < SNS) )
          {
            uint8  b;
            for (b=0; b < dl.bsn; b++)
            {
              RX_ARR( b).vn = RXS_RECEIVED;
            }
            dl.vq = dl.bsn;
            dl.vr = dl.bsn;
          }
        }

        /* If the data block in cur_dl_blk is within the receive window,
        ** then attach it to the receive array. If not then ignore it but
        ** free the DSM memory. Update VR, VN and SSN. Note in Test Mode B
        ** no re-assemble takes place.
        */
        process_test_mode_b_dl_ack_block();
      }
      else
      {
         MSG_GERAN_LOW_2_G( "GRLD ACK TEST_MODE rcved bsn=%d with vn=%d",
            dl.bsn,RX_ARR( dl.bsn).vn);
      }

      /*-----------------------------------------------------------------
      ** If this is the 1st block received then send indication to MAC
      ** and set the flag. DO this before consider sending paknak to MAC
      **-----------------------------------------------------------------
      */
      if (dl.b_first_block_ind_sent == FALSE)
      {
        dl.b_first_block_ind_sent = TRUE;

        /* Indicate to MAC so it can enter DL transfer mode
        */
        rlc_msg_ptr = mac_get_cmd_buf_rlc();
        if (rlc_msg_ptr != NULL)
        {
          rlc_msg_ptr->msg.dl_first_block_ind.sig = RM_DL_FIRST_BLOCK_IND;
          mac_put_cmd_rlc( rlc_msg_ptr);
        }
        else
        {
          ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
        }
      }

      /*-------------------------------------------------------------
      ** now check for FBI, polling, VR=VQ and do:
      ** Now check for these conditions:
      ** 1.polled (RRBP valid): send Pkt Dl Acknack msg
      ** 2.polled and FBI set and VQ = VR (all blocks received):
      **   assemble PDU(s) and send to LLC, send Pkt Dl Acknack with
      **   FAI set. Enter RX_ACK_PENDING_RELEASE.
      ** 3.polled and FBI set and VQ != VR: send Pkt Dl Acknack with
      **   FAI clear.
      **-------------------------------------------------------------
      */

      if ( grlc_dl_schedule_pkt_ack_nack_transmission() )
      {
        grlc_dl_request_pkt_ack_nack_transmission();
      }

      /* Check if DL blk needs to be looped back on the UL
      ** Generate number of blks based on cnt send by L1 for that timeslot
      */
      if( loop_blk_cnt  )
      {
        while(loop_blk_cnt)
        {
          /*==================================================
          ** Convert DL data block to an UL data block and
          ** send it to UL FIFO.
          **==================================================
          */

          /* Fill UL MAC Header */
          cur_blk.mac_hdr.type.ul_data.cv    = 15; /* No countdown in Test Mode B */
          ul.cv                              = 15;
          cur_blk.mac_hdr.type.ul_data.si    = 0;  /* N/A */
          cur_blk.mac_hdr.type.ul_data.retry = ul.cur_res.retry;
          cur_blk.mac_hdr.payload            = MAC_PAYLOAD_DATA;

          /* Fill the rest of the header fields */
          cur_blk.tfi   = ul.cur_res.ul_tfi;
          cur_blk.ti    = 0;
          cur_blk.bsn   = (uint8)ul.bsn;
          INC_MOD(ul.bsn, SNS);
          cur_blk.ext   = cur_dl_blk.ext;

          /* Set the CS for the UL block */
          cur_blk.cs = dl.cs;

          /* Make sure that there are no extension octets */
          cur_blk.num_ext_octets    = 0;

          /* Copy the dl payload to the ul payload. Use the dup pointer since it
          ** is used in xlate_int_ul_data ( )
          */
          cur_blk.dup_dsm_ptr = gprs_pdu_copy( cur_dl_blk.dsm_payload_ptr );

          /* Calculate the length of the payload */
          cur_blk.data_len = (uint8)GPRS_PDU_LENGTH( cur_blk.dup_dsm_ptr );

          /* Set remaining octet counter to zero to avoid adding filling octets */
          cur_blk.rem_cnt  = 0;

          cur_blk.pdu_prio = GPDU_LOW_PRIORITY;

          /* Put the block into the FIFO */
          if( UL_FIFO_PUT_OK != grlc_ul_fifo_put_block( &cur_blk, TXA_PTX) )
          {
            MSG_GERAN_LOW_0_G("GRLD DS_TEST_MODE error in fifo put block");
          }
          else
          {

            MSG_GERAN_LOW_3_G("RLD DS_TEST_MODE B ACK sent bsn=%d tn=%d ctr=%d",
                          ul.bsn ,pl1_blk_hdr_ptr->time_slot, grlc_ul_fifo.ptx_ctr);

            /*=====================================================================
            ** Inform PL1 to start polling the FIFO for data blocks by sending the
            ** PH_DATA_REQ primitive on the very first data block of a TBF
            **=====================================================================
            */
            if (ul.b_first_fifo_load)
            {
              pl1_send_ph_data_req();
              grlc_ul_fifo_enable_pl1_access();

              ul.b_first_fifo_load = FALSE;
            }
          }

          --loop_blk_cnt;
        }

        /* Free DL payload */
        GPRS_PDU_FREE( &cur_dl_blk.dsm_payload_ptr);

        if( cur_dl_blk.dsm_payload_ptr != NULL )
          MSG_GERAN_LOW_0_G("GRLD DS_TEST_MODE failed to free payload");

      } /* DL blk needs to be looped back on the UL */
      else
      {
        /* No need to loop the DL block back on the UL. Free DSM items */
        if( cur_dl_blk.dsm_payload_ptr )
          GPRS_PDU_FREE( &cur_dl_blk.dsm_payload_ptr);

        if( cur_dl_blk.dsm_payload_ptr != NULL )
          MSG_GERAN_LOW_0_G( "GRLD Ack DS_TEST_MODE B failed to free payload");
      }
    } /* end get valid DL block */
  } /* end while */
} /* ds_rx_ack_test_mode_b_state_pl1_ph_data_ind_event_handler */

/*===========================================================================
===
===  FUNCTION      E_DS_RX_ACK_TEST_MODE_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in DS_RX_ACK state in EGPRS TEST Mode B and SRB modes.
===   loop_blk_cnt now is count given by L1 for any DL / UL config, requesting RLC
===   to generate more than one block on same TS.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
void e_ds_rx_ack_test_mode_state_pl1_ph_data_ind_event_handler(void)
{
  /* pointer to buffer for holding messages to send to MAC
  */
  rlc_mac_msg_t           *rlc_msg_ptr;

  /* Holds the EPDAN REQ message from PL1 */
  pl1_rlc_msg_t           *epdan_req_ptr;

  /* EGPRS DL data stractures to hold dual payload during DL queue processing */
  grlc_dl_data_block_t  data_block1;
  grlc_dl_data_block_t  data_block2;
  grlc_dl_data_block_t  *tmp_data_block_ptr;

  /* Hold the UL data block for Test Mode B loop back */
  grlc_ul_data_block_t    ul_data_block1;
  grlc_ul_data_block_t    ul_data_block2;

  /* Flag to indicate that the EPDAN has been send to MAC */
  boolean                 epdan_req_not_serviced = FALSE;
  uint8                   loop_blk_cnt=0;

  /* loop counter for dual payload */
  uint8                   max_block_no;

  /* if there is data block available
  */
  while (grlc_dl_fifo_get_cnt() > 0)
  {
    if (e_grlc_dl_fifo_get_block( &e_pl1_dl_blk) == DL_FIFO_EMPTY)
    {
      MSG_GERAN_LOW_0_G("ERLD TEST_MODE PH_DATA_IND with DL Q empty ");
    }
    else
    {
      /* Initialize temp payload storage */
      data_block1.dsm_payload_ptr = NULL;
      data_block2.dsm_payload_ptr = NULL;

      loop_blk_cnt = e_pl1_dl_blk.loop_dl_blk;

      /* re-start when receiving a new data block in SRB test mode
      */
      if( dl.test_mode == TEST_MODE_SRB )
      {
        TIMER_START_T3190();
      }

      /* Validate L1 DL block(s)
      */
      if( (e_pl1_dl_blk.crc_1 == 1) && (e_pl1_dl_blk.payload_1_ptr == NULL) )
      {
        /* 1st payload CRC passed but payload pointer is NULL.
        */
        MSG_GERAN_MED_1_G("ERLD TEST_MODE Rcved 1st payload with CRC=1 & dsm_ptr= NULL [%d]",
                    e_pl1_dl_blk.frame_number);

      }

      if( (e_pl1_dl_blk.crc_2 == 1) && (e_pl1_dl_blk.payload_2_ptr == NULL) )
      {
        /* 2nd payload payload CRC passed but payload pointer is NULL.
        */
        MSG_GERAN_HIGH_1_G("ERLD TEST_MODE Rcved 2nd payload with CRC=1 & dsm_ptr=NULL [%d]",
                    e_pl1_dl_blk.frame_number);

      }

      if( (e_pl1_dl_blk.mcs <= RM_MCS_6) && (e_pl1_dl_blk.payload_2_ptr!= NULL) )
      {
        /* Single payload MCS with valid 2nd payload pointer! Release 2nd payload */
        GPRS_PDU_FREE( &e_pl1_dl_blk.payload_2_ptr );
        e_pl1_dl_blk.payload_2_ptr = NULL;

        MSG_GERAN_LOW_2_G("ERLD TEST_MODE Rcved MCS=%d with valid 2nd payload [%d]",
                    e_pl1_dl_blk.mcs, e_pl1_dl_blk.frame_number);
      }

      MSG_GERAN_MED_3_G("ERLD ACK TEST_MODE Rcved bsn1=%d and bsn2=%d",
                  E_GET_RAW_DL_BSN1(&(e_pl1_dl_blk.hdr_arr[0])),
                  (dl.cs > RM_MCS_6) ?
                  (MOD_ESNS(E_GET_RAW_DL_BSN1(&(e_pl1_dl_blk.hdr_arr[0])) +
                           E_GET_RAW_DL_BSN2(&(e_pl1_dl_blk.hdr_arr[0])))) :0,
                  0
                );

      /* translate the over-the-air raw data into internal format grlc_dl_data_block_t
      */
      e_test_mode_xlate_ext_dl_data( &e_pl1_dl_blk, &data_block1, &data_block2);

      /* If the first block sent was polled but CRC failed we still need to send an
      ** ACK/NACK to the N/W. MAC needs RM_DL_FIRST_BLOCK_IND to transition to TRANSFER
      ** state prior to handling ACK/NACK request.
      */
      if (dl.b_first_block_ind_sent == FALSE )
      {
        dl.b_first_block_ind_sent = TRUE;

        /* Indicate to MAC so it can enter DL transfer mode
        */
        rlc_msg_ptr = mac_get_cmd_buf_rlc();
        if (rlc_msg_ptr != NULL)
        {
          rlc_msg_ptr->msg.dl_first_block_ind.sig = RM_DL_FIRST_BLOCK_IND;
          mac_put_cmd_rlc( rlc_msg_ptr);
        }
        else
        {
          ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
        }
      }

#ifdef  ERLD_SHOW_BLOCK
      show_dl_block( &e_pl1_dl_blk, &data_block1, &data_block2);
#endif

      /* Set the loop counter according to MCS */
      if (dl.cs > RM_MCS_6 && dl.cs <= RM_MCS_9)
        max_block_no = 2;
      else
        max_block_no = 1;

      /* Start processing the first payload */
      tmp_data_block_ptr = &data_block1;

      while( max_block_no-- )
      {
        /* Store DL block to current RLC DL block */
        cur_dl_blk =  *tmp_data_block_ptr;

        /* In Test Mode there is no need to keep a reference coppy of the
        ** payload pointer in the cur_dl_blk or RX array.
        */
        cur_dl_blk.dsm_payload_ptr = NULL;

        /* Save the coding scheme, frame number, time_slot, ES/P, RRBP and
        ** final block indicator from the data block just translated to the
        ** dl struct
        */
        dl.bsn = cur_dl_blk.bsn;

        /* Save the TFI for generating EPDAN later on
        */
        dl.tfi = e_cur_hdr.tfi;

        /* Update coding scheme for engg_mode
        */
        if (dl.cs != cur_dl_blk.cs)
        {
          geran_eng_mode_data_write(ENG_MODE_EGPRS_DL_MCS, &cur_dl_blk.cs, TRUE);
        }

        /* copy PL1 header info
        */
        dl.cs = cur_dl_blk.cs;

        dl.sp   = e_cur_hdr.esp;
        dl.rrbp = e_cur_hdr.rrbp;

        if (dl.cs > RM_MCS_6 && dl.cs <= RM_MCS_9)
        {
          dl.fbi = data_block1.fbi | data_block2.fbi;
        }
        else
        {
          dl.fbi = data_block1.fbi;
        }

#if defined(GRLD_SHOW_BLOCK) || defined(GRLD_SHOW_BSN)
        show_bsn_and_fifo_vars();
#endif
        /* latch the FBI value if the current FBI (belonging to a block)
        ** is set and the latch value if it is NOT set
        */
        if (dl.fbi == RM_FBI_FINAL)
        {
          dl.fbi_latch = RM_FBI_FINAL;
        }

        /* Put data block on dl queue and update state vars if the block
        ** has not been received before otherwise ignore the data carried
        ** by the block
        */
        if ( (E_RX_ARR( dl.bsn).vn == RXS_INVALID           ) ||
             (E_RX_ARR( dl.bsn).vn == RXS_RECEIVED_ASSEMBLED)    )
        {
          /* If the data block in cur_dl_blk is within the receive window,
          ** then attach it to the receive array. Update VR, VN and SSN.
          ** If the BSN of data block co-incides with V(Q), V(Q) is moved
          ** along until a contiguous data block is not found.
          */
          if( cur_dl_blk.crc == 1)
            e_process_test_mode_dl_ack_block();
          else
            MSG_GERAN_HIGH_1_G( "ERLD ACK TEST_MODE rcved bsn=%d failed crc",dl.bsn);
        }
        else
        {
          MSG_GERAN_LOW_2_G( "ERLD ACK TEST_MODE rcved bsn=%d with vn=%d",
            dl.bsn,E_RX_ARR( dl.bsn).vn);
        }

        /*-------------------------------------------------------------
        ** now check for FBI, polling, VR=VQ and do:
        ** Now check for these conditions:
        ** 1.polled (RRBP valid): send Pkt Dl Acknack msg
        ** 2.polled and FBI set and VQ = VR (all blocks received):
        **   assemble PDU(s) and send to LLC, send Pkt Dl Acknack with
        **   FAI set. Enter RX_ACK_PENDING_RELEASE.
        ** 3.polled and FBI set and VQ != VR: send Pkt Dl Acknack with
        **   FAI clear.
        **-------------------------------------------------------------
        */

        /* Check if the EPDAN_REQ signal mask is set
        */
        if( grlc_dl_task_sigs & GRLC_DL_PL1_EPDAN_REQ_SIG )
        {
          /* Without removing the EPDAN_REQ signal from the queue, check if the
          ** current block is the the block that EPDAN was requested by PL1.
          */
          epdan_req_ptr = (pl1_rlc_msg_t *)q_check(&DPQ());

          if( epdan_req_ptr == NULL )
          {
            MSG_GERAN_LOW_0_G( "ERLD EPDAN_REQ mask without msg");
          }
          else
          {
            /* Flag to check if an EPDAN was sent to MAC after the request was detected
            */
            epdan_req_not_serviced = TRUE;

            /* Reply to the EPDAN only if
            ** a. Single payload in the DL block
            ** b. Dual payload and the 2nd payload has been processed
            ** For both cases make sure that the BSN, which the EPDAN has
            ** been requested for, has been procesed and BSN < V(R), or
            ** the bsn is outside the rx window.
            */
            if( max_block_no == 0 )
            {
              /* The current block is the block that EPDAN was requested by PL1. Read
              ** EPDAN signal from the queue and clear signal mask if queue empty.
              */
              epdan_req_ptr = (pl1_rlc_msg_t *)q_get(&DPQ());

              GRLC_DL_MSG_LOCK();

              if (q_cnt(&DPQ()) == 0)
              {
                RLC_DL_CLR_PL1_EPDAN_REQ_SIG();
              }

              GRLC_DL_MSG_UNLOCK();

              if( epdan_req_ptr != NULL )
              {
                /* Check the Final Block and if all DL blocks sent by the peer
                ** have been received and format the EPDAN.
                */

                /* get buffer for msg to send to MAC
                */
                rlc_msg_ptr = mac_get_cmd_buf_rlc();
                if (rlc_msg_ptr != NULL)
                {
                  /* vq=vr & fbi means all data blocks have been received
                  */
                  if (dl.fbi_latch == RM_FBI_FINAL && dl.vr == dl.vq)
                  {

                    UPD_DL_STATE();

                    /* set FAI of packet dl acknack
                    ** ! formulate acknack msg
                    */
                    rlc_msg_ptr->msg.dl_pkt_acknack_req.e_acknack_desc.fai =
                              RM_ACKNACK_FAI_TBF_COMPLETE;

                  } /* end tbf complete */
                  else
                  {
                    /* all data blocks have not been received.
                    ** Set FAI = 0 in packet dl acknack
                    */
                    rlc_msg_ptr->msg.dl_pkt_acknack_req.e_acknack_desc.fai =
                              RM_ACKNACK_FAI_TBF_NOT_COMPLETE;

                    /*MSG_LOW("GRLD RX_A sp vq=%d vr=%d ",dl.vq,dl.vr,0);*/
                  } /* end tbf NOT complete */

                  /*---------------------------------------------------------------
                  ** Formulate ssn & rbb array vars. Set signal and send packet dl
                  ** acknack RRBP and frame number and TFI
                  **---------------------------------------------------------------
                  */

                  /* Copy the Quality Report from PL1. If the Quality Report is dropped
                  ** while the EPDAN description is formatted (ESP 1/2), the following
                  ** pointer is set to NULL, in e_format_dl_acknack_desc(), to indicate
                  ** to MAC that the QReport should not be included in the EPDAN.
                  */
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.pl1_qrep_ptr =
                    epdan_req_ptr->qrep_ptr;
#ifdef PERLUTF
                 #error code not present
#endif
                  e_format_dl_acknack_desc( &rlc_msg_ptr->msg.dl_pkt_acknack_req,
                                            epdan_req_ptr);
                  UPD_DL_ACKNACK();
                  /* Fill the rest of the message */
                  rlc_msg_ptr->msg.sig = RM_DL_PKT_ACKNACK_REQ;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.esp          = epdan_req_ptr->esp;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.frame_number = epdan_req_ptr->
                                                                          frame_no;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.include_chan_req = epdan_req_ptr->
                                                                          include_chan_req;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.rrbp         = epdan_req_ptr->rrbp;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.tfi          = dl.tfi;
                  rlc_msg_ptr->msg.dl_pkt_acknack_req.time_slot    = epdan_req_ptr->
                                                                            timeslot;

#ifdef  ERLD_SHOW_EPDAN
                  show_epdan ( &rlc_msg_ptr->msg.dl_pkt_acknack_req);
#endif
                  /* Log DL EPDAN
                  ** Log Code: 0x5211
                  */
                  rlc_log_epdan( &rlc_msg_ptr->msg.dl_pkt_acknack_req);

                  /* Send EPDAN to MAC */
                  mac_put_cmd_rlc( rlc_msg_ptr);

                  epdan_req_not_serviced = FALSE;

                  /* Free epdan_req_ptr */
#if defined (DEBUG_GSM_EGPRS_DS_UNIT_TEST) || defined (PERLUTF)
                  #error code not present
#else
                  gprs_rlc_l1isr_free_buffer( epdan_req_ptr);
#endif /* DEBUG_GSM_EGPRS_DS_UNIT_TEST / PERLUTF */
                }
                else
                {
                  ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
                }
              } /* end if( epdan_req_ptr != NULL ) */
              else
              {
                MSG_GERAN_LOW_0_G( "ERLD EPDAN_REQ without sig in DPQueue");
              }
            } /*  if( !(IS_GT_MOD( epdan_req_ptr->bsn, dl.vr, dl.ws)) ) */
          } /*  end if( epdan_req_ptr == NULL ) */
        }  /*  end if( grlc_dl_task_sigs & GRLC_DL_PL1_EPDAN_REQ_SIG ) */

        /* Now process the 2nd payload if present */
        if (dl.cs > RM_MCS_6 && dl.cs <= RM_MCS_9)
          tmp_data_block_ptr = &data_block2;

      } /* max_block_no-- ) */

      if( dl.test_mode == TEST_MODE_B )
      {
        /* Check if DL blk needs to be looped back on the UL
        ** Generate number of blks based on cnt send by L1 for that timeslot
        */
        if ( loop_blk_cnt )
        {
          while(loop_blk_cnt)
          {
            /*==================================================
            ** Convert DL data block(s) to an UL data block(s) and
            ** send it to UL FIFO.
            **==================================================
            */
            ul.cv                    = 15;

            /* Fill UL Header */
            ul_data_block1.hdr.cv    = 15; /* No countdown in Test Mode B */
            ul_data_block1.hdr.retry = ul.cur_res.retry;
            ul_data_block1.hdr.si    = 0;  /* N/A */
            ul_data_block1.hdr.tfi   = ul.cur_res.ul_tfi;

            ul_data_block1.hdr.bsn1   = data_block1.bsn;
            ul_data_block1.bsn        = data_block1.bsn;
            ul_data_block1.hdr.cps    = e_cur_hdr.cps;
            ul_data_block1.hdr.rsb    = RM_RSB_1ST_TX;

            /* Fill the rest of the fields */
            ul_data_block1.hdr.pi        = 0; /* N/A */
            ul_data_block1.ti            = 0; /* N/A */


            /* ul_data_block1.bsn = ul.bsn; */
            /* INC_MOD(ul.bsn, SNS);        */
            ul_data_block1.ext   = data_block1.ext;

            /* Set the MCS and PS for the UL block */
            ul_data_block1.cs        = data_block1.cs;
            ul_data_block1.init_mcs  = data_block1.cs;
            ul_data_block1.ps        = 0;

            /* Make sure that there are no extension octets */
            ul_data_block1.num_ext_octets    = 0;

            /* Set remaining octet counter to zero to avoid adding filling octets */
            ul_data_block1.rem_cnt  = 0;

            ul_data_block1.pdu_prio = GPDU_LOW_PRIORITY;

            /* Copy the dl payload to the ul payload. Use the dup pointer since it
            ** is used in xlate_int_ul_data ( )
            */
            ul_data_block1.dup_dsm_ptr = gprs_pdu_copy( data_block1.dsm_payload_ptr );

            /* Calculate the length of the payload */
            ul_data_block1.data_len = (uint8)GPRS_PDU_LENGTH( ul_data_block1.dup_dsm_ptr );

            if (dl.cs > RM_MCS_6 && dl.cs <= RM_MCS_9)
            {
              /* Deal with the 2nd payload
              */
              ul_data_block1.hdr.bsn2 = MOD_DIF_ESNS( data_block2.bsn, data_block1.bsn);
              ul_data_block2.hdr      = ul_data_block1.hdr;
              ul_data_block2.bsn      = data_block2.bsn;
              ul_data_block2.hdr.bsn1 = data_block2.bsn;
              ul_data_block2.hdr.bsn2 = 0; /* N/A */

              /* Fill the rest of the fields */
              ul_data_block2.hdr.pi        = 0; /* N/A */
              ul_data_block2.ti       = 0; /* N/A */

              ul_data_block2.ext   = data_block2.ext;

              /* Set the MCS and PS for the UL block */
              ul_data_block2.cs        = data_block2.cs;
              ul_data_block2.init_mcs  = data_block2.cs;
              ul_data_block2.ps        = 0;

              /* Make sure that there are no extension octets */
              ul_data_block2.num_ext_octets    = 0;

              /* Set remaining octet counter to zero to avoid adding filling octets */
              ul_data_block2.rem_cnt  = 0;

              ul_data_block2.pdu_prio = GPDU_LOW_PRIORITY;

              /* Copy the dl payload to the ul payload. Use the dup pointer since it
              ** is used in xlate_int_ul_data ( )
              */
              ul_data_block2.dup_dsm_ptr = gprs_pdu_copy( data_block2.dsm_payload_ptr );

              /* Calculate the length of the payload */
              ul_data_block2.data_len =
                  (uint8)GPRS_PDU_LENGTH( ul_data_block2.dup_dsm_ptr );

            }

            /* Put the block into the FIFO */
            if( UL_FIFO_PUT_OK !=
                e_grlc_ul_fifo_put_block( &ul_data_block1,
                                         ((dl.cs > RM_MCS_6) ? &ul_data_block2 : NULL),
                                         TXA_PTX
                                         )
              )
            {
              MSG_GERAN_LOW_0_G("ERLD DS_TEST_MODE error in fifo put block ");
            }
            else
            {
              MSG_GERAN_LOW_3_G("ERLD DS_TEST_MODE B ACK sent bsn1=%d bsn2=%d ctr=%d",
                             ul_data_block1.hdr.bsn1 ,
                             (dl.cs > RM_MCS_6) ?
                             MOD_ESNS(ul_data_block1.hdr.bsn1 + ul_data_block1.hdr.bsn2):
                             0,
                             grlc_ul_fifo.ptx_ctr);

              /*=====================================================================
              ** Inform PL1 to start polling the FIFO for data blocks by sending the
              ** PH_DATA_REQ primitive on the very first data block of a TBF
              **=====================================================================
              */
              if (ul.b_first_fifo_load)
              {
                pl1_send_ph_data_req();
                grlc_ul_fifo_enable_pl1_access();

                ul.b_first_fifo_load = FALSE;
              }
            }
            --loop_blk_cnt;
          }

          /* Free DL payload */
          GPRS_PDU_FREE( &data_block1.dsm_payload_ptr);

          if( data_block1.dsm_payload_ptr != NULL )
            MSG_GERAN_LOW_0_G("ERLD DS_TEST_MODE ACK failed to free payload");

          if (dl.cs > RM_MCS_6 && dl.cs <= RM_MCS_9)
          {
            /* Free DL payload */
            GPRS_PDU_FREE( &data_block2.dsm_payload_ptr);

            if( data_block2.dsm_payload_ptr != NULL )
              MSG_GERAN_LOW_0_G("ERLD DS_TEST_MODE ACK failed to free payload");
          }
        }
        else
        {
          /* No need to loop the DL block back on the UL. Free DSM items */
          if( data_block1.dsm_payload_ptr )
          {
            GPRS_PDU_FREE( &data_block1.dsm_payload_ptr);
          }
          if( data_block2.dsm_payload_ptr )
          {
            GPRS_PDU_FREE( &data_block2.dsm_payload_ptr);
          }
        }
      } /* end get TEST MODE B */
      else
      {
        /* In EGPRS SRB test mode, just discard the DL data block(s) & free DSM items */
        if( data_block1.dsm_payload_ptr )
        {
          GPRS_PDU_FREE( &data_block1.dsm_payload_ptr);
        }
        if( data_block2.dsm_payload_ptr )
        {
          GPRS_PDU_FREE( &data_block2.dsm_payload_ptr);
        }
      } /* end TEST_MODE_SRB */
    } /* end get valid DL block */
  } /* end while */

  if( epdan_req_not_serviced == TRUE )
  {
    MSG_GERAN_LOW_0_G("ERLD Did not respond to EPDAN_REQ ");
  }
} /* e_ds_rx_ack_test_mode_state_pl1_ph_data_ind_event_handler */

/*===========================================================================
===
===  FUNCTION      DS_RX_UNACK_TEST_MODE_B_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in TEST_MODE_B state in GPRS mode.
===   loop_blk_cnt now is count given by L1 for any DL / UL config, requesting RLC
===   to generate more than one block on same TS.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
void ds_rx_unack_test_mode_b_state_pl1_ph_data_ind_event_handler(void)
{

  /* pointer to buffer for holding messages to send to MAC
  */
  rlc_mac_msg_t           *rlc_msg_ptr;
  uint8                    loop_blk_cnt=0;

  int8                    u;

  /* make sure there are data blocks in the fifo
  ** before setting the L1 block processing flag
  */
  while (grlc_dl_fifo_get_cnt() > 0)
  {
    /* count is not 0. There's at least 1 block hence ignore return value
    */
    if (grlc_dl_fifo_get_block( &pl1_blk_hdr_ptr, &pl1_blk_dsm_ptr) == DL_FIFO_EMPTY)
    {
      MSG_GERAN_LOW_0_G("GRLD DS_TEST_MODE un-exp DL fifo empty");
    }
    else
    {
      if( dl.test_mode == TEST_MODE_B )
      {
        /* translate the over-the-air raw data into cur_dl_blk
        */
        test_mode_xlate_ext_dl_data( pl1_blk_hdr_ptr, pl1_blk_dsm_ptr, &cur_dl_blk);
        loop_blk_cnt = pl1_blk_hdr_ptr->loop_dl_blk;

        dl.bsn = cur_dl_blk.bsn;
        MSG_GERAN_LOW_1_G("GRLD UnAck DS_TEST_MODE B Rcvd PH_DATA_IND bsn=%d ",dl.bsn);

        /* Update coding scheme for engg_mode
        */
        if (dl.cs != cur_dl_blk.cs)
        {
          geran_eng_mode_data_write(ENG_MODE_GPRS_DL_CS, &cur_dl_blk.cs, TRUE);
        }

        /* Save block's parameters to the dl global storage
        */
        dl.cs = cur_dl_blk.cs;
        dl.sp = cur_mac_hdr.type.dl_data.sp;
        dl.rrbp = cur_mac_hdr.type.dl_data.rrbp;
        dl.fbi = cur_dl_blk.fbi;

        /* latch the FBI value if the current FBI (belonging to a block)
        ** is set and the latch value if it is NOT set
        */
        if (dl.fbi == RM_FBI_FINAL)
        {
          dl.fbi_latch = RM_FBI_FINAL;
        }

        /*-----------------------------------------------------------------
        ** If this is the 1st block received then send indication to MAC
        ** and set the flag. Do this before consider sending paknak to MAC
        **-----------------------------------------------------------------
        */
        if (dl.b_first_block_ind_sent == FALSE)
        {
          dl.b_first_block_ind_sent = TRUE;

          /* Indicate to MAC so it can enter DL transfer mode
          */
          rlc_msg_ptr = mac_get_cmd_buf_rlc();
          if (rlc_msg_ptr != NULL)
          {
            rlc_msg_ptr->msg.dl_first_block_ind.sig = RM_DL_FIRST_BLOCK_IND;
            mac_put_cmd_rlc( rlc_msg_ptr);
          }
          else
          {
            ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
          }
        }

        /*------------------------------------------------------------------
        ** Send PCA and enter PENDING_RELEASE if FBI is received. Otherwise
        ** send PDAN if polled
        **------------------------------------------------------------------
        */
        if (dl.fbi == RM_FBI_FINAL)
        {
          /* Stop T3190 when sending Pkt Ctrl Acknack
          */
          TIMER_STOP_T3190();

          /*----------------------------------------------------
          ** Request MAC to send Packet Control Acknowledgement
          **----------------------------------------------------
          */
          rlc_msg_ptr = mac_get_cmd_buf_rlc();
          if (rlc_msg_ptr != NULL)
          {
            rlc_msg_ptr->msg.sig = RM_DL_PKT_CTRL_ACK_REQ;

            /* rrbp from the current dl data block where polling bit is detected
            */
            rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.rrbp = dl.rrbp;

            /* frame number carried over from PL1 for each data block
            */
            rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.frame_number = dl.frame_number;

            /* ..and time slot
            */
            rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.time_slot = dl.time_slot;

            /* ..and TFI
            */
            rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.tfi = ul.cur_res.ul_tfi;

            MSG_GERAN_LOW_0_G("GRLD RX_U FBI Sent PCAck");
            mac_put_cmd_rlc( rlc_msg_ptr);

            /* Next state is unack pending release
            */
            dl.state = DS_RX_UNACK_PENDING_RELEASE;

            dl.test_mode = TEST_MODE_OFF;

            UPD_DL_STATE();
          }
          else
          {
            ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
          }
        } /* end FB bit set */
        else
        /*------------------------------------------------------------
        ** Send PDAN if Polling bit is set. Yes even in un-ack mode !
        **------------------------------------------------------------
        */
        if (dl.sp == RM_SP_RRBP_VALID)
        {
          rlc_msg_ptr = mac_get_cmd_buf_rlc();
          if (rlc_msg_ptr != NULL)
          {
            rlc_msg_ptr->msg.sig = RM_DL_PKT_ACKNACK_REQ;

            /*---------------------------------------------------------------
            ** Set SSN to the last received BSN. Set RBB to all 0xff
            **---------------------------------------------------------------
            */
            rlc_msg_ptr->msg.dl_pkt_acknack_req.acknack_desc.ssn = (uint8)dl.bsn;

            /* Fill the bitmap with all 1's
            */
            for (u=0; u<RM_RBB_ARRAY_LEN; u++)
              rlc_msg_ptr->msg.dl_pkt_acknack_req.acknack_desc.rbb[u] = 0xff;
            UPD_DL_ACKNACK();

            /* set FA to 0 in un-ack mode
            */
            rlc_msg_ptr->msg.dl_pkt_acknack_req.acknack_desc.fai =
                      RM_ACKNACK_FAI_TBF_NOT_COMPLETE;

            show_dl_rbb( &rlc_msg_ptr->msg.dl_pkt_acknack_req.acknack_desc,
              rlc_msg_ptr->msg.dl_pkt_acknack_req.acknack_desc.fai);

            /* rrbp from the current dl data block where the polling bit
            ** is detected
            */
            rlc_msg_ptr->msg.dl_pkt_acknack_req.rrbp = dl.rrbp;

            /* frame number carried beside data block from PL1 for each data block
            */
            rlc_msg_ptr->msg.dl_pkt_acknack_req.frame_number = dl.frame_number;

            /* copy time slot to MAC as well
            */
            rlc_msg_ptr->msg.dl_pkt_acknack_req.time_slot = dl.time_slot;

            /* ..and TFI
            */
            rlc_msg_ptr->msg.dl_pkt_acknack_req.tfi = dl.tfi;
            mac_put_cmd_rlc( rlc_msg_ptr);
          }
          else
          {
            ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
          }
        } /* end SP bit set */

        /* Check if DL blk needs to be looped back on the UL
        ** Generate number of blks based on cnt send by L1 for that timeslot
        */
        if( loop_blk_cnt  )
        {
          while(loop_blk_cnt)
          {
            /*==================================================
            ** Convert DL data block to an UL data block and
            ** send it to UL FIFO.
            **==================================================
            */

            /* Fill UL MAC Header */
            cur_blk.mac_hdr.type.ul_data.cv    = 15; /* No countdown in Test Mode B */
            ul.cv                              = 15;
            cur_blk.mac_hdr.type.ul_data.si    = 0;  /* N/A */
            cur_blk.mac_hdr.type.ul_data.retry = ul.cur_res.retry;
            cur_blk.mac_hdr.payload            = MAC_PAYLOAD_DATA;

            /* Fill the rest of the header fields */
            cur_blk.tfi   = ul.cur_res.ul_tfi;
            cur_blk.ti    = 0;
            cur_blk.bsn   = (uint8)ul.bsn;
            INC_MOD(ul.bsn, SNS);
            cur_blk.ext   = cur_dl_blk.ext;

            /* Set the CS for the UL block */
            cur_blk.cs = dl.cs;

            /* Make sure that there are no extension octets */
            cur_blk.num_ext_octets    = 0;

            /* Copy the dl payload to the ul payload. Use the dup pointer since it
            ** is used in xlate_int_ul_data ( )
            */
            cur_blk.dup_dsm_ptr = gprs_pdu_copy( cur_dl_blk.dsm_payload_ptr );

            /* Calculate the length of the payload */
            cur_blk.data_len = (uint8)GPRS_PDU_LENGTH( cur_blk.dup_dsm_ptr );

            /* Set remaining octet counter to zero to avoid adding filling octets */
            cur_blk.rem_cnt  = 0;

            cur_blk.pdu_prio = GPDU_LOW_PRIORITY;

                /* Put the block into the FIFO */
            if( UL_FIFO_PUT_OK != grlc_ul_fifo_put_block( &cur_blk, TXA_PTX) )
            {
              MSG_GERAN_LOW_0_G("GRLD DS_TEST_MODE error in fifo put block ");
            }
            else
            {
              MSG_GERAN_LOW_3_G("RLD DS_TEST_MODE B UNACK sent bsn=%d tn=%d ctr=%d",
                               ul.bsn ,pl1_blk_hdr_ptr->time_slot, grlc_ul_fifo.ptx_ctr);

              /*=====================================================================
              ** Inform PL1 to start polling the FIFO for data blocks by sending the
              ** PH_DATA_REQ primitive on the very first data block of a TBF
              **=====================================================================
              */
              if (ul.b_first_fifo_load)
              {
                pl1_send_ph_data_req();
                grlc_ul_fifo_enable_pl1_access();

                ul.b_first_fifo_load = FALSE;
              }
            }
            -- loop_blk_cnt;
          }

          /* Free DL payload */
          GPRS_PDU_FREE( &cur_dl_blk.dsm_payload_ptr);

          if( cur_dl_blk.dsm_payload_ptr != NULL )
            MSG_GERAN_LOW_0_G( "GRLD DS_TEST_MODE failed to free payload");

        }/* DL blk needs to be looped back on the UL */
        else
        {
          /* No need to loop the DL block back on the UL. Free DSM items */
          if( cur_dl_blk.dsm_payload_ptr )
            GPRS_PDU_FREE( &cur_dl_blk.dsm_payload_ptr);

          if( cur_dl_blk.dsm_payload_ptr != NULL )
            MSG_GERAN_LOW_0_G( "GRLD Ack DS_TEST_MODE B failed to free payload");
        }
      }
      else
      {
        MSG_GERAN_LOW_1_G("GRLD DS_TEST_MODE invalid mode %d",dl.test_mode);
      }
    }
  }/* end reading DL fifo loop */
}/* ds_rx_unack_test_mode_b_state_pl1_ph_data_ind_event_handler() */

/*===========================================================================
===
===  FUNCTION      E_DS_RX_UNACK_TEST_MODE_STATE_PL1_PH_DATA_IND_EVENT_HANDLER()
===
===  DESCRIPTION
===
===   Handler of PL1_PH_DATA_IND event in TEST_MODE_B and SRB state in EGPRS mode.
===   loop_blk_cnt now is count given by L1 for any DL / UL config, requesting RLC
===   to generate more than one block on same TS.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   None
===
===  SIDE EFFECTS
===
===========================================================================*/
void e_ds_rx_unack_test_mode_state_pl1_ph_data_ind_event_handler(void)
{
    /* pointer to buffer for holding messages to send to MAC
  */
  rlc_mac_msg_t           *rlc_msg_ptr;

  /* Holds the EPDAN REQ message from PL1 */
  pl1_rlc_msg_t           *epdan_req_ptr;

  /* EGPRS DL data stractures to hold dual payload during DL queue processing */
  grlc_dl_data_block_t  data_block1;
  grlc_dl_data_block_t  data_block2;

  grlc_ul_data_block_t    ul_data_block1;
  grlc_ul_data_block_t    ul_data_block2;
  uint8                   loop_blk_cnt=0;


  /* if there is data block available
  */
  while (grlc_dl_fifo_get_cnt() > 0)
  {
    if (e_grlc_dl_fifo_get_block( &e_pl1_dl_blk) == DL_FIFO_EMPTY)
    {
      MSG_GERAN_LOW_0_G("ERLD TEST_MODE PH_DATA_IND with DL Q empty ");
    }
    else
    {
      /* Initialize temp payload storage */
      data_block1.dsm_payload_ptr = NULL;
      data_block2.dsm_payload_ptr = NULL;

      loop_blk_cnt = e_pl1_dl_blk.loop_dl_blk;

      /* re-start when receiving a new data block in SRB test mode
      */
      if( dl.test_mode == TEST_MODE_SRB )
      {
        TIMER_START_T3190();
      }

      /* Validate L1 DL block(s)
      */
      if( (e_pl1_dl_blk.crc_1 == 1) && (e_pl1_dl_blk.payload_1_ptr == NULL) )
      {
        /* 1st payload CRC passed but payload pointer is NULL.
        */
        MSG_GERAN_LOW_1_G("ERLD TEST_MODE Rcved 1st payload with CRC=1 & dsm_ptr= NULL [%d]",
                    e_pl1_dl_blk.frame_number);

      }
      else if( (e_pl1_dl_blk.crc_1 == 0) && (e_pl1_dl_blk.payload_1_ptr == NULL) )
      {
        MSG_GERAN_LOW_1_G("ERLD TEST_MODE Rcved 1st payload with CRC=0 & dsm_ptr= NULL [%d]",
                  e_pl1_dl_blk.frame_number);

      }

      if( (e_pl1_dl_blk.crc_2 == 1) && (e_pl1_dl_blk.payload_2_ptr == NULL) )
      {
        /* 2nd payload payload CRC passed but payload pointer is NULL.
        */
        MSG_GERAN_LOW_1_G("ERLD REST_MODE Rcved 2nd payload with CRC=1 & dsm_ptr=NULL [%d]",
                    e_pl1_dl_blk.frame_number);

      }
      else if( (e_pl1_dl_blk.mcs > RM_MCS_6) &&
               (e_pl1_dl_blk.crc_2 == 0) &&
               (e_pl1_dl_blk.payload_2_ptr == NULL)
             )
      {
        MSG_GERAN_LOW_1_G("ERLD TEST_MODE Rcved 2nd payload with CRC=0 & dsm_ptr= NULL [%d]",
                  e_pl1_dl_blk.frame_number);

      }

      if( (e_pl1_dl_blk.mcs <= RM_MCS_6) && (e_pl1_dl_blk.payload_2_ptr!= NULL) )
      {
        /* Single payload MCS with valid 2nd payload pointer! Release 2nd payload */
        GPRS_PDU_FREE( &e_pl1_dl_blk.payload_2_ptr );
        e_pl1_dl_blk.payload_2_ptr = NULL;

        MSG_GERAN_LOW_2_G("ERLD TEST_MODE Rcved MCS=%d with valid 2nd payload [%d]",
                    e_pl1_dl_blk.mcs, e_pl1_dl_blk.frame_number);
      }

      /* translate the over-the-air raw data into internal format grlc_dl_data_block_t
      */
      e_test_mode_xlate_ext_dl_data( &e_pl1_dl_blk, &data_block1, &data_block2);


      /* If the first block sent was polled but CRC failed we still need to send an
      ** ACK/NACK to the N/W. MAC needs RM_DL_FIRST_BLOCK_IND to transition to TRANSFER
      ** state prior to handling ACK/NACK request.
      */
      if (dl.b_first_block_ind_sent == FALSE )
      {
        dl.b_first_block_ind_sent = TRUE;

        /* Indicate to MAC so it can enter DL transfer mode
        */
        rlc_msg_ptr = mac_get_cmd_buf_rlc();
        if (rlc_msg_ptr != NULL)
        {
          rlc_msg_ptr->msg.dl_first_block_ind.sig = RM_DL_FIRST_BLOCK_IND;
          mac_put_cmd_rlc( rlc_msg_ptr);
        }
        else
        {
          ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
        }
      }

#ifdef  ERLD_SHOW_BLOCK
      show_dl_block( &e_pl1_dl_blk, &data_block1, &data_block2);
#endif

      /* Save the coding scheme, frame number, time_slot, ES/P, RRBP and
      ** final block indicator from the data block just translated to the
      ** dl struct
      */
      dl.bsn = data_block1.bsn;

      /* Save the TFI for generating EPDAN later on
      */
      dl.tfi = e_cur_hdr.tfi;

      /* Update coding scheme for engg_mode
      */
      if (dl.cs != cur_dl_blk.cs)
      {
        geran_eng_mode_data_write(ENG_MODE_EGPRS_DL_MCS, &cur_dl_blk.cs, TRUE);
      }

      /* copy PL1 header info
      */
      dl.cs   = data_block1.cs;
      dl.sp   = e_cur_hdr.esp;
      dl.rrbp = e_cur_hdr.rrbp;

      if (dl.cs > RM_MCS_6 && dl.cs <= RM_MCS_9)
      {
        dl.fbi = data_block1.fbi | data_block2.fbi;
        dl.vr  = data_block2.bsn;
      }
      else
      {
        dl.fbi = data_block1.fbi;
        dl.vr  = data_block1.bsn;
      }

      e_update_dl_unack_state_vars( dl.bsn);

#if defined(GRLD_SHOW_BLOCK) || defined(GRLD_SHOW_BSN)
      show_bsn_and_fifo_vars();
#endif
      /* latch the FBI value if the current FBI (belonging to a block)
      ** is set and the latch value if it is NOT set
      */
      if (dl.fbi == RM_FBI_FINAL)
      {
        dl.fbi_latch = RM_FBI_FINAL;
      }

      /*------------------------------------------------------------------
      ** Send PCA and enter PENDING_RELEASE if FBI is received. Otherwise
      ** send PDAN if polled. Only send one PCA if processing a dual payload
      **------------------------------------------------------------------
      */
      if (dl.fbi == RM_FBI_FINAL)
      {
        /* Stop T3190 when sending Pkt Ctrl Acknack
        */
        TIMER_STOP_T3190();

        /*----------------------------------------------------
        ** Request MAC to send Packet Control Acknowledgement
        **----------------------------------------------------
        */
        rlc_msg_ptr = mac_get_cmd_buf_rlc();
        if (rlc_msg_ptr != NULL)
        {
          rlc_msg_ptr->msg.sig = RM_DL_PKT_CTRL_ACK_REQ;

          /* rrbp from the current dl data block where polling bit is detected
          */
          rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.rrbp = dl.rrbp;

          /* frame number carried over from PL1 for each data block
          */
          rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.frame_number = dl.frame_number;

          /* ..and time slot
          */
          rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.time_slot = dl.time_slot;

          /* ..and TFI
          */
          rlc_msg_ptr->msg.dl_pkt_ctrl_ack_req.tfi = dl.tfi;

          MSG_GERAN_LOW_0_G("ERLD RX_U FBI Sent PCAck");
          mac_put_cmd_rlc( rlc_msg_ptr);

          /* Next state is unack pending release
          */
          dl.state = DS_RX_UNACK_PENDING_RELEASE;

          dl.test_mode = TEST_MODE_OFF;

          UPD_DL_STATE();
        }
        else
        {
          ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
        }
      } /* end FBI bit set */
      else /* Check if the EPDAN_REQ signal mask is set */
      if( grlc_dl_task_sigs & GRLC_DL_PL1_EPDAN_REQ_SIG )
      {
        epdan_req_ptr = (pl1_rlc_msg_t *)q_check(&DPQ());

        if( epdan_req_ptr == NULL )
        {
          MSG_GERAN_LOW_0_G( "ERLD EPDAN_REQ mask without msg");
        }
        else
        {
          /* Reply to the EPDAN only if the BSN, which the EPDAN has
          ** been requested for, has been procesed i.e BSN < V(R)
          */
          if( IS_GT_MOD( dl.vr, epdan_req_ptr->bsn, dl.ws)  )
          {
            epdan_req_ptr = (pl1_rlc_msg_t *)q_get(&DPQ());

            GRLC_DL_MSG_LOCK();

            if (q_cnt(&DPQ()) == 0)
            {
              RLC_DL_CLR_PL1_EPDAN_REQ_SIG();
            }

            GRLC_DL_MSG_UNLOCK();

            if( epdan_req_ptr != NULL )
            {
              /* get buffer for msg to send to MAC
              */
              rlc_msg_ptr = mac_get_cmd_buf_rlc();
              if (rlc_msg_ptr != NULL)
              {
                /* set FA to 0 in un-ack mode
                */
                rlc_msg_ptr->msg.dl_pkt_acknack_req.e_acknack_desc.fai =
                                    RM_ACKNACK_FAI_TBF_NOT_COMPLETE;

                /*---------------------------------------------------------------
                ** Formulate ssn & rbb array vars. Set signal and send packet dl
                ** acknack RRBP and frame number and TFI
                **---------------------------------------------------------------
                */

                /* Copy the Quality Report from PL1. If the Quality Report is dropped
                ** while the EPDAN description is formatted (ESP 1/2), the following
                ** pointer is set to NULL, in e_format_dl_acknack_desc(), to indicate
                ** to MAC that the QReport should not be included in the EPDAN.
                */
                rlc_msg_ptr->msg.dl_pkt_acknack_req.pl1_qrep_ptr = epdan_req_ptr->qrep_ptr;
#ifdef PERLUTF
                #error code not present
#endif

                e_format_dl_acknack_desc( &rlc_msg_ptr->msg.dl_pkt_acknack_req,
                                          epdan_req_ptr);
                UPD_DL_ACKNACK();
                /* Fill the rest of the message */
                rlc_msg_ptr->msg.sig = RM_DL_PKT_ACKNACK_REQ;
                rlc_msg_ptr->msg.dl_pkt_acknack_req.esp          = epdan_req_ptr->esp;
                rlc_msg_ptr->msg.dl_pkt_acknack_req.frame_number = epdan_req_ptr->frame_no;
                rlc_msg_ptr->msg.dl_pkt_acknack_req.include_chan_req = epdan_req_ptr->
                                                                    include_chan_req;
                rlc_msg_ptr->msg.dl_pkt_acknack_req.rrbp         = epdan_req_ptr->rrbp;
                rlc_msg_ptr->msg.dl_pkt_acknack_req.tfi          = dl.tfi;
                rlc_msg_ptr->msg.dl_pkt_acknack_req.time_slot    = epdan_req_ptr->timeslot;

#ifdef  ERLD_SHOW_EPDAN
                show_epdan ( &rlc_msg_ptr->msg.dl_pkt_acknack_req);
#endif
                /* Log DL EPDAN
                ** Log Code: 0x5211
                */
                rlc_log_epdan( &rlc_msg_ptr->msg.dl_pkt_acknack_req);

                /* Send EPDAN to MAC */
                mac_put_cmd_rlc( rlc_msg_ptr);

                /* Free epdan_req_ptr */
#if defined (DEBUG_GSM_EGPRS_DS_UNIT_TEST) || defined (PERLUTF)
                #error code not present
#else
                gprs_rlc_l1isr_free_buffer( epdan_req_ptr);
#endif /* DEBUG_GSM_EGPRS_DS_UNIT_TEST / PERLUTF */
              }
              else
              {
                ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
              }
            } /* end if( epdan_req_ptr != NULL ) */
            else
            {
              MSG_GERAN_LOW_0_G( "ERLD EPDAN_REQ without sig in DPQueue");
            }
          }  /* if( IS_GT_MOD( dl.vr, epdan_req_ptr->bsn, dl.ws ) */
        }  /*  epdan_req_ptr == NULL */
      }/*  end if( grlc_dl_task_sigs & GRLC_DL_PL1_EPDAN_REQ_SIG ) */

      if( dl.test_mode == TEST_MODE_B )
      {
        /* Check if DL blk needs to be looped back on the UL
        ** Generate number of blks based on cnt send by L1 for that timeslot
        */
        if ( loop_blk_cnt )
        {
          while (loop_blk_cnt)
          {
            /*==================================================
            ** Convert DL data block(s) to an UL data block(s) and
            ** send it to UL FIFO.
            **==================================================
            */
            ul.cv                    = 15;

            /* Fill UL Header */
            ul_data_block1.hdr.cv    = 15; /* No countdown in Test Mode B */
            ul_data_block1.hdr.retry = ul.cur_res.retry;
            ul_data_block1.hdr.si    = 0;  /* N/A */
            ul_data_block1.hdr.tfi   = ul.cur_res.ul_tfi;

            ul_data_block1.hdr.bsn1   = data_block1.bsn;
            ul_data_block1.bsn        = data_block1.bsn;
            ul_data_block1.hdr.cps    = e_cur_hdr.cps;
            ul_data_block1.hdr.rsb    = RM_RSB_1ST_TX;

            /* Fill the rest of the fields */
            ul_data_block1.hdr.pi        = 0; /* N/A */
            ul_data_block1.ti            = 0; /* N/A */
            ul_data_block1.ext   = data_block1.ext;

            /* Set the MCS and PS for the UL block */
            ul_data_block1.cs        = data_block1.cs;
            ul_data_block1.init_mcs  = data_block1.cs;
            ul_data_block1.ps        = 0;

            /* Make sure that there are no extension octets */
            ul_data_block1.num_ext_octets    = 0;

            /* Set remaining octet counter to zero to avoid adding filling octets */
            ul_data_block1.rem_cnt  = 0;

            ul_data_block1.pdu_prio = GPDU_LOW_PRIORITY;

            /* Copy the dl payload to the ul payload. Use the dup pointer since it
            ** is used in xlate_int_ul_data ( )
            */
            ul_data_block1.dup_dsm_ptr = gprs_pdu_copy( data_block1.dsm_payload_ptr );

            /* Calculate the length of the payload */
            ul_data_block1.data_len = (uint8)GPRS_PDU_LENGTH( ul_data_block1.dup_dsm_ptr );

            if (dl.cs > RM_MCS_6 && dl.cs <= RM_MCS_9)
            {
              /* Deal with the 2nd payload
              */
              ul_data_block1.hdr.bsn2 = MOD_DIF_ESNS( data_block2.bsn, data_block1.bsn);
              ul_data_block2.hdr      = ul_data_block1.hdr;
              ul_data_block2.bsn      = data_block2.bsn;
              ul_data_block2.hdr.bsn1 = data_block2.bsn;
              ul_data_block2.hdr.bsn2 = 0; /* N/A */

              /* Fill the rest of the fields */
              ul_data_block2.hdr.pi        = 0; /* N/A */
              ul_data_block2.ti       = 0; /* N/A */

              ul_data_block2.ext   = data_block2.ext;

              /* Set the MCS and PS for the UL block */
              ul_data_block2.cs        = data_block2.cs;
              ul_data_block2.init_mcs  = data_block2.cs;
              ul_data_block2.ps        = 0;


              /* Make sure that there are no extension octets */
              ul_data_block2.num_ext_octets    = 0;

              /* Set remaining octet counter to zero to avoid adding filling octets */
              ul_data_block2.rem_cnt  = 0;

              ul_data_block2.pdu_prio = GPDU_LOW_PRIORITY;

              /* Copy the dl payload to the ul payload. Use the dup pointer since it
              ** is used in xlate_int_ul_data ( )
              */
              ul_data_block2.dup_dsm_ptr = gprs_pdu_copy( data_block2.dsm_payload_ptr );

              /* Calculate the length of the payload */
              ul_data_block2.data_len =
                  (uint8)GPRS_PDU_LENGTH( ul_data_block2.dup_dsm_ptr );
            }

            /* Put the block into the FIFO */
            if( UL_FIFO_PUT_OK !=
                e_grlc_ul_fifo_put_block( &ul_data_block1,
                                         ((dl.cs > RM_MCS_6) ? &ul_data_block2 : NULL),
                                         TXA_PTX
                                         )
              )
            {
              MSG_GERAN_LOW_0_G("ERLD DS_TEST_MODE error in fifo put block ");
            }
            else
            {
              MSG_GERAN_LOW_3_G( "ERLD DS_TEST_MODE B sent bsn1=%d bsn2=%d tn=%d",
                             ul_data_block1.hdr.bsn1 ,
                             (dl.cs > RM_MCS_6) ?
                             MOD_ESNS(ul_data_block1.hdr.bsn1 + ul_data_block1.hdr.bsn2):
                             0,
                             e_pl1_dl_blk.time_slot);

              /*=====================================================================
              ** Inform PL1 to start polling the FIFO for data blocks by sending the
              ** PH_DATA_REQ primitive on the very first data block of a TBF
              **=====================================================================
              */
              if (ul.b_first_fifo_load)
              {
                pl1_send_ph_data_req();
                grlc_ul_fifo_enable_pl1_access();

                ul.b_first_fifo_load = FALSE;
              }
            }
            --loop_blk_cnt;
          }

          /* Free DL payload */
          GPRS_PDU_FREE( &data_block1.dsm_payload_ptr);

          if( data_block1.dsm_payload_ptr != NULL )
            MSG_GERAN_LOW_0_G("ERLD DS_TEST_MODE failed to free payload");

          if (dl.cs > RM_MCS_6 && dl.cs <= RM_MCS_9)
          {
            /* Free DL payload */
            GPRS_PDU_FREE( &data_block2.dsm_payload_ptr);

            if( data_block2.dsm_payload_ptr != NULL )
              MSG_GERAN_LOW_0_G("ERLD DS_TEST_MODE failed to free payload");
          }
        } /* end TEST_MODE_B */
        else
        {
          /* No need to loop the DL block back on the UL. Free DSM items */
          if( data_block1.dsm_payload_ptr )
          {
            GPRS_PDU_FREE( &data_block1.dsm_payload_ptr);
          }
          if( data_block2.dsm_payload_ptr )
          {
            GPRS_PDU_FREE( &data_block2.dsm_payload_ptr);
          }
        }
      } /* end TEST_MODE_B */
      else
      {
        /* In EGPRS SRB test mode, just discard the DL data block(s) & free DSM items */
        if( data_block1.dsm_payload_ptr )
        {
          GPRS_PDU_FREE( &data_block1.dsm_payload_ptr);
        }
        if( data_block2.dsm_payload_ptr )
        {
          GPRS_PDU_FREE( &data_block2.dsm_payload_ptr);
        }
      } /* end TEST_MODE_SRB */
    }
  }/* end reading DL fifo loop */
}/* e_ds_rx_unack_test_mode_state_pl1_ph_data_ind_event_handler() */


/*** EOF: don't remove! ***/
