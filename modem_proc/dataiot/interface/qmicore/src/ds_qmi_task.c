/*===========================================================================

                         D S _ Q M I _ T A S K . C

DESCRIPTION

 The Data Services Qualcomm MSM Interface Task source file.

EXTERNALIZED FUNCTIONS

Copyright (c) 2005-2019 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/qmicore/src/ds_qmi_task.c#2 $ $DateTime: 2022/05/13 03:59:34 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/21/14    sk     QMI-DMS Service QCSI migration.
05/19/14    vrk    Added support for current device config through policyman
12/07/13    vrk    Added EAP Notificaton code Indication support
01/11/13    rk     Removed feature mobileap.
11/08/12    rk     Added QCMAP service init.
08/08/12    pgm    Support for different APNS in Dual-IP.
7/18/12     ab     Fix for Memory Leak in qmi_process_cmd
07/26/12    sb     Fixed Compiler Warnings.
06/11/12    sb     Fix to move to correct UIM state in case of CARD ERROR
                   event.
05/14/12    sa     Modified the api for qsap initialization.
03/06/12    wc     Remove QMI UIM/CAT from Data
03/01/12    kk     Fix to clean up RmNet and WDS states coherently.
01/31/12    wc     Support QSAP registration on multiple QMUX instances
01/05/12    sa     Changes for mew QMI CSVT service.
10/04/11    sa     Q6 free floating changes for QMI.
08/18/11    sa     GSDI cleanup changes.
06/22/11    wc     Send PRL init indication when PRL is loaded
06/21/11    kk     Added support for DUN entitlement and silent redial control.
12/17/10    kk     Removing DCC command buffer dependency from QMI services.
11/08/10    mct    Support for AKAv2 QMI messages.
06/11/10    mib    Added handler for CAT decoded command
06/07/10    kk     Fix to update app_type and uim_state from all possible
                   events.
10/23/09    ar     Add supprot for power save indication filtering and
                   port configuration from NVRAM.
10/08/09    kk     Windows7 compliance - DMS changes.
02/19/09    am     DS Task De-coupling effort and introduction of DCC task.
06/05/07    ks     Added qmi_task_set_qos_cmd_handler() to register cback to
                   be called to process flow events.
04/30/07    ks     Added new cmd to process flow events.
09/05/06    ks     Featurized pin support code, under FEATURE_MMGSDI.
09/05/06    ks     Added new cmds for UIM PIN1/PIN2 support.
07/11/06    ks     Renamed some functions and cmds.
07/06/06    ks     Added new cmds to process CM events in DS context.
09/08/05    ks     Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "amssassert.h"
#include "qmi_svc_defs.h"
#include "ds_qmi_defs.h"
#include "ds_qmux_ext.h"
#include "ds_qmux.h"
#include "ds_qmi_if.h"
#include "ds_qmi_task.h"
#include "ds_qmi_dms.h"
#include "ds_qmi_svc.h"
#include "ds_qmi_dsd.h"
#include "ds_qmi_int.h"
#ifdef FEATURE_DATA_ATP
  #include "ds_qmi_atp.h"
#endif /* FEATURE_DATA_ATP */
#include "ds_qmi_ctl.h"
#include "qmi_cmd_process.h"
#include "modem_mem.h"
#include "ds_system_heap.h"

#include "ds_rmnet_defs.h"

#ifdef FEATURE_DATA_QMI_AT
  #include "ds_qmi_at.h"
#endif /* FEATURE_DATA_QMI_AT */

#ifdef FEATURE_DATA_QMI_PDS_COUPLED
  #include "qmi_pds.h"
#endif /* FEATURE_DATA_QMI_PDS_COUPLED */


#ifdef FEATURE_DATA_QMI_EAP
  //#include "ds_qmi_auth.h"
#endif /* FEATURE_DATA_QMI_EAP */


#include "data_msg.h"
#include "ds_qmi_wds.h"
#include "ds_qmi_wda.h"
#include "ds_qmi_dfs.h"
#include "ds_qmi_nipd.h"

