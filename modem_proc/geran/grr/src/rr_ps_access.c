/*! \file rr_ps_access.c

  This module contains the functionality behind the API in rr_ps_if.h.
  It co-ordinates the enabling/disabling is PS access and manages T3142.

                Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_ps_access.c#5 $ */
/* $DateTime: 2018/10/16 07:03:13 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "rr_seg_load.h"
#include "rr_ps_access.h"
#include "rr_ps_if.h"
#include "rr_task.h"
#include "rr_defs.h"
#include "rr_gprs_debug.h"
#include "rr_mac_send.h"
#include "rr_general.h"
#include "rr_ps_domain.h"
#ifdef FEATURE_GSM_DTM
#include "rr_nv.h"
#endif // FEATURE_GSM_DTM
#include "rr_thermal.h"
#include "gprs_mem.h"
#include "rr_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum
{
  RR_PS_STATE_MAC_NO_ACCESS,
  RR_PS_STATE_MAC_ACCESS,
  RR_PS_STATE_ENABLING_MAC_ACCESS,
  RR_PS_STATE_DISABLING_MAC_ACCESS,
  RR_PS_STATE_MAX
} rr_ps_state_e;

typedef struct
{
  rr_ps_state_e  old_state;
  rr_ps_state_e  state;
  struct
  {
    ARFCN_T  arfcn;
    BSIC_T   bsic;
  }              t3142_info;
  boolean        del_all_confirmed;
} rr_ps_access_data_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static rr_ps_access_data_t rr_ps_access_data;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns a pointer to the module data.
 *
 *
 * \return rr_ps_access_data_t*
 */
static rr_ps_access_data_t *rr_ps_access_get_data_ptr( void )
{
  return &rr_ps_access_data;
}

/*!
 * \brief Returns a character string representing the state indicated.
 *
 * \param state
 *
 * \return const char*
 */
static const char *rr_ps_access_state_name(rr_ps_state_e state)
{
  switch (state)
  {
    case RR_PS_STATE_MAC_NO_ACCESS:
      return "MAC_NO_ACCESS";

    case RR_PS_STATE_MAC_ACCESS:
      return "MAC_ACCESS";

    case RR_PS_STATE_ENABLING_MAC_ACCESS:
      return "ENABLING_MAC_ACCESS";

    case RR_PS_STATE_DISABLING_MAC_ACCESS:
      return "DISABLING_MAC_ACCESS";

    default:
      return "?";
  }
} /* end rr_ps_access_state_name() */

/*!
 * \brief Resets the cell ARFCN+BSIC associated with T3142.
 *
 */
static void reset_t3142_info( )
{
  rr_ps_access_data_t *rr_ps_access_data_ptr = rr_ps_access_get_data_ptr();

  //RR_NULL_CHECK_FATAL(rr_ps_access_data_ptr);

  rr_ps_access_data_ptr->t3142_info.arfcn.num = INVALID_ARFCN;
  rr_ps_access_data_ptr->t3142_info.arfcn.band = INVALID_BAND;
  rr_ps_access_data_ptr->t3142_info.bsic.BS_colour_code = INVALID_BSIC_CC;
  rr_ps_access_data_ptr->t3142_info.bsic.PLMN_colour_code = INVALID_PLMN_CC;

} /* end reset_t3142_info() */

/*!
 * \brief Returns TRUE if T3142 is running and the cell is unchanged.
 *
 *  The current cell ARFCN+BSIC is compared against against the ARFCN+BSIC recorded at the time T3142 was
 *  started. If the cell has changed, T3142 is stopped.
 *
 *
 * \return boolean
 */
