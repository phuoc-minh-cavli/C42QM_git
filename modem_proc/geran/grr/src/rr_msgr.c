/*============================================================================
  @file rr_msgr.c

  @brief This module provides a wrapper function to send a message via
         message router, and print the message name as a OMsg: debug string.

                Copyright (c) 2009-2014 QUALCOMM Technologies, Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_msgr.c#4 $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "amssassert.h"
#include "rr_seg_load.h" 

#include "rr_msgr.h"
#include "rr_defs.h"
#include "rr_gprs_debug.h"
#include "msgr.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/
static void rr_msgr_show_output_message(msgr_hdr_struct_type *msg_ptr  )
{
#ifdef FEATURE_GSM_BAND_AVOIDANCE
  if ((MCS_CXM_BAND_AVOID_BLIST_RSP == msg_ptr->id) ||
      (MCS_CXM_BAND_AVOID_FREQ_IND  == msg_ptr->id))
  {
    rr_show_output_message("MCS CXM", NULL_PTR, MS_MSGR_RR, msg_ptr->id);
  }
  else
#endif /* FEATURE_GSM_BAND_AVOIDANCE */
#ifdef FEATURE_IDLE_DRX_SCALING
  if (GERAN_GRR_MTC_CFG_RSP == msg_ptr->id)
  {
    rr_show_output_message("QMI NAS", NULL_PTR, MS_MSGR_RR, msg_ptr->id);
  }
  else
#endif /* FEATURE_IDLE_DRX_SCALING */
  {
    rr_show_output_message("LTE RRC", NULL_PTR, MS_MSGR_RR, msg_ptr->id);
    if (FALSE == geran_is_multi_mode_gas_id())
    {
      /* output debug if the mapping has given an unexpected result */
      MSG_GERAN_ERROR_0_G("Using MSGR for non-multimode subscription");
    }
  }

  return;
}

/*-----------------------------------------------------------------------------
 * Externalised Function Definitions
 *---------------------------------------------------------------------------*/

/**
  @brief Sends a message via MessageRouter.

  This function will pring a debug string indicating the message being sent,
  then send that message via MessageRouter.

  @param msg_ptr Pointer to message to be sent. Note that the first member of
         the message that is pointed to by this pointer must be of
         msgr_hdr_struct_type type.
  @param msg_len Total message length (header and payload) in bytes.
  @return E_BAD_ADDRESS if msg_ptr is NULL, otherwise the return value of
          rr_msgr_send is returned.
*/
errno_enum_type rr_msgr_send(
  /*! Pointer to message to be sent. Note that the first member of the message
   *  that is pointed to by this pointer must be of msgr_hdr_struct_type type
   */
  msgr_hdr_struct_type*      msg_ptr,
  /*! Total message length (header and payload) in bytes */
  uint32                     msg_len
  /*! Subscription identifier */
)
{
  
  
  //RR_NULL_CHECK_RETURN_PARAM(msg_ptr, E_BAD_ADDRESS);

  rr_msgr_show_output_message(msg_ptr);
#ifdef FEATURE_MSGR_RR_DEBUG
  rr_debug_log_msgr_rr_omsg(msg_ptr);
#endif
  /* output debug if the mapping has given an unexpected result */
  if (FALSE == geran_is_multi_mode_gas_id())
  {
    MSG_GERAN_LOW_0_G("MSGR use of ASID 0x001 for non-multimode subscription");
  }

  /* insert the instance ID, which is derived from the ASID */
  msgr_set_hdr_inst(msg_ptr, GERAN_ACCESS_STRATUM_ID_1);

  return msgr_send(msg_ptr, msg_len);
}

