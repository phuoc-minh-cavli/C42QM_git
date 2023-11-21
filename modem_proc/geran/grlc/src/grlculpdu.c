/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC UPLINK PDU
***
***
*** DESCRIPTION
***
***  This module performs PDU processing functions, at exceptions, such as
***  loading, un-loading, shortening, truncation, restoring and freeing etc..
***
*** EXTERNALIZED FUNCTIONS
***
***  gprs_grr_ul_mes_t *load_cur_pdu (void)
***  void unput_pdu (gprs_grr_ul_mes_t *msg_ptr)
***
***  void unput_next_pdu (void)
***
***  void shorten_cur_pdu (uint16 offset)
***  void delink_data_pdus (void)
***  void relink_data_pdus (void)
***
***  void dequeue_pdu (gprs_grr_ul_mes_t **pptr)
***  void enqueue_pdu (gprs_grr_ul_mes_t *grr_ul_msg_ptr )
***
***  void free_llc_pdu_and_msg(gprs_grr_ul_mes_t *msg_ptr)
***  void grlc_ul_del_signalling_pdus(uint8 nas_indication )
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  - struct ul must be declared
***  - GRLC_UL_MAX_PAQ_SIZE must be defined at compile time
***  - grlc_ul_init_paq() must be called prior to calling other functions
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
*** $Header: //components/rel/geran.mpss/5.2.0/grlc/src/grlculpdu.c#6 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 05/10/13   gk      CR489056 - Uninit'd DUPs items in Temp queue incorrectly freed
*** 10/04/13   hv      CR472248 - Issue with reallocation in change of RLC mode
*** 05/07/11   hv      Added promoting sig PDUs to the front of the uplink queue
*** 08/03/04   hv      Added EGPRS code
*** 11/25/03   hv      Created
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST  */

#include "comdef.h"
#include "queue.h"
#include "grlcllc.h"
#include "grlcultst.h"
#include "grlctx.h"
#include "grlcsgm.h"
#include "grlculproc.h"
#include "grlci.h"
#include "grlculpdu.h"
#include "geran_tasks.h"
#include "geran_dual_sim_g.h"
#include "grlc_dyn_api.h"

#if defined TEST_FRAMEWORK
#error code not present
#endif

/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*==========================================================================
===
===  FUNCTION      GRLC_UL_RE_SEQUENCE_PDUS
===
===  DESCRIPTION
===
===    Re-assign PSN values to PDUs in the queue following PDUs re-ordering
===
===  PARAMS
===    psn -- the first PDU in the queue is to be assigned this PSN
===
===  DEPENDENCIES
===
===    ul, ULQ
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
static void grlc_ul_re_sequence_pdus (uint16 psn)
{
  gprs_grr_ul_mes_t*    next_ptr;
  uint16                qcnt;
  uint16                start_psn = psn;

  /*------------------------------------------------------------------------*/

  qcnt = (uint16)q_cnt(&ULQ());

  /* Peek the PDU at the front of the queue */
  next_ptr = q_check(&ULQ());

  if (next_ptr == NULL)
  {
    MSG_GERAN_LOW_2_G("GRLU re-psn head PDU NULL nw psn=%u qcnt=%u",psn, qcnt);
    return;
  }
  else
  {
    do
    {
      next_ptr->psn = psn;
      /* Reset the realloc state to avoid realloc */
      next_ptr->realloc_action = RA_NULL;
      next_ptr = q_next(&ULQ(), &next_ptr->link);

      qcnt--;
      INC_MOD(psn,32768);
    } while (next_ptr != NULL && qcnt);
  }

  /* If PDU(s) were deleted. Then pg.psn should hold the last PSN value.
  ** When a new PDU is enqueued, The next PDU will take that PSN value +1,
  ** to avoid a gap
  */
  DEC_MOD(psn, 32768);
  MSG_GERAN_HIGH_3_G("GRLU re-seq-pdus old pg.psn=%u start_psn=%u end_psn=%u", pg.psn, start_psn, psn);
  pg.psn = psn;
} /* grlc_ul_re_sequence_pdus */

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GRLC_UL_DEL_SIGNALLING_PDUS()
===
===  DESCRIPTION
===
===    Delete all SIGNALLING PDUs from the LLC PDU main queue.
===    First all PDUs in the PAQ queue and current and next pdu pointers
===    are returned to the main queue. The PDU is then read out one by one,
===    and deleted if a mtach occurs, otherwise it is put in the PAQ queue.
===    Finally the PAQ queue is unput in reverse order
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_del_signalling_pdus (uint8 nas_indication )
{
  gprs_grr_ul_mes_t*  pdu_ptr;
  uint16              u;

  /* Make sure all PDUs from the PAQ and next_pdu_ptr and cur_pdu_ptr are put
  ** back in the LLC PDU main queue
  */
  grlc_ul_unput_paq();

  u = (uint16)q_cnt(&ULQ());
  while (u > 0)
  {
    pdu_ptr = (gprs_grr_ul_mes_t*) q_get(&ULQ());
    if (pdu_ptr != NULL)
    {
      /* Delete all the signalling PDUs in the Q , after MS performs
      ** cell reselection. This is required because MS should not
      ** retain any older signaling PDUS as it would confuse the NW
      ** in the New Cell.
      */
      if (pdu_ptr->cause != GRR_CAUSE_LAY3_DATA &&
          /*
          ** added GRR_CAUSE_SM_SIGNALLING cause as SM PDU is cell independent
          ** and should be tried after cell is camped.
          */
          pdu_ptr->cause != GRR_CAUSE_SM_SIGNALLING
         )
      {
        MSG_GERAN_MED_3_G("GRLU del_sig_pdus psn=%u cause=%u len=%u",
                pdu_ptr->psn,
                pdu_ptr->cause,
                pdu_ptr->len);

        /*
        ** PDUs with following causes are deleted:
        ** GRR_CAUSE_GMM_SIGNALLING
        ** GRR_CAUSE_LLC_SIGNALLING
        ** GRR_CAUSE_GMM_PAGE_RESPONSE
        ** GRR_CAUSE_GMM_CELL_UPDATE
        ** GRR_CAUSE_GMM_CELL_NOTIFY
        */

        if(( GRR_CAUSE_GMM_SIGNALLING == pdu_ptr->cause ) &&
           ( TRUE == ul.b_access )                &&
           ( NAS_INDICATION == nas_indication )
          )
        {
          /* Inform NAS when SIG PDU is deleted so it can be retried quickly
          */
          MSG_GERAN_HIGH_1_G("GRLU inform gmm pdu tx failure when ps access = %d", ul.b_access);
          indicate_to_llc_rlcmac_failure( pdu_ptr);
        }

        free_llc_pdu_and_msg(pdu_ptr);
      }
      else
      {
        /* PDU is not a match, move it into the PAQ
        */
        grlc_ul_put_paq( pdu_ptr);
      }
    }
    else
    {
      MSG_GERAN_LOW_1_G("GRLU del_signalling_pdus PDU null when q_cnt=%u", u);
    }

    --u;
  }
  if (ul.paq_ctr > 0)
  {
    grlc_ul_unput_paq();

    /* Re-sequence the PSNs
    */
    pdu_ptr = q_check(&ULQ());
    if (NULL != pdu_ptr)
    {
      grlc_ul_re_sequence_pdus( pdu_ptr->psn);
      MSG_GERAN_LOW_1_G("GRLU del_signalling_pdus starting PSN=%u", pdu_ptr->psn);
    }
  }

  pg_clr_tbc_info( &pg.tbci);
  grlc_llc_ul_flow_control();

} /* grlc_ul_del_signalling_pdus */


