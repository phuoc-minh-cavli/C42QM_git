/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S

                        A T   C O M M A N D

                        D Y N A M I C   L O A D I N G

GENERAL DESCRIPTION
  This module contains functions that are used for dynamic loading of
  AT commands.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2014 - 2019 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/dataiot.mpss/2.0/interface/atcop/src/dsatdl.c#2 $ $DateTime: 2020/08/05 02:45:39 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     -----------------------------------------------------------------
05/01/17   skc     Featurizing PBM Code.
08/27/14   tk      Fixed issue in RING URC reporting for MT CSD call.
07/29/14   sc      Added support for NV Refresh.
06/27/14   tk/sc   Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "dsatdl.h"

#include "dsatact.h"
#include "dsatctab.h"
#include "dsatvoice.h"

#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsicall.h"
#include "dsatetsictab.h"
#include "dsatetsime.h"
#include "dsatetsicmif.h"
#endif /* FEATURE_DSAT_ETSI_MODE */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
            Import External Variables
-------------------------------------------------------------------------*/

#ifdef FEATURE_ETSI_SMS
extern const dsatsms_msg_handler_tab_type dsatetsisms_msg_handler_table;
#endif /* FEATURE_ETSI_SMS */

/*-------------------------------------------------------------------------
            Local Variables
-------------------------------------------------------------------------*/

