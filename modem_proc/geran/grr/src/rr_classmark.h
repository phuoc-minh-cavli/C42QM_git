#ifndef RR_CLASSMARK_H
#define RR_CLASSMARK_H
/*============================================================================
 $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_classmark.h#2 $$DateTime: 2018/10/10 02:31:27 $$$
 $DateTime: 2018/10/10 02:31:27 $
 $Author: pwbldsvc $

 @file rr_classmark.h

 RR internal header file for RR-CLASSMARK module

 Copyright (c) 2008-2013 QUALCOMM Technologies, Inc
 All Rights Reserved.
 Qualcomm Confidential and Proprietary
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "comdef.h"
#include "sys_stru.h"
#ifdef FEATURE_LTE
#include "geran_grr_msg.h"
#endif /* FEATURE_LTE */
#include "geran_multi_sim.h"
#include "mm_rr.h"

/*----------------------------------------------------------------------------
 * Definitions and Constants
 * -------------------------------------------------------------------------*/

/* Classmark enquiry client enumeration */
typedef enum
{
  RR_CLASSMARK_CLIENT_NONE = 0,
  RR_CLASSMARK_CLIENT_ECSC,       /* request for early classmark sending */
  RR_CLASSMARK_CLIENT_NETW,       /* classmark enquiry from the network  */
  RR_CLASSMARK_CLIENT_GHDI        /* internal request from GHDI module   */
} rr_classmark_client_t;

/* Classmark encoding enumeration */
typedef enum
{
  RR_CLASSMARK_2_ONLY  = 2,
  RR_CLASSMARK_3_ONLY  = 3,
  RR_CLASSMARK_2_AND_3 = 4
} rr_classmark_encode_t;

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

extern boolean rr_encode_classmark(
  rr_classmark_encode_t,
  classmark_2_ie_T *,
  classmark_3_ie_T *
);

extern void rr_perform_classmark_sending(rr_classmark_client_t );

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_TDS)
extern void rr_classmark_set_irat_ho_info_compressed(boolean );
#endif /* FEATURE_WCDMA || FEATURE_GSM_TDS */

#ifdef FEATURE_LTE
void rr_handle_lte_cs_capabilities_req(
  geran_grr_lte_cs_capabilities_req_t *req_ptr );

void rr_handle_lte_ps_capabilities_req(
  geran_grr_lte_ps_capabilities_req_t *req_ptr );

void rr_send_lte_ps_capabilities_rsp(
  rr_gmm_gprs_ms_ra_cap_ie_ind_T *ind_ptr );
#endif /* FEATURE_LTE */


#endif /* #ifndef RR_CLASSMARK_H */

/* EOF */

