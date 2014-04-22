/*
@file secdspdemo.c
*/
/*===========================================================================
  Copyright (c) 2018 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/apps.tz/2.1.3/securemsm/trustzone/qsapps/secdspdemo/secdspdemo.c#2 $
  $DateTime: 2021/01/12 23:23:09 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------

===========================================================================*/
#include "secdspdemo.h"
#include "dsc_api.h"
#include "ipprotector_api.h"
#include "qsee_core.h"
#include "qsee_services.h"
#include "secdspdemo.h"
#include "stdbool.h"
#include "utils.h"

#define VERSION_MAJOR 1
#define VERSION_MINOR 0

static uint8_t qsee_log_mask;
static dsc_feat_handle_t *g_dsc_handle = NULL;

static buffer_vmid_perms_t g_buf_vmid_perms[] = {
    {MODEL, {{IAccessControl_VM_CP_CDSP, IAccessControl_PERM_TYPE_RW}}, 1},
    {AUTH, {{IAccessControl_VM_CP_CDSP, IAccessControl_PERM_TYPE_RW}}, 1},
    {CAMERA,
     {{IAccessControl_VM_CP_CDSP, IAccessControl_PERM_TYPE_RW},
      {IAccessControl_VM_CP_CAMERA, IAccessControl_PERM_TYPE_RW}},
     2},
};

static buffer_vmids_t cmd_buffer_vmids[] = {
    {DECRYPT_IP, {AC_VM_HLOS, AC_VM_CP_CDSP}},
    {VERIFY_IP, {AC_VM_CP_CDSP, AC_VM_NONE}},
    {GET_PUBLIC_KEY, {AC_VM_NONE, AC_VM_HLOS}},
};

static uint32_t nr_buf_vmid_perms = NUM_ELEMS(g_buf_vmid_perms);

/*
 * Description: Get the expected VMIDs for the input and output
 *              buffers based on the provided command ID
 *
 * In:          cmd_id: ID of command being performed by TA
 * Out:         vmids_out: List of expected VMIDs corresponding
 *              to input and output buffers
 * Return:      SUCCESS on success.
 *              Any other error code on failure.
 */
static int32_t __get_buffer_vmids(uint32_t cmd_id, uint32_t **vmids_out)
{
    int32_t ret = SUCCESS;
    size_t i = 0;

    *vmids_out = NULL;
    for (i = 0; i < NUM_ELEMS(cmd_buffer_vmids); i++) {
        if (cmd_buffer_vmids[i].cmd_id == cmd_id) {
            *vmids_out = cmd_buffer_vmids[i].vmids;
            break;
        }
    }

    CHECK_COND((*vmids_out != NULL), ret, FAILED,
               "Cannot find VM list for cmd_id : %d", cmd_id);

end:
    return ret;
}

/*
 * Description: Verify that the provided buffer is assigned
 *              to the provided VM
 *
 * In:          ptr: Address of buffer
 *              size: Length of buffer
 *              vmid: Expected VMID of buffer
 * Out:         void
 * Return:      SUCCESS on success.
 *              Any other error code on failure.
 */
static int32_t __buffer_vmid_check(uint8_t *ptr, uint32_t size, uint32_t vmid)
{
    int32_t ret = SUCCESS;
    bool success = true;

    if (vmid == AC_VM_NONE) {
        goto end;
    }

    if (vmid == AC_VM_HLOS) {
        success = qsee_is_ns_range(ptr, size);
        CHECK_COND(success, ret, FAILED,
                   "qsee_is_ns_range : addr not in ns range");
    } else {
        success =
            qsee_is_s_tag_area(vmid, (uint64_t)ptr, (uint64_t)(ptr + size));
        CHECK_COND(success, ret, FAILED,
                   "qsee_is_s_tag_area : addr doesn't belong to vmid : %d",
                   vmid);
    }

end:
    return ret;
}

/*
 * Description: Map buffer as readable by TA
 *
 * In:          ptr: Address of buffer
 *              size: Length of buffer
 * Out:         void
 * Return:      SUCCESS on success.
 *              Any other error code on failure.
 */
