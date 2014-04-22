#ifndef _QMI_TIME_CLIENT_H_
#define _QMI_TIME_CLIENT_H_

/* Private part of the server */
#include "qmi_csi_common.h"
#include "qmi_client.h"
#include "time_service_v01.h"
#include <stdint.h>

#include "time_genoff.h"
#include "time_remote.h"

#if (!defined (FEATURE_NO_DB) && !defined (FEATURE_GSM_PLT))
#include "db.h"
#endif

#define TIME_NO_ERR QMI_NO_ERR
#define TIME_SERVICE_ERR (-1)
#define TIME_CLIENT_ERR  (-2)
#define TIME_SYNC_ERR    (-3)

boolean qmi_time_client_init
(
  qurt_signal_t * signal_ptr,
  uint32  signal_to_be_set,
  uint32  timeout_signal_to_be_set
);


void qmi_time_ind_cb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *ind_buf,
 unsigned int                   ind_buf_len,
 void                           *ind_cb_data
);

/**
Initalizes the connection to the service

@param[in] 
None

@return
Connection Status.

@dependencies
qmi_time_client_init function should be called 
atleast once before this function
*/
int qmi_time_client_connect(void);

#endif

