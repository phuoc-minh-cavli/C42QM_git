#ifndef _DS_QMI_QTEELS_CLIENT_H
#define _DS_QMI_QTEELS_CLIENT_H
/*===========================================================================

               D S _ Q M I _ Q T E E L S _ C L I E N T . H

DESCRIPTION


EXTERNALIZED FUNCTIONS


Copyright (c) 2019 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: 

when        who         what, where, why
--------    ---        -----------------------------------------------------
18/03/18    sraghuve    First version of file
===========================================================================*/

#ifdef QWES_FEATURE_ENABLE 

#include "comdef.h"
#include "customer.h"
#include "ds_sys_eventi.h"

/*===========================================================================
  FUNCTION QMI_QTEELS_CLIENT_INIT()

  DESCRIPTION
    Initialize the client module

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void  qmi_qteels_client_init
(
  void
);


/*===========================================================================
  FUNCTION QMI_QTEELS_SEND_FEATURE_LIST_INFO()

  DESCRIPTION
    This API is used to send the feature status request message to TZ

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void qmi_qteels_send_feature_list_info
(
  ds_sys_caas_feature_list_type  * list_ptr
);




#endif /* QWES_FEATURE_ENABLE  */

#endif /* _DS_QMI_QTEELS_CLIENT_H */
