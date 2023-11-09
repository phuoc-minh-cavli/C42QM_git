/*===========================================================================
                      DSMSGRRECV.C
DESCRIPTION
This file defines 3GPP MSGR APIs to external clients outside DS.

Copyright (c) 2018-2021 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE
when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "dsmsgr.h"

#include "amssassert.h"
#include "err.h"
#include "dsmsgr.h"
#include "queue.h"
#include "pthread.h"
#include "msgr.h"
#include "appmgr.h"

#ifdef FEATURE_DATA_LTE
#include "lte_pdcp_msg.h"
#include "lte_tlb_msg.h"
#include "lte_tlb.h"
#include "lte_rlc_msg.h"
#include "emm_ext_msg.h"
#include "lte_rrc_ext_msg.h"
#ifdef FEATURE_FAST_DORMANCY
#include "lte_as.h"
#include "nb1_as.h"
#endif /* FEATURE_FAST_DORMANCY */
#endif

#ifdef FEATURE_DSAT_ATCOP_VEND
#if defined(FEATURE_GSM)
#include "geran_grr_msg.h"
#endif
#ifdef FEATURE_FSK_TX
#include "msgr_geran.h"
#endif /* FEATURE_FSK_TX */
#endif

#include "msgr_nas.h"

#include "data_msg.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif

#include "dog.h"
#if (defined(FEATURE_WDOG_DYNAMIC) || defined (DOG_DYNAMIC_API))
#define FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC
#endif /* FEATURE_WDOG_DYNAMIC */

#ifdef FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC
#include "rex.h"
#include "mcs_timer.h"
#endif /* FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC */

#include "cm_msgr_msg.h"
#include "dstaski.h"
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#define ARRSIZ(a)	(sizeof(a)/sizeof(a[0]))

/*
** Receiver-side type defs
*/

/* Union of all messages that DS can receive */

typedef union {
  /* This will be the only info in case the message is a SYNC MSG*/
  msgr_hdr_s                         hdr;

#ifdef FEATURE_DATA_LTE
  /* These messages will be received from PDCP */
  lte_pdcpul_rab_register_cnf_msg_s  ds_lte_pdcpul_rab_register_cnf_msg;
  lte_pdcpdl_rab_register_cnf_msg_s  ds_lte_pdcpdl_rab_register_cnf_msg;
  lte_pdcp_rab_deregister_cnf_msg_s  ds_lte_pdcp_rab_deregister_cnf_msg;
  lte_tlb_lb_close_tl_ind_s          ds_lte_tlb_lb_close_tl_ind_s;
#endif /* FEATURE_DATA_LTE */

  /* This message will be received from CM */ 
  cm_3gpp_service_status_s_type      cm_service_status_ind;
  cm_psm_get_cfg_params_rsp_s_type     ds_psm_get_cfg_params_resp;
  cm_psm_status_ind_s_type             ds_psm_status_ind;
  cm_mm_psm_cfg_change_ind_s_type      ds_psm_cfg_change_ind;  
#ifdef FEATURE_REF_TRACKER
  cm_qmi_sensor_event_ind_s_type       ds_qmi_sensor_event_ind;

  cm_qmi_sns_data_collection_rsp_s_type  ds_qmi_sensor_data_rsp;
#endif
#ifdef FEATURE_DSAT_ATCOP_VEND
  cm_psm_image_boot_type_ind_s_type    ds_psm_image_boot_ind;

#ifdef FEATURE_FSK_TX
  geran_fsk_debug_rsp_type             fsk_dbg_rsp;
  geran_fsk_debug_query_rsp_type       fsk_dbg_query_rsp;
  geran_fsk_data_rsp_type              fsk_data_rsp;
  geran_fsk_hop_table_rsp_type         fsk_hop_table_rsp;
  geran_fsk_hop_table_query_rsp_type   fsk_hop_table_query_rsp;
  geran_fsk_start_rsp_type             fsk_start_rsp;
  geran_fsk_stop_rsp_type              fsk_stop_rsp;
  geran_fsk_pcl_rsp_type               fsk_pcl_rsp;
  geran_fsk_pcl_query_rsp_type         fsk_pcl_query_rsp;
#endif /* FEATURE_FSK_TX */
#endif
} dsmsgrrcv_msg_u;


