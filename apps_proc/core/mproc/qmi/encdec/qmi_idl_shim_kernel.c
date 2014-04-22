/*===========================================================================

                    Q M I _ I D L _ S H I M _ K E R N E L . C
  
  Shim layer that provides QMI IDL apis to DAM user space

  ---------------------------------------------------------------------------
  Copyright (c) 2017, 2020 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/mproc/qmi/encdec/qmi_idl_shim_kernel.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

04/03/17   ab      File created.

=============================================================================*/
/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "qapi_txm_base.h"
#include "qurt_txm_qapi_fwk.h"
#include "qmi_txm_shim.h"
#include "qmi_idl_lib.h"
#include "qmi_idl_shim.h"

/*===========================================================================
                      MACROS AND DEFINES
===========================================================================*/

/*===========================================================================
                      GLOBAL VARIABLES
===========================================================================*/

/*===========================================================================
                         FUNCTIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  qmi_idl_shim_init
  DESCRIPTION: Handler function for handling qmi idl function requests
  from user space tx dam applications
===========================================================================*/
UINT qmi_idl_shim_kernel_handler
(
   UINT request_ID, UINT a, UINT b, UINT c, 
   UINT d, UINT e, UINT f, UINT g, UINT h, 
   UINT i, UINT j, UINT k, UINT l
)
{
  INT ret;
  
  switch(request_ID)
  {
    case QMI_SHIM_IDL_GET_SERVICE_ID:
        ret = (int)qmi_idl_get_service_id((qmi_idl_service_object_type )a, (uint32_t *)b);
    break;
    
    case QMI_SHIM_IDL_GET_IDL_MINOR_VERSION:
        ret = qmi_idl_get_idl_minor_version((qmi_idl_service_object_type )a, (uint32_t *)b);
    break;
        
    case QMI_SHIM_IDL_GET_IDL_VERSION:
        ret = (int)qmi_idl_get_idl_version((qmi_idl_service_object_type )a, (uint32_t *)b);    
    break;
    
    case QMI_SHIM_IDL_GET_MAX_SERVICE_LEN:
        ret = (int)qmi_idl_get_max_service_len((qmi_idl_service_object_type )a, (uint32_t *)b);
    break;
        
    case QMI_SHIM_IDL_GET_MAX_MESSAGE_LEN:
        ret = (int)qmi_idl_get_max_message_len((qmi_idl_service_object_type )a, (qmi_idl_type_of_message_type)b, (uint16_t)c, (uint32_t *)d);
    break;

    case QMI_SHIM_IDL_GET_MESSAGE_C_STRUCT_LEN:
        ret = (int)qmi_idl_get_message_c_struct_len((qmi_idl_service_object_type )a, (qmi_idl_type_of_message_type)b, (uint16_t)c, (uint32_t *)d);
    break;

    case QMI_SHIM_IDL_GET_MAX_C_STRUCT_LEN:
        ret = (int)qmi_idl_get_max_c_struct_len((qmi_idl_service_object_type )a, (uint32_t *)b);
    break;
    
    case QMI_SHIM_IDL_MESSAGE_DECODE:
        ret = (int)qmi_idl_message_decode((qmi_idl_service_object_type )a, (qmi_idl_type_of_message_type)b, (uint16_t)c, (void *)d, (uint32_t)e, (void *)f, (uint32_t)g);
    break;
    
    case QMI_SHIM_IDL_MESSAGE_ENCODE:
        ret = (int)qmi_idl_message_encode((qmi_idl_service_object_type)a, (qmi_idl_type_of_message_type)b, (uint16_t)c, (void *)d, 
                                          (uint32_t)e, (void *)f, (uint32_t)g, (uint32_t *)h);
    break;
    
    case QMI_SHIM_IDL_ENCODE_RESP_TLV:
        ret = (int)qmi_idl_encode_resp_tlv((uint16_t)a, (uint16_t)b, (void *)c, (uint32_t)d);
    break;
    
    case QMI_SHIM_IDL_GET_STD_RESP_TLV_LEN:
        ret = (int)qmi_idl_get_std_resp_tlv_len();
    break;
    
    case QMI_SHIM_IDL_INHERIT_SERVICE_OBJECT:
        ret = (int)qmi_idl_inherit_service_object((qmi_idl_service_object_type)a, (qmi_idl_service_object_type)b);
    break;
    
    case QMI_SHIM_IDL_GET_INHERITED_SERVICE_OBJECT:
        ret = (int)qmi_idl_get_inherited_service_object((qmi_idl_service_object_type)a);
    break;
    
    default:
        ret = 1;
    break;
  }
  
  return ret;
} /* qmi_idl_shim_kernel_handler */

/*===========================================================================
  FUNCTION  qmi_idl_shim_init
  DESCRIPTION: qmi idl shim init function
===========================================================================*/
void qmi_idl_shim_init(void)
{
  qurt_qapi_register(QMI_SHIM_IDL_BASE, QMI_SHIM_IDL_NUM_IDS, qmi_idl_shim_kernel_handler);
} /* qmi_client_shim_init */

