/**
 * @file smdl_profile.c
 *
 * Implements SMD Lite profiling.
 *
 * On startup, SMD Lite creates a thread to run profiling. The thread remains
 * dormant until triggered by the debugger by configuring the data in
 * #smdl_profile_control. Once the profiling results are complete they are
 * reported in #smdl_profile_results.
 */

/*==============================================================================
     Copyright (c) 2009-2014 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smdl_profile.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/01/14   rv      Replace the MSG_* macros with MSG V2 APIs.
03/05/14   rv      Freeing memory when loopback server open fails. 
12/12/13   bt      Fix ARM compiler warnings.
10/17/13   an      diag hook to kick off profiling.
03/15/13   bt      Wait at end of each trial for LB server to complete close.
10/17/12   bt      Add smd_internal.h inclusion for smd_host_exists_in_channel.
09/05/12   pa      Fixing compile errors due to use of deprecated SMD_RPM.
07/12/12   bt      Remove mistake for an aborted feature.
05/10/12   bt      Fixes for race conditions, further profiling conveniences.
04/27/12   bt      Remove unneeded rex.h header, allow dynamic fifo_size.
04/16/12   bt      Fix typo in smdl_profile_lb_cb references.
03/02/12   bt      smdl_profile as a client of Nway lb, allow any server API.
02/09/12   bt      Added OS implementation for timetick calls in smdl_profile.
02/01/12   pa      Removing inclusion of deprecated customer.h
12/16/11   bt      Add option to profile with or without verification overhead.
04/28/11   bt      Resolve compiler warning (smdl_profile_result_count).
07/20/10   tl      Use SMD Lite API for SMD priority.
06/29/10   tl      Wait for 1000 ms while sleeping in profile task
04/21/10   tl      Updated to SMD Lite v2
04/17/09   tl      Initial SMD Lite test code
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include "smd_lite.h"
#include "smd_os_common.h"
#include "smdl_profile.h"
#include "smdl_debug.h"
#include "smd_loopback_client.h"
#include "smd_internal.h"
#include "assert.h"
#include "err.h"
#include "smem_log.h"
#include "DALSys.h"
#include <stdio.h>
#include "diagdiag.h"

/*===========================================================================
                      LOCAL CONSTANT DECLARATIONS
===========================================================================*/

/** The maximum number of packets sent in a single profiling trial. */
#define MAX_PACKET_COUNT 1024

/** The maximum packet size. */
#define MAX_PACKET_SIZE 4096

/** The maximum number of results returned from a single profiling run. */
#define SMDL_PROFILE_MAX_RESULTS 100

/*===========================================================================
                    LOCAL MACRO DEFINITIONS
===========================================================================*/

/** 
 * Assertion macro for SMD Lite profiling
 *
 * If the assertion fails, the macro will call ERR_FATAL().
 */
