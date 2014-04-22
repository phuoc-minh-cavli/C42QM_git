/*
@file app_gp_fts.c

*/
/*===========================================================================
   Copyright (c) 2017 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/ssg.tz/1.1.3/securemsm/trustzone/qsapps/sampleapp/src/app_gp_fts.c#1 $
  $DateTime: 2019/05/23 05:32:51 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------

===========================================================================*/
//#include "gpPersistObjData.h"
#include "qsee_fts_crypto.h"
#include "qsee_fts_fileio.h"
#include "qsee_heap.h"
#include "qsee_timer.h"
#include "tee_internal_api.h"

#define BASE (AES_BLOCK_SZ*50) //(AES_BLOCK_SZ*100)
#define LONG_NAME "ksdfkajasl djhfasdfia shenbcihai sldjhfljah sdflkjhasd \
                   lkjfhalksj dfhlkasjdh fkljashdf lkjashdfl kjashasdfa   \
                   sdfasdfasd fasdfasdfa sdfasdfasdf asdfasdfasdf asdfasd"
#define ITERS 100
#define TEST_OBJ_SIZE   (12*1024) //((300 * 1024))
#define TEST_OBJ_NAME   "/persist/data/obj1"
#define UNIT_OBJ_NAME   "/persist/data/objUnit"
#define UNIT_OBJ_RENAME "/persist/data/objUnitRename"
#define VALID_NAME "/persist/data/gp.txt"

#define GP_ERROR_EXIT(value, ret) { \
  GPLOG_ERROR(value); \
  ret = -1; \
  goto Exit; \
} 
  
#define GP_ERROR_BREAK(value, ret) { \
  GPLOG_ERROR(value); \
  ret = -1; \
  break; \
}

typedef struct
{
  uint8_t data[TEST_OBJ_SIZE];
  uint8_t iv[QSEE_FTS_CRYPTO_IV_SIZE];
  uint8_t hmac[QSEE_FTS_CRYPTO_HMAC_SIZE];
} dataObj;

typedef struct
{
  uint8_t *data;
  uint8_t iv[QSEE_FTS_CRYPTO_IV_SIZE];
  uint8_t hmac[QSEE_FTS_CRYPTO_HMAC_SIZE];
  uint8_t hash[QSEE_FTS_CRYPTO_HASH_SIZE];
} unitDataObj;

/** Test cases **/

/*********************************************************************
 *                  GP_PERSISTENT_OBJ_DEP_TESTS                     *
 *********************************************************************/
