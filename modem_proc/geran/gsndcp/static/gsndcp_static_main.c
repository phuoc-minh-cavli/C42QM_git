/*****************************************************************************
***
*** TITLE
***
***  GPRS SNDCP LAYER
***
***
*** DESCRIPTION
***
***  Provides the SNDCP layer access functions to the
***  layer 2 control tasks, in the form of uplink and downlink
***  initialisation, and uplink and downlink event dispatch functions.
***
***
*** Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
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
*** $Header: //components/rel/geran.mpss/5.2.0/gsndcp/static/gsndcp_static_main.c#5 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
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

#include "comdef.h"
#include "gmutex.h"
#include "gprs_mem.h"
#include "gsndcpi.h"
#include "gsndcp_static_main.h"
#include "gllc_v_g.h"
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
#include "geran_dmm.h"
#endif

/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/

/* If TRUE, Indicates that the SNDCP layer has been initialised  */
boolean             sndcp_init = FALSE;
boolean             sndcp_log_initialized = FALSE;

/* Pointer to the host task control block. Set upon SNDCP initialisation */
rex_tcb_type       *host_tcb_ptr = NULL;

/* SNDCP critical section to protect accesses to messages and internal vars
*/
rex_crit_sect_type  gsn_crit_sec;

#ifndef FEATURE_DATA_MM
/* Rex signal for the gsn_cmd_q queue */
rex_sigs_type       host_cmd_queue_sig;
/* Rex signal for the gsnll_data_q queue and associated flow control mask. */
rex_sigs_type       host_ll_dl_queue_sig;
#else
comp_sig_enum_type  host_cmd_queue_sig;
comp_sig_enum_type  host_ll_dl_queue_sig;
#endif /* FEATURE_DATA_MM */

/* Table of active NSAPIs. Order of array elements according to NSAPI number */
gsn_nsapi_e_t      *actv_nsapis[GSN_NSAPI_MAX];

/* Table of active SAPIs. Order of array elements according to SAPI number */
gsn_sapi_e_t       *actv_sapis[GSN_SAPI_MAX];

/* Hold LLC SAPIs' N201_I and N201_U  */
uint16              gsn_n201_i[GSN_SAPI_MAX];
uint16              gsn_n201_u[GSN_SAPI_MAX];

/* SNDCP to GPRS SAPI address translator. */
const uint8 gsn_extern_sapi[ GSN_SAPI_MAX ] =
{
  3,    /* GPRS LL SAPI 3   */
  5,    /* GPRS LL SAPI 5   */
  9,    /* GPRS LL SAPI 9   */
  11    /* GPRS LL SAPI 11  */
};

#ifdef FEATURE_EXTERNAL_PAGING_GERAN  
static q_type      gsn_dyn_pendq;
#endif

/* Queue that holds LL->SNDCP SN-PDUs (LL-DATA.IND and LL-UNITDATA.IND primitives) */
q_type              gsnll_data_q;
dsm_watermark_type  gsnll_data_wm;

/* Queue that holds SM->SNDCP and LLC->SNDCP commands (SNSM and LL control primitives)  */
q_type              gsn_cmd_q;

uint32              process_delay_max  = 0;
boolean             gsn_ll_dl_susp_flag = FALSE;

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
void gsn_send_to_dyn_pendq(void* msg_ptr, gsn_cmd_id_t type)
{
  gsn_ext_msg_t* pend_data_ptr = GPRS_MEM_MALLOC(sizeof(gsn_ext_msg_t)/sizeof(uint8));

  if (pend_data_ptr!= NULL && msg_ptr != NULL)
  {
    pend_data_ptr->msg = msg_ptr;
    pend_data_ptr->msg_type = type;

    /* Put recently received message in Dynamic pending Queue */
    (void)q_link(pend_data_ptr, &pend_data_ptr->link);
    q_put(&gsn_dyn_pendq, &pend_data_ptr->link);
  }
  else
  {
    ERR_GERAN_FATAL_0_G("GSN UL Pendq Put: NULL Ptr Exception");
  }
}

q_type* gsn_get_dyn_pendq_ptr(void)
{
  return &gsn_dyn_pendq;
}

void gsn_clear_dyn_pendq()
{
  gsn_ext_msg_t* pend_data_ptr = (gsn_ext_msg_t*)q_get(&gsn_dyn_pendq);

  while(pend_data_ptr != NULL)
  {
    GPRS_MEM_FREE(pend_data_ptr->msg);
    GPRS_MEM_FREE(pend_data_ptr);
    pend_data_ptr = (gsn_ext_msg_t*)q_get(&gsn_dyn_pendq);
  }
}
#endif

