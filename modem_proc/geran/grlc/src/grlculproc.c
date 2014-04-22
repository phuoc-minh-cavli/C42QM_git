/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC UPLINK PROCESSING
***
***
*** DESCRIPTION
***
***  This module performs the following processing:
***    - RLC queue
***    - Resource request and reallocation
***    - The Pending-ack queue
***    - Processing PUAN
***    - GPRS suspension and resumption
***    - Normal and abnormal releases
***    - Re-processing of RLC transmit buffer following:
***       . Change of coding scheme
***       . Change of number of timeslots or TFI
***       . Removal of TLLI following contention resolution
***
***
*** EXTERNALIZED FUNCTIONS
***
***  void grlc_ul_init_grps_data_block (grlc_ul_data_block_t *bp)
***  void grlc_ul_init_egprs_data_block (grlc_ul_data_block_t *bp)
***  boolean grlc_is_xfer_active (void)
***  void grlc_ul_data_detect (boolean b)
***  void clr_ul_data (void)
***  void read_ul_sysinfo (void)
***  void dequeue_pdu (gprs_grr_ul_mes_t **pptr)
***  gprs_grr_ul_mes_t *load_cur_pdu (void)
***  void update_ack_state_array (void)
***  void handle_contention_resolution_failure (void)
***  void handle_contention_resolution_success (mac_rlc_msg_t  *mac_msg_ptr)
***  void change_coding_scheme (const uint8 cs)
***  realloc_action_t  determine_realloc_action (void)
***  void grr_suspend_req (void)
***  void grr_purge_suspended_data_req (void)
***  void grr_purge_data_req (uint8 sapi)
***  void grr_resume_req (void)
***  void grr_test_mode_req(void)
***  void accept_resource (void)
***  void realloc_for_next_pdu (boolean   b_cv_zero)
***  void est_tbf_from_null (void)
***  void indicate_to_llc_rlcmac_failure (const gprs_grr_ul_mes_t *pdu_ptr)
***  void handle_ul_release_event (rm_tbf_release_t ul_release_cause,boolean b_from_mac)
***  void flush_llc_pdu_queue (void)
***  void detect_error_and_recover (void)
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
*** $Header: //components/rel/geran.mpss/5.2.0/grlc/src/grlculproc.c#7 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 30/09/13   hv      CR550639
*** 20/08/13   hv      CR531564
*** 04/07/13   hv      CR504756 - PDU purge in xfer leading to NULL pointer exception
*** 17/07/13   hv      CR417915
*** 04/07/13   hv      CR506277 - Issue with reallocation whilst in RLC un-ack mode
*** 08/02/13   hv      CR447071 - RLC per-tbf memory allocation
*** 10/04/13   hv      CR472248 - Issue with reallocation in change of RLC mode
*** 05/07/11   hv      Added promoting sig PDUs to the front of uplink queue
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
#include "gmutex.h"
#include "queue.h"
#include "grlcllc.h"
#include "grlcultst.h"
#include "grlctx.h"
#include "grlcsgm.h"
#include "grlculproc.h"
#include "geran_msgs.h"

#include "grlci.h"
#include "grlct4decode.h"
#include "grlcpaq.h"
#include "grlculpdu.h"
#include "grlcultypes.h"
#include "grlc_dyn_api.h"

#include "geran_eng_mode_info.h"
#include "geran_tasks.h"
#include "geran_dual_sim_g.h"
#if defined(TEST_FRAMEWORK)
#error code not present
#endif // TEST_FRAMEWORK


/*---------------------------------------------------------------
** Critical section to protect access to GRLC UL FIFO
**---------------------------------------------------------------
*/

extern rex_crit_sect_type grlc_ul_fifo_crit_sec;


/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/

/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/

/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/

/*****************************************************************************
***
***     Module Data Declarations
***
*****************************************************************************/

/*----------------------------------------------------------------------------
** Pointers outside the PDU queue to hold the current pdu being worked on and
** on and the next pdu
**----------------------------------------------------------------------------
*/
gprs_grr_ul_mes_t   *cur_pdu_ptr, *next_pdu_ptr;

#ifndef FEATURE_GERAN_REDUCED_DEBUG
/* UL Diagnostic Data
*/
grlc_diag_ul_statistics_t                ul_diag_stats_st;
grlc_diag_ul_abnormal_release_counts_t   ul_diag_abn_rel_cnts_st;
grlc_diag_ul_event_counts_t              ul_diag_evt_cnts_st;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

/* Counter to detect "rlc no data" condition
*/
uint16 grlc_ul_no_data_ctr = {0};

/*****************************************************************************
***
***     Public Data Declarations
***
*****************************************************************************/

/*-----------------------------------------------------------------------
** RLC data block size in octets without the MAC header, with RLC header
** and payload according to coding scheme
**-----------------------------------------------------------------------
*/
const uint8 GRLC_DATA_BLOCK_LEN[] = { 22, 32, 38, 52 };

/*-----------------------------------------------------------------------------------
** EGPRS RLC data block size in octets without the header. These values specify
** the length of each payload without the extra octet for the FBI/E and TI/E fields.
** For MCS-7/8/9 the total data unit length for both payloads should be doubled
** of the values stated below
**
** To re-iterate, the octets below do not include the FBI/E and TI/E octets.
**
** The array should be indexed by the value of the MCS within the range from MCS-1(0)
** to MCS-9(8). MCS_5_7 and MCS_6_9 are not proper MCS but are used only by the network
** to command the MS to use MCS_5 or MCS_7 and MCS_6 or MCS_9. To avoid an out-of-bound
** access 3 dummy entries are included after MCS-9
**------------------------ ----------------------------------------------------------
*/
const uint8 E_GRLC_DATA_BLOCK_LEN[] = { 22,       /* MCS-1 */
                                        28,       /* MCS-2 */
                                        37,       /* MCS_3 */
                                        44,       /* MCS-4 */
                                        56,       /* MCS-5 */
                                        74,       /* MCS-6 */
                                        56,       /* MCS-7 */
                                        68,       /* MCS-8 */
                                        74,       /* MCS-9 */
                                        56,       /* dummy value for MCS-5-7 */
                                        74,       /* dummy value for MCS-6-9 */
                                         0        /* dummy value for RM_MCS_UNDEF */
                                      };


/* main data structure containing the majority of data
** related to RLC uplink operation
*/
grlc_ul_t     ul;

/* This flag controls whether the data block timers are serviced (in ack mode)
** by the PL1 ISR. When this flag is set, the timer service re-starts the
** timer after serviving it.
**
** This flag is set when RLC first sends PH_DATA_REQ primitive to PL1 and
** and stays on for the duration of the TBF. It is cleared when a TBF terminates
*/
boolean b_block_timers_service_active = {FALSE};

/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      RLC_UPLINK_INIT
===
===  DESCRIPTION
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
===    return value written to module var cv.
===
===========================================================================*/
static void clr_ul_timers (void)
{
  /*-------------------------------------------*/

  /* deactivate block timer service for ack mode and un-ack mode as
  ** well (especially)
  */
  b_block_timers_service_active = FALSE;

  (void)rex_clr_timer(&(ul.timer_mac_ul_resp.rex_timer));
  ul.timer_mac_ul_resp.state = TMR_INACTIVE;

  (void)rex_clr_timer(&(ul.timer_t3182.rex_timer));
  ul.timer_t3182.state = TMR_INACTIVE;

  (void)rex_clr_timer(&(ul.timer_await_ul_acknack.rex_timer));
  ul.timer_await_ul_acknack.state = TMR_INACTIVE;
} /* clr_ul_timers */

/*===========================================================================
===
===  FUNCTION      DETERMINE_USER_PLANE_ACCESS_PHASE()
===
===  DESCRIPTION
===
===    Determine access mode required as per 7.1.2.1 given the rlc mode
===    and the number of data blocks to send in the uplink. The result is
===    used as parameter for the est_tbf_req msg to peer entity via MAC.
===
===    When there are more than 8 blocks to send in ack mode the MS can,
===    as an implementation option, choose to request two phase access
===
===    Removal of Short Access
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    uint8 (access phase type)
===
===  SIDE EFFECTS
===
===    none
===
===  NOTES
===    Removed forcing from TWO-PHASE to ONE-PHASE on GCF testing
===
===========================================================================*/
static uint8 determine_user_plane_access_phase
(
  const uint32  num_octets,
  const uint8   rlc_mode
)
{
  /* 7.1.2.1 para8:
  ** If the mobile station intends to use the TBF to send user data, it shall
  ** request two phase access if the requested RLC mode is unacknowledged mode.
  ** If the requested RLC mode is acknowledged mode and the amount of data can
  ** fit in 8 or less than 8 RLC/MAC blocks, the mobile station shall indicate
  ** One/Two Phase Access as access type. The number of blocks shall be
  ** calculated assuming channel coding scheme CS-1. If the requested RLC mode
  ** is acknowledged mode and the amount of data to send takes more than 8 RLC/MAC
  ** blocks, the mobile station shall request either one phase access or
  ** two phase access
  */

  if (rlc_mode == GRLC_MODE_UNACK)
  {
    return GMAC_TWO_PHASE_ACCESS;
  }
  else
  {
    /* For WAP access it is generally more efficient to request one phase
    ** access where the data available to transmit is less than 500 octets.
    **
    ** Below 500 octets it is known for some BSS equipments to only allocate
    ** 1 uplink timeslot for a class 10 mobile, and thus if a DL TBF is
    ** established shortly after the UL is established, the BSS is in a
    ** position to immediately allocate a full 4 downlink timeslots.
    **
    ** Above 500 octest we would wish to request two phase access.
    */

    if ( (num_octets < 500) || (ul.anite_gcf_flg) )
    {
      return GMAC_ONE_PHASE_ACCESS;
    }
    else
    {
      return GMAC_TWO_PHASE_ACCESS;
    }
  }

} /* determine_user_plane_access_phase */

/*===========================================================================
===
===  FUNCTION      FILL_TBF_EST_REQ_MSG()
===
===  DESCRIPTION
===
===    Fill the message to send to MAC to request a tbf from the network.
===    Parameters taken from LLC PDU delivery request such as radio
===    priority, rlc mode, peak throughput and llc pdu type are used to
===    fill the Channel Request Description.
===
===    The preferred access mode is set to FIXED even though the nw will
===    decide which is to be used.
===
===    The size of the PDU and rlc mode determine the access phase
===    requested.
===
===    SHORT_ACCESS is now removed
===
===    For one and two phase access, the number
===    of octets in octet_cnt is copied to RLC_OCTET_COUNT field of the
===    CRD, if the tbf type requested is close-ended. For open-ended tbf
===    request, CRD is made 0.
===
===    Notes on grr_data_req_cause_t:
===    GRR_CAUSE_LAY3_DATA and GRR_LLC_SIGNALLING are considered as 'data'.
===    RLC can specify to MAC the access phase it prefers, from the RLC Mode
===    and the data length as per function determine_access_phase(). For the
===    remaining causes: GRR_CAUSE_PAGE_RESPONSE, GRR_CAUSE_GMM_SIGNALLING and
===    GRR_CAUSE_GMM_CELL_UPDATE, MAC does not accept any access phase input
===    from RLC. This will be decided and returned by the network. One-phase
===    or probably two-phase will be used as it is more robust.
===
===  PARAMETERS
===
===    msg_ptr: pointer to message to send to MAC.
===    llc_pdu_type, ptc (peak throughout class), radio prio, octet count,
===    are part of the CRD from LLC PDU.
===
===  DEPENDENCIES
===
===    ul struct must be initialised
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    ul.octets_req and ul.blocks_req are filled in
===
===========================================================================*/
static void fill_tbf_est_req_msg
(
  rlc_mac_msg_t         *rlc_msg_ptr,
  uint8                 llc_pdu_type,
  uint8                 peak_thr_class,
  uint8                 radio_prio,
  uint16                octet_cnt,
  uint32                llc_tlli,
  uint8                 pfi,
  grr_data_req_cause_t  cause
)
{
  /*----------------------------------------------------------*/

  /* req TBF est
  */
  rlc_msg_ptr->msg.sig = RM_UL_EST_TBF_REQ;

  /* Pass TLLI onto MAC
  */
  rlc_msg_ptr->msg.ul_est_tbf_req.tlli = llc_tlli;

  /* store pdu length
  */
  ul.octets_req = octet_cnt;

  /* clear blocks granted ready for resource to come and fill it
  */
  ul.blocks_granted = 0;

  /* TBF Est Type preference is open-ended
  */
  rlc_msg_ptr->msg.ul_est_tbf_req.tbf_type = RM_OPEN_ENDED_TBF;

  /* Determine the number of blocks assuming no TLLI.
  ** Value returned in ul.num_blks
  **
  ** ! Set b_next_pdu_avail to TRUE to get a larger value for ul.num_blks
  */

  e_get_num_blks_ex( octet_cnt, RM_CS_1, RM_CS_1, RM_TLLI_NOT_PRESENT, TRUE);

  ul.blocks_req = ul.num_blks;

  /* if the cause is user plane data then RLC can state its preferred access-phase
  */
  if ( (cause == GRR_CAUSE_LAY3_DATA     ) ||
       (cause == GRR_CAUSE_LLC_SIGNALLING)    )
  {
    /* first determine access type given the number of blocks, octets and rlc mode.
    ** The outcome can be one phase, two phase.
    */
    rlc_msg_ptr->msg.ul_est_tbf_req.access_phase =
      determine_user_plane_access_phase( (uint32)ul.octets_req, ul.rlc_mode);
  }
  else if ( cause == GRR_CAUSE_GMM_CELL_NOTIFY )
  {
    /* Siemens BSS which supports Cell Notification for R99 appears to prefer
       a Cell Notify LLC NULL Command to be conveyed using two phase access.
       From MAC perspective a CELL Notify is to be handled as a Cell Update. */

    rlc_msg_ptr->msg.ul_est_tbf_req.access_phase = GMAC_TWO_PHASE_ACCESS;
    cause                                        = GRR_CAUSE_GMM_CELL_UPDATE;
  }
  else
  {
    /* All other GMM Procedure causes must request one phase access. */

#if  defined (DEBUG_GSM_GPRS_RLC_TEST) || defined (PERLUTF)
    #error code not present
#else
    rlc_msg_ptr->msg.ul_est_tbf_req.access_phase = GMAC_ONE_PHASE_ACCESS;
#endif
  }

  switch(rlc_msg_ptr->msg.ul_est_tbf_req.access_phase)
  {
    case GMAC_ONE_PHASE_ACCESS:
      /* Determine the number of blocks with TLLI required for One-phase.
      ** Value returned in ul.num_blks
      **
      ** ! Set b_next_pdu_avail to TRUE to get a larger value for ul.num_blks
      */
      get_num_blks_ex( octet_cnt, RM_CS_1, RM_CS_1, RM_TLLI_PRESENT, TRUE);

      break;

    case GMAC_TWO_PHASE_ACCESS:
      /* If the access mode is dynamic then TBF type is determined
      ** by the n/w. Close-ended TBF if RLC_DATA_BLOCKS_GRANTED field
      ** is present, otherwise open-ended TBF is assumed.
      **
      ** Determine the number of blocks without TLLI.
      ** Value returned in ul.num_blks
      **
      ** ! Set b_next_pdu_avail to TRUE to get a larger value for ul.num_blks
      */
      e_get_num_blks_ex( octet_cnt, RM_CS_1, RM_CS_1, RM_TLLI_NOT_PRESENT, TRUE);
      break;

    case GMAC_NO_PHASE_ACCESS:
      /* Force two-phase access for No-phase-access
      */
      rlc_msg_ptr->msg.ul_est_tbf_req.access_phase = GMAC_TWO_PHASE_ACCESS;

      /* Determine the number of blocks without TLLI.
      ** Value returned in ul.num_blks
      **
      ** ! Set b_next_pdu_avail to TRUE to get a larger value for ul.num_blks
      */
      get_num_blks_ex( octet_cnt, RM_CS_1, RM_CS_1, RM_TLLI_NOT_PRESENT, TRUE);

      break;

    default:
        MSG_GERAN_LOW_1_G("GRLU bad access_phase %d",
                    rlc_msg_ptr->msg.ul_est_tbf_req.access_phase);

        break;
  }

  /* Store number of blocks requested
  */
  ul.blocks_req = ul.num_blks;

  /* For a Class 10 mobile, Open Ended versus Closed Ended request is a key
  ** performance parameter for downlink FTP. Under such conditions we wish to
  ** avoid being given 2 UL slots by the BSS, as the DL will then be limited to
  ** 3 slots instead of the maximum 4.
  **
  ** In general it has been found that most BSS will allocate two UL slots
  ** for open ended request, or for closed ended request of greater than 500 octets.
  ** This threshold shall be used to determine the nature of the request.
  */

  if ( ul.octets_req < 500 )
    rlc_msg_ptr->msg.ul_est_tbf_req.chan_req_desc.rlc_octet_cnt = ul.octets_req;
  else
    rlc_msg_ptr->msg.ul_est_tbf_req.chan_req_desc.rlc_octet_cnt = 0;

  /* Always fill 'num_of_blocks' even though it's only required for SHORT_ACCESS.
  ** MAC will need to subtract 1 from this to encode its message to the nw
  */
  rlc_msg_ptr->msg.ul_est_tbf_req.num_of_blocks = (uint8)ul.blocks_req;

  /* Fill in the remaining parameters of the Channel
  ** Request Description.
  */
  rlc_msg_ptr->msg.ul_est_tbf_req.chan_req_desc.rlc_mode = ul.rlc_mode;
  rlc_msg_ptr->msg.ul_est_tbf_req.chan_req_desc.llc_pdu_type = llc_pdu_type;
  rlc_msg_ptr->msg.ul_est_tbf_req.chan_req_desc.peak_thr_class = peak_thr_class;
  rlc_msg_ptr->msg.ul_est_tbf_req.chan_req_desc.radio_priority = radio_prio;

  /* Only determine these values when resource is assigned to allow PDU
  ** grouping operation
  */
  ul.tbc = 0;
  ul.abs_bsn = 0;

  /* Translate the cause to GMM_SIGNALLING as MAC need not know SM cuase
  ** as its still signalling for MAC.
  */
  if ( cause == GRR_CAUSE_SM_SIGNALLING)
  {
    cause = GRR_CAUSE_GMM_SIGNALLING;
  }

  /* fill in the llc data cause so MAC can map it to
  ** the access type to request of its PCR msg. If the mode is ACK
  */
  if (ul.rlc_mode == GRLC_MODE_ACK)
  {
    rlc_msg_ptr->msg.ul_est_tbf_req.data_req_cause = cause;
  }
  else
  {
    /* The cause must be data in unack mode even though LLC does not say this
    */
    rlc_msg_ptr->msg.ul_est_tbf_req.data_req_cause = GRR_CAUSE_LAY3_DATA;
  }

#if defined(FEATURE_GERAN_IMPLICIT_PS_REJECT) || defined(FEATURE_GERAN_EAB)
  /* For EAB and IR, extended access check is not applicable for Page Rsp.
   * So, set the TBF request cause to page response if RLC has any PDU for Page Rsp.
   * By doing so GRR will allow access, even if there are PDUs ahead of Page Rsp PDU. */
  if (rlc_msg_ptr->msg.ul_est_tbf_req.data_req_cause != GRR_CAUSE_GMM_PAGE_RESPONSE)
  {
    /* If any PDU has Page Rsp cause, then establish TBF for Page Rsp */
    if (next_pdu_ptr && next_pdu_ptr->cause == GRR_CAUSE_GMM_PAGE_RESPONSE)
    {
      MSG_GERAN_HIGH_1_G("Cause (%d) changed to Page Rsp", rlc_msg_ptr->msg.ul_est_tbf_req.data_req_cause);
      rlc_msg_ptr->msg.ul_est_tbf_req.data_req_cause = GRR_CAUSE_GMM_PAGE_RESPONSE;
    }
    else
    {
      boolean page_rsp_found = FALSE;
      gprs_grr_ul_mes_t *temp = (gprs_grr_ul_mes_t *) q_check(&ULQ());

      /* Check main UL queue */
      while (temp)
      {
        if (temp->cause == GRR_CAUSE_GMM_PAGE_RESPONSE)
        {
          MSG_GERAN_HIGH_1_G("Cause (%d) changed to Page Rsp", rlc_msg_ptr->msg.ul_est_tbf_req.data_req_cause);
          rlc_msg_ptr->msg.ul_est_tbf_req.data_req_cause = GRR_CAUSE_GMM_PAGE_RESPONSE;
          page_rsp_found = TRUE;
          break;
        }
        
        temp = (gprs_grr_ul_mes_t *) q_next(&ULQ(), &temp->link);
      }

      /* Check temporary UL queue, if still not found */
      if (page_rsp_found == FALSE)
      {
        temp = (gprs_grr_ul_mes_t *) q_check(&ULTQ());
        while (temp)
        {
          if (temp->cause == GRR_CAUSE_GMM_PAGE_RESPONSE)
          {
            MSG_GERAN_HIGH_1_G("Cause (%d) changed to Page Rsp", rlc_msg_ptr->msg.ul_est_tbf_req.data_req_cause);
            rlc_msg_ptr->msg.ul_est_tbf_req.data_req_cause = GRR_CAUSE_GMM_PAGE_RESPONSE;
            break;
          }
          
          temp = (gprs_grr_ul_mes_t *) q_next(&ULTQ(), &temp->link);
        }
      }
    }
  }
#endif /* FEATURE_GERAN_IMPLICIT_PS_REJECT || FEATURE_GERAN_EAB */

  /* Make sure that the test mode is turned off */
  rlc_msg_ptr->msg.ul_est_tbf_req.test_mode = TEST_MODE_OFF;
  rlc_msg_ptr->msg.ul_est_tbf_req.srb_mode  = FALSE;

  ul.num_ests++;

  if( IS_PFC_NW_SUPPORTED() )
  {
    /* PFC is supported in the cell. Add PFI in tbf_est_req msg */
    rlc_msg_ptr->msg.ul_est_tbf_req.pfi_present = TRUE;
    rlc_msg_ptr->msg.ul_est_tbf_req.pfi         = pfi;

    /* Latch latest PFI value sent to NW */
    ul.pfi = pfi;
  }
  else
  {
    rlc_msg_ptr->msg.ul_est_tbf_req.pfi_present = FALSE;
    rlc_msg_ptr->msg.ul_est_tbf_req.pfi         = 0;
  }

} /* fill_tbf_est_req_msg */

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GRLC_UL_INIT_GPRS_DATA_BLOCK()
===
===  DESCRIPTION
===
===    Fill GPRS data block default starting values
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
void grlc_ul_init_gprs_data_block (grlc_ul_data_block_t *bp)
{
  if (bp->dsm_ptr != NULL)
  {
    MSG_GERAN_LOW_1_G("## cur_blk dsm %d not NULL",bp->bsn);
    GPRS_PDU_FREE(&bp->dsm_ptr);
  }
  if (bp->dup_dsm_ptr != NULL)
  {
    MSG_GERAN_LOW_1_G("## cur_blk dup %d not NULL",bp->bsn);
    GPRS_PDU_FREE(&(bp)->dup_dsm_ptr);
  }
  bp->dsm_ptr = NULL;
  bp->dup_dsm_ptr = NULL;
  bp->data_len = 0;
  bp->num_ext_octets = 0;
  bp->ptx_miss_cnt = 0;
  bp->pdu_prio     = GPDU_LOW_PRIORITY;
  if (IS_TLLI_REQUIRED())
  {
    if (ul.cur_res.tlli_block_cs ==
        RM_TLLI_USE_CS_1
       )
    {
      bp->rem_cnt = GRLC_DATA_BLOCK_LEN[RM_CS_1] -
                 RM_RLC_HDR_LEN;
    }
    else
    {
      bp->rem_cnt = GRLC_DATA_BLOCK_LEN[ul.cur_res.cs] -
                 RM_RLC_HDR_LEN;
    }
    bp->rem_cnt = (bp)->rem_cnt - RM_TLLI_LEN;
    bp->ti = RM_TLLI_PRESENT;
    bp->tlli = cur_pdu_ptr->tlli;
  }
  else
  {
    bp->ti = RM_TLLI_NOT_PRESENT;
    bp->rem_cnt = GRLC_DATA_BLOCK_LEN[ul.cur_res.cs] -
                   RM_RLC_HDR_LEN;
    bp->tlli = 0;
  }
  if( IS_PFI_REQUIRED() )
  {
    /* PFI length is 7 bits and 1 bit for the E bit */
    bp->rem_cnt -= RM_PFI_E_LEN;
    bp->pi       = RM_PFI_PRESENT;
    bp->pfi      = cur_pdu_ptr->pfi;
  }
  else
  {
    bp->pi       = RM_PFI_NOT_PRESENT;
    bp->pfi      = 0;
  }
} /* grlc_ul_init_gprs_data_block */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_INIT_EGPRS_DATA_BLOCK()
===
===  DESCRIPTION
===
===    Fill EGPRS data block default starting values
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
void grlc_ul_init_egprs_data_block (grlc_ul_data_block_t *bp)
{
  if (bp->dsm_ptr != NULL)
  {
    MSG_GERAN_LOW_1_G("## cur_blk dsm %d not NULL",bp->bsn);
    GPRS_PDU_FREE(&bp->dsm_ptr);
  }

  if (bp->dup_dsm_ptr != NULL)
  {
    MSG_GERAN_LOW_1_G("## cur_blk dup %d not NULL",bp->bsn);
    GPRS_PDU_FREE(&bp->dup_dsm_ptr);
  }

  bp->dsm_ptr = NULL;
  bp->dup_dsm_ptr = NULL;
  bp->data_len = 0;
  bp->num_ext_octets = 0;
  bp->ptx_miss_cnt = 0;
  bp->nack_miss_cnt = 0;
  bp->pdu_prio      = GPDU_LOW_PRIORITY;

  bp->mcs_demoted_ps = NO_MCS_DEMOTION;

  /* Clear SPB bit in the header */
  bp->hdr.spb = 0;
  if (IS_TLLI_REQUIRED())
  {
    if (ul.cur_res.tlli_block_cs ==
        RM_TLLI_USE_MCS_1
       )
    {
      bp->rem_cnt = E_GRLC_DATA_BLOCK_LEN[RM_MCS_1];
      bp->cs = RM_MCS_1;
    }
    else
    {
      bp->rem_cnt = E_GRLC_DATA_BLOCK_LEN[ul.cur_res.cs];
      SET_MCS_FOR_TX(bp->cs, ul.cur_res.cs);
    }
    bp->rem_cnt = bp->rem_cnt - RM_TLLI_LEN;
    bp->ti = RM_TLLI_PRESENT;
    bp->tlli = cur_pdu_ptr->tlli;
  }
  else
  {
    SET_MCS_FOR_TX(bp->cs, ul.cur_res.cs);
    bp->ti = RM_TLLI_NOT_PRESENT;

    if (ul.cur_res.cs < RM_MCS_UNDEF)
    {
      bp->rem_cnt = E_GRLC_DATA_BLOCK_LEN[ul.cur_res.cs];
    }
    else
    {
      MSG_GERAN_LOW_1_G("ERLU ##cur_res.cs oor %u",ul.cur_res.cs);

      ul.b_error_recovery_required = TRUE;
#ifndef FEATURE_GERAN_REDUCED_DEBUG
      ul_diag_stats_st.ul_gen_purpose_count_a++;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
      detect_error_and_recover();
    }

    bp->tlli = 0;
  }

  if( IS_PFI_REQUIRED() )
  {
    /* PFI length is 7 bits and 1 bit for the E bit */
    bp->rem_cnt -= RM_PFI_E_LEN;
    bp->hdr.pi   = RM_PFI_PRESENT;
    bp->pfi      = cur_pdu_ptr->pfi;
  }
  else
  {
    bp->hdr.pi   = RM_PFI_NOT_PRESENT;
    bp->pfi      = 0;
  }
} /* grlc_ul_init_egprs_data_block */

/*===========================================================================
===
===  FUNCTION      GRLC_IS_XFER_ACTIVE()
===
===  DESCRIPTION
===
===    Returns TRUE if RLC is in acknowledged or unacknowledged transfer state
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
boolean grlc_is_xfer_active_dyn(void)
{
  return (ul.state == US_ACK_XFER || ul.state == US_UNACK_XFER);
}

/*===========================================================================
===
===  FUNCTION      GRLC_UL_ADJUST_FLOW_CONTROL_THRESHOLDS()
===
===  DESCRIPTION
===
===    This function reads the USF usage statistic provided by L1 and determines
===    if a reduction to the current flow control values is required. If so a scale
===    factor (in percentage) is applied, and the current flow control is updated
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
void grlc_ul_adjust_flow_control_thresholds (void)
{
  /* Get average tx blanking figure from L1
  */
  l1_get_usf_utilisation_info ( &(ul.usf_info) );

  MSG_GERAN_HIGH_5_G("GRLU usf info FN = %lu, usfs = %lu, blanked = %lu, consecutive blanked = %lu, percentage blanking = %lu",
    ul.usf_info.frame_num,
    ul.usf_info.total_usfs,
    ul.usf_info.total_blocks_blanked,
    ul.usf_info.num_consecutive_blanked_blocks,
    ul.usf_info.blanking_percentage_average);

  /* Update flow control
  */
  grlc_llc_ul_flow_control();

} /* grlc_ul_eval_usf_stat */

/*===========================================================================
===
===  FUNCTION      GRLC_SERVICE_BLOCK_TIMERS()
===
===  DESCRIPTION
===
===    This function has been modified for R99 where block timers are replaced
===    by block counters. This function now simply increments the un-signed 16-bit
===    global variable 'grlc_ul_block_counter'.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_service_block_timers_dyn(void)
{
  if (b_block_timers_service_active)
  {
    grlc_ul_block_counter++;
    if (grlc_ul_block_counter >= 32768)
      grlc_ul_block_counter = 0;

    /* Invoke USF utilisation evaluation function
    */
    if ( (++ul.usf_evaluation_interval_cnt) > USF_EVALUATION_INTERVAL_MAX )
    {
      /* reset interval count
      */
      ul.usf_evaluation_interval_cnt = 0;

      /* Trigger USF utilisation evaluation
      */
      ul.b_adj_flow_control = TRUE;
  }
}
} /*( grlc_service_block_timers */

