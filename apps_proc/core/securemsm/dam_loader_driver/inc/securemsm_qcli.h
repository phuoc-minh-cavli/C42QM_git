/***********************************************************
 Copyright (c) 2018 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
************************************************************/

#ifndef _SECUREMSM_QCLI_H_
#define _SECUREMSM_QCLI_H_

/** picked QCLI_Parameter_t from qcli_api.h and renamed for param passing */
typedef struct securemsm_QCLI_Parameter_s
{
    UCHAR   *String_Value;
    INT     Integer_Value;
    UINT    Integer_Is_Valid;
} securemsm_QCLI_Parameter_t;

#endif // _SECUREMSM_QCLI_H_