typedef struct {
  q_link_type     link;
  dsmsgrrcv_msg_u msg;
} dsmsgrrcv_cmd_s;

typedef union {
  msgr_hdr_s        hdr;
  struct {
    dsmsgrrcv_msg_u msg;
  } ds_cmd;
  msgr_spr_loopback_s loopback_msg;
} dsmsgrrcv_super_cmd_u;

#define     DSMSGRRCV_CMD_MAX        24

static dsmsgrrcv_cmd_s dsmsgrrcv_cmd_arr[DSMSGRRCV_CMD_MAX] = {{{NULL}}};

typedef struct {
  msgr_umid_type        msgrtype;
  dsmsgrcv_msg_hdlr_f   hdlrfunc;
} dsmsgrrcv_msg_map_t;

static dsmsgrrcv_msg_map_t dsmsgrrcv_msg_map_arr[] = {
#ifdef FEATURE_DATA_LTE
  { LTE_PDCPUL_RAB_REGISTER_CNF, NULL },
  { LTE_PDCPDL_RAB_REGISTER_CNF, NULL },
  { LTE_PDCPUL_RAB_DEREGISTER_CNF, NULL },
  { LTE_PDCPDL_RAB_DEREGISTER_CNF, NULL },
#endif

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#ifdef FEATURE_DATA_LTE
  { LTE_TLB_LB_ACT_IND, NULL },
  { LTE_TLB_LB_CLOSE_TL_IND, NULL },
  { LTE_TLB_LB_OPEN_IND, NULL },
  { LTE_TLB_LB_DEACT_IND, NULL },
  { LTE_RRC_DEACTIVATE_REQ, NULL},
  { LTE_RRC_UE_CAP_INFO_IND, NULL},
  { LTE_RRC_CELL_DEPRIOR_STATUS_IND, NULL},
#ifdef FEATURE_FAST_DORMANCY
  { LTE_MAC_RAI_INDI, NULL },
  { NB1_MAC_RAI_INDI, NULL },
#endif /* FEATURE_FAST_DORMANCY */
#endif /* FEATURE_DATA_LTE */

  { MM_CM_PSM_GET_CFG_PARAMS_RSP, NULL },
  { MM_CM_PSM_STATUS_IND, NULL },
  { MM_CM_PSM_CFG_CHANGE_IND, NULL },
  { MM_CM_3GPP_SRV_IND , NULL},
  { MM_CM_PSM_DEEP_SLEEP_ENTER_IND, NULL},
  { MM_CM_PSM_DEEP_SLEEP_EXIT_IND , NULL},
  { DS_3GPP_TLB_MODE_H_ACT_DEACT_IND, NULL },
  { NAS_EMM_SET_EDRX_CFG_RSP, NULL },
  { NAS_EMM_GET_EDRX_PARAMS_RSP, NULL },
  { DS_3GPP_MO_EXP_ACTIVITY_STATUS_IND, NULL},
  { MM_CM_T3346_INFO_IND, NULL}
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
  ,{ MM_CM_WWAN_GPS_SCAN_STATUS_IND, NULL}
#endif /* FEATURE_GPS_WWAN_PRIORITY_SEARCH */
#ifdef FEATURE_REF_TRACKER
  ,{ MM_CM_SNS_MGR_SNS_APP_EVENT_IND ,NULL}
  ,{ MM_CM_SNS_MGR_DATA_RSP ,NULL}
#endif
#ifdef FEATURE_DSAT_ATCOP_VEND
  ,{ MM_CM_PSM_IMAGE_BOOT_TYPE_IND ,NULL}
  ,{ LTE_CPHY_CE_INFO_REPORT_RSP ,NULL}
  ,{ LTE_RRC_DS_SERVICE_IND ,NULL}
  ,{ LTE_CPHY_GET_NEXT_SFN_RSP ,NULL}
  ,{ LTE_CPHY_SET_SFN_CONFIG_RSP ,NULL}
  ,{ LTE_CPHY_SFN_THRESHOLD_REACHED_IND ,NULL}
  ,{ LTE_RRC_CELL_SELECT_INFO_RSP ,NULL}
  #if defined(FEATURE_GSM)
  ,{ GERAN_GRR_CELL_SELECT_INFO_RSP , NULL}
  #endif
#ifdef FEATURE_FSK_TX
  ,{ GERAN_GL1_FSK_DEBUG_RSP ,NULL}
  ,{ GERAN_GL1_FSK_DEBUG_QUERY_RSP ,NULL}
  ,{ GERAN_GL1_FSK_DATA_RSP ,NULL}
  ,{ GERAN_GL1_FSK_HOP_TABLE_RSP ,NULL}
  ,{ GERAN_GL1_FSK_HOP_TABLE_QUERY_RSP ,NULL}
  ,{ GERAN_GL1_FSK_START_RSP ,NULL}
  ,{ GERAN_GL1_FSK_STOP_RSP ,NULL}
  ,{ GERAN_GL1_FSK_PCL_RSP ,NULL}
  ,{ GERAN_GL1_FSK_PCL_QUERY_RSP ,NULL}
#endif /* FEATURE_FSK_TX */
#endif
};

