#ifndef GRLC_DYN_API_H
#define GRLC_DYN_API_H

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "grlclog.h"
#include "geran_qmi.h"
#include "grlctimer.h"
#include "grlculfifo.h"
#include "grlcdlfifo.h"
#include "gl1rlccmn.h"
#include "grlcl1.h"
#include "gllcrlcif.h"
#include "grlcmac.h"

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/
extern void grlc_ul_one_time_init (void);
extern void grlc_dl_one_time_init (void);
extern void grlc_ul_timer_callback (uint32  timer_param);

/****************************************************************************/
extern void grlc_dl_start_t3192_dyn(void);
extern void grlc_dl_stop_t3192_dyn(void);
extern rm_timer_state_t grlc_dl_t3192_status_dyn(void);
extern grlc_dl_fifo_op_result_t grlc_dl_fifo_put_block_dyn(const pl1_radio_block_header_t* pl1_blk_hdr_ptr_ptr, dsm_item_type* pl1_blk_pyld_ptr_ptr);
extern grlc_dl_fifo_op_result_t e_grlc_dl_fifo_put_block_dyn(const e_pl1_radio_block_t* pl1_blk_ptr);
extern boolean grlc_is_rlc_dl_required_context_dyn(const uint8* data_block_ptr);
extern boolean e_grlc_is_rlc_dl_required_context_dyn(uint8 crc_1, uint8 crc_2, uint8 fbi_e_1, uint8 fbi_e_2, uint8 esp);
extern boolean grlc_is_xfer_active_dyn(void);
extern void grlc_ul_data_detect_dyn(boolean b);
extern void grlc_send_ph_data_ind_dyn(void);
extern void grlc_send_ph_ready_to_send_ind_dyn(void);
extern void grlc_service_block_timers_dyn(void);
extern pl1_rlc_msg_t* grlc_get_cmd_buf_pl1_dyn(void);
extern void grlc_put_cmd_pl1_dyn(pl1_rlc_msg_t* msg_ptr);
extern boolean e_grlc_dl_is_padding_removal_required_dyn(const uint8* hdr_ptr, uint8 mcs);
extern boolean e_grlc_dl_get_window_info_dyn(e_grlc_dl_window_info_t* info_ptr);
extern void grlc_dl_set_oper_win_size_dyn(uint16 oper_win_sz);
#ifdef ERLD_DUMP_12BYTES_PLD
extern void e_grlc_dl_copy_pld_for_dump_dyn(uint16* hdr_ptr, uint8* pld1_ptr, uint8* pld2_ptr);
#endif
extern uint8 grlc_ul_fifo_get_cnt_dyn(void);
extern uint8 grlc_ul_fifo_get_nack_cnt_dyn(void);
extern uint8 grlc_ul_fifo_get_ptx_cnt_dyn(void);
extern grlc_ul_fifo_op_result_t grlc_ul_fifo_access_block_dyn(grlc_ul_fifo_access_block_t* ff_ptr);
extern void grlc_llc_ul_put_mes_buf_dyn(gprs_grr_ul_mes_t* msg_ptr);
extern uint32 grlc_llc_get_current_octet_count_dyn(void);
extern mac_rlc_msg_t* grlc_get_cmd_buf_mac_dyn(void);
extern void grlc_put_cmd_mac_dyn(mac_rlc_msg_t* msg_ptr);
extern void grlc_dl_start_t3190_dyn(void);
extern boolean grlc_ul_is_cv_active_dyn(void);
extern void grlc_ul_restart_await_ass_guard_dyn(uint32 time_ms);
extern void grlc_ul_stop_await_ass_guard_dyn(void);

#if defined (DEBUG_GSM_GPRS_RLC_TEST) || defined (PERLUTF)
#error code not present
#else
extern void xlate_ext_mac_hdr_dyn(uint8* src, mac_hdr_t* mac_hdr);
#endif

extern void grlc_dl_handle_pl1_epdan_req_sig_dyn(void);
extern void grlc_dl_handle_pl1_sig_dyn(void);
extern void grlc_dl_handle_mac_sig_dyn(void);
extern void grlc_dl_handle_tmr_sig_dyn(void);

extern void grlc_ul_handle_pl1_sig_dyn(void);
extern void grlc_ul_handle_mac_sig_dyn(void);
extern void grlc_ul_handle_llc_msg_sig_dyn(void);
extern void grlc_ul_handle_llc_tmp_pdu_sig_dyn(void);
extern void grlc_ul_handle_llc_pdu_sig_dyn(void);
extern void grlc_ul_handle_tmr_sig_dyn(void);
extern void grlc_ul_handle_internal_sig_dyn(void);
#ifndef FEATURE_GERAN_REDUCED_DEBUG
extern rlc_log_rfblks_cnts_t* rlc_log_rfblks_cnts_dyn(void);
extern rlc_eng_mode_dl_cnts_t* rlc_log_dl_blks_cnts_dyn(void);
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
extern void geran_get_bearer_data_rates_dyn(geran_bearer_data_rate_t* p_data_rates);
extern void geran_get_bearer_data_rates_as_id_dyn(geran_bearer_data_rate_t* p_data_rates, sys_modem_as_id_e_type as_id);
#ifndef FEATURE_GERAN_REDUCED_DEBUG
extern log_on_demand_status_enum_type rlc_log_on_demand_dyn(log_code_type log_code);
extern void rlc_log_packet_request_dyn(uint16 log_code, uint8 zero_stats, uint8 additional_info);
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

#endif
