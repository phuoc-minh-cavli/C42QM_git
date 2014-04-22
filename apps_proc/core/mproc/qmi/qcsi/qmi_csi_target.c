/******************************************************************************
  @file    qmi_cci_target.c
  @brief   OS Specific routines internal to QCCI.

  DESCRIPTION
  This source provides an OS abstraction to QCCI.
  
  The current design is to use QuRT abstraction for all interfaces except
  when the user is involved. If the user provides NULL as the rex TCB type,
  the QuRT abstraction is automatically used.

  ---------------------------------------------------------------------------
  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/qmi/qcsi/qmi_csi_target.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

/*===========================================================================
                      INCLUDES
===========================================================================*/
#include "qmi_csi_target.h"


size_t strlcpy(char *dst, const char *src, size_t siz);

extern void qmi_csi_os_signal_set(qmi_csi_os_params *sig)
{
  ASSERT(sig->signal);
  if(sig->signal)
  {
    qurt_signal_set(sig->signal, sig->sig);
  }
}

void qcsi_log_error(char *filename, unsigned int line)
{
  uint32 name[5];
  strlcpy((char *)name, filename, sizeof(name));
#ifdef FEATURE_QMI_SMEM_LOG
  SMEM_LOG_EVENT6(QMI_CSI_LOG_EVENT_ERROR, name[0], name[1], name[2], name[3], 
      name[4], line);
#endif
#ifdef FEATURE_QMI_MSG
  MSG_2(MSG_SSID_ONCRPC, MSG_LEGACY_ERROR,
        "Runtime error. File 0x%s, Line: %d", filename, line);
#endif
}

