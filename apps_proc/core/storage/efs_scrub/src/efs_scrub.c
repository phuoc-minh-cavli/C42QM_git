/****************************************************************************
 * efs_scrub.c
 *
 * The scrub code performs scrubbing of EFS2:APPS Partition when
 * signalled by scrub task on MPSS module .
 *
 * Copyright (C) 2016, 2019 QUALCOMM Technologies, Inc.
 *
 *  For this partition ,it keeps track of:
 * - Has the scrubbing operation started.
 * - At what stage of scrubbing the system is in, so it can resume on power
 *   cycle.
 * - Was the last scrubbing operation completed .
 * - Is APPS(client) - MPSS(server) broken or connected.
 ***************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



when         who   what, where, why
----------   ---   ---------------------------------------------------------
2019-08-22   as    Add support to featurise efs_scrub
2016-11-17   as    Create.

===========================================================================*/

#include "efs_scrub_config.h"
#ifdef FEATURE_ENABLE_EFS_SCRUB
#include "efs_scrub.h"
#include "fs_lib.h"
#include "fs_sys_types.h"
#include "fs_osal.h"
#include "fs_err.h"  /* for FS_MSG_HIGH_1 and for "PACKED" structures */
#include "fs_task_i.h" /* for FS_EFS_SCRUB_SIG */
#include "flash_driver_service_v01.h"
#include "fs_priv_funcs.h" /* for efs_full_scrub() */
#include "fs_public.h"
#include "fs_errno.h"
#include "qmi_client.h"

#define EFS_SCRUB_FDS_SERVER_INSTANCE      0
#define EFS_SCRUB_FDS_WAIT_FOREVER         0
#define EFS_SCRUB_INFO_FILE_OFLAG          (O_CREAT|O_RDWR|O_AUTODIR)
#define EFS_SCRUB_INFO_FILE_MODE           (0644)

#ifndef EFS_SCRUB_MAGIC_NO_1
  #define EFS_SCRUB_MAGIC_NO_1  0x73736665
#endif

#ifndef EFS_SCRUB_MAGIC_NO_2
  #define EFS_SCRUB_MAGIC_NO_2  0x62757263
#endif

#ifndef EFS_SCRUB_VERSION_NO
  #define EFS_SCRUB_VERSION_NO  1
#endif

#ifndef EFS_SCRUB_CONNECT_TO_SER_RETRY_COUNT
  #define EFS_SCRUB_CONNECT_TO_SER_RETRY_COUNT 3
#endif

#define EFS_SCRUB_NUM_OF_LAST_ERRORS 10

/*---------------------------------------------------------------------------
 * Scrub-state-Information file:
 * This file is updated everytime Scrub-state changes.
---------------------------------------------------------------------------*/
enum efs_scrub_state_type
{
  EFS_SCRUB_STATE_COMPLETE = 0,
  EFS_SCRUB_STATE_PENDING  = 1,
};

PACKED struct efs_scrub_state_info_type
{
  uint32 magic_no_1;
  uint32 magic_no_2;
  uint32 version_no;
  uint32 scrub_seq_no;
  enum efs_scrub_state_type scrub_status;
}PACKED_POST;


/*---------------------------------------------------------------------------
 * efs scrub debug information file:
 * This file is updated everytime any qmi API returns error.
 * Rule followed for enum names :
   <EFS_SCRUB_ERR>_<MODULE_NAME>_<MODULE_SPECIFIC_INFO>
   for e.g.
   EFS_SCRUB_ERR_QMI_SEND_MSG_SYNC
     prefix      : EFS_SCRUB_ERR_
     MODULE_NAME : QMI
     MODULE SPECIFIC INFO :  SEND_MSG_SYNC which indicates that
                                qmi_client_send_msg_sync failed

---------------------------------------------------------------------------*/
enum api_failed_type
{
  EFS_SCRUB_ERR_QMI_INVLD_MESSAGE_ID=1,
  EFS_SCRUB_ERR_QMI_MESSAGE_DECODE,
  EFS_SCRUB_ERR_QMI_CLIENT_INIT_INSTANCE,
  EFS_SCRUB_ERR_QMI_REG_ERROR_CB,
  EFS_SCRUB_ERR_QMI_SEND_MSG_SYNC,
};

