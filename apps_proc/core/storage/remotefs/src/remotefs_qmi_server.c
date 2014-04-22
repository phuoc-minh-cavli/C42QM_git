/*=============================================================================

                  Remote Storage Server

DESCRIPTION
  This file contains implementation of the QMI based Remote Storage Server.

Copyright (c) 2011-15 by QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/

/*=============================================================================

               EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/storage/remotefs/src/remotefs_qmi_server.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

YYYY-MM-DD      who    what, where, why
-------------   ---   ---------------------------------------------------------
2015-02-04      dks   KW fixes.
2014-12-05      dks   Add device serial number read support.
2014-06-04      ab    Fix racecondition between TX and RX.
2014-01-21      bb    Wait for HISC to come up indefinitely
2013-10-25      ak    Replace rex_sleep(10) by timer_sleep(10, T_MSEC, TRUE);
2013-10-21      sb    Add RDM support
2013-10-03      ak    Added support to recognize modem_fsg2 for SFP2
2013-10-20      bb    Added force SIO flush for RESET_RSP after write iovec
2013-03-11      bb    Removed HELLO handshake during server init
2013-01-17      ak    Replace banned api by safe alternatives
2013-01-15      ak    make changes to send one hello followed by dummy mem
               	      dbg file a default behavior
2012-10-26      vk    Added rcinit_handshake()
2012-04-11      vk    Updates to work on SPINOR builds
2011-10-11      vk    Initial version
==============================================================================*/

#include "remotefs_qmi_server.h"
#include "remote_storage_v01.h"
#include "remotefs_sahara.h"
#include "remotefs_msg.h"
#include "remotefs_comm_api.h"

#include "comdef.h"
#include "err.h"
#include "rex.h"
#include "assert.h"
#include "timer.h"
#include <string.h>
#include "stringl/stringl.h"
#include "rcinit.h"

#define REMOTEFS_SRV_WAIT_SIG        0x00010000
#define REMOTEFS_QMI_MAX_DEBUG_ITEMS 10

typedef struct
{
  uint32 msg_id;
  qmi_error_type_v01 qmi_error;
  remotefs_status_type rmts_error;
}remotefs_qmi_debug_info;

struct remotefs_server_info
{
  rex_crit_sect_type cs;
  service_context_type service_cookie;
  remotefs_client_info_t remotefs_client_table[RMTS_MAX_CLIENT_FILE_HANDLES];
  uint32 num_requests;
  remotefs_qmi_debug_info debug_info[REMOTEFS_QMI_MAX_DEBUG_ITEMS];
  uint32 curr_debug_index;
};
static struct remotefs_server_info remotefs_svc_info;

static qmi_error_type_v01
remotefs_rmts_to_qmi_error (remotefs_status_type rmts_err)
{
  switch (rmts_err)
  {
     case RMFS_NO_ERROR:
       return QMI_ERR_NONE_V01;

     case RMFS_ERROR_PARAM:
       return QMI_ERR_EINVAL_V01;

     case RMFS_ERROR_OP_NOT_SUPPORTED:
       return QMI_ERR_EOPNOTSUPP_V01;

     case RMFS_RX_TIMEOUT:
     case RMFS_FLUSH_TIMEOUT:
       return QMI_ERR_ETIMEDOUT_V01;

     case RMFS_ERROR_NO_MEM:
       return QMI_ERR_NO_MEMORY_V01;

     case RMFS_ERROR_MAX_CLIENTS:
        return QMI_ERR_EUSERS_V01;

     case RMFS_ERROR_PROTOCOL:
       return QMI_ERR_EPROTO_V01;

     case RMFS_ERROR_BAD_STATE:
       return QMI_ERR_ENOTRECOVERABLE_V01;

     default:
      return QMI_ERR_INTERNAL_V01;
  }
}

static void
remotefs_qmi_add_debug_info (uint32 msg_id, remotefs_status_type rmts_status,
                             qmi_error_type_v01 qmi_error)
{
  remotefs_qmi_debug_info *debug_data;

  debug_data = &remotefs_svc_info.debug_info[remotefs_svc_info.curr_debug_index];
  debug_data->msg_id = msg_id;
  debug_data->qmi_error = qmi_error;
  debug_data->rmts_error = rmts_status;

  remotefs_svc_info.curr_debug_index++;
  if (remotefs_svc_info.curr_debug_index == REMOTEFS_QMI_MAX_DEBUG_ITEMS)
  {
    remotefs_svc_info.curr_debug_index = 0;
  }
}