/*===========================================================================
===
===  FUNCTION      LOAD_CUR_PDU()
===
===  DESCRIPTION
===
===    Attempt to load the current PDU and the next pdu with PDUs from the queue.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    cur_pdu_ptr is returned either NULL or pointing to a valid PDU
===
===  SIDE EFFECTS
===
===========================================================================*/
gprs_grr_ul_mes_t *load_cur_pdu (void)
{
  uint32 temp_tlli;

  /* Attempt to load current PDU from the next PDU or from the queue
  */
  if (cur_pdu_ptr == NULL)
  {
    /* If next PDU is loaded then transfer it to current PDU
    */
    if (next_pdu_ptr != NULL)
    {
      /* cur pdu and backup PDUs are empty. So load the next PDU
      */
      cur_pdu_ptr = next_pdu_ptr;
      next_pdu_ptr = NULL;
    }
    else
    {
      /* cur PDU, backup PDU and next PDU are all empty so load one
      ** from the PDU queue
      */
      dequeue_pdu( &cur_pdu_ptr);
    }

    dequeue_pdu( &next_pdu_ptr);

    /*----------------------------------------------------------------------------
    ** If there is a valid PDU then overlay the TLLI passed from LLC with the
    ** latest TLLI value in the Public Store. Except when the TLLI value from the
    ** Public Store contains all 0xff's
    **----------------------------------------------------------------------------
    */
    if (cur_pdu_ptr != NULL)
    {
      /* Reset reallocation state when a new current PDU is loaded
      */
      if (ul.realloc_state == RALS_COMPLETE)
        ul.realloc_state = RALS_NULL;

      temp_tlli = 0L;
#if  defined (DEBUG_GSM_GPRS_RLC_TEST) || defined (PERLUTF)
      #error code not present
#else
      rr_read_public_store(RRPS_TLLI,&temp_tlli);
#endif
      if (temp_tlli != (uint32)0xffffffff)
      {
        cur_pdu_ptr->tlli = temp_tlli;
        MSG_GERAN_LOW_3_G("GRLU tlli_ovl p=%u old_tlli=0x%x new=0x%x",
                cur_pdu_ptr->psn,
                cur_pdu_ptr->tlli,
                temp_tlli);
      }

      MSG_GERAN_LOW_2_G("GRLU load check_prr cur_p=0x%x next_p=0x%x",cur_pdu_ptr, next_pdu_ptr);
      check_to_send_prr();
    }
  }

  /* when loading the next pdu as cur_pdu_ptr, if the previous pdu
  ** is marked for free, free it now
  */
  if ( (cur_pdu_ptr != NULL) && (ul.pdu_freeing_state == FREE_CURRENT_PDU))
  {
    /* Successful transmission of PDU , set the flag to TRUE
    ** to start ready timer
    */
    grlc_ul_del_free_paq_upto_psn( ul.psn_to_free_inclusive,TRUE);
    ul.pdu_freeing_state = NONE_TO_FREE;
  }

  /* Place PDU on PAQ only if one was loaded
  */
  if (cur_pdu_ptr != NULL)
  {
    if (!grlc_ul_paq_does_psn_exist( cur_pdu_ptr->psn))
    {
      grlc_ul_put_paq( cur_pdu_ptr);

#ifdef  GSM_GPRS_GRLU_SHOW_DATA_XFER
      MSG_GERAN_LOW_3_G("GRLU load_cur p=%u l=%u ol=%u",
              cur_pdu_ptr->psn,
              cur_pdu_ptr->len,
              cur_pdu_ptr->orig_len);
#endif

      /* Re-assess flow control
      */
      /* Every loading of new pdu would result in advancement of V(s)
      ** As the flow control is based on Va and Vs, it is efficient to
      ** flow control during load_cur_pdu rather than enqueue PDU or every
      ** Pkt AckNack (without mcs/tfi/ts) change.
      ** For Pkt AckNack with mcs/tfi/ts change - flow control alreasy exists.
      */
      grlc_llc_ul_flow_control();
    }
  }

  /* cur_pdu_ptr is NULL or pointing to a valid PDU
  */
  return cur_pdu_ptr;

} /* load_cur_pdu */

/*===========================================================================
===
===  FUNCTION       UNPUT_PDU
===
===  DESCRIPTION
===
===    Put back LLC PDU message in msg_ptr back on the LLC UL Q
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void unput_pdu (gprs_grr_ul_mes_t *msg_ptr)
{
  gprs_grr_ul_mes_t*    head_pdu_ptr;
  uint16                qcnt;
  /*--------------------------------------------------------*/

  qcnt = (uint16)q_cnt(&ULQ());
  if (msg_ptr == NULL)
  {
    MSG_GERAN_LOW_1_G("GRLU unp NULL pdu qcnt=%u",qcnt);
  }
  else
  {
#ifdef  GSM_GPRS_GRLU_SHOW_PAQ
    MSG_GERAN_MED_3_G("GRLU unp %u qcnt=%u msg_ptr=%x",msg_ptr->psn, qcnt, msg_ptr);
    MSG_GERAN_MED_2_G("GRLU     ptr=%x dup=%x",msg_ptr->pdu_ptr, msg_ptr->dup_pdu_ptr);
#endif
    /* clear grouping state
    */
    pg_clr_group_info(msg_ptr);

    /* Get the PDU at the front of the queue
    */
    head_pdu_ptr = q_check(&ULQ());

    if (head_pdu_ptr)
    {
      /* Unput the next PDU using q_insert when the queue is NOT empty
      */
      q_insert(
               &ULQ(),
               &msg_ptr->link,        /* item to insert */
               &head_pdu_ptr->link    /* insertion point */
              );
    }
    else
    {
      /* Otherwise we must use q_put() because q_insert() is not able to insert into
      ** an empty queue !
      */
      q_put( &ULQ(), &msg_ptr->link );
    }
  }
} /* unput_pdu */

/*===========================================================================
===
===  FUNCTION      UNPUT_NEXT_PDU()
===
===  DESCRIPTION
===
===    Puts back the next PDU on the queue if it is loaded.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    next_pdu_ptr, PDU queue
===
============================================================================*/
void unput_next_pdu (void)
{
  gprs_grr_ul_mes_t*    head_pdu_ptr;

  /*--------------------------------------------------------*/

  if (next_pdu_ptr != NULL)
  {
    /* clear grouping state
    */
    pg_clr_group_info(next_pdu_ptr);

    MSG_GERAN_LOW_3_G("GRLU unp_next qcnt=%u n_p=%u next=%x",
            q_cnt(&ULQ()),
            next_pdu_ptr->psn,
            next_pdu_ptr);

    /* Get the PDU at the front of the queue
    */
    head_pdu_ptr = q_check(&ULQ());

    if (head_pdu_ptr)
    {
      /* Unput the next PDU using q_insert when the queue is NOT empty
      */
      q_insert(
               &ULQ(),
               &next_pdu_ptr->link,  /* item to insert */
               &head_pdu_ptr->link   /* insertion point */
              );
    }
    else
    {
      /* Otherwise we must use q_put() because q_put() is not able to insert into
      ** an empty queue !
      */
      q_put( &ULQ(), &next_pdu_ptr->link );
    }

    next_pdu_ptr = NULL;
  }
} /* unput_next_pdu */