static int32_t __ta_register_buffer(uint8_t *ptr, uint32_t size)
{
    int32_t ret = SUCCESS;

    if (ptr == NULL) {
        goto end;
    }

    CHECK_N_CALL(ret, qsee_register_shared_buffer(ptr, size));

    CHECK_N_CALL(ret, qsee_prepare_shared_buf_for_secure_read(ptr, size));

end:
    return ret;
}

/*
 * Description: Unmap buffer from TA
 *
 * In:          ptr: Address of buffer
 *              size: Length of buffer
 * Out:         void
 * Return:      SUCCESS on success.
 *              Any other error code on failure.
 */
static int32_t __ta_deregister_buffer(uint8_t *ptr, uint32_t size)
{
    int32_t ret = SUCCESS;

    if (ptr == NULL) {
        goto end;
    }

    CHECK_N_CALL(ret, qsee_prepare_shared_buf_for_nosecure_read(ptr, size));

    CHECK_N_CALL(ret, qsee_deregister_shared_buffer(ptr));

end:
    return ret;
}

/*
 * Description: Map or unmap buffers as readable by TA and ensure
 *              they belong to correct VMs if mapping
 *
 * In:          cmd_ptr: Struct containing buffer and command info
 *                       provided to TA
 *              map: Determines whether to map or unmap buffers
 *                   for reading by TA
 * Out:         void
 * Return:      SUCCESS on success.
 *              Any other error code on failure.
 */
static int32_t __buffer_mem_ops(send_cmd_t *cmd_ptr, bool map)
{
    int32_t ret = SUCCESS;
    uint8_t *ptr = (uint8_t *)cmd_ptr->addr;
    uint8_t *out_ptr = (uint8_t *)cmd_ptr->out_addr;
    uint32_t size = cmd_ptr->size;
    uint32_t out_size = cmd_ptr->out_size;
    uint32_t *vmids = NULL;

    if (map) {
        CHECK_N_CALL(ret, __get_buffer_vmids(cmd_ptr->cmd_id, &vmids));
        CHECK_N_CALL(ret, __buffer_vmid_check(ptr, size, vmids[0]));
        CHECK_N_CALL(ret, __buffer_vmid_check(out_ptr, out_size, vmids[1]));

        CHECK_N_CALL(ret, __ta_register_buffer(ptr, size));
        CHECK_N_CALL(ret, __ta_register_buffer(out_ptr, out_size));
    } else {
        CHECK_N_CALL(ret, __ta_deregister_buffer(ptr, size));
        CHECK_N_CALL(ret, __ta_deregister_buffer(out_ptr, out_size));
    }

end:
    return ret;
}

/*
 * Description: Register the provided buffer with the DSC
 *              Shared Channel
 *
 * In:          addr: Address of buffer
 *              size: Size of buffer
 *              buf_type: Type of buffer
 * Out:         void
 * Return:      SUCCESS on success.
 *              Any other error code on failure.
 */
static int32_t __dsc_register_buffer(uint64_t addr, uint32_t size,
                                     uint32_t buf_type)
{
    int32_t ret = SUCCESS;
    uint32_t i = 0;
    IAccessControl_vmidPermission *vmid_perms = NULL;
    uint32_t num_vmid_perms = 0;

    for (i = 0; i < nr_buf_vmid_perms; i++) {
        if (buf_type == g_buf_vmid_perms[i].buf_type) {
            vmid_perms = g_buf_vmid_perms[i].vmid_perms;
            num_vmid_perms = g_buf_vmid_perms[i].num_vmid_perms;
            break;
        }
    }

    CHECK_COND(vmid_perms != NULL, ret, FAILED,
               "Cannot find VM List for buf_type : %d", buf_type);

    CHECK_N_CALL(ret, dsc_register_buffer(g_dsc_handle, buf_type, addr, size,
                                          vmid_perms, num_vmid_perms));

end:
    return ret;
}

