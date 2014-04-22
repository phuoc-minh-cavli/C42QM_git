/*
  @file app_main.cpp
  @brief App main entry point.

*/
/*===========================================================================
  Copyright (c) 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

#include <string> //for string class
#include "cheaders.h"
#include "cppFeatures.h"
#include "samplecppapp.h"

using namespace std;

#define LOG_ERROR(callee)                                                      \
  qsee_log(QSEE_LOG_MSG_ERROR, "[%s] FAILED %s at line %d", __func__, #callee, \
           __LINE__)

#define SAMPLE_CPP_APP_VERSION_MAJOR 0x1
#define SAMPLE_CPP_APP_VERSION_MINOR 0x0

/**
   @brief
   Data structure

   @param[in]   cmd_id      Requested command
   @param[in]   data        information (could be data or a pointer to the
   memory that holds the data
   @param[in]   len         if data is ptr to some buffer, len indicates length
   of the buffer
   @param[in]   test_buf_size  When running crypto test, this indicates the test
   packet size
*/

typedef struct send_cmd {
  uint32_t cmd_id;
  uintptr_t data;
  uintptr_t data2;
  uint32_t len;
  uint32_t start_pkt;
  uint32_t end_pkt;
  uint32_t test_buf_size;
} send_cmd_t;

typedef struct send_cmd_rsp {
  uint32_t data;
  int32_t status;
} send_cmd_rsp_t;

int smplap_sfs_unit_test_suite(void);
int tz_app_sfs(void);
int run_smplserv_test(void);
int run_smplcert_test(void);

/*
* Globals
*/
static uint8_t qsee_log_mask;

/**
  @brief
  Add any app specific initialization code here
  QSEE will call this function after secure app is loaded and
  authenticated
*/
void tz_app_init(void) {
  //  App specific initialization code
  qsee_log(QSEE_LOG_MSG_DEBUG, "%s completed. ", __func__);
  qsee_log(QSEE_LOG_MSG_DEBUG, "Version: %x.%x", SAMPLE_CPP_APP_VERSION_MAJOR,
           SAMPLE_CPP_APP_VERSION_MINOR);
}

/*Need to move into qsee_api_tests file.*/
int run_abort_test(void) {
  volatile uint32_t *my_bad_ptr = NULL;
  qsee_log(QSEE_LOG_MSG_DEBUG, "Sampleapp about to abort");
  *my_bad_ptr = 5;
  return *my_bad_ptr;
}

/**
  @brief
  Command Handler
 */