#define SMDL_ASSERT(cond) \
  do { \
    if(!(cond)) { \
      SMDL_DEBUG("Assertion failed: " #cond, 0, 0, 0); \
      ERR_FATAL("Assertion failed: " #cond, 0, 0, 0); \
    } \
  } while(0)

#define SMDL_TEST_Q6FW

#define SMDL_PROFILE_DIAG_CMD_CODE 2049

/*===========================================================================
                      LOCAL TYPE DECLARATIONS
===========================================================================*/

typedef enum{
    SMDL_PROFILE_VERIFY_DATA,
    SMDL_PROFILE_NO_OVERHEAD,
} smd_profile_overhead_type;

typedef struct {

  /** The edge for the profiling run. */
  smd_channel_type edge;

  /** Flags to use when opening the SMD Lite port */
  uint32 open_flags;

  /** FIFO size to use when opening the SMD Lite port 
    * The FIFO size must be a multiple of 32 and 
    * must not be less than SMD_MIN_FIFO or greater 
    * than SMD_MAX_FIFO. 
    * If this parameter is 0, the default size (SMD_STANDARD_FIFO) 
    * will be used.*/
  uint32 fifo_size;

  /** The total number of packets per profiling trial. Must
    * not be larger than MAX_PACKET_COUNT. */
  int total_count;

  /** The packet size. Must not be
    * larger than MAX_PACKET_SIZE */
  int packet_size;

  /** The limit on number of in-flight packets in the fifo */
  int in_flight_limit;

  /** Flag indicating whether to verify looped-back data or obtain optimal 
   *  total tick count (especially useful for multiple in-flight packets). */
 smd_profile_overhead_type overhead;
  
  /** Determines which SMD API the remote processor should use when 
   *  reading and writing data. */
  smd_loopback_api_type server_api;
  
} smdl_profile_control_type;

/**
 * Control information for SMDL profiling.
 *
 * The debugger updates this information and the SMDL profiling thread polls
 * it to determine when to launch a profiling test. The results are reported
 * in the static array #smdl_profile_result_count.
 */
volatile static struct {
  /**
   * The state of the SMD Lite profiling thread.
   *
   * At startup, the state is SMDL_PROFILE_STOPPED. When the debugger finishes
   * configuring the test, it sets the state to SMDL_PROFILE_READY. As soon as
   * the test thread awakens and begins running the test, it sets the state to
   * SMDL_PROFILE_RUNNING. When the test thread finishes running the tests, it
   * sets the state to SMDL_PROFILE_DONE. The debugger may configure
   * additional tests and set the state to SMDL_PROFILE_READY to repeat the
   * process.
   */
  enum {
    SMDL_PROFILE_STOPPED,
    SMDL_PROFILE_READY,
    SMDL_PROFILE_RUNNING,
    SMDL_PROFILE_DONE,
  } state;

  /** The edge for the profiling run. */
  smd_channel_type edge;

  /** Flags to use when opening the SMD Lite port */
  uint32 open_flags;

  /** The total number of packets per profiling trial */
  int total_packet_count;

  /** The minimum packet size */
  int min_packet_size;

  /** The maximum packet size */
  int max_packet_size;

  /** The minimum number of in-flight packets in the fifo */
  int min_fifo_count;

  /** The maximum number of in-flight packets in the fifo */
  int max_fifo_count;
  
  /** Flag indicating whether to verify looped-back data or obtain optimal 
   *  total tick count (especially useful for multiple in-flight packets). */
  smd_profile_overhead_type overhead;
  
  /** Determines which SMD API the remote processor should use when 
   *  reading and writing data. */
  smd_loopback_api_type server_api;
  
} smdl_profile_control_dbg;

/**
 * The results for a single profiling trial.
 *
 * This structure is populated by smdl_profile_trial() and returned to the
 * debugger in the static array #smdl_profile_result_count.
 *
 * Total throughput for the entire test, in bytes/tick, is
 * total_bytes/total_ticks.
 *
 * Average latency per packet, in ticks, is total_latency/total_count.
 *
 * @todo It may be useful to report CPU load information, especially that of
 * the SMD Lite code, for both processors.
 */
typedef struct {
  /** The edge for the profiling run. */
  smd_channel_type edge;

  /** The size of each packet sent */
  int packet_size;

  /** The number of packets sent before waiting for them to be received. */
  int max_in_flight;

  /** The number of ticks per second */
  uint32 timetick_frequency;

  /** The total number of bytes sent, used in throughput calculations. */
  int total_bytes;

  /** The total number of timeticks from beginning to end, used in throughput
   * calculations. */
  int total_ticks;

  /** The total number of timeticks elapsed between sending a particular
   * packet and receiving the response, used in latency calculations. */
  int total_latency;
  
  /** The total number of timeticks elapsed during smdl_write function calls. */
  int total_write_ticks;
  
  /** The total number of timeticks elapsed during smdl_read function calls. */
  int total_read_ticks;

  /** The total number of timeticks elapsed during srv write callback function calls. */
  int total_srv_write_ticks;
  
  /** The total number of timeticks elapsed during srv read callback function calls. */
  int total_srv_read_ticks;

  /** The total number of timeticks elapsed during srv smd_write_notify function calls. */
  int total_write_notify;

  /** The total number of packets sent, used in latency calculations. */
  int total_count;
} smdl_profile_result_type;

/**
 * The context for a current profiling trial.
 *
 * This structure contains ncessary information to initiate
 * a profiling test when a diag command arrives.
 */
typedef struct
{
  /** The profiling workloop handle */
  DALSYSWorkLoopHandle hWorkLoop;

  /** The event to kick off profiling workloop */
  DALSYSEventHandle hStartEvent;

  /** The diag response id to send the results */
  diagpkt_subsys_delayed_rsp_id_type delayed_rsp_id;

  /** The profiling configuration */
  smdl_profile_control_type controls;
} smd_profile_context_type;

/**
 * The diag profiling request descriptor.
 *
 * This structure contains profiling command settings 
 * sent over diag.
 */
typedef struct
{
  /** The diag packet header */
  diagpkt_subsys_header_type header;

  /** The loopback edge (should include this host) */
  uint8  edge;

  /** The server API to use */
  uint8  server_api;

  /** The FIFO size in KB */
  uint8  fifo_size_in_kb;

  /** Various flags to configure profiling */
  uint8  flags;

  /** The number of packets to send in a single trial */
  uint16 total_count;

  /** The packet size */
  uint16 packet_size;

  /** The maximum limit of packets in flight */
  uint16 in_flight_limit;
} smd_profile_diag_request_type;

/**
 * The diag profiling response descriptor.
 *
 * This structure contains profiling response 
 * results to be sent back over diag.
 */
typedef struct
{
  /** The diag packet header */
  diagpkt_subsys_header_v2_type header;

  /** The profiling result data */
  smdl_profile_result_type      result;
} smd_profile_diag_response_type;

/*===========================================================================
                              LOCAL DATA DECLARATIONS
===========================================================================*/

/**
 * The number of profiling trials executed in the most recent profiling run.
 *
 * This is the number of entries in #smdl_profile_results that are valid.
 */
#if 0 /* Uncomment when the body of smdl_profile() is uncommented. */
static int smdl_profile_result_count = 0;
#endif

/**
 * The profiling results, as reported back to the debugger.
 *
 * The number of entries valid for this particular profiling run is
 * #smdl_profile_result_count.
 */
static smdl_profile_result_type smdl_profile_results[SMDL_PROFILE_MAX_RESULTS];

/** The event used to block the profiling thread until data is received. */
static DALSYSEventHandle smdl_profile_event = NULL;

/** Flag used to indicate when the port is finished opening or closing. */
static boolean smdl_profile_port_opened = FALSE;

PACK(void *) smd_profile_diag_test (
  PACK(void *) req_pkt,
  uint16 pkt_len
);

static const diagpkt_user_table_entry_type smd_profile_rsp_tbl[] =
{
  {SMDL_PROFILE_DIAG_CMD_CODE, SMDL_PROFILE_DIAG_CMD_CODE, smd_profile_diag_test}
};

static smd_profile_context_type smd_profile_context = {0};

/*===========================================================================
                    LOCAL FUNCTION DEFINITIONS
===========================================================================*/
volatile static smd_channel_type smdl_test_edge = SMD_INVALID_CHANNEL_TYPE;

/**
 * SMD Lite profiling callback
 *
 * This callback signals the event to awaken the SMD Lite profiling thread
 * from the SMD Lite ISR. All parameters are ignored.
 */
static void smdl_profile_trial_cb
(
  smdl_handle_type  port,
  smdl_event_type   event,
  void *            data
)
{
  if (event == SMDL_EVENT_OPEN)
  {
    smdl_profile_port_opened = TRUE;
  }
  else if (event == SMDL_EVENT_CLOSE_COMPLETE)
  {
    smdl_profile_port_opened = FALSE;
  }
  DALSYS_EventCtrl(smdl_profile_event, DALSYS_EVENT_CTRL_TRIGGER);
}

static void smdl_profile_lb_cb( boolean success, void *data )
{
  if (success)
  {
    *((int32 *)data) = 1;
  }
  else
  {
    *((int32 *)data) = -1;
  }
  DALSYS_EventCtrl(smdl_profile_event, DALSYS_EVENT_CTRL_TRIGGER);
}

/**
 * Executes a single profiling trial.
 *
 * This function performs a loopback test on the specified edge by
 * sending the specified number of packets, each of the specified size, to the
 * remote processor. When the packets are returned, the function verifies that
 * the data returned is correct. At the completion of the test, the function
 * reports the total throughput and average latency.
 *
 * @param[in]    smdl_profile_control           
                                Profiling configuration
 * @param[out]   result         The structure in which the profiling results
 *                              are returned.
 *
 * @return       None
 * @sideeffects  Opens a loopback port to a remote processor. Blocks until the
 *               test is complete.
 */
static void smdl_profile_trial
(
  smdl_profile_control_type *smdl_profile_control,
  smdl_profile_result_type  *result
)
{
  int sent_cnt, rcv_cnt;
  smdl_handle_type port;
  int actual_in_flight;
  int max_in_flight = 0;
  uint32 test_start, test_end, total_latency;
  uint32 total_write_ticks, total_read_ticks;
  volatile int32 loopback_open_success = 0;
  volatile int32 loopback_sig_success = 0;
  volatile int32 loopback_close_success = 0;
  smd_loopback_handle_type loopback_handle = NULL;
  char *wbuf;
  char *rbuf;
  uint32 *tx_time;

  wbuf = (char*)smd_malloc(MAX_PACKET_SIZE);
  rbuf = (char*)smd_malloc(MAX_PACKET_SIZE);
  tx_time = (uint32*)smd_malloc(sizeof(*tx_time)*MAX_PACKET_COUNT);

  SMDL_ASSERT(wbuf != NULL && rbuf != NULL && tx_time != NULL);

  SMDL_ASSERT(DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &smdl_profile_event,
        NULL) == DAL_SUCCESS);

  /* Open this end */
  port = smdl_open("SMDL_PROFILE", smdl_profile_control->edge, smdl_profile_control->open_flags,
      smdl_profile_control->fifo_size, smdl_profile_trial_cb, NULL);
  SMDL_ASSERT(port != NULL);

  /* Send open request to the server side */
  loopback_handle = 
      smd_loopback_open("SMDL_PROFILE", 
                        smdl_profile_control->edge, 
                        smdl_profile_control->fifo_size, 
                        smdl_profile_control->server_api, 
                        SMD_PKTXFR_TYPE, 
                        SMD_LOOPBACK_ECHO, 
                        smdl_profile_lb_cb, 
                        (void *)&loopback_open_success);

  while (loopback_open_success != 1 && loopback_open_success != -1){
    DALSYS_EventWait(smdl_profile_event);
  }
  if (loopback_open_success != 1)
  {
    SMDL_INFO("Loopback server failed to open.", 0, 0, 0);
    SMDL_ASSERT(smdl_close(port) == 0);
    while (smdl_profile_port_opened)
    {
      DALSYS_EventWait(smdl_profile_event);
    }
    SMDL_ASSERT(DALSYS_DestroyObject(smdl_profile_event) == DAL_SUCCESS);
    smd_free(tx_time);
    smd_free(rbuf);
    smd_free(wbuf);
    return;
  }
  SMDL_ASSERT(loopback_handle != NULL);
  if (smdl_profile_control->server_api == SMD_LOOPBACK_API_MEMCPY || 
      smdl_profile_control->server_api == SMD_LOOPBACK_API_DSM)
  {
    smd_loopback_signal(loopback_handle, SMD_LOOPBACK_SIGNAL_DTR, 
                        TRUE, smdl_profile_lb_cb, 
                        (void *)&loopback_sig_success);
    while (!loopback_sig_success){
      DALSYS_EventWait(smdl_profile_event);
    }
    loopback_sig_success = 0;
    smd_loopback_signal(loopback_handle, SMD_LOOPBACK_SIGNAL_CTS, 
                        TRUE, smdl_profile_lb_cb, 
                        (void *)&loopback_sig_success);
    while (!loopback_sig_success){
      DALSYS_EventWait(smdl_profile_event);
    }
  }
  
  while (!smdl_profile_port_opened)
  {
    DALSYS_EventWait(smdl_profile_event);
  }

  if (smdl_profile_control->overhead != SMDL_PROFILE_VERIFY_DATA)
  {
    memset(wbuf, 'a', smdl_profile_control->packet_size);
  }
  
  actual_in_flight = 0;

  total_latency = 0;
  total_write_ticks = 0;
  total_read_ticks = 0;
  sent_cnt = 0;
  rcv_cnt = 0;
  test_start = smdl_profile_timetick_get();

  while(sent_cnt < smdl_profile_control->total_count) {
    int32 wlen;
    if (smdl_profile_control->overhead == SMDL_PROFILE_VERIFY_DATA)
    {
      /* Fill in the write buffer for verification */
      memset(wbuf, 'a' + (sent_cnt % 26), smdl_profile_control->packet_size);
    }
    do {
      tx_time[sent_cnt] = smdl_profile_timetick_get();
      wlen = smdl_write(port, smdl_profile_control->packet_size, wbuf, 0);
      SMDL_ASSERT(wlen >= 0);
      if(wlen == 0) {
        /* if there is data to be read just proceed to reading loop */
        if (smdl_rx_peek(port) != 0)
        {
          break;
        }
        /* Wait until we can write. This probably
         * means until the other end opens the port,
         * if we sized the fifo correctly.
         */
        DALSYS_EventWait(smdl_profile_event);
      } else {
        total_write_ticks += smdl_profile_timetick_get() - tx_time[sent_cnt];
        SMDL_ASSERT(wlen == smdl_profile_control->packet_size);
      }
    } while(wlen == 0);

    if (wlen != 0)
    {
      sent_cnt++;

      if (++actual_in_flight > max_in_flight)
        max_in_flight = actual_in_flight;
    }

    /* Read as many packets as are available. Only wait for packets
     * if in_flight packets are in flight. */
    for(;;) {
      uint32 before_rx_time = smdl_profile_timetick_get();
      int32 rlen = smdl_read(port, smdl_profile_control->packet_size, rbuf, 0);
      SMDL_ASSERT(rlen >= 0);
      if(rlen > 0) {
        uint32 rx_time = smdl_profile_timetick_get();

        SMDL_ASSERT(rlen == smdl_profile_control->packet_size);

        actual_in_flight--;

        total_latency += (rx_time - tx_time[rcv_cnt]);
        total_read_ticks += rx_time - before_rx_time;

        /* Validate the packet */
        if (smdl_profile_control->overhead == SMDL_PROFILE_VERIFY_DATA)
        {
          memset(wbuf, 'a' + ((rcv_cnt) % 26), smdl_profile_control->packet_size);

          SMDL_ASSERT(memcmp(wbuf, rbuf, smdl_profile_control->packet_size) == 0);
        }
        rcv_cnt++;
      } else {
        /* If max in_flight packets are in flight, OR if we are in the last 
         * iteration of the trial loop AND there are any packets in flight, 
         * we wait for smdl_profile_event before continuing on. */
        if( (actual_in_flight >= smdl_profile_control->in_flight_limit) || 
            ( (actual_in_flight > 0) && (sent_cnt == smdl_profile_control->total_count) ) ) {
          DALSYS_EventWait(smdl_profile_event);
        } else {
          break;
        }
      }
    }
  }

  test_end = smdl_profile_timetick_get();

  SMDL_ASSERT(sent_cnt == rcv_cnt);
  SMDL_ASSERT(smdl_close(port) == 0);
  SMDL_ASSERT(smd_loopback_close(loopback_handle, smdl_profile_lb_cb, 
                                 (void *)&loopback_close_success));
  while (smdl_profile_port_opened || loopback_close_success != 1)
  {
    DALSYS_EventWait(smdl_profile_event);
  }

  SMDL_ASSERT(DALSYS_DestroyObject(smdl_profile_event) == DAL_SUCCESS);

  result->edge = smdl_profile_control->edge;
  result->packet_size = smdl_profile_control->packet_size;
  result->max_in_flight = max_in_flight;
  result->timetick_frequency = smdl_profile_timetick_get_freq();
  result->total_bytes = smdl_profile_control->packet_size * smdl_profile_control->total_count;
  result->total_ticks = test_end - test_start;
  result->total_latency = total_latency;
  result->total_write_ticks = total_write_ticks;
  result->total_read_ticks = total_read_ticks;
  result->total_srv_write_ticks = 0; //need support at the other end
  result->total_srv_read_ticks = 0; //need support at the other end
  result->total_write_notify = 0; //need support at the other end
  result->total_count = sent_cnt - actual_in_flight;

  smd_free(wbuf);
  smd_free(rbuf);
  smd_free(tx_time);
}

/**
 * This function is called when the profiling is complete.
 *
 * The debugger may choose to place a breakpoint on this function for
 * immediate notification.
 */
static void smdl_profile_complete(void)
{
  /* Debugger breakpoint here */
  SMDL_INFO("SMDL profiling operation complete", 0, 0, 0);
}

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/**
 * Executes the SMD Lite profiling from this thread's context.
 *
 * This function never returns. It waits until triggered by the debugger, then
 * executes the requested profiling tests, and resumes waiting until triggered
 * by the debugger again.
 *
 * @return       None
 */
void smdl_profile(void) {
#if 0
  int i, j, k = 0;

  smdl_profile_control_dbg.state = SMDL_PROFILE_STOPPED;
#endif
  
  (void)smdl_profile_timetick_init();
  
#if 0
  for(;;)
  {
    /* Wait for the debugger to start the test. Poll every second until
     * smdl_profile_control.state transitions to SMDL_PROFILE_READY. */
    while(smdl_profile_control_dbg.state != SMDL_PROFILE_READY)
    {
      smdl_profile_wait(1000);
    }

    if(smdl_profile_control_dbg.total_packet_count > MAX_PACKET_COUNT)
      smdl_profile_control_dbg.total_packet_count = MAX_PACKET_COUNT;

    if(smdl_profile_control_dbg.max_packet_size > MAX_PACKET_SIZE)
      smdl_profile_control_dbg.max_packet_size = MAX_PACKET_SIZE;

    smdl_profile_control_dbg.state = SMDL_PROFILE_RUNNING;
    if(smdl_profile_result_count > 0)
      smdl_profile_result_count = 0;

    /* The debugger launched a profiling session. Run it, and report the
     * results in the local data structure. */
    for(i = 0; ; i++) {
      /* Test both powers of 2 and numbers halfway between the powers */
      int packet_size = 1 << (i / 2);
      if(i % 2) {
        if(i == 1) {
          continue;
        } else {
          packet_size += 1 << (i / 2 - 1);
        }
      }

      if(packet_size < smdl_profile_control_dbg.min_packet_size)
        continue;

      if(packet_size > smdl_profile_control_dbg.max_packet_size)
        break;

      for(j = smdl_profile_control_dbg.min_fifo_count;
          j <= smdl_profile_control_dbg.max_fifo_count;
          j++)
      {
        smdl_profile_control_type control = {0};

        if(k > 0)
        {
          /* Wait between each test to give the rest of the system a chance to
           * catch up */
          smdl_profile_wait(50);
        }

        control.server_api = smdl_profile_control_dbg.server_api;
        control.edge = smdl_profile_control_dbg.edge;
        control.open_flags = smdl_profile_control_dbg.open_flags;
        control.packet_size = packet_size;
        control.in_flight_limit = j;
        control.total_count = smdl_profile_control_dbg.total_packet_count;
        control.fifo_size = SMD_STANDARD_FIFO;
        control.overhead = smdl_profile_control_dbg.overhead;

        smdl_profile_trial(&control, &smdl_profile_results[k]);

        k++;
        smdl_profile_result_count = k;
        if(smdl_profile_result_count >= SMDL_PROFILE_MAX_RESULTS)
          break;
      }

      if(smdl_profile_result_count >= SMDL_PROFILE_MAX_RESULTS)
        break;
    }

    smdl_profile_control_dbg.state = SMDL_PROFILE_DONE;
    smdl_profile_complete();
  }
  /* Never returns */
#else

  if(smdl_test_edge != SMD_INVALID_CHANNEL_TYPE)
  {
    smdl_profile_control_type control;

    memset(&control, 0, sizeof(smdl_profile_control_type));

    control.server_api = SMD_LOOPBACK_API_SMDLITE;
    control.edge = smdl_test_edge;

    if (smd_host_exists_in_channel(smdl_test_edge, SMEM_RPM))
    {
      control.packet_size = 64;
      control.in_flight_limit = 1;
      control.total_count = 30;
      control.fifo_size = SMD_MIN_FIFO;
    }
    else
    {
      control.packet_size = 1024;
      control.in_flight_limit = 1;
      control.total_count = 8;
      control.fifo_size = SMD_STANDARD_FIFO;
    }

    smdl_profile_trial(&control, &smdl_profile_results[0]);
    smdl_profile_complete();
  }
#endif

}

/**
 * Executes the SMD profiling workloop.
 *
 * This function will be called by DAL in workloop context 
 * when smd_profile_context.hStartEvent is triggered.
 * It performs one profile trial and sends the delayed 
 * response with profiling results back to DIAG.
 * 
 * @return DAL_SUCCESS if the profiling was performed
 *         DAL_ERROR   if the response packet could not be allocated
 */
static DALResult smd_profile_workloop(DALSYSEventHandle handle, void * param)
{
  smd_profile_diag_response_type *rsp = (smd_profile_diag_response_type *)
                                          diagpkt_subsys_alloc_v2_delay(
                                            DIAG_SUBSYS_COREBSP, 
                                            SMDL_PROFILE_DIAG_CMD_CODE, 
                                            smd_profile_context.delayed_rsp_id, 
                                            sizeof(*rsp));

  if(!rsp)
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "SMD profile: Cannot send the delayed response");
    return DAL_ERROR;
  }

  smdl_profile_trial(&smd_profile_context.controls, &rsp->result);

  diagpkt_delay_commit(rsp);

  return DAL_SUCCESS;
}

/**
 * Configures the SMD profiling parameters.
 *
 * This function will be called by DIAG when profiling command
 * is sent to this host.
 * It parses command parameters and triggers profiling workloop
 * to execute. If command parameters are out of expected range
 * the error code DIAG_BAD_PARM_F is sent. In case of another 
 * failure error code DIAG_BAD_CMD_F is used.
 * 
 * @return NULL always
 */
PACK(void *) smd_profile_diag_test (
  PACK(void *) req_pkt,
  uint16 pkt_len
)
{
  smd_profile_diag_request_type *req = (smd_profile_diag_request_type*)req_pkt;
  void* rsp;
  diagpkt_cmd_code_type err_code;
  DALResult result = DAL_SUCCESS;
  
  if (!req || pkt_len != sizeof(*req) ||
      req->total_count > MAX_PACKET_COUNT || 
      req->packet_size > MAX_PACKET_SIZE ||
      !smd_host_exists_in_channel((smd_channel_type)req->edge, SMD_THIS_HOST))
  {
    err_code = DIAG_BAD_PARM_F;
  }
  else
  {
    do
    {
      if (smd_profile_context.hWorkLoop == NULL)
      {
        result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT,
                                    &smd_profile_context.hStartEvent, NULL);
        if (DAL_SUCCESS != result)
          break;

        /* Register work loop. */
        result = DALSYS_RegisterWorkLoopEx("SMD_PROFILE_WL", 2048, 0, 1,
                                           &smd_profile_context.hWorkLoop, NULL);
        if (DAL_SUCCESS != result)
          break;
    
        /* Add start task event to work loop. */
        result = DALSYS_AddEventToWorkLoop(smd_profile_context.hWorkLoop, 
                                           &smd_profile_workloop,
                                           NULL, 
                                           smd_profile_context.hStartEvent, 
                                           NULL);
        if (DAL_SUCCESS != result)
          break;
      }

      smd_profile_context.controls.edge = (smd_channel_type)req->edge;
      smd_profile_context.controls.open_flags = SMDL_OPEN_FLAGS_MODE_PACKET;
      smd_profile_context.controls.server_api = (smd_loopback_api_type)req->server_api;
      smd_profile_context.controls.fifo_size = 1024*(uint32)req->fifo_size_in_kb;
      smd_profile_context.controls.overhead = req->flags == 0 ? 
                                                SMDL_PROFILE_VERIFY_DATA : 
                                                SMDL_PROFILE_NO_OVERHEAD;
      smd_profile_context.controls.total_count = req->total_count;
      smd_profile_context.controls.packet_size = req->packet_size;
      smd_profile_context.controls.in_flight_limit = req->in_flight_limit;
  
      /* get the delayed response ID and ack the command  */
      rsp = (void *)diagpkt_subsys_alloc_v2(DIAG_SUBSYS_COREBSP, 
                                            SMDL_PROFILE_DIAG_CMD_CODE, 
                                            sizeof(diagpkt_subsys_header_v2_type));
      smd_profile_context.delayed_rsp_id = diagpkt_subsys_get_delayed_rsp_id(rsp);
      diagpkt_commit(rsp);

      /* Kick off work loop. */
      result = DALSYS_EventCtrl(smd_profile_context.hStartEvent, 
                                DALSYS_EVENT_CTRL_TRIGGER);
    
      if (DAL_SUCCESS == result)
        return NULL;
  
    } while (0);

    err_code = DIAG_BAD_CMD_F;
  }

  /* send error response */
  rsp = (void *)diagpkt_err_rsp( err_code, req_pkt, pkt_len );
  diagpkt_commit(rsp);

  return NULL;
}

/**
 * Registers SMD profiling command table with DIAG.
 *
 * @return None
 */
void smd_profile_diag_init(void)
{
    DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY_PROC(DIAG_MODEM_PROC, 
                                                  DIAG_SUBSYS_CMD_VER_2_F, 
                                                  DIAG_SUBSYS_COREBSP,
                                                  smd_profile_rsp_tbl);
}
