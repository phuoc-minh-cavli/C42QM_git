#ifndef L1_STATIC_API_H
#define L1_STATIC_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          L 1   S T A T I C   A P I

GENERAL DESCRIPTION
   This header file contains prototypes of static APIs 
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2015 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gl1/static/inc/l1_static_api.h#7 $
$DateTime: 2019/01/01 22:24:35 $ $Author: pwbldsvc $


when       who       what, where, why
--------   ---       ---------------------------------------------------------
09/08/17   sk        CR2294063 Adding a wrapper for the external fucntion gsm_call
07/08/18   shm       CR2274892 | Instantaneous WWAN timing and frequency information (rude wakeup)
09/01/18   hd        CR2167422 FR 46686: Support GERAN as dynamic module using DMM/External paging framework 
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_dual_sim.h"
#include "geran_eng_mode_read_api.h"
#include "l1_task.h"
#include "log.h" 

extern void geran_gl1_init();
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
extern void geran_gl1_deinit();
extern void geran_gl1_suspend();
extern void geran_gl1_resume();
#endif /* FEATURE_EXTERNAL_PAGING_GERAN */


typedef void (*l1_handle_message_t)(l1_queue_message_T *);
typedef void (*l1_handle_msg_gas_id_t)(void);
typedef uint8 (*l1_log_get_TxPa_state_t)(void);
typedef void (*l1_log_signal_info_t)(geran_eng_mode_signal_info_t *);
typedef void (*l1_log_convert_txlev_to_pwr_dBm_dyn_t)(eng_mode_sys_band_T, uint8, uint8 *);
typedef boolean (*l1_ftm_mode_enabled_t)(void);
typedef void (*l1_get_usf_utilisation_info_t)(l1_usf_utilisation_info_t*);
typedef void (*disable_timer_handler_t)( void );
typedef void (*l1_isr_femto_timeout_handler_callback_t) (const uint32);
typedef log_on_demand_status_enum_type (*l1_log_on_demand_t)(log_code_type);
typedef void (*gts_timer_handler_t)(void);
typedef uint32 (*gsm_call_t)( uint32, void*, uint32 );

typedef struct 
{
  l1_handle_message_t                       handle_msg;
  l1_handle_msg_gas_id_t                    handle_rlc_ph_data_req;
  l1_handle_msg_gas_id_t                    serving_cell_meas_if;
  l1_handle_msg_gas_id_t                    report_watchdog;
  l1_handle_msg_gas_id_t                    watchdog_check;
#if defined (FEATURE_GSM_TO_LTE) || defined (FEATURE_GSM_TDS)
  l1_handle_msg_gas_id_t                    handle_msgr_msgs;
#endif
#if defined(FEATURE_GSM_COEX_SW_CXM)
  l1_handle_msg_gas_id_t                    handle_cxm_msgr_msgs;
#endif
  l1_log_get_TxPa_state_t                   log_get_TxPa_state;
  l1_log_signal_info_t                      log_signal_info;
  l1_log_convert_txlev_to_pwr_dBm_dyn_t     log_convert_txlev_to_pwr_dBm;
  l1_ftm_mode_enabled_t                     ftm_mode_enabled;
  l1_get_usf_utilisation_info_t             get_usf_utilisation_info;
  disable_timer_handler_t                   saic_disable_timer_handler;
#if defined ( FEATURE_DUAL_SIM ) || defined ( FEATURE_TRIPLE_SIM )
  disable_timer_handler_t                   rach_disable_timer_handler;
#endif
#if defined ( FEATURE_WCDMA ) || defined ( FEATURE_GSM_TDS )|| defined ( FEATURE_GSM_TO_LTE )
  l1_isr_femto_timeout_handler_callback_t   femto_timeout_handler;

#endif
  disable_timer_handler_t                   ncell_monitor_timer_handler;
  l1_log_on_demand_t                        l1_log_on_demand;
  gts_timer_handler_t                       gts_timer_handler;
  gsm_call_t                                gsm_call;

} geran_static_gl1_ext_api_s;

extern geran_static_gl1_ext_api_s gl1_vtbl;

void l1_handle_message_static( l1_queue_message_T* message );

void gpl1_gprs_handle_rlc_ph_data_req_sig_static(void);

void gpl1_gprs_serving_cell_meas_if_static(void);

void gsm_l1_report_watchdog_static(void);

void l1_isr_watchdog_check_static (void);

#if defined (FEATURE_GSM_TO_LTE) || defined (FEATURE_GSM_TDS)
void  gl1_handle_message_router_msgs_static(void);
#endif

#if defined(FEATURE_GSM_COEX_SW_CXM)
void gl1_handle_cxm_msgr_msgs_static(void);
#endif

void l1_idle_saic_disable_timer_handler( unsigned long  );

void l1_rach_disable_timer_handler( unsigned long  );

void l1_isr_femto_timeout_handler_callback (const uint32 timer_id);

void l1_sc_ncell_monitor_timer_handler( void  );

void gl1_gts_time_update_cb( void );

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
boolean l1_handle_proc_arb_message( l1_queue_message_T* message );
#endif

#endif /* L1_STATIC_API_H */
