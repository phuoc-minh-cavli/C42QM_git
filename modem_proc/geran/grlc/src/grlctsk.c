/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC LAYER 2 TASK CONTROL FUNCTIONS
***
***
*** DESCRIPTION
***
***  Provides a REX framework for the scheduling of RLC Uplink Task and
***  RLC Downlink Task processing.
***
***
*** EXTERNALIZED FUNCTIONS
***
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //components/rel/geran.mpss/5.2.0/grlc/src/grlctsk.c#10 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 03/15/01    hv     Defined RLC UL & DL task control functions
*** 08/03/04    hv     Added PL1_EPDAN_REQ signal and handler for EGPRS
*** 03/08/12    hv     Corrected priority of uplink message handler
*** 09/18/12    hv     Added dual data space functionality
*** 03/05/13    hv     CR462174
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "comdef.h"
#include "geran_tasks.h"
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "grlctsk.h"
#include "grlcutil.h"
#include "queue.h"
#include "grlcul.h"
#include "grlcultypes.h"
#include "grlculproc.h"
#include "grlcdl.h"
#include "grlclog.h"
#include "grlc_dyn_api.h"
#include "grlc_static_main.h"

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/

/*--------------------------------------------------------------------------
** Temporary declaration of system information.
** This structure will be created and maintained by modules other than RLC.
** To be removed at integration time
**--------------------------------------------------------------------------
*/
gprs_sysinfo_t  sysinfo;

/* GRLC vtable to map static API fptrs with dynamic APIs */
geran_static_grlc_ext_api_s grlc_vtbl =
{
  grlc_dl_start_t3192_dyn,
  grlc_dl_stop_t3192_dyn,
  grlc_dl_t3192_status_dyn,
  grlc_dl_fifo_put_block_dyn,
  e_grlc_dl_fifo_put_block_dyn,
  grlc_is_rlc_dl_required_context_dyn,
  e_grlc_is_rlc_dl_required_context_dyn,
  grlc_is_xfer_active_dyn,
  grlc_ul_data_detect_dyn,
  grlc_send_ph_data_ind_dyn,
  grlc_send_ph_ready_to_send_ind_dyn,
  grlc_service_block_timers_dyn,
  grlc_get_cmd_buf_pl1_dyn,
  grlc_put_cmd_pl1_dyn,
  e_grlc_dl_is_padding_removal_required_dyn,
  e_grlc_dl_get_window_info_dyn,
  grlc_dl_set_oper_win_size_dyn,
#ifdef ERLD_DUMP_12BYTES_PLD
  e_grlc_dl_copy_pld_for_dump_dyn,
#endif
  grlc_ul_fifo_get_cnt_dyn,
  grlc_ul_fifo_get_nack_cnt_dyn,
  grlc_ul_fifo_get_ptx_cnt_dyn,
  grlc_ul_fifo_access_block_dyn,
  grlc_llc_ul_put_mes_buf_dyn,
  grlc_llc_get_current_octet_count_dyn,
  grlc_get_cmd_buf_mac_dyn,
  grlc_put_cmd_mac_dyn,
  grlc_dl_start_t3190_dyn,
  grlc_ul_is_cv_active_dyn,
  grlc_ul_restart_await_ass_guard_dyn,
  grlc_ul_stop_await_ass_guard_dyn,
  xlate_ext_mac_hdr_dyn,
  grlc_dl_handle_pl1_epdan_req_sig_dyn,
  grlc_dl_handle_pl1_sig_dyn,
  grlc_dl_handle_mac_sig_dyn,
  grlc_dl_handle_tmr_sig_dyn,
  grlc_ul_handle_pl1_sig_dyn,
  grlc_ul_handle_mac_sig_dyn,
  grlc_ul_handle_llc_msg_sig_dyn,
  grlc_ul_handle_llc_tmp_pdu_sig_dyn,
  grlc_ul_handle_llc_pdu_sig_dyn,
  grlc_ul_handle_tmr_sig_dyn,
  grlc_ul_handle_internal_sig_dyn,
#ifndef FEATURE_GERAN_REDUCED_DEBUG
  rlc_log_rfblks_cnts_dyn,
  rlc_log_dl_blks_cnts_dyn,
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
  geran_get_bearer_data_rates_dyn,
  geran_get_bearer_data_rates_as_id_dyn,
#ifndef FEATURE_GERAN_REDUCED_DEBUG
  rlc_log_on_demand_dyn,
  rlc_log_packet_request_dyn
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
};

void grlc_dl_init(void)
{
  MSG_GERAN_HIGH_0("DMMSUSREM:: grlc_dl_init");
  grlc_dl_one_time_init();
  grlc_dl_init_vars();
}

void grlc_ul_init(void)
{
  MSG_GERAN_HIGH_0("DMMSUSREM:: grlc_ul_init");
  grlc_ul_one_time_init();
  grlc_ul_init_vars();
#ifndef FEATURE_GERAN_REDUCED_DEBUG
  rlc_log_init();
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
}

void geran_grlc_init()
{
  MSG_GERAN_HIGH_1("DMMSUSREM:: geran_grlc_init %d", rlc_log_initialized);
  grlc_ul_init();
  grlc_dl_init();
}

void geran_grlc_deinit()
{
  MSG_GERAN_HIGH_0("DMMSUSREM:: geran_grlc_deinit");

  grlc_ul_free_tx_array();
  grlc_dl_free_rx_array();

  /* Undefine all the general purpose (signal sent on expiry) timers */
  rex_undef_timer(&(ul.timer_mac_ul_resp.rex_timer));
  rex_undef_timer(&(ul.timer_t3182.rex_timer));
  rex_undef_timer(&(ul.timer_await_ul_acknack.rex_timer));
  rex_undef_timer(&(ul.timer_await_ass_guard.rex_timer));
  rex_undef_timer(&(dl.timer_t3190.rex_timer));
  rex_undef_timer(&(dl.timer_t3192.rex_timer));
}

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
void geran_grlc_resume()
{
  MSG_GERAN_HIGH_0("GRLC dynamic resume");
  RLC_UL_SET_DYN_RESUME_SIG();
}
void geran_grlc_suspend()
{
  MSG_GERAN_HIGH_0("GRLC dynamic suspend");
}
#endif

/*** EOF: don't remove! ***/