static boolean check_t3142_running( )
{
  boolean t3142_running;

  t3142_running = rr_timer_is_running(T3142);

  if (t3142_running)
  {
    rr_ps_access_data_t *rr_ps_access_data_ptr;
    gprs_scell_info_t *cell_info_ptr;
    ARFCN_T arfcn;
    BSIC_T bsic;

    rr_ps_access_data_ptr = rr_ps_access_get_data_ptr();
    //RR_NULL_CHECK_FATAL(rr_ps_access_data_ptr);

    cell_info_ptr = rr_gprs_get_scell_info();
    //RR_NULL_CHECK_FATAL(cell_info_ptr);

    arfcn = cell_info_ptr->gsm.BCCH_ARFCN;
    bsic = cell_info_ptr->gsm.BSIC;

    /* Check if still on the same cell where T3142 was started */
    if (ARFCNS_EQUAL(rr_ps_access_data_ptr->t3142_info.arfcn, arfcn) &&
        rr_same_BSIC(&(rr_ps_access_data_ptr->t3142_info.bsic),&bsic))
    {
      MSG_GERAN_LOW_0_G("T3142 still running (cell unchanged)");
    }
    else
    {
      MSG_GERAN_MED_0_G("T3142 stopped as cell changed");
      (void)rr_timer_cancel(T3142);
      t3142_running = FALSE;

      reset_t3142_info();
    }
  }

  return t3142_running;

} /* end check_t3142_running() */

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Start T3142 with the time-out value indicated. The ARFCN+BSIC of the current cell is recorded.
 *
 * \param t3142_timeout_secs
 */
void grr_start_t3142(
  uint8 t3142_timeout_secs
   
)
{
  // Only start T3142 if the timeout value in IARej is not zero
  if (t3142_timeout_secs != 0)
  {
    rr_ps_access_data_t *rr_ps_access_data_ptr;
    gprs_scell_info_t *cell_info_ptr;
    ARFCN_T arfcn;
    BSIC_T bsic;
    int t3142_timeout_ms;

    rr_ps_access_data_ptr = rr_ps_access_get_data_ptr();
    //RR_NULL_CHECK_FATAL(rr_ps_access_data_ptr);

    cell_info_ptr = rr_gprs_get_scell_info();
    //RR_NULL_CHECK_FATAL(cell_info_ptr);

    arfcn = cell_info_ptr->gsm.BCCH_ARFCN;
    bsic = cell_info_ptr->gsm.BSIC;

    // Convert value from secs to milliseconds
    t3142_timeout_ms = (int) t3142_timeout_secs * 1000;

    MSG_GERAN_HIGH_1_G("Starting T3142 (%d secs)",t3142_timeout_secs);

    // Start T3142, using millisecond timer value
    (void)rr_timer_start(T3142, (dword)t3142_timeout_ms);

    // Record the current serving cell ARFCN & BSIC where T3142 started
    rr_ps_access_data_ptr->t3142_info.arfcn = arfcn;
    rr_ps_access_data_ptr->t3142_info.bsic = bsic;
  }
  else
  {
    MSG_GERAN_HIGH_0_G("T3142 timeout value is 0, not starting T3142");
  }

} /* end grr_start_t3142() */

/*!
 * \brief Returns TRUE if an UL TBF establishment is allowed.
 *
 *  An UL TBF establishment is allowed unless:
 *   - prevented by thermal mitigation
 *   - the cell does not support GPRS
 *   - GMM has not requested GPRS service
 *   - T3142 is running
 *   - PS abort timer is running (if FEATURE_DUAL_SIM defined)
 *   - the access class of the cell does not allow access
 *
 *
 * \return boolean
 */
