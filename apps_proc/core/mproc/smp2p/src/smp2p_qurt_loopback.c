/*
 * @file smp2p_dal_loopback.c
 *
 * Loopback related routines for DAL systems.
 */

/*==============================================================================
     Copyright (c) 2012-2013 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smp2p/src/smp2p_qurt_loopback.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/16/14   bm      Self-loopback should be supported with proper param 
                   validation.
12/17/13   bt      Add descriptive names to SMP2P DAL Workloops.
09/25/13   pa      Remove self-checking code from early smp2p development.
06/26/13   pa      Fix race condition between server and client tasks.
03/05/13   pa      Fixing bug in remote spinlock code for both loopback target
                   (server) and initiator (client)
02/26/13   bt      Add assert for yet another malloc check.
02/08/13   bt      Fix another KW warning about uninitialized result variable.
01/25/13   pa      Fixing klocwork issues.
01/22/13   bt      Resolve compiler warnings comparing uints to 0.
01/17/13   pa      Disable client workloop by default so that timer does not 
                   impact low power modes. 
10/10/12   pa      Initial revision.
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smp2p.h"
#include "smp2p_internal.h"
#include "smp2p_os_common.h"
#include "smem.h"
#include "assert.h"
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALFramework.h"
#include "DALTimer.h"
#include "DALDeviceId.h"
#include "spinlock.h"

#define SMP2P_LB_MAX_EVENTS 10 /* I don't think this makes a difference. */
#define SMP2P_WORKLOOP_STACK_SZ 8192

/* OS objects for workloops. */
static DALSYSEventHandle hStartEvent_server, hStartEvent_client;
static DALSYSWorkLoopHandle hWorkLoop_server, hWorkLoop_client;
static DALSYSEventHandle hTimerEvent;
static DalDeviceHandle *hTimer;

/* Workloop prototypes for server and client */
static void smp2p_lb_server_workloop(void);
static void smp2p_lb_client_workloop(void);

/* Helper functions for loopback client */
static void smp2p_lb_cmd_send_receive
(
  smem_host_type loopback_dest,
  uint32   data_tx,
  uint32 * data_rx
);
static void smp2p_lb_open_local_loopback(void);
static uint32 smp2p_lb_get_rand(void);

/** Define the loopback command structure.
 *  This is the format of the entry data for loopback ports named "smp2p". */
typedef union
{
  uint32 all;
  struct
  {
    uint32 data     : 24;
    uint32 cmd      : 7;
    uint32 req_resp : 1;
  } fields;
} smp2p_lb_cmd_type;

/* The following are the supported loopback commands */
#define SMP2P_LB_CMD_ECHO         1
#define SMP2P_LB_CMD_CLEAR        2
#define SMP2P_LB_CMD_DEC          3
#define SMP2P_LB_CMD_RS_START     4
#define SMP2P_LB_CMD_RS_LOCKED    5
#define SMP2P_LB_CMD_RS_UNLOCKED  6
#define SMP2P_LB_CMD_RS_END       7

#define SMP2P_LB_CMD_RS_END_FAIL_MASK 0x00f00000

/* Context structures for loopback workloops */
smp2p_context_type * smp2p_lb_ctx_tx[SMEM_NUM_HOSTS];
smp2p_context_type * smp2p_lb_ctx_rx[SMEM_NUM_HOSTS];

/* This flag tells SMP2P to start the loopback client workloop on SMP2P init.
 * User should break at main() or smp2p_init() then set this flag.
 * The client loopback test is kicked off by setting variables in 
 * smp2p_lb_client_workloop().  
 * SMP2P server always runs and will respond to loopback commands regardless 
 * of this flag. 
 * Use volatile here so that compiler does not optimize away code based on 
 * this flag. */
volatile uint32 smp2p_lb_client_workloop_enable = 0;

extern smp2p_info_type smp2p_info[SMEM_NUM_HOSTS];

/** The count of interrupts triggered to the local proc as part of local loopback */
uint32 smp2p_lb_int_count = 0;

/** Random number seed */
static uint32 rand_seed = 0;

/** API to get spinlock value as needed for spinlock test. */
extern uint32 (*spin_lock_get_fn_ptr)(uint32);

