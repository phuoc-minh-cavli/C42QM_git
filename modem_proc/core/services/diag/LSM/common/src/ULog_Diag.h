#ifndef ULOG_DIAG_H
#define ULOG_DIAG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostics Packet Processing

General Description
  ULog internal declarations.

Copyright (c) 2016, 2018-2019 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //components/rel/core.mpss/3.10/services/diag/LSM/common/src/ULog_Diag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/15/18   sa      Diagbuffer ULOG Integration in userpd.
05/25/18   gn      Read NV item value using diag_nv_read (non-blocking)
07/11/16   rs      Created file.
===========================================================================*/

#include "ULog.h"
#include "ULogFront.h"

#define DIAG_DEBUG_ULOG_SIZE  (1024 * 4)
#define DIAG_CMD_ULOG_SIZE    (1024 * 2)
#define DIAG_DATA_ULOG_SIZE   (1024 * 2)

extern ULogHandle diag_debug_ulog_handle; // ULog handle for general debug information
extern ULogHandle diag_cmd_ulog_handle;   // ULog handle for command/response and signals
extern ULogHandle diag_data_ulog_handle;  // ULog handle for log draining and transmit and flow control
extern ULogHandle diag_err_log_handle;  // ULog handle for log draining and transmit and flow control

void * ulogdiag_handler( PACKED void *req, word pkt_len );
void ULogDiag_Init(void);


#endif /* ULOG_DIAG_H */
