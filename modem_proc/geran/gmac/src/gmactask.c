/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC task
***
*** DESCRIPTION
***
***  Handles the initialisation of the MAC task and the signal reception
***  handler.
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
*** $Header: //components/rel/geran.mpss/5.2.0/gmac/src/gmactask.c#12 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 09/11/10   sjw     Updated for Dual SIM support
*** 06/27/01   tlx     Created
*** 03/08/02   pjr     added handling for L1 messages using static memory.
***                    Module affected rex_sigs_type gprs_mac_signal_handler,
***                    added flag free_l1_memory_flag;
*** 10/10/02  DLH      Added a gprs_l1_page_group_flag which is used to inform MAC
***                    from L1 whether the block of data has been received in the MS
***                    paging group or not.
***
*****************************************************************************/

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#include "customer.h"

#include "geran_tasks.h"
#include "gprsdef.h"
#include "gprsdef_g.h"

/* rex and system type operations */
#include "rex.h"
#include "task.h"
#include "rcinit.h"

/* for messaging */
#include "queue.h"
#include "dsm.h"
#include "memory.h"
#include "gprs_mem.h"
#include "string.h"

/* for debuging */
#include "msg.h"
#include "err.h"
#include "amssassert.h"

#include "gmac.h"
#include "gmacdef.h"
#include "gmacsig.h"
#include "gmacnull.h"
#include "gmacidle_if.h"
#include "gmactransfer_if.h"
#include "grlcmaccmn.h"
#include "gmacrb.h"
#include "gmactask.h"
#include "gmacutil.h"
#include "gmaclog.h"
#include "gmacl1.h"
#ifdef PERLUTF
#error code not present
#endif
#include "dog_hb_rex.h"
#include "gmacproc.h"
#include "gmutex.h"
#include "gmacds.h"
#include "stringl/stringl.h"

#include "gtmrs.h"
#include "gmac_static_task.h"

/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/
#ifdef FEATURE_GERAN_REDUCED_DEBUG
#define GMAC_NUM_TRACE_MSGS 20
#else /* FEATURE_GERAN_REDUCED_DEBUG */
#define GMAC_NUM_TRACE_MSGS 100
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

typedef struct
{
  boolean next_entry;/* This will tell whether next entry is free or not*/

  uint8 gmac_mode; /*NULL/IDLE/TRANSFER*/

  uint8 state;/*Corresponding mode state*/

  uint8 message_set;/*Received/sent message set*/

  uint8 message_id;/*Received/sent message ID*/

  uint8 message_frag[3];/*first three bytes of message*/
  uint32 FN;       /*current FN sig was rcvd on*/

} gmac_trace_msg_t;

typedef struct
{
  gmac_trace_msg_t gmac_trace_msg_buf[GMAC_NUM_TRACE_MSGS];
  byte gmac_trace_msg_buf_index;
} gmac_task_data_t;


static byte tracebuff_ver = 1;

/*task,msg mutex/crit section declaration outside of gmac_task_data to make rex mutex operation efficient*/
/*-----------------------------------------------------------------
** Critical Section to protect GMAC shared vars.
**-----------------------------------------------------------------
*/
  /* Critical section to access control GMAC messages */
extern  rex_crit_sect_type gmac_msg_crit_sec;

/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/

#ifdef PERLUTF
#error code not present
#else
static void gmac_initialise(void);
#endif

static boolean gmac_gprs_dl_ctrl_msg_proceed_validation(uint8 message_type, boolean proceed_flag);
static void gmac_gprs_dl_ctrl_msg_validation(uint8 message_type);
static void gmac_store_trace_rvcd_msg_buf(uint8 gmac_mode, uint8 state);
static boolean gmac_dl_msg_address_data_valid(uint8 sig_id,uint8 tsk_id);
static boolean gmac_assignment_unsupported(mac_modes_t mode,uint8 sig_id,uint8 tsk_id);
static boolean  gmac_check_q_for_mac_internal_sigs(mac_signals_u* item_ptr,uint8 compare_task_id);

static void gmac_log_reshuffling_of_internal_sigs(void);

/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/
gmac_task_data_t gmac_task_data;

/* GMAC vtable to map static fptrs with dynamic fpts */

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
geran_static_gmac_ext_api_s mac_vtbl =
{ 
  gprs_mac_signal_handler_dyn,
  gmac_store_trace_send_msg_buf_dyn,
  gprs_l1_mac_acc_func_dyn,
  gmac_start_t3166_dyn,
  new_gmac_trans_calc_e_pdan_size_dyn,
#ifndef FEATURE_GERAN_REDUCED_DEBUG
  mac_log_on_demand_dyn,
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */
  gprs_odm_info_is_assigned_tbf_egprs_dyn
};

#endif

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/
/*===========================================================================
===
===  FUNCTION    gmac_get_task_data_ptr()
===
===  DESCRIPTION
===  Returns a pointer to null data space 
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    gmac_null_data_t*
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static gmac_task_data_t* gmac_get_task_data_ptr(void)
{
  return &gmac_task_data;
}

/*===========================================================================
===
===  FUNCTION    gmac_init_task_data_non_zero()
===
===  DESCRIPTION
===  Initialises non zero (including all enums) task data.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_task_data_non_zero(void)
{
}


/*===========================================================================
===
===  FUNCTION    gmac_init_task_data()
===
===  DESCRIPTION
===  Initialises Task data.  Used for Lower layer resets.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_task_data(void)
{
  /*No Data to reset*/
}

/*===========================================================================
===
===  FUNCTION    gmac_reset_clear_task_queue()
===
===  DESCRIPTION
===  Used for Lower layer resets.  Clears all msgs from the task queue.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_reset_clear_task_queue(void)
{
  mac_signals_u  *reset_ptr;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr();

  /* gets message and clears it from message queue */
  reset_ptr = q_get(&global_data_ptr->mac_q);

  while(reset_ptr != NULL)
  {
    GPRS_MEM_FREE(reset_ptr);

    /* check message queue for more signals. if msg_ptr returns NULL queue is empty */
    reset_ptr = q_get(&global_data_ptr->mac_q);
  }
}

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
/*===========================================================================
===
===  FUNCTION    geran_gmac_init()
===
===  DESCRIPTION
===  Used by GCOMMON module for initialising.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/

void geran_gmac_init(void)
{
  gsm_mac_fn_init();
}

void geran_gmac_deinit(void)
{  
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr();
  geran_q_destroy(&global_data_ptr->mac_q);
  gmac_reset_stop_all_timers();
  mac_log_deinit();
  return;
}

void geran_gmac_resume(void)
{
  (void)rex_set_sigs(geran_tcb_read(GERAN_TASK_ID_GMAC), GMAC_DYN_RESUME_SIG);
  return;
}

void geran_gmac_suspend(void)
{
  return;
}
#endif /*FEATURE_EXTERNAL_PAGING_GERAN*/

/*===========================================================================
===
===  FUNCTION    gsm_mac_fn_init()
===
===  DESCRIPTION
===  Used for GMAC  initialising.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/

void gsm_mac_fn_init(void)
{
  /* Only use this mechanism to perform memory init, hardware init,
     or other startup init to satisfy dependency that follow. Avoid
     all HLOS mechanisms that will block. */

  gmac_initialise();

  /* Code in this function will add to startup latency. Do not use
     large loops that can be avoided.*/
}

/*===========================================================================
===
===  FUNCTION      gmac_multislot_class_init()
===
===  DESCRIPTION
===  Multislot class parameters to be initialised depending on the
===  current multislot class supported.
===
===  NOTE: This function only supports multislot classes 1 to 12 and shall
===        cause a fatal error if any other value set.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_multislot_class_init(void)
{
  uint8 nv_ms_class;

  uint8 nv_egprs_ms_class;

  uint8 upper_ms_class_limit = 0;

#ifdef FEATURE_GSM_DTM
  uint8 nv_dtm_ms_class; /*used for DTM GPRS and DTM EGPRS MS Classes*/
#endif /*FEATURE_GSM_DTM*/
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr();

#ifdef FEATURE_GSM_GPRS_MSC33
  upper_ms_class_limit = RRPS_MULTISLOT_CLASS_33;
#else
  upper_ms_class_limit = RRPS_MULTISLOT_CLASS_12;