/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION  smp2p_lb_start_tasks
===========================================================================*/
/**
  This function initializes and starts the loopback tasks for server and client.

  @return
  None
*/
/*=========================================================================*/
void smp2p_os_start_loopback_tasks(void)
{
  DALResult result = 0;
  NOTUSED(result); /* Avoid warning since result is used only in ASSERTs */

  /* Start server workloop */

  /* Create event to kick off the server task.
   * This event will be used to start the workloop itself and is not used
   * within the workloop. */
  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT,
                              &hStartEvent_server, NULL);
  ASSERT(DAL_SUCCESS == result);

  /* Register work loop. */
  result = DALSYS_RegisterWorkLoopEx("SMP2P_SERVER", SMP2P_WORKLOOP_STACK_SZ,
      SYSTEM_WORKLOOP_PRIORITY_MEDIUM,
      SMP2P_LB_MAX_EVENTS, &hWorkLoop_server, NULL);
  ASSERT(DAL_SUCCESS == result);

  /* Add start task event to work loop. */
  result = DALSYS_AddEventToWorkLoop(hWorkLoop_server,
              (DALSYSWorkLoopExecute)&smp2p_lb_server_workloop,
                                     NULL, hStartEvent_server, NULL);
  ASSERT(DAL_SUCCESS == result);

  /* Kick off work loop. */
  result = DALSYS_EventCtrl(hStartEvent_server, DALSYS_EVENT_CTRL_TRIGGER);
  ASSERT(DAL_SUCCESS == result);

  /* Start client workloop if enabled */
  if (smp2p_lb_client_workloop_enable)
  {
    /* Create event to kick off the server task.
     * This event will be used to start the workloop itself and is not used
     * within the workloop. */
    result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT,
                                &hStartEvent_client, NULL);
    ASSERT(DAL_SUCCESS == result);

    /* Register work loop. */
    result = DALSYS_RegisterWorkLoopEx("SMP2P_CLIENT", SMP2P_WORKLOOP_STACK_SZ,
        SYSTEM_WORKLOOP_PRIORITY_MEDIUM,
        SMP2P_LB_MAX_EVENTS, &hWorkLoop_client, NULL);
    ASSERT(DAL_SUCCESS == result);

    /* Add start task event to work loop. */
    result = DALSYS_AddEventToWorkLoop(hWorkLoop_client,
                (DALSYSWorkLoopExecute)&smp2p_lb_client_workloop,
                                       NULL, hStartEvent_client, NULL);
    ASSERT(DAL_SUCCESS == result);

    /* Kick off work loop. */
    result = DALSYS_EventCtrl(hStartEvent_client, DALSYS_EVENT_CTRL_TRIGGER);
    ASSERT(DAL_SUCCESS == result);
  }
}

