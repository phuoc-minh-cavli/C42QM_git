/*===========================================================================
                         QTI_AT_DUN_BRIDGE.C

DESCRIPTION
  This  file contains definitions used  by the QTI AT DUN  Module.

  Copyright (c)2016-2021 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/

#include "ds_log.h"
#include "at_fwd_ext_api.h"
#include "ds_mux_taski.h"
#include "qti_bridge.h"
#include "ds_mux_platform.h"
#include "timer.h"
#ifdef QTI_LOGGING_ENABLED
#include "ULogFront.h"
#endif

qti_bridge_info_t  qti_bridge_info_table[QTI_BRIDGES_MAX];

qti_bridge_qurt_t  qti_bridge_qurt;

//global declaration of stracture used to store cmux parameters/tokens received from TE
uint8 *input_at_params;

at_fwd_cmux_cmd_params_struct *cmux_params=NULL;

uint8  at_mask = 0;

uint32 at_smd_port = 0;

boolean ds_mux_bridge_active;

boolean ds_mux_first_init_done;

boolean  at_cmux_handle_in_progress;

#ifdef QTI_LOGGING_ENABLED
ULogHandle qti_ulog;
#endif

qurt_thread_t qti_bridge_thread_id = 0;
qurt_thread_t get_qti_bridge_task_thread_id(void);

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled signals
---------------------------------------------------------------------------*/
uint32 bridge_mux_enabled_sig_mask = 0;


/*==============================================================================

FUNCTION QTI_BRIDGE_AT_CMD_HANDLER_CB

DESCRIPTION
  Callback function registered with ATFwD.
  This callback will be invoked to handle any AT Command which QTI Bridge module
  handles (CMUX,IPR,QCEMBP)

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_at_cmd_handler_cb(boolean is_reg, char *atcmd_name,
                                             uint8* at_fwd_params, uint8 mask,
                                             uint32 at_handle)
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;

  ds_iot_log_high ("qti_bridge_at_cmd_handler_cb() ");

  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer");
    return;
  }
  else
  {

    if (mask == QTI_BRIDGE_AT_MASK_EMPTY)
    {
      ds_iot_log_strf_high("Last registered AT Command %s ",
                               atcmd_name);
      return;
    }
		
    cmd_ptr->cmd_hdr.cmd_id        = QTI_BRIDGE_ATFWD_EVT;

    if (strncasecmp (atcmd_name,QTI_BRIDGE_CMUX,strlen(QTI_BRIDGE_CMUX)) == 0)
    {
      cmd_ptr->data.bridge_id = QTI_BRIDGE_AT_CMD_CMUX;
    }
    else if (strncasecmp (atcmd_name,QTI_BRIDGE_IPR,strlen(QTI_BRIDGE_IPR)) == 0)
    {
      cmd_ptr->data.bridge_id = QTI_BRIDGE_AT_CMD_IPR;
    }
	 else if (strncasecmp (atcmd_name,QTI_BRIDGE_QCEMBP,strlen(QTI_BRIDGE_QCEMBP)) == 0)
    {
      cmd_ptr->data.bridge_id = QTI_BRIDGE_AT_CMD_QCEMBP;
    }
	

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  at_mask = mask;
  at_smd_port = at_handle;
  input_at_params = (uint8*)malloc(QTI_BRIDGE_MAX_AT_TOKENS);

  if (input_at_params == NULL) 
  {
    ds_iot_log_err("at_fwd_mux_params buffer allocation failed");
    free(cmd_ptr);
    cmd_ptr = NULL;
    return;
  }
  if (at_fwd_params != NULL)
  {
    memcpy(input_at_params,at_fwd_params,QTI_BRIDGE_MAX_AT_TOKENS);
  }

  qti_bridge_set_signal();

  return;
}

/*==============================================================================

FUNCTION QTI_BRIDGE_CMUX_POST_PROCESS_CB

DESCRIPTION
  callback function invoked to do post processing of CMUX cmd

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/

void qti_bridge_cmux_post_process_cb (uint8 * arg1, uint8 arg2, char* arg3, uint32 arg4)
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;
  UNUSED(arg1);
  UNUSED(arg2);
  UNUSED(arg3);
  UNUSED(arg4);
  
  ds_iot_log_high ("qti_bridge_cmux_post_process_cb() ");

  /* To let the CMUX OK response being sent first and then start cmux active init */
  timer_sleep(100, T_MSEC, 1);

  at_cmux_handle_in_progress = TRUE;
  
  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    return;
  }
  else
  {
    ds_iot_log_high("CMDQ Buffer allocated");

    cmd_ptr->cmd_hdr.cmd_id      = QTI_BRIDGE_CMUX_POST_PROCESS_EVT;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}

