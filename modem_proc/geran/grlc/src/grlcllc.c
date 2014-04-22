/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC ACCESS FUNCTION TO LLC
***
***
*** DESCRIPTION
***
***  This module implements the access function provided by RLC to support
***  the interface with LLC.
***
***
*** EXTERNALIZED FUNCTIONS
***
***  grlc_llc_ul_put_mes_buf()
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
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
*** $Header: //components/rel/geran.mpss/5.2.0/grlc/src/grlcllc.c#5 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 08/11/13    hv     CR561807
*** 07/11/13    ra     CR547423 - Fixed regression of the fix for CR539621
*** 10/10/13    hv     CR555392
*** 05/10/13    gk     CR539621:RLC to flow control when UL PDUs are placed in temp queue (ULTQ)
*** 03/28/01    hv     RLC access function to LLC
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
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

#include "comdef.h"
#include "gprs_mem.h"
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "grlcllc.h"
#include "msg.h"
#include "gprs_pdu.h"
#include "gllc.h"
#ifndef DEBUG_GSM_GPRS_DS_UNIT_TEST
#include "gllc_v.h"
#include "gllc_v_g.h"
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */
#include "grlcutil.h"
#include "grlcul.h"
#include "grlcultypes.h"
#include "grlculproc.h"
#include "grlcultst.h"
#include "queue.h"
#include "gmutex.h"
#include "geran_tasks.h"
#include "geran_multi_sim.h"

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


/*===========================================================================
** Definitions of parameters used in flow control calculations in EGPRS mode
**===========================================================================
*/


/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/

/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      SHOW_FCTRL()
===
===  DESCRIPTION
===
===     Show the flow control status between LLC and RLC.
===     Not in use
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
static void show_fctrl(void)
{
  MSG_GERAN_HIGH_8_G("GRLC tbf %d FCtrl-Status=%d (0=RESUME, 1=SUSPEND) E_HI_WM=%u E_LO_WM=%u G_HI_WM=%u G_LO_WM=%u llc_octets=%lu temppdu=%u",
    ul.cur_res.tbf_mode,
    (ul.flow_control_suspend != 0),
    ERLCLLC_HIWATER_OCTETS(),
    ERLCLLC_LOWATER_OCTETS(),
    GRLCLLC_HIWATER_OCTETS(),
    GRLCLLC_LOWATER_OCTETS(),
    ul.llc_octets + ul.temp_queue_octet_cnt,
    ul.temp_queue_pdu_cnt);
} /* show_fctrl */

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GRLC_GET_TEMP_QUEUE_OCTET_COUNT()
===
===  DESCRIPTION
===
===    This function provides current octet and PDU count in RLC UL temp queue
===    (ULTQ). LLC places PDUs in this queue from LLC task context. RLC then
===    removes PDUs from RLC task context. Therefore, this fucntion should be
===    used under mutex protection to get accurate octet count in temp queue.
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
===
===========================================================================*/

void grlc_get_temp_queue_octet_count ( uint32 *temp_queue_octet_cnt, uint16 *temp_queue_pdu_cnt )
{

  gprs_grr_ul_mes_t *msg_ptr;

  /* Get current octet count in ULTQ buffer. */

  msg_ptr = q_check( &ULTQ() );

  *temp_queue_octet_cnt = 0UL;
  *temp_queue_pdu_cnt = 0;

  while ( msg_ptr != NULL )
  {
    *temp_queue_octet_cnt += (uint32) msg_ptr->orig_len;
    *temp_queue_pdu_cnt    = *temp_queue_pdu_cnt + 1;

    msg_ptr = q_next( &ULTQ(), &msg_ptr->link );
  }

}


/*===========================================================================
===
===  FUNCTION      GRLC_LLC_UL_FLOW_CONTROL()
===
===  DESCRIPTION
===
===    For use by RLC to flow control LLC as a function of the current octet
===    counts. The absolute LLC octet count is used to test against the
===    upper flow control threshold, and the notional LLC octet counts is
===    used to test against the lower flow control threshold.
===
===    During the US_AWAIT_MAC_IDLE state, such as during cell reslection, it
===    is desirable to suspend data flow from LLC AND SNDCP, otherwise
===    potential RAU and XID reset procedure following cell reslection will
===    result in excessive purging of queued data in RLC (CR54783)
===
===    During RLC User Plane Suspended state, PDUs are not subjected to
===    flow control.
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
===
===========================================================================*/
void grlc_llc_ul_flow_control(void)
{
  /* Note - during RLC Suspended state PDUs are not subjected to flow control. */

  /* This fn can be called from both RLC context and LLC context. The latter, via
  ** enqueue_pdu(). So we need to lock out RLC when it is called from LLC context
  ** and vice-versa. However, we do not need to lock interrupts.
  **
  ** Hence replace intlock() with tasklock()
  */
  GRLC_UL_MSG_LOCK();

  /* Get total octet count held in temp queue (ULTQ). */

  grlc_get_temp_queue_octet_count ( &ul.temp_queue_octet_cnt, &ul.temp_queue_pdu_cnt );


  if (ul.suspend_state != GRLC_SUSPEND)
  {

    if ( ( ( ul.llc_octets + ul.temp_queue_octet_cnt ) >
#ifdef  DEBUG_GSM_GPRS_DS_UNIT_TEST
          #error code not present
#else
          ((ul.cur_res.tbf_mode == TBF_MODE_EGPRS) ?
           ERLCLLC_HIWATER_OCTETS() :
           GRLCLLC_HIWATER_OCTETS()
          )
#endif
         )
         ||
         ( ul.llc_pdus + ul.temp_queue_pdu_cnt > NUM_LLC_PDUS_HIGH )
       )
    {
      if ( ul.flow_control_suspend == FALSE )
      {
        ul.flow_control_suspend = TRUE;
        show_fctrl();
        gllc_grr_ul_suspend_ind();
      }
    }

    else
    if
    ( ( ( ul.llc_octets + ul.temp_queue_octet_cnt ) <=
#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
      #error code not present
#else
        ((ul.cur_res.tbf_mode == TBF_MODE_EGPRS) ?
         ERLCLLC_LOWATER_OCTETS() :
         GRLCLLC_LOWATER_OCTETS()
        )
#endif
      )
      /* both octet and PDU counts must be low in order to resume
      */
      &&
      ( ul.llc_pdus + ul.temp_queue_pdu_cnt < NUM_LLC_PDUS_LOW )
    )
    {
      if (ul.flow_control_suspend == TRUE)
      {
        ul.flow_control_suspend = FALSE;
        show_fctrl();
        gllc_grr_ul_resume_ind();
      }
    }
  }

  GRLC_UL_MSG_UNLOCK();

} /* end of grlc_llc_ul_flow_control() */

