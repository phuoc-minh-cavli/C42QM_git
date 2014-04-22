#ifndef ULOG_DIAG_H
#define ULOG_DIAG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostics Packet Processing

General Description
  ULog internal declarations.

Copyright (c) 2022 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "ULogFront.h"


#if defined(DIAG_FLASH_LOGGING_SUPPORT)

#define DIAG_FLASH_LOGGING_ULOG_SIZE   (1024 * 2)

extern ULogHandle diag_flash_logging_ulog_handle; // ULog handle for flash logging debug information

#endif // DIAG_FLASH_LOGGING_SUPPORT && DIAG_FEATURE_EFS2

#define DIAG_DEBUG_ULOG_SIZE  (1024 * 1)

extern ULogHandle diag_debug_ulog_handle; // ULog handle for general debug information


#endif /* ULOG_DIAG_H */