/******************************************************************************
* Description:
*  Create and return a handle for remote storage operations, parses the device
*  and updates internal data structures. Stores any previously opened handle and
*  even when clients on other procs like modem crashes remote fs maintains all
*  information about them because they will come back again after a non-apps
*  crash and any subsystem's restart. When clients on modem come back again
*  and ask for a handle associated with their files, remote fs checks
*  to see if a handle associated with them is already opened and returns that
*  information.
*
* Arguments:
*   path   Path of the storage area to operate on
*
* Returns:
*   a handle if "/mnt/modem_remoteFS" was parsed and client data was populated
*   NULL otherwise
*
******************************************************************************/
static remotefs_status_type
remotefs_srv_open (const char *path, uint32 *handle)
{
  int i = 0;

  if (NULL == handle)
  {
    RMTS_MSG_ERR("Open: NULL handle passed", 0, 0, 0);
    return RMFS_ERROR_PARAM;
  }

  if (NULL == path)
  {
    RMTS_MSG_ERR("Open: NULL path passed", 0, 0, 0);
    return RMFS_ERROR_PARAM;
  }

  if (strlen(path) >
      sizeof (remotefs_svc_info.remotefs_client_table[0].filename))
  {
    RMTS_MSG_ERR("Open: NULL path passed", 0, 0, 0);
    return RMFS_ERROR_NAMETOOLONG;
  }

  for (i = 0; i < RMTS_MAX_CLIENT_FILE_HANDLES; i++)
  {
    if (remotefs_svc_info.remotefs_client_table[i].assigned != TRUE)
    {
      remotefs_svc_info.remotefs_client_table[i].assigned = TRUE;
      remotefs_svc_info.remotefs_client_table[i].last_error = RMFS_NO_ERROR;
      strlcpy (remotefs_svc_info.remotefs_client_table[i].filename, path,
               RMFS_MAX_FILE_PATH);
      break;
    }
    else if (remotefs_svc_info.remotefs_client_table[i].assigned == TRUE)
    {
      if (strcmp (path,
         (remotefs_svc_info.remotefs_client_table[i].filename)) == 0)
      {
        break;
      }
    }
  }

  if (i >= RMTS_MAX_CLIENT_FILE_HANDLES)
  {
    RMTS_MSG_ERR("Open: Failed to assign server file handle", 0, 0, 0);
    return RMFS_ERROR_MAX_CLIENTS;
  }
  else
  {
    RMTS_MSG_HIGH("Open: Assigning server file handle %d", i, 0, 0);
    *handle = i;
  }

  return RMFS_NO_ERROR;
}

/******************************************************************************
* Description:
*  The server intends to keep the opened handles and associated state
*  information about a client if apps proc remains up and running. If a
*  client goes away and comes back, it gets the same previously opened
*  handle. Remote fs on apps must maintain information about clients from
*  other processors when other processors go down and come back while Apps
*  processor remains up.
*
* Arguments:
*   handle     Handle returned by remotefs_srv_open
*
* Returns:
*   RMFS_NO_ERROR or RMFS_ERROR_PARAM
*
******************************************************************************/
static remotefs_status_type
remotefs_srv_close (uint32 handle)
{
  uint32 client_id = handle;

  if (client_id >= RMTS_MAX_CLIENT_FILE_HANDLES)
  {
    RMTS_MSG_ERR("Close: Invalid handle pointer %d > Max_Handles",
                  client_id, RMTS_MAX_CLIENT_FILE_HANDLES, 0);
    return RMFS_ERROR_PARAM;
  }

  remotefs_svc_info.remotefs_client_table[client_id].assigned = FALSE;

  return RMFS_NO_ERROR;
}

static remotefs_status_type
remotefs_read_iovec_helper(char *filename, remotefs_iovec_desc_type *iovec,
                         uint32 iovec_length)
{
  RMTS_MSG_ERR("Read_iovec: Not supported", 0, 0, 0);

  (void) filename;
  (void) iovec;
  (void) iovec_length;
  return RMFS_ERROR_OP_NOT_SUPPORTED;
}