#endif /*FEATURE_GSM_GPRS_MSC33*/

  MSG_GERAN_LOW_1_G("MAC trace buffer Version%d",tracebuff_ver);

  rr_read_public_store( RRPS_MS_MULTISLOT_CLASS, &nv_ms_class );

  if ( (nv_ms_class >= RRPS_MULTISLOT_CLASS_1 ) &&
       (nv_ms_class <= upper_ms_class_limit)    )
  {
    MSG_GERAN_LOW_1_G("GPRS multislot class %d",nv_ms_class);
  }
  else
  {
    nv_ms_class = RRPS_MULTISLOT_CLASS_1;
    MSG_GERAN_LOW_1_G("GPRS multislot class default %d",nv_ms_class);
  }

  switch (nv_ms_class)
  {
    case RRPS_MULTISLOT_CLASS_1:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_1");
      global_data_ptr->gmac_ms_class = MULTISLOT_CLASS_1;
      global_data_ptr->gmac_ms_class_sum = MULTISLOT_CLASS_SUM_1;
      global_data_ptr->gmac_ms_class_max_ts_tx = MS1_MAX_TS_TX;
      global_data_ptr->gmac_ms_class_max_ts_rx = MS1_MAX_TS_RX;
      global_data_ptr->gmac_ms_class_ts_tta = MS1_MIN_TS_TTA;
      global_data_ptr->gmac_ms_class_ts_ttb = MS1_MIN_TS_TTB;
      global_data_ptr->gmac_ms_class_ts_tra = MS1_MIN_TS_TRA;
      global_data_ptr->gmac_ms_class_ts_trb = MS1_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_2:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_2");
      global_data_ptr->gmac_ms_class = MULTISLOT_CLASS_2;
      global_data_ptr->gmac_ms_class_sum = MULTISLOT_CLASS_SUM_2;
      global_data_ptr->gmac_ms_class_max_ts_tx = MS2_MAX_TS_TX;
      global_data_ptr->gmac_ms_class_max_ts_rx = MS2_MAX_TS_RX;
      global_data_ptr->gmac_ms_class_ts_tta = MS2_MIN_TS_TTA;
      global_data_ptr->gmac_ms_class_ts_ttb = MS2_MIN_TS_TTB;
      global_data_ptr->gmac_ms_class_ts_tra = MS2_MIN_TS_TRA;
      global_data_ptr->gmac_ms_class_ts_trb = MS2_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_3:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_3");
      global_data_ptr->gmac_ms_class = MULTISLOT_CLASS_3;
      global_data_ptr->gmac_ms_class_sum = MULTISLOT_CLASS_SUM_3;
      global_data_ptr->gmac_ms_class_max_ts_tx = MS3_MAX_TS_TX;
      global_data_ptr->gmac_ms_class_max_ts_rx = MS3_MAX_TS_RX;
      global_data_ptr->gmac_ms_class_ts_tta = MS3_MIN_TS_TTA;
      global_data_ptr->gmac_ms_class_ts_ttb = MS3_MIN_TS_TTB;
      global_data_ptr->gmac_ms_class_ts_tra = MS3_MIN_TS_TRA;
      global_data_ptr->gmac_ms_class_ts_trb = MS3_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_4:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_4");
      global_data_ptr->gmac_ms_class = MULTISLOT_CLASS_4;
      global_data_ptr->gmac_ms_class_sum = MULTISLOT_CLASS_SUM_4;
      global_data_ptr->gmac_ms_class_max_ts_tx = MS4_MAX_TS_TX;
      global_data_ptr->gmac_ms_class_max_ts_rx = MS4_MAX_TS_RX;
      global_data_ptr->gmac_ms_class_ts_tta = MS4_MIN_TS_TTA;
      global_data_ptr->gmac_ms_class_ts_ttb = MS4_MIN_TS_TTB;
      global_data_ptr->gmac_ms_class_ts_tra = MS4_MIN_TS_TRA;
      global_data_ptr->gmac_ms_class_ts_trb = MS4_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_5:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_5");
      global_data_ptr->gmac_ms_class = MULTISLOT_CLASS_5;
      global_data_ptr->gmac_ms_class_sum = MULTISLOT_CLASS_SUM_5;
      global_data_ptr->gmac_ms_class_max_ts_tx = MS5_MAX_TS_TX;
      global_data_ptr->gmac_ms_class_max_ts_rx = MS5_MAX_TS_RX;
      global_data_ptr->gmac_ms_class_ts_tta = MS5_MIN_TS_TTA;
      global_data_ptr->gmac_ms_class_ts_ttb = MS5_MIN_TS_TTB;
      global_data_ptr->gmac_ms_class_ts_tra = MS5_MIN_TS_TRA;
      global_data_ptr->gmac_ms_class_ts_trb = MS5_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_6:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_6");
      global_data_ptr->gmac_ms_class = MULTISLOT_CLASS_6;
      global_data_ptr->gmac_ms_class_sum = MULTISLOT_CLASS_SUM_6;
      global_data_ptr->gmac_ms_class_max_ts_tx = MS6_MAX_TS_TX;
      global_data_ptr->gmac_ms_class_max_ts_rx = MS6_MAX_TS_RX;
      global_data_ptr->gmac_ms_class_ts_tta = MS6_MIN_TS_TTA;
      global_data_ptr->gmac_ms_class_ts_ttb = MS6_MIN_TS_TTB;
      global_data_ptr->gmac_ms_class_ts_tra = MS6_MIN_TS_TRA;
      global_data_ptr->gmac_ms_class_ts_trb = MS6_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_7:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_7");
      global_data_ptr->gmac_ms_class = MULTISLOT_CLASS_7;
      global_data_ptr->gmac_ms_class_sum = MULTISLOT_CLASS_SUM_7;
      global_data_ptr->gmac_ms_class_max_ts_tx = MS7_MAX_TS_TX;
      global_data_ptr->gmac_ms_class_max_ts_rx = MS7_MAX_TS_RX;
      global_data_ptr->gmac_ms_class_ts_tta = MS7_MIN_TS_TTA;
      global_data_ptr->gmac_ms_class_ts_ttb = MS7_MIN_TS_TTB;
      global_data_ptr->gmac_ms_class_ts_tra = MS7_MIN_TS_TRA;
      global_data_ptr->gmac_ms_class_ts_trb = MS7_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_8:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_8");
      global_data_ptr->gmac_ms_class = MULTISLOT_CLASS_8;
      global_data_ptr->gmac_ms_class_sum = MULTISLOT_CLASS_SUM_8;
      global_data_ptr->gmac_ms_class_max_ts_tx = MS8_MAX_TS_TX;
      global_data_ptr->gmac_ms_class_max_ts_rx = MS8_MAX_TS_RX;
      global_data_ptr->gmac_ms_class_ts_tta = MS8_MIN_TS_TTA;
      global_data_ptr->gmac_ms_class_ts_ttb = MS8_MIN_TS_TTB;
      global_data_ptr->gmac_ms_class_ts_tra = MS8_MIN_TS_TRA;
      global_data_ptr->gmac_ms_class_ts_trb = MS8_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_9:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_9");
      global_data_ptr->gmac_ms_class = MULTISLOT_CLASS_9;
      global_data_ptr->gmac_ms_class_sum = MULTISLOT_CLASS_SUM_9;
      global_data_ptr->gmac_ms_class_max_ts_tx = MS9_MAX_TS_TX;
      global_data_ptr->gmac_ms_class_max_ts_rx = MS9_MAX_TS_RX;
      global_data_ptr->gmac_ms_class_ts_tta = MS9_MIN_TS_TTA;
      global_data_ptr->gmac_ms_class_ts_ttb = MS9_MIN_TS_TTB;
      global_data_ptr->gmac_ms_class_ts_tra = MS9_MIN_TS_TRA;
      global_data_ptr->gmac_ms_class_ts_trb = MS9_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_10:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_10");
      global_data_ptr->gmac_ms_class = MULTISLOT_CLASS_10;
      global_data_ptr->gmac_ms_class_sum = MULTISLOT_CLASS_SUM_10;
      global_data_ptr->gmac_ms_class_max_ts_tx = MS10_MAX_TS_TX;
      global_data_ptr->gmac_ms_class_max_ts_rx = MS10_MAX_TS_RX;
      global_data_ptr->gmac_ms_class_ts_tta = MS10_MIN_TS_TTA;
      global_data_ptr->gmac_ms_class_ts_ttb = MS10_MIN_TS_TTB;
      global_data_ptr->gmac_ms_class_ts_tra = MS10_MIN_TS_TRA;
      global_data_ptr->gmac_ms_class_ts_trb = MS10_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_11:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_11");
      global_data_ptr->gmac_ms_class = MULTISLOT_CLASS_11;
      global_data_ptr->gmac_ms_class_sum = MULTISLOT_CLASS_SUM_11;
      global_data_ptr->gmac_ms_class_max_ts_tx = MS11_MAX_TS_TX;
      global_data_ptr->gmac_ms_class_max_ts_rx = MS11_MAX_TS_RX;
      global_data_ptr->gmac_ms_class_ts_tta = MS11_MIN_TS_TTA;
      global_data_ptr->gmac_ms_class_ts_ttb = MS11_MIN_TS_TTB;
      global_data_ptr->gmac_ms_class_ts_tra = MS11_MIN_TS_TRA;
      global_data_ptr->gmac_ms_class_ts_trb = MS11_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_12:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_12");
      global_data_ptr->gmac_ms_class = MULTISLOT_CLASS_12;
      global_data_ptr->gmac_ms_class_sum = MULTISLOT_CLASS_SUM_12;
      global_data_ptr->gmac_ms_class_max_ts_tx = MS12_MAX_TS_TX;
      global_data_ptr->gmac_ms_class_max_ts_rx = MS12_MAX_TS_RX;
      global_data_ptr->gmac_ms_class_ts_tta = MS12_MIN_TS_TTA;
      global_data_ptr->gmac_ms_class_ts_ttb = MS12_MIN_TS_TTB;
      global_data_ptr->gmac_ms_class_ts_tra = MS12_MIN_TS_TRA;
      global_data_ptr->gmac_ms_class_ts_trb = MS12_MIN_TS_TRB;
      break;

#ifdef FEATURE_GSM_GPRS_MSC33
    case RRPS_MULTISLOT_CLASS_30:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_30");
      global_data_ptr->gmac_ms_class = MULTISLOT_CLASS_30;
      global_data_ptr->gmac_ms_class_sum = MULTISLOT_CLASS_SUM_30;
      global_data_ptr->gmac_ms_class_max_ts_tx = MS30_MAX_TS_TX;
      global_data_ptr->gmac_ms_class_max_ts_rx = MS30_MAX_TS_RX;
      global_data_ptr->gmac_ms_class_ts_tta = MS30_MIN_TS_TTA;
      global_data_ptr->gmac_ms_class_ts_ttb = MS30_MIN_TS_TTB;
      global_data_ptr->gmac_ms_class_ts_tra = MS30_MIN_TS_TRA;
      global_data_ptr->gmac_ms_class_ts_trb = MS30_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_31:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_31");
      global_data_ptr->gmac_ms_class = MULTISLOT_CLASS_31;
      global_data_ptr->gmac_ms_class_sum = MULTISLOT_CLASS_SUM_31;
      global_data_ptr->gmac_ms_class_max_ts_tx = MS31_MAX_TS_TX;
      global_data_ptr->gmac_ms_class_max_ts_rx = MS31_MAX_TS_RX;
      global_data_ptr->gmac_ms_class_ts_tta = MS31_MIN_TS_TTA;
      global_data_ptr->gmac_ms_class_ts_ttb = MS31_MIN_TS_TTB;
      global_data_ptr->gmac_ms_class_ts_tra = MS31_MIN_TS_TRA;
      global_data_ptr->gmac_ms_class_ts_trb = MS31_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_32:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_32");
      global_data_ptr->gmac_ms_class = MULTISLOT_CLASS_32;
      global_data_ptr->gmac_ms_class_sum = MULTISLOT_CLASS_SUM_32;
      global_data_ptr->gmac_ms_class_max_ts_tx = MS32_MAX_TS_TX;
      global_data_ptr->gmac_ms_class_max_ts_rx = MS32_MAX_TS_RX;
      global_data_ptr->gmac_ms_class_ts_tta = MS32_MIN_TS_TTA;
      global_data_ptr->gmac_ms_class_ts_ttb = MS32_MIN_TS_TTB;
      global_data_ptr->gmac_ms_class_ts_tra = MS32_MIN_TS_TRA;
      global_data_ptr->gmac_ms_class_ts_trb = MS32_MIN_TS_TRB;
      break;

     case RRPS_MULTISLOT_CLASS_33:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_33");
      global_data_ptr->gmac_ms_class = MULTISLOT_CLASS_33;
      global_data_ptr->gmac_ms_class_sum = MULTISLOT_CLASS_SUM_33;
      global_data_ptr->gmac_ms_class_max_ts_tx = MS33_MAX_TS_TX;
      global_data_ptr->gmac_ms_class_max_ts_rx = MS33_MAX_TS_RX;
      global_data_ptr->gmac_ms_class_ts_tta = MS33_MIN_TS_TTA;
      global_data_ptr->gmac_ms_class_ts_ttb = MS33_MIN_TS_TTB;
      global_data_ptr->gmac_ms_class_ts_tra = MS33_MIN_TS_TRA;
      global_data_ptr->gmac_ms_class_ts_trb = MS33_MIN_TS_TRB;
      break;
#endif /*FEATURE_GSM_GPRS_MSC33*/

    default:
      MSG_GERAN_LOW_1_G("Multislot class NOT VALID %d",nv_ms_class);
      break;
  }

  rr_read_public_store( RRPS_MS_EGPRS_MULTISLOT_CLASS, &nv_egprs_ms_class );

  if ( (nv_egprs_ms_class >= RRPS_MULTISLOT_CLASS_1 ) &&
       (nv_egprs_ms_class <= upper_ms_class_limit)    )
  {
    MSG_GERAN_LOW_1_G("EGPRS multislot class %d",nv_egprs_ms_class);
  }
  else
  {
    nv_egprs_ms_class = RRPS_MULTISLOT_CLASS_1;
    MSG_GERAN_LOW_1_G("EGPRS multislot class default %d",nv_egprs_ms_class);
  }

  switch (nv_egprs_ms_class)
  {
    case RRPS_MULTISLOT_CLASS_1:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_1");
      global_data_ptr->gmac_egprs_ms_class = MULTISLOT_CLASS_1;
      global_data_ptr->gmac_egprs_ms_class_sum = MULTISLOT_CLASS_SUM_1;
      global_data_ptr->gmac_egprs_ms_class_max_ts_tx = MS1_MAX_TS_TX;
      global_data_ptr->gmac_egprs_ms_class_max_ts_rx = MS1_MAX_TS_RX;
      global_data_ptr->gmac_egprs_ms_class_ts_tta = MS1_MIN_TS_TTA;
      global_data_ptr->gmac_egprs_ms_class_ts_ttb = MS1_MIN_TS_TTB;
      global_data_ptr->gmac_egprs_ms_class_ts_tra = MS1_MIN_TS_TRA;
      global_data_ptr->gmac_egprs_ms_class_ts_trb = MS1_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_2:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_2");
      global_data_ptr->gmac_egprs_ms_class = MULTISLOT_CLASS_2;
      global_data_ptr->gmac_egprs_ms_class_sum = MULTISLOT_CLASS_SUM_2;
      global_data_ptr->gmac_egprs_ms_class_max_ts_tx = MS2_MAX_TS_TX;
      global_data_ptr->gmac_egprs_ms_class_max_ts_rx = MS2_MAX_TS_RX;
      global_data_ptr->gmac_egprs_ms_class_ts_tta = MS2_MIN_TS_TTA;
      global_data_ptr->gmac_egprs_ms_class_ts_ttb = MS2_MIN_TS_TTB;
      global_data_ptr->gmac_egprs_ms_class_ts_tra = MS2_MIN_TS_TRA;
      global_data_ptr->gmac_egprs_ms_class_ts_trb = MS2_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_3:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_3");
      global_data_ptr->gmac_egprs_ms_class = MULTISLOT_CLASS_3;
      global_data_ptr->gmac_egprs_ms_class_sum = MULTISLOT_CLASS_SUM_3;
      global_data_ptr->gmac_egprs_ms_class_max_ts_tx = MS3_MAX_TS_TX;
      global_data_ptr->gmac_egprs_ms_class_max_ts_rx = MS3_MAX_TS_RX;
      global_data_ptr->gmac_egprs_ms_class_ts_tta = MS3_MIN_TS_TTA;
      global_data_ptr->gmac_egprs_ms_class_ts_ttb = MS3_MIN_TS_TTB;
      global_data_ptr->gmac_egprs_ms_class_ts_tra = MS3_MIN_TS_TRA;
      global_data_ptr->gmac_egprs_ms_class_ts_trb = MS3_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_4:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_4");
      global_data_ptr->gmac_egprs_ms_class = MULTISLOT_CLASS_4;
      global_data_ptr->gmac_egprs_ms_class_sum = MULTISLOT_CLASS_SUM_4;
      global_data_ptr->gmac_egprs_ms_class_max_ts_tx = MS4_MAX_TS_TX;
      global_data_ptr->gmac_egprs_ms_class_max_ts_rx = MS4_MAX_TS_RX;
      global_data_ptr->gmac_egprs_ms_class_ts_tta = MS4_MIN_TS_TTA;
      global_data_ptr->gmac_egprs_ms_class_ts_ttb = MS4_MIN_TS_TTB;
      global_data_ptr->gmac_egprs_ms_class_ts_tra = MS4_MIN_TS_TRA;
      global_data_ptr->gmac_egprs_ms_class_ts_trb = MS4_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_5:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_5");
      global_data_ptr->gmac_egprs_ms_class = MULTISLOT_CLASS_5;
      global_data_ptr->gmac_egprs_ms_class_sum = MULTISLOT_CLASS_SUM_5;
      global_data_ptr->gmac_egprs_ms_class_max_ts_tx = MS5_MAX_TS_TX;
      global_data_ptr->gmac_egprs_ms_class_max_ts_rx = MS5_MAX_TS_RX;
      global_data_ptr->gmac_egprs_ms_class_ts_tta = MS5_MIN_TS_TTA;
      global_data_ptr->gmac_egprs_ms_class_ts_ttb = MS5_MIN_TS_TTB;
      global_data_ptr->gmac_egprs_ms_class_ts_tra = MS5_MIN_TS_TRA;
      global_data_ptr->gmac_egprs_ms_class_ts_trb = MS5_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_6:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_6");
      global_data_ptr->gmac_egprs_ms_class = MULTISLOT_CLASS_6;
      global_data_ptr->gmac_egprs_ms_class_sum = MULTISLOT_CLASS_SUM_6;
      global_data_ptr->gmac_egprs_ms_class_max_ts_tx = MS6_MAX_TS_TX;
      global_data_ptr->gmac_egprs_ms_class_max_ts_rx = MS6_MAX_TS_RX;
      global_data_ptr->gmac_egprs_ms_class_ts_tta = MS6_MIN_TS_TTA;
      global_data_ptr->gmac_egprs_ms_class_ts_ttb = MS6_MIN_TS_TTB;
      global_data_ptr->gmac_egprs_ms_class_ts_tra = MS6_MIN_TS_TRA;
      global_data_ptr->gmac_egprs_ms_class_ts_trb = MS6_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_7:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_7");
      global_data_ptr->gmac_egprs_ms_class = MULTISLOT_CLASS_7;
      global_data_ptr->gmac_egprs_ms_class_sum = MULTISLOT_CLASS_SUM_7;
      global_data_ptr->gmac_egprs_ms_class_max_ts_tx = MS7_MAX_TS_TX;
      global_data_ptr->gmac_egprs_ms_class_max_ts_rx = MS7_MAX_TS_RX;
      global_data_ptr->gmac_egprs_ms_class_ts_tta = MS7_MIN_TS_TTA;
      global_data_ptr->gmac_egprs_ms_class_ts_ttb = MS7_MIN_TS_TTB;
      global_data_ptr->gmac_egprs_ms_class_ts_tra = MS7_MIN_TS_TRA;
      global_data_ptr->gmac_egprs_ms_class_ts_trb = MS7_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_8:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_8");
      global_data_ptr->gmac_egprs_ms_class = MULTISLOT_CLASS_8;
      global_data_ptr->gmac_egprs_ms_class_sum = MULTISLOT_CLASS_SUM_8;
      global_data_ptr->gmac_egprs_ms_class_max_ts_tx = MS8_MAX_TS_TX;
      global_data_ptr->gmac_egprs_ms_class_max_ts_rx = MS8_MAX_TS_RX;
      global_data_ptr->gmac_egprs_ms_class_ts_tta = MS8_MIN_TS_TTA;
      global_data_ptr->gmac_egprs_ms_class_ts_ttb = MS8_MIN_TS_TTB;
      global_data_ptr->gmac_egprs_ms_class_ts_tra = MS8_MIN_TS_TRA;
      global_data_ptr->gmac_egprs_ms_class_ts_trb = MS8_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_9:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_9");
      global_data_ptr->gmac_egprs_ms_class = MULTISLOT_CLASS_9;
      global_data_ptr->gmac_egprs_ms_class_sum = MULTISLOT_CLASS_SUM_9;
      global_data_ptr->gmac_egprs_ms_class_max_ts_tx = MS9_MAX_TS_TX;
      global_data_ptr->gmac_egprs_ms_class_max_ts_rx = MS9_MAX_TS_RX;
      global_data_ptr->gmac_egprs_ms_class_ts_tta = MS9_MIN_TS_TTA;
      global_data_ptr->gmac_egprs_ms_class_ts_ttb = MS9_MIN_TS_TTB;
      global_data_ptr->gmac_egprs_ms_class_ts_tra = MS9_MIN_TS_TRA;
      global_data_ptr->gmac_egprs_ms_class_ts_trb = MS9_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_10:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_10");
      global_data_ptr->gmac_egprs_ms_class = MULTISLOT_CLASS_10;
      global_data_ptr->gmac_egprs_ms_class_sum = MULTISLOT_CLASS_SUM_10;
      global_data_ptr->gmac_egprs_ms_class_max_ts_tx = MS10_MAX_TS_TX;
      global_data_ptr->gmac_egprs_ms_class_max_ts_rx = MS10_MAX_TS_RX;
      global_data_ptr->gmac_egprs_ms_class_ts_tta = MS10_MIN_TS_TTA;
      global_data_ptr->gmac_egprs_ms_class_ts_ttb = MS10_MIN_TS_TTB;
      global_data_ptr->gmac_egprs_ms_class_ts_tra = MS10_MIN_TS_TRA;
      global_data_ptr->gmac_egprs_ms_class_ts_trb = MS10_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_11:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_11");
      global_data_ptr->gmac_egprs_ms_class = MULTISLOT_CLASS_11;
      global_data_ptr->gmac_egprs_ms_class_sum = MULTISLOT_CLASS_SUM_11;
      global_data_ptr->gmac_egprs_ms_class_max_ts_tx = MS11_MAX_TS_TX;
      global_data_ptr->gmac_egprs_ms_class_max_ts_rx = MS11_MAX_TS_RX;
      global_data_ptr->gmac_egprs_ms_class_ts_tta = MS11_MIN_TS_TTA;
      global_data_ptr->gmac_egprs_ms_class_ts_ttb = MS11_MIN_TS_TTB;
      global_data_ptr->gmac_egprs_ms_class_ts_tra = MS11_MIN_TS_TRA;
      global_data_ptr->gmac_egprs_ms_class_ts_trb = MS11_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_12:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_12");
      global_data_ptr->gmac_egprs_ms_class = MULTISLOT_CLASS_12;
      global_data_ptr->gmac_egprs_ms_class_sum = MULTISLOT_CLASS_SUM_12;
      global_data_ptr->gmac_egprs_ms_class_max_ts_tx = MS12_MAX_TS_TX;
      global_data_ptr->gmac_egprs_ms_class_max_ts_rx = MS12_MAX_TS_RX;
      global_data_ptr->gmac_egprs_ms_class_ts_tta = MS12_MIN_TS_TTA;
      global_data_ptr->gmac_egprs_ms_class_ts_ttb = MS12_MIN_TS_TTB;
      global_data_ptr->gmac_egprs_ms_class_ts_tra = MS12_MIN_TS_TRA;
      global_data_ptr->gmac_egprs_ms_class_ts_trb = MS12_MIN_TS_TRB;
      break;

#ifdef FEATURE_GSM_GPRS_MSC33
    case RRPS_MULTISLOT_CLASS_30:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_30");
      global_data_ptr->gmac_egprs_ms_class = MULTISLOT_CLASS_30;
      global_data_ptr->gmac_egprs_ms_class_sum = MULTISLOT_CLASS_SUM_30;
      global_data_ptr->gmac_egprs_ms_class_max_ts_tx = MS30_MAX_TS_TX;
      global_data_ptr->gmac_egprs_ms_class_max_ts_rx = MS30_MAX_TS_RX;
      global_data_ptr->gmac_egprs_ms_class_ts_tta = MS30_MIN_TS_TTA;
      global_data_ptr->gmac_egprs_ms_class_ts_ttb = MS30_MIN_TS_TTB;
      global_data_ptr->gmac_egprs_ms_class_ts_tra = MS30_MIN_TS_TRA;
      global_data_ptr->gmac_egprs_ms_class_ts_trb = MS30_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_31:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_31");
      global_data_ptr->gmac_egprs_ms_class = MULTISLOT_CLASS_31;
      global_data_ptr->gmac_egprs_ms_class_sum = MULTISLOT_CLASS_SUM_31;
      global_data_ptr->gmac_egprs_ms_class_max_ts_tx = MS31_MAX_TS_TX;
      global_data_ptr->gmac_egprs_ms_class_max_ts_rx = MS31_MAX_TS_RX;
      global_data_ptr->gmac_egprs_ms_class_ts_tta = MS31_MIN_TS_TTA;
      global_data_ptr->gmac_egprs_ms_class_ts_ttb = MS31_MIN_TS_TTB;
      global_data_ptr->gmac_egprs_ms_class_ts_tra = MS31_MIN_TS_TRA;
      global_data_ptr->gmac_egprs_ms_class_ts_trb = MS31_MIN_TS_TRB;
      break;

    case RRPS_MULTISLOT_CLASS_32:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_32");
      global_data_ptr->gmac_egprs_ms_class = MULTISLOT_CLASS_32;
      global_data_ptr->gmac_egprs_ms_class_sum = MULTISLOT_CLASS_SUM_32;
      global_data_ptr->gmac_egprs_ms_class_max_ts_tx = MS32_MAX_TS_TX;
      global_data_ptr->gmac_egprs_ms_class_max_ts_rx = MS32_MAX_TS_RX;
      global_data_ptr->gmac_egprs_ms_class_ts_tta = MS32_MIN_TS_TTA;
      global_data_ptr->gmac_egprs_ms_class_ts_ttb = MS32_MIN_TS_TTB;
      global_data_ptr->gmac_egprs_ms_class_ts_tra = MS32_MIN_TS_TRA;
      global_data_ptr->gmac_egprs_ms_class_ts_trb = MS32_MIN_TS_TRB;
      break;

     case RRPS_MULTISLOT_CLASS_33:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_33");
      global_data_ptr->gmac_egprs_ms_class = MULTISLOT_CLASS_33;
      global_data_ptr->gmac_egprs_ms_class_sum = MULTISLOT_CLASS_SUM_33;
      global_data_ptr->gmac_egprs_ms_class_max_ts_tx = MS33_MAX_TS_TX;
      global_data_ptr->gmac_egprs_ms_class_max_ts_rx = MS33_MAX_TS_RX;
      global_data_ptr->gmac_egprs_ms_class_ts_tta = MS33_MIN_TS_TTA;
      global_data_ptr->gmac_egprs_ms_class_ts_ttb = MS33_MIN_TS_TTB;
      global_data_ptr->gmac_egprs_ms_class_ts_tra = MS33_MIN_TS_TRA;
      global_data_ptr->gmac_egprs_ms_class_ts_trb = MS33_MIN_TS_TRB;
      break;
#endif /*FEATURE_GSM_GPRS_MSC33*/

    default:
      MSG_GERAN_LOW_1_G("Multislot class NOT VALID %d",nv_egprs_ms_class);
      break;
  }