/*===========================================================================
===
===  FUNCTION      SHORTEN_CUR_PDU
===
===  DESCRIPTION
===
===     Shorten and restore the current PDU:
===     - free the remaining of the working copy of the payload (dup_pdu_ptr)
===     - shorten the original payload by stripping 'offset' octets off the start
===     - dup the shortened payload and use it as a new working copy of the payload
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void shorten_cur_pdu (uint16 offset)
{
  dsm_item_type *temp_dsm_ptr = NULL;

  /* Mark the current PDU offset
  */
  MSG_GERAN_MED_3_G("GRLU psn=%u cur-ofs=%u new-ofs=%u",
          cur_pdu_ptr->psn,
          cur_pdu_ptr->pdu_ofs,
          offset
         );
  cur_pdu_ptr->pdu_ofs = offset;

  if (cur_pdu_ptr->dup_pdu_ptr != NULL)
  {
    GPRS_PDU_FREE(&cur_pdu_ptr->dup_pdu_ptr);
  }

  /* First, create a temporary dup of the original PDU
  */
  cur_pdu_ptr->dup_pdu_ptr = gprs_pdu_duplicate (cur_pdu_ptr->pdu_ptr, 0, cur_pdu_ptr->orig_len);

  if (offset > 0)
  {
    /* truncate the temporary version of the original PDU by the (existing truncation and
    ** the additional truncation)
    */
    temp_dsm_ptr = gprs_pdu_segment_head ( &(cur_pdu_ptr->dup_pdu_ptr), offset);

    /* Now free the head bit (offset)
    */
    if (temp_dsm_ptr != NULL)
    {
      GPRS_PDU_FREE(&temp_dsm_ptr);
    }
  }

  /* Remaining length is the new length
  */
  cur_pdu_ptr->len = (uint16)GPRS_PDU_LENGTH(cur_pdu_ptr->dup_pdu_ptr);

  /* Check for DSM error and recover
  */
  if (cur_pdu_ptr->len > 0)
  {
    if (cur_pdu_ptr->dup_pdu_ptr == NULL)
    {
      MSG_GERAN_ERROR_0_G("GRLU ##shorten dup returns NULL");
      grlc_ul_show_dsm_info();
      grlc_ul_show_vars();

      /* Failed to create dup'ed packet. Set flag to release at the appro fn level
      */
      ul.b_error_recovery_required = TRUE;
#ifndef FEATURE_GERAN_REDUCED_DEBUG
      ul_diag_stats_st.ul_gen_purpose_count_a++;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

      return;
    }
  }
  else
  {
    MSG_GERAN_HIGH_2_G("GRLU ##shorten len=0. psn=%u dup_pdu=0x%x",
              cur_pdu_ptr->psn,
              cur_pdu_ptr->dup_pdu_ptr
              );
  }

  /* clear the remain-index so processing starts from the 1st octet
  */
  cur_pdu_ptr->rem_ind = 0;
  pg_clr_group_info(cur_pdu_ptr);

#ifdef  GSM_GPRS_GRLU_SHOW_PAQ
  MSG_GERAN_MED_3_G("GRLU shorten %u orig_l=%u len=%u",
          cur_pdu_ptr->psn,
          cur_pdu_ptr->orig_len,
          cur_pdu_ptr->len);
#endif
} /* shorten_cur_pdu */

/*===========================================================================
===
===  FUNCTION      DELINK_DATA_PDUS
===
===  DESCRIPTION
===
===    Moves all PDUs from the ULQ to grlc_ul_old_llc_q
===
===  PARAMS
===
===  DEPENDENCIES
===
===    ul_q
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void delink_data_pdus (void)
{
  uint16            u;
  gprs_grr_ul_mes_t *ptr;
  rlc_ul_state_t    cur_state;

  /*Improve failure indication of GMM/SM PDUs */
  #define FAILURES_IND_MAX 100
  uint8              index = 0;
  uint8              i;
  boolean            b_fail = FALSE;
  indicate_failure_t failures_ind[FAILURES_IND_MAX];

  /*-------------------------------------------------------------*/

  /* Free the cur_pdu_ptr PDU it has been already acked
  */
  if (cur_pdu_ptr != NULL &&
      ul.pdu_freeing_state == FREE_CURRENT_PDU &&
      cur_pdu_ptr->psn == ul.psn_to_free_inclusive)
  {
     /* Successful transmission of PDU , set the flag to TRUE
     ** to start ready timer
     */
     grlc_ul_del_free_paq_upto_psn( cur_pdu_ptr->psn,TRUE);
     ul.pdu_freeing_state = NONE_TO_FREE;
     ul.psn_to_free_inclusive = ul.first_psn_to_free;
  }

  /*
  ** Free any NAS PDU if they were completely transmitted to avoid retry in NAS
  */
  if ( (cur_pdu_ptr != NULL)                            &&
       ( cur_pdu_ptr->cause == GRR_CAUSE_GMM_SIGNALLING ||
         cur_pdu_ptr->cause == GRR_CAUSE_SM_SIGNALLING
       )                                                &&
       (is_pdu_transmitted( cur_pdu_ptr->psn))
     )
  {
    MSG_GERAN_LOW_2_G("GRLU Delete higher layer pdu cause=%d  psn=%d",cur_pdu_ptr->cause,
      cur_pdu_ptr->psn);
    /* FALSE - because not explictely acked by NW */
    grlc_ul_del_free_paq_upto_psn( cur_pdu_ptr->psn,FALSE);
  }

  /* Restore and unput all PDUs in the PAQ into the main LLC UL Q.
  */
  grlc_ul_unput_paq();

  /* Use 'u' to avoid calling q_cnt() multiple times in MSG_X() macro.
  ** 'u' holds the number of data PDUs
  */
  u = (uint16)q_cnt(&ULQ());

  /* Xfer the data PDUs in the main queue into the reserved queue
  */
  while (q_cnt(&ULQ()) > 0)
  {
    ptr = (gprs_grr_ul_mes_t *)q_get(&ULQ());
    if (ptr != NULL)
    {
      if (ptr->cause != GRR_CAUSE_LAY3_DATA &&
          ptr->cause != GRR_CAUSE_LLC_SIGNALLING)
      {
        MSG_GERAN_LOW_3_G("GRLU delink del non-data psn=%u cause=%u transaction_id=%u",
                ptr->psn,
                ptr->cause,
                ptr->transaction_id);

        /* To avoid indicating failure for the same GMM/SM PDU multiple times, we have an array
        ** which stores the cause & transaction id each time we indicate failure, so next time we
        ** come across the same PDU, we check against the array and only indicate if its unique.
        **/

        if( ptr->cause == GRR_CAUSE_GMM_SIGNALLING ||
            ptr->cause == GRR_CAUSE_SM_SIGNALLING )
        {
          if( index != 0 )
          {
            /* Presume to add item to array
            */
            b_fail = TRUE;

            for(i = 0; i<index; i++)
            {
              /* Same cause & same TI
              */
              if( failures_ind[i].cause          == ptr->cause &&
                  failures_ind[i].transaction_id == ptr->transaction_id )
              {
                /* Found match: no adding to array
                */
                b_fail = FALSE;
                break;
              }
            } /* for loop */
          }
          else
          {
            /* Add item to array
            */
            b_fail = TRUE;
          }

          /* Search array failed to find a match so add item to array
          */
          if(b_fail == TRUE)
          {
            if(index < FAILURES_IND_MAX)
            {
              failures_ind[index].cause          = ptr->cause;
              failures_ind[index].transaction_id = ptr->transaction_id;
              ++index;
            }
            else
            {
              MSG_GERAN_LOW_1_G("Failures_ind reached MAX size=%d",index);
            }
            indicate_to_llc_rlcmac_failure( ptr);
          }
        }

        /* Free the PDU */
        free_llc_pdu_and_msg(ptr);
      }
      else
      {
        /* realloc_action will be cleared in relink_data_pdu(). In case,
        ** GRR_PURGE_DATA_REQ received while PDUs are in reserve queue, then
        ** reallocation parameters held in "ul" struct could get cleared if a
        ** PDU held in reserve queue is as same as that in ul.realloc_pdu_ptr.
        ** In anticipation of this, clear realloc_action now.
        */
        ptr->realloc_action = RA_NULL;
        q_put(&grlc_ul_old_llc_q, &(ptr->link));
      }
    }
  }

  /* Clear PDU grouping info
  */
  pg_clr_tbc_info( &pg.tbci);

  /* Clear UL data
  */
  cur_state = ul.state;
  grlc_ul_init_vars();
  ul.state = cur_state;

  /* Init the number of LLC PDUs and octets
  */
  ul.llc_pdus   = 0;
  ul.llc_octets = 0;

  MSG_GERAN_HIGH_3_G("GRLU Delink s=%u qcnt=%u susp=%u", ul.state, u, ul.suspend_state);

} /* delink_data_pdus */

