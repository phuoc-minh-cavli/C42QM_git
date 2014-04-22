/******************************************************************************
 * FILE: qmi_utils_svc.c
 *
 * SERVICES: QMI utility server side module
 *
 * DESCRIPTION: QMI utility server to get utility data on the modem side.
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

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 *  $Header: //components/dev/core.mpss/3.10/snanaval.CORE.MPSS.3.5.mpss_efs/products/qapi_utils/src/qmi_utils_svc.c#1 $ $DateTime: 2022/03/02 06:16:25 $ $Author: snanaval $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 04/01/22     sna      created the file
 *==================================================================*/

#include <string.h>
#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "fs_flash_usage_stats.h"
#include "util_counter_v01.h"
#include "qmi_common.h"
#include "qmi_csi_target.h"
#include "qmi_utils_svc.h"

typedef struct {
  volatile int cleanup;
  qmi_csi_service_handle service_handle;
  int num_requests;
}service_context_type;

static service_context_type utils_service_cookie;   

struct fs_flash_usage_stats_info mon_fs_flash_stats_info_write_counter;  
struct fs_block_info_t mon_fs_block_info;


unsigned int qmi_util_service_instance = 0;

/*=============================================================================
  FUNCTION flash_counter_response
=============================================================================*/
/*!
@brief
  This function is called when flash EFS counter request message is received by
  the service, it will read the EFS counter values and  response to client.

@param[in]  connection_handle    Cookie specified by the service to identify
                                 each client (output parameter of connect_cb).

@param[in]   req_c_struct        Decoded request message structure

@param[in]   req_c_struct_len    Decoded request message structure length
                                 
@param[in]   req_handle          Opaque handle provided by the infrastructure
                                 to specify a particular transaction and
                                 message.
@param[in]   msg_id              msg_id of the request 

@param[in]  service_cookie       Service specific data.Service cookie is
                                 registered with the infrastructure during
                                 service registration(qmi_csi_register).

@retval    QMI_CSI_CB_NO_ERR     Success 
@retval    QMI_CSI_CB_...        Error, see error codes defined in qmi_csi.h 

*/
/*=========================================================================*/
static qmi_csi_cb_error flash_counter_response
(
 void                     *conn_handle,
 qmi_req_handle           req_handle,
 unsigned int             msg_id,
 void                     *req_c_struct,
 unsigned int             req_c_struct_len,
 void                     *service_cookie
) 
{
	int rc;
	qmi_csi_error resp_err;
	qmi_util_flash_counter_resp_msg_v01 flash_util_resp;

	memset( &flash_util_resp, 0, sizeof(flash_util_resp) );

	/* To get the write count value from the MPSS */
	rc = fs_get_flash_usage_stats_info(&mon_fs_flash_stats_info_write_counter);
	if ( rc != 0)
	{
		UTILS_MSG_ERROR_1("fs_get_flash_usage_stats_info got failed with rc %x",rc);
	}
	else
	{
		flash_util_resp.fs_counter_valid = 1;  
		flash_util_resp.fs_counter = mon_fs_flash_stats_info_write_counter.write_region_stats[FS_FOP_LOG];
	}
	/* To get the block information */
	rc = fs_get_block_info(&mon_fs_block_info);
	if( rc != 0 )
	{
		UTILS_MSG_ERROR_1("fs_get_block_info got failed with rc %x",rc);
	}
	else
	{
		flash_util_resp.fs_block_count_valid=1;
		flash_util_resp.fs_block_count = mon_fs_block_info.log_block_count;
	}
	/* To get the page count */
	flash_util_resp.fs_page_count_valid=1;
	flash_util_resp.fs_page_count = mon_fs_block_info.pages_per_block;
	resp_err = qmi_csi_send_resp( req_handle, msg_id, &flash_util_resp, sizeof(flash_util_resp) );      
	if(resp_err != QMI_CSI_NO_ERR)
	{
		UTILS_MSG_ERROR_1("qmi_csi_send_resp got failed rc %x",resp_err);
		rc=resp_err;
	}
	else
	{
		rc = QMI_CSI_CB_NO_ERR;
	}
	return rc;
}
static qmi_csi_req_handler_tbl_type req_handle_table[] = 
{
	{QMI_UTIL_FlASH_COUNTER_REQ_V01, 1, flash_counter_response},
};

	static qmi_csi_cb_error util_connect_cb
(
 qmi_client_handle         client_handle,
 void                      *service_cookie,
 void                      **connection_handle
 )
{
	return QMI_CSI_CB_NO_ERR;     
}

	static void util_disconnect_cb
(
 void                      *connection_handle,
 void                      *service_cookie
 )
{
	return;
}

/*=============================================================================
  FUNCTION qmi_util_register_service 
Description: qmi utility server registration with csi framework
=============================================================================*/

void *qmi_util_register_service(qmi_csi_os_params *os_params)
{
	qmi_idl_service_object_type util_service_object = util_get_service_object_v01();
	qmi_csi_error rc = QMI_CSI_INTERNAL_ERR;
	qmi_csi_options options;

	if(!util_service_object || !os_params)
	{
		return NULL;
	}
	QMI_CSI_OPTIONS_INIT(options);
	QMI_CSI_OPTIONS_SET_REQ_HANDLER_TBL(options, req_handle_table,
			(uint16_t)(sizeof(req_handle_table)/sizeof(req_handle_table[0])));

	rc = qmi_csi_register_with_options(util_service_object, util_connect_cb, 
			util_disconnect_cb, NULL, &utils_service_cookie, os_params, 
			&options, &utils_service_cookie.service_handle); 
	if(rc != QMI_CSI_NO_ERR)
		return NULL;
	return utils_service_cookie.service_handle;
}

