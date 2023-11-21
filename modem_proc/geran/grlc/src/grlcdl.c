/*****************************************************************************
***
*** TITLE
***
***  GPRS Radio Link Control Downlink Module
***
*** DESCRIPTION
***
***  This module contains the state machine, the event handler and initialisation
***  functions for the GPRS RLC downlink module
***
***
*** EXTERNALIZED FUNCTIONS
***
***  grlc_dl_one_time_int()
***
***  grlc_dl_handle_pl1_sig()
***  grlc_dl_handle_mac_sig()
***  grlc_dl_handle_tmr_sig()
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  grlc_dl_one_time_int()
***
*** Copyright (c) 2001-2014 Qualcomm Technologies, Inc.
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
*** $Header: //components/rel/geran.mpss/5.2.0/grlc/src/grlcdl.c#6 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 03/28/01    hv     Created
*** 07/10/01    hv     Added RRBP and frame no to pkt_acknack sent to MAC
*** 10/25/01    hv     Enabled timers. Added FBI-latch. DSM'ised L1-FIFO.
***                    Added signal to MAC on first received data block.
***                    Corrected init fns.
*** 01/29/13    hv     CR447071 - RLC per-tbf memory allocation
*** 20/08/13    hv     CR531564
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "comdef.h"
#include "gmutex.h"
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
#include "geran_eng_mode_info.h"
#include "geran_tasks.h"
#include "geran_multi_sim.h"
#include "geran_multi_sim_api.h"
#include "grlc_dyn_api.h"

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/

/*-----------------------------------------------------------------------
** Definition of durations in ms for RLC DL T3192 timer. The duration
** is obtained using the system information t3192 variable which ranges
** from 0 to 7 to index the array below.
**-----------------------------------------------------------------------
*/
static const uint16  t3192_dur_ms[] =
{
  500,
  1000,
  1500,
  0,
  80,
  120,
  160,
  200
};

/*****************************************************************************
***
***     Public Data
***
*****************************************************************************/

/*----------------------------------------------------------
** downlink data struct
** Contains most data required for RLC downlink processing
**----------------------------------------------------------
*/
grlc_dl_t  dl;


/*---------------------------------------------------------------
** Critical section to protect messages coming into GRLC DL from
** PL1 & MAC
**---------------------------------------------------------------
*/

rex_crit_sect_type grlc_dl_msg_crit_sec;


/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/

/********************************************************************
**
**    Forward Declarations
**
*********************************************************************
*/
static void grlc_dl_panic_reset (void);
static void grlc_dl_unblock_tuneaway (void);

/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      grlc_dl_create_rx_array
===
===  DESCRIPTION   allocate the memory for the Receive Array off the heap
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS  since HOST test forces one_time initialisation directly
===                from the testcase, this function is called the 2nd time
===                which is not desirable. Hence the check for the pointer
===                already initialised and allocation is then skipped
===
===========================================================================*/
static void grlc_dl_create_rx_array (   uint16 rx_workspace )
{
  if (NULL != p_rx_arr)
  {
    MSG_GERAN_LOW_0_G("p_rx_arr already initialised");
  }
  else
  {
    p_rx_arr = (grlc_dl_data_block_t (*)[]) (EXTPL_MEM_MALLOC(sizeof(grlc_dl_data_block_t) * rx_workspace));

    if (NULL != p_rx_arr)
    {
      MSG_GERAN_MED_0_G("RX array allocated OK");
      (void)memset((void*)p_rx_arr, 0, (sizeof(grlc_dl_data_block_t)*rx_workspace));
    }
    else
    {
      ERR_GERAN_FATAL_0_G("ERLD failed to allocate rx array");
    }
  }
} /* grlc_dl_create_rx_array */

/*===========================================================================
===
===  FUNCTION      GRLC_DL_FREE_RX_ARRAY
===
===  DESCRIPTION   FREE memory for Receive Array
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===    none
===
===========================================================================*/
void grlc_dl_free_rx_array(void)
{
  if(NULL != p_rx_arr)
  {
    MSG_GERAN_LOW_0_G("ERLD DL RX Workspace free");
    EXTPL_MEM_FREE (p_rx_arr);

    /* No access to rx_arr is allowed till
    ** another allocation can happen
    */
    p_rx_arr = NULL;
  }
} /* grlc_dl_free_rx_array */

/*===========================================================================
===
===  FUNCTION      GRLC_DL_START_T3192()
===
===  DESCRIPTION
===
===    Start the T3192 timer if its b_allow_t3192_to_start flag is set.
===    Reset the flag afterwards so PL1 can not set it again until RLC allows
===    it to.
===
===    This is provided to PL1 so it can start this timer when it sends the
===    Packet Downlink Ack/nack with the FAI bit set. This is only done if
===    RLC has set the flag when it receives the 'last' data block with the
===    Polling bit (SP) set and the FBI bit set and, that RLC is about to send
===    the Packet Downlink Ack/nack with the FAI bit set.
===
===  MODIFICATIONS
===
===    Starting or re-starting of T3192 is nolonger conditional to the
===    ul.b_allow_t3192_to_start flag
===
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
void grlc_dl_start_t3192_dyn(void)
{

  /* Refresh RM_TMR_T3192_DUR
  */
  rr_read_public_store(RRPS_CELL_OPTIONS_T3192, &sysinfo.t3192_index);

  if (dl.state == DS_RX_ACK_PENDING_RELEASE ||
      dl.state == DS_RX_UNACK_PENDING_RELEASE
     )
  {
  /* Locking of interrupts is not required since only PL1 calls this function
  */
  TIMER_START_T3192();
  dl.b_allow_t3192_to_start = FALSE;
  }
  else
  {
    MSG_GERAN_ERROR_1_G("ERLD t3192 not started state=%u", dl.state);
  }
}

/*===========================================================================
===
===  FUNCTION      GRLC_DL_TIMER_CALLBACK()
===
===  DESCRIPTION
===
===    RLC downlink timer callback function.
===    When a RLC downlink timer expires, a timer message is created which
===    carries the timer id passed on by the parameter of this function.
===    This function creates a downlink timer message and fills in the timer
===    id and queue up the message on the timer list.
===
===  DEPENDENCIES
===
===    dl timer queue is initialised and ready to accept queue item.
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
static void grlc_dl_timer_callback (const uint32 timer_param)
{
  rm_timer_msg_t    *timer_msg_ptr;

  /*---------------------------------------------------------------
  ** send RLC DL task timer signal as specified in timer_param
  **---------------------------------------------------------------
  */

  /* get buffer memory for the command
  */
  timer_msg_ptr = GPRS_MEM_MALLOC(sizeof(rm_timer_msg_t));

  /* if it's NULL shout
  */
  if (timer_msg_ptr == NULL)
  {
    ERR_GERAN_FATAL_0("GRLD tmr-callback malloc failure");
  }
  else
  {

    /* copy param to command buffer
    */
    timer_msg_ptr->sig = (rm_timer_t)TIMER_GET_ID(timer_param);

    /* init q item by pointing to data area
    */
    (void)q_link(timer_msg_ptr, &timer_msg_ptr->link);

    /* add command to rlc ul MAC queue
    */
    q_put(&DTQ(), &timer_msg_ptr->link);

    /* set signal mask as the final action in posting the message
    */
    RLC_DL_SET_TMR_SIG();
  }
} /* grlc_dl_timer_callback */

/*===========================================================================
===
===  FUNCTION       GRLC_DL_RELEASE()
===
===  DESCRIPTION
===
===    Perform release of the downlink TBF. This applies to both abnormal and
===    normal as the actions are the same.
===
===    This function and related code was put in to anticipate MAC sending
===    an explicit release signal (MR_DL_RELEASE_IND) during Cell Reselection.
===    MAC is to determine if this signal is required. In not this handler
===    and associated code will be removed.
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
===========================================================================*/
void grlc_dl_release (void)
{
  /*-----------------------------------------------------------------
  ** Read out all un-read blocks and free DSM item for each to drain
  ** out un-read items in the FIFO. This is required before starting
  ** a new TBF
  **-----------------------------------------------------------------
  */
  grlc_dl_init_vars();
  grlc_dl_fifo_clear();
#ifdef FEATURE_GPRS_PS_HANDOVER
  grlc_dl_clr_psho_vars();
#endif /* FEATURE_GPRS_PS_HANDOVER */
  dl.state = DS_NULL;
  UPD_DL_STATE();

  /* un-block tuneaway if it were blocked */
  grlc_dl_unblock_tuneaway();
} /* grlc_dl_release */

/*===========================================================================
===
===  FUNCTION      GRLC_DL_BLOCK_TUNEAWAY
===
===  DESCRIPTION
===
===    Send vote against tune away operation during downlink un-ack mode xfer
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
===========================================================================*/
static void grlc_dl_block_tuneaway (void)
{
  if (!dl.b_block_tuneaway)
  {
    dl.b_block_tuneaway = TRUE;
    geran_api_set_tuneaway_blocked(GERAN_API_CLIENT_RLC, TRUE);
    MSG_GERAN_LOW_1_G("ERLD block tuneaway %d", dl.b_block_tuneaway);
  }
}

/*===========================================================================
===
===  FUNCTION      GRLC_DL_UNBLOCK_TUNEAWAY
===
===  DESCRIPTION
===
===    Send vote FOR tune away operation during downlink ack mode xfer
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
===========================================================================*/
static void grlc_dl_unblock_tuneaway (void)
{
  if (dl.b_block_tuneaway)
  {
    dl.b_block_tuneaway = FALSE;
    geran_api_set_tuneaway_blocked(GERAN_API_CLIENT_RLC, FALSE);
    MSG_GERAN_LOW_1_G("ERLD unblock tuneaway %d", dl.b_block_tuneaway);
  }
}

/*===========================================================================
===
===  FUNCTION      RLC_DL_STATE_MACHINE
===
===  DESCRIPTION
===
===    RLC downlink state machine.
===
===    Expects signals in order of priority from L1, MAC and timer services
===    each in their own REX queue: grlc_dl_mac_q and grlc_dl_tmr_q.
===    When a signal is active, the event variable identifies the source of
===    the signal. Depending on the current downlink state dl.state, the
===    signal is then serviced
===
===    There are 3 main states:
===
===    - DS_NULL (downlink state null),
===    - DS_RX (downlink receive) and
===    - DS_RX_ACK/UNACK_PENDING_RELEASE.
===
===    During DS_NULL, the only signal expected from MAC is to establish
===    a downlink tbf in ack or un-ack mode. The state becomes DS_RX after
===    a downlink assignment indication from MAC. The assignment info is
===    stored and the current workspace pdu cur_dl_pdu struct and the
===    temporary cur_dl_blk struct is initialised ready to accept data.
===    The T3190_AWAIT_VALID_DL_DATA timer is started.
===
===    During DS_RX state in ack mode, the raw data is translated into a
===    temporary internal data block cur_dl_block. This is then copied to
===    the downlink queue ? blk_q. The state variables are then updated
===    and if polled, rlc sends the packet dl acknack msg to the peer
===    entity via MAC.
===
===    When the Final Block Indicator is set, and all blocks are received,
===    the dl queue is then assembled into pdu(s) and sent to LLC. The
===    packet dl acknack with Final Ack Indicator set to 1 will be sent to
===    the peer and the timer is set and rlc then enters
===    DS_RX_PENDING_RELEASE state.
===
===    There is an option to identify a complete and contiguous group of
===    blocks belonging to a pdu and send it off before a tbf terminates.
===    This will save storage space and may be implemented later on.
===
===    In un-ack mode of DS_RX state, the raw data block translated into
===    a temporary data block cur_dl_block. If the bsn of this block is
===    contiguous with the previous block, the block is assembled into
===    the current pdu and and left there until the next block arrives.
===    If the bsn of this block puts it before the last succefully
===    assembled block, it is out of sequence and shall be ignored. If the
===    bsn of this block is greater than the previously succesfully
===    assembled block. A series of blocks with filler octets will be
===    created and assembled together with this block. In other words, the
===    missing blocks are filled in to preserve the data length. During
===    assembling of blocks, if the a complete pdu is found it will be sent
===    off to LLC and a new pdu is created to take the remaining blocks.
===    If a pdu's length exceeds the prescribed 1536 octets limit. The pdu
===    is sent off to LLC truncated. The remaining blocks following the
===    pdu shall be ignored until the next pdu boundary. This also applies
===    to ack mode operation.
===
===    During DS_RX_PENDING_RELEASE state, if the appriopriate timer
===    expires, the tbf shall be terminated and MAC shall be informed.
===    If a downlink occurs in this state, the current tbf is terminated
===    and the new tbf is acted upon as it were in DS_NULL state.
===
===    In ack mode, the data blocks are put in the dl queue waiting for
===    all blocks to be received before being assembled into pdu(s).
===    In un-ack mode, each block is assembled into the current pdu as
===    it comes in. Missing blocks revealed by looking at the bsn of the
===    last block and the new block are filled in and assembled into the
===    current pdu. When the pdu completes it is sent off to LLC and a
===    new pdu is created.
===
===  DEPENDENCIES
===
===    all rlc dl module vars
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/
static void grlc_dl_state_machine (void)
{
  /* pointer to buffer for holding messages to send to MAC
  */
  rlc_mac_msg_t         *rlc_msg_ptr;

  /* pointer to incoming MAC message
  */
  mac_rlc_msg_t         *mac_msg_ptr;

  /* pointer to incoming timer message
  */
  rm_timer_msg_t        *timer_msg_ptr;

  /* pointer to incoming L1 message
  */
  pl1_rlc_msg_t         *pl1_msg_ptr;

