/*===========================================================================

                            DS APPSRV DIAG

GENERAL DESCRIPTION
  This software unit contains functions for 3G Data Services diag handling.
  It provides APIs for DS3G diag registration and processing.

  Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/appsrv/utils/src/ds_appsrv_diag.c#2 $

when         who           what, where, why
--------   -------       --------------------------------------------
05/10/18   xiaoyud         Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"
#include "ds_system_heap.h"
#include "ds_appsrv_task.h"
#ifndef TEST_FRAMEWORK
#ifdef FEATURE_DATA_APPSRV_DIAG_SUPPORT
#include "ds_appsrv_diag.h"
#include <stringl/stringl.h>
#include "ds_bearer_manager.h"

#ifdef QWES_FEATURE_ENABLE
#include "ds_caas_mgr_testapp.h"
#endif /* QWES_FEATURE_ENABLE */


/*===========================================================================

                      LOCAL DATA STRUCTURES

===========================================================================*/

/*===========================================================================

                      INTERNAL FUNCTIONS

===========================================================================*/
LOCAL void ds_appsrv_diag_cmd_dispatcher(uint32* msg_content_ptr);

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.
===========================================================================*/


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_APPSRV_DIAG_CMD_HDLR

DESCRIPTION   This function is the called by DS_FWK DIAG framework to
              process APPSRV diag cmds.

DEPENDENCIES  None

RETURN VALUE  A void* response pointer back to diag

SIDE EFFECTS  None
===========================================================================*/
void* ds_appsrv_diag_cmd_hdlr
(
  void*    diag_msg_ptr,
  uint16   diag_msg_len
)
{
  ds_appsrv_diag_req_type       *req_ptr = NULL;
  ds_appsrv_diag_rsp_type       *rsp_ptr = NULL;
  diagpkt_subsys_cmd_code_type  subsys_req_code = 0;
  boolean                       rsp_status = FALSE;
  ds_appsrv_diag_req_type*      local_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (diag_msg_ptr == NULL)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "NULL diag msg ptr passed. Returning");
    return rsp_ptr;
  }

  if ( diag_msg_len < sizeof(ds_appsrv_diag_req_type))
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "Incorrect diag msg lenth passed. Returning");
    return rsp_ptr;
  }

  req_ptr = (ds_appsrv_diag_req_type*)diag_msg_ptr;

  subsys_req_code = diagpkt_subsys_get_cmd_code((void*)req_ptr);

  DATA_APPSRV_MSG8(MSG_LEGACY_HIGH,
                  "subsys req_code: %d, cmd_code: %d, [0]: %d, [1]: %d, [2]: %d,"
                  " [3]: %d, [4]: %d, [5]: %d",
                  subsys_req_code,
                  req_ptr->payload.cmd_code,
                  req_ptr->payload.cmd_data_1,
                  req_ptr->payload.cmd_data_2,
                  req_ptr->payload.cmd_data_3,
                  req_ptr->payload.cmd_data_4,
                  req_ptr->payload.cmd_data_5,
                  req_ptr->payload.cmd_data_6);

  switch (req_ptr->payload.cmd_code)
  {
    #ifdef QWES_FEATURE_ENABLE
    case DS_APPSRV_CMD_CODE_REQ_ITEM_CAAS_SET_CLIENT:
    case DS_APPSRV_CMD_CODE_REQ_ITEM_CAAS_SET_COMM:
    case DS_APPSRV_CMD_CODE_REQ_ITEM_CAAS_SET_DEFAULT_ALLOWED_STATUS:
      rsp_status = TRUE;
      break;
    #endif /* QWES_FEATURE_ENABLE */
    case DS_APPSRV_CMD_CODE_REQ_ITEM_BEARER_FLOW_DISABLE:
    case DS_APPSRV_CMD_CODE_REQ_ITEM_BEARER_FLOW_ENABLE:
      rsp_status = TRUE;
      break;
    default:
      DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "Unsupported diag cmd_code %d",
                      req_ptr->payload.cmd_code);
      rsp_status = FALSE;
      break;
  }

  /*-------------------------------------------------------------------------
   Give back Diag response
  -------------------------------------------------------------------------*/
  rsp_ptr =  (ds_appsrv_diag_rsp_type*)
               diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_DS_DS3G,
                                    subsys_req_code,
                                    sizeof(ds_appsrv_diag_rsp_type));
  if (rsp_ptr != NULL)
  {
    rsp_ptr->status = rsp_status;
    rsp_ptr->payload.cmd_code = req_ptr->payload.cmd_code;
    rsp_ptr->payload.cmd_data_1 = req_ptr->payload.cmd_data_1;
    rsp_ptr->payload.cmd_data_2 = req_ptr->payload.cmd_data_2;
    rsp_ptr->payload.cmd_data_3 = req_ptr->payload.cmd_data_3;
    rsp_ptr->payload.cmd_data_4 = req_ptr->payload.cmd_data_4;
    rsp_ptr->payload.cmd_data_5 = req_ptr->payload.cmd_data_5;
    rsp_ptr->payload.cmd_data_6 = req_ptr->payload.cmd_data_6;
  }
  else
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "diag rsp_ptr allocation failed");
  }

  if( rsp_status == TRUE )
  {
    /*-------------------------------------------------------------------------
      Context switch to DS task
    -------------------------------------------------------------------------*/
    DS_SYSTEM_HEAP_MEM_ALLOC(local_ptr,
                             sizeof(ds_appsrv_diag_req_type),
                             ds_appsrv_diag_req_type*);
    if(local_ptr == NULL) 
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "Unable to allocate memory to local_ptr.");
      return rsp_ptr;
    }

    memscpy(local_ptr,sizeof(ds_appsrv_diag_req_type),
            req_ptr,sizeof(ds_appsrv_diag_req_type));

    ds_appsrv_put_cmd(ds_appsrv_diag_cmd_dispatcher, (uint32*)local_ptr);
  }

  return rsp_ptr;
} /* ds_appsrv_diag_cmd_hdlr() */

