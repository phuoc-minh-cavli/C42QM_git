/**********************************************************************
 * flash_clocks.c
 *
 * SERVICES:    QPIC/PNOC clock and SPI NOR related node voting management
 *
 * DESCRIPTION: Provides flash operation clocks voting for NAND/SPI NOR
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2016-2017 QUALCOMM  Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===================================================================
 *
 *                   EDIT HISTORY FOR MODULE
 *
 * This section contains comments describing changes made to the module.
 * Notice that changes are listed in reverse chronological order.
 *
 *
 * when         who     what, where, why
 * --------     ---     ----------------------------------------------
 * 08/09/17     svl     made timer non deferable for power optimization
 * 11/11/16     svl     optimized clock voting by using timer of 5sec.
 * 09/22/16     sb      Initial Version
 *===================================================================*/

 /*===================================================================

===========================================================================*/

#include "flash.h"

#include "flash_clocks.h"
#include "flash_dal_config.h"

#include "npa.h"

#define FLASH_TIMER_BASED_CLOCK_VOTE
#ifdef FLASH_TIMER_BASED_CLOCK_VOTE
#include "timer.h"
#endif

/* Init PNOC and QPIC clock source request of 100MHZ */ 
#define FLASH_NAND_NPA_PNOC_KHZ 100000
#define FLASH_NAND_NPA_QPIC_KHZ 100000

/* NPA handles for PNOC and QPIC clock voting */
static npa_client_handle flash_nand_npa_pnoc_handle = NULL;
static npa_client_handle flash_nand_npa_qpic_handle = NULL;

/* Global provides ability to adjust value in t32 for tuning and testing */
uint32 flash_pnoc_clock_request = FLASH_NAND_NPA_PNOC_KHZ;
uint32 flash_qpic_clock_request = FLASH_NAND_NPA_PNOC_KHZ;

#ifdef FLASH_TIMER_BASED_CLOCK_VOTE

/** Delay 250 msecs to check flash clocks voting release. */
#ifndef FLASH_CLOCKS_UNVOTE_WAIT_TIME
#define FLASH_CLOCKS_UNVOTE_WAIT_TIME (250)
#endif

typedef enum
{
  FLASH_CLOCKS_UNVOTED = 0,
  FLASH_CLOCKS_UNVOTE_REQUESTED,
  FLASH_CLOCKS_VOTED
}flash_clock_vote_status_t;

/* Global provides ability to adjust value in t32 for tuning and testing */
flash_clock_vote_status_t flash_clocks_vote_status = FLASH_CLOCKS_UNVOTED;
timer_type flash_clocks_req_complete_timer;
qurt_mutex_t flash_clocks_vote_sync;

/* Timer callback to release vote for QPIC and PNOC clocks
 * at end of flash operations.
 */
void flash_clocks_req_complete_timer_cb
      (time_osal_notify_data flash_clocks_req_counter_l)
{
  qurt_mutex_lock(&flash_clocks_vote_sync);
  if(FLASH_CLOCKS_UNVOTE_REQUESTED == flash_clocks_vote_status)
  {
    if (flash_nand_npa_pnoc_handle && flash_nand_npa_qpic_handle)
    {
      /* Release qpic clock vote after flash nand transfer*/
      npa_complete_request(flash_nand_npa_qpic_handle);
        
      /* Release pnoc clock vote after flash nand transfer*/
      npa_complete_request(flash_nand_npa_pnoc_handle);
    }
    flash_clocks_vote_status = FLASH_CLOCKS_UNVOTED;
  }
  qurt_mutex_unlock(&flash_clocks_vote_sync);
}

#endif