#define DSMSGRRCV_MSG_MAP_ARR_MAX   ARRSIZ(dsmsgrrcv_msg_map_arr)

static q_type dsmsgrrcv_cmd_q = {{NULL}};
static q_type dsmsgrrcv_cmd_free_q = {{NULL}};

/*---------------------------------------------------------------------------
  Dog data structures for DSMSGRRCV POSIX thread
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC
LOCAL void dsmsgrrcv_wdog_expired_ind_handler
(
  void
);
LOCAL void dsmsgrrcv_timer_cb
( 
  mcs_timer_cb_data_type data
);

typedef struct
{
  mcs_timer_type timer;               /*!< MCS timer  */
  msgr_tech_module_type  moduleid;    /*!< TECH,MODULE Performing Operation */
  msgr_umid_type umid;                /*!< UMID of message to be posted from callback */
} dsmsgrrcv_timer_s;

LOCAL boolean dsmsgrrcv_dog_rpt_enabled = TRUE; /*! Reporting enabled for this task */
typedef struct
{
  uint32 rpt_var;                     /*! dog report ID       */
  uint32 rpt_time_var;                /*! Report Period       */
  dsmsgrrcv_timer_s rpt_timer;        /*! timer info          */
} dsmsgrrcv_dog_info_s;

LOCAL dsmsgrrcv_dog_info_s dsmsgrrcv_wdog_info;
#endif /* FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC */

static pthread_mutex_t dsmsgrrcv_mutex;

static pthread_t       dsmsgrrcv_thread;

static msgr_client_t   dsmsgrrcv_msgr_client;
static msgr_id_t       dsmsgrrcv_msgr_q_id;

#define     DSMSGRRCV_MSGR_PRI          1
#define     DSMSGRRCV_MSGR_MAX_MSGS     20
#define     DSMSGRRCV_MSGR_MAX_MSG_SZ   sizeof(dsmsgrrcv_msg_u)
#define     DSMSGRRCV_MSGR_MAX_MALLOC   1

#define     DSMSGRRECV_TASK_STACK_SIZE 2048 // 16384 earlier from pthread default

static dsmsgrrcv_super_cmd_u dsmsgrrcv_super_cmd;

/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

static dsmsgrrcv_cmd_s * dsmsgrrcv_get_cmd_buf (void)
{
  dsmsgrrcv_cmd_s * cmd_p;

  cmd_p = q_get(&dsmsgrrcv_cmd_free_q);
  DS_ASSERT(cmd_p != NULL);

  return cmd_p;
}

