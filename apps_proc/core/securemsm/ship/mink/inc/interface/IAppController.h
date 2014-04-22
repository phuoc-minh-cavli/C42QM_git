// ============================================================================
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
// ============================================================================
#pragma once
// AUTOGENERATED FILE: DO NOT EDIT

#include <stdint.h>
#include "object.h"

#define IAppController_CANCELLATION_NOT_SET UINT32_C(0)
#define IAppController_CANCELLATION_SET UINT32_C(1)

#define IAppController_ERROR_APP_SUSPENDED INT32_C(10)
#define IAppController_ERROR_APP_BLOCKED_ON_LISTENER INT32_C(11)
#define IAppController_ERROR_APP_UNLOADED INT32_C(12)
#define IAppController_ERROR_APP_IN_USE INT32_C(13)

#define IAppController_OP_openSession 0
#define IAppController_OP_unload 1
#define IAppController_OP_getAppObject 2
#define IAppController_OP_setCancellationFlag 3
#define IAppController_OP_getCancellationFlag 4

static inline int32_t
IAppController_release(Object self)
{
  return Object_invoke(self, Object_OP_release, 0, 0);
}

static inline int32_t
IAppController_retain(Object self)
{
  return Object_invoke(self, Object_OP_retain, 0, 0);
}

static inline int32_t
IAppController_openSession(Object self, Object cancellationFlag_val, uint32_t paramTypes_val, const void *i1_ptr, size_t i1_len, const void *i2_ptr, size_t i2_len, const void *i3_ptr, size_t i3_len, const void *i4_ptr, size_t i4_len, void *o1_ptr, size_t o1_len, size_t *o1_lenout, void *o2_ptr, size_t o2_len, size_t *o2_lenout, void *o3_ptr, size_t o3_len, size_t *o3_lenout, void *o4_ptr, size_t o4_len, size_t *o4_lenout, Object *session_ptr, uint32_t *retValue_ptr, uint32_t *retOrigin_ptr)
{
  ObjectArg a[12];
  struct {
    uint32_t m_retValue;
    uint32_t m_retOrigin;
  } o;
  a[5].b = (ObjectBuf) { &o, 8 };
  a[10].o = cancellationFlag_val;
  a[0].b = (ObjectBuf) { &paramTypes_val, sizeof(uint32_t) };
  a[1].bi = (ObjectBufIn) { i1_ptr, i1_len * 1 };
  a[2].bi = (ObjectBufIn) { i2_ptr, i2_len * 1 };
  a[3].bi = (ObjectBufIn) { i3_ptr, i3_len * 1 };
  a[4].bi = (ObjectBufIn) { i4_ptr, i4_len * 1 };
  a[6].b = (ObjectBuf) { o1_ptr, o1_len * 1 };
  a[7].b = (ObjectBuf) { o2_ptr, o2_len * 1 };
  a[8].b = (ObjectBuf) { o3_ptr, o3_len * 1 };
  a[9].b = (ObjectBuf) { o4_ptr, o4_len * 1 };

  int32_t result = Object_invoke(self, IAppController_OP_openSession, a, ObjectCounts_pack(5, 5, 1, 1));

  *o1_lenout = a[6].b.size / 1;
  *o2_lenout = a[7].b.size / 1;
  *o3_lenout = a[8].b.size / 1;
  *o4_lenout = a[9].b.size / 1;
  *session_ptr = a[11].o;
  *retValue_ptr = o.m_retValue;
  *retOrigin_ptr = o.m_retOrigin;

  return result;
}

static inline int32_t
IAppController_unload(Object self)
{
  return Object_invoke(self, IAppController_OP_unload, 0, 0);
}

static inline int32_t
IAppController_getAppObject(Object self, Object *obj_ptr)
{
  ObjectArg a[1];

  int32_t result = Object_invoke(self, IAppController_OP_getAppObject, a, ObjectCounts_pack(0, 0, 0, 1));

  *obj_ptr = a[0].o;

  return result;
}

static inline int32_t
IAppController_setCancellationFlag(Object self, uint32_t flag_val)
{
  ObjectArg a[1];
  a[0].b = (ObjectBuf) { &flag_val, sizeof(uint32_t) };

  return Object_invoke(self, IAppController_OP_setCancellationFlag, a, ObjectCounts_pack(1, 0, 0, 0));
}

static inline int32_t
IAppController_getCancellationFlag(Object self, uint32_t *flag_ptr)
{
  ObjectArg a[1];
  a[0].b = (ObjectBuf) { flag_ptr, sizeof(uint32_t) };

  return Object_invoke(self, IAppController_OP_getCancellationFlag, a, ObjectCounts_pack(0, 1, 0, 0));
}



