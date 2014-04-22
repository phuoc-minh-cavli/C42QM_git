// Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
#include <stdint.h>
#include <stddef.h>
#include "qsee_log.h"
#include <qsee_timer.h>
#include <sp_iris.h>
#include "seccamlib.h"

#define MAX_CERT_SIZE (2048)

// Vendor command IDs
typedef enum {
    SECCAM_VENDOR_CMD_EXCHANGE_TIMESTAMP     = 2000,
    SECCAM_VENDOR_CMD_INIT_BIOMETRICS_BUFFER = 2001,
    SECCAM_VENDOR_CMD_RESET_HAVEN_LICENSE = 2002,
    SECCAM_VENDOR_CMD_SET_SENSORS = 2003,
    SECCAM_VENDOR_CMD_ERR_SIZE = 0x7FFFFFFF,
} seccam_vendor_cmd_id_t;

#pragma pack(push, seccam_vendor, 1)

typedef struct {
    seccam_vendor_cmd_id_t cmd_id;
    uint64_t cmd_data;
} seccam_vendor_cmd_req_t;

typedef struct {
    seccam_error_t ret;
    uint64_t ret_data;
} seccam_vendor_cmd_rsp_t;

typedef struct {
    seccam_vendor_cmd_id_t     cmd_id;
    uint64_t                   buffer_base_address;
    uint64_t                   buffer_length;
} seccam_vendor_cmd_init_biometrics_t;

typedef struct  {
    uint32_t cmd_id;                //command id
    uint32_t cert_size;             //cert size
    uint8_t  cert[MAX_CERT_SIZE];   //cert payload offset
} seccam_vendor_cmd_reset_haven_req_t;

typedef struct {
    seccam_error_t ret;    //returned status
} seccam_vendor_cmd_reset_haven_rsp_t;

typedef struct  {
    uint32_t cmd_id;        //command id
    uint32_t num_sensors;   //number of protected sensors
} seccam_vendor_cmd_set_sensors_req_t;

typedef struct {
    seccam_error_t ret;    //returned status
} seccam_vendor_cmd_set_sensors_rsp_t;

#pragma pack(pop, seccam_vendor)

////////////// SECURE MODE TRANSITION CALLBACKS ///////////////
seccam_error_t transition_to_secure_event(void* user_ctx)
{
    //setup anything which has to be initialized only AFTER secure camera session was activated
    //e.g: turning ON protected LED

    qsee_log(QSEE_LOG_MSG_HIGH, "transition_to_secure_event");

    return SECCAM_E_SUCCESS;
}

seccam_error_t transition_to_non_secure_event(void* user_ctx)
{
    //release everything requiring de-initialization BEFORE secure camera session will be completed.
    //e.g: turning OFF protected LED

    qsee_log(QSEE_LOG_MSG_HIGH, "transition_to_non_secure_event");

    return SECCAM_E_SUCCESS;
}

seccam_error_t on_error_event(void* user_ctx)
{
    qsee_log(QSEE_LOG_MSG_HIGH, "on_error_event");
    //TA should return SECCAM_E_FAIL, if it does not want seccamlib to release Camera HW
    return SECCAM_E_SUCCESS;
}
/////////////////// FRAME/PREVIEW CALLBACK ///////////////////
//This will be called for any secure camera streaming (dedicated, non-dedicated, preview cameras)

//frame event callback will be called once the secure camera(s) has generated a frame
//the buffers parameters provided to this callback are mapped and validated to have proper security permission
//this call back will only be called if secure camera is in valid secure mode and it is safe to manipulate the buffers
seccam_error_t frame_event(void* user_ctx, const seccam_frame_info_t* frame_info,
        uint8_t* camera_buffer, uint32_t cam_buf_size, const seccam_buffer_info_t* cam_buf_info,
        uint8_t* preview_buffer, uint32_t preview_buf_size, const seccam_buffer_info_t* preview_buf_info,
        const void* custom_req, uint32_t custom_req_size,
        void* custom_rsp, uint32_t custom_rsp_size)
{
    uint32_t cam_id = (uint32_t)-1;
    seccam_error_t status = SECCAM_E_SUCCESS;

    if (frame_info != NULL) {
        cam_id = frame_info->cam_id;
    }
    qsee_log(QSEE_LOG_MSG_HIGH, "frame_event: cam_id:%u, cbuf:%x, cbuf_size: %d, pbuf:%x, pbuf_size:%d",
        cam_id, camera_buffer, cam_buf_size, preview_buffer, preview_buf_size);

    //Authentication algorithm can be implemented here:
    // if (cam_id == MY_SECURE_CAMERA_ID) {
    //    authenticate(camera_buffer, cam_buf_size);
    // }

    //for secure preview we would like to show the buffer content on screen
    if (preview_buffer != NULL) {
        //copy the camera frame to the secure preview surface
        status = secCamCopyPreviewFrame(camera_buffer, cam_buf_size, cam_buf_info,
            preview_buffer, preview_buf_size, preview_buf_info);
    }

    //Set the custom return parameters to demonstrate the interface capabilities
    //the custom_rsp data will be passed back to the frame event caller in HLOS.
    //Make sure to not pass back any confidential data!
    if (custom_rsp != NULL && custom_rsp_size >= (sizeof(uint64_t)*3)) {
        uint64_t* params = (uint64_t*)custom_rsp;
        params[0] = cam_id;
        if (frame_info != NULL) {
            params[1] = frame_info->frame_number;
            params[2] = frame_info->time_stamp;
        }
    }

    return status;
}

