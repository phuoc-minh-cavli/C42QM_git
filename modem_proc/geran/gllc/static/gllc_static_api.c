/*!
  @file gllc_static_api.c

  @brief
  External static API implementation for GLLC task.

  @detail

*/

/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gllc/static/gllc_static_api.c#8 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
24/10/17   ns      Initial Revision

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "sys.h"
#include "geran_dual_sim.h"
#include "gprs_mem.h"

#include "gllclient.h"
#include "gllsap.h"
#include "gmmllcif.h"
#include "grlcllcif.h"
#include "gllc_static_main.h"
#include "gllc_dyn_api.h"
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
#include "geran_dmm.h"
#endif

/*****************************************************************************
***
***     EXTERNAL DEFINITIONS AND TYPES
***
*****************************************************************************/

/* Default parameters which are used in static context */
const uint16 gllc_dflt_N201_I_octets[ GLLC_LL_NUM_SAPIS ] =
{
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_1_GMM   */
  1503,                   /* GLLC_LL_SAPI_3_LL3   */
  1503,                   /* GLLC_LL_SAPI_5_LL5   */
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_7_SMS   */
  1503,                   /* GLLC_LL_SAPI_9_LL9   */
  1503                    /* GLLC_LL_SAPI_11_LL11 */
};

const uint16 gllc_dflt_N201_U_octets[ GLLC_LL_NUM_SAPIS ] =
{
  400,                    /* GLLC_LL_SAPI_1_GMM   */
  500,                    /* GLLC_LL_SAPI_3_LL3   */
  500,                    /* GLLC_LL_SAPI_5_LL5   */
  270,                    /* GLLC_LL_SAPI_7_SMS   */
  500,                    /* GLLC_LL_SAPI_9_LL9   */
  500                     /* GLLC_LL_SAPI_11_LL11 */
};

/*===========================================================================

                        DYNAMIC Loadable wrapper APIs

===========================================================================*/

void gllc_test_mode_handler(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, gllc_test_mode_handler);
#else
  gllc_test_mode_handler_dyn();
#endif
}

void gllc_gmm_ready_timer_handler(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, gllc_gmm_ready_timer_handler);
#else
  gllc_gmm_ready_timer_handler_dyn();
#endif
}

void gllc_event_handler(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, gllc_event_handler);
#else
  gllc_event_handler_dyn();
#endif
}

gmmllc_llgmm_mes_type* gmmllc_llgmm_get_mes_buf(void)
{
  return (GPRS_MEM_MALLOC(sizeof(gmmllc_llgmm_mes_type) / sizeof(uint8)));
}

void gmmllc_llgmm_put_mes_buf(gmmllc_llgmm_mes_type* mes_ptr)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  if (is_geran_loaded() == FALSE)
  {
    gllc_send_to_dyn_pendq(mes_ptr, GMM_LLGMM);
  }
  else
  {
    DM_VOIDFUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, gmmllc_llgmm_put_mes_buf, mes_ptr);
  }
#else
  gmmllc_llgmm_put_mes_buf_dyn(mes_ptr);
#endif
}

gmmllc_ll_mes_type* gmmllc_ll_get_mes_buf(void)
{
  return (GPRS_MEM_MALLOC(sizeof(gmmllc_ll_mes_type) / sizeof(uint8)));
}

void gmmllc_ll_put_mes(gmmllc_ll_mes_type* mes_ptr)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  if (is_geran_loaded() == FALSE)
  {
    gllc_send_to_dyn_pendq(mes_ptr, GMM_LLE);
  }
  else
  {
    DM_VOIDFUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, gmmllc_ll_put_mes, mes_ptr);
  }
#else
  gmmllc_ll_put_mes_dyn(mes_ptr);
#endif
}

gsmsllc_ll_mes_type* gsmsllc_ll_get_mes_buf(void)
{
  return (GPRS_MEM_MALLOC(sizeof(gsmsllc_ll_mes_type) / sizeof(uint8)));
}

void gsmsllc_ll_put_mes(gsmsllc_ll_mes_type* mes_ptr)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  if (is_geran_loaded() == FALSE)
  {
    gllc_send_to_dyn_pendq(mes_ptr, GSMS_LLE);
  }
  else
  {
    DM_VOIDFUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, gsmsllc_ll_put_mes, mes_ptr);
  }
