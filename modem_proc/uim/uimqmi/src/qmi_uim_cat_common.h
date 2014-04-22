/*================================================================================

DESCRIPTION


Copyright (c) 2012 - 2013, 2016, 2018 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary

$Header: //components/rel/uim.mpss/3.3.1/uimqmi/src/qmi_uim_cat_common.h#1 $$ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
10/15/18    tq     Migration of QMI UIM service to QCCI-QCSI
08/10/16    ar     Support for setting terminal capability from AP
03/08/16    vr     Common function for result tlv and send response
03/08/16    vr     Remove duplicated code between QMI CAT and QMI UIM
03/27/14    at     Usage of common QMI APIs from DS header
07/02/13    kb     Added support for memory leak detection
06/12/13    kb     Added QMI malloc and free wrapper functions
09/07/12    at     Initial version
=============================================================================*/

#ifndef QMI_UIM_CAT_COMMON_H
#define QMI_UIM_CAT_COMMON_H

/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "customer.h"
#include "comdef.h"
#include "mmgsdilib_common.h"
#include "qmi_svc_utils.h"
#include "qmi_framework.h"
#include "uim_common_efs.h"
#include "user_identity_module_v01.h"

/*---------------------------------------------------------------------------
  Service related common defines
---------------------------------------------------------------------------*/
#define UIMQMI_SVC_INVALID_INSTANCE    (-1)
#define UIMQMI_SVC_MAX_CLIDS           (QMI_FRAMEWORK_SVC_MAX_CLIENTS - 1)


/*=============================================================================

                           STRUCTURE DECLARATIONS

=============================================================================*/

/*---------------------------------------------------------------------------
  QMI Framework details for QMI UIM or CAT message
---------------------------------------------------------------------------*/
typedef struct
{
  qmi_framework_msg_hdr_type   cmd_hdr;
  dsm_item_type              * sdu;
} uimqmi_framework_cmd_type;

/*---------------------------------------------------------------------------
  QMI UIM or CAT command check function pointer
---------------------------------------------------------------------------*/
typedef boolean (* uimqmi_command_check_ftype)
(
  uint16 cmd_type
);

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*===========================================================================
  FUNCTION UIMQMI_MALLOC

  DESCRIPTION
    The uimqmi_malloc calls modem_mem_calloc to request memory from the Modem Heap.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void * uimqmi_malloc
(
  uint32 size
);

/*===========================================================================
  FUNCTION UIMQMI_FREE

  DESCRIPTION
    The uimqmi_free frees the pointer from the Modem Heap.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void uimqmi_free
(
  void * ptr
);

/*===========================================================================
  FUNCTION UIMQMI_DEEP_COPY()

  DESCRIPTION
    This function allocates memory and duplicates the content of a buffer.
    This is a utility function to make deep copies.

  PARAMETERS
    dest : pointer to the destination buffer. Output value.
    src  : pointer to the source buffer
    size : size of the source buffer

  RETURN VALUE
    None. Errors are ignored.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void uimqmi_deep_copy
(
  void       ** dest,
  const void  * src,
  uint32        size
);

/*===========================================================================
  FUNCTION UIMQMI_MMGSDI_SLOT_TO_SLOT_INDEX()

  DESCRIPTION
    Determines the slot index from the MMGSDI slot id

  PARAMETERS
    mmgsdi_slot_id      : MMGSDI slot id
    slot_index_ptr      : output parameter containing slot index

  RETURN VALUE
    QMI client-specific error message

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 uimqmi_mmgsdi_slot_to_slot_index
(
  mmgsdi_slot_id_enum_type       mmgsdi_slot_id,
  uint8                        * slot_index_ptr
);

/*===========================================================================
  FUNCTION UIMQMI_SEND_INDICATION()

  DESCRIPTION
    This function calls the QMI Framework API to send out the UIM or CAT
    service indication to client.

  PARAMETERS
    client_id   : client ID
    cmd_type    : type of indication
    ind_ptr     : dsm item
    service_type: service type

  RETURN VALUE
    TRUE  - For success
    FALSE - For qmi svc failure

  DEPENDENCIES
    QMI UIM or CAT service must be initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
boolean uimqmi_send_indication
(
  uint8                        client_id,
  uint16                       cmd_type,
  dsm_item_type              * ind_ptr,
  qmux_service_e_type          service_type
);

/*===========================================================================
  FUNCTION  UIMQMI_PROCESS_FRAMEWORK_CMD()

  DESCRIPTION
    Generic QMI processing for QMI UIM/CAT commands

  PARAMETERS
    framework_cmd_ptr       : Pointer to the framework command details
    svc_sp                  : QMI UIM or CAT service state pointer
    cl_sp                   : Client state pointer
    qmi_callback_table_ptr  : Pointer to the service's callback table
    qmi_callback_table_size : Number of entries in the callback table
    is_command_disabled_fptr: Function pointer to check if command is disabled

  RETURN VALUE
    None

  DEPENDENCIES
    QMI UIM/CAT must already have been initialized and registered with Framework

  SIDE EFFECTS
    None
===========================================================================*/
void uimqmi_process_framework_cmd
(
  uimqmi_framework_cmd_type        * framework_cmd_ptr,
  qmi_common_svc_state_type        * svc_sp,
  qmi_common_client_state_type     * cl_sp,
  qmi_svc_cmd_hdlr_type            * qmi_callback_table_ptr,
  uint16                             qmi_callback_table_size,
  uimqmi_command_check_ftype         is_command_disabled_fptr
);

/*===========================================================================
  FUNCTION UIMQMI_PUT_RESULT_SEND_RESPONSE_IND

  DESCRIPTION
    This function copies the result TLV and send response or indication to
    the client.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void uimqmi_put_result_send_response_ind
(
  qmi_cmd_buf_type *       cmd_buf_p,
  dsm_item_type *          response_p,
  qmi_error_e_type         errval,
  qmux_service_e_type      service_type,
  boolean                  ind_required,
  uint8                    clid,
  uint16                   request_id
);

/*===========================================================================
  FUNCTION UIMQMI_CONVERT_MMGSDI_SLOT_TO_EFS_CONTEXT()

  DESCRIPTION
    Determines the uim common EFS context from mmgsdi slot id

  PARAMETERS
    mmgsdi_slot_id      : MMGSDI slot id

  RETURN VALUE
    uim_common_efs_context_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uim_common_efs_context_type uimqmi_convert_mmgsdi_slot_to_efs_context
(
  mmgsdi_slot_id_enum_type                   mmgsdi_slot_id
);

#endif /* QMI_UIM_CAT_COMMON_H */
