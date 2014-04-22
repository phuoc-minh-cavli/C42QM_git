#ifndef RR_CONN_RELEASE_H
#define RR_CONN_RELEASE_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               rr_conn_release.h

GENERAL DESCRIPTION
   This module contains functions for dealing with rr connection release.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_conn_release.h#3 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_GERAN_CS_SUPPORT
#include "rr_task.h"
#include "rr_events.h"
#include "geran_multi_sim.h"

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/


/*==============================================================================
  Constants and Macros
==============================================================================*/


/*==============================================================================
  Typedefs
==============================================================================*/

/* Reason for the connection being released */
typedef enum
{
  /* Initialised state */
  RELEASE_CAUSE_NONE,

  /* release was due to normal ChannelRelease from network */
  RELEASE_CAUSE_NETW_CHANNEL_RELEASE,

  /* release was due to an upper layer (NAS) request to release */
  RELEASE_CAUSE_NAS_REQUEST,

  /* release was due to an upper layer (NAS) request which requires a */
  /* release before it can be actioned */
  RELEASE_CAUSE_IMPLICIT_NAS_REQUEST,

  /* release was due to a radio link failure */
  RELEASE_CAUSE_RADIO_LINK_FAILURE,

  /* release was due to suspend indication */
  RELEASE_CAUSE_SUSPEND_IND,

  /* Release is due to T3174 timer expiry while awaiting L2 link establishment.
  Connection must be released before returning to the previous serving cell and
  sending a Packect Cell Change Failure message to the network. */
  RELEASE_CAUSE_PCCO_FAILURE,

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_TDS) || defined(FEATURE_LTE)
  /* Release is due to X2G CCO timer expiry while awaiting L2 link
  establishment.  Connection must be released before control may be returned
  to the initiating RAT. */
  RELEASE_CAUSE_X2G_CCO_FAILURE,
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS || FEATURE_LTE */

  /* the connection establishment failed */
  RELEASE_CAUSE_CONN_EST_FAILURE,

  /* release was due to the L2 releasing the link autonomously */
  RELEASE_CAUSE_L2_LINK_RELEASED,

  /* release was due to the L2 link being reset due to some procedure failure */
  RELEASE_CAUSE_L2_LINK_RESET,

  /* end of list - do not use */
  RELEASE_CAUSE_MAX

} rr_conn_release_cause_t;

#ifdef FEATURE_GSM_DTM
/* Release signalling scheme in use - GSM, DTM or Enhanced DTM */
typedef enum
{
   RELEASE_MODE_GSM
  ,RELEASE_MODE_DTM
#ifdef FEATURE_GSM_EDTM
  ,RELEASE_MODE_EDTM
#endif /* FEATURE_GSM_EDTM */
} rr_conn_release_mode_t;

#endif /* FEATURE_GSM_DTM */

/*==============================================================================
  Variables
==============================================================================*/

/*==============================================================================
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/**
 * Connection release state machine.
 */
extern rr_event_T rr_connection_release(
  rr_event_T release_event,
  rr_cmd_bdy_type *new_message
);

#ifdef FEATURE_GSM_EDTM
extern boolean rr_power_scan_on_edtm_release( );
#endif /* FEATURE_GSM_EDTM */

/**
 * Sets the release cause variable.  This is used to determine which signal
 * to send to MM when the connection is released (RR_ABORT_CNF, RR_ABORT_IND,
 * RR_REL_IND).
 *
 * @param release_cause Cause of RR connection release.
 */
extern void rr_conn_set_release_cause(
  rr_conn_release_cause_t release_cause
);

/**
 * Accesses the stored release cause.
 *
 * @return The stored release cause.
 */
extern rr_conn_release_cause_t rr_conn_get_release_cause( );

/**
 * Sends MM either RR_ABORT_CNF, RR_ABORT_IND or RR_REL_IND, depending on the
 * value of the stored release cause.
 *
 * Also, optionally, sends RR_SERVICE_IND, and if required RR_GPRS_PCCCH_CAMP_CNF.
 *
 * The signal sent to MM is determined using the following mapping of release
 * cause:
 *
 * RELEASE_CAUSE_NAS_REQUEST          -> RR_ABORT_CNF(MM_AS_SUCCESS)
 * RELEASE_CAUSE_IMPLICIT_NAS_REQUEST -> RR_ABORT_IND(OTHER_REASON)
 * RELEASE_CAUSE_NETW_CHANNEL_RELEASE -> RR_REL_IND(NORMAL_RELEASE)
 * Any other values                   -> RR_ABORT_IND(RR_CONNECTION_FAILURE)
 *
 * @param send_rr_service_ind TRUE if RR should send RR_SERVICE_IND/
 *        RR_GPRS_PCCCH_CAMP_CNF, FALSE otherwise.
 */
extern void rr_conn_inform_mm_of_channel_release(
  boolean send_rr_service_ind
);

#ifdef FEATURE_LTE
/**
  @brief Informs connection release state machine that on release of the
         connection a blind G2L redirection (non CSFB cases) should be performed.
*/
extern void rr_connection_release_set_g2l_blind_redir_flag( );

/**
  @brief Makes G2L blind redirection (non CSFB cases) disabled.
*/
extern void rr_connection_release_reset_g2l_blind_redir_flag( );

/**
  @brief Check whether G2L blind redirection (non CSFB cases) has been enabled.
*/
extern boolean rr_connection_release_get_g2l_blind_redir_flag( );

/**
  @brief Informs connection release state machine that on release of the
         connection a blind G2L redirection after CSFB should be performed.
*/
extern void rr_connection_release_set_g2l_blind_redir_after_csfb_flag( );

/**
  @brief Makes G2L blind redirection after CSFB disabled.
*/
extern void rr_connection_release_reset_g2l_blind_redir_after_csfb_flag( );

/**
  @brief Check whether G2L blind redirection after CSFB has been enabled.
*/
extern boolean rr_connection_release_get_g2l_blind_redir_after_csfb_flag( );
#endif /* FEATURE_LTE */

extern void rr_ce_rel_task_start_init(void);
#endif /* FEATURE_GERAN_CS_SUPPORT */

#endif /* RR_CONN_RELEASE_H */

