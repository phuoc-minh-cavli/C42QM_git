/********************************************************************
---------------------------------------------------------------------
 Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
----------------------------------------------------------------------
 SMCInvoke AppLoader Sample/Test Client app.
*********************************************************************/

#include <stdint.h>
#include <stdio.h>

// sampleapp common header
#include "smcinvoke_sample_client.h"

// For QTEE<->TZ SMCInvoke communication
#include "qapi_QTEEInvoke.h"
#include "qapi_IOpener.h"

// For app loading
#include "CAppLoader.h"
#include "IAppLoader.h"
#include "IAppController.h"

// FS interfaces
#include "qapi_fs.h"
#include "qapi_fs_types.h"

// sampleapp test interfaces
#include "IAppLegacyTest.h"

/* A few defines from cdefs.h to make it easier to port code over */
#define C_LENGTHOF(array)                     \
  ((void*) &(array) == (void*) (array)        \
   ? sizeof (array) / sizeof *(array)         \
   : 0)

#define C_FOR_ARRAY(var, array) \
  for (var = 0; var < C_LENGTHOF(array); ++var)

/* commands supported by sample client */
#define CLIENT_CMD0_GET_VERSION                               0
#define CLIENT_CMD1_BASIC_DATA                                1
#define CLIENT_CMD2_REGISTER_SB_PTR                           2
#define CLIENT_CMD3_RUN_CRYTPO_TEST                           3
#define CLIENT_CMD4_RUN_CRYTPO_PERF                           4
#define CLIENT_CMD5_RUN_SFS_TEST                              5
#define CLIENT_CMD6_RUN_FS_TEST                               6
#define CLIENT_CMD7_RUN_RSA_TEST                              7
#define CLIENT_CMD8_RUN_RSA_PERF_TEST                         8
#define CLIENT_CMD9_RUN_CMNLIB_TEST                           9
#define CLIENT_CMD10_RUN_CORE_TEST                            10
#define CLIENT_CMD11_RUN_SECURECHANNEL_TEST                   11
#define CLIENT_CMD12_RUN_MESSAGE_PASSING                      12
#define CLIENT_CMD13_RUN_MISC_TEST                            13

#define CLIENT_CMD16_RUN_BUSES_TEST                           16
#define CLIENT_CMD17_RUN_STOR_TEST                            17
#define CLIENT_CMD18_RUN_FUSE_TEST                            18
#define CLIENT_CMD19_RUN_BUSES_SPI_TEST                       19
#define CLIENT_CMD21_RUN_CRYPTO_COPY                          21
#define CLIENT_CMD22_RUN_SECURE_UI_DISPLAY_TEST               22
#define CLIENT_CMD23_RUN_SECURE_UI_TOUCH_TEST                 23
#define CLIENT_CMD24_RUN_CP_UTILS_TAG_MEM                     24
#define CLIENT_CMD25_RUN_SAFE_TEST                            25
#define CLIENT_CMD26_RUN_ABORT_TEST                           26
#define CLIENT_CMD27_RUN_SMPLSERV_TEST                        27
#define CLIENT_CMD29_RUN_BIO_LIB_TEST                         29
#define CLIENT_CMD30_RUN_BULLETIN_BOARD_TEST                  30
#define CLIENT_CMD31_RUN_TLMM_TEST                            31
#define CLIENT_CMD32_RUN_ABORT_TEST                           32
#define CLIENT_CMD33_RUN_SMPLCERT_TEST                        33
#define CLIENT_CMD34_RUN_TRANS_TEST                           34
#define CLIENT_CMD35_RUN_OEM_BUF_TEST                         35
#define CLIENT_CMD36_RUN_INTLOCK_TEST                         36
#define CLIENT_CMD37_REG_SHARED_BUFS                          37
#define CLIENT_CMD38_ACCESS_REGION                            38
#define CLIENT_CMD39_RUN_SANDBOX_TEST                         39

#define CLIENT_CMD50_RUN_LISTENER_FRAMEWORK_TEST              50
#define CLIENT_CMD51_RUN_VFP_TEST                             51
#define CLIENT_CMD52_SPIN_TEST                                52
#define CLIENT_CMD53_RUN_LOG_BITMASK_TEST                     53
#define CLIENT_CMD54_MINK_SERVER_EXIT                         54
#define CLIENT_CMD55_RUN_SPCOM_TEST                           55

#define CLIENT_CMD60_RUN_HDCP_KEY_WRITE_TEST                  60

#define CLIENT_CMD99_RUN_SFS_PERF_TEST                        99
#define CLIENT_CMD100_RUN_CACHE_TEST                         100
#define CLIENT_CMD101_RUN_SHAREDBUFFER_TEST                  101
#define CLIENT_CMD102_RUN_SHAREDBUFFER_RO_TEST               102
#define CLIENT_CMD999_RUN_SMPLSERV_CRASH_TEST                999
#define CLIENT_CMD1000_RUN_BLSP_MODIFY_OWNERSHIP_TEST       1000

#define CLIENT_CMD201_GET_VERSION_ADVERSARIAL_TEST             201
#define CLIENT_CMD501_RUN_SFS_ADVERSARIAL_TEST                 205

#define MAX_PATH_LEN  256

typedef struct {
  char const *name;
  bool is32;
} SampleApp_t;

/* LE only test 32bit TA */
static SampleApp_t sampleapps[] = {
  {"smplap32.mbn", true},
};

typedef struct testCfg {
  uint32_t cmdId;
  char const * name;
  bool enabled32;
  bool enabled64;
  char const * notes;
} TestCfg;

static const TestCfg smplappTests[] = {
  {  CLIENT_CMD0_GET_VERSION,                "CLIENT_CMD0_GET_VERSION",               true,   true,   "" },
  {  CLIENT_CMD100_RUN_CACHE_TEST,           "CLIENT_CMD100_RUN_CACHE_TEST",          true,   true,   "" },
  {  CLIENT_CMD101_RUN_SHAREDBUFFER_TEST,    "CLIENT_CMD101_RUN_SHAREDBUFFER_TEST",   false,  false,  "" },
  {  CLIENT_CMD10_RUN_CORE_TEST,             "CLIENT_CMD10_RUN_CORE_TEST",            false,  false,  "" },
  {  CLIENT_CMD11_RUN_SECURECHANNEL_TEST,    "CLIENT_CMD11_RUN_SECURECHANNEL_TEST",   false,  false,  "" },
  {  CLIENT_CMD12_RUN_MESSAGE_PASSING,       "CLIENT_CMD12_RUN_MESSAGE_PASSING",      false,  false,  "" },
  {  CLIENT_CMD13_RUN_MISC_TEST,             "CLIENT_CMD13_RUN_MISC_TEST",            true,   true,   "" },
  {  CLIENT_CMD18_RUN_FUSE_TEST,             "CLIENT_CMD18_RUN_FUSE_TEST",            true,   true,   "" },
  {  CLIENT_CMD1_BASIC_DATA,                 "CLIENT_CMD1_BASIC_DATA",                false,  false,  "" },
//  {  CLIENT_CMD27_RUN_SMPLSERV_TEST,         "CLIENT_CMD27_RUN_SMPLSERV_TEST",        true,   true,   "needs smplserv" },
  {  CLIENT_CMD31_RUN_TLMM_TEST,             "CLIENT_CMD31_RUN_TLMM_TEST",            false,  false,   "" },
  {  CLIENT_CMD32_RUN_ABORT_TEST,            "CLIENT_CMD32_RUN_ABORT_TEST",           false,  false,  "" },
//  {  CLIENT_CMD33_RUN_SMPLCERT_TEST,         "CLIENT_CMD33_RUN_SMPLCERT_TEST",        true,   true,   "needs smplcert" },
  {  CLIENT_CMD34_RUN_TRANS_TEST,            "CLIENT_CMD34_RUN_TRANS_TEST",           false,  false,  "" },
  {  CLIENT_CMD35_RUN_OEM_BUF_TEST,          "CLIENT_CMD35_RUN_OEM_BUF_TEST",         false,  false,  "" },
  {  CLIENT_CMD36_RUN_INTLOCK_TEST,          "CLIENT_CMD36_RUN_INTLOCK_TEST",         false,  false,   "" },
//  {  CLIENT_CMD39_RUN_SANDBOX_TEST,          "CLIENT_CMD39_RUN_SANDBOX_TEST",         true,   true,   "needs smplserv" },
  {  CLIENT_CMD3_RUN_CRYTPO_TEST,            "CLIENT_CMD3_RUN_CRYTPO_TEST",           false,  false,  "" },
  {  CLIENT_CMD4_RUN_CRYTPO_PERF,            "CLIENT_CMD4_RUN_CRYTPO_PERF",           false,  false,  "" },
  {  CLIENT_CMD5_RUN_SFS_TEST,               "CLIENT_CMD5_RUN_SFS_TEST",              true,  true,  "" },
  {  CLIENT_CMD6_RUN_FS_TEST,                "CLIENT_CMD6_RUN_FS_TEST",               true,  true,  "" },
  {  CLIENT_CMD51_RUN_VFP_TEST,              "CLIENT_CMD51_RUN_VFP_TEST",             false,  false,   "" },
  {  CLIENT_CMD52_SPIN_TEST,                 "CLIENT_CMD52_SPIN_TEST",                false,  false,   "" },
  {  CLIENT_CMD53_RUN_LOG_BITMASK_TEST,      "CLIENT_CMD53_RUN_LOG_BITMASK_TEST",     false,  false,   "" },
  {  CLIENT_CMD54_MINK_SERVER_EXIT,          "CLIENT_CMD54_MINK_SERVER_EXIT",         false,  false,  "" },
  {  CLIENT_CMD7_RUN_RSA_TEST,               "CLIENT_CMD7_RUN_RSA_TEST",              false,  false,  "" },
//  {  CLIENT_CMD999_RUN_SMPLSERV_CRASH_TEST,  "CLIENT_CMD999_RUN_SMPLSERV_CRASH_TEST", false,  true,   "needs smplserv" }, // we can crash it for 64 since we run 32 before 64
};

static const TestCfg adversarialTests[] = {
  {  CLIENT_CMD201_GET_VERSION_ADVERSARIAL_TEST,           "CLIENT_CMD201_GET_VERSION_ADVERSARIAL_TEST",               true,   true,   "" },
  {  CLIENT_CMD501_RUN_SFS_ADVERSARIAL_TEST,               "CLIENT_CMD501_RUN_SFS_ADVERSARIAL_TEST",              true,  true,  "" },
};

enum test_types {
  NOMINAL,
  ADVERSARIAL,
  INTERFACE,
  INTERNAL,
  CALLBACKOBJ,
  MEMORYOBJ,
  LAST_TEST,
};

/* Pretty printing :) */
static char const *__cmd2str(uint32_t cmdId) {
  size_t ii;
  C_FOR_ARRAY(ii, smplappTests) {
    if (smplappTests[ii].cmdId == cmdId) return smplappTests[ii].name;
  }
  return "UNKNOWN";
}

struct qsc_send_cmd {
     uint32_t cmd_id;
     uint32_t data;
     uint32_t data2;
     uint32_t len;
     uint32_t start_pkt;
     uint32_t end_pkt;
     uint32_t test_buf_size;
};

struct qsc_send_cmd_64bit {
     uint32_t cmd_id;
     uint64_t data;
     uint64_t data2;
     uint32_t len;
     uint32_t start_pkt;
     uint32_t end_pkt;
     uint32_t test_buf_size;
};

struct qsc_send_cmd_rsp {
  uint32_t data;
  int32_t status;
};

#ifdef  QAPI_TXM_MODULE     // USER_MODE_DEFS

#define SAMPLEAPP_BYTE_POOL_SIZE  (20 * 1024)

static TX_BYTE_POOL *sampleapp_byte_pool = NULL;
static ULONG sampleapp_byte_pool_space[SAMPLEAPP_BYTE_POOL_SIZE / 4];

#define malloc  sampleapp_malloc
#define free    sampleapp_free

#define LOGD(fmt, ...) QAPI_MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH, fmt, ## __VA_ARGS__);
#define LOGE(fmt, ...) QAPI_MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, fmt, ## __VA_ARGS__);

qapi_Status_t sampleapp_mem_init(
    unsigned long *byte_pool, unsigned long byte_pool_len)
{
    int tx_ret = TX_SUCCESS;

    if (!sampleapp_byte_pool)
    {
        tx_ret = txm_module_object_allocate(&sampleapp_byte_pool, sizeof(TX_BYTE_POOL));
        if (tx_ret != TX_SUCCESS)
            goto obj_alloc_failed;

        tx_ret = tx_byte_pool_create(sampleapp_byte_pool, "sampleapp_byte_pool",
                byte_pool, byte_pool_len);
        if (tx_ret != TX_SUCCESS)
            goto pool_create_failed;
    }

    return QAPI_OK;

pool_create_failed:
    txm_module_object_deallocate(sampleapp_byte_pool);

obj_alloc_failed:
    sampleapp_byte_pool = NULL;
    return QAPI_ERROR;
}

VOID *sampleapp_malloc(UINT size)
{
    CHAR *pointer = NULL;

    if(sampleapp_byte_pool == NULL) {
        if(QAPI_OK == sampleapp_mem_init((unsigned long *)&sampleapp_byte_pool_space, 
                                    SAMPLEAPP_BYTE_POOL_SIZE)) {
        } else {
            return NULL;
        }
    }
    
    tx_byte_allocate(sampleapp_byte_pool, (VOID **) &pointer, size, TX_NO_WAIT);

    return pointer;
}

INT sampleapp_free(VOID *addr)
{
    INT ret = TX_POOL_ERROR;
    if(sampleapp_byte_pool) {
        ret = tx_byte_release(addr);
    }
    
    return ret;
}

#else

// QSEECom logging
#include "qseecom_log.h"

#endif


static int __getFileSize(const char *filename)
{
  int size    = 0;
  qapi_FS_Status_t ret     = 0;
  struct qapi_FS_Stat_Type_s stat_buf = {0};

  ret = qapi_FS_Stat(filename, &stat_buf);
  if(ret != QAPI_OK) {
    LOGSE("[%s]::Failed to open file %s", __FUNCTION__, filename);
    size = -1;
  }
  else{
    size = stat_buf.st_size;
  }
  return size;
}

static qapi_FS_Status_t qapi_FS_Read_Full(int Fd, uint8_t *Buf, uint32_t Count,
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
          (uint32 *)&bytes_read);

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
      LOGSE("[%s]::Failed to open file %s", __FUNCTION__, filename);
      ret = -1;
      break;
    }
    qapi_status = qapi_FS_Read_Full(fd, buffer, size, &readBytes);
    if ((qapi_status != QAPI_OK) || (readBytes != size)) {
      LOGSE("[%s]::Error reading the file %s: %zu vs %zu bytes",
        __FUNCTION__,
        filename,
        readBytes,
        size);
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
    LOGE("Load app %s failed: %d", path, ret);
  } else {
    LOGD("Load app %s succeeded", path);
  }

  return ret;
}


static int sendCommand(uint32_t cmdId, Object appLegacy, bool b32)
{
  int ret                                 = Object_OK;
  struct qsc_send_cmd smplap32_req        = {0};
  struct qsc_send_cmd_64bit smplap64_req  = {0};
  struct qsc_send_cmd_rsp smplap_rsp      = {0};
  size_t rspSizeOut                       = sizeof(smplap_rsp);
  void * req                              = NULL;
  size_t reqLen                           = 0;
  const char *cmdName                     = NULL;

  if (b32) {
    req = &smplap32_req;
    reqLen = sizeof(smplap32_req);
  } else {
    req = &smplap64_req;
    reqLen = sizeof(smplap64_req);
  }
  smplap32_req.cmd_id = cmdId;
  smplap64_req.cmd_id = cmdId;
  smplap_rsp.status = -1;

  cmdName = __cmd2str(cmdId);

  LOGD_PRINT("CMD: %u %s (%s)", cmdId, cmdName, (b32)?"32":"64");
  switch (cmdId) {
    /* Simple shared buffer case: None of these tests are supposed to work, since the WL will be empty! */
    case CLIENT_CMD101_RUN_SHAREDBUFFER_TEST:
    case CLIENT_CMD10_RUN_CORE_TEST:
    case CLIENT_CMD11_RUN_SECURECHANNEL_TEST:
    case CLIENT_CMD1_BASIC_DATA:
    case CLIENT_CMD3_RUN_CRYTPO_TEST:
    case CLIENT_CMD4_RUN_CRYTPO_PERF:
    {
      LOGD_PRINT("SMC Invoke doesn't support sharing buffers yet, skipping cmd %u", cmdId);
      ret = Object_OK;
      break;
    }

    /* Simple case, just pass cmdId, and check status */
    case CLIENT_CMD0_GET_VERSION:
    case CLIENT_CMD100_RUN_CACHE_TEST:
    //case CLIENT_CMD12_RUN_MESSAGE_PASSING: // API unavailable for SMCI-loaded TAs
    case CLIENT_CMD13_RUN_MISC_TEST:
    case CLIENT_CMD18_RUN_FUSE_TEST:
    case CLIENT_CMD27_RUN_SMPLSERV_TEST:
    case CLIENT_CMD31_RUN_TLMM_TEST:
    case CLIENT_CMD33_RUN_SMPLCERT_TEST:
    case CLIENT_CMD35_RUN_OEM_BUF_TEST:
    case CLIENT_CMD36_RUN_INTLOCK_TEST:
    case CLIENT_CMD39_RUN_SANDBOX_TEST:
    case CLIENT_CMD5_RUN_SFS_TEST:
    case CLIENT_CMD51_RUN_VFP_TEST:
    case CLIENT_CMD52_SPIN_TEST:
    case CLIENT_CMD53_RUN_LOG_BITMASK_TEST:
    case CLIENT_CMD6_RUN_FS_TEST:
    case CLIENT_CMD7_RUN_RSA_TEST:
    case CLIENT_CMD999_RUN_SMPLSERV_CRASH_TEST:
    {
      ret = IAppLegacyTest_handleRequest(appLegacy, req, reqLen, &smplap_rsp, sizeof(smplap_rsp), &rspSizeOut);
      if (ret) break;
      TEST_TRUE(rspSizeOut == sizeof(smplap_rsp));
      ret = smplap_rsp.status;
      break;
    }

    /* This test crashes sampleapp intentionally */
    case CLIENT_CMD54_MINK_SERVER_EXIT:
    case CLIENT_CMD32_RUN_ABORT_TEST:
    {
      ret = IAppLegacyTest_handleRequest(appLegacy, req, reqLen, &smplap_rsp, sizeof(smplap_rsp), &rspSizeOut);
      TEST_TRUE(ret == Object_ERROR_DEFUNCT);
      smplap_rsp.status = 0;
      ret = Object_OK; /* test passed */
      break;
    }

    case CLIENT_CMD201_GET_VERSION_ADVERSARIAL_TEST:
    case CLIENT_CMD501_RUN_SFS_ADVERSARIAL_TEST:
    {
      ret = IAppLegacyTest_handleRequest(appLegacy, req, reqLen, &smplap_rsp, sizeof(smplap_rsp), &rspSizeOut);
      LOGE_PRINT("ret = %d", ret);
      smplap_rsp.status = 0;
// check resp.data == pattern
      ret = Object_OK; /* test passed */
      break;
    }
    default:
    {
      if (strncmp(cmdName, "UNKNOWN", sizeof("UNKNOWN") != 0)) {
        LOGD_PRINT("Command %s is currently unsupported/untested", cmdName);
        break;
      }
      LOGD_PRINT("Unknown command, sending it through");
      ret = IAppLegacyTest_handleRequest(appLegacy, req, reqLen, &smplap_rsp, sizeof(smplap_rsp), &rspSizeOut);
      LOGD_PRINT("IApp_handleRequest: %d", ret);
      LOGD_PRINT("status: %d", smplap_rsp.status);
      break;
    }
  }
  if (Object_isOK(ret) && (smplap_rsp.status == 0)) {
    LOGD_PRINT("sendCommand succeeded");
  } else {
    LOGE_PRINT("sendCommand failed: %d %d (%x)", ret, smplap_rsp.status, smplap_rsp.status);
  }
  if (ret == 0) {
    ret = smplap_rsp.status;
  }
  return ret;
}

static int smcinvoke_setup_env(Object *opener, Object *appLoader)
{
  int err = Object_OK;

  // Test that the openerObj returns successfully
  err = qapi_QTEEInvoke_GetOpener(opener);
  if (Object_isERROR(err)) {
      LOGSE("FAILED [%s] qapi_QTEEInvoke_GetOpener with %d", __FUNCTION__, err);
      goto exit;
  }
  
  LOGSD("[%s]::qapi_QTEEInvoke_GetOpener passed", __FUNCTION__);
  
  // Test that the TA service opens correctly
  err = qapi_IOpener_open(*opener, CAppLoader_UID, appLoader);
  if (Object_isERROR(err)) {
      LOGSE("FAILED [%s] qapi_IOpener_open with %d", __FUNCTION__, err);
      goto exit;
  }
  LOGSD("[%s]:: qapi_IOpener_open passed", __FUNCTION__);

exit:
  return err;
}

