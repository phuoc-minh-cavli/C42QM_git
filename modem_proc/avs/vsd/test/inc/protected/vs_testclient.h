#ifndef __VS_TESTCLIENT_H__
#define __VS_TESTCLIENT_H__

/**
  @file  vs_testclient.h
  @brief This file contains the definitions of Voice Services test client.

*/

/*
  ============================================================================

   Copyright (C) 2016 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

  $Header: //components/rel/avs.mpss/8.0.c12/vsd/test/inc/protected/vs_testclient.h#1 $
  $Author: pwbldsvc $
  
  ============================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "apr_comdef.h"

/****************************************************************************
 * TYPE DEFINITIONS                                                         *
 ****************************************************************************/
 
#define VS_ITEST_CMD_INIT ( 0x00000001 )

#define VS_ITEST_CMD_LOOPBACK_START ( 0x00000002 )

#define VS_ITEST_CMD_LOOPBACK_STOP ( 0x00000003 )

#define VS_ITEST_CMD_LOOPBACK_SET_VOC_PROPERTY ( 0x00000004 )

#define VS_ITEST_CMD_DEINIT ( 0x0000FFFF )

typedef struct {
  uint8_t size;
  uint8_t payload1;
  uint8_t payload2;
  uint8_t payload3;
  uint8_t payload4;
  uint8_t payload5;
}
  vs_test_loopback_cmd_t;


/****************************************************************************
 * CALL                                                                     *
 ****************************************************************************/
/**
  Provides a single API entry point to the Voice Services test client.
 
  @param[in] index       Command ID.
  @param[in,out] params   Command payload.
  @param[in] size         Size of the command payload.
   
  @return
  APR_EOK when successful.
 
  @dependencies
  None.

  @comments
  None.
*/

APR_EXTERNAL int32_t vs_test_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
);

#endif /*__VS_TESTCLIENT_H__ */