/*==============================================================================

FUNCTION QTI_BRIDGE_AT_CMD_POST_PROCESS_CB

DESCRIPTION
  callback function invoked to do post processing of AT cmd

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_at_cmd_post_process_cb (void)
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;

  ds_iot_log_high ("qti_bridge_at_cmd_post_process_cb() ");
  
  /* To let the OK response being sent first and then change baud rate */
  timer_sleep(100, T_MSEC, 1);

  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    return;
  }
  else
  {
    ds_iot_log_high("CMDQ Buffer allocated");

    cmd_ptr->cmd_hdr.cmd_id      = QTI_BRIDGE_AT_POST_PROCESS_EVT;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}


/*==============================================================================

FUNCTION QTI_BRIDGE_RDM_OPEN_CB

DESCRIPTION
  RDM invokes this callback when it has enumerated devices associated
  with DUN service transport.

DEPENDENCIES
  None.

RETURN VALUE
  uint32 to indicate Init API successfully invoked

SIDE EFFECTS
  None

==============================================================================*/

void qti_bridge_rdm_open_cb(sio_port_id_type port_id, void *data)
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;

  ds_iot_log_high ("qti_bridge_rdm_open_cb() ");

  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    ASSERT(0);
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id      = QTI_BRIDGE_RDM_OPEN_EVT;
    cmd_ptr->data.port_id        = port_id;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}

