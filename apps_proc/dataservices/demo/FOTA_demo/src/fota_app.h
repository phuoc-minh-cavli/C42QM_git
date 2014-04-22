/*===========================================================================
                         FOTA_APP.H

DESCRIPTION
   This header file contains definitions used internally by the fota object test
   Module. 
  
   Copyright (c) 2021 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/
#include "stdlib.h"
#include "stdint.h"
#include <stdarg.h>
#include "qapi_lwm2m.h"
#include "qapi_diag.h"
#include "qapi_timer.h"
#include "time.h"
#include "qurt_signal.h"
#include "qurt_thread.h"
#include "qurt_error.h"
#include "qurt.h"
#include "msgcfg.h"
#include "rcinit.h"
#include "qurt_mutex.h"
#include "queue.h"
#include "qapi_coap.h"
#include "qapi_diag.h"

/*------------------------------------------------------------------------------
  Typedef Declaration
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  This is the command queue that will be used to enqueue commands from
  external entities
------------------------------------------------------------------------------*/
typedef q_type fota_obj_task_cmd_q;

typedef enum
{
  FOTA_OBJ_MIN_CMD_EVT,
  FOTA_OBJ_APP_ORIGINATED_EVT,
  FOTA_OBJ_CALL_BACK_EVT,
  FOTA_OBJ_MAX_CMD_EVT
}fota_obj_cmd_id_e;

/*------------------------------------------------------------------------------
  Structure Declaration
------------------------------------------------------------------------------*/

typedef struct
{
  qurt_signal_t          fota_obj_signal;
  qurt_mutex_t           fota_obj_mutex;
  q_type*                cmd_q_ptr;
}fota_obj_info_t;


typedef struct{
  q_link_type                      link;
  fota_obj_cmd_id_e                cmd_id;
}fota_obj_cmd_hdr_t;


typedef struct{
  void *data;
}fota_obj_cmd_data_t;


typedef struct{
  fota_obj_cmd_hdr_t               cmd_hdr;
  fota_obj_cmd_data_t              cmd_data;
} fota_obj_cmd_t;

/*------------------------------------------------------------------------------
  Signal Related Macros
------------------------------------------------------------------------------*/
#define FOTA_OBJ_SIGNAL_MASK       0x00000001

#define FOTA_OBJ_TASK_IS_SIG_SET(signal, signal_mask)  ((signal & signal_mask) != 0)

#define FOTA_OBJ_TASK_CLEAR_SIG(signal, signal_mask) \
                                    qurt_signal_clear(&signal,signal_mask)

#define FOTA_OBJ_TASK_SET_SIG(signal, signal_mask) \
                                    qurt_signal_set(&signal, signal_mask)


/*------------------------------------------------------------------------------
  Command Queue Related Macros
------------------------------------------------------------------------------*/

#define FOTA_OBJ_CMD_ID_RANGE_CHECK(cmd_id) \
                                  ( (cmd_id > FOTA_OBJ_MIN_CMD_EVT) && \
                                    (cmd_id < FOTA_OBJ_MAX_CMD_EVT) )

/* Maximum length of log message */
#define MAX_DIAG_LOG_MSG_SIZE       512

/* Log message to Diag */
#define FOTA_LOG_MSG_DIAG( lvl, ... )  \
  { \
    char buf[ MAX_DIAG_LOG_MSG_SIZE ]; \
     \
    /* Format message for logging */  \
    fota_format_log_msg( buf, MAX_DIAG_LOG_MSG_SIZE, __VA_ARGS__ );\
    \
    /* Log message to Diag */  \
    QAPI_MSG_SPRINTF( MSG_SSID_LINUX_DATA, lvl, "%s", buf );  \
  }

#define LOG_INFO(...) FOTA_LOG_MSG_DIAG(MSG_LEGACY_HIGH, __VA_ARGS__)

#define LOG_ERROR(...) FOTA_LOG_MSG_DIAG(MSG_LEGACY_ERROR, __VA_ARGS__)

void fota_format_log_msg
(
  char *buf_ptr,
  int buf_size,
  char *fmt,
  ...
);

typedef struct 
{
  qapi_Net_LWM2M_App_Handler_t handler;
  qapi_Net_LWM2M_Data_v2_t *lwm2m_data;
}fota_data_ext_t;

typedef struct 
{
  qapi_Coap_Session_Hdl_t hdl;
  qapi_Coap_Transaction_t *transaction;
  qapi_Coap_Packet_t *message;
}fota_data_coap_t;

typedef struct
{
  union
  {
    fota_data_ext_t dl_op;
    fota_data_coap_t coap_op;
  }app_data;
} fota_app_data_t;

void lwm2m_firmware_set_state(qapi_Net_Firmware_State_t state);
void lwm2m_firmware_set_result(qapi_Net_LWM2M_Fota_Result_t result);

void fota_obj_process_commands(void);
void fota_obj_cmdq_init(fota_obj_info_t *fota_obj_state);
fota_obj_cmd_t* fota_obj_get_cmd_buffer(void);
void fota_obj_enqueue_cmd(fota_obj_cmd_t* cmd_ptr);
void fota_obj_set_signal(void);

qapi_Status_t Initialize_Fota_App(void);

qapi_Status_t fota_handle_data(qapi_Net_LWM2M_App_Handler_t handler,
                                      qapi_Net_LWM2M_Data_v2_t lwm2m_data);
qapi_Status_t free_lwm2m_copy_data_
(
  qapi_Net_LWM2M_Data_v2_t **lwm2m_data
);
void free_memory(void **var);

qapi_Status_t fota_handle_coap_pkt(qapi_Coap_Session_Hdl_t hdl ,qapi_Coap_Transaction_t *transaction,
                                            qapi_Coap_Packet_t *message);

void free_coap_client_transaction(qapi_Coap_Session_Hdl_t hdl, qapi_Coap_Transaction_t **in_clientTransaction);

qapi_Status_t fota_send_coap_pkt(int blocksize, int pkt_num);
