/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC UPLINK PENDING ACKNOWLEDGEMENT QUEUE
***
***
*** DESCRIPTION
***
***  The (PDU) Pending Acknowledgement Queue, or simply, PAQ is an array of
***  pointers which keeps tracks of the PDUs in transfer. When a PDU is taken
***  out of the main queue for transfer, its pointer is put on the PAQ. When
***  the NW acknowledges all the data blocks in a PDU, that PDU is deleted
***  from the PAQ (oldest entry). When an exception occurs, transfer resumes
***  from the point of the last un-acknowledged PDU (oldest PAQ entry).
***
***  This module performs all the simple queue management functions, as well
***  as other supporting PDU management functions
***
*** EXTERNALIZED FUNCTIONS
***
***  void grlc_ul_init_paq (void)
***  void grlc_ul_put_paq (  gprs_grr_ul_mes_t *msg_ptr )
***  void grlc_ul_unput_paq (void)
***  void grlc_ul_paq_make_psn_current (  uint16 psn, uint16 offset)
***  boolean grlc_ul_paq_does_psn_exist (  uint16 psn)
***  uint16 grlc_ul_paq_seek_psn_cv0 (  uint16 bsn)
***  uint16 grlc_ul_paq_seek_psn (uint16 vs)
***  void grlc_ul_del_free_paq_upto_psn (  uint16 psn,boolean b_ul_psn_txed)
***  !
***  void shorten_cur_pdu (uint16 offset)
***  void unput_pdu (gprs_grr_ul_mes_t *msg_ptr)
***  void unput_next_pdu (void)
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
*** $Header: //components/rel/geran.mpss/5.2.0/grlc/src/grlcpaq.c#4 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 11/25/03   hv      Created
*** 08/03/04   hv      Added EGPRS code
*** 20/08/13   hv      CR531564
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
#include "grlcpaq.h"
#include "geran_msgs.h"
#include "geran_multi_sim.h"
#include "geran_dual_sim_g.h"

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GRLC_UL_CLR_PAQ
===
===  DESCRIPTION
===
===     Clear the PAQ leaving the max counter intact
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
static void grlc_ul_clr_paq (void)
{
  uint16  u;

  ul.paq_in = 0;
  ul.paq_out = 0;
  ul.paq_ctr = 0;
  for (u=0; u<GRLC_UL_MAX_PAQ_SIZE; u++)
    ul.paq_ptr[u] = NULL;
} /* grlc_ul_clr_paq */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_INIT_PAQ
===
===  DESCRIPTION
===
===    Init the Pending-acknowledgement-queue
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_init_paq (void)
{
  grlc_ul_clr_paq();
  ul.paq_ctr_max = 0;
}

/*===========================================================================
===
===  FUNCTION      GRLC_UL_PUT_PAQ
===
===  DESCRIPTION
===
===    Put the pointer of the LLC PDU message just loaded into the PAQ
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_put_paq ( gprs_grr_ul_mes_t *msg_ptr )
{
  if (msg_ptr != NULL)
  {
    if (ul.paq_ctr < GRLC_UL_MAX_PAQ_SIZE)
    {
#ifdef  GSM_GPRS_GRLU_SHOW_PAQ
      MSG_GERAN_MED_1_G("GRLU put %u",msg_ptr->psn);
#endif
      ul.paq_ptr[ul.paq_in] = msg_ptr;
      INC_MOD(ul.paq_in, GRLC_UL_MAX_PAQ_SIZE);
      ul.paq_ctr++;

      /* Latch the max value
      */
      if (ul.paq_ctr > ul.paq_ctr_max)
        ul.paq_ctr_max = ul.paq_ctr;
    }
    else
    {
      MSG_GERAN_ERROR_3_G("GRLU put_paq full 0CtrMxCt=%08x in=%d out=%d, triggering panic reset",
                ((uint32)ul.paq_ctr << 16) |
                ((uint32)ul.paq_ctr_max & 0xffff),
                ul.paq_in,
                ul.paq_out);

      grlc_ul_show_vars();

      ul.state = US_AWAIT_RECOVERY;

      /* If PANIC RESET RESTART is enabled perform soft recovery otherwise
      ** do an ERR_FATAL to stop where it occurs.
      */
      if( geran_get_nv_recovery_restart_enabled())
      {
        /* Invoke Panic Reset
        */
        geran_initiate_recovery_restart(GERAN_CLIENT_GRLC);
      }
      else
      {
        ERR_GERAN_FATAL_1_G("GRLU GRLC_UL_MAX_PAQ_SIZE %d exceeded", GRLC_UL_MAX_PAQ_SIZE);
      }
    }

  }
  else
  {
    MSG_GERAN_LOW_0_G("GRLU put_paq NULL");
  }