/*
 * Description: Print the IP Protector public key
 *
 * In:          public_key: Address of the public key
 *              public_key_size: Length of the public key
 * Out:         void
 * Return:      SUCCESS on success.
 *              Any other error code on failure.
 */
static int32_t __print_pub_key(uint8_t *public_key, uint32_t public_key_size)
{
    int32_t ret = SUCCESS;
    size_t key_index = 0;

    LOG_MSG("Public Key: 0x%02x", public_key[key_index]);
    for (key_index = 1; key_index < public_key_size; key_index++) {
        LOG_MSG("            0x%02x", public_key[key_index]);
    }

    return ret;
}

/*
 * Description: Verify DSC current state and ensure buffer is
 *              registered with DSC
 *
 * In:          cmd_ptr: Struct containing buffer and command info
 *                       provided to TA
 *              exp_state: Expected current state of DSC
 *              addr: Address of buffer to verify with DSC
 *              size: Size of buffer to verify with DSC
 * Out:         void
 * Return:      SUCCESS on success.
 *              Any other error code on failure.
 */
static int32_t __ipp_setup(send_cmd_t *cmd_ptr, uint32_t exp_state,
                           uint64_t addr, uint32_t size)
{
    int32_t ret = SUCCESS;
    uint32_t state;

    CHECK_N_CALL(ret, dsc_get_state(g_dsc_handle, &state));
    CHECK_COND(state == exp_state, ret, FAILED,
               "DSP Shared Channel is in wrong state : %d", state);

    CHECK_N_CALL(ret, dsc_verify_buffer(g_dsc_handle, MODEL, addr, size));

end:
    return ret;
}

/*
 * Description: Perform the specified IP Protector operation
 *
 * In:          cmd_ptr: Struct containing buffer and command
 *                       info provided to TA
 * Out:         void
 * Return:      SUCCESS on success.
 *              Any other error code on failure.
 */
static int32_t __ipp_ops(send_cmd_t *cmd_ptr)
{
    int32_t ret = SUCCESS;
    bool map = true;

    CHECK_N_CALL(ret, __buffer_mem_ops(cmd_ptr, map));

    switch (cmd_ptr->cmd_id) {
        case DECRYPT_IP:
            CHECK_N_CALL(ret, __ipp_setup(cmd_ptr, INIT, cmd_ptr->out_addr,
                                          cmd_ptr->out_size));

            CHECK_N_CALL(ret, ipprotector_decrypt(
                                  cmd_ptr->feat_id, (uint8_t *)cmd_ptr->addr,
                                  cmd_ptr->size, (uint8_t *)cmd_ptr->out_addr,
                                  cmd_ptr->out_size));
            break;
        case VERIFY_IP:
            CHECK_N_CALL(
                ret, __ipp_setup(cmd_ptr, INIT, cmd_ptr->addr, cmd_ptr->size));

            CHECK_N_CALL(ret,
                         ipprotector_verify_sig(
                             cmd_ptr->feat_id, cmd_ptr->anti_rollback_version,
                             (uint8_t *)cmd_ptr->addr, cmd_ptr->size));

            CHECK_N_CALL(ret, dsc_set_state(g_dsc_handle, LOADED));
            break;
        case GET_PUBLIC_KEY:
            CHECK_N_CALL(
                ret, ipprotector_get_public_key(cmd_ptr->feat_id,
                                                (uint8_t *)cmd_ptr->out_addr,
                                                cmd_ptr->out_size));

            CHECK_N_CALL(ret, __print_pub_key((uint8_t *)cmd_ptr->out_addr,
                                              cmd_ptr->out_size));
            break;
    }

end:
    (void)__buffer_mem_ops(cmd_ptr, !map);
    return ret;
}

/*
 * Description: Release the DSC handle
 *
 * In:          void
 * Out:         void
 * Return:      void
 */
void release_dsc(void) {
    dsc_release_feat_handle(g_dsc_handle);
    g_dsc_handle = NULL;
}

/*
 * Description: Initialize secdspdemo TA
 *
 * In:          void
 * Out:         void
 * Return:      void
 */
