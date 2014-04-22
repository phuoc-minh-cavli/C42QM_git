#ifndef REMOTEFS_MSG_H
#define REMOTEFS_MSG_H

/*==============================================================================

                      Remote Storage Msg Module

DESCRIPTION
   This file contains RMTS Diag Messaging defines

Copyright (c) 2014 by QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/storage/remotefs/src/remotefs_msg.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

YYYY-MM-DD       who     what, where, why
--------------   ---     -------------------------------------------------------
2014-12-05       dks     Create.
==============================================================================*/


#include "msg.h"

#define RMTS_MSG_ERR(a,b,c,d) MSG_3(MSG_SSID_RMTS, MSG_LEGACY_ERROR,a,b,c,d)
#define RMTS_MSG_HIGH(a,b,c,d) MSG_3(MSG_SSID_RMTS, MSG_LEGACY_HIGH,a,b,c,d)
#define RMTS_MSG_MED(a,b,c,d)  MSG_3(MSG_SSID_RMTS, MSG_LEGACY_MED, a,b,c,d)
#define RMTS_MSG_LOW(a,b,c,d)  MSG_3(MSG_SSID_RMTS, MSG_LEGACY_LOW, a,b,c,d)

#endif /* REMOTEFS_MSG_H */

