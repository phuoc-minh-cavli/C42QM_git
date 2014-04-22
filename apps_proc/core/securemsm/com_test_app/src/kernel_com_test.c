#include <stdint.h>
#include <stdio.h>
#include "qurt_txm_qapi_fwk.h"
#include "qurt_memory.h"
#include "qurt_error.h"

// QSEECom logging
#include "qseecom_log.h"

// For QTEE<->TZ SMCInvoke communication
#include "qapi_QTEEInvoke.h"
#include "qapi_IOpener.h"
#include "CComTestApp.h"
#include "IComTestApp.h"

// For app loading
#include "fs_public.h"
#include "CAppLoader.h"
#include "IAppLoader.h"
#include "IAppController.h"

#define pr_emerg    LOGD
#define pr_alert    LOGD
#define pr_crit     LOGD
#define pr_err      LOGE
#define pr_warn     LOGW
#define pr_debug    LOGD
#define pr_notice   LOGI
#define pr_info     LOGI

const char *COM_TEST_APP_TA = "/firmware/image/com_test_app.mbn";

static int test_simple_comtestapp(void);

INT smcinvoke_kernel_mode_test(void);

// debug flag to control com_test_app loading
INT errno = 0;


INT smcinvoke_kernel_mode_test(void)
{
  INT err = 0;

  err = test_simple_comtestapp();

  return err;
}


int32_t test_add_nums(Object self)
{
    int32_t err = Object_OK;
    add_nums_in in = {0};
    add_nums_out out = {0};
    size_t out_len = 0;

    LOGD("RUNNING test_add_nums...\n");

    in.num1 = 10;
    in.num2 = 5;
    err = IComTestApp_run_test_with_args(self, IComTestApp_TEST_CMD_ADD_NUMS,
            &in, sizeof(add_nums_in), &out, sizeof(add_nums_out), &out_len);

    if (Object_isERROR(err)) {
        LOGE("FAILED test_add_nums with %d", err);
        return Object_ERROR;
    }

    if (out.response != in.num1 + in.num2) {
        LOGE("FAILED test_add_nums - incorrect output");
        return Object_ERROR;
    }

    LOGD("PASSED test_add_nums");
    return Object_OK;
}

static int __getFileSize(const char *filename)
{
  int size    = 0;
  int ret     = 0;
  struct fs_stat stat_buf = {0};

  ret = efs_stat(filename, &stat_buf);
  if(ret) {
    LOGSE("Failed to open file %s: %s (%d)", filename, __func__, efs_errno);
    size = -1;
  }
  else{
    size = stat_buf.st_size;
  }
  return size;
}

int efs_Read_Full(int Fd, uint8_t *Buf, uint32_t Count,
    uint32_t *Bytes_Read_Ptr)
{
    uint32_t bytes_remaining = Count;
    uint32_t bytes_read = 0;

    *Bytes_Read_Ptr = 0;
    while (bytes_remaining > 0) {
      bytes_remaining -= bytes_read;

      bytes_read = 0;
      bytes_read = efs_read(Fd, Buf + *Bytes_Read_Ptr, bytes_remaining);

      *Bytes_Read_Ptr += bytes_read;
    }

    return ((bytes_remaining == 0) ? (0) : (-1));
}

static int __readFile(const char *filename, size_t size, uint8_t *buffer)
{
  int fd;
  size_t readBytes  = 0;
  int ret           = 0;

  do {
    fd = efs_open(filename, O_RDONLY);
    if(fd < 0) {
      LOGSE("Failed to open file %s: %s (%d)", filename, __func__, errno);
      ret = -1;
      break;
    }
    ret = efs_Read_Full(fd, buffer, size, &readBytes);
    if ((ret) || (readBytes != size)) {
      LOGSE("Error reading the file %s: %zu vs %zu bytes: %s (%d)",
        filename,
        readBytes,
        size,
        __func__,
        efs_errno);
      ret = -1;
      break;
    }
    ret = size;
  } while (0);
  
  if (fd >= 0) {
    efs_close(fd);
  }
  return ret;
}

