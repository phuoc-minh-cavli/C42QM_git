#ifndef __AUDIO_CSD_QMI_SERVICES_COMMON_H__
#define __AUDIO_CSD_QMI_SERVICES_COMMON_H__
/*===========================================================================

      A U D I O _ C S D _ Q M I _ S E R V I C E   H E A D E R    F I L E

DESCRIPTION


Copyright(c) 2010 - 2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/csd_qmi_services/inc/csd_qmi_services_common.h#1 $
$DateTime: 2018/12/25 22:39:07 $ 
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/24/13    rp     Added session name logging 
09/06/11    akj    Added Data declaration.
07/06/11    jp     Initial version.

===========================================================================*/
/****************************************************************************
  INCLUDE HEADER FILES
****************************************************************************/

#include <stddef.h>
#include <string.h>
//#include "qmi_sap.h"
#include "qmi_csi.h"

#include "msg.h"
#include "err.h"

#include "core_sound_driver_v01.h"

#include "DALSys.h"
#include "mmdefs.h"

/****************************************************************************
   DEFINES
****************************************************************************/
#define CSD_QMI_UI_PROPERTY_BUFF_SIZE 512
/* This struct can hold more information
   about the client that is connecting to the service
   if there is any reason to keep state information
   about clients */
typedef struct {
  qmi_client_handle clnt;
}client_info_type;

/* This struct is used to hold context of the server.  Cleanup is used
   to signal the reader thread to clean up and die, num_requests just tracks
   how many requests the service has handled, and service_handle is passed
   into the qmi_csi_handle_event function
   An instance of this structure is passed to the qmi_csi_register function as
   the service_cookie, and this structure can be tailored to a specific services needs */
typedef struct {
  volatile int32_t cleanup;
  qmi_csi_service_handle service_handle;
  int32_t num_requests;
}service_context_type;

/* This struct can hold more information
   about the client that is connecting to the service
   This structure is used during the event callback funtion
   to identify which client the indication response needs to be sent*/
typedef struct {
   client_info_type *clnt_info_data;
   uint32_t  handle;
} csd_cb_data_type;

/* This struct can hold more information
   about the client that is connecting to the service
   This structure is used for storing all the client related information 
   needed for sending the response message*/
struct csd_cmd_token_param_t
{
  uint32_t cmd_token;
  uint32_t cmd_type;
  uint32_t cmd_resp_ptr;
  client_info_type *clnt_info;
  qmi_req_handle req_handle;
  int32_t msg_id;
  service_context_type *service_cookie;
};
typedef struct csd_cmd_token_param_t csd_cmd_token_param_t;

struct csd_cmd_param_UI_property_data_t
{
  uint8_t param_data[CSD_QMI_UI_PROPERTY_BUFF_SIZE];
};
typedef struct csd_cmd_param_UI_property_data_t csd_cmd_param_UI_property_data_t;


/*===========================================================================
 Function Prototype

===========================================================================*/
uint32_t qmi_csd_client_info_set_list ( csd_cmd_token_param_t *client_info);
uint32_t qmi_csd_client_info_get_list ( csd_cmd_token_param_t *client_info );


#endif /* __AUDIO_CSD_QMI_SERVICES_COMMON_H__*/

