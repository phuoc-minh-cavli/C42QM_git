/******************************************************************************
 * FILE: qmi_utils_svc_main.c
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

 *==================================================================*/


/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 *  $Header: //components/dev/core.mpss/3.5/snanaval.CORE.MPSS.3.5.mpss_efs/producst/qapi_utils/src/qmi_utils_svc_main.c#1 $ $DateTime: 2022/03/02 06:16:25 $ $Author: snanaval $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 04/01/22     sna      created the file
 *==================================================================*/

#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "task.h"
#include "rcinit.h"
#include "rex.h"
#include <string.h>
#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "util_counter_v01.h"

#ifdef FEATURE_QMI_NATIVE_REX
typedef unsigned char *rex_stack_pointer_type;
typedef unsigned long rex_task_func_param_type;
#endif

#define QMI_SVC_WAIT_SIG 0x00010000

#define QMI_UTIL_SERVICE_STACK_SIZE  (2048)
static rex_tcb_type        qmi_util_service_tcb;
static char qmi_util_service_stack[QMI_UTIL_SERVICE_STACK_SIZE];
static rex_task_attributes_type qmi_util_service_task_attributes;

extern void *qmi_util_register_service(qmi_csi_os_params *os_params);

/* 
qmi utility service thread will start handling the service request 
 from the threadx client side

*/
static void qmi_util_service_thread(uint32 handle)
{
  qmi_csi_os_params os_params; 
  void *sp;

  os_params.tcb = rex_self();
  os_params.sig = QMI_SVC_WAIT_SIG;

  sp = qmi_util_register_service(&os_params);

  while(1)
  {
    rex_wait(QMI_SVC_WAIT_SIG);
    rex_clr_sigs(os_params.tcb, QMI_SVC_WAIT_SIG);
    if(sp)
    {
      qmi_csi_handle_event(sp, &os_params);
    }
  }
}

/* 
qmi utility service will start the service thread to address the request 
 from the threadx client side

*/
void qmi_util_service_start(void)
{
  unsigned int prio = 10;   
  char task_name[] = "QMI_UTIL_SVC";
  rex_task_attr_init(& qmi_util_service_task_attributes);
  rex_task_attr_set_stack_addr(&qmi_util_service_task_attributes,
                               (rex_stack_pointer_type)qmi_util_service_stack);
  rex_task_attr_set_stack_size(&qmi_util_service_task_attributes,
                                QMI_UTIL_SERVICE_STACK_SIZE);
  rex_task_attr_set_prio(&qmi_util_service_task_attributes, prio);
  rex_task_attr_set_entry_func(& qmi_util_service_task_attributes,
                               qmi_util_service_thread,
                               NULL);
  rex_task_attr_set_initially_suspended(&qmi_util_service_task_attributes,
                                        FALSE);
  rex_task_attr_set_task_name(&qmi_util_service_task_attributes,
                              task_name);
  rex_common_def_task(&qmi_util_service_tcb, & qmi_util_service_task_attributes);
}
