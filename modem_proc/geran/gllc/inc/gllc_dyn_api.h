#ifndef GLLC_DYN_API_H
#define GLLC_DYN_API_H

#include "gllclient.h"
#include "gllsap.h"
#include "gmmllcif.h"
#include "grlcllcif.h"
#include "gllc_v.h"
#include "gllc_v_g.h"
#include "gllclog.h"
#include "log.h"

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/
extern void gllc_test_mode_handler_dyn(void);
extern void gllc_gmm_ready_timer_handler_dyn(void);
extern void gllc_event_handler_dyn(void);
extern void gmmllc_llgmm_put_mes_buf_dyn(gmmllc_llgmm_mes_type *mes_ptr);
extern void gmmllc_ll_put_mes_dyn(gmmllc_ll_mes_type* mes_ptr);
extern void gsmsllc_ll_put_mes_dyn(gsmsllc_ll_mes_type* mes_ptr);
extern boolean gllclient_gea_classmark_enquiry_dyn(gmmllc_gea_e_type gea);
extern boolean gllclient_gea_classmark_enquiry_per_subs_dyn(sys_modem_as_id_e_type as_id, gmmllc_gea_e_type gea);
extern gprs_ll_ul_sig_mes_t* gllc_ll_ul_sig_get_mes_buf_dyn(void);
extern void gllc_ll_ul_sig_put_mes_buf_dyn(gprs_ll_ul_sig_mes_t* mes_ptr);
extern void gllc_ll_ul_dat_put_mes_buf_dyn(gprs_ll_ul_dat_mes_t* mes_ptr);
extern gmmllc_gea_e_type gllc_llme_get_gea_algorithm_dyn(void);
extern void gllc_grr_ul_suspend_ind_dyn(void);
extern void gllc_grr_ul_resume_ind_dyn(void);
extern void gllc_grr_ul_L3L2_suspend_ind_dyn(void);
extern boolean gllc_grr_ul_tbf_rel_ind_dyn(void);
extern void gllc_grr_ul_transmit_ind_dyn(uint8 sapi, uint8 reliability_class, ll_sndcp_ref_t* sndcp_ref, grr_data_req_cause_t cause);
extern void gllc_grr_dl_put_mes_buf_dyn(gprs_grr_dl_mes_t* mes_ptr);
extern uint32 gllc_llme_get_current_octet_count_dyn(void);
#ifndef FEATURE_GERAN_REDUCED_DEBUG
extern const gllc_diag_pdu_stat_rsp_t* gllc_diag_get_pdu_stats_dyn(uint8 sapi_addr);
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

#ifdef FEATURE_DSM_WM_CB
extern void gllc_llme_clr_own_rcvr_busy_cond_dyn(dsm_watermark_type* wm_p, void* context);
extern void gllc_llme_set_own_rcvr_busy_cond_dyn(dsm_watermark_type* wm_p, void* context);
#else
extern void gllc_llme_clr_own_rcvr_busy_cond_dyn(void);
extern void gllc_llme_set_own_rcvr_busy_cond_dyn(void);
#endif /* FEATURE_DSM_WM_CB */

#ifndef FEATURE_GERAN_REDUCED_DEBUG
extern void llc_log_packet_request_dyn(uint16 log_code, uint8 zero_stats, uint8 additional_info);
extern log_on_demand_status_enum_type llc_log_on_demand_dyn(log_code_type log_code);
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

#endif
