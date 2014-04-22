#ifndef MDSP_STATIC_API_H
#define MDSP_STATIC_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          M D S P   S T A T I C   A P I

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

$Header: //components/rel/geran.mpss/5.2.0/gmdsp6/static/inc/gmdsp_static_api.h#2 $
$DateTime: 2019/03/14 03:27:07 $ $Author: pwbldsvc $


when       who       what, where, why
--------   ---       ---------------------------------------------------------
14/03/19   sk        CR924524 : Atlas 1.0 : Enhancement and corrections to mdsp_set_tx_burst_offset()
09/01/18   hd        CR2167422 FR 46686: Support GERAN as dynamic module using DMM/External paging framework 
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"

#include "gl1_hw_clk_ctl.h"
#include "l1i.h"
#include "l1_isr.h"
#include "l1_utils.h"
#include "l1_task.h"
#include "l1i.h"
#include "l1_isr.h"
#include "l1_utils.h"
#include "l1_task.h"

#include "gl1_hwi.h"
#include "mdsp_cmd.h"
#include "gl1_hw_g.h"


void mdsp_setup_linear_pa_dyn ( uint16 tx_cfg2_flag_val, sys_modem_as_id_e_type as_id);
void mdsp_ftm_set_tx_burst_offset_dyn
(
  int16 tx_burst_offset,
  sys_modem_as_id_e_type as_id
);

void mdsp_ftm_cfg_pa_profile_polar_dyn
(
   mdsp_pa_ramp_type *ramp_tbl,
   sys_modem_as_id_e_type as_id
);
void mdsp_ftm_cfg_amam_ampm_tables_dyn
(
   mdsp_amam_ampm_tables_type  *amam_ampm_tables,
   sys_modem_as_id_e_type      as_id
);
void mdsp_ftm_update_amam_ampm_tables_dyn
(
   boolean  new_am_am_table,
   boolean  new_am_pm_table,
   sys_modem_as_id_e_type as_id
);
void mdsp_ftm_issue_qdsp6_command_dyn (GfwCmdBufType *cmd, GfwMdspCmdType  cmdId , sys_modem_as_id_e_type as_id );
void mdsp_ftm_start_tx_rnd_cont_dyn ( uint8 gmsk_mod,sys_modem_as_id_e_type as_id );
void mdsp_ftm_start_tx_data_cont_dyn ( uint16 *buffer, uint16 buf_len,sys_modem_as_id_e_type as_id );
void mdsp_ftm_start_tx_tone_cont_dyn ( sys_modem_as_id_e_type as_id );
void mdsp_ftm_stop_tx_tone_cont_dyn ( sys_modem_as_id_e_type as_id );
void mdsp_cm_set_ftsm_dyn (uint16 ftsm, sys_modem_as_id_e_type as_id);
void mdsp_ftm_wait_for_frame_tick_dyn (sys_modem_as_id_e_type as_id);
void mdsp_xo_desense_arfcn_dyn (mdsp_xo_desense_arfcn_struct *pointer, sys_modem_as_id_e_type as_id); /*API use*/
void mdsp_rf_xo_desense_arfcn_dyn (mdsp_xo_desense_arfcn_struct *pointer , sys_modem_as_id_e_type as_id );
void mdsp_rf_set_iq_log_dyn(boolean value,sys_modem_as_id_e_type as_id);
void mdsp_rf_send_iq_samples_to_rf_dyn(sys_modem_as_id_e_type as_id );
boolean mdsp_register_iq_samples_callback_dyn ( mdsp_collect_iq_samples_cb_t callback );


typedef void (*mdsp_setup_linear_pa_t) ( uint16 tx_cfg2_flag_val, sys_modem_as_id_e_type as_id);
typedef void (*mdsp_ftm_set_tx_burst_offset_t)
 (
   int16 tx_burst_offset,
   sys_modem_as_id_e_type as_id
 );
 
typedef void (*mdsp_ftm_cfg_pa_profile_polar_t)
 (
    mdsp_pa_ramp_type *ramp_tbl,
    sys_modem_as_id_e_type as_id
 );