void tz_app_cmd_handler(void *cmd, uint32_t cmdlen, void *rsp,
                        uint32_t rsplen) {
  /* Request-response buffers are allocated by non-secure side*/
  /* Add code to process requests and set response (if any)*/
  int retval;
  int j;
  uint32_t i = 0;
  uint32_t cmd_id;
  uint32_t expected_cmdlen;
  uint32_t expected_rpslen;
  unsigned char *ptr;
  struct send_cmd *cmd_ptr = (struct send_cmd *)cmd;
  struct send_cmd_rsp *rsp_ptr = (struct send_cmd_rsp *)rsp;
  uint32_t list_test_req = 0;
  uint32_t list_test_rsp = 0;

  /*First we check if the response pointer is large enough to support a
   * pass/fail response*/
  if (rsplen < sizeof(send_cmd_rsp_t)) {
    qsee_log(QSEE_LOG_MSG_FATAL, "Response buffer len insufficient, ERROR OUT");
    return;
  }

  /* sanity check */
  if ((!cmd_ptr) || (!rsp_ptr)) {
    qsee_log(QSEE_LOG_MSG_FATAL, "Command pointer equals NULL!");
    return;
  }
  /**Determine the command id.
   * We check if the command buffer is large enough to support the
   * uint32_t read for cmd_id.
   * It is assumed that the first member of the command buffer is the
   * cmd_id
   */
  if (cmdlen < sizeof(uint32_t)) {
    qsee_log(QSEE_LOG_MSG_FATAL,
             "Command buffer len insufficient for reading cmd_id, ERROR OUT");
    rsp_ptr->status = FAIL_CMD_BUF_TOO_SMALL_FOR_CMD_ID;
    return;
  }
  cmd_id = cmd_ptr->cmd_id;
  qsee_log(QSEE_LOG_MSG_DEBUG, "TZ App cmd handler, cmd_id = %d", cmd_id);

  expected_cmdlen = sizeof(send_cmd_t);
  expected_rpslen = sizeof(send_cmd_rsp_t);

  /*Validate the command buffer and response buffer are the correct size.
    If not, the the MPU protection and ns_range checks done by QSEE kernel might
    be insufficient*/
  if (cmdlen < expected_cmdlen || rsplen < expected_rpslen) {
    qsee_log(QSEE_LOG_MSG_FATAL,
             "Cmd/rsp buffer lens insufficient - %x, %x, ERROR OUT", cmdlen,
             rsplen);
    rsp_ptr->status = FAIL_CMD_RSP_BUFS_TOO_SMALL;
    return;
  }

  switch (cmd_id) {
  case CLIENT_CMD0_GET_VERSION:
    /* Return major version of the sample test app*/
    rsp_ptr->data = SAMPLE_CPP_APP_VERSION_MAJOR;
    rsp_ptr->status = SUCCESS;
    qsee_log(QSEE_LOG_MSG_DEBUG, "****PASSED : get version test");
    break;

  case CLIENT_CMD1_BASIC_DATA:
    /*Modify response by 10 */
    rsp_ptr->data = ((uint32_t)cmd_ptr->data) * 10;
    qsee_log(QSEE_LOG_MSG_FATAL, "cmd_ptr->data%u", cmd_ptr->data);
    rsp_ptr->status = SUCCESS;
    qsee_log(QSEE_LOG_MSG_DEBUG, "****PASSED : Basic data test");
    break;

  case CLIENT_CMD5_RUN_SFS_TEST:
    if (smplap_sfs_unit_test_suite()) {
      rsp_ptr->status = FAILED;
      qsee_log(QSEE_LOG_MSG_ERROR, "****FAILED : SFS unit Test");
      break;
    }
    if (tz_app_sfs()) {
      rsp_ptr->status = FAILED;
      qsee_log(QSEE_LOG_MSG_ERROR, "****FAILED : SFS unit Test");
      break;
    }

    rsp_ptr->status = SUCCESS;
    qsee_log(QSEE_LOG_MSG_DEBUG, "****PASSED : SFS unit Test");
    break;

  case CLIENT_CMD32_RUN_ABORT_TEST:
    run_abort_test();
    break;

  case CLIENT_CMD27_RUN_SMPLSERV_TEST:
     retval = run_smplserv_test();
     if (retval) {
        rsp_ptr->status = FAILED;
        qsee_log(QSEE_LOG_MSG_ERROR, "****FAILED : Smplserv Test");
     } else {
        rsp_ptr->status = SUCCESS;
        qsee_log(QSEE_LOG_MSG_ERROR, "****PASSED : Smplserv Test");
     }
     break;
  case CLIENT_CMD33_RUN_SMPLCERT_TEST:
     retval = run_smplcert_test();
     if (retval) {
        rsp_ptr->status = FAILED;
        qsee_log(QSEE_LOG_MSG_ERROR, "****FAILED : Smplcert Test");
     } else {
        rsp_ptr->status = SUCCESS;
        qsee_log(QSEE_LOG_MSG_ERROR, "****PASSED : Smplcert Test");
     }
     break;

  case CLIENT_CMD121_CPP_FEATURE_TEST:
    retval = cppFeatures();
    if (retval) {
      rsp_ptr->status = FAILED;
      qsee_log(QSEE_LOG_MSG_ERROR, "****FAILED : CPP_FEATURE_TEST");
    } else {
      rsp_ptr->status = SUCCESS;
      qsee_log(QSEE_LOG_MSG_DEBUG, "****PASSED : CPP_FEATURE_TEST");
    }
    break;

  default: {
    rsp_ptr->status = SUCCESS;
    qsee_log(
        QSEE_LOG_MSG_ERROR,
        "Unsupported command: %d.  Test not yet implemented or commented out.",
        cmd_ptr->cmd_id);
    break;
  }
  }
}

/**
  @brief
  App specific shutdown
  App will be given a chance to shutdown gracefully
*/
void tz_app_shutdown(void) {
  // app specific shutdown code
  qsee_log(QSEE_LOG_MSG_DEBUG, "%s completed.", __func__);
  return;
}