static void dsmsgrrcv_free_cmd_buf (dsmsgrrcv_cmd_s * cmd_p)
{
  DS_ASSERT(cmd_p != NULL);

  q_put(&dsmsgrrcv_cmd_free_q, &cmd_p->link);
}

static void dsmsgrrcv_post_cmd (dsmsgrrcv_cmd_s * cmd_p)
{
  q_put( &dsmsgrrcv_cmd_q, &cmd_p->link );
}

static dsmsgrrcv_cmd_s * dsmsgrrcv_get_next_cmd (void)
{
  dsmsgrrcv_cmd_s * cmd_p;

  cmd_p = q_get(&dsmsgrrcv_cmd_q);
  return cmd_p;
}

static dsmsgrrcv_msg_map_t * dsmsgrrcv_get_msg_map_p (msgr_umid_type msgrtype)
{
  unsigned int i;
  dsmsgrrcv_msg_map_t * msg_map_p;

  for (i = 0, msg_map_p = &dsmsgrrcv_msg_map_arr[0];
       i < DSMSGRRCV_MSG_MAP_ARR_MAX;
       ++i, ++msg_map_p) 
  {
    if (msgrtype == msg_map_p->msgrtype) 
    {
      break;
    }
  }

  if (i == DSMSGRRCV_MSG_MAP_ARR_MAX) 
  {
    DS_ERROR_LOG_1("Invalid msgrtype: =d, no mapping found", msgrtype);
    msg_map_p = NULL;
  }

  return msg_map_p;
}

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

void dsmsgrrcv_reg_hdlr (msgr_umid_type msgrtype, dsmsgrcv_msg_hdlr_f dshdlr)
{
  unsigned int i;
  dsmsgrrcv_msg_map_t * msg_map_p;

  DS_ASSERT(dshdlr != NULL);

  for (i = 0, msg_map_p = &dsmsgrrcv_msg_map_arr[0];
       i < DSMSGRRCV_MSG_MAP_ARR_MAX;
       ++i, ++msg_map_p) 
  {
    if (msgrtype == msg_map_p->msgrtype) 
    {
//#ifndef TEST_FRAMEWORK
//      ASSERT(msg_map_p->hdlrfunc == NULL);
//#endif
      msg_map_p->hdlrfunc = dshdlr;
      break;
    }
  }

  DS_ASSERT(i <= DSMSGRRCV_MSG_MAP_ARR_MAX);
  return;
}

void dsmsgrrcv_init (void)
{
  int i;
  dsmsgrrcv_cmd_s * cmd_p;

  (void)q_init( &dsmsgrrcv_cmd_q );
  (void)q_init( &dsmsgrrcv_cmd_free_q );

  for (i = 0, cmd_p = &dsmsgrrcv_cmd_arr[0];
       i < DSMSGRRCV_CMD_MAX; 
       ++i, ++cmd_p)
  {
    (void)q_link(cmd_p, &cmd_p->link);
    q_put(&dsmsgrrcv_cmd_free_q, &cmd_p->link);
  }

  (void)pthread_mutex_init(&dsmsgrrcv_mutex, NULL);

#ifdef FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC
  if (dsmsgrrcv_dog_rpt_enabled)
  {
    rex_tcb_type *rex_tcb_ptr = rex_self(); /*! Get my TCB pointer */
    if(NULL != rex_tcb_ptr)
    {
      /*-----------------------------------------------------------------------
        Watchdog registration and timer kick-off.
        -----------------------------------------------------------------------*/
      dsmsgrrcv_wdog_info.rpt_var     = dog_register (rex_tcb_ptr, 
                                                      DOG_DEFAULT_TIMEOUT, 
                                                      DOG_DEADLKCHK_ENABLE);
      dsmsgrrcv_wdog_info.rpt_time_var   = 
                         dog_get_report_period (dsmsgrrcv_wdog_info.rpt_var);

      /*----------------------------------------------------------------------
        If the return value from dog_get_report_period() was zero then we MUST
        NOT report to dog directly, as DOG will monitor us in a different manner.
        Hence we won't initialize or start any timer.
      -----------------------------------------------------------------------*/
      if (dsmsgrrcv_wdog_info.rpt_time_var > 0) 
      {
        /*--------------------------------------------------------------------
        Define and start the watchdog timer to wake up for the 1st petting.
        ---------------------------------------------------------------------*/
        dsmsgrrcv_wdog_info.rpt_timer.moduleid = MSGR_DS_MSGRRECV;
        dsmsgrrcv_wdog_info.rpt_timer.umid = 
                             DS_MSGRRECV_WDOG_RPT_TMR_EXPIRED_TMRI;
        mcs_timer_def(&dsmsgrrcv_wdog_info.rpt_timer.timer,
                      NULL,
                      dsmsgrrcv_timer_cb,
                     (mcs_timer_cb_data_type)&dsmsgrrcv_wdog_info.rpt_timer);
        mcs_timer_set (&dsmsgrrcv_wdog_info.rpt_timer.timer, 
                       dsmsgrrcv_wdog_info.rpt_time_var, 0, T_MSEC);
      }
    }/* NULL != rex_tcb_ptr */
  } /* dsmsgrrcv_dog_rpt_enabled */
#endif /* FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC */
}

