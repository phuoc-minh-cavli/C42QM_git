/*============================================================================
  rr_l1_idle_mode_if.c

  This file implements the signalling interface to the RR-L1-IDLE module.

                Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_l1_idle_mode_if.c#3 $ */
/* $DateTime: 2018/10/10 08:50:54 $ */

/**
  @file rr_l1_idle_mode_if.c
  @brief This file implements the signalling interface to the RR-L1-IDLE module
 */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "rr_seg_load.h"
#include "rr_l1_idle_mode_if.h"
#include "rr_rr.h"
#include "msg.h"
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

void rr_l1_idle_send_imsg_start_early_camp_req(
  ARFCN_T arfcn,
  BSIC_T bsic,
  LAI_T lai,
  word cell_identity,
  control_channel_description_T control_channel_descr,
  boolean control_channel_descr_valid
   
)
{
  rr_l1_idle_imsg_start_early_camp_req_t msg;

  msg.arfcn = arfcn;
  msg.bsic = bsic;
  msg.lai = lai;
  msg.cell_identity = cell_identity;
  msg.control_channel_descr = control_channel_descr;
  msg.control_channel_descr_valid = control_channel_descr_valid;

  rr_rr_send(
    &msg.header,
    (uint8) RR_L1_IDLE_IMSG_START_EARLY_CAMP_REQ,
    sizeof(msg),
    RR_L1_IDLE_IMSG
  );
}

void rr_l1_idle_send_imsg_start_idle_req(
   
)
{
  rr_l1_idle_imsg_start_idle_req_t msg;

  rr_rr_send(
    &msg.header,
    (uint8) RR_L1_IDLE_IMSG_START_IDLE_REQ,
    sizeof(msg),
    RR_L1_IDLE_IMSG
  );
}

void rr_l1_idle_send_imsg_change_idle_req(
   
)
{
  rr_l1_idle_imsg_change_idle_req_t msg;

  rr_rr_send(
    &msg.header,
    (uint8) RR_L1_IDLE_IMSG_CHANGE_IDLE_REQ,
    sizeof(msg),
    RR_L1_IDLE_IMSG
  );
}

const char *rr_l1_idle_imsg_name(
  rr_l1_idle_imsg_e msg
)
{
  switch (msg)
  {
    case RR_L1_IDLE_IMSG_START_EARLY_CAMP_REQ:
    {
      return "RR_L1_IDLE_IMSG_START_EARLY_CAMP_REQ";
    }

    case RR_L1_IDLE_IMSG_START_IDLE_REQ:
    {
      return "RR_L1_IDLE_IMSG_START_IDLE_REQ";
    }

    case RR_L1_IDLE_IMSG_CHANGE_IDLE_REQ:
    {
      return "RR_L1_IDLE_IMSG_CHANGE_IDLE_REQ";
    }

    default:
    {
      MSG_GERAN_LOW_1("Unknown message: %d", msg);
      return "?";
    }
  }
}

void rr_l1_idle_send_omsg_start_early_camp_cnf(
   
)
{
  rr_l1_idle_omsg_start_early_camp_cnf_t msg;

  rr_rr_send(
    &msg.header,
    (uint8) RR_L1_IDLE_OMSG_START_EARLY_CAMP_CNF,
    sizeof(msg),
    RR_L1_IDLE_OMSG
  );
}

void rr_l1_idle_send_omsg_start_idle_cnf(
  boolean gprs_allowed
   
)
{
  rr_l1_idle_omsg_start_idle_cnf_t msg;

  msg.gprs_allowed = gprs_allowed;

  rr_rr_send(
    &msg.header,
    (uint8) RR_L1_IDLE_OMSG_START_IDLE_CNF,
    sizeof(msg),
    RR_L1_IDLE_OMSG
  );
}

const char *rr_l1_idle_omsg_name(
  rr_l1_idle_omsg_e msg
)
{
  switch (msg)
  {
    case RR_L1_IDLE_OMSG_START_EARLY_CAMP_CNF:
    {
      return "RR_L1_IDLE_OMSG_START_EARLY_CAMP_CNF";
    }

    case RR_L1_IDLE_OMSG_START_IDLE_CNF:
    {
      return "RR_L1_IDLE_OMSG_START_IDLE_CNF";
    }

    default:
    {
      MSG_GERAN_LOW_1("Unknown message: %d", msg);
      return "?";
    }
  }
}

/* EOF */