void tz_app_init(void)
{
    int32_t error = SUCCESS;
    uint32_t state;

    qsee_log_mask = qsee_log_get_mask();
    qsee_log_set_mask(qsee_log_mask | QSEE_LOG_MSG_DEBUG);
    LOG_MSG("Init");
    LOG_MSG("Version: %x.%x", VERSION_MAJOR, VERSION_MINOR);

    CHECK_N_CALL(error, dsc_get_feat_handle(&g_dsc_handle));

    CHECK_N_CALL(error, dsc_get_state(g_dsc_handle, &state));

    CHECK_COND(state == RESET, error, FAILED,
               "DSP Shared Channel is in wrong state : %d", state);

    CHECK_N_CALL(error, dsc_set_state(g_dsc_handle, INIT));

end:
    if (error) {
        release_dsc();
    }
}

/*
 * Description: Execute secdsp TA for given command
 *
 * In:          cmd: Inputs to TA
 *              cmdlen: Length of structure containing inputs to TA
 *              rsplen: Length of structure containing TA return code
 * Out:         rsp: TA return code
 * Return:      SUCCESS on success.
 *              Any other error code on failure.
 */
void tz_app_cmd_handler(void *cmd, uint32_t cmdlen, void *rsp, uint32_t rsplen)
{
    int32_t ret = SUCCESS;
    send_cmd_t *cmd_in = NULL;
    send_cmd_rsp_t *rsp_ptr = NULL;

    if (rsplen < sizeof(send_cmd_rsp_t)) {
        LOG_MSG("Response buffer len insufficient");
        return;
    }

    CHECK_COND(cmd && rsp, ret, FAILED, "Invalid cmd/rsp parameters");

    CHECK_COND(cmdlen >= sizeof(send_cmd_t), ret, FAILED,
               "Command buffer len insufficient");

    rsp_ptr = (send_cmd_rsp_t *)rsp;
    cmd_in = (send_cmd_t *)cmd;

    CHECK_COND(g_dsc_handle != NULL, ret, FAILED,
               "DSC Handle initialization failed");

    LOG_MSG("cmd_id = %d", cmd_in->cmd_id);
    LOG_MSG("addr = 0x%x, size = 0x%x", cmd_in->addr, cmd_in->size);
    LOG_MSG("buf_type = %d", cmd_in->buf_type);
    LOG_MSG("out_addr = 0x%x, out_size = 0x%x", cmd_in->out_addr,
            cmd_in->out_size);
    LOG_MSG("feat_id = %d, anti_rollback_version = %d", cmd_in->feat_id,
            cmd_in->anti_rollback_version);

    switch (cmd_in->cmd_id) {
        case REGISTER_BUFFER:
            CHECK_N_CALL(ret, __dsc_register_buffer(cmd_in->addr, cmd_in->size,
                                                    cmd_in->buf_type));
            break;
        case DEREGISTER_BUFFER:
            CHECK_N_CALL(ret,
                         dsc_deregister_buffer(g_dsc_handle, cmd_in->buf_type,
                                               cmd_in->addr, cmd_in->size));
            break;
        case VERIFY_BUFFER:
            CHECK_N_CALL(ret, dsc_verify_buffer(g_dsc_handle, cmd_in->buf_type,
                                                cmd_in->addr, cmd_in->size));
            break;
        case DECRYPT_IP:
        case VERIFY_IP:
        case GET_PUBLIC_KEY:
            CHECK_N_CALL(ret, __ipp_ops(cmd_in));
            break;
        default:
            ERROR("Unsupported Command ID", ret, FAILED);
    }

end:
    if (rsp_ptr) {
        rsp_ptr->status = ret;
    }
}

/*
 * Description: Shutsdown secdspdemo TA
 *
 * In:          void
 * Out:         void
 * Return:      void
 */
void tz_app_shutdown(void)
{
    release_dsc();
    LOG_MSG("Exiting Secure DSP Demo App!");
    qsee_log_set_mask(qsee_log_mask);
    return;
}
