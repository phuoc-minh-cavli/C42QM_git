#ifndef __TZ_HWFUSE_H
#define __TZ_HWFUSE_H

/*===========================================================================
   Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

#include <stdint.h>
#include <object.h>

extern int32_t CHwFuse_open(uint32_t uid, Object credentials, Object *objOut);

#endif
