/*****************************************************************************
***
*** TITLE
***
***  GPRS LAYER 2 SERVICES CONTROL TASK FUNCTIONS
***
***
*** DESCRIPTION
***
***  REX task based scheduling framework for GPRS LLC functionality.
***
***
*** EXTERNALIZED FUNCTIONS
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  REX initialisation of the task control blocks and stacks is the
***  function of the TMC Task Mode Controller.
***
***
*** Copyright (c) 2001-2017 Qualcomm Technologies, Inc.
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
*** $Header: //components/rel/geran.mpss/5.2.0/gllc/src/gllctask.c#14 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10/12/13   ra      CR563511
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "geran_tasks.h"
#include "gllci.h"
#include "gllclient.h"
#include "gprs_mem.h"
#include "comdef.h"
#include "gmutex.h"
#include "dsm.h"
#include "err.h"
#include "msg.h"

#include "gllclog.h"
#include "gsndcp_v_g.h"
#include "gllc_static_main.h"
#include "gllc_dyn_api.h"
#include "stringl/stringl.h"

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/

/* This feature is defined to support two input parameters into call back
   functions.*/

#define FEATURE_DSM_WM_CB


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
rex_crit_sect_type gllc_dl_msg_crit_sec;
rex_crit_sect_type gllc_cipher_crit_sec;

/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/
/* GLLC vtable to map static API fptrs with dynamic APIs */
geran_static_gllc_ext_api_s gllc_vtbl =
{
  gllc_test_mode_handler_dyn,
  gllc_gmm_ready_timer_handler_dyn,
  gllc_event_handler_dyn,
  gmmllc_llgmm_put_mes_buf_dyn,
  gmmllc_ll_put_mes_dyn,
  gsmsllc_ll_put_mes_dyn,
  gllclient_gea_classmark_enquiry_dyn,
  gllclient_gea_classmark_enquiry_per_subs_dyn,
  gllc_ll_ul_sig_get_mes_buf_dyn,
  gllc_ll_ul_sig_put_mes_buf_dyn,
  gllc_ll_ul_dat_put_mes_buf_dyn,
  gllc_llme_get_gea_algorithm_dyn,
  gllc_grr_ul_suspend_ind_dyn,
  gllc_grr_ul_resume_ind_dyn,
  gllc_grr_ul_L3L2_suspend_ind_dyn,
  gllc_grr_ul_tbf_rel_ind_dyn,
  gllc_grr_ul_transmit_ind_dyn,
  gllc_grr_dl_put_mes_buf_dyn,
  gllc_llme_get_current_octet_count_dyn,
#ifndef FEATURE_GERAN_REDUCED_DEBUG
  gllc_diag_get_pdu_stats_dyn,
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
  gllc_llme_clr_own_rcvr_busy_cond_dyn,
  gllc_llme_set_own_rcvr_busy_cond_dyn,
#ifndef FEATURE_GERAN_REDUCED_DEBUG
  llc_log_packet_request_dyn,
  llc_log_on_demand_dyn
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
};

/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/* LLC UL Watermark Event Notification Functions */
static void gllc_ll_1_ul_ack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_SET_LLE_UL_EVT( GLLC_LL_SAPI_1_GMM, GLLC_LLE_UL_LL_ACK_Q_EVT );
}