#ifdef FEATURE_GPRS_PS_HANDOVER
  /* Holds the EPDAN REQ message from PL1
  */
  pl1_rlc_msg_t           *epdan_req_ptr;
#endif /*FEATURE_GPRS_PS_HANDOVER */
  /*---------------------------------------------------------------*/

  switch(dl.state)
  {
    /*=========================
    **  N U L L    S T A T E
    **=========================
    */
    case DS_NULL:
      /*-------------------------------------------------------
      ** Expect signal from MAC only to assign a downlink tbf
      **-------------------------------------------------------
      */

      /*================================
      ** State: DS_NULL, event: DE_MAC
      **================================
      */
      if (dl.event == DE_MAC)
      {
        /* get command
        */
        if ( (q_cnt(&DMQ())) == 0)
        {
          MSG_GERAN_LOW_0_G("GRLD MAC event without msg");
        }

        mac_msg_ptr = (mac_rlc_msg_t *)q_get(&DMQ());

        /* shout if is NULL, because a message is expected
        */
        if (mac_msg_ptr == NULL)
        {
          MSG_GERAN_LOW_0_G("GRLD NULL MAC msg null");
        }
        else
        {
          if (mac_msg_ptr->msg.sig == MR_DL_RESET_REQ)
          {
            grlc_dl_panic_reset();
          }
          else
          if (mac_msg_ptr->msg.sig == MR_DL_ASS_IND)
          {
#ifdef FEATURE_GPRS_PS_HANDOVER
            if (dl.b_psho_active)
            {
              MSG_GERAN_ERROR_0_G("GRLD DS_NULL ##MR_DL_ASS_IND not expected in PSHO");
            }
            else
#endif /* FEATURE_GPRS_PS_HANDOVER */
            {
              /*---------------------------------------------------
              ** downlink tbf established.
              ** Rlc to get ready to receive data blocks from L1.
              **---------------------------------------------------
              */

              /* re-start downlink activities
              */
              grlc_dl_fifo_clear();
              grlc_dl_init_vars();
#ifdef FEATURE_GPRS_PS_HANDOVER
              grlc_dl_clr_psho_vars();
#endif /* FEATURE_GPRS_PS_HANDOVER */

              /* store assignment info
              */
              dl.dl_ass = mac_msg_ptr->msg.dl_ass_ind;

              if (mac_msg_ptr->msg.dl_ass_ind.tbf_mode == TBF_MODE_EGPRS)
              {
                dl.tbf_mode = TBF_MODE_EGPRS;

                /* Store the DL Window Size */
                if ((mac_msg_ptr->msg.dl_ass_ind.ws  >= 64  ) &&
                    (mac_msg_ptr->msg.dl_ass_ind.ws  <= 1024))
                {
                  dl.ws = mac_msg_ptr->msg.dl_ass_ind.ws;
                }
                else
                {
                  MSG_GERAN_ERROR_1_G("ERLD NULL WS=%u in DL_ASS_IND", mac_msg_ptr->msg.dl_ass_ind.ws);
                }

                /* Allocate memory for rx_arr */
                grlc_dl_create_rx_array( E_RX_WORKSPACE);
              }
              else if (mac_msg_ptr->msg.dl_ass_ind.tbf_mode == TBF_MODE_GPRS)
              {
                dl.tbf_mode = TBF_MODE_GPRS;

                /* Allocate memory for rx_arr */
                grlc_dl_create_rx_array(  RM_RLC_RX_WORKSPACE );
              }
              else
              {
                MSG_GERAN_LOW_1_G("ERLD NULL PDA unxp TBF_MODE %u",
                          mac_msg_ptr->msg.dl_ass_ind.tbf_mode
                          );
              }

  #ifdef  GRLD_SHOW_DL_ASS
              MSG_GERAN_MED_3_G("GRLD NULL Rcvd DL_ASS mode=%d tlli=0x%x ws=%u",
                      dl.dl_ass.rlc_mode,
                      dl.dl_ass.tlli,
                      dl.ws);
  #endif
              /* init dl pdu with appropriate rlc mode
              */
              grlc_dl_init_llc_pdu( &cur_dl_pdu, dl.dl_ass.rlc_mode);

              /* ready to receive.
              ** Next state is rx ack or rx un-ack
              */
              dl.state = (dl_state_t)((dl.dl_ass.rlc_mode == GRLC_MODE_ACK) ?
                                      DS_RX_ACK :
                                      DS_RX_UNACK
                                     );
              UPD_DL_STATE();

              if (GRLC_MODE_ACK == dl.dl_ass.rlc_mode)
              {
                grlc_dl_unblock_tuneaway();
              }
              else
              {
                /* assume un-ack mode here */
                grlc_dl_block_tuneaway();
              }

              /* set flag to indicate RLC has not received the 1st data block and
              ** so has not sent to MAC the indication
              */
              dl.b_first_block_ind_sent = FALSE;

              /* Start at dl assignment to trap when the 1st data block is late
              */
              TIMER_START_T3190();

              /* stop T3192 in case it's on
              */
              grlc_dl_stop_t3192_dyn();
            }
          }
#ifdef FEATURE_GPRS_PS_HANDOVER
          else if (mac_msg_ptr->msg.sig == MR_DL_PSHO_START_IND)
          {
            if (dl.b_psho_active)
            {
              MSG_GERAN_ERROR_0_G("GRLD DS_NULL ##MR_DL_PSHO_START_IND not expected");
            }
            /* re-start downlink activities
            */
            grlc_dl_fifo_clear();
            grlc_dl_init_vars();
            grlc_dl_clr_psho_vars();

            dl.b_psho_active = TRUE;
            dl.psho_tbf_mode = mac_msg_ptr->msg.dl_psho_start_ind.tbf_mode;

            /* grlc_dl_fifo_put_block uses dl.tbf_mode */
            dl.tbf_mode = dl.psho_tbf_mode;
            MSG_GERAN_MED_1_G("GRLD DS_NULL PSHO MR_DL_PSHO_START_IND tbf_mode=%d",dl.psho_tbf_mode);
          }
          else if (mac_msg_ptr->msg.sig == MR_DL_PSHO_COMPLETE_IND)
          {
            MSG_GERAN_MED_0_G("GRLD DS_NULL PSHO MR_DL_PSHO_COMPLETE_IND");

            if(dl.b_psho_active)
            {
              grlc_handle_dl_psho_complete_ind_null_and_release_state( &mac_msg_ptr->msg.dl_psho_complete_ind);
            }
            else
            {
              MSG_GERAN_ERROR_0_G("GRLD DS_NULL PSHO ##Invalid MR_DL_PSHO_COMPLETE_IND as b_psho_active is FALSE");
            }
          }
#endif /* FEATURE_GPRS_PS_HANDOVER */
          else
          {
            MSG_GERAN_LOW_1_G("GRLD NULL Unexp MAC sig %d", mac_msg_ptr->msg.sig);
          }
        }

        /* don't expect any other signals.
        ** free command buffer
        */
        if (mac_msg_ptr)
          GPRS_MEM_FREE(mac_msg_ptr);
      }

      else

      /*================================
      ** State: DS_NULL, event: DE_PL1
      **================================
      */
      if (dl.event == DE_PL1)
      {
        /* get PL1 => RLC command
        */
        if ( (q_cnt(&DPQ())) != 0)
        {
          pl1_msg_ptr = (pl1_rlc_msg_t *)q_get(&DPQ());
          if( pl1_msg_ptr == NULL )
          {
            MSG_GERAN_LOW_0_G("GRLD NULL PL1 msg null");
          }
          else
          {
            /* Only DL EPDAN request message is sent in DL PL1 Queue at the moment.
            ** PL1 should not ask for EPDAN request while RLC is not in an active DL TBF.
            */
#ifndef FEATURE_GPRS_PS_HANDOVER
            if( pl1_msg_ptr->sig == PR_DL_EPDAN_REQ )
            {
              MSG_GERAN_LOW_0_G("GRLD NULL PL1 EPDAN_REQ without DL_ASS");
            }
#endif /* FEATURE_GPRS_PS_HANDOVER */
#ifdef FEATURE_GPRS_PS_HANDOVER
            if( pl1_msg_ptr->sig == PR_DL_EPDAN_REQ && dl.b_psho_active == FALSE )
            {
              MSG_GERAN_ERROR_0_G("GRLD NULL PL1 EPDAN_REQ without DL_ASS");
            }
            else if ( pl1_msg_ptr->sig == PR_DL_EPDAN_REQ && dl.b_psho_active == TRUE )
            {
              MSG_GERAN_MED_0_G("GRLD NULL PL1 EPDAN_REQ PSHO ignored");
            }
#endif /* FEATURE_GPRS_PS_HANDOVER */
            else
            {
              MSG_GERAN_LOW_0_G("GRLD NULL invalid PL1 msg");
            }

            if (q_cnt(&DPQ()) == 0)
            {
              RLC_DL_CLR_PL1_EPDAN_REQ_SIG();
            }

            /* free command buffer */
#if defined (DEBUG_GSM_EGPRS_DS_UNIT_TEST) || defined (PERLUTF)
            #error code not present
#else
            gprs_rlc_l1isr_free_buffer( pl1_msg_ptr);
#endif /* DEBUG_GSM_EGPRS_DS_UNIT_TEST / PERLUTF */

          }
        }

        while (grlc_dl_fifo_get_cnt() > 0)
        {
          /* We are getting PH_DATA_IND from PL1 which means data blocks are
          ** filling the FIFO whilst we have no Downlink Assignment. We could
          ** ignore this but if this persists the FIFO will be full. So we are
          ** discarding and freeing the data blocks in the FIFO as we received
          ** them
          */

#ifdef FEATURE_GPRS_PS_HANDOVER
          /* However in PSHO , MS can receive blind data blocks in DS_NULL.
          **
          */
          if ( dl.b_psho_active == TRUE )
          {
            /* process the dl data blocks performing blind data store as
            ** per the psho tbf_mode
            */
            grlc_dl_store_blind_data_blocks();
          }
          else
#endif /* FEATURE_GPRS_PS_HANDOVER */
          {
            MSG_GERAN_LOW_0_G("GRLD NULL PH_DATA_IND without DL_ASS");

            /* if there is a message there must be data blocks in
            ** the fifo. Point b_ptr to data block.
            */

            if( dl.tbf_mode == TBF_MODE_EGPRS )
            {
              if (e_grlc_dl_fifo_get_block( &e_pl1_dl_blk) == DL_FIFO_EMPTY)
              {
                MSG_GERAN_LOW_0_G("GRLD un-exp DL empty");
              }
              else
              {
                MSG_GERAN_LOW_0_G("GRLD R_U_P_R discard block");

                if( e_pl1_dl_blk.payload_1_ptr )
                  GPRS_PDU_FREE(&e_pl1_dl_blk.payload_1_ptr);

                if( e_pl1_dl_blk.payload_2_ptr )
                  GPRS_PDU_FREE(&e_pl1_dl_blk.payload_1_ptr);

              }
            }
            else  /* GPRS */
            {
              if (grlc_dl_fifo_get_block( &pl1_blk_hdr_ptr, &pl1_blk_dsm_ptr) ==
                  DL_FIFO_EMPTY
                 )
              {
                MSG_GERAN_LOW_0_G("GRLD un-exp DL empty");
              }
              else
              {
                MSG_GERAN_LOW_0_G("GRLD R_U_P_R discard block");

                GPRS_PDU_FREE(&pl1_blk_dsm_ptr);
              }
            }
          }
        } /* end while */
      }
      else
      /*================================
      ** State: DS_NULL, event: DE_TMR
      **================================
      */
      if (dl.event == DE_TMR)
      {
        timer_msg_ptr = (rm_timer_msg_t *)q_get(&DTQ());

        if (timer_msg_ptr == NULL)
        {
          MSG_GERAN_LOW_0_G("GRLD TIMER event without msg");
        }
        else
        {
          MSG_GERAN_LOW_1_G("GRLD NULL Unexp timer sig %d", timer_msg_ptr->sig);
          GPRS_MEM_FREE(timer_msg_ptr);
        }
      }
      /*=================================
      ** State: DS_NULL, Event: invalid
      **=================================
      */
      else
      {
        /* don't expect timer or L1 signals in DS_NULL state
        **
        ** ! unless to check if MS expects multiple dl assignment whilst
        ** waiting for the first data block or
        */
        MSG_GERAN_LOW_1_G("NULL Unexp event %d",dl.event);
      }
      break;


    /*=======================================================
    **  R E C E I V E   A C K N O W L E D G E    S T A T E
    **=======================================================
    */
    case DS_RX_ACK:
      /*=====================================================================
      ** State: DS_RX_ACK, Event: DE_PL1.
      ** Note the EPDAN_REQ is handled at this state during EGPRS dl data processing
      **======================================================================
      */
      if (dl.event == DE_PL1)
      {
#ifdef  GRLD_SHOW_PH_DATA_IND
        MSG_GERAN_LOW_0_G("GRLD RX_A Rcvd PH_DATA_IND");
#endif

#ifdef FEATURE_GPRS_PS_HANDOVER
        if ( dl.b_psho_active == TRUE )
        {
          /* process the dl data blocks performing blind data store as
          ** per the psho tbf_mode
          */
          grlc_dl_store_blind_data_blocks();

          /* if there a signal from Layer 1 for EGPRS EPDAN , it need
          ** to clear.
          */
          if( grlc_dl_task_sigs & GRLC_DL_PL1_EPDAN_REQ_SIG )
          {
            MSG_GERAN_MED_0_G("ERLD RX_A PSHO ##EPDAN_SIG ignored");

            epdan_req_ptr = (pl1_rlc_msg_t *)q_get(&DPQ());

            GRLC_DL_MSG_LOCK();

            if (q_cnt(&DPQ()) == 0)
            {
              RLC_DL_CLR_PL1_EPDAN_REQ_SIG();
            }

            GRLC_DL_MSG_UNLOCK();

#if defined (DEBUG_GSM_EGPRS_DS_UNIT_TEST) || defined (PERLUTF)
            #error code not present
#else
            gprs_rlc_l1isr_free_buffer( epdan_req_ptr);
#endif /* DEBUG_GSM_EGPRS_DS_UNIT_TEST / PERLUTF */
          }
        }
        else
#endif /* FEATURE_GPRS_PS_HANDOVER */
        /* Handle any PL1 data blocks in DL RLC queue */
        if( grlc_dl_fifo_get_cnt() > 0 )
        {

          if( dl.tbf_mode == TBF_MODE_EGPRS )
            e_ds_rx_ack_state_pl1_ph_data_ind_event_handler();
          else
            ds_rx_ack_state_pl1_ph_data_ind_event_handler();

#ifdef  ERLD_SHOW_DL_VARS
          show_dl_vars();
#endif /* ERLD_SHOW_DL_VARS */

        }
        else
        {
          if( grlc_dl_task_sigs & GRLC_DL_PL1_EPDAN_REQ_SIG )
          {
            e_ds_rx_ack_state_pl1_epdan_req_event_handler();
          }
          else
          {
            MSG_GERAN_LOW_0_G("GRLD Rcvd unexpected DE_PL1 event");
          }
        }
      } /* end DL_L1 event */

      else
      /*==================================
      ** State: DS_RX_ACK, Event: DE_MAC
      **==================================
      */
      if (dl.event == DE_MAC)
      {
        mac_msg_ptr = (mac_rlc_msg_t *)q_get(&DMQ());

        if (mac_msg_ptr == NULL)
        {
          MSG_GERAN_LOW_0_G("RX_ACK sig without msg");
        }
        else
        {
          if (mac_msg_ptr->msg.sig == MR_DL_RESET_REQ)
          {
            grlc_dl_panic_reset();
          }
          else
          if (mac_msg_ptr->msg.sig == MR_DL_RELEASE_IND)
          {
#ifdef  GRLD_SHOW_DL_REL
            MSG_GERAN_MED_0_G("GRLD R_A Rcvd DL_REL");
#endif
            rlc_log_dl_release_ind(  dl.tfi,RLC_LOG_DL_MAC_RELEASE_IND);

            /* Process DL FIFO before release */
            if( grlc_dl_fifo_get_cnt() > 0 )
            {
              MSG_GERAN_HIGH_0_G("GRLD DL ACK Rel, Proc DL FIFO - start");

              if( dl.tbf_mode == TBF_MODE_EGPRS )
                e_ds_rx_ack_state_pl1_ph_data_ind_event_handler();
              else
                ds_rx_ack_state_pl1_ph_data_ind_event_handler();

              MSG_GERAN_HIGH_0_G("GRLD DL ACK Rel, Proc DL FIFO - end");
#ifdef  ERLD_SHOW_DL_VARS
              show_dl_vars();
#endif /* ERLD_SHOW_DL_VARS */
            }
            grlc_dl_release();
          }
          else
          if (mac_msg_ptr->msg.sig == MR_DL_TIMER_IND)
          {
#ifdef  GRLD_SHOW_TIMER_IND
            MSG_GERAN_MED_0_G("GRLD RX_A Rcvd DL_TIMER_IND(MAC)");
#endif
            if (mac_msg_ptr->msg.dl_timer_ind.id == RM_TMR_T3190_AWAIT_VALID_DL_DATA &&
                mac_msg_ptr->msg.dl_timer_ind.state == TMR_START
               )
            {
              TIMER_START_T3190();
            }
          }
          /* DL Assingnment to indicate change of WS, is valid only in EGPRS */
          else if( mac_msg_ptr->msg.sig == MR_DL_ASS_IND )
          {
#ifdef FEATURE_GPRS_PS_HANDOVER
            if (dl.b_psho_active)
            {
              MSG_GERAN_ERROR_0_G("GRLD RX_A MR_DL_ASS_IND not expected in PSHO");
            }
            else
#endif /* FEATURE_GPRS_PS_HANDOVER */
            if( (mac_msg_ptr->msg.dl_ass_ind.tbf_mode == TBF_MODE_EGPRS) &&
                (dl.tbf_mode                          == TBF_MODE_EGPRS) &&
                (mac_msg_ptr->msg.dl_ass_ind.rlc_mode == GRLC_MODE_ACK)
              )
            {
              /* un-block tuneaway */
              grlc_dl_unblock_tuneaway();

              /* Assign new window size only if within range and not
              ** smaller than current window size
              */
              if ( (mac_msg_ptr->msg.dl_ass_ind.ws  >= 64    ) &&
                   (mac_msg_ptr->msg.dl_ass_ind.ws  <= 1024  ) &&
                   (mac_msg_ptr->msg.dl_ass_ind.ws  >= dl.ws )
                  )
              {
                dl.ws = mac_msg_ptr->msg.dl_ass_ind.ws;
              }
              else
              {
                MSG_GERAN_MED_3_G("ERLD RX_A WS=%u in DL_ASS_IND curr ws=%u not updated nts =%d",
                  mac_msg_ptr->msg.dl_ass_ind.ws, dl.ws,mac_msg_ptr->msg.dl_ass_ind.num_dl_ts);
              }
            }
            /* Ignore message if in GPRS mode */
            else if ( !((mac_msg_ptr->msg.dl_ass_ind.tbf_mode == TBF_MODE_GPRS) &&
                        (dl.tbf_mode                          == TBF_MODE_GPRS) &&
                        (mac_msg_ptr->msg.dl_ass_ind.rlc_mode == GRLC_MODE_ACK)   )
                    )
            {
              MSG_GERAN_LOW_3_G(
                "ERLD RX_A Rcvd invalid DL_ASS_IND tbf_m %d, curr_tbf_m %d, rlc_m %d",
                mac_msg_ptr->msg.dl_ass_ind.tbf_mode,
                dl.tbf_mode,
                mac_msg_ptr->msg.dl_ass_ind.rlc_mode);
            }

            /* Update dl nts assigned by NW */
            dl.dl_ass.num_dl_ts = mac_msg_ptr->msg.dl_ass_ind.num_dl_ts;

#ifdef  GRLD_SHOW_DL_ASS
            MSG_GERAN_MED_3_G("GRLD RX_A DL_ASS mode=%d tlli=0x%x ws=%u",
                    dl.dl_ass.rlc_mode,
                    dl.dl_ass.tlli,
                    dl.ws);
#endif
          }
#ifdef FEATURE_GPRS_PS_HANDOVER
          else if (mac_msg_ptr->msg.sig == MR_DL_PSHO_START_IND)
          {
            if (dl.b_psho_active)
            {
              MSG_GERAN_ERROR_0_G("GRLD RX_A ##MR_DL_PSHO_START_IND not expected");
            }

            dl.b_psho_active = TRUE;
            dl.psho_tbf_mode = mac_msg_ptr->msg.dl_psho_start_ind.tbf_mode;
            MSG_GERAN_MED_1_G("GRLD RX_A PSHO MR_DL_PSHO_START_IND tbf_mode=%d",dl.psho_tbf_mode);
          }
          else if (mac_msg_ptr->msg.sig == MR_DL_PSHO_COMPLETE_IND)
          {
            MSG_GERAN_MED_0_G("GRLD RX_A PSHO MR_DL_PSHO_COMPLETE_IND");

            if(dl.b_psho_active)
            {
              grlc_handle_dl_psho_complete_ind_xfer( &mac_msg_ptr->msg.dl_psho_complete_ind);
            }
            else
            {
              MSG_GERAN_ERROR_0_G("GRLD RX_A ##Invalid MR_DL_PSHO_COMPLETE_IND as b_psho_active is FALSE");
            }
          }
#endif /* FEATURE_GPRS_PS_HANDOVER */
          else if( mac_msg_ptr->msg.sig == MR_DL_TEST_MODE_IND )
          {
            if( mac_msg_ptr->msg.dl_test_mode_ind.test_mode == TEST_MODE_B )
            {
              /* Start of Test Mode B in DL */
              dl.test_mode = TEST_MODE_B;
              dl.state     = DS_TEST_MODE;
            }
            else if(  mac_msg_ptr->msg.dl_test_mode_ind.test_mode == TEST_MODE_SRB )
            {
              /* Start of SRB in DL */
              dl.test_mode = TEST_MODE_SRB;
              dl.state     = DS_TEST_MODE;
            }
            else
            {
              MSG_GERAN_LOW_1_G("ERLD RX_U Rcvd invalid Test Mode %d",
                mac_msg_ptr->msg.dl_test_mode_ind.test_mode);
            }
          }
          else
          {
            /* ! don't expect any other MAC signals here until after FBI has
            ** been received, and the state is pending release, by which
            ** time the n/w may send packet dl assignment
            */
            MSG_GERAN_LOW_1_G("GRLD RX_A Unexp MAC sig %d",mac_msg_ptr->msg.sig);
          }
        }

        if (mac_msg_ptr)
          GPRS_MEM_FREE(mac_msg_ptr);

      } /* end MAC event */

      else
      /*==================================
      ** State: DS_RX_ACK, Event: DE_TMR
      **==================================
      */
      if (dl.event == DE_TMR)
      {
        timer_msg_ptr = (rm_timer_msg_t *)q_get(&DTQ());

        /* shout if is NULL
        */
        if (timer_msg_ptr == NULL)
        {
          MSG_GERAN_LOW_0_G("GRLD null timer msg");
        }
        else
        {
          if (timer_msg_ptr->sig == RM_TMR_T3190_AWAIT_VALID_DL_DATA)
          {
            dl.timer_t3190_expiry_cnt++;
            /* ! do abnormal release without retry
            */
#ifdef  GRLD_SHOW_T3190
            MSG_GERAN_MED_1_G("GRLD RX_A Expy T3190 %u",dl.timer_t3190_expiry_cnt);
#endif

            rlc_msg_ptr = mac_get_cmd_buf_rlc();
            if (rlc_msg_ptr != NULL)
            {
              rlc_msg_ptr->msg.dl_release_ind.sig = RM_DL_RELEASE_IND;
              rlc_msg_ptr->msg.dl_release_ind.cause = REL_DL_ABNORMAL_WITHOUT_RETRY;
              mac_put_cmd_rlc( rlc_msg_ptr);
            }
            else
            {
              ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
            }

            rlc_log_dl_release_ind(  dl.tfi,
                RLC_LOG_DL_EXPIRED_T3190_IN_ACK_TRANSFER);

            /* release downlink */
            grlc_dl_release();
          }
#if 0
          /* Un-expected expiry of T3192 in Downlink Transfer mode (RX_ACK). This is
          ** due to PL1/MAC not stopping the timer when a new downlink TBF is
          ** established. RLC DL always stops this timer when starting a new TBF, but by
          ** the time RLC is in context the timer had already expired. The fix in PL1
          ** will be done in due course. To verify the PL1 fix, turn this message back
          ** on. In the meantime, RLC DL will ignore this timer expiry
          */
          else
          {
            MSG_GERAN_ERROR_1_G("RX_ACK Unexp timer sig %d",timer_msg_ptr->sig);
          }
#endif
        } /* valid timer */

        if (timer_msg_ptr)
          GPRS_MEM_FREE(timer_msg_ptr);

      } /* end TIMER event */
      break;


    /*=========================================================================
    **  R E C E I V E   A C K N O W L E D G E    P E N D I N G   R E L E A S E
    **=========================================================================
    */
    case DS_RX_ACK_PENDING_RELEASE:
      /* deal with dl ass and timeslot reconf msg from MAC whilst
      ** waiting for release. This is considered as early release
      ** for new establishment.
      */

      /*==================================================
      ** State: DS_RX_ACK_PENDING_RELEASE, Event: DE_MAC
      **==================================================
      */
      if (dl.event == DE_MAC)
      {
        /* may get MAC dl assigment
        */
        mac_msg_ptr = (mac_rlc_msg_t *)q_get(&DMQ());

        /* shout if is NULL, because a message is expected
        */
        if (mac_msg_ptr == NULL)
        {
          MSG_GERAN_LOW_0_G("GRLD null MAC msg");
        }
        else
        {
          if (mac_msg_ptr->msg.sig == MR_DL_RESET_REQ)
          {
            grlc_dl_panic_reset();
          }
          else
          /* this msg may result from MAC receiving either the Packet dl ass
          ** or packet timeslot reconfigure msgs from the n/w
          */
          if (mac_msg_ptr->msg.sig == MR_DL_RELEASE_IND)
          {
#ifdef  GRLD_SHOW_DL_REL
            MSG_GERAN_MED_0_G("GRLD RX_A_P_R Rcvd DL_REL");
#endif
            rlc_log_dl_release_ind(  dl.tfi,RLC_LOG_DL_MAC_RELEASE_IND);
            grlc_dl_release();
          }
          else
          if (mac_msg_ptr->msg.sig == MR_DL_ASS_IND)
          {
            /* make sure this is a assignment for NEW DL TBF */
            /* It could happen at reception of DL ASS , L1 did not TX
            ** EPDAN - thus T3192 is not running and MAC accepts it,
            ** but due to message sequence , if it receives DL_ASS_INd
            ** with ctrl_ack = 0 , it should ignore Ass
            */
            if ( mac_msg_ptr->msg.dl_ass_ind.ctrl_ack == 0 )
            {
              /* Stop T3190 as MAC would have started it */
              TIMER_STOP_T3190();

              MSG_GERAN_LOW_2_G("GRLD RX_A_P_R DL_ASS IGNORED as Releasing DL TBF mode=%d ctrl_ack=0x%x",
                        dl.dl_ass.rlc_mode,
                        dl.dl_ass.ctrl_ack
                        );

              if (TMR_INACTIVE == grlc_dl_t3192_status_dyn())
              {
                 MSG_GERAN_LOW_0_G("GRLD RX_A_P_R DL_ASS  -- T3192 NOT RUNNING");

                 /* May have been stopped by L1 when it get config from MAC - race condition*/
                 /* If we didnt restart then we would never release */
                 /* If wasnt running and we start it , L1 shortly will start T3192 again  after rrbp is sent */
                 TIMER_START_T3192();
              }
            }
            else  // Accept the Assignmment
            {
              /* Stop waiting for release of tbf
              */
              grlc_dl_stop_t3192_dyn();

              /*---------------------------------------------------
              ** previous tbf is considered terminated and new
              ** tbf is now established.
              **---------------------------------------------------
              */

              /* re-start downlink activities
              */
              grlc_dl_fifo_clear();
              grlc_dl_init_vars();
#ifdef FEATURE_GPRS_PS_HANDOVER
              grlc_dl_clr_psho_vars();
#endif /* FEATURE_GPRS_PS_HANDOVER */

              /* store assignment info
              */
              dl.dl_ass = mac_msg_ptr->msg.dl_ass_ind;

              /* init dl pdu with appropriate rlc mode
              */
              grlc_dl_init_llc_pdu( &cur_dl_pdu, mac_msg_ptr->msg.dl_ass_ind.rlc_mode);

              /* ready to receive.
              ** Next state is rx ack or rx un-ack
              */
              dl.state = (dl_state_t)((dl.dl_ass.rlc_mode == GRLC_MODE_ACK) ?
                                      DS_RX_ACK :
                                      DS_RX_UNACK
                                     );
              UPD_DL_STATE();

              /* Start on a new dl assignment
              */
              TIMER_START_T3190();

              if (mac_msg_ptr->msg.dl_ass_ind.tbf_mode == TBF_MODE_EGPRS)
              {
                dl.tbf_mode = TBF_MODE_EGPRS;

                /* Store the DL Window Size */
                if ((mac_msg_ptr->msg.dl_ass_ind.ws  >= 64  ) &&
                    (mac_msg_ptr->msg.dl_ass_ind.ws  <= 1024))
                {
                  dl.ws = mac_msg_ptr->msg.dl_ass_ind.ws;
                }
                else
                {
                  MSG_GERAN_LOW_1_G("ERLD NULL WS=%u in DL_ASS_IND", mac_msg_ptr->msg.dl_ass_ind.ws);
                }
              }
              else if (mac_msg_ptr->msg.dl_ass_ind.tbf_mode == TBF_MODE_GPRS)
              {
                dl.tbf_mode = TBF_MODE_GPRS;
              }
              else
              {
                MSG_GERAN_LOW_1_G( "ERLD NULL PDA unxp TBF_MODE %u",
                          mac_msg_ptr->msg.dl_ass_ind.tbf_mode
                          );
              }

              if (GRLC_MODE_ACK == dl.dl_ass.rlc_mode)
              {
                grlc_dl_unblock_tuneaway();
              }
              else
              {
                grlc_dl_block_tuneaway();
              }


              /* Allocate for rx_arr before start of  new DL TBF.
              ** Size of the rx_arr will be based on TBF MODE
              ** and not on the window sized assigned as window size can be
              ** increased during DL TBF and hence allocate maximum supported
              ** for that TBF MODE
              ** */
              if( TBF_MODE_EGPRS == dl.tbf_mode )
              {
                /* Allocate memory for rx_arr */
                grlc_dl_create_rx_array(  E_RX_WORKSPACE );
              }
              else /* TBF MODE GPRS */
              {
                grlc_dl_create_rx_array(  RM_RLC_RX_WORKSPACE );
              }

#ifdef  GRLD_SHOW_DL_ASS
              MSG_GERAN_MED_3_G("GRLD RX_A_P_R DL_ASS mode=%d tlli=0x%x ws=%u",
                      dl.dl_ass.rlc_mode,
                      dl.dl_ass.tlli,
                      dl.ws);
#endif
            }
          } /* end DL_ASS_IND */
#ifdef FEATURE_GPRS_PS_HANDOVER
          else if (mac_msg_ptr->msg.sig == MR_DL_PSHO_START_IND)
          {
            if (dl.b_psho_active)
            {
              MSG_GERAN_ERROR_0_G("GRLD REL_ACK ##DS_REL_ACK MR_DL_PSHO_START_IND not expected");
            }

            /* T3192 is not stopped
            */
            dl.b_psho_active = TRUE;
            dl.psho_tbf_mode = mac_msg_ptr->msg.dl_psho_start_ind.tbf_mode;

            MSG_GERAN_MED_1_G("GRLD REL_ACK PSHO MR_DL_PSHO_START_IND  tbf_mode=%d",dl.psho_tbf_mode);
          }
          else if (mac_msg_ptr->msg.sig == MR_DL_PSHO_COMPLETE_IND)
          {
            MSG_GERAN_MED_0_G("GRLD REL_ACK PSHO MR_DL_PSHO_COMPLETE_IND");

            if(dl.b_psho_active)
            {
              grlc_handle_dl_psho_complete_ind_null_and_release_state( &mac_msg_ptr->msg.dl_psho_complete_ind);
            }
            else
            {
              MSG_GERAN_ERROR_0_G("GRLD REL_ACK ##Invalid MR_DL_PSHO_COMPLETE_IND as b_psho_active is FALSE");
            }
          }
#endif /* FEATURE_GPRS_PS_HANDOVER */
        }

        /* don't expect any other signals.
        ** free command buffer
        */
        if (mac_msg_ptr)
          GPRS_MEM_FREE(mac_msg_ptr);

      }
      else
      /*==================================================
      ** State: DS_RX_ACK_PENDING_RELEASE, Event: DE_TMR
      **==================================================
      */
      if (dl.event == DE_TMR)
      {
        timer_msg_ptr = (rm_timer_msg_t *)q_get(&DTQ());

        /* shout if is NULL, because a message is expected
        */
        if (timer_msg_ptr == NULL)
        {
          MSG_GERAN_LOW_0_G("GRLD RX_A_P_REL null timer msg");
        }
        else
        {
          if (timer_msg_ptr->sig == RM_TMR_T3192_AWAIT_RELEASE_DL_TBF)
          {
            dl.timer_t3192_expiry_cnt++;
#ifdef  GRLD_SHOW_T3192
            MSG_GERAN_MED_1_G("GRLD RX_A_P_REL Expy T3192 %u",dl.timer_t3192_expiry_cnt);
#endif
            rlc_log_dl_release_ind(  dl.tfi,
                                    RLC_LOG_DL_EXPIRED_T3192_IN_ACK_TRANSFER);

            grlc_dl_release();

            /* inform MAC of normal release of DL TBF
            */
            rlc_msg_ptr = mac_get_cmd_buf_rlc();
            if (rlc_msg_ptr != NULL)
            {
              rlc_msg_ptr->msg.dl_release_ind.sig = RM_DL_RELEASE_IND;
              rlc_msg_ptr->msg.dl_release_ind.cause = REL_DL_NORMAL;
              mac_put_cmd_rlc( rlc_msg_ptr);
            }
            else
            {
              ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
            }
          } /* end T3192 service */

          else
          {
            /* illegal state
            */
            MSG_GERAN_LOW_1_G("GRLD RX_A Unexp timer sig %d",timer_msg_ptr->sig);
          }
        }

        if (timer_msg_ptr)
          GPRS_MEM_FREE(timer_msg_ptr);

      }
      else
      /*==================================================
      ** State: DS_RX_ACK_PENDING_RELEASE, Event: DE_PL1
      **==================================================
      */
      if (dl.event == DE_PL1)
      {
#ifdef FEATURE_GPRS_PS_HANDOVER
        if ( dl.b_psho_active == TRUE )
        {
          /* process the dl data blocks performing blind data store as
          ** per the psho tbf_mode
          */
          grlc_dl_store_blind_data_blocks();

          /* if there a signal from Layer 1 for EGPRS EPDAN , it need
          ** to clear.
          */
          if( grlc_dl_task_sigs & GRLC_DL_PL1_EPDAN_REQ_SIG )
          {
            MSG_GERAN_MED_0_G("ERLD REL_ACK PSHO ##EPDAN_SIG ignored");

            epdan_req_ptr = (pl1_rlc_msg_t *)q_get(&DPQ());

            GRLC_DL_MSG_LOCK();

            if (q_cnt(&DPQ()) == 0)
            {
              RLC_DL_CLR_PL1_EPDAN_REQ_SIG();
            }

            GRLC_DL_MSG_UNLOCK();

#if defined (DEBUG_GSM_EGPRS_DS_UNIT_TEST) || defined (PERLUTF)
            #error code not present
#else
            gprs_rlc_l1isr_free_buffer( epdan_req_ptr);
#endif /* DEBUG_GSM_EGPRS_DS_UNIT_TEST / PERLUTF */
          }
        }
        else
#endif /* FEATURE_GPRS_PS_HANDOVER */
        /* Handle any PL1 data blocks in DL RLC queue */
        if( grlc_dl_fifo_get_cnt() > 0 )
        {
          /*--------------------------------------------------------------------
          ** We have received all data blocks and sent DL acknack with FAI.
          ** However, if the network might have missed it in which case it will
          ** re-send data block(s) with the Polling bit set (SP). We must check
          ** for this Polling bit and re-send our DL acknack with FAI until such
          ** time as T3192 expires
          **--------------------------------------------------------------------
          */
          if( dl.tbf_mode == TBF_MODE_EGPRS )
            e_ds_rx_ack_pend_rel_state_pl1_ph_data_ind_event_handler();
          else
            ds_rx_ack_pend_rel_state_pl1_ph_data_ind_event_handler();

#ifdef  ERLD_SHOW_DL_VARS
          show_dl_vars();
#endif /* ERLD_SHOW_DL_VARS */

        }
        else
        {
          if( grlc_dl_task_sigs & GRLC_DL_PL1_EPDAN_REQ_SIG )
          {
            e_ds_rx_ack_state_pl1_epdan_req_event_handler();
          }
          else
          {
            MSG_GERAN_LOW_0_G("GRLD Rcvd unexpected DE_PL1 event");
          }
        }
      }
      else
      /*===================================================
      ** State: DS_RX_ACK_PENDING_RELEASE, Event: invalid
      **===================================================
      */
      {
        /* don't expect L1 signals at DS_RX_PENDING_RELEASE state
        */
        MSG_GERAN_LOW_1_G("GRLD RX_A_P_REL Unexp event %d",dl.event);
      }
      break;

    /*===========================================================
    **  R E C E I V E   U N A C K N O W L E D G E    S T A T E
    **===========================================================
    */
    case DS_RX_UNACK:
      /*===================================
      ** State: DS_RX_UNACK, Event: DE_PL1
      **===================================
      */
      if (dl.event == DE_PL1)
      {
#ifdef  GRLD_SHOW_PH_DATA_IND
        MSG_GERAN_MED_0_G("GRLD RX_U Rcvd PH_DATA_IND");
#endif
#ifdef FEATURE_GPRS_PS_HANDOVER
        if ( dl.b_psho_active == TRUE )
        {
          /* process the dl data blocks performing blind data store as
          ** per the psho tbf_mode
          */
          grlc_dl_store_blind_data_blocks();

          /* if there a signal from Layer 1 for EGPRS EPDAN , it need
          ** to clear.
          */
          if( grlc_dl_task_sigs & GRLC_DL_PL1_EPDAN_REQ_SIG )
          {
            MSG_GERAN_MED_0_G("ERLD RX_U PSHO ##EPDAN_SIG ignored by RLC");

            epdan_req_ptr = (pl1_rlc_msg_t *)q_get(&DPQ());

            GRLC_DL_MSG_LOCK();

            if (q_cnt(&DPQ()) == 0)
            {
              RLC_DL_CLR_PL1_EPDAN_REQ_SIG();
            }

            GRLC_DL_MSG_UNLOCK();

#if defined (DEBUG_GSM_EGPRS_DS_UNIT_TEST) || defined (PERLUTF)
            #error code not present
#else
            gprs_rlc_l1isr_free_buffer( epdan_req_ptr);
#endif /* DEBUG_GSM_EGPRS_DS_UNIT_TEST / PERLUTF */
          }
        }
        else
#endif /* FEATURE_GPRS_PS_HANDOVER */
        /* Handle any PL1 data blocks in DL RLC queue */
        if( grlc_dl_fifo_get_cnt() > 0 )
        {
          if( dl.tbf_mode == TBF_MODE_EGPRS )
          {
            e_ds_rx_unack_state_pl1_ph_data_ind_event_handler();
          }
          else
          {
            ds_rx_unack_state_pl1_ph_data_ind_event_handler();
          }

#ifdef  ERLD_SHOW_DL_VARS
          show_dl_vars();
#endif /* ERLD_SHOW_DL_VARS */
        }
        else
        {
          if( grlc_dl_task_sigs & GRLC_DL_PL1_EPDAN_REQ_SIG )
          {
            e_ds_rx_unack_state_pl1_epdan_req_event_handler();
          }
          else
          {
            MSG_GERAN_LOW_0_G("GRLD U Rcvd unexpected DE_PL1 event");
          }
        }
      } /* end DL_L1 event */
      else
      /*====================================
      ** State: DS_RX_UNACK, Event: DE_MAC
      **====================================
      */
      if (dl.event == DE_MAC)
      {
        mac_msg_ptr = (mac_rlc_msg_t *)q_get(&DMQ());

        if (mac_msg_ptr == NULL)
        {
          MSG_GERAN_LOW_0_G("RX_UNACK sig without msg");
        }
        else
        {
          if (mac_msg_ptr->msg.sig == MR_DL_RESET_REQ)
          {
            grlc_dl_panic_reset();
          }
          else
          if (mac_msg_ptr->msg.sig == MR_DL_RELEASE_IND)
          {
#ifdef  GRLD_SHOW_DL_REL
              MSG_GERAN_MED_0_G("GRLD RX_U Rcvd DL_REL");
#endif
            rlc_log_dl_release_ind(  dl.tfi,RLC_LOG_DL_MAC_RELEASE_IND);

            /* Process DL FIFO before release */
            if( grlc_dl_fifo_get_cnt() > 0 )
            {
              MSG_GERAN_HIGH_0_G("GRLD DL UNACK Rel, Proc DL FIFO - start");

              if( dl.tbf_mode == TBF_MODE_EGPRS )
                e_ds_rx_unack_state_pl1_ph_data_ind_event_handler();
              else
                ds_rx_unack_state_pl1_ph_data_ind_event_handler();

              MSG_GERAN_HIGH_0_G("GRLD DL UNACK Rel, Proc DL FIFO - end");
#ifdef  ERLD_SHOW_DL_VARS
              show_dl_vars();
#endif /* ERLD_SHOW_DL_VARS */
            }
            grlc_dl_release();
          }
          /* DL Assingnment to indicate change of WS, is valid only in EGPRS */
          else if( mac_msg_ptr->msg.sig == MR_DL_ASS_IND )
          {
#ifdef FEATURE_GPRS_PS_HANDOVER
            if (dl.b_psho_active)
            {
              MSG_GERAN_ERROR_0_G("GRLD RX_U ##MR_DL_ASS_IND not expected in PSHO");
            }
            else
#endif /* FEATURE_GPRS_PS_HANDOVER */
            if( (mac_msg_ptr->msg.dl_ass_ind.tbf_mode == TBF_MODE_EGPRS)  &&
                (dl.tbf_mode                          == TBF_MODE_EGPRS)  &&
                (mac_msg_ptr->msg.dl_ass_ind.rlc_mode == GRLC_MODE_UNACK)
              )
            {
              grlc_dl_block_tuneaway();

              /* Assign new window size only if within range and not
              ** smaller than current window size
              */
              if ( (mac_msg_ptr->msg.dl_ass_ind.ws >= 64   ) &&
                   (mac_msg_ptr->msg.dl_ass_ind.ws <= 1024 ) &&
                   (mac_msg_ptr->msg.dl_ass_ind.ws >= dl.ws)
                 )
              {
                /* Store the DL Window Size */
                dl.ws = mac_msg_ptr->msg.dl_ass_ind.ws;
              }
              else
              {
                MSG_GERAN_MED_3_G("ERLD RX_U WS=%u in DL_ASS_IND curr ws=%u not updated nts=%d",
                  mac_msg_ptr->msg.dl_ass_ind.ws, dl.ws,mac_msg_ptr->msg.dl_ass_ind.num_dl_ts);
              }
            }
            else if ( !((mac_msg_ptr->msg.dl_ass_ind.tbf_mode == TBF_MODE_GPRS)  &&
                        (dl.tbf_mode                          == TBF_MODE_GPRS)  &&
                        (mac_msg_ptr->msg.dl_ass_ind.rlc_mode == GRLC_MODE_UNACK)  )
                    )
            {
               MSG_GERAN_LOW_3_G(
                 "ERLD RX_A Rcvd invalid DL_ASS_IND tbf_m %d, curr_tbf_m %d, rlc_m %d",
                 mac_msg_ptr->msg.dl_ass_ind.tbf_mode,
                 dl.tbf_mode,
                 mac_msg_ptr->msg.dl_ass_ind.rlc_mode);
            }

            /* Update dl nts assigned by NW */
            dl.dl_ass.num_dl_ts = mac_msg_ptr->msg.dl_ass_ind.num_dl_ts;
          }
#ifdef FEATURE_GPRS_PS_HANDOVER
          else if (mac_msg_ptr->msg.sig == MR_DL_PSHO_START_IND)
          {
            if (dl.b_psho_active)
            {
              MSG_GERAN_ERROR_0_G("GRLD RX_U ##MR_DL_PSHO_START_IND not expected");
            }

            dl.b_psho_active = TRUE;
            dl.psho_tbf_mode = mac_msg_ptr->msg.dl_psho_start_ind.tbf_mode;
            MSG_GERAN_MED_1_G("GRLD RX_U PSHO MR_DL_PSHO_START_IND tbf_mode=%d",dl.psho_tbf_mode);
          }
          else if (mac_msg_ptr->msg.sig == MR_DL_PSHO_COMPLETE_IND)
          {
            MSG_GERAN_MED_0_G("GRLD RX_U PSHO MR_DL_PSHO_COMPLETE_IND");

            if(dl.b_psho_active)
            {
              grlc_handle_dl_psho_complete_ind_xfer( &mac_msg_ptr->msg.dl_psho_complete_ind);
            }
            else
            {
              MSG_GERAN_ERROR_0_G("GRLD RX_U ##Invalid MR_DL_PSHO_COMPLETE_IND as b_psho_active is FALSE");
            }
          }
#endif /* FEATURE_GPRS_PS_HANDOVER */
          else if( mac_msg_ptr->msg.sig == MR_DL_TEST_MODE_IND )
          {
            if( mac_msg_ptr->msg.dl_test_mode_ind.test_mode == TEST_MODE_B )
            {
              /* Start of Test Mode B in DL */
              dl.test_mode = TEST_MODE_B;
              dl.state     = DS_TEST_MODE;
            }
            else if(  mac_msg_ptr->msg.dl_test_mode_ind.test_mode == TEST_MODE_SRB )
            {
              /* Start of SRB in DL */
              dl.test_mode = TEST_MODE_SRB;
              dl.state     = DS_TEST_MODE;
            }
            else
            {
              MSG_GERAN_LOW_1_G("ERLD RX_U Rcvd invalid Test Mode %d",
                mac_msg_ptr->msg.dl_test_mode_ind.test_mode);
            }
          }
          else
          {
            MSG_GERAN_LOW_1_G("GRLD RX_U Unexp MAC sig %d",mac_msg_ptr->msg.sig);
          }
        }

        if (mac_msg_ptr)
        {
          GPRS_MEM_FREE(mac_msg_ptr);
        }
      }
      else
      /*====================================
      ** State: DS_RX_UNACK, Event: DE_TMR
      **====================================
      */
      if (dl.event == DE_TMR)
      {
        timer_msg_ptr = (rm_timer_msg_t *)q_get(&DTQ());

        /* shout if is NULL, because a message is expected
        */
        if (timer_msg_ptr == NULL)
        {
          MSG_GERAN_LOW_0_G("GRLD RX_U null timer msg");
        }
        else
        {
          if (timer_msg_ptr->sig == RM_TMR_T3190_AWAIT_VALID_DL_DATA)
          {
            dl.timer_t3190_expiry_cnt++;
#ifdef  GRLD_SHOW_T3190
            MSG_GERAN_MED_1_G("GRLD RX_U Expy T3190 %u",dl.timer_t3190_expiry_cnt);
#endif

            /* do abnormal release
            */
            rlc_msg_ptr = mac_get_cmd_buf_rlc();
            if (rlc_msg_ptr != NULL)
            {
              rlc_msg_ptr->msg.dl_release_ind.sig = RM_DL_RELEASE_IND;
              rlc_msg_ptr->msg.dl_release_ind.cause = REL_DL_ABNORMAL_WITHOUT_RETRY;
              mac_put_cmd_rlc( rlc_msg_ptr);
            }
            else
            {
              ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
            }

            rlc_log_dl_release_ind(  dl.tfi,
                                    RLC_LOG_DL_EXPIRED_T3190_IN_UNACK_TRANSFER);

            grlc_dl_release();
          } /* T3190_AWAIT_VALID_DL_DATA */
        } /* tmr_msg_ptr valid */

        if (timer_msg_ptr)
          GPRS_MEM_FREE(timer_msg_ptr);
      }
      else          /* DS_RX_UNACK, Event: ? */
      {
        MSG_GERAN_LOW_1_G("GRLD RX_U Unexp event %d",dl.event);
      }
      break;


    /*========================================================================
    ** R E C E I V E   U N A C K    P E N D I N G   R E L E A S E   S T A T E
    **========================================================================
    */
    case DS_RX_UNACK_PENDING_RELEASE:
      /* deal with dl ass and timeslot reconf msg from MAC whilst
      ** waiting for release. This is considered as early release
      ** for new establishment.
      */
      /*====================================================
      ** State: DS_RX_UNACK_PENDING_RELEASE, Event: DE_MAC
      **====================================================
      */
      if (dl.event == DE_MAC)
      {
        /* may get MAC dl assigment
        */
        mac_msg_ptr = (mac_rlc_msg_t *)q_get(&DMQ());

        /* shout if is NULL, because a message is expected
        */
        if (mac_msg_ptr == NULL)
        {
          MSG_GERAN_LOW_0_G("GRLD null MAC msg pointer");
        }
        else
        {
          if (mac_msg_ptr->msg.sig == MR_DL_RESET_REQ)
          {
            grlc_dl_panic_reset();
          }
          else
          if (mac_msg_ptr->msg.sig == MR_DL_RELEASE_IND)
          {
#ifdef  GRLD_SHOW_DL_REL
            MSG_GERAN_MED_0_G("GRLD RX_U_P_R Rcvd DL_REL");
#endif
            rlc_log_dl_release_ind(  dl.tfi,RLC_LOG_DL_MAC_RELEASE_IND);
            grlc_dl_release();
          }
          else
          /* this msg may result from MAC receiving either the Packet dl ass
          ** or packet timeslot reconfigure msgs from the n/w
          */
          if (mac_msg_ptr->msg.sig == MR_DL_ASS_IND)
          {
            /* make sure this is a assignment for NEW DL TBF */
            /* It could happen at reception of DL ASS , L1 did not TX
            ** EPDAN - thus T3192 is not running and MAC accepts it,
            ** but due to message sequence , if it receives DL_ASS_INd
            ** with ctrl_ack = 0 , it should ignore Ass
            */
            if ( mac_msg_ptr->msg.dl_ass_ind.ctrl_ack == 0 )
            {
              /* Stop T3190 as MAC would have started it */
              TIMER_STOP_T3190();

              MSG_GERAN_LOW_2_G("GRLD RX_U_P_REL DL_ASS IGNORED as Releasing DL TBF mode=%d ctrl_ack=0x%x",
                        dl.dl_ass.rlc_mode,
                        dl.dl_ass.ctrl_ack
                        );

              if (TMR_INACTIVE == grlc_dl_t3192_status_dyn())
              {
                 MSG_GERAN_LOW_0_G("GRLD RX_U_P_REL DL_ASS  -- T3192 NOT RUNNING");

                 /* May have been stopped by L1 when it get config from MAC */
                 /* If we didnt then we would never release */
                 /* If wasnt running and we start it , L1 shortly will start T3192 again  after rrbp is sent */
              }
            }
            else  // Accept the Assignmment
            {
              /* Stop waiting for release of tbf
              */
              grlc_dl_stop_t3192_dyn();

              /*---------------------------------------------------
              ** previous tbf is considered terminated and new
              ** tbf is now established.
              **---------------------------------------------------
              */

              /* re-start downlink activities
              */
              grlc_dl_fifo_clear();
              grlc_dl_init_vars();
  #ifdef FEATURE_GPRS_PS_HANDOVER
              grlc_dl_clr_psho_vars();
  #endif /* FEATURE_GPRS_PS_HANDOVER */
              /* store assignment info
              */
              dl.dl_ass = mac_msg_ptr->msg.dl_ass_ind;

#ifdef  GRLD_SHOW_DL_ASS
            MSG_GERAN_MED_3_G("GRLD RX_U_P_REL DL_ASS mode=%d tlli=0x%x nts=%d",
                    dl.dl_ass.rlc_mode,
                    dl.dl_ass.tlli,
                    dl.dl_ass.num_dl_ts);
#endif

              /* init dl pdu with appropriate rlc mode
              */
              grlc_dl_init_llc_pdu( &cur_dl_pdu, dl.dl_ass.rlc_mode);

              /* ready to receive.
              ** Next state is rx ack or rx un-ack
              */
              dl.state = (dl_state_t)((dl.dl_ass.rlc_mode == GRLC_MODE_ACK) ?
                                      DS_RX_ACK :
                                      DS_RX_UNACK
                                     );
              UPD_DL_STATE();


              if (GRLC_MODE_ACK == dl.dl_ass.rlc_mode)
              {
                grlc_dl_unblock_tuneaway();
              }
              else
              {
                grlc_dl_block_tuneaway();
              }

              /* Start on a new dl assignment
              */
              TIMER_START_T3190();

              if (mac_msg_ptr->msg.dl_ass_ind.tbf_mode == TBF_MODE_EGPRS)
              {
                dl.tbf_mode = TBF_MODE_EGPRS;

                /* Store the DL Window Size */
                if ((mac_msg_ptr->msg.dl_ass_ind.ws  >= 64  ) &&
                    (mac_msg_ptr->msg.dl_ass_ind.ws  <= 1024))
                {
                  dl.ws = mac_msg_ptr->msg.dl_ass_ind.ws;
                }
                else
                {
                  MSG_GERAN_LOW_1_G("ERLD NULL WS=%u in DL_ASS_IND", mac_msg_ptr->msg.dl_ass_ind.ws);
                }

                /* DL assignment in pending release state
                ** Allocate memory for rx_arr
                */
                grlc_dl_create_rx_array(  E_RX_WORKSPACE );
              }
              else if (mac_msg_ptr->msg.dl_ass_ind.tbf_mode == TBF_MODE_GPRS)
              {
                dl.tbf_mode = TBF_MODE_GPRS;

                /* DL assignment in pending release state
                ** Allocate memory for rx_arr
                */
                grlc_dl_create_rx_array(  RM_RLC_RX_WORKSPACE );
              }
              else
              {
                MSG_GERAN_LOW_1_G("ERLD NULL PDA unxp TBF_MODE %u",
                            mac_msg_ptr->msg.dl_ass_ind.tbf_mode
                            );
              }
            }
          }
#ifdef FEATURE_GPRS_PS_HANDOVER
          else
          if (mac_msg_ptr->msg.sig == MR_DL_PSHO_START_IND)
          {
            if (dl.b_psho_active)
            {
              MSG_GERAN_ERROR_0_G("GRLD REL_UNACK PSHO ##MR_DL_PSHO_START_IND not expected");
            }

            /* T3192 is not stopped.
            */
            dl.b_psho_active = TRUE;
            dl.psho_tbf_mode = mac_msg_ptr->msg.dl_psho_start_ind.tbf_mode;

            MSG_GERAN_MED_1_G("GRLD REL_UNACK PSHO MR_DL_PSHO_START_IND tbf_mode=%d",dl.psho_tbf_mode);
          }
          else if (mac_msg_ptr->msg.sig == MR_DL_PSHO_COMPLETE_IND)
          {
            MSG_GERAN_MED_0_G("GRLD REL_UNACK PSHO MR_DL_PSHO_COMPLETE_IND");

            if(dl.b_psho_active)
            {
              grlc_handle_dl_psho_complete_ind_null_and_release_state( &mac_msg_ptr->msg.dl_psho_complete_ind);
            }
            else
            {
              MSG_GERAN_ERROR_0_G("GRLD REL_UNACK PSHO ##Invalid MR_DL_PSHO_COMPLETE_IND as b_psho_active is FALSE");
            }
          }
#endif /* FEATURE_GPRS_PS_HANDOVER */
        }

        /* don't expect any other signals.
        ** free command buffer
        */
        if (mac_msg_ptr)
          GPRS_MEM_FREE(mac_msg_ptr);
      }

      else
      /*====================================================
      ** State: DS_RX_UNACK_PENDING_RELEASE, Event: DE_TMR
      **====================================================
      */
      if (dl.event == DE_TMR)
      {
        timer_msg_ptr = (rm_timer_msg_t *)q_get(&DTQ());

        /* shout if is NULL, because a message is expected
        */
        if (timer_msg_ptr == NULL)
        {
          MSG_GERAN_LOW_0_G("GRLD null timer msg pointer");
        }
        else
        {
          if (timer_msg_ptr->sig == RM_TMR_T3192_AWAIT_RELEASE_DL_TBF)
          {
            dl.timer_t3192_expiry_cnt++;
#ifdef  GRLD_SHOW_T3192
            MSG_GERAN_MED_1_G("GRLD RX_U_P_REL Expy T3192 %u",dl.timer_t3192_expiry_cnt);
#endif
            rlc_log_dl_release_ind(  dl.tfi,
                                    RLC_LOG_DL_EXPIRED_T3192_IN_UNACK_TRANSFER);

            grlc_dl_release();

            /* inform MAC of release and cause
            */
            rlc_msg_ptr = mac_get_cmd_buf_rlc();
            if (rlc_msg_ptr != NULL)
            {
              rlc_msg_ptr->msg.dl_release_ind.sig = RM_DL_RELEASE_IND;
              rlc_msg_ptr->msg.dl_release_ind.cause = REL_DL_NORMAL;
              mac_put_cmd_rlc( rlc_msg_ptr);
            }
            else
            {
              ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
            }
          }
          else
          {
            /* illegal state
            */
            MSG_GERAN_LOW_1_G("GRLD RX_U_P_REL Unexp timer sig %d",timer_msg_ptr->sig);
          }
        }

        if (timer_msg_ptr)
          GPRS_MEM_FREE(timer_msg_ptr);
      }

      else if (dl.event == DE_PL1)
      /*===================================================
      ** State: DS_RX_UNACK_PENDING_RELEASE, Event: DE_PL1
      **===================================================
      */
      {
        /*MSG_LOW("GRLD RX_U_P_REL Rcvd PH_DATA_IND",0,0,0);*/
#ifdef FEATURE_GPRS_PS_HANDOVER
        if ( dl.b_psho_active == TRUE )
        {
          /* process the dl data blocks performing blind data store as
          ** per the psho tbf_mode
          */
          grlc_dl_store_blind_data_blocks();

          /* if there a signal from Layer 1 for EGPRS EPDAN , it need
          ** to clear.
          */
          if( grlc_dl_task_sigs & GRLC_DL_PL1_EPDAN_REQ_SIG )
          {
            MSG_GERAN_LOW_0_G("ERLD REL_UNACK PSHO ##EPDAN_SIG ignored by RLC");

            epdan_req_ptr = (pl1_rlc_msg_t *)q_get(&DPQ());

            GRLC_DL_MSG_LOCK();

            if (q_cnt(&DPQ()) == 0)
            {
              RLC_DL_CLR_PL1_EPDAN_REQ_SIG();
            }

            GRLC_DL_MSG_UNLOCK();

#if defined (DEBUG_GSM_EGPRS_DS_UNIT_TEST) || defined (PERLUTF)
            #error code not present
#else
            gprs_rlc_l1isr_free_buffer( epdan_req_ptr);
#endif /* DEBUG_GSM_EGPRS_DS_UNIT_TEST / PERLUTF */
          }
        }
        else
#endif /* FEATURE_GPRS_PS_HANDOVER */
        /* Handle any PL1 data blocks in DL RLC queue */
        if( grlc_dl_fifo_get_cnt() > 0 )
        {
          /*--------------------------------------------------------------------
          ** We have received all data blocks and sent DL acknack with FAI.
          ** However, if the network might have missed it in which case it will
          ** re-send data block(s) with the Polling bit set (SP). We must check
          ** for this Polling bit and re-send our DL acknack with FAI until such
          ** time as T3192 expires
          **--------------------------------------------------------------------
          */
          if( dl.tbf_mode == TBF_MODE_EGPRS )
          {
            e_ds_rx_unack_pend_rel_state_pl1_ph_data_ind_event_handler();
          }
          else
          {
            ds_rx_unack_pend_rel_state_pl1_ph_data_ind_event_handler();
          }

#ifdef  ERLD_SHOW_DL_VARS
          show_dl_vars();
#endif /* ERLD_SHOW_DL_VARS */

        }
        else
        {
          if( grlc_dl_task_sigs & GRLC_DL_PL1_EPDAN_REQ_SIG )
          {
            e_ds_rx_unack_state_pl1_epdan_req_event_handler();
          }
          else
          {
            MSG_GERAN_LOW_0_G("ERLD U PEND_REL rcvd unexpected DE_PL1 event");
          }
        }
      } /* PL1 event */

      else
      /*===================================================
      ** State: DS_RX_UNACK_PENDING_RELEASE, Event: ?
      **===================================================
      */
      {
        /* don't expect L1 signals at DS_RX_PENDING_RELEASE state
        */
        MSG_GERAN_LOW_1_G("GRLD RX_U_P_REL Unexp event %d",dl.event);
      }
      break;

    /*===========================================================
    **  T E S T   M O D E    S T A T E
    **===========================================================
    */
    case DS_TEST_MODE:
      /*===================================
      ** State: DS_TEST_MODE, Event: DE_PL1
      **===================================
      */
      if (dl.event == DE_PL1)
      {
#ifdef  GRLD_SHOW_PH_DATA_IND
        MSG_GERAN_MED_0_G("GRLD RX_U Rcvd PH_DATA_IND in DS_TEST_MODE");
#endif
        /* Handle any PL1 data blocks in DL RLC queue */
        if( grlc_dl_fifo_get_cnt() > 0 )
        {
          if( dl.tbf_mode == TBF_MODE_EGPRS )
          {
            if( dl.dl_ass.rlc_mode == GRLC_MODE_ACK )
            {

              /* Read DL blocks from FIFO. Function handles Test Mode B
              ** and SRB in DL ACK mode
              */
              e_ds_rx_ack_test_mode_state_pl1_ph_data_ind_event_handler();
            }
            else
            {
              /* Read DL blocks from FIFO. Function handles both Test Mode B and
              ** SRB in RLC UNACK Mode
              */
              e_ds_rx_unack_test_mode_state_pl1_ph_data_ind_event_handler();
            }
          }
          else
          {
            if( dl.test_mode == TEST_MODE_B )
            {
              if( dl.dl_ass.rlc_mode == GRLC_MODE_ACK )
              {
                ds_rx_ack_test_mode_b_state_pl1_ph_data_ind_event_handler();
              }
              else
              {
                ds_rx_unack_test_mode_b_state_pl1_ph_data_ind_event_handler();
              }
            }
            else
            {
              /* SRB is not supported in GPRS */
              MSG_GERAN_LOW_0_G("GRLD Invalid GPRS TEST MODE Rcvd PH_DATA_IND in SRB");
            }
          }
        }
        else
        {
          if( grlc_dl_task_sigs & GRLC_DL_PL1_EPDAN_REQ_SIG )
          {
            if( dl.dl_ass.rlc_mode == GRLC_MODE_ACK )
            {
              e_ds_rx_ack_state_pl1_epdan_req_event_handler();
            }
            else
            {
              e_ds_rx_unack_state_pl1_epdan_req_event_handler();
            }
          }
          else
          {
            MSG_GERAN_LOW_0_G("GRLD Rcvd unexpected DE_PL1 event");
          }
        }
      } /*  dl.event == DE_PL1 */

      else
      /*====================================
      ** State: DS_TEST_MODE, Event: DE_MAC
      **====================================
      */
      if (dl.event == DE_MAC)
      {
        mac_msg_ptr = (mac_rlc_msg_t *)q_get(&DMQ());

        if( mac_msg_ptr )
        {
          if (mac_msg_ptr->msg.sig == MR_DL_RESET_REQ)
          {
            grlc_dl_panic_reset();
          }
          else
          if (mac_msg_ptr->msg.sig == MR_DL_RELEASE_IND)
          {
#ifdef  GRLD_SHOW_DL_REL
            MSG_GERAN_MED_0_G("GRLD TestMode Rcvd DL_REL");
#endif
            rlc_log_dl_release_ind(  dl.tfi,RLC_LOG_DL_MAC_RELEASE_IND);
            grlc_dl_release();
          }
          else if (mac_msg_ptr->msg.sig == MR_DL_ASS_IND)
          {
            /* Update dl nts assigned by NW */
            dl.dl_ass.num_dl_ts = mac_msg_ptr->msg.dl_ass_ind.num_dl_ts;

            MSG_GERAN_LOW_1_G("GRLD TestMode Rcvd MR_DL_ASS_IND nts %d- No Action",dl.dl_ass.num_dl_ts);
          }
          else
          {
            MSG_GERAN_LOW_1_G("GRLD DS_TEST_MODE RX_U Unexp MAC sig %d",mac_msg_ptr->msg.sig);
          }

          GPRS_MEM_FREE(mac_msg_ptr);
        }
        else
        {
          MSG_GERAN_LOW_1_G("GRLD DS_TEST_MODE Invalid MAC sig ptr %p",mac_msg_ptr);
        }
      }/*  dl.event == DE_MAC */
      else
      /*====================================
      ** State: DS_TEST_MODE, Event: DE_TMR
      **====================================
      */
      if (dl.event == DE_TMR)
      {
        timer_msg_ptr = (rm_timer_msg_t *)q_get(&DTQ());

        /* shout if is NULL, because a message is expected
        */
        if (timer_msg_ptr == NULL)
        {
          MSG_GERAN_LOW_1_G("GRLD TestMode null timer msg %p",timer_msg_ptr);
        }
        else
        {
          if( dl.test_mode == TEST_MODE_SRB  )
          {
            /* T3190 is active in SRB mode */
            if (timer_msg_ptr->sig == RM_TMR_T3190_AWAIT_VALID_DL_DATA)
            {
              dl.timer_t3190_expiry_cnt++;
#ifdef  GRLD_SHOW_T3190
              MSG_GERAN_MED_1_G("GRLD TestMode Expy T3190 %u",dl.timer_t3190_expiry_cnt);
#endif

              /* do abnormal release
              */
              rlc_msg_ptr = mac_get_cmd_buf_rlc();
              if (rlc_msg_ptr != NULL)
              {
                rlc_msg_ptr->msg.dl_release_ind.sig = RM_DL_RELEASE_IND;
                rlc_msg_ptr->msg.dl_release_ind.cause = REL_DL_ABNORMAL_WITHOUT_RETRY;
                mac_put_cmd_rlc( rlc_msg_ptr);
              }
              else
              {
                ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
              }

              rlc_log_dl_release_ind(  dl.tfi,
                                      RLC_LOG_DL_EXPIRED_T3190_IN_UNACK_TRANSFER);

              grlc_dl_release();
            }
          }
          else
          {
            /* No timers are running in Test Mode B */
            MSG_GERAN_LOW_1_G("GRLD TestMode B RX_U Invalid timer sig %d",
                      timer_msg_ptr->sig);
          }
        }

        if (timer_msg_ptr)
          GPRS_MEM_FREE(timer_msg_ptr);

      } /* dl.event == DE_TMR */
      else
      {
        MSG_GERAN_LOW_1_G("GRLD DS_TEST_MODE Unexp event %d",dl.event);
      }
      break;

    /*==============================
    **  I L L E G A L   S T A T E
    **==============================
    */
    default:
      /* illegal state
      */
      MSG_GERAN_LOW_1_G("Invalid downlink state %d",dl.state);
      break;
  }
} /* grlc_dl_state_machine() */

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      E_GRLC_DL_IS_PADDING_REMOVAL_REQUIRED()
===
===  DESCRIPTION
===
===    This is an access function provided for PL1 to call to determine if
===    padding octets removal is required given the pointer to the RLC header
===    and the MCS associated with it.
===
===    If this function returns TRUE then PL1 strips off the first 6 padding
===    octets from a payload.
===
===  PARAMETERS
===
===    const uint8 *hdr_ptr: pointer to the header of the EGPRS RLC Downlink Data Block
===    uint8 mcs:     the modulation and coding scheme of the data block
===
===  DEPENDENCIES
===
===    none
===
===  RETURN VALUE
===
===    boolean
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
boolean e_grlc_dl_is_padding_removal_required_dyn
(
  const uint8 *hdr_ptr,
  uint8       mcs
)
{
  register uint8 cps;

  switch (GET_HEADER_TYPE(mcs))
  {
    case HDR_TYPE_1:
      /*----------------------------------------------------
      ** MCS-7/8/9 contains no padding info so return FALSE
      **----------------------------------------------------
      */
      return FALSE;

    case HDR_TYPE_2:
      /*-------------------------------------------------------------------------
      ** MCS-5/6 contains padding info when CPS value is MCS_6_P1_PAD_MSC_8_RETX
      ** or MCS_6_P2_PAD_MCS_8_RETX
      **-------------------------------------------------------------------------
      */
      cps = (hdr_ptr[3] >> 1) & 0x07;
      return (cps == MCS_6_P1_PAD_MSC_8_RETX || cps == MCS_6_P2_PAD_MCS_8_RETX);

    case HDR_TYPE_3:
      /*-----------------------------------------------------------------------
      ** Due to the grey area in the spec both CPS and SPB must be considered
      ** when determining the presence of padding octets
      **-----------------------------------------------------------------------
      */
      cps = (hdr_ptr[3] >> 1) & 0x0f;

      /*-------------------------------------------------------------------
      ** Padding is only present when CPS == MCS-3 and the Split field SPB
      ** indicates the block is the first part of a split block
      **-------------------------------------------------------------------
      */

      /* spb = (hdr_ptr[3] >> 5) & 0x03
      */
      return ((cps == MCS_3_P1_PAD_MCS_8_RETX ||
               cps == MCS_3_P2_PAD_MCS_8_RETX ||
               cps == MCS_3_P3_PAD_MCS_8_RETX
              ) &&
              (((hdr_ptr[3] >> 5) & 0x03) == SPB_RETX_PART_1)
             );

    default:
      MSG_GERAN_LOW_1("ERLD padding_removal_req() wrong hdr_type %u",
                (GET_HEADER_TYPE(mcs)));
      break;
  }

  /* Superfluous return for Lint compliance
  */
  return FALSE;

} /* e_grlc_dl_is_padding_removal_required */