/*==============================================================================

FUNCTION QTI_BRIDGE_RDM_CLOSE_CB

DESCRIPTION
  RDM invokes this callback when it wants to close SIO port
  associated with DUN service transport.

DEPENDENCIES
  None.

RETURN VALUE
  uint32 to indicate Init API successfully invoked

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_rdm_close_cb(sio_port_id_type port_id, void *data)
{

  qti_bridge_cmd_t*             cmd_ptr = NULL;

  ds_iot_log_high("qti_bridge_rdm_close_cb() ");

  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    ASSERT(0);
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id      = QTI_BRIDGE_RDM_CLOSE_EVT;
    cmd_ptr->data.port_id       = port_id;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}

/*=============================================================================

FUNCTION QTI_MUTEX_INIT

DESCRIPTION
  QTI Bridge mutex Initialization

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void qti_bridge_mutex_init(void)
{
  ds_iot_log_high ("qti_bridge_mutex_init ");
  qurt_mutex_init(&qti_bridge_qurt.qti_bridge_mutex);
  return;
}

/*=============================================================================

FUNCTION QTI_BRIDGE_SIGNAL_INIT

DESCRIPTION
  QTI Bridge Signal Initialization

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void qti_bridge_signal_init(void)
{
  qurt_signal_init(&qti_bridge_qurt.qti_bridge_signal);
  return;
}

/*=============================================================================

FUNCTION QTI_BRIDGE_SIGNAL_MASK_INIT

DESCRIPTION
  QTI Bridge Signal Mask Initialization

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void qti_bridge_signal_mask_init(void)
{
  ds_iot_log_high ("qti_bridge_signal_mask_init ");
  bridge_mux_enabled_sig_mask |= QTI_BRIDGE_SIGNAL_MASK;
  return;
}


/*=============================================================================

FUNCTION QTI_BRIDGE_SIGNAL_WAIT

DESCRIPTION
  This API will wait on the QTI  Bridge signal.
  The QTI Bridge signal will be set during various operations.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void qti_bridge_signal_wait(void)
{
  uint32_t received_sigs = 0;

  uint32_t                  set_sigs = 0;   /* Currently set signals               */
  uint32                  loop_sigs = 0;
  uint32_t                  num = 0;
  uint32_t                  i;
  uint32                  sig_buffer[DS_MUX_MAX_SIGNALS];
  ds_mux_sig_enum_type      curr_sig;

  ds_iot_log_high("qti_bridge_signal_wait ");

  while (1)
  {
/*============================================================================
  Wait for the QTI Signal to be set
=============================================================================*/
    received_sigs = qurt_signal_wait(&qti_bridge_qurt.qti_bridge_signal,
                    bridge_mux_enabled_sig_mask, QURT_SIGNAL_ATTR_WAIT_ANY);

    if (QTI_BRIDGE_TASK_IS_SIG_SET (received_sigs, QTI_BRIDGE_SIGNAL_MASK))
    {
/*============================================================================
  Clear the QTI signal **Need To add mutex in the correct place**
=============================================================================*/

      qurt_mutex_lock(&qti_bridge_qurt.qti_bridge_mutex);

      QTI_BRIDGE_TASK_CLEAR_SIG(qti_bridge_qurt.qti_bridge_signal, received_sigs);

      qurt_mutex_unlock(&qti_bridge_qurt.qti_bridge_mutex);

      // QTI Signal is set Dequeue the QTI Command queue
      qti_bridge_process_commands();
    }
    else
    {
/*============================================================================
  Clear the QTI signal **Need To add mutex in the correct place**
 =============================================================================*/

      received_sigs &= bridge_mux_enabled_sig_mask;

      qurt_mutex_lock(&qti_bridge_qurt.qti_bridge_mutex);

      QTI_BRIDGE_TASK_CLEAR_SIG(qti_bridge_qurt.qti_bridge_signal, received_sigs);
 
      qurt_mutex_unlock(&qti_bridge_qurt.qti_bridge_mutex);

    }

    loop_sigs = received_sigs & ~( 1 << DS_MUX_SIG_MASK_CHANGE_SIGNAL );
    received_sigs = 0;

    /*-----------------------------------------------------------------------
      Now handle the active signals one by one.
    -----------------------------------------------------------------------*/
    num = ds_mux_find_set_bits(&loop_sigs, 1, sig_buffer);
    for( i=0; i < num; i++)
    {
      curr_sig = (ds_mux_sig_enum_type)sig_buffer[i];
      loop_sigs &= ~( 1 << curr_sig );

      if ( ds_mux_sig_handler[curr_sig].sig_handler
            (
              curr_sig ,
              ds_mux_sig_handler[curr_sig].user_data_ptr
            ) == FALSE )
      {
        set_sigs |= (  1 <<  curr_sig);
      }
    }
  }
}


/*==============================================================================

FUNCTION QTI_BRIDGE_SET_SIGNAL

DESCRIPTION
  Set the SIGNAL for QTI Bridge

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_set_signal(void)
{
  qurt_mutex_lock(&qti_bridge_qurt.qti_bridge_mutex);

  QTI_BRIDGE_TASK_SET_SIG(qti_bridge_qurt.qti_bridge_signal,QTI_BRIDGE_SIGNAL_MASK);

  qurt_mutex_unlock(&qti_bridge_qurt.qti_bridge_mutex);
}

/*==============================================================================

FUNCTION QTI_BRIDGE_TASK_INI

DESCRIPTION
  Register callbacks with RDM framework to detect the peripheral availability

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_task_init(void)
{
  /* Initialize smd_bridge_server_info's SMD/PHY stream IDs */
  int i;
#ifdef QTI_LOGGING_ENABLED
  // Initialization 
  ULogFront_RealTimeInit(&qti_ulog,       // The log to create
                         "QTI Log",       // The log name
                                           // The amount of memory to 
                                           // allocate
                         5*1024,
                         ULOG_MEMORY_LOCAL,  // Local memory (normal case)
                         ULOG_LOCK_OS );     // Use a standard OS lock