static const seccam_callbacks_t g_callbacks = {
        .frame_event_cb =      frame_event,
        .transition_to_non_secure_cb = transition_to_non_secure_event,
        .transition_to_secure_cb = transition_to_secure_event,
        .error_cleanup_cb = on_error_event,
};

//=========================================================================
//  @brief
//    Add any app specific initialization code here
//    QSEE will call this function after secure app is loaded and
//    authenticated
//=========================================================================
void tz_app_init(void) 
{
    //uncomment below to enable all debug logs for this TA
    qsee_log_set_mask(qsee_log_get_mask() | QSEE_LOG_MSG_HIGH /* | QSEE_LOG_MSG_DEBUG */);

    qsee_log(QSEE_LOG_MSG_HIGH, "tz_app_init");

    //Valid Haven license for secure camera in DER format should be used in production
    const uint8_t haven_license[1024] = {0};

    seccam_error_t retval = secCamInit(NULL, &g_callbacks, sizeof(g_callbacks),
                                       haven_license, sizeof(haven_license));
    if (retval != SECCAM_E_SUCCESS) {
        qsee_log(QSEE_LOG_MSG_ERROR, "failed to initialize secure camera library %d", retval);
    }

    const uint32_t number_sensors = 1;
    retval = secCamSetNumSensors(number_sensors);
    if (retval != SECCAM_E_SUCCESS) {
        qsee_log(QSEE_LOG_MSG_ERROR, "failed to set expected number of secure sensors. defaulting to 1. %d", retval);
    }
}

//=========================================================================
//  @brief
//    App specific shutdown
//    App will be given a chance to shutdown gracefully
//=========================================================================
void tz_app_shutdown(void) 
{
    qsee_log(QSEE_LOG_MSG_HIGH, "tz_app_shutdown");
    seccam_error_t retval = secCamShutdown();
    if (retval != SECCAM_E_SUCCESS) {
        qsee_log(QSEE_LOG_MSG_ERROR, "failed to release secure camera library %d", retval);
    }
}

//=========================================================================
//  @brief
//    Vendor specific cmd to reset haven license.
//    Should not be called while secure camera streaming active.
//=========================================================================
static seccam_error_t reset_haven_license(const seccam_vendor_cmd_reset_haven_req_t* req, uint32_t req_size,
                                          const seccam_vendor_cmd_reset_haven_rsp_t* rsp, uint32_t rsp_size)
{
    seccam_error_t retval = SECCAM_E_SUCCESS;
    uint32_t license_size = 0;
    const uint8_t *license = NULL;

    if (sizeof(*req) > req_size || sizeof(*rsp) > rsp_size) {
        qsee_log(QSEE_LOG_MSG_ERROR, "Invalid Input Parameters");
        retval = SECCAM_E_INVALID_PARAM;
        goto exit;
    }

    //Attempt Shutdown - release CameraHW - Notify TA of teardown
    retval = secCamShutdown();
    if (retval != SECCAM_E_SUCCESS) {
        qsee_log(QSEE_LOG_MSG_ERROR, "secCamShutdown Failed. Cannot reset license on an active session. ret=%d", retval);
        goto exit;
    }

    //Prepare and validate custom request structure
    if (req->cert_size != 0) {
        license = req->cert;
        license_size = req->cert_size;
    } else {
        retval = SECCAM_E_INVALID_PARAM;
        goto exit;
    }

    retval = secCamInit(NULL, &g_callbacks, sizeof(g_callbacks), license, license_size);
    if (retval != SECCAM_E_SUCCESS) {
        qsee_log(QSEE_LOG_MSG_ERROR, "failed to initialize secure camera library %d", retval);
    }

exit:
    return retval;
}

//=========================================================================
//  @brief
//    Vendor specific cmd to set number of sensors to be protected
//=========================================================================
static seccam_error_t set_sensors(const seccam_vendor_cmd_set_sensors_req_t* req, uint32_t req_size,
                                  const seccam_vendor_cmd_set_sensors_rsp_t* rsp, uint32_t rsp_size)
{
    seccam_error_t retval = SECCAM_E_SUCCESS;

    if (sizeof(*req) > req_size || sizeof(*rsp) > rsp_size) {
        qsee_log(QSEE_LOG_MSG_ERROR, "Invalid Input Parameters");
        retval = SECCAM_E_INVALID_PARAM;
        goto exit;
    }

    //Prepare and validate custom request structure
    retval = secCamSetNumSensors(req->num_sensors);
    if (retval) {
        qsee_log(QSEE_LOG_MSG_ERROR, "Failed attempt to set expected number of secure_sensors=%d, ret=%d.", req->num_sensors, retval);
    }

exit:
    return retval;
}

