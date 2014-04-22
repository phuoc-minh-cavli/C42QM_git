#ifndef QCMAP_H
#define QCMAP_H
/*===========================================================================
                         QCMAP.H

DESCRIPTION
   This header file contains definitions used internally by the QCMAP Module. 
  
   Copyright (c) 2003 - 2022 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

#include "comdef.h"
#include "dsm.h"
#include "sio.h"
#include "rdevmap.h"
#include "stdlib.h"
#include "qurt_signal.h"
#include "qurt_thread.h"
#include "qurt_error.h"
#include "err.h"
#include "qurt.h"
#include "msgcfg.h"
#include "amssassert.h"
#include "rcinit.h"
#include "qmi_client.h"
#include "qmi_cci_target_ext.h"
#include "qmi_idl_lib.h"
#include "wireless_data_service_common_v01.h"
#include "stdint.h"
#include "qurt_mutex.h"
#include "qcmap_ext_api.h"
#include "ds_log.h"

#define TRUE                                   1
#define FALSE                                  0
#define QCMAP_SUCCESS                          0
#define QCMAP_FAILURE                         -1
#define QCMAP_QMI_CLIENT_MAX_RETRY             10
#define QCMAP_QMI_MSG_TIMEOUT_VALUE            1000
#define QCMAP_IPPORT_RESERVED 				   1024
#define QCMAP_MAX_NWSTACK_SOCKETS              20


/*------------------------------------------------------------------------------
  Typedef Declaration
------------------------------------------------------------------------------*/
typedef qurt_signal_t qcmap_signal_t;

/*------------------------------------------------------------------------------
  This is the command queue that will be used to enqueue commands from
  external entities
------------------------------------------------------------------------------*/
typedef q_type qcmap_task_cmd_q;


typedef enum
{
  QCMAP_MIN_CMD_EVT,
  QCMAP_MOBILEAP_ENABLE_REQ_EVT,
  QCMAP_MOBILEAP_ENABLE_RESP_EVT,
  QCMAP_MOBILEAP_ENABLE_IND_EVT,
  QCMAP_MOBILEAP_DISABLE_REQ_EVT,
  QCMAP_MOBILEAP_DISABLE_RESP_EVT,
  QCMAP_MOBILEAP_DISABLE_IND_EVT,
  QCMAP_MOBILEAP_BRING_UP_WWAN_REQ_EVT,
  QCMAP_MOBILEAP_BRING_UP_WWAN_RESP_EVT,
  QCMAP_MOBILEAP_BRING_UP_WWAN_IND_EVT,
  QCMAP_MOBILEAP_TEAR_DOWN_WWAN_REQ_EVT,
  QCMAP_MOBILEAP_TEAR_DOWN_WWAN_RESP_EVT,
  QCMAP_MOBILEAP_TEAR_DOWN_WWAN_IND_EVT,
  QCMAP_MOBILEAP_ENABLE_BACKHAUL_V4_REQ_EVT,
  QCMAP_MOBILEAP_ENABLE_BACKHAUL_V4_RESP_EVT,
  QCMAP_MOBILEAP_ENABLE_BACKHAUL_V4_IND_EVT,
  QCMAP_MOBILEAP_ENABLE_BACKHAUL_V6_REQ_EVT,
  QCMAP_MOBILEAP_ENABLE_BACKHAUL_V6_RESP_EVT,
  QCMAP_MOBILEAP_ENABLE_BACKHAUL_V6_IND_EVT,
  QCMAP_MOBILEAP_DISABLE_BACKHAUL_V4_REQ_EVT,
  QCMAP_MOBILEAP_DISABLE_BACKHAUL_V4_RESP_EVT,
  QCMAP_MOBILEAP_DISABLE_BACKHAUL_V4_IND_EVT,
  QCMAP_MOBILEAP_DISABLE_BACKHAUL_V6_REQ_EVT,
  QCMAP_MOBILEAP_DISABLE_BACKHAUL_V6_RESP_EVT,
  QCMAP_MOBILEAP_DISABLE_BACKHAUL_V6_IND_EVT,
  QCMAP_MOBILEAP_SET_AUTOCONNECT_REQ_EVT,
  QCMAP_MOBILEAP_SET_AUTOCONNECT_RESP_EVT,
  QCMAP_MOBILEAP_GET_AUTOCONNECT_RESP_EVT,
  QCMAP_MOBILEAP_GET_AUTOCONNECT_REQ_EVT,
  QCMAP_MOBILEAP_SET_ROAMING_REQ_EVT,
  QCMAP_MOBILEAP_SET_ROAMING_RESP_EVT,
  QCMAP_MOBILEAP_GET_ROAMING_REQ_EVT,
  QCMAP_MOBILEAP_GET_ROAMING_RESP_EVT,
  QCMAP_MOBILEAP_SET_DMZIP_REQ_EVT,
  QCMAP_MOBILEAP_SET_DMZIP_RESP_EVT,
  QCMAP_MOBILEAP_DELETE_DMZIP_REQ_EVT,
  QCMAP_MOBILEAP_DELETE_DMZIP_RESP_EVT,
  QCMAP_QMI_NAS_IN_SERVICE_EVT,
  QCMAP_QTI_EVT,
  QCMAP_DTR_CHANGE_EVT,
  QCMAP_QMI_NAS_IND_EVT,
  QCMAP_DSS_IND_EVT,
  QCMAP_V4_TIMEOUT_EVT,
  QCMAP_V6_TIMEOUT_EVT,
  QCMAP_MOBILEAP_SET_DHCP_REQ_EVT,
  QCMAP_MOBILEAP_SET_DHCP_RESP_EVT,
  QCMAP_MAX_CMD_EVT
} qcmap_cmd_id_e;