#define QMI_SAP_WAIT_SIG  0x00010000
#define QMI_SAP_TIMER_SIG 0x00000001

#ifdef FEATURE_QSAP_SERVICE
extern void qmi_sap_post_init_task_info(rex_tcb_type *tcb, rex_sigs_type wait_sig, rex_sigs_type timer_sig);
#endif /* FEATURE_QSAP_SERVICE */

struct
{
/* 
Since all the services are moved to the new QCCI-QCSI model this cmd_hdlr would 
not be neede for all other services apart from CTL to why to take a array of 
all services that would take around 1 KB
  qmi_task_svc_cmd_hdlr_cb_f_type  cmd_hdlr[QMUX_SERVICE_MAX];
  */
   qmi_task_svc_cmd_hdlr_cb_f_type  cmd_hdlr;

  //qmi_task_svc_cmd_hdlr_cb_f_type  common_cmd_hdlr;
  //qmi_task_svc_cmd_hdlr_cb_f_type  qmux_cmd_hdlr;
  //qmi_task_svc_cmd_hdlr_cb_f_type  qmap_cmd_hdlr;

} qmi_task_global = {0};

/*===========================================================================

                               DEFINITIONS

===========================================================================*/
// forward declaration of ds_qmi_otdt_init
// header file cannot be included since it might not be preset
int ds_qmi_otdt_init
(
  void
);
/*===========================================================================
  FUNCTION QMI_PROCESS_INIT_LEGACY_SERVICES()

  DESCRIPTION
    This function initializes the QMI legacy services running on DCC task.

  PARAMETERS
    cmd          :  DCC command to be processed
    user_data_ptr:  private data buffer containing the QMI command
                    information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI module must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_process_init_legacy_services
(
  ds_command_enum_type   cmd,              /* Command to be processed  */
  void *              user_data_ptr     /* Payload to be processed  */
)
{
  uint32 i = 0;
  qmi_cmd_init_legacy_services_type * dcc_cmd_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_ASSERT((int)user_data_ptr);

  dcc_cmd_ptr = (qmi_cmd_init_legacy_services_type*) user_data_ptr;

  DS_MSG1(MSG_LEGACY_MED,"Initialize legacy QMI services. Num of QMI instances %d",
                  dcc_cmd_ptr->num_qmi_instance);

  for(i = 0; i < MIN(dcc_cmd_ptr->num_qmi_instance, QMI_INSTANCE_MAX); i++)
  {
    /*-----------------------------------------------------------------------
      Initialize the QMI services running in dcc task
    -----------------------------------------------------------------------*/
    qmi_svc_ext_init((qmi_instance_e_type)i);
  }

 
#ifdef FEATURE_QSAP_SERVICE
  qmi_sap_post_init_task_info(rex_self(), QMI_SAP_WAIT_SIG, QMI_SAP_TIMER_SIG);
#endif /* FEATURE_QSAP_SERVICE */

  /*-----------------------------------------------------------------------
    Initialize new QMUX based QMI service
  -----------------------------------------------------------------------*/

  qmi_dms_init();	/* initialize QMI Device Management service */
  qmi_wds_init();
  qmi_wda_init();
  qmi_dsd_init();
  qmi_int_init();
#ifndef TEST_FRAMEWORK
#ifdef FEATURE_DATA_QMI_AT
#ifdef FEATURE_DATA_ATCOP
  qmi_at_init();	/* initialize QMI Access Terminal service */
#endif
#endif /* FEATURE_DATA_ATCOP */
#endif
/*  -----------------------------------------------------------------------*/
  if (!qmi_if_init())
  {
	DS_MSG0(MSG_LEGACY_ERROR,"Couldn't initialize QMI_IF module!");
  }

  /*-----------------------------------------------------------------------
    Since rmnet init is depend on qmux init, the initialization has been
    moved here.
  -----------------------------------------------------------------------*/
  (void)rmnet_sm_init();
  qmi_dfs_init();

  qmi_nipd_init();

  //qmi_wdslite_init();
} /* qmi_init_legacy_services */

