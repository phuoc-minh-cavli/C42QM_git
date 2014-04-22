/*!
  @file
  mem_utility.h

  @brief
  memory utility function file.
*/

/*===========================================================================

  Copyright (c) 2020 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

#include "stdbool.h"
#include "qapi_status.h"
#include "net_utils_ext.h"
#include "qapi_driver_access.h"

bool  mem_utility_IsFromKernel  (void* handle, data_app_type app);
void* mem_utility_Get_MemoryPool(void* handle, data_app_type app);
void* mem_utility_Get_Memory_From_Pool(void *usr_buf, uint16_t memory_size);
void* mem_utility_Get_Memory_For_App  (void* handle, data_app_type app, uint16_t memory_size);
void  mem_utility_release_memory(bool user_space_res, void* memory_addr);