#ifdef FEATURE_GSM_DTM
  rr_read_public_store( RRPS_MS_DTM_EGPRS_MULTISLOT_SUBCLASS, &nv_dtm_ms_class );

  if ( (nv_dtm_ms_class == RRPS_DTM_EGPRS_MULTISLOT_SUBCLASS_5 ) ||
       (nv_dtm_ms_class == RRPS_DTM_EGPRS_MULTISLOT_SUBCLASS_9 ) ||
       (nv_dtm_ms_class == RRPS_DTM_EGPRS_MULTISLOT_SUBCLASS_11) )
  {
    MSG_GERAN_LOW_1_G("GMAC DTM multislot class %d",nv_dtm_ms_class);
  }
  else
  {
    nv_dtm_ms_class = RRPS_DTM_EGPRS_MULTISLOT_SUBCLASS_5;
    MSG_GERAN_LOW_1_G("GMAC DTM multislot class out of range, default %d",nv_dtm_ms_class);
  }

  switch (nv_dtm_ms_class)
  {
    case RRPS_DTM_EGPRS_MULTISLOT_SUBCLASS_5:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_5");
      global_data_ptr->gmac_dtm_ms_class_sum = MULTISLOT_CLASS_SUM_5;
      global_data_ptr->gmac_dtm_ms_class_max_ts_tx = MS5_MAX_TS_TX;
      global_data_ptr->gmac_dtm_ms_class_max_ts_rx = MS5_MAX_TS_RX;
      global_data_ptr->gmac_dtm_ms_class_ts_tta = MS5_MIN_TS_TTA;
      global_data_ptr->gmac_dtm_ms_class_ts_ttb = MS5_MIN_TS_TTB;
      global_data_ptr->gmac_dtm_ms_class_ts_tra = MS5_MIN_TS_TRA;
      global_data_ptr->gmac_dtm_ms_class_ts_trb = MS5_MIN_TS_TRB;
      break;

    case RRPS_DTM_EGPRS_MULTISLOT_SUBCLASS_9:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_9");
      global_data_ptr->gmac_dtm_ms_class_sum = MULTISLOT_CLASS_SUM_9;
      global_data_ptr->gmac_dtm_ms_class_max_ts_tx = MS9_MAX_TS_TX;
      global_data_ptr->gmac_dtm_ms_class_max_ts_rx = MS9_MAX_TS_RX;
      global_data_ptr->gmac_dtm_ms_class_ts_tta = MS9_MIN_TS_TTA;
      global_data_ptr->gmac_dtm_ms_class_ts_ttb = MS9_MIN_TS_TTB;
      global_data_ptr->gmac_dtm_ms_class_ts_tra = MS9_MIN_TS_TRA;
      global_data_ptr->gmac_dtm_ms_class_ts_trb = MS9_MIN_TS_TRB;
      break;

    case RRPS_DTM_EGPRS_MULTISLOT_SUBCLASS_11:
      MSG_GERAN_LOW_0_G("MAC info MULTISLOT_CLASS_11");
      global_data_ptr->gmac_dtm_ms_class_sum = MULTISLOT_CLASS_SUM_11;
      global_data_ptr->gmac_dtm_ms_class_max_ts_tx = MS11_MAX_TS_TX;
      global_data_ptr->gmac_dtm_ms_class_max_ts_rx = MS11_MAX_TS_RX;
      global_data_ptr->gmac_dtm_ms_class_ts_tta = MS11_MIN_TS_TTA;
      global_data_ptr->gmac_dtm_ms_class_ts_ttb = MS11_MIN_TS_TTB;
      global_data_ptr->gmac_dtm_ms_class_ts_tra = MS11_MIN_TS_TRA;
      global_data_ptr->gmac_dtm_ms_class_ts_trb = MS11_MIN_TS_TRB;
      break;

    default:
      MSG_GERAN_ERROR_1_G("Multislot class NOT VALID %d",nv_dtm_ms_class);
      break;
  }
