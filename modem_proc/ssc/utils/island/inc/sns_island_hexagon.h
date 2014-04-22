#pragma once
/*=============================================================================
  @file sns_island_hexagon.h

  Header file for the island hexagon utility

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*============================================================================
  INCLUDES
  ============================================================================*/

#include "sns_island.h"
#include "npa.h"
#include "npa_resource.h"
#include "pb_encode.h"
#include "sns_isafe_list.h"
#include "sns_island_service.h"
#include "sns_types.h"
#include "sns_time.h"
#include "sns_osa_lock.h"
#include "uSleep_mode_trans.h"
/*============================================================================
  Macros
  ============================================================================*/
#define SNS_ISLAND_CODE __attribute__((section (".text.SNS")))
#define SNS_ISLAND_DATA __attribute__((section (".data.SNS")))
#define SNS_ISLAND_EXIT_NOTIFY_ARR_SIZE 5
#define SNS_ISLAND_TRANSITION_LOG_ARR_SIZE 5
#define SNS_ISLAND_MAX_BLOCKED_THREADS 15

/* Uncomment this flag to enable island debug */
//#define SNS_ISLAND_ENABLE_DEBUG
#define DEBUG_ARRAY_LENGTH 20
/*============================================================================
  Functions
  ============================================================================*/
/**
 * This function sends a npa request to enable island.
 *
 * @param[i] npa_handle      The npa handle for issuing the request.
 * @param[i] state           The island state to be conveyed to npa interface.
 *
 */
void sns_island_enable_npa_request(
       npa_client_handle npa_handle,
       npa_resource_state state);

/**
 * This function sends a npa request to disable island.
 *
 * @param[i] npa_handle      The npa handle for issuing the request.
 * @param[i] state           The island state to be conveyed to npa interface.
 *
 */
void sns_island_disable_npa_request(
       npa_client_handle npa_handle,
       npa_resource_state state);

/**
 * This function creates a npa handle.
 *
 * @param[o] npa_client_handle  The npa client handle created.
 */
npa_client_handle sns_island_npa_create_handle(void);

/**
 * This function registers a callback with usleep module
 * and returns an usleep handle.
 *
 * @param[o] uSleep_notification_cb_handle  The usleep handle.
 */

uSleep_notification_cb_handle
       sns_island_usleep_registration(uSleep_notification_cb_type callback);

/*============================================================================
  Types
  ============================================================================*/

typedef enum
{
  ISLAND_ENTER_CALLBACK = 0,
  ISLAND_EXIT_CALLBACK,
  ISLAND_ENTRY_BLOCKED,
  ISLAND_ENTRY_UNBLOCKED,
  NO_ACTIVE_THREADS,
  ISLAND_EXIT_CALLED
}sns_island_state_transition_triggers;

// System state.
#define SET_IN_ISLAND(state) qurt_atomic_or(&state, 0x04)
#define SET_NOT_IN_ISLAND(state) qurt_atomic_and(&state, 0x03)
#define IS_IN_ISLAND(state) (state & 0x04)

// SEE state
#define SET_ISLAND_UNBLOCKED(state) qurt_atomic_or(&state, 0x02)
#define SET_ISLAND_BLOCKED(state) qurt_atomic_and(&state, 0x05)
#define IS_UNBLOCKED(state)(state & 0x02)

// SEE vote
#define SET_ISLAND_ENABLED(state) qurt_atomic_or(&state, 0x01)
#define SET_ISLAND_DISABLED(state) qurt_atomic_and(&state, 0x06)
#define IS_ENABLED(state) (state & 0x01)