int gpFTS_Kdf_Unit_Test(void)
{
  TEE_Result res = TEE_SUCCESS;
  unitDataObj obj_t = { 0 };
  unitDataObj *obj  = &obj_t;
  int len = BASE;
  int ret = 0;

  if (NULL == (obj->data = qsee_malloc(len)))
    GP_ERROR_EXIT(len, ret);

  if (TEE_SUCCESS != (res = qsee_fts_encrypt(obj->data, len, obj->iv, sizeof(obj->iv))))
    GP_ERROR_EXIT(res, ret);

  Exit:
  TEE_Free(obj->data);

  if (0 != ret) {
    qsee_log(QSEE_LOG_MSG_DEBUG, "-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/");
    qsee_log(QSEE_LOG_MSG_DEBUG, "-/- FAILED: gpFTS_Kdf_Unit_Tests() -/-/-/-/-/-/-/-/-/-/-/-/");
    qsee_log(QSEE_LOG_MSG_DEBUG, "-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/");
  }
  else {
    qsee_log(QSEE_LOG_MSG_DEBUG, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    qsee_log(QSEE_LOG_MSG_DEBUG, "++ SUCCESS: gpFTS_Kdf_Unit_Tests() +++++++++++++++++++++++");
    qsee_log(QSEE_LOG_MSG_DEBUG, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  }
  return ret;
}//gpFTS_Kdf_Unit_Test()

/*********************************************************************
 *                  GP_PERSISTENT_OBJ_FUNC_TESTS                     *
 *********************************************************************/

int gpFTS_FileIO_Unit_Tests(void)
{
  TEE_Result res = TEE_SUCCESS;
  int i = 0, j = 0, k = 0, ret = 0;
  uint8_t* data = NULL;
  uint32_t count = 0;

  for (i = 1; i < 10; i++)
  {
    int len = BASE * i;
    if (NULL == (data = qsee_malloc(len)))
      GP_ERROR_EXIT(len, ret);

    qsee_log(QSEE_LOG_MSG_ERROR, "-- gpPersistObjFileIO TESTS ---------------------------------");

    // Negative Test Cases
    if (TEE_SUCCESS ==  qsee_fts_write_file(NULL, data, len, 0, &count))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_write_file(LONG_NAME, data, len, 0, &count))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_write_file(UNIT_OBJ_NAME, NULL, len, 0, &count))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_write_file(UNIT_OBJ_NAME, data, 0, 0, NULL))
      GP_ERROR_EXIT(res, ret);

    if (TEE_SUCCESS ==  qsee_fts_read_file(NULL, data, len, 0, &count))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_read_file(LONG_NAME, data, len, 0, &count))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_read_file(UNIT_OBJ_NAME, NULL, len, 0, &count))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_read_file(UNIT_OBJ_NAME, data, 0, 0, NULL))
      GP_ERROR_EXIT(res, ret);

    if (TEE_SUCCESS ==  qsee_fts_remove_file(LONG_NAME))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_remove_file(NULL))
      GP_ERROR_EXIT(res, ret);

    if (TEE_SUCCESS == qsee_fts_rename_file(LONG_NAME, VALID_NAME))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS == qsee_fts_rename_file(VALID_NAME, LONG_NAME))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS == qsee_fts_rename_file(VALID_NAME, NULL))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS == qsee_fts_rename_file(NULL, VALID_NAME))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS == qsee_fts_rename_file(NULL, NULL))
      GP_ERROR_EXIT(res, ret);

    qsee_log(QSEE_LOG_MSG_ERROR, "-- Negative Tests PASSED ---------------------------------");

    // Typical Test Cases

    // Different Sizes
    for (j = 1; j <= len; j *= 7)
    {
      memset(data, 'T', len/j);

      if (TEE_SUCCESS != (res = qsee_fts_write_file(UNIT_OBJ_NAME, data, len/j, 0, &count)))
        GP_ERROR_EXIT(res, ret);

      if (len/j != count)
        GP_ERROR_EXIT(res, ret);

      memset(data, 0, len/j);

      if (TEE_SUCCESS != (res = qsee_fts_read_file(UNIT_OBJ_NAME, data, len/j, 0, &count)))
        GP_ERROR_EXIT(res, ret);

      if (len/j != count)
        GP_ERROR_EXIT(res, ret);

      for (k = 0; k < len/j; k++)
      {
        if (data[k] != 'T')
          GP_ERROR_EXIT(res, ret);
      }

      if (TEE_SUCCESS != (res = qsee_fts_rename_file(UNIT_OBJ_NAME, UNIT_OBJ_RENAME)))
        GP_ERROR_EXIT(res, ret);

      if (TEE_SUCCESS != (res = qsee_fts_remove_file(UNIT_OBJ_RENAME)))
        GP_ERROR_EXIT(res, ret);
    }

    qsee_log(QSEE_LOG_MSG_ERROR, "-- Typical Tests PASSED ---------------------------------");

    // No file rename
    if (TEE_SUCCESS == (res = qsee_fts_rename_file(UNIT_OBJ_NAME, UNIT_OBJ_RENAME)))
      GP_ERROR_EXIT(res, ret);

    // No file to Read
    if (TEE_SUCCESS == (res = qsee_fts_read_file(UNIT_OBJ_NAME, data, len, 0, &count)))
      GP_ERROR_EXIT(res, ret);

    // Read more than whats in file
    memset(data, 'G', len);

    if (TEE_SUCCESS != (res = qsee_fts_write_file(UNIT_OBJ_NAME, data, len, 0, &count)))
      GP_ERROR_EXIT(res, ret);
    if (len != count)
      GP_ERROR_EXIT(res, ret);

    memset(data, 0, len);

    if (TEE_SUCCESS != (res = qsee_fts_read_file(UNIT_OBJ_NAME, data, len + 10, 0, &count)))
      GP_ERROR_EXIT(res, ret);

    if (len != count)
      GP_ERROR_EXIT(res, ret);

    for (k = 0; k < len; k++)
    {
      if (data[k] != 'G')
        GP_ERROR_EXIT(res, ret);
    }

    if (TEE_SUCCESS != (res = qsee_fts_remove_file(UNIT_OBJ_NAME)))
      GP_ERROR_EXIT(res, ret);
    qsee_log(QSEE_LOG_MSG_ERROR, "-- Edge Case Tests PASSED ---------------------------------");

    qsee_free(data);
    data = NULL;

    qsee_log(QSEE_LOG_MSG_ERROR, "-- gpPersistObjFileIO PASSED ---------------------------------");
  }
