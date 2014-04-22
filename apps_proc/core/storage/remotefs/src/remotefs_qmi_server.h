#ifndef RMT_STORAGE_SRV_H
#define RMT_STORAGE_SRV_H

/*=======================================================================================

                           Remote Storage Server

DESCRIPTION
   This file contains definitions for QMI based Remote Storage Server.

Copyright (c) 2011-14 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=========================================================================================*/
/*=======================================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/storage/remotefs/src/remotefs_qmi_server.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

YYYY-MM-DD       who     what, where, why
--------------   ---     ----------------------------------------------------------------
2014-12-05       dks     Add device serial number read support.
2011-10-11       vin     Initial version
=========================================================================================*/

#include "qmi_csi.h"
#include "qmi_idl_lib.h"
#include "comdef.h"
#include "customer.h"
#include "remotefs_comm_api.h"

#ifndef RMTS_MAX_EXTERNAL_CLIENTS
#define RMTS_MAX_EXTERNAL_CLIENTS   9
#endif

/* Define limits */
// This number is based on the total number of files opened by clients, so
#define RMTS_MAX_INTERNAL_FILE_HANDLES       4
#define RMTS_MAX_CLIENT_FILE_HANDLES    \
                    (RMTS_MAX_EXTERNAL_CLIENTS + RMTS_MAX_INTERNAL_FILE_HANDLES)
#define RMFS_MAX_IOVEC_XFR_CNT   	 4
#define RMFS_MAX_FILE_PATH         255

/* This structure holds information about the client that is connecting
   to the server. It can hold extra information as needed. */
typedef struct {
    qmi_client_handle clnt;
} client_info_type;


/* This structure is used to hold context of the server. An instance of
   this structure is passed to the qmi_csi_register function
   as the service_cookie, and this structure can be tailored to
   a specific service's needs */
typedef struct {
    qmi_csi_service_handle service_handle;  // Passed into qmi_csi_handle_event
} service_context_type;

/* Remote file server's direction of data transfer */
typedef enum
{
    REMOTEFS_DIRECTION_READ = 0,
    REMOTEFS_DIRECTION_WRITE = 1
} remotefs_direction_enum;

/* Client specific information */
typedef struct
{
   boolean assigned; 	                   // indicates successful open operation
   remotefs_iovec_desc_type
   iovec_struct[RMFS_MAX_IOVEC_XFR_CNT]; // actual iovec
   uint32  ent_cnt;  			               // number of iovec elements
   remotefs_direction_enum direction;   // direction of transfer for the iovec
   char   filename[RMFS_MAX_FILE_PATH]; // Path of the client
   remotefs_status_type last_error; 	  // last device error
} remotefs_client_info_t;

#endif /* RMT_STORAGE_SRV_H */