/*===========================================================================
  FUNCTION  smp2p_lb_server_workloop
===========================================================================*/
/**
  This function runs as the SMP2P workloop server task.
  It creates outbound SMP2P entries to all procs called "smp2p", as well as
  registers for inbound entries with the same name.
  The task blocks and waits for inbound loopback commands.  When a known
  command is received, it writes the appropriate response.

  @return
  None
*/
/*=========================================================================*/
static void smp2p_lb_server_workloop(void)
{
  smp2p_lb_cmd_type cmd;
  int32 status;
  uint32 ii;
  smp2p_context_type * rx_ctx_arr[SMEM_NUM_HOSTS];
  uint32 rx_map[SMEM_NUM_HOSTS];
  uint32 rx_cnt;

#if 0
  {
    /* create an entry before negotiation to mimic the recovery/detection
     * mechanism */
    uint32 size = SMP2P_SMEM_ITEM_SIZE(SMP2P_ENTRIES_TOTAL);
    uint32 smem_id = SMP2P_GET_SMEM_ID(SMP2P_THIS_HOST, SMEM_APPS);
    smp2p_header_type * header_tx = smem_alloc((smem_mem_type)smem_id, size);
    smp2p_header_version_type version;

    SMP2P_WRITE_SMEM_4(&header_tx->identifier,                SMP2P_IDENTIFIER);
    SMP2P_WRITE_SMEM_2(&header_tx->procs.proc_local,          SMP2P_THIS_HOST);
    SMP2P_WRITE_SMEM_2(&header_tx->procs.proc_remote,         SMEM_APPS);
    SMP2P_WRITE_SMEM_2(&header_tx->entry_count.entries_total, SMP2P_ENTRIES_TOTAL);
    SMP2P_WRITE_SMEM_2(&header_tx->entry_count.entries_valid, 0);
    SMP2P_WRITE_SMEM_4(&header_tx->version,                   1);
    SMP2P_WRITE_SMEM_4(&header_tx->reserved,                  0);

    version.version = 1;
    version.flags   = 0;
    SMP2P_COPY_TO_SMEM(&header_tx->version, &version, sizeof(version));

    /* notify remote endpoint of update */
    smp2p_os_send_interrupt(SMEM_APPS);
  }
#endif

  for (ii=0, rx_cnt=0; ii<SMEM_NUM_HOSTS; ii++)
  {
    /* ignore unused endpoints */
    if (SMEM_RPM != ii && SMEM_RSVD3 != ii && SMEM_RSVD5 != ii)
    {
      if(ii == SMP2P_LPBK_HOST)
      {
        /* This means we are trying to connect to local loopback. If 
         * local_loopback has not been opened by the client task, exit */
        if(smp2p_info[SMP2P_LPBK_HOST].header_rx == NULL)
        {
          continue;
        }
      }
        
      /* create outbound entries.
       * don't wait for negotation to complete since we don't know
       * what processors are on this target and will respond.
       * instead, assume that negotiation is done if we receiving anything */
      status = smp2p_create(&smp2p_lb_ctx_tx[ii], (smem_host_type)ii, "smp2p");
      ASSERT(SMP2P_STATUS_SUCCESS  == status ||
             SMP2P_STATUS_NOT_INIT == status);

      /* listen on incoming entries */
      status = smp2p_register(&smp2p_lb_ctx_rx[ii], (smem_host_type)ii, "smp2p");
      ASSERT(SMP2P_STATUS_SUCCESS   == status ||
             SMP2P_STATUS_NOT_FOUND == status);

      /* save the incoming event handle */
      rx_ctx_arr[rx_cnt] = smp2p_lb_ctx_rx[ii];

      /* save the destination id, so that when rx_ctx_arr[x] has an event,
       * we can map that back to an destination */
      rx_map[rx_cnt] = ii;

      /* increment count */
      rx_cnt++;
    }
  }

  SMP2P_LOG(SMP2P_LOG_EVENT_LB_SERVER_ENTRIES_CREATED, 0, 0, 0);

  /* main receiving loop */
  while (1)
  {
    uint32 remote_id;
    uint32 index;

    /* wait for incoming events
     * this is a blocking call until we receive data on an incoming entry.
     * when this returns, the context at rx_ctx_arr[i] will have data
     * available to read */
    status = smp2p_wait_multiple(rx_ctx_arr, rx_cnt, &ii);
    ASSERT(SMP2P_STATUS_SUCCESS == status);
    ASSERT(ii < rx_cnt);

    /* map the remote id */
    remote_id = rx_map[ii];
    ASSERT(remote_id < SMEM_NUM_HOSTS);

    /* read the data there */
    status = smp2p_read(smp2p_lb_ctx_rx[remote_id], &cmd.all);
    ASSERT(SMP2P_STATUS_SUCCESS == status);

    /* for logging, read out the index from the context */
    index = smp2p_lb_ctx_tx[remote_id]->index;

    SMP2P_LOG(SMP2P_LOG_EVENT_LB_SERVER_RX, remote_id, index, cmd.all);

    /* if this is an incoming request, take action on it */
    if (cmd.fields.req_resp)
    {
      switch (cmd.fields.cmd)
      {
      case SMP2P_LB_CMD_ECHO:
        SMP2P_LOG(SMP2P_LOG_EVENT_LB_SERVER_TX, remote_id, index, cmd.all);
        cmd.fields.req_resp = 0;
        status = smp2p_write(smp2p_lb_ctx_tx[remote_id], cmd.all);
        ASSERT(SMP2P_STATUS_SUCCESS == status);
        break;
      case SMP2P_LB_CMD_CLEAR:
        SMP2P_LOG(SMP2P_LOG_EVENT_LB_SERVER_TX, remote_id, index, cmd.all);
        status = smp2p_write(smp2p_lb_ctx_tx[remote_id], 0);
        ASSERT(SMP2P_STATUS_SUCCESS == status);
        break;
      case SMP2P_LB_CMD_DEC:
        cmd.fields.req_resp = 0;
        if (cmd.fields.data)
          cmd.fields.data--;
        SMP2P_LOG(SMP2P_LOG_EVENT_LB_SERVER_TX, remote_id, index, cmd.all);
        status = smp2p_write(smp2p_lb_ctx_tx[remote_id], cmd.all);
        ASSERT(SMP2P_STATUS_SUCCESS == status);
        break;

      case SMP2P_LB_CMD_RS_START:
        {
          uint32 jj;
          uint32 lock_count = 0;
          smp2p_lb_cmd_type tx_cmd;

          uint32 spinlock_owner;
          
          /* spin in the loop until the remote side sends an END cmd */
          while (1)
          {
            /* lock the hw mutex */
            smem_spin_lock(SMEM_SPINLOCK_SMEM_ALLOC);

            /* increment running count */
            lock_count++;

            /* set the value to locked in the smp2p tx entry */
            tx_cmd.fields.req_resp = 1;
            tx_cmd.fields.cmd      = SMP2P_LB_CMD_RS_LOCKED;
            tx_cmd.fields.data     = lock_count;
            status = smp2p_write(smp2p_lb_ctx_tx[remote_id], tx_cmd.all);
            ASSERT(SMP2P_STATUS_SUCCESS == status);

            /* poll the rx entry to see if remote side was able to acquire 
             * the lock.  if the lock is broken, remote side will write
             * RS_LOCKED into its entry. */
            jj = 0;
            do
            {
              status = smp2p_read(smp2p_lb_ctx_rx[remote_id], &cmd.all);
              ASSERT(SMP2P_STATUS_SUCCESS == status);

              /* check that the local endpoint is still the owner of the lock */
              spinlock_owner = spin_lock_get_fn_ptr(SMEM_SPINLOCK_SMEM_ALLOC);
              if (SMP2P_LB_CMD_RS_LOCKED == cmd.fields.cmd ||
                  SMP2P_THIS_HOST + 1 != spinlock_owner)
              {
                /* getting here means that the remote endpoint has acquired the
                 * lock while the local endpoint had it. 
                 * wait here for initiator to detect the issue and 
                 * end the test. */
                while (cmd.fields.cmd != SMP2P_LB_CMD_RS_END)
                {
                  status = smp2p_read(smp2p_lb_ctx_rx[remote_id], &cmd.all);
                  ASSERT(SMP2P_STATUS_SUCCESS == status);
                }
              }

              jj++;
            }
            while (jj<1000 && cmd.fields.cmd != SMP2P_LB_CMD_RS_END);

            /* if we read an END, then send the ack and exit the main loop */
            if (SMP2P_LB_CMD_RS_END == cmd.fields.cmd)
            { 
              /* unlock the mutex */
              smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);

              tx_cmd.fields.req_resp = 0;
              tx_cmd.fields.cmd      = SMP2P_LB_CMD_RS_END;
              tx_cmd.fields.data     = lock_count;

              if (SMP2P_THIS_HOST + 1 != spinlock_owner)
              {
                tx_cmd.fields.data  |= spinlock_owner << 20;
              }

              status = smp2p_write(smp2p_lb_ctx_tx[remote_id], tx_cmd.all);
              ASSERT(SMP2P_STATUS_SUCCESS == status);

              break;
            }
            else
            {
              /* set the value to unlocked and continue the tests */
              tx_cmd.fields.req_resp = 1;
              tx_cmd.fields.cmd      = SMP2P_LB_CMD_RS_UNLOCKED;
              tx_cmd.fields.data     = lock_count;
              status = smp2p_write(smp2p_lb_ctx_tx[remote_id], tx_cmd.all);
              ASSERT(SMP2P_STATUS_SUCCESS == status);

              /* unlock the mutex */
              smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);

              #if 0
              /* random delay loop to allow remote side to acquire lock
               * but only every 8 iterations. */
              if (0 == (lock_count & 7))
              {
                volatile uint32 kk;
                uint32 rand;
                rand = smp2p_lb_get_rand() & 0x7f;
                for (kk=0; kk<rand; kk++) { }
              }
              #endif
            }
          }
        }
        break;

      default:
        break;
      }
    }
  }
}

/*===========================================================================
  FUNCTION  smp2p_lb_client_workloop
===========================================================================*/
/**
  This function runs as the SMP2P workloop client task.
  This function is the test initiator for SMP2P loopback.
  The task is dormant until the user sets a volatile variable via the debugger.
  When the variable is non-zero, the task issues several commands to the
  remote SMP2P endpoint.  All outbound SMP2P traffic should be in the form
  of loopback command requests (as opposed to responses).
  When a response is received, the values are checked with asserts.

  Note that for local loopback, the outbound entry will use the item in SMEM,
  while the inbound entry will be in a local buffer created by the client
  code, which also points the smp2p_info to the local buffer.

  @return
  None
*/
/*=========================================================================*/
volatile int32 loopback_dest = -1;
volatile int32 num_echo_cmds = 0;
volatile int32 multiplier = 13;
volatile int32 rand_test = 0;
volatile int32 max_tests = 100;
volatile int32 spinlock_test = 0;

static void smp2p_lb_client_workloop()
{
  uint32 i;

  smp2p_lb_cmd_type cmd;
  smp2p_lb_cmd_type resp;

  DALResult result;

  /* setup connection to timer */
  result = DAL_DeviceAttach(DALDEVICEID_TIMER, &hTimer);
  ASSERT(DAL_SUCCESS == result);

  /* create the timer event */
  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_TIMER_EVENT,
                              &hTimerEvent,
                              NULL);
  ASSERT(DAL_SUCCESS == result);

  /* register for timer events */
  result = DalTimer_Register(hTimer, hTimerEvent, 100000);
  ASSERT(DAL_SUCCESS == result);

  /* open the local loopback connection */
  smp2p_lb_open_local_loopback();

  while (1)
  {
    /* when this variable is set to 1, a random test is initiated.
     * this sends a pseudo random number of loopback commands with
     * pseudo random data to apps, modem, adsp, and wcnss (including
     * the local processor). */
    if (rand_test)
    {
      max_tests--;
      if (max_tests == 0)
      {
        /* end the tests */
        rand_test = 0;
        loopback_dest = -1;
        max_tests = 100;
      }
      else
      {
        uint32 random_num = smp2p_lb_get_rand();

        num_echo_cmds = random_num & 0x0f;
        loopback_dest = (random_num >> 8) & 0x03;

        /* SMEM ID 3 is invalid, so convert it to a valid ID */
        if (3 == loopback_dest) loopback_dest = 4;

        ASSERT_BRK(loopback_dest == 4 ||
                   loopback_dest == 2 ||
                   loopback_dest == 1 ||
                   loopback_dest == 0);
      }
    }

    /* wait here for debugger/user to set loopback_dest via T32 */
    if (loopback_dest < 0 ||
        SMP2P_STATE_OPEN != smp2p_info[loopback_dest].state)
    {
      result = DALSYS_EventWait(hTimerEvent);
      ASSERT(DAL_SUCCESS == result);
    }
    else
    {
      smp2p_os_params_type saved_params = {0};

      if (SMP2P_LPBK_HOST != loopback_dest)
      {
        /* save existing event */
        memcpy(&saved_params, &smp2p_lb_ctx_rx[loopback_dest]->os_params, sizeof(saved_params));

        /* overwrite with new event corresponding to this thread */
        smp2p_os_init_event(&smp2p_lb_ctx_rx[loopback_dest]->os_params);
      }

      /* check that the connection is open */
      ASSERT_BRK(SMP2P_STATE_OPEN == smp2p_info[loopback_dest].state);

      /****************************************************************/
      /* create a command: Echo */
      cmd.fields.req_resp = 1;
      cmd.fields.cmd      = SMP2P_LB_CMD_ECHO;
      cmd.fields.data     = 0x456789;

      /* send the command and wait for a response */
      smp2p_lb_cmd_send_receive((smem_host_type)loopback_dest, cmd.all, &resp.all);

      /* check response */
      ASSERT_BRK(0 == resp.fields.req_resp);
      ASSERT_BRK(cmd.fields.cmd == resp.fields.cmd);
      ASSERT_BRK(cmd.fields.data == resp.fields.data);

      /****************************************************************/
      /* create a command: Clear */
      cmd.fields.req_resp = 1;
      cmd.fields.cmd      = SMP2P_LB_CMD_CLEAR;
      cmd.fields.data     = 0xF0F0F0;

      /* send the command and wait for a response */
      smp2p_lb_cmd_send_receive((smem_host_type)loopback_dest, cmd.all, &resp.all);

      /* check response */
      ASSERT_BRK(0 == resp.fields.req_resp);
      ASSERT_BRK(0 == resp.fields.cmd);
      ASSERT_BRK(0 == resp.fields.data);

      /****************************************************************/
      /* create a command: Decrement */
      cmd.fields.req_resp = 1;
      cmd.fields.cmd      = SMP2P_LB_CMD_DEC;
      cmd.fields.data     = 0x123456;

      /* send the command and wait for a response */
      smp2p_lb_cmd_send_receive((smem_host_type)loopback_dest, cmd.all, &resp.all);

      ASSERT_BRK(0 == resp.fields.req_resp);
      ASSERT_BRK(cmd.fields.cmd == resp.fields.cmd);
      ASSERT_BRK(cmd.fields.data-1 == resp.fields.data);

      /****************************************************************/
      /* issue Echo commands with various data */
      for (i=0; i<num_echo_cmds; i++)
      {
        cmd.fields.req_resp = 1;
        cmd.fields.cmd      = SMP2P_LB_CMD_ECHO;
        cmd.fields.data    *= multiplier;

        /* send the command and wait for a response */
        smp2p_lb_cmd_send_receive((smem_host_type)loopback_dest, cmd.all, &resp.all);

        /* check response */
        ASSERT_BRK(0 == resp.fields.req_resp);
        ASSERT_BRK(cmd.fields.cmd == resp.fields.cmd);
        ASSERT_BRK(cmd.fields.data == resp.fields.data);
      }

      /****************************************************************/
      /* run the remote spinlock test, if enabled */
      if (spinlock_test && SMP2P_LPBK_HOST != loopback_dest)
      {
        uint32 ii, jj;
        uint32 status;

        uint32 spinlock_owner;

        /* tell the remote side to start the test
         * either local or remote can get the spinlock first */
        cmd.fields.req_resp = 1;
        cmd.fields.cmd      = SMP2P_LB_CMD_RS_START;
        cmd.fields.data     = 0;
        status = smp2p_write(smp2p_lb_ctx_tx[loopback_dest], cmd.all);
        ASSERT_BRK(SMP2P_STATUS_SUCCESS == status);

        /* send the command and wait for a response */
        smp2p_lb_cmd_send_receive((smem_host_type)loopback_dest, cmd.all, &resp.all);

        ASSERT_BRK(SMP2P_LB_CMD_RS_LOCKED == resp.fields.cmd);
        
        /* run the lock test */
        for (ii=0; ii<10000; ii++)
        {
          /* wait for the spinlock to become available */
          smem_spin_lock(SMEM_SPINLOCK_SMEM_ALLOC);

          /* set the value to locked in the smp2p tx entry */
          cmd.fields.req_resp = 1;
          cmd.fields.cmd      = SMP2P_LB_CMD_RS_LOCKED;
          cmd.fields.data     = ii+1;
          status = smp2p_write(smp2p_lb_ctx_tx[loopback_dest], cmd.all);
          ASSERT_BRK(SMP2P_STATUS_SUCCESS == status);

          /* poll the rx entry to see if remote side was able to acquire 
           * the lock.  if the lock is broken, remote side will write
           * RS_LOCKED into its entry. */
          jj = 0;
          do
          {
            status = smp2p_read(smp2p_lb_ctx_rx[loopback_dest], &resp.all);
            ASSERT_BRK(SMP2P_STATUS_SUCCESS == status);

            /* check that the local endpoint is still the owner of the lock.
             * if this fails, it means that the remote processor was able 
             * to acquire the lock while owned by the local proc. */
            spinlock_owner = spin_lock_get_fn_ptr(SMEM_SPINLOCK_SMEM_ALLOC);
            if (SMP2P_THIS_HOST + 1 != spinlock_owner)
            {
              ASSERT_BRK(0);
              break;
            }

            /* check that the remote endpoint does not already have the lock.
             * if this is the case, the local endpoint was able to acquire the
             * lock while the remote proc had it. */
            if (SMP2P_LB_CMD_RS_LOCKED == resp.fields.cmd)
            {
              break;
            }

            jj++;
          }
          while (jj<1000);

          /* if either failure event occurs, just unlock and send the END msg */
          if (SMP2P_LB_CMD_RS_LOCKED == resp.fields.cmd ||
              SMP2P_THIS_HOST + 1 != spinlock_owner)
          {
            /* unlock the mutex */
            smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);
            
            break;
          }
          else
          {
            /* set the value to unlocked */
            cmd.fields.req_resp = 1;
            cmd.fields.cmd      = SMP2P_LB_CMD_RS_UNLOCKED;
            cmd.fields.data     = ii+1;
            status = smp2p_write(smp2p_lb_ctx_tx[loopback_dest], cmd.all);
            ASSERT_BRK(SMP2P_STATUS_SUCCESS == status);

            /* unlock the mutex */
            smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);
          }
        }

        /* end the test */
        cmd.fields.req_resp = 1;
        cmd.fields.cmd      = SMP2P_LB_CMD_RS_END;
        cmd.fields.data     = ii+1;

        /* write command into the entry */
        status = smp2p_write(smp2p_lb_ctx_tx[loopback_dest], cmd.all);
        ASSERT(SMP2P_STATUS_SUCCESS == status);

        /* wait for response. do polling loop since remote side may issue
         * other commands before it gets to the end */
        do
        {
          status = smp2p_read(smp2p_lb_ctx_rx[loopback_dest], &resp.all);
          ASSERT(SMP2P_STATUS_SUCCESS == status);
        }
        while (resp.fields.cmd != SMP2P_LB_CMD_RS_END);

        /* check response */
        ASSERT_BRK(0 == resp.fields.req_resp);
        ASSERT_BRK(cmd.fields.cmd == resp.fields.cmd);

        /* check to see if remote/target indicates a failure */
        ASSERT_BRK(0 == (resp.fields.data & SMP2P_LB_CMD_RS_END_FAIL_MASK));
      }

      /****************************************************************/
      if (SMP2P_LPBK_HOST != loopback_dest)
      {
        /* restore existing event
         * note that the temporary event is lost (not freed).  this is ok since
         * this code only runs with the debugger attached */
        memcpy(&smp2p_lb_ctx_rx[loopback_dest]->os_params, &saved_params, sizeof(saved_params));
      }

      /* end the test */
      loopback_dest = -1;
    }
  }
}