/*===========================================================================
===
===  FUNCTION      GRLC_DL_START_T3190()
===
===  DESCRIPTION
===
===    Start or re-start the T3190 timer.
===
===    This is provided such that MAC can re-start this timer when it receives
===    a DL timeslot Reconfigure message while RLC DL is active.
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
void grlc_dl_start_t3190_dyn(void)
{
  TIMER_START_T3190();
}

/*===========================================================================
===
===  FUNCTION      GRLC_DL_START_T3192()
===
===  DESCRIPTION
===
===    Stop the T3192 timer if it is active.
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
void grlc_dl_stop_t3192_dyn(void)
{
  TIMER_STOP_T3192();
}

/*===========================================================================
===
===  FUNCTION      GRLC_DL_START_T3192()
===
===  DESCRIPTION
===
===    Returns Timer T3192 status. This function is used by MAC to find whether
===    T3192 has been started by PL1.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    rm_timer_state_t
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
rm_timer_state_t grlc_dl_t3192_status_dyn(void)
{
  return dl.timer_t3192.state;
}

/*===========================================================================
===
===  FUNCTION      GRLC_DL_INIT()
===
===  DESCRIPTION
===
===    Clear all downlink variables to prepare for a new downlink TBF
===
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
void grlc_dl_init_vars(void)
{
  uint16  u=0;
  uint16 loop_cnt = E_RX_WORKSPACE;

  /* Set loop count to initialise elements of the rx array depending on tbf mode
  */
  if( NULL != p_rx_arr )
  {
    if( TBF_MODE_EGPRS == dl.tbf_mode )
    {
      loop_cnt = E_RX_WORKSPACE;
    }
    else
    {
      /* This is the minimum size for rx_arr */
      loop_cnt = RM_RLC_RX_WORKSPACE;
    }
  }
  else
  {
    /* p_rx_arr is NULL, already been freed earlier
    ** dont run the loop
    */
    loop_cnt = 0;
  }

  /*-----------------------------------------------------------------------
  ** Check and clear the downlink rx array, freeing the DSM items if found
  **-----------------------------------------------------------------------
  */

  for (u=0; u<loop_cnt; u++)
  {
    if (E_RX_ARR( u).dsm_payload_ptr != NULL)
    {
      if (E_RX_ARR( u).vn == RXS_RECEIVED)
      {
        /* free DSM only if item has been marked RECEIVED but not assembled
        */
#ifdef  GRLD_SHOW_NON_NULL_PAYLOAD
        MSG_GERAN_HIGH_2_G("GRLD dl_init blk[%u], vq=%d.dsm not NULL", u, dl.vq);
#endif  /*  */
        GPRS_PDU_FREE(&E_RX_ARR( u).dsm_payload_ptr);
      }
      else  /* Free partial split block during abnormal TBF Release  in EGPRS*/
      if ( (dl.tbf_mode == TBF_MODE_EGPRS) &&
           E_RX_ARR( u).vn == RXS_INVALID &&
           ( E_RX_ARR( u).spb == SPB_RETX_PART_1 ||
             E_RX_ARR( u).spb == SPB_RETX_PART_2) )
      {
#ifdef  GRLD_SHOW_NON_NULL_PAYLOAD
        MSG_GERAN_HIGH_2_G("GRLD Partial Split : dl_init blk[%u], vq=%d.dsm not NULL", u, dl.vq);
#endif
        GPRS_PDU_FREE(&E_RX_ARR( u).dsm_payload_ptr);
      }
      else
      if (E_RX_ARR( u).vn == RXS_INVALID)
      {
        MSG_GERAN_LOW_2_G("GRLD dl_init non-NULL dsm and vn=INVALID at rx_arr[%u] ptr=%x",
                  u,
                  E_RX_ARR( u).dsm_payload_ptr
                  );
      }
      else if (E_RX_ARR( u).vn == RXS_RECEIVED_ASSEMBLED)
      {
        MSG_GERAN_LOW_2_G("GRLD dl_init non-NULL dsm and vn=ASSEMBLED at rx_arr[%u] ptr=%x",
                  u,
                  E_RX_ARR( u).dsm_payload_ptr
                  );
      }
      else
      {
        MSG_GERAN_LOW_3_G("GRLD dl_init bad vn(%u) rx_arr[%u] ptr=%x",
                  E_RX_ARR( u).vn,
                  u,
                  E_RX_ARR( u).dsm_payload_ptr
                 );
      }
    }
  }

  /*----------------------------------------------------------------------
  ** Check and clear the current downlink PDU structure. This is cleared
  ** after a PDU is sent to the LLC but if for some reason it is not then
  ** free the resource and clear it to NULL
  **----------------------------------------------------------------------
  */
  if (cur_dl_pdu.dsm_ptr != NULL)
  {
    GPRS_PDU_FREE(&cur_dl_pdu.dsm_ptr);
    cur_dl_pdu.dsm_ptr = NULL;
  }

  /*-------------------------------------------------------
  ** Re-init all downlink operational variables and timers
  **-------------------------------------------------------
  */
  /*-----------------------------------------------------------------------
  ** If rx_arr is dynamically allocated, it can be freed here if not yet
  ** freed
  **-----------------------------------------------------------------------
  */
  if( NULL != p_rx_arr )
  {
    grlc_dl_free_rx_array();
  }

  dl.vr = 0;
  UPD_DL_VR();

