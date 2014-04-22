#ifndef _DS_QMI_AT_H
#define _DS_QMI_AT_H
/*===========================================================================

                         D S _ Q M I _ A T . H

DESCRIPTION

 The Data Services QMI Atcop Service header file.


Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/qmidata/inc/ds_qmi_at.h#3 $ $DateTime: 2022/10/06 21:20:24 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/17/10    mkumarsa    Creted module.
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_ATCOP
#ifdef FEATURE_DATA_QMI_AT
#include "ds_qmi_defs.h"
#include "ds_qmi_if_ext.h"
#include "dstaski.h"


/*===========================================================================
  FUNCTION QMI_AT_INIT()

  DESCRIPTION
    Register the AT service with QMUX for all applicable QMI links

  PARAMETERS
    QMI Instance

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_at_init
(
  void
);

void qmi_ati_process_cmd
(
  ds_command_enum_type  cmd,
  void *cmd_ptr
);

#endif /* FEATURE_DATA_QMI_AT */
#endif /* FEATURE_DATA_ATCOP */
#endif /* _DS_QMI_AT_H */