#endif /*FEATURE_GSM_DTM*/
}

/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/
/*===========================================================================
===
===  FUNCTION      gmac_gprs_dl_ctrl_msg_proceed_validation()
===
===  DESCRIPTION
===    This function will return whether the received control message should pass
===    it to the main processing or not
===
===  DEPENDENCIES
===    This function access mac_mode variable
===
===  RETURN VALUE
===    boolean
===
===  SIDE EFFECTS
===  This function is called only from gprs_mac_signal_handler function
===
===========================================================================*/
static boolean gmac_gprs_dl_ctrl_msg_proceed_validation(uint8 message_type, boolean proceed_flag)
{
  boolean mac_to_process_msg_flg = TRUE;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr();

  if(proceed_flag == FALSE)
  {
    mac_to_process_msg_flg = FALSE;
  }
  switch(message_type)
  {
    case PACKET_PSI1:
    case PACKET_PSI2:
    case PACKET_PSI3:
    case PACKET_PSI3_BIS:
    case PACKET_PSI4:
    case PACKET_PSI5:
    case PACKET_PSI6:
    case PACKET_PSI13:
    case PACKET_PSI7:
    case PACKET_PSI8:
    case PACKET_PSI14:
    case PACKET_PSI3_QUATER:
    case PACKET_PSI15:
      /* no need to enter main processing loop */
      mac_to_process_msg_flg = FALSE;
    break;

#ifdef FEATURE_GPRS_PS_HANDOVER
    case PACKET_PS_HANDOVER:
#ifdef PERLUTF
      #error code not present
#endif
    break;
#endif /* FEATURE_GPRS_PS_HANDOVER */

    case PACKET_SERVING_CELL_DATA:
      /* no need to enter main processing loop */
      mac_to_process_msg_flg = FALSE;
    break;

    case PACKET_SERVING_CELL_SI:
      /* no need to enter main processing loop */
      mac_to_process_msg_flg = FALSE;
    break;

    case PACKET_CELL_CHANGE_CONTINUE:
      mac_to_process_msg_flg = FALSE;
    break;

    case PACKET_NEIGHBOUR_CELL_DATA:
      /* no need to enter main processing loop */
      mac_to_process_msg_flg = FALSE;
    break;

    case PACKET_DOWNLINK_DUMMY_CNTRL_BLK:
      if((global_data_ptr->mac_mode == IDLE_MODE) || (global_data_ptr->mac_mode == TRANSFER_MODE))
      {
        /* no need to enter main processing loop */
        mac_to_process_msg_flg = FALSE;
      }
    break;

    default:
    break;
  }
  return(mac_to_process_msg_flg);
}