#ifdef FEATURE_GSM_EGPRS_IR_WS_REDUCTION
  dl.vr_max = 0;
  dl.ms_out_of_mem = FALSE;
#endif /* FEATURE_GSM_EGPRS_IR_WS_REDUCTION */

  dl.vq = 0;
  UPD_DL_VQ();

  dl.ass_start = E_GRLC_INVALID_BSN;
  dl.ass_end   = E_GRLC_INVALID_BSN;

  dl.ssn = 0;
  UPD_DL_SSN();

  (void)memset(&dl.acknack, 0, sizeof(dl.acknack));
  (void)memset(&dl.dl_ass, 0, sizeof(dl.dl_ass));

  dl.b_first_block_ind_sent = FALSE;

  dl.cs = RM_CS_1; /* same as MCS_1 */
  dl.last_valid_channel_coding = RM_CS_1;

  dl.ws  = 64; /* Minimum value for 1 time slot see TS 04.60 Table 9.1.9.2.1 */
  dl.sp = 0;

  dl.bsn = 0;
  dl.sp = 0;
  dl.rrbp = 0;
  dl.fbi = RM_FBI_NOT_FINAL;
  dl.fbi_latch = RM_FBI_NOT_FINAL;
  dl.b_pdu_truncated = FALSE;
  dl.poll_queue_index = 0;
  dl.frame_number = 0;
  dl.time_slot = 0;
  dl.pbsn = 0;

  TIMER_STOP_T3190();
  grlc_dl_stop_t3192_dyn();
  dl.timer_t3190.state = TMR_INACTIVE;
  dl.timer_t3192.state = TMR_INACTIVE;
  dl.b_allow_t3192_to_start = FALSE;

  /*----------------------------------
  ** Re-init downlink state variables
  **----------------------------------
  */
  dl.state = DS_NULL;
  UPD_DL_STATE();

  dl.event = DE_NULL;

  dl.test_mode = TEST_MODE_OFF;

  dl.eng_mode_rcvd_dl_block_cnt = 0;

  dl.eng_mode_rcvd_dup_dl_block_cnt = 0;

  /*---------------------------------------
  ** Clear the current downlink data block
  **---------------------------------------
  */
  if (cur_dl_blk.dsm_payload_ptr != NULL)
  {
    MSG_GERAN_LOW_0_G("GRLD cur_dl_blk[].dsm not NULL");
    GPRS_PDU_FREE(&cur_dl_blk.dsm_payload_ptr);
  }
  (void)memset(&cur_dl_blk, 0, sizeof(cur_dl_blk));

   /* initialize to default dl nts */
   dl.dl_ass.num_dl_ts  = 1;

  /* Update for engg_mode
  */
  geran_eng_mode_data_write(ENG_MODE_GPRS_DL_CS,   &dl.cs, FALSE);
  geran_eng_mode_data_write(ENG_MODE_EGPRS_DL_MCS, &dl.cs, FALSE);

} /* grlc_dl_init */