static void gllc_ll_1_ul_ack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_CLR_LLE_UL_EVT( GLLC_LL_SAPI_1_GMM, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_1_ul_unack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_SET_LLE_UL_EVT( GLLC_LL_SAPI_1_GMM, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}


static void gllc_ll_1_ul_unack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_CLR_LLE_UL_EVT( GLLC_LL_SAPI_1_GMM, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}

/* ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------ */


static void gllc_ll_3_ul_ack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_SET_LLE_UL_EVT( GLLC_LL_SAPI_3_LL3, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_3_ul_ack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_CLR_LLE_UL_EVT( GLLC_LL_SAPI_3_LL3, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_3_ul_unack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_SET_LLE_UL_EVT( GLLC_LL_SAPI_3_LL3, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}


static void gllc_ll_3_ul_unack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_CLR_LLE_UL_EVT( GLLC_LL_SAPI_3_LL3, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}

/* ------------------------------------------------------------------------ */


static void gllc_ll_5_ul_ack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_SET_LLE_UL_EVT( GLLC_LL_SAPI_5_LL5, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_5_ul_ack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_CLR_LLE_UL_EVT( GLLC_LL_SAPI_5_LL5, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_5_ul_unack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_SET_LLE_UL_EVT( GLLC_LL_SAPI_5_LL5, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}


static void gllc_ll_5_ul_unack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_CLR_LLE_UL_EVT( GLLC_LL_SAPI_5_LL5, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}

/* ------------------------------------------------------------------------ */


static void gllc_ll_7_ul_ack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_SET_LLE_UL_EVT( GLLC_LL_SAPI_7_SMS, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_7_ul_ack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_CLR_LLE_UL_EVT( GLLC_LL_SAPI_7_SMS, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_7_ul_unack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_SET_LLE_UL_EVT( GLLC_LL_SAPI_7_SMS, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}


static void gllc_ll_7_ul_unack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_CLR_LLE_UL_EVT( GLLC_LL_SAPI_7_SMS, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}

/* ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------ */


static void gllc_ll_9_ul_ack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_SET_LLE_UL_EVT( GLLC_LL_SAPI_9_LL9, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_9_ul_ack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_CLR_LLE_UL_EVT( GLLC_LL_SAPI_9_LL9, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_9_ul_unack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_SET_LLE_UL_EVT( GLLC_LL_SAPI_9_LL9, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}


static void gllc_ll_9_ul_unack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_CLR_LLE_UL_EVT( GLLC_LL_SAPI_9_LL9, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}

/* ------------------------------------------------------------------------ */


static void gllc_ll_11_ul_ack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_SET_LLE_UL_EVT( GLLC_LL_SAPI_11_LL11, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_11_ul_ack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_CLR_LLE_UL_EVT( GLLC_LL_SAPI_11_LL11, GLLC_LLE_UL_LL_ACK_Q_EVT );
}


static void gllc_ll_11_ul_unack_wm_set_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_SET_LLE_UL_EVT( GLLC_LL_SAPI_11_LL11, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}


static void gllc_ll_11_ul_unack_wm_clr_evt
(
  dsm_watermark_type *wm_p,
  void *context
)
{
   GLLC_CLR_LLE_UL_EVT( GLLC_LL_SAPI_11_LL11, GLLC_LLE_UL_LL_UNACK_Q_EVT );
}

static void gllc_perform_static_backup()
{
  gllc_static_data_t* gllc_static_data_ptr = gllc_static_data_get_ptr();
  MSG_GERAN_HIGH_0("DMMSUSREM:: GLLC perform_static_backup");

  gllc_static_data_ptr->gllc_static_backup_ptr = (gllc_static_data_backup_t*)GPRS_MEM_MALLOC(sizeof(gllc_static_data_backup_t));

  /* Store gllc_lle into static backup */
  if (gllc_static_data_ptr->gllc_static_backup_ptr != NULL)
  {
    memscpy(
      gllc_static_data_ptr->gllc_static_backup_ptr->gllc_lle_data,
      (GLLC_LL_NUM_SAPIS * sizeof(gllc_lle_t)),
      gllc_lle,
      (GLLC_LL_NUM_SAPIS * sizeof(gllc_lle_t)));

    MSG_GERAN_HIGH_0("DMMSUSREM:: GLLC lle static_backup done");

    /* Store gllc_llme into static backup */
    memscpy(
      &(gllc_static_data_ptr->gllc_static_backup_ptr->gllc_llme_data),
      sizeof(gllc_llme_t),
      &gllc_llme,
      sizeof(gllc_llme_t));

    MSG_GERAN_HIGH_0("DMMSUSREM:: GLLC llme static_backup done");

    memscpy(
      gllc_static_data_ptr->gllc_static_backup_ptr->gllc_lle_ul_evt,
      (GLLC_LL_NUM_SAPIS * sizeof(uint16)),
      gllc_lle_ul_event_map,
      (GLLC_LL_NUM_SAPIS * sizeof(uint16)));

    MSG_GERAN_HIGH_0("DMMSUSREM:: GLLC gllc_lle_ul_event_map_backup done");

    memscpy(
      gllc_static_data_ptr->gllc_static_backup_ptr->gllc_lle_ul_mask,
      (GLLC_LL_NUM_SAPIS * sizeof(uint16)),
      gllc_lle_ul_mask_map,
      (GLLC_LL_NUM_SAPIS * sizeof(uint16)));

    MSG_GERAN_HIGH_0("DMMSUSREM:: GLLC gllc_lle_ul_mask_map_backup done");

    memscpy(
      &(gllc_static_data_ptr->gllc_static_backup_ptr->gllc_lle_ul_evt_flags),
      sizeof(uint16),
      &gllc_lle_ul_event_flags,
      sizeof(uint16));

    MSG_GERAN_HIGH_0("DMMSUSREM:: GLLC gllc_lle_ul_event_flags_backup done");

    memscpy(
      gllc_static_data_ptr->gllc_static_backup_ptr->gllc_llme_ul_evt_mask_scheme,
      (GLLC_LLME_NUM_SCHEMES * sizeof(boolean)),
      gllc_llme_ul_evt_mask_scheme_flags,
      (GLLC_LLME_NUM_SCHEMES * sizeof(boolean)));

    MSG_GERAN_HIGH_0("DMMSUSREM:: GLLC gllc_llme_ul_evt_mask_scheme_flags_backup done");

    memscpy(
      gllc_static_data_ptr->gllc_static_backup_ptr->gllc_lle_ul_evt_mask_scheme,
      (GLLC_LLE_NUM_SCHEMES * GLLC_LL_NUM_SAPIS * sizeof(boolean)),
      gllc_lle_ul_evt_mask_scheme_flags,
      (GLLC_LLE_NUM_SCHEMES * GLLC_LL_NUM_SAPIS * sizeof(boolean)));

    MSG_GERAN_HIGH_0("DMMSUSREM:: GLLC gllc_lle_ul_evt_mask_scheme_flags_backup done");
  }
  else
  {
    MSG_GERAN_ERROR_0("DMMSUSREM:: GLLC lle static_backup Failed to allocate memory");
  }
}

static void gllc_restore_static_data()
{
  gllc_static_data_t* gllc_static_data_ptr = gllc_static_data_get_ptr();
  MSG_GERAN_HIGH_0("DMMSUSREM:: GLLC restore_static_data");

  if (gllc_static_data_ptr->gllc_static_backup_ptr != NULL)
  {
    memscpy(
      gllc_lle,
      (GLLC_LL_NUM_SAPIS * sizeof(gllc_lle_t)),
      gllc_static_data_ptr->gllc_static_backup_ptr->gllc_lle_data,
      (GLLC_LL_NUM_SAPIS * sizeof(gllc_lle_t)));

    MSG_GERAN_MED_0_G("DMMSUSREM:: restore gllc_lle from backup");

    memscpy(
      &gllc_llme,
      sizeof(gllc_llme_t),
      &(gllc_static_data_ptr->gllc_static_backup_ptr->gllc_llme_data),
      sizeof(gllc_llme_t));

    MSG_GERAN_MED_0_G("DMMSUSREM:: restore gllc_llme from backup");

    memscpy(
      gllc_lle_ul_event_map,
      (GLLC_LL_NUM_SAPIS * sizeof(uint16)),
      gllc_static_data_ptr->gllc_static_backup_ptr->gllc_lle_ul_evt,
      (GLLC_LL_NUM_SAPIS * sizeof(uint16)));

    MSG_GERAN_MED_0_G("DMMSUSREM:: restore gllc_lle_ul_event_map from backup");

    memscpy(
      gllc_lle_ul_mask_map,
      (GLLC_LL_NUM_SAPIS * sizeof(uint16)),
      gllc_static_data_ptr->gllc_static_backup_ptr->gllc_lle_ul_mask,
      (GLLC_LL_NUM_SAPIS * sizeof(uint16)));

    MSG_GERAN_MED_0_G("DMMSUSREM:: restore gllc_lle_ul_mask_map from backup");

    memscpy(
      &gllc_lle_ul_event_flags,
      sizeof(uint16),
      &(gllc_static_data_ptr->gllc_static_backup_ptr->gllc_lle_ul_evt_flags),
      sizeof(uint16));

    MSG_GERAN_MED_0_G("DMMSUSREM:: restore gllc_lle_ul_event_flags from backup");

    memscpy(
      gllc_llme_ul_evt_mask_scheme_flags,
      (GLLC_LLME_NUM_SCHEMES * sizeof(boolean)),
      gllc_static_data_ptr->gllc_static_backup_ptr->gllc_llme_ul_evt_mask_scheme,
      (GLLC_LLME_NUM_SCHEMES * sizeof(boolean)));

    MSG_GERAN_MED_0_G("DMMSUSREM:: restore gllc_llme_ul_evt_mask_scheme_flags from backup");

    memscpy(
      gllc_lle_ul_evt_mask_scheme_flags,
      (GLLC_LLE_NUM_SCHEMES * GLLC_LL_NUM_SAPIS * sizeof(boolean)),
      gllc_static_data_ptr->gllc_static_backup_ptr->gllc_lle_ul_evt_mask_scheme,
      (GLLC_LLE_NUM_SCHEMES * GLLC_LL_NUM_SAPIS * sizeof(boolean)));

    GPRS_MEM_FREE(gllc_static_data_ptr->gllc_static_backup_ptr);
    gllc_static_data_ptr->gllc_static_backup_ptr = NULL;

    MSG_GERAN_MED_0_G("DMMSUSREM:: restore gllc_lle_ul_evt_mask_scheme_flags from backup");
  }
  else
  {
    MSG_GERAN_HIGH_0("DMMSUSREM:: gllc_static_backup_ptr - No data available");
  }
}

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GLLC_INIT()
===
===  DESCRIPTION
===
===    For use by LLC control task to initialise the LLC Process.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

void geran_gllc_init()
{
  uint32   lle;
  uint32   llme_ul_scheme;
  uint32   lle_ul_scheme;

  MSG_GERAN_HIGH_1("DMMSUSREM:: geran_gllc_init %d", gllc_initialised);

  /* This is a mutex common to all GLLC tasks, only initialise in association with task 1 */
  rex_init_crit_sect(&gllc_cipher_crit_sec);
  rex_init_crit_sect(&gllc_dl_msg_crit_sec);

  if (gllc_initialised == FALSE)
  {
    /* Initialise non-LLE queues. */
    (void)q_init(&gllc_gmm_ul_q);
    (void)q_init(&gllc_grr_dl_q);

    /* LLE initialisations. */
    for ( lle = 0; lle <= (uint32)GLLC_LL_LAST_SAPI; lle++ )
    {
      /* Initialise LL SAP Client Records. */
      gllc_ll_client_record[lle].ll_dl_sig_get_mes_buf_fnc_ptr = NULL;
      gllc_ll_client_record[lle].ll_dl_sig_put_mes_buf_fnc_ptr = NULL;
      gllc_ll_client_record[lle].ll_dl_dat_put_mes_buf_fnc_ptr = NULL;

      /* Initialise LLE queues and associated parameters. */
      (void)q_init( &gllc_cr_ul_q[lle] );
      (void)q_init( &gllc_ll_ul_sig_q[lle] );

      gllc_lle[lle].ack.retrx_first_p = NULL;
      gllc_lle[lle].ack.retrx_last_p  = NULL;

      gllc_lle[lle].ack.reseq_first_p = NULL;
      gllc_lle[lle].ack.reseq_last_p  = NULL;

      /* Initialise LLE acknowledged data watermarks. */
      gllc_ll_ul_ack_wm[lle].lo_watermark             = 0;
      gllc_ll_ul_ack_wm[lle].hi_watermark             = 0;
      gllc_ll_ul_ack_wm[lle].current_cnt              = 0;

  #ifdef FEATURE_DSM_MEM_CHK
      gllc_ll_ul_ack_wm[lle].highest_cnt              = 0;
      gllc_ll_ul_ack_wm[lle].total_rcvd_cnt           = 0;
  #endif

      gllc_ll_ul_ack_wm[lle].each_enqueue_func_ptr    = NULL;
      gllc_ll_ul_ack_wm[lle].gone_empty_func_ptr      = NULL;
      gllc_ll_ul_ack_wm[lle].non_empty_func_ptr       = NULL;
      gllc_ll_ul_ack_wm[lle].lowater_func_ptr         = NULL;
      gllc_ll_ul_ack_wm[lle].hiwater_func_ptr         = NULL;

      /* Initialise the ack watermark queue. Don't Exceed count is set to zero
         via the function below. Ensure that Don't Exceed count is set to
         appropriate value. */
      dsm_queue_init ( &gllc_ll_ul_ack_wm[lle], 0, &gllc_ll_ul_ack_q[lle]);

      /* Initialise LLE unacknowledged data watermarks. */
      gllc_ll_ul_unack_wm[lle].lo_watermark           = 0;
      gllc_ll_ul_unack_wm[lle].hi_watermark           = 0;
      gllc_ll_ul_unack_wm[lle].current_cnt            = 0;

  #ifdef FEATURE_DSM_MEM_CHK
      gllc_ll_ul_unack_wm[lle].highest_cnt            = 0;
      gllc_ll_ul_unack_wm[lle].total_rcvd_cnt         = 0;
  #endif

      gllc_ll_ul_unack_wm[lle].each_enqueue_func_ptr  = NULL;
      gllc_ll_ul_unack_wm[lle].gone_empty_func_ptr    = NULL;
      gllc_ll_ul_unack_wm[lle].non_empty_func_ptr     = NULL;
      gllc_ll_ul_unack_wm[lle].lowater_func_ptr       = NULL;
      gllc_ll_ul_unack_wm[lle].hiwater_func_ptr       = NULL;

      /* Initialise the unack watermark queue. Don't Exceed count is set to zero
         via the function below. Ensure that Don't Exceed count is set to
         appropriate value. */
      dsm_queue_init ( &gllc_ll_ul_unack_wm[lle], 0, &gllc_ll_ul_unack_q[lle]);

      /* Initialise LLE timers. */
      rex_def_timer_ex
      (
        &gllc_timer_t200[lle],
        gllc_static_timer_callback,
        (uint32)( (lle << 16) | GLLC_LLE_UL_TIMER_T200_EVT )
      );

      rex_def_timer_ex
      (
        &gllc_timer_t201[lle],
        gllc_static_timer_callback,
        (uint32)( (lle << 16) | GLLC_LLE_UL_TIMER_T201_EVT )
      );

      /* ................................................................... */

      /* Initialise LLE event masking scheme record flags. */

      for
      (
        lle_ul_scheme = 0;
        lle_ul_scheme < (uint32)GLLC_LLE_NUM_SCHEMES;
        lle_ul_scheme++
      )
      {
        gllc_lle_ul_evt_mask_scheme_flags[lle_ul_scheme][lle] = FALSE;
      }

      /* ................................................................... */

      /* Intialise LLE active event and mask bitmaps. */

      gllc_lle_ul_event_map[lle] = GLLC_LLE_UL_NO_EVT;
      gllc_lle_ul_mask_map[lle]  = (uint16)~GLLC_LLE_UL_NO_EVT;

      /* ................................................................... */

      /* Finally perform a full initialisation of the LLE. */
      gllc_lle_reset( (gllc_ll_sapi_t)lle, GLLC_LLE_TLLI_UNASSIGNED );
    }
  
    /* --------------------------------------------------------------------- */
  
    /* Non LLE specific event masking scheme flags initialisations. */
  
    for
    (
      llme_ul_scheme = 0;
      llme_ul_scheme < (uint32)GLLC_LLME_NUM_SCHEMES;
      llme_ul_scheme++
    )
    {
      gllc_llme_ul_evt_mask_scheme_flags[llme_ul_scheme] = FALSE;
    }

    /* --------------------------------------------------------------------- */

    /* LLE Watermark event notification, high, low, and dne initialisations. */

    /* SAPI-1 */
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_1_GMM].gone_empty_func_ptr     = gllc_ll_1_ul_ack_wm_clr_evt;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_1_GMM].non_empty_func_ptr      = gllc_ll_1_ul_ack_wm_set_evt;

    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_1_GMM].gone_empty_func_ptr   = gllc_ll_1_ul_unack_wm_clr_evt;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_1_GMM].non_empty_func_ptr    = gllc_ll_1_ul_unack_wm_set_evt;

    /* Flow control is not supported for GMM but set a maximum limit of the unacknowledged
       watermark occupancy of N maximal length frames. */

    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_1_GMM].dont_exceed_cnt       =
      (uint16)(gllc_max_N201_U_octets[ GLLC_LL_SAPI_1_GMM ] * 5);

    /* SAPI-3 */
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_3_LL3].gone_empty_func_ptr     = gllc_ll_3_ul_ack_wm_clr_evt;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_3_LL3].non_empty_func_ptr      = gllc_ll_3_ul_ack_wm_set_evt;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_3_LL3].hiwater_func_ptr        = gsn_ll3_ul_ack_hiwater;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_3_LL3].lowater_func_ptr        = gsn_ll3_ul_ack_lowater;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_3_LL3].hi_watermark            = GSN_LL3_ACK_HIWATER_CNT;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_3_LL3].lo_watermark            = GSN_LL3_ACK_LOWATER_CNT;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_3_LL3].dont_exceed_cnt         = GSN_LL3_ACK_DISCARD_CNT;

    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_3_LL3].gone_empty_func_ptr   = gllc_ll_3_ul_unack_wm_clr_evt;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_3_LL3].non_empty_func_ptr    = gllc_ll_3_ul_unack_wm_set_evt;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_3_LL3].hiwater_func_ptr      = gsn_ll3_ul_unack_hiwater;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_3_LL3].lowater_func_ptr      = gsn_ll3_ul_unack_lowater;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_3_LL3].hi_watermark          = GSN_LL3_UNACK_HIWATER_CNT;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_3_LL3].lo_watermark          = GSN_LL3_UNACK_LOWATER_CNT;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_3_LL3].dont_exceed_cnt       = GSN_LL3_UNACK_DISCARD_CNT;

    /* SAPI-5 */
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_5_LL5].gone_empty_func_ptr     = gllc_ll_5_ul_ack_wm_clr_evt;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_5_LL5].non_empty_func_ptr      = gllc_ll_5_ul_ack_wm_set_evt;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_5_LL5].hiwater_func_ptr        = gsn_ll5_ul_ack_hiwater;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_5_LL5].lowater_func_ptr        = gsn_ll5_ul_ack_lowater;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_5_LL5].hi_watermark            = GSN_LL5_ACK_HIWATER_CNT;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_5_LL5].lo_watermark            = GSN_LL5_ACK_LOWATER_CNT;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_5_LL5].dont_exceed_cnt         = GSN_LL5_ACK_DISCARD_CNT;

    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_5_LL5].gone_empty_func_ptr   = gllc_ll_5_ul_unack_wm_clr_evt;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_5_LL5].non_empty_func_ptr    = gllc_ll_5_ul_unack_wm_set_evt;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_5_LL5].hiwater_func_ptr      = gsn_ll5_ul_unack_hiwater;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_5_LL5].lowater_func_ptr      = gsn_ll5_ul_unack_lowater;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_5_LL5].hi_watermark          = GSN_LL5_UNACK_HIWATER_CNT;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_5_LL5].lo_watermark          = GSN_LL5_UNACK_LOWATER_CNT;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_5_LL5].dont_exceed_cnt       = GSN_LL5_UNACK_DISCARD_CNT;

    /* SAPI-7 */
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_7_SMS].gone_empty_func_ptr     = gllc_ll_7_ul_ack_wm_clr_evt;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_7_SMS].non_empty_func_ptr      = gllc_ll_7_ul_ack_wm_set_evt;

    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_7_SMS].gone_empty_func_ptr   = gllc_ll_7_ul_unack_wm_clr_evt;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_7_SMS].non_empty_func_ptr    = gllc_ll_7_ul_unack_wm_set_evt;

    /* Flow control is not supported for GSMS but set a maximum limit on the unacknowledged
       watermark occupancy of N maximal length frames. */

    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_7_SMS].dont_exceed_cnt =
        (uint16)(gllc_max_N201_U_octets[ GLLC_LL_SAPI_7_SMS ] * 5);

    /* SAPI-9 */
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_9_LL9].gone_empty_func_ptr     = gllc_ll_9_ul_ack_wm_clr_evt;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_9_LL9].non_empty_func_ptr      = gllc_ll_9_ul_ack_wm_set_evt;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_9_LL9].hiwater_func_ptr        = gsn_ll9_ul_ack_hiwater;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_9_LL9].lowater_func_ptr        = gsn_ll9_ul_ack_lowater;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_9_LL9].hi_watermark            = GSN_LL9_ACK_HIWATER_CNT;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_9_LL9].lo_watermark            = GSN_LL9_ACK_LOWATER_CNT;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_9_LL9].dont_exceed_cnt         = GSN_LL9_ACK_DISCARD_CNT;

    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_9_LL9].gone_empty_func_ptr   = gllc_ll_9_ul_unack_wm_clr_evt;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_9_LL9].non_empty_func_ptr    = gllc_ll_9_ul_unack_wm_set_evt;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_9_LL9].hiwater_func_ptr      = gsn_ll9_ul_unack_hiwater;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_9_LL9].lowater_func_ptr      = gsn_ll9_ul_unack_lowater;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_9_LL9].hi_watermark          = GSN_LL9_UNACK_HIWATER_CNT;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_9_LL9].lo_watermark          = GSN_LL9_UNACK_LOWATER_CNT;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_9_LL9].dont_exceed_cnt       = GSN_LL9_UNACK_DISCARD_CNT;

    /* SAPI-11 */
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_11_LL11].gone_empty_func_ptr   = gllc_ll_11_ul_ack_wm_clr_evt;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_11_LL11].non_empty_func_ptr    = gllc_ll_11_ul_ack_wm_set_evt;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_11_LL11].hiwater_func_ptr      = gsn_ll11_ul_ack_hiwater;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_11_LL11].lowater_func_ptr      = gsn_ll11_ul_ack_lowater;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_11_LL11].hi_watermark          = GSN_LL11_ACK_HIWATER_CNT;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_11_LL11].lo_watermark          = GSN_LL11_ACK_LOWATER_CNT;
    gllc_ll_ul_ack_wm[GLLC_LL_SAPI_11_LL11].dont_exceed_cnt       = GSN_LL11_ACK_DISCARD_CNT;

    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_11_LL11].gone_empty_func_ptr = gllc_ll_11_ul_unack_wm_clr_evt;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_11_LL11].non_empty_func_ptr  = gllc_ll_11_ul_unack_wm_set_evt;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_11_LL11].hiwater_func_ptr    = gsn_ll11_ul_unack_hiwater;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_11_LL11].lowater_func_ptr    = gsn_ll11_ul_unack_lowater;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_11_LL11].hi_watermark        = GSN_LL11_UNACK_HIWATER_CNT;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_11_LL11].lo_watermark        = GSN_LL11_UNACK_LOWATER_CNT;
    gllc_ll_ul_unack_wm[GLLC_LL_SAPI_11_LL11].dont_exceed_cnt     = GSN_LL11_UNACK_DISCARD_CNT;

    /* --------------------------------------------------------------------- */

    /* LLME initialisations. */
    gllc_llme_reset();

    /* TLLI Last Received, GEA, Cipher Key and IOV-UI must traverse general
       LLME Reset operations, and so must be reset individually */
    gllc_llme.tlli_last_rxd       = GLLC_TLLI_UNASSIGNED;
    gllc_llme.gea                 = GMMLLC_GEA_NONE;
    gllc_llme.key_Kc[0]           = 0;
    gllc_llme.key_Kc[1]           = 0;
    gllc_llme.key_Kc128[0]        = 0;
    gllc_llme.key_Kc128[1]        = 0;
    gllc_llme.key_Kc128[2]        = 0;
    gllc_llme.key_Kc128[3]        = 0;
    gllc_llme.input_offset_IOV_UI = 0;
  
    /* Host the GMM Ready Timer on behalf of GMM for restart efficiency purposes. */
    rex_def_timer( &gllc_gmm_ready_timer, geran_tcb_read(GERAN_TASK_ID_GLLC_1), GLLC_GMM_RDY_TMR_SIG );

    MSG_GERAN_LOW_0_G( "GLC ME: Initialised");
    gllc_initialised = TRUE;
  }
  else
  {
    gllc_restore_static_data();
  }

  /* Register GMM, GSMS, GSNDCP proxy clients on appropriate LL SAPs. */
  gllclient_reg_gmm_gsms_proxy();

  /* Initialise logging. */
  llc_log_init();

  /* --------------------------------------------------------------------- */
  (void) gllc_read_psm_info();

  /* Set GLLC_resume signal so that pending messages could be processed */
  (void)rex_set_sigs(geran_tcb_read(GERAN_TASK_ID_GLLC_1), GLLC_DYN_RESUME_SIG);
} /* end of llc_init() */