/**
 * Run a set of automated tests on sampleapp, both 32 and 64 bit.
 * Load smplap and smplcert automatically.
 *
 * smplap and smplcert are loaded using QSEEComAPI, sampleapp binaries
 * are assumed to be in argv[1]
 * */
static int run_sampleapp_test(char const *path, const TestCfg Tests[], size_t length)
{
  Object appLoader                = Object_NULL;
  Object appController            = Object_NULL;
  Object appLegacyTest            = Object_NULL;
  Object opener                   = Object_NULL;
  char appFullPath[MAX_PATH_LEN]  = {""};
  size_t ii                       = 0;
  size_t kk                       = 0;
  int ret                         = 0;

  if (path == NULL) {
    LOGE("Invalid path");;
    return -1;
  }

  //setup client environment
  TEST_OK(smcinvoke_setup_env(&opener, &appLoader));

  C_FOR_ARRAY(ii, sampleapps) {
    PRINT("pass");

    SILENT_OK(MAX_PATH_LEN < (strlen(path) + 
                              strlen(sampleapps[ii].name) + 
                              strlen("/") + 
                              1 /* NULL */));
    
    strlcpy(appFullPath, path, MAX_PATH_LEN);
    strlcat(appFullPath, "/", (MAX_PATH_LEN - strlen(path)));
    strlcat(appFullPath, sampleapps[ii].name, (MAX_PATH_LEN - strlen(path) - 1));
    
    PRINT("pass");
    /* Test loading smplap */

    Object_ASSIGN_NULL(appLegacyTest);
    Object_ASSIGN_NULL(appController);

    TEST_OK(loadApp(appLoader, appFullPath, &appController));
    PRINT("pass");

    /* Get the legacy interface object, so we can send commands to it */
    TEST_OK(IAppController_getAppObject(appController, &appLegacyTest));

    /* Try to load it again, this should fail */
   // TEST_FAIL(loadApp(appLoader, appFullPath, &tmp));
   // LOGD_PRINT("Failed to load app twice, this is expected behavior");

    //C_FOR_ARRAY(kk, Tests) {
    for (kk = 0; kk < length; kk++) {
      if (Tests[kk].enabled32 && sampleapps[ii].is32) {
        SILENT_OK(sendCommand(Tests[kk].cmdId, appLegacyTest, true));
      } else if (Tests[kk].enabled64 && !sampleapps[ii].is32) {
        SILENT_OK(sendCommand(Tests[kk].cmdId, appLegacyTest, false));
      }
    }
    /* Crash it */
    SILENT_OK(sendCommand(CLIENT_CMD32_RUN_ABORT_TEST, appLegacyTest, sampleapps[ii].is32));
    /* Defunct now */
    ret = sendCommand(CLIENT_CMD0_GET_VERSION, appLegacyTest, sampleapps[ii].is32);
    SILENT_TRUE(ret == Object_ERROR_DEFUNCT);
    LOGD_PRINT("Failed after crash test, this is expected behavior");
    /* Verify I cannot load it again, i.e. it has not been unloaded */
    //TEST_FAIL(loadApp(appLoader, appFullPath, &tmp));

    /* Release the controller, thus also unloading the (crashed) TA */
    Object_release(appLegacyTest);
    Object_release(appController);

    /* Make sure we can load it and talk to it again now */
    TEST_OK(loadApp(appLoader, appFullPath, &appController));
    TEST_OK(IAppController_getAppObject(appController, &appLegacyTest));
    TEST_OK(sendCommand(CLIENT_CMD0_GET_VERSION, appLegacyTest, sampleapps[ii].is32));

    Object_release(appLegacyTest);
    Object_release(appController);

  }

  Object_RELEASE_IF(appLoader);

  return 0;
}

INT run_smcinovke_sampleapp_client_test(UCHAR *path, INT test_type)
{
  if (test_type == NOMINAL) {
     PRINT("Run norminal test...\n");
     return run_sampleapp_test((const char *)path, smplappTests, C_LENGTHOF(smplappTests));
  } else if (test_type == ADVERSARIAL) {
     PRINT("Run adversatial test...\n");
     return run_sampleapp_test((const char *)path, adversarialTests, C_LENGTHOF(adversarialTests));
  } else {
     LOGE("Invalid test_type");
     return -1;
  }
}