/*===========================================================================
===
===  FUNCTION      gmac_gprs_dl_ctrl_msg_validation()
===
===  DESCRIPTION
===    This function validates the received dl control message and if the
===    message is for GRR then it forwards the message to GRR
===
===  DEPENDENCIES
===    This function access mac_mode variable
===
===  RETURN VALUE
===    void
===
===  SIDE EFFECTS
===  This functio is called only from gprs_mac_signal_handler function
===
===========================================================================*/
static void gmac_gprs_dl_ctrl_msg_validation(uint8 message_type)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr();

  switch(message_type)
  {
    case PACKET_PSI1:
    case PACKET_PSI2:
    case PACKET_PSI3:
    case PACKET_PSI3_BIS:
    case PACKET_PSI4:
    case PACKET_PSI5:
    case PACKET_PSI6:
    case PACKET_PSI13:
    case PACKET_PSI7:
    case PACKET_PSI8:
    case PACKET_PSI14:
    case PACKET_PSI3_QUATER:
    case PACKET_PSI15:

      /* send GRR the PSI message, GRR will free CSN.1  dl ctrl msg memory */
      MSG_GERAN_LOW_0_G("MAC Rcvd PSI");
      gmac_send_data_ind_to_grr();
      break;

    case PACKET_CELL_CHANGE_ORDER:
    case PACKET_MEASUREMENT_ORDER:
#ifdef FEATURE_GPRS_PS_HANDOVER
    case PACKET_PS_HANDOVER:
#endif /* FEATURE_GPRS_PS_HANDOVER */
#ifdef FEATURE_GSM_EDTM
    case PACKET_CS_COMMAND:
#endif /*FEATURE_GSM_EDTM*/
    /* Message to GRR is sent from state machine handler */
      break;

    case PACKET_SERVING_CELL_DATA:
      gmac_util_handle_pkt_cell_data(PACKET_SERVING_CELL_DATA,
                                     global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.bcch_arfcn);
      break;

    case PACKET_SERVING_CELL_SI:
      gmac_util_send_serving_si(global_data_ptr->csn_dec_result_ptr,
                                global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.bcch_arfcn);
      break;

    case PACKET_CELL_CHANGE_CONTINUE:
      /* if the TFI matches, forward the PCCC message to GRR */
      if (gmac_util_check_tfi(PACKET_CELL_CHANGE_CONTINUE))
      {
        MSG_GERAN_MED_0_G("MAC Rcvd PACKET_CELL_CHANGE_CONTINUE");
        if (global_data_ptr->mac_mode == TRANSFER_MODE)
        {
          if ( gmac_tran_get_tbfs_active() == DL_ONLY )
          {
            /*Only a DL TBF is active so set the the PCA type to DL.
              L1 will put this into its DL PCA queue and will gaurantee
              that it will be transmitted when we ab rel for the cell
              resel that follows the PCCC*/
            rrbp_control_block_handler_2(PCA_DL_MSG);
          }
          else
          {
            rrbp_control_block_handler_2(PCA_UL_MSG);
          }
        }
        gmac_send_data_ind_to_grr();
      }
      break;

    case PACKET_NEIGHBOUR_CELL_DATA:
      gmac_util_handle_pkt_cell_data(PACKET_NEIGHBOUR_CELL_DATA,
                                     global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.bcch_arfcn);
      break;

    case PACKET_DOWNLINK_DUMMY_CNTRL_BLK:
      break;

    default:
      break;
  }

  return;
}
/*===========================================================================
===
===  FUNCTION      GPRS_MAC_SIGNAL_HANDLER_DYN()
===
===  DESCRIPTION
===
===    Retrieves signals form MAC signal queue and uses MAC mode to determine
===    which mode the signals are destined for.
===
===  DEPENDENCIES
===
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===========================================================================*/
void gprs_mac_signal_handler_dyn(void)
{
  boolean       segmented = FALSE;
  uint8         signal_id;
  uint8         task_id;
  boolean       proceed_flag = TRUE;

  /*TLX free floating*/
  uint8         compare_task_id = MS_MAC_MAC;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr();

  /*search MAC q for MAC internal signals and process these 1st.                  */
  /*q_linear_search does not remove the sig from the q just returns the pointer   */
  /*If requested item is found then q_linear_search returns the pointer to it     */
  /*otherwise it returns NULL                                                     */

  global_data_ptr->msg_ptr = (mac_signals_u*) (q_linear_search (&global_data_ptr->mac_q,(q_compare_func_type)gmac_check_q_for_mac_internal_sigs,
                                               &compare_task_id));

  /*if MAC internal signal was found fetch it off the Q 1st */
  if(global_data_ptr->msg_ptr == NULL)
  {
    /*No intrenal MAC signal found on Q. Retrieve 1st signal on q*/
    global_data_ptr->msg_ptr = q_get(&global_data_ptr->mac_q);
  }
  else
  {
    /*internal MAC signal found on Q. delete mac sig from q as q_linear_search does not do this.*/


    gmac_log_reshuffling_of_internal_sigs();

    q_delete(
              &global_data_ptr->mac_q,
              &(global_data_ptr->msg_ptr->mac_int_sig.link));
  }


/*tlx free floating */
//was  /* gets message and clears it from message queue */
//was   msg_ptr = q_get(&mac_q);

  while(global_data_ptr->msg_ptr != NULL)
  {
    task_id = global_data_ptr->msg_ptr->grr_mac_sig.task_id;
    if (task_id == MS_MAC_L1)
    {
      signal_id = (uint8)global_data_ptr->msg_ptr->l1_mac_sig.sig_id;
    }
    else if (task_id == MS_MAC_RR)
    {
      signal_id = (uint8)global_data_ptr->msg_ptr->grr_mac_sig.sig_id;


/*page mode hadling moved to  NUll state NULL_INACTIVE for IPA*/
//      /* set page mode to re-org when changing cell */
/*      if ((signal_id == (uint8)GRR_MAC_CAMPED_ON_CELL) ||
          (signal_id == (uint8)GRR_MAC_PCCO_CAMPED_ON_CELL))
      {
         gmac_local_current_page_mode = PAGING_REORGANIZATION;
      }*/
      if (signal_id == (uint8)GRR_MAC_NACC_RELEASE_REQ)
      {
        /* handle GRR request for release of NACC storage */
        gmac_nacc_respond_to_release_req(global_data_ptr->msg_ptr);
        proceed_flag = FALSE;
      }

      if (signal_id == (uint8)GRR_MAC_RESET_REQ)
      {
        if ( global_data_ptr->mac_mode == TRANSFER_MODE )
        {
          /* This function free's the PDAN memory, this is done before the complete reset
             because we only only know that we're in Transfer mode here*/
          gmac_transfer_dl_release_clean();
        }

        gmac_set_mac_state_for_reset();

        gmac_util_send_ul_reset_req_to_rlc();
        gmac_util_send_dl_reset_req_to_rlc();
      }
/*event logging changes*/
      mac_log_grr_events(signal_id);

    }
    else if (task_id == MS_MAC_RLC)
    {
      signal_id = (uint8)global_data_ptr->msg_ptr->rlc_mac_sig.msg.sig;
    }
    else if (task_id == MS_MAC_MAC)
    {
      signal_id = (uint8)global_data_ptr->msg_ptr->mac_int_sig.sig_id;
    }
    else
    {
      GPRS_MEM_FREE(global_data_ptr->msg_ptr);
      MSG_GERAN_LOW_1_G("MAC error TASK ID unknown: 0x%x",task_id);
      return;
    }

    /* Check task and signal id's before they get changed
    ** and set free memory flag accordingly.
    ** This applies to signals from L1 ISR layer.
    */

    /********************************************************************/

    if(signal_id == (uint8)MAC_PH_DATA_IND && task_id == MS_MAC_L1)
    {
      /* retrieve dl logical channel msg and perform RRBP functionality */
      segmented = rb_block_handler();

      if (segmented == FALSE)
      {
        if (proceed_flag == TRUE)
        {
          /* Check "Page Mode" of all DL control messages in idle mode only */
          if ((global_data_ptr->mac_mode == IDLE_MODE) || (global_data_ptr->mac_mode == NULL_MODE))
          {
            /*MAC will process the page mode only if the mode is idle or null mode
              The reason for processing page mode in null is,
              In the case where we have camped on a 52 cell and there is no cell access.
              Under these conditions GRR will not see GRR_MAC_PS_ACCESS because the cell is
              barred. However L1 has is camped onto a PBCCH and will forward PH_DATA_IND's
              (ie PSI) to MAC. MS is obliged to monitor the page mode in the rcvd control
              msg and update L1 with changes in page mode. To accommodate this gmac has to
              monitor the page mode in NULL mode also
              Page mode initialization will be done only when gmac process PS_ACCESS IND in
              null mode*/
            gmac_util_page_mode_handler();
          }

          /*Check whether we need to enter into the main processing loop or not*/
          proceed_flag = gmac_gprs_dl_ctrl_msg_proceed_validation(global_data_ptr->csn_dec_hdr_ptr->message_type,proceed_flag);
        }

        /* validate DL control message */
        gmac_gprs_dl_ctrl_msg_validation(global_data_ptr->csn_dec_hdr_ptr->message_type);
      }
      else
      {
        /* segmented message so avoid entering MAC state machine */
        proceed_flag = FALSE;
      }
      }

#ifdef DEBUG_GSM_GPRS_DS_UNIT_TEST
    #error code not present
#endif

    if(proceed_flag == TRUE)
    {
      /*filter out all FIXED assigments*/
      if (gmac_assignment_unsupported(global_data_ptr->mac_mode,signal_id,task_id) == FALSE)
      {
        switch(global_data_ptr->mac_mode)
        {
          case NULL_MODE:
            gmac_store_trace_rvcd_msg_buf((uint8)global_data_ptr->mac_mode, (uint8)gmac_null_get_null_state());
            null_mode_handler(task_id, signal_id);
            break;

          case IDLE_MODE:
            if(gmac_dl_msg_address_data_valid(signal_id,task_id) == TRUE)
            {
              gmac_store_trace_rvcd_msg_buf((uint8)global_data_ptr->mac_mode, (uint8)gmac_idle_get_idle_state());
              idle_mode_handler(task_id, signal_id);
            }
            break;

          case TRANSFER_MODE:
            gmac_store_trace_rvcd_msg_buf((uint8)global_data_ptr->mac_mode, (uint8)gmac_transfer_get_transfer_state());
            transfer_mode_handler(task_id, signal_id);
            break;

          default:
            break;
        }
      }
      else
      {
        MSG_GERAN_LOW_0_G(" RCVD fixed alloc in assigment msg. Not supported");
      }
    }
    else
    {
      /* reset segmented indicator before checking next signal in queue */
      segmented = FALSE;
      proceed_flag = TRUE;
    }

    /* free memory allocated by CSN.1 decoder */
    if (global_data_ptr->csn_mem_free_pending)
    {
      free_csn_decoder_ptrs_t(global_data_ptr->csn_dec_result_ptr);
      global_data_ptr->csn_mem_free_pending = FALSE;
    }

    GPRS_MEM_FREE(global_data_ptr->msg_ptr);

    /* check message queue for more signals. if msg_ptr returns NULL queue is empty */
    global_data_ptr->msg_ptr = (mac_signals_u*) (q_linear_search (&global_data_ptr->mac_q,(q_compare_func_type)gmac_check_q_for_mac_internal_sigs,
                                                 &compare_task_id));

    /*if MAC internal signal was found fetch it off the Q 1st */
    if(global_data_ptr->msg_ptr == NULL)
    {
      /*No intrenal MAC signal found on Q. Retrieve 1st signal on q*/
      global_data_ptr->msg_ptr = q_get(&global_data_ptr->mac_q);
    }
    else
    {
      /*internal MAC signal found on Q. delete mac sig from q as q_linear_search does not do this.*/

      gmac_log_reshuffling_of_internal_sigs();

      q_delete(
                &global_data_ptr->mac_q,
                &(global_data_ptr->msg_ptr->mac_int_sig.link));
    }
  }

  /*clear GPRS_MAC_MASTER_SIG mask */

  GMAC_MSG_LOCK();

  if ( q_cnt(&global_data_ptr->mac_q) == 0 )
    (void)rex_clr_sigs(rex_self(), GPRS_MAC_MASTER_SIG);

  GMAC_MSG_UNLOCK();
}