struct efs_scrub_debug_file_type
{
  uint32 scrub_seq_no;
  uint32 line_no;
  int err_code;
};

struct efs_scrub_debug_info_type
{
  uint8 debug_file_index;
  struct efs_scrub_debug_file_type debug_file[EFS_SCRUB_NUM_OF_LAST_ERRORS];
};
 

/*---------------------------------------------------------------------------
 efs-scrub information file:
 efs_scrub_debug_info_type debug_info[EFS_SCRUB_NUM_OF_LAST_ERRORS] 
 is a cyclic array .i.e it fills entries from 0 to 9 
 then 10th entry at 0,11th at 1 so on...
 
 If debug_info_index is 3,then among all non-zero entries of
 efs_scrub_debug_info_type debug_info[EFS_SCRUB_NUM_OF_LAST_ERRORS] are latest
 ones with entry at index 2 is the latest 
 next non-zero entry from 3 onwards(including 3) is oldest .
---------------------------------------------------------------------------*/
struct efs_scrub_info_type
{
  qmi_client_type fds_server_hdl;
  uint8 is_client_server_connected; 
  struct efs_scrub_state_info_type state_info;
  struct efs_scrub_debug_info_type debug_info;
};

static struct efs_scrub_info_type efs_scrub_info;

static qurt_mutex_t efs_scrub_lock;
uint8 retry_count = 0;
uint8 debug_index = 0;

/*---------------------------------------------------------------------------
 * Read-Write functionality for state file and debug file.
---------------------------------------------------------------------------*/
static int efs_scrub_read_status_file (void)
{
  int result;
  result = efs_get (EFS_SCRUB_STATE_FILE_PATH,
                    &efs_scrub_info.state_info,
                    sizeof(efs_scrub_info.state_info));
  return result;
}

static void efs_scrub_write_status_file (void)
{
  int result;
  result = efs_put (EFS_SCRUB_STATE_FILE_PATH,
                    &efs_scrub_info.state_info,
                    sizeof(efs_scrub_info.state_info),
                    EFS_SCRUB_INFO_FILE_OFLAG,
                    EFS_SCRUB_INFO_FILE_MODE);
  if (result < 0)
  {
    FS_ERR_FATAL("%d, efs_put failed to put scrub state file path.",
                  efs_errno, 0, 0);
  }
}

static void efs_scrub_read_debug_file (void)
{
  (void) efs_get (EFS_SCRUB_DEBUG_FILE_PATH,
                  &efs_scrub_info.debug_info,
                  sizeof(efs_scrub_info.debug_info));
}

static void efs_scrub_write_debug_file (void)
{
  int result;
  result = efs_put(EFS_SCRUB_DEBUG_FILE_PATH,
                   &efs_scrub_info.debug_info,
                   sizeof(efs_scrub_info.debug_info),
                   EFS_SCRUB_INFO_FILE_OFLAG,
                   EFS_SCRUB_INFO_FILE_MODE);
  if (result < 0)
  {
    FS_ERR_FATAL("%d, error writing debug file.", efs_errno, 0, 0);
  }
}

static void efs_scrub_update_debug_file (int line,int result)
{
  efs_scrub_info.debug_info.debug_file[debug_index].line_no = line;
  efs_scrub_info.debug_info.debug_file[debug_index].err_code = result;
  efs_scrub_info.debug_info.debug_file_index = debug_index + 1;
  debug_index++;
  if(efs_scrub_info.debug_info.debug_file_index == EFS_SCRUB_NUM_OF_LAST_ERRORS)
  {
    efs_scrub_info.debug_info.debug_file_index = 0;
    debug_index =0;
  }
  efs_scrub_write_debug_file ();
}


/* this is the callback that QMI will call if MPSS reboots silently 
   or any error on the MPSS side */ 

static void efs_scrub_fds_error_cb
(
  qmi_client_type user_handle,
  qmi_client_error_type error,
  void *err_cb_data
)
{
  qurt_mutex_lock (&efs_scrub_lock);

  efs_scrub_info.is_client_server_connected = FALSE;
  fs_os_set_sigs (fs_task_tcb, fs_task_signal, FS_EFS_SCRUB_SIG);

  qurt_mutex_unlock (&efs_scrub_lock);
}


