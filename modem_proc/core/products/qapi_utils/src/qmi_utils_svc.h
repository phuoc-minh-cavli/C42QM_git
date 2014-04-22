#ifndef QMI_UTIL_SVC_H
#define QMI_UTIL_SVC_H

/*============================================================================
  @file qmi_util_svc.h

  Function and data structure declarations for qmi_util server
  framework logging.


  Copyright (c) 2022 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

==============================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2022-05-05   sn   created file for qmi server logging.

============================================================================*/


#include "msg.h"


#define FS_FOP_LOG 2

#define UTILS_MSG_ERROR_0(a) \
	MSG(MSG_SSID_DFLT,MSG_LEGACY_ERROR,a)
#define UTILS_MSG_ERROR_1(a,b) \
	MSG_1(MSG_SSID_DFLT,MSG_LEGACY_ERROR,a,b)
#define UTILS_MSG_ERROR_2(a,b,c) \
	MSG_2(MSG_SSID_DFLT,MSG_LEGACY_ERROR,a,b,c)
#define UTILS_MSG_ERROR_3(a,b,c,d) \
	MSG_3(MSG_SSID_DFLT,MSG_LEGACY_ERROR,a,b,c,d)

#define UTILS_MSG_HIGH_0(a) \
	MSG(MSG_SSID_DFLT,MSG_LEGACY_HIGH,a)
#define UTILS_MSG_HIGH_1(a,b) \
	MSG_1(MSG_SSID_DFLT,MSG_LEGACY_HIGH,a,b)
#define UTILS_MSG_HIGH_2(a,b,c) \
	MSG_2(MSG_SSID_DFLT,MSG_LEGACY_HIGH,a,b,c)
#define UTILS_MSG_HIGH_3(a,b,c,d) \
	MSG_3(MSG_SSID_DFLT,MSG_LEGACY_HIGH,a,b,c,d)

#endif 
