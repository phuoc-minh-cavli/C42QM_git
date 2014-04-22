/*
@file dsc_api.h
@brief DSC -> DSP Shared Channel
*/
/*===========================================================================
  Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/ssg.tz/1.1.3/securemsm/trustzone/qsapps/libs/secdsplib/dsc_api.h#1 $
  $DateTime: 2019/05/23 05:32:51 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
===========================================================================*/

#ifndef __DSC_API_H__
#define __DSC_API_H__

#include "qsee_access_control.h"
#include "stdint.h"

typedef enum dsc_errors {
    // Success return code
    DSC_SUCCESS = 0,
    // General validation error
    DSC_ERROR = 1000,
    // Ran out of memory
    DSC_OOM_ERROR,
    // Access Control error
    DSC_AC_ERROR,
    // Channel has exceeded maximum number of supported buffers
    DSC_OOB_ERROR,
    // Failed to register or de-register buffer
    DSC_BUFFER_ERROR,
    // Channel was not reset properly
    DSC_UNKNOWN_ERROR,
    // Failed to initialize Channel
    DSC_CHANNEL_ERROR,
    // Reading from Channel failed
    DSC_READ_ERROR,
    // Writing to Channel failed
    DSC_WRITE_ERROR,
    // Channel was updated by DSP while TZ was operating on it
    DSC_UNEXPECTED_CONFIG_ERROR,
} dsc_errors;

// Encapsulated structure serving as a handle
typedef struct dsc_feat_priv dsc_feat_handle_t;

/*
 * Description: Initialize DSC and get DSC handle
 *
 * In:          void
 * Out:         out_handle: DSC handle
 * Return:      DSC_SUCCESS on success.
 *              Any other error code on failure.
 */
int32_t dsc_get_feat_handle(dsc_feat_handle_t **out_handle);

/*
 * Description: Free DSC handle, unlock all buffers registered
 *              to DSC, and have DSC wiped by QSEE kernel
 *
 * In:          handle: DSC handle
 * Out:         void
 * Return:      void
 */
void dsc_release_feat_handle(dsc_feat_handle_t *handle);

/*
 * Description: Register buffer to DSC
 *
 * In:          handle: DSC handle
 *              buf_type: The type of the buffer
 *              addr: The address of the buffer
 *              size: The length of the buffer
 *              vmid_perms: The Access Control permissions to enforce
 *                          on the buffer
 *              num_vmid_perms: The number of Access Control permissions
 *                              to enforce
 * Out:         void
 * Return:      DSC_SUCCESS on success.
 *              Any other error code on failure.
 */
int32_t dsc_register_buffer(dsc_feat_handle_t *handle, uint32_t buf_type,
                            uint64_t addr, size_t size,
                            IAccessControl_vmidPermission *vmid_perms,
                            uint32_t num_vmid_perms);

/*
 * Description: De-register buffer from DSC
 *
 * In:          handle: DSC handle
 *              buf_type: The type of the buffer
 *              addr: The address of the buffer
 *              size: The length of the buffer
 * Out:         void
 * Return:      DSC_SUCCESS on success.
 *              Any other error code on failure.
 */
int32_t dsc_deregister_buffer(dsc_feat_handle_t *handle, uint32_t buf_type,
                              uint64_t addr, size_t size);

/*
 * Description: Verify buffer is registered with DSC
 *
 * In:          handle: DSC handle
 *              buf_type: The type of the buffer
 *              addr: The address of the buffer
 *              size: The length of the buffer
 * Out:         void
 * Return:      DSC_SUCCESS on success.
 *              Any other error code on failure.
 */
int32_t dsc_verify_buffer(dsc_feat_handle_t *handle, uint32_t buf_type,
                          uint64_t addr, size_t size);

/*
 * Description: Get current state of DSC
 *
 * In:          handle: DSC handle
 * Out:         cur_state: Current state of DSC
 * Return:      DSC_SUCCESS on success.
 *              Any other error code on failure.
 */
int32_t dsc_get_state(dsc_feat_handle_t *handle, uint32_t *cur_state);

/*
 * Description: Set current state of DSC
 *
 * In:          handle: DSC handle
 *              new_state: State to set for DSC
 * Out:         void
 * Return:      DSC_SUCCESS on success.
 *              Any other error code on failure.
 */
int32_t dsc_set_state(dsc_feat_handle_t *handle, uint32_t new_state);

#endif  // __DSC_API_H__