/*===========================================================================
===
===  FUNCTION      RELINK_DATA_PDUS
===
===  DESCRIPTION
===
===    Moves all PDUs in the Reserve queue back the main ULQ
===
===  PARAMS
===
===  DEPENDENCIES
===
===    ul_q
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void relink_data_pdus (void)
{
  uint16            u;
  gprs_grr_ul_mes_t *ptr;
  uint32            temp_tlli;

  /*-------------------------------------------------------------*/


  while (q_cnt(&grlc_ul_old_llc_q) > 0)
  {
    ptr = (gprs_grr_ul_mes_t *)q_get(&grlc_ul_old_llc_q);
    if (ptr != NULL)
    {
      /*----------------------------------------------------------------------------
      ** It is likely that during the user data suspension phase a signalling
      ** exchange such as a RAU may have seen the TLLI reassigned by the network.
      ** It is beneficial to update the TLLI for the restored PDUs now, rather than
      ** leave it to load_cur_pdu(), to avoid the grouping mechanisms causing each
      ** PDU to go out in a separate TBF.
      **----------------------------------------------------------------------------
      */

      temp_tlli = 0L;
#if  defined (DEBUG_GSM_GPRS_RLC_TEST) || defined (PERLUTF)
      #error code not present
#else
      rr_read_public_store(RRPS_TLLI,&temp_tlli);
#endif /* defined (DEBUG_GSM_GPRS_RLC_TEST) || defined (PERLUTF) */
      if (temp_tlli != (uint32)0xffffffff)
      {
        ptr->tlli = temp_tlli;
      }

      /* Refresh the psn's of old data PDUs.
      */
      group_pdu_init( ptr);

      q_put(&ULQ(), &(ptr->link));

      /* Adjust the number of LLC PDUs and octets
      */
      ul.llc_pdus++;
      ul.llc_octets += (uint32)ptr->len;
    }
  };

  /* Use 'u' in MSG_MED later
  */
  u = (uint16)q_cnt(&ULQ());

  /* Clear PDU grouping info if not in transfer as we could alter the ptx_cv
  */
  if (ul.state != US_ACK_XFER  &&
      ul.state != US_UNACK_XFER
     )
  {
    pg_clr_tbc_info( &pg.tbci);
  }

  MSG_GERAN_HIGH_3_G("GRLU Relink s=%u qcnt=%u susp=%u", ul.state, u, ul.suspend_state);

} /* relink_data_pdus */

/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*==========================================================================
===
===  FUNCTION      GRLC_UL_INSERT_PDU
===
===  DESCRIPTION
===
===    Traverse the RLC uplink queue and insert the incoming PDU at the head
===    head of the queue, if no others PDUs were previously inserted, or behind
===    other previously inserted PDUs
===
===    Expect cur_pdu_ptr to be un-loaded
===
===  PARAMS
===    grr_ul_msg_ptr -- pointer to PDU to be inserted
===
===  DEPENDENCIES
===
===    ul, ULQ
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
static void grlc_ul_insert_pdu
(
  gprs_grr_ul_mes_t   *grr_ul_msg_ptr
)
{
  gprs_grr_ul_mes_t*    next_ptr;       /* temp pointer */
  uint16                qcnt;           /* bound-safety counter */

  /*------------------------------------------------------------------------*/

  if (grr_ul_msg_ptr != NULL)
  {
    /* Obtain head ptr and queue count
    */
    qcnt = (uint16)q_cnt(&ULQ());
    next_ptr = q_check(&ULQ());

    /* Traverse the queue looking for insertion point
    */
    while ( next_ptr != NULL &&
            qcnt != 0
          )
    {
      /* Has this PDU satisfied the SWAP criteria. If so it must have been inserted
      ** before, in which case our incoming PDU should be behind it, hence skip
      */
      if (cur_pdu_ptr != NULL &&
          IS_SWAP_REQUIRED(cur_pdu_ptr, next_ptr)
         )
      {
        MSG_GERAN_LOW_1_G("GRLU enq SWAP INS skip cnt=%u", qcnt);

        /* Skip to the next PDU in the queue
        */
        next_ptr = q_next(&ULQ(), &next_ptr->link);
      }
      else
      {
        MSG_GERAN_LOW_1_G("GRLU enq SWAP INS brk cnt=%u", qcnt);

        /* Insertion is at PDU held in next_pdu
        */
        break;
      }

      qcnt--;
    };

    /* At this point if qcnt is:
    ** - zero. Incoming PDU to be appended to the queue
    ** - non-zero. Incoming PDU to be inserted at next_ptr
    */
    MSG_GERAN_LOW_1_G("GRLU enq SWAP loop-end qcnt=%u", qcnt);

    (void)q_link(grr_ul_msg_ptr, &grr_ul_msg_ptr->link);

    if (next_ptr != NULL)
    {
      MSG_GERAN_LOW_0_G("GRLU enq SWAP ins");

      /* Insert PDU using q_insert if the queue is NOT empty
      */
      q_insert(
               &ULQ(),
               &grr_ul_msg_ptr->link,       /* item to insert */
               &next_ptr->link              /* insertion point */
              );
    }
    else
    {
      MSG_GERAN_LOW_0_G("GRLU enq SWAP append");

      /* Otherwise we must use q_put() because q_insert() is not able to insert into
      ** an empty queue !
      */
      q_put( &ULQ(), &grr_ul_msg_ptr->link );
    }
  }
  else
  {
    MSG_GERAN_LOW_0_G("GRLU enq SWAP PDU NULL");
  }
} /* grlc_ul_insert_pdu */

