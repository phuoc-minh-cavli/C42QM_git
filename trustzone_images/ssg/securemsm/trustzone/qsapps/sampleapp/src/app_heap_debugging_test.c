/*
  @file app_heap_debugging_test.c
  @brief This file is a sample code that test TA heap Debugging Interface.

*/
/*===========================================================================
  Copyright (c) 2019 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
  ===========================================================================*/


#include <stdint.h>
#include "IxErrno.h"
#include "qsee_log.h"
#include "qsee_heap.h"

#define MALLOC_SIZE (4*1024)

static int get_heap_info(heap_info_t *heap_info_ptr)
{
  int ret;
  ret = qsee_query_heap_info(heap_info_ptr);
  if (ret) {
    qsee_log(QSEE_LOG_MSG_ERROR, "qsee_queryHeapInfo Failed %d status", ret);
    return E_FAILURE;
  }
  qsee_log(QSEE_LOG_MSG_DEBUG, "Printing sampleapp heap information");
  qsee_log(QSEE_LOG_MSG_DEBUG, "total size = %x", heap_info_ptr->total_size);
  qsee_log(QSEE_LOG_MSG_DEBUG, "used size = %x", heap_info_ptr->used_size);
  qsee_log(QSEE_LOG_MSG_DEBUG, "free size = %x", heap_info_ptr->free_size);
  qsee_log(QSEE_LOG_MSG_DEBUG, "overhead size = %x", heap_info_ptr->overhead_size);
  qsee_log(QSEE_LOG_MSG_DEBUG, "wasted size = %x", heap_info_ptr->wasted_size);
  qsee_log(QSEE_LOG_MSG_DEBUG, "largest free block size = %x", heap_info_ptr->largest_free_block_size);

  return E_SUCCESS;
}

int smplap_heap_debugging_test(void)
{
  heap_info_t heap_info_ptr[3];
  int ret = 0;
  void *data = NULL;
  qsee_log(QSEE_LOG_MSG_DEBUG, "----  Heap Info in the begining ----");
  if (!(ret = get_heap_info(&heap_info_ptr[0]))) {
    qsee_log(QSEE_LOG_MSG_DEBUG, "sampleapp ta heap info SUCCESS %d", ret);
  } else {
    qsee_log(QSEE_LOG_MSG_DEBUG, "sampleapp ta heap info FAILED %d", ret);
    goto Exit;
  }

  if (NULL == (data = qsee_malloc(3*MALLOC_SIZE))) {//12 KB
    qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_malloc() FAILED");
    ret = -1;
    goto Exit;
  }

  qsee_log(QSEE_LOG_MSG_DEBUG, "----  Getting Heap Info after  12 KB malloc ----");
  if (!(ret = get_heap_info(&heap_info_ptr[1]))) {
    if ((heap_info_ptr[1].used_size == (heap_info_ptr[0].used_size + (3*MALLOC_SIZE))) &&
       ((heap_info_ptr[1].free_size <= (heap_info_ptr[0].free_size - (3*MALLOC_SIZE)))) &&
       (heap_info_ptr[1].overhead_size >= heap_info_ptr[0].overhead_size)) {
      qsee_log(QSEE_LOG_MSG_DEBUG, "sampleapp ta heap info SUCCESS %d", ret);
    } else {
      qsee_log(QSEE_LOG_MSG_DEBUG, "sampleapp ta get heap info FAILED due to incorrect results %d", ret);
      ret = -1;
      goto Exit;
    }
  } else {
    qsee_log(QSEE_LOG_MSG_DEBUG, "sampleapp ta get heap info FAILED %d", ret);
    goto Exit;
  }

  qsee_free(data);
  data = NULL;

  qsee_log(QSEE_LOG_MSG_DEBUG, "----  Getting Heap Info after  12 KB free ----");
  if (!(ret = get_heap_info(&heap_info_ptr[2]))) {
    if ((heap_info_ptr[2].used_size == heap_info_ptr[0].used_size) &&
        (heap_info_ptr[2].free_size == heap_info_ptr[0].free_size) &&
        (heap_info_ptr[2].overhead_size == heap_info_ptr[0].overhead_size)) {
      qsee_log(QSEE_LOG_MSG_DEBUG, "sampleapp ta heap info SUCCESS %d", ret);
    } else {
      qsee_log(QSEE_LOG_MSG_DEBUG, "sampleapp ta get heap info FAILED due to incorrect results %d", ret);
      ret = -1;
      goto Exit;
    }
  } else {
    qsee_log(QSEE_LOG_MSG_DEBUG, "sampleapp ta get heap info FAILED %d", ret);
    goto Exit;
  }

  if (NULL == (data = qsee_malloc(MALLOC_SIZE))) {//4KB
    qsee_log(QSEE_LOG_MSG_DEBUG,"qsee_malloc() FAILED");
    ret = -1;
    goto Exit;
  }

  qsee_log(QSEE_LOG_MSG_DEBUG, "----  Getting Heap Info after  4 KB malloc ----");
  if (!(ret = get_heap_info(&heap_info_ptr[1]))) {
    if ((heap_info_ptr[1].used_size == (heap_info_ptr[0].used_size + MALLOC_SIZE)) &&
       ((heap_info_ptr[1].free_size <= (heap_info_ptr[0].free_size - MALLOC_SIZE))) &&
       (heap_info_ptr[1].overhead_size >= heap_info_ptr[0].overhead_size)) {
      qsee_log(QSEE_LOG_MSG_DEBUG, "sampleapp ta heap info SUCCESS %d", ret);
    } else {
      qsee_log(QSEE_LOG_MSG_DEBUG, "sampleapp ta get heap info FAILED due to incorrect results %d", ret);
      ret = -1;
      goto Exit;
    }
  } else {
    qsee_log(QSEE_LOG_MSG_DEBUG, "sampleapp ta get heap info FAILED %d", ret);
    goto Exit;
  }

  qsee_free(data);
  data = NULL;

  qsee_log(QSEE_LOG_MSG_DEBUG, "----  Getting Heap Info after  4 KB free ----");
  if (!(ret = get_heap_info(&heap_info_ptr[2]))) {
    if ((heap_info_ptr[2].used_size == heap_info_ptr[0].used_size) &&
        (heap_info_ptr[2].free_size == heap_info_ptr[0].free_size) &&
        (heap_info_ptr[2].overhead_size == heap_info_ptr[0].overhead_size)) {
      qsee_log(QSEE_LOG_MSG_DEBUG, "sampleapp ta heap info SUCCESS %d", ret);
    } else {
      qsee_log(QSEE_LOG_MSG_DEBUG, "sampleapp ta get heap info FAILED due to incorrect results %d", ret);
      ret = -1;
      goto Exit;
    }
  } else {
    qsee_log(QSEE_LOG_MSG_DEBUG, "sampleapp ta get heap info FAILED %d", ret);
    goto Exit;
  }

  Exit:
    if (NULL != data) {
      qsee_free(data);
    }
   
    if (ret != E_SUCCESS) {
      qsee_log(QSEE_LOG_MSG_DEBUG, "-- End %s TESTS FAILED ---------------------------------", __func__);
      return E_FAILURE;
    } else {
      qsee_log(QSEE_LOG_MSG_DEBUG, "-- End %s TESTS PASSED ---------------------------------", __func__);
      return E_SUCCESS;
    }
}