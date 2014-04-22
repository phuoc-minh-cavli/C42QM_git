/*============================================================================
  FILE:         rr_g2x_redir_if.c

  OVERVIEW:

  DEPENDENCIES: None

                Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_g2x_redir_if.c#2 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"

#if defined(FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE) || defined(FEATURE_LTE) || defined(FEATURE_GSM_TDS_G2T_REDIRECTION)

#include "comdef.h"
#include "rr_g2x_redir_if.h"
#include "rr_rr.h"
#include "rr_multi_sim.h"
#include "gprs_mem.h"


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
* Externalized Function Definitions
* -------------------------------------------------------------------------*/

const char * rr_g2x_redir_imsg_name(rr_g2x_redir_imsg_e imsg)
{
  switch (imsg)
  {
    case RR_G2X_REDIR_IMSG_REDIR_REQ:
      return "RR_G2X_REDIR_IMSG_REDIR_REQ";

    case RR_G2X_REDIR_IMSG_ABORT_REQ:
      return "RR_G2X_REDIR_IMSG_ABORT_REQ";

    case RR_G2X_REDIR_IMSG_L1_DEACTIVATED_IND:
      return "RR_G2X_REDIR_IMSG_L1_DEACTIVATED_IND";

    default:
      MSG_GERAN_ERROR_1("Unknown message: %d", imsg);
      return "?";
  }
}

const char * rr_g2x_redir_omsg_name(rr_g2x_redir_omsg_e omsg)
{
  switch (omsg)
  {
    case RR_G2X_REDIR_OMSG_REDIR_CNF:
      return "RR_G2X_REDIR_OMSG_REDIR_CNF";

    case RR_G2X_REDIR_OMSG_ABORT_CNF:
      return "RR_G2X_REDIR_OMSG_ABORT_CNF";

#ifdef FEATURE_SGLTE
    case RR_G2X_REDIR_OMSG_SGLTE_ABORT_CNF:
      return "RR_G2X_REDIR_OMSG_SGLTE_ABORT_CNF";
#endif /*FEATURE_SGLTE*/

    default:
      MSG_GERAN_ERROR_1("Unknown message: %d", omsg);
      return "?";
  }
}

void rr_g2x_redir_send_imsg(rr_g2x_redir_imsg_e imsg,  )
{
  rr_message_header_t header;

  rr_rr_send(&header,
             (uint8) imsg,
             sizeof(header),
             RR_G2X_REDIR_IMSG);
}

void rr_g2x_redir_send_imsg_redir_req(
  rr_chan_rel_cell_selection_indicator_t * csi
   
)
{
  rr_g2x_redir_imsg_redir_req_t *redir_req_ptr;

  RR_NULL_CHECK_RETURN_VOID(csi);
  
  redir_req_ptr = (rr_g2x_redir_imsg_redir_req_t *)GPRS_MEM_CALLOC(sizeof(rr_g2x_redir_imsg_redir_req_t));
  
  RR_NULL_CHECK_RETURN_VOID(redir_req_ptr );

  redir_req_ptr->csi = *csi;
 
  rr_rr_send(&redir_req_ptr->header,
             (uint8) RR_G2X_REDIR_IMSG_REDIR_REQ,
             sizeof(rr_g2x_redir_imsg_redir_req_t),
             RR_G2X_REDIR_IMSG);

  GPRS_MEM_FREE(redir_req_ptr);

  
}

void rr_g2x_redir_send_omsg(rr_g2x_redir_omsg_e omsg  )
{
  rr_message_header_t header;

  rr_rr_send(&header,
             (uint8) omsg,
             sizeof(header),
             RR_G2X_REDIR_OMSG);
}

void rr_g2x_redir_send_omsg_redir_cnf(
  rr_g2x_redir_status_e         status,
  rr_g2x_redir_failure_action_e failure_action
 )
{
  rr_g2x_redir_omsg_redir_cnf_t redir_cnf;

  memset(&redir_cnf, 0, sizeof(redir_cnf));

  redir_cnf.status = status;
  redir_cnf.failure_action = failure_action;

  rr_rr_send(&redir_cnf.header,
             (uint8) RR_G2X_REDIR_OMSG_REDIR_CNF,
             sizeof(redir_cnf),
             RR_G2X_REDIR_OMSG);
}

#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE || FEATURE_LTE || FEATURE_GSM_TDS_G2T_REDIRECTION */

/* EOF */