/*===========================================================================
===
===  FUNCTION      GSNDCP_INITIALISE()
===
===  DESCRIPTION
===
===    Permits a given task context (PSMGR) to host SNDCP processing by
===    registering the host task's TCB pointer with SNDCP. The REX signals
===    that have been allocated by the host task, for scheduling the
===    processing of the SNDCP command queue and LL SAP downlink
===    queue, are also registered.
===
===  DEPENDENCIES
===
===    Must be called at host task initialization.
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
void gsndcp_initialise
(
#ifdef FEATURE_DATA_MM
  void
#else
  rex_tcb_type            *tcb_ptr,
  rex_sigs_type           cmd_queue_sig,
  rex_sigs_type           ll_dl_queue_sig
#endif /* FEATURE_DATA_MM */
)
{
  uint8                         i;

  /* Initialise critical section */
  rex_init_crit_sect(&gsn_crit_sec);

#ifndef FEATURE_DATA_MM
  /* Get host task control block pointer and the REX signals related to SNDCP queues */
  host_tcb_ptr = tcb_ptr;
  host_cmd_queue_sig = cmd_queue_sig;
  host_ll_dl_queue_sig = ll_dl_queue_sig;
#else
  /* Set the REX signals related to SNDCP queues */
  host_cmd_queue_sig = COMP_SNDCP_CMD_Q_SIGNAL;
  host_ll_dl_queue_sig = COMP_SNDCP_LL_DL_Q_SIGNAL;

  /* Set the signal handler for the command Queue signal */
  (void)comp_set_sig_handler((comp_sig_enum_type)host_cmd_queue_sig,
                             gsndcp_process_cmd_queue, NULL);

  /* Set the signal handler for the dl data Queue signal */
  (void)comp_set_sig_handler((comp_sig_enum_type)host_ll_dl_queue_sig,
                             gsndcp_process_ll_dl_queue, NULL);
#endif /* FEATURE_DATA_MM */

  /* Initialise all NSAPI entities */
  for(i=0; i < (uint8) GSN_NSAPI_MAX; i++)
  {
    actv_nsapis[i]  = NULL;
  }

  /* Initialise all SAPI entities */
  for(i=0; i < (uint8) GSN_SAPI_MAX; i++)
  {
    actv_sapis[i]  = NULL;
  }

  /* Initialise LLC SAPIs N201_I and N201_U */
  gsn_n201_i[GSN_SAPI_3] = gllc_ll_get_default_N201_I(gsn_extern_sapi[GSN_SAPI_3]);
  gsn_n201_u[GSN_SAPI_3] = gllc_ll_get_default_N201_U(gsn_extern_sapi[GSN_SAPI_3]);

  gsn_n201_i[GSN_SAPI_5] = gllc_ll_get_default_N201_I(gsn_extern_sapi[GSN_SAPI_5]);
  gsn_n201_u[GSN_SAPI_5] = gllc_ll_get_default_N201_U(gsn_extern_sapi[GSN_SAPI_5]);

  gsn_n201_i[GSN_SAPI_9] = gllc_ll_get_default_N201_I(gsn_extern_sapi[GSN_SAPI_9]);
  gsn_n201_u[GSN_SAPI_9] = gllc_ll_get_default_N201_U(gsn_extern_sapi[GSN_SAPI_9]);

  gsn_n201_i[GSN_SAPI_11] = gllc_ll_get_default_N201_I(gsn_extern_sapi[GSN_SAPI_11]);
  gsn_n201_u[GSN_SAPI_11] = gllc_ll_get_default_N201_U(gsn_extern_sapi[GSN_SAPI_11]);

#ifdef FEATURE_DATA_MM

  /* Enable the signals. The signals have to be enabled before they can be used */

#endif /* FEATURE_DATA_MM */

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  (void)q_init(&gsn_dyn_pendq);
#endif

  (void)q_init(&gsn_cmd_q);

  /* Initialise the watermark that holds LL->SNDCP SN-PDUs (LL DL data
   primitives). Need to ensure that High Water, Low Water, and Don't Exceed
   counts for this watermark are initialised via a call to
   gllc_register_ll_client_sndcp( ) as is done below. */
  dsm_queue_init(&gsnll_data_wm, 0, &gsnll_data_q);

  gsnll_data_wm.each_enqueue_func_ptr = NULL;
  gsnll_data_wm.lowater_func_ptr      = gllc_llme_clr_own_rcvr_busy_cond;
  gsnll_data_wm.lowater_func_data     = 0;
  gsnll_data_wm.gone_empty_func_ptr   = NULL;
  gsnll_data_wm.non_empty_func_ptr    = NULL;
  gsnll_data_wm.hiwater_func_ptr      = gllc_llme_set_own_rcvr_busy_cond;
  gsnll_data_wm.hiwater_func_data     = 0;
  gsnll_data_wm.lo_watermark          =
    (gllc_ll_get_max_N201_U(gsn_extern_sapi[GSN_SAPI_3]) * 2);

  gsnll_data_wm.hi_watermark          =
    (gllc_ll_get_max_N201_U(gsn_extern_sapi[GSN_SAPI_3]) * 4);

  gsnll_data_wm.dont_exceed_cnt       =
    (gsnll_data_wm.hi_watermark * 28);

  gsnll_data_wm.current_cnt           = 0;
#ifdef FEATURE_DSM_MEM_CHK
  gsnll_data_wm.highest_cnt           = 0;
  gsnll_data_wm.total_rcvd_cnt        = 0;
#endif /* FEATURE_DSM_MEM_CHK */

  /* Set SNDCP initialisation flag */
  sndcp_init = TRUE;

  MSG_GERAN_LOW_0("GSN " "ME: SNDCP Initialized");
}