/* Flash Operations Clocks init */
void flash_clocks_init(void)
{
  /* check if flash nand npa pnoc handle exists */
  if (!flash_nand_npa_pnoc_handle)
  {
      /* Create this client's NPA handle for PNOC requests only once.
       * NPA resource release by client should be done by invoking
       * npa_complete_request() API.
       */
    flash_nand_npa_pnoc_handle = npa_create_sync_client(
        "/clk/pcnoc", "FLASH-PCNOC", NPA_CLIENT_REQUIRED);
  }
  if (!flash_nand_npa_qpic_handle)
  {
      /* Create this client's NPA handle for QPIC clock requests only once.
       * NPA resource release by client should be done by invoking
       * npa_complete_request() API.
       */
    flash_nand_npa_qpic_handle = npa_create_sync_client(
        "/clk/qpic", "FLASH-QPIC-TX", NPA_CLIENT_REQUIRED);
  }

#ifdef FLASH_TIMER_BASED_CLOCK_VOTE
  qurt_mutex_init(&flash_clocks_vote_sync);
  timer_def_osal( &flash_clocks_req_complete_timer,
                  &timer_non_defer_group,
                  TIMER_FUNC1_CB_TYPE,
                  flash_clocks_req_complete_timer_cb,
                  NULL);
#endif
}

/* Flash Operations Clocks deinit */
void flash_clocks_deinit(void)
{
#ifdef FLASH_TIMER_BASED_CLOCK_VOTE
  qurt_mutex_lock(&flash_clocks_vote_sync);
  timer_clr(&flash_clocks_req_complete_timer, T_MSEC);
  timer_undef(&flash_clocks_req_complete_timer);
#endif

  /* check if npa pnoc handle is registered */
  if (flash_nand_npa_pnoc_handle)
  {
    npa_destroy_client(flash_nand_npa_pnoc_handle);
    flash_nand_npa_pnoc_handle = NULL;
  }
  /* check if npa qpic handle is registered */
  if (flash_nand_npa_qpic_handle)
  {
    npa_destroy_client(flash_nand_npa_qpic_handle);
    flash_nand_npa_qpic_handle = NULL;
  }

#ifdef FLASH_TIMER_BASED_CLOCK_VOTE
  qurt_mutex_unlock(&flash_clocks_vote_sync);
  qurt_mutex_destroy(&flash_clocks_vote_sync);
#endif
}

/* Enter vote for QPIC and PNOC clocks for flash before
 * begining of flash operations.
 */
void flash_clocks_request(void)
{
#ifdef FLASH_TIMER_BASED_CLOCK_VOTE
  qurt_mutex_lock(&flash_clocks_vote_sync);
  if(FLASH_CLOCKS_UNVOTED == flash_clocks_vote_status)
  {
#endif
    if (flash_nand_npa_pnoc_handle && flash_nand_npa_qpic_handle)
    {
      /* Request pnoc clock for flash nand transfer */
      npa_issue_required_request(flash_nand_npa_pnoc_handle,
          flash_pnoc_clock_request);
      /* Request qpic clock for flash nand transfer */
      npa_issue_required_request(flash_nand_npa_qpic_handle,
          flash_qpic_clock_request);
    }
#ifdef FLASH_TIMER_BASED_CLOCK_VOTE
  }
  flash_clocks_vote_status = FLASH_CLOCKS_VOTED;
  qurt_mutex_unlock(&flash_clocks_vote_sync);
#endif
}

/* Release vote for QPIC and PNOC clocks for flash at end of 
 * flash operations.
 */
void flash_clocks_request_complete(void)
{
#ifdef FLASH_TIMER_BASED_CLOCK_VOTE

  qurt_mutex_lock(&flash_clocks_vote_sync);

  flash_clocks_vote_status = FLASH_CLOCKS_UNVOTE_REQUESTED;
  timer_set(&flash_clocks_req_complete_timer, FLASH_CLOCKS_UNVOTE_WAIT_TIME, 0, T_MSEC);

  qurt_mutex_unlock(&flash_clocks_vote_sync);

#else
  if (flash_nand_npa_pnoc_handle && flash_nand_npa_qpic_handle)
  {
    /* Release qpic clock vote after flash nand transfer*/
    npa_complete_request(flash_nand_npa_qpic_handle);
      
    /* Release pnoc clock vote after flash nand transfer*/
    npa_complete_request(flash_nand_npa_pnoc_handle);
  }
#endif
}
