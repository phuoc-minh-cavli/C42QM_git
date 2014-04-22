/*
 * Copyright (c) 2012-2019 by QUALCOMM Technologies, Inc. All Rights Reserved.
 * All Rights Reserved.
 */

/* 
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
#include "qapi_ssl.h"
qapi_Status_t qapi_Net_SSL_Cert_Store(const char *name, qapi_Net_SSL_Cert_Type_t type, qapi_Net_SSL_Cert_t cert, uint32_t size)
{
    return QAPI_ERROR;
}

qapi_Status_t qapi_Net_SSL_Cert_Convert_And_Store(qapi_Net_SSL_Cert_Info_t *cert_info, const uint8_t *cert_name)
{
    return QAPI_ERROR;
}

qapi_Status_t qapi_Net_SSL_Cert_Load(qapi_Net_SSL_Obj_Hdl_t hdl, qapi_Net_SSL_Cert_Type_t type, const char *name)
{
    return QAPI_ERROR;
}

qapi_Status_t qapi_Net_SSL_Cert_Load_Get_Identifier(qapi_Net_SSL_Obj_Hdl_t hdl, qapi_Net_SSL_Identifier_t *identifier, const char *name)
{
    return QAPI_ERROR;
}

qapi_Status_t qapi_Net_SSL_Cert_List(qapi_Net_SSL_Cert_Type_t type, qapi_Net_SSL_Cert_List_t *list)
{
    return QAPI_ERROR;
}

qapi_Status_t qapi_Net_SSL_Cert_delete(char *name, qapi_Net_SSL_Cert_Type_t type)
{
    return QAPI_ERROR;
}