/*===========================================================================
===
===  FUNCTION      GRLC_UL_DATA_DETECT()
===
===  DESCRIPTION
===
===    This function is called by PL1 to aid detecting the "rlc no data"
===    condition. PL1 is to pass TRUE on a good access and FALSE on a bad one.
===
===  PARAMETERS
===    b: boolean value indicating whether a data block was succesfully obtained
===       or not
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_data_detect_dyn(boolean b)
{
  GRLC_UL_NO_DATA_LOCK();

  if (b)
  {
    /* Reset counter when a data block was successully got by PL1
    */
    grlc_ul_no_data_ctr = 0;
  }
  else
  {
    /* If EXT_UTBF is active then no data conditions are valid. So in this
    ** case we keep clearing the no-data-counter to disable this detection feature
    */
    if (IS_EXT_UTBF_ACTIVE())
    {
      grlc_ul_no_data_ctr = 0;
    }
    else
    {
      /* PL1 at this point finds no data in the FIFO. If CV=0 had been sent then
      ** 1) in un-ack mode RLC can repeat sending the last block CV=0 upto 4 times
      ** and no more after that, or 2) RLC ack mode but all blocks were ack'ed by the
      ** network so RLC can not repeat sending PACK or NACK blocks. Either way CV would
      ** have to be 0 for this to happen
      */
      if (ul.cv == 0)
      {
        /* Not a genuine "no data" screnario. Clear the counter as if data were detected
        */
        grlc_ul_no_data_ctr = 0;
      }
      else
      {
        /* Count up to ceiling value
        */
        if (grlc_ul_no_data_ctr < GRLC_UL_NO_DATA_THRESHOLD)
          grlc_ul_no_data_ctr++;
      }
    }
  }
  GRLC_UL_NO_DATA_UNLOCK();

} /* grlc_ul_data_detect */

/*===========================================================================
===
===  FUNCTION      CLR_UL_STATE_INFO()
===
===  DESCRIPTION
===
===    Clear the uplink state and event variables
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
===========================================================================*/
static void clr_ul_state_info (void)
{
  ul.state = US_NULL;
  UPD_UL_STATE();
  ul.event = UE_NULL;
  ul.segment_state = SEGMENT_NULL;
  ul.realloc_state = RALS_NULL;
  UPD_UL_SUBSTATE();
}

/*===========================================================================
===
===  FUNCTION      grlc_ul_free_tx_array
===
===  DESCRIPTION
===
===        Free up the memory for tx_arr
===  DEPENDENCIES
===
===  RETURN VALUE
===
===           None
===
===========================================================================*/
void grlc_ul_free_tx_array(void)
{
  if(NULL != p_tx_arr)
  {
    MSG_GERAN_LOW_0_G("GRLU UL free TX workspace");
    /* Free the tx_arr */
    EXTPL_MEM_FREE(p_tx_arr);

    /*The tx_arr is no longer valid*/
    p_tx_arr = NULL;
  }
} /* grlc_ul_free_tx_array */

/*===========================================================================
===
===  FUNCTION      CLR_UL_DATA()
===
===  DESCRIPTION
===
===    Clear the uplink data structure
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
===    return value written to module var cv.
===
===========================================================================*/
void clr_ul_data (void)
{
  uint16 loop_cnt;
  uint16 b;

  /*------------------------------------------------------------------*/

  /* clear out RLC UL-PL1 FIFO
  */
  grlc_ul_fifo_re_init();

  clr_ul_timers();

  ul.guard_timer_state = GUARD_OFF;

  ul.b_ext_utbf_init_group = FALSE;

  /* Pre-emptive transmission is the default for the MS see 9.1.3.2 */
  ul.pre_emptive_tx = 1;

  /* Clear tagged BSN with CV=0 */
  ul.e_bsn_of_cv_0_block = E_GRLC_INVALID_BSN;

  /* The tx_arr to be freed here, if not already free. First loop
  ** thru to check if any dsm_ptr needs to be freed.
  */
  if(NULL != p_tx_arr)
  {
    if (TBF_MODE_EGPRS == ul.cur_res.tbf_mode)
    {
      loop_cnt = E_TX_WORKSPACE;
    }
    else
    {
      loop_cnt = RM_RLC_TX_WORKSPACE;
    }

    for (b=0; b < loop_cnt ; b++)
    {
      TX_ARR( b).vb = TXA_INACTIVE;
      TX_ARR( b).blk.ptx_miss_cnt = 0;
      TX_ARR( b).blk.nack_miss_cnt = 0;
      TX_ARR( b).blk.mcs_demoted_ps = NO_MCS_DEMOTION;

      if (NULL != TX_ARR( b).blk.dup_dsm_ptr)
      {
        MSG_GERAN_LOW_2_G("ERLU tx_arr[%u].dup_dsm_ptr not NULL 0x%x",
                b,
                TX_ARR( b).blk.dup_dsm_ptr);
        GPRS_PDU_FREE(&TX_ARR( b).blk.dup_dsm_ptr);
      }

      if (NULL != TX_ARR( b).blk.dsm_ptr)
      {
        MSG_GERAN_LOW_2_G("ERLU tx_arr[%u].dsm_ptr not NULL 0x%x",
                b,
                TX_ARR( b).blk.dsm_ptr);
        GPRS_PDU_FREE(&TX_ARR( b).blk.dsm_ptr);
      }
    }

    grlc_ul_free_tx_array();
  }

  /* Reset BSN of block with CV=0 in un-ack mode
  */
  ul.bsn_of_cv_0_block = GRLC_INVALID_BSN;
  ul.e_bsn_of_cv_0_block = E_GRLC_INVALID_BSN;

  ul.b_cv_0 = FALSE;
  ul.seg_ind = 0;
  UPD_UL_NSEG();

  ul.va = 0;
  UPD_UL_VA();

  ul.vs = 0;
  UPD_UL_VS();

  ul.vsf = 0;
  UPD_UL_VSF();

  ul.nack_vsf = 0;
  UPD_UL_NACK_VSF();

  ul.pack_vsf = 0;
  UPD_UL_PACK_VSF();

  ul.ssn = 0;
  ul.cv = (uint8)BS_CV_CONST;
  ul.si = RM_TX_NOT_STALLED;

  /* Ensure no reallocation on value is not properly initialised
  */
  ul.ptx_cv = BS_CV_CONST;
  ul.pak_cv = BS_CV_CONST;

  ul.bsn = 0;
  ul.abs_bsn = 0;

  ul.rlc_mode = GRLC_MODE_UNACK;

  ul.last_valid_coding_scheme = ul.cur_res.cs;
  ul.re_tx_cv_0_cnt = 0;
  ul.octets_req = 0;
  ul.octets_rem = 0;

  ul.contention_status = RM_CONTENTION_UNRESOLVED;
  ul.ul_release_cause = REL_UL_NORMAL;
  ul.b_first_blk_sent = FALSE;
  ul.tbc = 0;
  ul.b_li_0_exists = FALSE;
  ul.blocks_req = 0;
  ul.blocks_rem = 0;
  ul.blocks_granted = 0;

  ul.b_first_fifo_load = TRUE;
  ul.b_to_send_last_block = FALSE;
  ul.b_forced_release = FALSE;

  ul.pdu_freeing_state = NONE_TO_FREE;
  ul.psn_to_free_inclusive = ul.first_psn_to_free;
  ul.prev_realloc_psn = 0;

  ul.test_mode = TEST_MODE_OFF;
  ul.test_mode_ul_tbf_rel_after_fai = FALSE;
  ul.b_puan = FALSE;

  ul.nack_cnt = 0;
  ul.ptx_cnt = 0;
  ul.pack_cnt = 0;

  /* Omit clearing ul.cur_res and ul.alloc_cnf
  */
  clr_ul_state_info();

  /* Update for engg_mode
  */
  geran_eng_mode_data_write(ENG_MODE_GPRS_UL_CS,   &ul.cur_res.cs, FALSE);
  geran_eng_mode_data_write(ENG_MODE_EGPRS_UL_MCS, &ul.cur_res.cs, FALSE);

} /* clr_ul_data */

/*===========================================================================
===
===  FUNCTION      READ_UL_SYSINFO()
===
===  DESCRIPTION
===
===    Reads the uplink sysinfo from GRR
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
===========================================================================*/
void read_ul_sysinfo (void)
{
#if (!defined(DEBUG_GSM_GPRS_RLC_TEST) && !defined(TEST_FRAMEWORK))

  /*------------------------------------------------------------------*/

  /* Read  Cell Options IEs
  */
  rr_read_public_store(RRPS_CELL_OPTIONS_T3192, &sysinfo.t3192_index);
  rr_read_public_store(RRPS_CELL_OPTIONS_BS_CV_MAX, &sysinfo.bs_cv_max);
  /* If bs_cv_max is zero then interpret it as 1
  */
  if (sysinfo.bs_cv_max == 0)
  {
    sysinfo.bs_cv_max = 1;
  }
  rr_read_public_store(RRPS_CELL_OPTIONS_PAN_DEC, &sysinfo.pan_dec);
  rr_read_public_store(RRPS_CELL_OPTIONS_PAN_INC, &sysinfo.pan_inc);
  rr_read_public_store(RRPS_CELL_OPTIONS_PAN_MAX, &sysinfo.pan_max);
  rr_read_public_store(RRPS_CELL_OPTIONS_PFC_FEATURE_MODE, &sysinfo.pfc_feature_mode);

  /* Read the MAX multislot class
  */
  rr_read_public_store(RRPS_MS_MULTISLOT_CLASS, &sysinfo.max_gprs_multislot_class);
  rr_read_public_store(RRPS_MS_EGPRS_MULTISLOT_CLASS, &sysinfo.max_egprs_multislot_class);

  /*-----------------------------------------------------
  ** Set sysinfo.ext_utbf to turn the feature ON, if:
  ** - feature is supported and active OR
  ** - when it is forced to being active during testing
  ** Otherwise, clear the value to turn the feature OFF
  **-----------------------------------------------------
  */
    /* Take value from the Public Store
    */
    rr_read_public_store(RRPS_CELL_OPTIONS_NW_EXT_UL_TBF, &sysinfo.ext_utbf);

  /* Everytime we read the PAN_MAX parameter we need to convert its coded
  ** value into a real value and store it in ul.pan_max_value
  */
  DECODE_PAN_MAX();

#endif /* !defined(DEBUG_GSM_GPRS_RLC_TEST) && !defined(TEST_FRAMEWORK) */

  MSG_GERAN_MED_3_G("GRLU Sys ext_utbf cv_max t3192_ind X...YYZZ=%08x PAQ 0CtrInOu=%08x PaqMax=%u",
          ((uint32)sysinfo.ext_utbf  << 28) |
          ((uint32)sysinfo.bs_cv_max <<  8) |
          ((uint32)sysinfo.t3192_index & 0x0f),

          ((uint32)ul.paq_ctr << 16) |
          ((uint32)ul.paq_in  <<  8) |
          ((uint32)ul.paq_out & 0xff),

          ul.paq_ctr_max);

} /* read_ul_sysinfo */

/*===========================================================================
===
===  FUNCTION      CHECK_REALLOC_ACTION()
===
===  DESCRIPTION
===
===    Determine the re-allocation action required for the next pdu based
===    on its requirements of rlc mode, radio priority, peak-throughput class
===    and whether countdown has started on the current PDU, and whether the
===    mode is Dynamic/Fixed Open/Close-ended
===
===    Reallocation should be assessed just after initiation of transfer of
===    the current PDU to ensure reallocation negotiation is complete before
===    countdown of the current PDU.
===
===  PARAMS
===
===    none
===
===  DEPENDENCIES
===
===    ul_q.pdu_arr[1] -- uplink resource queue
===    next_pdu is NOT NULL.
===
===  RETURN VALUE
===
===    Resulting action is stored in ul.realloc_action. See definition of
===    realloc_action_t
===
===  SIDE EFFECTS
===
===========================================================================*/
static realloc_action_t  determine_realloc_action
(
  gprs_grr_ul_mes_t       *cur_ptr,
  const gprs_grr_ul_mes_t *next_ptr
)
{
  if (cur_ptr->realloc_action == RA_NULL)
  {
    /* Avoid reallocation during countdown in non-extended-ul-tbf
    */
    if (!IS_EXT_UTBF_ACTIVE() &&
        IS_TX_CV_ACTIVE()
       )
    {
      cur_ptr->realloc_action = RA_NULL;
    }
    else if (ul.prev_realloc_psn == next_ptr->psn)
    {
      MSG_GERAN_LOW_1_G("ERLU det_realloc PSN realloc'ed earlier %u", ul.prev_realloc_psn);
      cur_ptr->realloc_action = RA_NULL;
    }
    else if (IS_REALLOC_TYPE_1(cur_ptr, next_ptr))
      cur_ptr->realloc_action = RA_IMMEDIATE;
    else if (IS_REALLOC_TYPE_2(cur_ptr, next_ptr))
      cur_ptr->realloc_action = RA_AT_TX_COMPLETE;
    else if (IS_REALLOC_TYPE_3(cur_ptr, next_ptr))
      cur_ptr->realloc_action = RA_AT_TX_COMPLETE_CHANGE_MODE;
    else
      cur_ptr->realloc_action = RA_NULL;
  }

  return cur_ptr->realloc_action;

} /* determine_realloc_action */

/*==========================================================================
===
===  FUNCTION      CHECK_TO_SEND_PRR
===
===  DESCRIPTION
===
===    If state in transfer and both current and next PDU exist then assess Qos
===    of current PDU against next PDU and initiate reallocation action.
===
===  PARAMS
===
===    grr_ul_msg_ptr points to LLC message containing info about the LLC PDU
===    and a pointer to the PDU in DSM pool. It also has space for info internal
===    to RLC when RLC processes it.
===
===  DEPENDENCIES
===
===    cur_pdu_ptr, next_pdu_ptr, ul
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void check_to_send_prr (void)
{
  MSG_GERAN_LOW_3_G("GRLU prr cur_p=0x%x next_p=0x%x state=%u",
           cur_pdu_ptr,
           next_pdu_ptr,
           ul.state
         );

  /*------------------------------------------------------------------------
  ** Exit if current PDU or next PDU does not exist or if current state is
  ** NOT transfer
  **------------------------------------------------------------------------
  */
  if (cur_pdu_ptr == NULL           ||
      ul.realloc_state != RALS_NULL ||

      /* non-transfer state
      */
      (ul.state != US_ACK_XFER &&
       ul.state != US_UNACK_XFER
      )
     )
  {
    MSG_GERAN_LOW_0_G("GRLU prr cur_p NULL or not in xfer or bad ral state NULL");
    return;
  }
  else
  {
    if (next_pdu_ptr == NULL)
    {
      dequeue_pdu( &next_pdu_ptr);
      if (next_pdu_ptr == NULL)
      {
        MSG_GERAN_MED_2_G("GRLU prr ret next_p = 0x%x rals = %u",
                next_pdu_ptr,
                ul.realloc_state
               );
        return;
      }
    }
  }

  (void)determine_realloc_action ( cur_pdu_ptr, next_pdu_ptr);
  MSG_GERAN_MED_3_G("GRLU prr rals = %u cur-psn =%u next-psn=%u",ul.realloc_state,
                              cur_pdu_ptr->psn,
                              next_pdu_ptr->psn
         );

  /* If reallocation is not in progress
  */
  if (ul.realloc_state == RALS_NULL)
  {
    if (cur_pdu_ptr->realloc_action == RA_IMMEDIATE)
    {
      /* Send PRR with parameters from the next PDU
      */
      ul.realloc_pdu_ptr = next_pdu_ptr;
      ul.realloc_psn = next_pdu_ptr->psn;

      realloc_for_next_pdu( ul.realloc_pdu_ptr,FALSE);

      /* Set RALS_COMPLETE once after sending a PRR.
      ** Expects this realloc_state to change when the next current PDU is loaded
      */
      ul.realloc_state = RALS_COMPLETE;

      MSG_GERAN_LOW_1_G("GRLU prr RA_IMMEDIATE psn=%u",next_pdu_ptr->psn);
    }
    else if (cur_pdu_ptr->realloc_action == RA_AT_TX_COMPLETE)
    {
      if (IS_TX_COMPLETE())
      {
        /* Send PRR with parameters from the next PDU
        */
        ul.realloc_pdu_ptr = next_pdu_ptr;
        ul.realloc_psn = next_pdu_ptr->psn;

        /* b_cv_zero - is set to TRUE , because we
           have transitioned to
           realloc_state = RA_AT_TX_COMPLETE,
           that means we have Data in our Uplink Q.
        */
        realloc_for_next_pdu( ul.realloc_pdu_ptr,FALSE);

        /* Set RALS_COMPLETE once after sending a PRR.
        ** Expects this realloc_state to change when the next current PDU is loaded
        */
        ul.realloc_state = RALS_COMPLETE;

        MSG_GERAN_MED_1_G("GRLU prr TX_COMPLETED psn=%u",next_pdu_ptr->psn);
      }
      else
      {
        /* Mark pointer to PDU to reallocate
        */
        ul.realloc_pdu_ptr = next_pdu_ptr;

        /* Mark the PSN where PRR type 2 will take place
        */
        ul.realloc_psn = next_pdu_ptr->psn;
        DEC_MOD(ul.realloc_psn, 32768);

        /* Now wait for last block of current PDU to be confirmed as transmitted
        */
        ul.realloc_state = RALS_AWAIT_TX_COMPLETE;

        MSG_GERAN_MED_1_G("GRLU prr AWAIT_TX_COMPLETE psn=%u",ul.realloc_psn);
      }
    }
    else if (cur_pdu_ptr->realloc_action == RA_AT_TX_COMPLETE_CHANGE_MODE)
    {
      /* Determine what actions are required from this change of RLC mode in EXT_UTBF
      **
      ** 1. if current PDU countdown procedure is complete (all transmitted by MS and
      **    acknowedged by the NW), then send reallocation request now and say
      **    RALS_COMPLETE. TC 51.3.6.10
      **
      ** 2. if current PDU countdown procedure is complete (all transmitted by MS
      **    including CV=0 and mode is un-ack then send reallocation request now
      **    and say RALS_COMPLETE
      **
      ** 3. if current PDU countdown procedure is on-going and ack mode is in force,
      **    then arrange for the condition where all all blocks are transmitted and
      **    acknowledged by the NW (following PUAN/EPUAN operation for example) and
      **    send a PRR then. TC 51.3.6.9
      **
      **    This is similar to 1 but the PDU arrives before the current PDU has
      **    completed countdown, as opposed to case 1.
      **
      ** 4. if current PDU countdown procedure is on-going and un-ack mode is in force,
      **    then arrange for the condition where all blocks are transitted and without
      **    waiting for PUAN/EPUAN, send the PRR. No TC for this
      */

      /* case 3: un-ack mode and all blocks in TBF is transmitted. What about CV=0 ?
      */
      if (IS_EXT_UTBF_ACTIVE())
      {
        if (IS_TX_COMPLETE())
        {
          if ((ul.rlc_mode == GRLC_MODE_UNACK) ||
              ((ul.rlc_mode == GRLC_MODE_ACK) && (ul.va == ul.vs))
             )
          {
            /* case 1 and 2: send PRR, say RALS_COMPLETE
            */
            ul.realloc_pdu_ptr = next_pdu_ptr;
            ul.realloc_psn = next_pdu_ptr->psn;
            /* b_cv_zero - is set to TRUE , because we
               have Data in our Uplink Q.
            */
            realloc_for_next_pdu( ul.realloc_pdu_ptr,FALSE);

            /* Set RALS_COMPLETE once after sending a PRR.
            */
            ul.realloc_state = RALS_COMPLETE;
            MSG_GERAN_MED_2_G("GRLU prr CHANGE_MODE complete psn=%u realloc_psn=%u",
                    next_pdu_ptr->psn, ul.realloc_psn);
          }
          else
          {
            /* case 4: un-ack mode only. Expect PRR triggering in post_tx_actions()
            ** case 3: ack mode. Expect trigger in E/PUAN handling
            */
            ul.realloc_pdu_ptr = next_pdu_ptr;
            ul.realloc_psn = next_pdu_ptr->psn;

            ul.realloc_state = RALS_AWAIT_TX_COMPLETE;
            MSG_GERAN_MED_1_G("GRLU prr CHANGE_MODE 3/4 psn=%u",
                    next_pdu_ptr->psn);
          }
        } /* sub-states of RA_AT_TX_COMPLETE_CHANGE_MODE */
        else
        {
          /* case 3 & 4: load next PDU for the next realloc req
          ** ack mode: check VA=VS after each PUAN/EPUAN then send PRR ..
          ** un-ack mode: check in post_tx_actions when last block with CV=0 is
          **    sent in un-ack mode (PAK block), then set mask to signal trigger
          **    sending the PRR
          */
          /* Send PRR with parameters from the next PDU
          */
          ul.realloc_pdu_ptr = next_pdu_ptr;
          ul.realloc_psn = next_pdu_ptr->psn;

          MSG_GERAN_MED_1_G("GRLU prr CHANGE_MODE ack/un-ack not complete psn=%u",
                  next_pdu_ptr->psn);
        }
      }
      else
      {
        /* Not in EXT_UTBF. Let the TBF count down
        */
        /*
        */
        MSG_GERAN_MED_1_G("GRLU prr CHANGE_MODE non-ext No Realloc psn=%u",
                next_pdu_ptr->psn);

        cur_pdu_ptr->realloc_action = RA_AT_NEW_TBF;
        ul.realloc_state = RALS_COMPLETE;
      }
    } /* RA_AT_TX_COMPLETE_CHANGE_MODE */
    else
    {
      MSG_GERAN_LOW_1_G("GRLU prr bad realloc_state = %u",ul.realloc_state);
      MSG_GERAN_LOW_2_G("GRLU prr psn=%u next_psn=%u",cur_pdu_ptr->psn,next_pdu_ptr->psn);
    }
  } /* rals is correct or reallocation */
} /* check_to_send_prr */

/*===========================================================================
===
===  FUNCTION      SHOW_GUARD_TIMER_INFO()
===
===  DESCRIPTION
===
===    xxx
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===  COMMENTS
===
===========================================================================*/
void show_guard_timer_info (void)
{
  MSG_GERAN_MED_2_G("ERLU EXT_UTBF process guard timer sub-state = %u timer-state =%u",
    ul.guard_timer_state,
    ul.timer_await_ul_acknack.state);
}

/*===========================================================================
===
===  FUNCTION      PROCESS_GUARD_TIMER()
===
===  DESCRIPTION
===
===    This function maintains the sub-state machine for the Guard Timer in Extended
===    Uplink TBF mode. The sub-states are:
===
===    GUARD_OFF: initial state
===               timer starts when a BSN with CV=0 been sent (run in post_tx_actions)
===               state change to CV_ZERO
===
===    CV_ZERO:   - timer re-started when PUAN is received which advances Va
===               - timer re-started when Vseg = Va, ie. all BSNs been ack'ed (run after PUAN/CRes)
===               state change to GUARD_ON
===               - timer stopped when Vs > Va, ie. new blocks arrived
===               state change to GUARD_OFF
===               - timer stopped when sending reallocation request (GCF 41.3.6.10)
===
===    GUARD_ON:  - timer stopped when CV not zero, ie. new blocks arrived
===               state change to GUARD_OFF
===               - timer stopped when sending reallocation request (GCF 41.3.6.10)
===
===    When Guard Timer expires, abnormal release TBF
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
void process_guard_timer (void)
{
  switch (ul.guard_timer_state)
  {
    case GUARD_OFF:
      MSG_GERAN_LOW_0_G("ERLU XTimer GUARD_OFF .. no actions");
      show_guard_timer_info();
      break;

    case CV_ZERO:
      /* If UE in extended uplink TBF mode, and
      ** VA=VS=VSEG then re-start guard timer
      ** otherwise,
      ** if VS > VA, ie. new blocks have arrived then stop timer, as the new blocks
      ** will go pass the block with CV=0, again
      */
      MSG_GERAN_MED_3_G("ERLU XTimer CV_ZERO va=%u vs=%u vseg=%u",
                          ul.va,
                          ul.vs,
                          ul.seg_ind);
      if (IS_EXT_UTBF_ACTIVE())
      {
        if (ul.va == ul.vs && ul.vs == ul.seg_ind)
        {
          TIMER_START_AWAIT_UL_ACKNACK();
          MSG_GERAN_HIGH_0_G("ERLU XTimer started va=vseg CV_ZERO -> GUARD_ON");
          ul.guard_timer_state = GUARD_ON;
        }
        else
        {
          if (IS_GT_MODULO_COMP(ul.va, ul.va_old,
                                ((ul.cur_res.tbf_mode == TBF_MODE_EGPRS) ?
                                 ESNS :
                                 SNS
                                )
                               )
             )
          {
            MSG_GERAN_HIGH_2_G("ERLU XTimer re-started CV_ZERO va=%u old_va=%u",
              ul.va, ul.va_old);

            /* update the old VA value */
            ul.va_old = ul.va;
            TIMER_START_AWAIT_UL_ACKNACK();
          }
        }
      }
      show_guard_timer_info();
      break;

    case GUARD_ON:
      /* If VS > VA, ie. new blocks have arrived past the CV=0 block
      ** then STOP the timer. As it will go pass the block witgh CV=0 again ..
      ** Update state to GUARD_OFF
      */
      MSG_GERAN_MED_3_G("ERLU XTimer GUARD_ON va=%u vs=%u vseg=%u",
                          ul.va,
                          ul.vs,
                          ul.seg_ind);
      if (IS_GT_MODULO_COMP(ul.vs, ul.va,
                            ((ul.cur_res.tbf_mode == TBF_MODE_EGPRS) ?
                             ESNS :
                             SNS
                            )
                           )
         )
      {
        TIMER_STOP_AWAIT_UL_ACKNACK();
        MSG_GERAN_HIGH_0_G("ERLU XTimer stopped vs > va GUARD_ON > GUARD_OFF");
        ul.guard_timer_state = GUARD_OFF;
      }
      show_guard_timer_info();
      break;

    default:
      MSG_GERAN_LOW_1_G("ERLU XTimer bad state = %u", ul.guard_timer_state);
      break;
  }
} /* process_guard_timer */

/*===========================================================================
===
===  FUNCTION      FLUSH_LLC_PDU_QUEUE
===
===  DESCRIPTION
===
===    Clear the RLC UL pdu queue by reading each item and deallocate DSM
===    resources for each item.
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===  COMMENTS
===
===    Not used but we might re-surrect to implement Multi-RAT or Deactivation
===    handling where the remaining data PDUs may be purged
===
===========================================================================*/
void flush_llc_pdu_queue (boolean is_del_all_active)
{
  gprs_grr_ul_mes_t *pdu_ptr;

  /*----------------------------------------------------------------*/

  MSG_GERAN_MED_0_G("GRLU Info Flushed PDU queue");

  GRLC_UL_MSG_LOCK();

  /*-----------------------------------------------
  ** Free LLC PDUs in the temporary LLC PDU queue
  **-----------------------------------------------
  */
  pdu_ptr = (gprs_grr_ul_mes_t *)q_get(&ULTQ());
  while ( pdu_ptr != NULL )
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

    pdu_ptr = (gprs_grr_ul_mes_t *)q_get(&ULTQ());
  }

  /* free the cur_pdu_ptr
  */
  /* (2) no need to put it in the PAQ. This is to clear the PDU held in cur_pdu_ptr */
  if (cur_pdu_ptr != NULL)
  {
    free_llc_pdu_and_msg(cur_pdu_ptr);
    cur_pdu_ptr = NULL;
  }

  /* Look at what's in the queue
  */
  pdu_ptr = NULL;
  dequeue_pdu( &pdu_ptr);

  while (pdu_ptr != NULL)
  {

    /* Inform NAS when SM PDU is deleted, so it can be retried quickly,
    ** rather than waiting for NAS timeouts.
    */
    if( ( pdu_ptr->cause == GRR_CAUSE_SM_SIGNALLING ) && ( is_del_all_active ) )
    {
      indicate_to_llc_rlcmac_failure( pdu_ptr);
    }

    free_llc_pdu_and_msg(pdu_ptr);
    dequeue_pdu( &pdu_ptr);
  }

  if (next_pdu_ptr != NULL)
  {

    /* Inform NAS when SM PDU is deleted, so it can be retried quickly,
    ** rather than waiting for NAS timeouts.
    */
    if( ( next_pdu_ptr->cause == GRR_CAUSE_SM_SIGNALLING ) && ( is_del_all_active ) )
    {
      indicate_to_llc_rlcmac_failure(  next_pdu_ptr );
    }

    free_llc_pdu_and_msg(next_pdu_ptr);
    next_pdu_ptr = NULL;
  }

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

  RLC_UL_CLR_LLC_PDU_SIG();

  GRLC_UL_MSG_UNLOCK();

} /* flush_llc_pdu_queue */

/*===========================================================================
===
===  FUNCTION      GRLC_CLEAR_REALLOC_VAR
===
===  DESCRIPTION
===
===     Clears reallocation details held in "ul" structure.
===
===  PARAMS
===
===     None
===
===  DEPENDENCIES
===
===     None
===
===  RETURN VALUE
===
===     None
===
===  SIDE EFFECTS
===
===     None
===
===  COMMENTS
===
===
===========================================================================*/
void grlc_clear_realloc_vars (void)
{
  ul.realloc_pdu_ptr    = NULL;
  ul.realloc_state      = RALS_NULL;
  ul.realloc_psn        = 0;
  ul.realloc_octets_req = 0;
  ul.realloc_blocks_req = 0;
  ul.prev_realloc_psn = 0;
} /* grlc_clear_realloc_vars () */