/*==========================================================================
===
===  FUNCTION      GRLC_UL_REORDER_PDUS
===
===  DESCRIPTION
===
===    Sort the main uplink Q and Temp Q, such that the signalling PDUs are
===    at the front of the ULQ , everytime there is a release.
===
===  PARAMS
===
===  DEPENDENCIES
===
===    ul, ULQ, ULTQ
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_reorder_pdus (void)
{
  gprs_grr_ul_mes_t *next, *head, *temp;
  int               qcnt;
  uint16            psn = 1;
  boolean           b_reorder = FALSE;
  /*------------------------------------------------------------------------*/


  /* ULQ and ULTQ may both contain data and sig PDUs. So first consolidate by
  **  1) copying ULTQ to ULQ, then
  **  2) sort the ULQ with sig PDUs to the front - in-situ
  */

  /* 1) PDUs from ULTQ is rappended to ULQ,
  ** eg ULQ = A,B,C, TQ = D, E, after copying ULQ = A, B, C ,D, E. TQ = empty
  */

  /* get head item from TQ and initialize the pdu's
  */
  grlc_ul_handle_llc_tmp_pdu_sig_dyn();

  /* Sort ONLY if queue is not empty
  */
  qcnt = q_cnt(&ULQ());
  head = q_check(&ULQ());
  if ( (qcnt > 1) && (NULL != head) )
  {
    /* Remember the 1st PSN number for PDU re-sequencing later
    */
    psn = head->psn;

    /* Traverse the queue to seek an insertion point if the 1st PDU is a signalling PDU,
    ** or if there is a group of CONTIGUOUS signalling PDUs
    */
    while ( (qcnt > 0) &&
            (head != NULL) &&
            (
             head->cause == GRR_CAUSE_GMM_SIGNALLING    ||
             head->cause == GRR_CAUSE_LLC_SIGNALLING    ||
             head->cause == GRR_CAUSE_GMM_PAGE_RESPONSE ||
             head->cause == GRR_CAUSE_GMM_CELL_UPDATE   ||
             head->cause == GRR_CAUSE_GMM_CELL_NOTIFY   ||
             head->cause == GRR_CAUSE_SM_SIGNALLING
            )
          )
    {
      head = q_next(&ULQ(), &head->link);
      --qcnt;
    }

    /* ULQ just contains signalling PDU hence check need for insert.
    */
    if ( head != NULL )
    {
      /* Promote signalling PDUs to the front of the queue.
      ** temp = A,B,C,S1,D,S2; outcome temp = S1,S2,A,B,C,D
      */

      /* q_head points to the insertion point.
      ** next_ptr points to behind the front of the queue
      */
      next = q_next(&ULQ(), &head->link);

      while (((qcnt-1) > 0) &&
             (next != NULL)
            )
      {
        /* Promote all signalling PDU types except LAY3_DATA
        */
        if (next->cause == GRR_CAUSE_GMM_SIGNALLING    ||
            next->cause == GRR_CAUSE_LLC_SIGNALLING    ||
            next->cause == GRR_CAUSE_GMM_PAGE_RESPONSE ||
            next->cause == GRR_CAUSE_GMM_CELL_UPDATE   ||
            next->cause == GRR_CAUSE_GMM_CELL_NOTIFY   ||
            next->cause == GRR_CAUSE_SM_SIGNALLING
           )
        {
          temp = next;
          next = q_next(&ULQ(), &next->link);

          MSG_GERAN_MED_1_G("GRLU PDU reordering for signalling PDU - reorder pdu %d",temp->psn);

          q_delete(
                    &ULQ(),
                    &temp->link
                  );
          q_insert(
                    &ULQ(),
                    &temp->link,    /* item to insert */
                    &head->link     /* insertion point */
                  );

          /* new head
          */
          head = q_next (&ULQ(), &temp->link);
          b_reorder  = TRUE;
        }
        else
        {
          next = q_next(&ULQ(), &next->link);
        }

        --qcnt;
      } /* promotion loop */

      if (b_reorder)
      {
        grlc_ul_re_sequence_pdus( psn);
        grlc_clear_realloc_vars();
      }
    }  /* (head |= NULL) */
  } /* if (qcnt > 1) */

} /* grlc_ul_reorder_pdus */

#ifdef  FEATURE_GPRS_PS_HANDOVER

/*==========================================================================
===
===  FUNCTION      GRLC_UL_PROMOTE_SIG_PDUS
===
===  DESCRIPTION
===
===    Sort the PDUs in the main (ULQ) and temp (ULTQ) such that
===    RAUs and Cell-Update are promoted to the front. Ater sorting all
===    PDUs are to reside on the temp queue and the main queueu should be empty
===
===  PARAMS
===
===  DEPENDENCIES
===
===    ul, ULQ, ULTQ
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_promote_sig_pdus (void)
{
  gprs_grr_ul_mes_t *next, *head, *temp;
  uint16            qcnt;

  /*------------------------------------------------------------------------*/

#if 1

  /* Exit if temp queue is empty
  */
  if (q_cnt(&ULTQ()) == 0)
     return;

  /*============================================================================
  ** Assume signalling PDUs may scatter in the main queue and some in the temp
  ** queue. First copy all PDUs in the main queue to the temp queue in time
  ** order, then traverse the temp queue and promote any signalling PDUs to
  ** the front of the temp queue. If there are than one signalling PDUs, they
  ** will be time-ordered
  **============================================================================
  */

  /* Copy all PDUs in ULQ to ULTQ queue in time order.
  ** Main = A,B,C; temp = D
  ** Outcome main = exmpty; temp = A,B,C,D
  */
  qcnt = (uint16)q_cnt(&ULQ());
  if (qcnt)
  {
    temp = q_check(&ULTQ());
    while (qcnt--)
    {
      next = q_get(&ULQ());

      if( next != NULL)
      {
        /* Free the dup that was created when previously enqueued
        */
        if (next->dup_pdu_ptr != NULL)
        {
          GPRS_PDU_FREE(&next->dup_pdu_ptr);
        }

        /* Always insert as temp queue is never empty
        */
        q_insert(
                  &ULTQ(),
                  &next->link,    /* item to insert */
                  &temp->link     /* insertion point */
                );

        temp = q_next(&ULTQ(), &next->link);

        /* Ensure number of PDUs and octets are adjusted, as these only increase
        ** going from temp queue to main queue
        */
        --ul.llc_pdus;
        ul.llc_octets -= (uint32)next->orig_len;
      }
      else
      {
        MSG_GERAN_LOW_0_G("GRLU grlc_ul_promote_sig_pdus failure next=NULL");
      }
    } /* end copy main to temp queue loop */
  }

  /* Promote signalling PDUs to the front of the queue.
  ** temp = A,B,C,S1,D,S2; outcome temp = S1,S2,A,B,C,D
  */
  qcnt = (uint16)q_cnt(&ULTQ());
  if (qcnt)
  {
    /* q_head points to the front of the queue.
    ** next_ptr points to behind the front of the queue
    */
    head = q_check(&ULTQ());
    next = q_next(&ULTQ(), &head->link);

    while ( ((qcnt-1) > 0) &&
            (next != NULL)
          )
    {
      /* Promote all signalling PDU types except LAY3_DATA
      */
      if (next->cause == GRR_CAUSE_GMM_SIGNALLING    ||
          next->cause == GRR_CAUSE_LLC_SIGNALLING    ||
          next->cause == GRR_CAUSE_GMM_PAGE_RESPONSE ||
          next->cause == GRR_CAUSE_GMM_CELL_UPDATE   ||
          next->cause == GRR_CAUSE_GMM_CELL_NOTIFY   ||
          next->cause == GRR_CAUSE_SM_SIGNALLING
         )
      {
        temp = next;
        next = q_next(&ULTQ(), &next->link);

        q_delete(
                  &ULTQ(),
                  &temp->link
                );
        q_insert(
                  &ULTQ(),
                  &temp->link,    /* item to insert */
                  &head->link     /* insertion point */
                );

        /* new head
        */
        head = q_check(&ULTQ());
      }
      else
      {
        next = q_next(&ULTQ(), &next->link);
      }

      --qcnt;
    } /* promotion loop */
  } /* temp queue NOT empty */

#else

  /*==========================================================================
  ** Assume that the main queue contains just data PDUs (if any) and the temp
  ** queue would contain just signalling PDUs. Promote by appending all PDUs
  ** in the main queue to the temp queue
  **==========================================================================
  */

  /* Exit if temp queue is empty
  */
  if (q_cnt(&ULTQ()) == 0)
     return;

  /* Main = A,B,C; temp = S1,S2
  ** Outcome: Main = empty, temp = S1,S2,A,B,C
  */
  next = q_get(&ULQ());
  while (next != NULL)
  {
    /* Free the dup that was created when previously enqueued
    */
    if (next->dup_pdu_ptr != NULL)
    {
      GPRS_PDU_FREE(&next->dup_pdu_ptr);
    }

    /* Ensure number of PDUs and octets are adjusted, as these only increase
    ** going from temp queue to main queue
    */
    --ul.llc_pdus;
    ul.llc_octets -= (uint32)next->orig_len;

    q_put( &ULTQ(), &next->link );
    next = q_get(&ULQ());
  }

