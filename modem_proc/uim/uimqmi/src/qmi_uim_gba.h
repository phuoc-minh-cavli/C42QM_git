#ifndef QMI_UIM_GBA_H
#define QMI_UIM_GBA_H
/*===========================================================================

                         Q M I _ U I M _ G B A. C

DESCRIPTION

 The Data Services Qualcomm MSM Interface Device Management Services source
 file.

Copyright (c) 2016, 2018 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_gba.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/15/18    tq     Migration of QMI UIM service to QCCI-QCSI
07/26/16    sp     Move qmi_uimi_ functions to corresponding files
05/19/16    vdc    Initial Revision.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "ds_qmi_fw_common.h"
#include "mmgsdilib_common.h"
#include "gba_lib.h"
#include "qmi_uim.h"

/*===========================================================================

                            CONSTANT DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Start position of MCC and MNC
---------------------------------------------------------------------------*/
#define QMI_UIM_MNC_START_POS                  (8)
#define QMI_UIM_MCC_START_POS                  (15)

/*---------------------------------------------------------------------------
  IMSI lower & upper byte mask
---------------------------------------------------------------------------*/
#define QMI_UIM_IMSI_LOW_BYTE_MASK             0x0F
#define QMI_UIM_IMSI_HIGH_BYTE_MASK            0xF0

/*---------------------------------------------------------------------------
  Convert upper case alphabet to lower case alphabet
---------------------------------------------------------------------------*/
#define QMI_UIM_LOWER_CASE(x)  ((x >= 'A' && x <= 'Z') ? (x - 'A' + 'a') : x)

/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_UIM_GBA_CONVERT_GBA_STATUS_TO_ERRVAL

  DESCRIPTION
    To obtain QMI client-specific error message from GBA error message

  PARAMETERS
    gba_status   : GBA return value

  RETURN VALUE
    errval       : QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qmi_uim_gba_convert_gba_status_to_errval
(
  gba_result_enum_type gba_status
);

/*===========================================================================
  FUNCTION QMI_UIMI_GBA_CALLBACK()

  DESCRIPTION
    This function is executed as callback of GBA API. This function makes a
    deep copy of the results and post a command to the QMI task to be
    processed.

  PARAMETERS

  RETURN VALUE
    None.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_uimi_gba_callback
(
  gba_result_enum_type          status,
  uint32                        request_id,
  const gba_response_data_type *cnf_ptr,
  const void                   *user_data_ptr
);

/*===========================================================================
  FUNCTION  QMI_UIM_GBA_PROCESS_GBA_CB()

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
void qmi_uim_gba_process_gba_cb
(
  qmi_uim_message_type * uim_message_ptr
);


/*===========================================================================
  FUNCTION QMI_UIMI_GBA()

  DESCRIPTION
    Performs GBA authentication

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
qmi_csi_error qmi_uimi_gba
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
);

/*===========================================================================
  FUNCTION QMI_UIMI_GET_GBA_IMPI()

  DESCRIPTION
    Retrieves the IMPI used for GBA corresponding to the session

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
qmi_csi_error qmi_uimi_get_gba_impi
(
  qmi_uimi_client_state_type  * cl_sp,
  qmi_req_handle                req_handle,
  void                        * req_c_struct,
  int                           req_c_struct_len
);

#endif /* QMI_UIM_GBA_H */