static int loadApp(Object appLoader, const char *path, Object *appController)
{
  size_t size       = 0;
  uint8_t * buffer  = NULL;
  int ret           = 0;

  do {
    ret = __getFileSize(path);
    if (ret <= 0) {
      ret = -1;
      break;
    }
    size = (size_t)ret;

    buffer = malloc(size*sizeof(*buffer));
    if(buffer == NULL) {
        ret = -1;
        break;
    }

    ret = __readFile(path, size, buffer);
    if (ret < 0) break;

    ret = IAppLoader_loadFromBuffer(appLoader, buffer, size, appController);

  } while (0);

  free(buffer);

  if (ret) {
    LOGE("Load app %s failed: %d", "path.cstr()", ret);
  } else {
    LOGD("Load app %s succeeded", "path.cstr()");
  }

  return ret;
}

int32_t test_fs(Object self)
{
    int32_t err = Object_OK;

    LOGD("RUNNING test_fs...\n");

    err = IComTestApp_run_test(self, IComTestApp_TEST_CMD_RUN_FS_TEST);

    if (Object_isERROR(err)) {
        LOGE("FAILED test_fs with %d", err);
        return Object_ERROR;
    }

    LOGD("PASSED test_fs");
    return Object_OK;
}

int32_t test_sfs(Object self)
{
    int32_t err = Object_OK;

    LOGD("RUNNING test_sfs...\n");

    err = IComTestApp_run_test(self, IComTestApp_TEST_CMD_RUN_SFS_TEST);

    if (Object_isERROR(err)) {
        LOGE("FAILED test_sfs with %d", err);
        return Object_ERROR;
    }

    LOGD("PASSED test_sfs");
    return Object_OK;
}

static int test_simple_comtestapp()
{
    int err = Object_OK;
    Object openerObj = Object_NULL;
    Object appLoaderAppObj = Object_NULL;
    Object comTestAppObj = Object_NULL;
    Object appController  = Object_NULL;

    LOGD("RUNNING [test_simple_comtestapp]...\n");

    // Test that the openerObj returns successfully
    err = qapi_QTEEInvoke_GetOpener(&openerObj);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_simple_comtestapp] qapi_QTEEInvoke_GetOpener with %d", err);
        return err;
    }

    LOGE("CTD:: qapi_QTEEInvoke_GetOpener passed");

    // Test that the TA service opens correctly
    err = qapi_IOpener_open(openerObj, CAppLoader_UID, &appLoaderAppObj);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_simple_comtestapp] IOpener_open with %d", err);
        goto IOpener_release;
    }
    LOGE("CTD:: qapi_IOpener_open passed");

    err = loadApp(appLoaderAppObj, COM_TEST_APP_TA, &appController);
    if(err) {
        LOGE("FAILED [test_simple_comtestapp] loadApp with %d", err);
        goto IOpener_release;
    }
    LOGE("CTD:: loadApp passed");
    
    err = IAppController_getAppObject(appController, &comTestAppObj);
    if(err) {
        LOGE("FAILED [test_simple_comtestapp] IAppController_getAppObject with %d", err);
        goto IOpener_release;
    }
    LOGE("CTD:: IAppController_getAppObject passed");

    // Test that the TA service operates as expected
    err = test_add_nums(comTestAppObj);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_simple_comtestapp] test_add_nums failed with %d",
            err);
    }
    LOGE("CTD:: test_add_nums passed");

    // Test that the TA service operates as expected
    err = test_fs(comTestAppObj);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_simple_comtestapp] test_fs failed with %d",
            err);
    }
    LOGE("CTD:: test_fs passed");


    // Test that the TA service operates as expected
    err = test_sfs(comTestAppObj);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_simple_comtestapp] test_sfs failed with %d",
            err);
    }
    LOGE("CTD:: test_sfs passed");

