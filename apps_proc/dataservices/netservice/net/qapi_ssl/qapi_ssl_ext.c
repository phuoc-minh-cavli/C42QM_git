/*
 * Copyright (c) 2012-2020 by QUALCOMM Technologies, Inc. All Rights Reserved.
 * All Rights Reserved.
 */
#include "qapi_ssl.h"
qapi_Net_SSL_Obj_Hdl_t qapi_Net_SSL_Obj_New(qapi_Net_SSL_Role_t role)
{
    return (0);
}

qapi_Status_t qapi_Net_SSL_Obj_Free(qapi_Net_SSL_Obj_Hdl_t hdl)
{
    return QAPI_ERROR;
}

qapi_Net_SSL_Con_Hdl_t qapi_Net_SSL_Con_New(qapi_Net_SSL_Obj_Hdl_t hdl, qapi_Net_SSL_Protocol_t prot)
{
    return (0);
}

qapi_Status_t qapi_Net_SSL_Fd_Set(qapi_Net_SSL_Con_Hdl_t ssl, uint32_t fd)
{
    return QAPI_ERROR;
}

qapi_Status_t qapi_Net_SSL_Accept(qapi_Net_SSL_Con_Hdl_t ssl)
{
    return QAPI_ERROR;
}

qapi_Status_t qapi_Net_SSL_Connect(qapi_Net_SSL_Con_Hdl_t ssl)
{
    return QAPI_ERROR;
}

qapi_Status_t qapi_Net_SSL_Shutdown(qapi_Net_SSL_Con_Hdl_t ssl)
{
    return QAPI_ERROR;
}

qapi_Status_t qapi_Net_SSL_Configure(qapi_Net_SSL_Con_Hdl_t ssl, qapi_Net_SSL_Config_t *cfg)
{
    return QAPI_ERROR;
}

qapi_Status_t qapi_Net_SSL_Read(qapi_Net_SSL_Con_Hdl_t ssl, void *buf, uint32_t num)
{
    return QAPI_ERROR;
}

qapi_Status_t qapi_Net_SSL_Write(qapi_Net_SSL_Con_Hdl_t ssl, void *buf, uint32_t num)
{
   return QAPI_ERROR;
}

qapi_Status_t qapi_Net_SSL_Cert_File_Exists(char *file_name, qapi_Net_SSL_Cert_Type_t type)
{
    return QAPI_ERROR;
}

qapi_Status_t qapi_Net_SSL_Set_Extended_Config_Option
(
  qapi_Net_SSL_Con_Hdl_t                 hdl , 
  qapi_Net_SSL_Extended_Config_Options_t option,
  void * val , uint32_t len
)
{
    return QAPI_ERROR;
}

boolean qapi_Net_SSL_Validate_Handle_Internal(uint32_t obj_hdl, uint32_t module_uid)
{
    return FALSE;
}