/*===========================================================================
===
===  FUNCTION      GRLC_CLEAR_CUR_PDU_GROUPING_INFO
===
===  DESCRIPTION
===
===     A PDU being flushed may have been part of a group held in the 'cur_pdu_ptr'.
===     The grouping info such as pg_last_psn needs to be cleared (or made equal to the
===     curret psn value)
===
===
===  PARAMS
===
===     PSN number of the PDU being cleared
===
===  DEPENDENCIES
===
===     None
===
===  RETURN VALUE
===
===     None
===
===  SIDE EFFECTS
===
===     None
===
===  COMMENTS
===
===
===========================================================================*/
static void grlc_clear_cur_pdu_grouping_info (  uint16 psn)
{
  if (NULL != cur_pdu_ptr)
  {
    if (psn == cur_pdu_ptr->pg_last_psn)
    {
      MSG_GERAN_HIGH_2_G("GRLU pg_last_psn was %d, now changed to cur_pdu_ptr->psn = %u",
        cur_pdu_ptr->pg_last_psn, cur_pdu_ptr->psn);
      cur_pdu_ptr->pg_last_psn = cur_pdu_ptr->psn;
    }
  }
} /* grlc_clear_cur_pdu_grouping_info */

/*===========================================================================
===
===  FUNCTION      FLUSH_LLC_PDU_QUEUE_SAPI
===
===  DESCRIPTION
===
===    Clear the RLC UL PDU queue as per the SAPI from LLC.
===    we do not attempt to free cur_pdu_ptr as it will freed
===    under normal conditions using unput_paq and in gan/gttp
===    using the confirmation mechanism.
===
===  PARAMS
===     sapi
===
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===  COMMENTS
===
===
===========================================================================*/
static void flush_llc_pdu_queue_sapi (uint8 sapi_id)
{
  gprs_grr_ul_mes_t *c_pdu_ptr;
  gprs_grr_ul_mes_t *n_pdu_ptr;
  uint16            qcnt;

  MSG_GERAN_LOW_1_G("GRLU Info Flushed PDU queue sapi_id=%d",sapi_id);

  /* Purge based on sapi_id
  */
  /*-----------------------------------------------
  ** Free LLC PDUs in the temporary LLC PDU queue
  **-----------------------------------------------
  */

  GRLC_UL_MSG_LOCK();

  c_pdu_ptr = (gprs_grr_ul_mes_t *)q_check(&ULTQ());

  while ( c_pdu_ptr != NULL )
  {

    /* Get next PDU in case current one gets deleted. */
    n_pdu_ptr = (gprs_grr_ul_mes_t *)q_next( &ULTQ(), &c_pdu_ptr->link );

    MSG_GERAN_HIGH_3_G(" GRLU ULTQ: PDU cause =%d, SAPI = %d PDU Type = %d", c_pdu_ptr->cause, c_pdu_ptr->sapi, c_pdu_ptr->llc_pdu_type );

    if ( ( c_pdu_ptr->sapi         == sapi_id                  ) &&
         ( c_pdu_ptr->llc_pdu_type != GRR_LL_PDU_U_FR_SRES     )
       )
    {

      /* Remove PDU from ULTQ. */
      q_delete
        (
            &ULTQ(),
            &c_pdu_ptr->link
        );


      if (c_pdu_ptr->dup_pdu_ptr != NULL)
      {
        GPRS_PDU_FREE(&c_pdu_ptr->dup_pdu_ptr);
      }

      if (c_pdu_ptr->pdu_ptr != NULL)
      {
        GPRS_PDU_FREE(&c_pdu_ptr->pdu_ptr);
      }

      grlc_clear_cur_pdu_grouping_info( c_pdu_ptr->psn);

      /* now free the memory holding the LLC PDU message
      */
      GPRS_MEM_FREE(c_pdu_ptr);
    }

    /* Make next PDU the current one. */
    c_pdu_ptr = n_pdu_ptr;
  }

  GRLC_UL_MSG_UNLOCK();


  /* free the next_pdu_ptr
  */
  if (next_pdu_ptr == NULL)
  {
    MSG_GERAN_LOW_1_G("GRLU next_pdu_ptr is NULL sapi=%d", sapi_id);
  }
  else if ( ( next_pdu_ptr->sapi         == sapi_id                  ) &&
            ( next_pdu_ptr->llc_pdu_type != GRR_LL_PDU_U_FR_SRES     )
          )
  {

    /* If PDU held in next_pdu_ptr is the same as that held in
    ** ul.realloc_ptr, then clear reallocation parameters held in "ul"
    ** structure. Reallocation_action held in cur_pdu_ptr would be cleared
    ** at the end of this function.
    */
    if ( next_pdu_ptr == ul.realloc_pdu_ptr )
    {
      grlc_clear_realloc_vars();
    }

    grlc_clear_cur_pdu_grouping_info( next_pdu_ptr->psn);

    free_llc_pdu_and_msg(next_pdu_ptr);
    next_pdu_ptr = NULL;
  }
  else
  {
    MSG_GERAN_LOW_2_G("GRLU next_pdu_ptr not freed psn=%d sapi=%d",next_pdu_ptr->psn, sapi_id);
  }


  /* Flush the reserve Q */

  c_pdu_ptr = (gprs_grr_ul_mes_t *)q_check(&grlc_ul_old_llc_q);

  while ( c_pdu_ptr != NULL )
  {

    MSG_GERAN_LOW_3_G(" GRLU OldQ: PDU cause =%d, SAPI = %d, PDU Type =%d ", c_pdu_ptr->cause, c_pdu_ptr->sapi, c_pdu_ptr->llc_pdu_type );

    /* Get next PDU in case current one gets deleted. */
    n_pdu_ptr = (gprs_grr_ul_mes_t *)q_next(&grlc_ul_old_llc_q, &c_pdu_ptr->link );

    if ( ( c_pdu_ptr->sapi         == sapi_id                  ) &&
         ( c_pdu_ptr->llc_pdu_type != GRR_LL_PDU_U_FR_SRES     )
       )
    {

      /* Remove PDU from OldQ. */
      q_delete
        (
            &grlc_ul_old_llc_q,
            &c_pdu_ptr->link
        );

        /* If PDU held in reserve queue (pdu_ptr) is the same as that held in
        ** ul.realloc_ptr, then clear reallocation parameters held in "ul"
        ** structure. Reallocation_action would have already been cleared when
        ** relevant PDU was delinked in delink_data_pdu( ).
        */
      if (c_pdu_ptr == ul.realloc_pdu_ptr)
        {
          grlc_clear_realloc_vars();
        }

      if (c_pdu_ptr->dup_pdu_ptr != NULL)
        {
        GPRS_PDU_FREE(&c_pdu_ptr->dup_pdu_ptr);
        }

      if (c_pdu_ptr->pdu_ptr != NULL)
        {
        GPRS_PDU_FREE(&c_pdu_ptr->pdu_ptr);
        }
        /* now free the memory holding the LLC PDU message
        */
      GPRS_MEM_FREE(c_pdu_ptr);
      }

    /* Make next PDU the current one. */
    c_pdu_ptr = n_pdu_ptr;

    }


  /* Look at what's in the queue
  */
  c_pdu_ptr = NULL;
  qcnt = (uint16)q_cnt(&ULQ());

  while (qcnt != 0)
  {
    dequeue_pdu( &c_pdu_ptr);


    if ( c_pdu_ptr != NULL )
    {

      MSG_GERAN_LOW_3_G(" GRLU ULQ: PDU cause =%d, SAPI = %d, PDU Type =%d ", c_pdu_ptr->cause, c_pdu_ptr->sapi, c_pdu_ptr->llc_pdu_type );

      if ( ( c_pdu_ptr->sapi         == sapi_id                  ) &&
          /* Removed exclusion for LLC_SIGNALLING which will cover XID PDUs
           ( c_pdu_ptr->cause        != GRR_CAUSE_LLC_SIGNALLING ) &&
          */
           ( c_pdu_ptr->llc_pdu_type != GRR_LL_PDU_U_FR_SRES     )
         )
      {
        /* If PDU held in main queue (pdu_ptr) is the same as that held in
        ** ul.realloc_ptr, then clear reallocation parameters held in "ul"
        ** structure. If PDU is not deleted, Reallocation_action would be
        ** cleared via a call to group_pdu_init() below.
        */
        if (c_pdu_ptr == ul.realloc_pdu_ptr)
        {
          grlc_clear_realloc_vars();
        }

        grlc_clear_cur_pdu_grouping_info( c_pdu_ptr->psn);
        free_llc_pdu_and_msg(c_pdu_ptr);
      }
      else
      {
        group_pdu_init( c_pdu_ptr);

        (void)q_link(c_pdu_ptr, &c_pdu_ptr->link);
        q_put(&ULQ(), &c_pdu_ptr->link);

        MSG_GERAN_MED_3_G("GRLU enq psn=%u len=%u NpOctets=%08x",
          c_pdu_ptr->psn,
          c_pdu_ptr->len,
          grlc_ul_pack_queue_info());

#ifdef  GSM_GPRS_GRLU_LLC_PDU_TYPE
        grlc_ul_show_llc_pdu_type( c_pdu_ptr);
#endif
      }
    }
    qcnt--;
  }

  /* Set cur_pdu_ptr->realloc_action = RA_NULL and clear reallocation
  ** variables.
  */
  if ( cur_pdu_ptr != NULL )
  {
    cur_pdu_ptr->realloc_action = RA_NULL;
    grlc_clear_realloc_vars();
  }

  RLC_UL_CLR_LLC_PDU_SIG();

  if (NULL != cur_pdu_ptr)
  {
    MSG_GERAN_HIGH_3_G("GRLU cur-psn=%u last_p=%u pgstate=%d",
              cur_pdu_ptr->psn,
              cur_pdu_ptr->pg_last_psn,
              cur_pdu_ptr->pg_state);
    MSG_GERAN_HIGH_3_G("GRLU cur#blks=%u #blks=%u #pdus=%u",
              cur_pdu_ptr->pg_num_blks,
              cur_pdu_ptr->pg_num_blks_cur_pdu,
              cur_pdu_ptr->pg_pdus);
  }

} /* flush_llc_pdu_queue_sapi */


/*===========================================================================
===
===  FUNCTION      FLUSH_LLC_SIG_QUEUE
===
===  DESCRIPTION
===
===    Read out all LLC signalling messages and free them
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===  COMMENTS
===
===========================================================================*/
void flush_llc_sig_queue (void)
{
  gprs_grr_ul_mes_t *msg_ptr = NULL;

  while (q_cnt(&ULMQ()) > 0)
  {
    msg_ptr = (gprs_grr_ul_mes_t *)q_get(&ULMQ());
    if (msg_ptr != NULL)
    {
      GPRS_MEM_FREE( msg_ptr );
    }
  };

  RLC_UL_CLR_LLC_MSG_SIG();

} /* flush_llc_sig_queue */

/*===========================================================================
===
===  FUNCTION      FLUSH_MAC_SIG_QUEUE
===
===  DESCRIPTION
===
===    Read out all MAC messages and free them
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===  COMMENTS
===
===========================================================================*/
void flush_mac_sig_queue (void)
{
  mac_rlc_msg_t *msg_ptr = NULL;

  while (q_cnt(&UMQ()) > 0)
  {
    msg_ptr = (mac_rlc_msg_t *)q_get(&UMQ());
    if (msg_ptr != NULL)
    {
      GPRS_MEM_FREE( msg_ptr );
    }
  };

  RLC_UL_CLR_MAC_SIG();

} /* flush_mac_sig_queue */

/*===========================================================================
===
===  FUNCTION      FLUSH_TMR_SIG_QUEUE
===
===  DESCRIPTION
===
===    Read out all TIMER messages and free them
===
===  PARAMS
===
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===  COMMENTS
===
===========================================================================*/
void flush_tmr_sig_queue (void)
{
  rm_timer_msg_t *msg_ptr = NULL;

  while (q_cnt(&UTQ()) > 0)
  {
    msg_ptr = (rm_timer_msg_t *)q_get(&UTQ());
    if (msg_ptr != NULL)
    {
      GPRS_MEM_FREE( msg_ptr );
    }
  };

  RLC_UL_CLR_TMR_SIG();

} /* flush_tmr_sig_queue */


/*===========================================================================
===
===  FUNCTION      DETECT_ERROR_AND_RECOVER()
===
===  DESCRIPTION
===
===    If b_error_recovery_required is set then indicate abnormal release to MAC
===    and flush LLC PDUs and enter NULL state. If there are incoming LLC PDU then
===    re-initate uplink TBF
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
void detect_error_and_recover (void)
{
  if (ul.b_error_recovery_required)
  {
    MSG_GERAN_HIGH_0_G("GRLU ##error recovery");
    grlc_ul_show_vars();

    MSG_GERAN_HIGH_0_G("GRLU triggering panic reset");

    ul.b_error_recovery_required = FALSE;

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
      ERR_GERAN_FATAL_0_G("ERLU ERROR OCCURED - PERFORM PANIC RESET");
    }
  }
} /* detect_error_and_recover */

/*===========================================================================
===
===  FUNCTION      UPDATE_ACK_STATE_ARRAY
===
===  DESCRIPTION
===
===    Interpret the Receive Bitmap Block (RBB) & Starting Sequence Number
===    (SSN) in packet uplink acknack msg and update the Ack State Array V(B),
===    and the Ack State V(A).
===
===    The bitmap is arranged as follows (as per 11.0 and 12.3)
===
===    MSB                                LSB
===
===    b64  b63  b62  b61  b60  b59  b58  b57   Octet N + 0  RBB[0]
===    b56  b55  b54  b53  b52  b51  b50  b49   Octet N + 1
===    b48  b47  b46  b45  b44  b43  b42  b41   Octet N + 2
===    b40  b39  b38  b37  b36  b35  b34  b33   Octet N + 3
===
===    b32  b31  b30  b29  b28  b27  b26  b25   Octet N + 4
===    b24  b23  b22  b21  b20  b19  b18  b17   Octet N + 5
===    b16  b15  b14  b13  b12  b11  b10  b09   Octet N + 6
===    b08  b07  b06  b05  b04  b03  b02  b01   Octet N + 7  RBB[7]
===
===    Given a bitpos (1..64) the octet number is:
===    octet = 7 - ((bitpos-1)/8)
===
===    Given a bitpos (1..64) the mask is:
===    mask = 1 << ((bitpos-1)%8)
===
===    The BSN is then:
===    bsn = ssn - bitpos;
===
===    -------------------------------------------------------------------
===
===          BSN
===        |  0  |  1  |  2  |  3  |  4  |  5  |
===        |     |     |     |  ^  |     |     |
===                             |
===   bit64  <...  bit2  bit1   |____ssn
===
===  DEPENDENCIES
===
===    input
===    ul_acknack is the latest packet uplink acknack struct from peer entity
===    output
===    ack_state_arr_vb
===    ack_state_va
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/

void update_ack_state_array (void)
{
  /* loop count, index to the tx workspace
  */
  uint8     b, ind;

  /* the bsn value obtained from ssn and rbb of the packet uplink acknack
  */
  uint8     bsn;

  /* temp value of V(A)
  */
  uint8     va = 0;

  /* the (ack status) bit value associated with the bsn_value above
  */
  boolean   b_bit_value;

  /* Set to TRUE when V(A) requires update due to at least one of
  ** the blocks becoming ACK
  */
  boolean   b_to_update_va = FALSE;
  boolean   b_to_inc_n3102 = FALSE;

  uint8     offset, mask;

  boolean   b_psn_change = FALSE;
  uint16    psn, psn_to_free = 0;

  /* Ensure SSN is in range
  */
  if (!(IS_GE_MODULO_COMP(ul.vs,(ul.ul_acknack.ssn),SNS) &&
        IS_GE_MODULO_COMP(MOD_SNS(ul.ul_acknack.ssn-1),ul.va,SNS)
       )
     )
  {
    MSG_GERAN_LOW_3_G("GRLU ##OOR ssn=%u va=%u vs=%u",
             ul.ul_acknack.ssn,
             ul.va,
             ul.vs);
    return;
  }
  /*----------------------------------------------------------*/

  /* The rbb array contains 8 bytes comprising 64 status bits.
  ** The bit numbering here goes from 0 to 63 to represent bit 1 to 64.
  ** The bsn value is simply ssn - bit_position
  ** the status value associating with that bsn is held by the
  ** in the array.
  ** The index to the offset is simply bit_position/8
  ** The mask used to extract the bit value is calculated  as
  ** 0x80 >> ((b % 8) - 1)
  ** eg. bit 13 (from 1) falls into
  */

  /* Loop from WS to 1 inclusive. Reverve so that BSN starts from a low value
  */

  for (b=RM_RLC_WINDOW_SIZE; b >= 1; b--)
  {
    /* Work out the offset from the start of RBB and the mask given
    ** the bit position from 1 to WS
    */
    offset = 7 - ((b-1)/8);
    mask = 0x1 << ((b-1)%8);

    /* Work out the BSN value given bit position and the SSN
    */
    bsn = MOD_DIF(ul.ul_acknack.ssn, b, SNS);

    /* The check below fails when BSN = 0, VA = 0 and VS = 64. This results
    ** in item 0'th not being set to TXA_ACK. hence the check is removed
    ** at present.
    **
    ** Was if (IS_UL_BSN_VALID(bsn, (ul.vs-1), ul.va)) but when we try to account for
    ** PL1 miss-acknowledging PTX blocks, VS is late in advancing thereby preventing
    ** valid blocks from being ACKed by the network. So we use seg_ind to extend this
    ** window in those cases
    */
    if (IS_UL_BSN_VALID(bsn, (ul.seg_ind-1), ul.va))
    {
      b_bit_value = ul.ul_acknack.rbb[offset] & mask;
      if (b_bit_value)
      {
        /* Mark the block ACK
        */
        GRLC_UL_TX_VARS_LOCK();
        TX_ARR( bsn).vb = TXA_ACK;
        GRLC_UL_TX_VARS_UNLOCK();

        MSG_GERAN_LOW_1_G("GRLU upd ACK %d", bsn);
        /* Free the dsm_ptr after TXA_ACK is received
        */

        if (TX_ARR( bsn).blk.dup_dsm_ptr != NULL)
        {
          MSG_GERAN_LOW_1_G("GRLU ##update tx-dup not cleared at bsn=%d",bsn);
          GPRS_PDU_FREE(&(TX_ARR( bsn).blk.dup_dsm_ptr));
        }

        if (TX_ARR( bsn).blk.dsm_ptr != NULL)
        {
          GPRS_PDU_FREE(&(TX_ARR( bsn).blk.dsm_ptr));
        }

        /* Mark it so V(A) can be updated outside this loop
        */
        if (IS_GE_MODULO_COMP(bsn,ul.va,SNS))
        {
          b_to_update_va = TRUE;
          b_to_inc_n3102 = TRUE;
        }
      }
      else
      {
        /* Bit value is '0' but can not assign TXA_NACK until the block period
        ** between the end of the block transmitted and the beginning of the
        ** packet uplink acknack is greater than MAX(BS_CV_MAX,1)-1
        ** And then only if the block's ack status is TXA_PACK, ie. already been
        ** acknowledged
        */
        GRLC_UL_TX_VARS_LOCK();
        if ((TX_ARR( bsn).vb == TXA_PACK ||
             TX_ARR( bsn).vb == TXA_PENDING_PACK
            ) &&
            IS_GT_MODULO_COMP((ul.acknack_blk_cnt), (TX_ARR( bsn).blk_cnt), 32768)
           )
        {
          /* ! is there a need to check if the block status
          ** is PACK as well before setting it to NACKED.
          ** Because if a block has not been transmitted then
          ** it couldn't be NACKED.
          */
          TX_ARR( bsn).vb = TXA_NACK;
  
          /* If NACK block is behind nack_vsf then update nack_vsf
          */
          if (IS_GT_MODULO_COMP(ul.nack_vsf, bsn, SNS))
          {
            /*MSG_HIGH("GRLU upd NACK %d upd nack_vsf (was %d)",bsn,ul.nack_vsf,0);*/
            ul.nack_vsf = bsn;
            UPD_UL_NACK_VSF();
          }
  
          /*MSG_HIGH("GRLU upd NACK %d nack_ctr=%d", bsn,grlc_ul_fifo.nack_ctr,0);*/
        }
        GRLC_UL_TX_VARS_UNLOCK();
      }
    }
    /*--------------------------------------------------------------------
    ** 'else' case:
    ** Even though the spec says the blocks that fall outside are to
    ** be marked INVALID (9.1.8 At the RLC transmitter). We simply ignore
    ** these blocks instead. Because in our implementation we use the
    ** blocks from V(S) to seg_ind to store segmented blocks and we
    ** mark these blocks as PENDING_TX. We can not overwrite PENDING_TX
    ** with INVALID.
    **--------------------------------------------------------------------
    */
  } /* for loop */

  if (b_to_update_va)
  {
    /* Get existing PSN to free
    */
    psn = ul.first_psn_to_free;
    va = (uint8)ul.va;
    ul.pdu_freeing_state = NONE_TO_FREE;

    for (b=0; b<(int8)GET_TX_WINDOW_SIZE(); b++)
    {
      ind = MOD_SNS(ul.va+b);

      MSG_GERAN_LOW_3_G("ELRU ind=%u 1st=%u last=%u",
              ind,
              TX_ARR( ind).blk.first_psn,
              TX_ARR( ind).blk.last_psn
             );

      if (TX_ARR( ind).vb == TXA_ACK)
      {
        va = MOD_SNS(ind+1);

        /* PSN change detected. Free all PDUs in the PAQ inclusively
        */
        if (IS_GT_MODULO_COMP(TX_ARR( ind).blk.last_psn, TX_ARR( ind).blk.first_psn, 32768))
        {
          b_psn_change = TRUE;

          psn_to_free = psn;
          psn = TX_ARR( ind).blk.last_psn;
          DEC_MOD(psn, 32768);
          ul.psn_to_free_inclusive = psn;
        }
        else
        {
          if (TX_ARR( ind).blk.last_psn == TX_ARR( ind).blk.first_psn)
          {
            /* Only free the PDU if CV is zero and the PSN of current block really ends here 
            ** ie. PSN of the next block is different to PSN of the current block
            */
            if ( (TX_ARR( ind).blk.mac_hdr.type.ul_data.cv == 0) &&
                 (IS_GT_MODULO_COMP(ul.seg_ind, ind, SNS)) &&  /* next block exists */
                 (TX_ARR((MOD_SNS(ind+1))).blk.first_psn != TX_ARR( ind).blk.last_psn)
               )
            {
              b_psn_change = TRUE;

              psn_to_free = psn;
              psn = TX_ARR( ind).blk.last_psn;
              ul.psn_to_free_inclusive = psn;
              MSG_GERAN_HIGH_2_G("ELRU upd CV 0 ind %u psn to free %u", ind, psn);
            }
          }
          else
          {
            /* Otherwise free the PSN previous to the current PSN */
            b_psn_change = TRUE;

            psn_to_free = psn;
            psn = TX_ARR( ind).blk.last_psn;
            DEC_MOD(psn, 32768);
            ul.psn_to_free_inclusive = psn;
            MSG_GERAN_HIGH_3_G("ERLU upd CV=%u ind %u psn to free %u",
                TX_ARR( ind).blk.mac_hdr.type.ul_data.cv,
                ind,
                psn);
          }
        }
      }
      else
      {
        break;
      }
    } /* end for */

    if (b_psn_change)
    {
      if (ul.psn_to_free_inclusive == cur_pdu_ptr->psn)
      {
        ul.pdu_freeing_state = FREE_CURRENT_PDU;
        psn_to_free = ul.psn_to_free_inclusive;
        DEC_MOD(psn_to_free, 32768);
      }
      else
      {
        ul.pdu_freeing_state = DONE_FREEING;
        psn_to_free = ul.psn_to_free_inclusive;
      }

      /* MSG_GERAN_MED_3_G("GRLU upd del & free upto %u va=%d upd 1st_psn=%u",
          psn_to_free,
          ul.va,
          ul.first_psn_to_free);
      */
      /* Successful transmission of PDU , set the flag to TRUE
      ** to start ready timer
      */
      grlc_ul_del_free_paq_upto_psn( psn_to_free,TRUE);
    }

    GRLC_UL_TX_VARS_LOCK();

    ul.va = va;
    UPD_UL_VA();

    /*----------------------------------------------------------------------
    ** If V(A) moves past vsf, nack_vsf or pack_vsf then bring these values
    ** to V(A)
    **----------------------------------------------------------------------
    */
    if (IS_GT_MODULO_COMP(ul.va, ul.vsf, SNS))
    {
      ul.vsf = ul.va;
      UPD_UL_VSF();
    }

    if (IS_GT_MODULO_COMP(ul.va, ul.nack_vsf, SNS))
    {
      ul.nack_vsf = ul.va;
      UPD_UL_NACK_VSF();
    }

    if (IS_GT_MODULO_COMP(ul.va, ul.pack_vsf, SNS))
    {
      ul.pack_vsf = ul.va;
      UPD_UL_PACK_VSF();
    }

    GRLC_UL_TX_VARS_UNLOCK();

    /*---------------------------------------------------------
    ** VA has changed so check and un-stall window if required
    **---------------------------------------------------------
    */
    if (ul.si == RM_TX_STALLED &&
        !IS_TX_WINDOW_FULL()
       )
    {
      TIMER_STOP_T3182();
      ul.si = RM_TX_NOT_STALLED;
#if defined(GSM_GPRS_GRLU_SHOW_T3182) || defined(GSM_GPRS_GRLU_SHOW_SI)
      MSG_GERAN_MED_3_G("GRLU upd T3182 stopped SI=0 va=%d vs=%d seg=%d",ul.va,ul.vs,ul.seg_ind);
#endif
    }
  }

  /* If tx window becomes un-stall OR V(A) has been advanced then increment
  ** the N3102 counter
  */
  if (IS_N3102_ENABLED())
  {
    if (!IS_TX_WINDOW_FULL() ||
        b_to_inc_n3102
       )
    {
      INC_N3102();
#ifdef  GSM_GPRS_GRLU_SHOW_N3102
      MSG_GERAN_MED_3_G("GRLU upd IncN3102=%d max=%d inc=%d",ul.n3102,ul.pan_max_value,PAN_INC());
#endif
    }
  }
} /* update_ack_state_array */

/*===========================================================================
===
===  FUNCTION      E_ACK_ACTION
===
===  DESCRIPTION
===
===    This function performs the appropriate actions to positively acknowledge
===    a data block or negatively acknowledge a data block. The PUAN interpreting
===    function E_UPDATE_ACK_STATE_ARRAY() should call grlc_t4_decode() if a compressed
===    bitmap is present. A pointer to this function should be passed to grlc_t4_decode()
===    For each bit decoded, grlc_t4_decode() shall invoke this function to perform ACK
===    or NACK actions as required.
===
===    Positive ack actions:
===     - Mark state variable V(B),
===     - Free dsm pointer(s)
===     - Check if V(A) should be updated
===     - etc ..
===    Negative ack actions:
===     - Check the ack state and block counter whether negative ack should be accepted
===       and acted upon. Not if block counter has not expired
===     - Mark state variable V(B)
===     - Update Nack state variable (nack_vsf)
===     - etc ..
===
===  PARAMS
===     pos_rel_ssn: block position relative to SSN
===     ack_value: 1 or 0
===
===  DEPENDENCIES
===
===    TX_ARR and all variables
===
===  RETURN VALUE
===
===    boolean
===
===  SIDE EFFECTS
===
===========================================================================*/
static void e_ack_action
(
  const uint16  pos_rel_ssn,
  const uint16  ack_value
)
{
  uint16  bsn = MOD_ESNS(pos_rel_ssn + ul.egprs_acknack_desc.ssn);

  if (IS_UL_BSN_VALID_IN_WINDOW( bsn))            /* use ul.va */
  {
    /* shift right to divide by 32
    */
    if (ack_value)
    {
      GRLC_UL_TX_VARS_LOCK();
      TX_ARR( bsn).vb = TXA_ACK;
      GRLC_UL_TX_VARS_UNLOCK();

      TX_ARR( bsn).blk.mcs_demoted_ps = NO_MCS_DEMOTION;

#ifdef  GSM_GPRS_ERLU_SHOW_EPUAN
      MSG_GERAN_LOW_1_G("ERLU e_upd ACK %u",bsn);
#endif

      /* Warn and free dup-dsm if it is not already freed. Because it should
      ** have been freed by the transmission process
      */
      if (TX_ARR( bsn).blk.dup_dsm_ptr != NULL)
      {
        GPRS_PDU_FREE(&TX_ARR( bsn).blk.dup_dsm_ptr);
        MSG_GERAN_LOW_1_G("ERLU e_upd dup ! clr at BSN %u",bsn);
      }

      /* Free the original dsm item
      */
      if (TX_ARR( bsn).blk.dsm_ptr != NULL)
      {
        GPRS_PDU_FREE(&TX_ARR( bsn).blk.dsm_ptr);
      }
    } /* bit is 1 */
    else
    {
      /* Bit value is '0' but can not assign TXA_NACK until the block period
      ** between the end of the block transmitted and the beginning of the
      ** packet uplink acknack is greater than MAX(BS_CV_MAX,1)-1
      ** And then only if the block's ack status is TXA_PACK, ie. already been
      ** acknowledged
      */
      GRLC_UL_TX_VARS_LOCK();
      if ((TX_ARR( bsn).vb == TXA_PACK ||
           TX_ARR( bsn).vb == TXA_PENDING_PACK
          ) &&
          /* Special case. Using GPRS' macro to do modulo-65536 comparison
          */
          IS_GT_MODULO_COMP((ul.acknack_blk_cnt), (TX_ARR( bsn).blk_cnt), 32768)
         )
      {
        /* ! is there a need to check if the block status
        ** is PACK as well before setting it to NACKED.
        ** Because if a block has not been transmitted then
        ** it couldn't be NACKED.
        */
        TX_ARR( bsn).vb = TXA_NACK;

        /* If NACK block is behind nack_vsf then update nack_vsf
        */
        if (IS_GT_MOD(ul.nack_vsf, bsn, ul.cur_res.ws))
        {
          /*MSG_HIGH("GRLU upd NACK %d upd nack_vsf (was %d)",bsn,ul.nack_vsf,0);*/
          ul.nack_vsf = bsn;
          UPD_UL_NACK_VSF();
        }

#ifdef  GSM_GPRS_ERLU_SHOW_EPUAN
        MSG_GERAN_LOW_2_G("GRLU upd NACK %d nack_ctr=%d", bsn, grlc_ul_fifo.nack_ctr);
#endif
      } /* check if bit = 0 would result in a negative ack */
      GRLC_UL_TX_VARS_UNLOCK();
    } /* bit is 0 */
  } /* BSN is valid */

#ifdef  GSM_GPRS_ERLU_SHOW_EPUAN
  MSG_GERAN_LOW_2_G("ERLU e_ack pos_rel_ssn=%u ack=%u", pos_rel_ssn, ack_value);
#endif

} /* e_ack_action */

