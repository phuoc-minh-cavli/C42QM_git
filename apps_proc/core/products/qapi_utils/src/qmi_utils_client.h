#ifndef QMI_UTILS_CLIENT_H
#define QMI_UTILS_CLIENT_H

/*============================================================================
  @file qmi_utils_client.h

  Function and data structure declarations for qmi_utils client
  framework.


  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

==============================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2022-03-16   sna   created file for qapi support.

============================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include <stdlib.h>
#include "comdef.h"
#include "qurt.h"
#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "qmi_cci_target.h"
#include "qmi_cci_common.h"
#include "qapi_utils.h"
#include "qapi_status.h"

#define ___QAPI_ERROR(x)  ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_BSP_FS,  x)))
#define QAPI_ERR_INVLD_ID         ___QAPI_ERROR(19)

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
struct mpss_efs_counter{

int fs_counter;
int fs_block_count;
int  fs_page_count;

};

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

/**
 * @brief get the required counter values from the MPSS EFS through qmi framework
 *
 * @details This function sends the request to MPSS and get the counter values
 * 	    This function need to be called after qmi_util_client_start
 *
 * @param[in]  Pass the EFS counter structure to get the below data
 *		1.No of writes
 *		2.Total block count
 *
 * @return Returns QAPI_OK on success and -ve error code is returned on failure.
 * 
 * 
 */

int utils_connect_server(struct mpss_efs_counter *efs_counter);

/**
 * @brief Initializes the qmi framework to communicate from threadx to modem.
 *
 * @details This function will initilizes the qmi framwork 
 *          this will help in get the required counters
 *
 *
 * @return Returns QAPI_OK on success and -ve error code is returned on failure.
 *
 *
 */

int qmi_util_client_start(void);

#endif 
