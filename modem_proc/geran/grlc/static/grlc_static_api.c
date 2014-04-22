/*!
  @file grlc_static_api.c

  @brief
  External static API implementation for grlc task.

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

$Header: //components/rel/geran.mpss/5.2.0/grlc/static/grlc_static_api.c#5 $

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
#include "grlc_static_main.h"
#include "grlc_dyn_api.h"
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
#include "geran_dmm.h"
#endif

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

void grlc_dl_start_t3192(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_dl_start_t3192);
#else
  grlc_dl_start_t3192_dyn();
#endif
  return;
}

void grlc_dl_stop_t3192(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_dl_stop_t3192);
#else
  grlc_dl_stop_t3192_dyn();
#endif
  return;
}

rm_timer_state_t grlc_dl_t3192_status(void)
{
  rm_timer_state_t result = TMR_INACTIVE;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, result, grlc_dl_t3192_status);
#else
  result = grlc_dl_t3192_status_dyn();
#endif
  return result;
}

grlc_dl_fifo_op_result_t grlc_dl_fifo_put_block(const pl1_radio_block_header_t* pl1_blk_hdr_ptr_ptr, dsm_item_type* pl1_blk_pyld_ptr_ptr)
{
  grlc_dl_fifo_op_result_t result = DL_FIFO_FULL;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, result, grlc_dl_fifo_put_block, pl1_blk_hdr_ptr_ptr, pl1_blk_pyld_ptr_ptr);
#else
  result = grlc_dl_fifo_put_block_dyn(pl1_blk_hdr_ptr_ptr, pl1_blk_pyld_ptr_ptr);
#endif
  return result;
}

grlc_dl_fifo_op_result_t e_grlc_dl_fifo_put_block(const e_pl1_radio_block_t* pl1_blk_ptr)
{
  grlc_dl_fifo_op_result_t result = DL_FIFO_FULL;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, result, e_grlc_dl_fifo_put_block, pl1_blk_ptr);
#else
  result = e_grlc_dl_fifo_put_block_dyn(pl1_blk_ptr);
#endif
  return result;
}

boolean grlc_is_rlc_dl_required_context(const uint8* data_block_ptr)
{
  boolean result = FALSE;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, result, grlc_is_rlc_dl_required_context, data_block_ptr);
#else
  result = grlc_is_rlc_dl_required_context_dyn(data_block_ptr);
#endif
  return result;
}

boolean e_grlc_is_rlc_dl_required_context(uint8 crc_1, uint8 crc_2, uint8 fbi_e_1, uint8 fbi_e_2, uint8 esp)
{
  boolean result = FALSE;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, result, e_grlc_is_rlc_dl_required_context, crc_1, crc_2, fbi_e_1, fbi_e_2, esp);
#else
  result = e_grlc_is_rlc_dl_required_context_dyn(crc_1, crc_2, fbi_e_1, fbi_e_2, esp);
#endif
  return result;
}

boolean grlc_is_xfer_active(void)
{
  boolean result = FALSE;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, result, grlc_is_xfer_active);
#else
  result = grlc_is_xfer_active_dyn();
#endif
  return result;
}

void grlc_ul_data_detect(boolean b)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_ul_data_detect, b);
#else
  grlc_ul_data_detect_dyn(b);
#endif
  return;
}

void grlc_send_ph_data_ind(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_send_ph_data_ind);
#else
  grlc_send_ph_data_ind_dyn();
#endif
  return;
}

void grlc_send_ph_ready_to_send_ind(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_send_ph_ready_to_send_ind);
#else
  grlc_send_ph_ready_to_send_ind_dyn();
#endif
  return;
}

void grlc_service_block_timers(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_service_block_timers);
#else
  grlc_service_block_timers_dyn();
#endif
  return;
}

pl1_rlc_msg_t* grlc_get_cmd_buf_pl1(void)
{
  pl1_rlc_msg_t* result = NULL;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, result, grlc_get_cmd_buf_pl1);
#else
  result = grlc_get_cmd_buf_pl1_dyn();
#endif
  return result;
}

void grlc_put_cmd_pl1(pl1_rlc_msg_t* msg_ptr)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_put_cmd_pl1, msg_ptr);
#else
  grlc_put_cmd_pl1_dyn(msg_ptr);
#endif
  return;
}

boolean e_grlc_dl_is_padding_removal_required(const uint8* hdr_ptr, uint8 mcs)
{
  boolean result = FALSE;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, result, e_grlc_dl_is_padding_removal_required, hdr_ptr, mcs);
#else
  result = e_grlc_dl_is_padding_removal_required_dyn(hdr_ptr, mcs);
#endif
  return result;
}

boolean e_grlc_dl_get_window_info(e_grlc_dl_window_info_t* info_ptr)
{
  boolean result = FALSE;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, result, e_grlc_dl_get_window_info, info_ptr);
#else
  result = e_grlc_dl_get_window_info_dyn(info_ptr);
#endif
  return result;
}

void grlc_dl_set_oper_win_size(uint16 oper_win_sz)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_dl_set_oper_win_size, oper_win_sz);
#else
  grlc_dl_set_oper_win_size_dyn(oper_win_sz);
#endif
  return;
}

#ifdef ERLD_DUMP_12BYTES_PLD
void e_grlc_dl_copy_pld_for_dump(uint16* hdr_ptr, uint8* pld1_ptr, uint8* pld2_ptr)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, e_grlc_dl_copy_pld_for_dump, hdr_ptr, pld1_ptr, pld2_ptr);
#else
  e_grlc_dl_copy_pld_for_dump_dyn(hdr_ptr, pld1_ptr, pld2_ptr);
#endif
  return;
}
#endif

uint8 grlc_ul_fifo_get_cnt(void)
{
  uint8 result = 0;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, result, grlc_ul_fifo_get_cnt);
#else
  result = grlc_ul_fifo_get_cnt_dyn();
#endif
  return result;
}

uint8 grlc_ul_fifo_get_nack_cnt(void)
{
  uint8 result = 0;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, result, grlc_ul_fifo_get_nack_cnt);
#else
  result = grlc_ul_fifo_get_nack_cnt_dyn();
#endif
  return result;
}

uint8 grlc_ul_fifo_get_ptx_cnt(void)
{
  uint8 result = 0;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, result, grlc_ul_fifo_get_ptx_cnt);
#else
  result = grlc_ul_fifo_get_ptx_cnt_dyn();
#endif
  return result;
}

grlc_ul_fifo_op_result_t grlc_ul_fifo_access_block(grlc_ul_fifo_access_block_t* ff_ptr)
{
  grlc_ul_fifo_op_result_t result = UL_FIFO_BUSY;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, result, grlc_ul_fifo_access_block, ff_ptr);
#else
  result = grlc_ul_fifo_access_block_dyn(ff_ptr);
#endif
  return result;
}

void grlc_llc_ul_put_mes_buf(gprs_grr_ul_mes_t* msg_ptr)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_llc_ul_put_mes_buf, msg_ptr);
#else
  grlc_llc_ul_put_mes_buf_dyn(msg_ptr);
#endif
  return;
}

uint32 grlc_llc_get_current_octet_count(void)
{
  uint32 result = 0;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, result, grlc_llc_get_current_octet_count);
#else
  result = grlc_llc_get_current_octet_count_dyn();
#endif
  return result;
}

mac_rlc_msg_t* grlc_get_cmd_buf_mac(void)
{
  mac_rlc_msg_t* result = NULL;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, result, grlc_get_cmd_buf_mac);
#else
  result = grlc_get_cmd_buf_mac_dyn();
#endif
  return result;
}

void grlc_put_cmd_mac(mac_rlc_msg_t* msg_ptr)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_put_cmd_mac, msg_ptr);
#else
  grlc_put_cmd_mac_dyn(msg_ptr);
#endif
  return;
}

void grlc_dl_start_t3190(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_dl_start_t3190);
#else
  grlc_dl_start_t3190_dyn();
#endif
  return;
}

boolean grlc_ul_is_cv_active(void)
{
  boolean result = FALSE;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, result, grlc_ul_is_cv_active);
#else
  result = grlc_ul_is_cv_active_dyn();
#endif
  return result;
}

void grlc_ul_restart_await_ass_guard(uint32 time_ms)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_ul_restart_await_ass_guard, time_ms);
#else
  grlc_ul_restart_await_ass_guard_dyn(time_ms);
#endif
  return;
}

void grlc_ul_stop_await_ass_guard(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_ul_stop_await_ass_guard);
#else
  grlc_ul_stop_await_ass_guard_dyn();
#endif
  return;
}

#if defined (DEBUG_GSM_GPRS_RLC_TEST) || defined (PERLUTF)
#error code not present
#else
void xlate_ext_mac_hdr(uint8* src, mac_hdr_t* mac_hdr)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, xlate_ext_mac_hdr, src, mac_hdr);
#else
  xlate_ext_mac_hdr_dyn(src, mac_hdr);
#endif
  return;
}
#endif

void grlc_dl_handle_pl1_epdan_req_sig(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_dl_handle_pl1_epdan_req_sig);
#else
  grlc_dl_handle_pl1_epdan_req_sig_dyn();
#endif
  return;
}

void grlc_dl_handle_pl1_sig(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_dl_handle_pl1_sig);
#else
  grlc_dl_handle_pl1_sig_dyn();
#endif
  return;
}

void grlc_dl_handle_mac_sig(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_dl_handle_mac_sig);
#else
  grlc_dl_handle_mac_sig_dyn();
#endif
  return;
}

void grlc_dl_handle_tmr_sig(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_dl_handle_tmr_sig);
#else
  grlc_dl_handle_tmr_sig_dyn();
#endif
  return;
}

void grlc_ul_handle_pl1_sig(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_ul_handle_pl1_sig);
#else
  grlc_ul_handle_pl1_sig_dyn();
#endif
  return;
}

void grlc_ul_handle_mac_sig(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_ul_handle_mac_sig);
#else
  grlc_ul_handle_mac_sig_dyn();
#endif
  return;
}

void grlc_ul_handle_llc_msg_sig(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL_PRIV(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_ul_handle_llc_msg_sig);
#else
  grlc_ul_handle_llc_msg_sig_dyn();
#endif
  return;
}

void grlc_ul_handle_llc_tmp_pdu_sig(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_ul_handle_llc_tmp_pdu_sig);
#else
  grlc_ul_handle_llc_tmp_pdu_sig_dyn();
#endif
  return;
}

void grlc_ul_handle_llc_pdu_sig(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_ul_handle_llc_pdu_sig);
#else
  grlc_ul_handle_llc_pdu_sig_dyn();
#endif
  return;
}

void grlc_ul_handle_tmr_sig(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_ul_handle_tmr_sig);
#else
  grlc_ul_handle_tmr_sig_dyn();
#endif
  return;
}

void grlc_ul_handle_internal_sig(void)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, grlc_ul_handle_internal_sig);
#else
  grlc_ul_handle_internal_sig_dyn();
#endif
  return;
}

#ifndef FEATURE_GERAN_REDUCED_DEBUG
rlc_log_rfblks_cnts_t* rlc_log_rfblks_cnts(void)
{
  rlc_log_rfblks_cnts_t* result = NULL;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, result, rlc_log_rfblks_cnts);
#else
  result = rlc_log_rfblks_cnts_dyn();
#endif
  return result;
}

rlc_eng_mode_dl_cnts_t* rlc_log_dl_blks_cnts(void)
{
  rlc_eng_mode_dl_cnts_t* result = NULL;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, result, rlc_log_dl_blks_cnts);
#else
  result = rlc_log_dl_blks_cnts_dyn();
#endif
  return result;
}
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

geran_cost_of_modem_power_t geran_get_cost_of_modem_power(void)
{
  return NA_POWER;
}

void geran_get_bearer_data_rates(geran_bearer_data_rate_t* p_data_rates)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, geran_get_bearer_data_rates, p_data_rates);
#else
  geran_get_bearer_data_rates_dyn(p_data_rates);
#endif
  return;
}

void geran_get_bearer_data_rates_as_id(geran_bearer_data_rate_t* p_data_rates, sys_modem_as_id_e_type as_id)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, geran_get_bearer_data_rates_as_id, p_data_rates, as_id);
#else
  geran_get_bearer_data_rates_as_id_dyn(p_data_rates, as_id);
#endif
  return;
}

#ifndef FEATURE_GERAN_REDUCED_DEBUG
log_on_demand_status_enum_type rlc_log_on_demand(log_code_type log_code)
{
  log_on_demand_status_enum_type result = LOG_ON_DEMAND_FAILED_ATTEMPT_S;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_FUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, result, rlc_log_on_demand, log_code);
#else
  result = rlc_log_on_demand_dyn(log_code);
#endif
  return result;
}

void rlc_log_packet_request(uint16 log_code, uint8 zero_stats, uint8 additional_info)
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  DM_VOIDFUNC_CALL(GERAN_DMM_GRLC_INTF_ID, geran_static_grlc_ext_api_s, rlc_log_packet_request, log_code, zero_stats, additional_info);
#else
  rlc_log_packet_request_dyn(log_code, zero_stats, additional_info);
#endif
  return;
}
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */


