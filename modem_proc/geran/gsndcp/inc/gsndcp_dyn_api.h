#ifndef GSNDCP_DYN_API_H
#define GSNDCP_DYN_API_H

#include "gsni.h"
#include "gsndcplog.h"

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/
extern void gsndcp_snsm_put_mes_dyn(gsmsn_mes_type* mes_ptr);
extern void gsn_int_put_mes_dyn(gsn_internal_msg_t* mes_ptr);
#ifndef FEATURE_GERAN_REDUCED_DEBUG
extern const gsndcp_diag_pdu_stat_rsp_t* gsndcp_diag_get_pdu_stats_dyn(uint8 nsapi_addr);
extern void gsndcp_diag_reset_pdu_stats_dyn(uint8 nsapi_addr);
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
extern uint32 gsndcp_follow_on_req_enquiry_dyn(void);

#ifdef FEATURE_DATA_MM
extern boolean gsndcp_process_cmd_queue_dyn(comp_sig_enum_type sig, void* pkt_instance);
extern boolean gsndcp_process_ll_dl_queue_dyn(comp_sig_enum_type sig, void* pkt_instance);
#else
extern boolean gsndcp_process_cmd_queue_dyn(void);
extern boolean gsndcp_process_ll_dl_queue_dyn(void);
#endif /* FEATURE_DATA_MM */

extern void gsndcp_to_sndcp_each_enqueue_cb_dyn(dsm_watermark_type* wm, void* callback_data);
extern boolean gsndcp_to_sndcp_sig_hdlr_dyn(comp_sig_enum_type sig, void* user_data_ptr);
extern void gsn_ul_clr_est_susp_dyn(uint8 sapi);
extern void gsn_timer_callback_dyn(uint32 gasid_nsapi_and_timer_id);
#ifndef FEATURE_GERAN_REDUCED_DEBUG
extern log_on_demand_status_enum_type sndcp_log_on_demand_dyn(log_code_type log_code);
extern void sndcp_log_packet_request_dyn(uint16 log_code, uint8 zero_stats, uint8 additional_info);
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

#endif