boolean rr_mac_ul_tbf_est_is_allowed( )
{
  boolean ps_allowed = TRUE;


#ifdef FEATURE_RR_THERMAL_MITIGATION
  if (!rr_thermal_ps_allowed())
  {
    MSG_GERAN_HIGH_0_G("PS access blocked due to Thermal Mitigation");
    ps_allowed = FALSE;
  }
#endif

  if (!rr_cell_supports_gprs())
  {
    MSG_GERAN_HIGH_0_G("PS access blocked as cell does not support GPRS");
    ps_allowed = FALSE;
  }

  if (!rr_gmm_gprs_service_requested())
  {
    MSG_GERAN_HIGH_0_G("PS access blocked as GPRS service not requested");
    ps_allowed = FALSE;
  }

  if (check_t3142_running())
  {
    MSG_GERAN_HIGH_0_G("PS access blocked by T3142");
    ps_allowed = FALSE;
  }

  if (rr_ps_blocked_due_to_external_request())
  {
    MSG_GERAN_HIGH_0_G("PS blocked due to external request");
    ps_allowed = FALSE;
  }
  
#ifdef FEATURE_GERAN_IMPLICIT_PS_REJECT

  if (rr_timer_is_running(T3236))
  {
    MSG_GERAN_HIGH_0_G("PS access blocked by T3236");
    ps_allowed = FALSE;
  }
#endif /*FEATURE_GERAN_IMPLICIT_PS_REJECT*/

  if (ps_allowed)
  {
    gprs_scell_info_t *cell_info_ptr;
    access_result_T access_result;

    cell_info_ptr = rr_gprs_get_scell_info();
    //RR_NULL_CHECK_FATAL(cell_info_ptr);

    // Check the cell's access class settings
    access_result = rr_check_access_class_for_camping(&cell_info_ptr->gsm);

    if ((access_result != ALL_CALLS_ALLOWED) &&
        (access_result != NORMAL_CALLS_ALLOWED))
    {
      MSG_GERAN_HIGH_0_G("PS access blocked by cell access check");
      ps_allowed = FALSE;
    }
  }

  if (ps_allowed)
  {
    MSG_GERAN_HIGH_0_G("PS access allowed");
  }

  return ps_allowed;

} /* end rr_mac_ul_tbf_est_is_allowed() */

#ifdef FEATURE_GSM_DTM
/*!
 * \brief Returns TRUE if an UL transfer via GTTP is allowed.
 *
 *  An UL transfer via GTTP is allowed unless:
 *   - DTM is not enabled in NV
 *   - prevented by thermal mitigation
 *   - RR is not in dedicated
 *   - the cell does not support DTM
 *   - GMM has not requested GPRS service
 *
 *
 * \return boolean
 */
boolean rr_mac_gttp_transfer_is_allowed( )
{
  boolean ps_allowed = FALSE;

  if (rr_get_nv_dtm_enabled() && rr_in_dedicated())
  {
    ps_allowed = TRUE;

#ifdef FEATURE_RR_THERMAL_MITIGATION
    if (!rr_thermal_ps_allowed())
    {
      MSG_GERAN_HIGH_0_G("PS access blocked due to Thermal Mitigation");
      ps_allowed = FALSE;
    }
#endif

    if (!rr_cell_supports_dtm())
    {
      MSG_GERAN_HIGH_0_G("PS access blocked as cell does not support DTM");
      ps_allowed = FALSE;
    }

    if (!rr_gmm_gprs_service_requested())
    {
      MSG_GERAN_HIGH_0_G("PS access blocked as GPRS service not requested");
      ps_allowed = FALSE;
    }

    if (rr_ps_blocked_due_to_external_request())
    {
      MSG_GERAN_HIGH_0_G("PS blocked due to external request");
      ps_allowed = FALSE;
    }
  }

  if (ps_allowed)
  {
    MSG_GERAN_HIGH_0_G("PS access allowed (GTTP)");
  }

  return ps_allowed;

} /* end rr_mac_gttp_transfer_is_allowed() */
#endif // FEATURE_GSM_DTM

/*!
 * \brief Returns TRUE if a DL TBF establishment is allowed.
 *
 *  A DUL TBF establishment is allowed unless:
 *   - PS domain is not active (i.e. no TLLI assigned
 *
 *
 * \return boolean
 */
