/******************************************************************************
 *
 * FILE: qmi_utils_client.c
 *
 * SERVICES: QMI utility client side module
 *
 * DESCRIPTION: QMI utility client to get the required data from modem
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2022 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *
 ==================================================================*/

/*===========================================================================
 
                         EDIT HISTORY FOR MODULE
 
 This section contains comments describing changes made to the module. 
 Notice that changes are listed in reverse chronological order.
 
 
when         who   what, where, why
----------   ---   ---------------------------------------------------------
2022-03-14   sna    Create the file

===========================================================================*/

#include <stdlib.h>
#include "comdef.h"
#include "msg.h"
#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "qmi_cci_target.h"
#include "qmi_cci_common.h"
#include "util_counter_v01.h"
#include "qapi_utils.h"
#include "qapi_diag.h"
#include "qmi_utils_client.h"
#include "qurt.h"

#define FLASH_ERR_RESP -1

#define QMI_CLNT_WAIT_SIG  0x00010000
#define QMI_CLNT_TIMER_SIG 0x00000001
#define QAPI_QMI_EFS_MSG_TIMEOUT_MSEC  5000

qmi_util_flash_counter_req_msg_v01 req;
qmi_util_flash_counter_resp_msg_v01 resp;
struct mpss_efs_counter mpss_efs_data;

qmi_idl_service_object_type util_service_object;
qmi_client_type qmi_utils_handle;
qmi_cci_os_signal_type os_params;

int utils_connect_server(struct mpss_efs_counter *efs_counter)
{
  int rc=0;
  qmi_service_info info[10];

  memset(&req, 0, sizeof(req));
  rc = qmi_client_send_msg_sync(qmi_utils_handle, QMI_UTIL_FlASH_COUNTER_REQ_V01, &req, sizeof(req),&resp, sizeof(resp), QAPI_QMI_EFS_MSG_TIMEOUT_MSEC);
  if (rc != 0)
  {
      QAPI_MSG(MSG_SSID_DIAG,MSG_LEGACY_HIGH,
				"qmi_client_send_msg_sync returned %x",rc);
      return rc;
  }

  if( (resp.fs_block_count == 0)||(resp.fs_page_count == 0) )
  {
      QAPI_MSG(MSG_SSID_DIAG,MSG_LEGACY_HIGH,
				"block count and page counts are zero not a valid response");
      return FLASH_ERR_RESP; 

  }
  efs_counter->fs_counter      = resp.fs_counter;
  efs_counter->fs_block_count  = resp.fs_block_count;
  efs_counter->fs_page_count   = resp.fs_page_count;

  QAPI_MSG(MSG_SSID_DIAG,MSG_LEGACY_HIGH,
			"utils_connect_server ret=%d wr_cnt %d block_count %d pagecnt%d",
			rc,resp.fs_counter,resp.fs_block_count,resp.fs_page_count);

  return 0;

}

int qmi_util_client_start(void)
{
  qmi_txn_handle txn;
  qmi_client_type notifier;
  unsigned int num_services, num_entries=10, i=0, num_services_old=0;
  int rc;
  qmi_service_info info[10];
  util_service_object = util_get_service_object_v01();

  os_params.ext_signal = NULL;
  os_params.sig = QMI_CLNT_WAIT_SIG;
  os_params.timer_sig = QMI_CLNT_TIMER_SIG;

  rc=qmi_client_init_instance(util_service_object,0,NULL,NULL,&os_params,0,&qmi_utils_handle);
  if(rc !=  QMI_NO_ERR )
  {
        QAPI_MSG(MSG_SSID_DIAG,MSG_LEGACY_HIGH,
				"qmi_client_init_instance failed ret %x",rc);
        return FALSE;
  }
  return rc;

}