#else
  gsmsllc_ll_put_mes_dyn(mes_ptr);
#endif
}

/*===========================================================================
===
===  FUNCTION      GLLC_LL_GET_DEFAULT_N201_I()
===
===  DESCRIPTION
===
===   For a complete description refer to interface gllc.h.
===
===========================================================================*/

uint16 gllc_ll_get_default_N201_I(uint8 sapi)
{
  if ( sapi >= 16 || gllc_lle_lookup[sapi] == GLLC_LL_SAPI_RESERVED)
  {
    MSG_GERAN_ERROR_1("GLC LL: Get Dflt N201-I: Invalid SAPI %d", sapi);
    return (0);
  }
  else
    return ( gllc_dflt_N201_I_octets[ gllc_lle_lookup[sapi] ] );
} /* end of gprs_ll_get_default_N201_I() */

/*===========================================================================
===
===  FUNCTION      GLLC_LL_GET_DEFAULT_N201_U()
===
===  DESCRIPTION
===
===   For a complete description refer to interface gllc.h.
===
===========================================================================*/

uint16 gllc_ll_get_default_N201_U(uint8 sapi)
{
  if ( sapi >= 16 || gllc_lle_lookup[sapi] == GLLC_LL_SAPI_RESERVED)
  {
    MSG_GERAN_ERROR_1("GLC LL: Get Dflt N201-U: Invalid SAPI %d", sapi);
    return (0);
  }
  else
    return ( gllc_dflt_N201_U_octets[ gllc_lle_lookup[sapi] ] );
} /* end of gprs_ll_get_default_N201_U() */

boolean gllclient_gea_classmark_enquiry(gmmllc_gea_e_type gea)
{
  boolean result = FALSE;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, result, gllclient_gea_classmark_enquiry, gea);
#else
  result = gllclient_gea_classmark_enquiry_dyn(gea);
#endif
  return result;
}

boolean gllclient_gea_classmark_enquiry_per_subs(sys_modem_as_id_e_type as_id, gmmllc_gea_e_type gea)
{
  boolean result = FALSE;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, result, gllclient_gea_classmark_enquiry_per_subs, as_id, gea);
#else
  result = gllclient_gea_classmark_enquiry_per_subs_dyn(as_id, gea);
#endif
  return result;
}

gprs_ll_ul_sig_mes_t* gllc_ll_ul_sig_get_mes_buf(void)
{
  gprs_ll_ul_sig_mes_t* result = NULL;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, result, gllc_ll_ul_sig_get_mes_buf);
#else
  result = gllc_ll_ul_sig_get_mes_buf_dyn();
#endif
  return result;
}

void gllc_ll_ul_sig_put_mes_buf(gprs_ll_ul_sig_mes_t* mes_ptr)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, gllc_ll_ul_sig_put_mes_buf, mes_ptr);
#else
  gllc_ll_ul_sig_put_mes_buf_dyn(mes_ptr);
#endif
}

void gllc_ll_ul_dat_put_mes_buf(gprs_ll_ul_dat_mes_t* mes_ptr)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, gllc_ll_ul_dat_put_mes_buf, mes_ptr);
#else
  gllc_ll_ul_dat_put_mes_buf_dyn(mes_ptr);
#endif
}

gmmllc_gea_e_type gllc_llme_get_gea_algorithm(void)
{
  gmmllc_gea_e_type result = GMMLLC_GEA_NONE;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, result, gllc_llme_get_gea_algorithm);
#else
  result = gllc_llme_get_gea_algorithm_dyn();
#endif
  return result;
}

void gllc_grr_ul_suspend_ind(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, gllc_grr_ul_suspend_ind);
#else
  gllc_grr_ul_suspend_ind_dyn();
#endif
}

void gllc_grr_ul_resume_ind(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, gllc_grr_ul_resume_ind);
#else
  gllc_grr_ul_resume_ind_dyn();
#endif
}

