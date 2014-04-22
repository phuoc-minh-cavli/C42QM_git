#ifndef QCMAP_EXT_API_H
#define QCMAP_EXT_API_H

/*===========================================================================
                         QCMAP_EXT_API_H

DESCRIPTION
  This header file contains API's which are exposed to external tasks/applications
  from QCMAP task

  Copyright (c)2016 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/


/*==============================================================================

                            EDIT HISTORY FOR FILE
when        who    what, where, why
--------    ---    -------------------------------------------------------------
09/16/2016   sr     created file
==============================================================================*/
#include "comdef.h"


/*==============================================================================
Callback Type used by QCMAP Client to register a callback with QCMAP Server 
to send responses and Indications
==============================================================================*/
typedef void (*qcmap_client_resp_ind_cb)(uint8 evt, void* data, uint8 idx);



#endif