static remotefs_status_type
remotefs_write_iovec_helper(char *filename, remotefs_iovec_desc_type *iovec,
                            uint32 iovec_length)
{
  uint32 i = 0, base_physical_address_offset = 0, transfer_size = 0;
  boolean first_base_address = TRUE;
  remotefs_status_type status = RMFS_NO_ERROR;

  for (i = 0; i < iovec_length; ++i)
  {
     if (TRUE == first_base_address)
     {
       base_physical_address_offset = iovec[i].data_phy_addr;
       first_base_address = FALSE;
     }
     else
     {
       // Take the minimum
       base_physical_address_offset =
         (iovec[i].data_phy_addr < base_physical_address_offset)
         ? iovec[i].data_phy_addr
         : base_physical_address_offset;
     }
     transfer_size = transfer_size + (iovec[i].num_sector * 512);
  }

  status = remotefs_sahara_memory_debug (filename,
                                        (uint8 *) base_physical_address_offset,
                                         transfer_size);
  return status;
}

/******************************************************************************
* Description:
*  This function is called when a rw iovec request message is received by
*  the service. It sends a response indicating the read/write status.
*
* Arguments:
*   req_handle      Handle provided by the infrastructure to specify
*               a particular transaction and message
*   msg_id          Message ID pertaining to this particular message
*   req_c_struct      Decoded request message structure
*
* Returns:
*  QMI_CSI_CB_NO_ERR if success, error code otherwise
*
******************************************************************************/
static qmi_csi_cb_error
remotefs_rw_iovec_response (qmi_req_handle req_handle, int msg_id,
                            void *req_c_struct)
{
  rmtfs_rw_iovec_req_msg_v01 *rw_req;
  rmtfs_rw_iovec_resp_msg_v01 rw_resp;
  qmi_csi_error resp_err;
  uint32 i;
  remotefs_client_info_t *pclient = NULL;
  remotefs_status_type status = RMFS_NO_ERROR;

  RMTS_MSG_MED ("rw_iovec request received", 0, 0, 0);

  rw_req = (rmtfs_rw_iovec_req_msg_v01 *) req_c_struct;

  if (rw_req->caller_id >= RMTS_MAX_CLIENT_FILE_HANDLES ||
      rw_req->iovec_struct == NULL ||
      remotefs_svc_info.remotefs_client_table[rw_req->caller_id].assigned ==
      FALSE)
  {
    RMTS_MSG_ERR ("rw_iovec: Invalid parameters %d %d", rw_req->caller_id,
                   (uint32) rw_req->iovec_struct, 0);
    status = RMFS_ERROR_PARAM;
    goto End;
  }

  pclient = &(remotefs_svc_info.remotefs_client_table[rw_req->caller_id]);
  for (i = 0; i < rw_req->iovec_struct_len; i++)
  {
    pclient->iovec_struct[i].sector_addr
                            = rw_req->iovec_struct[i].sector_addr;
    pclient->iovec_struct[i].data_phy_addr
                            = rw_req->iovec_struct[i].data_phy_addr_offset;
    pclient->iovec_struct[i].num_sector = rw_req->iovec_struct[i].num_sector;
  }
  pclient->ent_cnt = rw_req->iovec_struct_len;

  if (REMOTEFS_DIRECTION_WRITE ==
                            ((remotefs_direction_enum) (rw_req->direction)))
  {
    status = remotefs_write_iovec_helper (pclient->filename,
                                          pclient->iovec_struct,
                                          pclient->ent_cnt);
  }
  else
  {
    status = remotefs_read_iovec_helper (pclient->filename,
                                         pclient->iovec_struct,
                                         pclient->ent_cnt);
  }

End:
  // Set up response
  memset (&rw_resp, 0, sizeof(rmtfs_rw_iovec_resp_msg_v01));

  if (status == RMFS_NO_ERROR)
  {
    rw_resp.resp.result = QMI_RESULT_SUCCESS_V01;
    rw_resp.resp.error = QMI_ERR_NONE_V01;
  }
  else
  {
    if (pclient != NULL)
    {
      pclient->last_error = status;
    }

    rw_resp.resp.result = QMI_RESULT_FAILURE_V01;
    rw_resp.resp.error = remotefs_rmts_to_qmi_error (status);
  }

  remotefs_qmi_add_debug_info (msg_id, status, rw_resp.resp.error);

  // Send response
  resp_err = qmi_csi_send_resp (req_handle, msg_id, &rw_resp, sizeof(rw_resp));
  if (resp_err != QMI_CSI_NO_ERR)
  {
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  return QMI_CSI_CB_NO_ERR;
}

/******************************************************************************
* Description:
*  This function is called when a get_dev_error request message is received by
*  the service. It sends a response containing the last device error.
*
* Arguments:
*   req_handle      Handle provided by the infrastructure to specify
*               a particular transaction and message
*   msg_id          Message ID pertaining to this particular message
*   req_c_struct      Decoded request message structure
*
* Returns:
*  QMI_CSI_CB_NO_ERR if success, error code otherwise
*
******************************************************************************/
static qmi_csi_cb_error
remotefs_get_dev_error_response (qmi_req_handle req_handle, int msg_id,
                              void *req_c_struct)
{
  qmi_csi_error resp_err;
  rmtfs_get_dev_error_req_msg_v01 *dev_err_req;
  rmtfs_get_dev_error_resp_msg_v01 dev_err_resp;
  uint32 client_id;

  RMTS_MSG_MED ("get_dev_error request received", 0, 0, 0);

  // Retrieve client handle from the message
  dev_err_req = (rmtfs_get_dev_error_req_msg_v01 *) req_c_struct;
  client_id = dev_err_req->caller_id;

  // Get the last device error, set up response
  memset (&dev_err_resp, 0, sizeof (rmtfs_get_dev_error_resp_msg_v01));

  if (client_id >= RMTS_MAX_CLIENT_FILE_HANDLES)
  {
    RMTS_MSG_ERR ("get_dev_error: Received bad handle %d", client_id, 0, 0);
    dev_err_resp.status_valid = 0;
    dev_err_resp.resp.result = QMI_RESULT_FAILURE_V01;
    dev_err_resp.resp.error = QMI_ERR_EINVAL_V01;
  }
  else
  {
    dev_err_resp.status_valid = 1;
    dev_err_resp.status = (rmtfs_dev_error_enum_v01)
                  remotefs_svc_info.remotefs_client_table[client_id].last_error;
    dev_err_resp.resp.result = QMI_RESULT_SUCCESS_V01;
    dev_err_resp.resp.error = QMI_ERR_NONE_V01;
  }

  remotefs_qmi_add_debug_info (msg_id, RMFS_ERROR_PARAM,
                               dev_err_resp.resp.error);

  // Send the response
  resp_err = qmi_csi_send_resp (req_handle, msg_id, &dev_err_resp,
                                sizeof(dev_err_resp));
  if (resp_err != QMI_CSI_NO_ERR)
  {
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  return QMI_CSI_CB_NO_ERR;
}

/******************************************************************************
* Description:
*  This function is called when an alloc_buff request message is received by
*  the service. It sends a response containing the physical address of the
*  shared buffer.
*
* Arguments:
*   req_handle      Handle provided by the infrastructure to specify
*               a particular transaction and message
*   msg_id          Message ID pertaining to this particular message
*   req_c_struct      Decoded request message structure
*
*   Returns:
*     QMI_CSI_CB_NO_ERR if success, error code otherwise
*
******************************************************************************/
static qmi_csi_cb_error
remotefs_alloc_buff_response (qmi_req_handle req_handle, int msg_id,
                              void *req_c_struct)
{
  qmi_csi_error resp_err;
  rmtfs_alloc_buff_resp_msg_v01 alloc_resp;
  int32 phy_addr;
  remotefs_status_type status;

  // Get the physical addr of the shared buffer
  phy_addr = 0x0;
  status = RMFS_ERROR_DEVICE;
#ifdef SCL_SHARED_EFS_RAM_BASE
  phy_addr = SCL_SHARED_EFS_RAM_BASE;
#endif

  // Set up response
  memset (&alloc_resp, 0, sizeof(rmtfs_alloc_buff_resp_msg_v01));

  if (phy_addr == 0x0)
  {
    alloc_resp.resp.result = QMI_RESULT_FAILURE_V01;
    alloc_resp.resp.error = QMI_ERR_INTERNAL_V01;
    alloc_resp.buff_address_valid = 0;
  }
  else
  {
    alloc_resp.resp.result = QMI_RESULT_SUCCESS_V01;
    alloc_resp.resp.error = QMI_ERR_NONE_V01;
    alloc_resp.buff_address_valid = 1;
    alloc_resp.buff_address = phy_addr;
    status = RMFS_NO_ERROR;
  }

  remotefs_qmi_add_debug_info (msg_id, status, alloc_resp.resp.error);

  // Send response
  resp_err = qmi_csi_send_resp (req_handle, msg_id,
                      &alloc_resp, sizeof(alloc_resp));
  if (resp_err != QMI_CSI_NO_ERR)
  {
    return QMI_CSI_CB_INTERNAL_ERR;
  }
  return QMI_CSI_CB_NO_ERR;
}

/******************************************************************************
* Description:
*  This function is called when a close request message is received by
*  the service, it sends a response indicating the close status
*
* Arguments:
*   req_handle      Handle provided by the infrastructure to specify
*               a particular transaction and message
*   msg_id          Message ID pertaining to this particular message
*   req_c_struct      Decoded request message structure
*
* Returns:
*  QMI_CSI_CB_NO_ERR if success, error code otherwise
*
******************************************************************************/
static qmi_csi_cb_error
remotefs_close_response (qmi_req_handle req_handle, int msg_id,
                         void *req_c_struct)
{
  qmi_csi_error resp_err;
  rmtfs_close_req_msg_v01 *close_req;
  rmtfs_close_resp_msg_v01 close_resp;
  uint32 rhandle;
  remotefs_status_type status;

  RMTS_MSG_MED ("Close request received", 0, 0, 0);

  // Retrieve client handle from the message
  close_req = (rmtfs_close_req_msg_v01 *) req_c_struct;
  rhandle = (uint32) (close_req->caller_id);

  // Close the specified client handle
  status = remotefs_srv_close (rhandle);

  // Set up response
  memset (&close_resp, 0, sizeof(rmtfs_close_resp_msg_v01));

  if (status == RMFS_NO_ERROR)
  {
    close_resp.resp.result = QMI_RESULT_SUCCESS_V01 ;
    close_resp.resp.error = QMI_ERR_NONE_V01;
  }
  else
  {
    close_resp.resp.result = QMI_RESULT_FAILURE_V01;
    close_resp.resp.error = remotefs_rmts_to_qmi_error (status);
  }

  remotefs_qmi_add_debug_info (msg_id, status, close_resp.resp.error);

  // Send the response
  resp_err = qmi_csi_send_resp (req_handle, msg_id, &close_resp,
                                sizeof(close_resp));
  if (resp_err != QMI_CSI_NO_ERR)
  {
    return QMI_CSI_CB_INTERNAL_ERR;
  }
  return QMI_CSI_CB_NO_ERR;
}

/******************************************************************************
* Description:
*  This function is called when a open request message is received by
*  the service, it sends a response containing the client handle
*
* Arguments:
*   req_handle      Handle provided by the infrastructure to specify
*               a particular transaction and message
*   msg_id          Message ID pertaining to this particular message
*   req_c_struct      Decoded request message structure
*
*   Returns:
*     QMI_CSI_CB_NO_ERR if success, error code otherwise
*
******************************************************************************/
static qmi_csi_cb_error
remotefs_open_response (qmi_req_handle req_handle, int msg_id,
                        void *req_c_struct)
{
  qmi_csi_error resp_err;
  rmtfs_open_req_msg_v01 *open_req;
  rmtfs_open_resp_msg_v01 open_resp;
  uint32 client_handle;
  remotefs_status_type status;

  RMTS_MSG_MED ("Open request received", 0, 0, 0);

  // Retrieve path from the message
  open_req = (rmtfs_open_req_msg_v01 *) req_c_struct;

  // Set up response
  memset (&open_resp, 0, sizeof(rmtfs_open_resp_msg_v01));

  // Call open to get client handle
  status = remotefs_srv_open (open_req->path, &client_handle);
  if (status == RMFS_NO_ERROR)
  {
    open_resp.resp.result = QMI_RESULT_SUCCESS_V01;
    open_resp.resp.error = QMI_ERR_NONE_V01;
    open_resp.caller_id_valid = 1;
    open_resp.caller_id = client_handle;
  }
  else
  {
    open_resp.resp.result = QMI_RESULT_FAILURE_V01;
    open_resp.resp.error = remotefs_rmts_to_qmi_error (status);
    open_resp.caller_id_valid = 0;
  }

  remotefs_qmi_add_debug_info (msg_id, status, open_resp.resp.error);

  // Send the response
  resp_err = qmi_csi_send_resp(req_handle, msg_id, &open_resp,
                               sizeof(open_resp));
  if (resp_err != QMI_CSI_NO_ERR)
  {
    return QMI_CSI_CB_INTERNAL_ERR;
  }

  return QMI_CSI_CB_NO_ERR;
}

/******************************************************************************
* Description:
*  Define a jump table to handle the dispatch of request handler functions.
******************************************************************************/
static qmi_csi_cb_error (* const req_handle_table[]) (qmi_req_handle req_handle,
                         int msg_id, void *req_c_struct) =
{
  NULL,                    // Request handler for message ID 0x00
  &remotefs_open_response,        // Request handler for message ID 0x01
  &remotefs_close_response,         // Request handler for message ID 0x02
  &remotefs_rw_iovec_response,      // Request handler for message ID 0x03
  &remotefs_alloc_buff_response,    // Request handler for message ID 0x04
  &remotefs_get_dev_error_response    // Request handler for message ID 0x05
};

/* This callback function is invoked when the infrastructure receives an */
static qmi_csi_cb_error
remotefs_handle_req_cb (void *connection_handle, qmi_req_handle req_handle,
                     uint32 msg_id, void *req_c_struct, uint32 req_c_struct_len,
                     void *service_cookie)
{
  qmi_csi_cb_error rc = QMI_CSI_CB_INTERNAL_ERR;

  RMTS_MSG_MED ("remotefs_handle_req_cb", 0, 0, 0);

  rex_enter_crit_sect (&remotefs_svc_info.cs);

  remotefs_svc_info.num_requests++;

  /* Verify that the msg_id received is not greater than the largest
     known msg_id */
  if (msg_id < (sizeof(req_handle_table) / sizeof(*req_handle_table)))
  {
    /* If the jump table has a valid entry at this message ID, call the
       handler function */
    if (req_handle_table[msg_id])
    {
      rc = req_handle_table[msg_id] (req_handle, msg_id, req_c_struct);
    }
  }

  rex_leave_crit_sect (&remotefs_svc_info.cs);

  return rc;
}

/* This callback function is called by the QCSI infrastructure when a client
   (user of service) deregisters with the QCSI infrastructure. This will happen
   incase of sub system crash and recovery */
static void
remotefs_disconnect_cb (void *connection_handle, void *service_cookie)
{
  RMTS_MSG_MED ("remotefs_disconnect_cb", 0, 0, 0);

  if (connection_handle)
  {
    free(connection_handle);
  }
}

static qmi_csi_cb_error
remotefs_connect_cb (qmi_client_handle client_handle, void *service_cookie,
                     void **connection_handle)
{
  client_info_type *clnt_info;
  clnt_info = (client_info_type*) malloc(sizeof(client_info_type));

  RMTS_MSG_MED ("remotefs_connect_cb", 0, 0, 0);

  if (clnt_info == NULL)
  {
    return QMI_CSI_CB_CONN_REFUSED;
  }

  clnt_info->clnt = client_handle;
  *connection_handle = clnt_info;
  return QMI_CSI_CB_NO_ERR;
}

static void
remotefs_qmi_register_service (service_context_type *srv,
                               qmi_csi_os_params *os_params)
{
  qmi_idl_service_object_type rmtfs_service_object;
  qmi_csi_error rc = QMI_CSI_INTERNAL_ERR;

  rmtfs_service_object = rmtfs_get_service_object_v01();

  // Register Remote FS server with the QCSI infrastructure
  rc = qmi_csi_register (rmtfs_service_object,
                         remotefs_connect_cb,
                         remotefs_disconnect_cb,
                         (qmi_csi_process_req)remotefs_handle_req_cb,
                         srv, os_params, &srv->service_handle);

  if (rc != QMI_CSI_NO_ERR)
  {
    ERR_FATAL ("Unable to start rmts service!", 0,0,0);
  }

  if (srv->service_handle == NULL)
  {
    ERR_FATAL ("rmtfs service handle NULL after register %d!", rc, 0, 0);
  }
}

/* RCINIT called Init-Function */
void
remotefs_srv_task (void)
{
  qmi_csi_os_params os_params;
  rex_tcb_type *rmts_task;

  memset (&remotefs_svc_info, 0, sizeof(remotefs_svc_info));
  rex_init_crit_sect (&remotefs_svc_info.cs);
  rex_enter_crit_sect (&remotefs_svc_info.cs);

  rcinit_handshake_startup ();

  rmts_task = rex_self ();
  remotefs_sahara_init (rmts_task);

  os_params.tcb = rmts_task;
  os_params.sig = REMOTEFS_SRV_WAIT_SIG;
  remotefs_qmi_register_service (&remotefs_svc_info.service_cookie,
                                 &os_params);

  /* Server is up, and the modem can start sending service requests */
  rex_leave_crit_sect (&remotefs_svc_info.cs);

  while (1)
  {
    rex_wait (REMOTEFS_SRV_WAIT_SIG);
    rex_clr_sigs (rmts_task, REMOTEFS_SRV_WAIT_SIG);
    RMTS_MSG_HIGH ("RMTS QMI Signal received", 0, 0, 0);
    qmi_csi_handle_event (remotefs_svc_info.service_cookie.service_handle,
                          &os_params);
  }
}