static void smp2p_lb_cmd_send_receive
(
  smem_host_type loopback_dest,
  uint32   data_tx,
  uint32 * data_rx
)
{
  int32 status;
  int32 prev_int_count;
  DALResult result;

  SMP2P_LOG(SMP2P_LOG_EVENT_LB_CLIENT_TX, loopback_dest, data_tx, 0);

  if (SMP2P_LPBK_HOST == loopback_dest)
  {
    /* write command to the entry */
    smp2p_info[SMP2P_LPBK_HOST].header_rx->entry[0].data = data_tx;
    SMP2P_MEMORY_BARRIER();

    /* save interrupt count to check below */
    prev_int_count = smp2p_lb_int_count;

    /* trigger the isr to notify server thread */
    smp2pi_isr(SMP2P_LPBK_HOST);

    /* wait for other thread to process command */
    while (smp2p_lb_int_count == prev_int_count)
    {
      result = DALSYS_EventWait(hTimerEvent);
      ASSERT(DAL_SUCCESS == result);
    }

    /* read response */
    *data_rx = smp2p_info[SMP2P_LPBK_HOST].header_tx->entry[0].data;
  }
  else
  {
    /* write command into the entry */
    status = smp2p_write(smp2p_lb_ctx_tx[loopback_dest], data_tx);
    ASSERT(SMP2P_STATUS_SUCCESS == status);

    /* wait for remote endpoint to respond */
    status = smp2p_wait(smp2p_lb_ctx_rx[loopback_dest]);
    ASSERT(SMP2P_STATUS_SUCCESS == status);

    /* read out the response data */
    smp2p_read(smp2p_lb_ctx_rx[loopback_dest], data_rx);
    ASSERT(SMP2P_STATUS_SUCCESS == status);
  }

  SMP2P_LOG(SMP2P_LOG_EVENT_LB_CLIENT_RX, loopback_dest, *data_rx, 0);
}