void dsmsgrrcv_register (void)
{
  errno_enum_type errnum;
  unsigned int i;
  dsmsgrrcv_msg_map_t * msg_map_p;

  errnum = msgr_client_create(&dsmsgrrcv_msgr_client);
  DS_ASSERT(errnum == E_SUCCESS);

  errnum = msgr_client_add_queue
           (
             "DS MSGR RECV",
             &dsmsgrrcv_msgr_client,
             DSMSGRRCV_MSGR_PRI,
             DSMSGRRCV_MSGR_MAX_MSGS * DSMSGRRCV_MSGR_MAX_MSG_SZ,
             DSMSGRRCV_MSGR_MAX_MALLOC,
             &dsmsgrrcv_msgr_q_id             
           );
  DS_ASSERT(errnum == E_SUCCESS);

  /* 
  ** Now register for all messages
  */

  errnum = msgr_register
           (
             MSGR_DS_MSGRRECV,
             &dsmsgrrcv_msgr_client,
             dsmsgrrcv_msgr_q_id,
             DS_MSGRRECV_SPR_LOOPBACK
           );
  DS_ASSERT(errnum == E_SUCCESS);

  errnum = msgr_register
           (
             MSGR_DS_MSGRRECV,
             &dsmsgrrcv_msgr_client,
             dsmsgrrcv_msgr_q_id,
             DS_MSGRRECV_SPR_THREAD_KILL
           );
  DS_ASSERT(errnum == E_SUCCESS);

  for (i = 0, msg_map_p = &dsmsgrrcv_msg_map_arr[0];
       i < DSMSGRRCV_MSG_MAP_ARR_MAX;
       ++i, ++msg_map_p) 
  {
    errnum = msgr_register
             (
               MSGR_DS_MSGRRECV, 
               &dsmsgrrcv_msgr_client,
               dsmsgrrcv_msgr_q_id,
               msg_map_p->msgrtype
             );
    DS_ASSERT(errnum == E_SUCCESS);
  }

   /* 
   ** Register for WDOG expiry internal timer indication
   */
#ifdef FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC
   errnum = msgr_register
           (
             MSGR_DS_MSGRRECV,
             &dsmsgrrcv_msgr_client,
             dsmsgrrcv_msgr_q_id,
             DS_MSGRRECV_WDOG_RPT_TMR_EXPIRED_TMRI
           );
  DS_ASSERT(errnum == E_SUCCESS);
#endif /* FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC */

#ifdef FEATURE_TDSCDMA
  errnum = msgr_register
           (
             MSGR_DS_MSGRRECV,
             &dsmsgrrcv_msgr_client,
             dsmsgrrcv_msgr_q_id,
             TDSCDMA_RRC_SET_CONFIG_RSP
           );
  DS_ASSERT(errnum == E_SUCCESS);

  errnum = msgr_register
           (
             MSGR_DS_MSGRRECV,
             &dsmsgrrcv_msgr_client,
             dsmsgrrcv_msgr_q_id,
             TDSCDMA_RRC_GET_CONFIG_RSP
           );
  DS_ASSERT(errnum == E_SUCCESS);
#endif
}