/*===========================================================================
===
===  FUNCTION      E_UPDATE_ACK_STATE_ARRAY
===
===  DESCRIPTION
===
===    Interpret the EGPRS packet uplink acknack description and update the
===    transmit V(B) array.
===
===  DEPENDENCIES
===
===    input
===    ul_acknack is the latest packet uplink acknack struct from peer entity
===    output
===    ack_state_arr_vb
===    ack_state_va
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void e_update_ack_state_array (void)
{
  uint16  va,                 /* default to current value of V(A) */
          psn,                /* temp PSN */
          psn_to_free = 0,
          start_eow_bsn,      /* start_eow_bsn */
          bits_decoded,
          loop_cnt,
          orig_va,            /* holds value of ul.va */
          ssn;                /* SSN value in progress */

  boolean b_to_update_va  = FALSE,  /* temp flags */
          b_to_inc_n3102  = FALSE,
          b_psn_change    = FALSE;

  uint32 mask = 1L;           /* mask to extract bitmap */
  int16  i;                   /* loop counter. Use signed to detect negative value */
  uint16 bsn = 0;             /* working BSN value */
  uint16 ind;                 /* index the CRBB or URBB */

  /*------------------------------------------------------------------
  ** Set temp V(A) to actual ul.va and ssn value given by the network
  **------------------------------------------------------------------
  */
  orig_va = ul.va;
  va = ul.va;
  ssn = ul.egprs_acknack_desc.ssn;

  /*----------------------------------------------------------------------------
  ** Check the length of the un-compressed bitmap calculated by the CSN module.
  ** If it is exceeded the current window size then force it to the value of
  ** the window size and emit an error message
  **----------------------------------------------------------------------------
  */
  if (ul.egprs_acknack_desc.urbb_len > ul.cur_res.ws)
  {
    MSG_GERAN_LOW_2_G("ERLU e_upd invalid urbb_len %u. Forced to WS %u",
        ul.egprs_acknack_desc.urbb_len,
        ul.cur_res.ws
        );
    ul.egprs_acknack_desc.urbb_len = (uint8)ul.cur_res.ws;
  }

  /*--------------------------------------------------------------------------
  ** If BOW is set, positively ack all blocks between V(A) to SSN-2 inclusive
  **--------------------------------------------------------------------------
  */
  if (ul.egprs_acknack_desc.bow)
  {
    /* Loop i times where i is the distance between SSN-2 and V(A) inclusive.
    ** Note MOD_ESNS(ul.ssn+ESNS-2) performs [SSN-2] mod 2048.
    */
    for ( i=0;
          /* loop only if V(A) is behind SSN-2 */
          IS_GE_MOD(MOD_ESNS(ssn+ESNS-2),orig_va,ul.cur_res.ws) &&

          /* distance between V(A) and SSN-2 */
          i <= MOD_DIF_ESNS(MOD_ESNS(ssn+ESNS-2),orig_va);
          i++
        )
    {
      bsn = MOD_ESNS(orig_va+(uint16)i);

      /* If BSN is in range and not yet ACK'ed then update
      */
      if (IS_UL_BSN_VALID_IN_WINDOW(bsn))   /* this macro uses ul.va which is OK */
      {
        GRLC_UL_TX_VARS_LOCK();
        if (TX_ARR( bsn).vb != TXA_ACK)                      /* ..and not yet ACK */
        {
          TX_ARR( bsn).vb = TXA_ACK;
          MSG_GERAN_LOW_3_G("ERLU e_upd BOW=1 ACK %u va=%u vs=%u", bsn, va, ul.vs);

          TX_ARR( bsn).blk.mcs_demoted_ps = NO_MCS_DEMOTION;

          /* A dup item is cleared following a transmission. Warn and free it if
          ** it is not already freed
          */
          if (TX_ARR( bsn).blk.dup_dsm_ptr != NULL)
          {
            GPRS_PDU_FREE(&TX_ARR( bsn).blk.dup_dsm_ptr);
            MSG_GERAN_LOW_1_G("GRLU e_upd dup ! clr at BSN %u", bsn);
          }

          /* Free the original dsm item
          */
          if (TX_ARR( bsn).blk.dsm_ptr != NULL)
          {
            GPRS_PDU_FREE(&TX_ARR( bsn).blk.dsm_ptr);
          }

          if (IS_GT_MOD(ul.vsf, va, ul.cur_res.ws))
          {
            ul.va = MOD_ESNS(bsn+1);
            va = ul.va;
          }

          b_to_update_va = TRUE;
          b_to_inc_n3102 = TRUE;                      /* because V(A) has changed */

        } /* ACK'able */
        GRLC_UL_TX_VARS_UNLOCK();

      } /* BSN is indeed between V(A) and SSN-2 */
      else
      {
        /* BSN is out of range
        */
        MSG_GERAN_HIGH_3_G("ERLU e_upd BOW=1 BSN oor va=%u bsn=%u vs=%u",va, bsn, ul.vs);
      }
    } /* for loop ACK'ing blocks for BOW bit */

    /*------------------------------------------------
    ** Nack block at SSN-1 because the BOW bit is set
    **------------------------------------------------
    */
    bsn = MOD_ESNS(ssn+ESNS-1);

    GRLC_UL_TX_VARS_LOCK();
    if ((TX_ARR( bsn).vb == TXA_PACK ||
         TX_ARR( bsn).vb == TXA_PENDING_PACK
        ) &&
        /* Special case. Using GPRS to do modulo-65536 comparison
        */
        IS_GT_MODULO_COMP((ul.acknack_blk_cnt), (TX_ARR( bsn).blk_cnt), 32768)
       )
    {

      MSG_GERAN_MED_3_G("ERLU e_upd BOW=1 NACK SSN-1 global blk cnt=%u [%04x].blk_cnt = %u",
              ul.acknack_blk_cnt,
              bsn,
              TX_ARR( bsn).blk_cnt);
      /* ! is there a need to check if the block status
      ** is PACK as well before setting it to NACKED.
      ** Because if a block has not been transmitted then
      ** it couldn't be NACKED.
      */
      TX_ARR( bsn).vb = TXA_NACK;

      /* If NACK block is behind nack_vsf then update nack_vsf
      */
      if (IS_GT_MOD(ul.nack_vsf, bsn, ul.cur_res.ws))
      {
        MSG_GERAN_MED_2_G("ERLU e_upd NACK %u upd nack_vsf (was %u)",bsn, ul.nack_vsf);
        ul.nack_vsf = bsn;
        UPD_UL_NACK_VSF();
      }

      MSG_GERAN_MED_3_G("ERLU e_upd BOW=1 NACK SSN-1 %d va=%u vs=%u", bsn,va,ul.vs);

    } /* check if bit = 0 would result in a negative ack */
    GRLC_UL_TX_VARS_UNLOCK();

  } /* end BOW processing */

  /* orig_va holds original ul.va
  ** ul.va gets updated and va follows ul.va
  */

  /*---------------------------------------------------------------------------
  ** If the CRBB is present then decode it and process the ack for each blocks
  **---------------------------------------------------------------------------
  */
  if (ul.egprs_acknack_desc.comp_present)
  {
    /* If the compressed flag is set, un-compress the CRBB and perform the appropriate
    ** ack-ing and nack-ing of each data blocks
    */
    if (grlc_t4_decode(
                       &(ul.egprs_acknack_desc.crbb[0]),
                       e_ack_action,
                       ul.egprs_acknack_desc.scc,
                       ul.egprs_acknack_desc.crbb_len,
                       &bits_decoded
                      )
       )
#ifndef  GSM_GPRS_ERLU_SHOW_EPUAN
    {
    }
#else
    {
      MSG_GERAN_LOW_1_G("ERLU e_upd t4_dec rets OK bits_decoded=%u", bits_decoded);
    }
    else
    {
      MSG_GERAN_LOW_1_G("ERLU e_upd t4_dec rets FAILED bits_decoded=%u", bits_decoded);
    }
#endif

    /* Update SSN to the block following the last bit in the CRBB, ie ESN_CRBB+1
    */
    ssn = MOD_ESNS(ssn + bits_decoded);

    /* Init BSN value for EOW operation, in case URBB length is 0
    */
    bsn = MOD_ESNS(ssn+ESNS-1);

#ifdef  GSM_GPRS_ERLU_SHOW_EPUAN
    MSG_GERAN_LOW_2_G("ERLU e_upd t4_dec rets bits_decoded=%u ssn=%u", bits_decoded, ssn);
#endif

    /* If CRBB exists then blindly assunme that V(A) and N3102 need updating
    */
    b_to_update_va = TRUE;
    b_to_inc_n3102 = TRUE;

  } /* CRBB (comp_present) */

  /* ssn gets updated by the CRBB process.
  ** ul.va gets the updated value after BOW processing and va follows ul.va
  ** b_to_update_va is set
  */

  /*----------------------------------------------------------
  ** Loop to update the URBB.
  **
  ** URBB[0]     b32 b31 .. b8  b7  b6  b5  b4  b3  b2  b1
  **                                                    SSN+0
  **                                                SSN+1
  **                                            SSN+2
  **                         ....
  ** URBB[1]
  **                         ....
  **----------------------------------------------------------
  */
  mask = 1L;
  for (i=0; i<ul.egprs_acknack_desc.urbb_len; i++)
  {
    /* Use ul.va instead of va (temporary)
    */

    /* calc abslolute BSN value relative to SSN
    */
    bsn = MOD_ESNS(ssn + (uint16)i);

    if (IS_UL_BSN_VALID_IN_WINDOW( bsn))
    {
      /* shift right to divide by 32
      */
      if (ul.egprs_acknack_desc.urbb[(uint16)i >> 5] & mask)
      {
        GRLC_UL_TX_VARS_LOCK();
        TX_ARR(bsn).vb = TXA_ACK;
        GRLC_UL_TX_VARS_UNLOCK();

        TX_ARR(bsn).blk.mcs_demoted_ps = NO_MCS_DEMOTION;

#ifdef  GSM_GPRS_ERLU_SHOW_EPUAN
        MSG_GERAN_LOW_3_G("ERLU e_upd URBB ACK %u va=%u vs=%u",bsn,ul.va,ul.vs);
#endif
        /*---------------------------------------------
        ** Clear DSM memory items after a positive ACK
        **---------------------------------------------
        */

        /* Warn and free dup-dsm if it is not already freed. Because it should
        ** have been freed by the transmission process
        */
        if (TX_ARR(bsn).blk.dup_dsm_ptr != NULL)
        {
          GPRS_PDU_FREE(&TX_ARR(bsn).blk.dup_dsm_ptr);
          MSG_GERAN_LOW_1_G("GRLU e_upd URBB dup ! clr at BSN %u",bsn);
        }

        /* Free the original dsm item
        */
        if (TX_ARR(bsn).blk.dsm_ptr != NULL)
        {
          GPRS_PDU_FREE(&TX_ARR(bsn).blk.dsm_ptr);
        }

        /* If either update flags are set then check to set them so ul.va and
        ** N3102 counter can be updated later on
        */
        if (!b_to_update_va || !b_to_inc_n3102)
        {
          if (IS_GE_MOD(bsn, ul.va, ul.cur_res.ws))
          {
            b_to_update_va = TRUE;
            b_to_inc_n3102 = TRUE;
          }
        }
      } /* bit is 1 */
      else
      {
        /* Bit value is '0' but can not assign TXA_NACK until the block period
        ** between the end of the block transmitted and the beginning of the
        ** packet uplink acknack is greater than MAX(BS_CV_MAX,1)-1
        ** And then only if the block's ack status is TXA_PACK, ie. already been
        ** acknowledged
        */
        GRLC_UL_TX_VARS_LOCK();
        if ((TX_ARR(bsn).vb == TXA_PACK ||
             TX_ARR(bsn).vb == TXA_PENDING_PACK
            ) &&
            /* Special case. Using GPRS function to do modulo-65536 comparison
            */
            IS_GT_MODULO_COMP((ul.acknack_blk_cnt), (TX_ARR(bsn).blk_cnt), 32768)
           )
        {
          MSG_GERAN_MED_3_G("ERLU e_upd URBB NACK SSN-1 global blk cnt=%u [%u].blk_cnt = %u",
                  ul.acknack_blk_cnt,
                  bsn,
                  TX_ARR(bsn).blk_cnt);
          /* ! is there a need to check if the block status
          ** is PACK as well before setting it to NACKED.
          ** Because if a block has not been transmitted then
          ** it couldn't be NACKED.
          */
          TX_ARR(bsn).vb = TXA_NACK;

          /* If NACK block is behind nack_vsf then update nack_vsf
          */
          if (IS_GT_MOD(ul.nack_vsf, bsn, ul.cur_res.ws))
          {
            MSG_GERAN_LOW_2_G("ERLU e_upd URBB NACK %d upd nack_vsf (was %d)", bsn, ul.nack_vsf);
            ul.nack_vsf = bsn;
            UPD_UL_NACK_VSF();
          }

#ifdef  GSM_GPRS_ERLU_SHOW_EPUAN
          MSG_GERAN_LOW_2_G("ERLU e_upd URBB NACK %d nack_vsf=%u", bsn, ul.nack_vsf);
#endif
        } /* check if bit = 0 would result in a negative ack */
        GRLC_UL_TX_VARS_UNLOCK();
      } /* bit is 0 */
    } /* BSN is valid */
    /*--------------------------------------------------------------------
    ** 'else' case:
    ** Even though the spec says the blocks that fall outside are to
    ** be marked INVALID (9.1.8 At the RLC transmitter). We simply ignore
    ** these blocks instead. Because in our implementation uses the
    ** blocks from V(S) to seg_ind to store segmented blocks and we
    ** mark these blocks as PENDING_TX. We can not overwrite PENDING_TX
    ** with INVALID.
    **--------------------------------------------------------------------
    */
    else
    {
      MSG_GERAN_LOW_1_G("GRLU e_upd BSN %u OOR", bsn);
    }

    /* Rotate to the next bit and re-start at 1 when the 32nd bit is drop out of the MSB
    */
    mask <<= 1;
    if (mask == 0L)
      mask = 1L;

  } /* inner mask rotating loop */

  /* Last BSN addressed by the bitmap is preserved here because the EOW bit starts
  ** from this BSN value
  */
  start_eow_bsn = MOD_ESNS(bsn+1);

  /*------------------------------------------------------------------------
  ** Deal with End-Of-Window bit. If set, blocks between the last BSN value
  ** addressed by the URBB plus one and V(S)-1 are assumed a value of '0'.
  **------------------------------------------------------------------------
  */
  if (ul.egprs_acknack_desc.eow)
  {
    for ( i=0;
          /* do this only if the block following the end of the bitmap is behind V(S) */
          IS_GT_MOD(ul.vs, start_eow_bsn, ul.cur_res.ws) &&
          /* distance to V(S) */
          i < MOD_DIF_ESNS(ul.vs, start_eow_bsn);
          i++
        )
    {
      bsn = MOD_ESNS(start_eow_bsn + (uint16)i);

      if (IS_UL_BSN_VALID_IN_WINDOW(bsn))
      /* Bit value is '0' but can not assign TXA_NACK until the block period
      ** between the end of the block transmitted and the beginning of the
      ** packet uplink acknack is greater than MAX(BS_CV_MAX,1)-1
      ** And then only if the block's ack status is TXA_PACK or PENDING_PACK
      */
      {
        GRLC_UL_TX_VARS_LOCK();
        if ((TX_ARR(bsn).vb == TXA_PACK ||
             TX_ARR(bsn).vb == TXA_PENDING_PACK
            ) &&
                /* Special case. Using GPRS macro to do modulo-65536 comparison */
            IS_GT_MODULO_COMP((ul.acknack_blk_cnt), (TX_ARR( bsn).blk_cnt), 32768)
           )
        {
          TX_ARR( bsn).vb = TXA_NACK;

          /* If NACK block is behind nack_vsf then update nack_vsf
          */
          if (IS_GT_MOD(ul.nack_vsf, bsn, ul.cur_res.ws))
          {
            /*MSG_HIGH("GRLU upd NACK %d upd nack_vsf (was %d)",bsn,ul.nack_vsf,0);*/
            ul.nack_vsf = bsn;
            UPD_UL_NACK_VSF();
          }

#ifdef  GSM_GPRS_ERLU_SHOW_EPUAN
          MSG_GERAN_LOW_2_G("ERLU upd EOW=1 NACK %d nack_ctr=%d", bsn, grlc_ul_fifo.nack_ctr);
#endif
        } /* check if bit = 0 would result in a negative ack */
        GRLC_UL_TX_VARS_UNLOCK();
      } /* end valid BSN in window */
      else
      {
#ifdef  GSM_GPRS_ERLU_SHOW_EPUAN
        MSG_GERAN_MED_2_G("ERLU upd EOW=1 can't NACK %d nack_ctr=%d", bsn, grlc_ul_fifo.nack_ctr);
#endif
      }
    }
  } /* end EOW processing */

  /*-----------------------------------------------------------
  ** Update V(A) and also free any PDUs along the path of V(A)
  **-----------------------------------------------------------
  */
  if (b_to_update_va)
  {
    /* temp 'va' already set to ul.va at the beginning and updated when BOW is set
    ** orig_va holds the original value of ul.va prior to entering this function
    */

    /* Loop from ul.va to V(S)
    */
    loop_cnt = MOD_DIF_ESNS(ul.vs, ul.va);

    for (i=0; i<loop_cnt; i++)
    {
      ind = MOD_ESNS(ul.va + (uint16)i);

      if (TX_ARR(ind).vb == TXA_ACK)
      {
        va = MOD_ESNS(ind+1);
      }
      else
      {
        /* Exit loop at the very first non-ACK block. Temp va carries the
        ** value to update V(A) with
        */
        break;
      }
    } /* end for */

    GRLC_UL_TX_VARS_LOCK();

    ul.va = va;
    UPD_UL_VA();

    /*----------------------------------------------------------------------
    ** If V(A) moves past vsf, nack_vsf or pack_vsf then bring these values
    ** to V(A)
    **----------------------------------------------------------------------
    */
    if (IS_GT_MOD(ul.va, ul.vsf, ul.cur_res.ws))
    {
      ul.vsf = ul.va;
      UPD_UL_VSF();
    }

    if (IS_GT_MOD(ul.va, ul.nack_vsf, ul.cur_res.ws))
    {
      ul.nack_vsf = ul.va;
      UPD_UL_NACK_VSF();
    }

    if (IS_GT_MOD(ul.va, ul.pack_vsf, ul.cur_res.ws))
    {
      ul.pack_vsf = ul.va;
      UPD_UL_PACK_VSF();
    }

    GRLC_UL_TX_VARS_UNLOCK();

    /*---------------------------------------------------------
    ** VA has changed so check and un-stall window if required
    **---------------------------------------------------------
    */
    if (ul.si == RM_TX_STALLED &&
        !IS_E_TX_WINDOW_FULL()
       )
    {
      TIMER_STOP_T3182();
      ul.si = RM_TX_NOT_STALLED;
#if defined(GSM_GPRS_GRLU_SHOW_T3182) || defined(GSM_GPRS_GRLU_SHOW_SI)
      MSG_GERAN_MED_3_G("GRLU upd T3182 stopped SI=0 va=%d vs=%d seg=%d",ul.va,ul.vs,ul.seg_ind);
#endif
    }

    /*----------------------------------------------------------------------
    ** Loop and free PDUs from the original ul.va value to the latest ul.va
    **----------------------------------------------------------------------
    */

    /* Get existing PSN to free
    */
    psn = ul.first_psn_to_free;
    ul.pdu_freeing_state = NONE_TO_FREE;

    loop_cnt = MOD_DIF_ESNS(ul.va, orig_va);
    for (i=0; i<loop_cnt; i++)
    {
      ind = MOD_ESNS(orig_va + (uint16)i);

      MSG_GERAN_LOW_3_G("ELRU ind=%u 1st=%u last=%u",
              ind,
              TX_ARR(ind).blk.first_psn,
              TX_ARR(ind).blk.last_psn
             );

      /* CASE 1 : if a block has more than one pdu , and non cv 0 , check for pdu that
      **          can be freed
      ** CASE 2 : if the first and last psn is same , such that it has LI=0 , and that
      **          particular bsn is acked , then free the previous pdu i.e last_psn-1
      */
      if (  (IS_GT_MODULO_COMP(TX_ARR(ind).blk.last_psn, TX_ARR(ind).blk.first_psn, 32768))
            ||
            ( (TX_ARR(ind).blk.hdr.cv != 0)  &&
              (TX_ARR(ind).blk.last_psn == TX_ARR(ind).blk.first_psn) &&
              (TX_ARR(ind).blk.num_ext_octets) &&
              (TX_ARR(ind).blk.ext_octet[0].egprs.li == 0)
            )
         )
      {
        b_psn_change = TRUE;

        psn_to_free = psn;
        psn = TX_ARR(ind).blk.last_psn;
        DEC_MOD(psn, 32768);
        ul.psn_to_free_inclusive = psn;
      }
      else
      {
        if (TX_ARR(ind).blk.last_psn == TX_ARR(ind).blk.first_psn)
        {
          /* Only free the PDU if CV is zero and the PSN of current block really ends here 
          ** ie. PSN of the next block is different to PSN of the current block
          */
          if ((TX_ARR(ind).blk.hdr.cv == 0) &&
              /* adjacent block contains a different PSN */
              (IS_GT_MODULO_COMP(ul.seg_ind, ind, ESNS)) && /* there is a next block */
              ( (TX_ARR((MOD_ESNS(ind+1))).blk.first_psn) != (TX_ARR(ind).blk.last_psn) )
             )
          {
            b_psn_change = TRUE;

            psn_to_free = psn;
            psn = TX_ARR(ind).blk.last_psn;
            ul.psn_to_free_inclusive = psn;
            MSG_GERAN_HIGH_2_G("ELRU e_upd CV 0 ind %u psn to free %u", ind, psn);
          }
          else
          {
            /* Otherwise free the PSN previous to the current PSN */
            b_psn_change = TRUE;

            psn_to_free = psn;
            psn = TX_ARR(ind).blk.last_psn;
            DEC_MOD(psn, 32768);
            ul.psn_to_free_inclusive = psn;

            if (psn_to_free != psn)
            {
              MSG_GERAN_HIGH_3_G("ERLU e_upd CV=%u ind %u psn to free %u",
                TX_ARR(ind).blk.hdr.cv,
                ind,
                psn);
            }
          }
        }
      }
    } /* end for */

    if (b_psn_change)
    {
      if (ul.psn_to_free_inclusive == cur_pdu_ptr->psn)
      {
        ul.pdu_freeing_state = FREE_CURRENT_PDU;
        psn_to_free = ul.psn_to_free_inclusive;
        DEC_MOD(psn_to_free, 32768);
      }
      else
      {
        ul.pdu_freeing_state = DONE_FREEING;
        psn_to_free = ul.psn_to_free_inclusive;
      }

      MSG_GERAN_MED_3_G("ERLU upd free inc %u va=%d upd free inclusive = %u",
              psn_to_free,
              ul.va,
              ul.psn_to_free_inclusive);
      /* Successful transmission of PDU , set the flag to TRUE
      ** to start ready timer
      */
      grlc_ul_del_free_paq_upto_psn( psn_to_free,TRUE);
    }
    else
    {
      ul.pdu_freeing_state = NONE_TO_FREE;
    }
  } /* end b_to_update_va */

#ifdef  GSM_GPRS_ERLU_SHOW_EPUAN
  MSG_GERAN_MED_3_G("ERLU e_upd    va=%u    vs=%u vsf=%u", ul.va, ul.vs,ul.vsf);
  MSG_GERAN_MED_3_G("ERLU       n_vsf=%u p_vsf=%u seg=%u", ul.nack_vsf, ul.pack_vsf,ul.seg_ind);
#endif

  /* If tx window becomes un-stall OR V(A) has been advanced then increment
  ** the N3102 counter
  */
  if (IS_N3102_ENABLED())
  {
    if (!IS_E_TX_WINDOW_FULL() ||
        b_to_inc_n3102
       )
    {
      INC_N3102();
#ifdef  GSM_GPRS_GRLU_SHOW_N3102
      MSG_GERAN_MED_3_G("ERLU upd N3102 IncN3102=%d max=%d inc=%d",
              ul.n3102,
              ul.pan_max_value,
              PAN_INC());
#endif
    }
  }
} /* e_update_ack_state_array */


/*===========================================================================
===
===  FUNCTION      HANDLE_CONTENTION_RESOLUTION_FAILURE()
===
===  DESCRIPTION
===
===    This function clears the variables associated with the transfer of the
===    current PDU which has failed Contention Resolution. However, since MAC
===    will retry upto 4 times by sending the previously TBF establishment
===    request RLC had sent earlier. So RLC only needs to jump to AWAIT_ASS
===    state without re-sending TBF request. RLC however needs to re-instate
===    some key variables in order to restore its state to before the
===    Uplink Assignment had been received
===
===  PARAMS
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void handle_contention_resolution_failure (void)
{
  grlc_ul_handle_no_ps_access( NO_DEL);
  MSG_GERAN_MED_2_G("GRLU CRes failed ResN3104=%d max=%d",ul.n3104, N3104_MAX());
  clr_ul_data();
  detect_error_and_recover();

} /* handle_contention_resolution_failure */


