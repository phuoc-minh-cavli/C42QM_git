/**
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __SAMPLECPPAPP_H
#define __SAMPLECPPAPP_H


/* commands supported from sample client */
#define CLIENT_CMD0_GET_VERSION                               0
#define CLIENT_CMD1_BASIC_DATA                                1
#define CLIENT_CMD5_RUN_SFS_TEST                              5
#define CLIENT_CMD27_RUN_SMPLSERV_TEST                        27
#define CLIENT_CMD32_RUN_ABORT_TEST                           32
#define CLIENT_CMD33_RUN_SMPLCERT_TEST                        33
#define CLIENT_CMD121_CPP_FEATURE_TEST                        121



/* Error code: status sent as response to command from sample client*/

#define SUCCESS                                (0)
#define FAILED                                 (-1)
#define FAIL_REGISTER_SB                       (-2)
#define FAIL_DEREGISTER_SB                     (-3)
#define FAIL_PREP_SB_NON_SECURE_READ           (-4)
#define FAIL_PREP_SB_SECURE_READ               (-5)
#define FAIL_CMD_BUF_TOO_SMALL_FOR_CMD_ID      (-6)
#define FAIL_CMD_RSP_BUFS_TOO_SMALL            (-7)
#define FAIL_SHARED_BUFF_TOO_SMALL             (-8)
#define FAIL_UNDEFINED_COMMAND                 (-9)
#define FAIL_DEVICE_NOT_PROVISIONED_ERROR      (-10)

#endif //__SAMPLECPPAPP_H