/*===========================================================================
===
===  FUNCTION      gmac_store_trace_rvcd_msg_buf()
===
===  DESCRIPTION
===
===    This function stores the received signal messasge by GMAC in gmac_trace_msg_buf.
===    This function will also increment gmac_trace_msg_buf_index
===
===  DEPENDENCIES
===
===  gmac_trace_msg_buf, gmac_trace_msg_buf_index
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===========================================================================*/
static void gmac_store_trace_rvcd_msg_buf(uint8 gmac_mode, uint8 state)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr();
  gmac_task_data_t *task_data_ptr = gmac_get_task_data_ptr();
  uint32 current_fn = GSTMR_GET_FN();

  task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].next_entry = FALSE;

  task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].FN = current_fn;

  task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].gmac_mode = gmac_mode;

  task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].state = state;

  memset(task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].message_frag, 0, sizeof(task_data_ptr->gmac_trace_msg_buf[0].message_frag));

  if(global_data_ptr->msg_ptr->grr_mac_sig.task_id == MS_MAC_L1)
  {
    /* Do not store messages which are not used in debugging stability issues */
    if ((global_data_ptr->msg_ptr->l1_mac_sig.sig_id == L1_MAC_USF_DETECTED) ||
        (global_data_ptr->msg_ptr->l1_mac_sig.sig_id == MAC_PH_DATA_IND))
    {
      return;
    }
    
    task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].message_set = MS_MAC_L1;
    task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].message_id = (byte)global_data_ptr->msg_ptr->l1_mac_sig.sig_id;

    /*Grab 3 Bytes of Message */
    (void) memscpy(task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].message_frag,
                   sizeof(task_data_ptr->gmac_trace_msg_buf[0].message_frag),
                   &(global_data_ptr->msg_ptr->l1_mac_sig.msg),
                   sizeof(task_data_ptr->gmac_trace_msg_buf[0].message_frag)
                  );
  }
  else if(global_data_ptr->msg_ptr->grr_mac_sig.task_id == MS_MAC_RR)
  {
    task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].message_set = MS_MAC_RR;
    task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].message_id = (byte)global_data_ptr->msg_ptr->grr_mac_sig.sig_id;
    /*Grab 3 Bytes of Message */
    (void) memscpy(task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].message_frag,
                   sizeof(task_data_ptr->gmac_trace_msg_buf[0].message_frag),
                   &(global_data_ptr->msg_ptr->grr_mac_sig.msg),
                   sizeof(task_data_ptr->gmac_trace_msg_buf[0].message_frag)
                  );
  }
  else if(global_data_ptr->msg_ptr->grr_mac_sig.task_id == MS_MAC_RLC)
  {
    task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].message_set = MS_MAC_RLC;
    task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].message_id = (byte)global_data_ptr->msg_ptr->rlc_mac_sig.msg.sig;
    /*Grab 3 Bytes of Message */
    (void) memscpy(task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].message_frag,
                   sizeof(task_data_ptr->gmac_trace_msg_buf[0].message_frag),
                   &(global_data_ptr->msg_ptr->rlc_mac_sig.msg),
                   sizeof(task_data_ptr->gmac_trace_msg_buf[0].message_frag)
               );
  }
  else if(global_data_ptr->msg_ptr->grr_mac_sig.task_id == MS_MAC_MAC)
  {
    task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].message_set = MS_MAC_MAC;
    task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].message_id = (byte)global_data_ptr->msg_ptr->mac_int_sig.sig_id;
  }

  task_data_ptr->gmac_trace_msg_buf_index++;

  if (task_data_ptr->gmac_trace_msg_buf_index >= GMAC_NUM_TRACE_MSGS)
  {
    task_data_ptr->gmac_trace_msg_buf_index = 0;
  }
  task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].next_entry = TRUE;
}