void gllc_grr_ul_L3L2_suspend_ind(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, gllc_grr_ul_L3L2_suspend_ind);
#else
  gllc_grr_ul_L3L2_suspend_ind_dyn();
#endif
}

boolean gllc_grr_ul_tbf_rel_ind(void)
{
  boolean result = FALSE;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, result, gllc_grr_ul_tbf_rel_ind);
#else
  result = gllc_grr_ul_tbf_rel_ind_dyn();
#endif
  return result;
}

void gllc_grr_ul_transmit_ind(uint8 sapi, uint8 reliability_class, ll_sndcp_ref_t* sndcp_ref, grr_data_req_cause_t cause)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, gllc_grr_ul_transmit_ind, sapi, reliability_class, sndcp_ref, cause);
#else
  gllc_grr_ul_transmit_ind_dyn(sapi, reliability_class, sndcp_ref, cause);
#endif
}

void gllc_grr_dl_put_mes_buf(gprs_grr_dl_mes_t* mes_ptr)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, gllc_grr_dl_put_mes_buf, mes_ptr);
#else
  gllc_grr_dl_put_mes_buf_dyn(mes_ptr);
#endif
}

uint32 gllc_llme_get_current_octet_count(void)
{
  uint32 result = 0;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, result, gllc_llme_get_current_octet_count);
#else
  result = gllc_llme_get_current_octet_count_dyn();
#endif
  return result;
}

#ifndef FEATURE_GERAN_REDUCED_DEBUG
const gllc_diag_pdu_stat_rsp_t* gllc_diag_get_pdu_stats(uint8 sapi_addr)
{
  const gllc_diag_pdu_stat_rsp_t* result = NULL;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, result, gllc_diag_get_pdu_stats, sapi_addr);
#else
  result = gllc_diag_get_pdu_stats_dyn(sapi_addr);
#endif
  return result;
}
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

#ifdef FEATURE_DSM_WM_CB
void gllc_llme_clr_own_rcvr_busy_cond(dsm_watermark_type* wm_p, void* context)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, gllc_llme_clr_own_rcvr_busy_cond, wm_p, context);
#else
  gllc_llme_clr_own_rcvr_busy_cond_dyn(wm_p, context);
#endif
}

void gllc_llme_set_own_rcvr_busy_cond(dsm_watermark_type* wm_p, void* context)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, gllc_llme_set_own_rcvr_busy_cond, wm_p, context);
#else
  gllc_llme_set_own_rcvr_busy_cond_dyn(wm_p, context);
#endif
}
#else
void gllc_llme_clr_own_rcvr_busy_cond(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, gllc_llme_clr_own_rcvr_busy_cond);
#else
  gllc_llme_clr_own_rcvr_busy_cond_dyn();
#endif
}

void gllc_llme_set_own_rcvr_busy_cond(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, gllc_llme_set_own_rcvr_busy_cond);
#else
  gllc_llme_set_own_rcvr_busy_cond_dyn();
#endif
}
#endif /* FEATURE_DSM_WM_CB */

uint16 gllc_ll_get_max_N201_U(uint8 sapi)
{
  if (sapi >= 16 || gllc_lle_lookup[sapi] == GLLC_LL_SAPI_RESERVED)
  {
    MSG_ERROR( "GLC LL: Get Max N201-U: Invalid SAPI %d", sapi, 0, 0);
    return (0);
  }
  else
    return (gllc_max_N201_U_octets[gllc_lle_lookup[sapi]]);
}

#ifndef FEATURE_GERAN_REDUCED_DEBUG
void llc_log_packet_request(uint16 log_code, uint8 zero_stats, uint8 additional_info)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, llc_log_packet_request, log_code, zero_stats, additional_info);
#else
  llc_log_packet_request_dyn(log_code, zero_stats, additional_info);
#endif
}

log_on_demand_status_enum_type llc_log_on_demand(log_code_type log_code)
{
  log_on_demand_status_enum_type result = LOG_ON_DEMAND_FAILED_ATTEMPT_S;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GLLC_INTF_ID, geran_static_gllc_ext_api_s, result, llc_log_on_demand, log_code);
#else
  result = llc_log_on_demand_dyn(log_code);
#endif
  return result;
}
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */


