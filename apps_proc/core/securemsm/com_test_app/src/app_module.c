#include "txm_module.h"
#include "qapi_QTEEInvoke.h"
#include "qapi_IOpener.h"
#include "CComTestApp.h"
#include "IComTestApp.h"
#include "qapi_diag.h"

#include "qapi_fs.h"
#include "qapi_fs_types.h"
#include "CAppLoader.h"
#include "IAppLoader.h"
#include "IAppController.h"

ULONG __stack_chk_guard __attribute__((weak)) = 0xDEADDEAD;

#define LOGD(fmt, ...) QAPI_MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH, fmt, ## __VA_ARGS__);
#define LOGE(fmt, ...) QAPI_MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, fmt, ## __VA_ARGS__);
#define LOGSD(fmt, ...) QAPI_MSG_SPRINTF( MSG_SSID_SEC_QWES, MSG_LEGACY_HIGH, fmt, __VA_ARGS__);
#define LOGSE(fmt, ...) QAPI_MSG_SPRINTF( MSG_SSID_SEC_QWES, MSG_LEGACY_ERROR, fmt, __VA_ARGS__);

/* Define constants.  */
#define STACK_SIZE         8 * 1024
#define BYTE_POOL_SIZE     (200 * 1024)
#define BLOCK_POOL_SIZE    100
#define QUEUE_SIZE         100

#define COM_TEST_APP_FILE  "/firmware/image/com_test_app.mbn"

/* Define the pool space in the bss section of the module. ULONG is used to 
   get the word alignment.  */
ULONG                   module_pool_space[BYTE_POOL_SIZE / 4];

/* Define the ThreadX object control blocks...  */
TX_THREAD               *thread_0;

TX_BYTE_POOL            *byte_pool_0;
TX_BLOCK_POOL           *block_pool_0;

INT errno = 0;

/* Define thread prototypes.  */
void thread_0_entry(ULONG thread_input);
int ta_abort_test(void);

int g_test_progress = 0;

static int __getFileSize(const char *filename)
{
  int size    = 0;
  qapi_FS_Status_t ret     = 0;
  struct qapi_FS_Stat_Type_s stat_buf = {0};

  ret = qapi_FS_Stat(filename, &stat_buf);
  if(ret != QAPI_OK) {
    LOGSE("Failed to open file %s: %s (%d)", filename, __func__, errno);
    size = -1;
  }
  else{
    size = stat_buf.st_size;
  }
  return size;
}

qapi_FS_Status_t qapi_FS_Read_Full(int Fd, uint8_t *Buf, uint32_t Count,
    uint32_t *Bytes_Read_Ptr)
{
    qapi_FS_Status_t qapi_status = QAPI_OK;
    uint32_t bytes_remaining = Count;
    uint32_t bytes_read = 0;

    *Bytes_Read_Ptr = 0;
    while (qapi_status == QAPI_OK && bytes_remaining > 0) {
      bytes_remaining -= bytes_read;

      bytes_read = 0;
      qapi_status = qapi_FS_Read(Fd, Buf + *Bytes_Read_Ptr, bytes_remaining,
          &bytes_read);

      *Bytes_Read_Ptr += bytes_read;
    }

    return qapi_status;
}

static int __readFile(const char *filename, size_t size, uint8_t *buffer)
{
  int fd = -1;
  size_t readBytes  = 0;
  int ret           = 0;
  qapi_FS_Status_t qapi_status        = QAPI_OK;
  do {
    ret = qapi_FS_Open (filename, QAPI_FS_O_RDONLY_E, &fd);
    if((ret != QAPI_OK) || (fd < 0)) {
      LOGSE("Failed to open file %s: %s (%d)", filename, __func__, errno);
      ret = -1;
      break;
    }
    qapi_status = qapi_FS_Read_Full(fd, buffer, size, &readBytes);
    if ((qapi_status != QAPI_OK) || (readBytes != size)) {
      LOGSE("Error reading the file %s: %zu vs %zu bytes: %s (%d)",
        filename,
        readBytes,
        size,
        __func__,
        errno);
      ret = -1;
      break;
    }
    ret = size;
  } while (0);
  if (fd >= 0) {
    qapi_FS_Close(fd);
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

    // buffer = qtee_malloc(size*sizeof(*buffer));
    /* Allocate the stack for thread 0.  */
    ret = tx_byte_allocate(byte_pool_0, (VOID **) &buffer, size, TX_NO_WAIT);
    if (ret) {
        break;
    }

    ret = __readFile(path, size, buffer);
    if (ret < 0) break;

    ret = IAppLoader_loadFromBuffer(appLoader, buffer, size, appController);

  } while (0);

  tx_byte_release(buffer);

  if (ret) {
    LOGE("Load app %s failed: %d", "path.cstr()", ret);
  } else {
    LOGD("Load app %s succeeded", "path.cstr()");
  }

  return ret;
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

int32_t test_perf_sfs(Object self)
{
    int32_t err = Object_OK;

    LOGD("RUNNING test_perf_sfs...\n");

    err = IComTestApp_run_test(self, IComTestApp_TEST_CMD_RUN_SFS_PERF_TEST);

    if (Object_isERROR(err)) {
        LOGE("FAILED test_perf_sfs with %d", err);
        return Object_ERROR;
    }

    LOGD("PASSED test_perf_sfs");
    return Object_OK;
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

/* Define the module start function.  */
void com_test_app_module_start(ULONG id)
{
    CHAR *pointer=NULL;

    /* Allocate all the objects. In MMU mode, modules cannot allocate control blocks within
       their own memory area so they cannot corrupt the resident portion of ThreadX by overwriting 
       the control block(s).  */
    txm_module_object_allocate((void **)&thread_0, sizeof(TX_THREAD));
    txm_module_object_allocate((void **)&byte_pool_0, sizeof(TX_BYTE_POOL));

    /* Create a byte memory pool from which to allocate the thread stacks.  */
    tx_byte_pool_create(byte_pool_0, "module byte pool 0", module_pool_space,
            BYTE_POOL_SIZE);

    /* Allocate the stack for thread 0.  */
    tx_byte_allocate(byte_pool_0, (VOID **) &pointer, STACK_SIZE, TX_NO_WAIT);

    /* Create the main thread.  */
    tx_thread_create(thread_0, "module thread 0", thread_0_entry, 0, pointer,
            STACK_SIZE, 170, 170, TX_NO_TIME_SLICE, TX_AUTO_START);
}

static int test_mult_getopener()
{
    int err = Object_OK;
    Object openerObj = Object_NULL;
    Object openerObj2 = Object_NULL;

    LOGD("RUNNING [test_mult_getopener]...\n");

    // Test that the openerObj returns the same stuff each time.
    err = qapi_QTEEInvoke_GetOpener(&openerObj);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_mult_getopener] with %d", err);
        return err;
    }

    err = qapi_QTEEInvoke_GetOpener(&openerObj2);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_mult_getopener] with %d", err);
        return err;
    }

    err = qapi_IOpener_release(openerObj2);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_mult_getopener] IOpener_release #1 with %d", err);
        return err;
    }

    err = qapi_IOpener_release(openerObj);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_mult_getopener] IOpener_release #2 with %d", err);
        return err;
    }

    LOGD("PASSED [test_mult_getopener]");
    return Object_OK;
}

static int test_mult_comtestapp()
{
    int err = Object_OK;
    Object openerObj = Object_NULL;
    Object appLoaderAppObj = Object_NULL;
    Object appLoaderAppObj2 = Object_NULL;
    Object appLoaderAppObj3 = Object_NULL;
    Object comTestAppObj = Object_NULL;
    Object comTestAppObj2 = Object_NULL;
    Object comTestAppObj3 = Object_NULL;
    Object appController  = Object_NULL;

    LOGD("RUNNING [test_mult_comtestapp]...\n");

    // Test that the openerObj returns successfully
    err = qapi_QTEEInvoke_GetOpener(&openerObj);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_mult_comtestapp] qapi_QTEEInvoke_GetOpener with %d", err);
        return err;
    }

    // Test that multiple TA services open correctly and don't collide
    err = qapi_IOpener_open(openerObj, CAppLoader_UID, &appLoaderAppObj);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_mult_comtestapp] IOpener_open #1 with %d", err);
        return err;
    }

    // Test that subsequent comtestapp open doesn't collide with previous
    err = qapi_IOpener_open(openerObj, CAppLoader_UID, &appLoaderAppObj2);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_mult_comtestapp] IOpener_open #2 with %d", err);
        return err;
    }

    // Test that subsequent comtestapp open doesn't collide with previous
    err = qapi_IOpener_open(openerObj, CAppLoader_UID, &appLoaderAppObj3);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_mult_comtestapp] IOpener_open #3 with %d", err);
        return err;
    }

    err = loadApp(appLoaderAppObj, COM_TEST_APP_FILE, &appController);
    if(err) {
        LOGE("FAILED [test_simple_comtestapp] loadApp with %d", err);
        goto IOpener_release;
    }

    err = IAppController_getAppObject(appController, &comTestAppObj);
    if(err) {
        LOGE("FAILED [test_simple_comtestapp] IAppController_getAppObject with %d", err);
        goto IOpener_release;
    }

    err = IAppController_getAppObject(appController, &comTestAppObj2);
    if(err) {
        LOGE("FAILED [test_simple_comtestapp] IAppController_getAppObject with %d", err);
        goto IOpener_release;
    }

    err = IAppController_getAppObject(appController, &comTestAppObj3);
    if(err) {
        LOGE("FAILED [test_simple_comtestapp] IAppController_getAppObject with %d", err);
        goto IOpener_release;
    }

    // Test that each TA service handle operates as expected
    err = test_add_nums(comTestAppObj);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_simple_comtestapp] test_add_nums(comTestAppObj) failed with %d",
            err);
        return err;
    }

    err =test_add_nums(comTestAppObj2);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_simple_comtestapp] test_add_nums(comTestAppObj2) failed with %d",
            err);
        return err;
    }

    err = test_add_nums(comTestAppObj3);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_simple_comtestapp] test_add_nums(comTestAppObj3) failed with %d",
            err);
        return err;
    }

    // Test that all TA service objects release properly
    err = IAppController_unload(appController);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_mult_comtestapp] FAILED IComTestApp_release #1 with %d", err);
        return err;
    }

IOpener_release:
    err = qapi_IOpener_release(openerObj);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_mult_comtestapp] IOpener_release with %d", err);
        return err;
    }

    LOGD("PASSED [test_mult_comtestapp]");
    return Object_OK;
}

static int test_simple_comtestapp()
{
    int err = Object_OK;
    Object openerObj = Object_NULL;
    Object appLoaderAppObj = Object_NULL;
    Object comTestAppObj = Object_NULL;
    Object appController  = Object_NULL;

    LOGSD("RUNNING [%s]...\n", __FUNCTION__);

    // Test that the openerObj returns successfully
    err = qapi_QTEEInvoke_GetOpener(&openerObj);
    if (Object_isERROR(err)) {
        LOGSE("FAILED [%s] qapi_QTEEInvoke_GetOpener with %d", __FUNCTION__, err);
        return err;
    }
    LOGSE("[%s]:: qapi_QTEEInvoke_GetOpener passed", __FUNCTION__);

    // Test that the TA service opens correctly
    err = qapi_IOpener_open(openerObj, CAppLoader_UID, &appLoaderAppObj);
    if (Object_isERROR(err)) {
        LOGSE("FAILED [%s] IOpener_open with %d", __FUNCTION__, err);
        goto IOpener_release;
    }
    LOGSE("[%s]:: qapi_IOpener_open passed", __FUNCTION__);

    err = loadApp(appLoaderAppObj, COM_TEST_APP_FILE, &appController);
    if (Object_isERROR(err)) {
        LOGSE("FAILED [%s] loadApp with %d", __FUNCTION__, err);
        goto qapi_IAppLoader_release;
    }
    LOGSE("[%s]:: loadApp passed", __FUNCTION__);

    err = IAppController_getAppObject(appController, &comTestAppObj);
    if (Object_isERROR(err)) {
        LOGSE("FAILED [%s] IAppController_getAppObject with %d", __FUNCTION__, err);
        goto qapi_IAppController_release;
    }
    LOGSE("[%s]:: IAppController_getAppObject passed", __FUNCTION__);

    // Test that the TA service operates as expected
    err = test_add_nums(comTestAppObj);
    if (Object_isERROR(err)) {
        LOGSE("FAILED [%s] test_add_nums failed with %d", __FUNCTION__,
            err);
        goto IComTestApp_release;
    }
    LOGSE("[%s]:: test_add_nums passed", __FUNCTION__);

    // Test that TA service object and opener object release properly
    err |= IAppController_unload(appController);
    if (Object_isERROR(err)) {
        LOGSE("FAILED [%s] IAppController_unload with %d", __FUNCTION__, err);
        return err;
    }
    LOGSE("[%s]:: IAppController_unload passed", __FUNCTION__);

IComTestApp_release:
    err |= IComTestApp_release(comTestAppObj);
    if (Object_isERROR(err)) {
        LOGSE("FAILED [%s] IComTestApp_release with %d", __FUNCTION__, err);
    }

qapi_IAppController_release:
    err |= IAppController_release(appController);
    if (Object_isERROR(err)) {
        LOGSE("FAILED [%s] IAppController_release with %d", __FUNCTION__, err);
    }

qapi_IAppLoader_release:
    err |= IAppLoader_release(appLoaderAppObj);
    if (Object_isERROR(err)) {
        LOGSE("FAILED [%s] IAppLoader_release with %d", __FUNCTION__, err);
    }

IOpener_release:
    err |= qapi_IOpener_release(openerObj);
    if (Object_isERROR(err)) {
        LOGSE("FAILED [%s] IOpener_release with %d", __FUNCTION__, err);
    }

    LOGSD("[%s]:: PASSED", __FUNCTION__);
    return err;
}

