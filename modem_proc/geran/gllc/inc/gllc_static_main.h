#ifndef GLLC_STATIC_MAIN_H
#define GLLC_STATIC_MAIN_H

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "comdef.h"
#include "geran_tasks.h"
#include "gmutex.h"
#include "dsm.h"
#include "queue.h"
#include "rex.h"
#include "task.h"
#include "geran_multi_sim.h"
#include "gllci.h"
#include "gllclient.h"
#include "gllsap.h"
#include "gmmllcif.h"
#include "grlcllcif.h"
#include "gllc_v.h"
#include "gllc_v_g.h"
#include "gllclog.h"
#include "log.h"

/* LLC critical section to protect accesses to LLC messages and internal vars */
extern rex_crit_sect_type gllc_crit_sec;
extern rex_crit_sect_type gllc_ul_msg_crit_sec;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
typedef struct
{
  gllc_llme_t   gllc_llme_data;
  gllc_lle_t    gllc_lle_data[GLLC_LL_NUM_SAPIS];

  uint16        gllc_lle_ul_evt[GLLC_LL_NUM_SAPIS];
  uint16        gllc_lle_ul_mask[GLLC_LL_NUM_SAPIS];

  uint16        gllc_lle_ul_evt_flags;
  boolean       gllc_llme_ul_evt_mask_scheme[GLLC_LLME_NUM_SCHEMES];
  boolean       gllc_lle_ul_evt_mask_scheme[GLLC_LLE_NUM_SCHEMES][GLLC_LL_NUM_SAPIS];
}gllc_static_data_backup_t;

typedef struct
{
  /* To store T200 and T201 expiries for different SAPIs during suspend sate */
  uint32                     gllc_expired_timers[2 * GLLC_LL_NUM_SAPIS];
  uint8                      gllc_expired_timers_cnt;
  /* Tracks if GMM Ready Timer expired during unloaded state */
  boolean                    gllc_rdy_tmr_expired;
  gllc_static_data_backup_t* gllc_static_backup_ptr;
} gllc_static_data_t;

typedef enum
{
  GMM_LLGMM,
  GMM_LLE,
  GSMS_LLE
} gllc_ext_msg_type;

typedef struct
{
  q_link_type        link;
  void*              msg;
  gllc_ext_msg_type  msg_type;
} llc_ext_msg_t;
#endif

/***********************************************************************************/
typedef void (*gllc_test_mode_handler_t)(void);
typedef void (*gllc_gmm_ready_timer_handler_t)(void);
typedef void (*gllc_event_handler_t)(void);
typedef void (*gmmllc_llgmm_put_mes_buf_t)(gmmllc_llgmm_mes_type*);
typedef void (*gmmllc_ll_put_mes_t)(gmmllc_ll_mes_type*);
typedef void (*gsmsllc_ll_put_mes_t)(gsmsllc_ll_mes_type*);
typedef boolean (*gllclient_gea_classmark_enquiry_t)(gmmllc_gea_e_type);
typedef boolean (*gllclient_gea_classmark_enquiry_per_subs_t)(sys_modem_as_id_e_type, gmmllc_gea_e_type);
typedef gprs_ll_ul_sig_mes_t* (*gllc_ll_ul_sig_get_mes_buf_t)(void);
typedef void (*gllc_ll_ul_sig_put_mes_buf_t)(gprs_ll_ul_sig_mes_t*);
typedef void (*gllc_ll_ul_dat_put_mes_buf_t)(gprs_ll_ul_dat_mes_t*);
typedef gmmllc_gea_e_type (*gllc_llme_get_gea_algorithm_t)(void);
typedef void (*gllc_grr_ul_suspend_ind_t)(void);
typedef void (*gllc_grr_ul_resume_ind_t)(void);
typedef void (*gllc_grr_ul_L3L2_suspend_ind_t)(void);
typedef boolean (*gllc_grr_ul_tbf_rel_ind_t)(void);
typedef void (*gllc_grr_ul_transmit_ind_t)(uint8, uint8, ll_sndcp_ref_t*, grr_data_req_cause_t);
typedef void (*gllc_grr_dl_put_mes_buf_t)(gprs_grr_dl_mes_t*);
typedef uint32 (*gllc_llme_get_current_octet_count_t)(void);
#ifndef FEATURE_GERAN_REDUCED_DEBUG
typedef const gllc_diag_pdu_stat_rsp_t* (*gllc_diag_get_pdu_stats_t)(uint8);
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
#ifdef FEATURE_DSM_WM_CB
typedef void (*gllc_llme_clr_own_rcvr_busy_cond_t)(dsm_watermark_type*, void*);
typedef void (*gllc_llme_set_own_rcvr_busy_cond_t)(dsm_watermark_type*, void*);
#else
typedef void (*gllc_llme_clr_own_rcvr_busy_cond_t)(void);
typedef void (*gllc_llme_set_own_rcvr_busy_cond_t)(void);
#endif /* FEATURE_DSM_WM_CB */

