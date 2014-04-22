#ifndef GRLC_STATIC_MAIN_H
#define GRLC_STATIC_MAIN_H

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "comdef.h"
#include "geran_tasks.h"
#include "grlctimer.h"
#include "grlculfifo.h"
#include "grlcdlfifo.h"
#include "gl1rlccmn.h"
#include "grlcl1.h"
#include "gllcrlcif.h"
#include "grlcmac.h"
#include "grlclog.h"
#include "geran_qmi.h"

/*------------------------------
** UL task and DL task
**------------------------------
**
*/
extern rex_sigs_type  grlc_ul_task_sigs;
extern rex_sigs_type  grlc_dl_task_sigs;

extern boolean rlc_log_initialized;

extern void grlc_static_ul_timer_callback(uint32  timer_param);
extern void grlc_dl_handle_pl1_epdan_req_sig(void);
extern void grlc_dl_handle_pl1_sig(void);
extern void grlc_dl_handle_mac_sig(void);
extern void grlc_dl_handle_tmr_sig (void);

extern void grlc_ul_handle_pl1_sig (void);
extern void grlc_ul_handle_mac_sig (void);
extern void grlc_ul_handle_llc_msg_sig (void);
extern void grlc_ul_handle_llc_tmp_pdu_sig (void);
extern void grlc_ul_handle_llc_pdu_sig (void);
extern void grlc_ul_handle_tmr_sig (void);
extern void grlc_ul_handle_internal_sig (void);

/**********************************************************************************/
typedef void (*grlc_dl_start_t3192_t)(void);
typedef void (*grlc_dl_stop_t3192_t)(void);
typedef rm_timer_state_t (*grlc_dl_t3192_status_t)(void);
typedef grlc_dl_fifo_op_result_t (*grlc_dl_fifo_put_block_t)(const pl1_radio_block_header_t*, dsm_item_type*);
typedef grlc_dl_fifo_op_result_t (*e_grlc_dl_fifo_put_block_t)(const e_pl1_radio_block_t*);
typedef boolean (*grlc_is_rlc_dl_required_context_t)(const uint8*);
typedef boolean (*e_grlc_is_rlc_dl_required_context_t)(uint8, uint8, uint8, uint8, uint8);
typedef boolean (*grlc_is_xfer_active_t)(void);
typedef void (*grlc_ul_data_detect_t)(boolean);
typedef void (*grlc_send_ph_data_ind_t)(void);
typedef void (*grlc_send_ph_ready_to_send_ind_t)(void);
typedef void (*grlc_service_block_timers_t)(void);
typedef pl1_rlc_msg_t* (*grlc_get_cmd_buf_pl1_t)(void);
typedef void (*grlc_put_cmd_pl1_t)( pl1_rlc_msg_t*);
typedef boolean (*e_grlc_dl_is_padding_removal_required_t)(const uint8*, uint8);
typedef boolean (*e_grlc_dl_get_window_info_t)(e_grlc_dl_window_info_t*);
typedef void (*grlc_dl_set_oper_win_size_t)(uint16);
#ifdef ERLD_DUMP_12BYTES_PLD
typedef void (*e_grlc_dl_copy_pld_for_dump_t)(uint16*, uint8*, uint8*);
#endif
typedef uint8 (*grlc_ul_fifo_get_cnt_t)(void);
typedef uint8 (*grlc_ul_fifo_get_nack_cnt_t)(void);
typedef uint8 (*grlc_ul_fifo_get_ptx_cnt_t)(void);
typedef grlc_ul_fifo_op_result_t (*grlc_ul_fifo_access_block_fptr_t)(grlc_ul_fifo_access_block_t*);
typedef void (*grlc_llc_ul_put_mes_buf_t)(gprs_grr_ul_mes_t*);
typedef uint32 (*grlc_llc_get_current_octet_count_t)(void);
typedef mac_rlc_msg_t* (*grlc_get_cmd_buf_mac_t)(void);
typedef void (*grlc_put_cmd_mac_t)(mac_rlc_msg_t* msg_ptr);
typedef void (*grlc_dl_start_t3190_t)(void);
typedef boolean (*grlc_ul_is_cv_active_t)(void);
typedef void (*grlc_ul_restart_await_ass_guard_t)(uint32);
typedef void (*grlc_ul_stop_await_ass_guard_t)(void);

#if defined (DEBUG_GSM_GPRS_RLC_TEST) || defined (PERLUTF)
#error code not present
#else
typedef void (*xlate_ext_mac_hdr_t)(uint8*, mac_hdr_t*);
#endif

typedef void (*grlc_dl_handle_pl1_epdan_req_sig_t)(void);
typedef void (*grlc_dl_handle_pl1_sig_t)(void);
typedef void (*grlc_dl_handle_mac_sig_t)(void);
typedef void (*grlc_dl_handle_tmr_sig_t)(void);

typedef void (*grlc_ul_handle_pl1_sig_t)(void);
typedef void (*grlc_ul_handle_mac_sig_t)(void);
typedef void (*grlc_ul_handle_llc_msg_sig_t)(void);
typedef void (*grlc_ul_handle_llc_tmp_pdu_sig_t)(void);
typedef void (*grlc_ul_handle_llc_pdu_sig_t)(void);
typedef void (*grlc_ul_handle_tmr_sig_t)(void);
typedef void (*grlc_ul_handle_internal_sig_t)(void);
#ifndef FEATURE_GERAN_REDUCED_DEBUG
typedef rlc_log_rfblks_cnts_t* (*rlc_log_rfblks_cnts_fptr_t)(void);
typedef rlc_eng_mode_dl_cnts_t* (*rlc_log_dl_blks_cnts_fptr_t)(void);;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
typedef void (*geran_get_bearer_data_rates_t)(geran_bearer_data_rate_t*);
typedef void (*geran_get_bearer_data_rates_as_id_t)(geran_bearer_data_rate_t*, sys_modem_as_id_e_type);
typedef log_on_demand_status_enum_type (*rlc_log_on_demand_t)(log_code_type);
typedef void (*rlc_log_packet_request_t)(uint16, uint8, uint8);

