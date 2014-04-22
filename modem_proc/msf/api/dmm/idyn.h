/**
  @file idyn.h

  @brief  
*/
/*
    Copyright (c) 2016 Qualcomm Technologies, Inc.
              All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

All ideas, data and information contained in or disclosed by this
document are confidential and proprietary information of
QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
By accepting this material the recipient agrees that this material
and the information contained therein are held in confidence and in
trust and will not be used, copied, reproduced in whole or in part,
nor its contents revealed in any manner to others without the express
written permission of QUALCOMM Technologies Incorporated.

$Header: //components/rel/msf.mpss/1.0/api/dmm/idyn.h#1 $
$DateTime: 2018/06/27 11:21:46 $
$Author: pwbldsvc $
*/

#pragma once

#include "comdef.h"
#include <sys/types.h>

#define  IDYN_INTERFACE0 idyn_iface_t *idyn_iface_0

/* Enum for dm status */
typedef enum dm_status_e_type {
  DM_STATUS_DONE = 0,
  DM_STATUS_PENDING,
  DM_STATUS_FAILED
} dm_status_e_type;

typedef uint32 dm_ref_type;

typedef dm_status_e_type(*pfnNotify)(dm_ref_type);

typedef struct idyn_iface_t
{
  pfnNotify dyn_init;
  pfnNotify dyn_deinit;
  pfnNotify dyn_resume;
  pfnNotify dyn_suspend;
} idyn_iface_t;

typedef struct
{
  IDYN_INTERFACE0;
} idyn_t;