/*===========================================================================
===
===  FUNCTION      E_HANDLE_CONTENTION_RESOLUTION_SUCCESS()
===
===  DESCRIPTION
===
===    This function reprocesses the data blocks in the FIFO and those already
===    segmented (in the attached window) when the Contention Resolution
===    Indication is received. Re-processing is required in order to take out
===    the TLLI fields in new data blocks -- for both ACK and UNACK modes.
===
===    ! This function is written to handle the current version of the FIFO where
===    ABORT_ACCESS is not supported. This function needs to be modified when
===    ABORT_ACCESS is supported.
===
===    When RLC receives the MR_UL_CONTENTION_IND msg with RM_CONTENTION_RESOLVED,
===    It disables PL1 access to the FIFO. PL1 also receives the same message from
===    MAC causing it to stop accessing the FIFO. If PL1 does access the FIFO,
===    UL_FIFO_BUSY will be returned. When RLC completes its processing, it
===    re-enables PL1 access and sends PH_DATA_REQ. PL1 should clear its read/get
===    records as RLC will resynchronise the read/get indices of the FIFO.
===
===    Processing involves searching the FIFO to find the oldest un-transmitted
===    data block. The offset of the block is then mapped to a position in the
===    current PDU to allow the PDU to be re-wound. Segmentation then starts
===    again from this point (without TLLI), and new values for V(S), seg_ind,
===    CV, TBC, blocks_rem etc.. are adjusted.
===
===  PARAMS
===
===  DEPENDENCIES
===
===    ul struct
===
===    PL1 should clear its read/get records as RLC will resynchronise the
===    read/get indices of the FIFO
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    Alter PL1-RLC UL FIFO
===
===========================================================================*/
static void e_handle_contention_resolution_success
(
  const mac_rlc_msg_t *mac_msg_ptr
)
{
  /* temp offset in octets from the beginning of the current pdu to the octet
  ** where re-segementation starts from.
  ** Temp block seq number of the oldest block to take back
  */
  register uint16 offset, psn;
  uint16  bsn;


  /*----------------------------------------------------------*/

  /* We are in one phase acess and contention is resolved so reset N3104
  */
  RESET_N3104();

#ifdef  GSM_GPRS_GRLU_SHOW_N3104
  MSG_GERAN_MED_2_G("ERLU CRes success ResN3104=%u max=%u",ul.n3104, N3104_MAX());
#endif

#ifdef  GSM_GPRS_GRLU_SHOW_CRES
  GRLC_UL_SHOW_TX_VARS();
  grlc_ul_show_fifo_vars();
#endif

  /* Reset timer now that we have packet uplink acknack
  */
  TIMER_STOP_T3182();

#ifdef  GSM_GPRS_GRLU_SHOW_T3182
  MSG_GERAN_LOW_0_G("ERLU CRes success stopped T3182");
#endif

  /* Only process the acknack description bitmap if this flag is set
  */
  if (mac_msg_ptr->msg.ul_tlli_ind.ack_nack_info_flg)
  {
    /* Store packet uplink acknack. The acknack description comes from the
    ** TLLI-indication message
    */
    ul.egprs_acknack_desc = mac_msg_ptr->msg.ul_tlli_ind.egprs_acknack_desc;
    e_update_ack_state_array();
    e_show_rbb( &(ul.egprs_acknack_desc),
                 ul.egprs_acknack_desc.fai
              );

    /* Log EPUAN
    ** Log Code: 0x5209
    */
    /*rlc_log_epuan(&(mr_ul_pkt_acknack_ind_t*)(mac_msg_ptr->msg.ul_tlli_ind));*/
  }
  else
  {
    /* When the acknack description is not valid we must force a few values
    */
    ul.egprs_acknack_desc.fai = RM_ACKNACK_FAI_TBF_NOT_COMPLETE;
  }

  /*--------------------------------------------
  ** Update the commanded MCS if it has changed
  **--------------------------------------------
  */
  if (ul.cur_res.cs != mac_msg_ptr->msg.ul_tlli_ind.mcs)
  {
#ifdef  GSM_GPRS_GRLU_SHOW_CRES
    MSG_GERAN_MED_3_G("ERLU CRes ul.mcs=%u tlli_ind.mcs=%u cur_res.mcs=%u",
      ul.cur_res.cs,
      mac_msg_ptr->msg.ul_tlli_ind.mcs,
      ul.cur_res.cs
   );
#endif

    ul.cur_res.cs = mac_msg_ptr->msg.ul_tlli_ind.mcs;
    ul.last_valid_mcs = ul.cur_res.cs;

    /* Update coding scheme for engg_mode
    */
    geran_eng_mode_data_write(ENG_MODE_EGPRS_UL_MCS, &ul.cur_res.cs, TRUE);
  }

  ul.cur_res.ti = RM_TLLI_NOT_PRESENT;
  ul.contention_status = RM_CONTENTION_RESOLVED;

  if (ul.egprs_acknack_desc.fai == RM_ACKNACK_FAI_TBF_COMPLETE)
  {
    MSG_GERAN_MED_3_G("ERLU CRes FAI mcs=%u tlli-cs=%u p=%u, FAI XTimer stopped",
            ul.cur_res.cs,
            ul.tlli_cs,
            cur_pdu_ptr->psn);

    TIMER_STOP_AWAIT_UL_ACKNACK();
    ul.guard_timer_state = GUARD_OFF;

    /* Shut down TBF in ack mode or unack mode.
    ** Clear current PDU and backup PDU and LLC message itself.
    ** Adjust total llc octets and send CTS ON if required
    */

    /* To transfer grouping info to the next PDU
    */
    ungroup_pdu( cur_pdu_ptr);

    /* For Non EXT UL TBF - NW does not give sensible bitmap and SSN and hence
    ** use V(s) otherwise for EXT UL TBF use V(a)
    */
    if (IS_EXT_UTBF_ACTIVE())
    {
      psn = grlc_ul_paq_seek_psn_cv0( ul.va);
    }
    else
    {
      psn = grlc_ul_paq_seek_psn_cv0( ul.vs);
    }

    MSG_GERAN_MED_3("ERLU seek p=%u seg=%u psn_cv0=%u",cur_pdu_ptr->psn,ul.seg_ind,psn);

    clr_ul_data();

    /* Successful transmission of PDU , set the flag to TRUE
    ** to start ready timer
    */
    grlc_ul_del_free_paq_upto_psn( psn,TRUE);
    grlc_ul_unput_paq();

    ul.state = US_NULL;
    UPD_UL_STATE();

    /* Consider the TBF released. Now load the current PDU from the next PDU or
    ** the queue.  If current PDU exists then est TBF, otherwise go back to NULL
    */
    (void)load_cur_pdu();
    ul.b_access = FALSE;
  } /* FAI */

  else
  /*----------------------------------------------------------
  ** Contention resolved, TBF not complete but TX is complete
  **----------------------------------------------------------
  */
  if (IS_TX_COMPLETE())
  {
    MSG_GERAN_LOW_0_G("ERLU CRes tx-complete");

    /* Freeze FIFO access for reprocessing
    */
    grlc_ul_fifo_disable_pl1_access();

    grlc_ul_fifo_delete_unread_nack_pack_blocks( &ul.nack_vsf, &ul.pack_vsf);
    UPD_UL_NACK_VSF();
    UPD_UL_PACK_VSF();

    ul.b_first_fifo_load = TRUE;
    grlc_ul_fifo_enable_pl1_access();
    pl1_send_ph_data_req();

    if (IS_EXT_UTBF_ACTIVE())
    {
      /* Reflect the new TLLI status to the current block's template as TBF might
      ** be extended and the first block following may not be up-to-date
      */
      E_NEW_DATA_BLOCK( &cur_blk);
    }
  } /* tx_complete */

  else
  /*----------------------------------------------------
  ** Handle Ack mode - Contention resolved TBF on going
  **----------------------------------------------------
  */
  if (ul.rlc_mode == GRLC_MODE_ACK)
  {
    MSG_GERAN_MED_3_G("ERLU A CRes cs=%u tlli-cs=%u p=%u",ul.cur_res.cs, ul.tlli_cs, cur_pdu_ptr->psn);

    grlc_ul_fifo_disable_pl1_access();

    GRLC_UL_FIFO_SAVE_LOCK();

    grlc_ul_fifo_delete_unread_nack_pack_blocks( &ul.nack_vsf, &ul.pack_vsf);
    UPD_UL_NACK_VSF();
    UPD_UL_PACK_VSF();

    /* Detect the gap between VA and VS. Normally the bitmap would ack the earlier BSNs
    ** first but this might not always happen, eg. SSN = 6, bitmap = 0000001, ie. only
    ** BSN 5 is seen.
    ** This means the gap is between VA=0 and BSN 5. In this case we want to start
    ** re-processing from the latest ack'ed BSN+1, ie. BSN 6.
    */

    /* Pre-sume there are no blocks to take back
    */
    bsn = ul.vs;

    /* case (2) and case (3)
    */

    if (grlc_ul_fifo_get_ptx_cnt_dyn() > 0)
    {
      /* If there are un-read block(s) (case 3) then delete them and return the
      ** oldest's BSN. Otherwise leave 'bsn' unchanged.
      ** Function returns FALSE if there are no unread blocks (no delete)
      */
      if (grlc_ul_fifo_delete_unread_ptx_blocks( &bsn) == FALSE)
      {

        GRLC_UL_FIFO_SAVE_UNLOCK();

        /* case (2)
        **
        ** (2a) seg_ind is beyond vsf
        ** |------------|--------------|
        ** vs          vsf            seg
        **
        ** (2b) seg_ind coincides with vsf
        ** |------------|
        ** vs          vsf
        **             seg
        **
        ** |------------> ptx_ctr
        ** |------------> ptx_read_ctr
        **              ^
        **              bsn
        */

        /* No un-read PTX blocks, VS=VSF
        */
        bsn = ul.vsf;

        /* if bsn is inside seg_ind then we can obtain offset and psn using bsn as index
        */
        if (IS_GT_MOD(ul.seg_ind, bsn, ul.cur_res.ws))
        {
          offset = TX_ARR( bsn).blk.offset;
          psn = TX_ARR( bsn).blk.first_psn;
        }
        else
        {
          psn = cur_pdu_ptr->psn;
          offset = cur_pdu_ptr->rem_ind + cur_pdu_ptr->pdu_ofs;
        }
        MSG_GERAN_MED_3_G("ERLU CRes PTX (2) bsn=vsf=%u vs=%u seg=%u",bsn,ul.vs,ul.seg_ind);
      } /* end case (2) */
      else
      {

        GRLC_UL_FIFO_SAVE_UNLOCK();

        /* case (3). Unread blocks deleted and bsn holds the oldest unread BSN
        **
        ** |------------|--------------|
        ** vs          vsf            seg (could be the same as vsf)
        **
        ** |------------> ptx_ctr
        ** |---------> ptx_read_ctr. After deleting the un-read blocks
        **
        ** |--------->    ptx_ctr
        ** |---------> ptx_read_ctr
        **           ^
        **           bsn
        **
        ** vsf and seg_ind could coincide with BSN being behind, both offset is valid
        */

        /* blocks were deleted so bsn is well behind seg_ind hence we can find
        ** psn and offset from bsn
        */
        offset = TX_ARR( bsn).blk.offset;
        psn = TX_ARR( bsn).blk.first_psn;

        MSG_GERAN_MED_3_G("ERLU CRes PTX (3) del bsn=%u vs=%u vsf=%u",bsn, ul.vs, ul.vsf);

        /* If VS is in front of BSN then take it back
        */
        if (IS_GT_MOD(ul.vs, bsn, ul.cur_res.ws))
          ul.vs = bsn;

        /* If VSF is in front of BSN then take it back
        */
        if (IS_GT_MOD(ul.vsf, bsn, ul.cur_res.ws))
          ul.vsf = bsn;
      } /* end case (3) */
    }
    else
    {

      GRLC_UL_FIFO_SAVE_UNLOCK();

      /* (1a) seg > vsf(bsn)
      **     -------|--------|
      **           vs       seg
      **           vsf
      **     bsn = vs = vsf.
      ** offset = rem_ind.
      **
      ** (1b) seg = vsf
      **     -------|---
      **           vs
      **           vsf
      **           seg
      **           bsn
      **
      ** No blocks to take back.
      ** If segment_done then do nothing
      */
      bsn = ul.vs;

      /* If bsn is inside seg_ind, we can obtain offset and psn using bsn as index
      */
      if (IS_GT_MOD(ul.seg_ind, bsn, ul.cur_res.ws))
      {
        offset = TX_ARR( bsn).blk.offset;
        psn = TX_ARR( bsn).blk.first_psn;
      }
      else
      {
        /* seg_ind == bsn then use current PDU and its remaining octets as offset
        */
        offset = cur_pdu_ptr->rem_ind + cur_pdu_ptr->pdu_ofs;
        psn = cur_pdu_ptr->psn;
      }
      MSG_GERAN_MED_3_G("ERLU CRes PTX (1) bsn=vsf=%u vs=%u seg=%u",bsn,ul.vs,ul.seg_ind);

    } /* end case (1) */

    /* Common for all cases (1), (2) and (3)
    */
    e_grlc_ul_unsegment( bsn);

    /* Restore PSN from the PAQ and make it the current PDU. Strip 'offset'
    ** octets off the front of the PDU
    */
    grlc_ul_paq_make_psn_current( psn, offset);

    /* Update VS and seg_ind
    */
    ul.seg_ind = bsn;
    UPD_UL_NSEG();

    UPD_UL_VS();

    ul.vsf = bsn;
    UPD_UL_VSF();

    /* if next pending ack block is outside V(A) and V(S) then set it to V(A)
    */
    if (IS_GT_MOD(ul.pack_vsf, ul.vs, ul.cur_res.ws))
    {
      ul.pack_vsf = ul.va;
      UPD_UL_PACK_VSF();
    }
    /* if next pending ack block is outside V(A) and V(S) then set it to V(A)
    */
    if (IS_GT_MOD(ul.nack_vsf, ul.vs, ul.cur_res.ws))
    {
      ul.nack_vsf = ul.va;
      UPD_UL_NACK_VSF();
    }

    E_NEW_DATA_BLOCK( &cur_blk);
    recalc_cv_related_values( GRLC_REPROCESS_CON_RES);

    /* There were segmented blocks so segment a few blocks.
    */
    ul.segment_state = e_segment_pdu();
    segment_loop( FALSE);

    /* Setting this flag causes PH_DATA_REQ to be sent to PL1 so it can start
    ** polling the FIFO for data again
    */
    ul.b_first_fifo_load = TRUE;
    grlc_ul_fifo_enable_pl1_access();
    pl1_send_ph_data_req();

    TX_BLKS();
  } /* end ack mode */
  else
  /*-------------------
  ** Handle Unack mode
  **-------------------
  */
  {
    MSG_GERAN_MED_3_G("ERLU U CRes cs=%u tlli-cs=%u p=%u",ul.cur_res.cs, ul.tlli_cs, cur_pdu_ptr->psn);

    /* Contention resolved but TBF is on going
    */
    grlc_ul_fifo_disable_pl1_access();

    /* Preload bsn with the last block sent off to the FIFO (case 1)
    */
    bsn = ul.pack_vsf;

    GRLC_UL_FIFO_SAVE_LOCK();

    /* Pre-sume there are no blocks to take back
    */
    bsn = ul.vs;

    /* case (2) and case (3)
    */
    if (grlc_ul_fifo_get_pack_cnt() > 0)
    {
      /* If there are un-read block(s) (case 3) then delete them and return the
      ** oldest's BSN. Otherwise leave 'bsn' unchanged.
      ** Function returns FALSE if there are no unread blocks (no delete)
      */
      if (grlc_ul_fifo_delete_unread_pack_blocks( &bsn) == FALSE)
      {

        GRLC_UL_FIFO_SAVE_UNLOCK();

        /* case (2)
        **
        ** (2a) seg_ind is beyond vsf
        ** |------------|--------------|
        ** vs          vsf            seg
        **
        ** (2b) seg_ind coincides with vsf
        ** |------------|
        ** vs          vsf
        **             seg
        **
        ** |------------> ptx_ctr
        ** |------------> ptx_read_ctr
        **              ^
        **              bsn
        */

        /* No un-read PTX blocks, VS=VSF
        */
        bsn = ul.pack_vsf;

        /* if bsn is inside seg_ind then we can obtain offset and psn using bsn as index
        */
        if (IS_GT_MOD(ul.seg_ind, bsn, ul.cur_res.ws))
        {
          offset = TX_ARR( bsn).blk.offset;
          psn = TX_ARR( bsn).blk.first_psn;
        }
        else
        {
          psn = cur_pdu_ptr->psn;
          offset = cur_pdu_ptr->rem_ind + cur_pdu_ptr->pdu_ofs;
        }
        MSG_GERAN_MED_3_G("ERLU CRes UNACK (2) bsn=vsf=%u vs=%u seg=%u",bsn,ul.vs,ul.seg_ind);
      } /* end case (2) */
      else
      {

        GRLC_UL_FIFO_SAVE_UNLOCK();

        /* case (3). Unread blocks deleted and bsn holds the oldest unread BSN
        **
        ** |------------|--------------|
        ** vs          vsf            seg (could be the same as vsf)
        **
        ** |------------> ptx_ctr
        ** |---------> ptx_read_ctr. After deleting the un-read blocks
        **
        ** |--------->    ptx_ctr
        ** |---------> ptx_read_ctr
        **           ^
        **           bsn
        **
        ** vsf and seg_ind could coincide with BSN being behind, both offset is valid
        */

        /* blocks were deleted so bsn is well behind seg_ind hence we can find
        ** psn and offset from bsn
        */
        offset = TX_ARR(bsn).blk.offset;
        psn = TX_ARR( bsn).blk.first_psn;

        MSG_GERAN_MED_3_G("ERLU CRes PAK (3) del bsn=%u vs=%u vsf=%u",bsn, ul.vs, ul.pack_vsf);

        /* If VS is in front of BSN then take it back
        */
        if (IS_GT_MOD(ul.vs, bsn, ul.cur_res.ws))
          ul.vs = bsn;

        /* If PACK_VSF is in front of BSN then take it back
        */
        if (IS_GT_MOD(ul.pack_vsf, bsn, ul.cur_res.ws))
          ul.pack_vsf = bsn;
      } /* end case (3) */
    }
    else
    {

      GRLC_UL_FIFO_SAVE_UNLOCK();

      /* (1a) seg > vsf(bsn)
      **     -------|--------|
      **           vs       seg
      **           vsf
      **     bsn = vs = vsf.
      ** offset = rem_ind.
      **
      ** (1b) seg = vsf
      **     -------|---
      **           vs
      **           vsf
      **           seg
      **           bsn
      **
      ** No blocks to take back.
      ** If segment_done then do nothing
      */
      bsn = ul.vs;

      /* If bsn is inside seg_ind, we can obtain offset and psn using bsn as index
      */
      if (IS_GT_MOD(ul.seg_ind, bsn, ul.cur_res.ws))
      {
        offset = TX_ARR( bsn).blk.offset;
        psn = TX_ARR( bsn).blk.first_psn;
      }
      else
      {
        /* seg_ind == bsn then use current PDU and its remaining octets as offset
        */
        offset = cur_pdu_ptr->rem_ind + cur_pdu_ptr->pdu_ofs;
        psn = cur_pdu_ptr->psn;
      }
      MSG_GERAN_MED_3_G("ERLU CRes UNACK (1) bsn=vsf=%u vs=%u seg=%u",bsn,ul.vs,ul.seg_ind);
    } /* end case (1) */

    /* Common for all cases (1), (2) and (3)
    */
    e_grlc_ul_unsegment( bsn);

    /* Restore PSN from the PAQ and make it the current PDU. Strip 'offset'
    ** octets off the front of the PDU
    */
    grlc_ul_paq_make_psn_current( psn, offset);

    /* Update VS and seg_ind
    */
    ul.seg_ind = bsn;
    UPD_UL_NSEG();

    UPD_UL_VS();

    ul.pack_vsf = bsn;
    UPD_UL_PACK_VSF();

    E_NEW_DATA_BLOCK( &cur_blk);
    recalc_cv_related_values( GRLC_REPROCESS_CON_RES);

    /* There were segmented blocks so segment a few blocks.
    */
    ul.segment_state = e_segment_pdu();
    segment_loop( FALSE);

    /* Setting this flag causes PH_DATA_REQ to be sent to PL1 so it can start
    ** polling the FIFO for data again
    */
    ul.b_first_fifo_load = TRUE;
    grlc_ul_fifo_enable_pl1_access();
    pl1_send_ph_data_req();

    TX_BLKS();
  } /* end un-ack mode */

  detect_error_and_recover();

#ifndef FEATURE_GERAN_REDUCED_DEBUG
  ul_diag_abn_rel_cnts_st.arc_cres_cnt++;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

#ifdef  GSM_GPRS_GRLU_SHOW_CRES
  GRLC_UL_SHOW_TX_VARS();
  grlc_ul_show_fifo_vars();
#endif

} /* e_handle_contention_resolution_success */


/*===========================================================================
===
===  FUNCTION      HANDLE_CONTENTION_RESOLUTION_SUCCESS()
===
===  DESCRIPTION
===
===    This function reprocesses the data blocks in the FIFO and those already
===    segmented (in the attached window) when the Contention Resolution
===    Indication is received. Re-processing is required in order to take out
===    the TLLI fields in new data blocks -- for both ACK and UNACK modes.
===
===    ! This function is written to handle the current version of the FIFO where
===    ABORT_ACCESS is not supported. This function needs to be modified when
===    ABORT_ACCESS is supported.
===
===    When RLC receives the MR_UL_CONTENTION_IND msg with RM_CONTENTION_RESOLVED,
===    It disables PL1 access to the FIFO. PL1 also receives the same message from
===    MAC causing it to stop accessing the FIFO. If PL1 does access the FIFO,
===    UL_FIFO_BUSY will be returned. When RLC completes its processing, it
===    re-enables PL1 access and sends PH_DATA_REQ. PL1 should clear its read/get
===    records as RLC will resynchronise the read/get indices of the FIFO.
===
===    Processing involves searching the FIFO to find the oldest un-transmitted
===    data block. The offset of the block is then mapped to a position in the
===    current PDU to allow the PDU to be re-wound. Segmentation then starts
===    again from this point (without TLLI), and new values for V(S), seg_ind,
===    CV, TBC, blocks_rem etc.. are adjusted.
===
===  PARAMS
===
===  DEPENDENCIES
===
===    ul struct
===
===    PL1 should clear its read/get records as RLC will resynchronise the
===    read/get indices of the FIFO
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    Alter PL1-RLC UL FIFO
===
===========================================================================*/
void handle_contention_resolution_success
(
  const mac_rlc_msg_t *mac_msg_ptr
)
{
  /* temp block seq number of the oldest block to take back
  */
  uint16  bsn;

  /* temp offset in octets from the beginning of the current pdu to the octet
  ** where re-segementation starts from.
  */
  uint16  offset, psn;


  /*----------------------------------------------------------*/

  if (ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
  {
    e_handle_contention_resolution_success( mac_msg_ptr);
    process_guard_timer();
    return;
  }
  else
  {
    /* We are in one phase acess and this is contention is resolved so reset N3104
    */
    RESET_N3104();
#ifdef  GSM_GPRS_GRLU_SHOW_N3104
    MSG_GERAN_MED_2_G("GRLU CRes success ResN3104=%d max=%d",ul.n3104, N3104_MAX());
#endif

    /* Reset timer now that we have packet uplink acknack
    */
    TIMER_STOP_T3182();

#ifdef  GSM_GPRS_GRLU_SHOW_T3182
    MSG_GERAN_LOW_0_G("GRLU CRes success stopped T3182");
#endif

    /* Store packet uplink acknack. The acknack description comes from the
    ** TLLI-indication message
    */
    ul.ul_acknack = mac_msg_ptr->msg.ul_tlli_ind.acknack_desc;
    update_ack_state_array();
    rlc_log_ul_acknack_params();

    show_rbb( &ul.ul_acknack,ul.ul_acknack.fai);

    /*----------------------------------------
    ** Update coding scheme if it has changed
    **----------------------------------------
    */
    if (ul.cur_res.cs != mac_msg_ptr->msg.ul_tlli_ind.cs)
    {
      ul.cur_res.cs = mac_msg_ptr->msg.ul_tlli_ind.cs;
      ul.last_valid_coding_scheme = ul.cur_res.cs;

      /* Update coding scheme for engg_mode
      */
      geran_eng_mode_data_write(ENG_MODE_GPRS_UL_CS, &ul.cur_res.cs, TRUE);
    }

    ul.cur_res.ti = RM_TLLI_NOT_PRESENT;
    ul.contention_status = RM_CONTENTION_RESOLVED;

    if (ul.ul_acknack.fai == RM_ACKNACK_FAI_TBF_COMPLETE)
    {
      MSG_GERAN_MED_3_G("GRLU CRes FAI cs=%d tlli-cs=%d p=%u",
              ul.cur_res.cs,
              ul.tlli_cs,
              cur_pdu_ptr->psn);

      MSG_GERAN_HIGH_0_G("GRLU CRes FAI XTimer stopped");
      TIMER_STOP_AWAIT_UL_ACKNACK();
      ul.guard_timer_state = GUARD_OFF;

      /* Shut down TBF in ack mode or unack mode.
      ** Clear current PDU and backup PDU and LLC message itself.
      ** Adjust total llc octets and send CTS ON if required
      */

      /* To transfer grouping info to the next PDU
      */
      ungroup_pdu( cur_pdu_ptr);

      /* For Non EXT UL TBF - NW does not give sensible bitmap and SSN and hence
      ** use V(s) otherwise for EXT UL TBF use V(a)
      */
      if (IS_EXT_UTBF_ACTIVE())
      {
        psn = grlc_ul_paq_seek_psn_cv0( ul.va);
      }
      else
      {
        psn = grlc_ul_paq_seek_psn_cv0( ul.vs);
      }

      MSG_GERAN_MED_3_G("GRLU seek p=%u seg=%d psn_cv0=%u",cur_pdu_ptr->psn,ul.seg_ind,psn);

      clr_ul_data();

      /* Successful transmission of PDU , set the flag to TRUE
      ** to start ready timer
      */
      grlc_ul_del_free_paq_upto_psn( psn,TRUE);
      grlc_ul_unput_paq();

      ul.state = US_NULL;
      UPD_UL_STATE();

      /* Consider the TBF released. Now load the current PDU from the next PDU or
      ** the queue.  If current PDU exists then est TBF, otherwise go back to NULL
      */
      (void)load_cur_pdu();

      ul.b_access = FALSE;
    } /* FAI */

    else
    /*----------------------------------------------------------
    ** Contention resolved, TBF not complete but TX is complete
    **----------------------------------------------------------
    */
    if (IS_TX_COMPLETE())
    {
      MSG_GERAN_LOW_0_G("GRLU CRes tx-complete");

      grlc_ul_fifo_disable_pl1_access();

      grlc_ul_fifo_delete_unread_nack_pack_blocks( &ul.nack_vsf, &ul.pack_vsf);
      UPD_UL_NACK_VSF();
      UPD_UL_PACK_VSF();

      ul.b_first_fifo_load = TRUE;
      grlc_ul_fifo_enable_pl1_access();
      pl1_send_ph_data_req();

      if (IS_EXT_UTBF_ACTIVE())
      {
        /* Reflect the new TLLI status to the current block's template as TBF might
        ** be extended and the first block following may not be up-to-date
        */
        NEW_DATA_BLOCK( &cur_blk);
      }
    } /* tx_complete */

    else
    /*----------------------------------------------------
    ** Handle Ack mode - Contention resolved TBF on going
    **----------------------------------------------------
    */
    if (ul.rlc_mode == GRLC_MODE_ACK)
    {
      MSG_GERAN_MED_3_G("GRLU A CRes cs=%d tlli-cs=%d p=%u",ul.cur_res.cs, ul.tlli_cs, cur_pdu_ptr->psn);

      grlc_ul_fifo_disable_pl1_access();

      GRLC_UL_FIFO_SAVE_LOCK();

      grlc_ul_fifo_delete_unread_nack_pack_blocks( &ul.nack_vsf, &ul.pack_vsf);
      UPD_UL_NACK_VSF();
      UPD_UL_PACK_VSF();

      /* ! Detect gap(s) between VA and VS. Normally the bitmap would ack the earlier
      ** BSNs first but this might not always happen, eg. SSN = 6, bitmap = 0000001,
      ** ie. only BSN 5 is seen. This means the gap is between VA=0 and BSN 5.
      ** In this case we want to start re-processing from the latest ack'ed BSN+1,
      ** ie. BSN 6.
      */

      /* Pre-sume there are no blocks to take back
      */
      bsn = ul.vs;

      /* case (2) and case (3)
      */
      if (grlc_ul_fifo_get_ptx_cnt_dyn() > 0)
      {
        if (grlc_ul_fifo_delete_unread_ptx_blocks( &bsn) == FALSE)
        {

          GRLC_UL_FIFO_SAVE_UNLOCK();

          /* case (2)
          **
          ** (2a) seg_ind is beyond vsf
          ** |------------|--------------|
          ** vs          vsf            seg
          **
          ** (2b) seg_ind coincides with vsf
          ** |------------|
          ** vs          vsf
          **             seg
          **
          ** |------------> ptx_ctr
          ** |------------> ptx_read_ctr
          **              ^
          **              bsn
          */

          /* No un-read PTX blocks, VS=VSF
          */
          bsn = ul.vsf;

          /* if bsn is inside seg_ind, we can obtain offset and psn using bsn as index
          */
          if (IS_GT_MODULO_COMP(ul.seg_ind,bsn,SNS))
          {
            offset = TX_ARR( bsn).blk.offset;
            psn = TX_ARR( bsn).blk.first_psn;
          }
          else
          {
            psn = cur_pdu_ptr->psn;
            offset = cur_pdu_ptr->rem_ind + cur_pdu_ptr->pdu_ofs;
          }
          MSG_GERAN_MED_3_G("GRLU CRes PTX (2) bsn=vsf=%d vs=%d seg=%d",bsn, ul.vs, ul.seg_ind);
        }
        else
        {

          GRLC_UL_FIFO_SAVE_UNLOCK();

          /* case (3). Unread blocks deleted and bsn holds the oldest unread BSN
          **
          ** |------------|--------------|
          ** vs          vsf            seg (could be the same as vsf)
          **
          ** |------------> ptx_ctr
          ** |---------> ptx_read_ctr. After deleting the un-read blocks
          **
          ** |--------->    ptx_ctr
          ** |---------> ptx_read_ctr
          **           ^
          **           bsn
          **
          ** vsf and seg_ind could coincide with BSN being behind, both offset is valid
          */

          /* blocks were deleted so bsn is well behind seg_ind hence we can find
          ** psn and offset from bsn
          */
          offset = TX_ARR( bsn).blk.offset;
          psn = TX_ARR( bsn).blk.first_psn;

          MSG_GERAN_MED_3_G("GRLU CRes PTX (3) del bsn=%d vs=%d vsf=%d",bsn, ul.vs, ul.vsf);

          /* If VS is in front of BSN then take it back
          */
          if (IS_GT_MODULO_COMP(ul.vs, bsn, SNS))
            ul.vs = bsn;

          /* If VSF is in front of BSN then take it back
          */
          if (IS_GT_MODULO_COMP(ul.vsf, bsn, SNS))
            ul.vsf = bsn;
        }
      }
      else
      {

        GRLC_UL_FIFO_SAVE_UNLOCK();

        /* (1a) seg > vsf(bsn)
        **     -------|--------|
        **           vs       seg
        **           vsf
        **     bsn = vs = vsf.
        ** offset = rem_ind.
        **
        ** (1b) seg = vsf
        **     -------|---
        **           vs
        **           vsf
        **           seg
        **           bsn
        **
        ** No blocks to take back.
        ** If segment_done then do nothing
        */
        bsn = ul.vs;

        /* If bsn is inside seg_ind, we can obtain offset and psn using bsn as index
        */
        if (IS_GT_MODULO_COMP(ul.seg_ind,bsn,SNS))
        {
          offset = TX_ARR( bsn).blk.offset;
          psn = TX_ARR( bsn).blk.first_psn;
        }
        else
        {
          /* seg_ind == bsn then use current PDU and its remaining octets as offset
          */
          offset = cur_pdu_ptr->rem_ind + cur_pdu_ptr->pdu_ofs;
          psn = cur_pdu_ptr->psn;
        }
      }

      /* Common for all cases (1), (2) and (3)
      */
      grlc_ul_unsegment( bsn);

      /* Restore PSN from the PAQ and make it the current PDU. Strip 'offset'
      ** octets off the front of the PDU
      */
      grlc_ul_paq_make_psn_current( psn, offset);

      /* Update VS and seg_ind
      */
      ul.seg_ind = bsn;
      UPD_UL_NSEG();

      UPD_UL_VS();

      ul.vsf = bsn;
      UPD_UL_VSF();

      /* if next pending ack block is outside V(A) and V(S) then set it to V(A)
      */
      if (IS_GT_MODULO_COMP(ul.pack_vsf, ul.vs, SNS))
      {
        ul.pack_vsf = ul.va;
        UPD_UL_PACK_VSF();
      }
      /* if next pending ack block is outside V(A) and V(S) then set it to V(A)
      */
      if (IS_GT_MODULO_COMP(ul.nack_vsf, ul.vs, SNS))
      {
        ul.nack_vsf = ul.va;
        UPD_UL_NACK_VSF();
      }


      NEW_DATA_BLOCK( &cur_blk);

      recalc_cv_related_values( GRLC_REPROCESS_CON_RES);

      /* There were segmented blocks so segment a few blocks.
      */
      ul.segment_state = segment_pdu();
      segment_loop( FALSE);

      /* Setting this flag causes PH_DATA_REQ to be sent to PL1 so it can start
      ** polling the FIFO for data again
      */
      ul.b_first_fifo_load = TRUE;
      grlc_ul_fifo_enable_pl1_access();
      pl1_send_ph_data_req();

      TX_BLKS();
    } /* end ack mode */
    else
    /*-------------------
    ** Handle Unack mode
    **-------------------
    */
    {
      MSG_GERAN_MED_3_G("GRLU U CRes cs=%d tlli-cs=%d p=%u",ul.cur_res.cs, ul.tlli_cs, cur_pdu_ptr->psn);

      /* Contention resolved but TBF is on going
      */
      grlc_ul_fifo_disable_pl1_access();

      /* Preload bsn with the last block sent off to the FIFO (case 1)
      */
      bsn = ul.pack_vsf;

      GRLC_UL_FIFO_LOCK();

      /* If un-read PACK blocks exist then delete them and load the oldest BSN
      ** value into 'bsn'
      */
      (void)grlc_ul_fifo_delete_unread_pack_blocks( &bsn);

      GRLC_UL_FIFO_UNLOCK();

      /* get offset to the octet from the block where re-processing starts from
      */
      offset = TX_ARR( bsn).blk.offset;

      grlc_ul_unsegment( bsn);

      shorten_cur_pdu( offset);

      /* That is what we have left to transmit
      */
      ul.octets_rem = cur_pdu_ptr->len;

      /* Update TBC, blocks_rem, CV, ABS_BSN
      */
      recalc_cv_related_values( GRLC_REPROCESS_CON_RES);

      /* Update VS and seg_ind
      */
      ul.seg_ind = bsn;
      UPD_UL_NSEG();

      ul.vs = bsn;
      UPD_UL_VS();

      ul.pack_vsf = bsn;
      UPD_UL_PACK_VSF();

      /* In the ABORT_ACCESS version we should update vsf as well
      */

      /* if next pending ack block is outside V(A) and V(S) then set it to V(A)
      */
      if (IS_GT_MODULO_COMP(ul.pack_vsf, ul.vs, SNS))
      {
        ul.pack_vsf = ul.vs;
        UPD_UL_PACK_VSF();
      }

      /* Remained of the new PDU is at the start of the new PDU
      */
      cur_pdu_ptr->rem_ind = 0;

      /* Ensure rem_cnt is updated according to the new TI status and CS
      */
      NEW_DATA_BLOCK( &cur_blk);

      (void)segment_pdu();
      segment_loop( FALSE);

      /* Reset flag to allow PH_DATA_REQ primitive to be sent to PL1 when
      ** blocks are available for accessing
      */
      ul.b_first_fifo_load = TRUE;
      grlc_ul_fifo_enable_pl1_access();
      pl1_send_ph_data_req();

      TX_BLKS();
    } /* end un-ack mode */

    process_guard_timer();

    detect_error_and_recover();

#ifndef FEATURE_GERAN_REDUCED_DEBUG
    ul_diag_abn_rel_cnts_st.arc_cres_cnt++;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
  }

} /* handle_contention_resolution_success */

/*===========================================================================
===
===  FUNCTION      CHANGE_CODING_SCHEME()
===
===  DESCRIPTION
===
===    This function reprocesses some of the data blocks already produced
===    in the old coding scheme and produce the new data blocks with new
===    coding scheme.
===
===    The content of the L1 FIFO is examined to find the oldest un-read data
===    block for each area. This is the point at which reprocessing starts from.
===    Each data block contains an 'offset' value to the beginning of the PDU.
===    This offset value together with the PSN (pdu number) allows the PDU in
===    to be restored to where the offset is. Segmentation then starts again
===    from this point with the new coding scheme, and new values for VS, etc..
===    are adjusted.
===
===    This function also sets a flag to trigger PL1 to start looking at the
===    FIFO again once resegmentation is complete. Since PL1 waits for this
===    signal from RLC when new coding scheme is commanded by the n/w.
===
===    MAC sends coding scheme info to both PL1 and RLC where PL1 gets this
===    info first and hence stops accessing the FIFO before RLC gets to
===    reprocess it.
===
===    As the FIFO contains 3 distinct types of blocks Nack, Pending-tx and
===    Pending-ack. Only the Pending-tx blocks need to be re-segmented and the
===    other two are left untouched and re-transmitted in their original
===    CV and coding scheme. However, PL1 expects the Read Out indices in the
===    Pending-ack and Nack areas to be re-synced.
===
===    The 3 cases referred to below are the states of the FIFO when coding scheme
===    change command is received
===
===  PARAMS
===
===  DEPENDENCIES
===
===    ul struct
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    Alter PL1-RLC UL FIFO
===
===========================================================================*/
void change_coding_scheme(const uint8 cs)
{
  /*----------------------------------------------------------*/
  /* De-activate the block timers service when PL1 is not actively getting blocks
  */
  b_block_timers_service_active = FALSE;

  MSG_GERAN_MED_3_G("GRLU ChCS cs=%d new-cs=%d p=%u",ul.cur_res.cs,cs,cur_pdu_ptr->psn);
  GRLC_UL_SHOW_TX_VARS();

  ul.cur_res.cs = cs;

  reprocess( GRLC_CHANGE_OF_CS_MCS);
} /* change_coding_scheme */
/*===========================================================================
===
===  FUNCTION      GRR_SUSPEND_REQ()
===
===  DESCRIPTION
===
===    Processes the GPRS Suspend Request from LLC. If RLC is not in IDLE then
===    say error and do nothing. If RLC is already suspended then say error.
===    Otherwise go in to GRLC_SUSPEND and delink by moving the current 'data'
===    into the reserved queue.
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
void grr_suspend_req (void)
{
  /* RLC should not get suspended and ignore the suspend request
   */

  /*  Optimization for CR74482 to get round failure in
      ** TC 44.2.2.1.8: GPRS detach / abnormal cases / change of cell into new
      ** routing area

      ** &&&&&&&&

      ** TC 42.4.2.1.4: Cell change order procedure / Uplink transfer /
      ** Failure cases / Contention resolution failure
  */

  if (ul.state == US_AWAIT_ASS )
  {
    MSG_GERAN_HIGH_0_G("GRLU Rcvd Susp in US_AWAIT_ASS");
    return;
  }

  /* Suspend by delinking the data PDUs
  */
  if (ul.suspend_state != GRLC_SUSPEND)
  {
    if ( ul.state != US_NULL
       )
    {
      /*---------------------------------------------------------------------------
      ** Special handler for scenario as described in CR 74482 - Handling a Detach
      ** request during UL packet data transfer.
      **---------------------------------------------------------------------------
      */
      MSG_GERAN_MED_1_G("GRLU Rcvd SuspInXfer (%d) new state=NULL",
              ul.state);

      /* We want to use the REL_UL_ABNORMAL_WITHOUT_RETRY signal to release MAC.
      ** However, MAC does not expect this signal so we make do with
      ** REL_UL_ABNORMAL_WITH_ACCESS_RETRY instead
      */
      ul.ul_release_cause = REL_UL_ABNORMAL_WITH_ACCESS_RETRY;

      /* Inform MAC of RLC initiated ul release
      */
      indicate_ul_release(  REL_UL_ABNORMAL_WITH_ACCESS_RETRY );

      ul.state = US_NULL;
    }

    ul.suspend_state = GRLC_SUSPEND;
    delink_data_pdus();
  }
  else
  {
    MSG_GERAN_LOW_1_G("GRLU Already Susp s=%d", ul.state);
  }
} /* grr_suspend_req */