boolean rr_mac_dl_tbf_est_is_allowed( )
{
  gprs_scell_info_t *cell_info_ptr;
  boolean ps_allowed = FALSE;

  cell_info_ptr = rr_gprs_get_scell_info();
  //RR_NULL_CHECK_FATAL(cell_info_ptr);

  if (cell_info_ptr->gsm.gprs_indicator)
  {
    if (rr_ps_blocked_due_to_external_request())
    {
      MSG_GERAN_HIGH_0_G("PS blocked due to external request");
      ps_allowed = FALSE;
    }
    else
    {
      ps_allowed = rr_ps_domain_active();
    }
  }

  return ps_allowed;
} /* end rr_mac_dl_tbf_est_is_allowed() */

/*!
 * \brief Called when T3142 expires.
 *
 *  RR_IMSG_PS_ACCESS_REQ is sent to RR if UL TBF establishment is now allowed
 *
 */
void rr_ps_t3142_expiry( )
{

  if (rr_mac_ul_tbf_est_is_allowed())
  {
    MSG_GERAN_HIGH_0_G("T3142 expired - enable PS access");
    rr_send_imsg_ps_access_req(
      TRUE,    // purge_gmm_signalling
      NULL    // confirm_cb_fn
    );
  }
  else
  {
    MSG_GERAN_HIGH_0_G("T3142 expired - PS access still blocked");
  }

} /* end rr_ps_t3142_expiry() */

#ifdef FEATURE_GERAN_IMPLICIT_PS_REJECT

/*!
 * \brief Called when T3236 expires.
 *
 *  RR_IMSG_PS_ACCESS_REQ is sent to RR if UL TBF establishment is now allowed
 *
 */
void rr_ps_t3236_expiry( )
{
  rr_ps_update_implicit_reject(FALSE);
  if (rr_mac_ul_tbf_est_is_allowed())
  {
    MSG_GERAN_HIGH_0_G("T3236 expired - enable PS access");
    rr_send_imsg_ps_access_req(
      TRUE,    // purge_gmm_signalling
      NULL    // confirm_cb_fn
    );
  }
  else
  {
    MSG_GERAN_HIGH_0_G("T3236 expired - PS access still blocked");
  }

} /* end rr_ps_t3142_expiry() */

#endif /*FEATURE_GERAN_IMPLICIT_PS_REJECT*/

/*!
 * \brief Ensures that PS access is blocked with a DEL ALL action.
 *
 *  GRR_MAC_NO_PS_ACCESS_IND is sent to GMAC indicating deletion of all PDUs, if that was not the previous
 *  action
 *
 */
void rr_ps_no_ps_access_del_all( )
{

  // Disable PS access & purge if this is not the current MAC PS access status
  if (!rr_mac_ps_access_blocked_del_all())
  {
    rr_ps_access_data_t *rr_ps_access_data_ptr = rr_ps_access_get_data_ptr();
    //RR_NULL_CHECK_FATAL(rr_ps_access_data_ptr);

    rr_ps_access_data_ptr->del_all_confirmed = FALSE;

    rr_send_grr_mac_no_ps_access_ind(DEL_ALL);
  }

} /* end rr_ps_no_ps_access_del_all() */

#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_TRIPLE_SIM)
/*!
 * \brief Indicates if PS access has been blocked with a DEL ALL action.
 *  
 *  
 * \return boolean - TRUE if this is confirmed, FALSE otherwise
 */
boolean rr_ps_access_blocked_del_all( )
{
  rr_ps_access_data_t *rr_ps_access_data_ptr = rr_ps_access_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_ps_access_data_ptr);

  return rr_ps_access_data_ptr->del_all_confirmed;

} /* end rr_ps_access_blocked_del_all() */

/*!
* \brief Indicates if PS access has been blocked and MAC has confirmed.
*  
*  
* \return boolean - TRUE if this is confirmed, FALSE otherwise
*/
static boolean rr_ps_access_blocked( )
{
  if ((rr_mac_ps_access_granted()) ||
      ((rr_mac_ps_access_blocked()) && (rr_macinfo_get_mac_status() != MAC_IDLE)))
  {
    /*Sub 'other_gas_id' does not have PS blocked (or MAC has not confirmed yet) - exit*/
    return FALSE;
  }

  return TRUE;
} /* end rr_ps_access_blocked() */

