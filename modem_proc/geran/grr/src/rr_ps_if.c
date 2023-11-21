/*! \file rr_ps_if.c 
 
  Signalling interface for the RR-PS modules.
 
                Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_ps_if.c#2 $ */
/* $DateTime: 2018/10/10 02:31:27 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_ps_if.h"
#include "msg.h"
#include "rr_rr.h"
#include "geran_dual_sim.h"

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

/*!
 * \brief Returns a text string representing 'imsg'.
 * 
 * \param imsg 
 * 
 * \return const char* 
 */
const char *rr_ps_imsg_name(
  rr_ps_imsg_e imsg
)
{
  switch (imsg)
  {
    case RR_PS_IMSG_PS_ACCESS_REQ:
    {
      return "RR_PS_IMSG_PS_ACCESS_REQ";
    }

    case RR_PS_IMSG_NO_PS_ACCESS_REQ:
    {
      return "RR_PS_IMSG_NO_PS_ACCESS_REQ";
    }

    default:
    {
      return "?";
    }
  }
}

/*!
 * \brief Sends RR_PS_IMSG_PS_ACCESS_REQ.
 * 
 * \param purge_gmm_signalling - TRUE if GMM PDUs should be deleted
 */
void rr_ps_send_imsg_ps_access_req(
  boolean purge_gmm_signalling
   
)
{
  rr_ps_imsg_ps_access_req_t msg;

  MSG_GERAN_HIGH_1_G("RR_PS_IMSG_PS_ACCESS_REQ: purge_gmm=%d", purge_gmm_signalling);

  msg.purge_gmm_signalling = purge_gmm_signalling;

  rr_rr_send(
    &msg.header,
    (uint8) RR_PS_IMSG_PS_ACCESS_REQ,
    sizeof(msg),
    RR_PS_IMSG
  );
}

/*!
 * \brief Sends RR_PS_IMSG_NO_PS_ACCESS_REQ.
 * 
 * \param suspend_gmm - TRUE if GMM should be suspended
 * \param delete_pdu - PDU deletion action {NO_DEL, DEL_ONE, DEL_ALL}
 */
void rr_ps_send_imsg_no_ps_access_req(
  boolean suspend_gmm,
  delete_pdu_cause_t delete_pdu
   
)
{
  rr_ps_imsg_no_ps_access_req_t msg;

  MSG_GERAN_HIGH_2_G("RR_PS_IMSG_NO_PS_ACCESS_REQ: suspend_gmm=%d delete_pdu=%d", suspend_gmm, delete_pdu);

  msg.suspend_gmm = suspend_gmm;
  msg.delete_pdu = delete_pdu;

  rr_rr_send(
    &msg.header,
    (uint8) RR_PS_IMSG_NO_PS_ACCESS_REQ,
    sizeof(msg),
    RR_PS_IMSG
  );
}

/*!
 * \brief Returns a text string representing 'omsg'.
 * 
 * \param imsg 
 * 
 * \return const char* 
 */
const char *rr_ps_omsg_name(
  rr_ps_omsg_e omsg
)
{
  switch (omsg)
  {
    case RR_PS_OMSG_PS_ACCESS_CNF:
    {
      return "RR_PS_OMSG_PS_ACCESS_CNF";
    }
    
    case RR_PS_OMSG_NO_PS_ACCESS_CNF:
    {
      return "RR_PS_OMSG_NO_PS_ACCESS_CNF";
    }

    default:
    {
      return "?";
    }
  }
}

/*!
 * \brief Sends RR_OMSG_PS_ACCESS_CNF.
 * 
 */
void rr_ps_send_omsg_ps_access_cnf(
   
)
{
  rr_ps_omsg_ps_access_cnf_t msg;

  rr_rr_send(
    &msg.header,
    (uint8) RR_PS_OMSG_PS_ACCESS_CNF,
    sizeof(msg),
    RR_PS_OMSG
  );
}

/*!
 * \brief Sends RR_OMSG_NO_PS_ACCESS_CNF.
 * 
 */
void rr_ps_send_omsg_no_ps_access_cnf(
   
)
{
  rr_ps_omsg_no_ps_access_cnf_t msg;

  rr_rr_send(
    &msg.header,
    (uint8) RR_PS_OMSG_NO_PS_ACCESS_CNF,
    sizeof(msg),
    RR_PS_OMSG
  );
}


/* EOF */

