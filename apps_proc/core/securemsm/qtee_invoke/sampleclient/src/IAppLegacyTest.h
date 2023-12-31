#ifndef __IAPPLEGACYTEST_H
#define __IAPPLEGACYTEST_H

// AUTOGENERATED FILE: DO NOT EDIT

#include <stdint.h>
#include "object.h"

// Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.

// Qualcomm Technologies Proprietary and Confidential.

/**
 * IAppLegacy is an interface that an app can implement to expose the legacy
 * tz_app_cmd_handler interface via SMC Invoke objects
 */

#define IAppLegacyTest_OP_handleRequest 0

static inline int32_t
IAppLegacyTest_release(Object self)
{
  return Object_invoke(self, Object_OP_release, 0, 0);
}

static inline int32_t
IAppLegacyTest_retain(Object self)
{
  return Object_invoke(self, Object_OP_retain, 0, 0);
}

static inline int32_t
IAppLegacyTest_handleRequest(Object self, const void *request_ptr, size_t request_len, void *response_ptr, size_t response_len, size_t *response_lenout)
{
  ObjectArg a[2];
  a[0].bi = (ObjectBufIn) { request_ptr, request_len * 1 };
  a[1].b = (ObjectBuf) { response_ptr, response_len * 1 };

  int32_t result = Object_invoke(self, IAppLegacyTest_OP_handleRequest, a, ObjectCounts_pack(1, 1, 0, 0));

  *response_lenout = a[1].b.size / 1;

  return result;
}


#endif /* __IAPPLEGACYTEST_H */