/**********************************************************************************/
/* Static API fptr struct */
typedef struct
{
  grlc_dl_start_t3192_t                      grlc_dl_start_t3192;
  grlc_dl_stop_t3192_t                       grlc_dl_stop_t3192;
  grlc_dl_t3192_status_t                     grlc_dl_t3192_status;
  grlc_dl_fifo_put_block_t                   grlc_dl_fifo_put_block;
  e_grlc_dl_fifo_put_block_t                 e_grlc_dl_fifo_put_block;
  grlc_is_rlc_dl_required_context_t          grlc_is_rlc_dl_required_context;
  e_grlc_is_rlc_dl_required_context_t        e_grlc_is_rlc_dl_required_context;
  grlc_is_xfer_active_t                      grlc_is_xfer_active;
  grlc_ul_data_detect_t                      grlc_ul_data_detect;
  grlc_send_ph_data_ind_t                    grlc_send_ph_data_ind;
  grlc_send_ph_ready_to_send_ind_t           grlc_send_ph_ready_to_send_ind;
  grlc_service_block_timers_t                grlc_service_block_timers;
  grlc_get_cmd_buf_pl1_t                     grlc_get_cmd_buf_pl1;
  grlc_put_cmd_pl1_t                         grlc_put_cmd_pl1;
  e_grlc_dl_is_padding_removal_required_t    e_grlc_dl_is_padding_removal_required;
  e_grlc_dl_get_window_info_t                e_grlc_dl_get_window_info;
  grlc_dl_set_oper_win_size_t                grlc_dl_set_oper_win_size;
#ifdef ERLD_DUMP_12BYTES_PLD
  e_grlc_dl_copy_pld_for_dump_t              e_grlc_dl_copy_pld_for_dump;
#endif
  grlc_ul_fifo_get_cnt_t                     grlc_ul_fifo_get_cnt;
  grlc_ul_fifo_get_nack_cnt_t                grlc_ul_fifo_get_nack_cnt;
  grlc_ul_fifo_get_ptx_cnt_t                 grlc_ul_fifo_get_ptx_cnt;
  grlc_ul_fifo_access_block_fptr_t           grlc_ul_fifo_access_block;
  grlc_llc_ul_put_mes_buf_t                  grlc_llc_ul_put_mes_buf;
  grlc_llc_get_current_octet_count_t         grlc_llc_get_current_octet_count;
  grlc_get_cmd_buf_mac_t                     grlc_get_cmd_buf_mac;
  grlc_put_cmd_mac_t                         grlc_put_cmd_mac;
  grlc_dl_start_t3190_t                      grlc_dl_start_t3190;
  grlc_ul_is_cv_active_t                     grlc_ul_is_cv_active;
  grlc_ul_restart_await_ass_guard_t          grlc_ul_restart_await_ass_guard;
  grlc_ul_stop_await_ass_guard_t             grlc_ul_stop_await_ass_guard;
  xlate_ext_mac_hdr_t                        xlate_ext_mac_hdr;
  grlc_dl_handle_pl1_epdan_req_sig_t         grlc_dl_handle_pl1_epdan_req_sig;
  grlc_dl_handle_pl1_sig_t                   grlc_dl_handle_pl1_sig;
  grlc_dl_handle_mac_sig_t                   grlc_dl_handle_mac_sig;
  grlc_dl_handle_tmr_sig_t                   grlc_dl_handle_tmr_sig;
  grlc_ul_handle_pl1_sig_t                   grlc_ul_handle_pl1_sig;
  grlc_ul_handle_mac_sig_t                   grlc_ul_handle_mac_sig;
  grlc_ul_handle_llc_msg_sig_t               grlc_ul_handle_llc_msg_sig;
  grlc_ul_handle_llc_tmp_pdu_sig_t           grlc_ul_handle_llc_tmp_pdu_sig;
  grlc_ul_handle_llc_pdu_sig_t               grlc_ul_handle_llc_pdu_sig;
  grlc_ul_handle_tmr_sig_t                   grlc_ul_handle_tmr_sig;
  grlc_ul_handle_internal_sig_t              grlc_ul_handle_internal_sig;
#ifndef FEATURE_GERAN_REDUCED_DEBUG
  rlc_log_rfblks_cnts_fptr_t                 rlc_log_rfblks_cnts;
  rlc_log_dl_blks_cnts_fptr_t                rlc_log_dl_blks_cnts;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
  geran_get_bearer_data_rates_t              geran_get_bearer_data_rates;
  geran_get_bearer_data_rates_as_id_t        geran_get_bearer_data_rates_as_id;
#ifndef FEATURE_GERAN_REDUCED_DEBUG
  rlc_log_on_demand_t                        rlc_log_on_demand;
  rlc_log_packet_request_t                   rlc_log_packet_request;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
}geran_static_grlc_ext_api_s;

/**********************************************************************************/
extern geran_static_grlc_ext_api_s grlc_vtbl;

#endif
