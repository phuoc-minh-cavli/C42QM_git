#ifndef _DS_QMI_DSD_H
#define _DS_QMI_DSD_H
/*===========================================================================

                         D S _ Q M I _ D S D . H

DESCRIPTION

 The Data Services QMI Data Service Determination header file.

EXTERNALIZED FUNCTIONS

   qmi_dsd_init()
     Initialize the QMI-DSD service and register with QCSI

Copyright (c) 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/qmidata/inc/ds_qmi_dsd.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/07/12    sj     Created module
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "dstaski.h"

/*===========================================================================
  FUNCTION QMI_DSD_INIT()

  DESCRIPTION
    Initialize the QMI-DSD service and register with QCSI

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_dsd_init
(
  void
);

void qmi_dsdi_process_cmd
(
  ds_command_enum_type  cmd,
  void              *cmd_ptr
);
#endif /* _DS_QMI_DSD_H */