#ifndef FEATURE_GERAN_REDUCED_DEBUG
typedef void (*llc_log_packet_request_t)(uint16, uint8, uint8);
typedef log_on_demand_status_enum_type (*llc_log_on_demand_t)(log_code_type);
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

/*****************************************************************************
*************************** Static API fptr struct****************************
*****************************************************************************/
typedef struct
{
  gllc_test_mode_handler_t                        gllc_test_mode_handler;
  gllc_gmm_ready_timer_handler_t                  gllc_gmm_ready_timer_handler;
  gllc_event_handler_t                            gllc_event_handler;
  gmmllc_llgmm_put_mes_buf_t                      gmmllc_llgmm_put_mes_buf;
  gmmllc_ll_put_mes_t                             gmmllc_ll_put_mes;
  gsmsllc_ll_put_mes_t                            gsmsllc_ll_put_mes;
  gllclient_gea_classmark_enquiry_t               gllclient_gea_classmark_enquiry;
  gllclient_gea_classmark_enquiry_per_subs_t      gllclient_gea_classmark_enquiry_per_subs;
  gllc_ll_ul_sig_get_mes_buf_t                    gllc_ll_ul_sig_get_mes_buf;
  gllc_ll_ul_sig_put_mes_buf_t                    gllc_ll_ul_sig_put_mes_buf;
  gllc_ll_ul_dat_put_mes_buf_t                    gllc_ll_ul_dat_put_mes_buf;
  gllc_llme_get_gea_algorithm_t                   gllc_llme_get_gea_algorithm;
  gllc_grr_ul_suspend_ind_t                       gllc_grr_ul_suspend_ind;
  gllc_grr_ul_resume_ind_t                        gllc_grr_ul_resume_ind;
  gllc_grr_ul_L3L2_suspend_ind_t                  gllc_grr_ul_L3L2_suspend_ind;
  gllc_grr_ul_tbf_rel_ind_t                       gllc_grr_ul_tbf_rel_ind;
  gllc_grr_ul_transmit_ind_t                      gllc_grr_ul_transmit_ind;
  gllc_grr_dl_put_mes_buf_t                       gllc_grr_dl_put_mes_buf;
  gllc_llme_get_current_octet_count_t             gllc_llme_get_current_octet_count;
#ifndef FEATURE_GERAN_REDUCED_DEBUG
  gllc_diag_get_pdu_stats_t                       gllc_diag_get_pdu_stats;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
  gllc_llme_clr_own_rcvr_busy_cond_t              gllc_llme_clr_own_rcvr_busy_cond;
  gllc_llme_set_own_rcvr_busy_cond_t              gllc_llme_set_own_rcvr_busy_cond;
#ifndef FEATURE_GERAN_REDUCED_DEBUG
  llc_log_packet_request_t                        llc_log_packet_request;
  llc_log_on_demand_t                             llc_log_on_demand;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
}geran_static_gllc_ext_api_s;

extern geran_static_gllc_ext_api_s gllc_vtbl;

extern void gllc_static_timer_callback(uint32 lle_and_timer_evt);
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
extern gllc_static_data_t* gllc_static_data_get_ptr(void);
extern void gllc_send_to_dyn_pendq(void* msg_ptr, gllc_ext_msg_type type);
extern void gllc_clear_dyn_pendq();
#endif

#endif
