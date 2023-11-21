#ifndef QSOCKET_IDS_H
#define QSOCKET_IDS_H
/******************************************************************************
  @file    qsocket_ids.h
  
  @brief   List of approved QSocket IDs

  Copyright (c) 2014 Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
  
  $Header: //components/rel/qmimsgs.tx/1.0/common/api/qsocket_ids.h#1 $
*******************************************************************************/

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

  enum {
    QSOCKET_MIN_ID = 0x1000,
    QSOCKET_TFTP_ID = QSOCKET_MIN_ID,
    QSOCKET_DIAG_ID,
    QSOCKET_MAX_ID,
    QSOCKET_VENDOR_MIN_ID = 0x1800,
    QSOCKET_VENDOR_MAX_ID = 0x2000
  }qsocket_ids_enum;

#ifdef __cplusplus
}
#endif
#endif