#ifdef FEATURE_GPRS_PS_HANDOVER
/*===========================================================================
===
===  FUNCTION      grlc_dl_clr_psho_vars
===
===  DESCRIPTION
===
===
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
void grlc_dl_clr_psho_vars (void)
{
  dl.b_psho_active = FALSE;
  dl.blind_data_blks_ctr = 0;
  dl.psho_tbf_mode = TBF_MODE_INVALID;
}
#endif /* FEATURE_GPRS_PS_HANDOVER */

/*===========================================================================
===
===  FUNCTION      GRLC_DL_ONE_TIME_INIT
===
===  DESCRIPTION
===
===    Power up init of rlc downlink operation.
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
void grlc_dl_one_time_init (void)
{
#ifdef FEATURE_GPRS_PS_HANDOVER
  uint16 u = 0;
#endif /* #ifdef FEATURE_GPRS_PS_HANDOVER */


  /* Initialise GRLC DL message access control critical section */

  rex_init_crit_sect(&grlc_dl_msg_crit_sec);



  /* define the static timer structs.
  ** Only activated after call to rex_set_timer()
  ** One callback is used for all timers
  */

#ifdef PERLUTF
  #error code not present
#endif

  rex_def_timer_ex(&(dl.timer_t3190.rex_timer), grlc_dl_timer_callback,
        (uint32)TIMER_SET_PARAM( RM_TMR_T3190_AWAIT_VALID_DL_DATA));

  rex_def_timer_ex(&(dl.timer_t3192.rex_timer), grlc_dl_timer_callback,
        (uint32)TIMER_SET_PARAM( RM_TMR_T3192_AWAIT_RELEASE_DL_TBF));

  grlc_dl_fifo_one_time_init();

  /* call other functions to init individual sets of variables ..
  */
  dl.filler_block_cnt = 0;
  dl.timer_t3190_expiry_cnt = 0;
  dl.timer_t3192_expiry_cnt = 0;

  dl.rx_blk_cnt = 0;

  (void)q_init(&DMQ());
  (void)q_init(&DTQ());
  (void)q_init(&DPQ());

