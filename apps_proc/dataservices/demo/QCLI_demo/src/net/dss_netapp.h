/*!
  @file
  dss_netapp.h

  @brief
  DSS netctrl test application functionality to interact with underlying 
  data control plane. 
*/

 
/*===========================================================================
 
   Copyright (c) 2017 Qualcomm Technologies, Inc. All Rights Reserved
 
   Qualcomm Technologies Proprietary
 
   Export of this technology or software is regulated by the U.S. Government.
   Diversion contrary to U.S. law prohibited.
 
   All ideas, data and information contained in or disclosed by
   this document are confidential and proprietary information of
   Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
   By accepting this material the recipient agrees that this material
   and the information contained therein are held in confidence and in
   trust and will not be used, copied, reproduced in whole or in part,
   nor its contents revealed in any manner to others without the express
   written permission of Qualcomm Technologies, Inc.
 
===========================================================================*/

#include "qurt_signal.h"
#include "qurt_mutex.h"

typedef qurt_signal_t            qcli_ds_platform_pthread_cond_t;
typedef qurt_mutex_t             qcli_ds_platform_pthread_mutex_t;


void Initialize_Dss_Netapp_Demo(void);
int dss_get_data_call_handle_of_index(int app_id ,int ind);


int qcli_ds_platform_pthread_mutex_init(
                                        qcli_ds_platform_pthread_mutex_t *mutex,
                                        void *attr);