#endif
  ds_iot_log_high ("qti_bridge_rdm_registration_init ");

  memset (&qti_bridge_info_table, 0 , sizeof(qti_bridge_info_table));

  for(i = 0; i < QTI_BRIDGES_MAX; i++)
  {
    qti_bridge_info_table[i].smd_stream_id = SIO_NO_STREAM_ID;
    qti_bridge_info_table[i].phys_stream_id = SIO_NO_STREAM_ID;
    qti_bridge_info_table[i].uart_stream_id = SIO_NO_STREAM_ID;
  }

  /* Register DUN with RDM */
  rdm_register_func_multi_dev(RDM_DUN_SRVC, qti_bridge_rdm_open_cb,
                              qti_bridge_rdm_close_cb, NULL);

  rdm_set_compatibility(RDM_DUN_SRVC, RDM_USB_MDM_DEV);
  rdm_set_compatibility(RDM_DUN_SRVC, RDM_USB_MDM2_DEV);
  rdm_set_compatibility(RDM_DUN_SRVC, RDM_UART2_DEV);
#ifdef QTI_LOGGING_ENABLED
  ULogFront_RealTimePrintf(qti_ulog, 0, "qbci");
#endif
  qti_bridge_cmdq_init();
  
  at_reg_qmi_serv_cb((at_qmi_service_up_ind_cb_type)qti_bridge_qmi_service_up_cb);

  qti_bridge_set_dne_wm_level();

  ds_mux_bridge_active = FALSE;
  ds_mux_powerup();
}


/*==============================================================================

FUNCTION QTI_AT_DUN_BRIDGE_INIT

DESCRIPTION
  Init API called from SIO layer to initialize the QTI AT DUN Bridge

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_task_entry(sio_register_device_driver_cb_type unused)
{
  ds_iot_log_high ("qti_bridge_task_entry ");

/*============================================================================
  Handshake with RCInit
  Indicate to the Task start framework that we have completed the task start
  procedures.
  If any synchronization is needed with other tasks during the task start time
  we should send an event indicating what we wish to communicate.
  =============================================================================*/

  rcinit_handshake_startup();

/*=============================================================================
  Initialize Mutex Object
==============================================================================*/

  qti_bridge_mutex_init();

/*=============================================================================
  Initialize Signal
==============================================================================*/

  qti_bridge_signal_init ();

/*=============================================================================
  Initialize Signal Mask for QTI Bridge operation
==============================================================================*/
  qti_bridge_signal_mask_init ();

/*=============================================================================
  Wait on the QTI Bridge Signal
==============================================================================*/
  qti_bridge_thread_id = qurt_thread_get_id();

  ds_mux_init();
  qti_bridge_signal_wait();
}

/*==============================================================================

FUNCTION QTI_BRIDGE_QMI_SERVICE_UP_CB

DESCRIPTION
  Function invoked for QMI service UP ind from atfwd

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/

void qti_bridge_qmi_service_up_cb ()
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;

  ds_iot_log_high ("qti_bridge_qmi_service_up_cb() ");

  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    return;
  }
  else
  {
    ds_iot_log_high("CMDQ Buffer allocated");

    cmd_ptr->cmd_hdr.cmd_id      = QTI_BRIDGE_ATFWD_QMI_SERVICE_UP_ENT;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}

/*==============================================================================

FUNCTION QTI_BRIDGE_REG_AT_CMD

DESCRIPTION
  Function invoked to register AT cmds

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_reg_at_cmd ()
{

/*
 * Register a callback function with ATFWD which will be invoked when AT+CMUX
 * or AT+IPR is sent from TE
 */
  qapi_atfwd_reg("+CMUX;+IPR;$QCEMBP",qti_bridge_at_cmd_handler_cb);
  at_fwd_reg_at_cmux_cb((at_cmux_handler_cb_type)qti_bridge_cmux_post_process_cb,
                        (at_post_process_cb_type)qti_bridge_at_cmd_post_process_cb);
}

qurt_thread_t get_qti_bridge_task_thread_id(void)
{
    return qti_bridge_thread_id;
}
