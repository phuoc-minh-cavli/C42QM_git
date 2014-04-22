#ifndef _DS_QMI_DFS_H
#define _DS_QMI_DFS_H
/*===========================================================================

                         D S _ Q M I _ D S D . H

DESCRIPTION

 The Data Services QMI Data Service Determination header file.

EXTERNALIZED FUNCTIONS

   qmi_DFS_init()
     Initialize the QMI-DFS service and register with QCSI

Copyright (c) 2012 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/qmidata/inc/ds_qmi_dfs.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/07/12    bh     Created module
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "dstaski.h"


/*===========================================================================
  FUNCTION QMI_DFS_INIT()

  DESCRIPTION
    Initialize the QMI-DFS service and register with QCSI

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_dfs_init
(
  void
);

void qmi_dfsi_process_cmd
(
  ds_command_enum_type  cmd,
  void              *cmd_ptr
);

#endif /* _DS_QMI_DFS_H */
