#ifndef QMI_CSI_TARGET_EXT_H
#define QMI_CSI_TARGET_EXT_H
/******************************************************************************
  @file    qmi_csi_target_ext.h
  @brief   The QMI Common Service Interface (CSI) QuRT specific Header file.

  DESCRIPTION
  Describes the QuRT specific OS parameters to QCSI
  
  ---------------------------------------------------------------------------
  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------

 *******************************************************************************/
#include "comdef.h"
#include "qurt.h"

#ifdef __cplusplus
extern "C" {
#endif

/** OS Parameters provided by the
 * service to the QCSI framework 
 */
typedef struct
{
  /***************************
   *  QURT USER SET MEMBERS  *
   ***************************/

  /** signal structure which the 
   * service thread shall wait on
   * using qurt_signal_wait() */
  qurt_signal_t *signal;

  /** Signal mask which should be 
   * set by the QCSI framework 
   * when there is an event to be
   * handled by the service. The 
   * service shall clear the signal 
   * using qurt_anysignal_clear() before
   * handling the event by calling
   * qmi_csi_handle_event() */
  unsigned int  sig;
} qmi_csi_os_params;

#ifdef __cplusplus
}
#endif
#endif
