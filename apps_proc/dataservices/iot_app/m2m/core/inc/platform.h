/******************************************************************************

  @file    platform.h
  @brief   Platform specific headers

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

******************************************************************************/

#ifndef PLATFORM_HDR_H
#define PLATFORM_HDR_H

#include <stringl/stringl.h>
#include "qapi_socket.h"
#include "errno_codes.h"
#include "timer.h"
#include "fs_public.h"

#include <time.h>                                                 
#include <errno.h>

#define LWM2M_STRCPY(d,s,n) strlcpy(d,s,n)
#define LWM2M_MEMCPY(d,dn,s,cn) memscpy(d,dn,s,cn)
#define LWM2M_MEMMOVE(d,dn,s,cn) memsmove(d,dn,s,cn)           

bool lwm2m_isprint(char c);
bool lwm2m_isspace(char c);

#endif
