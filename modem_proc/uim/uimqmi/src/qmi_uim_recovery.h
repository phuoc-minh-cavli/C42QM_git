#ifndef QMI_UIM_RECOVERY_H
#define QMI_UIM_RECOVERY_H
/*===========================================================================

                         Q M I _ U I M _ R E C O V E R Y. H

DESCRIPTION

 The Data Services Qualcomm MSM Interface Device Management Services source
 file.

Copyright (c) 2016, 2018 - 2019 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_recovery.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/22/19    tq     Masked unsupported event registration 
10/15/18    tq     Migration of QMI UIM service to QCCI-QCSI
07/14/16    ar     Move qmi_uimi_recovery and unify fdn/acl get_status func.
05/19/16    vdc    Initial Revision.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/

/*===========================================================================

                                DATA TYPES

===========================================================================*/


/*===========================================================================

                               INTERNAL DATA

===========================================================================*/


/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/


/*===========================================================================
  FUNCTION QMI_UIMI_RECOVERY_CALLBACK()

  DESCRIPTION
    This function is executed as callback of UIM Recovery API. This
    is executed in the context of UIM task and needs to make a deep
    copy of the results and post a command to the QMI task to be
    processed.

  PARAMETERS

  RETURN VALUE
    None.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uimi_recovery_callback
(
  uim_slot_type                     slot,
  const void                       *user_data,
  uimdrv_recovery_stage_enum_type   recovery_stage
);

/*===========================================================================
  FUNCTION  QMI_UIM_RECOVERY_PROCESS_CALLBACK()

  DESCRIPTION
    Generic QMI processing for external commands

  PARAMETERS
    uim_message_ptr : message to the internal qmi_uim_message
                      containing an external command

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_recovery_process_callback
(
  qmi_uim_message_type * uim_message_ptr
);

/*===========================================================================
  FUNCTION QMI_UIM_PROCESS_RECOVERY_COMPLETE()

  DESCRIPTION
    This function processes the recovery complete indication from UIM drivers.
    It passes indicates that recovery has completed successfully to all registered
    QMI clients and QMI CAT.

  PARAMETERS
    slot : slot in which the recovery was performed

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_process_recovery_complete
(
  uim_slot_type        slot
);

/*===========================================================================
  FUNCTION QMI_UIM_RECOVERY_PROCESS_COMPLETE_EVT()

  DESCRIPTION
    This function processes the recovery complete indication from UIM drivers.
    It indicates to all registered QMI clients and to QMI CAT that recovery
    has completed successfully.

  PARAMETERS
    mmgsdi_slot:  Slot in which the recovery was performed

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uim_recovery_process_complete_evt
(
  mmgsdi_slot_id_enum_type        mmgsdi_slot
);

/*===========================================================================
  FUNCTION QMI_UIMI_RECOVERY()

  DESCRIPTION
    Recovers the card information

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
qmi_csi_error qmi_uimi_recovery
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
);

/*===========================================================================
  FUNCTION QMI_UIM_IS_RECOVERY_SUPPORTED()

  DESCRIPTION
    this fucntion indicates qmi uim recovery indication
    is supported or not based upon the file compilation.
    qmi_uim_recovery.c or qmi_uim_recovery_stubs.c

  PARAMETERS
    None
    
  RETURN VALUE
    TRUE: if qmi_uim_recovery.c got compiled
    FALSE: if qmi_uim_recovery_stubs.c got compiled
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_uim_is_recovery_supported(void);
#endif /* QMI_UIM_RECOVERY_H */