/*!
 * \brief Indicates if all other subs have PS access blocked.
 * 
 * 
 * \return boolean - TRUE if all other subs have PS access blocked, FALSE otherwise
 */
boolean rr_ps_access_blocked_other_subs( )
{

  {
    {
      if (FALSE == rr_ps_access_blocked())
      {
        // Sub 'other_gas_id' does not have PS blocked - exit
        return FALSE;
      }
    }
  }

  // All other subs have PS blocked in DEL_ALL state
  return TRUE;

} /* end rr_ps_access_blocked_other_subs() */

/*!
 * \brief Indicates if all other subs have PS access blocked in DEL_ALL state.
 * 
 * 
 * \return boolean - TRUE if all other subs have PS access blocked in DEL_ALL state, FALSE otherwise
 */
boolean rr_ps_access_blocked_del_all_other_subs( )
{

  {
    {
      if (rr_ps_access_blocked_del_all() == FALSE)
      {
        // Sub 'other_gas_id' does not have PS blocked in DEL_ALL state - exit
        return FALSE;
      }
    }
  }

  // All other subs have PS blocked in DEL_ALL state
  return TRUE;

} /* end rr_ps_access_blocked_del_all_other_subs() */
#endif // #if defined (FEATURE_DUAL_SIM) || defined (FEATURE_TRIPLE_SIM)

/*!
 * \brief The RR-PS-ACCESS state machine
 *
 * \param msg_ptr
 */
