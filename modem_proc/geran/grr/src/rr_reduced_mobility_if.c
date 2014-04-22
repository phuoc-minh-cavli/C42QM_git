/*============================================================================
  FILE:         rr_resel_if.c

  OVERVIEW:     This module provides functions to send signals to/from the
                RR reselection module (i.e. RR_RESEL_IMSG, RR_RESEL_OMSG).

  DEPENDENCIES: None

                Copyright (c) 2002, 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_reduced_mobility_if.c#2 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "geran_multi_sim.h"
#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
#include "rr_seg_load.h"
#include "rr_reduced_mobility_if.h"
#include "rr_rr.h"

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

/*===========================================================================

FUNCTION      rr_rmc_imsg_name

DESCRIPTION   Outputs the message name.

DEPENDENCIES  None

RETURN VALUE Void

SIDE EFFECTS  None

===========================================================================*/

const char * rr_rmc_imsg_name(
  const rr_resel_rmc_imsg_e imsg
)
{
  switch (imsg)
  {
    case RR_RESEL_RMC_IMSG_START_IND:
      return "RR_RESEL_RMC_IMSG_START_IND";    

      case RR_RESEL_RMC_IMSG_CCCH_MSG_RCVD_IND:
      return "RR_RESEL_RMC_IMSG_CCCH_MSG_RCVD_IND";   

      case RR_RESEL_RMC_IMSG_CS_DATA_REQ:
      return "RR_RESEL_RMC_IMSG_CS_DATA_REQ";  

      case RR_RESEL_RMC_IMSG_PS_DATA_REQ:
      return "RR_RESEL_RMC_IMSG_PS_DATA_REQ";      

      case RR_RESEL_RMC_IMSG_BSI_ACQ_COMPLETE_IND:
      return "RR_RESEL_RMC_IMSG_BSI_ACQ_COMPLETE_IND";

    default:
      return "?";
  }
}

/*===========================================================================

FUNCTION      rr_rmc_omsg_name

DESCRIPTION   Outputs the OMSG message name.

DEPENDENCIES  None

RETURN VALUE Void

SIDE EFFECTS  None

===========================================================================*/

const char * rr_rmc_omsg_name(
  const rr_resel_rmc_omsg_e omsg
)
{
  switch (omsg)
  {    
    case RR_RESEL_RMC_OMSG_FIND_CELL_IND:
     return "RR_RESEL_RMC_OMSG_FIND_CELL_IND";

    case RR_RESEL_RMC_OMSG_CS_DATA_CNF:
     return "RR_RESEL_RMC_OMSG_CS_DATA_CNF";

    case RR_RESEL_RMC_OMSG_PS_DATA_CNF:
     return "RR_RESEL_RMC_OMSG_PS_DATA_CNF";

    default:
      return "?";
  }
}

/*===========================================================================

FUNCTION      rr_resel_rmc_send_imsg_start_ind

DESCRIPTION   Informs RMC about start of reduced mobility.

DEPENDENCIES  None

RETURN VALUE Void

SIDE EFFECTS  None

===========================================================================*/

void rr_resel_rmc_send_imsg_start_ind(
  rr_reduced_mobility_start_reason_e reason
   
)
{
  rr_resel_rmc_imsg_start_ind_t msg;
  memset(&msg, 0, sizeof(msg));

  msg.reason = reason;

  rr_rr_send(
    &msg.header,
    (uint8) RR_RESEL_RMC_IMSG_START_IND,
    sizeof(msg),
    RR_RESEL_RMC_IMSG
  );
}

/*===========================================================================

FUNCTION      rr_resel_rmc_send_imsg_ps_data_req

DESCRIPTION   Informs RMC about start of PS datay.

DEPENDENCIES  None

RETURN VALUE Void

SIDE EFFECTS  None

===========================================================================*/

void rr_resel_rmc_send_imsg_ps_data_req(
   
)
{
  rr_message_header_t header;
  rr_rr_send(
    &header,
    (uint8) RR_RESEL_RMC_IMSG_PS_DATA_REQ,
    sizeof(header),
    RR_RESEL_RMC_IMSG
  );
}

/*===========================================================================

FUNCTION      rr_resel_rmc_send_imsg_cs_data_req

DESCRIPTION   Informs RMC about start of CS datay.

DEPENDENCIES  None

RETURN VALUE Void

SIDE EFFECTS  None

===========================================================================*/

void rr_resel_rmc_send_imsg_cs_data_req(
   
)
{
  rr_message_header_t header;
  rr_rr_send(
    &header,
    (uint8) RR_RESEL_RMC_IMSG_CS_DATA_REQ,
    sizeof(header),
    RR_RESEL_RMC_IMSG
  );
}

/*===========================================================================

FUNCTION      rr_resel_rmc_send_imsg_ccch_msg_rcvd_ind

DESCRIPTION   Informs RMC about start of recpetion of CCCH message.

DEPENDENCIES  None

RETURN VALUE Void

SIDE EFFECTS  None

===========================================================================*/

void rr_resel_rmc_send_imsg_ccch_msg_rcvd_ind(
  boolean rcc_changed,
  boolean peo_bcch_changed
   
)
{

  rr_resel_rmc_imsg_ccch_msg_rcvd_ind_t msg;
  memset(&msg, 0, sizeof(msg));
 
  msg.rcc_changed = rcc_changed;
  msg.peo_bcch_changed = peo_bcch_changed;
  
  rr_rr_send(
    &msg.header,
    (uint8) RR_RESEL_RMC_IMSG_CCCH_MSG_RCVD_IND,
    sizeof(msg),
    RR_RESEL_RMC_IMSG
  );

}

/*===========================================================================

FUNCTION      rr_resel_rmc_send_imsg_bsi_acquition_complete_ind

DESCRIPTION   Informs RMC about completion of BSI acquition

DEPENDENCIES  None

RETURN VALUE Void

SIDE EFFECTS  None

===========================================================================*/

void rr_resel_rmc_send_imsg_bsi_acquition_complete_ind(
   
)
{
  rr_message_header_t header;
  rr_rr_send(
    &header,
    (uint8) RR_RESEL_RMC_IMSG_BSI_ACQ_COMPLETE_IND,
    sizeof(header),
    RR_RESEL_RMC_IMSG
  );

}


/*===========================================================================

FUNCTION      rr_resel_rmc_send_omsg_find_cell_ind

DESCRIPTION   Informs reselction module to find a cell

DEPENDENCIES  None

RETURN VALUE Void

SIDE EFFECTS  None

===========================================================================*/


void rr_resel_rmc_send_omsg_find_cell_ind(
   
)
{
  rr_message_header_t header;

  rr_rr_send(
    &header,
    (uint8) RR_RESEL_RMC_OMSG_FIND_CELL_IND,
    sizeof(header),
    RR_RESEL_RMC_OMSG
  );
}

void rr_resel_rmc_send_omsg_ps_data_cnf(
   
)
{
  rr_message_header_t header;

  rr_rr_send(
    &header,
    (uint8) RR_RESEL_RMC_OMSG_PS_DATA_CNF,
    sizeof(header),
    RR_RESEL_RMC_OMSG
  );
}

void rr_resel_rmc_send_omsg_cs_data_cnf(
   
)
{
  rr_message_header_t header;

  rr_rr_send(
    &header,
    (uint8) RR_RESEL_RMC_OMSG_CS_DATA_CNF,
    sizeof(header),
    RR_RESEL_RMC_OMSG
  );
}

#endif /*EATURE_GERAN_POWER_EFFICIENT_OPERATION*/

/* EOF */

