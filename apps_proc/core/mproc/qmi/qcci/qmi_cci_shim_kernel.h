#ifndef QMI_CCI_SHIM_KERNEL_H
#define QMI_CCI_SHIM_KERNEL_H

#include "qmi_client.h"
#include "qmi_client_shim.h"
#include "qmi_common.h"
#include "qmi_cci_common.h"
#include "qapi_txm_base.h"
#include "qurt_txm_qapi_fwk.h"

#define QMI_CLIENT_SHIM_MAGIC 0xc3c3c3c3
#define QMI_SHIM_MODULE_COOKIE 0xd2d2d2d2
#define QMI_MSG_Q_MAX_NUM_CB_INFOS   16

typedef struct qmi_client_user_module_h qmi_client_user_module_handle;

typedef struct qmi_client_shim_handle_s qmi_client_shim_handle_type;

typedef struct qmi_client_shim_buf_s qmi_client_shim_buf_type;

typedef struct qmi_client_shim_buf_s
{
  LINK(qmi_client_shim_buf_type, link);
  void    *buf;
  uint32  len;
} qmi_client_shim_buf_type;

typedef struct qmi_client_shim_handle_s
{
  uint32                         magic;
  LINK(qmi_client_shim_handle_type, link);
  qmi_idl_service_object_type    service_obj;
  qmi_client_type                qmi_handle;
  qmi_client_tx_os_params*       tx_os_params;
  qmi_client_os_params          *os_params;
  qmi_client_user_module_handle *module_handle;
  qmi_cci_lock_type              mutex;
  qmi_client_notify_event_type   last_service_event;
  uint32                         notify_received;
  qmi_cci_client_category_type   category;
  LIST(qmi_client_shim_buf_type, buf_q);
  qmi_client_notify_cb           notify_cb;
  void                          *notify_cb_data;
  qmi_client_log_cb              log_cb;
  void                          *log_cb_data;
  qmi_client_ind_cb              ind_cb;
  void                          *ind_cb_data;
  qmi_client_error_cb            err_cb;
  void                          *err_cb_data;
  qmi_client_release_cb          release_cb;
  void                          *release_cb_data;
  qmi_client_recv_raw_msg_async_cb   raw_msg_async_cb;
  void                          *raw_msg_async_cb_data;
  qmi_client_recv_msg_async_cb   struct_msg_async_cb;
  void                          *struct_msg_async_cb_data;
}qmi_client_shim_handle_type;

typedef struct qmi_client_user_module_h
{
  uint32 magic;
  LINK(qmi_client_user_module_handle, link);
  TX_QUEUE msg_q;
  void *msg_q_buf;
  LIST(qmi_client_shim_handle_type, clnt_list);
  qmi_cci_lock_type clnt_list_lock;
}qmi_client_user_module_handle;


UINT qmi_client_shim_kernel_handler(UINT id, UINT a1, UINT a2, UINT a3, UINT a4, UINT a5, UINT a6, UINT a7, UINT a8, UINT a9, UINT a10, UINT a11, UINT a12);

qmi_client_error_type _qmi_client_notifier_init(
                            qmi_client_user_module_handle             *handle,
                            qmi_idl_service_object_type               service_obj,
                            qmi_client_tx_os_params                   *os_params,
                            qmi_client_type                           *user_handle);
                            
qmi_client_error_type _qmi_client_init(
                            qmi_client_user_module_handle             *handle,
                            qmi_service_info                          *service_info,
                            qmi_idl_service_object_type               service_obj,
                            qmi_client_ind_cb                         ind_cb,
                            void                                      *ind_cb_data,
                            qmi_client_tx_os_params                   *os_params,
                            qmi_client_type                           *user_handle);

qmi_client_error_type _qmi_client_register_notify_cb
(
                            qmi_client_user_module_handle             *handle,
 qmi_client_type user_handle,
 qmi_client_notify_cb notify_cb,
 void *notify_cb_data
);

qmi_client_error_type _qmi_client_get_service_list
(
                            qmi_client_user_module_handle             *handle,
 qmi_idl_service_object_type service_obj,
 qmi_service_info *service_info_array,
 unsigned int *num_entries,
 unsigned int *num_services
);

qmi_client_error_type
_qmi_client_get_service_instance
(
                            qmi_client_user_module_handle             *handle,
 qmi_idl_service_object_type service_obj,
 qmi_service_instance instance_id,
 qmi_service_info *service_info
);

qmi_client_error_type _qmi_client_release
(
 qmi_client_user_module_handle    *handle,
 qmi_client_type     user_handle
);

void qmi_client_shim_init(void);

#endif /* QMI_CCI_SHIM_KERNEL_H */
