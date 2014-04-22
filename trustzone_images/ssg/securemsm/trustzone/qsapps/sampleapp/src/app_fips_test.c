/*
  @file app_fips_test.c
  @brief This file is sample code that show how QSEE fips services APIs can be used.
*/

#include "IxErrno.h"
#include "qsee_log.h"
#include "qsee_fips_services.h"
#include "stdio.h"
#include "qsee_heap.h"

static void print_hmac(uint8_t* hmac, size_t len)
{
    size_t out_buf_len = (len * 2) + 1;
    char* out = qsee_malloc(out_buf_len);
    if (out == NULL)
    {
        return;
    }

    for (size_t i = 0; i < len; i++)
    {
        snprintf(out + (i * 2), out_buf_len - (i * 2), "%02x", hmac[i]);
    }

    qsee_log(QSEE_LOG_MSG_DEBUG, "FIPS Module HMAC: %s", out);
    qsee_free(out);
}

int run_fips_services_test(void)
{
    uint8_t hmac[32] = {0};
	size_t len = 0;
    uint32_t fuse_status;
    uint32_t state;
    uint32_t enablement;
    size_t out_len;
    int ret = 0;

    // This will fail if the HMAC of the crypto module is not present, or the integrity check does not pass.
    ret = qsee_get_fips_info(QSEE_FIPS_MODULE_HMAC, hmac, sizeof(hmac), &out_len);
    if (ret != QSEE_FIPS_SUCCESS)
    {
        qsee_log(QSEE_LOG_MSG_ERROR, "ERROR: qsee_get_fips_information failed to get HMAC");
        return E_FAILURE;
    }

    print_hmac(hmac, sizeof(hmac));

    ret = qsee_get_fips_info(QSEE_FIPS_FUSE_STATUS, &fuse_status, sizeof(fuse_status), &out_len);
    if (ret != QSEE_FIPS_SUCCESS)
    {
        qsee_log(QSEE_LOG_MSG_ERROR, "ERROR: qsee_get_fips_information failed to get selftest fuse status");
        return E_FAILURE;
    }

    ret = qsee_get_fips_info(QSEE_FIPS_ENABLEMENT, &enablement, sizeof(enablement), &out_len);
    if (ret != QSEE_FIPS_SUCCESS)
    {
        qsee_log(QSEE_LOG_MSG_ERROR, "ERROR: qsee_get_fips_information failed to get selftest level");
        return E_FAILURE;
    }

    ret = qsee_get_fips_info(QSEE_FIPS_SELFTEST_STATUS, &state, sizeof(state), &out_len);
    if (ret != QSEE_FIPS_SUCCESS)
    {
        qsee_log(QSEE_LOG_MSG_ERROR, "ERROR: qsee_get_fips_information failed to get selftest status");
        return E_FAILURE;
    }

    qsee_log(QSEE_LOG_MSG_DEBUG, "FIPS fuse status: %d", fuse_status);
    qsee_log(QSEE_LOG_MSG_DEBUG, "FIPS enablement status: %d", enablement);
    qsee_log(QSEE_LOG_MSG_DEBUG, "FIPS selftest state: %d", state);

    // Since a selftest should be enabled in the sampleapp, the crypto selftest should be run
    // This does not return an error because in modem targets it will not be run
    if (state == QSEE_CRYPTO_SELFTEST_NOT_RUN)
    {
        qsee_log(QSEE_LOG_MSG_ERROR, "ERROR: Selftest is enabled, but was not run");
    }
    else if (state == QSEE_CRYPTO_SELFTEST_PASSED)
    {
        qsee_log(QSEE_LOG_MSG_DEBUG, "Crypto selftest passed");
    }
    else
    {
        qsee_log(QSEE_LOG_MSG_ERROR, "Invalid state from qsee_get_selftest_status_information: %d", state);
        return E_FAILURE;
    }

    return E_SUCCESS;
}