/*===========================================================================
FUNCTION      DS_APPSRV_DIAG_CMD_DISPATCHER

DESCRIPTION   This function dispatches DataAppsrv DIAG cmd to different modules. 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
LOCAL void ds_appsrv_diag_cmd_dispatcher
(
	uint32*  msg_content_ptr
)
{
  ds_appsrv_diag_req_type  *req_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (msg_content_ptr == NULL)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "Null pointer: msg_content_ptr.");
    return;
  } 
  req_ptr = (ds_appsrv_diag_req_type*)msg_content_ptr;

  switch (req_ptr->payload.cmd_code)
  {
    #ifdef QWES_FEATURE_ENABLE
    case DS_APPSRV_CMD_CODE_REQ_ITEM_CAAS_SET_CLIENT:
    case DS_APPSRV_CMD_CODE_REQ_ITEM_CAAS_SET_COMM:
    case DS_APPSRV_CMD_CODE_REQ_ITEM_CAAS_SET_DEFAULT_ALLOWED_STATUS:
      (void)ds_caas_diag_hdlr_process_cmds(req_ptr);
      break;
    #endif /* QWES_FEATURE_ENABLE */
	
    case DS_APPSRV_CMD_CODE_REQ_ITEM_BEARER_FLOW_DISABLE:
      ds_bearer_mgr_diag_tool_flow_control(FALSE,req_ptr->payload.cmd_data_1);
      break;
    case DS_APPSRV_CMD_CODE_REQ_ITEM_BEARER_FLOW_ENABLE:
      ds_bearer_mgr_diag_tool_flow_control(TRUE, req_ptr->payload.cmd_data_1);
      break;
    default:
      DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "Unsupported diag cmd_code %d",
                      req_ptr->payload.cmd_code);
      break;
  }
  if (req_ptr != NULL)
  {
    DS_SYSTEM_HEAP_MEM_FREE(req_ptr);
  }
  return;
}/*ds_appsrv_diag_cmd_dispatcher*/
#endif /*TEST_FRAMEWORK*/
#endif /*FEATURE_DATA_APPSRV_DIAG_SUPPORT*/

