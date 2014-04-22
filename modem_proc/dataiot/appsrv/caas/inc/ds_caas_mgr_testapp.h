#ifndef DS_CAAS_MGR_TESTAPP_H
#define DS_CAAS_MGR_TESTAPP_H
/*===========================================================================
                      DS_CAAS_MGR_TESTAPP.H

DESCRIPTION
 Header file of DS CAAS MGR TESTAPP.

EXTERNALIZED FUNCTIONS

 Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who                            what, where, why
--------    -------                ----------------------------------------
04/05/18    xiaoyud                          First version of file
===========================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"

#include "ds_appsrv_diag.h"

#ifdef QWES_FEATURE_ENABLE

typedef enum
{
  DS_CAAS_TEST_APP_SET_CLIENT_REG_API = 1,
  DS_CAAS_TEST_APP_SET_CLIENT_GET_ACT_STAT = 2
}ds_caas_mgr_diag_set_client_type_t;

/*===========================================================================
                         GLOBAL VARIABLES
===========================================================================*/

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS_CAAS_DIAG_HDLR_PROCESS_CMDS

DESCRIPTION   This function process DIAG cmd from QXDM.

PARAMETERS    None
 
DEPENDENCIES  None 
 
RETURN VALUE  None 

SIDE EFFECTS  None
===========================================================================*/
void ds_caas_diag_hdlr_process_cmds(ds_appsrv_diag_req_type* req_ptr);

#endif /* QWES_FEATURE_ENABLE */


#endif /* DS_CAAS_MGR_TESTAPP_H */
