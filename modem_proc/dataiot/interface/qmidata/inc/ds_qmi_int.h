#ifndef DS_QMI_INT_H
#define DS_QMI_INT_H
/*===========================================================================

                         D S _ Q M I _ I N T . H

DESCRIPTION

 The Data Services QMI DS INT header file.

EXTERNALIZED FUNCTIONS

   qmi_int_init()
   
     Initialize the QMI-DS INT module

Copyright (c) 2019 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/qmidata/inc/ds_qmi_int.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/26/19    mks     Created module
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================
  FUNCTION QMI_INT_INIT()

  DESCRIPTION
    Initialize the QMI-INTERNAL module 

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_int_init
(
  void
);

#endif /* DS_QMI_INT_H */
