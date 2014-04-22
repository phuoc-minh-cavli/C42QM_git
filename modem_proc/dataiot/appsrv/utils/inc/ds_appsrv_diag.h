#ifndef DS_APPSRV_DIAG_H
#define DS_APPSRV_DIAG_H
/*===========================================================================

                                DS APPSRV DIAG

                           H E A D E R   F I L E

GENERAL DESCRIPTION
  This software unit contains functions for 3G Data Services diag handling.
  It provides APIs for DS3G diag registration and processing.

  Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/appsrv/utils/inc/ds_appsrv_diag.h#2 $

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

#include "diagpkt.h"
#include "diagcmd.h"
#include "diagdiag.h"
#include "dstask.h"
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


#ifdef QWES_FEATURE_ENABLE
/*==============================================================================
                 EXTERNAL VARIABLE DEFINITIONS AND TYPES
==============================================================================*/
#define DS_APPSRV_CMD_CODE_REQ_ITEM_CAAS_SET_CLIENT    1
#define DS_APPSRV_CMD_CODE_REQ_ITEM_CAAS_SET_COMM      2
#define DS_APPSRV_CMD_CODE_REQ_ITEM_CAAS_SET_DEFAULT_ALLOWED_STATUS 3
#endif

#define DS_APPSRV_CMD_CODE_REQ_ITEM_BEARER_FLOW_DISABLE 4
#define DS_APPSRV_CMD_CODE_REQ_ITEM_BEARER_FLOW_ENABLE  5



/* ds appsrv diag cmd payload structure */
typedef PACKED struct PACKED_POST 
{
  /* cmd_code */
  uint16  cmd_code;
  /* CMD data can be used for different purposes depending on the cmd_code*/
  uint32 cmd_data_1;
  uint32 cmd_data_2;
  uint32 cmd_data_3;
  uint32 cmd_data_4;
  uint32 cmd_data_5;
  uint32 cmd_data_6;
}ds_appsrv_diag_req_payload_type;

/* diag req type structure */
typedef PACKED struct PACKED_POST 
{
  /* Sub System header */
  diagpkt_subsys_header_type        header;
  /* Payload sent for the requested cmd */
  ds_appsrv_diag_req_payload_type   payload;     
}ds_appsrv_diag_req_type;

/* diag response type structure */
typedef PACKED struct PACKED_POST 
{
  /* Sub System header */
  diagpkt_subsys_header_type       header;   
  /* echo back the cmd code process status */
  boolean                          status;
  /* Payload sent for the requested cmd */
  ds_appsrv_diag_req_payload_type  payload;
}ds_appsrv_diag_rsp_type;


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
void* ds_appsrv_diag_cmd_hdlr(void* diag_msg_ptr, uint16 diag_msg_len);


#endif /* DS_APPSRV_DIAG_H */