/*===========================================================================
  FUNCTION QMI_PROCESS_LEGACY_CMD()

  DESCRIPTION
    This function processes a QMI cmd running on legacy services in DCC task.

    It is called by the serving task main loop and will dispatch the
    associated event handler function.

  PARAMETERS
    cmd          :  QMI command to be processed
    user_data_ptr:  private data buffer containing the QMI command
                    information.

  RETURN VALUE
    None

  DEPENDENCIES
    QMI module must already have been initialized

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_process_legacy_cmd
(
  ds_command_enum_type        cmd,
  void                   * user_data_ptr
)
{
  //dcc_cmd_data_buf_type  * cmd_ptr;
  uint16  *cmd_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  
  cmd_ptr = (uint16 *)user_data_ptr;
  DS_ASSERT((int)cmd_ptr);
  DS_ASSERT(cmd == DS_QMI_CMD);
  DS_ASSERT((*cmd_ptr) < QMI_CMD_MAX);

  //ds_qmi_cflog_amss_rsp( cmd_ptr );

  switch (*cmd_ptr)
  {
    /*
    case QMI_CMD_WDS_INIT_CB:
    case QMI_CMD_WDS_ALLOC_CLID_CB:
    case QMI_CMD_WDS_DEALLOC_CLID_CB:
    case QMI_CMD_WDS_CMD_HDLR_CB:
    case QMI_CMD_WDS_BIND_DATA_PORT:
    case QMI_CMD_SEND_PKT_STATS:
    case QMI_CMD_PROCESS_PHYS_LINK_EVT:
    case QMI_CMD_PROCESS_DUN_PHYS_LINK_EVT:
    case QMI_CMD_PROCESS_WDS_UM_IFACE_EVT:
    case QMI_CMD_PROCESS_DUN_CALL_INFO_EVT:
    case QMI_CMD_PROCESS_DATA_CALL_STATUS_EVT:
#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
    case QMI_CMD_PROCESS_CALL_THROTTLE_CB:
    case QMI_CMD_PROCESS_EVDO_PMP_CHANGE_EVT:
    case QMI_CMD_PROCESS_SET_EVDO_PMP_CB:
#endif /* defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900) 
    case QMI_CMD_PROCESS_DUN_RMSM_EVENT:
    case QMI_CMD_WDS_DATA_SYSTEM_STATUS_CHANGE_EVT:
    case QMI_CMD_SEND_FLOW_CTRL:
#if defined (FEATURE_DATA_LTE)
    case QMI_CMD_WDS_LTE_ATTACH_PDN_LIST_CHANGE_EVT:
#endif /* defined (FEATURE_DATA_LTE) 
    case QMI_CMD_PROCESS_WLAN_IFACE_EVT:
    case QMI_CMD_WDS_PROFILE_CHANGED_EVT:
    case QMI_CMD_WDS_THRPT_INFO_EVT:
    case QMI_CMD_WDS_ROAMING_INFO_EVT:
    case QMI_CMD_WDS_DL_THRPT_INFO_EVT:
    case QMI_CMD_WDS_GEN_REV_IP_CONNECTED_IND:
    case QMI_CMD_WDS_OUTAGE_NOTIFICATION_EVT:
    {
      if(qmi_task_global.cmd_hdlr[QMUX_SERVICE_WDS] != NULL)
      {
        (qmi_task_global.cmd_hdlr[QMUX_SERVICE_WDS])(user_data_ptr);
      }
      else
      {
        DS_MSG0(MSG_LEGACY_ERROR,"QMI WDS cmd hdlr NOT registered!");
      }
      break;
    }*/ //WDS handled in qcsi framework

	/* 
    case QMI_CMD_PROCESS_FLOW_EVT:
    case QMI_CMD_PROCESS_QOS_PHYS_LINK_EVT:
    case QMI_CMD_PROCESS_QOS_IFACE_EVT:
    case QMI_CMD_QOS_INIT_CB:
    case QMI_CMD_QOS_ALLOC_CLID_CB:
    case QMI_CMD_QOS_DEALLOC_CLID_CB:
    case QMI_CMD_QOS_CMD_HDLR_CB:
    case QMI_CMD_QOS_NW_STATUS_CHANGE_EVT:
    {
      if(qmi_task_global.cmd_hdlr[QMUX_SERVICE_QOS] != NULL)
      {
        (qmi_task_global.cmd_hdlr[QMUX_SERVICE_QOS])(user_data_ptr);
      }
      else
      {
        DS_MSG0(MSG_LEGACY_ERROR,"QMI QOS cmd hdlr NOT registered!");
      }
      break;
    }*/ //QOS not needed