#ifdef  GSM_GPRS_GRLU_SHOW_PAQ
  grlc_ul_show_paq();
#endif
} /* grlc_ul_put_paq */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_PAQ_MAKE_PSN_CURRENT
===
===  DESCRIPTION
===
===     Make the PSN the current PDU with the given offset. Expect the given
===     PDU is present in the PAQ
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_paq_make_psn_current (uint16 psn, uint16 offset)
{
  gprs_grr_ul_mes_t *temp_ptr;
  uint16            ind, temp_psn;

  if ( (cur_pdu_ptr != NULL) && (psn == cur_pdu_ptr->psn))
  {
    /* PSN is current means the PDU is the latest item pushed onto PAQ. Here
    ** we can simply restore the current PDU to the given offset.
    */

#ifdef  GSM_GPRS_GRLU_SHOW_PAQ
    MSG_GERAN_MED_3_G("GRLU make_cur psn=%u ofs=%u cur_p=%u",psn,offset,cur_pdu_ptr->psn);
#endif

    /* Warn if offset is greater or same as the remaining PDU length
    */
    if (cur_pdu_ptr->orig_len <= offset)
    {
      /* pdu is at the end. Do nothing
      */
      MSG_GERAN_HIGH_3_G("GRLU ##make_cur psn=%u len(%u) <= offset(%u)",
                psn,
                cur_pdu_ptr->len,
                offset);
    }
    else
    {
      shorten_cur_pdu( offset);
    }
  }
  else
  {
    if (grlc_ul_paq_does_psn_exist( psn))
    {
      /* eg. PAQ holds a0,a1,a2,a3 where cur_pdu is a3 and PSN points at a1.
      ** We need to restore a2 and a3 and push them back on the queue. we also
      ** need to load a1 into cur_pdu_ptr. When we finish PAQ should hold a0 and a1
      */

      /* ind indexes the latest item in PAQ (also in cur_pdu_ptr).
      ** Access the items in PAQ in reverse order starting with the latest.
      ** Restore and unput the PDU until the PDU with the given PSN is met.
      */
      ind = (ul.paq_in + GRLC_UL_MAX_PAQ_SIZE - 1) % GRLC_UL_MAX_PAQ_SIZE;
      do
      {
        temp_ptr = ul.paq_ptr[ind];
        temp_psn = temp_ptr->psn;

        if (psn != temp_psn)
        {
          /* Ignore the item in cur_pdu_ptr as a copy is held in PAQ anyway.
          ** Take items in PAQ and restore using cur_pdu_ptr, then unput the PDU.
          */
          cur_pdu_ptr = temp_ptr;

          /* free wokring copy
          */
          if (cur_pdu_ptr->dup_pdu_ptr != NULL)
          {
            GPRS_PDU_FREE(&cur_pdu_ptr->dup_pdu_ptr);
          }

          /* create working copy
          */
          cur_pdu_ptr->len = (uint16)GPRS_PDU_LENGTH(cur_pdu_ptr->pdu_ptr);

          if (cur_pdu_ptr->len != 0)
          {
            cur_pdu_ptr->dup_pdu_ptr =
              gprs_pdu_duplicate(cur_pdu_ptr->pdu_ptr, 0, cur_pdu_ptr->len);
            if (cur_pdu_ptr->dup_pdu_ptr == NULL)
            {
              MSG_GERAN_ERROR_0_G("GRLU ##make_cur dup returns NULL");
              grlc_ul_show_dsm_info();
              grlc_ul_show_vars();

              /* Failed to create dup packet. Set flag to release at the appro fn level
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
            MSG_GERAN_HIGH_1_G("GRLU ## make_cur len=0. psn=%u",cur_pdu_ptr->psn);
          }
          cur_pdu_ptr->rem_ind = 0;
          cur_pdu_ptr->pdu_ofs = 0;

#ifdef  GSM_GPRS_GRLU_SHOW_PAQ
          MSG_GERAN_MED_1_G("GRLU make_cur unput %u",temp_psn);
#endif
          /* Put back PDU onto the queue via next_pdu_ptr
          */
          unput_next_pdu();
          next_pdu_ptr = cur_pdu_ptr;
          cur_pdu_ptr = NULL;

          /* take item out of PAQ
          */
          ul.paq_ctr--;

          /* new in index is at ind
          */
          ul.paq_in = ind;

          /* clear pointer of unput PDU for checking purposes later on
          */
          ul.paq_ptr[ind] = NULL;

          /* New running index
          */
          ind = (ind + GRLC_UL_MAX_PAQ_SIZE - 1) % GRLC_UL_MAX_PAQ_SIZE;
        }
        else
        {
          /* item is PSN so restore item to the given offset and put it in cur_pdu_ptr
          */
          cur_pdu_ptr = temp_ptr;
          if (cur_pdu_ptr->orig_len > offset)
          {
            shorten_cur_pdu( offset);
          }
          else
          {
            MSG_GERAN_HIGH_3_G("GRLU ##make_cur psn=%u len(%u) <= offset(%u)",
                      cur_pdu_ptr->psn,
                      cur_pdu_ptr->len,
                      offset);
          }

          break;
        }
      } while (ul.paq_ctr > 0);
    } /* psn exists */
    else
    {
      if (cur_pdu_ptr != NULL)
      {
        MSG_GERAN_MED_2_G("GRLU ##make_cur %u not in PAQ cur_psn=%u",
                psn,
                cur_pdu_ptr->psn
                );
      }

      /* PSN given is not in PAQ, eg. a0,a1,a2,a3 are in PAQ. cur_psn is a3, psn is a4.
      ** Hence psn is not in PAQ. We should simply invoke load_cur_pdu, to:
      ** - Move next_pdu_ptr or top PDU from LLC queue into cur_pdu_ptr
      ** - Then, put the new cur_pdu_ptr into the PAQ
      ** First NULLify cur_pdu_ptr to force load_cur_pdu(). cur_pdu_ptr is in the PAQ so
      ** we have not lost it
      */
      cur_pdu_ptr = NULL;

      (void)load_cur_pdu();
    } /* PSN in PAQ */

  } /* psn in not current PDU */

} /* grlc_ul_paq_make_psn_current */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_UNPUT_PAQ
===
===  DESCRIPTION
===
===    Restore and unput all PDUs in the PAQ into the main LLC UL Q.
===    next_pdu_ptr is unput as it is not held in the PAQ.
===    - unput next_pdu_ptr
===    - restore (if required) and unput PDUs in the PAQ in reverse order
===    - clear the PAQ
===    - clear cur_pdu_ptr if it points to a PDU among the paq queue
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_unput_paq (void)
{
  gprs_grr_ul_mes_t *temp_ptr;
  uint16            u, ind, temp_psn, temp_len;

  /*-----------------------------------------------------------------------*/

  /* Unput next_pdu_ptr
  */
  unput_next_pdu();

  /* current PDU is already held in the PAQ. After all the PDUs are unput
  ** we can make cur_pdu_ptr NULL; without freeing any data.
  */

  /* Restore and unput each PDUs in the PAQ in reverse order
  */
  ind = ul.paq_in;
  for (u=0; u<ul.paq_ctr; u++)
  {
    ind = (ind + GRLC_UL_MAX_PAQ_SIZE - 1) % GRLC_UL_MAX_PAQ_SIZE;
    temp_ptr = ul.paq_ptr[ind];
    /* clear pointer of PDU about to be unput
    */
    ul.paq_ptr[ind] = NULL;

    temp_psn = temp_ptr->psn;

    /* free the remainder of the working copy
    */
    if (temp_ptr->dup_pdu_ptr != NULL)
    {
      GPRS_PDU_FREE(&temp_ptr->dup_pdu_ptr);
    }


    /* Restore the working copy from the original
    */
    if (temp_ptr->pdu_ptr != NULL)
    {
      /* Re-create the current pdu by making a duplicate using its backup
      ** with the offset just calculated
      */
      temp_len = (uint16)GPRS_PDU_LENGTH(temp_ptr->pdu_ptr);
      temp_ptr->dup_pdu_ptr = gprs_pdu_duplicate(temp_ptr->pdu_ptr, 0, temp_len);
      temp_ptr->pdu_ofs = 0;
      if (temp_ptr->dup_pdu_ptr == NULL)
      {
        MSG_GERAN_ERROR_0_G("GRLU ##unput_paq dup returns NULL");
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

      temp_ptr->len = temp_len;
      temp_ptr->rem_ind = 0;
      unput_pdu( temp_ptr);

      /* If cur_pdu_ptr points to a PDU that is being unput then clear cur_pdu_ptr
      */
      if (cur_pdu_ptr != NULL)
      {
        if (temp_psn == cur_pdu_ptr->psn)
        {
          MSG_GERAN_LOW_0_G("GRLU unput_paq clrs cur_pdu_ptr as well");
          cur_pdu_ptr = NULL;
        }
      }
    }
    else
    {
      MSG_GERAN_HIGH_3_G("GRLU ##unput_paq dup ptr NULL psn=%u dup=%x ptr=%x",
                temp_ptr->psn,
                temp_ptr->dup_pdu_ptr,
                temp_ptr->pdu_ptr);
    }
  }

  /* Clear PAQ after taking items out in reverse order
  */
  grlc_ul_clr_paq();

} /* grlc_ul_unput_paq */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_PAQ_DOES_PSN_EXIST
===
===  DESCRIPTION
===
===     Search and return TRUE if PSN exists in the PAQ
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===     boolean
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean grlc_ul_paq_does_psn_exist (uint16 psn)
{
  uint16  ind, ctr;

  ctr = ul.paq_ctr;
  ind = ul.paq_out;

  while (ctr > 0)
  {
    if (psn == (ul.paq_ptr[ind])->psn)
      return TRUE;
    else
    {
      INC_MOD(ind, GRLC_UL_MAX_PAQ_SIZE);
      ctr--;
    }
  }

#ifdef  GSM_GPRS_GRLU_SHOW_PAQ
  MSG_GERAN_LOW_3_G("GRLU psn %u not in PAQ ctr=%u cur_p=%u",psn,ul.paq_ctr,cur_pdu_ptr->psn);
#endif

  return FALSE;

} /* grlc_ul_paq_does_psn_exist */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_DEL_FREE_PAQ_UPTO_PSN
===
===  DESCRIPTION
===
===     Free PDUs and messages in PAQ upto and including the LL PDU with the
===     given PSN value. If cur_pdu_ptr points to a PDU to be freed then
===     cur_pdu_ptr is set to NULL as that PDU is freed.
===
===     When b_ul_psn_txed = TRUE, call gllc_grr_ul_transmit_ind() to indicate
===     that UL PDU transmission is completed.
===
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_del_free_paq_upto_psn (uint16 psn, boolean b_ul_psn_txed)
{
  gprs_grr_ul_mes_t *paq_pdu_ptr;
  uint16            paq_psn = 0;


  /* If PAQ contains item with given 'psn' then loop and delete and free items
  ** upto and including the item 'psn'.
  ** Terminate loop when count reaches 0 or after item with PSN is deleted and
  ** freed.
  */
  if (grlc_ul_paq_does_psn_exist( psn))
  {
    while (ul.paq_ctr > 0)
    {
      paq_pdu_ptr = ul.paq_ptr[ul.paq_out];
      ul.paq_ptr[ul.paq_out] = NULL;

      paq_psn = paq_pdu_ptr->psn;

      INC_MOD(ul.paq_out, GRLC_UL_MAX_PAQ_SIZE);
      ul.paq_ctr--;

      /* If cur_pdu_ptr points to the same PDU then make it NULL -- BEFORE
      ** freeing the msg
      */
      if (cur_pdu_ptr != NULL)
      {
        if (paq_psn == cur_pdu_ptr->psn)
        {
          cur_pdu_ptr = NULL;
        }
      }


      /*
      ** Notify upper layer transmission status of PDU,
      ** check for cur_pdu_ptr to send only one status_ind feedback
      */
      if( (b_ul_psn_txed       == TRUE) &&
          ((paq_pdu_ptr->cause == GRR_CAUSE_GMM_SIGNALLING) ||
           (paq_pdu_ptr->cause == GRR_CAUSE_SM_SIGNALLING)
          )
        )
      {
        indicate_to_llc_rlcmac_success( paq_pdu_ptr);
      }

      /* Indicate the completion of PDU transmission.
      */
      if (b_ul_psn_txed == TRUE)
      {
#if  (!((defined (DEBUG_GSM_EGPRS_DS_UNIT_TEST)) || (defined (PERLUTF))))
        gllc_grr_ul_transmit_ind( 
                                  paq_pdu_ptr-> sapi,
                                  paq_pdu_ptr-> reliability_class,
                                  &paq_pdu_ptr->sndcp_ref,
                                  paq_pdu_ptr-> cause
                                );
#else
        #error code not present
#endif /* DEBUG_GSM_EGPRS_DS_UNIT_TEST/PERLUTF */
      }

      free_llc_pdu_and_msg(paq_pdu_ptr);

      MSG_GERAN_MED_1_G("freeing llc pdu with psn=%d",paq_psn);

      /* terminate loop when PDU with given PSN has been deleted
      */
      if (paq_psn == psn)
      {
        break;
      }
    }
  }

  /* IN EXT UL TBF , RLC retains the last PDU even though acked
  ** by the NW. RLC needs to inform Tx Completion to LLC.
  */
  if ( ( cur_pdu_ptr != NULL                          ) &&
       ( ul.pdu_freeing_state == FREE_CURRENT_PDU     ) &&
       ( cur_pdu_ptr->psn == ul.psn_to_free_inclusive )
     )
  {
    /* Indicate PDU transmission completion to LLC */

#if  (!((defined (DEBUG_GSM_EGPRS_DS_UNIT_TEST)) || (defined (PERLUTF))))
    gllc_grr_ul_transmit_ind( 
                              cur_pdu_ptr-> sapi,
                              cur_pdu_ptr-> reliability_class,
                              &cur_pdu_ptr->sndcp_ref,
                              cur_pdu_ptr-> cause
                            );
#else
    #error code not present
#endif /* DEBUG_GSM_EGPRS_DS_UNIT_TEST/PERLUTF */

    /*
    ** Notify upper layer transmission status of PDU
    */
    if( (cur_pdu_ptr->cause == GRR_CAUSE_GMM_SIGNALLING) ||
        (cur_pdu_ptr->cause == GRR_CAUSE_SM_SIGNALLING)
      )
    {
      indicate_to_llc_rlcmac_success( cur_pdu_ptr);
    }
  }
} /* grlc_ul_del_free_paq_upto_psn */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_PAQ_SEEK_PSN_CV0
===
===  DESCRIPTION
===
===    Seek the PSN of the last PDU that was acknowledged as received by the
===    NW when FA=1. Since we can't use the bitmap value when FA=1 is received.
===    We need to find the last block of the TBF (when CV=0) to identify the
===    PSN of the PDU
===
===  PARAMS
===
===    bsn: indicates the BSN where BSN-1 is the starts of the search for the
===         block with CV=0 -- backwards
===
===    Expect 'ul.vs' to be passed in in all cases except when PUAN is received
===    in GPRS mode EXT_UTBF where the passed in value should be VA; following
===    an update of the RRB/SSN
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
uint16 grlc_ul_paq_seek_psn_cv0 (uint16 bsn)
{
  uint16  ind, b;
  uint16  ret_psn;

  /* If search failed return the previous PSN
  */
  ret_psn = (cur_pdu_ptr->psn - 1) % 32768;

  /* Search from VSF backward to find block with CV=0. seg_ind is not used as
  ** block must be at least acknowledged by PL1 as having been transmitted.
  ** The max search distance is half of SNS/ESNS
  */

  /* Start from bsn. The real start in the loop below is at bsn-1
  ** (Use VS for both ack mode and unack mode)
  */
  ind = bsn;

#ifdef  GSM_GPRS_GRLU_SHOW_PAQ
  MSG_GERAN_MED_3_G("GRLU seek vs=%d vsf=%d p_vsf=%d",ul.vs,ul.vsf,ul.pack_vsf);
#endif

  if (ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
  {
    for (b=0; b<ul.cur_res.ws/2; b++)
    {
      ind = MOD_ESNS(ind+ESNS-1);

      if (TX_ARR( ind).blk.hdr.cv == 0)
      {
        ret_psn = TX_ARR( ind).blk.last_psn;
#ifdef  GSM_GPRS_GRLU_SHOW_PAQ
        MSG_GERAN_MED_3_G("ERLU seek_cv0 va=%d ind=%d psn=%u",ul.va,ind,ret_psn);
#endif
        break;
      }
    }
  }
  else
  {
    for (b=0; b<SNS/2; b++)
    {
      ind = ((ind + SNS) - 1) % SNS;
      if (TX_ARR( ind).blk.mac_hdr.type.ul_data.cv == 0)
      {
        ret_psn = TX_ARR( ind).blk.last_psn;
#ifdef  GSM_GPRS_GRLU_SHOW_PAQ
        MSG_GERAN_MED_3_G("GRLU seek_cv0 va=%d ind=%d psn=%u",ul.va,ind,ret_psn);
#endif
        break;
      }
    }
  }

  /* Warn if the returned PSN is not the current PDU
  */
  if (ret_psn != cur_pdu_ptr->psn)
  {
    MSG_GERAN_MED_2_G("GRLU ##seek ret_psn=%u cur_p=%u",ret_psn,cur_pdu_ptr->psn);
  }

  return ret_psn;

} /* grlc_ul_paq_seek_psn_cv0 */