#if (!((defined (DEBUG_GSM_EGPRS_DS_UNIT_TEST)) || (defined (PERLUTF))))
  gprs_rlc_l1isr_init_buffers();
#endif

#ifdef ERLD_DUMP_12BYTES_PLD
  dl.dump_pld_once = FALSE;
#endif /* ERLD_DUMP_12BYTES_PLD  */

#ifndef FEATURE_GERAN_REDUCED_DEBUG
  /* Clear Diag-related vars
  */
  dl_diag_stats_st.grlc_dl_state = (uint32)DS_NULL;
  grlc_diag_clr_dl_statistics();
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

#ifdef FEATURE_GPRS_PS_HANDOVER
  /* Free the dsm items of the stored blocks if not freed.
  */
  for (u=0; u < dl.blind_data_blks_ctr; u++)
  {
    if (dl.dl_ass.tbf_mode == TBF_MODE_EGPRS)
    {
      if ( dl_psho_blind_arr[u].e_dl_blk.payload_1_ptr != NULL)
      {
        GPRS_PDU_FREE(&dl_psho_blind_arr[u].e_dl_blk.payload_1_ptr);
      }
      if ( dl_psho_blind_arr[u].e_dl_blk.payload_2_ptr != NULL)
      {
        GPRS_PDU_FREE(&dl_psho_blind_arr[u].e_dl_blk.payload_2_ptr);
      }
    }
    else if (dl.dl_ass.tbf_mode == TBF_MODE_GPRS)
    {
      if (dl_psho_blind_arr[u].dl_blk.pl1_blk_dsm_ptr != NULL)
      {
        GPRS_PDU_FREE(&dl_psho_blind_arr[u].dl_blk.pl1_blk_dsm_ptr);
      }
    }
    else
    {
      MSG_GERAN_LOW_0_G("GRLD ##Invalid tbf mode");
      break;
    }
  }
  grlc_dl_clr_psho_vars();