/*
#ifdef FEATURE_MMGSDI_SESSION_LIB
    case QMI_CMD_MMGSDI_OPERATION_COMPLETE:
    case QMI_CMD_MMGSDI_SESSION_READY:
#endif /* FEATURE_MMGSDI_SESSION_LIB 
    case QMI_CMD_NEW_CM_SS_INFO:
    case QMI_CMD_NEW_CM_PH_INFO:
    case QMI_CMD_CM_CLIENT_ACTIVATE:
    case QMI_CMD_NEW_CM_OPRT_MODE_INFO:
    //case QMI_CMD_CM_PRL_INIT:
    //case QMI_CMD_CDMA_LOCK_MODE:
    case QMI_CMD_PROCESS_CALL_EVT:
    case QMI_CMD_PROCESS_OTKSL_FLAG_UPDATE:
      if(qmi_task_global.common_cmd_hdlr != NULL)
      {
        (qmi_task_global.common_cmd_hdlr)(user_data_ptr);
      }
      else

	  {
		DS_MSG0(MSG_LEGACY_ERROR,"QMI Common cmd hdlr NOT registered!");
      }

     break;*/

  #ifdef FEATURE_DATA_QMI_PDS_COUPLED
    case QMI_CMD_PDS_LBS_MSG:
      qmi_pds_process_command_from_lbs_api(&(cmd_ptr->cmd.qmi.data.pds.lbs_msg));
      break;
  #endif /* FEATURE_DATA_QMI_PDS_COUPLED */

  #ifdef FEATURE_DATA_QMI_EAP
  /*
    case QMI_CMD_EAP_SIM_AKA_TASK_SWITCH_EV:
    case QMI_CMD_SEND_EAP_RESP:
    case QMI_CMD_SEND_EAP_RESULT:
    case QMI_CMD_SEND_AKA_ALGO_RESULT:
    case QMI_CMD_AUTH_INIT_CB:
    case QMI_CMD_AUTH_ALLOC_CLID_CB:
    case QMI_CMD_AUTH_DEALLOC_CLID_CB:
    case QMI_CMD_AUTH_CMD_HDLR_CB:
    case QMI_CMD_EAP_NOTIFICATION_CODE:

      if(qmi_task_global.cmd_hdlr[QMUX_SERVICE_AUTH] != NULL)
      {
        (qmi_task_global.cmd_hdlr[QMUX_SERVICE_AUTH])(user_data_ptr);
      }
      else
      {
        DS_MSG0(MSG_LEGACY_ERROR,"QMI AUTH cmd hdlr NOT registered!");
      }
      break;*/// Auth not needed
  #endif /* FEATURE_DATA_QMI_EAP */

  #ifdef FEATURE_DATA_QMI_AT
  /*
    case QMI_CMD_AT_CLIENT_REG_STATUS:
    case QMI_CMD_AT_CMD_REG_STATUS:
    case QMI_CMD_AT_CMD_DEREG_STATUS:
    case QMI_CMD_FWD_AT_CMD:
    case QMI_CMD_AT_ABORT_CMD:
    case QMI_CMD_AT_INIT_CB:
    case QMI_CMD_AT_ALLOC_CLID_CB:
    case QMI_CMD_AT_DEALLOC_CLID_CB:
    case QMI_CMD_AT_CMD_HDLR_CB:
    {
      if(qmi_task_global.cmd_hdlr[QMUX_SERVICE_AT] != NULL)
      {
        (qmi_task_global.cmd_hdlr[QMUX_SERVICE_AT])(user_data_ptr);
      }
      else
      {
        DS_MSG0(MSG_LEGACY_ERROR,"QMI AT cmd hdlr NOT registered!");
      }
      break;
    }*/ //AT handled in qcsi framework
  #endif /* FEATURE_DATA_QMI_AT */
    /*
    case QMI_CMD_WDA_INIT_CB:
    case QMI_CMD_WDA_ALLOC_CLID_CB:
    case QMI_CMD_WDA_DEALLOC_CLID_CB:
    case QMI_CMD_WDA_CMD_HDLR_CB:

      if(qmi_task_global.cmd_hdlr[QMUX_SERVICE_WDA] != NULL)
      {
        (qmi_task_global.cmd_hdlr[QMUX_SERVICE_WDA])(user_data_ptr);
      }
      else
      {
        DS_MSG0(MSG_LEGACY_ERROR,"QMI WDA cmd hdlr NOT registered!");
      }
      break;*/ //WDA handled in qcsi framework