void dsmsgrrcv_task_exit (void)
{
  int rval = -1;
  // Release the mailbox
  (void) msgr_client_delete(&dsmsgrrcv_msgr_client);

  // release mutex
  rval = pthread_mutex_destroy(&dsmsgrrcv_mutex);

  if( rval != 0 )
  {
    DS_ERROR_FATAL("Failed dsmsgrrcv_mutex destroy");
  }

#ifdef FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC
  if (dsmsgrrcv_dog_rpt_enabled)
  {
    dog_deregister(dsmsgrrcv_wdog_info.rpt_var);
    (void ) mcs_timer_clr(&dsmsgrrcv_wdog_info.rpt_timer.timer,T_SCLK);
  }
#endif /* FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC */

  pthread_exit(NULL);
}

void * dsmsgrrcv_thrd_main (void * arg)
{
  errno_enum_type errnum;
  dsmsgrrcv_cmd_s * cmd_p;
  uint32 msglen;
  dsmsgrrcv_msg_map_t * msg_map_p;

  dsmsgrrcv_init();
  dsmsgrrcv_register();

  appmgr_thread_init_complete(MSGR_DS_MSGRRECV);

  for (;;)
  {
    errnum = msgr_receive
             (
               &dsmsgrrcv_msgr_client,
               (uint8 *)&dsmsgrrcv_super_cmd,
               DSMSGRRCV_MSGR_MAX_MSG_SZ,
               &msglen
             );
    DS_ASSERT(errnum == E_SUCCESS);
    DS_ASSERT(msglen <= DSMSGRRCV_MSGR_MAX_MSG_SZ);

    switch (dsmsgrrcv_super_cmd.hdr.id) {
    case DS_MSGRRECV_SPR_LOOPBACK:
    {
      msgr_spr_loopback_struct_type loopback_msg;

      errnum = msgr_loopback_reply(&loopback_msg, MSGR_DS_MSGRRECV);
      DS_ASSERT(errnum == E_SUCCESS);
      break;
    }
    case DS_MSGRRECV_SPR_THREAD_KILL:
    {
      // Thread is being killed, cleanup
      dsmsgrrcv_task_exit();
      break;
    }

#ifdef FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC
    case DS_MSGRRECV_WDOG_RPT_TMR_EXPIRED_TMRI:
    {
      // WDOG timer expired, report DOG, and kick the timer again.
      if (dsmsgrrcv_dog_rpt_enabled) 
      {
        dsmsgrrcv_wdog_expired_ind_handler();
      }
      break;
    }
#endif /* FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC */


    default:
    {
      /*policyman_ind may come even before DS task is ready, this check ensures
         not to post any command to DS till some handler is registered*/
      msg_map_p = dsmsgrrcv_get_msg_map_p(dsmsgrrcv_super_cmd.ds_cmd.msg.hdr.id);
      if ((msg_map_p != NULL) && 
          (msg_map_p->hdlrfunc == NULL))
      {
        break;
      }

      cmd_p = dsmsgrrcv_get_cmd_buf();
      DS_ASSERT(cmd_p != NULL);

      cmd_p->msg = dsmsgrrcv_super_cmd.ds_cmd.msg;

      dsmsgrrcv_post_cmd(cmd_p);
      dsmsgr_post_ds_cmd();
      break;
    }
    }
  } /* for(;;) */
}