#endif

} /* grlc_ul_promote_sig_pdus */

#endif /* FEATURE_GPRS_PS_HANDOVER  */

/*===========================================================================
===
===  FUNCTION      ENQUEUE_PDU
===
===  DESCRIPTION
===
===    Queue up the incoming LLC PDU in FIFO order. All PDUs are subjected to Flow
===    Control checking except for the GMM-procedure related PDUs which come in
===    during RLC Suspend/Resume operation.
===
===    Added FEATURE_RLC_PDU_REORDERING to push a PDU of higher radio priority and
===    different RLC mode compared to the current PDU, to just behind the current
===    PDU. If the current PDU contains a group of other PDUs behind it then the
===    PDU is re-processed and re-grouped accordingly
===
===  PARAMS
===
===    grr_ul_msg_ptr points to LLC message containing info about the LLC PDU
===    and a pointer to the PDU in DSM pool. It also has space for info internal
===    to RLC when RLC processes it.
===
===  DEPENDENCIES
===
===    ul_q
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    CTS_ON/OFF status
===
===========================================================================*/
void enqueue_pdu ( gprs_grr_ul_mes_t *grr_ul_msg_ptr )
{
  uint16  psn = 0;
  uint32  temp_thres = 0L;

  if (grr_ul_msg_ptr != NULL)
  {

    /*------------------------------------------------------------------------
    ** Delete incoming PDUs if ul_static.b_ul_pdu_delete_after_stale_tmr_expiry is
    ** TRUE.
    **------------------------------------------------------------------------
    */

    if (ul_static.b_ul_pdu_delete_after_stale_tmr_expiry == TRUE)
    {
      MSG_GERAN_MED_2_G("GRLU enq delete, pdu len=%u, cause %d",
                 grr_ul_msg_ptr->len, grr_ul_msg_ptr->cause);

      /* free the dsm_ptr if it's not NULL
      */
      if (grr_ul_msg_ptr->pdu_ptr != NULL)
      {
        GPRS_PDU_FREE(&grr_ul_msg_ptr->pdu_ptr);
      }

      /* free the actual LLC PDU message
      */
      GPRS_MEM_FREE(grr_ul_msg_ptr);

      return;
    }

    /* DEL_ALL cause when getting no_ps_access signifies no_service
    ** If RLC get LLC PDU under no service , delete the LLC PDU.
    ** CELL_UPD and PAGE RESP would not be coming under no_service
    ** NAS Signalling wont be coming in no service as GMM is aware of it
    ** SM signalling may or may not be blocked and should not be deleted
    */
    /*
    ** SM Signalling is cell independant and should not be deleted.
    ** During NO_SRVICE in RR ,NAS (GMM) is not aware of it (as feature
    ** UOOS is no longer supported in RR (which used to inform NAS of
    ** NO_SERVICE which prevented any Signalling PDU) and can now
    ** send SM Signalling PDUs which should be processed at RLC.
    */
    if ((ul.no_ps_access_del_pdu_cause == DEL_ALL)     &&
        (grr_ul_msg_ptr->cause != GRR_CAUSE_LAY3_DATA) &&
        (grr_ul_msg_ptr->cause != GRR_CAUSE_SM_SIGNALLING) )
    {
      MSG_GERAN_MED_2_G("GRLU enq delete ALL signalling pdu under no service , pdu len=%u, cause %d",
                                 grr_ul_msg_ptr->len, grr_ul_msg_ptr->cause);

      /* free the dsm_ptr if it's not NULL
      */
      if (grr_ul_msg_ptr->pdu_ptr != NULL)
      {
        GPRS_PDU_FREE(&grr_ul_msg_ptr->pdu_ptr);
      }

      /* free the actual LLC PDU message
      */
      GPRS_MEM_FREE(grr_ul_msg_ptr);

      return;
    }

    /*----------------------------------------------------------------------------
    ** Delete the incoming PDU if PDU is a CELL_UPDATE and there are other PDU(s)
    ** existing in the current queue or reserved queue
    **----------------------------------------------------------------------------
    */
    if
    ( (q_cnt(&ULQ()) || q_cnt(&grlc_ul_old_llc_q)
        /* in psho , we may have only 1 pdu loaded , thus ULQ is empty,
        ** if the pdu is fully acked , then we should not delete cell_update
        */
        ||
        (ul.paq_ctr > 0
#ifdef FEATURE_GPRS_PS_HANDOVER
          &&

          /*
          */
          !( cur_pdu_ptr != NULL &&
             ul.pdu_freeing_state == FREE_CURRENT_PDU
           )
#endif /* FEATURE_GPRS_PS_HANDOVER */
        )
      )   &&
      (
        (grr_ul_msg_ptr->cause == GRR_CAUSE_GMM_CELL_UPDATE) ||
        (grr_ul_msg_ptr->cause == GRR_CAUSE_GMM_CELL_NOTIFY)
      )
    )
    {
      MSG_GERAN_MED_1_G("GRLU enq delete llc pdu len=%u",grr_ul_msg_ptr->len);

      /* free the dsm_ptr if it's not NULL
      */
      if (grr_ul_msg_ptr->pdu_ptr != NULL)
        GPRS_PDU_FREE(&grr_ul_msg_ptr->pdu_ptr);

      /* free the actual LLC PDU message
      */
      GPRS_MEM_FREE(grr_ul_msg_ptr);

      return;
    }

    /*----------------------------------------------------------------------------
    ** Say error and reject pdu if queue is full. Keep this simple condition
    ** based on just llc-octets for host tests
    **----------------------------------------------------------------------------
    */
#if defined(DEBUG_GSM_GPRS_DS_UNIT_TEST)
    #error code not present
#else
    temp_thres = GRLC_DISCARDING_OCTETS;
#endif

    if ( (ul.llc_octets + (uint32)grr_ul_msg_ptr->orig_len) >=
          temp_thres
       )
    {
      MSG_GERAN_LOW_3_G("GRLU enq Q full tbf-mode=%u FCtrl=%u octes=%lu",
                  ul.cur_res.tbf_mode,
                  ul.flow_control_suspend,
                  ul.llc_octets
               );
      /* free the dsm_ptr if it's not NULL
      */
      if (grr_ul_msg_ptr->pdu_ptr != NULL)
        GPRS_PDU_FREE(&grr_ul_msg_ptr->pdu_ptr);

      /* free the actual LLC PDU message
      */
      GPRS_MEM_FREE(grr_ul_msg_ptr);

      return;
    }

    /* If there the is an out of sync between RLC and MAC , RLC could receive
    ** continuously PDU from LLC , which would Q up and cause memory exhasustion
    ** In order to prevent crash and see why we got out of sync , panic reset is
    ** performed. RLC considers total PDU  count = Main Q (ULQ) + PAQ Q.
    */
    /* To traps and delete backlog signalling PDUs when NO_PS_ACCESS is given
    ** (currently trapped where handling of no-ps-access
    */
    if (US_NULL == ul.state)
    {
      grlc_ul_detect_and_flush_backlog_pdus();
    }

    if ( (ul.suspend_state == GRLC_SUSPEND) &&
         ( ( q_cnt(&ULQ()) + ul.paq_ctr )  >= MAX_NUM_SIGNALLING_PDU)
       )
    {
      MSG_GERAN_ERROR_0_G("GRLU ##exceeded max num of MAX_NUM_SIGNALLING_PDU-RLC out of sync");

      /* free the dsm_ptr if it's not NULL
      */
      if (grr_ul_msg_ptr->pdu_ptr != NULL)
        GPRS_PDU_FREE(&grr_ul_msg_ptr->pdu_ptr);

      /* free the actual LLC PDU message
      */
      GPRS_MEM_FREE(grr_ul_msg_ptr);

      ul.b_error_recovery_required = TRUE;
#ifndef FEATURE_GERAN_REDUCED_DEBUG
      ul_diag_stats_st.ul_gen_purpose_count_a++;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
      grlc_ul_show_vars();
      grlc_ul_show_pdus();
      detect_error_and_recover();
      return;
    }

    /*--------------------------------------------------------------------------
    ** Now put some additional info into the PDU structure and put PDU on queue
    **--------------------------------------------------------------------------
    */
    /* Add RLC internal info to the LLC PDU data struct
    */
    if (grr_ul_msg_ptr->prim == GRR_DATA_REQ)
    {
      grr_ul_msg_ptr->rlc_mode = GRLC_MODE_ACK;
    }
    else
    {
      grr_ul_msg_ptr->rlc_mode = GRLC_MODE_UNACK;

      /* additional info for unack mode (for MAC purposes)
      */
      grr_ul_msg_ptr->llc_pdu_type = RM_LLC_PDU_TYPE_NON_ACK_SACK;
    }

    /* Index to remaining of PDU is at the 1st octet
    */
    grr_ul_msg_ptr -> rem_ind = 0;

    /* Never been restored
    */
    grr_ul_msg_ptr->restore_cnt = 0;

    grr_ul_msg_ptr->pdu_ofs = 0;


    /* init pointer to backup PDU
    */
    /* Create the working copy
    */
    grr_ul_msg_ptr->dup_pdu_ptr =
      gprs_pdu_duplicate(grr_ul_msg_ptr->pdu_ptr, 0, grr_ul_msg_ptr->orig_len);
    if (grr_ul_msg_ptr->dup_pdu_ptr == NULL)
    {
      MSG_GERAN_ERROR_0_G("GRLU ##enq dup returns NULL");
      grlc_ul_show_dsm_info();
      grlc_ul_show_vars();

      if (grr_ul_msg_ptr->pdu_ptr != NULL)
        GPRS_PDU_FREE(&grr_ul_msg_ptr->pdu_ptr);

      /* free the actual LLC PDU message
      */
      GPRS_MEM_FREE(grr_ul_msg_ptr);

      /* Failed to create dup'ed packet. Set flag to release at the appro fn level
      */
      ul.b_error_recovery_required = TRUE;
#ifndef FEATURE_GERAN_REDUCED_DEBUG
      ul_diag_stats_st.ul_gen_purpose_count_a++;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

      return;
    }

#ifdef  GSM_GPRS_SHOW_FREE_DUP_IN_ENQUEUE
    grlc_ul_show_dsm_info();
#endif

    /*---------------------------------------------------------------------------
    ** Assess incoming PDU for insertion behind the current PDU (re-ordering) or
    ** simply to append PDU at the end of the queue
    **---------------------------------------------------------------------------
    */
    if (cur_pdu_ptr != NULL &&
        IS_SWAP_REQUIRED(cur_pdu_ptr, grr_ul_msg_ptr) &&
        next_pdu_ptr != NULL
       )
    {
      MSG_GERAN_MED_2_G("GRLU enq SWAP cur=%u last=%u",
          cur_pdu_ptr->psn,
          pg.psn
          );

      /* Clear the grouping info in the current PDU ..
      */
      pg_clr_group_info(cur_pdu_ptr);
      pg_clr_tbc_info( &pg.tbci);

      if (next_pdu_ptr != NULL)
      {
        psn = next_pdu_ptr->psn;
        unput_next_pdu();
      }
      else
      {
        psn = cur_pdu_ptr->psn;
        INC_MOD(psn, 32768);
      }

      /* Put a PSN number on the PDU and increment the running PSN value
      */
      group_pdu_init( grr_ul_msg_ptr);

      /* Insert PDU. Current PDU is still in cur_pdu_ptr. Next PDU is unput. Existing
      ** PDUs in the queue and incoming PDU is grr_ul_msg_ptr
      */
      grlc_ul_insert_pdu( grr_ul_msg_ptr);

      /* Re-sequence the PSNs of PDUs on the queue
      */
      grlc_ul_re_sequence_pdus( (uint16)psn);

      /* reload the next_pdu_ptr
      */
      dequeue_pdu( &next_pdu_ptr);

      if (next_pdu_ptr != NULL)
      {
        MSG_GERAN_LOW_2_G("GRLU enq SWAP next psn=%u qcnt=%u",
          next_pdu_ptr->psn,
          q_cnt(&ULQ())
          );
      }

      ul.llc_pdus++;
      ul.llc_octets += (uint32)grr_ul_msg_ptr -> orig_len;

      grlc_llc_ul_flow_control();

#ifdef  GSM_GPRS_GRLU_LLC_PDU_TYPE
      grlc_ul_show_llc_pdu_type( grr_ul_msg_ptr);
#endif

      if (ul.state == US_ACK_XFER ||
          ul.state == US_UNACK_XFER
         )
      {
        /* Freeze FIFO access for reprocessing
        */
        grlc_ul_fifo_disable_pl1_access();

        reprocess( GRLC_REPROCESS_OTHER);

        /* Re-enable PL1 access to the FIFO
        */
        pl1_send_ph_data_req();
        grlc_ul_fifo_enable_pl1_access();

        MSG_GERAN_LOW_0_G("enq group_pdus");
        group_pdus();
      }
    } /* end swapping PDUs */
    else
    {
      /*------------------------------------------------------------------
      ** Append PDU to the end of the queue (no re-ordering)
      ** Put a PSN number on the PDU and increment the running PSN value.
      ** Also init the PDU group status to start the process off
      **------------------------------------------------------------------
      */
      group_pdu_init( grr_ul_msg_ptr);

      ul.llc_pdus++;
      ul.llc_octets += (uint32)grr_ul_msg_ptr -> orig_len;

      grlc_llc_ul_flow_control();

      (void)q_link(grr_ul_msg_ptr, &grr_ul_msg_ptr->link);
      q_put(&ULQ(), &grr_ul_msg_ptr->link);

      MSG_GERAN_MED_3_G("GRLU enq psn=%u len=%u NpOctets=%08x",
        grr_ul_msg_ptr->psn,
        grr_ul_msg_ptr->len,
        grlc_ul_pack_queue_info());

#ifdef  GSM_GPRS_GRLU_LLC_PDU_TYPE
      grlc_ul_show_llc_pdu_type( grr_ul_msg_ptr);
#endif

      if (ul.state == US_ACK_XFER ||
          ul.state == US_UNACK_XFER
         )
      {
        MSG_GERAN_LOW_0_G("enq group_pdus");
        group_pdus();
      }
    } /* end append PDU */


    /* Debug info collection */

    if (ul.suspend_state == GRLC_SUSPEND)
    {
      ul.susp_state_data_pdus++;

      MSG_GERAN_HIGH_2_G("GRLU susp state data cnt %d, cause %d", ul.susp_state_data_pdus, grr_ul_msg_ptr->cause );
    }

    MSG_GERAN_LOW_2_G("GRLU enq LLC PDU PFI=%d, PFC=%d",
      grr_ul_msg_ptr->pfi, IS_PFC_NW_SUPPORTED());
    /*------------------------------------------------------------------
    ** Always set the signal
    **------------------------------------------------------------------
    */
    RLC_UL_SET_LLC_PDU_SIG();
  }
  else
  {
    MSG_GERAN_LOW_0_G("GRLU NULL pdu ptr received");
  }
} /* enqueue_pdu */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_DETECT_AND_FLUSH_BACKLOG_PDUS
===
===  DESCRIPTION
===    If Commercial Recovery NV is NOT enabled then if the maximum signalling
===    PDU threshold is about to be reached then delete all PDUs except one or
===    newest PDUs
===
===  PARAMS
===
===    ptr points to the rlc ul pdu struct
===
===  DEPENDENCIES
===
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===    CTS_ON/OFF status
===
===========================================================================*/
void grlc_ul_detect_and_flush_backlog_pdus (void)
{
  gprs_grr_ul_mes_t *pdu_ptr;

  /* Check that Commercial Recovery NV is DISABLED, ie. pre-cs testing only
  ** then if the maximum signalling PDUs is about to exceed limit the delete all
  ** PDUs except the last PDU or two..
  */
  if( !geran_get_nv_recovery_restart_enabled() &&
      (ul.suspend_state == GRLC_SUSPEND) &&
      ( ( q_cnt(&ULQ()) + ul.paq_ctr )  >= (MAX_NUM_BACKLOG_SIGNALLING_PDUS))
    )
  {
    MSG_HIGH("GRLU det-flush state[%u]=%u", ul.state, 0, 0);

    /* return all PDUs to queue
    */
    grlc_ul_unput_paq();

    /* Load the latest PDU and free to PDU behind the current
    */
    (void)load_cur_pdu();
    if (NULL != cur_pdu_ptr)
    {
      /* now there is 1 newest PDU in the PAQ and the rest of the PDUs in the queue(s)
      */
      MSG_MED("GRLU Info Flush temp queue ULTQ=%u",q_cnt(&ULTQ()),0,0);

      /*-----------------------------------------------
      ** Free LLC PDUs in the temporary LLC PDU queue
      **-----------------------------------------------
      */
      pdu_ptr = (gprs_grr_ul_mes_t *)q_get(&ULTQ());
      while ( pdu_ptr != NULL )
      {
        if (pdu_ptr->dup_pdu_ptr != NULL)
        {
          /* If LLC had not blanked the message then make this NULL now.
          ** dup_dsm_ptr is only intialised/processed in enqueue() later on
          */
          pdu_ptr->dup_pdu_ptr = NULL;
        }

        if (pdu_ptr->pdu_ptr != NULL)
        {
          GPRS_PDU_FREE(&pdu_ptr->pdu_ptr);
        }
        /* now free the memory holding the LLC PDU message
        */
        GPRS_MEM_FREE(pdu_ptr);

        pdu_ptr = (gprs_grr_ul_mes_t *)q_get(&ULTQ());
      }

      /* Look at what's in the queue
      */
      pdu_ptr = NULL;
      dequeue_pdu( &pdu_ptr);

      while (pdu_ptr != NULL)
      {
        free_llc_pdu_and_msg(pdu_ptr);
        dequeue_pdu( &pdu_ptr);
      }

      /* next_pdu_ptr should be clear but does not matter if not
      */

      /* Flush the reserve Q */
      while (q_cnt(&grlc_ul_old_llc_q) > 0)
      {
        /* Flush the reserved Q */
        pdu_ptr = (gprs_grr_ul_mes_t *)q_get(&grlc_ul_old_llc_q);

        if ( pdu_ptr != NULL )
        {
          if (pdu_ptr->dup_pdu_ptr != NULL)
          {
            GPRS_PDU_FREE(&pdu_ptr->dup_pdu_ptr);
          }

          if (pdu_ptr->pdu_ptr != NULL)
          {
            GPRS_PDU_FREE(&pdu_ptr->pdu_ptr);
          }
          /* now free the memory holding the LLC PDU message
          */
          GPRS_MEM_FREE(pdu_ptr);
        }
      }

      /* Put back the only PDU into the queue
      */
      grlc_ul_unput_paq();

      /* Clear UL realloc variables.
      */
      grlc_clear_realloc_vars();
#ifdef  GSM_GPRS_GRLU_SHOW_PAQ
      grlc_ul_show_paq();
#endif
    } /* if there are PDUs to flush */
  } /* panic reset NOT enabled and .. */
} /* grlc_ul_detect_and_flush_backlog_pdus */