#if (defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS))
/*
    case QMI_CMD_PROCESS_CSVT_CALL_CONNECT_EVT:
    case QMI_CMD_PROCESS_CSVT_PROGRESS_INFO_EVT:
    case QMI_CMD_PROCESS_CSVT_CONFIRM_INFO_EVT:
    case QMI_CMD_PROCESS_CSVT_INCOMING_INFO_EVT:
    case QMI_CMD_PROCESS_CSVT_SETUP_INFO_EVT:
    case QMI_CMD_PROCESS_CSVT_END_INFO_EVT:
    case QMI_CMD_PROCESS_CSVT_MODIFY_INFO_EVT:
    case QMI_CMD_CSVT_INIT_CB:
    case QMI_CMD_CSVT_ALLOC_CLID_CB:
    case QMI_CMD_CSVT_DEALLOC_CLID_CB:
    case QMI_CMD_CSVT_CMD_HDLR_CB:

      if(qmi_task_global.cmd_hdlr[QMUX_SERVICE_CSVT] != NULL)
      {
        (qmi_task_global.cmd_hdlr[QMUX_SERVICE_CSVT])(user_data_ptr);
      }
      else
      {
        DS_MSG0(MSG_LEGACY_ERROR,"QMI CSVT cmd hdlr NOT registered!");
      }
      break;*/ //CSVT not needed
#endif /* (defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)) */
    case QMI_CMD_LEGACY_SVC_ALLOC_CLID:
    case QMI_CMD_LEGACY_SVC_DEALLOC_CLID:

      /*if(qmi_task_global.cmd_hdlr[QMUX_SERVICE_CTL] != NULL)
      {
        (qmi_task_global.cmd_hdlr[QMUX_SERVICE_CTL])(user_data_ptr);
      }process the CTL cmd handler without indexing as we have not taken any 
      array now*/
      
      if(qmi_task_global.cmd_hdlr != NULL)
      {
        qmi_task_global.cmd_hdlr(user_data_ptr);
      }
      else
      {
	   DS_MSG0(MSG_LEGACY_ERROR,"QMI CTL cmd hdlr NOT registered!");
	  }
	  break;
      /*  QMUX and QMAP handled in qmi_if.c
    case QMI_CMD_LEGACY_SVC_QMI_LINK_CLOSED:
      if(qmi_task_global.qmux_cmd_hdlr != NULL)
      {
        (qmi_task_global.qmux_cmd_hdlr)(user_data_ptr);
      }
      else
      {
        DS_MSG0(MSG_LEGACY_ERROR,"QMI QMUX cmd hdlr NOT registered!");
      }
      break;

    case QMI_CMD_QMAP_CMD_RESP_CB:
    case QMI_CMD_QMAP_CMD_TIMER_EXP_CB:

      if (qmi_task_global.qmap_cmd_hdlr != NULL)
      {
        (qmi_task_global.qmap_cmd_hdlr)(user_data_ptr);
      }
      else
      {
        DS_MSG0(MSG_LEGACY_ERROR,"QMI QMAP cmd hdlr NOT registered!");
      }
      break;*/

    default:
	 DS_MSG1(MSG_LEGACY_ERROR,"Ignoring QMI cmd %d", *cmd_ptr);
	  break;
  }
} /* qmi_process_legacy_cmd() */

