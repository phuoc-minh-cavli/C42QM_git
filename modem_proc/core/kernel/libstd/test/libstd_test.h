/*===============================================================================================
 * FILE:        libstd_test.h
 *
 * DESCRIPTION: Internal header file for libstd tests
 *
 *              Copyright (c) 1999 - 2010 Qualcomm Technologies Incorporated.
 *              All Rights Reserved. QUALCOMM Proprietary and Confidential.
 *===============================================================================================*/

/*===============================================================================================
 *
 *                            Edit History
 * $Header: //components/rel/core.mpss/3.10/kernel/libstd/test/libstd_test.h#1 $ 
 * $DateTime: 2018/06/26 03:18:17 $
 *
 *===============================================================================================*/
#ifndef __LIBSTD_TEST_H__
#define __LIBSTD_TEST_H__

#include "comdef.h"

/* LIBSTD Test Config Items */
#define LIBSTD_FUNC_NAME_LEN            32
#define TEST_BUFFER_SIZE                64
#define LIBSTD_TEST_RECORD_ARRAY_SIZE   50

int libstd_start_tests(void);
int libstd_test_log_init(void);
int libstd_test_log_destroy(void);
int libstd_log_result(int test_number, boolean result, int line_number, const char *function_name);

#define LIBSTD_RECORD_TEST(test_number, result) \
            libstd_log_result(test_number, result, __LINE__, __func__)

#endif //__LIBSTD_TEST_H__