typedef void (*mdsp_ftm_cfg_amam_ampm_tables_t)
 (
    mdsp_amam_ampm_tables_type  *amam_ampm_tables,
    sys_modem_as_id_e_type      as_id
 );
typedef void (*mdsp_ftm_update_amam_ampm_tables_t)
 (
    boolean  new_am_am_table,
    boolean  new_am_pm_table,
    sys_modem_as_id_e_type as_id
 );
typedef void (*mdsp_ftm_issue_qdsp6_command_t) (GfwCmdBufType *, GfwMdspCmdType , sys_modem_as_id_e_type );
typedef void (*mdsp_ftm_start_tx_rnd_cont_t) ( uint8 ,sys_modem_as_id_e_type  );
typedef void (*mdsp_ftm_start_tx_data_cont_t) ( uint16 *, uint16 ,sys_modem_as_id_e_type );
typedef void (*mdsp_ftm_start_tx_tone_cont_t) ( sys_modem_as_id_e_type );
typedef void (*mdsp_ftm_stop_tx_tone_cont_t) ( sys_modem_as_id_e_type );
typedef void (*mdsp_cm_set_ftsm_t) (uint16 ftsm, sys_modem_as_id_e_type );
typedef void (*mdsp_ftm_wait_for_frame_tick_t) (sys_modem_as_id_e_type );
typedef void (*mdsp_xo_desense_arfcn_t) (mdsp_xo_desense_arfcn_struct *, sys_modem_as_id_e_type );
typedef void (*mdsp_rf_xo_desense_arfcn_t) (mdsp_xo_desense_arfcn_struct * , sys_modem_as_id_e_type  );
typedef void (*mdsp_rf_set_iq_log_t)(boolean value,sys_modem_as_id_e_type );
typedef void (*mdsp_rf_send_iq_samples_to_rf_t)(sys_modem_as_id_e_type  );
typedef boolean (*mdsp_register_iq_samples_callback_t)( mdsp_collect_iq_samples_cb_t );


typedef struct
{
    mdsp_setup_linear_pa_t                                                    mdsp_setup_linear_pa;
    mdsp_ftm_set_tx_burst_offset_t                                            mdsp_ftm_set_tx_burst_offset;
    mdsp_ftm_cfg_pa_profile_polar_t                                           mdsp_ftm_cfg_pa_profile_polar;
    mdsp_ftm_cfg_amam_ampm_tables_t                                           mdsp_ftm_cfg_amam_ampm_tables;
    mdsp_ftm_update_amam_ampm_tables_t                                        mdsp_ftm_update_amam_ampm_tables;
    mdsp_ftm_issue_qdsp6_command_t                                            mdsp_ftm_issue_qdsp6_command;
    mdsp_ftm_start_tx_rnd_cont_t                                              mdsp_ftm_start_tx_rnd_cont;
    mdsp_ftm_start_tx_data_cont_t                                             mdsp_ftm_start_tx_data_cont;
    mdsp_ftm_start_tx_tone_cont_t                                             mdsp_ftm_start_tx_tone_cont;
    mdsp_ftm_stop_tx_tone_cont_t                                              mdsp_ftm_stop_tx_tone_cont;
    mdsp_cm_set_ftsm_t                                                        mdsp_cm_set_ftsm;
    mdsp_ftm_wait_for_frame_tick_t                                            mdsp_ftm_wait_for_frame_tick;
    mdsp_xo_desense_arfcn_t                                                   mdsp_xo_desense_arfcn;
    mdsp_rf_xo_desense_arfcn_t                                                mdsp_rf_xo_desense_arfcn;
    mdsp_rf_set_iq_log_t                                                      mdsp_rf_set_iq_log;
    mdsp_rf_send_iq_samples_to_rf_t                                           mdsp_rf_send_iq_samples_to_rf;
    mdsp_register_iq_samples_callback_t                                       register_iq_samples_callback;
} geran_static_gmdsp_ext_api_s;

extern geran_static_gmdsp_ext_api_s gmdsp_vtbl;

#endif /* MDSP_STATIC_API_H */