IOpener_release:
    Object_ASSIGN_NULL(comTestAppObj);
    Object_ASSIGN_NULL(appController);
    Object_ASSIGN_NULL(appLoaderAppObj);
    Object_ASSIGN_NULL(openerObj);

    LOGD("CTD:: PASSED [test_simple_comtestapp]");
    return Object_OK;
}

INT g_qsee_log_test_cmd_id = -1;
static INT app_loaded = FALSE;
static INT unload_app = FALSE;

INT test_qsee_log_extraction(INT cmd_id)
{
    int32_t err = Object_OK;
    int32_t dummy_out = 0;
    size_t dummy_out_len = 0;
    static Object openerObj = {0};
    static Object appLoaderAppObj = {0};
    static Object comTestAppObj = {0};
    static Object appController  = {0};

    LOGD("RUNNING [test_qsee_log_extraction]...\n");

    if(!app_loaded) {
        // Test that the openerObj returns successfully
        err = qapi_QTEEInvoke_GetOpener(&openerObj);
        if (Object_isERROR(err)) {
            LOGE("FAILED [test_qsee_log_extraction] qapi_QTEEInvoke_GetOpener with %d", err);
            return err;
        }

        LOGE("CTD:: qapi_QTEEInvoke_GetOpener passed");

        // Test that the TA service opens correctly
        err = qapi_IOpener_open(openerObj, CAppLoader_UID, &appLoaderAppObj);
        if (Object_isERROR(err)) {
            LOGE("FAILED [test_qsee_log_extraction] IOpener_open with %d", err);
            goto IOpener_release;
        }
        LOGE("CTD:: qapi_IOpener_open passed");

        err = loadApp(appLoaderAppObj, COM_TEST_APP_TA, &appController);
        if(err) {
            LOGE("FAILED [test_qsee_log_extraction] loadApp with %d", err);
            goto IOpener_release;
        }
        LOGE("CTD:: loadApp passed");
        
        err = IAppController_getAppObject(appController, &comTestAppObj);
        if(err) {
            LOGE("FAILED [test_qsee_log_extraction] IAppController_getAppObject with %d", err);
            goto IOpener_release;
        }
        LOGE("CTD:: IAppController_getAppObject passed");

        app_loaded = TRUE;
    }

    switch(cmd_id)
    {
        case IComTestApp_TEST_CMD_RUN_QSEE_LOG_NO_WRAP_TEST:
        case IComTestApp_TEST_CMD_RUN_QSEE_LOG_WRAP_ONCE_TEST:
        case IComTestApp_TEST_CMD_RUN_QSEE_LOG_WRAP_MULT_TEST:
            g_qsee_log_test_cmd_id = cmd_id;
            unload_app = FALSE; // don't unload app
            break;
        case 0xFF:
        default:
            unload_app = TRUE;
            g_qsee_log_test_cmd_id = -1;    // Mark Invalid test cmd_id
            goto unload_app_only;
    }

    
    err = IComTestApp_run_test_with_args(comTestAppObj, cmd_id,
                &cmd_id, sizeof(cmd_id), &dummy_out, sizeof(dummy_out), &dummy_out_len);
    if (Object_isERROR(err)) {
        LOGE("FAILED test_add_nums with %d", err);
        return Object_ERROR;
    }

unload_app_only:
    if(!unload_app) {
        goto dont_unload_app;   // Return without unloading TA for now. as its generating extra QSEE logs.
    }

    // Test that TA service object and opener object release properly
    err |= IAppController_unload(appController);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_simple_comtestapp] IComTestApp_release with %d", err);
        return err;
    }
    LOGE("CTD:: IAppController_unload passed");

IOpener_release:
    err |= qapi_IOpener_release(openerObj);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_simple_comtestapp] IOpener_release with %d", err);
        return err;
    }

    app_loaded = FALSE; // app is unloaded successfully.

dont_unload_app:    
    LOGD("PASSED test_qsee_log_extraction");
    return Object_OK;
}