/*===========================================================================
===
===  FUNCTION      DEQUEUE_PDU
===
===  DESCRIPTION
===
===    Takes an llc pdu at the front of the queue and return its address if
===    one exists, otherwise return NULL.
===
===    This function checks the RLC UL data queue capacity and resumes
===    LLC data flow if near empty.
===
===  PARAMS
===
===    ptr points to the rlc ul pdu struct
===
===  DEPENDENCIES
===
===    ul_q
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===    CTS_ON/OFF status
===
===========================================================================*/
void dequeue_pdu (gprs_grr_ul_mes_t **pptr)
{
  /* return if queue is empty
  */
  if (q_cnt(&ULQ()) == 0)
  {
    *pptr = NULL;
    return;
  }

  /* remove message from the queue - Note the ul.llc_octets is not adjusted
  ** as this PDU has not been acknowledged as delivered.
  */
  *pptr = (gprs_grr_ul_mes_t *)q_get(&ULQ());

} /* dequeue_pdu */

/*===========================================================================
===
===  FUNCTION      FREE_LLC_PDU_AND_MSG
===
===  DESCRIPTION
===
===     Free the PDU payload, dup payload and the memory carrying the PDU (msg)
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void free_llc_pdu_and_msg(gprs_grr_ul_mes_t *msg_ptr)
{

  if (msg_ptr == NULL)
  {
    MSG_GERAN_LOW_0("GRLU free PM msg NULL");
    return;
  }


  /* llc pdu sent, so reduce the number of llc pdu octets
  */
  if ( ul.llc_pdus != 0 )
  {
    ul.llc_pdus -= 1;
#ifndef FEATURE_GERAN_REDUCED_DEBUG
    if (msg_ptr->rlc_mode == GRLC_MODE_ACK)
    {
      ul_diag_stats_st.ack_tx_pdu_cnt++;
      ul_diag_stats_st.ack_tx_oct_cnt += msg_ptr->orig_len;
    }
    else
    {
      ul_diag_stats_st.unack_tx_pdu_cnt++;
      ul_diag_stats_st.unack_tx_oct_cnt += msg_ptr->orig_len;
    }
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
  }

  /* Use 'orig_len' instead of 'len' because TLLI handling or changing coding
  ** scheme may have occured which reduces value of 'len'
  */
  if ( ul.llc_octets >= (uint32)msg_ptr->orig_len )
    ul.llc_octets -= (uint32)msg_ptr->orig_len;
  else
    ul.llc_octets = 0;

  grlc_llc_ul_flow_control();

  /* Subtract the 'same_type' vars
  */
  ungroup_pdu( msg_ptr);

  /* free the remainder of the working copy
  */
  if (msg_ptr->dup_pdu_ptr != NULL)
  {
    GPRS_PDU_FREE(&msg_ptr->dup_pdu_ptr);
  }

  /* free the original
  */
  if (msg_ptr->pdu_ptr != NULL)
  {
    GPRS_PDU_FREE(&msg_ptr->pdu_ptr);
  }

#if defined(GSM_GPRS_GRLU_SHOW_PAQ) || defined(GSM_GPRS_GRLU_SHOW_FREE_PDU)
  MSG_GERAN_MED_3_G("GRLU Pfree %u PM NpOctets=%08x small=%u",
    msg_ptr->psn,
    grlc_ul_pack_queue_info(),
    DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL));

  grlc_ul_show_dsm_info();
#endif

  /* now free the memory holding the LLC PDU message
  */
  GPRS_MEM_FREE(msg_ptr);

} /* free_llc_pdu_and_msg */