/*===========================================================================
===
===  FUNCTION      GRLC_LLC_UL_PUT_MES_BUF
===
===  DESCRIPTION
===
===    For use by the LLC layer implementation to put a dynamically
===    allocated message resource of type gprs_grr_ul_mes_t to RLC's GRR SAP
===    uplink message queue.
===
===  DEPENDENCIES
===
===    LLC entity is responsible for dynamic message resource allocation
===    for the given message.
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    May cause a context switch.
===
===========================================================================*/

void grlc_llc_ul_put_mes_buf_dyn(gprs_grr_ul_mes_t* msg_ptr)
{
  if ( msg_ptr == NULL )
  {
    MSG_GERAN_ERROR_0( "GRLU LLC Put: Msg NULL");
  }
#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
  #error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST*/

  else 
  {
    msg_ptr->dup_pdu_ptr = NULL;

    switch ( msg_ptr->prim )
    {
    case GRR_DATA_REQ:
    case GRR_UNITDATA_REQ:

      /* Store octet length of PDU
      */
      msg_ptr->orig_len = (uint16)GPRS_PDU_LENGTH(msg_ptr->pdu_ptr);
      msg_ptr->len = msg_ptr->orig_len;

      /* Put the message on RLC UL Temporary PDU queue and set the appropriate signal.
      ** Message will be freed by RLC.
      ** Avoid calling enqueue_pdu(msg_ptr) directly
      */
  

      GRLC_UL_MSG_LOCK();

      (void)q_link(msg_ptr, &msg_ptr->link);
      q_put(&ULTQ(), &msg_ptr->link);

#ifdef  FEATURE_GPRS_PS_HANDOVER
      if (!ul.b_psho_active)
#endif
      {
        RLC_UL_SET_LLC_TMP_PDU_SIG();
      }

      /* This is the entry point to RLC when sending UL PDUs. Flow control here. */
      grlc_llc_ul_flow_control();
      GRLC_UL_MSG_UNLOCK();

      break;

    case GRR_SUSPEND_REQ:
    case GRR_PURGE_SUSPENDED_DATA_REQ:
    case GRR_RESUME_REQ:
    case GRR_PURGE_DATA_REQ:

      /* Put the message on RLC UL MSG queue and set the appropriate signal.
      ** Message will be freed by RLC
      */
      GRLC_UL_MSG_LOCK();

      (void)q_link(msg_ptr, &msg_ptr->link);
      q_put(&ULMQ(), &msg_ptr->link);

      RLC_UL_SET_LLC_MSG_SIG();

      GRLC_UL_MSG_UNLOCK();

      break;

      case GRR_TEST_MODE_REQ:
      grr_test_mode_req( msg_ptr );
      GPRS_MEM_FREE( msg_ptr );
      break;
    default:

      MSG_GERAN_LOW_1( "GRLU LLC Put: Invalid Prim %d", msg_ptr -> prim);
      GPRS_MEM_FREE( msg_ptr );
      break;
    } /* switch */
  }
} /* end of grlc_llc_ul_put_mes_buf() */


/*===========================================================================
===
===  FUNCTION      GRLC_LLC_UL_GET_CURRENT_OCTET_COUNT
===
===  DESCRIPTION
===
===    For use by the LLC layer implementation to query the quantity of
===    data and signalling currently awaiting transmission.
===
===    A typical usage of this enquiry would be in support of the Follow
===    On Request feature.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Current octet count of the LLC PDU queue.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/
uint32 grlc_llc_get_current_octet_count_dyn(void)
{
  uint32            octet_count;
  gprs_grr_ul_mes_t *ptr;

  /* This fn is called exclusively from LLC context. So we only need to lock out
  ** RLC task. Hence replace intlock() with tasklock()
  */
  GRLC_UL_MSG_LOCK();
  octet_count = ul.llc_octets;

  if ( ul.suspend_state == GRLC_SUSPEND )
  {
    ptr = q_check( &grlc_ul_old_llc_q );

    while ( ptr != NULL )
    {
      octet_count += ptr -> len;
      ptr          = q_next( &grlc_ul_old_llc_q, &ptr -> link );
    }
  }
  GRLC_UL_MSG_UNLOCK();

  return ( octet_count );
}


/*** EOF: don't remove! ***/