/*===========================================================================
===
===  FUNCTION      gmac_store_trace_send_msg_buf_dyn()
===
===  DESCRIPTION
===
===    This function stores the send signal message by GMAC in gmac_trace_msg_buf.
===    This function will also increment gmac_trace_msg_buf_index
===
===  DEPENDENCIES
===
===  gmac_trace_msg_buf, gmac_trace_msg_buf_index
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===========================================================================*/
void gmac_store_trace_send_msg_buf_dyn(uint8 message_set, void* message_ptr)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr();
  gmac_task_data_t *task_data_ptr = gmac_get_task_data_ptr();
  uint32 current_fn = GSTMR_GET_FN();

  task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].next_entry = FALSE;

  task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].FN = current_fn;

  task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].gmac_mode = (byte)global_data_ptr->mac_mode;

  memset(task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].message_frag, 0, sizeof(task_data_ptr->gmac_trace_msg_buf[0].message_frag));

  if(global_data_ptr->mac_mode == NULL_MODE)
  {
    task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].state = (byte)gmac_null_get_null_state();
  }
  else if(global_data_ptr->mac_mode == IDLE_MODE)
  {
    task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].state = (byte)gmac_idle_get_idle_state();
  }
  else if(global_data_ptr->mac_mode == TRANSFER_MODE)
  {
    task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].state = (byte)gmac_transfer_get_transfer_state();
  }

  if((message_set == MS_MAC_L1) || (message_set == MS_MAC_RR))
  {
    task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].message_set = ((IMH_T *) message_ptr)->message_set;
    task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].message_id = ((IMH_T *) message_ptr)->message_id;
    /*Grab 3 Bytes of Message */
    (void) memscpy(task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].message_frag,
                   sizeof(task_data_ptr->gmac_trace_msg_buf[0].message_frag),
                   ((byte *)message_ptr + sizeof(IMH_T)),
                   sizeof(task_data_ptr->gmac_trace_msg_buf[0].message_frag)
                  );
  }
  else if(message_set == MS_MAC_RLC)
  {
    task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].message_set = MS_MAC_RLC;
    task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].message_id = (byte)((mac_rlc_msg_t*)message_ptr)->msg.sig;
    /*Grab 3 Bytes of Message */
    (void) memscpy(task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].message_frag,
                   sizeof(task_data_ptr->gmac_trace_msg_buf[0].message_frag),
                   &(((mac_rlc_msg_t*)message_ptr)->msg),
                    sizeof(task_data_ptr->gmac_trace_msg_buf[0].message_frag)
                  );
  }
  else if(message_set == MS_MAC_MAC)
  {
    task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].message_set = MS_MAC_MAC;
    task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].message_id = (byte)((mac_int_sig_t*)message_ptr)->sig_id;
  }

  task_data_ptr->gmac_trace_msg_buf_index++;

  if (task_data_ptr->gmac_trace_msg_buf_index >= GMAC_NUM_TRACE_MSGS)
  {
    task_data_ptr->gmac_trace_msg_buf_index = 0;
  }
  task_data_ptr->gmac_trace_msg_buf[task_data_ptr->gmac_trace_msg_buf_index].next_entry = TRUE;
}

/*===========================================================================
===
===  FUNCTION      gmac_initialise()
===
===  DESCRIPTION
===  Contains all initialisation procedures required for MAC
===  upon start up
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
#ifdef PERLUTF
#error code not present
#else
static void gmac_initialise(void)
#endif
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr();


  /* Initialise GMAC critical section */

  rex_init_crit_sect(&gmac_msg_crit_sec);


  /* initialise mac's signal queue */
  (void)q_init(&global_data_ptr->mac_q);

  /* define timer to OS */
  mac_timer_init();

  /* initialise MAC mode and gmac_gprs_mode */
  global_data_ptr->mac_mode = NULL_MODE;

//  memset(&gmac_local_global_pwr_ctrl_params, 0, sizeof(global_pwr_ctrl_t));
//  memset(&gmac_local_pwr_ctrl_params, 0, sizeof(pwr_ctrl_paramtrs_t));

  /* retry bit in MAC header */
  global_data_ptr->c_flags.mac_ctrl_header_r_bit = NOT_SET;

  /* RRBP valid set to NOT valid initially */
  global_data_ptr->gmac_local_rrbp_valid = RM_SP_RRBP_NOT_VALID;

  gmac_util_init_cell_data();

  gmac_init_transfer_flags();
  gmac_init_idle_flags();
  gmac_init_common_flags();
  gmac_init_null_flags();

  gmac_init_null_data_non_zero();
  gmac_init_mac_proc_data_non_zero();
  gmac_init_transfer_data_non_zero();
  gmac_init_tran_dl_data_non_zero();
  gmac_init_idle_data_non_zero();
  gmac_init_rb_data_non_zero();
  gmac_init_util_data_non_zero();
  gmac_init_task_data_non_zero();

  mac_log_init();
}
/*===========================================================================
===
===  FUNCTION      gmac_dl_msg_address_data_valid()
===
===  DESCRIPTION
===
===  If the signal that has been received is MAC_PH_DATA_IND and the
===  dl control msg within it has as after csn.1 decoding contains
===  an address_info_error then the dl_msg is ignored.
===  No further processing of the dl control msg is required.
===
===  DEPENDENCIES
===
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header
===
===  RETURN VALUE
===
===  FALSE if error found.
===
===========================================================================*/
static boolean gmac_dl_msg_address_data_valid(uint8 sig_id,uint8 tsk_id)
{
  uint8 error = TRUE;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr();

  if ((sig_id == (uint8)MAC_PH_DATA_IND) && (tsk_id == MS_MAC_L1))
  {
    if(global_data_ptr->csn_dec_hdr_ptr->error_code == ADDRESS_INFO_ERROR)
    {
      /* DL msg address error ignore msg */
      error = FALSE;
    }
  }
  return(error);
}