/*===========================================================================
===
===  FUNCTION      grlc_search_paq_for_sapi()
===
===  DESCRIPTION
===
===    This function checks whether thre are PDUs in PAQ queue from SAPIs
===    other than the SAPI indicated as the input to function
===
===  PARAMS
===
===    sapi - Purge PDUs from this SAPI.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    TRUE  - If PDUs from other SAPIs than that is indicated are in PAQ.
===    FALSE - If all PDUs in PAQ are from the indicated SAPI.
===
===  SIDE EFFECTS
===
===    None
===========================================================================*/
static boolean grlc_search_paq_for_sapi(   uint8 sapi )
{
  uint16 ind;
  uint16 ctr;

  ctr = ul.paq_ctr;
  ind = ul.paq_out;

  while (ctr > 0)
  {
    if (sapi != (ul.paq_ptr[ind])->sapi)
    {
      MSG_GERAN_LOW_1_G("GRLU psn from sapi %d in PAQ",ul.paq_ptr[ind]->sapi);
      return TRUE;
    }
    else
    {
      INC_MOD(ind, GRLC_UL_MAX_PAQ_SIZE);
      ctr--;
    }
  }

  MSG_GERAN_MED_1_G("GRLU psn from sapi %d only in PAQ",sapi);

  return FALSE;

} /* grlc_search_paq_for_sapi() */

/*===========================================================================
===
===  FUNCTION      GRR_PURGE_SUSPENDED_DATA_REQ()
===
===  DESCRIPTION
===
===    This function purges the suspended data PDU in the reserved queue. This
===    only happens when GRLC in is Suspend state. This is called by GRR following
===    a Detach Request during Suspension. This purging action prevents suspended
===    data to be delivered by RLC after the Detach Sequence
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
void grr_purge_suspended_data_req (void)
{
  gprs_grr_ul_mes_t *ptr;

  if (ul.suspend_state == GRLC_SUSPEND)
  {
    MSG_GERAN_LOW_2_G("GRLU SUSP PURGE susp data s=%d qcnt=%d", ul.state, (uint16)q_cnt(&grlc_ul_old_llc_q));

    while (q_cnt(&grlc_ul_old_llc_q) > 0)
    {
      ptr = (gprs_grr_ul_mes_t *)q_get(&grlc_ul_old_llc_q);
      if (ptr != NULL)
      {
        free_llc_pdu_and_msg(ptr);
      }
    }
  }
  else
  {
    MSG_GERAN_MED_1_G("GRLU Rcvd Purge not in Suspend(%d)", ul.suspend_state);
  }
} /* grr_purge_suspended_data_req */

/*===========================================================================
===
===  FUNCTION      GRR_PURGE_DATA_REQ()
===
===  DESCRIPTION
===
===    This function purges the data PDU in the reserve queue and main queue. This
===    only happens when GRLC in asked to delete data after PDP deactivation of a
===    particular SAPI.
===
===  PARAMS
===
===    sapi - PDUs from this SAPI are to be purged.
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===========================================================================*/
void grr_purge_data_req (uint8 sapi)
{

  MSG_GERAN_MED_5_G("GRLU PURGE_DATA_REQ: Pre purge s=%d susp_state=%d, ULQ cnt=%d ULTQ cnt=%d OldQ cnt=%d",
            ul.state,
            ul.suspend_state,
            (uint16)q_cnt(&ULQ()),
            (uint16)q_cnt(&ULTQ()),
            (uint16)q_cnt(&grlc_ul_old_llc_q));

  switch (ul.state)
  {
    case US_NULL:
    {
      /* Clear UL data structure. */
      clr_ul_data();

      /* Unput PDU held in next_pdu_ptr into main UL queue */
      grlc_ul_unput_paq();

      /* Flush PDUs in main PDU queue that came from indicated SAPI. */
      flush_llc_pdu_queue_sapi( sapi);

      /*
      * Re-order signalling pdu to be sent first to NW
      */
      grlc_ul_reorder_pdus();

      /* Load cur_pdu_ptr with a PDU */
      (void)load_cur_pdu();

      /* Est TBF from ul.state = NULL */
      est_tbf_from_null();
      break;
    }

    case US_ACK_XFER:
    case US_UNACK_XFER:
    {
      if ( !grlc_search_paq_for_sapi(  sapi ) )
      {
        /* There are no UL PDUs in PAQ that came from SAPIs other than the
        ** SAPI indicated. Under these conditions, TBF will be released and
        ** PAQ will be cleared.
        */

        /* We want to use the REL_UL_ABNORMAL_WITHOUT_RETRY signal to release
        ** MAC. However, MAC does not expect this signal so we make do with
        ** REL_UL_ABNORMAL_WITH_ACCESS_RETRY instead.
        */
        ul.ul_release_cause = REL_UL_ABNORMAL_WITH_ACCESS_RETRY;

        /* Inform MAC of RLC initiated ul release
        */
        indicate_ul_release(  REL_UL_ABNORMAL_WITH_ACCESS_RETRY );

        /* Disable PS access in RLC. */
        ul.b_access = FALSE;

        /* TBF released. Clear UL data structure. */
        clr_ul_data();

        /* To ensure that PSNs are in sequence */
        if ( cur_pdu_ptr != NULL )
        {
          pg.psn = cur_pdu_ptr->psn;
        }

        /* Free cur_pdu_ptr and corresponding PDU in paq queue if
        ** that psn has been already acked.
        */
        if (cur_pdu_ptr != NULL &&
            ul.pdu_freeing_state == FREE_CURRENT_PDU &&
            cur_pdu_ptr->psn == ul.psn_to_free_inclusive
           )
        {
          /* Successful transmission of PDU , set the flag to TRUE
          ** to start ready timer
          */
          grlc_ul_del_free_paq_upto_psn( cur_pdu_ptr->psn,TRUE);
          ul.pdu_freeing_state = NONE_TO_FREE;
          ul.psn_to_free_inclusive = ul.first_psn_to_free;
        }

        /* TBF released. Unput PDUs in PAQ. */
        grlc_ul_unput_paq();

        /* Flush PDUs in main PDU queue that came from indicated SAPI. */
        flush_llc_pdu_queue_sapi( sapi);

        /*
        * Re-order signalling pdu to be sent first to NW
        */
        grlc_ul_reorder_pdus();

        /* Load cur_pdu_ptr with a PDU */
        (void)load_cur_pdu();

        /* Est TBF from ul.state = NULL */
        est_tbf_from_null();
      }
      else
      {
        /* There are PDUs from other SAPIs than the one requested */

        /* Unput PDU held in next_pdu_ptr into main UL queue */
        unput_next_pdu();

        /* Flush PDUs in main PDU queue that came from indicated SAPI. */
        flush_llc_pdu_queue_sapi( sapi);

        /* Freeze FIFO access for reprocessing */
        grlc_ul_fifo_disable_pl1_access();

        /* Some UL PDUs in ULQ might have been flushed. Do repeocessing now.*/
        reprocess( GRLC_REPROCESS_OTHER);

        /* Re-enable PL1 access to the FIFO */
        pl1_send_ph_data_req();

        /* Reprocessing completed. Enable FIFO */
        grlc_ul_fifo_enable_pl1_access();
      }
      break;
    }

    case US_AWAIT_ASS:
    {
      /* Unput PDU held in next_pdu_ptr into main UL queue */
      unput_next_pdu();

      /* Flush PDUs in main PDU queue that came from indicated SAPI. */
      flush_llc_pdu_queue_sapi( sapi);
      break;
    }


#ifdef  FEATURE_GSM_DTM
    case US_GTTP_XFER:
    {
      /* Flush PDUs in main PDU queue that came from indicated SAPI. */
      flush_llc_pdu_queue_sapi( sapi);
      break;
    }
#endif /* FEATURE_GSM_DTM */

    default:
    {
      MSG_GERAN_LOW_1_G("GRLU invalid ul state %d",ul.state);
      break;
    }

  } /* End of Switch (ul.state) */


  MSG_GERAN_LOW_3_G("GRLU PURGE_DATA_REQ: Post purge ULQ cnt=%d ULTQ cnt=%d OldQ cnt=%d"
            ,(uint16)q_cnt(&ULQ())
            ,(uint16)q_cnt(&ULTQ())
            ,(uint16)q_cnt(&grlc_ul_old_llc_q));

  /* Some UL PDUs may have been flushed, do flow control */
  grlc_llc_ul_flow_control();

} /* grr_purge_data_req */

/*===========================================================================
===
===  FUNCTION      GRR_RESUME_REQ()
===
===  DESCRIPTION
===
===    Processes the GPRS Resume Request from LLC. If there is no GMM-PDUs in
===    the ULQ and that Suspend state is active then go straight to Resume
===    state and ULQ is restored. If not then advance to RESUME_PENDING state.
===    When the last GMM-PDU has been delivered and ULQ is empty the Suspend
===    state will change from RESUME_PENDING to RESUME.
===
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
void grr_resume_req (void)
{
  /* LLC may send this signal more than once
  */
  if (ul.suspend_state == GRLC_SUSPEND)
  {

    ul.suspend_state = GRLC_RESUME;
    relink_data_pdus();

    MSG_GERAN_MED_2_G("GRLU SUSP Resume s=%d qcnt=%u", ul.state, (uint16)q_cnt(&ULQ()));

    /* Request to est UL if in the right state
    */
    if (ul.b_access &&
        ul.state == US_NULL
       )
    {
      (void)load_cur_pdu();
      est_tbf_from_null();
    }
    else
    {
      MSG_GERAN_LOW_1_G("GRLU RESUME state = %d NoTBFEst",ul.state);
    }
  }
  else
  {
    MSG_GERAN_LOW_3_G("GRLU resume_req not in SUSPEND susp_s=%d s=%d qcnt=%u",
            ul.suspend_state,
            ul.state,
            (uint16)q_cnt(&ULQ()));
  }

  /* grr_resume_req() is only called when there is a GMM initiated user-plane resumption.
     Therefore, send GRR_RESUME_CNF to LLC regardless of RLC suspension state to enable
     UL data flow. */
  send_resume_cnf_to_llc();


} /* grr_resume_req */

/*===========================================================================
===
===  FUNCTION      GRR_TEST_MODE_REQ()
===
===  DESCRIPTION
===
===    Processes request from LLC to establish an UL tbf for kick starting
===    the ETSI Test mode B or SRB.
===
===  PARAMS
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===     Note that this function is called from the LLC task thread
===
===========================================================================*/
void grr_test_mode_req( const gprs_grr_ul_mes_t *msg_ptr )
{

  /* pointer to message buffer to send to MAC
  */
  rlc_mac_msg_t       *rlc_msg_ptr;
  boolean egprs_8psk_ul_enabled = FALSE;

  /*---------------------------------------------------------------*/

  if( msg_ptr->test_mode == TEST_MODE_B )
  {
    MSG_GERAN_LOW_1_G("GRLU Est tbf req for Test Mode B ts_ofsset=%d", msg_ptr->ts_offset);
  }
  else if( msg_ptr->test_mode == TEST_MODE_SRB )
  {
    MSG_GERAN_LOW_2_G("GRLU Est tbf req for Test Mode SRB M_bit = %d ts_ofsset=%d",
             (boolean)msg_ptr->srb_loopback_mode, msg_ptr->ts_offset);
  }
  else
  {
    MSG_GERAN_LOW_1_G("GRLU INVALID Est tbf req for Test Mode=%d", msg_ptr->test_mode);
    return;
  }

  /* Read GCF Flag
  */
#if (!((defined(DEBUG_GSM_GPRS_DS_UNIT_TEST)) || (defined (PERLUTF))))
  /* rr_read_public_store(RRPS_MS_ANITE_GCF_FLAG, &ul.anite_gcf_flg, msg_ptr-> ); */
#else
  #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST/PERLUTF */

  /* Test mode B is running in UNACK mode
  */
  ul.rlc_mode = GRLC_MODE_UNACK;

  /*---------------------------------------------------------------
  ** Fill msg to send to MAC to req tbf. Use the info in the LLC
  ** message and not from the pdu in the queue.
  **
  ** This func also determine other input params such as TBF type
  ** (open/close), access mode (fixed,dyn) and access phase
  ** (1/2/short)
  **
  ** 'cause' is not relevant in unack mode and is therefore made 0
  **---------------------------------------------------------------
  */
  rlc_msg_ptr = mac_get_cmd_buf_rlc();
  if (rlc_msg_ptr != NULL)
  {
    /* Test mode entry */
    ul.test_mode = msg_ptr->test_mode;

    /*----------------------------------------
    ** Request MAC to est tbf.
    ** Set internal RLC to MAC signal timeout
    **----------------------------------------
    */
    rlc_msg_ptr->msg.ul_est_tbf_req.sig = RM_UL_EST_TBF_REQ;

    rlc_msg_ptr->msg.ul_est_tbf_req.test_mode = msg_ptr->test_mode;

    if( msg_ptr->test_mode == TEST_MODE_B )
    {
      rlc_msg_ptr->msg.ul_est_tbf_req.srb_mode = FALSE;
    }
    else if ( msg_ptr->test_mode == TEST_MODE_SRB )
    {
      rr_read_public_store(RRPS_MS_EGPRS_8PSK_UL_ENABLED,&egprs_8psk_ul_enabled);

      /* As per spec 4.14 , If MS supports 8PSK it shall ignore M Bit */
      if(egprs_8psk_ul_enabled)
      {
        rlc_msg_ptr->msg.ul_est_tbf_req.srb_mode = FALSE;
      }
      else
      {
        rlc_msg_ptr->msg.ul_est_tbf_req.srb_mode = (boolean)msg_ptr->srb_loopback_mode;
      }
    }

    #if  (!((defined (DEBUG_GSM_EGPRS_DS_UNIT_TEST)) || (defined (PERLUTF))))
    rlc_msg_ptr->msg.ul_est_tbf_req.ts_offset = msg_ptr->ts_offset;
    #endif /* DEBUG_GSM_EGPRS_DS_UNIT_TEST/PERLUTF */

    /* TLLI attached to LLC PDU causing this request
    */
    rlc_msg_ptr->msg.ul_est_tbf_req.tlli = msg_ptr->tlli;

    /* channel request description
    */
    rlc_msg_ptr->msg.ul_est_tbf_req.chan_req_desc.peak_thr_class = 0; /* N/A */
    rlc_msg_ptr->msg.ul_est_tbf_req.chan_req_desc.radio_priority = 3; /* N/A */
    rlc_msg_ptr->msg.ul_est_tbf_req.chan_req_desc.rlc_mode       = GRLC_MODE_UNACK;
    rlc_msg_ptr->msg.ul_est_tbf_req.chan_req_desc.llc_pdu_type   =
    RM_LLC_PDU_TYPE_NON_ACK_SACK;

    /* Set octet number > 9 so that a PRR is sent by MAC */
    rlc_msg_ptr->msg.ul_est_tbf_req.chan_req_desc.rlc_octet_cnt  = 10;

    rlc_msg_ptr->msg.ul_est_tbf_req.access_phase = GMAC_TWO_PHASE_ACCESS;

    rlc_msg_ptr->msg.ul_est_tbf_req.num_of_blocks = 0;  /* N/A */

    rlc_msg_ptr->msg.ul_est_tbf_req.tbf_type = RM_OPEN_ENDED_TBF;

    rlc_msg_ptr->msg.ul_est_tbf_req.data_req_cause = GRR_CAUSE_LLC_SIGNALLING;

    /* update ul state prior to sending message to MAC
    */
    ul.state = US_AWAIT_ASS;
    TIMER_START_AWAIT_ASS_GUARD();
    UPD_UL_STATE();

    /* Store away the params of the last UL est req
    */
    ul.last_req = rlc_msg_ptr->msg.ul_est_tbf_req;

    mac_put_cmd_rlc(rlc_msg_ptr);

    /* start request timeout
    */
    TIMER_START_MAC_UL_RESP();
  }
  else
  {
    ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
  }
}/* grr_test_mode_req */

/*===========================================================================
===
===  FUNCTION      ACCEPT_RESOURCE()
===
===  DESCRIPTION
===
===    Accept the resource granted by the n/w and store away the information.
===    Change the realloc_state according to whether resource assigned is:
===     1. to establish a new TBF.
===     2. in response to RLC request to extend the TBF
===     3. un-solicited assignment from the network.
===
===    Check for FINAL_ALLOCATION is done for all 3 cases.
===    Check for coding scheme change is done for cases 2 and 3
===    If tx is in resource exhaust state then case 3 un-halts the tx process
===
===  PARAMS
===
===    None
===
===  DEPENDENCIES
===
===    ul.state
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void accept_resource (void)
{
  /*---------------------------------------------
  ** Accept resource when establishing a new TBF
  **---------------------------------------------
  */

  /* Preset RALS state to NULL and refine it later on
  ** !
  */
  ul.realloc_state = RALS_NULL;
  UPD_UL_SUBSTATE();

  /* If Dynamic Open then the blocks_granted field is not filled in by MAC
  ** because RLC can have un-limited resource
  */
  if (IS_DYNAMIC_OPEN())
  {
    /* Allow an extra block
    */
    ul.cur_res.blocks_granted = 1 + ul.blocks_req;
  }

  /* If Contention has not been resolved then resets the N3014 counter to 0
  ** such that when the 1st data block is sent it will get incremented to 1
  */
  if (IS_TLLI_REQUIRED())
  {
    /* Set n3104 to zero. After the 1st block is transmitted n3104 will be
    ** incremented to 1 (in post_tx_actions())
    */
    RESET_N3104();
#ifdef  GSM_GPRS_GRLU_SHOW_N3104
    MSG_GERAN_MED_3_G("GRLU acc_res TLLI ResN3104=%d max=%d 1st_psn=%u",
            ul.n3104,
            N3104_MAX(),
            ul.first_psn_to_free);
#endif
  }

  /* Copy a few selective vars out, even for dyna open-ended
  */
  ul.blocks_granted = ul.cur_res.blocks_granted;

  /* In one-phase access mode, ul.tlli_cs should be assigned the coding scheme
  ** as follows:
  ** if alloc_cnf.tlli_block_cs = 0, tlli_cs = RM_CS_1/RM_MCS_1
  ** if alloc_cnf.tlli_block_cs = 1, tlli_cs = alloc_cnf.cs/alloc_cnf.mcs
  */
  if (ul.cur_res.ti == RM_TLLI_PRESENT)
  {
    if (ul.cur_res.tlli_block_cs == RM_TLLI_USE_CS_1)
      ul.tlli_cs = RM_CS_1; /* RM_CS_1 == RM_MCS_1*/
    else
      ul.tlli_cs = ul.cur_res.cs;
  }
  else
  {
    ul.tlli_cs = ul.cur_res.cs;
  }

  /* re-adjust blocks_req now we know the access phase & coding scheme
  ** blocks_req determines tbc and blocks_rem
  **
  ** ! b_next_pdu_avail is set to TRUE because at this point we may not have
  ** done group_pdus(). Also the purpose is to get a representative value for
  ** ul.num_blks and TRUE will give us a greater value
  */


  get_num_blks_req( ul.octets_req,
                    (uint8)(IS_TLLI_REQUIRED() ?
                            RM_TLLI_PRESENT :
                            RM_TLLI_NOT_PRESENT
                           ),
                    (uint16)(IS_DYNAMIC_OPEN() &&
                             IS_GT_MODULO_COMP(cur_pdu_ptr->pg_last_psn,
                                               cur_pdu_ptr->psn,
                                               32768
                                              )
                            )
                   );

  ul.blocks_req = ul.num_blks;

  /* Adjust the total number of blocks to be transferred in this TBF
  ** as well (TBC). This value ultimately effects CV values
  */
  pg_clr_group_info(cur_pdu_ptr);
  pg_clr_tbc_info( &pg.tbci);

  group_pdus();

  if( ul.cur_res.tbf_mode == TBF_MODE_EGPRS )
    e_pg_calc_xfer_value();
  else
    pg_calc_xfer_value();

  MSG_GERAN_MED_3("GRLU acc_res tbc=%u blocks_req=%u typ=%d",
          ul.tbc,
          pg.tbci.num_blks,
          ul.cur_res.tbf_type);

  /* Transfer to blocks_rem
  */
  ul.octets_rem = ul.octets_req;
  ul.blocks_rem = ul.blocks_req;

  /* After accepting the tbf est we now have: ul.cur_res struct
  ** plus: ul.blocks_req (adjusted according to assigned access phase)
  ** and ul.blocks_granted.
  **
  ** Note that ul.blocks_granted is not used in dyna open mode.
  */

  if (ul.cur_res.tbf_type == RM_CLOSE_ENDED_TBF)
  {
    MSG_GERAN_LOW_1_G("GRLU res_inf Dyn Close Final=%d",ul.cur_res.final_alloc);
  }
  else /* open */
  {
    MSG_GERAN_LOW_1_G("GRLU res_inf Dyn Open Final=%d",ul.cur_res.final_alloc);
  }

  /* Show blocks_granted, coding_scheme and retry
  */
  if( ul.cur_res.tbf_mode == TBF_MODE_EGPRS )
  {
    MSG_GERAN_LOW_3_G("GRLU res_inf blks_grtd=%d cs=%d retry=%d",
            ul.cur_res.blocks_granted,
            ul.cur_res.cs,
            ul.cur_res.retry
         );
  }
  else
  {
    MSG_GERAN_LOW_3_G("GRLU res_inf blks_grtd=%d cs=%d retry=%d",
              ul.cur_res.blocks_granted,
              ul.cur_res.cs,
              ul.cur_res.retry
           );
  }
} /* accept_resource */