static void smp2p_lb_open_local_loopback(void)
{
  uint32 num_entries = 4;
  uint32 prev_int_count;
  smp2p_header_type * header;
  DALResult result;

  ASSERT_BRK(SMP2P_STATE_OPEN != smp2p_info[SMP2P_LPBK_HOST].state);

  /* wait for other thread to initiate tx side of connection */
  while (SMP2P_STATE_INIT != smp2p_info[SMP2P_LPBK_HOST].state)
  {
    result = DALSYS_EventWait(hTimerEvent);
    ASSERT(DAL_SUCCESS == result);
  }

  /* for local loopback, create a mock connection
   * start the negotiation with an unsupported version number
   * and check that it is geared down to a supported one. */

  /* mimic the remote side creation of the SMEM item and SMP2P entry */
  header = smp2p_os_malloc(SMP2P_SMEM_ITEM_SIZE(num_entries));
  ASSERT_BRK(header != NULL);
  header->identifier                = SMP2P_IDENTIFIER;
  header->procs.proc_local          = SMP2P_LPBK_HOST;
  header->procs.proc_remote         = SMP2P_THIS_HOST;
  header->version.version           = 2; /* this is unsupported */
  header->version.flags             = 0;
  header->entry_count.entries_total = num_entries;
  header->entry_count.entries_valid = 0;

  /* manually connect the header pointer to the mimic header
   * we just created */
  smp2p_info[SMP2P_LPBK_HOST].header_rx = header;

  /* call the ISR since we won't actually fire the interrupt. */
  prev_int_count = smp2p_lb_int_count;
  SMP2P_MEMORY_BARRIER();
  smp2pi_isr(SMP2P_LPBK_HOST);

  /* isr should fire one interrupt to respond */
  ASSERT_BRK(prev_int_count + 1 == smp2p_lb_int_count);

  /* the connection should not be open yet due to version negotiation */
  ASSERT_BRK(SMP2P_STATE_INIT == smp2p_info[SMP2P_LPBK_HOST].state);
  ASSERT_BRK(0 != smp2p_info[SMP2P_LPBK_HOST].header_tx->version.version);
  ASSERT_BRK(0 == smp2p_info[SMP2P_LPBK_HOST].header_tx->entry_count.entries_valid);

  /* set version down to a valid number */
  header->version.version =
      smp2p_info[SMP2P_LPBK_HOST].header_tx->version.version;

  /* call the ISR since we won't actually fire the interrupt. */
  prev_int_count = smp2p_lb_int_count;
  SMP2P_MEMORY_BARRIER();
  smp2pi_isr(SMP2P_LPBK_HOST);

  /* no response since connection is now open */
  ASSERT_BRK(prev_int_count == smp2p_lb_int_count);

  /* connection should be open now */
  ASSERT_BRK(SMP2P_STATE_OPEN == smp2p_info[SMP2P_LPBK_HOST].state);

  /* check that no valid rx entries are there yet */
  ASSERT_BRK(0 == smp2p_info[SMP2P_LPBK_HOST].remote_entries_valid_shadow);

  /* but there should be one valid tx entry */
  ASSERT_BRK(1 == smp2p_info[SMP2P_LPBK_HOST].header_tx->entry_count.entries_valid);

  /* create a the loopback entry, and notify the client side */
  strlcpy(header->entry[0].name, "smp2p", SMP2P_NAME_STRING_LEN);
  header->entry_count.entries_valid++;
  SMP2P_MEMORY_BARRIER();
  smp2pi_isr(SMP2P_LPBK_HOST);

  /* check that we now have a valid entry */
  ASSERT_BRK(1 == smp2p_info[SMP2P_LPBK_HOST].remote_entries_valid_shadow);

  /* Wait for the server thread to quiesce */
  result = DALSYS_EventWait(hTimerEvent);
  ASSERT(DAL_SUCCESS == result);
  result = DALSYS_EventWait(hTimerEvent);
  ASSERT(DAL_SUCCESS == result);
  result = DALSYS_EventWait(hTimerEvent);
  ASSERT(DAL_SUCCESS == result);
}

static uint32 smp2p_lb_get_rand(void)
{
  if (0 == rand_seed)
  {
    rand_seed = smp2p_os_timetick_get();
  }

  /* really pseudo random number */
  rand_seed = rand_seed * 13 + 17328193;

  return rand_seed;
}