/*===========================================================================
  FUNCTION QMI_TASK_SET_SVC_CMD_HANDLER()

  DESCRIPTION
    Specify the callback function that should be invoked when we get a QMI CMD
    for a given service

  PARAMETERS
    svc      : QMI service type for which the callback is being set
    cmd_hdlr : Command handler to invoke when a command is received

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
    
===========================================================================*/

//Should be called only for the CTL service earlier was called for all the 
// legacy services in QMI but now all the services are moved to new QCCI model
void  qmi_task_set_svc_cmd_handler
(
  qmux_service_e_type              svc,
  qmi_task_svc_cmd_hdlr_cb_f_type  cmd_hdlr_cb
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT((int)cmd_hdlr_cb);
  DS_ASSERT(svc < QMUX_SERVICE_MAX);
  DS_ASSERT(svc ==  QMUX_SERVICE_CTL);

  /* check that svc is within range in case ASSERT is removed */
  if (svc >= QMUX_SERVICE_MAX)
  {
   DS_MSG1(MSG_LEGACY_ERROR,"QMI svc out of range (%d)!  Discarding", svc);
   return;
  }

  qmi_task_global.cmd_hdlr = cmd_hdlr_cb;
} /* qmi_task_set_svc_cmd_handler */

#if 0
//Done in qmi_if.c
/*===========================================================================
  FUNCTION QMI_TASK_SET_COMMON_CMD_HANDLER

  DESCRIPTION
    Specify the callback function that should be invoked when we get a QMI CMD

  PARAMETERS
    cmd_hdlr : Command handler to invoke when a command is received

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void  qmi_task_set_common_cmd_handler
(
  qmi_task_svc_cmd_hdlr_cb_f_type  cmd_hdlr_cb
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(cmd_hdlr_cb);
  qmi_task_global.common_cmd_hdlr = cmd_hdlr_cb;

} /* qmi_task_set_common_cmd_handler() */


/*===========================================================================
  FUNCTION QMI_TASK_SET_QMUX_CMD_HANDLER

  DESCRIPTION
    Specify the callback function that should be invoked when we get a
    QMUX CMD

  PARAMETERS
    cmd_hdlr : Command handler to invoke when a command is received

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void  qmi_task_set_qmux_cmd_handler
(
  qmi_task_svc_cmd_hdlr_cb_f_type  cmd_hdlr_cb
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(cmd_hdlr_cb);
  qmi_task_global.qmux_cmd_hdlr = cmd_hdlr_cb;

} /* qmi_task_set_common_cmd_handler() */

/*===========================================================================
  FUNCTION QMI_TASK_SET_QMAP_CMD_HANDLER

  DESCRIPTION
    Specify the callback function that should be invoked when we get a
    QMAP CMD

  PARAMETERS
    cmd_hdlr : Command handler to invoke when a command is received

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void  qmi_task_set_qmap_cmd_handler
(
  qmi_task_svc_cmd_hdlr_cb_f_type  cmd_hdlr_cb
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(cmd_hdlr_cb);
  qmi_task_global.qmap_cmd_hdlr = cmd_hdlr_cb;

} /* qmi_task_set_qmap_cmd_handler() */
#endif