/*===========================================================================
===
===  FUNCTION      REALLOC_FOR_NEXT_PDU()
===
===  DESCRIPTION
===
===    This function sends a reallocation request to MAC with the number
===    octets being the sum of the un-transmitted octets of the current
===    pdu and the length of the next pdu.
===
===    The number of blocks is also worked out and stored in
===    realloc_blocks_req.
===
===  PARAMETERS
===
===    b_cv_zero: indicates (to MAC) whether RLC has sent the block with CV = 0
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
void realloc_for_next_pdu
(
  const gprs_grr_ul_mes_t *pdu_ptr,
  boolean                 b_cv_zero
)
{
  /* pointer to message buffer
  */
  rlc_mac_msg_t   *rlc_msg_ptr;

  /*----------------------------------------------------------------*/

  /* Check for null pointer
  */
  if ( (NULL == pdu_ptr) ||
       (US_NULL == ul.state) ||
     /* After the re-allocation state machine determines the data block to send the reallocation
     ** request on, has been set. The transfer may will happen very quickly and the destined data
     ** block acknowledged as tramsmitted in L1 had been reached. A race condition may occur if the
     ** NW terminates the TBF and the transfer process will free the ack'ed data BEFORE the reallocation
     ** mechanism kicked in.. To detect this race condition, the PSN being freed is compared with the
     ** the PSN where reallocatiom is supposed to take place. If the PDU had already been freed, or
     ** the current state is NULL, then reallocation is aborted
     */
       ((GRLC_MODE_ACK == ul.rlc_mode) &&
        (IS_GE_MODULO_COMP(ul.psn_to_free_inclusive, ul.realloc_psn, 32768))
       ) ||
       /* un-ack mode uses a different var to track the PSN to be freed */
       ((GRLC_MODE_UNACK == ul.rlc_mode) &&
        (IS_GE_MODULO_COMP((ul.first_psn_to_free-1), ul.realloc_psn, 32768))
       )
     )
  {
    MSG_GERAN_LOW_3("GRLU RA ptr NULL or PSN(%u) freed(%u). State = %u",ul.realloc_psn, ul.psn_to_free_inclusive, ul.state);

    grlc_clear_realloc_vars();

    return;
  }

  /* length of next pdu in octets. pdu_ptr already checked for NULL
  */
  ul.next_pdu_octets = (uint16)GPRS_PDU_LENGTH(pdu_ptr->dup_pdu_ptr);

  /* octets request is the sum of remainder octets to send plus length of next pdu
  ** PLUS an extra extension octet
  */

  if(ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
  {
    ul.realloc_octets_req = (uint16)(ul.next_pdu_octets + 1);


    ul.realloc_blocks_req = e_get_num_blks( ul.realloc_octets_req,
                                       ul.cur_res.cs,
                                       ul.tlli_cs,
                                       RM_TLLI_NOT_PRESENT
                                      );

    /* Request open ended if there are more data pdu octets.*/
    if ( ul.realloc_octets_req >= 500 )
    {
      ul.realloc_octets_req = 0;
    }
  }
  else
  {
    ul.realloc_octets_req = (uint16)(ul.next_pdu_octets + 1);

    /* Use TRUE to get a larger estimate
    */
    ul.realloc_blocks_req = get_num_blks( ul.realloc_octets_req,
                                       ul.cur_res.cs,
                                       ul.tlli_cs,
                                       RM_TLLI_NOT_PRESENT,TRUE
                                      );

    /* Request open ended if there are more data pdu octets.*/
    if ( ul.realloc_octets_req >= 500 )
    {
      ul.realloc_octets_req = 0;
    }
  }

  /* use the macro below to fill the rest of the message to MAC.
  ** Since the request is to extend this TBF the info used by this
  ** macro comes from the current PDU.
  */
  rlc_msg_ptr = mac_get_cmd_buf_rlc();
  if (rlc_msg_ptr != NULL)
  {
    if( IS_PFC_NW_SUPPORTED() )
    {
      rlc_msg_ptr->msg.ul_realloc_req.pfi_present = TRUE;
      rlc_msg_ptr->msg.ul_realloc_req.pfi         = pdu_ptr->pfi;
      /* Latch latest PFI value send to NW */
      ul.pfi = pdu_ptr->pfi;
    }
    else
    {
      rlc_msg_ptr->msg.ul_realloc_req.pfi_present = FALSE;
      rlc_msg_ptr->msg.ul_realloc_req.pfi         = 0;
    }

#ifdef  FEATURE_GPRS_PS_HANDOVER
    if (ul.b_psho_active)
    {
      /* Suppress Reallocation during PSHO
      */
      MSG_GERAN_MED_0_G("GRLU suppress REALLOC during PSHO");
      GPRS_MEM_FREE(rlc_msg_ptr);
    }
    else
    {
      SEND_REALLOC_REQ( rlc_msg_ptr, pdu_ptr, ul.realloc_octets_req, b_cv_zero);
      /* Stop the Guard Timer as PRR retry process takes a long time
      */
      TIMER_STOP_AWAIT_UL_ACKNACK();
      ul.guard_timer_state =  GUARD_OFF;
      MSG_GERAN_HIGH_0_G("ERLU XTimer stopped sending realloc req");
    }
#else
    SEND_REALLOC_REQ( rlc_msg_ptr, pdu_ptr, ul.realloc_octets_req, b_cv_zero);
    /* Stop the Guard Timer as PRR retry process takes a long time
    */
    TIMER_STOP_AWAIT_UL_ACKNACK();
    ul.guard_timer_state =  GUARD_OFF;
    MSG_GERAN_HIGH_0_G("ERLU XTimer stopped sending realloc req");
#endif /* FEATURE_GPRS_PS_HANDOVER  */
  }
  else
  {
    ERR_GERAN_FATAL_0_G("GRLU heap exhaustion");
  }

  MSG_GERAN_HIGH_2_G("GRLU RA ra_octs=%d cv-zero=%d",ul.realloc_octets_req,b_cv_zero);

} /* realloc_for_next_pdu */

/*===========================================================================
===
===  FUNCTION      EST_TBF_FROM_NULL()
===
===  DESCRIPTION
===
===    This function establishes an UL TBF given the current PDU by sending
===    RM_UL_EST_TBF_REQ to MAC and entering US_AWAIT_ASS state.
===
===    This function is called from within the state machine. It expects
===    a valid pdu in cur_pdu. No backup is made of the current pdu.
===
===  DEPENDENCIES
===
===    expects cur_pdu to hold a valid LLC PDU in ACK or UNACK mode
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
============================================================================*/
void est_tbf_from_null (void)
{
  /* pointer to message buffer to send to MAC
  */
  rlc_mac_msg_t       *rlc_msg_ptr;
  uint16              octets = 0;

  /*---------------------------------------------------------------*/

  if (cur_pdu_ptr == NULL ||
      ul.b_access == FALSE
     )
  {
    MSG_GERAN_LOW_0_G("GRLU est_tbf PDU NULL or no-access");
    return;
  }

  /* Read GCF Flag
  */
#if(!((defined (DEBUG_GSM_GPRS_DS_UNIT_TEST)) || (defined (PERLUTF))))
  rr_read_public_store(RRPS_MS_ANITE_GCF_FLAG, &ul.anite_gcf_flg);
#else
  #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST/PERLUTF */

  /* copy mode of operation to the UL struct
  */
  ul.rlc_mode = cur_pdu_ptr->rlc_mode;

  /* if RLC mode is ACK then leave the cause value from LLC as is.
  ** Otherwise force it to GRR_CAUSE_LAY3_DATA to get ready to fill in
  ** request to establish TBF
  */
  if (ul.rlc_mode == GRLC_MODE_UNACK)
  {
    cur_pdu_ptr->cause = GRR_CAUSE_LAY3_DATA;
  }

  /*---------------------------------------------------------------
  ** Fill msg to send to MAC to req tbf. Use the info in the LLC
  ** message and not from the pdu in the queue.
  **
  ** This func also determine other input params such as TBF type
  ** (open/close), access mode (fixed,dyn) and access phase
  ** (1/2/short)
  **
  ** 'cause' is not relevant in unack mode and is therefore made 0
  **---------------------------------------------------------------
  */
  rlc_msg_ptr = mac_get_cmd_buf_rlc();
  if (rlc_msg_ptr != NULL)
  {
    /* If group is open and there are more than 1 PDUs then take the total octets in group
    */
    if (cur_pdu_ptr->pg_state != PGS_INIT &&
        cur_pdu_ptr->psn != cur_pdu_ptr->pg_last_psn
       )
    {
      octets = cur_pdu_ptr->pg_octets;
      MSG_GERAN_LOW_3_G("est_tbf octets=%u pg_octets=%u qcnt=%u",
              GPRS_PDU_LENGTH(cur_pdu_ptr->dup_pdu_ptr),
              octets,
              (uint16)q_cnt(&ULQ()));
    }
    else
      octets = (uint16)GPRS_PDU_LENGTH(cur_pdu_ptr->dup_pdu_ptr);

    fill_tbf_est_req_msg
    (
      
      rlc_msg_ptr,
      cur_pdu_ptr->llc_pdu_type,
      cur_pdu_ptr->peak_throughput,
      cur_pdu_ptr->radio_priority,
      octets,
      cur_pdu_ptr->tlli,
      cur_pdu_ptr->pfi,
      cur_pdu_ptr->cause
    );


    MSG_GERAN_MED_3_G("GRLU tbf_est rlc_mode=%d ptc=%d prio=%d",
              rlc_msg_ptr->msg.ul_est_tbf_req.chan_req_desc.rlc_mode,
              rlc_msg_ptr->msg.ul_est_tbf_req.chan_req_desc.peak_thr_class,
              rlc_msg_ptr->msg.ul_est_tbf_req.chan_req_desc.radio_priority
           );

  #ifdef  GSM_GPRS_GRLU_SHOW_ACC_PHASE
    MSG_GERAN_MED_3_G("GRLU         cause=%d phase=%d o/c=%d",
              rlc_msg_ptr->msg.ul_est_tbf_req.data_req_cause,
              rlc_msg_ptr->msg.ul_est_tbf_req.access_phase,
              rlc_msg_ptr->msg.ul_est_tbf_req.tbf_type
           );
  #endif

    MSG_GERAN_MED_3_G("GRLU         octs=%d blks=%d psn=%u",
              rlc_msg_ptr->msg.ul_est_tbf_req.chan_req_desc.rlc_octet_cnt,
              rlc_msg_ptr->msg.ul_est_tbf_req.num_of_blocks,
              cur_pdu_ptr->psn
           );
    /* Propagate test mode A info to MAC.
    */
    rlc_msg_ptr->msg.ul_est_tbf_req.test_mode = cur_pdu_ptr->test_mode;

    /*----------------------------------------
    ** Request MAC to est tbf.
    ** Set internal RLC to MAC signal timeout
    **----------------------------------------
    */
    rlc_msg_ptr->msg.ul_est_tbf_req.sig = RM_UL_EST_TBF_REQ;

    /* update ul state prior to sending message to MAC
    */
    ul.state = US_AWAIT_ASS;
    TIMER_START_AWAIT_ASS_GUARD();
    UPD_UL_STATE();

    /* Store away the params of the last UL est req
    */
    ul.last_req = rlc_msg_ptr->msg.ul_est_tbf_req;

  #ifdef  FEATURE_GSM_DTM
    /* Take the length of the current PDU in octets
    */
    rlc_msg_ptr->msg.ul_est_tbf_req.pdu_octets =
      (uint16)GPRS_PDU_LENGTH(cur_pdu_ptr->dup_pdu_ptr);
  #endif  /* FEATURE_GSM_DTM */

    mac_put_cmd_rlc( rlc_msg_ptr);

    /* start request timeout
    */
    TIMER_START_MAC_UL_RESP();
  }
  else
  {
    ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
  }

} /* est_tbf_from_null */

/*===========================================================================
===
===  FUNCTION      INDICATE_TO_LLC_RLCMAC_FAILURE()
===
===  DESCRIPTION
===
===    Send status indication to LLC to inform of RLCMAC failure
===    If RLC fails to transmit GMM/SM signaling PDU such that
===    if LAYER3_TX_CONFIRM is  defined then NAS can perform retry
===    on a selective basis.
===
===  PARAMS
===    gprs_grr_ul_mes_t
===
===  DEPENDENCIES
===
===    ul struct
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/
void indicate_to_llc_rlcmac_failure (  const gprs_grr_ul_mes_t *pdu_ptr)
{
  gprs_grr_dl_mes_t *llc_msg_ptr;

  /*-----------------------------------------------------*/
  MSG_GERAN_MED_3_G("GRLU GRR_STATUS_IND failure psn=%d cause=%d transaction_id=%d",pdu_ptr->psn, pdu_ptr->cause, pdu_ptr->transaction_id);

  /* send reject message to LLC with RLCMAC_FAILURE cause
  */
  llc_msg_ptr = GPRS_MEM_MALLOC(sizeof(gprs_grr_dl_mes_t));

  if (llc_msg_ptr == NULL)
  {
    ERR_GERAN_FATAL_0_G("GRLU REL malloc failure");
  }
  else
  {
    llc_msg_ptr -> prim  = GRR_STATUS_IND;
    llc_msg_ptr -> transaction_id    = pdu_ptr->transaction_id;
    llc_msg_ptr -> pdu_cause         = pdu_ptr->cause;
    llc_msg_ptr -> cause             = GRR_RLCMAC_FAILURE;

    gllc_grr_dl_put_mes_buf( llc_msg_ptr );
  }
} /* indicate_to_llc_rlcmac_failure */


/*===========================================================================
===
===  FUNCTION      SEND_RESUME_CNF_TO_LLC
===
===  DESCRIPTION
===
===    Send GRR_RESUME_CNF to LLC
===
===  PARAMS
===    gprs_grr_ul_mes_t
===
===  DEPENDENCIES
===
===    ul struct
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/
void send_resume_cnf_to_llc (void)
{
  gprs_grr_dl_mes_t *llc_msg_ptr;

  /*-----------------------------------------------------*/
  MSG_GERAN_MED_0_G("GRLU GRR_RESUME_CNF");

  /* send reject message to LLC with RLCMAC_FAILURE cause
  */
  llc_msg_ptr = GPRS_MEM_MALLOC(sizeof(gprs_grr_dl_mes_t));

  if (llc_msg_ptr == NULL)
  {
    ERR_GERAN_FATAL_0_G("GRLU REL malloc failure");
  }
  else
  {
    llc_msg_ptr -> prim   = GRR_RESUME_CNF;
    gllc_grr_dl_put_mes_buf( llc_msg_ptr );
  }
} /* send_resume_cnf_to_llc */


/*===========================================================================
===
===  FUNCTION      INDICATE_TO_LLC_RLCMAC_SUCCESS()
===
===  DESCRIPTION
===
===    Send status indication to LLC to inform of GMM/SM PDU tx success at RLC
===
===  PARAMS
===    gprs_grr_ul_mes_t
===
===  DEPENDENCIES
===
===    ul struct
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/
void indicate_to_llc_rlcmac_success (  const gprs_grr_ul_mes_t *pdu_ptr)
{
  gprs_grr_dl_mes_t *llc_msg_ptr;

  /*-----------------------------------------------------*/

  /* Check if success for PSN previously indicated */
  if(ul.psn_success_ind != pdu_ptr->psn)
  {
    MSG_GERAN_MED_3_G("GRLU GRR_STATUS_IND success psn=%d cause=%d transaction_id=%u",
      pdu_ptr->psn, pdu_ptr->cause, pdu_ptr->transaction_id);

    /* send status message to LLC with GRR_PDU_TX_SUCCESS cause
    */
    llc_msg_ptr = GPRS_MEM_MALLOC(sizeof(gprs_grr_dl_mes_t));

    if (llc_msg_ptr == NULL)
    {
      ERR_GERAN_FATAL_0_G("GRLU REL malloc failure");
    }
    else
    {
      ul.psn_success_ind       = pdu_ptr->psn;

      llc_msg_ptr -> prim              = GRR_STATUS_IND;

      llc_msg_ptr -> transaction_id    = pdu_ptr->transaction_id;
      llc_msg_ptr -> pdu_cause         = pdu_ptr->cause;
      llc_msg_ptr -> cause             = GRR_PDU_TX_SUCCESS;

      gllc_grr_dl_put_mes_buf( llc_msg_ptr );
    }
  }else
  {
    MSG_GERAN_LOW_1_G("GRLU GRR_STATUS_IND success already indicated for psn_success_ind=%d", ul.psn_success_ind );
  }

} /* indicate_to_llc_rlcmac_success */


/*===========================================================================
===
===  FUNCTION      GRLC_UL_PURGE_PDU_QUEUE
===
===  DESCRIPTION
===
===    Purge PDUs
===
===  PARAMS
===    None
===
===  DEPENDENCIES
===    ul struct
===
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_purge_pdu_queue (void)
{
  gprs_grr_ul_mes_t *pdu_ptr = NULL;

  MSG_GERAN_LOW_0_G("GRLU GPRS-TEST-MODE-A Release - Purge all PDUs");

  /* In IPA, on Abnormal release, RLC will be in US_NULL --> del all inc current PDU
  **         on Normal Release, RLC will be in XFER (to countdown) --> del all
  **         except current PDU
  */
  if (ul.state == US_NULL &&
      cur_pdu_ptr != NULL
     )
  {
    /* UN-Successful transmission of PDU , set the flag to FALSE
    ** so not to start ready timer
    */
    grlc_ul_del_free_paq_upto_psn( cur_pdu_ptr->psn,FALSE);
    cur_pdu_ptr = NULL;
  }

  dequeue_pdu( &pdu_ptr);
  while (pdu_ptr != NULL)
  {
    free_llc_pdu_and_msg(pdu_ptr);
    dequeue_pdu( &pdu_ptr);
  }

  if (next_pdu_ptr != NULL)
  {
    free_llc_pdu_and_msg(next_pdu_ptr);
    next_pdu_ptr = NULL;
  }

  ul.llc_pdus   = 0;
  ul.llc_octets = 0;

  RLC_UL_CLR_LLC_PDU_SIG();

} /* grlc_ul_purge_pdu_queue */

/*===========================================================================
===
===  FUNCTION      HANDLE_UL_RELEASE_EVENT()
===
===  DESCRIPTION
===
===    Handle uplink release which can be caused by MAC or from within RLC
===
===  PARAMS
===
===    ul_release_cause: cause value
===    b_from_mac: if TRUE indicates the release order came from MAC
===        otherwise the release is caused within RLC in which case RLC will
===        need to inform MAC of the release
===
===  DEPENDENCIES
===
===    ul struct
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/
void handle_ul_release_event
(
  rm_tbf_release_t  ul_release_cause,
  boolean           b_from_mac
)
{
  boolean           etsi_test_mode_is_active = FALSE;

  /*-----------------------------------------------------*/

  NOTUSED(b_from_mac);

  /*-----------------------------------------------------*/

  /* Inform LLC about TBF release in UL Unack Transfer in
     case Etsi Test Mode A or B is in progress, as this event
     is used to terminate the test. If test mode A is in
     progress then all outstanding PDUs must be purged
     including the current PDU. */

  if ( ul.state == US_UNACK_XFER )
  {
    etsi_test_mode_is_active = gllc_grr_ul_tbf_rel_ind();
  }

  /*-----------------------------------------------------*/

  /* Stop all timers on any release cause
  */
  clr_ul_timers();

  switch(ul_release_cause)
  {
    case REL_UL_ABNORMAL_WITHOUT_RETRY:
      if (cur_pdu_ptr != NULL)
      {
        MSG_GERAN_MED_3_G("GRLU AbRelWoRetry psn=%u cause=%u DEL_ONE",
                cur_pdu_ptr->psn,
                ul_release_cause,
                0);
      }
      grlc_ul_handle_no_ps_access( DEL_ONE);
      break;

    case REL_UL_ABNORMAL_WITH_ACCESS_RETRY:
    case REL_UL_ABNORMAL_CELL_RESEL:
      if (cur_pdu_ptr != NULL)
      {
        MSG_GERAN_MED_3_G("GRLU AbRelAccRetry/CellResel psn=%u cause=%u NO_DEL",
                cur_pdu_ptr->psn,
                ul_release_cause,
                0);
      }
      grlc_ul_handle_no_ps_access( NO_DEL);
      break;

    /*-------------------------------------------------------------
    ** Normal release. Proceed to CV count and release current TBF
    ** and a new pdu will start on a new tbf
    **-------------------------------------------------------------
    */
    case REL_UL_NORMAL:

      if (cur_pdu_ptr != NULL)
      {
        MSG_GERAN_HIGH_3_G("GRLU NormRel cur-psn=%u cur-tbc=%u cur-blks=%u",
                  cur_pdu_ptr->psn,
                  ul.tbc,
                  cur_pdu_ptr->pg_num_blks_cur_pdu);

        ul.b_forced_release = TRUE;

        /* Freeze FIFO access for reprocessing
        */
        grlc_ul_fifo_disable_pl1_access();

        reprocess( GRLC_REPROCESS_OTHER);

        /* Re-enable PL1 access to the FIFO
        */
        pl1_send_ph_data_req();
        grlc_ul_fifo_enable_pl1_access();

        ul.b_forced_release = FALSE;

        /* Force closing PDU grouping on the next PDU
        */
        MSG_GERAN_MED_3_G("GRLU NormRel PGS_CLOSE p=%u lp=%u tbc=%u",
                cur_pdu_ptr->psn,
                cur_pdu_ptr->pg_last_psn,
                ul.tbc);

        UPD_UL_SUBSTATE();
#ifndef FEATURE_GERAN_REDUCED_DEBUG
        ul_diag_abn_rel_cnts_st.arc_norm_rel_cnt++;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
      }
      break;

    default:
      MSG_GERAN_LOW_1_G("GRLU REL bad release cause %d",ul_release_cause);
      break;

  } /* switch on release cause */

  /*-----------------------------------------------------*/

  /* If Etsi Test Mode is active then purge all PDUs as the
  ** test has completed.
  */
  if ( etsi_test_mode_is_active )
  {
    MSG_GERAN_LOW_0_G("GRLU GPRS-TEST-MODE-Release - Purge all PDUs");
    grlc_ul_purge_pdu_queue();
  }
} /* handle_ul_release_event */


/*===========================================================================
===
===  FUNCTION      REPROCESS()
===
===  DESCRIPTION
===
===    This function reprocesses the pending transmit blocks. This is done to
===    allow grouping of the incoming PDU, ie. to extend the current TBF to
===    include another PDU. In ack mode, the PTX blocks if the FIFO are reprocessed.
===    In un-ack mode, the PAK blocks in the FIFO are reprocessed.
===    The function covers GPRS / EGPRS ACK / UNACK MODE
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/

void reprocess( grlc_reprocess_cause_t reprocess_cause )
{
  uint16  bsn, offset, psn;
  boolean b_reprocess = FALSE;


  /* Init the locals
  */
  bsn = 0;
  psn = cur_pdu_ptr->psn;
  offset = 0;

  grlc_ul_show_fifo_vars();
  MSG_GERAN_MED_3_G("GRLU reproc cause=%u psn=%u rem_ind=%u",
      reprocess_cause,
      cur_pdu_ptr->psn,
      cur_pdu_ptr->rem_ind);

#ifdef  GSM_GPRS_GRLU_SHOW_REALLOC
  MSG_GERAN_MED_3_G("GRLU reproc0 mode=%u ptx_cv=%u pak_cv=%u",
      ul.rlc_mode,
      ul.ptx_cv,
      ul.pak_cv);
  MSG_GERAN_MED_3_G("GRLU reproc0 psn=%u rem_ind=%u len=%u",
      cur_pdu_ptr->psn,
      cur_pdu_ptr->rem_ind,
      cur_pdu_ptr->len);
  MSG_GERAN_MED_3_G("GRLU reproc0 seg=%u vs=%u vsf=%u",
      ul.seg_ind,
      ul.vs,
      ul.vsf);