pthread_t dsmsgrrecv_task (int priority)
{
  int rval;
  pthread_attr_t attr;
  struct sched_param param;

  // Init thread attributes
  rval = pthread_attr_init(&attr);
  DS_ASSERT(rval == 0);

#ifndef T_UNIX
  rval = pthread_attr_setstacksize(&attr, DSMSGRRECV_TASK_STACK_SIZE); // dsmsgrrecv_task stack
  DS_ASSERT(rval == 0);
#endif
  
  memset(&param,0,sizeof(param));
  
  // Set priority to what the app mgr passed in
  param.sched_priority = priority;
  rval = pthread_attr_setschedparam(&attr, &param);

#ifndef T_UNIX
  rval = pthread_attr_setthreadname(&attr, "DSMSGR RECV");
  DS_ASSERT(rval == 0);
#endif

  rval = pthread_create
         (
           &dsmsgrrcv_thread, 
           &attr,
           dsmsgrrcv_thrd_main,
           NULL
         );

  if (rval != 0) 
  {
    DS_ERROR_LOG_0("DSMSGR RECV thread creation failed!");
  }

  return dsmsgrrcv_thread;
}

void dsmsgr_process_ds_cmd (void * ds_cmd_p)
{
  dsmsgrrcv_cmd_s * cmd_p;
  dsmsgrrcv_msg_map_t * msg_map_p;
  boolean rval;

  while ((cmd_p = dsmsgrrcv_get_next_cmd()) != NULL)
  {
    msg_map_p = dsmsgrrcv_get_msg_map_p(cmd_p->msg.hdr.id);

    if(msg_map_p != NULL)
    {
      rval = FALSE;
      if (msg_map_p->hdlrfunc != NULL) 
      {
        rval = msg_map_p->hdlrfunc(msg_map_p->msgrtype,\
		      (msgr_hdr_struct_type*)&cmd_p->msg);
      } 

      if (rval == FALSE) 
      {
        DS_ERROR_LOG_1("Failed to handle msg =d", msg_map_p->msgrtype);
      }
  }

    dsmsgrrcv_free_cmd_buf(cmd_p);
  }
}


void dsmsgr_post_ds_cmd (void)
{
  ds_command_type * ds_cmd_p;

  ds_cmd_p = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
  DS_ASSERT(ds_cmd_p != NULL);

  ds_cmd_p->hdr.cmd_id = DS_CMD_MSGR_RECV;
  ds_put_cmd(ds_cmd_p);

}

#ifdef FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC
/*===========================================================================

  FUNCTION:  dsmsgrrcv_timer_cb

===========================================================================*/
/*!
    @brief
    This function is called by MCS when the timer expires.

    @detail
    This function posts a message with the specified message id.

    @return
    
    @note
    Currently no users other than WDOG timer, hence wrapped under that feat.

*/
/*=========================================================================*/
LOCAL void dsmsgrrcv_timer_cb
( 
  mcs_timer_cb_data_type data
)
{
  msgr_hdr_s hdr;
  errno_enum_type result;
  dsmsgrrcv_timer_s* timer;

  timer = ( dsmsgrrcv_timer_s* )data;
  msgr_init_hdr(&hdr, timer->moduleid, timer->umid);
  result = msgr_send(&hdr,sizeof(hdr));
  DS_ASSERT(E_SUCCESS == result);
}

/*===========================================================================

  FUNCTION:  dsmsgrrcv_wdog_expired_handler

===========================================================================*/
/*!
  @brief
  WDOG expiration handler

*/
/*=========================================================================*/
LOCAL void dsmsgrrcv_wdog_expired_ind_handler
(
  void
)
{
  if (dsmsgrrcv_wdog_info.rpt_time_var > 0) 
  {
    dog_report(dsmsgrrcv_wdog_info.rpt_var);
    mcs_timer_set (&dsmsgrrcv_wdog_info.rpt_timer.timer, 
                   dsmsgrrcv_wdog_info.rpt_time_var, 0, T_MSEC);
  }
} /* dsmsgrrcv_wdog_timer_expired_ind_handler */
#endif /* FEATURE_DATA_DSMSGRRECV_WDOG_DYNAMIC */



