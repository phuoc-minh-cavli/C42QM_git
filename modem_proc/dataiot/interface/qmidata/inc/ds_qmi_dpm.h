#ifndef _DS_QMI_DPM_H
#define _DS_QMI_DPM_H
/*===========================================================================

                         D S _ Q M I _ D P M . H

DESCRIPTION

 The Data Services QMI Data Port Mapper header file.

EXTERNALIZED FUNCTIONS

   qmi_dpm_init()
     Initialize the QMI-DPM service and register with QCSI

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/qmidata/inc/ds_qmi_dpm.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/14/13    bh     Created module
===========================================================================*/

#include "comdef.h"
#include "customer.h"

/*===========================================================================
  FUNCTION QMI_DPM_INIT()

  DESCRIPTION
    Initialize the QMI-DPM service and register with QCSI

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_dpm_init
(
  void
);

#endif /* _DS_QMI_DPM_H */