#endif /* FEATURE_GPRS_PS_HANDOVER */

  dl.b_block_tuneaway = FALSE;


} /* grlc_dl_one_time_init */

/*===========================================================================
===
===  FUNCTION      FLUSH_DL_MAC_QUEUE
===
===  DESCRIPTION
===
===    Read out all downlink MAC messages and free them
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
static void flush_dl_mac_queue (void)
{
  mac_rlc_msg_t * msg_ptr = NULL;

  while (q_cnt(&DMQ()) > 0)
  {
    msg_ptr = (mac_rlc_msg_t *)q_get(&DMQ());
    if (msg_ptr != NULL)
    {
      GPRS_MEM_FREE( msg_ptr );
    }
  };

  RLC_DL_CLR_MAC_SIG();

} /* flush_dl_mac_queue */

/*===========================================================================
===
===  FUNCTION      FLUSH_DL_PL1_QUEUE
===
===  DESCRIPTION
===
===    Read out all downlink PL1 messages and free them
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
static void flush_dl_pl1_queue (void)
{
  pl1_rlc_msg_t * msg_ptr = NULL;

  while (q_cnt(&DPQ()) > 0)
  {
    msg_ptr = (pl1_rlc_msg_t *)q_get(&DPQ());
    if (msg_ptr != NULL)
    {
      GPRS_MEM_FREE( msg_ptr );
    }
  };

  RLC_DL_CLR_PL1_SIG();

} /* flush_dl_pl1_queue */