/* The callback function thatgets invoked when the FDS-QMI-server 
   in the MPSS sends Scrub-notification */ 

static void efs_scrub_fds_ind_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                           *ind_buf,
  unsigned int                   ind_buf_len,
  void                           *ind_cb_data
)
{
  fds_scrub_apps_start_scrub_ind_msg_v01 scrub_ind_msg;
  int result ;

  qurt_mutex_lock(&efs_scrub_lock);

  memset(&scrub_ind_msg, 0, sizeof(scrub_ind_msg));

  if (msg_id != QMI_FDS_SCRUB_APPS_START_SCRUB_IND_V01)
  {
    FS_MSG_HIGH_1 ("[%d] Unsupported message indication.",
                     scrub_ind_msg.scrub_token);
    efs_scrub_update_debug_file(__LINE__,msg_id);
    goto End_cb;
  }

  result = qmi_client_message_decode (user_handle,
                                      QMI_IDL_INDICATION,
                                      msg_id,ind_buf,
                                      ind_buf_len,
                                      &scrub_ind_msg,
                                      sizeof(scrub_ind_msg));


  if (result != QMI_NO_ERR)
  {
    FS_MSG_HIGH_1 ("Indication decode failed in \
                   qmi_client_message_decode API with error [%d]", result);
    efs_scrub_update_debug_file(__LINE__,result);
    goto End_cb;
  }

  if (efs_scrub_info.state_info.scrub_seq_no == scrub_ind_msg.scrub_token)
  {
    FS_MSG_HIGH_1 ("[%d] duplicate callback for the same scrub token",
                     scrub_ind_msg.scrub_token);
    goto End_cb;
  }

  efs_scrub_info.state_info.scrub_status = EFS_SCRUB_STATE_PENDING;
  efs_scrub_info.state_info.scrub_seq_no = scrub_ind_msg.scrub_token;

  efs_scrub_write_status_file ();
  fs_os_set_sigs (fs_task_tcb, fs_task_signal, FS_EFS_SCRUB_SIG);

End_cb:
  qurt_mutex_unlock(&efs_scrub_lock);
}

static int efs_scrub_connect_to_server (void)
{
  qmi_idl_service_object_type service_obj;
  qmi_client_os_params os_params;
  qmi_client_error_type result;
  fds_indication_register_req_msg_v01 indication_register_req;
  fds_indication_register_resp_msg_v01 indication_register_resp;
  uint32 timeout_ms = EFS_SCRUB_FDS_WAIT_FOREVER;
  int status = 0;
  
  memset (&indication_register_req, 0, sizeof (indication_register_req));
  memset (&indication_register_resp, 0, sizeof (indication_register_resp));

  indication_register_resp.resp.result = QMI_RESULT_FAILURE_V01;
  indication_register_req.need_apps_start_scrub_indication_valid = 1;
  indication_register_req.need_apps_start_scrub_indication = 1;

  /* get a handle from the QMI module to call QMI APIS
     to check about the FDS-QMI-server status */

  service_obj = fds_get_service_object_v01 ();
  if (service_obj == NULL)
  {
    FS_ERR_FATAL ("Unable to get the handle for the FDI QMI service", 0, 0, 0);
  }

  memset (&os_params, 0, sizeof(os_params));

  result = qmi_client_init_instance (service_obj,
                                     EFS_SCRUB_FDS_SERVER_INSTANCE,
                                     efs_scrub_fds_ind_cb,
                                     NULL,
                                     &os_params,
                                     timeout_ms,
                                     &(efs_scrub_info.fds_server_hdl));
  if (result != QMI_NO_ERR)
  {
    FS_MSG_HIGH_1 ("Unable to connect to FDS-server in MPSS side.Failure in\
                    API qmi_client_init_instance with error [%d]", result);
    efs_scrub_update_debug_file(__LINE__,result);
    status = (-EFS_SCRUB_ERR_QMI_CLIENT_INIT_INSTANCE);
    goto End_con;
  }

  result = qmi_client_register_error_cb (efs_scrub_info.fds_server_hdl,
                                         efs_scrub_fds_error_cb,
                                         NULL);
  if (result != QMI_NO_ERR)
  {
    FS_MSG_HIGH_1 ("Unable to register error callback function\
                    with FDS-server on MPSS side.Failure in API\
                    qmi_client_register_error_cb with error [%d]" , result);
    efs_scrub_update_debug_file(__LINE__,result);
    status = (-EFS_SCRUB_ERR_QMI_REG_ERROR_CB);
    goto End_con;
  }

  result = qmi_client_send_msg_sync(efs_scrub_info.fds_server_hdl,
                                    QMI_FDS_INDICATION_REGISTER_REQ_V01,
                                    &indication_register_req,
                                    sizeof(indication_register_req),
                                    &indication_register_resp,
                                    sizeof(indication_register_resp),
                                    timeout_ms);
  if (result != QMI_NO_ERR)
  {
    FS_MSG_HIGH_1 ("Unable to send indication register message \
                    to FDS-server in MPSS side.Failure in API\
                    qmi_client_send_msg_sync with error [%d]" , result);
    efs_scrub_update_debug_file(__LINE__,result);
    status = (-EFS_SCRUB_ERR_QMI_SEND_MSG_SYNC);
    goto End_con;
  }

  efs_scrub_info.is_client_server_connected = TRUE;

End_con:
  if(status != 0)
  {
    /* This should be called whenever client-server connection fails.
    It will free memory occupied by client's information and will avoid any memory leak*/
    result = qmi_client_release(efs_scrub_info.fds_server_hdl);
    if (result != QMI_NO_ERR)
    {
      FS_ERR_FATAL ("Unable to release client instance", 0, 0, 0);
    }
  };
  return status;
}