typedef enum
{
  NOT_IN_ISLAND_BLOCKED_DISABLED   = 0x00,    // 0000 0000
  NOT_IN_ISLAND_UNBLOCKED_DISABLED = 0x02,    // 0000 0010
  NOT_IN_ISLAND_UNBLOCKED_ENABLED  = 0x03,    // 0000 0011
  IN_ISLAND_UNBLOCKED_ENABLED      = 0x07,     // 0000 0111
  TRANSITION_FROM_NOT_IN_ISLAND_BLOCKED_DISABLED   = 0x10,    // 0001 0000
  TRANSITION_FROM_NOT_IN_ISLAND_UNBLOCKED_DISABLED = 0x12,    // 0001 0010
  TRANSITION_FROM_NOT_IN_ISLAND_UNBLOCKED_ENABLED  = 0x13,    // 0001 0011
  TRANSITION_FROM_IN_ISLAND_UNBLOCKED_ENABLED      = 0x17     // 0001 0111
}sns_island_valid_island_states;

typedef struct
{
  bool in_use;
  sns_island_exit_cb cb;
  intptr_t args;
} sns_island_exit_signals;

typedef struct
{
  uint32_t thread_id;
  bool block;
}sns_island_thread_blocks;


typedef struct
{
  sns_isafe_list_item list_entry;
  char          client_name[16];
  sns_osa_lock *lock;
#ifdef SNS_ISLAND_ENABLE_DEBUG
  /* Time stamp of last island block */
  sns_time last_block_timestamp;
  /* Time stamp of last island unblock*/
  sns_time last_unblock_timestamp;
  /* Longest island block time since device bootup*/
  sns_time longest_block_time;
#endif
}sns_island_aggregator_client;

typedef struct
{
  /* Island service pointer */
  sns_island_service service;

  /* Client handle provided by the NPA */
  npa_client_handle sns_island_handle;

  /* Current island state */
  unsigned int current_state;

  /* Enable/Disable Configuration via Registry */
  bool configure_island_enable;

  /* Mutex protecting current_state
  */
  sns_osa_lock *island_state_lock;

  /* Timestamp of last entry into island mode*/
  sns_time island_entry_timestamp;

  /* Timestamp of last exit out of island mode*/
  sns_time island_exit_timestamp;

  /* Island exit notifications */
  sns_island_exit_signals sns_island_exit_notify[SNS_ISLAND_EXIT_NOTIFY_ARR_SIZE];

  /* Island blocks */
  sns_isafe_list island_blocks;

  /* Last island vote*/
  unsigned int last_vote;

  /* Last enable vote */
  sns_time last_enable;

  /* Previous enable */
  sns_time prvs_enable;

  /* Last disable vote */
  sns_time last_disable;

  /* Previous disable */
  sns_time prvs_disable;

  /* Last entry */
  sns_time last_entry;

  /* Last exit */
  sns_time last_exit;

  /* previous entry */
  sns_time prvs_entry;

  /* previous exit */
  sns_time prvs_exit;

  /*count from userpd_init*/
  uint32_t island_entry_count;
  
  /*count from userpd_init*/
  uint32_t island_exit_count;
  
  /*max time_us spent in island*/
  uint64_t max_time_spent_us;
  
  /*Min time_us spent in island*/
  uint64_t min_time_spent_us;

}sns_fw_island_state;

#ifdef SNS_ISLAND_ENABLE_DEBUG
typedef struct
{
  /* Time stamp of island state update */
  sns_time island_transition_ts;

  /* Store 3 levels of caller pointers */
  void* caller0;
  void* caller1;
  void* caller2;

  /* Initial and final state after island state update */
  unsigned int island_transition_states[2];

}sns_island_transition_debug_state;

typedef struct
{
  /* Circular buffer for island transition debug */
  sns_island_transition_debug_state island_transition_debug[DEBUG_ARRAY_LENGTH];

  /* Overload the cookie field to add an island packet number */
  uint64_t island_log_packet_number;

  /* Last time stamps when all threads were idle */
  sns_time last_all_threads_idle_ts;

  /* Last island block count when all sensors threads were idle */
  uint32_t last_all_threads_idle_island_block_cnt;

  /* Index used to write into island transition debug circular buffer */
  uint8_t island_transition_write_index;
}sns_island_debug_state;
#endif