void rr_ps_access_control(
  const rr_cmd_bdy_type *msg_ptr
   
)
{
  rr_ps_access_data_t *rr_ps_access_data_ptr;

  /* Validate input */
  //RR_NULL_CHECK_RETURN_VOID(msg_ptr);

  rr_ps_access_data_ptr = rr_ps_access_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_ps_access_data_ptr);

  /*
  Before performing the normal message processing per state, just check that the current state machine
  state matches the actual state of MAC access / no access. These could be different if a module has
  updated MAC directly via the functions in rr_mac_send.c, rather than using this module.
  If the MAC access / no access state is mis-matched with this state machine state, correct it before
  processing a request for action
  */

  switch (msg_ptr->message_header.message_set)
  {
    case MS_RR_RR:
    {
      switch (msg_ptr->rr.header.rr_message_set)
      {
        case RR_PS_IMSG:
        {
          switch (msg_ptr->message_header.message_id)
          {
            case RR_PS_IMSG_PS_ACCESS_REQ:
            case RR_PS_IMSG_NO_PS_ACCESS_REQ:
            {
              if (rr_mac_ps_access_granted())
              {
                if (rr_ps_access_data_ptr->state != RR_PS_STATE_MAC_ACCESS)
                {
                  MSG_GERAN_HIGH_2_G("Correcting RR-PS-ACCESS-CONTROL state (%d->%d)",
                           (int)rr_ps_access_data_ptr->state,(int)RR_PS_STATE_MAC_ACCESS);

                  rr_ps_access_data_ptr->state = RR_PS_STATE_MAC_ACCESS;
                }
              }
              else
              {
                if (rr_ps_access_data_ptr->state != RR_PS_STATE_MAC_NO_ACCESS)
                {
                  MSG_GERAN_HIGH_2_G("Correcting RR-PS-ACCESS-CONTROL state (%d->%d)",
                           (int)rr_ps_access_data_ptr->state,(int)RR_PS_STATE_MAC_NO_ACCESS);

                  rr_ps_access_data_ptr->state = RR_PS_STATE_MAC_NO_ACCESS;
                }
              }
              break;
            }

            default: ;
          }
          break;
        } /* RR_PS_IMSG */

        default: ;
      }
      break;
    } /* MS_RR_RR */

    case MS_MAC_RR:
    {
      switch (msg_ptr->message_header.message_id)
      {
         case MAC_GRR_MAC_STATUS:
         {
           if (msg_ptr->mac_grr_sig.mac_grr_mac_status.mac_status == MAC_IDLE)
           {
             rr_ps_access_data_ptr->del_all_confirmed = rr_mac_ps_access_blocked_del_all();
           }
           break;
         }

         default: ;
      }
      break;
    } /* MS_MAC_RR */

    default: ;
  }

  // Now perform normal message processing per state

  switch (rr_ps_access_data_ptr->state)
  {
    case RR_PS_STATE_MAC_NO_ACCESS:
    {
      switch (msg_ptr->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg_ptr->rr.header.rr_message_set)
          {
            case RR_PS_IMSG:
            {
              switch (msg_ptr->message_header.message_id)
              {
                case RR_PS_IMSG_PS_ACCESS_REQ:
                {
                  const rr_ps_imsg_ps_access_req_t *imsg = &msg_ptr->rr.ps.imsg.ps_access_req;

                  rr_send_grr_mac_ps_access_req(imsg->purge_gmm_signalling);
                  rr_ps_access_data_ptr->state = RR_PS_STATE_ENABLING_MAC_ACCESS;
                  break;
                }

                case RR_PS_IMSG_NO_PS_ACCESS_REQ:
                {
                  const rr_ps_imsg_no_ps_access_req_t *imsg = &msg_ptr->rr.ps.imsg.no_ps_access_req;

                  if (imsg->suspend_gmm)
                  {
                    rr_gmm_suspend_gprs();
                  }

                  // PS access already blocked, so send confirm
                  rr_ps_send_omsg_no_ps_access_cnf();
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(msg_ptr, "MAC_NO_ACCESS");
                }
              }
              break;
            } /* RR_PS_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(msg_ptr, "MAC_NO_ACCESS");
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_MAC_RR:
        {
          break;  // ignore other MAC messages (most likely MAC_GRR_MAC_STATUS)
        } /* MS_MAC_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(msg_ptr, "MAC_NO_ACCESS");
        }
      }
      break;
    } /* RR_PS_STATE_MAC_NO_ACCESS */

    case RR_PS_STATE_MAC_ACCESS:
    {
      switch (msg_ptr->message_header.message_set)
      {
        case MS_RR_RR:
        {
          switch (msg_ptr->rr.header.rr_message_set)
          {
            case RR_PS_IMSG:
            {
              switch (msg_ptr->message_header.message_id)
              {
                case RR_PS_IMSG_PS_ACCESS_REQ:
                {
                  // PS access already granted, so send confirm
                  rr_ps_send_omsg_ps_access_cnf();
                  break;
                }

                case RR_PS_IMSG_NO_PS_ACCESS_REQ:
                {
                  const rr_ps_imsg_no_ps_access_req_t *imsg = &msg_ptr->rr.ps.imsg.no_ps_access_req;

                  if (imsg->suspend_gmm)
                  {
                    rr_gmm_suspend_gprs();
                  }

                  rr_send_grr_mac_no_ps_access_ind(imsg->delete_pdu);
                  rr_ps_access_data_ptr->state = RR_PS_STATE_DISABLING_MAC_ACCESS;
                  break;
                }

                default:
                {
                  LOG_UNEXPECTED_MESSAGE(msg_ptr, "MAC_ACCESS");
                }
              }
              break;
            } /* RR_PS_IMSG */

            default:
            {
              LOG_UNEXPECTED_MESSAGE(msg_ptr, "MAC_ACCESS");
            }
          }
          break;
        } /* MS_RR_RR */

        case MS_MAC_RR:
        {
          break;  // ignore other MAC messages (most likely MAC_GRR_MAC_STATUS)
        } /* MS_MAC_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(msg_ptr, "MAC_ACCESS");
        }
      }
      break;
    } /* RR_PS_STATE_MAC_ACCESS */

    case RR_PS_STATE_ENABLING_MAC_ACCESS:
    {
      switch (msg_ptr->message_header.message_set)
      {
        case MS_MAC_RR:
        {
          switch (msg_ptr->message_header.message_id)
          {
            case MAC_GRR_PS_ACCESS_CNF:
            {
              rr_ps_send_omsg_ps_access_cnf();
              rr_ps_access_data_ptr->state = RR_PS_STATE_MAC_ACCESS;
              break;
            }

            case MAC_GRR_MAC_STATUS:
            {
              break;  // no action
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(msg_ptr, "ENABLING_MAC_ACCESS");
            }
          }
          break;
        } /* MS_MAC_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(msg_ptr, "ENABLING_MAC_ACCESS");
        }
      }
      break;
    } /* RR_PS_STATE_ENABLING_MAC_ACCESS */

    case RR_PS_STATE_DISABLING_MAC_ACCESS:
    {
      switch (msg_ptr->message_header.message_set)
      {
        case MS_MAC_RR:
        {
          switch (msg_ptr->message_header.message_id)
          {
            case MAC_GRR_MAC_STATUS:
            {
              if (msg_ptr->mac_grr_sig.mac_grr_mac_status.mac_status == MAC_IDLE)
              {
                rr_ps_send_omsg_no_ps_access_cnf();
                rr_ps_access_data_ptr->state = RR_PS_STATE_MAC_NO_ACCESS;
              }
              break;
            }

            default:
            {
              LOG_UNEXPECTED_MESSAGE(msg_ptr, "DISABLING_MAC_ACCESS");
            }
          }
          break;
        } /* MS_MAC_RR */

        default:
        {
          LOG_UNEXPECTED_MESSAGE(msg_ptr, "DISABLING_MAC_ACCESS");
        }
      }
      break;
    } /* RR_PS_STATE_DISABLING_MAC_ACCESS */

    default:
    {
      ERR_GERAN_FATAL_1_G("Unknown state %d", rr_ps_access_data_ptr->state);
    }
  }

#ifdef DEBUG_RR_TRACE_MSG
  rr_store_trace_msg_buf(
    RR_PS_ACCESS_SM,
    EV_INPUT_MESSAGE,
    (byte) rr_ps_access_data_ptr->state,
    msg_ptr
  );
#endif

  if (rr_ps_access_data_ptr->state != rr_ps_access_data_ptr->old_state)
  {
    RR_LOG_STATE_TRANSITION(
      "rr_ps_access_control",
      rr_ps_access_data_ptr->old_state,
      rr_ps_access_data_ptr->state,
      rr_ps_access_state_name
    );

    rr_ps_access_data_ptr->old_state = rr_ps_access_data_ptr->state;
  }

} /* end rr_ps_access_control() */

/*!
 * \brief Initialisation function.
 *
 */
void rr_ps_access_init( )
{
  // Obtain a pointer to the module data
  rr_ps_access_data_t *rr_ps_access_data_ptr = rr_ps_access_get_data_ptr();
  //RR_NULL_CHECK_FATAL(rr_ps_access_data_ptr);

  rr_ps_access_data_ptr->state = RR_PS_STATE_MAC_NO_ACCESS;
  rr_ps_access_data_ptr->old_state = RR_PS_STATE_MAC_NO_ACCESS;
  rr_ps_access_data_ptr->del_all_confirmed = TRUE;

  reset_t3142_info();

} /* end rr_ps_access_init() */

/*!
 * \brief Initialisation function. Called once at task start.
 */
void rr_ps_access_task_start_init( void )
{
  // Zero the control/data structure(s)
  memset(&rr_ps_access_data, 0, sizeof(rr_ps_access_data));

  // Setup any specific values not setup by the general module initialisation function
  rr_ps_access_init();
}

/* EOF */