void efs_scrub_signal_received (void)
{
  int result;
  qurt_mutex_lock (&efs_scrub_lock);

  while (FALSE == efs_scrub_info.is_client_server_connected)
  {
    result = efs_scrub_connect_to_server ();
    if(result != 0)
    {
      retry_count += 1;
      if(retry_count < EFS_SCRUB_CONNECT_TO_SER_RETRY_COUNT)
      {
        continue;
      }
      FS_ERR_FATAL ("efs_scrub_connect_to_server failed.\
                     Check debug file", 0, 0, 0);
    }
  }

  if (efs_scrub_info.state_info.scrub_status == EFS_SCRUB_STATE_PENDING)
  {
    qurt_mutex_unlock (&efs_scrub_lock);
    result = efs_full_scrub ("/");
    if(result != 0)
    {
      FS_ERR_FATAL ("efs_full_scrub failed.", 0, 0, 0);
    }
    
    qurt_mutex_lock (&efs_scrub_lock);
    efs_scrub_info.state_info.scrub_status = EFS_SCRUB_STATE_COMPLETE;
    efs_scrub_write_status_file ();
  }

  qurt_mutex_unlock (&efs_scrub_lock);

}


void efs_scrub_init (void)
{
  int result;

  qurt_mutex_init (&efs_scrub_lock);
  qurt_mutex_lock (&efs_scrub_lock);

  memset (&efs_scrub_info, 0, sizeof(efs_scrub_info));

  efs_scrub_read_debug_file ();
  debug_index = efs_scrub_info.debug_info.debug_file_index;
  
  result = efs_scrub_read_status_file ();
  if (result == -1)
  {
    if(efs_errno == ENOENT)
    {
      efs_scrub_info.state_info.magic_no_1 = EFS_SCRUB_MAGIC_NO_1;
      efs_scrub_info.state_info.magic_no_2 = EFS_SCRUB_MAGIC_NO_2;
      efs_scrub_info.state_info.version_no = EFS_SCRUB_VERSION_NO;
      efs_scrub_info.state_info.scrub_status = EFS_SCRUB_STATE_COMPLETE;
      efs_scrub_info.state_info.scrub_seq_no = 0;
      efs_scrub_write_status_file ();
    }
    else
    {
      FS_ERR_FATAL ("%d, problem with the state file.", efs_errno, 0, 0);
    }
  }

  efs_scrub_info.is_client_server_connected = FALSE;
  fs_os_set_sigs (fs_task_tcb, fs_task_signal, FS_EFS_SCRUB_SIG);

  qurt_mutex_unlock (&efs_scrub_lock);

  return;
}
#endif
