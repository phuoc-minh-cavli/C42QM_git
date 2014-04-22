/*
@file secdspdemo.h
@brief This file contains all definitions used in secdspdemo
*/
/*===========================================================================
  Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/apps.tz/2.1.3/securemsm/trustzone/qsapps/secdspdemo/secdspdemo.h#1 $
  $DateTime: 2019/05/23 05:23:25 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
===========================================================================*/

#ifndef __SECDSPDEMO_H__
#define __SECDSPDEMO_H__

#include "CAccessControl.h"
#include "IAccessControl.h"
#include "qsee_access_control.h"
#include "stdint.h"

#define NUM_VMIDS 2
#define NUM_ELEMS(x) (sizeof(x) / sizeof((x)[0]))

typedef enum commands {
    REGISTER_BUFFER = 1,
    VERIFY_BUFFER,
    DEREGISTER_BUFFER,
    DECRYPT_IP,
    VERIFY_IP,
    GET_PUBLIC_KEY,
    NUM_CMDS = GET_PUBLIC_KEY,
} commands_t;

typedef enum buffer_type {
    MODEL = 1,
    AUTH = 2,
    CAMERA = 3,
} buf_type_t;

typedef enum state_type {
    RESET = 0,
    INIT,
    LOADED,
    IDLE,
    EXECUTING,
    TEMPLATE_PROCESS,
    PASS,
    ERROR,
    CLEANUP,
} state_type_t;

typedef struct send_cmd_struct {
    uint32_t cmd_id;
    uint64_t addr;
    uint32_t size;
    uint32_t buf_type;
    uint64_t out_addr;
    uint32_t out_size;
    uint32_t feat_id;
    uint32_t anti_rollback_version;
} __attribute__((packed)) send_cmd_t;

typedef struct send_cmd_rsp_struct {
    uint32_t status;
} __attribute__((packed)) send_cmd_rsp_t;

typedef struct buffer_vmid_perms {
    buf_type_t buf_type;
    IAccessControl_vmidPermission vmid_perms[NUM_VMIDS];
    uint32_t num_vmid_perms;
} buffer_vmid_perms_t;

typedef struct buffer_vmids {
    uint32_t cmd_id;
    uint32_t vmids[NUM_VMIDS];
} buffer_vmids_t;

#endif  // __SECDSPDEMO_H__
