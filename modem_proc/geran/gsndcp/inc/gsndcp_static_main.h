#ifndef GSNDCP_STATIC_MAIN_H
#define GSNDCP_STATIC_MAIN_H

/*!
  @file GSNDCP_STATIC_MAIN.h

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*==========================================================================*/
/* $Header: 
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
***
*****************************************************************************/

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "gsni.h"
#include "gsndcplog.h"
#include "geran_tasks.h"

/*****************************************************************************
***
***     Public Data Declarations
***
*****************************************************************************/
/* SNDCP critical section to protect accesses to messages and internal vars */
extern rex_crit_sect_type gsn_crit_sec;

#ifndef FEATURE_DATA_MM
/* Rex signal for the gsn_cmd_q queue */
extern rex_sigs_type      host_cmd_queue_sig;
/* Rex signal for the gsnll_data_q queue and associated flow control mask. */
extern rex_sigs_type      host_ll_dl_queue_sig;
#else
extern comp_sig_enum_type host_cmd_queue_sig;
extern comp_sig_enum_type host_ll_dl_queue_sig;
#endif /* FEATURE_DATA_MM */

/* Queue that holds LL->SNDCP SN-PDUs (LL-DATA.IND and LL-UNITDATA.IND primitives) */
extern q_type              gsnll_data_q;
extern dsm_watermark_type  gsnll_data_wm;

/* Queue that holds SM->SNDCP and LLC->SNDCP commands (SNSM and LL control primitives)  */
extern q_type              gsn_cmd_q;

extern uint32              process_delay_max;
extern boolean             gsn_ll_dl_susp_flag;

/* Table of active NSAPIs. Order of arrey elemets according to NSAPI number */
extern gsn_nsapi_e_t *actv_nsapis[GSN_NSAPI_MAX];

/* Table of active SAPIs. Order of arrey elemets according to SAPI number */
extern gsn_sapi_e_t  *actv_sapis[GSN_SAPI_MAX];
extern const uint8    gsn_extern_sapi[];

/* Control primitives Ids send to SNDCP from SM and LLC layers */
typedef enum
{
  LL_CMD,    /* LL control primitive    */
  SM_CMD,    /* SM control primitive    */
  GSN_CMD    /* SNDCP internal message  */
} gsn_cmd_id_t;

/* The Control primitives that the SM and LLC layers (plus internal SNDCP cmd)
** send to SNDCP need to be unified and stored in a common queue (gsn_cmd_q).
** The following type serves this purpose.
*/
typedef struct
{
  q_link_type             link;
  gsn_cmd_id_t            cmd_id;

  union
  {
    gsmsn_mes_type        *sm_msg;
    gprs_ll_dl_sig_mes_t  *ll_msg;
    gsn_internal_msg_t    *gsn_msg;
  } ctrl_msg;
} gsn_ctrl_msg_t;

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
typedef struct
{
  q_link_type    link;
  void*          msg;
  gsn_cmd_id_t   msg_type;
} gsn_ext_msg_t;
#endif

/*****************************************************************************
***
***     External API prototypes for interface
***
*****************************************************************************/
typedef void (*gsndcp_snsm_put_mes_t)(gsmsn_mes_type*);
typedef void (*gsn_int_put_mes_t)(gsn_internal_msg_t*);
#ifndef FEATURE_GERAN_REDUCED_DEBUG
typedef const gsndcp_diag_pdu_stat_rsp_t* (*gsndcp_diag_get_pdu_stats_t)(uint8);
typedef void (*gsndcp_diag_reset_pdu_stats_t)(uint8);
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
typedef uint32 (*gsndcp_follow_on_req_enquiry_t)(void);

#ifdef FEATURE_DATA_MM
typedef boolean (*gsndcp_process_cmd_queue_t)(comp_sig_enum_type, void*);
typedef boolean (*gsndcp_process_ll_dl_queue_t)(comp_sig_enum_type, void*);
#else
typedef boolean (*gsndcp_process_cmd_queue_t)(void);
typedef boolean (*gsndcp_process_ll_dl_queue_t)(void);
#endif /* FEATURE_DATA_MM */

typedef void (*gsndcp_to_sndcp_each_enqueue_cb_t)(dsm_watermark_type*, void*);
typedef boolean (*gsndcp_to_sndcp_sig_hdlr_t)(comp_sig_enum_type, void*);
typedef void (*gsn_ul_clr_est_susp_t)(uint8);
typedef void (*gsn_timer_callback_t)(uint32);
#ifndef FEATURE_GERAN_REDUCED_DEBUG
typedef log_on_demand_status_enum_type (*sndcp_log_on_demand_t)(log_code_type);
typedef void (*sndcp_log_packet_request_t)(uint16, uint8, uint8);
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */


/**********************************************************************************/
typedef struct
{
  gsndcp_snsm_put_mes_t               gsndcp_snsm_put_mes;
  gsn_int_put_mes_t                   gsn_int_put_mes;
#ifndef FEATURE_GERAN_REDUCED_DEBUG
  gsndcp_diag_get_pdu_stats_t         gsndcp_diag_get_pdu_stats;
  gsndcp_diag_reset_pdu_stats_t       gsndcp_diag_reset_pdu_stats;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
  gsndcp_follow_on_req_enquiry_t      gsndcp_follow_on_req_enquiry;
  gsndcp_process_cmd_queue_t          gsndcp_process_cmd_queue;
  gsndcp_process_ll_dl_queue_t        gsndcp_process_ll_dl_queue;
  gsndcp_to_sndcp_each_enqueue_cb_t   gsndcp_to_sndcp_each_enqueue_cb;
  gsndcp_to_sndcp_sig_hdlr_t          gsndcp_to_sndcp_sig_hdlr;
  gsn_ul_clr_est_susp_t               gsn_ul_clr_est_susp;
  gsn_timer_callback_t                gsn_timer_callback;
#ifndef FEATURE_GERAN_REDUCED_DEBUG
  sndcp_log_on_demand_t               sndcp_log_on_demand;
  sndcp_log_packet_request_t          sndcp_log_packet_request;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
}geran_static_gsn_ext_api_s;

/**********************************************************************************/
extern geran_static_gsn_ext_api_s gsndcp_vtbl;

/**********************************************************************************/
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
extern void gsn_send_to_dyn_pendq(void* msg_ptr, gsn_cmd_id_t type);
extern q_type* gsn_get_dyn_pendq_ptr(void);
extern void gsn_clear_dyn_pendq();
#endif


#endif
