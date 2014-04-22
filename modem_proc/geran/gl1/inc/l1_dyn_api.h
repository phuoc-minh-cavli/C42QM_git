#ifndef L1_DYN_API_H
#define L1_DYN_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          L 1   D Y N A M I C   A P I

GENERAL DESCRIPTION
   This header file contains prototypes of dynamic APIs 
   
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2015 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gl1/inc/l1_dyn_api.h#5 $
$DateTime: 2018/10/12 05:20:45 $ $Author: pwbldsvc $


when       who       what, where, why
--------   ---       ---------------------------------------------------------
09/08/17   sk       CR2294063 Adding a wrapper for the external fucntion gsm_call
07/08/18   shm      CR 2274892 | Instantaneous WWAN timing and frequency information (rude wakeup)
09/01/18   hd       CR2167422 : FR 46686: Support GERAN as dynamic module using DMM/External paging framework 
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_dual_sim.h"
#include "geran_eng_mode_read_api.h"
#include "log.h" 

uint8  l1_log_get_TxPa_state_dyn(void);

void l1_log_signal_info_dyn(geran_eng_mode_signal_info_t *signal_info_ptr);

void l1_log_convert_txlev_to_pwr_dBm_dyn(eng_mode_sys_band_T band, uint8 txlev, uint8 *power_out);

boolean L1_ftm_mode_enabled_dyn( void );

void l1_get_usf_utilisation_info_dyn(l1_usf_utilisation_info_t* usf_utilisation_info);
void l1_idle_saic_disable_timer_handler_dyn( void );

void l1_rach_disable_timer_handler_dyn( void  );

#if defined ( FEATURE_WCDMA ) || defined ( FEATURE_GSM_TDS )|| defined ( FEATURE_GSM_TO_LTE )
void l1_isr_femto_timeout_handler_callback_dyn(const uint32 timer_id);
#endif

void l1_sc_ncell_monitor_timer_handler_dyn( void  );

log_on_demand_status_enum_type l1_log_on_demand_dyn(log_code_type log_code);
uint32 gsm_call_dyn ( uint32 cmd_id, void* generic_params,  uint32 size );
void gl1_gts_time_update_cb_dyn( void );

#endif /* L1_DYN_API_H */