//=========================================================================
//  @brief
//    App command handler
//    Parse and execute commands send from NS client
//=========================================================================
void tz_app_cmd_handler(void* cmd_ptr, uint32_t cmdlen, void* rsp_ptr, uint32_t rsplen) {

    //pass command to secure camera library first
    seccam_error_t seccam_err = secCamProcessCommand(cmd_ptr,cmdlen, rsp_ptr,rsplen);
    //check if the command was handled by the secure camera library

    //secCamProcessCommand expects non-NULL struct pointers with at least 4-byte size
    //if it receives invalid parameters will return SECCAM_E_INVALID_PARAM
    //if it receives command ID not handled by secure camera library - will return SECCAM_E_UNKNOWN_QSEE_COMMAND
    if (seccam_err == SECCAM_E_SUCCESS) {
        qsee_log(QSEE_LOG_MSG_DEBUG, "secCamProcessCommand: handled the command successfully");
        return;
    }

    // Handle vendor commands
    if (cmd_ptr == NULL || sizeof(seccam_vendor_cmd_req_t) > cmdlen) {
        qsee_log(QSEE_LOG_MSG_ERROR, "tz_app_cmd_handler: request structure is invalid");
        return;
    }
    seccam_vendor_cmd_req_t* req = (seccam_vendor_cmd_req_t*) cmd_ptr;

    if (rsp_ptr == NULL || sizeof(seccam_vendor_cmd_rsp_t) > rsplen) {
        qsee_log(QSEE_LOG_MSG_ERROR, "tz_app_cmd_handler: response structure is invalid");
        return;
    }
    seccam_vendor_cmd_rsp_t* rsp = (seccam_vendor_cmd_rsp_t*) rsp_ptr;

    switch(req->cmd_id) {
        // The timestamp exchange is an example of usage for the vendor command mechanism between HLOS and TZ
        case SECCAM_VENDOR_CMD_EXCHANGE_TIMESTAMP:
            qsee_log(QSEE_LOG_MSG_DEBUG, "tz_app_cmd_handler: HLOS timestamp received: %lu", req->cmd_data);
            rsp->ret_data = qsee_get_uptime();
            rsp->ret = SECCAM_E_SUCCESS;
            break;
        // This command is used to pass a pointer to ION buffer that will be shared between
        // Trustzone and SPU for the purpose of biometric apps such as iris and facial recognition
        case SECCAM_VENDOR_CMD_INIT_BIOMETRICS_BUFFER:
            if (sizeof(seccam_vendor_cmd_init_biometrics_t) > cmdlen) {
                qsee_log(QSEE_LOG_MSG_ERROR, "tz_app_cmd_handler: init_biometrics_buffer request structure is invalid");
                return;
            }
            seccam_vendor_cmd_init_biometrics_t* init_biom_cmd = (seccam_vendor_cmd_init_biometrics_t*)(req);

            qsee_log(QSEE_LOG_MSG_DEBUG, "tz_app_cmd_handler: biometrics buffer addr: %lx",
                     init_biom_cmd->buffer_base_address);
            qsee_log(QSEE_LOG_MSG_DEBUG, "tz_app_cmd_handler: biometrics buffer length: %d",
                     init_biom_cmd->buffer_length);

            sp_iris_result_e result;
            result = sp_iris_init(init_biom_cmd->buffer_base_address, init_biom_cmd->buffer_length);
            qsee_log(QSEE_LOG_MSG_HIGH, "tz_app_cmd_handler: init_biometrics_buffer return value: %d", result);
            rsp->ret = (result == SP_IRIS_SUCCESS ? SECCAM_E_SUCCESS : SECCAM_E_FAIL);
            break;
        case SECCAM_VENDOR_CMD_RESET_HAVEN_LICENSE:
            seccam_err = reset_haven_license((seccam_vendor_cmd_reset_haven_req_t*)req, cmdlen, 
                                             (seccam_vendor_cmd_reset_haven_rsp_t*)rsp, rsplen);
            qsee_log(QSEE_LOG_MSG_HIGH, "tz_app_cmd_handler: reset license return value: %d", seccam_err);
            rsp->ret = seccam_err;
            break;
        case SECCAM_VENDOR_CMD_SET_SENSORS:
            seccam_err = set_sensors((seccam_vendor_cmd_set_sensors_req_t*)req, cmdlen, (seccam_vendor_cmd_set_sensors_rsp_t*)rsp, rsplen);
            qsee_log(QSEE_LOG_MSG_HIGH, "tz_app_cmd_handler: set sensors return value: %d", seccam_err);
            rsp->ret = seccam_err;
            break;
        default: {
            qsee_log(QSEE_LOG_MSG_ERROR, "tz_app_cmd_handler invalid command id:%d", req->cmd_id);
            rsp->ret = SECCAM_E_FAIL;
            break;
        }
    }

}