static int test_out_of_order_release_compute()
{
    int err = Object_OK;
    Object openerObj = Object_NULL;
    Object appLoaderAppObj = Object_NULL;
    Object appController  = Object_NULL;
    Object comTestAppObj = Object_NULL;

    LOGD("RUNNING [test_out_of_order_release_compute]...\n");

    // Test that the openerObj returns successfully
    err = qapi_QTEEInvoke_GetOpener(&openerObj);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_out_of_order_release_compute] qapi_QTEEInvoke_GetOpener with %d", err);
        return err;
    }

    // Test that the TA service opens correctly
    err = qapi_IOpener_open(openerObj, CAppLoader_UID, &appLoaderAppObj);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_out_of_order_release_compute] IOpener_open with %d", err);
        return err;
    }

    err = loadApp(appLoaderAppObj, COM_TEST_APP_FILE, &appController);
    if(err) {
        LOGE("FAILED [test_simple_comtestapp] loadApp with %d", err);
        return err;
    }

    err = IAppController_getAppObject(appController, &comTestAppObj);
    if(err) {
        LOGE("FAILED [test_simple_comtestapp] IAppController_getAppObject with %d", err);
        return err;
    }

    // Test early release of opener obj, then compute comtestapp after
    err = qapi_IOpener_release(openerObj);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_out_of_order_release_compute] early IOpener_release with %d", err);
        return err;
    }

    // Test that the TA service operates as expected
    err = test_add_nums(comTestAppObj);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_out_of_order_release_compute] test_add_nums failed with %d",
            err);
        return err;
    }

    // Test that TA service object and opener object release properly
    err |= IAppController_unload(appController);
    if (Object_isERROR(err)) {
        LOGE("FAILED [test_simple_comtestapp] IComTestApp_release with %d", err);
        return err;
    }

    LOGD("PASSED [test_out_of_order_release_compute]");
    return Object_OK;
}

// Main test thread
void thread_0_entry(ULONG thread_input)
{
    Object openerObj = Object_NULL;
    Object comTestAppObj = Object_NULL;
    int32_t err = Object_OK;
    int32_t num_iterations = 0;
    Object appLoaderAppObj = Object_NULL;
    Object appController  = Object_NULL;
    g_test_progress = 0;

    for(num_iterations = 0; num_iterations < 1; num_iterations++) {

        //*************************************************************************
        // Start app


        //*************************************************************************
        // Test the simplest use case
        err = test_simple_comtestapp();
        if (Object_isERROR(err))
            goto cleanup;
        ++g_test_progress;

        //*************************************************************************
        // Test asking for multiple opener objects
        err = test_mult_getopener();
        if (Object_isERROR(err))
            goto cleanup;
        ++g_test_progress;

        //*************************************************************************
        // Test opening, using, and closing multiple comtestapp objects
        err = test_mult_comtestapp();
        if (Object_isERROR(err))
            goto cleanup;
        ++g_test_progress;

        //*************************************************************************
        // Test releasing objects out of order, then using comtestapp
        err = test_out_of_order_release_compute();
        if (Object_isERROR(err))
            goto cleanup;
        ++g_test_progress;

        //*************************************************************************
        // FS Tests
        err = qapi_QTEEInvoke_GetOpener(&openerObj);
        if (Object_isERROR(err))
            goto cleanup;

        // Test that the TA service opens correctly
        err = qapi_IOpener_open(openerObj, CAppLoader_UID, &appLoaderAppObj);
        if (Object_isERROR(err)) {
            LOGE("FAILED [test_simple_comtestapp] IOpener_open with %d", err);
            goto cleanup;
        }
        LOGE("thread_0_entry:: qapi_IOpener_open passed");

        err = loadApp(appLoaderAppObj, COM_TEST_APP_FILE, &appController);
        if (Object_isERROR(err)) {
            LOGE("FAILED [test_simple_comtestapp] loadApp with %d", err);
            goto cleanup;
        }
        LOGE("thread_0_entry:: loadApp passed");

        err = IAppController_getAppObject(appController, &comTestAppObj);
        if (Object_isERROR(err)) {
            LOGE("FAILED [test_simple_comtestapp] IAppController_getAppObject with %d", err);
            goto cleanup;
        }
        LOGE("thread_0_entry:: IAppController_getAppObject passed");

        err = test_fs(comTestAppObj);
        if (Object_isERROR(err))
            goto cleanup;
        ++g_test_progress;

        err = test_sfs(comTestAppObj);
        if (Object_isERROR(err))
            goto cleanup;
        ++g_test_progress;

        err = IComTestApp_release(comTestAppObj);
        if (Object_isERROR(err))
            goto cleanup;

        err = qapi_IOpener_release(openerObj);
        if (Object_isERROR(err))
            goto cleanup;

    cleanup:
        Object_ASSIGN_NULL(comTestAppObj);
        Object_ASSIGN_NULL(appController);
        Object_ASSIGN_NULL(appLoaderAppObj);
        Object_ASSIGN_NULL(openerObj);

        //*************************************************************************
        // Negative test -- Try sending cmds to TA, after TA is shutdown completely
        // Test the simplest use case
        // TODO: write a negative test
        /* err = test_simple_comtestapp();
        if (err == Object_ERROR_KMEM) // QTEEProxyObj_invoke is returning Object_ERROR_KMEM incase of failure
          ++g_test_progress; */

        if(!ta_abort_test()) {
            LOGSE("[%s]::ta_abort_test() failed", __FUNCTION__);
        }
    }

}