LOCAL const dsatdl_symbol_table_entry_s_type dsatdl_symbol_table_entries[]  =
{
  { (void *) &async_event_table,
    (void **) &dsatdl_vtable.async_event_table_ptr },
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
  { (void *) &pbm_cb_handler_table,
    (void **) &dsatdl_vtable.pbm_cb_handler_table_ptr },
#endif /* (FEATURE_ETSI_PBM) || (FEATURE_DSAT_CDMA_PBM) */    

#ifdef FEATURE_ETSI_SMS
  { (void *) &dsatetsisms_msg_handler_table,
    (void **) &dsatdl_vtable.dsatetsisms_msg_handler_table_ptr },
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_DATA_MUX
  { (void *) dsatact_cmux_handler,
    (void **) &dsatdl_vtable.dsatact_cmux_handler_fp },
#endif /* FEATURE_DATA_MUX */

  { (void *) dsatact_proc_dial_str,
    (void **) &dsatdl_vtable.dsatact_proc_dial_str_fp },

  { (void *) dsatcmdp_init,
    (void **) &dsatdl_vtable.dsatcmdp_init_fp },

  { (void *) dsatcmdp_deinit,
    (void **) &dsatdl_vtable.dsatcmdp_deinit_fp },

  { (void *) dsatcmdp_process_cmd_line,
    (void **) &dsatdl_vtable.dsatcmdp_process_cmd_line_fp },

  { (void *) dsatcmdpi_queue_cmd_results,
    (void **) &dsatdl_vtable.dsatcmdpi_queue_cmd_results_fp },

  { (void *) dsatcmdp_process_async_cmd,
    (void **) &dsatdl_vtable.dsatcmdp_process_async_cmd_fp },

  { (void *) dsatcmdp_process_async_signal,
    (void **) &dsatdl_vtable.dsatcmdp_process_async_signal_fp },

  { (void *) dsatcmdp_block_indications,
    (void **) &dsatdl_vtable.dsatcmdp_block_indications_fp },

  { (void *) dsatcmdp_sio_is_busy,
    (void **) &dsatdl_vtable.dsatcmdp_sio_is_busy_fp },

  { (void *) dsatcmdp_sio_is_free,
    (void **) &dsatdl_vtable.dsatcmdp_sio_is_free_fp },

  { (void *) dsatcmdp_rmsm_done_handler,

    (void **) &dsatdl_vtable.dsatcmdp_rmsm_done_handler_fp },

  { (void *) dsatcmdp_rmsm_crtdcp_done_handler,
    (void **) &dsatdl_vtable.dsatcmdp_rmsm_crtdcp_done_handler_fp },

  { (void *) dsatcmdp_refresh_subs_based_nv,
    (void **) &dsatdl_vtable.dsatcmdp_refresh_subs_based_nv_fp },

  { (void *) dsatcmif_cm_call_event_handler,
    (void **) &dsatdl_vtable.dsatcmif_cm_call_event_handler_fp },

  { (void *) dsatcmif_cm_call_cmd_handler,
    (void **) &dsatdl_vtable.dsatcmif_cm_call_cmd_handler_fp },

  { (void *) dsatcmif_cm_ss_event_handler,
    (void **) &dsatdl_vtable.dsatcmif_cm_ss_event_handler_fp },

  { (void *) dsatcmif_cm_ph_cmd_handler,
    (void **) &dsatdl_vtable.dsatcmif_cm_ph_cmd_handler_fp },

  { (void *) dsatcmif_cm_ph_event_handler,
    (void **) &dsatdl_vtable.dsatcmif_cm_ph_event_handler_fp },

  { (void *) dsatclient_deregister_fwd_cmd,
    (void **) &dsatdl_vtable.dsatclient_deregister_fwd_cmd_fp },

  { (void *) dsatclient_ext_send_response,
    (void **) &dsatdl_vtable.dsatclient_ext_send_response_fp },

  { (void *) dsatclienti_ext_send_urc,
    (void **) &dsatdl_vtable.dsatclient_ext_send_urc_fp },

#ifdef FEATURE_DSAT_ETSI_MODE
  { (void *) dsatetsicall_send_cr_result,
    (void **) &dsatdl_vtable.dsatetsicall_send_cr_result_fp },

  { (void *) dsatetsicall_send_cr_result_ex,
    (void **) &dsatdl_vtable.dsatetsicall_send_cr_result_ex_fp },

  { (void *) dsatetsicall_report_creg_result,
    (void **) &dsatdl_vtable.dsatetsicall_report_creg_result_fp },

  { (void *) etsicmif_report_ussd_result,
    (void **) &dsatdl_vtable.etsicmif_report_ussd_result_fp },  
    
  { (void *) etsicmif_send_sups_error,
    (void **) &dsatdl_vtable.etsicmif_send_sups_error_fp },  
    
  { (void *) dsatetsicmif_cm_sups_event_handler,
    (void **) &dsatdl_vtable.dsatetsicmif_cm_sups_event_handler_fp },

  { (void *) dsatetsicmif_cm_sups_cmd_handler,
    (void **) &dsatdl_vtable.dsatetsicmif_cm_sups_cmd_handler_fp },

  { (void *) dsatetsime_report_ciev_result,
    (void **) &dsatdl_vtable.dsatetsime_report_ciev_result_fp },

  { (void *) dsat_pin_mmgsdi_event_handler,
    (void **) &dsatdl_vtable.dsat_pin_mmgsdi_event_handler_fp },
#endif /* FEATURE_DSAT_ETSI_MODE */

  { (void *) dsatme_cmd_status_handler,
    (void **) &dsatdl_vtable.dsatme_cmd_status_handler_fp },

#ifdef FEATURE_MMGSDI
  { (void *) dsatme_mmgsdi_sim_event_handler,
    (void **) &dsatdl_vtable.dsatme_mmgsdi_sim_event_handler_fp },
#endif /* FEATURE_MMGSDI */

  { (void *) dsatprepi_set_preprocessor_mode,
    (void **) &dsatdl_vtable.dsatprepi_set_preprocessor_mode_fp },

  { (void *) dsatprep_process_sio_command,
    (void **) &dsatdl_vtable.dsatprep_process_sio_command_fp },

  { (void *) dsatrsp_send_intermediate_result_code,
    (void **) &dsatdl_vtable.dsatrsp_send_intermediate_result_code_fp },

  { (void *) dsatrsp_send_result_code,
    (void **) &dsatdl_vtable.dsatrsp_send_result_code_fp },

  { (void *) dsatrsp_send_result_code_ex,
    (void **) &dsatdl_vtable.dsatrsp_send_result_code_ex_fp },

  { (void *) dsatrsp_report_rate_on_connect,
    (void **) &dsatdl_vtable.dsatrsp_report_rate_on_connect_fp },

#ifdef FEATURE_DSAT_MDM_FUSION
  { (void *) dsatrsp_process_proxy_response,
    (void **) &dsatdl_vtable.dsatrsp_process_proxy_response_fp },

  { (void *) dsatrsp_process_proxy_urc,
    (void **) &dsatdl_vtable.dsatrsp_process_proxy_urc_fp },
#endif /* FEATURE_DSAT_MDM_FUSION */

  { (void *) dsatrsp_send_urc,
    (void **) &dsatdl_vtable.dsatrsp_send_urc_fp },

#ifdef FEATURE_ETSI_SMS
  { (void *) dsatsms_flush_sms_indications,
    (void **) &dsatdl_vtable.dsatsms_flush_sms_indications_fp },


 { (void *) dsatsmsi_cmd_err_cb_func,
   (void **) &dsatdl_vtable.dsatsmsi_cmd_err_cb_func_fp },

  { (void *) dsatsmsi_memory_status_cfg_event_handler,
    (void **) &dsatdl_vtable.dsatsmsi_memory_status_cfg_event_handler_fp },

  { (void *) dsatsmsu_send_async_indication_to_te,
  	(void **) &dsatdl_vtable.dsatsmsu_send_async_indication_to_te_fp },
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_ETSI_SMS
  { (void *) write_msg_event_handler,
    (void **) &dsatdl_vtable.write_msg_event_handler_fp },
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_ETSI_SMS_CB
  { (void *) send_cb_msg_to_te,
    (void **) &dsatdl_vtable.send_cb_msg_to_te_fp },
#endif /* FEATURE_ETSI_SMS_CB */

  { (void *) voice_send_ring_result,                 
    (void **) &dsatdl_vtable.voice_send_ring_result_fp }
};

const dsatdl_symbol_table_s_type dsatdl_symbol_table =
{
  dsatdl_symbol_table_entries,
  ARR_SIZE(dsatdl_symbol_table_entries)
};

