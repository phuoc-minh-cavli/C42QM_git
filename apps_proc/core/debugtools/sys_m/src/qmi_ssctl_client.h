#ifndef QMI_SSCTL_CLIENT_H
#define QMI_SSCTL_CLIENT_H
/*===========================================================================

           QMI_SSCTL_CLIENT . H

  Copyright (c) 2014-17 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtools/sys_m/src/qmi_ssctl_client.h#1 $

 when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
 02/06/14   rks      Initial Version
   
===========================================================================*/

/*===========================================================================
   
                        INCLUDE FILES FOR MODULE
   
   ===========================================================================*/

#include "qmi_csi.h"
#include "subsystem_control_v02.h"


typedef enum _ssctl_status_t
{
   SSCTL_SUCCESS      =  0,
   SSCTL_FAILURE      =  1,
   SSCTL_IN_PROCESS   =  2,
}ssctl_status_t;

typedef enum _ssctl_cmd_type{
   SSCTL_SHUTDOWN           = QMI_SSCTL_SHUTDOWN_REQ_V02,
   SSCTL_RESTART            = QMI_SSCTL_RESTART_REQ_V02,
   SSCTL_FAILURE_REASON     = QMI_SSCTL_GET_FAILURE_REASON_REQ_V02,
   SSCTL_SUBSYS_EVENT       = QMI_SSCTL_SUBSYS_EVENT_REQ_V02,
   SSCTL_HIBERNATION        = QMI_SSCTL_HIBERNATION_REQ_V02,
}ssctl_cmd_type;

typedef enum _ssctl_svc_type{
   MPSS_SSCTL_SVC =  SSCTL_QMI_SERVICE_INSTANCE_MPSS_V02,
   ADSP_SSCTL_SVC =  SSCTL_QMI_SERVICE_INSTANCE_ADSP_V02,
}ssctl_svc_type;

/*===========================================================================

  FUNCTION:  ssctl_v02_send_req

===========================================================================*/
/*!
    @brief
    Initialize the SSCTL QMI client and send request to the SSSCTL service
    at the peripheral requested
    @detail

    @return
    SSCTL_SUCCESS : Request sent successfully 
    SSCTL_FAILURE : Request failed
    SSREQ_IN_PROCESS: already a request is in process
*/
/*=========================================================================*/
ssctl_status_t ssctl_v02_send_req(ssctl_svc_type ssctl_service, 
                                      ssctl_cmd_type cmd,
                                      qmi_ssctl_subsys_event_req_msg_v02 * ss_evt);

/**
  @brief ssctl_v02_send_ind

  This function sends indication back to ssctl_v02 client


  @return : none.
*/

void ssctl_v02_send_ind(void);


#endif  /* QMI_SSREQ_CLIENT_H */