/*===========================================================================
===
===  FUNCTION      FLUSH_DL_TMR_QUEUE
===
===  DESCRIPTION
===
===    Read out all downlink TIMER messages and free them
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
static void flush_dl_tmr_queue (void)
{
  rm_timer_msg_t * msg_ptr = NULL;

  while (q_cnt(&DTQ()) > 0)
  {
    msg_ptr = (rm_timer_msg_t *)q_get(&DTQ());
    if (msg_ptr != NULL)
    {
      GPRS_MEM_FREE( msg_ptr );
    }
  };

  RLC_DL_CLR_TMR_SIG();

} /* flush_dl_tmr_queue */

/*===========================================================================
===
===  FUNCTION      GRLC_DL_PANIC_RESET
===
===  DESCRIPTION
===
===    Perform a warm reset on the RLC DL state machine
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
static void grlc_dl_panic_reset (void)
{
  /* pointer to buffer holding message intended for MAC
  */
  rlc_mac_msg_t   *rlc_msg_ptr;

  /*---------------------------------------------------------------------*/

  /* Ensure no incoming messages
  */
  GRLC_DL_MSG_LOCK();

  /*-----------------------------------------------------------------
  ** Read out all un-read blocks and free DSM item for each to drain
  ** out un-read items from the FIFO
  **-----------------------------------------------------------------
  */
  grlc_dl_fifo_clear();

  /* Stop all timers and initialise all DL variables including the RX arrays
  ** Free the current LLC PDU as well
  */
  grlc_dl_init_vars();

#ifdef FEATURE_GPRS_PS_HANDOVER
  grlc_dl_delete_blind_data_blocks();
  grlc_dl_clr_psho_vars();
#endif /* FEATURE_GPRS_PS_HANDOVER */

  /* Free all incoming downlink messages in MAC/PL1 and TIMER queues
  */
  flush_dl_mac_queue();
  flush_dl_pl1_queue();
  flush_dl_tmr_queue();

  /* Initialise the rest of the LLC PDU member variables
  */
  grlc_dl_init_llc_pdu ( &cur_dl_pdu, GRLC_MODE_ACK);

#ifndef FEATURE_GERAN_REDUCED_DEBUG
  /* clear diagnostic and statistic vars
  */
  grlc_diag_clr_dl_statistics();
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

  /* Allow other tasks to run now
  */
  GRLC_DL_MSG_UNLOCK();

  MSG_GERAN_MED_0_G("GRLD Resetting ..");

  /* Send Reset Confirmation to MAC
  */
  rlc_msg_ptr = mac_get_cmd_buf_rlc();
  if (rlc_msg_ptr != NULL)
  {
    rlc_msg_ptr->msg.dl_reset_cnf.sig = RM_DL_RESET_CNF;
    mac_put_cmd_rlc( rlc_msg_ptr);
  }
  else
  {
    ERR_GERAN_FATAL_0_G("GRLD heap exhaustion");
  }

  /* Un-block tuneaway explicitly
  */
  dl.b_block_tuneaway = FALSE;
  geran_api_set_tuneaway_blocked(GERAN_API_CLIENT_RLC, FALSE);

} /* grlc_dl_panic_reset */

/*===========================================================================
===
===  FUNCTION      GRLC_DL_HANDLE_PL1_SIG
===
===  DESCRIPTION
===
===    Wrapper function to set the event to L1 and invoke the RLC downlink
===    state machine.
===
===    The mask is cleared if there are no more PL1 messages in the queue
===
===  DEPENDENCIES
===
===    rlc dl state machine must be initialised.
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_dl_handle_pl1_sig_dyn(void)
{
  /* mark the event being serviced
  */
  dl.event = DE_PL1;

  /* process downlink state machine with L1 event type
  */
  grlc_dl_state_machine();

  /* clear the input event after servicing
  */
  dl.event = DE_NULL;

  /* Always return completed
  */
  GRLC_DL_MSG_LOCK();

  /* It could happen that L1 has put a block in DL_FIFO and
  ** GRLC_DL_PL1_SIG is set , implying that grlc_dl_fifo.ctr
  ** has been incremented , thus RLC should remain in context
  ** such that it can process the block and also send DL_ACK_NACK
  */
  if (grlc_dl_fifo_get_cnt() == 0)
  {
    RLC_DL_CLR_PL1_SIG();
  }

  GRLC_DL_MSG_UNLOCK();

} /* grlc_dl_handle_pl1_sig */

/*===========================================================================
===
===  FUNCTION      GRLC_DL_HANDLE_PL1_EPDAN_REQ_SIG
===
===  DESCRIPTION
===
===    Wrapper function intended to invoke the RLC DL state machine to process
===    the EPDAN. The signal originates from PL1.
===
===    The mask is cleared if there are no more PL1 messages in the queue
===
===  DEPENDENCIES
===
===    rlc dl state machine must be initialised.
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_dl_handle_pl1_epdan_req_sig_dyn(void)
{
  /* mark the event being serviced
  */
  dl.event = DE_PL1;

  /* process downlink state machine with L1 event type
  */
  grlc_dl_state_machine();

  /* clear the input event after servicing
  */
  dl.event = DE_NULL;

  /* Always return completed
  */
  GRLC_DL_MSG_LOCK();

  if (q_cnt(&DPQ()) == 0)
  {
    RLC_DL_CLR_PL1_EPDAN_REQ_SIG();
  }

  /* Processing the EPDAN can result in clearing the DL FIFO when the PH_DATA_IND
  ** signal is active. Clear the signal if the DL FIFO is empty.
  */
  if (grlc_dl_fifo_get_cnt() == 0)
  {
    RLC_DL_CLR_PL1_SIG();
  }

  GRLC_DL_MSG_UNLOCK();

} /* grlc_dl_handle_pl1_sig */

/*===========================================================================
===
===  FUNCTION      GRLC_DL_HANDLE_TMR_SIG
===
===  DESCRIPTION
===
===    Wrapper function set the event to TIMER and to invoke the RLC DL
===    state machine to process all downlink timer events
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
===    alter module var event
===
===========================================================================*/
void grlc_dl_handle_tmr_sig_dyn(void)
{
  /* mark the event being serviced
  */
  dl.event = DE_TMR;

  /* process downlink state machine with timer event type
  */
  grlc_dl_state_machine();

  /* clear the input event after servicing
  */
  dl.event = DE_NULL;

  /* Clear the mask if there are no more messages in the queue
  */
  GRLC_DL_MSG_LOCK();

  if (q_cnt(&DTQ()) == 0)
  {
    RLC_DL_CLR_TMR_SIG();
  }

  GRLC_DL_MSG_UNLOCK();

} /* grlc_dl_handle_tmr_sig */

/*===========================================================================
===
===  FUNCTION      GRLC_DL_HANDLE_MAC_SIG
===
===  DESCRIPTION
===
===    Wrapper function to loop through and invoke the RLC DL state machine
===    to process all downlink MAC commands in the rlc dl MAC command queue.
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
===    alter module var event
===
===========================================================================*/
void grlc_dl_handle_mac_sig_dyn(void)
{
  /* mark the event to be serviced
  */
  dl.event = DE_MAC;

  /* process downlink state machine with MAC event type
  */
  grlc_dl_state_machine();

  /* clear the input event after servicing
  */
  dl.event = DE_NULL;

  /* Clear the mask if there are no more messages in the queue
  */
  GRLC_DL_MSG_LOCK();

  if (q_cnt(&DMQ()) == 0)
  {
    RLC_DL_CLR_MAC_SIG();
  }

  GRLC_DL_MSG_UNLOCK();

} /* grlc_dl_handle_mac_sig */


/*** EOF: don't remove! ***/
