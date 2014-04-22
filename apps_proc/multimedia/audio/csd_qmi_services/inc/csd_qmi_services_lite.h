#ifndef __CSD_QMI_SERVICES_LITE_H__
#define __CSD_QMI_SERVICES_LITE_H__
/*===========================================================================

 C S D _ Q M I _ S E R V I C E _ L I T E   H E A D E R    F I L E

DESCRIPTION


Copyright(c) 2015 by Qualcomm Technologies, Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/csd_qmi_services/inc/csd_qmi_services_lite.h#1 $
$DateTime: 2018/12/25 22:39:07 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/08/15    jy     Initial version.

===========================================================================*/
/****************************************************************************
  INCLUDE HEADER FILES
****************************************************************************/
#include "csd_qmi_services_common.h"
#include "core_sound_driver_v01.h"
#include "csd.h"
#include "csd_vcommon.h"
#include "csd_debug_msg.h"

/****************************************************************************
   DEFINES
****************************************************************************/


/*===========================================================================
 Function Prototype

===========================================================================*/

qmi_csi_cb_error qmi_csdlite_handle_req
( client_info_type *clnt_info, qmi_req_handle req_handle,
  int32_t msg_id, void *req_c_struct, int32_t req_c_struct_len,
  void *service_cookie);
  
#endif /* __CSD_QMI_SERVICES_LITE_H__*/