/*------------------------------------------------------------------------------
  Structure Declaration
------------------------------------------------------------------------------*/

typedef struct
{
  qcmap_signal_t              qcmap_signal;
  qurt_mutex_t                qcmap_mutex;
  q_type*                     cmd_q_ptr;
}qcmap_config_info_t;


typedef struct{
  q_link_type                   link;
  qcmap_cmd_id_e                cmd_id;
} qcmap_cmd_hdr_t;


typedef struct{
  void * data;
  uint8 connection_handle;
} qcmap_cmd_data_t;


typedef struct{
  qcmap_cmd_hdr_t               cmd_hdr;
  qcmap_cmd_data_t              cmd_data;
} qcmap_cmd_t;


/*------------------------------------------------------------------------------
  Signal Related Macros
------------------------------------------------------------------------------*/
#define QCMAP_SIGNAL_MASK       0x00000001

#define QCMAP_TASK_IS_SIG_SET(signal, signal_mask)  ((signal & signal_mask) != 0)

#define QCMAP_TASK_CLEAR_SIG(signal, signal_mask) \
                                    qurt_signal_clear(&signal,signal_mask)

#define QCMAP_TASK_SET_SIG(signal, signal_mask) \
                                    qurt_signal_set(&signal, signal_mask)

/*------------------------------------------------------------------------------
  Command Queue Related Macros
------------------------------------------------------------------------------*/

#define QCMAP_CMD_ID_RANGE_CHECK(cmd_id) \
                                       ( (cmd_id > QCMAP_MIN_CMD_EVT) && \
                                         (cmd_id < QCMAP_MAX_CMD_EVT) )


/*------------------------------------------------------------------------------
  Log Message Macros
------------------------------------------------------------------------------*/

#define QCMAP_LOG_MSG_LOW(fmtstr, arg1, arg2, arg3) \
			ds_iot_log_low3(fmtstr, arg1, arg2, arg3)
#define QCMAP_LOG_MSG_LOW_SPRINT(fmtstr, arg1, arg2, arg3) \
			ds_iot_log_strf_low(fmtstr, arg1, arg2, arg3)
            
#define QCMAP_LOG_MSG_MED(fmtstr, arg1, arg2, arg3) \
			ds_iot_log_med3(fmtstr, arg1, arg2, arg3)
#define QCMAP_LOG_MSG_MED_SPRINT(fmtstr, arg1, arg2, arg3) \
			ds_iot_log_strf_med(fmtstr, arg1, arg2, arg3)
			
#define QCMAP_LOG_MSG_HIGH(fmtstr, arg1, arg2, arg3) \
			ds_iot_log_high3(fmtstr, arg1, arg2, arg3)
