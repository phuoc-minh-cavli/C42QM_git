#pragma once
/*=============================================================================
  @file sns_test_osa.h

  Several Operating System Abstractions to be used by the sns_qsocket client
  library.

  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "err.h"

/*=============================================================================
  Macros
  ===========================================================================*/

#if defined(SSC_TARGET_X86)
#define GET_FMT_STR(fmt) fmt
#else
#define GET_FMT_STR(fmt) __FILENAME__ ":" fmt
#endif

/* Log a string message */
#define SNS_LOG(prio, fmt, ...) do { \
    static const char fmt_str[] = GET_FMT_STR(fmt); \
    sns_test_osa_printf(SNS_##prio, __FILENAME__, \
          __LINE__, fmt_str, ##__VA_ARGS__); \
} while(0)

/* See assert() */
#define SNS_ASSERT(value) if(!(value))\
      ERR_FATAL( #value ,0,0,0)

#ifndef ARR_SIZE
#define ARR_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

#ifndef UNUSED_VAR
#define UNUSED_VAR(var) ((void)(var));
#endif

#define SNS_SECTION(name)  \
  __attribute__((section(name)))

/*=============================================================================
  Type Definitions and Declarations
  ===========================================================================*/

typedef void* (*sns_test_osa_thread_func)(void *arg);

struct sns_test_osa_thread;
struct sns_test_osa_mutex;
struct sns_test_osa_sem;

/**
 * priority levels for debug messages
 */
typedef enum sns_test_osa_priority
{
  SNS_VERBOSE = 0,
  SNS_INFO    = 1,
  SNS_WARN   = 2,
  SNS_ERROR   = 3,
  SNS_FATAL   = 4,
} sns_test_osa_priority;

/* Callback notification function prototype
 *
 * @param[i] island_entry Island exit if false; island entry if true
 */
typedef void (*sns_test_osa_island_cb)(bool island_entry);

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

int sns_test_osa_thread_create(struct sns_test_osa_thread **thread_out,
    char *name, sns_test_osa_thread_func func, void *arg, void *stack,
    uint32_t stack_len);

int sns_test_osa_mutex_create(struct sns_test_osa_mutex **mutex);

int sns_test_osa_mutex_lock(struct sns_test_osa_mutex *mutex);

int sns_test_osa_mutex_unlock(struct sns_test_osa_mutex *mutex);

int sns_test_osa_mutex_destroy(struct sns_test_osa_mutex *mutex);

int sns_test_osa_sem_create(struct sns_test_osa_sem **sem);

int sns_test_osa_sem_post(struct sns_test_osa_sem *sem);

int sns_test_osa_sem_wait(struct sns_test_osa_sem *sem);

int sns_test_osa_sem_destroy(struct sns_test_osa_sem *sem);

void sns_test_osa_printf(sns_test_osa_priority prio, const char *file,
    uint32_t line, const char *format, ...);

void* sns_test_osa_malloc(size_t len);
void sns_test_osa_free(void *ptr);

/**
 * One-time initialization for the sns_test_osa module.
 */
void sns_test_osa_init(void);

/**
 * Register or unregister for island state updates.
 */
void sns_test_osa_island_reg(sns_test_osa_island_cb cb_func);
void sns_test_osa_island_unreg(sns_test_osa_island_cb cb_func);

/* Return true if presently in island mode */
bool sns_test_osa_island_status(void);
