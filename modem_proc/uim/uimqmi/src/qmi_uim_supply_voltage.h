#ifndef QMI_UIM_SUPPLY_VOLTAGE_H
#define QMI_UIM_SUPPLY_VOLTAGE_H

/*================================================================================

DESCRIPTION


Copyright (c) 2016, 2018 - 2019 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_supply_voltage.h#1 $$ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
01/22/19   tq      Masked unsupported event registration 
10/15/18   tq      Migration of QMI UIM service to QCCI-QCSI
07/26/16   sp      Move qmi_uimi_ functions to corresponding files
06/16/16   vdc     Split QMI UIM parsing files for supporting IOE
05/20/16   ar      Initial version
=============================================================================*/

/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h"
#include "mmgsdilib_common.h"
#include "qmi_uim.h"
#include "qmi_svc_defs.h"

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*===========================================================================
  FUNCTION QMI_UIM_ALL_REG_CLIENTS_ACK_VCC_DEACTIVATION()

  DESCRIPTION
    Indicates whether all registered clients have acknowledged Vcc
    power down.

  PARAMETERS
    mmgsdi_slot : MMGSDI slot id

  RETURN VALUE
    qmi_error_e_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_e_type qmi_uim_all_reg_clients_ack_vcc_deactivation
(
  mmgsdi_slot_id_enum_type  mmgsdi_slot
);

/*===========================================================================
  FUNCTION QMI_UIM_PROCESS_SUPPLY_VOLTAGE_IND()

  DESCRIPTION
    This function processes the supply voltage indication from UIM drivers.
    It indicates either Vcc needs to be deactivated or that Vcc activation has
    occurred to all registered QMI clients.

  PARAMETERS
    uim_message_ptr: QMI UIM message

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_process_supply_voltage_ind
(
  const qmi_uim_message_type * uim_message_ptr
);

/*===========================================================================
  FUNCTION QMI_UIMI_SUPPLY_VOLTAGE()

  DESCRIPTION
    Marks that the client has acknowledged the supply voltage indication
    and once all clients have responded, notifies uimdrv to proceed with
    Vcc deactivation.

  PARAMETERS
    cl_sp            : client state pointer
    req_handle       : req handle
    req_c_struct     : IDL client structure
    req_c_struct_len : structure len

  RETURN VALUE
    qmi_csi_error

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_csi_error qmi_uimi_supply_voltage
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
);

/*===========================================================================
  FUNCTION QMI_UIM_IS_SUPPLY_VOLTAGE_SUPPORTED()

  DESCRIPTION
    this fucntion indicates qmi uim supply_voltage indication
    is supported or not based upon the file compilation.
    qmi_uim_supply_voltage.c or qmi_uim_supply_voltage_stubs.c

  PARAMETERS
    None
    
  RETURN VALUE
    TRUE: if qmi_uim_supply_voltage.c got compiled
    FALSE: if qmi_uim_supply_voltage_stubs.c got compiled
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_is_supply_voltage_supported(void);
#endif /* QMI_UIM_SUPPLY_VOLTAGE_H */
