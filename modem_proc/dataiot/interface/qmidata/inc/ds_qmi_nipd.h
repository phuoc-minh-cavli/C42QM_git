#ifndef _DS_QMI_NIPD_H
#define _DS_QMI_NIPD_H
/*===========================================================================

                         D S _ Q M I _ N I P D . H

DESCRIPTION

 The Data Services QMI NON IP DATA header file.

EXTERNALIZED FUNCTIONS

   qmi_nipd_init()
   
     Initialize the QMI-NIPD service and register with QCSI

Copyright (c) 2017 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/qmidata/inc/ds_qmi_nipd.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/07/12    ad     Created module
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "dstaski.h"

#include "dsm.h"
#include "ds_fwk.h"




/*===========================================================================
  FUNCTION QMI_NIPD_INIT()

  DESCRIPTION
    Initialize the QMI-NIPD service and register with QCSI

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void  qmi_nipd_init
(
  void
);

/*===========================================================================
  FUNCTION QMI_NIPDI_DOWN_LINK_DATA_IND()

  DESCRIPTION
    Generate downlink indication and send to registered client

  PARAMETERS
    tx_pkt    : Pakcet to be transmitted
    um_iface_ptr : NON IP um iface ptr

  RETURN VALUE
    void.

  DEPENDENCIES
    qmi_nipdi_init has to be called.

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_nipdi_down_link_data_indication
(
  dsm_item_type                          **tx_pkt,
  ds_fwk_clnt_handle_type                  fwk_client
);

void qmi_nipdi_process_cmd
(
  ds_command_enum_type  cmd,
  void              *cmd_ptr
);


#endif /* _DS_QMI_NIPD_H */