#endif /* GSM_GPRS_GRLU_SHOW_REALLOC */

  GRLC_UL_PROCEDURE_LOCK();
  if (ul.rlc_mode == GRLC_MODE_ACK)
  {
    if (ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
    {
      /*--------------------------
      ** Reprocess Ack EGPRS mode
      **--------------------------
      */
      /*-------------------------------------------------------------------------
      ** Seek the BSN of the oldest un-read PTX block to resegment from.
      **
      ** When change of mcs occurs PL1 may have read some PTX blocks.
      ** So we seek the BSN of the oldest un-read PTX block to resegment from.
      ** Since each block contains an offset field which maps that block to a
      ** position in the current PDU, we can take the backup PDU and strip off
      ** 'offset' octets and start re-segmenting from there. First, we need to
      ** find the offset held in the oldest UN-READ PTX block in the FIFO.
      ** If there are no un-read PTX blocks then we use V(S)
      **
      **               <-------> segmented blocks attached to TX_ARR( )
      ** the current transmit and attach windows:
      ** |-------|-----|--------|-------
      ** VA      VS   VSF      seg_ind
      **
      ** the blocks to take back is at VS.
      **
      ** The PTX area may be
      ** (1) empty: ptx_ctr = 0: resegment from VS (or VSF).
      ** (2) ptx_ctr = ptx_read_ctr > 0: ie there are blocks but all read by PL1.
      **     So resegment from VSF
      ** (3) ptx_ctr > ptx_read_ctr: ie there are un-read blocks. Delete un-read
      **     and resegment from the next ptx_read_out index.
      ** VS and VSF should be reset to this point
      **
      ** The FIFO may contain NACK and PACK blocks. The unread NACK and PACK
      ** blocks are deleted because they are out of context
      **-------------------------------------------------------------------------
      */

      /* Delete unread PACK and NACK blocks and restore pack_vsf and nack_vsf
      */
      grlc_ul_fifo_delete_unread_nack_pack_blocks( &ul.nack_vsf, &ul.pack_vsf);

      GRLC_UL_FIFO_SAVE_LOCK();

      /* PTX blocks available: case (2) and (3)
      */
      if (grlc_ul_fifo_get_ptx_cnt_dyn() > 0)
      {
        /* If there are un-read block(s) (case 3) then delete them and return the
        ** oldest's BSN. Otherwise leave 'bsn' unchanged.
        ** Function returns FALSE if there are no unread blocks (no delete)
        */
        if (grlc_ul_fifo_delete_unread_ptx_blocks( &bsn) == FALSE)
        {
          GRLC_UL_FIFO_SAVE_UNLOCK();

          /* case (2)
          **
          ** (2a) seg_ind is beyond vsf
          ** |------------|--------------|
          ** vs          vsf            seg
          **
          ** (2b) seg_ind coincides with vsf
          ** |------------|
          ** vs          vsf
          **             seg
          **
          ** |------------> ptx_ctr
          ** |------------> ptx_read_ctr
          **              ^
          **              bsn
          */
          bsn = ul.vsf;
#ifndef FEATURE_GERAN_REDUCED_DEBUG
          ul_diag_abn_rel_cnts_st.arc_chcs2_cnt++;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

          /* If bsn is inside seg_ind, we can get the offset and psn value using bsn
          ** as index
          */
          if (IS_GT_MOD(ul.seg_ind,bsn,ul.cur_res.ws))
          {
            offset = TX_ARR( bsn).blk.offset;
            psn = TX_ARR( bsn).blk.first_psn;
            b_reprocess = TRUE;
            MSG_GERAN_MED_3_G("ERLU Reproc A (2) NoDel vsf(bsn)=%d 1st_psn=%u ofs=%u",
                    bsn,
                    psn,
                    offset);
          }
          else
          {
            /* seg_ind == bsn then use current PDU and its remaining octets as offset
            */
            offset = cur_pdu_ptr->rem_ind + cur_pdu_ptr->pdu_ofs;
            psn = cur_pdu_ptr->psn;
            MSG_GERAN_HIGH_2_G("ERLU ##No Reproc A (2) bsn(%u) outside seg(%d)",bsn, ul.seg_ind);
          }
        }
        else
        {
          GRLC_UL_FIFO_SAVE_UNLOCK();

          /* case (3). Unread blocks deleted and bsn holds the oldest unread BSN
          **
          ** |------------|--------------|
          ** vs          vsf            seg (could be the same as vsf)
          **
          ** |------------> ptx_ctr
          ** |---------> ptx_read_ctr. After deleting the un-read blocks
          **
          ** |--------->    ptx_ctr
          ** |---------> ptx_read_ctr
          **           ^
          **           bsn
          **
          ** vsf and seg_ind could coincide with BSN being behind, both offset is valid
          */
          ul.vsf = bsn;

          /* bsn is well inside (behind) seg_ind so we can obtain offset and psn using
          ** bsn as index
          */
          offset = TX_ARR( bsn).blk.offset;
          psn = TX_ARR( bsn).blk.first_psn;
          MSG_GERAN_MED_3_G("ERLU Reproc A (3) Del bsn=%u 1st_psn=%u ofs=%u",bsn,psn,offset);
#ifndef FEATURE_GERAN_REDUCED_DEBUG
          ul_diag_abn_rel_cnts_st.arc_chcs3_cnt++;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

          b_reprocess = TRUE;

        } /* end (3) */
      } /* end (2) and (3) */
      else
      {
        GRLC_UL_FIFO_SAVE_UNLOCK();

        /* (1a) seg > vsf(bsn)
        **     -------|--------|
        **           vs       seg
        **           vsf
        **     bsn = vs = vsf.
        ** offset = rem_ind.
        ** Reclaim VS to seg-1
        **
        ** (1b) seg = vsf
        **     -------|---
        **           vs
        **           vsf
        **           seg
        **           bsn
        ** If segment_done then do nothing
        */
        bsn = ul.vs;
        ul.vsf = bsn;

#ifndef FEATURE_GERAN_REDUCED_DEBUG
        ul_diag_abn_rel_cnts_st.arc_chcs1_cnt++;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

        /* If bsn is inside seg_ind, we can obtain offset and psn using bsn as index
        */
        if (IS_GT_MOD(ul.seg_ind,bsn,ul.cur_res.ws))
        {
          offset = TX_ARR( bsn).blk.offset;
          psn = TX_ARR( bsn).blk.first_psn;
          b_reprocess = TRUE;
          MSG_GERAN_MED_3_G("ERLU Reproc A (1) empty TRUE bsn(vs)=%d vsf=%d psn=%u",
                  bsn,
                  ul.vsf,
                  psn);
        }
        else
        {
          /* seg_ind == bsn (seg_ind < bsn can not happen).
          ** If the current PDU ends here then wait for FAI
          */
          if (cur_pdu_ptr->rem_ind == cur_pdu_ptr->len)
          {
            if (cur_pdu_ptr->pg_last_psn == cur_pdu_ptr->psn ||
                cur_pdu_ptr->pg_state == PGS_CLOSE )
            {
              /* Current PDU ends here and group ends here so wait for FAI.
              ** No reprocessing
              */
              MSG_GERAN_MED_3_G("ERLU Reproc A (1) rem=%u len=%u CV=%d",
                      cur_pdu_ptr->rem_ind,
                      cur_pdu_ptr->len,
                      ul.cv);
              MSG_GERAN_MED_3_G("ERLU          (1) psn=%u last_p=%u",
                      cur_pdu_ptr->psn,
                      cur_pdu_ptr->pg_last_psn,
                      0);

                E_NEW_DATA_BLOCK( &cur_blk);

              if(reprocess_cause == GRLC_CHANGE_OF_CS_MCS)
              {
                grlc_ul_fifo_enable_pl1_access();
                pl1_send_ph_data_req();
              }

              ul.b_first_fifo_load = TRUE;
              TX_BLKS();
            }
            else
            {
              /* Group continues so load the next PDU and reprocess with the new CS
              */
              if (IS_GT_MODULO_COMP(cur_pdu_ptr->pg_last_psn,cur_pdu_ptr->psn,32768))
              {
                psn = (cur_pdu_ptr->psn+1)%32768;
                offset = 0;
                b_reprocess = TRUE;
                MSG_GERAN_MED_3_G("ERLU Reproc A (1) cont' cur=%u new_cur=%u last_p=%u",
                        cur_pdu_ptr->psn,
                        psn,
                        cur_pdu_ptr->pg_last_psn);
              }
            }
          }
          else
          {
            /* segment_ind = bsn AND len > rem_ind (current PDU is partial) so shorten it
            */
            MSG_GERAN_MED_3_G("ERLU Reproc A (1) rem(%u) != len(%u)",
                    cur_pdu_ptr->rem_ind,
                    cur_pdu_ptr->len,
                    0);
            psn = cur_pdu_ptr->psn;
            offset = cur_pdu_ptr->rem_ind + cur_pdu_ptr->pdu_ofs;
            b_reprocess = TRUE;
          }
        }
      } /* end (1) */


    } /* end Reprocess EGPRS mode */

    else
    {
      /*----------------------------------------
      ** Reprocess Ack GPRS mode in EGPRS build
      **----------------------------------------
      */

      /*-------------------------------------------------------------------------
      ** When change of coding scheme occurs PL1 may have read some PTX blocks.
      ** So we seek the BSN of the oldest un-read PTX block to resegment from.
      ** Since each block contains an offset field which maps that block to a
      ** position in the current PDU, we can take the backup PDU and strip off
      ** 'offset' octets and start re-segmenting from there. First, we need to
      ** find the offset held in the oldest UN-READ PTX block in the FIFO.
      ** If there are no un-read PTX blocks then we use V(S)
      **
      **               <-------> segmented blocks attached to TX_ARR( )
      ** the current transmit and attach windows:
      ** |-------|-----|--------|-------
      ** VA      VS   VSF      seg_ind
      **
      ** the blocks to take back is at VS.
      **
      ** The PTX area may be
      ** (1) empty: ptx_ctr = 0: resegment from VS (or VSF).
      ** (2) ptx_ctr = ptx_read_ctr > 0: ie there are blocks but all read by PL1.
      **     So resegment from VSF
      ** (3) ptx_ctr > ptx_read_ctr: ie there are un-read blocks. Delete un-read
      **     and resegment from the next ptx_read_out index.
      ** VS and VSF should be reset to this point
      **
      ** The FIFO may contain NACK and PACK blocks. The unread NACK and PACK
      ** blocks are deleted because they are out of context
      **-------------------------------------------------------------------------
      */

      /* Delete unread PACK and NACK blocks and restore pack_vsf and nack_vsf
      */
      grlc_ul_fifo_delete_unread_nack_pack_blocks( &ul.nack_vsf, &ul.pack_vsf);

      GRLC_UL_FIFO_SAVE_LOCK();

      /* PTX blocks available: case (2) and (3)
      */
      if (grlc_ul_fifo_get_ptx_cnt_dyn() > 0)
      {
        /* If there are un-read block(s) (case 3) then delete them and return the
        ** oldest's BSN. Otherwise leave 'bsn' unchanged.
        ** Function returns FALSE if there are no unread blocks (no deletes)
        */
        if (grlc_ul_fifo_delete_unread_ptx_blocks( &bsn) == FALSE)
        {
          GRLC_UL_FIFO_SAVE_UNLOCK();

          /* case (2)
          **
          ** (2a) seg_ind is beyond vsf
          ** |------------|--------------|
          ** vs          vsf            seg
          **
          ** (2b) seg_ind coincides with vsf
          ** |------------|
          ** vs          vsf
          **             seg
          **
          ** |------------> ptx_ctr
          ** |------------> ptx_read_ctr
          **              ^
          **              bsn
          */
          bsn = ul.vsf;
#ifndef FEATURE_GERAN_REDUCED_DEBUG
          ul_diag_abn_rel_cnts_st.arc_chcs2_cnt++;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

          /* If bsn is inside seg_ind, we can obtain offset and psn using bsn as index
          */
          if (IS_GT_MODULO_COMP(ul.seg_ind,bsn,SNS))
          {
            offset = TX_ARR( bsn).blk.offset;
            psn = TX_ARR( bsn).blk.first_psn;
            b_reprocess = TRUE;
            MSG_GERAN_MED_3_G("GRLU Reproc A (2) NoDel vsf(bsn)=%d 1st_psn=%u ofs=%u",
                     bsn,
                     psn,
                     offset);
          }
          else
          {
            /* seg_ind == bsn then use current PDU and its remaining octets as offset
            ** !.. use current pdu where it is no processing required
            */
            offset = cur_pdu_ptr->rem_ind + cur_pdu_ptr->pdu_ofs;
            psn = cur_pdu_ptr->psn;
            MSG_GERAN_HIGH_2_G("GRLU ##No Reproc A (2) bsn(%d) outside seg(%d)",bsn,ul.seg_ind);
          }
        }
        else
        {
          GRLC_UL_FIFO_SAVE_UNLOCK();

          /* case (3). Unread blocks deleted and bsn holds the oldest unread BSN
          **
          ** |------------|--------------|
          ** vs          vsf            seg (could be the same as vsf)
          **
          ** |------------> ptx_ctr
          ** |---------> ptx_read_ctr. After deleting the un-read blocks
          **
          ** |--------->    ptx_ctr
          ** |---------> ptx_read_ctr
          **           ^
          **           bsn
          **
          ** vsf and seg_ind could coincide with BSN being behind, both offset is valid
          */
          ul.vsf = bsn;
          UPD_UL_VSF();

          /* bsn is well inside (behind) seg_ind so we can obtain offset and psn using
          ** bsn as index
          */
          offset = TX_ARR( bsn).blk.offset;
          psn = TX_ARR( bsn).blk.first_psn;
          MSG_GERAN_MED_3_G("GRLU Reproc A (3) Del bsn=%d 1st_psn=%u ofs=%u",bsn,psn,offset);
#ifndef FEATURE_GERAN_REDUCED_DEBUG
          ul_diag_abn_rel_cnts_st.arc_chcs3_cnt++;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

          b_reprocess = TRUE;

        } /* end (3) */
      } /* end (2) and (3) */
      else
      {
        GRLC_UL_FIFO_SAVE_UNLOCK();

        /* (1a) seg > vsf(bsn)
        **     -------|--------|
        **           vs       seg
        **           vsf
        **     bsn = vs = vsf.
        ** offset = rem_ind.
        ** Reclaim VS to seg-1
        **
        ** (1b) seg = vsf
        **     -------|---
        **           vs
        **           vsf
        **           seg
        **           bsn
        ** If segment_done then do nothing
        */
        bsn = ul.vs;

        ul.vsf = bsn;
        UPD_UL_VSF();

#ifndef FEATURE_GERAN_REDUCED_DEBUG
        ul_diag_abn_rel_cnts_st.arc_chcs1_cnt++;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

        /* If bsn is inside seg_ind, we can obtain offset and psn using bsn as index
        */
        if (IS_GT_MODULO_COMP(ul.seg_ind,bsn,SNS))
        {
          offset = TX_ARR( bsn).blk.offset;
          psn = TX_ARR( bsn).blk.first_psn;
          b_reprocess = TRUE;
          MSG_GERAN_MED_3_G("GRLU Rep A (1) empty TRUE bsn(vs)=%d vsf=%d psn=%u",bsn,ul.vsf,psn);
        }
        else
        {
          /* seg_ind == bsn (seg_ind < bsn can not happen).
          ** If the current PDU ends here then wait for FAI
          */
          if (cur_pdu_ptr->rem_ind == cur_pdu_ptr->len)
          {
            if (cur_pdu_ptr->pg_last_psn == cur_pdu_ptr->psn ||
                cur_pdu_ptr->pg_state == PGS_CLOSE )
            {
              /* Current PDU ends here and group ends here so wait for FAI.
              ** No reprocessing
              */
              MSG_GERAN_MED_5_G("GRLU (1) psn=%u last_p=%u, CV=%d, rem=%u len=%u",
                cur_pdu_ptr->rem_ind,
                cur_pdu_ptr->len,
                ul.cv,
                cur_pdu_ptr->psn,
                cur_pdu_ptr->pg_last_psn);

              NEW_DATA_BLOCK( &cur_blk);

             if(reprocess_cause == GRLC_CHANGE_OF_CS_MCS)
             {
               grlc_ul_fifo_enable_pl1_access();
               pl1_send_ph_data_req();
             }
             ul.b_first_fifo_load = TRUE;
             TX_BLKS();
            }
            else
            {
              /* Group continues so load the next PDU and reprocess with the new CS
              */
              if (IS_GT_MODULO_COMP(cur_pdu_ptr->pg_last_psn,cur_pdu_ptr->psn,32768))
              {
                psn = (cur_pdu_ptr->psn+1)%32768;
                offset = 0;
                b_reprocess = TRUE;
                MSG_GERAN_MED_3_G("GRLU Reproc A (1) cont' cur=%u new_cur=%u last_p=%u",
                        cur_pdu_ptr->psn,
                        psn,
                        cur_pdu_ptr->pg_last_psn);
              }
            }
          }
          else
          {
            /* segment_ind = bsn AND len > rem_ind (current PDU is partial) so shorten it
            */
            MSG_GERAN_MED_3_G("ERLU Reproc A (1) rem(%u) != len(%u)",
                    cur_pdu_ptr->rem_ind,
                    cur_pdu_ptr->len,
                    0);
            psn = cur_pdu_ptr->psn;
            offset = cur_pdu_ptr->rem_ind + cur_pdu_ptr->pdu_ofs;
            b_reprocess = TRUE;
          }
        }
      } /* end (1) */

    } /* end GPRS Ack mode in EGPRS build */
  } /* end ACK mode */
  else
  {
  /*-----------------------
  ** Reprocess un-ack mode
  **-----------------------
  */
    if (ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
    {
      /*-----------------------------
      ** Reprocess Un-ack EGPRS mode
      **-----------------------------
      */
      /*-------------------------------------------------------------------------
      ** Only the PACK area is used in unack mode. Transmit window is from VS thro
      ** pack_vsf to seg_ind. VS moves forward after a PACK block transmitted.
      ** So the PACK area contains the difference between pack_vsf and VS.
      **
      ** If there are unread PACK blocks then delete them and resegment from the
      ** oldest un-read block.
      **
      ** Three cases:
      ** (1) pack_ctr = 0: No pack blocks so resegment from pack_VSF
      ** (2) pack_ctr > 0 and pack_ctr = pack_read_ctr. No unread blocks so
      **     resegment from pack_VSF.
      ** (3) pack_ctr > 0 and pack_ctr > pack_read_ctr. Delete un-read block(s)
      **     and resegment from the oldest un-read PACK block
      **
      ** Transmission in unack mode is strictly in BSN order. The oldest
      ** block in the FIFO will give us the BSN of the block where re-
      ** segmentation should start from. New V(S) and seg_ind should
      ** start from this point as well.
      **
      ** The un-ack transmit window is between V(S) and seg_ind
      **                       segmented blks
      **                       <----------->
      **     blks in PACK area
      **    <----------------->
      ** ---|------------------|------------|----
      **    VS                 pack_VSF     seg_ind
      **    x      x           x
      **
      ** case
      **   (1)    (3)         (2)
      **
      **-------------------------------------------------------------------------
      */

      GRLC_UL_FIFO_SAVE_LOCK();

      /* PACK blocks available: case (2) and (3)
      */
      if (grlc_ul_fifo_get_pack_cnt() > 0)
      {
        /* If there un-read block(s) (case 3) then delete them and return the
        ** oldest's BSN. Otherwise leave 'bsn' unchanged.
        ** Function returns FALSE if there are no unread blocks (no deletes)
        */
        if (grlc_ul_fifo_delete_unread_pack_blocks( &bsn) == FALSE)
        {
          GRLC_UL_FIFO_SAVE_UNLOCK();

          /* seg_ind is beyond vsf (2a) or coincides with it (2b)
          ** (2a)
          ** |------------|--------------|     OR
          ** vsf        p_vsf           seg
          ** (2b)
          ** |------------|
          ** vsf        p_vsf
          **             seg
          **
          ** |------------> pak_ctr
          ** |------------> pak_read_ctr
          **              ^
          **              |
          **              bsn
          */
          bsn = ul.pack_vsf;

          /* If bsn is inside seg_ind, we can obtain offset and psn using bsn as index
          */
          if (IS_GT_MOD(ul.seg_ind,bsn,ul.cur_res.ws))
          {
            offset = TX_ARR( bsn).blk.offset;
            psn = TX_ARR( bsn).blk.first_psn;
            b_reprocess = TRUE;
            MSG_GERAN_MED_3_G("ERLU Reproc U (2) NoDel p_vsf=%u p_vsf=bsn=%u seg=%u",
                    ul.pack_vsf,
                    bsn,
                    ul.seg_ind);
          }
          else
          {
            /* seg_ind == bsn then use current PDU and its remaining octets as offset
            */
            offset = cur_pdu_ptr->rem_ind + cur_pdu_ptr->pdu_ofs;
            psn = cur_pdu_ptr->psn;

            MSG_GERAN_HIGH_2_G("ERLU ##No Reproc U (2) bsn(%u) outside seg(%u)",bsn,ul.seg_ind);
          }
        }
        else
        {
          GRLC_UL_FIFO_SAVE_UNLOCK();

          /* case (3). Unread blocks deleted and bsn holds the oldest unread BSN
          **
          ** |------------|--------------|
          ** vsf         p_vsf          seg (could be the same as p_vsf)
          **
          ** |------------> pak_ctr
          ** |---------> pak_read_ctr. After deleting the un-read blocks
          **
          ** |--------->    pak_ctr
          ** |---------> pak_read_ctr
          **           ^
          **           |
          **           bsn
          */

          ul.pack_vsf = bsn;

          /* Drag VS back to pack_vsf if required
          */
          if (IS_GT_MOD(ul.vs,ul.pack_vsf,ul.cur_res.ws))
          {
            ul.vs = ul.pack_vsf;
          }

          /* bsn is well inside (behind) seg_ind so we can obtain offset and psn using
          ** bsn as index
          */
          offset = TX_ARR( bsn).blk.offset;
          psn = TX_ARR( bsn).blk.first_psn;
          MSG_GERAN_MED_3_G("ERLU Reproc U (3) Del bsn=%u vs=%u p_vsf=%u",bsn,ul.vs,ul.pack_vsf);
          b_reprocess = TRUE;
        } /* end case (3) */
      } /* end case (2) and case (3) */
      else
      {
        GRLC_UL_FIFO_SAVE_UNLOCK();

        /*
        ** case (1a) seg > vsf(bsn)
        **
        ** a)  -------|--------|
        **           vs       seg
        **          p_vsf
        **     bsn = vsf = p_vsf.
        ** offset = rem_ind.
        ** To take back blocks from vs to seg-1
        ** Keep current PDU and simply change len = len - rem_ind. rem_ind = 0.
        ** Re-group PDUs
        **
        ** case (1b) seg = vsf
        **
        ** b)  -------|---
        **           vsf
        **          p_vsf
        **           seg
        **           bsn
        ** offset = is not valid
        ** No blocks to take back.
        ** If segment_done then do nothing otherwise
        ** Keep current PDU and simply change len = len - rem_ind. rem_ind = 0.
        ** Re-group PDUs
        */
        bsn = ul.vs;
        ul.pack_vsf = bsn;

        /* If bsn is inside seg_ind, we can obtain offset and psn using bsn as index
        */
        if (IS_GT_MOD(ul.seg_ind,bsn,ul.cur_res.ws))
        {
          offset = TX_ARR( bsn).blk.offset;
          psn = TX_ARR( bsn).blk.first_psn;
          b_reprocess = TRUE;
          MSG_GERAN_MED_3_G("ERLU Reproc U (1) empty bsn=vs=%u p_vsf=%u seg=%u",
                  bsn,
                  ul.pack_vsf,
                  ul.seg_ind);
        }
        else
        {
          /* seg_ind == bsn (seg_ind < bsn can not happen).
          ** If the current PDU ends here then wait for FAI
          */
          if (cur_pdu_ptr->rem_ind == cur_pdu_ptr->len)
          {
            if (cur_pdu_ptr->pg_last_psn == cur_pdu_ptr->psn || ul.cv == 0)
            {
              /* Current PDU ends here and group ends here so wait for FAI.
              ** No reprocessing
              */
              MSG_GERAN_MED_5_G("ERLU Reproc U (1) rem=%u len=%u CV=%u, psn=%u last_p=%u",
                cur_pdu_ptr->rem_ind,
                cur_pdu_ptr->len,
                ul.cv,
                cur_pdu_ptr->psn,
                cur_pdu_ptr->pg_last_psn);

                E_NEW_DATA_BLOCK( &cur_blk);

              ul.b_first_fifo_load = TRUE;

              if(reprocess_cause == GRLC_CHANGE_OF_CS_MCS)
              {
                grlc_ul_fifo_enable_pl1_access();
                pl1_send_ph_data_req();
              }

              TX_BLKS();
            }
            else
            {
              /* Group continues so load the next PDU and reprocess with the new CS
              */
              if (IS_GT_MODULO_COMP(cur_pdu_ptr->pg_last_psn,cur_pdu_ptr->psn,32768))
              {
                psn = (cur_pdu_ptr->psn+1)%32768;
                offset = 0;
                b_reprocess = TRUE;
                MSG_GERAN_MED_3_G("ERLU Reproc U (1) cont' cur=%u new_cur=%u last_p=%u",
                        cur_pdu_ptr->psn,
                        psn,
                        cur_pdu_ptr->pg_last_psn);
              }
            }
          }
          else
          {
            /* segment_ind = bsn AND len > rem_ind (current PDU is partial) so shorten it
            */
            MSG_GERAN_MED_2_G("ERLU Reproc U (1) rem(%u) != len(%u)",
              cur_pdu_ptr->rem_ind,
              cur_pdu_ptr->len);
            psn = cur_pdu_ptr->psn;
            offset = cur_pdu_ptr->rem_ind + cur_pdu_ptr->pdu_ofs;
            b_reprocess = TRUE;
          }
        }
      }
    } /* end Un-ack EGPRS mode */
    else
    {
      /*-------------------------------------------
      ** Reprocess Un-ack GPRS mode in EGPRS build
      **-------------------------------------------
      */

      /*-------------------------------------------------------------------------
      ** Only the PACK area is used in unack mode. Transmit window is from VS thro
      ** pack_vsf to seg_ind. VS moves forward after a PACK block transmitted.
      ** So the PACK area contains the difference between pack_vsf and VS.
      **
      ** If there are unread PACK blocks then delete them and resegment from the
      ** oldest un-read block.
      **
      ** Three cases:
      ** (1) pack_ctr = 0: No pack blocks so resegment from pack_VSF
      ** (2) pack_ctr > 0 and pack_ctr = pack_read_ctr. No unread blocks so
      **     resegment from pack_VSF.
      ** (3) pack_ctr > 0 and pack_ctr > pack_read_ctr. Delete un-read block(s)
      **     and resegment from the oldest un-read PACK block
      **
      ** Transmission in unack mode is strictly in BSN order. The oldest
      ** block in the FIFO will give us the BSN of the block where re-
      ** segmentation should start from. New V(S) and seg_ind should
      ** start from this point as well.
      **
      ** The un-ack transmit window is between V(S) and seg_ind
      **                       segmented blks
      **                       <----------->
      **     blks in PACK area
      **    <----------------->
      ** ---|------------------|------------|----
      **    VS                 pack_VSF     seg_ind
      **    x      x           x
      **
      ** case
      **   (1)    (3)         (2)
      **
      **-------------------------------------------------------------------------
      */

      GRLC_UL_FIFO_SAVE_LOCK();

      /* PACK blocks available: case (2) and (3)
      */
      if (grlc_ul_fifo_get_pack_cnt() > 0)
      {
        /* If there un-read block(s) (case 3) then delete them and return the
        ** oldest's BSN. Otherwise leave 'bsn' unchanged.
        ** Function returns FALSE if there are no unread blocks (no deletes)
        */
        if (grlc_ul_fifo_delete_unread_pack_blocks( &bsn) == FALSE)
        {
          GRLC_UL_FIFO_SAVE_UNLOCK();

          /* seg_ind is beyond vsf (2a) or coincides with it (2b)
          ** (2a)
          ** |------------|--------------|     OR
          ** vsf        p_vsf           seg
          ** (2b)
          ** |------------|
          ** vsf        p_vsf
          **             seg
          **
          ** |------------> pak_ctr
          ** |------------> pak_read_ctr
          **              ^
          **              |
          **              bsn
          */
          bsn = ul.pack_vsf;

          /* If bsn is inside seg_ind, we can obtain offset and psn using bsn as index
          */
          if (IS_GT_MODULO_COMP(ul.seg_ind,bsn,SNS))
          {
            offset = TX_ARR( bsn).blk.offset;
            psn = TX_ARR( bsn).blk.first_psn;
            b_reprocess = TRUE;
            MSG_GERAN_MED_3_G("GRLU Reproc U (2) NoDel p_vsf=%d p_vsf=bsn=%d seg=%d",
              ul.pack_vsf,
              bsn,
              ul.seg_ind);
          }
          else
          {
            /* seg_ind == bsn then use current PDU and its remaining octets as offset
            */
            offset = cur_pdu_ptr->rem_ind + cur_pdu_ptr->pdu_ofs;
            psn = cur_pdu_ptr->psn;

            MSG_GERAN_HIGH_2_G("GRLU ##No Reproc U (2) bsn(%d) outside seg(%d)",bsn, ul.seg_ind);
          }
        }
        else
        {
          GRLC_UL_FIFO_SAVE_UNLOCK();

          /* case (3). Unread blocks deleted and bsn holds the oldest unread BSN
          **
          ** |------------|--------------|
          ** vsf         p_vsf          seg (could be the same as p_vsf)
          **
          ** |------------> pak_ctr
          ** |---------> pak_read_ctr. After deleting the un-read blocks
          **
          ** |--------->    pak_ctr
          ** |---------> pak_read_ctr
          **           ^
          **           |
          **           bsn
          */
          ul.pack_vsf = bsn;
          UPD_UL_VSF();

          /* Drag VS back to pack_vsf if required */
          if (IS_GT_MODULO_COMP(ul.vs,ul.pack_vsf,SNS))
          {
            ul.vs = ul.pack_vsf;
            UPD_UL_VS();
          }

          /* bsn is well inside (behind) seg_ind so we can obtain offset and psn using
          ** bsn as index
          */
          offset = TX_ARR( bsn).blk.offset;
          psn = TX_ARR( bsn).blk.first_psn;
          MSG_GERAN_MED_3_G("GRLU Reproc U (3) Del bsn=%d vs=%d p_vsf=%d",bsn,ul.vs,ul.pack_vsf);
          b_reprocess = TRUE;
        } /* end case (3) */
      } /* end case (2) and case (3) */
      else
      {
        GRLC_UL_FIFO_SAVE_UNLOCK();

        /*
        ** case (1a) seg > vsf(bsn)
        **
        ** a)  -------|--------|
        **           vs       seg
        **          p_vsf
        **     bsn = vsf = p_vsf.
        ** offset = rem_ind.
        ** To take back blocks from vs to seg-1
        ** Keep current PDU and simply change len = len - rem_ind. rem_ind = 0.
        ** Re-group PDUs
        **
        ** case (1b) seg = vsf
        **
        ** b)  -------|---
        **           vsf
        **          p_vsf
        **           seg
        **           bsn
        ** offset = is not valid
        ** No blocks to take back.
        ** If segment_done then do nothing otherwise
        ** Keep current PDU and simply change len = len - rem_ind. rem_ind = 0.
        ** Re-group PDUs
        */
        bsn = ul.vs;

        ul.pack_vsf = bsn;
        UPD_UL_VSF();

        /* If bsn is inside seg_ind, we can obtain offset and psn using bsn as index
        */
        if (IS_GT_MODULO_COMP(ul.seg_ind,bsn,SNS))
        {
          offset = TX_ARR( bsn).blk.offset;
          psn = TX_ARR( bsn).blk.first_psn;
          b_reprocess = TRUE;
          MSG_GERAN_MED_3_G("GRLU Reproc U (1) empty bsn=vs=%d p_vsf=%d seg=%d",
                  bsn,
                  ul.pack_vsf,
                  ul.seg_ind);
        }
        else
        {
          /* seg_ind == bsn (seg_ind < bsn can not happen).
          ** If the current PDU ends here then wait for FAI
          */
          if (cur_pdu_ptr->rem_ind == cur_pdu_ptr->len)
          {
            if (cur_pdu_ptr->pg_last_psn == cur_pdu_ptr->psn || ul.cv == 0)
            {
              /* Current PDU ends here and group ends here so wait for FAI.
              ** No reprocessing
              */
              MSG_GERAN_MED_5_G("GRLU Reproc U (1) rem=%u len=%u CV=%d,psn=%u last_p=%u",
                cur_pdu_ptr->rem_ind,
                cur_pdu_ptr->len,
                ul.cv,
                cur_pdu_ptr->psn,
                cur_pdu_ptr->pg_last_psn);

              NEW_DATA_BLOCK( &cur_blk);
              ul.b_first_fifo_load = TRUE;

              if (reprocess_cause == GRLC_CHANGE_OF_CS_MCS)
              {
                grlc_ul_fifo_enable_pl1_access();
                pl1_send_ph_data_req();
              }
              TX_BLKS();
            }
            else
            {
              /* Group continues so load the next PDU and reprocess with the new CS
              */
              if (IS_GT_MODULO_COMP(cur_pdu_ptr->pg_last_psn,cur_pdu_ptr->psn,32768))
              {
                psn = (cur_pdu_ptr->psn+1)%32768;
                offset = 0;
                b_reprocess = TRUE;
                MSG_GERAN_MED_3_G("GRLU Reproc U (1) cont' cur=%u new_cur=%u last_p=%u",
                        cur_pdu_ptr->psn,
                        psn,
                        cur_pdu_ptr->pg_last_psn);
              }
            }
          }
          else
          {
            /* segment_ind = bsn AND len > rem_ind (current PDU is partial) so shorten it
            */
            MSG_GERAN_MED_3_G("GRLU Reproc U (1) rem(%u) != len(%u)",
                    cur_pdu_ptr->rem_ind,
                    cur_pdu_ptr->len,
                    0);
            psn = cur_pdu_ptr->psn;
            offset = cur_pdu_ptr->rem_ind + cur_pdu_ptr->pdu_ofs;
            b_reprocess = TRUE;
          }
        }

      }

    } /* end Un-ack GPRS mode in EGPRS build */
  }/* end Un-ack mode */


  /* For rlc edge ack and unack mode */
  if (ul.cur_res.tbf_mode == TBF_MODE_EGPRS)
  {
    if (b_reprocess)
    {
      /* Reclaim segmented blocks and restore PDU from given offset
      */
      e_grlc_ul_unsegment( bsn);
      grlc_ul_paq_make_psn_current( psn, offset);

      /*---------------------------------------------------------------
      ** Update relevant parameters (coding scheme and reprocess)
      **---------------------------------------------------------------
      */
        E_NEW_DATA_BLOCK( &cur_blk);
      recalc_cv_related_values( reprocess_cause);

      /* There were segmented blocks so segment a few blocks.
      */
      ul.segment_state = e_segment_pdu();

      if (reprocess_cause == GRLC_CHANGE_OF_CS_MCS)
      {
        segment_loop( FALSE);
      }
      else
      {
        segment_loop( TRUE);
      }

      ul.b_first_fifo_load = TRUE;
      grlc_ul_fifo_enable_pl1_access();
      pl1_send_ph_data_req();

      TX_BLKS();
    }
    else
    {
      ul.b_first_fifo_load = TRUE;
      if (reprocess_cause == GRLC_CHANGE_OF_CS_MCS)
      {
        E_NEW_DATA_BLOCK( &cur_blk);
        grlc_ul_fifo_enable_pl1_access();
        pl1_send_ph_data_req();
      }
    }
  }
  else
  {
    /* GPRS reprocess ack and unack mode
    */
    if (b_reprocess)
    {
      /* Reclaim segmented blocks and restore PDU from given offset
      */
      grlc_ul_unsegment( bsn);
      grlc_ul_paq_make_psn_current( psn, offset);

      /*---------------------------------------------------------------
      ** Update relevant parameters ( like update coding scheme and reprocess)
      **---------------------------------------------------------------
      */
      NEW_DATA_BLOCK( &cur_blk);
      recalc_cv_related_values( reprocess_cause);
      ul.segment_state = segment_pdu();

      if (reprocess_cause == GRLC_CHANGE_OF_CS_MCS)
      {
        segment_loop( FALSE);
      }
      else
      {
        segment_loop( TRUE);
      }

      ul.b_first_fifo_load = TRUE;
      grlc_ul_fifo_enable_pl1_access();
      pl1_send_ph_data_req();

      TX_BLKS();
    }
    else
    {
      ul.b_first_fifo_load = TRUE;
      if (reprocess_cause == GRLC_CHANGE_OF_CS_MCS)
      {
        grlc_ul_fifo_enable_pl1_access();
        NEW_DATA_BLOCK( &cur_blk);
        pl1_send_ph_data_req();
      }
    }
  }
  UPD_UL_VSF();
  GRLC_UL_PROCEDURE_UNLOCK();

#ifdef  GSM_GPRS_GRLU_SHOW_REALLOC
  MSG_GERAN_MED_3_G("GRLU reproc1 psn=%u rem_ind=%u len=%u",
      cur_pdu_ptr->psn,
      cur_pdu_ptr->rem_ind,
      cur_pdu_ptr->len);
  MSG_GERAN_MED_3_G("GRLU reproc1 seg=%u vs=%u vsf=%u",
          ul.seg_ind,
          ul.vs,
          ul.vsf);
  MSG_GERAN_MED_3_G("GRLU reproc1 ptx_cv=%u pak_cv=%u pack_vsf=%u",
          ul.ptx_cv,
          ul.pak_cv,
          ul.pack_vsf);
  MSG_GERAN_MED_3_G("GRLU reproc1 tbc=%u psn=%u last_psn=%u",
          ul.tbc,
          cur_pdu_ptr->psn,
          cur_pdu_ptr->pg_last_psn);

#endif /* GSM_GPRS_GRLU_SHOW_REALLOC */
} /* end of reprocess */


/*** EOF: don't remove! ***/