/*===========================================================================
===
===  FUNCTION      gmac_assignment_unsupported()
===
===  DESCRIPTION
===
===  This function filters out all unsupported assignment control msgs i.e.
===  fixed allocations for all targets, and EGPRS for Worldphones (6800 and 7500-90)
===
===  DEPENDENCIES
===
===  csn_dec_hdr_ptr - global pointer to decoded CSN.1 info header
===
===  RETURN VALUE
===
===  TRUE if error fixed allocation is found.
===
===========================================================================*/
static boolean gmac_assignment_unsupported(mac_modes_t mode,uint8 sig_id,uint8 tsk_id)
{
 boolean unsupported_assignment = FALSE;

 boolean egprs_capable_cell = FALSE;

 gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr();

 /*Check cell supports EGPRS before acpeting any EGPRS assigmanets */
 rr_read_public_store( RRPS_CELL_OPTIONS_EGPRS_CAPABLE_CELL, &egprs_capable_cell );

 MSG_GERAN_MED_4_G("GMAC mode%d, task_id %d, sig %d alloc %d",
   mode, tsk_id, sig_id, global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.alloc_flag);

 if(mode == NULL_MODE)
 {
   /*Check assignments RCVD from RR*/
   if(tsk_id == MS_MAC_RR)
   {
     switch (sig_id)
     {
        case GRR_MAC_UL_TBF_EST:
          /*check for Fixed allaoctions*/
          if (global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.alloc_flag == fixed)
          {
            unsupported_assignment = TRUE;
            MSG_GERAN_LOW_0_G("Fixed alloc in IA");
          }

          /*check for EGPRS assignment against cell EGPRS capability support  */
          if  ( (global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_tbf_est.egprs_ul_tbf.egprs_alloc == TRUE)
                && (egprs_capable_cell == FALSE) )
          {
            unsupported_assignment = TRUE;
            MSG_GERAN_LOW_0_G("EGPRS Not suppressed on cell : EGPRS assignment rcvd in UL IA. Ignore assignment");
          }
          break;

        case GRR_MAC_UL_SB_EST:
           /*check for EGPRS assignment against cell EGPRS capability support  */
           if  ( (global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_ul_sb_est.egprs_ul_sb_est.egprs_alloc == TRUE)
                 && (egprs_capable_cell == FALSE) )
           {
             unsupported_assignment = TRUE;
             MSG_GERAN_LOW_0_G("EGPRS Not suppressed on cell : EGPRS assignment rcvd in UL SB IA. Ignore assignment");
           }
           break;

        case GRR_MAC_DL_TBF_EST:
          /*check for EGPRS assignment against cell EGPRS capability support  */
          if  ( (global_data_ptr->msg_ptr->grr_mac_sig.msg.gm_dl_tbf_est.egprs_alloc == TRUE)
                && (egprs_capable_cell == FALSE) )
          {
            unsupported_assignment = TRUE;
            MSG_GERAN_LOW_0_G("EGPRS Not suppressed on cell : EGPRS assignment rcvd in DL IA Ignore assignment");
          }
          break;

        default:
          /*No action*/
          break;
     }
   }
 }
 else
 {
  /*check PH_DATA_IND for PDA,PUA,PUAN,PTR*/
  if(sig_id == (uint8)MAC_PH_DATA_IND && tsk_id == MS_MAC_L1)
  {
     switch (global_data_ptr->csn_dec_hdr_ptr->message_type)
     {
      case PACKET_DOWNLINK_ASSIGNMENT:
        if (  (((packet_downlink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->mac_mode) == 2
           || (((packet_downlink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->mac_mode) == 3    )
        {
          unsupported_assignment = TRUE;
          MSG_GERAN_LOW_1_G("Fixed alloc in PDA %d",global_data_ptr->csn_dec_hdr_ptr->message_type);
        }

#ifdef FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED
        if ((((packet_downlink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->r99_additions_flag == TRUE) &&
            (((packet_downlink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->egprs_options_flag == TRUE))
        {
          unsupported_assignment = TRUE;
        }
#else /*  FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED */
        /*check for EGPRS assignment against cell EGPRS capability support  */
        if ((((packet_downlink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->r99_additions_flag == TRUE) &&
            (((packet_downlink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->egprs_options_flag == TRUE) &&
            (egprs_capable_cell == FALSE))
        {
          unsupported_assignment = TRUE;
          MSG_GERAN_LOW_0_G("EGPRS Not suppressed on cell : EGPRS assignment rcvd in PDA. Ignore assignment");
        }
#endif /*  FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED */

        break;

      case PACKET_TIMESLOT_RECONFIGURE:
        if ((((packet_timeslot_reconfigure_t *)global_data_ptr->csn_dec_data_ptr)->alloc_flag) == fixed)
        {
          unsupported_assignment = TRUE;
          MSG_GERAN_LOW_1_G("Fixed alloc in PTR %d",global_data_ptr->csn_dec_hdr_ptr->message_type);
        }

#ifdef FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED
        if (((packet_timeslot_reconfigure_t *)global_data_ptr->csn_dec_data_ptr)->egprs_ptr == TRUE)
        {
          unsupported_assignment = TRUE;
        }
#else /*  FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED */
        if ((((packet_timeslot_reconfigure_t *)global_data_ptr->csn_dec_data_ptr)->egprs_ptr == TRUE) &&
            (egprs_capable_cell == FALSE))
        {
          unsupported_assignment = TRUE;
          MSG_GERAN_LOW_0_G("EGPRS Not suppressed on cell : EGPRS assignment rcvd in PTR. Ignore assignment");
        }
#endif /*  FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED */

        break;

      case PACKET_UPLINK_ACK_NACK:
        if ((((packet_uplink_ack_nack_t *)global_data_ptr->csn_dec_data_ptr)->fixed_alloc_flag) == valid)
        {
          unsupported_assignment = TRUE;
          MSG_GERAN_LOW_1_G("Fixed alloc in PUAN %d",global_data_ptr->csn_dec_hdr_ptr->message_type);
        }

#ifdef FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED
        if (((packet_uplink_ack_nack_t *)global_data_ptr->csn_dec_data_ptr)->egprs_puan == TRUE)
        {
          unsupported_assignment = TRUE;
        }
#else /*  FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED */
        if ((((packet_uplink_ack_nack_t *)global_data_ptr->csn_dec_data_ptr)->egprs_puan == TRUE) &&
            (egprs_capable_cell == FALSE))
        {
          unsupported_assignment = TRUE;
          MSG_GERAN_LOW_0_G("EGPRS Not suppressed on cell : EGPRS assignment rcvd in PUAN. Ignore assignment");
        }
#endif /*  FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED */
        break;

      case PACKET_UPLINK_ASSIGNMENT:
        if ((((packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->allocation_flag) == fixed_alloc)
        {
          unsupported_assignment = TRUE;
          MSG_GERAN_MED_1_G("Fixed alloc in PUA %d",global_data_ptr->csn_dec_hdr_ptr->message_type);
        }

#ifdef FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED
        if (((packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->egprs_pua == TRUE)
        {
          unsupported_assignment = TRUE;
        }
#else /*  FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED */
       if ((((packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr)->egprs_pua == TRUE) &&
           (egprs_capable_cell == FALSE))
        {
          unsupported_assignment = TRUE;
          MSG_GERAN_LOW_0_G("EGPRS Not suppressed on cell : EGPRS assignment rcvd in PUA. Ignore assignment");
        }
#endif /*  FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED */
        break;

      default:
        /*No action*/
        break;
     }
   }
 }

 return(unsupported_assignment);

}


#ifdef FEATURE_GSM_EDTM
/*===========================================================================
===
===  FUNCTION      gmac_edtm_enabled_init()
===
===  DESCRIPTION
===  Initilialise the edtm_enabled flag from the RR public store.
===
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_edtm_enabled_init(void)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr();

  rr_read_public_store( RRPS_MS_EDTM_ENABLED, &global_data_ptr->gmac_edtm_enabled );
}

#endif /*FEATURE_GSM_EDTM*/

/*tlx free floating */
/*===========================================================================
===
===  FUNCTION      gmac_check_q_for_mac_internal_sigs()
===
===  DESCRIPTION
===
===  Compare function passed to q_linear_search and is used by q_linear_search to checks
===  the Q for the item requested.
===
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  Return 1 if the value matches and 0 otherwise
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static boolean  gmac_check_q_for_mac_internal_sigs(
                                            mac_signals_u*   item_ptr,
                                            uint8            compare_task_id
                                           )
{

  if(item_ptr->mac_int_sig.task_id == MS_MAC_MAC )
  {
     return (1);
  }
  else
  {
     return (0);
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_log_reshuffling_of_internal_sigs()
===
===  DESCRIPTION
===
===  Debug to monitor the re shuffling of mac internal signals to the front of the Q
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void gmac_log_reshuffling_of_internal_sigs(void)
{
  mac_signals_u   *next_debug_msg_ptr;
  uint8           mac_num_sigs_in_q = 0;
  uint8           task_id;
  uint8           signal_id;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr();

  mac_num_sigs_in_q = q_cnt(&global_data_ptr->mac_q);

  if(mac_num_sigs_in_q > 1)
  {

    next_debug_msg_ptr = q_check(&global_data_ptr->mac_q);
    if (next_debug_msg_ptr == NULL)
    {
      MSG_GERAN_LOW_0_G("MAC error q_check return NULL pointer ");
      return;
    }
    task_id = next_debug_msg_ptr->grr_mac_sig.task_id;


    if (task_id == MS_MAC_L1)
    {
      signal_id = (uint8)next_debug_msg_ptr->l1_mac_sig.sig_id;
    }
    else if (task_id == MS_MAC_RR)
    {
      signal_id = (uint8)next_debug_msg_ptr->grr_mac_sig.sig_id;
    }
    else if (task_id == MS_MAC_RLC)
    {
      signal_id = (uint8)next_debug_msg_ptr->rlc_mac_sig.msg.sig;
    }
    else if (task_id == MS_MAC_MAC)
    {
      signal_id = (uint8)next_debug_msg_ptr->mac_int_sig.sig_id;
    }
    else
    {
      MSG_GERAN_LOW_1_G("MAC error TASK ID unknown: 0x%x",task_id);
      return;
    }

    MSG_GERAN_LOW_2_G("First sig in q task id %d. sig id %d",task_id,signal_id);

    next_debug_msg_ptr = q_next(&global_data_ptr->mac_q,&(next_debug_msg_ptr->l1_mac_sig.link));

    while(next_debug_msg_ptr != NULL)
    {
      task_id = next_debug_msg_ptr->grr_mac_sig.task_id;

      if (task_id == MS_MAC_L1)
      {
        signal_id = (uint8)next_debug_msg_ptr->l1_mac_sig.sig_id;
      }
      else if (task_id == MS_MAC_RR)
      {
        signal_id = (uint8)next_debug_msg_ptr->grr_mac_sig.sig_id;
      }
      else if (task_id == MS_MAC_RLC)
      {
        signal_id = (uint8)next_debug_msg_ptr->rlc_mac_sig.msg.sig;
      }
      else if (task_id == MS_MAC_MAC)
      {
        signal_id = (uint8)next_debug_msg_ptr->mac_int_sig.sig_id;
      }
      else
      {
        MSG_GERAN_LOW_1_G("MAC error TASK ID unknown: 0x%x",task_id);
        return;
      }

      MSG_GERAN_LOW_2_G("next sig in q task id %d sig id %d",task_id,signal_id);

      next_debug_msg_ptr = q_next(&global_data_ptr->mac_q,&(next_debug_msg_ptr->l1_mac_sig.link));
    }
  }
    }

#ifdef PERLUTF
#error code not present
#endif /*PERLUTF*/

/*** EOF: don't remove! ***/