Object ta_excpn_openerObj = Object_NULL;
Object ta_excpn_appLoaderAppObj = Object_NULL;
Object ta_excpn_comTestAppObj = Object_NULL;
Object ta_excpn_appController  = Object_NULL;

void com_test_app_module_stop(ULONG id)
{
    Object_ASSIGN_NULL(ta_excpn_comTestAppObj);
    Object_ASSIGN_NULL(ta_excpn_appController);
    Object_ASSIGN_NULL(ta_excpn_appLoaderAppObj);
    Object_ASSIGN_NULL(ta_excpn_openerObj);

    ++g_test_progress;
    return;
}

int ta_abort_test(void)
{
    int err = Object_ERROR;
    int *abortPtr = NULL;

    LOGSD("RUNNING [%s]...\n", __FUNCTION__);

    // Test that the ta_excpn_openerObj returns successfully
    err = qapi_QTEEInvoke_GetOpener(&ta_excpn_openerObj);
    if (Object_isERROR(err)) {
        LOGSE("FAILED [%s] qapi_QTEEInvoke_GetOpener with %d", __FUNCTION__, err);
        return err;
    }
    LOGSE("[%s]:: qapi_QTEEInvoke_GetOpener passed", __FUNCTION__);

    // Test that the TA service opens correctly
    err = qapi_IOpener_open(ta_excpn_openerObj, CAppLoader_UID, &ta_excpn_appLoaderAppObj);
    if (Object_isERROR(err)) {
        LOGSE("FAILED [%s] IOpener_open with %d", __FUNCTION__, err);
        goto IOpener_release;
    }
    LOGSE("[%s]:: qapi_IOpener_open passed", __FUNCTION__);

    err = loadApp(ta_excpn_appLoaderAppObj, COM_TEST_APP_FILE, &ta_excpn_appController);
    if (Object_isERROR(err)) {
        LOGSE("FAILED [%s] loadApp with %d", __FUNCTION__, err);
        goto qapi_IAppLoader_release;
    }
    LOGSE("[%s]:: loadApp passed", __FUNCTION__);

    err = IAppController_getAppObject(ta_excpn_appController, &ta_excpn_comTestAppObj);
    if (Object_isERROR(err)) {
        LOGSE("FAILED [%s] IAppController_getAppObject with %d", __FUNCTION__, err);
        goto qapi_IAppController_release;
    }
    LOGSE("[%s]:: IAppController_getAppObject passed", __FUNCTION__);

    // Test that the TA service operates as expected
    err = test_add_nums(ta_excpn_comTestAppObj);
    if (Object_isERROR(err)) {
        LOGSE("FAILED [%s] test_add_nums failed with %d", __FUNCTION__,
            err);
        goto IComTestApp_release;
    }

    // generate an abort scenario
    *abortPtr = 10;

    // Never come here
    g_test_progress = 0xDEADBEEF;
    
    LOGSE("[%s]:: test_add_nums passed", __FUNCTION__);

IComTestApp_release:
    Object_ASSIGN_NULL(ta_excpn_comTestAppObj);

qapi_IAppController_release:
    Object_ASSIGN_NULL(ta_excpn_appController);

qapi_IAppLoader_release:
    Object_ASSIGN_NULL(ta_excpn_appLoaderAppObj);

IOpener_release:
    Object_ASSIGN_NULL(ta_excpn_openerObj);

    LOGSD("[%s]:: PASSED", __FUNCTION__);
    return err;  
}