#define QCMAP_LOG_MSG_HIGH_SPRINT(fmtstr, arg1, arg2, arg3) \
			ds_iot_log_strf_high(fmtstr, arg1, arg2, arg3)
            
#define QCMAP_LOG_MSG_ERROR(fmtstr, arg1, arg2, arg3) \
			ds_iot_log_err3(fmtstr, arg1, arg2, arg3)
#define QCMAP_LOG_MSG_ERROR_SPRINT(fmtstr, arg1, arg2, arg3) \
			ds_iot_log_strf_err(fmtstr, arg1, arg2, arg3)
			
#define QCMAP_LOG(...)                         \
  fprintf( stderr, "%s %d:", __FILE__, __LINE__); \
  fprintf( stderr, __VA_ARGS__ )

#define QCMAP_LOG_FUNC_ENTRY()  \
    QCMAP_LOG                   \
    (                              \
        "Entering function %s",  \
        __FUNCTION__               \
    )



/*------------------------------------------------------------------------------
  Function Declaration
------------------------------------------------------------------------------*/

/*=============================================================================

FUNCTION QCMAP_CLIENT_REG_CB

DESCRIPTION
  Callback Exposed for QCMAP Clients to register a Function which will be used
  for QCMAP Task -> Client Communication
DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void qcmap_client_reg_cb
(
  qcmap_client_resp_ind_cb cb_ptr,
  uint8                    *connection_handle
);

/*=============================================================================

FUNCTION QCMAP_CLIENT_REG_CB

DESCRIPTION
  Callback Exposed for QCMAP Clients to send various event for MobileAP 
  functionality

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void qcmap_evt_handler_cb
(
  qcmap_cmd_id_e evt,
  void * data,
  uint8  connection_handle
);


/*=============================================================================

FUNCTION QTI_QCMAP_CB

DESCRIPTION
  Callback Exposed for QTI to send the peripheral stream ID


DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void qti_qcmap_cb
(
  sio_stream_id_type          qti_ph_stream_id
);


/*==============================================================================
FUNCTION QCMAP_PROCESS_COMMANDS()

DESCRIPTION
  This function dequeues all outstanding commands from and dispatches the
  processor functions.

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES 
  None 

SIDE EFFECTS
  None
==============================================================================*/
void qcmap_process_commands(void);


/*===========================================================================

FUNCTION QCMAP_CMDQ_INIT()

DESCRIPTION
  Initialize the command queues and set the command queue pointers in the
  qcmap_config_info struct to the appropriate values.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

=========================================================================*/
void qcmap_cmdq_init (qcmap_config_info_t * qcmap_state);

/*===========================================================================

FUNCTION QCMAP_GET_CMD_BUFFER

DESCRIPTION
  This function is used to allocate a command buffer that the client can
  then enqueue into the command queue.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the command

SIDE EFFECTS
  None

=========================================================================*/
qcmap_cmd_t* qcmap_get_cmd_buffer(void);

/*===========================================================================

FUNCTION  QCMAP_ENQUEUE_CMD

DESCRIPTION
  This function is used to enqueue the command into the QCMAP queue

DEPENDENCIES
  None.

PARAMETERS
  Pointer to the command to enqueue

RETURN VALUE
  Void

SIDE EFFECTS
  None

=========================================================================*/
void qcmap_enqueue_cmd(qcmap_cmd_t* cmd_ptr);

/*=============================================================================

FUNCTION QCMAP_SET_SIGNAL

DESCRIPTION
  Set the qcmap signal using lock protection

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void qcmap_set_signal(void);

/*================================================================

FUNCTION QCMAP_READ_CONFIG_FILE

DESCRIPTION
  Read config file from EFS
  
DEPENDENCIES
  None.

RETURN VALUE
  Sucess/Failure

SIDE EFFECTS
  None

==================================================================*/

int qcmap_read_config_file(void );

/*================================================================

FUNCTION QCMAP_READ_DEFAULT_CONFIG_FILE

DESCRIPTION
  Default config file in case of missing or corrupted EFS file.
  
DEPENDENCIES
  None.

RETURN VALUE
  Sucess/Failure

SIDE EFFECTS
  None

==================================================================*/

int qcmap_read_default_config_file(void);


#endif /* QCMAP_H */