void geran_gllc_deinit()
{
  uint32   lle;

  MSG_GERAN_HIGH_0("DMMSUSREM:: geran_gllc_deinit");
  gllc_perform_static_backup();
}

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
void gllc_process_dyn_pendq(q_type *q_ptr)
{
  llc_ext_msg_t* pend_data_ptr = (llc_ext_msg_t*)q_get(q_ptr);

  while(pend_data_ptr != NULL)
  {
    switch (pend_data_ptr->msg_type)
    {
      case GMM_LLGMM:
      {
        gmmllc_llgmm_put_mes_buf_dyn((gmmllc_llgmm_mes_type*)pend_data_ptr->msg);
        break;
      }

      case GMM_LLE:
      {
        gmmllc_ll_put_mes_dyn((gmmllc_ll_mes_type*)pend_data_ptr->msg);
        break;
      }

      case GSMS_LLE:
      {
        gsmsllc_ll_put_mes_dyn((gsmsllc_ll_mes_type*)pend_data_ptr->msg);
        break;
      }
    }

    GPRS_MEM_FREE(pend_data_ptr);
    pend_data_ptr = (llc_ext_msg_t*)q_get(q_ptr);
  }
}

void geran_gllc_resume()
{
  MSG_GERAN_HIGH_0("GLLC dynamic Resume");
}
 
void geran_gllc_suspend()
{
  MSG_GERAN_HIGH_0("GLLC dynamic Suspend");
}
#endif

/*** EOF: don't remove! ***/