Exit:

  if (NULL != data) qsee_free(data);

  if (TEE_SUCCESS != ret) {
    qsee_log(QSEE_LOG_MSG_DEBUG, "-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/");
    qsee_log(QSEE_LOG_MSG_DEBUG, "-/- FAILED: gpFTS_FileIO_Unit_Tests() -/-/-/-/-/-/-/-/-/-/-/-/");
    qsee_log(QSEE_LOG_MSG_DEBUG, "-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/");
  }
  else {
    qsee_log(QSEE_LOG_MSG_DEBUG, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    qsee_log(QSEE_LOG_MSG_DEBUG, "++ SUCCESS: gpFTS_FileIO_Unit_Tests() +++++++++++++++++++++++");
    qsee_log(QSEE_LOG_MSG_DEBUG, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  }

  return ret;
}//gpFTS_FileIO_Unit_Tests()

int gpFTS_Crypto_Unit_Tests(void)
{
  TEE_Result res = TEE_SUCCESS;
  unitDataObj obj_t = { 0 };
  unitDataObj *obj  = &obj_t;
  int i = 0, j = 0, ret = 0;

  for (i = 1; i < 10; i++)
  {
    int len = BASE * i;
    if (NULL == (obj->data = qsee_malloc(len)))
      GP_ERROR_EXIT(len, ret);

    qsee_log(QSEE_LOG_MSG_ERROR, "-- gpPersistObjCrypto TESTS ---------------------------------");

    // Negative Test Cases
    if (TEE_SUCCESS == qsee_fts_encrypt(NULL, len, obj->iv, sizeof(obj->iv)))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS == qsee_fts_encrypt(obj->data, 0, obj->iv, sizeof(obj->iv)))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_encrypt(obj->data, 0, obj->iv, sizeof(obj->iv)))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_encrypt(obj->data, len, NULL, sizeof(obj->iv)))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_encrypt(obj->data, len, obj->iv, QSEE_FTS_CRYPTO_IV_SIZE - 1))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_encrypt(obj->data, len, obj->iv, 0))
      GP_ERROR_EXIT(res, ret);

    if (TEE_SUCCESS ==  qsee_fts_decrypt(NULL, len, obj->iv, sizeof(obj->iv)))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_decrypt(obj->data, 0, obj->iv, sizeof(obj->iv)))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_decrypt(obj->data, 0, obj->iv, sizeof(obj->iv)))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_decrypt(obj->data, len, NULL, sizeof(obj->iv)))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_decrypt(obj->data, len, obj->iv, QSEE_FTS_CRYPTO_IV_SIZE - 1))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_decrypt(obj->data, len, obj->iv, 0))
      GP_ERROR_EXIT(res, ret);

    if (TEE_SUCCESS ==  qsee_fts_integrity_protect(NULL, len, NULL, 0, obj->hmac, sizeof(obj->hmac)))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_integrity_protect(obj->data, 0, NULL, 0, obj->hmac, sizeof(obj->hmac)))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_integrity_protect(obj->data, len, obj->hash, 0, obj->hmac, sizeof(obj->hmac)))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_integrity_protect(obj->data, len, NULL, 0, obj->hmac, 0))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_integrity_protect(obj->data, len, obj->hash, 0, obj->hmac, 0))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_integrity_protect(obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac) - 1))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_integrity_protect(obj->data, len, obj->hash, sizeof(obj->hash) - 1, obj->hmac, sizeof(obj->hmac)))
      GP_ERROR_EXIT(res, ret);

    if (TEE_SUCCESS ==  qsee_fts_integrity_verify(NULL, len, NULL, 0, obj->hmac, sizeof(obj->hmac)))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_integrity_verify(obj->data, 0, NULL, 0, obj->hmac, sizeof(obj->hmac)))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_integrity_verify(obj->data, len, obj->hash, 0, obj->hmac, sizeof(obj->hmac)))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_integrity_verify(obj->data, len, NULL, 0, obj->hmac, 0))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_integrity_verify(obj->data, len, obj->hash, 0, obj->hmac, 0))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_integrity_verify(obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac) - 1))
      GP_ERROR_EXIT(res, ret);
    if (TEE_SUCCESS ==  qsee_fts_integrity_verify(obj->data, len, obj->hash, sizeof(obj->hash) - 1, obj->hmac, sizeof(obj->hmac)))
      GP_ERROR_EXIT(res, ret);

    qsee_log(QSEE_LOG_MSG_ERROR, "-- Negative Tests PASSED ---------------------------------");

    // Typical Cases
    memset(obj->data, 'T', len);

    if (TEE_SUCCESS != (res = qsee_fts_encrypt(obj->data, len, obj->iv, sizeof(obj->iv))))
      GP_ERROR_EXIT(res, ret);

    if (TEE_SUCCESS != (res = qsee_fts_decrypt(obj->data, len, obj->iv, sizeof(obj->iv))))
      GP_ERROR_EXIT(res, ret);

    for (j = 0; j < len; j++)
    {
      if ((obj->data)[j] != 'T')
        GP_ERROR_EXIT(res, ret);
    }

    if (TEE_SUCCESS != (res = qsee_fts_integrity_protect(obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac))))
      GP_ERROR_EXIT(res, ret);

    if (TEE_SUCCESS != (res = qsee_fts_integrity_verify(obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac))))
      GP_ERROR_EXIT(res, ret);

    if (TEE_SUCCESS != (res = qsee_fts_integrity_protect(obj->data, len, obj->hash, sizeof(obj->hash), NULL, 0)))
      GP_ERROR_EXIT(res, ret);

    if (TEE_SUCCESS != (res = qsee_fts_integrity_verify(obj->data, len, obj->hash, sizeof(obj->hash), NULL, 0)))
      GP_ERROR_EXIT(res, ret);

    // Different Sizes
    for (j = 1; j <= len; j*=7)
    {
      if (TEE_SUCCESS != (res = qsee_fts_encrypt(obj->data, len/j, obj->iv, sizeof(obj->iv))))
        GP_ERROR_EXIT(res, ret);

      if (TEE_SUCCESS != (res = qsee_fts_decrypt(obj->data, len/j, obj->iv, sizeof(obj->iv))))
        GP_ERROR_EXIT(res, ret);

      if (TEE_SUCCESS != (res = qsee_fts_integrity_protect(obj->data, len/j, NULL, 0, obj->hmac, sizeof(obj->hmac))))
        GP_ERROR_EXIT(res, ret);

      if (TEE_SUCCESS != (res = qsee_fts_integrity_verify(obj->data, len/j, NULL, 0, obj->hmac, sizeof(obj->hmac))))
        GP_ERROR_EXIT(res, ret);

      if (TEE_SUCCESS != (res = qsee_fts_integrity_protect(obj->data, len / j, obj->hash, sizeof(obj->hash), NULL, 0)))
        GP_ERROR_EXIT(res, ret);

      if (TEE_SUCCESS != (res = qsee_fts_integrity_verify(obj->data, len / j, obj->hash, sizeof(obj->hash), NULL, 0)))
        GP_ERROR_EXIT(res, ret);
    }

    qsee_log(QSEE_LOG_MSG_ERROR, "-- Typical Tests PASSED ---------------------------------");

    // Corruption Case
    if (TEE_SUCCESS != (res = qsee_fts_integrity_protect(obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac))))
      GP_ERROR_EXIT(res, ret);

    memset(obj->data, 'Y', len / 2);

    if (TEE_SUCCESS == (res = qsee_fts_integrity_verify(obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac))))
      GP_ERROR_EXIT(res, ret);

    if (TEE_SUCCESS != (res = qsee_fts_integrity_protect(obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac))))
      GP_ERROR_EXIT(res, ret);

    memset(obj->data + len - 1, 1, 1);

    if (TEE_SUCCESS == (res = qsee_fts_integrity_verify(obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac))))
      GP_ERROR_EXIT(res, ret);

    if (TEE_SUCCESS != (res = qsee_fts_integrity_protect(obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac))))
      GP_ERROR_EXIT(res, ret);

    memset(obj->hmac, 2, 1);

    if (TEE_SUCCESS == (res = qsee_fts_integrity_verify(obj->data, len, NULL, 0, obj->hmac, sizeof(obj->hmac))))
      GP_ERROR_EXIT(res, ret);

    if (TEE_SUCCESS != (res = qsee_fts_integrity_protect(obj->data, len, obj->hash, sizeof(obj->hash), NULL, 0)))
      GP_ERROR_EXIT(res, ret);

    memset(obj->data, 'W', len / 2);

    if (TEE_SUCCESS == (res = qsee_fts_integrity_verify(obj->data, len, obj->hash, sizeof(obj->hash), NULL, 0)))
      GP_ERROR_EXIT(res, ret);

    if (TEE_SUCCESS != (res = qsee_fts_integrity_protect(obj->data, len, obj->hash, sizeof(obj->hash), NULL, 0)))
      GP_ERROR_EXIT(res, ret);

    memset(obj->data + len - 1, 3, 1);

    if (TEE_SUCCESS == (res = qsee_fts_integrity_verify(obj->data, len, obj->hash, sizeof(obj->hash), NULL, 0)))
      GP_ERROR_EXIT(res, ret);

    if (TEE_SUCCESS != (res = qsee_fts_integrity_protect(obj->data, len, obj->hash, sizeof(obj->hash), NULL, 0)))
      GP_ERROR_EXIT(res, ret);

    memset(obj->hash, 4, 1);

    if (TEE_SUCCESS == (res = qsee_fts_integrity_verify(obj->data, len, obj->hash, sizeof(obj->hash), NULL, 0)))
      GP_ERROR_EXIT(res, ret);

    qsee_log(QSEE_LOG_MSG_ERROR, "-- Corruption Tests PASSED ---------------------------------");

    qsee_free(obj->data);
    obj->data = NULL;

    qsee_log(QSEE_LOG_MSG_ERROR, "-- gpPersistObjCrypto PASSED ---------------------------------");
  }
