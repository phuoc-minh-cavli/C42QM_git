#ifndef __PERF_MEM_SERVICES_H__
#define __PERF_MEM_SERVICES_H__

/*===========================================================================
 * FILE:         perf_mem_services.h
 *
 * DESCRIPTION:  This file provides API for run time memory operations
 *
 * Copyright (c) 2017 Qualcomm Technologies Incorporated.
 ===========================================================================*/

/*==========================================================================

  EDIT HISTORY FOR MODULE

  $Header: 

  when       who     what, where, why
  --------   ---     --------------------------------------------------------
  05/17/17   rr      Initial revision
  ===========================================================================*/

/*
* Modem error callbacks can call this 
* API at crash time to request for
* memory to dump debug information to
* in:
*   - size - Amount of memory requested
*   - Alignment - Address alignment for the pointer
* Returns:
*  - Pointer to memory if request is successfully fullfilled
*    else returns NULL pointer
*/
void* crashdump_overlay_mem_alloc(int size, int align);

#endif
