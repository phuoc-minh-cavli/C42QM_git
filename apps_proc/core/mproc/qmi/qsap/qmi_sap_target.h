#ifndef QMI_SAP_TARGET_H
#define QMI_SAP_TARGET_H
/******************************************************************************
  @file    qmi_sap_target.h
  @brief   The QMI Service Access Proxy (SAP) Target specific  Header file.

  DESCRIPTION
  QMI SAP Rex OS specific defines.
  
  ---------------------------------------------------------------------------
  Copyright (c) 2011-2012, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "msg.h"

#include "qurt.h"
#include "err.h"
#include "smem_log.h"
#include "qmi_common.h"

typedef qurt_mutex_t qmi_sap_lock_type;
#define QSAP_LOCK(ptr)        qurt_mutex_lock(ptr)
#define QSAP_UNLOCK(ptr)      qurt_mutex_unlock(ptr)
#define QSAP_LOCK_INIT(ptr)   qurt_mutex_init(ptr)
#define QSAP_LOCK_DEINIT(ptr) qurt_mutex_destroy(ptr)

#endif /* QMI_SAP_TARGET_H */