Exit:

  if (NULL != obj->data) qsee_free(obj->data);

  if (TEE_SUCCESS != ret) {
    qsee_log(QSEE_LOG_MSG_DEBUG, "-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/");
    qsee_log(QSEE_LOG_MSG_DEBUG, "-/- FAILED: gpFTS_Crypto_Unit_Tests() -/-/-/-/-/-/-/-/-/-/-/-/");
    qsee_log(QSEE_LOG_MSG_DEBUG, "-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/");
  }
  else {
    qsee_log(QSEE_LOG_MSG_DEBUG, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    qsee_log(QSEE_LOG_MSG_DEBUG, "++ SUCCESS: gpFTS_Crypto_Unit_Tests() +++++++++++++++++++++++");
    qsee_log(QSEE_LOG_MSG_DEBUG, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  }

  return ret;
}//gpFTS_Crypto_Unit_Tests()

int gpFTS_Basic_Use_Case(void)
{
  int res = TEE_SUCCESS;
  int ret        = 0;
  uint32_t count = 0;
  dataObj* obj   = NULL;

  do
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "-- gpFTS_Basic_Use_Case TESTS ---------------------------------");

    // Run Typical Application Use Case
    if (NULL == (obj = qsee_malloc(sizeof(dataObj))))
      GP_ERROR_BREAK(sizeof(dataObj), ret);

    // Perform Encrypt and Protect
    if (TEE_SUCCESS != (res = qsee_fts_encrypt(obj->data, sizeof(obj->data), obj->iv, sizeof(obj->iv))))
      GP_ERROR_BREAK(res, ret);

    if (TEE_SUCCESS != (res = qsee_fts_integrity_protect(obj->data, sizeof(obj->data)+sizeof(obj->iv), NULL, 0, obj->hmac, sizeof(obj->hmac))))
      GP_ERROR_BREAK(res, ret);

    // Store Data
    if (TEE_SUCCESS != (res = qsee_fts_write_file(TEST_OBJ_NAME, obj, sizeof(*obj), 0, &count)))
      GP_ERROR_BREAK(res, ret);

    // Read Data
    if (TEE_SUCCESS != (res = qsee_fts_read_file(TEST_OBJ_NAME, obj, sizeof(*obj), 0, &count)))
      GP_ERROR_BREAK(res, ret);

    // Verify and Decrypt
    if (TEE_SUCCESS != (res = qsee_fts_integrity_verify(obj->data, sizeof(obj->data) + sizeof(obj->iv), NULL, 0, obj->hmac, sizeof(obj->hmac))))
      GP_ERROR_BREAK(res, ret);

    if (TEE_SUCCESS != (res = qsee_fts_decrypt(obj->data, sizeof(obj->data), obj->iv, sizeof(obj->iv))))
      GP_ERROR_BREAK(res, ret);

    // Remove the Object
    if (TEE_SUCCESS != (res = qsee_fts_remove_file(TEST_OBJ_NAME)))
      GP_ERROR_BREAK(res, ret);

    qsee_log(QSEE_LOG_MSG_ERROR, "-- gpFTS_Basic_Use_Case TESTS PASSED");

  } while (0);

  qsee_log(QSEE_LOG_MSG_ERROR, "-- gpFTS_Basic_Use_Case TESTS ---------------------------------");

  if (NULL != obj) qsee_free(obj);

  if (TEE_SUCCESS != ret) {
    qsee_log(QSEE_LOG_MSG_DEBUG, "-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/");
    qsee_log(QSEE_LOG_MSG_DEBUG, "-/- FAILED: gpFTS_Basic_Use_Case() -/-/-/-/-/-/-/-/-/-/-/-/");
    qsee_log(QSEE_LOG_MSG_DEBUG, "-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/");
  }
  else {
    qsee_log(QSEE_LOG_MSG_DEBUG, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    qsee_log(QSEE_LOG_MSG_DEBUG, "++ SUCCESS: gpFTS_Basic_Use_Case() +++++++++++++++++++++++");
    qsee_log(QSEE_LOG_MSG_DEBUG, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  }

  return ret;
}//gpFTS_Basic_Use_Case()

int gpFTS_Test_Suite(void)
{
  int ret = 0;

  do
  {
    // First Run Unit Tests
    if (0 != (ret = gpFTS_FileIO_Unit_Tests()))
    {
      GPLOG_ERROR(ret);
      break;
    }

    if (0 != (ret = gpFTS_Crypto_Unit_Tests()))
    {
      GPLOG_ERROR(ret);
      break;
    }

    if (0 != (ret = gpFTS_Basic_Use_Case()))
    {
      GPLOG_ERROR(ret);
      break;
    }

  } while (0);

  return ret;
}//gpFTS_Test_Suite()

/*********************************************************************
 *                  GP_PERSISTENT_OBJ_PERF_TESTS                     *
 *********************************************************************/
int gpFTS_Performance_Tests()
{
  TEE_Result res = TEE_SUCCESS;
  int ret        = 0;
  uint32_t count   = 0;
  dataObj* obj   = NULL;
  unsigned long long start = 0;
  unsigned long long stop  = 0;
  unsigned long long total = 0;
  int i = 0;

  qsee_log(QSEE_LOG_MSG_ERROR, "-- gpPersistObj Perf TESTS ---------------------------------");

  do
  {

    if (NULL == (obj = qsee_malloc(sizeof(dataObj))))
      GP_ERROR_EXIT(sizeof(dataObj), ret);

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      // Perform Encrypt
      if (TEE_SUCCESS != (res = qsee_fts_encrypt(obj->data, sizeof(obj->data), obj->iv, sizeof(obj->iv))))
        GP_ERROR_EXIT(res, ret);

      stop = qsee_get_uptime();
      total += stop - start;
    }

    qsee_log(QSEE_LOG_MSG_ERROR, "  --> Encrypt (Without Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      // Perform Protect
      if (TEE_SUCCESS != (res = qsee_fts_integrity_protect(obj->data, sizeof(obj->data) + sizeof(obj->iv), NULL, 0, obj->hmac, sizeof(obj->hmac))))
        GP_ERROR_EXIT(res, ret);

      stop = qsee_get_uptime();
      total += stop - start;
    }

    qsee_log(QSEE_LOG_MSG_ERROR, "  --> Protect (Without Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      // Perform Write
      if (TEE_SUCCESS != (res = qsee_fts_write_file(TEST_OBJ_NAME, obj, sizeof(*obj), 0, &count)))
        GP_ERROR_EXIT(res, ret);

      stop = qsee_get_uptime();
      total += stop - start;
    }

    qsee_log(QSEE_LOG_MSG_ERROR, "  --> Write (Without Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      // Perform Read
      if (TEE_SUCCESS != (res = qsee_fts_read_file(TEST_OBJ_NAME, obj, sizeof(*obj), 0, &count)))
        GP_ERROR_BREAK(res, ret);

      stop = qsee_get_uptime();
      total += stop - start;
    }

    qsee_log(QSEE_LOG_MSG_ERROR, "  --> Read (Without Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      // Perform Verify
      if (TEE_SUCCESS != (res = qsee_fts_integrity_verify(obj->data, sizeof(obj->data) + sizeof(obj->iv), NULL, 0, obj->hmac, sizeof(obj->hmac))))
        GP_ERROR_BREAK(res, ret);

      stop = qsee_get_uptime();
      total += stop - start;
    }

    qsee_log(QSEE_LOG_MSG_ERROR, "  --> Verify (Without Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      // Perform Decrypt
      if (TEE_SUCCESS != (res = qsee_fts_decrypt(obj->data, sizeof(obj->data), obj->iv, sizeof(obj->iv))))
        GP_ERROR_BREAK(res, ret);

      stop = qsee_get_uptime();
      total += stop - start;
    }

    qsee_log(QSEE_LOG_MSG_ERROR, "  --> Decrypt (Without Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      // Perform Encrypt and Protect
      if (TEE_SUCCESS != (res = qsee_fts_encrypt(obj->data, sizeof(obj->data), obj->iv, sizeof(obj->iv))))
        GP_ERROR_EXIT(res, ret);

      if (TEE_SUCCESS != (res = qsee_fts_integrity_protect(obj->data, sizeof(obj->data) + sizeof(obj->iv), NULL, 0, obj->hmac, sizeof(obj->hmac))))
        GP_ERROR_EXIT(res, ret);

      // Store Data 
      if (TEE_SUCCESS != (res = qsee_fts_write_file(TEST_OBJ_NAME, obj, sizeof(*obj), 0, &count)))
        GP_ERROR_EXIT(res, ret);

      stop = qsee_get_uptime();
      total += stop - start;
    }

    qsee_log(QSEE_LOG_MSG_ERROR, "  --> Encrypt, Protect, Write (Without Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      // Read Data 
      if (TEE_SUCCESS != (res = qsee_fts_read_file(TEST_OBJ_NAME, obj, sizeof(*obj), 0, &count)))
        GP_ERROR_BREAK(res, ret);

      // Verify and Decrypt
      if (TEE_SUCCESS != (res = qsee_fts_integrity_verify(obj->data, sizeof(obj->data) + sizeof(obj->iv), NULL, 0, obj->hmac, sizeof(obj->hmac))))
        GP_ERROR_BREAK(res, ret);

      if (TEE_SUCCESS != (res = qsee_fts_decrypt(obj->data, sizeof(obj->data), obj->iv, sizeof(obj->iv))))
        GP_ERROR_BREAK(res, ret);

      stop = qsee_get_uptime();
      total += stop - start;
    }

    qsee_log(QSEE_LOG_MSG_ERROR, "  --> Read, Verify, Decrypt (Without Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      // Perform Encrypt
      if (TEE_SUCCESS != (res = qsee_fts_encrypt(obj->data, sizeof(obj->data), obj->iv, sizeof(obj->iv))))
        GP_ERROR_EXIT(res, ret);

      stop = qsee_get_uptime();
      total += stop - start;
    }

    qsee_log(QSEE_LOG_MSG_ERROR, "  --> Encrypt (With Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      // Perform Protect
      if (TEE_SUCCESS != (res = qsee_fts_integrity_protect(obj->data, sizeof(obj->data) + sizeof(obj->iv), NULL, 0, obj->hmac, sizeof(obj->hmac))))
        GP_ERROR_EXIT(res, ret);

      stop = qsee_get_uptime();
      total += stop - start;
    }

    qsee_log(QSEE_LOG_MSG_ERROR, "  --> Protect (With Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      // Perform Write
      if (TEE_SUCCESS != (res = qsee_fts_write_file(TEST_OBJ_NAME, obj, sizeof(*obj), 0, &count)))
        GP_ERROR_EXIT(res, ret);

      stop = qsee_get_uptime();
      total += stop - start;
    }

    qsee_log(QSEE_LOG_MSG_ERROR, "  --> Write (With Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      // Perform Read
      if (TEE_SUCCESS != (res = qsee_fts_read_file(TEST_OBJ_NAME, obj, sizeof(*obj), 0, &count)))
        GP_ERROR_EXIT(res, ret);

      stop = qsee_get_uptime();
      total += stop - start;
    }

    qsee_log(QSEE_LOG_MSG_ERROR, "  --> Read (With Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      // Perform Verify
      if (TEE_SUCCESS != (res = qsee_fts_integrity_verify(obj->data, sizeof(obj->data) + sizeof(obj->iv), NULL, 0, obj->hmac, sizeof(obj->hmac))))
        GP_ERROR_EXIT(res, ret);

      stop = qsee_get_uptime();
      total += stop - start;
    }

    qsee_log(QSEE_LOG_MSG_ERROR, "  --> Verify (With Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      // Perform Decrypt
      if (TEE_SUCCESS != (res = qsee_fts_decrypt(obj->data, sizeof(obj->data), obj->iv, sizeof(obj->iv))))
        GP_ERROR_EXIT(res, ret);

      stop = qsee_get_uptime();
      total += stop - start;
    }

    qsee_log(QSEE_LOG_MSG_ERROR, "  --> Decrypt (With Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      // Perform Encrypt and Protect
      if (TEE_SUCCESS != (res = qsee_fts_encrypt(obj->data, sizeof(obj->data), obj->iv, sizeof(obj->iv))))
        GP_ERROR_EXIT(res, ret);

      if (TEE_SUCCESS != (res = qsee_fts_integrity_protect(obj->data, sizeof(obj->data) + sizeof(obj->iv), NULL, 0, obj->hmac, sizeof(obj->hmac))))
        GP_ERROR_EXIT(res, ret);

      // Store Data
      if (TEE_SUCCESS != (res = qsee_fts_write_file(TEST_OBJ_NAME, obj, sizeof(*obj), 0, &count)))
        GP_ERROR_EXIT(res, ret);

      stop = qsee_get_uptime();
      total += stop - start;
    }

    qsee_log(QSEE_LOG_MSG_ERROR, "  --> Encrypt, Protect, Write (With Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    for (i = 0; i < ITERS; i++)
    {
      start = qsee_get_uptime();

      // Read Data
      if (TEE_SUCCESS != (res = qsee_fts_read_file(TEST_OBJ_NAME, obj, sizeof(*obj), 0, &count)))
        GP_ERROR_EXIT(res, ret);

      // Verify and Decrypt
      if (TEE_SUCCESS != (res = qsee_fts_integrity_verify(obj->data, sizeof(obj->data) + sizeof(obj->iv), NULL, 0, obj->hmac, sizeof(obj->hmac))))
        GP_ERROR_EXIT(res, ret);

      if (TEE_SUCCESS != (res = qsee_fts_decrypt(obj->data, sizeof(obj->data), obj->iv, sizeof(obj->iv))))
        GP_ERROR_EXIT(res, ret);

      stop = qsee_get_uptime();
      total += stop - start;
    }

    qsee_log(QSEE_LOG_MSG_ERROR, "  --> Read, Verify, Decrypt (With Clocks): avg=%llu ms", total / ITERS);
    total = 0;

    qsee_log(QSEE_LOG_MSG_ERROR, "-- gpPersistObj Perf PASSED ---------------------------------");

  }while (0);
Exit:

  if (NULL != obj) qsee_free(obj);

  if (TEE_SUCCESS != ret) {
    qsee_log(QSEE_LOG_MSG_DEBUG, "-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/");
    qsee_log(QSEE_LOG_MSG_DEBUG, "-/- FAILED: gpFTS_Performance_Tests() -/-/-/-/-/-/-/-/-/-/-/-/");
    qsee_log(QSEE_LOG_MSG_DEBUG, "-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/");
  }
  else {
    qsee_log(QSEE_LOG_MSG_DEBUG, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    qsee_log(QSEE_LOG_MSG_DEBUG, "++ SUCCESS: gpFTS_Performance_Tests() +++++++++++++++++++++++");
    qsee_log(QSEE_LOG_MSG_DEBUG, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  }

  return ret;
}//gpFTS_Performance_Tests()

/** External functions **/

int run_persistent_obj_dep_tests(void)
{
  int res = 0;
	
  res = gpFTS_Kdf_Unit_Test();
  return res;  
}

int run_persistent_obj_func_tests(void)
{
  int res = 0;
  
  res = gpFTS_Test_Suite();

  return res;
}

int run_persistent_obj_perf_tests(void)
{
  int res = 0;
  res = gpFTS_Performance_Tests();

  return res;
}
